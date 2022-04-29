
		.export _ispunct

		.code

_ispunct:
		ldab 3(s)
		bm fail
		ldbb ' '
		bl fail
		ldbb 'z'+1
		sabb
		bnl good
		ldbb 'a'+1
		sabb
		bnl fail
		ldbb 'Z'+1
		sabb
		bnl good
		ldbb 'A'+1
		sabb
		bnl fail
		ldbb '9'+1
		sabb
		bnl good
		ldbb '0'+1
		sabb
		bnl good
fail:
		cla
good:
		rtr
		; any non zero is 'good
