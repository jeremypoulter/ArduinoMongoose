#ifndef MongooseCore_h
#define MongooseCore_h

#include "MongoosePlatform.h"

#include "mongoose.h"

#include <functional>

#ifndef ARDUINO_MONGOOSE_DEFAULT_ROOT_CA
#define ARDUINO_MONGOOSE_DEFAULT_ROOT_CA ""
#endif

typedef std::function<const char *(void)> ArduinoMongooseGetRootCaCallback;
typedef mg_random_fn_t ArduinoMongooseRandomCallback;

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

    // Replacement for older mg_ssl_if_mbed_random()-style hooks.
    ArduinoMongooseRandomCallback getRandomCallback() {
      return mg_random_get();
    }

    void setRandomCallback(ArduinoMongooseRandomCallback callback) {
      mg_random_set(callback);
    }

    bool fillRandom(void *buf, size_t len) {
      return mg_random(buf, len);
    }
};

extern MongooseCore Mongoose;

#endif // MongooseCore_h
