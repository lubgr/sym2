#pragma once

#include <chibi/sexp.h>
#include <span>
#include <stack>
#include <stdexcept>
#include <vector>
#include "expr.h"
#include "operandsview.h"
#include "predicates.h"

namespace sym2 {
    class PreservedSexp {
      public:
        explicit PreservedSexp(sexp notCollected)
            : notCollected{notCollected}
        {}

        const sexp& get() const
        {
            return notCollected;
        }

      private:
        sexp notCollected;
    };

    class SexpPreserver {
      public:
        virtual ~SexpPreserver() = default;
        virtual PreservedSexp markAsPreserved(sexp ctx, sexp what) = 0;
    };

    struct FailedConversionToExpr : std::invalid_argument {
        FailedConversionToExpr(const char* msg, sexp ctx, PreservedSexp orig)
            : std::invalid_argument(msg)
            , orig{std::move(orig)}
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
        FromChibiToExpr(sexp ctx, SexpPreserver& registry);

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
        Expr compositeToExpr(CompositeType kind, std::span<const PreservedSexp> operands);
        Expr compositeToExpr(std::string_view name, UnaryDoubleFctPtr fct, std::span<const PreservedSexp> operands);
        Expr compositeToExpr(std::string_view name, BinaryDoubleFctPtr fct, std::span<const PreservedSexp> operands);
        Expr attemptConstantToExpr(std::string_view name, std::span<const PreservedSexp> operands);

        Expr throwSexp(const char* msg, sexp irritant = nullptr);

        sexp ctx;
        std::stack<PreservedSexp> current; /* For getting the relevant bits into exceptions. */
        SexpPreserver& registry;
    };

    class FromExprToChibi {
      public:
        FromExprToChibi(sexp ctx, SexpPreserver& registry);

        sexp convert(ExprView<> from);

      private:
        PreservedSexp preserve(sexp what);
        sexp symbolFrom(ExprView<symbol> symbol);
        PreservedSexp chibiSymbolFromNamedExpr(ExprView<> named);
        PreservedSexp chibiSymbolFromString(std::string_view name);
        sexp dispatchOver(ExprView<number> from);
        PreservedSexp serializeLargeInt(const LargeInt& n);
        sexp symbolDoubleListFrom(ExprView<constant> from);
        sexp compositeFromFunction(ExprView<function> fct);
        sexp serializeListWithLeadingSymbol(const PreservedSexp& identifier, OperandsView rest);
        PreservedSexp leadingSymbolForComposite(ExprView<sum || product || power> composite);
        sexp compositeFromSumProductOrPower(ExprView<sum || product || power> composite);

        sexp ctx;
        SexpPreserver& registry;
    };

    std::vector<Expr> convertList(sexp ctx, sexp list, SexpPreserver& registry);
}
