
#include <cmath>
#include "expr.h"
#include "trigonometric.h"

using namespace sym2;

auto check(ExprView<> e)
{
    return e;
}

int main(int, char**)
{
    const Expr si{42};
    const Expr sr{7, 11};
    const Expr li{LargeInt{"8233298749837489247029730960165010709217309487209740928934928"}};
    const Expr lr{LargeRational{LargeInt{"28937984279872384729834729837498237489237498273489273984723897483"},
      LargeInt{"823329874983748924702973096016501070921730948720974092893492817"}}};
    const Expr fp{9.876543212345};
    const Expr cst{"pi", 3.142373847234};
    const Expr ss{"abc"};
    const Expr ls{"abc_{defy}^g"};
    const Expr cx{Type::complexNumber, {si, sr}};
    const Expr sum{Type::sum, {si, ss, ls}};
    const Expr pro{Type::product, {si, ss, ls, sum}};
    const Expr pw{Type::power, {pro, li}};
    const Expr f1 = sym2::sin("a"_ex);
    const Expr f2 = sym2::atan2("a"_ex, li);

    for (ExprView<> v : {si, sr, li, lr, fp, cx, cst, ss, ls, sum, pro, pw, f1, f2})
        check(v);

    return 0;
}
