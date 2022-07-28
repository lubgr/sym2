#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <utility>
#include "operandsview.h"
#include "predicates.h"
#include "view.h"

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

    std::size_t nOperands(ExprView<> e);
    std::size_t nPhysicalChildren(ExprView<> e);

    /* Access to the logical operands of composites, like sums, products, functions. It is _not_
     * meant to access physical children of scalars that happen to have > 1 blobs (e.g. large
     * rationals, complex numbers) etc. Parameter n must be > 0, otherwise UB. Also UB if there is
     * no corresponding subpart, e.g. nthOperand(smallIntExpr, 100). Note that this is not random
     * access, but worst case linear time complexity. */
    ExprView<> nthOperand(ExprView<composite> e, std::uint32_t n);
    ExprView<> firstOperand(ExprView<composite> e);
    ExprView<> secondOperand(ExprView<composite> e);

    ExprView<number> real(ExprView<number> c);
    ExprView<number> imag(ExprView<number> c);
    ExprView<integer> numerator(ExprView<rational> n);
    ExprView<integer> denominator(ExprView<rational> n);

    std::pair<ExprView<>, OperandsView> frontAndRest(OperandsView ops);
    std::pair<ExprView<>, std::span<const Expr>> frontAndRest(std::span<const Expr> ops);
    std::pair<ExprView<>, std::span<const ExprView<>>> frontAndRest(
      std::span<const ExprView<>> ops);

    bool contains(ExprView<> needle, ExprView<> haystack);
}
