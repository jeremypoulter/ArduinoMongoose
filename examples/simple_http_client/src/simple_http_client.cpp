//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <MongooseCore.h>
#include <MongooseHttpClient.h>

#ifdef ESP32
#include <WiFi.h>
#define START_ESP_WIFI
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#define START_ESP_WIFI
#endif

#if MG_TLS != MG_TLS_NONE
#define PROTO "https"
#else
#define PROTO "http"
#endif

#ifndef LOGF
#ifdef ARDUINO
#define LOGF Serial.printf
#else
#define LOGF printf
#endif
#endif

MongooseHttpClient client;

#ifdef START_ESP_WIFI
const char *ssid = "wifi";
const char *password = "password";
#endif

const char *url = nullptr;

int run = 0;
bool s_show_headers = false;

extern const char *root_ca;

void printResponse(MongooseHttpClientResponse *response)
{
  LOGF("%d %.*s\n", response->respCode(), (int)response->respStatusMsg().length(), (const char *)response->respStatusMsg());
  int headers = response->headers();
  int i;
  for(i=0; i<headers; i++) {
    LOGF("_HEADER[%.*s]: %.*s\n", 
      (int)response->headerNames(i).length(), (const char *)response->headerNames(i), 
      (int)response->headerValues(i).length(), (const char *)response->headerValues(i));
  }

  LOGF("\n%.*s\n", (int)response->body().length(), (const char *)response->body());
}

void connectionClosed()
{
  LOGF("Connection closed (%d)\n", run);
  run--;
}

void setup()
{
#ifdef ARDUINO
  Serial.begin(115200);

#ifdef START_ESP_WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    LOGF("WiFi Failed!\n");
    return;
  }

  Serial.printf("IP Address: %s");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname: ");
#ifdef ESP32
  Serial.println(WiFi.getHostname());
#elif defined(ESP8266)
  Serial.println(WiFi.hostname());
#endif
#endif
#endif

  Mongoose.begin();
  Mongoose.setRootCa(root_ca);

  // Based on https://github.com/typicode/jsonplaceholder#how-to
  run++;
  client.get(url != nullptr ? url : PROTO"://jsonplaceholder.typicode.com/posts/1", printResponse, connectionClosed);

  if(url != nullptr) { 
    return;
  }

  run++;
  client.post(PROTO"://jsonplaceholder.typicode.com/posts", "application/json; charset=UTF-8",
    "{\"title\":\"foo\",\"body\":\"bar\",\"userId\":1}",
    printResponse, connectionClosed);

  run++;
  client.put(PROTO"://jsonplaceholder.typicode.com/posts/1", "application/json; charset=UTF-8",
    "{\"id\":1,\"title\":\"foo\",\"body\":\"bar\",\"userId\":1}",
    printResponse, connectionClosed);

//  client.patch(PROTO"://jsonplaceholder.typicode.com/posts/1", "application/json; charset=UTF-8",
//    "{\"title\":\"foo\"}",
//    [](MongooseHttpClientResponse *response)
//  {
//    printResponse(response);
//  });

//  client.delete(PROTO"://jsonplaceholder.typicode.com/posts/1", [](MongooseHttpClientResponse *response) {
//    printResponse(response);
//  });

//  MongooseHttpClientRequest *request = client.beginRequest(PROTO"://jsonplaceholder.typicode.com/posts");
//  request->setMethod(HTTP_GET);
//  request->addHeader("X-hello", "world");
//  request->onBody([](const uint8_t *data, size_t len) {
//    LOGF("%.*s", len, (const char *)data));
//  };
//  request->onResponse([](MongooseHttpClientResponse *response) {
//    printResponse(response);
//  });
//  client.send(request);
}

void loop()
{
  Mongoose.poll(1000);
}

#ifndef ARDUINO
int main(int argc, char *argv[])
{
  int i;

  const char *log_level = getenv("LOG_LEVEL");  // Allow user to set log level
  if (log_level == NULL) log_level = "1";       // Default is error
  mg_log_set(atoi(log_level));    // Set to 0 to disable debug

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--show-headers") == 0) {
      s_show_headers = true;
    } else if (strcmp(argv[i], "--help") == 0) {
      fprintf(stderr, "Usage: %s [--show-headers] <URL>\n", argv[0]);
      exit(EXIT_SUCCESS);
    } else {
      break;
    }
  }

  if (i + 1 == argc) {
    url = argv[i];
  }

  setup();
  while(run ) {
    loop();
  }
}
#endif
