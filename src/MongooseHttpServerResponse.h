#ifndef MongooseHttpServerResponse_h
#define MongooseHttpServerResponse_h

#include "MongoosePlatform.h"

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"
#include "MongooseHttp.h"
#include "MongooseSocket.h"

class MongooseHttpServerResponse
{
  private:
    int _code;
    char *_contentType;
    int64_t _contentLength;

    char * _headerBuffer;

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

#endif /* MongooseHttpServerResponse_h */
