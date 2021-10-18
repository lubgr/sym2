
#include "chibiconverter.h"
// clang-format off
// This is a chibi-scheme issue; including bignum.h first doesn't compile
#include <chibi/sexp.h>
#include <chibi/bignum.h>
// clang-format on
#include <algorithm>
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

    template <class T, class Container>
    struct ScopedPushPop {
        ScopedPushPop(std::stack<T, Container>& target, PreservedSexp&& toPush)
            : target{target}
        {
            target.push(std::move(toPush));
        }

        ~ScopedPushPop()
        {
            target.pop();
        }

        std::stack<T, Container>& target;
    };

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
        using MappedType = std::variant<CompositeType, UnaryDoubleFctPtr, BinaryDoubleFctPtr>;
        static const boost::container::flat_map<std::string_view, MappedType> known{
          {"+", CompositeType::sum},
          {"*", CompositeType::product},
          {"^", CompositeType::power},
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

sym2::FromChibiToExpr::FromChibiToExpr(sexp ctx)
    : ctx{ctx}
{}

sym2::Expr sym2::FromChibiToExpr::convert(sexp from)
{
    const ScopedPushPop guard{current, preserve(from)};

    if (sexp_numberp(from))
        return numberToExpr(from);
    else if (sexp_symbolp(from)) {
        const auto str = preserve(sexp_symbol_to_string(ctx, from));
        return convert(str);
    } else if (sexp_stringp(from)) {
        return symbolFromString(from);
    } else if (sexp_pairp(from)) {
        return nonEmptyListToExpr(collectItems(from));
    }

    return throwSexp("Can't convert illegal argument to an Expr", from);
}

sym2::Expr sym2::FromChibiToExpr::throwSexp(const char* msg, sexp irritant)
{
    if (irritant == nullptr)
        throw FailedConversionToExpr{msg, ctx, current.top().get()};
    else
        throw FailedConversionToExpr{msg, ctx, irritant};
}

sym2::PreservedSexp sym2::FromChibiToExpr::preserve(sexp what)
{
    return PreservedSexp{ctx, what};
}

sym2::Expr sym2::FromChibiToExpr::convert(const PreservedSexp& from)
{
    return convert(from.get());
}

sym2::Expr sym2::FromChibiToExpr::numberToExpr(sexp from)
{
    assert(sexp_numberp(from));

    if (sexp_flonump(from))
        /* This must come before the integer branch, as floating point numbers that truncate to integers without
         * loss also qualify as integers. */
        return Expr{sexp_flonum_value(from)};
    else if (sexp_exact_integerp(from))
        return Expr{LargeIntRef{extractSmallOrLargeInt(from)}};
    else if (sexp_ratiop(from)) {
        const auto num = preserve(sexp_ratio_numerator(from));
        const auto denom = preserve(sexp_ratio_denominator(from));
        const LargeRational n{extractSmallOrLargeInt(num.get()), extractSmallOrLargeInt(denom.get())};

        return Expr{LargeRationalRef{n}};
    } else if (sexp_complexp(from)) {
        const auto realPart = preserve(sexp_complex_real(from));
        const auto imagPart = preserve(sexp_complex_imag(from));
        const std::array<Expr, 2> operands{{convert(realPart), convert(imagPart)}};

        return Expr{CompositeType::complexNumber, operands};
    }

    return throwSexp("Can't convert number to a numeric Expr", from);
}

sym2::LargeInt sym2::FromChibiToExpr::extractSmallOrLargeInt(sexp integer)
{
    if (sexp_fixnump(integer)) {
        return LargeInt{sexp_unbox_fixnum(integer)};
    } else if (sexp_bignump(integer))
        return extractLargeInt(integer);

    throwSexp("Can't extract integer from non-integer chibi type", integer);

    return {};
}

sym2::Expr sym2::FromChibiToExpr::symbolFromString(sexp str)
{
    const auto length = sexp_string_size(str);
    const char* buffer = sexp_string_data(str);
    std::string_view name{buffer, length};
    const std::size_t colon = name.find(':');

    if (colon == std::string_view::npos)
        return Expr{name};

    const SymbolFlag flag = symbolConstraintsOrThrow(name.substr(colon + 1));
    return Expr{name.substr(0, colon), flag};
}

sym2::SymbolFlag sym2::FromChibiToExpr::symbolConstraintsOrThrow(std::string_view flags)
{
    if (flags == "r")
        return SymbolFlag::real;
    else if (flags == "+")
        return SymbolFlag::positive;
    else if (flags == "+r" || flags == "r+")
        return SymbolFlag::positiveReal;

    const auto msg = std::string{"Invalid symbol constraints: "}.append(flags);

    throwSexp(msg.c_str());

    return SymbolFlag::real;
}

std::vector<sym2::PreservedSexp> sym2::FromChibiToExpr::collectItems(sexp list)
{
    std::vector<sym2::PreservedSexp> result;

    while (!sexp_nullp(list)) {
        result.push_back(preserve(sexp_car(list)));
        list = sexp_cdr(list);
    }

    return result;
}

sym2::Expr sym2::FromChibiToExpr::nonEmptyListToExpr(std::span<const PreservedSexp> from)
{
    const PreservedSexp& type = from.front();
    const std::span<const PreservedSexp> ops = from.subspan(1);

    if (!sexp_symbolp(type.get()))
        throwSexp("Composite/constant starting with non-symbol", type.get());
    else if (ops.empty())
        throwSexp("Composite/constant without operands");

    const auto str = preserve(sexp_symbol_to_string(ctx, type.get()));
    const std::string_view name{sexp_string_data(str.get()), sexp_string_size(str.get())};
    const auto& lookup = knownCompositeOperators();
    const auto dispatch = overloaded{[&ops, this](CompositeType kind) { return compositeToExpr(kind, ops); },
      [&ops, &name, this](auto fct) { return compositeToExpr(name, fct, ops); }};

    if (lookup.contains(name))
        return std::visit(dispatch, lookup.at(name));
    else
        return attemptConstantToExpr(name, ops);
}

sym2::Expr sym2::FromChibiToExpr::compositeToExpr(CompositeType kind, std::span<const PreservedSexp> operands)
{
    std::vector<Expr> converted;

    converted.reserve(operands.size());

    std::transform(operands.begin(), operands.end(), std::back_inserter(converted),
      [this](const PreservedSexp& expr) { return convert(expr); });

    return Expr{kind, converted};
}

sym2::Expr sym2::FromChibiToExpr::compositeToExpr(
  std::string_view name, UnaryDoubleFctPtr fct, std::span<const PreservedSexp> operands)
{
    if (operands.size() != 1)
        throwSexp("Unary functions must have exactly one operand");

    return Expr{name, convert(operands.front()), fct};
}

sym2::Expr sym2::FromChibiToExpr::compositeToExpr(
  std::string_view name, BinaryDoubleFctPtr fct, std::span<const PreservedSexp> operands)
{
    if (operands.size() != 2)
        throwSexp("Binary functions must have exactly two operands");

    return Expr{name, convert(operands.front()), convert(operands.back()), fct};
}

sym2::Expr sym2::FromChibiToExpr::attemptConstantToExpr(std::string_view name, std::span<const PreservedSexp> operands)
{
    if (operands.size() != 1)
        throwSexp("Can't convert expression");
    else if (!sexp_flonump(operands.front().get()))
        throwSexp("Constant must have a floating point argument");

    const double value = sexp_flonum_value(operands.front().get());

    return Expr{name, value};
}

sym2::FromExprToChibi::FromExprToChibi(sexp ctx)
    : ctx{ctx}
{}

sexp sym2::FromExprToChibi::convert(ExprView<> from)
{
    if (is<symbol>(from))
        return symbolFrom(tag<symbol>(from));
    else if (is<number>(from))
        return dispatchOver(tag<number>(from));
    else if (is<constant>(from))
        return symbolDoubleListFrom(tag<constant>(from));
    else if (is<function>(from))
        return compositeFrom(tag<function>(from));
    else
        return compositeFrom(tag < sum || product || power > (from));

    throw FailedConversionToSexp{"Can't convert to chibi type", Expr{from}};
}

sym2::PreservedSexp sym2::FromExprToChibi::preserve(sexp what)
{
    return PreservedSexp{ctx, what};
}

sexp sym2::FromExprToChibi::symbolFrom(ExprView<symbol> symbol)
{
    std::string name{get<std::string_view>(symbol)};

    if (is < positive || realDomain > (symbol))
        name.push_back(':');
    if (is<realDomain>(symbol))
        name.push_back('r');
    if (is<positive>(symbol))
        name.push_back('+');

    return chibiSymbolFromString(name).get();
}

sym2::PreservedSexp sym2::FromExprToChibi::chibiSymbolFromNamedExpr(ExprView<> named)
{
    return chibiSymbolFromString(get<std::string_view>(named));
}

sym2::PreservedSexp sym2::FromExprToChibi::chibiSymbolFromString(std::string_view name)
{
    const auto str = preserve(sexp_c_string(ctx, name.data(), name.size()));

    return preserve(sexp_string_to_symbol(ctx, str.get()));
}

sexp sym2::FromExprToChibi::dispatchOver(ExprView<number> from)
{
    if (is < small && integer > (from))
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
        const auto nonNormalized =
          preserve(sexp_make_ratio(ctx, serializeLargeInt(num).get(), serializeLargeInt(denom).get()));

        return sexp_ratio_normalize(ctx, nonNormalized.get(), SEXP_VOID);
    } else if (is<floatingPoint>(from))
        return sexp_make_flonum(ctx, get<double>(from));
    else if (is<complexDomain>(from)) {
        const auto realPart = preserve(convert(real(from)));
        const auto imagPart = preserve(convert(imag(from)));

        return sexp_make_complex(ctx, realPart.get(), imagPart.get());
    }

    throw FailedConversionToSexp{"Unknown numeric type", Expr{from}};
}

