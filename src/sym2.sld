
(define-library (sym2)
  (import (scheme base) (scheme write))
  (export
    roundtrip
    auto+
    auto*
    auto^
    order-lt
    split-const-term)
  (include "sym2chibi.scm")
  (include-shared "sym2chibi"))
