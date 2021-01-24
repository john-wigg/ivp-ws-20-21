project("Java SST Compiler" LANGUAGES CXX)
cmake_minimum_required(VERSION 3.0)

enable_testing()

add_library(scanner scanner.cpp input.cpp)
add_library(symbol_table symbol_table.cpp)
add_library(parser parser.cpp)
target_link_libraries(parser PUBLIC scanner)
target_link_libraries(parser PUBLIC symbol_table)

add_executable(tests tests/tests.cpp)
target_include_directories(tests PUBLIC tests)
target_include_directories(tests PUBLIC .)
target_link_libraries(tests PUBLIC scanner)

add_test(tests tests)