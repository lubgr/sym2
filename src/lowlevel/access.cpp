
#include "access.h"

sym2::Type sym2::type(ExprView<> e)
{
    return e[0].header;
}

sym2::Flag sym2::flags(ExprView<> e)
{
    return e[0].flags;
}

std::size_t sym2::nOperands(Blob b)
{
    switch (b.header) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::floatingPoint:
        case Type::largeInt:
        case Type::largeRational:
        case Type::symbol:
        case Type::constant:
            return 0;
        default:
            return b.mid.nLogicalOrPhysicalChildren;
    }
}

std::size_t sym2::nPhysicalChildren(Blob b)
{
    switch (b.header) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::floatingPoint:
        case Type::symbol:
        case Type::constant:
            return 0;
        default:
            return b.main.nChildBlobs;
    }
}
