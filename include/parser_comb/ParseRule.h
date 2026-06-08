#ifndef PARSER_COMB_PARSE_RULE_H
#define PARSER_COMB_PARSE_RULE_H

#include <concepts>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

namespace comb {

template <typename T>
using ParseResult = std::optional<std::pair<T, std::string_view>>;

template <typename Fn, typename... Args>
concept Predicate = requires(Fn&& fn, Args&&... args) {
    {
        std::forward<Fn>(fn)(std::forward<Args>(args)...)
    } -> std::convertible_to<bool>;
};

template <typename P>
concept ParseRule = requires(P const& p, std::string_view s) {
    typename P::ValueType;
    { p.parse(s) } -> std::same_as<ParseResult<typename P::ValueType>>;
};

template <typename P, typename T>
concept ParserOf = ParseRule<P> && std::same_as<T, typename P::ValueType>;

namespace detail {
template <typename P>
using PureT = std::remove_cvref_t<P>;

template <typename P>
    requires(ParseRule<PureT<P>>)
using ValueOfParser = PureT<P>::ValueType;

}  // namespace detail

template <typename... Ps>
concept ParseRules = (ParseRule<detail::PureT<Ps>> && ...);

}  // namespace comb

#endif