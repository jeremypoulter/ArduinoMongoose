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
    void disconnect();

    mg_connection *getConnection() {
      return _nc;
    }

  public:
    MongooseSocket();
    ~MongooseSocket();

    virtual bool connected() {
      return _nc;
    }
    void onError(MongooseSocketErrorHandler fnHandler) {
      _onError = fnHandler;
    }
    void onClose(MongooseSocketCloseHandler fnHandler) {
      _onClose = fnHandler;
    }
};

#endif // MongooseSocket_h
     