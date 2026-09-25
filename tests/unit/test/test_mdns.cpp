#include <unity.h>

#include <MongooseMdns.h>

#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>
#include "test_support.h"

extern "C" void mg_multicast_add(struct mg_connection *, char *);

// mDNS tests exercise the service registry (addService/removeService) without
// requiring actual multicast network traffic: no Mongoose event loop is needed.

static void test_mdns_add_service_returns_true() {
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.addService("_http._tcp", 80));
}

static void test_mdns_add_service_fields_stored() {
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.addService("_http._tcp", 80, "version=1"));

  const MongooseMdns::ServiceRecord &svc = mdns.getService(0);
  TEST_ASSERT_EQUAL_STRING("_http._tcp", svc.srvcproto);
  TEST_ASSERT_EQUAL(80, svc.port);
  TEST_ASSERT_EQUAL(10, svc.txtLength);
  TEST_ASSERT_EQUAL_MEMORY("\x09version=1", svc.txt, svc.txtLength);
}

static void test_mdns_add_service_protocol_transport_overload() {
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.addService("_mqtt", "_tcp", 1883));
  TEST_ASSERT_EQUAL(1, mdns.numServices());

  const MongooseMdns::ServiceRecord &svc = mdns.getService(0);
  TEST_ASSERT_EQUAL_STRING("_mqtt._tcp", svc.srvcproto);
  TEST_ASSERT_EQUAL(1883, svc.port);
}

static void test_mdns_remove_service_returns_true_when_found() {
  MongooseMdns mdns;
  mdns.addService("_http._tcp", 80);
  TEST_ASSERT_TRUE(mdns.removeService("_http._tcp"));
  TEST_ASSERT_EQUAL(0, mdns.numServices());
}

static void test_mdns_remove_service_returns_false_when_not_found() {
  MongooseMdns mdns;
  TEST_ASSERT_FALSE(mdns.removeService("_http._tcp"));
}

static void test_mdns_remove_service_shifts_remaining() {
  MongooseMdns mdns;
  mdns.addService("_http._tcp", 80);
  mdns.addService("_mqtt._tcp", 1883);
  mdns.addService("_mdap._tcp", 7962);

  // Remove the middle entry
  TEST_ASSERT_TRUE(mdns.removeService("_mqtt._tcp"));
  TEST_ASSERT_EQUAL(2, mdns.numServices());

  // Remaining entries should be _http._tcp and _mdap._tcp in order
  TEST_ASSERT_EQUAL_STRING("_http._tcp", mdns.getService(0).srvcproto);
  TEST_ASSERT_EQUAL_STRING("_mdap._tcp", mdns.getService(1).srvcproto);
}

static void test_mdns_remove_first_service() {
  MongooseMdns mdns;
  mdns.addService("_http._tcp", 80);
  mdns.addService("_mqtt._tcp", 1883);

  TEST_ASSERT_TRUE(mdns.removeService("_http._tcp"));
  TEST_ASSERT_EQUAL(1, mdns.numServices());
  TEST_ASSERT_EQUAL_STRING("_mqtt._tcp", mdns.getService(0).srvcproto);
}

static void test_mdns_remove_last_service() {
  MongooseMdns mdns;
  mdns.addService("_http._tcp", 80);
  mdns.addService("_mqtt._tcp", 1883);

  TEST_ASSERT_TRUE(mdns.removeService("_mqtt._tcp"));
  TEST_ASSERT_EQUAL(1, mdns.numServices());
  TEST_ASSERT_EQUAL_STRING("_http._tcp", mdns.getService(0).srvcproto);
}

static void test_mdns_remove_service_two_part_overload() {
  MongooseMdns mdns;
  mdns.addService("_http._tcp", 80);
  TEST_ASSERT_TRUE(mdns.removeService("_http", "_tcp"));
  TEST_ASSERT_EQUAL(0, mdns.numServices());
}

static void test_mdns_max_services_boundary() {
  MongooseMdns mdns;
  for (int i = 0; i < MongooseMdns::MAX_SERVICES; i++) {
    char name[32];
    snprintf(name, sizeof(name), "_svc%d._tcp", i);
    TEST_ASSERT_TRUE(mdns.addService(name, (uint16_t)(8000 + i)));
  }
  TEST_ASSERT_EQUAL(MongooseMdns::MAX_SERVICES, mdns.numServices());

  // One more service beyond MAX_SERVICES should fail
  TEST_ASSERT_FALSE(mdns.addService("_overflow._tcp", 9999));
  TEST_ASSERT_EQUAL(MongooseMdns::MAX_SERVICES, mdns.numServices());
}

