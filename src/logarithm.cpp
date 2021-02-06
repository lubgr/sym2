
#include "logarithm.h"
#include <cmath>

sym2::Expr sym2::log(ExprView arg)
{
    return Expr{"log", arg, std::log};
}