sym2::PreservedSexp sym2::FromExprToChibi::serializeLargeInt(const LargeInt& n)
{
    auto result = preserve(sexp_make_bignum(ctx, n.backend().size()));

    sexp_uint_t* data = sexp_bignum_data(result.get());
    export_bits(n, data, 8 * sizeof(sexp_uint_t), false);
    sexp_bignum_sign(result.get()) = static_cast<signed char>(n.sign());

    return result;
}

sexp sym2::FromExprToChibi::symbolDoubleListFrom(ExprView<constant> from)
{
    const auto symbol = chibiSymbolFromNamedExpr(from);
    const auto value = get<double>(from);
    const auto flonum = preserve(sexp_make_flonum(ctx, value));

    return sexp_list2(ctx, symbol.get(), flonum.get());
}

sexp sym2::FromExprToChibi::compositeFrom(ExprView<function> fct)
{
    const auto symbol = chibiSymbolFromNamedExpr(fct);

    return serializeListWithLeadingSymbol(symbol, OperandsView::operandsOf(fct));
}

sexp sym2::FromExprToChibi::serializeListWithLeadingSymbol(const PreservedSexp& identifier, OperandsView rest)
{
    PreservedSexp result = preserve(sexp_list1(ctx, identifier.get()));

    for (const ExprView<> operand : rest) {
        const PreservedSexp op = preserve(convert(operand));
        result = preserve(sexp_append2(ctx, result.get(), sexp_list1(ctx, op.get())));
    }

    return result.get();
}

