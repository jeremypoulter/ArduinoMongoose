#include <unity.h>

#include <MongooseHttpServer.h>
#include <MongooseWebSocketClient.h>

#include <string>
#include <vector>

#include "test_support.h"

static void test_websocket_round_trip_and_disconnect() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18084));

  server.on("/ws", [](MongooseHttpWebSocketConnection *connection, int, uint8_t *data, size_t len) {
    std::string reply = "echo:" + std::string(reinterpret_cast<const char *>(data), len);
    connection->send(reply.c_str());
  });

  MongooseWebSocketClient client;
  client.setReconnectInterval(0);

  bool open = false;
  bool closed = false;
  std::string message;

  client.setOnOpen([&open, &client](MongooseWebSocketClient *) {
    open = true;
    const char *payload = "hello";
    client.sendTXT(payload, strlen(payload));
  });
  client.setOnMessage([&message](int, const uint8_t *data, size_t len) {
    message.assign(reinterpret_cast<const char *>(data), len);
  });
  client.setOnClose([&closed](int, const char *) { closed = true; });

  TEST_ASSERT_TRUE(client.connect("ws://127.0.0.1:18084/ws"));
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&open, &message]() { return open && message == "echo:hello"; },
                                     3000,
                                     [&client]() { client.loop(); }),
                           "WebSocket echo timed out");

  client.disconnect();
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }, 2000, [&client]() { client.loop(); }),
                           "WebSocket disconnect timed out");
}

static void test_websocket_sendall_broadcasts_to_other_clients() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18085));

  server.on("/ws", [&server](MongooseHttpWebSocketConnection *connection, int, uint8_t *data, size_t len) {
    std::string payload(reinterpret_cast<const char *>(data), len);
    if (payload == "broadcast") {
      server.sendAll(connection, "/ws", "fanout");
    }
  });

  MongooseWebSocketClient sender;
  MongooseWebSocketClient receiver;
  sender.setReconnectInterval(0);
  receiver.setReconnectInterval(0);

  bool senderOpen = false;
  bool receiverOpen = false;
  std::vector<std::string> receiverMessages;

  sender.setOnOpen([&senderOpen](MongooseWebSocketClient *) { senderOpen = true; });
  receiver.setOnOpen([&receiverOpen](MongooseWebSocketClient *) { receiverOpen = true; });
  receiver.setOnMessage([&receiverMessages](int, const uint8_t *data, size_t len) {
    receiverMessages.emplace_back(reinterpret_cast<const char *>(data), len);
  });

  TEST_ASSERT_TRUE(sender.connect("ws://127.0.0.1:18085/ws"));
  TEST_ASSERT_TRUE(receiver.connect("ws://127.0.0.1:18085/ws"));
  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&senderOpen, &receiverOpen]() { return senderOpen && receiverOpen; },
                3000,
                [&sender, &receiver]() {
                  sender.loop();
                  receiver.loop();
                }),
      "WebSocket clients did not connect");

  TEST_ASSERT_TRUE(sender.sendTXT("broadcast", strlen("broadcast")));
  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&receiverMessages]() { return !receiverMessages.empty(); },
                3000,
                [&sender, &receiver]() {
                  sender.loop();
                  receiver.loop();
                }),
      "WebSocket broadcast timed out");

  TEST_ASSERT_EQUAL_STRING("fanout", receiverMessages.front().c_str());
}

void runWebSocketTests() {
  RUN_TEST(test_websocket_round_trip_and_disconnect);
  RUN_TEST(test_websocket_sendall_broadcasts_to_other_clients);
}
