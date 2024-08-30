#ifndef MongooseSocket_h
#define MongooseSocket_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include <mongoose.h>

#include <functional>

class MongooseSocket
{
  private:
    mg_connection *_nc;

  protected:
    static void eventHandler(struct mg_connection *nc, int ev, void *p, void *u);
    void eventHandler(struct mg_connection *nc, int ev, void *p);

    virtual void onConnect(int error);
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
};

#endif // MongooseSocket_h
     