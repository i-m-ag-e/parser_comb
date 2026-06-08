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