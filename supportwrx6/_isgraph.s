
		.export _isgraph

		.code

_isgraph:
		ldab 3(s)
		ldbb ' '
		sabb
		bl fail
		ldbb 127
		sabb
		bnl fail
		; Any non zero value is valid
		rtr
fail:		cla
		rtr