static void test_mdns_add_service_null_returns_false() {
  MongooseMdns mdns;
  TEST_ASSERT_FALSE(mdns.addService(nullptr, 80));
}

static void test_mdns_remove_service_null_returns_false() {
  MongooseMdns mdns;
  TEST_ASSERT_FALSE(mdns.removeService(nullptr));
}

namespace {
using Packet = std::vector<uint8_t>;

void wordBE(Packet &p, unsigned value) {
  p.push_back((uint8_t)(value >> 8)); p.push_back((uint8_t)value);
}

Packet name(const std::string &text) {
  Packet p;
  size_t start = 0;
  while (start < text.size()) {
    size_t end = text.find('.', start);
    if (end == std::string::npos) end = text.size();
    p.push_back((uint8_t)(end - start));
    p.insert(p.end(), text.begin() + start, text.begin() + end);
    start = end + 1;
  }
  p.push_back(0);
  return p;
}

void record(Packet &p, const std::string &owner, unsigned type,
            const Packet &data, unsigned ttl = 120) {
  auto labels = name(owner);
  p.insert(p.end(), labels.begin(), labels.end());
  wordBE(p, type); wordBE(p, 0x8001); wordBE(p, ttl >> 16); wordBE(p, ttl);
  wordBE(p, data.size()); p.insert(p.end(), data.begin(), data.end());
}

Packet response(unsigned answers, unsigned additional = 0) {
  Packet p(12, 0);
  p[2] = 0x84; p[7] = answers; p[11] = additional;
  return p;
}

void inject(mg_connection *c, const Packet &p) {
  mg_iobuf_add(&c->recv, c->recv.len, p.data(), p.size());
  mg_call(c, MG_EV_READ, &c->recv.len);
}

mg_connection *receiver(Packet &packet) {
  return mg_listen(Mongoose.getMgr(), "udp://127.0.0.1:0",
    [](mg_connection *c, int ev, void *) {
      if (ev == MG_EV_READ) {
        auto &p = *static_cast<Packet *>(c->fn_data);
        p.assign(c->recv.buf, c->recv.buf + c->recv.len);
        c->recv.len = 0;
      }
    }, &packet);
}

// --- Isolation from the real network ---------------------------------------
//
// mg_mdns_listen() joins 224.0.0.251:5353 on every interface, so anything
// listening on that group hears the whole LAN, not just this process. On an
// ordinary developer network that is around one query a second from a dozen
// hosts, plus their responses.
//
// Two of these tests count queries seen on the group and one browses
// "_openevse._tcp" -- a service real units on the same network advertise. So
// the counters counted other people's queries, and the browse collected real
// answers, which is what made them fail roughly one run in two.
//
// Both go away by browsing a service type that exists only inside this
// process: nothing out there queries it, and nothing answers it. The query
// counter matches on that name as well, so a stray datagram is ignored even
// if one does arrive. It also stops the suite advertising a bogus
// "_openevse._tcp" responder onto the real network while it runs.
// mg_millis() rather than getpid(): this file is also compiled for the
// esp32_test build, where there are no processes.
std::string uniqueService() {
  static int n = 0;
  char buf[64];
  snprintf(buf, sizeof(buf), "_amtest%lu-%d._tcp",
           (unsigned long) (mg_millis() & 0xffffff), ++n);
  return buf;
}

// True if this datagram is an mDNS *query* carrying `service` as a question.
// Matching on the first label is enough: it is unique to this process, so a
// match cannot have come from the network.
bool isQueryFor(const mg_connection *c, const std::string &service) {
  if (c->recv.len <= 12 || c->recv.buf[2] != 0) {
    return false;  // too short, or a response rather than a query
  }
  const std::string label = service.substr(0, service.find('.'));
  if (c->recv.len < label.size()) {
    return false;
  }
  for (size_t i = 0; i + label.size() <= c->recv.len; i++) {
    if (0 == memcmp(c->recv.buf + i, label.data(), label.size())) {
      return true;
    }
  }
  return false;
}

// No default member initialisers: a C++11 aggregate cannot have them, and
// these are brace-initialised at every use.
struct QueryCounter {
  std::string service;
  int count;
};

// Joins the group and counts only the queries for counter.service.
mg_connection *queryCounter(QueryCounter &counter) {
  auto *rx = mg_listen(Mongoose.getMgr(), "udp://224.0.0.251:5353",
    [](mg_connection *c, int ev, void *) {
      if (ev == MG_EV_READ) {
        auto &q = *static_cast<QueryCounter *>(c->fn_data);
        if (isQueryFor(c, q.service)) {
          q.count++;
        }
        c->recv.len = 0;
      }
    }, &counter);
  if (rx) {
    mg_multicast_add(rx, const_cast<char *>("224.0.0.251"));
  }
  return rx;
}
}

