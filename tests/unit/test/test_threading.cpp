#include <unity.h>

#include <MongooseCore.h>

#include "test_support.h"

#ifndef ARDUINO
#include <atomic>
#include <thread>

static void test_post_runs_on_polling_thread_and_allows_reentry()
{
  ScopedMongoose mongoose;
  const std::thread::id pollThread = std::this_thread::get_id();
  std::atomic<bool> ran(false);
  std::atomic<bool> accepted(false);
  bool onPollThread = false;

  std::thread producer([&]() {
    accepted = Mongoose.post([&]() {
      onPollThread = std::this_thread::get_id() == pollThread;
      Mongoose.setNameserver("udp://127.0.0.1:53");
      ran = true;
    });
  });
  producer.join();

  TEST_ASSERT_TRUE(accepted.load());
  TEST_ASSERT_TRUE(pumpUntil([&]() { return ran.load(); }));
  TEST_ASSERT_TRUE(onPollThread);
  TEST_ASSERT_EQUAL_STRING("udp://127.0.0.1:53", Mongoose.nameserver());
  Mongoose.setNameservers(nullptr, 0);
}
#endif

void runThreadingTests()
{
#ifndef ARDUINO
  RUN_TEST(test_post_runs_on_polling_thread_and_allows_reentry);
#endif
}
