
add_library(sym2
    autosimpl.cpp
    blob.cpp
    expr.cpp
    functions.cpp
    get.cpp
    logarithm.cpp
    numeric.cpp
    power.cpp
    predicates.cpp
    product.cpp
    query.cpp
    trigonometric.cpp
    var.cpp
    view.cpp)

target_link_libraries(sym2
    PUBLIC
    sanitizer
    flags)

target_include_directories(sym2
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

target_include_directories(sym2
    SYSTEM
    PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_compile_features(sym2
    PUBLIC
    cxx_std_20)

add_library(sym2chibi
    chibibridge.cpp
    chibiconverter.cpp
    chibiutils.cpp)

target_link_libraries(sym2chibi
    PRIVATE
    chibi::libchibi-scheme
    sym2)

target_include_directories(sym2chibi
    SYSTEM
    PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

set_target_properties(sym2chibi
    PROPERTIES
    PREFIX "")
