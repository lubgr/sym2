
#include <string>
#include <benchmark/benchmark.h>

void EmptyString(benchmark::State& state) {
  for (auto _ : state)
    std::string empty;
}

void StringCopy(benchmark::State& state) {
  std::string non_empty = "hello";
  for (auto _ : state)
    std::string copy(non_empty);
}

BENCHMARK(EmptyString);
BENCHMARK(StringCopy);

BENCHMARK_MAIN();
