#pragma once

#include <cassert>
#include <cstdint>
#include <numeric>

namespace sym2 {
    struct SmallRational {
        std::int16_t num;
        std::int16_t denom;
    };
}
