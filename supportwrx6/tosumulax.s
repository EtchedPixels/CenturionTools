;
;	Perform the classic bit shift multiply
;
;	We need to keep everything in registers
;
;
		.export tosmulax
		.export tosumulax
		.code
tosmulax:
tosumulax:
		stx	(-s)
		sty	(-s)

		clr	y		; Work register
		ldb	6(s)		; Going to do A * B
		ldx	32		; counter
		; Rotate through the number
nextbit:
		srl	a
		bnl	noadd
		add	b,y
noadd:		sll	b
		dcrb	x
		bnz	nextbit
		; BA is now the result

		ldy	(s+)
		ldx	(s+)
		; Throw away a word
		inr	s
		inr	s

		; Result in BA, we don't use the upper half for anything
		; yet but it might be a handy optimisation path for 32x16
		; maths when it pops up
		rtr
