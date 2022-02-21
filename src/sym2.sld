
(define-library (sym2)
  (import (scheme base) (scheme write))
  (export
    expr->string
    auto+
    auto*
    auto^
    order-lt
    roundtrip
    sign
    split-const-term)
  (include "sym2chibi.scm")
  (include-shared "sym2chibi"))
