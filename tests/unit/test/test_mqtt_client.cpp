#include <unity.h>

#include <MongooseMqttClient.h>

#include <cstdlib>
#include <string>

#include "test_support.h"

// ---------------------------------------------------------------------------
// API / build-only tests (no broker required)
// ---------------------------------------------------------------------------

// Exposes the protected handleEvent() so MG_EV_MQTT_OPEN can be injected
// directly, without a real broker to reject the connection.
class TestableMqttClient : public MongooseMqttClient {
  public:
    using MongooseMqttClient::handleEvent;
};

static void test_mqtt_api_setters_compile_and_do_not_crash() {
  MongooseMqttClient client;

  // setCertificate / setRejectUnauthorized are public since our PR
  client.setCertificate("fake-cert", "fake-key");
  client.setRejectUnauthorized(false);
  client.setRejectUnauthorized(true);

  // setLastWillAndTestament (correct spelling) and legacy spelling
  client.setLastWillAndTestament("status", "offline");
  client.setLastWillAndTestament("status", "offline", true);
  client.setLastWillAndTestimment("status", "offline");

  // onDisconnect handler registration
  bool disconnectFired = false;
  client.onDisconnect([&disconnectFired]() { disconnectFired = true; });
  TEST_ASSERT_FALSE(disconnectFired);  // not yet fired

  // Verify connected() returns false before any connect() call
  TEST_ASSERT_FALSE(client.connected());
}

static void test_mqtt_connack_code_tracks_broker_rejection() {
  ScopedMongoose mongoose;
  TestableMqttClient client;

  // No attempt made yet.
  TEST_ASSERT_EQUAL(0, client.connackCode());

  // A rejection (CONNACK_NOT_AUTHORIZED = 5) is recorded...
  int rejectCode = 5;
  client.handleEvent(nullptr, MG_EV_MQTT_OPEN, &rejectCode);
  TEST_ASSERT_EQUAL(5, client.connackCode());

  // ...and reset by the next connect() attempt, even one that never reaches
  // the broker (no Mongoose manager is running in this test, so this fails
  // synchronously) -- connackCode() must not still describe the last attempt.
  client.connect("127.0.0.1:1", "arduino-mongoose-connack-test");
  TEST_ASSERT_EQUAL(0, client.connackCode());

  // A non-zero ack that is not a broker rejection this library models (0
  // itself means success and is handled separately; nothing to inject there).
  // An accepted connection leaves connackCode() at 0.
  int acceptCode = 0;
  client.handleEvent(nullptr, MG_EV_MQTT_OPEN, &acceptCode);
  TEST_ASSERT_EQUAL(0, client.connackCode());

  // An MQTT 5 reason code (>= 0x80) is recorded the same way as a 3.1.1
  // CONNACK return code -- mqtt_cb() in mongoose.c hands us the raw wire byte
  // regardless of protocol version, so this is a real case, not merely
  // defensive.
  int mqtt5RejectCode = 0x87;  // MQTT5 "Not authorized"
  client.handleEvent(nullptr, MG_EV_MQTT_OPEN, &mqtt5RejectCode);
  TEST_ASSERT_EQUAL(0x87, client.connackCode());
}

// ---------------------------------------------------------------------------
// Network tests (gated by env var; require a local broker on 127.0.0.1:1883)
// ---------------------------------------------------------------------------

static void test_mqtt_round_trip_with_local_broker() {
  const char *enabled = std::getenv("ARDUINO_MONGOOSE_ENABLE_MQTT_TESTS");
  if (!enabled || std::string(enabled) != "1") {
    TEST_IGNORE_MESSAGE("MQTT tests are gated; set ARDUINO_MONGOOSE_ENABLE_MQTT_TESTS=1 with a broker on localhost:1883");
  }

  ScopedMongoose mongoose;
  MongooseMqttClient subscriber;
  MongooseMqttClient publisher;

  bool subscriberConnected = false;
  bool publisherConnected = false;
  bool received = false;
  std::string receivedTopic;
  std::string receivedPayload;

  const std::string topic = "arduino-mongoose/unit/" + std::to_string(static_cast<unsigned long long>(mg_millis()));
  const std::string payload = "mqtt-payload";

  subscriber.onMessage([&](const MongooseString topicValue, const MongooseString payloadValue) {
    received = true;
    receivedTopic.assign(topicValue.c_str(), topicValue.length());
    receivedPayload.assign(payloadValue.c_str(), payloadValue.length());
  });

  TEST_ASSERT_TRUE(subscriber.connect("127.0.0.1:1883", "arduino-mongoose-sub", [&]() {
    subscriberConnected = true;
  }));
  TEST_ASSERT_TRUE(publisher.connect("127.0.0.1:1883", "arduino-mongoose-pub", [&]() {
    publisherConnected = true;
  }));

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&]() { return subscriberConnected && publisherConnected; }, 5000),
      "MQTT clients failed to connect");

  TEST_ASSERT_TRUE(subscriber.subscribe(topic.c_str()));
  pumpFor(250);
  TEST_ASSERT_TRUE(publisher.publish(topic.c_str(), payload.c_str()));

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&]() { return received; }, 5000),
      "MQTT message was not received");

  TEST_ASSERT_EQUAL_STRING(topic.c_str(), receivedTopic.c_str());
  TEST_ASSERT_EQUAL_STRING(payload.c_str(), receivedPayload.c_str());
}

static void test_mqtt_subscribe_with_qos_and_disconnect_handler() {
  const char *enabled = std::getenv("ARDUINO_MONGOOSE_ENABLE_MQTT_TESTS");
  if (!enabled || std::string(enabled) != "1") {
    TEST_IGNORE_MESSAGE("MQTT tests are gated; set ARDUINO_MONGOOSE_ENABLE_MQTT_TESTS=1 with a broker on localhost:1883");
  }

  ScopedMongoose mongoose;
  MongooseMqttClient client;

  bool connected = false;
  bool disconnected = false;

  client.onDisconnect([&disconnected]() { disconnected = true; });

  TEST_ASSERT_TRUE(client.connect("127.0.0.1:1883", "arduino-mongoose-qos", [&connected]() {
    connected = true;
  }));

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&connected]() { return connected; }, 5000),
      "MQTT client failed to connect");

  // subscribe with explicit QoS 1
  const std::string topic = "arduino-mongoose/qos/" + std::to_string(static_cast<unsigned long long>(mg_millis()));
  TEST_ASSERT_TRUE(client.subscribe(topic.c_str(), 1));

  // Disconnect and verify onDisconnect fires
  client.disconnect();
  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&disconnected]() { return disconnected; }, 3000),
      "onDisconnect handler was not called after disconnect()");
}

void runMqttClientTests() {
  RUN_TEST(test_mqtt_api_setters_compile_and_do_not_crash);
  RUN_TEST(test_mqtt_connack_code_tracks_broker_rejection);
  RUN_TEST(test_mqtt_round_trip_with_local_broker);
  RUN_TEST(test_mqtt_subscribe_with_qos_and_disconnect_handler);
}
