#ifndef PARSER_COMB_CORE_H
#define PARSER_COMB_CORE_H

#include <parser_comb/ParseRule.h>

namespace comb {

template <typename T>
struct Pure {
    using ValueType = T;

    explicit constexpr Pure(T const& val) : value(val) {}
    explicit constexpr Pure(T&& val) : value(std::move(val)) {}

    constexpr auto parse(std::string_view input) const -> ParseResult<T> {
        return std::pair{value, input};
    }

   private:
    T value;
};

template <typename T>
explicit Pure(T&&) -> Pure<std::remove_cvref_t<T>>;

template <typename T>
constexpr auto pure(T&& value) -> Pure<T> {
    return Pure{std::forward<T>(value)};
}

template <typename T>
struct Empty {
    using ValueType = T;

    constexpr auto parse(std::string_view const&) const -> ParseResult<T> {
        return std::nullopt;
    }
};

template <typename T>
constexpr auto empty() -> Empty<T> {
    return {};
}

}  // namespace comb

#endif