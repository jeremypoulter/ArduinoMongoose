#ifndef MongooseHttpServerResponseStream_h
#define MongooseHttpServerResponseStream_h

#include "MongoosePlatform.h"

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"
#include "MongooseHttp.h"
#include "MongooseSocket.h"
#include "MongooseHttpServerResponse.h"

#ifdef ARDUINO
/**
 * @brief An HTTP server response that allows Print-style streaming (Arduino only)
 */
class MongooseHttpServerResponseStream:
  public MongooseHttpServerResponse,
  public Print
{
  private:
    mg_iobuf _content;

  public:
    MongooseHttpServerResponseStream();
    virtual ~MongooseHttpServerResponseStream();

    /**
     * @brief Write data to the response buffer
     * @param data Buffer containing data
     * @param len Number of bytes to write
     * @return size_t Number of bytes written
     */
    size_t write(const uint8_t *data, size_t len);
    size_t write(uint8_t data);
  //  using Print::write;

    virtual size_t sendBody(struct mg_connection *nc, size_t bytes);
};
#endif

#endif /* MongooseHttpServerResponseStream_h */
