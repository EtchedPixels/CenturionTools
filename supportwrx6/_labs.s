
		.export _labs

		.setcpu 6
		.code

_abs:
		lda	4(s)
		ldy	2(s)
		bp	noswap
		; Bit invert
		iva
		ivr	y
		; Add one
		ina
		bnz	noswap
		inr	y
noswap:
		rtr

