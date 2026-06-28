# ParserComb

![C++20](https://img.shields.io/badge/C++-20-blue.svg)
![CMake](https://img.shields.io/badge/CMake-Supported-green.svg)

A header-only, purely functional parser combinator library for C++20. This project was built as an exercise in modern template metaprogramming, value semantics, and zero-copy string processing. It provides Haskell-style parsing primitives with strict C++ type safety enforced via Concepts.

## Architecture & Trade-offs

* **Zero-Allocation Primitives:** Base combinators (`seq`, `alt`, `separated_by0`) operate strictly by advancing `std::string_view` slices. Heap allocations are deferred entirely to the caller, occurring only when explicitly aggregating parsed AST nodes (e.g., into a `std::vector`).
* **Exceptionless Control Flow:** Instead of relying on C++ exceptions for syntax errors, the library utilizes algebraic return types (`std::optional`). This models parsing failures as standard control flow rather than exceptional states, avoiding stack-unwinding overhead and maintaining purely functional composition.
* **Type-Erased Recursion:** Self-referential grammars natively cause infinite template instantiation in C++. The `RecursiveP` wrapper breaks these compile-time cycles by injecting a runtime boundary using type erasure (`std::function`) and shared ownership (`std::shared_ptr`), localizing the abstraction overhead strictly to recursive nodes.

## Quick Start: Parsing an Integer Array

Here is an example of parsing a comma-separated list of integers enclosed in brackets (e.g., `[10, 25, 42]`).

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
        for (int n : result->first) {
            std::cout << n << "\n";
        }
    }
    return 0;
}

```

## JSON Parser 

The `tests/integration/test_json_parser.cpp` contains a fully compliant JSON parser built using this library. It serves as a practical stress test for handling recursive AST generation (using `std::variant`), whitespace stripping, and string escape sequences.

## API Overview

The library relies heavily on C++20 Concepts (`ParseRule`, `ParserOf`) to constrain inputs.

### 1. Core Primitives

* `charP(c)`, `stringP(s)`: Match exact characters or string literals.
* `satisfy(predicate)`: Matches a single character against a `<cctype>` wrapper.
* `take_char_while(predicate)`: Consumes characters matching a predicate, returning a `std::string_view`.

### 2. Combinators

* `seq(p1, p2, ...)`: Chains parsers, returning a `std::tuple` of results.
* `alt(p1, p2, ...)`: Attempts parsers in sequence, returning the first success.
* `left(p1, p2)`, `right(p1, p2)`, `enclosed(l, p, r)`: Sequence parsers but discard surrounding structural tokens.
* `fmap(func, p)`: Transforms the resulting AST node of a successful parse.

### 3. Quantifiers

* `zero_or_more(p)`, `one_or_more(p)`: Repeats a parser, aggregating into a `std::vector`.
* `separated_by0(p, delim)`: Parses a list of `p` separated by `delim`.

### 4. Recursion

* `RecursiveP<T>`: A wrapper that allows a parser to reference itself before definition via the `.ref()` method, enabling recursive grammar structures.

## Build and Test Instructions

This is a header-only library. You can include the `include/` directory directly in your project.

To build and run the test suite (requires CMake and a C++20 compliant compiler):

```bash
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure

```

To build without tests, you can disable them during the CMake configuration step:

```bash
cmake -DBUILD_TESTS=OFF ..
```

To include this library in your own CMake project, you can use `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
    parser_comb
    GIT_REPOSITORY "https://github.com/i-m-ag-e/parser_comb.git"
    GIT_TAG "main"
)
FetchContent_MakeAvailable(parser_comb)

target_link_libraries(your_target PRIVATE ParserComb::ParserComb)
```

## Documentation

Full API documentation generated via Doxygen is available at: [https://i-m-ag-e.github.io/parser_comb/](https://i-m-ag-e.github.io/parser_comb/)

You can generate the API documentation locally using [Doxygen](https://www.doxygen.nl/index.html). After installing Doxygen, run the following command in the root of the repository:

```bash
doxygen Doxyfile
```

This will generate HTML documentation in the `docs/html` directory. Open `docs/html/index.html` in your web browser to view the documentation.
