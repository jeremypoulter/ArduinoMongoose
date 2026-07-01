#include <unity.h>

void runMongooseStringTests();
void runHttpMethodTests();
void runHttpServerTests();
void runHttpClientTests();
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
  runWebSocketTests();
  runMqttClientTests();
  runSntpClientTests();
  return UNITY_END();
}
