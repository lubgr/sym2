
#include "expr.h"
#include <algorithm>
#include <boost/range/algorithm.hpp>
#include <stdexcept>

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
    : structure{structureFrom(info, ops)}
{
    if (info == Tag::scalar)
        throw std::invalid_argument{"Composite Expr instantiated with scalar tag"};
    else if (ops.empty())
        throw std::invalid_argument{"Composite Expr instantiated with empty operands"};

    for (ExprView e : ops)
        boost::copy(e.leaves, std::back_inserter(leaves));
}

sym2::Expr::Expr(Tag info, std::initializer_list<ExprView> ops)
    : Expr{info, std::span<const ExprView>{ops.begin(), ops.end()}}
{}

sym2::SmallVec<sym2::OpDesc, sym2::staticStructureBufferSize> sym2::Expr::structureFrom(
  Tag info, std::span<const ExprView> ops)
{
    SmallVec<OpDesc, staticStructureBufferSize> result{{info, static_cast<std::uint32_t>(ops.size())}};

    for (const ExprView op : ops)
        for (const OpDesc& desc : op.structure)
            if (desc.info == Tag::scalar && result.back().info == Tag::scalar)
                ++result.back().count;
            else
                result.push_back(desc);

    return result;
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
