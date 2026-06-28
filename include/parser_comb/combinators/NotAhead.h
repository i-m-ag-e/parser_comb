#ifndef PARSER_COMB_NOT_AHEAD_H
#define PARSER_COMB_NOT_AHEAD_H

#include <optional>
#include <string_view>
#include <variant>

#include "parser_comb/ParseRule.h"

namespace comb {

/**
 * @brief Parser that asserts that the given parser **fails** on the given input
 * @ingroup combinators
 *
 * Usage:
 * ```cpp
 * auto p = comb::not_ahead(comb::charP(';'));
 * auto r1 = p.parse("abcd"); // (std::monostate, "abcd")
 * auto r2 = p.parse(";bcd"); // std::nullopt
 * ```
 *
 * @tparam P the type of internal parser
 */
template <ParseRule P>
struct NotAhead {
    using ValueType = std::monostate;

    explicit constexpr NotAhead(P const& p) : parser(p) {}
    explicit constexpr NotAhead(P&& p) : parser(std::move(p)) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        if (parser.parse(input))
            return std::nullopt;
        return std::pair{std::monostate{}, input};
    }

   private:
    P parser;
};

template <ParseRule P>
explicit NotAhead(P const&) -> NotAhead<P>;
template <ParseRule P>
explicit NotAhead(P&&) -> NotAhead<P>;

/**
 * @brief helper that constructs `NotAhead<P>` from the given parser
 * @ingroup combinators
 *
 * @tparam P
 */
template <typename P>
    requires(ParseRules<P>)
constexpr auto not_ahead(P&& p) -> NotAhead<detail::PureT<P>> {
    return NotAhead{std::forward<P>(p)};
}

}  // namespace comb

#endif