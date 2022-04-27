/*	$Id: order.c,v 1.13 2019/04/25 17:40:33 ragge Exp $	*/
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

#include <string.h>

/*
 *	Feature sets
 */

unsigned m_has_divs = 1;		/* TMS9995 has divs, 9900 does not */

/*
 *	Can we addr this ?
 */
static int canaddr(NODE *p)
{
	int o = p->n_op;

	if (o==NAME || o==REG || o==ICON || o==OREG ||
	    (o==UMUL && shumul(p->n_left, SOREG|STARNM)))
		return(1);
	return(0);
}

/* is it legal to make an OREG or NAME entry which has an
 * offset of off, (from a register of r), if the
 * resulting thing had type t
 *
 * In the Warrex case we've got 8bit signed offsets. For anything bigger
 * we say no and the compiler core will turn it into an add and dereference
 */
int
notoff(TWORD t, int r, CONSZ off, char *cp)
{
	if (x2debug)
		printf("notoff: t %x r %d off %lld\n", t, r, off);
	/* Signed 8bit index and allow for double words */
        if (off < -128 || off + szty(t) > 127) {
        	printf("badoff\n");
		return 1;
	}
	return(0);  /* YES */
}

/*
 * Turn a UMUL-referenced node into OREG.
 * Be careful about register classes, this is a place where classes change.
 */
void
offstar(NODE *p, int shape)
{
	NODE *r;
	if (x2debug)
		printf("offstar(%p)\n", p);
	r = p->n_right;
	if ((p->n_op == PLUS || p->n_op == MINUS) &&
	    r->n_op == ICON && (getlval(r) >= -128 && getlval(r) <= 127)) {
		if (!isreg(p->n_left))
			(void)geninsn(p->n_left, INAREG);
		return;
	}
	(void)geninsn(p, INAREG);
}

/*
 * Do the actual conversion of offstar-found OREGs into real OREGs.
 */
void
myormake(NODE *p)
{
	NODE *q = p->n_left;
	if (x2debug) {
		printf("myormake(%p)\n", p);
		fwalk(p, e2print, 0);
	}
#if 1
	/* Remove any UMULs that are left over */
	if (p->n_op == UMUL && q->n_op == REG && getlval(q) == 0) {
		printf("; killed UMUL for %d\n", q->n_rval);
		p->n_op = OREG;
		p->n_rval = q->n_rval;
		p->n_name = q->n_name;
		p->n_left = NULL;	/* ??? */
		setlval(p, 0);
		tfree(q);
	}
	if (q->n_op != OREG)
		return;
	if (TBLIDX(q->n_su))
		return; /* got sub-conversion, cannot convert */
	if (R2TEST(regno(q)))
		return; /* cannot convert anymore */
	if (p->n_type >= LONG && p->n_type <= ULONGLONG)
		return;
	p->n_op = OREG;
	setlval(p, getlval(q));
	p->n_rval = R2PACK(q->n_rval, 0, 0);
	if (1 || x2debug) {
		printf("myormake end(%p)\n", p);
		fwalk(p, e2print, 0);
	}
	nfree(q);
#endif	
}

/*
 * Shape matches for UMUL.  Cooperates with offstar().
 */
int
shumul(NODE *p, int shape)
{
	if (x2debug)
		printf("shumul(%p) %x\n", p, p->n_op);
	if (shape & SOREG) {
		return SROREG;	/* Calls offstar */
	}
#if 0
	if (p->n_op == NAME && (shape & STARNM))
		return SRDIR;
	if (shape & STARREG)
		return SROREG;
#endif		
	return SRNOPE;
}

/*
 * Rewrite operations on binary operators (like +, -, etc...).
 * Called as a result of table lookup.
 */
int
setbin(NODE *p)
{

	if (x2debug)
		printf("setbin(%p)\n", p);
	return 0;

}

/* setup for assignment operator */
int
setasg(NODE *p, int cookie)
{
	if (x2debug)
		printf("setasg(%p)\n", p);
	return(0);
}

/* setup for unary operator */
int
setuni(NODE *p, int cookie)
{
	return(0);
}

/*
 * Special handling of some instruction register allocation.
 */


/*
 *	We can only memory load/store into A B or X
 */
static struct rspecial load8[] = {
	{NOLEFT,  R_Y}, {NOLEFT, R_X}, { 0 }
};

static struct rspecial load16[] = {
	{NOLEFT,  R_Y}, { 0 }
};

