#ifndef MongooseCore_h
#define MongooseCore_h

#include "MongoosePlatform.h"

#include "mongoose.h"

#include <functional>

#ifndef ARDUINO_MONGOOSE_DEFAULT_ROOT_CA
#define ARDUINO_MONGOOSE_DEFAULT_ROOT_CA ""
#endif

typedef std::function<const char *(void)> ArduinoMongooseGetRootCaCallback;

/** @brief Enough for the longest IPv6 text form plus its terminator */
#define MONGOOSE_ADDRESS_LEN 46

/**
 * @brief How many DNS servers MongooseCore tracks for failover
 *
 * lwIP's own resolver keeps DNS_MAX_SERVERS (3 on ESP32), and DHCP commonly
 * hands out two, so three covers the platforms this runs on with room for a
 * statically configured extra. Override it if you need more.
 */
#ifndef MONGOOSE_NAMESERVERS
#define MONGOOSE_NAMESERVERS 3
#endif
/** @brief Room for "udp://[<ipv6>]:53" plus terminator */
#define MONGOOSE_NAMESERVER_LEN (MONGOOSE_ADDRESS_LEN + 12)

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
    // DHCP hands out more than one DNS server; Mongoose's resolver only talks
    // to one (mgr.dns4). Keep them all so a resolve timeout can fail over.
    char _nameserver[MONGOOSE_NAMESERVERS][MONGOOSE_NAMESERVER_LEN];
    char _active[MONGOOSE_NAMESERVER_LEN];  // what mgr.dns4.url points at
    int _nameserverCount;
    int _activeNameserver;
    uint64_t _lastFailover;
    struct mg_mgr mgr;

    void useNameserver(int index);
    void closeResolver();

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
     * @brief Set the DNS servers the resolver may use
     *
     * The first usable entry becomes active immediately; the rest are only
     * used after a resolve timeout against the active one, see dnsError(),
     * which rotates through them in order. Any DNS connection to a previous
     * server is closed so the next lookup goes to the new one.
     * ipConfigChanged() calls this with the DHCP-supplied servers; call it
     * directly on platforms without WiFi/ETH.
     *
     * NULL and empty entries are skipped, so a partly filled array is fine.
     * At most MONGOOSE_NAMESERVERS are kept and the rest ignored. With no
     * usable entry the resolver keeps the URL it already has (the
     * mg_mgr_init() default, or the last server set).
     *
     * The strings are copied; the array need not outlive the call.
     *
     * @param servers DNS server URLs, e.g. "udp://192.168.1.1:53"
     * @param count How many entries @p servers has
     */
    void setNameservers(const char *const *servers, size_t count);

    /**
     * @brief Set a single DNS server, with no failover
     *
     * @param server DNS server URL, or NULL to keep the current one
     */
    void setNameserver(const char *server);

    /**
     * @brief Get the DNS server currently used for lookups
     *
     * @return const char* Server URL
     */
    const char *nameserver() const;

    /**
     * @brief Report a socket error so DNS failover can act on it
     *
     * Called by MongooseSocket for every MG_EV_ERROR. Only a resolve timeout
     * rotates to the other server: a lookup that *answered* NXDOMAIN is a
     * real answer, and a closed DNS connection already fails every lookup
     * that was in flight. Timeouts reported within one DNS timeout of the
     * last rotation were issued against the server that has already been
     * rotated away from, and are ignored so a burst of stalled lookups moves
     * the active server once, not once per lookup.
     *
     * @param error The MG_EV_ERROR message
     * @return true if the active DNS server changed
     */
    bool dnsError(const char *error);

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
