/*	$Id: local2.c,v 1.20 2019/04/27 20:35:52 ragge Exp $	*/
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
# include <ctype.h>
# include <string.h>

static void lpput(NODE *p);

static int spcoff;
static int argsiz(NODE *p);
static void negcon(FILE *fp, int con);

void
deflab(int label)
{
	printf(LABFMT ":\n", label);
}

/*
 *	It would be nicer to put the frame pointer at the base of the frame
 *	with all values indexed upwards but that doesn't seem to be handled
 *	by the compiler core so do it the generic way
 */
void
prologue(struct interpass_prolog *ipp)
{
	int addto;
	int i;

#ifdef LANG_F77
	if (ipp->ipp_vis)
		printf(".export	%s\n", ipp->ipp_name);
	printf("%s:\n", ipp->ipp_name);
#endif
	printf("push.w	fp\n");

	addto = p2maxautooff - 2;

	/* We juggle these around a bit so that r7 is pointing to the
	   first value not to the saved old r7. This makes our first
	   stacked value accessible via *r7 which saves us a word (and
	   a bunch of cycles) a reference */

	if (addto > 0)
		printf("addi.w	sp,%d\n", addto);
	printf("mov.w	sp,fp\n");

	/* Save old register variables below the frame */
	for (i = 0; i < 32; i++)
		if (TESTBIT(p2env.p_regs, i))
			printf("push.w %s\n", rnames[i]);
	spcoff = 0;
}

/* TODO - if we did an alloca() who owned the cleanup ?? */
void
eoftn(struct interpass_prolog *ipp)
{
	int i;
	if (spcoff)
		comperr("spcoff == %d", spcoff);
	if (ipp->ipp_ip.ip_lbl == 0)
		return; /* no code needs to be generated */

	/* our registers should be top of stack */
	for (i = 0; i < 32; i++)
		if (TESTBIT(p2env.p_regs, i))
			printf("pop.w %s\n", rnames[i]);
	printf("jmp	cret\n");
}

/*
 * add/sub/...
 *
 * Param given:
 */
void
hopcode(int f, int o)
{
	char *str;

	switch (o) {
	case PLUS:
		str = "a";
		break;
	case MINUS:
		str = "s";
		break;
	case AND:
		str = "and";
		break;
	case OR:
		str = "or";
		break;
	case ER:
		str = "xor";
		break;
	default:
		comperr("hopcode2: %d", o);
		str = 0; /* XXX gcc */
	}
	printf("%s%c", str, f);
}

/*
 * Return type size in bytes.  Used by R2REGS, arg 2 to offset().
 */
int tlen(NODE *p)
{
	switch(p->n_type) {
		case CHAR:
		case UCHAR:
			return(1);

		case SHORT:
		case USHORT:
			return(SZSHORT/SZCHAR);

		case FLOAT:
		case DOUBLE:
			return(SZFLOAT/SZCHAR);

		case INT:
		case UNSIGNED:
		case LONG:
		case ULONG:
			return(SZINT/SZCHAR);

		case LONGLONG:
		case ULONGLONG:
			return SZLONGLONG/SZCHAR;

		default:
			if (!ISPTR(p->n_type))
				comperr("tlen type %d not pointer");
			return SZPOINT(p->n_type)/SZCHAR;
		}
}

static void
uconput(FILE *fp, NODE *p)
{
	int val = getlval(p);

	switch (p->n_op) {
	case ICON:
		if (p->n_name[0] != '\0') {
			fprintf(fp, "%s", p->n_name);
			if (val)
				fprintf(fp, "+%d", val & 0xFFFF);
		} else if (p->n_type == LONG || p->n_type == ULONG) {
			val >>= 16;
			negcon(fp, val & 0xFFFF);
		} else
			negcon(fp, val);
		return;

	default:
		comperr("illegal conput, p %p", p);
	}
}


/* rmove is used directly for temporary register moves in the compiler as
   well as by rule based code */
