#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MongooseHttpServerConnection.h>


void MongooseHttpServerConnection::onEvent(mg_connection *nc, int ev, void *p)
{
  switch (ev)
  {
    case MG_EV_HTTP_HDRS:
    {
      struct mg_http_message *hm = (struct mg_http_message *) p;
      onHeaders(nc, hm);
      break;
    }

    case MG_EV_HTTP_MSG:
    {
      struct mg_http_message *hm = (struct mg_http_message *) p;
      onMessage(nc, hm);
      break;
    }

    case MG_EV_WS_OPEN:
    {
      struct mg_http_message *hm = (struct mg_http_message *) p;
      onWebSocketConnect(nc, hm);
      break;
    }

    case MG_EV_WS_MSG:
    {
      struct mg_ws_message *wm = (struct mg_ws_message *) p;
      onWebSocketMessage(nc, wm);
      break;
    }

    case MG_EV_WS_CTL:
    {
      struct mg_ws_message *wm = (struct mg_ws_message *) p;
      onWebSocketControl(nc, wm);
      break;
    }
  }
}

