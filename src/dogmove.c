/*	SCCS Id: @(#)dogmove.c	3.4	2002/09/10	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "xhity.h"

#include "mfndpos.h"


extern boolean notonhead;
extern struct obj *propellor;

int FDECL(extra_pref, (struct monst *, struct obj *));

extern boolean FDECL(would_prefer_hwep,(struct monst *,struct obj *));
extern boolean FDECL(would_prefer_rwep,(struct monst *,struct obj *));

#define DOG_SATIATED 3000

#ifdef OVL0

STATIC_DCL boolean FDECL(dog_hunger,(struct monst *,struct edog *));
STATIC_DCL int FDECL(dog_invent,(struct monst *,struct edog *,int));
STATIC_DCL int FDECL(dog_goal,(struct monst *,struct edog *,int,int,int));

STATIC_DCL boolean FDECL(can_reach_location,(struct monst *,XCHAR_P,XCHAR_P,
    XCHAR_P,XCHAR_P));
STATIC_DCL boolean FDECL(could_reach_item,(struct monst *, XCHAR_P,XCHAR_P));

/*
 * See if this armor is better than what we're wearing.
 */
boolean
is_better_armor(mtmp, otmp)
register struct monst *mtmp;
register struct obj *otmp;
{
    register struct obj *obj;
    register struct obj *best = (struct obj *)0;

    if (otmp->oclass != ARMOR_CLASS) return FALSE;

	//Special case: can't wear most torso armor
	if (mtmp->mtyp == PM_HARROWER_OF_ZARIEL
	 && ((is_suit(otmp) && arm_blocks_upper_body(otmp->otyp))
		|| is_shirt(otmp)
	)){
		return FALSE;
	}
		
    if (is_suit(otmp) && (!arm_match(mtmp->data, otmp) || !arm_size_fits(mtmp->data, otmp)))
		return FALSE;
    
    if (is_shirt(otmp) && (otmp->objsize != mtmp->data->msize || !shirt_match(mtmp->data,otmp) || (mtmp->misc_worn_check & W_ARM)))
        return FALSE;
    
    if (is_cloak(otmp) && ((abs(otmp->objsize - mtmp->data->msize) > 1)))
        return FALSE;

    if (is_helmet(otmp) && !(helm_match(mtmp->data, otmp) && helm_size_fits(mtmp->data, otmp)))
        return FALSE;
    
    if (is_shield(otmp) && (
			((mtmp == &youmonst) ? (uwep && bimanual(uwep,youracedata)) 
							: (MON_WEP(mtmp) && bimanual(MON_WEP(mtmp),mtmp->data)))
			|| mon_offhand_attack(mtmp)
		)
	)
		return FALSE;
    
    if (is_gloves(otmp) && (otmp->objsize != mtmp->data->msize || !can_wear_gloves(mtmp->data))) return FALSE;
    
    if (is_boots(otmp) &&
        (!boots_size_fits(mtmp->data, otmp) || !can_wear_boots(mtmp->data)))
	return FALSE;
    
    if (is_helmet(otmp) &&
        !is_flimsy(otmp) &&
        otmp->otyp != find_gcirclet() &&
	num_horns(mtmp->data) > 0)
	return FALSE;

    obj = (mtmp == &youmonst) ? invent : mtmp->minvent;

    for(; obj; obj = obj->nobj)
    {
        if (is_cloak(otmp)  && !is_cloak(obj) ) continue;
        if (is_suit(otmp)   && !is_suit(obj)  ) continue;
#ifdef TOURIST
        if (is_shirt(otmp)  && !is_shirt(obj) ) continue;
#endif
	if (is_boots(otmp)  && !is_boots(obj) ) continue;
	if (is_shield(otmp) && !is_shield(obj)) continue;
	if (is_helmet(otmp) && !is_helmet(obj)) continue;
	if (is_gloves(otmp) && !is_gloves(obj)) continue;

	if (!obj->owornmask) continue;

	if (best &&
	     (arm_total_bonus(obj) +  extra_pref(mtmp,obj) >=
	      arm_total_bonus(best) + extra_pref(mtmp,best)))
	     best = obj;
    }
    
    return ((best == (struct obj *)0) ||
	    (arm_total_bonus(otmp) + extra_pref(mtmp,otmp) >
	     arm_total_bonus(best) + extra_pref(mtmp,best)));
}

/*
 * See if a monst could use this item in an offensive or defensive capacity.
 */
boolean
could_use_item(mtmp, otmp, check_if_better)
register struct monst *mtmp;
register struct obj *otmp;
boolean check_if_better;
{
    boolean can_use;
    if(mindless_mon(mtmp) && otmp && otmp->where == OBJ_MINVENT)
		can_use = TRUE;
	else can_use =
            /* make sure this is an intelligent monster */
            (mtmp && !is_animal(mtmp->data) && !mindless_mon(mtmp) && 
	      !nohands(mtmp->data) &&
	     otmp &&
	    /* food */
            ((dogfood(mtmp, otmp) < APPORT) ||
	    /* collect artifacts and oprop items */
		 (otmp->oartifact
			|| !check_oprop(otmp, OPROP_NONE)
			|| (rakuyo_prop(otmp) && u.uinsight >= 20)
			|| (is_mercy_blade(otmp) && !u.veil)
			|| (otmp->otyp == ISAMUSEI && u.uinsight >= 22)
			|| (otmp->otyp == DISKOS && u.uinsight >= 10)
		 ) ||
	    /* slotless non-artifact items */
		 ((otmp->otyp == ARMOR_SALVE && u.uinsight >= 66) || otmp->otyp == PRESERVATIVE_ENGINE) ||
	    /* chains for some */
		 ((mtmp->mtyp == PM_CATHEZAR) && otmp->otyp == CHAIN) ||
	    /* better weapons */
	     (is_armed_mon(mtmp) &&
	      (otmp->oclass == WEAPON_CLASS || is_weptool(otmp)) && 
		   (!check_if_better ||
		    mtmp->mtyp == PM_MARILITH ||
		    would_prefer_hwep(mtmp, otmp) ||
		    would_prefer_rwep(mtmp, otmp))) ||
	    /* useful masks */
	     (otmp->otyp == MASK && otmp->corpsenm != NON_PM && mtmp->mtyp == PM_LILLEND) ||
	     (is_worn_tool(otmp) && can_wear_blindf(mtmp->data)) ||
	    /* better armor */
	     (otmp->oclass == ARMOR_CLASS &&
	      (!check_if_better || is_better_armor(mtmp, otmp))) ||
	    /* useful amulets */
	     is_museable_amulet(otmp->otyp) ||
	    /* misc magic items that muse can use */
	     otmp->otyp == SCR_TELEPORTATION ||
             otmp->otyp == SCR_EARTH ||
             otmp->otyp == SCR_REMOVE_CURSE ||
	     otmp->otyp == WAN_DEATH ||
	     otmp->otyp == WAN_DIGGING ||
	     otmp->otyp == WAN_FIRE ||
	     otmp->otyp == WAN_COLD ||
	     otmp->otyp == WAN_LIGHTNING ||
	     otmp->otyp == WAN_MAGIC_MISSILE ||
	     otmp->otyp == WAN_STRIKING ||
	     otmp->otyp == WAN_TELEPORTATION ||
	     otmp->otyp == POT_HEALING ||
	     otmp->otyp == POT_EXTRA_HEALING ||
	     otmp->otyp == POT_FULL_HEALING ||
	     otmp->otyp == POT_PARALYSIS ||
	     otmp->otyp == POT_BLINDNESS ||
	     otmp->otyp == POT_CONFUSION ||
	     otmp->otyp == POT_AMNESIA ||
	     otmp->otyp == POT_ACID ||
	     otmp->otyp == FROST_HORN ||
	     otmp->otyp == FIRE_HORN ||
	     otmp->otyp == UNICORN_HORN));

    if (can_use){
        /* arbitrary - greedy monsters keep any item you can use */
        if (likes_gold(mtmp->data)) return TRUE;
		
        /* mindless monsters hold onto anything they are already carrying */
		if(mindless_mon(mtmp))
			return TRUE;

        if (otmp->oclass == ARMOR_CLASS){
			return !check_if_better || !is_better_armor(&youmonst, otmp);
		}
		else if (otmp->oclass == WAND_CLASS && otmp->spe <= 0)
            return FALSE;  /* used charges or was cancelled? */
		else {
			/*Hold all useful items.  The player can take with #loot if needed*/
			return TRUE;
		}
    }

    return FALSE;
}

