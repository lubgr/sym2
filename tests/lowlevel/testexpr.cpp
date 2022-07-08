
#include <algorithm>
#include <cmath>
#include <limits>
#include <string_view>
#include "blob.h"
#include "doctest/doctest.h"
#include "expr.h"
#include "exprliteral.h"
#include "query.h"
#include "testutils.h"

using namespace sym2;
using namespace std::literals::string_view_literals;

bool numEvalAndSize1(ExprView<> e)
{
    return e.size() == 1 && (e[0].flags & Flag::numericallyEvaluable) != Flag::none;
}

bool isSmallIntEqualTo(ExprView<> e, int num)
{
    return e[0].header == Type::smallInt && numEvalAndSize1(e) && e[0].main.exact.num == num
      && e[0].main.exact.denom == 1;
}

bool isSmallRationalEqualTo(ExprView<> e, int num, int denom)
{
    return e[0].header == Type::smallRational && numEvalAndSize1(e) && e[0].main.exact.num == num
      && e[0].main.exact.denom == denom;
}

bool hasLargeIntCharacteristics(ExprView<> e)
{
    return e[0].header == Type::largeInt
      && static_cast<std::size_t>(e.size()) == e[0].main.nChildBlobs + 1
      && (e[0].flags & Flag::numericallyEvaluable) != Flag::none;
}

