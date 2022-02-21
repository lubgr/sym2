
#include "exprliteral.h"
#include <limits>
#include <stdexcept>
#include "blob.h"

sym2::ExprLiteral::ExprLiteral(std::string_view symbol)
{
    reinterpret_cast<Blob&>(*blob.data()) = symbolBlob(symbol);
}

sym2::ExprLiteral::ExprLiteral(std::int32_t n)
{
    reinterpret_cast<Blob&>(*blob.data()) = smallIntBlob(n);
}

sym2::ExprLiteral::ExprLiteral(std::int32_t num, std::int32_t denom)
{
    reinterpret_cast<Blob&>(*blob.data()) = smallRationalBlob(num, denom);
}

sym2::ExprLiteral::ExprLiteral(double n)
{
    reinterpret_cast<Blob&>(*blob.data()) = floatingPointBlob(n);
}

sym2::ExprView<> sym2::ExprLiteral::view() const
{
    static_assert(sizeof(Blob) == sizeof(blob));
    static_assert(alignof(Blob) == alignof(decltype(blob.data())));

    return ExprView<>{reinterpret_cast<const Blob*>(blob.data()), 1};
}

sym2::ExprLiteral sym2::operator"" _ex(const char* str, std::size_t)
{
    return ExprLiteral{std::string_view{str}};
}

sym2::ExprLiteral sym2::operator"" _ex(unsigned long long n)
{
    if (n > std::numeric_limits<std::int32_t>::max())
        throw std::domain_error("Integral Expr literals must fit into an 32bit int");

    return ExprLiteral{static_cast<std::int32_t>(n)};
}

sym2::ExprLiteral sym2::operator"" _ex(long double n)
{
    return ExprLiteral{static_cast<double>(n)};
}
