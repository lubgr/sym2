
#include "doctest/doctest.h"
#include "sym2/autosimpl.h"
#include "sym2/constants.h"
#include "sym2/expr.h"
#include "sym2/predicates.h"
#include "sym2/query.h"
#include "testutils.h"

using namespace sym2;

TEST_CASE("Type queries for simple types")
{
    const Expr::allocator_type alloc{};
    const Expr fp{3.14, alloc};
    const Expr sr{7, 11, alloc};
    const Expr a{"a"_ex, alloc};
    const Expr n{42_ex, alloc};
    const LargeInt largeInt{"2323498273984729837498234029380492839489234902384"};
    const Expr li{largeInt, alloc};
    const Expr lr{LargeRational{LargeInt{"1234528973498279834827384284"}, largeInt}, alloc};
    const Expr cx = directComplex(2_ex, 3_ex, alloc);
    const Expr s = directSum({n, a, "b"_ex}, alloc);
    const Expr pr = directProduct({n, a, "b"_ex}, alloc);
    const Expr pw = directPower(n, a, alloc);
    const Expr sinA{"sin", "a"_ex, std::sin, alloc};
    const Expr atan2Ab{"atan2", "a"_ex, "b"_ex, std::atan2, alloc};

    SUBCASE("Numeric subtypes")
    {
        CHECK(is<integer>(li));
        CHECK(is < large && integer > (li));

        CHECK(is<rational>(lr));
        CHECK(is < large && rational > (lr));

        CHECK(is<integer>(n));
        CHECK(is < small && integer > (n));

        CHECK(is < small && rational > (sr));

        CHECK(is<floatingPoint>(fp));

        for (ExprView<> n : std::initializer_list<ExprView<>>{fp, sr, li, lr})
            CHECK(is < realDomain && number > (n));

        CHECK(is < complexDomain && number > (cx));
    }

    SUBCASE("Small/large distinction")
    {
        CHECK(is < small && !large > (n));
        CHECK(is < small && !large > (sr));

        CHECK(is < !small && large > (a));
        CHECK(is < !small && large > (pi));
        CHECK(is < !small && large > (fp));
        CHECK(is < !small && large > (pr));
        CHECK(is < !small && large > (sinA));
        CHECK(is < !small && large > (atan2Ab));
    }

    SUBCASE("Scalar")
    {
        CHECK(is < scalar && !composite > (n));
        CHECK(is < scalar && !composite > (cx));
        CHECK(is < scalar && !composite > ("d"_ex));
        CHECK(is < scalar && !composite > (euler));
    }

    SUBCASE("Number")
    {
        for (ExprView<> n : std::initializer_list<ExprView<>>{n, fp, sr, li, lr, cx})
            CHECK(is<number>(n));

        for (ExprView<> e :
          std::initializer_list<ExprView<>>{pi, euler, a, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<number>(e));
    }

    SUBCASE("Integer")
    {
        CHECK(is<integer>(n));
        CHECK(is<integer>(Expr{li, alloc}));

        for (ExprView<> e :
          std::initializer_list<ExprView<>>{pi, euler, a, s, pr, pw, fp, sr, lr, cx, sinA, atan2Ab})
            CHECK_FALSE(is<integer>(e));
    }

    SUBCASE("Constant")
    {
        CHECK(is<constant>(pi));
        CHECK(is<constant>(euler));

        for (ExprView<> e :
          std::initializer_list<ExprView<>>{n, fp, sr, lr, cx, a, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<constant>(e));
    }

    SUBCASE("Symbol")
    {
        for (ExprView<> e : {"a"_ex, "b"_ex, "c"_ex})
            CHECK(is<symbol>(e));

        for (ExprView<> e :
          std::initializer_list<ExprView<>>{n, fp, sr, lr, cx, pi, euler, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<symbol>(e));
    }

    SUBCASE("Symbol domain and sign")
    {
        {
            const Expr symbol{"a", alloc};
            CHECK_FALSE(is < positive || negative || realDomain > (symbol));
            CHECK(is<complexDomain>(symbol));
        }

        {
            const Expr symbol{"a", DomainFlag::positive, alloc};
            CHECK(is<positive>(symbol));
            CHECK_FALSE(is < negative || complexDomain > (symbol));
            CHECK(is < positive && realDomain > (symbol));
        }

        {
            const Expr symbol{"a", DomainFlag::real, alloc};
            CHECK(is<realDomain>(symbol));
            CHECK_FALSE(is < positive || negative || complexDomain > (symbol));
            CHECK(is<realDomain>(symbol));
        }

        {
            const Expr symbol{"a", DomainFlag::none, alloc};
            CHECK(is<complexDomain>(symbol));
            CHECK_FALSE(is < positive || negative || realDomain > (symbol));
            CHECK(is<complexDomain>(symbol));
        }
    }

    SUBCASE("Named types")
    {
        for (ExprView<> e : std::initializer_list<ExprView<>>{a, pi, euler, atan2Ab, sinA})
            CHECK(is < symbol || function || constant > (e));

        for (ExprView<> e :
          std::initializer_list<ExprView<>>{n, fp, sr, lr, cx, s, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is < symbol || constant > (e));
    }

    SUBCASE("Sum")
    {
        CHECK(is<sum>(s));

        for (ExprView<> e :
          std::initializer_list<ExprView<>>{n, a, pi, fp, sr, lr, cx, pr, pw, sinA, atan2Ab})
            CHECK_FALSE(is<sum>(e));
    }

    SUBCASE("Product")
    {
        CHECK(is<product>(pr));

        for (ExprView<> e :
          std::initializer_list<ExprView<>>{n, a, pi, fp, sr, lr, cx, s, pw, sinA, atan2Ab})
            CHECK_FALSE(is<product>(e));
    }

    SUBCASE("Power")
    {
        CHECK(is<power>(pw));

        for (ExprView<> e :
          std::initializer_list<ExprView<>>{n, a, pi, fp, sr, lr, cx, s, pr, sinA, atan2Ab})
            CHECK_FALSE(is<power>(e));
    }

    SUBCASE("Function")
    {
        CHECK(is<function>(sinA));
        CHECK(is<function>(atan2Ab));

        for (ExprView<> e : std::initializer_list<ExprView<>>{n, a, pi, fp, sr, lr, cx, s, pr}) {
            CHECK_FALSE(is<function>(e));
        }
    }
}

TEST_CASE("Type queries for tagged types")
{
    const auto n = 42_ex;

    CHECK(is<number>(ExprView < number && positive && realDomain > {n}));
    CHECK(is<small>(ExprView<number>{n}));
    CHECK(is < small && realDomain > (ExprView<number>{n}));
    CHECK(is < small && number && realDomain > (ExprView<number>{n}));
    CHECK(is<number>(
      ExprView < !symbol && !function && !(sum || power || complexDomain || !small) > {n}));
}
