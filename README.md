# ArduinoMongoose

[![Build Status](https://travis-ci.org/jeremypoulter/ArduinoMongoose.svg?branch=master)](https://travis-ci.org/jeremypoulter/ArduinoMongoose)

A wrapper for Mongoose to help build into Arduino framework.

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
