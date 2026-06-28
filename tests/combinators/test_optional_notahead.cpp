#include <gtest/gtest.h>
#include <parser_comb/ParserComb.h>

#include <string_view>

using std::string_view_literals::operator""sv;

TEST(OptionalCombinatorTest, Success) {
    auto p = comb::optional_or(comb::stringP("val"), "default"sv);

    auto res = p.parse("value");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, "val");
    EXPECT_EQ(res->second, "ue");
}

TEST(OptionalCombinatorTest, Failure) {
    auto p = comb::optional_or(comb::stringP("val"), "default"sv);

    auto res = p.parse("other");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, "default");
    EXPECT_EQ(res->second, "other");
}

TEST(NotAheadCombinatorTest, Succeess) {
    auto p = comb::not_ahead(comb::stringP("fail"));

    auto res = p.parse("success");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->second, "success");
}

TEST(NotAheadCombinatorTest, Failure) {
    auto p = comb::not_ahead(comb::stringP("fail"));

    EXPECT_FALSE(p.parse("fail_state").has_value());
}