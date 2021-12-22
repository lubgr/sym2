
(import (sym2)
        (chibi test))

(define-syntax tlt
  (syntax-rules()
    ((tlt first second)
     (and (test-assert (order-lt first second)) (test-not (order-lt second first))))))

(define-syntax tlt-not
  (syntax-rules()
    ((tlt-not first second)
     (test-not (order-lt first second)))))

(test-group "Order relation same type"
  (test-group "Both numeric"
    (tlt 1 2)
    (tlt -1 1)
    (tlt 0 1.2345678)
    (tlt 2/3 7)
    (tlt 1/2 2/3)
    (tlt 29038470234243209429834928942759 9823748927438923498273498237489273489729)
    (tlt 29038470234243209429834928942759/9823748927438923498273498237489273489729 1)

    (tlt-not 10 10)
    (tlt-not 0.0 0.0)
    (tlt-not 0.0 -0.0))

  (test-group "Both symbol or both constant"
    (tlt 'a1 'a2)
    (tlt 'a1:+ 'a1)
    (tlt 'a1:+ 'a1:r)
    (tlt 'a1:r+ 'a1:+)
    (tlt 'a1:r+ 'a1:r)
    (tlt 'A 'a)
    (tlt 'a 'e)
    (tlt 'abcdefghi 'abcdefghij)

    (tlt '(e 2.7) '(pi 3.14))
    ;; This shouldn't happen in practice, but it's a reasonable behavior:
    (tlt '(e 2.7) '(e 3.14)))

  (test-group "Both power"
    (tlt '(^ a 1/2) '(^ b 1/2))
    (tlt '(^ a 1/2) '(^ a 10))
    (tlt '(^ a 2) '(^ b 2/3))
    (tlt-not '(^ 2 2/3) '(^ 2 2/3)))

  (test-group "Both sum or both product"
    (tlt '(+ a b) '(+ a c))
    (tlt '(+ a c d) '(+ b c d))
    (tlt '(+ c d) '(+ b c d))
    (tlt-not '(+ a b c d) '(+ a b c d))

    (tlt '(* a b) '(* a c))
    (tlt '(* a c d) '(* b c d))
    (tlt '(* c d) '(* b c d))
    (tlt-not '(* a b c d) '(* a b c d)))

  (test-group "Both functions"
    (tlt '(cos a) '(sin a))
    (tlt '(sin a) '(tan a))
    (tlt '(sin b) '(tan a))
    (tlt '(sin a) '(sin b))
    (tlt '(atan2 a b) '(atan2 a c))
    (tlt '(atan2 a (+ 2 a b)) '(atan2 a (+ 2 b c)))))

  (test-group "Different types"
    (tlt 42 'a)
    (tlt 42 '(pi 3.14))
    (tlt 42 '(+ a b))
    (tlt 42 '(^ a 1/2))

    (tlt '(pi 3.14) 'a)

    (tlt '(* a (^ b 2)) '(^ b 3) )
    (tlt 'b '(* a b))

    (tlt '(^ a 1/2) 'a)
    (tlt '(^ a 1/2) 'b)
    (tlt '(pi 3.14) '(^ a 1/2))
    (tlt '(^ a 1/4) '(^ a 1/2))

    (tlt 'b '(+ a b c))
    (tlt '(pi 3.14) '(+ a b c))
    (tlt '(^ (+ 1/2 a) 2) '(+ 1/2 b))
    (tlt '(^ (+ a b) 1/2) '(+ a b))
    (tlt '(^ a 1/2) 'b)
    (tlt 2 '(^ 2 1/2))
    (tlt '(+ 2 a) 'e)

    (tlt 'b '(cos a))
    (tlt '(cos a) 'd)
    (tlt '(* a b) '(sin a))

    (tlt '(sin d) '(* a b c (sin d)))
    (tlt '(^ a 2) '(sin a))
    (tlt '(+ b (sin a)) '(sin c))
    (tlt-not '(cos (^ 7 2/3)) '(cos (^ 7 2/3))))

(test-exit)
