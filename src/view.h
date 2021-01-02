#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <cassert>
#include <cstdint>
#include "operand.h"
#include "query.h"

namespace sym2 {
    class ConstOpIterator : public boost::stl_interfaces::iterator_interface<ConstOpIterator,
                              std::random_access_iterator_tag, const Operand> {
        /* Simple iterator based on the example in the STL interfaces library. While this could be a template, it won't
         * be reused as anything else, so we prefer simplicity and nicer error messages. The only notable detail of this
         * iterator is that it can be used for mutable access - everything ist const. */
      public:
        ConstOpIterator() = default;
        explicit ConstOpIterator(const Operand* op) noexcept
            : op{op}
        {}

        const Operand& operator*() const noexcept
        {
            return *op;
        }

        ConstOpIterator& operator+=(std::ptrdiff_t n) noexcept
        {
            op += n;
            return *this;
        }

        auto operator-(ConstOpIterator other) const noexcept
        {
            return op - other.op;
        }

      private:
        const Operand* op = nullptr;
    };

    class ExprView
        : public boost::stl_interfaces::view_interface<ExprView, boost::stl_interfaces::element_layout::contiguous> {
      public:
        ExprView() = default;
        ExprView(const Operand* first, std::size_t n) noexcept
            : first{first}
            , sentinel{first + n}
        {}

        /* Necessary at least for Boost range compatibility: */
        using const_iterator = ConstOpIterator;

        auto begin() const noexcept
        {
            return first;
        }

        auto end() const noexcept
        {
            return sentinel;
        }

      private:
        ConstOpIterator first{};
        ConstOpIterator sentinel{};
    };

    bool operator==(ExprView lhs, ExprView rhs);
    bool operator!=(ExprView lhs, ExprView rhs);

    class ConstSemanticOpIterator : public boost::stl_interfaces::proxy_iterator_interface<ConstSemanticOpIterator,
                                      std::forward_iterator_tag, ExprView> {
        /* Proxy iterator that traverses only through root operands, i.e., those Operand instances that define the
         * header of the sub-expression they belong to. As an example. consider the sum 2*(b + c) + d*e. When iterating
         * over the operands of that sum with a ConstSemanticOpIterator, it traverses through 2*(b + c)  and d*e. As the
         * step size is varies and must be constructed from individual root Operand instances, this can only be a
         * forward iterator (as we don't want to store additional state apart from a pointer. */
      public:
        ConstSemanticOpIterator() = default;
        explicit ConstSemanticOpIterator(ExprView e) noexcept
            : op{type(e) == Type::function ? &e[2] : &e[1]}
            , n{nOps(e)}
        {
            assert(e.size() >= 1);
        }

        ExprView operator*() const noexcept
        {
            return {op, currentSize()};
        }

        ConstSemanticOpIterator& operator++() noexcept
        {
            assert(op != nullptr);

            if (--n == 0)
                op = nullptr;
            else
                op = op + currentSize();

            return *this;
        }

        friend bool operator==(ConstSemanticOpIterator lhs, ConstSemanticOpIterator rhs) noexcept
        {
            return lhs.op == rhs.op && lhs.n == rhs.n;
        }

      private:
        std::size_t currentSize() const noexcept
        {
            return nChildBlobs(*op) + 1;
        }

        const Operand* op = nullptr;
        std::size_t n = 0;
    };
}
