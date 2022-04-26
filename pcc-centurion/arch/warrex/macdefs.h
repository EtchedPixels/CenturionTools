/*	$Id: macdefs.h,v 1.18 2021/10/08 15:59:07 ragge Exp $	*/
/*
 * Copyright (c) 2022 Alan Cox <etchedpixels@gmail.com>
 * Copyright (c) 2003 Anders Magnusson (ragge@ludd.luth.se).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
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

/*
 * Machine-dependent defines for both passes.
 */

/*
 * Convert (multi-)character constant to integer.
 */
#define makecc(val,i)	lastcon = i ? (val<<8)|lastcon : val

#define ARGINIT		(32)	/* # bits above fp where arguments start */
#define AUTOINIT	(0)	/* # bits below fp where automatics start */

/*
 * Storage space requirements
 */
#define SZCHAR		8
#define SZBOOL		8
#define SZINT		16
#define SZFLOAT		32
#define SZDOUBLE	64
#define SZLDOUBLE	64
#define SZLONG		32
#define SZSHORT		16
#define SZLONGLONG	64
#define SZPOINT(t)	16

/*
 * Alignment constraints
 */
#define ALCHAR		8
#define ALBOOL		8
#define ALINT		8
#define ALFLOAT		8
#define ALDOUBLE	8
#define ALLDOUBLE	8
#define ALLONG		8
#define ALLONGLONG	8
#define ALSHORT		8
#define ALPOINT		8
#define ALSTRUCT	8
#define ALSTACK		8 

/*
 * Min/max values.
 */
#define	MIN_CHAR	-128
#define	MAX_CHAR	127
#define	MAX_UCHAR	255
#define	MIN_SHORT	-32768
#define	MAX_SHORT	32767
#define	MAX_USHORT	65535
#define	MIN_INT		(-0x7fff-1)
#define	MAX_INT		0x7fff
#define	MAX_UNSIGNED	0xffff
#define	MIN_LONG	(-0x7fffffff-1)
#define	MAX_LONG	0x7fffffff
#define	MAX_ULONG	0xffffffff
#define	MIN_LONGLONG	0x8000000000000000LL
#define	MAX_LONGLONG	0x7fffffffffffffffLL
#define	MAX_ULONGLONG	0xffffffffffffffffULL

/* Default char is unsigned */
#define	CHAR_UNSIGNED
#define	BOOL_TYPE	UNSIGNED	/* what used to store _Bool */

/*
 * Use large-enough types.
 */
typedef	long long CONSZ;
typedef	unsigned long long U_CONSZ;
typedef long long OFFSZ;

#define CONFMT	"%llo"		/* format for printing constants */
#define LABFMT	"L%d"		/* format for printing labels */
#ifdef LANG_F77
#define BLANKCOMMON "_BLNK_"
#define MSKIREG  (M(TYSHORT)|M(TYLONG))
#define TYIREG TYLONG
#define FSZLENG  FSZLONG
#define	AUTOREG	EBP
#define	ARGREG	EBP
#define ARGOFFSET 4
#endif

#define STACK_DOWN 		/* stack grows negatively for automatics */

#undef	FIELDOPS		/* no bit-field instructions */
#define TARGET_ENDIAN TARGET_BE /* big endian */
#define	MYINSTRING
#define	MYALIGN

/* Definitions mostly used in pass2 */

#define BYTEOFF(x)	((x)&01)
#define wdal(k)		(BYTEOFF(k)==0)

#define STOARG(p)
#define STOFARG(p)
#define STOSTARG(p)

#define	FINDMOPS	/* We have memory operations */
#define	MYDOTFILE
#define	printdotfile(x)

#define szty(t) ((t) == DOUBLE || (t) == LONGLONG || (t) == ULONGLONG ? 4 : \
	(t) == FLOAT || (t) == LONG || (t) == ULONG ? 2 : 1)

/*
 *	Classes for now
 *	A	integer and char
 *	B	register pairs for long
 *	C	float (to be decided)
 *	D	long long (not implemented)
 *
 */

#define	R_A	0x00	/* Main accumulator */
#define	R_B	0x01	/* Secondary accumulator */
#define	R_X	0x02	/* Link and index */
#define	R_Y	0x03	/* Scratch */
#define	R_Z	0x04	/* Frame pointer */
#define	R_S	0x05	/* Stack */
#define	R_F	0x06	/* Magic */
#define	R_P	0x07	/* More magic */

#define	SP	R_S

