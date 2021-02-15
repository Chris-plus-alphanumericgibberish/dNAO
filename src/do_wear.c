/*	SCCS Id: @(#)do_wear.c	3.4	2003/11/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <math.h>
#include "hack.h"
#include "xhity.h"
#include "zap.h"
#include "artifact.h"

#ifndef OVLB

STATIC_DCL long takeoff_mask, taking_off;

#else /* OVLB */

STATIC_OVL NEARDATA long takeoff_mask = 0L;
static NEARDATA long taking_off = 0L;

static NEARDATA int todelay;
static boolean cancelled_don = FALSE;

static NEARDATA const char see_yourself[] = "see yourself";
static NEARDATA const char unknown_type[] = "Unknown type of %s (%d)";
static NEARDATA const char c_armor[]  = "armor",
			   c_suit[]   = "suit",
#ifdef TOURIST
			   c_shirt[]  = "shirt",
#endif
			   c_cloak[]  = "cloak",
			   c_gloves[] = "gloves",
			   c_boots[]  = "boots",
			   c_helmet[] = "helmet",
			   c_shield[] = "shield",
			   c_weapon[] = "weapon",
			   c_sword[]  = "sword",
			   c_axe[]    = "axe",
			   c_that_[]  = "that";

static NEARDATA const long takeoff_order[] = { WORN_BLINDF, W_WEP,
	WORN_SHIELD, WORN_GLOVES, LEFT_RING, RIGHT_RING, WORN_CLOAK,
	WORN_HELMET, WORN_AMUL, WORN_ARMOR,
#ifdef TOURIST
	WORN_SHIRT,
#endif
	WORN_BOOTS, W_SWAPWEP, W_QUIVER, 0L };

STATIC_DCL void FDECL(on_msg, (struct obj *));
STATIC_DCL int NDECL(Cloak_on);
STATIC_PTR int NDECL(Gloves_on);
STATIC_PTR int NDECL(Shield_on);
STATIC_DCL void FDECL(Ring_off_or_gone, (struct obj *, BOOLEAN_P));
STATIC_PTR int FDECL(select_off, (struct obj *));
STATIC_DCL struct obj *NDECL(do_takeoff);
STATIC_PTR int NDECL(take_off);
STATIC_DCL int FDECL(menu_remarm, (int));
STATIC_DCL void FDECL(already_wearing, (const char*));
STATIC_DCL void FDECL(already_wearing2, (const char*, const char*));

void
off_msg(otmp)
register struct obj *otmp;
{
	if(flags.verbose)
	    You("were wearing %s.", doname(otmp));
}

/* for items that involve no delay */
STATIC_OVL void
on_msg(otmp)
register struct obj *otmp;
{
	if (flags.verbose) {
	    char how[BUFSZ];

	    how[0] = '\0';
	    if (otmp->otyp == TOWEL)
		Sprintf(how, " around your %s", body_part(HEAD));
	    You("are now wearing %s%s.",
		obj_is_pname(otmp) ? the(xname(otmp)) : an(xname(otmp)),
		how);
	}
}

/*
 * The Type_on() functions should be called *after* setworn().
 * The Type_off() functions call setworn() themselves.
 */


int
Boots_on()
{
    long oldprop;
    if (!uarmf) return 0;
 	if(uarmf->otyp == STILETTOS) {
		if(!Flying && !Levitation) pline("%s are stylish, but not very practical to fight in.", The(xname(uarmf)));
		else pline("%s are stylish, and, since you don't have to walk, quite practical to fight in.", The(xname(uarmf)));
		ABON(A_CHA) += 1 + uarmf->spe;
		flags.botl = 1;
	}
   oldprop = (u.uprops[objects[uarmf->otyp].oc_oprop].extrinsic & ~WORN_BOOTS);

    switch(uarmf->otyp) {
	case LOW_BOOTS:
	case SHOES:
	case ARMORED_BOOTS:
	case ARCHAIC_BOOTS:
	case HIGH_BOOTS:
	case HEELED_BOOTS:
	case PLASTEEL_BOOTS:
	case CRYSTAL_BOOTS:
	case JUMPING_BOOTS:
	case KICKING_BOOTS:
	case HARMONIUM_BOOTS:
	case STILETTOS:
		break;
	case WATER_WALKING_BOOTS:
		if (u.uinwater) spoteffects(TRUE);
		break;
	case SPEED_BOOTS:
		/* Speed boots are still better than intrinsic speed, */
		/* though not better than potion speed */
		if (!oldprop && !(HFast & TIMEOUT)) {
			makeknown(uarmf->otyp);
			You_feel("yourself speed up%s.",
				(oldprop || HFast) ? " a bit more" : "");
		}
		break;
	case ELVEN_BOOTS:
		if (!oldprop && !HStealth && !BStealth) {
			makeknown(uarmf->otyp);
			You("walk very quietly.");
		}
		break;
	case FUMBLE_BOOTS:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			incr_itimeout(&HFumbling, rnd(20));
		break;
	case FLYING_BOOTS:
		if (!oldprop && !(species_flies(youracedata) || (u.usteed && mon_resistance(u.usteed,FLYING))) && !HLevitation) {
			makeknown(uarmf->otyp);
			float_up();
			spoteffects(FALSE);
		}
		break;
	default: impossible(unknown_type, c_boots, uarmf->otyp);
    }
    return 0;
}

int
Boots_off()
{
    int otyp = uarmf->otyp;
    long oldprop = u.uprops[objects[otyp].oc_oprop].extrinsic & ~WORN_BOOTS;

 	if(uarmf->otyp == STILETTOS && !cancelled_don) {
		ABON(A_CHA) -= 1 + uarmf->spe;
		flags.botl = 1;
	}
	
    takeoff_mask &= ~W_ARMF;
	/* For levitation, float_down() returns if Levitation, so we
	 * must do a setworn() _before_ the levitation case.
	 */
    setworn((struct obj *)0, W_ARMF);
    switch (otyp) {
	case SPEED_BOOTS:
		if (!Very_fast && !cancelled_don) {
			makeknown(otyp);
			You_feel("yourself slow down%s.",
				Fast ? " a bit" : "");
		}
		break;
	case WATER_WALKING_BOOTS:
		if (is_pool(u.ux,u.uy, TRUE) && !Levitation && !Flying &&
		    !is_clinger(youracedata) && !cancelled_don) {
			makeknown(otyp);
			/* make boots known in case you survive the drowning */
			spoteffects(TRUE);
		}
		break;
	case ELVEN_BOOTS:
		if (!oldprop && !HStealth && !BStealth && !cancelled_don) {
			makeknown(otyp);
			You("sure are noisy.");
		}
		break;
	case FUMBLE_BOOTS:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			HFumbling = EFumbling = 0;
		break;
	case FLYING_BOOTS:
		if (!oldprop && !species_flies(youracedata) && !(u.usteed && mon_resistance(u.usteed,FLYING)) && !Levitation && !cancelled_don) {
			(void) float_down(0L, 0L);
			makeknown(otyp);
		}
		break;
	case LOW_BOOTS:
	case SHOES:
	case ARMORED_BOOTS:
	case ARCHAIC_BOOTS:
	case HIGH_BOOTS:
	case HEELED_BOOTS:
	case PLASTEEL_BOOTS:
	case CRYSTAL_BOOTS:
	case JUMPING_BOOTS:
	case KICKING_BOOTS:
	case HARMONIUM_BOOTS:
	case STILETTOS:
		break;
	default: impossible(unknown_type, c_boots, otyp);
    }
    cancelled_don = FALSE;
    return 0;
}

STATIC_OVL 
int
Cloak_on()
{
    long oldprop;
    if (!uarmc) return 0;
    oldprop =
	u.uprops[objects[uarmc->otyp].oc_oprop].extrinsic & ~WORN_CLOAK;

    switch(uarmc->otyp) {
	case ELVEN_CLOAK:
	case CLOAK_OF_PROTECTION:
	case CLOAK_OF_DISPLACEMENT:
		makeknown(uarmc->otyp);
		break;
	case ORCISH_CLOAK:
	case DROVEN_CLOAK:
	case DWARVISH_CLOAK:
	case CLOAK_OF_MAGIC_RESISTANCE:
	case ROBE:
	case CLOAK:
	case LEO_NEMAEUS_HIDE:
	case WHITE_FACELESS_ROBE:
	case BLACK_FACELESS_ROBE:
		break;
	case SMOKY_VIOLET_FACELESS_ROBE:
		adj_abon(uarmc, uarmc->spe);
		break;
	case MUMMY_WRAPPING:
		/* Note: it's already being worn, so we have to cheat here. */
		if ((HInvis || EInvis || pm_invisible(youracedata)) && !Blind) {
		    newsym(u.ux,u.uy);
		    You("can %s!",
			See_invisible(u.ux, u.uy) ? "no longer see through yourself"
			: see_yourself);
		}
		break;
	case CLOAK_OF_INVISIBILITY:
		/* since cloak of invisibility was worn, we know mummy wrapping
		   wasn't, so no need to check `oldprop' against blocked */
		if (!oldprop && !HInvis && !Blind) {
		    makeknown(uarmc->otyp);
		    newsym(u.ux,u.uy);
		    pline("Suddenly you can%s yourself.",
			See_invisible(u.ux, u.uy) ? " see through" : "not see");
		}
		break;
	case OILSKIN_CLOAK:
		pline("%s very tightly.", Tobjnam(uarmc, "fit"));
		break;
	/* Alchemy smock gives poison _and_ acid resistance */
	case ALCHEMY_SMOCK:
		EAcid_resistance |= WORN_CLOAK;
  		break;
	default: impossible(unknown_type, c_cloak, uarmc->otyp);
    }
    /* vampires get a charisma bonus when wearing an opera cloak */
	if (uarmc->otyp == find_opera_cloak() && is_vampire(youracedata)) {
		You("%s very impressive in your %s.", Blind ||
				(Invis && !See_invisible(u.ux, u.uy)) ? "feel" : "look",
				OBJ_DESCR(objects[uarmc->otyp]));
		ABON(A_CHA) += 1;
		flags.botl = 1;
    }
    /* racial armor bonus */
	if(arti_lighten(uarmc, FALSE)) inv_weight();
	
    return 0;
}

int
Cloak_off()
{
    int otyp = uarmc->otyp;
	boolean checkweight = FALSE;
    long oldprop = u.uprops[objects[otyp].oc_oprop].extrinsic & ~WORN_CLOAK;
	if(arti_lighten(uarmc, FALSE)) checkweight = TRUE;
	if(otyp == SMOKY_VIOLET_FACELESS_ROBE)
		adj_abon(uarmc, -uarmc->spe);
    takeoff_mask &= ~W_ARMC;
	/* For mummy wrapping, taking it off first resets `Invisible'. */
    setworn((struct obj *)0, W_ARMC);
	if(checkweight) inv_weight();
    switch (otyp) {
	case ELVEN_CLOAK:
	case ORCISH_CLOAK:
	case DROVEN_CLOAK:
	case DWARVISH_CLOAK:
	case CLOAK_OF_PROTECTION:
	case CLOAK_OF_MAGIC_RESISTANCE:
	case CLOAK_OF_DISPLACEMENT:
	case OILSKIN_CLOAK:
	case ROBE:
	case CLOAK:
	case LEO_NEMAEUS_HIDE:
	case WHITE_FACELESS_ROBE:
	case BLACK_FACELESS_ROBE:
	case SMOKY_VIOLET_FACELESS_ROBE:
		break;
	case MUMMY_WRAPPING:
		if (Invis && !Blind) {
		    newsym(u.ux,u.uy);
		    You("can %s.",
			See_invisible(u.ux, u.uy) ? "see through yourself"
			: "no longer see yourself");
		}
		break;
	case CLOAK_OF_INVISIBILITY:
		if (!oldprop && !HInvis && !Blind) {
		    makeknown(CLOAK_OF_INVISIBILITY);
		    newsym(u.ux,u.uy);
		    pline("Suddenly you can %s.",
			See_invisible(u.ux, u.uy) ? "no longer see through yourself"
			: see_yourself);
		}
		break;
	/* Alchemy smock gives poison _and_ acid resistance */
	case ALCHEMY_SMOCK:
		EAcid_resistance &= ~WORN_CLOAK;
  		break;
	default: impossible(unknown_type, c_cloak, otyp);
    }
    /* vampires get a charisma bonus when wearing an opera cloak */
	if (otyp == find_opera_cloak() &&
		!cancelled_don &&
		is_vampire(youracedata)) {
		ABON(A_CHA) -= 1;
		flags.botl = 1;
    }
    return 0;
}


int
Helmet_on()
{
	boolean already_blind, blind_changed = FALSE;	/* blindness from wearing the helmet was set before this was called, we need a kludge*/
	struct obj * otmp;
    if (!uarmh) return 0;
    switch(uarmh->otyp) {
	case FEDORA:
	case SHEMAGH:
	case HELMET:
	case DROVEN_HELM:
	case FLACK_HELMET:
	case PLASTEEL_HELM:
	case CRYSTAL_HELM:
	case PONTIFF_S_CROWN:
	case SEDGE_HAT:
	case WAR_HAT:
	case WIDE_HAT:
	case WITCH_HAT:
	case ELVEN_HELM:
	case ARCHAIC_HELM:
	case LEATHER_HELM:
	case HIGH_ELVEN_HELM:
	case DWARVISH_HELM:
	case GNOMISH_POINTY_HAT:
	case ORCISH_HELM:
	case HELM_OF_TELEPATHY:
	case HELM_OF_DRAIN_RESISTANCE:
	case HARMONIUM_HELM:
		if (uarmh->otyp == find_gcirclet()) adj_abon(uarmh, uarmh->spe);
		break;
	case HELM_OF_BRILLIANCE:
		adj_abon(uarmh, uarmh->spe);
		break;
	case CORNUTHAUM:
		/* people think marked wizards know what they're talking
		 * about, but it takes trained arrogance to pull it off,
		 * and the actual enchantment of the hat is irrelevant.
		 */
		ABON(A_CHA) += ((Role_if(PM_WIZARD) || Race_if(PM_INCANTIFIER)) ? 1 : -1);
		flags.botl = 1;
		makeknown(uarmh->otyp);
		break;
	case HELM_OF_OPPOSITE_ALIGNMENT:
		if (uarmh->otyp == find_gcirclet()) adj_abon(uarmh, uarmh->spe);
		if (u.ualign.type == A_NEUTRAL)
		    u.ualign.type = rn2(2) ? A_CHAOTIC : A_LAWFUL;
		else if(u.ualign.type == A_VOID){
		    u.ualign.type = !rn2(3) ? A_NEUTRAL : rn2(2) ? A_CHAOTIC : A_LAWFUL;
		} else u.ualign.type = -(u.ualign.type);
		u.ublessed = 0; /* lose your god's protection */
	     /* makeknown(uarmh->otyp);   -- moved below, after xname() */
		/*FALLTHRU*/
	case DUNCE_CAP:
		if (!uarmh->cursed) {
		    if (Blind)
			pline("%s for a moment.", Tobjnam(uarmh, "vibrate"));
		    else
			pline("%s %s for a moment.",
			      Tobjnam(uarmh, "glow"), hcolor(NH_BLACK));
		    if(!(uarmh->oartifact)) curse(uarmh);
		}
		flags.botl = 1;		/* reveal new alignment or INT & WIS */
		if (Hallucination) {
		    pline("My brain hurts!"); /* Monty Python's Flying Circus */
		} else if (uarmh->otyp == DUNCE_CAP) {
		    You_feel("%s.",	/* track INT change; ignore WIS */
		  ACURR(A_INT) <= (ABASE(A_INT) + ABON(A_INT) + ATEMP(A_INT)) ?
			     "like sitting in a corner" : "giddy");
		} else {
		    Your("mind oscillates briefly.");
		    makeknown(HELM_OF_OPPOSITE_ALIGNMENT);
		}
		break;
	default: impossible(unknown_type, c_helmet, uarmh->otyp);
    }

	/* putting on an opaque helmet may have effectively blindfolded you */
	/* kludge to check if you'd be blind without it on */
	otmp = uarmh; uarmh = (struct obj *)0;
	already_blind = Blind;
	uarmh = otmp;
	if (Blind && !already_blind) {
		blind_changed = TRUE;
		if (flags.verbose) You_cant("see any more.");
		/* set ball&chain variables before the hero goes blind */
		if (Punished) set_bc(0);
	}
	else if (already_blind && !Blind) {
		blind_changed = TRUE;
		You("can see!");
	}
	if (blind_changed) {
		/* blindness has just been toggled */
		if (Blind_telepat || Infravision)
			see_monsters();
		vision_full_recalc = 1;	/* recalc vision limits */
		flags.botl = 1;
	}

    return 0;
}

int
Helmet_off()
{
	boolean was_blind = Blind, blind_changed = FALSE;
	struct obj * otmp = uarmh;
	
    takeoff_mask &= ~W_ARMH;

    switch(uarmh->otyp) {
	case FEDORA:
	case SHEMAGH:
	case HELMET:
	case DROVEN_HELM:
	case FLACK_HELMET:
	case PLASTEEL_HELM:
	case CRYSTAL_HELM:
	case PONTIFF_S_CROWN:
	case SEDGE_HAT:
	case WAR_HAT:
	case WIDE_HAT:
	case WITCH_HAT:
	case ELVEN_HELM:
	case ARCHAIC_HELM:
	case LEATHER_HELM:
	case HIGH_ELVEN_HELM:
	case DWARVISH_HELM:
	case GNOMISH_POINTY_HAT:
	case ORCISH_HELM:
	case HARMONIUM_HELM:
		if (uarmh->otyp == find_gcirclet() && !cancelled_don) adj_abon(uarmh, -uarmh->spe);
	    break;
	case DUNCE_CAP:
	    flags.botl = 1;
	    break;
	case CORNUTHAUM:
	    if (!cancelled_don) {
			ABON(A_CHA) += ((Role_if(PM_WIZARD) || Race_if(PM_INCANTIFIER)) ? -1 : 1);
			flags.botl = 1;
	    }
	    break;
	case HELM_OF_TELEPATHY:
	    /* need to update ability before calling see_monsters() */
		if (uarmh->otyp == find_gcirclet() && !cancelled_don) adj_abon(uarmh, -uarmh->spe);
	    setworn((struct obj *)0, W_ARMH);
	    see_monsters();
	    return 0;
	case HELM_OF_BRILLIANCE:
	    if (!cancelled_don) adj_abon(uarmh, -uarmh->spe);
	    break;
	case HELM_OF_OPPOSITE_ALIGNMENT:
		if (uarmh->otyp == find_gcirclet() && !cancelled_don) adj_abon(uarmh, -uarmh->spe);
	    u.ualign.type = u.ualignbase[A_CURRENT];
	    u.ublessed = 0; /* lose the other god's protection */
	    flags.botl = 1;
	    break;
	case HELM_OF_DRAIN_RESISTANCE:
		if (uarmh->otyp == find_gcirclet() && !cancelled_don) adj_abon(uarmh, -uarmh->spe);
	    setworn((struct obj *)0, W_ARMH);
	    return 0;
	default: impossible(unknown_type, c_helmet, uarmh->otyp);
    }
    setworn((struct obj *)0, W_ARMH);
    cancelled_don = FALSE;

	if (Blind) {
		if (was_blind) {
			/* "still cannot see" needs the helmet to possibly have been the cause of your blindness */
			if ((otmp->otyp == CRYSTAL_HELM && is_opaque(otmp)) ||
				(otmp->otyp == PLASTEEL_HELM && is_opaque(otmp) && otmp->obj_material != objects[otmp->otyp].oc_material))
				You("still cannot see.");
		}
		else {
			blind_changed = TRUE;	/* !was_blind */
			You_cant("see anything now!");
			/* set ball&chain variables before the hero goes blind */
			if (Punished) set_bc(0);
		}
	}
	else if (was_blind) {
		blind_changed = TRUE;	/* !Blind */
		You("can see again.");
	}
	if (blind_changed) {
		/* blindness has just been toggled */
		if (Blind_telepat || Infravision) see_monsters();
		vision_full_recalc = 1;	/* recalc vision limits */
		flags.botl = 1;
	}
    return 0;
}

