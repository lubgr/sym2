
add_executable(benchmark-mixed-arithmetic
    mixed-arithmetic.cpp)

target_link_libraries(benchmark-mixed-arithmetic
    PRIVATE
    sym2
    ginac::ginac
    benchmark::benchmark)

add_executable(benchmark-queries
    queries.cpp)

target_link_libraries(benchmark-queries
    PRIVATE
    sym2
    ginac::ginac
    benchmark::benchmark)
