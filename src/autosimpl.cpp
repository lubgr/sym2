
#include "autosimpl.h"
#include <array>
#include "get.h"
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
    return Expr{Type::sum, ops};
}

sym2::Expr sym2::autoProduct(ExprView<> lhs, ExprView<> rhs)
{
    // TODO
    std::array<ExprView<>, 2> ops{{lhs, rhs}};
    return autoProduct(ops);
}

sym2::Expr sym2::autoProduct(std::span<const ExprView<>> ops)
{
    if (ops.size() == 1)
        return Expr{ops.front()};
    else if (std::any_of(ops.begin(), ops.end(), isZero))
        return 0_ex;

    const auto res = autoProductIntermediate(ops);

    if (res.empty())
        return 1_ex;
    else if (res.size() == 1)
        return res.front();
    else
        return {Type::product, res};
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
    return Expr{Type::complexNumber, {real, imag}};
}
