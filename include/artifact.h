/*	SCCS Id: @(#)artifact.h 3.4	1995/05/31	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef ARTIFACT_H
#define ARTIFACT_H

#define MZ_DEFAULT	-1
#define MT_DEFAULT	-1
#define WT_DEFAULT	-1
#define WT_SPECIAL  -2  /* must be handled in artifact_weight() in artifact.c */

#define TIER_S	8	/* Is somehow superior to all others */
#define TIER_A	6	/* Preferrable endgame artifact */
#define TIER_B	4	/* Good enough artifact to use by endgame */
#define TIER_C	3	/* You'll want to switch out for a better artifact if you can */
#define TIER_D	2	/* Situationally useful; enough to carry around, at least */
#define TIER_F	1	/* Often not worth using at all */
#define NO_TIER	0	/* Not tierable */

#define ARTG_NOGEN		0x0001L /* does not randomly generate */
#define ARTG_NOWISH		0x0002L /* cannot be wished for */
#define ARTG_NAME		0x0004L /* can be #named */
#define ARTG_GIFT		0x0008L /* favoured early gift for a player */
#define ARTG_INHER		0x0010L /* allowable for an Inheritor to start with */
#define ARTG_MAJOR		0x0020L /* artifact evades the grasp of the unworthy */
#define ARTG_NOCNT		0x0040L /* should not be sacrificable to Priests of an Unknown God ??? */
#define ARTG_FXALGN		0x0080L /* doesn't change alignment to match a role's */

#define ARTA_HATES		0x00000001L /* hates specific kinds of foes; only deals bonus damage to them (cross-aligned is a possibility too) */
#define ARTA_CROSSA		0x00000002L /* artifact hates cross-aligned foes (as well as any specified monster flags) -- use with ARTA_HATES */
#define ARTA_DEXPL		0x00000004L /* weapon dice explode */
#define ARTA_DLUCK		0x00000008L /* weapon dice are luck-biased -- combine with DEXPL with extreme caution! */
#define ARTA_POIS		0x00000010L /* permanently poisoned */
#define ARTA_SILVER		0x00000020L /* deals silver damage, even though it isn't made of silver */
#define ARTA_VORPAL		0x00000040L /* vorpal -- beheads, cleaves, crushes, or otherwise occasionally instakills foes */
#define ARTA_CANCEL		0x00000080L	/* attempts to cancel (hated) creatures on a 1-2 dieroll */
#define ARTA_TENTROD	0x00000100L /* does tentacle-rod bonus effects */
#define ARTA_THREEHEAD	0x00000200L /* attacks 1-3x with one swing, stunning if all three hit */
#define ARTA_MAGIC		0x00000400L /* does Magicbane-like bonus effects -- damage must be special cased in mb_hit */
#define ARTA_DRAIN		0x00000800L /* drains levels from defender and heals attacker */
#define ARTA_BRIGHT		0x00001000L /* turns gremlins to dust and trolls to stone */
#define ARTA_BLIND		0x00002000L /* blinds defender */
#define ARTA_SHINING	0x00004000L /* phases armor; can hit shades */
#define ARTA_SHATTER	0x00008000L /* shatter's defender's weapon */
#define ARTA_DISARM		0x00010000L /* disarms opponent */
#define ARTA_STEAL		0x00020000L /* steals item from defender */
#define ARTA_HASTE		0x00040000L /* hitting defender grants movement to attacker */
#define ARTA_EXPLFIRE	0x00080000L /* fire explosion; 25% chance */
#define ARTA_EXPLCOLD	0x00100000L /* cold explosion; 25% chance */
#define ARTA_EXPLELEC	0x00200000L /* elec explosion; 25% chance */
#define ARTA_KNOCKBACK	0x00400000L /* knockback; 25% chance */
#define ARTA_EXPLFIREX	0x00800000L /* fire explosion; 100% chance*/
#define ARTA_EXPLCOLDX	0x01000000L /* cold explosion; 100% chance*/
#define ARTA_EXPLELECX	0x02000000L /* elec explosion; 100% chance*/
#define ARTA_KNOCKBACKX	0x04000000L /* knockback; 100% chance*/

