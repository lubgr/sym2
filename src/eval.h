#pragma once

#include <cassert>
#include <complex>
#include <numeric>
#include "childiterator.h"
#include "get.h"
#include "query.h"
#include "view.h"

namespace sym2 {
    template <class LookupFct>
    double evalReal(ExprView<> e, LookupFct symbols);

    template <class LookupFct>
    std::complex<double> evalComplex(ExprView<> e, LookupFct symbols)
    {
        const auto recurComplex = [&symbols](ExprView<> e) { return evalComplex(e, symbols); };
        const auto recurReal = [&symbols](ExprView<> e) { return evalReal(e, symbols); };

        switch (type(e)) {
            case Type::complexNumber:
                return {recurReal(real(e)), recurReal(imag(e))};
            case Type::sum:
                return std::transform_reduce(ChildIterator::logicalChildren(e), ChildIterator{}, std::complex<double>{},
                  std::plus<>{}, recurComplex);
            case Type::product:
                return std::transform_reduce(ChildIterator::logicalChildren(e), ChildIterator{},
                  std::complex<double>{1.0, 0.0}, std::multiplies<>{}, recurComplex);
            case Type::power:
                return std::pow(recurComplex(firstOperand(e)), recurComplex(secondOperand(e)));
            default:
                return {recurReal(e)};
        }
    }

    template <class LookupFct>
    double evalReal(ExprView<> e, LookupFct symbols)
    {
        const auto recur = [&symbols](ExprView<> e) { return evalReal(e, symbols); };

        switch (type(e)) {
            case Type::symbol:
                return symbols(get<std::string_view>(e));
            case Type::floatingPoint:
            case Type::constant:
                return e[0].main.inexact;
            case Type::smallInt:
            case Type::smallRational:
                return static_cast<double>(e[0].main.exact.num) / e[0].main.exact.denom;
            case Type::largeRational:
                return recur(numerator(e)) / recur(denominator(e));
            case Type::largeInt:
                return static_cast<double>(get<LargeInt>(e));
            case Type::complexNumber:
                return recur(real(e));
            case Type::sum:
                return std::transform_reduce(
                  ChildIterator::logicalChildren(e), ChildIterator{}, 0.0, std::plus<>{}, recur);
            case Type::product:
                return std::transform_reduce(
                  ChildIterator::logicalChildren(e), ChildIterator{}, 1.0, std::multiplies<>{}, recur);
            case Type::power:
                return std::pow(recur(firstOperand(e)), recur(secondOperand(e)));
            case Type::function:
                assert(nOperands(e) == 1 || nOperands(e) == 2);
                return nOperands(e) == 1 ? get<UnaryDoubleFctPtr>(e)(recur(firstOperand(e))) :
                                           get<BinaryDoubleFctPtr>(e)(recur(firstOperand(e)), recur(secondOperand(e)));
            default:
                assert(false);
                return 0.0;
        }
    }
}
