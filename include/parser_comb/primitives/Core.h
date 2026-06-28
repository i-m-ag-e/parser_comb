#ifndef PARSER_COMB_CORE_H
#define PARSER_COMB_CORE_H

#include <parser_comb/ParseRule.h>

namespace comb {

/**
 * @brief Parser that always succeeds and returns the provided value without
 * consuming any input.
 * @ingroup primitives
 *
 * Usage:
 * ```cpp
 * comb::pure(42).parse("abc"); // (42, "abc")
 * comb::pure(std::string("hello")).parse(""); // ("hello", "")
 * ```
 *
 * @tparam T
 */
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

/**
 * @brief Constructs a `Pure<T>(value)`
 * @ingroup primitives
 *
 * @tparam T
 * @param value
 * @return Pure<T>
 */
template <typename T>
constexpr auto pure(T&& value) -> Pure<T> {
    return Pure{std::forward<T>(value)};
}

/**
 * @brief Parser that always fails
 * @ingroup primitives
 *
 * @tparam T
 */
template <typename T>
struct Empty {
    using ValueType = T;

    constexpr auto parse(std::string_view const&) const -> ParseResult<T> {
        return std::nullopt;
    }
};

/**
 * @brief Constructs a `Empty<T>`
 * @ingroup primitives
 *
 * @tparam T
 * @return Empty<T>
 */
template <typename T>
constexpr auto empty() -> Empty<T> {
    return {};
}

}  // namespace comb

#endif