#define ARTP_SEEK		0x0001L /* helps you search, ie, adds enhancement bonus to attempts -- only coded for mainhand weapons */
#define ARTP_NOCALL		0x0002L /* prevents demons from being gated in */
#define ARTP_NOWERE		0x0004L /* protects from lycanthropy and lycanthrope summoning */
#define ARTP_FORCESIGHT	0x0008L /* you may see even when blinded. not a real property, so special casing required when used */
#define	ARTP_LIGHTEN	0x0010L /* increases the weight you can haul. Currently only includes Shirts, Cloaks, and body armor.  See do_wear.c and hack.c */
#define	ARTP_WCATRIB	0x0020L /* when worn above armor, increases Cha.  Under armor, increases Wis. */

#define ARTI_ENGRAVE	0x0001L /* can be used to engrave quickly like an athame */
#define ARTI_DIG		0x0002L /* can be used to dig like a pickaxe */
#define ARTI_LIGHT		0x0004L	/* is a lightsource while wielded/worn */
#define ARTI_PERMALIGHT	0x0008L /* is a permanent lightsource, even when just lying on the floor */
#define ARTI_MANDALA	0x0010L /* (r)ead object to benifit as from unihorn ONLY CHECKED FOR SHIRTS */
#define ARTI_PLUSSEV	0x0020L /* artifact armor can be enchanted to +7 easily */
#define ARTI_FEAR		0x0040L /* acts as a scroll of scare monster while on floor */
#define ARTI_BLOODTHRST	0x0080L /* wants to attack peaceful and tame creatures */
#define ARTI_SPEAK		0x0100L /* speaks rumours */
#define ARTI_LUCK		0x0200L /* acts as a luckstone */
#define ARTI_PLUSTEN	0x0400L /* can be enchanted to plus 10 */

//#define SPFX2_NINJA		0x0000008L	/* throws from 1-your skill level ninja stars after each attack */
//#define SPFX3_CARCAP	0x0000020L	/* increases carrying capacity when carried */


/*Necronomicon flags*/
//	-Summon 
#define	S_BYAKHEE		0x00000001L
#define	S_NIGHTGAUNT	0x00000002L
#define	S_SHOGGOTH		0x00000004L
#define	S_OOZE			0x00000008L
//	-Spells
#define	SP_PROTECTION	0x00000010L
#define	SP_TURN_UNDEAD	0x00000020L

#define	SP_FORCE_BOLT	0x00000040L
#define	SP_DRAIN_LIFE	0x00000080L
#define	SP_DEATH		0x00000100L

#define	SP_DETECT_MNSTR	0x00000200L
#define	SP_CLAIRVOYANCE	0x00000400L
#define	SP_DETECT_UNSN	0x00000800L
#define	SP_IDENTIFY		0x00001000L

#define	SP_CONFUSE		0x00002000L
#define	SP_CAUSE_FEAR	0x00004000L

#define	SP_LEVITATION	0x00008000L

#define	SP_STONE_FLESH	0x00010000L

#define	SP_CANCELLATION	0x00020000L

#define	SP_COMBAT		0x00040000L
#define	SP_HEALTH		0x00080000L

#define	SUM_DEMON		0x00100000L
#define	S_DEVIL			0x00200000L

#define	R_Y_SIGN		0x00400000L	//yellow sign
#define	R_WARDS			0x00800000L	//eye, queen
#define	R_ELEMENTS		0x01000000L	//Cthuqha, Ithaqua, and Karakal
#define	R_NAMES_1		0x02000000L	//Half of the spirits (based on random order)
#define	R_NAMES_2		0x04000000L	//Other half of the spirits


#define	LAST_PAGE		0x40000000L

//	-Summon 
#define	SELECT_BYAKHEE		1
#define	SELECT_NIGHTGAUNT	SELECT_BYAKHEE + 1
#define	SELECT_SHOGGOTH		SELECT_NIGHTGAUNT + 1
#define	SELECT_OOZE			SELECT_SHOGGOTH + 1
#define	SELECT_DEMON		SELECT_OOZE + 1
#define	SELECT_DEVIL		SELECT_DEMON + 1
//	-Spells
#define	SELECTED_SPELL		SELECT_DEVIL + 1
#define	SELECT_PROTECTION	SELECTED_SPELL
#define	SELECT_TURN_UNDEAD	SELECT_PROTECTION + 1

#define	SELECT_FORCE_BOLT	SELECT_TURN_UNDEAD + 1
#define	SELECT_DRAIN_LIFE	SELECT_FORCE_BOLT + 1
#define	SELECT_DEATH		SELECT_DRAIN_LIFE + 1

