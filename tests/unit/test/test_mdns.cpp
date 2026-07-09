#include <unity.h>

#include <MongooseMdns.h>

#include <string>

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
  TEST_ASSERT_EQUAL_STRING("version=1", svc.txt);
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

void runMdnsTests() {
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
