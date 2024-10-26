/*
#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>
#include <algorithm>

#include "MongooseCore.h"
#include "MongooseHttpServer.h"

MongooseHttpServer::MongooseHttpServer() :
  MongooseHttpServerConnection()
{

}

MongooseHttpServer::~MongooseHttpServer()
{

}

bool MongooseHttpServer::begin(uint16_t port)
{
  char addr[32];
  snprintf(addr, sizeof(addr), "http://0.0.0.0:%u", port);
  DBUGF("Starting HTTP server on %s", addr);
  if(mg_http_listen(Mongoose.getMgr(), addr, MongooseSocket::eventHandler, this))
  {
    return true;
  }

  return false;
}

bool MongooseHttpServer::begin(uint16_t port, const char *cert, const char *private_key)
{
  setCertificate(cert, private_key);
  if(begin(port)) {
    return true;
  }

  return false;
}

MongooseHttpServerEndpoint *MongooseHttpServer::on(const char* uri)
{
  return on(uri, HTTP_ANY);
}

MongooseHttpServerEndpoint *MongooseHttpServer::on(const char* uri, MongooseHttpRequestHandler onRequest)
{
  return on(uri, HTTP_ANY)->onRequest(onRequest);
}

MongooseHttpServerEndpoint *MongooseHttpServer::on(const char* uri, HttpRequestMethodComposite method, MongooseHttpRequestHandler onRequest)
{
  return on(uri, method)->onRequest(onRequest);
}

MongooseHttpServerEndpoint *MongooseHttpServer::on(const char* uri, HttpRequestMethodComposite method)
{
  MongooseHttpServerEndpoint *handler = new MongooseHttpServerEndpoint(method);
  if(handler) {
    handler->attach(getConnection(), uri, method);
    return handler;
  }

  return nullptr;
}

void MongooseHttpServer::onNotFound(MongooseHttpRequestHandler fn)
{
  onRequest(fn);
}


#if MG_ENABLE_HTTP_WEBSOCKET
void MongooseHttpServer::sendAll(MongooseHttpWebSocketConnection *from, const char *endpoint, int op, const void *data, size_t len)
{
  mg_mgr *mgr = Mongoose.getMgr();

  const struct mg_connection *nc = from ? from->getConnection() : nullptr;
  struct mg_connection *c;
  for (c = mg_next(mgr, nullptr); c != nullptr; c = mg_next(mgr, c)) {
    if (c == nc) { 
      continue; // Don't send to the sender.
    }
    if (c->flags & MG_F_IS_WEBSOCKET && c->flags & MG_F_IS_MongooseHttpWebSocketConnection)
    {
      MongooseHttpWebSocketConnection *to = (MongooseHttpWebSocketConnection *)c->user_connection_data;
      if(endpoint && !to->uri().equals(endpoint)) {
        continue;
      }
      DBUGF("%.*s sending to %p", (int)to->uri().length(), to->uri().c_str(), to);
      to->send(op, data, len);
    }
  }
}
#endif

/// MongooseHttpServerRequest object

MongooseHttpServerRequest::MongooseHttpServerRequest(mg_connection *nc, mg_http_message *msg) :
  _nc(nc),
#if MG_COPY_HTTP_MESSAGE
  _msg(duplicateMessage(msg)),
#else
  _msg(msg),
#endif
  _response(nullptr),
  _responseSent(false)
{
  if(0 == mg_strcasecmp(msg->method, mg_str_s("GET"))) {
    _method = HTTP_GET;
  } else if(0 == mg_strcasecmp(msg->method, mg_str_s("POST"))) {
    _method = HTTP_POST;
  } else if(0 == mg_strcasecmp(msg->method, mg_str_s("DELETE"))) {
    _method = HTTP_DELETE;
  } else if(0 == mg_strcasecmp(msg->method, mg_str_s("PUT"))) {
    _method = HTTP_PUT;
  } else if(0 == mg_strcasecmp(msg->method, mg_str_s("PATCH"))) {
    _method = HTTP_PATCH;
  } else if(0 == mg_strcasecmp(msg->method, mg_str_s("HEAD"))) {
    _method = HTTP_HEAD;
  } else if(0 == mg_strcasecmp(msg->method, mg_str_s("OPTIONS"))) {
    _method = HTTP_OPTIONS;
  }

  nc->user_connection_data = this;
}

MongooseHttpServerRequest::~MongooseHttpServerRequest()
{
  if(_response) {
    delete _response;
    _response = nullptr;
  }

#if MG_COPY_HTTP_MESSAGE
  mg_strfree(&_msg->message);
  delete _msg;
  _msg = nullptr;
#endif

  _nc->user_connection_data = nullptr;
}

#if MG_COPY_HTTP_MESSAGE

mg_str mg_mk_str_from_offsets(mg_str &dest, mg_str &src, mg_str &value) {
  mg_str s;

  s.p = value.p ? (dest.p + (value.p - src.p)) : nullptr;
  s.len = value.len;

  return s;
}

mg_http_message *MongooseHttpServerRequest::duplicateMessage(mg_http_message *sm)
{
  mg_http_message *nm = new mg_http_message();
  memset(nm, 0, sizeof(*nm));

  mg_str headers = mg_mk_str_n(sm->message.p, sm->message.len - sm->body.len);

  nm->message = mg_strdup_nul(headers);
  nm->body = sm->body;

  nm->method = mg_mk_str_from_offsets(nm->message, sm->message, sm->method);
  nm->uri = mg_mk_str_from_offsets(nm->message, sm->message, sm->uri);
  nm->proto = mg_mk_str_from_offsets(nm->message, sm->message, sm->proto);

  nm->resp_code = sm->resp_code;
  nm->resp_status_msg = mg_mk_str_from_offsets(nm->message, sm->message, sm->resp_status_msg);

  nm->query_string = mg_mk_str_from_offsets(nm->message, sm->message, sm->query_string);

  for(int i = 0; i < MG_MAX_HTTP_HEADERS; i++)
  {
    nm->header_names[i] = mg_mk_str_from_offsets(nm->message, sm->message, sm->header_names[i]);
    nm->header_values[i] = mg_mk_str_from_offsets(nm->message, sm->message, sm->header_values[i]);
  }
  
  return nm;
}
#endif

void MongooseHttpServerRequest::redirect(const char *url)
{

}

#ifdef ARDUINO
void MongooseHttpServerRequest::redirect(const String& url)
{
  redirect(url.c_str());
}
#endif

MongooseHttpServerResponseBasic *MongooseHttpServerRequest::beginResponse()
{
  return new MongooseHttpServerResponseBasic();
}

#ifdef ARDUINO

MongooseHttpServerResponseStream *MongooseHttpServerRequest::beginResponseStream()
{
  return new MongooseHttpServerResponseStream();
}

#endif

void MongooseHttpServerRequest::send(MongooseHttpServerResponse *response)
{
  if(_response) {
    delete _response;
    _response = nullptr;
  }

  response->sendHeaders(_nc);
  _response = response;
  sendBody();
  _responseSent = true;
}

void MongooseHttpServerRequest::sendBody()
{
  if(_response) 
  {
    size_t free = _nc->send_mbuf.size - _nc->send_mbuf.len;
    if(0 == _nc->send_mbuf.size) {
      free = ARDUINO_MONGOOSE_SEND_BUFFER_SIZE;
    }
    if(free > 0) 
    {
      size_t sent = _response->sendBody(_nc, free);
      DBUGF("Connection %p: sent %zu/%zu, %lx", _nc, sent, free, _nc->flags & MG_F_SEND_AND_CLOSE);

      if(sent < free) {
        DBUGLN("Response finished");
        delete _response;
        _response = nullptr;
      }
    }
  }
}

extern "C" const char *mg_status_message(int status_code);

void MongooseHttpServerRequest::send(int code)
{
  send(code, "text/plain", mg_status_message(code));
}

void MongooseHttpServerRequest::send(int code, const char *contentType, const char *content)
{
  char headers[64], *pheaders = headers;
  mg_asprintf(&pheaders, sizeof(headers), 
      "Connection: close\r\n"
      "Content-Type: %s",
      contentType);

  mg_send_head(_nc, code, strlen(content), pheaders);
  mg_send(_nc, content, strlen(content));
  _nc->flags |= MG_F_SEND_AND_CLOSE;

  if (pheaders != headers) free(pheaders);

  _responseSent = true;
}

#ifdef ARDUINO
void MongooseHttpServerRequest::send(int code, const String& contentType, const String& content)
{
  send(code, contentType.c_str(), content.c_str());
}
#endif

// IMPROVE: add a function to Mongoose to do this
bool MongooseHttpServerRequest::hasParam(const char *name) const
{
  char dst[8];
  int ret = getParam(name, dst, sizeof(dst));
  return ret >= 0 || -3 == ret; 
}

#ifdef ARDUINO
bool MongooseHttpServerRequest::hasParam(const String& name) const
{
  char dst[8];
  int ret = getParam(name, dst, sizeof(dst));
  return ret >= 0 || -3 == ret; 
}
#endif

#ifdef ARDUINO
bool MongooseHttpServerRequest::hasParam(const __FlashStringHelper * data) const
{
  char dst[8];
  int ret = getParam(data, dst, sizeof(dst));
  return ret >= 0 || -3 == ret; 
}
#endif

int MongooseHttpServerRequest::getParam(const char *name, char *dst, size_t dst_len) const
{
  return mg_get_http_var((HTTP_GET == _method) ? (&_msg->query_string) : (&_msg->body), name, dst, dst_len);
}

#ifdef ARDUINO
int MongooseHttpServerRequest::getParam(const String& name, char *dst, size_t dst_len) const
{
  return getParam(name.c_str(), dst, dst_len);
}
#endif

#ifdef ARDUINO
int MongooseHttpServerRequest::getParam(const __FlashStringHelper * data, char *dst, size_t dst_len) const
{
  PGM_P p = reinterpret_cast<PGM_P>(data);
  size_t n = strlen_P(p);
  char * name = (char*) malloc(n+1);
  if (name) {
    strcpy_P(name, p);   
    int result = getParam(name, dst, dst_len); 
    free(name); 
    return result; 
  } 
  
  return -5; 
}
#endif

#ifdef ARDUINO
String MongooseHttpServerRequest::getParam(const char *name) const
{
  String ret = "";
  char *tempString = new char[ARDUINO_MONGOOSE_PARAM_BUFFER_LENGTH];
  if(tempString)
  {
    if(getParam(name, tempString, ARDUINO_MONGOOSE_PARAM_BUFFER_LENGTH) > 0) {
      ret.concat(tempString);
    }

    delete tempString;
  }
  return ret;
}
#endif

#ifdef ARDUINO
String MongooseHttpServerRequest::getParam(const String& name) const
{
  String ret = "";
  char *tempString = new char[ARDUINO_MONGOOSE_PARAM_BUFFER_LENGTH];
  if(tempString)
  {
    if(getParam(name, tempString, ARDUINO_MONGOOSE_PARAM_BUFFER_LENGTH) > 0) {
      ret.concat(tempString);
    }

    delete tempString;
  }
  return ret;
}
#endif

#ifdef ARDUINO
String MongooseHttpServerRequest::getParam(const __FlashStringHelper * data) const
{
  String ret = "";
  char *tempString = new char[ARDUINO_MONGOOSE_PARAM_BUFFER_LENGTH];
  if(tempString)
  {
    if(getParam(data, tempString, ARDUINO_MONGOOSE_PARAM_BUFFER_LENGTH) > 0) {
      ret.concat(tempString);
    }

    delete tempString;
  }
  return ret;
}
#endif

bool MongooseHttpServerRequest::authenticate(const char * username, const char * password)
{
  DBUGVAR(username);
  DBUGVAR(password);

  char user_buf[64];
  char pass_buf[64];

  if(0 == mg_get_http_basic_auth(_msg, user_buf, sizeof(user_buf),
                           pass_buf, sizeof(pass_buf)))
  {
    DBUGVAR(user_buf);
    DBUGVAR(pass_buf);

    if(0 == strcmp(username, user_buf) && 0 == strcmp(password, pass_buf))
    {
      return true;
    }
  }

  return false;
}

void MongooseHttpServerRequest::requestAuthentication(const char* realm)
{
  // https://github.com/me-no-dev/ESPAsyncWebServer/blob/master/src/WebRequest.cpp#L852
  // mg_http_send_digest_auth_request

  char headers[64], *pheaders = headers;
  mg_asprintf(&pheaders, sizeof(headers), 
      "WWW-Authenticate: Basic realm=%s",
      realm);

  mg_send_head(_nc, 401, 0, pheaders);
  _nc->flags |= MG_F_SEND_AND_CLOSE;

  if (pheaders != headers) free(pheaders);

  _responseSent = true;
}

MongooseHttpServerResponse::MongooseHttpServerResponse() :
  _code(200),
  _contentType(nullptr),
  _contentLength(-1),
  _headerBuffer(nullptr)
{

}

MongooseHttpServerResponse::~MongooseHttpServerResponse()
{
  if(_contentType) {
    free(_contentType);
    _contentType = nullptr;
  }

  if(_headerBuffer) {
    free(_headerBuffer);
    _headerBuffer = nullptr;
  }
}
void MongooseHttpServerResponse::sendHeaders(struct mg_connection *nc)
{
  char headers[64], *pheaders = headers;
  mg_asprintf(&pheaders, sizeof(headers), 
      "Connection: close\r\n"
      "Content-Type: %s%s",
      _contentType ? _contentType : "text/plain", _headerBuffer ? _headerBuffer : "");

  mg_send_head(nc, _code, _contentLength, pheaders);

  if(_headerBuffer) {
    free(_headerBuffer);
    _headerBuffer = NULL;
  }
  if(pheaders != headers) {
    free(pheaders);
  }
}

bool MongooseHttpServerResponse::addHeader(const char *name, const char *value)
{
  size_t startLen = _headerBuffer ? strlen(_headerBuffer) : 0;
  size_t newLen = sizeof(": \r\n");
  newLen += strlen(name);
  newLen += strlen(value);
  size_t len = startLen + newLen;

  char * newBuffer = (char *)realloc(_headerBuffer, len);
  if(newBuffer)
  {
    snprintf(newBuffer + startLen, newLen, "\r\n%s: %s", name, value);
    _headerBuffer = newBuffer;
    return true;
  }

  return false;
}

#ifdef ARDUINO
bool MongooseHttpServerResponse::addHeader(const String& name, const String& value)
{
  return addHeader(name.c_str(), value.c_str());
}
#endif

void MongooseHttpServerResponse::setContentType(const char *contentType)
{
  size_t len = strlen(contentType);
  char *newPtr = (char *)realloc(_contentType, len + 1);
  if(newPtr) {
    strcpy(newPtr, contentType);
    _contentType = newPtr;
  }
}

#ifdef ARDUINO
void MongooseHttpServerResponse::setContentType(const __FlashStringHelper *contentType)
{
  size_t len = strlen_P((PGM_P)contentType);
  char *newPtr = (char *)realloc(_contentType, len + 1);
  if(newPtr) {
    strcpy_P(newPtr, (PGM_P)contentType);
    _contentType = newPtr;
  }
}
#endif



MongooseHttpServerResponseBasic::MongooseHttpServerResponseBasic() :
  ptr(NULL), len(0)
{

}

void MongooseHttpServerResponseBasic::setContent(const char *content)
{
  setContent((uint8_t *)content, strlen(content));
}

void MongooseHttpServerResponseBasic::setContent(const uint8_t *content, size_t len)
{
  this->ptr = content;
  this->len = len;
  setContentLength(this->len);
}

size_t MongooseHttpServerResponseBasic::sendBody(struct mg_connection *nc, size_t bytes)
{
  size_t send = std::min(len, bytes);

  mg_send(nc, ptr, send);

  ptr += send;
  len -= send;

  if(0 == len) {
    nc->flags |= MG_F_SEND_AND_CLOSE;
  }

  return send;
}

#ifdef ARDUINO
MongooseHttpServerResponseStream::MongooseHttpServerResponseStream()
{
  mbuf_init(&_content, ARDUINO_MONGOOSE_DEFAULT_STREAM_BUFFER);
}

MongooseHttpServerResponseStream::~MongooseHttpServerResponseStream()
{
  mbuf_free(&_content);
}

size_t MongooseHttpServerResponseStream::write(const uint8_t *data, size_t len)
{
  size_t written = mbuf_append(&_content, data, len);
  setContentLength(_content.len);
  return written;
}

size_t MongooseHttpServerResponseStream::write(uint8_t data)
{
  return write(&data, 1);
}

size_t MongooseHttpServerResponseStream::sendBody(struct mg_connection *nc, size_t bytes)
{
  size_t send = std::min(_content.len, bytes);

  mg_send(nc, _content.buf, send);

  mbuf_remove(&_content, send);

  if(0 == _content.len) {
    nc->flags |= MG_F_SEND_AND_CLOSE;
  }

  return send;
}

#endif

#if MG_ENABLE_HTTP_WEBSOCKET
MongooseHttpWebSocketConnection::MongooseHttpWebSocketConnection(mg_connection *nc, mg_http_message *msg) :
  MongooseHttpServerRequest(nc, msg)
{
  nc->flags |= MG_F_IS_MongooseHttpWebSocketConnection;
}

MongooseHttpWebSocketConnection::~MongooseHttpWebSocketConnection()
{

}

void MongooseHttpWebSocketConnection::send(int op, const void *data, size_t len)
{
  mg_send_websocket_frame(_nc, op, data, len);
}

#endif
*/
