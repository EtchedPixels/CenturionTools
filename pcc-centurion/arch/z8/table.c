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
	SAREG,	TWORD|TPOINT|TCHAR|TUCHAR,
	SAREG,	TWORD|TPOINT|TCHAR|TUCHAR,
		0,	RLEFT,
		"", },

/* long -> ptr */
{ PCONV,	INAREG,
	SBREG,		TLONG|TULONG,
	SAREG,			TPOINT,
		NAREG|NASL,	RESC1,
		"ld.x	A1,AL", },

/* Unsigned char to char : no work needed */
{ SCONV,	INAREG,
	SAREG,	TUCHAR,
	SAREG,	TCHAR,
		NAREG|NASL,	RLEFT,
		"", },

/* Char to unsigned char : no work needed */
{ SCONV,	INAREG,
	SAREG,	TCHAR,
	SAREG,	TUCHAR,
		NAREG|NASL,	RLEFT,
		"", },

/* Char to int or unsigned */
{ SCONV,	INAREG,
	SAREG,	TCHAR,
	SAREG,	TWORD,
		NAREG|NASL,	RESC1,
		"", },

/* Char or unsigned char to int or uint: constant */
{ SCONV,	INAREG,
	SCON,			TCHAR|TUCHAR,
	SAREG,			TWORD,
		NAREG|NASL,	RESC1,
		"ld.w	A1,CL\n", },

/* Unsigned char to int or uint: register */
{ SCONV,	INAREG,
	SAREG,	TUCHAR,
	SAREG,	TWORD,
		NAREG|NASL,	RESC1,
		"", },

/* char to ulong: register - no sign extend to expands somewhat */
{ SCONV,	INBREG,
	SAREG,	TCHAR,
	SANY,	TULONG|TLONG,
		NBREG,	RESC1,
		"ld.sx	A1,AL\n", },

/* Unsigned char to unsigned long: register */
{ SCONV,	INBREG,
	SAREG,	TUCHAR,
	SANY,	TLONG|TULONG,
		NBREG|NBSL,	RESC1,
		"ld.x	A1,AL\n", },

/* Unsigned char to float/double */
/* Constant is forced into R1, masked and converted */
{ SCONV,	INCREG,
	SAREG,	TUCHAR,
	SANY,	TFLOAT,
		NSPECIAL|NCREG,	RESC1,
		"clr	r0\nsrl	AL,8\ncir	AL\n", },

/* Signed char to float/double */
/* Constant is forced into R1, masked and converted */
{ SCONV,	INCREG,
	SAREG,	TCHAR,
	SANY,	TFLOAT,
		NSPECIAL|NCREG,	RESC1,
		"bl	@s8fp\n", },

/* Int or unsigned to char or unsigned: register */
{ SCONV,	INAREG,
	SAREG,	TWORD|TPOINT,
	SANY,	TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"swpb	AL\n", },

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
		NAREG|NASL,	RESC1,
		"swpb	AL\n", },

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
		"ld.sx	A1,AL", },

/* int to float: nice and easy except for the register swap */
{ SCONV,	INCREG,
	SAREG,	TINT,
	SANY,	TFLOAT,
		NSPECIAL|NCREG,	RESC1,
		"bl	@cir_AL\n", },

/* int to float: object in memory: actually easier */
{ SCONV,	INCREG,
	SNAME|SOREG,	TINT,
	SANY,	TFLOAT,
		NSPECIAL|NCREG,	RESC1,
		"cir	AL\n", },

{ SCONV,	INBREG,
	SAREG,	TUNSIGNED,
	SANY,	TLONG|TULONG,
		NBREG,	RESC1,
		"ld.x	AL,A1\n", },

/* unsigned int to float : we don't have a 16bit op for it. use the 32bit one */
{ SCONV,	INCREG,
	SAREG,		TUNSIGNED,
	SANY,		TFLOAT,
		NSPECIAL,	RLEFT,
		"clr	r0\ncer AL\n", },

