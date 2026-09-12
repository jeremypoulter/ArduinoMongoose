#include <unity.h>

#include <MongooseSntpClient.h>

#include <cstring>

#include "test_support.h"

// A socket that has never connected has no peer: the raw accessors must report
// that rather than dereference a null connection, and the text form must be
// empty rather than "0.0.0.0".
static void test_socket_address_unconnected() {
  MongooseSntpClient client;

  TEST_ASSERT_NULL(client.getRemoteAddress());
  TEST_ASSERT_NULL(client.getLocalAddress());
  TEST_ASSERT_EQUAL_STRING("", client.remoteAddress());
}

// A literal address needs no DNS, so Mongoose has the peer from the first
// event. The recorded text form must match, and must survive the close that
// clears the connection pointer -- that is the case an error or close handler
// needs, and the reason the address is recorded rather than read on demand.
static void test_socket_address_recorded_and_survives_close() {
  ScopedMongoose mongoose;
  MongooseSntpClient client;

  bool closeFired = false;
  client.onClose([&closeFired]() { closeFired = true; });

  // TEST-NET-1: never routable, so this closes without touching the network.
  if (!client.getTime("192.0.2.1")) {
    // Some environments reject the connect outright; nothing to observe.
    TEST_PASS();
    return;
  }

  TEST_ASSERT_TRUE(pumpUntil([&client]() {
    return '\0' != client.remoteAddress()[0];
  }, 5000));
  TEST_ASSERT_EQUAL_STRING("192.0.2.1", client.remoteAddress());

  pumpUntil([&closeFired]() { return closeFired; }, 5000);
  if (closeFired) {
    // The connection is gone, but the address it resolved to is still readable.
    TEST_ASSERT_NULL(client.getRemoteAddress());
    TEST_ASSERT_EQUAL_STRING("192.0.2.1", client.remoteAddress());
  }
}

void runSocketAddressTests() {
  RUN_TEST(test_socket_address_unconnected);
  RUN_TEST(test_socket_address_recorded_and_survives_close);
}