STATIC_PTR
int
Gloves_on()
{
    long oldprop;
    if (!uarmg) return 0;
    oldprop =
	u.uprops[objects[uarmg->otyp].oc_oprop].extrinsic & ~WORN_GLOVES;

    switch(uarmg->otyp) {
	case GLOVES:
	case LONG_GLOVES:
	case HIGH_ELVEN_GAUNTLETS:
	case GAUNTLETS:
	case HARMONIUM_GAUNTLETS:
	case ARCHAIC_GAUNTLETS:
	case CRYSTAL_GAUNTLETS:
	case PLASTEEL_GAUNTLETS:
	case ORIHALCYON_GAUNTLETS:
		break;
	case GAUNTLETS_OF_FUMBLING:
		if (!oldprop && !(HFumbling & ~TIMEOUT))
			incr_itimeout(&HFumbling, rnd(20));
		break;
	case GAUNTLETS_OF_POWER:
		makeknown(uarmg->otyp);
		flags.botl = 1; /* taken care of in attrib.c */
		break;
	case GAUNTLETS_OF_DEXTERITY:
		adj_abon(uarmg, uarmg->spe);
		break;
	default: impossible(unknown_type, c_gloves, uarmg->otyp);
    }
    return 0;
}

int
Gloves_off()
{
    long oldprop =
	u.uprops[objects[uarmg->otyp].oc_oprop].extrinsic & ~WORN_GLOVES;

    takeoff_mask &= ~W_ARMG;

    switch(uarmg->otyp) {
	case GLOVES:
	case LONG_GLOVES:
	case HIGH_ELVEN_GAUNTLETS:
	case GAUNTLETS:
	case HARMONIUM_GAUNTLETS:
	case ARCHAIC_GAUNTLETS:
	case CRYSTAL_GAUNTLETS:
	case PLASTEEL_GAUNTLETS:
	case ORIHALCYON_GAUNTLETS:
	    break;
	case GAUNTLETS_OF_FUMBLING:
	    if (!oldprop && !(HFumbling & ~TIMEOUT))
		HFumbling = EFumbling = 0;
	    break;
	case GAUNTLETS_OF_POWER:
	    makeknown(uarmg->otyp);
	    flags.botl = 1; /* taken care of in attrib.c */
	    break;
	case GAUNTLETS_OF_DEXTERITY:
	    if (!cancelled_don) adj_abon(uarmg, -uarmg->spe);
	    break;
	default: impossible(unknown_type, c_gloves, uarmg->otyp);
    }
    setworn((struct obj *)0, W_ARMG);
    cancelled_don = FALSE;
    (void) encumber_msg();		/* immediate feedback for GoP */

    /* Prevent wielding cockatrice when not wearing gloves */
    if (uwep && uwep->otyp == CORPSE &&
		touch_petrifies(&mons[uwep->corpsenm])) {
	char kbuf[BUFSZ];

	You("wield the %s in your bare %s.",
	    corpse_xname(uwep, TRUE), makeplural(body_part(HAND)));
	Strcpy(kbuf, an(corpse_xname(uwep, TRUE)));
	instapetrify(kbuf);
	uwepgone();  /* life-saved still doesn't allow touching cockatrice */
    }

    /* KMH -- ...or your secondary weapon when you're wielding it */
    if (u.twoweap && uswapwep && uswapwep->otyp == CORPSE &&
	touch_petrifies(&mons[uswapwep->corpsenm])) {
	char kbuf[BUFSZ];

	You("wield the %s in your bare %s.",
	    corpse_xname(uswapwep, TRUE), body_part(HAND));

	Strcpy(kbuf, an(corpse_xname(uswapwep, TRUE)));
	instapetrify(kbuf);
	uswapwepgone();	/* lifesaved still doesn't allow touching cockatrice */
    }

    return 0;
}

STATIC_OVL 
int
Shield_on()
{
/*
    switch (uarms->otyp) {
	case BUCKLER:
	case ELVEN_SHIELD:
	case URUK_HAI_SHIELD:
	case ORCISH_SHIELD:
	case DWARVISH_ROUNDSHIELD:
	case KITE_SHIELD:
	case SHIELD_OF_REFLECTION:
		break;
	default: impossible(unknown_type, c_shield, uarms->otyp);
    }
*/
    return 0;
}

int
Shield_off()
{
    takeoff_mask &= ~W_ARMS;
/*
    switch (uarms->otyp) {
	case BUCKLER:
	case ELVEN_SHIELD:
	case URUK_HAI_SHIELD:
	case ORCISH_SHIELD:
	case DWARVISH_ROUNDSHIELD:
	case KITE_SHIELD:
	case SHIELD_OF_REFLECTION:
		break;
	default: impossible(unknown_type, c_shield, uarms->otyp);
    }
*/
    setworn((struct obj *)0, W_ARMS);
    return 0;
}

int
Shirt_on()
{
/*
    switch (uarmu->otyp) {
	case HAWAIIAN_SHIRT:
	case T_SHIRT:
		break;
	default: impossible(unknown_type, c_shirt, uarmu->otyp);
    }
*/
	if(uarmu->otyp == RUFFLED_SHIRT) {
		You("%s very dashing in your %s.", Blind ||
				(Invis && !See_invisible(u.ux, u.uy)) ? "feel" : "look",
				OBJ_NAME(objects[uarmu->otyp]));
		ABON(A_CHA) += 1;
		flags.botl = 1;
	}
	else if(uarmu->otyp == VICTORIAN_UNDERWEAR){
		pline("The %s shapes your figure, but it isn't very practical to fight in.",
				OBJ_NAME(objects[uarmu->otyp]));
		ABON(A_CHA) += 2*(1 + uarmu->spe);
		flags.botl = 1;
	}
	else if(uarmu->otyp == BLACK_DRESS) {
		pline("%s complements your figure nicely.", The(xname(uarmu)));
		ABON(A_CHA) += 1 + uarmu->spe;
		flags.botl = 1;
	}
	if(arti_lighten(uarmu, FALSE)) inv_weight();
    return 0;
}

int
Shirt_off()
{
	boolean checkweight = FALSE;
    takeoff_mask &= ~W_ARMU;
	if(arti_lighten(uarmu, FALSE)) checkweight = TRUE;
/*
    switch (uarmu->otyp) {
	case HAWAIIAN_SHIRT:
	case T_SHIRT:
		break;
	default: impossible(unknown_type, c_shirt, uarmu->otyp);
    }
*/
	if(uarmu->otyp == RUFFLED_SHIRT) {
		ABON(A_CHA) -= 1;
		flags.botl = 1;
	}
	else if(uarmu->otyp == VICTORIAN_UNDERWEAR){
		ABON(A_CHA) -= 2*(1 + uarmu->spe);
		flags.botl = 1;
	}
	else if(uarmu->otyp == BLACK_DRESS) {
		ABON(A_CHA) -= (1 + uarmu->spe);
		flags.botl = 1;
	}
	
    setworn((struct obj *)0, W_ARMU);
	if(checkweight) inv_weight();
    return 0;
}

/* This must be done in worn.c, because one of the possible intrinsics conferred
 * is fire resistance, and we have to immediately set HFire_resistance in worn.c
 * since worn.c will check it before returning.
 */
int
Armor_on()
{
	if(uarm->otyp == NOBLE_S_DRESS || uarm->otyp == BLACK_DRESS) {
		pline("%s complements your figure nicely.", The(xname(uarm)));
		ABON(A_CHA) += 1 + uarm->spe;
		flags.botl = 1;
	}
	else if(uarm->otyp == CONSORT_S_SUIT){
		pline("%s shows off your figure, but is not very practical as armor.", The(xname(uarm)));
		ABON(A_CHA) += 2 + uarm->spe;
		flags.botl = 1;
	}
	else if(uarm->otyp == ELVEN_TOGA){
		You("%s very elegant in your %s.", Blind ||
				(Invis && !See_invisible(u.ux, u.uy)) ? "feel" : "look",
				OBJ_NAME(objects[uarm->otyp]));
		ABON(A_CHA) += 2;
		flags.botl = 1;
	}
	else if(uarm->otyp == GENTLEWOMAN_S_DRESS || uarm->otyp == GENTLEMAN_S_SUIT){
		You("%s very elegant in your %s.", Blind ||
				(Invis && !See_invisible(u.ux, u.uy)) ? "feel" : "look",
				OBJ_NAME(objects[uarm->otyp]));
		ABON(A_CHA) += 2*(1 + uarm->spe);
		flags.botl = 1;
	}
	if(arti_lighten(uarm, FALSE)) inv_weight();
    return 0;
}

int
Armor_off()
{
	boolean checkweight = FALSE;
    takeoff_mask &= ~W_ARM;
	if(arti_lighten(uarmu, FALSE)) checkweight = TRUE;

	if(!uarm) {
		impossible("Armor_off called with no uarm");
	}
	else if((uarm->otyp == NOBLE_S_DRESS || uarm->otyp == BLACK_DRESS) && !cancelled_don) {
		ABON(A_CHA) -= (1 + uarm->spe);
		flags.botl = 1;
	}
	else if(uarm->otyp == CONSORT_S_SUIT && !cancelled_don){
		ABON(A_CHA) -= (2 + uarm->spe);
		flags.botl = 1;
	}
	else if(uarm->otyp == ELVEN_TOGA && !cancelled_don){
		ABON(A_CHA) -= 2;
		flags.botl = 1;
	}
	else if((uarm->otyp == GENTLEWOMAN_S_DRESS || uarm->otyp == GENTLEMAN_S_SUIT) && !cancelled_don){
		ABON(A_CHA) -= 2*(1+uarm->spe);
		flags.botl = 1;
	}
    setworn((struct obj *)0, W_ARM);
	if(checkweight) inv_weight();
    cancelled_don = FALSE;
    return 0;
}

/* The gone functions differ from the off functions in that if you die from
 * taking it off and have life saving, you still die.
 */
int
Armor_gone()
{
    takeoff_mask &= ~W_ARM;
    setnotworn(uarm);
    cancelled_don = FALSE;
    return 0;
}

void
Amulet_on()
{
    if (!uamul) return;
    switch(uamul->otyp) {
	case AMULET_OF_ESP:
	case AMULET_OF_LIFE_SAVING:
	case AMULET_VERSUS_POISON:
	case AMULET_OF_DRAIN_RESISTANCE:
	case AMULET_OF_REFLECTION:
	case AMULET_OF_MAGICAL_BREATHING:
	case FAKE_AMULET_OF_YENDOR:
		break;
	case AMULET_OF_UNCHANGING:
		if (Slimed) {
		    Slimed = 0;
		    flags.botl = 1;
		}
		break;
	case AMULET_OF_CHANGE:
	    {
		int orig_sex = poly_gender();

		if (Unchanging) break;
		change_sex();
		/* Don't use same message as polymorph */
		if (orig_sex != poly_gender()) {
		    makeknown(AMULET_OF_CHANGE);
		    You("are suddenly very %s!", flags.female ? "feminine"
			: "masculine");
		    flags.botl = 1;
		} else
		    /* already polymorphed into single-gender monster; only
		       changed the character's base sex */
		    You("don't feel like yourself.");
		pline_The("amulet disintegrates!");
		if (orig_sex == poly_gender() && uamul->dknown &&
			!objects[AMULET_OF_CHANGE].oc_name_known &&
			!objects[AMULET_OF_CHANGE].oc_uname)
		    docall(uamul);
		useup(uamul);
		break;
	    }
	case AMULET_OF_STRANGULATION:
		makeknown(AMULET_OF_STRANGULATION);
		pline("It constricts your throat!");
		break;
	case AMULET_OF_RESTFUL_SLEEP:
		HSleeping = rnd(100);
		break;
	case AMULET_OF_YENDOR:
		break;
    }
}

void
Amulet_off()
{
    takeoff_mask &= ~W_AMUL;

    switch(uamul->otyp) {
	case AMULET_OF_ESP:
		/* need to update ability before calling see_monsters() */
		setworn((struct obj *)0, W_AMUL);
		see_monsters();
		return;
	case AMULET_OF_LIFE_SAVING:
	case AMULET_VERSUS_POISON:
	case AMULET_OF_DRAIN_RESISTANCE:
	case AMULET_OF_REFLECTION:
	case AMULET_OF_CHANGE:
	case AMULET_OF_UNCHANGING:
	case FAKE_AMULET_OF_YENDOR:
		break;
	case AMULET_OF_MAGICAL_BREATHING:
		if (Underwater) {
		    /* HMagical_breathing must be set off
			before calling drown() */
		    setworn((struct obj *)0, W_AMUL);
		    if (!breathless(youracedata) && !amphibious(youracedata)
						&& !Swimming) {
			You("suddenly inhale an unhealthy amount of water!");
		    	(void) drown();
		    }
		    return;
		}
		break;
	case AMULET_OF_STRANGULATION:
		if (Strangled) {
			You("can breathe more easily!");
		}
		break;
	case AMULET_OF_RESTFUL_SLEEP:
		setworn((struct obj *)0, W_AMUL);
		if (!ESleeping)
			HSleeping = 0;
		return;
	case AMULET_OF_YENDOR:
		break;
    }
    setworn((struct obj *)0, W_AMUL);
    return;
}

void
Ring_on(obj)
register struct obj *obj;
{
    long oldprop = u.uprops[objects[obj->otyp].oc_oprop].extrinsic;
    int old_attrib, which;

    if (obj == uwep) setuwep((struct obj *) 0);
    if (obj == uswapwep) setuswapwep((struct obj *) 0);
    if (obj == uquiver) setuqwep((struct obj *) 0);

    /* only mask out W_RING when we don't have both
       left and right rings of the same type */
    if ((oldprop & W_RING) != W_RING) oldprop &= ~W_RING;

    switch(obj->otyp){
	case RIN_TELEPORTATION:
	case RIN_REGENERATION:
	case RIN_SEARCHING:
	case RIN_STEALTH:
	case RIN_HUNGER:
	case RIN_AGGRAVATE_MONSTER:
	case RIN_POISON_RESISTANCE:
	case RIN_CONFLICT:
	case RIN_TELEPORT_CONTROL:
	case RIN_POLYMORPH:
	case RIN_POLYMORPH_CONTROL:
	case RIN_FREE_ACTION:                
	case RIN_SLOW_DIGESTION:
	case RIN_SUSTAIN_ABILITY:
	case MEAT_RING:
		break;
	case RIN_WARNING:
		see_monsters();
		break;
	case RIN_ALACRITY:
		if (!oldprop && !(HFast & TIMEOUT)) {
			makeknown(obj->otyp);
			You_feel("yourself speed up%s.",
				(oldprop || HFast) ? " a bit more" : "");
		}
	break;
	case RIN_FIRE_RESISTANCE:
		if(!oldprop && !HFire_resistance) pline("The ring feels cool!");
	break;
	case RIN_COLD_RESISTANCE:
		if(!oldprop && !HCold_resistance) pline("The ring feels warm!");
	break;
	case RIN_SHOCK_RESISTANCE:
		if(!oldprop && !HShock_resistance) Your("ringfinger feels numb!");
	break;
	case RIN_SEE_INVISIBLE:
		/* can now see invisible monsters */
		set_mimic_blocking(); /* do special mimic handling */
		see_monsters();
#ifdef INVISIBLE_OBJECTS
		see_objects();
#endif

		if (Invis && !oldprop && !HSee_invisible &&
				!species_perceives(youracedata) && !Blind) {
		    newsym(u.ux,u.uy);
		    pline("Suddenly you are transparent, but there!");
		    makeknown(RIN_SEE_INVISIBLE);
		}
		break;
	case RIN_INVISIBILITY:
		if (!oldprop && !HInvis && !BInvis && !Blind) {
		    makeknown(RIN_INVISIBILITY);
		    newsym(u.ux,u.uy);
		    self_invis_message();
		}
		break;
	case RIN_LEVITATION:
		if (!oldprop && !HLevitation) {
		    float_up();
		    makeknown(RIN_LEVITATION);
		    spoteffects(FALSE);	/* for sinks */
		}
		break;
	case RIN_GAIN_STRENGTH:
		which = A_STR;
		goto adjust_attrib;
	case RIN_GAIN_CONSTITUTION:
		which = A_CON;
		goto adjust_attrib;
	case RIN_ADORNMENT:
		which = A_CHA;
 adjust_attrib:
		old_attrib = ACURR(which);
		ABON(which) += obj->spe;
		if (ACURR(which) != old_attrib ||
			(objects[obj->otyp].oc_name_known &&
			    old_attrib != 25 && old_attrib != 3)) {
		    flags.botl = 1;
		    makeknown(obj->otyp);
		    obj->known = 1;
		    update_inventory();
		}
		break;
	case RIN_INCREASE_ACCURACY:	/* KMH */
		u.uhitinc += obj->spe;
		break;
	case RIN_INCREASE_DAMAGE:
		u.udaminc += obj->spe;
		break;
	case RIN_PROTECTION_FROM_SHAPE_CHAN:
		rescham();
		break;
	case RIN_PROTECTION:
		if (obj->spe || objects[RIN_PROTECTION].oc_name_known) {
		    flags.botl = 1;
		    makeknown(RIN_PROTECTION);
		    obj->known = 1;
		    update_inventory();
		}
		break;
    }
}