void
rmove(int s, int d, TWORD t)
{
	if (t == FLOAT) {
		comperr("float rmove");
		return;
	}
	if (t < LONG)
		printf("ld.w %s,%s\n",
			rnames[s], rnames[t]);
	else
		printf("ld.l %s,%s\n",
			rnames[s]. rnames[t]);
}

void zzzcode(NODE *p, int c)
{
	struct attr *ap;
	NODE *l;
	int o;
	int len;

	switch (c) {
	case 'C': /* subtract stack after call */
		spcoff -= p->n_qual;
		if (p->n_qual == 2)
			printf("inct	r13\n");
		else if (p->n_qual > 2)
			printf("ai	r13,%d\n", (int)p->n_qual);
		break;
	case 'G':
		rmove(regno(p->n_right), regno(p->n_left), p->n_type);
		break;
	/* TODO : unroll for smaller structs */
	case 'I': /* struct assign. Right in R1, left R2, counter R0. */
		ap = attr_find(p->n_ap, ATTR_P2STRUCT);
		l = p->n_left;
		len = ap->iarg(0);
		
		if (l->n_op == OREG) {
			int r = l->n_rval;
			if (R2TEST(r)) {
				l->n_rval = R2UPK1(r);
				expand(p, FOREFF, "mov	AL,r1\n");
				l->n_rval = r;
			} else {
				if (r != 1)
					printf("mov	%s,r1\n", regname(r));
				if (getlval(l))
					printf("ai	r1, %d\n", (int)getlval(l));
			}
		} else
			printf("li	r1, @%s\n", l->n_name);
		o = getlab2();
		printf("li	r0, %d\n", (len + 1) >> 1);
		deflab(o);
		printf("mov	*r2+, *r1+\n");
		printf("dec	r0\njne	" LABFMT "\n", o);
		break;
	/* TODO : unroll for smaller structs */
	case 'J': /* struct argument */
		ap = attr_find(p->n_ap, ATTR_P2STRUCT);
		o = (ap->iarg(0) + 1) & ~1;
		if (o == 2)
			printf("dect	r13");
		else
			printf("ai	r13, %d", -o);
		printf("mov	r13,r2\n");
		printf("li	r0, %d\n", o >> 1);
		deflab(o);
		printf("mov	*r1+, *r2+\n");
		printf("dec	r0");
		printf("jne	" LABFMT "\n", o);
		spcoff += argsiz(p);
		break;
	case 'S': /* Adust sp for argument push */
		spcoff += argsiz(p);
		break;
	case 'Z': /* Force debug */
		fwalk(p, e2print, 0);
		fflush(stdout);
		break;
	default:
		comperr("zzzcode %c", c);
	}
}

/*ARGSUSED*/
int
rewfld(NODE *p)
{
	return(1);
}

/*
 * Does the bitfield shape match?
 */
int
flshape(NODE *p)
{
	int o = p->n_op;

	if (o == OREG || o == REG || o == NAME)
		return SRDIR; /* Direct match */
	if (o == UMUL && shumul(p->n_left, SOREG))
		return SROREG; /* Convert into oreg */
	return SRREG; /* put it into a register */
}

/* INTEMP shapes must not contain any temporary registers */
/* XXX should this go away now? */
int
shtemp(NODE *p)
{
	return 0;
}

static void
negcon(FILE *fp, int con)
{
	if (con < 0)
		fprintf(fp, "-"), con = -con;
	fprintf(fp, "%d", con & 0xFFFF);
}

void
adrcon(CONSZ val)
{
	printf(CONFMT, val);
}

void
conput(FILE *fp, NODE *p)
{
	int val = getlval(p);

	switch (p->n_op) {
	case ICON:
		if (p->n_name[0] != '\0') {
			fprintf(fp, "%s", p->n_name);
			if (val)
				fprintf(fp, "+%d", val & 0xFFFF);
		} else if (p->n_type == LONG || p->n_type == ULONG)
			negcon(fp, val & 0xFFFF);
		else
			negcon(fp, val);
		return;

	default:
		comperr("illegal conput, p %p", p);
	}
}


