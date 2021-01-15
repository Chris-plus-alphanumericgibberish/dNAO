/*	SCCS Id: @(#)exstruct.h	3.4	1992/11/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef EXSTRUCT_H
#define EXSTRUCT_H

/*	various types of food, the lower, the better liked.	*/
#define DOGFOOD 0
#define CADAVER 1
#define ACCFOOD 2
#define MANFOOD 3
#define APPORT	4
#define POISON	5
#define UNDEF	6
#define TABU	7
struct edog {
	long droptime;			/* moment dog dropped object */
	unsigned dropdist;		/* dist of drpped obj from @ */
	int apport;			/* amount of training */
	long whistletime;		/* last time he whistled */
	long hungrytime;		/* will get hungry at this time */
	coord ogoal;			/* previous goal location */
	int abuse;			/* track abuses to this pet */
	int revivals;			/* count pet deaths */
	int mhpmax_penalty;		/* while starving, points reduced */
	Bitfield(killed_by_u, 1);	/* you attempted to kill him */
//ifdef BARD
	Bitfield(friend, 1);		/* tamed by song - will lose tameness */
	Bitfield(waspeaceful, 1);      	/* was peaceful before tame song */
//endif
	Bitfield(loyal, 1);      	/* is particularly loyal (starting pet, quest home pet) */
};
#define EDOG(mon)	((mon)->mextra_p->edog_p)


struct ehor {
	/* allocated memory for a random name */
	char randname[40];
	/* standard data to base this horror off of */
	struct permonst basehorrordata;
	/* current and modifiable memory (for zombified, etc) */
	struct permonst currhorrordata;
};
#define EHOR(mon)	((mon)->mextra_p->ehor_p)


struct emin {
	aligntyp min_align;	/* alignment of minion */
};
#define EMIN(mon)	((mon)->mextra_p->emin_p)

struct emon {
	long size;				/* size of mondat in bytes; can be >sizeof(struct monst) to hold mextra bundle */
	struct monst mondat;	/* all of a single struct monst, plus potentially an mextra bundle */
};
#define EMON(obj)	(&((obj)->oextra_p->emon_p->mondat))


struct enam {
	long name_lth;			/* length of name in bytes; how far to read */
	char name[PL_PSIZ];		/* length is actually name_lth */
};
#define M_HAS_NAME(mtmp)	((mtmp) && (mtmp)->mextra_p && (mtmp)->mextra_p->enam_p)
#define MNAME(mtmp)	((mtmp)->mextra_p->enam_p->name)
#define ONAME(otmp)	((otmp)->oextra_p->enam_p->name)

struct epri {
	aligntyp shralign;	/* alignment of priest's shrine */
				/* leave as first field to match emin */
	schar shroom;		/* index in rooms */
	coord shrpos;		/* position of shrine */
	d_level shrlevel;	/* level (& dungeon) of shrine */
	boolean pbanned;	/* player banned by priest */
	char signspotted;	/* max number of signs spotted by priest */
};
#define HAS_EPRI(mon)	((mon) && (mon)->mextra_p && (mon)->mextra_p->epri_p)
#define EPRI(mon)	((mon)->mextra_p->epri_p)
/* A priest without ispriest is a roaming priest without a shrine, so
 * the fields (except shralign, which becomes only the priest alignment)
 * are available for reuse.
 */
#define renegade shroom


#define REPAIR_DELAY	5	/* minimum delay between shop damage & repair */
#define BILLSZ	200
struct bill_x {
	unsigned bo_id;
	boolean useup;
	long price;		/* price per unit */
	long bquan;		/* amount used up */
};
struct eshk {
	long robbed;		/* amount stolen by most recent customer */
	long credit;		/* amount credited to customer */
	long debit;		/* amount of debt for using unpaid items */
	long loan;		/* shop-gold picked (part of debit) */
	int shoptype;		/* the value of rooms[shoproom].rtype */
	schar shoproom;		/* index in rooms; set by inshop() */
	schar unused;		/* to force alignment for stupid compilers */
	boolean following;	/* following customer since he owes us sth */
	boolean surcharge;	/* angry shk inflates prices */
	boolean pbanned;	/* player is banned from the shop */
	char signspotted;	/* max number of signs spotted by shopkeeper */
	coord shk;		/* usual position shopkeeper */
	coord shd;		/* position shop door */
	d_level shoplevel;	/* level (& dungeon) of his shop */
	int billct;		/* no. of entries of bill[] in use */
	struct bill_x bill[BILLSZ];
	struct bill_x *bill_p;
	int visitct;		/* nr of visits by most recent customer */
	char customer[PL_NSIZ]; /* most recent customer */
	char shknam[PL_NSIZ];
#ifdef OTHER_SERVICES
	long services;          /* Services offered */
#define SHK_ID_BASIC    00001L
#define SHK_ID_PREMIUM  00002L
#define SHK_UNCURSE     00010L
#define SHK_APPRAISE    00100L
#define SHK_SPECIAL_A   01000L
#define SHK_SPECIAL_B   02000L
#define SHK_SPECIAL_C   04000L
#endif
};
#define ESHK(mon)	((mon)->mextra_p->eshk_p)
/* TODO: deprecate these */
#define NOTANGRY(mon)	((mon)->mpeaceful)
#define ANGRY(mon)	(!NOTANGRY(mon))

struct esum {
	struct monst * summoner;	/* monster responsible for this mon/obj -- can be null */
	unsigned sm_id;				/* m_id of summoner */
	int summonstr;				/* tax on summoner, who may maintain up to its own HD of summons at a time. */
};


#define FCSIZ	(ROWNO+COLNO)
struct fakecorridor {
	xchar fx,fy,ftyp;
};

struct evgd {
	int fcbeg, fcend;	/* fcend: first unused pos */
	int vroom;		/* room number of the vault */
	xchar gdx, gdy;		/* goal of guard's walk */
	xchar ogx, ogy;		/* guard's last position */
	d_level gdlevel;	/* level (& dungeon) guard was created in */
	xchar warncnt;		/* number of warnings to follow */
	Bitfield(gddone,1);	/* true iff guard has released player */
	Bitfield(unused,7);
	struct fakecorridor fakecorr[FCSIZ];
};

#define EVGD(mon)	((mon)->mextra_p->evgd_p)

#endif /* EXSTRUCT_H */
