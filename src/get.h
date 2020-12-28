#pragma once

#include <cstdint>
#include <string_view>
#include "rational.h"
#include "view.h"

namespace sym2 {
    template <class T>
    T get(ExprView e) = delete;
    /* If there is a mismatch between requested and actual type, this results in UB. Still, these specializations try to
     * be as applicable as possible: anything that can be numerically evaluated can be retrieved as get<double>, and a
     * small integer can be accessed via get<Int> etc. */
    template <>
    std::int32_t get<std::int32_t>(ExprView e);
    template <>
    SmallRational get<SmallRational>(ExprView e);
    template <>
    double get<double>(ExprView e);
    template <>
    Int get<Int>(ExprView e);
    template <>
    Rational get<Rational>(ExprView e);
    template <>
    std::string_view get<std::string_view>(ExprView e);
    template <>
    UnaryDoubleFctPtr get<UnaryDoubleFctPtr>(ExprView e);
    template <>
    BinaryDoubleFctPtr get<BinaryDoubleFctPtr>(ExprView e);
}
