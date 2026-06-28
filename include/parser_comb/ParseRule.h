#ifndef PARSER_COMB_PARSE_RULE_H
#define PARSER_COMB_PARSE_RULE_H

#include <concepts>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

namespace comb {

/**
 * @brief Resulting type of all parsers
 *
 * Currently, there is no support for having error messages or maintaining
 * states across parsers. So, the parsers just return a `std::nullopt` on
 * failure, or a `T` wrapped in `std::optional` on success.
 *
 * @tparam T
 */
template <typename T>
using ParseResult = std::optional<std::pair<T, std::string_view>>;

/**
 * @brief Constrains a type to be callable with the specified arguments,
 * returning a boolean.
 *
 * @tparam Fn The callable type.
 * @tparam Args The argument types passed to the callable.
 */
template <typename Fn, typename... Args>
concept Predicate = requires(Fn&& fn, Args&&... args) {
    {
        std::forward<Fn>(fn)(std::forward<Args>(args)...)
    } -> std::convertible_to<bool>;
};

/**
 * @brief Defines the core requirements for a parser combinator.
 *
 * @details To satisfy `ParseRule`, a type must provide:
 * - A `ValueType` alias indicating the type of the parsed AST node or
 * primitive.
 * - A `parse(std::string_view)` method.
 * * Semantically, the `parse` method must attempt to match the prefix of the
 * input. On success, it returns a pair containing the parsed value and the
 * unconsumed suffix of the string. On failure, it must safely return
 * `std::nullopt` without throwing.
 *
 * @tparam P The type being constrained.
 */
template <typename P>
concept ParseRule = requires(P const& p, std::string_view s) {
    typename P::ValueType;
    { p.parse(s) } -> std::same_as<ParseResult<typename P::ValueType>>;
};

/**
 * @brief Refines `ParseRule` by strictly enforcing the output type.
 * * Useful for type-erasure or when explicitly constraining a combinator (like
 * `map` or `alt`) to yield a specific AST node type.
 *
 * @tparam P The parser type.
 * @tparam T The exact value type required.
 */
template <typename P, typename T>
concept ParserOf = ParseRule<P> && std::same_as<T, typename P::ValueType>;

namespace detail {
template <typename P>
using PureT = std::remove_cvref_t<P>;

template <typename P>
    requires(ParseRule<PureT<P>>)
using ValueOfParser = PureT<P>::ValueType;

}  // namespace detail

/**
 * @brief Checks if all `Ps...` are ParseRules
 *
 * @tparam Ps
 */
template <typename... Ps>
concept ParseRules = (ParseRule<detail::PureT<Ps>> && ...);

}  // namespace comb

#endif