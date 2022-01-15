
#include "autosimpl.h"
#include <array>
#include <functional>
#include <memory_resource>
#include <vector>
#include "cohenautosimpl.h"
#include "get.h"
#include "numberarithmetic.h"
#include "orderrelation.h"
#include "predicates.h"

namespace sym2 {
    template <class NumericAddFct, class NumericMultiplyFct>
    struct SimplificationBundle {
        std::pmr::memory_resource* buffer;
        NumericAddFct numericAdd;
        NumericMultiplyFct numericMultiply;

        CohenAutoSimpl::Dependencies callbacks{orderLessThan, numericAdd, numericMultiply};
        CohenAutoSimpl simplifier{callbacks, buffer};
    };

    auto createSimplificationBundle(std::pmr::memory_resource* buffer)
    {
        NumberArithmetic numerics{buffer};

        return SimplificationBundle{buffer, std::bind_front(&NumberArithmetic::add, numerics),
          std::bind_front(&NumberArithmetic::multiply, numerics)};
    }
}

sym2::Expr sym2::autoSum(ExprView<> lhs, ExprView<> rhs)
{
    const std::array<ExprView<>, 2> ops{{lhs, rhs}};

    return autoSum(ops);
}

sym2::Expr sym2::autoSum(std::span<const ExprView<>> ops)
{
    std::pmr::memory_resource* const buffer = std::pmr::get_default_resource();
    auto bundle = createSimplificationBundle(buffer);

    return bundle.simplifier.simplifySum(ops);
}

sym2::Expr sym2::autoProduct(ExprView<> lhs, ExprView<> rhs)
{
    const std::array<ExprView<>, 2> ops{{lhs, rhs}};

    return autoProduct(ops);
}

sym2::Expr sym2::autoProduct(std::span<const ExprView<>> ops)
{
    std::pmr::memory_resource* const buffer = std::pmr::get_default_resource();
    auto bundle = createSimplificationBundle(buffer);

    return bundle.simplifier.simplifyProduct(ops);
}

sym2::Expr sym2::autoMinus(ExprView<> arg)
{
    static const Expr minusOne{-1};

    return autoProduct(minusOne, arg);
}

sym2::Expr sym2::autoPower(ExprView<> base, ExprView<> exp)
{
    std::pmr::memory_resource* const buffer = std::pmr::get_default_resource();
    auto bundle = createSimplificationBundle(buffer);

    return bundle.simplifier.simplifyPower(base, exp);
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
