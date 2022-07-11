
#include <cstdlib>
#include <chibi/eval.h>

int main(int, char**) {
  // Standard initialisation procedure from the chibi documentation. This doesn't do anything
  // useful, but will make sure headers and library are found.
  sexp_scheme_init();

  sexp ctx;
  ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);

  sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
  sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 1);

  sexp_destroy_context(ctx);

  return EXIT_SUCCESS;
}
