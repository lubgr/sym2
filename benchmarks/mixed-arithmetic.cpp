
#include <benchmark/benchmark.h>
#include <ginac/ginac.h>
#include "sym2/sym2.h"

void AddTwoSymbolsSym2(benchmark::State& state)
{
    const sym2::FixedExpr<1> a{"a"};
    const sym2::FixedExpr<1> b{"b"};
    sym2::StackBuffer<1024> arena;

    for (auto _ : state) {
        const sym2::Expr result = sym2::autoSum(a, b, &arena);
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
    const sym2::FixedExpr<1> a{"a"};
    const sym2::FixedExpr<1> b{"b"};
    const sym2::FixedExpr<1> two{2};
    const sym2::FixedExpr<1> three{3};
    sym2::StackBuffer<2048> arena;

    for (auto _ : state) {
        // c = a + a + 2 * b + 2 * a / 3;
        const sym2::Expr c =
          sym2::autoSum({a, a, sym2::autoProduct(two, b, &arena),
                          sym2::autoProduct({two, a, sym2::FixedExpr<1>{1, 3}}, &arena)},
            &arena);
        // d = 3 * b * b * c + 2 * a - 4 * b;
        const sym2::Expr d = sym2::autoSum(
          {sym2::autoProduct({three, b, b, c}, &arena), sym2::autoProduct(two, a, &arena),
            sym2::autoProduct(sym2::FixedExpr<1>{-4}, b, &arena)},
          &arena);
        // e = 2 * a / 7 * b * c * c - 2 * b;
        const sym2::Expr e =
          sym2::autoSum({sym2::autoProduct({two, a, sym2::FixedExpr<1>{1, 7}, b, c, c}, &arena),
                          sym2::autoProduct(sym2::FixedExpr<1>{-2}, b, &arena)},
            &arena);

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