/* long or unsigned long to char or uchar: lower byte swapped*/
{ SCONV,	INAREG,
	SBREG,		TLONG|TULONG,
	SAREG,		TCHAR|TUCHAR,
		NAREG,	RESC1,
		"ld.x	A1,AL\n", };

/* long or unsigned long to int or uint: no work required */
/* Alas the compiler isn't quite smart enough for this to happen in situ */
{ SCONV,	INAREG,
	SBREG,		TLONG|TULONG,
	SAREG,		TWORD|TPOINT,
		NAREG,	RESC1,	/*XX */
		"ld.x	A1,AL\n", },

/* (u)long -> (u)long, nothing */
{ SCONV,	INBREG,
	SBREG,	TLONG|TULONG,
	SANY,	TLONG|TULONG,
		NBREG|NBSL,	RESC1,
		"", },

/* long -> float/double */
{ SCONV,	INCREG,
	SNAME|SOREG,	TLONG,
	SANY,		TFLOAT,
		NSPECIAL,	RLEFT,
		"cer AL\n", },

/* ulong -> float/double */
{ SCONV,	INCREG,
	SBREG,		TULONG,
	SANY,		TFLOAT,
		NSPECIAL|NCSL,	RLEFT,
		"bl	@u32fp\n", },


/* float to long or ulong : helper converts and flips in situ */
/* CRE needs the words flipping */
{ SCONV,	INBREG,
	SCREG,	TFLOAT,
	SANY,	TLONG|TULONG,
		NSPECIAL,	RLEFT,
		"bl	@cre_flip\n" },

/* float to int : special rule plonks it in the right place */
{ SCONV,	INAREG,
	SCREG,	TFLOAT,
	SANY,	TINT,
		NSPECIAL,	RLEFT,
		"cri\n", },

/* float/double -> float/double */
{ SCONV,	INCREG,
	SCREG,	TFLOAT,
	SANY,	TANY,
		0,	RLEFT,
		"", },

/*
 * Subroutine calls.
 */
{ CALL,		INAREG,
	SCON,	TANY,
	SAREG,	TWORD|TPOINT|TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"call	AL\nZC", },

{ UCALL,	INAREG,
	SNAME|SCON|SOREG,	TANY,
	SAREG,	TWORD|TPOINT|TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"call	AL\nZC", },

{ CALL,		INAREG,
	SAREG|SOREG,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,	/* should be 0 */
		"call	AL\nZC", },

{ UCALL,	INAREG,
	SAREG,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,	/* should be 0 */
		"call	AL\nZC", },

{ CALL,		INBREG,
	SNAME|SCON|SOREG,	TANY,
	SBREG,	TLONG|TULONG,
		NBREG|NBSL,	RESC1,
		"call	AL\nZC", },

{ UCALL,	INBREG,
	SNAME|SCON|SOREG,	TANY,
	SBREG,	TLONG|TULONG,
		NBREG|NBSL,	RESC1,
		"call	AL\nZC", },

{ CALL,		INBREG,
	SAREG,	TANY,
	SBREG,	TLONG|TULONG,
		NBREG|NBSL,	RESC1,
		"call	AL\nZC", },

{ UCALL,	INBREG,
	SAREG,	TANY,
	SBREG,	TLONG|TULONG,
		NBREG|NBSL,	RESC1,
		"call	AL\nZC", },

{ CALL,		INCREG,
	SNAME|SCON|SOREG,	TANY,
	SCREG,	TFLOAT,
		NCREG|NCSL,	RESC1,
		"call	AL\nZC", },

{ UCALL,	INCREG,
	SNAME|SCON|SOREG,	TANY,
	SCREG,	TFLOAT,
		NCREG|NCSL,	RESC1,
		"fcall	AL\nZC", },

{ CALL,		INCREG,
	SAREG,	TANY,
	SCREG,	TFLOAT,
		NCREG|NCSL,	RESC1,
		"call	AL\nZC", },