struct obj *
DROPPABLES(mon)
register struct monst *mon;
{
	register struct obj *obj;
	struct obj *wep  = MON_WEP(mon),
                   *hwep = mon_attacktype(mon, AT_WEAP)
		           ? select_hwep(mon) : (struct obj *)0,
		   *proj = mon_attacktype(mon, AT_WEAP)
		           ? select_rwep(mon) : (struct obj *)0,
		   *rwep;
	boolean item1 = FALSE, item2 = FALSE;
	boolean intelligent = TRUE;
	boolean marilith = mon_attacktype(mon, AT_MARI);

	if(on_level(&valley_level, &u.uz))
		return (struct obj *)0; //The Dead hold on to their possessions (prevents the "drop whole inventory" bug
	
	if(is_eeladrin(mon->data))
		return (struct obj *)0; //Eladrin don't drop objects in their energy form.
	
	rwep = mon_attacktype(mon, AT_WEAP) ? propellor : &zeroobj;

	if (is_animal(mon->data) || mindless_mon(mon)) {
		intelligent = FALSE;
		item1 = item2 = TRUE;
	}
	if (!tunnels(mon->data) || !needspick(mon->data))
		item1 = TRUE;
	for(obj = mon->minvent; obj; obj = obj->nobj) {
		if (!item1 && is_pick(obj) && (obj->otyp != DWARVISH_MATTOCK
						|| !which_armor(mon, W_ARMS))) {
			item1 = TRUE;
			continue;
		}
		if (!item2 && obj->otyp == UNICORN_HORN && !obj->cursed) {
			item2 = TRUE;
			continue;
		}
		if(marilith && (obj->oclass == WEAPON_CLASS || is_weptool(obj)) && objects[obj->otyp].oc_skill > 0){
			continue; //Keep all weapons
		}
		if (!obj->owornmask && obj != wep &&
		    (!intelligent ||
		    (obj != rwep
		    && obj != proj && obj != hwep
		    && !would_prefer_hwep(mon, obj) /*cursed item in hand?*/
		    && !would_prefer_rwep(mon, obj)
		    && ((rwep != &zeroobj) ||
		        (!is_ammo(obj) && !is_launcher(obj)))
		    && (rwep == &zeroobj || !ammo_and_launcher(obj, rwep))
		    && !could_use_item(mon, obj, TRUE))))
		    return obj;
	}
	return (struct obj *)0;
}

struct obj *
drop_envy(mon)
register struct monst *mon;
{
	register struct obj *obj;
	if(MON_WEP(mon))
		return MON_WEP(mon);

	if(MON_SWEP(mon))
		return MON_SWEP(mon);

	for(obj = mon->minvent; obj; obj = obj->nobj) {
		if(!obj->owornmask)
		    return obj;
	}

	return (struct obj *)0;
}

static NEARDATA const char nofetch[] = { BALL_CLASS, CHAIN_CLASS, ROCK_CLASS, BED_CLASS, 0 };

#endif /* OVL0 */

STATIC_OVL boolean FDECL(cursed_object_at, (int, int));

STATIC_VAR xchar gtyp, gx, gy;	/* type and position of dog's current goal */

STATIC_PTR void FDECL(wantdoor, (int, int, genericptr_t));

#ifdef OVLB
STATIC_OVL boolean
cursed_object_at(x, y)
int x, y;
{
	struct obj *otmp;

	for(otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere)
		if (otmp->cursed) return TRUE;
	return FALSE;
}

int
dog_nutrition(mtmp, obj)
struct monst *mtmp;
struct obj *obj;
{
	int nutrit;

	/*
	 * It is arbitrary that the pet takes the same length of time to eat
	 * as a human, but gets more nutritional value.
	 */
	if (obj->oclass == FOOD_CLASS) {
	    if(obj->otyp == CORPSE) {
			mtmp->meating = 3 + (mons[obj->corpsenm].cwt >> 6);
			nutrit = mons[obj->corpsenm].cnutrit;
			if(mtmp->mtyp == PM_DRACAE_ELADRIN && HAS_ESMT(mtmp))
				ESMT(mtmp)->smith_biomass_stockpile += nutrit;
	    } else {
			mtmp->meating = objects[obj->otyp].oc_delay;
			nutrit = objects[obj->otyp].oc_nutrition;
			if(mtmp->mtyp == PM_DRACAE_ELADRIN && HAS_ESMT(mtmp))
				ESMT(mtmp)->smith_biomass_stockpile += nutrit;
	    }
	    switch(mtmp->data->msize) {
		case MZ_TINY: nutrit *= 8; break;
		case MZ_SMALL: nutrit *= 6; break;
		default:
		case MZ_MEDIUM: nutrit *= 5; break;
		case MZ_LARGE: nutrit *= 4; break;
		case MZ_HUGE: nutrit *= 3; break;
		case MZ_GIGANTIC: nutrit *= 2; break;
	    }
	    if(obj->oeaten) {
		mtmp->meating = eaten_stat(mtmp->meating, obj);
		nutrit = eaten_stat(nutrit, obj);
	    }
	} else if (obj->oclass == COIN_CLASS) {
	    mtmp->meating = (int)(obj->quan/2000) + 1;
	    if (mtmp->meating < 0) mtmp->meating = 1;
	    nutrit = (int)(obj->quan/20);
	    if (nutrit < 0) nutrit = 0;
	} else if (obj->otyp == POT_BLOOD) {
		/* 1/5th multiplier applied in dog_eat */
		nutrit = ((obj->odiluted ? 1 : 2) *
					(obj->blessed ? mons[(obj)->corpsenm].cnutrit*3/2 : mons[(obj)->corpsenm].cnutrit ));
	} else {
	    /* Unusual pet such as gelatinous cube eating odd stuff.
	     * meating made consistent with wild monsters in mon.c.
	     * nutrit made consistent with polymorphed player nutrit in
	     * eat.c.  (This also applies to pets eating gold.)
	     */
	    mtmp->meating = obj->owt/20 + 1;
	    nutrit = 5*objects[obj->otyp].oc_nutrition;
	}
	return nutrit;
}

