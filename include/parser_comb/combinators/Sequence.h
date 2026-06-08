#ifndef PARSER_COMB_SEQ_H
#define PARSER_COMB_SEQ_H

#include <parser_comb/ParseRule.h>
#include <parser_comb/combinators/FMap.h>
#include <parser_comb/utils/TupleUtils.h>

#include <tuple>
#include <utility>

namespace comb {

template <ParseRule... Ps>
struct Seq {
    using ValueType = std::tuple<typename Ps::ValueType...>;

    explicit constexpr Seq(std::tuple<Ps...> const& tup) : parsers(tup) {}
    explicit constexpr Seq(std::tuple<Ps...>&& tup) : parsers(std::move(tup)) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        return std::apply(
            [input](const auto&... p) { return parse_impl(input, p...); },
            parsers);
    }

   private:
    std::tuple<Ps...> parsers;

    template <ParseRule P>
    static constexpr auto parse_impl(std::string_view input, P const& p)
        -> ParseResult<std::tuple<typename P::ValueType>> {
        if (auto res = p.parse(input)) {
            return std::pair{std::make_tuple(std::move(res->first)),
                             res->second};
        }
        return std::nullopt;
    }

    template <ParseRule P, ParseRule... Rest>
    static constexpr auto parse_impl(std::string_view input, P const& p,
                                     Rest const&... rest)
        -> ParseResult<
            std::tuple<typename P::ValueType, typename Rest::ValueType...>> {
        auto res1 = p.parse(input);
        if (!res1)
            return std::nullopt;

        auto res_rest = parse_impl(res1->second, rest...);
        if (!res_rest)
            return std::nullopt;

        return std::pair{std::tuple_cat(std::make_tuple(std::move(res1->first)),
                                        std::move(res_rest->first)),
                         res_rest->second};
    }
};

template <ParseRule... Ps>
explicit Seq(std::tuple<Ps...> const&) -> Seq<Ps...>;
template <ParseRule... Ps>
explicit Seq(std::tuple<Ps...>&&) -> Seq<Ps...>;

template <typename... Ps>
    requires(ParseRule<detail::PureT<Ps>> && ...)
constexpr auto seq(Ps&&... ps) -> Seq<detail::PureT<Ps>...> {
    return Seq{std::make_tuple(std::forward<Ps>(ps)...)};
}

template <ParseRule L, ParseRule R>
struct Left {
    using ValueType = typename L::ValueType;

    template <typename LU, typename RU>
    explicit constexpr Left(LU&& l, RU&& r)
        : left(std::forward<LU>(l)), right(std::forward<RU>(r)) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        auto res_l = left.parse(input);
        if (!res_l)
            return std::nullopt;

        auto res_r = right.parse(res_l->second);
        if (!res_r)
            return std::nullopt;

        return std::pair{res_l->first, res_r->second};
    }

   private:
    L left;
    R right;
};

template <typename L, typename R>
explicit Left(L&&, R&&) -> Left<detail::PureT<L>, detail::PureT<R>>;

template <typename L, typename R>
constexpr auto left(L&& l, R&& r) {
    using PureL = detail::PureT<L>;
    using PureR = detail::PureT<R>;
    return Left<PureL, PureR>{std::forward<L>(l), std::forward<R>(r)};
}

template <ParseRule L, ParseRule R>
struct Right {
    using ValueType = typename R::ValueType;

    template <typename LU, typename RU>
    explicit constexpr Right(LU&& l, RU&& r)
        : left(std::forward<LU>(l)), right(std::forward<RU>(r)) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        auto res_l = left.parse(input);
        if (!res_l)
            return std::nullopt;

        auto res_r = right.parse(res_l->second);
        if (!res_r)
            return std::nullopt;

        return std::pair{res_r->first, res_r->second};
    }

   private:
    L left;
    R right;
};

template <typename L, typename R>
explicit Right(L&&, R&&) -> Right<detail::PureT<L>, detail::PureT<R>>;

template <typename L, typename R>
    requires(ParseRule<detail::PureT<L>> && ParseRule<detail::PureT<R>>)
constexpr auto right(L&& l, R&& r) {
    using PureL = detail::PureT<L>;
    using PureR = detail::PureT<R>;
    return Right<PureL, PureR>{std::forward<L>(l), std::forward<R>(r)};
}

template <typename L, typename M, typename R>
    requires(ParseRules<L, M, R>)
constexpr auto separated(L&& left, M&& mid, R&& right)  //
    -> ParserOf<
        std::pair<detail::ValueOfParser<L>, detail::ValueOfParser<R>>> auto {
    return comb::fmap(ExtractPair<0, 2>{},
                      comb::seq(std::forward<L>(left), std::forward<M>(mid),
                                std::forward<R>(right)));
}

template <typename L, typename M, typename R>
    requires(ParseRules<L, M, R>)
constexpr auto enclosed(L&& left, M&& mid, R&& right)
    -> ParserOf<detail::ValueOfParser<M>> auto {
    return comb::left(comb::right(std::forward<L>(left), std::forward<M>(mid)),
                      std::forward<R>(right));
}

namespace detail {
template <typename ValT, typename OpT>
concept Chainable = requires(ValT&& v, OpT&& op) {
    { op(std::forward<ValT>(v), std::forward<ValT>(v)) } -> std::same_as<ValT>;
};
}  // namespace detail

template <ParseRule P, ParseRule Op>
    requires(detail::Chainable<typename P::ValueType, typename Op::ValueType>)
struct Chain {
    using ValueType = P::ValueType;

    template <typename P1, typename Op1>
    explicit constexpr Chain(P1&& p, Op1&& op)
        : parser(std::forward<P1>(p)), op_parser(std::forward<Op1>(op)) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        auto res = parser.parse(input);
        if (!res)
            return std::nullopt;

        auto curr = std::move(res->first);
        input = res->second;

        while (1) {
            auto op_res = op_parser.parse(input);
            if (!op_res)
                break;

            auto next_res = parser.parse(op_res->second);
            if (!next_res)
                break;

            curr = op_res->first(std::move(curr), std::move(next_res->first));
            input = next_res->second;
        }

        return std::pair{curr, input};
    }

   private:
    P parser;
    Op op_parser;
};

template <typename P, typename Op>
    requires(ParseRules<P, Op>)
constexpr auto chain(P&& p, Op&& op) {
    return Chain<detail::PureT<P>, detail::PureT<Op>>{std::forward<P>(p),
                                                      std::forward<Op>(op)};
}

}  // namespace comb

#endif