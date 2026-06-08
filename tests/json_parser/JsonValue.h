#ifndef JSON_VALUE_H
#define JSON_VALUE_H

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace json_parser {

namespace detail {

template <class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

}  // namespace detail

class JsonValue;

using JsonArray = std::vector<JsonValue>;
using JsonObject = std::map<std::string, JsonValue>;

class JsonValue {
   private:
    using VariantType = std::variant<std::nullptr_t, bool, double, std::string,
                                     JsonArray, JsonObject>;

    VariantType m_value;

   public:
    // --- 1. Ergonomic Constructors ---
    // These allow your combinators to just return standard types
    // and let the compiler auto-wrap them.
    constexpr JsonValue() : m_value(nullptr) {}
    constexpr JsonValue(std::nullptr_t) : m_value(nullptr) {}
    constexpr JsonValue(bool b) : m_value(b) {}
    constexpr JsonValue(int i)
        : m_value(static_cast<double>(i)) {
    }  // Catch ints so they don't ambiguously cast to bool
    constexpr JsonValue(double d) : m_value(d) {}
    constexpr JsonValue(const char* s) : m_value(std::string(s)) {}
    constexpr JsonValue(std::string s) : m_value(std::move(s)) {}
    constexpr JsonValue(JsonArray a) : m_value(std::move(a)) {}
    JsonValue(JsonObject o) : m_value(std::move(o)) {}

    // --- 2. Type Checking ---
    bool is_null() const {
        return std::holds_alternative<std::nullptr_t>(m_value);
    }
    bool is_bool() const { return std::holds_alternative<bool>(m_value); }
    bool is_number() const { return std::holds_alternative<double>(m_value); }
    bool is_string() const {
        return std::holds_alternative<std::string>(m_value);
    }
    bool is_array() const { return std::holds_alternative<JsonArray>(m_value); }
    bool is_object() const {
        return std::holds_alternative<JsonObject>(m_value);
    }

    // --- 3. Value Extraction ---
    template <typename T>
    T& get() {
        return std::get<T>(m_value);
    }

    template <typename T>
    const T& get() const {
        return std::get<T>(m_value);
    }

    // --- 4. Container Ergonomics (Auto-vivification) ---
    // If you use [] on a null value, it becomes an object.
    JsonValue& operator[](const std::string& key) {
        if (is_null())
            m_value = JsonObject{};
        if (is_object())
            return get<JsonObject>()[key];
        throw std::runtime_error(
            "Attempted to access key on non-object JsonValue");
    }

    // If you push_back on a null value, it becomes an array.
    void push_back(const JsonValue& val) {
        if (is_null())
            m_value = JsonArray{};
        if (is_array()) {
            get<JsonArray>().push_back(val);
        } else {
            throw std::runtime_error(
                "Attempted to push_back on non-array JsonValue");
        }
    }

    // --- 5. Pretty Printing ---
    std::string dump(int indent = 4, int current_indent = 0) const {
        std::ostringstream os;
        std::string spaces(current_indent, ' ');
        std::string inner_spaces(current_indent + indent, ' ');

        std::visit(detail::overload{
                       [&](std::nullptr_t) { os << "null"; },
                       [&](bool b) { os << (b ? "true" : "false"); },
                       [&](double d) { os << d; },
                       [&](const std::string& s) {
                           os << std::quoted(s);
                       },  // (Doesn't handle escaping internal quotes yet)

                       [&](const JsonArray& arr) {
                           if (arr.empty()) {
                               os << "[]";
                               return;
                           }
                           os << "[\n";
                           for (size_t i = 0; i < arr.size(); ++i) {
                               os << inner_spaces
                                  << arr[i].dump(indent,
                                                 current_indent + indent);
                               if (i < arr.size() - 1)
                                   os << ",";
                               os << "\n";
                           }
                           os << spaces << "]";
                       },

                       [&](const JsonObject& obj) {
                           if (obj.empty()) {
                               os << "{}";
                               return;
                           }
                           os << "{\n";
                           bool first = true;
                           for (const auto& [key, val] : obj) {
                               if (!first)
                                   os << ",\n";
                               os << inner_spaces << '"' << key << "\": "
                                  << val.dump(indent, current_indent + indent);
                               first = false;
                           }
                           os << "\n" << spaces << "}";
                       }},
                   m_value);

        return os.str();
    }
};
}  // namespace json_parser

#endif