
add_library(sym2
    autosimpl.cpp
    blob.cpp
    expr.cpp
    functions.cpp
    get.cpp
    logarithm.cpp
    numeric.cpp
    power.cpp
    product.cpp
    query.cpp
    trigonometric.cpp
    typetags.cpp
    var.cpp
    view.cpp)

target_link_libraries(sym2
    PUBLIC
    sanitizer
    flags)

target_include_directories(sym2
    SYSTEM
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/sym2>
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_compile_features(sym2
    PUBLIC
    cxx_std_20)
