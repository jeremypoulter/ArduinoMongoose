#include <unity.h>

#include <MongooseHttpClient.h>
#include <MongooseHttpServer.h>

#include <string>
#include <vector>

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

#ifdef ARDUINO
static void test_http_server_stream_response_preserves_write_order() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18087));

  server.on("/stream", HTTP_GET, [](MongooseHttpServerRequest *request) {
    MongooseHttpServerResponseStream *response = request->beginResponseStream();
    response->setContentType("application/json");
    response->print("{");
    response->print("\"ok\":");
    response->print("true}");
    request->send(response);
  });

  HttpResponseCapture capture;
  {
    MongooseHttpClient client;
    TEST_ASSERT_TRUE(client.get("http://127.0.0.1:18087/stream", [&capture](MongooseHttpClientResponse *response) {
      capture.responded = true;
      capture.code = response->respCode();
      capture.body.assign(response->body().c_str(), response->body().length());
    }, [&capture]() { capture.closed = true; }));
    TEST_ASSERT_TRUE_MESSAGE(pumpUntil([&capture]() { return capture.closed; }), "stream request timed out");
  }

  TEST_ASSERT_TRUE(capture.responded);
  TEST_ASSERT_EQUAL(200, capture.code);
  TEST_ASSERT_EQUAL_STRING("{\"ok\":true}", capture.body.c_str());
}
#endif

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
// test_multipart_upload_basic
// Verify that a small multipart/form-data POST is dispatched correctly via
// mg_http_next_multipart() (the Mongoose 7 multipart API):
// PART_BEGIN → PART_DATA → PART_END.
// The response is sent from the onUpload callback.
// ---------------------------------------------------------------------------
static void test_multipart_upload_basic()
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

  const char *payload = "Hello, upload world!";
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

  TEST_ASSERT_EQUAL(200, capture.code);
  TEST_ASSERT_TRUE(capture.requestHandlerCalled);
  TEST_ASSERT_EQUAL_STRING(payload, capture.data.c_str());

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
// test_multipart_upload_filename
// Verify that the filename reported in upload callbacks matches what was sent.
// ---------------------------------------------------------------------------
static void test_multipart_upload_filename()
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
// test_multipart_upload_auth_rejection
// Verify that if onRequest rejects the upload (sends 401), no PART_* events
// are dispatched afterwards.
// ---------------------------------------------------------------------------
static void test_multipart_upload_auth_rejection()
{
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18084));

  bool uploadCalled = false;

  server.on("/secure_upload", HTTP_POST)
    ->onRequest([](MongooseHttpServerRequest *request) {
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
// test_raw_body_upload_basic
// Verify that a raw (non-multipart) POST body is streamed incrementally to
// the upload handler: PART_BEGIN → one or more PART_DATA → PART_END.
// This path uses the same escape-hatch mechanism as mg_http_start_upload()
// so the upload handler receives data without buffering the full body first.
// ---------------------------------------------------------------------------
static void test_raw_body_upload_basic()
{
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18085));

  UploadCapture capture;

  server.on("/raw_upload", HTTP_POST)
    ->onRequest([&capture](MongooseHttpServerRequest * /*request*/) {
        capture.requestHandlerCalled = true;
      })
    ->onUpload([&capture](MongooseHttpServerRequest *request, int ev,
                           MongooseString /*filename*/, uint64_t /*index*/,
                           uint8_t *data, size_t len) -> size_t {
        capture.events.push_back(ev);
        if(ev == MG_EV_HTTP_PART_DATA && data && len > 0) {
          capture.data.append((const char *)data, len);
        }
        if(ev == MG_EV_HTTP_PART_END) {
          request->send(200, "text/plain", "Raw OK");
        }
        return 0;
      });

  const char *payload = "Raw binary firmware data";
  {
    MongooseHttpClientRequest *req =
        new MongooseHttpClientRequest("http://127.0.0.1:18085/raw_upload");
    req->setMethod(HTTP_POST);
    req->setContentType("application/octet-stream");
    req->setContent((const uint8_t *)payload, strlen(payload));
    req->onResponse([&capture](MongooseHttpClientResponse *response) {
      capture.responded = true;
      capture.code      = response->respCode();
    });
    req->onClose([&capture]() { capture.closed = true; });
    TEST_ASSERT_TRUE(req->send());
    TEST_ASSERT_TRUE_MESSAGE(
        pumpUntil([&capture]() { return capture.closed; }, 3000),
        "raw_upload request timed out");
  }

  TEST_ASSERT_EQUAL(200, capture.code);
  TEST_ASSERT_TRUE(capture.requestHandlerCalled);
  TEST_ASSERT_EQUAL_STRING(payload, capture.data.c_str());

  bool sawBegin = false, sawData = false, sawEnd = false;
  for(int ev : capture.events) {
    if(ev == MG_EV_HTTP_PART_BEGIN) sawBegin = true;
    if(ev == MG_EV_HTTP_PART_DATA)  sawData  = true;
    if(ev == MG_EV_HTTP_PART_END)   sawEnd   = true;
  }
  TEST_ASSERT_TRUE_MESSAGE(sawBegin, "raw upload: expected PART_BEGIN event");
  TEST_ASSERT_TRUE_MESSAGE(sawData,  "raw upload: expected PART_DATA event");
  TEST_ASSERT_TRUE_MESSAGE(sawEnd,   "raw upload: expected PART_END event");
}

