#include <unity.h>

#include <MongooseCore.h>
#include <MongooseHttpClient.h>
#include <MongooseHttpServer.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <string>

#include "test_support.h"

// DNS failover: DHCP hands out two servers, Mongoose's resolver talks to one.
// MongooseCore keeps both and rotates on a resolve timeout, so the next lookup
// goes to the server that is still answering.

static void test_dns_failover_first_server_is_active() {
  ScopedMongoose mongoose;
  Mongoose.setNameservers("udp://10.0.0.1:53", "udp://10.0.0.2:53");
  TEST_ASSERT_EQUAL_STRING("udp://10.0.0.1:53", Mongoose.nameserver());
}

static void test_dns_failover_single_server_never_rotates() {
  ScopedMongoose mongoose;
  Mongoose.setNameservers("udp://10.0.0.1:53");
  TEST_ASSERT_FALSE(Mongoose.dnsError("DNS timeout"));
  TEST_ASSERT_EQUAL_STRING("udp://10.0.0.1:53", Mongoose.nameserver());
}

static void test_dns_failover_no_servers_keeps_existing_url() {
  ScopedMongoose mongoose;
  // A NULL dns4.url would make mg_dnsc_init() call mg_error(0, ...) on the
  // next lookup, so "nothing configured" must leave the resolver URL alone.
  Mongoose.setNameservers(nullptr, nullptr);
  TEST_ASSERT_NOT_NULL(Mongoose.nameserver());
  TEST_ASSERT_EQUAL_STRING("udp://8.8.8.8:53", Mongoose.nameserver());  // mg_mgr_init() default
  TEST_ASSERT_FALSE(Mongoose.dnsError("DNS timeout"));

  Mongoose.setNameservers("udp://10.0.0.1:53");
  Mongoose.setNameservers(nullptr, nullptr);
  TEST_ASSERT_EQUAL_STRING("udp://10.0.0.1:53", Mongoose.nameserver());
}

static void test_dns_failover_only_a_timeout_rotates() {
  ScopedMongoose mongoose;
  Mongoose.setNameservers("udp://10.0.0.1:53", "udp://10.0.0.2:53");
  // NXDOMAIN is an answer; a closed resolver socket is not a server fault.
  TEST_ASSERT_FALSE(Mongoose.dnsError("example.com DNS lookup failed"));
  TEST_ASSERT_FALSE(Mongoose.dnsError("DNS error"));
  TEST_ASSERT_FALSE(Mongoose.dnsError("socket error"));
  TEST_ASSERT_FALSE(Mongoose.dnsError(nullptr));
  TEST_ASSERT_EQUAL_STRING("udp://10.0.0.1:53", Mongoose.nameserver());

  TEST_ASSERT_TRUE(Mongoose.dnsError("DNS timeout"));
  TEST_ASSERT_EQUAL_STRING("udp://10.0.0.2:53", Mongoose.nameserver());
}

static void test_dns_failover_burst_of_timeouts_rotates_once() {
  ScopedMongoose mongoose;
  Mongoose.setNameservers("udp://10.0.0.1:53", "udp://10.0.0.2:53");
  Mongoose.getMgr()->dnstimeout = 200;

  // Three lookups stalled on the same dead server report within the window:
  // one rotation, not three (which would land back on the dead server).
  TEST_ASSERT_TRUE(Mongoose.dnsError("DNS timeout"));
  TEST_ASSERT_FALSE(Mongoose.dnsError("DNS timeout"));
  TEST_ASSERT_FALSE(Mongoose.dnsError("DNS timeout"));
  TEST_ASSERT_EQUAL_STRING("udp://10.0.0.2:53", Mongoose.nameserver());

  // A timeout after the window was issued against the new server: rotate again.
  pumpFor(250);
  TEST_ASSERT_TRUE(Mongoose.dnsError("DNS timeout"));
  TEST_ASSERT_EQUAL_STRING("udp://10.0.0.1:53", Mongoose.nameserver());
}

