
(import (sym2)
        (chibi test))

(test-group "Simple product cases"
  (test 0 (auto* 'a 0))
  (test 0 (auto* 0 'a))

  (test 42 (auto* 1 42))
  (test 42 (auto* 42 1))

  (test 'a (auto* 'a 1)))
