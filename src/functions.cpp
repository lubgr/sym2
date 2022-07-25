
#include "autosimpl.h"
#include "logarithm.h"
#include "polynomial.h"
#include "sym2/sym2.h"
#include "trigonometric.h"

sym2::Var sym2::sqrt(const Var& base)
{
    static const Var half{1, 2};

    return pow(base, half);
}

sym2::Var sym2::pow(const Var& base, const Var& exp)
{
    return Var(autoPower(base.get(), exp.get()));
}

namespace sym2 {
    namespace {
        Var unary(Expr (*f)(ExprView<>), const Var& arg)
        {
            return Var(f(arg.get()));
        }
    }
}

sym2::Var sym2::log(const Var& arg)
{
    return unary(sym2::log, arg);
}

sym2::Var sym2::sin(const Var& arg)
{
    return unary(sym2::sin, arg);
}

sym2::Var sym2::cos(const Var& arg)
{
    return unary(sym2::cos, arg);
}

sym2::Var sym2::tan(const Var& arg)
{
    return unary(sym2::tan, arg);
}

sym2::Var sym2::asin(const Var& arg)
{
    return unary(sym2::asin, arg);
}

sym2::Var sym2::acos(const Var& arg)
{
    return unary(sym2::acos, arg);
}

sym2::Var sym2::atan(const Var& arg)
{
    return unary(sym2::atan, arg);
}

sym2::Var sym2::atan2(const Var& y, const Var& x)
{
    return Var{sym2::atan2(y.get(), x.get())};
}

std::int32_t sym2::degree(const Var& arg, const Var& wrt)
{
    return polyDegree(arg.get(), wrt.get());
}
