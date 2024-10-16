#ifndef MongooseCore_h
#define MongooseCore_h

#ifdef ARDUINO
#include <Arduino.h>
#include <IPAddress.h>
#endif // ARDUINO

#include "mongoose.h"

#include <functional>

#ifndef ARDUINO_MONGOOSE_DEFAULT_ROOT_CA
#define ARDUINO_MONGOOSE_DEFAULT_ROOT_CA ""
#endif

typedef std::function<const char *(void)> ArduinoMongooseGetRootCaCallback;

class MongooseCore
{
  private:
    const char *_rootCa;
    ArduinoMongooseGetRootCaCallback _rootCaCallback;
#ifdef ARDUINO
    String _nameserver;
#endif // ARDUINO
    struct mg_mgr mgr;

  public:
    MongooseCore();
    void begin();
    void end();
    void poll(int timeout_ms);

    struct mg_mgr *getMgr();
//    void getDefaultOpts(struct mg_connect_opts *opts, bool secure = false);

    void ipConfigChanged();

    mg_str getRootCa() {
      return mg_str_s(_rootCaCallback());
    }

    void setRootCa(const char *rootCa) {
      _rootCa = rootCa;
    }

    void setRootCaCallback(ArduinoMongooseGetRootCaCallback callback) {
      _rootCaCallback = callback;
    }
};

extern MongooseCore Mongoose;

#endif // MongooseCore_h
