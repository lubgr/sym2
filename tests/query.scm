
(import (scheme base)
        (scheme inexact)
        (sym2)
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

(test-group "Sign"
  (test 1 (sign 42))
  (test 1 (sign 3/7))
  (test -1 (sign -42))
  (test -1 (sign -3/7))
  (test 1 (sign 3.14))
  (test -1 (sign -3.14))
  (let ((large-int 98238975982759238528759827952085702973597250927935709235))
    (test 1 (sign large-int))
    (test -1 (sign (- large-int))))
  (let ((large-rational
          8934798745051763784578587416987315971397135871375349/79823475828592837569273754239572837869785627382893798235))
    (test 1 (sign large-rational))
    (test -1 (sign (- large-rational))))

  (test 1 (sign 'a:+))
  (test 1 (sign 'a:r+))
  (test-not (sign 'a))

  (test 1 (sign '(pi 3.14)))
  (test -1 (sign '(const -2.345)))

  (test -1 (sign '(* -1 a:+)))
  (test 1 (sign '(+ 1 a:+ b:+)))

  (test -1 (sign '(* -2/3 (+ a:+ b:+))))
  (test #f (sign '(* -2/3 (+ a:+ b))))
  (test 1 (sign '(* -2/3 (sin -1) (+ a:+ b:+))))

  (test #f (sign '(^ a b)))
  (test #f (sign '(^ a 2)))
  (test #f (sign '(^ a 2/3)))
  (test #f (sign '(^ a -2/3)))
  (test #f (sign '(^ a b:r+)))
  (test #f (sign '(^ a:r+ 2+3i)))

  ; Can't be determined, since numeric part is < 0 and rest is > 0:
  (test #f (sign '(+ -1 (sin 2) a:+ b:+ c:+)))
  ; Numeric part is > 0, and so is non-numeric part:
  (test 1 (sign '(+ 1 (* -1 (sin 2)) a:+ b:+ c:+)))
  (test -1 (sign '(+ -1 (sin 2))))

  ; Numeric part evaluates to zero:
  (test 1 (sign `(+ ,(sin 2.0) (sin 2) a:+)))
  (test #f (sign `(+ ,(sin 2.0) (sin 2) a)))
  (test -1 (sign `(+ ,(- (sin 2.0)) (sin 2) (* -1 a:+))))

  ; 2*a + b*c + b^(2*c + pi) + 0.12345*c^2 is positive when all symbols are real and positive:
  (test 1 (sign '(+ (* 2 a:r+) (* b:r+ c:r+) (^ b:r+ (+ (* 2 c:r+) (pi 3.14))) (* 0.12345 (^ c:r+ 2)))))

  ; (-pi)^2 > 0
  (test 1 (sign '(^ (* -1 (pi 3.14)) 2)))
)

(test-group "Contains"
  (test-assert (contains 'a 'a))
  (test-not (contains 'a 'b))
  (test-assert (contains 42 42))
  (test-not (contains 42 43))
  (test-assert (contains '(pi 3.14) '(pi 3.14)))
  (test-not (contains '(pi 3.14) '(other 3.14)))

  (test-not (contains 'a 42))
  (test-not (contains 'a '(pi 3.14)))
  (test-not (contains 42 'a))

  (for-each
    (lambda (operand)
      (test-assert (contains operand '(* a b c d)))
      (test-assert (contains operand '(+ a b c d))))
    '(a b c d))

  (test-not (contains '(* a b) '(* b c)))
  (test-assert (contains '(* a b) '(+ 42 (^ a b) (* a b))))
  (test-not (contains '(* a b) '(+ 42 (^ a b) (* b c))))
  (test-assert (contains '(sin (+ a b)) '(^ c (* (sin (+ a b)) (cos (* c d))))))

  (test-assert (contains 'a '(sin a)))
)

(test-exit)
