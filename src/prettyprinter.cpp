
#include "prettyprinter.h"
#include <sstream>
#include <stdexcept>
#include <vector>
#include "sym2/autosimpl.h"
#include "sym2/expr.h"
#include "sym2/get.h"
#include "sym2/query.h"

#include <iostream>

namespace sym2 {
    namespace {
        unsigned short precedence(ExprView<> e)
        {
            if (is < sum || (complexDomain && number) > (e))
                return 1;
            else if (is<product>(e))
                return 2;
            else if (is<power>(e))
                return 3;
            else
                return 4;
        }
    }

    bool isScalarPowerPart(ExprView<> baseOrExp)
    {
        return is < symbol || constant || function || (integer && positive) > (baseOrExp);
    }

}

sym2::PrettyPrinter::PrettyPrinter(
  PrintEngine& engine, PowerAsFraction opt, std::pmr::memory_resource* resource)
    : engine{engine}
    , powerAsFraction{opt}
    , resource{resource}
{}

void sym2::PrettyPrinter::print(ExprView<> e)
{
    if (is<symbol>(e))
        printSymbolOrConstant(e);
    else if (is<number>(e))
        printNumber(e);
    else if (is<power>(e)) {
        const auto [base, exp] = splitAsPower(e);
        printPower(base, exp);
    } else if (is<sum>(e))
        printSum(e);
    else if (is<product>(e))
        printProduct(e);
    else if (is<function>(e))
        printFunction(e);
    else if (is<constant>(e)) {
        printSymbolOrConstant(e);
    } else
        throw std::invalid_argument{"Can't print unknown expression type"};
}

void sym2::PrettyPrinter::printSymbolOrConstant(ExprView<symbol || constant> e)
{
    const auto name = get<std::string_view>(e);
    std::optional<DomainFlag> flag = std::nullopt;

    if (is < positive && realDomain > (e))
        flag = DomainFlag::positive;
    else if (is<realDomain>(e))
        flag = DomainFlag::real;

    engine.symbol(name, flag);
}

void sym2::PrettyPrinter::printNumber(ExprView<number> e)
{
    static const auto toString = [](const LargeInt& n) {
        std::ostringstream os;
        os << n;
        return os.str();
    };

    if (is<floatingPoint>(e))
        engine.floatingPoint(get<double>(e));
    else if (is < integer && small > (e))
        engine.integer(get<std::int16_t>(e));
    else if (is < integer && large > (e))
        engine.largeInteger(toString(get<LargeInt>(e)));
    else if (is < rational && small > (e)) {
        const auto sr = get<SmallRational>(e);
        const bool denominatorIsScalar = true;
        engine.openNumerator()
          .integer(sr.num)
          .closeNumerator()
          .openDenominator(denominatorIsScalar)
          .integer(sr.denom)
          .closeDenominator(denominatorIsScalar);
    } else if (is < rational && large > (e)) {
        const auto lr = get<LargeRational>(e);
        const bool denominatorIsScalar = true;
        engine.openNumerator();
        engine.largeInteger(toString(numerator(lr)));
        engine.closeNumerator();
        engine.openDenominator(denominatorIsScalar);
        engine.largeInteger(toString(denominator(lr)));
        engine.closeDenominator(denominatorIsScalar);
    } else if (is<complexDomain>(e)) {
        printNumber(real(e));

        Expr imaginaryPart{imag(e), resource};

        if (is<negative>(imag(e))) {
            engine.minusSign();
            imaginaryPart = autoMinus(imaginaryPart);
        } else
            engine.plusSign();

        if (imaginaryPart == 1_ex)
            engine.singleImaginaryI();
        else {
            printNumber(imaginaryPart);
            engine.timesImaginayI();
        }
    }
}

void sym2::PrettyPrinter::printPower(ExprView<> base, ExprView<!power> exp)
{
    if (exp == FixedExpr<1>{1, 2}) {
        engine.openSquareRoot();
        print(base);
        engine.closeSquareRoot();
    } else if (is < number && negative > (exp) && powerAsFraction == PowerAsFraction::asFraction)
        printPowerWithNegativeNumericExp(base, exp);
    else
        printStandardPower(base, exp);
}

void sym2::PrettyPrinter::printPowerWithNegativeNumericExp(
  ExprView<> base, ExprView<number && negative> exp)
{
    const bool denomNeedsParentheses = isScalarPowerPart(base);

    engine.openNumerator().integer(1).closeNumerator().openDenominator(denomNeedsParentheses);

    print(autoPower(base, autoMinus(exp)));

    engine.closeDenominator(denomNeedsParentheses);
}

void sym2::PrettyPrinter::printStandardPower(ExprView<> base, ExprView<!power> exp)
{
    printPowerBase(base);
    printPowerExponent(exp);
}

void sym2::PrettyPrinter::printPowerBase(ExprView<> base)
{
    if (isScalarPowerPart(base))
        print(base);
    else {
        engine.openParentheses();
        print(base);
        engine.closeParentheses();
    }
}

