/*	SCCS Id: @(#)mthrowu.c	3.4	2003/05/09	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "mfndpos.h" /* ALLOW_M */

//STATIC_DCL int FDECL(drop_throw,(struct monst *, struct obj *,BOOLEAN_P,int,int));

#define URETREATING(x,y) (distmin(u.ux,u.uy,x,y) > distmin(u.ux0,u.uy0,x,y))

#define POLE_LIM 5	/* How far monsters can use pole-weapons */

static const int dirx[8] = {0, 1, 1,  1,  0, -1, -1, -1},
				 diry[8] = {1, 1, 0, -1, -1, -1,  0,  1};


#ifdef OVLB

/* Remove an item from the monster's inventory and destroy it. */
void
m_useup(mon, obj)
struct monst *mon;
struct obj *obj;
{
	if (obj->quan > 1L) {
		obj->quan--;
		obj->owt = weight(obj);
	} else {
		boolean gloves = !!(obj->owornmask&W_ARMG);
		obj_extract_self(obj);
		possibly_unwield(mon, FALSE);
		if (obj->timed) stop_all_timers(obj->timed);
		if (obj->owornmask) {
		    mon->misc_worn_check &= ~obj->owornmask;
		    update_mon_intrinsics(mon, obj, FALSE, FALSE);
		}
		obfree(obj, (struct obj*) 0);
		if(gloves)
			mselftouch(mon, "No longer wearing hand-protection, ", !flags.mon_moving);
	}
}

#endif /* OVLB */
#ifdef OVL1

boolean
mtarget_adjacent(magr)
struct monst *magr;
{
	int i,j, ix, jy;
	int x = magr->mx, y = magr->my;
	struct monst *mtmp;
	
	for(i = -1; i < 2; i++){
		for(j = -1; j < 2; j++){
			if(!i && !j)
				continue;
			ix = x + i;
			jy = y + j;
			if(!isok(ix,jy))
				continue;
			if(!(mtmp = m_u_at(ix, jy)))
				continue;
			if(mtmp == &youmonst){
				if(mtmp->mpeaceful)
					continue;
				else return TRUE;
			}
			if(!mm_aggression(magr, mtmp))
				continue;
			if(magr->mtame && !acceptable_pet_target(magr, mtmp, FALSE))
				continue;
			return TRUE;
		}
	}
	return FALSE;
}

extern int monstr[];

