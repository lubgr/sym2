
(import (scheme base)
        (sym2)
        (chibi test))

(define-syntax pt
  (syntax-rules()
    ((pt expected expr)
     (test expected (expr->string expr)))))

(test-group "Plaintext scalars"
  (pt "a" 'a)
  (pt "abc" 'abc)
  (pt "abc^d" 'abc^d)
  (pt "abc^d_e" 'abc^d_e)

  ; The plaintext printer ignores with symbol flags
  (pt "a" 'a:r)
  (pt "a" 'a:+)
  (pt "a" 'a:r+)

  (pt "pi" '(pi 3.14))
  (pt "e" '(e 2.7))
  (pt "x" '(x -1.2345))

  (pt "2" 2)
  (pt "-2" -2)
  (pt "2/3" 2/3)
  (pt "-2/3" -2/3)
  (pt "290384702342432094298349289427599823748927438923498273498237489273489729"
      290384702342432094298349289427599823748927438923498273498237489273489729)
  (pt "29038470234243209429834928942759/9823748927438923498273498237489273489729"
      29038470234243209429834928942759/9823748927438923498273498237489273489729)
  (pt "2 + 3i" 2+3i)
  (pt "-2 + 3i" -2+3i)
  (pt "2 - 3/4i" 2-3/4i)
  (pt "-9.876 + 1.2345i" -9.876+1.2345i)

  ; We need to work around a bug in chibi here, see https://github.com/ashinn/chibi-scheme/issues/815
  (pt "-2/3 - 3i" (* -1 2/3+3i))

  ; The assertion is sensitive to precision. This works for now, but when it fails, we should change
  ; the assertion to checking if the result begins with "1.234[...]" rather than equality.
  (pt "1.23456" 1.23456))

(test-group "Plaintext sums"
  (pt "a + b + c + d" '(+ a b c d))
  (pt "a - b + c - d" '(+ a (* -1 b) c (* -1 d)))
  (pt "-a - b - c - d" '(+ (* -1 a) (* -1 b) (* -1 c) (* -1 d)))
  (pt "2 + 3i + a" '(+ 2+3i a)))

(test-group "Plaintext products"
  (pt "a/2" '(* 1/2 a))
  (pt "a*b*c*d" '(* a b c d))
  (pt "2*b*c*(a + d)" '(* 2 b c (+ a d)))
  (pt "2*a*b/3" '(* 2/3 a b))
  (pt "2*a*b/(3*c)" '(* 2/3 a b (^ c -1)))
  (pt "1.2345*a*b" '(* 1.2345 a b))
  (pt "a*(2 + 3i)" '(* a 2+3i))
  (pt "1/(a*b*c)" '(* (^ a -1) (^ b -1) (^ c -1)))
  (pt "a*b/(c*d*e)" '(* a b (^ c -1) (^ d -1) (^ e -1)))

  (pt "(a + b)/(c*d)" '(* (+ a b) (^ c -1) (^ d -1)))
  (pt "a/b" '(* a (^ b -1)))
  (pt "1/(a^2*b^2)" '(* (^ a -2) (^ b -2))))

(test-group "Functions"
  (pt "sin(a)" '(sin a))
  (pt "atan2(2*a, b)" '(atan2 (* 2 a) b)))

(test-group "Plaintext power"
  (pt "1/(2*a*b)" '(^ (* 2 a b) -1))

  (pt "1/a" '(^ a -1))
  (pt "a^2" '(^ a 2))
  (pt "a^(2/3)" '(^ a 2/3))
  (pt "1/a^(2/3)" '(^ a -2/3))
  (pt "(5/7)^a" '(^ 5/7 a))
  (pt "pi^(a + b)" '(^ (pi 3.14) (+ a b)))
  (pt "(a + b)^pi" '(^ (+ a b) (pi 3.14)))
  (pt "a^b" '(^ a b))
  (pt "(((a^b)^c)^(-a/4))^d" '(^ (^ (^ (^ a b) c) (* -1/4 a)) d))
  (pt "-c/(2*a*b)" '(* -1 c (^ (* 2 a b) -1)))

  (pt "(a + b)^2" '(^ (+ a b) 2))
  (pt "2^(a + b)" '(^ 2 (+ a b)))

  (pt "sqrt(42)" '(^ 42 1/2))
  (pt "sqrt(a*b)" '(^ (* a b) 1/2))
  (pt "1/sqrt(a)" '(^ a -1/2))
  (pt "1/(sqrt(a)*sqrt(b))" '(* (^ a -1/2) (^ b -1/2))))

(test-group "Plaintext mixed"
  (pt "a*b*(a + c)*f^a/(d*e^2)" '(* a b (+ a c) (* (^ f a) (^ d -1) (^ e -2))))
  (pt "a*atan(1/sqrt(17))*cos(c*d)*sin(a*b)^3/cos(a*b)"
      '(* a (atan (^ 17 -1/2)) (cos (* c d)) (^ (sin (* a b)) 3) (^ (cos (* a b)) -1))))

(test-exit)
