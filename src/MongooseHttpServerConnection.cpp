#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>

#include <MongooseHttpServerConnection.h>

void MongooseHttpServerConnection::handleEvent(mg_connection *nc, int ev, void *p)
{
  switch (ev)
  {
    case MG_EV_HTTP_HDRS:
    {
      DBUGF("MG_EV_HTTP_HDRS");
      struct mg_http_message *hm = (struct mg_http_message *) p;
      handleHeaders(nc, hm);
      break;
    }

    case MG_EV_HTTP_MSG:
    {
      DBUGF("MG_EV_HTTP_MSG");
      struct mg_http_message *hm = (struct mg_http_message *) p;
      handleMessage(nc, hm);
      break;
    }

    case MG_EV_WS_OPEN:
    {
      DBUGF("MG_EV_WS_OPEN");
      struct mg_http_message *hm = (struct mg_http_message *) p;
      handleWebSocketConnect(nc, hm);
      break;
    }

    case MG_EV_WS_MSG:
    {
      DBUGF("MG_EV_WS_MSG");
      struct mg_ws_message *wm = (struct mg_ws_message *) p;
      handleWebSocketMessage(nc, wm);
      break;
    }

    case MG_EV_WS_CTL:
    {
      DBUGF("MG_EV_WS_CTL");
      struct mg_ws_message *wm = (struct mg_ws_message *) p;
      handleWebSocketControl(nc, wm);
      break;
    }
  }
}

