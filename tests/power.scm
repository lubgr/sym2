
(import (sym2)
        (chibi test))

(test-group "Simple power cases"
  (test 1 (auto^ 'a 0))
  (test 0 (auto^ 0 'a))
  (test 1 (auto^ 1 42))
  (test 42 (auto^ 42 1))
  (test 'a (auto^ 'a 1)))
