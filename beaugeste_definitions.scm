(define-syntax swipe
    (syntax-rules (north south east west)
        ((swipe (n dir) expr)
            (eval `(define ,(string->symbol (string-append "swipe-"
                                                    (number->string n)
                                                    "-" (symbol->string dir)))
                           ,expr)))
    ))
