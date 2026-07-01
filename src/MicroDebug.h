#ifndef MICRODEBUG_H
#define MICRODEBUG_H

#ifdef ENABLE_DEBUG

#ifdef ARDUINO

#ifndef DBUGF
#define DBUGF(...)        \
  do {                    \
    Serial.printf(__VA_ARGS__); \
    Serial.println();     \
  } while (0)
#endif

#ifndef DBUGLN
#define DBUGLN(msg) Serial.println(msg)
#endif

#ifndef DBUGVAR
#define DBUGVAR(value)       \
  do {                       \
    Serial.print(#value "="); \
    Serial.println(value);   \
  } while (0)
#endif

#else

#include <stdio.h>

#ifndef DBUGF
#define DBUGF(...)             \
  do {                         \
    fprintf(stderr, __VA_ARGS__); \
    fputc('\n', stderr);       \
  } while (0)
#endif

#ifndef DBUGLN
#define DBUGLN(msg)            \
  do {                         \
    fprintf(stderr, "%s\n", msg); \
  } while (0)
#endif

#ifndef DBUGVAR
#define DBUGVAR(value)                         \
  do {                                         \
    fprintf(stderr, "%s=%s\n", #value, value); \
  } while (0)
#endif

#endif

#else

#ifndef DBUGF
#define DBUGF(...) do { } while (0)
#endif

#ifndef DBUGLN
#define DBUGLN(...) do { } while (0)
#endif

#ifndef DBUGVAR
#define DBUGVAR(...) do { } while (0)
#endif

#endif

#endif
