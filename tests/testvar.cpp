
#include "doctest/doctest.h"
#include "sym2.h"

using namespace sym2;
using namespace sym2::literals;

TEST_CASE("Basic usage")
{
    const Var n{42};

    SUBCASE("Copy constructor and equality")
    {
        const Var copy{n};

        CHECK(copy == n);
    }

    SUBCASE("Unary plus and minus")
    {
        CHECK(n == +n);
        CHECK(-n == -42);
    }

    SUBCASE("UDL")
    {
        CHECK(42_v == n);
        CHECK("a"_v == Var{"a"});
        CHECK(3.14_v == Var{3.14});
    }
}
