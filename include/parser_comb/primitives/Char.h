#ifndef PARSER_COMB_CHAR_H
#define PARSER_COMB_CHAR_H

#include <parser_comb/ParseRule.h>

#include <optional>
#include <variant>

namespace comb {

/**
 * @brief Parser that parses a singe character
 * @ingroup primitives
 *
 * This parser fails if the input is empty.
 *
 * Usage:
 * ```cpp
 * comb::any_char.parse("abc"); // ('a', "bc")
 * comb::any_char.parse(""); // std::nullopt
 * ```
 *
 */
struct AnyChar {
    using ValueType = char;

    constexpr auto parse(std::string_view input) const -> ParseResult<char> {
        if (!input.empty())
            return std::pair{input[0], input.substr(1)};
        return std::nullopt;
    }
};

/**
 * @brief A global instance of `AnyChar` parser.
 * @ingroup primitives
 */
inline constexpr AnyChar any_char;

/**
 * @brief A parser that peeks at the first character of the input without
 * consuming it.
 * @ingroup primitives
 *
 * This parser fails if the input is empty.
 *
 * Usage:
 * ```cpp
 * comb::peek_char.parse("abc"); // ('a', "abc")
 * comb::peek_char.parse(""); // std::nullopt
 * ```
 *
 */
struct PeekChar {
    using ValueType = char;

    constexpr auto parse(std ::string_view input) const -> ParseResult<char> {
        if (!input.empty())
            return std::pair{input[0], input};
        return std::nullopt;
    }
};

/**
 * @brief A global instance of `PeekChar` parser.
 * @ingroup primitives
 */
inline constexpr PeekChar peek_char;

/**
 * @brief A parser that matches a specific character.
 * @ingroup primitives
 *
 * This parser fails if the input is empty or if the first character does not
 * match the specified character.
 *
 * Usage:
 * ```cpp
 * auto p = comb::charP('a');
 * p.parse("abc"); // ('a', "bc")
 * p.parse("xbc"); // std::nullopt
 * p.parse(""); // std::nullopt
 * ```
 *
 */
struct CharP {
    using ValueType = char;

    explicit constexpr CharP(char ch) : ch(ch) {};

    constexpr auto parse(std::string_view input) const -> ParseResult<char> {
        if (!input.empty() && input[0] == ch)
            return std::pair{input[0], input.substr(1)};
        return std::nullopt;
    }

   private:
    char ch;
};

/**
 * @brief Creates a `CharP` parser that matches the specified character.
 * @ingroup primitives
 *
 * @param ch
 * @return CharP
 */
constexpr auto charP(char ch) -> CharP {
    return CharP{ch};
}

/**
 * @brief Parser that matches a character satisfying a given predicate.
 * @ingroup primitives
 *
 * The predicate must be a callable that takes a `char` and returns a `bool`.
 *
 * Using `<cctype>` predicates can be a bit of a hassle due to their overloads.
 * To use them with `satisfy`, there are `<cctype>` wrappers given in
 * `parser_comb/CType.h`. See <CType.h> for more details.
 *
 * Usage:
 * ```cpp
 * auto p = comb::satisfy(comb::isdigit);
 * p.parse("123"); // ('1', "23")
 * p.parse("abc"); // std::nullopt
 * ```
 *
 * @tparam Fn the type of the predicate callable.
 */
template <Predicate<char> Fn>
struct Satisfy {
    using ValueType = char;

    explicit constexpr Satisfy(Fn const& fn) : fn(fn) {}
    explicit constexpr Satisfy(Fn&& fn) : fn(std::move(fn)) {}

    constexpr auto parse(std::string_view input) const -> ParseResult<char> {
        if (!input.empty() && static_cast<bool>(fn(input[0])))
            return std::pair{input[0], input.substr(1)};
        return std::nullopt;
    }

   private:
    Fn fn;
};

template <typename Fn>
explicit Satisfy(Fn&&) -> Satisfy<std::remove_cvref_t<Fn>>;

/**
 * @brief Constructs a `Satisfy` parser with the given predicate.
 * @ingroup primitives
 *
 * @tparam Fn
 */
template <typename Fn>
    requires(Predicate<detail::PureT<Fn>, char>)
constexpr auto satisfy(Fn&& fn) -> Satisfy<std::remove_cvref_t<Fn>> {
    return Satisfy{std::forward<Fn>(fn)};
}

/**
 * @brief Parser that matches the end of string.
 * @ingroup primitives
 *
 * Usage:
 * ```cpp
 * comb::eof.parse(""); // (std::monostate{}, "")
 * comb::eof.parse("abc"); // std::nullopt
 * ```
 *
 */
struct Eof {
    using ValueType = std::monostate;

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        if (input.empty())
            return std::pair{std::monostate{}, input};
        return std::nullopt;
    }
};

/**
 * @brief A global instance of `Eof` parser.
 * @ingroup primitives
 */
inline constexpr auto eof = Eof{};

}  // namespace comb

#endif