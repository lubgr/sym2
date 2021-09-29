#pragma once

#include <cstdint>
#include <string_view>
#include "predicates.h"
#include "view.h"

namespace sym2 {
    Type type(ExprView<> e);
    Flag flags(ExprView<> e);

    struct BaseExp {
        ExprView<> base;
        ExprView<!power> exponent;
    };

    BaseExp asPower(ExprView<> e);

    std::size_t nOperands(ExprView<> e);
    std::size_t nOperands(Blob b);
    std::size_t nPhysicalChildren(ExprView<> e);
    std::size_t nPhysicalChildren(Blob b);

    /* Access to the logical operands of composites, like sums, products, functions. It is _not_ meant to access
     * physical children of scalars that happen to have > 1 blobs (e.g. large rationals, complex numbers) etc. Parameter
     * n must be > 0, otherwise UB. Also UB if there is no corresponding subpart, e.g. nthOperand(smallIntExpr, 100).
     * Note that this is not random access, but worst case linear time complexity. */
    ExprView<> nthOperand(ExprView<> e, std::uint32_t n);
    ExprView<> firstOperand(ExprView<> e);
    ExprView<> secondOperand(ExprView<> e);

    ExprView<number> real(ExprView<complexDomain> c);
    ExprView<number> imag(ExprView<complexDomain> c);
    ExprView<integer> numerator(ExprView<rational> n);
    ExprView<integer> denominator(ExprView<rational> n);
}
