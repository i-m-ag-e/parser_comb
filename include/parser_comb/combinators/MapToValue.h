#ifndef PARSER_COMB_MAP_TO_VALUE_H
#define PARSER_COMB_MAP_TO_VALUE_H

#include "parser_comb/ParseRule.h"

namespace comb {
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

template <typename P, typename T>
    requires(ParseRules<P>)
constexpr auto map_to_value(P&& p, T&& val) {
    return MapToValue<detail::PureT<P>, detail::PureT<T>>{std::forward<P>(p),
                                                          std::forward<T>(val)};
}

}  // namespace comb

#endif