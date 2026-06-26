#ifndef PARSER_COMB_SKIP_H
#define PARSER_COMB_SKIP_H

#include <variant>

#include "parser_comb/ParseRule.h"

namespace comb {

template <ParseRule P>
struct Skip {
    using ValueType = std::monostate;

    explicit constexpr Skip(P const& p) : parser(p) {}
    explicit constexpr Skip(P&& p) : parser(std::move(p)) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        while (const auto res = parser.parse(input)) {
            input = res->second;
        }

        return std::pair{std::monostate{}, input};
    }

   private:
    P parser;
};

template <typename P>
explicit Skip(P&&) -> Skip<detail::PureT<P>>;

template <typename P>
    requires(ParseRules<P>)
constexpr auto skip(P&& p) -> Skip<detail::PureT<P>> {
    return Skip<detail::PureT<P>>{std::forward<P>(p)};
}

template <ParseRule P, Predicate<typename P::ValueType> Fn, bool AllowZero>
struct SkipWhile {
    using ValueType = std::monostate;

    template <typename PU, typename FnU>
    explicit constexpr SkipWhile(PU&& p, FnU&& fn)
        : parser(std::forward<PU>(p)), pred(std::forward<FnU>(fn)) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        auto res = parser.parse(input);

        if (res && pred(res->first)) {
            do {
                input = res->second;
            } while ((res = parser.parse(input)) && pred(res->first));
        } else if constexpr (!AllowZero) {
            return std::nullopt;
        }
        return std::pair{std::monostate{}, input};
    }

   private:
    P parser;
    Fn pred;
};

template <typename P, typename Fn>
    requires(ParseRules<P> &&
             Predicate<detail::PureT<Fn>, detail::ValueOfParser<P>>)
constexpr auto skip_while0(P&& p, Fn&& fn) {
    return SkipWhile<detail::PureT<P>, detail::PureT<Fn>, true>{
        std::forward<P>(p), std::forward<Fn>(fn)};
}

template <typename P, typename Fn>
    requires(ParseRules<P> &&
             Predicate<detail::PureT<Fn>, detail::ValueOfParser<P>>)
constexpr auto skip_while1(P&& p, Fn&& fn) {
    return SkipWhile<detail::PureT<P>, detail::PureT<Fn>, false>{
        std::forward<P>(p), std::forward<Fn>(fn)};
}

}  // namespace comb

#endif