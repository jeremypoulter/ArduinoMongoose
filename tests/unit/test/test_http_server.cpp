#include <unity.h>

#include <MongooseHttpClient.h>
#include <MongooseHttpServer.h>

#include <string>
#include <vector>

#include "test_support.h"

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

// ---------------------------------------------------------------------------
// Helpers for building raw multipart/form-data bodies in tests.
// ---------------------------------------------------------------------------
namespace {

// Build a multipart body and return it together with the Content-Type header.
struct MultipartBody {
  std::string contentType;  // "multipart/form-data; boundary=<boundary>"
  std::string body;
};

static MultipartBody buildMultipart(const char *boundary,
                                    const char *filename,
                                    const char *data, size_t data_len)
{
  MultipartBody mp;
  mp.contentType = std::string("multipart/form-data; boundary=") + boundary;

  std::string &b = mp.body;
  b += "--";
  b += boundary;
  b += "\r\n";
  b += "Content-Disposition: form-data; name=\"file\"; filename=\"";
  b += filename;
  b += "\"\r\n";
  b += "Content-Type: application/octet-stream\r\n";
  b += "\r\n";
  b.append(data, data_len);
  b += "\r\n--";
  b += boundary;
  b += "--\r\n";
  return mp;
}

struct UploadCapture {
  std::vector<int>         events;    // sequence of MG_EV_HTTP_PART_* events
  std::vector<std::string> filenames; // filename per event
  std::string              data;      // accumulated PART_DATA bytes
  bool requestHandlerCalled = false;
  bool responded = false;
  bool closed    = false;
  int  code      = 0;
};

}  // namespace

// ---------------------------------------------------------------------------
// test_streaming_multipart_upload_basic
// Verify that a small multipart POST is dispatched correctly via the
// streaming (MG_EV_HTTP_HDRS) path: PART_BEGIN → PART_DATA → PART_END.
// The response is sent from the onUpload callback (matching the OTA pattern).
// ---------------------------------------------------------------------------
static void test_streaming_multipart_upload_basic()
{
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18082));

  UploadCapture capture;

  server.on("/upload", HTTP_POST)
    ->onRequest([&capture](MongooseHttpServerRequest * /*request*/) {
        // Auth / state initialisation only – do NOT send the response here.
        capture.requestHandlerCalled = true;
      })
    ->onUpload([&capture](MongooseHttpServerRequest *request, int ev,
                           MongooseString filename, uint64_t /*index*/,
                           uint8_t *data, size_t len) -> size_t {
        capture.events.push_back(ev);
        capture.filenames.push_back(std::string(filename.c_str(), filename.length()));
        if(ev == MG_EV_HTTP_PART_DATA && data && len > 0) {
          capture.data.append((const char *)data, len);
        }
        if(ev == MG_EV_HTTP_PART_END) {
          request->send(200, "text/plain", "Upload OK");
        }
        return 0;
      });

  const char *payload = "Hello, streaming world!";
  MultipartBody mp = buildMultipart("testbdy", "hello.txt",
                                    payload, strlen(payload));

  {
    MongooseHttpClientRequest *req =
        new MongooseHttpClientRequest("http://127.0.0.1:18082/upload");
    req->setMethod(HTTP_POST);
    req->setContentType(mp.contentType.c_str());
    req->setContent((const uint8_t *)mp.body.c_str(), mp.body.size());
    req->onResponse([&capture](MongooseHttpClientResponse *response) {
      capture.responded = true;
      capture.code      = response->respCode();
    });
    req->onClose([&capture]() { capture.closed = true; });
    TEST_ASSERT_TRUE(req->send());
    TEST_ASSERT_TRUE_MESSAGE(
        pumpUntil([&capture]() { return capture.closed; }, 3000),
        "upload request timed out");
  }

  // Verify upload events
  TEST_ASSERT_EQUAL(200, capture.code);
  TEST_ASSERT_TRUE(capture.requestHandlerCalled);
  TEST_ASSERT_EQUAL_STRING(payload, capture.data.c_str());

  // Must have seen at least one PART_BEGIN, PART_DATA, PART_END
  bool sawBegin = false, sawData = false, sawEnd = false;
  for(int ev : capture.events) {
    if(ev == MG_EV_HTTP_PART_BEGIN) sawBegin = true;
    if(ev == MG_EV_HTTP_PART_DATA)  sawData  = true;
    if(ev == MG_EV_HTTP_PART_END)   sawEnd   = true;
  }
  TEST_ASSERT_TRUE_MESSAGE(sawBegin, "expected PART_BEGIN event");
  TEST_ASSERT_TRUE_MESSAGE(sawData,  "expected PART_DATA event");
  TEST_ASSERT_TRUE_MESSAGE(sawEnd,   "expected PART_END event");
}

