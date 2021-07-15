/*	SCCS Id: @(#)seduce.h	3.4	1990/02/22	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef SEDUCE_H
#define SEDUCE_H


/* Bad effects are negative */

/* standard foocubi (enhanced for some uniques) */
#define SEDU_DRAINEN         -1
#define SEDU_DUMPS           -2
#define SEDU_DULLSENSES      -3
#define SEDU_DRAINLVL        -4
#define SEDU_EXHAUSTED       -5
/* clockwork-only */
#define SEDU_SUCKSOUL      -101
#define SEDU_STEALONE      -102
#define SEDU_STEALEIGHT    -103
#define SEDU_DAMAGECLK     -104
/* Lolth-specific */
#define SEDU_LOLTHATTACK	 -6
#define SEDU_VAMP            -7
#define SEDU_POISONBITE      -8
/* Malcanthet-specifc */
#define SEDU_BURDEN          -9
#define SEDU_CURSE          -10
#define SEDU_GREMLIN        -11
#define SEDU_PARALYZE       -12
/* Grazzt-specific */
#define SEDU_STEALSIX       -13
#define SEDU_BADWEAP        -14
#define SEDU_BADHAT         -15
#define SEDU_BADBOOTS       -16
#define SEDU_BADAMU         -17
#define SEDU_PUNISH         -18
/* shared between Malcanthet and Graz'zt */
#define SEDU_JEALOUS		-19

/* Good effects are positive */
/* standard foocubi (enchanced for some uniques) */
#define SEDU_GAINEN           1
#define SEDU_GOODENOUGH       2
#define SEDU_REMEMBER         3
#define SEDU_EDUCATE          4
#define SEDU_RESTOREHP        5
/* clockwork-only */
#define SEDU_WIND           101
#define SEDU_NOTHING        102
/* uniques-specific */
#define SEDU_WISH             6
#define SEDU_BLESS            7
#define SEDU_PROTECT          8
#define SEDU_RAISESTATS       9
#define SEDU_LIFEFONT        10
#define SEDU_CARRYCAP        11
#define SEDU_POISRES         12
#define SEDU_ACIDRES         13
#define SEDU_SICKRES         14
#define SEDU_GENOCIDE        15
#define SEDU_SIXMAGICS       16
#define SEDU_SIXTRUTHS       17
#define SEDU_SIXFOLLOWERS    18
#define SEDU_LIFESAVING      19

#endif /* SEDUCE_H */
