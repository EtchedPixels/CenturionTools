
		.export _isascii

		.code

_isascii:
		ldab 3(s)
		bm fail
		lda 1
		rts
fail:		cla
		rts