{ UCALL,	INCREG,
	SAREG,	TANY,
	SCREG,	TFLOAT,
		NCREG|NCSL,	RESC1,
		"call	AL\nZC", },

{ CALL,		FOREFF,
	SNAME|SCON|SOREG,	TANY,
	SANY,	TANY,
		0,	0,
		"call	AL\nZC", },

{ UCALL,	FOREFF,
	SNAME|SCON|SOREG,	TANY,
	SANY,	TANY,
		0,	0,
		"call	AL\nZC", },

{ CALL,		FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
		0,	0,
		"call	AL\nZC", },

{ UCALL,	FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
		0,	0,
		"call	AL\nZC", },

{ STCALL,	INAREG,
	SCON|SOREG|SNAME,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,
		"call	AL\nZC", },

{ USTCALL,	INAREG,
	SCON|SOREG|SNAME,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,
		"call	AL\nZC", },

{ STCALL,	FOREFF,
	SCON|SOREG|SNAME,	TANY,
	SANY,	TANY,
		0,	0,
		"call	AL\nZC", },

{ USTCALL,	FOREFF,
	SCON|SOREG|SNAME,	TANY,
	SANY,	TANY,
		0,	0,
		"call	AL\nZC", },

{ STCALL,	INAREG,
	SAREG,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,	/* should be 0 */
		"call	AL\nZC", },

{ USTCALL,	INAREG,
	SAREG,	TANY,
	SANY,	TANY,
		NAREG|NASL,	RESC1,	/* should be 0 */
		"call	AL\nZC", },

/*
 * The next rules handle all binop-style operators.
 */

/* Add one to anything left but use only for side effects */
{ PLUS,		FOREFF|INAREG|FORCC,
	SAREG,		TWORD|TPOINT,
	SONE,			TANY,
		0,	RLEFT|RESCC,
		"inc.w	AL\n", },

/* add one for char to reg, special handling */
{ PLUS,		FOREFF|INAREG|FORCC,
	SAREG,	TCHAR|TUCHAR,
	SONE,		TANY,
		0,	RLEFT|RESCC,
		"inc.b	AL\n", },

{ PLUS,		INBREG|FOREFF,
	SBREG,			TLONG|TULONG,
	SONE,			TLONG|TULONG,
		0,	RLEFT,
		"inc.l	AL\n", },

{ PLUS,		INBREG|FOREFF,
	SBREG,			TLONG|TULONG,
	SBREG,			TLONG|TULONG,
		0,	RLEFT,
		"add.l	AL,AR\n", },

{ PLUS,		INBREG|FOREFF,
	SBREG,			TLONG|TULONG,
	SCON,			TLONG|TULONG,
		0,	RLEFT,
		"addi.l	AL,AR\n", },

/* Integer to pointer addition */
{ PLUS,		INAREG,
	SAREG,	TPOINT|TWORD,
	SAREG,	TPOINT|TWORD,
		0,	RLEFT,
		"add.w	AL,AR\n", },

/* Add to reg left and reclaim reg */
{ PLUS,		INAREG|FOREFF|FORCC,
	SAREG,		TWORD|TPOINT,
	SCON,		TWORD|TPOINT,
		0,	RLEFT|RESCC,
		"add.w	AL,CR\n", },

{ PLUS,		INAREG|FOREFF|FORCC,
	SAREG,			TCHAR|TUCHAR,
	SAREG,		TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"add.b	AL,AR\n", },

{ PLUS,		INAREG|FOREFF|FORCC,
	SAREG,			TCHAR|TUCHAR,
	SCON,			TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"add.b	AL,AR\n", },

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
		"call	@dec32\n", },

{ MINUS,		INBREG|FOREFF,
	SBREG,		TLONG|TULONG,
	SCON,		TLONG|TULONG,
		NSPECIAL,	RDEST,
		/* Words reversed for speed */
		"call	@sub32i\n.word	CR\n.word	ZQ\n", },

