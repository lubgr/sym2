#pragma once

#include <cstdint>
#include <iosfwd>
#include <string_view>
#include <type_traits>
#include "symbolflag.h"

namespace sym2 {
    class Expr; /* Only defined internally. */

    class Var {
      public:
        Var();
        Var(std::int32_t n);
        Var(double n);
        Var(std::int32_t num, std::int32_t denom);
        explicit Var(std::string_view symbol);
        Var(std::string_view symbol, SymbolFlag constraint);
        Var(std::string_view constant, double value);
        Var(std::string_view function, const Var& arg, double (*eval)(double));
        Var(std::string_view function, const Var& arg1, const Var& arg2,
          double (*eval)(double, double));
        Var(const Var& other);
        Var& operator=(const Var& other);
        Var(Var&& other) noexcept;
        Var& operator=(Var&& other) noexcept;
        ~Var() noexcept;

        Var& operator+=(const Var& rhs);
        Var& operator-=(const Var& rhs);
        Var& operator*=(const Var& rhs);
        Var& operator/=(const Var& rhs);

        const Var& operator+() const;
        Var operator-() const;

      public:
        /* To be used internally only: */
        static Var internal(const Expr& backend);
        Expr& get();
        const Expr& get() const;

      private:
        template <class... Args>
        void initialize(Args&&... ctorArgs);

        static constexpr std::size_t pimplSize = 32;
        static constexpr std::size_t pimplAlignment = 8;

        std::aligned_storage_t<pimplSize, pimplAlignment> buffer;
    };

    namespace literals {
        Var operator"" _v(const char* str, std::size_t);
        Var operator"" _v(unsigned long long n);
        Var operator"" _v(long double n);
    }

    bool operator==(const Var& lhs, const Var& rhs);
    bool operator!=(const Var& lhs, const Var& rhs);

    Var operator+(Var lhs, const Var& rhs);
    Var operator-(Var lhs, const Var& rhs);
    Var operator*(Var lhs, const Var& rhs);
    Var operator/(Var lhs, const Var& rhs);

    std::ostream& operator<<(std::ostream& os, const Var& rhs);
}
