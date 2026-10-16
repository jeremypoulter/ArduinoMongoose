//
// A simple WebSocket client example showing how to:
//  * connect to a WebSocket server
//  * send and receive messages
//  * handle automatic reconnection
//  * use heartbeat/ping monitoring
//

#ifdef ARDUINO
#include <Arduino.h>
#include <MicroDebug.h>
#else
#include <signal.h>
#include <stdlib.h>
#endif
#include <MongooseCore.h>
#include <MongooseWebSocketClient.h>
#include <stdio.h>
#include <string.h>

#if defined(ESP32)
#include <WiFi.h>
#define START_WIFI
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#define START_WIFI
#endif

#ifndef LOGF
#ifdef ARDUINO
#define LOGF Serial.printf
#else
#define LOGF printf
#endif
#endif

MongooseWebSocketClient wsClient;

// Replace with your WiFi credentials
#ifdef START_WIFI
const char *ssid = "wifi";
const char *password = "password";
#endif

#ifndef ARDUINO
static bool running = true;
#endif

static bool setupClient(const char *url)
{
  Mongoose.begin();

  // Set up WebSocket event handlers
  wsClient.onOpen([](MongooseWebSocketClient *client) {
    LOGF("WebSocket connected!\n");

    const char *msg = "Hello from ArduinoMongoose!";
    client->sendTXT(msg, strlen(msg));
    LOGF("Sent: %s\n", msg);
  });

  wsClient.onMessage([](int flags, const uint8_t *data, size_t len) {
    const size_t maxLen = 512;
    char buffer[maxLen + 1];
    size_t copyLen = len > maxLen ? maxLen : len;
    memcpy(buffer, data, copyLen);
    buffer[copyLen] = '\0';
    if (len > maxLen) {
      LOGF("Received (truncated): %s...\n", buffer);
    } else {
      LOGF("Received: %s\n", buffer);
    }
  });

  wsClient.setOnClose([](int code, const char *reason) {
    LOGF("WebSocket closed: code=%d, reason=%s\n", code, reason);
  });

  wsClient.setReconnectInterval(5000);
  wsClient.setPingInterval(15000);
  wsClient.setStaleTimeout(30000);

  LOGF("Connecting to %s\n", url);
  return wsClient.connect(url);
}

#ifndef ARDUINO
static void signal_handler(int sig)
{
  (void)sig;
  running = false;
}
#endif

static unsigned long next_time = 0;
static int message_count = 0;

static void poll()
{
  Mongoose.poll(10);
  wsClient.loop();

  unsigned long now = (unsigned long)mg_millis();
  if (wsClient.connected() && now >= next_time) {
    char msg[128];
    snprintf(msg, sizeof(msg), "Message #%d from client", ++message_count);
    wsClient.sendTXT(msg, strlen(msg));
    LOGF("Sent: %s\n", msg);
    next_time = now + 10000;
  }
}

#ifdef ARDUINO
void setup()
{
  Serial.begin(115200);
  delay(1000);

#ifdef START_WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname: ");
#ifdef ESP32
  Serial.println(WiFi.getHostname());
#else
  Serial.println(WiFi.hostname());
#endif
#endif

  setupClient("ws://echo.websocket.org");
}

void loop()
{
  poll();
}
#else
int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <WebSocket URL>\n", argv[0]);
    fprintf(stderr, "Example: %s ws://echo.websocket.org\n", argv[0]);
    return EXIT_FAILURE;
  }

  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  if (!setupClient(argv[1])) {
    fprintf(stderr, "Failed to initiate connection\n");
    return EXIT_FAILURE;
  }

  while (running) {
    poll();
  }

  LOGF("\nShutting down...\n");
  wsClient.disconnect();
  for (int i = 0; i < 10; i++) {
    Mongoose.poll(100);
  }

  return 0;
}
#endif
