#ifndef MongooseHttpServerRequestUpload_h
#define MongooseHttpServerRequestUpload_h

#include "MongoosePlatform.h"

#include <mongoose.h>
#include <string>

#include "MongooseString.h"
#include "MongooseHttpServerRequest.h"

// Streaming state for incremental multipart body parsing.
enum UploadStreamingState {
  UPLOAD_STREAMING_INACTIVE,       // Not streaming (or already completed)
  UPLOAD_STREAMING_PREAMBLE,       // Looking for the opening "--boundary\r\n"
  UPLOAD_STREAMING_PART_HEADERS,   // Reading per-part headers up to \r\n\r\n
  UPLOAD_STREAMING_PART_BODY,      // Streaming part body, watching for next boundary
  UPLOAD_STREAMING_COMPLETE        // All parts received and dispatched
};

class MongooseHttpServerRequestUpload : public MongooseHttpServerRequest
{
  private:
    uint64_t index;

    // Streaming multipart state
    UploadStreamingState _streamState;
    std::string _boundary;      // "--<boundary-value>" string
    std::string _partFilename;  // Filename of the current part
    std::string _partName;      // Field name of the current part
    uint64_t _partIndex;        // Bytes dispatched so far for the current part

    // Called from MG_EV_HTTP_MSG (fallback, non-streaming path).
    void handleMessage(mg_connection *nc, mg_http_message *msg);

    // Incrementally consume multipart bytes from nc->recv.
    void processBodyData(mg_connection *nc);

  protected:
    // Called by MongooseSocket when MG_EV_READ fires (streaming path).
    void onReceive(mg_connection *nc, long num_bytes) override;

  public:
    MongooseHttpServerRequestUpload(mg_connection *nc,
                                    HttpRequestMethodComposite method,
                                    mg_http_message *msg,
                                    MongooseHttpServerEndpoint *endpoint);

    virtual ~MongooseHttpServerRequestUpload() {
    }

    virtual bool isUpload() { return true; }
};

#endif /* MongooseHttpServerRequestUpload_h */