{ MINUS,		INBREG|FOREFF,
	SBREG,		TLONG|TULONG,
	SBREG,		TLONG|TULONG,
		NSPECIAL,	RDEST,
		"call	@sub32\n", },

/* Sub one from anything left */
{ MINUS,	FOREFF|INAREG|FORCC,
	SAREG,		TWORD|TPOINT,
	SONE,		TANY,
		0,	RLEFT|RESCC,
		"dec.w	AL\n", },

{ MINUS,		INAREG|FOREFF,
	SAREG,			TWORD|TPOINT,
	SAREG|SCON,	TWORD|TPOINT,
		0,	RLEFT,
		"sub.w	AL,AR\n", },

/* Sub one left but use only for side effects */
{ MINUS,	FOREFF,
	SAREG,		TCHAR|TUCHAR,
	SONE,			TANY,
		0,	RLEFT,
		"dec.b	AL\n", },

/* Sub from anything left but use only for side effects. Need to review
   because of the SUB 0 funny */
{ MINUS,		FOREFF,
	SAREG,		TCHAR|TUCHAR,
	SAREG,		TCHAR|TUCHAR,
		0,	RLEFT,
		"sub.b	AL,AR\n", },

{ MINUS,		FOREFF,
	SAREG,		TCHAR|TUCHAR,
	SCON,		TCHAR|TUCHAR,
		0,	RLEFT,
		"sub.b	AL,AR\n", },

/* floating point */
{ MINUS,	INCREG|FOREFF|FORCC,
	SCREG,			TFLOAT,
	SCREG|SNAME|SOREG,	TFLOAT,
		NSPECIAL,	RDEST|RESCC,
		"sr	AR; subtract from AL\n", },

/*
 * The next rules handle all shift operators.
  */
 
{ LS,	INBREG|FOREFF,
	SBREG,	TLONG|TULONG,
	SAREG,	TINT|TUNSIGNED|TCHAR|TUCHAR,
		0,	RLEFT,
		"sla.l	AL,ARn", },

{ LS,	INAREG|FOREFF,
	SAREG,		TWORD,
	SAREG|SCON,	TWORD|TCHAR|TUCHAR,
	0,	RLEFT,
		"sla.w	AL,AR\n", },
		

{ LS,	INAREG|FOREFF,
	SAREG,		TCHAR|TUCHAR,
	SAREG|SCON,	TWORD|TCHAR|TUCHAR,
	0,	RLEFT,	
		"sla.b	AL,AR\n", },
		

/* Same again - right unsigned 8 or 16bit shifts */

{ RS,	INBREG|FOREFF,
	SBREG,	TLONG,
	SAREG,	TINT|TUNSIGNED|TCHAR|TUCHAR,
		0,	RLEFT,
		"sra.l	AL,ARn", },

{ RS,	INAREG|FOREFF,
	SAREG,		TINT,
	SAREG|SCON,	TWORD|TCHAR|TUCHAR,
	0,	RLEFT,
		"sra.w	AL,AR\n", },
		

{ RS,	INAREG|FOREFF,
	SAREG,		TCHAR,
	SAREG|SCON,	TWORD|TCHAR|TUCHAR,
	0,	RLEFT,	
		"sra.b	AL,AR\n", },

{ RS,	INBREG|FOREFF,
	SBREG,	TULONG,
	SAREG,	TINT|TUNSIGNED|TCHAR|TUCHAR,
		0,	RLEFT,
		"srl.l	AL,ARn", },

{ RS,	INAREG|FOREFF,
	SAREG,		TUNSIGNED,
	SAREG|SCON,	TWORD|TCHAR|TUCHAR,
	0,	RLEFT,
		"srl.w	AL,AR\n", },
		

{ RS,	INAREG|FOREFF,
	SAREG,		TUCHAR,
	SAREG|SCON,	TWORD|TCHAR|TUCHAR,
	0,	RLEFT,	
		"srl.b	AL,AR\n", },


/*
 * The next rules takes care of assignments. "=".
 */

