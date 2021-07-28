
#include "chibiconverter.h"
// clang-format off
// This is a chibi-scheme issue; including bignum.h first doesn't compile
#include <chibi/sexp.h>
#include <chibi/bignum.h>
// clang-format on
#include <array>
#include <boost/container/flat_map.hpp>
#include <cassert>
#include <cmath>
#include <variant>
#include "chibiutils.h"
#include "get.h"
#include "query.h"

namespace sym2 {
    template <class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };

    template <class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

    namespace {
        LargeInt extractLargeInt(sexp bignum)
        {
            signed char sign = sexp_bignum_sign(bignum);
            const sexp_uint_t length = sexp_bignum_length(bignum);
            const sexp_uint_t* data = sexp_bignum_data(bignum);
            LargeInt result;

            import_bits(result, data, data + length, 8 * sizeof(sexp_uint_t), false);

            if (sign == -1)
                result *= -1;

            return result;
        }
    }

    const auto& knownCompositeOperators()
    {
        using MappedType = std::variant<Type, UnaryDoubleFctPtr, BinaryDoubleFctPtr>;
        static const boost::container::flat_map<std::string_view, MappedType> known{
          {"+", Type::sum},
          {"*", Type::product},
          {"^", Type::power},
          {"atan2", BinaryDoubleFctPtr{&std::atan2}},
          {"sin", UnaryDoubleFctPtr{&std::sin}},
          {"cos", UnaryDoubleFctPtr{&std::cos}},
          {"tan", UnaryDoubleFctPtr{&std::tan}},
          {"asin", UnaryDoubleFctPtr{&std::asin}},
          {"acos", UnaryDoubleFctPtr{&std::acos}},
          {"atan", UnaryDoubleFctPtr{&std::atan}},
          {"log", UnaryDoubleFctPtr{&std::log}},
          {"log2", UnaryDoubleFctPtr{&std::log2}},
          {"log10", UnaryDoubleFctPtr{&std::log10}},
        };

        return known;
    }
}

sym2::ChibiConverter::ChibiConverter(sexp ctx)
    : ctx{ctx}
{}

sym2::Expr sym2::ChibiConverter::toExpr(sexp from)
{
    if (sexp_numberp(from))
        return numberToExpr(from);
    else if (sexp_symbolp(from)) {
        const PreservedSexp str{ctx, sexp_symbol_to_string(ctx, from)};
        return toExpr(str);
    } else if (sexp_stringp(from)) {
        const auto length = sexp_string_size(from);
        const char* str = sexp_string_data(from);

        return Expr{std::string_view{str, length}};
    } else if (sexp_pairp(from)) {
        return nonEmptyListToExpr(collectItems(from));
    }

    throw FailedConversionToExpr{"Can't convert illegal argument to an Expr", ctx, from};
}

sym2::Expr sym2::ChibiConverter::toExpr(const PreservedSexp& from)
{
    return toExpr(from.get());
}

sym2::Expr sym2::ChibiConverter::numberToExpr(sexp from)
{
    assert(sexp_numberp(from));

    if (sexp_flonump(from))
        /* This must come before the integer branch, as floating point numbers that truncate to integers without
         * loss also qualify as integers. */
        return Expr{sexp_flonum_value(from)};
    else if (sexp_exact_integerp(from))
        return Expr{extractSmallOrLargeInt(from)};
    else if (sexp_ratiop(from)) {
        const PreservedSexp num{ctx, sexp_ratio_numerator(from)};
        const PreservedSexp denom{ctx, sexp_ratio_denominator(from)};
        const LargeRational n{extractSmallOrLargeInt(num.get()), extractSmallOrLargeInt(denom.get())};

        return Expr{n};
    } else if (sexp_complexp(from)) {
        const PreservedSexp real{ctx, sexp_complex_real(from)};
        const PreservedSexp imag{ctx, sexp_complex_imag(from)};
        const std::array<Expr, 2> operands{{toExpr(real), toExpr(imag)}};

        return Expr{Type::complexNumber, operands};
    }

    throw FailedConversionToExpr{"Can't convert number to a numeric Expr", ctx, from};
}

sym2::LargeInt sym2::ChibiConverter::extractSmallOrLargeInt(sexp integer)
{
    if (sexp_fixnump(integer)) {
        return LargeInt{sexp_unbox_fixnum(integer)};
    } else if (sexp_bignump(integer))
        return extractLargeInt(integer);
    else
        throw FailedConversionToExpr{"Can't extract integer from non-integer chibi type", ctx, integer};
}

std::vector<sym2::PreservedSexp> sym2::ChibiConverter::collectItems(sexp list)
{
    std::vector<sym2::PreservedSexp> result;

    while (!sexp_nullp(list)) {
        result.emplace_back(ctx, sexp_car(list));
        list = sexp_cdr(list);
    }

    return result;
}

