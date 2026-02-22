#ifndef MongooseWebSocketClient_h
#define MongooseWebSocketClient_h

#ifdef ARDUINO
#include "Arduino.h"
#include <IPAddress.h>
#endif

#include <mongoose.h>
#include <functional>

// Forward declarations
class MongooseWebSocketClient;

// WebSocket opcodes (from mongoose.h)
#ifndef WEBSOCKET_OP_CONTINUE
#define WEBSOCKET_OP_CONTINUE 0
#define WEBSOCKET_OP_TEXT 1
#define WEBSOCKET_OP_BINARY 2
#define WEBSOCKET_OP_CLOSE 8
#define WEBSOCKET_OP_PING 9
#define WEBSOCKET_OP_PONG 10
#endif

// Callback types
typedef std::function<void(MongooseWebSocketClient *client)> MongooseWebSocketOpenHandler;
typedef std::function<void(int flags, const uint8_t *data, size_t len)> MongooseWebSocketMessageHandler;
typedef std::function<void(int code, const char *reason)> MongooseWebSocketCloseHandler;

/**
 * @brief WebSocket client following MicroOcppMongooseClient patterns
 * 
 * Provides reliable WebSocket connectivity with:
 * - Automatic reconnection with exponential backoff
 * - Heartbeat monitoring (PING/PONG)
 * - Stale connection detection
 * - Event-driven callbacks (non-blocking)
 * 
 * Usage:
 *   MongooseWebSocketClient client;
 *   client.connect("ws://192.168.1.100/ws");
 *   client.setReceiveTXTcallback([](const uint8_t *data, size_t len) {
 *     // Handle incoming message
 *   });
 *   
 *   // In loop():
 *   client.loop();  // Handles reconnection, ping, etc.
 */
class MongooseWebSocketClient
{
  private:
    // Mongoose connection
    struct mg_mgr *_mgr;
    struct mg_connection *_nc;
    
    // Connection state
    enum class State {
      DISCONNECTED,
      CONNECTING,
      CONNECTED,
      CLOSING,
      ERROR_STATE
    };
    State _state;
    
    // Configuration
    char *_url;
    char *_protocol;
    char *_extraHeaders;
    
    // Reconnection management
    unsigned long _reconnectInterval;      // ms between reconnect attempts (default 5000)
    unsigned long _lastReconnectAttempt;   // mg_millis() timestamp
    int _reconnectAttemptCount;            // exponential backoff counter
    
    // Heartbeat/ping management
    unsigned long _pingInterval;           // ms between PING frames (default 15000)
    unsigned long _lastPing;               // mg_millis() timestamp
    unsigned long _staleTimeout;           // ms without any message = stale (default 30000)
    unsigned long _lastRecv;               // mg_millis() timestamp of last received message
    unsigned long _lastConnected;          // mg_millis() timestamp when connection established
    
    // Callbacks
    MongooseWebSocketOpenHandler _onOpen;
    MongooseWebSocketMessageHandler _onMessage;
    MongooseWebSocketCloseHandler _onClose;
    
    // Static event handler (mongoose callback)
    static void eventHandler(struct mg_connection *nc, int ev, void *ev_data, void *user_data);
    
    // Instance event handler
    void handleEvent(struct mg_connection *nc, int ev, void *ev_data);
    
    // Internal helpers
    void cleanupConnection();
    void attemptReconnect();
    void sendPing();
    bool isStale();
    
  public:
    MongooseWebSocketClient();
    ~MongooseWebSocketClient();
    
    /**
     * @brief Establish WebSocket connection
     * @param url WebSocket URL (ws://host/path or wss://host/path)
     * @param protocol Optional Sec-WebSocket-Protocol header value
     * @param extraHeaders Optional additional HTTP headers (newline-separated)
     * @return true if connection initiated, false on error
     */
    bool connect(const char *url, const char *protocol = nullptr, const char *extraHeaders = nullptr);
    
    /**
     * @brief Gracefully close WebSocket connection
     */
    void disconnect();
    
    /**
     * @brief Must be called periodically from main loop or MicroTask
     * Handles reconnection, heartbeat, stale detection
     */
    void loop();
    
    /**
     * @brief Send text message (WebSocket TEXT opcode)
     * @param msg Text message to send
     * @param length Message length
     * @return true if sent successfully
     */
    bool sendTXT(const char *msg, size_t length);
    
    /**
     * @brief Send raw WebSocket frame
     * @param op WebSocket opcode (TEXT, BINARY, PING, PONG, CLOSE)
     * @param data Frame data
     * @param len Data length
     * @return true if sent successfully
     */
    bool send(int op, const void *data, size_t len);
    
    /**
     * @brief Register callback for WebSocket open event
     * Called after successful handshake (HTTP 101)
     */
    void setOnOpen(MongooseWebSocketOpenHandler handler) {
      _onOpen = handler;
    }
    
    /**
     * @brief Register callback for incoming messages
     * Called for TEXT and BINARY frames (auto-defragmented by mongoose)
     * 
     * Note: Despite the name, this callback is invoked for both TEXT and
     * BINARY frames. The flags parameter can be used to distinguish frame types.
     */
    void setReceiveTXTcallback(MongooseWebSocketMessageHandler handler) {
      _onMessage = handler;
    }
    
    /**
     * @brief Register callback for incoming messages (preferred name)
     * Called for TEXT and BINARY frames (auto-defragmented by mongoose)
     */
    void setOnMessage(MongooseWebSocketMessageHandler handler) {
      _onMessage = handler;
    }
    
    /**
     * @brief Register callback for connection close
     * Called on clean close or error disconnect
     */
    void setOnClose(MongooseWebSocketCloseHandler handler) {
      _onClose = handler;
    }
    
    /**
     * @brief Check if WebSocket is connected and ready
     */
    bool isConnectionOpen() const {
      return _state == State::CONNECTED;
    }
    
    /**
     * @brief Get milliseconds since last received message
     * Used for heartbeat timeout detection
     */
    unsigned long getLastRecv() const {
      return _lastRecv;
    }
    
    /**
     * @brief Get milliseconds when connection was established
     */
    unsigned long getLastConnected() const {
      return _lastConnected;
    }
    
    /**
     * @brief Configure reconnection interval (default 5000ms)
     * Exponential backoff applied: interval * 2^attempt (capped at 60s)
     */
    void setReconnectInterval(unsigned long ms) {
      _reconnectInterval = ms;
    }
    
    /**
     * @brief Configure PING interval (default 15000ms, 0 = disable)
     */
    void setPingInterval(unsigned long ms) {
      _pingInterval = ms;
    }
    
    /**
     * @brief Configure stale timeout (default 30000ms, 0 = disable)
     * Connection closed if no messages received within timeout
     */
    void setStaleTimeout(unsigned long ms) {
      _staleTimeout = ms;
    }
    
    /**
     * @brief Get reconnection interval
     */
    unsigned long getReconnectInterval() const {
      return _reconnectInterval;
    }
    
    /**
     * @brief Get PING interval
     */
    unsigned long getPingInterval() const {
      return _pingInterval;
    }
    
    /**
     * @brief Get stale timeout
     */
    unsigned long getStaleTimeout() const {
      return _staleTimeout;
    }
};

#endif // MongooseWebSocketClient_h
