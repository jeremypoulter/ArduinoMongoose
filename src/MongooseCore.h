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
    /**
     * @brief Construct a new Mongoose Core object
     */
    MongooseCore();

    /**
     * @brief Initialize the Mongoose event manager
     */
    void begin();

    /**
     * @brief Stop the Mongoose event manager and clean up
     */
    void end();

    /**
     * @brief Poll the networking subsystem for events
     * 
     * @param timeout_ms Maximum time to block waiting for events, in milliseconds
     */
    void poll(int timeout_ms);

    /**
     * @brief Get the underlying Mongoose mg_mgr struct
     * 
     * @return struct mg_mgr* Pointer to the Mongoose manager
     */
    struct mg_mgr *getMgr();

    /**
     * @brief Re-evaluates IP configuration when network interfaces change
     */
    void ipConfigChanged();

    /**
     * @brief Get the currently configured Root CA certificate
     * 
     * @return mg_str The Root CA as a Mongoose string
     */
    mg_str getRootCa() {
      return mg_str_s(_rootCaCallback());
    }

    /**
     * @brief Set the Root CA certificate directly
     * 
     * @param rootCa Null-terminated string containing the Root CA PEM
     */
    void setRootCa(const char *rootCa) {
      _rootCa = rootCa;
    }

    /**
     * @brief Set a callback function to retrieve the Root CA
     * 
     * @param callback Function returning the Root CA string
     */
    void setRootCaCallback(ArduinoMongooseGetRootCaCallback callback) {
      _rootCaCallback = callback;
    }
};

extern MongooseCore Mongoose;

#endif // MongooseCore_h
