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

// Enough for the longest IPv6 text form plus its terminator
#define MONGOOSE_ADDRESS_LEN 46

/*
 * Format the address a connection's peer was resolved to, into buf.
 *
 * Mongoose resolves the peer asynchronously and stores the answer on the
 * connection, so this needs no DNS query of its own -- which matters because
 * the synchronous resolvers block, and on a single-threaded application task
 * under a watchdog that is a reboot rather than a delay.
 *
 * Reads nc->sa rather than going through mg_conn_addr_to_str(), which asks the
 * socket layer for the peer and so answers only once the socket is connected;
 * nc->sa holds the address as soon as resolution completes.
 *
 * Writes an empty string and returns false when there is no resolved peer:
 * no connection, resolution still in flight, or resolution failed.
 */
bool mongooseRemoteAddress(struct mg_connection *nc, char *buf, size_t len);

class MongooseCore
{
  private:
#if MG_ENABLE_SSL
    const char *_rootCa;
    ArduinoMongooseGetRootCaCallback _rootCaCallback;
#endif
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
    void getDefaultOpts(struct mg_connect_opts *opts, bool secure = false);

    void ipConfigChanged();

#if MG_ENABLE_SSL
    void setRootCa(const char *rootCa) {
      _rootCa = rootCa;
    }

    void setRootCaCallback(ArduinoMongooseGetRootCaCallback callback) {
      _rootCaCallback = callback;
    }
#endif

};

extern MongooseCore Mongoose;

#endif // MongooseCore_h
