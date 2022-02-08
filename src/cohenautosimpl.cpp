
#include "cohenautosimpl.h"
#include <array>
#include <boost/hof/construct.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <cassert>
#include <cmath>
#include <complex>
#include <optional>
#include <stdexcept>
#include "exprliteral.h"
#include "get.h"
#include "query.h"

namespace sym2 {
    template <class Iter>
    auto constructIter(Iter op)
    {
        return boost::make_transform_iterator(op, boost::hof::construct<Expr>());
    }

    std::optional<LargeInt> exactPower(const LargeInt& n, const std::uint32_t expDenom)
    {
        const double exact = std::pow(static_cast<double>(n), 1.0 / static_cast<double>(expDenom));
        const auto truncatedRoot = static_cast<LargeInt>(exact + 0.1);

        if (pow(truncatedRoot, expDenom) == n)
            return truncatedRoot;
        else
            return std::nullopt;
    }
}

sym2::CohenAutoSimpl::CohenAutoSimpl(Dependencies callbacks, std::pmr::memory_resource* buffer)
    : callbacks{std::move(callbacks)}
    , buffer{buffer}
{}

sym2::Expr sym2::CohenAutoSimpl::simplifySum(std::span<const ExprView<>> ops)
{
    if (ops.size() == 1)
        return Expr{ops.front(), buffer};

    const auto res = simplSumIntermediate(ops);

    if (res.empty())
        return Expr{0, buffer};
    else if (res.size() == 1)
        return Expr{res.front(), buffer};
    else
        return {CompositeType::sum, std::move(res), buffer};
}

sym2::Expr sym2::CohenAutoSimpl::simplifySum(ExprView<> lhs, ExprView<> rhs)
{
    const std::array<ExprView<>, 2> summands{{lhs, rhs}};

    return simplifySum(summands);
}

std::pmr::vector<sym2::Expr> sym2::CohenAutoSimpl::simplSumIntermediate(
  std::span<const ExprView<>> ops)
{
    if (ops.size() == 2)
        return simplTwoSummands(ops.front(), ops.back());
    else
        return simplMoreThanTwoSummands(ops);
}

std::pmr::vector<sym2::Expr> sym2::CohenAutoSimpl::simplTwoSummands(ExprView<> lhs, ExprView<> rhs)
{
    static const auto asSumOperands = [](ExprView<> e) {
        return is<sum>(e) ? OperandsView::operandsOf(e) : OperandsView::singleOperand(e);
    };

    if (is<sum>(lhs) || is<sum>(rhs))
        return merge(asSumOperands(lhs), asSumOperands(rhs), &CohenAutoSimpl::simplTwoSummands);
    else
        return binarySum(lhs, rhs);
}

std::pmr::vector<sym2::Expr> sym2::CohenAutoSimpl::binarySum(ExprView<!sum> lhs, ExprView<!sum> rhs)
{
    const auto haveEqualNonConstTerm = [lhs, rhs]() {
        if (isOneOf<number>(lhs, rhs))
            return false;
        else
            return boost::equal(splitConstTerm(lhs).term, splitConstTerm(rhs).term);
    };
    std::pmr::vector<Expr> result{buffer};

    if (lhs == 0_ex)
        result.emplace_back(rhs);
    else if (rhs == 0_ex)
        result.emplace_back(lhs);
    else if (areAll<number>(lhs, rhs)) {
        Expr numSum = callbacks.numericAdd(lhs, rhs);
        if (numSum != 0_ex)
            result.push_back(std::move(numSum));
    } else if (haveEqualNonConstTerm()) {
        // Contract equal non-numeric terms, e.g. 2*a*b + 3*a*b = 5*a*b
        const ConstAndTerm lhsSplit = splitConstTerm(lhs);
        const ConstAndTerm rhsSplit = splitConstTerm(rhs);
        const std::array<ExprView<>, 2> constants{{lhsSplit.constant, rhsSplit.constant}};
        const Expr factor = simplifySum(constants);
        const Expr product = simplifyProduct(factor, lhsSplit.term);

        assert(is<number>(factor));

        // Check for zero summands, e.g. a + b - b = a + 0.
        if (factor != 0_ex)
            result.push_back(product);
    } else if (callbacks.orderLessThan(lhs, rhs)) {
        result.emplace_back(lhs);
        result.emplace_back(rhs);
    } else {
        result.emplace_back(rhs);
        result.emplace_back(lhs);
    }

    return result;
}

