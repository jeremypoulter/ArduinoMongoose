#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>
#include <algorithm>
#include <cstring>

#include "MongooseHttpServerRequest.h"
#include "MongooseHttpServerEndpoint.h"
#include "MongooseHttpServer.h"

MongooseHttpServerRequest::MongooseHttpServerRequest(MongooseHttpServer *server, mg_connection *nc, HttpRequestMethodComposite method, mg_http_message *msg, MongooseHttpServerEndpoint *endpoint) :
  MongooseHttpServerConnection(),
#if MG_COPY_HTTP_MESSAGE
  MongooseHttpMessage(duplicateMessage(msg)),
#else
  MongooseHttpMessage(msg),
#endif
  _method(method),
  _response(nullptr),
  _endpoint(endpoint),
  _responseSent(false),
  _server(server),
  _keepAlive(false),
  _completed(false),
  _lastActivity(0)
{
  connect(nc);
  nc->fn_data = this;
  _keepAlive = negotiateKeepAlive(msg);
}

// HTTP/1.1 defaults to keep-alive, HTTP/1.0 to close; an explicit Connection
// header from the client wins either way. The server can still veto (pool
// pressure or the runtime switch) through allowKeepAlive().
bool MongooseHttpServerRequest::negotiateKeepAlive(mg_http_message *msg)
{
  bool clientKeepAlive = (0 == mg_strcasecmp(msg->proto, mg_str("HTTP/1.1")));

  struct mg_str *connection = mg_http_get_header(msg, "Connection");
  if(connection) {
    if(0 == mg_strcasecmp(*connection, mg_str("close"))) {
      clientKeepAlive = false;
    } else if(0 == mg_strcasecmp(*connection, mg_str("keep-alive"))) {
      clientKeepAlive = true;
    }
  }

  return clientKeepAlive && _server->allowKeepAlive();
}

MongooseHttpServerRequest::~MongooseHttpServerRequest()
{
  if(_response) {
    delete _response;
    _response = nullptr;
  }

#if MG_COPY_HTTP_MESSAGE
  free(_msg->message.buf);
  delete _msg;
  _msg = nullptr;
#endif
}

#if MG_COPY_HTTP_MESSAGE

mg_str mg_mk_str_from_offsets(mg_str &dest, mg_str &src, mg_str &value)
{
  mg_str s;

  s.buf = value.buf ? (dest.buf + (value.buf - src.buf)) : nullptr;
  s.len = value.len;

  return s;
}