static void test_mdns_query_encodes_requested_type_and_local_suffix() {
  ScopedMongoose scope;
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.begin("unit-query"));
  Packet packet;
  // Filtered the same way as the counters above: this compares the captured
  // bytes against an exact expected packet, so capturing a passing stranger's
  // query off the group would fail it.
  auto *rx = mg_listen(Mongoose.getMgr(), "udp://224.0.0.251:5353",
    [](mg_connection *c, int ev, void *) {
      if (ev == MG_EV_READ) {
        if (isQueryFor(c, "_unit-query._tcp")) {
          auto &p = *static_cast<Packet *>(c->fn_data);
          p.assign(c->recv.buf, c->recv.buf + c->recv.len);
        }
        c->recv.len = 0;
      }
    }, &packet);
  TEST_ASSERT_NOT_NULL(rx);
  mg_multicast_add(rx, const_cast<char *>("224.0.0.251"));
  TEST_ASSERT_TRUE(mdns.query("_unit-query._tcp", MG_DNS_RTYPE_PTR));
  TEST_ASSERT_TRUE(pumpUntil([&]() { return !packet.empty(); }));
  Packet expected(12, 0);
  expected[5] = 1;
  auto labels = name("_unit-query._tcp.local");
  expected.insert(expected.end(), labels.begin(), labels.end());
  wordBE(expected, MG_DNS_RTYPE_PTR); wordBE(expected, 1);
  TEST_ASSERT_EQUAL(expected.size(), packet.size());
  TEST_ASSERT_EQUAL_MEMORY(expected.data(), packet.data(), packet.size());
  // sendto() refreshes loc to the wildcard bind address. A second query must
  // still go to the multicast group rather than 0.0.0.0.
  packet.clear();
  TEST_ASSERT_TRUE(mdns.query("_unit-query._tcp", MG_DNS_RTYPE_PTR));
  TEST_ASSERT_TRUE(pumpUntil([&]() { return !packet.empty(); }));
  TEST_ASSERT_EQUAL_MEMORY(expected.data(), packet.data(), packet.size());
}

// Round-trip: drive our OWN responder to emit a combined PTR reply, capture
// the exact bytes it puts on the wire, then feed those bytes to the browse
// layer. Every other browse test hand-builds the packet it *believes* the
// responder sends, so the two can drift apart and still both pass. This one
// cannot: if the responder's real record order or owner names are not what
// handle_mdns_response()'s chain parser expects, browse() sees nothing and
// this fails -- which is what four native instances discovering each other
// actually exercises.
static void test_mdns_browse_parses_our_own_responders_real_reply() {
  const std::string service = uniqueService();
  Packet reply;
  {
    ScopedMongoose scope;
    MongooseMdns responder;
    TEST_ASSERT_TRUE(responder.begin("peer"));
    TEST_ASSERT_TRUE(responder.addService(service.c_str(), 8443));
    TEST_ASSERT_TRUE(responder.addServiceTxt(service.c_str(), "id", "7"));
    auto *rx = receiver(reply);
    TEST_ASSERT_NOT_NULL(rx);
    auto *c = Mongoose.getMgr()->mdns;
    c->rem = rx->loc;
    Packet q(12, 0); q[5] = 1;                 // 1 question
    auto labels = name((service + ".local").c_str());
    q.insert(q.end(), labels.begin(), labels.end());
    wordBE(q, MG_DNS_RTYPE_PTR); wordBE(q, 0x8001);  // QU so the reply is unicast
    inject(c, q);
    TEST_ASSERT_TRUE(pumpUntil([&]() { return !reply.empty(); }));
  }
  TEST_ASSERT_TRUE(reply.size() > 12);
  {
    ScopedMongoose scope;
    MongooseMdns browser;
    TEST_ASSERT_TRUE(browser.begin("unit-browse"));
    TEST_ASSERT_TRUE(browser.browse(service.c_str()));
    inject(Mongoose.getMgr()->mdns, reply);
    auto services = browser.services();
    TEST_ASSERT_EQUAL(1, services.size());
    TEST_ASSERT_EQUAL_STRING(("peer." + service + ".local").c_str(), services[0].instance.c_str());
    TEST_ASSERT_EQUAL_STRING("peer.local", services[0].hostname.c_str());
    TEST_ASSERT_EQUAL(8443, services[0].port);
  }
}

