// Tests for MongooseHttpMessage shared base-class accessors.
// Both MongooseHttpServerRequest and MongooseHttpClientResponse inherit from
// MongooseHttpMessage; these tests exercise the shared accessors from both
// sides over a real local HTTP round-trip.

#include <unity.h>

#include <MongooseHttpClient.h>
#include <MongooseHttpServer.h>

#include <string>

#include "test_support.h"

// ── server-side accessors ──────────────────────────────────────────────────

static void test_server_request_uri_and_query_string() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18084));

  std::string capturedUri;
  std::string capturedQuery;

  server.on("/path", HTTP_GET, [&](MongooseHttpServerRequest *request) {
    capturedUri.assign(request->uri().c_str(), request->uri().length());
    capturedQuery.assign(request->queryString().c_str(), request->queryString().length());
    request->send(200, "text/plain", "ok");
  });

  bool closed = false;
  MongooseHttpClient client;
  TEST_ASSERT_TRUE(client.get("http://127.0.0.1:18084/path?foo=bar&baz=1",
                               nullptr, [&closed]() { closed = true; }));
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }), "request timed out");

  TEST_ASSERT_EQUAL_STRING("/path", capturedUri.c_str());
  TEST_ASSERT_EQUAL_STRING("foo=bar&baz=1", capturedQuery.c_str());
}

static void test_server_request_method_str() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18085));

  std::string capturedMethod;

  server.on("/method", HTTP_POST, [&](MongooseHttpServerRequest *request) {
    capturedMethod.assign(request->methodStr().c_str(), request->methodStr().length());
    request->send(200, "text/plain", "ok");
  });

  bool closed = false;
  MongooseHttpClient client;
  TEST_ASSERT_TRUE(client.post("http://127.0.0.1:18085/method", "text/plain", "data",
                                nullptr, [&closed]() { closed = true; }));
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }), "request timed out");

  TEST_ASSERT_EQUAL_STRING("POST", capturedMethod.c_str());
}

static void test_server_request_host_and_content_type() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18086));

  std::string capturedHost;
  std::string capturedContentType;

  server.on("/ct", HTTP_POST, [&](MongooseHttpServerRequest *request) {
    capturedHost.assign(request->host().c_str(), request->host().length());
    capturedContentType.assign(request->contentType().c_str(), request->contentType().length());
    request->send(200, "text/plain", "ok");
  });

  bool closed = false;
  MongooseHttpClient client;
  TEST_ASSERT_TRUE(client.post("http://127.0.0.1:18086/ct", "application/json", "{\"x\":1}",
                                nullptr, [&closed]() { closed = true; }));
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }), "request timed out");

  // mongoose sends the Host header without port for non-standard loopback addresses
  TEST_ASSERT_TRUE_MESSAGE(capturedHost.find("127.0.0.1") != std::string::npos,
                           "Host header does not contain expected IP address");
  TEST_ASSERT_EQUAL_STRING("application/json", capturedContentType.c_str());
}

static void test_server_request_proto_is_http11() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18087));

  std::string capturedProto;

  server.on("/proto", HTTP_GET, [&](MongooseHttpServerRequest *request) {
    capturedProto.assign(request->proto().c_str(), request->proto().length());
    request->send(200, "text/plain", "ok");
  });

  bool closed = false;
  MongooseHttpClient client;
  TEST_ASSERT_TRUE(client.get("http://127.0.0.1:18087/proto",
                               nullptr, [&closed]() { closed = true; }));
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }), "request timed out");

  TEST_ASSERT_EQUAL_STRING("HTTP/1.1", capturedProto.c_str());
}

static void test_server_request_header_count_and_indexed_access() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18088));

  int headerCount = 0;
  bool foundXCustom = false;

  server.on("/headers", HTTP_GET, [&](MongooseHttpServerRequest *request) {
    headerCount = request->headers();
    for (int i = 0; i < headerCount; i++) {
      std::string name(request->headerNames(i).c_str(), request->headerNames(i).length());
      std::string value(request->headerValues(i).c_str(), request->headerValues(i).length());
      if (name == "X-Custom" && value == "hello") {
        foundXCustom = true;
      }
    }
    request->send(200, "text/plain", "ok");
  });

  bool closed = false;
  MongooseHttpClient client;
  MongooseHttpClientRequest *request = client.beginRequest("http://127.0.0.1:18088/headers");
  request->addHeader("X-Custom", "hello");
  request->onClose([&closed]() { closed = true; });
  TEST_ASSERT_TRUE(request->send());
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }), "request timed out");

  TEST_ASSERT_TRUE_MESSAGE(headerCount > 0, "expected at least one header");
  TEST_ASSERT_TRUE_MESSAGE(foundXCustom, "X-Custom header not found via indexed access");
}

