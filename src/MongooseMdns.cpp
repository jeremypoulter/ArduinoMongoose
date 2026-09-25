#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_MDNS)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>

#include <string.h>
#include <stdio.h>
#include <algorithm>

#include "MongooseCore.h"
#include "MongooseMdns.h"

MongooseMdns Mdns;

MongooseMdns::MongooseMdns() :
  _mdns(nullptr),
  _hostname(nullptr),
  _numServices(0),
  _onRequest(nullptr)
{
}

MongooseMdns::~MongooseMdns()
{
  end();
}

// static
void MongooseMdns::eventHandler(struct mg_connection *nc, int ev, void *ev_data)
{
  MongooseMdns *self;
  memcpy(&self, nc->data, sizeof(self));
  if (self && ev == MG_EV_MDNS_REQ) {
    self->handleReq(nc, (struct mg_mdns_req *)ev_data);
  } else if (self && ev == MG_EV_MDNS_RESP) {
    self->handleResponse(*(struct mg_mdns_resp *)ev_data);
  } else if (self && ev == MG_EV_CLOSE) {
    self->_mdns = nullptr;
  } else if (self && ev == MG_EV_POLL) {
    self->pollBrowse();
  }
  (void)ev_data;
}

void MongooseMdns::handleReq(struct mg_connection *nc, struct mg_mdns_req *req)
{
  if (!req) return;

  if (req->is_listing) {
    for (int i = 0; i < _numServices; i++) {
      _listing[i] = {};
      _listing[i].srvcproto = mg_str(_services[i].srvcproto);
    }
    req->listing = _listing;
    req->listing_count = _numServices;
    req->is_resp = _numServices > 0;
    return;
  }

  // A-record queries for our hostname are handled automatically by mongoose
  // (fn_data is used as the hostname string inside handle_mdns_query).
  if (req->rr && req->rr->atype == MG_DNS_RTYPE_A) {
    DBUGF("mDNS: A-record query for '%.*s'", (int)req->reqname.len, req->reqname.buf);
    // mongoose already set is_resp=true when hostname matches fn_data
    return;
  }

  // For PTR/SRV/TXT queries, match against registered services.
  if (req->rr &&
      (req->rr->atype == MG_DNS_RTYPE_PTR ||
       req->rr->atype == MG_DNS_RTYPE_SRV ||
       req->rr->atype == MG_DNS_RTYPE_TXT))
  {
    for (int i = 0; i < _numServices; i++) {
      if (mg_strcasecmp(req->reqname, mg_str(_services[i].srvcproto)) == 0) {
        DBUGF("mDNS: matched service '%s' for query '%.*s'",
              _services[i].srvcproto, (int)req->reqname.len, req->reqname.buf);

        // Build the service record pointing to our storage
        static struct mg_dnssd_record svc;
        svc.srvcproto = mg_str_n(_services[i].srvcproto,
                                 strlen(_services[i].srvcproto));
        svc.txt       = mg_str_n(_services[i].txt, _services[i].txtLength);
        svc.port      = _services[i].port;

        req->r = &svc;
        req->is_resp = true;
        return;
      }
    }
  }

  // Call user-supplied handler for anything not handled above.
  if (_onRequest) {
    MongooseMdnsRequest wrapper;
    wrapper._req = req;
    _onRequest(&wrapper);
  }

  (void)nc;
}

bool MongooseMdns::begin(const char *hostname)
{
  if (!hostname || hostname[0] == '\0' || strlen(hostname) > 63 || strchr(hostname, '.')) {
    return false;
  }

  end();  // Clean up any existing listener

  _hostname = strdup(hostname);
  if (!_hostname) {
    return false;
  }

  _mdns = mg_mdns_listen(Mongoose.getMgr(), eventHandler, _hostname);
  if (!_mdns) {
    free(_hostname);
    _hostname = nullptr;
    return false;
  }

  // fn_data must remain the hostname string (mongoose internals use it for
  // automatic A-record responses). Store 'this' in the connection data slot.
  MongooseMdns *self = this;
  memcpy(_mdns->data, &self, sizeof(self));

  DBUGF("mDNS: started, advertising '%s.local'", _hostname);
  return true;
}

void MongooseMdns::end()
{
  if (_mdns) {
    // Detach synchronously: the connection is freed on the next poll, after
    // this object and its hostname may already have been destroyed.
    _mdns->pfn(_mdns, MG_EV_CLOSE, nullptr);
    _mdns->pfn = nullptr;
    _mdns->fn = nullptr;
    _mdns->fn_data = nullptr;
    _mdns->is_closing = 1;
    _mdns = nullptr;
  }
  if (_hostname) {
    free(_hostname);
    _hostname = nullptr;
  }
  _numServices = 0;
  cancelBrowse();
}

