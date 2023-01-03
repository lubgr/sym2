
#include <ostream>
#include "sym2/printengine.h"

sym2::PlaintextPrintEngine::PlaintextPrintEngine(std::ostream& out)
    : out{out}
{}

sym2::PrintEngine& sym2::PlaintextPrintEngine::symbol(
  std::string_view name, std::optional<DomainFlag>)
{
    out << name;

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::functionName(std::string_view name)
{
    symbol(name, std::nullopt);

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::floatingPoint(double n)
{
    out << n;

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::integer(std::int16_t n)
{
    out << n;

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::largeInteger(std::string_view n)
{
    out << n;

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::plusSign()
{
    out << " + ";

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::minusSign()
{
    out << " - ";

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::unaryMinusSign()
{
    out << '-';

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::timesSign()
{
    out << '*';

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::divisionSign()
{
    out << '/';

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::comma()
{
    out << ", ";

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::openNumerator(bool numeratorIsSum)
{
    if (numeratorIsSum)
        openParentheses();

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::closeNumerator(bool numeratorWasSum)
{
    if (numeratorWasSum)
        closeParentheses();

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::openDenominator(bool denominatorIsScalar)
{
    divisionSign();

    if (!denominatorIsScalar)
        openParentheses();

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::closeDenominator(bool denominatorWasScalar)
{
    if (!denominatorWasScalar)
        closeParentheses();

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::timesImaginayI()
{
    return singleImaginaryI();
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::singleImaginaryI()
{
    out << "i";

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::openScalarExponent()
{
    out << '^';

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::closeScalarExponent()
{
    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::openCompositeExponent()
{
    openScalarExponent();
    openParentheses();

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::closeCompositeExponent()
{
    closeParentheses();

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::openSquareRoot()
{
    out << "sqrt";
    openParentheses();

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::closeSquareRoot()
{
    closeParentheses();

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::openParentheses()
{
    out << '(';

    return *this;
}

sym2::PrintEngine& sym2::PlaintextPrintEngine::closeParentheses()
{
    out << ')';

    return *this;
}
