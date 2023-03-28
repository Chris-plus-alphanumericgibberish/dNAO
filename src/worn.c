/*	SCCS Id: @(#)worn.c	3.4	2003/01/08	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <math.h>
#include "hack.h"
void FDECL(mon_block_extrinsic, (struct monst *, struct obj *, int, boolean, boolean));
boolean FDECL(mon_gets_extrinsic, (struct monst *, int, struct obj *));
STATIC_DCL void FDECL(update_mon_intrinsic, (struct monst *,struct obj *,int,BOOLEAN_P,BOOLEAN_P));
STATIC_DCL void FDECL(m_dowear_type, (struct monst *,long, BOOLEAN_P, BOOLEAN_P));
STATIC_DCL int NDECL(def_beastmastery);
STATIC_DCL int NDECL(def_vilya);
STATIC_DCL int NDECL(def_narya);
STATIC_DCL int NDECL(def_lomya);
STATIC_DCL int NDECL(def_mountedCombat);

const struct worn {
	long w_mask;
	struct obj **w_obj;
} worn[] = {
	{ W_ARM, &uarm },
	{ W_ARMC, &uarmc },
	{ W_ARMH, &uarmh },
	{ W_ARMS, &uarms },
	{ W_ARMG, &uarmg },
	{ W_ARMF, &uarmf },
#ifdef TOURIST
	{ W_ARMU, &uarmu },
#endif
	{ W_RINGL, &uleft },
	{ W_RINGR, &uright },
	{ W_WEP, &uwep },
	{ W_SWAPWEP, &uswapwep },
	{ W_QUIVER, &uquiver },
	{ W_AMUL, &uamul },
	{ W_TOOL, &ublindf },
	{ W_BALL, &uball },
	{ W_CHAIN, &uchain },
	{ 0, 0 }
};

/* This only allows for one blocking item per property */
#define w_blocks(o,m) \
		((is_mummy_wrap(o) && ((m) & W_ARMC)) ? INVIS : \
		 (o->otyp == CORNUTHAUM && ((m) & W_ARMH) && \
			!(Role_if(PM_WIZARD) || Race_if(PM_INCANTIFIER))) ? CLAIRVOYANT : 0)
		/* note: monsters don't have clairvoyance, so your role
		   has no significant effect on their use of w_blocks() */


/* returns TRUE if obj confers prop
 * also checks artifact properties
 */
boolean
item_has_property(obj, prop)
struct obj * obj;
int prop;
{
	int property_list[LAST_PROP];
	int i;
	/* first check object (ocl, oprops) */
	get_item_property_list(property_list, obj, obj->otyp);
	for (i = 0; property_list[i] != 0; i++)	{
		if (property_list[i] == prop)
			return TRUE;
	}
	/* then while-worn artifact properties */
	get_art_property_list(property_list, obj->oartifact, FALSE);
	for (i = 0; property_list[i] != 0; i++)	{
		if (property_list[i] == prop)
			return TRUE;
	}
	/* then while-carried artifact properties */
	get_art_property_list(property_list, obj->oartifact, TRUE);
	for (i = 0; property_list[i] != 0; i++)	{
		if (property_list[i] == prop)
			return TRUE;
	}
	return FALSE;
}

/* 
 * Fills an int array propert_list with all the properties (from prop.h) an item has
 *
 * If called without an obj, uses otyp to give as much info as possible without knowing obj
 */
void
get_item_property_list(property_list, obj, otyp)
int * property_list;
struct obj* obj;
int otyp;
{
	int cur_prop, i, j;
	boolean got_prop;

	if (obj)
		otyp = obj->otyp;

	i = 0;
	for (cur_prop = 1; cur_prop < LAST_PROP; cur_prop++)
	{
		got_prop = FALSE;
		// from objclass
		j = 0;
		while(objects[otyp].oc_oprop[j] && !got_prop) {
			if (objects[otyp].oc_oprop[j] == cur_prop)
				got_prop = TRUE;
			j++;
		}

		// from object properties
		if (!got_prop && obj && !check_oprop(obj, OPROP_NONE)){
			switch (cur_prop)
			{
			case FIRE_RES:
				if (check_oprop(obj, OPROP_FIRE) || check_oprop(obj, OPROP_WOOL))
					got_prop = TRUE;
				break;
			case COLD_RES:
				if (check_oprop(obj, OPROP_COLD) || check_oprop(obj, OPROP_WOOL))
					got_prop = TRUE;
				break;
			case SHOCK_RES:
				if (check_oprop(obj, OPROP_ELEC))
					got_prop = TRUE;
				break;
			case ACID_RES:
				if (check_oprop(obj, OPROP_ACID))
					got_prop = TRUE;
				break;
			case ANTIMAGIC:
				if (check_oprop(obj, OPROP_MAGC))
					got_prop = TRUE;
				break;
			case REFLECTING:
				if (check_oprop(obj, OPROP_REFL))
					got_prop = TRUE;
				break;
			case DISINT_RES:
				if (check_oprop(obj, OPROP_DISN))
					got_prop = TRUE;
				break;
			case LIFESAVED:
				if (check_oprop(obj, OPROP_LIFE))
					got_prop = TRUE;
				break;
			}
		}
		if(!got_prop && obj && is_imperial_elven_armor(obj)){
			switch(cur_prop){
				case MAGICAL_BREATHING:
					if(check_imp_mod(obj, IEA_NOBREATH))
						got_prop = TRUE;
				break;
				case LIFESENSE:
					if(check_imp_mod(obj, IEA_LIFESENSE))
						got_prop = TRUE;
				break;
				case SEE_INVIS:
					if(check_imp_mod(obj, IEA_SEE_INVIS))
						got_prop = TRUE;
				break;
				case TELEPAT:
					if(check_imp_mod(obj, IEA_TELEPAT))
						got_prop = TRUE;
				break;
				case BLIND_RES:
					if(check_imp_mod(obj, IEA_BLIND_RES))
						got_prop = TRUE;
				break;
				case TELEPORT_CONTROL:
					if(check_imp_mod(obj, IEA_TELE_CNTRL))
						got_prop = TRUE;
				break;
				case FIXED_ABIL:
					if(check_imp_mod(obj, IEA_FIXED_ABIL))
						got_prop = TRUE;
				break;
				case REFLECTING:
					if(check_imp_mod(obj, IEA_REFLECTING))
						got_prop = TRUE;
				break;
				case SICK_RES:
					if(check_imp_mod(obj, IEA_SICK_RES))
						got_prop = TRUE;
				break;
				case HALF_PHDAM:
					if(check_imp_mod(obj, IEA_HALF_PHDAM))
						got_prop = TRUE;
				break;
				case HALF_SPDAM:
					if(check_imp_mod(obj, IEA_HALF_SPDAM))
						got_prop = TRUE;
				break;
				case DISPLACED:
					if(check_imp_mod(obj, IEA_DISPLACED))
						got_prop = TRUE;
				break;
				case INVIS:
					if(check_imp_mod(obj, IEA_INVIS))
						got_prop = TRUE;
				break;
				case SWIMMING:
					if(check_imp_mod(obj, IEA_SWIMMING))
						got_prop = TRUE;
				break;
				case FLYING:
					if(check_imp_mod(obj, IEA_FLYING))
						got_prop = TRUE;
				break;
				case JUMPING:
					if(check_imp_mod(obj, IEA_JUMPING))
						got_prop = TRUE;
				break;
				case FAST:
					if(check_imp_mod(obj, IEA_FAST))
						got_prop = TRUE;
				break;
				case TELEPORT:
					if(check_imp_mod(obj, IEA_TELEPORT))
						got_prop = TRUE;
				break;
				case PROT_FROM_SHAPE_CHANGERS:
					if(check_imp_mod(obj, IEA_PROT_SHAPE))
						got_prop = TRUE;
				break;
			}
		}
		// if we've got the property, add it to the array
		if (got_prop)
		{
			property_list[i] = cur_prop;
			i++;
		}
	}
	// add a terminator to the array
	property_list[i] = 0;
	return;	
}

/* Updated to use the extrinsic and blocked fields. */
void
setworn(obj, mask)
register struct obj *obj;
long mask;
{
	register const struct worn *wp;
	register struct obj *oobj;
	register int p;
	
	/*Handle the pen of the void here*/
	if(obj && obj->oartifact == ART_PEN_OF_THE_VOID){
		if(obj->ovar1_seals && !Role_if(PM_EXILE)){
			long oldseals = u.sealsKnown;
			u.sealsKnown |= obj->ovar1_seals;
			if(oldseals != u.sealsKnown) You("learned new seals.");
		}
		obj->ovar1_seals = u.spiritTineA|u.spiritTineB;
		if(u.voidChime){
			int i;
			for(i=0; i<u.sealCounts; i++){
				obj->ovar1_seals |= u.spirit[i];
			}
		}
	} else if(obj && obj->oartifact == ART_HELM_OF_THE_ARCANE_ARCHER){
      if(P_UNSKILLED == OLD_P_SKILL(P_ATTACK_SPELL)) OLD_P_SKILL(P_ATTACK_SPELL) = P_BASIC;
      if(P_BASIC     == OLD_P_SKILL(P_ATTACK_SPELL)) OLD_P_SKILL(P_ATTACK_SPELL) = P_SKILLED;
    }
	
	if ((mask & (W_ARM | W_SKIN)) == (W_ARM | W_SKIN)) {
	    /* restoring saved game; no properties are conferred via skin */
	    uskin = obj;
	 /* assert( !uarm ); */
	} else {
	    for(wp = worn; wp->w_mask; wp++) if(wp->w_mask & mask) {
		oobj = *(wp->w_obj);
		if(oobj && !(oobj->owornmask & wp->w_mask))
			impossible("Setworn: mask = %ld.", wp->w_mask);
		if(oobj) {
		    if (u.twoweap && (oobj->owornmask & (W_WEP|W_SWAPWEP)) && !test_twoweapon())
				u.twoweap = 0;
		    oobj->owornmask &= ~wp->w_mask;
		    if (wp->w_mask & ~(W_SWAPWEP|W_QUIVER)) {
			/* leave as "x = x <op> y", here and below, for broken
			 * compilers */			
			int property_list[LAST_PROP];
			get_item_property_list(property_list, oobj, oobj->otyp);
			p = 0;
			while (property_list[p] != 0)	{
				u.uprops[property_list[p]].extrinsic = u.uprops[property_list[p]].extrinsic & ~wp->w_mask;
				p++;
			}
			
			if ((p = w_blocks(oobj,mask)) != 0)
			    u.uprops[p].blocked &= ~wp->w_mask;
			if (oobj->oartifact)
			    set_artifact_intrinsic(oobj, 0, mask);
		    }
		}
		*(wp->w_obj) = obj;
		if(obj) {
		    obj->owornmask |= wp->w_mask;
		    /* Prevent getting/blocking intrinsics from wielding
		     * potions, through the quiver, etc.
		     * Allow weapon-tools, too.
		     * wp_mask should be same as mask at this point.
		     */
		    if (wp->w_mask & ~(W_SWAPWEP|W_QUIVER)) {
			if (obj->oclass == WEAPON_CLASS || is_weptool(obj) ||
					    mask != W_WEP) {
				int property_list[LAST_PROP];
				get_item_property_list(property_list, obj, obj->otyp);
				p = 0;
				while (property_list[p] != 0)	{
					u.uprops[property_list[p]].extrinsic = u.uprops[property_list[p]].extrinsic | wp->w_mask;
					p++;
				}
			    if ((p = w_blocks(obj, mask)) != 0)
				u.uprops[p].blocked |= wp->w_mask;
			}
			if (obj->oartifact)
			    set_artifact_intrinsic(obj, 1, mask);
		    }
		}
	    }
	}
	if(!restoring) {
		vision_full_recalc = 1;
		see_monsters(); //More objects than just artifacts grant warning now, and this is a convienient place to add a failsafe see_monsters check
		update_inventory();
	}
}

