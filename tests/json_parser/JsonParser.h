#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <JsonValue.h>
#include <parser_comb/ParserComb.h>

#include <optional>
#include <string_view>

namespace json_parser {

auto parse_json(std::string_view input) -> std::optional<JsonValue>;

}  // namespace json_parser

#endif