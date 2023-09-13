cmake_minimum_required(VERSION 3.11)
project(tuple_arithmetic)

option(BUILD_TESTS "build tuple_arithmetic tests" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tuple_arithmetic INTERFACE)
target_include_directories(
    tuple_arithmetic INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tuple_arithmetic>
    $<INSTALL_INTERFACE:include/tuple_arithmetic>
)

# Find xsimd or download from GitHub
find_package(xsimd QUIET)
if(NOT xsimd_FOUND)
    message(STATUS "xsimd not found. Downloading from github...")
    include(FetchContent)
    FetchContent_Declare(
        xsimd
        URL https://github.com/xtensor-stack/xsimd/archive/refs/tags/11.1.0.zip
        TLS_VERIFY true
        URL_HASH MD5=3b17c8dd08724b025f8bc618590e085f
    )
    set(BUILD_TESTS_TMP ${BUILD_TESTS})
    set(BUILD_TESTS OFF)
    FetchContent_MakeAvailable(xsimd)
    set(BUILD_TESTS ${BUILD_TESTS_TMP})

    target_link_libraries(tuple_arithmetic INTERFACE xsimd)
else()
    target_include_directories(tuple_arithmetic INTERFACE ${xsimd_INCLUDE_DIRS})
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(simd_tests)
endif()

target_compile_features(tuple_arithmetic INTERFACE cxx_std_20)

install(TARGETS tuple_arithmetic EXPORT tuple_arithmeticConfig)
install(EXPORT tuple_arithmeticConfig DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/tuple_arithmetic)
install(DIRECTORY tuple_arithmetic DESTINATION include)

export(TARGETS tuple_arithmetic FILE tuple_arithmeticConfig.cmake)