std::pmr::vector<sym2::Expr> sym2::CohenAutoSimpl::simplMoreThanTwoSummands(
  std::span<const ExprView<>> ops)
{
    assert(ops.size() > 2);

    const auto [u1, rest] = frontAndRest(ops);
    const std::pmr::vector<Expr> simplifiedRest = simplSumIntermediate(rest);

    if (is<sum>(u1))
        return merge(
          OperandsView::operandsOf(u1), simplifiedRest, &CohenAutoSimpl::simplTwoSummands);
    else
        return merge(
          OperandsView::singleOperand(u1), simplifiedRest, &CohenAutoSimpl::simplTwoSummands);
}

template <class View, class BinarySimplMember>
std::pmr::vector<sym2::Expr> sym2::CohenAutoSimpl::merge(
  OperandsView p, View q, BinarySimplMember reduce)
{
    if (p.empty())
        return {constructIter(q.begin()), constructIter(q.end()), buffer};
    else if (q.empty())
        return {constructIter(p.begin()), constructIter(p.end()), buffer};
    else
        return mergeNonEmpty(p, q, reduce);
}

template <class View, class BinarySimplMember>
std::pmr::vector<sym2::Expr> sym2::CohenAutoSimpl::mergeNonEmpty(
  OperandsView p, View q, BinarySimplMember reduce)
{
    const auto [p1, pRest] = frontAndRest(p);
    const auto [q1, qRest] = frontAndRest(q);
    const std::pmr::vector<Expr> firstTwo = std::invoke(reduce, this, p1, q1);

    if (firstTwo.empty())
        return merge(pRest, qRest, reduce);
    else if (firstTwo.size() == 1)
        return prepend(firstTwo.front(), merge(pRest, qRest, reduce));

    ExprView<> first = firstTwo.front();
    ExprView<> second = firstTwo.back();

    assert(firstTwo.size() == 2);
    assert((first == p1 && second == q1) || (first == q1 && second == p1));

    if (first == p1 && second == q1)
        return prepend(p1, merge(pRest, q, reduce));
    else
        return prepend(q1, merge(p, qRest, reduce));
}

std::pmr::vector<sym2::Expr> sym2::CohenAutoSimpl::prepend(
  ExprView<> first, std::pmr::vector<Expr>&& rest)
{
    std::pmr::vector<Expr> result{buffer};

    result.reserve(rest.size() + 1);

    result.emplace_back(first);
    std::move(rest.begin(), rest.end(), std::back_inserter(result));

    return result;
}

sym2::Expr sym2::CohenAutoSimpl::simplifyProduct(std::span<const ExprView<>> ops)
{
    if (ops.size() == 1)
        return Expr{ops.front(), buffer};
    else if (std::any_of(ops.begin(), ops.end(), isZero))
        return Expr{0, buffer};

    const std::pmr::vector<Expr> res = simplProductIntermediate(ops);

    if (res.empty())
        return Expr{1, buffer};
    else if (res.size() == 1)
        return Expr{res.front(), buffer};
    else
        return {CompositeType::product, std::move(res), buffer};
}

sym2::Expr sym2::CohenAutoSimpl::simplifyProduct(ExprView<> lhs, ExprView<> rhs)
{
    const std::array<ExprView<>, 2> factors{{lhs, rhs}};

    return simplifyProduct(factors);
}

sym2::Expr sym2::CohenAutoSimpl::simplifyProduct(ExprView<> first, OperandsView rest)
{
    std::pmr::vector<ExprView<>> allOperands{buffer};
    allOperands.reserve(rest.size() + 1);

    allOperands.emplace_back(first);
    allOperands.insert(allOperands.end(), rest.begin(), rest.end());

    return simplifyProduct(allOperands);
}

std::pmr::vector<sym2::Expr> sym2::CohenAutoSimpl::simplProductIntermediate(
  std::span<const ExprView<>> ops)
{
    if (ops.size() == 2)
        return simplTwoFactors(ops.front(), ops.back());
    else
        return simplMoreThanTwoFactors(ops);
}

std::pmr::vector<sym2::Expr> sym2::CohenAutoSimpl::simplTwoFactors(ExprView<> lhs, ExprView<> rhs)
{
    static const auto asProductOperands = [](ExprView<> e) {
        return is<product>(e) ? OperandsView::operandsOf(e) : OperandsView::singleOperand(e);
    };

    if (is<product>(lhs) || is<product>(rhs))
        return merge(
          asProductOperands(lhs), asProductOperands(rhs), &CohenAutoSimpl::simplTwoFactors);
    else
        return binaryProduct(lhs, rhs);
}

