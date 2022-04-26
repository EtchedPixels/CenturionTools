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

#define ARGINIT		48	/* # bits above fp where arguments start */
#define AUTOINIT	(-16)	/* # bits below fp where automatics start */

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
#define ALINT		16
#define ALFLOAT		16
#define ALDOUBLE	16
#define ALLDOUBLE	16
#define ALLONG		16
#define ALLONGLONG	16
#define ALSHORT		16
#define ALPOINT		16
#define ALSTRUCT	16
#define ALSTACK		16 

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

//FIXME??#define	FINDMOPS	/* We have memory operations */
#define	MYDOTFILE
#define	printdotfile(x)

#define szty(t) ((t) == DOUBLE || (t) == LONGLONG || (t) == ULONGLONG ? 4 : \
	(t) == FLOAT || (t) == LONG || (t) == ULONG ? 2 : 1)

/*
 *	We have between 124 and 200 or so 8bit registers. This doesn't
 *	fit the 'max 31 regs in a class' model or the sizes we want
 *
 *	Instead we build for a virtual CPU with
 *	32 16bit registers	W0-W31 	(no R31 due to limit - used as FP)
 *	16 32bit registers	L0-L31	(no L31 due to limit)
 *
 *	This takes us to=
 *	32 * 4 = 64		(including FP)
 *	14 * 4 = 56		= 120 registers
 *
 *	The stack pointer is actually outside of this range as is PC
 *	We have 4 registers free as scratch.
 *
 *	Our goal is to keep everything in registers all the time
 *
 *
 */

/* Fast word registers */
#define RW00	0x00
#define RW01	0x01
#define RW02	0x02
#define RW03	0x03
#define RW04	0x04
#define RW05	0x05
#define RW06	0x06
#define RW07	0x07
#define RW08	0x08
#define RW09	0x09
#define RW10	0x0A
#define RW11	0x0B
#define RW12	0x0C
#define RW13	0x0D
#define RW14	0x0E
#define RW15	0x0F
#define RW16	0x10
#define RW17	0x11
#define RW18	0x12
#define RW19	0x13
#define RW20	0x14
#define RW21	0x15
#define RW22	0x16
#define RW23	0x17
#define RW24	0x18
#define RW25	0x19
#define RW26	0x1A
#define RW27	0x1B
#define RW28	0x1C
#define RW29	0x1D
#define RW30	0x1E
#define RW31	0x1F

#define RL00	0x20
#define RL01	0x21
#define RL02	0x22
#define RL03	0x23
#define RL04	0x24
#define RL05	0x25
#define RL06	0x26
#define RL07	0x27
#define RL08	0x28
#define RL09	0x29
#define RL10	0x2A
#define RL11	0x2B
#define RL12	0x2C
#define RL13	0x2D
#define RL14	0x2E
#define RL15	0x2F

#define	MAXREGS	0x40	/* 64 registers */

#define	RSTATUS	\
	/* First 16 temporary */\
	SAREG|TEMPREG, SAREG|TEMPREG, SAREG|TEMPREG, SAREG|TEMPREG,\
	SAREG|TEMPREG, SAREG|TEMPREG, SAREG|TEMPREG, SAREG|TEMPREG,\	
	SAREG|TEMPREG, SAREG|TEMPREG, SAREG|TEMPREG, SAREG|TEMPREG,\	
	SAREG|TEMPREG, SAREG|TEMPREG, SAREG|TEMPREG, SAREG|TEMPREG,\	
	/* Second 16 perm */\
	SAREG|PERMREG, SAREG|PERMREG, SAREG|PERMREG, SAREG|PERMREG,\
	SAREG|PERMREG, SAREG|PERMREG, SAREG|PERMREG, SAREG|PERMREG,\	
	SAREG|PERMREG, SAREG|PERMREG, SAREG|PERMREG, SAREG|PERMREG,\	
	SAREG|PERMREG, SAREG|PERMREG, SAREG|PERMREG, 0 /* FP */,\	
	/* Long registers */\
	SBREG|TEMPREG, SBREG|TEMPREG, SBREG|TEMPREG, SBREG|TEMPREG,\
	SBREG|TEMPREG, SBREG|TEMPREG, SBREG|TEMPREG, SBREG|TEMPREG,\	
	SBREG|PERMREG, SBREG|PERMREG, SBREG|PERMREG, SBREG|PERMREG,\	
	SBREG|PERMREG, SBREG|PERMREG, 0, 0,\
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

