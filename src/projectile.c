#include "hack.h"
#include "xhity.h"

/* PROJECTILE.C */
/* 
 * projectile()
 * 
 * Omnibus projectile firing function
 * 
 * returns MM_HIT/MM_DEF_DIED-etc values IF it hit a creature, otherwise returns MM_MISS (0).
 * 
 *  
 */
int
projectile(magr, ammo, launcher, fired, initx, inity, dx, dy, initrange, destroy, verbose)
struct monst * magr;			/* Creature responsible for the projectile. Can be non-existant. */
struct obj * ammo;				/* Projectile object. Must exist. May be in an inventory, or free, or anywhere. */
struct obj * launcher;			/* Launcher for the projectile. Can be non-existant. Implies "fired" is true. */
boolean fired;					/* Whether or not the projectile was fired (ex arrow from a bow). Fired without a launcher is possible (ex AT_ARRW). */
int initx;						/* x; Where the projectile originates from. Does not hit this location. */
int inity;						/* y; Where the projectile originates from. Does not hit this location. */
int dx;							/* x; Direction of projectile's movement */
int dy;							/* y; Direction of projectile's movement */
int initrange;						/* Maximum range for projectile */
boolean destroy;				/* TRUE if projectile should be forced to be destroyed at the end */
boolean verbose;				/* TRUE if messages should be printed even if the player can't see what happened */
{
	struct obj * thrownobj;		/* singular fired/thrown object */
	boolean onlyone;			/* if ammo only consists of thrownobj */
	struct monst * mdef;
	int result = 0;
	int range = initrange;
	int curx = initx;
	int cury = inity;

	/* Sanity checks */
	if (!ammo) {
		impossible("projectile() called without ammo?");
		return 0;
	}
	if (launcher && !fired) {
		impossible("projectile() called with launcher but not fired?");
		return 0;
	}

	/* First, create thrownobj */
	if (ammo->quan > 1L) {
		/* split off 1 object */
		thrownobj = splitobj(ammo, 1L);
		onlyone = FALSE;
	}
	else {
		/* there is just the one being thrown */
		thrownobj = ammo;
		onlyone = TRUE;
	}
	/* clean up where it came from */
	switch (thrownobj->where)
	{
	case OBJ_FREE:		/* object not attached to anything */
		/* nothing required */
		break;
	case OBJ_FLOOR:		/* object on floor */
		obj_extract_self(thrownobj);
		break;
	case OBJ_INVENT:	/* object in the hero's inventory */
		if (onlyone) {
			if (thrownobj->owornmask)
				remove_worn_item(thrownobj, FALSE);
		}
		freeinv(thrownobj);
		break;
	case OBJ_MINVENT:	/* object in a monster inventory */
		if (!magr) {
			impossible("thrownobj in monster's inventory with no monster?");
			return 0;
		}
		if (onlyone) {
			if (MON_WEP(magr) == thrownobj) {
				setmnotwielded(magr, thrownobj);
				MON_NOWEP(magr);
			}
			if (MON_SWEP(magr) == thrownobj) {
				setmnotwielded(magr, thrownobj);
				MON_NOSWEP(magr);
			}
		}
		m_freeinv(thrownobj);
		break;
	default:			/* object elsewhere (migrating? contained?) */
		impossible("unhandled where of thrown item, %d", thrownobj->where);
		break;
	}
	thrownobj->owornmask = 0;

	/* mis-throw/fire can change the direction of the projectile */
	if (/* needs a creature to be involved in throwing the projectile */
		magr && (
		/* cursed thrownobj */
		(thrownobj->cursed && !((magr == &youmonst) ? is_weldproof(youracedata) : is_weldproof_mon(magr)))
		||
		/* flintlock (fired by not-you) */
		(magr != &youmonst && fired && launcher && launcher->otyp == FLINTLOCK)
		)
		&&
		/* at a 1/7 chance */
		(dx || dy) && !rn2(7)
		) {
		/* message */
		if (magr == &youmonst){
			if (fired)
				Your("weapon misfires!");
			else
				pline("%s as you throw it!",
				Tobjnam(thrownobj, "slip"));
		}
		if (canseemon(magr) && flags.verbose) {
			if (fired)
				pline("%s misfires!", Monnam(magr));
			else
				pline("%s as %s throws it!",
					Tobjnam(thrownobj, "slip"), mon_nam(magr));
		}
		/* new direction */
		dx = rn2(3) - 1;
		dy = rn2(3) - 1;
		/* check validity of new direction */
		if (!dx && !dy) {
			end_projectile();
			return 0;
		}
	}

	/* move the projectile loop */
	while (range--)	/* always ends via break */
	{
		/* priority order: */
		/* space has monster and is not initxy */
		/* space has sink and is not initxy */
		/* space has wall and is not initxy */
		/* space ahead has wall and no monster */

		/* projectile is on a creature */
		if ((curx != initx || cury != inity) &&
			(mdef = m_at(curx, cury) || (curx == u.ux && cury == u.uy)))
		{
			if (!mdef)
				mdef = &youmonst;

			result = projectile_attack();

			if (result)
			{
				end_projectile();
				return result;
			}
		}

		/* projectile is on a sink (it "sinks" down) or is on a non-allowable square */
		if ((curx != initx || cury != inity) &&
			(!ZAP_POS(levl[curx][cury].typ)) || (IS_SINK(levl[curx][cury].typ)))
		{
			end_projectile();
			return 0;
		}

		/* space ahead has wall and no monster */
		if (!isok(curx + dx, cury + dy) &&
			(!ZAP_POS(levl[curx+dx][cury+dy].typ) &&
			!m_at(curx+dx, cury+dy)))
		{
			end_projectile();
			return 0;
		}
		/* out of range */
		if (range == 0)
		{
			end_projectile();
			return 0;
		}
	}
	impossible("oh no");
	return 0;
}

