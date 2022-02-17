
#include <benchmark/benchmark.h>
#include <ginac/ginac.h>
#include "sym2/sym2.h"

void AddTwoSymbolsSym2(benchmark::State& state)
{
    const sym2::Var a{"a"};
    const sym2::Var b{"b"};

    for (auto _ : state) {
        const sym2::Var result = a + b;
        benchmark::DoNotOptimize(result);
    }
}

void AddTwoSymbolsGiNaC(benchmark::State& state)
{
    const GiNaC::symbol a{"a"};
    const GiNaC::symbol b{"b"};

    for (auto _ : state) {
        const GiNaC::ex result = a + b;
        benchmark::DoNotOptimize(result);
    }
}

void MixedArithmetic01Sym2(benchmark::State& state)
{
    const sym2::Var a{"a"};
    const sym2::Var b{"b"};

    for (auto _ : state) {
        const sym2::Var c = a + a + 2 * b + 2 * a / 3;
        const sym2::Var d = 3 * b * b * c + 2 * a - 4 * b;
        const sym2::Var e = 2 * a / 7 * b * c * c - 2 * b;

        benchmark::DoNotOptimize(e);
    }
}

void MixedArithmetic01GiNaC(benchmark::State& state)
{
    const GiNaC::symbol a{"a"};
    const GiNaC::symbol b{"b"};

    for (auto _ : state) {
        const GiNaC::ex c = a + a + 2 * b + 2 * a / 3;
        const GiNaC::ex d = 3 * b * b * c + 2 * a - 4 * b;
        const GiNaC::ex e = 2 * a / 7 * b * c * c - 2 * b;

        benchmark::DoNotOptimize(e);
    }
}

BENCHMARK(MixedArithmetic01Sym2);
BENCHMARK(MixedArithmetic01GiNaC);
BENCHMARK(AddTwoSymbolsSym2);
BENCHMARK(AddTwoSymbolsGiNaC);

BENCHMARK_MAIN();
