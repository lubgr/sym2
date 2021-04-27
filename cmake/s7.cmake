
set(s7SourceDir ${sym2_SOURCE_DIR}/lib/s7)
set(s7BinaryDir ${sym2_BINARY_DIR}/lib/s7)

add_library(s7
    ${s7SourceDir}/s7.c)

file(TOUCH
    ${s7BinaryDir}/mus-config.h)

target_include_directories(s7
    PUBLIC
    ${s7SourceDir}
    PRIVATE
    ${s7BinaryDir})
