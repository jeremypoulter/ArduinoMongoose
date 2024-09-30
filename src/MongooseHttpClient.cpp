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
  if(NULL != onResponse) {
    request->onResponse(onResponse);
  }
  if(NULL != onClose) {
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
  if(NULL != onResponse) {
    request->onResponse(onResponse);
  }
  if(NULL != onClose) {
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
  _onResponse(NULL),
  _onBody(NULL),
  _uri(uri),
  _method(HTTP_GET),
  _contentType("application/x-www-form-urlencoded"),
  _contentLength(-1),
  _body(NULL),
  _extraHeaders(NULL)
{

}

MongooseHttpClientRequest::~MongooseHttpClientRequest()
{
  if (_extraHeaders) {
    free(_extraHeaders);
    _extraHeaders = NULL;
  }
}

void MongooseHttpClientRequest::onEvent(mg_connection *nc, int ev, void *p)
{
  switch (ev)
  {
    case MG_EV_HTTP_CHUNK:
    case MG_EV_HTTP_REPLY:
    {
      char addr[32];
      struct http_message *hm = (struct http_message *) p;
      mg_sock_addr_to_str(&getConnection()->sa, addr, sizeof(addr),
                          MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
      DBUGF("HTTP %s from %s, body %zu @ %p",
        MG_EV_HTTP_REPLY == ev ? "reply" : "chunk",
        addr, hm->body.len, hm->body.p);

      MongooseHttpClientResponse response(hm);
      if(MG_EV_HTTP_CHUNK == ev)
      {
        if(_onBody) {
          _onBody(&response);
          setFlags(MG_F_DELETE_CHUNK);
        }
      } else {
        if(_onResponse) {
          _onResponse(&response);
        }
        abort();
      }

      break;
    }
  }
}

void MongooseHttpClientRequest::onClose(mg_connection *nc)
{
  MongooseSocket::onClose(nc);
  delete this;
}

bool MongooseHttpClientRequest::send()
{
  struct mg_connect_opts opts;
  Mongoose.getDefaultOpts(&opts);

  const char *err;
  opts.error_string = &err;

  if(MongooseSocket::connect(
    mg_connect_http_opt(Mongoose.getMgr(), eventHandler, this, opts, _uri, _extraHeaders, (const char *)_body)))
  {
    return true;
  } else {
    DBUGF("Failed to connect to %s: %s", _uri, err);
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
  if(content_length != NULL) {
    return atoll(content_length.c_str());
  }
  return _msg->body.len;
}
