#ifndef MongooseHttpServerRequest_h
#define MongooseHttpServerRequest_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include "MongooseHttpServerConnection.h"
#include "MongooseHttpServerResponse.h"
#include "MongooseHttpServerResponseBasic.h"

// Make a copy of the HTTP header so it is avalible outside of the onReceive
// callback. Setting to 0 will save some runtime memory but accessing the HTTP
// message details outside of the onReceive callback will give undefined behaviour.
// The body may not allways be avalible even in onReceive, eg file upload
#ifndef MG_COPY_HTTP_MESSAGE
#define MG_COPY_HTTP_MESSAGE 1
#endif

class MongooseHttpServerEndpoint;

class MongooseHttpServerRequest : public MongooseHttpServerConnection
{
  private:
    void onPoll(mg_connection *nc);
    void onSend(mg_connection *nc, int num_bytes) {
      onPoll(nc);
    }
    void onClose(mg_connection *nc);
    void onMessage(mg_connection *nc, mg_http_message *msg);

  protected:
    mg_http_message *_msg;
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

//    int respCode() {
//      return _msg->resp_code;
//    }
//    MongooseString respStatusMsg() {
//      return MongooseString(_msg->resp_status_msg);
//    }

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

    size_t contentLength() {
      return _msg->body.len;
    }

    void redirect(const char *url);
#ifdef ARDUINO
    void redirect(const String& url);
#endif

    MongooseHttpServerResponseBasic *beginResponse();

#ifdef ARDUINO
    MongooseHttpServerResponseStream *beginResponseStream();
#endif

    // Takes ownership of `response`, will delete when finished. Do not use `response` after calling
    void send(MongooseHttpServerResponse *response);
    bool responseSent() {
      return NULL != _response;
    }

    void send(int code);
    void send(int code, const char *contentType, const char *content="");
#ifdef ARDUINO
    void send(int code, const String& contentType, const String& content=String());
#endif

    bool hasParam(const char *name) const;
#ifdef ARDUINO
    bool hasParam(const String& name) const;
    bool hasParam(const __FlashStringHelper * data) const;
#endif

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

    bool authenticate(const char * username, const char * password);
#ifdef ARDUINO
    bool authenticate(const String& username, const String& password) {
      return authenticate(username.c_str(), password.c_str());
    }
#endif
    void requestAuthentication(const char* realm);
#ifdef ARDUINO
    void requestAuthentication(const String& realm) {
      requestAuthentication(realm.c_str());
    }
#endif
};

#endif /* MongooseHttpServerRequest_h */
