
		.export _memcpy

		.code

_memcpy:
		lda	2(s)
		xay
		lda	4(s)
		xaz
		lda	6(s)
		bz	nowork
memcpyl:
		ldb	(z+)
		stb	(y+)
		dca
		bnz	memcpyl
		lda	2(s)
nowork:
		rsr
