
#include "expr.h"

using namespace sym2;

auto check(ExprView e)
{
    return e;
}

int main(int, char**)
{
    const Expr si{42};
    const Expr sr{7, 11};
    const Expr li{Int{"8233298749837489247029730960165010709217309487209740928934928"}};
    const Expr lr{Rational{Int{"28937984279872384729834729837498237489237498273489273984723897483"},
      Int{"823329874983748924702973096016501070921730948720974092893492817"}}};
    const Expr fp{9.876543212345};
    const Expr cst{"pi", 3.142373847234};
    const Expr ss{"abc"};
    const Expr ls{"abc_{defy}^g"};
    const Expr sum{Type::sum, {si, ss, ls}};
    const Expr pro{Type::product, {si, ss, ls, sum}};
    const Expr pw{Type::power, {pro, li}};

    for (ExprView v : {si, sr, li, lr, fp, cst, ss, ls, sum, pro, pw})
        check(v);

    return 0;
}
