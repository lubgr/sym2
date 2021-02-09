#pragma once

#include <functional>
#include "view.h"

namespace sym2 {
    /* Those tags won't be defined anywhere. They serve as a tag only. For every such tag, there must be a template
     * specialization isTag<...> as below. */

    struct Real;
    struct Complex;

    struct Number;
    struct Int;
    struct Rational;
    struct Double;

    struct Small; /* Everything that fits into 1 Blob. */
    struct Large; /* Everything > 1 Blob. */

    struct Scalar; /* A leaf in any expression tree. */
    struct Symbol;
    struct Constant;
    struct Sum;
    struct Product;
    struct Power;
    struct Function;

    struct NumericallyEvaluable;
    struct Positive;
    struct Negative;

    namespace detail {
        template <class Tag>
        bool isTag(ExprView) = delete;

        template <>
        bool isTag<Real>(ExprView e);
        template <>
        bool isTag<Complex>(ExprView e);
        template <>
        bool isTag<Number>(ExprView e);
        template <>
        bool isTag<Int>(ExprView e);
        template <>
        bool isTag<Rational>(ExprView e);
        template <>
        bool isTag<Double>(ExprView e);
        template <>
        bool isTag<Small>(ExprView e);
        template <>
        bool isTag<Large>(ExprView e);
        template <>
        bool isTag<Scalar>(ExprView e);
        template <>
        bool isTag<Symbol>(ExprView e);
        template <>
        bool isTag<Constant>(ExprView e);
        template <>
        bool isTag<Sum>(ExprView e);
        template <>
        bool isTag<Product>(ExprView e);
        template <>
        bool isTag<Power>(ExprView e);
        template <>
        bool isTag<Function>(ExprView e);
        template <>
        bool isTag<NumericallyEvaluable>(ExprView e);
        template <>
        bool isTag<Positive>(ExprView e);
        template <>
        bool isTag<Negative>(ExprView e);

        template <class Op, class FirstTag, class... RestTag>
        struct TypeQuery {
            /* We implement this as a function object to enable a two-step variadic template instantiation. The first
             * one (the types of this class) is explicit, while the second (the types of the Tagged<...> argument that
             * we wish to query is deduced. Having both variadic templates in one signature doesn't work (only with
             * workarounds that are unacceptable here), so this is preferred. It can be nicely exposed available through
             * a short template variable. */

            template <class... Tag>
            bool operator()(Tagged<Tag...> e) const
            {
                /* It's somewhat ironic that we cast all tags away now. Unfortunately, this is necessary for choosing
                 * the correct template specialization of isTag, which can't preserve template-ness on the Tags as this
                 * becomes a partial function specialization, which isn't allowed. However, as the implementation of the
                 * type queries is treated as an implementation detail of all is<Tag>(...) instantiations/calls, we
                 * should be able to live with that. */
                return this->operator()(ExprView{e});
            }

            /* The above doesn't capture implicit coversions from Expr, but this one does: */
            bool operator()(ExprView e) const
            {
                if constexpr (sizeof...(RestTag) == 0)
                    return isTag<FirstTag>(e);
                else
                    return Op{}(isTag<FirstTag>(e), TypeQuery<Op, RestTag...>{}(e));
            }
        };
    }

    template <class... Tag>
    inline constexpr auto is = detail::TypeQuery<std::logical_and<>, Tag...>{};
    template <class... Tag>
    inline constexpr auto isOneOf = detail::TypeQuery<std::logical_or<>, Tag...>{};

}
