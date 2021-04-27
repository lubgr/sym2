
set(linenoiseSourceDir ${sym2_SOURCE_DIR}/lib/linenoise)

add_library(linenoise
    ${linenoiseSourceDir}/linenoise.c)

target_include_directories(linenoise
    PUBLIC
    ${linenoiseSourceDir})