#define	SELECT_DETECT_MNSTR	SELECT_DEATH + 1
#define	SELECT_CLAIRVOYANCE	SELECT_DETECT_MNSTR + 1
#define	SELECT_DETECT_UNSN	SELECT_CLAIRVOYANCE + 1
#define	SELECT_IDENTIFY		SELECT_DETECT_UNSN + 1

#define	SELECT_CONFUSE		SELECT_IDENTIFY + 1
#define	SELECT_CAUSE_FEAR	SELECT_CONFUSE + 1

#define	SELECT_LEVITATION	SELECT_CAUSE_FEAR + 1

#define	SELECT_STONE_FLESH	SELECT_LEVITATION + 1

#define	SELECT_CANCELLATION	SELECT_STONE_FLESH + 1

#define	SELECTED_SPECIAL	SELECT_CANCELLATION + 1
#define	SELECT_COMBAT		SELECTED_SPECIAL
#define	SELECT_HEALTH		SELECT_COMBAT + 1
#define	SELECT_SIGN			SELECT_HEALTH + 1
#define	SELECT_WARDS		SELECT_SIGN + 1
#define	SELECT_ELEMENTS		SELECT_WARDS + 1
#define	SELECT_SPIRITS1		SELECT_ELEMENTS + 1
#define	SELECT_SPIRITS2		SELECT_SPIRITS1 + 1


#define SELECT_STUDY		SELECT_SPIRITS2 + 1

#define SELECT_WHISTLE		1
#define SELECT_LEASH		SELECT_WHISTLE + 1
#define SELECT_SADDLE		SELECT_LEASH + 1
#define SELECT_TRIPE		SELECT_SADDLE + 1
#define SELECT_APPLE		SELECT_TRIPE + 1
#define SELECT_BANANA		SELECT_APPLE + 1

#define COMMAND_RAPIER			 1
#define COMMAND_AXE				 2
#define COMMAND_MACE			 3
#define COMMAND_SPEAR			 4
#define COMMAND_LANCE			 5

#define COMMAND_D_GREAT			 6
#define COMMAND_MOON_AXE		 7
#define COMMAND_KHAKKHARA		 8
#define COMMAND_DROVEN_SPEAR	 9
#define COMMAND_D_LANCE			10

#define COMMAND_E_SWORD			11
#define COMMAND_E_SICKLE		12
#define COMMAND_E_MACE			13
#define COMMAND_E_SPEAR			14
#define COMMAND_E_LANCE			15

#define COMMAND_SCIMITAR		16
#define COMMAND_WHIP			17

#define COMMAND_ATLATL			18

#define COMMAND_SABER			19
#define COMMAND_RING			20
#define COMMAND_ARM				21
#define COMMAND_RAY				22
#define COMMAND_BFG				23
#define COMMAND_ANNULUS			24

#define COMMAND_LADDER			25
#define COMMAND_CLAIRVOYANCE	26
#define COMMAND_FEAR			27
#define COMMAND_LIFE			28
#define COMMAND_KNEEL			29

#define COMMAND_STRIKE			30

#define COMMAND_AMMO			31

#define COMMAND_BELL			32
#define COMMAND_BULLETS			33
#define COMMAND_ROCKETS			34
#define COMMAND_BEAM			35
#define COMMAND_ANNUL			36
#define COMMAND_CHARGE			37

#define COMMAND_POISON			38
#define COMMAND_DRUG  			39
#define COMMAND_STAIN 			40
#define COMMAND_ENVENOM			41
#define COMMAND_FILTH 			42

#define COMMAND_IMPROVE_WEP     43
#define COMMAND_IMPROVE_ARM     44

#define COMMAND_DEATH           45

#define COMMAND_GRAY            46
#define COMMAND_SILVER          47
#define COMMAND_MERCURIAL       48
#define COMMAND_SHIMMERING      49
#define COMMAND_DEEP            50
#define COMMAND_RED             51
#define COMMAND_WHITE           52
#define COMMAND_ORANGE          53
#define COMMAND_BLACK           54
#define COMMAND_BLUE            55
#define COMMAND_GREEN           56
#define COMMAND_YELLOW          57

struct artifact {
	/* Description */
	int otyp;			/* base artifact type */
	const char * name;	/* artifact name */
	const char * desc;	/* appearance when unknown */
	long cost;			/* price when sold to hero (default 100 x base cost) */
	int material;		/* default material */
	int size;			/* default size */
	int weight;			/* default weight */

