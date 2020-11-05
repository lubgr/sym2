
set(gccFlags -pedantic-errors -Wall -Wextra -pedantic -Wredundant-decls -Wwrite-strings -Wpointer-arith -Wcast-qual
    -Wformat=2 -Wmissing-include-dirs -Wcast-align -Wnon-virtual-dtor -Wctor-dtor-privacy -Wdisabled-optimization
    -Winvalid-pch -Wconversion -Wno-ctor-dtor-privacy -Wno-format-nonliteral -Wno-shadow)

set(clangFlags -Wall -Wextra -pedantic -Wconversion -Wno-sign-conversion)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 4.6)
        set(gccFlags ${gccFlags} -Wno-dangling-else -Wno-unused-local-typedefs)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5.0)
        set(gccFlags ${gccFlags} -Wdouble-promotion -Wtrampolines -Wzero-as-null-pointer-constant
            -Wvector-operation-performance -Wsized-deallocation)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6.0)
        set(gccFlags ${gccFlags} -Wshift-overflow=2 -Wnull-dereference -Wduplicated-cond)
    endif()
endif()

add_library(flags
    INTERFACE)

target_compile_options(flags
    INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:${gccFlags}>
    $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>>:${clangFlags}>
    $<$<AND:$<CONFIG:DEBUG>,$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:${sanitizerCompilerFlags}>)
