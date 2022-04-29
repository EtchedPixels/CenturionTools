;
;	Left shift 32bit
;
	.export tosasleax
	.export tosshleax

	.setcpu 6
	.code

;
;	TODO: optimize 8, 16 steps on asl and asr cases
;
tosasleax:
tosshleax:
	ldb	32
	sab
	bnl	ret0
	ldb	16
	sab
	bnl	shleax16
	xab
	ld	y,2(s)
	ld	a,4(s)
	ori	b,b
	bz	retpop
loop:
	sla
	rlr	y
	dcrb	b
	bnz	loop
	; Throw 4 bytes off the stack, we will corrupt the caller X but
	; the caller doesn't expect it saved
retpop:
	ldb	(s+)
	ldb	(s+)
	rtr
ret0:
	cla
	clr	y
	bra	retpop

;
;	Common fast path
;
shleax16:
	xab
	lda	8
	sab
	cla
	ldy	4,(s)
	ori	b,b
	bz	retpop
loop2:
	slr	y
	dcr	b
	bn	loop2
	bra	retpop
