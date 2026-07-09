# ArduinoMongoose

[![Build Status](https://travis-ci.org/jeremypoulter/ArduinoMongoose.svg?branch=master)](https://travis-ci.org/jeremypoulter/ArduinoMongoose)

A wrapper for Mongoose to help build into Arduino framework.

## TLS entropy / RNG overrides

TLS uses `mg_random()` for cryptographic randomness. By default this uses the
best built-in source available for the current platform (for example
`/dev/urandom` on native builds and the ESP32 hardware RNG on ESP32 builds).

To provide a custom entropy source without forking the library, install a
callback before opening secure connections:

```cpp
static bool fillEntropy(void *buf, size_t len) {
  esp_fill_random(buf, len);
  return true;
}

void setup() {
  Mongoose.setRandomCallback(fillEntropy);
}
```

If you previously overrode `mg_ssl_if_mbed_random()` in older Mongoose-based
code, use this callback instead.

Pass `nullptr` to `Mongoose.setRandomCallback()` to restore the built-in
platform default.
