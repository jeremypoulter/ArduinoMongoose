#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>
#include <algorithm>

#include "MongooseHttpServerRequestUpload.h"
#include "MongooseHttpServerEndpointUpload.h"

// ---------------------------------------------------------------------------
// Constructor – during MG_EV_HTTP_HDRS, decide which upload path to use:
//
//  a) multipart/form-data  → do nothing here; Mongoose continues to buffer
//     the body.  handleMessage() is called from MG_EV_HTTP_MSG once the full
//     body has arrived, and it uses mg_http_next_multipart() to iterate parts.
//
//  b) any other Content-Type (e.g. application/octet-stream) → enable raw
//     body streaming using the same escape-hatch mechanism as Mongoose's own
//     mg_http_start_upload(): strip HTTP headers from nc->recv so that
//     http_cb detects the buffer change after MG_EV_HTTP_HDRS, sets
//     c->pfn = NULL, and exits.  All subsequent MG_EV_READ events then reach
//     our onReceive() override directly.
// ---------------------------------------------------------------------------
MongooseHttpServerRequestUpload::MongooseHttpServerRequestUpload(
    mg_connection *nc,
    HttpRequestMethodComposite method,
    mg_http_message *msg,
    MongooseHttpServerEndpoint *endpoint) :
  MongooseHttpServerRequest(nc, method, msg, endpoint),
  index(0),
  _streaming(false),
  _bodyExpected(0),
  _bodyReceived(0)
{
  MongooseHttpServerEndpointUpload *uploadEndpoint =
      static_cast<MongooseHttpServerEndpointUpload *>(_endpoint);

  if(!uploadEndpoint->hasUploadHandler()) {
    return;
  }

  // Detect multipart by looking for a "boundary" parameter in Content-Type.
  // If present, let Mongoose buffer the full body and use mg_http_next_multipart().
  struct mg_str *ct = mg_http_get_header(msg, "Content-Type");
  if(ct) {
    struct mg_str boundary = mg_http_get_header_var(*ct, mg_str("boundary"));
    if(boundary.buf != nullptr && boundary.len > 0) {
      return;  // multipart/form-data → non-streaming; handleMessage() will run
    }
  }

  // --- Raw body streaming path ---
  // body.len at MG_EV_HTTP_HDRS time reflects the Content-Length header value.
  _bodyExpected = msg->body.len;

  // Strip HTTP headers from the receive buffer (same technique used internally
  // by mg_http_start_upload).  http_cb records recv.len before dispatching
  // MG_EV_HTTP_HDRS; if it changes on return, http_cb sets c->pfn = NULL and
  // returns, so all subsequent MG_EV_READ events go directly to our handler.
  mg_iobuf_del(&nc->recv, 0, msg->head.len);

  _streaming    = true;
  _bodyReceived = 0;

  // Call the request handler now (auth checks, state initialisation).
  uploadEndpoint->handleRequest(this);
  if(_responseSent) return;

  // Notify: upload is starting (no filename for raw body uploads).
  uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_BEGIN,
                             MongooseString(""), 0, nullptr, 0);
  if(_responseSent) return;

  // Process any body bytes that arrived together with the headers.
  if(nc->recv.len > 0) {
    onReceive(nc, (long)nc->recv.len);
  }
}

// ---------------------------------------------------------------------------
// onReceive – called from MG_EV_READ events on the raw-body streaming path.
// Forwards body bytes to the upload handler and fires MG_EV_HTTP_PART_END
// once all expected bytes have been delivered.
// ---------------------------------------------------------------------------
void MongooseHttpServerRequestUpload::onReceive(mg_connection *nc, long /*num_bytes*/)
{
  if(!_streaming || _responseSent) return;

  MongooseHttpServerEndpointUpload *uploadEndpoint =
      static_cast<MongooseHttpServerEndpointUpload *>(_endpoint);

  while(nc->recv.len > 0 && !_responseSent) {
    size_t available = nc->recv.len;
    // Clamp to remaining expected bytes when Content-Length is known.
    size_t toProcess = (_bodyExpected > 0 && _bodyExpected > _bodyReceived)
                       ? std::min(available, _bodyExpected - _bodyReceived)
                       : available;
    if(toProcess == 0) break;

    uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_DATA,
                               MongooseString(""), _bodyReceived,
                               nc->recv.buf, toProcess);
    _bodyReceived += toProcess;
    mg_iobuf_del(&nc->recv, 0, toProcess);

    if(_bodyExpected > 0 && _bodyReceived >= _bodyExpected) {
      if(!_responseSent) {
        uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_END,
                                   MongooseString(""), _bodyReceived, nullptr, 0);
      }
      break;
    }
  }
}

// ---------------------------------------------------------------------------
// handleMessage – called from MG_EV_HTTP_MSG once the full multipart body has
// been buffered by Mongoose.  Uses mg_http_next_multipart() – the correct
// Mongoose 7 API for iterating multipart/form-data parts.
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

  // Iterate over multipart parts using the Mongoose 7 API.
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
