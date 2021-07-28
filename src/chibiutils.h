#pragma once

#include <chibi/sexp.h>

namespace sym2 {
    class PreservedSexp {
      public:
        explicit PreservedSexp(sexp ctx, sexp what);
        PreservedSexp(const PreservedSexp&) = delete;
        PreservedSexp& operator=(const PreservedSexp&) = delete;
        PreservedSexp(PreservedSexp&& other) noexcept;
        PreservedSexp& operator=(PreservedSexp&&) noexcept;
        ~PreservedSexp();

        const sexp& get() const;

      private:
        sexp ctx;
        sexp what;
        sexp_gc_var_t preservation_list;
    };
}