STATIC_OVL void
Ring_off_or_gone(obj,gone)
register struct obj *obj;
boolean gone;
{
    long mask = (obj->owornmask & W_RING);
    int old_attrib, which;

    takeoff_mask &= ~mask;
    if(objects[obj->otyp].oc_oprop && !(u.uprops[objects[obj->otyp].oc_oprop].extrinsic & mask))
	impossible("Strange... I didn't know you had that ring.");
    if(gone) setnotworn(obj);
    else setworn((struct obj *)0, obj->owornmask);

    switch(obj->otyp) {
	case RIN_ALACRITY:
		if (!Very_fast && !cancelled_don) {
			makeknown(obj->otyp);
			You_feel("yourself slow down%s.",
				Fast ? " a bit" : "");
		}
	break;
	case RIN_TELEPORTATION:
	case RIN_REGENERATION:
	case RIN_SEARCHING:
	case RIN_STEALTH:
	case RIN_HUNGER:
	case RIN_AGGRAVATE_MONSTER:
	case RIN_POISON_RESISTANCE:
	case RIN_FIRE_RESISTANCE:
	case RIN_COLD_RESISTANCE:
	case RIN_SHOCK_RESISTANCE:
	case RIN_CONFLICT:
	case RIN_TELEPORT_CONTROL:
	case RIN_POLYMORPH:
	case RIN_POLYMORPH_CONTROL:
	case RIN_FREE_ACTION:                
	case RIN_SLOW_DIGESTION:
	case RIN_SUSTAIN_ABILITY:
	case MEAT_RING:
		break;
	case RIN_WARNING:
		see_monsters();
		break;
	case RIN_SEE_INVISIBLE:
		/* Make invisible monsters go away */
		if (!See_invisible(u.ux, u.uy)) {
		    set_mimic_blocking(); /* do special mimic handling */
		    see_monsters();
#ifdef INVISIBLE_OBJECTS                
		    see_objects();
#endif
		}

		if (Invisible && !Blind) {
		    newsym(u.ux,u.uy);
		    pline("Suddenly you cannot see yourself.");
		    makeknown(RIN_SEE_INVISIBLE);
		}
		break;
	case RIN_INVISIBILITY:
		if (!Invis && !BInvis && !Blind) {
		    newsym(u.ux,u.uy);
		    Your("body seems to unfade%s.",
			 See_invisible(u.ux, u.uy) ? " completely" : "..");
		    makeknown(RIN_INVISIBILITY);
		}
		break;
	case RIN_LEVITATION:
		(void) float_down(0L, 0L);
		if (!Levitation) makeknown(RIN_LEVITATION);
		break;
	case RIN_GAIN_STRENGTH:
		which = A_STR;
		goto adjust_attrib;
	case RIN_GAIN_CONSTITUTION:
		which = A_CON;
		goto adjust_attrib;
	case RIN_ADORNMENT:
		which = A_CHA;
 adjust_attrib:
		old_attrib = ACURR(which);
		ABON(which) -= obj->spe;
		if (ACURR(which) != old_attrib) {
		    flags.botl = 1;
		    makeknown(obj->otyp);
		    obj->known = 1;
		    update_inventory();
		}
		break;
	case RIN_INCREASE_ACCURACY:	/* KMH */
		u.uhitinc -= obj->spe;
		break;
	case RIN_INCREASE_DAMAGE:
		u.udaminc -= obj->spe;
		break;
	case RIN_PROTECTION:
		/* might have forgotten it due to amnesia */
		if (obj->spe) {
		    flags.botl = 1;
		    makeknown(RIN_PROTECTION);
		    obj->known = 1;
		    update_inventory();
		}
	case RIN_PROTECTION_FROM_SHAPE_CHAN:
		/* If you're no longer protected, let the chameleons
		 * change shape again -dgk
		 */
		restartcham();
		break;
    }
}

void
Ring_off(obj)
struct obj *obj;
{
	Ring_off_or_gone(obj,FALSE);
}

void
Ring_gone(obj)
struct obj *obj;
{
	Ring_off_or_gone(obj,TRUE);
}

void
Blindf_on(otmp)
register struct obj *otmp;
{
	boolean already_blind = Blind, changed = FALSE;

	if (otmp == uwep)
	    setuwep((struct obj *) 0);
	if (otmp == uswapwep)
	    setuswapwep((struct obj *) 0);
	if (otmp == uquiver)
	    setuqwep((struct obj *) 0);
	setworn(otmp, W_TOOL);
	on_msg(otmp);

	if (Blind && !already_blind) {
	    changed = TRUE;
	    if (flags.verbose) You_cant("see any more.");
	    /* set ball&chain variables before the hero goes blind */
	    if (Punished) set_bc(0);
	} else if (already_blind && !Blind) {
	    changed = TRUE;
	    /* "You are now wearing the Eyes of the Overworld." */
	    You("can see!");
	}
	if (changed) {
	    /* blindness has just been toggled */
	    if (Blind_telepat || Infravision) see_monsters();
	    vision_full_recalc = 1;	/* recalc vision limits */
	    flags.botl = 1;
	}
}

void
Blindf_off(otmp)
register struct obj *otmp;
{
	boolean was_blind = Blind, changed = FALSE;

	takeoff_mask &= ~W_TOOL;
	setworn((struct obj *)0, otmp->owornmask);
	off_msg(otmp);

	if (Blind) {
	    if (was_blind) {
		/* "still cannot see" makes no sense when removing lenses
		   since they can't have been the cause of your blindness */
		if (otmp->otyp != LENSES && otmp->otyp != LIVING_MASK && otmp->otyp != MASK && otmp->otyp != R_LYEHIAN_FACEPLATE)
		    You("still cannot see.");
	    } else {
		changed = TRUE;	/* !was_blind */
		/* "You were wearing the Eyes of the Overworld." */
		You_cant("see anything now!");
		/* set ball&chain variables before the hero goes blind */
		if (Punished) set_bc(0);
	    }
	} else if (was_blind) {
	    changed = TRUE;	/* !Blind */
	    You("can see again.");
	}
	if (changed) {
	    /* blindness has just been toggled */
	    if (Blind_telepat || Infravision) see_monsters();
	    vision_full_recalc = 1;	/* recalc vision limits */
	    flags.botl = 1;
	}
}

/* called in main to set intrinsics of worn start-up items */
void
set_wear()
{
#ifdef TOURIST
	if (uarmu) (void) Shirt_on();
#endif
	if (uarm)  (void) Armor_on();
	if (uarmc) (void) Cloak_on();
	if (uarmf) (void) Boots_on();
	if (uarmg) (void) Gloves_on();
	if (uarmh) (void) Helmet_on();
	if (uarms) (void) Shield_on();
}

/* check whether the target object is currently being put on (or taken off) */
boolean
donning(otmp)		/* also checks for doffing */
register struct obj *otmp;
{
 /* long what = (occupation == take_off) ? taking_off : 0L; */
    long what = taking_off;	/* if nonzero, occupation is implied */
    boolean result = FALSE;

    if (otmp == uarm)
	result = (afternmv == Armor_on || afternmv == Armor_off ||
		  what == WORN_ARMOR);
#ifdef TOURIST
    else if (otmp == uarmu)
	result = (afternmv == Shirt_on || afternmv == Shirt_off ||
		  what == WORN_SHIRT);
#endif
    else if (otmp == uarmc)
	result = (afternmv == Cloak_on || afternmv == Cloak_off ||
		  what == WORN_CLOAK);
    else if (otmp == uarmf)
	result = (afternmv == Boots_on || afternmv == Boots_off ||
		  what == WORN_BOOTS);
    else if (otmp == uarmh)
	result = (afternmv == Helmet_on || afternmv == Helmet_off ||
		  what == WORN_HELMET);
    else if (otmp == uarmg)
	result = (afternmv == Gloves_on || afternmv == Gloves_off ||
		  what == WORN_GLOVES);
    else if (otmp == uarms)
	result = (afternmv == Shield_on || afternmv == Shield_off ||
		  what == WORN_SHIELD);

    return result;
}

void
cancel_don()
{
	/* the piece of armor we were donning/doffing has vanished, so stop
	 * wasting time on it (and don't dereference it when donning would
	 * otherwise finish)
	 */
	cancelled_don = (afternmv == Boots_on || afternmv == Helmet_on ||
			 afternmv == Gloves_on || afternmv == Armor_on);
	afternmv = 0;
	nomovemsg = (char *)0;
	multi = 0;
	todelay = 0;
	taking_off = 0L;
}

static NEARDATA const char clothes[] = {ARMOR_CLASS, 0};
static NEARDATA const char accessories[] = {RING_CLASS, AMULET_CLASS, TOOL_CLASS, FOOD_CLASS, 0};

/* the 'T' command */
int
dotakeoff()
{
	register struct obj *otmp = (struct obj *)0;
	int armorpieces = 0;

	/* nohands checks for shields, gloves, etc... */
	if (nohands(youracedata)) {
		pline("Don't even bother.");
		return(0);
	}
	
	if(!freehand()){
		You("have no free %s to undress yourself with!", body_part(HAND));
		return(0);
	}

#define MOREARM(x) if (x) { armorpieces++; otmp = x; }
	MOREARM(uarmh);
	MOREARM(uarms);
	MOREARM(uarmg);
	MOREARM(uarmf);
	if (uarmc) {
		armorpieces++;
		otmp = uarmc;
	} else if (uarm) {
		armorpieces++;
		otmp = uarm;
#ifdef TOURIST
	} else if (uarmu) {
		armorpieces++;
		otmp = uarmu;
#endif
	}
	if (!armorpieces) {
	     /* assert( GRAY_DRAGON_SCALES > YELLOW_DRAGON_SCALE_MAIL ); */
		if (uskin)
		    pline_The("%s merged with your skin!",
				uskin->otyp == LEO_NEMAEUS_HIDE ? 
				"lion skin is" : 
				uskin->otyp >= GRAY_DRAGON_SCALES ?
				"dragon scales are" : 
				"dragon scale mail is");
		else
		    pline("Not wearing any armor.%s", (iflags.cmdassist && 
				(uleft || uright || uamul || ublindf)) ?
			  "  Use 'R' command to remove accessories." : "");
		return 0;
	}
	if (armorpieces > 1
#ifdef PARANOID
	    || iflags.paranoid_remove
#endif
	    )
		otmp = getobj(clothes, "take off");
	if (otmp == 0) return(0);
	if (!(otmp->owornmask & W_ARMOR)) {
		You("are not wearing that.");
		return(0);
	}
	/* note: the `uskin' case shouldn't be able to happen here; dragons
	   can't wear any armor so will end up with `armorpieces == 0' above */
	if (otmp == uskin || ((otmp == uarm) && uarmc)
#ifdef TOURIST
			  || ((otmp == uarmu) && (uarmc || uarm))
#endif
		) {
	    You_cant("take that off.");
	    return 0;
	}

	reset_remarm();		/* clear takeoff_mask and taking_off */
	(void) select_off(otmp);
	if (!takeoff_mask) return 0;
	reset_remarm();		/* armoroff() doesn't use takeoff_mask */

	(void) armoroff(otmp);
	return(1);
}

/* the 'R' command */
int
doremring()
{
	register struct obj *otmp = 0;
	int Accessories = 0;

#define MOREACC(x) if (x) { Accessories++; otmp = x; }
	MOREACC(uleft);
	MOREACC(uright);
	MOREACC(uamul);
	MOREACC(ublindf);

	if(!Accessories) {
		pline("Not wearing any accessories.%s", (iflags.cmdassist &&
			    (uarm || uarmc ||
#ifdef TOURIST
			     uarmu ||
#endif
			     uarms || uarmh || uarmg || uarmf)) ?
		      "  Use 'T' command to take off armor." : "");
		return(0);
	}
	if (Accessories != 1
#ifdef PARANOID
	    || iflags.paranoid_remove
#endif
	    ) otmp = getobj(accessories, "remove");
	if(!otmp) return(0);
	if(!(otmp->owornmask & (W_RING | W_AMUL | W_TOOL))) {
		You("are not wearing that.");
		return(0);
	}

	reset_remarm();		/* clear takeoff_mask and taking_off */
	(void) select_off(otmp);
	if (!takeoff_mask) return 0;
	reset_remarm();		/* not used by Ring_/Amulet_/Blindf_off() */

	if (otmp == uright || otmp == uleft) {
		/* Sometimes we want to give the off_msg before removing and
		 * sometimes after; for instance, "you were wearing a moonstone
		 * ring (on right hand)" is desired but "you were wearing a
		 * square amulet (being worn)" is not because of the redundant
		 * "being worn".
		 */
		off_msg(otmp);
		Ring_off(otmp);
	} else if (otmp == uamul) {
		Amulet_off();
		off_msg(otmp);
	} else if (otmp == ublindf) {
		Blindf_off(otmp);	/* does its own off_msg */
	} else {
		impossible("removing strange accessory?");
	}
	return(1);
}

/* Check if something worn is cursed _and_ unremovable. */
int
cursed(otmp)
register struct obj *otmp;
{
	/* Curses, like chickens, come home to roost. */
	if((otmp == uwep) ? welded(otmp) : ((int)otmp->cursed && !Weldproof)) {
		You("can't.  %s cursed.",
			(is_boots(otmp) || is_gloves(otmp) || otmp->quan > 1L)
			? "They are" : "It is");
		otmp->bknown = TRUE;
		return(1);
	}
	return(0);
}

int
armoroff(otmp)
register struct obj *otmp;
{
	register int delay = -objects[otmp->otyp].oc_delay;

	if(cursed(otmp)) return(0);
	if(delay) {
		nomul(delay, "disrobing");
		if (otmp == uarms) {
			nomovemsg = "You finish taking off your shield.";
			afternmv = Shield_off;
		     }
		else if (otmp == uarmh) {
			nomovemsg = "You finish taking off your helmet.";
			afternmv = Helmet_off;
		     }
		else if (otmp == uarmf) {
			nomovemsg = "You finish taking off your boots.";
			afternmv = Boots_off;
		     }
		else if (otmp == uarmg) {
			nomovemsg = "You finish taking off your gloves.";
			afternmv = Gloves_off;
		     }
		else if (otmp == uarmc) {
			nomovemsg = "You finish taking off your cloak.";
			afternmv = Cloak_off;
		     }
		else if (otmp->otyp == VICTORIAN_UNDERWEAR) {
			nomovemsg = "You finish taking off your impractical underwear.";
			afternmv = Shirt_off;
		     }
		else if (otmp == uarmu) {
			nomovemsg = "You finish taking off your shirt.";
			afternmv = Shirt_off;
		     }
		else if (otmp == uarm){
			nomovemsg = "You finish taking off your suit.";
			afternmv = Armor_off;
		} else {
			impossible("Attempting to remove unknown armor type (with delay)");
		}
	} else {
		/* Be warned!  We want off_msg after removing the item to
		 * avoid "You were wearing ____ (being worn)."  However, an
		 * item which grants fire resistance might cause some trouble
		 * if removed in Hell and lifesaving puts it back on; in this
		 * case the message will be printed at the wrong time (after
		 * the messages saying you died and were lifesaved).  Luckily,
		 * no cloak, shield, or fast-removable armor grants fire
		 * resistance, so we can safely do the off_msg afterwards.
		 * Rings do grant fire resistance, but for rings we want the
		 * off_msg before removal anyway so there's no problem.  Take
		 * care in adding armors granting fire resistance; this code
		 * might need modification.
		 * 3.2 (actually 3.1 even): this comment is obsolete since
		 * fire resistance is not needed for Gehennom.
		 */
		if(is_shield(otmp))
			(void) Shield_off();
		else if(is_helmet(otmp))
			(void) Helmet_off();
		else if(is_boots(otmp))
			(void) Boots_off();
		else if(is_gloves(otmp))
			(void) Gloves_off();
		else if(is_cloak(otmp))
			(void) Cloak_off();
		else if(is_shirt(otmp))
			(void) Shirt_off();
		else if(is_suit(otmp))
			(void) Armor_off();
		else setworn((struct obj *)0, otmp->owornmask & W_ARMOR);
		off_msg(otmp);
	}
	takeoff_mask = taking_off = 0L;
	return(1);
}

STATIC_OVL void
already_wearing(cc)
const char *cc;
{
	You("are already wearing %s%c", cc, (cc == c_that_) ? '!' : '.');
}

STATIC_OVL void
already_wearing2(cc1, cc2)
const char *cc1, *cc2;
{
	You_cant("wear %s because you're wearing %s there already.", cc1, cc2);
}

/*
 * canwearobj checks to see whether the player can wear a piece of armor
 *
 * inputs: otmp (the piece of armor)
 *         noisy (if TRUE give error messages, otherwise be quiet about it)
 * output: mask (otmp's armor type)
 */
int
canwearobj(otmp,mask,noisy)
struct obj *otmp;
long *mask;
boolean noisy;
{
    int err = 0;

	if (otmp->owornmask & W_ARMOR) {
		if (noisy) already_wearing(c_that_);
		return 0;
    }

    if (welded(uwep) && bimanual(uwep,youracedata) &&
	    (is_suit(otmp)
			|| is_shirt(otmp)
	)) {
		if (noisy)
			You("cannot do that while holding your %s.",
			is_sword(uwep) ? c_sword : c_weapon);
		return 0;
    }
	
	if(is_whirly(youracedata) || noncorporeal(youracedata)){
		Your("body can't support clothing.");
		return 0;
	}

    if (is_helmet(otmp)) {
		if (uarmh) {
			if (noisy) already_wearing(an(c_helmet));
			err++;
		} else if (!is_flimsy(otmp)) {
			/* (flimsy exception matches polyself handling), you can even just set a hat on top of your body (no head requried)*/
			if(!has_head_mon(&youmonst)){
				if (noisy)
				You("don't have a head.");
				err++;
			} else if(youracedata->msize != otmp->objsize){
				if (noisy)
				pline_The("%s is the wrong size for you.", c_helmet);
				err++;
			} else if(!helm_match(youracedata,otmp)){
				if (noisy)
				pline_The("%s is the wrong shape for your head.", c_helmet);
				err++;
			} else if(has_horns(youracedata)){
				if (noisy)
				pline_The("%s won't fit over your horn%s.",
					  c_helmet, plur(num_horns(youracedata)));
				err++;
			} else
				*mask = W_ARMH;
		} else
			*mask = W_ARMH;
    } else if (is_shield(otmp)) {
		if (uarms) {
			if (noisy) already_wearing(an(c_shield));
			err++;
		} else if (uwep && bimanual(uwep,youracedata)) {
			if (noisy) 
			You("cannot wear a shield while wielding a two-handed %s.",
				is_sword(uwep) ? c_sword :
				(uwep->otyp == BATTLE_AXE) ? c_axe : c_weapon);
			err++;
		} else if (u.twoweap) {
			if (noisy)
			You("cannot wear a shield while wielding two weapons.");
			err++;
		} else
			*mask = W_ARMS;
    } else if (is_boots(otmp)) {
		if (uarmf) {
			if (noisy) already_wearing(c_boots);
			err++;
		} else if (noboots(youracedata)) {
			if (noisy) You("have no feet...");	/* not body_part(FOOT) */
			err++;
		} else if (!humanoid(youracedata) && !can_wear_boots(youracedata)) {
			if (noisy) pline("You have too many legs to wear %s.",  c_boots);
			err++;
		} else if(youracedata->msize != otmp->objsize){
			if (noisy)
			pline_The("%s are the wrong size for you.", c_boots);
			err++;
		} else if (u.utrap && (u.utraptype == TT_BEARTRAP ||
					u.utraptype == TT_INFLOOR)) {
			if (u.utraptype == TT_BEARTRAP) {
			if (noisy) Your("%s is trapped!", body_part(FOOT));
			} else {
			if (noisy) Your("%s are stuck in the %s!",
					makeplural(body_part(FOOT)),
					surface(u.ux, u.uy));
			}
			err++;
		} else
			*mask = W_ARMF;
    } else if (is_gloves(otmp)) {
		if (uarmg) {
			if (noisy) already_wearing(c_gloves);
			err++;
		} else if(nohands(youracedata)){
			/*Included for completeness, but having no hands actually prevents you from equiping anything*/
			if (noisy)
			You("don't have hands.");
			err++;
		} else if(youracedata->msize != otmp->objsize){
			if (noisy)
			pline_The("%s are the wrong size for you.", c_gloves);
			err++;
		} else if (welded(uwep)) {
			if (noisy) You("cannot wear gloves over your %s.",
				   is_sword(uwep) ? c_sword : c_weapon);
			err++;
		} else if (uwep && uwep->otyp == ARM_BLASTER && is_metal(otmp)) {
			if (noisy) pline("%s too tightly to allow such bulky, rigid gloves.",
				   Tobjnam(uwep, "fit"));
			err++;
		} else if (otmp->oartifact == ART_CLAWS_OF_THE_REVENANCER && uright) {
			if (noisy) You("cannot wear a ring on your right hand with this artifact.");
			err++;
		} else
			*mask = W_ARMG;
	} else if (is_shirt(otmp)) {
		if (uarm || uarmc || uarmu) {
			if (uarmu) {
			if (noisy) already_wearing(an(c_shirt));
			} else {
			if (noisy) You_cant("wear that over your %s.",
						   (uarm && !uarmc) ? c_armor : cloak_simple_name(uarmc));
			}
			err++;
		} else if(youracedata->msize != otmp->objsize){
			if (noisy)
			pline_The("%s is the wrong size for you.", c_shirt);
			err++;
		} else if(!shirt_match(youracedata,otmp)){
			if (noisy)
			pline_The("%s is the wrong shape for your body.", c_shirt);
			err++;
		} else
			*mask = W_ARMU;
	} else if (is_cloak(otmp)) {
		if (uarmc) {
			if (noisy) already_wearing(an(cloak_simple_name(uarmc)));
			err++;
		} else if(abs(otmp->objsize - youracedata->msize) > 1){
			if (noisy)
			pline_The("%s is the wrong size for you.", cloak_simple_name(uarmc));
			err++;
		} else
			*mask = W_ARMC;
    } else if (is_suit(otmp)) {
		if (uarmc) {
			if (noisy) You("cannot wear armor over a %s.", cloak_simple_name(uarmc));
			err++;
		} else if (uarm) {
			if (noisy) already_wearing("some armor");
			err++;
		} else if(!arm_size_fits(youracedata,otmp)){
			if (noisy)
			pline_The("%s is the wrong size for you.", c_armor);
			err++;
		} else if(!arm_match(youracedata,otmp)){
			if (noisy)
			pline_The("%s is the wrong shape for your body.", c_armor);
			err++;
		} else
			*mask = W_ARM;
    } else {
		/* getobj can't do this after setting its allow_all flag; that
		   happens if you have armor for slots that are covered up or
		   extra armor for slots that are filled */
		if (noisy) silly_thing("wear", otmp);
		err++;
    }
/* Unnecessary since now only weapons and special items like pick-axes get
 * welded to your hand, not armor
    if (welded(otmp)) {
	if (!err++) {
	    if (noisy) weldmsg(otmp);
	}
    }
 */
    return !err;
}

