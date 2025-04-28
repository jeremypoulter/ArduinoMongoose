#ifndef MongooseHttpServerConnection_h
#define MongooseHttpServerConnection_h

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
#include "MongooseSocket.h"

class MongooseHttpServerConnection : public MongooseSocket
{
  private:
  
  protected:
    void handleEvent(mg_connection *nc, int ev, void *p);

    virtual void handleHeaders(mg_connection *nc, mg_http_message *msg) { }
    virtual void handleMessage(mg_connection *nc, mg_http_message *msg) { }
    virtual void handleWebSocketConnect(mg_connection *nc, mg_http_message *msg) { }
    virtual void handleWebSocketMessage(mg_connection *nc, mg_ws_message *msg) { }
    virtual void handleWebSocketControl(mg_connection *nc, mg_ws_message *msg) { }

  public:
    MongooseHttpServerConnection() :
      MongooseSocket()
    {
    }
    virtual ~MongooseHttpServerConnection() {
    }
};

#endif /* MongooseHttpServerConnection_h */