// ---------------------------------------------------------------------------
// test_raw_body_upload_large
// Send a large payload (64 KB) as a raw binary POST and verify all bytes are
// delivered correctly to the upload handler via streaming MG_EV_READ events.
// ---------------------------------------------------------------------------
static void test_raw_body_upload_large()
{
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18086));

  const size_t PAYLOAD_SIZE = 64 * 1024;
  std::string payload(PAYLOAD_SIZE, 'A');

  size_t totalReceived = 0;
  int    dataCallCount = 0;

  server.on("/big_raw_upload", HTTP_POST)
    ->onRequest([](MongooseHttpServerRequest * /*request*/) { })
    ->onUpload([&totalReceived, &dataCallCount](
                   MongooseHttpServerRequest *request, int ev,
                   MongooseString /*filename*/, uint64_t /*index*/,
                   uint8_t * /*data*/, size_t len) -> size_t {
        if(ev == MG_EV_HTTP_PART_DATA) {
          totalReceived += len;
          dataCallCount++;
        }
        if(ev == MG_EV_HTTP_PART_END) {
          request->send(200, "text/plain", "Big Raw OK");
        }
        return 0;
      });

  bool closed = false;
  int  code   = 0;
  {
    MongooseHttpClientRequest *req =
        new MongooseHttpClientRequest("http://127.0.0.1:18086/big_raw_upload");
    req->setMethod(HTTP_POST);
    req->setContentType("application/octet-stream");
    req->setContent((const uint8_t *)payload.c_str(), payload.size());
    req->onResponse([&code](MongooseHttpClientResponse *response) {
      code = response->respCode();
    });
    req->onClose([&closed]() { closed = true; });
    TEST_ASSERT_TRUE(req->send());
    TEST_ASSERT_TRUE_MESSAGE(
        pumpUntil([&closed]() { return closed; }, 10000),
        "big_raw_upload request timed out");
  }

  TEST_ASSERT_EQUAL(200, code);
  TEST_ASSERT_EQUAL(PAYLOAD_SIZE, totalReceived);
  TEST_ASSERT_GREATER_THAN(0, dataCallCount);
}

// ---------------------------------------------------------------------------
// Keep-alive coverage.
//
// The in-process MongooseHttpClient always sends "Connection: close", so these
// tests drive the server with a raw HTTP/1.1 client (mg_http_connect) that can
// negotiate keep-alive and reuse the socket for a second request.
// ---------------------------------------------------------------------------
namespace {

struct RawResponse {
  int code = 0;
  std::string connection;  // value of the response Connection header
  std::string body;
};

// A minimal HTTP/1.1 client that sends a queued list of raw requests, one after
// each response, all on the same connection.
struct RawKeepAliveClient {
  std::vector<std::string> requests;
  size_t sent = 0;
  std::vector<RawResponse> responses;
  bool connected = false;
  bool closed = false;
  bool errored = false;

  void sendNext(mg_connection *c) {
    if (sent < requests.size()) {
      mg_send(c, requests[sent].data(), requests[sent].size());
      sent++;
    }
  }

