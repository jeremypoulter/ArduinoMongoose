#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>
#include <cstring>

#include "MongooseHttpServerRequestUpload.h"
#include "MongooseHttpServerEndpointUpload.h"

// ---------------------------------------------------------------------------
// Internal helper: find needle inside haystack.
// Returns a pointer to the first match, or nullptr if not found.
// ---------------------------------------------------------------------------
static const char *findBytes(const char *haystack, size_t haystack_len,
                              const char *needle,   size_t needle_len)
{
  if(needle_len == 0) return haystack;
  if(needle_len > haystack_len) return nullptr;
  size_t limit = haystack_len - needle_len;
  for(size_t i = 0; i <= limit; i++) {
    if(memcmp(haystack + i, needle, needle_len) == 0) {
      return haystack + i;
    }
  }
  return nullptr;
}

// ---------------------------------------------------------------------------
// Constructor – when the endpoint has an upload handler and the request is
// multipart/form-data, enable streaming by stripping the HTTP headers from
// the receive buffer.  Mongoose's http_cb detects the buffer length change
// after MG_EV_HTTP_HDRS and removes itself (sets c->pfn = NULL), so
// subsequent MG_EV_READ events go directly to our onReceive() handler.
// ---------------------------------------------------------------------------
MongooseHttpServerRequestUpload::MongooseHttpServerRequestUpload(
    mg_connection *nc,
    HttpRequestMethodComposite method,
    mg_http_message *msg,
    MongooseHttpServerEndpoint *endpoint) :
  MongooseHttpServerRequest(nc, method, msg, endpoint),
  index(0),
  _streamState(UPLOAD_STREAMING_INACTIVE),
  _boundary(),
  _partFilename(),
  _partName(),
  _partIndex(0)
{
  MongooseHttpServerEndpointUpload *uploadEndpoint =
      static_cast<MongooseHttpServerEndpointUpload *>(_endpoint);

  if(!uploadEndpoint->hasUploadHandler()) {
    return;
  }

  // Check Content-Type for multipart/form-data
  struct mg_str *ct = mg_http_get_header(msg, "Content-Type");
  if(!ct) return;

  // Must contain "multipart/form-data"
  static const char kMultipart[] = "multipart/form-data";
  if(!findBytes(ct->buf, ct->len, kMultipart, sizeof(kMultipart) - 1)) return;

  // Extract boundary parameter from Content-Type
  struct mg_str b = mg_http_get_header_var(*ct, mg_str("boundary"));
  if(!b.buf || b.len == 0) return;

  // Build the boundary delimiter string: "--<boundary>"
  _boundary = "--";
  _boundary.append(b.buf, b.len);

  // Initialise streaming state
  _streamState = UPLOAD_STREAMING_PREAMBLE;
  _partIndex   = 0;
  _partFilename.clear();
  _partName.clear();

  // -------------------------------------------------------------------
  // Key step: strip the HTTP headers from the receive buffer.
  // Mongoose's http_cb saves c->recv.len before firing MG_EV_HTTP_HDRS.
  // After the event returns it checks whether c->recv.len changed; if it
  // did, it sets c->pfn = NULL and returns, detaching itself so that all
  // future MG_EV_READ events reach our user handler directly.
  // -------------------------------------------------------------------
  size_t head_len = msg->head.len;
  mg_iobuf_del(&nc->recv, 0, head_len);

  // The duplicated _msg->body.buf was pointing into the receive buffer
  // that we just compacted; clear both fields to prevent any accidental
  // access (e.g. getParam() calling mg_http_get_var with a NULL buf).
  _msg->body.buf = nullptr;
  _msg->body.len = 0;

  // Call the request handler now (for auth checks, state initialisation).
  // If the handler rejects the request (sets _responseSent), stop here.
  uploadEndpoint->handleRequest(this);
  if(_responseSent) {
    _streamState = UPLOAD_STREAMING_COMPLETE;
    return;
  }

  // Process any body bytes that arrived together with the headers.
  processBodyData(nc);
}

