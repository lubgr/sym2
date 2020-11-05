
#include "expr.h"
#include <algorithm>
#include <boost/range/algorithm.hpp>

sym2::Tag sym2::operator|(Tag lhs, Tag rhs)
{
    return lhs |= rhs;
}

sym2::Tag& sym2::operator|=(Tag& lhs, Tag rhs)
{
    using T = std::underlying_type_t<Tag>;

    lhs = Tag{static_cast<T>(lhs) | static_cast<T>(rhs)};

    return lhs;
}

sym2::Tag sym2::operator&(Tag lhs, Tag rhs)
{
    return lhs &= rhs;
}

sym2::Tag& sym2::operator&=(Tag& lhs, Tag rhs)
{
    using T = std::underlying_type_t<Tag>;

    lhs = Tag{static_cast<T>(lhs) & static_cast<T>(rhs)};

    return lhs;
}

sym2::Expr::Expr(int n)
    : Expr{Number{n}}
{}

sym2::Expr::Expr(double n)
    : Expr{Number{n}}
{}

sym2::Expr::Expr(const Number& n)
    : structure{{Tag::scalar, 1}}
    , leaves{{n}}
{}

sym2::Expr::Expr(const char* symbol)
    : structure{{Tag::scalar, 1}}
    , leaves{{symbol}}
{}

sym2::Expr::Expr(const String& symbol)
    : structure{{Tag::scalar, 1}}
    , leaves{{symbol}}
{}

sym2::Expr::Expr(ExprView e)
    : structure{e.structure.begin(), e.structure.end()}
    , leaves{e.leaves.begin(), e.leaves.end()}
{}

sym2::Expr::Expr(Tag info, std::span<const ExprView> ops)
    : structure{{info, static_cast<std::uint32_t>(ops.size())}}
{
    for (ExprView e : ops) {
        boost::copy(e.structure, std::back_inserter(structure));
        boost::copy(e.leaves, std::back_inserter(leaves));
    }
}

sym2::Expr::Expr(Tag info, SmallVecBase<Expr>&& ops)
    : structure{{info, static_cast<std::uint32_t>(ops.size())}}
{
    for (ExprView e : ops) {
        std::move(e.structure.begin(), e.structure.end(), std::back_inserter(structure));
        std::move(e.leaves.begin(), e.leaves.end(), std::back_inserter(leaves));
    }
}

sym2::Expr::operator sym2::ExprView() const
{
    return {structure, leaves};
}

bool sym2::operator==(ExprView lhs, ExprView rhs)
{
    static const auto eqDesc = [](OpDesc lhs, OpDesc rhs) {
        return std::tie(lhs.info, lhs.count) == std::tie(rhs.info, rhs.count);
    };

    return boost::equal(lhs.structure, rhs.structure, eqDesc) && boost::equal(lhs.leaves, rhs.leaves);
}

bool sym2::operator==(ExprView lhs, int rhs)
{
    return lhs == Expr{rhs};
}

bool sym2::operator==(int lhs, ExprView rhs)
{
    return Expr{lhs} == rhs;
}

bool sym2::operator!=(ExprView lhs, ExprView rhs)
{
    return !(lhs == rhs);
}

bool sym2::operator!=(ExprView lhs, int rhs)
{
    return !(lhs == rhs);
}

bool sym2::operator!=(int lhs, ExprView rhs)
{
    return !(lhs == rhs);
}
