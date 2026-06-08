include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
include(CTest)

# Basic Test Executable
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
target_include_directories(json_parser PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/json_parser")
target_link_libraries(json_parser
    PRIVATE 
        ParserComb::ParserComb
)

add_executable(test_json_parser
    test_json_parser.cpp
    json_parser/JsonValue.h
)
target_link_libraries(test_json_parser
    PRIVATE 
        ParserComb::ParserComb
        json_parser
)
target_include_directories(test_json_parser PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/json_parser")

# Full Test Suite Executable
# add_executable(run_tests
#     # test_parser.cpp
#     # Add other combinator/primitive test files here
# )

# target_link_libraries(run_tests
#     PRIVATE 
#         ParserComb::ParserComb
#         GTest::gtest_main
# )

# # Register tests with CTest
# gtest_discover_tests(run_tests)