// ---------------------------------------------------------------------------
// onReceive – called by MongooseSocket::processEvent for MG_EV_READ events.
// Delegates to the streaming multipart parser.
// ---------------------------------------------------------------------------
void MongooseHttpServerRequestUpload::onReceive(mg_connection *nc, long /*num_bytes*/)
{
  if(_streamState != UPLOAD_STREAMING_INACTIVE &&
     _streamState != UPLOAD_STREAMING_COMPLETE)
  {
    processBodyData(nc);
  }
}

// ---------------------------------------------------------------------------
// processBodyData – streaming multipart state machine.
// Consumes bytes from nc->recv and dispatches upload events to the handler.
// ---------------------------------------------------------------------------
void MongooseHttpServerRequestUpload::processBodyData(mg_connection *nc)
{
  MongooseHttpServerEndpointUpload *uploadEndpoint =
      static_cast<MongooseHttpServerEndpointUpload *>(_endpoint);

  while(nc->recv.len > 0 &&
        _streamState != UPLOAD_STREAMING_COMPLETE &&
        !_responseSent)
  {
    const char *buf = (const char *)nc->recv.buf;
    size_t      len = nc->recv.len;

    switch(_streamState)
    {
      // ------------------------------------------------------------------
      case UPLOAD_STREAMING_PREAMBLE:
      {
        // Look for the opening "--boundary\r\n" line.
        std::string delim = _boundary + "\r\n";
        const char *found = findBytes(buf, len, delim.c_str(), delim.size());
        if(!found) {
          // Not found yet; discard all but the last (delim.size()-1) bytes,
          // which might be the start of an incomplete boundary.
          size_t safe = (len > delim.size() - 1) ? len - (delim.size() - 1) : 0;
          if(safe > 0) mg_iobuf_del(&nc->recv, 0, safe);
          return;
        }
        // Consume everything up to and including the opening boundary + CRLF.
        size_t consumed = (size_t)(found - buf) + delim.size();
        mg_iobuf_del(&nc->recv, 0, consumed);
        _streamState = UPLOAD_STREAMING_PART_HEADERS;
        break;
      }

      // ------------------------------------------------------------------
      case UPLOAD_STREAMING_PART_HEADERS:
      {
        // Look for the blank line that ends the per-part headers.
        const char *found = findBytes(buf, len, "\r\n\r\n", 4);
        if(!found) {
          return;  // Wait for more data
        }
        size_t hdr_len = (size_t)(found - buf) + 4;

        // Parse Content-Disposition to extract filename/name attributes.
        _partFilename.clear();
        _partName.clear();

        static const char kCD[] = "Content-Disposition:";
        const char *cd = findBytes(buf, hdr_len, kCD, sizeof(kCD) - 1);
        if(cd)
        {
          const char *eol = findBytes(cd, hdr_len - (size_t)(cd - buf), "\r\n", 2);
          if(!eol) eol = buf + hdr_len;

          const char *cdval_start = cd + sizeof(kCD) - 1;
          while(cdval_start < eol && *cdval_start == ' ') cdval_start++;
          struct mg_str cdval = mg_str_n(cdval_start, (size_t)(eol - cdval_start));

          struct mg_str fname = mg_http_get_header_var(cdval, mg_str("filename"));
          if(fname.buf && fname.len > 0) {
            _partFilename.assign(fname.buf, fname.len);
          }
          struct mg_str name = mg_http_get_header_var(cdval, mg_str("name"));
          if(name.buf && name.len > 0) {
            _partName.assign(name.buf, name.len);
          }
        }

        _partIndex = 0;
        mg_iobuf_del(&nc->recv, 0, hdr_len);

        // Notify: a new part is beginning.
        uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_BEGIN,
                                   MongooseString(_partFilename.c_str()),
                                   _partIndex, nullptr, 0);

        _streamState = UPLOAD_STREAMING_PART_BODY;
        break;
      }

      // ------------------------------------------------------------------
      case UPLOAD_STREAMING_PART_BODY:
      {
        // The delimiter between parts (or at end) is: "\r\n--boundary"
        std::string delim = "\r\n" + _boundary;
        const char *found = findBytes(buf, len, delim.c_str(), delim.size());

        if(found)
        {
          // Dispatch all data before the delimiter.
          size_t data_len = (size_t)(found - buf);
          if(data_len > 0 && !_responseSent) {
            uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_DATA,
                                       MongooseString(_partFilename.c_str()),
                                       _partIndex,
                                       (uint8_t *)buf, data_len);
            _partIndex += data_len;
          }

          // Notify: the current part has ended.
          if(!_responseSent) {
            uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_END,
                                       MongooseString(_partFilename.c_str()),
                                       _partIndex, nullptr, 0);
          }

          // Consume data + delimiter from the buffer.
          size_t consumed = data_len + delim.size();
          mg_iobuf_del(&nc->recv, 0, consumed);

          // After the boundary, peek at the next two bytes:
          //   "--"   → closing boundary → all done
          //   "\r\n" → more parts follow → read next part headers
          buf = (const char *)nc->recv.buf;
          len = nc->recv.len;

          if(len >= 2 && buf[0] == '-' && buf[1] == '-') {
            // Closing boundary "--boundary--".  Consume the rest.
            mg_iobuf_del(&nc->recv, 0, len);
            _streamState = UPLOAD_STREAMING_COMPLETE;
            // The upload callback (at PART_END) is expected to have sent
            // the response already.  If it didn't, the connection will be
            // closed without a response, which is acceptable behaviour.
          } else {
            // More parts follow: skip the CRLF that follows the boundary.
            if(len >= 2) mg_iobuf_del(&nc->recv, 0, 2);
            _streamState = UPLOAD_STREAMING_PART_HEADERS;
            _partIndex    = 0;
            _partFilename.clear();
            _partName.clear();
          }
        }
        else
        {
          // Boundary not yet fully buffered.
          // Dispatch everything except the last (delim.size()-1) bytes,
          // which might overlap with an incoming boundary delimiter.
          size_t hold = delim.size() - 1;
          size_t safe = (len > hold) ? len - hold : 0;
          if(safe > 0 && !_responseSent) {
            uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_DATA,
                                       MongooseString(_partFilename.c_str()),
                                       _partIndex,
                                       (uint8_t *)buf, safe);
            _partIndex += safe;
            mg_iobuf_del(&nc->recv, 0, safe);
          } else {
            return;  // Need more data before we can make progress
          }
        }
        break;
      }

      default:
        return;
    }
  }
}

