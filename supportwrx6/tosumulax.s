;
;	Perform the classic bit shift multiply
;
;	We need to keep everything in registers
;
;
		.export tosmulax
		.export tosumulax

		.setcpu 4

		.code
tosmulax:
tosumulax:
		stx	(-s)
		xfr	y,x
		stx	(-s)

		clr	y		; Work register
		ldb	6(s)		; Going to do A * B
		ldx	32		; counter
		; Rotate through the number
nextbit:
		slr	a
		bnl	noadd
		add	b,y
noadd:		slr	b
		dcrb	xl
		bnz	nextbit
		; BA is now the result

		ldx	(s+)
		xfr	x,y
		ldx	(s+)
		; Throw away a word
		inr	s
		inr	s

		; Result in BA, we don't use the upper half for anything
		; yet but it might be a handy optimisation path for 32x16
		; maths when it pops up
		rsr
