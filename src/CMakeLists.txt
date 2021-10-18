
add_library(sym2common
    INTERFACE)

target_link_libraries(sym2common
    INTERFACE
    ${CMAKE_DL_LIBS}
    $<$<BOOL:${WITH_SANITIZER}>:sanitizer>
    flags)

target_include_directories(sym2common
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

target_compile_features(sym2common
    INTERFACE
    cxx_std_20)

add_library(sym2lowlevel
    OBJECT
    lowlevel/access.cpp
    lowlevel/blob.cpp
    lowlevel/childiterator.cpp
    lowlevel/expr.cpp
    lowlevel/get.cpp
    lowlevel/predicates.cpp
    lowlevel/query.cpp
    lowlevel/view.cpp)

set_property(TARGET sym2lowlevel
    PROPERTY POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})

target_link_libraries(sym2lowlevel
    PRIVATE
    sym2common)

target_include_directories(sym2lowlevel
    SYSTEM
    PUBLIC
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

target_include_directories(sym2lowlevel
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_library(sym2
    autosimpl.cpp
    functions.cpp
    logarithm.cpp
    numeric.cpp
    operandsview.cpp
    orderrelationimpl.cpp
    powersimpl.cpp
    productsimpl.cpp
    query.cpp
    trigonometric.cpp
    var.cpp
    violationhandler.cpp)

target_link_libraries(sym2
    PUBLIC
    sym2common
    PRIVATE
    sym2lowlevel)

target_include_directories(sym2
    SYSTEM
    PRIVATE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

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
