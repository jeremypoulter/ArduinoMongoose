#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "MongooseHttpWebSocketConnection.h"
#include "MongooseHttpServerEndpointWebSocket.h"

MongooseHttpWebSocketConnection::MongooseHttpWebSocketConnection(mg_connection *nc, HttpRequestMethodComposite method, mg_http_message *msg, MongooseHttpServerEndpoint *endpoint) :
  MongooseHttpServerRequest(nc, method, msg, endpoint)
{
  // Upgrade to websocket. From now on, a connection is a full-duplex
  // Websocket connection, which will receive MG_EV_WS_MSG events.
  mg_ws_upgrade(nc, msg, NULL);
}

MongooseHttpWebSocketConnection::~MongooseHttpWebSocketConnection()
{

}

void MongooseHttpWebSocketConnection::send(int op, const void *data, size_t len)
{
  mg_ws_send(getConnection(), data, len, op);
}

void MongooseHttpWebSocketConnection::handleWebSocketConnect(mg_connection *nc, mg_http_message *msg)
{
  MongooseHttpServerEndpointWebSocket *ws_endpoint = (MongooseHttpServerEndpointWebSocket *)_endpoint;
  ws_endpoint->handleConnect(this);
}

void MongooseHttpWebSocketConnection::handleWebSocketMessage(mg_connection *nc, mg_ws_message *msg)
{
  MongooseHttpServerEndpointWebSocket *ws_endpoint = (MongooseHttpServerEndpointWebSocket *)_endpoint;
  ws_endpoint->handleFrame(this, msg->flags, (uint8_t *)msg->data.buf, msg->data.len);
}

void MongooseHttpWebSocketConnection::handleWebSocketControl(mg_connection *nc, mg_ws_message *msg)
{
}
