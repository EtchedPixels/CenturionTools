
		.export _isblank

		.code

_isblank:
		ldab 3(s)
		ldbb ' '
		sabb
		bz good
		ldbb 9		; tab
		sabb
		bz good
		; Any non zero value is valid
		cla
good:
		rts
