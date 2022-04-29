;
;	D = TOS / D (unsigned)
;

	.export tosudivax
	.export tosumodax

	.setcpu 4
	.code

tosudivax:
	ldb	2(s)
	jsr div16x16
	ldb	(s+)
	rsr

tosumodax:
	ldb	2(s)
	jsr	div16x16
	xfr	b,a
	ldb	(s+)
	rsr
