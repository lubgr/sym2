#pragma once

#include <boost/container/small_vector.hpp>

namespace sym2 {
    template <class T, std::size_t N>
    using SmallVec = boost::container::small_vector<T, N>;

    template <class T>
    using SmallVecBase = boost::container::small_vector_base<T>;
}