	/* For Whom */
	aligntyp    alignment;	/* alignment of bequeathing gods */
	short	    role;		/* character role associated with */
	short	    race;		/* character race associated with */
	short		giftval;	/* SUBJECTIVE measure of how good an artifact gift it is */
	unsigned long gflags;	/* how it should generate */

	/* Versus */
	unsigned long mtype;	/* monster symbol */
	unsigned long mflagsm;	/* Monster Motility boolean bitflags */
	unsigned long mflagst;	/* Monster Thoughts and behavior boolean bitflags */
    unsigned long mflagsf;	/* Monster Fighting skills boolean bitflags */
	unsigned long mflagsb;	/* Monster Body plan boolean bitflags */
	unsigned long mflagsg;	/* Monster Game mechanics and bookkeeping boolean bitflags */
	unsigned long mflagsa;	/* Monster rAce boolean bitflags */
	unsigned long mflagsv;	/* Monster Vision boolean bitflags */

	/* Offense */
	int adtyp;				/* damage type */
	int accuracy;			/* increased accuracy */
	int damage;				/* damage -- positive numbers for dX damage, 0 for double damage */
	unsigned long aflags;	/* offensive artifact properties */

	/* Worn */
#define MAXARTPROP 8
	uchar wprops[MAXARTPROP];	/* properties granted while wielded/worn */
	unsigned long wflags;		/* special effect while wielding/wearing*/
	/* Carried */
	uchar cprops[MAXARTPROP];	/* properties granted while carried */
	unsigned long cflags;		/* special effect while carrying */
	/* Intrinsic */
	uchar inv_prop;				/* property obtained by invoking artifact OR invokable power */
	unsigned long iflags;		/* special effect intrinsic to the artifact */
};

struct artinstance{
	boolean exists; /*Has the artifact been generated*/
	//Per-artifact variables. Access them via artinstance[ART_NUM].foo
	long avar1;
#define SnSd1 avar1
#define ZangetsuSafe avar1
#define RoSPkills avar1
#define BoISspell avar1
#define RRSember avar1
#define BLactive avar1
#define PlagueDuration avar1
	long avar2;
#define SnSd2 avar2
#define RoSPflights avar2
#define RRSlunar avar2
#define PlagueDoOnHit avar2
	long avar3;
#define SnSd3 avar3
	long avar4;
#define SnSd3duration avar4
};


#define get_artifact(o) \
		(((o)&&(o)->oartifact) ? &artilist[(o)->oartifact] : 0)

extern struct artinstance artinstance[];
extern struct artifact artilist[];

