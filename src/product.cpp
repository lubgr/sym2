
#include "product.h"
#include <boost/hof/construct.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include "predicates.h"
#include "query.h"

namespace sym2 {
    namespace {
        ProductExprVec prepend(ExprView<> first, ProductExprVec&& rest)
        {
            ProductExprVec result;

            result.reserve(rest.size() + 1);

            result.emplace_back(first);
            std::move(rest.begin(), rest.end(), std::back_inserter(rest));

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

sym2::ProductExprVec sym2::autoProductIntermediate(std::span<const ExprView<>> ops)
{
    if (ops.size() == 2)
        return simplTwoFactors(ops.front(), ops.back());
    else
        return simplNFactors(ops);
}

sym2::ProductExprVec sym2::simplTwoFactors(ExprView<> lhs, ExprView<> rhs)
{
    static const auto asProductOperands = [](ExprView<> e) {
        return is<product>(e) ? OperandsView::operandsOf(e) : OperandsView::singleOperand(e);
    };

    if (is<product>(lhs) || is<product>(rhs))
        return merge(asProductOperands(lhs), asProductOperands(rhs));

    return {Expr{lhs}, Expr{rhs}};
}

sym2::ProductExprVec sym2::simplNFactors(std::span<const ExprView<>> ops)
{
    const auto [u1, rest] = frontAndRest(ops);
    const auto simplifiedRest = autoProductIntermediate(rest);

    if (is<product>(u1))
        return merge(OperandsView::operandsOf(u1), simplifiedRest);
    else
        return merge(OperandsView::singleOperand(u1), simplifiedRest);
}

template <class View>
sym2::ProductExprVec sym2::merge(OperandsView p, View q)
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
sym2::ProductExprVec sym2::mergeNonEmpty(OperandsView p, View q)
{
    const auto [p1, pRest] = frontAndRest(p);
    const auto [q1, qRest] = frontAndRest(q);
    const ProductExprVec firstTwo = simplTwoFactors(p1, q1);

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
