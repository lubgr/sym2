
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


function(add_scm_test source)
    get_filename_component(name ${source} NAME_WLE)
    add_test(NAME "sym2.${name}"
        WORKING_DIRECTORY ${chibiExecWorkingDir}
        COMMAND ${chibiExecutable} ${CMAKE_CURRENT_SOURCE_DIR}/${source})
endfunction()

add_scm_test(order.scm)
add_scm_test(power.scm)
add_scm_test(product.scm)
add_scm_test(roundtrip.scm)
add_scm_test(sum.scm)

add_executable(pretty
    prettydebug.cpp)

target_include_directories(pretty
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include/sym2
    ${PROJECT_SOURCE_DIR}/src)

target_include_directories(pretty
    SYSTEM
    PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_link_libraries(pretty
    PRIVATE
    sym2)

add_test(NAME sym2.pretty
    COMMAND ${PROJECT_SOURCE_DIR}/bin/test-lldb-formatter.sh ${CMAKE_CURRENT_BINARY_DIR}/pretty
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