  static void handler(mg_connection *c, int ev, void *ev_data) {
    RawKeepAliveClient *self = static_cast<RawKeepAliveClient *>(c->fn_data);
    if (ev == MG_EV_CONNECT) {
      self->connected = true;
      self->sendNext(c);
    } else if (ev == MG_EV_HTTP_MSG) {
      mg_http_message *hm = static_cast<mg_http_message *>(ev_data);
      RawResponse r;
      r.code = mg_http_status(hm);
      mg_str *cc = mg_http_get_header(hm, "Connection");
      if (cc) {
        r.connection.assign(cc->buf, cc->len);
      }
      r.body.assign(hm->body.buf, hm->body.len);
      self->responses.push_back(r);
      self->sendNext(c);  // reuse the connection for the next queued request
    } else if (ev == MG_EV_ERROR) {
      self->errored = true;
    } else if (ev == MG_EV_CLOSE) {
      self->closed = true;
    }
  }
};

static size_t managerConnectionCount() {
  size_t count = 0;
  for (mg_connection *c = Mongoose.getMgr()->conns; c != nullptr; c = c->next) {
    count++;
  }
  return count;
}

static void nopHandler(mg_connection * /*c*/, int /*ev*/, void * /*ev_data*/) {}

}  // namespace

static void test_http_server_keepalive_reuses_connection() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18101));

  int hits = 0;
  server.on("/ka", HTTP_GET, [&hits](MongooseHttpServerRequest *request) {
    hits++;
    TEST_ASSERT_TRUE_MESSAGE(request->keepAlive(), "HTTP/1.1 request should negotiate keep-alive");
    request->send(200, "text/plain", "ka");
  });

  RawKeepAliveClient client;
  client.requests.push_back("GET /ka HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");
  client.requests.push_back("GET /ka HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");

  mg_connection *c = mg_http_connect(Mongoose.getMgr(), "http://127.0.0.1:18101",
                                     RawKeepAliveClient::handler, &client);
  TEST_ASSERT_NOT_NULL(c);

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&client]() { return client.responses.size() >= 2; }, 3000),
      "expected two responses on a single kept-alive connection");

  TEST_ASSERT_EQUAL(2, client.responses.size());
  TEST_ASSERT_EQUAL(2, hits);
  TEST_ASSERT_EQUAL(200, client.responses[0].code);
  TEST_ASSERT_EQUAL_STRING("keep-alive", client.responses[0].connection.c_str());
  TEST_ASSERT_EQUAL_STRING("ka", client.responses[0].body.c_str());
  TEST_ASSERT_EQUAL_STRING("ka", client.responses[1].body.c_str());
  TEST_ASSERT_FALSE_MESSAGE(client.closed,
      "connection must stay open across keep-alive responses");
}

static void test_http_server_keepalive_explicit_close() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18102));

  server.on("/ka", HTTP_GET, [](MongooseHttpServerRequest *request) {
    TEST_ASSERT_FALSE_MESSAGE(request->keepAlive(),
        "explicit Connection: close must defeat keep-alive");
    request->send(200, "text/plain", "bye");
  });

  RawKeepAliveClient client;
  client.requests.push_back(
      "GET /ka HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n");

  TEST_ASSERT_NOT_NULL(mg_http_connect(Mongoose.getMgr(), "http://127.0.0.1:18102",
                                       RawKeepAliveClient::handler, &client));

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&client]() { return client.closed; }, 3000),
      "connection should close after an explicit Connection: close request");

  TEST_ASSERT_EQUAL(1, client.responses.size());
  TEST_ASSERT_EQUAL_STRING("close", client.responses[0].connection.c_str());
}

static void test_http_server_keepalive_disabled() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18103));
  server.enableKeepAlive(false);

  server.on("/ka", HTTP_GET, [](MongooseHttpServerRequest *request) {
    TEST_ASSERT_FALSE_MESSAGE(request->keepAlive(),
        "enableKeepAlive(false) must force close mode");
    request->send(200, "text/plain", "x");
  });

  RawKeepAliveClient client;
  client.requests.push_back("GET /ka HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");

  TEST_ASSERT_NOT_NULL(mg_http_connect(Mongoose.getMgr(), "http://127.0.0.1:18103",
                                       RawKeepAliveClient::handler, &client));

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&client]() { return client.closed; }, 3000),
      "connection should close when keep-alive is disabled");

  TEST_ASSERT_EQUAL(1, client.responses.size());
  TEST_ASSERT_EQUAL_STRING("close", client.responses[0].connection.c_str());
}

