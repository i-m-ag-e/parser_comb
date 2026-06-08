# ParserComb

A minimal C++20 parser combinator library built as an exercise in value semantics and template metaprogramming. This is intended as an exercise and aims to mimic the standard Haskell-style parser combinators in C++, but with an equal amount of type-safety. 

### Architecture & Trade-offs
- **Zero-Allocation Primitives**: Flat combinators (`seq`, `alt`, `separated_by0`) operate entirely by slicing `std::string_view` without heap allocations.
- **Recursive Boundaries**: To prevent infinite template instantiation and cyclic memory leaks, `comb::RecursiveP` uses type erasure (`std::function`) and shared states (`std::shared_ptr/std::weak_ptr`). This introduces standard library overhead at recursive nodes.

### Features
The library provides standard monadic combinators and primitives for building grammars:
- **Primitives**: Match exact strings (`stringP`), single characters (`charP`), or match characters against predicates (`satisfy`, `take_char_while`).
- **Sequencing**: Chain parsers together (`seq`) or discard unwanted surroundings (`left`, `right`, `enclosed`).
- **Choice**: Prioritize alternative paths (`alt`) or provide fallbacks (`with_default`).
- **Quantifiers**: Match repetition (`zero_or_more`, `take_while`) and lists separated by delimiters (`separated_by0`).
- **Recursion**: Handle self-referential grammars safely using `RecursiveP` and `.ref()`.
- **Transformations**: Map parsed tokens into C++ objects using `fmap` and `map_to_value`.

### Example Usage
Here is a simple parser that extracts a comma-separated list of integers enclosed in brackets.
```cpp
#include <parser_comb/ParserComb.h>
#include <iostream>
#include <string>

using namespace comb;

int main() {
    // 1. Define primitive rules
    auto number = fmap(
        [](std::string_view s) { return std::stoi(std::string(s)); },
        take_char_while1(isdigit)
    );
    
    auto comma = lexeme(charP(','));
    auto l_bracket = lexeme(charP('['));
    auto r_bracket = lexeme(charP(']'));

    // 2. Build the structural combinator
    auto array_p = enclosed(l_bracket, separated_by0(number, comma), r_bracket);

    // 3. Execute
    auto result = array_p.parse("[10, 25, 42]");
    
    if (result) {
        std::cout << "Parsed " << result->first.size() << " integers:\n";
        for (int n : result->first) {
            std::cout << n << "\n";
        }
    } else {
        std::cout << "Parse failed.\n";
    }

    return 0;
}
```

#### Output
```shell
Parsed 3 integers:
10
25
42
```

### JSON Example
The `tests/json_parser/` directory contains a working JSON parser. It serves as a practical test for handling recursive ASTs, whitespace, and string escape sequences using the combinators.
