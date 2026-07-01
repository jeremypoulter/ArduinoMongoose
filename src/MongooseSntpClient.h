#ifndef MongooseSntpClient_h
#define MongooseSntpClient_h

#include "MongoosePlatform.h"

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

  bool getTime(const char *server, MongooseSntpTimeHandler onTime);

#ifdef ARDUINO
  bool getTime(String &server, MongooseSntpTimeHandler onTime) {
    return getTime(server.c_str(), onTime);
  }
#endif
};

#endif /* MongooseSntpClient_h */
