#pragma once

#include <cassert>
#include <cstdio>
#include "predicateexpr.h"

namespace sym2 {
    template <PredicateTag auto tag>
    class ExprView;

    namespace detail {
        void printStacktrace(std::FILE* fp);
    }

    template <auto tag>
    void assertTag([[maybe_unused]] ExprView<tag> e)
    {
        if constexpr (!std::is_same_v<decltype(tag), AnyFlag>)
            assert(is<tag>(e));
    }

    template <auto tag>
    void assertTagWithStacktrace(ExprView<tag> e)
    {
        if constexpr (!std::is_same_v<decltype(tag), AnyFlag>)
            if (!is<tag>(e)) {
                detail::printStacktrace(stderr);
                assert(is<tag>(e));
            }
    }
}
