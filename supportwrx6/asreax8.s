;
;	Right shift the 32bit working register by 8
;
	.export asreax8

	.setcpu 6

	.code

asreax8:
	xfrb	ah,al
	xfrb	yl,ah
	xfrb	yh,yl
	bpl	nosign
	clrb	yh
	decb	yh
	rtr
nosign:
	clrb	yh
	rtr

