
(import (sym2)
        (chibi test))

(test-group "Const/term decomposition"
  (test '(1 ((pi 3.14))) (split-const-term '(pi 3.14)))
  (test '(1 (a)) (split-const-term 'a))
  (test '(1 ((+ 42 a b c))) (split-const-term '(+ 42 a b c)))
  (test '(1 ((atan a))) (split-const-term '(atan a)))
  (test '(1 ((^ a b))) (split-const-term '(^ a b)))

  (test '(2 (a b c)) (split-const-term '(* 2 a b c)))
  (test '(1 ((+ 42 a b c) (+ 43 d e f))) (split-const-term '(* (+ 42 a b c) (+ 43 d e f))))

  (test-error (split-const-term 42))
  (test-error (split-const-term 42/43))
  (test-error (split-const-term 42i))
  (test-error (split-const-term 3.14)))
