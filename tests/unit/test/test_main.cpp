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

#if defined(ARDUINO)
#if defined(EPOXY_DUINO)
extern "C" {
#endif

void setup() {
  int result = runAllTests();
#if defined(EPOXY_DUINO)
  exit(result == 0 ? 0 : 1);
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
  return runAllTests();
}
#endif
