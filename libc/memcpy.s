
		.export _memcpy

		.code

_memcpy:
		ld	a,2(s)
		mov	y,a
		ld	a,4(s)
		mov	z,a
		ld	b,6(s)
		bz	nowork
memcpyl:
		ld	a,(z+)
		st	a,(y+)
		dec	b
		bnz	memcpyl
		ld	a,2(s)
nowork:
		ret
