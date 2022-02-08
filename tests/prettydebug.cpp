
#include <cmath>
#include "expr.h"
#include "exprliteral.h"
#include "trigonometric.h"

// Blob must be known in this executable for the pretty printers to work properly. If we don't
// include its definition, there is no way for lldb to retrieve the info on type. While it works
// with the `expr` command, the pretty printer implementations are restricted to the ordinary
// environment of a simple `frame variable` dump. Note that this probably also means that using the
// pretty printers from an application that links against sym2 won't be straightforward.
#include "lowlevel/blob.h"

using namespace sym2;

auto check(ExprView<> e)
{
    return e;
}

int main(int, char**)
{
    const Expr si{42};
    const Expr sr{7, 11};
    const Expr li{
      LargeIntRef{LargeInt{"8233298749837489247029730960165010709217309487209740928934928"}}};
    const Expr otherLi{LargeIntRef{LargeInt{"2323498273984729837498234029380492839489234902384"}}};
    const Expr lr{LargeRationalRef{
      LargeRational{LargeInt{"28937984279872384729834729837498237489237498273489273984723897483"},
        LargeInt{"823329874983748924702973096016501070921730948720974092893492817"}}}};
    const Expr fp{9.876543212345};
    const Expr cst{"pi", 3.142373847234};
    const Expr ss{"abc"};
    const Expr ls{"abc_{defy}^g"};
    const Expr cx{CompositeType::complexNumber, {si, sr}};
    const Expr sum{CompositeType::sum, {si, ss, ls}};
    const Expr pro{CompositeType::product, {si, ss, ls, sum}};
    const Expr pw{CompositeType::power, {pro, li}};
    const Expr f1 = sym2::sin("a"_ex);
    const Expr f2 = sym2::atan2("a"_ex, li);
    const Expr f3 = sym2::sin(sr);

    for (ExprView<> v : {si, sr, li, otherLi, lr, fp, cx, cst, ss, ls, sum, pro, pw, f1, f2, f3})
        check(v);

    return 0;
}