/* invoked properties with special powers */
#define TAMING		(LAST_PROP+1)
#define HEALING		(LAST_PROP+2)
#define ENERGY_BOOST	(LAST_PROP+3)
#define UNTRAP		(LAST_PROP+4)
#define CHARGE_OBJ	(LAST_PROP+5)
#define LEV_TELE	(LAST_PROP+6)
#define CREATE_PORTAL	(LAST_PROP+7)
#define ENLIGHTENING	(LAST_PROP+8)
#define CREATE_AMMO	(LAST_PROP+9)
#define SMOKE_CLOUD    (LAST_PROP+10)
#define CANNONADE	(LAST_PROP+11)
#define MAGICALDEVICE	(LAST_PROP+12)
#define SHADOW_FLARE	(LAST_PROP+13)
#define BLIZAGA	(LAST_PROP+14)
#define FIRAGA	(LAST_PROP+15)
#define THUNDAGA	(LAST_PROP+16)
#define QUAKE	(LAST_PROP+17)
#define PETMASTER	(LAST_PROP+18)
#define LEADERSHIP	(LAST_PROP+19)//from GreyKnight's Clarent mod
#define BLESS	(LAST_PROP+20)
#define SEVENFOLD	(LAST_PROP+21)
#define SATURN	(LAST_PROP+22)
#define PLUTO	(LAST_PROP+23)
#define SPEED_BANKAI	(LAST_PROP+24)
#define ICE_SHIKAI	(LAST_PROP+25)
#define FIRE_SHIKAI	(LAST_PROP+26)
#define MANDALA	(LAST_PROP+27)
#define DRAIN_MEMORIES	(LAST_PROP+28)
#define SLAY_LIVING	(LAST_PROP+29)
#define NECRONOMICON	(LAST_PROP+30)
#define BLADESONG	(LAST_PROP+31)
#define PHASING		(LAST_PROP+32)
#define OBJECT_DET	(LAST_PROP+33)
#define TELEPORT_SHOES	(LAST_PROP+34)
#define LORDLY	(LAST_PROP+35)
#define VOID_CHIME	(LAST_PROP+36)
#define INFINITESPELLS	(LAST_PROP+37)
#define SPIRITNAMES	(LAST_PROP+38)
#define RAISE_UNDEAD	(LAST_PROP+39)
#define FALLING_STARS	(LAST_PROP+40)
#define THEFT_TYPE		(LAST_PROP+41)
#define ANNUL			(LAST_PROP+42)
#define ARTI_REMOVE_CURSE	(LAST_PROP+43)
#define SUMMON_UNDEAD   (LAST_PROP+44)
#define DEATH_GAZE      (LAST_PROP+45)
#define SMITE           (LAST_PROP+46)
#define PROTECT         (LAST_PROP+47)
#define TRAP_DET        (LAST_PROP+48)
#define UNBIND_SEALS    (LAST_PROP+49)
#define HEAL_PETS       (LAST_PROP+50)
#define FREE_SPELL      (LAST_PROP+51)
#define BURN_WARD       (LAST_PROP+52)
#define FAST_TURNING    (LAST_PROP+53)
#define FIRE_BLAST      (LAST_PROP+54)
#define SELF_POISON     (LAST_PROP+55)
#define ADD_POISON      (LAST_PROP+56)
#define TOWEL_ITEMS     (LAST_PROP+57)
#define MAJ_RUMOR       (LAST_PROP+58)
#define ARTIFICE        (LAST_PROP+59)
#define SUMMON_PET      (LAST_PROP+60)
#define LIFE_DEATH      (LAST_PROP+61)
#define PRISMATIC       (LAST_PROP+62)
#define STEAL           (LAST_PROP+63)
#define SUMMON_VAMP     (LAST_PROP+64)
#define COLLECT_TAX     (LAST_PROP+65)
#define ALTMODE         (LAST_PROP+66)
#define AEGIS           (LAST_PROP+67)
#define WATER           (LAST_PROP+68)
#define SINGING         (LAST_PROP+69)
#define WIND_PETS	    (LAST_PROP+70)
#define DEATH_TCH       (LAST_PROP+71)
#define SKELETAL_MINION (LAST_PROP+72)
#define ORACLE          (LAST_PROP+73)
#define FILTH_ARROWS    (LAST_PROP+74)
#define ALLSIGHT        (LAST_PROP+75)
#define INVOKE_DARK     (LAST_PROP+76)
#define QUEST_PORTAL    (LAST_PROP+77)
#define STONE_DRAGON    (LAST_PROP+78)
#define MAD_KING        (LAST_PROP+79)
#define RINGED_SPEAR    (LAST_PROP+80)
#define RINGED_ARMOR    (LAST_PROP+81)
#define BLOODLETTER     (LAST_PROP+82)


#define MASTERY_ARTIFACT_LEVEL 20

