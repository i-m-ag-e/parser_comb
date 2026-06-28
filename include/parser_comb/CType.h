/**
 * @file CType.h
 * @brief constexpr wrappers around the C `<cctype>` functions.
 *
 * Provides `constexpr` function objects wrapping the standard `<cctype>`
 * character predicates and conversion functions. These wrappers are
 * intended for use with parser combinators, allowing functions such as
 * `comb::isdigit` or `comb::isspace` to be passed directly as predicates.
 *
 * The reason for the existence of this header is that if we try to use
 * ```cpp
 * comb::satisfy(std::isdigit)
 * ```
 * the compiler is unable to resolve `Fn` in `comb::satisfy`, since functions
 * like `std::isdigit` are overloads and we need to cast them to specific
 * functions, in order to get them to work. For example,
 * ```cpp
 * comb::satisfy(static_cast<int(*)(int)>(std::isdigit))
 * ```
 *
 * Since the functions listed in this header have specific signatures, they can
 * be used without casting.
 */

#ifndef PARSER_COMB_CTYPE_H
#define PARSER_COMB_CTYPE_H

#include <cctype>

namespace comb {

constexpr auto isalnum = [](unsigned char c) { return std::isalnum(c); };
constexpr auto isalpha = [](unsigned char c) { return std::isalpha(c); };
constexpr auto isblank = [](unsigned char c) { return std::isblank(c); };
constexpr auto iscntrl = [](unsigned char c) { return std::iscntrl(c); };
constexpr auto isdigit = [](unsigned char c) { return std::isdigit(c); };
constexpr auto isgraph = [](unsigned char c) { return std::isgraph(c); };
constexpr auto islower = [](unsigned char c) { return std::islower(c); };
constexpr auto isprint = [](unsigned char c) { return std::isprint(c); };
constexpr auto ispunct = [](unsigned char c) { return std::ispunct(c); };
constexpr auto isspace = [](unsigned char c) { return std::isspace(c); };
constexpr auto isupper = [](unsigned char c) { return std::isupper(c); };
constexpr auto isxdigit = [](unsigned char c) { return std::isxdigit(c); };
constexpr auto tolower = [](unsigned char c) { return std::tolower(c); };
constexpr auto toupper = [](unsigned char c) { return std::toupper(c); };

}  // namespace comb

#endif