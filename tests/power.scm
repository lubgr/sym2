
(import (sym2)
        (chibi test))

(test-group "Simple power cases"
  (test 1 (auto^ 'a 0))

  (test 0 (auto^ 0 'a))
  (test 0 (auto^ 0 '+a))
  (test 0 (auto^ 0 1))

  (test-error (auto^ 0 -1))
  (test-error (auto^ 0 -2/3))
  (test-error (auto^ 0 0))

  (test 1 (auto^ 1 42))
  (test 1 (auto^ 1 '(+ a b c)))
  (test 1 (auto^ '(+ a b) 0))
  (test 1 (auto^ '(* a b) 0))
  (test 42 (auto^ 42 1))
  (test '(* a b) (auto^ '(* a b) 1))
  (test 'a (auto^ 'a 1)))

(test-group "Numeric power - rational base"
  (test 4 (auto^ 2 2))
  (test 4 (auto^ -2 2))
  (test -8 (auto^ -2 3))
  (test 1/4 (auto^ -2 -2))
  (test -1/8 (auto^ -2 -3))
  (test 1/16 (auto^ 2 -4))
  (test 1/8 (auto^ 2 -3))

  (test '(^ 3 1/7) (auto^ 3 1/7))
  (test '(^ -3 1/7) (auto^ -3 1/7))
  (test '(^ 3 -1/7) (auto^ 3 -1/7))
  (test '(^ -3 -1/7) (auto^ -3 -1/7))

  (test '(^ 3 1+2i) (auto^ 3 1+2i))
  (test '(^ -3 -1-2i) (auto^ -3 -1-2i))

  (test 1i (auto^ -1 1/2));
  (test 1/2i (auto^ -1/4 1/2));
  (test '(^ -1 1/3) (auto^ -1 1/3));
  (test '(^ -1 1/27) (auto^ -1 1/27));
  (test '(^ -1 1/28) (auto^ -1 1/28));

  (test 2 (auto^ 4 1/2))
  (test 3/7 (auto^ 9/49 1/2))
  (test 3/7i (auto^ -9/49 1/2))
  (test '(^ 2/3 1/7) (auto^ 2/3 1/7))
  (test 4/9 (auto^ 2/3 2))
  (test -8/343 (auto^ -2/7 3))
  (test '(^ -2 1/2) (auto^ -2 1/2))
  (test '(^ -3 1/2) (auto^ -3 1/2))

  (test 85070591730234615847396907784232501249 (auto^ (- (expt 2 63) 1) 2))
  (test 1/3425112392337918538031401725219615410929593111707220358591204846437647956050064
        (auto^ 1850705917302346158473969077842325012492 -2)))

(test-group "Numeric power - floating point base or exponent"
  (test-error (auto^ 0 -1.23456))
  (test 3.881558974007327 (auto^ 3 1.2345))
  (test -2.875021-2.60782i (auto^ -3 1.2345))
  (test 0.2576284443174642 (auto^ 3 -1.2345))

  (test -0.1908221290993779+0.17308763782378306i (auto^ -3 -1.2345))
  (test (expt 1.2345 6.789) (auto^ 1.2345 6.789))
  (test (expt 1.2345 -6.789) (auto^ 1.2345 -6.789))

  (test -3.2944550122327256412+2.572058914169188218i (auto^ -1.2345 6.789))
  (test -0.1885894975316828221-0.1472362792765273504i (auto^ -1.2345 -6.789))
  (test -7.985548461967260062e66-7.220561617287906803e66i
        (auto^ -2093870249834092348927508975897209423856892365237293804 1.234)))

(test-group "Automatic simplification of powers"
  (test '(^ 2 pi) (auto^ 2 'pi))
  (test '(^ 2 2/3) (auto^ 2 2/3))

  (test 'a (auto^ '(^ a 1/2) 2))
  (test 'a (auto^ '(^ a 1/3) 3))
)

(test-group "Positive base power simplifications"
  (test '+a (auto^ '(^ +a 3) 1/3))
  (test '+a (auto^ '(^ +a 2) 1/2))
  (test '(+ +a +b +c) (auto^ '(^ (+ +a +b +c) 8) 1/8)))

(test-exit)
