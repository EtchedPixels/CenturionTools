;
;	Boolean negation for a long
;
	.export bnegeax

	.setcpu 6
	.code

bnegeax:
	ori	y,a
	bz	set1
	cla
	rsr
set1:
	ina
	rsr
