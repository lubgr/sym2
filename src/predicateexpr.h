#pragma once

#include <boost/callable_traits/args.hpp>
#include <boost/hana/basic_tuple.hpp>
#include <boost/hana/concat.hpp>
#include <boost/hana/size.hpp>
#include <boost/hana/unpack.hpp>
#include <concepts>
#include <functional>
#include <type_traits>
#include "stdbackport.h"

namespace sym2 {
    enum class PredicateExprType { leaf, logicalNot, logicalAnd, logicalOr };

    template <auto fct, class... Arg>
    requires stdbackport::predicate<decltype(fct), Arg...>
    struct Predicate;

    template <PredicateExprType Kind, class... T>
    struct PredicateExpr {
        /* Must be a structural type, so std::tuple doesn't work: */
        const boost::hana::basic_tuple<T...> operands;

        template <auto fct, class... Arg>
        constexpr PredicateExpr(const Predicate<fct, Arg...>&);
        constexpr explicit PredicateExpr(const boost::hana::basic_tuple<T...>& operands)
            : operands{operands}
        {}
        constexpr PredicateExpr(const T&... operands)
            : operands{operands...}
        {}

        template <PredicateExprType OtherKind, class... S>
        constexpr auto operator&&(const PredicateExpr<OtherKind, S...>& rhs) const
        {
            if constexpr (Kind == PredicateExprType::logicalAnd
              && OtherKind == PredicateExprType::logicalAnd) {
                const auto joined = boost::hana::concat(operands, rhs.operands);
                return PredicateExpr<PredicateExprType::logicalAnd, T..., S...>{joined};
            } else
                return PredicateExpr<PredicateExprType::logicalAnd, PredicateExpr<Kind, T...>,
                  PredicateExpr<OtherKind, S...>>{*this, rhs};
        }

        template <PredicateExprType OtherKind, class... S>
        constexpr auto operator||(const PredicateExpr<OtherKind, S...>& rhs) const
        {
            if constexpr (Kind == PredicateExprType::logicalOr
              && OtherKind == PredicateExprType::logicalOr) {
                const auto joined = boost::hana::concat(operands, rhs.operands);
                return PredicateExpr<PredicateExprType::logicalOr, T..., S...>{joined};
            } else
                return PredicateExpr<PredicateExprType::logicalOr, PredicateExpr<Kind, T...>,
                  PredicateExpr<OtherKind, S...>>{*this, rhs};
        }

        template <auto fct, class... Arg>
        constexpr auto operator&&(const Predicate<fct, Arg...>& rhs) const
        {
            return PredicateExpr<PredicateExprType::logicalAnd, PredicateExpr<Kind, T...>,
              Predicate<fct, Arg...>>{*this, rhs};
        }

        template <auto fct, class... Arg>
        constexpr auto operator||(const Predicate<fct, Arg...>& rhs) const
        {
            return PredicateExpr<PredicateExprType::logicalOr, PredicateExpr<Kind, T...>,
              Predicate<fct, Arg...>>{*this, rhs};
        }

        constexpr auto operator!() const
        {
            return PredicateExpr<PredicateExprType::logicalNot, PredicateExpr<Kind, T...>>{*this};
        }
    };

    namespace detail {
        template <class T>
        struct IsValidOperator : std::false_type {};

        template <auto fct, class... Arg>
        struct IsValidOperator<Predicate<fct, Arg...>> : std::true_type {};

        template <PredicateExprType Kind, class... T>
        struct IsValidOperator<PredicateExpr<Kind, T...>> : std::true_type {};
    }

    template <class Operand>
    concept PredicateOperand = detail::IsValidOperator<Operand>::value;

    template <auto fct, class... Arg>
    requires stdbackport::predicate<decltype(fct), Arg...>
    struct Predicate {
        /* Leaf type for predicates in expressions. Its main purpose is to allow for extracting the
         * function NTTP, which wouldn't be as easy with the full-blown PredicateExpr. */
        constexpr auto operator&&(const PredicateOperand auto& rhs) const
        {
            return construct<PredicateExprType::logicalAnd>(rhs);
        }

        constexpr auto operator||(const PredicateOperand auto& rhs) const
        {
            return construct<PredicateExprType::logicalOr>(rhs);
        }

        constexpr auto operator!() const
        {
            return PredicateExpr<PredicateExprType::logicalNot, Predicate<fct, Arg...>>{*this};
        }

