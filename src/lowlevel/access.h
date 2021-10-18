#pragma once

#include "blob.h"
#include "view.h"

namespace sym2 {
    Type type(ExprView<> e);
    Flag flags(ExprView<> e);

    std::size_t nOperands(Blob b);
    std::size_t nPhysicalChildren(Blob b);
}
