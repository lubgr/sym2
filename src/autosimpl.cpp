
#include "autosimpl.h"
#include <array>
#include <functional>
#include <memory_resource>
#include "get.h"
#include "numberarithmetic.h"
#include "orderrelation.h"
#include "powersimpl.h"
#include "predicates.h"
#include "productsimpl.h"
#include "sumsimpl.h"

sym2::Expr sym2::autoSum(ExprView<> lhs, ExprView<> rhs)
{
    // TODO
    std::array<ExprView<>, 2> ops{{lhs, rhs}};
    return autoSum(ops);
}

sym2::Expr sym2::autoSum(std::span<const ExprView<>> ops)
{
    // TODO
    return Expr{CompositeType::sum, ops};
}

sym2::Expr sym2::autoProduct(ExprView<> lhs, ExprView<> rhs)
{
    // TODO
    std::array<ExprView<>, 2> ops{{lhs, rhs}};
    return autoProduct(ops);
}

sym2::Expr sym2::autoProduct(std::span<const ExprView<>> ops)
{
    std::pmr::memory_resource* const buffer = std::pmr::get_default_resource();
    NumberArithmetic numerics{buffer};
    auto numericMultiply = std::bind_front(&NumberArithmetic::multiply, numerics);
    ProductSimpl::Dependencies callbacks{
      orderLessThan, autoPower, static_cast<Expr (*)(ExprView<>, ExprView<>)>(autoSum), numericMultiply};
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
