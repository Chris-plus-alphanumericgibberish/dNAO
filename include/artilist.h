/*	SCCS Id: @(#)artilist.h 3.4	2003/02/12	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef ARTILIST_H
#define ARTILIST_H

#include "macromagic.h"
/* we need to set these *before* makedefs.c or else it won't be getting the right number of arguments */
#define PROPS(...) {FIRST_TEN(dummy, ##__VA_ARGS__, 0,0,0,0,0,0,0,0,0,0)}
#define FIRST_TEN(dummy, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ...) a1, a2, a3, a4, a5, a6, a7, a8, a9, a10

#define NO_MONS()									 0,   0,   0,   0,   0,   0,   0,   0,   0
//#define MONS(mt, mfm, mft, mfb, mfg, mfr, mfv, mfw)		mt, mfm, mft, mff, mfb, mfg, mfr, mfv, mfw

#define MONS(...) SET09(0,0,0,0,0,0,0,0,0, __VA_ARGS__)
#define vsMSYM(x) C01((x))
#define vsMM(x)   C02((x))
#define vsMT(x)   C03((x))
#define vsMF(x)   C04((x))
#define vsMB(x)   C05((x))
#define vsMG(x)   C06((x))
#define vsMA(x)   C07((x))
#define vsMV(x)   C08((x))
#define vsMW(x)   C09((x))

#define NO_ATTK()									    0,   0,   0
#define ATTK(adtyp, acc, dam)						adtyp, acc, dam

#define NOINVOKE 0
#define NOFLAG 0

#ifdef MAKEDEFS_C
/* in makedefs.c, all we care about is the list of names */

#define A(nam, ...) nam

static const char *artifact_names[] = {
#else
/* set up the actual artifact list structure */
#include "hack.h"
#include "artifact.h"

#define A(nam, typ, desc, cost, mat, siz, wgt, aln, cls, rac, val, gen, vsmons, attack, afl, wpr, wfl, cpr, cfl, inv, ifl) { \
	 typ, nam, desc, \
	 cost, mat, siz, wgt, \
	 aln, cls, rac, val, gen, \
	 vsmons, \
	 attack, afl, \
	 wpr, wfl, \
	 cpr, cfl, \
	 inv, ifl }
