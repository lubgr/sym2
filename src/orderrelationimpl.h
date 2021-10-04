#pragma once

#include <boost/logic/tribool_fwd.hpp>
#include "operandsview.h"
#include "predicates.h"
#include "view.h"

namespace sym2 {
    bool numbers(ExprView<number> lhs, ExprView<number> rhs);
    bool symbols(ExprView<symbol> lhs, ExprView<symbol> rhs);
    bool powers(ExprView<power> lhs, ExprView<power> rhs);
    bool productsOrSums(ExprView<product || sum> lhs, ExprView<product || sum> rhs);
    boost::logic::tribool orderLessThanOperandsReverse(OperandsView lhs, OperandsView rhs);
    bool constants(ExprView<constant> lhs, ExprView<constant> rhs);
    bool functions(ExprView<function> lhs, ExprView<function> rhs);
    boost::logic::tribool orderLessThanOperands(OperandsView lhs, OperandsView rhs);
}