/* First optimizations, in lack of weight it uses first found */
/* Start with class A registers */


/* Must have multiple rules for long otherwise regs may be trashed */
{ ASSIGN,	FOREFF|INBREG,
	SBREG,			TLONG|TULONG,
	SCON,			TLONG|TULONG,
		0,	RDEST,
		"ld.l	AL,AR", },

{ ASSIGN,	FOREFF|INBREG,
	SBREG,			TLONG|TULONG,
	SBREG,			TLONG|TULONG,
		0,	RDEST,
		"ld.l	AL,AR", },

{ ASSIGN,	FOREFF|INBREG,
	SBREG,			TLONG|TULONG,
	SNAME|SOREG,		TLONG|TULONG,
		0,	RDEST,
		"fetch.l	AL,AR", },

{ ASSIGN,	FOREFF|INBREG,
	SNAME|SOREG,		TLONG|TULONG,
	SBREG,			TLONG|TULONG,
		0,	RDEST,
		"store.l	AL,AR", },

{ ASSIGN,	INBREG|FOREFF,
	SBREG,	TLONG|TULONG,
	SBREG,	TLONG|TULONG,
		0,	RDEST,
		"ld.l	AL,AR", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SAREG,			TWORD|TPOINT,
	SCON,			TWORD|TPOINT,
		0,	RDEST|RESCC,
		"ld.w	AL,AR\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SAREG,			TWORD|TPOINT,
	SAREG,			TWORD|TPOINT,
		0,	RDEST|RESCC,
		"ld.w	AL,AR\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SAREG,			TWORD|TPOINT,
	SAREG|SNAME|SOREG,	TWORD|TPOINT,
		0,	RDEST|RESCC,
		"fetch.w	AL,AR\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SNAME|SOREG,	TWORD|TPOINT,
	SAREG,	TWORD|TPOINT,
		0,	RDEST|RESCC,
		"store.w	AL,AR\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SAREG,	,	TCHAR|TUCHAR,
	SNAME|SOREG,		TCHAR|TUCHAR,
		0,	RDEST|RESCC,
		"fetch.b	AL,AR\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SNAME|SOREG,		TCHAR|TUCHAR,
	SAREG,	,	TCHAR|TUCHAR,
		0,	RDEST|RESCC,
		"store.b	AL,AR\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SAREG,		TCHAR|TUCHAR,
	SCON,		TCHAR|TUCHAR,
		0,	RDEST|RESCC,
		"ld.b	AL, AR\n", },

{ ASSIGN,	FOREFF|INAREG|FORCC,
	SAREG,,	TCHAR|TUCHAR,
	SAREG,	TCHAR|TUCHAR,
		0,	RDEST|RESCC,
		"ld.b	AL,AR\n", },

/* FIXME: SNAME/OREG  SNAME/OREG with rdest ?? looks wrong on 9995 - not on
   here at all */

/* Floating point */

{ ASSIGN,	FOREFF|INCREG,
	SCREG|SNAME|SOREG,	TFLOAT,
	SCREG|SNAME|SOREG,	TFLOAT,
	0,	RDEST,
		"; mov AR,AL\nZH", },

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
		0,	RLEFT,
		"mul.w	AL,AR\n", },

{ MUL,	INBREG,
	SBREG,			TLONG|TULONG,
	SBREG,			TLONG|TULONG,
		0,		RLEFT,
		"mul.l	AL,AR\n", },

{ MUL,	INBREG,
	SBREG,			TLONG|TULONG,
	SBREG,			TLONG|TULONG,
		0,		RLEFT,
		"mul.l	AL,AR\n", },

{ MUL,	INCREG,
	SCREG,			TFLOAT,
	SCREG,			TFLOAT,
		NSPECIAL,	RDEST,
		"mr	AR; AL *= AR\n", },

{ DIV,	INAREG,
	SAREG,			TINT|TPOINT,
	SAREG,			TINT|TPOINT,
		0,	RLEFT,
		"div.sw	AL,AR\n", },

