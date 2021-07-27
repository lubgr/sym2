#pragma once

#include <chibi/sexp.h>
#include <stdexcept>
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
        LargeInt extractSmallOrLargeInt(sexp from);
        PreservedSexp serializeLargeInt(const LargeInt& n);

        sexp ctx;
    };
}
