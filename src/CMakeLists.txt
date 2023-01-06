
add_library(sym2common
    INTERFACE)

target_link_libraries(sym2common
    INTERFACE
    ${CMAKE_DL_LIBS}
    $<$<BOOL:${WITH_SANITIZER}>:sanitizer>
    flags)

target_compile_options(sym2common
    INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-fno-semantic-interposition>)

target_include_directories(sym2common
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

target_include_directories(sym2common
    SYSTEM
    INTERFACE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_include_directories(sym2common
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_compile_features(sym2common
    INTERFACE
    cxx_std_23)

if (WITH_UNITY_BUILD)
    add_library(sym2
        unity.cpp)
else()
    add_library(sym2
        autosimpl.cpp
        blob.cpp
        childiterator.cpp
        cohenautosimpl.cpp
        expr.cpp
        exprview.cpp
        get.cpp
        logarithm.cpp
        numberarithmetic.cpp
        operandsview.cpp
        orderrelationimpl.cpp
        plaintextprintengine.cpp
        polynomial.cpp
        predicates.cpp
        prettyprinter.cpp
        query.cpp
        trigonometric.cpp
        violationhandler.cpp
        )
endif()

set_property(TARGET sym2 PROPERTY POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})

target_link_libraries(sym2
    PUBLIC
    sym2common)

add_library(sym2chibi
    chibibridge.cpp
    chibiconverter.cpp)

target_link_libraries(sym2chibi
    PRIVATE
    chibi::libchibi-scheme
    sym2)

set_target_properties(sym2chibi
    PROPERTIES
    PREFIX "")
