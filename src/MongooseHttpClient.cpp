#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_CLIENT)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#else
#define utoa(i, buf, base) sprintf(buf, "%u", i)
#endif

#include <MicroDebug.h>

#include "MongooseCore.h"
#include "MongooseHttpClient.h"

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
  return request->send();
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
  return request->send();
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
  return request->send();
}

MongooseHttpClientRequest *MongooseHttpClient::beginRequest(const char *uri)
{
  return new MongooseHttpClientRequest(uri);
}

MongooseHttpClientRequest::MongooseHttpClientRequest(const char *uri) :
  MongooseSocket(),
  _onResponse(nullptr),
  _onBody(nullptr),
  _uri(uri),
  _method(HTTP_GET),
  _contentType(nullptr),
  _contentLength(-1),
  _body(nullptr),
  _extraHeaders(nullptr)
{

}

MongooseHttpClientRequest::~MongooseHttpClientRequest()
{
  if (_extraHeaders) {
    free(_extraHeaders);
    _extraHeaders = nullptr;
  }
}

void MongooseHttpClientRequest::onEvent(mg_connection *nc, int ev, void *p)
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

MongooseHttpClientRequest *MongooseHttpClientRequest::setContent(const uint8_t *content, size_t len)
{
  setContentLength(len);
  _body = content;
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
