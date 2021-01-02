
#include "blob.h"
#include <type_traits>

static_assert(sizeof(sym2::Blob) == 16);
static_assert(std::is_trivial_v<sym2::Blob>);
