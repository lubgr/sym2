

add_library(sym2
    canonical.cpp
    expr.cpp
    get.cpp
    power.cpp
    query.cpp
    view.cpp)

target_link_libraries(sym2
    PUBLIC
    sanitizer
    flags)

target_include_directories(sym2
    SYSTEM
    PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_compile_features(sym2
    PUBLIC
    cxx_std_20)