/* returns 2 if pet dies, otherwise 1 */
int
dog_eat(mtmp, obj, x, y, devour)
register struct monst *mtmp;
register struct obj * obj;
int x, y;
boolean devour;
{
	register struct edog *edog = EDOG(mtmp);
	boolean poly = FALSE, grow = FALSE, heal = FALSE, ston = FALSE,
		tainted = FALSE, acidic = FALSE, freeze = FALSE, burning = FALSE, poison = FALSE;
	int nutrit;
	boolean vampiric = is_vampire(mtmp->data);
	boolean eatonlyone = (obj->oclass == FOOD_CLASS || obj->oclass == CHAIN_CLASS || obj->oclass == POTION_CLASS);
	int mtyp = NON_PM;

	// boolean can_choke = (edog->hungrytime >= monstermoves + DOG_SATIATED && !vampiric);
	boolean can_choke = mtmp->mgluttony && !Breathless_res(mtmp);

	if(!can_choke && edog->hungrytime < monstermoves)
	    edog->hungrytime = monstermoves;

	nutrit = dog_nutrition(mtmp, obj);
	long rotted = 0;
	poly = polyfodder(obj) && !resists_poly(mtmp->data);
	grow = mlevelgain(obj);
	heal = mhealup(obj);
	ston = (obj->otyp == CORPSE || obj->otyp == EGG || obj->otyp == TIN || obj->otyp == POT_BLOOD) && obj->corpsenm >= LOW_PM && touch_petrifies(&mons[obj->corpsenm]) && !Stone_res(mtmp);
	
	if(obj->otyp == CORPSE){
		mtyp = obj->corpsenm;
		if (mtyp != PM_LIZARD && mtyp != PM_SMALL_CAVE_LIZARD && mtyp != PM_CAVE_LIZARD 
			&& mtyp != PM_LARGE_CAVE_LIZARD && mtyp != PM_LICHEN && mtyp != PM_BEHOLDER
		) {
			long age = peek_at_iced_corpse_age(obj);

			rotted = (monstermoves - age)/(10L + rn2(20));
			if (obj->cursed) rotted += 2L;
			else if (obj->blessed) rotted -= 2L;
		}

		if(mtyp != PM_ACID_BLOB && !ston && rotted > 5L)
			tainted = TRUE;
		else if(acidic(&mons[mtyp]) && !Acid_res(mtmp))
			acidic = TRUE;
		if(freezing(&mons[mtyp]) && !Cold_res(mtmp))
			freeze = TRUE;
		if(burning(&mons[mtyp]) && !Fire_res(mtmp))
			burning = TRUE;
		if(poisonous(&mons[mtyp]) && !Poison_res(mtmp))
			poison = TRUE;
	}

	if (devour) {
	    if (mtmp->meating > 1) mtmp->meating /= 2;
	    if (nutrit > 1) nutrit = (nutrit * 3) / 4;
	}
	/* vampires only get 1/5 normal nutrition */
	if (vampiric) {
	    mtmp->meating = (mtmp->meating + 4) / 5;
	    nutrit = (nutrit + 4) / 5;
	}
	edog->hungrytime += nutrit;
	if(u.sealsActive&SEAL_MALPHAS && mtmp->mtyp == PM_CROW && obj->otyp == CORPSE){
		more_experienced(ptrexperience(&mons[obj->corpsenm]),0);
		newexplevel();
	}
	mtmp->mconf = 0;
	if (edog->mhpmax_penalty) {
	    /* no longer starving */
	    mtmp->mhpmax += edog->mhpmax_penalty;
	    edog->mhpmax_penalty = 0;
	}
	if (mtmp->mflee && mtmp->mfleetim > 1) mtmp->mfleetim /= 2;
	if (mtmp->mtame < 20) mtmp->mtame++;
	if (x != mtmp->mx || y != mtmp->my) {	/* moved & ate on same turn */
	    newsym(x, y);
	    newsym(mtmp->mx, mtmp->my);
	}
	if (is_pool(x, y, FALSE) && !Underwater) {
	    /* Don't print obj */
	    /* TODO: Reveal presence of sea monster (especially sharks) */
	} else
	/* hack: observe the action if either new or old location is in view */
	/* However, invisible monsters should still be "it" even though out of
	   sight locations should not. */
	if (cansee(x, y) || cansee(mtmp->mx, mtmp->my))
	    pline("%s %s %s.", mon_visible(mtmp) ? noit_Monnam(mtmp) : "It",
		  obj->oclass == POTION_CLASS ? "drinks" : vampiric ? "drains" : devour ? "devours" : "eats",
		  eatonlyone ? singular(obj, doname) : doname(obj));
	/* It's a reward if it's DOGFOOD and the player dropped/threw it. */
	/* We know the player had it if invlet is set -dlc */
	if(dogfood(mtmp,obj) == DOGFOOD && obj->invlet)
#ifdef LINT
	    edog->apport = 0;
#else
	    edog->apport += (int)(200L/
		((long)edog->dropdist + monstermoves - edog->droptime));
#endif
	if (mtmp->mtyp == PM_RUST_MONSTER && obj->oerodeproof) {
	    /* The object's rustproofing is gone now */
	    obj->oerodeproof = 0;
	    mtmp->mstun = 1;
	    if (canseemon(mtmp) && flags.verbose) {
		pline("%s spits %s out in disgust!",
		      Monnam(mtmp), distant_name(obj,doname));
	    }
		can_choke = FALSE;
		nutrit = 0;
	} else if (vampiric && !(obj->otyp == POT_BLOOD)) {
		/* Split Object */
		if (obj->quan > 1L) {
		    if(!carried(obj)) {
			(void) splitobj(obj, 1L);
		    } else {
		    	/* Carried */
			obj = splitobj(obj, obj->quan - 1L);
			
			freeinv(obj);
			if (inv_cnt() >= 52 && !merge_choice(invent, obj))
			    dropy(obj);
			else
			    obj = addinv(obj); /* unlikely but a merge is possible */			
		    }
#ifdef DEBUG
		    debugpline("split object,");
#endif
		}
		
		/* Take away blood nutrition */
	    	obj->oeaten = drainlevel(obj);
		obj->odrained = 1;
		can_choke = FALSE;
	} else {
		/*These cases destroy the object, rescue its contents*/
		struct obj *obj2;
		while((obj2 = obj->cobj)){
			obj_extract_self(obj2);
			/* Compartmentalize tip() */
			place_object(obj2, mtmp->mx, mtmp->my);
			stackobj(obj2);
		}

		if (obj == uball) {
			unpunish();
			delobj(obj);
		} else if (obj == uchain)
			unpunish();
		else if (obj->quan > 1L && eatonlyone) {
			obj->quan--;
			obj->owt = weight(obj);
		} else
			delobj(obj);
	}

	if (can_choke && edog->hungrytime >= (monstermoves + 5*DOG_SATIATED))
	{
	    if (canseemon(mtmp))
	    {
	        pline("%s chokes over %s food!", Monnam(mtmp), mhis(mtmp));
			pline("%s dies!", Monnam(mtmp));
	    } else {
	        You("have a very sad feeling for a moment, then it passes.");
	    }
		mondied(mtmp);
	    if (mtmp->mhp <= 0)
			return 2;
	}

	if (ston) {
		xstoney((struct monst *)0, mtmp);
	    if (mtmp->mhp <= 0)
			return 2;
	}
	if(tainted){
		int dmg = d(3, 12);
		if(!rn2(10))
			dmg += 100;
		if(m_losehp(mtmp, dmg, FALSE, "tainted corpse"))
			return 2;
	}
	if(acidic){
		// if(canspotmon(mtmp))
			// pline()
		if(m_losehp(mtmp, rnd(15), FALSE, "acidic corpse"))
			return 2;
	}
	if(freeze){
		if(m_losehp(mtmp, d(2, 12), FALSE, "frozen corpse"))
			return 2;
	}
	if(burning){
		if(m_losehp(mtmp, rnd(20), FALSE, "burning corpse"))
			return 2;
	}
	if(poison){
		int dmg = d(1, 8);
		if(!rn2(10))
			dmg += 80;
		if(m_losehp(mtmp, dmg, FALSE, "poisonous corpse"))
			return 2;
	}
	if (poly) {
	    (void) newcham(mtmp, NON_PM, FALSE,
			   cansee(mtmp->mx, mtmp->my));
	}
	
	/* limit "instant" growth to prevent potential abuse */
	if (grow && (int) mtmp->m_lev < (int)mtmp->data->mlevel + 15) {
	    if (!grow_up(mtmp, (struct monst *)0)) return 2;
	}
	if (heal) mtmp->mhp = mtmp->mhpmax;
	if(mtyp != NON_PM){
		give_mon_corpse_intrinsic(mtmp, mtyp);
	}
	return 1;
}

#endif /* OVLB */
#ifdef OVL0

