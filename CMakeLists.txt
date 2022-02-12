
cmake_minimum_required(VERSION 3.20)
project(sym2 VERSION 0.0.0 DESCRIPTION "Fast, tiny symbolic library" LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# This will change the bin/ and lib/ output directories for this project to ${CMAKE_BINARY_DIR}/bin
# and ${CMAKE_BINARY_DIR}/lib. We can pass NO_OUTPUT_DIRS to prevent that, but I find the build tree
# layout desirable.
conan_basic_setup(TARGETS KEEP_RPATHS)
set(Boost_INCLUDE_DIRS ${CONAN_INCLUDE_DIRS_BOOST-HEADERS})

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTING "Compile unit tests" ON)
option(WITH_COVERAGE "Enable test coverage flags/generation" OFF)
option(WITH_SANITIZER "Enable UB and address sanitizer" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/Flags.cmake)
include(cmake/Sanitizer.cmake)

if(${WITH_COVERAGE})
    include(lib/cmake-modules/CodeCoverage.cmake)
    append_coverage_compiler_flags()
endif()

add_subdirectory(src)

if(${BUILD_TESTING} AND (${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}))
    enable_testing()

    set(chibiExecutable ${sym2_SOURCE_DIR}/bin/chibi)
    set(chibiExecWorkingDir ${sym2_SOURCE_DIR})
    add_subdirectory(tests)

    if(${WITH_COVERAGE})
        setup_target_for_coverage_lcov(NAME coverage
            EXECUTABLE unit-tests
            EXCLUDE tests/* lib/* /usr/* /Library*)
    endif()
endif()
