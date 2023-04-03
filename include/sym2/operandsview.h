#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <cstdint>
#include <ranges>
#include "childiterator.h"

namespace sym2 {
    class OperandsView : public std::ranges::view_interface<OperandsView> {
      public:
        OperandsView() = default;

        // See ChildIterator for an explanation of the different named constructors.
        static OperandsView operandsOf(ExprView<> e) noexcept;
        static OperandsView singleOperand(ExprView<> e) noexcept;

        // Necessary at least for Boost range compatibility:
        using const_iterator = ChildIterator;

        ChildIterator begin() const noexcept;
        ChildIterator end() const noexcept;
        std::size_t size() const noexcept;
        bool empty() const noexcept;

        // UB if the requested subview is out-of-range.
        OperandsView subview(std::size_t offset, std::size_t count = -1) const noexcept;

        bool operator==(OperandsView rhs) const noexcept;
        bool operator!=(OperandsView rhs) const noexcept;

      private:
        OperandsView(ChildIterator first, ChildIterator last) noexcept;

        ChildIterator first{};
        ChildIterator sentinel{};
    };
}
