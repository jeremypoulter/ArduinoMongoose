#include <unity.h>
#include <MongooseString.h>

static void test_constructs_from_char_pointer_mg_str_and_nullptr() {
  const char *text = "Hello";
  mg_str raw = mg_str_s(text);

  MongooseString fromChar(text);
  MongooseString fromPointer(&raw);
  MongooseString fromValue(raw);
  MongooseString fromNull((const char *) nullptr);

  TEST_ASSERT_EQUAL(5, fromChar.length());
  TEST_ASSERT_EQUAL(5, fromPointer.length());
  TEST_ASSERT_EQUAL(5, fromValue.length());
  TEST_ASSERT_EQUAL(0, fromNull.length());
  TEST_ASSERT_EQUAL_STRING_LEN("Hello", fromChar.c_str(), fromChar.length());
  TEST_ASSERT_NULL(fromNull.c_str());
}

static void test_equals_compare_and_safe_bool_behaviour() {
  MongooseString hello("Hello");
  MongooseString helloCopy("Hello");
  MongooseString world("world");

  TEST_ASSERT_TRUE(hello.equals("Hello"));
  TEST_ASSERT_TRUE(hello.equals(helloCopy));
  TEST_ASSERT_TRUE(hello.equalsIgnoreCase("hello"));
  TEST_ASSERT_TRUE(hello == "Hello");
  TEST_ASSERT_TRUE(hello != world);
  TEST_ASSERT_EQUAL(0, hello.compareTo("Hello"));
  TEST_ASSERT_TRUE(hello.compareTo("World") < 0);
  TEST_ASSERT_EQUAL(0, hello.compareToIgnoreCase("hello"));
  TEST_ASSERT_TRUE(hello.compareToIgnoreCase("world") < 0);

  MongooseString empty;
  TEST_ASSERT_TRUE(hello);
  TEST_ASSERT_FALSE(empty);
}

static void test_accessors_return_expected_values() {
  MongooseString value("Payload");
  mg_str raw = value.toMgStr();

  TEST_ASSERT_EQUAL(7, value.length());
  TEST_ASSERT_EQUAL_STRING_LEN("Payload", value.c_str(), value.length());
  TEST_ASSERT_EQUAL(7, raw.len);
  TEST_ASSERT_EQUAL_STRING_LEN("Payload", raw.buf, raw.len);
}

void runMongooseStringTests() {
  RUN_TEST(test_constructs_from_char_pointer_mg_str_and_nullptr);
  RUN_TEST(test_equals_compare_and_safe_bool_behaviour);
  RUN_TEST(test_accessors_return_expected_values);
}
