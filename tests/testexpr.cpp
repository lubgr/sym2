
#include <algorithm>
#include <cmath>
#include <limits>
#include <string_view>
#include "doctest/doctest.h"
#include "sym2/expr.h"
#include "sym2/get.h"
#include "sym2/query.h"

using namespace sym2;
using namespace std::literals::string_view_literals;

TEST_CASE("Construction and retrieval")
{
    const Expr::allocator_type alloc{};

    SUBCASE("Small int")
    {
        SUBCASE("Zero")
        {
            const Expr zero{alloc};

            CHECK(get<std::int16_t>(zero) == 0);
        }

        SUBCASE("Successful roundtrip")
        {
            const Expr positive{123, alloc};
            const Expr negative{-123, alloc};

            CHECK(get<std::int16_t>(positive) == 123);
            CHECK(get<std::int16_t>(negative) == -123);
        }

        SUBCASE("Small int > 16bit throws")
        {
            CHECK_THROWS(Expr{std::numeric_limits<std::int16_t>::max() + 10, alloc});
        }
    }

    SUBCASE("Small rational")
    {
        SUBCASE("Num/denom are retained")
        {
            const Expr n{-3, 7, alloc};

            CHECK(get<SmallRational>(n).num == -3);
            CHECK(get<SmallRational>(n).denom == 7);
        }

        SUBCASE("Normalization")
        {
            const Expr n{4, 8, alloc};

            CHECK(get<SmallRational>(n).num == 1);
            CHECK(get<SmallRational>(n).denom == 2);
        }

        SUBCASE("Sign swap with negative denominator")
        {
            const Expr n{1, -42, alloc};

            CHECK(get<SmallRational>(n).num == -1);
            CHECK(get<SmallRational>(n).denom == 42);
        }

        SUBCASE("Zero denominator throws")
        {
            CHECK_THROWS(Expr{123, 0, alloc});
        }
    }

    SUBCASE("Floating point expression")
    {
        SUBCASE("Roundtrip")
        {
            const Expr n{-123.456789, alloc};

            CHECK(get<double>(n) == doctest::Approx(-123.456789));
        }

        SUBCASE("NaN floating point argument throws")
        {
            CHECK_THROWS(Expr{std::numeric_limits<double>::quiet_NaN(), alloc});
            CHECK_THROWS(Expr{std::numeric_limits<double>::signaling_NaN(), alloc});
        }
    }

    SUBCASE("Large int")
    {
        SUBCASE("Positive roundtrip")
        {
            const LargeInt expected{"2323498273984729837498234029380492839489234902384"};
            const Expr n{expected, alloc};

            const LargeInt actual = get<LargeInt>(n);

            CHECK(expected == actual);
        }

        SUBCASE("Negative roundtrip")
        {
            const LargeInt expected{"-2323498273984729837498234029380492839489234902384"};
            const Expr n{expected, alloc};

            const LargeInt actual = get<LargeInt>(n);

            CHECK(expected == actual);
        }

        SUBCASE("Large int to small int")
        {
            const LargeInt fits{"12345"};
            const Expr n{fits, alloc};

            CHECK(get<std::int16_t>(n) == 12345);
        }
    }

    SUBCASE("Large rational")
    {
        SUBCASE("Large to small rational")
        {
            const LargeRational fits{17, 31};
            const Expr n{fits, alloc};

            CHECK(get<SmallRational>(n).num == 17);
            CHECK(get<SmallRational>(n).denom == 31);
        }

        SUBCASE("Roundtrips")
        {
            const LargeInt large{"283749237498273489274382709084938593857982374982729873"};
            const LargeRational lr{LargeInt{1}, large};

            SUBCASE("Positive")
            {
                const Expr n{lr, alloc};

                CHECK(get<LargeRational>(n) == lr);
            }

            SUBCASE("Negative")
            {
                const Expr n{-lr, alloc};

                CHECK(get<LargeRational>(n) == -lr);
            }
        }
    }

    SUBCASE("Symbols")
    {
        SUBCASE("Empty symbol name throws")
        {
            CHECK_THROWS(Expr{"", alloc});
        }

        SUBCASE("Short symbol roundtrip")
        {
            const Expr oneByte{"a", alloc};
            const Expr sevenBytes{"a_{b}^c", alloc};

            CHECK(get<std::string_view>(oneByte) == "a");
            CHECK(get<std::string_view>(sevenBytes) == "a_{b}^c");
        }

        SUBCASE("Long symbol roundtrip")
        {
            const std::string_view name{
              "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
            const Expr symbol{name, alloc};

            CHECK(get<std::string_view>(symbol) == name);
        }

        SUBCASE("Long symbol null byte on next Blob")
        {
            const std::string_view name{"12345678"};
            const Expr symbol{name, alloc};

            CHECK(get<std::string_view>(symbol) == name);
        }
    }

    SUBCASE("Constants")
    {
        SUBCASE("Empty constant name throws")
        {
            CHECK_THROWS(Expr{"", 1.2345, alloc});
        }

        SUBCASE("Short constant roundtrip")
        {
            const Expr pi{"pi", 3.14, alloc};

            CHECK(get<std::string_view>(pi) == "pi");
            CHECK(get<double>(pi) == doctest::Approx(3.14));
        }

        SUBCASE("Long constant roundtrip")
        {
            const std::string_view name{"A-rather-long-name-that-needs-more-space"};
            const Expr constant{name, 1.2345, alloc};

            CHECK(get<std::string_view>(constant) == name);
            CHECK(get<double>(constant) == doctest::Approx(1.2345));
        }
    }

    SUBCASE("Unary function roundtrip")
    {
        const Expr sinA{"sin", "a"_ex, std::sin, alloc};

        CHECK(is<function>(sinA));

        CHECK(get<std::string_view>(sinA) == "sin");

        // doctest can't output a function pointer properly and tries to cast to void*, which
        // doesn't compile. Also note that taking the address of a std:: function is technically not
        // allowed either, but we don't care for now.
        const bool evalFctPointerIsSin =
          get<UnaryDoubleFctPtr>(sinA) == static_cast<UnaryDoubleFctPtr>(std::sin);
        CHECK(evalFctPointerIsSin);

        CHECK(firstOperand(sinA) == "a"_ex);
    }

    SUBCASE("Unary function roundtrip with long name")
    {
        const std::string_view longName = "abcdefghijklmnopqrstuvwxyz0123456789";
        const Expr symbol{"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", alloc};
        const Expr sinOfLongSymbol{longName, symbol, std::sin, alloc};

        CHECK(is<function>(sinOfLongSymbol));

        CHECK(get<std::string_view>(sinOfLongSymbol) == longName);

        // See above.
        const bool evalFctPointerIsSin =
          get<UnaryDoubleFctPtr>(sinOfLongSymbol) == static_cast<UnaryDoubleFctPtr>(std::sin);
        CHECK(evalFctPointerIsSin);

        CHECK(firstOperand(sinOfLongSymbol) == symbol);
    }

    SUBCASE("Binary function roundtrip")
    {
        const Expr atan2ab{"atan2", "a"_ex, "b"_ex, std::atan2, alloc};

        CHECK(is<function>(atan2ab));

        CHECK(get<std::string_view>(atan2ab) == "atan2");

        // doctest can't output a function pointer properly and tries to cast to void*, which
        // doesn't compile. Also note that taking the address of a std:: function is technically not
        // allowed either, but we don't care for now.
        const bool evalFctPointerIsAtan2 =
          get<BinaryDoubleFctPtr>(atan2ab) == static_cast<BinaryDoubleFctPtr>(std::atan2);
        CHECK(evalFctPointerIsAtan2);

        CHECK(firstOperand(atan2ab) == "a"_ex);
        CHECK(secondOperand(atan2ab) == "b"_ex);
    }

    SUBCASE("Complex number roundtrip")
    {
        const ScopedLocalVec<Expr> args{{Expr{2, alloc}, Expr{3, 7, alloc}}, alloc};
        const Expr cx{CompositeType::complexNumber, args, alloc};

        CHECK(get<std::int16_t>(real(cx)) == 2);
        CHECK(get<SmallRational>(imag(cx)).num == 3);
        CHECK(get<SmallRational>(imag(cx)).denom == 7);
    }

    SUBCASE("Small expression")
    {
        // We setup a buffer so that no actual space is left; the fact that construction of the
        // SmallExpr objects doesn't throw shows that the fixed in-object storage is sufficient.
        StackBuffer<sizeof(Blob), alignof(Blob)> full{nullptr, false};
        Expr::allocator_type alloc{&full};

        {
            alloc.allocate(1); // No buffer space left from now on

            CHECK_THROWS_AS((Expr{123, alloc}), std::bad_alloc);
        }

        SUBCASE("Small rational numbers")
        {
            const SmallExpr<1> zero{alloc};
            const SmallExpr<1> integer{42, alloc};
            const SmallExpr<2> rational{1, 7, alloc};

            CHECK(get<std::int16_t>(zero) == 0);
            CHECK(get<std::int16_t>(integer) == 42);
            CHECK(get<SmallRational>(rational).num == 1);
            CHECK(get<SmallRational>(rational).denom == 7);
        }

        SUBCASE("Symbols")
        {
            const SmallExpr<1> symbol{"abc", alloc};
            const SmallExpr<3> longer{"0123456789abcde", alloc};

            CHECK(get<std::string_view>(symbol) == "abc");
            CHECK(get<std::string_view>(longer) == "0123456789abcde");

            CHECK_THROWS_AS(SmallExpr<1>("0123456789abcde", alloc), std::bad_alloc);
        }
    }

    SUBCASE("FixedExpr roundtrips")
    {
        SUBCASE("Integer")
        {
            const FixedExpr<1> zero{};
            CHECK(get<std::int16_t>(zero) == 0);

            const FixedExpr<1> positive{123};
            CHECK(get<std::int16_t>(positive) == 123);
        }

        SUBCASE("Num/denom")
        {
            const FixedExpr<1> n{-3, 7};

            CHECK(get<SmallRational>(n).num == -3);
            CHECK(get<SmallRational>(n).denom == 7);
        }

        SUBCASE("Symbols")
        {
            const FixedExpr<1> small{"abc"};
            const FixedExpr<3> threeBytes{"0123456789abcde"};
            const FixedExpr<4> fourBytes{"0123456789abcdef"};

            CHECK(get<std::string_view>(small) == "abc");
            CHECK(get<std::string_view>(threeBytes) == "0123456789abcde");
            CHECK(get<std::string_view>(fourBytes) == "0123456789abcdef");
        }
    }

    SUBCASE("User defined literal _ex")
    {
        CHECK(get<std::int16_t>(123_ex) == 123);

        CHECK(get<std::string_view>("abc"_ex) == "abc");
        CHECK_THROWS_AS(get<std::string_view>("abcdefghijklmnopqrstuvwxyz"_ex), std::bad_alloc);
    }
}
