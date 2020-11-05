#pragma once

#include <boost/operators.hpp>
#include <boost/rational.hpp>
#include <optional>
#include <variant>
#include "int.h"

namespace sym2 {
    class Number : private boost::totally_ordered<Number, boost::arithmetic<Number>> {
      public:
        Number() = default;
        Number(int value);
        Number(double value);
        Number(int numerator, int denominator);
        explicit Number(const Int& value);
        Number(const Int& numerator, const Int& denominator);

        Number& operator+=(const Number& rhs);
        Number& operator-=(const Number& rhs);
        Number& operator*=(const Number& rhs);
        Number& operator/=(const Number& rhs);
        const Number& operator+() const;
        Number operator-() const;

        bool isRational() const;
        bool isDouble() const;
        Int numerator() const;
        Int denominator() const;
        double toDouble() const;

      private:
        using Rational = boost::rational<Int>;
        using Rep = std::variant<Rational, double>;

        template <class Operation> struct Operate {
            template <class T> Rep operator()(const T& lhs, const T& rhs)
            {
                return Operation{}(lhs, rhs);
            }

            Rep operator()(const Rational& lhs, double rhs)
            {
                return Operation{}(boost::rational_cast<double>(lhs), rhs);
            }

            Rep operator()(double lhs, const Rational& rhs)
            {
                return Operation{}(lhs, boost::rational_cast<double>(rhs));
            }
        };

        Rep rep{Rational{0}};
    };

    bool operator==(const Number& lhs, const Number& rhs);
    bool operator<(const Number& lhs, const Number& rhs);
}

namespace std {
    template <> struct hash<sym2::Number> {
        size_t operator()(const sym2::Number& n) const;
    };
}