static void test_server_request_message_contains_method_and_uri() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18089));

  std::string rawMessage;

  server.on("/raw", HTTP_GET, [&](MongooseHttpServerRequest *request) {
    rawMessage.assign(request->message().c_str(), request->message().length());
    request->send(200, "text/plain", "ok");
  });

  bool closed = false;
  MongooseHttpClient client;
  TEST_ASSERT_TRUE(client.get("http://127.0.0.1:18089/raw",
                               nullptr, [&closed]() { closed = true; }));
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }), "request timed out");

  // The raw HTTP message must start with the request line
  TEST_ASSERT_TRUE_MESSAGE(rawMessage.find("GET /raw") != std::string::npos,
                           "raw message does not contain expected request line");
}

// ── client-side accessors ──────────────────────────────────────────────────

static void test_client_response_content_type_and_header_count() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18090));

  server.on("/info", HTTP_GET, [](MongooseHttpServerRequest *request) {
    MongooseHttpServerResponseBasic *response = request->beginResponse();
    response->setCode(200);
    response->setContentType("application/json");
    response->addHeader("X-Extra", "value");
    response->setContent("{}");
    request->send(response);
  });

  std::string capturedContentType;
  int capturedHeaderCount = 0;
  bool foundXExtra = false;
  bool closed = false;

  MongooseHttpClient client;
  MongooseHttpClientRequest *request = client.beginRequest("http://127.0.0.1:18090/info");
  request->onResponse([&](MongooseHttpClientResponse *response) {
    capturedContentType.assign(response->contentType().c_str(),
                               response->contentType().length());
    capturedHeaderCount = response->headers();
    for (int i = 0; i < capturedHeaderCount; i++) {
      std::string name(response->headerNames(i).c_str(), response->headerNames(i).length());
      std::string value(response->headerValues(i).c_str(), response->headerValues(i).length());
      if (name == "X-Extra" && value == "value") {
        foundXExtra = true;
      }
    }
  })->onClose([&closed]() { closed = true; });

  TEST_ASSERT_TRUE(request->send());
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }), "response timed out");

  TEST_ASSERT_EQUAL_STRING("application/json", capturedContentType.c_str());
  TEST_ASSERT_TRUE_MESSAGE(capturedHeaderCount > 0, "expected at least one response header");
  TEST_ASSERT_TRUE_MESSAGE(foundXExtra, "X-Extra response header not found via indexed access");
}

static void test_client_response_proto_and_message() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18091));

  server.on("/proto", HTTP_GET, [](MongooseHttpServerRequest *request) {
    request->send(200, "text/plain", "ok");
  });

  std::string capturedProto;
  std::string rawMessage;
  bool closed = false;

  MongooseHttpClient client;
  MongooseHttpClientRequest *request = client.beginRequest("http://127.0.0.1:18091/proto");
  request->onResponse([&](MongooseHttpClientResponse *response) {
    capturedProto.assign(response->proto().c_str(), response->proto().length());
    rawMessage.assign(response->message().c_str(), response->message().length());
  })->onClose([&closed]() { closed = true; });

  TEST_ASSERT_TRUE(request->send());
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }), "response timed out");

  // For HTTP responses, mongoose parses the status line as "HTTP/1.1 200 OK" where
  // proto field holds the reason phrase ("OK"), not the protocol version string.
  TEST_ASSERT_EQUAL_STRING("OK", capturedProto.c_str());
  // The full raw message must still start with the HTTP/1.1 status line
  TEST_ASSERT_TRUE_MESSAGE(rawMessage.find("HTTP/1.1") != std::string::npos,
                           "raw response message does not contain HTTP/1.1");
  TEST_ASSERT_TRUE_MESSAGE(rawMessage.find("200") != std::string::npos,
                           "raw response message does not contain status code 200");
}

static void test_client_response_resp_status_msg() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18092));

  server.on("/status", HTTP_GET, [](MongooseHttpServerRequest *request) {
    request->send(200, "text/plain", "ok");
  });

  std::string capturedStatusMsg;
  bool closed = false;

  MongooseHttpClient client;
  MongooseHttpClientRequest *request = client.beginRequest("http://127.0.0.1:18092/status");
  request->onResponse([&](MongooseHttpClientResponse *response) {
    capturedStatusMsg.assign(response->respStatusMsg().c_str(),
                             response->respStatusMsg().length());
  })->onClose([&closed]() { closed = true; });

  TEST_ASSERT_TRUE(request->send());
  TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }), "response timed out");

  // respStatusMsg() returns the proto field from mg_http_message.
  // For HTTP responses, mongoose parses "HTTP/1.1 200 OK" such that proto = "OK".
  TEST_ASSERT_EQUAL_STRING("OK", capturedStatusMsg.c_str());
}

void runHttpMessageTests() {
  RUN_TEST(test_server_request_uri_and_query_string);
  RUN_TEST(test_server_request_method_str);
  RUN_TEST(test_server_request_host_and_content_type);
  RUN_TEST(test_server_request_proto_is_http11);
  RUN_TEST(test_server_request_header_count_and_indexed_access);
  RUN_TEST(test_server_request_message_contains_method_and_uri);
  RUN_TEST(test_client_response_content_type_and_header_count);
  RUN_TEST(test_client_response_proto_and_message);
  RUN_TEST(test_client_response_resp_status_msg);
}