bool MongooseMdns::addService(const char *srvcproto, uint16_t port, const char *txt)
{
  if (!srvcproto || strlen(srvcproto) >= sizeof(_services[0].srvcproto) ||
      _numServices >= MAX_SERVICES || (txt && strlen(txt) > 255)) {
    return false;
  }
  const char *transport = strrchr(srvcproto, '.');
  if (srvcproto[0] != '_' || !transport || transport == srvcproto ||
      (strcmp(transport, "._tcp") && strcmp(transport, "._udp"))) return false;
  for (const char *p = srvcproto; p < transport; p++) if (*p == '.') return false;
  for (int i = 0; i < _numServices; i++)
    if (!mg_casecmp(srvcproto, _services[i].srvcproto)) return false;

  ServiceRecord &svc = _services[_numServices];
  strncpy(svc.srvcproto, srvcproto, sizeof(svc.srvcproto) - 1);
  svc.srvcproto[sizeof(svc.srvcproto) - 1] = '\0';

  size_t len = txt ? strlen(txt) : 0;
  svc.txt[0] = (char)len;
  if (len) memcpy(svc.txt + 1, txt, len);
  svc.txtLength = len + 1;

  svc.port = port;
  _numServices++;

  DBUGF("mDNS: registered service '%s' on port %u", svc.srvcproto, svc.port);
  return true;
}

bool MongooseMdns::addService(const char *protocol, const char *transport, uint16_t port, const char *txt)
{
  if (!protocol || !transport) {
    return false;
  }

  char srvcproto[64];
  snprintf(srvcproto, sizeof(srvcproto), "%s.%s", protocol, transport);
  return addService(srvcproto, port, txt);
}

bool MongooseMdns::removeService(const char *srvcproto)
{
  if (!srvcproto) {
    return false;
  }

  for (int i = 0; i < _numServices; i++) {
    if (strcmp(_services[i].srvcproto, srvcproto) == 0) {
      // Shift remaining entries down
      for (int j = i; j < _numServices - 1; j++) {
        _services[j] = _services[j + 1];
      }
      _numServices--;
      DBUGF("mDNS: removed service '%s'", srvcproto);
      return true;
    }
  }

  return false;
}

bool MongooseMdns::removeService(const char *protocol, const char *transport)
{
  if (!protocol || !transport) {
    return false;
  }

  char srvcproto[64];
  snprintf(srvcproto, sizeof(srvcproto), "%s.%s", protocol, transport);
  return removeService(srvcproto);
}

bool MongooseMdns::query(const char *name, unsigned int rtype)
{
  if (!_mdns || !name) {
    return false;
  }
  std::string fqdn(name);
  if (!fqdn.empty() && fqdn.back() == '.') fqdn.pop_back();
  if (fqdn.size() < 6 || mg_casecmp(fqdn.c_str() + fqdn.size() - 6, ".local") != 0)
    fqdn += ".local";
  return mg_mdns_query(_mdns, fqdn.c_str(), rtype);
}

bool MongooseMdns::addServiceTxt(const char *service, const char *key, const char *value)
{
  if (!service || !key || !*key || strchr(key, '=') || !value) return false;
  std::string entry = std::string(key) + "=" + value;
  if (entry.size() > 255) return false;
  for (int i = 0; i < _numServices; i++) {
    ServiceRecord &svc = _services[i];
    if (mg_casecmp(service, svc.srvcproto)) continue;
    std::string txt;
    for (size_t pos = 0; pos < svc.txtLength;) {
      size_t len = (unsigned char)svc.txt[pos++];
      std::string old(svc.txt + pos, len);
      if (len && mg_casecmp(old.substr(0, old.find('=')).c_str(), key)) {
        txt += (char)len;
        txt += old;
      }
      pos += len;
    }
    txt += (char)entry.size();
    txt += entry;
    if (txt.size() > sizeof(svc.txt)) return false;
    memcpy(svc.txt, txt.data(), txt.size());
    svc.txtLength = txt.size();
    return true;
  }
  return false;
}

static std::string mdnsString(mg_str value)
{
  return value.len ? std::string(value.buf, value.len) : std::string();
}

