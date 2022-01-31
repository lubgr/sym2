#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include "predicateexpr.h"
#include "violationhandler.h"

namespace sym2 {
    struct Blob;

    class ConstBlobIterator : public boost::stl_interfaces::iterator_interface<ConstBlobIterator,
                                std::contiguous_iterator_tag, const Blob> {
        /* Simple iterator based on the example in the STL interfaces library. While this could be a
         * template, it won't be reused as anything else, so we prefer simplicity and nicer error
         * messages. The only notable detail of this iterator is that it can't be used for mutable
         * access - everything ist const. */
      public:
        ConstBlobIterator() = default;
        explicit ConstBlobIterator(const Blob* b) noexcept;

        const Blob& operator*() const noexcept;
        ConstBlobIterator& operator+=(std::ptrdiff_t n) noexcept;
        difference_type operator-(ConstBlobIterator other) const noexcept;

      private:
        const Blob* b = nullptr;
    };

    namespace detail {
        // We need this being defined in a TU where Blob is know.
        const Blob* nextBlobHelper(const Blob* current, std::size_t n);
    }

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

        auto data() const noexcept
        {
            return &*first;
        }

        template <PredicateTag auto fromTag>
        ExprView(ExprView<fromTag> other) noexcept
            : first{other.first}
            , sentinel{other.sentinel}
        {
            assertTagWithStacktrace(*this);
        }

        // TODO only required until libstdc++ implements https://wg21.link/P2325R3
        ExprView() = default;

      private:
        friend class Expr;
        friend class ExprLiteral;
        friend class ChildIterator;
        template <PredicateTag auto fromTag>
        friend class ExprView;

        ExprView(const Blob* first, std::size_t n) noexcept
            : first{first}
            , sentinel{detail::nextBlobHelper(first, n)}
        {
            assertTagWithStacktrace(*this);
        }

        ConstBlobIterator first;
        ConstBlobIterator sentinel;
    };

    bool operator==(ExprView<> lhs, ExprView<> rhs);
    bool operator!=(ExprView<> lhs, ExprView<> rhs);

}
