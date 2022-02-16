
#include "orderrelationimpl.h"
#include <algorithm>
#include <boost/container/static_vector.hpp>
#include <boost/logic/tribool.hpp>
#include <limits>
#include <tuple>
#include "eval.h"
#include "expr.h"
#include "exprliteral.h"
#include "get.h"
#include "operandsview.h"
#include "orderrelation.h"
#include "query.h"

bool sym2::orderLessThan(ExprView<> lhs, ExprView<> rhs)
{
    if (areAll<number>(lhs, rhs))
        return numbers(lhs, rhs);
    else if (areAll<symbol>(lhs, rhs))
        return symbols(lhs, rhs);
    else if (areAll<power>(lhs, rhs))
        return powers(lhs, rhs);
    else if (areAll<product>(lhs, rhs) || areAll<sum>(lhs, rhs))
        return productsOrSums(lhs, rhs);
    else if (areAll<constant>(lhs, rhs))
        // This diverges from Cohen's algorithm, where constants are treated the same way as
        // numbers:
        return constants(lhs, rhs);
    else if (areAll<function>(lhs, rhs))
        return functions(lhs, rhs);

    if (is<number>(lhs))
        return true;
    else if (is<number>(rhs))
        // This branch could be omitted since it's handled by the final recursive swap, but it
        // simplifies the following condition for catching a constant.
        return false;
    else if (is<constant>(lhs))
        // Cohen's algorithm treats constants as numbers, hence this branch doesn't exist in his
        // outline. We need it here since we don't treat constants as numbers.
        return true;
    else if (is<product>(lhs) && is < power || sum || symbol || function > (rhs))
        return orderLessThan(OperandsView::operandsOf(lhs), OperandsView::singleOperand(rhs));
    else if (is<power>(lhs) && is < sum || symbol || function > (rhs))
        return powers(splitAsPower(lhs), {rhs, (1_ex).view()});
    else if (is<sum>(lhs) && is < symbol || function > (rhs))
        return orderLessThan(OperandsView::operandsOf(lhs), OperandsView::singleOperand(rhs));
    else if (is<function>(lhs) && is<symbol>(rhs))
        return leftFunctionRightSymbol(lhs, rhs);

    return !orderLessThan(rhs, lhs);
}

bool sym2::numbers(ExprView<number> lhs, ExprView<number> rhs)
{
    const auto zeroLookup = [](auto&&...) {
        assert(false);
        return 0.0;
    };
    const std::complex<double> complexLhs = evalComplex(lhs, zeroLookup);
    const std::complex<double> complexRhs = evalComplex(rhs, zeroLookup);
    const double lhsReal = complexLhs.real();
    const double lhsImag = complexLhs.imag();
    const double rhsReal = complexRhs.real();
    const double rhsImag = complexRhs.imag();

    return std::tie(lhsReal, lhsImag) < std::tie(rhsReal, rhsImag);
}

bool sym2::symbols(ExprView<symbol> lhs, ExprView<symbol> rhs)
{
    const auto asTuple = [](ExprView<symbol> e) {
        return std::make_tuple(
          /* We invert the booleans here to achieve R+ < + < R. The actual ordering this enforces is
           * arbitrary, but it's important to implement a strict weak ordering. */
          get<std::string_view>(e), !is < positive && realDomain > (e), !is<positive>(e),
          !is<realDomain>(e));
    };

    return asTuple(lhs) < asTuple(rhs);
}

bool sym2::powers(ExprView<power> lhs, ExprView<power> rhs)
{
    return powers(splitAsPower(lhs), splitAsPower(rhs));
}

bool sym2::powers(BaseExp lhs, BaseExp rhs)
{
    const auto [lhsBase, lhsExp] = lhs;
    const auto [rhsBase, rhsExp] = rhs;

    if (lhsBase == rhsBase)
        return orderLessThan(lhsExp, rhsExp);
    else
        return orderLessThan(lhsBase, rhsBase);
}

bool sym2::productsOrSums(ExprView<product || sum> lhs, ExprView<product || sum> rhs)
{
    /* Cohen's algorithm determines the order relation of two sums or products such that the order
     * relation of their operands are compared, starting at the end, i.e., the last operand is the
     * most significant one. When both last operands compare equal, the two next to last are
     * evaluated and so on. This imposes a technical difficulty for handling the data layout of our
     * composites: iterators over operands are only forward iterators, so we can't easily reverse a
     * sequence of operands. See below for details. Examples of order relation of sum/product
     * operands:
     *
     * lhs: b c d              lhs: a b c             lhs: b c g h x
     * rhs: a c d              rhs: b c d             rhs: b c g h i
     * <?:  f t t => false     <?:  t t t => true     <?:  t t t t f => false
     *
     * lhs: a b c              lhs: a b c             lhs:   b d
     * rhs:   b c              rhs:   b d             rhs: a b c
     * <?:  f t t => false     <?:  f t t => false    <?:  t t f => false */
    const auto lhsOps = OperandsView::operandsOf(lhs);
    const auto rhsOps = OperandsView::operandsOf(rhs);

    return orderLessThan(lhsOps, rhsOps);
}