/* called e.g. when obj is destroyed */
/* Updated to use the extrinsic and blocked fields. */
void
setnotworn(obj)
register struct obj *obj;
{
	register const struct worn *wp;
	register int p;

	if (!obj) return;
	if (obj->oartifact && obj->oartifact == ART_HELM_OF_THE_ARCANE_ARCHER){
      if(P_BASIC   == OLD_P_SKILL(P_ATTACK_SPELL)) OLD_P_SKILL(P_ATTACK_SPELL) = P_UNSKILLED;
      if(P_SKILLED == OLD_P_SKILL(P_ATTACK_SPELL)) OLD_P_SKILL(P_ATTACK_SPELL) = P_BASIC;
    }
	for(wp = worn; wp->w_mask; wp++)
	    if(obj == *(wp->w_obj)) {
		*(wp->w_obj) = 0;
		int property_list[LAST_PROP];
		get_item_property_list(property_list, obj, obj->otyp);
		p = 0;
		while (property_list[p] != 0)	{
			u.uprops[property_list[p]].extrinsic = u.uprops[property_list[p]].extrinsic & ~wp->w_mask;
			p++;
		}
		
		if(obj->oartifact == ART_GAUNTLETS_OF_THE_BERSERKER){
//        adj_abon(uarmg, -uarmg->ovar1_gober);
          uarmg->ovar1_gober = 0;
        }
		obj->owornmask &= ~wp->w_mask;
		if (obj->oartifact)
		    set_artifact_intrinsic(obj, 0, wp->w_mask);
		if ((p = w_blocks(obj,wp->w_mask)) != 0)
		    u.uprops[p].blocked &= ~wp->w_mask;
	    }
	update_inventory();
	if ((obj == uwep || obj == uswapwep) && !test_twoweapon()) u.twoweap = 0;
	
}

void
mon_set_minvis(mon)
struct monst *mon;
{
	mon->perminvis = 1;
	if (!mon->invis_blkd) {
	    mon->minvis = 1;
	    if (opaque(mon->data) && !See_invisible(mon->mx, mon->my))
			unblock_point(mon->mx, mon->my);
	    newsym(mon->mx, mon->my);		/* make it disappear */
	    if (mon->wormno) see_wsegs(mon);	/* and any tail too */
	}
}

void
mon_adjust_speed(mon, adjust, obj, verbose)
struct monst *mon;
int adjust;	/* positive => increase speed, negative => decrease */
struct obj *obj;	/* item to make known if effect can be seen */
boolean verbose;
{
    boolean give_msg = !in_mklev, petrify = FALSE;
    unsigned int oldspeed = mon->mspeed;

    switch (adjust) {
     case  2:
	mon->permspeed = MFAST;
	give_msg = FALSE;	/* special case monster creation */
	break;
     case  1:
	if (mon->permspeed == MSLOW) mon->permspeed = 0;
	else mon->permspeed = MFAST;
	break;
     case  0:			/* just check for worn speed boots */
	break;
     case -1:
	if (mon->permspeed == MFAST) mon->permspeed = 0;
	else mon->permspeed = MSLOW;
	break;
     case -2:
	mon->permspeed = MSLOW;
	give_msg = FALSE;	/* (not currently used) */
	break;
     case -3:			/* petrification */
	/* take away intrinsic speed but don't reduce normal speed */
	if (mon->permspeed == MFAST) mon->permspeed = 0;
	petrify = TRUE;
	break;
    }

    if (mon_extrinsic(mon, FAST))		/* speed boots or other extrinsic source */
	mon->mspeed = MFAST;
    else
	mon->mspeed = mon->permspeed;

    if (give_msg && (mon->mspeed != oldspeed || petrify) && canseemon(mon)) {
	/* fast to slow (skipping intermediate state) or vice versa */
	const char *howmuch = (mon->mspeed + oldspeed == MFAST + MSLOW) ?
				"much " : "";

	if (petrify) {
	    /* mimic the player's petrification countdown; "slowing down"
	       even if fast movement rate retained via worn speed boots */
	    if (flags.verbose && verbose) pline("%s is slowing down.", Monnam(mon));
	} else if (adjust > 0 || mon->mspeed == MFAST)
	    if (is_weeping(mon->data)) {
		if(verbose)
			pline("%s is suddenly changing positions %sfaster.", Monnam(mon), howmuch);
	    } else {
		if(verbose)
			pline("%s is suddenly moving %sfaster.", Monnam(mon), howmuch);
	    }
	else
	    if (is_weeping(mon->data)) {
		if(verbose)
			pline("%s is suddenly changing positions %sslower.", Monnam(mon), howmuch);
	    } else {
		if(verbose)
			pline("%s seems to be moving %sslower.", Monnam(mon), howmuch);
	    }

	/* might discover an object if we see the speed change happen, but
	   avoid making possibly forgotten book known when casting its spell */
	if (obj != 0 && obj->dknown &&
		objects[obj->otyp].oc_class != SPBOOK_CLASS)
	    makeknown(obj->otyp);
    }
}

/* update a blocked extrinsic
 * assumes single source of each blocked extrinsic
 */
void
mon_block_extrinsic(mon, obj, which, on, silently)
struct monst *mon;
struct obj *obj;
int which;
boolean on, silently;
{
	if (on) {
		switch (which)
		{
		case INVIS:
			if (mon->mtyp != PM_HELLCAT){
				mon->invis_blkd = TRUE;
				update_mon_intrinsic(mon, obj, which, !on, silently);
			}
			break;
		default:
			update_mon_intrinsic(mon, obj, which, !on, silently);
			break;
		}
	}
	else { /* off */
		switch (which)
		{
		case INVIS:
			if (mon->mtyp != PM_HELLCAT){
				mon->invis_blkd = FALSE;
				if (mon_gets_extrinsic(mon, which, obj))
					update_mon_intrinsic(mon, obj, which, !on, silently);
				if (mon->perminvis)
					mon->minvis = TRUE;
			}
			break;
		default:
			if (mon_gets_extrinsic(mon, which, obj))
				update_mon_intrinsic(mon, obj, which, !on, silently);
			break;
		}
	}
}

/* find out if a monster gets a certain extrinsic from its equipment 
 * if given an ignored_obj, does not consider it to give any extrinsics
 */
boolean
mon_gets_extrinsic(mon, which, ignored_obj)
struct monst *mon;
int which;
struct obj *ignored_obj;
{
	struct obj *otmp;					/* item in mon's inventory */
	boolean got_prop = FALSE;			/* property to find */
	int tmp_property_list[LAST_PROP];	/* list of item/artifact properties */
	int i;								/* loop counter */

	for (otmp = mon->minvent; (otmp && !got_prop); otmp = otmp->nobj){
		/* ignore one object in particular */
		if (otmp == ignored_obj)
			continue;

		/* worn items */
		if (otmp->owornmask) {
			get_item_property_list(tmp_property_list, otmp, otmp->otyp);
			for (i = 0; tmp_property_list[i]; i++)
			{
				if (tmp_property_list[i] == which)
					got_prop = TRUE;
			}
		}
		/* worn artifacts */
		if (otmp->owornmask && otmp->oartifact){
			get_art_property_list(tmp_property_list, otmp->oartifact, FALSE);
			//tmp_property_list = art_property_list(otmp->oartifact, FALSE);
			for (i = 0; tmp_property_list[i]; i++)
			{
				if (tmp_property_list[i] == which)
					got_prop = TRUE;
			}
		}
		/* carried artifacts */
		if (otmp->oartifact){
			get_art_property_list(tmp_property_list, otmp->oartifact, TRUE);
			//tmp_property_list = art_property_list(otmp->oartifact, TRUE);
			for (i = 0; tmp_property_list[i]; i++)
			{
				if (tmp_property_list[i] == which)
					got_prop = TRUE;
			}
		}
	}
	return got_prop;
}

STATIC_OVL void
update_mon_intrinsic(mon, obj, which, on, silently)
struct monst *mon;
struct obj *obj;
int which;
boolean on, silently;
{
    uchar mask;
    struct obj *otmp;
	boolean oldprop = mon_resistance(mon, which);;

    if (on) {
		/* some properties need special handling */
		switch (which)
		{
		case INVIS:
			if (mon->mtyp != PM_HELLCAT){
				mon->mextrinsics[(which-1)/32] |= (1 << (which-1)%32);
				mon->minvis = !mon->invis_blkd;
			}
			break;
		case FAST:
			{
			mon->mextrinsics[(which-1)/32] |= (1 << (which-1)%32);
			boolean save_in_mklev = in_mklev;
			if (silently) in_mklev = TRUE;
			mon_adjust_speed(mon, 0, obj, TRUE);
			in_mklev = save_in_mklev;
			
			break;
			}
		case LEVITATION:
		case FLYING:
			mon->mextrinsics[(which-1)/32] |= (1 << (which-1)%32);
			if (!oldprop && (mon_resistance(mon,LEVITATION) || mon_resistance(mon,FLYING))) {
				m_float_up(mon, silently);
				if (obj && !silently && canseemon(mon)) {
					makeknown(obj->otyp);
				}
			}
			break;
		case DISPLACED:
			mon->mextrinsics[(which-1)/32] |= (1 << (which-1)%32);
			if (!oldprop && mon_resistance(mon,DISPLACED) && !silently && canseemon(mon)) {
				pline("%s outline begins shimmering!", s_suffix(Monnam(mon)));
				if (obj) {
					makeknown(obj->otyp);
				}
			}
			break;
		case SWIMMING:
			mon->mextrinsics[(which-1)/32] |= (1 << (which-1)%32);
			if (!oldprop && mon_resistance(mon,SWIMMING)) {
				minliquid(mon);
			}
			break;
		default:
			mon->mextrinsics[(which-1)/32] |= (1 << (which-1)%32);
			break;
		}
    }
	else { /* off */
		/* we need to check that this property isn't being granted by any other equipment */
		if (!mon_gets_extrinsic(mon, which, obj)) {
			/* again, some properties need special handling */
			switch (which)
			{
			case INVIS:
				mon->mextrinsics[(which-1)/32] &= ~(1 << (which-1)%32);
				mon->minvis = (mon->invis_blkd ? FALSE : mon->perminvis);
				break;
			case FAST:
				{
				mon->mextrinsics[(which-1)/32] &= ~(1 << (which-1)%32);
				boolean save_in_mklev = in_mklev;
				if (silently) in_mklev = TRUE;
				mon_adjust_speed(mon, 0, obj, TRUE);
				in_mklev = save_in_mklev;
				break;
				}
			case LEVITATION:
			case FLYING:
				mon->mextrinsics[(which-1)/32] &= ~(1 << (which-1)%32);
				if (oldprop && !mon_resistance(mon,LEVITATION) && !mon_resistance(mon,FLYING)) {
					m_float_down(mon, silently);
					if (obj && !silently && canseemon(mon)) {
						makeknown(obj->otyp);
					}
				}
				break;
			case DISPLACED:
				mon->mextrinsics[(which-1)/32] &= ~(1 << (which-1)%32);
				if (oldprop && !mon_resistance(mon,DISPLACED) && !silently && canseemon(mon)) {
					pline("%s outline stops shimmering.", s_suffix(Monnam(mon)));
					if (obj) {
						makeknown(obj->otyp);
					}
				}
				break;
			case SWIMMING:
				mon->mextrinsics[(which-1)/32] &= ~(1 << (which-1)%32);
				if (oldprop && !mon_resistance(mon,SWIMMING)) {
					minliquid(mon);
				}
				break;
			default:
				mon->mextrinsics[(which-1)/32] &= ~(1 << (which-1)%32);
				break;
			}
		}
    }
	return;
}

