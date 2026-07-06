#include <unity.h>

#include <MongooseSntpClient.h>

static void test_sntp_client_build_only_placeholder() {
  // Build-only coverage for now: a deterministic mock UDP SNTP responder can be
  // added later without changing the public API exercised here.
  MongooseSntpClient client;
  TEST_ASSERT_FALSE(client.connected());
}

void runSntpClientTests() {
  RUN_TEST(test_sntp_client_build_only_placeholder);
}
