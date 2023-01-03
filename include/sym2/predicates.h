#pragma once

#include "exprview.h"
#include "predicateexpr.h"

namespace sym2 {
    bool isRealDomain(ExprView<> e) noexcept;
    bool isComplexDomain(ExprView<> e) noexcept;
    bool isNumber(ExprView<> e) noexcept;
    bool isInteger(ExprView<> e) noexcept;
    bool isRational(ExprView<> e) noexcept;
    bool isFloatingPoint(ExprView<> e) noexcept;
    bool isSmall(ExprView<> e) noexcept;
    bool isLarge(ExprView<> e) noexcept;
    bool isScalar(ExprView<> e) noexcept;
    bool isComposite(ExprView<> e) noexcept;
    bool isSymbol(ExprView<> e) noexcept;
    bool isConstant(ExprView<> e) noexcept;
    bool isSum(ExprView<> e) noexcept;
    bool isProduct(ExprView<> e) noexcept;
    bool isPower(ExprView<> e) noexcept;
    bool isFunction(ExprView<> e) noexcept;
    bool isNumericallyEvaluable(ExprView<> e) noexcept;
    bool isPositive(ExprView<> e) noexcept;
    bool isNegative(ExprView<> e) noexcept;

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
}
