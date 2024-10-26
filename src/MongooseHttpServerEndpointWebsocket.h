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
  private:
    MongooseHttpWebSocketConnectionHandler _wsConnect;
    MongooseHttpWebSocketFrameHandler _wsFrame;

  protected:
    virtual MongooseHttpServerRequest *requestFactory(mg_connection *nc, HttpRequestMethodComposite method, mg_http_message *msg) {
      return new MongooseHttpWebSocketConnection(nc, method, msg);
    }

  public:
    MongooseHttpServerEndpointWebSocket(HttpRequestMethodComposite method, const char *uri = nullptr) :
      MongooseHttpServerEndpoint(method, uri),
      _wsConnect(nullptr),
      _wsFrame(nullptr)
    {

    }

    MongooseHttpServerEndpoint *onConnect(MongooseHttpWebSocketConnectionHandler handler) {
      this->_wsConnect = handler;
      return this;
    }

    MongooseHttpServerEndpoint *onFrame(MongooseHttpWebSocketFrameHandler handler) {
      this->_wsFrame = handler;
      return this;
    }
};

#endif /* MongooseHttpServerEndpointWebSocket_h */
