
(import (sym2)
        (chibi test))

(test-group "Simple product cases"
  (test 'a (auto+ 'a 0))
  (test 'a (auto+ 0 'a))
  (test 42 (auto+ 21 21))
  (test 42 (auto+ 42 0))
  (test 'a (auto+ 'a 0))

  (test '(+ a b) (auto+ 'a 'b))
  (test '(* 2 a) (auto+ 'a 'a))
  (test '(* 5 a) (auto+ 'a '(* 2 a) 'a 'a))
  (test 0 (auto+ 'a '(* -2 a) '(* -1 a) '(* 2 a)))

  (test '(+ a b c d) (auto+ '(+ a c) '(+ b d)))
  (test '(+ (* 4 a) (* 4 b)) (auto+ 'a 'b 'b 'a 'a 'b 'b 'a)))

(test-group "Numeric sums same type"
  (test 84 (auto+ 42 42))
  (test 23/21 (auto+ 3/7 2/3))
  (test 7-3i (auto+ 3+2i 4-5i))
  (test 67/65-373/462i (auto+ 3/13+2/77i 4/5-5/6i))
  (let ((n 7.2384729384723846))
    (test (+ n n) (auto+ n n)))
  (test 2793521658576270471628227320792042080689899
    (auto+ 9879283748929498723498273498292836748927 2783642374827340972904729047293749243940972)))

(test-group "Numeric sums different type"
  (test 5.23459827394872 (auto+ 1.23459827394872 4))
  (test 1.6961063515384613 (auto+ 1.23456789 6/13))

  (test 10.543209876666666 (auto+ 2/3 9.87654321))
  (test 4.2342983749824826e+45 (auto+ 4234298374982482468562523333333323738287237286 123.456789))
  (test 6.087142857142857-2.765433i (auto+ 5.23-4i 6/7+1.234567i))
  (test 2+3i (auto+ 2 3i))
  (test 5+6i (auto+ 0 5+6i)))

(test-group "(Re-)ordering"
  (test '(+ a b c (sin a)) (auto+ 'a 'b 'c '(sin a)))
  (test '(+ 2 a b c) (auto+ 2 'a 'b 'c))
  (test '(+ a b) (auto+ 'b 'a))
  (test '(+ a b c d e f g h) (auto+ 'h 'b 'a 'c 'e 'g 'f 'd)))

(test-exit)