mg_http_message *MongooseHttpServerRequest::duplicateMessage(mg_http_message *sm)
{
  mg_http_message *nm = new mg_http_message();
  memset(nm, 0, sizeof(*nm));

  mg_str headers = mg_str_n(sm->message.buf, sm->message.len - sm->body.len);

  nm->message = mg_strdup(headers);
  nm->body = sm->body;

  nm->method = mg_mk_str_from_offsets(nm->message, sm->message, sm->method);
  nm->uri = mg_mk_str_from_offsets(nm->message, sm->message, sm->uri);
  nm->query = mg_mk_str_from_offsets(nm->message, sm->message, sm->query);
  nm->proto = mg_mk_str_from_offsets(nm->message, sm->message, sm->proto);
  nm->head = mg_mk_str_from_offsets(nm->message, sm->message, sm->head);

  for(int i = 0; i < MG_MAX_HTTP_HEADERS; i++)
  {
    nm->headers[i].name = mg_mk_str_from_offsets(nm->message, sm->message, sm->headers[i].name);
    nm->headers[i].value = mg_mk_str_from_offsets(nm->message, sm->message, sm->headers[i].value);
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

  response->setKeepAlive(_keepAlive);
  response->sendHeaders(getConnection());
  _response = response;
  sendBody();
  _responseSent = true;
}

void MongooseHttpServerRequest::sendBody()
{
  if(_response)
  {
    size_t free = getConnection()->send.size - getConnection()->send.len;
    if(0 == getConnection()->send.size) {
      free = ARDUINO_MONGOOSE_SEND_BUFFER_SIZE;
    }
    if(free > 0)
    {
      size_t sent = _response->sendBody(getConnection(), free);
      DBUGF("Connection %p: sent %zu/%zu, %d", getConnection(), sent, free, getConnection()->is_draining);

      if(sent < free) {
        DBUGLN("Response finished");
        delete _response;
        _response = nullptr;
        completeRequest();
      }
    }
  }
}

// Mongoose latches c->is_resp while a response is being generated and will not
// dispatch the next request until it is cleared (see http_cb). Clearing it here
// hands the connection back: in keep-alive mode Mongoose keeps it open for the
// next request, otherwise we drain it and close as before.
void MongooseHttpServerRequest::completeRequest()
{
  _completed = true;
  _lastActivity = mg_millis();

  mg_connection *nc = getConnection();
  if(nc) {
    nc->is_resp = 0;
    if(!_keepAlive) {
      nc->is_draining = 1;
    }
  }
}


void MongooseHttpServerRequest::send(int code) {
  send(code, "text/plain", "");
}

void MongooseHttpServerRequest::send(int code, const char *contentType, const char *content)
{
  char headers[128];
  mg_snprintf(headers, sizeof(headers),
      "Connection: %s\r\n"
      "Content-Type: %s\r\n",
      _keepAlive ? "keep-alive" : "close",
      contentType);

  mg_http_reply(getConnection(), code, headers, "%s", content);

  completeRequest();

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
  return mg_http_get_var((HTTP_GET == _method) ? (&_msg->query) : (&_msg->body),
    name, dst, dst_len);
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
  return getParam(name.c_str());
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


  mg_http_creds(_msg,
    user_buf, sizeof(user_buf),
    pass_buf, sizeof(pass_buf));

  if(user_buf[0] != '\0' && pass_buf[0] != '\0')
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
  char headers[256];
  mg_snprintf(headers, sizeof(headers),
      "Connection: %s\r\n"
      "WWW-Authenticate: Basic realm=%s\r\n",
      _keepAlive ? "keep-alive" : "close",
      realm ? realm : "");

  mg_http_reply(getConnection(), 401, headers, "");
  completeRequest();

  _responseSent = true;
}


void MongooseHttpServerRequest::handleClose(mg_connection *nc)
{
  _endpoint->handleClose(this);
  MongooseSocket::onClose(nc);
  nc->fn_data = nullptr;
  delete this;
}


void MongooseHttpServerRequest::handlePoll(mg_connection *nc)
{
  if(responseSent()) {
    sendBody();
    return;
  }

  // Reap a kept-alive connection that has been parked idle for too long. A
  // connection still flushing queued response bytes to a slow client is not
  // idle, so the timeout only runs once the send buffer is empty. (Draining
  // early would not truncate the response - Mongoose closes a draining
  // connection only once the send buffer is empty - but transmit time should
  // not count as idle time.)
  if(_completed && _keepAlive && 0 == nc->send.len &&
     (mg_millis() - _lastActivity) >= _server->keepAliveTimeout()) {
    DBUGF("Connection %p: keep-alive idle timeout", nc);
    nc->is_draining = 1;
  }
}

void MongooseHttpServerRequest::handleHeaders(mg_connection *nc, mg_http_message *msg)
{
  // Headers for the FIRST request are matched by the server; this override only
  // fires for a further request arriving on a kept-alive connection this request
  // already answered. Hand it back to the server for fresh endpoint matching,
  // which installs a new request as the connection handler, then retire this one.
  if(!_completed) {
    return;
  }

  _server->handleHeaders(nc, msg);

  // Retire only once a new request has taken over the connection. If matching
  // did not install one (e.g. a 405, which drains the connection) this request
  // stays attached so the eventual MG_EV_CLOSE still dispatches through
  // handleClose() and the endpoint close callbacks fire as usual.
  if(nc->fn_data != this) {
    // The new request owns the connection now; ~MongooseSocket() would
    // otherwise clear its freshly installed fn_data and mark the connection
    // closing. Release our reference first, as in handleClose().
    clearConnection();
    delete this;
  }
}

void MongooseHttpServerRequest::handleMessage(mg_connection *nc, mg_http_message *msg)
{
  DBUGF("MongooseHttpServerRequest::onMessage");
#if MG_COPY_HTTP_MESSAGE
  _msg->body = msg->body;
#else
  _msg = msg;
#endif
  _endpoint->handleRequest(this);
}
