#include <unity.h>
#include <MongooseHttp.h>

static void test_named_http_method_flags_are_stable() {
  TEST_ASSERT_EQUAL_HEX8(0x01, HTTP_GET);
  TEST_ASSERT_EQUAL_HEX8(0x02, HTTP_POST);
  TEST_ASSERT_EQUAL_HEX8(0x7F, HTTP_ANY);
}

static void test_composite_bitmasks_match_expected_methods() {
  HttpRequestMethodComposite readWrite = HTTP_GET | HTTP_POST;

  TEST_ASSERT_TRUE((readWrite & HTTP_GET) != 0);
  TEST_ASSERT_TRUE((readWrite & HTTP_POST) != 0);
  TEST_ASSERT_FALSE((readWrite & HTTP_PUT) != 0);
  TEST_ASSERT_TRUE((HTTP_ANY & HTTP_GET) != 0);
  TEST_ASSERT_TRUE((HTTP_ANY & HTTP_POST) != 0);
  TEST_ASSERT_TRUE((HTTP_ANY & HTTP_PUT) != 0);
}

void runHttpMethodTests() {
  RUN_TEST(test_named_http_method_flags_are_stable);
  RUN_TEST(test_composite_bitmasks_match_expected_methods);
}
