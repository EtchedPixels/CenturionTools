;
;	Negate the working register
;
	.export negeax

	.setcpu 6
	.code

negeax:
	iva
	ivr	y
	ina
	bnz	noripple
	inr	y
noripple:
	rsr
