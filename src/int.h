#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>
#include <limits>

namespace sym2 {
    constexpr inline unsigned integerBitBufferSize = 128;

    using Int = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<integerBitBufferSize>>;

    using boost::multiprecision::abs;
    using boost::multiprecision::gcd;
    using boost::multiprecision::lcm;
    using boost::multiprecision::pow;

    template <class IntegralType> bool fitsInto(const Int& i)
    {
        static const Int upperLimit(std::numeric_limits<IntegralType>::max());
        static const Int lowerLimit(std::numeric_limits<IntegralType>::min());

        return i <= upperLimit && i >= lowerLimit;
    }
}
