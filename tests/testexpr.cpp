
#include <algorithm>
#include <cmath>
#include <string_view>
#include "doctest/doctest.h"
#include "expr.h"
#include "query.h"
#include "testutils.h"

using namespace sym2;
using namespace std::literals::string_view_literals;

bool numEvalAndSize1(ExprView e)
{
    return e.size() == 1 && e[0].flags == Flag::numericallyEvaluable;
}

bool isSmallIntEqualTo(ExprView e, int num)
{
    return e[0].header == Type::smallInt && numEvalAndSize1(e) && e[0].main.exact.num == num
      && e[0].main.exact.denom == 1;
}

bool isSmallRationalEqualTo(ExprView e, int num, int denom)
{
    return e[0].header == Type::smallRational && numEvalAndSize1(e) && e[0].main.exact.num == num
      && e[0].main.exact.denom == denom;
}

bool hasLargeIntCharacteristics(ExprView e)
{
    return e[0].header == Type::largeInt && static_cast<std::size_t>(e.size()) == e[0].main.nChildBlobs + 1
      && e[0].flags == Flag::numericallyEvaluable;
}

bool hasLargeRationalCharacteristics(ExprView e)
{
    return e[0].header == Type::largeRational && e[0].flags == Flag::numericallyEvaluable;
}

template <std::size_t N>
bool allNullChars(const char (&str)[N])
{
    return std::find_if(str, std::cend(str), [](char c) { return c != '\0'; }) == std::cend(str);
}

bool isShortSymbol(Blob what, std::string_view expectedName)
{
    return what.header == Type::symbol && what.flags == Flag::none && what.pre.name == expectedName
      && allNullChars(what.main.name);
}

TEST_CASE("Expr constructor")
{
    SUBCASE("Zero")
    {
        CHECK(isSmallIntEqualTo(0_ex, 0));
    }

    SUBCASE("Small int")
    {
        CHECK(isSmallIntEqualTo(42_ex, 42));
        CHECK(isSmallIntEqualTo(Expr{-42}, -42));
    }

    SUBCASE("Minus one")
    {
        CHECK(isSmallIntEqualTo(Expr{-1}, -1));
    }

    SUBCASE("Small rational")
    {
        CHECK(isSmallRationalEqualTo(Expr{2, 3}, 2, 3));
        CHECK(isSmallRationalEqualTo(Expr{-2, 3}, -2, 3));
    }

    SUBCASE("Small rational normalization")
    {
        CHECK(isSmallRationalEqualTo(Expr{9, 6}, 3, 2));
    }

    SUBCASE("Small rational throws if denom == 0")
    {
        CHECK_THROWS(Expr{42, 0});
    }

    SUBCASE("Small rational negative denom")
    {
        CHECK(isSmallRationalEqualTo(Expr{2, -3}, -2, 3));
    }

    SUBCASE("Small rational to small int")
    {
        CHECK(Expr{10, 5} == 2_ex);
        CHECK(Expr{10, -5} == Expr{-2});
    }

    SUBCASE("Large int")
    {
        const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};

        SUBCASE("> 0")
        {
            CHECK(hasLargeIntCharacteristics(Expr{largeInt}));
        }

        SUBCASE("< 0")
        {
            CHECK(hasLargeIntCharacteristics(Expr{-largeInt}));
        }
    }

    SUBCASE("Large int to small int")
    {
        const LargeInt fits{"12345"};

        CHECK(isSmallIntEqualTo(Expr{fits}, 12345));
        CHECK(isSmallIntEqualTo(Expr{-fits}, -12345));
    }

    SUBCASE("Large zero to small zero")
    {
        const LargeInt zero{0};

        CHECK(isSmallIntEqualTo(Expr{zero}, 0));
    }

    SUBCASE("Large to small rational")
    {
        const LargeRational fits{17, 31};

        CHECK(isSmallRationalEqualTo(Expr{fits}, 17, 31));
    }

    SUBCASE("Large rational")
    {
        const LargeInt large{"283749237498273489274382709084938593857982374982729873"};
        const LargeRational lr{LargeInt{1}, large};

        SUBCASE("> 0")
        {
            const Expr e{lr};

            CHECK(hasLargeRationalCharacteristics(e));
            CHECK(isSmallIntEqualTo(first(e), 1));
            CHECK(hasLargeIntCharacteristics(second(e)));
        }

        SUBCASE("< 0")
        {
            const Expr e{-lr};

            CHECK(hasLargeRationalCharacteristics(e));
            CHECK(isSmallIntEqualTo(first(e), -1));
            CHECK(hasLargeIntCharacteristics(second(e)));
        }

        SUBCASE("Num/denom saved as small int if possible")
        {
            const Expr e{lr};
            auto v = view(e);

            CHECK(v[1].header == Type::smallInt);
            CHECK(v[2].header == Type::largeInt);
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
        CHECK(e[0].pre.name == std::string_view{"abcdef_{gh}^i"});
    }

    SUBCASE("Constant creation")
    {
        const double value = -1.234;
        const Expr c{"test", value};
        auto e = view(c);

        CHECK(e.size() == 1);

        CHECK(e[0].header == Type::constant);
        CHECK(e[0].flags == Flag::numericallyEvaluable);
        CHECK(e[0].pre.name == "test"sv);
        CHECK(e[0].main.inexact == doctest::Approx(value));
    }

    SUBCASE("Unary function creation")
    {
        const Expr sinA{"sin", "a"_ex, std::sin};
        auto e = view(sinA);

        CHECK(e.size() == 3);

        CHECK(e[0].header == Type::function);
        CHECK(e[0].flags == Flag::none);
        CHECK(allNullChars(e[0].pre.name));
        CHECK(e[0].mid.nLogicalOperands == 1);
        CHECK(e[0].main.nChildBlobs == 2);

        CHECK(e[1].pre.name == "sin"sv);
        CHECK(e[1].main.unaryEval == static_cast<UnaryDoubleFctPtr>(std::sin));

        CHECK(isShortSymbol(e[2], "a"));
    }

    SUBCASE("Binary function creation")
    {
        const Expr atan2ab{"atan2", "a"_ex, "b"_ex, std::atan2};
        auto e = view(atan2ab);

        CHECK(e.size() == 4);

        CHECK(e[0].header == Type::function);
        CHECK(e[0].flags == Flag::none);
        CHECK(allNullChars(e[0].pre.name));
        CHECK(e[0].mid.nLogicalOperands == 2);
        CHECK(e[0].main.nChildBlobs == 3);

        CHECK(e[1].pre.name == "atan2"sv);
        CHECK(e[1].main.binaryEval == static_cast<BinaryDoubleFctPtr>(std::atan2));

        CHECK(isShortSymbol(e[2], "a"));
        CHECK(isShortSymbol(e[3], "b"));
    }

    SUBCASE("Complex number creation")
    {
        const auto two = 2_ex;
        const Expr frac{3, 7};
        const std::vector<ExprView> args{two, frac};
        const Expr c{Type::complexNumber, args};
        auto e = view(c);

        CHECK(e[0].header == Type::complexNumber);
        CHECK(e[0].flags == Flag::numericallyEvaluable);
        CHECK(allNullChars(e[0].pre.name));
        CHECK(e[0].mid.nLogicalOperands == 2);
        CHECK(e[0].main.nChildBlobs == 2);
    }
}