      private:
        template <PredicateExprType Kind, PredicateOperand Other>
        constexpr auto construct(const Other& rhs) const
        {
            return PredicateExpr<Kind, Predicate<fct, Arg...>, Other>{*this, rhs};
        }
    };

    template <PredicateExprType Kind, class... T>
    template <auto fct, class... Arg>
    constexpr PredicateExpr<Kind, T...>::PredicateExpr(const Predicate<fct, Arg...>& leaf)
        : operands{leaf}
    {}

    namespace detail {
        template <auto fct, class Tuple>
        struct DeducePredicateType;

        template <auto fct, template <class...> class Tuple, class... Arg>
        struct DeducePredicateType<fct, Tuple<Arg...>> {
            using Type = Predicate<fct, Arg...>;
        };
    }

    template <auto fct, class... ExplicitArgs>
    constexpr auto predicate()
    {
        if constexpr (sizeof...(ExplicitArgs) == 0) {
            using PredicateType = typename detail::DeducePredicateType<fct,
              boost::callable_traits::args_t<decltype(fct)>>::Type;

            return PredicateType{};
        } else {
            return Predicate<fct, ExplicitArgs...>{};
        }
    }

    /* We will need this for templates with default non-type template parameters. The default value
     * is likely to be completely unconstrained w.r.t. what the default predicate implies. While it
     * was more natural to have a any-Predicate instances as the default, this cannot work because
     * this any-Predicate can't be declared without the type in question being at least
     * forward-declared. Hence, we need another, unrelated type/flag that acts as a replacement for
     * the any-predicate. The machinery for determining explicitness of constructors doesn't account
     * for this type, so we have to filter it out beforehand. */
    constexpr inline enum class AnyFlag { any } any{AnyFlag::any};

    template <class T>
    concept PredicateTag = PredicateOperand<T> || std::is_same_v<T, AnyFlag>;

    namespace detail
    {
        template <auto fct, class... Arg, class... Actual>
        auto invokeEval(const Predicate<fct, Arg...>&, Actual&&... arg)
        {
            return std::invoke(fct, std::forward<Actual>(arg)...);
        }

        template <PredicateExprType Kind, class... T, class... Arg>
        auto invokeEval(const PredicateExpr<Kind, T...>& expr, Arg&&... arg)
        {
            if constexpr (Kind == PredicateExprType::leaf)
                return invokeEval(
                  boost::hana::at(expr.operands, boost::hana::int_c<0>), std::forward<Arg>(arg)...);
            else if constexpr (Kind == PredicateExprType::logicalNot)
                return !invokeEval(
                  boost::hana::at(expr.operands, boost::hana::int_c<0>), std::forward<Arg>(arg)...);
            else if constexpr (Kind == PredicateExprType::logicalAnd)
                return boost::hana::unpack(expr.operands, [&arg...](auto&&... op) {
                    return (... && invokeEval(op, std::forward<Arg>(arg)...));
                });
            else if constexpr (Kind == PredicateExprType::logicalOr)
                return boost::hana::unpack(expr.operands, [&arg...](auto&&... op) {
                    return (... || invokeEval(op, std::forward<Arg>(arg)...));
                });
        }

        template <PredicateOperand auto what>
        struct IsInvoker {
            /* This could be a function template, but it would be overly verbose to pass this as a
             * higher order function, because all function parameters (...Arg) need to be specified.
             * With the distinct type and an suitable variable template, this is easier to use and
             * more concise. */
            template <class... Arg>
            bool operator()(Arg&&... arg) const
            {
                return invokeEval(what, std::forward<Arg>(arg)...);
            }
        };

        template <PredicateOperand auto what>
        struct AreAllInvoker {
            /* Only works for predicate expressions with one single argument. With more arguments,
             * it's unclear how to pass the function arguments. */
            template <class... Arg>
            bool operator()(Arg&&... arg) const
            {
                return (... && detail::invokeEval(what, std::forward<Arg>(arg)));
            }
        };

        template <PredicateOperand auto what>
        struct IsOneOfInvoker {
            /* Only works for predicate expressions with one single argument. */
            template <class... Arg>
            bool operator()(Arg&&... arg) const
            {
                return (... || detail::invokeEval(what, std::forward<Arg>(arg)));
            }
        };

    }

    template <PredicateOperand auto what>
    constexpr inline detail::IsInvoker<what> is{};

    template <PredicateOperand auto what>
    constexpr inline detail::AreAllInvoker<what> areAll{};

    template <PredicateOperand auto what>
    constexpr inline detail::IsOneOfInvoker<what> isOneOf{};
}
