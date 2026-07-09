#ifndef MongooseHttpServerEndpointUpload_h
#define MongooseHttpServerEndpointUpload_h

#include "MongoosePlatform.h"

#include "MongooseHttpServerEndpoint.h"
#include "MongooseHttpServerRequestUpload.h"

typedef std::function<size_t(MongooseHttpServerRequest *request, int ev, MongooseString filename, uint64_t index, uint8_t *data, size_t len)> MongooseHttpUploadHandler;

// Synthetic multipart event codes passed to MongooseHttpUploadHandler
#define MG_EV_HTTP_PART_BEGIN 1
#define MG_EV_HTTP_PART_DATA  2
#define MG_EV_HTTP_PART_END   3


class MongooseHttpServerEndpointUpload : public MongooseHttpServerEndpoint
{
  friend class MongooseHttpServerRequestUpload;

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

    MongooseHttpServerEndpointUpload *onRequest(MongooseHttpRequestHandler handler) {
      MongooseHttpServerEndpoint::onRequest(handler);
      return this;
    }

    MongooseHttpServerEndpointUpload *onClose(MongooseHttpRequestHandler handler) {
      MongooseHttpServerEndpoint::onClose(handler);
      return this;
    }

    MongooseHttpServerEndpointUpload *onUpload(MongooseHttpUploadHandler handler) {
      this->_upload = handler;
      return this;
    }

    bool hasUploadHandler() const {
      return _upload != nullptr;
    }

    void callUpload(MongooseHttpServerRequest *request, int ev, MongooseString filename, uint64_t index, uint8_t *data, size_t len) {
      if(_upload) {
        _upload(request, ev, filename, index, data, len);
      }
    }
};

#endif /* MongooseHttpServerEndpointUpload_h */
