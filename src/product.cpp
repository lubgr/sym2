
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
        return is<product>(e) ? OperandsView{e} : OperandsView::single(e);
    };

    if (is<product>(lhs) || is<product>(rhs))
        return merge(asProductOperands(lhs), asProductOperands(rhs));

    return {Expr{lhs}, Expr{rhs}};
}

sym2::ProductExprVec sym2::simplNFactors(std::span<const ExprView<>> ops)
{
    const ExprView<>& u1 = ops.front();
    const std::span<const ExprView<>> rest = ops.subspan(1);

    const auto simplifiedRest = autoProductIntermediate(rest);

    if (is<product>(u1))
        return merge(OperandsView{u1}, OperandsView::sequence(simplifiedRest));
    else
        return merge(OperandsView::single(u1), OperandsView::sequence(simplifiedRest));
}

sym2::ProductExprVec sym2::merge(OperandsView p, OperandsView q)
{
    const auto constructIter = [](OperandIterator op) {
        return boost::make_transform_iterator(op, boost::hof::construct<Expr>());
    };

    assert(!(p.empty() && q.size()));

    if (p.empty())
        return {constructIter(p.begin()), constructIter(p.end())};
    else if (q.empty())
        return {constructIter(p.begin()), constructIter(p.end())};
    else
        return merge(p.front(), q.front(), p.subview(1), q.subview(1));
}

sym2::ProductExprVec sym2::merge(ExprView<> p1, ExprView<> q1, OperandsView p, OperandsView q)
{
    const ProductExprVec firstTwo = simplTwoFactors(p1, q1);

    if (firstTwo.empty())
        return merge(p, q);
    else if (firstTwo.size() == 1)
        return prepend(firstTwo.front(), merge(p, q));
    // TODO

    return {};
}
