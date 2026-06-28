#ifndef PARSER_COMB_RECURSIVE_H
#define PARSER_COMB_RECURSIVE_H

#include <functional>
#include <memory>

#include "parser_comb/ParseRule.h"
namespace comb {

template <typename T>
using ParserFn = ParseResult<T>(std::string_view);

/**
 * @brief A reference to a recursive parser.
 *
 * This reference is a parser in itself, but calls the underlying parser stored
 * in a `std::shared_ptr`.
 *
 * See comb::RecursiveP for usage.
 *
 * @tparam T
 */
template <typename T>
struct RecursiveRef {
    using ValueType = T;

    explicit RecursiveRef(
        std::shared_ptr<std::function<ParserFn<T>>> const& parser_ptr)
        : parser_ref(parser_ptr) {}

    constexpr auto parse(std::string_view input) const -> ParseResult<T> {
        auto parser_ptr = parser_ref.lock();
        if (!parser_ptr || !*parser_ptr)
            return std::nullopt;
        return (*parser_ptr)(input);
    }

   private:
    std::weak_ptr<std::function<ParserFn<T>>> parser_ref;
};

/**
 * @brief A parser that allows for recursive definitions.
 *
 * This parser allocates on the heap and is not compatible with `constexpr`
 * contexts at all.
 *
 * Since most combinators store the inner parser by value, recursive parsers
 * would result in an infinite type size, and would not be permitted by the
 * compiler. For example, a grammar definition like:
 *
 * expr = expr '+' expr | expr '*' expr | '(' expr ')' | number
 *
 * cannot be expressed directly using comb::alt.
 *
 * Instead, you can use `RecursiveP` to define recursive parsers.
 * - The left-hand side is first declared as an empty `lhs = RecursiveP<T>`,
 * where `T` is the type of the parsed value.
 * - The right-hand side is then defined separately, using `lhs.ref()` in place
 * of `lhs` in the grammar.
 * - Finally, the right-hand side is set to the left-hand side using
 * `lhs.set(rhs)`.
 *
 * For example, the above grammar can be expressed as:
 * ```cpp
 * comb::RecursiveP<Expr> expr;
 * auto plus = comb::seq(expr.ref(), comb::charP('+'), expr.ref());
 * auto mult = comb::seq(expr.ref(), comb::charP('*'), expr.ref());
 * auto parens = comb::seq(comb::charP('('), expr.ref(), comb::charP(')'));
 * auto number = comb::take_char_while1(comb::isdigit);
 *
 * expr.set(comb::alt(plus, mult, parens, number));
 * // this won't compile in itself since number is a string_view. Each parser
 * // would need to be mapped to an Expr type, which is omitted here for
 * brevity.
 * // However, the point is that the recursive parser can be defined in this
 * way.
 * ```
 * @tparam T
 */
template <typename T>
struct RecursiveP {
    using ValueType = T;

    RecursiveP()
        : inner_parser(std::make_shared<std::function<ParserFn<T>>>()) {}

    template <typename P>
        requires(ParseRules<P> && ParserOf<detail::PureT<P>, T>)
    void set(P&& p) {
        *inner_parser = [p = std::forward<P>(p)](std::string_view input) {
            return p.parse(input);
        };
    }

    constexpr auto is_set() const -> bool {
        return inner_parser && *inner_parser;
    }

    constexpr auto parse(std::string_view input) const -> ParseResult<T> {
        if (is_set())
            return (*inner_parser)(input);
        return std::nullopt;
    }

    constexpr auto ref() const -> RecursiveRef<T> {
        return RecursiveRef{inner_parser};
    }

   private:
    std::shared_ptr<std::function<ParserFn<T>>> inner_parser;
};

}  // namespace comb

#endif