/* div can use things other than r0/r1 but we don't */
{ DIV,	INAREG,
	SAREG,			TUNSIGNED,
	SAREG,			TUNSIGNED,
		0,	RLEFT,
		"div.w	AL,AR\n", },

/* FIXME: tms9995 nspecial,rleft should be rdest ??? */
{ DIV,	INBREG,
	SBREG,			TLONG,
	SBREG,			TLONG,
		0,		RLEFT,
		"div.sl	AL,AR\n", },

{ DIV,	INBREG,
	SBREG,			TLONG|TULONG,
	SBREG,			TLONG|TULONG,
		0,		RLEFT,
		"div.l	AL,AR", },

{ DIV,	INCREG,
	SCREG,			TFLOAT,
	SCREG|SNAME|SOREG,		TFLOAT,
		NSPECIAL,	RDEST,
		"dr	AR; AL /= AR\n", },

/* signed divide r0/r1 by operand into r0/r1 (r1 = remainder) */

{ MOD,	INAREG,
	SAREG,			TINT|TPOINT,
	SAREG,			TINT|TPOINT,
		0,	RLEFT,
		"mod.sw	AL,AR\n", },

/* Form looks wrong (nspecial rleft) on tms9995 FIXME */
{ MOD,	INAREG,
	SAREG,			TUNSIGNED,
	SAREGS,			TUNSIGNED,
		0,	RLEFT,
		"mod.w	AL,AR\n", },

{ MOD,	INBREG,
	SBREG,			TLONG,
	SBREG,			TLONG,
		0,	RLEFT,
		"mod.sl	AL,AR\n", },

{ MOD,	INBREG,
	SBREG,			TLONG|TULONG,
	SBREG,			TLONG|TULONG,
		0,		RLEFT,
		"mod.l	AL,AR\n", },

/*
 * Indirection operators.
 */

{ UMUL,	INBREG,
	SANY,	TPOINT|TWORD,
	SOREG|SNAME,	TLONG|TULONG,
	NBREG,	RESC1, /* |NBSL - may overwrite index reg */
		"fetch.l	A1,AL\n", },

{ UMUL,	INAREG,
	SANY,	TPOINT|TWORD,
	SOREG|SNAME,	TPOINT|TWORD,
		NAREG|NASL,	RESC1,
		"fetch.w	A1,AL\n", },

{ UMUL,	INAREG,
	SANY,	TPOINT | TWORD,
	SOREG|SNAME,	TCHAR|TUCHAR,
		NAREG|NASL,	RESC1,
		"fetch.b	A1,AL\n", },

/* Use a pair of moves. For FR0 we could use LR but it's likely to be
   emulated anyway */
 { UMUL,	INCREG,
 	SANY,	TPOINT | TWORD,
 	SOREG|SNAME,	TFLOAT,
 		NCREG|NCSL,	RESC1,		/* ?? NCSL */
		"mov	ZL,Z1\nmov	UL,U1; umul AL into A1\n", },

/*
 * Logical/branching operators
 */

{ OPLOG,	FORCC,
	SAREG|SOREG|SNAME|SCON,	TCHAR|TUCHAR,
	SCON,	TANY,
		0, 	RESCC,
		"cp.b	AL,AR\n", },

{ OPLOG,	FORCC,
	SAREG,		TWORD|TPOINT,
	SAREG,		TWORD|TPOINT,
		0, 	RESCC,
		"cp.w	AL,AR\n", },

{ OPLOG,	FORCC,
	SAREG,		TWORD|TPOINT,
	SCON,		TWORD|TPOINT,
		0, 	RESCC,
		"cp.w	AL,AR\n", },

/* No compare so we subtract and destroy */
{ OPLOG,	FORCC,
	SCREG|SNAME|SOREG,	TFLOAT,
	SCREG,		TFLOAT,
		NSPECIAL, 	RESCC,
		"sr	AL\n", },

