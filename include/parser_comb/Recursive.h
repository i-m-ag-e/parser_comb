#ifndef PARSER_COMB_RECURSIVE_H
#define PARSER_COMB_RECURSIVE_H

#include <functional>
#include <memory>

#include "parser_comb/ParseRule.h"
namespace comb {

template <typename T>
using ParserFn = ParseResult<T>(std::string_view);

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

    constexpr auto parse(std::string_view input) const -> ParseResult<T> {
        return (*inner_parser)(input);
    }

    constexpr auto ref() const -> RecursiveRef<T> {
        return RecursiveRef{inner_parser};
    }

   private:
    std::shared_ptr<std::function<ParserFn<T>>> inner_parser;
};

}  // namespace comb

#endif