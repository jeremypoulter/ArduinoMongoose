
#include "MongooseCore.h"

// The IDF-backed .local resolver below is for real ESP32 targets; EpoxyDuino
// defines ESP32 too but runs on a host whose own mDNS daemon (avahi) shares
// port 5353 happily, so it keeps Mongoose's listener.
#if defined(ESP32) && !defined(EPOXY_DUINO)
#define MONGOOSE_IDF_LOCAL_RESOLVER 1
#endif

#ifdef ARDUINO
#ifdef ESP32
#include <WiFi.h>
#if MONGOOSE_IDF_LOCAL_RESOLVER
#include <mdns.h>
#include <esp_idf_version.h>
#endif
#ifdef ENABLE_WIRED_ETHERNET
#include <ETH.h>
#endif
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#endif // ARDUINO

#if MONGOOSE_IDF_LOCAL_RESOLVER
// .local resolution through ESP-IDF's mDNS component. Mongoose's own listener
// would share UDP 5353 with the IDF responder, and on lwIP that starves the
// responder - the device stops answering for its own name - so on this
// platform Mongoose is pointed at the stack that already owns the port. Each
// lookup is an asynchronous IDF query, polled from MongooseCore::poll().
#define LOCAL_RESOLVER_MAX_PENDING 4
#define LOCAL_RESOLVER_TIMEOUT_MS 2000

struct LocalLookup {
  struct mg_connection *c;      // NULL: slot free
  mdns_search_once_t *search;
  char name[64];                // as asked for, .local included
};
static LocalLookup s_lookups[LOCAL_RESOLVER_MAX_PENDING];

static void localResolveStart(struct mg_connection *c, struct mg_str host)
{
  // IDF wants the name without .local
  size_t base = host.len > 6 ? host.len - 6 : 0;
  char name[64];
  if(base == 0 || base >= sizeof(name) || host.len >= sizeof(s_lookups[0].name)) {
    mg_error(c, "mDNS name too long");
    return;
  }
  memcpy(name, host.buf, base);
  name[base] = '\0';

  LocalLookup *slot = NULL;
  for(int i = 0; i < LOCAL_RESOLVER_MAX_PENDING; i++) {
    if(s_lookups[i].c == NULL) {
      slot = &s_lookups[i];
      break;
    }
  }
  if(slot == NULL) {
    mg_error(c, "mDNS busy");
    return;
  }

  slot->search = mdns_query_async_new(name, NULL, NULL, MDNS_TYPE_A,
                                      LOCAL_RESOLVER_TIMEOUT_MS, 1, NULL);
  if(slot->search == NULL) {
    // mDNS not started (MDNS.begin() not called yet), or out of memory
    mg_error(c, "mDNS query failed");
    return;
  }
  memcpy(slot->name, host.buf, host.len);
  slot->name[host.len] = '\0';
  slot->c = c;
}

static void localResolveCancel(struct mg_connection *c)
{
  for(int i = 0; i < LOCAL_RESOLVER_MAX_PENDING; i++) {
    if(s_lookups[i].c == c) {
      mdns_query_async_delete(s_lookups[i].search);
      s_lookups[i].search = NULL;
      s_lookups[i].c = NULL;
    }
  }
}

static void localResolvePoll()
{
  for(int i = 0; i < LOCAL_RESOLVER_MAX_PENDING; i++) {
    LocalLookup *slot = &s_lookups[i];
    if(slot->c == NULL) {
      continue;
    }
    mdns_result_t *results = NULL;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    uint8_t num = 0;
    if(!mdns_query_async_get_results(slot->search, 0, &results, &num)) {
      continue;  // still running
    }
#else
    if(!mdns_query_async_get_results(slot->search, 0, &results)) {
      continue;  // still running
    }
#endif

    struct mg_connection *c = slot->c;
    bool found = false;
    for(mdns_result_t *r = results; r != NULL && !found; r = r->next) {
      for(mdns_ip_addr_t *a = r->addr; a != NULL; a = a->next) {
        if(a->addr.type == ESP_IPADDR_TYPE_V4) {
          struct mg_addr addr;
          memset(&addr, 0, sizeof(addr));
          addr.is_ip6 = false;
          memcpy(addr.addr.ip, &a->addr.u_addr.ip4.addr, 4);
          mg_resolver_cache_put(mg_str(slot->name), &addr, r->ttl);
          addr.port = c->rem.port;
          c->rem = addr;
          found = true;
          break;
        }
      }
    }
    mdns_query_results_free(results);
    mdns_query_async_delete(slot->search);
    slot->search = NULL;
    slot->c = NULL;

    if(found) {
      mg_connect_resolved(c);
    } else {
      mg_error(c, "%s mDNS lookup failed", slot->name);
    }
  }
}
#endif // MONGOOSE_IDF_LOCAL_RESOLVER

MongooseCore::MongooseCore() : 
  _rootCa(ARDUINO_MONGOOSE_DEFAULT_ROOT_CA),
  _rootCaCallback([this]() -> const char * { return _rootCa; }),
#ifdef ARDUINO
  _nameserver(""),
#endif
  mgr({0})
{
}

void MongooseCore::begin() 
{
  mg_mgr_init(&mgr);

  ipConfigChanged();
}

void MongooseCore::end() 
{
  mg_mgr_free(&mgr);
}

void MongooseCore::poll(int timeout_ms) 
{
  mg_mgr_poll(&mgr, timeout_ms);
#if MONGOOSE_IDF_LOCAL_RESOLVER
  localResolvePoll();
#endif
}

struct mg_mgr *MongooseCore::getMgr()
{
  return &mgr;
}

void MongooseCore::ipConfigChanged() 
{
#ifdef ARDUINO
#if defined(ESP32) || defined(ESP8266)
  IPAddress dns = WiFi.dnsIP(0);
#if defined(ESP32) && defined(ENABLE_WIRED_ETHERNET)
  if(0 == dns) {
    dns = ETH.dnsIP(0);
  }
#endif
  snprintf(_nameserver, sizeof(_nameserver), "udp://%s:53", dns.toString().c_str());
  mgr.dns4.url = _nameserver;
#endif
#endif // ARDUINO

  ensureMdnsResolver();
}

void MongooseCore::ensureMdnsResolver()
{
  // Mongoose 7 resolves *.local names only through an mDNS listener of its
  // own (mg_resolve -> sendmdnsreq): without one, every connect to a .local
  // host fails outright with "no mDNS listener".
#if MONGOOSE_IDF_LOCAL_RESOLVER
  // The IDF mDNS responder owns 5353 here; hand lookups to it (see the top
  // of this file) rather than open a second socket on the port.
  mg_set_local_resolver(localResolveStart, localResolveCancel);
#else
  // Open a listener with no name registered, so it answers nothing - the
  // platform responder keeps that job, and Mdns.begin() supersedes this one
  // if the sketch advertises through Mongoose. Done here rather than in
  // begin() because the multicast join binds to the interface, which has to
  // be up first.
  if(mgr.mdns != NULL) {
    return;
  }
  if(NULL == mg_mdns_listen(&mgr, NULL, NULL)) {
    MG_ERROR(("mDNS resolver listener failed"));
  }
#endif
}

MongooseCore Mongoose;