/* hunger effects -- returns TRUE on starvation */
STATIC_OVL boolean
dog_hunger(mtmp, edog)
register struct monst *mtmp;
register struct edog *edog;
{
	if (monstermoves+900 > edog->hungrytime && (
		(!carnivorous(mtmp->data) && !herbivorous(mtmp->data)) || 
		(In_quest(&u.uz) && 
			((Is_qtown(&u.uz) && !flags.stag) || 
			 (Is_nemesis(&u.uz) && flags.stag)) &&
		 !(Race_if(PM_DROW) && Role_if(PM_NOBLEMAN) && !flags.initgend)
		)
	)) {
		/* Pets don't get hungery on quest home */
		edog->hungrytime = monstermoves + 1000;
		/* but not too high; it might polymorph */
		return(FALSE);
	}
	if(is_demon(mtmp->data) || is_minion(mtmp->data)){
		if(monstermoves > edog->hungrytime + 750)
			edog->hungrytime = monstermoves + 750;
	}
	if((monstermoves + DOG_SATIATED) > edog->hungrytime){
		if(herbivorous(mtmp->data) && !carnivorous(mtmp->data) && levl[mtmp->mx][mtmp->my].typ == GRASS){
			if(cansee(mtmp->mx, mtmp->my))
				pline("%s eats some grass.", mon_visible(mtmp) ? noit_Monnam(mtmp) : "It");
			edog->hungrytime += 5*objects[FOOD_RATION].oc_nutrition;
		} else if (monstermoves > edog->hungrytime){
			/* We're hungry; check if we're carrying anything we can eat
			   Intelligent pets should be able to carry such food */
			register struct obj *otmp, *obest = (struct obj *)0;
			int best_nutrit = -1; //cur_nutrit = -1,
			int cur_food = APPORT, best_food = APPORT;
			for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
			{
	//	        cur_nutrit = dog_nutrition(mtmp, otmp);
				cur_food = dogfood(mtmp, otmp);
				if (cur_food < best_food) /*&& cur_nutrit > best_nutrit)*/
				{
	//			    best_nutrit = cur_nutrit;
					best_food = cur_food;
					obest = otmp;
				}
			}
			if (obest != (struct obj *)0)
			{
				obj_extract_self(obest);
				place_object(obest, mtmp->mx, mtmp->my);
				if (dog_eat(mtmp, obest, mtmp->mx, mtmp->my, FALSE) == 2)
					return(TRUE);
				return(FALSE);
			}
		}
	}
	if (monstermoves > edog->hungrytime + 500) {
		if (!edog->mhpmax_penalty) {
			/* starving pets are limited in healing */
			int newmhpmax = mtmp->mhpmax / 3;
			mtmp->mconf = 1;
			edog->mhpmax_penalty = mtmp->mhpmax - newmhpmax;
			mtmp->mhpmax = newmhpmax;
			if (mtmp->mhp > mtmp->mhpmax)
				mtmp->mhp = mtmp->mhpmax;
			if (mtmp->mhp < 1) goto dog_died;
			if (cansee(mtmp->mx, mtmp->my))
				pline("%s is confused from hunger.", Monnam(mtmp));
			else if (couldsee(mtmp->mx, mtmp->my))
				beg(mtmp);
			else
				You_feel("worried about %s.", y_monnam(mtmp));
			stop_occupation();
	    } else if (monstermoves > edog->hungrytime + 750 || mtmp->mhp < 1) {
dog_died:
			if (mtmp->mleashed
#ifdef STEED
				&& mtmp != u.usteed
#endif
		    ) Your("leash goes slack.");
			else if (cansee(mtmp->mx, mtmp->my))
				pline("%s starves.", Monnam(mtmp));
			else
				You_feel("%s for a moment.",
			Hallucination ? "bummed" : "sad");
			mondied(mtmp);
			return(TRUE);
	    }
	}
	return(FALSE);
}

void
give_mon_corpse_intrinsic(mon, mtyp)
struct monst *mon;
int mtyp;
{
	struct permonst *ptr = &mons[mtyp];
	 for (int i = 1; i <= LAST_PROP; i++) {
		if (intrinsic_possible(i, ptr)) {
			if(mon_acquired_trinsic(mon, i))
				continue;
			if(ptr->mlevel <= rn2(15))
				continue;
			give_mintrinsic(mon, i);
			if(canspotmon(mon)){
#define Mon(str) pline("%s %s", Monnam(mon), str)
				switch (i) {
					case FIRE_RES:
						Mon(Hallucination ? "be chillin'." :"looks cool.");
					break;
					case SLEEP_RES:
						Mon("looks wide awake.");
					break;
					case COLD_RES:
						Mon("looks warm.");
					break;
					case DISINT_RES:
						Mon(Hallucination ? "has it totally together, man." : "looks very firm.");
					break;
					case SHOCK_RES:	/* shock (electricity) resistance */
						if (Hallucination)
							rn2(2) ? Mon("is grounded in reality.") : pline("%s health is currently amplified!", s_suffix(Monnam(mon)));
						else Mon("looks well grounded.");
					break;
					case ACID_RES:	/* acid resistance */
						if (Hallucination)
							rn2(2) ? Mon("has really gotten back to basics!") : Mon("looks insoluble.");
						else Mon("looks tough.");
					break;
					case POISON_RES:
						Mon("looks healthy.");
					break;
					case DISPLACED:	/* displacement resistance */
						if (Hallucination) pline("%s is quite beside %sself!", Monnam(mon), mhim(mon));
						else pline("%s outline shimmers and shifts.", s_suffix(Monnam(mon)));
					break;
					case TELEPORT:
						Mon(Hallucination ? "looks diffuse." : "looks very jumpy.");
					break;
					case TELEPORT_CONTROL:
						Mon(Hallucination ? "looks centered." : "looks in control.");
					break;
					case TELEPAT:
						if(Hallucination){
							Mon("is in touch with the cosmos.");
							change_uinsight(1);
						}
						//else nothing
					break;
				}
			}
		}
	 }
}

/* do something with object (drop, pick up, eat) at current position
 * returns 1 if object eaten (since that counts as dog's move), 2 if died
 */
