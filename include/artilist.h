/*	SCCS Id: @(#)artilist.h 3.4	2003/02/12	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* we need to set these *before* makedefs.c or else it won't be getting the right number of arguments */
#define PROP0()                {0,0,0,0,0,0,0,0}
#define PROP1(a)               {a,0,0,0,0,0,0,0}
#define PROP2(a,b)             {a,b,0,0,0,0,0,0}
#define PROP3(a,b,c)           {a,b,c,0,0,0,0,0}
#define PROP4(a,b,c,d)         {a,b,c,d,0,0,0,0}
#define PROP5(a,b,c,d,e)       {a,b,c,d,e,0,0,0}
#define PROP6(a,b,c,d,e,f)     {a,b,c,d,e,f,0,0}
#define PROP7(a,b,c,d,e,f,g)   {a,b,c,d,e,f,g,0}
#define PROP8(a,b,c,d,e,f,g,h) {a,b,c,d,e,f,g,h}

#define NO_MONS()									 0,   0,   0,   0,   0,   0,   0
#define MONS(mt, mfm, mft, mfb, mfg, mfr, mfv)		mt, mfm, mft, mfb, mfg, mfr, mfv
#define NO_ATTK()									    0,   0,   0
#define ATTK(adtyp, acc, dam)						adtyp, acc, dam

#define NOINVOKE 0
#define NOFLAG 0

#ifdef MAKEDEFS_C
/* in makedefs.c, all we care about is the list of names */

#define A(nam, typ, desc, cost, mat, siz, wgt, aln, cls, rac, val, gen, vsmons, attack, afl, wpr, wfl, cpr, cfl, inv, ifl) nam

static const char *artifact_names[] = {
#else
/* in artifact.c, set up the actual artifact list structure */

#define A(nam, typ, desc, cost, mat, siz, wgt, aln, cls, rac, val, gen, vsmons, attack, afl, wpr, wfl, cpr, cfl, inv, ifl) { \
	 typ, nam, desc, \
	 cost, mat, siz, wgt, \
	 aln, cls, rac, val, gen, \
	 vsmons, \
	 attack, afl, \
	 wpr, wfl, \
	 cpr, cfl, \
	 inv, ifl }

STATIC_OVL NEARDATA struct artifact artilist[] = {
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

//////////////////////Crowning Gifts///////////////////////////////////////
/*Take Me Up/Cast Me Away*/
/*Excalibur is a very accurate weapon, a property that almost doesn't matter except for vs high level demons*/
/*Excalibur does extra blessed damage to demons and undead, +3d7 instead of 1d4*/
A("Excalibur",			LONG_SWORD,						(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_KNIGHT, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_INHER|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 10), NOFLAG,
	PROP2(DRAIN_RES, SEARCHING), (ARTP_SEEK),
	PROP0(), NOFLAG,
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
	PROP1(DRAIN_RES), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Marauder's Map",	SCR_MAGIC_MAPPING,				(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_PIRATE, NON_PM, TIER_B, (ARTG_INHER),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	OBJECT_DET, NOFLAG
	),

/*//////////Namable Artifacts//////////*/
/*most namables can be twoweaponed, look in obj.h */

/*
 *	Orcrist and Sting have same alignment as elves.
 */
A("Orcrist",			ELVEN_BROADSWORD,				(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_C, (ARTG_NAME|ARTG_INHER),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_ORC | MA_DEMON) /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 0), (ARTA_HATES),
	PROP0(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
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
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_ORC | MA_ARACHNID) /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 5, 0), (ARTA_HATES),
	PROP0(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Grimtooth",			ORCISH_DAGGER,					(const char *)0,
	300L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ORC, TIER_D, (ARTG_NAME),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_ELF | MA_HUMAN | MA_DWARF | MA_MINION) /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 5, 0), (ARTA_HATES),
	PROP0(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Carnwennan",			DAGGER,							(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_KNIGHT, NON_PM, TIER_C, (ARTG_NOGEN),	/* ARTG_NAME is added during game-creation for eligible characters */
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, MT_MAGIC /*MT*/, 0 /*MB*/, 0 /*MG*/, MA_FEY /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 5, 10), (ARTA_HATES),
	PROP1(STEALTH), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	INVIS, NOFLAG
	),

/*DF Dwarves can be a nasty lot.*/
/*two handed, so no twoweaponing.*/
A("Slave to Armok",		DWARVISH_MATTOCK,				(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_DWARF, TIER_F, (ARTG_NAME),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, MT_PEACEFUL /*MT*/, 0 /*MB*/, MG_LORD /*MG*/, (MA_ELF | MA_ORC) /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 5, 0), (ARTA_HATES),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_BLOODTHRST)
	),

A("Claideamh",			LONG_SWORD,						(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN | ARTG_NAME),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_ELF | MA_FEY | MA_GIANT | MA_ELEMENTAL | MA_PRIMORDIAL) /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 5, 0), (ARTA_HATES),
	PROP1(WARN_OF_MON), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*//////////The Banes//////////*/
/*banes can be twoweaponed, look in obj.h*/

/* also makes a handy weapon for knights, since it can't break */
A("Dragonlance",		LANCE,							(const char *)0,
	5000L, DRAGON_HIDE, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, MA_DRAGON | MA_REPTILIAN /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES),
	PROP1(REFLECTING), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Nodensfork",			TRIDENT,						"wave-etched %s",
	5000L, SHELL_MAT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, MA_PRIMORDIAL | MA_ET /*MA*/, MV_TELEPATHIC | MV_RLYEHIAN /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_SILVER),
	PROP0(), NOFLAG,
	PROP2(SHOCK_RES, WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Gaia's Fate",		SICKLE,							(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_D, NOFLAG,
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/,
		MA_PLANT | MA_INSECTOID | MA_ARACHNID | MA_AVIAN | MA_REPTILIAN | MA_ANIMAL | MA_FEY | MA_ELF | MA_ELEMENTAL /*MA*/,
		0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES),
	PROP0(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Demonbane",			SABER,							(const char *)0,
	2500L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, MA_DEMON /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES),
	PROP0(), (ARTP_NOCALL), 
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* silver, protects from lycanthropy, and works against quite a number of late-game enemies */
A("Werebane",			SABER,							"wolf-hilted %s",
	1500L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, MA_WERE | MA_DEMIHUMAN /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES),
	PROP0(), (ARTP_NOWERE),
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* deducts movement points from hit giants, and also works vs a few late game enemies */
/* bonus damage applies to all large monsters, and has better damage than a normal axe (base d6+d4/3d4 total) */
A("Giantslayer",		AXE,							"worn %s labeled as belonging to 'Jack'",
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, MT_ROCKTHROW /*MT*/, 0 /*MB*/, 0 /*MG*/, MA_GIANT /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES),
	PROP0(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),


/* effective vs some standard castlevainia enemy types*/
/*is given extra damage in weapon.c, since whip damage is so low*/
A("The Vampire Killer",	BULLWHIP,						"bloodstained chain whip",
	2500L, METAL, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_UNDEAD | MA_DEMON | MA_WERE) /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES),
	PROP1(DRAIN_RES), (ARTP_NOWERE),
	PROP0(), NOFLAG,
	BLESS, NOFLAG
	),

