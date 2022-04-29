
		.export _tolower

		.code

_tolower:
		ldaa 3(s)
		ldbb 'A'
		sabb
		bl done
		ldbb 'Z' + 1
		sabb
		bnl done
		ldab 32
		addb bl,al
done:		rtr
