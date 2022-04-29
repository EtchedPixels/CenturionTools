;
;	On entry B points to the object
;
	.code
	.export lsubeq
	.export lsubeqysp
	.setcpu 4

; In this form B is the stack offset. Turn that into B is a pointer and
; fall into the static form
lsubeqysp:
	add	s,b
;
;	In this form B is a pointer to the long
;
lsubeq:
	ldx	2(b)
	sub	a,x
	stx	2(b)
	xfr	x,a		; Save low result
	bnl	nocarry
	inr	y
nocarry:
	ldx	(b)
	sub	y,x
	stx	(b)
	xfr	x,y		; Save high result
	rsr
