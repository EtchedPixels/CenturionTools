all: 	ccwrx6
	(cd as; make)
	(cd ld; make)
	(cd common; make)
	(cd ccom; make)
	(cd copt; make)

install:
	mkdir -p /opt/aswrx6/bin
	mkdir -p /opt/aswrx6/include
	mkdir -p /opt/aswrx6/lib
	(cd as; make install)
	(cd ld; make install)
	cp ccwrx6 /opt/aswrx6/bin/
	cp -r include/* /opt/aswrx6/include/

ccwrx6: ccwrx6.c
	gcc -Wall -pedantic -O2 ccwrx6.c -o ccwrx6


# Members for the C library
COBJ =	libc/memcpy.o \
	libc/memset.o \
	libc/strlen.o \
	libc/abort.o \
	libc/abs.o \
	libc/atoi.o \
	libc/atol.o \
	libc/bcmp.o \
	libc/bcopy.o \
	libc/bsearch.o \
	libc/bzero.o \
	libc/calloc.o \
	libc/crypt.o \
	libc/exit.o \
	libc/free.o \
	libc/getopt.o \
	libc/index.o \
	libc/isalnum.o \
	libc/isalpha.o \
	libc/isascii.o \
	libc/isblank.o \
	libc/iscntrl.o \
	libc/isdigit.o \
	libc/isgraph.o \
	libc/islower.o \
	libc/isprint.o \
	libc/ispunct.o \
	libc/isspace.o \
	libc/isupper.o \
	libc/isxdigit.o \
	libc/libintl.o \
	libc/lrand48.o \
	libc/lsearch.o \
	libc/ltoa.o \
	libc/ltostr.o \
	libc/malloc.o \
	libc/memccpy.o \
	libc/memcmp.o \
	libc/memmove.o \
	libc/qsort.o \
	libc/realloc.o \
	libc/rindex.o \
	libc/strcasecmp.o \
	libc/strcasestr.o \
	libc/strcat.o \
	libc/strchr.o \
	libc/strcmp.o \
	libc/strcoll.o \
	libc/strcpy.o \
	libc/strcspn.o \
	libc/strdup.o \
	libc/stricmp.o \
	libc/strlcpy.o \
	libc/strncasecmp.o \
	libc/strncat.o \
	libc/strncmp.o \
	libc/strncpy.o \
	libc/strnicmp.o \
	libc/strnlen.o \
	libc/strpbrk.o \
	libc/strrchr.o \
	libc/strsep.o \
	libc/strspn.o \
	libc/strstr.o \
	libc/strtok.o \
	libc/strtok_r.o \
	libc/strtol.o \
	libc/strxfrm.o \
	libc/swab.o \
	libc/tfind.o \
	libc/tolower.o \
	libc/toupper.o \
	libc/xitoa.o

STDIO =	stdio/fclose.o \
	stdio/fflush.o \
	stdio/fgetc.o \
	stdio/fgetpos.o \
	stdio/fgets.o \
	stdio/fopen.o \
	stdio/fprintf.o \
	stdio/fputc.o \
	stdio/fputs.o \
	stdio/fread.o \
	stdio/fscanf.o \
	stdio/fsetpos.o \
	stdio/ftell.o \
	stdio/fwrite.o \
	stdio/gets.o \
	stdio/getw.o \
	stdio/printf.o \
	stdio/putchar.o \
	stdio/putw.o \
	stdio/sprintf.o \
	stdio/sscanf.o \
	stdio/stdio0.o \
	stdio/vfprintf.o \
	stdio/vfscanf.o \
	stdio/vprintf.o \
	stdio/vscanf.o \
	stdio/vsscanf.o

libc.a:	ccwrx6 $(STDIO) $(COBJ)
	ar rc libc.a $(STDIO) $(COBJ) $(AOBJ)

%.o: %.s
	aswrx6 $^

%.o: %.c
	./ccwrx6 -X -Iinclude -c $^

clean:
	(cd as; make clean)
	(cd ld; make clean)
	(cd common; make clean)
	(cd ccom; make clean)
	rm -f $(COBJ) $(STDIO)
	rm *~
