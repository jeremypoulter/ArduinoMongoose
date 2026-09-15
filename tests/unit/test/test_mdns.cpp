#include <unity.h>

#include <MongooseMdns.h>

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

void word(Packet &p, unsigned value) {
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
  word(p, type); word(p, 0x8001); word(p, ttl >> 16); word(p, ttl);
  word(p, data.size()); p.insert(p.end(), data.begin(), data.end());
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
}

static void test_mdns_query_encodes_requested_type_and_local_suffix() {
  ScopedMongoose scope;
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.begin("unit-query"));
  Packet packet;
  auto *rx = mg_listen(Mongoose.getMgr(), "udp://224.0.0.251:5353",
    [](mg_connection *c, int ev, void *) {
      if (ev == MG_EV_READ) {
        if (c->recv.len > 12 && c->recv.buf[2] == 0) {
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
  word(expected, MG_DNS_RTYPE_PTR); word(expected, 1);
  TEST_ASSERT_EQUAL(expected.size(), packet.size());
  TEST_ASSERT_EQUAL_MEMORY(expected.data(), packet.data(), packet.size());
  // sendto() refreshes loc to the wildcard bind address. A second query must
  // still go to the multicast group rather than 0.0.0.0.
  packet.clear();
  TEST_ASSERT_TRUE(mdns.query("_unit-query._tcp", MG_DNS_RTYPE_PTR));
  TEST_ASSERT_TRUE(pumpUntil([&]() { return !packet.empty(); }));
  TEST_ASSERT_EQUAL_MEMORY(expected.data(), packet.data(), packet.size());
}

static void test_mdns_browse_assembles_split_out_of_order_records_and_goodbye() {
  ScopedMongoose scope;
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.begin("unit-browse"));
  TEST_ASSERT_TRUE(mdns.browse("_openevse._tcp"));
  auto *c = Mongoose.getMgr()->mdns;
  const char *instance = "Display Name._openevse._tcp.local";
  Packet p = response(1, 4);
  // Unrelated record >512 bytes used to make the whole datagram unparseable.
  Packet padding(601, 'x'); padding[0] = 255; padding[256] = 255; padding[512] = 88;
  record(p, "other.local", MG_DNS_RTYPE_TXT, padding);
  record(p, "peer.local", MG_DNS_RTYPE_AAAA, Packet(16, 1));
  record(p, "peer.local", MG_DNS_RTYPE_A, {10, 2, 3, 4});
  record(p, instance, MG_DNS_RTYPE_TXT, {4, 'i', 'd', '=', '7', 5, 's', 's', 'l', '=', '1'});
  Packet srv(4, 0); word(srv, 8443);
  auto host = name("peer.local"); srv.insert(srv.end(), host.begin(), host.end());
  record(p, instance, MG_DNS_RTYPE_SRV, srv);
  inject(c, p);
  TEST_ASSERT_TRUE(mdns.services().empty());
  p = response(1);
  record(p, "_openevse._tcp.local", MG_DNS_RTYPE_PTR, name(instance));
  inject(c, p);
  auto services = mdns.services();
  TEST_ASSERT_EQUAL(1, services.size());
  TEST_ASSERT_EQUAL_STRING(instance, services[0].instance.c_str());
  TEST_ASSERT_EQUAL_STRING("peer.local", services[0].hostname.c_str());
  TEST_ASSERT_EQUAL(8443, services[0].port);
  TEST_ASSERT_EQUAL(2, services[0].addresses.size());
  TEST_ASSERT_TRUE(services[0].addresses[0].is_ip6);
  TEST_ASSERT_EQUAL_STRING("ssl", services[0].txt[1].first.c_str());
  TEST_ASSERT_EQUAL_STRING("1", services[0].txt[1].second.c_str());
  p = response(1);
  record(p, "_openevse._tcp.local", MG_DNS_RTYPE_PTR, name(instance), 0);
  inject(c, p);
  TEST_ASSERT_TRUE(mdns.services().empty());
}

static void test_mdns_resolves_parallel_clients_caches_and_evicts_goodbye() {
  ScopedMongoose scope;
  MongooseMdns mdns;
  TEST_ASSERT_TRUE(mdns.begin("unit-resolver"));
  auto *mgr = Mongoose.getMgr();
  auto *first = mg_connect(mgr, "udp://peer.local:1234", nullptr, nullptr);
  auto *second = mg_connect(mgr, "udp://PEER.LOCAL:5678", nullptr, nullptr);
  TEST_ASSERT_TRUE(first->is_resolving);
  TEST_ASSERT_TRUE(second->is_resolving);
  Packet p = response(1, 1);
  record(p, "irrelevant.local", MG_DNS_RTYPE_TXT, {0});
  record(p, "peer.local", MG_DNS_RTYPE_A, {127, 0, 0, 1});
  inject(mgr->mdns, p);
  TEST_ASSERT_FALSE(first->is_resolving);
  TEST_ASSERT_FALSE(second->is_resolving);
  TEST_ASSERT_EQUAL(1234, mg_ntohs(first->rem.port));
  TEST_ASSERT_EQUAL(5678, mg_ntohs(second->rem.port));
  auto *cached = mg_connect(mgr, "udp://peer.local:9012", nullptr, nullptr);
  TEST_ASSERT_FALSE(cached->is_resolving);
  TEST_ASSERT_EQUAL(9012, mg_ntohs(cached->rem.port));
  p = response(1);
  record(p, "peer.local", MG_DNS_RTYPE_A, {127, 0, 0, 1}, 0);
  inject(mgr->mdns, p);
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
  word(q, MG_DNS_RTYPE_SRV); word(q, 0x8001);
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
  q.insert(q.end(), labels.begin(), labels.end()); word(q, 1); word(q, 0x8001);
  labels = name("_services._dns-sd._udp.local");
  q.insert(q.end(), labels.begin(), labels.end()); word(q, 12); word(q, 0x8001);
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
  p.insert(p.end(), labels.begin(), labels.end()); word(p, 1); word(p, 1);
  // The answer owner is a compression pointer to the question, not a label.
  word(p, 0xc00c); word(p, 1); word(p, 0x8001);
  word(p, 0); word(p, 120); word(p, 4);
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
  RUN_TEST(test_mdns_browse_assembles_split_out_of_order_records_and_goodbye);
  RUN_TEST(test_mdns_resolves_parallel_clients_caches_and_evicts_goodbye);
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
