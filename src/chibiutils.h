#pragma once

#include <chibi/sexp.h>
#include <memory>

namespace sym2 {
    class PreservedSexp {
      public:
        explicit PreservedSexp(sexp ctx, sexp what);

        const sexp& get() const;

      private:
        struct Payload {
            Payload(sexp ctx, sexp what);
            ~Payload();

            sexp ctx;
            sexp what;
            sexp_gc_var_t preservation_list;
        };

        std::unique_ptr<Payload> handle;
    };
}
