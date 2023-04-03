#pragma once

#include <cstdint>
#include <initializer_list>
#include <span>
#include <string_view>
#include <vector>
#include "blob.h"
#include "compositetype.h"
#include "doublefctptr.h"
#include "exprview.h"
#include "largerational.h"
#include "allocator.h"
#include "sym2/domainflag.h"

namespace sym2 {
    class Expr {
      public:
        using allocator_type = LocalAlloc<>;

        explicit Expr(allocator_type allocator); // Equivalent to Expr(0, allocator);
        Expr(std::int16_t n, allocator_type allocator);
        // Always constructs a canonical rational number, and an integer if denom is one. Throws
        // std::invalid_argument if denom is zero.
        Expr(std::int16_t num, std::int16_t denom, allocator_type allocator);

        // Convenience overloads with 32bit ints, throw std::domain_error if the values don't fit
        // into 16bit:
        Expr(std::int32_t n, allocator_type allocator);
        Expr(std::int32_t num, std::int32_t denom, allocator_type allocator);

        Expr(double n, allocator_type allocator);
        // In case the value fits into a small int, results in a small integer:
        Expr(const LargeInt& n, allocator_type allocator);
        Expr(const LargeRational& n, allocator_type allocator);
        // Symbol constructors throw std::invalid_argument on empty symbol names:
        Expr(std::string_view symbol, allocator_type allocator);
        Expr(std::string_view symbol, DomainFlag domain, allocator_type allocator);
        // Constructs a constant with given numeric value. The name must be <= 8 bytes, but not
        // empty (throws std::invalid_argument otherwise). The value must be finite (throws
        // std::domain_error otherwise). Only constants in the real domain are supported.
        Expr(std::string_view constant, double value, allocator_type allocator);
        Expr(std::string_view function, ExprView<> arg, UnaryDoubleFctPtr eval,
          allocator_type allocator);
        Expr(std::string_view function, ExprView<> arg1, ExprView<> arg2, BinaryDoubleFctPtr eval,
          allocator_type allocator);
        Expr(ExprView<> e, allocator_type allocator);
        // Constructors for composites require exactly two arguments for powers, and exactly two
        // numeric, real-domain arguments for complex numbers. Otherwise, std::invalid_argument is
        // thrown.
        Expr(CompositeType composite, std::span<const Expr> ops, allocator_type allocator);
        Expr(CompositeType composite, std::span<const ExprView<>> ops, allocator_type allocator);
        // Convenience overload for composites with two operands:
        Expr(CompositeType composite, ExprView<> op1, ExprView<> op2, allocator_type allocator);

        Expr(const Expr& other, allocator_type allocator);
        Expr& operator=(const Expr& other) = default;
        Expr(Expr&& other, allocator_type allocator); // Can't be noexcept when allocators differ
        Expr& operator=(Expr&& other) = default; // Can't be noexcept when allocators differ
        ~Expr() = default;

        // Implicit conversions to any tag are indeed desired here:
        template <PredicateTag auto tag>
        operator ExprView<tag>() const
        {
            return ExprView<tag>{buffer.data()};
        }

      private:
        LocalVec<Blob> buffer;
    };

    template <std::size_t N>
    class SmallExpr {
      public:
        using allocator_type = LocalAlloc<>;

        explicit SmallExpr(allocator_type allocator)
            : storage{allocator.getBuffer()}
            , expression{&storage}
        {}

        template <class T>
        SmallExpr(T&& arg, allocator_type allocator)
            : storage{allocator.getBuffer()}
            , expression{std::forward<T>(arg), &storage}
        {}

        template <class T, class U>
        SmallExpr(T&& arg1, U&& arg2, allocator_type allocator)
            : storage{allocator.getBuffer()}
            , expression{std::forward<T>(arg1), std::forward<U>(arg2), &storage}
        {}

        template <class T, class U, class V>
        SmallExpr(T&& arg1, U&& arg2, V&& arg3, allocator_type allocator)
            : storage{allocator.getBuffer()}
            , expression{
                std::forward<T>(arg1), std::forward<U>(arg2), std::forward<V>(arg3), &storage}
        {}

        template <class T, class U, class V, class W>
        SmallExpr(T&& arg1, U&& arg2, V&& arg3, W&& arg4, allocator_type allocator)
            : storage{allocator.getBuffer()}
            , expression{std::forward<T>(arg1), std::forward<U>(arg2), std::forward<V>(arg3),
                std::forward<V>(arg4), &storage}
        {}

        template <PredicateTag auto tag>
        operator ExprView<tag>() const
        {
            return static_cast<ExprView<tag>>(expression);
        }

      private:
        StackBuffer<N * sizeof(Blob), alignof(Blob)> storage{};
        Expr expression;
    };

    template <std::size_t N>
    class FixedExpr {
      public:
        template <class... T>
        explicit FixedExpr(T&&... args)
            : storage{nullptr, false}
            , expression{std::forward<T>(args)..., LocalAlloc{&storage}}
        {}

        template <PredicateTag auto tag>
        operator ExprView<tag>() const
        {
            return static_cast<ExprView<tag>>(expression);
        }

      private:
        StackBuffer<N * sizeof(Blob), alignof(Blob)> storage;
        Expr expression;
    };

    FixedExpr<2> operator"" _ex(long double n);
    // The string literal must fit into a small symbol, throws std::bad_alloc otherwise.
    FixedExpr<1> operator"" _ex(const char* str, std::size_t length);
    // The argument must fit into a std::int16_t, throws std::domain_error otherwise.
    FixedExpr<1> operator"" _ex(unsigned long long n);
}
