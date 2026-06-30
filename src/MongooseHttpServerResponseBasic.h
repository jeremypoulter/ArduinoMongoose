#ifndef MongooseHttpServerResponseBasic_h
#define MongooseHttpServerResponseBasic_h

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
#include "MongooseHttpServerResponseBasic.h"

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

#endif /* MongooseHttpServerResponseBasic_h */
