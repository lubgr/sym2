
#include "abc.h"
#include "canonical.h"
#include "doctest/doctest.h"
#include "query.h"

using namespace sym2;

TEST_CASE("Automatic power simplification")
{
    CHECK(power(a, 0) == 1_ex);
    CHECK(power(0, a) == 0_ex);
    CHECK(power(1, 42_ex) == 1_ex);
    CHECK(power(a, 1) == a);
    CHECK(power(42, 1) == 42_ex);
}
