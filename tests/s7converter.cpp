
#include "s7converter.h"
#include <cassert>
#include <s7.h>
#include <stdexcept>
#include "get.h"
#include "operands.h"
#include "query.h"

namespace sym2 {
    namespace {
        std::string_view leadingSymbol(ExprView<> e)
        {
            switch (type(e)) {
                case Type::sum:
                    return "+";
                case Type::product:
                    return "*";
                case Type::power:
                    return "^";
                case Type::function:
                    return get<std::string_view>(e);
                default:
                    return "";
            }
        }
    }
}

sym2::S7Converter::S7Converter(s7_scheme& sc)
    : sc{sc}
{}

S7Ptr sym2::S7Converter::fromExpr(ExprView<> expression)
{
    if (is<scalar>(expression))
        return fromExpr(tag<scalar>(expression));
    else
        return fromExpr(tag<!scalar>(expression));
}

S7Ptr sym2::S7Converter::fromExpr(ExprView<!scalar> expression)
{
    const std::string_view typeId = leadingSymbol(expression);
    auto result = s7_make_list(&sc, 1, s7_make_symbol(&sc, typeId.data()));

    for (ExprView<> op : OperandsView{expression})
        result = s7_cons(&sc, fromExpr(op), result);

    return s7_reverse(&sc, result);
}

S7Ptr sym2::S7Converter::fromExpr(ExprView<scalar> expression)
{
    if (is<symbol>(expression)) {
        const auto name = std::string{get<std::string_view>(expression)};

        return s7_make_symbol(&sc, name.c_str());
    } else if (is < small && integer > (expression))
        return s7_make_integer(&sc, get<std::int32_t>(expression));
    else if (is<floatingPoint>(expression))
        return s7_make_real(&sc, get<double>(expression));

    return s7_nil(&sc);
}

sym2::Expr sym2::S7Converter::toExpr(S7Ptr expression)
{
    if (s7_is_integer(expression)) {
        const s7_int value = s7_integer(expression);

        if (value > std::numeric_limits<std::int32_t>::max())
            throw std::domain_error{"Can only handle 4byte integral values"};

        return Expr{static_cast<std::int32_t>(value)};
    } else if (s7_is_symbol(expression))
        return Expr{s7_symbol_name(expression)};
    else if (s7_is_string(expression))
        return Expr{s7_string(expression)};
    else if (s7_is_real(expression))
        return Expr{s7_real(expression)};
    else if (s7_is_boolean(expression))
        throw std::invalid_argument{"Can't convert boolean to an Expr"};
    else if (s7_is_null(&sc, expression))
        throw std::invalid_argument{"Can't convert '() to an Expr"};

    throw std::invalid_argument{"Can't convert illegal argument to an Expr"};
}

sym2::Expr sym2::S7Converter::listToSingleExpr(S7Ptr list)
{
    assert(s7_is_list(&sc, list));

    return toExpr(s7_car(list));
}

sym2::SmallVec<sym2::Expr, 10> sym2::S7Converter::listToExprVec(S7Ptr list)
{
    SmallVec<Expr, 10> result;

    if (!s7_is_pair(list))
        return result;

    for (s7_pointer item = s7_car(list); s7_is_list(&sc, s7_cdr(list)); list = s7_cdr(list), item = s7_car(list))
        result.push_back(toExpr(item));

    return result;
}