#define	ROVERLAP \
\
	/* 32 word registers */\
	{ /* RW0 */ -1 },\
	{ /* RW1 */ -1 },\
	{ /* RW2 */ -1 },\
	{ /* RW3 */ -1 },\
	{ /* RW4 */ -1 },\
	{ /* RW5 */ -1 },\
	{ /* RW6 */ -1 },\
	{ /* RW7 */ -1 },\
	{ /* RW8 */ -1 },\
	{ /* RW9 */ -1 },\
	{ /* RW10 */ -1 },\
	{ /* RW11 */ -1 },\
	{ /* RW12 */ -1 },\
	{ /* RW13 */ -1 },\
	{ /* RW14 */ -1 },\
	{ /* RW15 */ -1 },\
	{ /* RW16 */ -1 },\
	{ /* RW17 */ -1 },\
	{ /* RW18 */ -1 },\
	{ /* RW19 */ -1 },\
	{ /* RW20 */ -1 },\
	{ /* RW21 */ -1 },\
	{ /* RW22 */ -1 },\
	{ /* RW23 */ -1 },\
	{ /* RW24 */ -1 },\
	{ /* RW25 */ -1 },\
	{ /* RW26 */ -1 },\
	{ /* RW27 */ -1 },\
	{ /* RW28 */ -1 },\
	{ /* RW29 */ -1 },\
	{ /* RW30 */ -1 },\
	{ /* RW31 */ -1 },\
	/* 16 long registers */\
	{ /* RW0 */ -1 },\
	{ /* RW1 */ -1 },\
	{ /* RW2 */ -1 },\
	{ /* RW3 */ -1 },\
	{ /* RW4 */ -1 },\
	{ /* RW5 */ -1 },\
	{ /* RW6 */ -1 },\
	{ /* RW7 */ -1 },\
	{ /* RW8 */ -1 },\
	{ /* RW9 */ -1 },\
	{ /* RW10 */ -1 },\
	{ /* RW11 */ -1 },\
	{ /* RW12 */ -1 },\
	{ /* RW13 */ -1 },\
	{ /* RW14 */ -1 },\
	{ /* RW15 */ -1 },\


/* Return a register class based on the type of the node */

#define PCLASS(p) (p->n_type < LONG || p->n_type > BTMASK ? SAREG : \
		  (p->n_type == LONG || p->n_type == ULONG ? SBREG : SBREG))

#define	NUMCLASS 	2	/* highest number of reg classes used */

int COLORMAP(int c, int *r);
#define	GCLASS(x) (x < 32 ? CLASSA : x < 64 ? CLASSB : x < 96 ? CLASSC : CLASSD)
#define DECRA(x,y)	(((x) >> (y*6)) & 63)	/* decode encoded regs */
#define	ENCRD(x)	(x)		/* Encode dest reg in n_reg */
#define ENCRA1(x)	((x) << 6)	/* A1 */
#define ENCRA2(x)	((x) << 12)	/* A2 */
#define ENCRA(x,y)	((x) << (6+y*6))	/* encode regs in int */
#define	RETREG(x)	((x) == LONG || (x) == ULONG ? RP01 : \
	(x) == FLOAT || (x) == DOUBLE ? FR0 : \
	(x) == LONGLONG || (x) == ULONGLONG ? LL0: R1)

//#define R2REGS	1	/* permit double indexing */

#define SP	0x3F	/* Just some random cookie to use */

/* XXX - to die */
#define FPREG	RW31	/* frame pointer */
#define STKREG	SP	/* stack pointer */

/* floating point definitions */
#define	FDFLOAT
#define	DEFAULT_FPI_DEFS { &fpi_ffloat, &fpi_ffloat, &fpi_ffloat }