static void test_dns_failover_set_nameservers_resets_to_primary() {
  ScopedMongoose mongoose;
  Mongoose.setNameservers("udp://10.0.0.1:53", "udp://10.0.0.2:53");
  TEST_ASSERT_TRUE(Mongoose.dnsError("DNS timeout"));
  TEST_ASSERT_EQUAL_STRING("udp://10.0.0.2:53", Mongoose.nameserver());

  // ipConfigChanged() path: a fresh DHCP lease starts over on its primary.
  Mongoose.setNameservers("udp://10.0.0.3:53", "udp://10.0.0.4:53");
  TEST_ASSERT_EQUAL_STRING("udp://10.0.0.3:53", Mongoose.nameserver());
}

// --- End to end -------------------------------------------------------------
//
// Two UDP sockets on loopback stand in for the DHCP-supplied servers: the
// primary never answers, the secondary answers every query with A 127.0.0.1.
// A client request to a hostname must fail once with a DNS timeout, after
// which the same request resolves through the secondary and completes.

namespace {

struct FakeDns {
  int fd = -1;
  uint16_t port = 0;
  int queries = 0;
  bool answer = false;

  bool begin(bool answerQueries) {
    answer = answerQueries;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return false;
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sa.sin_port = 0;  // kernel picks a free port
    if (bind(fd, (struct sockaddr *) &sa, sizeof(sa)) != 0) return false;
    socklen_t len = sizeof(sa);
    if (getsockname(fd, (struct sockaddr *) &sa, &len) != 0) return false;
    port = ntohs(sa.sin_port);
    return true;
  }

  std::string url() const {
    return "udp://127.0.0.1:" + std::to_string(port);
  }

  // Drain pending queries; when answering, echo the question back with one
  // A record for 127.0.0.1 (RFC 1035 4.1: header, question, then the answer
  // as a compression pointer to the question name).
  void pump() {
    uint8_t buf[512];
    struct sockaddr_in from;
    socklen_t fromLen = sizeof(from);
    for (;;) {
      ssize_t n = recvfrom(fd, buf, sizeof(buf), MSG_DONTWAIT,
                           (struct sockaddr *) &from, &fromLen);
      if (n < 12) break;
      queries++;
      if (!answer) continue;
      buf[2] = 0x81, buf[3] = 0x80;          // QR, RD, RA
      buf[6] = 0, buf[7] = 1;                // ANCOUNT = 1
      buf[8] = buf[9] = buf[10] = buf[11] = 0;
      size_t q = 12;
      while (q < (size_t) n && buf[q] != 0) q += buf[q] + 1;
      q += 1 + 4;                            // root label, QTYPE, QCLASS
      const uint8_t rr[] = {0xc0, 0x0c,      // name: pointer to offset 12
                            0, 1, 0, 1,      // TYPE A, CLASS IN
                            0, 0, 0, 60,     // TTL
                            0, 4,            // RDLENGTH
                            127, 0, 0, 1};
      memcpy(buf + q, rr, sizeof(rr));
      sendto(fd, buf, q + sizeof(rr), 0, (struct sockaddr *) &from, fromLen);
    }
  }

  ~FakeDns() {
    if (fd >= 0) close(fd);
  }
};

struct Attempt {
  bool closed = false;
  int code = 0;
  std::string error;
};

}  // namespace

static void test_dns_failover_end_to_end_via_secondary() {
  ScopedMongoose mongoose;
  FakeDns dead, live;
  TEST_ASSERT_TRUE(dead.begin(false));
  TEST_ASSERT_TRUE(live.begin(true));

  Mongoose.setNameservers(dead.url().c_str(), live.url().c_str());
  Mongoose.getMgr()->dnstimeout = 200;

  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18093));
  server.on("/ping", HTTP_GET, [](MongooseHttpServerRequest *request) {
    MongooseHttpServerResponseBasic *response = request->beginResponse();
    response->setCode(200);
    response->setContent("pong");
    request->send(response);
  });

  MongooseHttpClient client;
  auto pumpDns = [&dead, &live]() { dead.pump(); live.pump(); };
  auto attempt = [&client, &pumpDns](Attempt &result) {
    MongooseHttpClientRequest *request =
        client.beginRequest("http://failover.test:18093/ping");
    request->onResponse([&result](MongooseHttpClientResponse *response) {
      result.code = response->respCode();
    })->onError([&result](const char *err) {
      result.error = err ? err : "";
    })->onClose([&result]() { result.closed = true; });
    TEST_ASSERT_TRUE(request->send());
    TEST_ASSERT_TRUE_MESSAGE(
        pumpUntil([&result]() { return result.closed; }, 3000, pumpDns),
        "request did not complete");
  };

  // First attempt: the primary swallows the query, the lookup times out.
  Attempt first;
  attempt(first);
  TEST_ASSERT_EQUAL_STRING("DNS timeout", first.error.c_str());
  TEST_ASSERT_EQUAL(0, first.code);
  TEST_ASSERT_EQUAL_STRING(live.url().c_str(), Mongoose.nameserver());
  TEST_ASSERT_TRUE(dead.queries > 0);
  TEST_ASSERT_EQUAL(0, live.queries);

  // Second attempt (what any client's reconnect does): resolves via the
  // secondary and completes.
  Attempt second;
  attempt(second);
  TEST_ASSERT_EQUAL_STRING("", second.error.c_str());
  TEST_ASSERT_EQUAL(200, second.code);
  TEST_ASSERT_TRUE(live.queries > 0);
}

