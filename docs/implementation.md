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

## Basic types

The single, central _public_ API type is `sym2::Var`. It is a very simple type that represents all
expression types, without exposing much of the tree structure. It doesn't have many member functions
- the majority of features provided as free functions. `sym2::Var` hides the actual internal
  representation for library clients using a non-allocating Pimpl pattern.

The internal data representation type is `sym2::Expr`. The main purpose of the `Expr` class is to
own all its data. An `Expr` instance is immutable except assignment. It is also cheap to copy, given
a reasonable polymorphic allocator is provided. The only complex part of its implementation is the
constructor overload set.

Any query, access to operands etc. will be handled through a separate view over the expression data
called `sym2::ExprView`. `sym2::ExprView` is a non-owning, immutable view or handle for the owning
`Expr`, similar to what `std::string_view` is for `std::string`. It is _the_ expression type to be
passed around. It is only 16 bytes long, trivially copy/movable and can hence be passed in CPU
registers. Again, its class API is slim, and most queries are implemented as free functions.

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
custom polymorphic allocators to avoid numerous small allocations, and it arranges the data layout
of all expression trees. These trees are represented in a contiguous array of a 16 byte `struct`
called `Blob`. It consists of some small flags and tightly packed `union`s that use only primitive
builtin types. `Blob` sequences are trivially copyable, which is crucial for having performant value
semantics without synchronization cost or an artificial restriction to single-threaded processes.

The expression example from above, `42 + a + c^2*sin(b)`, will be laid out as follows (slightly
simplified, there are additional tags that we don't specify here, e.g. is a symbol positive, real or
complex domain etc.):
```
{ [+, 3/9] [42] ["a"] [*, 2/6] [^, 2] ["c"] [2] ["sin", 1/2] [&std::sin] ["b"] }

Blob  1: Sum tag, number of operands (3) and child blobs (9)
Blob  2: Small integer tag and the integer itself (42)
Blob  3: Symbol tag and the the symbol string data ("a")
Blob  4: Product tag, number of operands (2) and child blobs (6)
Blob  5: Power tag and number of child blobs (2)
Blob  6: Symbol tag and the symbol string ("c")
Blob  7: Small integer tag and integer itself (2)
Blob  8: Function tag (1) number of arguments (1) and child blobs (2)
Blob  9: Function tag (2), name and pointer to an evaluation function
Blob 10: Symbol tag and the symbol string ("b")
```
The whole expression is represented in 160 contiguous bytes. More importantly, there is no further
indirection: in order to transform the whole expression into a new one (e.g. compute a derivative
with respect to the symbol "b"), there is only one contiguous memory location to read. Similarly,
creating a new expression with the above one being an operand, e.g. `d*(42 + a + c^2*sin(b))` can be
done by one allocation and two trivial `std::memcpy` operations.

The flat buffer representation requires special care when it comes to large integers. Recall that
multiprecision integers are part of every computer algebra system, as several central algorithms
require them. However, we can't limit the size of these integers as we did for symbols. Instead, we
need to serialize the bytes that make up the "limbs" of the large integer object, and save some
metadata alongside (sign, number of bytes). These bytes are stored within an arbitrary number of
`Blob`s. Only when required, they are deserialized back into a multiprecision integer object that
can be used for arithmetics. This is a distinct performance pessimization - in total, it is expected
to pay off though, as this scheme preserves both flat-ness and trivial copyability. Critically,
multiprecision integers are a rare case in practice (for the use cases in mind when designing this
library), and small 4-byte integers are expected to be sufficient in 99% of the time.

Also, the variable length of composite operands makes it impossible to provide random access into a
sequence of `Blob`s. With the number of child blobs being recorded alongside a composite tag, we can
easily identify expression sub-parts though. And given that all composite parts are laid out in one
cache-friendly sequence should ensure sufficient speed when accessing into `Blob`s.
