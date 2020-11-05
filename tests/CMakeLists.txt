
add_executable(unit-tests
    testcanonical.cpp
    testequality.cpp
    testquery.cpp
    main.cpp)

target_include_directories(unit-tests
    PRIVATE
    ${sym2_SOURCE_DIR}/src
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_link_libraries(unit-tests
    PRIVATE
    sym2
    SYSTEM
    doctest::doctest)

add_test(NAME sym2.unittests COMMAND unit-tests)
