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

// clang-format off
/**
 * @brief Functor that extracts elements from a tuple at specified indices.
 *
 * Usage:
 * ```cpp
 * auto extractor = comb::ExtractIndices<0, 2>{};
 * auto p = comb::fmap(extractor, comb::seq(comb::any_char, comb::charP(':'), comb::any_char, comb::charP(';')));
 * p.parse("a:b;"); // (('a', 'b'), "")
 * ```
 *
 * @tparam Is the indices of the elements to extract from the tuple.
 */
// clang-format on
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

/**
 * @brief Functor that extracts elements from a tuple at specified indices,
 * given as an index sequence.
 *
 * @tparam IndexSeq the index sequence containing the indices
 */
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
        std::index_sequence<Rest...>, std::index_sequence<Ignore...>>::Type;
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

// clang-format off
/**
 * @brief Functor that extracts elements from a tuple, ignoring specified
 * indices.
 *
 * Usage:
 * ```cpp
 * auto extractor = comb::IgnoreIndices<0, 2>{};
 * auto p = comb::fmap(extractor, comb::seq(comb::any_char, comb::charP(':'), comb::any_char, comb::charP(';')));
 * p.parse("a:b;"); // ((':', ';'), "")
 * ```
 *
 * @tparam Is indices to ignore
 */
// clang-format on
template <size_t... Is>
struct IgnoreIndices {
    template <detail::TupleLike T>
    constexpr auto operator()(T&& tup) const {
        constexpr size_t N = std::tuple_size_v<std::remove_cvref_t<T>>;
        using ComplementedSeq = detail::ComplementSequence<N, Is...>;

        return ExtractIndicesFromSeq<ComplementedSeq>{}(std::forward<T>(tup));
    }
};

/**
 * @brief Functor that extracts elements from a tuple, ignoring indices
 * specified in an index sequence.
 *
 * @tparam Seq the index sequence containing the indices to ignore
 */
template <detail::IndexSequence Seq>
struct IgnoreIndicesFromSeq;

template <size_t... Is>
struct IgnoreIndicesFromSeq<std::index_sequence<Is...>> {};

// clang-format off
/**
 * @brief Functor that extracts two elements as a pair from a tuple at specified
 * indices
 *
 * Usage:
 * ```cpp
 * auto extractor = comb::ExtractPair<0, 2>{};
 * auto p = comb::fmap(extractor, comb::seq(comb::any_char, comb::charP(':'), comb::any_char, comb::charP(';')));
 * p.parse("a:b;"); // (std::pair('a', 'b'), "")
 * ```
 *
 * @tparam N1 Index of the first element to extract
 * @tparam N2 Index of the second element to extract
 */
// clang-format on
template <size_t N1, size_t N2>
struct ExtractPair {
    template <detail::TupleLike T>
    constexpr auto operator()(T&& tup) const {
        return std::pair{std::get<N1>(std::forward<T>(tup)),
                         std::get<N2>(std::forward<T>(tup))};
    }
};

// clang-format off
/**
 * @brief Functor that extracts the N-th element from a tuple.
 *
 * Usage:
 * ```cpp
 * auto extractor = comb::ExtractNth<2>{};
 * auto p = comb::fmap(extractor, comb::seq(comb::any_char, comb::charP(':'), comb::any_char, comb::charP(';')));
 * p.parse("a:b;"); // ('b', "")
 * ```
 *
 * @tparam N index of the element to extract
 */
// clang-format off
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

/**
 * @brief Functor that constructs an object of type `T` from a tuple of arguments.
 * 
 * Usage:
 * ```cpp
 * struct MyStruct {
 *    int a;
 *    char b;
 *    std::string_view c;
 * };
 *
 * auto constructor = comb::ConstructFromSequence<MyStruct>{};
 * auto number = comb::fmap([](auto s){ return std::stoi(s); }, comb::take_char_while1(comb::isdigit));
 * auto var = comb::take_char_while1(comb::isalpha);
 * auto p = comb::fmap(constructor, comb::seq(number, comb::charP(':'), var));
 * p.parse("123:abc"); // (MyStruct{123, ':', "abc"}, "")
 * ```
 * 
 * @tparam T 
 */
template <typename T>
struct ConstructFromSequence {
    template <detail::TupleLike Tup>
        requires(detail::ConstructibleFromTupleLike<T, Tup>)
    constexpr auto operator()(Tup&& tup) const {
        return std::apply(
            []<typename... Args>(Args&&... args) {
                return T(std::forward<Args>(args)...);
            },
            std::forward<Tup>(tup));
    }
};

}  // namespace comb

#endif
