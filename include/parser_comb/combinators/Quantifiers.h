#ifndef PARSER_COMB_MANY_H
#define PARSER_COMB_MANY_H

#include <vector>

#include "parser_comb/ParseRule.h"
#include "parser_comb/combinators/Sequence.h"

namespace comb {

/**
 * @brief Combinator that executes a parser as many times as it succeeds
 * @ingroup combinators
 *
 * Since this returns a `std::vector` of the produced values, this allocates on
 * the heap. This is also not `constexpr` compatible before C++20.
 *
 * Usage given with construction functions `comb::zero_or_more` and
 * `comb::one_or_more`.
 *
 * @tparam P
 * @tparam AllowZero flag to allow or disallow zero executions of a parser
 */
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
        return std::pair{std::exchange(parsed_values, {}), input};
    }

   private:
    P parser;
};

/**
 * @brief constructs a `Many<P, true>`
 * @ingroup combinators
 *
 * This parser never fails.
 *
 * Usage:
 * ```cpp
 * auto p = comb::zero_or_more(comb::stringP("xyz"));
 * auto r1 = p.parse("xyzxyzxyz"); // (["xyz", "xyz", "xyz"], "")
 * auto r2 = p.parse("#"); // ([], "#")
 * ```
 *
 * @tparam P
 */
template <typename P>
    requires(ParseRule<detail::PureT<P>>)
constexpr auto zero_or_more(P&& p) -> Many<detail::PureT<P>, true> {
    return Many<detail::PureT<P>, true>{std::forward<P>(p)};
}

/**
 * @brief constructs a `Many<P, false>`
 * @ingroup combinators
 *
 * This parser fails when P cannot be executed even once.
 *
 * Usage:
 * ```cpp
 * auto p = comb::one_or_more(comb::stringP("xyz"));
 * auto r1 = p.parse("xyzxyzxyz"); // (["xyz", "xyz", "xyz"], "")
 * auto r2 = p.parse("#"); // std::nullopt
 * ```
 *
 * @tparam P
 */
template <typename P>
    requires(ParseRule<detail::PureT<P>>)
constexpr auto one_or_more(P&& p) -> Many<detail::PureT<P>, false> {
    return Many<detail::PureT<P>, false>{std::forward<P>(p)};
}

/**
 * @brief Combinator that executes a parser as many times as it succeeds,
 * separated by a delimiter
 * @ingroup combinators
 *
 * Allocates on the heap and not `constexpr` compatible before C++20.
 *
 * Usage given with construction functions `comb::separated_by0` and
 * `comb::separated_by1`.
 *
 * @tparam P           parser to execute
 * @tparam Delim       parser that parses the delimiter
 * @tparam AllowZero   flag to allow or disallow zero executions of a parser
 */
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
        return std::pair{std::exchange(parsed_values, {}), input};
    }

   private:
    P parser;
    Delim delimiter;
};

/**
 * @brief constructs a `DelimitedSequence<P, Delim, true>`
 * @ingroup combinators
 *
 * This parser never fails.
 *
 * Usage:
 * ```cpp
 * auto p = comb::separated_by0(comb::stringP("xyz"), comb::charP(';'));
 * auto r1 = p.parse("xyz;xyz;xyz"); // (["xyz", "xyz", "xyz"], "")
 * auto r2 = p.parse("xyz;xyz;"); // (["xyz", "xyz"], "")
 * auto r3 = p.parse("#"); // ([], "#")
 * ```
 *
 * @tparam P
 * @tparam Delim
 */
template <typename P, typename Delim>
    requires(ParseRules<P, Delim>)
constexpr auto separated_by0(P&& p, Delim&& delim)
    -> DelimitedSequence<detail::PureT<P>, detail::PureT<Delim>, true> {
    return DelimitedSequence<detail::PureT<P>, detail::PureT<Delim>, true>{
        std::forward<P>(p), std::forward<Delim>(delim)};
}

/**
 * @brief constructs a `DelimitedSequence<P, Delim, false>`
 * @ingroup combinators
 *
 * This parser fails when P cannot be executed even once.
 *
 * Usage:
 * ```cpp
 * auto p = comb::separated_by1(comb::stringP("xyz"), comb::charP(';'));
 * auto r1 = p.parse("xyz;xyz;xyz"); // (["xyz", "xyz", "xyz"], "")
 * auto r2 = p.parse("xyz;xyz;"); // (["xyz", "xyz"], "")
 * auto r3 = p.parse("#"); // std::nullopt
 * ```
 *
 * @tparam P
 * @tparam Delim
 */
template <typename P, typename Delim>
    requires(ParseRules<P, Delim>)
constexpr auto separated_by1(P&& p, Delim&& delim)
    -> DelimitedSequence<detail::PureT<P>, detail::PureT<Delim>, false> {
    return DelimitedSequence<detail::PureT<P>, detail::PureT<Delim>, false>{
        std::forward<P>(p), std::forward<Delim>(delim)};
}

/**
 * @brief Combinator that executes a parser as many times as it succeeds and the
 * @ingroup combinators
 * consumed value satisfies a predicate
 *
 * Allocates on the heap and not `constexpr` compatible before C++20.
 *
 * Usage given with construction functions `comb::take_while0` and
 * `comb::take_while1`.
 *
 * @tparam P         parser to execute
 * @tparam Fn        predicate that checks the parsed value
 * @tparam AllowZero flag to allow or disallow zero executions of a parser
 */
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

        return std::pair{std::exchange(parsed_values, {}), input};
    }

   private:
    P parser;
    Fn pred;
};

// clang-format off
/**
 * @brief Constructs a `TakeWhile<P, Fn, true>`
 * @ingroup combinators
 * 
 * This parser never fails.
 *
 * Usage:
 * ```cpp
 * auto p = comb::take_while0(comb::satisfy(comb::isdigit), [](char c){ return c <= '5'; });
 * auto r1 = p.parse("1234567bye"); // (['1', '2', '3', '4', '5'], "67bye")
 * auto r2 = p.parse("6789"); // ([], "6789")
 * auto r3 = p.parse("abc"); // ([], "abc")
 * ```
 *
 * @tparam P 
 * @tparam Fn 
 */
// clang-format on
template <typename P, typename Fn>
    requires(ParseRules<P> &&
             Predicate<detail::PureT<Fn>, typename P::ValueType>)
constexpr auto take_while0(P&& p, Fn&& fn) {
    return TakeWhile<detail::PureT<P>, detail::PureT<Fn>, true>{
        std::forward<P>(p), std::forward<Fn>(fn)};
}

// clang-format off
/**
 * @brief Constructs a `TakeWhile<P, Fn, false>`
 * @ingroup combinators
 * 
 * This parser fails when P cannot be executed even once or the first parsed value does not satisfy the predicate.
 *
 * Usage:
 * ```cpp
 * auto p = comb::take_while1(comb::satisfy(comb::isdigit), [](char c){ return c <= '5'; });
 * auto r1 = p.parse("1234567bye"); // (['1', '2', '3', '4', '5'], "67bye")
 * auto r2 = p.parse("6789"); // std::nullopt
 * auto r3 = p.parse("abc"); // std::nullopt
 * ```
 *
 * @tparam P 
 * @tparam Fn 
 */
// clang-format on
template <typename P, typename Fn>
    requires(ParseRules<P> &&
             Predicate<detail::PureT<Fn>, typename P::ValueType>)
constexpr auto take_while1(P&& p, Fn&& fn) {
    return TakeWhile<detail::PureT<P>, detail::PureT<Fn>, false>{
        std::forward<P>(p), std::forward<Fn>(fn)};
}

}  // namespace comb

#endif