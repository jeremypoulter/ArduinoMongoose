#ifndef MongoosePlatform_h
#define MongoosePlatform_h

#ifdef ARDUINO
#include "Arduino.h"
#include <IPAddress.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#endif // ARDUINO

#endif // MongoosePlatform_h
