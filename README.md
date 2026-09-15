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

## TLS entropy / RNG behaviour

TLS uses Mongoose's built-in `mg_random()` implementation for cryptographic
randomness. On supported targets it uses the strongest platform source Mongoose
has available, such as `/dev/urandom` on native builds and the ESP32 hardware
RNG on ESP32 builds.

If Mongoose cannot access a strong platform RNG on a target, `mg_random()` logs
"Weak RNG: using rand()", fills the buffer using `rand()`, and returns `false`.
Mongoose TLS treats that as a fatal RNG error and will close the connection, so
production TLS deployments must use a platform with a strong built-in RNG or
provide a custom `mg_random()` implementation via `MG_ENABLE_CUSTOM_RANDOM=1`.

ArduinoMongoose does not add a wrapper-specific runtime RNG override API, which
keeps future Mongoose updates straightforward without patching vendored
`mongoose.c` / `mongoose.h`.

## mDNS and DNS-SD

`Mdns` owns one Mongoose listener for both advertising and `.local` client
resolution. Start it **after the network interface is up**, and call `begin()`
again and re-register services after an interface/IP change. `end()` cancels
pending lookups and clears the address cache. Do not run a platform mDNS
responder on the same interface alongside it (disable ArduinoOTA's built-in
mDNS with `ArduinoOTA.setMdnsEnabled(false)` when using that library).

```cpp
Mdns.begin("mydevice");
Mdns.addService("_http._tcp", 80);
Mdns.addServiceTxt("_http._tcp", "version", "1");
Mdns.browse("_http._tcp");
// Continue Mongoose.poll() normally. After your application's browse window:
auto peers = Mdns.services();
Mdns.cancelBrowse();
```

Browsing assembles PTR, SRV, TXT and A/AAAA records across packets, including
additional records, and requests missing records asynchronously. Results carry
the service instance and SRV hostname separately. Incomplete instances can
appear before their target/port/addresses arrive. Record TTLs and goodbyes are
honoured; storage is capped at 80 records and 1024 bytes per received TXT RR.

The IPv4 `.local` resolver coalesces concurrent queries and caches eight host
addresses (names up to 63 bytes) using received TTLs. Cold lookups use the
manager's configured DNS timeout. URLs remain intact, preserving HTTP Host and
TLS server names. `addService()`'s optional TXT argument is a single **text**
string; use `addServiceTxt()` for multiple keys. `ServiceRecord::txt` now holds
wire-format length-prefixed strings; use `txtLength`, not `strlen()`.

See [`examples/mdns_discovery`](examples/mdns_discovery) for a native example.

Run the packet and lifecycle regressions with `pio test -e native_mdns_asan`
in `tests/unit` to enable address, undefined-behaviour and leak sanitizers for
the mDNS suite, or `pio test -e native` for all library unit tests.

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
