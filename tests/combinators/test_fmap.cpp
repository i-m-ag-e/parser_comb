#include <gtest/gtest.h>
#include <parser_comb/ParserComb.h>

#include <cctype>

TEST(FMapCombinatorTest, CharTransform) {
    auto p =
        comb::fmap([](char c) { return static_cast<char>(std::toupper(c)); },
                   comb::any_char);

    auto res = p.parse("abc");
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res->first, 'A');
    EXPECT_EQ(res->second, "bc");
}

TEST(FMapCombinatorTest, TypeChange) {
    auto p = comb::fmap([](char c) { return c == 't'; }, comb::charP('t'));

    auto res = p.parse("true");
    ASSERT_TRUE(res.has_value());
    EXPECT_TRUE(res->first);
    EXPECT_EQ(res->second, "rue");
}

TEST(FMapCombinatorTest, Failure) {
    auto p = fmap([](char c) { return static_cast<char>(std::toupper(c)); },
                  comb::charP('x'));

    EXPECT_FALSE(p.parse("abc").has_value());
    EXPECT_FALSE(p.parse("").has_value());
}