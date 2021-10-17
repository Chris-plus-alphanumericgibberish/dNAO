/* gods.h	*/
/* NetHack Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef GODS_H
#define GODS_H

#include "align.h"

struct god {
	const char * name;
	aligntyp alignment;
	int holiness;
	int minionlist[20];
};

extern struct god godlist[];

struct crowning {
	int godnum;					/* god that grants this crowning */
	int crowninggift;			/* artifact */
	const char * title;			/* title displayed in enlightenment and dumplog */
	const char * announcement;	/* godvoice() announcement -- if godvoice is not used, 0 and special-cased */
	const char * livelogstr;	/* livelog string */
	int title_mod;				/* what should %s in the title be filled with? */
#define CRWN_TTL_LORD	1	/* Lord / Lady */
#define CRWN_TTL_KING	2	/* King / Queen */
#define CRWN_TTL_PRIE	3	/* Priest / Priestess */
#define CRWN_TTL_NAME	4	/* playername */
};

#endif	/* GODS_H */