/* armor put on, taken off, grabbed, or dropped; might be magical variety */
void
update_mon_intrinsics(mon, obj, on, silently)
struct monst *mon;
struct obj *obj;
boolean on, silently;
{
	/* dead monsters shouldn't print messages about them no longer getting their intrinsics */
	if (DEADMONSTER(mon))
		silently = TRUE;

	int unseen = !canseemon(mon);
    int which;
    long all_worn = ~0L; /* clang lint */
	int property_list[LAST_PROP];
	get_item_property_list(property_list, obj, obj->otyp);
	/* only turn on properties from this list if obj is worn */
	if (!on || obj->owornmask) {
		which = 0;
		while (property_list[which] != 0)	{
			update_mon_intrinsic(mon, obj, property_list[which], on, silently);
			which++;
		}
	}
	if (obj->oartifact)
	{
		/* only turn on properties from this list if obj is worn */
		if (!on || obj->owornmask) {
			get_art_property_list(property_list, obj->oartifact, FALSE);
			which = 0;
			while (property_list[which] != 0)	{
				update_mon_intrinsic(mon, obj, property_list[which], on, silently);
				which++;
			}
		}
		/* while-carried properties */
		get_art_property_list(property_list, obj->oartifact, TRUE);
		which = 0;
		while (property_list[which] != 0)	{
			update_mon_intrinsic(mon, obj, property_list[which], on, silently);
			which++;
		}
	}
	/* if the object blocks an extrinsic, recalculate if the monster should get that extrinsic */
	/* use all_worn because the owornmask may have been cleared already and monsters will not wield armor */
	if ((which = w_blocks(obj, all_worn)))
		mon_block_extrinsic(mon, obj, which, on, silently);

#ifdef STEED
	if (!on && mon == u.usteed && obj->otyp == SADDLE)
	    dismount_steed(DISMOUNT_FELL);
#endif

    /* if couldn't see it but now can, or vice versa, update display */
    if (!silently && (unseen ^ !canseemon(mon)))
		newsym(mon->mx, mon->my);
}

int 
base_mac(mon)
struct monst *mon;
{
	int base = 10, armac = 0;
	struct obj *monwep;
	
	if(mon->mtyp == PM_OONA || mon->mtyp == PM_PORO_AULON){
		base -= mon->data->nac*mon->mhp/mon->mhpmax;
	}
	else{
		base -= mon->data->nac;
	}
	if(!mon->mcan){
		base -= mon->data->pac;
		if(mon->mtyp == PM_CENTER_OF_ALL && u.uinsight < 32)
			base -= (32-u.uinsight)/2;
	}
	
	if(mon->mtyp == PM_ASMODEUS && base < -9) base = -9 + MONSTER_AC_VALUE(base+9);
	else if(mon->mtyp == PM_PALE_NIGHT && base < -6) base = -6 + MONSTER_AC_VALUE(base+6);
	else if(mon->mtyp == PM_BAALPHEGOR && base < -8) base = -8 + MONSTER_AC_VALUE(base+8);
	else if(mon->mtyp == PM_ZAPHKIEL && base < -8) base = -8 + MONSTER_AC_VALUE(base+8);
	else if(mon->mtyp == PM_QUEEN_OF_STARS && base < -6) base = -6 + MONSTER_AC_VALUE(base+6);
	else if(mon->mtyp == PM_ETERNAL_LIGHT && base < -6) base = -6 + MONSTER_AC_VALUE(base+6);
	else if(mon->mtyp == PM_STRANGE_CORPSE && base < -5) base = -5 + MONSTER_AC_VALUE(base+5);
	else if(mon->mtyp == PM_ANCIENT_OF_DEATH && base < -4) base = -4 + MONSTER_AC_VALUE(base+4);
	else if(mon->mtyp == PM_CHOKHMAH_SEPHIRAH){
		base -= u.chokhmah;
	}
	else if(is_weeping(mon->data)){
		if(mon->mvar2 & 0x4L) base = -125; //Fully Quantum Locked
		if(mon->mvar2 & 0x2L) base = -20; //Partial Quantum Lock
	}
	else if(mon->mtyp == PM_CHAOS && !PURIFIED_WIND){
		base -= 6;
	}

	if(is_alabaster_mummy(mon->data) && mon->mvar_syllable == SYLLABLE_OF_GRACE__UUR)
		base -= 10;
	
	if (mon->mtame){
		base -= rnd(def_beastmastery());
		if (uarm && uarm->oartifact == ART_BEASTMASTER_S_DUSTER && is_animal(mon->data))
			base -= rnd(def_beastmastery()); // the duster doubles for tame animals
		
		if(u.usteed && mon==u.usteed) base -= rnd(def_mountedCombat());
		
		if(uring_art(ART_VILYA) && def_vilya())
			base -=  sgn(def_vilya())*rnd(abs(def_vilya()));

		if(uring_art(ART_NARYA) && def_narya())
			base -= sgn(def_narya())*rnd(abs(def_narya()));
	}

	monwep = MON_WEP(mon);
	if(monwep){
		if(monwep->oartifact == ART_LANCE_OF_LONGINUS) base -= max((monwep->spe+1)/2,0);
		else if(monwep->oartifact == ART_TENSA_ZANGETSU){
			base -= max( (monwep->spe+1)/2,0);
			if(!uarmc || !uarm) base -= max( monwep->spe,0);
			if(!uarmc && !uarm) base -= max( (monwep->spe+1)/2,0);
		}
		else if(monwep->oartifact == ART_LASH_OF_THE_COLD_WASTE){
			if(u.uinsight >= 20)
				base -= 10;
			else if(u.uinsight > 10)
				base -= u.uinsight - 10;
		}
		if(monwep->obj_material == MERCURIAL){
			int level = monwep->ocarry->m_lev;
			//Streaming
			if(mon_merc_streaming(monwep)){
				if(level < 3);
				else if(level < 10)
					base -= 1;
				else if(level < 18)
					base -= 2;
				else
					base -= 3;
			}
			//Kinstealing
			else if(mon_merc_kinstealing(monwep)){
				if(level < 10);
				else if(level < 18)
					base -= 1;
				else
					base -= 2;
			}
			//Chained
			else {
				if(level < 3);
				else if(level < 10)
					base -= 1;
				else if(level < 18)
					base -= 4;
				else
					base -= 6;
			}
		}
	}

	if (helpless(mon) || mon->msuicide)
		base += 5;
	else {
		struct obj * armor = which_armor(mon, W_ARM);
		register int mondodgeac = mon->data->dac;
		if(u.ustuck == mon)
			mondodgeac = -5;
		if ((mondodgeac < 0)						/* penalties have full effect */
			|| (!armor)								/* no armor = max mobility */
			|| (armor && is_light_armor(armor))		/* light armor is also fine  */
			)
			base -= mondodgeac;
		else if (armor && is_medium_armor(armor))	/* medium armor halves dodge AC */
			base -= mondodgeac/2;
		/* else no adjustment */
	}
	
	return base;
}

int
find_mac(mon)
struct monst *mon;
{
	struct obj *obj;
	int base, armac = 0;
	long mwflags = mon->misc_worn_check;
	
	base = base_mac(mon);
	
	if(mon->mtyp == PM_GIANT_TURTLE && mon->mflee){
		base -= 15;
	}
	//Block attack with weapon
	if(mon != &youmonst &&
		(mon->mtyp == PM_MARILITH 
		|| mon->mtyp == PM_SHAKTARI
		|| mon->mtyp == PM_CATHEZAR
	)){
		int wcount = 0;
		struct obj *otmp;
		for(otmp = mon->minvent; otmp; otmp = otmp->nobj){
			if((otmp->oclass == WEAPON_CLASS || is_weptool(otmp)
				|| (otmp->otyp == CHAIN && mon->mtyp == PM_CATHEZAR)
				) && !otmp->oartifact
				&& otmp != MON_WEP(mon) && otmp != MON_SWEP(mon)
				&& !otmp->owornmask
				&& ++wcount >= 4
			) break;
		}
		if(MON_WEP(mon))
			wcount++;
		if(MON_SWEP(mon))
			wcount++;
		if(rn2(6) < wcount){
			base -= rnd(20);
		}
	}
	
	if(mon->mtame){
		if(u.specialSealsActive&SEAL_COSMOS) base -= spiritDsize();	
	}
	
	
	//armor AC
	if(mon->mtyp == PM_HOD_SEPHIRAH){
		if(uarm) armac += arm_ac_bonus(uarm);
		if(uarmf) armac += arm_ac_bonus(uarmf);
		if(uarmg) armac += arm_ac_bonus(uarmg);
		if(uarmu) armac += arm_ac_bonus(uarmu);
		if(uarms){
			armac += max(0, arm_ac_bonus(uarms) + (uarms->objsize - mon->data->msize));
		}
		if(uarmh) armac += arm_ac_bonus(uarmh);
		if(uarmc) armac += arm_ac_bonus(uarmc);
		if(armac < 0) armac *= -1;
	}
	else for (obj = mon->minvent; obj; obj = obj->nobj) {
	    if (obj->owornmask & mwflags){
			if(is_shield(obj))
				armac += max(0, arm_ac_bonus(obj) + (obj->objsize - mon->data->msize));
			else
				armac += arm_ac_bonus(obj);
		}
	}
	if(armac > 11) armac = rnd(armac-10) + 10; /* high armor ac values act like player ac values */
	
	if (wizard && (iflags.wizcombatdebug & WIZCOMBATDEBUG_ACCURACY) && WIZCOMBATDEBUG_APPLIES((struct monst *)0, mon)) {
		pline("base: %d, armac: %d", base, armac);
	}
	
	base -= armac;
	/* since arm_ac_bonus is positive, subtracting it increases AC */
	return base;
}

int
full_mac(mon)
struct monst *mon;
{
	struct obj *obj;
	int base = 10, armac = 0;
	long mwflags = mon->misc_worn_check;
	
	if(mon->mtyp == PM_OONA || mon->mtyp == PM_PORO_AULON){
		base -= mon->data->nac*mon->mhp/mon->mhpmax;
	}
	else {
		base -= mon->data->nac;
	}
	if(!mon->mcan){
		base -= mon->data->pac;
		if(mon->mtyp == PM_CENTER_OF_ALL && u.uinsight < 32)
			base -= (32-u.uinsight)/2;
	}
	
