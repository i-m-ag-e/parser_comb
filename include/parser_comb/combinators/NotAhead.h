#ifndef PARSER_COMB_NOT_AHEAD_H
#define PARSER_COMB_NOT_AHEAD_H

#include <optional>
#include <string_view>
#include <variant>

#include "parser_comb/ParseRule.h"

namespace comb {

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

template <typename P>
    requires(ParseRules<P>)
constexpr auto not_ahead(P&& p) -> NotAhead<detail::PureT<P>> {
    return NotAhead{std::forward<P>(p)};
}

}  // namespace comb

#endif