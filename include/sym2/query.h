#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <utility>
#include "exprview.h"
#include "operandsview.h"
#include "predicates.h"

namespace sym2 {
    class Expr;

    struct BaseExp {
        ExprView<> base;
        ExprView<!power> exponent;
    };

    struct ConstAndTerm {
        ExprView<number> constant;
        OperandsView term;
    };

    BaseExp splitAsPower(ExprView<> e);
    ConstAndTerm splitConstTerm(ExprView<!number> e);

    // Access to the logical operands of composites, like sums, products, functions. Operands are
    // counted starting with 0, same as array indexing. UB if the parameter n is out of range or
    // there are no operands (e.g. nthOperand(smallIntExpr, 100)).
    ExprView<> nthOperand(ExprView<!small> e, std::uint16_t n);
    ExprView<> firstOperand(ExprView<!small> e);
    ExprView<> secondOperand(ExprView<!small> e);
    std::size_t nOperands(ExprView<> e);

    // Returns the real part if the argument is a complex number, and the argument itself otherwise.
    ExprView<number> real(ExprView<number> n);
    // Returns the imaginary part if the argument is a complex number, and zero (backed by static
    // storage duration) otherwise.
    ExprView<number> imag(ExprView<number> n);

    std::pair<ExprView<>, OperandsView> frontAndRest(OperandsView ops);
    std::pair<ExprView<>, std::span<const Expr>> frontAndRest(std::span<const Expr> ops);
    std::pair<ExprView<>, std::span<const ExprView<>>> frontAndRest(
      std::span<const ExprView<>> ops);

    bool contains(ExprView<> needle, ExprView<> haystack);
}
