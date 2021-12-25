#pragma once

#include <array>
#include <cstddef>
#include <memory_resource>

namespace sym2 {
    enum class ByteSize : std::size_t {};
    template <ByteSize N>
    auto monotonicStackPmrResource()
    {
        constexpr std::size_t bufferSize = static_cast<std::underlying_type_t<ByteSize>>(N);
        struct DataAndAllocator {
            std::array<std::byte, bufferSize> data;
            std::pmr::monotonic_buffer_resource buffer{data.data(), data.size()};
        };

        return DataAndAllocator{};
    }
}
