#include <JsonParser.h>
#include <JsonValue.h>
#include <parser_comb/ParserComb.h>

#include <string_view>

using std::string_view_literals::operator""sv;

namespace json_parser {

constexpr auto numbers = comb::take_char_while1(comb::isdigit);

constexpr auto parse_true =
    comb::map_to_value(comb::lexeme(comb::stringP("true")), JsonValue{true});
constexpr auto parse_false =
    comb::map_to_value(comb::lexeme(comb::stringP("false")), JsonValue{false});

constexpr auto parse_null = comb::map_to_value(
    comb::lexeme(comb::stringP("null")), json_parser::JsonValue{nullptr});

constexpr auto parse_number = comb::lexeme(comb::fmap(
    [](std::tuple<std::string_view, std::optional<std::string_view>> pair) {
        auto& [int_part, frac_part] = pair;
        std::string num_str(int_part);
        if (frac_part) {
            num_str += '.';
            num_str += *frac_part;
        }
        return JsonValue{std::stod(num_str)};
    },
    comb::seq(numbers, comb::optional_or(comb::right(comb::charP('.'), numbers),
                                         ""sv))));

constexpr auto comma = comb::lexeme(comb::charP(','));
constexpr auto colon = comb::lexeme(comb::charP(':'));
constexpr auto l_sq_bracket = comb::lexeme(comb::charP('['));
constexpr auto r_sq_bracket = comb::lexeme(comb::charP(']'));
constexpr auto l_curly = comb::lexeme(comb::charP('{'));
constexpr auto r_curly = comb::lexeme(comb::charP('}'));

constexpr auto normal_char =
    comb::satisfy([](char c) { return c != '"' && c != '\\'; });

constexpr auto escaped_char = comb::right(
    comb::charP('\\'), comb::alt(comb::map_to_value(comb::charP('n'), '\n'),
                                 comb::map_to_value(comb::charP('t'), '\t'),
                                 comb::map_to_value(comb::charP('r'), '\r'),
                                 comb::map_to_value(comb::charP('b'), '\b'),
                                 comb::map_to_value(comb::charP('f'), '\f'),
                                 comb::map_to_value(comb::charP('\\'), '\\'),
                                 comb::map_to_value(comb::charP('"'), '\"')));

constexpr auto string_content =
    comb::zero_or_more(comb::alt(escaped_char, normal_char));

constexpr auto parse_string = comb::fmap(
    [](std::vector<char> chars) {
        return std::string(chars.begin(), chars.end());
    },
    comb::enclosed(comb::charP('"'), string_content, comb::charP('"')));

constexpr auto parse_json_string = comb::fmap(
    [](std::string&& s) { return JsonValue{std::move(s)}; }, parse_string);

auto make_json_parser() -> comb::ParserOf<JsonValue> auto {
    comb::RecursiveP<JsonValue> value_p;

    const auto array_elements = comb::separated_by0(value_p.ref(), comma);
    const auto parse_array =
        comb::enclosed(l_sq_bracket, array_elements, r_sq_bracket);
    const auto parse_json_array = comb::fmap(
        [](std::vector<JsonValue>&& vals) {
            JsonArray arr;
            for (auto&& v : vals) {
                arr.push_back(std::move(v));
            }
            return JsonValue{std::move(arr)};
        },
        parse_array);

    const auto kv_pair = comb::separated(parse_string, colon, value_p.ref());
    const auto parse_object =
        comb::enclosed(l_curly, comb::separated_by0(kv_pair, comma), r_curly);
    const auto parse_json_object = comb::fmap(
        [](std::vector<std::pair<std::string, JsonValue>>&& kvs) {
            JsonObject obj;
            for (auto&& [k, v] : kvs) {
                obj.emplace(std::move(k), std::move(v));
            }
            return JsonValue{std::move(obj)};
        },
        parse_object);

    value_p.set(comb::alt(parse_true, parse_false, parse_null, parse_number,
                          parse_json_string, parse_json_array,
                          parse_json_object));
    return value_p;
}

auto parse_json(std::string_view input) -> std::optional<JsonValue> {
    static const auto parser = make_json_parser();
    if (const auto res = parser.parse(input)) {
        return res->first;
    }
    return std::nullopt;
}

}  // namespace json_parser