#pragma once

#include <cstdint>
#include <string_view>
#include "expr.h"

namespace sym2 {
    struct ExprView;

    bool isScalar(ExprView e);
    bool isNumber(ExprView e);
    bool isSymbolOrConstant(ExprView e);
    bool isSymbol(ExprView e);
    bool isConstant(ExprView e);
    bool isSum(ExprView e);
    bool isProduct(ExprView e);
    bool isPower(ExprView e);
    bool isFunction(ExprView e);
    bool isFunction(ExprView e, std::string_view name);

    bool isZero(ExprView e);
    bool isOne(ExprView e);

    ExprView base(ExprView e);
    ExprView exponent(ExprView e);

    bool isPositive(ExprView e);
    bool isNegative(ExprView e);

    std::uint32_t nOps(ExprView e);

    // (define (base expr)
    //   (match expr
    //     (('^ base exponent) base)
    //     ((? number?) #f)
    //     (else expr)))

    // (define (exponent expr)
    //   (match expr
    //     (('^ base exponent) exponent)
    //     ((? number?) #f)
    //     (else 1)))

    // (define (term u)
    //   (match u
    //     ((? number?) #f)
    //     (('* (? number? u1) . u-rest) `(* ,@u-rest))
    //     (('* . u-ops) u)
    //     (else `(* ,u))))

    // (define (const u)
    //   (match u
    //     ((? number?) #f)
    //     (('* (? number? u1) . u-rest) u1)
    //     (('* . u-ops) 1)
    //     (else 1)))
}