/* the 'W' command */
int
dowear()
{
	struct obj *otmp;
	int delay;
	long mask = 0;

	/* nohands checks for shields, gloves, etc... */
	if (nohands(youracedata)) {
		pline("Don't even bother.");
		return(0);
	}
	
	if(!freehand()){
		You("have no free %s to dress yourself with!", body_part(HAND));
		return(0);
	}

	otmp = getobj(clothes, "wear");
	if(!otmp) return(0);

	if (!canwearobj(otmp,&mask,TRUE)) return(0);

	if (otmp->oartifact && !touch_artifact(otmp, &youmonst, FALSE))
	    return 1;	/* costs a turn even though it didn't get worn */

	if (otmp->otyp == HELM_OF_OPPOSITE_ALIGNMENT &&
			qstart_level.dnum == u.uz.dnum) {	/* in quest */
		if (u.ualignbase[A_CURRENT] == u.ualignbase[A_ORIGINAL])
			You("narrowly avoid losing all chance at your goal.");
		else	/* converted */
			You("are suddenly overcome with shame and change your mind.");
		u.ublessed = 0; /* lose your god's protection */
		makeknown(otmp->otyp);
		flags.botl = 1;
		return 1;
	}

	if(otmp->ostolen && u.sealsActive&SEAL_ANDROMALIUS) unbind(SEAL_ANDROMALIUS, TRUE);
	otmp->known = TRUE;
	if(otmp == uwep)
		setuwep((struct obj *)0);
	if (otmp == uswapwep)
		setuswapwep((struct obj *) 0);
	if (otmp == uquiver)
		setuqwep((struct obj *) 0);
	setworn(otmp, mask);
	delay = -objects[otmp->otyp].oc_delay;
	if(delay){
		nomul(delay, "dressing up");
		if(is_shield(otmp)) afternmv = Shield_on;
		else if(is_helmet(otmp)) afternmv = Helmet_on;
		else if(is_boots(otmp)) afternmv = Boots_on;
		else if(is_gloves(otmp)) afternmv = Gloves_on;
		else if(is_cloak(otmp)) afternmv = Cloak_on;
		else if(is_shirt(otmp)) afternmv = Shirt_on;
		else if(otmp == uarm) afternmv = Armor_on;
		nomovemsg = "You finish your dressing maneuver.";
	} else {
		if(is_shield(otmp)) (void) Shield_on();
		else if(is_helmet(otmp)) (void) Helmet_on();
		else if(is_boots(otmp)) (void) Boots_on();
		else if(is_gloves(otmp)) (void) Gloves_on();
		else if(is_cloak(otmp)) (void) Cloak_on();
		else if(is_shirt(otmp)) (void) Shirt_on();
		else if(otmp == uarm) (void) Armor_on();
		on_msg(otmp);
	}
	takeoff_mask = taking_off = 0L;
	return(1);
}

int
doputon()
{
	register struct obj *otmp;
	long mask = 0L;

	if(uleft && (uright || (uarmg && uarmg->oartifact == ART_CLAWS_OF_THE_REVENANCER)) && uamul && ublindf) {
		Your("%s%s are full, and you're already wearing an amulet and %s.",
			humanoid(youracedata) ? "ring-" : "",
			makeplural(body_part(FINGER)),
			(ublindf->otyp==LENSES || ublindf->otyp==SUNGLASSES) ? "some lenses" : (ublindf->otyp==MASK || ublindf->otyp==LIVING_MASK || ublindf->otyp==R_LYEHIAN_FACEPLATE) ? "a mask" : "a blindfold");
		return(0);
	}
	otmp = getobj(accessories, "put on");
	if(!otmp) return(0);
	if(otmp->owornmask & (W_RING | W_AMUL | W_TOOL)) {
		already_wearing(c_that_);
		return(0);
	}
	if(welded(otmp)) {
		weldmsg(otmp);
		return(0);
	}
	if(otmp == uwep)
		setuwep((struct obj *)0);
	if(otmp == uswapwep)
		setuswapwep((struct obj *) 0);
	if(otmp == uquiver)
		setuqwep((struct obj *) 0);
	if(otmp->oclass == RING_CLASS || otmp->otyp == MEAT_RING) {
		if(nolimbs(youracedata)) {
			You("cannot make the ring stick to your body.");
			return(0);
		}
		if(uleft && (uright || (uarmg && uarmg->oartifact == ART_CLAWS_OF_THE_REVENANCER))){
			There("are no more %s%s to fill.",
				humanoid(youracedata) ? "ring-" : "",
				makeplural(body_part(FINGER)));
			return(0);
		}
		if(uleft) mask = RIGHT_RING;
		else if((uright || (uarmg && uarmg->oartifact == ART_CLAWS_OF_THE_REVENANCER))) mask = LEFT_RING;
		else do {
			char qbuf[QBUFSZ];
			char answer;

			Sprintf(qbuf, "Which %s%s, Right or Left?",
				humanoid(youracedata) ? "ring-" : "",
				body_part(FINGER));
			if(!(answer = yn_function(qbuf, "rl", '\0')))
				return(0);
			switch(answer){
			case 'l':
			case 'L':
				mask = LEFT_RING;
				break;
			case 'r':
			case 'R':
				mask = RIGHT_RING;
				break;
			}
		} while(!mask);
		if (uarmg && uarmg->cursed && !Weldproof) {
			uarmg->bknown = TRUE;
		    You("cannot remove your gloves to put on the ring.");
			return(0);
		}
		if (welded(uwep) && bimanual(uwep,youracedata)) {
			/* welded will set bknown */
	    You("cannot free your weapon hands to put on the ring.");
			return(0);
		}
		if (welded(uwep) && mask==RIGHT_RING) {
			/* welded will set bknown */
	    You("cannot free your weapon hand to put on the ring.");
			return(0);
		}
		if (otmp->oartifact && !touch_artifact(otmp, &youmonst, FALSE))
		    return 1; /* costs a turn even though it didn't get worn */
		setworn(otmp, mask);
		Ring_on(otmp);
	} else if (otmp->oclass == AMULET_CLASS) {
		if(uamul) {
			already_wearing("an amulet");
			return(0);
		}
		if (otmp->oartifact && !touch_artifact(otmp, &youmonst, FALSE))
		    return 1;
		setworn(otmp, W_AMUL);
		if (otmp->otyp == AMULET_OF_CHANGE) {
			Amulet_on();
			/* Don't do a prinv() since the amulet is now gone */
			return(1);
		}
		Amulet_on();
	} else {	/* it's a blindfold, towel, or lenses */
		if (ublindf) {
			if (ublindf->otyp == TOWEL)
				Your("%s is already covered by a towel.",
					body_part(FACE));
			else if (ublindf->otyp == BLINDFOLD || ublindf->otyp == LIVING_MASK || ublindf->otyp == ANDROID_VISOR) {
				if (otmp->otyp == LENSES || otmp->otyp == SUNGLASSES)
					already_wearing2("lenses", "a blindfold");
				else
					already_wearing("a blindfold");
			} else if (ublindf->otyp == LENSES || ublindf->otyp == SUNGLASSES) {
				if (otmp->otyp == BLINDFOLD || otmp->otyp == LIVING_MASK || otmp->otyp == ANDROID_VISOR)
					already_wearing2("a blindfold", "some lenses");
				else
					already_wearing("some lenses");
			} else
				already_wearing(something);
			return(0);
		}
		if (otmp->otyp != MASK && otmp->otyp != R_LYEHIAN_FACEPLATE && 
			otmp->otyp != BLINDFOLD && otmp->otyp != ANDROID_VISOR && 
			otmp->otyp != TOWEL && otmp->otyp != LENSES && 
			otmp->otyp != SUNGLASSES && otmp->otyp != LIVING_MASK
		) {
			You_cant("wear that!");
			return(0);
		}
		if (uarmh && (uarmh->otyp == PLASTEEL_HELM || uarmh->otyp == CRYSTAL_HELM || uarmh->otyp == PONTIFF_S_CROWN) && uarmh->cursed && !Weldproof){
			pline("The %s covers your whole face. You need to remove it first.", xname(uarmh));
			display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return 0;
		}
		if (otmp->oartifact && !touch_artifact(otmp, &youmonst, FALSE))
		    return 1;
		if((otmp->otyp == LENSES || otmp->otyp == SUNGLASSES) && otmp->objsize != youracedata->msize){
			pline("They don't fit!");
			return 0;
		}
		Blindf_on(otmp);
		return(1);
	}
	if (is_worn(otmp))
	    prinv((char *)0, otmp, 0L);
	return(1);
}

#endif /* OVLB */

int arm_total_bonus(otmp)
struct obj * otmp;
{
	return arm_ac_bonus(otmp) + arm_dr_bonus(otmp);
}

int
greatest_erosion(otmp)
struct obj * otmp;
{
	int greatest = 0;
	if(otmp->oeroded > greatest) greatest = (int) otmp->oeroded;
	if(otmp->oeroded2 > greatest) greatest = (int) otmp->oeroded2;
	if(otmp->oeroded3 > greatest) greatest = (int) otmp->oeroded3;
	
	return greatest;
}

/* 
 * Material defense bonuses are multiplicative to the armor's base
 * protection. Slots other than body armor see little or no change
 * in their effectiveness, and even body armor tends to only change
 * by +- 1 AC and DR. Partly, this is due to most commonly-subsititable
 * materials being very close in their defense ratings; see decl.c
 */
int
material_def_bonus(otmp, def)
struct obj * otmp;
int def;
{
	int curr = materials[otmp->obj_material].defense;
	int base = materials[objects[otmp->otyp].oc_material].defense;
	return ((def * curr) + base / 2) / base - def;
}

int arm_ac_bonus(otmp)
struct obj * otmp;
{
	/* no armor, no defense! */
	if (!otmp)
		return 0;

	int def = objects[otmp->otyp].a_ac;
	
	// add material bonus
	def += material_def_bonus(otmp, def);

	// reduce by erosion
	def -= min((int)greatest_erosion(otmp), def);

	// cloak of protection's magic is not reduced by erosion or multiplied by mat
	if (otmp->otyp == CLOAK_OF_PROTECTION)
		def += 2;
	// combat boots
	if (otmp->otyp == find_cboots()) def += 1;

	// add enchantment
	if (otmp->spe)
	{
		int spemult = 1; // out of 2
		// shields get full enchantment to AC
		if (is_shield(otmp))
			spemult += 1;
		// crystal armor bonus enchantment
		if (otmp->otyp == CRYSTAL_PLATE_MAIL ||
			otmp->otyp == CRYSTAL_HELM ||
			otmp->otyp == CRYSTAL_BOOTS ||
			otmp->otyp == CRYSTAL_SHIELD ||
			otmp->otyp == CRYSTAL_GAUNTLETS)
			spemult *= 2;

		def += (otmp->spe * spemult + 0) / 2;
	}

	// artifact bonus def
	switch (otmp->oartifact)
	{
	case ART_STEEL_SCALES_OF_KURTULMAK:
		def += objects[otmp->otyp].a_ac;
		def += (otmp->spe+1)/2;
		break;
	case ART_MANTLE_OF_HEAVEN:
	case ART_VESTMENT_OF_HELL:
	case ART_WEB_OF_THE_CHOSEN:
	case ART_CLOAK_OF_THE_CONSORT:
		def *= 2;
		break;
	case ART_ARMOR_OF_EREBOR:
		def += 5;
		break;
	case ART_ARMOR_OF_KHAZAD_DUM:
		def += 4;
		break;
	case ART_CLAWS_OF_THE_REVENANCER:
		def += 5;
		break;
	case ART_AEGIS:
		def += 3;//3 total, Same as Cloak of protection
		break;
	case ART_SPIDERSILK:
		def += 2;//offsetting the cloth material
		break;
	}

	return def;
}

int
arm_dr_bonus(otmp)
struct obj * otmp;
{
	/* no armor, no defense! */
	if (!otmp)
		return 0;

	int def = objects[otmp->otyp].a_dr;
	
	if(is_shield(otmp))
		return 0;

	// add material bonus
	def += material_def_bonus(otmp, def);

	// reduce by erosion
	def -= min((int)greatest_erosion(otmp), def);

	// cloak of protection's magic is not reduced by erosion or multiplied by mat
	if (otmp->otyp == CLOAK_OF_PROTECTION)
		def += 2;
	// padded gloves
	if (otmp->otyp == find_pgloves()) def += 1;
	// gold circlet
	if (otmp->otyp == find_gcirclet()) def -= 1;
	// visored helm
	if (otmp->otyp == find_vhelm()) def += 1;
	// combat boots
	if (otmp->otyp == find_cboots()) def += 1;


	// add enchantment
	if (otmp->spe)
	{
		int spemult = 1; // out of 2
		// shields get no enchantment to DR
		if (is_shield(otmp))
			spemult = 0;
		// crystal armor bonus enchantment
		if (otmp->otyp == CRYSTAL_PLATE_MAIL ||
			otmp->otyp == CRYSTAL_HELM ||
			otmp->otyp == CRYSTAL_BOOTS ||
			otmp->otyp == CRYSTAL_GAUNTLETS)
			spemult *= 2;

		def += sgn(otmp->spe)*(abs(otmp->spe) * spemult + 1) / 2;
	}

	// artifact bonus def
	switch (otmp->oartifact)
	{
	case ART_STEEL_SCALES_OF_KURTULMAK:
		def += objects[otmp->otyp].a_dr;
		def += (otmp->spe)/2;
		break;
	case ART_MANTLE_OF_HEAVEN:
	case ART_VESTMENT_OF_HELL:
	case ART_WEB_OF_THE_CHOSEN:
	case ART_CLOAK_OF_THE_CONSORT:
		def *= 2;
		break;
	case ART_ARMOR_OF_EREBOR:
		def += 5;
		break;
	case ART_ARMOR_OF_KHAZAD_DUM:
		def += 4;
		break;
	case ART_AEGIS:
		def += 1;//3 total, Same as Cloak of protection
		break;
	case ART_SPIDERSILK:
		def += 2;//offsetting the cloth material
		break;
	// case ART_CLAWS_OF_THE_REVENANCER:
		// def += 5;
		// break;
	}

	return def;
}

int
properties_dr(arm, agralign, agrmoral)
struct obj *arm;
int agralign;
int agrmoral;
{
	int bonus = 0;
	int base = arm_dr_bonus(arm);
	if(is_harmonium_armor(arm)){
		if(agralign == 0) bonus += 1;
		else if(agralign < 0) bonus += 2;
	}
	if(check_oprop(arm, OPROP_ANAR)){
		if(agralign >= 0) bonus += base;
		else bonus -= base/2+1;
	}
	if(check_oprop(arm, OPROP_CONC)){
		if(agralign != 0) bonus += base;
		else bonus -= base/2+1;
	}
	if(check_oprop(arm, OPROP_AXIO)){
		if(agralign <= 0) bonus += base;
		else bonus -= base/2+1;
	}
	if(check_oprop(arm, OPROP_HOLY) && arm->blessed){
		if(agrmoral < 0) bonus += base;
		else if(agrmoral > 0) bonus -= base/2+1;
	}
	if(check_oprop(arm, OPROP_UNHY) && arm->cursed){
		if(agrmoral > 0) bonus += base;
		else if(agrmoral < 0) bonus -= base/2+1;
	}
	return bonus;
}

#ifdef OVL0

