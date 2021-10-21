
#include <cassert>
#include <chibi/sexp.h>
#include <deque>
#include <exception>
#include <iostream>
#include <vector>
#include "autosimpl.h"
#include "chibiconverter.h"
#include "orderrelation.h"

using namespace sym2;

class ScopedSexpPreserver : public SexpPreserver {
  public:
    explicit ScopedSexpPreserver(sexp ctx)
        : ctx{ctx}
    {}

    ~ScopedSexpPreserver()
    {
        if (!targets.empty())
            sexp_context_saves(ctx) = targets.back().linkedList.next;
    }

    PreservedSexp markAsPreserved(sexp ctx, sexp what) override
    {
        assert(this->ctx == ctx);

        targets.push_front({});

        Entry& newEntry = targets.front();

        newEntry.what = what;
        newEntry.linkedList.var = &newEntry.what;
        newEntry.linkedList.next = sexp_context_saves(ctx);
        sexp_context_saves(ctx) = &newEntry.linkedList;

        return PreservedSexp{what};
    }

  private:
    struct Entry {
        sexp_gc_var_t linkedList;
        sexp what;
    };

    std::deque<Entry> targets;
    sexp ctx;
};

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
sexp roundtrip(sexp ctx, sexp self, sexp_sint_t n, sexp arg)
{
    ScopedSexpPreserver registry{ctx};
    assert(n == 1);

    return wrappedTryCatch(ctx, self, [&]() {
        FromChibiToExpr first{ctx, registry};
        const Expr expression = first.convert(arg);

        FromExprToChibi second{ctx, registry};

        return second.convert(expression);
    });
}

sexp auto_times(sexp ctx, sexp self, sexp_sint_t n, sexp args)
{
    assert(n = 1 && sexp_listp(ctx, args));

    ScopedSexpPreserver registry{ctx};

    return wrappedTryCatch(ctx, self, [&]() {
        const auto convertedArgs = convertList(ctx, args, registry);
        const auto views = expressionsToViews(convertedArgs);
        const Expr result = autoProduct(views);

        return FromExprToChibi{ctx, registry}.convert(result);
    });

    return SEXP_FALSE;
}

sexp auto_plus(sexp ctx, sexp self, sexp_sint_t n, sexp args)
{
    assert(n = 1 && sexp_listp(ctx, args));
    assert(sexp_unbox_fixnum(sexp_length(ctx, args)) >= 2);

    ScopedSexpPreserver registry{ctx};
    FromChibiToExpr conv{ctx, registry};
    std::vector<Expr> ops;

    try {
        while (!sexp_nullp(args)) {
            const sexp arg = sexp_car(args);
            args = sexp_cdr(args);

            ops.push_back(conv.convert(arg));
        }
    } catch (const FailedConversionToExpr& error) {
        return translate(ctx, self, error);
    }

    // TODO
    return SEXP_FALSE;
}

sexp auto_power(sexp ctx, sexp self, [[maybe_unused]] sexp_sint_t n, sexp base, sexp exponent)
{
    assert(n == 2);

    ScopedSexpPreserver registry{ctx};

    return wrappedTryCatch(ctx, self, [&]() {
        FromChibiToExpr conv{ctx, registry};
        const Expr convertedBase = conv.convert(base);
        const Expr convertedExp = conv.convert(exponent);

        const Expr result = autoPower(convertedBase, convertedExp);

        FromExprToChibi back{ctx, registry};

        return back.convert(result);
    });
}

sexp order_less_than(sexp ctx, sexp self, [[maybe_unused]] sexp_sint_t n, sexp lhs, sexp rhs)
{
    assert(n == 2);

    ScopedSexpPreserver registry{ctx};

    return wrappedTryCatch(ctx, self, [&]() {
        FromChibiToExpr conv{ctx, registry};
        const bool result = orderLessThan(conv.convert(lhs), conv.convert(rhs));
        return result ? SEXP_TRUE : SEXP_FALSE;
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

    return SEXP_VOID;
}
}
