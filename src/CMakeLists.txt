
add_library(sym2
    canonical.cpp
    expr.cpp
    get.cpp
    blob.cpp
    power.cpp
    query.cpp
    trigonometric.cpp
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
