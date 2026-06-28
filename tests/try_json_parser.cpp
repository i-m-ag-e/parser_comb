#include <JsonValue.h>

#include "JsonParser.h"

int main() {
    json_parser::JsonValue expected_ast;
    auto res = json_parser::parse_json(
        R"json(
                {
                  "project_name": "ParserComb \"V1.0\"",
                  "description": "A purely functional parser.\n\tIt handles:\n\t1. Recursive structures\n\t2. Zero-allocation arrays",
                  "windows_path": "C:\\dev\\cpp\\parser_comb\\include",
                  "url": "https://github.com/cpp-thing/parser",
                  "weird_controls": "Backspace\b and Formfeed\f",
                  "nested_quotes": {
                    "dialogue": "He looked at the compiler and said, \"This is \\\"insane\\\"!\""
                  },
                  "is_ready": true,
                  "bugs": null
                }
                )json");

    if (res) {
        std::cout << "Parsed JSON:\n" << res->dump() << std::endl;
    } else {
        std::cout << "Failed to parse JSON." << std::endl;
    }
}