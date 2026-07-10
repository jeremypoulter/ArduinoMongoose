#include <unity.h>

#include <MongooseHttpClient.h>
#include <MongooseHttpServer.h>

#include <string>

#include "test_support.h"
#include "test_tls_certs.h"

namespace {
struct HttpResponseCapture {
  bool responded = false;
  bool closed = false;
  int code = 0;
  std::string body;
  std::string authenticateHeader;
};

static HttpResponseCapture performRequest(const std::function<bool(MongooseHttpClient &)> &sendRequest) {
  MongooseHttpClient client;
  HttpResponseCapture capture;

  TEST_ASSERT_TRUE(sendRequest(client));
  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&capture]() { return capture.closed; }, 3000),
      "HTTP request timed out");

  return capture;
}
}  // namespace

static void test_http_server_routes_and_not_found() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18080));

  server.on("/", HTTP_GET, [](MongooseHttpServerRequest *request) {
    request->send(200, "text/plain", "Hello world");
  });
  server.on("/get", HTTP_GET, [](MongooseHttpServerRequest *request) {
    char message[64] = "";
    if (request->getParam("message", message, sizeof(message)) > 0) {
      std::string reply = std::string("Hello, GET: ") + message;
      request->send(200, "text/plain", reply.c_str());
    } else {
      request->send(400, "text/plain", "No message sent");
    }
  });
  server.on("/post", HTTP_POST, [](MongooseHttpServerRequest *request) {
    char message[64] = "";
    if (request->getParam("message", message, sizeof(message)) > 0) {
      std::string reply = std::string("Hello, POST: ") + message;
      request->send(200, "text/plain", reply.c_str());
    } else {
      request->send(400, "text/plain", "No message sent");
    }
  });
  server.onNotFound([](MongooseHttpServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });

  HttpResponseCapture root;
  {
    MongooseHttpClient client;
    TEST_ASSERT_TRUE(client.get("http://127.0.0.1:18080/", [&root](MongooseHttpClientResponse *response) {
      root.responded = true;
      root.code = response->respCode();
      root.body.assign(response->body().c_str(), response->body().length());
    }, [&root]() { root.closed = true; }));
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&root]() { return root.closed; }), "root request timed out");
  }
  TEST_ASSERT_TRUE(root.responded);
  TEST_ASSERT_EQUAL(200, root.code);
  TEST_ASSERT_EQUAL_STRING("Hello world", root.body.c_str());

  HttpResponseCapture getResponse;
  {
    MongooseHttpClient client;
    TEST_ASSERT_TRUE(client.get("http://127.0.0.1:18080/get?message=Copilot", [&getResponse](MongooseHttpClientResponse *response) {
      getResponse.responded = true;
      getResponse.code = response->respCode();
      getResponse.body.assign(response->body().c_str(), response->body().length());
    }, [&getResponse]() { getResponse.closed = true; }));
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&getResponse]() { return getResponse.closed; }), "GET request timed out");
  }
  TEST_ASSERT_EQUAL(200, getResponse.code);
  TEST_ASSERT_EQUAL_STRING("Hello, GET: Copilot", getResponse.body.c_str());

  HttpResponseCapture postResponse;
  {
    MongooseHttpClient client;
    TEST_ASSERT_TRUE(client.post("http://127.0.0.1:18080/post",
                                 "application/x-www-form-urlencoded",
                                 "message=PlatformIO",
                                 [&postResponse](MongooseHttpClientResponse *response) {
                                   postResponse.responded = true;
                                   postResponse.code = response->respCode();
                                   postResponse.body.assign(response->body().c_str(), response->body().length());
                                 },
                                 [&postResponse]() { postResponse.closed = true; }));
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&postResponse]() { return postResponse.closed; }), "POST request timed out");
  }
  TEST_ASSERT_EQUAL(200, postResponse.code);
  TEST_ASSERT_EQUAL_STRING("Hello, POST: PlatformIO", postResponse.body.c_str());

  HttpResponseCapture missing;
  {
    MongooseHttpClient client;
    TEST_ASSERT_TRUE(client.get("http://127.0.0.1:18080/notfound", [&missing](MongooseHttpClientResponse *response) {
      missing.responded = true;
      missing.code = response->respCode();
      missing.body.assign(response->body().c_str(), response->body().length());
    }, [&missing]() { missing.closed = true; }));
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&missing]() { return missing.closed; }), "not found request timed out");
  }
  TEST_ASSERT_EQUAL(404, missing.code);
  TEST_ASSERT_EQUAL_STRING("Not found", missing.body.c_str());
}