static struct rspecial load32[] = {
	{NOLEFT,  R_YA}, {NOLEFT, R_YX }, { 0 }
};

static struct rspecial store8[] = {
	{NORIGHT,  R_Y}, {NORIGHT, R_X}, { 0 }
};

static struct rspecial store16[] = {
	{NORIGHT,  R_Y}, { 0 }
};

static struct rspecial store32[] = {
	{NORIGHT,  R_YA}, {NORIGHT, R_YX }, { 0 }
};

/* Not ideal */
static struct rspecial opl8[] = {
	{ NRES, R_A} , { 0 }
};

static struct rspecial opl16[] = {
	{NRES,  R_A}, { 0 }
};

static struct rspecial opl32[] = {
	{NRES,  R_BA}, { 0 }
};

/*
 *	32bith math helpers
 */

static struct rspecial helper16[] = {
	{ NLEFT, R_A }, { NRIGHT, R_B }, { 0 }
};

static struct rspecial helper32[] = {
	{ NLEFT, R_YA }, { NRIGHT, R_XB }, { 0 }
};

static struct rspecial helper32_1[] = {
	{ NLEFT, R_YA }, { 0 }
};

static struct rspecial shift16[] = {
	{ NLEFT, R_A }, {NRIGHT, R_B }, { 0 }
};

static struct rspecial shift32[] = {
	{ NLEFT, R_YA }, {NRIGHT, R_B }, { 0 }
};

/*
 *	Conversion .. need to force the right output register
 */
static struct rspecial conv32[] = {
	{NRES, R_BA}, { 0 }
};

/*
 *	Constant compare - trashes B
 */
static struct rspecial oplogc16[] = {
	{ NOLEFT, R_B },  { NEVER, R_B }, { 0 }
};

/*
 *	Structure copy - put the values in X Y B
 */

static struct rspecial struct_copy[] = {
	{ NLEFT, R_X }, { NRIGHT, R_Y },
	{ NEVER, R_X }, { NEVER, R_Y },
	{ NEVER, R_A }, { NEVER, R_B },
	{ 0 }
};

struct rspecial *nspecial(struct optab *q)
{
	switch (q->op) {
		case SCONV:
			if (q->visit & INAREG) {
				if (q->rtype & TCHAR)
					return load8;
				else
					return load16;
			}
			if (q->visit & INBREG)
				return conv32;
			break;
		case UMUL:
			if (q->visit & INAREG) {
				if (q->rtype & TCHAR)
					return opl8;
				else
					return opl16;
			}
			if (q->visit & INBREG)
				return conv32;
			break;
		case MINUS:
		case PLUS:
		case MUL:
		case DIV:
		case MOD:
			if (q->visit & INBREG)
				return helper32;
			if (q->visit & INAREG)
				return helper16;
			break;
		case LS:
		case RS:
			if (q->visit & INAREG)
				return shift16;
			if (q->visit & INBREG)
				return shift32;
			break;
		case ASSIGN:
			/* Load left with right, store right to left */
			if (q->visit & INAREG) {
				if (q->lshape == SAREG) {
					if (q->ltype & TCHAR)
						return load8;
					else
						return load16;
				}
				if (q->rtype & TCHAR)
					return store8;
				return store16;
			}
			if (q->visit & INBREG) {
				if (q->lshape == SBREG)
					return load32;
				return store32;
			}
			break;
		case OPLOG:
			if (q->lshape & INAREG)
				return oplogc16;
			break;
		case OPLTYPE:
			if (q->visit & INAREG) {
				if (q->rtype & TCHAR)
					return opl8;
				else
					return opl16;
			}
			if (q->visit & INBREG)
				return opl32;
			break;
		case UMINUS:
			if (q->visit & INBREG)
				return helper32_1;
			break;
		case STASG:
			return struct_copy;
	} 
	comperr("nspecial entry %d", q - table);
	return 0; /* XXX gcc */
}

/*
 * Set evaluation order of a binary node if it differs from default.
 */
int
setorder(NODE *p)
{
	return 0; /* nothing differs on tms9995 */
}

/*
 * set registers in calling conventions live.
 *
 * No work needed
 */
int *
livecall(NODE *p)
{
	static int r[] = { -1 };
	return r;
}

/*
 * Signal whether the instruction is acceptable for this target.
 */
int
acceptable(struct optab *op)
{
	return 1;
}