bool hasLargeRationalCharacteristics(ExprView<> e)
{
    return e[0].header == Type::largeRational
      && (e[0].flags & Flag::numericallyEvaluable) != Flag::none;
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

bool hasSignFlag(ExprView<> e)
{
    const Flag actual = e[0].flags;

    return (actual & Flag::positive) != Flag::none || (actual & Flag::negative) != Flag::none;
}

TEST_CASE("Expr constructor")
{
    std::pmr::memory_resource* mr = std::pmr::get_default_resource();

    SUBCASE("Zero")
    {
        CHECK(isSmallIntEqualTo(0_ex, 0));
    }

    SUBCASE("Small int")
    {
        CHECK(isSmallIntEqualTo(42_ex, 42));
        CHECK(isSmallIntEqualTo(Expr{-42, mr}, -42));
    }

    SUBCASE("Minus one")
    {
        CHECK(isSmallIntEqualTo(Expr{-1, mr}, -1));
    }

    SUBCASE("Small rational")
    {
        CHECK(isSmallRationalEqualTo(Expr{2, 3, mr}, 2, 3));
        CHECK(isSmallRationalEqualTo(Expr{-2, 3, mr}, -2, 3));
    }

    SUBCASE("Small rational normalization")
    {
        CHECK(isSmallRationalEqualTo(Expr{9, 6, mr}, 3, 2));
    }

    SUBCASE("NaN floating point argument throws")
    {
        CHECK_THROWS(Expr{std::numeric_limits<double>::quiet_NaN(), mr});
        CHECK_THROWS(Expr{std::numeric_limits<double>::signaling_NaN(), mr});
    }

    SUBCASE("Small rational throws if denom == 0")
    {
        CHECK_THROWS(Expr{42, 0, mr});
    }

    SUBCASE("Small rational negative denom")
    {
        CHECK(isSmallRationalEqualTo(Expr{2, -3, mr}, -2, 3));
    }

    SUBCASE("Small rational to small int")
    {
        CHECK(Expr{10, 5, mr} == 2_ex);
        CHECK(Expr{10, -5, mr} == Expr{-2, mr});
    }

    SUBCASE("Large int")
    {
        const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};

        SUBCASE("> 0")
        {
            CHECK(hasLargeIntCharacteristics(Expr{LargeIntRef{largeInt}, mr}));
        }

        SUBCASE("< 0")
        {
            CHECK(hasLargeIntCharacteristics(Expr{LargeIntRef{-largeInt}, mr}));
        }
    }

    SUBCASE("Large int to small int")
    {
        const LargeInt fits{"12345"};

        CHECK(isSmallIntEqualTo(Expr{LargeIntRef{fits}, mr}, 12345));
        CHECK(isSmallIntEqualTo(Expr{LargeIntRef{-fits}, mr}, -12345));
    }

    SUBCASE("Large zero to small zero")
    {
        const LargeInt zero{0};

        CHECK(isSmallIntEqualTo(Expr{LargeIntRef{zero}, mr}, 0));
    }

    SUBCASE("Large to small rational")
    {
        const LargeRational fits{17, 31};

        CHECK(isSmallRationalEqualTo(Expr{LargeRationalRef{fits}, mr}, 17, 31));
    }

    SUBCASE("Large rational")
    {
        const LargeInt large{"283749237498273489274382709084938593857982374982729873"};
        const LargeRational lr{LargeInt{1}, large};

        SUBCASE("> 0")
        {
            const Expr e{LargeRationalRef{lr}};

            CHECK(hasLargeRationalCharacteristics(e));
            CHECK(isSmallIntEqualTo(numerator(e), 1));
            CHECK(hasLargeIntCharacteristics(denominator(e)));
        }

        SUBCASE("< 0")
        {
            const Expr e{LargeRationalRef{-lr}};

            CHECK(hasLargeRationalCharacteristics(e));
            CHECK(isSmallIntEqualTo(numerator(e), -1));
            CHECK(hasLargeIntCharacteristics(denominator(e)));
        }

        SUBCASE("Num/denom saved as small int if possible")
        {
            const Expr e{LargeRationalRef{lr}};
            auto v = view(e);

            CHECK(v[1].header == Type::smallInt);
            CHECK(v[2].header == Type::largeInt);
        }
    }

    SUBCASE("Sign flags")
    {
        CHECK(hasSignFlag(42_ex));
        CHECK(hasSignFlag(Expr{-42, mr}));
        CHECK(hasSignFlag(Expr{2, 3, mr}));
        CHECK(hasSignFlag(1.23456_ex));

        const LargeInt n1{"2893479827489234263426423649823478238570935809575903675026398235"};
        const LargeInt n2{"283749237498273489274382709084938593857982374982729873"};
        const LargeRational lr{n1, n2};

        CHECK(hasSignFlag(Expr{LargeIntRef{n1}, mr}));
        CHECK(hasSignFlag(Expr{LargeRationalRef{lr}, mr}));

        CHECK(hasSignFlag(Expr{"a", SymbolFlag::positive, mr}));
        CHECK(hasSignFlag(Expr{"a", SymbolFlag::positiveReal, mr}));

        CHECK(hasSignFlag(Expr{"pi", 3.14, mr}));
        CHECK(hasSignFlag(Expr{"minpi", -3.14, mr}));
    }

    SUBCASE("Too long symbol name throws")
    {
        CHECK_THROWS(Expr{"12345678901234", mr});
    }

    SUBCASE("Empty symbol name throws")
    {
        CHECK_THROWS(Expr{"", mr});
    }

    SUBCASE("Successfull symbol creation")
    {
        const Expr symbol{"abcdef_{gh}^i", mr};
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
        CHECK((e[0].flags & Flag::numericallyEvaluable) != Flag::none);
        CHECK((e[0].flags & Flag::negative) != Flag::none);
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
        CHECK(e[0].mid.nLogicalOrPhysicalChildren == 1);
        CHECK(e[0].main.nChildBlobs == 2);

        CHECK(e[1].header == Type::functionId);
        CHECK(e[1].pre.name == "sin"sv);

        // doctest can't output a function pointer properly and tries to cast to void*, which
        // doesn't compile. Also note that taking the address of a std:: function is technically not
        // allowed either, but we don't care for now.
        const bool evalFctPointerIsStdSin =
          e[1].main.unaryEval == static_cast<UnaryDoubleFctPtr>(std::sin);
        CHECK(evalFctPointerIsStdSin);

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
        CHECK(e[0].mid.nLogicalOrPhysicalChildren == 2);
        CHECK(e[0].main.nChildBlobs == 3);

        CHECK(e[1].header == Type::functionId);
        CHECK(e[1].pre.name == "atan2"sv);

        // doctest can't output a function pointer properly and tries to cast to void*, which
        // doesn't compile. Also note that taking the address of a std:: function is technically not
        // allowed either, but we don't care for now.
        const bool evalFctPointerIsStdAtan2 =
          e[1].main.binaryEval == static_cast<BinaryDoubleFctPtr>(std::atan2);
        CHECK(evalFctPointerIsStdAtan2);

        CHECK(isShortSymbol(e[2], "a"));
        CHECK(isShortSymbol(e[3], "b"));
    }

    SUBCASE("Complex number creation")
    {
        const auto two = 2_ex;
        const Expr frac{3, 7};
        const std::vector<ExprView<>> args{two, frac};
        const Expr c{CompositeType::complexNumber, args, mr};
        auto e = view(c);

        CHECK(e[0].header == Type::complexNumber);
        CHECK(e[0].flags == Flag::numericallyEvaluable);
        CHECK(allNullChars(e[0].pre.name));
        CHECK(e[0].mid.nLogicalOrPhysicalChildren == 2);
        CHECK(e[0].main.nChildBlobs == 2);
    }
}