/*ARGSUSED*/
void
insput(NODE *p)
{
	comperr("insput");
}

/*
 * Write out the upper address, like the upper register of a 2-register
 * reference, or the next memory location.
 */
void
upput(NODE *p, int size)
{
	size /= SZINT;
	switch (p->n_op) {
	case ICON:
		negcon(stdout, (getlval(p) >> 16) & 0xFFFF);
		break;
	default:
		comperr("upput bad op %d size %d", p->n_op, size);
	}
}

/*
 * output an address, with offsets, from p
 */
void
adrput(FILE *io, NODE *p)
{
	int lv;
	int at = 0;

	if (p->n_op == FLD)
		p = p->n_left;

	switch (p->n_op) {
	case NAME:
		/* A constant name possibly with an offset */
		if (p->n_name[0] != '\0') {
			fputs(p->n_name, io);
			if (getlval(p) != 0)
				fprintf(io, "+%d", getlval(p) & 0xFFFF);
		} else
			negcon(io, getlval(p));
		return;

	case OREG:
		lv = getlval(p);
		/* A named object maybe with offset */
		if (p->n_name[0]) {
			fprintf(io, "%s%s", p->n_name, lv ? "+" : "");
			at = 1;
		}
		if (lv) {
			if (!at)
				fprintf(io, "@");
			fprintf(io, "%d", lv);
			at = 1;
		}
		if (at)
			fprintf(io, "(%s)", rnames[p->n_rval]);
		else
			fprintf(io, "*%s", rnames[p->n_rval]);
		return;
	case ICON:
		conput(io, p);
		return;

	case REG:
		fprintf(io, "%s", rname[p->n_rval]);
		return;

	case UMUL:
		if (tshape(p, STARNM)) {
			fprintf(io, "*");
			adrput(io, p->n_left);
			break;
		}
	default:
		comperr("illegal address, op %d, node %p", p->n_op, p);
		return;

	}
}

/*
 *	The real processor only allows for short range branches and
 *	also lacks signed <= and >= tests. The assembler pseudo ops
 *	however support all 10 we need and generates either the op if
 *	in range, or the reversed operation and a B @addr. This is also
 *	why the imaginary lte/gte exists. These are always generated
 *	inverted (ie ljlte foo is actually jgt 2; b @foo)
 */
static char *
ccbranches[] = {
	"jp eq, ",		/* jumpe */
	"jp ne, ",		/* jumpn */
	/* Signed ST1 / ST 2*/
	"jp lte, ",		/* jumple */
	"jp lt, ",		/* jumpl */
	"jp gte, ",		/* jumpge */
	"jp gt, ",		/* jumpg */
	/* Unsigned ST0 / ST2 */
	"jp le"	,	/* jumple (jlequ) */
	"jp l, ",		/* jumpl (jlssu) */
	"jp he, ",		/* jumpge (jgequ) */
	"jp h, ",		/* jumpg (jgtru) */
};


/*   printf conditional and unconditional branches */
void
cbgen(int o, int lab)
{
	if (o < EQ || o > UGT)
		comperr("bad conditional branch: %s", opst[o]);
	printf("%s	" LABFMT "\n", ccbranches[o-EQ], lab);
}

#define	IS1CON(p) ((p)->n_op == ICON && getlval(p) == 1)

static void
fixops(NODE *p, void *arg)
{
	static int fltwritten;

	if (!fltwritten && (p->n_type == FLOAT || p->n_type == DOUBLE)) {
		printf(".globl	fltused\n");
		fltwritten = 1;
	}
}

void
myreader(struct interpass *ipole)
{
	struct interpass *ip;

#ifdef PCC_DEBUG
	if (x2debug) {
		printf("myreader before\n");
		printip(ipole);
	}
#endif
	DLIST_FOREACH(ip, ipole, qelem) {
		if (ip->type != IP_NODE)
			continue;
		walkf(ip->ip_node, fixops, 0);
		canon(ip->ip_node); /* call it early */
	}
#ifdef PCC_DEBUG
	if (x2debug) {
		printf("myreader middle\n");
		printip(ipole);
	}
#endif
#ifdef PCC_DEBUG
	if (x2debug) {
		printf("myreader after\n");
		printip(ipole);
	}
#endif
}

