#ifndef MongooseSocket_h
#define MongooseSocket_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include <mongoose.h>

#include <functional>

typedef std::function<void(int retCode)> MongooseSocketErrorHandler;
typedef std::function<void()> MongooseSocketCloseHandler;

class MongooseSocket
{
  private:
    mg_connection *_nc;

    MongooseSocketErrorHandler _onError;
    MongooseSocketCloseHandler _onClose;

  protected:
    static void eventHandler(struct mg_connection *nc, int ev, void *p, void *u);
    void eventHandler(struct mg_connection *nc, int ev, void *p);

    virtual void onConnect(mg_connection *nc);
    virtual void onAccept(mg_connection *nc, socket_address *addr);
    virtual void onError(mg_connection *nc, int error);
    virtual void onReceive(mg_connection *nc, int num_bytes);
    virtual void onSend(mg_connection *nc, int num_bytes);
    virtual void onPoll(mg_connection *nc);
    virtual void onClose(mg_connection *nc);
    virtual void onEvent(mg_connection *nc, int ev, void *p);

    bool connect(mg_connection *nc);
    bool bind(uint16_t port);
#if MG_ENABLE_SSL
    bool bind(uint16_t port, const char *cert, const char *private_key);
#endif
    bool bind(uint16_t port, mg_bind_opts opts);

    mg_connection *getConnection() {
      return _nc;
    }

    void setFlags (unsigned long mask, unsigned long flags);
    void setFlags(unsigned long flags) {
      setFlags(flags, flags);
    }
    void disconnect() {
      setFlags(MG_F_SEND_AND_CLOSE);
    }
    void abort() {
      setFlags(MG_F_CLOSE_IMMEDIATELY);
    }

  public:
    MongooseSocket();
    MongooseSocket(mg_connection *nc);
    ~MongooseSocket();

    virtual bool connected() {
      return _nc;
    }
    MongooseSocket *onError(MongooseSocketErrorHandler fnHandler) {
      _onError = fnHandler;
      return this;
    }
    MongooseSocket *onClose(MongooseSocketCloseHandler fnHandler) {
      _onClose = fnHandler;
      return this;
    }
};

#endif // MongooseSocket_h
     