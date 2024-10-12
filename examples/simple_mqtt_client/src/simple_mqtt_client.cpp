//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <MicroDebug.h>
#include <MongooseCore.h>
#include <MongooseMqttClient.h>

#ifdef ESP32
#include <WiFi.h>
#define START_ESP_WIFI
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#define START_ESP_WIFI
#endif
#include <string>

#if MG_ENABLE_SSL
// SHA-1 hash, not supported
#define MQTT_HOST "test.mosquitto.org:8883"
#define MQTT_PROTOCOL MQTT_MQTTS
#else
#define MQTT_HOST "test.mosquitto.org:1883"
#define MQTT_PROTOCOL MQTT_MQTT
#endif

#ifndef LOGF
#ifdef ARDUINO
#define LOGF Serial.printf
#else
#define LOGF printf
#endif
#endif

MongooseMqttClient client;
char clientId[16];

#ifdef START_ESP_WIFI
const char *ssid = "wifi";
const char *password = "password";
#endif

MongooseMqttProtocol mqtt_protocol = MQTT_PROTOCOL;
const char *mqtt_host = MQTT_HOST;

#if MG_ENABLE_SSL
// Root CA bundle
const char *root_ca =
// test.mosquitto.org Root CAs
"-----BEGIN CERTIFICATE-----\r\n"
"MIIC8DCCAlmgAwIBAgIJAOD63PlXjJi8MA0GCSqGSIb3DQEBBQUAMIGQMQswCQYD\r\n"
"VQQGEwJHQjEXMBUGA1UECAwOVW5pdGVkIEtpbmdkb20xDjAMBgNVBAcMBURlcmJ5\r\n"
"MRIwEAYDVQQKDAlNb3NxdWl0dG8xCzAJBgNVBAsMAkNBMRYwFAYDVQQDDA1tb3Nx\r\n"
"dWl0dG8ub3JnMR8wHQYJKoZIhvcNAQkBFhByb2dlckBhdGNob28ub3JnMB4XDTEy\r\n"
"MDYyOTIyMTE1OVoXDTIyMDYyNzIyMTE1OVowgZAxCzAJBgNVBAYTAkdCMRcwFQYD\r\n"
"VQQIDA5Vbml0ZWQgS2luZ2RvbTEOMAwGA1UEBwwFRGVyYnkxEjAQBgNVBAoMCU1v\r\n"
"c3F1aXR0bzELMAkGA1UECwwCQ0ExFjAUBgNVBAMMDW1vc3F1aXR0by5vcmcxHzAd\r\n"
"BgkqhkiG9w0BCQEWEHJvZ2VyQGF0Y2hvby5vcmcwgZ8wDQYJKoZIhvcNAQEBBQAD\r\n"
"gY0AMIGJAoGBAMYkLmX7SqOT/jJCZoQ1NWdCrr/pq47m3xxyXcI+FLEmwbE3R9vM\r\n"
"rE6sRbP2S89pfrCt7iuITXPKycpUcIU0mtcT1OqxGBV2lb6RaOT2gC5pxyGaFJ+h\r\n"
"A+GIbdYKO3JprPxSBoRponZJvDGEZuM3N7p3S/lRoi7G5wG5mvUmaE5RAgMBAAGj\r\n"
"UDBOMB0GA1UdDgQWBBTad2QneVztIPQzRRGj6ZHKqJTv5jAfBgNVHSMEGDAWgBTa\r\n"
"d2QneVztIPQzRRGj6ZHKqJTv5jAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBBQUA\r\n"
"A4GBAAqw1rK4NlRUCUBLhEFUQasjP7xfFqlVbE2cRy0Rs4o3KS0JwzQVBwG85xge\r\n"
"REyPOFdGdhBY2P1FNRy0MDr6xr+D2ZOwxs63dG1nnAnWZg7qwoLgpZ4fESPD3PkA\r\n"
"1ZgKJc2zbSQ9fCPxt2W3mdVav66c6fsb7els2W2Iz7gERJSX\r\n"
"-----END CERTIFICATE-----\r\n";
#endif

#ifndef ARDUINO
struct timespec millis_start_timestamp;

unsigned long millis(void) 
{
  struct timespec timenow, start, end;
  clock_gettime(CLOCK_REALTIME, &timenow);
  start = millis_start_timestamp;
  end = timenow;
  // timeDiffmillis:
  return ((end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) * 1e-6);
}
#endif

void setup()
{
  uint64_t deviceId = 0;

#ifdef ARDUINO
  Serial.begin(115200);

#ifdef START_ESP_WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname: ");
#ifdef ESP32
  Serial.println(WiFi.getHostname());
  deviceId = ESP.getEfuseMac();
#elif defined(ESP8266)
  Serial.println(WiFi.hostname());
  deviceId = ESP.getChipId();
#endif
#endif // START_ESP_WIFI
#else
  clock_gettime(CLOCK_REALTIME, &millis_start_timestamp);
  deviceId = 0x12345678;
#endif // ARDUINO

  Mongoose.begin();

#if MG_ENABLE_SSL
  Mongoose.setRootCa(root_ca);
#endif

  snprintf(clientId, sizeof(clientId), "mg-%" PRIx64, deviceId);
  client.onMessage([](MongooseString topic, MongooseString payload)
  {
    DBUGF("%.*s: %.*s", (int)topic.length(), (const char *)topic, (int)payload.length(), (const char *)payload);
    client.publish("am/echo", payload);
  });
  client.onError([](const char *err) {
    DBUGF("Got error %s", err);
  });

  client.setLastWillAndTestimment("am/lastwill", "I'm gone", true);

  DBUGF("Trying to connect to %s", mqtt_host);
  client.connect(mqtt_protocol, mqtt_host, clientId, []()
  {
    DBUGF("Connected, subscribing to am/+");
    client.subscribe("am/clock");
    client.subscribe("am/test/+");
  });
}

static unsigned long next_time = 0;
void loop()
{
  Mongoose.poll(next_time - millis());

  unsigned long now = millis();
  if(client.connected() && now >= next_time)
  {
    char time[16];
    snprintf(time, sizeof(time), "%lu", now);
    client.publish("am/clock", time);
    next_time += 1000;
    DBUGVAR(next_time);
  }
}

#ifndef ARDUINO
void usage(const char *name)
{
  fprintf(stderr, "Usage: %s [<protocol>://<host>:<port>]\n", name);
}

int main(int argc, char *argv[])
{
  int i;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0) {
      usage(argv[0]);
      exit(EXIT_SUCCESS);
    } else {
      break;
    }
  }

  if (i + 1 == argc)
  {
    if(0 == strncmp(argv[i], "mqtt://", 6))
    {
      mqtt_protocol = MQTT_MQTT;
      mqtt_host = argv[i] + 6;
    }
#if MG_ENABLE_SSL
    else if(0 == strncmp(argv[i], "mqtts://", 7))
    {
      mqtt_protocol = MQTT_MQTTS;
      mqtt_host = argv[i] + 7;
    }
#endif
    else
    {
      usage(argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  setup();
  while(true) {
    loop();
  }
}
#endif
