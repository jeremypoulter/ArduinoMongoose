# API Reference

`ArduinoMongoose` wraps Mongoose in a set of easy-to-use classes. All network activity is handled asynchronously by calling `Mongoose.poll()` in your `loop()`.

## Core

### `MongooseCore`
Singleton instance `Mongoose` manages the core networking loop and configuration.

- `void begin()`: Initialize the Mongoose event manager.
- `void poll(int timeout_ms)`: Poll the networking subsystem for events. Call this frequently in your main loop.
- `struct mg_mgr *getMgr()`: Get the underlying Mongoose `mg_mgr` struct.

## Servers

### `MongooseHttpServer`
Provides HTTP and WebSocket server functionality.

- `bool begin(uint16_t port)`: Starts the server on the specified port.
- `MongooseHttpServerEndpoint *on(const char* uri, MongooseHttpRequestHandler onRequest)`: Adds a request handler.
- `MongooseHttpServerEndpoint *on(const char* uri, HttpRequestMethodComposite method, MongooseHttpRequestHandler onRequest)`: Adds a handler for a specific HTTP method (e.g. `HTTP_GET`, `HTTP_POST`, `HTTP_ANY`).
- `void onNotFound(MongooseHttpRequestHandler onRequest)`: Adds a handler for 404 responses.

### Request & Response
The callback receives a `MongooseHttpServerRequest *request`.
- `request->send(int code, const char *contentType, const char *content)`: Send a quick response.
- `request->getParam(const char *name, char *buffer, size_t maxLen)`: Read GET/POST parameters.
- `request->beginResponse()`: Returns a `MongooseHttpServerResponseBasic` to build a complex response.
- `request->beginResponseStream()`: Returns a `MongooseHttpServerResponseStream` for streaming data (`printf`, `println`).

## Clients

### `MongooseHttpClient`
Asynchronous HTTP client.

- `void get(const char *url, MongooseHttpClientResponseHandler onResponse)`: Perform a GET request.
- `void post(const char *url, const char *contentType, const char *body, MongooseHttpClientResponseHandler onResponse)`: Perform a POST request.

### `MongooseMqttClient`
MQTT Client with TLS support.

- `bool connect(const char *server, const char *client_id, MongooseMqttConnectionHandler onConnect)`: Connects to the MQTT broker.
- `void publish(const char *topic, const char *payload)`: Publish a message.
- `void subscribe(const char *topic, MongooseMqttMessageHandler onMessage)`: Subscribe to a topic.

### `MongooseWebSocketClient`
Reliable WebSocket client with auto-reconnect and heartbeat.

- `void connect(const char *url)`: Connect to a WebSocket endpoint.
- `void onOpen(MongooseWebSocketOpenHandler handler)`: Callback when connection opens.
- `void onMessage(MongooseWebSocketMessageHandler handler)`: Callback on receiving a message.
- `void send(const char *data)`: Send text data.

## Utilities

### `MongooseSntpClient`
Fetch time via SNTP.

- `bool getTime(const char *server, MongooseSntpTimeHandler onTime)`: Request time asynchronously.

### `MongooseMdns`
mDNS discovery.

- `void begin(const char *hostname)`: Start mDNS with a given hostname.
- `void addService(const char *service, uint16_t port)`: Advertise a service (e.g., `_http._tcp`).