	if(mon->mtyp == PM_CHOKHMAH_SEPHIRAH){
		base -= u.chokhmah;
	}
	else if(is_weeping(mon->data)){
		if(mon->mvar2 & 0x4L) base = -125; //Fully Quantum Locked
		if(mon->mvar2 & 0x2L) base = -20; //Partial Quantum Lock
	}
	else if(mon->mtyp == PM_GIANT_TURTLE && mon->mflee){
		base -= 15;
	}
	else if(mon != &youmonst &&
		(mon->mtyp == PM_MARILITH 
		|| mon->mtyp == PM_SHAKTARI
		|| mon->mtyp == PM_CATHEZAR
	)){
		int wcount = 0;
		struct obj *otmp;
		for(otmp = mon->minvent; otmp; otmp = otmp->nobj){
			if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp)
				|| (otmp->otyp == CHAIN && !otmp->owornmask && mon->mtyp == PM_CATHEZAR)
			){
				base -= 20;
				break;
			}
		}
	}
	else if(mon->mtyp == PM_CHAOS && !PURIFIED_WIND){
		base -= 6;
	}
	
	if(is_alabaster_mummy(mon->data) && mon->mvar_syllable == SYLLABLE_OF_GRACE__UUR)
		base -= 10;
	
	if(mon->mtame){
		base -= def_beastmastery();
		if(u.specialSealsActive&SEAL_COSMOS) base -= spiritDsize();
		if(u.usteed && mon==u.usteed) base -= def_mountedCombat();
		
		if (uarm && uarm->oartifact == ART_BEASTMASTER_S_DUSTER && is_animal(mon->data))
			base -= def_beastmastery(); // the duster doubles for tame animals

		if(uring_art(ART_VILYA))
			base -= def_vilya();

		if(uring_art(ART_NARYA))
			base -= def_narya();
	}
	
	if(mon->mtyp == PM_HOD_SEPHIRAH){
		if(uarm) armac += arm_ac_bonus(uarm);
		if(uarmf) armac += arm_ac_bonus(uarmf);
		if(uarmg) armac += arm_ac_bonus(uarmg);
		if(uarmu) armac += arm_ac_bonus(uarmu);
		if(uarms){
			armac += max(0, arm_ac_bonus(uarms) + (uarms->objsize - mon->data->msize));
		}
		if(uarmh) armac += arm_ac_bonus(uarmh);
		if(uarmc) armac += arm_ac_bonus(uarmc);
		
		if(armac < 0) armac *= -1;
	}
	else for (obj = mon->minvent; obj; obj = obj->nobj) {
	    if (obj->owornmask & mwflags){
			if(is_shield(obj))
				armac += max(0, arm_ac_bonus(obj) + (obj->objsize - mon->data->msize));
			else
				armac += arm_ac_bonus(obj);
		}
	}

	base -= armac;
	
	if (helpless(mon) || mon->msuicide)
		base += 5;
	else {
		struct obj * armor = which_armor(mon, W_ARM);
		register int mondodgeac = mon->data->dac;
		if(u.ustuck == mon)
			mondodgeac = -5;
		if ((mondodgeac < 0)						/* penalties have full effect */
			|| (!armor)								/* no armor = max mobility */
			|| (armor && is_light_armor(armor))		/* light armor is also fine  */
			)
			base -= mondodgeac;
		else if (armor && is_medium_armor(armor))	/* medium armor halves dodge AC */
			base -= mondodgeac / 2;
		/* else no adjustment */
	}
	
	/* since arm_ac_bonus is positive, subtracting it increases AC */
	return base;
}

int
full_marmorac(mon)
struct monst *mon;
{
	struct obj *obj;
	int armac = 0;
	long mwflags = mon->misc_worn_check;
	
	if(mon->mtyp == PM_GIANT_TURTLE && mon->mflee){
		armac += 15;
	}
	
	if(mon->mtyp == PM_DANCING_BLADE){
		return -20;
	}
	
	if(mon->mtyp == PM_HOD_SEPHIRAH){
		if(uarm) armac += arm_ac_bonus(uarm);
		if(uarmf) armac += arm_ac_bonus(uarmf);
		if(uarmg) armac += arm_ac_bonus(uarmg);
		if(uarmu) armac += arm_ac_bonus(uarmu);
		if(uarms){
			armac += max(0, arm_ac_bonus(uarms) + (uarms->objsize - mon->data->msize));
		}
		if(uarmh) armac += arm_ac_bonus(uarmh);
		if(uarmc) armac += arm_ac_bonus(uarmc);
		
		if(armac < 0) armac *= -1;
	}
	else for (obj = mon->minvent; obj; obj = obj->nobj) {
	    if (obj->owornmask & mwflags){
			if(is_shield(obj))
				armac += max(0, arm_ac_bonus(obj) + (obj->objsize - mon->data->msize));
			else
				armac += arm_ac_bonus(obj);
		}
	}

	return 10 - armac;
}

int 
base_nat_mdr(mon)
struct monst *mon;
{
	int base = 0;
	
	if(is_weeping(mon->data)){
		if(mon->mvar2 & 0x4L) base = +125; //Fully Quantum Locked
		if(mon->mvar2 & 0x2L) base = +5; //Partial Quantum Lock
	}
	
	if(mon->mtame){
		if(u.specialSealsActive&SEAL_COSMOS) base += rnd(spiritDsize());
	}
	
	return base;
}

int
base_mdr(mon)
struct monst *mon;
{
	int base = 0;
	
	if(mon->mtyp == PM_CHOKHMAH_SEPHIRAH){
		base += u.chokhmah;
	}
	else if(mon->mtyp == PM_CHAOS && !PURIFIED_WIND){
		base += 4;
	}
	
	if(mon->mtame){
		if(active_glyph(IMPURITY)) base += 3;
		if(Role_if(PM_HEALER))
			base += heal_mlevel_bonus();

		if(uring_art(ART_LOMYA))
			base += def_lomya();
	}
	if(is_alabaster_mummy(mon->data) && mon->mvar_syllable == SYLLABLE_OF_SPIRIT__VAUL)
		base += 10;
	
	return base;
}

int
avg_spell_mdr(mon)
struct monst *mon;
{
	int base = 0;
	
	if(mon->mtyp == PM_CHOKHMAH_SEPHIRAH){
		base += u.chokhmah;
	}
	else if(mon->mtyp == PM_CHAOS && !PURIFIED_WIND){
		base += 4;
	}
	
	if(mon->mtame){
		if(active_glyph(IMPURITY)) base += 3;
		if(Role_if(PM_HEALER))
			base += heal_mlevel_bonus();

		if(uring_art(ART_LOMYA))
			base += def_lomya();
	}
	if(is_alabaster_mummy(mon->data) && mon->mvar_syllable == SYLLABLE_OF_SPIRIT__VAUL)
		base += 10;
	

	if(!mon->mcan){
		int dr = 0;
#define m_bdr mon->data->spe_bdr
#define m_ldr mon->data->spe_ldr
#define m_hdr mon->data->spe_hdr
#define m_fdr mon->data->spe_fdr
#define m_gdr mon->data->spe_gdr
		dr += m_bdr*2;
		dr += m_ldr*2;

		if (has_head_mon(mon))			dr += m_hdr;
		else							dr += m_bdr;

		if (can_wear_boots(mon->data))	dr += m_fdr;
		else							dr += m_ldr;

		if (can_wear_gloves(mon->data))	dr += m_gdr;
		else							dr += m_bdr;
		
#undef m_bdr
#undef m_ldr
#undef m_hdr
#undef m_fdr
#undef m_gdr
		base += (dr / 7);
		if(mon->mtyp == PM_CENTER_OF_ALL && u.uinsight < 32)
			base += (33-u.uinsight)/2;

		if(mon->mtyp == PM_OONA && mon->mhp < mon->mhpmax/2){
			base += 7;
		}
		if(mon->mtyp == PM_PORO_AULON && mon->mhp < mon->mhpmax/2){
			base += 1;
		}
	}
	return base;
}

int
roll_mdr(mon, magr)
struct monst *mon;
struct monst *magr;
{
	return roll_mdr_detail(mon, magr, 0, 0);
}

int
roll_mdr_detail(mon, magr, slot, depth)
struct monst *mon;
struct monst *magr;
int slot;
int depth;
{
	int base, nat_dr, armac;
	
	if(!slot) switch(rn2(7)){
		case 0:
		case 1:
			slot = UPPER_TORSO_DR;
		break;
		case 2:
		case 3:
			slot = LOWER_TORSO_DR;
		break;
		case 4:
			slot = HEAD_DR;
		break;
		case 5:
			slot = LEG_DR;
		break;
		case 6:
			slot = ARM_DR;
		break;
	}
	
	mon_slot_dr(mon, magr, slot, &base, &armac, &nat_dr, depth);

	//Star spawn reach extra-dimensionally past all armor, even bypassing natural armor.
	if(magr && (magr->mtyp == PM_STAR_SPAWN || magr->mtyp == PM_GREAT_CTHULHU || magr->mtyp == PM_VEIL_RENDER || (magr->mtyp == PM_LADY_CONSTANCE && !rn2(2)) || mad_monster_turn(magr, MAD_NON_EUCLID))){
		armac = 0;
		if(undiffed_innards(mon->data))
			nat_dr /= 2;
		else if(!no_innards(mon->data) && !removed_innards(mon->data))
			nat_dr = 0;
	}
	
	if(armac > 11) armac = rnd(armac-10) + 10; /* high armor dr values act like player ac values */
	
	if(nat_dr && armac){
		base += sqrt(nat_dr*nat_dr + armac*armac);
	} else if(nat_dr){
		base += nat_dr;
	} else {
		base += armac;
	}
	
	/* since arm_ac_bonus is positive, subtracting it increases AC */
	return base;
}

/* Calculates mon's DR for a slot 
 * Does not randomize values >10 (must be done elsewhere)
 * 
 * Includes effectiveness vs magr (optional)
 */
