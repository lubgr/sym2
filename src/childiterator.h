#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include "view.h"

namespace sym2 {
    class ChildIterator
        : public boost::stl_interfaces::proxy_iterator_interface<ChildIterator, std::forward_iterator_tag, ExprView<>> {
        /* Proxy iterator that is able to traverse through both logical and physical children. Can be constructed with
         * static functions, for an expression with children, or treating a single expression as a single child. As the
         * step size can vary when constructed from an expression with children, this can only be a forward iterator (as
         * we don't want to store additional state apart from a pointer. */
      public:
        ChildIterator() = default;

        /* Creates an iterator over all logical children/operands of the given composite. Example: consider the sum
         * 2*(b + c) + d*e. When iterating over the operands of that sum with a ChildIterator, it traverses through two
         * expressions, 2*(b + c)  and d*e. */
        static ChildIterator logicalChildren(ExprView<> e) noexcept;
        /* Creates an iterator over all physical children of the given expression. Example: passing a complex number
         * will create an iterator over real and complex part. Passing a large rational will iterate over numerator and
         * denomiator. For composites with logical children, the behaviour is identical to iterators created with
         * logicalChildren(...); */
        static ChildIterator physicalChildren(ExprView<> e) noexcept;
        /* Treats the expression as the single operand (the physical/logical distinction hence doesn't apply here).
         * Example: when passing the symbol a, the resulting iterator points to a and becomes a sentinel after
         * incrementing. When passing 2(b + c) + d*e, it points to 2(b + c) + d*e as one single 'child', and is a
         * sentinel after incrementing, too. */
        static ChildIterator singleChild(ExprView<> e) noexcept;

        ExprView<> operator*() const noexcept;
        ChildIterator& operator++() noexcept;

        /* Necessary because the above operator++ hides the one in the base class, as mentioned in the docs. */
        using boost::stl_interfaces::proxy_iterator_interface<ChildIterator, std::forward_iterator_tag,
          ExprView<>>::operator++;

        friend bool operator==(ChildIterator lhs, ChildIterator rhs) noexcept
        {
            return lhs.op == rhs.op && lhs.n == rhs.n;
        }

      private:
        friend class OperandsView;

        ChildIterator(const Blob* op, std::size_t n);

        std::size_t currentSize() const noexcept;

        const Blob* op = nullptr;
        std::size_t n = 0;
    };
}
