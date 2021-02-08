
#include <boost/range/algorithm.hpp>
#include "autosimpl.h"
#include "doctest/doctest.h"
#include "testutils.h"
#include "view.h"

using namespace sym2;

TEST_CASE("Basic ExprView behavior")
{
    const auto s = sum("a"_ex, "b"_ex);
    const auto sv = view(s);

    SUBCASE("Size")
    {
        CHECK(sv.size() == 3);
    }

    SUBCASE("Copy and compare")
    {
        std::vector<Blob> dest;

        boost::copy(sv, std::back_inserter(dest));

        boost::equal(
          sv, dest, [](const Blob& lhs, const Blob& rhs) { return std::memcmp(&lhs, &rhs, sizeof(Blob)) == 0; });
    }
}
