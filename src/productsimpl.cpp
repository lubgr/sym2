
#include "productsimpl.h"
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

sym2::ProductSimpl::ProductSimpl(Dependencies callbacks, std::pmr::memory_resource* buffer)
    : callbacks{std::move(callbacks)}
    , buffer{buffer}
{}

sym2::Expr sym2::ProductSimpl::autoSimplify(std::span<const ExprView<>> ops)
{
    if (ops.size() == 1)
        return Expr{ops.front()};
    else if (std::any_of(ops.begin(), ops.end(), isZero))
        return 0_ex;

    const auto res = intermediateSimplify(ops);

    if (res.empty())
        return 1_ex;
    else if (res.size() == 1)
        return res.front();
    else
        return {CompositeType::product, res};
}

std::pmr::vector<sym2::Expr> sym2::ProductSimpl::intermediateSimplify(std::span<const ExprView<>> ops)
{
    if (ops.size() == 2)
        return simplTwoFactors(ops.front(), ops.back());
    else
        return simplMoreThanTwoFactors(ops);
}

std::pmr::vector<sym2::Expr> sym2::ProductSimpl::simplTwoFactors(ExprView<> lhs, ExprView<> rhs)
{
    static const auto asProductOperands = [](ExprView<> e) {
        return is<product>(e) ? OperandsView::operandsOf(e) : OperandsView::singleOperand(e);
    };

    if (is<product>(lhs) || is<product>(rhs))
        return merge(asProductOperands(lhs), asProductOperands(rhs));
    else
        return binaryProduct(lhs, rhs);
}

std::pmr::vector<sym2::Expr> sym2::ProductSimpl::binaryProduct(ExprView<!product> lhs, ExprView<!product> rhs)
{
    std::pmr::vector<Expr> result;

    if (areAll<number>(lhs, rhs)) {
        Expr numProduct = callbacks.numericMultiply(lhs, rhs);
        if (numProduct != 1_ex)
            result.push_back(std::move(numProduct));
    } else if (lhs == 1_ex)
        result.emplace_back(rhs);
    else if (rhs == 1_ex)
        result.emplace_back(lhs);
    else if (asPower(lhs).base == asPower(rhs).base) {
        const auto [base, exp1] = asPower(lhs);
        const ExprView<> exp2 = asPower(rhs).exponent;
        const Expr expSum = callbacks.autoSum(exp1, exp2);

        if (Expr power = callbacks.autoPower(base, expSum); power != 1_ex)
            result.push_back(std::move(power));
    } else if (callbacks.orderLessThan(lhs, rhs)) {
        result.emplace_back(lhs);
        result.emplace_back(rhs);
    } else {
        result.emplace_back(rhs);
        result.emplace_back(lhs);
    }

    return result;
}

std::pmr::vector<sym2::Expr> sym2::ProductSimpl::simplMoreThanTwoFactors(std::span<const ExprView<>> ops)
{
    assert(ops.size() > 2);

    const auto [u1, rest] = frontAndRest(ops);
    const auto simplifiedRest = intermediateSimplify(rest);

    if (is<product>(u1))
        return merge(OperandsView::operandsOf(u1), simplifiedRest);
    else
        return merge(OperandsView::singleOperand(u1), simplifiedRest);
}

template <class View>
std::pmr::vector<sym2::Expr> sym2::ProductSimpl::merge(OperandsView p, View q)
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
std::pmr::vector<sym2::Expr> sym2::ProductSimpl::mergeNonEmpty(OperandsView p, View q)
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
