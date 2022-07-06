/*	SCCS Id: @(#)ehor.h	3.4	1997/05/01	*/
/* Copyright (c) Izchak Miller, 1989.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef EHOR_H
#define EHOR_H
/* How horrors are made */
static int randWeaponAttackTypes[] =
					{
						/* 6x */
						AT_WEAP, AT_WEAP,
						AT_WEAP, AT_WEAP,
						AT_WEAP, AT_WEAP,
						/* 2x */
						AT_SRPR, AT_SRPR,
						/* 1x */
						AT_HODS,
						/* 1x */
						AT_DEVA
					};

static int randMeleeAttackTypes[] =
					{
						/* normal melee (2x each) */
						AT_CLAW, AT_CLAW,
						AT_BITE, AT_BITE,
						AT_KICK, AT_KICK,
						AT_BUTT, AT_BUTT,
						AT_STNG, AT_STNG,
						AT_TENT, AT_TENT,
						AT_WHIP, AT_WHIP,
						/* extended range melee */
						AT_LNCK,
						/* very extended range melee */
						AT_5SBT
					};

static int randRangedAttackTypes[] =
					{
						/* 4x */
						AT_BREA, AT_BREA, AT_BREA, AT_BREA,
						AT_GAZE, AT_GAZE, AT_GAZE, AT_GAZE,
						AT_MAGC, AT_MAGC, AT_MAGC, AT_MAGC,
						/* 2x */
						AT_SPIT, AT_SPIT,
						AT_ARRW, AT_ARRW,
						AT_BEAM, AT_BEAM,
						AT_BRSH, AT_BRSH,
						/* 1x */
						AT_WDGZ
					};

static int randSpecialAttackTypes[] =
					{
						/* follow-ups */
						AT_HUGS,
						AT_REND,
						/* engulf */
						AT_ENGL
					};

static int randWeaponDamageTypes[] =
					{
						/* 3/9 physical */
						AD_PHYS, AD_PHYS, AD_PHYS,
						AD_PHYS, AD_PHYS, AD_PHYS,
						AD_PHYS, AD_PHYS, AD_PHYS,
						/* 4/9 elemental (1/3 of which pierce resistance) */
						AD_FIRE, AD_FIRE, AD_EFIR,
						AD_COLD, AD_COLD, AD_ECLD,
						AD_ELEC, AD_ELEC, AD_EELC,
						AD_ACID, AD_ACID, AD_EACD,
						/* 1/9 poisons */
						AD_DRST, AD_DRDX, AD_DRCO,
						/* 1/9 lifedrain */
						AD_DRLI, AD_DRLI, AD_DRLI,
						/* rarely vorpal (which is still subject to 1/20 odds per hit) */
						AD_VORP
					};

static int randRapierDamageTypes[] =
					{
						/* 2x */
						AD_SHDW, AD_SHDW,
						AD_STAR, AD_STAR,
						AD_BLUD, AD_BLUD,
						/* 1x */
						AD_MOON,
						AD_EFIR,
						AD_ECLD,
						AD_EELC,
						AD_EACD
					};

static int randRendDamageTypes[] =
					{
						/* 2/10 physical */
						AD_PHYS, AD_PHYS, AD_PHYS,
						AD_PHYS, AD_PHYS, AD_PHYS,
						/* 4/10 elemental (2/3 of which pierce resistance) */
						AD_FIRE, AD_EFIR, AD_EFIR,
						AD_COLD, AD_ECLD, AD_ECLD,
						AD_ELEC, AD_EELC, AD_EELC,
						AD_ACID, AD_EACD, AD_EACD,
						/* 1/10 crush and drown */
						AD_WRAP, AD_WRAP, AD_WRAP,
						/* 1/10 shred gear */
						AD_SHRD, AD_SHRD, AD_SHRD,
						/* 1/10 vorpal */
						AD_VORP, AD_VORP, AD_VORP,
						/* 1/10 physical + status effects */
						AD_STUN, AD_SLOW, AD_PLYS
					};

static int randSpecialDamageTypes[] =
					{
						/* elements */
						AD_FIRE,
						AD_COLD,
						AD_ELEC,
						AD_ACID,
						AD_DRLI,
						AD_WET, 
						AD_DESC,
						AD_PSON,
						/* poisons */
						AD_DRST,
						AD_DRDX,
						AD_DRCO,
						AD_POSN,
						AD_SVPN,
						/* physical + status effects */
						AD_SLEE,
						AD_BLND,
						AD_STUN,
						AD_SLOW,
						AD_PLYS,
						AD_CONF,
						AD_LEGS,
						/* lethal status effects */
						AD_STON,
						AD_DISE,
						/* targets your stuff */
						AD_RUST,
						AD_CORR,
						AD_DCAY,
						AD_ENCH,
						AD_SHRD,
						AD_SGLD,
						AD_SITM,
						/* misc */
						AD_DREN,
						AD_WEBS,
						/* grabs */
						AD_STCK,
						AD_MALK,
						/* statdrain */
						AD_DRIN,
						AD_WISD,
						AD_NPDC
					};

