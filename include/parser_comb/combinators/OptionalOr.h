#ifndef PARSER_COMB_OPTIONAL_H
#define PARSER_COMB_OPTIONAL_H

#include <parser_comb/ParseRule.h>
#include <parser_comb/combinators/Alt.h>
#include <parser_comb/primitives/Core.h>

namespace comb {
template <typename P, typename T>
    requires(ParseRules<P> && ParserOf<detail::PureT<P>, T>)
constexpr auto optional_or(P&& p, T&& default_val) {
    return comb::alt(std::forward<P>(p),
                     comb::pure(std::forward<T>(default_val)));
}
}  // namespace comb

#endif