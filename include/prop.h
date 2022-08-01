/*	SCCS Id: @(#)prop.h	3.4	1999/07/07	*/
/* Copyright (c) 1989 Mike Threepoint				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef PROP_H
#define PROP_H

/*** What the properties are ***/
#define NO_PROP						0
#define FIRE_RES					1 + NO_PROP
#define COLD_RES					1 + FIRE_RES
#define SLEEP_RES					1 + COLD_RES
#define DISINT_RES					1 + SLEEP_RES
#define SHOCK_RES					1 + DISINT_RES
#define POISON_RES					1 + SHOCK_RES
#define ACID_RES					1 + POISON_RES
#define STONE_RES					1 + ACID_RES
#define DRAIN_RES					1 + STONE_RES
#define SICK_RES					1 + DRAIN_RES
#define ANTIMAGIC					1 + SICK_RES
#define REFLECTING					1 + ANTIMAGIC
/* note: for the first 12 properties, MR_xxx == (1 << (xxx_RES - 1)) */
#define DISPLACED					1 + REFLECTING
#define SEARCHING					1 + DISPLACED
#define SEE_INVIS					1 + SEARCHING
#define INVIS						1 + SEE_INVIS
#define TELEPORT_CONTROL			1 + INVIS
#define TELEPORT					1 + TELEPORT_CONTROL
#define POLYMORPH					1 + TELEPORT
#define POLYMORPH_CONTROL			1 + POLYMORPH
#define LEVITATION					1 + POLYMORPH_CONTROL
#define STEALTH						1 + LEVITATION
#define AGGRAVATE_MONSTER			1 + STEALTH
#define CONFLICT					1 + AGGRAVATE_MONSTER
#define PROTECTION					1 + CONFLICT
#define PROT_FROM_SHAPE_CHANGERS	1 + PROTECTION
#define WARNING						1 + PROT_FROM_SHAPE_CHANGERS
#define TELEPAT						1 + WARNING
#define FAST						1 + TELEPAT
#define STUNNED						1 + FAST
#define CONFUSION					1 + STUNNED
#define SICK						1 + CONFUSION
#define BLINDED						1 + SICK
#define BLIND_RES					1 + BLINDED
#define SLEEPING					1 + BLIND_RES
#define WOUNDED_LEGS				1 + SLEEPING
#define STONED						1 + WOUNDED_LEGS
#define GOLDED						1 + STONED
#define STRANGLED					1 + GOLDED
#define FROZEN_AIR					1 + STRANGLED
#define HALLUC						1 + FROZEN_AIR
#define HALLUC_RES					1 + HALLUC
#define FUMBLING					1 + HALLUC_RES
#define JUMPING						1 + FUMBLING
#define WWALKING					1 + JUMPING
#define HUNGER						1 + WWALKING
/* Insanity-related */
#define PANIC						1 + HUNGER
#define STUMBLE_BLIND				1 + PANIC
#define STAGGER_SHOCK				1 + STUMBLE_BLIND
#define BABBLING					1 + STAGGER_SHOCK
#define SCREAMING					1 + BABBLING
#define FAINTING_FIT				1 + SCREAMING
/* More Misc. */
#define GLIB						1 + FAINTING_FIT
#define LIFESAVED					1 + GLIB
#define CLAIRVOYANT					1 + LIFESAVED
#define VOMITING					1 + CLAIRVOYANT
#define ENERGY_REGENERATION			1 + VOMITING
#define MAGICAL_BREATHING			1 + ENERGY_REGENERATION
#define WATERPROOF					1 + MAGICAL_BREATHING
#define HALF_SPDAM					1 + WATERPROOF
#define HALF_PHDAM					1 + HALF_SPDAM
#define ADORNED						1 + HALF_PHDAM
#define REGENERATION				1 + ADORNED
#define SANCTUARY					1 + REGENERATION
#define FREE_ACTION					1 + SANCTUARY
#define SWIMMING					1 + FREE_ACTION
#define SLIMED						1 + SWIMMING
#define FIXED_ABIL					1 + SLIMED
#define FLYING						1 + FIXED_ABIL
#define UNCHANGING					1 + FLYING
#define PASSES_WALLS				1 + UNCHANGING
#define SLOW_DIGESTION				1 + PASSES_WALLS
/* vision */
#define NORMALVISION				1 + SLOW_DIGESTION	/* see in lit LoS; x1 night vision */
#define LOWLIGHTSIGHT				1 + NORMALVISION	/* see in lit LoS; x2 night vision */
#define ELFSIGHT					1 + LOWLIGHTSIGHT	/* see in lit LoS; x3 night vision */
#define DARKSIGHT					1 + ELFSIGHT		/* see in dark LoS; blinded in light; */
#define CATSIGHT					1 + DARKSIGHT		/* see in lit LoS OR dark LoS depending on what current square is */
#define EXTRAMISSION				1 + CATSIGHT		/* see in LoS */
#define XRAY_VISION					1 + EXTRAMISSION	/* see in radius, ignorining LoS */
#define ECHOLOCATION				1 + XRAY_VISION		/* supplementary vision that maps terrain, finds items, `I`s creatures */
/* senses */
#define INFRAVISION					1 + ECHOLOCATION	/* see creatures in LoS that are infravisible */
#define BLOODSENSE					1 + INFRAVISION		/* sense creatures in LoS that have blood */
#define LIFESENSE					1 + BLOODSENSE		/* sense creatures in LoS that are not non-living */
#define SENSEALL					1 + LIFESENSE		/* sense all creatures */
#define OMNISENSE					1 + SENSEALL		/* ignore LoS requirement on above senses (but not sight) */
#define EARTHSENSE					1 + OMNISENSE		/* sense creatures that are touching the ground */
/* warning */
#define WARN_OF_MON					1 + EARTHSENSE		/* sense particular monsters on level */
#define DETECT_MONSTERS				1 + WARN_OF_MON		/* sense creatures */
/* more misc */
#define SPELLBOOST					1 + DETECT_MONSTERS
#define NECROSPELLS					1 + SPELLBOOST
#define CARCAP						1 + NECROSPELLS
#define WELDPROOF					1 + CARCAP
#define NULLMAGIC					1 + WELDPROOF
#define STERILE						1 + NULLMAGIC
#define CHASTITY					1 + STERILE
#define CLEAVING					1 + CHASTITY
#define GOOD_HEALTH					1 + CLEAVING
#define RAPID_HEALING				1 + GOOD_HEALTH
#define DESTRUCTION					1 + RAPID_HEALING
#define MIND_BLASTS					1 + DESTRUCTION
#define PRESERVATION				1 + MIND_BLASTS
#define QUICK_DRAW					1 + PRESERVATION
#define CLEAR_THOUGHTS				1 + QUICK_DRAW
#define DOUBT						1 + CLEAR_THOUGHTS
#define BLOCK_CONFUSION				1 + DOUBT
#define BLOWING_WINDS				1 + BLOCK_CONFUSION
#define TIME_STOP					1 + BLOWING_WINDS
#define BLOOD_DROWN					1 + TIME_STOP
#define DEADMAGIC					1 + BLOOD_DROWN
#define CATAPSI						1 + DEADMAGIC
#define MISOTHEISM					1 + CATAPSI
#define DARKVISION_ONLY				1 + MISOTHEISM
#define SHATTERING					1 + DARKVISION_ONLY
#define DIMENSION_LOCK				1 + SHATTERING
#define WALKING_NIGHTMARE			1 + DIMENSION_LOCK
#define FIRE_CRYSTAL				1 + WALKING_NIGHTMARE
#define WATER_CRYSTAL				1 + FIRE_CRYSTAL
#define EARTH_CRYSTAL				1 + WATER_CRYSTAL
#define AIR_CRYSTAL					1 + EARTH_CRYSTAL
#define BLACK_CRYSTAL				1 + AIR_CRYSTAL
#define LAST_PROP					(BLACK_CRYSTAL)

