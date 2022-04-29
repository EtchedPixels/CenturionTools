	.setcpu 6
	.code

	.export booleq
	.export boolne
	.export boolle
	.export boollt
	.export boolge
	.export boolgt
	.export boolule
	.export boolult
	.export booluge
	.export boolugt

;
;	Turn  val op test into 1 for true 0 for false. Ensure the Z flag
;	is appropriately set
;

booleq:
	bnz	ret0
ret1:
	lda	1
	rtr

boolne:
	bnz	ret1
ret0:
	cla
	rtr

boollt:
	bz	ret0
boolle:
	ble	ret1
	cla
	rtr

boolge:
	bz	ret1
boolgt:	bgz	ret1
	cla
	rtr

boolugt:
	bz	ret0
booluge:
	bnl	ret1
	cla
	rtr

boolule:
	bz	ret1
boolult:			; use C flag
	lda	0		; doesn't affect carry unlike clr
	rlrb`	al
	rts