void
mon_slot_dr(mon, magr, slot, base_dr_out, armor_dr_out, natural_dr_out, depth)
struct monst *mon;
struct monst *magr;
int slot;
int *base_dr_out;
int *armor_dr_out;
int *natural_dr_out;
int depth;
{
	/* DR addition: bas + sqrt(nat^2 + arm^2) (not done in this function) */
	int bas_mdr; /* base DR:    magical-ish   */
	int nat_mdr; /* natural DR: (poly)form    */
	int arm_mdr; /* armor DR:   worn armor    */
	boolean blip_humanoid_armor = FALSE;

	bas_mdr = base_mdr(mon);
	nat_mdr = base_nat_mdr(mon);
	arm_mdr = 0;

	/* for use vs specific magr */
	int agralign = 0;
	int agrmoral = 0;
	if(magr){
		agralign = (magr == &youmonst) ? sgn(u.ualign.type) : sgn(magr->data->maligntyp);
		
		if(magr == &youmonst){
			if(hates_holy(youracedata))
				agrmoral = -1;
			else if(hates_unholy(youracedata))
				agrmoral = 1;
		} else {
			if(hates_holy_mon(magr))
				agrmoral = -1;
			else if(hates_unholy_mon(magr))
				agrmoral = 1;
		}
	}
	
	/* some slots may be unacceptable and must be replaced */
	if (magr && magr->mtyp == PM_XAN)
		slot = LEG_DR;
	if (mon->mtyp == PM_GIANT_TURTLE && (mon->mflee || rn2(2)))
		slot = UPPER_TORSO_DR;
	if (slot == HEAD_DR && !has_head_mon(mon))
		slot = UPPER_TORSO_DR;
	if (slot == LEG_DR && !can_wear_boots(mon->data))
		slot = LOWER_TORSO_DR;
	if (slot == ARM_DR && !can_wear_gloves(mon->data))
		slot = UPPER_TORSO_DR;
	if(mon->mtyp == PM_BLIBDOOLPOOLP_S_MINDGRAVEN_CHAMPION && magr && !depth){
		if(slot != LEG_DR && rn2(3)){
			slot = LOWER_TORSO_DR;
		}
		else {
			blip_humanoid_armor = TRUE;
		}
	}

	/* DR of worn armor */
	int marmor[] = { W_ARM,          				W_ARMC,         					  W_ARMF, W_ARMH,  W_ARMG, W_ARMS, W_ARMU };
	int adfalt[] = { UPPER_TORSO_DR|LOWER_TORSO_DR, UPPER_TORSO_DR|LOWER_TORSO_DR|LEG_DR, LEG_DR, HEAD_DR, ARM_DR, 0,     UPPER_TORSO_DR };
	int i;
	struct obj * curarm;
	for (i = 0; i < SIZE(marmor); i++) {
		if((curarm = which_armor(mon, marmor[i]))){
			if(curarm->oclass == ARMOR_CLASS){
				if (curarm && ((objects[curarm->otyp].oc_dtyp & slot) || (!objects[curarm->otyp].oc_dtyp && (slot&adfalt[i])))) {
					if(depth && higher_depth(objects[curarm->otyp].oc_armcat, depth))
						continue;
					if(marmor[i] == W_ARM && slot == LOWER_TORSO_DR && mon->mtyp == PM_BLIBDOOLPOOLP_S_MINDGRAVEN_CHAMPION && !blip_humanoid_armor && magr && !depth){
						if(!full_body_match(mon->data, curarm))
							continue;
					}
					arm_mdr += arm_dr_bonus(curarm);
					if (magr) arm_mdr += properties_dr(curarm, agralign, agrmoral);
				}
			}
			else if(!depth){
				if (slot&adfalt[i]){
					arm_mdr += arm_dr_bonus(curarm);
					if (magr) arm_mdr += properties_dr(curarm, agralign, agrmoral);
				}
			}
		}
	}
	/* Tensa Zangetsu adds to worn armor */
	if(MON_WEP(mon)){
		if (MON_WEP(mon)->oartifact == ART_TENSA_ZANGETSU) {
			if (!which_armor(mon, W_ARMC) && (slot & CLOAK_DR)) {
				arm_mdr += max(1 + (MON_WEP(mon)->spe + 1) / 2, 0);
			}
			if (!which_armor(mon, W_ARM) && (slot & TORSO_DR)) {
				arm_mdr += max(1 + (MON_WEP(mon)->spe + 1) / 2, 0);
			}
		}
		else if(MON_WEP(mon)->oartifact == ART_LASH_OF_THE_COLD_WASTE){
			if(u.uinsight >= 40)
				bas_mdr += 5;
			else if(u.uinsight > 20)
				bas_mdr += (u.uinsight - 20)/4;
		}
	}
	/* Hod Sephirah OVERRIDE other arm_mdr sources with the player's total DR (regardless of who's attacking them) */
	if (mon->mtyp == PM_HOD_SEPHIRAH) {
		arm_mdr = slot_udr(slot, magr, 0);
	}
	/* Natural DR */
	int slotnatdr;
	switch (slot)
	{
	case UPPER_TORSO_DR: slotnatdr = mon->data->bdr; break;
	case LOWER_TORSO_DR: slotnatdr = mon->data->ldr; break;
	case HEAD_DR:        slotnatdr = mon->data->hdr; break;
	case LEG_DR:         slotnatdr = mon->data->fdr; break;
	case ARM_DR:         slotnatdr = mon->data->gdr; break;
	}
	if(mon->mtyp == PM_OONA || mon->mtyp == PM_PORO_AULON){
		slotnatdr = slotnatdr*mon->mhp/mon->mhpmax;
	}
	nat_mdr += slotnatdr;
	if (!mon->mcan) {
		switch (slot)
		{
		case UPPER_TORSO_DR: bas_mdr += mon->data->spe_bdr; break;
		case LOWER_TORSO_DR: bas_mdr += mon->data->spe_ldr; break;
		case HEAD_DR:        bas_mdr += mon->data->spe_hdr; break;
		case LEG_DR:         bas_mdr += mon->data->spe_fdr; break;
		case ARM_DR:         bas_mdr += mon->data->spe_gdr; break;
		}
		if(mon->mtyp == PM_CENTER_OF_ALL && u.uinsight < 32)
			bas_mdr += (33-u.uinsight)/2;

		if(mon->mtyp == PM_OONA && mon->mhp < mon->mhpmax/2){
			bas_mdr += 7;
		}
		if(mon->mtyp == PM_PORO_AULON && mon->mhp < mon->mhpmax/2){
			bas_mdr += 1;
		}
	}

	*base_dr_out    = bas_mdr;
	*armor_dr_out   = arm_mdr;
	*natural_dr_out = nat_mdr;
	
	return;
}

int
avg_mdr(mon)
struct monst *mon;
{
	int i;
	int sum = 0;
	int base, nat_dr, armac;
	int slot;
	
	for (i = 0; i < 7; i++){
		switch(i){
			case 0:
			case 1:
				slot = UPPER_TORSO_DR;
			break;
			case 2:
			case 3:
				slot = LOWER_TORSO_DR;
			break;
			case 4:
				slot = HEAD_DR;
			break;
			case 5:
				slot = LEG_DR;
			break;
			case 6:
				slot = ARM_DR;
			break;
		}
		
		mon_slot_dr(mon, (struct monst *) 0, slot, &base, &armac, &nat_dr, 0);

		if(armac > 11) armac = (armac-10)/2 + 10; /* high armor dr values act like player ac values */
		
		if(nat_dr && armac){
			base += sqrt(nat_dr*nat_dr + armac*armac);
		} else if(nat_dr){
			base += nat_dr;
		} else {
			base += armac;
		}
		sum += base;
	}

	return sum / 7;
}

int
mdat_avg_mdr(mon)
struct monst * mon;
{
	/* only looks at a monster's base stats with minimal adjustment (and no worn armor) */
	/* used for pokedex entry */
	int dr = 0;
#define m_bdr (mon->data->bdr + mon->data->spe_bdr)
#define m_ldr (mon->data->ldr + mon->data->spe_ldr)
#define m_hdr (mon->data->hdr + mon->data->spe_hdr)
#define m_fdr (mon->data->fdr + mon->data->spe_fdr)
#define m_gdr (mon->data->gdr + mon->data->spe_gdr)

	dr += m_bdr*2;
	dr += m_ldr*2;

	if (has_head_mon(mon))			dr += m_hdr;
	else							dr += m_bdr;

	if (can_wear_boots(mon->data))	dr += m_fdr;
	else							dr += m_ldr;

	if (can_wear_gloves(mon->data))	dr += m_gdr;
	else							dr += m_bdr;
	
#undef m_bdr
#undef m_ldr
#undef m_hdr
#undef m_fdr
#undef m_gdr

	return (dr / 7);
}

/* weapons are handled separately; rings and eyewear aren't used by monsters */

/* Wear the best object of each type that the monster has.  During creation,
 * the monster can put everything on at once; otherwise, wearing takes time.
 * This doesn't affect monster searching for objects--a monster may very well
 * search for objects it would not want to wear, because we don't want to
 * check which_armor() each round.
 *
 * We'll let monsters put on shirts and/or suits under worn cloaks, but
 * not shirts under worn suits.  This is somewhat arbitrary, but it's
 * too tedious to have them remove and later replace outer garments,
 * and preventing suits under cloaks makes it a little bit too easy for
 * players to influence what gets worn.  Putting on a shirt underneath
 * already worn body armor is too obviously buggy...
 */

void
m_dowear(mon, creation)
register struct monst *mon;
boolean creation;
{
#define RACE_EXCEPTION TRUE
	/* Note the restrictions here are the same as in dowear in do_wear.c
	 * except for the additional restriction on intelligence.  (Players
	 * are always intelligent, even if polymorphed).
	 *
	 * Give animals and mindless creatures a chance to wear their initial
	 * equipment.
	 */
	if(mad_no_armor(mon))
		return;
	if(mon->mtyp == PM_BLIBDOOLPOOLP__GRAVEN_INTO_FLESH)
		return;
	if ((is_animal(mon->data) || mindless_mon(mon)) && !creation)
		return;

	m_dowear_type(mon, W_AMUL, creation, FALSE);
#ifdef TOURIST
	/* can't put on shirt if already wearing suit */
	if (!(mon->misc_worn_check & W_ARM) || creation)
	    m_dowear_type(mon, W_ARMU, creation, FALSE);
#endif
	/* treating small as a special case allows
	   hobbits, gnomes, and kobolds to wear cloaks */
	m_dowear_type(mon, W_ARMC, creation, FALSE);
	m_dowear_type(mon, W_ARMH, creation, FALSE);
	if (!MON_WEP(mon) || !bimanual(MON_WEP(mon),mon->data))
	    m_dowear_type(mon, W_ARMS, creation, FALSE);
	m_dowear_type(mon, W_ARMG, creation, FALSE);
	m_dowear_type(mon, W_ARMF, creation, FALSE);
	m_dowear_type(mon, W_ARM, creation, FALSE);
	m_dowear_type(mon, W_TOOL, creation, FALSE);
}

