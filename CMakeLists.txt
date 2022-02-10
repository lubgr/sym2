
cmake_minimum_required(VERSION 3.20)
project(sym2 VERSION 0.0.0 DESCRIPTION "Tiny symbolic library" LANGUAGES C CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTING "Compile unit tests" ON)
option(WITH_COVERAGE "Enable test coverage flags/generation" OFF)
option(WITH_SANITIZER "Enable UB and address sanitizer" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost 1.78 REQUIRED)
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
    add_subdirectory(lib/chibi-scheme)

    set(chibiExecutable ${sym2_SOURCE_DIR}/bin/chibi)
    set(chibiExecWorkingDir ${sym2_SOURCE_DIR})
    add_subdirectory(tests)

    if(${WITH_COVERAGE})
        setup_target_for_coverage_lcov(NAME coverage
            EXECUTABLE unit-tests
            EXCLUDE tests/* lib/* /usr/* /Library*)
    endif()
endif()
