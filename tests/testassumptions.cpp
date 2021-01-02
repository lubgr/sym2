
#include "doctest/doctest.h"
#include "expr.h"

using namespace sym2;

TEST_CASE("Size of data")
{
    CHECK(sizeof(Flag) == 1);
    CHECK(sizeof(Blob) == 16);
    CHECK(sizeof(Blob::Data2) == 2);
    CHECK(sizeof(Blob::Data4) == 4);
    CHECK(sizeof(Blob::Data8) == 8);
    CHECK(sizeof(ExprView) == 16);
    CHECK(sizeof(Expr) == 184);
}
