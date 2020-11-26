#pragma once

#include "expr.h"

#if __has_include(<numbers>)
#include <numbers>
#else
#include <cmath>
#endif

namespace sym2 {
#if __cpp_lib_math_constants
    inline const Expr pi{"pi", std::numbers::pi};
    inline const Expr euler{"e", std::numbers::e};
#else
    inline const Expr pi{"pi", M_PI};
    inline const Expr euler{"e", M_E};
#endif
}
