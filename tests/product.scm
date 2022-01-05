
(import (sym2)
        (chibi test))

(test-group "Simple product cases"
  (test 0 (auto* 'a 0))
  (test 0 (auto* 0 'a))

  (test 42 (auto* 1 42))
  (test 42 (auto* 42 1))

  (test 'a (auto* 'a 1)))

(test-group "Numeric products same type"
  (test (* 42 42) (auto* 42 42))
  (test 2/7 (auto* 3/7 2/3))
  (test 22-7i (auto* 3+2i 4-5i))
  (test 3097/15015-1717/10010i (auto* 3/13+2/77i 4/5-5/6i))
  (let ((n 7.2384729384723846))
    (test (* n n) (auto* n n)))
  (test 27500392876463266013749914033174582653601713315643190880605402980124494493572337044
    (auto* 9879283748929498723498273498292836748927 2783642374827340972904729047293749243940972)))

(test-group "Numeric products different type"
  (test 4.93839309579488 (auto* 1.23459827394872 4))
  (test 0.5698005646153846 (auto* 1.23456789 6/13))
  (test 6.58436214 (auto* 9.87654321 2/3))
  (test 5.2275288104325525e+47 (auto* 4234298374982482468562523333333323738287237286 123.456789))
  (test 9.421125142857143+3.028213981428572i (auto* 5.23-4i 6/7+1.234567i))
  (test 0+6i (auto* 2 3i))
  (test 0 (auto* 0 5+6i)))

(test-group "(Re-)ordering"
  (test '(* a b c (sin a)) (auto* 'a 'b 'c '(sin a)))
  (test '(* 2 a b c) (auto* 2 'a 'b 'c))
  (test '(* a b) (auto* 'b 'a))
  (test '(* a b c d e f g h) (auto* 'h 'b 'a 'c 'e 'g 'f 'd)))

(test-group "Larger product simplifications"
  (test '(* 464472.1368 (^ a 2) (^ b 4) c y z)
        (auto* 'b  42 'a 'z 6.1234 'y 'a 'b 43 'b 'b 'c 42)))

(test-exit)
