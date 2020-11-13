
#include "expr.h"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include "query.h"

sym2::Expr::Expr(int n)
    : small{Operand{.header = Flag::smallInt, .data = {.exact = {n, 1}}}}
{
    /* Importante note: when changing this to a vector with a local buffer, we need to implemented move (assignment)
     * constructors that are currently = defaulted above: */
    static_assert(std::is_same_v<decltype(large), std::vector<sym2::Rational>>);
}

sym2::Expr::Expr(double n)
    : small{Operand{.header = Flag::floatingPoint, .data = {.inexact = n}}}
{}

sym2::Expr::Expr(int num, int denom)
    : small{Operand{.header = Flag::smallRational, .data = {.exact = {num, denom}}}}
{}

sym2::Expr::Expr(const Rational& n)
    : large{n}
{
    small.push_back({.header = Flag::largeRational, .hasLargeRationals = true, .data = {.large = &large.front()}});
}

sym2::Expr::Expr(Rational&& n)
    : large{std::move(n)}
{
    small.push_back({.header = Flag::largeRational, .hasLargeRationals = true, .data = {.large = &large.front()}});
}

sym2::Expr::Expr(std::string_view symbol)
{
    if (symbol.length() > 13)
        throw std::invalid_argument("Symbol names must be < 13 characters long");

    Operand op{Flag::symbol};

    if (symbol == "pi" || symbol == "euler")
        op.header = Flag::constant;

    auto* dest = std::next(reinterpret_cast<char*>(&op), 2);

    std::copy(symbol.cbegin(), symbol.cend(), dest);

    small.push_back(op);
}

sym2::Expr::Expr(ExprView e)
    : small{e.begin(), e.end()}
{
    storeAndUpdateRationals();
}

sym2::Expr::Expr(Flag composite, std::span<const ExprView> ops)
    : small{Operand{.header = composite, .data = {.count = ops.size()}}}
{
    assert(
      composite == Flag::sum || composite == Flag::product || composite == Flag::power || composite == Flag::function);

    for (ExprView e : ops) {
        if (e.front().hasLargeRationals)
            small.front().hasLargeRationals = true;

        std::copy(e.begin(), e.end(), std::back_inserter(small));
    }

    storeAndUpdateRationals();
}

void sym2::Expr::storeAndUpdateRationals()
{
    for (const Operand& op : small)
        if (op.header == Flag::largeRational)
            large.push_back(*op.data.large);

    /* We need a second step after the first one is finished, due to possible iterator/reference invalidation. */
    updateRationalPointer();
}

void sym2::Expr::updateRationalPointer()
{
    auto rational = large.begin();

    for (Operand& op : small)
        if (op.header == Flag::largeRational)
            op.data.large = &*rational++;
}

sym2::Expr::Expr(Flag composite, std::initializer_list<ExprView> ops)
    : Expr{composite, std::span<const ExprView>{ops.begin(), ops.end()}}
{}

sym2::Expr::Expr(const Expr& other)
    : small{other.small}
    , large{other.large}
{
    updateRationalPointer();
}

sym2::Expr& sym2::Expr::operator=(Expr other)
{
    swap(*this, other);

    return *this;
}

sym2::Expr::operator sym2::ExprView() const
{
    assert(small.size() >= 1);

    return ExprView{small.data(), small.size()};
}

bool sym2::operator==(ExprView lhs, ExprView rhs)
{
    if (lhs.size() != rhs.size())
        return false;
    else if (!lhs.front().hasLargeRationals)
        /* Take shortcut if we have only trivial data to compare: */
        return std::memcmp(&lhs.front(), &rhs.front(), lhs.size() * sizeof(Operand)) == 0;

    const auto operandsEqual = [](const Operand& lhs, const Operand& rhs) {
        if (lhs.header == Flag::largeRational && rhs.header == Flag::largeRational)
            return *lhs.data.large == *rhs.data.large;
        else
            return std::memcmp(&lhs, &rhs, sizeof(Operand)) == 0;
    };

    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), operandsEqual);
}

bool sym2::operator!=(ExprView lhs, ExprView rhs)
{
    return !(lhs == rhs);
}

void sym2::swap(Expr& lhs, Expr& rhs)
{
    using std::swap;

    swap(lhs.small, rhs.small);
    swap(lhs.large, rhs.large);
}

sym2::Expr sym2::operator"" _ex(const char* str, std::size_t)
{
    return Expr{str};
}

sym2::Expr sym2::operator"" _ex(unsigned long long n)
{
    if (n > std::numeric_limits<int>::max())
        throw std::domain_error("Integral Expr literals must fit into an int");

    return Expr{static_cast<int>(n)};
}
