#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>

#include "MongooseHttpServerEndpoint.h"
#include "MongooseHttp.h"

RequestHandle MongooseHttpServerEndpoint::willHandleRequest(mg_connection *nc, HttpRequestMethodComposite requestMethod, mg_http_message *msg)
{
  DBUGF("Checking if %x %.*s matches %x %.*s", requestMethod, (int)msg->uri.len, msg->uri.buf, _method, (int)_uri.length(), _uri.c_str());

  // Check if the URI matches
  if(mg_match(msg->uri, _uri, nullptr))
  {
    // Check if the method is allowed
    if(_method & requestMethod)
    {
      MongooseHttpServerRequest *request = requestFactory(nc, requestMethod, msg);
      if(request)
      {
        nc->fn_data = request;
        nc->data[MONGOOSE_SOCKET_TYPE] = request->getType();
        
        return REQUEST_WILL_HANDLE;
      }

      return REQUEST_ERROR;
    }

    return REQUEST_URL_MATCH;
  }
  
  return REQUEST_NO_MATCH;
}

void MongooseHttpServerEndpoint::handleRequest(MongooseHttpServerRequest *request)
{
  if(_request) {
    _request(request);
  }
}

void MongooseHttpServerEndpoint::handleClose(MongooseHttpServerRequest *request)
{
  if(_close) {
    _close(request);
  }
}