STATIC_OVL void
m_dowear_type(mon, flag, creation, racialexception)
struct monst *mon;
long flag;
boolean creation;
boolean racialexception;
{
	struct obj *old, *best, *obj;
	int m_delay = 0;
	int unseen = !canseemon(mon);
	char nambuf[BUFSZ];

	if (mon->mfrozen) return; /* probably putting previous item on */
	
	if(is_whirly(mon->data) || noncorporeal(mon->data)) return;

	/* Get a copy of monster's name before altering its visibility */
	Strcpy(nambuf, See_invisible(mon->mx,mon->my) ? Monnam(mon) : mon_nam(mon));

	old = which_armor(mon, flag);
	if (old && old->cursed && !is_weldproof_mon(mon)) return;
	if (old && old->otyp == STATUE && (old->corpsenm == PM_PARASITIC_MIND_FLAYER || old->corpsenm == PM_PARASITIC_MASTER_MIND_FLAYER))
		return;
	if (old && flag == W_AMUL) return; /* no such thing as better amulets */
	best = old;

	for(obj = mon->minvent; obj; obj = obj->nobj) {
		//Special case: can't wear most torso armor
		if (mon->mtyp == PM_HARROWER_OF_ZARIEL
		 && ((is_suit(obj) && arm_blocks_upper_body(obj->otyp))
			|| is_shirt(obj)
		)){
			continue;
		}
	    switch(flag) {
		case W_AMUL:
		    if (obj->oclass != AMULET_CLASS ||
				!can_wear_amulet(mon->data) || 
			    !searches_for_item(mon, obj))
			continue;
		    best = obj;
		    goto outer_break; /* no such thing as better amulets */
		case W_ARMU:
		    if (!is_shirt(obj) || obj->objsize != mon->data->msize || !shirt_match(mon->data,obj)) continue;
		    break;
		case W_ARMC:
			if(mon->mtyp == PM_CATHEZAR && obj->otyp == CHAIN)
				break;
		    if (!is_cloak(obj) || (abs(obj->objsize - mon->data->msize) > 1)) continue;
		    break;
		case W_ARMH:
			if(mon->mtyp == PM_CATHEZAR && obj->otyp == CHAIN)
				break;
		    if (!is_helmet(obj) || !helm_match(mon->data,obj) || !helm_size_fits(mon->data,obj)) continue;
		    break;
		case W_ARMS:
		    if (noshield(mon->data) || (mon_offhand_attack(mon) && !creation) || !is_shield(obj)) continue;
		    break;
		case W_ARMG:
			if((mon->mtyp == PM_CATHEZAR || mon->mtyp == PM_WARDEN_ARIANNA) && obj->otyp == CHAIN)
				break;
		    if (!is_gloves(obj) || obj->objsize != mon->data->msize || !can_wear_gloves(mon->data)) continue;
		    break;
		case W_ARMF:
			if((mon->mtyp == PM_WARDEN_ARIANNA) && obj->otyp == CHAIN)
				break;
		    if (!is_boots(obj) || !boots_size_fits(mon->data, obj) || !can_wear_boots(mon->data)) continue;
		    break;
		case W_ARM:
			if((mon->mtyp == PM_CATHEZAR || mon->mtyp == PM_WARDEN_ARIANNA) && obj->otyp == CHAIN)
				break;
		    if (!is_suit(obj) || !arm_match(mon->data, obj) || !arm_size_fits(mon->data, obj))
				continue;
		    break;
		case W_TOOL:
		    if(!is_worn_tool(obj)) continue;
		    if(!can_wear_blindf(mon->data) || (is_opaque_worn_tool(obj) && !(obj->otyp == R_LYEHIAN_FACEPLATE && is_mind_flayer(mon->data))) ) continue;
		    break;
	    }
	    if (obj->owornmask) continue;
	    /* I'd like to define a VISIBLE_arm_ac_bonus which doesn't assume the
	     * monster knows obj->spe, but if I did that, a monster would keep
	     * switching forever between two -2 caps since when it took off one
	     * it would forget spe and once again think the object is better
	     * than what it already has.
	     */
	    if (best && (arm_total_bonus(best) + extra_pref(mon,best) >= arm_total_bonus(obj) + extra_pref(mon,obj)))
		continue;
	    best = obj;
	}
outer_break:
	if (!best || best == old) return;

	/* if wearing a cloak, account for the time spent removing
	   and re-wearing it when putting on a suit or shirt */
	if ((flag == W_ARM
	  || flag == W_ARMU
			  ) && (mon->misc_worn_check & W_ARMC))
	    m_delay += 2;
	/* when upgrading a piece of armor, account for time spent
	   taking off current one */
	if (old)
	    m_delay += objects[old->otyp].oc_delay;

	if (old) /* do this first to avoid "(being worn)" */
	    old->owornmask = 0L;
	if (!creation) {
	    if (canseemon(mon)) {
		char buf[BUFSZ];

		if (old)
		    Sprintf(buf, " removes %s and", distant_name(old, doname));
		else
		    buf[0] = '\0';
		pline("%s%s puts on %s.", Monnam(mon),
		      buf, distant_name(best,doname));
	    } /* can see it */
	    m_delay += objects[best->otyp].oc_delay;
	    mon->mfrozen = m_delay;
	    if (mon->mfrozen) mon->mcanmove = 0;
	}
	if (old)
	    update_mon_intrinsics(mon, old, FALSE, creation);
	mon->misc_worn_check |= flag;
	best->owornmask |= flag;
	update_mon_intrinsics(mon, best, TRUE, creation);
	/* if couldn't see it but now can, or vice versa, */
	if (!creation && (unseen ^ !canseemon(mon))) {
		if (mon->minvis && !See_invisible(mon->mx,mon->my)) {
			pline("Suddenly you cannot see %s.", nambuf);
			makeknown(best->otyp);
		} /* else if (!mon->minvis) pline("%s suddenly appears!", Amonnam(mon)); */
	}
}
#undef RACE_EXCEPTION

struct obj *
mon_remove_armor(mon, flag)
struct monst *mon;
long flag;
{
	struct obj *old;
	int m_delay = 0;

	old = which_armor(mon, flag);

	if(!old)
		return (struct obj *)0;
	
	if ((flag == W_ARM
	  || flag == W_ARMU
	) && (mon->misc_worn_check & W_ARMC))
	    m_delay += 2;
	m_delay += objects[old->otyp].oc_delay;
	old->owornmask = 0L;
	mon->mfrozen = max(mon->mfrozen, m_delay);
	if(mon->mfrozen)
		mon->mcanmove = 0;
	update_mon_intrinsics(mon, old, FALSE, FALSE);
	mon->misc_worn_check &= ~flag;
	return old;
}

boolean
mon_throw_armor(mon)
struct monst *mon;
{
	struct obj *old;
	long flag;
	int seen = canseemon(mon);
	int tarx, tary;
	int tries = 10;
	
	if (mon->mfrozen) return FALSE;
	
	do switch(rnd(7)){
		case 1:
			flag = W_ARM;
		break;
		case 2:
			flag = W_ARMC;
		break;
		case 3:
			flag = W_ARMH;
		break;
		case 4:
			flag = W_ARMG;
		break;
		case 5:
			flag = W_ARMF;
		break;
		case 6:
			flag = W_ARMU;
		break;
		case 7:
			flag = W_AMUL;
		break;
	} while(tries-- && !(old = which_armor(mon, flag)));

	if(!old) return FALSE;
	
	old = mon_remove_armor(mon, flag);
	
	if(!old) return FALSE; //Shouldn't occur since we already checked this, but perhaps remove_armor will support welded armor.

	if(seen)
		pline("%s frantically removes %s.", Monnam(mon), distant_name(old, doname));
	do {
		tarx = rn2(17)-8+mon->mx;
		tary = rn2(17)-8+mon->my;
	} while((tarx == mon->mx && tary == mon->my) || !isok(tarx, tary));
	mthrow(mon, old, 0, tarx, tary, FALSE);
	return TRUE;
}

boolean
mon_strip_armor(mon)
struct monst *mon;
{
	struct obj *old;
	long flag;
	int seen = canseemon(mon);
	int i;
	
	if (mon->mfrozen) return FALSE;
	
	for(i = 1; i<=7;i++){
		switch(rnd(7)){
			case 1:
				flag = W_ARM;
			break;
			case 2:
				flag = W_ARMC;
			break;
			case 3:
				flag = W_ARMH;
			break;
			case 4:
				flag = W_ARMG;
			break;
			case 5:
				flag = W_ARMF;
			break;
			case 6:
				flag = W_ARMU;
			break;
			case 7:
				flag = W_AMUL;
			break;
		}

		if(!which_armor(mon, flag)) continue;
	
		old = mon_remove_armor(mon, flag);

		if(!old) continue; //Shouldn't occur since we already checked this, but perhaps remove_armor will support welded armor.

		if(seen){
			seen = FALSE;
			pline("%s removes %s clothes.", Monnam(mon), mhis(mon));
		}
	}
	return TRUE;
}

struct obj *
which_armor(mon, flag)
struct monst *mon;
long flag;
{
	register struct obj *obj;

	for(obj = mon->minvent; obj; obj = obj->nobj)
		if (obj->owornmask & flag) return obj;
	return((struct obj *)0);
}

/* remove an item of armor and then drop it */
void
m_lose_armor(mon, obj)
struct monst *mon;
struct obj *obj;
{
	mon->misc_worn_check &= ~obj->owornmask;
	if (obj->owornmask)
	    update_mon_intrinsics(mon, obj, FALSE, FALSE);
	obj->owornmask = 0L;

	obj_extract_self(obj);
	place_object(obj, mon->mx, mon->my);
	/* call stackobj() if we ever drop anything that can merge */
	newsym(mon->mx, mon->my);
}

/* all objects with their bypass bit set should now be reset to normal */
void
clear_bypasses()
{
	struct obj *otmp, *nobj;
	struct monst *mtmp;
	int i;

	for (otmp = fobj; otmp; otmp = nobj) {
	    nobj = otmp->nobj;
	    if (otmp->bypass) {
		otmp->bypass = 0;
		/* bypass will have inhibited any stacking, but since it's
		   used for polymorph handling, the objects here probably
		   have been transformed and won't be stacked in the usual
		   manner afterwards; so don't bother with this */
#if 0
		if (objects[otmp->otyp].oc_merge) {
		    xchar ox, oy;

		    (void) get_obj_location(otmp, &ox, &oy, 0);
		    stack_object(otmp);
		    newsym(ox, oy);
		}
#endif	/*0*/
	    }
	}
	/* invent and mydogs chains shouldn't matter here */
	for(i=0;i<10;i++)
		for (otmp = magic_chest_objs[i]; otmp; otmp = otmp->nobj)
			otmp->bypass = 0;
	for (otmp = migrating_objs; otmp; otmp = otmp->nobj)
	    otmp->bypass = 0;
	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (DEADMONSTER(mtmp)) continue;
	    for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		otmp->bypass = 0;
	}
	for (mtmp = migrating_mons; mtmp; mtmp = mtmp->nmon) {
	    for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
		otmp->bypass = 0;
	}
	flags.bypasses = FALSE;
}

void
bypass_obj(obj)
struct obj *obj;
{
	obj->bypass = 1;
	flags.bypasses = TRUE;
}

