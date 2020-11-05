
cmake_minimum_required(VERSION 3.17)
project(sym2 VERSION 0.0.0 DESCRIPTION "Tiny symbolic library" LANGUAGES CXX)

option(BUILD_TESTING "Compile unit tests" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost 1.73 REQUIRED)
include(cmake/Flags.cmake)
include(cmake/Sanitizer.cmake)

add_subdirectory(src)

if(${BUILD_TESTING} AND (${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}))
    enable_testing()
    add_subdirectory(lib/doctest)
    add_subdirectory(tests)

    include(lib/cmake-modules/CodeCoverage.cmake)
    append_coverage_compiler_flags()
    setup_target_for_coverage_gcovr_html(NAME coverage
        EXECUTABLE unit-tests
        BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
        EXCLUDE "${PROJECT_SOURCE_DIR}/lib/*")
endif()
