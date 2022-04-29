
		.export _isprint

		.code

_isprint:
		ldab 3(s)
		ldbb 32
		sabb
		bl fail
		ldbb 127
		sabb
		bnl fail
		; Any non zero value is valid
		rtr
fail:		cla
		rtr
