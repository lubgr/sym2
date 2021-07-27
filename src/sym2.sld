
(define-library (sym2)
  (import (scheme base) (scheme write))
  (export
    roundtrip
    auto+
    auto*)
  (include "sym2chibi.scm")
  (include-shared "sym2chibi"))
