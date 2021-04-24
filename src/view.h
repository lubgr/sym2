#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <cstdint>
#include <type_traits>
#include "blob.h"
#include "predicateexprsat.h"
#include "violationhandler.h"

namespace sym2 {
    class ConstBlobIterator : public boost::stl_interfaces::iterator_interface<ConstBlobIterator,
                                std::random_access_iterator_tag, const Blob> {
        /* Simple iterator based on the example in the STL interfaces library. While this could be a template, it won't
         * be reused as anything else, so we prefer simplicity and nicer error messages. The only notable detail of this
         * iterator is that it can be used for mutable access - everything ist const. */
      public:
        ConstBlobIterator() = default;
        explicit ConstBlobIterator(const Blob* b) noexcept
            : b{b}
        {}

        const Blob& operator*() const noexcept
        {
            return *b;
        }

        ConstBlobIterator& operator+=(std::ptrdiff_t n) noexcept
        {
            b += n;
            return *this;
        }

        auto operator-(ConstBlobIterator other) const noexcept
        {
            return b - other.b;
        }

      private:
        const Blob* b = nullptr;
    };

    template <PredicateTag auto tag = any>
    class ExprView : public boost::stl_interfaces::view_interface<ExprView<tag>,
                       boost::stl_interfaces::element_layout::contiguous> {
      public:
        /* Necessary at least for Boost range compatibility: */
        using const_iterator = ConstBlobIterator;

        auto begin() const noexcept
        {
            return first;
        }

        auto end() const noexcept
        {
            return sentinel;
        }

        template <PredicateTag auto fromTag>
        explicit(needsExplicitCtor<tag, fromTag>) ExprView(ExprView<fromTag> other) noexcept
            : first{other.first}
            , sentinel{other.sentinel}
        {
            enforceTag(*this);
        }

      private:
        friend class Expr;
        friend class OperandIterator;
        template <PredicateTag auto fromTag>
        friend class ExprView;

        ExprView(const Blob* first, std::size_t n) noexcept
            : first{first}
            , sentinel{first + n}
        {
            enforceTag(*this);
        }

        ConstBlobIterator first;
        ConstBlobIterator sentinel;
    };

    template <PredicateOperand auto newTag>
    auto tag(ExprView<> e)
    {
        return ExprView<newTag>{e};
    }

    bool operator==(ExprView<> lhs, ExprView<> rhs);
    bool operator!=(ExprView<> lhs, ExprView<> rhs);

}
