/*	SCCS Id: @(#)monst.h	3.4	1999/01/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MONST_H
#define MONST_H

#include "prop.h"
#include "attrib.h"

/* The weapon_check flag is used two ways:
 * 1) When calling mon_wield_item, is 2-6 depending on what is desired.
 * 2) Between calls to mon_wield_item, is 0 or 1 depending on whether or not
 *    the weapon is known by the monster to be cursed (so it shouldn't bother
 *    trying for another weapon).
 * I originally planned to also use 0 if the monster already had its best
 * weapon, to avoid the overhead of a call to mon_wield_item, but it turns out
 * that there are enough situations which might make a monster change its
 * weapon that this is impractical.  --KAA
 */
# define NO_WEAPON_WANTED 0
# define NEED_WEAPON 1
# define NEED_RANGED_WEAPON 2
# define NEED_HTH_WEAPON 3
# define NEED_PICK_AXE 4
# define NEED_AXE 5
# define NEED_PICK_OR_AXE 6

/* The following flags are used for the second argument to display_minventory
 * in invent.c:
 *
 * MINV_NOLET  If set, don't display inventory letters on monster's inventory.
 * MINV_ALL    If set, display all items in monster's inventory, otherwise
 *	       just display wielded weapons and worn items.
 */
#define MINV_NOLET 0x01
#define MINV_ALL   0x02

#ifndef ALIGN_H
#include "align.h"
#endif

#define toostrong(monindx, lev) (monstr[monindx] > lev)
#define tooweak(monindx, lev)	(monstr[monindx] < lev && !is_eladrin(&mons[monindx]))

struct monst {
	struct monst *nmon;
	struct permonst *data;
	unsigned m_id;
	int mtyp;			/* index number of base permonst array */
	short movement;		/* movement points (derived from permonst definition and added effects */
	uchar m_lev;		/* adjusted difficulty level of monster */
	aligntyp malign;	/* alignment of this monster, relative to the
				   player (positive = good to kill) */
	int mx, my;
	int mux, muy;		/* where the monster thinks you are */
#define MTSZ	4
#define no_upos(mon)	((mon)->mux == 0 && (mon)->muy == 0)
	coord mtrack[MTSZ];	/* monster track */
	int mhp, mhpmax;
	unsigned mappearance;	/* for undetected mimics and the wiz */
	uchar	 m_ap_type;	/* what mappearance is describing: */
#define M_AP_NOTHING	0	/* mappearance is unused -- monster appears
				   as itself */
#define M_AP_FURNITURE	1	/* stairs, a door, an altar, etc. */
#define M_AP_OBJECT	2	/* an object */
#define M_AP_MONSTER	3	/* a monster */

	int mtame;		/* level of tameness, implies peaceful */
	int mpeacetime;		/* level of peacefulness, implies peaceful, ignored if tame */
	unsigned long int mintrinsics[MPROP_SIZE];
	unsigned long int mextrinsics[MPROP_SIZE];
	unsigned long int acquired_trinsics[MPROP_SIZE];
	int mspec_used;		/* monster's special ability attack timeout */
	int mstdy;		/* to record extra damage to be delt due to having been studied */
	int ustdym;		/* to record extra damage to be delt by you due to having been studied by you*/
	int m_san_level;/* DEPRECATED: to record sanity level at which this monster will be obscured */
	int m_insight_level;/* to record insight level at which this monster will be reveled */
	
	struct attribs	acurr,		/* your attributes (eg. str)*/
			aexe,		/* for gain/loss via "exercise" */
			abon,		/* your bonus attributes (eg. str) */
			amax,		/* max attributes (eg. str) */
			atemp,		/* used for temporary loss/gain */
			atime;		/* used for loss/gain countdown */

	Bitfield(mferal,1);	/* was once tame */ /*1*/
	
	Bitfield(female,1);	/* is female */ /*2*/
	Bitfield(minvis,1);	/* currently invisible */ /*3*/
	Bitfield(invis_blkd,1); /* invisibility blocked */ /*4*/
	Bitfield(perminvis,1);	/* intrinsic minvis value */ /*5*/
	Bitfield(cham,4);	/* shape-changer */ /*9*/
/* note: lychanthropes are handled elsewhere */
#define CHAM_ORDINARY		0	/* not a shapechanger */
#define CHAM_CHAMELEON		1	/* animal */
#define CHAM_DOPPELGANGER	2	/* demi-human */
#define CHAM_SANDESTIN		3	/* demon */
#define CHAM_DREAM			4	/* anything */
#define CHAM_MAX_INDX		CHAM_DREAM
	Bitfield(mundetected,1);	/* not seen in present hiding place */ /*10*/
				/* implies one of MT_CONCEAL or MT_HIDE,
				 * but not mimic (that is, snake, spider,
				 * trapper, piercer, eel)
				 */

