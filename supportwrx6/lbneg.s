;
;	Boolean negation for a long
;
	.export bnegeax

	.setcpu 6
	.code

bnegeax:
	ori y,a
	bz set1
	cla
	rtr
set1:
	ina
	rtr
