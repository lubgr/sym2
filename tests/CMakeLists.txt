
add_library(unit-tests-lowlevel
    OBJECT
    lowlevel/testassumptions.cpp
    lowlevel/testexpr.cpp
    lowlevel/testview.cpp)

target_include_directories(unit-tests-lowlevel
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/lowlevel)

target_include_directories(unit-tests-lowlevel
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include/sym2
    ${PROJECT_SOURCE_DIR}/src)

target_include_directories(unit-tests-lowlevel
    SYSTEM
    PUBLIC
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_link_libraries(unit-tests-lowlevel
    PUBLIC
    sym2
    doctest::doctest)

add_executable(unit-tests
    testchilditerator.cpp
    testequality.cpp
    testfunctionview.cpp
    testget.cpp
    testeval.cpp
    testoperandsview.cpp
    testorderrelationimpl.cpp
    testpredicates.cpp
    testquery.cpp
    testvar.cpp
    main.cpp)

target_link_libraries(unit-tests
    PRIVATE
    unit-tests-lowlevel)

add_test(NAME sym2.unittests COMMAND unit-tests)


if (WITH_SANITIZER)
    set(libasan "libasan")
    if (APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(libasan "libclang_rt.asan_osx_dynamic")
    endif()

    execute_process(COMMAND
        ${CMAKE_CXX_COMPILER} -print-file-name=${libasan}${CMAKE_SHARED_LIBRARY_SUFFIX}
        OUTPUT_VARIABLE libasanpath
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    set(asanPreload "DYLD_INSERT_LIBRARIES=${libasanpath}")
endif()

function(add_scm_test source)
    cmake_path(GET source STEM LAST_ONLY name)

    add_test(NAME "sym2.${name}"
        COMMAND
        ${sym2_BINARY_DIR}/scheme/bin/chibi-scheme
        -i ${sym2_BINARY_DIR}/scheme/share/chibi/chibi.img
        -I ${sym2_SOURCE_DIR}/src
        -I ${sym2_BINARY_DIR}/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/${source})
    set_tests_properties("sym2.${name}" PROPERTIES
        ENVIRONMENT
        "LD_LIBRARY_PATH=${sym2_BINARY_DIR}/scheme/lib:$ENV{LD_LIBRARY_PATH};${asanPreload}")
endfunction()

add_scm_test(order.scm)
add_scm_test(power.scm)
add_scm_test(product.scm)
add_scm_test(query.scm)
add_scm_test(roundtrip.scm)
add_scm_test(sum.scm)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(pretty-printer-tests
        prettydebug.cpp)

    target_include_directories(pretty-printer-tests
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include/sym2
        ${PROJECT_SOURCE_DIR}/src)

    target_include_directories(pretty-printer-tests
        SYSTEM
        PRIVATE
        $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

    target_link_libraries(pretty-printer-tests
        PRIVATE
        sym2)

    add_test(NAME sym2.pretty-printer-tests
        COMMAND
        ${PROJECT_SOURCE_DIR}/bin/test-lldb-formatter.sh $<TARGET_FILE:pretty-printer-tests>
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()
