#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_SOCKET)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#else
#define utoa(i, buf, base) sprintf(buf, "%u", i)
#endif

#include <MicroDebug.h>

#include "MongooseCore.h"
#include "MongooseSocket.h"

MongooseSocket::MongooseSocket() :
  _nc(nullptr),
  _onError(nullptr),
  _onClose(nullptr)
{

}

MongooseSocket::MongooseSocket(mg_connection *nc) :
  _nc(nc),
  _onError(nullptr),
  _onClose(nullptr)
{

}

MongooseSocket::~MongooseSocket()
{
}

void MongooseSocket::eventHandler(mg_connection *nc, int ev, void *p)
{
  MongooseSocket *self = (MongooseSocket *)nc->fn_data;
  self->processEvent(nc, ev, p);
}

void MongooseSocket::processEvent(mg_connection *nc, int ev, void *p)
{
  if (ev != MG_EV_POLL) {
    DBUGF("%s %p: %d", __PRETTY_FUNCTION__, nc, ev);
  }

  switch (ev) 
  {
    case MG_EV_POLL:
    {
      onPoll(nc);
      break;
    }

    case MG_EV_OPEN:
    {
      DBUGF("MG_EV_OPEN");
      onOpen(nc);
      break;
    }

    case MG_EV_CONNECT:
    {
      DBUGF("MG_EV_CONNECT");
      onConnect(nc);
      break;
    }

    case MG_EV_ERROR:
    {
      const char *err = (const char *)p;
      DBUGF("MG_EV_ERROR, error = %s", err);
      onError(nc, err);
      break;
    }

    case MG_EV_ACCEPT:
    {
      DBUGF("MG_EV_ACCEPT, new connection");
      onAccept(nc);
      break;
    }

    case MG_EV_READ:
    {
      int num_bytes = *(int *)p;
      DBUGF("MG_EV_RECV, bytes = %d", num_bytes);
      onReceive(nc, num_bytes);
      break;
    }

    case MG_EV_WRITE:
    {
      int num_bytes = *(int *)p;
      DBUGF("MG_EV_SEND, bytes = %d", num_bytes);
      onSend(nc, num_bytes);
      break;
    }

    case MG_EV_CLOSE:
    {
      DBUGF("Connection %p closed", nc);
      if(_nc == nc) {
        _nc = nullptr;
      }
      onClose(nc);
      break;
    }

    default:
    {
      onEvent(nc, ev, p);
      break;
    }
  }
}

void MongooseSocket::onOpen(mg_connection *nc)
{
  DBUGF("Connection open");
}

void MongooseSocket::onConnect(mg_connection *nc)
{
  DBUGF("Successfully Connected");
}

void MongooseSocket::onPoll(mg_connection *nc)
{
}

void MongooseSocket::onError(mg_connection *nc, const char *error)
{
  DBUGF("Socket with error %s", error);
  if(_onError) {
    _onError(error);
  }
}

void MongooseSocket::onAccept(mg_connection *nc)
{
  char addr[32];
  mg_snprintf(addr, sizeof(addr), "%M", mg_print_ip_port, &nc->rem);
  DBUGF("Accepted connection from %s", addr);
}

void MongooseSocket::onReceive(mg_connection *nc, long num_bytes)
{
  DBUGF("Received %ld bytes", num_bytes);
}

void MongooseSocket::onSend(mg_connection *nc, long num_bytes)
{
  DBUGF("Sent %ld bytes", num_bytes);
}

void MongooseSocket::onClose(mg_connection *nc)
{
  DBUGF("Connection closed");
  if(_onClose) {
    _onClose();
  }
}

void MongooseSocket::onEvent(mg_connection *nc, int ev, void *p)
{
  DBUGF("Unhandled event %d", ev);
}

bool MongooseSocket::connect(mg_connection *nc)
{
  if(!connected() && nc) {
    _nc = nc;
    return true;
  }

  return false;
}

//bool MongooseSocket::bind(uint16_t port)
//{
//  struct mg_bind_opts bind_opts;
//  memset(&bind_opts, 0, sizeof(bind_opts));
//  return bind(port, bind_opts);
//}
//
//#if MG_ENABLE_SSL
//bool MongooseSocket::bind(uint16_t port, const char *cert, const char *private_key)
//{
//  struct mg_bind_opts bind_opts;
//  const char *err;
//
//  memset(&bind_opts, 0, sizeof(bind_opts));
//  bind_opts.ssl_cert = cert;
//  bind_opts.ssl_key = private_key;
//  bind_opts.error_string = &err;
//
//  return bind(port, bind_opts);
//}
//#endif
//
//bool MongooseSocket::bind(uint16_t port, mg_bind_opts opts)
//{
//  if(!connected())
//  {
//    char s_http_port[6];
//    utoa(port, s_http_port, 10);
//
//    _nc = mg_bind_opt(Mongoose.getMgr(), s_http_port, eventHandler, this, opts);
//    if(_nc) {
//      return true;
//    }
//  }
//
//  return false;
//}


//void MongooseSocket::setFlags(unsigned long mask, unsigned long flags)
//{
//  // IMPROVE: check that only the user flags are set
//  if(_nc) {
//    _nc->flags = (_nc->flags & ~mask) | flags;
//  }
//}
