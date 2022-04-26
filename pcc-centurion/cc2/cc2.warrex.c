/*	$Id: cc2.tms9995.c,v 1.2 2021/12/18 13:26:32 ragge Exp $	*/

/*
 * Copyright (c) 2022 Alan Cox. All rights reserved.
 * Copyright (c) 2021 Anders Magnusson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Dummy cc2 - we don't need to do anything here yet */
#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFLEN 256

int main(int argc, char *argv[])
{
	char buf[BUFLEN], *b, *pname;
	int ch;

	pname = argv[0];
	while ((ch = getopt(argc, argv, "v")) != -1) {
		switch (ch) {
		case 'v':
			break;
		default:
			fprintf(stderr,
			    "usage: %s [-v] [infile [outfile]]\n", pname);
		}
	}
	argc -= optind;
	argv += optind;

	if (argc > 0)
		if (freopen(argv[0], "r", stdin) == NULL)
			err(1, "reopen stdin");
	if (argc > 1)
		if (freopen(argv[1], "w", stdout) == NULL)
			err(1, "reopen stdout");

	for (;;) {
		while ((b = fgets(buf, BUFLEN, stdin)) != NULL)
			printf("%s", buf);
	}
	return 0;
}
