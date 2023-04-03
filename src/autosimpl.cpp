
#include "sym2/autosimpl.h"
#include <functional>
#include <vector>
#include "cohenautosimpl.h"
#include "numberarithmetic.h"
#include "orderrelation.h"
#include "sym2/get.h"
#include "sym2/predicates.h"

namespace sym2 {
    template <class NumericAddFct, class NumericMultiplyFct>
    struct SimplificationBundle {
        Expr::allocator_type allocator;
        NumericAddFct numericAdd;
        NumericMultiplyFct numericMultiply;

        CohenAutoSimpl::Dependencies callbacks{orderLessThan, numericAdd, numericMultiply};
        CohenAutoSimpl simplifier{callbacks, allocator};
    };

    auto createSimplificationBundle(Expr::allocator_type allocator)
    {
        NumberArithmetic numerics{allocator};

        return SimplificationBundle{allocator, std::bind_front(&NumberArithmetic::add, numerics),
          std::bind_front(&NumberArithmetic::multiply, numerics)};
    }
}

sym2::Expr sym2::autoSum(ExprView<> lhs, ExprView<> rhs, Expr::allocator_type allocator)
{
    return autoSum({{lhs, rhs}}, allocator);
}

sym2::Expr sym2::autoSum(std::span<const ExprView<>> ops, Expr::allocator_type allocator)
{
    StackBuffer<1024> arena;
    auto bundle = createSimplificationBundle(&arena);
    const Expr result = bundle.simplifier.simplifySum(ops);

    return Expr{result, allocator};
}

sym2::Expr sym2::autoSum(std::initializer_list<ExprView<>> ops, Expr::allocator_type allocator)
{
    return autoSum(std::span<const ExprView<>>{ops}, allocator);
}

sym2::Expr sym2::autoProduct(ExprView<> lhs, ExprView<> rhs, Expr::allocator_type allocator)
{
    return autoProduct({{lhs, rhs}}, allocator);
}

sym2::Expr sym2::autoProduct(std::span<const ExprView<>> ops, Expr::allocator_type allocator)
{
    StackBuffer<1024> arena;
    auto bundle = createSimplificationBundle(&arena);
    const Expr result = bundle.simplifier.simplifyProduct(ops);

    return Expr{result, allocator};
}

sym2::Expr sym2::autoProduct(std::initializer_list<ExprView<>> ops, Expr::allocator_type allocator)
{
    return autoProduct(std::span<const ExprView<>>{ops}, allocator);
}

sym2::Expr sym2::autoMinus(ExprView<> arg, Expr::allocator_type allocator)
{
    return autoProduct(FixedExpr<1>{-1}, arg, allocator);
}

sym2::Expr sym2::autoPower(ExprView<> base, ExprView<> exp, Expr::allocator_type allocator)
{
    StackBuffer<1024> arena;
    auto bundle = createSimplificationBundle(&arena);
    const Expr result = bundle.simplifier.simplifyPower(base, exp);

    return Expr{result, allocator};
}

sym2::Expr sym2::autoOneOver(ExprView<> arg, Expr::allocator_type allocator)
{
    return autoPower(arg, FixedExpr<1>{-1}, allocator);
}

sym2::Expr sym2::autoComplex(ExprView<> real, ExprView<> imag, Expr::allocator_type allocator)
{
    // TODO
    return Expr{CompositeType::complexNumber, real, imag, allocator};
}
