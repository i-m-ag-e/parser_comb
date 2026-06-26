#include <gtest/gtest.h>
#include <parser_comb/ParserComb.h>

TEST(CorePrimitiveTest, Pure) {
    auto p = comb::pure(42);

    auto res = p.parse("input");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, 42);
    EXPECT_EQ(res->second, "input");

    auto res_empty = p.parse("");
    ASSERT_TRUE(res_empty.has_value());
    EXPECT_EQ(res_empty->first, 42);
    EXPECT_TRUE(res_empty->second.empty());
}

TEST(CorePrimitiveTest, Empty) {
    auto p = comb::empty<std::string>();

    EXPECT_FALSE(p.parse("input").has_value());
    EXPECT_FALSE(p.parse("").has_value());
}