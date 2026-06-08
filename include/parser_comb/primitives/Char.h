#ifndef PARSER_COMB_CHAR_H
#define PARSER_COMB_CHAR_H

#include <parser_comb/ParseRule.h>

#include <optional>
#include <variant>

namespace comb {

struct AnyChar {
    using ValueType = char;

    constexpr auto parse(std::string_view input) const -> ParseResult<char> {
        if (!input.empty())
            return std::pair{input[0], input.substr(1)};
        return std::nullopt;
    }
};

constexpr AnyChar any_char;

struct PeekChar {
    using ValueType = char;

    constexpr auto parse(std ::string_view input) const -> ParseResult<char> {
        if (!input.empty())
            return std::pair{input[0], input};
        return std::nullopt;
    }
};

constexpr PeekChar peek_char;

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

constexpr auto charP(char ch) -> CharP {
    return CharP{ch};
}

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

template <typename Fn>
    requires(Predicate<detail::PureT<Fn>, char>)
constexpr auto satisfy(Fn&& fn) -> Satisfy<std::remove_cvref_t<Fn>> {
    return Satisfy{std::forward<Fn>(fn)};
}

struct Eof {
    using ValueType = std::monostate;

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        if (input.empty())
            return std::pair{std::monostate{}, input};
        return std::nullopt;
    }
};

constexpr auto eof = Eof{};

}  // namespace comb

#endif