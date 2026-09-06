#include <unity.h>

#include <MongooseHttpClient.h>
#include <MongooseHttpServer.h>

#include <cstdlib>
#include <cstring>
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

static void test_http_client_patch_and_delete() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18086));

  server.on("/resource", HTTP_PATCH | HTTP_DELETE, [](MongooseHttpServerRequest *request) {
    std::string method(request->methodStr().c_str(), request->methodStr().length());
    std::string body(request->body().c_str(), request->body().length());
    std::string reply = method + ":" + (body.empty() ? "empty" : body);

    MongooseHttpServerResponseBasic *response = request->beginResponse();
    response->setCode(200);
    response->setContentType("text/plain");
    response->addHeader("X-Method", method.c_str());
    response->setContent(reply.c_str());
    request->send(response);
  });

  ClientCapture patchCapture;
  {
    MongooseHttpClient client;
    TEST_ASSERT_TRUE(client.patch("http://127.0.0.1:18086/resource",
                                  "text/plain",
                                  "patch-body",
                                  [&patchCapture](MongooseHttpClientResponse *response) {
                                    patchCapture.responded = true;
                                    patchCapture.code = response->respCode();
                                    patchCapture.body.assign(response->body().c_str(), response->body().length());
                                    patchCapture.replyHeader.assign(response->headers("X-Method").c_str(),
                                                                    response->headers("X-Method").length());
                                  },
                                  [&patchCapture]() { patchCapture.closed = true; }));
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&patchCapture]() { return patchCapture.closed; }), "PATCH client request timed out");
  }
  TEST_ASSERT_EQUAL(200, patchCapture.code);
  TEST_ASSERT_EQUAL_STRING("PATCH:patch-body", patchCapture.body.c_str());
  TEST_ASSERT_EQUAL_STRING("PATCH", patchCapture.replyHeader.c_str());

  ClientCapture deleteCapture;
  {
    MongooseHttpClient client;
    TEST_ASSERT_TRUE(client.delete_("http://127.0.0.1:18086/resource",
                                    [&deleteCapture](MongooseHttpClientResponse *response) {
                                      deleteCapture.responded = true;
                                      deleteCapture.code = response->respCode();
                                      deleteCapture.body.assign(response->body().c_str(), response->body().length());
                                      deleteCapture.replyHeader.assign(response->headers("X-Method").c_str(),
                                                                       response->headers("X-Method").length());
                                    },
                                    [&deleteCapture]() { deleteCapture.closed = true; }));
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&deleteCapture]() { return deleteCapture.closed; }), "DELETE client request timed out");
  }
  TEST_ASSERT_EQUAL(200, deleteCapture.code);
  TEST_ASSERT_EQUAL_STRING("DELETE:empty", deleteCapture.body.c_str());
  TEST_ASSERT_EQUAL_STRING("DELETE", deleteCapture.replyHeader.c_str());
}

static void test_http_client_abort_returns_false_when_not_connected() {
  // A freshly-constructed request has no underlying connection, so abort()
  // and cancel() must return false without crashing.
  MongooseHttpClient client;
  MongooseHttpClientRequest *request = client.beginRequest("http://127.0.0.1:18087/noop");
  TEST_ASSERT_FALSE(request->abort());
  TEST_ASSERT_FALSE(request->cancel());
}

static void test_http_client_cancel_aborts_in_flight_request() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18087));

  // Server hangs forever (never responds) so we can exercise cancel().
  server.on("/hang", HTTP_GET, [](MongooseHttpServerRequest *) {
    // Intentionally do nothing – request is never answered.
  });

  MongooseHttpClient client;
  bool closed = false;
  MongooseHttpClientRequest *request = client.beginRequest("http://127.0.0.1:18087/hang");
  request->onClose([&closed]() { closed = true; });
  TEST_ASSERT_TRUE(request->send());

  // Pump briefly to let the TCP connection establish.
  pumpFor(100);

  // cancel() must return true (connection exists) and trigger the close path.
  TEST_ASSERT_TRUE(request->cancel());

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&closed]() { return closed; }, 2000),
      "close callback was not invoked after cancel()");
}

static void test_http_client_owns_uri_content_type_and_body_after_return() {
  // The exact shape of the use-after-free this class's ownership fix
  // targets: send() only *starts* the connection -- _uri, _contentType and
  // _body are not read until onConnect(), on a later poll -- so a caller
  // whose source buffers do not outlive the call used to hand the request
  // dangling pointers. Free the sources immediately (worse than the original
  // bug, which only needed them to survive past return) and clobber that
  // memory with unrelated allocations before send() so a regression would
  // observably corrupt the request instead of merely being UB that happens
  // to still work.
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18089));

  std::string receivedUri;
  std::string receivedContentType;
  std::string receivedBody;
  server.on("/scratch", HTTP_POST, [&](MongooseHttpServerRequest *request) {
    receivedUri.assign(request->uri().c_str(), request->uri().length());
    receivedContentType.assign(request->contentType().c_str(), request->contentType().length());
    receivedBody.assign(request->body().c_str(), request->body().length());

    MongooseHttpServerResponseBasic *response = request->beginResponse();
    response->setCode(200);
    response->setContent("ok");
    request->send(response);
  });

  MongooseHttpClient client;
  bool closed = false;

  {
    char *uri = (char *)malloc(64);
    strcpy(uri, "http://127.0.0.1:18089/scratch");
    char *contentType = (char *)malloc(32);
    strcpy(contentType, "text/plain");
    char *body = (char *)malloc(16);
    strcpy(body, "scratch-body");

    MongooseHttpClientRequest *request = client.beginRequest(uri);
    request->setMethod(HTTP_POST);
    request->setContentType(contentType);
    request->setContent(body);
    request->onClose([&closed]() { closed = true; });

    free(uri);
    free(contentType);
    free(body);

    // Encourage the allocator to hand that freed memory back out with
    // different content before send() -- and long before the network I/O
    // that actually reads it -- runs.
    for (int i = 0; i < 8; i++) {
      char *scratch = (char *)malloc(64);
      memset(scratch, 'X', 64);
      free(scratch);
    }

    TEST_ASSERT_TRUE(request->send());
  }

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&closed]() { return closed; }),
      "request built from freed source buffers timed out");

  TEST_ASSERT_EQUAL_STRING("/scratch", receivedUri.c_str());
  TEST_ASSERT_EQUAL_STRING("text/plain", receivedContentType.c_str());
  TEST_ASSERT_EQUAL_STRING("scratch-body", receivedBody.c_str());
}

void runHttpClientTests() {
  RUN_TEST(test_http_client_get_exposes_status_body_headers_and_onbody);
  RUN_TEST(test_http_client_post_and_put_round_trip);
  RUN_TEST(test_http_client_patch_and_delete);
  RUN_TEST(test_http_client_abort_returns_false_when_not_connected);
  RUN_TEST(test_http_client_cancel_aborts_in_flight_request);
  RUN_TEST(test_http_client_owns_uri_content_type_and_body_after_return);
}
