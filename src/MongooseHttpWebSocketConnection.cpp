#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "MongooseHttpWebSocketConnection.h"

MongooseHttpWebSocketConnection::MongooseHttpWebSocketConnection(mg_connection *nc, HttpRequestMethodComposite method, mg_http_message *msg, MongooseHttpServerEndpoint *endpoint) :
  MongooseHttpServerRequest(nc, method, msg, endpoint)
{
}

MongooseHttpWebSocketConnection::~MongooseHttpWebSocketConnection()
{

}

void MongooseHttpWebSocketConnection::send(int op, const void *data, size_t len)
{
//  mg_send_websocket_frame(_nc, op, data, len);
}
