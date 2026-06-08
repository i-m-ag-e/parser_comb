#ifndef PARSER_COMB_STRING_H
#define PARSER_COMB_STRING_H

#include <parser_comb/ParseRule.h>

#include <string_view>

namespace comb {

struct StringP {
    using ValueType = std::string_view;

    explicit constexpr StringP(std::string_view str) : str(str) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        if (input.starts_with(str)) {
            return std::pair{str, input.substr(str.length())};
        }
        return std::nullopt;
    }

   private:
    std::string_view str;
};

constexpr auto stringP(std::string_view str) -> StringP {
    return StringP{str};
}

template <Predicate<char> Fn, bool AllowEmpty>
struct TakeCharWhile {
    using ValueType = std::string_view;

    explicit constexpr TakeCharWhile(Fn fn) : pred(std::move(fn)) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        size_t i = 0;
        while (i < input.size() && pred(input[i])) {
            ++i;
        }
        if (!AllowEmpty && i == 0) {
            return std::nullopt;
        }
        return std::pair{input.substr(0, i), input.substr(i)};
    }

   private:
    Fn pred;
};

template <typename Fn>
    requires(Predicate<detail::PureT<Fn>, char>)
constexpr auto take_char_while0(Fn&& fn) {
    return TakeCharWhile<detail::PureT<Fn>, true>{std::forward<Fn>(fn)};
}

template <typename Fn>
    requires(Predicate<detail::PureT<Fn>, char>)
constexpr auto take_char_while1(Fn&& fn) {
    return TakeCharWhile<detail::PureT<Fn>, false>{std::forward<Fn>(fn)};
}

}  // namespace comb

#endif