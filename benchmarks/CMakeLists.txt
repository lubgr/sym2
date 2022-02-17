
add_executable(benchmark-mixed-arithmetic
    mixed-arithmetic.cpp)

target_link_libraries(benchmark-mixed-arithmetic
    PRIVATE
    sym2
    ginac::ginac
    benchmark::benchmark)
