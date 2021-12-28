
#include "allocator.h"
#include <boost/container/small_vector.hpp>
#include <cassert>
#include <stack>

namespace sym2 {
    thread_local static std::stack<std::pmr::memory_resource*,
      boost::container::small_vector<std::pmr::memory_resource*, 10>>
      currentBuffer;
}

std::pmr::memory_resource* sym2::detail::currentOrDefaultResource()
{
    return currentBuffer.empty() ? std::pmr::get_default_resource() : currentBuffer.top();
}

sym2::LargeIntResourceRegistrar::LargeIntResourceRegistrar(std::pmr::memory_resource* resource)
    : resource{resource}
{
    assert(resource != nullptr);

    currentBuffer.push(resource);
}

sym2::LargeIntResourceRegistrar::~LargeIntResourceRegistrar()
{
    assert(!currentBuffer.empty());
    assert(currentBuffer.top() == resource);

    currentBuffer.pop();
}
