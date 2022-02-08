#pragma once

#include "predicateexpr.h"
#include "view.h"

namespace sym2 {
    bool isRealDomain(ExprView<> e);
    bool isComplexDomain(ExprView<> e);
    bool isNumber(ExprView<> e);
    bool isInteger(ExprView<> e);
    bool isRational(ExprView<> e);
    bool isFloatingPoint(ExprView<> e);
    bool isSmall(ExprView<> e);
    bool isLarge(ExprView<> e);
    bool isScalar(ExprView<> e);
    bool isComposite(ExprView<> e);
    bool isSymbol(ExprView<> e);
    bool isConstant(ExprView<> e);
    bool isSum(ExprView<> e);
    bool isProduct(ExprView<> e);
    bool isPower(ExprView<> e);
    bool isFunction(ExprView<> e);
    bool isNumericallyEvaluable(ExprView<> e);
    bool isPositive(ExprView<> e);
    bool isNegative(ExprView<> e);
    bool isZero(ExprView<> e);
    bool isOne(ExprView<> e);

    constexpr inline auto realDomain = predicate<isRealDomain>();
    constexpr inline auto complexDomain = predicate<isComplexDomain>();
    constexpr inline auto number = predicate<isNumber>();
    constexpr inline auto integer = predicate<isInteger>();
    constexpr inline auto rational = predicate<isRational>();
    constexpr inline auto floatingPoint = predicate<isFloatingPoint>();
    constexpr inline auto small = predicate<isSmall>();
    constexpr inline auto large = predicate<isLarge>();
    constexpr inline auto composite = predicate<isComposite>(); // Sum, product, power, function.
    constexpr inline auto scalar = predicate<isScalar>(); // Not a composite
    constexpr inline auto symbol = predicate<isSymbol>();
    constexpr inline auto constant = predicate<isConstant>();
    constexpr inline auto sum = predicate<isSum>();
    constexpr inline auto product = predicate<isProduct>();
    constexpr inline auto power = predicate<isPower>();
    constexpr inline auto function = predicate<isFunction>();
    constexpr inline auto numericallyEvaluable = predicate<isNumericallyEvaluable>();
    // The notion of 'positive' and 'negative' only refer to the real part of an expression that is
    // potentially in the complex domain. Also, can only deliver meaningful results for functions
    // that are numerically evaluable.
    constexpr inline auto positive = predicate<isPositive>();
    constexpr inline auto negative = predicate<isNegative>();
    constexpr inline auto zero = predicate<isZero>();
    constexpr inline auto one = predicate<isOne>();
}