/* Find a target for a ranged attack. */
/* needs to set tbx, tby */
struct monst *
mfind_target(magr, force_linedup, use_find_offensive)
struct monst *magr;
boolean force_linedup;	/* if TRUE, we have some offensive item ready that will work if we are lined up */
boolean use_find_offensive;	/* if TRUE, we have some offensive item ready that will work if we are lined up */
{
	struct monst * mdef = (struct monst *)0;
	struct monst * best_target = (struct monst *)0;
	struct attack attkbuff = {0};
	int target_val = 0;
	int best_val = 0;
	int tarx;
	int tary;
	boolean coveted = FALSE;	/* mdef was found via covetousness (used to prioritize this mdef in targeting loop) */
	boolean tried_you = FALSE;	/* you have been attempted as mdef (used to prioritize you in targeting loop)*/

	boolean conflicted = (Conflict && couldsee(magr->mx, magr->my) &&
			(distu(magr->mx, magr->my) <= BOLT_LIM*BOLT_LIM) &&
			!resist(magr, RING_CLASS, 0, 0)) 
		|| (magr->mberserk);

	boolean dogbesafe = ((magr->mtame || magr->mpeaceful) && !(magr->mconf && !rn2(8)) && !conflicted);
	
	struct obj *mrwep = (struct obj *)0;
	if (!(mindless(magr->data) && is_undead(magr->data) && !(
			magr->mtyp == PM_SKELETAL_PIRATE ||
			magr->mtyp == PM_PHALANX
		))){
		mrwep = select_rwep(magr);
	}

	/* Check that magr can make any ranged attacks at all */
	if (!force_linedup &&
		!(
		(mon_attacktype(magr, AT_WEAP) && mrwep) ||
		(mon_attacktype(magr, AT_DEVA) && mrwep) ||
		(mon_attacktype(magr, AT_BREA) && !magr->mcan) ||
		(mon_attacktype(magr, AT_BRSH) && !magr->mcan) ||
		(mon_attacktype(magr, AT_MAGC) && !magr->mcan) ||
		(mon_attacktype(magr, AT_MMGC) && !magr->mcan) ||
		(mon_attacktype(magr, AT_GAZE) && !magr->mcan) ||
		(mon_turn_undead(magr) && !magr->mspec_used && !magr->mcan && (!Inhell || mon_healing_turn(magr))) ||
		(mon_attacktype(magr, AT_SPIT)) ||
		(mon_attacktype(magr, AT_ARRW)) ||
		(mon_attacktype(magr, AT_TNKR)) ||
		(mon_attacktype(magr, AT_BEAM)) ||
		(mon_attacktype(magr, AT_LRCH)) ||
		(mon_attacktype(magr, AT_LNCK)) ||
		(mon_attacktype(magr, AT_5SQR)) ||
		(mon_attacktype(magr, AT_5SBT)) ||
		(is_commander(magr->data)) ||
		(use_find_offensive && find_offensive(magr))
		))
		return (struct monst *)0;

	/* priority of targets:
	 * covetous item holder
	 * player
	 * others
	 */
	/* maybe have a prefered mdef */
	if (is_covetous(magr->data) && !magr->mtame) {
		tarx = STRAT_GOALX(magr->mstrategy);
		tary = STRAT_GOALY(magr->mstrategy);
		if ((mdef = m_at(tarx, tary)))
			coveted = TRUE;
	}
	
	/* target-finding loop */
	do {
		/* get next target */
		if (coveted)
			coveted = FALSE;
		else if (!tried_you) {
			mdef = &youmonst;
			tried_you = TRUE;
		}
		else if (mdef == &youmonst)
			mdef = fmon;
		else
			mdef = mdef->nmon;

		/* validate */
		if (!mdef || (mdef != &youmonst && DEADMONSTER(mdef)))
			continue;

		/* get location of next target */
		if (mdef == &youmonst) {
			if(no_upos(magr)) //They don't know where you are.
				continue;
			tarx = magr->mux;
			tary = magr->muy;

			/* don't attempt to attack self if displacement made magr think player is on top of magr */
			if (tarx == magr->mx && tary == magr->my)
				continue;
		}
		else {
			tarx = mdef->mx;
			tary = mdef->my;
		}

		/* is mdef an acceptable target? */
		if (mdef != &youmonst && !(mm_aggression(magr, mdef) & ALLOW_M) && !conflicted)
			continue;
		if (dogbesafe && (mdef == &youmonst))
			continue;
		/* Note: clear_path is somewhat costly */
		if (!clear_path(magr->mx, magr->my, tarx, tary))
			continue;

		/* don't make ranged attacks at melee distance */
		if (distmin(magr->mx, magr->my, tarx, tary) < 2)
			continue;

		/* don't make ranged attacks beyond max-range */
		if (distmin(magr->mx, magr->my, tarx, tary) > BOLT_LIM)
			continue;

		/* horrible kludge: Oona doesn't target those resistant to her at range */
		if (magr->mtyp == PM_OONA && (mdef == &youmonst ? Oona_resistance : resists_oona(mdef)))
			continue;

		/* are any of our attacks good? */
		if (/* force_linedup means we only check m_online -- we have a specific attack in mind to use that needs to be on a line */
			(force_linedup) ? m_online(magr, mdef, tarx, tary, dogbesafe, TRUE) : (

			/* OTHERWISE... (!force_linedup) */

			/* attacks that are on a line that do NOT stop on hit */
			(m_online(magr, mdef, tarx, tary, dogbesafe, FALSE) && (
				(mon_attacktype(magr, AT_BREA) && !magr->mcan) ||
				(mon_get_attacktype(magr, AT_MAGC, &attkbuff) && !magr->mcan && !real_spell_adtyp(attkbuff.adtyp)) ||
				(mon_get_attacktype(magr, AT_MMGC, &attkbuff) && !magr->mcan && !real_spell_adtyp(attkbuff.adtyp))
			))
			||
			/* attacks that splash */
			(m_insplash(magr, mdef, tarx, tary, dogbesafe) && (
				(mon_attacktype(magr, AT_BRSH) && !magr->mcan)
			))
			||
			/* attacks that are on a line that DO stop on hit */
			(m_online(magr, mdef, tarx, tary, dogbesafe, TRUE) && (
				(mon_attacktype(magr, AT_SPIT)) ||
				(mon_attacktype(magr, AT_ARRW)) ||
				(mon_attacktype(magr, AT_WEAP) && mrwep && !is_pole(mrwep)) ||
				(mon_attacktype(magr, AT_DEVA) && mrwep && !is_pole(mrwep)) ||
				(use_find_offensive && find_offensive(magr))
			))
			||
			/* attacks that are on a line that are ALWAYS SAFE */
			(m_online(magr, mdef, tarx, tary, FALSE, FALSE) && (
				(mon_attacktype(magr, AT_TNKR)) ||
				(mon_attacktype(magr, AT_BEAM))
			))
			||
			/* attacks in polearm range */
			(dist2(magr->mx, magr->my, tarx, tary) <= m_pole_range(magr) && (
				(mon_attacktype(magr, AT_WEAP) && mrwep && is_pole(mrwep)) ||
				(mon_attacktype(magr, AT_DEVA) && mrwep && is_pole(mrwep))
			))
			||
			/* attacks in a square range of 2 */
			(distmin(magr->mx, magr->my, tarx, tary) <= 2 && (
				(mon_attacktype(magr, AT_LRCH)) ||
				(mon_attacktype(magr, AT_LNCK))
			))
			||
			/* attacks in a square range of 5 */
			(distmin(magr->mx, magr->my, tarx, tary) <= 5 && (
				(mon_attacktype(magr, AT_5SQR)) ||
				(mon_attacktype(magr, AT_5SBT))
			))
			||
			/* attacks in a square range of 8 */
			(distmin(magr->mx, magr->my, tarx, tary) <= 8 && (
				(is_commander(magr->data) && !rn2(4)) ||	/* !rn2(4) -> reduce command frequency */
				(mon_attacktype(magr, AT_GAZE) && !magr->mcan) ||
				(mon_turn_undead(magr) && !magr->mspec_used && !magr->mcan) ||
				(mon_get_attacktype(magr, AT_MAGC, &attkbuff) && !magr->mcan && real_spell_adtyp(attkbuff.adtyp)) ||
				(mon_get_attacktype(magr, AT_MMGC, &attkbuff) && !magr->mcan && real_spell_adtyp(attkbuff.adtyp)) ||
				(mon_turn_undead(magr) && !magr->mspec_used && !magr->mcan && (!Inhell || mon_healing_turn(magr)))
			))
		)){
			/* mdef can be targeted by one of our attacks */
			/* calculate value of target */
			if (is_covetous(magr->data) && !magr->mtame &&
				mdef == m_at(STRAT_GOALX(magr->mstrategy), STRAT_GOALY(magr->mstrategy))) {
				/* prefer item-holder */
				target_val = 999;
			}
			else if (mdef == &youmonst) {
				/* prefer player */
				target_val = 999;
			}
			else {
				/* value is mdef's difficulty, but adjusted to prefer closer targets */
				target_val = monstr[mdef->mtyp] + BOLT_LIM - distmin(magr->mx, magr->my, tarx, tary);
			}
			/* maybe we have a new target */
			if (target_val > best_val) {
				best_target = mdef;
				best_val = target_val;
			}
			/* if we found a prefered target (best_val == 999), end target-finding loop early */
		}
	} while (mdef && best_val < 999);

	/* if we have a target, */
	if (best_target) {
		/* recalc tarx/tary */
		if (best_target == &youmonst) {
			tarx = magr->mux;
			tary = magr->muy;
		}
		else {
			tarx = best_target->mx;
			tary = best_target->my;
		}
		/* set tbx, tby */
		/* caller has to know to reject tbx==0, tby==0 if called with force_linedup */
		if (!linedup(tarx, tary, magr->mx, magr->my)) {
			tbx = tby = 0;
		}
	}

	/* return the best target we found */
	return best_target;
}

