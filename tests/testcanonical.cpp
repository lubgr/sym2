
#include "canonical.h"
#include "doctest/doctest.h"
#include "query.h"

using namespace sym2;

const Expr pi{"pi"};
const Expr a{"a"};
const Expr b{"b"};
const Expr c{"c"};
const Expr n{42};

TEST_CASE("Automatic power simplification")
{
    CHECK(power(a, 0) == 1);
    CHECK(power(0, a) == 0);
    CHECK(power(1, n) == 1);
    CHECK(power(a, 1) == a);
    CHECK(power(42, 1) == 42);
}
