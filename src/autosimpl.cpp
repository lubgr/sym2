
#include "autosimpl.h"
#include <array>
#include <functional>
#include <memory_resource>
#include <vector>
#include "get.h"
#include "numberarithmetic.h"
#include "orderrelation.h"
#include "powersimpl.h"
#include "predicates.h"
#include "productsimpl.h"
#include "sumsimpl.h"

sym2::Expr sym2::autoSum(ExprView<> lhs, ExprView<> rhs)
{
    const std::array<ExprView<>, 2> ops{{lhs, rhs}};

    return autoSum(ops);
}

sym2::Expr sym2::autoSum(std::span<const ExprView<>> ops)
{
    std::pmr::memory_resource* const buffer = std::pmr::get_default_resource();
    NumberArithmetic numerics{buffer};
    auto binaryAutoProduct = [&buffer](ExprView<number> n, OperandsView ops) {
        std::pmr::vector<ExprView<>> allOperands{n};
        allOperands.insert(allOperands.end(), ops.begin(), ops.end());
        return autoProduct(allOperands);
    };
    auto numericAdd = std::bind_front(&NumberArithmetic::add, numerics);
    SumSimpl::Dependencies callbacks{orderLessThan, binaryAutoProduct, numericAdd};
    SumSimpl simplifier{callbacks, buffer};

    return simplifier.autoSimplify(ops);
}

sym2::Expr sym2::autoProduct(ExprView<> lhs, ExprView<> rhs)
{
    const std::array<ExprView<>, 2> ops{{lhs, rhs}};

    return autoProduct(ops);
}

sym2::Expr sym2::autoProduct(std::span<const ExprView<>> ops)
{
    std::pmr::memory_resource* const buffer = std::pmr::get_default_resource();
    NumberArithmetic numerics{buffer};
    auto numericMultiply = std::bind_front(&NumberArithmetic::multiply, numerics);
    auto binaryAutoSum = static_cast<Expr (*)(ExprView<>, ExprView<>)>(autoSum);
    ProductSimpl::Dependencies callbacks{orderLessThan, autoPower, binaryAutoSum, numericMultiply};
    ProductSimpl simplifier{callbacks, buffer};

    return simplifier.autoSimplify(ops);
}

sym2::Expr sym2::autoMinus(ExprView<> arg)
{
    static const Expr minusOne{-1};

    return autoProduct(minusOne, arg);
}

sym2::Expr sym2::autoPower(ExprView<> base, ExprView<> exp)
{
    return autoPowerImpl(base, exp);
}

sym2::Expr sym2::autoOneOver(ExprView<> arg)
{
    static const Expr minusOne{-1};

    return autoPower(arg, minusOne);
}

sym2::Expr sym2::autoComplex(ExprView<> real, ExprView<> imag)
{
    // TODO
    return Expr{CompositeType::complexNumber, {real, imag}};
}
