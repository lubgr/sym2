
#include "logarithm.h"
#include <cmath>

sym2::Expr sym2::log(ExprView<> arg, std::pmr::polymorphic_allocator<> alloc)
{
    return Expr{"log", arg, std::log, alloc};
}
