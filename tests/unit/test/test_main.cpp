#include <unity.h>

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
void setup() {
  (void) runAllTests();
}

void loop() {}
#else
int main(int argc, char **argv) {
  (void) argc;
  (void) argv;
  return runAllTests();
}
#endif
