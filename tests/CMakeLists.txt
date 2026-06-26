include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
include(CTest)

add_executable(test_basic test_basic.cpp)
target_link_libraries(test_basic 
    PRIVATE 
        ParserComb::ParserComb
)

add_library(json_parser STATIC 
    json_parser/JsonParser.cpp
    json_parser/JsonValue.h
    json_parser/JsonParser.h
)
target_include_directories(json_parser PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/json_parser")
target_link_libraries(json_parser
    PRIVATE 
        ParserComb::ParserComb
)

add_executable(test_json_parser
    try_json_parser.cpp
    json_parser/JsonValue.h
)
target_link_libraries(test_json_parser
    PRIVATE 
        ParserComb::ParserComb
        json_parser
)
target_include_directories(test_json_parser PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/json_parser")

add_executable(run_tests
    # test_parser.cpp
    primitives/test_char.cpp
    primitives/test_core.cpp
    primitives/test_string.cpp
    combinators/test_alt.cpp
    combinators/test_sequence.cpp
    combinators/test_quantifiers.cpp
    combinators/test_fmap.cpp
    combinators/test_skip_utils.cpp
    combinators/test_optional_notahead.cpp
    test_recursive.cpp 
    test_json_parser.cpp
)

target_link_libraries(run_tests
    PRIVATE 
        ParserComb::ParserComb
        json_parser
        GTest::gtest_main
)

# Register tests with CTest
gtest_discover_tests(run_tests)
