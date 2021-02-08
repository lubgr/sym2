#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <cassert>
#include <cstdint>
#include "query.h"
#include "view.h"

namespace sym2 {
    class OperandIterator
        : public boost::stl_interfaces::proxy_iterator_interface<OperandIterator, std::forward_iterator_tag, ExprView> {
        /* Proxy iterator that traverses only through root operands, i.e., those Blob instances that define the header
         * of the sub-expression they belong to. As an example. consider the sum 2*(b + c) + d*e. When iterating over
         * the operands of that sum with a OperandIterator, it traverses through 2*(b + c)  and d*e. As the step size is
         * varies and must be constructed from individual root Blob instances, this can only be a forward iterator (as
         * we don't want to store additional state apart from a pointer. */
      public:
        OperandIterator() = default;
        explicit OperandIterator(ExprView e) noexcept
            : op{type(e) == Type::function ? &e[2] : &e[1]}
            , n{nLogicalOperands(e)}
        {
            assert(e.size() >= 1);
        }

        ExprView operator*() const noexcept
        {
            return {op, currentSize()};
        }

        OperandIterator& operator++() noexcept
        {
            assert(op != nullptr);

            if (--n == 0)
                op = nullptr;
            else
                op = op + currentSize();

            return *this;
        }

        /* Necessary because the above operator++ hides the one in the base class, as mentioned in the docs. */
        using boost::stl_interfaces::proxy_iterator_interface<OperandIterator, std::forward_iterator_tag,
          ExprView>::operator++;

        friend bool operator==(OperandIterator lhs, OperandIterator rhs) noexcept
        {
            return lhs.op == rhs.op && lhs.n == rhs.n;
        }

      private:
        std::size_t currentSize() const noexcept
        {
            return nChildBlobs(*op) + 1;
        }

        const Blob* op = nullptr;
        std::size_t n = 0;
    };

    class OperandsView : public boost::stl_interfaces::view_interface<OperandsView> {
      public:
        OperandsView() = default;
        explicit OperandsView(ExprView e) noexcept
            : first{e}
            , sentinel{}
        {}

        /* Necessary at least for Boost range compatibility: */
        using const_iterator = OperandIterator;

        auto begin() const noexcept
        {
            return first;
        }

        auto end() const noexcept
        {
            return sentinel;
        }

      private:
        OperandIterator first{};
        OperandIterator sentinel{};
    };
}
