#pragma once

#include <chibi/sexp.h>
#include <span>
#include <stdexcept>
#include <vector>
#include "chibiutils.h"
#include "expr.h"
#include "predicates.h"
#include "smallvec.h"

namespace sym2 {
    struct FailedConversionToExpr : std::invalid_argument {
        FailedConversionToExpr(const char* msg, sexp ctx, sexp orig)
            : std::invalid_argument(msg)
            , orig{ctx, orig}
            , ctx{ctx}
        {}

        PreservedSexp orig;
        sexp ctx;
    };

    struct FailedConversionToSexp : std::invalid_argument {
        FailedConversionToSexp(const char* msg, Expr orig)
            : std::invalid_argument(msg)
            , orig{orig}
        {}

        Expr orig;
    };

    class ChibiConverter {
      public:
        explicit ChibiConverter(sexp ctx);

        Expr toExpr(sexp from);

        sexp fromExpr(ExprView<> from);

      private:
        Expr toExpr(const PreservedSexp& from);
        Expr numberToExpr(sexp from);
        LargeInt extractSmallOrLargeInt(sexp from);
        std::vector<PreservedSexp> collectItems(sexp list);
        Expr nonEmptyListToExpr(std::span<const PreservedSexp> from);
        Expr compositeToExpr(Type kind, std::span<const PreservedSexp> operands);
        Expr compositeToExpr(std::string_view name, UnaryDoubleFctPtr fct, std::span<const PreservedSexp> operands);
        Expr compositeToExpr(std::string_view name, BinaryDoubleFctPtr fct, std::span<const PreservedSexp> operands);
        Expr constantToExpr(std::string_view name, std::span<const PreservedSexp> operands);
        PreservedSexp serializeLargeInt(const LargeInt& n);

        sexp ctx;
    };
}