#define has_named_mastery_artifact() (\
            art_already_exists(ART_TRUSTY_ADVENTURER_S_WHIP) ||\
            art_already_exists(ART_LOG_OF_THE_CURATOR) ||\
            art_already_exists(ART_FORCE_PIKE_OF_THE_RED_GUAR) ||\
            art_already_exists(ART_GAUNTLETS_OF_THE_BERSERKER) ||\
            art_already_exists(ART_DECLARATION_OF_THE_APOSTAT) ||\
            art_already_exists(ART_SOUL_LENS) ||\
            art_already_exists(ART_SEAL_OF_THE_SPIRITS) ||\
            art_already_exists(ART_TORCH_OF_ORIGINS) ||\
            art_already_exists(ART_STRIPED_SHIRT_OF_THE_MURDE) ||\
            art_already_exists(ART_STRIPED_SHIRT_OF_THE_THIEF) ||\
            art_already_exists(ART_STRIPED_SHIRT_OF_THE_FALSE) ||\
            art_already_exists(ART_SCALPEL_OF_LIFE_AND_DEATH) ||\
            art_already_exists(ART_GAUNTLETS_OF_THE_HEALING_H) ||\
            art_already_exists(ART_RING_OF_HYGIENE_S_DISCIPLE) ||\
            art_already_exists(ART_COPE_OF_THE_ELDRITCH_KNIGH) ||\
            art_already_exists(ART_SHIELD_OF_THE_PALADIN) ||\
            art_already_exists(ART_BOOZE_OF_THE_DRUNKEN_MASTE) ||\
            art_already_exists(ART_WRAPPINGS_OF_THE_SACRED_FI) ||\
            art_already_exists(ART_KHAKKHARA_OF_THE_MONKEY) ||\
            art_already_exists(ART_MARK_OF_THE_RIGHTFUL_SCION) ||\
            art_already_exists(ART_GAUNTLETS_OF_THE_DIVINE_DI) ||\
            art_already_exists(ART_MACE_OF_THE_EVANGELIST) ||\
            art_already_exists(ART_DART_OF_THE_ASSASSIN) ||\
            art_already_exists(ART_SWORD_OF_THE_KLEPTOMANIAC) ||\
            art_already_exists(ART_HELM_OF_THE_ARCANE_ARCHER) ||\
            art_already_exists(ART_FIGURINE_OF_PYGMALION) ||\
            art_already_exists(ART_FIGURINE_OF_GALATEA) ||\
            art_already_exists(ART_HELM_OF_THE_NINJA) ||\
            art_already_exists(ART_TOWEL_OF_THE_INTERSTELLAR_) ||\
            art_already_exists(ART_ENCYCLOPEDIA_GALACTICA) ||\
            art_already_exists(ART_TWIG_OF_YGGDRASIL) ||\
            art_already_exists(ART_SADDLE_OF_BRYNHILDR) ||\
            art_already_exists(ART_STAFF_OF_WILD_MAGIC) ||\
            art_already_exists(ART_ROBE_OF_THE_ARCHMAGI) ||\
            art_already_exists(ART_FORGE_HAMMER_OF_THE_ARTIFI) ||\
            art_already_exists(ART_RING_OF_LOLTH) ||\
            art_already_exists(ART_BULWARK_OF_THE_DWARVEN_DEF) ||\
            art_already_exists(ART_RING_OF_LOLTH) ||\
            art_already_exists(ART_NARYA) ||\
            art_already_exists(ART_NENYA) ||\
            art_already_exists(ART_VILYA) ||\
            art_already_exists(ART_HAT_OF_THE_GIANT_KILLER) ||\
            art_already_exists(ART_PRISMATIC_DRAGON_PLATE) ||\
            art_already_exists(ART_FOOTPRINTS_IN_THE_LABYRINT) ||\
            art_already_exists(ART_TRAPPINGS_OF_THE_GRAVE)\
)

#define is_nameable_artifact(a) (\
			(a->gflags & ARTG_NAME) != 0\
			|| (a == &artilist[ART_SNICKERSNEE] && Role_if(PM_TOURIST) && (u.ulevel > 18 || u.uevent.qcompleted) )\
			|| (a == &artilist[ART_KUSANAGI_NO_TSURUGI] && Role_if(PM_SAMURAI) && u.ulevel >= 22 )\
			|| (a == &artilist[ART_GRANDMASTER_S_ROBE] && P_SKILL(P_MARTIAL_ARTS) >= P_GRAND_MASTER && u.ulevel >= 30 )\
			|| (a == &artilist[ART_FIRST_KEY_OF_CHAOS] && \
							(art_already_exists(ART_THIRD_KEY_OF_CHAOS) \
							||  u.ualign.type == A_CHAOTIC ) \
				)\
			)
/* artifact has no specific material or size, eg "silver Grimtooth" */
#define is_malleable_artifact(a) (is_nameable_artifact((a)) || (a) == &artilist[ART_EXCALIBUR] || (a) == &artilist[ART_GUNGNIR] || (a) == &artilist[ART_DIRGE])

#define is_living_artifact(obj) ((obj)->oartifact == ART_TENTACLE_ROD || (obj)->oartifact == ART_DRAGONHEAD_SHIELD || (obj)->oartifact == ART_CRUCIFIX_OF_THE_MAD_KING || (obj)->oartifact == ART_RITUAL_RINGED_SPEAR || (obj)->oartifact == ART_RINGED_BRASS_ARMOR)

