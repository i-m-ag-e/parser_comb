#include <gtest/gtest.h>
#include <parser_comb/ParserComb.h>

#include <tuple>
#include <utility>

TEST(TupleUtilsTest, TestUtils) {
    constexpr std::tuple<int, double, char> t1{1, 2.0, 'c'};

    constexpr auto extracted = comb::ExtractIndices<0, 2>{}(t1);
    static_assert(extracted == std::make_tuple(1, 'c'),
                  "ExtractIndices failed");

    constexpr auto remaining = comb::IgnoreIndices<1>{}(t1);
    static_assert(remaining == std::make_tuple(1, 'c'), "IgnoreIndices failed");

    constexpr auto p = comb::ExtractPair<0, 2>{}(t1);
    static_assert(p.first == 1 && p.second == 'c', "ExtractPair failed");

    constexpr auto val = comb::ExtractNth<1>{}(t1);
    static_assert(val == 2.0, "ExtractNth failed");

    struct DummyStruct {
        int a;
        char b;
        constexpr DummyStruct(int a, char b) : a(a), b(b) {}
    };

    constexpr std::tuple<int, char> t2{42, 'x'};
    constexpr DummyStruct obj = comb::ConstructFromSequence<DummyStruct>{}(t2);
    static_assert(obj.a == 42 && obj.b == 'x', "ConstructFromSequence failed");

    // for GTest
    SUCCEED();
}