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
variation of it. Here, a base class commits to an interface for all concrete expression subclasses
that implement behaviour according to their needs, and composite subclasses store a sequence of
references to children, again through the base class interface. This design has benefits, but there
are shortcomings with such abstraction, too:

- Most computer algebra algorithms do require dispatching on concrete node types in an expression
  tree (do this if it's a sum, do that for numbers, ...). This happens in a way that's too
  algorithm-specific to be implemented by a clever virtual interface. Hence, a mixture of downcasts
  or type ids will creep in, breaking the encapsulation promise that the abstraction intended to
  provide.
- The memory layout for such design usually works with nodes being individually heap-allocated and
  reference counted. This makes it hard to use (thread-)local allocation schemes, since it's not
  clear which object lifetimes end when a library operation finishes. Using default allocations for
  expression trees comes with synchronisation costs and can lead to memory fragmentation and
  suboptimal cache locality.

This library tries to address both issues.

## Basic types

The central API types are `sym2::Expr` and `sym2::ExprView`. The former owns an expression, the
latter is a lightweight view to it (think of `std::string` and `std::string_view`). In addition,
there are `sym2::SmallExpr<N>` and `sym2::FixedExpr<N>`. In terms of their storage characteristics,
they are equivalent to `small_vector` and a fixed-size array, respectively. Otherwise, they are
identical to `sym2::Expr`.

All these types are immutable except assignment. They also don't have many member functions - the
majority of features are provided as free functions. All three owning expression types implicitly
convert to a `sym2::ExprView`, and functions that don't need ownership typically work with views. A
`sym2::ExprView` is a single pointer under the hood, trivially copy/movable and can hence be passed
in CPU registers.

Note that `ExprView` is not a class type, but a template over a predicate expression. There is a set
of common predicates for expressions, such as `number`, `scalar`, `negative` etc. - these aren't
ordinary functions, but wrapped callable objects that can be `constexpr`-combined with boolean
operators. Example: `number && negative || !scalar` is a predicate expression evaluated at compile
time. It returns an expression template that can be used to create custom queries such as `is<number
&& negative || !scalar>(someExpression)`, or to tag `ExprView`s. The latter is used to express pre-
and postconditions of functions, which can be understood as custom, poor implementation of
contracts. As an example, the function signature `void doImportantWork(ExprView<sum> u,
ExprView<symbol || constant> v)` signals that the first argument must be a sum, while the second is
expected to be symbol or a constant. Conversions between `ExprView`s with different tags are
implicit for convenience (re-tagging them manually turned out to be too annoying). This technique is
used to document requirements/assumptions and to catch bugs early. In release builds, all tags
compile to a noop. In debug builds, tags are checked upon `ExprView` construction and invoke a
violation handler when they aren't satisfied. The current violation handler prints a stacktrace and
triggers an abort through a failing `assert`.

## Flat tree layout

The `Expr` type provides most of the runtime performance of this library. It allows for local
unsynchronised stack storage to avoid numerous small allocations, and it arranges the data layout of
all expression trees. The trees are represented in a contiguous array of a 8 byte opaque `Blob`s.
The actual definition of these data blobs is hidden and used through an internal API. Under the
hood, a sequence of `Blob`s consists of small flags and tightly packed `union`s that use only
primitive builtin types, such that a `Blob` is trivially copyable. The latter is crucial for
performant value semantics without synchronization overhead or an artificial restriction
single-threaded use cases.
