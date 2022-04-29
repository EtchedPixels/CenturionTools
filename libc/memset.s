
			.export _memset
			.code

;
;	memset has an int sized pattern that is byte written.
;
;
_memset:
		lda	2(s)
		xay
		ldb	4(s)
		lda	6(s)
		jz	nowork

setl:
		stbb	(y+)
		dca
		jnz	setl
nowork:
		lda	2(s)
		rsr
