#pragma once

#include <boost/mp11/function.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/set.hpp>
#include <boost/mp11/utility.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <cstdint>
#include <type_traits>
#include "blob.h"

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

    struct Or;
    struct Not;

    namespace detail {
        using namespace boost::mp11;

        template <class L>
        using FrontNonEmpty = mp_eval_if<mp_empty<L>, void, mp_front, L>;
        template <class L, class T>
        using StartsWith = mp_same<T, FrontNonEmpty<L>>;
        template <class L>
        using OrCombined = StartsWith<L, Or>;
        template <class L>
        using NotCombined = StartsWith<L, Not>;
        template <class L>
        using AndCombined = mp_not<mp_or<OrCombined<L>, NotCombined<L>>>;
        template <class L>
        using Extract = mp_eval_if<AndCombined<L>, L, mp_rest, L>;

        template <class L1, class L2>
        using Common = mp_set_intersection<Extract<L1>, Extract<L2>>;

        // clang-format off
        template <class To, class From>
        using ImplicitlyFromTo = mp_cond<
          mp_empty<To>, mp_true,
          mp_and<AndCombined<To>, AndCombined<From>>, mp_same<Extract<To>, Common<From, To>>,
          mp_and<NotCombined<To>, NotCombined<From>>, mp_same<Extract<From>, Common<From, To>>,
          NotCombined<To>, mp_empty<Common<From, To>>,
          mp_and<OrCombined<To>, AndCombined<From>>,
              mp_and<mp_bool<mp_size<From>::value == 1>, mp_set_contains<To, FrontNonEmpty<From>>>,
          mp_and<OrCombined<To>, OrCombined<From>>, mp_same<Extract<From>, Common<From, To>>,
          mp_true, mp_false>;
        // clang-format on

        template <class To, class From>
        inline constexpr bool explicitFromTo = !ImplicitlyFromTo<To, From>::value;
    }

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

        /* 1) We want to allow implicit conversion into an untagged view. This is similar to upcasting in an inheritance
         * hierarchy, or to the type with fewer constraints. Functon signatures must make sure this happens in a
         * resposible fashion. Example:
         *
         * - Tagged<> implicit from Tagged<Symbol>
         *
         * 2) If the above doesn't apply, we want to allow for implicit construction of identically tagged types (this
         * is obvious, but needs to be implemented). Examples:
         *
         * - Tagged<Symbol, Positive> implicit from Tagged<Symbol, Positive>
         * - Tagged<Or, Symbol, Number> implicit from Tagged<Or, Symbol, Number>
         *
         * 3) I the above doesn't apply, whenever the from-type has no type, construction must be explicit. Examples:
         *
         * - Tagged<Sum> explicit from Tagged<>
         *
         * 4) If the above doesn't apply, we want an And-tagged target view to be implicitly constructible from another
         * And-tagged view whenever all of the To-tags are within the from-tags. Example
         *
         * - Tagged<Symbol, Positive> implicit from Tagged<Symbol, Positive>
         * - Tagged<Symbol, Positive> implicit from Tagged<Symbol, Positive, Real>
         * - Tagged<Number> implicit from Tagged<Number, Real>
         *
         * 5) If the above doesn't apply, we want a Not-tagged target view to be implicitly constructible from (a)
         * another Not-tagged view that has all of the tags that the target view has (b) a standard- or Or-tagged view
         * that doesn't include one of the inverted target tags. Examples:
         *
         * - Tagged<Not, Number> implicit from Tagged<Symbol>
         * - Tagged<Not, Number, Complex> implicit from Tagged<Or, Symbol, Constant>
         * - Tagged<Not, Number, Complex> implicit from Tagged<Not, Number, Complex, Symbol>
         *
         * - Tagged<Not, Number> explicit from Tagged<Number>
         * - Tagged<Not, Number> explicit from Tagged<Or, Number, Symbol>
         *
         * 6) If the above doesn't apply, we want an Or-tagged view to be implicitly constructible from (a) an
         * And-tagged view with one Tag that is in the target tag list (b) another Or-tagged view that has all its tags
         * in the target tag list. Examples:
         *
         * - Tagged<Or, Number, Symbol, Constant> implicit from Tagged<Symbol>
         * - Tagged<Or, Number, Symbol, Constant> implicit from Tagged<Or, Symbol, Constant>
         *
         * - Tagged<Or, Number, Symbol> explicit from Tagged<Or, Number, Constant>
         * */
        template <class... Other>
        explicit(detail::explicitFromTo<Tagged<Other...>, Tagged<Tag...>>) Tagged(Tagged<Other...> other) noexcept
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

    using ExprView = Tagged<>;
    template <class... Tag>
    using TaggedOneOf = Tagged<struct Or, Tag...>;
    template <class... Tag>
    using TaggedNot = Tagged<struct Not, Tag...>;

    template <class... Tag>
    auto tag(ExprView e)
    {
        return Tagged<Tag...>{e};
    }

    bool operator==(ExprView lhs, ExprView rhs);
    bool operator!=(ExprView lhs, ExprView rhs);

}
