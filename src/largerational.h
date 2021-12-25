#pragma once

#include "largeint.h"

namespace sym2 {
    using LargeRational =
      boost::multiprecision::number<boost::multiprecision::rational_adaptor<typename LargeInt::backend_type>>;
}
