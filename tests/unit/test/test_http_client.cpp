#include <unity.h>

#include <MongooseHttpClient.h>
#include <MongooseHttpServer.h>

#include <string>

#include "test_support.h"

namespace {
struct ClientCapture {
  bool responded = false;
  bool bodyCalled = false;
  bool closed = false;
  int code = 0;
  std::string body;
  std::string replyHeader;
};
}  // namespace

static void test_http_client_get_exposes_status_body_headers_and_onbody() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18082));

  server.on("/inspect", HTTP_GET, [](MongooseHttpServerRequest *request) {
    MongooseHttpServerResponseBasic *response = request->beginResponse();
    response->setCode(200);
    response->setContentType("text/plain");
    response->addHeader("X-Reply", "ready");
    response->setContent("GET response body");
    request->send(response);
  });

  MongooseHttpClient client;
  ClientCapture capture;
  MongooseHttpClientRequest *request = client.beginRequest("http://127.0.0.1:18082/inspect");
  request->onBody([&capture](MongooseHttpClientResponse *response) {
    capture.bodyCalled = true;
    capture.body.assign(response->body().c_str(), response->body().length());
  })->onResponse([&capture](MongooseHttpClientResponse *response) {
    capture.responded = true;
    capture.code = response->respCode();
    capture.replyHeader.assign(response->headers("X-Reply").c_str(),
                               response->headers("X-Reply").length());
  })->onClose([&capture]() { capture.closed = true; });

  TEST_ASSERT_TRUE(request->send());
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&capture]() { return capture.closed; }), "GET client request timed out");

  TEST_ASSERT_TRUE(capture.bodyCalled);
  TEST_ASSERT_TRUE(capture.responded);
  TEST_ASSERT_EQUAL(200, capture.code);
  TEST_ASSERT_EQUAL_STRING("GET response body", capture.body.c_str());
  TEST_ASSERT_EQUAL_STRING("ready", capture.replyHeader.c_str());
}

static void test_http_client_post_and_put_round_trip() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18083));

  server.on("/echo", HTTP_POST | HTTP_PUT, [](MongooseHttpServerRequest *request) {
    std::string method(request->methodStr().c_str(), request->methodStr().length());
    std::string body(request->body().c_str(), request->body().length());
    std::string reply = method + ":" + body;

    MongooseHttpServerResponseBasic *response = request->beginResponse();
    response->setCode(200);
    response->setContentType("text/plain");
    response->addHeader("X-Method", method.c_str());
    response->setContent(reply.c_str());
    request->send(response);
  });

  ClientCapture postCapture;
  {
    MongooseHttpClient client;
    TEST_ASSERT_TRUE(client.post("http://127.0.0.1:18083/echo",
                                 "text/plain",
                                 "post-body",
                                 [&postCapture](MongooseHttpClientResponse *response) {
                                   postCapture.responded = true;
                                   postCapture.code = response->respCode();
                                   postCapture.body.assign(response->body().c_str(), response->body().length());
                                   postCapture.replyHeader.assign(response->headers("X-Method").c_str(),
                                                                  response->headers("X-Method").length());
                                 },
                                 [&postCapture]() { postCapture.closed = true; }));
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&postCapture]() { return postCapture.closed; }), "POST client request timed out");
  }
  TEST_ASSERT_EQUAL(200, postCapture.code);
  TEST_ASSERT_EQUAL_STRING("POST:post-body", postCapture.body.c_str());
  TEST_ASSERT_EQUAL_STRING("POST", postCapture.replyHeader.c_str());

  ClientCapture putCapture;
  {
    MongooseHttpClient client;
    TEST_ASSERT_TRUE(client.put("http://127.0.0.1:18083/echo",
                                "text/plain",
                                "put-body",
                                [&putCapture](MongooseHttpClientResponse *response) {
                                  putCapture.responded = true;
                                  putCapture.code = response->respCode();
                                  putCapture.body.assign(response->body().c_str(), response->body().length());
                                  putCapture.replyHeader.assign(response->headers("X-Method").c_str(),
                                                                 response->headers("X-Method").length());
                                },
                                [&putCapture]() { putCapture.closed = true; }));
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&putCapture]() { return putCapture.closed; }), "PUT client request timed out");
  }
  TEST_ASSERT_EQUAL(200, putCapture.code);
  TEST_ASSERT_EQUAL_STRING("PUT:put-body", putCapture.body.c_str());
  TEST_ASSERT_EQUAL_STRING("PUT", putCapture.replyHeader.c_str());
}

void runHttpClientTests() {
  RUN_TEST(test_http_client_get_exposes_status_body_headers_and_onbody);
  RUN_TEST(test_http_client_post_and_put_round_trip);
}
