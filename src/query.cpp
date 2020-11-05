
#include "query.h"

bool sym2::isScalar(ExprView e)
{
    return (e.structure.front().info & Tag::scalar) != Tag{0};
}

bool sym2::isNumber(ExprView e)
{
    return isScalar(e) && holds_alternative<Number>(e.leaves.front());
}

bool sym2::isSymbolOrConstant(ExprView e)
{
    const auto& leaf = e.leaves.front();

    return isScalar(e) && holds_alternative<String>(leaf);
}

bool sym2::isSymbol(ExprView e)
{
    const auto& leaf = e.leaves.front();

    return isScalar(e) && holds_alternative<String>(leaf) && !isConstant(e);
}

bool sym2::isConstant(ExprView e)
{
    const auto& leaf = e.leaves.front();

    return isScalar(e) && holds_alternative<String>(leaf)
      && (get<String>(leaf) == "pi" || get<String>(leaf) == "euler");
}

namespace sym2 {
    template <int N>
    bool equalTo(ExprView e)
    {
        static const LeafUnion n = Number{N};

        return isScalar(e) && e.leaves.front() == n;
    }
}

bool sym2::isZero(ExprView e)
{
    return equalTo<0>(e);
}

bool sym2::isOne(ExprView e)
{
    return equalTo<1>(e);
}

std::uint32_t sym2::nOps(ExprView e)
{
    if (isScalar(e))
        return 0;

    return e.structure.front().count;
}
