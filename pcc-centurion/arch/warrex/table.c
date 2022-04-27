/*	$Id: table.c,v 1.17 2019/04/27 20:35:52 ragge Exp $	*/
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


# include "pass2.h"

# define TLL TLONGLONG|TULONGLONG
# define ANYSIGNED TINT|TLONG|TSHORT|TCHAR
# define ANYUSIGNED TUNSIGNED|TULONG|TUSHORT|TUCHAR
# define ANYFIXED ANYSIGNED|ANYUSIGNED
# define TUWORD TUNSIGNED
# define TSWORD TINT
# define TWORD TUWORD|TSWORD
# define ANYSH	SCON|SAREG|SOREG|SNAME
# define ARONS	SAREG|SOREG|SNAME

/*
 *	Be careful of the L/R orders. The TMS99xx instruction set is
 *	source,dest except for immediate operations which are reg, immediate
 *	(ie dest, source).
 */
struct optab table[] = {
/* First entry must be an empty entry */
{ -1, FOREFF, SANY, TANY, SANY, TANY, 0, 0, "", },

/* PCONVs are usually not necessary */
{ PCONV,	INAREG,
	SAREG,	TWORD|TPOINT,
	SAREG,	TWORD|TPOINT,
		0,	RLEFT,
		"", },

/* long -> ptr */
/* No work really needed */
{ PCONV,	INAREG,
	SBREG,			TLONG|TULONG,
	SAREG,			TPOINT,
		NAREG|NASL,	RESC1,
		"xfr	A1,AL", },

/* Load the lower half from memory - second word */
{ PCONV,	INAREG,
	SOREG|SNAME,		TLONG|TULONG,
	SAREG,			TPOINT,
		NSPECIAL|NAREG|NASL,	RESC1,
		"ld	A1,ZL", },

/* char to pointer - no op */
{ PCONV,	INAREG,
	SAREG,		TCHAR|TUCHAR,
	SAREG,			TPOINT,
		0,	RLEFT,
		"", },

{ PCONV,	INAREG,
	SOREG|SNAME,		TCHAR|TUCHAR,
	SAREG,			TPOINT,
		NAREG|NASL,	RESC1,
		"clr	A1h ; pconv\nld	A1l,AL\n", },


/* Unsigned char to char : no work needed */
{ SCONV,	INAREG,
	SAREG,	TUCHAR,
	SAREG,	TCHAR,
		0,	RLEFT,
		"", },

/* Char to unsigned char : no work needed */
{ SCONV,	INAREG,
	SAREG,	TCHAR,
	SAREG,	TUCHAR,
		0,	RLEFT,
		"", },

/* Char to int or unsigned : move byte down and sign extend */
{ SCONV,	INAREG,
	SAREG,	TCHAR,
	SAREG,	TWORD,
		0,	RLEFT,
		"clr	ALh\nadd	ALl,ALh\nZBbnl	ZE\ndcr	ALh\nZD\n", },

/* Char or unsigned char to int or uint: constant */
{ SCONV,	INAREG,
	SCON,			TCHAR|TUCHAR,
	SAREG,			TWORD,
		NSPECIAL,	RLEFT,
		"ld	A1,CL\n", },

/* Unsigned char to int or uint: register */
{ SCONV,	INAREG,
	SAREG,	TUCHAR,
	SAREG,	TWORD,
		0,	RLEFT,
		"clr	ALh\n", },


/* Unsigned char to int or uint: memory */
{ SCONV,	INAREG,
	SOREG|SNAME,	TUCHAR,
	SAREG,	TINT|TUNSIGNED,
		NSPECIAL|NAREG,	RESC1,
		"clr	A1h ; sconv\nld	A1l,AL\n", },

/* char to ulong: register */
{ SCONV,	INBREG,
	SAREG,	TCHAR,
	SBREG,	TULONG|TLONG,
		NBREG,	RESC1,
		"clr	Z1h\nxfr	Z1l,AL\nclr	U1\nZBadd	Z1l,Z1h\nZBbnl	ZE\ndcr	U1\ndcr	Z1h\nZD\n", },

/* Unsigned char to unsigned long: register */
{ SCONV,	INBREG,
	SAREG,	TUCHAR,
	SBREG,	TLONG|TULONG,
		NBREG|NBSL,	RESC1,
		"clr	Z1h\nld	Z1l,AL,Z1\nclr	U1\n", },

/* Unsigned char to float/double */
/* Constant is forced into R1, masked and converted */
{ SCONV,	INCREG,
	SAREG,	TUCHAR,
	SANY,	TFLOAT,
		NSPECIAL|NCREG,	RESC1,
		"jsr u8fp\nn", },

/* Signed char to float/double */
/* Constant is forced into R1, masked and converted */
{ SCONV,	INCREG,
	SAREG,	TCHAR,
	SANY,	TFLOAT,
		NSPECIAL|NCREG,	RESC1,
		"jsr	s8fp\n", },

/* Int or unsigned to char or unsigned: register */
{ SCONV,	INAREG,
	SAREG,	TWORD|TPOINT,
	SANY,	TCHAR|TUCHAR,
		0,	RLEFT,
		"", },

/* Int or unsigned to int or unsigned: register */
{ SCONV,	INAREG,
	SAREG,	TWORD,
	SANY,	TWORD,
		0,	RLEFT,
		"", },

/* Pointer to char: register */
{ SCONV,	INAREG,
	SAREG,	TPOINT,
	SAREG,	TCHAR,
		0,	RLEFT,
		"", },

/* Pointer to int or uint: no work */
{ SCONV,	INAREG,
	SAREG,	TPOINT,
	SANY,	TWORD,
		0,	RLEFT,
		"", },

/* Int to long/ulong:  Expands due to lack of sign extend */
{ SCONV,	INBREG,
	SAREG,	TINT|TPOINT,
	SANY,	TLONG|TULONG,
		NBREG,	RESC1,
		"xfr	AL,Z1\nclr	U1\nadd	AL,U1\nZBbnl	ZE\ndcr	U1\nZD\n", },

/* int to float: helper */
{ SCONV,	INCREG,
	SAREG,	TINT,
	SANY,	TFLOAT,
		NSPECIAL|NCREG,	RESC1,
		"TODO\n", },

/* int to float: object in memory: actually easier */
{ SCONV,	INCREG,
	SNAME|SOREG,	TINT,
	SANY,	TFLOAT,
		NSPECIAL|NCREG,	RESC1,
		"TODO\n", },

/* The compiler isn't too smart at folding matching registers into pairs */
/* unsigned int to long or ulong: just clear the upper */
{ SCONV,	INBREG,
	SAREG,	TUNSIGNED,
	SBREG,	TLONG|TULONG,
		NBREG,	RESC1,
		"xfr	Z1,AL\nclr	U1; sconvu16u32 AL,A1\n", },
/* unsigned int to long or ulong: generic move and conversion for unsigned to long/ulong */
{ SCONV,	INBREG,
	SOREG|SNAME,	TUNSIGNED,
	SBREG,	TLONG|TULONG,
		NSPECIAL|NBREG,	RESC1,
		"ld	Z1,AL\nclr	U1\n", },

/* unsigned int to float : we don't have a 16bit op for it. use the 32bit one */
{ SCONV,	INCREG,
	SAREG,		TUNSIGNED,
	SANY,		TFLOAT,
		NSPECIAL,	RLEFT,
		"TODO\n", },

/* long or unsigned long to char or uchar */
{ SCONV,	INAREG,
	SBREG,		TLONG|TULONG,
	SAREG,		TCHAR|TUCHAR,
		NAREG,		RESC1,
		"xfr	A1,ZL; sconv breg areg AL, A1\nclr	A1h\n", },

{ SCONV,	INAREG,
	SOREG|SNAME,	TLONG|TULONG,
	SAREG,		TCHAR|TUCHAR,
		NSPECIAL|NAREG,		RESC1,
		"ld	A1l,ZL; sconv breg areg AL, A1\nclr	A1h\n", },

/* long or unsigned long to int or uint: no work required */
/* Alas the compiler isn't quite smart enough for this to happen in situ */
{ SCONV,	INAREG,
	SBREG,		TLONG|TULONG,
	SAREG,		TWORD|TPOINT,
		NAREG|NASL,	RESC1,	/*XX */
		"xfr	A1,ZL; sconv breg areg AL, A1\n", },

/* (u)long -> (u)long, nothing */
{ SCONV,	INBREG,
	SBREG,	TLONG|TULONG,
	SANY,	TLONG|TULONG,
		0,	RLEFT,
		"", },

/* long -> float/double */
{ SCONV,	INCREG,
	SNAME|SOREG,	TLONG,
	SANY,		TFLOAT,
		NSPECIAL,	RLEFT,
		"TODO", },

/* ulong -> float/double */
{ SCONV,	INCREG,
	SBREG,		TULONG,
	SANY,		TFLOAT,
		NSPECIAL|NCSL,	RLEFT,
		"jsr	u32fp\n", },


/* float to long or ulong : helper converts and flips in situ */
/* CRE needs the words flipping */
{ SCONV,	INBREG,
	SCREG,	TFLOAT,
	SANY,	TLONG|TULONG,
		NSPECIAL,	RLEFT,
		"jsr	cre_flip\n" },

/* float to int : special rule plonks it in the right place */
{ SCONV,	INAREG,
	SCREG,	TFLOAT,
	SANY,	TINT,
		NSPECIAL,	RLEFT,
		"TODO\n", },

/* float/double -> float/double */
{ SCONV,	INCREG,
	SCREG,	TFLOAT,
	SANY,	TANY,
		0,	RLEFT,
		"", },

/*
 * Subroutine jsrs.
 */

/* Call by address of a function returning into an areg */

{ CALL,		INAREG,
	SCON,	TANY,
	SAREG,	TWORD|TPOINT|TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"jsr	AL\nZC", },

{ UCALL,	INAREG,
	SCON,	TANY,
	SAREG,	TWORD|TPOINT|TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"jsr	AL\nZC", },

/* Call by pointer of a function returning into an areg */

{ CALL,		INAREG,
	SAREG|SNAME|SOREG,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,	/* should be 0 */
		"jsr	(AL)\nZC", },

{ UCALL,	INAREG,
	SAREG|SNAME|SOREG,	TANY,
	SAREG,	TWORD|TPOINT|TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"jsr	(AL)\nZC", },

/* Call by address of a function returning into a breg (long) */

{ CALL,		INBREG,
	SCON,	TANY,
	SBREG|SNAME|SOREG,	TLONG|TULONG,
		NBREG|NBSL,	RESC1,
		"jsr	AL\nZC", },

{ UCALL,	INBREG,
	SCON,	TANY,
	SBREG,	TLONG|TULONG,
		NBREG|NBSL,	RESC1,
		"jsr	AL\nZC", },

/* Call by pointer of a function returning into a breg (long) */

{ CALL,		INBREG,
	SAREG,	TANY,
	SBREG,	TLONG|TULONG,
		NBREG|NBSL,	RESC1,
		"jsr	(AL)\nZC", },

{ UCALL,	INBREG,
	SAREG,	TANY,
	SBREG,	TLONG|TULONG,
		NBREG|NBSL,	RESC1,
		"jsr	(AL)\nZC", },

/* Call by address of a function returning into a creg (long) */

{ CALL,		INCREG,
	SCON|SNAME|SOREG,	TANY,
	SCREG,	TFLOAT,
		NCREG|NCSL,	RESC1,
		"jsr	AL\nZC", },

{ UCALL,	INCREG,
	SCON|SNAME|SOREG,	TANY,
	SCREG,	TFLOAT,
		NCREG|NCSL,	RESC1,
		"jsr	AL\nZC", },

/* Call by pointer of a function returning into a creg (long) */

{ CALL,		INCREG,
	SAREG|SNAME|SOREG,	TANY,
	SCREG,	TFLOAT,
		NCREG|NCSL,	RESC1,
		"jsr	(AL)\nZC", },

{ UCALL,	INCREG,
	SAREG|SNAME|SOREG,	TANY,
	SCREG,	TFLOAT,
		NCREG|NCSL,	RESC1,
		"jsr	(AL)\nZC", },

/* Functions being called for effect only */
{ CALL,		FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	0,
		"jsr	AL\nZC", },

{ UCALL,	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	0,
		"jsr	AL\nZC", },

{ CALL,		FOREFF,
	SAREG|SNAME|SOREG,	TANY,
	SANY,	TANY,
		0,	0,
		"jsr	(AL)\nZC", },

{ UCALL,	FOREFF,
	SAREG|SNAME|SOREG,	TANY,
	SANY,	TANY,
		0,	0,
		"jsr	(AL)\nZC", },

/* Function calls returning struct */
{ STCALL,	INAREG,
	SCON|SOREG|SNAME,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,
		"jsr	AL\nZC", },

{ USTCALL,	INAREG,
	SCON|SOREG|SNAME,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,
		"jsr	AL\nZC", },

{ STCALL,	FOREFF,
	SCON|SOREG|SNAME,	TANY,
	SANY,	TANY,
		0,	0,
		"jsr	AL\nZC", },

{ USTCALL,	FOREFF,
	SCON|SOREG|SNAME,	TANY,
	SANY,	TANY,
		0,	0,
		"jsr	AL\nZC", },

{ STCALL,	INAREG,
	SAREG,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,	/* should be 0 */
		"jsr	(AL)\nZC", },

{ USTCALL,	INAREG,
	SAREG,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,	/* should be 0 */
		"jsr	(AL)\nZC", },

/*
 * The next rules handle all binop-style operators.
 */

/* Add one to anything left but use only for side effects */
{ PLUS,		FOREFF|INAREG|FORCC,
	SAREG,		TWORD|TPOINT,
	SONE,		TANY,
		0,	RLEFT|RESCC,
		"inr	AL\n", },

/* No ADC */
{ PLUS,		INBREG|FOREFF,
	SBREG,		TLONG|TULONG,
	SONE,		TLONG|TULONG,
		NSPECIAL,	RLEFT,
		"jsr	inc32\n", },

{ PLUS,		INBREG|FOREFF,
	SBREG,		TLONG|TULONG,
	SBREG,		TLONG|TULONG,
		0,	RLEFT,
		"ZBadd	UL,UR\nadd	ZL,ZR\njnc	ZE\ninr	UL\nZD\n", },

/* Integer to pointer addition */
{ PLUS,		INAREG,
	SAREG,	TPOINT|TWORD,
	SAREG,	TINT|TUNSIGNED,
		0,	RLEFT,
		"add	AL,AR\n", },

{ PLUS,		INAREG|FOREFF|FORCC,
	SAREG,		TCHAR|TUCHAR,
	SAREG,		TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"add	ALl,ARl\n", },

/* floating point */

{ PLUS,		INCREG|FOREFF|FORCC,
	SCREG,			TFLOAT,
	SCREG|SNAME|SOREG,	TFLOAT,
		NSPECIAL,	RDEST,
		"ar	AR ; plus into AL\n", },


{ MINUS,		INBREG|FOREFF,
	SBREG,		TLONG|TULONG,
	SONE,		TLONG|TULONG,
		NSPECIAL,	RLEFT,
		"jsr	dec32\n", },

{ MINUS,		INBREG|FOREFF,
	SBREG,		TLONG|TULONG,
	SBREG,		TLONG|TULONG,
		NSPECIAL,	RDEST,
		"jsr	sub32\n", },

/* Sub one from anything left */
{ MINUS,	FOREFF|INAREG|FORCC,
	SAREG,		TWORD|TPOINT,
	SONE,		TANY,
		0,	RLEFT|RESCC,
		"dcr	AL\n", },

{ MINUS,		INAREG|FOREFF,
	SAREG,		TWORD|TPOINT,
	SAREG,		TWORD|TPOINT,
		0,	RLEFT,
		"sub	AL,AR\n", },

/* Sub one left but use only for side effects */
{ MINUS,	FOREFF,
	SAREG,		TCHAR|TUCHAR,
	SONE,			TANY,
		0,	RLEFT,
		"dcr	ALl\n", },

{ MINUS,		FOREFF,
	SAREG,		TCHAR|TUCHAR,
	SAREG,		TCHAR|TUCHAR,
		0,	RLEFT,
		"sub	ALl,ARl\n", },

/* floating point */
{ MINUS,	INCREG|FOREFF|FORCC,
	SCREG,			TFLOAT,
	SCREG|SNAME|SOREG,	TFLOAT,
		NSPECIAL,	RDEST|RESCC,
		"sr	AR; subtract from AL\n", },

/*
 * The next rules handle all shift operators.
 * We only have simple shift instructions
 */
 
{ LS,	INBREG|FOREFF,
	SBREG,	TLONG|TULONG,
	SONE,	TANY,
		NSPECIAL,	RLEFT,
		"slr	ZL\nrlr	UL\n", },

{ LS,	INBREG|FOREFF,
	SBREG,	TLONG|TULONG,
	SAREG,	TINT|TUNSIGNED,
		NSPECIAL,	RLEFT,
		"jsr	ls32\n", },

{ LS,	INAREG|FOREFF,
	SAREG,	TWORD,
	SONE,	TANY,
	0,	RLEFT,
		"slr		AL\n", },

{ LS,	INAREG|FOREFF,
	SAREG,	TWORD,
	STWO,	TANY,
	0,	RLEFT,
		"slr	AL\nslr	AL\n", },

{ LS,	INAREG|FOREFF,
	SAREG,	TCHAR|TUCHAR,
	STWO,	TANY,
	0,	RLEFT,
		"slr	ALl\nslr	AL1\n", },

{ LS,	INAREG|FOREFF,
	SAREG,	TCHAR|TUCHAR,
	SONE,	TANY,
	0,	RLEFT,
		"slr	ALl\n", },

/* FIXME: do we need to trash the right side ? */
{ LS,	INAREG|FOREFF,
	SAREG,	TCHAR|TUCHAR,
	SAREG,	TWORD,
	0,	RLEFT,
		"ZBst	AR,(-s)\nxfr AR,AR\nbz ZE\nloop_ZD\nslr	AL\ndcr	AR\nbnz loop_ZE\nZD\nld AR,(s+)\n", },

{ LS,	INAREG|FOREFF,
	SAREG,	TWORD,
	SAREG,	TWORD,
	NSPECIAL,	RLEFT,
		"ZBxfr AR,AR\nbz ZE\nloop_ZD\nslr	AL\ndcr	AR\nbnz loop_ZE\nZD\n", },

/* Same again - right unsigned 8 or 16bit shifts */

/* Shift of a register by a constant, works for 8 and 16bit */
{ RS,	INAREG|FOREFF,
	SAREG,	TUNSIGNED,
	SONE,	TANY,
	0,	RLEFT,
		"fcc\nrrr	AL\n", },
		
{ RS,	INAREG|FOREFF,
	SAREG,	TUCHAR,
	SONE,	TANY,
	0,	RLEFT,
		"fcc\nrrr	ALl\n", },
		
{ RS,	INAREG|FOREFF,
	SAREG,	TUNSIGNED,
	SAREG,	TCHAR|TUCHAR|TWORD,
	NSPECIAL,	RLEFT,
		"jsr	rsu16", },

{ RS,	INAREG|FOREFF,
	SAREG,	TUCHAR|TUNSIGNED,
	SAREG,	TCHAR|TUCHAR|TWORD,
	NSPECIAL,	RLEFT,
		"jsr	rsu8", },

/* And signed */

{ RS,	INAREG|FOREFF,
	SAREG,	TUNSIGNED,
	SONE,	TANY,
	0,	RLEFT,
		"srr	AL\n", },
		
{ RS,	INAREG|FOREFF,
	SAREG,	TUCHAR,
	SONE,	TANY,
	0,	RLEFT,
		"srr	ALl\n", },
		
{ RS,	INAREG|FOREFF,
	SAREG,	TUNSIGNED,
	SAREG,	TCHAR|TUCHAR|TWORD,
	NSPECIAL,	RLEFT,
		"jsr	rss16", },

{ RS,	INAREG|FOREFF,
	SAREG,	TUCHAR,
	SAREG,	TCHAR|TUCHAR|TWORD,
	NSPECIAL,	RLEFT,
		"jsr	rss8", },

/* And 32bit */

{ RS,	INBREG|FOREFF,
	SBREG,	TLONG,
	SAREG,	TWORD|TCHAR|TUCHAR,
		NSPECIAL,	RLEFT,
		"jsr	rss32\n", },

{ RS,	INBREG|FOREFF,
	SBREG,	TULONG,
	SAREG,	TWORD|TCHAR|TUCHAR,
		NSPECIAL,	RLEFT,
		"jsr	rsu32\n", },

/*
 * The next rules takes care of assignments. "=".
 */

/* First optimizations, in lack of weight it uses first found */
/* Start with class A registers */

/* Clear word at reg */
{ ASSIGN,	FOREFF|INAREG,
	SAREG,	TWORD|TPOINT|TCHAR|TUCHAR,
	SZERO,		TANY,
		0,	RDEST,
		"clr	AL ; assign AR to AL\n", },

/* The next is class B regs */

/* Must have multiple rules for long otherwise regs may be trashed */
{ ASSIGN,	FOREFF|INBREG,
	SBREG,			TLONG|TULONG,
	SCON,			TLONG|TULONG,
		NSPECIAL,	RDEST,
		"ZN", },

{ ASSIGN,	FOREFF|INBREG,
	SBREG,			TLONG|TULONG,
	SNAME|SOREG,		TLONG|TULONG,
		0,	RDEST,
		"ZP", },

/*
 *	We run into lack of register problems here. Needs more looking
 *	into. For now push it down into our specials to cope with the y
 *	cases.
 */
{ ASSIGN,	FOREFF|INBREG,
	SNAME|SOREG,		TLONG|TULONG,
	SBREG,			TLONG|TULONG,
		0,	RDEST,
		"ZK", },

{ ASSIGN,	INBREG|FOREFF,
	SBREG,	TLONG|TULONG,
	SBREG,	TLONG|TULONG,
		0,	RDEST,
		"ZG", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SAREG,			TWORD|TPOINT,
	SCON,			TWORD|TPOINT,
		NSPECIAL,	RDEST|RESCC,
		"ld	AL,CR\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SAREG,			TWORD|TPOINT,
	SNAME|SOREG,		TWORD|TPOINT,
		NSPECIAL,	RDEST|RESCC,
		"ld	AL,AR\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SNAME|SOREG,		TWORD|TPOINT,
	SAREG,			TWORD|TPOINT,
		0,	RDEST|RESCC,
		"ZT\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SAREG,	TWORD|TPOINT,
	SAREG,	TWORD|TPOINT,
		0,	RDEST|RESCC,
		"xfr	AL,AR; 2\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SAREG,		TCHAR|TUCHAR,
	ARONS	,	TCHAR|TUCHAR,
		NSPECIAL,	RDEST|RESCC,
		"ld	ALl,AR ; asb\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SAREG,		TCHAR|TUCHAR,
	SCON,		TCHAR|TUCHAR,
		NSPECIAL,	RDEST|RESCC,
		"ld	ALl, CR\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	ARONS,	TCHAR|TUCHAR,
	SAREG,	TCHAR|TUCHAR,
		0,	RDEST|RESCC,
		"ZU\n", },

/* Floating point */

{ ASSIGN,	FOREFF|INCREG,
	SCREG|SNAME|SOREG,	TFLOAT,
	SCREG|SNAME|SOREG,	TFLOAT,
	0,	RDEST,
		"; xfr AR,AL\nZH", },

/* Struct assigns */
{ STASG,	FOREFF|INAREG,
	SOREG|SNAME,	TANY,
	SAREG,		TPTRTO|TANY,
		NSPECIAL,	RDEST,
		"ZI", },
/*
 * DIV/MOD/MUL 
 *
 * FIXME: Rules
 *	Signed: DIVS or MPYS take an address for one argument, the other
 *	is in R0 and the result ends up in R0/R1
 *	Unsigned: DIVS/MPYS take a reg and an arg and use reg/reg+1 for
 *	result
 *
 *	This is a fudge to get us working
 */
{ MUL,	INAREG,
	SAREG,			TWORD|TPOINT,
	SAREG,			TWORD|TPOINT,
		NSPECIAL,	RLEFT,
		"jsr	mpy16\n", },

{ MUL,	INBREG,
	SBREG,			TLONG|TULONG,
	SBREG,			TLONG|TULONG,
		NSPECIAL,		RLEFT,
		"jsr	mpy32\n", },

{ MUL,	INCREG,
	SCREG,			TFLOAT,
	SCREG|OREG|SNAME,	TFLOAT,
		NSPECIAL,	RDEST,
		"TODO\n", },

{ DIV,	INAREG,
	SAREG,			TINT|TPOINT,
	SAREG,			TINT|TPOINT,
		NSPECIAL,	RDEST,
		"jsr	divs16\n", },

/* div can use things other than r0/r1 but we don't */
{ DIV,	INAREG,
	SAREG,			TUNSIGNED,
	SAREG,			TUNSIGNED,
		NSPECIAL,	RDEST,
		"jsr	divu16\n", },

{ DIV,	INBREG,
	SBREG,			TLONG,
	SCON,			TLONG,
		NSPECIAL,		RLEFT,
		"jsr	divs32i\n.word	ZQ\n.word	CR\n", },

{ DIV,	INBREG,
	SBREG,			TLONG|TULONG,
	SCON,			TLONG|TULONG,
		NSPECIAL,		RLEFT,
		"jsr	div32i\n.word	ZQ\n.word	CR\n", },

{ DIV,	INBREG,
	SBREG,			TLONG,
	SBREG,			TLONG,
		NSPECIAL,		RLEFT,
		"jsr	div32\n", },

{ DIV,	INBREG,
	SBREG,			TLONG|TULONG,
	SBREG,			TLONG|TULONG,
		NSPECIAL,		RDEST,
		"jsr	div32\n", },

{ DIV,	INCREG,
	SCREG,			TFLOAT,
	SCREG|SNAME|SOREG,		TFLOAT,
		NSPECIAL,	RDEST,
		"TODO\n", },

{ MOD,	INAREG,
	SAREG,			TINT,
	SAREG,			TINT,
		NSPECIAL,	RDEST,
		"jsr	mods16\n", },

{ MOD,	INAREG,
	SAREG,			TUNSIGNED|TPOINT,
	SAREG,			TUNSIGNED|TPOINT,
		NSPECIAL,	RDEST,
		"jsr	mods32\n", },

{ MOD,	INBREG,
	SBREG,			TLONG,
	SCON,			TLONG,
		NSPECIAL,		RLEFT,
		"jsr	mods32i\n.word	ZQ\n.word	CR\n", },

{ MOD,	INBREG,
	SBREG,			TLONG|TULONG,
	SCON,			TLONG|TULONG,
		NSPECIAL,		RLEFT,
		"jsr	mod32i\n.word	ZQ\n.word	CR\n", },

{ MOD,	INBREG,
	SBREG,			TLONG,
	SBREG,			TLONG,
		NSPECIAL,		RLEFT,
		"jsr	mods32\n", },

{ MOD,	INBREG,
	SBREG,			TLONG|TULONG,
	SBREG,			TLONG|TULONG,
		NSPECIAL,		RLEFT,
		"jsr	mod32\n", },

/*
 * Indirection operators.
 */

{ UMUL,	INBREG,
	SANY,		TANY,
	SNAME|SOREG,		TLONG|TULONG,
	NSPECIAL|NBREG,	RESC1, /* |NBSL - may overwrite index reg */
		"ld	Z1,ZR; umull\nld	U1,UR\n", },

{ UMUL,	INAREG,
	SAREG,		TPOINT|TWORD,
	SNAME|SOREG,		TPOINT|TWORD,
	/*NSPECIAL|*/NAREG|NASL,	RESC1,
		"ld	A1,AR\n", },

{ UMUL,	INAREG,
	SAREG,		TPOINT | TWORD,
	SNAME|SOREG,	TCHAR|TUCHAR,
	NSPECIAL|NAREG|NASL,	RESC1,
		"ld	A1l,AR; umulb\n", },

/* Use a pair of moves. For FR0 we could use LR but it's likely to be
   emulated anyway */
 { UMUL,	INCREG,
 	SANY,	TPOINT | TWORD,
 	SOREG|SNAME,	TFLOAT,
 		NCREG|NCSL,	RESC1,		/* ?? NCSL */
		"TODO\n", },

/*
 * Logical/branching operators. No constant compares but fake them using
 * B
 */

{ OPLOG,	FORCC,
	SAREG,	TCHAR|TUCHAR,
	SZERO,	TANY,
		NSPECIAL, 	RESCC,
		"clr	bl\nsub	ALl,bl\n", },

{ OPLOG,	FORCC,
	SAREG,	TCHAR|TUCHAR,
	SCON,	TANY,
		NSPECIAL, 	RESCC,
		"ld	bl,CR\nsub	ALl,bl\n", },

{ OPLOG,	FORCC,
	SAREG,	TPOINT | TWORD,
	SZERO,	TANY,
		NSPECIAL, 	RESCC,
		"clr	b\nsub	AL,b\n", },

{ OPLOG,	FORCC,
	SAREG,	TPOINT | TWORD,
	SCON,	TANY,
		NSPECIAL, 	RESCC,
		"ld	b,CR\nsub	AL,b\n", },

{ OPLOG,	FORCC,
	SAREG,	TCHAR|TUCHAR,
	SAREG,	TANY,
		0, 	RESCC,
		"sub	ALl,ARl\n", },

{ OPLOG,	FORCC,
	SAREG,	TPOINT | TWORD,
	SAREG,	TANY,
		0, 	RESCC,
		"sub	AL,AR\n", },

/* No compare so we subtract and destroy */
{ OPLOG,	FORCC,
	SCREG|SNAME|SOREG,	TFLOAT,
	SCREG,		TFLOAT,
		0, 	RESCC,
		"sr	AL\n", },

/* We have to be careful about comparisons involving R0+n because we have
   no inc/dec without trashing flags. It's ok to compare registers and names
   but not always reg/oreg. We could try something complicated but this is
   good enough for now */
{ OPLOG,	FORCC,
	SBREG,		TLONG|TULONG,
	SBREG,		TLONG|TULONG,
		0,	RNULL,
		"ZF", },

/* And for constants we only have ci of a register */
{ OPLOG,	FORCC,
	SBREG,		TLONG|TULONG,
	SCON,		TLONG|TULONG,
		0,	RNULL,
		"ZF", },

{ AND,	INBREG|FORCC,
	SBREG,			TLONG|TULONG,
	SCON,	TLONG|TULONG,
		0,	RLEFT|RESCC,
		"Za", },

{ AND,	INBREG|FORCC,
	SBREG,		TLONG|TULONG,
	SBREG,		TLONG|TULONG,
		0,	RLEFT|RESCC,
		"and	ZL,ZR\nand	UL,UR\n", },

{ AND,	INAREG|FORCC|FOREFF,
	SAREG,		TWORD,
	SAREG,		TWORD,
		0,	RLEFT|RESCC,
		"and	AL,AR\n", },

{ AND,	INAREG|FORCC,
	SAREG,		TCHAR|TUCHAR,
	SAREG,		TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"and	ARl,ALl\n", },

{ OR,	INBREG|FORCC,
	SBREG,		TLONG|TULONG,
	SBREG,		TLONG|TULONG,
		0,	RLEFT|RESCC,
		"ori	ZL,ZR\nori	UL,UR\n", },

{ OR,	INBREG|FORCC,
	SBREG,			TLONG|TULONG,
	SCON,			TLONG|TULONG,
		0,	RLEFT|RESCC,
		"Zo", },

/* OR with int */
{ OR,	FOREFF|INAREG|FORCC,
	SAREG,		TWORD,
	SAREG,		TWORD,
		0,	RLEFT|RESCC,
		"ori	AL,AR\n", },

/* OR with char */
{ OR,	INAREG|FORCC,
	SAREG,		TCHAR|TUCHAR,
	SAREG,		TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"ori	ALl,ARl\n", },

/* XOR with int  */
{ ER,	INAREG|FORCC,
	SAREG,	TWORD|TCHAR|TUCHAR,
	SAREG,	TWORD|TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"ore	AL,AR\n", },

/* XOR with long  */
{ ER,	INBREG|FORCC,
	SBREG|SOREG|SNAME,	TLONG|TULONG,
	SBREG,	TLONG|TULONG,
		0,	RLEFT|RESCC,
		"ore	ZL,ZR\nore	UL,UR\n", },

/*
 * Jumps.
 */
{ GOTO, 	FOREFF,
	SCON|SNAME|SOREG,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"jmp	LL\n", },

{ GOTO, 	FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"jmp	(AL)\n", },

/*
 * Convert LTYPE to reg.
 */

/* XXX - avoid OREG index register to be overwritten */

/* We have to keep separate const forms because of ld v mov */
{ OPLTYPE,	INBREG,
	SANY,	TANY,
	SCON,	TLONG|TULONG,
		NBREG,	RESC1,
		"ZO", },

{ OPLTYPE,	INBREG,
	SANY,	TANY,
	SBREG,	TLONG|TULONG,
		NBREG,	RESC1,
		"xfr	Z1,ZL\nxfr	UL,U1; opltype UL,UL\n", },

{ OPLTYPE,	INBREG,
	SANY,	TANY,
	SNAME|SOREG,	TLONG|TULONG,
		NSPECIAL|NBREG,	RESC1,	/* RDEST ? TODO */
		"ld	Z1,ZL\nld	U1,UL; opltype UL,UL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SZERO,		TWORD|TPOINT|TCHAR|TUCHAR,
		NAREG,	RESC1,
		"clr	A1\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SCON,		TWORD|TPOINT,
		NSPECIAL|NAREG,	RESC1,
		"ld	A1,CL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SCON,		TCHAR|TUCHAR,
		NSPECIAL|NAREG,	RESC1,
		"ld	A1l, CL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SOREG|SNAME,	TWORD|TPOINT,
		NSPECIAL|NAREG,	RESC1,
		"ld	A1,AL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SAREG|SOREG|SNAME,	TWORD|TPOINT,
		NAREG|NASR,	RESC1,
		"xfr	A1,AL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SAREG,	TCHAR|TUCHAR,
		NAREG,		RESC1,
		"xfr	A1l,ALl\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SOREG|SNAME,	TCHAR|TUCHAR,
		NSPECIAL|NAREG,		RESC1,
		"ld	A1l,ALl\nclr	A1h", },

{ OPLTYPE,	INCREG,
	SANY,			TANY,
	SCREG|SOREG|SNAME,	TFLOAT,
		NCREG,		RESC1,
		"; opltype AL into A1\nZM", },

/*
 * Negate a word.
 */
{ UMINUS,	INAREG|FOREFF,
	SAREG,	TWORD|TPOINT|TCHAR|TUCHAR,
	SANY,	TANY,
		0,	RLEFT,
		"ivr	AL\ninr	AL\n", },
		

{ UMINUS,	INBREG|FOREFF,
	SBREG,			TLONG|TULONG,
	SBREG,			TANY,
		NSPECIAL,	RLEFT,
		"jsr	neg32\n", },

{ UMINUS,	INCREG|FOREFF,
	SCREG,	TFLOAT,
	SANY,	TANY,
		NSPECIAL,	RLEFT,
		"negr ; fp negate AL\n", },


{ COMPL,	INBREG,
	SBREG,	TLONG|TULONG,
	SANY,	TANY,
		0,	RLEFT,
		"ivr	ZL\nivr	UL\n", },

{ COMPL,	INAREG,
	SAREG,	TWORD,
	SANY,	TANY,
		0,	RLEFT,
		"ivr	AL\n", },

/*
 * Arguments to functions.
 *
 * Again we will need helpers to juggle the awkward cases
 */

{ FUNARG,	FOREFF,
	SBREG,	TLONG|TULONG,
	SANY,	TLONG|TULONG,
		0,	RNULL,
		"ZSst	ZL,(-s)\nst	UL,(-s)\n", },

{ FUNARG,	FOREFF,
	SAREG,	TWORD|TPOINT,
	SANY,	TWORD|TPOINT,
		0,	RNULL,
		"ZSst	AL,(-s)\n", },

{ FUNARG,	FOREFF,
	SAREG,	TUCHAR|TCHAR,
	SANY,	TUCHAR|TCHAR,
		0,	RNULL,
		"ZSst	ALl,(-s)\n", },

/* FIXME: need to double check fr case works.. */
{ FUNARG,	FOREFF,
	SCREG,	TFLOAT,
	SANY,		TANY,
		0,	RNULL,
		"ZSdcrt	r13\nxfr ZL,*r13\ndcrt	r13\nxfr UL,*r13\n", },

{ STARG,	FOREFF,
	SAREG,	TPTRTO|TANY,
	SANY,	TSTRUCT,
		NSPECIAL,	0,
		"ZJ", },


# define DF(x) FORREW,SANY,TANY,SANY,TANY,REWRITE,x,""

{ UMUL, DF( UMUL ), },

{ ASSIGN, DF(ASSIGN), },

{ STASG, DF(STASG), },

{ FLD, DF(FLD), },

{ OPLEAF, DF(NAME), },

/* { INIT, DF(INIT), }, */

{ OPUNARY, DF(UMINUS), },

{ OPANY, DF(BITYPE), },

{ FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	"help; I'm in trouble\n" },
};

int tablesize = sizeof(table)/sizeof(table[0]);
