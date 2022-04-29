
		.export _islower

		.code

_islower:
		ldab 3(s)
		ldbb 'a'
		sabb
		bl fail
		ldbb 'z'+1
		sabb
		bnl fail
		; Any non zero value is valid
		rtr
fail:		cla
		rtr