static void test_mdns_browse_resolves_combined_ptr_srv_txt_a_reply() {
  // Our own responder always answers a PTR query with a single combined
  // PTR+SRV+TXT+A reply (see handle_mdns_query()'s "serve PTR + SRV + TXT +
  // A" in mongoose.c), and so does any peer running this same library --
  // simulate that shape, the one this library's browse() actually receives.
  ScopedMongoose scope;
  MongooseMdns mdns;
  const std::string service = uniqueService();
  TEST_ASSERT_TRUE(mdns.begin("unit-browse"));
  TEST_ASSERT_TRUE(mdns.browse(service.c_str()));
  auto *c = Mongoose.getMgr()->mdns;
  // Our responder never gives an instance a name distinct from its hostname
  // (see handleResponse()'s comment on this), so "peer" is used for both.
  const std::string instanceStr = "peer." + service + ".local";
  const char *instance = instanceStr.c_str();

  Packet p = response(1, 3);
  record(p, (service + ".local").c_str(), MG_DNS_RTYPE_PTR, name(instance));
  Packet srv(4, 0); wordBE(srv, 8443);
  auto host = name("peer.local"); srv.insert(srv.end(), host.begin(), host.end());
  record(p, instance, MG_DNS_RTYPE_SRV, srv);
  record(p, instance, MG_DNS_RTYPE_TXT, {4, 'i', 'd', '=', '7', 5, 's', 's', 'l', '=', '1'});
  record(p, "peer.local", MG_DNS_RTYPE_A, {10, 2, 3, 4});
  inject(c, p);

  auto services = mdns.services();
  TEST_ASSERT_EQUAL(1, services.size());
  TEST_ASSERT_EQUAL_STRING(instance, services[0].instance.c_str());
  TEST_ASSERT_EQUAL_STRING("peer.local", services[0].hostname.c_str());
  TEST_ASSERT_EQUAL(8443, services[0].port);
  TEST_ASSERT_EQUAL(1, services[0].addresses.size());
  TEST_ASSERT_FALSE(services[0].addresses[0].is_ip6);
  TEST_ASSERT_EQUAL_STRING("ssl", services[0].txt[1].first.c_str());
  TEST_ASSERT_EQUAL_STRING("1", services[0].txt[1].second.c_str());

  // A reply for an unrelated service type must not show up in this browse.
  p = response(1);
  record(p, "_http._tcp.local", MG_DNS_RTYPE_PTR, name("other._http._tcp.local"));
  inject(c, p);
  TEST_ASSERT_EQUAL(1, mdns.services().size());

  // struct mg_mdns_resp carries no TTL (see MG_MDNS_CACHE_TTL_MS in
  // mongoose.h), so a browse result expires on the same fixed lifetime as
  // the resolver cache rather than reacting to a goodbye immediately.
  pumpFor(MG_MDNS_CACHE_TTL_MS + 50);
  TEST_ASSERT_TRUE(mdns.services().empty());

  mdns.cancelBrowse();
  TEST_ASSERT_TRUE(mdns.services().empty());
}

