#ifndef MongooseHttpServerRequestUpload_h
#define MongooseHttpServerRequestUpload_h

#ifdef ARDUINO
#include "Arduino.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#endif

#include <mongoose.h>

#include "MongooseHttpServerRequest.h"

class MongooseHttpServerRequestUpload : public MongooseHttpServerRequest
{
  private:
    uint64_t index;

  public:
    MongooseHttpServerRequestUpload(mg_connection *nc, HttpRequestMethodComposite method, mg_http_message *msg, MongooseHttpServerEndpoint *endpoint) :
      MongooseHttpServerRequest(nc, method, msg, endpoint),
      index(0)
    {
    }
    virtual ~MongooseHttpServerRequestUpload() {
    }

    virtual bool isUpload() { return true; }
};

#endif /* MongooseHttpServerRequest_h Upload*/
