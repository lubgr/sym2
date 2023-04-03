
# sym2: Small, snappy C++ library for basic computer algebra

This small C++ library intends to provide a simple approach to symbolic algebra and good runtime
performance. In contrast to fully-fledged computer algebra systems or libraries (e.g. maxima, GiNaC,
sympy), only basic features are/will be implemented, e.g. automatic simplification, normalization,
and expansion, trigonometric and logarithmiic functions, or arbitrary precision integers. Most
algorithms are/will follow the techniques outlined in _Cohen, Computer Algebra and Symbolic
Computation [2003]_.

Primary target platforms are Linux, BSD and MacOS. This library is under development and unusable at
this point (apart from the tests). See the [notes on the implementation](docs/implementation.md) for
details on the design of internal data structures or [notes on testing](docs/testing.md) the
scripting interface.

## Planned features

- Automatic simplification: `a + b + 2*a = 3*a + b`
- Expression expansion: `(a + b)^2 = a^2 + 2*a*b + b^2`
- Substitute (sub-)expressions `3*a + b = 6 + b` for `a = 2`
- Trigonometric functions: `sin(pi/4) = 1/sqrt(2)` or `sin^2(a) + cos^2(a) = 1`
- Expression normalization via generalized gcd: `a/b + 1/(5*b) = 1/5*(1 + 5*a)/b`
- Differentiation: `d/da 2*a^4 = 8*a^3`
- Multiprecision integer arithmetic (not for floating point)

## Extensibility restriction

Design-wise, the system is _not_ extensible through subclasses that implement an interface for
expressions. Instead, the expression types are fixed (symbol, integer, rational number, floating
point number, complex number, sum, product, power, unary, function, binary function), and it is
assumed that the existing types are sufficient to solve the users' needs. If that is not the case,
you are probably looking for a different tool.
