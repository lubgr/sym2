
#include "sumsimpl.h"
#include <boost/hof/construct.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include "predicates.h"
#include "query.h"

namespace sym2 {
    namespace {
        std::pmr::vector<Expr> prepend(ExprView<> first, std::pmr::vector<Expr>&& rest)
        {
            std::pmr::vector<Expr> result;

            result.reserve(rest.size() + 1);

            result.emplace_back(first);
            std::move(rest.begin(), rest.end(), std::back_inserter(result));

            return result;
        }

        template <class Iter>
        auto constructIter(Iter op)
        {
            return boost::make_transform_iterator(op, boost::hof::construct<Expr>());
        }

        auto frontAndRest(OperandsView ops)
        {
            assert(ops.size() >= 1);
            return std::make_pair(ops.front(), ops.subview(1));
        }

        template <class T>
        auto frontAndRest(std::span<T> ops)
        {
            assert(ops.size() >= 1);
            return std::make_pair(std::ref(ops.front()), ops.subspan(1));
        }

        template <class Range>
        auto frontAndRest(const Range& ops)
        {
            return frontAndRest(std::span{ops});
        }
    }
}

sym2::SumSimpl::SumSimpl(Dependencies callbacks, std::pmr::memory_resource* buffer)
    : callbacks{std::move(callbacks)}
    , buffer{buffer}
{}

sym2::Expr sym2::SumSimpl::autoSimplify(std::span<const ExprView<>> ops)
{
    if (ops.size() == 1)
        return Expr{ops.front(), buffer};

    const auto res = intermediateSimplify(ops);

    if (res.empty())
        return Expr{0, buffer};
    else if (res.size() == 1)
        return Expr{res.front(), buffer};
    else
        return {CompositeType::sum, std::move(res), buffer};
}

std::pmr::vector<sym2::Expr> sym2::SumSimpl::intermediateSimplify(std::span<const ExprView<>> ops)
{
    if (ops.size() == 2)
        return simplTwoFactors(ops.front(), ops.back());
    else
        return simplMoreThanTwoFactors(ops);
}

std::pmr::vector<sym2::Expr> sym2::SumSimpl::simplTwoFactors(ExprView<> lhs, ExprView<> rhs)
{
    static const auto asSumOperands = [](ExprView<> e) {
        return is<sum>(e) ? OperandsView::operandsOf(e) : OperandsView::singleOperand(e);
    };

    if (is<sum>(lhs) || is<sum>(rhs))
        return merge(asSumOperands(lhs), asSumOperands(rhs));
    else
        return binarySum(lhs, rhs);
}

std::pmr::vector<sym2::Expr> sym2::SumSimpl::binarySum(ExprView<!sum> lhs, ExprView<!sum> rhs)
{
    std::pmr::vector<Expr> result{buffer};

    if (lhs == 0_ex)
        result.emplace_back(rhs);
    else if (rhs == 0_ex)
        result.emplace_back(lhs);
    else if (areAll<number>(lhs, rhs)) {
        Expr numSum = callbacks.numericAdd(lhs, rhs);
        if (numSum != 0_ex)
            result.push_back(std::move(numSum));
    }
    // TODO
    // 1. contract equal non-numeric terms, 2*a*b + 3*a*b = 5*a*b
    else if (callbacks.orderLessThan(lhs, rhs)) {
        result.emplace_back(lhs);
        result.emplace_back(rhs);
    } else {
        result.emplace_back(rhs);
        result.emplace_back(lhs);
    }

    return result;
}

std::pmr::vector<sym2::Expr> sym2::SumSimpl::simplMoreThanTwoFactors(std::span<const ExprView<>> ops)
{
    assert(ops.size() > 2);

    const auto [u1, rest] = frontAndRest(ops);
    const auto simplifiedRest = intermediateSimplify(rest);

    if (is<sum>(u1))
        return merge(OperandsView::operandsOf(u1), simplifiedRest);
    else
        return merge(OperandsView::singleOperand(u1), simplifiedRest);
}

template <class View>
std::pmr::vector<sym2::Expr> sym2::SumSimpl::merge(OperandsView p, View q)
{
    assert(!(p.empty() && q.empty()));

    if (p.empty())
        return {constructIter(q.begin()), constructIter(q.end())};
    else if (q.empty())
        return {constructIter(p.begin()), constructIter(p.end())};
    else
        return mergeNonEmpty(p, q);
}

template <class View>
std::pmr::vector<sym2::Expr> sym2::SumSimpl::mergeNonEmpty(OperandsView p, View q)
{
    const auto [p1, pRest] = frontAndRest(p);
    const auto [q1, qRest] = frontAndRest(q);
    const std::pmr::vector<Expr> firstTwo = simplTwoFactors(p1, q1);

    if (firstTwo.empty())
        return merge(pRest, qRest);
    else if (firstTwo.size() == 1)
        return prepend(firstTwo.front(), merge(pRest, qRest));

    ExprView<> first = firstTwo.front();
    ExprView<> second = firstTwo.back();

    assert(firstTwo.size() == 2);
    assert((first == p1 && second == q1) || (first == q1 && second == p1));

    if (first == p1 && second == q1)
        return prepend(p1, merge(pRest, q));
    else
        return prepend(q1, merge(p, qRest));
}
