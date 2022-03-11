
(import (scheme base)
        (sym2)
        (chibi test))

(test-group "Polynomial algorithms"
  (test-group "Min. degree"
    (test 1 (poly-min-degree 'a 'a))
    (test 0 (poly-min-degree 'a 'b))

    (test 4 (poly-min-degree '(^ (+ (* 2 a) (^ a 2)) 4) 'a))
    (test 8 (poly-min-degree '(^ (+ (^ a 2) (^ a 3)) 4) 'a))
    (test 0 (poly-min-degree '(^ (+ (* 2 a) b) 4) 'a))
    (test 0 (poly-min-degree '(^ (+ (* 2 a) b) 4) 'a))

    (test 1 (poly-min-degree '(+ a (^ a 2) (^ a 5)) 'a))
    (test 0 (poly-min-degree '(+ a 2) 'a))
    (test 0 (poly-min-degree '(+ (^ a 2) b) 'a))

    (test 1 (poly-min-degree '(* a b (+ a 2)) 'a))
    (test 0 (poly-min-degree '(* a b c) 'd))

    (test-error (poly-min-degree 'a 42))
    (test-error (poly-min-degree 42 42))
    (test-error (poly-min-degree '(^ a 1000000000000000000000000000000000000000) 'a))
    (test-error (poly-min-degree '(sin a) 'a))
  )
)

(test-exit)
