#ifndef PARSER_COMB_ALT_H
#define PARSER_COMB_ALT_H

#include <parser_comb/ParseRule.h>

#include <tuple>

namespace comb {

namespace detail {

template <typename... Ps>
concept IsSameAll =
    sizeof...(Ps) == 0 ||
    (std::is_same_v<std::tuple_element_t<0, std::tuple<Ps...>>, Ps> && ...);

}

template <ParseRule... Ps>
    requires(sizeof...(Ps) > 1 && detail::IsSameAll<typename Ps::ValueType...>)
struct Alt {
   private:
    using TuplePs = std::tuple<Ps...>;

   public:
    explicit constexpr Alt(TuplePs const& tup) : parsers(tup) {}
    explicit constexpr Alt(TuplePs&& tup) : parsers(std::move(tup)) {}

    using ValueType = std::tuple_element_t<0, TuplePs>::ValueType;

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        return std::apply(
            [input](auto&&... ps) { return parse_impl(input, ps...); },
            parsers);
    }

   private:
    TuplePs parsers;

    template <ParseRule P>
    static constexpr auto parse_impl(std::string_view input, P const& p)
        -> ParseResult<ValueType> {
        return p.parse(input);
    }

    template <ParseRule P, ParseRule... Rest>
    static constexpr auto parse_impl(std::string_view input, P const& p,
                                     Rest const&... rest)
        -> ParseResult<ValueType> {
        auto res = p.parse(input);
        if (res)
            return std::move(res);

        return parse_impl(input, rest...);
    }
};

template <ParseRule... Ps>
explicit Alt(std::tuple<Ps...> const&) -> Alt<Ps...>;
template <ParseRule... Ps>
explicit Alt(std::tuple<Ps...>&&) -> Alt<Ps...>;

template <typename... Ps>
    requires(ParseRule<std::remove_cvref_t<Ps>> && ...)
constexpr auto alt(Ps&&... ps) -> Alt<std::remove_cvref_t<Ps>...> {
    return Alt{std::make_tuple(std::forward<Ps>(ps)...)};
}

}  // namespace comb

#endif