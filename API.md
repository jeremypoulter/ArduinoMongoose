# API Reference

| Name | Description |
|------|-------------|
| [`MongooseCore`](#mongoosecore) | Core Mongoose manager for Arduino. |
| [`MongooseMdns`](#mongoosemdns) | Arduino-style mDNS wrapper using Mongoose's built-in mDNS API. |
| [`MongooseSocket`](#mongoosesocket) | Base class for Mongoose network sockets. |
| [`MongooseString`](#mongoosestring) | A string wrapper class interoperable with mg_str and Arduino String. |
| [`MongooseHttpClient`](#mongoosehttpclient) | HTTP Client wrapper. |
| [`MongooseHttpServer`](#mongoosehttpserver) | HTTP and WebSocket Server. |
| [`MongooseMqttClient`](#mongoosemqttclient) | MQTT Client wrapper. |
| [`MongooseSntpClient`](#mongoosesntpclient) | SNTP Client wrapper. |
| [`MongooseHttpMessage`](#mongoosehttpmessage) |  |
| [`MongooseWebSocketClient`](#mongoosewebsocketclient) | WebSocket client with fluent lifecycle handlers and automatic reconnect. |
| [`MongooseHttpClientRequest`](#mongoosehttpclientrequest) | Represents an outbound HTTP request. |
| [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) | Represents an incoming HTTP request. |
| [`MongooseHttpClientResponse`](#mongoosehttpclientresponse) | Represents an inbound HTTP response. |
| [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |  |
| [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) | Base class for HTTP server responses. |
| [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |  |
| [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload-1) |  |
| [`MongooseHttpServerResponseBasic`](#mongoosehttpserverresponsebasic) | A basic HTTP server response containing a static payload. |
| [`MongooseHttpWebSocketConnection`](#mongoosehttpwebsocketconnection) |  |
| [`MongooseHttpServerEndpointUpload`](#mongoosehttpserverendpointupload) |  |
| [`MongooseHttpServerEndpointWebSocket`](#mongoosehttpserverendpointwebsocket) |  |
| [`MongooseMdnsRequest`](#mongoosemdnsrequest) | Represents an incoming mDNS query, passed to the onRequest handler. |
| [`ServiceRecord`](#servicerecord) |  |

## Enumerations

{#httprequestmethod}

#### HttpRequestMethod

```cpp
enum HttpRequestMethod
```

| Value | Description |
|-------|-------------|
| `HTTP_GET` |  |
| `HTTP_POST` |  |
| `HTTP_DELETE` |  |
| `HTTP_PUT` |  |
| `HTTP_PATCH` |  |
| `HTTP_HEAD` |  |
| `HTTP_OPTIONS` |  |
| `HTTP_ANY` |  |

{#mongoosemqttprotocol}

#### MongooseMqttProtocol

```cpp
enum MongooseMqttProtocol
```

| Value | Description |
|-------|-------------|
| `MQTT_MQTT` |  |
| `MQTT_MQTTS` |  |
| `MQTT_WS` |  |
| `MQTT_WSS` |  |

{#requesthandle}

#### RequestHandle

```cpp
enum RequestHandle
```

| Value | Description |
|-------|-------------|
| `REQUEST_NO_MATCH` |  |
| `REQUEST_URL_MATCH` |  |
| `REQUEST_WILL_HANDLE` |  |
| `REQUEST_ERROR` |  |

## Typedefs

{#arduinomongoosegetrootcacallback}

#### ArduinoMongooseGetRootCaCallback

```cpp
using ArduinoMongooseGetRootCaCallback = std::function< const char *(void)>
```

{#httprequestmethodcomposite}

#### HttpRequestMethodComposite

```cpp
using HttpRequestMethodComposite = uint8_t
```

{#mongoosemdnsrequesthandler}

#### MongooseMdnsRequestHandler

```cpp
using MongooseMdnsRequestHandler = std::function< void(MongooseMdnsRequest *req)>
```

{#mongoosesocketerrorhandler}

#### MongooseSocketErrorHandler

```cpp
using MongooseSocketErrorHandler = std::function< void(const char *error)>
```

{#mongoosesocketclosehandler}

#### MongooseSocketCloseHandler

```cpp
using MongooseSocketCloseHandler = std::function< void()>
```

{#mongoosehttpresponsehandler}

#### MongooseHttpResponseHandler

```cpp
using MongooseHttpResponseHandler = std::function< void(MongooseHttpClientResponse *request)>
```

{#mongoosemqttconnectionhandler}

#### MongooseMqttConnectionHandler

```cpp
using MongooseMqttConnectionHandler = std::function< void()>
```

{#mongoosemqttmessagehandler}

#### MongooseMqttMessageHandler

```cpp
using MongooseMqttMessageHandler = std::function< void(const MongooseString topic, const MongooseString payload)>
```

{#mongoosemqttdisconnecthandler}

#### MongooseMqttDisconnectHandler

```cpp
using MongooseMqttDisconnectHandler = std::function< void()>
```

{#mongoosesntptimehandler}

#### MongooseSntpTimeHandler

```cpp
using MongooseSntpTimeHandler = std::function< void(struct timeval time)>
```

{#mongoosewebsocketopenhandler}

#### MongooseWebSocketOpenHandler

```cpp
using MongooseWebSocketOpenHandler = std::function< void(MongooseWebSocketClient *client)>
```

{#mongoosewebsocketmessagehandler}

#### MongooseWebSocketMessageHandler

```cpp
using MongooseWebSocketMessageHandler = std::function< void(int flags, const uint8_t *data, size_t len)>
```

{#mongoosewebsocketclosehandler}

#### MongooseWebSocketCloseHandler

```cpp
using MongooseWebSocketCloseHandler = std::function< void(int code, const char *reason)>
```

{#mongoosehttprequesthandler}

#### MongooseHttpRequestHandler

```cpp
using MongooseHttpRequestHandler = std::function< void(MongooseHttpServerRequest *request)>
```

{#mongoosehttpuploadhandler}

#### MongooseHttpUploadHandler

```cpp
using MongooseHttpUploadHandler = std::function< size_t(MongooseHttpServerRequest *request, int ev, MongooseString filename, uint64_t index, uint8_t *data, size_t len)>
```

{#mongoosehttpwebsocketconnectionhandler}

#### MongooseHttpWebSocketConnectionHandler

```cpp
using MongooseHttpWebSocketConnectionHandler = std::function< void(MongooseHttpWebSocketConnection *connection)>
```

{#mongoosehttpwebsocketframehandler}

#### MongooseHttpWebSocketFrameHandler

```cpp
using MongooseHttpWebSocketFrameHandler = std::function< void(MongooseHttpWebSocketConnection *connection, int flags, uint8_t *data, size_t len)>
```

## Functions

{#get_millis}

#### get_millis

```cpp
static inline unsigned long get_millis()
```

{#mg_mk_str_from_offsets}

#### mg_mk_str_from_offsets

```cpp
mg_str mg_mk_str_from_offsets(mg_str & dest, mg_str & src, mg_str & value)
```

{#mg_http_status_code_str}

#### mg_http_status_code_str

```cpp
static const char * mg_http_status_code_str(int status_code)
```

## Variables

{#mongoose}

#### Mongoose

```cpp
MongooseCore Mongoose
```

{#mdns}

#### Mdns

```cpp
MongooseMdns Mdns
```

{#mongoose-1}

#### Mongoose

```cpp
MongooseCore Mongoose
```

{#mdns-1}

#### Mdns

```cpp
MongooseMdns Mdns
```

{#mongoosecore}

## MongooseCore

```cpp
#include <MongooseCore.h>
```

```cpp
class MongooseCore
```

Defined in src/MongooseCore.h:21

Core Mongoose manager for Arduino.

Provides an initialization point and event loop wrapper for Mongoose.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseCore`](#mongoosecore-1) | `function` | Declared here |
| [`begin`](#begin) | `function` | Declared here |
| [`end`](#end) | `function` | Declared here |
| [`poll`](#poll) | `function` | Declared here |
| [`getMgr`](#getmgr) | `function` | Declared here |
| [`ipConfigChanged`](#ipconfigchanged) | `function` | Declared here |
| [`getRootCa`](#getrootca) | `function` | Declared here |
| [`setRootCa`](#setrootca) | `function` | Declared here |
| [`setRootCaCallback`](#setrootcacallback) | `function` | Declared here |
| [`_rootCa`](#_rootca) | `variable` | Declared here |
| [`_rootCaCallback`](#_rootcacallback) | `variable` | Declared here |
| [`mgr`](#mgr) | `variable` | Declared here |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseCore`](#mongoosecore-1)  | Construct a new Mongoose Core object. |
| `void` | [`begin`](#begin)  | Initialize the Mongoose event manager. |
| `void` | [`end`](#end)  | Stop the Mongoose event manager and clean up. |
| `void` | [`poll`](#poll)  | Poll the networking subsystem for events. |
| `struct mg_mgr *` | [`getMgr`](#getmgr)  | Get the underlying Mongoose mg_mgr struct. |
| `void` | [`ipConfigChanged`](#ipconfigchanged)  | Re-evaluates IP configuration when network interfaces change. |
| `mg_str` | [`getRootCa`](#getrootca) `inline` | Get the currently configured Root CA certificate. |
| `void` | [`setRootCa`](#setrootca) `inline` | Set the Root CA certificate directly. |
| `void` | [`setRootCaCallback`](#setrootcacallback) `inline` | Set a callback function to retrieve the Root CA. |

---

{#mongoosecore-1}

#### MongooseCore

```cpp
MongooseCore()
```

Defined in src/MongooseCore.h:35

Construct a new Mongoose Core object.

---

{#begin}

#### begin

```cpp
void begin()
```

Defined in src/MongooseCore.h:40

Initialize the Mongoose event manager.

---

{#end}

#### end

```cpp
void end()
```

Defined in src/MongooseCore.h:45

Stop the Mongoose event manager and clean up.

---

{#poll}

#### poll

```cpp
void poll(int timeout_ms)
```

Defined in src/MongooseCore.h:52

Poll the networking subsystem for events.

#### Parameters
* `timeout_ms` Maximum time to block waiting for events, in milliseconds

---

{#getmgr}

#### getMgr

```cpp
struct mg_mgr * getMgr()
```

Defined in src/MongooseCore.h:59

Get the underlying Mongoose mg_mgr struct.

#### Returns
struct mg_mgr* Pointer to the Mongoose manager

---

{#ipconfigchanged}

#### ipConfigChanged

```cpp
void ipConfigChanged()
```

Defined in src/MongooseCore.h:64

Re-evaluates IP configuration when network interfaces change.

---

{#getrootca}

#### getRootCa

`inline`

```cpp
inline mg_str getRootCa()
```

Defined in src/MongooseCore.h:71

Get the currently configured Root CA certificate.

#### Returns
mg_str The Root CA as a Mongoose string

---

{#setrootca}

#### setRootCa

`inline`

```cpp
inline void setRootCa(const char * rootCa)
```

Defined in src/MongooseCore.h:80

Set the Root CA certificate directly.

#### Parameters
* `rootCa` Null-terminated string containing the Root CA PEM

---

{#setrootcacallback}

#### setRootCaCallback

`inline`

```cpp
inline void setRootCaCallback(ArduinoMongooseGetRootCaCallback callback)
```

Defined in src/MongooseCore.h:89

Set a callback function to retrieve the Root CA.

#### Parameters
* `callback` Function returning the Root CA string

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const char *` | [`_rootCa`](#_rootca)  |  |
| `ArduinoMongooseGetRootCaCallback` | [`_rootCaCallback`](#_rootcacallback)  |  |
| `struct mg_mgr` | [`mgr`](#mgr)  |  |

---

{#_rootca}

#### _rootCa

```cpp
const char * _rootCa
```

Defined in src/MongooseCore.h:24

---

{#_rootcacallback}

#### _rootCaCallback

```cpp
ArduinoMongooseGetRootCaCallback _rootCaCallback
```

Defined in src/MongooseCore.h:25

---

{#mgr}

#### mgr

```cpp
struct mg_mgr mgr
```

Defined in src/MongooseCore.h:29

{#mongoosemdns}

## MongooseMdns

```cpp
#include <MongooseMdns.h>
```

```cpp
class MongooseMdns
```

Defined in src/MongooseMdns.h:60

Arduino-style mDNS wrapper using Mongoose's built-in mDNS API.

Allows a device to advertise its hostname and services on the local network using multicast DNS (RFC-6762 / DNS-SD, RFC-6763).

Usage: [MongooseMdns](#mongoosemdns) mdns; mdns.begin("mydevice"); // Advertises mydevice.local mdns.addService("_http._tcp", 80); // Advertises an HTTP service

// In loop(): Mongoose.poll(0);

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseMdns`](#mongoosemdns-1) | `function` | Declared here |
| [`begin`](#begin-1) | `function` | Declared here |
| [`end`](#end-1) | `function` | Declared here |
| [`addService`](#addservice) | `function` | Declared here |
| [`addService`](#addservice-1) | `function` | Declared here |
| [`removeService`](#removeservice) | `function` | Declared here |
| [`removeService`](#removeservice-1) | `function` | Declared here |
| [`query`](#query) | `function` | Declared here |
| [`onRequest`](#onrequest) | `function` | Declared here |
| [`isActive`](#isactive) | `function` | Declared here |
| [`hostname`](#hostname) | `function` | Declared here |
| [`numServices`](#numservices) | `function` | Declared here |
| [`getService`](#getservice) | `function` | Declared here |
| [`MAX_SERVICES`](#max_services) | `variable` | Declared here |
| [`_mdns`](#_mdns) | `variable` | Declared here |
| [`_hostname`](#_hostname) | `variable` | Declared here |
| [`_services`](#_services) | `variable` | Declared here |
| [`_numServices`](#_numservices) | `variable` | Declared here |
| [`_onRequest`](#_onrequest) | `variable` | Declared here |
| [`handleReq`](#handlereq) | `function` | Declared here |
| [`eventHandler`](#eventhandler) | `function` | Declared here |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseMdns`](#mongoosemdns-1)  |  |
| `bool` | [`begin`](#begin-1)  | Start the mDNS listener and advertise the given hostname. |
| `void` | [`end`](#end-1)  | Stop the mDNS listener and free resources. |
| `bool` | [`addService`](#addservice)  | Register a DNS-SD service to advertise. |
| `bool` | [`addService`](#addservice-1)  | Convenience overload accepting separate protocol and transport. |
| `bool` | [`removeService`](#removeservice)  | Remove a previously registered DNS-SD service. |
| `bool` | [`removeService`](#removeservice-1)  | Convenience overload accepting separate protocol and transport. |
| `bool` | [`query`](#query)  | Send an mDNS query for the given hostname. |
| `void` | [`onRequest`](#onrequest) `inline` | Register a handler for incoming mDNS requests. |
| `bool` | [`isActive`](#isactive) `const` `inline` | Check if the mDNS listener is active. |
| `const char *` | [`hostname`](#hostname) `const` `inline` | Get the current hostname (without .local suffix). Returns nullptr if [begin()](#begin-1) has not been called. |
| `int` | [`numServices`](#numservices) `const` `inline` | Get the number of currently registered services. |
| `const ServiceRecord &` | [`getService`](#getservice) `const` `inline` | Get a registered service record by index. |

---

{#mongoosemdns-1}

#### MongooseMdns

```cpp
MongooseMdns()
```

Defined in src/MongooseMdns.h:86

---

{#begin-1}

#### begin

```cpp
bool begin(const char * hostname)
```

Defined in src/MongooseMdns.h:98

Start the mDNS listener and advertise the given hostname.

Advertises <hostname>.local on the network. Must be called after Mongoose.begin() and after network connectivity is established.

#### Parameters
* `hostname` Hostname without the .local suffix (e.g. "mydevice") 

#### Returns
true if the listener was started successfully

---

{#end-1}

#### end

```cpp
void end()
```

Defined in src/MongooseMdns.h:109

Stop the mDNS listener and free resources.

---

{#addservice}

#### addService

```cpp
bool addService(const char * srvcproto, uint16_t port, const char * txt = "")
```

Defined in src/MongooseMdns.h:122

Register a DNS-SD service to advertise.

Example: addService("_http._tcp", 80); Example: addService("_mqtt._tcp", 1883, "version=3");

#### Parameters
* `srvcproto` Service type and protocol label, e.g. "_http._tcp" 

* `port` TCP/UDP port 

* `txt` Optional TXT record content (verbatim, max 255 bytes) 

#### Returns
true if the service was registered (false if MAX_SERVICES reached)

---

{#addservice-1}

#### addService

```cpp
bool addService(const char * protocol, const char * transport, uint16_t port, const char * txt = "")
```

Defined in src/MongooseMdns.h:135

Convenience overload accepting separate protocol and transport.

Example: addService("_http", "_tcp", 80);

#### Parameters
* `protocol` Service label, e.g. "_http" 

* `transport` Transport label, e.g. "_tcp" or "_udp" 

* `port` TCP/UDP port 

* `txt` Optional TXT record content 

#### Returns
true if the service was registered

---

{#removeservice}

#### removeService

```cpp
bool removeService(const char * srvcproto)
```

Defined in src/MongooseMdns.h:153

Remove a previously registered DNS-SD service.

#### Parameters
* `srvcproto` Service type and protocol label, e.g. "_http._tcp" 

#### Returns
true if the service was found and removed, false if not found

---

{#removeservice-1}

#### removeService

```cpp
bool removeService(const char * protocol, const char * transport)
```

Defined in src/MongooseMdns.h:162

Convenience overload accepting separate protocol and transport.

#### Parameters
* `protocol` Service label, e.g. "_http" 

* `transport` Transport label, e.g. "_tcp" or "_udp" 

#### Returns
true if the service was found and removed

---

{#query}

#### query

```cpp
bool query(const char * name, unsigned int rtype = MG_DNS_RTYPE_A)
```

Defined in src/MongooseMdns.h:184

Send an mDNS query for the given hostname.

The mDNS listener must already be started with [begin()](#begin-1). Responses are delivered via MG_EV_MDNS_RESP to Mongoose resolver.

#### Parameters
* `name` Hostname to query (without .local) 

* `rtype` DNS record type (default: MG_DNS_RTYPE_A for IPv4) 

#### Returns
true if the query was sent successfully

---

{#onrequest}

#### onRequest

`inline`

```cpp
inline void onRequest(MongooseMdnsRequestHandler handler)
```

Defined in src/MongooseMdns.h:201

Register a handler for incoming mDNS requests.

The handler is called for every incoming mDNS query that is not automatically handled (i.e., PTR/SRV/TXT queries not matched by a registered service). The handler may call req->respond() to reply.

#### Parameters
* `handler` Callback receiving a [MongooseMdnsRequest](#mongoosemdnsrequest) pointer

---

{#isactive}

#### isActive

`const` `inline`

```cpp
inline bool isActive() const
```

Defined in src/MongooseMdns.h:208

Check if the mDNS listener is active.

---

{#hostname}

#### hostname

`const` `inline`

```cpp
inline const char * hostname() const
```

Defined in src/MongooseMdns.h:216

Get the current hostname (without .local suffix). Returns nullptr if [begin()](#begin-1) has not been called.

---

{#numservices}

#### numServices

`const` `inline`

```cpp
inline int numServices() const
```

Defined in src/MongooseMdns.h:223

Get the number of currently registered services.

---

{#getservice}

#### getService

`const` `inline`

```cpp
inline const ServiceRecord & getService(int index) const
```

Defined in src/MongooseMdns.h:231

Get a registered service record by index.

#### Parameters
* `index` Zero-based index; must be < [numServices()](#numservices)

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const int` | [`MAX_SERVICES`](#max_services) `static` |  |

---

{#max_services}

#### MAX_SERVICES

`static`

```cpp
const int MAX_SERVICES = 8
```

Defined in src/MongooseMdns.h:64

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `struct mg_connection *` | [`_mdns`](#_mdns)  |  |
| `char *` | [`_hostname`](#_hostname)  |  |
| `ServiceRecord` | [`_services`](#_services)  |  |
| `int` | [`_numServices`](#_numservices)  |  |
| `MongooseMdnsRequestHandler` | [`_onRequest`](#_onrequest)  |  |

---

{#_mdns}

#### _mdns

```cpp
struct mg_connection * _mdns
```

Defined in src/MongooseMdns.h:74

---

{#_hostname}

#### _hostname

```cpp
char * _hostname
```

Defined in src/MongooseMdns.h:75

---

{#_services}

#### _services

```cpp
ServiceRecord _services
```

Defined in src/MongooseMdns.h:77

---

{#_numservices}

#### _numServices

```cpp
int _numServices
```

Defined in src/MongooseMdns.h:78

---

{#_onrequest}

#### _onRequest

```cpp
MongooseMdnsRequestHandler _onRequest
```

Defined in src/MongooseMdns.h:80

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`handleReq`](#handlereq)  |  |

---

{#handlereq}

#### handleReq

```cpp
void handleReq(struct mg_connection * nc, struct mg_mdns_req * req)
```

Defined in src/MongooseMdns.h:83

### Private Static Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`eventHandler`](#eventhandler) `static` |  |

---

{#eventhandler}

#### eventHandler

`static`

```cpp
static void eventHandler(struct mg_connection * nc, int ev, void * ev_data)
```

Defined in src/MongooseMdns.h:82

{#servicerecord}

## ServiceRecord

```cpp
#include <MongooseMdns.h>
```

```cpp
struct ServiceRecord
```

Defined in src/MongooseMdns.h:67

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`srvcproto`](#srvcproto) | `variable` | Declared here |
| [`txt`](#txt) | `variable` | Declared here |
| [`port`](#port) | `variable` | Declared here |

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `char` | [`srvcproto`](#srvcproto)  |  |
| `char` | [`txt`](#txt)  |  |
| `uint16_t` | [`port`](#port)  |  |

---

{#srvcproto}

#### srvcproto

```cpp
char srvcproto
```

Defined in src/MongooseMdns.h:68

---

{#txt}

#### txt

```cpp
char txt
```

Defined in src/MongooseMdns.h:69

---

{#port}

#### port

```cpp
uint16_t port
```

Defined in src/MongooseMdns.h:70

{#mongoosesocket}

## MongooseSocket

```cpp
#include <MongooseSocket.h>
```

```cpp
class MongooseSocket
```

Defined in src/MongooseSocket.h:22

> **Subclassed by:** [`MongooseHttpClientRequest`](#mongoosehttpclientrequest), [`MongooseHttpServerConnection`](#mongoosehttpserverconnection), [`MongooseMqttClient`](#mongoosemqttclient), [`MongooseSntpClient`](#mongoosesntpclient), [`MongooseWebSocketClient`](#mongoosewebsocketclient)

Base class for Mongoose network sockets.

Provides core socket management, connection state, and event routing.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseSocket`](#mongoosesocket-1) | `function` | Declared here |
| [`MongooseSocket`](#mongoosesocket-2) | `function` | Declared here |
| [`connected`](#connected) | `function` | Declared here |
| [`onError`](#onerror) | `function` | Declared here |
| [`onClose`](#onclose) | `function` | Declared here |
| [`getConnection`](#getconnection) | `function` | Declared here |
| [`getRemoteAddress`](#getremoteaddress) | `function` | Declared here |
| [`getLocalAddress`](#getlocaladdress) | `function` | Declared here |
| [`getType`](#gettype) | `function` | Declared here |
| [`Type`](#type) | `variable` | Declared here |
| [`onOpen`](#onopen) | `function` | Declared here |
| [`onResolve`](#onresolve) | `function` | Declared here |
| [`onConnect`](#onconnect) | `function` | Declared here |
| [`onAccept`](#onaccept) | `function` | Declared here |
| [`onError`](#onerror-1) | `function` | Declared here |
| [`onReceive`](#onreceive) | `function` | Declared here |
| [`onSend`](#onsend) | `function` | Declared here |
| [`onPoll`](#onpoll) | `function` | Declared here |
| [`onClose`](#onclose-1) | `function` | Declared here |
| [`handleEvent`](#handleevent) | `function` | Declared here |
| [`connect`](#connect) | `function` | Declared here |
| [`setSecure`](#setsecure) | `function` | Declared here |
| [`setSecure`](#setsecure-1) | `function` | Declared here |
| [`setSecure`](#setsecure-2) | `function` | Declared here |
| [`setCertificate`](#setcertificate) | `function` | Declared here |
| [`setRejectUnauthorized`](#setrejectunauthorized) | `function` | Declared here |
| [`clearSecurity`](#clearsecurity) | `function` | Declared here |
| [`clearConnection`](#clearconnection) | `function` | Declared here |
| [`disconnect`](#disconnect) | `function` | Declared here |
| [`abort`](#abort) | `function` | Declared here |
| [`_nc`](#_nc) | `variable` | Declared here |
| [`_onError`](#_onerror) | `variable` | Declared here |
| [`_onClose`](#_onclose) | `variable` | Declared here |
| [`_secure`](#_secure) | `variable` | Declared here |
| [`_reject_unauthorized`](#_reject_unauthorized) | `variable` | Declared here |
| [`_host`](#_host) | `variable` | Declared here |
| [`_cert`](#_cert) | `variable` | Declared here |
| [`_key`](#_key) | `variable` | Declared here |
| [`processEvent`](#processevent) | `function` | Declared here |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseSocket`](#mongoosesocket-1)  |  |
|  | [`MongooseSocket`](#mongoosesocket-2)  |  |
| `bool` | [`connected`](#connected) `virtual` `inline` | Check if the socket is connected. |
| `MongooseSocket *` | [`onError`](#onerror) `inline` |  |
| `MongooseSocket *` | [`onClose`](#onclose) `inline` |  |
| `mg_connection *` | [`getConnection`](#getconnection) `inline` | Get the underlying Mongoose connection object. |
| `mg_addr *` | [`getRemoteAddress`](#getremoteaddress) `inline` | Get the remote network address. |
| `mg_addr *` | [`getLocalAddress`](#getlocaladdress) `inline` | Get the local network address. |
| `char` | [`getType`](#gettype) `virtual` `inline` |  |

---

{#mongoosesocket-1}

#### MongooseSocket

```cpp
MongooseSocket()
```

Defined in src/MongooseSocket.h:108

---

{#mongoosesocket-2}

#### MongooseSocket

```cpp
MongooseSocket(mg_connection * nc)
```

Defined in src/MongooseSocket.h:109

---

{#connected}

#### connected

`virtual` `inline`

```cpp
virtual inline bool connected()
```

Defined in src/MongooseSocket.h:116

Check if the socket is connected.

#### Returns
true if connected

##### Reimplemented by

- [`connected`](#connected-1)
- [`connected`](#connected-2)

---

{#onerror}

#### onError

`inline`

```cpp
inline MongooseSocket * onError(MongooseSocketErrorHandler fnHandler)
```

Defined in src/MongooseSocket.h:120

---

{#onclose}

#### onClose

`inline`

```cpp
inline MongooseSocket * onClose(MongooseSocketCloseHandler fnHandler)
```

Defined in src/MongooseSocket.h:125

---

{#getconnection}

#### getConnection

`inline`

```cpp
inline mg_connection * getConnection()
```

Defined in src/MongooseSocket.h:134

Get the underlying Mongoose connection object.

#### Returns
mg_connection*

---

{#getremoteaddress}

#### getRemoteAddress

`inline`

```cpp
inline mg_addr * getRemoteAddress()
```

Defined in src/MongooseSocket.h:142

Get the remote network address.

#### Returns
mg_addr*

---

{#getlocaladdress}

#### getLocalAddress

`inline`

```cpp
inline mg_addr * getLocalAddress()
```

Defined in src/MongooseSocket.h:150

Get the local network address.

#### Returns
mg_addr*

---

{#gettype}

#### getType

`virtual` `inline`

```cpp
virtual inline char getType()
```

Defined in src/MongooseSocket.h:155

##### Reimplemented by

- [`getType`](#gettype-1)

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const char` | [`Type`](#type) `static` |  |

---

{#type}

#### Type

`static`

```cpp
const char Type = 'S'
```

Defined in src/MongooseSocket.h:154

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onOpen`](#onopen) `virtual` |  |
| `void` | [`onResolve`](#onresolve) `virtual` |  |
| `void` | [`onConnect`](#onconnect) `virtual` |  |
| `void` | [`onAccept`](#onaccept) `virtual` |  |
| `void` | [`onError`](#onerror-1) `virtual` |  |
| `void` | [`onReceive`](#onreceive) `virtual` |  |
| `void` | [`onSend`](#onsend) `virtual` |  |
| `void` | [`onPoll`](#onpoll) `virtual` |  |
| `void` | [`onClose`](#onclose-1) `virtual` |  |
| `void` | [`handleEvent`](#handleevent) `virtual` |  |
| `bool` | [`connect`](#connect)  |  |
| `void` | [`setSecure`](#setsecure) `inline` |  |
| `void` | [`setSecure`](#setsecure-1) `inline` |  |
| `void` | [`setSecure`](#setsecure-2) `inline` |  |
| `void` | [`setCertificate`](#setcertificate) `inline` |  |
| `void` | [`setRejectUnauthorized`](#setrejectunauthorized) `inline` |  |
| `void` | [`clearSecurity`](#clearsecurity) `inline` |  |
| `void` | [`clearConnection`](#clearconnection) `inline` |  |
| `void` | [`disconnect`](#disconnect) `inline` | Gracefully disconnect the socket (drain pending data) |
| `void` | [`abort`](#abort) `inline` | Forcibly abort the socket connection immediately. |

---

{#onopen}

#### onOpen

`virtual`

```cpp
virtual void onOpen(mg_connection * nc)
```

Defined in src/MongooseSocket.h:40

##### Reimplemented by

- [`onOpen`](#onopen-2)

---

{#onresolve}

#### onResolve

`virtual`

```cpp
virtual void onResolve(mg_connection * nc)
```

Defined in src/MongooseSocket.h:41

##### Reimplemented by

- [`onResolve`](#onresolve-1)

---

{#onconnect}

#### onConnect

`virtual`

```cpp
virtual void onConnect(mg_connection * nc)
```

Defined in src/MongooseSocket.h:42

##### Reimplemented by

- [`onConnect`](#onconnect-3)
- [`onConnect`](#onconnect-2)

---

{#onaccept}

#### onAccept

`virtual`

```cpp
virtual void onAccept(mg_connection * nc)
```

Defined in src/MongooseSocket.h:43

---

{#onerror-1}

#### onError

`virtual`

```cpp
virtual void onError(mg_connection * nc, const char * error)
```

Defined in src/MongooseSocket.h:44

##### Reimplemented by

- [`onError`](#onerror-5)

---

{#onreceive}

#### onReceive

`virtual`

```cpp
virtual void onReceive(mg_connection * nc, long num_bytes)
```

Defined in src/MongooseSocket.h:45

##### Reimplemented by

- [`onReceive`](#onreceive-1)

---

{#onsend}

#### onSend

`virtual`

```cpp
virtual void onSend(mg_connection * nc, long num_bytes)
```

Defined in src/MongooseSocket.h:46

---

{#onpoll}

#### onPoll

`virtual`

```cpp
virtual void onPoll(mg_connection * nc)
```

Defined in src/MongooseSocket.h:47

##### Reimplemented by

- [`onPoll`](#onpoll-2)
- [`onPoll`](#onpoll-1)

---

{#onclose-1}

#### onClose

`virtual`

```cpp
virtual void onClose(mg_connection * nc)
```

Defined in src/MongooseSocket.h:48

##### Reimplemented by

- [`onClose`](#onclose-8)
- [`onClose`](#onclose-3)
- [`onClose`](#onclose-6)

---

{#handleevent}

#### handleEvent

`virtual`

```cpp
virtual void handleEvent(mg_connection * nc, int ev, void * p)
```

Defined in src/MongooseSocket.h:49

##### Reimplemented by

- [`handleEvent`](#handleevent-4)
- [`handleEvent`](#handleevent-5)
- [`handleEvent`](#handleevent-1)
- [`handleEvent`](#handleevent-2)
- [`handleEvent`](#handleevent-3)

---

{#connect}

#### connect

```cpp
bool connect(mg_connection * nc)
```

Defined in src/MongooseSocket.h:51

---

{#setsecure}

#### setSecure

`inline`

```cpp
inline void setSecure()
```

Defined in src/MongooseSocket.h:56

---

{#setsecure-1}

#### setSecure

`inline`

```cpp
inline void setSecure(const char * host)
```

Defined in src/MongooseSocket.h:60

---

{#setsecure-2}

#### setSecure

`inline`

```cpp
inline void setSecure(mg_str host)
```

Defined in src/MongooseSocket.h:63

---

{#setcertificate}

#### setCertificate

`inline`

```cpp
inline void setCertificate(const char * cert, const char * key)
```

Defined in src/MongooseSocket.h:68

---

{#setrejectunauthorized}

#### setRejectUnauthorized

`inline`

```cpp
inline void setRejectUnauthorized(bool reject)
```

Defined in src/MongooseSocket.h:74

---

{#clearsecurity}

#### clearSecurity

`inline`

```cpp
inline void clearSecurity()
```

Defined in src/MongooseSocket.h:78

---

{#clearconnection}

#### clearConnection

`inline`

```cpp
inline void clearConnection()
```

Defined in src/MongooseSocket.h:86

---

{#disconnect}

#### disconnect

`inline`

```cpp
inline void disconnect()
```

Defined in src/MongooseSocket.h:93

Gracefully disconnect the socket (drain pending data)

---

{#abort}

#### abort

`inline`

```cpp
inline void abort()
```

Defined in src/MongooseSocket.h:101

Forcibly abort the socket connection immediately.

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `mg_connection *` | [`_nc`](#_nc)  |  |
| `MongooseSocketErrorHandler` | [`_onError`](#_onerror)  |  |
| `MongooseSocketCloseHandler` | [`_onClose`](#_onclose)  |  |
| `bool` | [`_secure`](#_secure)  |  |
| `bool` | [`_reject_unauthorized`](#_reject_unauthorized)  |  |
| `MongooseString` | [`_host`](#_host)  |  |
| `MongooseString` | [`_cert`](#_cert)  |  |
| `MongooseString` | [`_key`](#_key)  |  |

---

{#_nc}

#### _nc

```cpp
mg_connection * _nc
```

Defined in src/MongooseSocket.h:25

---

{#_onerror}

#### _onError

```cpp
MongooseSocketErrorHandler _onError
```

Defined in src/MongooseSocket.h:27

---

{#_onclose}

#### _onClose

```cpp
MongooseSocketCloseHandler _onClose
```

Defined in src/MongooseSocket.h:28

---

{#_secure}

#### _secure

```cpp
bool _secure
```

Defined in src/MongooseSocket.h:30

---

{#_reject_unauthorized}

#### _reject_unauthorized

```cpp
bool _reject_unauthorized
```

Defined in src/MongooseSocket.h:31

---

{#_host}

#### _host

```cpp
MongooseString _host
```

Defined in src/MongooseSocket.h:32

---

{#_cert}

#### _cert

```cpp
MongooseString _cert
```

Defined in src/MongooseSocket.h:33

---

{#_key}

#### _key

```cpp
MongooseString _key
```

Defined in src/MongooseSocket.h:34

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`processEvent`](#processevent)  |  |

---

{#processevent}

#### processEvent

```cpp
void processEvent(struct mg_connection * nc, int ev, void * p)
```

Defined in src/MongooseSocket.h:36

{#mongoosestring}

## MongooseString

```cpp
#include <MongooseString.h>
```

```cpp
class MongooseString
```

Defined in src/MongooseString.h:16

A string wrapper class interoperable with mg_str and Arduino String.

Simplifies conversions and comparisons between C-strings, Arduino Strings, and Mongoose mg_str.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseString`](#mongoosestring-1) | `function` | Declared here |
| [`MongooseString`](#mongoosestring-2) | `function` | Declared here |
| [`MongooseString`](#mongoosestring-3) | `function` | Declared here |
| [`MongooseString`](#mongoosestring-4) | `function` | Declared here |
| [`MongooseString`](#mongoosestring-5) | `function` | Declared here |
| [`operator struct mg_str`](#operatorstructmg_str) | `function` | Declared here |
| [`operator struct mg_str *`](#operatorstructmg_str-1) | `function` | Declared here |
| [`operator const char *`](#operatorconstchar) | `function` | Declared here |
| [`operator StringIfHelperType`](#operatorstringifhelpertype) | `function` | Declared here |
| [`get`](#get) | `function` | Declared here |
| [`length`](#length) | `function` | Declared here |
| [`c_str`](#c_str) | `function` | Declared here |
| [`compareTo`](#compareto) | `function` | Declared here |
| [`compareTo`](#compareto-1) | `function` | Declared here |
| [`compareTo`](#compareto-2) | `function` | Declared here |
| [`equals`](#equals) | `function` | Declared here |
| [`equals`](#equals-1) | `function` | Declared here |
| [`equals`](#equals-2) | `function` | Declared here |
| [`compareToIgnoreCase`](#comparetoignorecase) | `function` | Declared here |
| [`compareToIgnoreCase`](#comparetoignorecase-1) | `function` | Declared here |
| [`compareToIgnoreCase`](#comparetoignorecase-2) | `function` | Declared here |
| [`equalsIgnoreCase`](#equalsignorecase) | `function` | Declared here |
| [`equalsIgnoreCase`](#equalsignorecase-1) | `function` | Declared here |
| [`equalsIgnoreCase`](#equalsignorecase-2) | `function` | Declared here |
| [`operator==`](#operator) | `function` | Declared here |
| [`operator==`](#operator-1) | `function` | Declared here |
| [`operator==`](#operator-2) | `function` | Declared here |
| [`operator!=`](#operator-3) | `function` | Declared here |
| [`operator!=`](#operator-4) | `function` | Declared here |
| [`operator!=`](#operator-5) | `function` | Declared here |
| [`toMgStr`](#tomgstr) | `function` | Declared here |
| [`_string`](#_string) | `variable` | Declared here |
| [`StringIfHelper`](#stringifhelper) | `function` | Declared here |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseString`](#mongoosestring-1) `inline` |  |
|  | [`MongooseString`](#mongoosestring-2) `inline` |  |
|  | [`MongooseString`](#mongoosestring-3) `inline` |  |
|  | [`MongooseString`](#mongoosestring-4) `inline` |  |
|  | [`MongooseString`](#mongoosestring-5) `inline` |  |
|  | [`operator struct mg_str`](#operatorstructmg_str) `inline` |  |
|  | [`operator struct mg_str *`](#operatorstructmg_str-1) `inline` |  |
|  | [`operator const char *`](#operatorconstchar) `const` `inline` |  |
|  | [`operator StringIfHelperType`](#operatorstringifhelpertype) `const` `inline` |  |
| `void` | [`get`](#get) `const` `inline` |  |
| `size_t` | [`length`](#length) `const` `inline` | Get the length of the string. |
| `const char *` | [`c_str`](#c_str) `const` `inline` | Get the underlying C-string pointer (may not be null-terminated) |
| `int` | [`compareTo`](#compareto) `const` `inline` |  |
| `int` | [`compareTo`](#compareto-1) `const` `inline` |  |
| `int` | [`compareTo`](#compareto-2) `const` `inline` |  |
| `unsigned char` | [`equals`](#equals) `const` `inline` |  |
| `unsigned char` | [`equals`](#equals-1) `const` `inline` |  |
| `unsigned char` | [`equals`](#equals-2) `const` `inline` |  |
| `int` | [`compareToIgnoreCase`](#comparetoignorecase) `const` `inline` |  |
| `int` | [`compareToIgnoreCase`](#comparetoignorecase-1) `const` `inline` |  |
| `int` | [`compareToIgnoreCase`](#comparetoignorecase-2) `const` `inline` |  |
| `unsigned char` | [`equalsIgnoreCase`](#equalsignorecase) `const` `inline` |  |
| `unsigned char` | [`equalsIgnoreCase`](#equalsignorecase-1) `const` `inline` |  |
| `unsigned char` | [`equalsIgnoreCase`](#equalsignorecase-2) `const` `inline` |  |
| `unsigned char` | [`operator==`](#operator) `const` `inline` |  |
| `unsigned char` | [`operator==`](#operator-1) `const` `inline` |  |
| `unsigned char` | [`operator==`](#operator-2) `const` `inline` |  |
| `unsigned char` | [`operator!=`](#operator-3) `const` `inline` |  |
| `unsigned char` | [`operator!=`](#operator-4) `const` `inline` |  |
| `unsigned char` | [`operator!=`](#operator-5) `const` `inline` |  |
| `mg_str` | [`toMgStr`](#tomgstr) `const` `inline` |  |

---

{#mongoosestring-1}

#### MongooseString

`inline`

```cpp
inline MongooseString()
```

Defined in src/MongooseString.h:29

---

{#mongoosestring-2}

#### MongooseString

`inline`

```cpp
inline MongooseString(const mg_str * string)
```

Defined in src/MongooseString.h:33

---

{#mongoosestring-3}

#### MongooseString

`inline`

```cpp
inline MongooseString(const mg_str string)
```

Defined in src/MongooseString.h:40

---

{#mongoosestring-4}

#### MongooseString

`inline`

```cpp
inline MongooseString(const char * string)
```

Defined in src/MongooseString.h:43

---

{#mongoosestring-5}

#### MongooseString

`inline`

```cpp
inline MongooseString(const char * string, size_t len)
```

Defined in src/MongooseString.h:46

---

{#operatorstructmg_str}

#### operator struct mg_str

`inline`

```cpp
inline operator struct mg_str()
```

Defined in src/MongooseString.h:58

---

{#operatorstructmg_str-1}

#### operator struct mg_str *

`inline`

```cpp
inline operator struct mg_str *()
```

Defined in src/MongooseString.h:63

---

{#operatorconstchar}

#### operator const char *

`const` `inline`

```cpp
inline operator const char *() const
```

Defined in src/MongooseString.h:69

---

{#operatorstringifhelpertype}

#### operator StringIfHelperType

`const` `inline`

```cpp
inline operator StringIfHelperType() const
```

Defined in src/MongooseString.h:77

---

{#get}

#### get

`const` `inline`

```cpp
inline void get(const char *& p, size_t & len) const
```

Defined in src/MongooseString.h:105

---

{#length}

#### length

`const` `inline`

```cpp
inline size_t length() const
```

Defined in src/MongooseString.h:114

Get the length of the string.

#### Returns
size_t Length of the string in bytes

---

{#c_str}

#### c_str

`const` `inline`

```cpp
inline const char * c_str() const
```

Defined in src/MongooseString.h:122

Get the underlying C-string pointer (may not be null-terminated)

#### Returns
const char*

---

{#compareto}

#### compareTo

`const` `inline`

```cpp
inline int compareTo(const mg_str & str) const
```

Defined in src/MongooseString.h:127

---

{#compareto-1}

#### compareTo

`const` `inline`

```cpp
inline int compareTo(const MongooseString & str) const
```

Defined in src/MongooseString.h:130

---

{#compareto-2}

#### compareTo

`const` `inline`

```cpp
inline int compareTo(const char * str) const
```

Defined in src/MongooseString.h:133

---

{#equals}

#### equals

`const` `inline`

```cpp
inline unsigned char equals(const mg_str & str) const
```

Defined in src/MongooseString.h:138

---

{#equals-1}

#### equals

`const` `inline`

```cpp
inline unsigned char equals(const MongooseString & str) const
```

Defined in src/MongooseString.h:141

---

{#equals-2}

#### equals

`const` `inline`

```cpp
inline unsigned char equals(const char * str) const
```

Defined in src/MongooseString.h:144

---

{#comparetoignorecase}

#### compareToIgnoreCase

`const` `inline`

```cpp
inline int compareToIgnoreCase(const mg_str & str) const
```

Defined in src/MongooseString.h:148

---

{#comparetoignorecase-1}

#### compareToIgnoreCase

`const` `inline`

```cpp
inline int compareToIgnoreCase(const MongooseString & str) const
```

Defined in src/MongooseString.h:152

---

{#comparetoignorecase-2}

#### compareToIgnoreCase

`const` `inline`

```cpp
inline int compareToIgnoreCase(const char * str) const
```

Defined in src/MongooseString.h:155

---

{#equalsignorecase}

#### equalsIgnoreCase

`const` `inline`

```cpp
inline unsigned char equalsIgnoreCase(const mg_str & str) const
```

Defined in src/MongooseString.h:160

---

{#equalsignorecase-1}

#### equalsIgnoreCase

`const` `inline`

```cpp
inline unsigned char equalsIgnoreCase(const MongooseString & str) const
```

Defined in src/MongooseString.h:163

---

{#equalsignorecase-2}

#### equalsIgnoreCase

`const` `inline`

```cpp
inline unsigned char equalsIgnoreCase(const char * str) const
```

Defined in src/MongooseString.h:166

---

{#operator}

#### operator==

`const` `inline`

```cpp
inline unsigned char operator==(const mg_str & str) const
```

Defined in src/MongooseString.h:170

---

{#operator-1}

#### operator==

`const` `inline`

```cpp
inline unsigned char operator==(const MongooseString & str) const
```

Defined in src/MongooseString.h:173

---

{#operator-2}

#### operator==

`const` `inline`

```cpp
inline unsigned char operator==(const char * str) const
```

Defined in src/MongooseString.h:176

---

{#operator-3}

#### operator!=

`const` `inline`

```cpp
inline unsigned char operator!=(const mg_str & str) const
```

Defined in src/MongooseString.h:180

---

{#operator-4}

#### operator!=

`const` `inline`

```cpp
inline unsigned char operator!=(const MongooseString & str) const
```

Defined in src/MongooseString.h:183

---

{#operator-5}

#### operator!=

`const` `inline`

```cpp
inline unsigned char operator!=(const char * str) const
```

Defined in src/MongooseString.h:186

---

{#tomgstr}

#### toMgStr

`const` `inline`

```cpp
inline mg_str toMgStr() const
```

Defined in src/MongooseString.h:190

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `mg_str` | [`_string`](#_string)  |  |

---

{#_string}

#### _string

```cpp
mg_str _string
```

Defined in src/MongooseString.h:19

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`StringIfHelper`](#stringifhelper) `const` `inline` |  |

---

{#stringifhelper}

#### StringIfHelper

`const` `inline`

```cpp
inline void StringIfHelper() const
```

Defined in src/MongooseString.h:25

{#mongoosehttpclient}

## MongooseHttpClient

```cpp
#include <MongooseHttpClient.h>
```

```cpp
class MongooseHttpClient
```

Defined in src/MongooseHttpClient.h:148

HTTP Client wrapper.

Provides methods to perform asynchronous HTTP requests.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpClient`](#mongoosehttpclient-1) | `function` | Declared here |
| [`beginRequest`](#beginrequest) | `function` | Declared here |
| [`get`](#get-1) | `function` | Declared here |
| [`post`](#post) | `function` | Declared here |
| [`put`](#put) | `function` | Declared here |
| [`patch`](#patch) | `function` | Declared here |
| [`delete_`](#delete_) | `function` | Declared here |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpClient`](#mongoosehttpclient-1)  |  |
| `MongooseHttpClientRequest *` | [`beginRequest`](#beginrequest)  | Begin a custom HTTP request. |
| `bool` | [`get`](#get-1)  | Perform an HTTP GET request. |
| `bool` | [`post`](#post)  | Perform an HTTP POST request. |
| `bool` | [`put`](#put)  | Perform an HTTP PUT request. |
| `bool` | [`patch`](#patch)  | Perform an HTTP PATCH request. |
| `bool` | [`delete_`](#delete_)  | Perform an HTTP DELETE request. |

---

{#mongoosehttpclient-1}

#### MongooseHttpClient

```cpp
MongooseHttpClient()
```

Defined in src/MongooseHttpClient.h:151

---

{#beginrequest}

#### beginRequest

```cpp
MongooseHttpClientRequest * beginRequest(const char * uri)
```

Defined in src/MongooseHttpClient.h:159

Begin a custom HTTP request.

#### Parameters
* `uri` The URI to request 

#### Returns
MongooseHttpClientRequest* The request object to configure and send

---

{#get-1}

#### get

```cpp
bool get(const char * uri, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr)
```

Defined in src/MongooseHttpClient.h:168

Perform an HTTP GET request.

#### Parameters
* `uri` The URI to request 

* `onResponse` Callback when response is received 

* `onClose` Callback when connection is closed 

#### Returns
true if request was sent

---

{#post}

#### post

```cpp
bool post(const char * uri, const char * contentType, const char * body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr)
```

Defined in src/MongooseHttpClient.h:178

Perform an HTTP POST request.

#### Parameters
* `uri` The URI to request 

* `contentType` The Content-Type header value 

* `body` The body payload 

* `onResponse` Callback when response is received 

* `onClose` Callback when connection is closed 

#### Returns
true if request was sent

---

{#put}

#### put

```cpp
bool put(const char * uri, const char * contentType, const char * body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr)
```

Defined in src/MongooseHttpClient.h:188

Perform an HTTP PUT request.

#### Parameters
* `uri` The URI to request 

* `contentType` The Content-Type header value 

* `body` The body payload 

* `onResponse` Callback when response is received 

* `onClose` Callback when connection is closed 

#### Returns
true if request was sent

---

{#patch}

#### patch

```cpp
bool patch(const char * uri, const char * contentType, const char * body, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr)
```

Defined in src/MongooseHttpClient.h:198

Perform an HTTP PATCH request.

#### Parameters
* `uri` The URI to request 

* `contentType` The Content-Type header value 

* `body` The body payload 

* `onResponse` Callback when response is received 

* `onClose` Callback when connection is closed 

#### Returns
true if request was sent

---

{#delete_}

#### delete_

```cpp
bool delete_(const char * uri, MongooseHttpResponseHandler onResponse = nullptr, MongooseSocketCloseHandler onClose = nullptr)
```

Defined in src/MongooseHttpClient.h:206

Perform an HTTP DELETE request.

#### Parameters
* `uri` The URI to request 

* `onResponse` Callback when response is received 

* `onClose` Callback when connection is closed 

#### Returns
true if request was sent

{#mongoosehttpserver}

## MongooseHttpServer

```cpp
#include <MongooseHttpServer.h>
```

```cpp
class MongooseHttpServer
```

Defined in src/MongooseHttpServer.h:23

> **Inherits:** [`MongooseHttpServerConnection`](#mongoosehttpserverconnection)

HTTP and WebSocket Server.

Manages HTTP and WebSocket endpoints, requests, and responses.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpServer`](#mongoosehttpserver-1) | `function` | Declared here |
| [`~MongooseHttpServer`](#mongoosehttpserver-2) | `function` | Declared here |
| [`begin`](#begin-2) | `function` | Declared here |
| [`begin`](#begin-3) | `function` | Declared here |
| [`on`](#on) | `function` | Declared here |
| [`on`](#on-1) | `function` | Declared here |
| [`on`](#on-2) | `function` | Declared here |
| [`on`](#on-3) | `function` | Declared here |
| [`on`](#on-4) | `function` | Declared here |
| [`on`](#on-5) | `function` | Declared here |
| [`on`](#on-6) | `function` | Declared here |
| [`onNotFound`](#onnotfound) | `function` | Declared here |
| [`reset`](#reset) | `function` | Declared here |
| [`sendAll`](#sendall) | `function` | Declared here |
| [`sendAll`](#sendall-1) | `function` | Declared here |
| [`sendAll`](#sendall-2) | `function` | Declared here |
| [`sendAll`](#sendall-3) | `function` | Declared here |
| [`sendAll`](#sendall-4) | `function` | Declared here |
| [`sendAll`](#sendall-5) | `function` | Declared here |
| [`sendAll`](#sendall-6) | `function` | Declared here |
| [`sendAll`](#sendall-7) | `function` | Declared here |
| [`handleHeaders`](#handleheaders) | `function` | Declared here |
| [`_endpoints`](#_endpoints) | `variable` | Declared here |
| [`_notFound`](#_notfound) | `variable` | Declared here |
| [`method`](#method) | `function` | Declared here |
| [`MongooseHttpServerConnection`](#mongoosehttpserverconnection-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleEvent`](#handleevent-5) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleHeaders`](#handleheaders-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleMessage`](#handlemessage-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketConnect`](#handlewebsocketconnect) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketMessage`](#handlewebsocketmessage) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketControl`](#handlewebsocketcontrol) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`MongooseSocket`](#mongoosesocket-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`MongooseSocket`](#mongoosesocket-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connected`](#connected) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getConnection`](#getconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getRemoteAddress`](#getremoteaddress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getLocalAddress`](#getlocaladdress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getType`](#gettype) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`Type`](#type) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onOpen`](#onopen) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onResolve`](#onresolve) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onConnect`](#onconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onAccept`](#onaccept) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onReceive`](#onreceive) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onSend`](#onsend) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onPoll`](#onpoll) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`handleEvent`](#handleevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connect`](#connect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setCertificate`](#setcertificate) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setRejectUnauthorized`](#setrejectunauthorized) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearSecurity`](#clearsecurity) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearConnection`](#clearconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`disconnect`](#disconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`abort`](#abort) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_nc`](#_nc) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onError`](#_onerror) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onClose`](#_onclose) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_secure`](#_secure) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_reject_unauthorized`](#_reject_unauthorized) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_host`](#_host) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_cert`](#_cert) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_key`](#_key) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`processEvent`](#processevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |

### Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseHttpServerConnection`](#mongoosehttpserverconnection-1) `inline` |  |
| `function` | [`handleEvent`](#handleevent-5) `virtual` |  |
| `function` | [`handleHeaders`](#handleheaders-1) `virtual` `inline` |  |
| `function` | [`handleMessage`](#handlemessage-1) `virtual` `inline` |  |
| `function` | [`handleWebSocketConnect`](#handlewebsocketconnect) `virtual` `inline` |  |
| `function` | [`handleWebSocketMessage`](#handlewebsocketmessage) `virtual` `inline` |  |
| `function` | [`handleWebSocketControl`](#handlewebsocketcontrol) `virtual` `inline` |  |

### Inherited from [`MongooseSocket`](#mongoosesocket)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseSocket`](#mongoosesocket-1)  |  |
| `function` | [`MongooseSocket`](#mongoosesocket-2)  |  |
| `function` | [`connected`](#connected) `virtual` `inline` | Check if the socket is connected. |
| `function` | [`onError`](#onerror) `inline` |  |
| `function` | [`onClose`](#onclose) `inline` |  |
| `function` | [`getConnection`](#getconnection) `inline` | Get the underlying Mongoose connection object. |
| `function` | [`getRemoteAddress`](#getremoteaddress) `inline` | Get the remote network address. |
| `function` | [`getLocalAddress`](#getlocaladdress) `inline` | Get the local network address. |
| `function` | [`getType`](#gettype) `virtual` `inline` |  |
| `variable` | [`Type`](#type) `static` |  |
| `function` | [`onOpen`](#onopen) `virtual` |  |
| `function` | [`onResolve`](#onresolve) `virtual` |  |
| `function` | [`onConnect`](#onconnect) `virtual` |  |
| `function` | [`onAccept`](#onaccept) `virtual` |  |
| `function` | [`onError`](#onerror-1) `virtual` |  |
| `function` | [`onReceive`](#onreceive) `virtual` |  |
| `function` | [`onSend`](#onsend) `virtual` |  |
| `function` | [`onPoll`](#onpoll) `virtual` |  |
| `function` | [`onClose`](#onclose-1) `virtual` |  |
| `function` | [`handleEvent`](#handleevent) `virtual` |  |
| `function` | [`connect`](#connect)  |  |
| `function` | [`setSecure`](#setsecure) `inline` |  |
| `function` | [`setSecure`](#setsecure-1) `inline` |  |
| `function` | [`setSecure`](#setsecure-2) `inline` |  |
| `function` | [`setCertificate`](#setcertificate) `inline` |  |
| `function` | [`setRejectUnauthorized`](#setrejectunauthorized) `inline` |  |
| `function` | [`clearSecurity`](#clearsecurity) `inline` |  |
| `function` | [`clearConnection`](#clearconnection) `inline` |  |
| `function` | [`disconnect`](#disconnect) `inline` | Gracefully disconnect the socket (drain pending data) |
| `function` | [`abort`](#abort) `inline` | Forcibly abort the socket connection immediately. |
| `variable` | [`_nc`](#_nc)  |  |
| `variable` | [`_onError`](#_onerror)  |  |
| `variable` | [`_onClose`](#_onclose)  |  |
| `variable` | [`_secure`](#_secure)  |  |
| `variable` | [`_reject_unauthorized`](#_reject_unauthorized)  |  |
| `variable` | [`_host`](#_host)  |  |
| `variable` | [`_cert`](#_cert)  |  |
| `variable` | [`_key`](#_key)  |  |
| `function` | [`processEvent`](#processevent)  |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpServer`](#mongoosehttpserver-1)  | Construct a new HTTP Server object. |
|  | [`~MongooseHttpServer`](#mongoosehttpserver-2)  | Destroy the HTTP Server object and clean up endpoints. |
| `bool` | [`begin`](#begin-2)  | Start the HTTP server on the specified port. |
| `bool` | [`begin`](#begin-3)  | Start an HTTPS server on the specified port. |
| `MongooseHttpServerEndpointUpload *` | [`on`](#on)  | Register a generic endpoint upload handler for a URI. |
| `MongooseHttpServerEndpointUpload *` | [`on`](#on-1)  | Register an endpoint with a specific HTTP method. |
| `MongooseHttpServerEndpointUpload *` | [`on`](#on-2)  | Register a request handler for a URI (all methods) |
| `MongooseHttpServerEndpointUpload *` | [`on`](#on-3)  | Register a request handler for a specific method and URI. |
| `MongooseHttpServerEndpointUpload *` | [`on`](#on-4)  | Register an upload handler for a URI. |
| `MongooseHttpServerEndpointWebSocket *` | [`on`](#on-5)  | Register a WebSocket frame handler for a URI. |
| `MongooseHttpServerEndpoint *` | [`on`](#on-6)  | Register a custom endpoint object. |
| `void` | [`onNotFound`](#onnotfound)  | Register a handler for 404 Not Found responses. |
| `void` | [`reset`](#reset)  | Reset the server, clearing all endpoints. |
| `void` | [`sendAll`](#sendall)  | Send a WebSocket message to all connected clients on a specific endpoint. |
| `void` | [`sendAll`](#sendall-1) `inline` | Send WebSocket data to all clients. |
| `void` | [`sendAll`](#sendall-2) `inline` | Send WebSocket data to all clients. |
| `void` | [`sendAll`](#sendall-3) `inline` | Send a text WebSocket message to all clients. |
| `void` | [`sendAll`](#sendall-4) `inline` | Send a text WebSocket message to all clients. |
| `void` | [`sendAll`](#sendall-5) `inline` | Send WebSocket data to all clients on a specific endpoint. |
| `void` | [`sendAll`](#sendall-6) `inline` | Send a text WebSocket message to clients on a specific endpoint. |
| `void` | [`sendAll`](#sendall-7) `inline` | Send a text WebSocket message to clients on a specific endpoint. |

---

{#mongoosehttpserver-1}

#### MongooseHttpServer

```cpp
MongooseHttpServer()
```

Defined in src/MongooseHttpServer.h:38

Construct a new HTTP Server object.

---

{#mongoosehttpserver-2}

#### ~MongooseHttpServer

```cpp
~MongooseHttpServer()
```

Defined in src/MongooseHttpServer.h:43

Destroy the HTTP Server object and clean up endpoints.

---

{#begin-2}

#### begin

```cpp
bool begin(uint16_t port)
```

Defined in src/MongooseHttpServer.h:52

Start the HTTP server on the specified port.

#### Parameters
* `port` TCP port to listen on (e.g., 80) 

#### Returns
true if successful 

#### Returns
false if the server failed to start

---

{#begin-3}

#### begin

```cpp
bool begin(uint16_t port, const char * cert, const char * private_key)
```

Defined in src/MongooseHttpServer.h:63

Start an HTTPS server on the specified port.

#### Parameters
* `port` TCP port to listen on (e.g., 443) 

* `cert` Path or string containing the server certificate (PEM) 

* `private_key` Path or string containing the private key (PEM) 

#### Returns
true if successful 

#### Returns
false if the server failed to start

---

{#on}

#### on

```cpp
MongooseHttpServerEndpointUpload * on(const char * uri)
```

Defined in src/MongooseHttpServer.h:71

Register a generic endpoint upload handler for a URI.

#### Parameters
* `uri` The URI to match 

#### Returns
MongooseHttpServerEndpointUpload* The endpoint handler

---

{#on-1}

#### on

```cpp
MongooseHttpServerEndpointUpload * on(const char * uri, HttpRequestMethodComposite method)
```

Defined in src/MongooseHttpServer.h:80

Register an endpoint with a specific HTTP method.

#### Parameters
* `uri` The URI to match 

* `method` The HTTP method (e.g. HTTP_GET, HTTP_POST) 

#### Returns
MongooseHttpServerEndpointUpload* The endpoint handler

---

{#on-2}

#### on

```cpp
MongooseHttpServerEndpointUpload * on(const char * uri, MongooseHttpRequestHandler onRequest)
```

Defined in src/MongooseHttpServer.h:89

Register a request handler for a URI (all methods)

#### Parameters
* `uri` The URI to match 

* `onRequest` The callback for handling requests 

#### Returns
MongooseHttpServerEndpointUpload* The endpoint handler

---

{#on-3}

#### on

```cpp
MongooseHttpServerEndpointUpload * on(const char * uri, HttpRequestMethodComposite method, MongooseHttpRequestHandler onRequest)
```

Defined in src/MongooseHttpServer.h:99

Register a request handler for a specific method and URI.

#### Parameters
* `uri` The URI to match 

* `method` The HTTP method to match 

* `onRequest` The callback for handling requests 

#### Returns
MongooseHttpServerEndpointUpload* The endpoint handler

---

{#on-4}

#### on

```cpp
MongooseHttpServerEndpointUpload * on(const char * uri, MongooseHttpUploadHandler onUpload)
```

Defined in src/MongooseHttpServer.h:108

Register an upload handler for a URI.

#### Parameters
* `uri` The URI to match 

* `onUpload` The callback for handling chunked uploads 

#### Returns
MongooseHttpServerEndpointUpload* The endpoint handler

---

{#on-5}

#### on

```cpp
MongooseHttpServerEndpointWebSocket * on(const char * uri, MongooseHttpWebSocketFrameHandler onFrame)
```

Defined in src/MongooseHttpServer.h:117

Register a WebSocket frame handler for a URI.

#### Parameters
* `uri` The URI to match 

* `onFrame` The callback for handling WebSocket frames 

#### Returns
MongooseHttpServerEndpointWebSocket* The WebSocket endpoint handler

---

{#on-6}

#### on

```cpp
MongooseHttpServerEndpoint * on(MongooseHttpServerEndpoint * endpoint)
```

Defined in src/MongooseHttpServer.h:125

Register a custom endpoint object.

#### Parameters
* `endpoint` Pointer to the [MongooseHttpServerEndpoint](#mongoosehttpserverendpoint)

#### Returns
MongooseHttpServerEndpoint* The registered endpoint

---

{#onnotfound}

#### onNotFound

```cpp
void onNotFound(MongooseHttpRequestHandler fn)
```

Defined in src/MongooseHttpServer.h:132

Register a handler for 404 Not Found responses.

#### Parameters
* `fn` The request handler callback

---

{#reset}

#### reset

```cpp
void reset()
```

Defined in src/MongooseHttpServer.h:137

Reset the server, clearing all endpoints.

---

{#sendall}

#### sendAll

```cpp
void sendAll(MongooseHttpWebSocketConnection * from, const char * endpoint, int op, const void * data, size_t len)
```

Defined in src/MongooseHttpServer.h:148

Send a WebSocket message to all connected clients on a specific endpoint.

#### Parameters
* `from` The sender connection (optional, can be NULL to send to all) 

* `endpoint` The endpoint URI to target (optional, NULL for all endpoints) 

* `op` WebSocket opcode (e.g., WEBSOCKET_OP_TEXT) 

* `data` Pointer to data payload 

* `len` Length of the data payload

---

{#sendall-1}

#### sendAll

`inline`

```cpp
inline void sendAll(MongooseHttpWebSocketConnection * from, int op, const void * data, size_t len)
```

Defined in src/MongooseHttpServer.h:151

Send WebSocket data to all clients.

---

{#sendall-2}

#### sendAll

`inline`

```cpp
inline void sendAll(int op, const void * data, size_t len)
```

Defined in src/MongooseHttpServer.h:155

Send WebSocket data to all clients.

---

{#sendall-3}

#### sendAll

`inline`

```cpp
inline void sendAll(MongooseHttpWebSocketConnection * from, const char * buf)
```

Defined in src/MongooseHttpServer.h:159

Send a text WebSocket message to all clients.

---

{#sendall-4}

#### sendAll

`inline`

```cpp
inline void sendAll(const char * buf)
```

Defined in src/MongooseHttpServer.h:163

Send a text WebSocket message to all clients.

---

{#sendall-5}

#### sendAll

`inline`

```cpp
inline void sendAll(const char * endpoint, int op, const void * data, size_t len)
```

Defined in src/MongooseHttpServer.h:167

Send WebSocket data to all clients on a specific endpoint.

---

{#sendall-6}

#### sendAll

`inline`

```cpp
inline void sendAll(MongooseHttpWebSocketConnection * from, const char * endpoint, const char * buf)
```

Defined in src/MongooseHttpServer.h:171

Send a text WebSocket message to clients on a specific endpoint.

---

{#sendall-7}

#### sendAll

`inline`

```cpp
inline void sendAll(const char * endpoint, const char * buf)
```

Defined in src/MongooseHttpServer.h:175

Send a text WebSocket message to clients on a specific endpoint.

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`handleHeaders`](#handleheaders) `virtual` |  |

---

{#handleheaders}

#### handleHeaders

`virtual`

```cpp
virtual void handleHeaders(mg_connection * nc, mg_http_message * msg)
```

Defined in src/MongooseHttpServer.h:32

##### Reimplements

- [`handleHeaders`](#handleheaders-1)

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `std::list< MongooseHttpServerEndpoint * >` | [`_endpoints`](#_endpoints)  |  |
| `MongooseHttpServerEndpoint` | [`_notFound`](#_notfound)  |  |

---

{#_endpoints}

#### _endpoints

```cpp
std::list< MongooseHttpServerEndpoint * > _endpoints
```

Defined in src/MongooseHttpServer.h:27

---

{#_notfound}

#### _notFound

```cpp
MongooseHttpServerEndpoint _notFound
```

Defined in src/MongooseHttpServer.h:28

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `HttpRequestMethodComposite` | [`method`](#method)  |  |

---

{#method}

#### method

```cpp
HttpRequestMethodComposite method(mg_str method)
```

Defined in src/MongooseHttpServer.h:30

{#mongoosemqttclient}

## MongooseMqttClient

```cpp
#include <MongooseMqttClient.h>
```

```cpp
class MongooseMqttClient
```

Defined in src/MongooseMqttClient.h:31

> **Inherits:** [`MongooseSocket`](#mongoosesocket)

MQTT Client wrapper.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseMqttClient`](#mongoosemqttclient-1) | `function` | Declared here |
| [`connect`](#connect-1) | `function` | Declared here |
| [`connect`](#connect-2) | `function` | Declared here |
| [`connect`](#connect-3) | `function` | Declared here |
| [`connect`](#connect-4) | `function` | Declared here |
| [`setCredentials`](#setcredentials) | `function` | Declared here |
| [`setCertificate`](#setcertificate-1) | `function` | Declared here |
| [`setRejectUnauthorized`](#setrejectunauthorized-1) | `function` | Declared here |
| [`setLastWillAndTestimment`](#setlastwillandtestimment) | `function` | Declared here |
| [`setLastWillAndTestament`](#setlastwillandtestament) | `function` | Declared here |
| [`disconnect`](#disconnect-1) | `function` | Declared here |
| [`connected`](#connected-1) | `function` | Declared here |
| [`onConnect`](#onconnect-1) | `function` | Declared here |
| [`onMessage`](#onmessage) | `function` | Declared here |
| [`onError`](#onerror-2) | `function` | Declared here |
| [`onClose`](#onclose-2) | `function` | Declared here |
| [`onDisconnect`](#ondisconnect) | `function` | Declared here |
| [`subscribe`](#subscribe) | `function` | Declared here |
| [`publish`](#publish) | `function` | Declared here |
| [`publish`](#publish-1) | `function` | Declared here |
| [`publish`](#publish-2) | `function` | Declared here |
| [`onClose`](#onclose-3) | `function` | Declared here |
| [`handleEvent`](#handleevent-1) | `function` | Declared here |
| [`_client_id`](#_client_id) | `variable` | Declared here |
| [`_username`](#_username) | `variable` | Declared here |
| [`_password`](#_password) | `variable` | Declared here |
| [`_will_topic`](#_will_topic) | `variable` | Declared here |
| [`_will_message`](#_will_message) | `variable` | Declared here |
| [`_will_retain`](#_will_retain) | `variable` | Declared here |
| [`_connected`](#_connected) | `variable` | Declared here |
| [`_onConnect`](#_onconnect) | `variable` | Declared here |
| [`_onMessage`](#_onmessage) | `variable` | Declared here |
| [`_onDisconnect`](#_ondisconnect) | `variable` | Declared here |
| [`MongooseSocket`](#mongoosesocket-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`MongooseSocket`](#mongoosesocket-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connected`](#connected) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getConnection`](#getconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getRemoteAddress`](#getremoteaddress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getLocalAddress`](#getlocaladdress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getType`](#gettype) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`Type`](#type) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onOpen`](#onopen) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onResolve`](#onresolve) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onConnect`](#onconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onAccept`](#onaccept) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onReceive`](#onreceive) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onSend`](#onsend) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onPoll`](#onpoll) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`handleEvent`](#handleevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connect`](#connect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setCertificate`](#setcertificate) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setRejectUnauthorized`](#setrejectunauthorized) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearSecurity`](#clearsecurity) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearConnection`](#clearconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`disconnect`](#disconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`abort`](#abort) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_nc`](#_nc) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onError`](#_onerror) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onClose`](#_onclose) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_secure`](#_secure) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_reject_unauthorized`](#_reject_unauthorized) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_host`](#_host) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_cert`](#_cert) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_key`](#_key) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`processEvent`](#processevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |

### Inherited from [`MongooseSocket`](#mongoosesocket)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseSocket`](#mongoosesocket-1)  |  |
| `function` | [`MongooseSocket`](#mongoosesocket-2)  |  |
| `function` | [`connected`](#connected) `virtual` `inline` | Check if the socket is connected. |
| `function` | [`onError`](#onerror) `inline` |  |
| `function` | [`onClose`](#onclose) `inline` |  |
| `function` | [`getConnection`](#getconnection) `inline` | Get the underlying Mongoose connection object. |
| `function` | [`getRemoteAddress`](#getremoteaddress) `inline` | Get the remote network address. |
| `function` | [`getLocalAddress`](#getlocaladdress) `inline` | Get the local network address. |
| `function` | [`getType`](#gettype) `virtual` `inline` |  |
| `variable` | [`Type`](#type) `static` |  |
| `function` | [`onOpen`](#onopen) `virtual` |  |
| `function` | [`onResolve`](#onresolve) `virtual` |  |
| `function` | [`onConnect`](#onconnect) `virtual` |  |
| `function` | [`onAccept`](#onaccept) `virtual` |  |
| `function` | [`onError`](#onerror-1) `virtual` |  |
| `function` | [`onReceive`](#onreceive) `virtual` |  |
| `function` | [`onSend`](#onsend) `virtual` |  |
| `function` | [`onPoll`](#onpoll) `virtual` |  |
| `function` | [`onClose`](#onclose-1) `virtual` |  |
| `function` | [`handleEvent`](#handleevent) `virtual` |  |
| `function` | [`connect`](#connect)  |  |
| `function` | [`setSecure`](#setsecure) `inline` |  |
| `function` | [`setSecure`](#setsecure-1) `inline` |  |
| `function` | [`setSecure`](#setsecure-2) `inline` |  |
| `function` | [`setCertificate`](#setcertificate) `inline` |  |
| `function` | [`setRejectUnauthorized`](#setrejectunauthorized) `inline` |  |
| `function` | [`clearSecurity`](#clearsecurity) `inline` |  |
| `function` | [`clearConnection`](#clearconnection) `inline` |  |
| `function` | [`disconnect`](#disconnect) `inline` | Gracefully disconnect the socket (drain pending data) |
| `function` | [`abort`](#abort) `inline` | Forcibly abort the socket connection immediately. |
| `variable` | [`_nc`](#_nc)  |  |
| `variable` | [`_onError`](#_onerror)  |  |
| `variable` | [`_onClose`](#_onclose)  |  |
| `variable` | [`_secure`](#_secure)  |  |
| `variable` | [`_reject_unauthorized`](#_reject_unauthorized)  |  |
| `variable` | [`_host`](#_host)  |  |
| `variable` | [`_cert`](#_cert)  |  |
| `variable` | [`_key`](#_key)  |  |
| `function` | [`processEvent`](#processevent)  |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseMqttClient`](#mongoosemqttclient-1)  |  |
| `bool` | [`connect`](#connect-1) `inline` | Connect to an MQTT broker. |
| `bool` | [`connect`](#connect-2) `inline` |  |
| `bool` | [`connect`](#connect-3) `inline` |  |
| `bool` | [`connect`](#connect-4)  | Connect to an MQTT broker with protocol and callback. |
| `void` | [`setCredentials`](#setcredentials) `inline` | Set the MQTT Credentials. |
| `void` | [`setCertificate`](#setcertificate-1) `inline` | Set the TLS Client Certificate. |
| `void` | [`setRejectUnauthorized`](#setrejectunauthorized-1) `inline` |  |
| `void` | [`setLastWillAndTestimment`](#setlastwillandtestimment) `inline` |  |
| `void` | [`setLastWillAndTestament`](#setlastwillandtestament) `inline` |  |
| `bool` | [`disconnect`](#disconnect-1)  | Disconnect from the broker. |
| `bool` | [`connected`](#connected-1) `virtual` `inline` | Check if connected to broker. |
| `MongooseMqttClient *` | [`onConnect`](#onconnect-1) `inline` |  |
| `MongooseMqttClient *` | [`onMessage`](#onmessage) `inline` |  |
| `MongooseMqttClient *` | [`onError`](#onerror-2) `inline` |  |
| `MongooseMqttClient *` | [`onClose`](#onclose-2) `inline` |  |
| `void` | [`onDisconnect`](#ondisconnect) `inline` |  |
| `bool` | [`subscribe`](#subscribe)  | Subscribe to an MQTT topic. |
| `bool` | [`publish`](#publish) `inline` | Publish a message to a topic. |
| `bool` | [`publish`](#publish-1) `inline` |  |
| `bool` | [`publish`](#publish-2)  | Publish an mg_str payload to a topic. |

---

{#mongoosemqttclient-1}

#### MongooseMqttClient

```cpp
MongooseMqttClient()
```

Defined in src/MongooseMqttClient.h:51

---

{#connect-1}

#### connect

`inline`

```cpp
inline bool connect(const char * server, const char * client_id)
```

Defined in src/MongooseMqttClient.h:60

Connect to an MQTT broker.

#### Parameters
* `server` The server address 

* `client_id` The client ID 

#### Returns
true if connection started

---

{#connect-2}

#### connect

`inline`

```cpp
inline bool connect(const char * server, const char * client_id, MongooseMqttConnectionHandler onConnect)
```

Defined in src/MongooseMqttClient.h:63

---

{#connect-3}

#### connect

`inline`

```cpp
inline bool connect(MongooseMqttProtocol protocol, const char * server, const char * client_id)
```

Defined in src/MongooseMqttClient.h:66

---

{#connect-4}

#### connect

```cpp
bool connect(MongooseMqttProtocol protocol, const char * server, const char * client_id, MongooseMqttConnectionHandler onConnect)
```

Defined in src/MongooseMqttClient.h:77

Connect to an MQTT broker with protocol and callback.

#### Parameters
* `protocol` MQTT, MQTTS, etc. 

* `server` The server address 

* `client_id` The client ID 

* `onConnect` Connection callback 

#### Returns
true if connection started

---

{#setcredentials}

#### setCredentials

`inline`

```cpp
inline void setCredentials(const char * username, const char * password)
```

Defined in src/MongooseMqttClient.h:84

Set the MQTT Credentials.

#### Parameters
* `username` The MQTT username 

* `password` The MQTT password

---

{#setcertificate-1}

#### setCertificate

`inline`

```cpp
inline void setCertificate(const char * cert, const char * key)
```

Defined in src/MongooseMqttClient.h:92

Set the TLS Client Certificate.

---

{#setrejectunauthorized-1}

#### setRejectUnauthorized

`inline`

```cpp
inline void setRejectUnauthorized(bool reject)
```

Defined in src/MongooseMqttClient.h:96

---

{#setlastwillandtestimment}

#### setLastWillAndTestimment

`inline`

```cpp
inline void setLastWillAndTestimment(const char * topic, const char * message, bool retain = false)
```

Defined in src/MongooseMqttClient.h:100

---

{#setlastwillandtestament}

#### setLastWillAndTestament

`inline`

```cpp
inline void setLastWillAndTestament(const char * topic, const char * message, bool retain = false)
```

Defined in src/MongooseMqttClient.h:107

---

{#disconnect-1}

#### disconnect

```cpp
bool disconnect()
```

Defined in src/MongooseMqttClient.h:138

Disconnect from the broker.

---

{#connected-1}

#### connected

`virtual` `inline`

```cpp
virtual inline bool connected()
```

Defined in src/MongooseMqttClient.h:144

Check if connected to broker.

#### Returns
true if connected

##### Reimplements

- [`connected`](#connected)

---

{#onconnect-1}

#### onConnect

`inline`

```cpp
inline MongooseMqttClient * onConnect(MongooseMqttConnectionHandler fnHandler)
```

Defined in src/MongooseMqttClient.h:148

---

{#onmessage}

#### onMessage

`inline`

```cpp
inline MongooseMqttClient * onMessage(MongooseMqttMessageHandler fnHandler)
```

Defined in src/MongooseMqttClient.h:153

---

{#onerror-2}

#### onError

`inline`

```cpp
inline MongooseMqttClient * onError(MongooseSocketErrorHandler fnHandler)
```

Defined in src/MongooseMqttClient.h:158

---

{#onclose-2}

#### onClose

`inline`

```cpp
inline MongooseMqttClient * onClose(MongooseSocketCloseHandler fnHandler)
```

Defined in src/MongooseMqttClient.h:163

---

{#ondisconnect}

#### onDisconnect

`inline`

```cpp
inline void onDisconnect(MongooseMqttDisconnectHandler fnHandler)
```

Defined in src/MongooseMqttClient.h:168

---

{#subscribe}

#### subscribe

```cpp
bool subscribe(const char * topic, int qos = 0)
```

Defined in src/MongooseMqttClient.h:178

Subscribe to an MQTT topic.

#### Parameters
* `topic` Topic to subscribe to 

* `qos` Quality of Service (0, 1, 2) 

#### Returns
true if subscribe sent

---

{#publish}

#### publish

`inline`

```cpp
inline bool publish(const char * topic, const char * payload, bool retain = false, int qos = 0)
```

Defined in src/MongooseMqttClient.h:193

Publish a message to a topic.

#### Parameters
* `topic` Topic to publish to 

* `payload` Message payload 

* `retain` Retain flag 

* `qos` Quality of Service 

#### Returns
true if publish sent

---

{#publish-1}

#### publish

`inline`

```cpp
inline bool publish(const char * topic, MongooseString payload, bool retain = false, int qos = 0)
```

Defined in src/MongooseMqttClient.h:196

---

{#publish-2}

#### publish

```cpp
bool publish(const char * topic, mg_str payload, bool retain = false, int qos = 0)
```

Defined in src/MongooseMqttClient.h:202

Publish an mg_str payload to a topic.

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onClose`](#onclose-3) `virtual` |  |
| `void` | [`handleEvent`](#handleevent-1) `virtual` |  |

---

{#onclose-3}

#### onClose

`virtual`

```cpp
virtual void onClose(mg_connection * nc)
```

Defined in src/MongooseMqttClient.h:47

##### Reimplements

- [`onClose`](#onclose-1)

---

{#handleevent-1}

#### handleEvent

`virtual`

```cpp
virtual void handleEvent(mg_connection * nc, int ev, void * p)
```

Defined in src/MongooseMqttClient.h:48

##### Reimplements

- [`handleEvent`](#handleevent)

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `MongooseString` | [`_client_id`](#_client_id)  |  |
| `MongooseString` | [`_username`](#_username)  |  |
| `MongooseString` | [`_password`](#_password)  |  |
| `MongooseString` | [`_will_topic`](#_will_topic)  |  |
| `MongooseString` | [`_will_message`](#_will_message)  |  |
| `bool` | [`_will_retain`](#_will_retain)  |  |
| `bool` | [`_connected`](#_connected)  |  |
| `MongooseMqttConnectionHandler` | [`_onConnect`](#_onconnect)  |  |
| `MongooseMqttMessageHandler` | [`_onMessage`](#_onmessage)  |  |
| `MongooseMqttDisconnectHandler` | [`_onDisconnect`](#_ondisconnect)  |  |

---

{#_client_id}

#### _client_id

```cpp
MongooseString _client_id
```

Defined in src/MongooseMqttClient.h:34

---

{#_username}

#### _username

```cpp
MongooseString _username
```

Defined in src/MongooseMqttClient.h:35

---

{#_password}

#### _password

```cpp
MongooseString _password
```

Defined in src/MongooseMqttClient.h:36

---

{#_will_topic}

#### _will_topic

```cpp
MongooseString _will_topic
```

Defined in src/MongooseMqttClient.h:37

---

{#_will_message}

#### _will_message

```cpp
MongooseString _will_message
```

Defined in src/MongooseMqttClient.h:38

---

{#_will_retain}

#### _will_retain

```cpp
bool _will_retain
```

Defined in src/MongooseMqttClient.h:39

---

{#_connected}

#### _connected

```cpp
bool _connected
```

Defined in src/MongooseMqttClient.h:40

---

{#_onconnect}

#### _onConnect

```cpp
MongooseMqttConnectionHandler _onConnect
```

Defined in src/MongooseMqttClient.h:42

---

{#_onmessage}

#### _onMessage

```cpp
MongooseMqttMessageHandler _onMessage
```

Defined in src/MongooseMqttClient.h:43

---

{#_ondisconnect}

#### _onDisconnect

```cpp
MongooseMqttDisconnectHandler _onDisconnect
```

Defined in src/MongooseMqttClient.h:44

{#mongoosesntpclient}

## MongooseSntpClient

```cpp
#include <MongooseSntpClient.h>
```

```cpp
class MongooseSntpClient
```

Defined in src/MongooseSntpClient.h:23

> **Inherits:** [`MongooseSocket`](#mongoosesocket)

SNTP Client wrapper.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseSntpClient`](#mongoosesntpclient-1) | `function` | Declared here |
| [`onTime`](#ontime) | `function` | Declared here |
| [`onError`](#onerror-3) | `function` | Declared here |
| [`onClose`](#onclose-4) | `function` | Declared here |
| [`getTime`](#gettime) | `function` | Declared here |
| [`getTime`](#gettime-1) | `function` | Declared here |
| [`onResolve`](#onresolve-1) | `function` | Declared here |
| [`handleEvent`](#handleevent-2) | `function` | Declared here |
| [`_onTime`](#_ontime) | `variable` | Declared here |
| [`MongooseSocket`](#mongoosesocket-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`MongooseSocket`](#mongoosesocket-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connected`](#connected) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getConnection`](#getconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getRemoteAddress`](#getremoteaddress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getLocalAddress`](#getlocaladdress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getType`](#gettype) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`Type`](#type) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onOpen`](#onopen) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onResolve`](#onresolve) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onConnect`](#onconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onAccept`](#onaccept) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onReceive`](#onreceive) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onSend`](#onsend) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onPoll`](#onpoll) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`handleEvent`](#handleevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connect`](#connect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setCertificate`](#setcertificate) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setRejectUnauthorized`](#setrejectunauthorized) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearSecurity`](#clearsecurity) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearConnection`](#clearconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`disconnect`](#disconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`abort`](#abort) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_nc`](#_nc) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onError`](#_onerror) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onClose`](#_onclose) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_secure`](#_secure) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_reject_unauthorized`](#_reject_unauthorized) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_host`](#_host) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_cert`](#_cert) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_key`](#_key) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`processEvent`](#processevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |

### Inherited from [`MongooseSocket`](#mongoosesocket)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseSocket`](#mongoosesocket-1)  |  |
| `function` | [`MongooseSocket`](#mongoosesocket-2)  |  |
| `function` | [`connected`](#connected) `virtual` `inline` | Check if the socket is connected. |
| `function` | [`onError`](#onerror) `inline` |  |
| `function` | [`onClose`](#onclose) `inline` |  |
| `function` | [`getConnection`](#getconnection) `inline` | Get the underlying Mongoose connection object. |
| `function` | [`getRemoteAddress`](#getremoteaddress) `inline` | Get the remote network address. |
| `function` | [`getLocalAddress`](#getlocaladdress) `inline` | Get the local network address. |
| `function` | [`getType`](#gettype) `virtual` `inline` |  |
| `variable` | [`Type`](#type) `static` |  |
| `function` | [`onOpen`](#onopen) `virtual` |  |
| `function` | [`onResolve`](#onresolve) `virtual` |  |
| `function` | [`onConnect`](#onconnect) `virtual` |  |
| `function` | [`onAccept`](#onaccept) `virtual` |  |
| `function` | [`onError`](#onerror-1) `virtual` |  |
| `function` | [`onReceive`](#onreceive) `virtual` |  |
| `function` | [`onSend`](#onsend) `virtual` |  |
| `function` | [`onPoll`](#onpoll) `virtual` |  |
| `function` | [`onClose`](#onclose-1) `virtual` |  |
| `function` | [`handleEvent`](#handleevent) `virtual` |  |
| `function` | [`connect`](#connect)  |  |
| `function` | [`setSecure`](#setsecure) `inline` |  |
| `function` | [`setSecure`](#setsecure-1) `inline` |  |
| `function` | [`setSecure`](#setsecure-2) `inline` |  |
| `function` | [`setCertificate`](#setcertificate) `inline` |  |
| `function` | [`setRejectUnauthorized`](#setrejectunauthorized) `inline` |  |
| `function` | [`clearSecurity`](#clearsecurity) `inline` |  |
| `function` | [`clearConnection`](#clearconnection) `inline` |  |
| `function` | [`disconnect`](#disconnect) `inline` | Gracefully disconnect the socket (drain pending data) |
| `function` | [`abort`](#abort) `inline` | Forcibly abort the socket connection immediately. |
| `variable` | [`_nc`](#_nc)  |  |
| `variable` | [`_onError`](#_onerror)  |  |
| `variable` | [`_onClose`](#_onclose)  |  |
| `variable` | [`_secure`](#_secure)  |  |
| `variable` | [`_reject_unauthorized`](#_reject_unauthorized)  |  |
| `variable` | [`_host`](#_host)  |  |
| `variable` | [`_cert`](#_cert)  |  |
| `variable` | [`_key`](#_key)  |  |
| `function` | [`processEvent`](#processevent)  |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseSntpClient`](#mongoosesntpclient-1)  |  |
| `MongooseSntpClient *` | [`onTime`](#ontime) `inline` | Register callback for when time is received. |
| `MongooseSntpClient *` | [`onError`](#onerror-3) `inline` |  |
| `MongooseSntpClient *` | [`onClose`](#onclose-4) `inline` |  |
| `bool` | [`getTime`](#gettime) `inline` | Request time from an SNTP server. |
| `bool` | [`getTime`](#gettime-1)  | Request time from an SNTP server with inline callback. |

---

{#mongoosesntpclient-1}

#### MongooseSntpClient

```cpp
MongooseSntpClient()
```

Defined in src/MongooseSntpClient.h:33

---

{#ontime}

#### onTime

`inline`

```cpp
inline MongooseSntpClient * onTime(MongooseSntpTimeHandler handler)
```

Defined in src/MongooseSntpClient.h:41

Register callback for when time is received.

#### Parameters
* `handler` The callback function 

#### Returns
MongooseSntpClient* This client instance for chaining

---

{#onerror-3}

#### onError

`inline`

```cpp
inline MongooseSntpClient * onError(MongooseSocketErrorHandler fnHandler)
```

Defined in src/MongooseSntpClient.h:46

---

{#onclose-4}

#### onClose

`inline`

```cpp
inline MongooseSntpClient * onClose(MongooseSocketCloseHandler fnHandler)
```

Defined in src/MongooseSntpClient.h:51

---

{#gettime}

#### getTime

`inline`

```cpp
inline bool getTime(const char * server)
```

Defined in src/MongooseSntpClient.h:61

Request time from an SNTP server.

#### Parameters
* `server` The SNTP server address 

#### Returns
true if request started successfully

---

{#gettime-1}

#### getTime

```cpp
bool getTime(const char * server, MongooseSntpTimeHandler onTime)
```

Defined in src/MongooseSntpClient.h:70

Request time from an SNTP server with inline callback.

#### Parameters
* `server` The SNTP server address 

* `onTime` The callback function 

#### Returns
true if request started successfully

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onResolve`](#onresolve-1) `virtual` |  |
| `void` | [`handleEvent`](#handleevent-2) `virtual` |  |

---

{#onresolve-1}

#### onResolve

`virtual`

```cpp
virtual void onResolve(mg_connection * nc)
```

Defined in src/MongooseSntpClient.h:29

##### Reimplements

- [`onResolve`](#onresolve)

---

{#handleevent-2}

#### handleEvent

`virtual`

```cpp
virtual void handleEvent(mg_connection * nc, int ev, void * p)
```

Defined in src/MongooseSntpClient.h:30

##### Reimplements

- [`handleEvent`](#handleevent)

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `MongooseSntpTimeHandler` | [`_onTime`](#_ontime)  |  |

---

{#_ontime}

#### _onTime

```cpp
MongooseSntpTimeHandler _onTime
```

Defined in src/MongooseSntpClient.h:26

{#mongoosehttpmessage}

## MongooseHttpMessage

```cpp
#include <MongooseHttpMessage.h>
```

```cpp
class MongooseHttpMessage
```

Defined in src/MongooseHttpMessage.h:10

> **Subclassed by:** [`MongooseHttpClientResponse`](#mongoosehttpclientresponse), [`MongooseHttpServerRequest`](#mongoosehttpserverrequest)

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`message`](#message) | `function` | Declared here |
| [`body`](#body) | `function` | Declared here |
| [`methodStr`](#methodstr) | `function` | Declared here |
| [`uri`](#uri) | `function` | Declared here |
| [`proto`](#proto) | `function` | Declared here |
| [`respCode`](#respcode) | `function` | Declared here |
| [`respStatusMsg`](#respstatusmsg) | `function` | Declared here |
| [`queryString`](#querystring) | `function` | Declared here |
| [`headers`](#headers) | `function` | Declared here |
| [`headers`](#headers-1) | `function` | Declared here |
| [`headerNames`](#headernames) | `function` | Declared here |
| [`headerValues`](#headervalues) | `function` | Declared here |
| [`host`](#host) | `function` | Declared here |
| [`contentType`](#contenttype) | `function` | Declared here |
| [`_msg`](#_msg) | `variable` | Declared here |
| [`MongooseHttpMessage`](#mongoosehttpmessage-1) | `function` | Declared here |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `MongooseString` | [`message`](#message) `inline` |  |
| `MongooseString` | [`body`](#body) `inline` |  |
| `MongooseString` | [`methodStr`](#methodstr) `inline` |  |
| `MongooseString` | [`uri`](#uri) `inline` |  |
| `MongooseString` | [`proto`](#proto) `inline` |  |
| `int` | [`respCode`](#respcode) `inline` |  |
| `MongooseString` | [`respStatusMsg`](#respstatusmsg) `inline` |  |
| `MongooseString` | [`queryString`](#querystring) `inline` |  |
| `int` | [`headers`](#headers) `inline` |  |
| `MongooseString` | [`headers`](#headers-1) `inline` |  |
| `MongooseString` | [`headerNames`](#headernames) `inline` |  |
| `MongooseString` | [`headerValues`](#headervalues) `inline` |  |
| `MongooseString` | [`host`](#host) `inline` |  |
| `MongooseString` | [`contentType`](#contenttype) `inline` |  |

---

{#message}

#### message

`inline`

```cpp
inline MongooseString message()
```

Defined in src/MongooseHttpMessage.h:20

---

{#body}

#### body

`inline`

```cpp
inline MongooseString body()
```

Defined in src/MongooseHttpMessage.h:23

---

{#methodstr}

#### methodStr

`inline`

```cpp
inline MongooseString methodStr()
```

Defined in src/MongooseHttpMessage.h:27

---

{#uri}

#### uri

`inline`

```cpp
inline MongooseString uri()
```

Defined in src/MongooseHttpMessage.h:30

---

{#proto}

#### proto

`inline`

```cpp
inline MongooseString proto()
```

Defined in src/MongooseHttpMessage.h:33

---

{#respcode}

#### respCode

`inline`

```cpp
inline int respCode()
```

Defined in src/MongooseHttpMessage.h:37

---

{#respstatusmsg}

#### respStatusMsg

`inline`

```cpp
inline MongooseString respStatusMsg()
```

Defined in src/MongooseHttpMessage.h:40

---

{#querystring}

#### queryString

`inline`

```cpp
inline MongooseString queryString()
```

Defined in src/MongooseHttpMessage.h:44

---

{#headers}

#### headers

`inline`

```cpp
inline int headers()
```

Defined in src/MongooseHttpMessage.h:48

---

{#headers-1}

#### headers

`inline`

```cpp
inline MongooseString headers(const char * name)
```

Defined in src/MongooseHttpMessage.h:54

---

{#headernames}

#### headerNames

`inline`

```cpp
inline MongooseString headerNames(int i)
```

Defined in src/MongooseHttpMessage.h:57

---

{#headervalues}

#### headerValues

`inline`

```cpp
inline MongooseString headerValues(int i)
```

Defined in src/MongooseHttpMessage.h:60

---

{#host}

#### host

`inline`

```cpp
inline MongooseString host()
```

Defined in src/MongooseHttpMessage.h:64

---

{#contenttype}

#### contentType

`inline`

```cpp
inline MongooseString contentType()
```

Defined in src/MongooseHttpMessage.h:68

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `mg_http_message *` | [`_msg`](#_msg)  |  |

---

{#_msg}

#### _msg

```cpp
mg_http_message * _msg
```

Defined in src/MongooseHttpMessage.h:12

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpMessage`](#mongoosehttpmessage-1) `inline` |  |

---

{#mongoosehttpmessage-1}

#### MongooseHttpMessage

`inline`

```cpp
inline MongooseHttpMessage(mg_http_message * msg)
```

Defined in src/MongooseHttpMessage.h:14

{#mongoosewebsocketclient}

## MongooseWebSocketClient

```cpp
#include <MongooseWebSocketClient.h>
```

```cpp
class MongooseWebSocketClient
```

Defined in src/MongooseWebSocketClient.h:52

> **Inherits:** [`MongooseSocket`](#mongoosesocket)

WebSocket client with fluent lifecycle handlers and automatic reconnect.

Prefer registering lifecycle handlers up-front with [onError()](#onerror-5)/onClose() and [onOpen()](#onopen-1)/onMessage()/setOnClose(), then call [connect()](#connect-5). MQTT and SNTP also support this registration style via additive overloads.

Provides reliable WebSocket connectivity with:

* Automatic reconnection with exponential backoff

* Heartbeat monitoring (PING/PONG)

* Stale connection detection

* Event-driven callbacks (non-blocking)

Usage: [MongooseWebSocketClient](#mongoosewebsocketclient) client; client.connect("ws://192.168.1.100/ws"); client.setReceiveTXTcallback([](const uint8_t *data, size_t len) { // Handle incoming message });

// In [loop()](#loop): client.loop(); // Handles reconnection, ping, etc.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseWebSocketClient`](#mongoosewebsocketclient-1) | `function` | Declared here |
| [`connect`](#connect-5) | `function` | Declared here |
| [`disconnect`](#disconnect-2) | `function` | Declared here |
| [`loop`](#loop) | `function` | Declared here |
| [`sendTXT`](#sendtxt) | `function` | Declared here |
| [`send`](#send) | `function` | Declared here |
| [`setOnOpen`](#setonopen) | `function` | Declared here |
| [`onOpen`](#onopen-1) | `function` | Declared here |
| [`setReceiveTXTcallback`](#setreceivetxtcallback) | `function` | Declared here |
| [`setOnMessage`](#setonmessage) | `function` | Declared here |
| [`onMessage`](#onmessage-1) | `function` | Declared here |
| [`setOnClose`](#setonclose) | `function` | Declared here |
| [`onError`](#onerror-4) | `function` | Declared here |
| [`onClose`](#onclose-5) | `function` | Declared here |
| [`isConnectionOpen`](#isconnectionopen) | `function` | Declared here |
| [`connected`](#connected-2) | `function` | Declared here |
| [`getLastRecv`](#getlastrecv) | `function` | Declared here |
| [`getLastConnected`](#getlastconnected) | `function` | Declared here |
| [`setReconnectInterval`](#setreconnectinterval) | `function` | Declared here |
| [`setPingInterval`](#setpinginterval) | `function` | Declared here |
| [`setStaleTimeout`](#setstaletimeout) | `function` | Declared here |
| [`getReconnectInterval`](#getreconnectinterval) | `function` | Declared here |
| [`getPingInterval`](#getpinginterval) | `function` | Declared here |
| [`getStaleTimeout`](#getstaletimeout) | `function` | Declared here |
| [`onConnect`](#onconnect-2) | `function` | Declared here |
| [`onPoll`](#onpoll-1) | `function` | Declared here |
| [`onError`](#onerror-5) | `function` | Declared here |
| [`onClose`](#onclose-6) | `function` | Declared here |
| [`handleEvent`](#handleevent-3) | `function` | Declared here |
| [`_state`](#_state) | `variable` | Declared here |
| [`_url`](#_url) | `variable` | Declared here |
| [`_protocol`](#_protocol) | `variable` | Declared here |
| [`_extraHeaders`](#_extraheaders) | `variable` | Declared here |
| [`_reconnectInterval`](#_reconnectinterval) | `variable` | Declared here |
| [`_lastReconnectAttempt`](#_lastreconnectattempt) | `variable` | Declared here |
| [`_reconnectAttemptCount`](#_reconnectattemptcount) | `variable` | Declared here |
| [`_pingInterval`](#_pinginterval) | `variable` | Declared here |
| [`_lastPing`](#_lastping) | `variable` | Declared here |
| [`_staleTimeout`](#_staletimeout) | `variable` | Declared here |
| [`_lastRecv`](#_lastrecv) | `variable` | Declared here |
| [`_lastConnected`](#_lastconnected) | `variable` | Declared here |
| [`_onOpen`](#_onopen) | `variable` | Declared here |
| [`_onMessage`](#_onmessage-1) | `variable` | Declared here |
| [`_onClose`](#_onclose-1) | `variable` | Declared here |
| [`cleanupConnection`](#cleanupconnection) | `function` | Declared here |
| [`attemptReconnect`](#attemptreconnect) | `function` | Declared here |
| [`sendPing`](#sendping) | `function` | Declared here |
| [`isStale`](#isstale) | `function` | Declared here |
| [`MongooseSocket`](#mongoosesocket-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`MongooseSocket`](#mongoosesocket-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connected`](#connected) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getConnection`](#getconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getRemoteAddress`](#getremoteaddress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getLocalAddress`](#getlocaladdress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getType`](#gettype) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`Type`](#type) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onOpen`](#onopen) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onResolve`](#onresolve) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onConnect`](#onconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onAccept`](#onaccept) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onReceive`](#onreceive) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onSend`](#onsend) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onPoll`](#onpoll) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`handleEvent`](#handleevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connect`](#connect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setCertificate`](#setcertificate) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setRejectUnauthorized`](#setrejectunauthorized) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearSecurity`](#clearsecurity) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearConnection`](#clearconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`disconnect`](#disconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`abort`](#abort) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_nc`](#_nc) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onError`](#_onerror) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onClose`](#_onclose) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_secure`](#_secure) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_reject_unauthorized`](#_reject_unauthorized) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_host`](#_host) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_cert`](#_cert) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_key`](#_key) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`processEvent`](#processevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |

### Inherited from [`MongooseSocket`](#mongoosesocket)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseSocket`](#mongoosesocket-1)  |  |
| `function` | [`MongooseSocket`](#mongoosesocket-2)  |  |
| `function` | [`connected`](#connected) `virtual` `inline` | Check if the socket is connected. |
| `function` | [`onError`](#onerror) `inline` |  |
| `function` | [`onClose`](#onclose) `inline` |  |
| `function` | [`getConnection`](#getconnection) `inline` | Get the underlying Mongoose connection object. |
| `function` | [`getRemoteAddress`](#getremoteaddress) `inline` | Get the remote network address. |
| `function` | [`getLocalAddress`](#getlocaladdress) `inline` | Get the local network address. |
| `function` | [`getType`](#gettype) `virtual` `inline` |  |
| `variable` | [`Type`](#type) `static` |  |
| `function` | [`onOpen`](#onopen) `virtual` |  |
| `function` | [`onResolve`](#onresolve) `virtual` |  |
| `function` | [`onConnect`](#onconnect) `virtual` |  |
| `function` | [`onAccept`](#onaccept) `virtual` |  |
| `function` | [`onError`](#onerror-1) `virtual` |  |
| `function` | [`onReceive`](#onreceive) `virtual` |  |
| `function` | [`onSend`](#onsend) `virtual` |  |
| `function` | [`onPoll`](#onpoll) `virtual` |  |
| `function` | [`onClose`](#onclose-1) `virtual` |  |
| `function` | [`handleEvent`](#handleevent) `virtual` |  |
| `function` | [`connect`](#connect)  |  |
| `function` | [`setSecure`](#setsecure) `inline` |  |
| `function` | [`setSecure`](#setsecure-1) `inline` |  |
| `function` | [`setSecure`](#setsecure-2) `inline` |  |
| `function` | [`setCertificate`](#setcertificate) `inline` |  |
| `function` | [`setRejectUnauthorized`](#setrejectunauthorized) `inline` |  |
| `function` | [`clearSecurity`](#clearsecurity) `inline` |  |
| `function` | [`clearConnection`](#clearconnection) `inline` |  |
| `function` | [`disconnect`](#disconnect) `inline` | Gracefully disconnect the socket (drain pending data) |
| `function` | [`abort`](#abort) `inline` | Forcibly abort the socket connection immediately. |
| `variable` | [`_nc`](#_nc)  |  |
| `variable` | [`_onError`](#_onerror)  |  |
| `variable` | [`_onClose`](#_onclose)  |  |
| `variable` | [`_secure`](#_secure)  |  |
| `variable` | [`_reject_unauthorized`](#_reject_unauthorized)  |  |
| `variable` | [`_host`](#_host)  |  |
| `variable` | [`_cert`](#_cert)  |  |
| `variable` | [`_key`](#_key)  |  |
| `function` | [`processEvent`](#processevent)  |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseWebSocketClient`](#mongoosewebsocketclient-1)  |  |
| `bool` | [`connect`](#connect-5)  | Establish WebSocket connection. |
| `void` | [`disconnect`](#disconnect-2)  | Gracefully close WebSocket connection. |
| `void` | [`loop`](#loop)  | Must be called periodically from main loop or MicroTask Handles reconnection, heartbeat, stale detection. |
| `bool` | [`sendTXT`](#sendtxt)  | Send text message (WebSocket TEXT opcode) |
| `bool` | [`send`](#send)  | Send raw WebSocket frame. |
| `void` | [`setOnOpen`](#setonopen) `inline` | Register callback for WebSocket open event Called after successful handshake (HTTP 101) |
| `MongooseWebSocketClient *` | [`onOpen`](#onopen-1) `inline` |  |
| `void` | [`setReceiveTXTcallback`](#setreceivetxtcallback) `inline` | Register callback for incoming messages Called for TEXT and BINARY frames (auto-defragmented by mongoose) |
| `void` | [`setOnMessage`](#setonmessage) `inline` | Register callback for incoming messages (preferred name) Called for TEXT and BINARY frames (auto-defragmented by mongoose) |
| `MongooseWebSocketClient *` | [`onMessage`](#onmessage-1) `inline` |  |
| `void` | [`setOnClose`](#setonclose) `inline` | Register callback for connection close Called on clean close or error disconnect. |
| `MongooseWebSocketClient *` | [`onError`](#onerror-4) `inline` |  |
| `MongooseWebSocketClient *` | [`onClose`](#onclose-5) `inline` |  |
| `bool` | [`isConnectionOpen`](#isconnectionopen) `const` `inline` | Check if WebSocket is connected and ready. |
| `bool` | [`connected`](#connected-2) `virtual` `inline` `override` | Check if the socket is connected. |
| `unsigned long` | [`getLastRecv`](#getlastrecv) `const` `inline` | Get milliseconds since last received message Used for heartbeat timeout detection. |
| `unsigned long` | [`getLastConnected`](#getlastconnected) `const` `inline` | Get milliseconds when connection was established. |
| `void` | [`setReconnectInterval`](#setreconnectinterval) `inline` | Configure reconnection interval (default 5000ms) Exponential backoff applied: interval * 2^attempt (capped at 60s) |
| `void` | [`setPingInterval`](#setpinginterval) `inline` | Configure PING interval (default 15000ms, 0 = disable) |
| `void` | [`setStaleTimeout`](#setstaletimeout) `inline` | Configure stale timeout (default 30000ms, 0 = disable) Connection closed if no messages received within timeout. |
| `unsigned long` | [`getReconnectInterval`](#getreconnectinterval) `const` `inline` | Get reconnection interval. |
| `unsigned long` | [`getPingInterval`](#getpinginterval) `const` `inline` | Get PING interval. |
| `unsigned long` | [`getStaleTimeout`](#getstaletimeout) `const` `inline` | Get stale timeout. |

---

{#mongoosewebsocketclient-1}

#### MongooseWebSocketClient

```cpp
MongooseWebSocketClient()
```

Defined in src/MongooseWebSocketClient.h:101

---

{#connect-5}

#### connect

```cpp
bool connect(const char * url, const char * protocol = nullptr, const char * extraHeaders = nullptr)
```

Defined in src/MongooseWebSocketClient.h:111

Establish WebSocket connection.

#### Parameters
* `url` WebSocket URL (ws://host/path or wss://host/path) 

* `protocol` Optional Sec-WebSocket-Protocol header value 

* `extraHeaders` Optional additional HTTP headers (newline-separated) 

#### Returns
true if connection initiated, false on error

---

{#disconnect-2}

#### disconnect

```cpp
void disconnect()
```

Defined in src/MongooseWebSocketClient.h:124

Gracefully close WebSocket connection.

---

{#loop}

#### loop

```cpp
void loop()
```

Defined in src/MongooseWebSocketClient.h:130

Must be called periodically from main loop or MicroTask Handles reconnection, heartbeat, stale detection.

---

{#sendtxt}

#### sendTXT

```cpp
bool sendTXT(const char * msg, size_t length)
```

Defined in src/MongooseWebSocketClient.h:138

Send text message (WebSocket TEXT opcode)

#### Parameters
* `msg` Text message to send 

* `length` Message length 

#### Returns
true if sent successfully

---

{#send}

#### send

```cpp
bool send(int op, const void * data, size_t len)
```

Defined in src/MongooseWebSocketClient.h:152

Send raw WebSocket frame.

#### Parameters
* `op` WebSocket opcode (TEXT, BINARY, PING, PONG, CLOSE) 

* `data` Frame data 

* `len` Data length 

#### Returns
true if sent successfully

---

{#setonopen}

#### setOnOpen

`inline`

```cpp
inline void setOnOpen(MongooseWebSocketOpenHandler handler)
```

Defined in src/MongooseWebSocketClient.h:158

Register callback for WebSocket open event Called after successful handshake (HTTP 101)

---

{#onopen-1}

#### onOpen

`inline`

```cpp
inline MongooseWebSocketClient * onOpen(MongooseWebSocketOpenHandler handler)
```

Defined in src/MongooseWebSocketClient.h:162

---

{#setreceivetxtcallback}

#### setReceiveTXTcallback

`inline`

```cpp
inline void setReceiveTXTcallback(MongooseWebSocketMessageHandler handler)
```

Defined in src/MongooseWebSocketClient.h:174

Register callback for incoming messages Called for TEXT and BINARY frames (auto-defragmented by mongoose)

Note: Despite the name, this callback is invoked for both TEXT and BINARY frames. The flags parameter can be used to distinguish frame types.

---

{#setonmessage}

#### setOnMessage

`inline`

```cpp
inline void setOnMessage(MongooseWebSocketMessageHandler handler)
```

Defined in src/MongooseWebSocketClient.h:182

Register callback for incoming messages (preferred name) Called for TEXT and BINARY frames (auto-defragmented by mongoose)

---

{#onmessage-1}

#### onMessage

`inline`

```cpp
inline MongooseWebSocketClient * onMessage(MongooseWebSocketMessageHandler handler)
```

Defined in src/MongooseWebSocketClient.h:186

---

{#setonclose}

#### setOnClose

`inline`

```cpp
inline void setOnClose(MongooseWebSocketCloseHandler handler)
```

Defined in src/MongooseWebSocketClient.h:195

Register callback for connection close Called on clean close or error disconnect.

---

{#onerror-4}

#### onError

`inline`

```cpp
inline MongooseWebSocketClient * onError(MongooseSocketErrorHandler handler)
```

Defined in src/MongooseWebSocketClient.h:199

---

{#onclose-5}

#### onClose

`inline`

```cpp
inline MongooseWebSocketClient * onClose(MongooseSocketCloseHandler handler)
```

Defined in src/MongooseWebSocketClient.h:204

---

{#isconnectionopen}

#### isConnectionOpen

`const` `inline`

```cpp
inline bool isConnectionOpen() const
```

Defined in src/MongooseWebSocketClient.h:212

Check if WebSocket is connected and ready.

---

{#connected-2}

#### connected

`virtual` `inline` `override`

```cpp
virtual inline bool connected() override
```

Defined in src/MongooseWebSocketClient.h:216

Check if the socket is connected.

#### Returns
true if connected

##### Reimplements

- [`connected`](#connected)

---

{#getlastrecv}

#### getLastRecv

`const` `inline`

```cpp
inline unsigned long getLastRecv() const
```

Defined in src/MongooseWebSocketClient.h:224

Get milliseconds since last received message Used for heartbeat timeout detection.

---

{#getlastconnected}

#### getLastConnected

`const` `inline`

```cpp
inline unsigned long getLastConnected() const
```

Defined in src/MongooseWebSocketClient.h:231

Get milliseconds when connection was established.

---

{#setreconnectinterval}

#### setReconnectInterval

`inline`

```cpp
inline void setReconnectInterval(unsigned long ms)
```

Defined in src/MongooseWebSocketClient.h:239

Configure reconnection interval (default 5000ms) Exponential backoff applied: interval * 2^attempt (capped at 60s)

---

{#setpinginterval}

#### setPingInterval

`inline`

```cpp
inline void setPingInterval(unsigned long ms)
```

Defined in src/MongooseWebSocketClient.h:246

Configure PING interval (default 15000ms, 0 = disable)

---

{#setstaletimeout}

#### setStaleTimeout

`inline`

```cpp
inline void setStaleTimeout(unsigned long ms)
```

Defined in src/MongooseWebSocketClient.h:254

Configure stale timeout (default 30000ms, 0 = disable) Connection closed if no messages received within timeout.

---

{#getreconnectinterval}

#### getReconnectInterval

`const` `inline`

```cpp
inline unsigned long getReconnectInterval() const
```

Defined in src/MongooseWebSocketClient.h:261

Get reconnection interval.

---

{#getpinginterval}

#### getPingInterval

`const` `inline`

```cpp
inline unsigned long getPingInterval() const
```

Defined in src/MongooseWebSocketClient.h:268

Get PING interval.

---

{#getstaletimeout}

#### getStaleTimeout

`const` `inline`

```cpp
inline unsigned long getStaleTimeout() const
```

Defined in src/MongooseWebSocketClient.h:275

Get stale timeout.

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onConnect`](#onconnect-2) `virtual` |  |
| `void` | [`onPoll`](#onpoll-1) `virtual` |  |
| `void` | [`onError`](#onerror-5) `virtual` |  |
| `void` | [`onClose`](#onclose-6) `virtual` |  |
| `void` | [`handleEvent`](#handleevent-3) `virtual` |  |

---

{#onconnect-2}

#### onConnect

`virtual`

```cpp
virtual void onConnect(mg_connection * nc)
```

Defined in src/MongooseWebSocketClient.h:94

##### Reimplements

- [`onConnect`](#onconnect)

---

{#onpoll-1}

#### onPoll

`virtual`

```cpp
virtual void onPoll(mg_connection * nc)
```

Defined in src/MongooseWebSocketClient.h:95

##### Reimplements

- [`onPoll`](#onpoll)

---

{#onerror-5}

#### onError

`virtual`

```cpp
virtual void onError(mg_connection * nc, const char * error)
```

Defined in src/MongooseWebSocketClient.h:96

##### Reimplements

- [`onError`](#onerror-1)

---

{#onclose-6}

#### onClose

`virtual`

```cpp
virtual void onClose(mg_connection * nc)
```

Defined in src/MongooseWebSocketClient.h:97

##### Reimplements

- [`onClose`](#onclose-1)

---

{#handleevent-3}

#### handleEvent

`virtual`

```cpp
virtual void handleEvent(struct mg_connection * nc, int ev, void * ev_data)
```

Defined in src/MongooseWebSocketClient.h:98

##### Reimplements

- [`handleEvent`](#handleevent)

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `State` | [`_state`](#_state)  |  |
| `char *` | [`_url`](#_url)  |  |
| `char *` | [`_protocol`](#_protocol)  |  |
| `char *` | [`_extraHeaders`](#_extraheaders)  |  |
| `unsigned long` | [`_reconnectInterval`](#_reconnectinterval)  |  |
| `unsigned long` | [`_lastReconnectAttempt`](#_lastreconnectattempt)  |  |
| `int` | [`_reconnectAttemptCount`](#_reconnectattemptcount)  |  |
| `unsigned long` | [`_pingInterval`](#_pinginterval)  |  |
| `unsigned long` | [`_lastPing`](#_lastping)  |  |
| `unsigned long` | [`_staleTimeout`](#_staletimeout)  |  |
| `unsigned long` | [`_lastRecv`](#_lastrecv)  |  |
| `unsigned long` | [`_lastConnected`](#_lastconnected)  |  |
| `MongooseWebSocketOpenHandler` | [`_onOpen`](#_onopen)  |  |
| `MongooseWebSocketMessageHandler` | [`_onMessage`](#_onmessage-1)  |  |
| `MongooseWebSocketCloseHandler` | [`_onClose`](#_onclose-1)  |  |

---

{#_state}

#### _state

```cpp
State _state
```

Defined in src/MongooseWebSocketClient.h:63

---

{#_url}

#### _url

```cpp
char * _url
```

Defined in src/MongooseWebSocketClient.h:66

---

{#_protocol}

#### _protocol

```cpp
char * _protocol
```

Defined in src/MongooseWebSocketClient.h:67

---

{#_extraheaders}

#### _extraHeaders

```cpp
char * _extraHeaders
```

Defined in src/MongooseWebSocketClient.h:68

---

{#_reconnectinterval}

#### _reconnectInterval

```cpp
unsigned long _reconnectInterval
```

Defined in src/MongooseWebSocketClient.h:71

---

{#_lastreconnectattempt}

#### _lastReconnectAttempt

```cpp
unsigned long _lastReconnectAttempt
```

Defined in src/MongooseWebSocketClient.h:72

---

{#_reconnectattemptcount}

#### _reconnectAttemptCount

```cpp
int _reconnectAttemptCount
```

Defined in src/MongooseWebSocketClient.h:73

---

{#_pinginterval}

#### _pingInterval

```cpp
unsigned long _pingInterval
```

Defined in src/MongooseWebSocketClient.h:76

---

{#_lastping}

#### _lastPing

```cpp
unsigned long _lastPing
```

Defined in src/MongooseWebSocketClient.h:77

---

{#_staletimeout}

#### _staleTimeout

```cpp
unsigned long _staleTimeout
```

Defined in src/MongooseWebSocketClient.h:78

---

{#_lastrecv}

#### _lastRecv

```cpp
unsigned long _lastRecv
```

Defined in src/MongooseWebSocketClient.h:79

---

{#_lastconnected}

#### _lastConnected

```cpp
unsigned long _lastConnected
```

Defined in src/MongooseWebSocketClient.h:80

---

{#_onopen}

#### _onOpen

```cpp
MongooseWebSocketOpenHandler _onOpen
```

Defined in src/MongooseWebSocketClient.h:83

---

{#_onmessage-1}

#### _onMessage

```cpp
MongooseWebSocketMessageHandler _onMessage
```

Defined in src/MongooseWebSocketClient.h:84

---

{#_onclose-1}

#### _onClose

```cpp
MongooseWebSocketCloseHandler _onClose
```

Defined in src/MongooseWebSocketClient.h:85

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`cleanupConnection`](#cleanupconnection)  |  |
| `void` | [`attemptReconnect`](#attemptreconnect)  |  |
| `void` | [`sendPing`](#sendping)  |  |
| `bool` | [`isStale`](#isstale)  |  |

---

{#cleanupconnection}

#### cleanupConnection

```cpp
void cleanupConnection()
```

Defined in src/MongooseWebSocketClient.h:88

---

{#attemptreconnect}

#### attemptReconnect

```cpp
void attemptReconnect()
```

Defined in src/MongooseWebSocketClient.h:89

---

{#sendping}

#### sendPing

```cpp
void sendPing()
```

Defined in src/MongooseWebSocketClient.h:90

---

{#isstale}

#### isStale

```cpp
bool isStale()
```

Defined in src/MongooseWebSocketClient.h:91

{#mongoosehttpclientrequest}

## MongooseHttpClientRequest

```cpp
#include <MongooseHttpClient.h>
```

```cpp
class MongooseHttpClientRequest
```

Defined in src/MongooseHttpClient.h:28

> **Inherits:** [`MongooseSocket`](#mongoosesocket)

Represents an outbound HTTP request.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpClientRequest`](#mongoosehttpclientrequest-1) | `function` | Declared here |
| [`send`](#send-1) | `function` | Declared here |
| [`setMethod`](#setmethod) | `function` | Declared here |
| [`setContentType`](#setcontenttype) | `function` | Declared here |
| [`setContentLength`](#setcontentlength) | `function` | Declared here |
| [`setContent`](#setcontent) | `function` | Declared here |
| [`setContent`](#setcontent-1) | `function` | Declared here |
| [`addHeader`](#addheader) | `function` | Declared here |
| [`addHeader`](#addheader-1) | `function` | Declared here |
| [`addHeader`](#addheader-2) | `function` | Declared here |
| [`onResponse`](#onresponse) | `function` | Declared here |
| [`onBody`](#onbody) | `function` | Declared here |
| [`onClose`](#onclose-7) | `function` | Declared here |
| [`setInsecure`](#setinsecure) | `function` | Declared here |
| [`cancel`](#cancel) | `function` | Declared here |
| [`abort`](#abort-1) | `function` | Declared here |
| [`handleEvent`](#handleevent-4) | `function` | Declared here |
| [`onOpen`](#onopen-2) | `function` | Declared here |
| [`onConnect`](#onconnect-3) | `function` | Declared here |
| [`onPoll`](#onpoll-2) | `function` | Declared here |
| [`onClose`](#onclose-8) | `function` | Declared here |
| [`_onResponse`](#_onresponse) | `variable` | Declared here |
| [`_onBody`](#_onbody) | `variable` | Declared here |
| [`_uri`](#_uri) | `variable` | Declared here |
| [`_method`](#_method) | `variable` | Declared here |
| [`_contentType`](#_contenttype) | `variable` | Declared here |
| [`_contentLength`](#_contentlength) | `variable` | Declared here |
| [`_body`](#_body) | `variable` | Declared here |
| [`_extraHeaders`](#_extraheaders-1) | `variable` | Declared here |
| [`_timeout_ms`](#_timeout_ms) | `variable` | Declared here |
| [`MongooseSocket`](#mongoosesocket-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`MongooseSocket`](#mongoosesocket-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connected`](#connected) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getConnection`](#getconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getRemoteAddress`](#getremoteaddress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getLocalAddress`](#getlocaladdress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getType`](#gettype) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`Type`](#type) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onOpen`](#onopen) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onResolve`](#onresolve) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onConnect`](#onconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onAccept`](#onaccept) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onReceive`](#onreceive) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onSend`](#onsend) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onPoll`](#onpoll) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`handleEvent`](#handleevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connect`](#connect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setCertificate`](#setcertificate) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setRejectUnauthorized`](#setrejectunauthorized) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearSecurity`](#clearsecurity) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearConnection`](#clearconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`disconnect`](#disconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`abort`](#abort) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_nc`](#_nc) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onError`](#_onerror) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onClose`](#_onclose) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_secure`](#_secure) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_reject_unauthorized`](#_reject_unauthorized) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_host`](#_host) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_cert`](#_cert) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_key`](#_key) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`processEvent`](#processevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |

### Inherited from [`MongooseSocket`](#mongoosesocket)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseSocket`](#mongoosesocket-1)  |  |
| `function` | [`MongooseSocket`](#mongoosesocket-2)  |  |
| `function` | [`connected`](#connected) `virtual` `inline` | Check if the socket is connected. |
| `function` | [`onError`](#onerror) `inline` |  |
| `function` | [`onClose`](#onclose) `inline` |  |
| `function` | [`getConnection`](#getconnection) `inline` | Get the underlying Mongoose connection object. |
| `function` | [`getRemoteAddress`](#getremoteaddress) `inline` | Get the remote network address. |
| `function` | [`getLocalAddress`](#getlocaladdress) `inline` | Get the local network address. |
| `function` | [`getType`](#gettype) `virtual` `inline` |  |
| `variable` | [`Type`](#type) `static` |  |
| `function` | [`onOpen`](#onopen) `virtual` |  |
| `function` | [`onResolve`](#onresolve) `virtual` |  |
| `function` | [`onConnect`](#onconnect) `virtual` |  |
| `function` | [`onAccept`](#onaccept) `virtual` |  |
| `function` | [`onError`](#onerror-1) `virtual` |  |
| `function` | [`onReceive`](#onreceive) `virtual` |  |
| `function` | [`onSend`](#onsend) `virtual` |  |
| `function` | [`onPoll`](#onpoll) `virtual` |  |
| `function` | [`onClose`](#onclose-1) `virtual` |  |
| `function` | [`handleEvent`](#handleevent) `virtual` |  |
| `function` | [`connect`](#connect)  |  |
| `function` | [`setSecure`](#setsecure) `inline` |  |
| `function` | [`setSecure`](#setsecure-1) `inline` |  |
| `function` | [`setSecure`](#setsecure-2) `inline` |  |
| `function` | [`setCertificate`](#setcertificate) `inline` |  |
| `function` | [`setRejectUnauthorized`](#setrejectunauthorized) `inline` |  |
| `function` | [`clearSecurity`](#clearsecurity) `inline` |  |
| `function` | [`clearConnection`](#clearconnection) `inline` |  |
| `function` | [`disconnect`](#disconnect) `inline` | Gracefully disconnect the socket (drain pending data) |
| `function` | [`abort`](#abort) `inline` | Forcibly abort the socket connection immediately. |
| `variable` | [`_nc`](#_nc)  |  |
| `variable` | [`_onError`](#_onerror)  |  |
| `variable` | [`_onClose`](#_onclose)  |  |
| `variable` | [`_secure`](#_secure)  |  |
| `variable` | [`_reject_unauthorized`](#_reject_unauthorized)  |  |
| `variable` | [`_host`](#_host)  |  |
| `variable` | [`_cert`](#_cert)  |  |
| `variable` | [`_key`](#_key)  |  |
| `function` | [`processEvent`](#processevent)  |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpClientRequest`](#mongoosehttpclientrequest-1)  |  |
| `bool` | [`send`](#send-1)  |  |
| `MongooseHttpClientRequest *` | [`setMethod`](#setmethod) `inline` |  |
| `MongooseHttpClientRequest *` | [`setContentType`](#setcontenttype) `inline` |  |
| `MongooseHttpClientRequest *` | [`setContentLength`](#setcontentlength) `inline` |  |
| `MongooseHttpClientRequest *` | [`setContent`](#setcontent) `inline` |  |
| `MongooseHttpClientRequest *` | [`setContent`](#setcontent-1)  |  |
| `bool` | [`addHeader`](#addheader)  |  |
| `bool` | [`addHeader`](#addheader-1) `inline` |  |
| `bool` | [`addHeader`](#addheader-2) `inline` |  |
| `MongooseHttpClientRequest *` | [`onResponse`](#onresponse) `inline` |  |
| `MongooseHttpClientRequest *` | [`onBody`](#onbody) `inline` |  |
| `MongooseHttpClientRequest *` | [`onClose`](#onclose-7) `inline` |  |
| `MongooseHttpClientRequest *` | [`setInsecure`](#setinsecure) `inline` |  |
| `bool` | [`cancel`](#cancel) `inline` |  |
| `bool` | [`abort`](#abort-1) `inline` |  |

---

{#mongoosehttpclientrequest-1}

#### MongooseHttpClientRequest

```cpp
MongooseHttpClientRequest(const char * uri)
```

Defined in src/MongooseHttpClient.h:50

---

{#send-1}

#### send

```cpp
bool send()
```

Defined in src/MongooseHttpClient.h:53

---

{#setmethod}

#### setMethod

`inline`

```cpp
inline MongooseHttpClientRequest * setMethod(HttpRequestMethodComposite method)
```

Defined in src/MongooseHttpClient.h:55

---

{#setcontenttype}

#### setContentType

`inline`

```cpp
inline MongooseHttpClientRequest * setContentType(const char * contentType)
```

Defined in src/MongooseHttpClient.h:59

---

{#setcontentlength}

#### setContentLength

`inline`

```cpp
inline MongooseHttpClientRequest * setContentLength(int64_t contentLength)
```

Defined in src/MongooseHttpClient.h:63

---

{#setcontent}

#### setContent

`inline`

```cpp
inline MongooseHttpClientRequest * setContent(const char * content)
```

Defined in src/MongooseHttpClient.h:67

---

{#setcontent-1}

#### setContent

```cpp
MongooseHttpClientRequest * setContent(const uint8_t * content, size_t len)
```

Defined in src/MongooseHttpClient.h:71

---

{#addheader}

#### addHeader

```cpp
bool addHeader(const char * name, size_t nameLength, const char * value, size_t valueLength)
```

Defined in src/MongooseHttpClient.h:73

---

{#addheader-1}

#### addHeader

`inline`

```cpp
inline bool addHeader(const char * name, const char * value)
```

Defined in src/MongooseHttpClient.h:74

---

{#addheader-2}

#### addHeader

`inline`

```cpp
inline bool addHeader(MongooseString name, MongooseString value)
```

Defined in src/MongooseHttpClient.h:77

---

{#onresponse}

#### onResponse

`inline`

```cpp
inline MongooseHttpClientRequest * onResponse(MongooseHttpResponseHandler handler)
```

Defined in src/MongooseHttpClient.h:86

---

{#onbody}

#### onBody

`inline`

```cpp
inline MongooseHttpClientRequest * onBody(MongooseHttpResponseHandler handler)
```

Defined in src/MongooseHttpClient.h:91

---

{#onclose-7}

#### onClose

`inline`

```cpp
inline MongooseHttpClientRequest * onClose(MongooseSocketCloseHandler handler)
```

Defined in src/MongooseHttpClient.h:96

---

{#setinsecure}

#### setInsecure

`inline`

```cpp
inline MongooseHttpClientRequest * setInsecure()
```

Defined in src/MongooseHttpClient.h:101

---

{#cancel}

#### cancel

`inline`

```cpp
inline bool cancel()
```

Defined in src/MongooseHttpClient.h:107

---

{#abort-1}

#### abort

`inline`

```cpp
inline bool abort()
```

Defined in src/MongooseHttpClient.h:117

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`handleEvent`](#handleevent-4) `virtual` |  |
| `void` | [`onOpen`](#onopen-2) `virtual` |  |
| `void` | [`onConnect`](#onconnect-3) `virtual` |  |
| `void` | [`onPoll`](#onpoll-2) `virtual` |  |
| `void` | [`onClose`](#onclose-8) `virtual` |  |

---

{#handleevent-4}

#### handleEvent

`virtual`

```cpp
virtual void handleEvent(mg_connection * nc, int ev, void * p)
```

Defined in src/MongooseHttpClient.h:43

##### Reimplements

- [`handleEvent`](#handleevent)

---

{#onopen-2}

#### onOpen

`virtual`

```cpp
virtual void onOpen(mg_connection * nc)
```

Defined in src/MongooseHttpClient.h:44

##### Reimplements

- [`onOpen`](#onopen)

---

{#onconnect-3}

#### onConnect

`virtual`

```cpp
virtual void onConnect(mg_connection * nc)
```

Defined in src/MongooseHttpClient.h:45

##### Reimplements

- [`onConnect`](#onconnect)

---

{#onpoll-2}

#### onPoll

`virtual`

```cpp
virtual void onPoll(mg_connection * nc)
```

Defined in src/MongooseHttpClient.h:46

##### Reimplements

- [`onPoll`](#onpoll)

---

{#onclose-8}

#### onClose

`virtual`

```cpp
virtual void onClose(mg_connection * nc)
```

Defined in src/MongooseHttpClient.h:47

##### Reimplements

- [`onClose`](#onclose-1)

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `MongooseHttpResponseHandler` | [`_onResponse`](#_onresponse)  |  |
| `MongooseHttpResponseHandler` | [`_onBody`](#_onbody)  |  |
| `const char *` | [`_uri`](#_uri)  |  |
| `HttpRequestMethodComposite` | [`_method`](#_method)  |  |
| `const char *` | [`_contentType`](#_contenttype)  |  |
| `int64_t` | [`_contentLength`](#_contentlength)  |  |
| `const uint8_t *` | [`_body`](#_body)  |  |
| `char *` | [`_extraHeaders`](#_extraheaders-1)  |  |
| `uint64_t` | [`_timeout_ms`](#_timeout_ms)  |  |

---

{#_onresponse}

#### _onResponse

```cpp
MongooseHttpResponseHandler _onResponse
```

Defined in src/MongooseHttpClient.h:31

---

{#_onbody}

#### _onBody

```cpp
MongooseHttpResponseHandler _onBody
```

Defined in src/MongooseHttpClient.h:32

---

{#_uri}

#### _uri

```cpp
const char * _uri
```

Defined in src/MongooseHttpClient.h:34

---

{#_method}

#### _method

```cpp
HttpRequestMethodComposite _method
```

Defined in src/MongooseHttpClient.h:35

---

{#_contenttype}

#### _contentType

```cpp
const char * _contentType
```

Defined in src/MongooseHttpClient.h:36

---

{#_contentlength}

#### _contentLength

```cpp
int64_t _contentLength
```

Defined in src/MongooseHttpClient.h:37

---

{#_body}

#### _body

```cpp
const uint8_t * _body
```

Defined in src/MongooseHttpClient.h:38

---

{#_extraheaders-1}

#### _extraHeaders

```cpp
char * _extraHeaders
```

Defined in src/MongooseHttpClient.h:39

---

{#_timeout_ms}

#### _timeout_ms

```cpp
uint64_t _timeout_ms
```

Defined in src/MongooseHttpClient.h:40

{#mongoosehttpserverrequest}

## MongooseHttpServerRequest

```cpp
#include <MongooseHttpServerRequest.h>
```

```cpp
class MongooseHttpServerRequest
```

Defined in src/MongooseHttpServerRequest.h:25

> **Inherits:** [`MongooseHttpServerConnection`](#mongoosehttpserverconnection), [`MongooseHttpMessage`](#mongoosehttpmessage)
> **Subclassed by:** [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload-1), [`MongooseHttpWebSocketConnection`](#mongoosehttpwebsocketconnection)

Represents an incoming HTTP request.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-1) | `function` | Declared here |
| [`isUpload`](#isupload) | `function` | Declared here |
| [`isWebSocket`](#iswebsocket) | `function` | Declared here |
| [`method`](#method-1) | `function` | Declared here |
| [`contentLength`](#contentlength) | `function` | Declared here |
| [`redirect`](#redirect) | `function` | Declared here |
| [`beginResponse`](#beginresponse) | `function` | Declared here |
| [`send`](#send-2) | `function` | Declared here |
| [`responseSent`](#responsesent) | `function` | Declared here |
| [`send`](#send-3) | `function` | Declared here |
| [`send`](#send-4) | `function` | Declared here |
| [`hasParam`](#hasparam) | `function` | Declared here |
| [`getParam`](#getparam) | `function` | Declared here |
| [`authenticate`](#authenticate) | `function` | Declared here |
| [`requestAuthentication`](#requestauthentication) | `function` | Declared here |
| [`_method`](#_method-1) | `variable` | Declared here |
| [`_response`](#_response) | `variable` | Declared here |
| [`_endpoint`](#_endpoint) | `variable` | Declared here |
| [`_responseSent`](#_responsesent) | `variable` | Declared here |
| [`sendBody`](#sendbody) | `function` | Declared here |
| [`duplicateMessage`](#duplicatemessage) | `function` | Declared here |
| [`handlePoll`](#handlepoll) | `function` | Declared here |
| [`handleSend`](#handlesend) | `function` | Declared here |
| [`handleClose`](#handleclose) | `function` | Declared here |
| [`handleMessage`](#handlemessage) | `function` | Declared here |
| [`MongooseHttpServerConnection`](#mongoosehttpserverconnection-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleEvent`](#handleevent-5) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleHeaders`](#handleheaders-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleMessage`](#handlemessage-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketConnect`](#handlewebsocketconnect) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketMessage`](#handlewebsocketmessage) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketControl`](#handlewebsocketcontrol) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`MongooseSocket`](#mongoosesocket-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`MongooseSocket`](#mongoosesocket-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connected`](#connected) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getConnection`](#getconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getRemoteAddress`](#getremoteaddress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getLocalAddress`](#getlocaladdress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getType`](#gettype) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`Type`](#type) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onOpen`](#onopen) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onResolve`](#onresolve) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onConnect`](#onconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onAccept`](#onaccept) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onReceive`](#onreceive) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onSend`](#onsend) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onPoll`](#onpoll) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`handleEvent`](#handleevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connect`](#connect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setCertificate`](#setcertificate) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setRejectUnauthorized`](#setrejectunauthorized) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearSecurity`](#clearsecurity) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearConnection`](#clearconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`disconnect`](#disconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`abort`](#abort) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_nc`](#_nc) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onError`](#_onerror) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onClose`](#_onclose) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_secure`](#_secure) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_reject_unauthorized`](#_reject_unauthorized) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_host`](#_host) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_cert`](#_cert) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_key`](#_key) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`processEvent`](#processevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`message`](#message) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`body`](#body) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`methodStr`](#methodstr) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`uri`](#uri) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`proto`](#proto) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`respCode`](#respcode) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`respStatusMsg`](#respstatusmsg) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`queryString`](#querystring) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headers`](#headers) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headers`](#headers-1) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headerNames`](#headernames) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headerValues`](#headervalues) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`host`](#host) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`contentType`](#contenttype) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`_msg`](#_msg) | `variable` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`MongooseHttpMessage`](#mongoosehttpmessage-1) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |

### Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseHttpServerConnection`](#mongoosehttpserverconnection-1) `inline` |  |
| `function` | [`handleEvent`](#handleevent-5) `virtual` |  |
| `function` | [`handleHeaders`](#handleheaders-1) `virtual` `inline` |  |
| `function` | [`handleMessage`](#handlemessage-1) `virtual` `inline` |  |
| `function` | [`handleWebSocketConnect`](#handlewebsocketconnect) `virtual` `inline` |  |
| `function` | [`handleWebSocketMessage`](#handlewebsocketmessage) `virtual` `inline` |  |
| `function` | [`handleWebSocketControl`](#handlewebsocketcontrol) `virtual` `inline` |  |

### Inherited from [`MongooseSocket`](#mongoosesocket)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseSocket`](#mongoosesocket-1)  |  |
| `function` | [`MongooseSocket`](#mongoosesocket-2)  |  |
| `function` | [`connected`](#connected) `virtual` `inline` | Check if the socket is connected. |
| `function` | [`onError`](#onerror) `inline` |  |
| `function` | [`onClose`](#onclose) `inline` |  |
| `function` | [`getConnection`](#getconnection) `inline` | Get the underlying Mongoose connection object. |
| `function` | [`getRemoteAddress`](#getremoteaddress) `inline` | Get the remote network address. |
| `function` | [`getLocalAddress`](#getlocaladdress) `inline` | Get the local network address. |
| `function` | [`getType`](#gettype) `virtual` `inline` |  |
| `variable` | [`Type`](#type) `static` |  |
| `function` | [`onOpen`](#onopen) `virtual` |  |
| `function` | [`onResolve`](#onresolve) `virtual` |  |
| `function` | [`onConnect`](#onconnect) `virtual` |  |
| `function` | [`onAccept`](#onaccept) `virtual` |  |
| `function` | [`onError`](#onerror-1) `virtual` |  |
| `function` | [`onReceive`](#onreceive) `virtual` |  |
| `function` | [`onSend`](#onsend) `virtual` |  |
| `function` | [`onPoll`](#onpoll) `virtual` |  |
| `function` | [`onClose`](#onclose-1) `virtual` |  |
| `function` | [`handleEvent`](#handleevent) `virtual` |  |
| `function` | [`connect`](#connect)  |  |
| `function` | [`setSecure`](#setsecure) `inline` |  |
| `function` | [`setSecure`](#setsecure-1) `inline` |  |
| `function` | [`setSecure`](#setsecure-2) `inline` |  |
| `function` | [`setCertificate`](#setcertificate) `inline` |  |
| `function` | [`setRejectUnauthorized`](#setrejectunauthorized) `inline` |  |
| `function` | [`clearSecurity`](#clearsecurity) `inline` |  |
| `function` | [`clearConnection`](#clearconnection) `inline` |  |
| `function` | [`disconnect`](#disconnect) `inline` | Gracefully disconnect the socket (drain pending data) |
| `function` | [`abort`](#abort) `inline` | Forcibly abort the socket connection immediately. |
| `variable` | [`_nc`](#_nc)  |  |
| `variable` | [`_onError`](#_onerror)  |  |
| `variable` | [`_onClose`](#_onclose)  |  |
| `variable` | [`_secure`](#_secure)  |  |
| `variable` | [`_reject_unauthorized`](#_reject_unauthorized)  |  |
| `variable` | [`_host`](#_host)  |  |
| `variable` | [`_cert`](#_cert)  |  |
| `variable` | [`_key`](#_key)  |  |
| `function` | [`processEvent`](#processevent)  |  |

### Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`message`](#message) `inline` |  |
| `function` | [`body`](#body) `inline` |  |
| `function` | [`methodStr`](#methodstr) `inline` |  |
| `function` | [`uri`](#uri) `inline` |  |
| `function` | [`proto`](#proto) `inline` |  |
| `function` | [`respCode`](#respcode) `inline` |  |
| `function` | [`respStatusMsg`](#respstatusmsg) `inline` |  |
| `function` | [`queryString`](#querystring) `inline` |  |
| `function` | [`headers`](#headers) `inline` |  |
| `function` | [`headers`](#headers-1) `inline` |  |
| `function` | [`headerNames`](#headernames) `inline` |  |
| `function` | [`headerValues`](#headervalues) `inline` |  |
| `function` | [`host`](#host) `inline` |  |
| `function` | [`contentType`](#contenttype) `inline` |  |
| `variable` | [`_msg`](#_msg)  |  |
| `function` | [`MongooseHttpMessage`](#mongoosehttpmessage-1) `inline` |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-1)  |  |
| `bool` | [`isUpload`](#isupload) `virtual` `inline` |  |
| `bool` | [`isWebSocket`](#iswebsocket) `virtual` `inline` |  |
| `HttpRequestMethodComposite` | [`method`](#method-1) `inline` |  |
| `size_t` | [`contentLength`](#contentlength) `inline` |  |
| `void` | [`redirect`](#redirect)  | Redirect the client to a different URL. |
| `MongooseHttpServerResponseBasic *` | [`beginResponse`](#beginresponse)  | Begin a basic HTTP response. |
| `void` | [`send`](#send-2)  |  |
| `bool` | [`responseSent`](#responsesent) `inline` |  |
| `void` | [`send`](#send-3)  | Send a simple HTTP status code response. |
| `void` | [`send`](#send-4)  | Send a simple HTTP response with content. |
| `bool` | [`hasParam`](#hasparam) `const` | Check if a GET/POST parameter exists. |
| `int` | [`getParam`](#getparam) `const` | Get the value of a GET/POST parameter. |
| `bool` | [`authenticate`](#authenticate)  | Perform Basic Authentication against provided credentials. |
| `void` | [`requestAuthentication`](#requestauthentication)  | Respond with a 401 Unauthorized requesting Basic Authentication. |

---

{#mongoosehttpserverrequest-1}

#### MongooseHttpServerRequest

```cpp
MongooseHttpServerRequest(mg_connection * nc, HttpRequestMethodComposite method, mg_http_message * msg, MongooseHttpServerEndpoint * endpoint)
```

Defined in src/MongooseHttpServerRequest.h:48

---

{#isupload}

#### isUpload

`virtual` `inline`

```cpp
virtual inline bool isUpload()
```

Defined in src/MongooseHttpServerRequest.h:51

##### Reimplemented by

- [`isUpload`](#isupload-1)

---

{#iswebsocket}

#### isWebSocket

`virtual` `inline`

```cpp
virtual inline bool isWebSocket()
```

Defined in src/MongooseHttpServerRequest.h:52

##### Reimplemented by

- [`isWebSocket`](#iswebsocket-1)

---

{#method-1}

#### method

`inline`

```cpp
inline HttpRequestMethodComposite method()
```

Defined in src/MongooseHttpServerRequest.h:54

---

{#contentlength}

#### contentLength

`inline`

```cpp
inline size_t contentLength()
```

Defined in src/MongooseHttpServerRequest.h:58

---

{#redirect}

#### redirect

```cpp
void redirect(const char * url)
```

Defined in src/MongooseHttpServerRequest.h:66

Redirect the client to a different URL.

#### Parameters
* `url` The target URL

---

{#beginresponse}

#### beginResponse

```cpp
MongooseHttpServerResponseBasic * beginResponse()
```

Defined in src/MongooseHttpServerRequest.h:75

Begin a basic HTTP response.

#### Returns
MongooseHttpServerResponseBasic* Response object to populate

---

{#send-2}

#### send

```cpp
void send(MongooseHttpServerResponse * response)
```

Defined in src/MongooseHttpServerRequest.h:86

---

{#responsesent}

#### responseSent

`inline`

```cpp
inline bool responseSent()
```

Defined in src/MongooseHttpServerRequest.h:87

---

{#send-3}

#### send

```cpp
void send(int code)
```

Defined in src/MongooseHttpServerRequest.h:95

Send a simple HTTP status code response.

#### Parameters
* `code` HTTP status code (e.g. 200, 404)

---

{#send-4}

#### send

```cpp
void send(int code, const char * contentType, const char * content = "")
```

Defined in src/MongooseHttpServerRequest.h:102

Send a simple HTTP response with content.

#### Parameters
* `code` HTTP status code 

* `contentType` MIME type of the content 

* `content` Body payload

---

{#hasparam}

#### hasParam

`const`

```cpp
bool hasParam(const char * name) const
```

Defined in src/MongooseHttpServerRequest.h:112

Check if a GET/POST parameter exists.

#### Parameters
* `name` The parameter name 

#### Returns
true if parameter exists

---

{#getparam}

#### getParam

`const`

```cpp
int getParam(const char * name, char * dst, size_t dst_len) const
```

Defined in src/MongooseHttpServerRequest.h:125

Get the value of a GET/POST parameter.

#### Parameters
* `name` The parameter name 

* `dst` Buffer to store the value 

* `dst_len` Size of the destination buffer 

#### Returns
length of the extracted parameter

---

{#authenticate}

#### authenticate

```cpp
bool authenticate(const char * username, const char * password)
```

Defined in src/MongooseHttpServerRequest.h:143

Perform Basic Authentication against provided credentials.

#### Parameters
* `username` Expected username 

* `password` Expected password 

#### Returns
true if credentials match

---

{#requestauthentication}

#### requestAuthentication

```cpp
void requestAuthentication(const char * realm)
```

Defined in src/MongooseHttpServerRequest.h:153

Respond with a 401 Unauthorized requesting Basic Authentication.

#### Parameters
* `realm` The authentication realm to display

### Protected Attributes

| Return | Name | Description |
|--------|------|-------------|
| `HttpRequestMethodComposite` | [`_method`](#_method-1)  |  |
| `MongooseHttpServerResponse *` | [`_response`](#_response)  |  |
| `MongooseHttpServerEndpoint *` | [`_endpoint`](#_endpoint)  |  |
| `bool` | [`_responseSent`](#_responsesent)  |  |

---

{#_method-1}

#### _method

```cpp
HttpRequestMethodComposite _method
```

Defined in src/MongooseHttpServerRequest.h:36

---

{#_response}

#### _response

```cpp
MongooseHttpServerResponse * _response
```

Defined in src/MongooseHttpServerRequest.h:37

---

{#_endpoint}

#### _endpoint

```cpp
MongooseHttpServerEndpoint * _endpoint
```

Defined in src/MongooseHttpServerRequest.h:38

---

{#_responsesent}

#### _responseSent

```cpp
bool _responseSent
```

Defined in src/MongooseHttpServerRequest.h:39

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`sendBody`](#sendbody)  |  |
| `mg_http_message *` | [`duplicateMessage`](#duplicatemessage)  |  |

---

{#sendbody}

#### sendBody

```cpp
void sendBody()
```

Defined in src/MongooseHttpServerRequest.h:41

---

{#duplicatemessage}

#### duplicateMessage

```cpp
mg_http_message * duplicateMessage(mg_http_message *)
```

Defined in src/MongooseHttpServerRequest.h:44

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`handlePoll`](#handlepoll)  |  |
| `void` | [`handleSend`](#handlesend) `inline` |  |
| `void` | [`handleClose`](#handleclose)  |  |
| `void` | [`handleMessage`](#handlemessage) `virtual` |  |

---

{#handlepoll}

#### handlePoll

```cpp
void handlePoll(mg_connection * nc)
```

Defined in src/MongooseHttpServerRequest.h:28

---

{#handlesend}

#### handleSend

`inline`

```cpp
inline void handleSend(mg_connection * nc, int num_bytes)
```

Defined in src/MongooseHttpServerRequest.h:29

---

{#handleclose}

#### handleClose

```cpp
void handleClose(mg_connection * nc)
```

Defined in src/MongooseHttpServerRequest.h:32

---

{#handlemessage}

#### handleMessage

`virtual`

```cpp
virtual void handleMessage(mg_connection * nc, mg_http_message * msg)
```

Defined in src/MongooseHttpServerRequest.h:33

##### Reimplements

- [`handleMessage`](#handlemessage-1)

##### Reimplemented by

- [`handleMessage`](#handlemessage-2)

{#mongoosehttpclientresponse}

## MongooseHttpClientResponse

```cpp
#include <MongooseHttpClient.h>
```

```cpp
class MongooseHttpClientResponse
```

Defined in src/MongooseHttpClient.h:130

> **Inherits:** [`MongooseHttpMessage`](#mongoosehttpmessage)

Represents an inbound HTTP response.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpClientResponse`](#mongoosehttpclientresponse-1) | `function` | Declared here |
| [`contentLength`](#contentlength-1) | `function` | Declared here |
| [`message`](#message) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`body`](#body) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`methodStr`](#methodstr) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`uri`](#uri) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`proto`](#proto) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`respCode`](#respcode) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`respStatusMsg`](#respstatusmsg) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`queryString`](#querystring) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headers`](#headers) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headers`](#headers-1) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headerNames`](#headernames) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headerValues`](#headervalues) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`host`](#host) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`contentType`](#contenttype) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`_msg`](#_msg) | `variable` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`MongooseHttpMessage`](#mongoosehttpmessage-1) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |

### Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`message`](#message) `inline` |  |
| `function` | [`body`](#body) `inline` |  |
| `function` | [`methodStr`](#methodstr) `inline` |  |
| `function` | [`uri`](#uri) `inline` |  |
| `function` | [`proto`](#proto) `inline` |  |
| `function` | [`respCode`](#respcode) `inline` |  |
| `function` | [`respStatusMsg`](#respstatusmsg) `inline` |  |
| `function` | [`queryString`](#querystring) `inline` |  |
| `function` | [`headers`](#headers) `inline` |  |
| `function` | [`headers`](#headers-1) `inline` |  |
| `function` | [`headerNames`](#headernames) `inline` |  |
| `function` | [`headerValues`](#headervalues) `inline` |  |
| `function` | [`host`](#host) `inline` |  |
| `function` | [`contentType`](#contenttype) `inline` |  |
| `variable` | [`_msg`](#_msg)  |  |
| `function` | [`MongooseHttpMessage`](#mongoosehttpmessage-1) `inline` |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpClientResponse`](#mongoosehttpclientresponse-1) `inline` |  |
| `size_t` | [`contentLength`](#contentlength-1)  |  |

---

{#mongoosehttpclientresponse-1}

#### MongooseHttpClientResponse

`inline`

```cpp
inline MongooseHttpClientResponse(mg_http_message * msg)
```

Defined in src/MongooseHttpClient.h:132

---

{#contentlength-1}

#### contentLength

```cpp
size_t contentLength()
```

Defined in src/MongooseHttpClient.h:140

{#mongoosehttpserverendpoint}

## MongooseHttpServerEndpoint

```cpp
#include <MongooseHttpServerEndpoint.h>
```

```cpp
class MongooseHttpServerEndpoint
```

Defined in src/MongooseHttpServerEndpoint.h:29

> **Subclassed by:** [`MongooseHttpServerEndpointUpload`](#mongoosehttpserverendpointupload), [`MongooseHttpServerEndpointWebSocket`](#mongoosehttpserverendpointwebsocket)

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint-1) | `function` | Declared here |
| [`onRequest`](#onrequest-1) | `function` | Declared here |
| [`onClose`](#onclose-9) | `function` | Declared here |
| [`requestFactory`](#requestfactory) | `function` | Declared here |
| [`MongooseHttpServer`](#mongoosehttpserver-3) | `variable` | Declared here |
| [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-2) | `variable` | Declared here |
| [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload) | `variable` | Declared here |
| [`_method`](#_method-2) | `variable` | Declared here |
| [`_uri`](#_uri-1) | `variable` | Declared here |
| [`_request`](#_request) | `variable` | Declared here |
| [`_close`](#_close) | `variable` | Declared here |
| [`willHandleRequest`](#willhandlerequest) | `function` | Declared here |
| [`handleRequest`](#handlerequest) | `function` | Declared here |
| [`handleClose`](#handleclose-1) | `function` | Declared here |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint-1) `inline` |  |
| `MongooseHttpServerEndpoint *` | [`onRequest`](#onrequest-1) `inline` |  |
| `MongooseHttpServerEndpoint *` | [`onClose`](#onclose-9) `inline` |  |

---

{#mongoosehttpserverendpoint-1}

#### MongooseHttpServerEndpoint

`inline`

```cpp
inline MongooseHttpServerEndpoint(HttpRequestMethodComposite method, const char * uri = nullptr)
```

Defined in src/MongooseHttpServerEndpoint.h:51

---

{#onrequest-1}

#### onRequest

`inline`

```cpp
inline MongooseHttpServerEndpoint * onRequest(MongooseHttpRequestHandler handler)
```

Defined in src/MongooseHttpServerEndpoint.h:62

---

{#onclose-9}

#### onClose

`inline`

```cpp
inline MongooseHttpServerEndpoint * onClose(MongooseHttpRequestHandler handler)
```

Defined in src/MongooseHttpServerEndpoint.h:67

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `MongooseHttpServerRequest *` | [`requestFactory`](#requestfactory) `virtual` `inline` |  |

---

{#requestfactory}

#### requestFactory

`virtual` `inline`

```cpp
virtual inline MongooseHttpServerRequest * requestFactory(mg_connection * nc, HttpRequestMethodComposite method, mg_http_message * msg)
```

Defined in src/MongooseHttpServerEndpoint.h:46

##### Reimplemented by

- [`requestFactory`](#requestfactory-1)
- [`requestFactory`](#requestfactory-2)

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `friend` | [`MongooseHttpServer`](#mongoosehttpserver-3)  |  |
| `friend` | [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-2)  |  |
| `friend` | [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload)  |  |
| `HttpRequestMethodComposite` | [`_method`](#_method-2)  |  |
| `MongooseString` | [`_uri`](#_uri-1)  |  |
| `MongooseHttpRequestHandler` | [`_request`](#_request)  |  |
| `MongooseHttpRequestHandler` | [`_close`](#_close)  |  |

---

{#mongoosehttpserver-3}

#### MongooseHttpServer

```cpp
friend MongooseHttpServer
```

Defined in src/MongooseHttpServerEndpoint.h:31

---

{#mongoosehttpserverrequest-2}

#### MongooseHttpServerRequest

```cpp
friend MongooseHttpServerRequest
```

Defined in src/MongooseHttpServerEndpoint.h:32

---

{#mongoosehttpserverrequestupload}

#### MongooseHttpServerRequestUpload

```cpp
friend MongooseHttpServerRequestUpload
```

Defined in src/MongooseHttpServerEndpoint.h:33

---

{#_method-2}

#### _method

```cpp
HttpRequestMethodComposite _method
```

Defined in src/MongooseHttpServerEndpoint.h:36

---

{#_uri-1}

#### _uri

```cpp
MongooseString _uri
```

Defined in src/MongooseHttpServerEndpoint.h:37

---

{#_request}

#### _request

```cpp
MongooseHttpRequestHandler _request
```

Defined in src/MongooseHttpServerEndpoint.h:39

---

{#_close}

#### _close

```cpp
MongooseHttpRequestHandler _close
```

Defined in src/MongooseHttpServerEndpoint.h:40

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `RequestHandle` | [`willHandleRequest`](#willhandlerequest)  |  |
| `void` | [`handleRequest`](#handlerequest)  |  |
| `void` | [`handleClose`](#handleclose-1)  |  |

---

{#willhandlerequest}

#### willHandleRequest

```cpp
RequestHandle willHandleRequest(mg_connection * nc, HttpRequestMethodComposite requestMethod, mg_http_message * msg)
```

Defined in src/MongooseHttpServerEndpoint.h:42

---

{#handlerequest}

#### handleRequest

```cpp
void handleRequest(MongooseHttpServerRequest * request)
```

Defined in src/MongooseHttpServerEndpoint.h:43

---

{#handleclose-1}

#### handleClose

```cpp
void handleClose(MongooseHttpServerRequest * request)
```

Defined in src/MongooseHttpServerEndpoint.h:44

{#mongoosehttpserverresponse}

## MongooseHttpServerResponse

```cpp
#include <MongooseHttpServerResponse.h>
```

```cpp
class MongooseHttpServerResponse
```

Defined in src/MongooseHttpServerResponse.h:19

> **Subclassed by:** [`MongooseHttpServerResponseBasic`](#mongoosehttpserverresponsebasic)

Base class for HTTP server responses.

Implementations handle formatting and sending response headers and bodies.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpServerResponse`](#mongoosehttpserverresponse-1) | `function` | Declared here |
| [`setCode`](#setcode) | `function` | Declared here |
| [`setContentType`](#setcontenttype-1) | `function` | Declared here |
| [`setContentLength`](#setcontentlength-1) | `function` | Declared here |
| [`addHeader`](#addheader-3) | `function` | Declared here |
| [`sendHeaders`](#sendheaders) | `function` | Declared here |
| [`sendBody`](#sendbody-1) | `function` | Declared here |
| [`_code`](#_code) | `variable` | Declared here |
| [`_contentType`](#_contenttype-1) | `variable` | Declared here |
| [`_contentLength`](#_contentlength-1) | `variable` | Declared here |
| [`_headerBuffer`](#_headerbuffer) | `variable` | Declared here |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpServerResponse`](#mongoosehttpserverresponse-1)  |  |
| `void` | [`setCode`](#setcode) `inline` | Set the HTTP response code (e.g., 200, 404) |
| `void` | [`setContentType`](#setcontenttype-1)  | Set the Content-Type header. |
| `void` | [`setContentLength`](#setcontentlength-1) `inline` | Set the Content-Length header. |
| `bool` | [`addHeader`](#addheader-3)  | Add a custom HTTP header. |
| `void` | [`sendHeaders`](#sendheaders) `virtual` | Send formatted HTTP headers to the connection. |
| `size_t` | [`sendBody`](#sendbody-1) `virtual` | Send (a part of) the body to the connection. |

---

{#mongoosehttpserverresponse-1}

#### MongooseHttpServerResponse

```cpp
MongooseHttpServerResponse()
```

Defined in src/MongooseHttpServerResponse.h:29

---

{#setcode}

#### setCode

`inline`

```cpp
inline void setCode(int code)
```

Defined in src/MongooseHttpServerResponse.h:36

Set the HTTP response code (e.g., 200, 404)

#### Parameters
* `code` The HTTP status code

---

{#setcontenttype-1}

#### setContentType

```cpp
void setContentType(const char * contentType)
```

Defined in src/MongooseHttpServerResponse.h:43

Set the Content-Type header.

#### Parameters
* `contentType` MIME type

---

{#setcontentlength-1}

#### setContentLength

`inline`

```cpp
inline void setContentLength(int64_t contentLength)
```

Defined in src/MongooseHttpServerResponse.h:48

Set the Content-Length header.

#### Parameters
* `contentLength` Size of the body

---

{#addheader-3}

#### addHeader

```cpp
bool addHeader(const char * name, const char * value)
```

Defined in src/MongooseHttpServerResponse.h:58

Add a custom HTTP header.

#### Parameters
* `name` Header name 

* `value` Header value 

#### Returns
true if successfully added

---

{#sendheaders}

#### sendHeaders

`virtual`

```cpp
virtual void sendHeaders(struct mg_connection * nc)
```

Defined in src/MongooseHttpServerResponse.h:72

Send formatted HTTP headers to the connection.

#### Parameters
* `nc` Mongoose connection

---

{#sendbody-1}

#### sendBody

`virtual`

```cpp
virtual size_t sendBody(struct mg_connection * nc, size_t bytes)
```

Defined in src/MongooseHttpServerResponse.h:81

Send (a part of) the body to the connection.

#### Parameters
* `nc` Mongoose connection 

* `bytes` Maximum number of bytes to send in this chunk 

#### Returns
size_t Number of bytes actually sent

##### Reimplemented by

- [`sendBody`](#sendbody-2)

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `int` | [`_code`](#_code)  |  |
| `char *` | [`_contentType`](#_contenttype-1)  |  |
| `int64_t` | [`_contentLength`](#_contentlength-1)  |  |
| `char *` | [`_headerBuffer`](#_headerbuffer)  |  |

---

{#_code}

#### _code

```cpp
int _code
```

Defined in src/MongooseHttpServerResponse.h:22

---

{#_contenttype-1}

#### _contentType

```cpp
char * _contentType
```

Defined in src/MongooseHttpServerResponse.h:23

---

{#_contentlength-1}

#### _contentLength

```cpp
int64_t _contentLength
```

Defined in src/MongooseHttpServerResponse.h:24

---

{#_headerbuffer}

#### _headerBuffer

```cpp
char * _headerBuffer
```

Defined in src/MongooseHttpServerResponse.h:26

{#mongoosehttpserverconnection}

## MongooseHttpServerConnection

```cpp
#include <MongooseHttpServerConnection.h>
```

```cpp
class MongooseHttpServerConnection
```

Defined in src/MongooseHttpServerConnection.h:14

> **Inherits:** [`MongooseSocket`](#mongoosesocket)
> **Subclassed by:** [`MongooseHttpServer`](#mongoosehttpserver), [`MongooseHttpServerRequest`](#mongoosehttpserverrequest)

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpServerConnection`](#mongoosehttpserverconnection-1) | `function` | Declared here |
| [`handleEvent`](#handleevent-5) | `function` | Declared here |
| [`handleHeaders`](#handleheaders-1) | `function` | Declared here |
| [`handleMessage`](#handlemessage-1) | `function` | Declared here |
| [`handleWebSocketConnect`](#handlewebsocketconnect) | `function` | Declared here |
| [`handleWebSocketMessage`](#handlewebsocketmessage) | `function` | Declared here |
| [`handleWebSocketControl`](#handlewebsocketcontrol) | `function` | Declared here |
| [`MongooseSocket`](#mongoosesocket-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`MongooseSocket`](#mongoosesocket-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connected`](#connected) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getConnection`](#getconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getRemoteAddress`](#getremoteaddress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getLocalAddress`](#getlocaladdress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getType`](#gettype) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`Type`](#type) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onOpen`](#onopen) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onResolve`](#onresolve) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onConnect`](#onconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onAccept`](#onaccept) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onReceive`](#onreceive) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onSend`](#onsend) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onPoll`](#onpoll) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`handleEvent`](#handleevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connect`](#connect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setCertificate`](#setcertificate) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setRejectUnauthorized`](#setrejectunauthorized) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearSecurity`](#clearsecurity) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearConnection`](#clearconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`disconnect`](#disconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`abort`](#abort) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_nc`](#_nc) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onError`](#_onerror) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onClose`](#_onclose) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_secure`](#_secure) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_reject_unauthorized`](#_reject_unauthorized) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_host`](#_host) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_cert`](#_cert) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_key`](#_key) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`processEvent`](#processevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |

### Inherited from [`MongooseSocket`](#mongoosesocket)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseSocket`](#mongoosesocket-1)  |  |
| `function` | [`MongooseSocket`](#mongoosesocket-2)  |  |
| `function` | [`connected`](#connected) `virtual` `inline` | Check if the socket is connected. |
| `function` | [`onError`](#onerror) `inline` |  |
| `function` | [`onClose`](#onclose) `inline` |  |
| `function` | [`getConnection`](#getconnection) `inline` | Get the underlying Mongoose connection object. |
| `function` | [`getRemoteAddress`](#getremoteaddress) `inline` | Get the remote network address. |
| `function` | [`getLocalAddress`](#getlocaladdress) `inline` | Get the local network address. |
| `function` | [`getType`](#gettype) `virtual` `inline` |  |
| `variable` | [`Type`](#type) `static` |  |
| `function` | [`onOpen`](#onopen) `virtual` |  |
| `function` | [`onResolve`](#onresolve) `virtual` |  |
| `function` | [`onConnect`](#onconnect) `virtual` |  |
| `function` | [`onAccept`](#onaccept) `virtual` |  |
| `function` | [`onError`](#onerror-1) `virtual` |  |
| `function` | [`onReceive`](#onreceive) `virtual` |  |
| `function` | [`onSend`](#onsend) `virtual` |  |
| `function` | [`onPoll`](#onpoll) `virtual` |  |
| `function` | [`onClose`](#onclose-1) `virtual` |  |
| `function` | [`handleEvent`](#handleevent) `virtual` |  |
| `function` | [`connect`](#connect)  |  |
| `function` | [`setSecure`](#setsecure) `inline` |  |
| `function` | [`setSecure`](#setsecure-1) `inline` |  |
| `function` | [`setSecure`](#setsecure-2) `inline` |  |
| `function` | [`setCertificate`](#setcertificate) `inline` |  |
| `function` | [`setRejectUnauthorized`](#setrejectunauthorized) `inline` |  |
| `function` | [`clearSecurity`](#clearsecurity) `inline` |  |
| `function` | [`clearConnection`](#clearconnection) `inline` |  |
| `function` | [`disconnect`](#disconnect) `inline` | Gracefully disconnect the socket (drain pending data) |
| `function` | [`abort`](#abort) `inline` | Forcibly abort the socket connection immediately. |
| `variable` | [`_nc`](#_nc)  |  |
| `variable` | [`_onError`](#_onerror)  |  |
| `variable` | [`_onClose`](#_onclose)  |  |
| `variable` | [`_secure`](#_secure)  |  |
| `variable` | [`_reject_unauthorized`](#_reject_unauthorized)  |  |
| `variable` | [`_host`](#_host)  |  |
| `variable` | [`_cert`](#_cert)  |  |
| `variable` | [`_key`](#_key)  |  |
| `function` | [`processEvent`](#processevent)  |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpServerConnection`](#mongoosehttpserverconnection-1) `inline` |  |

---

{#mongoosehttpserverconnection-1}

#### MongooseHttpServerConnection

`inline`

```cpp
inline MongooseHttpServerConnection()
```

Defined in src/MongooseHttpServerConnection.h:28

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`handleEvent`](#handleevent-5) `virtual` |  |
| `void` | [`handleHeaders`](#handleheaders-1) `virtual` `inline` |  |
| `void` | [`handleMessage`](#handlemessage-1) `virtual` `inline` |  |
| `void` | [`handleWebSocketConnect`](#handlewebsocketconnect) `virtual` `inline` |  |
| `void` | [`handleWebSocketMessage`](#handlewebsocketmessage) `virtual` `inline` |  |
| `void` | [`handleWebSocketControl`](#handlewebsocketcontrol) `virtual` `inline` |  |

---

{#handleevent-5}

#### handleEvent

`virtual`

```cpp
virtual void handleEvent(mg_connection * nc, int ev, void * p)
```

Defined in src/MongooseHttpServerConnection.h:19

##### Reimplements

- [`handleEvent`](#handleevent)

---

{#handleheaders-1}

#### handleHeaders

`virtual` `inline`

```cpp
virtual inline void handleHeaders(mg_connection * nc, mg_http_message * msg)
```

Defined in src/MongooseHttpServerConnection.h:21

##### Reimplemented by

- [`handleHeaders`](#handleheaders)

---

{#handlemessage-1}

#### handleMessage

`virtual` `inline`

```cpp
virtual inline void handleMessage(mg_connection * nc, mg_http_message * msg)
```

Defined in src/MongooseHttpServerConnection.h:22

##### Reimplemented by

- [`handleMessage`](#handlemessage)
- [`handleMessage`](#handlemessage-2)

---

{#handlewebsocketconnect}

#### handleWebSocketConnect

`virtual` `inline`

```cpp
virtual inline void handleWebSocketConnect(mg_connection * nc, mg_http_message * msg)
```

Defined in src/MongooseHttpServerConnection.h:23

##### Reimplemented by

- [`handleWebSocketConnect`](#handlewebsocketconnect-1)

---

{#handlewebsocketmessage}

#### handleWebSocketMessage

`virtual` `inline`

```cpp
virtual inline void handleWebSocketMessage(mg_connection * nc, mg_ws_message * msg)
```

Defined in src/MongooseHttpServerConnection.h:24

##### Reimplemented by

- [`handleWebSocketMessage`](#handlewebsocketmessage-1)

---

{#handlewebsocketcontrol}

#### handleWebSocketControl

`virtual` `inline`

```cpp
virtual inline void handleWebSocketControl(mg_connection * nc, mg_ws_message * msg)
```

Defined in src/MongooseHttpServerConnection.h:25

##### Reimplemented by

- [`handleWebSocketControl`](#handlewebsocketcontrol-1)

{#mongoosehttpserverrequestupload-1}

## MongooseHttpServerRequestUpload

```cpp
#include <MongooseHttpServerRequestUpload.h>
```

```cpp
class MongooseHttpServerRequestUpload
```

Defined in src/MongooseHttpServerRequestUpload.h:11

> **Inherits:** [`MongooseHttpServerRequest`](#mongoosehttpserverrequest)

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload-2) | `function` | Declared here |
| [`isUpload`](#isupload-1) | `function` | Declared here |
| [`onReceive`](#onreceive-1) | `function` | Declared here |
| [`index`](#index) | `variable` | Declared here |
| [`_streaming`](#_streaming) | `variable` | Declared here |
| [`_bodyExpected`](#_bodyexpected) | `variable` | Declared here |
| [`_bodyReceived`](#_bodyreceived) | `variable` | Declared here |
| [`handleMessage`](#handlemessage-2) | `function` | Declared here |
| [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-1) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`isUpload`](#isupload) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`isWebSocket`](#iswebsocket) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`method`](#method-1) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`contentLength`](#contentlength) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`redirect`](#redirect) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`beginResponse`](#beginresponse) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`send`](#send-2) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`responseSent`](#responsesent) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`send`](#send-3) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`send`](#send-4) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`hasParam`](#hasparam) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`getParam`](#getparam) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`authenticate`](#authenticate) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`requestAuthentication`](#requestauthentication) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`_method`](#_method-1) | `variable` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`_response`](#_response) | `variable` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`_endpoint`](#_endpoint) | `variable` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`_responseSent`](#_responsesent) | `variable` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`sendBody`](#sendbody) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`duplicateMessage`](#duplicatemessage) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`handlePoll`](#handlepoll) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`handleSend`](#handlesend) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`handleClose`](#handleclose) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`handleMessage`](#handlemessage) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`MongooseHttpServerConnection`](#mongoosehttpserverconnection-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleEvent`](#handleevent-5) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleHeaders`](#handleheaders-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleMessage`](#handlemessage-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketConnect`](#handlewebsocketconnect) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketMessage`](#handlewebsocketmessage) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketControl`](#handlewebsocketcontrol) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`MongooseSocket`](#mongoosesocket-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`MongooseSocket`](#mongoosesocket-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connected`](#connected) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getConnection`](#getconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getRemoteAddress`](#getremoteaddress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getLocalAddress`](#getlocaladdress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getType`](#gettype) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`Type`](#type) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onOpen`](#onopen) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onResolve`](#onresolve) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onConnect`](#onconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onAccept`](#onaccept) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onReceive`](#onreceive) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onSend`](#onsend) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onPoll`](#onpoll) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`handleEvent`](#handleevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connect`](#connect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setCertificate`](#setcertificate) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setRejectUnauthorized`](#setrejectunauthorized) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearSecurity`](#clearsecurity) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearConnection`](#clearconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`disconnect`](#disconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`abort`](#abort) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_nc`](#_nc) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onError`](#_onerror) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onClose`](#_onclose) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_secure`](#_secure) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_reject_unauthorized`](#_reject_unauthorized) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_host`](#_host) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_cert`](#_cert) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_key`](#_key) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`processEvent`](#processevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`message`](#message) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`body`](#body) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`methodStr`](#methodstr) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`uri`](#uri) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`proto`](#proto) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`respCode`](#respcode) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`respStatusMsg`](#respstatusmsg) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`queryString`](#querystring) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headers`](#headers) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headers`](#headers-1) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headerNames`](#headernames) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headerValues`](#headervalues) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`host`](#host) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`contentType`](#contenttype) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`_msg`](#_msg) | `variable` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`MongooseHttpMessage`](#mongoosehttpmessage-1) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |

### Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-1)  |  |
| `function` | [`isUpload`](#isupload) `virtual` `inline` |  |
| `function` | [`isWebSocket`](#iswebsocket) `virtual` `inline` |  |
| `function` | [`method`](#method-1) `inline` |  |
| `function` | [`contentLength`](#contentlength) `inline` |  |
| `function` | [`redirect`](#redirect)  | Redirect the client to a different URL. |
| `function` | [`beginResponse`](#beginresponse)  | Begin a basic HTTP response. |
| `function` | [`send`](#send-2)  |  |
| `function` | [`responseSent`](#responsesent) `inline` |  |
| `function` | [`send`](#send-3)  | Send a simple HTTP status code response. |
| `function` | [`send`](#send-4)  | Send a simple HTTP response with content. |
| `function` | [`hasParam`](#hasparam) `const` | Check if a GET/POST parameter exists. |
| `function` | [`getParam`](#getparam) `const` | Get the value of a GET/POST parameter. |
| `function` | [`authenticate`](#authenticate)  | Perform Basic Authentication against provided credentials. |
| `function` | [`requestAuthentication`](#requestauthentication)  | Respond with a 401 Unauthorized requesting Basic Authentication. |
| `variable` | [`_method`](#_method-1)  |  |
| `variable` | [`_response`](#_response)  |  |
| `variable` | [`_endpoint`](#_endpoint)  |  |
| `variable` | [`_responseSent`](#_responsesent)  |  |
| `function` | [`sendBody`](#sendbody)  |  |
| `function` | [`duplicateMessage`](#duplicatemessage)  |  |
| `function` | [`handlePoll`](#handlepoll)  |  |
| `function` | [`handleSend`](#handlesend) `inline` |  |
| `function` | [`handleClose`](#handleclose)  |  |
| `function` | [`handleMessage`](#handlemessage) `virtual` |  |

### Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseHttpServerConnection`](#mongoosehttpserverconnection-1) `inline` |  |
| `function` | [`handleEvent`](#handleevent-5) `virtual` |  |
| `function` | [`handleHeaders`](#handleheaders-1) `virtual` `inline` |  |
| `function` | [`handleMessage`](#handlemessage-1) `virtual` `inline` |  |
| `function` | [`handleWebSocketConnect`](#handlewebsocketconnect) `virtual` `inline` |  |
| `function` | [`handleWebSocketMessage`](#handlewebsocketmessage) `virtual` `inline` |  |
| `function` | [`handleWebSocketControl`](#handlewebsocketcontrol) `virtual` `inline` |  |

### Inherited from [`MongooseSocket`](#mongoosesocket)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseSocket`](#mongoosesocket-1)  |  |
| `function` | [`MongooseSocket`](#mongoosesocket-2)  |  |
| `function` | [`connected`](#connected) `virtual` `inline` | Check if the socket is connected. |
| `function` | [`onError`](#onerror) `inline` |  |
| `function` | [`onClose`](#onclose) `inline` |  |
| `function` | [`getConnection`](#getconnection) `inline` | Get the underlying Mongoose connection object. |
| `function` | [`getRemoteAddress`](#getremoteaddress) `inline` | Get the remote network address. |
| `function` | [`getLocalAddress`](#getlocaladdress) `inline` | Get the local network address. |
| `function` | [`getType`](#gettype) `virtual` `inline` |  |
| `variable` | [`Type`](#type) `static` |  |
| `function` | [`onOpen`](#onopen) `virtual` |  |
| `function` | [`onResolve`](#onresolve) `virtual` |  |
| `function` | [`onConnect`](#onconnect) `virtual` |  |
| `function` | [`onAccept`](#onaccept) `virtual` |  |
| `function` | [`onError`](#onerror-1) `virtual` |  |
| `function` | [`onReceive`](#onreceive) `virtual` |  |
| `function` | [`onSend`](#onsend) `virtual` |  |
| `function` | [`onPoll`](#onpoll) `virtual` |  |
| `function` | [`onClose`](#onclose-1) `virtual` |  |
| `function` | [`handleEvent`](#handleevent) `virtual` |  |
| `function` | [`connect`](#connect)  |  |
| `function` | [`setSecure`](#setsecure) `inline` |  |
| `function` | [`setSecure`](#setsecure-1) `inline` |  |
| `function` | [`setSecure`](#setsecure-2) `inline` |  |
| `function` | [`setCertificate`](#setcertificate) `inline` |  |
| `function` | [`setRejectUnauthorized`](#setrejectunauthorized) `inline` |  |
| `function` | [`clearSecurity`](#clearsecurity) `inline` |  |
| `function` | [`clearConnection`](#clearconnection) `inline` |  |
| `function` | [`disconnect`](#disconnect) `inline` | Gracefully disconnect the socket (drain pending data) |
| `function` | [`abort`](#abort) `inline` | Forcibly abort the socket connection immediately. |
| `variable` | [`_nc`](#_nc)  |  |
| `variable` | [`_onError`](#_onerror)  |  |
| `variable` | [`_onClose`](#_onclose)  |  |
| `variable` | [`_secure`](#_secure)  |  |
| `variable` | [`_reject_unauthorized`](#_reject_unauthorized)  |  |
| `variable` | [`_host`](#_host)  |  |
| `variable` | [`_cert`](#_cert)  |  |
| `variable` | [`_key`](#_key)  |  |
| `function` | [`processEvent`](#processevent)  |  |

### Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`message`](#message) `inline` |  |
| `function` | [`body`](#body) `inline` |  |
| `function` | [`methodStr`](#methodstr) `inline` |  |
| `function` | [`uri`](#uri) `inline` |  |
| `function` | [`proto`](#proto) `inline` |  |
| `function` | [`respCode`](#respcode) `inline` |  |
| `function` | [`respStatusMsg`](#respstatusmsg) `inline` |  |
| `function` | [`queryString`](#querystring) `inline` |  |
| `function` | [`headers`](#headers) `inline` |  |
| `function` | [`headers`](#headers-1) `inline` |  |
| `function` | [`headerNames`](#headernames) `inline` |  |
| `function` | [`headerValues`](#headervalues) `inline` |  |
| `function` | [`host`](#host) `inline` |  |
| `function` | [`contentType`](#contenttype) `inline` |  |
| `variable` | [`_msg`](#_msg)  |  |
| `function` | [`MongooseHttpMessage`](#mongoosehttpmessage-1) `inline` |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload-2)  |  |
| `bool` | [`isUpload`](#isupload-1) `virtual` `inline` |  |

---

{#mongoosehttpserverrequestupload-2}

#### MongooseHttpServerRequestUpload

```cpp
MongooseHttpServerRequestUpload(mg_connection * nc, HttpRequestMethodComposite method, mg_http_message * msg, MongooseHttpServerEndpoint * endpoint)
```

Defined in src/MongooseHttpServerRequestUpload.h:32

---

{#isupload-1}

#### isUpload

`virtual` `inline`

```cpp
virtual inline bool isUpload()
```

Defined in src/MongooseHttpServerRequestUpload.h:40

##### Reimplements

- [`isUpload`](#isupload)

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`onReceive`](#onreceive-1) `virtual` `override` |  |

---

{#onreceive-1}

#### onReceive

`virtual` `override`

```cpp
virtual void onReceive(mg_connection * nc, long num_bytes) override
```

Defined in src/MongooseHttpServerRequestUpload.h:29

##### Reimplements

- [`onReceive`](#onreceive)

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint64_t` | [`index`](#index)  |  |
| `bool` | [`_streaming`](#_streaming)  |  |
| `size_t` | [`_bodyExpected`](#_bodyexpected)  |  |
| `size_t` | [`_bodyReceived`](#_bodyreceived)  |  |

---

{#index}

#### index

```cpp
uint64_t index
```

Defined in src/MongooseHttpServerRequestUpload.h:14

---

{#_streaming}

#### _streaming

```cpp
bool _streaming
```

Defined in src/MongooseHttpServerRequestUpload.h:19

---

{#_bodyexpected}

#### _bodyExpected

```cpp
size_t _bodyExpected
```

Defined in src/MongooseHttpServerRequestUpload.h:20

---

{#_bodyreceived}

#### _bodyReceived

```cpp
size_t _bodyReceived
```

Defined in src/MongooseHttpServerRequestUpload.h:21

### Private Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`handleMessage`](#handlemessage-2) `virtual` |  |

---

{#handlemessage-2}

#### handleMessage

`virtual`

```cpp
virtual void handleMessage(mg_connection * nc, mg_http_message * msg)
```

Defined in src/MongooseHttpServerRequestUpload.h:25

##### Reimplements

- [`handleMessage`](#handlemessage)

{#mongoosehttpserverresponsebasic}

## MongooseHttpServerResponseBasic

```cpp
#include <MongooseHttpServerResponseBasic.h>
```

```cpp
class MongooseHttpServerResponseBasic
```

Defined in src/MongooseHttpServerResponseBasic.h:18

> **Inherits:** [`MongooseHttpServerResponse`](#mongoosehttpserverresponse)

A basic HTTP server response containing a static payload.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpServerResponseBasic`](#mongoosehttpserverresponsebasic-1) | `function` | Declared here |
| [`setContent`](#setcontent-2) | `function` | Declared here |
| [`setContent`](#setcontent-3) | `function` | Declared here |
| [`setContent`](#setcontent-4) | `function` | Declared here |
| [`sendBody`](#sendbody-2) | `function` | Declared here |
| [`ptr`](#ptr) | `variable` | Declared here |
| [`len`](#len) | `variable` | Declared here |
| [`MongooseHttpServerResponse`](#mongoosehttpserverresponse-1) | `function` | Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) |
| [`setCode`](#setcode) | `function` | Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) |
| [`setContentType`](#setcontenttype-1) | `function` | Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) |
| [`setContentLength`](#setcontentlength-1) | `function` | Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) |
| [`addHeader`](#addheader-3) | `function` | Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) |
| [`sendHeaders`](#sendheaders) | `function` | Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) |
| [`sendBody`](#sendbody-1) | `function` | Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) |
| [`_code`](#_code) | `variable` | Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) |
| [`_contentType`](#_contenttype-1) | `variable` | Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) |
| [`_contentLength`](#_contentlength-1) | `variable` | Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) |
| [`_headerBuffer`](#_headerbuffer) | `variable` | Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse) |

### Inherited from [`MongooseHttpServerResponse`](#mongoosehttpserverresponse)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseHttpServerResponse`](#mongoosehttpserverresponse-1)  |  |
| `function` | [`setCode`](#setcode) `inline` | Set the HTTP response code (e.g., 200, 404) |
| `function` | [`setContentType`](#setcontenttype-1)  | Set the Content-Type header. |
| `function` | [`setContentLength`](#setcontentlength-1) `inline` | Set the Content-Length header. |
| `function` | [`addHeader`](#addheader-3)  | Add a custom HTTP header. |
| `function` | [`sendHeaders`](#sendheaders) `virtual` | Send formatted HTTP headers to the connection. |
| `function` | [`sendBody`](#sendbody-1) `virtual` | Send (a part of) the body to the connection. |
| `variable` | [`_code`](#_code)  |  |
| `variable` | [`_contentType`](#_contenttype-1)  |  |
| `variable` | [`_contentLength`](#_contentlength-1)  |  |
| `variable` | [`_headerBuffer`](#_headerbuffer)  |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpServerResponseBasic`](#mongoosehttpserverresponsebasic-1)  |  |
| `void` | [`setContent`](#setcontent-2)  | Set a null-terminated string as the response body. |
| `void` | [`setContent`](#setcontent-3)  | Set binary data as the response body. |
| `void` | [`setContent`](#setcontent-4) `inline` |  |
| `size_t` | [`sendBody`](#sendbody-2) `virtual` | Send (a part of) the body to the connection. |

---

{#mongoosehttpserverresponsebasic-1}

#### MongooseHttpServerResponseBasic

```cpp
MongooseHttpServerResponseBasic()
```

Defined in src/MongooseHttpServerResponseBasic.h:26

---

{#setcontent-2}

#### setContent

```cpp
void setContent(const char * content)
```

Defined in src/MongooseHttpServerResponseBasic.h:32

Set a null-terminated string as the response body.

#### Parameters
* `content` Null-terminated string payload

---

{#setcontent-3}

#### setContent

```cpp
void setContent(const uint8_t * content, size_t len)
```

Defined in src/MongooseHttpServerResponseBasic.h:38

Set binary data as the response body.

#### Parameters
* `content` Pointer to the data 

* `len` Length of the data

---

{#setcontent-4}

#### setContent

`inline`

```cpp
inline void setContent(MongooseString & content)
```

Defined in src/MongooseHttpServerResponseBasic.h:39

---

{#sendbody-2}

#### sendBody

`virtual`

```cpp
virtual size_t sendBody(struct mg_connection * nc, size_t bytes)
```

Defined in src/MongooseHttpServerResponseBasic.h:42

Send (a part of) the body to the connection.

#### Parameters
* `nc` Mongoose connection 

* `bytes` Maximum number of bytes to send in this chunk 

#### Returns
size_t Number of bytes actually sent

##### Reimplements

- [`sendBody`](#sendbody-1)

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const uint8_t *` | [`ptr`](#ptr)  |  |
| `size_t` | [`len`](#len)  |  |

---

{#ptr}

#### ptr

```cpp
const uint8_t * ptr
```

Defined in src/MongooseHttpServerResponseBasic.h:22

---

{#len}

#### len

```cpp
size_t len
```

Defined in src/MongooseHttpServerResponseBasic.h:23

{#mongoosehttpwebsocketconnection}

## MongooseHttpWebSocketConnection

```cpp
#include <MongooseHttpWebSocketConnection.h>
```

```cpp
class MongooseHttpWebSocketConnection
```

Defined in src/MongooseHttpWebSocketConnection.h:14

> **Inherits:** [`MongooseHttpServerRequest`](#mongoosehttpserverrequest)

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpWebSocketConnection`](#mongoosehttpwebsocketconnection-1) | `function` | Declared here |
| [`isWebSocket`](#iswebsocket-1) | `function` | Declared here |
| [`send`](#send-5) | `function` | Declared here |
| [`send`](#send-6) | `function` | Declared here |
| [`getType`](#gettype-1) | `function` | Declared here |
| [`Type`](#type-1) | `variable` | Declared here |
| [`handleWebSocketConnect`](#handlewebsocketconnect-1) | `function` | Declared here |
| [`handleWebSocketMessage`](#handlewebsocketmessage-1) | `function` | Declared here |
| [`handleWebSocketControl`](#handlewebsocketcontrol-1) | `function` | Declared here |
| [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-1) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`isUpload`](#isupload) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`isWebSocket`](#iswebsocket) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`method`](#method-1) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`contentLength`](#contentlength) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`redirect`](#redirect) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`beginResponse`](#beginresponse) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`send`](#send-2) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`responseSent`](#responsesent) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`send`](#send-3) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`send`](#send-4) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`hasParam`](#hasparam) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`getParam`](#getparam) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`authenticate`](#authenticate) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`requestAuthentication`](#requestauthentication) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`_method`](#_method-1) | `variable` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`_response`](#_response) | `variable` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`_endpoint`](#_endpoint) | `variable` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`_responseSent`](#_responsesent) | `variable` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`sendBody`](#sendbody) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`duplicateMessage`](#duplicatemessage) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`handlePoll`](#handlepoll) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`handleSend`](#handlesend) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`handleClose`](#handleclose) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`handleMessage`](#handlemessage) | `function` | Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest) |
| [`MongooseHttpServerConnection`](#mongoosehttpserverconnection-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleEvent`](#handleevent-5) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleHeaders`](#handleheaders-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleMessage`](#handlemessage-1) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketConnect`](#handlewebsocketconnect) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketMessage`](#handlewebsocketmessage) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`handleWebSocketControl`](#handlewebsocketcontrol) | `function` | Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection) |
| [`MongooseSocket`](#mongoosesocket-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`MongooseSocket`](#mongoosesocket-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connected`](#connected) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getConnection`](#getconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getRemoteAddress`](#getremoteaddress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getLocalAddress`](#getlocaladdress) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`getType`](#gettype) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`Type`](#type) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onOpen`](#onopen) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onResolve`](#onresolve) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onConnect`](#onconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onAccept`](#onaccept) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onError`](#onerror-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onReceive`](#onreceive) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onSend`](#onsend) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onPoll`](#onpoll) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`onClose`](#onclose-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`handleEvent`](#handleevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`connect`](#connect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-1) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setSecure`](#setsecure-2) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setCertificate`](#setcertificate) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`setRejectUnauthorized`](#setrejectunauthorized) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearSecurity`](#clearsecurity) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`clearConnection`](#clearconnection) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`disconnect`](#disconnect) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`abort`](#abort) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_nc`](#_nc) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onError`](#_onerror) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_onClose`](#_onclose) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_secure`](#_secure) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_reject_unauthorized`](#_reject_unauthorized) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_host`](#_host) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_cert`](#_cert) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`_key`](#_key) | `variable` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`processEvent`](#processevent) | `function` | Inherited from [`MongooseSocket`](#mongoosesocket) |
| [`message`](#message) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`body`](#body) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`methodStr`](#methodstr) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`uri`](#uri) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`proto`](#proto) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`respCode`](#respcode) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`respStatusMsg`](#respstatusmsg) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`queryString`](#querystring) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headers`](#headers) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headers`](#headers-1) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headerNames`](#headernames) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`headerValues`](#headervalues) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`host`](#host) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`contentType`](#contenttype) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`_msg`](#_msg) | `variable` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |
| [`MongooseHttpMessage`](#mongoosehttpmessage-1) | `function` | Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage) |

### Inherited from [`MongooseHttpServerRequest`](#mongoosehttpserverrequest)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-1)  |  |
| `function` | [`isUpload`](#isupload) `virtual` `inline` |  |
| `function` | [`isWebSocket`](#iswebsocket) `virtual` `inline` |  |
| `function` | [`method`](#method-1) `inline` |  |
| `function` | [`contentLength`](#contentlength) `inline` |  |
| `function` | [`redirect`](#redirect)  | Redirect the client to a different URL. |
| `function` | [`beginResponse`](#beginresponse)  | Begin a basic HTTP response. |
| `function` | [`send`](#send-2)  |  |
| `function` | [`responseSent`](#responsesent) `inline` |  |
| `function` | [`send`](#send-3)  | Send a simple HTTP status code response. |
| `function` | [`send`](#send-4)  | Send a simple HTTP response with content. |
| `function` | [`hasParam`](#hasparam) `const` | Check if a GET/POST parameter exists. |
| `function` | [`getParam`](#getparam) `const` | Get the value of a GET/POST parameter. |
| `function` | [`authenticate`](#authenticate)  | Perform Basic Authentication against provided credentials. |
| `function` | [`requestAuthentication`](#requestauthentication)  | Respond with a 401 Unauthorized requesting Basic Authentication. |
| `variable` | [`_method`](#_method-1)  |  |
| `variable` | [`_response`](#_response)  |  |
| `variable` | [`_endpoint`](#_endpoint)  |  |
| `variable` | [`_responseSent`](#_responsesent)  |  |
| `function` | [`sendBody`](#sendbody)  |  |
| `function` | [`duplicateMessage`](#duplicatemessage)  |  |
| `function` | [`handlePoll`](#handlepoll)  |  |
| `function` | [`handleSend`](#handlesend) `inline` |  |
| `function` | [`handleClose`](#handleclose)  |  |
| `function` | [`handleMessage`](#handlemessage) `virtual` |  |

### Inherited from [`MongooseHttpServerConnection`](#mongoosehttpserverconnection)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseHttpServerConnection`](#mongoosehttpserverconnection-1) `inline` |  |
| `function` | [`handleEvent`](#handleevent-5) `virtual` |  |
| `function` | [`handleHeaders`](#handleheaders-1) `virtual` `inline` |  |
| `function` | [`handleMessage`](#handlemessage-1) `virtual` `inline` |  |
| `function` | [`handleWebSocketConnect`](#handlewebsocketconnect) `virtual` `inline` |  |
| `function` | [`handleWebSocketMessage`](#handlewebsocketmessage) `virtual` `inline` |  |
| `function` | [`handleWebSocketControl`](#handlewebsocketcontrol) `virtual` `inline` |  |

### Inherited from [`MongooseSocket`](#mongoosesocket)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseSocket`](#mongoosesocket-1)  |  |
| `function` | [`MongooseSocket`](#mongoosesocket-2)  |  |
| `function` | [`connected`](#connected) `virtual` `inline` | Check if the socket is connected. |
| `function` | [`onError`](#onerror) `inline` |  |
| `function` | [`onClose`](#onclose) `inline` |  |
| `function` | [`getConnection`](#getconnection) `inline` | Get the underlying Mongoose connection object. |
| `function` | [`getRemoteAddress`](#getremoteaddress) `inline` | Get the remote network address. |
| `function` | [`getLocalAddress`](#getlocaladdress) `inline` | Get the local network address. |
| `function` | [`getType`](#gettype) `virtual` `inline` |  |
| `variable` | [`Type`](#type) `static` |  |
| `function` | [`onOpen`](#onopen) `virtual` |  |
| `function` | [`onResolve`](#onresolve) `virtual` |  |
| `function` | [`onConnect`](#onconnect) `virtual` |  |
| `function` | [`onAccept`](#onaccept) `virtual` |  |
| `function` | [`onError`](#onerror-1) `virtual` |  |
| `function` | [`onReceive`](#onreceive) `virtual` |  |
| `function` | [`onSend`](#onsend) `virtual` |  |
| `function` | [`onPoll`](#onpoll) `virtual` |  |
| `function` | [`onClose`](#onclose-1) `virtual` |  |
| `function` | [`handleEvent`](#handleevent) `virtual` |  |
| `function` | [`connect`](#connect)  |  |
| `function` | [`setSecure`](#setsecure) `inline` |  |
| `function` | [`setSecure`](#setsecure-1) `inline` |  |
| `function` | [`setSecure`](#setsecure-2) `inline` |  |
| `function` | [`setCertificate`](#setcertificate) `inline` |  |
| `function` | [`setRejectUnauthorized`](#setrejectunauthorized) `inline` |  |
| `function` | [`clearSecurity`](#clearsecurity) `inline` |  |
| `function` | [`clearConnection`](#clearconnection) `inline` |  |
| `function` | [`disconnect`](#disconnect) `inline` | Gracefully disconnect the socket (drain pending data) |
| `function` | [`abort`](#abort) `inline` | Forcibly abort the socket connection immediately. |
| `variable` | [`_nc`](#_nc)  |  |
| `variable` | [`_onError`](#_onerror)  |  |
| `variable` | [`_onClose`](#_onclose)  |  |
| `variable` | [`_secure`](#_secure)  |  |
| `variable` | [`_reject_unauthorized`](#_reject_unauthorized)  |  |
| `variable` | [`_host`](#_host)  |  |
| `variable` | [`_cert`](#_cert)  |  |
| `variable` | [`_key`](#_key)  |  |
| `function` | [`processEvent`](#processevent)  |  |

### Inherited from [`MongooseHttpMessage`](#mongoosehttpmessage)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`message`](#message) `inline` |  |
| `function` | [`body`](#body) `inline` |  |
| `function` | [`methodStr`](#methodstr) `inline` |  |
| `function` | [`uri`](#uri) `inline` |  |
| `function` | [`proto`](#proto) `inline` |  |
| `function` | [`respCode`](#respcode) `inline` |  |
| `function` | [`respStatusMsg`](#respstatusmsg) `inline` |  |
| `function` | [`queryString`](#querystring) `inline` |  |
| `function` | [`headers`](#headers) `inline` |  |
| `function` | [`headers`](#headers-1) `inline` |  |
| `function` | [`headerNames`](#headernames) `inline` |  |
| `function` | [`headerValues`](#headervalues) `inline` |  |
| `function` | [`host`](#host) `inline` |  |
| `function` | [`contentType`](#contenttype) `inline` |  |
| `variable` | [`_msg`](#_msg)  |  |
| `function` | [`MongooseHttpMessage`](#mongoosehttpmessage-1) `inline` |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpWebSocketConnection`](#mongoosehttpwebsocketconnection-1)  |  |
| `bool` | [`isWebSocket`](#iswebsocket-1) `virtual` `inline` |  |
| `void` | [`send`](#send-5)  |  |
| `void` | [`send`](#send-6) `inline` |  |
| `char` | [`getType`](#gettype-1) `virtual` `inline` |  |

---

{#mongoosehttpwebsocketconnection-1}

#### MongooseHttpWebSocketConnection

```cpp
MongooseHttpWebSocketConnection(mg_connection * nc, HttpRequestMethodComposite method, mg_http_message * msg, MongooseHttpServerEndpoint * endpoint)
```

Defined in src/MongooseHttpWebSocketConnection.h:22

---

{#iswebsocket-1}

#### isWebSocket

`virtual` `inline`

```cpp
virtual inline bool isWebSocket()
```

Defined in src/MongooseHttpWebSocketConnection.h:25

##### Reimplements

- [`isWebSocket`](#iswebsocket)

---

{#send-5}

#### send

```cpp
void send(int op, const void * data, size_t len)
```

Defined in src/MongooseHttpWebSocketConnection.h:27

---

{#send-6}

#### send

`inline`

```cpp
inline void send(const char * buf)
```

Defined in src/MongooseHttpWebSocketConnection.h:28

---

{#gettype-1}

#### getType

`virtual` `inline`

```cpp
virtual inline char getType()
```

Defined in src/MongooseHttpWebSocketConnection.h:38

##### Reimplements

- [`getType`](#gettype)

### Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const char` | [`Type`](#type-1) `static` |  |

---

{#type-1}

#### Type

`static`

```cpp
const char Type = 'W'
```

Defined in src/MongooseHttpWebSocketConnection.h:37

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`handleWebSocketConnect`](#handlewebsocketconnect-1) `virtual` |  |
| `void` | [`handleWebSocketMessage`](#handlewebsocketmessage-1) `virtual` |  |
| `void` | [`handleWebSocketControl`](#handlewebsocketcontrol-1) `virtual` |  |

---

{#handlewebsocketconnect-1}

#### handleWebSocketConnect

`virtual`

```cpp
virtual void handleWebSocketConnect(mg_connection * nc, mg_http_message * msg)
```

Defined in src/MongooseHttpWebSocketConnection.h:17

##### Reimplements

- [`handleWebSocketConnect`](#handlewebsocketconnect)

---

{#handlewebsocketmessage-1}

#### handleWebSocketMessage

`virtual`

```cpp
virtual void handleWebSocketMessage(mg_connection * nc, mg_ws_message * msg)
```

Defined in src/MongooseHttpWebSocketConnection.h:18

##### Reimplements

- [`handleWebSocketMessage`](#handlewebsocketmessage)

---

{#handlewebsocketcontrol-1}

#### handleWebSocketControl

`virtual`

```cpp
virtual void handleWebSocketControl(mg_connection * nc, mg_ws_message * msg)
```

Defined in src/MongooseHttpWebSocketConnection.h:19

##### Reimplements

- [`handleWebSocketControl`](#handlewebsocketcontrol)

{#mongoosehttpserverendpointupload}

## MongooseHttpServerEndpointUpload

```cpp
#include <MongooseHttpServerEndpointUpload.h>
```

```cpp
class MongooseHttpServerEndpointUpload
```

Defined in src/MongooseHttpServerEndpointUpload.h:17

> **Inherits:** [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint)

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload-3) | `friend` | Declared here |
| [`MongooseHttpServerEndpointUpload`](#mongoosehttpserverendpointupload-1) | `function` | Declared here |
| [`onRequest`](#onrequest-2) | `function` | Declared here |
| [`onClose`](#onclose-10) | `function` | Declared here |
| [`onUpload`](#onupload) | `function` | Declared here |
| [`hasUploadHandler`](#hasuploadhandler) | `function` | Declared here |
| [`callUpload`](#callupload) | `function` | Declared here |
| [`requestFactory`](#requestfactory-1) | `function` | Declared here |
| [`_upload`](#_upload) | `variable` | Declared here |
| [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint-1) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`onRequest`](#onrequest-1) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`onClose`](#onclose-9) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`requestFactory`](#requestfactory) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`MongooseHttpServer`](#mongoosehttpserver-3) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-2) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`_method`](#_method-2) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`_uri`](#_uri-1) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`_request`](#_request) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`_close`](#_close) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`willHandleRequest`](#willhandlerequest) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`handleRequest`](#handlerequest) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`handleClose`](#handleclose-1) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |

### Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint-1) `inline` |  |
| `function` | [`onRequest`](#onrequest-1) `inline` |  |
| `function` | [`onClose`](#onclose-9) `inline` |  |
| `function` | [`requestFactory`](#requestfactory) `virtual` `inline` |  |
| `variable` | [`MongooseHttpServer`](#mongoosehttpserver-3)  |  |
| `variable` | [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-2)  |  |
| `variable` | [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload)  |  |
| `variable` | [`_method`](#_method-2)  |  |
| `variable` | [`_uri`](#_uri-1)  |  |
| `variable` | [`_request`](#_request)  |  |
| `variable` | [`_close`](#_close)  |  |
| `function` | [`willHandleRequest`](#willhandlerequest)  |  |
| `function` | [`handleRequest`](#handlerequest)  |  |
| `function` | [`handleClose`](#handleclose-1)  |  |

### Friends

| Name | Description |
|------|-------------|
| [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload-3)  |  |

---

{#mongoosehttpserverrequestupload-3}

#### MongooseHttpServerRequestUpload

```cpp
friend class MongooseHttpServerRequestUpload
```

Defined in src/MongooseHttpServerEndpointUpload.h:19

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpServerEndpointUpload`](#mongoosehttpserverendpointupload-1) `inline` |  |
| `MongooseHttpServerEndpointUpload *` | [`onRequest`](#onrequest-2) `inline` |  |
| `MongooseHttpServerEndpointUpload *` | [`onClose`](#onclose-10) `inline` |  |
| `MongooseHttpServerEndpointUpload *` | [`onUpload`](#onupload) `inline` |  |
| `bool` | [`hasUploadHandler`](#hasuploadhandler) `const` `inline` |  |
| `void` | [`callUpload`](#callupload) `inline` |  |

---

{#mongoosehttpserverendpointupload-1}

#### MongooseHttpServerEndpointUpload

`inline`

```cpp
inline MongooseHttpServerEndpointUpload(HttpRequestMethodComposite method, const char * uri = nullptr)
```

Defined in src/MongooseHttpServerEndpointUpload.h:30

---

{#onrequest-2}

#### onRequest

`inline`

```cpp
inline MongooseHttpServerEndpointUpload * onRequest(MongooseHttpRequestHandler handler)
```

Defined in src/MongooseHttpServerEndpointUpload.h:37

---

{#onclose-10}

#### onClose

`inline`

```cpp
inline MongooseHttpServerEndpointUpload * onClose(MongooseHttpRequestHandler handler)
```

Defined in src/MongooseHttpServerEndpointUpload.h:42

---

{#onupload}

#### onUpload

`inline`

```cpp
inline MongooseHttpServerEndpointUpload * onUpload(MongooseHttpUploadHandler handler)
```

Defined in src/MongooseHttpServerEndpointUpload.h:47

---

{#hasuploadhandler}

#### hasUploadHandler

`const` `inline`

```cpp
inline bool hasUploadHandler() const
```

Defined in src/MongooseHttpServerEndpointUpload.h:52

---

{#callupload}

#### callUpload

`inline`

```cpp
inline void callUpload(MongooseHttpServerRequest * request, int ev, MongooseString filename, uint64_t index, uint8_t * data, size_t len)
```

Defined in src/MongooseHttpServerEndpointUpload.h:56

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `MongooseHttpServerRequest *` | [`requestFactory`](#requestfactory-1) `virtual` `inline` |  |

---

{#requestfactory-1}

#### requestFactory

`virtual` `inline`

```cpp
virtual inline MongooseHttpServerRequest * requestFactory(mg_connection * nc, HttpRequestMethodComposite method, mg_http_message * msg)
```

Defined in src/MongooseHttpServerEndpointUpload.h:25

##### Reimplements

- [`requestFactory`](#requestfactory)

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `MongooseHttpUploadHandler` | [`_upload`](#_upload)  |  |

---

{#_upload}

#### _upload

```cpp
MongooseHttpUploadHandler _upload
```

Defined in src/MongooseHttpServerEndpointUpload.h:22

{#mongoosehttpserverendpointwebsocket}

## MongooseHttpServerEndpointWebSocket

```cpp
#include <MongooseHttpServerEndpointWebSocket.h>
```

```cpp
class MongooseHttpServerEndpointWebSocket
```

Defined in src/MongooseHttpServerEndpointWebSocket.h:12

> **Inherits:** [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint)

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`MongooseHttpServerEndpointWebSocket`](#mongoosehttpserverendpointwebsocket-1) | `function` | Declared here |
| [`onConnect`](#onconnect-4) | `function` | Declared here |
| [`onFrame`](#onframe) | `function` | Declared here |
| [`requestFactory`](#requestfactory-2) | `function` | Declared here |
| [`handleConnect`](#handleconnect) | `function` | Declared here |
| [`handleFrame`](#handleframe) | `function` | Declared here |
| [`MongooseHttpWebSocketConnection`](#mongoosehttpwebsocketconnection-2) | `variable` | Declared here |
| [`_wsConnect`](#_wsconnect) | `variable` | Declared here |
| [`_wsFrame`](#_wsframe) | `variable` | Declared here |
| [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint-1) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`onRequest`](#onrequest-1) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`onClose`](#onclose-9) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`requestFactory`](#requestfactory) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`MongooseHttpServer`](#mongoosehttpserver-3) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-2) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`_method`](#_method-2) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`_uri`](#_uri-1) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`_request`](#_request) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`_close`](#_close) | `variable` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`willHandleRequest`](#willhandlerequest) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`handleRequest`](#handlerequest) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |
| [`handleClose`](#handleclose-1) | `function` | Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint) |

### Inherited from [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint)

| Kind | Name | Description |
|------|------|-------------|
| `function` | [`MongooseHttpServerEndpoint`](#mongoosehttpserverendpoint-1) `inline` |  |
| `function` | [`onRequest`](#onrequest-1) `inline` |  |
| `function` | [`onClose`](#onclose-9) `inline` |  |
| `function` | [`requestFactory`](#requestfactory) `virtual` `inline` |  |
| `variable` | [`MongooseHttpServer`](#mongoosehttpserver-3)  |  |
| `variable` | [`MongooseHttpServerRequest`](#mongoosehttpserverrequest-2)  |  |
| `variable` | [`MongooseHttpServerRequestUpload`](#mongoosehttpserverrequestupload)  |  |
| `variable` | [`_method`](#_method-2)  |  |
| `variable` | [`_uri`](#_uri-1)  |  |
| `variable` | [`_request`](#_request)  |  |
| `variable` | [`_close`](#_close)  |  |
| `function` | [`willHandleRequest`](#willhandlerequest)  |  |
| `function` | [`handleRequest`](#handlerequest)  |  |
| `function` | [`handleClose`](#handleclose-1)  |  |

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`MongooseHttpServerEndpointWebSocket`](#mongoosehttpserverendpointwebsocket-1) `inline` |  |
| `MongooseHttpServerEndpointWebSocket *` | [`onConnect`](#onconnect-4) `inline` |  |
| `MongooseHttpServerEndpointWebSocket *` | [`onFrame`](#onframe) `inline` |  |

---

{#mongoosehttpserverendpointwebsocket-1}

#### MongooseHttpServerEndpointWebSocket

`inline`

```cpp
inline MongooseHttpServerEndpointWebSocket(HttpRequestMethodComposite method, const char * uri = nullptr)
```

Defined in src/MongooseHttpServerEndpointWebSocket.h:38

---

{#onconnect-4}

#### onConnect

`inline`

```cpp
inline MongooseHttpServerEndpointWebSocket * onConnect(MongooseHttpWebSocketConnectionHandler handler)
```

Defined in src/MongooseHttpServerEndpointWebSocket.h:46

---

{#onframe}

#### onFrame

`inline`

```cpp
inline MongooseHttpServerEndpointWebSocket * onFrame(MongooseHttpWebSocketFrameHandler handler)
```

Defined in src/MongooseHttpServerEndpointWebSocket.h:51

### Protected Methods

| Return | Name | Description |
|--------|------|-------------|
| `MongooseHttpServerRequest *` | [`requestFactory`](#requestfactory-2) `virtual` `inline` |  |
| `void` | [`handleConnect`](#handleconnect) `inline` |  |
| `void` | [`handleFrame`](#handleframe) `inline` |  |

---

{#requestfactory-2}

#### requestFactory

`virtual` `inline`

```cpp
virtual inline MongooseHttpServerRequest * requestFactory(mg_connection * nc, HttpRequestMethodComposite method, mg_http_message * msg)
```

Defined in src/MongooseHttpServerEndpointWebSocket.h:21

##### Reimplements

- [`requestFactory`](#requestfactory)

---

{#handleconnect}

#### handleConnect

`inline`

```cpp
inline void handleConnect(MongooseHttpWebSocketConnection * connection)
```

Defined in src/MongooseHttpServerEndpointWebSocket.h:25

---

{#handleframe}

#### handleFrame

`inline`

```cpp
inline void handleFrame(MongooseHttpWebSocketConnection * connection, int flags, uint8_t * data, size_t len)
```

Defined in src/MongooseHttpServerEndpointWebSocket.h:31

### Private Attributes

| Return | Name | Description |
|--------|------|-------------|
| `friend` | [`MongooseHttpWebSocketConnection`](#mongoosehttpwebsocketconnection-2)  |  |
| `MongooseHttpWebSocketConnectionHandler` | [`_wsConnect`](#_wsconnect)  |  |
| `MongooseHttpWebSocketFrameHandler` | [`_wsFrame`](#_wsframe)  |  |

---

{#mongoosehttpwebsocketconnection-2}

#### MongooseHttpWebSocketConnection

```cpp
friend MongooseHttpWebSocketConnection
```

Defined in src/MongooseHttpServerEndpointWebSocket.h:14

---

{#_wsconnect}

#### _wsConnect

```cpp
MongooseHttpWebSocketConnectionHandler _wsConnect
```

Defined in src/MongooseHttpServerEndpointWebSocket.h:17

---

{#_wsframe}

#### _wsFrame

```cpp
MongooseHttpWebSocketFrameHandler _wsFrame
```

Defined in src/MongooseHttpServerEndpointWebSocket.h:18

{#mongoosemdnsrequest}

## MongooseMdnsRequest

```cpp
#include <MongooseMdns.h>
```

```cpp
struct MongooseMdnsRequest
```

Defined in src/MongooseMdns.h:25

Represents an incoming mDNS query, passed to the onRequest handler.

The handler may call [respond()](#respond) to send a reply using the registered service record for the matched service, or leave the request unhandled.

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`_req`](#_req) | `variable` | Declared here |
| [`name`](#name) | `function` | Declared here |
| [`isListing`](#islisting) | `function` | Declared here |
| [`respond`](#respond) | `function` | Declared here |

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `struct mg_mdns_req *` | [`_req`](#_req)  |  |

---

{#_req}

#### _req

```cpp
struct mg_mdns_req * _req
```

Defined in src/MongooseMdns.h:27

### Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `MongooseString` | [`name`](#name) `const` `inline` |  |
| `bool` | [`isListing`](#islisting) `const` `inline` |  |
| `void` | [`respond`](#respond) `inline` |  |

---

{#name}

#### name

`const` `inline`

```cpp
inline MongooseString name() const
```

Defined in src/MongooseMdns.h:29

---

{#islisting}

#### isListing

`const` `inline`

```cpp
inline bool isListing() const
```

Defined in src/MongooseMdns.h:33

---

{#respond}

#### respond

`inline`

```cpp
inline void respond(struct mg_dnssd_record * r = nullptr)
```

Defined in src/MongooseMdns.h:39

{#servicerecord}

## ServiceRecord

```cpp
#include <MongooseMdns.h>
```

```cpp
struct ServiceRecord
```

Defined in src/MongooseMdns.h:67

### List of all members

| Name | Kind | Owner |
|------|------|-------|
| [`srvcproto`](#srvcproto) | `variable` | Declared here |
| [`txt`](#txt) | `variable` | Declared here |
| [`port`](#port) | `variable` | Declared here |

### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `char` | [`srvcproto`](#srvcproto)  |  |
| `char` | [`txt`](#txt)  |  |
| `uint16_t` | [`port`](#port)  |  |

---

{#srvcproto}

#### srvcproto

```cpp
char srvcproto
```

Defined in src/MongooseMdns.h:68

---

{#txt}

#### txt

```cpp
char txt
```

Defined in src/MongooseMdns.h:69

---

{#port}

#### port

```cpp
uint16_t port
```

Defined in src/MongooseMdns.h:70

Generated by [Moxygen](https://0state.com/moxygen)