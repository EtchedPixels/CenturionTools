all:
	(cd as; make)
	(cd ld; make)

install:
	(cd as; make install)
	(cd ld; make install)

clean:
	(cd as; make clean)
	(cd ld; make clean)
	rm *~
