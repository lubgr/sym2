
#include <cassert>
#include <chibi/sexp.h>
#include <exception>
#include <vector>
#include "autosimpl.h"
#include "chibiconverter.h"
#include "orderrelation.h"
#include "query.h"

using namespace sym2;

namespace {
    sexp translate(sexp ctx, sexp fct, const FailedConversionToExpr& error)
    {
        return sexp_user_exception(ctx, fct, error.what(), error.orig.get());
    }

    sexp translate(sexp ctx, sexp fct, const std::exception& error)
    {
        return sexp_user_exception(ctx, fct, error.what(), SEXP_FALSE);
    }

    template <class Fct>
    sexp wrappedTryCatch(sexp ctx, sexp caller, Fct&& f)
    {
        try {
            return std::forward<Fct>(f)();
        } catch (const FailedConversionToExpr& error) {
            return translate(ctx, caller, error);
        } catch (const FailedConversionToSexp& error) {
            return translate(ctx, caller, error);
        } catch (const std::exception& error) {
            return translate(ctx, caller, error);
        }

        return SEXP_NULL;
    }

    template <class Range>
    std::vector<ExprView<>> expressionsToViews(const Range& input)
    {
        return {std::cbegin(input), std::cend(input)};
    }
}

extern "C" {
sexp roundtrip(sexp ctx, sexp self, [[maybe_unused]] sexp_sint_t n, sexp arg)
{
    assert(n == 1);

    return wrappedTryCatch(ctx, self, [&]() {
        FromChibiToExpr first{ctx};
        const Expr expression = first.convert(arg);

        FromExprToChibi second{ctx};

        return second.convert(expression);
    });
}

sexp auto_times(sexp ctx, sexp self, [[maybe_unused]] sexp_sint_t n, sexp args)
{
    assert(n == 1 && sexp_listp(ctx, args));

    return wrappedTryCatch(ctx, self, [&]() {
        const auto convertedArgs = convertFromList(ctx, args);
        const auto views = expressionsToViews(convertedArgs);
        const Expr result = autoProduct(views);

        return FromExprToChibi{ctx}.convert(result);
    });

    return SEXP_FALSE;
}

sexp auto_plus(sexp ctx, sexp self, [[maybe_unused]] sexp_sint_t n, sexp args)
{
    assert(n == 1 && sexp_listp(ctx, args));
    assert(sexp_unbox_fixnum(sexp_length(ctx, args)) >= 2);

    return wrappedTryCatch(ctx, self, [&]() {
        const auto convertedArgs = convertFromList(ctx, args);
        const auto views = expressionsToViews(convertedArgs);
        const Expr result = autoSum(views);

        return FromExprToChibi{ctx}.convert(result);
    });

    return SEXP_FALSE;
}

sexp auto_power(sexp ctx, sexp self, [[maybe_unused]] sexp_sint_t n, sexp base, sexp exponent)
{
    assert(n == 2);

    return wrappedTryCatch(ctx, self, [&]() {
        FromChibiToExpr conv{ctx};
        const Expr convertedBase = conv.convert(base);
        const Expr convertedExp = conv.convert(exponent);

        const Expr result = autoPower(convertedBase, convertedExp);

        FromExprToChibi back{ctx};

        return back.convert(result);
    });
}

sexp order_less_than(sexp ctx, sexp self, [[maybe_unused]] sexp_sint_t n, sexp lhs, sexp rhs)
{
    assert(n == 2);

    return wrappedTryCatch(ctx, self, [&]() {
        FromChibiToExpr conv{ctx};
        const bool result = orderLessThan(conv.convert(lhs), conv.convert(rhs));
        return result ? SEXP_TRUE : SEXP_FALSE;
    });
}

sexp const_and_term(sexp ctx, sexp self, [[maybe_unused]] sexp_sint_t n, sexp arg)
{
    assert(n == 1);

    if (sexp_numberp(arg))
        return sexp_xtype_exception(ctx, self, "const/term is not defined for numbers", arg);

    return wrappedTryCatch(ctx, self, [&]() {
        FromChibiToExpr conv{ctx};
        FromExprToChibi back{ctx};

        const Expr toSplit = conv.convert(arg);
        const ConstAndTerm result = splitConstTerm(toSplit);
        const PreservedSexp term{ctx, convertToList(ctx, result.term)};
        const PreservedSexp constant{ctx, back.convert(result.constant)};
        return sexp_list2(ctx, constant.get(), term.get());
    });
}

sexp sexp_init_library(sexp ctx, [[maybe_unused]] sexp self, [[maybe_unused]] sexp_sint_t n, sexp env,
  const char* version, const sexp_abi_identifier_t abi)
{
    if (!(sexp_version_compatible(ctx, version, sexp_version) && sexp_abi_compatible(ctx, abi, SEXP_ABI_IDENTIFIER)))
        return SEXP_ABI_ERROR;

    sexp_define_foreign(ctx, env, "roundtrip", 1, roundtrip);
    sexp_define_foreign(ctx, env, "auto-plus", 1, auto_plus);
    sexp_define_foreign(ctx, env, "auto-times", 1, auto_times);
    sexp_define_foreign(ctx, env, "auto^", 2, auto_power);
    sexp_define_foreign(ctx, env, "order-lt", 2, order_less_than);
    sexp_define_foreign(ctx, env, "split-const-term", 1, const_and_term);

    return SEXP_VOID;
}
}
