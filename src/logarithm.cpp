
#include "logarithm.h"
#include <cmath>

sym2::Expr sym2::log(ExprView<> arg, Expr::allocator_type alloc)
{
    return Expr{"log", arg, std::log, alloc};
}