/* works against just about all the late game baddies */
A("Kingslayer",			STILETTO,						"%s inlaid with delicate filigree",
	2500L, GOLD, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MG_LORD | MG_PRINCE) /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_POIS),
	PROP0(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* speaks for itself */
A("Peace Keeper",		ATHAME,							(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, MT_HOSTILE /*MT*/, 0 /*MB*/, 0 /*MG*/, 0 /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_SILVER),
	PROP0(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* vorpal smashes ogres (20/20 chance) */
/* set str and con to 25, and smashing ogres excercises str and wis <- A-tier */
A("Ogresmasher",		WAR_HAMMER,						(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	MONS(S_OGRE /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, 0 /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_VORPAL),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* petrifies trolls, gives bonus damage aginst monsters who pop in to ruin your day */
A("Trollsbane",			MORNING_STAR,					(const char *)0,
	2000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	MONS(S_TROLL /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, MG_REGEN /*MG*/, 0 /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES|ARTA_BRIGHT),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
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
A("Mjollnir",			WAR_HAMMER,						"massive %s embossed with a lightning bolt",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_VALKYRIE, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_ELEC, 5, 24), (ARTA_EXPLELEC),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Pen of the Void",ATHAME,							(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_VOID, PM_EXILE, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	VOID_CHIME, NOFLAG
	),

#ifdef CONVICT
A("Luck Blade",			SHORT_SWORD,					"clover-inscribed %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_CONVICT, NON_PM, TIER_D, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 7, 7), (ARTA_DLUCK),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_LUCK)
	),
#endif /* CONVICT */

A("Cleaver",			BATTLE_AXE,						(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_BARBARIAN, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), (ARTA_SHATTER),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),
A("Atlantean Royal Sword",			TWO_HANDED_SWORD,	(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_BARBARIAN, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), (ARTA_SHATTER),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*	Need a way to convert era times to Japanese luni-solar months.*/
A("Kiku-ichimonji",		KATANA,							"%s with a chrysanthemum on the hilt",
	1200L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_SAMURAI, NON_PM, TIER_C, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 12), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),
A("Jinja Naginata",		NAGINATA,						(const char *)0,
	1200L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_SAMURAI, NON_PM, TIER_C, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 12), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Rhongomyniad",		LANCE,							"%s engraved with a grail icon",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_KNIGHT, NON_PM, TIER_C, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Rod of Lordly Might",		MACE,				"%s with buttons on the haft",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	LORDLY, (ARTI_ENGRAVE)
	),

/* uses the Musicalize skill; learns special abilities from hearing songs */
A("The Singing Sword",	LONG_SWORD,						"%s that emanates soft music",
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_BARD, NON_PM, TIER_A, (ARTG_GIFT|ARTG_INHER|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 1), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	SINGING, NOFLAG
	),

/*Needs encyc entry*/
A("Xiuhcoatl",			BULLWHIP,							"turquoise scaled %s",
	4000L, DRAGON_HIDE, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_FIRE, 3, 12), NOFLAG,
	PROP1(FIRE_RES), NOFLAG,
	PROP0(), NOFLAG,
	LORDLY, NOFLAG
	),

/*invoke for skeletons, life draining */
A("Staff of Necromancy", QUARTERSTAFF,					"skull-topped %s",
	6660L, BONE, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_VAMPIRE, TIER_B, (ARTG_GIFT|ARTG_INHER),
	NO_MONS(),
	ATTK(AD_DRLI, 5, 0), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
	PROP0(), NOFLAG,
	SKELETAL_MINION, NOFLAG
	),
/*
 *	Magicbane is a bit different!  Its magic fanfare
 *	unbalances victims in addition to doing some damage.
 */
/*Needs encyc entry?*/
/*provides curse res while wielded */
A("Magicbane",			ATHAME,							"%s engraved with mystical runes",
	3500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_WIZARD, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_MAGM, 3, 4), (ARTA_MAGIC),
	PROP1(ANTIMAGIC), NOFLAG,
	PROP0(), NOFLAG,
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
	A_CHAOTIC, PM_ROGUE, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 8), (ARTA_HATES|ARTA_CROSSA|ARTA_POIS),	/* missing: SPFX2_NINJA */
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	CREATE_AMMO, NOFLAG /*makes shuriken*/
	),

A("The Golden Sword of Y'ha-Talla",			SCIMITAR,	"%s with a hydra-head hilt",
	3000L, GOLD, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_ROGUE, NON_PM, TIER_B, (ARTG_GIFT | ARTG_INHER),
	NO_MONS(),
	ATTK(AD_DRST, 5, 0), (ARTA_POIS),
	PROP1(POISON_RES), NOFLAG,
	PROP0(), NOFLAG,
	LORDLY, NOFLAG
	),

/*Needs encyc entry*/
/* adds sneak attacks "from behind" (allowing sneak attacks for anyone, and increasing damage for rogues/etc) */
A("Spineseeker",		SHORT_SWORD,					(const char *)0,
	1200L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_ROGUE, NON_PM, TIER_D, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 6), NOFLAG,
	PROP1(STEALTH), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*//////////Double Damage Artifacts//////////*/

A("Grayswandir",		SABER,							"%s decorated with amber swirls",
	8000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROP1(HALLUC_RES), NOFLAG,
	PROP1(WARNING), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Frost Brand",		LONG_SWORD,						"ice-runed %s",
	3000L, GLASS, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_COLD, 1, 0), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),
A("Fire Brand",			LONG_SWORD,						"ember-runed %s",
	3000L, OBSIDIAN_MT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_FIRE, 1, 0), NOFLAG,
	PROP1(FIRE_RES), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Green Dragon Crescent Blade",		NAGINATA,	(const char *)0,
	1200L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 25), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Mirror Brand",		LONG_SWORD,						"polished %s",
	3000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_MAGM, 1, 0), (ARTA_MAGIC),
	PROP1(REFLECTING), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Sunsword",			LONG_SWORD,						(const char *)0,
	1500L, GOLD, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT | ARTG_INHER),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_UNDEAD | MA_DEMON) /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 1, 0), (ARTA_HATES | ARTA_SILVER | ARTA_BRIGHT | ARTA_BLIND | ARTA_SHINING),
	PROP1(SEARCHING), (ARTP_BLINDRES),
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_LIGHT)
	),

/*can be thrown by dwarves*/
A("The Axe of the Dwarvish Lords",		BATTLE_AXE,		(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_DWARF, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROP2(XRAY_VISION, TELEPORT_CONTROL), NOFLAG, 	/*x-ray vision is for dwarves only.*/
	PROP0(), NOFLAG, 
	NOINVOKE, (ARTI_DIG)
	),

/*returns to your hand.*/
/*Needs encyc entry*/
A("Windrider",			BOOMERANG,						(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, NOFLAG,
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Rod of the Ram",	MACE,							"ram-headed %s",
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), (ARTA_KNOCKBACKX),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* attack power is bound to wielder's life force */
A("The Atma Weapon",	BEAMSWORD,						(const char *)0,
	6660L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, MG_NASTY /*MG*/, 0 /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 6, 6), (ARTA_HATES),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* attack power is bound to wielder's magical energy */
A("Limited Moon",		MOON_AXE,						(const char *)0,
	6660L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Black Arrow",	ANCIENT_ARROW,					(const char *)0,
	4444L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*also has a haste effect when wielded, but massively increases hunger and damages the wielder*/
/*The invoked attack is very powerful*/
A("Tensa Zangetsu",		TSURUGI,						(const char *)0,
	4444L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_GIFT | ARTG_INHER),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROP1(HALF_SPDAM), NOFLAG,
	PROP0(), NOFLAG,
	SPEED_BANKAI, NOFLAG
	),

/*//////////Other Artifacts//////////*/

/*Sort of intermediate between a double damage and a utility weapon,*/
/*Sode no Shirayuki gains x2 ice damage after using the third dance.*/
/*however, it only keeps it for a few rounds, and the other dances are attack magic. */
A("Sode no Shirayuki",	KATANA,							"snow-colored %s", // colored not covered
	8000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT | ARTG_INHER),
	NO_MONS(),
	ATTK(AD_COLD, 1, 0), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
	PROP0(), NOFLAG,
	ICE_SHIKAI, NOFLAG
	),

/*Tobiume is an awkward weapon.  It loses 3 damage vs large and 2 vs small*/
/*Ram and Fire blast only trigger if enemy is low hp*/
A("Tobiume",			LONG_SWORD,						"three-pronged %s",
	8000L, METAL, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_GIFT | ARTG_INHER),
	NO_MONS(),
	ATTK(AD_FIRE, 1, 1), (ARTA_DISARM|ARTA_EXPLFIREX|ARTA_KNOCKBACKX),
	PROP1(FIRE_RES), NOFLAG,
	PROP0(), NOFLAG,
	FIRE_SHIKAI, NOFLAG
	),

A("The Lance of Longinus",		SPEAR,					(const char *)0,
	1500L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROP4(ANTIMAGIC, REFLECTING, HALF_PHDAM, HALF_SPDAM), NOFLAG,
	PROP1(DRAIN_RES), NOFLAG,
	WWALKING, NOFLAG
	),

// /* still uses the old artifact structure */
// /* TODO aggrevate 'f' */
// A("The Pink Panther", DIAMOND,			0,			0,
	// (SPFX_NOGEN|SPFX_RESTR), 0, 0,
	// NO_ATTK(),	NO_DFNS,	NO_CARY,
	// TRAP_DET, A_NONE, PM_ARCHEOLOGIST, NON_PM, 0L,
	// 0,0,0),