sym2::PreservedSexp sym2::FromExprToChibi::leadingSymbolForComposite(ExprView<sum || product || power> composite)
{
    const auto& lookup = knownCompositeOperators();
    const auto compositeType = [](ExprView<sum || product || power> composite) {
        if (is<sum>(composite))
            return CompositeType::sum;
        else if (is<product>(composite))
            return CompositeType::product;
        else
            return CompositeType::power;
    };

    for (const auto& [identifier, mapped] : lookup)
        if (const CompositeType* kind = std::get_if<CompositeType>(&mapped);
            kind && *kind == compositeType(composite)) {
            return chibiSymbolFromString(identifier);
        }

    throw FailedConversionToSexp{"Unknown composite type", Expr{composite}};
}

sexp sym2::FromExprToChibi::compositeFrom(ExprView<sum || product || power> composite)
{
    const auto symbol = leadingSymbolForComposite(composite);

    return serializeListWithLeadingSymbol(symbol, OperandsView::operandsOf(composite));
}

std::vector<sym2::Expr> sym2::convertList(sexp ctx, sexp list)
{
    std::vector<Expr> result;
    FromChibiToExpr individual{ctx};

    while (!sexp_nullp(list)) {
        const PreservedSexp item{ctx, sexp_car(list)};
        result.push_back(individual.convert(item.get()));
        list = sexp_cdr(list);
    }

    return result;
}
