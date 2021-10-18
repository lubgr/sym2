
#include "query.h"
#include "access.h"

std::size_t sym2::nOperands(ExprView<> e)
{
    return nOperands(e[0]);
}

std::size_t sym2::nPhysicalChildren(ExprView<> e)
{
    return nPhysicalChildren(e[0]);
}
