#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_MDNS)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>

#include <string.h>
#include <stdio.h>

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
  }
  (void)ev_data;
}

void MongooseMdns::handleReq(struct mg_connection *nc, struct mg_mdns_req *req)
{
  if (!req) return;

  // Service-discovery listing: mongoose doesn't yet support sending the full
  // list automatically, so we skip it here (TODO in upstream mongoose).
  if (req->is_listing) {
    DBUGF("mDNS: service listing request (not yet supported)");
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
      MongooseString reqname(req->reqname);
      if (reqname == _services[i].srvcproto) {
        DBUGF("mDNS: matched service '%s' for query '%.*s'",
              _services[i].srvcproto, (int)req->reqname.len, req->reqname.buf);

        // Build the service record pointing to our storage
        static struct mg_dnssd_record svc;
        svc.srvcproto = mg_str_n(_services[i].srvcproto,
                                 strlen(_services[i].srvcproto));
        svc.txt       = mg_str_n(_services[i].txt,
                                 strlen(_services[i].txt));
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
  if (!hostname || hostname[0] == '\0') {
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
    _mdns->is_closing = 1;
    _mdns = nullptr;
  }
  if (_hostname) {
    free(_hostname);
    _hostname = nullptr;
  }
  _numServices = 0;
}

bool MongooseMdns::addService(const char *srvcproto, uint16_t port, const char *txt)
{
  if (!srvcproto || _numServices >= MAX_SERVICES) {
    return false;
  }

  ServiceRecord &svc = _services[_numServices];
  strncpy(svc.srvcproto, srvcproto, sizeof(svc.srvcproto) - 1);
  svc.srvcproto[sizeof(svc.srvcproto) - 1] = '\0';

  strncpy(svc.txt, txt ? txt : "", sizeof(svc.txt) - 1);
  svc.txt[sizeof(svc.txt) - 1] = '\0';

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

bool MongooseMdns::query(const char *name, unsigned int rtype)
{
  if (!_mdns || !name) {
    return false;
  }
  return mg_mdns_query(_mdns, name, rtype);
}