void
mon_break_armor(mon, polyspot)
struct monst *mon;
boolean polyspot;
{
	register struct obj *otmp;
	struct permonst *mdat = mon->data;
	boolean vis = cansee(mon->mx, mon->my);
	const char *pronoun = mhim(mon),
			*ppronoun = mhis(mon);

	if ((otmp = which_armor(mon, W_ARM)) != 0) {
		if ((Is_dragon_scales(otmp) &&
			mdat == Dragon_scales_to_pm(otmp)) ||
			(Is_dragon_mail(otmp) && mdat == Dragon_mail_to_pm(otmp)))
			m_useup(mon, otmp);	/* no message here;
			   "the dragon merges with his scaly armor" is odd
			   and the monster's previous form is already gone */
		else if(!arm_size_fits(mon->data, otmp) || !arm_match(mon->data,otmp) || is_whirly(mon->data) || noncorporeal(mon->data)){
			if (otmp->oartifact || otmp->objsize > mon->data->msize || is_whirly(mon->data) || noncorporeal(mon->data)) {
				if (vis)
					pline("%s armor falls around %s!",
						s_suffix(Monnam(mon)), pronoun);
				else
					You_hear("a thud.");
				if (polyspot) bypass_obj(otmp);
				m_lose_armor(mon, otmp);
			} else if (vis){
				pline("%s breaks out of %s armor!", Monnam(mon), ppronoun);
				m_useup(mon, otmp);
			} else {
				You_hear("a cracking sound.");
				m_useup(mon, otmp);
			}
		}
	}
	if ((otmp = which_armor(mon, W_ARMC)) != 0) {
		if(abs(otmp->objsize - mon->data->msize) > 1 || is_whirly(mon->data) || noncorporeal(mon->data)){
			if (otmp->oartifact || otmp->objsize > mon->data->msize || is_whirly(mon->data) || noncorporeal(mon->data)) {
				if (vis)
				pline("%s %s falls off!", s_suffix(Monnam(mon)),
					cloak_simple_name(otmp));
				if (polyspot) bypass_obj(otmp);
				m_lose_armor(mon, otmp);
			} else {
				if (vis)
				pline("%s %s tears apart!", s_suffix(Monnam(mon)),
					cloak_simple_name(otmp));
				else
				You_hear("a ripping sound.");
				m_useup(mon, otmp);
			}
		}
	}
	if ((otmp = which_armor(mon, W_ARMU)) != 0) {
		if(otmp->objsize != mon->data->msize || !shirt_match(mon->data,otmp) || is_whirly(mon->data) || noncorporeal(mon->data)){
			if (otmp->oartifact || otmp->objsize > mon->data->msize || is_whirly(mon->data) || noncorporeal(mon->data)) {
				if (vis)
				pline("%s %s falls off!", s_suffix(Monnam(mon)),
					cloak_simple_name(otmp));
				if (polyspot) bypass_obj(otmp);
				m_lose_armor(mon, otmp);
			} else {
				if (vis)
					pline("%s shirt rips to shreds!", s_suffix(Monnam(mon)));
				else
					You_hear("a ripping sound.");
				m_useup(mon, otmp);
			}
		}
	}
	if ((otmp = which_armor(mon, W_ARMG)) != 0) {
		if(nogloves(mon->data) || nolimbs(mon->data) || otmp->objsize != mon->data->msize || is_whirly(mon->data) || noncorporeal(mon->data)){
			if (vis)
				pline("%s drops %s gloves!", Monnam(mon), ppronoun);
			if (polyspot) bypass_obj(otmp);
			m_lose_armor(mon, otmp);
		}
	}
	if ((otmp = which_armor(mon, W_ARMS)) != 0) {
		if(nohands(mon->data) || nolimbs(mon->data) || bimanual(MON_WEP(mon),mon->data) || is_whirly(mon->data) || noncorporeal(mon->data)){
			if (vis)
				pline("%s can no longer hold %s shield!", Monnam(mon), ppronoun);
			else
				You_hear("a clank.");
			if (polyspot) bypass_obj(otmp);
			m_lose_armor(mon, otmp);
		}
	}
	if ((otmp = which_armor(mon, W_ARMH)) != 0 &&
		/* flimsy test for horns matches polyself handling */
		(!helm_match(mon->data, otmp) || !helm_size_fits(mon->data, otmp) || is_whirly(mon->data) || noncorporeal(mon->data) )
	) {
		if (vis)
			pline("%s helmet falls to the %s!",
			  s_suffix(Monnam(mon)), surface(mon->mx, mon->my));
		else
			You_hear("a clank.");
		if (polyspot) bypass_obj(otmp);
		m_lose_armor(mon, otmp);
	}
	if ((otmp = which_armor(mon, W_ARMF)) != 0) {
		if(((noboots(mon->data) || !humanoid(mon->data)) && !can_wear_boots(mon->data)) || !boots_size_fits(mon->data, otmp) || is_whirly(mon->data) || noncorporeal(mon->data)){
			if (vis) {
				if (is_whirly(mon->data) || noncorporeal(mon->data))
					pline("%s %s falls, unsupported!",
							 s_suffix(Monnam(mon)), cloak_simple_name(otmp));
				else pline("%s boots %s off %s feet!",
				s_suffix(Monnam(mon)),
				mon->data->msize < otmp->objsize ? "slide" : "are pushed", ppronoun);
			}
			if (polyspot) bypass_obj(otmp);
			m_lose_armor(mon, otmp);
		}
	}
#ifdef STEED
	if (!can_saddle(mon)) {
	    if ((otmp = which_armor(mon, W_SADDLE)) != 0) {
		if (polyspot) bypass_obj(otmp);
		m_lose_armor(mon, otmp);
		if (vis)
		    pline("%s saddle falls off.", s_suffix(Monnam(mon)));
	    }
	    if (mon == u.usteed)
		goto noride;
	} else if (mon == u.usteed && !can_ride(mon)) {
	noride:
	    You("can no longer ride %s.", mon_nam(mon));
	    if (touch_petrifies(u.usteed->data) &&
			!Stone_resistance && rnl(100) >= 33) {
		char buf[BUFSZ];

		You("touch %s.", mon_nam(u.usteed));
		Sprintf(buf, "falling off %s",
				an(u.usteed->data->mname));
		instapetrify(buf);
	    }
	    dismount_steed(DISMOUNT_FELL);
	}
#endif
	return;
}

/* bias a monster's preferences towards armor that has special benefits. */
int
extra_pref(mon, obj)
struct monst *mon;
struct obj *obj;
{
	if (!obj)
		return 0;

	int score = 0;

	/* specific item types that are more than their oc_oprop */
	switch (obj->otyp)
	{
		/* gloves */
	case GAUNTLETS_OF_POWER:
		score += 8;
		break;
	case GAUNTLETS_OF_DEXTERITY:
		score += (obj->spe / 2);
		break;
		/* cloaks */
	case ALCHEMY_SMOCK:
		if (!species_resists_acid(mon) || !species_resists_poison(mon))
			score += 5;
		break;
	case MUMMY_WRAPPING:
	case PRAYER_WARDED_WRAPPING:
		if (mon->data->mlet == S_MUMMY)
			score += 30;
		else if (mon->mtame && mon->minvis && !See_invisible_old)
			score += 10;
		else if (mon->minvis)
			score += -5;
		break;
		/* armor */
	case EILISTRAN_ARMOR:
		score += 10;
		break;
		/* facewear */
	case LIVING_MASK:
		score += 3;
		break;
	case SUNGLASSES:
		score += 2;
		break;
	case ANDROID_VISOR:
		if(is_android(mon)) score += 4;
		score += 1;
		break;
	}

	int j;
	for (j=0; objects[obj->otyp].oc_oprop[j]; j++) {
		switch (objects[obj->otyp].oc_oprop[j])
		{
		case ANTIMAGIC:
			if (!species_resists_magic(mon))
				score += 20;
			break;
		case REFLECTING:
			if (!(has_template(mon, FRACTURED) || species_reflects(mon)))
				score += 18;
			break;
		case FAST:
			if (mon->permspeed != MFAST)
				score += 15;
			break;
		case FLYING:
			if (!species_flies(mon->data))
				score += 10;
			break;
		case DISPLACED:
			if (!species_displaces(mon->data))
				score += 8;
			break;
		case STONE_RES:
			if (!species_resists_ston(mon))
				score += 7;
			break;
		case SICK_RES:
			if (!species_resists_sickness(mon))
				score += 5;
			break;
		case FIRE_RES:
			if (!species_resists_fire(mon))
				score += 3;
		case COLD_RES:
			if (!species_resists_cold(mon))
				score += 3;
		case SHOCK_RES:
			if (!species_resists_elec(mon))
				score += 3;
		case ACID_RES:
			if (!species_resists_acid(mon))
				score += 3;
		case POISON_RES:
			if (!species_resists_poison(mon))
				score += 3;
		case SLEEP_RES:
			if (!species_resists_sleep(mon))
				score += 3;
		case DRAIN_RES:
			if (!species_resists_drain(mon))
				score += 3;
			break;
		case TELEPAT:
			if (!species_is_telepathic(mon->data))
				score += 1;
			break;
		case FUMBLING:
			score += -20;
			break;
		/* pets prefer not to wear items that make themselves invisible to you */
		case INVIS:
			if (mon->mtame && !See_invisible_old)
				score += -20;
			else if (!pm_invisible(mon->data))
				score += 5;
			break;
		}
	}

    return score;
}

/* magic_negation()
 * 
 * armor that sufficiently covers the body might be able to block magic 
 */
int
magic_negation(mon)
struct monst *mon;
{
	struct obj *armor;
	int armpro = 0;
	int cpro = 0;
	
	if(u.sealsActive&SEAL_PAIMON && mon == &youmonst) return 3;

	armor = (mon == &youmonst) ? uarm : which_armor(mon, W_ARM);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmc : which_armor(mon, W_ARMC);
	if(armor){
		cpro = armor->otyp == DROVEN_CLOAK ? 
			objects[armor->otyp].a_can - armor->oeroded3 :
			objects[armor->otyp].a_can;
		if(armpro < cpro) armpro = cpro;
	}
	armor = (mon == &youmonst) ? uarmh : which_armor(mon, W_ARMH);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	if(mon == &youmonst && !uarmc && 
		uwep && uwep->oartifact==ART_TENSA_ZANGETSU) armpro = max(armpro, 2); //magic cancelation for tensa zangetsu

	/* armor types for shirt, gloves, shoes, and shield may not currently
	   provide any magic cancellation but we should be complete */
	armor = (mon == &youmonst) ? uarmu : which_armor(mon, W_ARMU);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmg : which_armor(mon, W_ARMG);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmf : which_armor(mon, W_ARMF);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarms : which_armor(mon, W_ARMS);
	if (armor && armpro < 3){
	    armpro = min(3, armpro+objects[armor->otyp].a_can);
	}

#ifdef STEED
	/* this one is really a stretch... */
	armor = (mon == &youmonst) ? 0 : which_armor(mon, W_SADDLE);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
#endif

	return armpro;
}

