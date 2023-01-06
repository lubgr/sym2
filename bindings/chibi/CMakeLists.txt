
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
