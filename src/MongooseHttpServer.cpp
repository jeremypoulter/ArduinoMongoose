#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#else
#define utoa(i, buf, base) sprintf(buf, "%u", i)
#endif

#include <MicroDebug.h>
#include <algorithm>

#include "MongooseCore.h"
#include "MongooseHttpServer.h"

MongooseHttpServer::MongooseHttpServer() :
  nc(NULL),
  defaultEndpoint(this, HTTP_ANY),
  _keepAliveEnabled(true)
{

}

bool MongooseHttpServer::allowKeepAlive()
{
  if(!_keepAliveEnabled) {
    return false;
  }

  // Count every connection on the manager, not just ours: HTTP clients, MQTT,
  // etc all share the same (small) network stack pool.
  int connections = 0;
  mg_mgr *mgr = Mongoose.getMgr();
  for(mg_connection *c = mg_next(mgr, NULL); c != NULL; c = mg_next(mgr, c)) {
    connections++;
  }

  return connections <= ARDUINO_MONGOOSE_KEEPALIVE_MAX_CONNECTIONS;
}

MongooseHttpServer::~MongooseHttpServer()
{

}

bool MongooseHttpServer::begin(uint16_t port)
{
  char s_http_port[6];
  utoa(port, s_http_port, 10);
  nc = mg_bind(Mongoose.getMgr(), s_http_port, defaultEventHandler, this);
  if(nc)
  {
    // Set up HTTP server parameters
    mg_set_protocol_http_websocket(nc);

    return true;
  }

  return false;
}

#if MG_ENABLE_SSL
bool MongooseHttpServer::begin(uint16_t port, const char *cert, const char *private_key)
{
  char s_http_port[6];
  utoa(port, s_http_port, 10);

  struct mg_bind_opts bind_opts;
  const char *err;

  memset(&bind_opts, 0, sizeof(bind_opts));
  bind_opts.ssl_cert = cert;
  bind_opts.ssl_key = private_key;
  bind_opts.error_string = &err;

  nc = mg_bind_opt(Mongoose.getMgr(), s_http_port, defaultEventHandler, this, bind_opts);
  if(nc)
  {
    // Set up HTTP server parameters
    mg_set_protocol_http_websocket(nc);

    return true;
  }

  return false;
}
#endif

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
  MongooseHttpServerEndpoint *handler = new MongooseHttpServerEndpoint(this, method);

  mg_register_http_endpoint(nc, uri, endpointEventHandler, handler);

  return handler;
}

void MongooseHttpServer::onNotFound(MongooseHttpRequestHandler fn)
{
  defaultEndpoint.onRequest(fn);
}

void MongooseHttpServer::defaultEventHandler(struct mg_connection *nc, int ev, void *p, void *u)
{
  MongooseHttpServer *self = (MongooseHttpServer *)u;
  //if (ev != MG_EV_POLL) { DBUGF("defaultEventHandler: nc = %p, self = %p", nc, self); }
  self->eventHandler(nc, ev, p, HTTP_ANY, &(self->defaultEndpoint));
}

void MongooseHttpServer::endpointEventHandler(struct mg_connection *nc, int ev, void *p, void *u)
{
  MongooseHttpServerEndpoint *self = (MongooseHttpServerEndpoint *)u;
  //if (ev != MG_EV_POLL) { DBUGF("endpointEventHandler: nc = %p,  self = %p,  self->server = %p", nc, self, self->server); }
  self->server->eventHandler(nc, ev, p, self->method, self);
}

// Free a keep-alive request whose response has fully completed, leaving the
// connection open for the next request. Close-mode requests stay attached
// until MG_EV_CLOSE so their endpoint close callback keeps firing.
static void checkRequestDone(struct mg_connection *nc)
{
  if(nc->user_connection_data)
  {
    MongooseHttpServerRequest *request = (MongooseHttpServerRequest *)nc->user_connection_data;
    if(request->keepAlive() && request->completed() && !request->responseSent()) {
      DBUGF("Connection %p: request complete, keeping alive", nc);
      delete request;
      nc->user_connection_data = NULL;
    }
  }
}