/* 
 * projectile_attack()
 * 
 * A projectile is going towards mdef
 * 
 * This should end the projectile unless MM_MISS is returned.
 *  
 */
int
projectile_attack(magr, mdef, thrownobj, launcher, fired, phitx, phity, pdx, pdy, prange, prange2, destroy, verbose)
struct monst * magr;			/* Creature responsible for the projectile. Can be non-existant. */
struct monst * mdef;			/* Creature under fire. */
struct obj * thrownobj;			/* Projectile object. Must be free. */
struct obj * launcher;			/* Launcher for the projectile. Can be non-existant. Implies "fired" is true. */
boolean fired;					/* Whether or not the projectile was fired (ex arrow from a bow). Fired without a launcher is possible (ex AT_ARRW). */
int * phitx;					/* pointer to: x; Where the projectile is. */
int * phity;					/* pointer to: y; Where the projectile is. */
int * pdx;						/* pointer to: x; Direction of projectile's movement */
int * pdy;						/* pointer to: y; Direction of projectile's movement */
int * prange;					/* pointer to: Remaining range for projectile */
int * prange2;					/* pointer to: Remaining 2x range for projectile */
boolean destroy;				/* TRUE if projectile should be forced to be destroyed at the end */
boolean verbose;				/* TRUE if messages should be printed even if the player can't see what happened */
{
	int hitx	= *phitx;
	int hity	= *phity;
	int dx		= *pdx;
	int dy		= *pdy;
	int range	= *prange;
	int range2	= *prange2;
	boolean youagr = (magr == &youmonst);
	boolean youdef = (mdef == &youmonst);
	struct permonst * pa = youagr ? youracedata : magr->data;
	struct permonst * pd = youdef ? youracedata : mdef->data;
	boolean misfired = (!fired && is_ammo(thrownobj));
	int result;
	int accuracy;
	int dieroll;
	int vis;
	boolean wepgone = FALSE;		/* lets hmon return whether or not it destroyed thrownobj */


	/* Set up the visibility of action */
	if (youagr || youdef || (cansee(x(magr), y(magr)) && cansee(x(mdef), y(mdef))))
	{
		if (youagr || (cansee(x(magr), y(magr)) && canseemon(magr)))
			vis |= VIS_MAGR;
		if (youdef || (cansee(x(mdef), y(mdef)) && canseemon(mdef)))
			vis |= VIS_MDEF;
		if (youagr || youdef || canspotmon(magr) || canspotmon(mdef))
			vis |= VIS_NONE;
	}


	/* train player's Shien skill, if player is defending */
	if (youdef && uwep && is_lightsaber(uwep) && litsaber(uwep) && P_SKILL(weapon_type(uwep)) >= P_BASIC){
		if (P_SKILL(FFORM_SHII_CHO) >= P_BASIC){
			if (u.fightingForm == FFORM_SHII_CHO ||
				(u.fightingForm == FFORM_SHIEN && (!uarm || is_light_armor(uarm)))
				) use_skill(FFORM_SHIEN, 1);
		}
	}

	/* sometimes projectiles have an early return */
	/* Sansara Mirror */
	if (rn2(2) && (
		(youdef && (
		(uwep && uwep->oartifact == ART_SANSARA_MIRROR) || (uswapwep && uswapwep->oartifact == ART_SANSARA_MIRROR)
		))
		||
		(!youdef && (
		(MON_WEP(mdef) && MON_WEP(mdef)->oartifact == ART_SANSARA_MIRROR) || (MON_SWEP(mdef) && MON_SWEP(mdef)->oartifact == ART_SANSARA_MIRROR)
		))
		))
	{
		/* message */
		if (vis) {
			pline("Sansara twists and reflects.");
		}
		/* return to sender */
		*pdx *= -1;
		*pdy *= -1;
		*prange = *prange2;
		return MM_MISS;
	}
	/* blaster bolts and laser beams are reflected by regular reflection */
	else if ((thrownobj->otyp == LASER_BEAM || thrownobj->otyp == BLASTER_BOLT || thrownobj->otyp == HEAVY_BLASTER_BOLT)
		&& (youdef ? Reflecting : mon_reflects(mdef, (char *)0)))
	{
		boolean shienuse = FALSE;
		/* if the player is using Shien lightsaber form, they can direct the reflection */
		if (youdef && uwep && is_lightsaber(uwep) && litsaber(uwep)
			&& u.fightingForm == FFORM_SHIEN && (!uarm || is_light_armor(uarm)
			&& rn2(3) < max((min(P_SKILL(u.fightingForm), P_SKILL(weapon_type(uwep)))) - 1, 1))) {
			You("reflect the %s with your lightsaber!", doname(thrownobj));
			if (getdir("Redirect it which way?"))
				shienuse = TRUE;
		}
		if (youdef && shienuse && (u.dx || u.dy)) {
			*pdx = u.dx;
			*pdy = u.dy;
			*prange = *prange2;
		}
		else if (youdef || mdef->mfaction != FRACTURED){
			*pdx *= -1;
			*pdy *= -1;
		}
		else {
			int i = rn2(8);
			*pdx = xdir[i];
			*pdy = ydir[i];
		}
		return MM_MISS;
	}
	/* the player has a chance to burn some projectiles (not blaster bolts or laser beams) out of the air with a lightsaber */
	else if (!(thrownobj->otyp == LASER_BEAM || thrownobj->otyp == BLASTER_BOLT || thrownobj->otyp == HEAVY_BLASTER_BOLT)
		&& youdef && uwep && is_lightsaber(uwep) && litsaber(uwep)
		&& ((u.fightingForm == FFORM_SHIEN && (!uarm || is_light_armor(uarm))) ||
		(u.fightingForm == FFORM_SORESU && (!uarm || is_light_armor(uarm) || is_medium_armor(uarm)))
		&& 
		rn2(3) < max((min(P_SKILL(u.fightingForm), P_SKILL(weapon_type(uwep))))-1,1))
		)
	{
		You("burn %s out of the %s!", doname(thrownobj), (Underwater || Is_waterlevel(&u.uz)) ? "water" : "air");
		obfree(thrownobj, (struct obj*) 0);
		tmp_at(DISP_END, 0);
		return MM_HIT;
	}

	/* unicorns */
	if (thrownobj->oclass == GEM_CLASS && is_unicorn(pd)) {
		if (youdef)
		{
			if (thrownobj->otyp > LAST_GEM) {
				You("catch the %s.", xname(thrownobj));
				You("are not interested in %s junk.",
					s_suffix(mon_nam(magr)));
				makeknown(thrownobj->otyp);
				dropy(thrownobj);
			}
			else {
				You("accept %s gift in the spirit in which it was intended.",
					s_suffix(mon_nam(magr)));
				(void)hold_another_object(thrownobj,
					"You catch, but drop, %s.", xname(thrownobj),
					"You catch:");
			}
			tmp_at(DISP_END, 0);
			return MM_HIT;
		}
		else if(youagr)
		{
			if (vis && mdef->mtame) {
				pline("%s catches and drops %s.", Monnam(mdef), the(xname(thrownobj)));
			}
			else {
				if (thrownobj->oartifact && vis){ //All gem artifacts need to force a "the" here.
					pline("%s catches the %s.", Monnam(mdef), xname(thrownobj));
				}
				else if (vis) {
					pline("%s catches %s.", Monnam(mdef), the(xname(thrownobj)));
				}
				if (!gem_accept(mdef, thrownobj)) {
					/* drop it there */
					*prange = *prange2 = 0;
					return MM_MISS;
				}
				else {
					tmp_at(DISP_END, 0);
					return MM_HIT;
				}
					
			}
			tmp_at(DISP_END, 0);
			return MM_HIT;
		}
		/* mvm unicorn gem throw-catch is not a thing. */
	}

	/* quest leader */
	if (youagr && quest_arti_hits_leader(thrownobj, mdef)) {
		/* not wakeup(), which angers non-tame monsters */
		mdef->msleeping = 0;
		mdef->mstrategy &= ~STRAT_WAITMASK;
		if (mdef->mcanmove) {
			tmp_at(DISP_END, 0);
			quest_art_swap(thrownobj, mdef);
			return MM_HIT;
		}
		else {
			/* drop it there */
			*prange = *prange2 = 0;
			return MM_MISS;
		}
	}

	/* you throwing to a pet */
	if (youagr && mdef->mtame) {
		if (mdef->mcanmove &&
			(!is_animal(mdef->data)) &&
			(!mindless_mon(mdef) || (mdef->data == &mons[PM_CROW_WINGED_HALF_DRAGON] && thrownobj->oartifact == ART_YORSHKA_S_SPEAR)) &&
			!fired
			) {
			if (vis) {
				pline("%s catches %s.", Monnam(mdef), the(xname(thrownobj)));
			}
			obj_extract_self(thrownobj);
			(void)mpickobj(mdef, thrownobj);
			if ((attacktype(mdef->data, AT_WEAP) ||
				attacktype(mdef->data, AT_DEVA) ||
				attacktype(mdef->data, AT_XWEP)
				) &&
				mdef->weapon_check == NEED_WEAPON) {
				mdef->weapon_check = NEED_HTH_WEAPON;
				(void)mon_wield_item(mdef);
			}
			m_dowear(mdef, FALSE);
			newsym(x(mdef), y(mdef));
			tmp_at(DISP_END, 0);
			return MM_HIT;
		}
	}

	/* Determine if the projectile hits */
	dieroll = rnd(20);
	accuracy = tohitval(magr, mdef, (struct attack *)0, thrownobj, launcher, (misfired ? 2 : 1), 0);

	if (accuracy > dieroll)
	{
		/* hit */
		/* (player-only) exercise dex */
		if (youagr)
			exercise(A_DEX, TRUE);
		/* call hmon to make the projectile hit */
		result = hmon2point0(magr, mdef, (struct attack *)0, thrownobj, launcher, (misfired ? 2 : 1), 1, TRUE, dieroll, FALSE, vis, &wepgone);

		/* deal with projectile */

		/* end tmp */
		tmp_at(DISP_END, 0);

		if (wepgone) {
			/*hmon destroyed it, we're already done*/;
		}
		else 
		/* projectiles other than magic stones
		 * sometimes disappear when thrown
		 * WAC - Spoon always disappears after doing damage
		 */
		if ((objects[thrownobj->otyp].oc_skill < P_NONE &&
			objects[thrownobj->otyp].oc_skill > -P_BOOMERANG &&
			thrownobj->oclass != GEM_CLASS &&
			!objects[thrownobj->otyp].oc_magic) ||
			(thrownobj->oartifact == ART_HOUCHOU)
			) {
			/* we were breaking 2/3 of everything unconditionally.
			 * we still don't want anything to survive unconditionally,
			 * but we need ammo to stay around longer on average.
			 */
			int broken, chance;
			if (thrownobj->oartifact && thrownobj->oartifact != ART_HOUCHOU){
				broken = 0;
			}
			else if (destroy ||
				(launcher && fired && (launcher->oartifact == ART_HELLFIRE || launcher->oartifact == ART_BOW_OF_SKADI)) ||
				thrownobj->oartifact == ART_HOUCHOU ||
				(fired && thrownobj->otyp == BULLET) || 
				(fired && thrownobj->otyp == SILVER_BULLET) || 
				(fired && thrownobj->otyp == SHOTGUN_SHELL) || 
				(fired && thrownobj->otyp == ROCKET) || 
				(fired && thrownobj->otyp == FRAG_GRENADE) || 
				(fired && thrownobj->otyp == GAS_GRENADE)) {
				broken = 1;
			}
			else {
				chance = 3 + greatest_erosion(thrownobj) - thrownobj->spe;
				if (chance > 1)
					broken = rn2(chance);
				else
					broken = !rn2(4);
				if (thrownobj->blessed && rnl(100) < 25)
					broken = 0;
			}
			if (broken) {
				if (*u.ushops)
					check_shop_obj(thrownobj, hitx, hity, TRUE);
				//#ifdef FIREARMS
				/*
				* Thrown grenades and explosive ammo used with the
				* relevant launcher explode rather than simply
				* breaking.
				*/
				if (fired && is_grenade(thrownobj)) {
					grenade_explode(thrownobj, hitx, hity, TRUE, 0);
				}
				else if (fired &&
					(objects[thrownobj->otyp].oc_dir & EXPLOSION)) {
					if (cansee(hitx, hity))
						pline("%s explodes in a ball of fire!", Doname2(thrownobj));
					else
						You_hear("an explosion");
					explode(hitx, hity, AD_FIRE, WEAPON_CLASS,
						d(3, 8), EXPL_FIERY, 1);
					obfree(thrownobj, (struct obj *)0);
				}
				else
					//#endif
					obfree(thrownobj, (struct obj *)0);

				/* check if explosions changed result */
				if (*hp(mdef) <= 0)
					result |= MM_DEF_DIED;
				return result;
			}
		}
		else
		{
			/* possibly damage the projectile -- at this point it is surviving */
			passive_obj2(magr, mdef, thrownobj, (struct attack *)0, (struct attack *)0);
		}

		return result;
	}

	return MM_MISS;
}