{ OPLOG,	FORCC,
	SAREG|SCON,	TCHAR|TUCHAR,
	SAREG|SCON,	TCHAR|TUCHAR,
		0, 	RESCC,
		"cb	AL,AR\n", },

{ OPLOG,	FORCC,
	SBREG,		TLONG|TULONG,
	SBREG,		TLONG|TULONG,
		0,	RNULL,
		"cp.l	AL,AR", },

{ OPLOG,	FORCC,
	SBREG,		TLONG|TULONG,
	SCON,		TLONG|TULONG,
		0,	RNULL,
		"ZF", },

{ AND,	INBREG|FORCC,
	SBREG,			TLONG|TULONG,
	SCON,	TLONG|TULONG,
		0,	RLEFT|RESCC,
		"and.l	AL,AR\n", },

/* This rule works because the fixops() modified the tree to complement the
   right hand side for us */
{ AND,	INBREG|FORCC,
	SBREG,			TLONG|TULONG,
	SBREG,		TLONG|TULONG,
		0,	RLEFT|RESCC,
		"and.l	AL,AR\n", },

/* set status bits */
{ AND,	FORCC,
	SAREG,	TWORD|TPOINT,
	SARG,	TWORD|TPOINT,
		0,	RESCC,
		"and.l	AL,AR\n", },

/* set status bits */
{ AND,	FORCC,
	SAREG,		TWORD|TPOINT,
	SCON,		TWORD|TPOINT,
		0,	RESCC,
		"and.l	AL,AR\n", },

/* AND with int */
/* This rule works because the fixops() modified the tree to complement the
   right hand side for us */
{ AND,	INAREG|FORCC|FOREFF,
	SAREG,		TWORD,
	SAREG,		TWORD,
		0,	RLEFT|RESCC,
		"and.w	AL,AR\n", },

{ AND,	INAREG|FORCC|FOREFF,
	SAREG,		TWORD,
	SCON,		TWORD,
		0,	RLEFT|RESCC,
		"and.w	AL,AR\n", },

/* AND with char */
{ AND,	INAREG|FORCC,
	SAREG,		TCHAR|TUCHAR,
	SAREG,			TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"szc	AR,AL\n", },

/*FIXME: check if msising on TMS9995 */
{ AND,	FORCC,
	SAREG,		TCHAR|TUCHAR,
	SCON,		TCHAR|TUCHAR,
		0,	RESCC,
		"and.b	AL,AR\n", },

{ OR,	INBREG|FORCC,
	SBREG,			TLONG|TULONG,
	SBREG,		TLONG|TULONG,
		0,	RLEFT|RESCC,
		"or.l	AL,AR\n", },

{ OR,	INBREG|FORCC,
	SBREG,			TLONG|TULONG,
	SCON,			TLONG|TULONG,
		0,	RLEFT|RESCC,
		"or.l	AL,AR\n", },

/* OR with int */
{ OR,	FOREFF|INAREG|FORCC,
	ARONS,		TWORD,
	ARONS|SCON,	TWORD,
		0,	RLEFT|RESCC,
		"soc	AR,AL\n", },

{ OR,	FOREFF|INAREG|FORCC,
	ARONS,		TWORD,
	SCON,		TWORD,
		0,	RLEFT|RESCC,
		"ori	AL,CR\n", },

/* OR with char */
{ OR,	INAREG|FORCC,
	SAREG|SOREG|SNAME,	TCHAR|TUCHAR,
	ARONS,			TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"socb	AR,AL\n", },

{ OR,	INAREG|FORCC,
	SAREG|SOREG|SNAME,	TCHAR|TUCHAR,
	SCON,			TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"socb	@_litb_ZT,AL\n", },

/* No XORI */

/* XOR with int  */
{ ER,	INAREG|FORCC,
	ARONS,	TWORD|TCHAR|TUCHAR,
	SAREG,	TWORD|TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"xor	AR,AL\n", },

