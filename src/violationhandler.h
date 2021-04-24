#pragma once

#include <cassert>
#include "predicateexprsat.h"

namespace sym2 {
    template <PredicateTag auto tag>
    class ExprView;

    template <auto tag>
    void enforceTag(ExprView<tag> e)
    {
        if constexpr (!std::is_same_v<decltype(tag), AnyFlag>)
            assert(is<tag>(e));
    }
}
