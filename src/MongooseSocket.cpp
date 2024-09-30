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
  _onError(NULL),
  _onClose(NULL)
{

}

MongooseSocket::MongooseSocket(mg_connection *nc) :
  _nc(nc),
  _onError(NULL),
  _onClose(NULL)
{

}

MongooseSocket::~MongooseSocket()
{
}

void MongooseSocket::eventHandler(mg_connection *nc, int ev, void *p, void *u)
{
  MongooseSocket *self = (MongooseSocket *)u;
  self->eventHandler(nc, ev, p);
}

void MongooseSocket::eventHandler(mg_connection *nc, int ev, void *p)
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

    case MG_EV_CONNECT:
    {
      int err = *((int *)p);
      if(0 == err) {
        DBUGF("MG_EV_CONNECT, success");
        onConnect(nc);
      } else {
        DBUGF("MG_EV_CONNECT, error = %d", err);
        onError(nc, err);
      }
      break;
    }

    case MG_EV_ACCEPT:
    {
      socket_address *sa = (socket_address *)p;
      DBUGF("MG_EV_ACCEPT, new connection");
      onAccept(nc, sa);
      break;
    }

    case MG_EV_RECV:
    {
      int num_bytes = *(int *)p;
      DBUGF("MG_EV_RECV, bytes = %d", num_bytes);
      onReceive(nc, num_bytes);
      break;
    }

    case MG_EV_SEND:
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

void MongooseSocket::onConnect(mg_connection *nc)
{
  DBUGF("Successfully Connected");
}

void MongooseSocket::onPoll(mg_connection *nc)
{
}

void MongooseSocket::onError(mg_connection *nc, int error)
{
  DBUGF("Socket with error %d", error);
  if(_onError) {
    _onError(error);
  }
}

void MongooseSocket::onAccept(mg_connection *nc, socket_address *sa)
{
  char addr[32];
  mg_sock_addr_to_str(sa, addr, sizeof(addr),
                      MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
  DBUGF("Accepted connection from %s", addr);
}

void MongooseSocket::onReceive(mg_connection *nc, int num_bytes)
{
  DBUGF("Received %d bytes", num_bytes);
}

void MongooseSocket::onSend(mg_connection *nc, int num_bytes)
{
  DBUGF("Sent %d bytes", num_bytes);
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
  if(!connected() && _nc) {
    _nc = nc;
    return true;
  }

  return false;
}

bool MongooseSocket::bind(uint16_t port)
{
  struct mg_bind_opts bind_opts;
  memset(&bind_opts, 0, sizeof(bind_opts));
  return bind(port, bind_opts);
}

#if MG_ENABLE_SSL
bool MongooseHttpServer::begin(uint16_t port, const char *cert, const char *private_key)
{
  struct mg_bind_opts bind_opts;
  const char *err;

  memset(&bind_opts, 0, sizeof(bind_opts));
  bind_opts.ssl_cert = cert;
  bind_opts.ssl_key = private_key;
  bind_opts.error_string = &err;

  return bind(port, bind_opts);
}
#endif

bool MongooseSocket::bind(uint16_t port, mg_bind_opts opts)
{
  if(!connected())
  {
    char s_http_port[6];
    utoa(port, s_http_port, 10);

    _nc = mg_bind_opt(Mongoose.getMgr(), s_http_port, eventHandler, this, opts);
    if(_nc) {
      return true;
    }
  }

  return false;
}


void MongooseSocket::setFlags(unsigned long mask, unsigned long flags)
{
  // IMPROVE: check that only the user flags are set
  if(_nc) {
    _nc->flags = (_nc->flags & ~mask) | flags;
  }
}
