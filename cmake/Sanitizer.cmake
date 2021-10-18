
set(sanitizerFlags -fsanitize=address,undefined)
set(sanitizerCompilerFlags
    ${sanitizerFlags},integer-divide-by-zero,float-divide-by-zero,float-cast-overflow,return
    -fno-omit-frame-pointer)

add_library(sanitizer
    INTERFACE)

target_compile_options(sanitizer
    INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:${sanitizerCompilerFlags}>)

target_link_libraries(sanitizer
    INTERFACE
    $<$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:${sanitizerFlags}>)