void MongooseHttpServer::eventHandler(struct mg_connection *nc, int ev, void *p, HttpRequestMethodComposite method, MongooseHttpServerEndpoint *endpoint)
{
  if (ev != MG_EV_POLL) { DBUGF("%s %p: %d", __PRETTY_FUNCTION__, nc, ev); }

  switch (ev) {
    case MG_EV_ACCEPT: {
      char addr[32];
      mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                          MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
      DBUGF("Connection %p from %s", nc, addr);
      break;
    }
#if MG_ENABLE_HTTP_STREAMING_MULTIPART
    case MG_EV_HTTP_MULTIPART_REQUEST:
#endif
#if MG_ENABLE_HTTP_WEBSOCKET
    case MG_EV_WEBSOCKET_HANDSHAKE_REQUEST:
#endif
    case MG_EV_HTTP_REQUEST: {
      char addr[32];
      struct http_message *hm = (struct http_message *) p;
      mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                          MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
      DBUGF("HTTP request from %s: %.*s %.*s", addr, (int) hm->method.len,
             hm->method.p, (int) hm->uri.len, hm->uri.p);

      // A new request on this connection: cancel any pending keep-alive idle
      // timer and free the previous request (normally already cleaned up).
      mg_set_timer(nc, 0);
      if(nc->user_connection_data) {
        delete (MongooseHttpServerRequest *)nc->user_connection_data;
        nc->user_connection_data = NULL;
      }
      MongooseHttpServerRequest *request =
#if MG_ENABLE_HTTP_STREAMING_MULTIPART
        MG_EV_HTTP_MULTIPART_REQUEST == ev ? new MongooseHttpServerRequestUpload(this, nc, hm) :
#endif
#if MG_ENABLE_HTTP_WEBSOCKET
        MG_EV_WEBSOCKET_HANDSHAKE_REQUEST == ev ? new MongooseHttpWebSocketConnection(this, nc, hm) :
#endif
                                             new MongooseHttpServerRequest(this, nc, hm);
      if(request)
      {
        nc->user_connection_data = request;

        if(endpoint->request)
        {
          endpoint->request(request);
#if MG_ENABLE_HTTP_WEBSOCKET
        } else if(endpoint->wsFrame) {

#endif
        } else {
          mg_http_send_error(nc, 404, NULL);
        }

        // Keep-alive: a request whose response completed synchronously is
        // finished with — free it so the connection can host the next one.
        // (Close-mode requests are freed on MG_EV_CLOSE as before.)
        checkRequestDone(nc);
      } else {
        mg_http_send_error(nc, 500, NULL);
      }
      break;
    }

#if MG_ENABLE_HTTP_STREAMING_MULTIPART
    case MG_EV_HTTP_PART_BEGIN:
    case MG_EV_HTTP_PART_DATA:
    case MG_EV_HTTP_PART_END:
    {
      DBUGF("nc->user_connection_data = %p", nc->user_connection_data);
      if(nc->user_connection_data)
      {
        struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *) p;
        MongooseHttpServerRequestUpload *request = (MongooseHttpServerRequestUpload *)nc->user_connection_data;
 
        if(!request->_responseSent && endpoint->upload) {
          mp->num_data_consumed = endpoint->upload(request, ev, MongooseString(mg_mk_str(mp->file_name)), 
                                           request->index, (uint8_t*)mp->data.p, mp->data.len);
        }

        request->index += mp->num_data_consumed;
      }
      break;
    }
#endif // MG_ENABLE_HTTP_STREAMING_MULTIPART

    case MG_EV_POLL:
    case MG_EV_SEND:
    {
      if(nc->user_connection_data)
      {
        MongooseHttpServerRequest *request = (MongooseHttpServerRequest *)nc->user_connection_data;
        if(request->responseSent()) {
          request->sendBody();
        }
        checkRequestDone(nc);
      }

      break;
    }

    case MG_EV_TIMER:
    {
      // Idle keep-alive connection outlived its welcome — close it. A live
      // request in flight means the timer is stale; ignore it.
      if(NULL == nc->user_connection_data) {
        DBUGF("Connection %p: keep-alive idle timeout", nc);
        nc->flags |= MG_F_SEND_AND_CLOSE;
      }
      break;
    }

    case MG_EV_RECV:
    {

#ifdef ENABLE_DEBUG
      int *num_bytes = (int *)p;
#endif
      DBUGF("Received %d bytes", *num_bytes);
      break;
    }

    case MG_EV_CLOSE: {
      DBUGF("Connection %p closed", nc);
      if(nc->user_connection_data) 
      {
        MongooseHttpServerRequest *request = (MongooseHttpServerRequest *)nc->user_connection_data;

        if(endpoint->close) {
          endpoint->close(request);
        }

        delete request;
        nc->user_connection_data = NULL;
      } 
      break;
    }

#if MG_ENABLE_HTTP_WEBSOCKET
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
      if(endpoint->wsConnect && nc->flags & MG_F_IS_MongooseHttpWebSocketConnection)
      {
        MongooseHttpWebSocketConnection *c = (MongooseHttpWebSocketConnection *)nc->user_connection_data;
        endpoint->wsConnect(c);
      }
    } break;

    case MG_EV_WEBSOCKET_FRAME: {
      if(endpoint->wsFrame && nc->flags & MG_F_IS_MongooseHttpWebSocketConnection)
      {
        MongooseHttpWebSocketConnection *c = (MongooseHttpWebSocketConnection *)nc->user_connection_data;
        struct websocket_message *wm = (struct websocket_message *)p;
        endpoint->wsFrame(c, wm->flags, wm->data, wm->size);
      }
    } break;

    case MG_EV_WEBSOCKET_CONTROL_FRAME: {

    } break;

