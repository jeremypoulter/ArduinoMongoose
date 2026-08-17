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
  MongooseSocket(),
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
  if (getConnection()) {
    getConnection()->fn_data = nullptr;
    MongooseSocket::abort();
    cleanupConnection();
  }
  
  // Store configuration (copy strings for lifetime management)
  if (_url) free(_url);
  _url = strdup(url);
  
  if (_protocol) free(_protocol);
  _protocol = protocol ? strdup(protocol) : nullptr;
  
  if (_extraHeaders) free(_extraHeaders);
  _extraHeaders = extraHeaders ? strdup(extraHeaders) : nullptr;

  if (mg_url_is_ssl(_url)) {
    setSecure(mg_url_host(_url));
  } else {
    clearSecurity();
  }
  
  // Reset reconnection state
  _reconnectAttemptCount = 0;
  _lastReconnectAttempt = 0;
  
  // Initiate connection immediately
  attemptReconnect();

  return getConnection() != nullptr;
}

void MongooseWebSocketClient::disconnect()
{
  if (getConnection()) {
    bool wasConnected = connected();
    _state = State::CLOSING;

    if (wasConnected) {
      mg_ws_send(getConnection(), nullptr, 0, WEBSOCKET_OP_CLOSE);
    }
    MongooseSocket::disconnect();
  }
}

void MongooseWebSocketClient::loop()
{
  // Handle reconnection if disconnected or in error state
  if ((_state == State::DISCONNECTED || _state == State::ERROR_STATE) && _url && _reconnectInterval > 0) {
    attemptReconnect();
  }
}

bool MongooseWebSocketClient::sendTXT(const char *msg, size_t length)
{
  return send(WEBSOCKET_OP_TEXT, msg, length);
}

bool MongooseWebSocketClient::send(int op, const void *data, size_t len)
{
  if (!connected()) {
    return false;
  }
  
  mg_ws_send(getConnection(), data, len, op);
  return true;
}

void MongooseWebSocketClient::onConnect(mg_connection *nc)
{
  _state = State::CONNECTING;
  MongooseSocket::onConnect(nc);
}

void MongooseWebSocketClient::onPoll(mg_connection *nc)
{
  (void) nc;

  if (_state == State::CONNECTED && _pingInterval > 0) {
    unsigned long now = get_millis();
    if (now - _lastPing >= _pingInterval) {
      sendPing();
    }
  }

  if (_state == State::CONNECTED && _staleTimeout > 0 && isStale()) {
    disconnect();
  }
}

void MongooseWebSocketClient::onError(mg_connection *nc, const char *error)
{
  _state = State::ERROR_STATE;
  MongooseSocket::onError(nc, error);
  nc->fn_data = nullptr;
  cleanupConnection();
}

void MongooseWebSocketClient::handleEvent(struct mg_connection *nc, int ev, void *ev_data)
{
  switch (ev) {
    case MG_EV_WS_OPEN: {
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
      struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
      _lastRecv = get_millis();
      if (_onMessage && wm) {
        _onMessage(wm->flags, (const uint8_t *)wm->data.buf, wm->data.len);
      }
      break;
    }

    case MG_EV_WS_CTL: {
      struct mg_ws_message *wm = (struct mg_ws_message *)ev_data;
      _lastRecv = get_millis();
      if (wm) {
        uint8_t opcode = wm->flags & 0x0F;

        if (opcode == WEBSOCKET_OP_PING) {
          mg_ws_send(getConnection(), wm->data.buf, wm->data.len, WEBSOCKET_OP_PONG);
        } else if (opcode == WEBSOCKET_OP_CLOSE) {
          _state = State::CLOSING;
        }
      }
      break;
    }

    default:
      break;
  }
}

void MongooseWebSocketClient::onClose(mg_connection *nc)
{
  if (_onClose) {
    _onClose(1000, "Connection closed");
  }

  cleanupConnection();
  MongooseSocket::onClose(nc);
}

void MongooseWebSocketClient::cleanupConnection()
{
  clearConnection();
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
  
  mg_connection *nc = nullptr;
  if (_protocol && _extraHeaders) {
    nc = mg_ws_connect(Mongoose.getMgr(), _url, MongooseSocket::eventHandler, this,
                       "Sec-WebSocket-Protocol: %s\r\n%s\r\n",
                       _protocol, _extraHeaders);
  } else if (_protocol) {
    nc = mg_ws_connect(Mongoose.getMgr(), _url, MongooseSocket::eventHandler, this,
                       "Sec-WebSocket-Protocol: %s\r\n", _protocol);
  } else if (_extraHeaders) {
    nc = mg_ws_connect(Mongoose.getMgr(), _url, MongooseSocket::eventHandler, this,
                       "%s\r\n", _extraHeaders);
  } else {
    nc = mg_ws_connect(Mongoose.getMgr(), _url, MongooseSocket::eventHandler, this, NULL);
  }
  
  if (MongooseSocket::connect(nc)) {
    _state = State::CONNECTING;
  } else {
    _state = State::DISCONNECTED;
  }
}

void MongooseWebSocketClient::sendPing()
{
  if (connected()) {
    mg_ws_send(getConnection(), nullptr, 0, WEBSOCKET_OP_PING);
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