#define	R_YA	0x08	/* Register longs or many pairs due to all the helpers */
#define	R_BA	0x09	/* Pairs. We don't seem to gain much from */
#define	R_XB	0x0A	/* Register longs or many pairs due to all the helpers */
#define	R_XY	0x0B

#define	FR0	0x10	/* Results accumulate here */
#define FR1	0x11	/* Fake FP register to keep code generator sane */
#define FR2	0x12	/* Ditto */

#define	LL0	0x18
#define	LL1	0x19
#define	LL2	0x1A
#define	LL3	0x1B

#define	MAXREGS	0x1C	

#define	RSTATUS	\
	SAREG|TEMPREG, SAREG|TEMPREG, SAREG|TEMPREG, SAREG|TEMPREG,\
	0, /* Frame pointer */ 0, /* Stack */ 0, 0,\
	SBREG, SBREG, SBREG, 0, 0, 0, 0, 0, \
	SCREG|TEMPREG, SCREG|TEMPREG, SCREG|TEMPREG, 0, 0, 0, 0, 0,\
	SDREG, SDREG, SDREG, SDREG,

#define	ROVERLAP \
	/* 8 basic registers */			\
	{ /* RA: */ R_BA, R_YA, -1 },	\
	{ /* RB: */ R_BA, R_XB, -1 },		\
	{ /* RX: */ R_XB, R_XY, -1 },		\
	{ /* RY: */ R_YA, R_XY, -1 },		\
	{ /* RZ: */ -1 },			\
	{ /* RS: */ -1 },			\
	{ /* RF: */ -1 },			\
	{ /* RP: */ -1 },			\
\
	/* 4 long registers made using pairs */		\
	{ /* RYA: */ R_Y, R_A, R_BA, R_XY, -1 },		\
	{ /* RBA */ R_A, R_B, R_YA, -1 },		\
	{ /* RXB: */ R_X, R_B, R_BA, R_XB, -1 },	\
	{ /* RXY: */ R_X, R_Y, R_XB, R_YA, -1 },	\
	{ -1 },						\
	{ -1 },						\
	{ -1 },						\
	{ -1 },						\
\
	/* The fp register TODO */\
	{ /* FR0 */ -1 },\
	{ /* FR1 */ -1 },\
	{ /* FR2 */ -1 },\
	{ /* FR3 */ -1 },\
	{ /* FR4 */ -1 },\
	{ /* FR5 */ -1 },\
	{ /* FR6 */ -1 },\
	{ /* FR7 */ -1 },\
	{ /* FR8 */ -1 },\
\
	/* Four emulated long long regs */\
	{ /* LL0 */ -1 },\
	{ /* LL1 */ -1 },\
	{ /* LL2 */ -1 }


/* Return a register class based on the type of the node */

#define PCLASS(p) (p->n_type < LONG || p->n_type > BTMASK ? SAREG : \
		  (p->n_type == LONG || p->n_type == ULONG ? SBREG : SCREG))

#define	NUMCLASS 	3	/* highest number of reg classes used */

int COLORMAP(int c, int *r);
#define	GCLASS(x) (x < 8 ? CLASSA : x < 16 ? CLASSB : x < 24 ? CLASSC : CLASSD)
#define DECRA(x,y)	(((x) >> (y*5)) & 31)	/* decode encoded regs */
#define	ENCRD(x)	(x)		/* Encode dest reg in n_reg */
#define ENCRA1(x)	((x) << 5)	/* A1 */
#define ENCRA2(x)	((x) << 10)	/* A2 */
#define ENCRA(x,y)	((x) << (5+y*5))	/* encode regs in int */
#define	RETREG(x)	((x) == LONG || (x) == ULONG ? R_YA : \
	(x) == FLOAT || (x) == DOUBLE ? FR0 : \
	(x) == LONGLONG || (x) == ULONGLONG ? LL0: R_A)

//#define R2REGS	1	/* permit double indexing */

/* XXX - to die */
#define FPREG	R_Z	/* frame pointer */
#define STKREG	SP	/* stack pointer */

/* A bunch of specials to make spotting long * shifts easier */
#define	STWO		(MAXSPECIAL+1)	/* exactly two */
#define	SMTWO		(MAXSPECIAL+2)	/* exactly minus two */

/* floating point definitions */
#define	FDFLOAT
#define	DEFAULT_FPI_DEFS { &fpi_ffloat, &fpi_ffloat, &fpi_ffloat }

