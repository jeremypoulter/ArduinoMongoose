#include <unity.h>

#include <MongooseMqttClient.h>

#include <cstdlib>
#include <string>

#include "test_support.h"

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

void runMqttClientTests() {
  RUN_TEST(test_mqtt_round_trip_with_local_broker);
}
