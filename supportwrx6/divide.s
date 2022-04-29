;
;	Baed on the 6303 version except that we lack xgdx so it's a little
;	bit uglier
;
;	This is the classic division algorithm
;
;	On entry D holds the divisor and X holds the dividend
;
;	work = 0
;	loop for each bit in size (tracked in tmp)
;		shift dividend left (A)
;		rotate left into work (X)
;		set low bit of dividend (A)
;		subtract divisor (B) from work
;		if work < 0
;			add divisor (B) back to work
;			clear lsb of A
;		end
;	end loop
;
;	On exit A holds the quotient, B holds the remainder
;
;
	.export div16x16
	.setcpu 6

	.code

div16x16:
	stx	(-s)
	sty	(-s)

	; Calculate A / B


	ldb	16		; bit count
	xfrb	b,y

	clr	x		; working register
loop:
	sla
	rlr	x
	ina			; sets low bit of A
	sub	b,x
	; Carry logic is bass-ackwards for sub
	bnl	skip
	bz	skip
	add	b,x
	dca			; clear low bit of A
skip:
	dcrb	y
	bnz	loop
	rtr
