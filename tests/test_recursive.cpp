#include <gtest/gtest.h>
#include <parser_comb/ParserComb.h>

TEST(RecursiveTest, ParseNestedExpressions) {
    comb::RecursiveP<char> expr;

    // Grammar: expr -> '(' expr ')' | 'x'
    auto parens =
        comb::enclosed(comb::charP('('), expr.ref(), comb::charP(')'));
    expr.set(comb::alt(parens, comb::charP('x')));

    auto res1 = expr.parse("x");
    ASSERT_TRUE(res1.has_value());
    EXPECT_EQ(res1->first, 'x');
    EXPECT_TRUE(res1->second.empty());

    auto res2 = expr.parse("(((x)))");
    ASSERT_TRUE(res2.has_value());
    EXPECT_EQ(res2->first, 'x');
    EXPECT_TRUE(res2->second.empty());

    auto res3 = expr.parse("(x))");
    ASSERT_TRUE(res3.has_value());
    EXPECT_EQ(res3->first, 'x');
    EXPECT_EQ(res3->second, ")");

    auto res4 = expr.parse("x))");
    ASSERT_TRUE(res4.has_value());
    EXPECT_EQ(res4->first, 'x');
    EXPECT_EQ(res4->second, "))");

    EXPECT_FALSE(expr.parse("((x)").has_value());
    EXPECT_FALSE(expr.parse("((x").has_value());
}

TEST(RecursiveTest, FailEmptyRecursive) {
    comb::RecursiveP<int> empty_parser;

    EXPECT_FALSE(empty_parser.parse("input").has_value());
    EXPECT_FALSE(empty_parser.ref().parse("input").has_value());
}