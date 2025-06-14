/*	SCCS Id: @(#)timeout.h	3.4	1999/02/13	*/
/* Copyright 1994, Dean Luick					  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef TIMEOUT_H
#define TIMEOUT_H

/* generic timeout function */
typedef void FDECL((*timeout_proc), (genericptr_t, long));

/* kind of timer */
#define TIMER_OBJECT	1	/* event follows a object */
#define TIMER_MONSTER	2	/* event follows a monster */

/* save/restore timer ranges */
#define RANGE_LEVEL  0		/* save/restore timers staying on level */
#define RANGE_GLOBAL 1		/* save/restore timers following global play */

/* flags for revive type after timeout */
#define	REVIVE_MONSTER	0
#define	GROW_MOLD		1
#define	REVIVE_MOLD		2
#define	GROW_SLIME		3
#define	REVIVE_ZOMBIE	4
#define	REVIVE_SHADE	5
#define	REVIVE_YELLOW	6
#define	GROW_BBLOOM		7
/*
 * Timeout functions.  Add a define here, then put it in the table
 * in timeout.c.  "One more level of indirection will fix everything."
 */
#define ROT_ORGANIC		0	/* for buried organics */
#define ROT_CORPSE		1
#define MOLDY_CORPSE	2
#define REVIVE_MON		3
#define BURN_OBJECT		4
#define HATCH_EGG		5
#define FIG_TRANSFORM	6
#define LIGHT_DAMAGE	7
#define SLIMY_CORPSE	8
#define ZOMBIE_CORPSE	9
#define SHADY_CORPSE	10
#define YELLOW_CORPSE	11
#define BOMB_BLOW		12
#define RETURN_AMMO		13
#define DESUMMON_MON	14
#define DESUMMON_OBJ	15
#define LARVAE_DIE		16
#define REVIVE_PICKUP	17
#define REVERT_OBJECT	18
#define REVERT_MERC		19
#define NUM_TIME_FUNCS	20

/* used in timeout.c */
typedef struct timer {
	struct timer *next;	/* next item in PROCESSING chain */
	struct timer *tnxt;	/* next item in LOCAL chain */
	long timeout;		/* when we time out */
	unsigned long tid;	/* timer ID */
	short kind;			/* kind of use */
	short func_index;	/* what to call when we time out */
	genericptr_t arg;	/* pointer to timeout argument */
	int timerflags;		/* flags for the timer */ 
} timer_element;
#define TIMERFLAG_PAUSED	0x01	/* pause progression of timer -- 'timeout' is how many turns were remaining when paused, instead of end-turn*/
#define TIMERFLAG_MIGRATING	0x10	/* progress but do not execute timer, as 'arg' is migrating */

#endif /* TIMEOUT_H */