#define MPROP_SIZE					(LAST_PROP/32 + 1)

/*** Where the properties come from ***/
/* Definitions were moved here from obj.h and you.h */
struct prop {
	/*** Properties conveyed by objects ***/
	long extrinsic;
	/* Armor */
#	define W_ARM	    0x00000001L /* Body armor */
#	define W_ARMC	    0x00000002L /* Cloak */
#	define W_ARMH	    0x00000004L /* Helmet/hat */
#	define W_ARMS	    0x00000008L /* Shield */
#	define W_ARMG	    0x00000010L /* Gloves/gauntlets */
#	define W_ARMF	    0x00000020L /* Footwear */
#	define W_ARMU	    0x00000040L /* Undershirt */
#	define W_ARMOR	     (W_ARM | W_ARMC | W_ARMH | W_ARMS | W_ARMG | W_ARMF | W_ARMU)
	/* Weapons and artifacts */
#	define W_WEP	    0x00000100L /* Wielded weapon */
#	define W_QUIVER     0x00000200L /* Quiver for (f)iring ammo */
#	define W_SWAPWEP    0x00000400L /* Secondary weapon */
#	define W_ART	    0x00001000L /* Carrying artifact (not really worn) */
#	define W_ARTI	    0x00002000L /* Invoked artifact  (not really worn) */
	/* Amulets, rings, tools, and other items */
#	define W_AMUL	    0x00010000L /* Amulet */
#	define W_RINGL	    0x00020000L /* Left ring */
#	define W_RINGR	    0x00040000L /* Right ring */
#	define W_RING	    (W_RINGL | W_RINGR)
#	define W_TOOL	    0x00080000L /* Eyewear */
#ifdef STEED
#	define W_SADDLE     0x00100000L	/* KMH -- For riding monsters */
#endif
#	define W_BALL	    0x00200000L /* Punishment ball */
#	define W_CHAIN	    0x00400000L /* Punishment chain */
#	define W_SPIRIT		0x00800000L	/* Bound spirit */
#	define W_GLYPH		0x01000000L	/* Active thought-glyph */
#	define W_SKIN		I_SPECIAL	/* merged into skin */
#	define W_WORN	     (W_ARMOR | W_AMUL | W_RING | W_TOOL)

