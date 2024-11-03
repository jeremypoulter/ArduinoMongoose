#ifndef MongooseHttpServerEndpoint_h
#define MongooseHttpServerEndpoint_h

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
#include "MongooseHttpServerRequest.h"
#include "MongooseHttpWebSocketConnection.h"

class MongooseHttpServer;

typedef std::function<void(MongooseHttpServerRequest *request)> MongooseHttpRequestHandler;

enum RequestHandle {
  REQUEST_NO_MATCH = 0,
  REQUEST_URL_MATCH,
  REQUEST_WILL_HANDLE,
  REQUEST_ERROR
};

class MongooseHttpServerEndpoint
{
  friend MongooseHttpServer;
  friend MongooseHttpServerRequest;

  private:
    HttpRequestMethodComposite _method;
    MongooseString _uri;

    MongooseHttpRequestHandler _request;
    MongooseHttpRequestHandler _close;

    RequestHandle willHandleRequest(mg_connection *nc, HttpRequestMethodComposite requestMethod, mg_http_message *msg);
    void handleRequest(MongooseHttpServerRequest *request);
    void handleClose(MongooseHttpServerRequest *request);
  protected:
    virtual MongooseHttpServerRequest *requestFactory(mg_connection *nc, HttpRequestMethodComposite method, mg_http_message *msg) {
      return new MongooseHttpServerRequest(nc, method, msg, this);
    }

  public:
    MongooseHttpServerEndpoint(HttpRequestMethodComposite method, const char *uri = nullptr) :
      _method(method),
      _uri(uri),
      _request(nullptr)
    {

    }

    ~MongooseHttpServerEndpoint() {
    }

    MongooseHttpServerEndpoint *onRequest(MongooseHttpRequestHandler handler) {
      this->_request = handler;
      return this;
    }

    MongooseHttpServerEndpoint *onClose(MongooseHttpRequestHandler handler) {
      this->_close = handler;
      return this;
    }
};


#endif /* MongooseHttpServerEndpoint_h */
