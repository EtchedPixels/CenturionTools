;
;	Boolean negation
;
	.setcpu 6

	.code

	.export bnega
	.export bnegax

bnegax:	xfr y,b
	ori a,b
	bz ret1
ret0:
	cla
	rtr

bnega:	ori a,a
	bnz ret0
ret1:
	lda 1
	rtr
