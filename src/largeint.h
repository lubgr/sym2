#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>
#include <limits>
#include <memory_resource>

namespace sym2 {
    namespace detail {
        /* A somewhat hacky and even unsafe (no virtual or protected base class destructor) workaround for using the pmr
         * allocator together with cpp_int from Boost multiprecision. The issue this fixes is that cpp_int_backend
         * requires the allocator to be copy assignable, which std::pmr::polymorphic_allocator doesn't satisfy. */
        template <class T>
        class AssignablePmrAllocator : public std::pmr::polymorphic_allocator<T> {
          public:
            using std::pmr::polymorphic_allocator<T>::polymorphic_allocator;

            AssignablePmrAllocator(const AssignablePmrAllocator&) = default;
            AssignablePmrAllocator& operator=(const AssignablePmrAllocator&)
            {
                static_assert(!std::allocator_traits<
                              std::pmr::polymorphic_allocator<int>>::propagate_on_container_move_assignment::value);

                // Just keep the current allocator. Most of the time, the allocators of this and the other instance
                // should be indentical anyhow (even more, most of the time we shouldn't copy-assign large integers).
                return *this;
            }
        };
    }

    using LargeInt = boost::multiprecision::number<
      boost::multiprecision::cpp_int_backend<128, 0, boost::multiprecision::signed_magnitude,
        boost::multiprecision::checked, detail::AssignablePmrAllocator<boost::multiprecision::limb_type>>>;

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