// The regression this pins: browse() sending its PTR query exactly once
// left four native firmware instances discovering zero peers of each other
// in openevse_esp32_firmware#1271's integration suite, because any peer not
// already listening at that exact instant, or a lost datagram, or a peer
// still inside its own RFC 6762 SS6 one-answer-per-second window, meant a
// silent, permanent miss. Measured directly against this branch before the
// fix: exactly 1 query, none more after a further 3s of polling.
static void test_mdns_browse_retries_the_ptr_query_a_bounded_number_of_times() {
  ScopedMongoose scope;
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.begin("unit-retry"));

  QueryCounter counter{uniqueService(), 0};
  TEST_ASSERT_NOT_NULL(queryCounter(counter));

  TEST_ASSERT_TRUE(mdns.browse(counter.service.c_str()));
  TEST_ASSERT_TRUE(pumpUntil([&]() { return counter.count >= 1; }));

  const int expected = 1 + MG_MDNS_BROWSE_MAX_RETRIES;
  TEST_ASSERT_TRUE(pumpUntil([&]() { return counter.count >= expected; },
                             MG_MDNS_BROWSE_MAX_RETRIES * MG_MDNS_BROWSE_RETRY_MS + 500));
  TEST_ASSERT_EQUAL(expected, counter.count);

  // Bounded: no further sends once MG_MDNS_BROWSE_MAX_RETRIES is spent, even
  // across several more retry intervals -- this is not the unbounded
  // per-poll re-query pollBrowse() used to do.
  pumpFor(MG_MDNS_BROWSE_RETRY_MS * 2);
  TEST_ASSERT_EQUAL(expected, counter.count);
}

static void test_mdns_cancel_browse_stops_retries_in_flight() {
  ScopedMongoose scope;
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.begin("unit-cancel"));

  QueryCounter counter{uniqueService(), 0};
  TEST_ASSERT_NOT_NULL(queryCounter(counter));

  TEST_ASSERT_TRUE(mdns.browse(counter.service.c_str()));
  TEST_ASSERT_TRUE(pumpUntil([&]() { return counter.count >= 1; }));

  mdns.cancelBrowse();
  int afterCancel = counter.count;
  pumpFor(MG_MDNS_BROWSE_RETRY_MS * 3);
  TEST_ASSERT_EQUAL(afterCancel, counter.count);
}

static void test_mdns_browse_instance_name_is_approximated_from_hostname() {
  // handle_mdns_response() only gives handleResponse() the resolved SRV
  // target once a combined PTR+SRV reply supplies one -- the PTR's own
  // instance name is not part of struct mg_mdns_resp in that case (see
  // handleResponse()'s comment). A service instance advertised under a name
  // that differs from its hostname is therefore reported under the hostname
  // instead: exact for this library's own responder (hostname *is* the
  // instance name there, with no separate "friendly name" API), and only an
  // approximation for a third-party responder using a distinct one --
  // documented here rather than silently assumed.
  ScopedMongoose scope;
  MongooseMdns mdns;
  const std::string service = uniqueService();
  TEST_ASSERT_TRUE(mdns.begin("unit-browse2"));
  TEST_ASSERT_TRUE(mdns.browse(service.c_str()));
  auto *c = Mongoose.getMgr()->mdns;
  const std::string instanceStr = "Display Name." + service + ".local";
  const char *instance = instanceStr.c_str();

  Packet p = response(1, 1);
  record(p, (service + ".local").c_str(), MG_DNS_RTYPE_PTR, name(instance));
  Packet srv(4, 0); wordBE(srv, 8443);
  auto host = name("peer.local"); srv.insert(srv.end(), host.begin(), host.end());
  record(p, instance, MG_DNS_RTYPE_SRV, srv);
  inject(c, p);

  auto services = mdns.services();
  TEST_ASSERT_EQUAL(1, services.size());
  TEST_ASSERT_EQUAL_STRING(("peer." + service + ".local").c_str(),
                           services[0].instance.c_str());
  TEST_ASSERT_EQUAL_STRING("peer.local", services[0].hostname.c_str());
}

