
#include <benchmark/benchmark.h>
#include <ginac/ginac.h>
#include "sym2/sym2.h"

void PolyDegree01Sym2(benchmark::State& state)
{
    const auto n = static_cast<std::int32_t>(state.range(0));
    const sym2::FixedExpr<1> a{"a"};
    auto* mr = std::pmr::new_delete_resource();
    sym2::Expr x{0, mr};

    for (std::int32_t i = 1; i < n; ++i)
        x = sym2::autoSum(x, sym2::autoPower(a, sym2::FixedExpr<1>{i}));

    for (auto _ : state) {
        const std::int32_t result = sym2::degree(x, a);
        benchmark::DoNotOptimize(result);
    }
}

void PolyDegree01GiNaC(benchmark::State& state)
{
    const auto n = static_cast<std::int32_t>(state.range(0));
    const GiNaC::symbol a{"a"};
    GiNaC::ex x = 0;

    for (std::int32_t i = 1; i < n; ++i)
        x += GiNaC::pow(a, i);

    for (auto _ : state) {
        const std::int32_t result = x.degree(a);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(PolyDegree01Sym2)->RangeMultiplier(2)->Range(1, 512);
BENCHMARK(PolyDegree01GiNaC)->RangeMultiplier(2)->Range(1, 512);

BENCHMARK_MAIN();
