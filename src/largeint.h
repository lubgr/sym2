#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>
#include <limits>
#include "allocator.h"

namespace sym2 {
    // Note that I have tried to use pmr allocators together with cpp_int_backend. This didn't work,
    // though: unfortunately, the number and cpp_int_backend templates etc. in Boost multiprecision
    // are written for the pre-C++11 allocator model, which allows for allocators with global state
    // only. With this model, allocators of the same type are assumed to be identical, and different
    // instances can share responsibility for allocation and deallocation. This is incompatible with
    // the stateful pmrs.
    //
    // Instead of custom allocators, we choose a rather high small object optimization buffer to
    // make allocations appear for very large integers only. This fits into the tendency to keep the
    // runtime optimal for usual expressions and pessimizing otherwise.
    using LargeInt = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<256>>;

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
