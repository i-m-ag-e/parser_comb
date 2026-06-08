#ifndef PARSER_COMB_MANY_H
#define PARSER_COMB_MANY_H

#include <vector>

#include "parser_comb/ParseRule.h"
#include "parser_comb/combinators/Sequence.h"

namespace comb {

template <ParseRule P, bool AllowZero>
struct Many {
    using ValueType = std::vector<typename P::ValueType>;

    explicit constexpr Many(P const& p) : parser(p) {};
    explicit constexpr Many(P&& p) : parser(std::move(p)) {};

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        ValueType parsed_values;

        auto res = parser.parse(input);
        if constexpr (!AllowZero) {
            if (!res)
                return std::nullopt;
        }

        if (res) {
            do {
                parsed_values.push_back(std::move(res->first));
                input = res->second;
            } while ((res = parser.parse(input)));
        }
        return std::pair{parsed_values, input};
    }

   private:
    P parser;
};

template <typename P>
    requires(ParseRule<detail::PureT<P>>)
constexpr auto zero_or_more(P&& p) -> Many<detail::PureT<P>, true> {
    return Many<detail::PureT<P>, true>{std::forward<P>(p)};
}

template <typename P>
    requires(ParseRule<detail::PureT<P>>)
constexpr auto one_or_more(P&& p) -> Many<detail::PureT<P>, false> {
    return Many<detail::PureT<P>, false>{std::forward<P>(p)};
}

template <ParseRule P, ParseRule Delim, bool AllowZero>
struct DelimitedSequence {
    using ValueType = std::vector<typename P::ValueType>;

    template <typename PU, typename DelimU>
    explicit constexpr DelimitedSequence(PU&& p, DelimU&& d)
        : parser(std::forward<PU>(p)), delimiter(std::forward<DelimU>(d)) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        ValueType parsed_values;

        auto res = parser.parse(input);
        if constexpr (!AllowZero) {
            if (!res)
                return std::nullopt;
        }

        const auto comb_parser = comb::right(delimiter, parser);

        if (res) {
            do {
                parsed_values.push_back(std::move(res->first));
                input = res->second;
            } while ((res = comb_parser.parse(input)));
        }
        return std::pair{parsed_values, input};
    }

   private:
    P parser;
    Delim delimiter;
};

template <typename P, typename Delim>
    requires(ParseRules<P, Delim>)
constexpr auto separated_by0(P&& p, Delim&& delim)
    -> DelimitedSequence<detail::PureT<P>, detail::PureT<Delim>, true> {
    return DelimitedSequence<detail::PureT<P>, detail::PureT<Delim>, true>{
        std::forward<P>(p), std::forward<Delim>(delim)};
}

template <typename P, typename Delim>
    requires(ParseRules<P, Delim>)
constexpr auto separated_by1(P&& p, Delim&& delim)
    -> DelimitedSequence<detail::PureT<P>, detail::PureT<Delim>, false> {
    return DelimitedSequence<detail::PureT<P>, detail::PureT<Delim>, false>{
        std::forward<P>(p), std::forward<Delim>(delim)};
}

template <ParseRule P, Predicate<typename P::ValueType> Fn, bool AllowZero>
struct TakeWhile {
    using ValueType = std::vector<typename P::ValueType>;

    template <typename PU, typename FnU>
    explicit constexpr TakeWhile(PU&& p, FnU&& fn)
        : parser(std::forward<PU>(p)), pred(std::forward<FnU>(fn)) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        ValueType parsed_values;

        auto res = parser.parse(input);

        if (res && pred(res->first)) {
            do {
                parsed_values.push_back(std::move(res->first));
                input = res->second;
            } while ((res = parser.parse(input)) && pred(res->first));
        } else if constexpr (!AllowZero) {
            return std::nullopt;
        }

        return std::pair{parsed_values, input};
    }

   private:
    P parser;
    Fn pred;
};

template <typename P, typename Fn>
    requires(ParseRules<P> &&
             Predicate<detail::PureT<Fn>, typename P::ValueType>)
constexpr auto take_while0(P&& p, Fn&& fn) {
    return TakeWhile<detail::PureT<P>, detail::PureT<Fn>, true>{
        std::forward<P>(p), std::forward<Fn>(fn)};
}

template <typename P, typename Fn>
    requires(ParseRules<P> &&
             Predicate<detail::PureT<Fn>, typename P::ValueType>)
constexpr auto take_while1(P&& p, Fn&& fn) {
    return TakeWhile<detail::PureT<P>, detail::PureT<Fn>, false>{
        std::forward<P>(p), std::forward<Fn>(fn)};
}

}  // namespace comb

#endif