static int randTouchDamageTypes[] = 
{
						/* teleportation */
						AD_TLPT,
						AD_ABDC,
						AD_TELE,
						AD_LVLT,
						AD_WEEP,
						/* nasty status effects */
						AD_HALU,
						AD_CHRN,
						AD_SLIM,
						/* nasty not-quite-status effects*/
						AD_TCKL,
						AD_POLY,
						AD_SUCK
};

static int randBreathDamageTypes[] =
					{
						AD_RBRE,
						AD_MAGM,
						AD_COLD,
						AD_DRST,
						AD_FIRE,
						AD_SLEE,
						AD_ELEC,
						AD_ACID,
						AD_DISN
					};

static int randSplashDamageTypes[] =
					{
						AD_FIRE,
						AD_EFIR,
						AD_ACID,
						AD_EACD,
						AD_SLIM,
						// AD_BLND,
						AD_DRST,
						AD_DARK,
						AD_PHYS,
						AD_DISE,
						AD_WET
					};

static int randBeamDamageTypes[] =
					{
						AD_MAGM,
						AD_COLD,
						AD_DRST,
						AD_FIRE,
						AD_SLEE,
						AD_ELEC,
						AD_ACID,
						AD_PSON,
						AD_BLND,
						AD_STUN,
						AD_PLYS,
						AD_DRLI,
						AD_DREN,
						AD_STON,
						AD_SGLD,
						AD_SITM,
						AD_TLPT,
						AD_RUST,
						AD_CONF,
						AD_DISE,
						AD_DCAY,
						AD_HALU,
						AD_SLIM,
						AD_ENCH,
						AD_CORR,
						AD_POSN,
						AD_SHRD,
						AD_ABDC,
						AD_TELE,
						AD_POLY,
						AD_CHRN,
						AD_LVLT,
						AD_VAMP,
						AD_WEBS,
						AD_SHDW,
						AD_DESC,
						AD_STTP
					};

static int randSpitDamageTypes[] =
					{
						AD_BLND,
						AD_ACID,
						AD_DRST
					};

static int randGazeDamageTypes[] =
					{
						/* random (3x) */
						AD_RGAZ, AD_RGAZ, AD_RGAZ,
						/* direct (2x) */
						AD_FIRE, AD_FIRE,
						AD_COLD, AD_COLD,
						AD_ELEC, AD_ELEC,
						AD_DRLI, AD_DRLI,
						AD_CNCL, AD_CNCL,
						AD_ENCH, AD_ENCH,
						/* eye-contact */
						AD_DEAD,
						AD_PLYS,
						AD_STON,
						AD_LUCK,
						AD_BLND,
						AD_CONF,
						AD_SLOW,
						AD_STUN,
						AD_HALU,
						AD_SLEE,
						AD_BLNK,
						AD_VAMP,
						AD_WISD
					};
static int randEngulfDamageTypes[] =
					{
						AD_DISE,
						AD_ACID,
						AD_DGST,
						AD_PHYS,
						AD_BLND,
						AD_COLD,
						AD_ELEC,
						AD_DESC,
						AD_FIRE
					};

static int randArrowDamageTypes[] = 
					{
						AD_PHYS, /*Phys uses tracked arrows rather than generated ones*/
						AD_LOAD,
						AD_VBLD,
						AD_BALL,
						AD_BLDR,
						AD_SHDW,
						AD_BLDR
					};

static int randMagicDamageTypes[] =
					{
						/* 6/10 standard spellcasting */
						AD_SPEL, AD_SPEL, AD_SPEL,
						AD_CLRC, AD_CLRC, AD_CLRC,
						/* 4/10 elemental spellcasting */
						AD_MAGM,
						AD_FIRE,
						AD_COLD,
						AD_ELEC
					};

/* corresponds to MZ sizes */
static int randCorpseWeights[] = 
						{WT_TINY,
						 WT_SMALL,
						 WT_MEDIUM,
						 WT_LARGE,
						 WT_HUGE,
						 0,
						 0,
						 WT_GIGANTIC };
/* corresponds to MZ sizes */
static int randCorpseNut[] = 
						{CN_TINY,
						 CN_SMALL,
						 CN_MEDIUM,
						 CN_LARGE,
						 CN_HUGE,
						 0,
						 0,
						 CN_GIGANTIC };


#endif /* EHOR_H */
