#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_CLIENT)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>

#include <stdlib.h>  // malloc, free
#include <string.h>  // memcpy, strlen

#include "MongooseCore.h"
#include "MongooseHttpClient.h"

// mongoose.h #defines strdup(s) to mg_strdup(), which allocates through
// mg_calloc() -- a custom allocator on builds that provide one (FreeRTOS and
// friends). Pairing that with plain free(), as this file does for every other
// buffer it owns, is an allocator mismatch that only shows up on those builds.
// Duplicate with plain malloc()/memcpy() instead so the pairing is always the
// same free() already used below.
static char *dupString(const char *s)
{
  if(nullptr == s) {
    return nullptr;
  }
  size_t len = strlen(s) + 1;
  char *copy = (char *)malloc(len);
  if(copy) {
    memcpy(copy, s, len);
  }
  return copy;
}

MongooseHttpClient::MongooseHttpClient()
{

}

MongooseHttpClient::~MongooseHttpClient()
{

}

bool MongooseHttpClient::get(const char *uri, MongooseHttpResponseHandler onResponse, MongooseSocketCloseHandler onClose)
{
  MongooseHttpClientRequest *request = beginRequest(uri);
  request->setMethod(HTTP_GET);
  if(nullptr != onResponse) {
    request->onResponse(onResponse);
  }
  if(nullptr != onClose) {
    request->onClose(onClose);
  }
  // A successful send() self-deletes from onClose() once mongoose is done with
  // it. A failed send() never connects, so onClose() never fires and nothing
  // else references this request -- this convenience wrapper handed back only
  // a bool, so it must clean up itself rather than leaking on every failure.
  if(request->send()) {
    return true;
  }
  delete request;
  return false;
}

bool MongooseHttpClient::post(const char* uri, const char *contentType, const char *body, MongooseHttpResponseHandler onResponse, MongooseSocketCloseHandler onClose)
{
  MongooseHttpClientRequest *request = beginRequest(uri);
  request->setMethod(HTTP_POST);
  request->setContentType(contentType);
  request->setContent(body);
  if(nullptr != onResponse) {
    request->onResponse(onResponse);
  }
  if(nullptr != onClose) {
    request->onClose(onClose);
  }
  if(request->send()) {
    return true;
  }
  delete request;
  return false;
}

bool MongooseHttpClient::put(const char* uri, const char *contentType, const char *body, MongooseHttpResponseHandler onResponse, MongooseSocketCloseHandler onClose)
{
  MongooseHttpClientRequest *request = beginRequest(uri);
  request->setMethod(HTTP_PUT);
  request->setContentType(contentType);
  request->setContent(body);
  if(nullptr != onResponse) {
    request->onResponse(onResponse);
  }
  if(nullptr != onClose) {
    request->onClose(onClose);
  }
  if(request->send()) {
    return true;
  }
  delete request;
  return false;
}

bool MongooseHttpClient::patch(const char* uri, const char *contentType, const char *body, MongooseHttpResponseHandler onResponse, MongooseSocketCloseHandler onClose)
{
  MongooseHttpClientRequest *request = beginRequest(uri);
  request->setMethod(HTTP_PATCH);
  request->setContentType(contentType);
  request->setContent(body);
  if(nullptr != onResponse) {
    request->onResponse(onResponse);
  }
  if(nullptr != onClose) {
    request->onClose(onClose);
  }
  if(request->send()) {
    return true;
  }
  delete request;
  return false;
}

bool MongooseHttpClient::delete_(const char* uri, MongooseHttpResponseHandler onResponse, MongooseSocketCloseHandler onClose)
{
  MongooseHttpClientRequest *request = beginRequest(uri);
  request->setMethod(HTTP_DELETE);
  if(nullptr != onResponse) {
    request->onResponse(onResponse);
  }
  if(nullptr != onClose) {
    request->onClose(onClose);
  }
  if(request->send()) {
    return true;
  }
  delete request;
  return false;
}

MongooseHttpClientRequest *MongooseHttpClient::beginRequest(const char *uri)
{
  return new MongooseHttpClientRequest(uri);
}

MongooseHttpClientRequest::MongooseHttpClientRequest(const char *uri) :
  MongooseSocket(),
  _onResponse(nullptr),
  _onBody(nullptr),
  _uri(uri ? dupString(uri) : nullptr),
  _method(HTTP_GET),
  _contentType(nullptr),
  _contentLength(-1),
  _body(nullptr),
  _extraHeaders(nullptr)
{

}

MongooseHttpClientRequest::~MongooseHttpClientRequest()
{
  free(_uri);
  _uri = nullptr;
  free(_contentType);
  _contentType = nullptr;
  free(_body);
  _body = nullptr;
  free(_extraHeaders);
  _extraHeaders = nullptr;
}

