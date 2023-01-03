#pragma once

#include <cassert>
#include <complex>
#include <numeric>
#include "childiterator.h"
#include "exprview.h"
#include "get.h"
#include "query.h"

namespace sym2 {
    template <class LookupFct>
    double evalReal(ExprView<> e, LookupFct symbols);

    template <class LookupFct>
    std::complex<double> evalComplex(ExprView<> e, LookupFct symbols)
    {
        const auto recurComplex = [&symbols](ExprView<> e) { return evalComplex(e, symbols); };
        const auto recurReal = [&symbols](ExprView<> e) { return evalReal(e, symbols); };

        if (is < number && complexDomain > (e))
            return {recurReal(real(e)), recurReal(imag(e))};
        else if (is<sum>(e))
            return std::transform_reduce(ChildIterator::logicalChildren(e),
              ChildIterator::logicalChildrenSentinel(e), std::complex<double>{}, std::plus<>{},
              recurComplex);
        else if (is<product>(e))
            return std::transform_reduce(ChildIterator::logicalChildren(e),
              ChildIterator::logicalChildrenSentinel(e), std::complex<double>{1.0, 0.0},
              std::multiplies<>{}, recurComplex);
        else if (is<power>(e))
            return std::pow(recurComplex(firstOperand(e)), recurComplex(secondOperand(e)));
        else
            return {recurReal(e)};
    }

    template <class LookupFct>
    double evalReal(ExprView<> e, LookupFct symbols)
    {
        const auto recur = [&symbols](ExprView<> e) { return evalReal(e, symbols); };

        if (is<symbol>(e))
            return symbols(get<std::string_view>(e));
        if (is < floatingPoint || constant > (e))
            return get<double>(e);
        else if (is < small && (integer || rational) > (e))
            return get<double>(e);
        else if (is < large && integer > (e))
            return static_cast<double>(get<LargeInt>(e));
        else if (is < large && rational > (e))
            return static_cast<double>(get<LargeRational>(e));
        else if (is < complexDomain && number > (e))
            return recur(real(e));
        else if (is<sum>(e))
            return std::transform_reduce(ChildIterator::logicalChildren(e),
              ChildIterator::logicalChildrenSentinel(e), 0.0, std::plus<>{}, recur);
        else if (is<product>(e))
            return std::transform_reduce(ChildIterator::logicalChildren(e),
              ChildIterator::logicalChildrenSentinel(e), 1.0, std::multiplies<>{}, recur);
        else if (is<power>(e))
            return std::pow(recur(firstOperand(e)), recur(secondOperand(e)));
        else if (is<function>(e)) {
            assert(nOperands(e) == 1 || nOperands(e) == 2);
            return nOperands(e) == 1 ?
              get<UnaryDoubleFctPtr>(e)(recur(firstOperand(e))) :
              get<BinaryDoubleFctPtr>(e)(recur(firstOperand(e)), recur(secondOperand(e)));
        }

        assert(false);
        return 0.0;
    }
}