#endif    
  }
}

#if MG_ENABLE_HTTP_WEBSOCKET
void MongooseHttpServer::sendAll(MongooseHttpWebSocketConnection *from, const char *endpoint, int op, const void *data, size_t len)
{
  mg_mgr *mgr = Mongoose.getMgr();

  const struct mg_connection *nc = from ? from->getConnection() : NULL;
  struct mg_connection *c;
  for (c = mg_next(mgr, NULL); c != NULL; c = mg_next(mgr, c)) {
    if (c == nc) { 
      continue; /* Don't send to the sender. */
    }
    if (c->flags & MG_F_IS_WEBSOCKET && c->flags & MG_F_IS_MongooseHttpWebSocketConnection)
    {
      MongooseHttpWebSocketConnection *to = (MongooseHttpWebSocketConnection *)c->user_connection_data;
      if(endpoint && !to->uri().equals(endpoint)) {
        continue;
      }
      DBUGF("%.*s sending to %p", to->uri().length(), to->uri().c_str(), to);
      to->send(op, data, len);
    }
  }
}
#endif

/// MongooseHttpServerRequest object

MongooseHttpServerRequest::MongooseHttpServerRequest(MongooseHttpServer *server, mg_connection *nc, http_message *msg) :
  _server(server),
  _nc(nc),
#if MG_COPY_HTTP_MESSAGE
  _msg(duplicateMessage(msg)),
#else
  _msg(msg),
#endif
  _response(NULL),
  _responseSent(false),
  _keepAlive(false),
  _completed(false)
{
  // HTTP/1.1 defaults to keep-alive, HTTP/1.0 to close; an explicit
  // Connection header from the client wins either way. The server can still
  // veto (pool pressure, runtime switch) via allowKeepAlive().
  bool clientKeepAlive = (0 == mg_vcmp(&msg->proto, "HTTP/1.1"));
  mg_str *connection = mg_get_http_header(msg, "Connection");
  if(connection) {
    if(0 == mg_vcasecmp(connection, "close")) {
      clientKeepAlive = false;
    } else if(0 == mg_vcasecmp(connection, "keep-alive")) {
      clientKeepAlive = true;
    }
  }
  _keepAlive = clientKeepAlive && server->allowKeepAlive();

  if(0 == mg_vcasecmp(&msg->method, "GET")) {
    _method = HTTP_GET;
  } else if(0 == mg_vcasecmp(&msg->method, "POST")) {
    _method = HTTP_POST;
  } else if(0 == mg_vcasecmp(&msg->method, "DELETE")) {
    _method = HTTP_DELETE;
  } else if(0 == mg_vcasecmp(&msg->method, "PUT")) {
    _method = HTTP_PUT;
  } else if(0 == mg_vcasecmp(&msg->method, "PATCH")) {
    _method = HTTP_PATCH;
  } else if(0 == mg_vcasecmp(&msg->method, "HEAD")) {
    _method = HTTP_HEAD;
  } else if(0 == mg_vcasecmp(&msg->method, "OPTIONS")) {
    _method = HTTP_OPTIONS;
  }
}

