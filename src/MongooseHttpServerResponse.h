#ifndef MongooseHttpServerResponse_h
#define MongooseHttpServerResponse_h

#include "MongoosePlatform.h"

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"
#include "MongooseHttp.h"
#include "MongooseSocket.h"

/**
 * @brief Base class for HTTP server responses
 * 
 * Implementations handle formatting and sending response headers and bodies.
 */
class MongooseHttpServerResponse
{
  private:
    int _code;
    char *_contentType;
    int64_t _contentLength;

    char * _headerBuffer;

  protected:
    bool _keepAlive;

  public:
    MongooseHttpServerResponse();
    virtual ~MongooseHttpServerResponse();

    /**
     * @brief Set the HTTP response code (e.g., 200, 404)
     * @param code The HTTP status code
     */
    void setCode(int code) {
      _code = code;
    }
    /**
     * @brief Set the Content-Type header
     * @param contentType MIME type
     */
    void setContentType(const char *contentType);
    /**
     * @brief Set the Content-Length header
     * @param contentLength Size of the body
     */
    void setContentLength(int64_t contentLength) {
      _contentLength = contentLength;
    }
    void setKeepAlive(bool keepAlive) {
      _keepAlive = keepAlive;
    }

    /**
     * @brief Add a custom HTTP header
     * @param name Header name
     * @param value Header value
     * @return true if successfully added
     */
    bool addHeader(const char *name, const char *value);
#ifdef ARDUINO
    void setContentType(String &contentType) {
      setContentType(contentType.c_str());
    }
    void setContentType(const __FlashStringHelper *contentType);
    bool addHeader(const String& name, const String& value);
#endif

    // send the to `nc`, return true if more to send
    /**
     * @brief Send formatted HTTP headers to the connection
     * @param nc Mongoose connection
     */
    virtual void sendHeaders(struct mg_connection *nc);

    // send (a part of) the body to `nc`, return < `bytes` if no more to send
    /**
     * @brief Send (a part of) the body to the connection
     * @param nc Mongoose connection
     * @param bytes Maximum number of bytes to send in this chunk
     * @return size_t Number of bytes actually sent
     */
    virtual size_t sendBody(struct mg_connection *nc, size_t bytes) = 0;
};

#endif /* MongooseHttpServerResponse_h */
