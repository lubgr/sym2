#pragma once

#include <cstdint>
#include <string_view>
#include "blob.h"

namespace sym2 {
    class ExprView;

    Type type(ExprView e);
    Flag flags(ExprView e);

    ExprView base(ExprView e);
    ExprView exponent(ExprView e);

    std::size_t nLogicalOperands(ExprView e);
    std::size_t nLogicalOperands(Blob b);
    std::size_t nChildBlobs(ExprView e);
    std::size_t nChildBlobs(Blob b);

    /* Access to the sub-parts of types with > 1 Blob, like composites, large integers etc. Parameter n must be
     * positive. UB if there is no corresponding subpart, e.g. nth(smallIntExpr, 100). Note that this is not random
     * access, but worst case linear time complexity. */
    ExprView nth(ExprView e, std::uint32_t n);
    ExprView first(ExprView e);
    ExprView second(ExprView e);
}
