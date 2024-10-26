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
#include <list>

#include "MongooseString.h"
#include "MongooseHttpServerConnection.h"
#include "MongooseHttpServerEndpoint.h"
#include "MongooseHttpServerEndpointUpload.h"
#include "MongooseHttpServerEndpointWebsocket.h"


class MongooseHttpServer : public MongooseHttpServerConnection
{

  private:
    std::list<MongooseHttpServerEndpoint *> _endpoints;
    MongooseHttpServerEndpoint _notFound;

    HttpRequestMethodComposite method(mg_str method);
  protected:
    void onHeaders(mg_connection *nc, mg_http_message *msg);

  public:
    MongooseHttpServer();
    ~MongooseHttpServer();

    bool begin(uint16_t port);
    bool begin(uint16_t port, const char *cert, const char *private_key);

    MongooseHttpServerEndpoint *on(const char* uri);
    MongooseHttpServerEndpoint *on(const char* uri, HttpRequestMethodComposite method);
    MongooseHttpServerEndpoint *on(const char* uri, MongooseHttpRequestHandler onRequest);
    MongooseHttpServerEndpoint *on(const char* uri, HttpRequestMethodComposite method, MongooseHttpRequestHandler onRequest);
    MongooseHttpServerEndpointUpload *on(const char* uri, MongooseHttpUploadHandler onUpload);
    MongooseHttpServerEndpointWebSocket *on(const char* uri, MongooseHttpWebSocketFrameHandler onFrame);
    MongooseHttpServerEndpoint *on(MongooseHttpServerEndpoint *endpoint);

    void onNotFound(MongooseHttpRequestHandler fn);

    void reset();

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
};

#endif /* _MongooseHttpServer_H_ */
