#include <gtest/gtest.h>
#include <parser_comb/ParserComb.h>

TEST(CharPrimitiveTest, AnyChar) {
    auto res = comb::any_char.parse("abc");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, 'a');
    EXPECT_EQ(res->second, "bc");

    EXPECT_FALSE(comb::any_char.parse("").has_value());
}

TEST(CharPrimitiveTest, PeekChar) {
    auto res = comb::peek_char.parse("abc");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, 'a');
    EXPECT_EQ(res->second, "abc");

    EXPECT_FALSE(comb::peek_char.parse("").has_value());
}

TEST(CharPrimitiveTest, CharP) {
    auto p = comb::charP('x');

    auto res = p.parse("xyz");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, 'x');
    EXPECT_EQ(res->second, "yz");

    EXPECT_FALSE(p.parse("abc").has_value());
    EXPECT_FALSE(p.parse("").has_value());
}

TEST(CharPrimitiveTest, Satisfy) {
    auto p = comb::satisfy([](char c) { return c >= '0' && c <= '9'; });

    auto res = p.parse("42");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, '4');
    EXPECT_EQ(res->second, "2");

    EXPECT_FALSE(p.parse("abc").has_value());
    EXPECT_FALSE(p.parse("").has_value());
}

TEST(CharPrimitiveTest, Eof) {
    auto res = comb::eof.parse("");
    ASSERT_TRUE(res.has_value());
    EXPECT_TRUE(res->second.empty());

    EXPECT_FALSE(comb::eof.parse("abc").has_value());
}