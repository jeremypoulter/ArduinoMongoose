#include <unity.h>
#if defined(EPOXY_DUINO)
#include <stdlib.h>
#endif

void runMongooseStringTests();
void runHttpMethodTests();
void runHttpServerTests();
void runHttpClientTests();
void runHttpMessageTests();
void runWebSocketTests();
void runMqttClientTests();
void runSntpClientTests();
void runMdnsTests();

static int runAllTests() {
  UNITY_BEGIN();
  runMongooseStringTests();
  runHttpMethodTests();
  runHttpServerTests();
  runHttpClientTests();
  runHttpMessageTests();
  runWebSocketTests();
  runMqttClientTests();
  runSntpClientTests();
  runMdnsTests();
  return UNITY_END();
}

void setUp(void) {}
void tearDown(void) {}

// Unity's own convention -- UNITY_END() returns the number of failed
// assertions as the process exit code -- collides with PlatformIO's native
// test runner. PlatformIO's asyncio-based reader (test/runners/readers/
// native.py, raise_for_status()) does `signal.Signals(abs(return_code))`
// unconditionally on any nonzero exit, without checking whether the process
// actually died from a signal (a negative return code, on POSIX) or simply
// returned a small positive number. So a completely ordinary run with
// exactly 2 failed assertions -- no crash anywhere -- gets reported as
// "Program received signal SIGINT (Interrupt)" (2 is SIGINT's number), 1
// failure as SIGHUP, 6 as SIGABRT, and so on for every count up to 31ish.
//
// Confirmed by direct reproduction rather than inference: running the built
// binary through `pio test` with a small, fixed number of genuine assertion
// failures (a rejected MQTT auth, unrelated to this file) reliably reports
// "ERRORED"/a fabricated "Program received signal SIG... (...)" -- with the
// process's own Unity summary line, printed immediately above it in the same
// run, correctly describing only ordinary failures and nothing else.
// Repeated runs of that exact scenario: same false "signal" report every
// time, zero actual crashes (confirmed under AddressSanitizer, Valgrind, and
// gdb backtraces on every run).
//
// A real crash still reports correctly: POSIX gives a *negative* return
// code for a signal death (Python's subprocess/asyncio follow this too), so
// abs() recovers the real signal number in that case. Only the "ordinary
// positive failure count" case is the false positive, and it depends only
// on how many assertions failed, not on anything this project controls at
// the call site -- so the fix has to be here, collapsing any nonzero result
// to a single sentinel outside the entire signal range (Linux tops out at
// 64 with real-time signals) rather than passing the raw count through.
static int toExitCode(int unityResult) {
  return 0 == unityResult ? 0 : 100;
}

#if defined(ARDUINO)
#if defined(EPOXY_DUINO)
extern "C" {
#endif

void setup() {
  int result = runAllTests();
#if defined(EPOXY_DUINO)
  exit(toExitCode(result));
#endif
}

void loop() {}

#if defined(EPOXY_DUINO)
}
#endif
#else
int main(int argc, char **argv) {
  (void) argc;
  (void) argv;
  return toExitCode(runAllTests());
}
#endif