void MongooseHttpClientRequest::handleEvent(mg_connection *nc, int ev, void *p)
{
  switch (ev)
  {
    case MG_EV_HTTP_MSG:
    {
      char addr[32];
      struct mg_http_message *hm = (struct mg_http_message *) p;
      mg_snprintf(addr, sizeof(addr), "%M", mg_print_ip_port, &nc->rem);
      DBUGF("HTTP message from %s, body %zu @ %p",
        addr, hm->body.len, hm->body.buf);

      MongooseHttpClientResponse response(hm);
      if(_onBody) {
        _onBody(&response);
      }
      if(_onResponse) {
        _onResponse(&response);
      }

      break;
    }
  }
}

void MongooseHttpClientRequest::onOpen(mg_connection *nc)
{
  _timeout_ms = mg_millis() + MOGOOSE_HTTP_CLIENT_TIMEOUT;
}

void MongooseHttpClientRequest::onPoll(mg_connection *nc)
{
  if (mg_millis() > _timeout_ms &&
      (nc->is_connecting || nc->is_resolving)) {
    mg_error(nc, "Connect timeout");
  }
}

void MongooseHttpClientRequest::onConnect(mg_connection *nc)
{
  MongooseSocket::onConnect(nc);

  struct mg_str host = mg_url_host(_uri);
  // Send request
  mg_printf(nc,
            "%s %s HTTP/1.1\r\n"
            "Host: %.*s\r\n"
            "Connection: close\r\n"
            "Content-Length: %lld\r\n"
            "%s%s%s"
            "%s"
            "\r\n",
            HTTP_POST == _method ? "POST" : 
            HTTP_DELETE == _method ? "DELETE" : 
            HTTP_PUT == _method ? "PUT" : 
            HTTP_PATCH == _method ? "PATCH" : 
            HTTP_HEAD == _method ? "HEAD" : 
            HTTP_OPTIONS == _method ? "OPTIONS" : "GET", 
            mg_url_uri(_uri), (int) host.len,
            host.buf, _contentLength > 0 ? _contentLength : 0,
            _contentType ? "Content-Type: " : "", 
            _contentType ? _contentType : "",
            _contentType ? "\r\n" : "",
            _extraHeaders ? _extraHeaders : "");
  if(_body) {
    mg_send(nc, _body, _contentLength);
  }
}

void MongooseHttpClientRequest::onClose(mg_connection *nc)
{
  MongooseSocket::onClose(nc);
  delete this;
}

bool MongooseHttpClientRequest::send()
{
  if(nullptr == _uri) {
    DBUGF("No URI, was the request allocated with one?");
    return false;
  }

  if(mg_url_is_ssl(_uri)) {
    setSecure(mg_url_host(_uri));
  }

  if(MongooseSocket::connect(
    mg_http_connect(Mongoose.getMgr(), _uri, MongooseSocket::eventHandler, this)))
  {
    return true;
  } else {
    DBUGF("Failed to connect to %s", _uri);
  }

  return false;
}

MongooseHttpClientRequest *MongooseHttpClientRequest::setContentType(const char *contentType)
{
  free(_contentType);
  _contentType = contentType ? dupString(contentType) : nullptr;
  return this;
}

MongooseHttpClientRequest *MongooseHttpClientRequest::setContent(const uint8_t *content, size_t len)
{
  if(nullptr == content || 0 == len) {
    free(_body);
    _body = nullptr;
    setContentLength(0);
    return this;
  }

  // Allocate before touching any existing state. Freeing the old body and
  // resetting the length first (as this used to) meant a failed allocation
  // silently downgraded the request to an empty body instead of the intended
  // payload -- a caller replacing an already-set body with a bigger one would
  // lose the good copy it had, with only a debug log to say why. Leave the
  // previous body/length untouched on failure instead.
  //
  // +1 and NUL-terminated: callers pass text bodies here via the const char *
  // overload and it costs one byte to keep that safe to print while debugging.
  uint8_t *newBody = (uint8_t *)malloc(len + 1);
  if(nullptr == newBody) {
    DBUGF("Failed to allocate %u bytes for the request body", (unsigned)len);
    return this;
  }

  memcpy(newBody, content, len);
  newBody[len] = '\0';

  free(_body);
  _body = newBody;
  setContentLength(len);
  return this;
}

bool MongooseHttpClientRequest::addHeader(const char *name, size_t nameLength, const char *value, size_t valueLength)
{
  size_t startLen = _extraHeaders ? strlen(_extraHeaders) : 0;
  size_t newLen = sizeof(": \r\n");
  newLen += nameLength;
  newLen += valueLength;
  size_t len = startLen + newLen;

  char * newBuffer = (char *)realloc(_extraHeaders, len);
  if(newBuffer)
  {
    snprintf(newBuffer + startLen, newLen, "%.*s: %.*s\r\n", (int)nameLength, name, (int)valueLength, value);
    _extraHeaders = newBuffer;
    return true;
  }

  return false;
}

size_t MongooseHttpClientResponse::contentLength() {
  MongooseString content_length = headers("Content-Length");
  if(content_length != nullptr) {
    return atoll(content_length.c_str());
  }
  return _msg->body.len;
}
