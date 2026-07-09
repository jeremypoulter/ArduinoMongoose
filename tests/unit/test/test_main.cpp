#include <unity.h>

void runMongooseStringTests();
void runHttpMethodTests();
void runRandomTests();
void runHttpServerTests();
void runHttpClientTests();
void runHttpMessageTests();
void runWebSocketTests();
void runMqttClientTests();
void runSntpClientTests();
void runMdnsTests();

void setUp(void) {}
void tearDown(void) {}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  runMongooseStringTests();
  runHttpMethodTests();
  runRandomTests();
  runHttpServerTests();
  runHttpClientTests();
  runHttpMessageTests();
  runWebSocketTests();
  runMqttClientTests();
  runSntpClientTests();
  runMdnsTests();
  return UNITY_END();
}
