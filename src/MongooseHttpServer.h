#ifndef MongooseHttpServer_h
#define MongooseHttpServer_h

#ifdef ARDUINO
#include "Arduino.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#endif

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"
#include "MongooseHttp.h"

// Make a copy of the HTTP header so it is avalible outside of the onReceive
// callback. Setting to 0 will save some runtime memory but accessing the HTTP
// message details outside of the onReceive callback will give undefined behaviour.
// The body may not allways be avalible even in onReceive, eg file upload
#ifndef MG_COPY_HTTP_MESSAGE
#define MG_COPY_HTTP_MESSAGE 1
#endif

// HTTP keep-alive. Connections are reused for further requests when the
// client supports it, instead of being closed after every response. An idle
// kept-alive connection is closed after ARDUINO_MONGOOSE_KEEPALIVE_TIMEOUT
// seconds. Keep-alive is not offered while the manager already has more than
// ARDUINO_MONGOOSE_KEEPALIVE_MAX_CONNECTIONS connections — embedded TCP/IP
// stacks have small connection pools (LWIP defaults to 16 on ESP32) and
// parked connections would starve them.
#ifndef ARDUINO_MONGOOSE_KEEPALIVE_TIMEOUT
#define ARDUINO_MONGOOSE_KEEPALIVE_TIMEOUT 30
#endif
#ifndef ARDUINO_MONGOOSE_KEEPALIVE_MAX_CONNECTIONS
#define ARDUINO_MONGOOSE_KEEPALIVE_MAX_CONNECTIONS 8
#endif

class MongooseHttpServer;
class MongooseHttpServerRequest;
class MongooseHttpServerResponse;
class MongooseHttpServerResponseBasic;
#ifdef ARDUINO
class MongooseHttpServerResponseStream;
#endif
#if MG_ENABLE_HTTP_WEBSOCKET
class MongooseHttpWebSocketConnection;
#define MG_F_IS_MongooseHttpWebSocketConnection MG_F_USER_1
#endif


class MongooseHttpServerRequest {
  friend MongooseHttpServer;

  protected:
    MongooseHttpServer *_server;
    mg_connection *_nc;
    http_message *_msg;
    HttpRequestMethodComposite _method;
    MongooseHttpServerResponse *_response;
    bool _responseSent;
    bool _keepAlive;
    bool _completed;

    void sendBody();
    bool bodyAllowed(int code);
    // Mark the response finished: close the connection (legacy mode) or arm
    // the idle timer and leave it open for the next request (keep-alive).
    void completeRequest();

#if MG_COPY_HTTP_MESSAGE
    http_message *duplicateMessage(http_message *);
#endif

  public:
    MongooseHttpServerRequest(MongooseHttpServer *server, mg_connection *nc, http_message *msg);
    virtual ~MongooseHttpServerRequest();

    virtual bool isUpload() { return false; }
    virtual bool isWebSocket() { return false; }

    // True when the connection will be kept open for further requests after
    // this response completes.
    bool keepAlive() const {
      return _keepAlive;
    }
    // Close the connection after this response even if the client asked for
    // keep-alive. Call before sending the response.
    void forceClose() {
      _keepAlive = false;
    }
    bool completed() const {
      return _completed;
    }

    HttpRequestMethodComposite method() {
      return _method;
    }

    MongooseString message() {
      return MongooseString(_msg->message);
    }
    MongooseString body() {
      return MongooseString(_msg->body);
    }

    MongooseString methodStr() {
      return MongooseString(_msg->method);
    }
    MongooseString uri() {
      return MongooseString(_msg->uri);
    }
    MongooseString proto() {
      return MongooseString(_msg->proto);
    }

    int respCode() {
      return _msg->resp_code;
    }
    MongooseString respStatusMsg() {
      return MongooseString(_msg->resp_status_msg);
    }

    MongooseString queryString() {
      return MongooseString(_msg->query_string);
    }

    int headers() {
      int i;
      for (i = 0; i < MG_MAX_HTTP_HEADERS && _msg->header_names[i].len > 0; i++) {
      }
      return i;
    }
    MongooseString headers(const char *name) {
      MongooseString ret(mg_get_http_header(_msg, name));
      return ret;
    }
    MongooseString headerNames(int i) {
      return MongooseString(_msg->header_names[i]);
    }
    MongooseString headerValues(int i) {
      return MongooseString(_msg->header_values[i]);
    }

    MongooseString host() {
      return headers("Host");
    }

    MongooseString contentType() {
      return headers("Content-Type");
    }

    size_t contentLength() {
      return _msg->body.len;
    }

    void redirect(const char *url);
#ifdef ARDUINO
    void redirect(const String& url);
#endif

    MongooseHttpServerResponseBasic *beginResponse();

#ifdef ARDUINO
    MongooseHttpServerResponseStream *beginResponseStream();
#endif

    // Takes ownership of `response`, will delete when finished. Do not use `response` after calling
    void send(MongooseHttpServerResponse *response);
    bool responseSent() {
      return NULL != _response;
    }

