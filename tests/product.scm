
(import (sym2)
        (chibi test))

(test-group "Simple product cases"
  (test 0 (auto* 'a 0))
  (test 0 (auto* 0 'a))

  (test 42 (auto* 1 42))
  (test 42 (auto* 42 1))

  (test 'a (auto* 'a 1)))

(test-group "(Re-)ordering"
  (test (auto* 'a 'b 'c '(sin a)) '(* a b c (sin a)))
  (test (auto* 2 'a 'b 'c) '(* 2 a b c))

  (test (auto* 'b 'a) '(* a b)))

(test-exit)
