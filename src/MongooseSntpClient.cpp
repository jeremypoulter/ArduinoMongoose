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

void MongooseSntpClient::onResolve(mg_connection *nc)
{
  DBUGF("Got address, sending request");
  mg_sntp_request(nc);
}

void MongooseSntpClient::onEvent(mg_connection *nc, int ev, void *p)
{
  switch (ev) 
  {
    case MG_EV_SNTP_TIME:
    {
      if(_onTime) 
      {
        uint64_t time_ms = *((uint64_t *)p);
        struct timeval time;
        time.tv_sec = time_ms / 1000;
        time.tv_usec = (time_ms % 1000) * 1000;
        _onTime(time);
      }
      break;
    }
  }
}

bool MongooseSntpClient::getTime(const char *server, MongooseSntpTimeHandler onTime)
{
  if(!connected()) 
  {
    DBUGF("Trying to connect to %s", server);
    _onTime = onTime;

    char url[128];
    snprintf(url, sizeof(url), "udp://%s:123", server);

    if(MongooseSocket::connect(
      mg_sntp_connect(Mongoose.getMgr(), url, eventHandler , this))) 
    {
      return true;
    }

    DBUGF("Failed to connect to %s", server);
  }

  return false;
}

#endif // MG_ENABLE_SNTP
