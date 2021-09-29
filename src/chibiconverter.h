#pragma once

#include <chibi/sexp.h>
#include <span>
#include <stack>
#include <stdexcept>
#include <vector>
#include "chibiutils.h"
#include "expr.h"
#include "operandsview.h"
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

    class FromChibiToExpr {
      public:
        explicit FromChibiToExpr(sexp ctx);

        Expr convert(sexp from);

      private:
        Expr convert(const PreservedSexp& from); /* Pure syntactical convenience. */
        PreservedSexp preserve(sexp what);
        Expr numberToExpr(sexp from);
        LargeInt extractSmallOrLargeInt(sexp from);
        Expr symbolFromString(sexp str);
        SymbolFlag symbolConstraintsOrThrow(std::string_view flags);
        std::vector<PreservedSexp> collectItems(sexp list);
        Expr nonEmptyListToExpr(std::span<const PreservedSexp> from);
        Expr compositeToExpr(Type kind, std::span<const PreservedSexp> operands);
        Expr compositeToExpr(std::string_view name, UnaryDoubleFctPtr fct, std::span<const PreservedSexp> operands);
        Expr compositeToExpr(std::string_view name, BinaryDoubleFctPtr fct, std::span<const PreservedSexp> operands);
        Expr attemptConstantToExpr(std::string_view name, std::span<const PreservedSexp> operands);

        Expr throwSexp(const char* msg, sexp irritant = nullptr);

        sexp ctx;
        std::stack<PreservedSexp> current; /* For getting the relevant bits into exceptions. */
    };

    class FromExprToChibi {
      public:
        explicit FromExprToChibi(sexp ctx);

        sexp convert(ExprView<> from);

      private:
        PreservedSexp preserve(sexp what);
        sexp symbolFrom(ExprView<symbol> symbol);
        PreservedSexp chibiSymbolFromNamedExpr(ExprView<> named);
        PreservedSexp chibiSymbolFromString(std::string_view name);
        sexp dispatchOver(ExprView<number> from);
        PreservedSexp serializeLargeInt(const LargeInt& n);
        sexp symbolDoubleListFrom(ExprView<constant> from);
        sexp compositeFrom(ExprView<function> fct);
        sexp serializeListWithLeadingSymbol(const PreservedSexp& identifier, OperandsView rest);
        PreservedSexp leadingSymbolForComposite(ExprView<> composite);
        sexp compositeFrom(ExprView<sum || product || power> composite);

        sexp ctx;
    };

    std::vector<Expr> convertList(sexp ctx, sexp list);
}
