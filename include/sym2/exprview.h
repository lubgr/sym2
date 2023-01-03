#pragma once

#include "predicateexpr.h"
#include "violationhandler.h"

namespace sym2 {
    struct Blob;

    template <PredicateTag auto tag = any>
    class ExprView {
      public:
        explicit ExprView(const Blob* ptr) noexcept
            : ptr{ptr}
        {
            assertTagWithStacktrace(*this);
        }

        template <PredicateTag auto fromTag>
        ExprView(ExprView<fromTag> other) noexcept
            : ptr{other.ptr}
        {
            assertTagWithStacktrace(*this);
        }

        const Blob* get() const
        {
            return ptr;
        }

      private:
        template <PredicateTag auto fromTag>
        friend class ExprView;

        const Blob* ptr;
    };

    bool operator==(ExprView<> lhs, ExprView<> rhs);
    bool operator!=(ExprView<> lhs, ExprView<> rhs);
}
