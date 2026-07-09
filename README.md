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