	Bitfield(mcan,1);	/* has been cancelled */ /*11*/
	Bitfield(mburied,1);	/* has been buried */ /*12*/
	Bitfield(mspeed,2);	/* current speed */ /*14*/
	Bitfield(permspeed,2);	/* intrinsic mspeed value */ /*16*/
	Bitfield(mrevived,1);	/* has been revived from the dead */ /*17*/
	Bitfield(mavenge,1);	/* did something to deserve retaliation */ /*18*/
	
	Bitfield(mflee,1);	/* fleeing */ /*19*/
	Bitfield(mfleetim,7);	/* timeout for mflee */ /*26*/
	
	Bitfield(mcrazed,1);	/* monster is crazed */ /*27*/
	
	Bitfield(mberserk,1);	/* monster is crazed */ /*28*/
	
	Bitfield(mcansee,1);	/* cansee 1, temp.blinded 0, blind 0 */ /*29*/
	Bitfield(mblinded,7);	/* cansee 0, temp.blinded n, blind 0 */ /*36*/

	Bitfield(mcanhear,1);	/* cansee 1, temp.blinded 0, blind 0 */ /*37*/
	Bitfield(mdeafened,7);	/* cansee 0, temp.blinded n, blind 0 */ /*44*/

	Bitfield(mcanmove,1);	/* paralysis, similar to mblinded */ /*45*/
	Bitfield(mfrozen,7); /*52*/
	
	Bitfield(mnotlaugh,1);	/* collapsed from laughter, similar to mblinded */ /*53*/
	Bitfield(mlaughing,7); /*60*/
	
	Bitfield(msleeping,1);	/* asleep until woken */ /*61*/
	Bitfield(mstun,1);	/* stunned (off balance) */ /*62*/
	Bitfield(mconf,1);	/* confused */ /*63*/
	Bitfield(mpeaceful,1);	/* does not attack unprovoked */ /*64*/
	Bitfield(mtrapped,1);	/* trapped in a pit, web or bear trap */ /*65*/
	Bitfield(mleashed,1);	/* monster is on a leash */ /*66*/
	Bitfield(mtraitor,1);	/* Former pet that turned traitor */ /*67*/
	Bitfield(isshk,1);	/* is shopkeeper */ /*68*/
	Bitfield(isminion,1);	/* is a minion */ /*69*/
	
