#pragma once

#include <boost/callable_traits/args.hpp>
#include <boost/hana/basic_tuple.hpp>
#include <boost/hana/concat.hpp>
#include <boost/hana/difference.hpp>
#include <boost/hana/find.hpp>
#include <boost/hana/fold.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/range.hpp>
#include <boost/hana/set.hpp>
#include <boost/hana/size.hpp>
#include <boost/hana/unpack.hpp>
#include <concepts>
#include <type_traits>

namespace sym2 {
    enum class PredicateExprType { leaf, logicalNot, logicalAnd, logicalOr };

    template <auto fct, class... Arg>
    requires std::predicate<decltype(fct), Arg...> struct Predicate;

    template <PredicateExprType Kind, class... T>
    struct PredicateExpr {
        static const PredicateExprType kind = Kind;
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
            if constexpr (Kind == PredicateExprType::logicalAnd && OtherKind == PredicateExprType::logicalAnd) {
                const auto joined = boost::hana::concat(operands, rhs.operands);
                return PredicateExpr<PredicateExprType::logicalAnd, T..., S...>{joined};
            } else
                return PredicateExpr<PredicateExprType::logicalAnd, PredicateExpr<Kind, T...>,
                  PredicateExpr<OtherKind, S...>>{*this, rhs};
        }

        template <PredicateExprType OtherKind, class... S>
        constexpr auto operator||(const PredicateExpr<OtherKind, S...>& rhs) const
        {
            if constexpr (Kind == PredicateExprType::logicalOr && OtherKind == PredicateExprType::logicalOr) {
                const auto joined = boost::hana::concat(operands, rhs.operands);
                return PredicateExpr<PredicateExprType::logicalOr, T..., S...>{joined};
            } else
                return PredicateExpr<PredicateExprType::logicalOr, PredicateExpr<Kind, T...>,
                  PredicateExpr<OtherKind, S...>>{*this, rhs};
        }

        template <auto fct, class... Arg>
        constexpr auto operator&&(const Predicate<fct, Arg...>& rhs) const
        {
            return PredicateExpr<PredicateExprType::logicalAnd, PredicateExpr<Kind, T...>, Predicate<fct, Arg...>>{
              *this, rhs};
        }

        template <auto fct, class... Arg>
        constexpr auto operator||(const Predicate<fct, Arg...>& rhs) const
        {
            return PredicateExpr<PredicateExprType::logicalOr, PredicateExpr<Kind, T...>, Predicate<fct, Arg...>>{
              *this, rhs};
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
    requires std::predicate<decltype(fct), Arg...> struct Predicate {
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
            using PredicateType =
              typename detail::DeducePredicateType<fct, boost::callable_traits::args_t<decltype(fct)>>::Type;

            return PredicateType{};
        } else {
            return Predicate<fct, ExplicitArgs...>{};
        }
    }

    namespace detail {
        namespace hana = boost::hana;

        template <class Set, auto fct, class... Arg>
        constexpr auto extractLeafs(const Set& collect, const Predicate<fct, Arg...>&)
        {
            return hana::insert(collect, hana::type_c<Predicate<fct, Arg...>>);
        }

        template <class Set, PredicateExprType Kind, class... T>
        constexpr auto extractLeafs(const Set& collect, const PredicateExpr<Kind, T...>& expr)
        {
            constexpr auto recurse = [](const auto& collect, const auto& operand) {
                return extractLeafs(collect, operand);
            };

            return hana::fold_left(expr.operands, collect, recurse);
        }

        template <PredicateOperand T>
        constexpr auto extractLeafs(const T& expr)
        {
            return extractLeafs(hana::make_set(), expr);
        }

        template <class FirstLeafValueMap, class SecondLeafValueMap, auto fct, class... Arg>
        static constexpr bool eval(
          const FirstLeafValueMap& first, const SecondLeafValueMap& second, const Predicate<fct, Arg...>&)
        {
            const auto key = hana::type_c<Predicate<fct, Arg...>>;
            const auto firstLookup = hana::find(first, key);
            const auto secondLookup = hana::find(second, key);

            if constexpr (!(firstLookup == hana::nothing))
                return firstLookup.value();
            else if constexpr (!(secondLookup == hana::nothing))
                return secondLookup.value();

            return false;
        }

        template <class FirstLeafValueMap, class SecondLeafValueMap, PredicateExprType Kind, class... T>
        static constexpr bool eval(
          const FirstLeafValueMap& first, const SecondLeafValueMap& second, const PredicateExpr<Kind, T...>& expr)
        {
            if (Kind == PredicateExprType::leaf) {
                return eval(first, second, hana::at(expr.operands, hana::int_c<0>));
            } else if (Kind == PredicateExprType::logicalNot)
                return !eval(first, second, hana::at(expr.operands, hana::int_c<0>));
            else if (Kind == PredicateExprType::logicalAnd) {
                return hana::unpack(
                  expr.operands, [&first, &second](auto&&... op) { return (... && eval(first, second, op)); });
            } else if (Kind == PredicateExprType::logicalOr) {
                return hana::unpack(
                  expr.operands, [&first, &second](auto&&... op) { return (... || eval(first, second, op)); });
            }

            return false;
        }

        template <std::size_t N, class Fct>
        constexpr auto unpackRange(Fct&& f)
        {
            const auto range = hana::make_range(hana::int_c<0>, hana::int_c<N>);

            return hana::unpack(range, std::forward<Fct>(f));
        }

        template <auto to, auto from>
        struct ImplicitlyConvertible {
            /* This is a brute-force SAT solver that evaluates whether the predicate expression `to`
             * is true for (a) every possible combination of predicates results in `from` that lead
             * to `from` being true _and_ (b) every possible combination of predicates in `to` that
             * are not in `from`. */

            static constexpr auto fromLeavesSet = extractLeafs(from);
            static constexpr auto fromLeaves = hana::to<hana::tuple_tag>(fromLeavesSet);
            static constexpr auto toRestLeavesSet = hana::difference(extractLeafs(to), fromLeavesSet);
            static constexpr auto toRestLeaves = hana::to<hana::tuple_tag>(toRestLeavesSet);
            static constexpr std::size_t nFromLeaves = hana::size(fromLeaves);
            static constexpr std::size_t nRestToLeaves = hana::size(toRestLeaves);

            static constexpr bool bruteForceSAT()
            {
                for (std::size_t combination = 0; combination < (1 << nFromLeaves); combination++) {
                    const auto fromTrueFalse = unpackRange<nFromLeaves>([combination](auto&&... i) {
                        return hana::make_map(hana::make_pair(hana::at(fromLeaves, i), combination & (1 << i))...);
                    });

                    if (!eval(fromTrueFalse, hana::make_map(), from))
                        /* We are only interested in predicate combinations that fulfill the
                         * condition. */
                        continue;

                    for (std::size_t remaining = 0; remaining < (1 << nRestToLeaves); remaining++) {
                        const auto remainingTrueFalse = unpackRange<nRestToLeaves>([remaining](auto&&... i) {
                            return hana::make_map(hana::make_pair(hana::at(toRestLeaves, i), remaining & (1 << i))...);
                        });

                        if (!eval(fromTrueFalse, remainingTrueFalse, to))
                            /* For the given predicate results, `from` already holds. If `to` does
                             * not, there is no point in continuing to try. */
                            return false;
                    }
                }

                return true;
            }

            static constexpr bool value = bruteForceSAT();
        };
    }

    template <auto To, auto From>
    constexpr inline bool implicitlyConvertible = detail::ImplicitlyConvertible<To, From>::value;
}
