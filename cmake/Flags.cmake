
set(gccFlags -pedantic-errors -Wall -Wextra -pedantic -Wwrite-strings -Wpointer-arith -Wcast-qual
    -Wformat=2 -Wmissing-include-dirs -Wcast-align -Wnon-virtual-dtor -Wctor-dtor-privacy
    -Wdisabled-optimization -Winvalid-pch -Wconversion -Wno-ctor-dtor-privacy -Wno-format-nonliteral
    -Wno-shadow -Wno-cast-function-type -Wno-dangling-else -Wno-unused-local-typedefs
    -Wdouble-promotion -Wtrampolines -Wzero-as-null-pointer-constant -Wvector-operation-performance
    -Wsized-deallocation -Wshift-overflow=2 -Wnull-dereference -Wduplicated-cond)

set(clangFlags -Wall -Wextra -pedantic -Wconversion -Wno-sign-conversion -Wno-c99-extensions)

if (COLORED_OUTPUT)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(gccFlags ${gccFlags} -fdiagnostics-color=always)
   elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
       set(clangFlags ${clangFlags} -fcolor-diagnostics)
    endif()
endif()

add_library(flags
    INTERFACE)

target_compile_options(flags
    INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:${gccFlags}>
    $<$<CONFIG:DEBUG>:-gdwarf-4> # Necessary for gcc on MacOS for std:: prefixes
    $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>>:${clangFlags}>
    $<$<AND:$<CONFIG:DEBUG>,$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:${sanitizerCompilerFlags}>)
