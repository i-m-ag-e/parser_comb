#ifndef PARSER_COMB_MAP_TO_VALUE_H
#define PARSER_COMB_MAP_TO_VALUE_H

#include "parser_comb/ParseRule.h"

namespace comb {

// clang-format off
/**
 * @brief Parser that always returns a constant on success
 * @ingroup combinators
 *
 * The parser executes the internal parser `P`, if it succeeds, returns the
 * constant passed during construction.
 *
 * Usage:
 * ```cpp
 * auto p = comb::map_to_value(comb::stringP("true"), true);
 * auto r1 = p.parse("true;"); // (true, ";");
 * auto r2 = p.parse("false"); // std::nullopt
 * ```
 *
 * @tparam P  the internal parser
 * @tparam T  the type of constant returned on success
 */

// clang-format on
template <ParseRule P, typename T>
struct MapToValue {
    using ValueType = T;

    template <typename Q, typename U>
    explicit constexpr MapToValue(Q&& p, U&& val)
        : parser(std::forward<Q>(p)), value(std::forward<U>(val)){};

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        if (const auto res = parser.parse(input)) {
            return std::pair{value, res->second};
        }
        return std::nullopt;
    }

   private:
    P parser;
    T value;
};

/**
 * @brief helper to construct `MapToValue` from parser and constant
 * @ingroup combinators
 *
 * @tparam P
 * @tparam T
 */
template <typename P, typename T>
    requires(ParseRules<P>)
constexpr auto map_to_value(P&& p, T&& val) {
    return MapToValue<detail::PureT<P>, detail::PureT<T>>{std::forward<P>(p),
                                                          std::forward<T>(val)};
}

}  // namespace comb

#endif