#ifndef MongooseSocket_h
#define MongooseSocket_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include <mongoose.h>

#include <MongooseString.h>

#include <functional>

#define MONGOOSE_SOCKET_TYPE    0

typedef std::function<void(const char *error)> MongooseSocketErrorHandler;
typedef std::function<void()> MongooseSocketCloseHandler;

class MongooseSocket
{
  private:
    mg_connection *_nc;

    MongooseSocketErrorHandler _onError;
    MongooseSocketCloseHandler _onClose;

    bool _secure;
    bool _reject_unauthorized;
    MongooseString _host;
    MongooseString _cert;
    MongooseString _key;

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
    virtual void handleEvent(mg_connection *nc, int ev, void *p);

    bool connect(mg_connection *nc);
//    bool bind(uint16_t port);
//    bool bind(uint16_t port, const char *cert, const char *private_key);
//    bool bind(uint16_t port, mg_bind_opts opts);

    void setSecure() {
      _secure = true;
    }

    void setSecure(const char *host) {
      setSecure(mg_str_s(host));
    }
    void setSecure(mg_str host) {
      _secure = true;
      _host = host;
    }

    void setCertificate(const char *cert, const char *key) {
      _secure = true;
      _cert = mg_str_s(cert);
      _key = mg_str_s(key);
    }

    void setRejectUnauthorized(bool reject) {
      _reject_unauthorized = reject;
    }

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

    mg_connection *getConnection() {
      return _nc;
    }

    mg_addr *getRemoteAddress() {
      return &_nc->rem;
    }

    mg_addr *getLocalAddress() {
      return &_nc->loc;
    }

    static const char Type = 'S';
    virtual char getType() {
      return Type;
    }
};

#endif // MongooseSocket_h
     