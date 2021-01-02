#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <cassert>
#include <cstdint>
#include "blob.h"
#include "query.h"

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

    class ExprView
        : public boost::stl_interfaces::view_interface<ExprView, boost::stl_interfaces::element_layout::contiguous> {
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

      private:
        friend class Expr;
        friend class OperandIterator;

        ExprView() = default;
        ExprView(const Blob* first, std::size_t n) noexcept
            : first{first}
            , sentinel{first + n}
        {}

        ConstBlobIterator first{};
        ConstBlobIterator sentinel{};
    };

    bool operator==(ExprView lhs, ExprView rhs);
    bool operator!=(ExprView lhs, ExprView rhs);

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
}
