#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>
#include <limits>
#include "allocator.h"

namespace sym2 {
    using LargeInt = boost::multiprecision::number<
      boost::multiprecision::cpp_int_backend<128, 0, boost::multiprecision::signed_magnitude,
        boost::multiprecision::unchecked, LargeIntAllocator<boost::multiprecision::limb_type>>>;

    using boost::multiprecision::abs;
    using boost::multiprecision::gcd;
    using boost::multiprecision::lcm;
    using boost::multiprecision::pow;

    template <class IntegralType>
    bool fitsInto(const LargeInt& i)
    {
        static const LargeInt upperLimit(std::numeric_limits<IntegralType>::max());
        static const LargeInt lowerLimit(std::numeric_limits<IntegralType>::min());

        return i <= upperLimit && i >= lowerLimit;
    }
}