MongooseHttpServerRequest::~MongooseHttpServerRequest()
{
  if(_response) {
    delete _response;
    _response = NULL;
  }

#if MG_COPY_HTTP_MESSAGE
  mg_strfree(&_msg->message);
  delete _msg;
  _msg = NULL;
#endif
}

#if MG_COPY_HTTP_MESSAGE

mg_str mg_mk_str_from_offsets(mg_str &dest, mg_str &src, mg_str &value) {
  mg_str s;

  s.p = value.p ? (dest.p + (value.p - src.p)) : NULL;
  s.len = value.len;

  return s;
}

http_message *MongooseHttpServerRequest::duplicateMessage(http_message *sm)
{
  http_message *nm = new http_message();
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

void MongooseHttpServerRequest::completeRequest()
{
  _completed = true;
  if(_keepAlive) {
    // Leave the connection open for the next request, but reap it if the
    // client parks it idle for too long.
    mg_set_timer(_nc, mg_time() + ARDUINO_MONGOOSE_KEEPALIVE_TIMEOUT);
  } else {
    _nc->flags |= MG_F_SEND_AND_CLOSE;
  }
}

void MongooseHttpServerRequest::send(MongooseHttpServerResponse *response)
{
  if(_response) {
    delete _response;
    _response = NULL;
  }

  response->setKeepAlive(_keepAlive);
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
      DBUGF("Connection %p: sent %u/%u, %lx", _nc, sent, free, _nc->flags & MG_F_SEND_AND_CLOSE);

      if(sent < free) {
        DBUGLN("Response finished");
        delete _response;
        _response = NULL;
        completeRequest();
      }
    }
  }
}

extern "C" const char *mg_status_message(int status_code);

bool MongooseHttpServerRequest::bodyAllowed(int code)
{
  if(code >= 100 && code < 200) {
    return false;
  }
  return !(code == 204 || code == 304);
}

void MongooseHttpServerRequest::send(int code)
{
  if (bodyAllowed(code)) {
    send(code, "text/plain", mg_status_message(code));
  } else {
    send(code, "text/plain", "");
  }

}

void MongooseHttpServerRequest::send(int code, const char *contentType, const char *content)
{
  char headers[64], *pheaders = headers;
  mg_asprintf(&pheaders, sizeof(headers),
      "Connection: %s\r\n"
      "Content-Type: %s",
      _keepAlive ? "keep-alive" : "close",
      contentType);

  mg_send_head(_nc, code, strlen(content), pheaders);
  mg_send(_nc, content, strlen(content));
  completeRequest();

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

  char headers[96], *pheaders = headers;
  mg_asprintf(&pheaders, sizeof(headers),
      "Connection: %s\r\n"
      "WWW-Authenticate: Basic realm=%s",
      _keepAlive ? "keep-alive" : "close",
      realm);

  mg_send_head(_nc, 401, 0, pheaders);
  completeRequest();

  if (pheaders != headers) free(pheaders);

  _responseSent = true;
}

MongooseHttpServerResponse::MongooseHttpServerResponse() :
  _code(200),
  _contentType(NULL),
  _contentLength(-1),
  _headerBuffer(NULL),
  _keepAlive(false)
{

}

MongooseHttpServerResponse::~MongooseHttpServerResponse()
{
  if(_contentType) {
    free(_contentType);
    _contentType = NULL;
  }

  if(_headerBuffer) {
    free(_headerBuffer);
    _headerBuffer = NULL;
  }
}
void MongooseHttpServerResponse::sendHeaders(struct mg_connection *nc)
{
  char headers[64], *pheaders = headers;
  mg_asprintf(&pheaders, sizeof(headers),
      "Connection: %s\r\n"
      "Content-Type: %s%s",
      _keepAlive ? "keep-alive" : "close",
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

  if(0 == len && !_keepAlive) {
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

  if(0 == _content.len && !_keepAlive) {
    nc->flags |= MG_F_SEND_AND_CLOSE;
  }

  return send;
}

#endif

#if MG_ENABLE_HTTP_WEBSOCKET
MongooseHttpWebSocketConnection::MongooseHttpWebSocketConnection(MongooseHttpServer *server, mg_connection *nc, http_message *msg) :
  MongooseHttpServerRequest(server, nc, msg)
{
  // WebSocket connections manage their own lifetime; the HTTP keep-alive
  // machinery (completion cleanup, idle reaper) must leave them alone.
  _keepAlive = false;
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