	Bitfield(isgd,1);	/* is guard */ /*70*/
	Bitfield(ispriest,1);	/* is a priest */ /*71*/
	Bitfield(iswiz,1);	/* is the Wizard of Yendor */ /*72*/
	Bitfield(wormno,5);	/* at most 31 worms on any level */ /*77*/
#define MAX_NUM_WORMS	32	/* should be 2^(wormno bitfield size) */
	Bitfield(moccupation,1);/* linked to your occupation */ /*78*/
	Bitfield(mclone,1);		/* is a clone of another monster (don't make death drop) */ /*79*/
	Bitfield(uhurtm,1);		/* injured by you */ /*80*/
	Bitfield(mhurtu,1);		/* has injured you */ /*81*/
	Bitfield(mattackedu,1);	/* attacked you on it's last turn */ /*82*/
	Bitfield(housealert,1);	/* won't accept house-based pacification */ /*83*/
	Bitfield(mspiritual,1);	/* Created by spirit power, doesn't count towards pet limit */ /*84*/
	Bitfield(artnum,2);		/* Created from an artifact. */ /*86*/
#define ART_PYGMALION	1
#define ART_GALATEA		2
	Bitfield(notame,1);/* can't be tamed */ /*87*/
	Bitfield(zombify,1);/* should rise as zombie */ /*88*/
	Bitfield(ispolyp,1);/* is a polypoid being */ /*89*/
	Bitfield(mdisrobe,1);/* disrobing */ /*90*/
	Bitfield(mgoatmarked,1);/* will be eaten by the goat if you kill it this turn */ /*91*/
	Bitfield(mpetitioner,1);/* already dead (shouldn't leave a corpse) */ /*92*/
	Bitfield(mdoubt,1);/* clerical spellcasting blocked */ /*93*/
	Bitfield(menvy,1);/* wants only others stuff */ /*94*/
	/*Monster madnesses*/
	Bitfield(msanctity,1);/* can't attack women */ /*95*/
	/** Needs improvement **/ Bitfield(mgluttony,1);/* eats food */ /*96*/
	Bitfield(mfrigophobia,1);/* won't cross ice */ /*97*/
	Bitfield(mcannibal,1);/* attacks same race, eats corpses */ /*98*/
	Bitfield(mrage,1);/* berserk plus morale*/ /*99*/
	Bitfield(margent,1);/* can't attack men, distracted by mirrors */ /*100*/
	Bitfield(msuicide,1);/* doesn't defend self */ /*101*/
	Bitfield(mnudist,1);/* takes off clothing */ /*102*/
	Bitfield(mophidio,1);/* attacked by snakes */ /*103*/
	Bitfield(marachno,1);/* attacked by spiders, can't attack women */ /*104*/
	Bitfield(mentomo,1);/* attacked by insects */ /*105*/
	Bitfield(mthalasso,1);/* attacked by seamonsters */ /*106*/
	Bitfield(mhelmintho,1);/* attacked by wormy things */ /*107*/
	Bitfield(mparanoid,1);/* attacks the wrong squares */ /*108*/
	Bitfield(mtalons,1);/* won't use items */ /*109*/
	Bitfield(mdreams,1);/* blasted by cthulhu while asleep */ /*110*/
	Bitfield(msciaphilia,1);/* elevated spell failure and lowered accuracy while not in shadows */ /*111*/
	Bitfield(mforgetful,1);/* can't use wizard spellcasting */ /*112*/
	Bitfield(mapostasy,1);/* can't use priest spellcasting */ /*113*/
	Bitfield(mtoobig,1);/* elevated spell failure */ /*114*/
	Bitfield(mrotting,1);/* spreads poison clouds */ /*115*/
	
	Bitfield(deadmonster,2); /* is DEADMONSTER */ /*116*/
#define DEADMONSTER_DEAD	0x1
#define DEADMONSTER_PURGE	0x2
#define DEADMONSTER(mon)	((mon) != &youmonst && (mon)->deadmonster)
	Bitfield(mnoise,1); /* made noise in the last turn (dochug) */ /*118*/
	Bitfield(marriving,1); /* monster is arriving on the level and should be placed when there's space */ /*119*/
	Bitfield(mflamemarked,1); /* monster was damaged by a silver flame weapon and will be sacced if they die */ /*120*/
	Bitfield(mspores,1); /* monster is being consumed by Zuggutmoy's spores */ /*121*/
	Bitfield(mformication,1); /* monster is covered in imaginary insects */ /*123*/
	Bitfield(mscorpions,1); /* monster is covered in stinging scorpions */ /*124*/
	Bitfield(myoumarked,1); /* monster was marked for cult sacrifice on your behalf */ /*125*/
	
	unsigned long long int 	seenmadnesses;	/* monster has seen these madnesses */
	
	char mbdrown;	/* drowning in blood */
	char mtaneggs;	/* tannin eggs */
	long mwait;/* if tame, won't follow between levels (turn last ordered to wait on) */
	int encouraged;	/* affected by Encourage song */
#define BASE_DOG_ENCOURAGED_MAX		7
	
	int entangled;/* The monster is entangled, and in what? */
#define imprisoned(mon)	((mon)->entangled == SHACKLES || ((mon)->mtrapped && t_at((mon)->mx, (mon)->my) && t_at((mon)->mx, (mon)->my)->ttyp == VIVI_TRAP))
#define noactions(mon)	((mon)->entangled || imprisoned(mon))
#define nonthreat(mon)	(imprisoned(mon) || has_template(mon, PLAGUE_TEMPLATE))
#define helpless(mon) (mon->msleeping || !(mon->mcanmove) || !(mon->mnotlaugh) || noactions(mon))	
#define helpless_still(mon) (mon->msleeping || !(mon->mcanmove) || noactions(mon))	
	long mstrategy;		/* for monsters with mflag3: current strategy */
#define STRAT_ARRIVE	0x40000000L	/* just arrived on current level */
#define STRAT_WAITFORU	0x20000000L
#define STRAT_CLOSE	0x10000000L
#define STRAT_WAITMASK	0x30000000L
#define STRAT_HEAL	0x08000000L
#define STRAT_GROUND	0x04000000L
#define STRAT_MONSTR	0x02000000L
#define STRAT_PLAYER	0x01000000L
#define STRAT_NONE	0x00000000L
#define STRAT_STRATMASK 0x0f000000L
#define STRAT_XMASK	0x00ff0000L
#define STRAT_YMASK	0x0000ff00L
#define STRAT_GOAL	0x000000ffL
#define STRAT_GOALX(s)	((xchar)((s & STRAT_XMASK) >> 16))
#define STRAT_GOALY(s)	((xchar)((s & STRAT_YMASK) >> 8))

