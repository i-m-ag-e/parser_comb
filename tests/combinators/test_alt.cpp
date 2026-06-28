#include <gtest/gtest.h>
#include <parser_comb/ParserComb.h>

TEST(AltCombinatorTest, Success) {
    auto p = alt(comb::charP('a'), comb::charP('b'));

    auto res = p.parse("abc");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, 'a');
    EXPECT_EQ(res->second, "bc");

    res = p.parse("bc");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, 'b');
    EXPECT_EQ(res->second, "c");
}

TEST(AltCombinatorTest, AllFail) {
    auto p = alt(comb::charP('a'), comb::charP('b'));

    EXPECT_FALSE(p.parse("xyz").has_value());
    EXPECT_FALSE(p.parse("").has_value());
}

TEST(AltCombinatorTest, MultipleCombinators) {
    auto p =
        alt(comb::stringP("foo"), comb::stringP("bar"), comb::stringP("baz"));

    auto res = p.parse("bazqux");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, "baz");
    EXPECT_EQ(res->second, "qux");

    res = p.parse("barqux");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, "bar");
    EXPECT_EQ(res->second, "qux");

    res = p.parse("fooqux");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, "foo");
    EXPECT_EQ(res->second, "qux");

    EXPECT_FALSE(p.parse("abcdefg").has_value());
}