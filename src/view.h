#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <cstdint>
#include "blob.h"
#include "predicateexprsat.h"

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

    template <class... Tag>
    class Tagged : public boost::stl_interfaces::view_interface<Tagged<Tag...>,
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

        template <class... Other>
        explicit(false) Tagged(Tagged<Other...> other) noexcept
            : first{other.first}
            , sentinel{other.sentinel}
        {}

      private:
        friend class Expr;
        friend class OperandIterator;
        template <class... Other>
        friend class Tagged;

        Tagged() = default;
        Tagged(const Blob* first, std::size_t n) noexcept
            : first{first}
            , sentinel{first + n}
        {}

        ConstBlobIterator first{};
        ConstBlobIterator sentinel{};
    };

    template <class... Tag>
    using ExprView = Tagged<Tag...>;

    template <class... Tag>
    auto tag(ExprView<> e)
    {
        return e;
    }

    bool operator==(ExprView<> lhs, ExprView<> rhs);
    bool operator!=(ExprView<> lhs, ExprView<> rhs);

}
