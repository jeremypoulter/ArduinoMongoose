
#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_CORE)
#undef ENABLE_DEBUG
#endif

#include <string.h>

#include <MicroDebug.h>

#include "MongooseCore.h"

#ifdef ARDUINO
#ifdef ESP32
#include <WiFi.h>
#ifdef ENABLE_WIRED_ETHERNET
#include <ETH.h>
#endif
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#endif // ARDUINO

MongooseCore::MongooseCore() : 
  _rootCa(ARDUINO_MONGOOSE_DEFAULT_ROOT_CA),
  _rootCaCallback([this]() -> const char * { return _rootCa; }),
  _nameserverCount(0),
  _activeNameserver(0),
  _lastFailover(0),
  mgr({0})
{
  memset(_nameserver, 0, sizeof(_nameserver));
  _active[0] = '\0';
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
}

struct mg_mgr *MongooseCore::getMgr()
{
  return &mgr;
}

void MongooseCore::ipConfigChanged() 
{
#ifdef ARDUINO
#if defined(ESP32) || defined(ESP8266)
  IPAddress dns[MONGOOSE_NAMESERVERS];
  for(int i = 0; i < MONGOOSE_NAMESERVERS; i++) {
    dns[i] = WiFi.dnsIP(i);
  }
#if defined(ESP32) && defined(ENABLE_WIRED_ETHERNET)
  if(0 == dns[0]) {
    for(int i = 0; i < MONGOOSE_NAMESERVERS; i++) {
      dns[i] = ETH.dnsIP(i);
    }
  }
#endif
  char url[MONGOOSE_NAMESERVERS][MONGOOSE_NAMESERVER_LEN];
  const char *urls[MONGOOSE_NAMESERVERS] = {nullptr};
  int n = 0;
  for(int i = 0; i < MONGOOSE_NAMESERVERS; i++) {
    if(0 == dns[i] || (n > 0 && dns[i] == dns[0])) {
      continue;
    }
    snprintf(url[n], sizeof(url[n]), "udp://%s:53", dns[i].toString().c_str());
    urls[n] = url[n];
    n++;
  }
  setNameservers(urls[0], urls[1]);
#endif
#endif // ARDUINO
}

void MongooseCore::setNameservers(const char *primary, const char *secondary)
{
  const char *urls[MONGOOSE_NAMESERVERS] = {primary, secondary};
  _nameserverCount = 0;
  for(int i = 0; i < MONGOOSE_NAMESERVERS; i++) {
    if(urls[i] && urls[i][0]) {
      snprintf(_nameserver[_nameserverCount], MONGOOSE_NAMESERVER_LEN, "%s", urls[i]);
      _nameserverCount++;
    }
  }
  _lastFailover = 0;
  useNameserver(0);
}

void MongooseCore::useNameserver(int index)
{
  _activeNameserver = index;
  if(index >= _nameserverCount) {
    // Nothing configured: keep whatever mgr.dns4.url already holds (the
    // mg_mgr_init() default, or the last server we set). A NULL url would
    // make the next lookup's mg_dnsc_init() call mg_error(0, ...) and crash.
    return;
  }
  // The active URL has its own buffer rather than pointing into the table:
  // setNameservers() rewrites the table in place, so a pointer compare could
  // never see a changed primary.
  if(0 != strcmp(_active, _nameserver[index])) {
    closeResolver();
    snprintf(_active, sizeof(_active), "%s", _nameserver[index]);
  }
  mgr.dns4.url = _active;
}

void MongooseCore::closeResolver()
{
  if(nullptr == mgr.dns4.c) {
    return;
  }
  // mg_dnsc_init() only connects while dns4.c is NULL, so an established
  // resolver socket would keep talking to the old server for ever. Closing
  // it also errors every lookup still in flight ("DNS error"), which is
  // right: they were all waiting on the server being retired. A lookup that
  // has already failed (the timeout that brought us here) must not be
  // errored a second time, so drop its request first.
  for(struct mg_connection *c = mgr.conns; c != nullptr; c = c->next) {
    if(c->is_closing) {
      mg_resolve_cancel(c);
    }
  }
  mgr.dns4.c->is_closing = 1;
  mgr.dns4.c = nullptr;
}

const char *MongooseCore::nameserver() const
{
  return mgr.dns4.url;
}

bool MongooseCore::dnsError(const char *error)
{
  if(_nameserverCount < 2 || nullptr == error || 0 != strcmp(error, "DNS timeout")) {
    return false;
  }
  uint64_t now = mg_millis();
  if(_lastFailover != 0 && now - _lastFailover < (uint64_t)mgr.dnstimeout) {
    return false;
  }
  _lastFailover = now;
  useNameserver((_activeNameserver + 1) % _nameserverCount);
  DBUGF("DNS timeout, switching nameserver to %s", mgr.dns4.url);
  return true;
}

MongooseCore Mongoose;
