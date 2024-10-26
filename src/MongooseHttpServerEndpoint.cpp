#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "MongooseHttpServerEndpoint.h"
#include "MongooseHttp.h"

RequestHandle MongooseHttpServerEndpoint::willHandleRequest(mg_connection *nc, HttpRequestMethodComposite requestMethod, mg_http_message *msg)
{
  // Check if the URI matches
  if(mg_match(_uri, msg->uri, nullptr))
  {
    // Check if the method is allowed
    if(_method & requestMethod)
    {
      MongooseHttpServerRequest *request = requestFactory(nc, requestMethod, msg);
      if(request)
      {
        nc->fn_data = request;
        
        return REQUEST_WILL_HANDLE;
      }

      return REQUEST_ERROR;
    }

    return REQUEST_URL_MATCH;
  }
  
  return REQUEST_NO_MATCH;
}