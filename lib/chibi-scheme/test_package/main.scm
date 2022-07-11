
(import (scheme base)
        (chibi test))

(test-group "Package test"
  (let ((test-string "Test string"))
    (test "Test string" test-string)))

(test-exit)
