/*	SCCS Id: @(#)align.h	3.4	1991/12/29	*/
/* Copyright (c) Mike Stephenson, Izchak Miller  1991.		  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef ALIGN_H
#define ALIGN_H

typedef schar	aligntyp;	/* basic alignment type */

typedef struct	align {		/* alignment & record */
	aligntyp	type;
	int		god;
	int		record;
	int		sins; //records trasgressions that should lower the max record.
} align;

/* NetHack may be freely redistributed.  See license for details. */

/* bounds for "record" -- respect initial alignments of 10 */
#define ALIGNLIM	(min(100L, 10L + (moves/200L) - u.ualign.sins))

#define A_NONE		(-128)	/* the value range of type */

#define A_CHAOTIC	(-1)
#define A_NEUTRAL	 0
#define A_LAWFUL	 1

#define A_VOID		 -3 /* chosen to line up with topten and aligns[] (aligns[] is defined in role.c) */

/* alignment mask values -- for sp_lev and role */
#define AM_NONE		 0
#define AM_CHAOTIC	 1
#define AM_NEUTRAL	 2
#define AM_LAWFUL	 4
#define AM_VOID		 8
#define AM_MASK		 15
#define AM_SPLEV_CO	 3
#define AM_SPLEV_NONCO	 7

#define HOLY_HOLINESS	 1
#define NEUTRAL_HOLINESS 0
#define UNHOLY_HOLINESS -1
#define VOID_HOLINESS -2

#define GOAT_EAT_PASSIVE 0
#define GOAT_EAT_OFFERED 1
#define GOAT_EAT_MARKED 2

#define Amask2align(x)	((aligntyp) ( (x)==AM_NONE ? A_NONE : \
									  (x)==AM_CHAOTIC ? A_CHAOTIC :\
									  (x)==AM_NEUTRAL ? A_NEUTRAL :\
									  (x)==AM_LAWFUL ? A_LAWFUL :\
									  (x)==AM_VOID ? A_VOID : A_NONE\
									))
#define Align2amask(x)	( (x)==A_NONE ? AM_NONE : \
						  (x)==A_CHAOTIC ? AM_CHAOTIC :\
						  (x)==A_NEUTRAL ? AM_NEUTRAL :\
						  (x)==A_LAWFUL ? AM_LAWFUL :\
						  (x)==A_VOID ? AM_VOID : AM_NONE\
						)

#define Align2ritual(x)	( (x)==A_LAWFUL ? RITUAL_LAW :\
						  (x)==A_NEUTRAL ? RITUAL_NEUTRAL :\
						  (x)==A_CHAOTIC ? RITUAL_CHAOS :\
						  0\
						)
#define Align2hiritual(x)	( (x)==A_LAWFUL ? RITUAL_HI_LAW :\
						  (x)==A_NEUTRAL ? RITUAL_HI_NEUTRAL :\
						  (x)==A_CHAOTIC ? RITUAL_HI_CHAOS :\
						  0\
						)
#endif /* ALIGN_H */
