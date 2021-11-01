
# sym2: Small, performant C++ library for basic computer algebra

This small C++ library intends to provide a simple approach to symbolic algebra and optimal runtime
performance. In contrast to fully-fledged computer algebra systems or libraries (e.g. maxima, GiNaC,
sympy), only basic features are/will be implemented, e.g. automatic simplification, normalization,
and expansion, trigonometric and logarithmiic functions, arbitrary precision integers. Most
algorithms are/will be implemented according to _Cohen, Computer Algebra and Symbolic Computation
[2003]_.

Primary target platforms are Linux, BSD and MacOS. This library is under development and unusable at
this point (apart from the tests). See the [notes on the implementation](docs/implementation.md) for
details on the design of internal high-speed data structures or [notes on testing](docs/testing.md)
the scripting interface.
