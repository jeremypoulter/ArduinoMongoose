
#include "MongooseCore.h"

#ifdef ARDUINO
#ifdef ESP32
#include <WiFi.h>
#include <esp_netif.h>
#ifdef ENABLE_WIRED_ETHERNET
#include <ETH.h>
#endif
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#endif // ARDUINO

MongooseCore::MongooseCore() : 
#if MG_ENABLE_SSL
  _rootCa(ARDUINO_MONGOOSE_DEFAULT_ROOT_CA),
  _rootCaCallback([this]() -> const char * { return _rootCa; }),
#endif
#ifdef ARDUINO
  _nameserver(""),
#endif
  mgr({0})
{
}

void MongooseCore::begin() 
{
  mg_mgr_init(&mgr, this);

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

void MongooseCore::getDefaultOpts(struct mg_connect_opts *opts, bool secure)
{
  memset(opts, 0, sizeof(*opts));

#if MG_ENABLE_SSL
  if(secure) {
    opts->ssl_ca_cert = _rootCaCallback();
  }
#else
  (void)secure;
#endif
}


void MongooseCore::ipConfigChanged()
{
#ifdef ARDUINO
#if defined(ESP32) || defined(ESP8266)
  /* WiFi.dnsIP(0) is unreliable on ESP32 Arduino v2.x: when RDNSS sets an IPv6
   * DNS server in LwIP slot 0, dnsIP() reads the first 4 bytes as IPv4, producing
   * garbage like 253.0.0.0 (fd00::... truncated). Use esp_netif_get_dns_info()
   * directly with explicit type checking instead. Prefer IPv4 DNS; fall back to
   * IPv6 RDNSS only if no valid IPv4 DNS is found. */
#if defined(ESP32)
  const char *netif_keys[] = {
    "WIFI_STA_DEF",
#if defined(ENABLE_WIRED_ETHERNET)
    "ETH_DEF",
#endif
    nullptr
  };

  /* Pass 1: look for a valid IPv4 DNS server */
  for(int k = 0; netif_keys[k]; k++) {
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey(netif_keys[k]);
    if(!netif) continue;
    for(int i = 0; i < 2; i++) {
      esp_netif_dns_info_t info;
      if(esp_netif_get_dns_info(netif, (esp_netif_dns_type_t)i, &info) != ESP_OK) continue;
      if(info.ip.type == ESP_IPADDR_TYPE_V4 && info.ip.u_addr.ip4.addr != 0) {
        IPAddress dns(info.ip.u_addr.ip4.addr);
        _nameserver = dns.toString();
        mg_set_nameserver(&mgr, _nameserver.c_str());
        Serial.printf("Mongoose DNS nameserver set to: %s\r\n", _nameserver.c_str());
        return;
      }
    }
  }

  /* Pass 2: no IPv4 DNS — look for IPv6 DNS from RDNSS */
  for(int k = 0; netif_keys[k]; k++) {
    esp_netif_t *netif = esp_netif_get_handle_from_ifkey(netif_keys[k]);
    if(!netif) continue;
    for(int i = 0; i < 2; i++) {
      esp_netif_dns_info_t info;
      if(esp_netif_get_dns_info(netif, (esp_netif_dns_type_t)i, &info) != ESP_OK) continue;
      if(info.ip.type == ESP_IPADDR_TYPE_V6) {
        char buf[64];
        /* The IPAddress/IPv6Address and its toString() String are temporaries;
         * their buffers live to the end of this full-expression, so the .c_str()
         * pointer is valid for the snprintf call. Keep this as one statement —
         * hoisting .c_str() to its own line would dangle after the String dies. */
        snprintf(buf, sizeof(buf), "[%s]",
#if defined(ESP_ARDUINO_VERSION) && ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
          // Arduino-ESP32 3.x unified v4/v6 into IPAddress; IPv6Address was removed.
          IPAddress(IPType::IPv6, (const uint8_t *)info.ip.u_addr.ip6.addr).toString().c_str());
#else
          IPv6Address(info.ip.u_addr.ip6.addr).toString().c_str());
#endif
        _nameserver = buf;
        mg_set_nameserver(&mgr, _nameserver.c_str());
        Serial.printf("Mongoose DNS nameserver set to: %s\r\n", _nameserver.c_str());
        return;
      }
    }
  }
#else
  /* ESP8266: no RDNSS, WiFi.dnsIP() is reliable */
  IPAddress dns = WiFi.dnsIP(0);
  if(dns) {
    _nameserver = dns.toString();
    mg_set_nameserver(&mgr, _nameserver.c_str());
  }
#endif
#endif
#endif // ARDUINO
}

MongooseCore Mongoose;