boolean
linedup(ax, ay, bx, by)
register xchar ax, ay, bx, by;
{
	tbx = ax - bx;	/* These two values are set for use */
	tby = ay - by;	/* after successful return.	    */

	/* sometimes displacement makes a monster think that you're at its
	   own location; prevent it from throwing and zapping in that case */
	if (!tbx && !tby) return FALSE;

	if((!tbx || !tby || abs(tbx) == abs(tby)) /* straight line or diagonal */
	   && distmin(tbx, tby, 0, 0) < BOLT_LIM) {
	    if(ax == u.ux && ay == u.uy) return((boolean)(couldsee(bx,by)));
	    else if(clear_path(ax,ay,bx,by)) return TRUE;
	}
	return FALSE;
}

boolean
lined_up(mtmp)		/* is mtmp in position to use ranged attack? */
	register struct monst *mtmp;
{
	return(linedup(mtmp->mux,mtmp->muy,mtmp->mx,mtmp->my));
}


/* returns the max euclidean distance magr can use a polearm at */
/* Ranges:
 * 
 * ESBSE
 * S...S
 * B.@.B
 * S...S
 * ESBSE
 *  
 */
int
m_pole_range(magr)
struct monst * magr;
{
	if (magr == &youmonst) {
		impossible("calculating player's polearm range with m_pole_range?");
		return 8;
	}
	
