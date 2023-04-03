# sym2: Small C++ library for computer algebra

This small library intends to provide a simple approach to symbolic algebra and good runtime
performance. In contrast to fully-fledged computer algebra systems or libraries (e.g. maxima, GiNaC,
sympy), only basic features are/will be implemented, e.g. automatic simplification, normalization,
and expansion, trigonometric and logarithmiic functions, or arbitrary precision integers. Most
algorithms are/will follow the techniques outlined in _Cohen, Computer Algebra and Symbolic
Computation [2003]_.

Primary target platforms are  64bit Linux, BSD and MacOS. This library is under development and
_highly_ unusable at this point (apart from the tests). The code is MIT licensed, with a small
Apache-2.0 exception. See LICENSE for more details.

## Planned features

- Automatic simplification: `a + b + 2*a = 3*a + b`
- Expression expansion: `(a + b)^2 = a^2 + 2*a*b + b^2`
- Substitute (sub-)expressions `3*a + b = 6 + b` for `a = 2`
- Trigonometric functions: `sin(pi/4) = 1/sqrt(2)` or `sin^2(a) + cos^2(a) = 1`
- Expression normalization via generalized gcd: `a/b + 1/(5*b) = 1/5*(1 + 5*a)/b`
- Differentiation: `d/da 2*a^4 = 8*a^3`
- Multiprecision integer arithmetic (not for floating point numbers)

## Getting started

The build configuration is written in `CMake`, 3rd party dependencies are managed with `conan`.
These dependencies are built using the `conan` recipes in `lib/`. Important: the expected compiler
is `gcc-12`, together with `libstdc++`; expect build failures for any other toolchain. There is no
such thing as a release yet, so the following sets up a default development/debug build with all
existing targets and UBSan/ASan enabled, for use with `ninja` and `ccache`:
```
git clone https://github.com/lubgr/sym2.git
cd sym2

conan create lib/benchmark
conan create lib/boost-headers
conan create lib/chibi-scheme
conan create lib/cmake-coverage
conan create lib/doctest
conan create lib/ginac

mkdir build-debug
cd !$

conan install ..
cmake --preset default ..
ninja
ctest
```
For more options like release builds, coverage, building subsets (no benchmarks, no bindings, etc.),
checkout the toplevel `CMakeLists.txt` or `CMakePresets.json`.

## Notes

See the [notes on the implementation](docs/implementation.md) for details on the design of internal
data structures or [notes on testing](docs/testing.md) for the scripting interface.

Design-wise, the library is _not_ intended to be extensible through subclasses that implement an
interface for expressions. Instead, the expression types are fixed (symbol, integer, rational
number, floating point number, complex number, sum, product, power, unary, function, binary
function), and it is assumed that the existing types are sufficient to solve the users' needs. If
that is not the case, you are probably looking for a different tool.