/*
 * Remove SCONVs where the left node is an OREG with a smaller type.
 */
static void
delsconv(NODE *p, void *arg)
{
}

void
mycanon(NODE *p)
{
	walkf(p, delsconv, 0);
}

void
myoptim(struct interpass *ip)
{
}

/*
 * For class c, find worst-case displacement of the number of
 * registers in the array r[] indexed by class.
 */
int
COLORMAP(int c, int *r)
{
	switch (c) {
	case CLASSA:
		/* Each class B register can cost us two class A */
		return  c[0] < 31;	/* We have 31 class A */
	case CLASSB:
		return cb < 14;	/* We have 14 */
	}
	return 0;
}

char *rnames[] = {
	"w0", "w1", "w2", "w3", "w4", "w5", "w6", "w7",
	"w8", "w9", "w10", "w11", "w12", "w13", "w14", "w15",
	"w16", "w17", "w18", "w19", "w20", "w21", "w22", "w23",
	"w24", "w25", "w26", "w27", "w28", "w29", "w30", "w31",
	"l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7",
	"l8", "l9", "l10", "l11", "l12", "l13", "l14", "l15",
	"l16", "l17", "l18", "l19", "l20", "l21", "l22", "l23",
	"l24", "l25", "l26", "l27", "l28", "l29", "l30", "l31"
};
 
/*
 * Return a class suitable for a specific type.
 */
int
gclass(TWORD t)
{
	if (t < LONG || t > BTMASK)
		return CLASSA;
	if (t == LONG || t == ULONG)
		return CLASSB;
	if (t == FLOAT || t == DOUBLE || t == LDOUBLE)
		return CLASSC;
	comperr("gclass");
	return CLASSD;
}

static int
argsiz(NODE *p)  
{
	TWORD t = p->n_type;

	if (t == LONG || t == ULONG || t == FLOAT)
		return 4;
	if (t == DOUBLE)
		return 8;
	if (t == STRTY || t == UNIONTY)
		return attr_find(p->n_ap, ATTR_P2STRUCT)->iarg(0);
	return 2;
}

/*
 * Argument specialties.
 */
void
lastcall(NODE *p)
{
	NODE *op = p;
	int size = 0;

	/*
	 * Calculate arg sizes.
	 * Mark first arg not to have - before it.
	 */
	p->n_qual = 0;
	if (p->n_op != CALL && p->n_op != FORTCALL && p->n_op != STCALL)
		return;
	for (p = p->n_right; p->n_op == CM; p = p->n_left) {
		p->n_right->n_qual = 0;
		size += argsiz(p->n_right);
	}
	p->n_qual = 0;
	size += argsiz(p);
	p = op->n_right;

	if (p->n_op == CM)
		p = p->n_right;
#if 0 /* XXX fixme */
	if (p->n_type == FLOAT || p->n_type == DOUBLE ||
	    p->n_type == STRTY || p->n_type == UNIONTY)
		op->n_flags |= NLOCAL1;	/* Does not use stack slot */
	else
		op->n_flags &= ~NLOCAL1;
#endif
	op->n_qual = size; /* XXX */
}

static int
is1con(NODE *p)
{
	if (p->n_op == ICON && getlval(p) == 1)
		return 1;
	return 0;
}

/*
 * Special shapes.
 */
int
special(NODE *p, int shape)
{
	return SRNOPE;
}

/*
 * Target-dependent command-line options.
 */
void
mflags(char *str)
{
}

/*
 * Do something target-dependent for xasm arguments.
 */
int
myxasm(struct interpass *ip, NODE *p)
{
	return 0;
}

int
fldexpand(NODE *p, int cookie, char **cp)
{
	return 0;
}
