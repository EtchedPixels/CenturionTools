		.code
		.export center
		.export center0
		.export center1
		.export center2


center2:
	lda	2
	bra	centern
center1:
	lda	1
	bra	centern
center0:
	cla
	bra	centern
	; The act of calling this pushed x for us for free, and set x to
	; point to the size info. We go back via jmp (x) as we don't want to
	; undo the work done by jsr
center:
	lda	(x+)
centern:
	xfr	z,a
	sta	(-s)
	xfr	s,z
	add	a,s
	jmp	(x)
