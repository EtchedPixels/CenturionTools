
		.export _isdigit

		.code

_isdigit:
		ldab 3(s)
		ldbb '0'
		sabb
		bl fail
		ldbb '9'
		sabb
		bnl fail
		; Any non zero value is valid
		rtr
fail:		cla
		rtr
