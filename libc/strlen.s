
		.export	_strlen
		.code

_strlen:
		clr	y
		ld	a,2(s)
sl:		ld	b,(a+)
		bz	strlen_done
		inc	y
		jump	sl
strlen_done:
		mov	a,y
		ret

