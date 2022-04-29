
		.export _isxdigit

		.code

_isxdigit:
		ldab 3(s)
		ldbb '0'
		sabb
		bl fail
		ldbb '9'+1
		sabb
		bl good
		ldbb 'A'
		sabb
		bl fail
		ldbb 'F'+1
		sabb
		bl good
		ldbb 'a'
		sabb
		bl fail
		ldbb 'f'+1
		sabb
		bnl good
fail:		cla
		; any non zero is 'good'
good:		rtr
