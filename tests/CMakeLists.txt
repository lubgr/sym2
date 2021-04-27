
add_executable(sym7
    s7converter.cpp
    s7repl.cpp
    s7bridge.cpp)

target_link_libraries(sym7
    PRIVATE
    sym2
    s7
    linenoise)

target_include_directories(sym7
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include/sym2
    ${PROJECT_SOURCE_DIR}/src)

target_include_directories(sym7
    SYSTEM
    PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

add_executable(unit-tests
    testassumptions.cpp
    testequality.cpp
    testfunctionview.cpp
    testget.cpp
    testeval.cpp
    testexpr.cpp
    testoperands.cpp
    testpower.cpp
    testpredicates.cpp
    testquery.cpp
    testvar.cpp
    testview.cpp
    main.cpp)

target_include_directories(unit-tests
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include/sym2
    ${PROJECT_SOURCE_DIR}/src)

target_include_directories(unit-tests
    SYSTEM
    PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_link_libraries(unit-tests
    PRIVATE
    sym2
    doctest::doctest)

add_test(NAME sym2.unittests COMMAND unit-tests)

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
