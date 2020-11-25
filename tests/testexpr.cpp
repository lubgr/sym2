
#include "doctest/doctest.h"
#include "expr.h"
#include "testutils.h"

using namespace sym2;
using namespace std::string_literals;

bool numEvalAndSize1(ExprView e)
{
    return e.size() == 1 && e[0].flags == Flag::numericallyEvaluable;
}

bool isSmallInt(ExprView e, int num, Sign expected)
{
    return e[0].header == Type::smallInt && e[0].sign == expected && numEvalAndSize1(e) && e[0].data.exact.num == num
      && e[0].data.exact.denom == 1;
}

bool isSmallRational(ExprView e, int num, int denom, Sign expected)
{
    return e[0].header == Type::smallRational && e[0].sign == expected && numEvalAndSize1(e)
      && e[0].data.exact.num == num && e[0].data.exact.denom == denom;
}

bool isLargeInt(ExprView e, Sign expected)
{
    return e[0].header == Type::largeInt && e[0].sign == expected && e.size() == e[0].data.count + 1
      && e[0].flags == Flag::numericallyEvaluable;
}

bool isLargeRational(ExprView e, Sign expected)
{
    return e[0].header == Type::largeRational && e[0].sign == expected && e[0].flags == Flag::numericallyEvaluable;
}

TEST_CASE("Expr constructor")
{
    SUBCASE("Zero")
    {
        CHECK(isSmallInt(0_ex, 0, Sign::neither));
    }

    SUBCASE("Small int")
    {
        CHECK(isSmallInt(42_ex, 42, Sign::positive));
        CHECK(isSmallInt(Expr{-42}, -42, Sign::negative));
    }

    SUBCASE("Small rational")
    {
        CHECK(isSmallRational(Expr{2, 3}, 2, 3, Sign::positive));
        CHECK(isSmallRational(Expr{-2, 3}, -2, 3, Sign::negative));
    }

    SUBCASE("Small rational normalization")
    {
        CHECK(isSmallRational(Expr{9, 6}, 3, 2, Sign::positive));
    }

    SUBCASE("Small rational throws if denom == 0")
    {
        CHECK_THROWS(Expr{42, 0});
    }

    SUBCASE("Small rational negative denom")
    {
        CHECK(isSmallRational(Expr{2, -3}, -2, 3, Sign::negative));
    }

    SUBCASE("Small rational to small int")
    {
        CHECK(Expr{10, 5} == 2_ex);
        CHECK(Expr{10, -5} == Expr{-2});
    }

    SUBCASE("Large int")
    {
        const Int largeInt{"2323498273984729837498234029380492839489234902384"};

        SUBCASE("> 0")
        {
            CHECK(isLargeInt(Expr{largeInt}, Sign::positive));
        }

        SUBCASE("< 0")
        {
            CHECK(isLargeInt(Expr{-largeInt}, Sign::negative));
        }
    }

    SUBCASE("Large int to small int")
    {
        const Int fits{"12345"};

        CHECK(isSmallInt(Expr{fits}, 12345, Sign::positive));
        CHECK(isSmallInt(Expr{-fits}, -12345, Sign::negative));
    }

    SUBCASE("Large zero to small zero")
    {
        const Int zero{0};

        CHECK(isSmallInt(Expr{zero}, 0, Sign::neither));
    }

    SUBCASE("Large to small rational")
    {
        const Rational fits{17, 31};

        CHECK(isSmallRational(Expr{fits}, 17, 31, Sign::positive));
    }

    SUBCASE("Large rational")
    {
        const Rational lr{1, Int{"283749237498273489274382709084938593857982374982729874"}};

        SUBCASE("> 0")
        {
            CHECK(isLargeRational(Expr{lr}, Sign::positive));
        }

        SUBCASE("< 0")
        {
            CHECK(isLargeRational(Expr{-lr}, Sign::negative));
        }
    }

    SUBCASE("Too long symbol name throws")
    {
        CHECK_THROWS(Expr{"12345678901234"});
    }

    SUBCASE("Empty symbol name throws")
    {
        CHECK_THROWS(Expr{""});
    }

    SUBCASE("Successfull symbol creation")
    {
        const Expr symbol{"abcdef_{gh}^i"};
        auto e = view(symbol);

        CHECK(e.size() == 1);
        CHECK(e[0].flags == Flag::none);
        CHECK(e[0].sign == Sign::neither);

        CHECK(e[0].name == std::string_view{"abcdef_{gh}^i"});
    }

    SUBCASE("Symbol starting with + is positive")
    {
        const Expr symbol{"+a"};
        auto e = view(symbol);

        CHECK(e[0].sign == Sign::positive);
    }
}
