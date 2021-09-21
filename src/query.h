#pragma once

#include <cstdint>
#include <string_view>
#include "view.h"

namespace sym2 {
    Type type(ExprView<> e);
    Flag flags(ExprView<> e);

    struct BaseExp {
        ExprView<> base;
        ExprView<> exponent;
    };

    BaseExp asPower(ExprView<> e);

    std::size_t nLogicalOperands(ExprView<> e);
    std::size_t nLogicalOperands(Blob b);
    std::size_t nChildBlobs(ExprView<> e);
    std::size_t nChildBlobs(Blob b);

    /* Access to the sub-parts of types with > 1 Blob, like composites, large rationals etc. Important: these functions
     * don't consider logical operands, but physical children. Hence, this is a low-level function with a usage that
     * requires knoledge of the underlying data layout. Example: the small rational number 2/3 doesn't have any child
     * blobs, because everything is stored within the first and only blob. The complex number 2i has two child blobs, 2
     * and 0.
     *
     * Parameter n must be positive. UB if there is no corresponding subpart, e.g. nth(smallIntExpr, 100). Note that
     * this is not random access, but worst case linear time complexity. */
    ExprView<> nth(ExprView<> e, std::uint32_t n);
    ExprView<> first(ExprView<> e);
    ExprView<> second(ExprView<> e);
}
