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

    virtual void onConnect();
    virtual void onError(int error);
    virtual void onReceive(int num_bytes);
    virtual void onSend(int num_bytes);
    virtual void onClose();
    virtual void onEvent(int ev, void *p);

    bool connect(mg_connection *nc);

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
     