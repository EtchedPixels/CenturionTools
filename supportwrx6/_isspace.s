
		.export _isspace

		.code

_isspace:
		ldab 3(s)
		ldbb ' '
		sabb
		bz good
		ldbb 9
		sabb		; tab
		bl fail
		ldbb 13+1	; line feed
		sabb
		bnl good
fail:		cla
		; any non zero is 'good
good:		rtr
