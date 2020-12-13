
#include "constants.h"
#include "doctest/doctest.h"
#include "expr.h"
#include "testutils.h"

using namespace sym2;
using namespace std::string_literals;
using namespace std::literals::string_view_literals;

bool numEvalAndSize1(ExprView e)
{
    return e.size() == 1 && e[0].flags == Flag::numericallyEvaluable;
}

bool isSmallInt(ExprView e, int num)
{
    return e[0].header == Type::smallInt && numEvalAndSize1(e) && e[0].main.exact.num == num
      && e[0].main.exact.denom == 1;
}

bool isSmallRational(ExprView e, int num, int denom)
{
    return e[0].header == Type::smallRational && numEvalAndSize1(e) && e[0].main.exact.num == num
      && e[0].main.exact.denom == denom;
}

bool isLargeInt(ExprView e)
{
    return e[0].header == Type::largeInt && e.size() == e[0].main.count + 1 && e[0].flags == Flag::numericallyEvaluable;
}

bool isLargeRational(ExprView e)
{
    return e[0].header == Type::largeRational && e[0].flags == Flag::numericallyEvaluable;
}

template <std::size_t N>
bool allNullChars(const char (&str)[N])
{
    return std::find_if(str, std::cend(str), [](char c) { return c != '\0'; }) == std::cend(str);
}

bool isShortSymbol(Operand op, std::string_view expectedName)
{
    return op.header == Type::symbol && op.flags == Flag::none && op.pre.name == expectedName
      && allNullChars(op.main.name);
}

TEST_CASE("Expr constructor")
{
    SUBCASE("Zero")
    {
        CHECK(isSmallInt(0_ex, 0));
    }

    SUBCASE("Small int")
    {
        CHECK(isSmallInt(42_ex, 42));
        CHECK(isSmallInt(Expr{-42}, -42));
    }

    SUBCASE("Small rational")
    {
        CHECK(isSmallRational(Expr{2, 3}, 2, 3));
        CHECK(isSmallRational(Expr{-2, 3}, -2, 3));
    }

    SUBCASE("Small rational normalization")
    {
        CHECK(isSmallRational(Expr{9, 6}, 3, 2));
    }

    SUBCASE("Small rational throws if denom == 0")
    {
        CHECK_THROWS(Expr{42, 0});
    }

    SUBCASE("Small rational negative denom")
    {
        CHECK(isSmallRational(Expr{2, -3}, -2, 3));
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
            CHECK(isLargeInt(Expr{largeInt}));
        }

        SUBCASE("< 0")
        {
            CHECK(isLargeInt(Expr{-largeInt}));
        }
    }

    SUBCASE("Large int to small int")
    {
        const Int fits{"12345"};

        CHECK(isSmallInt(Expr{fits}, 12345));
        CHECK(isSmallInt(Expr{-fits}, -12345));
    }

    SUBCASE("Large zero to small zero")
    {
        const Int zero{0};

        CHECK(isSmallInt(Expr{zero}, 0));
    }

    SUBCASE("Large to small rational")
    {
        const Rational fits{17, 31};

        CHECK(isSmallRational(Expr{fits}, 17, 31));
    }

    SUBCASE("Large rational")
    {
        const Int large{"283749237498273489274382709084938593857982374982729873"};
        const Rational lr{Int{1}, large};

        SUBCASE("> 0")
        {
            CHECK(isLargeRational(Expr{lr}));
        }

        SUBCASE("< 0")
        {
            CHECK(isLargeRational(Expr{-lr}));
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

        CHECK(e.size() == 2);

        CHECK(e[0].header == Type::unaryFunction);
        CHECK(e[0].flags == Flag::none);
        CHECK(e[0].pre.name == "sin"sv);
        CHECK(e[0].main.unaryEval == static_cast<UnaryDoubleFctPtr>(std::sin));

        CHECK(isShortSymbol(e[1], "a"));
    }

    SUBCASE("Binary function creation")
    {
        const Expr atan2ab{"atan2", "a"_ex, "b"_ex, std::atan2};
        auto e = view(atan2ab);

        CHECK(e.size() == 3);

        CHECK(e[0].header == Type::binaryFunction);
        CHECK(e[0].flags == Flag::none);
        CHECK(e[0].pre.name == "atan2"sv);
        CHECK(e[0].main.binaryEval == static_cast<BinaryDoubleFctPtr>(std::atan2));

        CHECK(isShortSymbol(e[1], "a"));
        CHECK(isShortSymbol(e[2], "b"));
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
        CHECK(e[0].main.count == 2);
    }
}

TEST_CASE("Scalar retrieval")
{
    const Expr si{42};
    const Expr sr{7, 11};
    const Int largeInt{"8233298749837489247029730960165010709217309487209740928934928"};
    const Expr li{largeInt};

    SUBCASE("Get small integer")
    {
        CHECK(get<int>(si) == 42);
    }

    SUBCASE("Get small rational from small integer")
    {
        CHECK(get<SmallRational>(si).num == 42);
        CHECK(get<SmallRational>(si).denom == 1);
    }

    SUBCASE("Get small rational")
    {
        CHECK(get<SmallRational>(sr).num == 7);
        CHECK(get<SmallRational>(sr).denom == 11);
    }

    SUBCASE("Get large integer from small integer")
    {
        CHECK(get<Int>(si) == Int{42});
    }

    SUBCASE("Get large positive integer")
    {
        CHECK(get<Int>(li) == largeInt);
    }

    SUBCASE("Get large negative integer")
    {
        const Expr nli{-largeInt};

        CHECK(get<Int>(nli) == -largeInt);
    }

    SUBCASE("Get large rational from small int/rational")
    {
        CHECK(get<Rational>(42_ex) == Rational{42, 1});
        CHECK(get<Rational>(sr) == Rational{7, 11});
    }

    SUBCASE("Get large rational")
    {
        const Rational largeRational{Int{3}, largeInt};

        CHECK(get<Rational>(Expr{largeInt}) == Rational{largeInt, 1});
        CHECK(get<Rational>(Expr{largeRational}) == largeRational);
    }

    SUBCASE("Get symbol name")
    {
        const auto a = "a"_ex;
        const auto longer = "abc_{def}"_ex;

        CHECK(get<std::string_view>(a) == "a"sv);
        CHECK(get<std::string_view>(longer) == "abc_{def}"sv);
    }
}
