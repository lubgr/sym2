
#include "s7bridge.h"
#include <cassert>
#include <s7.h>
#include "autosimpl.h"
#include "s7converter.h"
#include "trigonometric.h"

namespace sym2 {
    namespace {
        template <auto fct, std::size_t buffer = 10>
        s7_pointer nArySmallVecArgFct(s7_scheme* sc, s7_pointer args)
        {
            S7Converter conv{*sc};

            const auto operands = conv.listToExprVec(args);

            using ViewVec = SmallVec<ExprView<>, decltype(operands)::static_capacity>;

            const auto result = fct(ViewVec{operands.cbegin(), operands.cend()});

            return conv.fromExpr(result);
        }

        template <auto fct>
        s7_pointer singleArgFct(s7_scheme* sc, s7_pointer args)
        {
            S7Converter conv{*sc};

            const Expr arg = conv.listToSingleExpr(args);
            const auto result = fct(arg);

            return conv.fromExpr(result);
        }

        template <auto fct>
        s7_pointer twoArgFct(s7_scheme* sc, s7_pointer args)
        {
            S7Converter conv{*sc};

            const auto operands = conv.listToExprVec(args);
            const auto result = fct(operands.front(), operands.back());

            return conv.fromExpr(result);
        }
    }
}

sym2::S7Interperter::S7Interperter()
    : sc{s7_init(), &s7_free}
{
    assert(sc.get() != nullptr);

    replaceOperators();
    replaceMathFunctions();
}

sym2::S7Interperter::~S7Interperter() = default;

S7Ptr sym2::S7Interperter::loadFile(const char* path)
{
    return s7_load(sc.get(), path);
}

S7Ptr sym2::S7Interperter::eval(const char* code)
{
    return s7_eval_c_string(sc.get(), code);
}

std::string sym2::S7Interperter::toString(S7Ptr expression)
{
    return s7_object_to_c_string(sc.get(), expression);
}

void sym2::S7Interperter::replaceOperators()
{
    s7_define_function(sc.get(), "+", nArySmallVecArgFct<autoSumOf>, 2, 0, true, "(+ ...) sym2 addition");
    s7_define_function(sc.get(), "*", nArySmallVecArgFct<autoProductOf>, 2, 0, true, "(* ...) sym2 multiplication");
    s7_define_function(sc.get(), "^", twoArgFct<autoPower>, 2, 0, false, "(* ...) sym2 power");
}

void sym2::S7Interperter::replaceMathFunctions()
{
    defineSingleArgFct<sym2::sin, 1>("sin");
    defineSingleArgFct<sym2::cos, 1>("cos");
    defineSingleArgFct<sym2::tan, 1>("tan");
    defineSingleArgFct<sym2::asin, 1>("asin");
    defineSingleArgFct<sym2::acos, 1>("acos");
    defineSingleArgFct<sym2::atan, 1>("atan");

    s7_define_function(sc.get(), "atan2", twoArgFct<sym2::atan2>, 2, 0, false, "(atan2 ...) sym2 atan2");
}

template <auto fct, int arity>
void sym2::S7Interperter::defineSingleArgFct(std::string_view name)
{
    const std::string terminated{name};
    const std::string doc = "(" + terminated + " ...) sym2 " + terminated;

    s7_define_function(sc.get(), terminated.c_str(), singleArgFct<fct>, arity, 0, false, doc.c_str());
}
