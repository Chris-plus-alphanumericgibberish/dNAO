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
	int anger;
	int sacrifices;		/* TODO:  how much of a positive balance you have with this god for sac gifts */
};

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

#define is_sarnath_god(godnum)	(godnum == GOD_TAMASH || godnum == GOD_LOBON || godnum == GOD_ZO_KALAR)
#define hell_safe_prayer(p_god)	(galign(p_god) == A_VOID || p_god == GOD_LOLTH || (u.ualign.god == p_god && uarmh && uarmh->oartifact == ART_MITRE_OF_HOLINESS))
#define philosophy_index(god_index) ((god_index) == GOD_THE_COLLEGE || (god_index) == GOD_THE_CHOIR || (god_index) == GOD_DEFILEMENT)
#define no_altar_index(god_index) (philosophy_index(god_index) || (god_index) == GOD_THE_BLACK_MOTHER)

#define YOG_BAD	(u.ualign.type != A_NEUTRAL && u.ualign.type != A_NONE && u.ualign.type != A_VOID && !(artinstance[ART_FINGERPRINT_SHIELD].FingerprintProgress == 3 && (Role_if(PM_ANACHRONONAUT) || philosophy_index(u.ualign.god))))
#define GOAT_BAD	(u.ualign.type != A_CHAOTIC && u.ualign.type != A_NONE && u.ualign.type != A_VOID && !Role_if(PM_ANACHRONONAUT) && !philosophy_index(u.ualign.god))
#define FLAME_BAD	(u.ualign.type != A_LAWFUL && u.ualign.type != A_NONE && u.ualign.type != A_VOID && !Role_if(PM_ANACHRONONAUT) && !philosophy_index(u.ualign.god))


#endif	/* GODS_H */