// ---------------------------------------------------------------------------
// handleMessage – fallback path for non-streaming uploads (i.e. when
// streaming was not enabled in the constructor) or when the full body has
// been received.  Uses mg_http_next_multipart() to iterate parts.
// ---------------------------------------------------------------------------
void MongooseHttpServerRequestUpload::handleMessage(mg_connection *nc, mg_http_message *msg)
{
  DBUGF("MongooseHttpServerRequestUpload::handleMessage");

#if MG_COPY_HTTP_MESSAGE
  _msg->body = msg->body;
#else
  _msg = msg;
#endif

  // Call onRequest handler first (auth checks, state initialisation, etc.)
  _endpoint->handleRequest(this);
  if(_responseSent) {
    return;
  }

  MongooseHttpServerEndpointUpload *uploadEndpoint =
      static_cast<MongooseHttpServerEndpointUpload *>(_endpoint);

  if(!uploadEndpoint->hasUploadHandler()) {
    return;
  }

  // Iterate over multipart parts using the Mongoose 7 API
  size_t ofs = 0;
  struct mg_http_part part;
  index = 0;

  while((ofs = mg_http_next_multipart(msg->body, ofs, &part)) > 0)
  {
    uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_BEGIN,
                               MongooseString(part.filename),
                               index, nullptr, 0);
    if(_responseSent) { break; }

    uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_DATA,
                               MongooseString(part.filename),
                               index, (uint8_t *)part.body.buf, part.body.len);
    index += part.body.len;
    if(_responseSent) { break; }

    uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_END,
                               MongooseString(part.filename),
                               index, nullptr, 0);
    if(_responseSent) { break; }
  }
}
