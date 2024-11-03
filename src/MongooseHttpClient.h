#ifndef MongooseHttpClient_h
#define MongooseHttpClient_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"
#include "MongooseSocket.h"
#include "MongooseHttp.h"

#ifndef MOGOOSE_HTTP_CLIENT_TIMEOUT
#define MOGOOSE_HTTP_CLIENT_TIMEOUT 1500
#endif

class MongooseHttpClient;
class MongooseHttpClientRequest;
class MongooseHttpClientResponse;

typedef std::function<void(MongooseHttpClientResponse *request)> MongooseHttpResponseHandler;

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
};

class MongooseHttpClientResponse {
  protected:
    mg_http_message *_msg;

  public:
    MongooseHttpClientResponse(mg_http_message *msg) :
      _msg(msg)
    {
    }

    ~MongooseHttpClientResponse() {
    }

    MongooseString message() {
      return MongooseString(_msg->message);
    }
    MongooseString body() {
      return MongooseString(_msg->body);
    }

    MongooseString methodStr() {
      return MongooseString(_msg->method);
    }
    MongooseString uri() {
      return MongooseString(_msg->uri);
    }
    MongooseString proto() {
      return MongooseString(_msg->proto);
    }

    int respCode() {
      return mg_http_status(_msg);
    }
    MongooseString respStatusMsg() {
      return MongooseString(_msg->proto);
    }

    MongooseString queryString() {
      return MongooseString(_msg->query);
    }

    int headers() {
      int i;
      for (i = 0; i < MG_MAX_HTTP_HEADERS && _msg->headers[i].name.len > 0; i++) {
      }
      return i;
    }
    MongooseString headers(const char *name) {
      MongooseString ret(mg_http_get_header(_msg, name));
      return ret;
    }
    MongooseString headerNames(int i) {
      return MongooseString(_msg->headers[i].name);
    }
    MongooseString headerValues(int i) {
      return MongooseString(_msg->headers[i].value);
    }

    MongooseString host() {
      return headers("Host");
    }

    MongooseString contentType() {
      return headers("Content-Type");
    }

    size_t contentLength();
};

class MongooseHttpClient
{
  public:
    MongooseHttpClient();
    ~MongooseHttpClient();

    MongooseHttpClientRequest *beginRequest(const char *uri);

    bool get(const char* uri, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr);
    bool post(const char* uri, const char *contentType, const char *body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr);
    bool put(const char* uri, const char *contentType, const char *body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr);

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
#endif // ARDUINO
};


#endif /* _MongooseHttpClient_H_ */
