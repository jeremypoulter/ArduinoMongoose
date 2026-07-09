#ifndef MongooseSntpClient_h
#define MongooseSntpClient_h

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"
#include "MongooseSocket.h"

class MongooseSntpClient;

typedef std::function<void(struct timeval time)> MongooseSntpTimeHandler;

class MongooseSntpClient : public MongooseSocket
{
  private:
    MongooseSntpTimeHandler _onTime;

  protected:
    void onResolve(mg_connection *nc);
    void handleEvent(mg_connection *nc, int ev, void *p);

  public:
    MongooseSntpClient();
    ~MongooseSntpClient();

  MongooseSntpClient *onTime(MongooseSntpTimeHandler handler) {
    _onTime = handler;
    return this;
  }

  MongooseSntpClient *onError(MongooseSocketErrorHandler fnHandler) {
    MongooseSocket::onError(fnHandler);
    return this;
  }

  MongooseSntpClient *onClose(MongooseSocketCloseHandler fnHandler) {
    MongooseSocket::onClose(fnHandler);
    return this;
  }

  bool getTime(const char *server) {
    return getTime(server, _onTime);
  }
  bool getTime(const char *server, MongooseSntpTimeHandler onTime);

#ifdef ARDUINO
  bool getTime(String &server) {
    return getTime(server.c_str(), _onTime);
  }
  bool getTime(String &server, MongooseSntpTimeHandler onTime) {
    return getTime(server.c_str(), onTime);
  }
#endif
};

#endif /* MongooseSntpClient_h */