STATIC_OVL int
dog_invent(mtmp, edog, udist)
register struct monst *mtmp;
register struct edog *edog;
int udist;
{
	register int omx, omy;
	struct obj *obj;

	boolean droppables = FALSE;

	if (mtmp->msleeping || !mtmp->mcanmove) return(0);

	omx = mtmp->mx;
	omy = mtmp->my;

	/* if we are carrying sth then we drop it (perhaps near @) */
	/* Note: if apport == 1 then our behaviour is independent of udist */
	/* Use udist+1 so steed won't cause divide by zero */
#ifndef GOLDOBJ
	if(DROPPABLES(mtmp) || mtmp->mgold) {
#else
	if(DROPPABLES(mtmp)) {
#endif
	    if (!rn2(udist+1) || !rn2(edog->apport))
		if(rn2(10) < edog->apport){
		    relobj(mtmp, (int)mtmp->minvis, TRUE);
		    if(edog->apport > 1) edog->apport--;
		    edog->dropdist = udist;		/* hpscdi!jon */
		    edog->droptime = monstermoves;
		}
	    droppables = TRUE;
	}

	    if((obj=level.objects[omx][omy]) && !index(nofetch,obj->oclass)
#ifdef MAIL
			&& obj->otyp != SCR_MAIL
#endif
									){
		int edible = dogfood(mtmp, obj);
		
	    if (!droppables && (edible <= CADAVER ||
			/* starving pet is more aggressive about eating */
			(edog->mhpmax_penalty && edible == ACCFOOD)) &&
		    could_reach_item(mtmp, obj->ox, obj->oy))
		{
#ifdef PET_SATIATION
		    /* Don't eat if satiated.  (arbitrary) 
				Non-mindless pets can sense if you are hungry or starving, and will eat less.
			*/
		    if (edog->hungrytime < monstermoves + DOG_SATIATED || 
				(!mindless_mon(mtmp) && 
					((YouHunger < HUNGRY && edog->hungrytime < monstermoves + DOG_SATIATED/3) || 
					(YouHunger < WEAK && edog->hungrytime < monstermoves))
				)
			) 
#endif /* PET_SATIATION */
		    return dog_eat(mtmp, obj, omx, omy, FALSE);
		}

		if(can_carry(mtmp, obj) && !obj->cursed &&
			could_reach_item(mtmp, obj->ox, obj->oy)) {
	        boolean can_use = could_use_item(mtmp, obj, TRUE);
	        if (can_use ||
		        (!droppables && rn2(20) < edog->apport+3)) {
				if (can_use || rn2(udist) || !rn2(edog->apport)) {
					if (cansee(omx, omy) && flags.verbose)
					pline("%s picks up %s.", Monnam(mtmp),
						distant_name(obj, doname));
					obj_extract_self(obj);
					newsym(omx,omy);
					(void) mpickobj(mtmp,obj);
					if (mon_attacktype(mtmp, AT_WEAP) &&
						mtmp->weapon_check == NEED_WEAPON) {
					mtmp->weapon_check = NEED_HTH_WEAPON;
					(void) mon_wield_item(mtmp);
					}
					m_dowear(mtmp, FALSE);
				}
		    }
		}
	    }
	return 0;
}

/* set dog's goal -- gtyp, gx, gy
 * returns -1/0/1 (dog's desire to approach player) or -2 (abort move)
 */
STATIC_OVL int
dog_goal(mtmp, edog, after, udist, whappr)
register struct monst *mtmp;
struct edog *edog;
int after, udist, whappr;
{
	register int omx, omy;
	boolean in_masters_sight, dog_has_minvent;
	register struct obj *obj;
	xchar otyp;
	int appr;

#ifdef STEED
	/* Steeds don't move on their own will */
	if (mtmp == u.usteed)
		return (-2);
#endif

	omx = mtmp->mx;
	omy = mtmp->my;

	in_masters_sight = couldsee(omx, omy);
	dog_has_minvent = (DROPPABLES(mtmp) != 0);

	if (!edog || mtmp->mleashed) {	/* he's not going anywhere... */
	    gtyp = APPORT;
	    gx = u.ux;
	    gy = u.uy;
	} else if(distu(mtmp->mx,mtmp->my) > 5 || (!in_masters_sight && distu(mtmp->mx,mtmp->my) > 2) ){
	    gtyp = UNDEF;
	} else {
#define DDIST(x,y) (dist2(x,y,omx,omy))
#define SQSRCHRADIUS 5
	    int min_x, max_x, min_y, max_y;
	    register int nx, ny;
	    boolean can_use = FALSE;

	    gtyp = UNDEF;	/* no goal as yet */
	    gx = gy = 0;	/* suppress 'used before set' message */

	    if ((min_x = omx - SQSRCHRADIUS) < 1) min_x = 1;
	    if ((max_x = omx + SQSRCHRADIUS) >= COLNO) max_x = COLNO - 1;
	    if ((min_y = omy - SQSRCHRADIUS) < 0) min_y = 0;
	    if ((max_y = omy + SQSRCHRADIUS) >= ROWNO) max_y = ROWNO - 1;

	    /* nearby food is the first choice, then other objects */
	    for (obj = fobj; obj; obj = obj->nobj) {
		nx = obj->ox;
		ny = obj->oy;
		if (nx >= min_x && nx <= max_x && ny >= min_y && ny <= max_y) {
		    otyp = dogfood(mtmp, obj);
		    /* skip inferior goals */
		    if (otyp > gtyp || otyp == UNDEF)
			continue;
		    /* avoid cursed items unless starving */
		    if (cursed_object_at(nx, ny) &&
			    !(edog->mhpmax_penalty && otyp < MANFOOD))
			continue;
		    /* skip completely unreacheable goals */
		    if (!could_reach_item(mtmp, nx, ny) ||
		        !can_reach_location(mtmp, mtmp->mx, mtmp->my, nx, ny))
			continue;
		    if (otyp < MANFOOD) {
			if (otyp < gtyp || DDIST(nx,ny) < DDIST(gx,gy)) {
			    gx = nx;
			    gy = ny;
			    gtyp = otyp;
			}
		    } else if(gtyp == UNDEF && in_masters_sight &&
			      ((can_use = could_use_item(mtmp, obj, TRUE))
			       || !dog_has_minvent) &&
			      (!levl[omx][omy].lit || levl[u.ux][u.uy].lit) &&
			      (otyp == MANFOOD || m_cansee(mtmp, nx, ny)) &&
			      (can_use ||
			       edog->apport > rn2(8)) &&
			      can_carry(mtmp,obj)) {
			gx = nx;
			gy = ny;
			gtyp = APPORT;
		    }
		}
	    }
	}

	/* follow player if appropriate, or move to attack nearby enemies */
	if (gtyp == UNDEF ||
	    (gtyp != DOGFOOD && gtyp != APPORT && monstermoves < edog->hungrytime)) {
		gx = u.ux;
		gy = u.uy;
		if (after && udist <= 4 && gx == u.ux && gy == u.uy)
			return(-2);
		appr = (udist >= 9) ? 1 : (mtmp->mflee) ? -1 : 0;
		if (udist > 1) {
			if (!IS_ROOM(levl[u.ux][u.uy].typ) || !rn2(4) ||
			   whappr ||
			   (dog_has_minvent && rn2(edog->apport)))
				appr = 1;
		}
		if(appr == 0 && u.sealsActive&SEAL_ECHIDNA && !mindless_mon(mtmp) && (is_animal(mtmp->data) || slithy(mtmp->data) || nohands(mtmp->data))){
			appr = 1;
		}
		if(appr == 0 && Race_if(PM_DROW) && is_spider(mtmp->data)){
			appr = 1;
		}
		
		if(appr >= 0){
			struct monst *m2 = (struct monst *)0;
			int distminbest = SQSRCHRADIUS;
			for(m2=fmon; m2; m2 = m2->nmon){
				if(!m2->mtame && !m2->mpeaceful && distu(m2->mx,m2->my) <= SQSRCHRADIUS && distmin(mtmp->mx,mtmp->my,m2->mx,m2->my) < distminbest){
					distminbest = distmin(mtmp->mx,mtmp->my,m2->mx,m2->my);
					gx = m2->mx;
					gy = m2->my;
					if(MON_WEP(mtmp) && 
						(is_launcher(MON_WEP(mtmp)) || is_firearm(MON_WEP(mtmp)) )
					){
						if(distmin(mtmp->mx,mtmp->my,m2->mx,m2->my) >= BOLT_LIM) appr = 1;
						else if(distmin(mtmp->mx,mtmp->my,m2->mx,m2->my) < 4) appr = -1;
						else appr = 0;
					} else {
						appr = 1;
					}
				}
			}
		}
		/* if you have dog food it'll follow you more closely */
		if (appr == 0) {
			obj = invent;
			while (obj) {
				if(dogfood(mtmp, obj) == DOGFOOD) {
					appr = 1;
					break;
				}
				obj = obj->nobj;
			}
		}
	} else
	    appr = 1;	/* gtyp != UNDEF */
	if(mtmp->mconf)
	    appr = 0;

#define FARAWAY (COLNO + 2)		/* position outside screen */
	if (gx == u.ux && gy == u.uy && !in_masters_sight) {
	    register coord *cp;

	    cp = gettrack(omx,omy,0);
	    if (cp) {
		gx = cp->x;
		gy = cp->y;
		if(edog) edog->ogoal.x = 0;
	    } else {
		/* assume master hasn't moved far, and reuse previous goal */
		if(edog && edog->ogoal.x &&
		   ((edog->ogoal.x != omx) || (edog->ogoal.y != omy))) {
		    gx = edog->ogoal.x;
		    gy = edog->ogoal.y;
		    edog->ogoal.x = 0;
		} else {
		    int fardist = FARAWAY * FARAWAY;
		    gx = gy = FARAWAY; /* random */
		    do_clear_area(omx, omy, 9, wantdoor,
				  (genericptr_t)&fardist);

		    /* here gx == FARAWAY e.g. when dog is in a vault */
		    if (gx == FARAWAY || (gx == omx && gy == omy)) {
			gx = u.ux;
			gy = u.uy;
		    } else if(edog) {
			edog->ogoal.x = gx;
			edog->ogoal.y = gy;
		    }
		}
	    }
	} else if(edog) {
	    edog->ogoal.x = 0;
	}
	return appr;
}

boolean
acceptable_pet_target(mtmp, mtmp2, ranged)
register struct monst *mtmp;
register struct monst *mtmp2;
boolean ranged;
{
	if(mtmp2->moccupation) return FALSE;
	
	if(nonthreat(mtmp2)) return FALSE;
	
	if(mtmp->mtame && u.peaceful_pets && mtmp2->mpeaceful)
		return FALSE;

	if(mtmp->mtame && mtmp2->mpeaceful && !u.uevent.uaxus_foe && mtmp2->mtyp == PM_AXUS)
		return FALSE;
	
	if(mtmp->mtame && mtmp2->mpeaceful && is_metroid(mtmp->data) && is_metroid(mtmp2->data))
		return FALSE;
	
	if(mtmp->mhp < 100 && attacktype_fordmg(mtmp2->data, AT_BOOM, AD_MAND))
		return FALSE;
	
	if((Upolyd ? u.mh < 100 : u.uhp < 100) && mtmp->mtame && attacktype_fordmg(mtmp2->data, AT_BOOM, AD_MAND))
		return FALSE;
	
	if(mtmp2->mtyp == PM_MANDRAKE)
		return FALSE;

	if(has_template(mtmp2, SLIME_REMNANT) && mtmp->mpeaceful == mtmp2->mpeaceful)
		return FALSE;

	if(mtmp2->mtyp == PM_BEAUTEOUS_ONE && mtmp->mpeaceful == mtmp2->mpeaceful)
		return FALSE;
	
    return !(
		(!ranged &&
			(int)mtmp2->m_lev >= (int)mtmp->m_lev+2 + (mtmp->encouraged)*2 &&
			!(mon_attacktype(mtmp, AT_EXPL) || extra_nasty(mtmp->data) || mtmp->m_lev >= max(mtmp->data->mlevel*1.5, 5))
		) ||
		(!ranged &&
			 mtmp2->mtyp == PM_FLOATING_EYE && rn2(10) &&
			 !is_blind(mtmp) && haseyes(mtmp->data) && !is_blind(mtmp2)
			 && (mon_resistance(mtmp,SEE_INVIS) || !mtmp2->minvis)
		) ||
		(!ranged &&
			mtmp2->mtyp==PM_GELATINOUS_CUBE && rn2(10)
		) ||
		(!ranged &&
			max_passive_dmg(mtmp2, mtmp) >= mtmp->mhp
		) ||
		((   mtmp2->mtyp == urole.guardnum
			  || mtmp2->mtyp == urole.ldrnum
			  || (Role_if(PM_NOBLEMAN) && (mtmp->mtyp == PM_KNIGHT || mtmp->mtyp == PM_MAID || mtmp->mtyp == PM_PEASANT) && mtmp->mpeaceful)
			  || (Race_if(PM_DROW) && is_drow(mtmp->data) && mtmp->mpeaceful)
			  || (Role_if(PM_KNIGHT) && (mtmp->mtyp == PM_KNIGHT) && mtmp->mpeaceful)
			  || (Race_if(PM_GNOME) && (is_gnome(mtmp->data) && !is_undead(mtmp->data)) && mtmp->mpeaceful)
			  || always_peaceful(mtmp2->data)
		  ) &&
			mtmp2->mpeaceful && !(Conflict || mtmp->mberserk)
		) ||
	   (!ranged && touch_petrifies(mtmp2->data) &&
		!resists_ston(mtmp)));
}

boolean
betrayed(mtmp)
register struct monst *mtmp;
{
    int udist = distu(mtmp->mx, mtmp->my);
	if(get_mx(mtmp, MX_EDOG)){
		if(EDOG(mtmp)->loyal)
			return FALSE;
		else if(EDOG(mtmp)->waspeaceful && mtmp->mpeacetime == 0 && u.uhp >= u.uhpmax/10)
			return FALSE;

	}
//	pline("testing for betrayal");
    if ( (udist < 4 || !rn2(3)) 
			&& get_mx(mtmp, MX_EDOG)
		    && (can_betray(mtmp->data) || roll_madness(MAD_PARANOIA))
		    && u.uhp < u.uhpmax/2	/* You look like you're in a bad spot */
		    && mtmp->mhp >= u.uhp	/* Pet is buff enough */
		    && rn2(22) > mtmp->mtame  - 10	/* Roll against tameness */
		    && rn2(EDOG(mtmp)->abuse + 2)) {
	/* Treason */
	if (canseemon(mtmp))
	    pline("%s turns on you!", Monnam(mtmp));
	else
	    pline("You feel uneasy about %s.", y_monnam(mtmp));
	untame(mtmp, 0);
	mtmp->mtraitor = TRUE;
	if (get_mx(mtmp, MX_ESUM))
		mtmp->mextra_p->esum_p->summoner = (struct monst *)0;

	/* Do we need to call newsym() here? */
	newsym(mtmp->mx, mtmp->my);
	return TRUE;
    }
    return FALSE;
}


/* return 0 (no move), 1 (move) or 2 (dead) */
int
dog_move(mtmp, after)
register struct monst *mtmp;
register int after;	/* this is extra fast monster movement */
{
	int omx, omy;		/* original mtmp position */
	int appr, whappr, udist;
	int i, j, k;
	struct obj *obj = (struct obj *) 0;
	xchar otyp;
	boolean has_edog, cursemsg[9], do_eat = FALSE;
	xchar nix, niy;		/* position mtmp is (considering) moving to */
	register int nx, ny;	/* temporary coordinates */
	xchar cnt, uncursedcnt, chcnt;
	int chi = -1, nidist, ndist;
	coord poss[9];
	long info[9], allowflags;
#define GDIST(x,y) (dist2(x,y,gx,gy))

	/*
	 * Tame Angels have isminion set and an ispriest structure instead of
	 * an edog structure.  Fortunately, guardian Angels need not worry
	 * about mundane things like eating and fetching objects, and can
	 * spend all their energy defending the player.  (They are the only
	 * monsters with other structures that can be tame.)
	 */
	has_edog = !!get_mx(mtmp, MX_EDOG);

	omx = mtmp->mx;
	omy = mtmp->my;
	if (has_edog && dog_hunger(mtmp, EDOG(mtmp))) return(2);	/* starved */

	udist = distu(omx,omy);
#ifdef STEED
	/* Let steeds eat and maybe throw rider during Conflict */
	if (mtmp == u.usteed) {
	    if ((Conflict && !resist(mtmp, RING_CLASS, 0, 0)) || mtmp->mberserk) {
		dismount_steed(DISMOUNT_THROWN);
		return (1);
	    }
	    udist = 1;
	} else
#endif
	/* maybe we tamed him while being swallowed --jgm */
	if (!udist) return(0);

	if (!rn2(850) && betrayed(mtmp)) return 1;

	nix = omx;	/* set before newdogpos */
	niy = omy;
	cursemsg[0] = FALSE;	/* lint suppression */
	info[0] = 0;		/* ditto */

	if (has_edog) {
	    j = dog_invent(mtmp, EDOG(mtmp), udist);
	    if (j == 2) return 2;		/* died */
	    else if (j == 1) goto newdogpos;	/* eating something */

	    whappr = (monstermoves - EDOG(mtmp)->whistletime < 5) || (uwep && uwep->otyp == SHEPHERD_S_CROOK);
	} else
	    whappr = 0;

	appr = dog_goal(mtmp, has_edog ? EDOG(mtmp) : (struct edog *)0,
							after, udist, whappr);
	if (appr == -2) return(0);

#ifdef BARD
	/*NOTE: This may make pets skip their turns IF YOU ARE SINGING (pet_can_sing checks). */
	if (pet_can_sing(mtmp, FALSE))
		return(3);
	/* lose tameness if under effects of taming song */
	if (has_edog && EDOG(mtmp)->friend && mtmp->mtame) {
		mtmp->mtame -= (always_hostile_mon(mtmp) ? 2 : 1);
		if (wizard) pline("[%s friend for %d(%d)]", Monnam(mtmp), mtmp->mtame, EDOG(mtmp)->waspeaceful);
		if (mtmp->mtame <= 0) {
			untame(mtmp, EDOG(mtmp)->waspeaceful);
			return(3);
		}
	}
#endif

	allowflags = ALLOW_M | ALLOW_TRAPS | ALLOW_SSM | ALLOW_SANCT;
	if (mon_resistance(mtmp,PASSES_WALLS)) allowflags |= (ALLOW_ROCK | ALLOW_WALL);
	if (passes_bars(mtmp)  && !Is_illregrd(&u.uz) ) allowflags |= ALLOW_BARS;
	if (throws_rocks(mtmp->data)) allowflags |= ALLOW_ROCK;
	
	/*I'm making minions just RESIST conflict automatically, instead of becoming a swarm of hostile angels*/
	if ((Conflict && has_edog && !resist(mtmp, RING_CLASS, 0, 0)) || mtmp->mberserk) {
	    allowflags |= ALLOW_U;
	    // if (!has_edog) {
		// coord mm;
		// /* Guardian angel refuses to be conflicted; rather,
		 // * it disappears, angrily, and sends in some nasties
		 // */
		// if (canspotmon(mtmp)) {
		    // pline("%s rebukes you, saying:", Monnam(mtmp));
		    // verbalize("Since you desire conflict, have some more!");
		// }
		// mongone(mtmp);
		// i = rnd(4);
		// while(i--) {
		    // mm.x = u.ux;
		    // mm.y = u.uy;
		    // if(enexto(&mm, mm.x, mm.y, &mons[PM_ANGEL]))
			// (void) mk_roamer(&mons[PM_ANGEL], u.ualign.type,
					 // mm.x, mm.y, FALSE);
		// }
		// return(2);

	    // }
	}
	if (!Conflict && !mtmp->mberserk && !mtmp->mconf &&
	    mtmp == u.ustuck && !sticks(&youmonst)) {
	    unstuck(mtmp);	/* swallowed case handled above */
	    You("get released!");
	}

/*
 * We haven't moved yet, so search for monsters to attack from a
 * distance and attack them if it's plausible.
 */
	if (find_offensive(mtmp))
	{
	    int ret = use_offensive(mtmp);
	    if (ret == 1) return 2; /* died */
	    if (ret == 2) return 1; /* did something */
	}
	else if (find_defensive(mtmp))
	{
	    int ret = use_defensive(mtmp);
	    if (ret == 1) return 2; /* died */
	    if (ret == 2) return 1; /* did something */
	}
	else if (find_misc(mtmp))
	{
	    int ret = use_misc(mtmp);
	    if (ret == 1) return 2; /* died */
	    if (ret == 2) return 1; /* did something */
	}
	else if (mtmp->mlstmv != monstermoves) /* ?? only do a ranged attack once per turn? */
	{
		if(!mtarget_adjacent(mtmp)){ /* don't fight at range if there's a melee target */
			/* Look for monsters to fight (at a distance) */
			struct monst *mtmp2 = mfind_target(mtmp, FALSE, TRUE);
			if (mtmp2 && (mtmp2 != mtmp)
				&& (mtmp2 != &youmonst)
				&& acceptable_pet_target(mtmp, mtmp2, TRUE))
			{
				int res;
				mon_ranged_gazeonly = 1;//State variable
				res = (mtmp2 == &youmonst) ? mattacku(mtmp)
					: mattackm(mtmp, mtmp2);

				if (res & MM_AGR_DIED)
					return 2; /* Oops, died */

				if (!(mon_ranged_gazeonly) && (res & MM_HIT))
					return 1; /* that was our move for the round */
			}
		}
	}

	if (!nohands(mtmp->data) && !verysmall(mtmp->data)) {
		allowflags |= OPENDOOR;
		if (m_carrying(mtmp, SKELETON_KEY)||m_carrying(mtmp, UNIVERSAL_KEY)) allowflags |= UNLOCKDOOR;
	}
	if (species_busts_doors(mtmp->data)) allowflags |= BUSTDOOR;
	if (tunnels(mtmp->data)
#ifdef REINCARNATION
	    && !Is_rogue_level(&u.uz)	/* same restriction as m_move() */
#endif
		) allowflags |= ALLOW_DIG;
	cnt = mfndpos(mtmp, poss, info, allowflags);

	/* Normally dogs don't step on cursed items, but if they have no
	 * other choice they will.  This requires checking ahead of time
	 * to see how many uncursed item squares are around.
	 */
	uncursedcnt = 0;
	for (i = 0; i < cnt; i++) {
		nx = poss[i].x; ny = poss[i].y;
		if (MON_AT(nx,ny) && !(info[i] & ALLOW_M)) continue;
		if (cursed_object_at(nx, ny)) continue;
		uncursedcnt++;
	}

	chcnt = 0;
	chi = -1;
	nidist = GDIST(nix,niy);

	for (i = 0; i < cnt; i++) {
		nx = poss[i].x;
		ny = poss[i].y;
		cursemsg[i] = FALSE;

		/* if leashed, we drag him along. */
		if (mtmp->mleashed && distu(nx, ny) > 4) continue;

		/* if a guardian, try to stay close by choice */
		if (!has_edog &&
		    (j = distu(nx, ny)) > 16 && j >= udist) continue;

		if ((info[i] & ALLOW_M) && MON_AT(nx, ny)) {
		    int mstatus;
		    register struct monst *mtmp2 = m_at(nx,ny);

        	if (!acceptable_pet_target(mtmp, mtmp2, FALSE))
				continue;

		    if (after) return(0); /* hit only once each move */

		    notonhead = 0;
		    mstatus = mattackm(mtmp, mtmp2);

		    /* aggressor (pet) died */
		    if (mstatus & MM_AGR_DIED) return 2;

		    if ((mstatus & MM_HIT) && !(mstatus & MM_DEF_DIED) &&
			    rn2(4) && mtmp2->mlstmv != monstermoves &&
			    !onscary(mtmp->mx, mtmp->my, mtmp2) &&
			    /* monnear check needed: long worms hit on tail */
			    monnear(mtmp2, mtmp->mx, mtmp->my)
			){
			mstatus = mattackm(mtmp2, mtmp);  /* return attack */
			if (mstatus & MM_DEF_DIED) return 2;
		    }

		    return 0;
		}

		{   /* Dog avoids harmful traps, but perhaps it has to pass one
		     * in order to follow player.  (Non-harmful traps do not
		     * have ALLOW_TRAPS in info[].)  The dog only avoids the
		     * trap if you've seen it, unlike enemies who avoid traps
		     * if they've seen some trap of that type sometime in the
		     * past.  (Neither behavior is really realistic.)
		     */
		    struct trap *trap;

		    if ((info[i] & ALLOW_TRAPS) && (trap = t_at(nx,ny))) {
			if (mtmp->mleashed) {
			    if (flags.soundok) whimper(mtmp);
			} else
			    /* 1/40 chance of stepping on it anyway, in case
			     * it has to pass one to follow the player...
			     */
			    if ((trap->tseen || mon_resistance(mtmp, SEARCHING)) && rn2(40)) continue;
		    }
		}

		/* dog eschews cursed objects, but likes dog food */
		/* (minion isn't interested; `cursemsg' stays FALSE) */
		if (has_edog)
		for (obj = level.objects[nx][ny]; obj; obj = obj->nexthere) {
		    if (obj->cursed) cursemsg[i] = TRUE;
		    else if ((otyp = dogfood(mtmp, obj)) < MANFOOD &&
			     (otyp < ACCFOOD
			     || EDOG(mtmp)->hungrytime <= monstermoves)
#ifdef PET_SATIATION
			     && EDOG(mtmp)->hungrytime < monstermoves + DOG_SATIATED
#endif /* PET_SATIATION */
				 && !((mtmp->misc_worn_check & W_ARMH) && which_armor(mtmp, W_ARMH) && 
					   FacelessHelm(which_armor(mtmp, W_ARMH)) && (which_armor(mtmp, W_ARMH))->cursed)
				 && !((mtmp->misc_worn_check & W_ARMC) && which_armor(mtmp, W_ARMC) && 
					   FacelessCloak(which_armor(mtmp, W_ARMC)) && (which_armor(mtmp, W_ARMC))->cursed)
			     ) {
			/* Note: our dog likes the food so much that he
			 * might eat it even when it conceals a cursed object */
			nix = nx;
			niy = ny;
			chi = i;
			do_eat = TRUE;
			cursemsg[i] = FALSE;	/* not reluctant */
			goto newdogpos;
		    }
		}
		/* didn't find something to eat; if we saw a cursed item and
		   aren't being forced to walk on it, usually keep looking */
		if (cursemsg[i] && !mtmp->mleashed && uncursedcnt > 0 &&
		    rn2(13 * uncursedcnt)) continue;

		j = ((ndist = GDIST(nx,ny)) - nidist) * appr;
		if ((j == 0 && !rn2(++chcnt)) || j < 0 ||
			(j > 0 && !whappr &&
				((omx == nix && omy == niy && !rn2(3))
					|| !rn2(12))
			)) {
			nix = nx;
			niy = ny;
			nidist = ndist;
			if(j < 0) chcnt = 0;
			chi = i;
		}
	nxti:	;
	}
newdogpos:
	if (nix != omx || niy != omy) {
		struct obj *mw_tmp;

		if (info[chi] & ALLOW_U) {
			if (mtmp->mleashed) { /* play it safe */
				pline("%s breaks loose of %s leash!",
				      Monnam(mtmp), mhis(mtmp));
				m_unleash(mtmp, FALSE);
			}
			(void) mattacku(mtmp);
			return(0);
		}
		if (!m_in_out_region(mtmp, nix, niy))
		    return 1;
		if (((IS_ROCK(levl[nix][niy].typ) && may_dig(nix,niy)) ||
		     closed_door(nix, niy)) &&
		    tunnels(mtmp->data) && needspick(mtmp->data)) {
		    if (closed_door(nix, niy)) {
			if (!(mw_tmp = MON_WEP(mtmp)) ||
			    !is_pick(mw_tmp) || !is_axe(mw_tmp))
			    mtmp->weapon_check = NEED_PICK_OR_AXE;
		    } else if (IS_TREE(levl[nix][niy].typ)) {
			if (!(mw_tmp = MON_WEP(mtmp)) || !is_axe(mw_tmp))
			    mtmp->weapon_check = NEED_AXE;
		    } else if (!(mw_tmp = MON_WEP(mtmp)) || !is_pick(mw_tmp)) {
			mtmp->weapon_check = NEED_PICK_AXE;
		    }
			if (mtmp->weapon_check >= NEED_PICK_AXE) {
				mon_wield_item(mtmp);
				return 0;	/* did not move. Maybe spent time wielding,
							but definitely does not just move without required item */
			}
		}
		/* insert a worm_move() if worms ever begin to eat things */
		remove_monster(omx, omy);
		place_monster(mtmp, nix, niy);
		if(mtmp->mtyp == PM_SURYA_DEVA){
			struct monst *blade;
			for(blade = fmon; blade; blade = blade->nmon) if(blade->mtyp == PM_DANCING_BLADE && mtmp->m_id == blade->mvar_suryaID) break;
			if(blade){
				int bx = blade->mx, by = blade->my;
				remove_monster(bx, by);
				place_monster(blade, omx, omy);
				newsym(omx,omy);
				newsym(bx,by);
			}
		}
		if (cursemsg[chi] && (cansee(omx,omy) || cansee(nix,niy)))
			pline("%s moves only reluctantly.", Monnam(mtmp));
		for (j=MTSZ-1; j>0; j--) mtmp->mtrack[j] = mtmp->mtrack[j-1];
		mtmp->mtrack[0].x = omx;
		mtmp->mtrack[0].y = omy;
		/* We have to know if the pet's gonna do a combined eat and
		 * move before moving it, but it can't eat until after being
		 * moved.  Thus the do_eat flag.
		 */
		if (do_eat) {
		    if (dog_eat(mtmp, obj, omx, omy, FALSE) == 2) return 2;
		}
	} else if (mtmp->mleashed && distu(omx, omy) > 4) {
		/* an incredible kludge, but the only way to keep pooch near
		 * after it spends time eating or in a trap, etc.
		 */
		coord cc;

		nx = sgn(omx - u.ux);
		ny = sgn(omy - u.uy);
		cc.x = u.ux + nx;
		cc.y = u.uy + ny;
		if (goodpos(cc.x, cc.y, mtmp, 0)) goto dognext;

		i  = xytod(nx, ny);
		for (j = (i + 7)%8; j < (i + 1)%8; j++) {
			dtoxy(&cc, j);
			if (goodpos(cc.x, cc.y, mtmp, 0)) goto dognext;
		}
		for (j = (i + 6)%8; j < (i + 2)%8; j++) {
			dtoxy(&cc, j);
			if (goodpos(cc.x, cc.y, mtmp, 0)) goto dognext;
		}
		cc.x = mtmp->mx;
		cc.y = mtmp->my;
dognext:
		if (!m_in_out_region(mtmp, nix, niy))
		  return 1;
		remove_monster(mtmp->mx, mtmp->my);
		place_monster(mtmp, cc.x, cc.y);
		newsym(cc.x,cc.y);
		set_apparxy(mtmp);
	}
	return(1);
}

/* check if a monster could pick up objects from a location */
STATIC_OVL boolean
could_reach_item(mon, nx, ny)
struct monst *mon;
xchar nx, ny;
{
    if ((!is_pool(nx,ny, FALSE) || mon_resistance(mon,SWIMMING)) &&
	(!is_lava(nx,ny) || likes_lava(mon->data)) &&
	(!boulder_at(nx,ny) || throws_rocks(mon->data)))
    	return TRUE;
    return FALSE;
}

/* Hack to prevent a dog from being endlessly stuck near an object that
 * it can't reach, such as caught in a teleport scroll niche.  It recursively
 * checks to see if the squares in between are good.  The checking could be a
 * little smarter; a full check would probably be useful in m_move() too.
 * Since the maximum food distance is 5, this should never be more than 5 calls
 * deep.
 */
STATIC_OVL boolean
can_reach_location(mon, mx, my, fx, fy)
struct monst *mon;
xchar mx, my, fx, fy;
{
    int i, j;
    int dist;

    if (mx == fx && my == fy) return TRUE;
    if (!isok(mx, my)) return FALSE; /* should not happen */
    
    dist = dist2(mx, my, fx, fy);
    for(i=mx-1; i<=mx+1; i++) {
	for(j=my-1; j<=my+1; j++) {
	    if (!isok(i, j))
		continue;
	    if (dist2(i, j, fx, fy) >= dist)
		continue;
	    if (IS_ROCK(levl[i][j].typ) && !mon_resistance(mon,PASSES_WALLS) &&
				    (!may_dig(i,j) || !tunnels(mon->data)))
		continue;
	    if (IS_DOOR(levl[i][j].typ) &&
				(levl[i][j].doormask & (D_CLOSED | D_LOCKED)))
		continue;
	    if (!could_reach_item(mon, i, j))
		continue;
	    if (can_reach_location(mon, i, j, fx, fy))
		return TRUE;
	}
    }
    return FALSE;
}

#endif /* OVL0 */
#ifdef OVLB

/*ARGSUSED*/	/* do_clear_area client */
STATIC_PTR void
wantdoor(x, y, distance)
int x, y;
genericptr_t distance;
{
    int ndist;

    if (*(int*)distance > (ndist = distu(x, y))) {
	gx = x;
	gy = y;
	*(int*)distance = ndist;
    }
}

#endif /* OVLB */

/*dogmove.c*/
