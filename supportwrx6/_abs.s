
		.export _abs

		.setcpu 6
		.code

_abs:
		lda	2(sp)
		bp	noswap
		ivra
		dcra
noswap:
		rtr

