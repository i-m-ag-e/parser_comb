#ifndef PARSER_COMB_WHITESPACE_H
#define PARSER_COMB_WHITESPACE_H

#include <parser_comb/CType.h>
#include <parser_comb/ParseRule.h>
#include <parser_comb/combinators/Sequence.h>
#include <parser_comb/combinators/Skip.h>
#include <parser_comb/primitives/Char.h>
#include <parser_comb/primitives/String.h>

namespace comb {

constexpr auto whitespace = comb::satisfy(comb::isspace);

template <bool CR = false>
constexpr auto line_ending() -> ParserOf<std::string_view> auto {
    if constexpr (!CR)
        return comb::stringP("\n");
    else
        return comb::stringP("\r\n");
}

constexpr auto skip_whitespace = comb::skip(comb::whitespace);

template <typename P>
    requires(ParseRules<P>)
constexpr auto lexeme(P&& p)
    -> ParserOf<typename detail::PureT<P>::ValueType> auto {
    return comb::enclosed(comb::skip_whitespace, std::forward<P>(p),
                          comb::skip_whitespace);
}

}  // namespace comb

#endif