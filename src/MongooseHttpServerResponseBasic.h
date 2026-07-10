#ifndef MongooseHttpServerResponseBasic_h
#define MongooseHttpServerResponseBasic_h

#include "MongoosePlatform.h"

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"
#include "MongooseHttp.h"
#include "MongooseSocket.h"
#include "MongooseHttpServerResponseBasic.h"

/**
 * @brief A basic HTTP server response containing a static payload
 */
class MongooseHttpServerResponseBasic:
  public MongooseHttpServerResponse
{
  private:
    const uint8_t *ptr;
    size_t len;

  public:
    MongooseHttpServerResponseBasic();

    /**
     * @brief Set a null-terminated string as the response body
     * @param content Null-terminated string payload
     */
    void setContent(const char *content);
    /**
     * @brief Set binary data as the response body
     * @param content Pointer to the data
     * @param len Length of the data
     */
    void setContent(const uint8_t *content, size_t len);
    void setContent(MongooseString &content) {
      setContent((const uint8_t *)content.c_str(), content.length());
    }
    virtual size_t sendBody(struct mg_connection *nc, size_t bytes);
};

#endif /* MongooseHttpServerResponseBasic_h */
