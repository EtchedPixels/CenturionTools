
		.code
		.export cexit
		.setcpu 6

cexit:
	ldx	(s+)
	xfr	x,z
	ldx	(s+)
	rsr