//struct artifact * artilist;
NEARDATA const struct artifact base_artilist[] = {
#endif	/* MAKEDEFS_C */

/* Artifact cost rationale:
 * 1.  The more useful the artifact, the better its cost.
 * 2.  Quest artifacts are highly valued.
 * 3.  Chaotic artifacts are inflated due to scarcity (and balance).
 */

/*  dummy element #0, so that all interesting indices are non-zero */
A("",					STRANGE_OBJECT,					(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, NO_TIER, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

//////////////////////Crowning Gifts///////////////////////////////////////
/*Take Me Up/Cast Me Away*/
/*Excalibur is a very accurate weapon, a property that almost doesn't matter except for vs high level demons*/
/*Excalibur does extra blessed damage to demons and undead, +3d7 instead of 1d4*/
A("Excalibur",			LONG_SWORD,						(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_KNIGHT, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_INHER|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 10), NOFLAG,
	PROPS(DRAIN_RES, SEARCHING), (ARTP_SEEK),
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Dirge",			LONG_SWORD,						"half-melted %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_KNIGHT, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_INHER|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_ACID, 5, 10), NOFLAG,
	PROPS(ACID_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*
 *	Stormbringer only has a 2 because it can drain a level,
 *	providing 8 more.
 *
 *  Stormbringer now has a 10% chance to get +8 damage,
 *  and then convert all damage to level drain damage (dmg/4)
 *  
 *  Stormbringer now always counts as cursed against curse hating
 *  targets
 *  
 *  Stormbringer now gives its wielder great strength and vitality
 *	(25 Str and Con)
 *
 *	Stormbringer cuts through any substance (ie, it counts as shining)
 */
A("Stormbringer",		RUNESWORD,						(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_INHER | ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_DRLI, 5, 2), (ARTA_DRAIN|ARTA_SHINING),
	PROPS(DRAIN_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_BLOODTHRST)
	),
/*
 *	Two problems:  1) doesn't let trolls regenerate heads,
 *	2) doesn't give unusual message for 2-headed monsters (but
 *	allowing those at all causes more problems than worth the effort).
 *
 * Special code in weapon.c uses an extra die (2d8, 2d12) and makes the dice explode
 * According to an article on 1d4Chan, the average of an exploading die is roughly that of a die one size larger
 *  -> effectively, 2d10+2/2d14+2
 */
A("Vorpal Blade",		LONG_SWORD,						(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	ATTK(AD_PHYS, 5, 1), (ARTA_VORPAL|ARTA_DEXPL),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Marauder's Map",	SCR_MAGIC_MAPPING,				"parchment scroll",
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_PIRATE, NON_PM, TIER_B, (ARTG_INHER),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	OBJECT_DET, NOFLAG
	),

/*//////////Nameable Artifacts//////////*/
/*most nameables can be twoweaponed, look in obj.h */

/*
 *	Orcrist and Sting have same alignment as elves.
 */
A("Orcrist",			ELVEN_BROADSWORD,				(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_C, (ARTG_NAME|ARTG_INHER),
	MONS(vsMA(MA_ORC | MA_DEMON)),
	ATTK(AD_PHYS, 10, 0), (ARTA_HATES),
	PROPS(), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*
 *	The combination of SPFX_WARN and M2_something on an artifact
 *	will trigger EWarn_of_mon for all monsters that have the appropriate
 *	M2_something flags.  In Sting's case it will trigger EWarn_of_mon
 *	for MA_ORC monsters.
 */
A("Sting",				ELVEN_DAGGER,					(const char *)0,
	800L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_F, (ARTG_NAME),
	MONS(vsMA(MA_ORC | MA_ARACHNID)),
	ATTK(AD_PHYS, 5, 0), (ARTA_HATES),
	PROPS(), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Grimtooth",			ORCISH_DAGGER,					(const char *)0,
	300L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ORC, TIER_D, (ARTG_NAME),
	MONS(vsMA(MA_ELF | MA_HUMAN | MA_DWARF)),
	ATTK(AD_PHYS, 5, 0), (ARTA_HATES),
	PROPS(), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Carnwennan",			DAGGER,							(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_KNIGHT, NON_PM, TIER_C, (ARTG_NOGEN),	/* ARTG_NAME is added during game-creation for eligible characters */
	MONS(vsMT(MT_MAGIC), vsMA(MA_FEY)),
	ATTK(AD_PHYS, 5, 10), (ARTA_HATES),
	PROPS(STEALTH), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	INVIS, NOFLAG
	),

/*DF Dwarves can be a nasty lot.*/
/*two handed, so no twoweaponing.*/
A("Slave to Armok",		DWARVISH_MATTOCK,				(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_DWARF, TIER_F, (ARTG_NAME),
	MONS(vsMT(MT_PEACEFUL), vsMG(MG_LORD|MG_PRINCE), vsMA(MA_ELF | MA_ORC)),
	ATTK(AD_PHYS, 5, 0), (ARTA_HATES),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_BLOODTHRST)
	),

A("Claideamh",			LONG_SWORD,						(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN | ARTG_NAME),
	MONS(vsMA(MA_ELF | MA_FEY | MA_GIANT | MA_ELEMENTAL | MA_PRIMORDIAL)),
	ATTK(AD_PHYS, 5, 0), (ARTA_HATES),
	PROPS(WARN_OF_MON), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*//////////The Banes//////////*/
/*banes can be twoweaponed, look in obj.h*/

/* also makes a handy weapon for knights, since it can't break */
A("Dragonlance",		LANCE,							(const char *)0,
	5000L, DRAGON_HIDE, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(vsMA(MA_DRAGON | MA_REPTILIAN)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_CANCEL),
	PROPS(REFLECTING), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Nodensfork",			TRIDENT,						"wave-etched %s",
	5000L, SHELL_MAT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(vsMA(MA_PRIMORDIAL | MA_ET), vsMV(MV_TELEPATHIC | MV_RLYEHIAN)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_SILVER|ARTA_CANCEL),
	PROPS(), NOFLAG,
	PROPS(SHOCK_RES, WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Gaia's Fate",		SICKLE,							(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_D, NOFLAG,
	MONS(vsMA(MA_PLANT | MA_INSECTOID | MA_ARACHNID | MA_AVIAN | MA_REPTILIAN | MA_ANIMAL | MA_FEY | MA_ELF | MA_ELEMENTAL)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_CANCEL),
	PROPS(), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Demonbane",			SABER,							(const char *)0,
	2500L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	MONS(vsMA(MA_DEMON)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_CANCEL),
	PROPS(), (ARTP_NOCALL), 
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* silver, protects from lycanthropy, and works against quite a number of late-game enemies */
A("Werebane",			SABER,							"wolf-hilted %s",
	1500L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(vsMA(MA_WERE | MA_DEMIHUMAN)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_CANCEL),
	PROPS(), (ARTP_NOWERE),
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* deducts movement points from hit giants, and also works vs a few late game enemies */
/* bonus damage applies to all large monsters, and has better damage than a normal axe (base d6+d4/3d4 total) */
A("Giantslayer",		AXE,							(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	MONS(vsMT(MT_ROCKTHROW), vsMA(MA_GIANT)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_CANCEL),
	PROPS(), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),


/* effective vs some standard castlevainia enemy types*/
/*is given extra damage in weapon.c, since whip damage is so low*/
A("The Vampire Killer",	BULLWHIP,						"bloodstained chain whip",
	2500L, METAL, MZ_DEFAULT, WT_SPECIAL,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(vsMA(MA_UNDEAD | MA_DEMON | MA_WERE)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_CANCEL),
	PROPS(DRAIN_RES), (ARTP_NOWERE),
	PROPS(), NOFLAG,
	BLESS, NOFLAG
	),

/* works against just about all the late game baddies */
A("Kingslayer",			STILETTO,						"delicately filigreed %s",
	2500L, GOLD, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(vsMG(MG_LORD | MG_PRINCE)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_POIS|ARTA_CANCEL),
	PROPS(), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* speaks for itself */
A("Peace Keeper",		ATHAME,							"minute-rune-covered %s",
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(vsMT(MT_HOSTILE)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_SILVER|ARTA_CANCEL),
	PROPS(), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* vorpal smashes ogres (20/20 chance) */
/* set str and con to 25, and smashing ogres excercises str and wis <- A-tier */
A("Ogresmasher",		WAR_HAMMER,						(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	MONS(vsMSYM(S_OGRE)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_VORPAL|ARTA_CANCEL),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* petrifies trolls, gives bonus damage aginst monsters who pop in to ruin your day */
A("Trollsbane",			MORNING_STAR,					(const char *)0,
	2000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	MONS(vsMSYM(S_TROLL), vsMG(MG_REGEN)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_BRIGHT|ARTA_CANCEL),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*//////////First Gifts//////////*/
//first gifts can be twoweaponed by their associated classes, look in obj.h

/*
 *	Mjollnir will return to the hand of the wielder when thrown
 *	if the wielder is a Valkyrie wearing Gauntlets of Power.
 *	
 *	Now it will never strike the Valkyrie or fall to the ground,
 *	if both are in good condition.
 */
A("Mjollnir",			WAR_HAMMER,						"lightning-bolt embossed %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_VALKYRIE, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_ELEC, 5, 24), (ARTA_EXPLELEC),
	PROPS(SHOCK_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Pen of the Void",ATHAME,							(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_VOID, PM_EXILE, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	VOID_CHIME, NOFLAG
	),

A("Luck Blade",			SHORT_SWORD,					"clover-inscribed %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_CONVICT, NON_PM, TIER_D, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 7, 7), (ARTA_DLUCK),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_LUCK)
	),

A("Cleaver",			BATTLE_AXE,						(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_BARBARIAN, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), (ARTA_SHATTER),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),
A("Atlantean Royal Sword",			TWO_HANDED_SWORD,	(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_BARBARIAN, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), (ARTA_SHATTER),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*	Need a way to convert era times to Japanese luni-solar months.*/
A("Kiku-ichimonji",		KATANA,							"chrysanthemum-hilted %s",
	1200L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_SAMURAI, NON_PM, TIER_C, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 12), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),
A("Jinja Naginata",		NAGINATA,						(const char *)0,
	1200L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_SAMURAI, NON_PM, TIER_C, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 12), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Rhongomyniad",		LANCE,							(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_KNIGHT, NON_PM, TIER_C, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Rod of Lordly Might",		MACE,				(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_SPECIAL,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	LORDLY, (ARTI_ENGRAVE)
	),

/* uses the Musicalize skill; learns special abilities from hearing songs */
A("The Singing Sword",	LONG_SWORD,						"softly singing %s",
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_BARD, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_INHER|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 1), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	SINGING, NOFLAG
	),

/*Needs encyc entry*/
A("Xiuhcoatl",			BULLWHIP,							(const char *)0,
	4000L, DRAGON_HIDE, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_FIRE, 3, 12), NOFLAG,
	PROPS(FIRE_RES), NOFLAG,
	PROPS(), NOFLAG,
	LORDLY, NOFLAG
	),

/*invoke for skeletons, life draining */
A("Staff of Necromancy", QUARTERSTAFF,					"skull-topped %s",
	6660L, BONE, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_VAMPIRE, TIER_B, (ARTG_GIFT|ARTG_INHER),
	NO_MONS(),
	ATTK(AD_DRLI, 5, 0), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	PROPS(), NOFLAG,
	SKELETAL_MINION, NOFLAG
	),
/*
 *	Magicbane is a bit different!  Its magic fanfare
 *	unbalances victims in addition to doing some damage.
 */
/*Needs encyc entry?*/
/*provides curse res while wielded */
A("Magicbane",			ATHAME,							"mystically runed %s",
	3500L, METAL, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_WIZARD, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_MAGM, 3, 4), (ARTA_MAGIC),
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*  The name is meant to translate to "Sword of the Fuma clan,"
 *  "Ken" being a word for "sword" (as in "Bokken," "wooden sword")
 *  "no" being "of", and "Fuma-itto" being "Fuma-clan" (or so I hope...),
 *  this being a clan of ninja from the Sengoku era
 *  (there should be an accent over the u of "Fuma" and the o of "-itto").
 *  Nethack Samurai call broadswords "Ninja-to," which is the steriotypical ninja sword.
 *  Aparently, there was no such thing as an actual Ninja-to, it's something Hollywood made up!
 */
A("Fuma-itto no Ken",	BROADSWORD,						(const char *)0,
	1200L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 8), (ARTA_HATES|ARTA_CROSSA|ARTA_POIS),	/* missing: SPFX2_NINJA */
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	CREATE_AMMO, NOFLAG /*makes shuriken*/
	),

A("The Golden Sword of Y'ha-Talla",			SCIMITAR,	"scorpion-bladed %s",
	3000L, GOLD, MZ_DEFAULT, WT_SPECIAL,
	A_NONE, NON_PM, NON_PM, TIER_B, (ARTG_GIFT | ARTG_INHER),
	NO_MONS(),
	ATTK(AD_DRST, 5, 0), (ARTA_POIS),
	PROPS(POISON_RES), NOFLAG,
	PROPS(), NOFLAG,
	LORDLY, NOFLAG
	),

/*Needs encyc entry*/
/* adds sneak attacks "from behind" (allowing sneak attacks for anyone, and increasing damage for rogues/etc) */
A("Spineseeker",		SHORT_SWORD,					(const char *)0,
	1200L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_D, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 6), NOFLAG,
	PROPS(STEALTH), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*//////////Double Damage Artifacts//////////*/

A("Grayswandir",		SABER,							"amber-swirled %s",
	8000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROPS(HALLUC_RES), NOFLAG,
	PROPS(WARNING), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Frost Brand",		LONG_SWORD,						"ice-runed %s",
	3000L, GLASS, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_COLD, 1, 0), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),
A("Fire Brand",			LONG_SWORD,						"ember-runed %s",
	3000L, OBSIDIAN_MT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_FIRE, 1, 0), NOFLAG,
	PROPS(FIRE_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

A("The Green Dragon Crescent Blade",		NAGINATA,	(const char *)0,
	1200L, MT_DEFAULT, MZ_DEFAULT, WT_SPECIAL,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 25), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Mirror Brand",		LONG_SWORD,						"polished %s",
	3000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_MAGM, 1, 0), (ARTA_MAGIC),
	PROPS(REFLECTING), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Sunsword",			LONG_SWORD,						"sun-bladed %s",
	1500L, GOLD, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT | ARTG_INHER),
	MONS(vsMA(MA_UNDEAD | MA_DEMON)),
	ATTK(AD_PHYS, 1, 0), (ARTA_HATES | ARTA_SILVER | ARTA_BRIGHT | ARTA_BLIND | ARTA_SHINING | ARTA_CANCEL),
	PROPS(BLIND_RES, SEARCHING), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_LIGHT)
	),

/*can be thrown by dwarves*/
A("The Axe of the Dwarvish Lords",		BATTLE_AXE,		"dwarf-faced %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_DWARF, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROPS(XRAY_VISION, TELEPORT_CONTROL), NOFLAG, 	/*x-ray vision is for dwarves only.*/
	PROPS(), NOFLAG, 
	NOINVOKE, (ARTI_DIG)
	),

/*returns to your hand.*/
/*Needs encyc entry*/
A("Windrider",			BOOMERANG,						"winged %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, NOFLAG,
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), ARTA_RETURNING,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Rod of the Ram",	MACE,							"ram-headed %s",
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), (ARTA_KNOCKBACKX),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* attack power is bound to wielder's life force */
A("The Atma Weapon",	BEAMSWORD,						"horned %s",
	6660L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	MONS(vsMG(MG_NASTY)),
	ATTK(AD_PHYS, 6, 6), (ARTA_HATES),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* attack power is bound to wielder's magical energy */
A("Limited Moon",		MOON_AXE,						(const char *)0,
	6660L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Black Arrow",	ANCIENT_ARROW,					(const char *)0,
	4444L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*also has a haste effect when wielded, but massively increases hunger and damages the wielder*/
/*The invoked attack is very powerful*/
A("Tensa Zangetsu",		TSURUGI,						"black %s",
	4444L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_GIFT | ARTG_INHER),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROPS(HALF_SPDAM), NOFLAG,
	PROPS(), NOFLAG,
	SPEED_BANKAI, NOFLAG
	),

/*//////////Other Artifacts//////////*/

/*Sort of intermediate between a double damage and a utility weapon,*/
/*Sode no Shirayuki gains x2 ice damage after using the third dance.*/
/*however, it only keeps it for a few rounds, and the other dances are attack magic. */
A("Sode no Shirayuki",	KATANA,							"snow-white %s", // colored not covered
	8000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT | ARTG_INHER),
	NO_MONS(),
	ATTK(AD_COLD, 1, 0), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	PROPS(), NOFLAG,
	ICE_SHIKAI, NOFLAG
	),

/*Tobiume is an awkward weapon.  It loses 3 damage vs large and 2 vs small*/
/*Ram and Fire blast only trigger if enemy is low hp*/
A("Tobiume",			LONG_SWORD,						"three-branched %s",
	8000L, METAL, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_GIFT | ARTG_INHER),
	NO_MONS(),
	ATTK(AD_FIRE, 1, 1), (ARTA_DISARM),
	PROPS(FIRE_RES), NOFLAG,
	PROPS(), NOFLAG,
	FIRE_SHIKAI, NOFLAG
	),

A("The Lance of Longinus",		SPEAR,					(const char *)0,
	1500L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ANTIMAGIC, REFLECTING, HALF_PHDAM, HALF_SPDAM), NOFLAG,
	PROPS(DRAIN_RES), NOFLAG,
	WWALKING, NOFLAG
	),

// /* still uses the old artifact structure */
// /* TODO aggrevate 'f' */
// A("The Pink Panther", DIAMOND,			0,			0,
	// (SPFX_NOGEN|SPFX_RESTR), 0, 0,
	// NO_ATTK(),	NO_DFNS,	NO_CARY,
	// TRAP_DET, A_NONE, PM_ARCHEOLOGIST, NON_PM, 0L,
	// 0,0,0),

A("The Arkenstone",		DIAMOND,						"rainbow-glinting %s", 
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(AGGRAVATE_MONSTER), NOFLAG,
	CONFLICT, (ARTI_PERMALIGHT)
	),

/*Needs encyc entry*/
A("Release from Care",	SCYTHE,							(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 10), (ARTA_VORPAL),
	PROPS(DRAIN_RES), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	HEALING, NOFLAG
	),

/*Needs encyc entry*/
/* can only behead on sneak attacks */
/* uses STR and DEX scaling, +16 max */
A("The Lifehunt Scythe",SCYTHE,							(const char *)0,
	4000L, DRAGON_HIDE, MZ_LARGE, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_GIFT|ARTG_INHER),
	NO_MONS(),
	ATTK(AD_PHYS, 6, 6), (ARTA_HATES|ARTA_VORPAL), /* damage only applies vs living or undead monsters */
	PROPS(DRAIN_RES, STEALTH), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	INVIS, (ARTI_PLUSTEN)
	),

/* effectively becomes two sizes larger when lit, requiring two hands */
/* attack bonus only applies vs non-magic resistant targets and only when lit */
A("The Holy Moonlight Sword",	LONG_SWORD,				(const char *)0, // begging for a description but i couldn't think of a good one
	4000L, METAL, MZ_DEFAULT, WT_DEFAULT, // if it gets one, it would be sick to have a slightly different one while lit, requires some work tho
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT|ARTG_INHER),
	NO_MONS(),
	ATTK(AD_PHYS, 12, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	ENLIGHTENING, (ARTI_PLUSTEN)
	),

/* can be transformed by offering your blood */
/* transformation costs 20% health to activate, lasts for 20+2d10 turns */
/* while active, adds +defender mlev to blood damage, can be applied to cause an AoE and clear active*/
A("Bloodletter",	MORNING_STAR,				(const char *)0,
	4000L, IRON, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, (ARTG_INHER),
	NO_MONS(),
	ATTK(AD_BLUD, 8, 12), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	BLOODLETTER, (ARTI_PLUSTEN)
	),

/*Needs encyc entry*/
A("The Silence Glaive",	GLAIVE,							(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_GIFT|ARTG_INHER),
	NO_MONS(),
	ATTK(AD_DRLI, 1, 1), (ARTA_DRAIN),
	PROPS(DRAIN_RES), NOFLAG,
	PROPS(), NOFLAG,
	SATURN, NOFLAG
	),

/*Needs encyc entry*/
/*also has a haste effect when wielded, but massively increases hunger*/
A("The Garnet Rod",		UNIVERSAL_KEY,					"garnet-tipped rod",
	8000L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(REGENERATION, ENERGY_REGENERATION), NOFLAG,
	PROPS(), NOFLAG,
	PLUTO, NOFLAG
	),

/*Needs encyc entry*/
/* also protects vs curses while carried */
A("Helping Hand",		GRAPPLING_HOOK,					(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(STEALTH, WARNING, SEARCHING), (ARTP_SEEK),
	PROPS(), NOFLAG,
	UNTRAP, (ARTI_ENGRAVE)
	),

/*Needs encyc entry*/
A("The Blade Singer's Saber",		RAKUYO,				(const char *)0,
	1500L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 8, 8), (ARTA_HASTE),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	BLADESONG, NOFLAG
	),
/*Needs encyc entry*/
A("The Blade Dancer's Dagger",		RAKUYO_DAGGER,				(const char *)0,
	1500L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, (ARTG_NOCNT|ARTG_NOGEN),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 4), (ARTA_HASTE),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	BLADESONG, NOFLAG
	),

/*Needs encyc entry*/
A("The Limb of the Black Tree",		CLUB,				"gnarled tree branch",
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_FIRE, 4, 1), (ARTA_EXPLFIREX),
	PROPS(), NOFLAG,
	PROPS(FIRE_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Hellfire",			CROSSBOW,						(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_FIRE, 4, 1), (ARTA_EXPLFIREX),
	PROPS(FIRE_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Lash of the Cold Waste",		BULLWHIP,			(const char *)0,
	3000L, MINERAL, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_COLD, 4, 1), (ARTA_EXPLCOLDX),
	PROPS(SLEEP_RES), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Ramiel's ranged attack is far more useful than the lash and the limb*/
/*So it's your job to use it right!*/
A("Ramiel",				PARTISAN,						(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_ELEC, 4, 1), (ARTA_EXPLELECX),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* Player gets a second weapon attack after XL15 with this */
A("Quicksilver",		FLAIL,							(const char *)0,
	1200L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 8), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	FAST, NOFLAG
	),

/*Needs encyc entry*/
/* slotless displacement makes this quite good even after you've found a better weapon */
A("Sky Render",			KATANA,							"silver-and-black-iron %s",
	1200L, IRON, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 10), ARTA_SILVER,
	PROPS(), NOFLAG,
	PROPS(DISPLACED), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Crystal sword whose use comes with a price of your sanity */
A("Callandor",			CRYSTAL_SWORD,					(const char *)0,
	6660L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT, // there's an opportunity for a sick name here too
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT), 
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(HALF_SPDAM, ENERGY_REGENERATION, SPELLBOOST), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Unstoppable",	CROSSBOW,						(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 12, 8), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Yoichi no yumi",		YUMI,							(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/*Needs encyc entry*/
/* all arrows fired from it are treated as poisoned (or filthed for a short period after invoking) */
A("Plague",				BOW,							(const char *)0,
	4000L, BONE, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_DRST, 5, 7), NOFLAG,
	PROPS(POISON_RES, SICK_RES), NOFLAG,
	PROPS(), NOFLAG,
	FILTH_ARROWS, NOFLAG
	),

/* all arrows fired from it return to your inventory 5 turns later */
A("Epoch's Curve",			BOW,							"white ash longbow",
	4000L, WOOD, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 1), NOFLAG,
	PROPS(TELEPORT_CONTROL), NOFLAG,
	PROPS(), NOFLAG,
	TELEPORT_SHOES, NOFLAG
	),

/*Needs encyc entry*/
/* die size set to 1d8 in weapon.c */
A("The Fluorite Octahedron",		BLUE_FLUORITE,		"glowing cobalt eight-sided die",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, NOFLAG,
	NO_MONS(),
	NO_ATTK(), (ARTA_DLUCK|ARTA_DEXPL),	/* EXTREMELY potent combination of lucky-exploding dice */
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*//////////Artifact Armors//////////*/

#ifdef TOURIST
/*Needs encyc entry*/
A("The Tie-Dye Shirt of Shambhala",	T_SHIRT,			"flamboyantly colorful %s",
	4500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_S, (ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), (ARTP_LIGHTEN|ARTP_WCATRIB),
	PROPS(), NOFLAG,
	ENLIGHTENING, (ARTI_MANDALA|ARTI_PLUSSEV)
	),
#endif

/*double robe effect*/
/*martial arts attacks use exploding dice and get extra damage*/
A("The Grandmaster's Robe",			ROBE,				(const char *)0,
	4500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("The Cloak of the Unheld One",	OILSKIN_CLOAK,		(const char *)0,
	4500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ANTIMAGIC, FREE_ACTION), NOFLAG,
	PROPS(SLEEP_RES), NOFLAG,
	TELEPORT_SHOES, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("The Beastmaster's Duster",		JACKET,				(const char *)0,
	9000L, DRAGON_HIDE, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	PETMASTER, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("Soulmirror",						PLATE_MAIL,			(const char *)0,
	9000L, MITHRIL, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(DRAIN_RES, REFLECTING), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/* wearer gets a 1d4 poison sting */
A("The Scorpion Carapace",			SCALE_MAIL,			(const char *)0,
	3000L, CHITIN, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_D, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(POISON_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

A("Mirrorbright",					ROUNDSHIELD,		(const char *)0,
	4000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(REFLECTING, HALLUC_RES), NOFLAG,
	PROPS(), NOFLAG,
	CONFLICT, (ARTI_PLUSSEV)
	),

/*Perseus's shield, needs encyc entry*/
A("Aegis",							ROUNDSHIELD,		"gorgon-emblemed %s",
	4000L, LEATHER, MZ_DEFAULT, WT_SPECIAL,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(REFLECTING, HALF_PHDAM), NOFLAG,
	PROPS(), NOFLAG,
	AEGIS, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("The Shield of the All-Seeing",	ORCISH_SHIELD,		(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_ORC, TIER_D, NOFLAG,
	MONS(vsMA(MA_ELF | MA_MINION)),
	NO_ATTK(), NOFLAG,
	PROPS(SEARCHING, FIRE_RES, SEE_INVIS), (ARTP_SEEK),
	PROPS(WARN_OF_MON), NOFLAG,
	ALLSIGHT, (ARTI_PLUSSEV)	/* needs message for invoke */
	),

/*Needs encyc entry*/
A("The Shield of Yggdrasil",		ELVEN_SHIELD,		(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_ELF, TIER_D, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(POISON_RES, REGENERATION), NOFLAG,
	PROPS(), NOFLAG,
	HEALING, (ARTI_PLUSSEV)
	),

/* grants 25 STR as gauntlets of power */
/* heals you when your hp would be depleted at the cost of pw */
A("The Golden Knight",		BUCKLER,		(const char *)0,
	3000L, GOLD, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	QUAKE, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("Whisperfeet",					SPEED_BOOTS,		(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(STEALTH), NOFLAG,
	PROPS(), NOFLAG,
	INVIS, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("Seven League Boots",				JUMPING_BOOTS,		(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(FAST), NOFLAG,
	PROPS(), NOFLAG,
	SEVEN_LEAGUE_STEP, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("Water Flowers",					WATER_WALKING_BOOTS, "silver-flowered %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, NOFLAG,
	NO_MONS(),
	NO_ATTK(), (ARTA_SILVER),
	PROPS(DISPLACED), NOFLAG,
	PROPS(), NOFLAG,
	TELEPORT_SHOES, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
/* freezes water/lava underfoot */
A("Frost Treads",					WATER_WALKING_BOOTS,(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_D, NOFLAG,
	NO_MONS(),
	ATTK(AD_COLD, 1, 8), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("Hammerfeet",						KICKING_BOOTS,		(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_GIFT|ARTG_INHER),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), (ARTA_KNOCKBACKX),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
/* has a chance to make a bonus AT_HUGS attack on targets when hitting */
/* by extension, protects you from being grabbed */
A("The Grappler's Grasp",			GAUNTLETS_OF_POWER,	(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 6), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

A("The Shield of the Resolute Heart",	GAUNTLETS_OF_DEXTERITY,			(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(HALF_PHDAM), NOFLAG,
	PROPS(), NOFLAG,
	BLESS, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
/*It is quite deliberate that these cause a spellcasting penalty */
A("The Gauntlets of Spell Power",	GAUNTLETS_OF_POWER,	"silver-runed %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), (ARTA_SILVER),
	PROPS(HALF_SPDAM, SPELLBOOST), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* grants 25 DEX while worn */
/* potentially massive unarmed damage multiplier */
A("Premium Heart",					GAUNTLETS_OF_POWER,				(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

A("Godhands",					GAUNTLETS_OF_POWER,				(const char *)0,
	7777L, GOLD, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 10), NOFLAG,
	PROPS(DRAIN_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

A("The Wrappings of the Sacred Fist",	HAND_WRAPS,		"prayer-warded wraps",
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_D, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_HOLY, 8, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	FAST_TURNING, (ARTI_PLUSSEV)
	),

/* permanently polymorphs you into a death knight when you die -- does not work for monsters */
A("The Helm of Undeath",			WAR_HAT,				(const char *)0,
	2500L, BONE, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(DRAIN_RES, LIFESAVED), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/* lets the wearer cast Lightning Storm */
A("Stormhelm",						HELM_OF_BRILLIANCE,				(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	PROPS(SHOCK_RES), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/* non-wizards wearing it get reduced-hunger casting (with a -4 INT penalty) */
A("Apotheosis Veil",				CRYSTAL_HELM,			(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(SEE_INVIS, DRAIN_RES, EXTRAMISSION), NOFLAG,
	PROPS(), NOFLAG,
	ENLIGHTENING, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
/* Doubles gold found in the dungeon when worn. */
/* Also gives +1d6 physical damage to attacks when worn. */
A("Ring of Thror",								RIN_GAIN_STRENGTH,	/* granite or gold (Note: since gold is the fallback at most one of this or Narya will be gold) */			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DWARF, TIER_C, (ARTG_INHER),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 6), NOFLAG,
	PROPS(AGGRAVATE_MONSTER), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* Modifier: (charisma-11)/2 (-4 to +7) */
/* Inspires pets: add modifier to pet to-hit and damage, roll modifier and add to pet AC */
/* Pets gain the bold() property (stop fleeing faster) */
/* Applies the modifier to your save vs san loss */
/* Strongly boost fireball and firestorm success rate */
/* Also adds 1d10 fire damage to all attacks (only prints messages for unarmed attacks) */
/* Gives fire res when worn */
A("Narya",								RIN_CONFLICT,	/* ruby or gold */			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_ELF, TIER_A, (ARTG_INHER|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_FIRE, 1, 10), NOFLAG,
	PROPS(FIRE_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* Modifier: (wisdom-11)/2 (-4 to +7) */
/* Gain (or lose) modifer energy per 10 turns */
/* Applies the modifier to your base AC when not flat footed */
/* Strongly boost divination spell success rate */
/* Also adds 1d20 silver damage to all attacks (only prints messages for unarmed attacks) */
/* Grants monster detection, waterproofing, and water walking when worn */
A("Nenya",								RIN_WARNING,	/* diamond or silver */			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_ELF, TIER_A, (ARTG_INHER|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_STAR, 1, 20), NOFLAG,
	PROPS(DETECT_MONSTERS, WATERPROOF, WWALKING), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* Modifier: (intelligence-11)/2 (-4 to +7) */
/* Gain modifier HP per 10 turns. May worsen your condition if you are already dying due to negative HP regen, but won't cause HP regen to be negative otherwise. */
/* Add modifier to your San regen threshold */
/* Applies modifier to pet regen rate (may kill pets if negative!) */
/* Add modifier to your pet's AC */
/* Strongly boost healing spell success rate */
/* Also adds 1d20 study to all attacks */
/* Grants shock res, warning, stealth, and magical breathing when worn */
A("Vilya",								RIN_AGGRAVATE_MONSTER,	/* sapphire or brass */			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_ELF, TIER_A, (ARTG_INHER|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_STDY, 1, 20), NOFLAG,
	PROPS(SHOCK_RES, WARN_OF_MON, STEALTH, MAGICAL_BREATHING), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* Modifier: (wisdom-11)/2 (-4 to +7) */
/* Applies modifier to pet DR */
/* Level modifier: (wisdom + charisma - 18)/4 (-3 to +8) plus beast mastery (0 to 5, doubled by clarent) */
/* Applies level modifier to pet max level */
/* Halves pet special attack timeout */
/* Strongly boost enchantment spell success rate */
/* Also adds 1d8 turn sleep attack to all attacks (only prints messages for unarmed attacks) */
/*  Since it always has Lolth's holy symbol, also adds silver damage when used unarmed without gloves */
/* Grants teleportation, teleport control, and searching when worn */
A("Lomya",					RIN_PROTECTION_FROM_SHAPE_CHAN,/* black signet */			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DROW, TIER_A, (ARTG_INHER|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_SLEE,1,8), NOFLAG,
	PROPS(TELEPORT, TELEPORT_CONTROL, SEARCHING), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Hellrider's Saddle",				SADDLE,					(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ANTIMAGIC, REFLECTING), NOFLAG,
	PROPS(), NOFLAG,
	INVIS, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("The Wallet of Perseus",			BAG_OF_HOLDING,			(const char *)0,
	10000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*
 *	The artifacts for the quest dungeon, all self-willed.
 */

/*//////////Law Quest Artifacts//////////*/

A("The Rod of Seven Parts",			SPEAR,					(const char *)0,
	7777L, PLATINUM, MZ_DEFAULT, 25 /*wt spear*/,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_INHER),
	NO_MONS(),
	ATTK(AD_PHYS, 7, 20), (ARTA_HATES|ARTA_CROSSA),
	PROPS(DRAIN_RES), NOFLAG,
	PROPS(), NOFLAG,
	SEVENFOLD, NOFLAG
	),

A("The Field Marshal's Baton",		MACE,					(const char *)0,
	5000L, MT_DEFAULT, MZ_SMALL, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	MONS(vsMG(MG_MERC)),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	CANNONADE, NOFLAG
	),
/*//////////Chaos Quest Artifacts//////////*/

/*Needs encyc entry*/
/* single-use 1-hit-kill when thrown */
A("Houchou",						SPOON,					(const char *)0,
	50000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_NOWISH),	/* is allowed to be gifted and randomly generate */
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*does not protect against lycathropy*/
A("Werebuster",						LONG_SWORD,				(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_F, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	MONS(vsMA(MA_WERE)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Masamune",						TSURUGI,				(const char *)0,
	7500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 20), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*from Final Fantasy*/
/*The crystals are generated together.  The Black Crystal "counts", and the others don't.*/
A("The Black Crystal",				CRYSTAL_BALL,			(const char *)0,
	100L, MT_DEFAULT, MZ_DEFAULT, 30,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), (ARTA_HATES|ARTA_CROSSA),
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC, WARNING, BLACK_CRYSTAL), NOFLAG,
	SHADOW_FLARE, NOFLAG
	),
A("The Water Crystal",				CRYSTAL_BALL,			(const char *)0,
	100L, MT_DEFAULT, MZ_DEFAULT, 40,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(COLD_RES, WATER_CRYSTAL), NOFLAG,
	BLIZAGA, NOFLAG
	),
A("The Fire Crystal",				CRYSTAL_BALL,			(const char *)0,
	100L, MT_DEFAULT, MZ_DEFAULT, 10,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(FIRE_RES, FIRE_CRYSTAL), NOFLAG,
	FIRAGA, NOFLAG
	),
A("The Earth Crystal",				CRYSTAL_BALL,			(const char *)0,
	100L, MT_DEFAULT, MZ_DEFAULT, 100,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(HALF_PHDAM, EARTH_CRYSTAL), NOFLAG,
	QUAKE, NOFLAG
	),
A("The Air Crystal",				CRYSTAL_BALL,			(const char *)0,
	100L, MT_DEFAULT, MZ_DEFAULT, 20,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(SHOCK_RES, AIR_CRYSTAL), NOFLAG,
	THUNDAGA, NOFLAG
	),

/*Needs encyc entry*/
A("Nighthorn",						UNICORN_HORN,			(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_FIRE, 12, 24), (ARTA_EXPLFIREX),
	PROPS(FIRE_RES), NOFLAG,
	PROPS(), NOFLAG,
	LEVITATION, (ARTI_FEAR|ARTI_LUCK)	/* only fears vs non-chaotic */
	),

/*Needs encyc entry*/
/* blocks Aggravate monster while worn */
A("The Mantle of Wrath",			ORCISH_CLOAK,			(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ORC, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ACID_RES, HALF_SPDAM), NOFLAG,
	PROPS(FIRE_RES, AGGRAVATE_MONSTER), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("The Heart of Shadow",					BATTLE_AXE,			"black-flamed %s",
	6660L, OBSIDIAN_MT, MZ_HUGE, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_FIRE, 20, 10), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	INVOKE_DARK, NOFLAG
	),

/*Needs encyc entry*/
/*Actually an iron ring (or twisted, if iron is hunger)*/
A("The Shard from Morgoth's Crown",	RIN_ADORNMENT,			"twisted band of jagged iron",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_HLUH, 7, 9), NOFLAG,
	PROPS(SHOCK_RES), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* The Alignment Keys.  Must be grouped together.  Some code in lock.c, artifact.h, and invent.c depends on the order. */
#define ALIGNMENT_KEY(name, alignment)											\
A((name),							SKELETON_KEY,			(const char *)0,	\
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,									\
	(alignment), NON_PM, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),	\
	NO_MONS(),																	\
	NO_ATTK(), NOFLAG,															\
	PROPS(), NOFLAG,															\
	PROPS(), NOFLAG,															\
	NOINVOKE, NOFLAG															\
	)
ALIGNMENT_KEY("The First Key of Law", A_LAWFUL),
ALIGNMENT_KEY("The Second Key of Law", A_LAWFUL),
ALIGNMENT_KEY("The Third Key of Law", A_LAWFUL),
ALIGNMENT_KEY("The First Key of Chaos", A_CHAOTIC),
ALIGNMENT_KEY("The Second Key of Chaos", A_CHAOTIC),
ALIGNMENT_KEY("The Third Key of Chaos", A_CHAOTIC),
ALIGNMENT_KEY("The First Key of Neutrality", A_NEUTRAL),
ALIGNMENT_KEY("The Second Key of Neutrality", A_NEUTRAL),
ALIGNMENT_KEY("The Third Key of Neutrality", A_NEUTRAL),

#undef ALIGNMENT_KEY

/*//////////Neutral Quest Artifacts//////////*/

A("Infinity's Mirrored Arc",		DOUBLE_LIGHTSABER,		(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_INHER),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(REFLECTING), NOFLAG,
	PROPS(), NOFLAG,
	ALTMODE, NOFLAG
	),

A("The Staff of Twelve Mirrors",	KHAKKHARA,				(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 6), NOFLAG,
	PROPS(REFLECTING, DISPLACED), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Mirrored Mask",				MASK,					(const char *)0,
	3000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(REFLECTING, HALF_SPDAM), NOFLAG,
	PROPS(), NOFLAG,
	CAPTURE_REFLECTION, NOFLAG
	),

/* reflects projectiles and counterattacks, and doubles your multishot when carried in swapwep or wielded */
A("The Sansara Mirror",				MIRRORBLADE,			(const char *)0,
	3000L, GOLD, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 8, 8), NOFLAG,
	PROPS(REFLECTING, HALF_PHDAM), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Spell-warded Wrappings of Nitocris",		MUMMY_WRAPPING,			(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(NULLMAGIC), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*from the works of HP Lovecraft*/
A("The Hand-Mirror of Cthylla",		MIRROR,					(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC, SEARCHING, POLYMORPH_CONTROL, TELEPORT_CONTROL), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*from the works of HP Lovecraft*/
A("The Silver Key",					UNIVERSAL_KEY,			"strangely figured %s",
	5000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(ENERGY_REGENERATION, POLYMORPH_CONTROL, TELEPORT_CONTROL), NOFLAG,
	CREATE_PORTAL, NOFLAG
	),

/*//////////Artifact Books///////////*/
#define ARTIFACT_BOOK(name, invoke)												\
A((name),							SPE_SECRETS,			(const char *)0,	\
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,									\
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),		\
	NO_MONS(),																		\
	NO_ATTK(), NOFLAG,																\
	PROPS(), NOFLAG,															\
	PROPS(), NOFLAG,															\
	(invoke), NOFLAG															\
	)
/*from the works of HP Lovecraft*/
ARTIFACT_BOOK("The Necronomicon", NECRONOMICON),
/*Needs encyc entry*/
ARTIFACT_BOOK("The Book of Lost Names", SPIRITNAMES),
/*Needs encyc entry*/
ARTIFACT_BOOK("The Book of Infinite Spells", INFINITESPELLS),
#undef ARTIFACT_BOOK

/*//////////Role-specific Crowning Artifacts//////////*/

/* Clarent patch (Greyknight): Note that Clarent's SPFX2_DIG gives it another +2 to hit against thick-skinned
	monsters, as well as against wall-passers. Another special effect of Clarent
	is that it can be pulled out of a wall it is stuck in (by #untrapping towards
	it) if you are devoutly lawful. */
	/*Clarent has been modified to make it the Knight crowning-gift*/
A("Clarent",						LONG_SWORD,				(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_KNIGHT, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_INHER|ARTG_FXALGN),
	MONS(vsMB(MB_THICK_HIDE)),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	LEADERSHIP, (ARTI_DIG)
	),

/*Needs encyc entry*/
A("Reaver",							SCIMITAR,				(const char *)0,
	6000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_PIRATE, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 8), (ARTA_STEAL),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	THEFT_TYPE, NOFLAG
	),

/*Needs encyc entry*/
/* can be read to learn cone of cold */
A("The Bow of Skadi",				BOW,			"rune-carved %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_VALKYRIE, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_COLD, 1, 24), (ARTA_EXPLCOLDX),
	PROPS(), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Gungnir",			ATGEIR,						"rune-carved %s",
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_VALKYRIE, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 12), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* actually a gold circlet */
/*Also causes pets to always follow you when worn*/
A("The Crown of the Saint King",	HELMET,					(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	LEADERSHIP, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
/* actually a visored helmet */
/*Also causes pets to always follow you when worn*/
A("The Helm of the Dark Lord",		HELMET,					(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	LEADERSHIP, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
/* permanently filthed */
A("Sunbeam",						GOLDEN_ARROW,			(const char *)0,
	1000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_RANGER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 0), (ARTA_POIS|ARTA_BRIGHT),
	PROPS(), NOFLAG,
	PROPS(DRAIN_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*Needs encyc entry*/
/* permanently drugged */
A("Moonbeam",						SILVER_ARROW,			(const char *)0,
	1000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_RANGER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(DRAIN_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Veil of Latona",				CLOAK_OF_INVISIBILITY,	(const char *)0,
	1000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_RANGER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ANTIMAGIC, REFLECTING), NOFLAG,
	PROPS(DRAIN_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Hermes's Sandals",				FLYING_BOOTS,			"pair of winged sandals",
	4000L, GOLD, MZ_DEFAULT, WT_SPECIAL,
	A_NEUTRAL, PM_HEALER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_FXALGN),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(FAST), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

A("Poseidon's Trident",				TRIDENT,				(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_HEALER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	WATER, NOFLAG
	),

A("Feng Huo Lun",				WIND_AND_FIRE_WHEELS,				"flaming %s",
	4500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_MONK, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_FIRE, 1, 0), NOFLAG,
	PROPS(FAST, FLYING), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Jin Gang Zuo",				BANDS,				"hoop",
	4500L, GEMSTONE, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_MONK, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	NO_ATTK(), ARTA_RETURNING,
	PROPS(), NOFLAG,
	PROPS(FIRE_RES, WATERPROOF), NOFLAG,
	SNARE_WEAPONS, NOFLAG
	),

A("Ruyi Jingu Bang",				QUARTERSTAFF,				"golden-hooped %s",
	4500L, IRON, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_MONK, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), ARTA_RETURNING,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	CHANGE_SIZE, NOFLAG
	),

A("The Sickle of Thunderblasts",				SICKLE,				(const char *)0,
	4500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_MADMAN, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), ARTA_SONICX|ARTA_RETURNING,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The War-helm of the Dreaming",		FACELESS_HELM,		"bird-like faceless helm",
	4500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_MADMAN, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(MAGICAL_BREATHING, WALKING_NIGHTMARE), NOFLAG,
	PROPS(), NOFLAG,
	ENERGY_BOOST, NOFLAG
	),

A("The Spear of Peace",				SPEAR,				(const char *)0,
	1500L, WOOD, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_MADMAN, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(CLEAR_THOUGHTS, BLOCK_CONFUSION, HALLUC_RES, HALF_PHDAM, HALF_SPDAM), NOFLAG,
	PROPS(), NOFLAG,
	INVULNERABILITY, NOFLAG
	),

A("The Ibite arm",				CLUB,				"flabby green arm",
	4500L, FLESH, MZ_GIGANTIC, WT_DEFAULT,
	A_NONE, PM_MADMAN, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_ACID, 20, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	IBITE_ARM, NOFLAG
	),

A("Star-emperor's Ring",				RIN_WISHES,				(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_MADMAN, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_STAR, 20, 20), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	IMPERIAL_RING, NOFLAG
	),

A("The Eye of the Oracle",			EYEBALL,				(const char *)0,
	500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC, HALF_SPDAM), NOFLAG,
	ORACLE, NOFLAG
	),

/*Arc redesign by Riker*/
A("The Macuahuitl of Quetzalcoatl",	MACUAHUITL,				(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), (ARTA_VORPAL),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*Arc redesign by Riker*/
A("The Mask of Tlaloc",				MASK,					(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(WATERPROOF), NOFLAG,
	PROPS(HALF_SPDAM, ANTIMAGIC, COLD_RES, SHOCK_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*Arc redesign by Riker*/
A("Ehecailacocozcatl",				AMULET_OF_MAGICAL_BREATHING,			(const char *)0,
	3000L, SHELL_MAT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_FXALGN),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ENERGY_REGENERATION, REFLECTING), NOFLAG,
	PROPS(), NOFLAG,
	WIND_PETS, NOFLAG
	),
/*Arc redesign by Riker*/
A("Amhimitl",						JAVELIN,				(const char *)0,
	3000L, BONE, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_ARCHEOLOGIST, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_FIRE, 5, 8), ARTA_RETURNING,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*Arc redesign by Riker*/
/* can "sacrifice" monsters it kills, reducing prayer timeout */
A("The Tecpatl of Huhetotl",		TECPATL,				(const char *)0,
	3000L, BONE, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_ARCHEOLOGIST, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 8), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(DRAIN_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*//////////Role-specific Quest Artifacts//////////*/
/*
// NOTE: uses old artifact structure
A("The Orb of Detection",	CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_ESP|SPFX_HSPDAM), 0,
	NO_ATTK(),	NO_DFNS,	CARY(AD_MAGM),
	INVIS,		A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, 2500L ),
*/

/*From archeologist patch*/
A("Itlachiayaque",					ROUNDSHIELD,		(const char *)0,
	3000L, OBSIDIAN_MT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(FIRE_RES, REFLECTING), NOFLAG,
	PROPS(TELEPAT, HALF_SPDAM, ANTIMAGIC), NOFLAG,
	SMOKE_CLOUD, NOFLAG
	),

/*Needs encyc entry*/
A("The Annulus",					CHAKRAM,			"intricately-featured %s",
	3000L, SILVER, MZ_DEFAULT, WT_SPECIAL,
	A_CHAOTIC, PM_ANACHRONONAUT, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 1), ARTA_RETURNING,	/*Actually Phys(5,0) if not a lightsaber*/
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC, HALF_SPDAM), NOFLAG,
	ANNUL, NOFLAG
	),

A("The Heart of Ahriman",			RUBY,				(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_BARBARIAN, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC, REFLECTING, HALF_SPDAM, FIRE_RES, DRAIN_RES, POISON_RES), NOFLAG,
	ARTI_REMOVE_CURSE, NOFLAG
	),

#ifdef BARD
A("The Lyre of Orpheus",			MAGIC_HARP,			(const char *)0,
	5000L, GOLD, MZ_DEFAULT, WT_SPECIAL,
	A_NEUTRAL, PM_BARD, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	TAMING, (ARTI_SPEAK)
	),
#endif

A("The Sceptre of Might",			MACE,				(const char *)0,
	2500L, BONE, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_CAVEMAN, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), (ARTA_HATES|ARTA_CROSSA|ARTA_KNOCKBACK),
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	CONFLICT, NOFLAG
	),
// old-style artifact block
// A("The Iron Ball of Liberation", HEAVY_IRON_BALL,	0,			0,
	// (SPFX_NOGEN|SPFX_RESTR|SPFX_LUCK|SPFX_INTEL),
		// (SPFX_SEARCH|SPFX_SEEK|SPFX_WARN), 0,
	// NO_ATTK(),	NO_DFNS,	CARY(AD_MAGM),
	// PHASING,	A_CHAOTIC, PM_CONVICT, NON_PM, 5000L,
	// SPFX2_STLTH,0,0), /*Note: it had caried stealth before*/
	
A("The Iron Ball of Levitation",	HEAVY_IRON_BALL,	(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_CONVICT, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 10), (ARTA_HATES|ARTA_CROSSA),
	PROPS(STEALTH), NOFLAG,
	PROPS(WARNING, DRAIN_RES), NOFLAG,
	LEVITATION, (ARTI_LUCK)
	),
A("The Iron Spoon of Liberation",	SPOON,				(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_CONVICT, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROPS(STEALTH), NOFLAG,
	PROPS(SEARCHING, FREE_ACTION), (ARTP_SEEK),
	PHASING, (ARTI_DIG|ARTI_ENGRAVE|ARTI_LUCK)
	),

/*Creates throwing stars. Makes throwing stars count as silver if wielded */
/*Also can be (a)pplied as a magic flute.								  */
/* needs encyc entry */
A("Silver Starlight",				RAPIER,				(const char *)0,
	5000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 4), (ARTA_SHINING),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/* needs encyc entry */
/* shoots many crossbow bolts at a time (with reduced precision damage) */
A("Wrathful Spider",				DROVEN_CROSSBOW,	(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(STEALTH), NOFLAG,
	PROPS(), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/* needs encyc entry */
/* hits many times in one swing, with special effects */
/* helps spellcasting */
/* protects vs curses while wielded */
A("The Tentacle Rod",				FLAIL,				(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 7, 1), (ARTA_TENTROD),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* needs encyc entry */
A("The Crescent Blade",				SABER,				(const char *)0,
	5000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_FIRE, 4, 0), (ARTA_SHINING|ARTA_VORPAL),
	PROPS(), NOFLAG,
	PROPS(REFLECTING), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* needs encyc entry */
A("The Darkweaver's Cloak",			DROVEN_CLOAK,		(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/* needs encyc entry */
/*Adds sleep poison to unarmed attacks*/
A("Spidersilk",						DROVEN_CHAIN_MAIL,	(const char *)0,
	5000L, CLOTH, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(SPELLBOOST), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/* needs encyc entry */
A("The Webweaver's Crook",			FAUCHARD,			(const char *)0,
	5000L, CHITIN, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), (ARTA_POIS),
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* needs encyc entry */
/* Hedrow crowning gift, chaotic or neutral */
A("Lolth's Fang",					DROVEN_SHORT_SWORD,	"silver-edged %s",
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_ACID, 10, 10), (ARTA_POIS|ARTA_SILVER),
	PROPS(DRAIN_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* needs encyc entry */
/* Drow crowning gift, chaotic */
A("The Web of Lolth",				ELVEN_MITHRIL_COAT,	"silver-starred %s",
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	MONS(vsMA(MA_ELF)),
	NO_ATTK(), (ARTA_SILVER),
	PROPS(WARN_OF_MON, DRAIN_RES, SPELLBOOST), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	ENERGY_BOOST, (ARTI_PLUSSEV)
	),

/* needs encyc entry */
/* Drow crowning gift, neutral */
A("The Claws of the Revenancer",	GAUNTLETS_OF_DEXTERITY,			(const char *)0,
	8000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, PM_DROW, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_DRLI, 1, 1), (ARTA_DRAIN),
	PROPS(ENERGY_REGENERATION, DRAIN_RES, COLD_RES, REGENERATION, FIXED_ABIL, POISON_RES, SEE_INVIS), NOFLAG,
	PROPS(), NOFLAG,
	RAISE_UNDEAD, NOFLAG
	),

/* needs encyc entry */
/* Drow noble crowning gift, lawful */
A("Liecleaver",						DROVEN_CROSSBOW,	(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_DROW, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROPS(SEARCHING, DRAIN_RES), (ARTP_SEEK),
	PROPS(HALLUC_RES), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/* needs encyc entry */
/* Hedrow noble crowning gift, Chaotic */
A("The Ruinous Descent of Stars",	MORNING_STAR,		"silver-spiked %s",
	8000L, METAL, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), (ARTA_SILVER),
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	FALLING_STARS, NOFLAG
	),

/* needs encyc entry */
/* Drow noble crowning gift, Lawful */
/*returns to your hand when thrown; +1 multishot for drow & elves */
A("Sickle Moon",					ELVEN_SICKLE,				(const char *)0,
	4000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOWISH|ARTG_FXALGN),	/* may randomly generate */
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), ARTA_RETURNING,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/** Indwelling of the Black Web entity, lawful hedrow crowning gift */

/* needs encyc entry */
/* Elf crowning gift, Lawful */
A("Arcor Kerym",					CRYSTAL_SWORD,			"gold-runed craggy and pitted %s",
	8000L, IRON, MZ_DEFAULT, 40,
	A_LAWFUL, NON_PM, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 10), NOFLAG,
	PROPS(DRAIN_RES), NOFLAG,
	PROPS(), NOFLAG,
	HEALING, (ARTI_LIGHT)
	),

/* needs encyc entry */
/* Elf crowning gift, Neutral */
A("Aryfaern Kerym",					RUNESWORD,			"sword of crimson-edged darkness",
	8000L, MINERAL, MZ_DEFAULT, 4,
	A_NEUTRAL, NON_PM, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN|ARTA_SHINING),
	NO_MONS(),
	ATTK(AD_ELEC, 5, 10), NOFLAG,
	PROPS(SHOCK_RES, SPELLBOOST), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_LIGHT)
	),

/* needs encyc entry */
/* Elf crowning gift, Chaotic */
A("Aryvelahr Kerym",				ELVEN_BROADSWORD,		"blue-burning %s",
	8000L, SILVER, MZ_DEFAULT, 20,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROPS(REFLECTING, DRAIN_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_LIGHT)
	),

A("The Staff of Aesculapius",		QUARTERSTAFF,		"serpent-entwined staff",
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_HEALER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_DRLI, 1, 0), (ARTA_SILVER|ARTA_DRAIN), /*silver is associated with the healer's art*/
	PROPS(REGENERATION, DRAIN_RES), NOFLAG,
	PROPS(), NOFLAG,
	HEALING, NOFLAG
	),

/* only gives double spell damage for knights */
A("The Magic Mirror of Merlin",		MIRROR,				(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_KNIGHT, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC, TELEPAT), NOFLAG,
	NOINVOKE, (ARTI_SPEAK)
	),

A("The Eyes of the Overworld",		LENSES,				(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_MONK, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(BLIND_RES, XRAY_VISION, GAZE_RES), (ARTP_FORCESIGHT),
	PROPS(ANTIMAGIC), NOFLAG,
	ENLIGHTENING, NOFLAG
	),

A("Rite of Detestation",		SCR_BLANK_PAPER,	"water-damaged scroll",
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_MADMAN, NON_PM, TIER_F, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	DETESTATION, NOFLAG
	),

A("Idol of Bokrug, the water-lizard",		STATUE,	"sea-green stone likeness of a water-lizard",
	2500L, MINERAL, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_MADMAN, NON_PM, TIER_F, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Star of Hypernotus",		AMULET_VERSUS_CURSES,	"blue-green star-shaped %s",
	2500L, GEMSTONE, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_MADMAN, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(CLEAR_THOUGHTS, BLOCK_CONFUSION), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* Also has holy and unholy properties */
A("Avenger",						LONG_SWORD,			"winged long sword",
	1000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 7), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(DRAIN_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*Actually an ornamental cope in game*/
/* Plus double AC bonus */
A("The Mantle of Heaven",			CLOAK,				(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(SHOCK_RES, HALF_SPDAM), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*Needs encyc entry*/
/*Actually an opera cope in game*/
/* Plus double AC bonus */
A("The Vestment of Hell",			CLOAK,				(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_NOBLEMAN, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ACID_RES, HALF_PHDAM), NOFLAG,
	PROPS(FIRE_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*Moria dwarf noble first gift */
A("The Armor of Khazad-dum",		DWARVISH_MITHRIL_COAT,			(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, PM_DWARF, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
/*Moria dwarf noble */
/*+5 attk and damage with axes*/
A("The War-mask of Durin",			MASK,				(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, PM_DWARF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(FIRE_RES, ACID_RES, POISON_RES, HALF_SPDAM), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*Moria dwarf noble crowning gift */
A("Durin's Axe",					AXE,				"silver-rune-inlaid %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, PM_DWARF, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 10), (ARTA_SILVER),
	PROPS(DRAIN_RES, SEARCHING), (ARTP_SEEK),
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_DIG)
	),

/*Needs encyc entry*/
/*Lonely Mountain dwarf noble first gift */
/* the old elves fought balrogs too. */
A("Glamdring",						ELVEN_BROADSWORD,	(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_KNIGHT, PM_DWARF, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	MONS(vsMA(MA_ORC|MA_DEMON)),
	ATTK(AD_PHYS, 10, 10), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Key of Erebor",				SKELETON_KEY,		(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_F, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*Lonely Mountain dwarf noble */
/* has bonus +10 AC */
A("The Armor of Erebor",			PLATE_MAIL,			(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_DWARF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(HALF_PHDAM, ANTIMAGIC, FIRE_RES, COLD_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*Arkenstone*/ /*Lonely Mountain dwarf noble crown*/

/*Needs encyc entry*/
/* Drow noble first gift  */
A("The Sceptre of Lolth",			KHAKKHARA,			(const char *)0,
	4000L, SILVER, MZ_DEFAULT, WT_SPECIAL,
	A_CHAOTIC, PM_NOBLEMAN, PM_DROW, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	LORDLY, (ARTI_ENGRAVE)
	),

/*Needs encyc entry*/
/* Drow noble quest */
/* Plus double AC bonus */
A("The Web of the Chosen",			DROVEN_CLOAK,		"silver-dewed %s",
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_NOBLEMAN, PM_DROW, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), (ARTA_SILVER),
	PROPS(REFLECTING, HALF_SPDAM, ACID_RES), NOFLAG,
	PROPS(SHOCK_RES), NOFLAG,
	NOINVOKE, (ARTI_ENGRAVE)
	),

/*Needs encyc entry*/
/* Hedrow noble first gift */
A("The Death-Spear of Keptolo",		DROVEN_SPEAR,		(const char *)0,
	4000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_NOBLEMAN, PM_DROW, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 12), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(), NOFLAG,
	DEATH_TCH, (ARTI_ENGRAVE)
	),

/*Needs encyc entry*/
/* Herow noble quest */
/* Plus double AC bonus */
A("The Cloak of the Consort",		DROVEN_CLOAK,		(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_NOBLEMAN, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(HALF_PHDAM, DRAIN_RES), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Esscooahlipboourrr",			DOUBLE_SWORD,					"tentacle-auraed shackle-entwined %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_HEALER, PM_DROW, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 10), NOFLAG,
	PROPS(DRAIN_RES, SEARCHING), (ARTP_SEEK),
	PROPS(), NOFLAG,
	LOOT_SELF, NOFLAG
	),

A("The Robe of Closed Eyes",				ROBE,			"shut-eye-patterned %s",
	4000L, LEATHER, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_HEALER, PM_DROW, TIER_C, (ARTG_NOGEN),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(INVIS, GAZE_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("The Red Cords of Ilmater",		HAND_WRAPS,	"pair of red cords",
	1000L, CLOTH, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(FREE_ACTION, STONE_RES), NOFLAG,
	PROPS(DRAIN_RES), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("The Crown of the Percipient",				HELM_OF_BRILLIANCE,	(const char *)0,
	1000L, FLESH, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_HEALER, PM_DROW, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(DETECT_MONSTERS, DRAIN_RES, STONE_RES, FIRE_RES, COLD_RES, SHOCK_RES, HALLUC_RES, SLEEP_RES, BLOCK_CONFUSION), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	ENLIGHTENING, NOFLAG
	),

/*Needs encyc entry*/
A("Painting Fragment",	SCR_BLANK_PAPER,				"scroll fragment",
	200L, CLOTH, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_NOBLEMAN, PM_HALF_DRAGON, TIER_F, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	QUEST_PORTAL, NOFLAG
	),

/*Needs encyc entry*/
A("The Profaned Greatscythe",		SCYTHE,				(const char *)0,
	4000L, MINERAL, MZ_HUGE, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_FIRE, 8, 30), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(FIRE_RES), NOFLAG,
	NOINVOKE, (ARTI_PLUSTEN)
	),

/*Needs encyc entry*/
/* slowing cold magic offense */
/* scales with STR/2 & DEX & INT for +20 max*/
A("Friede's Scythe",				SCYTHE,				(const char *)0,
	4000L, METAL, MZ_SMALL, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_MAGM, 6, 6), (ARTA_MAGIC|ARTA_SHINING),
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	NOINVOKE, (ARTI_PLUSTEN)
	),

/*Needs encyc entry*/
/* Drains energy/sets special ability cooldowns */
/* scales with STR & DEX & WIS for +24 max*/
A("Yorshka's Spear",				SPEAR,				(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), (NOFLAG),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSTEN)
	),

/*Needs encyc entry*/
A("Dragonhead Shield",		STONE_DRAGON_SHIELD,					"stone dragon head",
	4000L, MINERAL, MZ_HUGE, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(COLD_RES, DISINT_RES, STONE_RES), NOFLAG,
	PROPS(), NOFLAG,
	STONE_DRAGON, (ARTI_PLUSTEN)
	),

/*Needs encyc entry*/
A("Crucifix of the Mad King",		HALBERD,					(const char *)0,
	4000L, METAL, MZ_LARGE, WT_DEFAULT,
	A_NEUTRAL, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_DARK, 1, 10), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	MAD_KING, (ARTI_PLUSTEN)
	),

/*Needs encyc entry*/
A("Ringed Brass Armor",		PLATE_MAIL,					"molten-ringed brass %s",
	4000L, COPPER, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	RINGED_ARMOR, (ARTI_PLUSTEN)
	),

/*Needs encyc entry*/
A("Ritual Ringed Spear",		SPEAR,					"molten-ringed spear",
	4000L, OBSIDIAN_MT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	RINGED_SPEAR, (ARTI_PLUSTEN)
	),

/*Needs encyc entry*/
A("Velka's Rapier",		RAPIER,					(const char *)0,
	4000L, METAL, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_MAGM, 20, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_PLUSTEN)
	),

/*Needs encyc entry*/
/* Doubles dragon attributes when carried */
/* TODO: Invoke to change element? */
A("The Dragon's Heart-Stone",		FLINT,				(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), (NOFLAG),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Mitre of Holiness",			HELM_OF_BRILLIANCE,	(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_PRIEST, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	MONS(vsMA(MA_UNDEAD)),
	NO_ATTK(), NOFLAG,
	PROPS(WARN_OF_MON, DRAIN_RES), NOFLAG,
	PROPS(FIRE_RES), NOFLAG,
	ENERGY_BOOST, (ARTI_PLUSSEV)
	),

/* protects inventory from curses */
/* polymorphs contained items occasionally */
A("The Treasury of Proteus",		CHEST,				(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, 50,
	A_CHAOTIC, PM_PIRATE, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	ENERGY_BOOST, (ARTI_LUCK)
	),

A("The Longbow of Diana",			BOW,				(const char *)0,
	4000L, SILVER, MZ_DEFAULT, 0,
	A_CHAOTIC, PM_RANGER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 6), NOFLAG,
	PROPS(REFLECTING), NOFLAG,
	PROPS(TELEPAT), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

A("The Rogue Gear-spirits",			CROSSBOW,			(const char *)0,
	4000L, MT_DEFAULT, MZ_SMALL, WT_DEFAULT,
	A_NEUTRAL, PM_RANGER, PM_GNOME, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROPS(SEARCHING), (ARTP_SEEK),
	PROPS(FIRE_RES, WARNING, TELEPAT), NOFLAG,
	UNTRAP, (ARTI_DIG|ARTI_SPEAK)
	),

/*Needs encyc entry*/
/* yes, causes spell penalties*/
A("The Steel Scales of Kurtulmak",	GRAY_DRAGON_SCALES,	(const char *)0,
	5000L, IRON, MZ_DEFAULT, 300,
	A_LAWFUL, NON_PM, PM_KOBOLD, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(FREE_ACTION, FIRE_RES, REFLECTING, HALF_PHDAM), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Glitterstone",				AMBER,				(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, PM_GNOME, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(ENERGY_REGENERATION), NOFLAG,
	CHARGE_OBJ, (ARTI_PERMALIGHT)
	),

A("Great Claws of Urdlen",			GAUNTLETS_OF_POWER,	"clawed %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_FXALGN),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	QUAKE, (ARTI_PLUSSEV|ARTI_DIG)
	),
	
/*Needs encyc entry*/
A("The Moonbow of Sehanine",		ELVEN_BOW,			(const char *)0,
	4000L, SILVER, MZ_DEFAULT, 0,
	A_CHAOTIC, PM_RANGER, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 6), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/*Needs encyc entry*/
A("The Spellsword of Corellon",		HIGH_ELVEN_WARSWORD,			(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 10), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Warhammer of Vandria",		WAR_HAMMER,			(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_ELF, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),


/*Needs encyc entry*/
A("The Shield of Saint Cuthbert",	SHIELD_OF_REFLECTION,(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(HALF_SPDAM, HALF_PHDAM), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Palantir of Westernesse",	CRYSTAL_BALL,		(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(WARNING, TELEPAT, REFLECTING, XRAY_VISION), NOFLAG,
	TAMING, NOFLAG
	),

/*Needs encyc entry*/
A("Belthronding",					ELVEN_BOW,			(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 6), NOFLAG,
	PROPS(STEALTH), NOFLAG,
	PROPS(DISPLACED), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/*Needs encyc entry*/
/* Elf noble first gift */
A("The Rod of the Elvish Lords",	ELVEN_MACE,			(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_SPECIAL,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	LORDLY, (ARTI_ENGRAVE)
	),

A("The Master Key of Thievery",		SKELETON_KEY,		(const char *)0,
	3500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_ROGUE, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(HALF_PHDAM, TELEPORT_CONTROL, WARNING), NOFLAG,
	UNTRAP, (ARTI_SPEAK)
	),

A("The Tsurugi of Muramasa",		TSURUGI,			(const char *)0,
	4500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_SAMURAI, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 2, 0), (ARTA_VORPAL|ARTA_SHATTER),
	PROPS(), (ARTP_SEEK),
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_LUCK|ARTI_BLOODTHRST)
	),

#ifdef TOURIST
A("The Platinum Yendorian Express Card",		CREDIT_CARD,			(const char *)0,
	7000L, MT_DEFAULT, MZ_DEFAULT, PLATINUM,
	A_NEUTRAL, PM_TOURIST, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), (ARTA_SILVER),
	PROPS(), NOFLAG,
	PROPS(HALF_SPDAM, TELEPAT, ANTIMAGIC), NOFLAG,
	CHARGE_OBJ, NOFLAG
	),
#endif

A("The Orb of Fate",				CRYSTAL_BALL,		(const char *)0,
	3500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_VALKYRIE, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), (NOFLAG),
	PROPS(), NOFLAG,
	PROPS(HALF_SPDAM, HALF_PHDAM, WARNING), NOFLAG,
	LEV_TELE, (ARTI_LUCK)
	),

/*Needs encyc entry*/
/* The Sun of the Gods of the Dead */
A("Sol Valtiva",					TWO_HANDED_SWORD,	(const char *)0,
	4000L, OBSIDIAN_MT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_FIRE_GIANT, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_FIRE, 5, 24), (ARTA_EXPLFIRE|ARTA_BLIND|ARTA_BRIGHT),
	PROPS(), NOFLAG,
	PROPS(FIRE_RES), NOFLAG,
	NOINVOKE, (ARTI_LIGHT)
	),

A("The Eye of the Aethiopica",		AMULET_OF_ESP,		(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_WIZARD, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), (NOFLAG),
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC, HALF_SPDAM, ENERGY_REGENERATION), NOFLAG,
	CREATE_PORTAL, NOFLAG
	),

/*//////////Special High-Level Artifacts//////////*/

A("The Hat of the Archmagi",		CORNUTHAUM,			(const char *)0,
	9000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_WIZARD, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), (NOFLAG),
	PROPS(XRAY_VISION, WARNING), NOFLAG,
	PROPS(), NOFLAG,
	ENERGY_BOOST, (ARTI_SPEAK)
	),

/* 22+ Daimyo can name Kusanagi no Tsurugi from a longsword 
 * (should be a broadsword maybe, but that has been "translated" as a ninja-to).
 * only a level 30 (Shogun) samurai or one who is carying the amulet can wield the sword.
 */
A("The Kusanagi no Tsurugi",		LONG_SWORD,			(const char *)0,
	4500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_SAMURAI, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 12), (ARTA_VORPAL),
	PROPS(ENERGY_REGENERATION, SEARCHING), NOFLAG,
	PROPS(), NOFLAG,
	ENERGY_BOOST, (ARTI_LUCK)
	),
/*
 *	Ah, never shall I forget the cry,
 *		or the shriek that shrieked he,
 *	As I gnashed my teeth, and from my sheath
 *		I drew my Snickersnee!
 *			--Koko, Lord high executioner of Titipu
 *			  (From Sir W.S. Gilbert's "The Mikado")
 */

/* currently nameable by advanced tourists */
/* Tourists and Samurai can twoweapon Snickersnee */
A("Snickersnee",					KNIFE,				(const char *)0,
	1200L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_SAMURAI, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 10), (ARTA_DEXPL|ARTA_VORPAL),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

  /*/////////////Special Monster Artifacts//////////////*/
 /*//Few of these count, since they are boss treasure//*/
/*////////////////////////////////////////////////////*/

/*Needs encyc entry*/
/*heavier than normal, and causes spell penalties*/
A("The Platinum Dragon Plate",		SILVER_DRAGON_SCALE_MAIL,			(const char *)0,
	9000L, MT_DEFAULT, MZ_DEFAULT, WT_SPECIAL,
	A_NONE, NON_PM, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ANTIMAGIC, FIRE_RES, COLD_RES, DISINT_RES, SHOCK_RES, SLEEP_RES, FREE_ACTION), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*heavier than normal */
A("The Chromatic Dragon Scales",	BLACK_DRAGON_SCALES,			(const char *)0,
	9000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(FIRE_RES, COLD_RES, DISINT_RES, SHOCK_RES, POISON_RES, SICK_RES, ACID_RES, STONE_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Eye of Vecna",				EYEBALL,			(const char *)0,
	500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(HALF_SPDAM), NOFLAG,
	PROPS(), NOFLAG,
	DEATH_GAZE, NOFLAG
	),
A("The Hand of Vecna",				SEVERED_HAND,		(const char *)0,
	700L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(DRAIN_RES), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	SUMMON_UNDEAD, NOFLAG
	),

/*Needs encyc entry, somehow*/
A("Genocide",						TWO_HANDED_SWORD,	(const char *)0,
	9999L, GREEN_STEEL, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_FIRE, 9, 9), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_BLOODTHRST)
	),

/*Needs encyc entry */
A("The Rod of Dis",					MACE,				(const char *)0,
	9999L, GREEN_STEEL, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 8), (ARTA_KNOCKBACK),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	TAMING, NOFLAG
	),

/*Needs encyc entry */
A("Avarice",						SHORT_SWORD,		(const char *)0,
	9999L, GOLD, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 1), (ARTA_STEAL),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	THEFT_TYPE, NOFLAG
	),

/*Needs encyc entry */
A("The Fire of Heaven",				TRIDENT,			(const char *)0,
	9999L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_FIRE, 1, 0), (ARTA_EXPLFIREX|ARTA_EXPLELEC),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
/*used to have DRAIN_MEMORIES instead of CONFLICT */ 
A("The Diadem of Amnesia",			DUNCE_CAP,			(const char *)0,
	9999L, MT_DEFAULT, MZ_HUGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_F, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	CONFLICT, NOFLAG
	),

/*Needs encyc entry */
/* has a unique interaction with Nudziarth */
A("Shadowlock",						RAPIER,				(const char *)0,
	9999L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 0), (ARTA_VORPAL|ARTA_SHINING),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("Thunder's Voice",				DAGGER,				(const char *)0,
	3333L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_ELEC, 6, 6), (ARTA_EXPLELEC),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("Serpent's Tooth",				ATHAME,				(const char *)0,
	3333L, GREEN_STEEL, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), (ARTA_POIS),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("Unblemished Soul",				UNICORN_HORN,		"silver-spattered %s",
	3333L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), (ARTA_SILVER),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_LUCK)
	),

/*Needs encyc entry */
A("Ramithaine",						LONG_SWORD,			(const char *)0,
	3333L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 6, 1), (ARTA_VORPAL),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("The Wrath of Heaven",			LONG_SWORD,			(const char *)0,
	9999L, SILVER, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_ELEC, 1, 0), (ARTA_EXPLFIRE|ARTA_EXPLELECX),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
/* Used to have SLAY_LIVING */
A("The All-seeing Eye of the Fly",	HELM_OF_TELEPATHY,	(const char *)0,
	9999L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(DETECT_MONSTERS), NOFLAG,
	PROPS(), NOFLAG,
	SHADOW_FLARE, NOFLAG
	),

/*Needs encyc entry */
A("Cold Soul",						RANSEUR,			(const char *)0,
	9999L, GREEN_STEEL, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), (ARTA_EXPLFIRE|ARTA_EXPLELEC|ARTA_EXPLCOLD),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("The Sceptre of the Frozen Floor of Hell",	QUARTERSTAFF,			(const char *)0,
	9999L, METAL, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_COLD, 1, 0), (ARTA_EXPLCOLDX),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	BLIZAGA, NOFLAG
	),

/*Needs encyc entry */
A("Caress",							BULLWHIP,			(const char *)0,
	9999L, GOLD, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_ELEC, 1, 20), NOFLAG,
	PROPS(SHOCK_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
/*Weapon of Lixer, Prince of Hell, from Dicefreaks the Gates of Hell*/
/*also does +9 damage to S_ANGELs*/
A("The Iconoclast",					SABER,				(const char *)0,
	9999L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	MONS(vsMA(MA_HUMAN|MA_ELF|MA_DWARF|MA_GNOME)),
	ATTK(AD_PHYS, 9, 99), (ARTA_HATES),
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("The Three-Headed Flail",			FLAIL,				"three-headed %s",
	6660L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), (ARTA_THREEHEAD),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("Heartcleaver",					POLEAXE,			(const char *)0,
	6660L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("Wrathful Wind",					CLUB,				"cloud-inlaid %s",
	6660L, MT_DEFAULT, MZ_HUGE, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_COLD, 10, 0), (ARTA_SILVER|ARTA_EXPLCOLD),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("Thorns",	BULLWHIP,				"thorned whip",
	6660L, VEGGY, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_ELEC, 1, 10), (ARTA_POIS),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("The Sting of the Poison Queen",	FLAIL,				(const char *)0,
	6660L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 12), (ARTA_POIS),
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
/* Hates elves, but not drow */
A("The Scourge of Lolth",			VIPERWHIP,			(const char *)0,
	6660L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	MONS(vsMA(MA_ELF)),
	ATTK(AD_PHYS, 1, 0), (ARTA_HATES|ARTA_DEXPL),
	PROPS(), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
/*Weapon of Graz'zt, from Gord the Rogue*/
A("Doomscreamer",					TWO_HANDED_SWORD,	(const char *)0,
	6660L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_ACID, 1, 0), NOFLAG,
	PROPS(ACID_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("The Wand of Orcus",				WAN_DEATH,			(const char *)0,
	8000L, MT_DEFAULT, MZ_HUGE, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_DRLI, 20, 12), (ARTA_DRAIN),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_BLOODTHRST)
	),

/* Weapons of the Archons. Need encyc entries. */
#define ANGELIC_WEAPON(name, type)														\
A((name),							(type),				(const char *)0,				\
	7777L, SILVER, MZ_DEFAULT, WT_DEFAULT,												\
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),	\
	NO_MONS(),																			\
	ATTK(AD_PHYS, 7, 10), (ARTA_BLIND|ARTA_BRIGHT),										\
	PROPS(BLIND_RES, SEARCHING), NOFLAG,												\
	PROPS(), NOFLAG,																	\
	NOINVOKE, (ARTI_LIGHT)																\
	)
ANGELIC_WEAPON("The Sword of Erathaol",		LONG_SWORD),
ANGELIC_WEAPON("The Saber of Sabaoth",		SABER),
ANGELIC_WEAPON("The Sword of Onoel",		TWO_HANDED_SWORD),
ANGELIC_WEAPON("The Glaive of Shamsiel",	GLAIVE),
ANGELIC_WEAPON("The Lance of Uriel",		LANCE),
ANGELIC_WEAPON("The Hammer of Barquiel",	LUCERN_HAMMER),
#undef ANGELIC_WEAPON

A("Arrow of Slaying",				SILVER_ARROW,		(const char *)0,
	777L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 0), (ARTA_VORPAL|ARTA_BLIND|ARTA_BRIGHT),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* mastery artifacts */

/* Archeologist */
/*Always activates special whip effects*/
A("The Trusty Adventurer's Whip",	BULLWHIP,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_ARCHEOLOGIST, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 4), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* TODO read */
A("The Log of the Curator",			SPE_BLANK_PAPER,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_ARCHEOLOGIST, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	OBJECT_DET, NOFLAG
	),

/* TODO read */
/* Detects traps */
/* +7 ac, +7+ench to Int, Wis, and Cha */
A("The Fedora of the Investigator",	FEDORA,				(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_ARCHEOLOGIST, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(), NOFLAG,
	OBJECT_DET, NOFLAG
	),

/* Anachrononaut */
A("The Force Pike of the Red Guard",FORCE_PIKE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_ANACHRONONAUT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_MAGM, 1, 0), (ARTA_MAGIC),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Barbarian */
/* TODO increase STR, DEX, CON by damage taken average */
/* TODO gaining CON increases hp with max hp */
A("The Gauntlets of the Berserker",	GAUNTLETS,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_BARBARIAN, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Binder */
A("The Declaration of the Apostate",SCR_REMOVE_CURSE,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_EXILE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	UNBIND_SEALS, NOFLAG
	),
A("The Soul Lens",					LENSES,				(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_EXILE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(WARNING), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),
A("The Seal of the Spirits",		SCR_BLANK_PAPER,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_EXILE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Caveman/Cavewoman */
/* TODO use club skill */
A("The Torch of Origins",			WAN_FIRE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_CAVEMAN, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_FIRE, 1, 5), (ARTA_VORPAL|ARTA_EXPLFIREX),
	PROPS(FIRE_RES), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Convict */
/* TODO */
A("The Striped Shirt of the Murderer",		STRIPED_SHIRT,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_CONVICT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* TODO protect from theft */
/* TODO implement STEAL */
A("The Striped Shirt of the Thief",	STRIPED_SHIRT,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_CONVICT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	STEAL, NOFLAG
	),

/* TODO */
A("The Striped Shirt of the Falsely Accused",	STRIPED_SHIRT,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_CONVICT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, (ARTI_LUCK)
	),

/* Healer */
/* TODO 2x vs living */
A("The Scalpel of Life and Death",	SCALPEL,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_HEALER, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	LIFE_DEATH, NOFLAG
	),

A("The Gauntlets of the Healing Hand",	GAUNTLETS_OF_DEXTERITY,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_HEALER, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	HEAL_PETS, (ARTI_PLUSSEV)
	),

A("The Ring of Hygiene's Disciple",	RIN_REGENERATION,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_HEALER, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(REGENERATION), NOFLAG,
	PROPS(), NOFLAG,
	HEALING, NOFLAG
	),

/* Knight */
/*double robe effect*/
A("The Cope of the Eldritch Knight",	ROBE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_KNIGHT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	FREE_SPELL, (ARTI_PLUSSEV)
	),

A("The Shield of the Paladin",			KITE_SHIELD,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_KNIGHT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	MONS(vsMA(MA_UNDEAD|MA_DEMON)),
	NO_ATTK(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	BURN_WARD, (ARTI_PLUSSEV)
	),

/* Monk */
A("The Booze of the Drunken Master",	POT_BOOZE,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_MONK, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	FIRE_BLAST, NOFLAG
	),

/* TODO jumping while wielded */
/* TODO staggering blows while wielded */
A("The Khakkhara of the Monkey",		KHAKKHARA,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_MONK, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), ARTA_RETURNING,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Nobleman/Noblewoman */
/* TODO */
A("The Ruffled Shirt of the Aristocrat",RUFFLED_SHIRT,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_NOBLEMAN, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),
/* TODO */
A("The Victorian Underwear of the Aristocrat",	VICTORIAN_UNDERWEAR,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_NOBLEMAN, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),
/* TODO name by appearance */
/* TODO implement LOOT_GOLD */
/* TODO PM_NOBLEWOMAN */
A("The Mark of the Rightful Scion",		RIN_TELEPORT_CONTROL,/* gold */			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_NOBLEMAN, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	MONS(vsMA(MA_ORC)),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	COLLECT_TAX, NOFLAG
	),

/* Priest/Priestess */
A("The Gauntlets of the Divine Disciple",	ORIHALCYON_GAUNTLETS,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_PRIEST, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	PROTECT, (ARTI_PLUSSEV)
	),

A("The Mace of the Evangelist",			MACE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_PRIEST, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	SMITE, NOFLAG
	),

/* Pirate */
/* Rogue */
/* TODO appearance when poisoned/drugged etc */
/* TODO multishot */
A("The Dart of the Assassin",			DART,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_ROGUE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), ARTA_RETURNING,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	SELF_POISON, NOFLAG
	),

/* TODO only name w/ expert short sword */
A("The Sword of the Kleptomaniac",		SHORT_SWORD,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_ROGUE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 5), (ARTA_STEAL),
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(), NOFLAG,
	THEFT_TYPE, NOFLAG
	),

/* Ranger */
A("The Helm of the Arcane Archer",		LEATHER_HELM,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_RANGER, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), (ARTA_EXPLFIRE|ARTA_EXPLCOLD|ARTA_EXPLELEC),
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* TODO naming gender */
/* TODO stoning resistance */
/* TODO apply */
A("The Figurine of Pygmalion",			FIGURINE,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_RANGER, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* TODO naming gender */
/* TODO sex resistance */
/* TODO apply */
A("The Figurine of Galatea",			FIGURINE,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_RANGER, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	SUMMON_PET, NOFLAG
	),

/* Samurai */
A("The Helm of the Ninja",				HELM_OF_OPPOSITE_ALIGNMENT,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_SAMURAI, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	ADD_POISON, (ARTI_PLUSSEV)
	),

/* Tourist */
/* TODO towel wetting as in vanilla 3.6.0 */
/* TODO 2x damage while wet */
/* TODO +1 whip skill while wielded */
/* TODO create tinned biscuits instead of lichen */
A("The Towel of the Interstellar Hitchhiker",	TOWEL,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_TOURIST, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROPS(COLD_RES), NOFLAG,
	PROPS(), NOFLAG,
	TOWEL_ITEMS, NOFLAG
	),

A("The Encyclopedia Galactica",			SPE_BLANK_PAPER,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_TOURIST, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(WARNING, TELEPAT), NOFLAG,
	MAJ_RUMOR, (ARTI_LUCK)
	),

/* Troubadour */
/* Valkyrie */
A("The Twig of Yggdrasil",				WAN_TELEPORTATION,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_VALKYRIE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(TELEPORT_CONTROL), NOFLAG,
	CREATE_PORTAL, NOFLAG
	),
/* TODO flying pets */
A("The Saddle of Brynhildr",			SADDLE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_VALKYRIE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(FLYING), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Wizard */
A("The Staff of Wild Magic",			QUARTERSTAFF,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_WIZARD, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_MAGM, 20, 6), (ARTA_MAGIC|ARTA_EXPLFIRE|ARTA_EXPLCOLD|ARTA_EXPLELEC|ARTA_KNOCKBACKX),
	PROPS(SEARCHING), NOFLAG,
	PROPS(), NOFLAG,
	ENERGY_BOOST, (ARTI_DIG|ARTI_LUCK)
	),

/* TODO exploding spell dice */
/* TODO remove as crowning option */
A("The Robe of the Archmagi",			ROBE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_WIZARD, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(XRAY_VISION, ANTIMAGIC, WARNING), NOFLAG,
	PROPS(), NOFLAG,
	ENERGY_BOOST, (ARTI_PLUSSEV)
	),

/* TODO 2x damage vs non-living */
A("The Forge Hammer of the Artificer",	WAR_HAMMER,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_WIZARD, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	ARTIFICE, NOFLAG
	),

/* Dwarf */
A("The Bulwark of the Dwarven Defender",DWARVISH_ROUNDSHIELD,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DWARF, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(), NOFLAG,
	BLESS, (ARTI_PLUSSEV)
	),
/* Gnome */
/* TODO +1d5 bth against med+ */
/* TODO warn against medium+ */
A("The Hat of the Giant Killer",		GNOMISH_POINTY_HAT,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_GNOME, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(HALF_PHDAM), NOFLAG,
	PROPS(WARN_OF_MON), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/* Half-Dragon */
/* TODO pet dragons +1 beast mastery */
A("The Prismatic Dragon Plate",			PLATE_MAIL,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_HALF_DRAGON, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(HALF_PHDAM), NOFLAG,
	PROPS(), NOFLAG,
	PRISMATIC, (ARTI_PLUSSEV)
	),

/* Human */
/* Incantifier */
/* TODO random clairvoyance */
A("Footprints in the Labyrinth",		STAR_SAPPHIRE,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_INCANTIFIER, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 10), NOFLAG,
	PROPS(SPELLBOOST), NOFLAG,
	PROPS(), NOFLAG,
	ENLIGHTENING, (ARTI_PLUSSEV)
	),

/* Orc */
/* Vampire */
/* TODO grant darkvision */
/* TODO implement SUMMON_VAMP */
A("The Trappings of the Grave",			AMULET_OF_RESTFUL_SLEEP,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_VAMPIRE, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(ANTIMAGIC), NOFLAG,
	PROPS(), NOFLAG,
	SUMMON_VAMP, NOFLAG
	),
/*
 *  terminator; otyp must be zero
 */
A((const char *)0,					STRANGE_OBJECT,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, NO_TIER, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROPS(), NOFLAG,
	PROPS(), NOFLAG,
	NOINVOKE, NOFLAG
	)
};	/* artilist[] (or artifact_names[]) */

#undef	A

#endif /*ARTILIST_H*/

/*artilist.h*/
