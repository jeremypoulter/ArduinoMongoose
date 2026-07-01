#include "MongooseWebSocketClient.h"
#include "MongooseCore.h"

#include <string.h>

// Time helper: get current time in milliseconds
static inline unsigned long get_millis() {
#ifdef ARDUINO
  return millis();
#else
  return (unsigned long)mg_millis();
#endif
}

MongooseWebSocketClient::MongooseWebSocketClient() :
  _mgr(Mongoose.getMgr()),
  _nc(nullptr),
  _state(State::DISCONNECTED),
  _url(nullptr),
  _protocol(nullptr),
  _extraHeaders(nullptr),
  _reconnectInterval(5000),
  _lastReconnectAttempt(0),
  _reconnectAttemptCount(0),
  _pingInterval(15000),
  _lastPing(0),
  _staleTimeout(30000),
  _lastRecv(0),
  _lastConnected(0),
  _onOpen(nullptr),
  _onMessage(nullptr),
  _onClose(nullptr)
{
}

MongooseWebSocketClient::~MongooseWebSocketClient()
{
  disconnect();
  
  // Free allocated strings
  if (_url) {
    free(_url);
    _url = nullptr;
  }
  if (_protocol) {
    free(_protocol);
    _protocol = nullptr;
  }
  if (_extraHeaders) {
    free(_extraHeaders);
    _extraHeaders = nullptr;
  }
}

bool MongooseWebSocketClient::connect(const char *url, const char *protocol, const char *extraHeaders)
{
  if (!url) {
    return false;
  }
  
  // Disconnect existing connection
  if (_nc) {
    disconnect();
  }
  
  // Store configuration (copy strings for lifetime management)
  if (_url) free(_url);
  _url = strdup(url);
  
  if (_protocol) free(_protocol);
  _protocol = protocol ? strdup(protocol) : nullptr;
  
  if (_extraHeaders) free(_extraHeaders);
  _extraHeaders = extraHeaders ? strdup(extraHeaders) : nullptr;
  
  // Reset reconnection state
  _reconnectAttemptCount = 0;
  _lastReconnectAttempt = 0;
  
  // Initiate connection immediately
  attemptReconnect();
  
  return _nc != nullptr;
}

void MongooseWebSocketClient::disconnect()
{
  if (_nc) {
    _state = State::CLOSING;
    
    // Send CLOSE frame then drain
    mg_ws_send(_nc, nullptr, 0, WEBSOCKET_OP_CLOSE);
    _nc->is_draining = 1;
    
    // Keep _nc set until MG_EV_CLOSE arrives
  }
}

void MongooseWebSocketClient::loop()
{
  // Handle reconnection if disconnected or in error state
  if ((_state == State::DISCONNECTED || _state == State::ERROR_STATE) && _url && _reconnectInterval > 0) {
    attemptReconnect();
  }
  
  // Handle heartbeat (PING)
  if (_state == State::CONNECTED && _pingInterval > 0) {
    unsigned long now = get_millis();
    if (now - _lastPing >= _pingInterval) {
      sendPing();
    }
  }
  
  // Handle stale connection detection
  if (_state == State::CONNECTED && _staleTimeout > 0) {
    if (isStale()) {
      // Connection is stale, close and reconnect
      disconnect();
    }
  }
}

bool MongooseWebSocketClient::sendTXT(const char *msg, size_t length)
{
  return send(WEBSOCKET_OP_TEXT, msg, length);
}

bool MongooseWebSocketClient::send(int op, const void *data, size_t len)
{
  if (!_nc || _state != State::CONNECTED) {
    return false;
  }
  
  mg_ws_send(_nc, data, len, op);
  return true;
}

void MongooseWebSocketClient::eventHandler(struct mg_connection *nc, int ev, void *ev_data)
{
  MongooseWebSocketClient *client = (MongooseWebSocketClient *)nc->fn_data;
  if (client) {
    client->handleEvent(nc, ev, ev_data);
  }
}

