#ifndef MongooseHttpClient_h
#define MongooseHttpClient_h

#include "MongoosePlatform.h"

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"
#include "MongooseSocket.h"
#include "MongooseHttp.h"
#include "MongooseHttpMessage.h"

#ifndef MOGOOSE_HTTP_CLIENT_TIMEOUT
#define MOGOOSE_HTTP_CLIENT_TIMEOUT 1500
#endif

class MongooseHttpClient;
class MongooseHttpClientRequest;
class MongooseHttpClientResponse;

typedef std::function<void(MongooseHttpClientResponse *request)> MongooseHttpResponseHandler;

/**
 * @brief Represents an outbound HTTP request
 */
class MongooseHttpClientRequest : public MongooseSocket
{
  private:
    MongooseHttpResponseHandler _onResponse;
    MongooseHttpResponseHandler _onBody;

    const char *_uri;
    HttpRequestMethodComposite _method;
    const char *_contentType;
    int64_t _contentLength;
    const uint8_t *_body;
    char *_extraHeaders;
    uint64_t _timeout_ms;

  protected:
    void handleEvent(mg_connection *nc, int ev, void *p);
    void onOpen(mg_connection *nc);
    void onConnect(mg_connection *nc);
    void onPoll(mg_connection *nc);
    void onClose(mg_connection *nc);

  public:
    MongooseHttpClientRequest(const char *uri);
    virtual ~MongooseHttpClientRequest();

    bool send();

    MongooseHttpClientRequest *setMethod(HttpRequestMethodComposite method) {
      _method = method;
      return this;
    }
    MongooseHttpClientRequest *setContentType(const char *contentType) {
      _contentType = contentType;
      return this;
    }
    MongooseHttpClientRequest *setContentLength(int64_t contentLength) {
      _contentLength = contentLength;
      return this;
    }
    MongooseHttpClientRequest *setContent(const char *content) {
      setContent((uint8_t *)content, strlen(content));
      return this;
    }
    MongooseHttpClientRequest *setContent(const uint8_t *content, size_t len);

    bool addHeader(const char *name, size_t nameLength, const char *value, size_t valueLength);
    bool addHeader(const char *name, const char *value) {
      return addHeader(name, strlen(name), value, strlen(value));
    }
    bool addHeader(MongooseString name, MongooseString value) {
      return addHeader(name.c_str(), name.length(), value.c_str(), value.length());
    }
#ifdef ARDUINO
    bool addHeader(const String& name, const String& value) {
      return addHeader(name.c_str(), name.length(), value.c_str(), value.length());
    };
#endif

    MongooseHttpClientRequest *onResponse(MongooseHttpResponseHandler handler) {
      _onResponse = handler;
      return this;
    }

    MongooseHttpClientRequest *onBody(MongooseHttpResponseHandler handler) {
      _onBody = handler;
      return this;
    }

    MongooseHttpClientRequest *onClose(MongooseSocketCloseHandler handler) {
      MongooseSocket::onClose(handler);
      return this;
    }

    MongooseHttpClientRequest *setInsecure() {
      setRejectUnauthorized(false);
      return this;
    }

    // Gracefully stop the request and let the connection drain to close.
    bool cancel() {
      if(!getConnection()) {
        return false;
      }

      MongooseSocket::disconnect();
      return true;
    }

    // Immediately tear down the underlying connection.
    bool abort() {
      if(!getConnection()) {
        return false;
      }

      MongooseSocket::abort();
      return true;
    }
};

/**
 * @brief Represents an inbound HTTP response
 */
class MongooseHttpClientResponse : public MongooseHttpMessage {
  public:
    MongooseHttpClientResponse(mg_http_message *msg) :
      MongooseHttpMessage(msg)
    {
    }

    ~MongooseHttpClientResponse() {
    }

    size_t contentLength();
};

/**
 * @brief HTTP Client wrapper
 * 
 * Provides methods to perform asynchronous HTTP requests.
 */
class MongooseHttpClient
{
  public:
    MongooseHttpClient();
    ~MongooseHttpClient();

    /**
     * @brief Begin a custom HTTP request
     * @param uri The URI to request
     * @return MongooseHttpClientRequest* The request object to configure and send
     */
    MongooseHttpClientRequest *beginRequest(const char *uri);

    /**
     * @brief Perform an HTTP GET request
     * @param uri The URI to request
     * @param onResponse Callback when response is received
     * @param onClose Callback when connection is closed
     * @return true if request was sent
     */
    bool get(const char* uri, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr);
    /**
     * @brief Perform an HTTP POST request
     * @param uri The URI to request
     * @param contentType The Content-Type header value
     * @param body The body payload
     * @param onResponse Callback when response is received
     * @param onClose Callback when connection is closed
     * @return true if request was sent
     */
    bool post(const char* uri, const char *contentType, const char *body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr);
    /**
     * @brief Perform an HTTP PUT request
     * @param uri The URI to request
     * @param contentType The Content-Type header value
     * @param body The body payload
     * @param onResponse Callback when response is received
     * @param onClose Callback when connection is closed
     * @return true if request was sent
     */
    bool put(const char* uri, const char *contentType, const char *body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr);
    /**
     * @brief Perform an HTTP PATCH request
     * @param uri The URI to request
     * @param contentType The Content-Type header value
     * @param body The body payload
     * @param onResponse Callback when response is received
     * @param onClose Callback when connection is closed
     * @return true if request was sent
     */
    bool patch(const char* uri, const char *contentType, const char *body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr);
    /**
     * @brief Perform an HTTP DELETE request
     * @param uri The URI to request
     * @param onResponse Callback when response is received
     * @param onClose Callback when connection is closed
     * @return true if request was sent
     */
    bool delete_(const char* uri, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr);

#ifdef ARDUINO
    bool get(String &uri, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr) {
      return get(uri.c_str(), onResponse, onClose);
    }

    bool post(String &uri, const char *contentType, const char *body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr) {
      return post(uri.c_str(), contentType, body, onResponse, onClose);
    }
    bool post(String& uri, String& contentType, const char *body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr) {
      return post(uri.c_str(), contentType.c_str(), body, onResponse, onClose);
    }
    bool post(String &uri, String& contentType, String& body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr) {
      return post(uri.c_str(), contentType.c_str(), body.c_str(), onResponse, onClose);
    }

    bool put(String &uri, const char *contentType, const char *body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr) {
      return put(uri.c_str(), contentType, body, onResponse, onClose);
    }
    bool put(String& uri, String& contentType, const char *body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr) {
      return put(uri.c_str(), contentType.c_str(), body, onResponse, onClose);
    }
    bool put(String &uri, String& contentType, String& body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr) {
      return put(uri.c_str(), contentType.c_str(), body.c_str(), onResponse, onClose);
    }

    bool patch(String &uri, const char *contentType, const char *body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr) {
      return patch(uri.c_str(), contentType, body, onResponse, onClose);
    }
    bool patch(String& uri, String& contentType, const char *body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr) {
      return patch(uri.c_str(), contentType.c_str(), body, onResponse, onClose);
    }
    bool patch(String &uri, String& contentType, String& body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr) {
      return patch(uri.c_str(), contentType.c_str(), body.c_str(), onResponse, onClose);
    }

    bool delete_(String &uri, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr) {
      return delete_(uri.c_str(), onResponse, onClose);
    }
#endif // ARDUINO
};


#endif /* _MongooseHttpClient_H_ */