void
quest_art_swap(obj, mon)
struct obj * obj;
struct monst * mon;
{
	/* don't make game unwinnable if naive player throws artifact
	at leader.... */
	pline("%s catches %s.", Monnam(mon), the(xname(obj)));
	if (mon->mpeaceful) {
		if (Race_if(PM_ELF) && mon->data == &mons[PM_CELEBORN]){
			boolean next2u = monnear(mon, u.ux, u.uy);
			if (obj->oartifact == ART_PALANTIR_OF_WESTERNESSE &&
				yn("If you prefer, we can use the Palantir to secure the city, and you can use my bow in your travels.") == 'y'
				){
				obfree(obj, (struct obj *)0);
				obj = mksobj(ELVEN_BOW, TRUE, FALSE);
				obj = oname(obj, artiname(ART_BELTHRONDING));
				obj->oerodeproof = TRUE;
				obj->blessed = TRUE;
				obj->cursed = FALSE;
				obj->spe = rn2(8);
				pline("%s %s %s to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return_to_u(obj, u.ux, u.uy);
				obj = addinv(obj);	/* into your inventory */
				(void)encumber_msg();
			}
			else {
				pline("%s %s %s back to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return_to_u(obj, u.ux, u.uy);
				obj = addinv(obj);	/* back into your inventory */
				(void)encumber_msg();
			}
		}
		else if (Race_if(PM_DROW)){
			boolean next2u = monnear(mon, u.ux, u.uy);
			if (quest_status.got_thanks) finish_quest(obj);
			if (obj->oartifact == ART_SILVER_STARLIGHT && quest_status.got_thanks && mon->data == &mons[PM_ECLAVDRA] &&
				yn("Do you wish to take the Wrathful Spider, instead of this?") == 'y'
				){
				obfree(obj, (struct obj *)0);
				obj = mksobj(DROVEN_CROSSBOW, TRUE, FALSE);
				obj = oname(obj, artiname(ART_WRATHFUL_SPIDER));
				obj->oerodeproof = TRUE;
				obj->blessed = TRUE;
				obj->cursed = FALSE;
				obj->spe = rn2(8);
				finish_quest(obj);
				pline("%s %s %s to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return_to_u(obj, u.ux, u.uy);
				obj = addinv(obj);	/* into your inventory */
				(void)encumber_msg();
			}
			else if (obj->oartifact == ART_TENTACLE_ROD && quest_status.got_thanks && mon->data == &mons[PM_SEYLL_AUZKOVYN] &&
				yn("Do you wish to take the Crescent Blade, instead of this?") == 'y'
				){
				obfree(obj, (struct obj *)0);
				obj = mksobj(SABER, TRUE, FALSE);
				obj = oname(obj, artiname(ART_CRESCENT_BLADE));
				obj->oerodeproof = TRUE;
				obj->blessed = TRUE;
				obj->cursed = FALSE;
				obj->spe = rnd(7);
				finish_quest(obj);
				pline("%s %s %s to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return_to_u(obj, u.ux, u.uy);
				obj = addinv(obj);	/* into your inventory */
				(void)encumber_msg();
			}
			else if (obj->oartifact == ART_DARKWEAVER_S_CLOAK && quest_status.got_thanks && mon->data == &mons[PM_ECLAVDRA] &&
				yn("Do you wish to take Spidersilk, instead of this?") == 'y'
				){
				obfree(obj, (struct obj *)0);
				obj = mksobj(DROVEN_CHAIN_MAIL, TRUE, FALSE);
				obj = oname(obj, artiname(ART_SPIDERSILK));
				obj->oerodeproof = TRUE;
				obj->blessed = TRUE;
				obj->cursed = FALSE;
				obj->spe = rn2(8);
				finish_quest(obj);
				pline("%s %s %s to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return_to_u(obj, u.ux, u.uy);
				obj = addinv(obj);	/* into your inventory */
				(void)encumber_msg();
			}
			else if (obj->oartifact == ART_TENTACLE_ROD && quest_status.got_thanks && mon->data == &mons[PM_DARUTH_XAXOX] &&
				yn("Do you wish to take the Webweaver's Crook, instead of this?") == 'y'
				){
				obfree(obj, (struct obj *)0);
				obj = mksobj(FAUCHARD, TRUE, FALSE);
				obj = oname(obj, artiname(ART_WEBWEAVER_S_CROOK));
				obj->oerodeproof = TRUE;
				obj->blessed = TRUE;
				obj->cursed = FALSE;
				obj->spe = rnd(3) + rnd(4);
				finish_quest(obj);
				pline("%s %s %s to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return_to_u(obj, u.ux, u.uy);
				obj = addinv(obj);	/* into your inventory */
				(void)encumber_msg();
			}
			else {
				if (!quest_status.got_thanks) finish_quest(obj);	/* acknowledge quest completion */
				pline("%s %s %s back to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return_to_u(obj, u.ux, u.uy);
				obj = addinv(obj);	/* back into your inventory */
				(void)encumber_msg();
			}
		}
		else {
			boolean next2u = monnear(mon, u.ux, u.uy);

			finish_quest(obj);	/* acknowledge quest completion */
			pline("%s %s %s back to you.", Monnam(mon),
				(next2u ? "hands" : "tosses"), the(xname(obj)));
			if (!next2u) sho_obj_return_to_u(obj, u.ux, u.uy);
			obj = addinv(obj);	/* back into your inventory */
			(void)encumber_msg();
		}
	}
	else {
		/* angry leader caught it and isn't returning it */
		(void)mpickobj(mon, obj);
	}
	return;
}