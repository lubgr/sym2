#pragma once

#include <chibi/sexp.h>
#include <memory>
#include <memory_resource>
#include <span>
#include <stack>
#include <stdexcept>
#include <vector>
#include "operandsview.h"
#include "sym2/expr.h"
#include "sym2/predicates.h"

namespace sym2 {
    class PreservedSexp {
      public:
        PreservedSexp(sexp ctx, sexp toPreserve);

        const sexp& get() const;

      private:
        struct Payload {
            sexp preserved;
            sexp ctx;
        };

        std::shared_ptr<Payload> handle;
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
            , orig{std::move(orig), {}}
        {}

        Expr orig;
    };

    class FromChibiToExpr {
      public:
        explicit FromChibiToExpr(sexp ctx, std::pmr::polymorphic_allocator<> allocator = {});

        Expr convert(sexp from);

      private:
        Expr convert(const PreservedSexp& from); /* Pure syntactical convenience. */
        PreservedSexp preserve(sexp what);
        Expr numberToExpr(sexp from);
        LargeInt extractSmallOrLargeInt(sexp from);
        Expr symbolFromString(sexp str);
        DomainFlag symbolConstraintsOrThrow(std::string_view flags);
        std::vector<PreservedSexp> collectItems(sexp list);
        Expr nonEmptyListToExpr(std::span<const PreservedSexp> from);
        Expr compositeToExpr(CompositeType kind, std::span<const PreservedSexp> operands);
        Expr compositeToExpr(
          std::string_view name, UnaryDoubleFctPtr fct, std::span<const PreservedSexp> operands);
        Expr compositeToExpr(
          std::string_view name, BinaryDoubleFctPtr fct, std::span<const PreservedSexp> operands);
        Expr attemptConstantToExpr(std::string_view name, std::span<const PreservedSexp> operands);

        Expr throwSexp(const char* msg, sexp irritant = nullptr);

        sexp ctx;
        std::pmr::polymorphic_allocator<> alloc;
        std::stack<PreservedSexp> current; /* For getting the relevant bits into exceptions. */
    };

    class FromExprToChibi {
      public:
        explicit FromExprToChibi(sexp ctx, std::pmr::polymorphic_allocator<> allocator = {});

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
        std::pmr::polymorphic_allocator<> alloc;
    };

    std::pmr::vector<Expr> convertFromList(sexp ctx, sexp list);
    sexp convertToList(sexp ctx, OperandsView operands);
}
