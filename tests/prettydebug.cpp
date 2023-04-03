
#include <cmath>
#include <initializer_list>
#include "sym2/expr.h"
#include "trigonometric.h"

// Blob must be known in this executable for the pretty printers to work properly. If we don't
// include its definition, there is no way for lldb to retrieve the info on type. While it works
// with the `expr` command, the pretty printer implementations are restricted to the ordinary
// environment of a simple `frame variable` dump. Note that this probably also means that using the
// pretty printers from an application that links against sym2 won't be straightforward.
#include "blob.cpp"

using namespace sym2;

auto check(ExprView<> e)
{
    return e;
}

int main(int, char**)
{
    const Expr::allocator_type alloc{};
    const Expr si{42, alloc};
    const Expr sr{7, 11, alloc};
    const Expr li{LargeInt{"8233298749837489247029730960165010709217309487209740928934928"}, alloc};
    const Expr otherLi{LargeInt{"2323498273984729837498234029380492839489234902384"}, alloc};
    const Expr lr{
      LargeRational{LargeInt{"28937984279872384729834729837498237489237498273489273984723897483"},
        LargeInt{"823329874983748924702973096016501070921730948720974092893492817"}},
      alloc};
    const Expr fp{9.876543212345, alloc};
    const Expr cst{"pi", 3.142373847234, alloc};
    const Expr ss{"abc", alloc};
    const Expr ls{"abc_{defy}^g", alloc};
    const Expr cx{CompositeType::complexNumber, si, sr, alloc};
    const Expr sum{CompositeType::sum, std::span<const ExprView<>>{{si, ss, ls}}, alloc};
    const Expr pro{CompositeType::product, std::span<const ExprView<>>{{si, ss, ls, sum}}, alloc};
    const Expr pw{CompositeType::power, pro, li, alloc};
    const Expr f1{"sin", "a"_ex, std::sin, alloc};
    const Expr f2{"atan2", "a"_ex, li, std::atan2, alloc};
    const Expr f3{"sin", sr, std::sin, alloc};

    for (ExprView<> v : std::initializer_list<ExprView<>>{
           si, sr, li, otherLi, lr, fp, cx, cst, ss, ls, sum, pro, pw, f1, f2, f3})
        check(v);

    return 0;
}
