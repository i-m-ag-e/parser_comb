cmake_minimum_required(VERSION 3.15)
project(ParserComb VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# General compiler flags (applied globally for tests built in this tree)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Define the library as header-only
add_library(parser_comb INTERFACE)

# Define an alias to make it act like an imported target (good practice)
add_library(ParserComb::ParserComb ALIAS parser_comb)

# Tell CMake where the headers are. 
# $<BUILD_INTERFACE:...> allows tests in this repo to find headers.
# $<INSTALL_INTERFACE:...> allows external projects to find headers if you install it.
target_include_directories(parser_comb
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Testing Options
option(PARSERCOMB_BUILD_TESTS "Build tests" ON)

if (PARSERCOMB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
