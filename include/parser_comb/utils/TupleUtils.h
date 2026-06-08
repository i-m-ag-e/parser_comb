#ifndef PARSER_COMB_TUPLE_UTILS_H
#define PARSER_COMB_TUPLE_UTILS_H

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace comb {

namespace detail {
template <typename T, size_t N>
concept HasTupleElement = requires(T t) {
    typename std::tuple_element<N, std::remove_cvref_t<T>>::type;
    {
        std::get<N>(t)
    } -> std::convertible_to<
        const std::tuple_element_t<N, std::remove_cvref_t<T>>&>;
};

template <typename T>
concept TupleLike = requires(T t) {
    std::tuple_size_v<std::remove_cvref_t<T>>;
} && []<size_t... Is>(std::index_sequence<Is...>) consteval {
    return (HasTupleElement<T, Is> && ...);
}(std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<T>>>{});

}  // namespace detail

template <size_t... Is>
struct ExtractIndices {
    template <detail::TupleLike T, typename U = std::remove_cvref_t<T>>
    constexpr auto operator()(T&& tup) const  //
        -> std::tuple<std::tuple_element_t<Is, U>...> {
        return std::make_tuple(std::get<Is>(std::forward<T>(tup))...);
    }
};

namespace detail {

template <typename T>
struct IsIndexSequenceImpl : std::false_type {};

template <size_t... Is>
struct IsIndexSequenceImpl<std::index_sequence<Is...>> : std::true_type {};

template <typename T>
concept IndexSequence = IsIndexSequenceImpl<T>::value;

}  // namespace detail

template <detail::IndexSequence IndexSeq>
struct ExtractIndicesFromSeq;

template <size_t... Is>
struct ExtractIndicesFromSeq<std::index_sequence<Is...>>
    : ExtractIndices<Is...> {};

namespace detail {

template <typename KeptSeq, typename RestSeq, typename IgnoreSeq>
struct FilterSequence;

template <size_t... Kept, size_t Cur, size_t... Rest, size_t... Ignore>
struct FilterSequence<std::index_sequence<Kept...>,
                      std::index_sequence<Cur, Rest...>,
                      std::index_sequence<Ignore...>> {
   private:
    static constexpr bool Keep = !((Cur == Ignore) || ...);

   public:
    using Type = FilterSequence<
        std::conditional_t<Keep, std::index_sequence<Kept..., Cur>,
                           std::index_sequence<Kept...>>,
        std::index_sequence<Rest...>, std::index_sequence<Ignore...>>;
};

template <size_t... Kept, size_t... Ignore>
struct FilterSequence<std::index_sequence<Kept...>, std::index_sequence<>,
                      std::index_sequence<Ignore...>> {
    using Type = std::index_sequence<Kept...>;
};

template <size_t N, size_t... Is>
using ComplementSequence =
    FilterSequence<std::index_sequence<>, std::make_index_sequence<N>,
                   std::index_sequence<Is...>>::Type;

}  // namespace detail

template <size_t... Is>
struct IgnoreIndices {
    template <detail::TupleLike T>
    constexpr auto operator()(T&& tup) const {
        constexpr size_t N = std::tuple_size_v<std::remove_cvref_t<T>>;
        using ComplementedSeq = detail::ComplementSequence<N, Is...>;

        return ExtractIndicesFromSeq<ComplementedSeq>{}(std::forward<T>(tup));
    }
};

template <detail::IndexSequence Seq>
struct IgnoreIndicesFromSeq;

template <size_t... Is>
struct IgnoreIndicesFromSeq<std::index_sequence<Is...>> {};

template <size_t N1, size_t N2>
struct ExtractPair {
    template <detail::TupleLike T>
    constexpr auto operator()(T&& tup) const {
        return std::pair{std::get<N1>(std::forward<T>(tup)),
                         std::get<N2>(std::forward<T>(tup))};
    }
};

template <size_t N>
struct ExtractNth {
    template <detail::TupleLike T>
    constexpr auto operator()(T&& tup) const {
        return std::get<N>(std::forward<T>(tup));
    }
};

namespace detail {

template <typename T, typename Tup>
concept ConstructibleFromTupleLike =
    detail::TupleLike<Tup> && requires(Tup&& tup) {
        {
            std::apply(
                []<typename... Args>(Args&&... args) {
                    return T(std::forward<Args>(args)...);
                },
                std::forward<Tup>(tup))
        };
    };

}  // namespace detail

template <typename T>
struct ConstructFromSequence {
    template <detail::TupleLike Tup>
        requires(detail::ConstructibleFromTupleLike<T, Tup>)
    constexpr auto operator()(Tup&& tup) {
        return std::apply(
            []<typename... Args>(Args&&... args) {
                return T(std::forward<Args>(args)...);
            },
            std::forward<Tup>(tup));
    }
};

}  // namespace comb

#endif