#include <gtest/gtest.h>
#include <parser_comb/ParserComb.h>

TEST(SkipCombinatorTest, Skip) {
    auto p = comb::skip(comb::charP('a'));

    auto res1 = p.parse("aaab");
    ASSERT_TRUE(res1.has_value());
    EXPECT_EQ(res1->second, "b");

    auto res2 = p.parse("b");
    ASSERT_TRUE(res2.has_value());
    EXPECT_EQ(res2->second, "b");
}

TEST(SkipCombinatorTest, SkipWhile0) {
    auto p = comb::skip_while0(comb::any_char, comb::isdigit);

    auto res1 = p.parse("123b");
    ASSERT_TRUE(res1.has_value());
    EXPECT_EQ(res1->second, "b");

    EXPECT_TRUE(p.parse("b").has_value());
}

TEST(SkipCombinatorTest, SkipWhile1) {
    auto p = comb::skip_while1(comb::any_char, comb::isdigit);

    auto res1 = p.parse("123b");
    ASSERT_TRUE(res1.has_value());
    EXPECT_EQ(res1->second, "b");

    EXPECT_FALSE(p.parse("b").has_value());
}

TEST(WhitespaceTest, Lexeme) {
    auto p = comb::lexeme(comb::stringP("test"));

    auto res = p.parse("   \t\n test \r\n  next");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, "test");
    EXPECT_EQ(res->second, "next");
}