#ifndef PARSER_COMB_OPTIONAL_H
#define PARSER_COMB_OPTIONAL_H

#include <parser_comb/ParseRule.h>
#include <parser_comb/combinators/Alt.h>
#include <parser_comb/primitives/Core.h>

namespace comb {

// clang-format off
/**
 * @brief Combinator that returns a default fallback on failure
 * @ingroup combinators
 * 
 * Internally implemented as `comb::alt(p, comb::pure(def))`, where `p` is the parser and `def` is the fallback value.
 * Therefore, `d` must have the same type as the type the parser produces.
 *
 * Usage:
 * ```cpp
 * auto p = comb::optional_or(comb::map_to_value(comb::stringP("true"), true), false);
 * auto r1 = p.parse("true"); // (true, "")
 * auto r2 = p.parse("123;"); // (false, "123;")
 * ```
 *
 * @tparam P the type of parser
 * @tparam T the type of fallback value
 */
// clang-format off
template <typename P, typename T>
    requires(ParseRules<P> && ParserOf<detail::PureT<P>, T>)
constexpr auto optional_or(P&& p, T&& default_val) {
    return comb::alt(std::forward<P>(p),
                     comb::pure(std::forward<T>(default_val)));
}
}  // namespace comb

#endif