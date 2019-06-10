#ifdef ARDUINO

#include <Arduino.h>
#include "mongoose.h"

/*
 * This is a callback invoked by Mongoose to signal that a poll is needed soon.
 * Since we're in a tight polling loop anyway (see below), we don't need to do
 * anything.
 */
void mg_lwip_mgr_schedule_poll(struct mg_mgr *mgr) {
}

#if MG_ENABLE_FILESYSTEM && defined(MG_USER_FILE_FUNCTIONS)

#include <FS.h>

int mg_stat(const char *path, cs_stat_t *st) {
  return -1;
}

MG_FILE mg_fopen(const char *path, const char *mode) {
  File file = SPIFFS.open(path, mode);
  return (MG_FILE)file;
}

void mg_close(MG_FILE f) {
  File file = (File)f;
  file.close();
}

int mg_open(const char *path, int flag, int mode) {
  return -1;
}

size_t mg_fread(void *ptr, size_t size, size_t count, MG_FILE f) {
  File file = (File)f;
  return file.read((uint8_t *)ptr, size * count);
}

size_t mg_fwrite(const void *ptr, size_t size, size_t count, MG_FILE f) {
  File file = (File)f;
  return file.write((uint8_t *)ptr, size * count);
}

#endif


#endif
