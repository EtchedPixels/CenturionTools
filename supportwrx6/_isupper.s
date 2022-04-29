
		.export _isupper

		.code

_isupper:
		ldab 3(s)
		ldbb 'A'
		sabb
		bl fail
		ldbb 'Z'+1
		sabb
		bnl fail
		; Any non zero value is valid
		rtr
fail:		cla
		rtr
