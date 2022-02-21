#pragma once

#include <array>
#include <string_view>
#include "predicateexpr.h"
#include "view.h"

namespace sym2 {
    // Stores small integers, small rationals, floating points or symbol names as one single blob.
    // Doesn't have a usable interface on its own, but offers implicit conversion to ExprView<...>.
    // The main purpose of ExprLiteral instances is to be independent of the allocators and their
    // buffer lifetimes that Expr uses.
    //
    // An ExprLiteral allows short literal-like leaf expressions to be used without the need to
    // allocate a buffer of size 1, which happens when constructing an Expr. ExprView instantiations
    // can be created from objects of this class, so we can use them e.g. as local static instances
    // in functions that return either a view onto some expression received as an argument or a
    // constant like zero or one. Note that this class currently cannot provide a constexpr API
    // since the Blob definition is not exposed but only visible in the TU.
    //
    // Another way to implementt this feature was to have a small Blob buffer optimisation inside
    // Expr. However, this would complicate the Expr implementation and require branching when
    // constructing ExprViews from Expr objects, and whether that's preferrable needs further study
    // and most importantly, benchnmarking.
    class ExprLiteral {
      public:
        explicit ExprLiteral(std::string_view symbol);
        explicit ExprLiteral(std::int32_t n);
        ExprLiteral(std::int32_t num, std::int32_t denom);
        explicit ExprLiteral(double n);

        // Implicit conversions to any tag are indeed desired here:
        template <PredicateTag auto tag>
        operator ExprView<tag>() const
        {
            return ExprView<tag>{view()};
        }
        ExprView<> view() const;

      private:
        alignas(8) std::array<std::byte, 16> blob;
    };

    ExprLiteral operator"" _ex(const char* str, std::size_t);
    ExprLiteral operator"" _ex(unsigned long long n);
    ExprLiteral operator"" _ex(long double n);
}
