#ifndef MongooseHttpServerEndpointUpload_h
#define MongooseHttpServerEndpointUpload_h

#ifdef ARDUINO
#include "Arduino.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#endif

#include "MongooseHttpServerEndpointUpload.h"
#include "MongooseHttpServerRequestUpload.h"

typedef std::function<size_t(MongooseHttpServerRequest *request, int ev, MongooseString filename, uint64_t index, uint8_t *data, size_t len)> MongooseHttpUploadHandler;


class MongooseHttpServerEndpointUpload : public MongooseHttpServerEndpoint
{
  private:
    MongooseHttpUploadHandler _upload;

  protected:
    virtual MongooseHttpServerRequest *requestFactory(mg_connection *nc, HttpRequestMethodComposite method, mg_http_message *msg) {
      return new MongooseHttpServerRequestUpload(nc, method, msg, this);
    }
  public:
    MongooseHttpServerEndpointUpload(HttpRequestMethodComposite method, const char *uri = nullptr) :
      MongooseHttpServerEndpoint(method, uri),
      _upload(nullptr)
    {

    }

    MongooseHttpServerEndpoint *onUpload(MongooseHttpUploadHandler handler) {
      this->_upload = handler;
      return this;
    }
};

#endif /* MongooseHttpServerEndpointUpload_h */