    void send(int code);
    void send(int code, const char *contentType, const char *content="");
#ifdef ARDUINO
    void send(int code, const String& contentType, const String& content=String());
#endif

    bool hasParam(const char *name) const;
#ifdef ARDUINO
    bool hasParam(const String& name) const;
    bool hasParam(const __FlashStringHelper * data) const;
#endif

    int getParam(const char *name, char *dst, size_t dst_len) const;
#ifdef ARDUINO
    int getParam(const String& name, char *dst, size_t dst_len) const;
    int getParam(const __FlashStringHelper * data, char *dst, size_t dst_len) const;
#endif

#ifdef ARDUINO
    String getParam(const char *name) const;
    String getParam(const String& name) const;
    String getParam(const __FlashStringHelper * data) const;
#endif

    bool authenticate(const char * username, const char * password);
#ifdef ARDUINO
    bool authenticate(const String& username, const String& password) {
      return authenticate(username.c_str(), password.c_str());
    }
#endif
    void requestAuthentication(const char* realm);
#ifdef ARDUINO
    void requestAuthentication(const String& realm) {
      requestAuthentication(realm.c_str());
    }
#endif
};

class MongooseHttpServerRequestUpload : public MongooseHttpServerRequest
{
  friend MongooseHttpServer;

  private:
    uint64_t index;

  public:
    MongooseHttpServerRequestUpload(MongooseHttpServer *server, mg_connection *nc, http_message *msg) :
      MongooseHttpServerRequest(server, nc, msg),
      index(0)
    {
      // Multipart uploads keep the close-per-request lifecycle: consumers use
      // the connection-close event to finalise the upload.
      _keepAlive = false;
    }
    virtual ~MongooseHttpServerRequestUpload() {
    }

    virtual bool isUpload() { return true; }
};

class MongooseHttpServerResponse
{
  private:
    int _code;
    char *_contentType;
    int64_t _contentLength;

    char * _headerBuffer;

  protected:
    bool _keepAlive;

  public:
    MongooseHttpServerResponse();
    virtual ~MongooseHttpServerResponse();

    void setCode(int code) {
      _code = code;
    }
    void setContentType(const char *contentType);
    void setContentLength(int64_t contentLength) {
      _contentLength = contentLength;
    }
    void setKeepAlive(bool keepAlive) {
      _keepAlive = keepAlive;
    }

    bool addHeader(const char *name, const char *value);
#ifdef ARDUINO
    void setContentType(String &contentType) {
      setContentType(contentType.c_str());
    }
    void setContentType(const __FlashStringHelper *contentType);
    bool addHeader(const String& name, const String& value);
#endif

    // send the to `nc`, return true if more to send
    virtual void sendHeaders(struct mg_connection *nc);

    // send (a part of) the body to `nc`, return < `bytes` if no more to send
    virtual size_t sendBody(struct mg_connection *nc, size_t bytes) = 0;
};

class MongooseHttpServerResponseBasic:
  public MongooseHttpServerResponse
{
  private:
    const uint8_t *ptr;
    size_t len;

  public:
    MongooseHttpServerResponseBasic();

    void setContent(const char *content);
    void setContent(const uint8_t *content, size_t len);
    void setContent(MongooseString &content) {
      setContent((const uint8_t *)content.c_str(), content.length());
    }
    virtual size_t sendBody(struct mg_connection *nc, size_t bytes);
};

#ifdef ARDUINO
class MongooseHttpServerResponseStream:
  public MongooseHttpServerResponse,
  public Print
{
  private:
    mbuf _content;

  public:
    MongooseHttpServerResponseStream();
    virtual ~MongooseHttpServerResponseStream();

    size_t write(const uint8_t *data, size_t len);
    size_t write(uint8_t data);
  //  using Print::write;

    virtual size_t sendBody(struct mg_connection *nc, size_t bytes);
};
#endif


typedef std::function<void(MongooseHttpServerRequest *request)> MongooseHttpRequestHandler;
typedef std::function<size_t(MongooseHttpServerRequest *request, int ev, MongooseString filename, uint64_t index, uint8_t *data, size_t len)> MongooseHttpUploadHandler;
#if MG_ENABLE_HTTP_WEBSOCKET
typedef std::function<void(MongooseHttpWebSocketConnection *connection)> MongooseHttpWebSocketConnectionHandler;
typedef std::function<void(MongooseHttpWebSocketConnection *connection, int flags, uint8_t *data, size_t len)> MongooseHttpWebSocketFrameHandler;
#endif

class MongooseHttpServerEndpoint
{
  friend MongooseHttpServer;

  private:
    MongooseHttpServer *server;
    HttpRequestMethodComposite method;
    MongooseHttpRequestHandler request;
    MongooseHttpUploadHandler upload;
    MongooseHttpRequestHandler close;
#if MG_ENABLE_HTTP_WEBSOCKET
    MongooseHttpWebSocketConnectionHandler wsConnect;
    MongooseHttpWebSocketFrameHandler wsFrame;
#endif
  public:
    MongooseHttpServerEndpoint(MongooseHttpServer *server, HttpRequestMethodComposite method) :
      server(server),
      method(method),
      request(NULL),
      upload(NULL),
      close(NULL)
#if MG_ENABLE_HTTP_WEBSOCKET
      ,
      wsConnect(NULL),
      wsFrame(NULL)
#endif
    {
    }