void sym2::PrettyPrinter::printPowerExponent(ExprView<!power> exp)
{
    if (exp == 1_ex)
        return;
    else if (isScalarPowerPart(exp)) {
        engine.openScalarExponent();
        print(exp);
        engine.closeScalarExponent();
    } else {
        engine.openCompositeExponent();
        print(exp);
        engine.closeCompositeExponent();
    }
}

void sym2::PrettyPrinter::printSum(ExprView<sum> e)
{
    const auto is_product_with_negative_numeric_first = [](ExprView<> e) {
        return is<product>(e) && is < number && negative > (firstOperand(e));
    };

    print(firstOperand(e));

    const OperandsView rest = OperandsView::operandsOf(e).subview(1);

    for (ExprView<!sum> summand : rest) {
        if (is_product_with_negative_numeric_first(summand)) {
            engine.minusSign();
            const Expr positive = autoMinus(summand);
            print(positive);
        } else {
            engine.plusSign();
            print(summand);
        }
    }
}

bool sym2::PrettyPrinter::isProductWithNegativeNumeric(ExprView<!sum> summand)
{
    if (is<product>(summand))
        return is < number && negative > (firstOperand(summand));
    else
        return false;
}

void sym2::PrettyPrinter::printProduct(ExprView<product> e)
{
    const OperandsView factors = OperandsView::operandsOf(e);

    if (powerAsFraction == PowerAsFraction::asFraction)
        printProductAsFraction(factors);
    else
        printProductWithoutFractions(factors);
}

void sym2::PrettyPrinter::printProductAsFraction(OperandsView factors)
{
    const unsigned short productPrecedence = 2;
    const auto [num, denom] = collectProductFractions(factors);

    if (num.empty())
        engine.integer(1);
    else if (num.size() == 1 && precedence(num.front()) < productPrecedence) {
        engine.openParentheses();
        print(num.front());
        engine.closeParentheses();
    } else
        printProductWithoutFractions(num);

    if (denom.empty())
        return;

    engine.divisionSign();

    if (denom.size() == 1 && precedence(denom.front()) > productPrecedence)
        print(denom.front());
    else {
        engine.openParentheses();
        const std::pmr::vector<ExprView<>> denomFactors{denom.cbegin(), denom.cend(), resource};
        print(autoProduct(denomFactors));
        engine.closeParentheses();
    }
}

std::pair<std::pmr::deque<sym2::Expr>, std::pmr::deque<sym2::Expr>>
  sym2::PrettyPrinter::collectProductFractions(OperandsView originalFactors)
{
    std::pair<std::pmr::deque<Expr>, std::pmr::deque<Expr>> frac{resource, resource};
    auto& [num, denom] = frac;

    for (const ExprView<> originalFactor : originalFactors) {
        const auto [base, exp] = splitAsPower(originalFactor);

        if (is < number && negative > (exp))
            denom.push_back(autoPower(base, autoMinus(exp)));
        else
            num.emplace_back(originalFactor);
    }

    // Adjust the previous logic and move factors like 2/3 to numerator/denominator.
    if (num.empty() || is < integer || !rational > (num.front()))
        return frac;

    if (is < small && rational > (num.front())) {
        const auto sr = get<SmallRational>(num.front());
        num.front() = Expr{sr.num, resource};
        denom.emplace_front(sr.denom);
    } else if (is < large && rational > (num.front())) {
        const auto lr = get<LargeRational>(num.front());
        num.front() = Expr{numerator(lr), resource};
        denom.emplace_front(denominator(lr));
    }

    return frac;
}

template <class View>
void sym2::PrettyPrinter::printProductWithoutFractions(View&& factors)
{
    const unsigned short productPrecedence = 2;
    auto first = factors.begin();
    auto current = std::next(first);
    auto last = factors.end();

    if (current == last) {
        print(*first);
    } else if (*first == FixedExpr<1>{-1})
        engine.unaryMinusSign();
    else if (*first == 1_ex)
        ;
    else if (precedence(*first) < productPrecedence) {
        engine.openParentheses();
        print(*first);
        engine.closeParentheses().timesSign();
    } else {
        print(*first);
        engine.timesSign();
    }

    for (auto factor = current; factor != last; ++factor) {
        if (precedence(*factor) < productPrecedence) {
            engine.openParentheses();
            print(*factor);
            engine.closeParentheses();
        } else
            print(*factor);

        if (std::next(factor) != last)
            engine.timesSign();
    }
}

void sym2::PrettyPrinter::printFunction(ExprView<function> e)
{
    const OperandsView operands = OperandsView::operandsOf(e);

    engine.functionName(get<std::string_view>(e)).openParentheses();
    print(*operands.begin());

    if (operands.size() == 2) {
        engine.comma();
        print(*std::next(operands.begin()));
    }

    engine.closeParentheses();
}