	long mtrapseen;		/* bitmap of traps we've been trapped in */
	long mlstmv;		/* for catching up with lost time */
#ifndef GOLDOBJ
	long mgold;
#endif
	struct obj *minvent;

	struct obj *mw;
	struct obj *msw;
	long misc_worn_check;
	xchar weapon_check;
	xchar combat_mode;
#define	RANGED_MODE	0
#define HNDHND_MODE	1
	int meating;		/* monster is eating timeout */

/* faction to which monster belongs */
	int mfaction;
#define	FACTION_PADDING	1000	/* padding for Drow factions -- first one is 8 */
#define	INCUBUS_FACTION	FACTION_PADDING+1	/* male foocubus */
#define	SUCCUBUS_FACTION	FACTION_PADDING+2	/* female foocubus */
#define	LAMASHTU_FACTION	FACTION_PADDING+3	/* Lamashtu's angels */
#define	HOLYDEAD_FACTION	FACTION_PADDING+4	/* Angel-aligned undead */
#define	YENDORIAN_FACTION	FACTION_PADDING+5	/* The Yendorian army */
#define	GOATMOM_FACTION		FACTION_PADDING+6	/* Shubbie's faction */
#define	QUEST_FACTION		FACTION_PADDING+7	/* The Quest Leader's faction */
#define	ILSENSINE_FACTION	FACTION_PADDING+8	/* Ilsensine's faction, not allied with the Yendorian faction despite filling the same role */
#define	SEROPAENES_FACTION	FACTION_PADDING+9	/* Binder quest faction, not allied with the Yendorian faction despite filling the same role */
#define	YELLOW_FACTION	    FACTION_PADDING+10	/* Hastur faction, not the same as the Yendorian faction, despite filling the same role in the Madman quest */

/* template applied to monster to create a new-ish monster */
	int mtemplate;
#define	ZOMBIFIED		1	/* zombies */
#define	SKELIFIED		2	/* skeletons */
#define	CRYSTALFIED		3	/* crystal dead */
#define	FRACTURED		4	/* witness of the fracture */
#define	VAMPIRIC		5	/* vampirified monster */
#define	ILLUMINATED		6	/* illuminated monster */
#define	PSEUDONATURAL	7	/* far-realms-touched monster */
#define	TOMB_HERD		8	/* possessed statue */
#define	YITH			9	/* possessed by the great race of yith */
#define	CRANIUM_RAT		10	/* psychic rat */
#define	MISTWEAVER		11	/* daughters of shubie */
#define	DELOUSED		12	/* android flag: parasite is killed, but not host */
#define	M_BLACK_WEB		13	/* Zombie with a shadow blade attack */
#define	M_GREAT_WEB		14	/* Has a stronger shadow blade attack */
#define	SLIME_REMNANT	15	/* slimey, like an ancient of corruption */
#define	YELLOW_TEMPLATE	16	/* causes sleep and damages sanity (unimplemented) */
#define	DREAM_LEECH		17	/* sucks mental atributes */
#define	MAD_TEMPLATE	18	/* mad angel template */
#define	FALLEN_TEMPLATE	19	/* fallen angel template */
#define WORLD_SHAPER	20	/* plane-of-earth nastify */
#define MINDLESS		21	/* brain eaten by mind flayers */
#define POISON_TEMPLATE	22	/* turned evil by poison */
#define MOLY_TEMPLATE	23	/* off-turn snake-bite + insight */
#define PLAGUE_TEMPLATE	24	/* suffering from a life-drain plague, cure to recruit */
#define SPORE_ZOMBIE	25	/* fungus zombie */
#define CORDYCEPS		26	/* spore shedder */
#define MAXTEMPLATE	CORDYCEPS

//define	HALF_DEMON	FACTION_PADDING+1	/* half-demon  ??? */
//define	HALF_DEVIL	FACTION_PADDING+2	/* half-devil  ??? */
//define	HALF_DRAGON	FACTION_PADDING+3	/* half-dragon ??? */

