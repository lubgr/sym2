
#include <cassert>
#include <chibi/sexp.h>
#include <exception>
#include <iostream>
#include <vector>
#include "chibiconverter.h"
#include "chibiutils.h"

using namespace sym2;

namespace {
    sexp translate(sexp ctx, sexp fct, const FailedConversionToExpr& error)
    {
        return sexp_user_exception(ctx, fct, error.what(), error.orig.get());
    }

    sexp translate(sexp ctx, sexp fct, const FailedConversionToSexp& error)
    {
        return sexp_user_exception(ctx, fct, error.what(), SEXP_FALSE);
    }
}

extern "C" {
sexp roundtrip(sexp ctx, sexp self, sexp_sint_t n, sexp arg)
{
    assert(n == 1);
    ChibiConverter conv{ctx};

    try {
        const Expr expression = conv.toExpr(arg);
        return conv.fromExpr(expression);
    } catch (const FailedConversionToExpr& error) {
        return translate(ctx, self, error);
    } catch (const FailedConversionToSexp& error) {
        return translate(ctx, self, error);
    }
}

sexp auto_times(sexp ctx, sexp self, sexp_sint_t n, sexp args)
{
    assert(n = 1 && sexp_listp(ctx, args));

    // TODO

    return SEXP_FALSE;
}

sexp auto_plus(sexp ctx, sexp self, sexp_sint_t n, sexp args)
{
    assert(n = 1 && sexp_listp(ctx, args));
    assert(sexp_unbox_fixnum(sexp_length(ctx, args)) >= 2);

    ChibiConverter conv{ctx};
    std::vector<Expr> ops;

    try {
        while (!sexp_nullp(args)) {
            const sexp arg = sexp_car(args);
            args = sexp_cdr(args);

            ops.push_back(conv.toExpr(arg));
        }
    } catch (const FailedConversionToExpr& error) {
        return translate(ctx, self, error);
    }

    // TODO
    return SEXP_FALSE;
}

sexp sexp_init_library(
  sexp ctx, sexp self, sexp_sint_t n, sexp env, const char* version, const sexp_abi_identifier_t abi)
{
    if (!(sexp_version_compatible(ctx, version, sexp_version) && sexp_abi_compatible(ctx, abi, SEXP_ABI_IDENTIFIER)))
        return SEXP_ABI_ERROR;

    sexp_define_foreign(ctx, env, "roundtrip", 1, roundtrip);
    sexp_define_foreign(ctx, env, "auto-plus", 1, auto_plus);
    sexp_define_foreign(ctx, env, "auto-times", 1, auto_times);

    return SEXP_VOID;
}
}
