
(define (auto+ first second . args)
  (auto-plus (cons first (cons second args))))

(define (auto* first second . args)
  (auto-times (cons first (cons second args))))