// ---------------------------------------------------------------------------
// test_streaming_multipart_upload_filename
// Verify that the filename reported in upload callbacks matches what was sent.
// ---------------------------------------------------------------------------
static void test_streaming_multipart_upload_filename()
{
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18083));

  std::string reportedFilename;

  server.on("/upload2", HTTP_POST)
    ->onRequest([](MongooseHttpServerRequest * /*request*/) {
        // Auth / init only – response is sent from onUpload.
      })
    ->onUpload([&reportedFilename](MongooseHttpServerRequest *request, int ev,
                                    MongooseString filename, uint64_t /*index*/,
                                    uint8_t * /*data*/, size_t /*len*/) -> size_t {
        if(ev == MG_EV_HTTP_PART_BEGIN) {
          reportedFilename.assign(filename.c_str(), filename.length());
        }
        if(ev == MG_EV_HTTP_PART_END) {
          request->send(200, "text/plain", "ok");
        }
        return 0;
      });

  MultipartBody mp = buildMultipart("mybnd", "firmware.bin", "DATA", 4);

  bool closed = false;
  {
    MongooseHttpClientRequest *req =
        new MongooseHttpClientRequest("http://127.0.0.1:18083/upload2");
    req->setMethod(HTTP_POST);
    req->setContentType(mp.contentType.c_str());
    req->setContent((const uint8_t *)mp.body.c_str(), mp.body.size());
    req->onClose([&closed]() { closed = true; });
    TEST_ASSERT_TRUE(req->send());
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }, 3000),
                             "upload2 request timed out");
  }

  TEST_ASSERT_EQUAL_STRING("firmware.bin", reportedFilename.c_str());
}

// ---------------------------------------------------------------------------
// test_streaming_multipart_upload_auth_rejection
// Verify that if onRequest rejects the upload (sends 401), no PART_* events
// are dispatched afterwards.
// ---------------------------------------------------------------------------
static void test_streaming_multipart_upload_auth_rejection()
{
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18084));

  bool uploadCalled = false;

  server.on("/secure_upload", HTTP_POST)
    ->onRequest([](MongooseHttpServerRequest *request) {
        // Always reject
        request->send(401, "text/plain", "Unauthorized");
      })
    ->onUpload([&uploadCalled](MongooseHttpServerRequest * /*request*/, int /*ev*/,
                                MongooseString /*filename*/, uint64_t /*index*/,
                                uint8_t * /*data*/, size_t /*len*/) -> size_t {
        uploadCalled = true;
        return 0;
      });

  MultipartBody mp = buildMultipart("bnd", "file.bin", "PAYLOAD", 7);

  bool closed = false;
  int code    = 0;
  {
    MongooseHttpClientRequest *req =
        new MongooseHttpClientRequest("http://127.0.0.1:18084/secure_upload");
    req->setMethod(HTTP_POST);
    req->setContentType(mp.contentType.c_str());
    req->setContent((const uint8_t *)mp.body.c_str(), mp.body.size());
    req->onResponse([&code](MongooseHttpClientResponse *response) {
      code = response->respCode();
    });
    req->onClose([&closed]() { closed = true; });
    TEST_ASSERT_TRUE(req->send());
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&closed]() { return closed; }, 3000),
                             "secure_upload request timed out");
  }

  TEST_ASSERT_EQUAL(401, code);
  TEST_ASSERT_FALSE_MESSAGE(uploadCalled,
    "upload callback must not be called when request is rejected");
}

// ---------------------------------------------------------------------------
// test_streaming_multipart_upload_large_payload
// Send a payload larger than a typical Mongoose receive buffer to verify
// that the streaming path processes data in multiple chunks.
// ---------------------------------------------------------------------------
static void test_streaming_multipart_upload_large_payload()
{
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18085));

  // 64 KB of repeating 'A' – well above any single receive-buffer fill.
  const size_t PAYLOAD_SIZE = 64 * 1024;
  std::string payload(PAYLOAD_SIZE, 'A');

  size_t totalReceived = 0;
  int    dataCallCount = 0;

  server.on("/big_upload", HTTP_POST)
    ->onRequest([](MongooseHttpServerRequest * /*request*/) {
        // Auth / init only – response is sent from onUpload.
      })
    ->onUpload([&totalReceived, &dataCallCount](
                   MongooseHttpServerRequest *request, int ev,
                   MongooseString /*filename*/, uint64_t /*index*/,
                   uint8_t * /*data*/, size_t len) -> size_t {
        if(ev == MG_EV_HTTP_PART_DATA) {
          totalReceived += len;
          dataCallCount++;
        }
        if(ev == MG_EV_HTTP_PART_END) {
          request->send(200, "text/plain", "Big OK");
        }
        return 0;
      });

  MultipartBody mp = buildMultipart("bigbnd", "big.bin",
                                    payload.c_str(), payload.size());

  bool closed = false;
  int  code   = 0;
  {
    MongooseHttpClientRequest *req =
        new MongooseHttpClientRequest("http://127.0.0.1:18085/big_upload");
    req->setMethod(HTTP_POST);
    req->setContentType(mp.contentType.c_str());
    req->setContent((const uint8_t *)mp.body.c_str(), mp.body.size());
    req->onResponse([&code](MongooseHttpClientResponse *response) {
      code = response->respCode();
    });
    req->onClose([&closed]() { closed = true; });
    TEST_ASSERT_TRUE(req->send());
    TEST_ASSERT_TRUE_MESSAGE(
        pumpUntil([&closed]() { return closed; }, 10000),
        "big_upload request timed out");
  }

  TEST_ASSERT_EQUAL(200, code);
  TEST_ASSERT_EQUAL(PAYLOAD_SIZE, totalReceived);
  // Streaming: data should have been delivered in at least one call.
  TEST_ASSERT_GREATER_THAN(0, dataCallCount);
}

void runHttpServerTests() {
  RUN_TEST(test_http_server_routes_and_not_found);
  RUN_TEST(test_http_server_authentication_and_challenge);
  RUN_TEST(test_streaming_multipart_upload_basic);
  RUN_TEST(test_streaming_multipart_upload_filename);
  RUN_TEST(test_streaming_multipart_upload_auth_rejection);
  RUN_TEST(test_streaming_multipart_upload_large_payload);
}
