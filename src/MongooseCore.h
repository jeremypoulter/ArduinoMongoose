#ifndef MongooseCore_h
#define MongooseCore_h

#include "MongoosePlatform.h"

#include "mongoose.h"

#include <functional>

#ifndef ARDUINO_MONGOOSE_DEFAULT_ROOT_CA
#define ARDUINO_MONGOOSE_DEFAULT_ROOT_CA ""
#endif

typedef std::function<const char *(void)> ArduinoMongooseGetRootCaCallback;

/**
 * @brief Core Mongoose manager for Arduino
 * 
 * Provides an initialization point and event loop wrapper for Mongoose.
 */
class MongooseCore
{
  private:
    const char *_rootCa;
    ArduinoMongooseGetRootCaCallback _rootCaCallback;
#ifdef ARDUINO
    char _nameserver[32];
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
