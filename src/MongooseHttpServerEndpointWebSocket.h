#ifndef MongooseHttpServerEndpointWebSocket_h
#define MongooseHttpServerEndpointWebSocket_h

#ifdef ARDUINO
#include "Arduino.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#endif

#include "MongooseHttpServerEndpoint.h"
#include "MongooseHttpWebSocketConnection.h"

typedef std::function<void(MongooseHttpWebSocketConnection *connection)> MongooseHttpWebSocketConnectionHandler;
typedef std::function<void(MongooseHttpWebSocketConnection *connection, int flags, uint8_t *data, size_t len)> MongooseHttpWebSocketFrameHandler;

class MongooseHttpServerEndpointWebSocket : public MongooseHttpServerEndpoint
{
  friend MongooseHttpWebSocketConnection;

  private:
    MongooseHttpWebSocketConnectionHandler _wsConnect;
    MongooseHttpWebSocketFrameHandler _wsFrame;

  protected:
    virtual MongooseHttpServerRequest *requestFactory(mg_connection *nc, HttpRequestMethodComposite method, mg_http_message *msg) {
      return new MongooseHttpWebSocketConnection(nc, method, msg, this);
    }

    void handleConnect(MongooseHttpWebSocketConnection *connection) {
      if(_wsConnect) {
        _wsConnect(connection);
      }
    }

    void handleFrame(MongooseHttpWebSocketConnection *connection, int flags, uint8_t *data, size_t len) {
      if(_wsFrame) {
        _wsFrame(connection, flags, data, len);
      }
    }

  public:
    MongooseHttpServerEndpointWebSocket(HttpRequestMethodComposite method, const char *uri = nullptr) :
      MongooseHttpServerEndpoint(method, uri),
      _wsConnect(nullptr),
      _wsFrame(nullptr)
    {

    }

    MongooseHttpServerEndpointWebSocket *onConnect(MongooseHttpWebSocketConnectionHandler handler) {
      this->_wsConnect = handler;
      return this;
    }

    MongooseHttpServerEndpointWebSocket *onFrame(MongooseHttpWebSocketFrameHandler handler) {
      this->_wsFrame = handler;
      return this;
    }
};

#endif /* MongooseHttpServerEndpointWebSocket_h */
