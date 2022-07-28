
(import (scheme base)
        (sym2)
        (chibi test))

(test-group "Polynomial algorithms"
  (test-group "Min. degree"
    (test 1 (min-degree 'a 'a))
    (test 0 (min-degree 'a 'b))

    (test 4 (min-degree '(^ (+ (* 2 a) (^ a 2)) 4) 'a))
    (test 8 (min-degree '(^ (+ (^ a 2) (^ a 3)) 4) 'a))
    (test 0 (min-degree '(^ (+ (* 2 a) b) 4) 'a))
    (test 0 (min-degree '(^ (+ (* 2 a) b) 4) 'a))

    (test 1 (min-degree '(+ a (^ a 2) (^ a 5)) 'a))
    (test 0 (min-degree '(+ a 2) 'a))
    (test 0 (min-degree '(+ (^ a 2) b) 'a))

    (test 1 (min-degree '(* a b (+ a 2)) 'a))
    (test 0 (min-degree '(* a b c) 'd))

    (test-error (min-degree 'a 42))
    (test-error (min-degree 42 42))
    (test-error (min-degree '(^ a 1000000000000000000000000000000000000000) 'a))
    (test-error (min-degree '(sin a) 'a))
  )

  (test-group "Degree"
    (test 1 (degree 5/7 5/7))
    (test 1 (degree 'a 'a))
    (test 0 (degree 'a 'a:+))
    (test 1 (degree '(sin a) '(sin a)))
    (test 1 (degree '(pi 3.14) '(pi 3.14)))
    (test 1 (degree '(^ a b) '(^ a b)))
    (test 1 (degree '(* a b) '(* a b)))
    (test 1 (degree '(+ a b) '(+ a b)))

    (test 0 (degree 2 3))
    (test 0 (degree 2 2/3))

    (test 0 (degree 'a 'b))
    (test 0 (degree 'a '(+ a b)))

    (test 0 (degree '(^ a b) '(^ c 3)))
    (test 0 (degree '(* a b) '(^ c 2)))
    (test 10 (degree '(^ a 10) 'a))
    (test -10 (degree '(^ a -10) 'a))
    (test 0 (degree '(^ a 2/3) 'a))
    (test 5 (degree '(^ (+ a b c) 5) '(+ a b c)))
    (test -2 (degree '(^ (+ a (^ a -5)) -2) 'a))
    (test 0 (degree '(^ a (* 2 b)) 'a))

    (test 1 (degree '(+ a b) 'a))
    (test 1 (degree '(+ a b) 'b))
    (test 1 (degree '(+ 2 a (* c d)) 'd))
    (test 5 (degree '(+ 10 a (* a b) (^ a 3) (* (^ a 5) b c)) 'a))
    (test 10 (degree '(+ 42 (^ a 2) (^ a -2) (^ a 3) (^ a 10) (^ a 7)) 'a))
    (test 0 (degree '(+ 2 a) '(* b c)))
    ; Note: degree of b^(-3) w.r.t. a is 0, which is > -4.
    (test 0 (degree '(+ (^ a -4) (^ b -3)) 'a))
    (test -3 (degree '(+ (^ a -7) (* (^ a -3) b (sin a))) 'a))

    (test 1 (degree '(* a b) 'a))
    (test 1 (degree '(* a b) 'b))
    (test 2 (degree '(* (^ a 2) b) 'a))
    (test 3 (degree '(* a (+ a (* a b (+ a c)))) 'a))
    (test 1 (degree '(* a (+ (^ a -4) b)) 'a))
  )
)

(test-exit)
