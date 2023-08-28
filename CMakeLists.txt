cmake_minimum_required(VERSION 3.10)
project(tuple_arithmetic)

option(BUILD_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tuple_arithmetic INTERFACE)
target_include_directories(
    tuple_arithmetic INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tuple_arithmetic>
    $<INSTALL_INTERFACE:include/tuple_arithmetic>
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

target_compile_features(tuple_arithmetic INTERFACE cxx_std_20)

install(TARGETS tuple_arithmetic EXPORT tuple_arithmeticConfig)
install(EXPORT tuple_arithmeticConfig DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/tuple_arithmetic)
install(DIRECTORY tuple_arithmetic DESTINATION include)

export(TARGETS tuple_arithmetic FILE tuple_arithmeticConfig.cmake)
