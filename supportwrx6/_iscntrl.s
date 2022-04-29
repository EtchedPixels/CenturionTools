
		.export _iscntrl

		.code

_iscntrl:
		ldab 3(s)
		ldbb 32
		sabb
		bl good
		cla
		rtr
good:		lda 1
		rts