	int summonpwr;	/* cumulative HD of summoned creatures */
	//hatching situation
	//AT_LAYEGG, PM_HATCHLING_NAME, x, d
	//timetohatch
	//larvatime (larva effects default or by hatchling, time default or by hatchling)
	//	time:  AT_LARVA+x, where x=time, and AT_LARVA is large, 10000 or so.
	//utype = PM_HATCHLING_NAME
	//inventory
	long mvar1;
#define	mvar_paleWarning	mvar1
#define	mvar_witchID	mvar1
#define	mvar_suryaID	mvar1
#define	mvar_huskID	mvar1
#define	mvar_syllable	mvar1
#define	mvar_vector	mvar1
#define	mvar_spList_1	mvar1
#define	mvar_hdBreath	mvar1
#define	mvar_dreadPrayer_cooldown	mvar1
#define	mvar_dracaePreg	mvar1
#define	mvar_dollTypes	mvar1
#define	DOLLMAKER_EFFIGY		0x00001L
#define	DOLLMAKER_JUMPING		0x00002L
#define	DOLLMAKER_FRIENDSHIP	0x00004L
#define	DOLLMAKER_CHASTITY		0x00008L
#define	DOLLMAKER_CLEAVING		0x00010L
#define	DOLLMAKER_SATIATION		0x00020L
#define	DOLLMAKER_HEALTH		0x00040L
#define	DOLLMAKER_HEALING		0x00080L
#define	DOLLMAKER_DESTRUCTION	0x00100L
#define	DOLLMAKER_MEMORY		0x00200L
#define	DOLLMAKER_BINDING		0x00400L
#define	DOLLMAKER_PRESERVATION	0x00800L
#define	DOLLMAKER_QUICK_DRAW	0x01000L
#define	DOLLMAKER_WAND_CHARGE	0x02000L
#define	DOLLMAKER_STEALING		0x04000L
#define	DOLLMAKER_MOLLIFICATION	0x08000L
#define	DOLLMAKER_CLEAR_THOUGHT	0x10000L
#define	DOLLMAKER_MIND_BLASTS	0x20000L
#define	MAX_DOLL_MASK	DOLLMAKER_MIND_BLASTS
#define	mvar_tanninType	mvar1
#define	mvar_ancient_breath_cooldown	mvar1
#define	mvar_deminymph_role	mvar1
#define	mvar_flask_charges	mvar1
#define MAX_FLASK_CHARGES(mtmp) (mtmp->m_lev/3)
	long mvar2;
#define	mvar_dracaePregTimer	mvar2
#define	mvar_spList_2	mvar2
#define	mvar_dreadPrayer_progress	mvar2
#define	mvar_attack_pm	mvar2
	long mvar3;
#define	mvar_conversationTracker	mvar3
#define	mvar_lifesigns	mvar3
#define has_lifesigns(mon)	(mon->mtyp != PM_CHAOS && mon->mvar_lifesigns)

	struct ls_t * light;

	struct timer * timed;

	union mextra * mextra_p;
};

/*
 * Out-of-date mextra information that hasn't been properly deprecated yet.
 *	isgd	 ->	struct egd
 *	ispriest ->	struct epri
 *	isshk	 ->	struct eshk
 *	isminion ->	struct emin
 *	mtame	 ->	struct edog
 * OLD:
 * Since at most one structure can be indicated in this manner, it is not
 * possible to tame any creatures using the other structures (the only
 * exception being the guardian angels which are tame on creation).
 */

#define dealloc_monst(mon) free((genericptr_t)(mon))

/* these are in mspeed */
#define MSLOW 1		/* slow monster */
#define MFAST 2		/* speeded monster */

#define MON_WEP(mon)	((mon)->mw)
#define MON_NOWEP(mon)	((mon)->mw = (struct obj *)0)
#define MON_SWEP(mon)	((mon)->msw)
#define MON_NOSWEP(mon)	((mon)->msw = (struct obj *)0)

#define MIGRATINGMONSTER(mon)	((mon) != &youmonst && !(mon)->mx && !(mon)->my)

#endif /* MONST_H */
