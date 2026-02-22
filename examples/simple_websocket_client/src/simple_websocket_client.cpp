//
// A simple WebSocket client example showing how to:
//  * connect to a WebSocket server
//  * send and receive messages
//  * handle automatic reconnection
//  * use heartbeat/ping monitoring
//

#include <Arduino.h>
#include <MicroDebug.h>
#include <MongooseCore.h>
#include <MongooseWebSocketClient.h>

#ifdef ESP32
#include <WiFi.h>
#define START_ESP_WIFI
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#define START_ESP_WIFI
#else
#error Platform not supported
#endif

// WebSocket server (echo.websocket.org or your own WebSocket server)
#define WS_HOST "ws://echo.websocket.org"

MongooseWebSocketClient wsClient;

// Replace with your WiFi credentials
const char *ssid = "wifi";
const char *password = "password";

void setup()
{
  Serial.begin(115200);
  delay(1000);

#ifdef START_ESP_WIFI
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
#elif defined(ESP8266)
  Serial.println(WiFi.hostname());
#endif
#endif

  Mongoose.begin();

  // Set up WebSocket event handlers
  wsClient.setOnOpen([](MongooseWebSocketClient *client) {
    DBUGLN("WebSocket connected!");
    
    // Send a test message once connected
    const char *msg = "Hello from ArduinoMongoose!";
    client->sendTXT(msg, strlen(msg));
    DBUGF("Sent: %s", msg);
  });

  wsClient.setReceiveTXTcallback([](int flags, const uint8_t *data, size_t len) {
    // Print received message (properly handle non-null-terminated data)
    // Use a fixed buffer to avoid VLA issues, truncate if message is too large
    const size_t maxLen = 512;
    char buffer[maxLen + 1];
    size_t copyLen = len > maxLen ? maxLen : len;
    memcpy(buffer, data, copyLen);
    buffer[copyLen] = '\0';
    String message = String(buffer);
    if (len > maxLen) {
      DBUGF("Received (truncated): %s...", message.c_str());
    } else {
      DBUGF("Received: %s", message.c_str());
    }
  });

  wsClient.setOnClose([](int code, const char *reason) {
    DBUGF("WebSocket closed: code=%d, reason=%s", code, reason);
  });

  // Configure reconnection and heartbeat
  wsClient.setReconnectInterval(5000);  // Reconnect every 5 seconds if disconnected
  wsClient.setPingInterval(15000);      // Send PING every 15 seconds
  wsClient.setStaleTimeout(30000);      // Close if no messages for 30 seconds

  // Connect to WebSocket server
  DBUGF("Connecting to %s", WS_HOST);
  wsClient.connect(WS_HOST);
}

static unsigned long next_time = 0;
void loop()
{
  // Poll mongoose for network events
  Mongoose.poll(10);
  
  // Call wsClient.loop() to handle reconnection, heartbeat, etc.
  wsClient.loop();

  // Send a message every 10 seconds
  unsigned long now = millis();
  if(wsClient.isConnectionOpen() && now >= next_time) {
    String msg = "Uptime: " + String(now / 1000) + " seconds";
    wsClient.sendTXT(msg.c_str(), msg.length());
    DBUGF("Sent: %s", msg.c_str());
    next_time = now + 10000;
  }
}