void
light_damage(arg, timeout)
genericptr_t arg;
long timeout;
{
	struct obj *obj = (struct obj *) arg;;
 	xchar x = 0, y = 0;
	boolean on_floor = obj->where == OBJ_FLOOR,
		in_invent = obj->where == OBJ_INVENT,
		in_trap = obj->where == OBJ_INTRAP;
	
	if(obj->shopOwned){
		start_timer(1, TIMER_OBJECT,
					LIGHT_DAMAGE, (genericptr_t)obj);
		return;
	}

//	pline("checking light damage");
	if (on_floor) {
	    x = obj->ox;
	    y = obj->oy;
		if (dimness(x, y) > 0){
			if(obj->oeroded && obj->oerodeproof) obj->oeroded--;
			start_timer(1, TIMER_OBJECT,
						LIGHT_DAMAGE, (genericptr_t)obj);
			return;
		}
		if(obj->oeroded < 2){
			obj->oeroded++;
			start_timer(1, TIMER_OBJECT,
						LIGHT_DAMAGE, (genericptr_t)obj);
			return;
		}else{
			if(obj->otyp == NOBLE_S_DRESS){
				obj = poly_obj(obj, PLAIN_DRESS);
				obj->oeroded = 0;
			} else {
				obj_extract_self(obj);
				obfree(obj, (struct obj *)0);
			}
		}
	}
	else if (in_trap) {
		x = obj->otrap->tx;
		y = obj->otrap->ty;
		/* if dark, continue timer and possibly restore durability */
		if ((dimness(x, y) > 0)
			/* only some traps are visible to light */
			|| !(obj->otrap->ttyp == BEAR_TRAP || obj->otrap->ttyp == FLESH_HOOK)
			){
			if (obj->oeroded && obj->oerodeproof) obj->oeroded--;
			start_timer(1, TIMER_OBJECT,
				LIGHT_DAMAGE, (genericptr_t)obj);
			return;
		}
		/* otherwise, damage and eventually destroy it */
		if (obj->oeroded < 2){
			obj->oeroded++;
			start_timer(1, TIMER_OBJECT,
				LIGHT_DAMAGE, (genericptr_t)obj);
			return;
		}
		else {
			struct monst *mtmp;
			/* destroy trap as well */
			if (x == u.ux && y == u.uy) {
				/* message */
				if (u.utrap) {
					pline("The %s holding you evaporates.", xname(obj));
				}
				u.utrap = 0;
				u.utraptype = 0;
			}
			else if ((mtmp = m_at(x, y)) != 0) {
				if (mtmp->mtrapped) {
					if (cansee(x, y) && canseemon(mtmp))
						pline("The %s holding %s evaporates.", xname(obj), mon_nam(mtmp));
					mtmp->mtrapped = 0;
				}
			}
			deltrap(obj->otrap);	/* deltrap deletes contained objects as well */
		}
	}
	else if (in_invent) {
//		pline("object in invent");
		int armpro = 0;
		boolean isarmor = obj == uarm || obj == uarmc || obj == uarms || obj == uarmh || 
					obj == uarmg || obj == uarmf || obj == uarms;
		if(uarmc){
			armpro = uarmc->otyp == DROVEN_CLOAK ? 
				objects[uarmc->otyp].a_can - uarmc->oeroded3 :
				objects[uarmc->otyp].a_can;
		}
		
		if (Invis){
			start_timer(1, TIMER_OBJECT, LIGHT_DAMAGE, (genericptr_t)obj);
			return;
		}
		
		if ((!u.uswallow ? (dimness(u.ux, u.uy) > 0) : (uswallow_indark()))
		  || ((rn2(3) < armpro) && rn2(50))
		){
			if(obj->oeroded && obj->oerodeproof && 
				(!u.uswallow ? (dimness(u.ux, u.uy) > 0) : (uswallow_indark())))
				obj->oeroded--;
			start_timer(1, TIMER_OBJECT, LIGHT_DAMAGE, (genericptr_t)obj);
			return;
		}
		if(obj->oeroded < 2){
			obj->oeroded++;
			Your("%s degrade%s.",xname(obj),(obj->quan > 1L ? "" : "s"));
			start_timer(1, TIMER_OBJECT, LIGHT_DAMAGE, (genericptr_t)obj);
			stop_occupation();
			if(flags.run) nomul(0, NULL);
			return;
		}
	    if (flags.verbose && !isarmor) {
			char *name = obj->otyp == CORPSE ? corpse_xname(obj, FALSE) : xname(obj);
			Your("%s%s%s %s away%c",
				 obj == uwep ? "wielded " : nul, name, obj->otyp == NOBLE_S_DRESS ? "'s armored plates" : "",
				 obj->otyp == NOBLE_S_DRESS ? "evaporate" : otense(obj, "evaporate"), obj == uwep ? '!' : '.');
	    }
	    if (obj == uwep) {
			uwepgone();	/* now bare handed */
			stop_occupation();
			if(flags.run) nomul(0, NULL);
			if(obj->otyp == NOBLE_S_DRESS){
				obj = poly_obj(obj, PLAIN_DRESS);
				obj->oeroded = 0;
			} else {
				useupall(obj);
			}
	    } else if (obj == uswapwep) {
			uswapwepgone();
			stop_occupation();
			if(flags.run) nomul(0, NULL);
			if(obj->otyp == NOBLE_S_DRESS){
				obj = poly_obj(obj, PLAIN_DRESS);
				obj->oeroded = 0;
			} else {
				useupall(obj);
			}
	    } else if (obj == uquiver) {
			uqwepgone();
			stop_occupation();
			if(flags.run) nomul(0, NULL);
			if(obj->otyp == NOBLE_S_DRESS){
				obj = poly_obj(obj, PLAIN_DRESS);
				obj->oeroded = 0;
			} else {
				useupall(obj);
			}
	    } else if (isarmor) {
			stop_occupation();
			if(flags.run) nomul(0, NULL);
			if(obj->otyp == NOBLE_S_DRESS){
				pline("The armored plates on your dress turn to dust and blow away.");
				remove_worn_item(obj, TRUE);
				obj = poly_obj(obj, PLAIN_DRESS);
				obj->oeroded = 0;
				if(!uarmu){
					setworn(obj, W_ARMU);
					Shirt_on();
				} else {
					setworn(obj, W_ARM);
					Armor_on();
				}
			} else destroy_arm(obj);
	    } else{
			stop_occupation();
			if(flags.run) nomul(0, NULL);
			if(obj->otyp == NOBLE_S_DRESS){
				obj = poly_obj(obj, PLAIN_DRESS);
				obj->oeroded = 0;
			} else {
				useupall(obj);
			}
		}
	} else if (obj->where == OBJ_MINVENT && obj->owornmask) {
		struct obj *armor = which_armor(obj->ocarry, W_ARMC);
		int armpro = 0;
		long unwornmask;
		struct monst *mtmp;
		x = obj->ocarry->mx;
		y = obj->ocarry->my;
		if(armor){
			armpro = armor->otyp == DROVEN_CLOAK ? 
				objects[armor->otyp].a_can - armor->oeroded3 :
				objects[armor->otyp].a_can;
		}
		if (obj->ocarry->perminvis || obj->ocarry->minvis){
			start_timer(1, TIMER_OBJECT, LIGHT_DAMAGE, (genericptr_t)obj);
			return;
		}
		if ((dimness(x, y) > 0) || ((rn2(3) < armpro) && rn2(50))){
			if(obj->oeroded && obj->oerodeproof 
				&& (dimness(x, y) > 0)
			) obj->oeroded--;
			start_timer(1, TIMER_OBJECT,
						LIGHT_DAMAGE, (genericptr_t)obj);
			return;
		}
		if(obj->oeroded < 2){
			obj->oeroded++;
			start_timer(1, TIMER_OBJECT,
						LIGHT_DAMAGE, (genericptr_t)obj);
			return;
		}
	    if (obj == MON_WEP(obj->ocarry)) {
			mtmp = obj->ocarry;
			setmnotwielded(mtmp,obj);
			MON_NOWEP(mtmp);
			if(obj->otyp == NOBLE_S_DRESS){
				obj = poly_obj(obj, PLAIN_DRESS);
				obj->oeroded = 0;
				place_object(obj, mtmp->mx, mtmp->my);
				/* call stackobj() if we ever drop anything that can merge */
				newsym(mtmp->mx, mtmp->my);
			} else {
				m_useup(mtmp, obj);
			}
		}
	    else if (obj == MON_SWEP(obj->ocarry)) {
			mtmp = obj->ocarry;
			setmnotwielded(mtmp,obj);
			MON_NOSWEP(mtmp);
			if(obj->otyp == NOBLE_S_DRESS){
				obj = poly_obj(obj, PLAIN_DRESS);
				obj->oeroded = 0;
				place_object(obj, mtmp->mx, mtmp->my);
				/* call stackobj() if we ever drop anything that can merge */
				newsym(mtmp->mx, mtmp->my);
			} else {
				m_useup(mtmp, obj);
			}
		}
		else if((unwornmask = obj->owornmask) != 0L){
			mtmp = obj->ocarry;
			obj_extract_self(obj);
			mtmp->misc_worn_check &= ~unwornmask;
			obj->owornmask = 0L;
			update_mon_intrinsics(mtmp, obj, FALSE, FALSE);
			if(obj->otyp == NOBLE_S_DRESS){
				obj = poly_obj(obj, PLAIN_DRESS);
				obj->oeroded = 0;
				place_object(obj, mtmp->mx, mtmp->my);
				/* call stackobj() if we ever drop anything that can merge */
				newsym(mtmp->mx, mtmp->my);
			} else {
				m_useup(mtmp, obj);
			}
		}
		else{
			mtmp = obj->ocarry;
			obj_extract_self(obj);
			if(obj->otyp == NOBLE_S_DRESS){
				obj = poly_obj(obj, PLAIN_DRESS);
				obj->oeroded = 0;
				place_object(obj, mtmp->mx, mtmp->my);
				/* call stackobj() if we ever drop anything that can merge */
				newsym(mtmp->mx, mtmp->my);
			} else {
				m_useup(obj->ocarry, obj);
			}
		}
	}
	if (on_floor) newsym(x, y);
	else if (in_invent) update_inventory();
}

STATIC_OVL int
def_beastmastery()
{
	int bm;
	switch (P_SKILL(P_BEAST_MASTERY)) {
		case P_ISRESTRICTED: bm =  0; break;
		case P_UNSKILLED:    bm =  0; break;
		case P_BASIC:        bm =  2; break;
		case P_SKILLED:      bm =  5; break;
		case P_EXPERT:       bm = 10; break;
		default: impossible(">Expert beast mastery unhandled"); bm = 10; break;
	}
	if((uwep && uwep->oartifact == ART_CLARENT) || (uswapwep && uswapwep->oartifact == ART_CLARENT))
		bm *= 2;
	return bm;
}

int
heal_vilya()
{
	return (ACURR(A_INT) - 11)/2;
}

STATIC_OVL int
def_vilya()
{
	return (ACURR(A_INT) - 11)/2;
}

STATIC_OVL int
def_lomya()
{
	return (ACURR(A_WIS) - 11)/2;
}

int
lev_lomya()
{
	int bm = 0;
	switch (P_SKILL(P_BEAST_MASTERY)) {
		case P_ISRESTRICTED: bm +=  0; break;
		case P_UNSKILLED:    bm +=  0; break;
		case P_BASIC:        bm +=  1; break;
		case P_SKILLED:      bm +=  2; break;
		case P_EXPERT:       bm +=  5; break;
		default: impossible(">Expert beast mastery unhandled"); bm += 5; break;
	}
	if((uwep && uwep->oartifact == ART_CLARENT) || (uswapwep && uswapwep->oartifact == ART_CLARENT))
		bm *= 2;
	return bm + (ACURR(A_WIS) + ACURR(A_CHA) - 18)/4;
}

int
en_nenya()
{
	return (ACURR(A_WIS) - 11)/2;
}

STATIC_OVL int
def_narya()
{
	return (ACURR(A_CHA) - 11)/2;
}

int
heal_mlevel_bonus()
{
	int bm = 0;
	switch (P_SKILL(P_BEAST_MASTERY)) {
		case P_ISRESTRICTED: bm +=  0; break;
		case P_UNSKILLED:    bm +=  0; break;
		case P_BASIC:        bm +=  1; break;
		case P_SKILLED:      bm +=  2; break;
		case P_EXPERT:       bm +=  5; break;
		default: impossible(">Expert beast mastery unhandled"); bm += 5; break;
	}
	if((uwep && uwep->oartifact == ART_CLARENT) || (uswapwep && uswapwep->oartifact == ART_CLARENT))
		bm *= 2;
	switch (P_SKILL(P_HEALING_SPELL)) {
		case P_ISRESTRICTED: bm +=  0; break;
		case P_UNSKILLED:    bm +=  0; break;
		case P_BASIC:        bm +=  1; break;
		case P_SKILLED:      bm +=  2; break;
		case P_EXPERT:       bm +=  5; break;
		default: impossible(">Expert healing unhandled"); bm += 5; break;
	}
	return bm;
}

STATIC_OVL int
def_mountedCombat()
{
	int bm;
	switch (P_SKILL(P_RIDING)) {
		case P_ISRESTRICTED: bm =  0; break;
		case P_UNSKILLED:    bm =  0; break;
		case P_BASIC:        bm =  2; break;
		case P_SKILLED:      bm =  5; break;
		case P_EXPERT:       bm = 10; break;
		default: impossible(">Expert riding unhandled"); bm = 10; break;
	}
	return bm;
}

/*worn.c*/
