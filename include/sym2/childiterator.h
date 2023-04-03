#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include "exprview.h"

namespace sym2 {
    class ChildIterator : public boost::stl_interfaces::proxy_iterator_interface<ChildIterator,
                            std::contiguous_iterator_tag, ExprView<>> {
        // Proxy iterator that is able to traverse through logical children. Can be constructed with
        // static functions, for an expression with children, or treating a single expression as a
        // single child.
      public:
        ChildIterator() = default;

        // Creates an iterator over all logical children/operands of the given composite. Example:
        // consider the sum 2*(b + c) + d*e. When iterating over the operands of that sum with a
        // ChildIterator, it traverses through two expressions, 2*(b + c)  and d*e.
        static ChildIterator logicalChildren(ExprView<> e) noexcept;
        // Treats the expression as the single operand (the physical/logical distinction hence
        // doesn't apply here). Example: when passing the symbol a, the resulting iterator points to
        // a and becomes a sentinel after incrementing. When passing 2(b + c) + d*e, it points to
        // 2(b + c) + d*e as one single 'child', and is a sentinel after incrementing, too.
        static ChildIterator singleChild(ExprView<> e) noexcept;
        // Will return a one-past-the-end iterator:
        static ChildIterator logicalChildrenSentinel(ExprView<> e) noexcept;
        static ChildIterator singleChildSentinel(ExprView<> e) noexcept;

        ExprView<> operator*() const noexcept;
        difference_type operator-(ChildIterator rhs) const noexcept;
        ChildIterator operator+=(std::size_t n) noexcept;

      private:
        friend class OperandsView;

        explicit ChildIterator(const Blob* op);

        const Blob* op = nullptr;
    };
}