static void test_mdns_resolves_parallel_clients_and_expires_fixed_ttl_cache() {
  ScopedMongoose scope;
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.begin("unit-resolver"));
  auto *mgr = Mongoose.getMgr();
  auto *first = mg_connect(mgr, "udp://peer.local:1234", nullptr, nullptr);
  auto *second = mg_connect(mgr, "udp://PEER.LOCAL:5678", nullptr, nullptr);
  TEST_ASSERT_TRUE(first->is_resolving);
  TEST_ASSERT_TRUE(second->is_resolving);
  // Note: unlike upstream's own DNS-SD chain parser, which is tolerant of a
  // coalesced packet's unrelated leading records (see handle_mdns_response()
  // "First Answer RR is primary; the rest must match it"), a single-answer
  // response is used here rather than one with an irrelevant record in front
  // of it. Vendored upstream mongoose locks onto whatever the first record
  // in the packet is; an irrelevant leading record derails resolution of the
  // real one entirely, the same limitation the mDNS browse layer has (see
  // MongooseMdns.cpp) rather than something specific to this resolver path.
  Packet p = response(1);
  record(p, "peer.local", MG_DNS_RTYPE_A, {127, 0, 0, 1});
  inject(mgr->mdns, p);
  TEST_ASSERT_FALSE(first->is_resolving);
  TEST_ASSERT_FALSE(second->is_resolving);
  TEST_ASSERT_EQUAL(1234, mg_ntohs(first->rem.port));
  TEST_ASSERT_EQUAL(5678, mg_ntohs(second->rem.port));
  auto *cached = mg_connect(mgr, "udp://peer.local:9012", nullptr, nullptr);
  TEST_ASSERT_FALSE(cached->is_resolving);
  TEST_ASSERT_EQUAL(9012, mg_ntohs(cached->rem.port));
  // struct mg_mdns_resp no longer carries the answer's TTL (see
  // MG_MDNS_CACHE_TTL_MS in mongoose.h), so a TTL=0 "goodbye" answer can no
  // longer trigger immediate eviction; the resolver cache instead always
  // expires itself after a fixed lifetime. Wait past that lifetime (shortened
  // for this test build via platformio.ini) and confirm it does go stale on
  // its own, even without an explicit goodbye.
  pumpFor(MG_MDNS_CACHE_TTL_MS + 50);
  auto *expired = mg_connect(mgr, "udp://peer.local:1234", nullptr, nullptr);
  TEST_ASSERT_TRUE(expired->is_resolving);
  // Closing a lookup and restarting the responder must not retain freed handles.
  expired->is_closing = 1;
  Mongoose.poll(0);
  TEST_ASSERT_NULL(mgr->active_mdns_requests);
  TEST_ASSERT_TRUE(mdns.begin("unit-renamed"));
  Mongoose.poll(0);
  TEST_ASSERT_TRUE(mdns.isActive());
  TEST_ASSERT_NOT_NULL(mgr->mdns);
  mdns.end();
  TEST_ASSERT_NULL(mgr->mdns);
  Mongoose.poll(0);
}

static void test_mdns_advertises_encoded_txt_and_full_srv_owner() {
  ScopedMongoose scope;
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.begin("unit-service"));
  TEST_ASSERT_TRUE(mdns.addService("_openevse._tcp", 8443));
  TEST_ASSERT_TRUE(mdns.addServiceTxt("_openevse._tcp", "id", "123"));
  TEST_ASSERT_TRUE(mdns.addServiceTxt("_openevse._tcp", "ssl", "0"));
  TEST_ASSERT_TRUE(mdns.addServiceTxt("_openevse._tcp", "ssl", "1"));
  const char txt[] = "\x06id=123\x05ssl=1";
  TEST_ASSERT_EQUAL(sizeof(txt) - 1, mdns.getService(0).txtLength);
  TEST_ASSERT_EQUAL_MEMORY(txt, mdns.getService(0).txt, sizeof(txt) - 1);
  Packet packet;
  auto *rx = receiver(packet);
  auto *c = Mongoose.getMgr()->mdns;
  c->rem = rx->loc;
  Packet q(12, 0); q[5] = 1;
  auto labels = name("unit-service._openevse._tcp.local");
  q.insert(q.end(), labels.begin(), labels.end());
  wordBE(q, MG_DNS_RTYPE_SRV); wordBE(q, 0x8001);
  inject(c, q);
  TEST_ASSERT_TRUE(pumpUntil([&]() { return !packet.empty(); }));
  TEST_ASSERT_EQUAL_MEMORY(labels.data(), packet.data() + 12, labels.size());
  mg_dns_rr rr;
  TEST_ASSERT_GREATER_THAN(0, mg_dns_parse_rr(packet.data(), packet.size(), 12, false, &rr));
  TEST_ASSERT_EQUAL(MG_DNS_RTYPE_SRV, rr.atype);
}

