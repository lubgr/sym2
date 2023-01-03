#pragma once

#include <cstdint>

namespace sym2 {
    // No assumption is equivalent to assuming the complex domain. This flag never applies to
    // numbers, since their domain is defined by their values and types.
    enum class DomainFlag : std::uint8_t { none = 0b00, positive = 0b01, real = 0b10 };
}
