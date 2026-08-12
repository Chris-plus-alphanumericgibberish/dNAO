/*	SCCS Id: @(#)skills.h	3.4	1999/10/27	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985-1999. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef SKILLS_H
#define SKILLS_H

/* Much of this code was taken from you.h.  It is now
 * in a separate file so it can be included in objects.c.
 */


/* Code to denote that no skill is applicable */
#define P_NONE				0

/* Weapon Skills -- Stephen White
 * Order matters and are used in macros.
 * Positive values denote hand-to-hand weapons or launchers.
 * Negative values denote ammunition or missiles.
 * Update weapon.c if you ammend any skills.
 * Also used for oc_subtyp.
 */
#define P_DAGGER             1
#define P_KNIFE              2
#define P_AXE                3
#define P_PICK_AXE           4
#define P_SHORT_SWORD        5
#define P_BROAD_SWORD        6
#define P_LONG_SWORD         7
#define P_TWO_HANDED_SWORD   8
#define P_SCIMITAR           9
#define P_SABER             10
#define P_CLUB              11	/* Heavy-shafted bludgeon */
#define P_MACE              12	
#define P_MORNING_STAR      13	/* Spiked bludgeon */
#define P_FLAIL             14	/* Two pieces hinged or chained together */
#define P_HAMMER            15	/* Heavy head on the end */
#define P_QUARTERSTAFF      16	/* Long-shafted bludgeon */
#define P_POLEARMS          17
#define P_SPEAR             18  /* includes javelin */
#define P_TRIDENT           19
#define P_LANCE             20
#define P_BOW               21
#define P_SLING             22
//#ifdef FIREARMS
#define P_FIREARM			23	/* KMH */
//#endif
#define P_CROSSBOW          24
#define P_DART              25
#define P_SHURIKEN          26
#define P_BOOMERANG         27
#define P_WHIP              28
#define P_HARVEST 		    29
#define P_UNICORN_HORN      30	/* last weapon */
#define P_FIRST_WEAPON      P_DAGGER
#define P_LAST_WEAPON       P_UNICORN_HORN

/* Spell Skills added by Larry Stewart-Zerba */
#define P_ATTACK_SPELL		(P_LAST_WEAPON + 1)
#define P_HEALING_SPELL		(P_LAST_WEAPON + 2)
#define P_DIVINATION_SPELL	(P_LAST_WEAPON + 3)
#define P_ENCHANTMENT_SPELL	(P_LAST_WEAPON + 4)

#define P_CLERIC_SPELL      (P_LAST_WEAPON + 5)
#define P_ESCAPE_SPELL      (P_LAST_WEAPON + 6)
#define P_MATTER_SPELL      (P_LAST_WEAPON + 7)
#define P_WAND_POWER		(P_LAST_WEAPON + 8)
#define P_MUSICALIZE		(P_LAST_WEAPON + 9)	/* 'cast' spells as songs */
#define P_SMITHING			(P_LAST_WEAPON + 10)
#define P_FIRST_SPELL		P_ATTACK_SPELL
#define P_LAST_SPELL		P_SMITHING

/* musicalize indices */
#define SNG_FEAR		1
#define SNG_SLEEP		2
#define SNG_HEAL		3
#define SNG_RLLY	    4
#define SNG_CONFUSION	5
#define SNG_HASTE		6
#define SNG_CNCL		7
#define SNG_SLOW		8
#define SNG_TAME		9
#define SNG_COURAGE	   10

/* Other types of combat */
#define P_BARE_HANDED_COMBAT	(P_LAST_SPELL + 1)
#define P_MARTIAL_ARTS			P_BARE_HANDED_COMBAT	/* Role distinguishes */
#define P_TWO_WEAPON_COMBAT		(P_LAST_SPELL + 2)
#define P_SHIELD				(P_LAST_SPELL + 3)
#define P_BEAST_MASTERY			(P_LAST_SPELL + 4)

#define P_SHII_CHO		(P_LAST_SPELL + 5)
#define P_MAKASHI		(P_SHII_CHO + 1)
#define P_SORESU		(P_SHII_CHO + 2)
#define P_ATARU			(P_SHII_CHO + 3)
#define P_DJEM_SO		(P_SHII_CHO + 4)
#define P_SHIEN			(P_SHII_CHO + 5)
#define P_NIMAN			(P_SHII_CHO + 6)
#define P_JUYO			(P_SHII_CHO + 7)
#define is_lightsaber_skill(type) \
	((type) >= P_SHII_CHO && (type) <= P_JUYO)

#define P_SHIELD_BASH	(P_SHII_CHO + 8)
#define P_GREAT_WEP 	(P_SHII_CHO + 9)
#define P_GENERIC_KNIGHT_FORM	(P_SHII_CHO + 10)
#define P_KNI_SACRED	(P_SHII_CHO + 11)
#define P_KNI_ELDRITCH	(P_SHII_CHO + 12)
#define P_KNI_RUNIC		(P_SHII_CHO + 13)

