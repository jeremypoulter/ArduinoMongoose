#ifndef MongooseSocket_h
#define MongooseSocket_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include <mongoose.h>

#include <functional>

typedef std::function<void(const char *error)> MongooseSocketErrorHandler;
typedef std::function<void()> MongooseSocketCloseHandler;

class MongooseSocket
{
  private:
    mg_connection *_nc;

    MongooseSocketErrorHandler _onError;
    MongooseSocketCloseHandler _onClose;

    void processEvent(struct mg_connection *nc, int ev, void *p);
  protected:
    static void eventHandler(struct mg_connection *nc, int ev, void *p);

    virtual void onOpen(mg_connection *nc);
    virtual void onResolve(mg_connection *nc);
    virtual void onConnect(mg_connection *nc);
    virtual void onAccept(mg_connection *nc);
    virtual void onError(mg_connection *nc, const char *error);
    virtual void onReceive(mg_connection *nc, long num_bytes);
    virtual void onSend(mg_connection *nc, long num_bytes);
    virtual void onPoll(mg_connection *nc);
    virtual void onClose(mg_connection *nc);
    virtual void onEvent(mg_connection *nc, int ev, void *p);

    bool connect(mg_connection *nc);
//    bool bind(uint16_t port);
//#if MG_ENABLE_SSL
//    bool bind(uint16_t port, const char *cert, const char *private_key);
//#endif
//    bool bind(uint16_t port, mg_bind_opts opts);

    mg_connection *getConnection() {
      return _nc;
    }

//    void setFlags (unsigned long mask, unsigned long flags);
//    void setFlags(unsigned long flags) {
//      setFlags(flags, flags);
//    }
    void disconnect() {
      _nc->is_draining = 1;
    }
    void abort() {
      _nc->is_closing = 1;
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
     