std::pmr::vector<sym2::Expr> sym2::CohenAutoSimpl::binaryProduct(
  ExprView<!product> lhs, ExprView<!product> rhs)
{
    std::pmr::vector<Expr> result{buffer};

    if (areAll<number>(lhs, rhs)) {
        Expr numProduct = callbacks.numericMultiply(lhs, rhs);
        if (numProduct != 1_ex)
            result.push_back(std::move(numProduct));
    } else if (lhs == 1_ex)
        result.emplace_back(rhs);
    else if (rhs == 1_ex)
        result.emplace_back(lhs);
    else if (splitAsPower(lhs).base == splitAsPower(rhs).base) {
        const auto [base, exp1] = splitAsPower(lhs);
        const ExprView<> exp2 = splitAsPower(rhs).exponent;
        const Expr expSum = simplifySum(exp1, exp2);

        if (Expr power = simplifyPower(base, expSum); power != 1_ex)
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

std::pmr::vector<sym2::Expr> sym2::CohenAutoSimpl::simplMoreThanTwoFactors(
  std::span<const ExprView<>> ops)
{
    assert(ops.size() > 2);

    const auto [u1, rest] = frontAndRest(ops);
    const std::pmr::vector<Expr> simplifiedRest = simplProductIntermediate(rest);

    if (is<product>(u1))
        return merge(
          OperandsView::operandsOf(u1), simplifiedRest, &CohenAutoSimpl::simplTwoFactors);
    else
        return merge(
          OperandsView::singleOperand(u1), simplifiedRest, &CohenAutoSimpl::simplTwoFactors);
}

sym2::Expr sym2::CohenAutoSimpl::simplifyPower(ExprView<> base, ExprView<> exp)
{
    // This might not be fully compliant with Cohen's algorithm outline, but needs to take complex
    // numbers into account and hence some more logic. Trivial cases first...
    if (base == 1_ex)
        return Expr{base, buffer};
    else if (base == 0_ex && exp == 0_ex)
        throw std::invalid_argument{"Invalid power with zero base and zero exponent"};
    else if (exp == 0_ex)
        return 1_ex;
    else if (exp == 1_ex)
        return Expr{base, buffer};
    // ... now error our on division by zero ...
    else if (base == 0_ex && is < number && realDomain && negative > (exp))
        throw std::invalid_argument{
          "Invalid power with zero base and real-valued negative exponent"};
    // ... but if zero is not in the denomiator, return it as is...
    else if (base == 0_ex)
        return Expr{base, buffer};
    // ... from here on, don't simplify anything with a complex number...
    else if (isOneOf < number && complexDomain > (base, exp)) {
        return Expr{CompositeType::power, {base, exp}, buffer};
    }
    // ... (-1.2345)^6.789 results in a complex number with floating point real/imag parts...
    else if (is < number && realDomain && negative > (base) && is<floatingPoint>(exp)) {
        const std::complex<double> numeric =
          std::pow(std::complex<double>{get<double>(base)}, std::complex<double>{get<double>(exp)});
        const std::array<Expr, 2> realAndImag{
          {Expr{std::real(numeric), buffer}, Expr{std::imag(numeric), buffer}}};
        return Expr{CompositeType::complexNumber, realAndImag, buffer};
    }
    // ... but otherwise, numeric power expressions with a real floating point part are just
    // evaluated...
    else if (isOneOf<floatingPoint>(base, exp) && areAll < realDomain && number > (base, exp)) {
        const double numericResult = std::pow(get<double>(base), get<double>(exp));
        return Expr{numericResult, buffer};
    }
    // ... evaluate expressions like (2/3)^7 (can result in big integers)...
    else if (is<rational>(base) && is < integer && small > (exp))
        return computePowerRationalToInt(base, get<std::int32_t>(exp));
    // ... (a^b)^2 becomes a^(2*b)...
    else if (is<power>(base) && is<integer>(exp)) {
        const auto [baseOfBase, expOfBase] = splitAsPower(base);
        return simplifyPower(baseOfBase, simplifyProduct(exp, expOfBase));
    }
    // ... catch rational powers that might have a simpler representation, e.g. sqrt(1/4) = 1/2...
    else if (is<rational>(base) && is < rational && !integer > (exp))
        return simplPowerRationalToRational(base, exp);
    // ... transform (a^b)^c into a^(b*c) when a is positive and real...
    else if (is<power>(base)) {
        const auto [baseOfBase, expOfBase] = splitAsPower(base);

        if (is < realDomain && positive > (baseOfBase))
            return simplifyPower(baseOfBase, simplifyProduct(expOfBase, exp));
        else
            return Expr{CompositeType::power, {base, exp}, buffer};
    }
    // ... and finally don't simplify anything.
    else
        return Expr{CompositeType::power, {base, exp}, buffer};
}

sym2::Expr sym2::CohenAutoSimpl::computePowerRationalToInt(
  ExprView<rational> base, std::int32_t exp)
{
    assert(exp != 0);

    const auto positiveExp = static_cast<std::uint32_t>(std::abs(exp));
    const Expr forPositiveExp = computePowerRationalToUnsigned(base, positiveExp);

    assert(is<rational>(forPositiveExp));

    if (exp > 0)
        return forPositiveExp;
    else if (is<small>(forPositiveExp)) {
        const auto r = get<SmallRational>(forPositiveExp);
        return Expr{r.denom, r.num, buffer};
    } else {
        assert(is < large && rational > (forPositiveExp));
        const auto r = get<LargeRational>(forPositiveExp);
        const LargeRational inverted{denominator(r), numerator(r)};
        return Expr{LargeRationalRef{inverted}, buffer};
    }
}

sym2::Expr sym2::CohenAutoSimpl::computePowerRationalToUnsigned(
  ExprView<rational> base, std::uint32_t exp)
{
    // Copied and adjusted from https://stackoverflow.com/questions/101439.
    Expr increasingBase{base, buffer};
    Expr result{1, buffer};

    while (true) {
        if (exp & 1)
            result = simplifyProduct(result, increasingBase);

        exp >>= 1;

        if (!exp)
            break;

        increasingBase = simplifyProduct(increasingBase, increasingBase);
    }

    return result;
}

sym2::Expr sym2::CohenAutoSimpl::simplPowerRationalToRational(
  ExprView<rational> base, ExprView<rational && !integer> exp)
{
    const auto rationalBase = get<LargeRational>(base);
    const auto rationalExp = get<LargeRational>(exp);
    const LargeInt expNum = numerator(rationalExp);
    const LargeInt expDenom = denominator(rationalExp);
    const LargeInt baseNum = numerator(rationalBase);
    const LargeInt baseDenom = denominator(rationalBase);

    assert(expDenom > 0 && baseDenom > 0);

    // First, don't get tangled up the attempt to compute powers with huge values
    if (!fitsInto<std::int32_t>(expNum) || !fitsInto<std::uint32_t>(expDenom))
        return Expr{CompositeType::power, {base, exp}, buffer};

    // Then handle sqrt(r), where r is negative. This simplifies sqrt(-1) to i or sqrt(-1/4) to
    // 1/2*i, but returns cases as is where simplifying sqrt(|base|) is not a rational number...
    if (baseNum < 0 && expNum == 1 && expDenom == 2) {
        const Expr positiveBase{LargeRationalRef{-rationalBase}};
        const Expr result = simplPowerRationalToRational(positiveBase, exp);

        // Only construct a complex number when sqrt(|base|) results in a rational number. That
        // doesn't have to be the case, e.g. for sqrt(-1/3) we will have evaluated sqrt(1/3) which
        // is not simplified. In this case, we return sqrt(-1/3) as it is.
        if (is<rational>(result)) {
            const std::array<Expr, 2> realAndImag{{0_ex, result}};
            return Expr{CompositeType::complexNumber, realAndImag, buffer};
        }

        return Expr{CompositeType::power, {base, exp}, buffer};
    }

    // Any other negative base won't be simplified.
    if (rationalBase < 0)
        return Expr{CompositeType::power, {base, exp}, buffer};

    const auto primitiveExpDenom = static_cast<std::uint32_t>(expDenom);
    const std::optional<LargeInt> numDenomPart = exactPower(baseNum, primitiveExpDenom);
    const std::optional<LargeInt> denomDenomPart = exactPower(baseDenom, primitiveExpDenom);

    if (numDenomPart && denomDenomPart) {
        // Now compute the numerator part
        const auto primitiveExpNum = static_cast<std::int32_t>(expNum);
        const LargeInt newNum = pow(*numDenomPart, primitiveExpNum);
        const LargeInt newDenom = pow(*denomDenomPart, primitiveExpNum);

        if (primitiveExpNum < 0) {
            // Constructor takes care of negative a numerator
            return Expr{LargeRationalRef{LargeRational{newDenom, newNum}}, buffer};
        } else
            return Expr{LargeRationalRef{LargeRational{newNum, newDenom}}, buffer};
    } else
        return Expr{CompositeType::power, {base, exp}, buffer};
}
