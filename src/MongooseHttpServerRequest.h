#ifndef MongooseHttpServerRequest_h
#define MongooseHttpServerRequest_h

#include "MongoosePlatform.h"

#include "MongooseHttpMessage.h"
#include "MongooseHttpServerConnection.h"
#include "MongooseHttpServerResponse.h"
#include "MongooseHttpServerResponseBasic.h"
#include "MongooseHttpServerResponseStream.h"

// Make a copy of the HTTP header so it is avalible outside of the onReceive
// callback. Setting to 0 will save some runtime memory but accessing the HTTP
// message details outside of the onReceive callback will give undefined behaviour.
// The body may not allways be avalible even in onReceive, eg file upload
#ifndef MG_COPY_HTTP_MESSAGE
#define MG_COPY_HTTP_MESSAGE 1
#endif

class MongooseHttpServerEndpoint;

 /**
  * @brief Represents an incoming HTTP request
  */ 
class MongooseHttpServerRequest : public MongooseHttpServerConnection, public MongooseHttpMessage
{
  private:
    void handlePoll(mg_connection *nc);
    void handleSend(mg_connection *nc, int num_bytes) {
      handlePoll(nc);
    }
    void handleClose(mg_connection *nc);
    void handleMessage(mg_connection *nc, mg_http_message *msg);

  protected:
    HttpRequestMethodComposite _method;
    MongooseHttpServerResponse *_response;
    MongooseHttpServerEndpoint *_endpoint;
    bool _responseSent;

    void sendBody();

#if MG_COPY_HTTP_MESSAGE
    mg_http_message *duplicateMessage(mg_http_message *);
#endif

  public:
    MongooseHttpServerRequest(mg_connection *nc, HttpRequestMethodComposite method, mg_http_message *msg, MongooseHttpServerEndpoint *endpoint);
    virtual ~MongooseHttpServerRequest();

    virtual bool isUpload() { return false; }
    virtual bool isWebSocket() { return false; }

    HttpRequestMethodComposite method() {
      return _method;
    }

    size_t contentLength() {
      return _msg->body.len;
    }

    /**
     * @brief Redirect the client to a different URL
     * @param url The target URL
     */
    void redirect(const char *url);
#ifdef ARDUINO
    void redirect(const String& url);
#endif

    /**
     * @brief Begin a basic HTTP response
     * @return MongooseHttpServerResponseBasic* Response object to populate
     */
    MongooseHttpServerResponseBasic *beginResponse();

#ifdef ARDUINO
    /**
     * @brief Begin a streamed HTTP response (chunked)
     * @return MongooseHttpServerResponseStream* Stream response object
     */
    MongooseHttpServerResponseStream *beginResponseStream();
#endif

    // Takes ownership of `response`, will delete when finished. Do not use `response` after calling
    void send(MongooseHttpServerResponse *response);
    bool responseSent() {
      return NULL != _response;
    }

    /**
     * @brief Send a simple HTTP status code response
     * @param code HTTP status code (e.g. 200, 404)
     */
    void send(int code);
    /**
     * @brief Send a simple HTTP response with content
     * @param code HTTP status code
     * @param contentType MIME type of the content
     * @param content Body payload
     */
    void send(int code, const char *contentType, const char *content="");
#ifdef ARDUINO
    void send(int code, const String& contentType, const String& content=String());
#endif

    /**
     * @brief Check if a GET/POST parameter exists
     * @param name The parameter name
     * @return true if parameter exists
     */
    bool hasParam(const char *name) const;
#ifdef ARDUINO
    bool hasParam(const String& name) const;
    bool hasParam(const __FlashStringHelper * data) const;
#endif

    /**
     * @brief Get the value of a GET/POST parameter
     * @param name The parameter name
     * @param dst Buffer to store the value
     * @param dst_len Size of the destination buffer
     * @return length of the extracted parameter
     */
    int getParam(const char *name, char *dst, size_t dst_len) const;
#ifdef ARDUINO
    int getParam(const String& name, char *dst, size_t dst_len) const;
    int getParam(const __FlashStringHelper * data, char *dst, size_t dst_len) const;
#endif

#ifdef ARDUINO
    String getParam(const char *name) const;
    String getParam(const String& name) const;
    String getParam(const __FlashStringHelper * data) const;
#endif

    /**
     * @brief Perform Basic Authentication against provided credentials
     * @param username Expected username
     * @param password Expected password
     * @return true if credentials match
     */
    bool authenticate(const char * username, const char * password);
#ifdef ARDUINO
    bool authenticate(const String& username, const String& password) {
      return authenticate(username.c_str(), password.c_str());
    }
#endif
    /**
     * @brief Respond with a 401 Unauthorized requesting Basic Authentication
     * @param realm The authentication realm to display
     */
    void requestAuthentication(const char* realm);
#ifdef ARDUINO
    void requestAuthentication(const String& realm) {
      requestAuthentication(realm.c_str());
    }
#endif
};

#endif /* MongooseHttpServerRequest_h */