int
base_uac()
{
	int dexbonus = 0;
	int uac = 10-mons[u.umonnum].nac;
	
	if(multi >= 0)
		dexbonus += mons[u.umonnum].dac;
	
	if((uright && uright->oartifact == ART_SHARD_FROM_MORGOTH_S_CROWN) || (uleft && uleft->oartifact == ART_SHARD_FROM_MORGOTH_S_CROWN)){
		uac -= 6;
	}

	if(uwep){
		if((is_rapier(uwep) && arti_shining(uwep)) || 
			(uwep->otyp == LIGHTSABER && litsaber(uwep) && uwep->oartifact != ART_ANNULUS && uwep->ovar1 == 0)
				) uac -= max(
					min(
					(ACURR(A_DEX)-13)/4,
					P_SKILL(weapon_type(uwep))-1
					)
				,0);
		if(uwep->oartifact == ART_LANCE_OF_LONGINUS) uac -= max((uwep->spe+1)/2,0);
		if(uwep->oartifact == ART_TENSA_ZANGETSU){
			uac -= max( (uwep->spe+1)/2,0);
			if(!uarmc || !uarm) uac -= max( uwep->spe,0);
			if(!uarmc && !uarm) uac -= max( (uwep->spe+1)/2,0);
		}
		if(is_lightsaber(uwep) && litsaber(uwep)){
			if(activeFightingForm(FFORM_SORESU) && (!uarm || is_light_armor(uarm) || is_medium_armor(uarm))){
				switch(min(P_SKILL(P_SORESU), P_SKILL(weapon_type(uwep)))){
					case P_BASIC:
						uac -=   max(0, (ACURR(A_DEX)+ACURR(A_INT) - 20)/5);
					break;
					case P_SKILLED:
						uac -= max(0, (ACURR(A_DEX)+ACURR(A_INT) - 20)/3);
					break;
					case P_EXPERT:
						uac -= max(0, (ACURR(A_DEX)+ACURR(A_INT) - 20)/2);
					break;
				}
			} else if(activeFightingForm(FFORM_ATARU) && (!uarm || is_light_armor(uarm))){
				switch(min(P_SKILL(P_ATARU), P_SKILL(weapon_type(uwep)))){
					case P_BASIC:
						uac += 20;
					break;
					case P_SKILLED:
						uac += 10;
					break;
					case P_EXPERT:
						uac += 5;
					break;
				}
			} else if(activeFightingForm(FFORM_MAKASHI) && (!uarm || is_light_armor(uarm) || is_medium_armor(uarm))){
				int sx, sy, mcount = 0;
				for(sx = u.ux-1; sx<=u.ux+1; sx++){
					for(sy = u.uy-1; sy<=u.uy+1; sy++){
						if(isok(sx,sy) && m_at(sx,sy)) mcount++;
					}
				}
				switch(min(P_SKILL(P_MAKASHI), P_SKILL(weapon_type(uwep)))){
					case P_BASIC:
						if(mcount) uac += (mcount-1) * 10;
					break;
					case P_SKILLED:
						if(mcount) uac += (mcount-1) * 5;
					break;
					case P_EXPERT:
						if(mcount) uac += (mcount-1) * 2;
					break;
				}
			}
		}
	}
	if(uleft && uleft->otyp == RIN_PROTECTION) uac -= uleft->spe;
	if(uright && uright->otyp == RIN_PROTECTION) uac -= uright->spe;
	if (HProtection & INTRINSIC) uac -= (u.ublessed+1)/2;
	uac -= u.uacinc;
	uac -= u.spiritAC;
	if(u.uuur_duration)
		uac -= 10;
	uac -= u.uuur/2;
	if(u.edenshield > moves) uac -= 7;
	if(u.specialSealsActive&SEAL_BLACK_WEB && (
		(u.utrap && u.utraptype == TT_WEB) ||
			(t_at(u.ux, u.uy) && t_at(u.ux, u.uy)->ttyp == WEB && (uarm && uarm->oartifact==ART_SPIDERSILK))
		)
	)
		uac -= 8;
	if(u.specialSealsActive&SEAL_UNKNOWN_GOD && uwep && uwep->oartifact == ART_PEN_OF_THE_VOID) uac -= 2*uwep->spe;
	if(multi < 0 || mad_turn(MAD_SUICIDAL) || u.ustuck){
		dexbonus = -5;
	} else {
		dexbonus += (int)( (ACURR(A_DEX)-11)/2 ); /*ranges from -5 to +7 (1 to 25) */
		if(u.umadness&MAD_RAGE && !ClearThoughts){
			dexbonus -= (Insanity)/10;
		}
		if(u.umadness&MAD_NUDIST && !ClearThoughts && u.usanity < 100){
			int delta = Insanity;
			int discomfort = u_clothing_discomfort();
			if (discomfort) {
				dexbonus -= (discomfort * delta)/20;
			} else {
				if (!uwep && !uarms) {
					dexbonus += delta/10;
				}
			}
		}
		if(Race_if(PM_ORC)){
			dexbonus += (u.ulevel+1)/3;
		}
		if(Role_if(PM_MONK) && !uarm){
			if(dexbonus < 0) dexbonus = (int)(dexbonus / 2);
			dexbonus += max((int)( (ACURR(A_WIS)-1)/2 - 5 ),0) + (int)(u.ulevel/6 + 1);
			if(Confusion && u.udrunken>u.ulevel) dexbonus += u.udrunken/9+1;
		}
		/*Corsets suck*/
		if(uarmu && uarmu->otyp == VICTORIAN_UNDERWEAR){
			uac += 2; //flat penalty. Something in the code "corrects" ac values >10, this is a kludge.
			dexbonus = min(dexbonus-2,0);
		}
		
		if(uarm && uarm->otyp == CONSORT_S_SUIT){
			dexbonus = min(dexbonus,0);
		}
		
		if(dexbonus < -5)
			dexbonus = -5;
		
		if(dexbonus > 0 && uarm){
			if(is_medium_armor(uarm))
					dexbonus = (int)(dexbonus/2);
			else if(!is_light_armor(uarm))
					dexbonus = 0;
		}
	}
	uac -= dexbonus;
	if(u.ufirst_sky)
		uac -= 3;
	if(u.uspellprot > 0 && uac > 0) uac = 0;
	uac -= u.uspellprot;
	if (uac < -128) uac = -128;	/* u.uac is an schar */
	return uac;
}

void
find_ac()
{
	int uac;
	
	uac = base_uac();
	uac -= (u.uuur+1)/2;
	if (uarm)	uac -= arm_ac_bonus(uarm);
	if (uarmc)	uac -= arm_ac_bonus(uarmc);
	if (uarmh)	uac -= arm_ac_bonus(uarmh);
	if (uarmf)	uac -= arm_ac_bonus(uarmf);
	if(uarms){
		uac -= max(0, arm_ac_bonus(uarms) + (uarms->objsize - youracedata->msize));
	}
	if (uarmg)	uac -= arm_ac_bonus(uarmg);
	if (uarmu)	uac -= arm_ac_bonus(uarmu);
	
	if(uwep){
		if((is_rapier(uwep) && !arti_shining(uwep)))
			uac -= max(
				min(
				(ACURR(A_DEX)-13)/4,
				P_SKILL(weapon_type(uwep))-1
				)
			,0);
		if(uwep->oartifact == ART_TOBIUME || uwep->oartifact == ART_MASAMUNE)
			uac -= max(uwep->spe,0);
		if(uwep->otyp == NAGINATA && !uarms){
			if(uwep->oartifact == ART_JINJA_NAGINATA)
				uac -= 2+uwep->spe;
			else
				uac -= 1+(uwep->spe)/2;
		}
	}
	if(Race_if(PM_HALF_DRAGON)){
		//DS half dragons may be more humanoid
		if(Role_if(PM_NOBLEMAN) && flags.initgend){
			if(carrying_art(ART_DRAGON_S_HEART_STONE))
				uac -= (u.ulevel+5)/10;
			else uac -= (u.ulevel+5)/15; //level 10 and 25
		} else {
			if(carrying_art(ART_DRAGON_S_HEART_STONE))
				uac -= (u.ulevel+2)/3;
			else uac -= (u.ulevel+3)/6;
		}
	}
	if(u.specialSealsActive&SEAL_COSMOS) uac -= (spiritDsize()/2);
	if(u.sealsActive&SEAL_ECHIDNA) uac -= max((ACURR(A_CON)-10)/2, 0);
	if(u.specialSealsActive&SEAL_DAHLVER_NAR && !Upolyd) uac -=  min(u.ulevel/2,(u.uhpmax - u.uhp)/10);
	else if(u.specialSealsActive&SEAL_DAHLVER_NAR && Upolyd) uac -=  min(u.ulevel/2,(u.mhmax - u.mh)/10);
	if(uclockwork) uac -= (u.clockworkUpgrades&ARMOR_PLATING) ? 5 : 2; /*armor bonus for automata*/
	if(uandroid) uac -= 6; /*armor bonus for androids*/
	if (uac < -128) uac = -128;	/* u.uac is an schar */
	if(uac != u.uac){
		u.uac = uac;
		flags.botl = 1;
	}
	find_dr(); /*Also recalculate the DR*/
}

int base_nat_udr()
{
	int udr = 0;

	if(Race_if(PM_ORC)){
		udr += u.ulevel/10;
	}
	
	if(Race_if(PM_HALF_DRAGON)){
		//DS half dragons may be more humanoid
		if(Role_if(PM_NOBLEMAN) && flags.initgend){
			if(carrying_art(ART_DRAGON_S_HEART_STONE))
				udr += (u.ulevel)/10;
			else udr += (u.ulevel)/15;
		} else {
			if(carrying_art(ART_DRAGON_S_HEART_STONE))
				udr += (u.ulevel)/3;
			else udr += (u.ulevel)/6;
		}
	}
	
	if(u.sealsActive&SEAL_ECHIDNA)
		udr += 2;
	
	if(u.specialSealsActive&SEAL_COSMOS) udr += (spiritDsize()+1)/2;
	if(u.sealsActive&SEAL_ECHIDNA) udr += max((ACURR(A_CON)-9)/4, 0);
	if(uclockwork && u.clockworkUpgrades&ARMOR_PLATING) udr += 4; /*armor bonus for automata (stacks with the 1 natural DR)*/
	
	if (udr > 127) udr = 127;	/* u.uac is an schar */
	return udr;
}

int base_udr()
{
	int udr = 0;
	
	if((uright && uright->oartifact == ART_SHARD_FROM_MORGOTH_S_CROWN) || (uleft && uleft->oartifact == ART_SHARD_FROM_MORGOTH_S_CROWN)){
		udr += 3;
	}
	
	if(uwep){
		if(uwep->oartifact == ART_LANCE_OF_LONGINUS) udr += max((uwep->spe)/2,0);
	}
	if (HProtection & INTRINSIC) udr += (u.ublessed)/2;
	if(u.edenshield > moves) udr += 7;

	if(u.umadness&MAD_NUDIST && !ClearThoughts && u.usanity < 100){
		int delta = Insanity;
		int discomfort = u_clothing_discomfort();
		if (discomfort) {
			udr -= (discomfort * delta)/100;
		}
	}

	if (udr > 127) udr = 127;
	if (udr < 0) udr = 0;
	return udr;
}

void
find_dr()
{
	int udr = 0, i;
	
	for(i = 0; i < 5; i++)
		udr += slot_udr(1<<i,0);
	udr += slot_udr(UPPER_TORSO_DR,0);
	udr += slot_udr(LOWER_TORSO_DR,0);
	udr += max(slot_udr(UPPER_TORSO_DR,0), slot_udr(ARM_DR,0))*2;
	udr /= 9;
	if (udr > 127) udr = 127;	/* u.uac is an schar */
	if(udr != u.udr){
		u.udr = udr;
		flags.botl = 1;
	}
}

#endif /* OVL0 */
#ifdef OVLB

/* Calculates your DR for a slot 
 * Does not randomize values >10 (must be done elsewhere)
 * 
 * Includes effectiveness vs magr (optional)
 */
int
slot_udr(slot, magr)
int slot;
struct monst *magr;
{
	/* DR addition: bas + sqrt(nat^2 + arm^2) */
	int bas_udr; /* base DR:    magical-ish   */
	int nat_udr; /* natural DR: (poly)form    */
	int arm_udr; /* armor DR:   worn armor    */

	bas_udr = base_udr();
	nat_udr = base_nat_udr();
	arm_udr = 0;

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
	if (slot == HEAD_DR && !has_head_mon(&youmonst))
		slot = UPPER_TORSO_DR;
	if (slot == LEG_DR && !can_wear_boots(youracedata))
		slot = LOWER_TORSO_DR;
	if (slot == ARM_DR && !can_wear_gloves(youracedata))
		slot = UPPER_TORSO_DR;

	/* DR of worn armor */
	struct obj * uarmor[] = { uarm, uarmc, uarmf, uarmh, uarmg, uarms, uarmu };
	int i;
	for (i = 0; i < SIZE(uarmor); i++) {
		if (uarmor[i] && (objects[uarmor[i]->otyp].oc_dir & slot)) {
			arm_udr += arm_dr_bonus(uarmor[i]);
			if (magr) arm_udr += properties_dr(uarmor[i], agralign, agrmoral);
		}
	}
	/* Tensa Zangetsu adds to worn armor */
	if (uwep && uwep->oartifact == ART_TENSA_ZANGETSU) {
		if (!uarmc && (slot & CLOAK_DR)) {
			arm_udr += max(1 + (uwep->spe + 1) / 2, 0);
		}
		if (!uarm && (slot & TORSO_DR)) {
			arm_udr += max(1 + (uwep->spe + 1) / 2, 0);
		}
	}
	/* Natural DR (overriden and ignored by base_nat_udr() for halfdragons) */
	if (!Race_if(PM_HALF_DRAGON)) {
		switch (slot)
		{
		case UPPER_TORSO_DR: nat_udr += youracedata->bdr; break;
		case LOWER_TORSO_DR: nat_udr += youracedata->ldr; break;
		case HEAD_DR:        nat_udr += youracedata->hdr; break;
		case LEG_DR:         nat_udr += youracedata->fdr; break;
		case ARM_DR:         nat_udr += youracedata->gdr; break;
		}
	}
	/* Wearing the Shard from Morgoth's Crown adds +3 magical DR to arms and head (in addition to its +3 to all slots) */
	if (((uright && uright->oartifact == ART_SHARD_FROM_MORGOTH_S_CROWN) || (uleft && uleft->oartifact == ART_SHARD_FROM_MORGOTH_S_CROWN))
		&& (slot & (ARM_DR | HEAD_DR))) {
		bas_udr += 3;
	}
	/* Vaul is not randomized, and contributes to magical DR */
	if (u.uvaul) {
		int offset;
		switch (slot)
		{
		case UPPER_TORSO_DR: offset = 3; break;
		case LOWER_TORSO_DR: offset = 1; break;
		case HEAD_DR:        offset = 4; break;
		case LEG_DR:         offset = 0; break;
		case ARM_DR:         offset = 2; break;
		}
		bas_udr += ((u.uvaul + offset) / 5);
	}
	/* Having the Deep Sea glyph increase magical DR by 3 */
	if (active_glyph(DEEP_SEA))
		bas_udr += 3;
	
	//Star spawn reach extra-dimensionally past all armor, even bypassing natural armor.
	if(magr && (magr->mtyp == PM_STAR_SPAWN || magr->mtyp == PM_GREAT_CTHULHU || mad_monster_turn(magr, MAD_NON_EUCLID))){
		arm_udr = 0;
		if(undiffed_innards(youracedata))
			nat_udr /= 2;
		else if(!no_innards(youracedata) && !removed_innards(youracedata))
			nat_udr = 0;
	}
	
	/* Combine into total */
	int total_dr = bas_udr;
	if(arm_udr > 0 && nat_udr > 0) {
		total_dr += (int)sqrt(nat_udr*nat_udr + arm_udr*arm_udr);
	}
	else {
		total_dr += nat_udr + arm_udr;
	}

	/* cap at 127 (u.udr is an schar) */
	if (total_dr > 127)
		total_dr = 127;
	
	return total_dr;
}

int
roll_udr(magr)
struct monst *magr;
{
	int slot;
	int udr;
	int cap = 10;
	switch(rn2(9)){
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
		case 7:
		case 8:
			if(slot_udr(UPPER_TORSO_DR, magr) > slot_udr(ARM_DR, magr))
				slot = UPPER_TORSO_DR;
			else slot = ARM_DR;
		break;
	}
	udr = slot_udr(slot, magr);
	if(active_glyph(DEEP_SEA))
		cap += 3;
	//diminishing returns after 10 points of DR.
	if(udr > (cap+1)){
		if(u.sealsActive&SEAL_BALAM)
			udr = cap + max_ints(rnd(udr-cap), rnd(udr-cap));
		else udr = cap + rnd(udr-cap);
	}
	return udr;
}

void
glibr()
{
	register struct obj *otmp;
	int xfl = 0;
	boolean leftfall, rightfall;
	const char *otherwep = 0;

	leftfall = (uleft && !uleft->cursed && !Weldproof &&
		    (!uwep || !welded(uwep) || !bimanual(uwep,youracedata)));
	rightfall = (uright && !uright->cursed && !Weldproof && (!welded(uwep)));
	if (!uarmg && (leftfall || rightfall) && !nolimbs(youracedata)) {
		/* changed so cursed rings don't fall off, GAN 10/30/86 */
		Your("%s off your %s.",
			(leftfall && rightfall) ? "rings slip" : "ring slips",
			(leftfall && rightfall) ? makeplural(body_part(FINGER)) :
			body_part(FINGER));
		xfl++;
		if (leftfall) {
			otmp = uleft;
			Ring_off(uleft);
			dropx(otmp);
		}
		if (rightfall) {
			otmp = uright;
			Ring_off(uright);
			dropx(otmp);
		}
	}

	otmp = uswapwep;
	if (u.twoweap && otmp && otmp->otyp != ARM_BLASTER) {
		otherwep = is_sword(otmp) ? c_sword :
		    makesingular(oclass_names[(int)otmp->oclass]);
		Your("%s %sslips from your %s.",
			otherwep,
			xfl ? "also " : "",
			makeplural(body_part(HAND)));
		setuswapwep((struct obj *)0);
		xfl++;
		if (otmp->otyp != LOADSTONE || !otmp->cursed)
			dropx(otmp);
	}
	otmp = uwep;
	if (otmp && !welded(otmp) && otmp->otyp != ARM_BLASTER) {
		const char *thiswep;

		/* nice wording if both weapons are the same type */
		thiswep = is_sword(otmp) ? c_sword :
		    makesingular(oclass_names[(int)otmp->oclass]);
		if (otherwep && strcmp(thiswep, otherwep)) otherwep = 0;

		/* changed so cursed weapons don't fall, GAN 10/30/86 */
		Your("%s%s %sslips from your %s.",
			otherwep ? "other " : "", thiswep,
			xfl ? "also " : "",
			makeplural(body_part(HAND)));
		setuwep((struct obj *)0);
		if (otmp->otyp != LOADSTONE || !otmp->cursed)
			dropx(otmp);
	}
}

void
bumbler()
{
	register struct obj *otmp;
	int xfl = 0;
	const char *otherwep = 0;

	otmp = uswapwep;
	if (u.twoweap && otmp && otmp->otyp != ARM_BLASTER) {
		otherwep = is_sword(otmp) ? c_sword :
		    makesingular(oclass_names[(int)otmp->oclass]);
		You("fumble your %s and %s %sslips from your %s.",
			otherwep,
			otmp->quan > 1 ? "they" : "it",
			xfl ? "also " : "",
			makeplural(body_part(HAND)));
		setuswapwep((struct obj *)0);
		xfl++;
		if (otmp->otyp != LOADSTONE || !otmp->cursed)
			dropx(otmp);
	}
	otmp = uwep;
	if (otmp && !welded(otmp) && otmp->otyp != ARM_BLASTER) {
		const char *thiswep;

		/* nice wording if both weapons are the same type */
		thiswep = is_sword(otmp) ? c_sword :
		    makesingular(oclass_names[(int)otmp->oclass]);
		if (otherwep && strcmp(thiswep, otherwep)) otherwep = 0;

		/* changed so cursed weapons don't fall, GAN 10/30/86 */
		You("%sfumble your %s%s and %s slips from your %s.",
			xfl ? "also " : "",
			otherwep ? "other " : "",
			thiswep,
			otmp->quan > 1 ? "they" : "it",
			makeplural(body_part(HAND)));
		setuwep((struct obj *)0);
		if (otmp->otyp != LOADSTONE || !otmp->cursed)
			dropx(otmp);
	}
}

