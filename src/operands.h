#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <cassert>
#include <cstdint>
#include <span>
#include "expr.h"
#include "predicates.h"
#include "query.h"
#include "view.h"

namespace sym2 {
    class OperandIterator : public boost::stl_interfaces::proxy_iterator_interface<OperandIterator,
                              std::forward_iterator_tag, ExprView<>> {
        /* Proxy iterator that is able to traverse through root operands, i.e., those Blob instances that define the
         * header of the sub-expression they belong to. Can be constructed to refer to a single element as well, or
         * refer to a sequence of owning Expr instances. Constructible through named static functions.
         *
         * As the step size varies when constructed from a root Blob composite, this can only be a forward iterator (as
         * we don't want to store additional state apart from a pointer. */
      public:
        OperandIterator() = default;

        /* Creates an iterator over all operands of the given composite. Example: consider the sum 2*(b + c) + d*e. When
         * iterating over the operands of that sum with a OperandIterator, it traverses through two expressions,
         * 2*(b + c)  and d*e. */
        static OperandIterator fromComposite(ExprView<> e) noexcept;
        /* Treats the expression as the single operand. Example: when passing the symbol a, the resulting iterator
         * points to a and becomes a sentinel after incrementing. When passing 2(b + c) + d*e, it points to 2(b + c) +
         * d*e as one single 'operator', and is a sentinel after incrementing, too. */
        static OperandIterator fromSingle(ExprView<> e) noexcept;
        /* Returns fromComposite when the argument is a composite and fromSingle otherwise. */
        static OperandIterator fromCompositeOrSingle(ExprView<> e) noexcept;
        /* Same as the operand for composites, except there is no root composite Blob: */
        static OperandIterator fromSequence(std::span<const Expr> expressions) noexcept;

        ExprView<> operator*() const noexcept;
        OperandIterator& operator++() noexcept;

        /* Necessary because the above operator++ hides the one in the base class, as mentioned in the docs. */
        using boost::stl_interfaces::proxy_iterator_interface<OperandIterator, std::forward_iterator_tag,
          ExprView<>>::operator++;

        friend bool operator==(OperandIterator lhs, OperandIterator rhs) noexcept
        {
            return lhs.op == rhs.op && lhs.n == rhs.n;
        }

      private:
        friend class OperandsView;

        OperandIterator(const Blob* op, std::size_t n);

        std::size_t currentSize() const noexcept;
        const Blob* op = nullptr;
        std::size_t n = 0;
    };

    class OperandsView : public boost::stl_interfaces::view_interface<OperandsView> {
      public:
        OperandsView() = default;

        static OperandsView fromComposite(ExprView<> e) noexcept;
        static OperandsView fromSingle(ExprView<> e) noexcept;
        static OperandsView fromCompositeOrSingle(ExprView<> e) noexcept;
        static OperandsView fromSequence(std::span<const Expr> expressions) noexcept;

        /* Necessary at least for Boost range compatibility: */
        using const_iterator = OperandIterator;

        OperandIterator begin() const noexcept;
        OperandIterator end() const noexcept;
        std::size_t size() const noexcept;

        /* UB if the requested subview is out-of-range. */
        OperandsView subview(std::size_t offset, std::size_t count = -1) const noexcept;

      private:
        explicit OperandsView(OperandIterator first) noexcept;

        OperandIterator first{};
        OperandIterator sentinel{};
    };
}
