#ifndef PARSER_COMB_FMAP_H
#define PARSER_COMB_FMAP_H

#include <parser_comb/ParseRule.h>

#include <type_traits>
#include <utility>

namespace comb {

// clang-format off
/**
 * @brief Parser that transforms the result of a parser into another value
 * @ingroup combinators
 * 
 * Usage:
 * ```cpp
 * auto p = comb::fmap(
 *     [](std::string_view s){ return std::stoi(s); },
 *     std::take_char_while1(comb::isdigit)
 * );
 * auto r1 = p.parse("123;"); // (123, ";")
 * auto r2 = p.parse("1a2b"); // (1, "a2b")
 * auto r3 = p.parse("xyz"); // std::nullopt
 * ```
 * 
 * @tparam Fn the transformer function
 * @tparam P  the parser that produces the initial value
 */
template <typename Fn, ParseRule P>
struct FMap {
    using ValueType = std::invoke_result_t<Fn, typename P::ValueType>;

    template <typename FnU, typename PU>
        requires(ParseRules<PU>)
    explicit constexpr FMap(FnU&& fn, PU&& p)
        : fn(std::forward<FnU>(fn)), parser(std::forward<PU>(p)) {}

    constexpr auto parse(std::string_view input) const
        -> ParseResult<ValueType> {
        if (auto parse_result = parser.parse(input)) {
            return std::pair{fn(std::move(parse_result->first)),
                             parse_result->second};
        }
        return std::nullopt;
    }

   private:
    Fn fn;
    P parser;
};

template <typename Fn, typename P>
explicit FMap(Fn&&, P&&) -> FMap<detail::PureT<Fn>, detail::PureT<P>>;

/**
 * @brief helper to construct `FMap<Fn, P>` 
 * @ingroup combinators
 * 
 * @tparam Fn 
 * @tparam P 
 */
template <typename Fn, typename P>
    requires(ParseRule<std::remove_cvref_t<P>>)
constexpr auto fmap(Fn&& fn, P&& p)  // -> FMap<PureFn, PureP>
{
    using PureFn = std::remove_cvref_t<Fn>;
    using PureP = std::remove_cvref_t<P>;
    return FMap<PureFn, PureP>{std::forward<Fn>(fn), std::forward<P>(p)};
}

}  // namespace comb

#endif