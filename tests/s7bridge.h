#pragma once

#include <memory>
#include <string_view>

struct s7_scheme;
using S7Ptr = struct s7_cell*;

namespace sym2 {
    class Expr;

    class S7Interperter {
      public:
        S7Interperter();
        ~S7Interperter();

        S7Ptr loadFile(const char* path);
        S7Ptr eval(const char* code);
        std::string toString(S7Ptr expression);

      private:
        void replaceOperators();
        void replaceMathFunctions();
        void defineUtilities();
        Expr toExpr(S7Ptr expr);

        template <auto fct, int arity>
        void defineSingleArgFct(std::string_view name);

        std::unique_ptr<s7_scheme, void (*)(s7_scheme*)> sc;
    };

}
