#pragma once

#include <deque>
#include <memory_resource>
#include <utility>
#include "sym2/exprview.h"
#include "operandsview.h"
#include "sym2/predicates.h"
#include "sym2/printengine.h"

namespace sym2 {
    class Expr;

    class PrettyPrinter {
      public:
        enum class PowerAsFraction : bool { asFraction, asPower };

        PrettyPrinter(PrintEngine& engine, PowerAsFraction opt,
          std::pmr::memory_resource* resource = std::pmr::get_default_resource());

        void print(ExprView<> e);

      private:
        void printSymbolOrConstant(ExprView<symbol || constant> e);
        void printNumber(ExprView<number> e);
        void printPower(ExprView<> base, ExprView<!power> exp);
        void printPowerWithNegativeNumericExp(ExprView<> base, ExprView<number && negative> exp);
        void printStandardPower(ExprView<> base, ExprView<!power> exp);
        void printPowerBase(ExprView<> base);
        void printPowerExponent(ExprView<!power> exp);
        void printSum(ExprView<sum> e);
        bool isProductWithNegativeNumeric(ExprView<!sum> summand);
        void printProduct(ExprView<product> e);
        void printProductAsFraction(OperandsView factors);
        std::pair<std::pmr::deque<Expr>, std::pmr::deque<Expr>> collectProductFractions(
          OperandsView originalFactors);
        template <class View>
        void printProductWithoutFractions(View&& factors);
        void printFunction(ExprView<function> e);

        PrintEngine& engine;
        PowerAsFraction powerAsFraction;
        std::pmr::memory_resource* resource;
    };
}
