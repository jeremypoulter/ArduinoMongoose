#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_SNTP_CLIENT)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>

#include "MongooseCore.h"
#include "MongooseSntpClient.h"

#if MG_ENABLE_SNTP

MongooseSntpClient::MongooseSntpClient() :
  MongooseSocket(),
  _onTime(NULL)
{

}

MongooseSntpClient::~MongooseSntpClient()
{

}


void MongooseSntpClient::onEvent(int ev, void *p)
{
  struct mg_sntp_message *msg = (struct mg_sntp_message *) p;
  switch (ev) 
  {
    case MG_SNTP_REPLY:
      if(_onTime) {
        _onTime(msg->tv);
      }
      break;

    case MG_SNTP_FAILED:
      onError(-1);
      break;
  }
}

bool MongooseSntpClient::getTime(const char *server, MongooseSntpTimeHandler onTime)
{
  if(!connected()) 
  {
    DBUGF("Trying to connect to %s", server);
    _onTime = onTime;

    if(MongooseSocket::connect(
      mg_sntp_get_time(Mongoose.getMgr(), eventHandler, server, this))) 
    {
      return true;
    }

    DBUGF("Failed to connect to %s", server);
  }

  return false;
}

#endif // MG_ENABLE_SNTP
