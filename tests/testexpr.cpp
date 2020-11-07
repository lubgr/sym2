
#include "abc.h"
#include "doctest/doctest.h"
#include "expr.h"
#include "testutils.h"

using namespace sym2;

TEST_CASE("Expr constructor leaf count")
{
    Expr composite{Tag::sum, {a, b, c}};
    auto [desc, leaves] = view(composite);

    CHECK_RANGES_EQ(leaves, (std::vector{leaf.a, leaf.b, leaf.c}));

    CHECK(desc.size() == 2);
    CHECK(desc[0].info == Tag::sum);
    CHECK(desc[0].count == 3);
    CHECK(desc[1].info == Tag::scalar);
    CHECK(desc[1].count == 3);
}

TEST_CASE("Expr constructor error handling")
{
    CHECK_THROWS(Expr{Tag::scalar, {a, b, c}});
    CHECK_THROWS(Expr{Tag::sum, {}});
}
