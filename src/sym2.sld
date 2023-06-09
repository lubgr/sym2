
(define-library (sym2)
  (import (scheme base) (scheme write))
  (export
    expr->string
    auto+
    auto*
    auto^
    contains
    order-lt
    min-degree
    degree
    roundtrip
    sign
    split-const-term)
  (include "sym2chibi.scm")
  (include-shared "sym2chibi"))