// A DHCP renewal that moves the primary: the established resolver socket must
// be dropped, or mg_dnsc_init() keeps it talking to the retired server.
static void test_dns_failover_reconfigure_replaces_resolver_socket() {
  ScopedMongoose mongoose;
  FakeDns dead, live;
  TEST_ASSERT_TRUE(dead.begin(false));
  TEST_ASSERT_TRUE(live.begin(true));

  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18094));
  server.on("/ping", HTTP_GET, [](MongooseHttpServerRequest *request) {
    MongooseHttpServerResponseBasic *response = request->beginResponse();
    response->setCode(200);
    response->setContent("pong");
    request->send(response);
  });

  // Only one server known, and it's dead: the lookup opens the resolver
  // socket and stalls.
  Mongoose.setNameservers(dead.url().c_str());
  Mongoose.getMgr()->dnstimeout = 5000;
  MongooseHttpClient client;
  bool closed = false;
  MongooseHttpClientRequest *request =
      client.beginRequest("http://reconfig.test:18094/ping");
  request->onClose([&closed]() { closed = true; });
  TEST_ASSERT_TRUE(request->send());
  pumpFor(100, [&dead]() { dead.pump(); });
  TEST_ASSERT_NOT_NULL(Mongoose.getMgr()->dns4.c);
  TEST_ASSERT_TRUE(dead.queries > 0);

  // "Lease renewed, different primary": same slot 0, new address.
  Mongoose.setNameservers(live.url().c_str());
  TEST_ASSERT_NULL(Mongoose.getMgr()->dns4.c);
  TEST_ASSERT_EQUAL_STRING(live.url().c_str(), Mongoose.nameserver());
  // The stalled lookup is failed by the close, not left to its 5 s timeout.
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }, 1000),
                           "in-flight lookup not failed on reconfigure");

  // Re-issuing goes to the new server on a fresh socket.
  Attempt retry;
  MongooseHttpClientRequest *again =
      client.beginRequest("http://reconfig.test:18094/ping");
  again->onResponse([&retry](MongooseHttpClientResponse *response) {
    retry.code = response->respCode();
  })->onClose([&retry]() { retry.closed = true; });
  TEST_ASSERT_TRUE(again->send());
  TEST_ASSERT_TRUE(pumpUntil([&retry]() { return retry.closed; }, 3000,
                             [&live]() { live.pump(); }));
  TEST_ASSERT_EQUAL(200, retry.code);
  TEST_ASSERT_TRUE(live.queries > 0);
}

void runDnsFailoverTests() {
  RUN_TEST(test_dns_failover_first_server_is_active);
  RUN_TEST(test_dns_failover_single_server_never_rotates);
  RUN_TEST(test_dns_failover_no_servers_keeps_existing_url);
  RUN_TEST(test_dns_failover_only_a_timeout_rotates);
  RUN_TEST(test_dns_failover_burst_of_timeouts_rotates_once);
  RUN_TEST(test_dns_failover_set_nameservers_resets_to_primary);
  RUN_TEST(test_dns_failover_end_to_end_via_secondary);
  RUN_TEST(test_dns_failover_reconfigure_replaces_resolver_socket);
}