    MongooseHttpServerEndpoint *onRequest(MongooseHttpRequestHandler handler) {
      this->request = handler;
      return this;
    }

    MongooseHttpServerEndpoint *onUpload(MongooseHttpUploadHandler handler) {
      this->upload = handler;
      return this;
    }

    MongooseHttpServerEndpoint *onClose(MongooseHttpRequestHandler handler) {
      this->close = handler;
      return this;
    }

#if MG_ENABLE_HTTP_WEBSOCKET
    MongooseHttpServerEndpoint *onConnect(MongooseHttpWebSocketConnectionHandler handler) {
      this->wsConnect = handler;
      return this;
    }

    MongooseHttpServerEndpoint *onFrame(MongooseHttpWebSocketFrameHandler handler) {
      this->wsFrame = handler;
      return this;
    }
#endif
};

#if MG_ENABLE_HTTP_WEBSOCKET
class MongooseHttpWebSocketConnection : public MongooseHttpServerRequest
{
  friend MongooseHttpServer;

  public:
    MongooseHttpWebSocketConnection(MongooseHttpServer *server, mg_connection *nc, http_message *msg);
    virtual ~MongooseHttpWebSocketConnection();

    virtual bool isWebSocket() { return true; }

    void send(int op, const void *data, size_t len);
    void send(const char *buf) {
      send(WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
#ifdef ARDUINO
    void send(String &str) {
      send(str.c_str());
    }
#endif

    const union socket_address *getRemoteAddress() {
      return &(_nc->sa);
    }
    const mg_connection *getConnection() {
      return _nc;
    }
};
#endif

class MongooseHttpServer
{
  protected:
    struct mg_connection *nc;
    MongooseHttpServerEndpoint defaultEndpoint;
    bool _keepAliveEnabled;

    static void defaultEventHandler(struct mg_connection *nc, int ev, void *p, void *u);
    static void endpointEventHandler(struct mg_connection *nc, int ev, void *p, void *u);
    void eventHandler(struct mg_connection *nc, int ev, void *p, HttpRequestMethodComposite method, MongooseHttpServerEndpoint *endpoint);

  public:
    MongooseHttpServer();
    ~MongooseHttpServer();

    bool begin(uint16_t port);

#if MG_ENABLE_SSL
    bool begin(uint16_t port, const char *cert, const char *private_key);
#endif

    MongooseHttpServerEndpoint *on(const char* uri);
    MongooseHttpServerEndpoint *on(const char* uri, HttpRequestMethodComposite method);
    MongooseHttpServerEndpoint *on(const char* uri, MongooseHttpRequestHandler onRequest);
    MongooseHttpServerEndpoint *on(const char* uri, HttpRequestMethodComposite method, MongooseHttpRequestHandler onRequest);

    void onNotFound(MongooseHttpRequestHandler fn);

    void reset();

    // Runtime switch for HTTP keep-alive (on by default).
    void enableKeepAlive(bool enable) {
      _keepAliveEnabled = enable;
    }
    // Whether a new request may hold its connection open: keep-alive enabled
    // and the connection pool not under pressure.
    bool allowKeepAlive();

#if MG_ENABLE_HTTP_WEBSOCKET
    void sendAll(MongooseHttpWebSocketConnection *from, const char *endpoint, int op, const void *data, size_t len);

    void sendAll(MongooseHttpWebSocketConnection *from, int op, const void *data, size_t len) {
      sendAll(from, NULL, op, data, len);
    }
    void sendAll(int op, const void *data, size_t len) {
      sendAll(NULL, NULL, op, data, len);
    }
    void sendAll(MongooseHttpWebSocketConnection *from, const char *buf) {
      sendAll(from, NULL, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
    void sendAll(const char *buf) {
      sendAll(NULL, NULL, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
    void sendAll(const char *endpoint, int op, const void *data, size_t len) {
      sendAll(NULL, endpoint, op, data, len);
    }
    void sendAll(MongooseHttpWebSocketConnection *from, const char *endpoint, const char *buf) {
      sendAll(from, endpoint, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
    void sendAll(const char *endpoint, const char *buf) {
      sendAll(NULL, endpoint, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
#ifdef ARDUINO
    void sendAll(MongooseHttpWebSocketConnection *from, String &str) {
      sendAll(from, str.c_str());
    }
    void sendAll(String &str) {
      sendAll(str.c_str());
    }
    void sendAll(MongooseHttpWebSocketConnection *from, const char *endpoint, String &str) {
      sendAll(from, endpoint, str.c_str());
    }
    void sendAll(const char *endpoint, String &str) {
      sendAll(endpoint, str.c_str());
    }
#endif
#endif // MG_ENABLE_HTTP_WEBSOCKET
};

#endif /* _MongooseHttpServer_H_ */
