#pragma once

#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string_view>
#include "symbolflag.h"

namespace sym2 {
    class PrintEngine {
      public:
        virtual ~PrintEngine() = default;

        virtual PrintEngine& symbol(std::string_view name, std::optional<SymbolFlag> flag) = 0;
        virtual PrintEngine& functionName(std::string_view name) = 0;
        virtual PrintEngine& floatingPoint(double n) = 0;
        virtual PrintEngine& integer(std::int32_t n) = 0;
        virtual PrintEngine& largeInteger(std::string_view n) = 0;

        virtual PrintEngine& plusSign() = 0;
        virtual PrintEngine& minusSign() = 0;
        virtual PrintEngine& unaryMinusSign() = 0;
        virtual PrintEngine& timesSign() = 0;
        virtual PrintEngine& divisionSign() = 0;
        virtual PrintEngine& comma() = 0;

        virtual PrintEngine& openNumerator(bool numeratorIsSum = false) = 0;
        virtual PrintEngine& closeNumerator(bool numeratorWasSum = false) = 0;
        virtual PrintEngine& openDenominator(bool denominatorIsScalar = false) = 0;
        virtual PrintEngine& closeDenominator(bool denominatorWasScalar = false) = 0;

        virtual PrintEngine& timesImaginayI() = 0;
        virtual PrintEngine& singleImaginaryI() = 0;

        virtual PrintEngine& openScalarExponent() = 0;
        virtual PrintEngine& closeScalarExponent() = 0;
        virtual PrintEngine& openCompositeExponent() = 0;
        virtual PrintEngine& closeCompositeExponent() = 0;

        virtual PrintEngine& openSquareRoot() = 0;
        virtual PrintEngine& closeSquareRoot() = 0;

        virtual PrintEngine& openParentheses() = 0;
        virtual PrintEngine& closeParentheses() = 0;
    };

    class PlaintextPrintEngine : public PrintEngine {
      public:
        explicit PlaintextPrintEngine(std::ostream& out);

        // This engine doesn't do anything with the symbol flag argument.
        PrintEngine& symbol(std::string_view name, std::optional<SymbolFlag>) override;
        PrintEngine& functionName(std::string_view name) override;
        PrintEngine& floatingPoint(double n) override;
        PrintEngine& integer(std::int32_t n) override;
        PrintEngine& largeInteger(std::string_view n) override;

        PrintEngine& plusSign() override;
        PrintEngine& minusSign() override;
        PrintEngine& unaryMinusSign() override;
        PrintEngine& timesSign() override;
        PrintEngine& divisionSign() override;
        PrintEngine& comma() override;

        PrintEngine& openNumerator(bool numeratorIsSum = false) override;
        PrintEngine& closeNumerator(bool numeratorWasSum = false) override;
        PrintEngine& openDenominator(bool denominatorIsScalar = false) override;
        PrintEngine& closeDenominator(bool denominatorWasScalar = false) override;

        PrintEngine& timesImaginayI() override;
        PrintEngine& singleImaginaryI() override;

        PrintEngine& openScalarExponent() override;
        PrintEngine& closeScalarExponent() override;
        PrintEngine& openCompositeExponent() override;
        PrintEngine& closeCompositeExponent() override;

        PrintEngine& openSquareRoot() override;
        PrintEngine& closeSquareRoot() override;

        PrintEngine& openParentheses() override;
        PrintEngine& closeParentheses() override;

      protected:
        std::ostream& out;
    };
}
