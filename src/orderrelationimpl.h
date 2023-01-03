#pragma once

#include <boost/logic/tribool_fwd.hpp>
#include "sym2/exprview.h"
#include "operandsview.h"
#include "sym2/predicates.h"

namespace sym2 {
    struct BaseExp;

    bool numbers(ExprView<number> lhs, ExprView<number> rhs);
    bool symbols(ExprView<symbol> lhs, ExprView<symbol> rhs);
    bool powers(ExprView<power> lhs, ExprView<power> rhs);
    bool powers(BaseExp lhs, BaseExp rhs);
    bool productsOrSums(ExprView<product || sum> lhs, ExprView<product || sum> rhs);
    bool orderLessThan(OperandsView lhs, OperandsView rhs);
    boost::logic::tribool orderLessThanOperandsReverse(OperandsView lhs, OperandsView rhs);
    bool constants(ExprView<constant> lhs, ExprView<constant> rhs);
    bool functions(ExprView<function> lhs, ExprView<function> rhs);
    boost::logic::tribool orderLessThanOperands(OperandsView lhs, OperandsView rhs);
    bool leftFunctionRightSymbol(ExprView<function> lhs, ExprView<symbol> rhs);
}
