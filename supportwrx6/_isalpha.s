
		.export _isalnum

		.code

_isalnum:
		ldab 3(s)
		ldbb 'A'
		sabb
		bl fail
		ldbb 'Z'+1
		sabb
		bl good
		ldbb 'a'
		sabb
		bl fail
		ldbb 'z'+1
		sabb
		bl good
fail:		cla
		rtr
good:		lda 1
		rtr

