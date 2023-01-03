#pragma once

#include <array>
#include <cstddef>

namespace sym2 {
    struct alignas(double) Blob {
        std::array<std::byte, 8> bytes;
    };
}
