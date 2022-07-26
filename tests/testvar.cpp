
#include <sstream>
#include "doctest/doctest.h"
#include "sym2/sym2.h"

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

    SUBCASE("Stream operator")
    {
        std::ostringstream os;

        SUBCASE("Symbol")
        {
            os << "a"_v;

            CHECK(os.str() == "a");
        }

        SUBCASE("Mixed expression")
        {
            const Var c = "a"_v + "a"_v + 2 * "b"_v + 2 * "a"_v / 3;
            const Var d = 3 * "b"_v * "b"_v * c + 2 * "a"_v - 4 * "b"_v + sym2::cos("a"_v);

            os << d;

            CHECK(os.str() == "2*a - 4*b + 3*b^2*(8*a/3 + 2*b) + cos(a)");
        }

        SUBCASE("No spaces")
        {
            os << "a"_v << 42_v << 3.14_v;

            CHECK(os.str() == "a423.14");
        }
    }
}
