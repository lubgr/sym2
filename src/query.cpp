
#include "query.h"
#include <cassert>
#include "childiterator.h"
#include "expr.h"
#include "get.h"
#include "predicates.h"
#include "view.h"

sym2::BaseExp sym2::asPower(ExprView<> e)
{
    static const auto one = 1_ex;

    if (is<power>(e))
        return {firstOperand(e), secondOperand(e)};

    return {e, one};
}

sym2::ExprView<> sym2::nthOperand(ExprView<composite> e, std::uint32_t n)
{
    auto operand = ChildIterator::logicalChildren(e);

    assert(n > 0);

    return *std::next(operand, n - 1);
}

sym2::ExprView<> sym2::firstOperand(ExprView<composite> e)
{
    return nthOperand(e, 1);
}

sym2::ExprView<> sym2::secondOperand(ExprView<composite> e)
{
    return nthOperand(e, 2);
}

namespace sym2 {
    namespace {
        sym2::ExprView<> nthPhysical(ExprView<> e, std::uint32_t n)
        {
            auto operand = ChildIterator::physicalChildren(e);

            assert(n > 0);

            return *std::next(operand, n - 1);
        }
    }
}

sym2::ExprView<sym2::number> sym2::real(ExprView<complexDomain> c)
{
    return nthPhysical(c, 1);
}

sym2::ExprView<sym2::number> sym2::imag(ExprView<complexDomain> c)
{
    return nthPhysical(c, 2);
}

sym2::ExprView<sym2::integer> sym2::numerator(ExprView<rational> n)
{
    return nthPhysical(n, 1);
}

sym2::ExprView<sym2::integer> sym2::denominator(ExprView<rational> n)
{
    return nthPhysical(n, 2);
}
