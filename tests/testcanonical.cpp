
#include "canonical.h"
#include "doctest/doctest.h"
#include "query.h"

using namespace sym2;

TEST_CASE("Automatic power simplification")
{
    CHECK(power("a"_ex, 0) == 1_ex);
    CHECK(power(0, "a"_ex) == 0_ex);
    CHECK(power(1, 42_ex) == 1_ex);
    CHECK(power("a"_ex, 1) == "a"_ex);
    CHECK(power(42, 1) == 42_ex);
}