void MongooseWebSocketClient::handleEvent(struct mg_connection *nc, int ev, void *ev_data)
{
  // Only process events for the currently-active connection.
  // This avoids stale connections (e.g. from a previous reconnect attempt)
  // from overwriting the state of the new connection.
  if (_nc && nc != _nc) {
    return;
  }
  
  switch (ev) {
    case MG_EV_ERROR: {
      // Connection or DNS error
      _state = State::ERROR_STATE;
      cleanupConnection();
      break;
    }

    case MG_EV_CONNECT: {
      // TCP connection established
      _state = State::CONNECTING;
      break;
    }
    
    case MG_EV_WS_OPEN: {
      // WebSocket upgrade successful (HTTP 101)
      _state = State::CONNECTED;
      _lastConnected = get_millis();
      _lastRecv = _lastConnected;
      _lastPing = _lastConnected;
      _reconnectAttemptCount = 0;
      
      if (_onOpen) {
        _onOpen(this);
      }
      break;
    }
    
    case MG_EV_WS_MSG: {
      // Incoming WebSocket data frame (auto-defragmented by mongoose)
      struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
      
      _lastRecv = get_millis();
      
      if (_onMessage && wm) {
        _onMessage(wm->flags, (const uint8_t *)wm->data.buf, wm->data.len);
      }
      break;
    }
    
    case MG_EV_WS_CTL: {
      // Control frame (PING, PONG, CLOSE)
      struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
      
      _lastRecv = get_millis();
      
      if (wm) {
        uint8_t opcode = wm->flags & 0x0F;
        
        if (opcode == WEBSOCKET_OP_PING) {
          // Respond with PONG
          mg_ws_send(_nc, wm->data.buf, wm->data.len, WEBSOCKET_OP_PONG);
        } else if (opcode == WEBSOCKET_OP_CLOSE) {
          // Peer initiated close
          _state = State::CLOSING;
        }
      }
      break;
    }
    
    case MG_EV_CLOSE: {
      // Connection closed (clean or error).
      //
      // NOTE: We currently do NOT propagate the actual WebSocket close code
      // or reason from the CLOSE control frame or underlying transport.
      // Instead, we always report a generic "normal closure" (1000,
      // "Connection closed") to the onClose callback. Callers MUST NOT rely
      // on these values to distinguish between normal, error, or
      // peer-initiated disconnects.
      int code = 1000;
      const char *reason = "Connection closed";
      
      if (_onClose) {
        _onClose(code, reason);
      }
      
      cleanupConnection();
      break;
    }
    
    default:
      break;
  }
}

void MongooseWebSocketClient::cleanupConnection()
{
  _nc = nullptr;
  _state = State::DISCONNECTED;
}

void MongooseWebSocketClient::attemptReconnect()
{
  if (!_url || (_state != State::DISCONNECTED && _state != State::ERROR_STATE)) {
    return;
  }
  
  // Check reconnection backoff
  unsigned long now = get_millis();
  
  // Compute exponential backoff safely without undefined shifts or overflow,
  // respecting the 60s cap.
  unsigned long baseInterval = _reconnectInterval ? _reconnectInterval : 1000UL;
  unsigned long maxBackoff   = 60000UL;
  unsigned long maxFactor    = maxBackoff / baseInterval;
  if (maxFactor == 0) {
    maxFactor = 1UL;
  }
  unsigned long factor = 1UL;
  for (unsigned int i = 0; i < _reconnectAttemptCount && factor < maxFactor; ++i) {
    factor <<= 1;
  }
  unsigned long backoffDelay = baseInterval * factor;
  if (backoffDelay > maxBackoff) {
    backoffDelay = maxBackoff;
  }
  
  if (_lastReconnectAttempt > 0 && (now - _lastReconnectAttempt) < backoffDelay) {
    return;  // Too soon, wait for backoff period
  }
  
  _lastReconnectAttempt = now;
  _reconnectAttemptCount++;
  
  // Build extra headers: optional Sec-WebSocket-Protocol + caller-supplied headers
  if (_protocol && _extraHeaders) {
    _nc = mg_ws_connect(_mgr, _url, eventHandler, this,
                        "Sec-WebSocket-Protocol: %s\r\n%s\r\n",
                        _protocol, _extraHeaders);
  } else if (_protocol) {
    _nc = mg_ws_connect(_mgr, _url, eventHandler, this,
                        "Sec-WebSocket-Protocol: %s\r\n", _protocol);
  } else if (_extraHeaders) {
    _nc = mg_ws_connect(_mgr, _url, eventHandler, this,
                        "%s\r\n", _extraHeaders);
  } else {
    _nc = mg_ws_connect(_mgr, _url, eventHandler, this, NULL);
  }
  
  if (_nc) {
    _state = State::CONNECTING;
  } else {
    _state = State::DISCONNECTED;
  }
}

void MongooseWebSocketClient::sendPing()
{
  if (_state == State::CONNECTED && _nc) {
    mg_ws_send(_nc, nullptr, 0, WEBSOCKET_OP_PING);
    _lastPing = get_millis();
  }
}

bool MongooseWebSocketClient::isStale()
{
  if (_staleTimeout == 0) {
    return false;
  }
  
  unsigned long now = get_millis();
  return (now - _lastRecv) > _staleTimeout;
}
