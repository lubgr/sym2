
#include "sym2/exprview.h"
#include "blob.h"

bool sym2::operator==(ExprView<> lhs, ExprView<> rhs)
{
    return equal(lhs.get(), rhs.get());
}

bool sym2::operator!=(ExprView<> lhs, ExprView<> rhs)
{
    return !(lhs == rhs);
}