static void test_http_server_authentication_and_challenge() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18081));

  server.on("/auth", HTTP_GET, [](MongooseHttpServerRequest *request) {
    if (!request->authenticate("user", "pass")) {
      request->requestAuthentication("UnitTestRealm");
      return;
    }
    request->send(200, "text/plain", "Authenticated");
  });

  HttpResponseCapture unauthorized;
  {
    MongooseHttpClient client;
    TEST_ASSERT_TRUE(client.get("http://127.0.0.1:18081/auth", [&unauthorized](MongooseHttpClientResponse *response) {
      unauthorized.responded = true;
      unauthorized.code = response->respCode();
      MongooseString header = response->headers("WWW-Authenticate");
      if (header) {
        unauthorized.authenticateHeader.assign(header.c_str(), header.length());
      }
    }, [&unauthorized]() { unauthorized.closed = true; }));
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&unauthorized]() { return unauthorized.closed; }), "unauthorized request timed out");
  }
  TEST_ASSERT_EQUAL(401, unauthorized.code);
  TEST_ASSERT_EQUAL_STRING("Basic realm=UnitTestRealm", unauthorized.authenticateHeader.c_str());

  HttpResponseCapture authorized;
  {
    MongooseHttpClient client;
    MongooseHttpClientRequest *request = client.beginRequest("http://127.0.0.1:18081/auth");
    request->addHeader("Authorization", "Basic dXNlcjpwYXNz");
    request->onResponse([&authorized](MongooseHttpClientResponse *response) {
      authorized.responded = true;
      authorized.code = response->respCode();
      authorized.body.assign(response->body().c_str(), response->body().length());
    })->onClose([&authorized]() { authorized.closed = true; });

    TEST_ASSERT_TRUE(request->send());
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&authorized]() { return authorized.closed; }), "authorized request timed out");
  }
  TEST_ASSERT_EQUAL(200, authorized.code);
  TEST_ASSERT_EQUAL_STRING("Authenticated", authorized.body.c_str());
}

static void test_https_server_tls_handshake_succeeds() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18090, TEST_TLS_CERT, TEST_TLS_KEY));

  server.on("/tls", HTTP_GET, [](MongooseHttpServerRequest *request) {
    request->send(200, "text/plain", "TLS OK");
  });

  struct Capture {
    bool responded = false;
    bool closed = false;
    int code = 0;
    std::string body;
  } capture;

  MongooseHttpClient client;
  MongooseHttpClientRequest *request = client.beginRequest("https://127.0.0.1:18090/tls");
  request->setInsecure();
  request->onResponse([&capture](MongooseHttpClientResponse *response) {
    capture.responded = true;
    capture.code = response->respCode();
    capture.body.assign(response->body().c_str(), response->body().length());
  })->onClose([&capture]() { capture.closed = true; });

  TEST_ASSERT_TRUE(request->send());
  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&capture]() { return capture.closed; }, 5000),
      "HTTPS request timed out - TLS handshake may not have completed");

  TEST_ASSERT_TRUE(capture.responded);
  TEST_ASSERT_EQUAL(200, capture.code);
  TEST_ASSERT_EQUAL_STRING("TLS OK", capture.body.c_str());
}

void runHttpServerTests() {
  RUN_TEST(test_http_server_routes_and_not_found);
  RUN_TEST(test_http_server_authentication_and_challenge);
  RUN_TEST(test_https_server_tls_handshake_succeeds);
}
