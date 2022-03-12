
#include "autosimpl.h"
#include <array>
#include <functional>
#include <memory_resource>
#include <vector>
#include "allocator.h"
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
    auto [_, buffer] = monotonicStackPmrResource<ByteSize{1000}>();
    auto bundle = createSimplificationBundle(&buffer);
    const Expr result = bundle.simplifier.simplifySum(ops);

    return Expr{result};
}

sym2::Expr sym2::autoProduct(ExprView<> lhs, ExprView<> rhs)
{
    const std::array<ExprView<>, 2> ops{{lhs, rhs}};

    return autoProduct(ops);
}

sym2::Expr sym2::autoProduct(std::span<const ExprView<>> ops)
{
    auto [_, buffer] = monotonicStackPmrResource<ByteSize{1000}>();
    auto bundle = createSimplificationBundle(&buffer);
    const Expr result = bundle.simplifier.simplifyProduct(ops);

    return Expr{result};
}

sym2::Expr sym2::autoMinus(ExprView<> arg)
{
    return autoProduct(ExprLiteral{-1}, arg);
}

sym2::Expr sym2::autoPower(ExprView<> base, ExprView<> exp)
{
    auto [_, buffer] = monotonicStackPmrResource<ByteSize{1000}>();
    auto bundle = createSimplificationBundle(&buffer);
    const Expr result = bundle.simplifier.simplifyPower(base, exp);

    return Expr{result};
}

sym2::Expr sym2::autoOneOver(ExprView<> arg)
{
    return autoPower(arg, ExprLiteral{-1});
}

sym2::Expr sym2::autoComplex(ExprView<> real, ExprView<> imag)
{
    // TODO
    return Expr{CompositeType::complexNumber, {real, imag}, {}};
}
