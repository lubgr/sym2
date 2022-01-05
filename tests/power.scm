
(import (sym2)
        (chibi test))

(test-group "Simple power cases"
  (test 1 (auto^ 'a 0))
  (test 0 (auto^ 0 'a))
  (test 0 (auto^ 0 '+a))
  (test 1 (auto^ 1 42))
  (test 1 (auto^ 1 '(+ a b c)))
  (test 1 (auto^ '(+ a b) 0))
  (test 1 (auto^ '(* a b) 0))
  (test 42 (auto^ 42 1))
  (test '(* a b) (auto^ '(* a b) 1))
  (test 'a (auto^ 'a 1)))

(test-group "Automatic simplification of powers"
  ; No simplifications made:
  (test '(^ 2 pi) (auto^ 2 'pi))
  (test '(^ 2 2/3) (auto^ 2 2/3))

;   (test 85070591730234615847396907784232501249 (auto^ (- (expt 2 63) 1) 2))

  (test 4 (auto^ 2 2))
  (test 4/9 (auto^ 2/3 2))
  (test -8/343 (auto^ -2/7 3))

;   (test 'a (auto^ '(^ a 1/2) 2))
;   (test 'a (auto^ '(^ a 1/3) 3))
)

; (test-group "Positive base enhancements"
;   (test '+a (auto^ '(^ +a 3) 1/3))
;   (test '+a (auto^ '(^ +a 2) 1/2))
;   (test '(+ +a +b +c) (auto^ '(^ (+ +a +b +c) 8) 1/8)))

(test-exit)
