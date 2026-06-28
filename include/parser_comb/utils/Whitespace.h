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

/**
 * @brief Parser that matches a line ending, either `\n` or `\r\n`.
 * @ingroup primitives
 *
 * @tparam CR flag indicating whether to match `\r\n` (true) or just `\n`
 * (false)
 * @return ParserOf<std::string_view> auto
 */
template <bool CR = false>
constexpr auto line_ending() -> ParserOf<std::string_view> auto {
    if constexpr (!CR)
        return comb::stringP("\n");
    else
        return comb::stringP("\r\n");
}

/**
 * @brief Parser that skips whitespace characters, returning the unconsumed
 * input.
 * @ingroup combinators
 */
constexpr auto skip_whitespace = comb::skip(comb::whitespace);

/**
 * @brief Parser that skips whitespace around another parser, returning the
 * result of the inner parser.
 * @ingroup combinators
 *
 * Usage:
 * ```cpp
 * auto p = comb::lexeme(comb::stringP("hello"));
 * p.parse("   hello   world"); // ("hello", "world")
 * ```
 *
 * @tparam P
 */
template <typename P>
    requires(ParseRules<P>)
constexpr auto lexeme(P&& p)
    -> ParserOf<typename detail::PureT<P>::ValueType> auto {
    return comb::enclosed(comb::skip_whitespace, std::forward<P>(p),
                          comb::skip_whitespace);
}

}  // namespace comb

#endif