bool MongooseMdns::browse(const char *srvcproto)
{
  cancelBrowse();
  if (!srvcproto || !*srvcproto) return false;
  _browseService = srvcproto;
  if (!_browseService.empty() && _browseService.back() == '.') _browseService.pop_back();
  // query() appends ".local" for the PTR query; _browseService itself stays
  // unsuffixed to match resp.sd.srvcproto, which never carries ".local".
  bool sent = query(_browseService.c_str(), MG_DNS_RTYPE_PTR);
  // Arm the retry schedule even if this first send failed (e.g. a transient
  // send error): the next poll tick will try again rather than leaving the
  // browse permanently silent.
  _browseNextQuery = mg_millis() + MG_MDNS_BROWSE_RETRY_MS;
  _browseRetriesLeft = MG_MDNS_BROWSE_MAX_RETRIES;
  return sent;
}

void MongooseMdns::cancelBrowse()
{
  _browseService.clear();
  _browseRetriesLeft = 0;
  std::vector<BrowseRecord>().swap(_records);
}

void MongooseMdns::pollBrowse()
{
  if (_browseService.empty() || _browseRetriesLeft <= 0) return;
  if (mg_millis() < _browseNextQuery) return;
  _browseRetriesLeft--;
  _browseNextQuery = mg_millis() + MG_MDNS_BROWSE_RETRY_MS;
  query(_browseService.c_str(), MG_DNS_RTYPE_PTR);
}

static std::vector<std::pair<std::string, std::string>> mdnsParseTxt(mg_str raw)
{
  std::vector<std::pair<std::string, std::string>> txt;
  for (size_t pos = 0; pos < raw.len;) {
    size_t len = (unsigned char) raw.buf[pos++];
    if (pos + len > raw.len) break;
    std::string entry(raw.buf + pos, len);
    size_t eq = entry.find('=');
    txt.push_back({entry.substr(0, eq), eq == std::string::npos ? "" : entry.substr(eq + 1)});
    pos += len;
  }
  return txt;
}

void MongooseMdns::handleResponse(const mg_mdns_resp &resp)
{
  if (_browseService.empty() || !resp.rr) return;
  std::string srvcproto = mdnsString(resp.sd.srvcproto);
  if (srvcproto.empty() || mg_casecmp(srvcproto.c_str(), _browseService.c_str())) return;

  // resp.name is the bare instance label (no ".local", no service suffix)
  // for a standalone PTR/SRV/TXT answer, but becomes the *resolved SRV
  // target* -- e.g. "peer.local" -- once a combined PTR+SRV reply supplies
  // one (see handle_mdns_response()'s "PTR response" branch: resp.name is
  // set to `host` whenever an SRV record was found). Either way, the first
  // label is the service instance's own base label: our own responder never
  // gives an instance a name distinct from its hostname (there is no
  // separate "friendly name" API), so re-suffixing it recovers the same
  // instance name a bare PTR would have given directly.
  std::string label = mdnsString(resp.name);
  if (label.empty()) return;
  std::string instance = label.substr(0, label.find('.')) + "." + srvcproto + ".local";

  BrowseRecord *rec = nullptr;
  for (auto &r : _records) {
    if (!mg_casecmp(r.instance.c_str(), instance.c_str())) { rec = &r; break; }
  }
  if (!rec) {
    if (_records.size() >= (size_t) MAX_BROWSE_RECORDS) return;
    _records.push_back(BrowseRecord{});
    rec = &_records.back();
    rec->instance = instance;
  }
  rec->expires = mg_millis() + MG_MDNS_CACHE_TTL_MS;

  // resp.sd.port is 0 unless an SRV record was matched; a real service never
  // advertises port 0. Guard on label containing a dot so a bare instance
  // label from an (unused by this library, but possible from a manual
  // mg_mdns_query(MG_DNS_RTYPE_SRV)) standalone SRV query -- for which
  // upstream's parser does not expose the resolved target -- is never
  // mistaken for a resolved hostname.
  if (resp.sd.port && label.find('.') != std::string::npos) {
    rec->hostname = label;
    rec->port = resp.sd.port;
  }
  if (resp.sd.txt.buf) rec->txt = mdnsParseTxt(resp.sd.txt);
  if (resp.addr.addr.ip4) rec->addresses = {resp.addr};
}

std::vector<MongooseMdns::DiscoveredService> MongooseMdns::services() const
{
  std::vector<DiscoveredService> result;
  uint64_t now = mg_millis();
  for (const auto &r : _records) {
    if (r.expires <= now) continue;
    DiscoveredService svc;
    svc.instance = r.instance;
    svc.hostname = r.hostname;
    svc.port = r.port;
    svc.addresses = r.addresses;
    svc.txt = r.txt;
    result.push_back(std::move(svc));
  }
  return result;
}
