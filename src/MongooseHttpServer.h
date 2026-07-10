#ifndef MongooseHttpServer_h
#define MongooseHttpServer_h

#include "MongoosePlatform.h"

#include <mongoose.h>

#include <functional>
#include <list>

#include "MongooseString.h"
#include "MongooseHttpServerConnection.h"
#include "MongooseHttpServerEndpoint.h"
#include "MongooseHttpServerEndpointUpload.h"
#include "MongooseHttpServerEndpointWebSocket.h"


 /**
  * @brief HTTP and WebSocket Server
  * 
  * Manages HTTP and WebSocket endpoints, requests, and responses.
  */ 
class MongooseHttpServer : public MongooseHttpServerConnection
{

  private:
    std::list<MongooseHttpServerEndpoint *> _endpoints;
    MongooseHttpServerEndpoint _notFound;

    HttpRequestMethodComposite method(mg_str method);
  protected:
    void handleHeaders(mg_connection *nc, mg_http_message *msg);

  public:
    /**
     * @brief Construct a new HTTP Server object
     */
    MongooseHttpServer();

    /**
     * @brief Destroy the HTTP Server object and clean up endpoints
     */
    ~MongooseHttpServer();

    /**
     * @brief Start the HTTP server on the specified port
     * 
     * @param port TCP port to listen on (e.g., 80)
     * @return true if successful
     * @return false if the server failed to start
     */
    bool begin(uint16_t port);

    /**
     * @brief Start an HTTPS server on the specified port
     * 
     * @param port TCP port to listen on (e.g., 443)
     * @param cert Path or string containing the server certificate (PEM)
     * @param private_key Path or string containing the private key (PEM)
     * @return true if successful
     * @return false if the server failed to start
     */
    bool begin(uint16_t port, const char *cert, const char *private_key);

    /**
     * @brief Register a generic endpoint upload handler for a URI
     * 
     * @param uri The URI to match
     * @return MongooseHttpServerEndpointUpload* The endpoint handler
     */
    MongooseHttpServerEndpointUpload *on(const char* uri);

    /**
     * @brief Register an endpoint with a specific HTTP method
     * 
     * @param uri The URI to match
     * @param method The HTTP method (e.g. HTTP_GET, HTTP_POST)
     * @return MongooseHttpServerEndpointUpload* The endpoint handler
     */
    MongooseHttpServerEndpointUpload *on(const char* uri, HttpRequestMethodComposite method);

    /**
     * @brief Register a request handler for a URI (all methods)
     * 
     * @param uri The URI to match
     * @param onRequest The callback for handling requests
     * @return MongooseHttpServerEndpointUpload* The endpoint handler
     */
    MongooseHttpServerEndpointUpload *on(const char* uri, MongooseHttpRequestHandler onRequest);

    /**
     * @brief Register a request handler for a specific method and URI
     * 
     * @param uri The URI to match
     * @param method The HTTP method to match
     * @param onRequest The callback for handling requests
     * @return MongooseHttpServerEndpointUpload* The endpoint handler
     */
    MongooseHttpServerEndpointUpload *on(const char* uri, HttpRequestMethodComposite method, MongooseHttpRequestHandler onRequest);

    /**
     * @brief Register an upload handler for a URI
     * 
     * @param uri The URI to match
     * @param onUpload The callback for handling chunked uploads
     * @return MongooseHttpServerEndpointUpload* The endpoint handler
     */
    MongooseHttpServerEndpointUpload *on(const char* uri, MongooseHttpUploadHandler onUpload);

    /**
     * @brief Register a WebSocket frame handler for a URI
     * 
     * @param uri The URI to match
     * @param onFrame The callback for handling WebSocket frames
     * @return MongooseHttpServerEndpointWebSocket* The WebSocket endpoint handler
     */
    MongooseHttpServerEndpointWebSocket *on(const char* uri, MongooseHttpWebSocketFrameHandler onFrame);

    /**
     * @brief Register a custom endpoint object
     * 
     * @param endpoint Pointer to the MongooseHttpServerEndpoint
     * @return MongooseHttpServerEndpoint* The registered endpoint
     */
    MongooseHttpServerEndpoint *on(MongooseHttpServerEndpoint *endpoint);

    /**
     * @brief Register a handler for 404 Not Found responses
     * 
     * @param fn The request handler callback
     */
    void onNotFound(MongooseHttpRequestHandler fn);

    /**
     * @brief Reset the server, clearing all endpoints
     */
    void reset();

    /**
     * @brief Send a WebSocket message to all connected clients on a specific endpoint
     * 
     * @param from The sender connection (optional, can be NULL to send to all)
     * @param endpoint The endpoint URI to target (optional, NULL for all endpoints)
     * @param op WebSocket opcode (e.g., WEBSOCKET_OP_TEXT)
     * @param data Pointer to data payload
     * @param len Length of the data payload
     */
    void sendAll(MongooseHttpWebSocketConnection *from, const char *endpoint, int op, const void *data, size_t len);

    /** @brief Send WebSocket data to all clients */
    void sendAll(MongooseHttpWebSocketConnection *from, int op, const void *data, size_t len) {
      sendAll(from, NULL, op, data, len);
    }
    /** @brief Send WebSocket data to all clients */
    void sendAll(int op, const void *data, size_t len) {
      sendAll(NULL, NULL, op, data, len);
    }
    /** @brief Send a text WebSocket message to all clients */
    void sendAll(MongooseHttpWebSocketConnection *from, const char *buf) {
      sendAll(from, NULL, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
    /** @brief Send a text WebSocket message to all clients */
    void sendAll(const char *buf) {
      sendAll(NULL, NULL, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
    /** @brief Send WebSocket data to all clients on a specific endpoint */
    void sendAll(const char *endpoint, int op, const void *data, size_t len) {
      sendAll(NULL, endpoint, op, data, len);
    }
    /** @brief Send a text WebSocket message to clients on a specific endpoint */
    void sendAll(MongooseHttpWebSocketConnection *from, const char *endpoint, const char *buf) {
      sendAll(from, endpoint, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
    /** @brief Send a text WebSocket message to clients on a specific endpoint */
    void sendAll(const char *endpoint, const char *buf) {
      sendAll(NULL, endpoint, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
#ifdef ARDUINO
    /** @brief Send an Arduino String as a WebSocket message to all clients */
    void sendAll(MongooseHttpWebSocketConnection *from, String &str) {
      sendAll(from, str.c_str());
    }
    /** @brief Send an Arduino String as a WebSocket message to all clients */
    void sendAll(String &str) {
      sendAll(str.c_str());
    }
    /** @brief Send an Arduino String as a WebSocket message to clients on a specific endpoint */
    void sendAll(MongooseHttpWebSocketConnection *from, const char *endpoint, String &str) {
      sendAll(from, endpoint, str.c_str());
    }
    /** @brief Send an Arduino String as a WebSocket message to clients on a specific endpoint */
    void sendAll(const char *endpoint, String &str) {
      sendAll(endpoint, str.c_str());
    }
#endif
};

#endif /* _MongooseHttpServer_H_ */
