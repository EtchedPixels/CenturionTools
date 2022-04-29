
		.export _isalnum

		.code

_isalnum:
		ldab	3(s)
		ldbb	'0'
		sabb
		bl	fail
		ldbb	'9'+1
		sabb
		bl	good
		ldbb	'A'
		sabb
		bl	fail
		ldbb	'Z'+1
		sabb
		bl	good
		ldbb	'a'
		sabb
		bl	fail
		ldbb	'z'+1
		bl	good
fail:		cla
		rtr
good:
		lda	1
		rtr

