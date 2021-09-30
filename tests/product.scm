
(import (sym2)
        (chibi test))

(test-group "Simple product cases"
  (test 0 (auto* 'a 0))
  (test 0 (auto* 0 'a))

  (test 42 (auto* 1 42))
  (test 42 (auto* 42 1))

  (test 'a (auto* 'a 1)))

(test-group "(Re-)ordering"
  (test '(* a b c (sin a)) (auto* 'a 'b 'c '(sin a)))
  (test '(* 2 a b c) (auto* 2 'a 'b 'c))

  (test '(* a b) (auto* 'b 'a)))

(test-exit)
