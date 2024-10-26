#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>
#include <algorithm>

#include "MongooseCore.h"
#include "MongooseHttpServer.h"

MongooseHttpServer::MongooseHttpServer() :
  MongooseHttpServerConnection(),
  _endpoints(),
  _notFound(HTTP_ANY, "#")
{
  _notFound.onRequest([](MongooseHttpServerRequest *request) {
    request->send(404);
  });
}

MongooseHttpServer::~MongooseHttpServer()
{

}

bool MongooseHttpServer::begin(uint16_t port)
{
  char addr[32];
  snprintf(addr, sizeof(addr), "http://0.0.0.0:%u", port);
  DBUGF("Starting HTTP server on %s", addr);

  if(mg_http_listen(Mongoose.getMgr(), addr, MongooseSocket::eventHandler, this))
  {
    return true;
  }

  return false;
}

bool MongooseHttpServer::begin(uint16_t port, const char *cert, const char *private_key)
{
  setCertificate(cert, private_key);
  if(begin(port)) {
    return true;
  }

  return false;
}

MongooseHttpServerEndpoint *MongooseHttpServer::on(const char* uri)
{
  return on(uri, HTTP_ANY);
}

MongooseHttpServerEndpoint *MongooseHttpServer::on(const char* uri, MongooseHttpRequestHandler onRequest)
{
  MongooseHttpServerEndpoint *endpoint = on(uri);
  if(endpoint)
  {
    endpoint->onRequest(onRequest);
    return endpoint;
  }

  return nullptr;
}

MongooseHttpServerEndpoint *MongooseHttpServer::on(const char* uri, HttpRequestMethodComposite method, MongooseHttpRequestHandler onRequest)
{
  MongooseHttpServerEndpoint *endpoint = on(uri, method);
  if(endpoint)
  {
    endpoint->onRequest(onRequest);
    return endpoint;
  }

  return nullptr;
}

MongooseHttpServerEndpoint *MongooseHttpServer::on(const char* uri, HttpRequestMethodComposite method)
{
  return on(new MongooseHttpServerEndpoint(method, uri));
}

MongooseHttpServerEndpointUpload *MongooseHttpServer::on(const char* uri, MongooseHttpUploadHandler onUpload)
{
  MongooseHttpServerEndpointUpload *endpoint = new MongooseHttpServerEndpointUpload(HTTP_POST, uri);
  if(endpoint)
  {
    endpoint->onUpload(onUpload);
    on(endpoint);
    return endpoint;
  }

  return nullptr;
}

MongooseHttpServerEndpointWebSocket *MongooseHttpServer::on(const char* uri, MongooseHttpWebSocketFrameHandler onFrame)
{
  MongooseHttpServerEndpointWebSocket *endpoint = new MongooseHttpServerEndpointWebSocket(HTTP_ANY, uri);
  if(endpoint)
  {
    endpoint->onFrame(onFrame);
    on(endpoint);
    return endpoint;
  }

  return nullptr;
}

MongooseHttpServerEndpoint *MongooseHttpServer::on(MongooseHttpServerEndpoint *endpoint)
{
  if(endpoint)
  {
    _endpoints.push_front(endpoint);

    return endpoint;
  }

  return nullptr;
}

void MongooseHttpServer::onNotFound(MongooseHttpRequestHandler fn)
{
  _notFound.onRequest(fn);
}

HttpRequestMethodComposite MongooseHttpServer::method(mg_str method)
{
  if(0 == mg_strcasecmp(method, mg_str_s("GET"))) {
    return HTTP_GET;
  } else if(0 == mg_strcasecmp(method, mg_str_s("POST"))) {
    return HTTP_POST;
  } else if(0 == mg_strcasecmp(method, mg_str_s("DELETE"))) {
    return HTTP_DELETE;
  } else if(0 == mg_strcasecmp(method, mg_str_s("PUT"))) {
    return HTTP_PUT;
  } else if(0 == mg_strcasecmp(method, mg_str_s("PATCH"))) {
    return HTTP_PATCH;
  } else if(0 == mg_strcasecmp(method, mg_str_s("HEAD"))) {
    return HTTP_HEAD;
  } else if(0 == mg_strcasecmp(method, mg_str_s("OPTIONS"))) {
    return HTTP_OPTIONS;
  }

  return HTTP_ANY;
}

void MongooseHttpServer::onHeaders(mg_connection *nc, mg_http_message *msg)
{
  RequestHandle handled = REQUEST_NO_MATCH;
  HttpRequestMethodComposite requestMethod = method(msg->method);

  for(auto &handler : _endpoints)
  {
    RequestHandle endpointHandled = handler->willHandleRequest(nc, requestMethod, msg);
    switch (endpointHandled)
    {
      case REQUEST_WILL_HANDLE:
        return;

      case REQUEST_URL_MATCH:
        handled = REQUEST_URL_MATCH;
        break;
      
      case REQUEST_ERROR:
        mg_http_reply(nc, 500, nullptr, "Internal Server Error");
        nc->is_draining = 1;
        return;

      default:
        break;
    }
  }

  if(handled == REQUEST_URL_MATCH)
  {
    mg_http_reply(nc, 405, nullptr, "Method Not Allowed");
    nc->is_draining = 1;
  } else if(handled == REQUEST_NO_MATCH) {
    _notFound.willHandleRequest(nc, requestMethod, msg);
  }
}
