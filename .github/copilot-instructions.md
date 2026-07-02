# Copilot instructions for ArduinoMongoose

## Repository overview
- ArduinoMongoose is an Arduino-style C++ wrapper around the Mongoose networking library.
- Core library sources are under `src/`.
- Runnable PlatformIO examples are under `examples/<example>/`.
- CI workflows are in `.github/workflows/`.

## Working conventions for this repository
- Make focused, minimal changes and avoid unrelated refactors.
- Keep existing coding style and preprocessor patterns used in nearby files.
- Do not modify vendored third-party code in `src/mongoose.c` or `src/mongoose.h` unless the task explicitly requires it.
- Prefer changing wrapper code in `src/Mongoose*.{h,cpp}` and validating behavior through relevant examples.

## Build and validation commands
- Install PlatformIO when needed:
  - `pip install -U platformio`
  - `platformio update`
- Build an example:
  - `cd examples/<example>`
  - `pio run -e <env>`
- Common environments in this repo include `native`, `esp-wrover-kit`, and `esp-wrover-kit-secure`.
- For quick validation, build the smallest set of examples affected by your change, and include at least one `native` build when possible.

## Testing guidance
- There is no top-level unit test suite currently configured in this repository.
- Validation is primarily done by compiling relevant examples with PlatformIO.
- If a change affects HTTP server/client, MQTT, SNTP, or websocket behavior, build at least one representative example from that area.

## PR and change reporting expectations
- Summarize what changed, which examples/environments were built, and any limitations.
- If errors occur, document:
  - the exact error,
  - the command that produced it,
  - and the workaround or resolution attempted.
- If an error cannot be resolved in-session, clearly call it out in the final report.
