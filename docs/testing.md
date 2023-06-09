# Notes on testing and the scripting interface

There are currently two kind of tests: ordinary unit tests and test scripts. The former are written
in C++ using [doctest](https://github.com/onqtam/doctest) and call directly into public and internal
API. The latter are scripts for a [Chibi Scheme](https://github.com/ashinn/chibi-scheme)
interpreter. This interpreter is brought in as an external dependency and invoked through the
`bin/chibi` helper script that is generated in each build directy. The scripted tests work in
conjunction with a shared library that provides bindings for Chibi.

Scheme has been chosen because Lisp's approach to syntax and symbol handling is a good match for
specifying hierarchical data that describe expression trees. The resulting scripted tests are dense,
yet readable, and writing tests is efficient and (hopefully) pleasant. In addition, Chibi's REPL is
suitable for interactive testing with immediate feedback.

Notable downsides of testing through a scripting interface might be the additional complexity of the
language bridging, and a coarser test granularity compared to classical unit tests (when fine
granularity is required, tests can be written against the C++ interface).
