#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_SOCKET)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>

#include "MongooseCore.h"
#include "MongooseSocket.h"

MongooseSocket::MongooseSocket() :
  _nc(nullptr)
{

}

MongooseSocket::~MongooseSocket()
{
}

void MongooseSocket::eventHandler(struct mg_connection *nc, int ev, void *p, void *u)
{
  MongooseSocket *self = (MongooseSocket *)u;
  self->eventHandler(nc, ev, p);
}

void MongooseSocket::eventHandler(struct mg_connection *nc, int ev, void *p)
{
  if (ev != MG_EV_POLL) {
    DBUGF("%s %p: %d", __PRETTY_FUNCTION__, nc, ev);
  }

  if(_nc != nc) {
    DBUGF("Ignoring event for unknown connection %p", nc);
    return;
  }

  switch (ev) 
  {
    case MG_EV_CONNECT:
    {
      int err = *((int *)p);
      DBUGF("MG_EV_CONNECT, error = %d", err);
      onConnect(err);
      break;
    }

    case MG_EV_RECV:
    {
      int num_bytes = *(int *)p;
      DBUGF("MG_EV_RECV, bytes = %d", num_bytes);
      onReceive(num_bytes);
      break;
    }

    case MG_EV_SEND:
    {
      int num_bytes = *(int *)p;
      DBUGF("MG_EV_SEND, bytes = %d", num_bytes);
      onSend(num_bytes);
      break;
    }

    case MG_EV_CLOSE:
    {
      DBUGF("Connection %p closed", nc);
      onClose();
      _nc = nullptr;
      break;
    }

    default:
    {
      onEvent(ev, p);
      break;
    }
  }
}

void MongooseSocket::onConnect(int error)
{
  DBUGF("Connected with error %d", error);
}

void MongooseSocket::onReceive(int num_bytes)
{
  DBUGF("Received %d bytes", num_bytes);
}

void MongooseSocket::onSend(int num_bytes)
{
  DBUGF("Sent %d bytes", num_bytes);
}

void MongooseSocket::onClose()
{
  DBUGF("Connection closed");
}

void MongooseSocket::onEvent(int ev, void *p)
{
  DBUGF("Unhandled event %d", ev);
}

bool MongooseSocket::connect(mg_connection *nc)
{
  _nc = nc;
  return nc != nullptr;
}

void MongooseSocket::disconnect()
{
  if(_nc) {
    _nc->flags |= MG_F_SEND_AND_CLOSE;
  }
}