A("The Arkenstone",		DIAMOND,						"rainbow-glinting sparkling %s", 
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(AGGRAVATE_MONSTER), NOFLAG,
	CONFLICT, (ARTI_PERMALIGHT)
	),

/*Needs encyc entry*/
A("Release from Care",	SCYTHE,							(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 10), (ARTA_VORPAL),
	PROP1(DRAIN_RES), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
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
	PROP2(DRAIN_RES, STEALTH), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
	INVIS, NOFLAG
	),

/* effectively becomes two sizes larger when lit, requiring two hands */
/* attack bonus only applies vs non-magic resistant targets and only when lit */
A("The Holy Moonlight Sword",	LONG_SWORD,				(const char *)0, // begging for a description but i couldn't think of a good one
	4000L, METAL, MZ_DEFAULT, WT_DEFAULT, // if it gets one, it would be sick to have a slightly different one while lit, requires some work tho
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT|ARTG_INHER),
	NO_MONS(),
	ATTK(AD_PHYS, 12, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	ENLIGHTENING, NOFLAG
	),

/*Needs encyc entry*/
A("The Silence Glaive",	GLAIVE,							(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_C, (ARTG_GIFT|ARTG_INHER),
	NO_MONS(),
	ATTK(AD_DRLI, 1, 1), (ARTA_DRAIN),
	PROP1(DRAIN_RES), NOFLAG,
	PROP0(), NOFLAG,
	SATURN, NOFLAG
	),

/*Needs encyc entry*/
/*also has a haste effect when wielded, but massively increases hunger*/
A("The Garnet Rod",		UNIVERSAL_KEY,					"garnet-tipped rod",
	8000L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP2(REGENERATION, ENERGY_REGENERATION), NOFLAG,
	PROP0(), NOFLAG,
	PLUTO, NOFLAG
	),

/*Needs encyc entry*/
/* also protects vs curses while carried */
A("Helping Hand",		GRAPPLING_HOOK,					(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP3(STEALTH, WARNING, SEARCHING), (ARTP_SEEK),
	PROP0(), NOFLAG,
	UNTRAP, (ARTI_ENGRAVE)
	),

/*Needs encyc entry*/
A("The Blade Singer's Saber",		RAKUYO,				(const char *)0,
	1500L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 8, 8), (ARTA_HASTE),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	BLADESONG, NOFLAG
	),
/*Needs encyc entry*/
A("The Blade Dancer's Dagger",		RAKUYO_DAGGER,				(const char *)0,
	1500L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, (ARTG_NOCNT|ARTG_NOGEN),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 4), (ARTA_HASTE),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	BLADESONG, NOFLAG
	),