sym2::Expr sym2::ChibiConverter::nonEmptyListToExpr(std::span<const PreservedSexp> from)
{
    const PreservedSexp& type = from.front();
    const std::span<const PreservedSexp> ops = from.subspan(1);

    if (!sexp_symbolp(type.get()))
        throw FailedConversionToExpr{"Composite/constant starting with non-symbol", ctx, type.get()};
    else if (ops.empty())
        throw FailedConversionToExpr{"Composite/constant without operands", ctx, type.get()};

    const PreservedSexp str{ctx, sexp_symbol_to_string(ctx, type.get())};
    const std::string_view name{sexp_string_data(str.get()), sexp_string_size(str.get())};
    const auto& lookup = knownCompositeOperators();
    const auto dispatch = overloaded{[&ops, this](Type kind) { return compositeToExpr(kind, ops); },
      [&ops, &name, this](auto fct) { return compositeToExpr(name, fct, ops); }};

    if (lookup.contains(name))
        return std::visit(dispatch, lookup.at(name));
    else
        return constantToExpr(name, ops);
}

sym2::Expr sym2::ChibiConverter::compositeToExpr(Type kind, std::span<const PreservedSexp> operands)
{
    // TODO
    return Expr{0};
}

sym2::Expr sym2::ChibiConverter::compositeToExpr(
  std::string_view name, UnaryDoubleFctPtr fct, std::span<const PreservedSexp> operands)
{
    if (operands.size() != 1)
        throw FailedConversionToExpr{"Unary functions must have exactly one operand", ctx, SEXP_FALSE};

    return Expr{name, toExpr(operands.front()), fct};
}

sym2::Expr sym2::ChibiConverter::compositeToExpr(
  std::string_view name, BinaryDoubleFctPtr fct, std::span<const PreservedSexp> operands)
{
    if (operands.size() != 2)
        throw FailedConversionToExpr{"Binary functions must have exactly two operands", ctx, SEXP_FALSE};

    return Expr{name, toExpr(operands.front()), toExpr(operands.back()), fct};
}

sym2::Expr sym2::ChibiConverter::constantToExpr(std::string_view name, std::span<const PreservedSexp> operands)
{
    if (operands.size() != 1 || !sexp_flonump(operands.front().get()))
        throw FailedConversionToExpr{"Constant must have exactly one floating point argument", ctx, SEXP_FALSE};

    const double value = sexp_flonum_value(operands.front().get());

    return Expr{name, value};
}

sexp sym2::ChibiConverter::fromExpr(ExprView<> from)
{
    if (is<symbol>(from)) {
        const auto name = get<std::string_view>(from);
        const PreservedSexp str{ctx, sexp_c_string(ctx, name.data(), name.size())};
        const PreservedSexp symbol{ctx, sexp_string_to_symbol(ctx, str.get())};

        return symbol.get();
    } else if (is < small && integer > (from))
        return sexp_make_fixnum(get<std::int32_t>(from));
    else if (is < large && integer > (from)) {
        const auto n = get<LargeInt>(from);
        return serializeLargeInt(n).get();
    } else if (is < small && rational > (from)) {
        const auto [num, denom] = get<SmallRational>(from);
        return sexp_make_ratio(ctx, sexp_make_fixnum(num), sexp_make_fixnum(denom));
    } else if (is < large && rational > (from)) {
        const LargeRational r = get<LargeRational>(from);
        const LargeInt& num = numerator(r);
        const LargeInt& denom = denominator(r);
        const PreservedSexp nonNormalized{
          ctx, sexp_make_ratio(ctx, serializeLargeInt(num).get(), serializeLargeInt(denom).get())};

        return sexp_ratio_normalize(ctx, nonNormalized.get(), SEXP_VOID);
    } else if (is<floatingPoint>(from))
        return sexp_make_flonum(ctx, get<double>(from));
    else if (is < number && complexDomain > (from)) {
        const PreservedSexp real{ctx, fromExpr(first(from))};
        const PreservedSexp imag{ctx, fromExpr(second(from))};

        return sexp_make_complex(ctx, real.get(), imag.get());
    } else if (is<function>(from)) {
        const std::string_view name = get<std::string_view>(from);

        const PreservedSexp str{ctx, sexp_c_string(ctx, name.data(), name.size())};
        const PreservedSexp symbol{ctx, sexp_string_to_symbol(ctx, str.get())};

        return symbol.get();
    }

    throw FailedConversionToSexp{"Can't convert to chibi type", Expr{from}};
}

sym2::PreservedSexp sym2::ChibiConverter::serializeLargeInt(const LargeInt& n)
{
    PreservedSexp result{ctx, sexp_make_bignum(ctx, n.backend().size())};

    sexp_uint_t* data = sexp_bignum_data(result.get());
    export_bits(n, data, 8 * sizeof(sexp_uint_t), false);
    sexp_bignum_sign(result.get()) = static_cast<signed char>(n.sign());

    return result;
}
