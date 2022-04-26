
			.export _memset
			.code

;
;	memset has an int sized pattern that is byte written.
;
;
_memset:
		ld	a,2(s)
		mov	y,a
		ld	a,4(s)
		ld	b,6(s)
		jz	nowork

setl:
		st	al,(y+)
		dec	b
		jnz	setl
nowork:
		ld	a,2(s)
		ret
