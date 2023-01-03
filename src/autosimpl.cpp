
#include "sym2/autosimpl.h"
#include <array>
#include <functional>
#include <memory_resource>
#include <vector>
#include "allocator.h"
#include "cohenautosimpl.h"
#include "sym2/get.h"
#include "numberarithmetic.h"
#include "orderrelation.h"
#include "sym2/predicates.h"

namespace sym2 {
    template <class NumericAddFct, class NumericMultiplyFct>
    struct SimplificationBundle {
        std::pmr::memory_resource* mr;
        NumericAddFct numericAdd;
        NumericMultiplyFct numericMultiply;

        CohenAutoSimpl::Dependencies callbacks{orderLessThan, numericAdd, numericMultiply};
        CohenAutoSimpl simplifier{callbacks, mr};
    };

    auto createSimplificationBundle(std::pmr::memory_resource* mr)
    {
        NumberArithmetic numerics{mr};

        return SimplificationBundle{mr, std::bind_front(&NumberArithmetic::add, numerics),
          std::bind_front(&NumberArithmetic::multiply, numerics)};
    }
}

sym2::Expr sym2::autoSum(ExprView<> lhs, ExprView<> rhs)
{
    return autoSum({{lhs, rhs}});
}

sym2::Expr sym2::autoSum(std::span<const ExprView<>> ops)
{
    auto [_, mr] = monotonicStackPmrResource<ByteSize{1000}>();
    auto bundle = createSimplificationBundle(&mr);
    const Expr result = bundle.simplifier.simplifySum(ops);

    return Expr{result};
}

sym2::Expr sym2::autoProduct(ExprView<> lhs, ExprView<> rhs)
{
    return autoProduct({{lhs, rhs}});
}

sym2::Expr sym2::autoProduct(std::span<const ExprView<>> ops)
{
    auto [_, mr] = monotonicStackPmrResource<ByteSize{1000}>();
    auto bundle = createSimplificationBundle(&mr);
    const Expr result = bundle.simplifier.simplifyProduct(ops);

    return Expr{result};
}

sym2::Expr sym2::autoMinus(ExprView<> arg)
{
    return autoProduct(FixedExpr<1>{-1}, arg);
}

sym2::Expr sym2::autoPower(ExprView<> base, ExprView<> exp)
{
    auto [_, mr] = monotonicStackPmrResource<ByteSize{1000}>();
    auto bundle = createSimplificationBundle(&mr);
    const Expr result = bundle.simplifier.simplifyPower(base, exp);

    return Expr{result};
}

sym2::Expr sym2::autoOneOver(ExprView<> arg)
{
    return autoPower(arg, FixedExpr<1>{-1});
}

sym2::Expr sym2::autoComplex(ExprView<> real, ExprView<> imag)
{
    // TODO
    return Expr{CompositeType::complexNumber, real, imag, {}};
}