struct obj *
some_armor(victim)
struct monst *victim;
{
	register struct obj *otmph, *otmp;

	otmph = (victim == &youmonst) ? uarmc : which_armor(victim, W_ARMC);
	if (!otmph)
	    otmph = (victim == &youmonst) ? uarm : which_armor(victim, W_ARM);
#ifdef TOURIST
	if (!otmph)
	    otmph = (victim == &youmonst) ? uarmu : which_armor(victim, W_ARMU);
#endif
	
	otmp = (victim == &youmonst) ? uarmh : which_armor(victim, W_ARMH);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	otmp = (victim == &youmonst) ? uarmg : which_armor(victim, W_ARMG);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	otmp = (victim == &youmonst) ? uarmf : which_armor(victim, W_ARMF);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	otmp = (victim == &youmonst) ? uarms : which_armor(victim, W_ARMS);
	if(otmp && (!otmph || !rn2(4))) otmph = otmp;
	return(otmph);
}

/* erode some arbitrary armor worn by the victim */
void
erode_armor(victim, acid_dmg)
struct monst *victim;
boolean acid_dmg;
{
	struct obj *otmph = some_armor(victim);

	if (otmph && (otmph != uarmf)) {
	    erode_obj(otmph, acid_dmg, FALSE);
	    if (carried(otmph)) update_inventory();
	}
}

/* used for praying to check and fix levitation trouble */
struct obj *
stuck_ring(ring, otyp)
struct obj *ring;
int otyp;
{
    if (ring != uleft && ring != uright) {
	impossible("stuck_ring: neither left nor right?");
	return (struct obj *)0;
    }

    if (ring && ring->otyp == otyp) {
	/* reasons ring can't be removed match those checked by select_off();
	   limbless case has extra checks because ordinarily it's temporary */
	if (nolimbs(youracedata) &&
		uamul && uamul->otyp == AMULET_OF_UNCHANGING && uamul->cursed && !Weldproof)
	    return uamul;
	if (welded(uwep) && (ring == uright || bimanual(uwep,youracedata))) return uwep;
	if (uarmg && uarmg->cursed && !Weldproof) return uarmg;
	if (ring->cursed && !Weldproof) return ring;
    }
    /* either no ring or not right type or nothing prevents its removal */
    return (struct obj *)0;
}

/* also for praying; find worn item that confers "Unchanging" attribute */
struct obj *
unchanger()
{
    if (uamul && uamul->otyp == AMULET_OF_UNCHANGING) return uamul;
    return 0;
}

/* occupation callback for 'A' */
STATIC_PTR
int
select_off(otmp)
register struct obj *otmp;
{
	struct obj *why;
	char buf[BUFSZ];

	if (!otmp) return 0;
	*buf = '\0';			/* lint suppresion */

	/* special ring checks */
	if (otmp == uright || otmp == uleft) {
	    if (nolimbs(youracedata)) {
		pline_The("ring is stuck.");
		return 0;
	    }
	    why = 0;	/* the item which prevents ring removal */
	    if (welded(uwep) && (otmp == uright || bimanual(uwep,youracedata))) {
		Sprintf(buf, "free a weapon %s", body_part(HAND));
		why = uwep;
	    } else if (uarmg && uarmg->cursed && !Weldproof) {
			Sprintf(buf, "take off your %s", c_gloves);
			why = uarmg;
	    }
	    if (why) {
			You("cannot %s to remove the ring.", buf);
			why->bknown = TRUE;
			return 0;
	    }
	}
	/* special glove checks */
	if (otmp == uarmg) {
	    if (welded(uwep)) {
		You("are unable to take off your %s while wielding that %s.",
		    c_gloves, is_sword(uwep) ? c_sword : c_weapon);
		uwep->bknown = TRUE;
		return 0;
	    } else if (Glib) {
		You_cant("take off the slippery %s with your slippery %s.",
			 c_gloves, makeplural(body_part(FINGER)));
		return 0;
	    }
	}
	/* special boot checks */
	if (otmp == uarmf) {
	    if (u.utrap && u.utraptype == TT_BEARTRAP) {
		pline_The("bear trap prevents you from pulling your %s out.",
			  body_part(FOOT));
		return 0;
	    } else if (u.utrap && u.utraptype == TT_INFLOOR) {
		You("are stuck in the %s, and cannot pull your %s out.",
		    surface(u.ux, u.uy), makeplural(body_part(FOOT)));
		return 0;
	    }
	}
	/* special suit and shirt checks */
	if (otmp == uarm
#ifdef TOURIST
			|| otmp == uarmu
#endif
		) {
	    why = 0;	/* the item which prevents disrobing */
	    if (uarmc && uarmc->cursed && !Weldproof) {
		Sprintf(buf, "remove your %s", cloak_simple_name(uarmc));
		why = uarmc;
#ifdef TOURIST
	    } else if (otmp == uarmu && uarm && uarm->cursed && !Weldproof) {
		Sprintf(buf, "remove your %s", c_suit);
		why = uarm;
#endif
	    } else if (welded(uwep) && bimanual(uwep,youracedata)) {
		Sprintf(buf, "release your %s",
			is_sword(uwep) ? c_sword :
			(uwep->otyp == BATTLE_AXE) ? c_axe : c_weapon);
		why = uwep;
	    }
	    if (why) {
		You("cannot %s to take off %s.", buf, the(xname(otmp)));
		why->bknown = TRUE;
		return 0;
	    }
	}
	/* basic curse check */
	if (otmp == uquiver || (otmp == uswapwep && !u.twoweap)) {
	    ;	/* some items can be removed even when cursed */
	} else {
	    /* otherwise, this is fundamental */
	    if (cursed(otmp)) return 0;
	}

	if(otmp == uarm) takeoff_mask |= WORN_ARMOR;
	else if(otmp == uarmc) takeoff_mask |= WORN_CLOAK;
	else if(otmp == uarmf) takeoff_mask |= WORN_BOOTS;
	else if(otmp == uarmg) takeoff_mask |= WORN_GLOVES;
	else if(otmp == uarmh) takeoff_mask |= WORN_HELMET;
	else if(otmp == uarms) takeoff_mask |= WORN_SHIELD;
#ifdef TOURIST
	else if(otmp == uarmu) takeoff_mask |= WORN_SHIRT;
#endif
	else if(otmp == uleft) takeoff_mask |= LEFT_RING;
	else if(otmp == uright) takeoff_mask |= RIGHT_RING;
	else if(otmp == uamul) takeoff_mask |= WORN_AMUL;
	else if(otmp == ublindf) takeoff_mask |= WORN_BLINDF;
	else if(otmp == uwep) takeoff_mask |= W_WEP;
	else if(otmp == uswapwep) takeoff_mask |= W_SWAPWEP;
	else if(otmp == uquiver) takeoff_mask |= W_QUIVER;

	else impossible("select_off: %s???", doname(otmp));

	return(0);
}

STATIC_OVL struct obj *
do_takeoff()
{
	register struct obj *otmp = (struct obj *)0;

	if (taking_off == W_WEP) {
	  if(!cursed(uwep)) {
	    setuwep((struct obj *) 0);
	    You("are empty %s.", body_part(HANDED));
		if(u.twoweap && !test_twoweapon())
			untwoweapon();
	  }
	} else if (taking_off == W_SWAPWEP) {
		setuswapwep((struct obj *) 0);
		You("no longer have a second weapon readied.");
		if(u.twoweap && !test_twoweapon())
			untwoweapon();
	} else if (taking_off == W_QUIVER) {
	  setuqwep((struct obj *) 0);
	  You("no longer have ammunition readied.");
	} else if (taking_off == WORN_ARMOR) {
	  otmp = uarm;
	  if(!cursed(otmp)) (void) Armor_off();
	} else if (taking_off == WORN_CLOAK) {
	  otmp = uarmc;
	  if(!cursed(otmp)) (void) Cloak_off();
	} else if (taking_off == WORN_BOOTS) {
	  otmp = uarmf;
	  if(!cursed(otmp)) (void) Boots_off();
	} else if (taking_off == WORN_GLOVES) {
	  otmp = uarmg;
	  if(!cursed(otmp)) (void) Gloves_off();
	} else if (taking_off == WORN_HELMET) {
	  otmp = uarmh;
	  if(!cursed(otmp)) (void) Helmet_off();
	} else if (taking_off == WORN_SHIELD) {
	  otmp = uarms;
	  if(!cursed(otmp)) (void) Shield_off();
#ifdef TOURIST
	} else if (taking_off == WORN_SHIRT) {
	  otmp = uarmu;
	  if (!cursed(otmp)) (void) Shirt_off();
#endif
	} else if (taking_off == WORN_AMUL) {
	  otmp = uamul;
	  if(!cursed(otmp)) Amulet_off();
	} else if (taking_off == LEFT_RING) {
	  otmp = uleft;
	  if(!cursed(otmp)) Ring_off(uleft);
	} else if (taking_off == RIGHT_RING) {
	  otmp = uright;
	  if(!cursed(otmp)) Ring_off(uright);
	} else if (taking_off == WORN_BLINDF) {
	  if (!cursed(ublindf)) Blindf_off(ublindf);
	} else impossible("do_takeoff: taking off %lx", taking_off);

	return(otmp);
}

static const char *disrobing = "";

STATIC_PTR
int
take_off()
{
	register int i;
	register struct obj *otmp;
	
	if (taking_off) {
	    if (todelay > 0) {
		todelay--;
		return(1);	/* still busy */
	    } else {
		if ((otmp = do_takeoff())) off_msg(otmp);
	    }
	    takeoff_mask &= ~taking_off;
	    taking_off = 0L;
	}

	for(i = 0; takeoff_order[i]; i++)
	    if(takeoff_mask & takeoff_order[i]) {
		taking_off = takeoff_order[i];
		break;
	    }

	otmp = (struct obj *) 0;
	todelay = 0;

	if (taking_off == 0L) {
	  You("finish %s.", disrobing);
	  return 0;
	} else if (taking_off == W_WEP) {
	  todelay = 1;
	} else if (taking_off == W_SWAPWEP) {
	  todelay = 1;
	} else if (taking_off == W_QUIVER) {
	  todelay = 1;
	} else if (taking_off == WORN_ARMOR) {
	  otmp = uarm;
	  /* If a cloak is being worn, add the time to take it off and put
	   * it back on again.  Kludge alert! since that time is 0 for all
	   * known cloaks, add 1 so that it actually matters...
	   */
	  if (uarmc) todelay += 2 * objects[uarmc->otyp].oc_delay + 1;
	} else if (taking_off == WORN_CLOAK) {
	  otmp = uarmc;
	} else if (taking_off == WORN_BOOTS) {
	  otmp = uarmf;
	} else if (taking_off == WORN_GLOVES) {
	  otmp = uarmg;
	} else if (taking_off == WORN_HELMET) {
	  otmp = uarmh;
	} else if (taking_off == WORN_SHIELD) {
	  otmp = uarms;
#ifdef TOURIST
	} else if (taking_off == WORN_SHIRT) {
	  otmp = uarmu;
	  /* add the time to take off and put back on armor and/or cloak */
	  if (uarm)  todelay += 2 * objects[uarm->otyp].oc_delay;
	  if (uarmc) todelay += 2 * objects[uarmc->otyp].oc_delay + 1;
#endif
	} else if (taking_off == WORN_AMUL) {
	  todelay = 1;
	} else if (taking_off == LEFT_RING) {
	  todelay = 1;
	} else if (taking_off == RIGHT_RING) {
	  todelay = 1;
	} else if (taking_off == WORN_BLINDF) {
	  todelay = 2;
	} else {
	  impossible("take_off: taking off %lx", taking_off);
	  return 0;	/* force done */
	}

	if (otmp) todelay += objects[otmp->otyp].oc_delay;

	/* Since setting the occupation now starts the counter next move, that
	 * would always produce a delay 1 too big per item unless we subtract
	 * 1 here to account for it.
	 */
	if (todelay > 0) todelay--;

	set_occupation(take_off, disrobing, 0);
	return(1);		/* get busy */
}

/* clear saved context to avoid inappropriate resumption of interrupted 'A' */
void
reset_remarm()
{
	taking_off = takeoff_mask = 0L;
	disrobing = nul;
}

/* the 'A' command -- remove multiple worn items */
int
doddoremarm()
{
    int result = 0;

    if (taking_off || takeoff_mask) {
	You("continue %s.", disrobing);
	set_occupation(take_off, disrobing, 0);
	return 0;
    } else if (!uwep && !uswapwep && !uquiver && !uamul && !ublindf &&
		!uleft && !uright && !wearing_armor()) {
	You("are not wearing anything.");
	return 0;
    }

    add_valid_menu_class(0); /* reset */
    if (flags.menu_style != MENU_TRADITIONAL ||
	    (result = ggetobj("take off", select_off, 0, FALSE, (unsigned *)0)) < -1)
	result = menu_remarm(result);

    if (takeoff_mask) {
	/* default activity for armor and/or accessories,
	   possibly combined with weapons */
	disrobing = "disrobing";
	/* specific activity when handling weapons only */
	if (!(takeoff_mask & ~(W_WEP|W_SWAPWEP|W_QUIVER)))
	    disrobing = "disarming";
	(void) take_off();
    }
    /* The time to perform the command is already completely accounted for
     * in take_off(); if we return 1, that would add an extra turn to each
     * disrobe.
     */
    return 0;
}

STATIC_OVL int
menu_remarm(retry)
int retry;
{
    int n, i = 0;
    menu_item *pick_list;
    boolean all_worn_categories = TRUE;

    if (retry) {
	all_worn_categories = (retry == -2);
    } else if (flags.menu_style == MENU_FULL) {
	all_worn_categories = FALSE;
	n = query_category("What type of things do you want to take off?",
			   invent, WORN_TYPES|ALL_TYPES, &pick_list, PICK_ANY);
	if (!n) return 0;
	for (i = 0; i < n; i++) {
	    if (pick_list[i].item.a_int == ALL_TYPES_SELECTED)
		all_worn_categories = TRUE;
	    else
		add_valid_menu_class(pick_list[i].item.a_int);
	}
	free((genericptr_t) pick_list);
    } else if (flags.menu_style == MENU_COMBINATION) {
	all_worn_categories = FALSE;
	if (ggetobj("take off", select_off, 0, TRUE, (unsigned *)0) == -2)
	    all_worn_categories = TRUE;
    }

    n = query_objlist("What do you want to take off?", invent,
			SIGNAL_NOMENU|USE_INVLET|INVORDER_SORT,
			&pick_list, PICK_ANY,
			all_worn_categories ? is_worn : is_worn_by_type);
    if (n > 0) {
	for (i = 0; i < n; i++)
	    (void) select_off(pick_list[i].item.a_obj);
	free((genericptr_t) pick_list);
    } else if (n < 0 && flags.menu_style != MENU_COMBINATION) {
	There("is nothing else you can remove or unwield.");
    }
    return 0;
}

/* hit by destroy armor scroll/black dragon breath/monster spell */
int
destroy_arm(atmp)
register struct obj *atmp;
{
	register struct obj *otmp;
#define DESTROY_ARM(o) ((otmp = (o)) != 0 && \
			(!atmp || atmp == otmp))
	
	if(Preservation)
		return 0;
	
	if (DESTROY_ARM(uarmc)) {
		if((!obj_resists(otmp, 0, 100))){
			if (donning(otmp)) cancel_don();
			Your("%s crumbles and turns to dust!",
				 cloak_simple_name(uarmc));
			(void) Cloak_off();
			useup(otmp);
			if(roll_madness(MAD_TALONS)){
				You("panic after having your cloak destroyed!");
				HPanicking += 1+rnd(6);
			}
		} else {
			Your("%s resists destruction!", cloak_simple_name(uarmc));
		}
	} else if (DESTROY_ARM(uarm)) {
		if((!obj_resists(otmp, 0, 100))){
			if (donning(otmp)) cancel_don();
			Your("armor turns to dust and falls to the %s!",
				surface(u.ux,u.uy));
			(void) Armor_gone();
			useup(otmp);
			if(roll_madness(MAD_TALONS)){
				You("panic after having your armor destroyed!");
				HPanicking += 1+rnd(6);
			}
		} else {
			Your("armor resists destruction!");
		}
	} else if (DESTROY_ARM(uarmu)) {
		if((!obj_resists(otmp, 0, 100))){
			if (donning(otmp)) cancel_don();
			Your("underclothes crumble into tiny threads and fall apart!");
			(void) Shirt_off();
			useup(otmp);
			if(roll_madness(MAD_TALONS)){
				You("panic after having your underclothes destroyed!");
				HPanicking += 1+rnd(6);
			}
		} else {
			Your("underclothes resists destruction!");
		}
	} else if (DESTROY_ARM(uarmh)) {
		if((!obj_resists(otmp, 0, 100))){
			if (donning(otmp)) cancel_don();
			Your("helmet turns to dust and is blown away!");
			(void) Helmet_off();
			useup(otmp);
			if(roll_madness(MAD_TALONS)){
				You("panic after having your helmet destroyed!");
				HPanicking += 1+rnd(6);
			}
		} else {
			Your("helmet resists destruction!");
		}
	} else if (DESTROY_ARM(uarmg)) {
		if((!obj_resists(otmp, 0, 100))){
			if (donning(otmp)) cancel_don();
			Your("gloves vanish!");
			(void) Gloves_off();
			useup(otmp);
			selftouch("You");
			if(roll_madness(MAD_TALONS)){
				You("panic after having your gloves destroyed!");
				HPanicking += 1+rnd(6);
			}
		} else {
			Your("gloves resists destruction!");
		}
	} else if (DESTROY_ARM(uarmf)) {
		if((!obj_resists(otmp, 0, 100))){
			if (donning(otmp)) cancel_don();
			Your("boots disintegrate!");
			(void) Boots_off();
			useup(otmp);
			if(roll_madness(MAD_TALONS)){
				You("panic after having your boots destroyed!");
				HPanicking += 1+rnd(6);
			}
		} else {
			Your("boots resists destruction!");
		}
	} else if (DESTROY_ARM(uarms)) {
		if((!obj_resists(otmp, 0, 100))){
			if (donning(otmp)) cancel_don();
			Your("shield crumbles away!");
			(void) Shield_off();
			useup(otmp);
			if(roll_madness(MAD_TALONS)){
				You("panic after having your shield destroyed!");
				HPanicking += 1+rnd(6);
			}
		} else {
			Your("shield resists destruction!");
		}
	} else {
		return 0;		/* could not destroy anything */
	}

#undef DESTROY_ARM
	stop_occupation();
	return(1);
}

