#ifndef MongooseHttpServerResponseStream_h
#define MongooseHttpServerResponseStream_h

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
#include "MongooseHttpServerResponse.h"

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

#endif /* MongooseHttpServerResponseStream_h */
