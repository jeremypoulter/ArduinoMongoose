#ifndef MongooseHttpServerRequestUpload_h
#define MongooseHttpServerRequestUpload_h

#include "MongoosePlatform.h"

#include <mongoose.h>

#include "MongooseString.h"
#include "MongooseHttpServerRequest.h"

class MongooseHttpServerRequestUpload : public MongooseHttpServerRequest
{
  private:
    uint64_t index;

    // Raw (non-multipart) body streaming state.
    // When _streaming is true, MG_EV_READ events deliver body chunks directly
    // to the upload handler rather than waiting for MG_EV_HTTP_MSG.
    bool     _streaming;      // true when streaming raw body
    size_t   _bodyExpected;   // total body bytes expected (from Content-Length)
    size_t   _bodyReceived;   // body bytes processed so far

    // Called from MG_EV_HTTP_MSG for multipart/form-data requests.
    // Uses mg_http_next_multipart() to iterate parts from the fully-buffered body.
    void handleMessage(mg_connection *nc, mg_http_message *msg);

  protected:
    // Called by MongooseSocket when MG_EV_READ fires (raw-body streaming path).
    void onReceive(mg_connection *nc, long num_bytes) override;

  public:
    MongooseHttpServerRequestUpload(MongooseHttpServer *server,
                                    mg_connection *nc,
                                    HttpRequestMethodComposite method,
                                    mg_http_message *msg,
                                    MongooseHttpServerEndpoint *endpoint);

    virtual ~MongooseHttpServerRequestUpload() {
    }

    virtual bool isUpload() { return true; }
};

#endif /* MongooseHttpServerRequestUpload_h */
