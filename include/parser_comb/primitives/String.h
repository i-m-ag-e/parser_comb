#ifndef PARSER_COMB_STRING_H
#define PARSER_COMB_STRING_H

#include <parser_comb/ParseRule.h>

#include <string_view>

namespace comb {

/**
 * @brief Parser that matches a specific string literal.
 * @ingroup primitives
 *
 * Usage:
 * ```cpp
 * auto p = comb::stringP("hello");
 * p.parse("hello world"); // ("hello", " world")
 * p.parse("hi there"); // std::nullopt
 * p.parse(""); // std::nullopt
 * ```
 *
 */
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

/**
 * @brief constructs a `StringP` with the given string view
 * @ingroup primitives
 *
 * @param str
 * @return StringP
 */
constexpr auto stringP(std::string_view str) -> StringP {
    return StringP{str};
}

/**
 * @brief Parser that consumes characters while they match a predicate,
 * returning the matched substring.
 * @ingroup primitives
 *
 * Usage given with construction functions `take_char_while0` and
 * `take_char_while1`.
 *
 * @tparam Fn the predicate function type ((char) -> bool)
 * @tparam AllowEmpty allows the parser to succeed even if no characters match
 * the predicate.
 */
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

/**
 * @brief constructs a `TakeCharWhile<Fn, true>`
 * @ingroup primitives
 *
 * Note: this function is different from `take_while0`. The latter consumes
 * another parser passed to it, and produces a std::vector<ValueType>. For
 * characters, this function is more efficient, since no allocation occurs, and
 * a direct substring of the input is returned.
 *
 * Usage:
 * ```cpp
 * auto p = comb::take_char_while0(comb::isdigit);
 * p.parse("123abc"); // ("123", "abc")
 * p.parse("abc"); // ("", "abc")
 * ```
 *
 * @tparam Fn
 */
template <typename Fn>
    requires(Predicate<detail::PureT<Fn>, char>)
constexpr auto take_char_while0(Fn&& fn) {
    return TakeCharWhile<detail::PureT<Fn>, true>{std::forward<Fn>(fn)};
}

/**
 * @brief constructs a `TakeCharWhile<Fn, false>`
 * @ingroup primitives
 *
 * Usage:
 * ```cpp
 * auto p = comb::take_char_while1(comb::isdigit);
 * p.parse("123abc"); // ("123", "abc")
 * p.parse("abc"); // std::nullopt
 * ```
 *
 * @tparam Fn
 */
template <typename Fn>
    requires(Predicate<detail::PureT<Fn>, char>)
constexpr auto take_char_while1(Fn&& fn) {
    return TakeCharWhile<detail::PureT<Fn>, false>{std::forward<Fn>(fn)};
}

}  // namespace comb

#endif