
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

bool isShortSymbol(Operand op, std::string_view expectedName)
{
    return op.header == Type::symbol && op.flags == Flag::none && op.sign == Sign::neither && op.name == expectedName
      && std::find_if(op.data.name, std::end(op.data.name), [](char c) { return c != '\0'; }) == std::end(op.data.name);
}

template <std::size_t N>
bool allNullChars(const char (&str)[N])
{
    return std::find_if(str, std::cend(str), [](char c) { return c != '\0'; }) == std::cend(str);
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
        const Int large{"283749237498273489274382709084938593857982374982729873"};
        const Rational lr{Int{1}, large};

        SUBCASE("> 0")
        {
            CHECK(isLargeRational(Expr{lr}, Sign::positive));
        }

        SUBCASE("< 0")
        {
            CHECK(isLargeRational(Expr{-lr}, Sign::negative));
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
        CHECK(e[0].sign == Sign::neither);

        CHECK(e[0].name == std::string_view{"abcdef_{gh}^i"});
    }

    SUBCASE("Symbol starting with + is positive")
    {
        const Expr symbol{"+a"};
        auto e = view(symbol);

        CHECK(e[0].sign == Sign::positive);
    }

    SUBCASE("Constant creation")
    {
        const double value = -1.234;
        const Expr c{"test", value};
        auto e = view(c);

        CHECK(e.size() == 3);

        CHECK(e[0].header == Type::constant);
        CHECK(e[0].flags == Flag::numericallyEvaluable);
        CHECK(e[0].sign == Sign::negative);
        CHECK(allNullChars(e[0].name));
        CHECK(e[0].data.count == 2);

        CHECK(isShortSymbol(e[1], "test"));

        CHECK(e[2].header == Type::floatingPoint);
        CHECK(e[2].flags == Flag::numericallyEvaluable);
        CHECK(e[2].sign == Sign::negative);
        CHECK(e[2].data.inexact == doctest::Approx(value));
    }

    SUBCASE("Unary function creation")
    {
        const Expr sinA{"sin", "a"_ex, std::sin};
        auto e = view(sinA);

        CHECK(e.size() == 3);

        CHECK(e[0].header == Type::unaryFunction);
        CHECK(e[0].flags == Flag::none);
        CHECK(e[0].sign == Sign::unknown);
        CHECK(allNullChars(e[0].name));
        CHECK(e[0].data.unaryEval == static_cast<UnaryDoubleFctPtr>(std::sin));

        CHECK(isShortSymbol(e[1], "sin"));
        CHECK(isShortSymbol(e[2], "a"));
    }

    SUBCASE("Binary function creation")
    {
        const Expr atan2ab{"atan2", "a"_ex, "b"_ex, std::atan2};
        auto e = view(atan2ab);

        CHECK(e.size() == 4);

        CHECK(e[0].header == Type::binaryFunction);
        CHECK(e[0].flags == Flag::none);
        CHECK(e[0].sign == Sign::unknown);
        CHECK(allNullChars(e[0].name));
        CHECK(e[0].data.binaryEval == static_cast<BinaryDoubleFctPtr>(std::atan2));

        CHECK(isShortSymbol(e[1], "atan2"));
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
        CHECK(e[0].sign == Sign::neither);
        CHECK(allNullChars(e[0].name));
        CHECK(e[0].data.count == 2);
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

    SUBCASE("Get large integer")
    {
        CHECK(get<Int>(li) == largeInt);
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