int
destroy_marm(mtmp, otmp)
register struct monst *mtmp;
register struct obj *otmp;
{
	long unwornmask;
	if(!otmp || !mtmp)
		return 0;
	if(obj_resists(otmp, 0, 100))
		return 0;
	if(!otmp->owornmask)
		return 0;
	obj_extract_self(otmp);
	if ((unwornmask = otmp->owornmask) != 0L) {
		mtmp->misc_worn_check &= ~unwornmask;
		if (otmp->owornmask & W_WEP){
			setmnotwielded(mtmp,otmp);
			MON_NOWEP(mtmp);
		}
		if (otmp->owornmask & W_SWAPWEP){
			setmnotwielded(mtmp,otmp);
			MON_NOSWEP(mtmp);
		}
		otmp->owornmask = 0L;
		update_mon_intrinsics(mtmp, otmp, FALSE, FALSE);
		if(unwornmask&W_ARM){
			if(canseemon(mtmp))
				pline("%s armor turns to dust!", s_suffix(Monnam(mtmp)));
		} else if(unwornmask&W_ARMC){
			if(canseemon(mtmp))
				pline("%s %s crumbles and turns to dust!", s_suffix(Monnam(mtmp)), cloak_simple_name(otmp));
		} else if(unwornmask&W_ARMH){
			if(canseemon(mtmp))
				pline("%s helm turns to dust and is blown away!", s_suffix(Monnam(mtmp)));
		} else if(unwornmask&W_ARMS){
			if(canseemon(mtmp))
				pline("%s shield crumbles away!", s_suffix(Monnam(mtmp)));
		} else if(unwornmask&W_ARMG){
			if(canseemon(mtmp))
				pline("%s gloves vanish!", s_suffix(Monnam(mtmp)));
		} else if(unwornmask&W_ARMF){
			if(canseemon(mtmp))
				pline("%s boots disintegrate!", s_suffix(Monnam(mtmp)));
		} else if(unwornmask&W_ARMU){
			if(canseemon(mtmp))
				pline("%s underclothes crumble into tiny threads and fall apart!", s_suffix(Monnam(mtmp)));
		}
		m_useup(mtmp, otmp);
	}
	return 1;
}

/* hit by destroy armor scroll/black dragon breath/monster spell */
int
claws_destroy_arm(atmp)
register struct obj *atmp;
{
	register struct obj *otmp;
#define DESTROY_ARM(o) ((otmp = (o)) != 0 && \
			(!atmp || atmp == otmp) && \
			(!obj_resists(otmp, 0, 90)))

	if(Preservation)
		return 0;
	
	if (DESTROY_ARM(uarmc)) {
		if (donning(otmp)) cancel_don();
		Your("%s is torn to shreds!",
		     cloak_simple_name(uarmc));
		(void) Cloak_off();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your cloak shredded!");
			HPanicking += 1+rnd(6);
		}
	} else if (DESTROY_ARM(uarm)) {
		if (donning(otmp)) cancel_don();
		Your("armor rips open and falls to the %s!",
			surface(u.ux,u.uy));
		(void) Armor_gone();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your armor ripped open!");
			HPanicking += 1+rnd(6);
		}
#ifdef TOURIST
	} else if (DESTROY_ARM(uarmu)) {
		if (donning(otmp)) cancel_don();
		Your("underclothes are torn off!");
		(void) Shirt_off();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your underclothes torn off!");
			HPanicking += 1+rnd(6);
		}
#endif
	} else if (DESTROY_ARM(uarmh)) {
		if (donning(otmp)) cancel_don();
		Your("helmet is knocked to pieces!");
		(void) Helmet_off();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your helmet destroyed!");
			HPanicking += 1+rnd(6);
		}
	} else if (DESTROY_ARM(uarmg)) {
		if (donning(otmp)) cancel_don();
		Your("gloves are torn off!");
		(void) Gloves_off();
		useup(otmp);
		selftouch("You");
		if(roll_madness(MAD_TALONS)){
			You("panic after having your gloves ripped off!");
			HPanicking += 1+rnd(6);
		}
	} else if (DESTROY_ARM(uarmf)) {
		if (donning(otmp)) cancel_don();
		Your("boots are ripped open!");
		(void) Boots_off();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your boots ripped open!");
			HPanicking += 1+rnd(6);
		}
	} else if (DESTROY_ARM(uarms)) {
		if (donning(otmp)) cancel_don();
		Your("shield shatters!");
		(void) Shield_off();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your shield destroyed!");
			HPanicking += 1+rnd(6);
		}
	} else {
		return 0;		/* could not destroy anything */
	}

#undef DESTROY_ARM
	stop_occupation();
	return(1);
}

int
claws_destroy_marm(mtmp, otmp)
register struct monst *mtmp;
register struct obj *otmp;
{
	/* call the player's version if need be */
	if (mtmp == &youmonst)
		return claws_destroy_arm(otmp);

	long unwornmask;
	if(!otmp || !mtmp)
		return 0;
	if(obj_resists(otmp, 0, 100))
		return 0;
	if(!otmp->owornmask)
		return 0;
	obj_extract_self(otmp);
	if ((unwornmask = otmp->owornmask) != 0L) {
		mtmp->misc_worn_check &= ~unwornmask;
		if (otmp->owornmask & W_WEP){
			setmnotwielded(mtmp,otmp);
			MON_NOWEP(mtmp);
		}
		if (otmp->owornmask & W_SWAPWEP){
			setmnotwielded(mtmp,otmp);
			MON_NOSWEP(mtmp);
		}
		otmp->owornmask = 0L;
		update_mon_intrinsics(mtmp, otmp, FALSE, FALSE);
		if(unwornmask&W_ARM){
			if(canseemon(mtmp))
				pline("%s armor rips open!", s_suffix(Monnam(mtmp)));
		} else if(unwornmask&W_ARMC){
			if(canseemon(mtmp))
				pline("%s %s is torn to shreds!", s_suffix(Monnam(mtmp)), cloak_simple_name(otmp));
		} else if(unwornmask&W_ARMH){
			if(canseemon(mtmp))
				pline("%s helm is knocked to pieces!", s_suffix(Monnam(mtmp)));
		} else if(unwornmask&W_ARMS){
			if(canseemon(mtmp))
				pline("%s shield shatters!", s_suffix(Monnam(mtmp)));
		} else if(unwornmask&W_ARMG){
			if(canseemon(mtmp))
				pline("%s gloves are torn off!", s_suffix(Monnam(mtmp)));
		} else if(unwornmask&W_ARMF){
			if(canseemon(mtmp))
				pline("%s boots are ripped open!", s_suffix(Monnam(mtmp)));
		} else if(unwornmask&W_ARMU){
			if(canseemon(mtmp))
				pline("%s underclothes are torn off!", s_suffix(Monnam(mtmp)));
		}
		m_useup(mtmp, otmp);
	}
	return 1;
}

int
teleport_arm(atmp)
register struct obj *atmp;
{
	register struct obj *otmp;
#define TELEPORT_ARM(o) ((otmp = (o)) != 0 && \
			(!atmp || atmp == otmp) && \
			(!obj_resists(otmp, 66, 90)))

	if (TELEPORT_ARM(uarmc)) {
		if (donning(otmp)) cancel_don();
		Your("%s vanishes!",
		     cloak_simple_name(uarmc));
		(void) Cloak_off();
		obj_extract_self(otmp);
		dropy(otmp);
		rloco(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after losing your cloak!");
			HPanicking += 1+rnd(6);
		}
	} else if (TELEPORT_ARM(uarm)) {
		if (donning(otmp)) cancel_don();
		Your("armor vanishes!");
		(void) Armor_gone();
		obj_extract_self(otmp);
		dropy(otmp);
		rloco(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after losing your armor!");
			HPanicking += 1+rnd(6);
		}
	} else if (TELEPORT_ARM(uarmu)) {
		if (donning(otmp)) cancel_don();
		Your("underclothes vanish!");
		(void) Shirt_off();
		obj_extract_self(otmp);
		dropy(otmp);
		rloco(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after losing your underclothes!");
			HPanicking += 1+rnd(6);
		}
	} else if (TELEPORT_ARM(uarmh)) {
		if (donning(otmp)) cancel_don();
		Your("helmet vanishes!");
		(void) Helmet_off();
		obj_extract_self(otmp);
		dropy(otmp);
		rloco(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after losing your helmet!");
			HPanicking += 1+rnd(6);
		}
	} else if (TELEPORT_ARM(uarmg)) {
		if (donning(otmp)) cancel_don();
		Your("gloves vanish!");
		(void) Gloves_off();
		obj_extract_self(otmp);
		dropy(otmp);
		rloco(otmp);
		selftouch("You");
		if(roll_madness(MAD_TALONS)){
			You("panic after losing your gloves!");
			HPanicking += 1+rnd(6);
		}
	} else if (TELEPORT_ARM(uarmf)) {
		if (donning(otmp)) cancel_don();
		Your("boots vanish!");
		(void) Boots_off();
		obj_extract_self(otmp);
		dropy(otmp);
		rloco(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after losing your boots!");
			HPanicking += 1+rnd(6);
		}
	} else if (TELEPORT_ARM(uarms)) {
		if (donning(otmp)) cancel_don();
		Your("shield vanishes!");
		(void) Shield_off();
		obj_extract_self(otmp);
		dropy(otmp);
		rloco(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after losing your shield!");
			HPanicking += 1+rnd(6);
		}
	} else {
		return 0;		/* could not destroy anything */
	}

#undef DESTROY_ARM
	stop_occupation();
	return(1);
}

int
tent_destroy_arm(atmp)
register struct obj *atmp;
{
	register struct obj *otmp;
#define DESTROY_ARM(o) ((otmp = (o)) != 0 && \
			(!atmp || atmp == otmp) && \
			(!obj_resists(otmp, 0, 90)))

	if(Preservation)
		return 0;
	
	if (DESTROY_ARM(uarmc)) {
		if (donning(otmp)) cancel_don();
		pline("The tentacles tear your cloak to shreds!");
		(void) Cloak_off();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your cloak shredded!");
			HPanicking += 1+rnd(6);
		}
	} else if (DESTROY_ARM(uarm)) {
		if (donning(otmp)) cancel_don();
		pline("The tentacles tear your armor to pieces!");
		(void) Armor_gone();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your armor torn apart!");
			HPanicking += 1+rnd(6);
		}
#ifdef TOURIST
	} else if (DESTROY_ARM(uarmu)) {
		if (donning(otmp)) cancel_don();
		pline("The tentacles tear your underclothes to shreds!");
		(void) Shirt_off();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your underclothes shredded!");
			HPanicking += 1+rnd(6);
		}
#endif
	} else if (DESTROY_ARM(uarmh)) {
		if (donning(otmp)) cancel_don();
		pline("The tentacles break your helmet to pieces!");
		(void) Helmet_off();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your helmet broken!");
			HPanicking += 1+rnd(6);
		}
	} else if (DESTROY_ARM(uarmg)) {
		if (donning(otmp)) cancel_don();
		pline("The tentacles tear apart your gloves!");
		(void) Gloves_off();
		useup(otmp);
		selftouch("You");
		if(roll_madness(MAD_TALONS)){
			You("panic after having your gloves shredded!");
			HPanicking += 1+rnd(6);
		}
	} else if (DESTROY_ARM(uarmf)) {
		if (donning(otmp)) cancel_don();
		pline("The tentacles tear your boots apart!");
		(void) Boots_off();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your boots shredded!");
			HPanicking += 1+rnd(6);
		}
	} else if (DESTROY_ARM(uarms)) {
		if (donning(otmp)) cancel_don();
		pline("The tentacles smash your shield to bits!");
		(void) Shield_off();
		useup(otmp);
		if(roll_madness(MAD_TALONS)){
			You("panic after having your shield smashed!");
			HPanicking += 1+rnd(6);
		}
	} else {
		return 0;		/* could not destroy anything */
	}

#undef DESTROY_ARM
	stop_occupation();
	return(1);
}

void
adj_abon(otmp, delta)
register struct obj *otmp;
register schar delta;
{
	if (uarmh && uarmh == otmp) {
		if(otmp->otyp == HELM_OF_BRILLIANCE){
			if (delta) {
				makeknown(otmp->otyp);
				ABON(A_INT) += (delta);
				ABON(A_WIS) += (delta);
				flags.botl = 1;
			}
		}
		if (otmp->otyp == find_gcirclet()){
			if (delta) {
				ABON(A_CHA) += (delta);
				flags.botl = 1;
			}
		}
	}
	if (uarmc && uarmc == otmp) {
		if(otmp->otyp == SMOKY_VIOLET_FACELESS_ROBE){
			if (delta) {
				ABON(A_CON) += (delta);
				ABON(A_CHA) += (delta);
				flags.botl = 1;
			}
		}
	}
	if(uarm && uarm == otmp){
		if(otmp->otyp == BLACK_DRESS){
			if (delta) {
				ABON(A_CHA) += (delta);
				flags.botl = 1;
			}
		}
		if(otmp->otyp == CONSORT_S_SUIT){
			if (delta) {
				ABON(A_CHA) += (delta);
				flags.botl = 1;
			}
		}
		if(otmp->otyp == NOBLE_S_DRESS || otmp->otyp == GENTLEMAN_S_SUIT){
			if (delta) {
				ABON(A_CHA) += 2*(delta);
				flags.botl = 1;
			}
		}
	}
	if(uarmu && uarmu == otmp){
		if(otmp->otyp == BLACK_DRESS){
			if (delta) {
				ABON(A_CHA) += (delta);
				flags.botl = 1;
			}
		}
		if(otmp->otyp == VICTORIAN_UNDERWEAR){
			if (delta) {
				ABON(A_CHA) += 2*(delta);
				flags.botl = 1;
			}
		}
	}
	if (uarmg && uarmg == otmp) {
		if(otmp->oartifact == ART_GAUNTLETS_OF_THE_BERSERKER){
			if (delta) {
				ABON(A_DEX) += (delta);
				ABON(A_STR) += (delta);
				ABON(A_CON) += (delta);
				ABON(A_INT) -= (delta);
				ABON(A_WIS) -= (delta);
				flags.botl = 1;
			}
		}
		if(otmp->otyp == GAUNTLETS_OF_DEXTERITY){
			if (delta) {
				makeknown(otmp->otyp);
				ABON(A_DEX) += (delta);
			flags.botl = 1;
			}
		}
	}
}

void
dosymbiotic(magr, armor)
struct monst *magr;
struct obj *armor;
{
	struct monst *mdef;
	int clockwisex[8] = { 0, 1, 1, 1, 0,-1,-1,-1};
	int clockwisey[8] = {-1,-1, 0, 1, 1, 1, 0,-1};
	int i = rnd(8),j, lim=0;
	struct attack symbiote = { AT_TENT, AD_PHYS, 3, 3 };
	boolean youagr = (magr == &youmonst);
	boolean youdef;
	
	//2 pips on a die is +1 on average
	if(armor)
		symbiote.damd = max(1, 3 + armor->spe);
	for(j=8;j>=1;j--){
		if(youagr && u.ustuck && u.uswallow)
			mdef = u.ustuck;
		else if(!isok(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]))
			continue;
		else mdef = m_u_at(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]);
		
		if(!mdef)
			continue;

		youdef = (mdef == &youmonst);
		if(!youdef && DEADMONSTER(mdef))
			continue;
		
		if(youagr && (mdef->mpeaceful || !rn2(4)))
			continue;
		if(youdef && (magr->mpeaceful || !rn2(4)))
			continue;
		if(!youagr && !youdef && ((mdef->mpeaceful == magr->mpeaceful) || !rn2(4)))
			continue;
		//Note: the armor avoids touching petrifying things even if you're immune
		if(touch_petrifies(mdef->data)
		 || mdef->mtyp == PM_MEDUSA
		 || mdef->mtyp == PM_PALE_NIGHT
		) continue;
		if (mdef && magr_can_attack_mdef(magr, mdef, x(magr) + clockwisex[(i + j) % 8], y(magr) + clockwisey[(i + j) % 8], FALSE)){
			xmeleehity(magr, mdef, &symbiote, (struct obj **)0, -1, 0, FALSE);
			if(!youagr && DEADMONSTER(magr))
				break; //oops!
			if(youagr) morehungry(1);
			lim++;
			if(lim > 4) break;
		}
	}
}

void
dodragonhead_roar(magr, wep)
struct monst *magr;
struct obj *wep;
{
	struct monst *mdef;
	int clockwisex[8] = { 0, 1, 1, 1, 0,-1,-1,-1};
	int clockwisey[8] = {-1,-1, 0, 1, 1, 1, 0,-1};
	int i = rnd(8),j;
	boolean youagr = (magr == &youmonst);
	boolean youdef;
	
	if(youagr || canseemon(magr))
		pline("The Dragonhead Shield roars!");
	else pline("It roars!");
	
	if(youagr && u.ustuck && u.uswallow){
		mdef = u.ustuck;
		mdef->movement -= 12;
	} else for(j=8;j>=1;j--){
		if(!isok(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]))
			continue;
		else mdef = m_u_at(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]);
		
		if(!mdef)
			continue;
		
		youdef = (mdef == &youmonst);
		if(!youdef && DEADMONSTER(mdef))
			continue;
		
		if(youagr && mdef->mpeaceful)
			continue;
		if(youdef && magr->mpeaceful)
			continue;
		if(!youagr && !youdef && (mdef->mpeaceful == magr->mpeaceful))
			continue;
		
		mdef->movement -= 12;
	}
	if(youagr)
		u.ustdy -= 10;
	else magr->mstdy -= 10;
}

static void
dodragonhead_bite(magr, wep)
struct monst *magr;
struct obj *wep;
{
	struct monst *mdef;
	int clockwisex[8] = { 0, 1, 1, 1, 0,-1,-1,-1};
	int clockwisey[8] = {-1,-1, 0, 1, 1, 1, 0,-1};
	int i = rnd(8),j;
	struct attack symbiote = { AT_HITS, AD_PHYS, 5, 8 };
	boolean youagr = (magr == &youmonst);
	boolean youdef;
	
	for(j=8;j>=1;j--){
		if(youagr && u.ustuck && u.uswallow)
			mdef = u.ustuck;
		else if(!isok(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]))
			continue;
		else mdef = m_u_at(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]);
		
		if(!mdef)
			continue;
		
		youdef = (mdef == &youmonst);
		if(!youdef && DEADMONSTER(mdef))
			continue;
		
		if(youagr && mdef->mpeaceful)
			continue;
		if(youdef && magr->mpeaceful)
			continue;
		if(!youagr && !youdef && (mdef->mpeaceful == magr->mpeaceful))
			continue;
		//Note: petrifying targets are safe, it's a weapon attack
		if(mdef->mtyp == PM_PALE_NIGHT) continue;
		
		if(youdef){
			if(canseemon(magr))
				pline("The Dragonhead Shield bites you!");
			else pline("It bites you!");
		}
		else if(youagr || canseemon(magr))
			pline("The Dragonhead Shield bites %s!", mon_nam(mdef));
		
		if (mdef && magr_can_attack_mdef(magr, mdef, x(magr) + clockwisex[(i + j) % 8], y(magr) + clockwisey[(i + j) % 8], FALSE)){
			xmeleehity(magr, mdef, &symbiote, (struct obj **)0, 0, 0, FALSE);
			if(!youagr && DEADMONSTER(magr))
				break; //oops!
			//limit of one attack for weapons
			break;
		}
	}
}

