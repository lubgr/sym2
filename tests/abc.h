#pragma once

#include "expr.h"

namespace sym2 {
    inline const Expr a{"a"};
    inline const Expr b{"b"};
    inline const Expr c{"c"};
    inline const Expr d{"d"};
    inline const Expr e{"e"};
    inline const Expr f{"f"};
    inline const Expr g{"g"};

    inline const Expr pi{"pi"};
    inline const Expr euler{"euler"};

    struct LeafsForExpr {
        const LeafUnion a{"a"};
        const LeafUnion b{"b"};
        const LeafUnion c{"c"};
        const LeafUnion d{"d"};
        const LeafUnion e{"e"};
        const LeafUnion f{"f"};
        const LeafUnion g{"g"};

        const LeafUnion pi{"pi"};
        const LeafUnion euler{"euler"};
    };

    inline const LeafsForExpr leaf;
}
