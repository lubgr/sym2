#pragma once

#include "int.h"

namespace sym2 {
    using Rational = boost::multiprecision::number<
      boost::multiprecision::rational_adaptor<boost::multiprecision::cpp_int_backend<integerBitBufferSize>>>;
}
