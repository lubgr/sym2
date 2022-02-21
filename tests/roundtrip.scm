
(import (scheme base)
        (sym2)
        (chibi test))

(define-syntax test-roundtrip
  (syntax-rules()
    ((test-roundtrip expr)
     (test expr (roundtrip expr)))))

(define-syntax test-roundtrip-error
  (syntax-rules()
    ((test-roundtrip-error expr)
     (test-error (roundtrip expr)))))

(test-group "Roundtrip chibi-Expr-chibi conversions"
  (test-group "Numbers"
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
    (test-roundtrip -2+3i)
    (test-roundtrip -2-3i)
    (test-roundtrip 2-3i)
    (test-roundtrip -2/3-3i)
    (test-roundtrip -2/7-3/4i)
    (test-roundtrip (+ 123 42/43i))
    (test-roundtrip (- 1.23456789 42/43i))
    (test-roundtrip (+ 1e6 10092342803984029834092809384098283489284i)))

  (test-group "Symbols"
    (test-roundtrip 'a)
    (test-roundtrip 'a_b)
    (test-roundtrip 'a^b)
    (test-roundtrip 'a_<bcd>)
    (test-roundtrip 'a^<bcd>)
    (test-roundtrip 'a_<bcd>^e)
    (test-roundtrip 'a^e_<bcd>)

    (test-roundtrip 'a_x:+)
    (test-roundtrip 'a_x:r)
    (test-roundtrip 'a_x:r+)
    (test 'a_x:r+ (roundtrip 'a_x:+r))

    (test-roundtrip-error 'a:c)
    (test-roundtrip-error 'a:rr)
    (test-roundtrip-error 'a:c+)
    (test-roundtrip-error 'a:123)
    (test-roundtrip-error 'a:)

    ; Strings turn into symbols - we could be stricter though and raise an error.
    ; This behavior is probably not needed.
    (test 'abc (roundtrip "abc")))

  (test-group "Errorneous input"
    (test-roundtrip-error '())
    (test-roundtrip-error #f)
    (test-roundtrip-error #t)
    (test-roundtrip-error '(1 2 3))
    (test-roundtrip-error '(#t 2 3))
    (test-roundtrip-error '('() 2 3))
    (test-roundtrip-error '(#f 2/3 4i)))

  (test-group "Functions"
    (test-roundtrip '(sin 2))
    (test-roundtrip '(cos a))
    (test-roundtrip '(tan b))
    (test-roundtrip '(atan2 a b))
    (test-roundtrip '(log a))

    (test-roundtrip-error '(sin))
    (test-roundtrip-error '(atan2))
    (test-roundtrip-error '(sin 2 3))
    (test-roundtrip-error '(atan2 1)))

  (test-group "Constants"
    ; Any leading symbol followed by a floating point number becomes a constant
    (test-roundtrip '(euler 2.7))
    (test-roundtrip '(pi 3.14))
    (test-roundtrip '(pi -3.0))

    ; Constants must have exactly one floating point value
    (test-roundtrip-error '(pi 3))
    (test-roundtrip-error '(euler 3))
    (test-roundtrip-error '(euler a))
    (test-roundtrip-error '(euler (sin 2)))
    (test-roundtrip-error '(constant 1.234 5.678))
    (test-roundtrip-error '(a b c))
    (test-roundtrip-error '(not-a-binary-fct 2 3)))

  (test-group "Sums, products, powers"
    (test-roundtrip '(+ a b c))
    (test-roundtrip '(+ 42 a b (* 2 c d) (^ 3 (+ 2 3))))
    (test-roundtrip '(* 42 a (+ 2 c d) (+ 3 a (cos c)) (+ a b c d e)))
    (test-roundtrip '(* a b c))
    (test-roundtrip '(* a b (c 1.2345)))

    (test-roundtrip '(^ a b))
    (test-roundtrip-error '(^ a))
    (test-roundtrip-error '(^ a b c))
    (test-roundtrip-error '(^ a b c d 1 2 3))))

(test-exit)