/* XOR with long (extended insn)  */
{ ER,	INBREG|FORCC,
	SBREG|SOREG|SNAME,	TLONG|TULONG,
	SBREG,	TLONG|TULONG,
		0,	RLEFT|RESCC,
		"xor	ZR,ZL\nxor	UR,UL\n", },

/*
 * Jumps.
 */
{ GOTO, 	FOREFF,
	SCON|SNAME|SOREG,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"ljp	LL\n", },

{ GOTO, 	FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"ljp	AL\n", },

/*
 * Convert LTYPE to reg.
 */

/* XXX - avoid OREG index register to be overwritten */

/* We have to keep separate const forms because of li v mov */
{ OPLTYPE,	INBREG,
	SANY,	TANY,
	SCON|SBREG,	TLONG|TULONG,
		NBREG,	RESC1,
		"ld.l	A1,AL", },

{ OPLTYPE,	INBREG,
	SANY,	TANY,
	SNAME|SOREG,	TLONG|TULONG,
		NBREG,	RESC1,
		"fetch.l A1,AL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SCON|SAREG,	TWORD|TPOINT,
		NAREG,	RESC1,
		"ld.w	A1,AL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SOREG|SNAME,	TWORD|TPOINT,
		NAREG|NASR,	RESC1,
		"fetch.w	A1,AL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SCON|SAREG,	TCHAR|TUCHAR,
		NAREG,	RESC1,
		"ld.b	A1,AL\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SOREG|SNAME,	TCHAR|TUCHAR,
		NAREG|NASR,	RESC1,
		"fetch.b	A1,AL\n", },

{ OPLTYPE,	INCREG,
	SANY,			TANY,
	SCREG|SOREG|SNAME,	TFLOAT,
		NCREG,		RESC1,
		"; opltype AL into A1\nZM", },

/*
 * Negate a word.
 */
{ UMINUS,	INAREG|FOREFF,
	SAREG,	TCHAR|TUCHAR,
	SANY,	TANY,
		0,	RLEFT,
		"neg.b	AL\n", },

{ UMINUS,	INAREG|FOREFF,
	SAREG,	TWORD|TPOINT,
	SANY,	TANY,
		0,	RLEFT,
		"neg.w	AL\n", },

{ UMINUS,	INBREG|FOREFF,
	SBREG,			TLONG|TULONG,
	SBREG,			TANY,
		0,	RLEFT,
		"neg.l AL\n", },

{ UMINUS,	INCREG|FOREFF,
	SCREG,	TFLOAT,
	SANY,	TANY,
		NSPECIAL,	RLEFT,
		"negr ; fp negate AL\n", },


/* Check do we need a byte form here and on TMS9995 */
{ COMPL,	INBREG,
	SBREG,	TLONG|TULONG,
	SANY,	TANY,
		0,	RLEFT,
		"cpl.l	AL\n", },

{ COMPL,	INAREG,
	SAREG,	TWORD,
	SANY,	TANY,
		0,	RLEFT,
		"cpl.w	AL\n", },

/*
 * Arguments to functions.
 */
 
{ FUNARG,	FOREFF,
	SBREG,	TLONG|TULONG,
	SANY,	TLONG|TULONG,
		0,	RNULL,
		"ZSpush.l	AL\n", },

{ FUNARG,	FOREFF,
	SAREG,	TWORD|TPOINT,
	SANY,	TWORD|TPOINT,
		0,	RNULL,
		"ZSpush.w	AL\n", },

{ FUNARG,	FOREFF,
	SAREG,	TUCHAR|TCHAR,
	SANY,	TUCHAR|TCHAR,
		0,	RNULL,
		"ZSpush.b	AL\n", },

/* FIXME: need to check if %fr0 case works.. */
{ FUNARG,	FOREFF,
	SCREG,	TFLOAT,
	SANY,		TANY,
		0,	RNULL,
		"ZSdect	r13\nmov ZL,*r13\ndect	r13\nmov UL,*r13\n", },

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