/*Needs encyc entry*/
A("The Limb of the Black Tree",		CLUB,				"gnarled tree branch",
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_FIRE, 4, 1), (ARTA_EXPLFIREX),
	PROP0(), NOFLAG,
	PROP1(FIRE_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Hellfire",			CROSSBOW,						(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_FIRE, 4, 1), (ARTA_EXPLFIREX),
	PROP1(FIRE_RES), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Lash of the Cold Waste",		BULLWHIP,			(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_COLD, 4, 1), (ARTA_EXPLCOLDX),
	PROP0(), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Ramiel's ranged attack is far more useful than the lash and the limb*/
/*So it's your job to use it right!*/
A("Ramiel",				PARTISAN,						(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_ELEC, 4, 1), (ARTA_EXPLELECX),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* Player gets a second weapon attack after XL15 with this */
A("Quicksilver",		FLAIL,							(const char *)0,
	1200L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 8), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	FAST, NOFLAG
	),

/*Needs encyc entry*/
/* slotless displacement makes this quite good even after you've found a better weapon */
A("Sky Render",			KATANA,							(const char *)0,
	1200L, METAL, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 10), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(DISPLACED), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* Crystal sword whose use comes with a price of your sanity */
A("Callandor",			CRYSTAL_SWORD,					(const char *)0,
	6660L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT, // there's an opportunity for a sick name here too
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT), 
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP3(HALF_SPDAM, ENERGY_REGENERATION, SPELLBOOST), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Yoichi no yumi",		YUMI,							(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_GIFT),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/*Needs encyc entry*/
/* die size set to 1d8 in weapon.c */
A("The Fluorite Octahedron",		BLUE_FLUORITE,		(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, NOFLAG,
	NO_MONS(),
	NO_ATTK(), (ARTA_DLUCK|ARTA_DEXPL),	/* EXTREMELY potent combination of lucky-exploding dice */
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP0(), (ARTP_LIGHTEN|ARTP_WCATRIB),
	PROP0(), NOFLAG,
	ENLIGHTENING, (ARTI_MANDALA|ARTI_PLUSSEV)
	),
#endif

/*double robe effect*/
/*martial arts attacks use exploding dice and get extra damage*/
A("The Grandmaster's Robe",			ROBE,				(const char *)0,
	4500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_GIFT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("The Cloak of the Unheld One",	OILSKIN_CLOAK,		(const char *)0,
	4500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP2(ANTIMAGIC, FREE_ACTION), NOFLAG,
	PROP1(SLEEP_RES), NOFLAG,
	TELEPORT_SHOES, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("The Beastmaster's Duster",		JACKET,				(const char *)0,
	9000L, DRAGON_HIDE, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	PETMASTER, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("Soulmirror",						PLATE_MAIL,			(const char *)0,
	9000L, MITHRIL, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP2(DRAIN_RES, REFLECTING), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

A("Mirrorbright",					ROUNDSHIELD,		(const char *)0,
	4000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP2(REFLECTING, HALLUC_RES), NOFLAG,
	PROP0(), NOFLAG,
	CONFLICT, (ARTI_PLUSSEV)
	),

/*Perseus's shield, needs encyc entry*/
A("Aegis",							ROUNDSHIELD,		"gorgon-emblemed %s",
	4000L, LEATHER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP2(REFLECTING, HALF_PHDAM), NOFLAG,
	PROP0(), NOFLAG,
	AEGIS, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("The Shield of the All-Seeing",	ORCISH_SHIELD,		(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_ORC, TIER_D, NOFLAG,
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, MA_ELF /*MA*/, 0 /*MV*/),
	NO_ATTK(), NOFLAG,
	PROP3(SEARCHING, WARN_OF_MON, FIRE_RES), (ARTP_SEEK),
	PROP0(), NOFLAG,
	PROT_FROM_SHAPE_CHANGERS, (ARTI_PLUSSEV)	/* needs message for invoke */
	),

/*Needs encyc entry*/
A("The Shield of Yggdrasil",		ELVEN_SHIELD,		(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_ELF, TIER_D, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP2(POISON_RES, REGENERATION), NOFLAG,
	PROP0(), NOFLAG,
	HEALING, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("Whisperfeet",					SPEED_BOOTS,		(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(STEALTH), NOFLAG,
	PROP0(), NOFLAG,
	INVIS, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("Water Flowers",					WATER_WALKING_BOOTS,(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, NOFLAG,
	NO_MONS(),
	NO_ATTK(), (ARTA_SILVER),
	PROP1(DISPLACED), NOFLAG,
	PROP0(), NOFLAG,
	TELEPORT_SHOES, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
A("Hammerfeet",						KICKING_BOOTS,		(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_GIFT|ARTG_INHER),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), (ARTA_KNOCKBACKX),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

A("The Shield of the Resolute Heart",	GAUNTLETS_OF_DEXTERITY,			(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(HALF_PHDAM), NOFLAG,
	PROP0(), NOFLAG,
	BLESS, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
/*It is quite deliberate that these cause a spellcasting penalty */
A("The Gauntlets of Spell Power",	GAUNTLETS_OF_POWER,	"silver-runed %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), (ARTA_SILVER),
	PROP2(HALF_SPDAM, SPELLBOOST), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
/* lets the wearer cast Lightning Storm */
A("Stormhelm",						HELM_OF_BRILLIANCE,				(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
	PROP1(SHOCK_RES), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

A("Hellrider's Saddle",				SADDLE,					(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_A, NOFLAG,
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP2(ANTIMAGIC, REFLECTING), NOFLAG,
	PROP0(), NOFLAG,
	INVIS, (ARTI_PLUSSEV)
	),
/*
 *	The artifacts for the quest dungeon, all self-willed.
 */

/*//////////Law Quest Artifacts//////////*/

A("The Rod of Seven Parts",			SPEAR,					(const char *)0,
	7777L, METAL, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_INHER),
	NO_MONS(),
	ATTK(AD_PHYS, 7, 20), (ARTA_HATES|ARTA_CROSSA),
	PROP1(DRAIN_RES), NOFLAG,
	PROP0(), NOFLAG,
	SEVENFOLD, NOFLAG
	),

A("The Field Marshal's Baton",		MACE,					(const char *)0,
	5000L, MT_DEFAULT, MZ_SMALL, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, MG_MERC /*MG*/, 0 /*MA*/, 0 /*MV*/),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*does not protect against lycathropy*/
A("Werebuster",						LONG_SWORD,				(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_F, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, MA_WERE /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("Masamune",						TSURUGI,				(const char *)0,
	7500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), (ARTA_SILVER),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	BLESS, NOFLAG
	),

/*from Final Fantasy*/
/*The crystals are generated together.  The Black Crystal "counts", and the others don't.*/
A("The Black Crystal",				CRYSTAL_BALL,			(const char *)0,
	100L, MT_DEFAULT, MZ_DEFAULT, 30,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), (ARTA_HATES|ARTA_CROSSA),
	PROP0(), NOFLAG,
	PROP2(ANTIMAGIC, WARNING), NOFLAG,
	SHADOW_FLARE, NOFLAG
	),
A("The Water Crystal",				CRYSTAL_BALL,			(const char *)0,
	100L, MT_DEFAULT, MZ_DEFAULT, 40,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
	BLIZAGA, NOFLAG
	),
A("The Fire Crystal",				CRYSTAL_BALL,			(const char *)0,
	100L, MT_DEFAULT, MZ_DEFAULT, 10,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(FIRE_RES), NOFLAG,
	FIRAGA, NOFLAG
	),
A("The Earth Crystal",				CRYSTAL_BALL,			(const char *)0,
	100L, MT_DEFAULT, MZ_DEFAULT, 100,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(HALF_PHDAM), NOFLAG,
	QUAKE, NOFLAG
	),
A("The Air Crystal",				CRYSTAL_BALL,			(const char *)0,
	100L, MT_DEFAULT, MZ_DEFAULT, 20,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(SHOCK_RES), NOFLAG,
	THUNDAGA, NOFLAG
	),

/*Needs encyc entry*/
A("Nighthorn",						UNICORN_HORN,			(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_FIRE, 12, 24), (ARTA_EXPLFIREX),
	PROP1(FIRE_RES), NOFLAG,
	PROP0(), NOFLAG,
	LEVITATION, (ARTI_FEAR|ARTI_LUCK)	/* only fears vs non-chaotic */
	),

/*Needs encyc entry*/
/* blocks Aggravate monster while worn */
A("The Mantle of Wrath",			ORCISH_CLOAK,			(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ORC, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP2(ACID_RES, HALF_SPDAM), NOFLAG,
	PROP2(FIRE_RES, AGGRAVATE_MONSTER), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
/*Actually an iron ring (or twisted, if iron is hunger)*/
A("The Shard from Morgoth's Crown",	RIN_ADORNMENT,			"twisted band of jagged iron",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(SHOCK_RES), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* The Alignment Keys.  Must be grouped together.  Some code in lock.c, artifact.h, and invent.c depends on the order. */
#define ALIGNMENT_KEY(name, alignment)											\
A((name),							SKELETON_KEY,			(const char *)0,	\
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,									\
	(alignment), NON_PM, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),	\
	NO_MONS(),																	\
	NO_ATTK(), NOFLAG,															\
	PROP0(), NOFLAG,															\
	PROP0(), NOFLAG,															\
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	ALTMODE, NOFLAG
	),

A("The Staff of Twelve Mirrors",	KHAKKHARA,				(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 6), NOFLAG,
	PROP2(REFLECTING, DISPLACED), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* reflects projectiles and counterattacks, and doubles your multishot when carried in swapwep or wielded */
A("The Sansara Mirror",				MIRRORBLADE,			(const char *)0,
	3000L, GOLD, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 8, 8), NOFLAG,
	PROP2(REFLECTING, HALF_PHDAM), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Prayer-warded Wrappings of Nitocris",		MUMMY_WRAPPING,			(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*from the works of HP Lovecraft*/
A("The Hand-Mirror of Cthylla",		MIRROR,					(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP4(ANTIMAGIC, SEARCHING, POLYMORPH_CONTROL, TELEPORT_CONTROL), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*from the works of HP Lovecraft*/
A("The Silver Key",					UNIVERSAL_KEY,			"silver key",
	5000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP3(ENERGY_REGENERATION, POLYMORPH_CONTROL, TELEPORT_CONTROL), NOFLAG,
	CREATE_PORTAL, NOFLAG
	),

/*//////////Artifact Books///////////*/
#define ARTIFACT_BOOK(name, invoke)												\
A((name),							SPE_SECRETS,			(const char *)0,	\
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,									\
	A_NONE, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),		\
	NO_MONS(),																		\
	NO_ATTK(), NOFLAG,																\
	PROP0(), NOFLAG,															\
	PROP0(), NOFLAG,															\
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
	A_LAWFUL, PM_KNIGHT, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_INHER),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, MB_THICK_HIDE /*MB*/, 0 /*MG*/, 0 /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 20), (ARTA_HATES),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	LEADERSHIP, (ARTI_DIG)
	),

/*Needs encyc entry*/
A("Reaver",							SCIMITAR,				(const char *)0,
	6000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_PIRATE, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 8), (ARTA_STEAL),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	THEFT_TYPE, NOFLAG
	),

/*Needs encyc entry*/
/* can be read to learn cone of cold */
A("The Bow of Skadi",				BOW,					(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_VALKYRIE, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_COLD, 1, 24), (ARTA_EXPLCOLDX),
	PROP0(), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	LEADERSHIP, (ARTI_PLUSSEV)
	),

/*Needs encyc entry*/
/* permanently filthed */
A("Sunbeam",						GOLDEN_ARROW,			(const char *)0,
	1000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_RANGER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 0), (ARTA_POIS|ARTA_BRIGHT),
	PROP0(), NOFLAG,
	PROP1(DRAIN_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*Needs encyc entry*/
/* permanently drugged */
A("Moonbeam",						SILVER_ARROW,			(const char *)0,
	1000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_RANGER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(DRAIN_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Veil of Latona",				CLOAK_OF_INVISIBILITY,	(const char *)0,
	1000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_RANGER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP2(ANTIMAGIC, REFLECTING), NOFLAG,
	PROP1(DRAIN_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("Hermes's Sandals",				FLYING_BOOTS,			"pair of winged sandals",
	4000L, GOLD, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(FAST), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

A("Poseidon's Trident",				TRIDENT,				(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	WATER, NOFLAG
	),

A("The Eye of the Oracle",			EYEBALL,				(const char *)0,
	500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP2(ANTIMAGIC, HALF_SPDAM), NOFLAG,
	ORACLE, NOFLAG
	),

/*Arc redesign by Riker*/
A("The Macuahuitl of Quetzalcoatl",	MACUAHUITL,				(const char *)0,
	3000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), (ARTA_VORPAL),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*Arc redesign by Riker*/
A("The Mask of Tlaloc",				MASK,					(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP4(HALF_SPDAM, ANTIMAGIC, COLD_RES, SHOCK_RES), NOFLAG,	/* missing: waterproofing */
	NOINVOKE, NOFLAG
	),
/*Arc redesign by Riker*/
A("Ehecailacocozcatl",				AMULET_OF_MAGICAL_BREATHING,			(const char *)0,
	3000L, SHELL_MAT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP2(ENERGY_REGENERATION, REFLECTING), NOFLAG,
	PROP0(), NOFLAG,
	WIND_PETS, NOFLAG
	),
/*Arc redesign by Riker*/
A("Amhimitl",						JAVELIN,				(const char *)0,
	3000L, BONE, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_ARCHEOLOGIST, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_FIRE, 5, 8), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*Arc redesign by Riker*/
/* can "sacrifice" monsters it kills, reducing prayer timeout */
A("The Tecpatl of Huhetotl",		TECPATL,				(const char *)0,
	3000L, BONE, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_ARCHEOLOGIST, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 8), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(DRAIN_RES), NOFLAG,
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
	PROP2(FIRE_RES, REFLECTING), NOFLAG,
	PROP3(TELEPAT, HALF_SPDAM, ANTIMAGIC), NOFLAG,
	SMOKE_CLOUD, NOFLAG
	),

/*Needs encyc entry*/
A("The Annulus",					CHAKRAM,			"%s",
	3000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_ANACHRONONAUT, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 1), NOFLAG,	/*Actually Phys(5,0) if not a lightsaber*/
	PROP0(), NOFLAG,
	PROP2(ANTIMAGIC, HALF_SPDAM), NOFLAG,
	ANNUL, NOFLAG
	),

A("The Heart of Ahriman",			RUBY,				(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_BARBARIAN, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP6(ANTIMAGIC, REFLECTING, HALF_SPDAM, FIRE_RES, DRAIN_RES, POISON_RES), NOFLAG,
	ARTI_REMOVE_CURSE, NOFLAG
	),

#ifdef BARD
A("The Lyre of Orpheus",			MAGIC_HARP,			(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_BARD, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	TAMING, (ARTI_SPEAK)
	),
#endif

A("The Sceptre of Might",			MACE,				(const char *)0,
	2500L, BONE, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_CAVEMAN, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), (ARTA_HATES|ARTA_CROSSA|ARTA_KNOCKBACK),
	PROP0(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	CONFLICT, NOFLAG
	),
#ifdef CONVICT
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
	PROP1(STEALTH), NOFLAG,
	PROP2(WARNING, DRAIN_RES), NOFLAG,
	LEVITATION, (ARTI_LUCK)
	),
A("The Iron Spoon of Liberation",	SPOON,				(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_CONVICT, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROP1(STEALTH), NOFLAG,
	PROP2(SEARCHING, FREE_ACTION), (ARTP_SEEK),
	PHASING, (ARTI_DIG|ARTI_ENGRAVE|ARTI_LUCK)
	),
#endif	/* CONVICT */

/*Creates throwing stars. Makes throwing stars count as silver if wielded */
/*Also can be (a)pplied as a magic flute.								  */
/* needs encyc entry */
A("Silver Starlight",				RAPIER,				(const char *)0,
	5000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 4), (ARTA_SHINING),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/* needs encyc entry */
/* shoots many crossbow bolts at a time (with reduced precision damage) */
A("Wrathful Spider",				DROVEN_CROSSBOW,	(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(STEALTH), NOFLAG,
	PROP0(), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/* needs encyc entry */
/* hits many times in one swing, with special effects */
/* helps spellcasting */
/* protects vs curses while wielded */
A("The Tentacle Rod",				FLAIL,				(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 7, 1), (ARTA_TENTROD),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* needs encyc entry */
A("The Crescent Blade",				SABER,				(const char *)0,
	5000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_DROW, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_FIRE, 4, 0), (ARTA_SHINING|ARTA_VORPAL),
	PROP0(), NOFLAG,
	PROP1(REFLECTING), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* needs encyc entry */
A("The Darkweaver's Cloak",			DROVEN_CLOAK,		(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DROW, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/* needs encyc entry */
/*Adds sleep poison to unarmed attacks*/
A("Spidersilk",						DROVEN_CHAIN_MAIL,	(const char *)0,
	5000L, CLOTH, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(SPELLBOOST), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/* needs encyc entry */
A("The Webweaver's Crook",			FAUCHARD,			(const char *)0,
	5000L, BONE, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), (ARTA_POIS),
	PROP0(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* needs encyc entry */
/* Hedrow crowning gift, chaotic or neutral */
A("Lolth's Fang",					DROVEN_SHORT_SWORD,	(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_ACID, 10, 10), (ARTA_POIS|ARTA_SILVER),
	PROP1(DRAIN_RES), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* needs encyc entry */
/* Drow crowning gift, chaotic */
A("The Web of Lolth",				ELVEN_MITHRIL_COAT,	(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_DROW, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, MA_ELF /*MA*/, 0 /*MV*/),
	NO_ATTK(), (ARTA_SILVER),
	PROP3(WARN_OF_MON, DRAIN_RES, SPELLBOOST), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	ENERGY_BOOST, (ARTI_PLUSSEV)
	),

/* needs encyc entry */
/* Drow crowning gift, neutral */
A("The Claws of the Revenancer",	GAUNTLETS_OF_DEXTERITY,			(const char *)0,
	8000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, PM_DROW, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_DRLI, 1, 1), (ARTA_DRAIN),
	PROP7(ENERGY_REGENERATION, DRAIN_RES, COLD_RES, REGENERATION, FIXED_ABIL, POISON_RES, SEE_INVIS), NOFLAG,
	PROP0(), NOFLAG,
	RAISE_UNDEAD, NOFLAG
	),

/* needs encyc entry */
/* Drow noble crowning gift, lawful */
A("Liecleaver",						DROVEN_CROSSBOW,	(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_DROW, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 10), NOFLAG,
	PROP2(SEARCHING, DRAIN_RES), (ARTP_SEEK),
	PROP1(HALLUC_RES), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/* needs encyc entry */
/* Hedrow noble crowning gift, Chaotic */
A("The Ruinous Descent of Stars",	MORNING_STAR,		(const char *)0,
	8000L, METAL, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_DROW, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), (ARTA_SILVER),
	PROP0(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	FALLING_STARS, NOFLAG
	),

/* needs encyc entry */
/* Drow noble crowning gift, Lawful */
/*returns to your hand when thrown; +1 multishot for drow & elves */
A("Sickle Moon",					SICKLE,				(const char *)0,
	4000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_B, (ARTG_NOWISH),	/* may randomly generate */
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/** Indwelling of the Black Web entity, lawful hedrow crowning gift */

/* needs encyc entry */
/* Elf crowning gift, Lawful */
A("Arcor Kerym",					LONG_SWORD,			(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROP1(DRAIN_RES), NOFLAG,
	PROP0(), NOFLAG,
	HEALING, (ARTI_LIGHT)
	),

/* needs encyc entry */
/* Elf crowning gift, Neutral */
A("Aryfaern Kerym",					RUNESWORD,			(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_ELEC, 5, 10), NOFLAG,
	PROP2(SHOCK_RES, SPELLBOOST), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_LIGHT)
	),

/* needs encyc entry */
/* Elf crowning gift, Chaotic */
A("Aryvelahr Kerym",				CRYSTAL_SWORD,		(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), (ARTA_SILVER),
	PROP2(REFLECTING, DRAIN_RES), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_LIGHT)
	),

A("The Staff of Aesculapius",		QUARTERSTAFF,		(const char *)0,
	5000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_HEALER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_DRLI, 1, 0), (ARTA_SILVER|ARTA_DRAIN), /*silver is associated with the healer's art*/
	PROP2(REGENERATION, DRAIN_RES), NOFLAG,
	PROP0(), NOFLAG,
	HEALING, NOFLAG
	),

/* only gives double spell damage for knights */
A("The Magic Mirror of Merlin",		MIRROR,				(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_KNIGHT, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP2(ANTIMAGIC, TELEPAT), NOFLAG,
	NOINVOKE, (ARTI_SPEAK)
	),

A("The Eyes of the Overworld",		LENSES,				(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_MONK, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(XRAY_VISION), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	ENLIGHTENING, NOFLAG
	),

/*Needs encyc entry*/
/* Also has holy and unholy properties */
A("Avenger",						LONG_SWORD,			(const char *)0,
	1000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 7), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	PROP1(DRAIN_RES), NOFLAG,
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
	PROP2(SHOCK_RES, HALF_SPDAM), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
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
	PROP2(ACID_RES, HALF_PHDAM), NOFLAG,
	PROP1(FIRE_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*Moria dwarf noble first gift */
A("The Armor of Khazad-dum",		DWARVISH_MITHRIL_COAT,			(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, PM_DWARF, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP4(FIRE_RES, ACID_RES, POISON_RES, HALF_SPDAM), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*Moria dwarf noble crowning gift */
A("Durin's Axe",					AXE,				(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, PM_DWARF, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 10), (ARTA_SILVER),
	PROP2(DRAIN_RES, SEARCHING), (ARTP_SEEK),
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_DIG)
	),

/*Needs encyc entry*/
/*Lonely Mountain dwarf noble first gift */
/* the old elves fought balrogs too. */
A("Glamdring",						ELVEN_BROADSWORD,	(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_KNIGHT, PM_DWARF, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_ORC|MA_DEMON) /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 10, 10), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Key of Erebor",				SKELETON_KEY,		(const char *)0,
	1500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, NON_PM, TIER_F, (ARTG_NOGEN|ARTG_NOWISH|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP4(HALF_PHDAM, ANTIMAGIC, FIRE_RES, COLD_RES), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),
/*Arkenstone*/ /*Lonely Mountain dwarf noble crown*/

/*Needs encyc entry*/
/* Drow noble first gift  */
A("The Sceptre of Lolth",			KHAKKHARA,			(const char *)0,
	4000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_NOBLEMAN, PM_DROW, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	LORDLY, (ARTI_ENGRAVE)
	),

/*Needs encyc entry*/
/* Drow noble quest */
/* Plus double AC bonus */
A("The Web of the Chosen",			DROVEN_CLOAK,		(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_NOBLEMAN, PM_DROW, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), (ARTA_SILVER),
	PROP3(REFLECTING, HALF_SPDAM, ACID_RES), NOFLAG,
	PROP1(SHOCK_RES), NOFLAG,
	NOINVOKE, (ARTI_ENGRAVE)
	),

/*Needs encyc entry*/
/* Hedrow noble first gift */
A("The Death-Spear of Vhaerun",		DROVEN_SPEAR,		(const char *)0,
	4000L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_NOBLEMAN, PM_DROW, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 12), (ARTA_SILVER),
	PROP1(ANTIMAGIC), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP2(HALF_PHDAM, DRAIN_RES), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Profaned Greatscythe",		SCYTHE,				(const char *)0,
	4000L, MINERAL, MZ_HUGE, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_FIRE, 8, 30), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(FIRE_RES), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* slowing cold magic offense */
/* scales with STR/2 & DEX & INT for +20 max*/
A("Friede's Scythe",				SCYTHE,				(const char *)0,
	4000L, METAL, MZ_SMALL, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_MAGM, 6, 6), (ARTA_MAGIC|ARTA_SHINING),
	PROP0(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* Drains energy/sets special ability cooldowns */
/* scales with STR & DEX & WIS for +24 max*/
A("Yorshka's Spear",				SPEAR,				(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), (NOFLAG),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/* Doubles dragon attributes when carried */
A("The Dragon's Heart-Stone",		FLINT,				(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_NOBLEMAN, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), (NOFLAG),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Mitre of Holiness",			HELM_OF_BRILLIANCE,	(const char *)0,
	2000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_PRIEST, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, MA_UNDEAD /*MA*/, 0 /*MV*/),
	NO_ATTK(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	PROP1(FIRE_RES), NOFLAG,
	ENERGY_BOOST, (ARTI_PLUSSEV)
	),

/* protects inventory from curses */
/* polymorphs contained items occasionally */
A("The Treasury of Proteus",		CHEST,				(const char *)0,
	2500L, MT_DEFAULT, MZ_DEFAULT, 50,
	A_CHAOTIC, PM_PIRATE, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	ENERGY_BOOST, (ARTI_LUCK)
	),

A("The Longbow of Diana",			BOW,				(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_RANGER, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), (ARTA_SILVER),
	PROP1(REFLECTING), NOFLAG,
	PROP1(TELEPAT), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

A("The Rogue Gear-spirits",			CROSSBOW,			(const char *)0,
	4000L, MT_DEFAULT, MZ_SMALL, WT_DEFAULT,
	A_NEUTRAL, PM_RANGER, PM_GNOME, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROP1(SEARCHING), (ARTP_SEEK),
	PROP3(FIRE_RES, WARNING, TELEPAT), NOFLAG,
	UNTRAP, (ARTI_DIG|ARTI_SPEAK)
	),

/*Needs encyc entry*/
/* yes, causes spell penalties*/
A("The Steel Scales of Kurtulmak",	GRAY_DRAGON_SCALES,	(const char *)0,
	5000L, IRON, MZ_DEFAULT, 300,
	A_LAWFUL, NON_PM, PM_KOBOLD, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP4(FREE_ACTION, FIRE_RES, REFLECTING, HALF_PHDAM), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Glitterstone",				AMBER,				(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, NON_PM, PM_GNOME, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(ENERGY_REGENERATION), NOFLAG,
	CHARGE_OBJ, (ARTI_PERMALIGHT)
	),

A("Great Claws of Urdlen",			GAUNTLETS_OF_POWER,	"clawed %s",
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	QUAKE, (ARTI_PLUSSEV|ARTI_DIG)
	),
	
/*Needs encyc entry*/
A("The Moonbow of Sehanine",		ELVEN_BOW,			(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_RANGER, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), (ARTA_SILVER),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/*Needs encyc entry*/
A("The Spellsword of Corellon",		HIGH_ELVEN_WARSWORD,			(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 10), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Warhammer of Vandria",		WAR_HAMMER,			(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, PM_ELF, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),


/*Needs encyc entry*/
A("The Shield of Saint Cuthbert",	SHIELD_OF_REFLECTION,(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP2(HALF_SPDAM, HALF_PHDAM), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
A("The Palantir of Westernesse",	CRYSTAL_BALL,		(const char *)0,
	8000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP4(WARNING, TELEPAT, REFLECTING, XRAY_VISION), NOFLAG,
	TAMING, NOFLAG
	),

/*Needs encyc entry*/
A("Belthronding",					ELVEN_BOW,			(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROP1(STEALTH), NOFLAG,
	PROP1(DISPLACED), NOFLAG,
	CREATE_AMMO, NOFLAG
	),

/*Needs encyc entry*/
/* Elf noble first gift */
A("The Rod of the Elvish Lords",	ELVEN_MACE,			(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_ELF, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 3, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	LORDLY, (ARTI_ENGRAVE)
	),

A("The Master Key of Thievery",		SKELETON_KEY,		(const char *)0,
	3500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, PM_ROGUE, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP2(HALF_PHDAM, TELEPORT_CONTROL), NOFLAG,
	UNTRAP, (ARTI_SPEAK)
	),

A("The Tsurugi of Muramasa",		TSURUGI,			(const char *)0,
	4500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, PM_SAMURAI, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_PHYS, 2, 0), (ARTA_VORPAL|ARTA_SHATTER),
	PROP0(), (ARTP_SEEK),
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_LUCK)
	),

#ifdef TOURIST
A("The Platinum Yendorian Express Card",		CREDIT_CARD,			(const char *)0,
	7000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_TOURIST, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), (ARTA_SILVER),
	PROP0(), NOFLAG,
	PROP3(HALF_SPDAM, TELEPAT, ANTIMAGIC), NOFLAG,
	CHARGE_OBJ, NOFLAG
	),
#endif

A("The Orb of Fate",				CRYSTAL_BALL,		(const char *)0,
	3500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_VALKYRIE, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), (NOFLAG),
	PROP0(), NOFLAG,
	PROP3(HALF_SPDAM, HALF_PHDAM, WARNING), NOFLAG,
	LEV_TELE, (ARTI_LUCK)
	),

/*Needs encyc entry*/
/* The Sun of the Gods of the Dead */
A("Sol Valtiva",					TWO_HANDED_SWORD,	(const char *)0,
	4000L, OBSIDIAN_MT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, PM_FIRE_GIANT, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_FIRE, 5, 24), (ARTA_EXPLFIRE|ARTA_BLIND|ARTA_BRIGHT),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_LIGHT)
	),

A("The Eye of the Aethiopica",		AMULET_OF_ESP,		(const char *)0,
	4000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NEUTRAL, PM_WIZARD, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), (NOFLAG),
	PROP0(), NOFLAG,
	PROP3(ANTIMAGIC, HALF_SPDAM, ENERGY_REGENERATION), NOFLAG,
	CREATE_PORTAL, NOFLAG
	),

/*//////////Special High-Level Artifacts//////////*/

A("The Hat of the Archmagi",		CORNUTHAUM,			(const char *)0,
	9000L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_WIZARD, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), (NOFLAG),
	PROP2(XRAY_VISION, WARNING), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP2(ENERGY_REGENERATION, SEARCHING), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

  /*/////////////Special Monster Artifacts//////////////*/
 /*//Few of these count, since they are boss treasure//*/
/*////////////////////////////////////////////////////*/

/*Needs encyc entry*/
/*heavier than normal, and causes spell penalties*/
A("The Platinum Dragon Plate",		SILVER_DRAGON_SCALE_MAIL,			(const char *)0,
	9000L, PLATINUM, MZ_DEFAULT, 225,
	A_NONE, NON_PM, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP7(ANTIMAGIC, FIRE_RES, COLD_RES, DISINT_RES, SHOCK_RES, SLEEP_RES, FREE_ACTION), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry*/
/*heavier than normal */
A("The Chromatic Dragon Scales",	BLACK_DRAGON_SCALES,			(const char *)0,
	9000L, MT_DEFAULT, MZ_DEFAULT, 225,
	A_NONE, NON_PM, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP8(FIRE_RES, COLD_RES, DISINT_RES, SHOCK_RES, POISON_RES, SICK_RES, ACID_RES, STONE_RES), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

A("The Eye of Vecna",				EYEBALL,			(const char *)0,
	500L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(HALF_SPDAM), NOFLAG,
	PROP0(), NOFLAG,
	DEATH_GAZE, NOFLAG
	),
A("The Hand of Vecna",				SEVERED_HAND,		(const char *)0,
	700L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(DRAIN_RES), NOFLAG,
	PROP1(COLD_RES), NOFLAG,
	SUMMON_UNDEAD, NOFLAG
	),

/*Needs encyc entry, somehow*/
A("Genocide",						TWO_HANDED_SWORD,	(const char *)0,
	9999L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_S, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_FIRE, 9, 9), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_BLOODTHRST)
	),

/*Needs encyc entry */
A("The Rod of Dis",					MACE,				(const char *)0,
	9999L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 8), (ARTA_KNOCKBACK),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	TAMING, NOFLAG
	),

/*Needs encyc entry */
A("Avarice",						SHORT_SWORD,		(const char *)0,
	9999L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 10, 1), (ARTA_STEAL),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	THEFT_TYPE, NOFLAG
	),

/*Needs encyc entry */
A("The Fire of Heaven",				TRIDENT,			(const char *)0,
	9999L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_FIRE, 1, 0), (ARTA_EXPLFIREX|ARTA_EXPLELEC),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
/*used to have DRAIN_MEMORIES instead of CONFLICT */ 
A("The Diadem of Amnesia",			DUNCE_CAP,			(const char *)0,
	9999L, MT_DEFAULT, MZ_HUGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_F, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	CONFLICT, NOFLAG
	),

/*Needs encyc entry */
/* has a unique interaction with Nudziarth */
A("Shadowlock",						RAPIER,				(const char *)0,
	9999L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 20, 0), (ARTA_VORPAL|ARTA_SHINING),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("Thunder's Voice",				DAGGER,				(const char *)0,
	3333L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_ELEC, 6, 6), (ARTA_EXPLELEC),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("Serpent's Tooth",				ATHAME,				(const char *)0,
	3333L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), (ARTA_POIS),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("Unblemished Soul",				UNICORN_HORN,		(const char *)0,
	3333L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), (ARTA_SILVER),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_LUCK)
	),

/*Needs encyc entry */
A("Ramithaine",						LONG_SWORD,			(const char *)0,
	3333L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 6, 1), (ARTA_VORPAL),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("The Wrath of Heaven",			LONG_SWORD,			(const char *)0,
	9999L, SILVER, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_ELEC, 1, 0), (ARTA_EXPLFIRE|ARTA_EXPLELECX),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
/* Used to have SLAY_LIVING */
A("The All-seeing Eye of the Fly",	HELM_OF_TELEPATHY,	(const char *)0,
	9999L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(DETECT_MONSTERS), NOFLAG,
	PROP0(), NOFLAG,
	SHADOW_FLARE, NOFLAG
	),

/*Needs encyc entry */
A("Cold Soul",						RANSEUR,			(const char *)0,
	9999L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), (ARTA_EXPLFIRE|ARTA_EXPLELEC|ARTA_EXPLCOLD),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("The Sceptre of the Frozen Floor of Hell",	QUARTERSTAFF,			(const char *)0,
	9999L, METAL, MZ_LARGE, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_COLD, 1, 0), (ARTA_EXPLCOLDX),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	BLIZAGA, NOFLAG
	),

/*Needs encyc entry */
A("Caress",							BULLWHIP,			(const char *)0,
	9999L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_ELEC, 1, 20), NOFLAG,
	PROP1(SHOCK_RES), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
/*Weapon of Lixer, Prince of Hell, from Dicefreaks the Gates of Hell*/
/*also does +9 damage to S_ANGELs*/
A("The Iconoclast",					SABER,				(const char *)0,
	9999L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_LAWFUL, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_HUMAN|MA_ELF|MA_DWARF|MA_GNOME) /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 9, 99), (ARTA_HATES),
	PROP1(ANTIMAGIC), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("The Three-Headed Flail",			FLAIL,				"three-headed %s",
	6660L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	NO_ATTK(), (ARTA_THREEHEAD),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("Heartcleaver",					HALBERD,			(const char *)0,
	6660L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 1, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("Wrathful Wind",					CLUB,				(const char *)0,
	6660L, MT_DEFAULT, MZ_HUGE, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_C, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_COLD, 10, 0), (ARTA_SILVER|ARTA_EXPLCOLD),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
A("The Sting of the Poison Queen",	FLAIL,				(const char *)0,
	6660L, MT_DEFAULT, MZ_LARGE, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_D, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_PHYS, 4, 12), (ARTA_POIS),
	PROP1(ANTIMAGIC), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
/* Hates elves, but not drow */
A("The Scourge of Lolth",			VIPERWHIP,			(const char *)0,
	6660L, SILVER, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, MA_ELF /*MA*/, 0 /*MV*/),
	ATTK(AD_PHYS, 1, 0), (ARTA_HATES|ARTA_DEXPL),
	PROP0(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
/*Weapon of Graz'zt, from Gord the Rogue*/
A("Doomscreamer",					TWO_HANDED_SWORD,	(const char *)0,
	6660L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_ACID, 1, 0), NOFLAG,
	PROP1(ACID_RES), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/*Needs encyc entry */
/*Weapon of Graz'zt, from Gord the Rogue*/
A("The Wand of Orcus",				WAN_DEATH,			(const char *)0,
	8000L, MT_DEFAULT, MZ_HUGE, WT_DEFAULT,
	A_CHAOTIC, NON_PM, NON_PM, TIER_A, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),
	NO_MONS(),
	ATTK(AD_DRLI, 5, 12), (ARTA_DRAIN),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_BLOODTHRST)
	),

/* Weapons of the Archons. Need encyc entries. */
#define ANGELIC_WEAPON(name, type)														\
A((name),							(type),				(const char *)0,				\
	7777L, SILVER, MZ_DEFAULT, WT_DEFAULT,												\
	A_LAWFUL, NON_PM, NON_PM, TIER_B, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR|ARTG_NOCNT),	\
	NO_MONS(),																			\
	ATTK(AD_PHYS, 7, 10), (ARTA_BLIND|ARTA_BRIGHT),										\
	PROP1(SEARCHING), (ARTP_BLINDRES),													\
	PROP0(), NOFLAG,																	\
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* TODO read */
A("The Log of the Curator",			SPE_BLANK_PAPER,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_ARCHEOLOGIST, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
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
	PROP1(ANTIMAGIC), NOFLAG,
	PROP0(), NOFLAG,
	OBJECT_DET, NOFLAG
	),

/* Anachrononaut */
A("The Force Pike of the Red Guard",FORCE_PIKE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_ANACHRONONAUT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_MAGM, 1, 0), (ARTA_MAGIC),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Binder */
A("The Declaration of the Apostate",SCR_REMOVE_CURSE,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_EXILE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	UNBIND_SEALS, NOFLAG
	),
A("The Soul Lens",					LENSES,				(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_EXILE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(WARNING), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),
A("The Seal of the Spirits",		SCR_BLANK_PAPER,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_EXILE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Caveman/Cavewoman */
/* TODO use club skill */
A("The Torch of Origins",			WAN_FIRE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_CAVEMAN, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_FIRE, 1, 5), (ARTA_VORPAL|ARTA_EXPLFIREX),
	PROP1(FIRE_RES), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Convict */
/* TODO */
A("The Striped Shirt of the Murderer",		STRIPED_SHIRT,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_CONVICT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* TODO protect from theft */
/* TODO implement STEAL */
A("The Striped Shirt of the Thief",	STRIPED_SHIRT,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_CONVICT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	STEAL, NOFLAG
	),

/* TODO */
A("The Striped Shirt of the Falsely Accused",	STRIPED_SHIRT,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_CONVICT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, (ARTI_LUCK)
	),

/* Healer */
/* TODO 2x vs living */
A("The Scalpel of Life and Death",	SCALPEL,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_HEALER, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	LIFE_DEATH, NOFLAG
	),

A("The Gauntlets of the Healing Hand",	GAUNTLETS_OF_DEXTERITY,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_HEALER, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	HEAL_PETS, (ARTI_PLUSSEV)
	),

A("The Ring of Hygiene's Disciple",	RIN_REGENERATION,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_HEALER, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(REGENERATION), NOFLAG,
	PROP0(), NOFLAG,
	HEALING, NOFLAG
	),

/* Knight */
/*double robe effect*/
A("The Cope of the Eldritch Knight",	ROBE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_KNIGHT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	FREE_SPELL, (ARTI_PLUSSEV)
	),

A("The Shield of the Paladin",			KITE_SHIELD,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_KNIGHT, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_UNDEAD|MA_DEMON) /*MA*/, 0 /*MV*/),
	NO_ATTK(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	BURN_WARD, (ARTI_PLUSSEV)
	),

/* Monk */
A("The Booze of the Drunken Master",	POT_BOOZE,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_MONK, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	FIRE_BLAST, NOFLAG
	),

/* TODO 2x damage against undead/demons */
A("The Wrappings of the Sacred Fist",	GLOVES,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_MONK, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	FAST_TURNING, (ARTI_PLUSSEV)
	),

/* TODO jumping while wielded */
/* TODO staggering blows while wielded */
A("The Khakkhara of the Monkey",		KHAKKHARA,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_MONK, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Nobleman/Noblewoman */
/* TODO */
A("The Ruffled Shirt of the Aristocrat",RUFFLED_SHIRT,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_NOBLEMAN, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),
/* TODO */
A("The Victorian Underwear of the Aristocrat",	VICTORIAN_UNDERWEAR,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_NOBLEMAN, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),
/* TODO name by appearance */
/* TODO implement LOOT_GOLD */
/* TODO PM_NOBLEWOMAN */
A("The Mark of the Rightful Scion",		RIN_TELEPORT_CONTROL,/* gold */			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_NOBLEMAN, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_ORC) /*MA*/, 0 /*MV*/),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	COLLECT_TAX, NOFLAG
	),

/* Priest/Priestess */
A("The Gauntlets of the Divine Disciple",	ORIHALCYON_GAUNTLETS,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_PRIEST, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	PROTECT, (ARTI_PLUSSEV)
	),

A("The Mace of the Evangelist",			MACE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_PRIEST, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 0), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
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
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	SELF_POISON, NOFLAG
	),

/* TODO only name w/ expert short sword */
A("The Sword of the Kleptomaniac",		SHORT_SWORD,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_ROGUE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	ATTK(AD_PHYS, 5, 5), (ARTA_STEAL),
	PROP1(ANTIMAGIC), NOFLAG,
	PROP0(), NOFLAG,
	THEFT_TYPE, NOFLAG
	),

/* Ranger */
A("The Helm of the Arcane Archer",		LEATHER_HELM,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_RANGER, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), (ARTA_EXPLFIRE|ARTA_EXPLCOLD|ARTA_EXPLELEC),
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	SUMMON_PET, NOFLAG
	),

/* Samurai */
A("The Helm of the Ninja",				HELM_OF_OPPOSITE_ALIGNMENT,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_SAMURAI, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP1(COLD_RES), NOFLAG,
	PROP0(), NOFLAG,
	TOWEL_ITEMS, NOFLAG
	),

A("The Encyclopedia Galactica",			SPE_BLANK_PAPER,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_TOURIST, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP2(WARNING, TELEPAT), NOFLAG,
	MAJ_RUMOR, (ARTI_LUCK)
	),

/* Troubadour */
/* Valkyrie */
A("The Twig of Yggdrasil",				WAN_TELEPORTATION,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_VALKYRIE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(TELEPORT_CONTROL), NOFLAG,
	CREATE_PORTAL, NOFLAG
	),
/* TODO flying pets */
A("The Saddle of Brynhildr",			SADDLE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_VALKYRIE, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* Wizard */
A("The Staff of Wild Magic",			QUARTERSTAFF,	(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_WIZARD, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	ATTK(AD_MAGM, 20, 6), (ARTA_MAGIC|ARTA_EXPLFIRE|ARTA_EXPLCOLD|ARTA_EXPLELEC|ARTA_KNOCKBACKX),
	PROP1(SEARCHING), NOFLAG,
	PROP0(), NOFLAG,
	ENERGY_BOOST, (ARTI_DIG|ARTI_LUCK)
	),

/* TODO exploding spell dice */
/* TODO remove as crowning option */
A("The Robe of the Archmagi",			ROBE,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_WIZARD, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH|ARTG_MAJOR),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP3(XRAY_VISION, ANTIMAGIC, WARNING), NOFLAG,
	PROP0(), NOFLAG,
	ENERGY_BOOST, (ARTI_PLUSSEV)
	),

/* TODO 2x damage vs non-living */
A("The Forge Hammer of the Artificer",	WAR_HAMMER,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, PM_WIZARD, NON_PM, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	ARTIFICE, NOFLAG
	),

/* Drow */
/* TODO name by appearance */
A("The Ring of Lolth",					RIN_PROTECTION_FROM_SHAPE_CHAN,/* black signet */			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DROW, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_ORC) /*MA*/, 0 /*MV*/),
	NO_ATTK(), NOFLAG,
	PROP1(FIRE_RES), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	FIRE_BLAST, NOFLAG
	),

/* Dwarf */
A("The Bulwark of the Dwarven Defender",DWARVISH_ROUNDSHIELD,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_DWARF, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(ANTIMAGIC), NOFLAG,
	PROP0(), NOFLAG,
	BLESS, (ARTI_PLUSSEV)
	),
/* Elf */
/* TODO name by appearance */
A("Narya",								RIN_TELEPORT_CONTROL,	/* gold */			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_ELF, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_ORC) /*MA*/, 0 /*MV*/),
	NO_ATTK(), NOFLAG,
	PROP1(FIRE_RES), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	FIRE_BLAST, NOFLAG
	),

/* TODO water walking */
/* TODO protect inventory from water damage */
/* TODO name by appearance */
A("Nenya",								RIN_TELEPORTATION,	/* silver */			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_ELF, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_ORC) /*MA*/, 0 /*MV*/),
	NO_ATTK(), NOFLAG,
	PROP0(), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, NOFLAG
	),

/* TODO name by appearance */
A("Vilya",								RIN_AGGRAVATE_MONSTER,	/* sapphire */			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_ELF, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	MONS(0 /*Monster Symbol*/, 0 /*MM*/, 0 /*MT*/, 0 /*MB*/, 0 /*MG*/, (MA_ORC) /*MA*/, 0 /*MV*/),
	NO_ATTK(), NOFLAG,
	PROP1(SHOCK_RES), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	HEALING, NOFLAG
	),

/* Gnome */
/* TODO +1d5 bth against med+ */
/* TODO warn against medium+ */
A("The Hat of the Giant Killer",		GNOMISH_POINTY_HAT,			(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_GNOME, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(HALF_PHDAM), NOFLAG,
	PROP1(WARN_OF_MON), NOFLAG,
	NOINVOKE, (ARTI_PLUSSEV)
	),

/* Half-Dragon */
/* TODO pet dragons +1 beast mastery */
A("The Prismatic Dragon Plate",			PLATE_MAIL,		(const char *)0,
	0L, MT_DEFAULT, MZ_DEFAULT, WT_DEFAULT,
	A_NONE, NON_PM, PM_HALF_DRAGON, NO_TIER, (ARTG_NOGEN|ARTG_NOWISH),
	NO_MONS(),
	NO_ATTK(), NOFLAG,
	PROP1(HALF_PHDAM), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP1(SPELLBOOST), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP1(ANTIMAGIC), NOFLAG,
	PROP0(), NOFLAG,
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
	PROP0(), NOFLAG,
	PROP0(), NOFLAG,
	NOINVOKE, NOFLAG
	)
};	/* artilist[] (or artifact_names[]) */

#undef	A

#ifndef MAKEDEFS_C
#endif

/*artilist.h*/
