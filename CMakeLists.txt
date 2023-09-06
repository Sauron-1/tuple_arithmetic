cmake_minimum_required(VERSION 3.11)
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

# Find xsimd or download from GitHub
find_package(xsimd QUIET)
if(NOT xsimd_FOUND)
    message(STATUS "xsimd not found. Downloading from github...")
    include(FetchContent)
    FetchContent_Declare(
        xsimd
        GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git
        GIT_TAG        v11.1.0
    )
    FetchContent_MakeAvailable(xsimd)
endif()
# Work around for xsimd generating "-lxsimd" flag
target_include_directories(tuple_arithmetic INTERFACE ${xsimd_INCLUDE_DIRS})

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
