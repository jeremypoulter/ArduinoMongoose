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
#include <MongooseSntpClient.h>

#ifdef ESP32
#include <WiFi.h>
#define START_ESP_WIFI
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#define START_ESP_WIFI
#endif

#ifndef SNTP_HOST
#define SNTP_HOST "pool.ntp.org"
//#define SNTP_HOST "time.google.com"
#endif

#ifndef LOGF
#ifdef ARDUINO
#define LOGF Serial.printf
#else
#define LOGF printf
#endif
#endif


MongooseSntpClient sntp;

#ifdef START_ESP_WIFI
const char *ssid = "wifi";
const char *password = "password";
#endif

const char *sntp_host = SNTP_HOST;

int run = 1;

static unsigned long next_time = 0;
static bool fetching_time = false;

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

double diff_time(timeval tv1, timeval tv2)
{
    double t1 = (double) tv1.tv_sec + (((double) tv1.tv_usec) / 1000000.0);
    double t2 = (double) tv2.tv_sec + (((double) tv2.tv_usec) / 1000000.0);

    return t1-t2;
}

void setup()
{
#ifdef ARDUINO

  Serial.begin(115200);

#ifdef START_ESP_WIFI
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.printf("\n\nConnecting to %s - %s\n", ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed! %d\n", WiFi.status());
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname: ");
#ifdef ESP32
  Serial.println(WiFi.getHostname());
#elif defined(ESP8266)
  Serial.println(WiFi.hostname());
#endif
#endif
#else
  clock_gettime(CLOCK_REALTIME, &millis_start_timestamp);
#endif

  Mongoose.begin();

  sntp.onError([](uint8_t err) {
    DBUGF("Got error %u", err);
    fetching_time = false;
    next_time = millis() + 10 * 1000;
  });
  
  next_time = millis();
}

void loop()
{
  Mongoose.poll(fetching_time ? 1000 : next_time - millis());

  if(false == fetching_time && millis() >= next_time)
  {
    fetching_time = true;

    DBUGF("Trying to get time from %s", sntp_host);
    sntp.getTime(sntp_host, [](struct timeval server_time)
    {
      struct timeval local_time;
      gettimeofday(&local_time, NULL);
      LOGF("Local time: %s", ctime(&local_time.tv_sec));
      LOGF("Time from %s: %s", sntp_host, ctime(&server_time.tv_sec));
      LOGF("Diff %.2f\n", diff_time(server_time, local_time));
      settimeofday(&server_time, NULL);
      
      fetching_time = false;
      next_time = millis() + 10 * 1000;
      run--;
    });
  }
}


#ifndef ARDUINO
int main(int argc, char *argv[])
{
  int i;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0) {
      fprintf(stderr, "Usage: %s [host]\n", argv[0]);
      exit(EXIT_SUCCESS);
    } else {
      break;
    }
  }

  if (i + 1 == argc) {
    sntp_host = argv[i];
  }

  setup();
  while(run) {
    loop();
  }
}
#endif
