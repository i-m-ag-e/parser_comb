#include <gtest/gtest.h>
#include <parser_comb/ParserComb.h>

#include <tuple>

TEST(SequenceCombinatorTest, SeqTuple) {
    auto p = comb::seq(comb::charP('a'), comb::charP('b'), comb::charP('c'));

    auto res = p.parse("abcd");
    ASSERT_TRUE(res.has_value());

    EXPECT_EQ(res->first, std::make_tuple('a', 'b', 'c'));
    EXPECT_EQ(res->second, "d");

    EXPECT_FALSE(p.parse("ab").has_value());
}

TEST(SequenceCombinatorTest, Left) {
    auto p = comb::left(comb::stringP("value"), comb::charP(';'));

    auto res = p.parse("value;next");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, "value");
    EXPECT_EQ(res->second, "next");
}

TEST(SequenceCombinatorTest, RightKeepsSecond) {
    auto p = comb::right(comb::stringP("return "), comb::stringP("0"));

    auto res = p.parse("return 0;");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, "0");
    EXPECT_EQ(res->second, ";");
}

TEST(SequenceCombinatorTest, Enclosed) {
    auto p =
        comb::enclosed(comb::charP('('), comb::charP('x'), comb::charP(')'));

    auto res = p.parse("(x)y");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, 'x');
    EXPECT_EQ(res->second, "y");

    EXPECT_FALSE(p.parse("(x").has_value());
    EXPECT_FALSE(p.parse("x)").has_value());
    EXPECT_FALSE(p.parse("()").has_value());
}