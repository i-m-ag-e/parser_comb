#include <JsonParser.h>
#include <JsonValue.h>
#include <gtest/gtest.h>

using namespace json_parser;

TEST(JsonParserTest, ParsePrimitives) {
    auto t = parse_json("true");
    ASSERT_TRUE(t.has_value());
    EXPECT_TRUE(t->is_bool());
    EXPECT_EQ(t->get<bool>(), true);

    auto f = parse_json("false");
    ASSERT_TRUE(f.has_value());
    EXPECT_TRUE(f->is_bool());
    EXPECT_EQ(f->get<bool>(), false);

    auto n = parse_json("null");
    ASSERT_TRUE(n.has_value());
    EXPECT_TRUE(n->is_null());

    auto num = parse_json("123.45");
    ASSERT_TRUE(num.has_value());
    EXPECT_TRUE(num->is_number());
    EXPECT_DOUBLE_EQ(num->get<double>(), 123.45);

    auto str = parse_json("\"hello\\nworld\"");
    ASSERT_TRUE(str.has_value());
    EXPECT_TRUE(str->is_string());
    EXPECT_EQ(str->get<std::string>(), "hello\nworld");
}

TEST(JsonParserTest, ParseArrays) {
    auto res = parse_json("[1, \"two\", true, null]");
    ASSERT_TRUE(res.has_value());
    EXPECT_TRUE(res->is_array());

    auto arr = res->get<JsonArray>();
    ASSERT_EQ(arr.size(), 4);
    EXPECT_DOUBLE_EQ(arr[0].get<double>(), 1.0);
    EXPECT_EQ(arr[1].get<std::string>(), "two");
    EXPECT_EQ(arr[2].get<bool>(), true);
    EXPECT_TRUE(arr[3].is_null());
}

TEST(JsonParserTest, ParseObjects) {
    auto res = parse_json(R"({"key1": "value", "key2": 42})");
    ASSERT_TRUE(res.has_value());
    EXPECT_TRUE(res->is_object());

    auto obj = res->get<JsonObject>();
    ASSERT_EQ(obj.size(), 2);
    EXPECT_EQ(obj["key1"].get<std::string>(), "value");
    EXPECT_DOUBLE_EQ(obj["key2"].get<double>(), 42.0);
}

TEST(JsonParserTest, FailsOnMalformedJson) {
    EXPECT_FALSE(parse_json("[1, 2,").has_value());
    EXPECT_FALSE(parse_json("{\"key\": }").has_value());
    EXPECT_FALSE(parse_json("\"unclosed string").has_value());
}