#define is_mastery_artifact_nameable(a) (\
            /* Mastery artifacts */\
            /* Archeologist */\
			((a) == &artilist[ART_TRUSTY_ADVENTURER_S_WHIP] && (Role_if(PM_ARCHEOLOGIST) || Pantheon_if(PM_ARCHEOLOGIST)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_LOG_OF_THE_CURATOR] && (Role_if(PM_ARCHEOLOGIST) || Pantheon_if(PM_ARCHEOLOGIST)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Anachrononaut */\
            || ((a) == &artilist[ART_FORCE_PIKE_OF_THE_RED_GUAR] && (Role_if(PM_ANACHRONONAUT) || Pantheon_if(PM_ANACHRONONAUT)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Barbarian */\
            || ((a) == &artilist[ART_GAUNTLETS_OF_THE_BERSERKER] && (Role_if(PM_BARBARIAN) || Pantheon_if(PM_BARBARIAN)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Binder */\
            || ((a) == &artilist[ART_DECLARATION_OF_THE_APOSTAT] && (Role_if(PM_EXILE) || Pantheon_if(PM_EXILE)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !u.sealCounts &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_SOUL_LENS] && (Role_if(PM_EXILE) || Pantheon_if(PM_EXILE)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_SEAL_OF_THE_SPIRITS] && (Role_if(PM_EXILE) || Pantheon_if(PM_EXILE)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Caveman/Cavewoman */\
            || ((a) == &artilist[ART_TORCH_OF_ORIGINS] && (Role_if(PM_CAVEMAN) || Pantheon_if(PM_CAVEMAN)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Convict */\
            || ((a) == &artilist[ART_STRIPED_SHIRT_OF_THE_MURDE] && (Role_if(PM_CONVICT) || Pantheon_if(PM_CONVICT)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_STRIPED_SHIRT_OF_THE_THIEF] && (Role_if(PM_CONVICT) || Pantheon_if(PM_CONVICT)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_STRIPED_SHIRT_OF_THE_FALSE] && (Role_if(PM_CONVICT) || Pantheon_if(PM_CONVICT)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Healer */\
            || ((a) == &artilist[ART_SCALPEL_OF_LIFE_AND_DEATH] && (Role_if(PM_HEALER) || Pantheon_if(PM_HEALER)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_GAUNTLETS_OF_THE_HEALING_H] && (Role_if(PM_HEALER) || Pantheon_if(PM_HEALER)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_RING_OF_HYGIENE_S_DISCIPLE] && (Role_if(PM_HEALER) || Pantheon_if(PM_HEALER)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Knight */\
            || ((a) == &artilist[ART_COPE_OF_THE_ELDRITCH_KNIGH] && (Role_if(PM_KNIGHT) || Pantheon_if(PM_KNIGHT)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
            /* Monk */\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_BOOZE_OF_THE_DRUNKEN_MASTE] && (Role_if(PM_MONK) || Pantheon_if(PM_MONK)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_WRAPPINGS_OF_THE_SACRED_FI] && (Role_if(PM_MONK) || Pantheon_if(PM_MONK)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_KHAKKHARA_OF_THE_MONKEY] && (Role_if(PM_MONK) || Pantheon_if(PM_MONK)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Nobleman/Noblewoman */\
            /* Priest/Priestess */\
            || ((a) == &artilist[ART_MACE_OF_THE_EVANGELIST] && (Role_if(PM_PRIEST) || Pantheon_if(PM_PRIEST)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !art_already_exists(ART_GAUNTLETS_OF_THE_DIVINE_DI))\
            || ((a) == &artilist[ART_GAUNTLETS_OF_THE_DIVINE_DI] && (Role_if(PM_PRIEST) || Pantheon_if(PM_PRIEST)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Pirate */\
            /* Rogue */\
            || ((a) == &artilist[ART_DART_OF_THE_ASSASSIN] && (Role_if(PM_ROGUE) || Pantheon_if(PM_ROGUE)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_SWORD_OF_THE_KLEPTOMANIAC] && (Role_if(PM_ROGUE) || Pantheon_if(PM_ROGUE)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Ranger */\
            || ((a) == &artilist[ART_HELM_OF_THE_ARCANE_ARCHER] && (Role_if(PM_RANGER) || Role_if(PM_BARD) || Pantheon_if(PM_RANGER)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_FIGURINE_OF_PYGMALION] && (Role_if(PM_RANGER) || Role_if(PM_BARD) || Pantheon_if(PM_RANGER)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_FIGURINE_OF_GALATEA] && (Role_if(PM_RANGER) || Role_if(PM_BARD) || Pantheon_if(PM_RANGER)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Samurai */\
            || ((a) == &artilist[ART_HELM_OF_THE_NINJA] && (Role_if(PM_SAMURAI) || Pantheon_if(PM_SAMURAI)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Tourist */\
            || ((a) == &artilist[ART_TOWEL_OF_THE_INTERSTELLAR_] && (Role_if(PM_TOURIST) || Pantheon_if(PM_TOURIST)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_ENCYCLOPEDIA_GALACTICA] && (Role_if(PM_TOURIST) || Pantheon_if(PM_TOURIST)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Troubadour */\
            /* Valkyrie */\
            || ((a) == &artilist[ART_TWIG_OF_YGGDRASIL] && (Role_if(PM_VALKYRIE) || Pantheon_if(PM_VALKYRIE)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_SADDLE_OF_BRYNHILDR] && (Role_if(PM_VALKYRIE) || Pantheon_if(PM_VALKYRIE)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Wizard */\
            || ((a) == &artilist[ART_STAFF_OF_WILD_MAGIC] && (Role_if(PM_WIZARD) || Pantheon_if(PM_WIZARD)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_ROBE_OF_THE_ARCHMAGI] && (Role_if(PM_WIZARD) || Pantheon_if(PM_WIZARD)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_FORGE_HAMMER_OF_THE_ARTIFI] && (Role_if(PM_WIZARD) || Pantheon_if(PM_WIZARD)) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Drow */\
            || ((a) == &artilist[ART_RING_OF_LOLTH] && Race_if(PM_DROW) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Dwarf */\
            || ((a) == &artilist[ART_BULWARK_OF_THE_DWARVEN_DEF] && Race_if(PM_DWARF) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Elf */\
            || ((a) == &artilist[ART_NARYA] && Race_if(PM_ELF) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_NENYA] && Race_if(PM_ELF) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            || ((a) == &artilist[ART_VILYA] && Race_if(PM_ELF) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Gnome */\
            || ((a) == &artilist[ART_HAT_OF_THE_GIANT_KILLER] && Race_if(PM_ELF) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Half-Dragon */\
            || ((a) == &artilist[ART_PRISMATIC_DRAGON_PLATE] && Race_if(PM_HALF_DRAGON) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Human */\
            /* Incantifier */\
            || ((a) == &artilist[ART_FOOTPRINTS_IN_THE_LABYRINT] && Race_if(PM_INCANTIFIER) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
            /* Orc */\
            /* Vampire */\
            || ((a) == &artilist[ART_TRAPPINGS_OF_THE_GRAVE] && Race_if(PM_VAMPIRE) && u.ulevel >= MASTERY_ARTIFACT_LEVEL &&\
                !has_named_mastery_artifact())\
)

#define is_monk_safe_artifact(m) (\
				(m) == ART_TIE_DYE_SHIRT_OF_SHAMBHALA ||\
				(m) == ART_CLOAK_OF_THE_UNHELD_ONE ||\
				(m) == ART_WHISPERFEET ||\
				(m) == ART_WATER_FLOWERS ||\
				(m) == ART_HAMMERFEET ||\
				(m) == ART_SHIELD_OF_THE_RESOLUTE_HEA ||\
				(m) == ART_GRAPPLER_S_GRASP ||\
				(m) == ART_GAUNTLETS_OF_SPELL_POWER ||\
				(m) == ART_PREMIUM_HEART ||\
				(m) == ART_GODHANDS ||\
				(m) == ART_FIRE_BRAND ||\
				(m) == ART_FROST_BRAND ||\
				(m) == ART_STORMHELM ||\
				(m) == ART_HELPING_HAND ||\
				(m) == ART_GARNET_ROD ||\
				(m) == ART_ARKENSTONE ||\
				(m) == ART_FLUORITE_OCTAHEDRON ||\
				(m) == ART_MARAUDER_S_MAP\
			)

#define double_bonus_damage_artifact(m) (\
	(m) == ART_LIMITED_MOON ||\
	(m) == ART_STAFF_OF_TWELVE_MIRRORS ||\
	(m) == ART_POSEIDON_S_TRIDENT\
	)

#endif /* ARTIFACT_H */
