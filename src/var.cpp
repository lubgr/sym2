
#include <limits>
#include <stdexcept>
#include "allocator.h"
#include "autosimpl.h"
#include "expr.h"
#include "prettyprinter.h"
#include "sym2/sym2.h"

sym2::Var::Var()
    : Var{0}
{}

sym2::Var::Var(std::int32_t n)
    : Var{n, 1}
{}

sym2::Var::Var(double n)
{
    initialize(n);
}

sym2::Var::Var(std::int32_t num, std::int32_t denom)
{
    initialize(num, denom);
}

sym2::Var::Var(std::string_view symbol)
{
    initialize(symbol);
}

sym2::Var::Var(std::string_view symbol, SymbolFlag constraint)
{
    initialize(symbol, constraint);
}

sym2::Var::Var(std::string_view constant, double value)
{
    initialize(constant, value);
}

sym2::Var::Var(std::string_view function, const Var& arg, double (*eval)(double))
{
    initialize(function, arg.get(), eval);
}

sym2::Var::Var(
  std::string_view function, const Var& arg1, const Var& arg2, double (*eval)(double, double))
{
    initialize(function, arg1.get(), arg2.get(), eval);
}

sym2::Var::Var(const Var& other)
{
    initialize(other.get());
}

sym2::Var& sym2::Var::operator=(const Var& other)
{
    buffer = other.buffer;

    return *this;
}

sym2::Var::Var(Var&& other) noexcept
{
    initialize(std::move(other.get()));
}

sym2::Var& sym2::Var::operator=(Var&& other) noexcept
{
    buffer = std::move(other.buffer);

    return *this;
}

sym2::Var::~Var() noexcept
{
    get().~Expr();
}

template <class... Args>
void sym2::Var::initialize(Args&&... ctorArgs)
{
    static_assert(sizeof(buffer) == sizeof(Expr));
    static_assert(alignof(decltype(buffer)) == alignof(Expr));

    new (&buffer) Expr{std::forward<Args>(ctorArgs)...};
}

sym2::Var& sym2::Var::operator+=(const Var& rhs)
{
    get() = autoSum(get(), rhs.get());

    return *this;
}

sym2::Var& sym2::Var::operator-=(const Var& rhs)
{
    get() = autoSum(get(), autoMinus(rhs.get()));

    return *this;
}

sym2::Var& sym2::Var::operator*=(const Var& rhs)
{
    get() = autoProduct(get(), rhs.get());

    return *this;
}

sym2::Var& sym2::Var::operator/=(const Var& rhs)
{
    get() = autoProduct(get(), autoOneOver(rhs.get()));

    return *this;
}

const sym2::Var& sym2::Var::operator+() const
{
    return *this;
}

sym2::Var sym2::Var::operator-() const
{
    static const Var minusOne{-1};
    Var result{*this};

    result *= minusOne;

    return result;
}

sym2::Var sym2::Var::internal(const Expr& backend)
{
    Var result;

    result.initialize(backend);

    return result;
}

sym2::Expr& sym2::Var::get()
{
    return reinterpret_cast<Expr&>(buffer);
}

const sym2::Expr& sym2::Var::get() const
{
    return reinterpret_cast<const Expr&>(buffer);
}

bool sym2::operator==(const Var& lhs, const Var& rhs)
{
    return ExprView<>{lhs.get()} == ExprView<>{rhs.get()};
}

bool sym2::operator!=(const Var& lhs, const Var& rhs)
{
    return !(lhs == rhs);
}

sym2::Var sym2::operator+(Var lhs, const Var& rhs)
{
    lhs += rhs;

    return lhs;
}

sym2::Var sym2::operator-(Var lhs, const Var& rhs)
{
    lhs -= rhs;

    return lhs;
}

sym2::Var sym2::operator*(Var lhs, const Var& rhs)
{
    lhs *= rhs;

    return lhs;
}

sym2::Var sym2::operator/(Var lhs, const Var& rhs)
{
    lhs /= rhs;

    return lhs;
}

sym2::Var sym2::literals::operator"" _v(const char* str, std::size_t)
{
    return Var{str};
}

sym2::Var sym2::literals::operator"" _v(unsigned long long n)
{
    if (n > std::numeric_limits<std::int32_t>::max())
        throw std::domain_error("Integral Var literals must fit into an int");

    return Var{static_cast<std::int32_t>(n)};
}

sym2::Var sym2::literals::operator"" _v(long double n)
{
    return Var{static_cast<double>(n)};
}

std::ostream& sym2::operator<<(std::ostream& os, const Var& rhs)
{
    PlaintextPrintEngine engine{os};
    auto [_, resource] = monotonicStackPmrResource<ByteSize{200}>();
    PrettyPrinter printer{engine, PrettyPrinter::PowerAsFraction::asFraction, &resource};

    printer.print(rhs.get());

    return os;
}