	if(magr->mformication || magr->mscorpions)
		return 4;

	switch (m_martial_skill(magr->data)) {
		case P_EXPERT:	return 8;
		case P_SKILLED:	return 5;
		case P_BASIC:	return 4;		
		default: return 4;
	}
}


#endif /* OVL1 */
#ifdef OVL0

/* Check if a monster is carrying a particular item.
 */
struct obj *
m_carrying(mtmp, type)
struct monst *mtmp;
int type;
{
	register struct obj *otmp;

	for(otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == type)
			return(otmp);
	return((struct obj *) 0);
}

/* Check if a monster is carrying a particular charged (ovar1_charges>0) item.
 */
struct obj *
m_carrying_charged(mtmp, type)
struct monst *mtmp;
int type;
{
	register struct obj *otmp;

	for(otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == type && otmp->ovar1_charges > 0)
			return(otmp);
	return((struct obj *) 0);
}

/* TRUE iff thrown/kicked/rolled object doesn't pass through iron bars */
boolean
hits_bars(obj_p, x, y, always_hit, whodidit)
struct obj **obj_p;	/* *obj_p will be set to NULL if object breaks */
int x, y;
int always_hit;	/* caller can force a hit for items which would fit through */
int whodidit;	/* 1==hero, 0=other, -1==just check whether it'll pass thru */
{
    struct obj *otmp = *obj_p;
    int obj_type = otmp->otyp;
    boolean hits = always_hit;

    if (!hits)
	switch (otmp->oclass) {
	case WEAPON_CLASS:
	    {
		int oskill = objects[obj_type].oc_skill;

		hits = (oskill != -P_BOW  && oskill != -P_CROSSBOW &&
			oskill != -P_DART && oskill != -P_SHURIKEN &&
//#ifdef FIREARMS
			(oskill != -P_FIREARM || obj_type == ROCKET) &&
//#endif
			oskill != P_SPEAR && oskill != P_KNIFE);	/* but not dagger */
		break;
	    }
	case ARMOR_CLASS:
		hits = (objects[obj_type].oc_armcat != ARM_GLOVES);
		break;
	case TOOL_CLASS:
		hits = (obj_type != SKELETON_KEY &&
			obj_type != UNIVERSAL_KEY &&
			obj_type != LOCK_PICK &&
#ifdef TOURIST
			obj_type != CREDIT_CARD &&
#endif
			obj_type != TALLOW_CANDLE &&
			obj_type != WAX_CANDLE &&
			obj_type != LENSES &&
			obj_type != SUNGLASSES &&
			obj_type != SOUL_LENS &&
			obj_type != LIVING_MASK &&
			obj_type != R_LYEHIAN_FACEPLATE &&
			obj_type != WHISTLE &&
			obj_type != MAGIC_WHISTLE);
		break;
	case ROCK_CLASS:	/* includes boulder */
		if (obj_type != STATUE ||
			mons[otmp->corpsenm].msize > MZ_TINY) hits = TRUE;
		break;
	case FOOD_CLASS:
		if (obj_type == CORPSE &&
			mons[otmp->corpsenm].msize > MZ_TINY) hits = TRUE;
		else
		    hits = (obj_type == MEAT_STICK ||
			    obj_type == MASSIVE_CHUNK_OF_MEAT);
		break;
	case SPBOOK_CLASS:
	case WAND_CLASS:
	case BALL_CLASS:
	case CHAIN_CLASS:
	case BED_CLASS:
		hits = TRUE;
		break;
	default:
		break;
	}

    if (hits && whodidit != -1) {
	if (whodidit ? hero_breaks(otmp, x, y, FALSE) : breaks(otmp, x, y))
	    *obj_p = otmp = 0;		/* object is now gone */
	    /* breakage makes its own noises */
	else if (obj_type == BOULDER || obj_type == STATUE || obj_type == HEAVY_IRON_BALL)
	    pline("Whang!");
	else if (otmp->oclass == COIN_CLASS ||
		otmp->obj_material == GOLD ||
		otmp->obj_material == SILVER)
	    pline("Clink!");
	else
	    pline("Clonk!");
    }

    return hits;
}

#endif /* OVL0 */

/*mthrowu.c*/
