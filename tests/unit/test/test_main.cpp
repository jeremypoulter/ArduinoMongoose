#include <unity.h>

void runMongooseStringTests();
void runHttpMethodTests();
void runHttpServerTests();
void runHttpClientTests();
void runHttpMessageTests();
void runWebSocketTests();
void runMqttClientTests();
void runSntpClientTests();

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  runMongooseStringTests();
  runHttpMethodTests();
  runHttpServerTests();
  runHttpClientTests();
  runHttpMessageTests();
  runWebSocketTests();
  runMqttClientTests();
  runSntpClientTests();
  return UNITY_END();
}