static void test_mdns_processes_later_questions_and_service_enumeration() {
  ScopedMongoose scope;
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.begin("unit-multi"));
  TEST_ASSERT_TRUE(mdns.addService("_http._tcp", 80));
  TEST_ASSERT_TRUE(mdns.addService("_openevse._tcp", 80));
  Packet packet;
  auto *rx = receiver(packet);
  auto *c = Mongoose.getMgr()->mdns;
  c->rem = rx->loc;
  Packet q(12, 0); q[5] = 2;
  auto labels = name("not-us.local");
  q.insert(q.end(), labels.begin(), labels.end()); wordBE(q, 1); wordBE(q, 0x8001);
  labels = name("_services._dns-sd._udp.local");
  q.insert(q.end(), labels.begin(), labels.end()); wordBE(q, 12); wordBE(q, 0x8001);
  inject(c, q);
  TEST_ASSERT_TRUE(pumpUntil([&]() { return !packet.empty(); }));
  TEST_ASSERT_EQUAL(2, packet[7]);
  TEST_ASSERT_EQUAL_MEMORY(labels.data(), packet.data() + 12, labels.size());
  size_t pos = 12;
  for (int i = 0; i < 2; i++) {
    mg_dns_rr rr;
    size_t n = mg_dns_parse_rr(packet.data(), packet.size(), pos, false, &rr);
    TEST_ASSERT_GREATER_THAN(0, n);
    TEST_ASSERT_EQUAL(MG_DNS_RTYPE_PTR, rr.atype);
    pos += n;
  }
  TEST_ASSERT_EQUAL(packet.size(), pos);
}

static void test_mdns_compressed_response_questions_and_malformed_records() {
  ScopedMongoose scope;
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.begin("unit-compression"));
  auto *mgr = Mongoose.getMgr();
  auto *client = mg_connect(mgr, "udp://compressed.local:1234", nullptr, nullptr);
  Packet p = response(1); p[5] = 1;
  auto labels = name("compressed.local");
  p.insert(p.end(), labels.begin(), labels.end()); wordBE(p, 1); wordBE(p, 1);
  // The answer owner is a compression pointer to the question, not a label.
  wordBE(p, 0xc00c); wordBE(p, 1); wordBE(p, 0x8001);
  wordBE(p, 0); wordBE(p, 120); wordBE(p, 4);
  p.insert(p.end(), {127, 0, 0, 1});
  auto truncated = p; truncated.pop_back();
  inject(mgr->mdns, truncated);
  TEST_ASSERT_TRUE(client->is_resolving);
  auto cyclic = p;
  size_t ofs = 12 + labels.size() + 4;
  cyclic[ofs] = (uint8_t)(0xc0 | (ofs >> 8)); cyclic[ofs + 1] = (uint8_t)ofs;
  inject(mgr->mdns, cyclic);
  TEST_ASSERT_TRUE(client->is_resolving);
  inject(mgr->mdns, p);
  TEST_ASSERT_FALSE(client->is_resolving);
}

void runMdnsTests() {
  RUN_TEST(test_mdns_processes_later_questions_and_service_enumeration);
  RUN_TEST(test_mdns_compressed_response_questions_and_malformed_records);
  RUN_TEST(test_mdns_query_encodes_requested_type_and_local_suffix);
  RUN_TEST(test_mdns_browse_resolves_combined_ptr_srv_txt_a_reply);
  RUN_TEST(test_mdns_browse_parses_our_own_responders_real_reply);
  RUN_TEST(test_mdns_browse_retries_the_ptr_query_a_bounded_number_of_times);
  RUN_TEST(test_mdns_cancel_browse_stops_retries_in_flight);
  RUN_TEST(test_mdns_browse_instance_name_is_approximated_from_hostname);
  RUN_TEST(test_mdns_resolves_parallel_clients_and_expires_fixed_ttl_cache);
  RUN_TEST(test_mdns_advertises_encoded_txt_and_full_srv_owner);
  RUN_TEST(test_mdns_add_service_returns_true);
  RUN_TEST(test_mdns_add_service_fields_stored);
  RUN_TEST(test_mdns_add_service_protocol_transport_overload);
  RUN_TEST(test_mdns_remove_service_returns_true_when_found);
  RUN_TEST(test_mdns_remove_service_returns_false_when_not_found);
  RUN_TEST(test_mdns_remove_service_shifts_remaining);
  RUN_TEST(test_mdns_remove_first_service);
  RUN_TEST(test_mdns_remove_last_service);
  RUN_TEST(test_mdns_remove_service_two_part_overload);
  RUN_TEST(test_mdns_max_services_boundary);
  RUN_TEST(test_mdns_add_service_null_returns_false);
  RUN_TEST(test_mdns_remove_service_null_returns_false);
}