	/*** Property is blocked by an object ***/
	long blocked;					/* Same assignments as extrinsic */

	/*** Timeouts, permanent properties, and other flags ***/
	long intrinsic;
	/* Timed properties */
#	define TIMEOUT	    0x00ffffffL /* Up to 16 million turns */
#	define TIMEOUT_INF	0x00800000L	/* If you get this much, it won't decrement. should be a subset of TIMEOUT */
	/* Permanent properties */
#	define FROMEXPER    0x01000000L /* Gain/lose with experience, for role */
#	define FROMRACE     0x02000000L /* Gain/lose with experience, for race */
#	define FROMPOLY		0x04000000L	/* Gain/lose with polyform */
#	define FROMOUTSIDE  0x08000000L /* Should generally not be lost. */
#	define INTRINSIC    (FROMOUTSIDE|FROMRACE|FROMPOLY|FROMEXPER|TIMEOUT_INF)
	/* Control flags */
#	define I_SPECIAL    0x10000000L /* Property is controllable */
};

/*** Definitions for backwards compatibility ***/
#define LEFT_RING	W_RINGL
#define RIGHT_RING	W_RINGR
#define LEFT_SIDE	LEFT_RING
#define RIGHT_SIDE	RIGHT_RING
#define BOTH_SIDES	(LEFT_SIDE | RIGHT_SIDE)
#define WORN_ARMOR	W_ARM
#define WORN_CLOAK	W_ARMC
#define WORN_HELMET	W_ARMH
#define WORN_SHIELD	W_ARMS
#define WORN_GLOVES	W_ARMG
#define WORN_BOOTS	W_ARMF
#define WORN_AMUL	W_AMUL
#define WORN_BLINDF	W_TOOL
#ifdef TOURIST
#define WORN_SHIRT	W_ARMU
#endif

#endif /* PROP_H */
