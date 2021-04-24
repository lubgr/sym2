
#include "autosimpl.h"
#include "doctest/doctest.h"
#include "power.h"

using namespace sym2;

TEST_CASE("Simple power cases")
{
    CHECK(autoPower("a"_ex, 0_ex) == 1_ex);
    CHECK(autoPower(0_ex, "a"_ex) == 0_ex);
    CHECK(autoPower(1_ex, 42_ex) == 1_ex);
    CHECK(autoPower("a"_ex, 1_ex) == "a"_ex);
    CHECK(autoPower(42_ex, 1_ex) == 42_ex);
}
