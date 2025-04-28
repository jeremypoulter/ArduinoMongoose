#ifndef MongooseHttpWebSocketConnection_h
#define MongooseHttpWebSocketConnection_h

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
#include "MongooseHttpServerRequest.h"

class MongooseHttpWebSocketConnection : public MongooseHttpServerRequest
{
  protected:
    void handleWebSocketConnect(mg_connection *nc, mg_http_message *msg);
    void handleWebSocketMessage(mg_connection *nc, mg_ws_message *msg);
    void handleWebSocketControl(mg_connection *nc, mg_ws_message *msg);

  public:
    MongooseHttpWebSocketConnection(mg_connection *nc, HttpRequestMethodComposite method, mg_http_message *msg, MongooseHttpServerEndpoint *endpoint);
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

    static const char Type = 'W';
    virtual char getType() {
      return Type;
    }
};

#endif /* MongooseHttpWebSocketConnection_h */
