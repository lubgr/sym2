
#include <cmath>
#include <memory_resource>
#include "sym2/expr.h"
#include "trigonometric.h"

// Blob must be known in this executable for the pretty printers to work properly. If we don't
// include its definition, there is no way for lldb to retrieve the info on type. While it works
// with the `expr` command, the pretty printer implementations are restricted to the ordinary
// environment of a simple `frame variable` dump. Note that this probably also means that using the
// pretty printers from an application that links against sym2 won't be straightforward.
#include "lowlevel/blob.cpp"

using namespace sym2;

auto check(ExprView<> e)
{
    return e;
}

int main(int, char**)
{
    std::pmr::memory_resource* mr = std::pmr::get_default_resource();
    const Expr si{42, mr};
    const Expr sr{7, 11, mr};
    const Expr li{LargeInt{"8233298749837489247029730960165010709217309487209740928934928"}, mr};
    const Expr otherLi{LargeInt{"2323498273984729837498234029380492839489234902384"}, mr};
    const Expr lr{
      LargeRational{LargeInt{"28937984279872384729834729837498237489237498273489273984723897483"},
        LargeInt{"823329874983748924702973096016501070921730948720974092893492817"}},
      mr};
    const Expr fp{9.876543212345, mr};
    const Expr cst{"pi", 3.142373847234, mr};
    const Expr ss{"abc", mr};
    const Expr ls{"abc_{defy}^g", mr};
    const Expr cx{CompositeType::complexNumber, si, sr, mr};
    const Expr sum{CompositeType::sum, std::span<const ExprView<>>{{si, ss, ls}}, mr};
    const Expr pro{CompositeType::product, std::span<const ExprView<>>{{si, ss, ls, sum}}, mr};
    const Expr pw{CompositeType::power, pro, li, mr};
    const Expr f1{"sin", "a"_ex, std::sin, mr};
    const Expr f2{"atan2", "a"_ex, li, std::atan2, mr};
    const Expr f3{"sin", sr, std::sin, mr};

    for (ExprView<> v : {si, sr, li, otherLi, lr, fp, cx, cst, ss, ls, sum, pro, pw, f1, f2, f3})
        check(v);

    return 0;
}
