;
;	Subtract Y:A from the top of stack into Y:A
;

	.export tossubeax

	.setcpu 6
	.code

tossubeax:
	ldb	2(s)
	sub	y,b
	stb	2(s)
	xfr	b,y
	ldb	4(s)
	sub	a,b
	stb	4(s)
	xfr	b,a
	bnl	noripple
	dcr	y
noripple:
	; Throw 4 bytes
	ldb	(-s)
	ldb	(-s)
	rsr


