# ArduinoMongoose

[![Build Status](https://travis-ci.org/jeremypoulter/ArduinoMongoose.svg?branch=master)](https://travis-ci.org/jeremypoulter/ArduinoMongoose)

A wrapper for the [Mongoose](https://mongoose.ws/) networking library to help integrate it into the Arduino framework (specifically ESP32 and ESP8266) as well as native platforms using PlatformIO.

Mongoose provides an event-driven, non-blocking networking core. `ArduinoMongoose` provides easy-to-use C++ wrappers for:
- HTTP Servers
- HTTP Clients
- MQTT Clients
- WebSockets
- SNTP
- mDNS

## Upgrade Notes

### File Upload Behaviour (Mongoose 7)

Mongoose 7 changed how HTTP uploads are handled. If you are upgrading from an earlier version or porting existing upload code, note the following:

**Large file / OTA uploads (recommended: `application/octet-stream`)**

For large payloads such as OTA firmware images, POST the file as a raw binary body with `Content-Type: application/octet-stream` (or any type without a `boundary` parameter). The library will stream the body incrementally to your `onUpload` handler via `MG_EV_HTTP_PART_BEGIN` / `MG_EV_HTTP_PART_DATA` / `MG_EV_HTTP_PART_END` events, keeping peak heap usage proportional to the chunk size rather than the total file size.

**`multipart/form-data` uploads**

`multipart/form-data` requests are still supported and the `onUpload` callback API is unchanged. However, Mongoose 7 buffers the entire request body before firing the message event, so `multipart/form-data` is **not suitable for large files** on memory-constrained devices (e.g. ESP32 with ~300 KB free heap). Use `application/octet-stream` for firmware-sized payloads.

**Client-side change**

If your upload page previously used an HTML `<form enctype="multipart/form-data">` or set `contentType: false` in an Ajax call, switch to posting the raw `File` object with an explicit content type:

```js
$.ajax({
  url: '/update',
  type: 'POST',
  data: file,
  contentType: file.type || 'application/octet-stream',
  processData: false
});
```

**Server-side API is unchanged** — the `onRequest` / `onUpload` / `onClose` handler registration and all `MG_EV_HTTP_PART_*` event constants remain the same.

## Documentation

- [API Reference](docs/API_REFERENCE.md)
- [AI Agent Instructions](llms.txt)

## Quick Start

### Basic Setup

To use the library, you need to initialize the core and poll it continuously in your loop.

```cpp
#include <Arduino.h>
#include <MongooseCore.h>

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi (ESP32 / ESP8266)
  // ...

  // Initialize Mongoose
  Mongoose.begin();
}

void loop() {
  // Poll Mongoose (parameter is timeout in ms)
  Mongoose.poll(1000);
}
```

### Simple HTTP Server

```cpp
#include <MongooseHttpServer.h>

MongooseHttpServer server;

void setup() {
  Mongoose.begin();
  server.begin(80);
  
  server.on("/", HTTP_GET, [](MongooseHttpServerRequest *request) {
    request->send(200, "text/plain", "Hello from ArduinoMongoose!");
  });
}

void loop() {
  Mongoose.poll(1000);
}
```

For more examples, check out the `examples/` directory!
