#include <unity.h>

#include <MongooseCore.h>

#include <cstring>

namespace {
bool fillWithPattern(void *buf, size_t len) {
  memset(buf, 0xA5, len);
  return true;
}

bool failWithZeros(void *buf, size_t len) {
  memset(buf, 0, len);
  return false;
}
}  // namespace

static void test_random_default_source_returns_success() {
  unsigned char buf[16] = {0};

  Mongoose.setRandomCallback(nullptr);

  TEST_ASSERT_TRUE(Mongoose.fillRandom(buf, sizeof(buf)));
}

static void test_random_callback_override_is_used() {
  unsigned char buf[8] = {0};

  Mongoose.setRandomCallback(fillWithPattern);

  TEST_ASSERT_EQUAL_PTR(fillWithPattern, Mongoose.getRandomCallback());
  TEST_ASSERT_TRUE(Mongoose.fillRandom(buf, sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); ++i) {
    TEST_ASSERT_EQUAL_HEX8(0xA5, buf[i]);
  }

  Mongoose.setRandomCallback(nullptr);
}

static void test_random_callback_failure_is_propagated() {
  unsigned char buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};

  Mongoose.setRandomCallback(failWithZeros);

  TEST_ASSERT_FALSE(Mongoose.fillRandom(buf, sizeof(buf)));
  for (size_t i = 0; i < sizeof(buf); ++i) {
    TEST_ASSERT_EQUAL_HEX8(0x00, buf[i]);
  }

  Mongoose.setRandomCallback(nullptr);
}

void runRandomTests() {
  RUN_TEST(test_random_default_source_returns_success);
  RUN_TEST(test_random_callback_override_is_used);
  RUN_TEST(test_random_callback_failure_is_propagated);
}
