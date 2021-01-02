#pragma once

#include <cassert>
#include <complex>
#include <numeric>
#include "get.h"
#include "query.h"
#include "view.h"

namespace sym2 {
    template <class LookupFct>
    double eval(ExprView e, LookupFct symbols);

    template <class LookupFct>
    std::complex<double> evalComplex(ExprView e, LookupFct symbols)
    {
        const auto recurComplex = [&symbols](ExprView e) { return evalComplex(e, symbols); };
        const auto recur = [&symbols](ExprView e) { return eval(e, symbols); };

        switch (type(e)) {
            case Type::symbol:
                return symbols(e[0].pre.name);
            case Type::floatingPoint:
            case Type::constant:
                return e[0].main.inexact;
            case Type::smallInt:
            case Type::smallRational:
                return static_cast<double>(e[0].main.exact.num) / e[0].main.exact.denom;
            case Type::largeRational:
                return recur(first(e)) / recur(second(e));
            case Type::largeInt:
                return static_cast<double>(get<Int>(e));
            case Type::complexNumber:
                return {recur(first(e)), recur(second(e))};
            case Type::sum:
                return std::transform_reduce(ConstSemanticOpIterator{e}, ConstSemanticOpIterator{},
                  std::complex<double>{}, std::plus<>{}, recurComplex);
            case Type::product:
                return std::transform_reduce(ConstSemanticOpIterator{e}, ConstSemanticOpIterator{},
                  std::complex<double>{1.0, 1.0}, std::multiplies<>{}, recurComplex);
            case Type::power:
                return std::pow(recurComplex(first(e)), recurComplex(second(e)));
            case Type::function:
                assert(nOps(e) == 1 || nOps(e) == 2);
                return nOps(e) == 1 ?
                  std::complex<double>{get<UnaryDoubleFctPtr>(e)(recur(first(e)))} :
                  std::complex<double>{get<BinaryDoubleFctPtr>(e)(recur(first(e)), recur(second(e)))};
            default:
                assert(false);
                return {0.0, 0.0};
        }
    }

    template <class LookupFct>
    double eval(ExprView e, LookupFct symbols)
    {
        return evalComplex(e, symbols).real();
    }
}
