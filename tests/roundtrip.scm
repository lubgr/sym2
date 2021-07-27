
(import (sym2)
        (chibi test))

(define-syntax test-roundtrip-non-identical
  (syntax-rules()
    ((test-roundtrip expected expr)
     (test expected (roundtrip expr)))))

(define-syntax test-roundtrip
  (syntax-rules()
    ((test-roundtrip expr)
     (test expr (roundtrip expr)))))

(test-group "Roundtrip chibi-sym2::Expr-chibi conversions"
  (test-roundtrip 42)
  (test-roundtrip -123)
  (test-roundtrip 0)
  (test-roundtrip 1)

  (test-roundtrip 1.23456789)
  (test-roundtrip -1.23456789)
  (test-roundtrip 1.e5)

  (test-roundtrip 2/3)
  (test-roundtrip -4/5)
  (test-roundtrip -7/10092342803984029834092809384098283489284)

  (let ((large-int 39541197446419701451744617141547)
        (denom 5046171401992014917046404692049141744642098230482234))
    (test-roundtrip large-int)
    (test-roundtrip (- large-int))
    (test-roundtrip (/ large-int denom))
    (test-roundtrip (/ (- large-int) denom)))

  (test-roundtrip 42i)
  (test-roundtrip 42/43i)
  (test-roundtrip -42/43i)
  (test-roundtrip (+ 123 42/43i))
  (test-roundtrip (- 1.23456789 42/43i))
  (test-roundtrip (+ 1e6 10092342803984029834092809384098283489284i))

  (test-roundtrip 'a)
  (test-roundtrip 'a_b)
  (test-roundtrip 'a^b)
  (test-roundtrip 'a_<bcd>)
  (test-roundtrip 'a^<bcd>)
  (test-roundtrip 'a_<bcd>^e)
  (test-roundtrip 'a^e_<bcd>)

  (test-roundtrip 'a:+)
  (test-roundtrip 'a:r)
  (test-roundtrip 'a:c)
  (test-roundtrip 'a:+r)

  (test-roundtrip-non-identical 'abc "abc"))
