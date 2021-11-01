# Notes on Internal Design Choices and Implementation Details

The central data structure in computer algebra system implementations is a tree. The sum `42 + a +
c^2*sin(b)` can be represented as
```
    [Sum]
    / | \
   /  |   \
 42   a   [Product]
            /   \
           /     \
       [Power] [Function "sin"]
        /   \         |
       /     \        |
      c       2       b
```
Numbers, symbols and constants like Pi or Euler's constant are leafs (factorials could be, too, but
they aren't implemented here), while sums, products, powers and functions are composites. These must
be able to accommodate any number and combination of child and parent nodes. The algorithms to work
with these trees are usually kept as generic as possible, and treating any kind of tree through a
common interface helps with this.

Such structure naturally lends itself to an OOP(-like) model, e.g. the composite design pattern or a
similar variation of it. However, there are two shortcomings with such abstraction:

- Leaky abstraction: most computer algebra algorithms do require dispatching on concrete node types
  in an expression tree (do this if it's a sum, do that for numbers, ...), which breaks the
  encapsulation that the abstraction intended to provide in the first place.
- Suboptimal runtime performance: the memory layout usually works with nodes being individually
  heap-allocated and often reference counted. This makes it possible to conveniently have composite
  nodes store an arbitrary number of references/pointers to child nodes, but fails to account for
  spatial locality of objects belonging to a single expression. This leads to more cache misses than
  necessary. In addition, the numerous small-sized allocations for constructing expression trees can
  further slow down the algorithms (e.g. when using default general-purpose allocators).

This library tries to address both issues.

## Narrow scope and feature restriction

Design-wise, the system is _not_ extensible through subclasses that implement an abstract interface
for expressions. Instead, the expression node types are fixed (symbol, integer, rational number,
floating point number, complex number, sum, product, power, function). Again, this accounts for the
characteristics of most computer algebra algorithms, which require knowledge of concrete types. The
necessary information hiding to store arbitrary trees as expression objects is accomplished through
unions of builtin primitives (see below).

Multiprecision arithmetics is supported, but only for integers and rational numbers, not for
floating point numbers. Symbol names can only be 13 characters long (+ trailing zero). These
restrictions allow for an efficient memory layout.

## Basic types

The single, central _public_ API type is `sym2::Var`. It catches all leaf and composite expression
types, without exposing much of the tree structure. It doesn't even have many member functions - the
majority of features provided as free functions.

`sym2::Var` hides the internal representation for library clients using a non-allocating Pimpl
pattern. The internal data representation type is `sym2::Expr`. The main purpose of the `Expr` class
to own all its data. An `Expr` instance is immutable except assignment. It is also cheap to copy, given
a reasonable polymorphic allocator is provided (not really implemented at this point). The only
complex part of its implementation is the constructor overload set. Any query, access to operands
etc. will be handled through a separate view over the expression data called `sym2::ExprView`.

`sym2::ExprView` is a non-owning, immutable view or handle for the owning `Expr`, similar to what
`std::string_view` is for `std::string`. It is _the_ expression type to be passed around. It is only
16 bytes long and can hence be passed in CPU registers. Again, its class API is slim, and most
queries are implemented as free functions.

Note that `ExprView` is a template over a predicate expression. There is a set of common predicates
for expressions, such as `number`, `scalar`, `negative` etc. - these aren't ordinary functions, but
wrapped callable objects that can be `constexpr`-combined with boolean operators. Example: `number
&& negative || !scalar` is a predicate expression evaluated at compile time. It returns an expression
template that can be used to create custom queries such as `is<number && negative ||
!scalar>(someExpression)`, or to _tag_ `ExprView` instances. The latter is used to express pre- and
postconditions of functions, which can be understood as custom, poor implementation of contracts. As
an example, the function signature `void doImportantWork(ExprView<sum> u, ExprView<symbol ||
constant> v)` signals that the first argument must be a sum, while the second is expected to be
symbol or a constant. Conversions between `ExprView`s with different tags are implicit at this point
(re-tagging them manually turned out to be too annoying). This technique is used to document
requirements/assumptions and to catch bugs early. In release builds, all tags should compile to a
noop. In debug builds, tags are checked upon `ExprView` construction and invoke a violation handler
when they aren't satisfied. The current violation handler prints a stacktrace and triggers an abort
through a failing `assert`.

While the `Var` class provides all operator overloads users would expect (only `==` and `!=` for
comparison, as `<` and friends are ambiguous in computer algebra), all internal arithmetic and
comparison is typed out; this is to keep things simple and readable and not having to distinguish
between public and internal operators.

## Flat tree layout

The `Expr` type is the backbone of most runtime performance of this library. It shall allow for
custom polymorphic allocators (not implemented yet) to avoid numerous small allocations, and it
arranges the data layout of all expression trees. These trees are represented in a contiguous array
of a 16 byte `struct` called `Blob`. It consists of some small flags and tightly packed `union`s
that use only primitive builtin types. Critically, it is trivially copyable.
