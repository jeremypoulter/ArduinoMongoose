#include <unity.h>

#include <MongooseCore.h>
#include <MongooseHttpClient.h>
#include <MongooseHttpServer.h>

#include "test_support.h"

#ifndef ARDUINO
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <string>

static void test_post_starts_http_request_on_polling_thread()
{
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18120));
  server.on("/config", HTTP_GET, [](MongooseHttpServerRequest *request) {
    request->send(200, "text/plain", "ready");
  });

  MongooseHttpClient client;
  const std::thread::id pollThread = std::this_thread::get_id();
  std::atomic<bool> accepted(false);
  std::atomic<bool> requested(false);
  std::atomic<bool> responded(false);
  bool postedOnPollThread = false;
  int status = 0;
  std::string body;

  std::thread producer([&]() {
    accepted = Mongoose.post([&]() {
      postedOnPollThread = std::this_thread::get_id() == pollThread;
      Mongoose.setNameserver("udp://127.0.0.1:53");
      requested = client.get("http://127.0.0.1:18120/config",
          [&](MongooseHttpClientResponse *response) {
            status = response->respCode();
            body.assign(response->body().c_str(), response->body().length());
            responded = true;
          });
    });
  });
  producer.join();

  TEST_ASSERT_TRUE(accepted.load());
  TEST_ASSERT_TRUE(pumpUntil([&]() { return responded.load(); }));
  TEST_ASSERT_TRUE(postedOnPollThread);
  TEST_ASSERT_TRUE(requested.load());
  TEST_ASSERT_EQUAL(200, status);
  TEST_ASSERT_EQUAL_STRING("ready", body.c_str());
  TEST_ASSERT_EQUAL_STRING("udp://127.0.0.1:53", Mongoose.nameserver());
  Mongoose.setNameservers(nullptr, 0);
}

static void test_wrapper_call_waits_for_active_poll_callback()
{
  ScopedMongoose mongoose;
  std::mutex mutex;
  std::condition_variable changed;
  bool callbackEntered = false;
  bool releaseCallback = false;
  bool queued = Mongoose.post([&]() {
    Mongoose.setNameserver("udp://127.0.0.1:53");
    std::unique_lock<std::mutex> lock(mutex);
    callbackEntered = true;
    changed.notify_all();
    changed.wait(lock, [&]() { return releaseCallback; });
  });

  if(!queued) {
    TEST_ASSERT_TRUE(queued);
    return;
  }

  std::thread poller([&]() { Mongoose.poll(1000); });
  bool entered = false;
  {
    std::unique_lock<std::mutex> lock(mutex);
    entered = changed.wait_for(lock, std::chrono::seconds(2),
                               [&]() { return callbackEntered; });
  }

  std::atomic<bool> workerStarted(false);
  std::atomic<bool> workerFinished(false);
  std::thread worker;
  if(entered) {
    worker = std::thread([&]() {
      workerStarted = true;
      Mongoose.setNameserver("udp://127.0.0.2:53");
      workerFinished = true;
      changed.notify_all();
    });
    while(!workerStarted.load()) {
      std::this_thread::yield();
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  const bool blockedByPoll = entered && !workerFinished.load();
  {
    std::lock_guard<std::mutex> lock(mutex);
    releaseCallback = true;
  }
  changed.notify_all();
  poller.join();
  if(worker.joinable()) {
    worker.join();
  }

  TEST_ASSERT_TRUE(entered);
  TEST_ASSERT_TRUE(blockedByPoll);
  TEST_ASSERT_TRUE(workerFinished.load());
  TEST_ASSERT_EQUAL_STRING("udp://127.0.0.2:53", Mongoose.nameserver());
  Mongoose.setNameservers(nullptr, 0);
}
#endif

void runThreadingTests()
{
#ifndef ARDUINO
  RUN_TEST(test_post_starts_http_request_on_polling_thread);
  RUN_TEST(test_wrapper_call_waits_for_active_poll_callback);
#endif
}
