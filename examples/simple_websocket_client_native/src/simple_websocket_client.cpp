//
// A simple native WebSocket client example showing how to:
//  * connect to a WebSocket server
//  * send and receive messages
//  * handle automatic reconnection
//  * use heartbeat/ping monitoring
//

#include <MongooseCore.h>
#include <MongooseWebSocketClient.h>
#include <stdio.h>
#include <signal.h>

MongooseWebSocketClient wsClient;
bool running = true;

void signal_handler(int sig) {
  (void)sig;
  running = false;
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <WebSocket URL>\n", argv[0]);
    fprintf(stderr, "Example: %s ws://echo.websocket.org\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  Mongoose.begin();

  // Set up WebSocket event handlers
  wsClient.setOnOpen([](MongooseWebSocketClient *client) {
    printf("WebSocket connected!\n");
    
    // Send a test message once connected
    const char *msg = "Hello from ArduinoMongoose Native!";
    client->sendTXT(msg, strlen(msg));
    printf("Sent: %s\n", msg);
  });

  wsClient.setReceiveTXTcallback([](int flags, const uint8_t *data, size_t len) {
    // Print received message
    printf("Received: %.*s\n", (int)len, (const char*)data);
  });

  wsClient.setOnClose([](int code, const char *reason) {
    printf("WebSocket closed: code=%d, reason=%s\n", code, reason);
  });

  // Configure reconnection and heartbeat
  wsClient.setReconnectInterval(5000);  // Reconnect every 5 seconds if disconnected
  wsClient.setPingInterval(15000);      // Send PING every 15 seconds
  wsClient.setStaleTimeout(30000);      // Close if no messages for 30 seconds

  // Connect to WebSocket server
  printf("Connecting to %s\n", argv[1]);
  if (!wsClient.connect(argv[1])) {
    fprintf(stderr, "Failed to initiate connection\n");
    return 1;
  }

  unsigned long next_send = 0;
  int message_count = 0;

  // Main loop
  while(running) {
    // Poll mongoose for network events (10ms timeout)
    Mongoose.poll(10);
    
    // Call wsClient.loop() to handle reconnection, heartbeat, etc.
    wsClient.loop();

    // Get current time (using mongoose's time function)
    unsigned long now = mg_time() * 1000;  // Convert to milliseconds
    
    // Send a message every 10 seconds
    if(wsClient.isConnectionOpen() && now >= next_send) {
      char msg[128];
      snprintf(msg, sizeof(msg), "Message #%d from native client", ++message_count);
      wsClient.sendTXT(msg, strlen(msg));
      printf("Sent: %s\n", msg);
      next_send = now + 10000;
    }
  }

  printf("\nShutting down...\n");
  wsClient.disconnect();
  
  // Give time for clean shutdown
  for(int i = 0; i < 10; i++) {
    Mongoose.poll(100);
  }

  return 0;
}
