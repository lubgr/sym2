
#include "doctest/doctest.h"
#include "expr.h"

using namespace sym2;

TEST_CASE("Size of data")
{
    CHECK(sizeof(Flag) == 1);
    CHECK(sizeof(Operand) == 16);
    CHECK(sizeof(Operand::Data6) == 6);
    CHECK(sizeof(Operand::Data8) == 8);
    CHECK(sizeof(ExprView) == 16);
    CHECK(sizeof(Expr) == 184);
}