static void
dodragonhead_breathe(magr, wep)
struct monst *magr;
struct obj *wep;
{
	struct zapdata zapdata;
	int clockwisex[8] = { 0, 1, 1, 1, 0,-1,-1,-1};
	int clockwisey[8] = {-1,-1, 0, 1, 1, 1, 0,-1};
	int i = rnd(8),j,k;
	boolean gooddir = FALSE;
	boolean youagr = (magr == &youmonst);
	boolean youdef;
	struct monst *mdef;
	int curx, cury;
	
	for(j=8;j>=1;j--){
		if(youagr && u.ustuck && u.uswallow){
			mdef = u.ustuck;
			gooddir = TRUE;
		} else for(k = 1; k < 5; k++){
			curx = x(magr)+k*clockwisex[(i+j)%8];
			cury = y(magr)+k*clockwisey[(i+j)%8];
			if(!isok(curx, cury))
				continue;
			else mdef = m_u_at(curx, cury);
			
			if(!mdef)
				continue;
			
			youdef = (mdef == &youmonst);
			if(!youdef && DEADMONSTER(mdef))
				continue;
			
			if(youagr && mdef->mpeaceful){
				gooddir = FALSE;
				break; //break out of inner loop now, we found a bad target.
			}
			if(youdef && magr->mpeaceful){
				gooddir = FALSE;
				break; //break out of inner loop now, we found a bad target.
			}
			if(!youagr && !youdef && (mdef->mpeaceful == magr->mpeaceful)){
				gooddir = FALSE;
				break; //break out of inner loop now, we found a bad target.
			}
			//else found a good target over here, and haven't hit a bad one (yet)
			gooddir = TRUE;
		}
		//found good direction, break out of outer loop
		if(gooddir)
			break;
	}
	if(!j)
		return;
	
	if(youagr || canseemon(magr))
		pline("The Dragonhead Shield breathes fire!");
	
	/* set up zapdata */
	basiczap(&zapdata, AD_FIRE, ZAP_BREATH, 0);

	/* dragonbreath */
	zapdata.unreflectable = ZAP_REFL_ADVANCED;
	/* set damage */
	zapdata.damn = 10;
	zapdata.damd = 10;
	zapdata.no_bounce = TRUE;

	zap(magr, x(magr), y(magr), clockwisex[(i+j)%8], clockwisey[(i+j)%8], 4, &zapdata);
}

void
doliving_dragonhead(magr, wep, invoked)
struct monst *magr;
struct obj *wep;
boolean invoked;
{
	if(!invoked){
		if(rn2(20))
			return;
		if(!nearby_targets(magr))
			return;
	}
	switch(rnd(3)){
		//roar
		case 1:
			dodragonhead_roar(magr, wep);
		break;
		//bite
		case 2:
			dodragonhead_bite(magr, wep);
		break;
		//breathe fire
		case 3:
			dodragonhead_breathe(magr, wep);
		break;
	}
}

void
doking_scream(magr, wep)
struct monst *magr;
struct obj *wep;
{
	struct monst *mdef;
	int clockwisex[8] = { 0, 1, 1, 1, 0,-1,-1,-1};
	int clockwisey[8] = {-1,-1, 0, 1, 1, 1, 0,-1};
	int i = rnd(8),j;
	boolean youagr = (magr == &youmonst);
	boolean youdef;
	
	if(youagr || canseemon(magr))
		pline("The Mad King screams!");
	else pline("It screams!");
	
	if(youagr && u.ustuck && u.uswallow){
		mdef = u.ustuck;
		mdef->movement -= 12;
	} else for(j=8;j>=1;j--){
		if(!isok(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]))
			continue;
		else mdef = m_u_at(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]);
		
		if(!mdef)
			continue;
		
		youdef = (mdef == &youmonst);
		if(!youdef && DEADMONSTER(mdef))
			continue;
		
		if(youagr && mdef->mpeaceful)
			continue;
		if(youdef && magr->mpeaceful)
			continue;
		if(!youagr && !youdef && (mdef->mpeaceful == magr->mpeaceful))
			continue;
		
		if(youdef){
			mdef->movement -= 12;
			if(!bigmonst(youracedata)){
				hurtle(clockwisex[(i+j)%8], clockwisey[(i+j)%8], 1, TRUE, FALSE);
			}
		} else {
			// if (!resist(mdef, WEAPON_CLASS, 0, NOTELL))
				// monflee(mdef, 3, FALSE, FALSE);
			if(bigmonst(mdef->data)){
				mdef->movement -= 12;
			} else {
				mhurtle(mdef, clockwisex[(i+j)%8], clockwisey[(i+j)%8], 1);
				//Note: mdef may be gone now due to traps
			}
		}
	}
}

void
doking_bless(magr, wep)
struct monst *magr;
struct obj *wep;
{
	boolean youagr = (magr == &youmonst);
	
	if(youagr)
		pline("The Mad King blesses you!");
	else if(canseemon(magr))
		pline("The Mad King blesses %s!", mon_nam(magr));
	
	if(youagr){
		u.ustdy -= 8;
		u.uencouraged += 8;
	} else {
		magr->mstdy -= 8;
		magr->encouraged += 8;
	}
}

void
doking_vex(magr, wep)
struct monst *magr;
struct obj *wep;
{
	boolean youagr = (magr == &youmonst);
	struct monst *mtmp;
	int maketame = ((magr->mtame || youagr) ? MM_EDOG : 0);
	
	if(youagr)
		pline("The Mad King curses you!");
	else if(canseemon(magr))
		pline("The Mad King curses %s!", mon_nam(magr));
	
	mtmp = makemon(&mons[PM_VEXING_ORB], x(magr), y(magr), MM_ADJACENTOK|maketame|MM_ESUM);
	if (mtmp) {
		/* time out, but not tied to magr -- the object summoned them */
		mark_mon_as_summoned(mtmp, (struct monst *)0, 4 + rn2(4), 0);
		/* can be peaceful */
		if(magr->mpeaceful)
			mtmp->mpeaceful = TRUE;
		/* can be tame */
		if (maketame) {
			initedog(mtmp);
		}
	}
}

void
doliving_mad_king(magr, wep, invoked)
struct monst *magr;
struct obj *wep;
boolean invoked;
{
	if(!invoked){
		if(rn2(20))
			return;
		if(!nearby_targets(magr))
			return;
	}
	switch(rnd(3)){
		//scream
		case 1:
			doking_scream(magr, wep);
		break;
		//bless
		case 2:
			doking_bless(magr, wep);
		break;
		//vexation
		case 3:
			doking_vex(magr, wep);
		break;
	}
}

void
doringed_awaken(magr, wep)
struct monst *magr;
struct obj *wep;
{
	boolean youagr = (magr == &youmonst);
	struct monst *mtmp;
	
	//Don't stack multiple
	if(artinstance[wep->oartifact].RRSember >= moves || artinstance[wep->oartifact].RRSlunar >= moves)
		return;
	
	if(rn2(2)){
		if(youagr || canseemon(magr))
			pline("The Ringed Spear awakens and begins to burn!");
		artinstance[wep->oartifact].RRSember = moves + 20 + rn2(10) + rn2(10);
	} else {
		if(youagr || canseemon(magr))
			pline("The Ringed Spear awakens and begins to gleam royal blue!");
		artinstance[wep->oartifact].RRSlunar = moves + 20 + rn2(10) + rn2(10);
	}
}

void
doringed_nurture(magr, wep)
struct monst *magr;
struct obj *wep;
{
	boolean youagr = (magr == &youmonst);
	struct monst *mtmp;
	
	if(rn2(2)){
		if(youagr)
			pline("The Ringed Spear nurtures your body!");
		else if(canseemon(magr))
			pline("The Ringed Spear nurtures %s body!", s_suffix(mon_nam(magr)));
		
		if(youagr){
			if(uhp() < uhpmax()){
				healup(min_ints(100, uhpmax()/3+1), 0, FALSE, FALSE);
			}
		} else {
			magr->mhp += min(magr->mhpmax/3+1, 100);
			if(magr->mhp > magr->mhpmax)
				magr->mhp = magr->mhpmax;
		}
	} else {
		if(youagr)
			pline("The Ringed Spear nurtures your spirit!");
		else if(canseemon(magr))
			pline("The Ringed Spear nurtures %s spirit!", s_suffix(mon_nam(magr)));
		
		if(youagr){
			u.uen += min(u.uenmax/3+1, 100);
			if(u.uen > u.uenmax)
				u.uen = u.uenmax;
		} else {
			magr->mspec_used = 0;
		}
	}
}

void
doringed_companions(magr, wep)
struct monst *magr;
struct obj *wep;
{
	boolean youagr = (magr == &youmonst);
	struct monst *mtmp;
	int maketame = ((magr->mtame || youagr) ? MM_EDOG : 0);
	int mid;
	int i;
	
	if(rn2(2)){
		if(youagr || canseemon(magr))
			pline("The Ringed Spear emits a seething orb of flame!");
		mid = PM_FLAMING_ORB;
		i = 1;
	} else {
		if(youagr || canseemon(magr))
			pline("The Ringed Spear emits star-eyed pursuers!");
		mid = PM_PURSUER;
		i = 5;
	}
	
	for(; i > 0; i--){
		mtmp = makemon(&mons[mid], x(magr), y(magr), MM_ADJACENTOK|maketame|MM_ESUM);
		if (mtmp) {
			/* time out, but not tied to magr -- the object summoned them */
			mark_mon_as_summoned(mtmp, (struct monst *)0, 4 + rn2(4), 0);
			mtmp->mspec_used = 0;
			/* can be peaceful */
			if(magr->mpeaceful)
				mtmp->mpeaceful = TRUE;
			/* can be tame */
			if (maketame) {
				initedog(mtmp);
			}
		}
	}
}

void
doliving_ringed_spear(magr, wep, invoked)
struct monst *magr;
struct obj *wep;
boolean invoked;
{
	if(!invoked){
		if(rn2(20))
			return;
		if(!nearby_targets(magr))
			return;
	}
	switch(rnd(3)){
		//awaken
		case 1:
			doringed_awaken(magr, wep);
		break;
		//nurture
		case 2:
			doringed_nurture(magr, wep);
		break;
		//companions
		case 3:
			doringed_companions(magr, wep);
		break;
	}
}

void
dobrass_awaken(magr, wep)
struct monst *magr;
struct obj *wep;
{
	boolean youagr = (magr == &youmonst);
	struct monst *mtmp;
	
	//Don't stack multiple
	if(artinstance[wep->oartifact].RRSember >= moves || artinstance[wep->oartifact].RRSlunar >= moves)
		return;
	
	if(rn2(2)){
		if(youagr || canseemon(magr))
			pline("The ringed armor's black tentacles awaken and begin to dance like fire!");
		artinstance[wep->oartifact].RRSember = moves + 4 + rn2(4) + rn2(4);
	} else {
		if(youagr || canseemon(magr))
			pline("The ringed armor's tattered skirt transforms into gleaming royal blue serpents!");
		artinstance[wep->oartifact].RRSlunar = moves + 4 + rn2(4) + rn2(4);
	}
}

void
dobrass_attack(magr, armor, atyp, etyp)
struct monst *magr;
struct obj *armor;
char atyp;
char etyp;
{
	struct monst *mdef;
	int clockwisex[8] = { 0, 1, 1, 1, 0,-1,-1,-1};
	int clockwisey[8] = {-1,-1, 0, 1, 1, 1, 0,-1};
	int i = rnd(8),j, lim=0;
	int striking = 0;
	struct attack symbiote = { atyp, etyp, 4, 2 };
	boolean youagr = (magr == &youmonst);
	boolean youdef;
	
	//2 pips on a die is +1 on average
	if(armor)
		symbiote.damd = max(1, 2 + armor->spe);
	for(j=8;j>=1;j--){
		if(youagr && u.ustuck && u.uswallow)
			mdef = u.ustuck;
		else if(!isok(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]))
			continue;
		else mdef = m_u_at(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]);
		
		if(!mdef)
			continue;

		youdef = (mdef == &youmonst);
		if(!youdef && DEADMONSTER(mdef))
			continue;
		
		if(youagr && (mdef->mpeaceful || !rn2(4)))
			continue;
		if(youdef && (magr->mpeaceful || !rn2(4)))
			continue;
		if(!youagr && !youdef && ((mdef->mpeaceful == magr->mpeaceful) || !rn2(4)))
			continue;
		//Note: the armor avoids touching petrifying things even if you're immune
		if(touch_petrifies(mdef->data)
		 || mdef->mtyp == PM_MEDUSA
		 || mdef->mtyp == PM_PALE_NIGHT
		) continue;
		if (mdef && magr_can_attack_mdef(magr, mdef, x(magr) + clockwisex[(i + j) % 8], y(magr) + clockwisey[(i + j) % 8], FALSE)){
			//More than one limb strikes at the same target.
			symbiote.damn = rnd(3) + rnd(3);
			symbiote.damn = min(symbiote.damn, 8-lim);
			
			xmeleehity(magr, mdef, &symbiote, (struct obj **)0, -1, 0, FALSE);
			if(!youagr && DEADMONSTER(magr))
				break; //oops!
			lim+=symbiote.damn;
			
			if(lim >= 8) break;
			//If few enemies are nearby, it may concentrate extra attacks on them.
			j = 9;
			i = rnd(8);
		}
	}
}

void
dobrass_nurture(magr, wep)
struct monst *magr;
struct obj *wep;
{
	boolean youagr = (magr == &youmonst);
	struct monst *mtmp;
	
	if(rn2(2)){
		if(youagr){
			if(uhp() < uhpmax()){
				pline("The ring of fire flares, nurturing your body!");
				healup(min_ints(100, uhpmax()/3+1), 0, FALSE, FALSE);
			}
		} else {
			if(magr->mhp < magr->mhpmax){
				if(canseemon(magr))
					pline("The ring of fire flares, nurturing %s body!", s_suffix(mon_nam(magr)));
				magr->mhp += min(magr->mhpmax/3+1, 100);
				if(magr->mhp > magr->mhpmax)
					magr->mhp = magr->mhpmax;
			}
		}
	} else {
		if(youagr){
			if(u.uen < u.uenmax){
				pline("The depths of the dark gleam, nurturing your spirit!");
				u.uen += min(u.uenmax/3+1, 100);
				if(u.uen > u.uenmax)
					u.uen = u.uenmax;
			}
		} else {
			if(magr->mspec_used || magr->mcan){
				if(canseemon(magr))
					pline("The depths of the dark gleam, nurturing %s spirit!", s_suffix(mon_nam(magr)));
				magr->mspec_used = 0;
				magr->mcan = 0;
			}
		}
	}
}

void
dobrass_companions(magr, wep)
struct monst *magr;
struct obj *wep;
{
	boolean youagr = (magr == &youmonst);
	struct monst *mtmp;
	int maketame = ((magr->mtame || youagr) ? MM_EDOG : 0);
	int mid;
	int i;
		
	if(rn2(2)){
		if(youagr || canseemon(magr))
			pline("The ring of fire emits a seething orb of flame!");
		mid = PM_FLAMING_ORB;
		i = 1;
	} else {
		if(youagr || canseemon(magr))
			pline("Star-eyed pursuers emerge from the darksign!");
		mid = PM_PURSUER;
		i = 5;
	}
	
	for(; i > 0; i--){
		mtmp = makemon(&mons[mid], x(magr), y(magr), MM_ADJACENTOK|maketame|MM_ESUM);
		if (mtmp) {
			/* time out, but not tied to magr -- the object summoned them */
			mark_mon_as_summoned(mtmp, (struct monst *)0, 4 + rn2(4), 0);
			mtmp->mspec_used = 0;
			/* can be peaceful */
			if(magr->mpeaceful)
				mtmp->mpeaceful = TRUE;
			/* can be tame */
			if (maketame) {
				initedog(mtmp);
			}
		}
	}
}

void
doliving_ringed_armor(magr, wep, invoked)
struct monst *magr;
struct obj *wep;
boolean invoked;
{
	if((invoked
		|| !rn2(20)
	) && (
		nearby_targets(magr)
		|| (magr == &youmonst && (uhp() < uhpmax() || u.uen < u.uenmax))
		|| (magr != &youmonst && (magr->mhp < magr->mhpmax || magr->mspec_used || magr->mcan))
	)){
		switch(rnd(3)){
			//awaken
			case 1:
				if(nearby_targets(magr) || invoked)
					dobrass_awaken(magr, wep);
			break;
			//nurture
			case 2:
				dobrass_nurture(magr, wep);
			break;
			//companions
			case 3:
				if(nearby_targets(magr) || invoked)
					dobrass_companions(magr, wep);
			break;
		}
	}
	
	if(artinstance[wep->oartifact].RRSember > moves)
		dobrass_attack(magr, wep, AT_TENT, AD_EFIR);
	if(artinstance[wep->oartifact].RRSlunar > moves)
		dobrass_attack(magr, wep, AT_BITE, AD_MAGM);
}

static void
doliving_single_attack(magr, wep)
struct monst *magr;
struct obj *wep;
{
	struct monst *mdef;
	int clockwisex[8] = { 0, 1, 1, 1, 0,-1,-1,-1};
	int clockwisey[8] = {-1,-1, 0, 1, 1, 1, 0,-1};
	int i = rnd(8),j;
	struct attack symbiote = { AT_WEAP, AD_PHYS, 4, 4 };
	boolean youagr = (magr == &youmonst);
	boolean youdef;
	
	for(j=8;j>=1;j--){
		if(youagr && u.ustuck && u.uswallow)
			mdef = u.ustuck;
		else if(!isok(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]))
			continue;
		else mdef = m_u_at(x(magr)+clockwisex[(i+j)%8], y(magr)+clockwisey[(i+j)%8]);
		
		if(!mdef)
			continue;
		
		youdef = (mdef == &youmonst);
		if(!youdef && DEADMONSTER(mdef))
			continue;
		
		if(youagr && (mdef->mpeaceful || !rn2(4)))
			continue;
		if(youdef && (magr->mpeaceful || !rn2(4)))
			continue;
		if(!youagr && !youdef && ((mdef->mpeaceful == magr->mpeaceful) || !rn2(4)))
			continue;
		//Note: petrifying targets are safe, it's a weapon attack
		if(mdef->mtyp == PM_PALE_NIGHT) continue;
		if (mdef && magr_can_attack_mdef(magr, mdef, x(magr) + clockwisex[(i + j) % 8], y(magr) + clockwisey[(i + j) % 8], FALSE)){
			xmeleehity(magr, mdef, &symbiote, &wep, -1, 0, FALSE);
			if(!youagr && DEADMONSTER(magr))
				break; //oops!
			//limit of one attack for weapons
			break;
		}
	}
}

void
doliving(magr, wep)
struct monst *magr;
struct obj *wep;
{
	if(wep->oartifact == ART_DRAGONHEAD_SHIELD)
		doliving_dragonhead(magr, wep, FALSE);
	else if(wep->oartifact == ART_CRUCIFIX_OF_THE_MAD_KING)
		doliving_mad_king(magr, wep, FALSE);
	else if(wep->oartifact == ART_RITUAL_RINGED_SPEAR)
		doliving_ringed_spear(magr, wep, FALSE);
	else if(wep->oartifact == ART_RINGED_BRASS_ARMOR)
		doliving_ringed_armor(magr, wep, FALSE);
	else doliving_single_attack(magr, wep);
}

#endif /* OVLB */

/*do_wear.c*/
