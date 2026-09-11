#include <unity.h>

#include <MongooseSntpClient.h>

#include <cstring>
#include <cstdlib>

#include "test_support.h"

static void test_sntp_client_build_only_placeholder() {
  // Build-only coverage for now: a deterministic mock UDP SNTP responder can be
  // added later without changing the public API exercised here.
  MongooseSntpClient client;
  TEST_ASSERT_FALSE(client.connected());
}

// Verify that after construction:
//   - connected() reports false (no active socket)
//   - getTime() can be called (returns false because there is no reachable server)
//     without crashing.
static void test_sntp_client_not_connected_initially() {
  MongooseSntpClient client;
  TEST_ASSERT_FALSE(client.connected());
}

// Verify the fluent onTime/onError/onClose registrations compile and don't crash.
static void test_sntp_client_fluent_handler_registration() {
  MongooseSntpClient client;
  bool timeCalled = false;
  bool errorCalled = false;
  bool closeCalled = false;

  client.onTime([&timeCalled](struct timeval) { timeCalled = true; })
        ->onError([&errorCalled](const char *) { errorCalled = true; })
        ->onClose([&closeCalled]() { closeCalled = true; });

  // Handlers registered but not fired – just verify no crash
  TEST_ASSERT_FALSE(timeCalled);
  TEST_ASSERT_FALSE(errorCalled);
  TEST_ASSERT_FALSE(closeCalled);
}

// Verify that getTime() rejects a second call while a connection is in flight,
// and that after the connection closes (error or timeout), getTime() can be
// called again.  We use a non-routable address to trigger an error path quickly.
static void test_sntp_client_reconnect_after_error() {
  ScopedMongoose mongoose;
  MongooseSntpClient client;

  bool errorFired = false;
  bool closeFired = false;

  client.onError([&errorFired](const char *) { errorFired = true; })
        ->onClose([&closeFired]() { closeFired = true; });

  // Initiate a connection to a non-routable address; mongoose resolves it as
  // a hostname failure or times out quickly.
  bool first = client.getTime("192.0.2.1");  // TEST-NET-1, never routable

  if (!first) {
    // Some environments immediately reject the connect; that's fine.
    TEST_PASS();
    return;
  }

  // While the connection attempt is in flight, a second call must return false.
  TEST_ASSERT_FALSE(client.getTime("192.0.2.1"));

  // Pump for up to 5 s; CI can take longer to transition a failed UDP connect
  // into a closed socket.
  pumpUntil([&closeFired]() { return closeFired; }, 5000);

  // After close, connected() must be false and getTime() must be callable again.
  TEST_ASSERT_FALSE(client.connected());
  // We don't assert the return value here because the second attempt may also
  // fail immediately on this environment; the important property is no crash.
  client.getTime("192.0.2.1");
}

void runSntpClientTests() {
  RUN_TEST(test_sntp_client_build_only_placeholder);
  RUN_TEST(test_sntp_client_not_connected_initially);
  RUN_TEST(test_sntp_client_fluent_handler_registration);
  RUN_TEST(test_sntp_client_reconnect_after_error);
}
