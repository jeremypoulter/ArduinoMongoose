#ifndef ARDUINO_MONGOOSE_TEST_SUPPORT_H
#define ARDUINO_MONGOOSE_TEST_SUPPORT_H

#include <MongooseCore.h>
#include <functional>

struct ScopedMongoose {
  ScopedMongoose() {
    Mongoose.begin();
  }

  ~ScopedMongoose() {
    Mongoose.end();
  }
};

inline bool pumpUntil(const std::function<bool()> &predicate,
                      int timeoutMs = 2000,
                      const std::function<void()> &step = std::function<void()>()) {
  uint64_t deadline = mg_millis() + timeoutMs;
  while (mg_millis() < deadline) {
    Mongoose.poll(10);
    if (step) {
      step();
    }
    if (predicate()) {
      return true;
    }
  }
  return predicate();
}

inline void pumpFor(int durationMs,
                    const std::function<void()> &step = std::function<void()>()) {
  uint64_t deadline = mg_millis() + durationMs;
  while (mg_millis() < deadline) {
    Mongoose.poll(10);
    if (step) {
      step();
    }
  }
}

#endif
