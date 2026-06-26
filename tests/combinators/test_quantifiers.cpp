#include <gtest/gtest.h>
#include <parser_comb/ParserComb.h>

#include <vector>

#include "parser_comb/combinators/Quantifiers.h"
#include "parser_comb/primitives/Char.h"
#include "parser_comb/primitives/String.h"

TEST(QuantifierTest, ZeroOrMore) {
    auto p = comb::zero_or_more(comb::charP('a'));

    auto res1 = p.parse("aaab");
    ASSERT_TRUE(res1.has_value());
    EXPECT_EQ(res1->first, (std::vector<char>{'a', 'a', 'a'}));
    EXPECT_EQ(res1->second, "b");

    auto res2 = p.parse("b");
    ASSERT_TRUE(res2.has_value());
    EXPECT_TRUE(res2->first.empty());
    EXPECT_EQ(res2->second, "b");
}

TEST(QuantifierTest, OneOrMore) {
    auto p = comb::one_or_more(comb::charP('a'));

    auto res1 = p.parse("aaab");
    ASSERT_TRUE(res1.has_value());
    EXPECT_EQ(res1->first, (std::vector<char>{'a', 'a', 'a'}));

    EXPECT_FALSE(p.parse("b").has_value());
}

TEST(QuantifierTest, SeparatedBy0) {
    auto p = comb::separated_by0(comb::charP('a'), comb::charP(','));

    auto res1 = p.parse("a,a,ab");
    ASSERT_TRUE(res1.has_value());
    EXPECT_EQ(res1->first, (std::vector<char>{'a', 'a', 'a'}));
    EXPECT_EQ(res1->second, "b");

    auto res2 = p.parse("a,a,");
    ASSERT_TRUE(res2.has_value());
    EXPECT_EQ(res2->first, (std::vector<char>{'a', 'a'}));
    EXPECT_EQ(res2->second, ",");

    auto res3 = p.parse("b");
    ASSERT_TRUE(res3.has_value());
    EXPECT_TRUE(res3->first.empty());
}

TEST(QuantifierTest, SeparatedBy1) {
    auto p = comb::separated_by0(comb::charP('a'), comb::charP(','));

    auto res1 = p.parse("a,a,ab");
    ASSERT_TRUE(res1.has_value());
    EXPECT_EQ(res1->first, (std::vector<char>{'a', 'a', 'a'}));
    EXPECT_EQ(res1->second, "b");

    auto res2 = p.parse("a,a,");
    ASSERT_TRUE(res2.has_value());
    EXPECT_EQ(res2->first, (std::vector<char>{'a', 'a'}));
    EXPECT_EQ(res2->second, ",");

    auto res3 = p.parse("b");
    ASSERT_TRUE(res3.has_value());
    EXPECT_TRUE(res3->first.empty());
}

TEST(QuantifierTest, NestedQuantifier) {
    auto p = comb::separated_by0(
        comb::separated_by0(comb::take_char_while0(comb::isdigit),
                            comb::charP(',')),
        comb::charP(';'));

    auto res = p.parse("12,13,14;100,101,102;123");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first,
              (std::vector<std::vector<std::string_view>>{
                  {"12", "13", "14"}, {"100", "101", "102"}, {"123"}}));
    EXPECT_EQ(res->second, "");

    res = p.parse("12,13,14");
    EXPECT_EQ(res->first,
              (std::vector<std::vector<std::string_view>>{{"12", "13", "14"}}));
    EXPECT_EQ(res->second, "");

    res = p.parse("12;13;14");
    EXPECT_EQ(res->first, (std::vector<std::vector<std::string_view>>{
                              {"12"}, {"13"}, {"14"}}));
    EXPECT_EQ(res->second, "");
}