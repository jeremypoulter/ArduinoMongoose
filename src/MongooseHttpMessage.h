#ifndef MongooseHttpMessage_h
#define MongooseHttpMessage_h

#include "MongoosePlatform.h"
#include "MongooseString.h"
#include <mongoose.h>

// Shared HTTP message accessors used by both MongooseHttpServerRequest and
// MongooseHttpClientResponse.
class MongooseHttpMessage {
  protected:
    mg_http_message *_msg;

    MongooseHttpMessage(mg_http_message *msg) :
      _msg(msg)
    {
    }

  public:
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
      return MongooseString(mg_http_get_header(_msg, name));
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
};

#endif // MongooseHttpMessage_h
