#ifndef MongooseSntpClient_h
#define MongooseSntpClient_h

#include "MongoosePlatform.h"

#include <mongoose.h>

#include <functional>

#include "MongooseString.h"
#include "MongooseSocket.h"

 /**
  * @brief SNTP Client wrapper
  */ 
class MongooseSntpClient;

typedef std::function<void(struct timeval time)> MongooseSntpTimeHandler;

 /**
  * @brief SNTP Client wrapper
  */ 
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

  /**
   * @brief Register callback for when time is received
   * @param handler The callback function
   * @return MongooseSntpClient* This client instance for chaining
   */
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

  /**
   * @brief Request time from an SNTP server
   * @param server The SNTP server address
   * @return true if request started successfully
   */
  bool getTime(const char *server) {
    return getTime(server, _onTime);
  }
  /**
   * @brief Request time from an SNTP server with inline callback
   * @param server The SNTP server address
   * @param onTime The callback function
   * @return true if request started successfully
   */
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
