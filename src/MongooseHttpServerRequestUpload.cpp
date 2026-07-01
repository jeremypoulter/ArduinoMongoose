#if defined(ENABLE_DEBUG) && !defined(ENABLE_DEBUG_MONGOOSE_HTTP_SERVER)
#undef ENABLE_DEBUG
#endif

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>

#include "MongooseHttpServerRequestUpload.h"
#include "MongooseHttpServerEndpointUpload.h"

void MongooseHttpServerRequestUpload::handleMessage(mg_connection *nc, mg_http_message *msg)
{
  DBUGF("MongooseHttpServerRequestUpload::handleMessage");

#if MG_COPY_HTTP_MESSAGE
  _msg->body = msg->body;
#else
  _msg = msg;
#endif

  // Call onRequest handler first (auth checks, state initialisation, etc.)
  _endpoint->handleRequest(this);
  if(_responseSent) {
    return;
  }

  MongooseHttpServerEndpointUpload *uploadEndpoint =
      static_cast<MongooseHttpServerEndpointUpload *>(_endpoint);

  if(!uploadEndpoint->hasUploadHandler()) {
    return;
  }

  // Iterate over multipart parts using the Mongoose 7 API
  size_t ofs = 0;
  struct mg_http_part part;
  index = 0;

  while((ofs = mg_http_next_multipart(msg->body, ofs, &part)) > 0)
  {
    uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_BEGIN,
                               MongooseString(part.filename),
                               index, nullptr, 0);
    if(_responseSent) { break; }

    uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_DATA,
                               MongooseString(part.filename),
                               index, (uint8_t *)part.body.buf, part.body.len);
    index += part.body.len;
    if(_responseSent) { break; }

    uploadEndpoint->callUpload(this, MG_EV_HTTP_PART_END,
                               MongooseString(part.filename),
                               index, nullptr, 0);
    if(_responseSent) { break; }
  }
}
