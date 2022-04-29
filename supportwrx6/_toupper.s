
		.export _toupper

		.code

_toupper:
		ldab 3(s)
		ldbb 'a'
		sabb
		bl done
		ldbb 'z'+1
		sabb
		bnl done
		ldbb 32
		subb bl,al
done:		rts