static void test_http_server_force_close() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18104));

  server.on("/ka", HTTP_GET, [](MongooseHttpServerRequest *request) {
    request->forceClose();
    request->send(200, "text/plain", "x");
  });

  RawKeepAliveClient client;
  client.requests.push_back("GET /ka HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");

  TEST_ASSERT_NOT_NULL(mg_http_connect(Mongoose.getMgr(), "http://127.0.0.1:18104",
                                       RawKeepAliveClient::handler, &client));

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&client]() { return client.closed; }, 3000),
      "forceClose() should close the connection after the response");

  TEST_ASSERT_EQUAL(1, client.responses.size());
  TEST_ASSERT_EQUAL_STRING("close", client.responses[0].connection.c_str());
}

static void test_http_server_keepalive_idle_timeout() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18105));
  server.setKeepAliveTimeout(150);  // ms

  server.on("/ka", HTTP_GET, [](MongooseHttpServerRequest *request) {
    request->send(200, "text/plain", "ka");
  });

  RawKeepAliveClient client;
  client.requests.push_back("GET /ka HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");

  TEST_ASSERT_NOT_NULL(mg_http_connect(Mongoose.getMgr(), "http://127.0.0.1:18105",
                                       RawKeepAliveClient::handler, &client));

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&client]() { return !client.responses.empty(); }, 3000),
      "expected a keep-alive response");
  TEST_ASSERT_EQUAL_STRING("keep-alive", client.responses[0].connection.c_str());
  TEST_ASSERT_FALSE(client.closed);

  // The connection is now parked idle and should be reaped after the timeout.
  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&client]() { return client.closed; }, 3000),
      "idle kept-alive connection should be reaped after the timeout");
  TEST_ASSERT_EQUAL(1, client.responses.size());
}

static void test_http_server_keepalive_pool_pressure_veto() {
  ScopedMongoose mongoose;
  MongooseHttpServer server;
  TEST_ASSERT_TRUE(server.begin(18106));

  server.on("/probe", HTTP_GET, [](MongooseHttpServerRequest *request) {
    TEST_ASSERT_FALSE_MESSAGE(request->keepAlive(),
        "keep-alive must be vetoed while the manager is over the connection limit");
    request->send(200, "text/plain", "x");
  });

  // Hold open enough idle connections to push the manager over
  // ARDUINO_MONGOOSE_KEEPALIVE_MAX_CONNECTIONS.
  std::vector<mg_connection *> held;
  for (int i = 0; i < 12; i++) {
    mg_connection *c = mg_connect(Mongoose.getMgr(), "tcp://127.0.0.1:18106",
                                  nopHandler, nullptr);
    TEST_ASSERT_NOT_NULL(c);
    held.push_back(c);
  }

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([]() {
        return managerConnectionCount() > ARDUINO_MONGOOSE_KEEPALIVE_MAX_CONNECTIONS + 1;
      }, 3000),
      "failed to establish enough connections to exceed the keep-alive limit");

  RawKeepAliveClient client;
  client.requests.push_back("GET /probe HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");

  TEST_ASSERT_NOT_NULL(mg_http_connect(Mongoose.getMgr(), "http://127.0.0.1:18106",
                                       RawKeepAliveClient::handler, &client));

  TEST_ASSERT_TRUE_MESSAGE(
      pumpUntil([&client]() { return !client.responses.empty(); }, 3000),
      "expected a response to the probe request");

  TEST_ASSERT_EQUAL_STRING("close", client.responses[0].connection.c_str());
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
#ifdef ARDUINO
  RUN_TEST(test_http_server_stream_response_preserves_write_order);
#endif
  RUN_TEST(test_multipart_upload_basic);
  RUN_TEST(test_multipart_upload_filename);
  RUN_TEST(test_multipart_upload_auth_rejection);
  RUN_TEST(test_raw_body_upload_basic);
  RUN_TEST(test_raw_body_upload_large);
  RUN_TEST(test_http_server_keepalive_reuses_connection);
  RUN_TEST(test_http_server_keepalive_explicit_close);
  RUN_TEST(test_http_server_keepalive_disabled);
  RUN_TEST(test_http_server_force_close);
  RUN_TEST(test_http_server_keepalive_idle_timeout);
  RUN_TEST(test_http_server_keepalive_pool_pressure_veto);
  RUN_TEST(test_https_server_tls_handshake_succeeds);
}
