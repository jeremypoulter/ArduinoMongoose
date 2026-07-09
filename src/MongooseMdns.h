#ifndef MongooseMdns_h
#define MongooseMdns_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"

// Forward declarations
class MongooseMdns;
struct MongooseMdnsRequest;

// Callback for incoming mDNS query events (advanced use)
typedef std::function<void(MongooseMdnsRequest *req)> MongooseMdnsRequestHandler;

/**
 * @brief Represents an incoming mDNS query, passed to the onRequest handler.
 *
 * The handler may call respond() to send a reply using the registered service
 * record for the matched service, or leave the request unhandled.
 */
struct MongooseMdnsRequest
{
  struct mg_mdns_req *_req;   // raw mongoose request

  MongooseString name() const {
    return MongooseString(_req->reqname);
  }

  bool isListing() const {
    return _req->is_listing;
  }

  // Call this to trigger an mDNS response for this query.
  // Set r to a mg_dnssd_record describing the service before calling.
  void respond(struct mg_dnssd_record *r = nullptr) {
    if (r) _req->r = r;
    _req->is_resp = true;
  }
};


/**
 * @brief Arduino-style mDNS wrapper using Mongoose's built-in mDNS API.
 *
 * Allows a device to advertise its hostname and services on the local network
 * using multicast DNS (RFC-6762 / DNS-SD, RFC-6763).
 *
 * Usage:
 *   MongooseMdns mdns;
 *   mdns.begin("mydevice");              // Advertises mydevice.local
 *   mdns.addService("_http._tcp", 80);   // Advertises an HTTP service
 *
 *   // In loop():
 *   Mongoose.poll(0);
 */
class MongooseMdns
{
  public:
    // Maximum number of services that can be registered
    static const int MAX_SERVICES = 8;

    // A registered DNS-SD service record
    struct ServiceRecord {
      char srvcproto[64];  // e.g. "_http._tcp"
      char txt[256];       // TXT record content (verbatim)
      uint16_t port;       // TCP/UDP port
    };

  private:
    struct mg_connection *_mdns;
    char *_hostname;

    ServiceRecord _services[MAX_SERVICES];
    int _numServices;

    MongooseMdnsRequestHandler _onRequest;

    static void eventHandler(struct mg_connection *nc, int ev, void *ev_data);
    void handleReq(struct mg_connection *nc, struct mg_mdns_req *req);

  public:
    MongooseMdns();
    ~MongooseMdns();

    /**
     * @brief Start the mDNS listener and advertise the given hostname.
     *
     * Advertises <hostname>.local on the network. Must be called after
     * Mongoose.begin() and after network connectivity is established.
     *
     * @param hostname  Hostname without the .local suffix (e.g. "mydevice")
     * @return true if the listener was started successfully
     */
    bool begin(const char *hostname);

#ifdef ARDUINO
    bool begin(const String &hostname) {
      return begin(hostname.c_str());
    }
#endif

    /**
     * @brief Stop the mDNS listener and free resources.
     */
    void end();

    /**
     * @brief Register a DNS-SD service to advertise.
     *
     * Example: addService("_http._tcp", 80);
     * Example: addService("_mqtt._tcp", 1883, "version=3");
     *
     * @param srvcproto  Service type and protocol label, e.g. "_http._tcp"
     * @param port       TCP/UDP port
     * @param txt        Optional TXT record content (verbatim, max 255 bytes)
     * @return true if the service was registered (false if MAX_SERVICES reached)
     */
    bool addService(const char *srvcproto, uint16_t port, const char *txt = "");

    /**
     * @brief Convenience overload accepting separate protocol and transport.
     *
     * Example: addService("_http", "_tcp", 80);
     *
     * @param protocol   Service label, e.g. "_http"
     * @param transport  Transport label, e.g. "_tcp" or "_udp"
     * @param port       TCP/UDP port
     * @param txt        Optional TXT record content
     * @return true if the service was registered
     */
    bool addService(const char *protocol, const char *transport, uint16_t port, const char *txt = "");

#ifdef ARDUINO
    bool addService(const String &srvcproto, uint16_t port, const String &txt = String()) {
      return addService(srvcproto.c_str(), port, txt.length() > 0 ? txt.c_str() : "");
    }

    bool addService(const String &protocol, const String &transport, uint16_t port, const String &txt = String()) {
      return addService(protocol.c_str(), transport.c_str(), port, txt.length() > 0 ? txt.c_str() : "");
    }
#endif

    /**
     * @brief Remove a previously registered DNS-SD service.
     *
     * @param srvcproto  Service type and protocol label, e.g. "_http._tcp"
     * @return true if the service was found and removed, false if not found
     */
    bool removeService(const char *srvcproto);

    /**
     * @brief Convenience overload accepting separate protocol and transport.
     *
     * @param protocol   Service label, e.g. "_http"
     * @param transport  Transport label, e.g. "_tcp" or "_udp"
     * @return true if the service was found and removed
     */
    bool removeService(const char *protocol, const char *transport);

#ifdef ARDUINO
    bool removeService(const String &srvcproto) {
      return removeService(srvcproto.c_str());
    }

    bool removeService(const String &protocol, const String &transport) {
      return removeService(protocol.c_str(), transport.c_str());
    }
#endif

    /**
     * @brief Send an mDNS query for the given hostname.
     *
     * The mDNS listener must already be started with begin().
     * Responses are delivered via MG_EV_MDNS_RESP to Mongoose resolver.
     *
     * @param name   Hostname to query (without .local)
     * @param rtype  DNS record type (default: MG_DNS_RTYPE_A for IPv4)
     * @return true if the query was sent successfully
     */
    bool query(const char *name, unsigned int rtype = MG_DNS_RTYPE_A);

#ifdef ARDUINO
    bool query(const String &name, unsigned int rtype = MG_DNS_RTYPE_A) {
      return query(name.c_str(), rtype);
    }
#endif

    /**
     * @brief Register a handler for incoming mDNS requests.
     *
     * The handler is called for every incoming mDNS query that is not
     * automatically handled (i.e., PTR/SRV/TXT queries not matched by a
     * registered service). The handler may call req->respond() to reply.
     *
     * @param handler  Callback receiving a MongooseMdnsRequest pointer
     */
    void onRequest(MongooseMdnsRequestHandler handler) {
      _onRequest = handler;
    }

    /**
     * @brief Check if the mDNS listener is active.
     */
    bool isActive() const {
      return _mdns != nullptr;
    }

    /**
     * @brief Get the current hostname (without .local suffix).
     * Returns nullptr if begin() has not been called.
     */
    const char *hostname() const {
      return _hostname;
    }

    /**
     * @brief Get the number of currently registered services.
     */
    int numServices() const {
      return _numServices;
    }

    /**
     * @brief Get a registered service record by index.
     * @param index  Zero-based index; must be < numServices()
     */
    const ServiceRecord &getService(int index) const {
      return _services[index];
    }
};

extern MongooseMdns Mdns;

#endif // MongooseMdns_h
