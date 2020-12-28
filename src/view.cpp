
#include "view.h"
#include <cstring>

bool sym2::operator==(ExprView lhs, ExprView rhs)
{
    if (lhs.size() != rhs.size())
        return false;
    else
        return std::memcmp(&lhs.front(), &rhs.front(), lhs.size() * sizeof(Operand)) == 0;
}

bool sym2::operator!=(ExprView lhs, ExprView rhs)
{
    return !(lhs == rhs);
}