bool sym2::orderLessThan(OperandsView lhs, OperandsView rhs)
{
    const std::size_t maxSize = std::max(lhs.size(), rhs.size());
    const std::size_t lhsStart = maxSize - rhs.size();
    const std::size_t rhsStart = maxSize - lhs.size();
    const boost::logic::tribool lessThan =
      orderLessThanOperandsReverse(lhs.subview(lhsStart), rhs.subview(rhsStart));

    if (boost::logic::indeterminate(lessThan))
        return lhsStart < rhsStart;
    else
        return static_cast<bool>(lessThan);
}

boost::logic::tribool sym2::orderLessThanOperandsReverse(OperandsView lhs, OperandsView rhs)
{
    /* This function deliberately avoids recursion (which allows for a much easier implementation)
     * to not exhaust stack space. In particular, we want sanitizer builds to run as smoothly as
     * non-sanitizer builds, and this imposes a strict recursion depth limit (can be somewhere
     * around 250, depending on the platform). Instead, it uses an artificial, compile-time sized
     * stack that is filled with the last parts of both function arguments while traversing them. We
     * assume that most of the time, the first pass will either yield a result because two operands
     * don't compare equal and their order relation can be evaluated, or the total number of
     * operands comparing equal is within reasonable bounds. The latter assumption is important to
     * not create a runtime performance bottleneck with the following iterations, which repeatedly
     * traverse the operands (recall: only forward-iterators!) to compare and possibly evaluate an
     * order relation. */
    assert(lhs.size() == rhs.size() && !lhs.empty());
    assert(lhs.size() <= std::numeric_limits<std::int64_t>::max());

    constexpr std::int64_t stackSize = 10;
    const auto n = static_cast<std::int64_t>(lhs.size());

    const auto collectOperands = [n](OperandsView ops, std::int64_t offset) {
        const auto first = std::next(ops.begin(), offset);
        const auto extent = offset == 0 ? n % stackSize : stackSize;
        const auto last = std::next(first, extent);
        return boost::container::static_vector<ExprView<>, stackSize>{first, last};
    };

    for (std::int64_t offset = n > stackSize ? n - stackSize : 0; offset >= 0;
         offset -= stackSize) {
        const auto lastOpsLhs = collectOperands(lhs, offset);
        const auto lastOpsRhs = collectOperands(rhs, offset);

        // This can be implemented more elegantly with boost::combine and a reverse adaptor.
        // However, this ends up pulling boost fusion in. We prefer the manual solution to avoid
        // dependency over-use and keep compile times reasonable.
        for (std::size_t i = lastOpsLhs.size(); i > 0; --i) {
            const ExprView<> lhsOp = lastOpsLhs[i - 1];
            const ExprView<> rhsOp = lastOpsRhs[i - 1];
            if (lhsOp != rhsOp)
                return orderLessThan(lhsOp, rhsOp);
        }
    }

    return boost::logic::indeterminate;
}

bool sym2::constants(ExprView<constant> lhs, ExprView<constant> rhs)
{
    const auto asPair = [](ExprView<constant> e) {
        // It's quite defensive to compare the floating point value here, too (if someone defines
        // two constants with the same name but different values, we shouldn't expect much to
        // function properly), but it doesn't hurt.
        return std::make_pair(get<std::string_view>(e), get<double>(e));
    };

    return asPair(lhs) < asPair(rhs);
}

bool sym2::functions(ExprView<function> lhs, ExprView<function> rhs)
{
    const auto lhsName = get<std::string_view>(lhs);
    const auto rhsName = get<std::string_view>(rhs);

    if (lhsName != rhsName)
        return lhsName < rhsName;

    const auto lhsOps = OperandsView::operandsOf(lhs);
    const auto rhsOps = OperandsView::operandsOf(rhs);
    const boost::logic::tribool lessThan = orderLessThanOperands(lhsOps, rhsOps);

    if (boost::logic::indeterminate(lessThan))
        return lhsOps.size() < rhsOps.size();
    else
        return static_cast<bool>(lessThan);
}

boost::logic::tribool sym2::orderLessThanOperands(OperandsView lhs, OperandsView rhs)
{
    const std::size_t minSize = std::min(lhs.size(), rhs.size());
    const OperandsView lhsRelevantOps = lhs.subview(0, minSize);
    const OperandsView rhsRelevantOps = rhs.subview(0, minSize);

    // This can be implemented more elegantly with boost::combine. However, this ends up pulling
    // boost fusion in. We prefer the manual solution to avoid dependency over-use and keep compile
    // times reasonable.
    for (auto lhsOp = lhsRelevantOps.begin(), rhsOp = rhsRelevantOps.begin();
         lhsOp != lhsRelevantOps.end(); ++lhsOp, ++rhsOp)
        if (*lhsOp != *rhsOp)
            return orderLessThan(*lhsOp, *rhsOp);

    return boost::logic::indeterminate;
}

bool sym2::leftFunctionRightSymbol(ExprView<function> lhs, ExprView<symbol> rhs)
{
    const auto lhsName = get<std::string_view>(lhs);
    const auto rhsName = get<std::string_view>(rhs);

    return lhsName < rhsName;
}
