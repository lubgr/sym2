
cmake_minimum_required(VERSION 3.17)
project(sym2 VERSION 0.0.0 DESCRIPTION "Tiny symbolic library" LANGUAGES C CXX)

option(BUILD_TESTING "Compile unit tests" ON)
option(WITH_COVERAGE "Enable test coverage flags/generation" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost 1.73 REQUIRED)
include(cmake/Flags.cmake)
include(cmake/Sanitizer.cmake)

if(${WITH_COVERAGE})
    include(lib/cmake-modules/CodeCoverage.cmake)
    append_coverage_compiler_flags()
endif()

add_subdirectory(src)

if(${BUILD_TESTING} AND (${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}))
    enable_testing()
    add_subdirectory(lib/doctest)
    include(cmake/s7.cmake)
    include(cmake/linenoise.cmake)
    add_subdirectory(tests)

    if(${WITH_COVERAGE})
        setup_target_for_coverage_lcov(NAME coverage
            EXECUTABLE unit-tests
            EXCLUDE tests/* lib/* /usr/* /Library*)
    endif()
endif()
