#pragma once

#include "predicates.h"
#include "view.h"

namespace sym2 {
    // Functions for multivariate polynomial terms with rational number coefficients, symbolic
    // variables and positive integer exponents. Implemented as described in Cohen [2003].

    // All types except numbers will return 1 when both arguments compare equal. This is similiar to
    // Mathematica, but not GiNaC. Prerequisite of this function is that the resulting degree fits
    // into the return type; UB otherwise.
    std::int32_t polyDegree(ExprView<> of, ExprView<> wrt);

    // The min. degree of a polynomial is a variation of the degree; returns the minimal degree,
    // e.g. min. degree(a^2 + a^3) = 2, while the degree will return 3.
    std::int32_t polyMinDegreeNoValidityCheck(ExprView<> of, ExprView<symbol> variable);
    // Throws std::domain_error when the input is not a valid polynomial:
    std::int32_t polyMinDegreeWithValidityCheck(ExprView<> of, ExprView<symbol> variable);

    // Valid polynomials are symbols, rational numbers, or composites of these leaves. In addition,
    // powers must have a small, integral, and positive exponent.
    bool isValidPolynomial(ExprView<> p);
}
