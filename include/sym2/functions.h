#pragma once

#include <string_view>
#include <vector>
#include "var.h"

namespace sym2 {
    Var sqrt(const Var& base);
    Var pow(const Var& base, const Var& exp);
    Var log(const Var& arg);
    Var sin(const Var& arg);
    Var cos(const Var& arg);
    Var tan(const Var& arg);
    Var asin(const Var& arg);
    Var acos(const Var& arg);
    Var atan(const Var& arg);
    Var atan2(const Var& y, const Var& x);

    Var subst(const Var& arg, const Var& from, const Var& to);
    Var expand(const Var& arg);
    Var normal(const Var& arg);
    Var simplify(const Var& arg);
    bool isPositive(const Var& arg);
    bool isNegative(const Var& arg);
    unsigned complexity(const Var& arg);
    Var numerator(const Var& arg);
    Var denominator(const Var& arg);
    /* Returns names for symbols, functions and constants, an empty string otherwise: */
    std::string_view name(const Var& arg);
    std::vector<Var> operands(const Var& arg);
    std::vector<Var> collectSymbols(const Var& arg);
}
