#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>
#include <vector>

namespace sym2 {
    class Expr; /* Only defined internally. */

    class Var {
      public:
        Var();
        Var(std::int32_t n);
        Var(double n);
        Var(std::int32_t num, std::int32_t denom);
        explicit Var(std::string_view symbol);
        Var(std::string_view constant, double value);
        Var(std::string_view function, const Var& arg, double (*eval)(double));
        Var(std::string_view function, const Var& arg1, const Var& arg2, double (*eval)(double, double));
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

        static constexpr std::size_t pimplSize = 184;
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

    Var sqrt(const Var& base);
    Var pow(const Var& base, const Var& exp);
    Var log(const Var& arg);
    Var sin(const Var& arg);
    Var cos(const Var& arg);
    Var tan(const Var& arg);
    Var asin(const Var& arg);
    Var acos(const Var& arg);
    Var atan(const Var& arg);
    Var atan2(const Var& y, const Var& x);

    Var subst(const Var& arg, const Var& from, const Var& to);
    Var expand(const Var& arg);
    Var normal(const Var& arg);
    Var simplify(const Var& arg);
    bool isPositive(const Var& arg);
    bool isNegative(const Var& arg);
    unsigned complexity(const Var& arg);
    Var numerator(const Var& arg);
    Var denominator(const Var& arg);
    /* Returns names for symbols, functions and constants, an empty string otherwise: */
    std::string_view name(const Var& arg);
    std::vector<Var> operands(const Var& arg);
    std::vector<Var> collectSymbols(const Var& arg);
}
