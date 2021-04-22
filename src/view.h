#pragma once

#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <cassert>
#include <cstdint>
#include <type_traits>
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

    constexpr inline enum class AnyFlag { instance } any{AnyFlag::instance};

    template <class T>
    concept PredicateTag = PredicateOperand<T> || std::is_same_v<T, AnyFlag>;

    template <auto toTag, auto fromTag>
    constexpr inline bool needsExplicitExprViewCtor = !implicitlyConvertible<toTag, fromTag>;

    /* If there are no constraints in the target type, implicit conversions are fine: */
    template <auto fromTag>
    constexpr inline bool needsExplicitExprViewCtor<AnyFlag::instance, fromTag> = false;

    /* ... otherwise, we certainly need to be explicit. While this would be caught by the general case, we need to
     * filter out the any flag type in order to make the SAT machinery work. */
    template <auto toTag>
    constexpr inline bool needsExplicitExprViewCtor<toTag, AnyFlag::instance> = true;

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

        template <PredicateTag auto fromTag>
        explicit(needsExplicitExprViewCtor<tag, fromTag>) ExprView(ExprView<fromTag> other) noexcept
            : first{other.first}
            , sentinel{other.sentinel}
        {
            enforceTag();
        }

      private:
        friend class Expr;
        friend class OperandIterator;
        template <PredicateTag auto fromTag>
        friend class ExprView;

        ExprView()
        {
            enforceTag();
        }

        ExprView(const Blob* first, std::size_t n) noexcept
            : first{first}
            , sentinel{first + n}
        {
            enforceTag();
        }

        void enforceTag()
        {
            if constexpr (!std::is_same_v<decltype(tag), AnyFlag>)
                assert(is<tag>(*this));
        }

        ConstBlobIterator first{};
        ConstBlobIterator sentinel{};
    };

    template <PredicateOperand auto newTag>
    auto tag(ExprView<> e)
    {
        return ExprView<newTag>{e};
    }

    bool operator==(ExprView<> lhs, ExprView<> rhs);
    bool operator!=(ExprView<> lhs, ExprView<> rhs);

}
