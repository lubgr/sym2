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

Multiprecision arithmetics are supported, but only for integers and rational numbers, not for
floating point numbers. Symbol names can only be 13 characters long (+ trailing zero). These
restrictions allow for an efficient memory layout.

## Flat tree layout


# Scripting interface for testing
