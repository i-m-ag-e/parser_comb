#include <gtest/gtest.h>
#include <parser_comb/ParserComb.h>

TEST(StringPrimitiveTest, StringP) {
    auto p = comb::stringP("hello");

    auto res = p.parse("hello world");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, "hello");
    EXPECT_EQ(res->second, " world");

    EXPECT_FALSE(p.parse("hell").has_value());
    EXPECT_FALSE(p.parse("world").has_value());
    EXPECT_FALSE(p.parse("").has_value());
}

TEST(StringPrimitiveTest, TakeCharWhile0) {
    auto p = comb::take_char_while0([](char c) { return c == 'a'; });

    auto res1 = p.parse("aaab");
    ASSERT_TRUE(res1.has_value());
    EXPECT_EQ(res1->first, "aaa");
    EXPECT_EQ(res1->second, "b");

    auto res2 = p.parse("bbba");
    ASSERT_TRUE(res2.has_value());
    EXPECT_EQ(res2->first, "");
    EXPECT_EQ(res2->second, "bbba");

    auto res3 = p.parse("");
    ASSERT_TRUE(res3.has_value());
    EXPECT_EQ(res3->first, "");
}

TEST(StringPrimitiveTest, TakeCharWhile1) {
    auto p = comb::take_char_while1([](char c) { return c == 'a'; });

    auto res1 = p.parse("aaab");
    ASSERT_TRUE(res1.has_value());
    EXPECT_EQ(res1->first, "aaa");
    EXPECT_EQ(res1->second, "b");

    EXPECT_FALSE(p.parse("bbba").has_value());
    EXPECT_FALSE(p.parse("").has_value());
}