#define P_RIDING			(P_KNI_RUNIC + 1)	/* How well you control your steed */

#define P_LAST_H_TO_H		P_RIDING
#define P_FIRST_H_TO_H		P_BARE_HANDED_COMBAT

#define P_FIRST_FORM	P_SHII_CHO
#define P_LAST_FORM		P_KNI_RUNIC

#define P_NUM_SKILLS		(P_LAST_H_TO_H+1)

/* These roles qualify for a martial arts bonus */
#define martial_bonus()	(u.umartial || Earth_crystal)

/* Fighting form IDs */
/* one bit per form in u.fightingForm[], FFORM_BATCH_SIZE to a word */
/* each batch is mutually exclusive, but not with other batches */
#define FFORM_BATCH_SIZE	16
#define LIGHTSABER_BATCH_NUMBER		0
#define BASIC_KNI_BATCH_NUMBER		1
#define ADV_KNI_BATCH_NUMBER		2
#define MNK_BATCH_NUMBER			3

/* The word a form lives in, and its bit within that word. */
#define FFORM_WORD(fform)	(((fform)-1)/FFORM_BATCH_SIZE)
#define FFORM_BIT(fform)	(0x1UL << (((fform)-1)%FFORM_BATCH_SIZE))

#define NO_FFORM		0

#define FFORM_SHII_CHO	(1 + LIGHTSABER_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_MAKASHI	(2 + LIGHTSABER_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_SORESU	(3 + LIGHTSABER_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_ATARU		(4 + LIGHTSABER_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_DJEM_SO	(5 + LIGHTSABER_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_SHIEN		(6 + LIGHTSABER_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_NIMAN		(7 + LIGHTSABER_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_JUYO		(8 + LIGHTSABER_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FIRST_LS_FFORM	FFORM_SHII_CHO
#define LAST_LS_FFORM	FFORM_JUYO

#define FFORM_SHIELD_BASH 	(1 + BASIC_KNI_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_GREAT_WEP		(2 + BASIC_KNI_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_HALF_SWORD 	(3 + BASIC_KNI_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_POMMEL 		(4 + BASIC_KNI_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_KNI_SACRED	(1 + ADV_KNI_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_KNI_ELDRITCH	(2 + ADV_KNI_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_KNI_RUNIC		(3 + ADV_KNI_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FIRST_BASIC_KNI_FFORM		FFORM_SHIELD_BASH
#define LAST_BASIC_KNI_FFORM		FFORM_POMMEL
#define FIRST_ADV_KNI_FFORM			FFORM_KNI_SACRED
#define LAST_ADV_KNI_FFORM			FFORM_KNI_RUNIC

/* the monk's default style is NO_FFORM, presented in the menu as the tiger style */
#define FFORM_MNK_KIRIN		(1 + MNK_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_MNK_CRANE		(2 + MNK_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_MNK_SNAKE		(3 + MNK_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FFORM_MNK_MANTIS	(4 + MNK_BATCH_NUMBER*FFORM_BATCH_SIZE)
#define FIRST_MNK_FFORM		FFORM_MNK_KIRIN
#define LAST_MNK_FFORM		FFORM_MNK_MANTIS
#define MNK_FFORM_COUNT		(LAST_MNK_FFORM - FIRST_MNK_FFORM + 1)

#define LAST_FFORM		FFORM_MNK_MANTIS

#define FightingFormSkillLevel(i)	P_SKILL(getFightingFormSkill(i))
/*
 * These are the standard weapon skill levels.  It is important that
 * the lowest "valid" skill be be 1.  The code calculates the
 * previous amount to practice by calling  practice_needed_to_advance()
 * with the current skill-1.  To work out for the UNSKILLED case,
 * a value of 0 needed.
 */
#define P_ISRESTRICTED	0
#define P_UNSKILLED		1
#define P_BASIC			2
#define P_SKILLED		3
#define P_EXPERT		4
#define P_MASTER		5	/* Unarmed combat/martial arts only */
#define P_GRAND_MASTER	6	/* Unarmed combat/martial arts only */

#define practice_needed_to_advance(level) ((level)*(level)*20)

/* The hero's skill in various weapons. */
struct skills {
	xchar skill;
	xchar max_skill;
	unsigned short advance;
};

#define OLD_P_SKILL(type)		(u.weapon_skills[type].skill)
#define OLD_P_MAX_SKILL(type)	(u.weapon_skills[type].max_skill)
#define P_ADVANCE(type)		(u.weapon_skills[type].advance)
#define OLD_P_RESTRICTED(type)	(u.weapon_skills[type].skill == P_ISRESTRICTED)

#define P_SKILL_LIMIT 90	/* Max number of skill advancements (added +30 for human bonus, even though shouldnt need that many) */

/* Initial skill matrix structure; used in u_init.c and weapon.c */
struct def_skill {
	xchar skill;
	xchar skmax;
};

#endif  /* SKILLS_H */
