#pragma once

#include "expr.h"

#if __has_include(<numbers>)
#include <numbers>
#else
#include <cmath>
#endif

namespace sym2 {
#if __cpp_lib_math_constants
    inline const FixedExpr<3> pi{"pi", std::numbers::pi};
    inline const FixedExpr<3> euler{"e", std::numbers::e};
#else
    inline const FixedExpr<3> pi{"pi", M_PI};
    inline const FixedExpr<3> euler{"e", M_E};
#endif
}
