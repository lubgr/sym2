#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <cassert>
#include <cstdint>
#include "query.h"
#include "view.h"

namespace sym2 {
    class OperandIterator : public boost::stl_interfaces::proxy_iterator_interface<OperandIterator,
                              std::forward_iterator_tag, ExprView<>> {
        /* Proxy iterator that traverses only through root operands, i.e., those Blob instances that define the header
         * of the sub-expression they belong to. As an example. consider the sum 2*(b + c) + d*e. When iterating over
         * the operands of that sum with a OperandIterator, it traverses through 2*(b + c)  and d*e. As the step size is
         * varies and must be constructed from individual root Blob instances, this can only be a forward iterator (as
         * we don't want to store additional state apart from a pointer. */
      public:
        OperandIterator() = default;
        explicit OperandIterator(ExprView<> e) noexcept
            : op{type(e) == Type::function ? &e[2] : &e[1]}
            , n{nLogicalOperands(e)}
        {
            assert(e.size() >= 1);
        }

        /* Circumvents the constructor logic and treats the expression as the single operand of a theoretical composite.
         * Example: when passing the symbol a, the resulting iterator points to a and becomes a sentinel after
         * incrementing. When passing 2(b + c) + d*e, it points to 2(b + c) + d*e as one single 'operator', and is a
         * sentinel after incrementing. */
        static OperandIterator single(ExprView<> e) noexcept
        {
            OperandIterator result{};

            result.op = &e[0];
            result.n = 1;

            return result;
        }

        ExprView<> operator*() const noexcept
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
          ExprView<>>::operator++;

        friend bool operator==(OperandIterator lhs, OperandIterator rhs) noexcept
        {
            return lhs.op == rhs.op && lhs.n == rhs.n;
        }

      private:
        friend class OperandsView;

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
        explicit OperandsView(ExprView<> e) noexcept
            : first{e}
        {}

        static OperandsView single(ExprView<> e) noexcept
        {
            OperandsView result{};

            result.first = OperandIterator::single(e);

            return result;
        }

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

        std::size_t size() const noexcept
        {
            return first.n;
        }

        /* UB if the requested subview is out-of-range. */
        auto subview(std::size_t offset, std::size_t count = -1) const noexcept
        {
            constexpr std::size_t npos = -1;
            OperandsView result{};

            assert(offset < size());
            assert(count == npos || offset + count <= size());

            result.first = std::next(first, offset);
            result.sentinel = count == npos ? sentinel : std::next(result.first, count);

            return result;
        }

      private:
        OperandIterator first{};
        OperandIterator sentinel{};
    };
}
