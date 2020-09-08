/*	SCCS Id: @(#)xhityhelpers.c	3.4	2003/10/20	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"
#include "monflag.h"
#include "mextra.h"
#include "xhity.h"

extern boolean notonhead;
extern struct attack noattack;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/* unhide creatures, possibly with message, before they make an attack */
void
xmakingattack(magr, mdef, tarx, tary)
struct monst * magr;
struct monst * mdef;
int tarx;
int tary;
{
	boolean youagr = (magr == &youmonst);
	boolean youdef = (mdef == &youmonst);
	struct permonst * pa = youagr ? youracedata : magr->data;
	struct permonst * pd = youdef ? youracedata : mdef->data;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

boolean
magr_can_attack_mdef(magr, mdef, tarx, tary, active)
struct monst * magr;
struct monst * mdef;
int tarx;
int tary;
boolean active;
{
	boolean youagr = (magr == &youmonst);
	boolean youdef = (mdef == &youmonst);
	struct permonst * pa = youagr ? youracedata : magr->data;
	struct permonst * pd = youdef ? youracedata : mdef->data;

	/* cases where the agressor cannot make any attacks at all */
	/* player is invincible*/
	if (youdef && (u.uinvulnerable || u.spiritPColdowns[PWR_PHASE_STEP] >= moves + 20)) {
		/* monsters won't attack you */
		if (active) {
			/* only print messages if they were actively attacking you */
			if (magr == u.ustuck)
				pline("%s loosens its grip slightly.", Monnam(magr));
			else if (distmin(x(magr), y(magr), x(mdef), y(mdef)) <= 1) {
				if (canseemon(magr) || sensemon(magr))
					pline("%s starts to attack you, but pulls back.",
					Monnam(magr));
				else
					You_feel("%s move nearby.", something);
			}
		}
		return FALSE;
	}

	/* agr can't attack */
	if (cantmove(magr))
		return FALSE;

	/* agr has no line of sight to def */
	if (!clear_path(x(magr), y(magr), tarx, tary))
		return FALSE;

	/* madness only prevents your active attempts to hit things */
	if (youagr && active && madness_cant_attack(mdef))
		return FALSE;

	/* some creatures are limited in *where* they can attack */
	/* Grid bugs and Bebeliths cannot attack at an angle. */
	if ((pa->mtyp == PM_GRID_BUG || pa->mtyp == PM_BEBELITH)
		&& x(magr) != tarx && y(magr) != tary)
		return FALSE;

	/* limited attack angles (monster-agressor only) */
	if (!youagr && (
		pa->mtyp == PM_CLOCKWORK_SOLDIER || pa->mtyp == PM_CLOCKWORK_DWARF ||
		pa->mtyp == PM_FABERGE_SPHERE || pa->mtyp == PM_FIREWORK_CART ||
		pa->mtyp == PM_JUGGERNAUT || pa->mtyp == PM_ID_JUGGERNAUT))
	{
		if (x(magr) + xdir[(int)magr->mvar1] != tarx ||
			y(magr) + ydir[(int)magr->mvar1] != tary)
			return FALSE;
	}

	/* Monsters can't attack a player that's underwater unless the monster can swim; asymetric */
	if (youdef && Underwater && !mon_resistance(magr, SWIMMING))
		return FALSE;

	/* Monsters can't attack a player that's swallowed unless the monster *is* u.ustuck */
	if (youdef && u.uswallow) {
		if (magr != u.ustuck)
			return FALSE;
		/* they also know exactly where you are */
		/* ...if they are making an attack action. */
		if (active) {
			u.ustuck->mux = u.ux;
			u.ustuck->muy = u.uy;
		}
		/* if you're invulnerable, you're fine though */
		if (u.uinvulnerable || u.spiritPColdowns[PWR_PHASE_STEP] >= moves + 20)
			return FALSE; /* stomachs can't hurt you! */
	}
	/* While swallowed OR stuck, you can't attack other monsters */
	if (youagr && u.ustuck) {
		if (mdef != u.ustuck) {
			if (u.uswallow)		/* when swallowed, always not */
				return FALSE;
			else if (active)	/* when held, only active attacks aren't allowed to hit others */
				return FALSE;
		}
	}
	/* if we made it through all of that, then we can attack */
	return TRUE;
}


/* attack_checks()
 * 
 * the player is attempting to attack [mdef]
 * 
 * old behaviour: FALSE means it's OK to attack
 * 
 * new behaviour: returns
 * 0 : can not attack
 * 1 : attack normally
 * 2 : bloodthirsty forced attack (a la Stormbringer, not 'F')
 */
int
attack_checks(mdef, wep)
struct monst * mdef;
struct obj * wep;	/* uwep for attack(), null for kick_monster() */
{
	/* if you're close enough to attack, alert any waiting monster */
	mdef->mstrategy &= ~STRAT_WAITMASK;

	/* if you're overtaxed (or worse), you cannot attack */
	if (check_capacity("You cannot fight while so heavily loaded."))
		return ATTACKCHECK_NONE;

	/* certain "pacifist" monsters don't attack */
	if (Upolyd && noattacks(youracedata)) {
		You("have no way to attack monsters physically.");
		return ATTACKCHECK_NONE;
	}

	/* if you are swallowed, you can attack */
	if (u.uswallow && mdef == u.ustuck)
		return ATTACKCHECK_ATTACK;

	if (flags.forcefight) {
		/* Do this in the caller, after we checked that the monster
		* didn't die from the blow.  Reason: putting the 'I' there
		* causes the hero to forget the square's contents since
		* both 'I' and remembered contents are stored in .glyph.
		* If the monster dies immediately from the blow, the 'I' will
		* not stay there, so the player will have suddenly forgotten
		* the square's contents for no apparent reason.
		if (!canspotmon(mdef) &&
		!glyph_is_invisible(levl[u.ux+u.dx][u.uy+u.dy].glyph))
		map_invisible(u.ux+u.dx, u.uy+u.dy);
		*/
		return ATTACKCHECK_ATTACK;
	}

	/* Put up an invisible monster marker, but with exceptions for
	* monsters that hide and monsters you've been warned about.
	* The former already prints a warning message and
	* prevents you from hitting the monster just via the hidden monster
	* code below; if we also did that here, similar behavior would be
	* happening two turns in a row.  The latter shows a glyph on
	* the screen, so you know something is there.
	*/
	if (!canspotmon(mdef) &&
		!glyph_is_warning(glyph_at(u.ux + u.dx, u.uy + u.dy)) &&
		!glyph_is_invisible(levl[u.ux + u.dx][u.uy + u.dy].glyph) &&
		!(!Blind && mdef->mundetected && hides_under(mdef->data))) {
		pline("Wait!  There's %s there you can't see!",
			something);
		map_invisible(u.ux + u.dx, u.uy + u.dy);
		/* if it was an invisible mimic, treat it as if we stumbled
		* onto a visible mimic
		*/
		if (mdef->m_ap_type && !Protection_from_shape_changers) {
			if (!u.ustuck && !mdef->mflee && dmgtype(mdef->data, AD_STCK))
				u.ustuck = mdef;
		}
		if (!mdef->mpeaceful)
			wakeup2(mdef, TRUE); /* always necessary; also un-mimics mimics */
		return ATTACKCHECK_NONE;
	}

	if (mdef->m_ap_type && !Protection_from_shape_changers &&
		!sensemon(mdef) &&
		!glyph_is_warning(glyph_at(u.ux + u.dx, u.uy + u.dy))) {
		/* If a hidden mimic was in a square where a player remembers
		* some (probably different) unseen monster, the player is in
		* luck--he attacks it even though it's hidden.
		*/
		if (glyph_is_invisible(levl[mdef->mx][mdef->my].glyph)) {
			seemimic(mdef);
			return ATTACKCHECK_ATTACK;
		}
		stumble_onto_mimic(mdef);
		return ATTACKCHECK_NONE;
	}

	if (mdef->mundetected && !canseemon(mdef) && !sensemon(mdef) &&
		!glyph_is_warning(glyph_at(u.ux + u.dx, u.uy + u.dy)) &&
		!MATCH_WARN_OF_MON(mdef) &&
		(hides_under(mdef->data) || mdef->data->mlet == S_EEL)) {
		mdef->mundetected = mdef->msleeping = 0;
		newsym(mdef->mx, mdef->my);
		if (glyph_is_invisible(levl[mdef->mx][mdef->my].glyph)) {
			seemimic(mdef);
			return ATTACKCHECK_ATTACK;
		}
		if (!(Blind ? Blind_telepat : Unblind_telepat)) {
			struct obj *obj;

			if (Blind || (is_pool(mdef->mx, mdef->my, FALSE) && !Underwater))
				pline("Wait!  There's a hidden monster there!");
			else if ((obj = level.objects[mdef->mx][mdef->my]) != 0)
				pline("Wait!  There's %s hiding under %s!",
				an(l_monnam(mdef)), doname(obj));
			return ATTACKCHECK_NONE;
		}
	}

	/*
	* make sure to wake up a monster from the above cases if the
	* hero can sense that the monster is there.
	*/
	if ((mdef->mundetected || mdef->m_ap_type) && sensemon(mdef)) {
		mdef->mundetected = 0;
		wakeup2(mdef, TRUE);
	}

	/* generally, don't attack peaceful monsters */
	if (mdef->mpeaceful && !Confusion && !Hallucination && !Stunned) {
		/* Intelligent chaotic weapons (Stormbringer) want blood */
		/* NOTE:  now generalized to a flag, also, more lawful weapons than chaotic weps have it now :) */
		if (wep && arti_is_prop(wep, ARTI_BLOODTHRST)) {
			/* Don't show Stormbringer's message if attack is intended. */
			if (iflags.attack_mode != ATTACK_MODE_FIGHT_ALL)
				return ATTACKCHECK_BLDTHRST;
			else
				return ATTACKCHECK_ATTACK;
		}
		if (canspotmon(mdef)) {
			if (iflags.attack_mode == ATTACK_MODE_CHAT
				|| iflags.attack_mode == ATTACK_MODE_PACIFIST) {
				if (mdef->ispriest) {
					/* Prevent accidental donation prompt. */
					pline("%s mutters a prayer.", Monnam(mdef));
				}
				else if (!dochat(FALSE, u.dx, u.dy, 0)) {
					flags.move = 0;
				}
				return ATTACKCHECK_NONE;
			}
			else if (iflags.attack_mode == ATTACK_MODE_ASK){
				char qbuf[QBUFSZ];
#ifdef PARANOID
				char buf[BUFSZ];
				if (iflags.paranoid_hit) {
					Sprintf(qbuf, "Really attack %s? [no/yes]",
						mon_nam(mdef));
					getlin(qbuf, buf);
					(void)lcase(buf);
					if (strcmp(buf, "yes")) {
						flags.move = 0;
						return ATTACKCHECK_NONE;
					}
				}
				else {
#endif
					Sprintf(qbuf, "Really attack %s?", mon_nam(mdef));
					if (yn(qbuf) != 'y') {
						flags.move = 0;
						return ATTACKCHECK_NONE;
					}
#ifdef PARANOID
				}
#endif
			}
		}
	}

	/* attack checks specific to the pacifist attack mode */
	if (iflags.attack_mode == ATTACK_MODE_PACIFIST) {
		/* Being not in full control of yourself causes you to attack */
		if (Confusion || Hallucination || Stunned)
			return ATTACKCHECK_ATTACK;
		/* Some weapons just want blood */
		if (wep && arti_is_prop(wep, ARTI_BLOODTHRST))
			return ATTACKCHECK_BLDTHRST;
		/* Otherwise, be a pacifist. */
		You("stop for %s.", mon_nam(mdef));
		flags.move = 0;
		return ATTACKCHECK_NONE;
	}

	/* default case: you can attack */
	return ATTACKCHECK_ATTACK;
}

/* madness_cant_attack()
 * 
 * returns TRUE if because of the player's madness, they cannot attack mon.
 */
boolean
madness_cant_attack(mon)
struct monst * mon;
{
	if (mon->female && humanoid_torso(mon->data) && roll_madness(MAD_SANCTITY)){
		You("can't bring yourself to strike %s!", mon_nam(mon));
		return TRUE;
	}

	if ((mon->data->mlet == S_SNAKE
		|| mon->data->mlet == S_NAGA
		|| mon->mtyp == PM_COUATL
		|| mon->mtyp == PM_LILLEND
		|| mon->mtyp == PM_MEDUSA
		|| mon->mtyp == PM_MARILITH
		|| mon->mtyp == PM_MAMMON
		|| mon->mtyp == PM_SHAKTARI
		|| mon->mtyp == PM_DEMOGORGON
		|| mon->mtyp == PM_GIANT_EEL
		|| mon->mtyp == PM_ELECTRIC_EEL
		|| mon->mtyp == PM_KRAKEN
		|| mon->mtyp == PM_SALAMANDER
		|| mon->mtyp == PM_KARY__THE_FIEND_OF_FIRE
		|| mon->mtyp == PM_CATHEZAR
		) && roll_madness(MAD_OPHIDIOPHOBIA)){
		pline("You're afraid to go near that horrid serpent!");
		return TRUE;
	}

	if ((is_insectoid(mon->data) || is_arachnid(mon->data)) && roll_madness(MAD_ENTOMOPHOBIA)){
		pline("You're afraid to go near that frightful bug!");
		return TRUE;
	}

	if ((is_spider(mon->data)
		|| mon->mtyp == PM_SPROW
		|| mon->mtyp == PM_DRIDER
		|| mon->mtyp == PM_PRIESTESS_OF_GHAUNADAUR
		|| mon->mtyp == PM_AVATAR_OF_LOLTH
		) && roll_madness(MAD_ARACHNOPHOBIA)){
		pline("You're afraid to go near that terrifying spider!");
		return TRUE;
	}

	if (mon->female && humanoid_upperbody(mon->data) && roll_madness(MAD_ARACHNOPHOBIA)){
		You("can't bring yourself to strike %s!", mon_nam(mon));
		return TRUE;
	}

	if (is_aquatic(mon->data) && roll_madness(MAD_THALASSOPHOBIA)){
		pline("You're afraid to go near that sea monster!");
		return TRUE;
	}

	if (u.umadness&MAD_PARANOIA && !ClearThoughts && u.usanity < rnd(100)){
		You("attack %s's hallucinatory twin!", mon_nam(mon));
		return TRUE;
	}

	if ((mon->data->mlet == S_WORM
		|| attacktype(mon->data, AT_TENT)
		) && roll_madness(MAD_HELMINTHOPHOBIA)){
		pline("You're afraid to go near that wormy thing!");
		return TRUE;
	}
	return FALSE;
}

/* Note: caller must ascertain mtmp is mimicking... */
void
stumble_onto_mimic(mtmp)
struct monst *mtmp;
{
	const char *fmt = "Wait!  That's %s!",
		   *generic = "a monster",
		   *what = 0;

	if(!u.ustuck && !mtmp->mflee && dmgtype(mtmp->data,AD_STCK))
	    u.ustuck = mtmp;

	if (Blind) {
	    if (!Blind_telepat)
		what = generic;		/* with default fmt */
	    else if (mtmp->m_ap_type == M_AP_MONSTER)
		what = a_monnam(mtmp);	/* differs from what was sensed */
	} else {
	    int glyph = levl[u.ux+u.dx][u.uy+u.dy].glyph;

	    if (glyph_is_cmap(glyph) &&
		    (glyph_to_cmap(glyph) == S_hcdoor ||
		     glyph_to_cmap(glyph) == S_vcdoor))
		fmt = "The door actually was %s!";
	    else if (glyph_is_object(glyph) &&
		    glyph_to_obj(glyph) == GOLD_PIECE)
		fmt = "That gold was %s!";

	    /* cloned Wiz starts out mimicking some other monster and
	       might make himself invisible before being revealed */
	    if (mtmp->minvis && !See_invisible(mtmp->mx,mtmp->my))
		what = generic;
	    else
		what = a_monnam(mtmp);
	}
	if (what) pline(fmt, what);

	wakeup(mtmp, TRUE);	/* clears mimicking */
}


/*
 * It is unchivalrous for a knight to attack the defenseless or from behind.
 */
void
check_caitiff(mtmp)
struct monst *mtmp;
{
	//Animals and mindless creatures are always considered fair game
	if(mindless_mon(mtmp) || is_animal(mtmp->data))
		return;
	
	if (Role_if(PM_KNIGHT) && u.ualign.type == A_LAWFUL &&
	    (!mtmp->mcanmove || !mtmp->mnotlaugh || mtmp->msleeping ||
		(mtmp->mflee && mtmp->mtyp != PM_BANDERSNATCH && !mtmp->mavenge))){
		    You("caitiff!");
			if(u.ualign.record > 10) {
				u.ualign.sins++;
			    adjalign(-2); //slightly stiffer penalty
				u.hod++;
			}
			else if(u.ualign.record > -10) {
			    adjalign(-5); //slightly stiffer penalty
			}
			else{
			    adjalign(-5); //slightly stiffer penalty
				u.hod++;
			}
	}
/*	attacking peaceful creatures is bad for the samurai's giri */
	if (Role_if(PM_SAMURAI) && mtmp->mpeaceful){
        if(!(uarmh && uarmh->oartifact && uarmh->oartifact == ART_HELM_OF_THE_NINJA)){
          You("dishonorably attack the innocent!");
          u.ualign.sins++;
          u.ualign.sins++;
          u.hod++;
          adjalign(-1);
          if(u.ualign.record > -10) {
              adjalign(-4);
          }
        } else {
          You("dishonorably attack the innocent!");
          adjalign(1);
        }
	}
}

/*
 * Send in a demon pet for the hero.  Exercise wisdom.
 *
 * This function used to be inline to damageum(), but the Metrowerks compiler
 * (DR4 and DR4.5) screws up with an internal error 5 "Expression Too Complex."
 * Pulling it out makes it work.
 */
extern void
demonpet()
{
	int i;
	struct permonst *pm;
	struct monst *dtmp;

	pline("Some hell-p has arrived!");
	i = (!is_demon(youracedata) || !rn2(6)) 
	     ? ndemon(u.ualign.type) : NON_PM;
	pm = i != NON_PM ? &mons[i] : youracedata;
	if(pm->mtyp == PM_ANCIENT_OF_ICE || pm->mtyp == PM_ANCIENT_OF_DEATH) {
	    pm = rn2(4) ? &mons[PM_METAMORPHOSED_NUPPERIBO] : &mons[PM_ANCIENT_NUPPERIBO];
	}
	if ((dtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS)) != 0)
	    (void)tamedog(dtmp, (struct obj *)0);
	exercise(A_WIS, TRUE);
}

/*
 * Player uses theft attack against monster.
 *
 * If the target is wearing body armor, take all of its possessions;
 * otherwise, take one object.  [Is this really the behavior we want?]
 *
 * This routine implicitly assumes that there is no way to be able to
 * resist petfication (ie, be polymorphed into a xorn or golem) at the
 * same time as being able to steal (poly'd into nymph or succubus).
 * If that ever changes, the check for touching a cockatrice corpse
 * will need to be smarter about whether to break out of the theft loop.
 */
void
steal_it(mdef, mattk)
struct monst *mdef;
struct attack *mattk;
{
	struct obj *otmp, *stealoid, **minvent_ptr;
	long unwornmask;
	int petrifies = FALSE;
	char kbuf[BUFSZ];

	if (!mdef->minvent) return;		/* nothing to take */

	/* look for worn body armor */
	stealoid = (struct obj *)0;
	if (could_seduce(&youmonst, mdef, mattk)) {
	    /* find armor, and move it to end of inventory in the process */
	    minvent_ptr = &mdef->minvent;
	    while ((otmp = *minvent_ptr) != 0)
		if (otmp->owornmask & W_ARM) {
		    if (stealoid) panic("steal_it: multiple worn suits");
		    *minvent_ptr = otmp->nobj;	/* take armor out of minvent */
		    stealoid = otmp;
		    stealoid->nobj = (struct obj *)0;
		} else {
		    minvent_ptr = &otmp->nobj;
		}
	    *minvent_ptr = stealoid;	/* put armor back into minvent */
	}

	if (stealoid) {		/* we will be taking everything */
	    if (gender(mdef) == (int) u.mfemale &&
			youracedata->mlet == S_NYMPH)
		You("charm %s.  She gladly hands over her possessions.",
		    mon_nam(mdef));
	    else
		You("seduce %s and %s starts to take off %s clothes.",
		    mon_nam(mdef), mhe(mdef), mhis(mdef));
	}
	while ((otmp = mdef->minvent) != 0) {
	    /* take the object away from the monster */
	    obj_extract_self(otmp);
	    if ((unwornmask = otmp->owornmask) != 0L) {
			mdef->misc_worn_check &= ~unwornmask;
			if (otmp->owornmask & W_WEP) {
				setmnotwielded(mdef,otmp);
				MON_NOWEP(mdef);
			}
			if (otmp->owornmask & W_SWAPWEP){
				setmnotwielded(mdef,otmp);
				MON_NOSWEP(mdef);
			}
			otmp->owornmask = 0L;
			update_mon_intrinsics(mdef, otmp, FALSE, FALSE);

			if (otmp == stealoid)	/* special message for final item */
				pline("%s finishes taking off %s suit.",
				  Monnam(mdef), mhis(mdef));
	    }

		if(near_capacity() < calc_capacity(otmp->owt)){
			You("steal %s %s and drop it to the %s.",
				  s_suffix(mon_nam(mdef)), xname(otmp), surface(u.ux, u.uy));
			if(otmp->otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm]) && !uarmg && !Stone_resistance){
				Sprintf(kbuf, "stolen %s corpse", mons[otmp->corpsenm].mname);
				petrifies = TRUE;
			}
			dropy(otmp);
		} else {
	    /* give the object to the character */
			if(otmp->otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm]) && !uarmg && !Stone_resistance){
				Sprintf(kbuf, "stolen %s corpse", mons[otmp->corpsenm].mname);
				petrifies = TRUE;
			}
			otmp = hold_another_object(otmp, "You snatched but dropped %s.",
						   doname(otmp), "You steal: ");
		}
	    /* more take-away handling, after theft message */
	    if (unwornmask & W_WEP || unwornmask & W_SWAPWEP) {		/* stole wielded weapon */
		possibly_unwield(mdef, FALSE);
	    } else if (unwornmask & W_ARMG) {	/* stole worn gloves */
		mselftouch(mdef, (const char *)0, TRUE);
		if (mdef->mhp <= 0)	/* it's now a statue */
		    return;		/* can't continue stealing */
	    }
	    if (petrifies) {
			instapetrify(kbuf);
			break;		/* stop the theft even if hero survives */
	    }

	    if (!stealoid) break;	/* only taking one item */
	}
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* mattacku()
 * mattackm()
 *
 * Monster is attacking something. Use xattacky().
 */
int
mattacku(mtmp)
register struct monst *mtmp;
{
	return xattacky(mtmp, &youmonst, mtmp->mux, mtmp->muy);
}

int
mattackm(magr, mdef)
register struct monst *magr, *mdef;
{
	/* this needs both attacker and defender, currently */
	if (!magr || !mdef)
		return MM_MISS;

	return xattacky(magr, mdef, mdef->mx, mdef->my);
}

/* fightm()  -- mtmp fights some other monster
 *
 * Returns:
 *	0 - Monster did nothing.
 *	1 - If the monster made an attack.  The monster might have died.
 *
 * There is an exception to the above.  If mtmp has the hero swallowed,
 * then we report that the monster did nothing so it will continue to
 * digest the hero.
 */
int
fightm(mtmp)		/* have monsters fight each other */
	register struct monst *mtmp;
{
	register struct monst *mon, *nmon;
	int result, has_u_swallowed;
	boolean conflict = (Conflict && 
						couldsee(mtmp->mx,mtmp->my) && 
						(distu(mtmp->mx,mtmp->my) <= BOLT_LIM*BOLT_LIM) && 
						!resist(mtmp, RING_CLASS, 0, 0))
					|| mtmp->mberserk;
#ifdef LINT
	nmon = 0;
#endif
	/* perhaps the monster will resist Conflict */
	/* if(resist(mtmp, RING_CLASS, 0, 0))
	    return(0); */

	if ((mtmp->mtame || is_covetous(mtmp->data)) && !conflict)
	    return(0);

	if(u.ustuck == mtmp) {
	    /* perhaps we're holding it... */
	    if(itsstuck(mtmp))
		return(0);
	}
	has_u_swallowed = (u.uswallow && (mtmp == u.ustuck));

	for(mon = fmon; mon; mon = nmon) {
	    nmon = mon->nmon;
	    if(nmon == mtmp) nmon = mtmp->nmon;
	    /* Be careful to ignore monsters that are already dead, since we
	     * might be calling this before we've cleaned them up.  This can
	     * happen if the monster attacked a cockatrice bare-handedly, for
	     * instance.
	     */
	    if(mon != mtmp && !DEADMONSTER(mon)) {
		if(monnear(mtmp,mon->mx,mon->my)) {
		    if (!conflict && !mm_aggression(mtmp, mon))
		    	continue;
		    if(!u.uswallow && (mtmp == u.ustuck)) {
			if(!rn2(4)) {
			    pline("%s releases you!", Monnam(mtmp));
			    u.ustuck = 0;
			} else
			    break;
		    }

		    /* mtmp can be killed */
		    bhitpos.x = mon->mx;
		    bhitpos.y = mon->my;
		    notonhead = 0;
		    result = mattackm(mtmp,mon);

		    if (result & MM_AGR_DIED) return 1;	/* mtmp died */
		    /*
		     *  If mtmp has the hero swallowed, lie and say there
		     *  was no attack (this allows mtmp to digest the hero).
		     */
		    if (has_u_swallowed) return 0;

		    /* Allow attacked monsters a chance to hit back. Primarily
		     * to allow monsters that resist conflict to respond.
		     */
		    if ((result & MM_HIT) && !(result & MM_DEF_DIED) &&
				rn2(4) && mon->movement >= NORMAL_SPEED
			) {
				mon->movement -= NORMAL_SPEED;
				notonhead = 0;
				(void) mattackm(mon, mtmp);	/* return attack */
		    }
			
			//If was conflict and a miss, can continue to attack.  Otherwise, ignore you.
		    return (((result & MM_HIT) || !conflict) ? 1 : 0);
		}
	    }
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* expels()
 *
 * The player's engulfer "lets" them leave
 */
void
expels(mtmp, mdat, message)
register struct monst *mtmp;
register struct permonst *mdat; /* if mtmp is polymorphed, mdat != mtmp->data */
boolean message;
{
	if (message) {
		if (is_animal(mdat))
			You("get regurgitated!");
		else {
			char blast[40];
			register int i;

			blast[0] = '\0';
			for(i = 0; i < NATTK; i++)
				if(mdat->mattk[i].aatyp == AT_ENGL)
					break;
			if (mdat->mattk[i].aatyp != AT_ENGL)
			      impossible("Swallower has no engulfing attack?");
			else {
				if (is_whirly(mdat)) {
					switch (mdat->mattk[i].adtyp) {
						case AD_ELEC:
							Strcpy(blast,
						      " in a shower of sparks");
							break;
						case AD_COLD:
							Strcpy(blast,
							" in a blast of frost");
							break;
					}
				} else
					Strcpy(blast, " with a squelch");
				You("get expelled from %s%s!",
				    mon_nam(mtmp), blast);
			}
		}
	}
	unstuck(mtmp);	/* ball&chain returned in unstuck() */
	mnexto(mtmp);
	newsym(u.ux,u.uy);
	spoteffects(TRUE);
	/* to cover for a case where mtmp is not in a next square */
	if(um_dist(mtmp->mx,mtmp->my,1))
		pline("Brrooaa...  You land hard at some distance.");
}

/* diseasemu()
 *
 * Some kind of monster mdat tries to make the player ill
 */
boolean
diseasemu(mdat)
struct permonst *mdat;
{
	if (Sick_resistance) {
		You_feel("a slight illness.");
		return FALSE;
	}
	else {
		make_sick(Sick ? Sick / 2L + 1L : (long)rn1(ACURR(A_CON), 20),
			mdat->mname, TRUE, SICK_NONVOMITABLE);
		return TRUE;
	}
}

/* u_slow_down()
 *
 *  called when your intrinsic speed is taken away
 */
void
u_slow_down()
{
	if (HFast){
		HFast = 0L;
		if (!Fast)
			You("slow down.");
		else	/* speed boots */
			Your("quickness feels less natural.");
	}
	exercise(A_DEX, FALSE);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* mpoisons_sibj()
 * 
 * return how a poison attack was delivered
 * works for both player and monster attackers
 */
const char *
mpoisons_subj(mtmp, mattk)
struct monst *mtmp;
struct attack *mattk;
{
	if (mattk->aatyp == AT_WEAP || mattk->aatyp == AT_XWEP || mattk->aatyp == AT_DEVA || mattk->aatyp == AT_MARI) {
		struct obj *mwep = (mtmp == &youmonst) ? uwep : MON_WEP(mtmp);
		/* "Foo's attack was poisoned." is pretty lame, but at least
		it's better than "sting" when not a stinging attack... */
		return (!mwep || !mwep->opoisoned) ? "attack" : "weapon";
	}
	else if(mattk->aatyp == AT_SRPR){
		if (mattk->adtyp == AD_SHDW){
		return "shadow blade";
	}
	else if (mattk->adtyp == AD_STAR){
		return "starlight rapier";
	}
		else if (mattk->adtyp == AD_BLUD){
			return "blade of rotted blood";
		}
		else {
			return "blade";
		}
	}
	else if (mattk->adtyp == AD_MERC){
		return "blade of metallic mercury";
	}
	else if (mattk->aatyp == AT_HODS){
		struct obj *mwep = uwep;
		/* "Foo's attack was poisoned." is pretty lame, but at least
		it's better than "sting" when not a stinging attack... */
		return (!mwep || !mwep->opoisoned) ? "attack" : "weapon";
	}
	else {
		return (mattk->aatyp == AT_TUCH || mattk->aatyp == AT_5SQR) ? "contact" :
			(mattk->aatyp == AT_GAZE || mattk->aatyp == AT_WDGZ) ? "gaze" :
			(mattk->aatyp == AT_CLAW) ? "claws" :
			(mattk->aatyp == AT_TENT) ? "tentacles" :
			(mattk->aatyp == AT_ENGL) ? "vapor" :
			(mattk->aatyp == AT_BITE || mattk->aatyp == AT_LNCK || mattk->aatyp == AT_5SBT) ? "bite" :
			(mattk->aatyp == AT_NONE) ? "attack" :
			"sting";
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* sleep_monst()
 *
 * `mon' is hit by a sleep attack; return 1 if it's affected, 0 otherwise
 */
int
sleep_monst(mon, amt, how)
struct monst *mon;
int amt, how;
{
	if (resists_sleep(mon) ||
		(how >= 0 && resist(mon, (char)how, 0, NOTELL))) {
		shieldeff(mon->mx, mon->my);
	}
	else if (mon->mcanmove) {
		amt += (int)mon->mfrozen;
		if (amt > 0) {	/* sleep for N turns */
			mon->mcanmove = 0;
			mon->mfrozen = min(amt, 127);
		}
		else {		/* sleep until awakened */
			mon->msleeping = 1;
		}
		return 1;
	}
	return 0;
}

/* slept_monst()
 *
 * sleeping grabber releases, engulfer doesn't; don't use for paralysis!
 */
void
slept_monst(mon)
struct monst *mon;
{
	if ((mon->msleeping || !mon->mcanmove) && mon == u.ustuck &&
		!sticks(&youmonst) && !u.uswallow) {
		pline("%s grip relaxes.", s_suffix(Monnam(mon)));
		unstuck(mon);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* hurtarmor()
 *
 * Monster damages player's armor
 */
void
hurtarmor(attk)
int attk;
{
	int	hurt;

	switch(attk) {
	    /* 0 is burning, which we should never be called with */
	    case AD_RUST: hurt = 1; break;
	    case AD_CORR: hurt = 3; break;
	    default: hurt = 2; break;
	}

	/* What the following code does: it keeps looping until it
	 * finds a target for the rust monster.
	 * Head, feet, etc... not covered by metal, or covered by
	 * rusty metal, are not targets.  However, your body always
	 * is, no matter what covers it.
	 */
	while (1) {
	    switch(rn2(5)) {
	    case 0:
		if (!uarmh || !rust_dmg(uarmh, xname(uarmh), hurt, FALSE, &youmonst))
			continue;
		break;
	    case 1:
		if (uarmc) {
		    (void)rust_dmg(uarmc, xname(uarmc), hurt, TRUE, &youmonst);
		    break;
		}
		/* Note the difference between break and continue;
		 * break means it was hit and didn't rust; continue
		 * means it wasn't a target and though it didn't rust
		 * something else did.
		 */
		if (uarm)
		    (void)rust_dmg(uarm, xname(uarm), hurt, TRUE, &youmonst);
#ifdef TOURIST
		else if (uarmu)
		    (void)rust_dmg(uarmu, xname(uarmu), hurt, TRUE, &youmonst);
#endif
		break;
	    case 2:
		if (!uarms || !rust_dmg(uarms, xname(uarms), hurt, FALSE, &youmonst))
		    continue;
		break;
	    case 3:
		if (!uarmg || !rust_dmg(uarmg, xname(uarmg), hurt, FALSE, &youmonst))
		    continue;
		break;
	    case 4:
		if (!uarmf || !rust_dmg(uarmf, xname(uarmf), hurt, FALSE, &youmonst))
		    continue;
		break;
	    }
	    break; /* Out of while loop */
	}
}

/* hurtmarmor()
 * 
 * modified from hurtarmor()
 * Something (you/monster) daamges a monster's armor
 */
void
hurtmarmor(mdef, attk)
struct monst *mdef;
int attk;
{
	int	hurt;
	struct obj *target;

	switch (attk) {
		/* 0 is burning, which we should never be called with */
	case AD_RUST: hurt = 1; break;
	case AD_CORR: hurt = 3; break;
	default: hurt = 2; break;
	}
	/* What the following code does: it keeps looping until it
	* finds a target for the rust monster.
	* Head, feet, etc... not covered by metal, or covered by
	* rusty metal, are not targets.  However, your body always
	* is, no matter what covers it.
	*/
	while (1) {
		switch (rn2(5)) {
		case 0:
			target = which_armor(mdef, W_ARMH);
			if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
				continue;
			break;
		case 1:
			target = which_armor(mdef, W_ARMC);
			if (target) {
				(void)rust_dmg(target, xname(target), hurt, TRUE, mdef);
				break;
			}
			if ((target = which_armor(mdef, W_ARM)) != (struct obj *)0) {
				(void)rust_dmg(target, xname(target), hurt, TRUE, mdef);
#ifdef TOURIST
			}
			else if ((target = which_armor(mdef, W_ARMU)) != (struct obj *)0) {
				(void)rust_dmg(target, xname(target), hurt, TRUE, mdef);
#endif
			}
			break;
		case 2:
			target = which_armor(mdef, W_ARMS);
			if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
				continue;
			break;
		case 3:
			target = which_armor(mdef, W_ARMG);
			if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
				continue;
			break;
		case 4:
			target = which_armor(mdef, W_ARMF);
			if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
				continue;
			break;
		}
		break; /* Out of while loop */
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* attk_protection()
 * 
 * Returns types of armor needed to prevent specified attack from touching its target
 *   ex) gloves makes claw attacks not touch a cockatrice
 */
long
attk_protection(aatyp)
int aatyp;
{
	long w_mask = 0L;

	switch (aatyp) {
	case AT_NONE:
	case AT_SPIT:
	case AT_EXPL:
	case AT_BOOM:
	case AT_GAZE:
	case AT_WDGZ:
	case AT_BREA:
	case AT_MAGC:
	case AT_MMGC:
	case AT_BEAM:
	case AT_SRPR:
	case AT_WISP:
	case AT_REND:		/* If the previous attacks were OK, this one is too */
		w_mask = ~0L;		/* special case; no defense needed */
		break;
	case AT_CLAW:
	case AT_LRCH:
	case AT_TUCH:
	case AT_5SQR:
	case AT_WEAP:
	case AT_XWEP:
	case AT_MARI:
	case AT_DEVA:
		w_mask = W_ARMG;	/* caller needs to check for weapon */
		break;
	case AT_KICK:
		w_mask = W_ARMF;
		break;
	case AT_BUTT:
		w_mask = W_ARMH;
		break;
	case AT_HUGS:
		w_mask = (W_ARMC | W_ARMG); /* attacker needs both to be protected */
		break;
	case AT_BITE:
	case AT_LNCK:
	case AT_5SBT:
	case AT_STNG:
	case AT_ENGL:
	case AT_TENT:
	default:
		w_mask = 0L;		/* no defense available */
		break;
	}
	return w_mask;
}

/* attk_equip_slot()
 * The offensive version of attk_protection()
 * Returns equipment slot that would be hitting the defender
 *   ex) silver gloves make punches do silver-searing damage
 */
long
attk_equip_slot(aatyp)
int aatyp;
{
	/* some worn armor may be involved depending on the attack type */
	long slot = 0L;
	switch (aatyp)
	{
		/* gloves */
		/* caller needs to check for weapons */
	case AT_CLAW:
	case AT_HODS:
	case AT_DEVA:
	case AT_REND:
	case AT_WEAP:
	case AT_XWEP:
	case AT_MARI:
		slot = W_ARMG;
		break;
		/* boots */
	case AT_KICK:
		slot = W_ARMF;
		break;
		/* helm */
	case AT_BUTT:
		slot = W_ARMH;
		break;
	}
	return slot;
}


/* badtouch()
 * returns TRUE if [attk] will touch [mdef]
 */
boolean
badtouch(magr, mdef, attk, weapon)
struct monst * magr;
struct monst * mdef;
struct attack * attk;
struct obj * weapon;
{
	long slot = attk_protection(attk->aatyp);
	boolean youagr = (magr == &youmonst);

	if (/* not using a weapon -- assumes weapons will only be passed if making a weapon attack */
		(!weapon)
		&&
		/* slots aren't covered */
		((!slot || (slot != ~0L)) && (
		/* player */
		(youagr && (
		(!slot) ||
		((slot & W_ARM) && !uarm) ||
		((slot & W_ARMC) && !uarmc) ||
		((slot & W_ARMH) && !uarmh) ||
		((slot & W_ARMS) && !uarms) ||
		((slot & W_ARMG) && !uarmg) ||
		((slot & W_ARMF) && !uarmf) ||
		((slot & W_ARMU) && !uarmu)))
		||
		/* monster */
		(!youagr && ((slot & ~(magr->misc_worn_check)) || !slot))
		))
		/* not a damage type that doesn't actually contact */
		&& !(
		attk->adtyp == AD_SHDW ||
		attk->adtyp == AD_BLUD ||
		attk->adtyp == AD_MERC ||
		attk->adtyp == AD_STAR
		)
		)
		return TRUE;	// will touch

	/* else won't touch */
	return FALSE;
}

/* safe_attack()
 * 
 * returns FALSE if [attk] will result in death for [magr]
 * due to:
 *  - cockatrice stoning
 *  - eating a fatal corpse
 */
boolean
safe_attack(magr, mdef, attk, weapon, pa, pd)
struct monst * magr;
struct monst * mdef;
struct attack * attk;
struct obj * weapon;
struct permonst * pa;
struct permonst * pd;
{
	long slot = attk_protection(attk->aatyp);
	boolean youagr = (magr == &youmonst);

	/* if there is no defender, it's safe */
	if (!mdef)
		return TRUE;

	/* Touching is fatal */
	if (touch_petrifies(pd) && !(Stone_res(magr))
		&& badtouch(magr, mdef, attk, weapon)
		)
		return FALSE;	// don't attack

	/* consuming the defender is fatal */
	if ((is_deadly(pd) || 
		((pd->mtyp == PM_GREEN_SLIME || pd->mtyp == PM_FLUX_SLIME) &&
			!(Change_res(magr)
			|| pa->mtyp == PM_GREEN_SLIME
			|| pa->mtyp == PM_FLUX_SLIME
			|| is_rider(pa)
			|| resists_poly(pa)))
		) && (
		((attk->aatyp == AT_BITE || attk->aatyp == AT_LNCK || attk->aatyp == AT_5SBT) && is_vampire(pa)) ||
		(attk->aatyp == AT_ENGL && attk->adtyp == AD_DGST)
		))
		return FALSE;	// don't attack

	/* otherwise, it is safe(ish) to attack */
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* beastmastery()
 * mountedCombat()
 *
 * returns the accuracy bonus a pet/mount gets from the player's skill
 */
int
beastmastery()
{
	int bm;
	switch (P_SKILL(P_BEAST_MASTERY)) {
	case P_ISRESTRICTED: bm = 0; break;
	case P_UNSKILLED:    bm = 0; break;
	case P_BASIC:        bm = 2; break;
	case P_SKILLED:      bm = 5; break;
	case P_EXPERT:       bm = 10; break;
	}
	if ((uwep && uwep->oartifact == ART_CLARENT) || (uswapwep && uswapwep->oartifact == ART_CLARENT))
		bm *= 2;
	return bm;
}
int
mountedCombat()
{
	int bm;
	switch (P_SKILL(P_RIDING)) {
	case P_ISRESTRICTED: bm = 0; break;
	case P_UNSKILLED:    bm = 0; break;
	case P_BASIC:        bm = 2; break;
	case P_SKILLED:      bm = 5; break;
	case P_EXPERT:       bm = 10; break;
	}
	return bm;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* obj_silver_searing()
 *
 * returns TRUE if object sears silver-haters
 * 
 * This includes only silver items, not jade
 */
boolean
obj_silver_searing(obj)
struct obj * obj;
{
	if (!obj)
		return FALSE;

	if (is_lightsaber(obj) && litsaber(obj))
		return FALSE;

	if ((obj->obj_material == SILVER) ||
		(obj->oclass == RING_CLASS && obj->ohaluengr
		&& (isEngrRing(obj->otyp) || isSignetRing(obj->otyp))
		&& obj->oward >= LOLTH_SYMBOL && obj->oward <= LOST_HOUSE) ||
		(obj->otyp == SHURIKEN && !flags.mon_moving && uwep && uwep->oartifact == ART_SILVER_STARLIGHT)	// THIS IS BAD AND SHOULD BE DONE DIFFERENTLY
		)
		return TRUE;

	return FALSE;
}
/* obj_jade_searing()
 *
 * returns TRUE if object sears silver-haters
 * 
 * This includes only jade items, not silver
 */
boolean
obj_jade_searing(obj)
struct obj * obj;
{
	if (!obj)
		return FALSE;

	static short jadeRing = 0;
	if (!jadeRing) jadeRing = find_jade_ring();

	if (is_lightsaber(obj) && litsaber(obj))
		return FALSE;

	if (
		(obj->otyp == JADE) ||
		(obj->oclass == RING_CLASS && obj->otyp == jadeRing) ||
		(obj->obj_material == GEMSTONE && !obj_type_uses_ovar1(obj) && !obj_art_uses_ovar1(obj) && obj->ovar1 == JADE)
		)
		return TRUE;

	return FALSE;
}

/* hatesobjdmg()
 * 
 * Calculates a damage roll from [mdef] being seared by [otmp]
 * Counts silver, jade, iron, holy, unholy
 * Does not print messages
 * 
 */
int
hatesobjdmg(mdef, otmp)
struct monst * mdef;
struct obj * otmp;
{
	//boolean youagr = (magr == &youmonst);
	boolean youdef = (mdef == &youmonst);
	//struct permonst * pa = youagr ? youracedata : magr->data;
	struct permonst * pd = youdef ? youracedata : mdef->data;

	boolean vulnerable = insubstantial(pd);
#define vd(n, x)	(vulnerable ? (n*x) : d(n, x))
	int diesize;
	int ndice;

	int dmg = 0;

	if (!otmp || !mdef)
		return 0;

	if (hates_silver(pd) && !(youdef && u.sealsActive&SEAL_EDEN)
		&& (obj_silver_searing(otmp) || obj_jade_searing(otmp))) {
		/* default: 1d20 */
		ndice = 1;
		diesize = 20;
		/* special cases */
		if (otmp->oartifact == ART_PEN_OF_THE_VOID && mvitals[PM_ACERERAK].died > 0)
			ndice = 2;
		else if (otmp->oartifact == ART_SILVER_STARLIGHT)
			ndice = 2;
		else if(otmp->otyp == KHAKKHARA)
			ndice = rnd(3);
		/* calculate */
		dmg += vd(ndice, diesize);
	}
	if (hates_iron(pd) &&
		otmp->obj_material == IRON &&
		!(is_lightsaber(otmp) && litsaber(otmp))) {
		/* default: 1d(XL) */
		ndice = 1;
		diesize = mlev(mdef);
		/* special cases */
		if (otmp->otyp == KHAKKHARA)
			ndice = rnd(3);
		/* calculate */
		dmg += vd(ndice, diesize);
	}
	if (hates_holy_mon(mdef) &&
		otmp->blessed) {
		/* default: 1d4 */
		ndice = 1;
		diesize = 4;
		/* special cases that don't affect dice */
		if (otmp->oartifact == ART_EXCALIBUR ||
			otmp->oartifact == ART_LANCE_OF_LONGINUS)
			dmg += vd(3, 7);
		else if (otmp->oartifact == ART_GODHANDS)
			dmg += 7;
		else if (otmp->oartifact == ART_JINJA_NAGINATA)
			dmg += vd(1, 12);
		else if (otmp->oartifact == ART_HOLY_MOONLIGHT_SWORD && !otmp->lamplit)
			dmg += vd(1, 10) + otmp->spe;
		else if (otmp->oartifact == ART_VAMPIRE_KILLER)
			dmg += 7;
		/* special cases that do affect dice */
		if (otmp->oartifact == ART_AMHIMITL)
			ndice = 3;
		else if (otmp->oartifact == ART_ROD_OF_SEVEN_PARTS)
			diesize = 20;
		else if (otmp->otyp == KHAKKHARA)
			ndice = rnd(3);
		/* gold has a particular affinity to blessings and curses */
		if (otmp->obj_material == GOLD &&
			!(is_lightsaber(otmp) && litsaber(otmp))) {
			diesize = 20;
		}
		/* calculate dice */
		dmg += vd(ndice, diesize);
	}
	if (hates_unholy_mon(mdef) &&
		is_unholy(otmp)) {
		/* default: 1d9 */
		ndice = 1;
		diesize = 9;
		/* special cases */
		if (otmp->oartifact == ART_STORMBRINGER)
			ndice = 4; //Extra unholy (4d9 vs excal's 3d7)
		else if (otmp->oartifact == ART_GODHANDS)
			dmg += 9;
		else if (otmp->oartifact == ART_DIRGE)
			dmg += 6;
		else if (otmp->oartifact == ART_LANCE_OF_LONGINUS)
			ndice = 3;
		else if (otmp->oartifact == ART_SCEPTRE_OF_THE_FROZEN_FLOO)
		{	ndice = 0; dmg += 8; } // add directly; no dice rolled
		else if (otmp->oartifact == ART_ROD_OF_SEVEN_PARTS)
			diesize = 20;
		else if (otmp->oartifact == ART_AMHIMITL)
		{	ndice = 3; diesize = 4; }
		else if (otmp->oartifact == ART_TECPATL_OF_HUHETOTL) /* SCOPECREEP: add ART_TECPATL_OF_HUHETOTL to is_unholy() macro */
		{	ndice = (otmp->cursed ? 4 : 2); diesize = 4; }
		else if (otmp->otyp == KHAKKHARA)
			ndice = rnd(3);
		/* gold has a particular affinity to blessings and curses */
		if (otmp->obj_material == GOLD &&
			!(is_lightsaber(otmp) && litsaber(otmp))) {
			ndice *= 2;
		}
		/* calculate */
		if (ndice)
			dmg += vd(ndice, diesize);
	}

	/* the Rod of Seven Parts gets a bonus vs holy and unholy when uncursed */
	if (otmp->oartifact == ART_ROD_OF_SEVEN_PARTS
		&& !otmp->blessed && !otmp->cursed
		&& (hates_holy_mon(mdef) || hates_unholy_mon(mdef))
		){
		dmg += vd(1, 10);
	}

	/* Glamdring sears orcs and demons */
	if (otmp->oartifact == ART_GLAMDRING &&
		(is_orc(pd) || is_demon(pd)))
		dmg += vd(1, 20);

	/* The Veioistafur stave hurts sea creatures */
	if (otmp->obj_material == WOOD && otmp->otyp != MOON_AXE
		&& (otmp->oward & WARD_VEIOISTAFUR) && pd->mlet == S_EEL) {
		dmg += vd(1, 20);
	}

	/* The Lifehunt Scythe is occult */
	if (mdef && mdef->isminion){
		if (otmp->oartifact == ART_LIFEHUNT_SCYTHE)
			dmg += vd(4, 4) + otmp->spe;
	}
#undef vd
	return dmg;
}

/* hits_insubstantial()
 * 
 * Caller is responsible for checking insubstantial(mdef->data) first
 * so that this function can also be used for other cases that can cause a creature
 * to be insubstantial.
 *
 * returns non-zero if [magr] attacking [mdef] with [attk] hits,
 * specifically in the case of [mdef] being insubstantial (as a shade)
 * 
 * returns 1 if the attack should only do on-hit-effects for damage
 * (like silver-hating)
 * 
 * returns 2 if the attack should do full damage
 * (like Sunsword)
 */
int
hits_insubstantial(magr, mdef, attk, weapon)
struct monst * magr;
struct monst * mdef;
struct attack * attk;
struct obj * weapon;
{
	boolean youagr = (magr == &youmonst);
	boolean youdef = (mdef == &youmonst);
	struct permonst * pa = (magr ? (youagr ? youracedata : magr->data) : (struct permonst *)0);
	struct permonst * pd = youdef ? youracedata : mdef->data;

	/* Chupoclops makes all your attacks ethereal */
	if (youagr && u.sealsActive&SEAL_CHUPOCLOPS)
		return 2;

	/* no weapon */
	if (!weapon) {
		/* some worn armor may be involved depending on the attack type */
		struct obj * otmp;
		long slot = attk_equip_slot(attk ? attk->aatyp : 0);
		switch (magr ? slot : 0L)
		{
		case W_ARMG:
			otmp = (youagr ? uarmg : which_armor(magr, slot));
			break;
		case W_ARMF:
			otmp = (youagr ? uarmf : which_armor(magr, slot));
			break;
		case W_ARMH:
			otmp = (youagr ? uarmh : which_armor(magr, slot));
			break;
		default:
			otmp = (struct obj *)0;
			break;
		}
		if (otmp && arti_shining(otmp))
			return 2;

		if ((hates_silver(pd) && !(youdef && u.sealsActive&SEAL_EDEN)) && (
			(attk && attk->adtyp == AD_STAR)
			))
			return 2;

		if (has_blood_mon(mdef) &&
			attk && attk->adtyp == AD_BLUD)
			return 2;

		if ((hates_silver(pd) && !(youdef && u.sealsActive&SEAL_EDEN)) && (
			(youagr && u.sealsActive&SEAL_EDEN) ||
			(attk && attk->adtyp == AD_GLSS) ||
			(magr && is_silver_mon(magr)) ||
			obj_silver_searing(otmp) ||
			(youagr && slot == W_ARMG && uright && obj_silver_searing(uright)) ||
			(youagr && slot == W_ARMG && uleft && obj_silver_searing(uleft))
			))
			return 1;

		if (hates_iron(pd) && (
			(attk && attk->adtyp == AD_SIMURGH) ||
			(magr && is_iron_mon(magr)) ||
			(otmp && otmp->obj_material == IRON) ||
			(youagr && slot == W_ARMG && uright && uright->obj_material == IRON) ||
			(youagr && slot == W_ARMG && uleft && uleft->obj_material == IRON)
			))
			return 1;

		if (hates_holy_mon(mdef) && (
			(attk->adtyp == AD_ACFR) ||
			(magr && is_holy_mon(magr)) ||
			(otmp && otmp->blessed) ||
			(youagr && slot == W_ARMG && uright && uright->blessed) ||
			(youagr && slot == W_ARMG && uleft && uleft->blessed)
			))
			return 1;

		if (hates_unholy_mon(mdef) && (
			(magr && is_unholy_mon(magr)) ||
			(otmp && is_unholy(otmp)) ||
			(youagr && slot == W_ARMG && uright && is_unholy(uright)) ||
			(youagr && slot == W_ARMG && uleft && is_unholy(uleft))
			))
			return 1;

		return 0;
	}
	/* weapon */
	else {
		if (arti_shining(weapon))	/* why is this used for more things than artifacts? >_> */
			return 2;

		if (hatesobjdmg(mdef, weapon))
			return 1;

		if ((hates_silver(pd) && !(youdef && u.sealsActive&SEAL_EDEN)) && (
			weapon->otyp == MIRROR ||
			weapon->otyp == POT_STARLIGHT
			))
			return 1;

		if (is_undead_mon(mdef) && (
			(weapon->otyp == CLOVE_OF_GARLIC)	/* causes shades to flee */
			))
			return 1;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/* item destruction strings */
const char * const destroy_strings[] = {	/* also used in trap.c */
	"freezes and shatters", "freeze and shatter", "shattered potion",
	"boils and explodes", "boil and explode", "boiling potion",
	"catches fire and burns", "catch fire and burn", "burning scroll",
	"catches fire and burns", "catch fire and burn", "burning book",
	"turns to dust and vanishes", "turn to dust and vanish", "",
	"shoots a spray of sparks", "shoots sparks and arcing current", "discharging wand",
	"boils vigorously", "boil vigorously", "boiling potion"
};

/* destroy_item()
*
* Called when item(s) are supposed to be destroyed in a defender's inventory
*
* Works for player and monster mtmp
* Assumes both the defender is alive and existant when called
*
* Can return:
* MM_MISS		0x00	no items destroyed
* MM_HIT		0x01	item(s) destroyed
*
* Only allows lethal damage against the player, so this function can be called
* while voiding the return.
*/
int
destroy_item(mtmp, osym, dmgtyp)
struct monst * mtmp;
int osym;
int dmgtyp;
{
	boolean youdef = mtmp == &youmonst;
	struct permonst * data = (youdef) ? youracedata : mtmp->data;
	int vis = (youdef) ? TRUE : canseemon(mtmp);
	int ndestroyed = 0;
	struct obj *obj, *obj2;
	int dmg, xresist, skip;
	long i, cnt, quan;
	int dindx;
	const char *mult;

	if (osym == RING_CLASS && dmgtyp == AD_ELEC)
		return MM_MISS; /*Rings aren't destroyed by electrical damage anymore*/

	for (obj = (youdef ? invent : mtmp->minvent); obj; obj = obj2) {
		obj2 = obj->nobj;
		if (obj->oclass != osym) continue; /* test only objs of type osym */
		if (obj->oartifact) continue; /* don't destroy artifacts */
		if (obj->in_use && obj->quan == 1) continue; /* not available */
		xresist = skip = 0;
		dmg = dindx = 0;
		quan = 0L;

		switch (dmgtyp) {
			/* Cold freezes potions */
		case AD_COLD:
			if (osym == POTION_CLASS && !(
				obj->otyp == POT_OIL ||
				obj->oerodeproof /* shatterproof */
				)) {
				quan = obj->quan;
				dindx = 0;
				dmg = 4;
			}
			else skip++;
			break;
			/* Fire boils potions, burns scrolls, burns spellbooks */
		case AD_FIRE:
			xresist = (Fire_res(mtmp) && obj->oclass != POTION_CLASS);

			if (obj->oerodeproof && is_flammable(obj))	/* fireproof */
				skip++;
			if (obj->otyp == SCR_FIRE || obj->otyp == SCR_GOLD_SCROLL_OF_LAW
				|| obj->otyp == SPE_FIREBALL || obj->otyp == SPE_FIRE_STORM)
				skip++;
			if (objects[obj->otyp].oc_unique) {
				skip++;
				if (!Blind && vis)
					pline("%s glows a strange %s, but remains intact.",
					The(xname(obj)), hcolor("dark red"));
			}
			quan = obj->quan;
			switch (osym) {
			case POTION_CLASS:
				dindx = 1;
				dmg = 6;
				break;
			case SCROLL_CLASS:
				dindx = 2;
				dmg = 1;
				break;
			case SPBOOK_CLASS:
				dindx = 3;
				dmg = 6;
				break;
			default:
				skip++;
				break;
			}
			break;
			/* electricity sparks charges out of wands */
		case AD_ELEC:
			xresist = (Shock_res(mtmp));
			quan = obj->quan;
			if (osym == WAND_CLASS){
				if (obj->otyp == WAN_LIGHTNING)
					skip++;
				dindx = 5;
				dmg = 6;
			}
			else
				skip++;
			break;
			/* other damage types don't destroy items here */
		default:
			skip++;
			break;
		}
		/* destroy the item, if allowed */
		if (!skip) {
			if (obj->in_use) --quan; /* one will be used up elsewhere */
			int amt = (osym == WAND_CLASS) ? obj->spe : quan;
			/* approx 10% of items in the stack get destroyed */
			for (i = cnt = 0L; i < amt; i++) {
				if (!rn2(10)) cnt++;
			}
			/* No items destroyed? Skip */
			if (!cnt)
				continue;
			/* print message */
			if (vis) {
				if (cnt == quan || quan == 1)	mult = "";
				else if (cnt > 1)				mult = "Some of ";
				else							mult = "One of ";
				pline("%s%s %s %s!",
					mult,
					(youdef) ? ((mult[0] != '\0') ? "your" : "Your") : ((mult[0] != '\0') ? s_suffix(mon_nam(mtmp)) : s_suffix(Monnam(mtmp))),
					xname(obj),
					(cnt > 1L) ? destroy_strings[dindx * 3 + 1]
					: destroy_strings[dindx * 3]);
			}

			/* potion vapors */
			if (osym == POTION_CLASS && dmgtyp != AD_COLD) {
				if (!breathless(data) || haseyes(data)) {
					if (youdef)
						potionbreathe(obj);
					else
						/* no function for monster breathing potions */;
				}
			}
			/* destroy item */
			if (osym == WAND_CLASS)
				obj->spe -= cnt;
			else {
				if (obj == current_wand) current_wand = 0;	/* destroyed */
				for (i = 0; i < cnt; i++) {
					/* use correct useup function */
					if (youdef) useup(obj);
					else m_useup(mtmp, obj);
				}
			}
			ndestroyed += cnt;

			/* possibly deal damage */
			if (dmg) {
				/* you */
				if (youdef) {
					if (xresist)	You("aren't hurt!");
					else {
						const char *how = destroy_strings[dindx * 3 + 2];
						boolean one = (cnt == 1L || osym == WAND_CLASS);

						dmg = d(cnt, dmg);
						losehp(dmg,
							(one ? how : (const char *)makeplural(how)),
							(one ? KILLED_BY_AN : KILLED_BY));
						exercise(A_STR, FALSE);
						/* Let's not worry about properly returning if that killed you. If it did, it's moot. I think. */
						/* at the very least, the return value from this function is being ignored often enough it doesn't matter */
					}
				}
				/* monster */
				else {
					if (xresist);	// no message, reduce spam
					else {
						dmg = d(cnt, dmg);
						/* not allowed to be lethal */
						if (dmg >= mtmp->mhp)
							dmg = min(0, mtmp->mhp - 1);
						mtmp->mhp -= dmg;
					}
				}
			}
		}
	}
	if (ndestroyed && roll_madness(MAD_TALONS) && osym != WAND_CLASS){
		if (ndestroyed > 1)
			You("panic after some of your possessions are destroyed!");
		else You("panic after one of your possessions is destroyed!");
		HPanicking += 1 + rnd(6);
	}

	/* return if anything was destroyed */
	return (ndestroyed ? MM_HIT : MM_MISS);
}

/* Spun out into own function becase 1) it's a lot and 2) it may be useful elsewhere */

int
android_braindamage(dmg, magr, mdef, vis)
int dmg;
struct monst *magr;
struct monst *mdef;
boolean vis;
{
	boolean youdef = mdef == &youmonst;
	boolean youagr = magr == &youmonst;
	int duration, newres = 0;
	int extra_damage = 0;
	boolean petrifies = FALSE;
	struct obj *otmp, *otmp2;
	long unwornmask;
	switch(rnd(12)){
		case 1:
			duration = dmg <= 2 ? dmg+1 : dmg*10;
			if(youdef){
				Your("photoreceptor wires are %s!", dmg <= 2 ? "unplugged" : "severed");
				make_blinded(itimeout_incr(Blinded, duration), FALSE);
			} else {
				if(youagr){
					if(dmg <= 2)
						You("unplug %s photoreceptors!", hisherits(mdef));
					else You("sever %s photoreceptor wires!", hisherits(mdef));
				}
				mdef->mcansee = 0;
				if ((mdef->mblinded + duration) > 127)
					mdef->mblinded = 127;
				else mdef->mblinded += duration;
			}
			extra_damage = d(dmg, 4);
		break;
		case 2:
			if(youdef){
				Your("visual lookup table has been corrupted!");
				make_hallucinated(itimeout_incr(HHallucination, dmg*10), FALSE, 0L);
			} else {
				if(youagr)
					You("reprogram %s visual lookup table!", hisherits(mdef));
				mdef->mberserk = 1;
			}
			extra_damage = d(dmg, 6);
		break;
		case 3:
			if(youdef){
				Your("efferent network has been crosswired!");
				make_stunned(itimeout_incr(HStun, dmg*10), FALSE);
			} else {
				if(youagr)
					You("crosswire %s efferent network!", hisherits(mdef));
				mdef->mconf = 1;
			}
			extra_damage = d(dmg, 4);
		break;
		case 4:
			if(youdef){
				Your("processing array has been crosswired!");
				make_confused(itimeout_incr(HConfusion, dmg*10), FALSE);
			} else {
				if(youagr)
					You("crosswire %s processing array!", hisherits(mdef));
				mdef->mconf = 1;
			}
			extra_damage = d(dmg, 6);
		break;
		case 5:
			if(youdef){
				Your("secondary data store has been compromised!");
				for(int i = dmg; dmg > 0; dmg--){
					forget(10);
				}
			} else {
				if(youagr)
					You("smash %s secondary data store!", hisherits(mdef));
				if(mdef->mtame && (mdef->isminion || !(EDOG(mdef)->loyal))){//note: won't check loyal if the target is a minion
					if(mdef->mtame > dmg)
						mdef->mtame -= dmg;
					else mdef->mtame = 0;
				}
				if(mdef->mcrazed && rnd(20) < dmg){
					mdef->mcrazed = 0;
					mdef->mberserk = 0;
				}
			}
			extra_damage = d(dmg, 6);
		break;
		case 6:
			if(youdef){
				Your("fear response has been dialed up to maximum!");
				HPanicking += dmg*10L;
			} else {
				if(youagr)
					You("dial %s fear response up to maximum!", hisherits(mdef));
				monflee(mdef, dmg*10, FALSE, TRUE);
			}
			extra_damage = d(dmg, 6);
		break;
		case 7:
			if(youdef){
				Your("CPU has been damaged!");
				(void)adjattrib(A_INT, -dmg, FALSE);
			} else {
				if(youagr)
					You("smash %s CPU!", hisherits(mdef));
				mdef->mconf = 1;
			}
			extra_damage = d(dmg, 10);
		break;
		case 8:
			if(youdef){
				Your("heuristic subprocessor has been damaged!");
				(void)adjattrib(A_WIS, -dmg, FALSE);
			} else {
				if(youagr)
					You("smash %s heuristic subprocessor!", hisherits(mdef));
				mdef->mconf = 1;
			}
			extra_damage = d(dmg, 10);
		break;
		case 9:
			if(youdef){
				Your("nausea subroutine has been activated!");
				if(!Vomiting)
					make_vomiting(d(5,4), TRUE);
			} else {
				if(youagr)
					You("activate %s nausea subroutine!", hisherits(mdef));
				if(vis)
					pline("%s vomits!", Monnam(mdef));
				mdef->mcanmove = 0;
				if ((mdef->mfrozen + 3) > 127)
					mdef->mfrozen = 127;
				else mdef->mfrozen += 3;
			}
			extra_damage = d(dmg, 6);
		break;
		case 10:
			if(youdef){
				Your("efferent network has been damaged!");
				set_itimeout(&HFumbling, itimeout_incr(HFumbling, dmg*10));
			} else {
				if(youagr)
					You("strip %s efferent wires!", hisherits(mdef));
				mdef->mconf = 1;
			}
			extra_damage = d(dmg, 6);
		break;
		case 11:{
			if(youdef){
				Your("primary effector wiring has been compromised!");
				if((otmp = uwep)){
					pline("%s directs your %s to surrender your weapon!", Monnam(magr), mbodypart(mdef, ARM));
				} else if((otmp = uswapwep)){
					pline("%s directs your %s to surrender your secondary weapon!", Monnam(magr), mbodypart(mdef, ARM));
				} else if((otmp = uquiver)){
					pline("%s directs your %s to surrender your quivered weapon!", Monnam(magr), makeplural(mbodypart(mdef, ARM)));
				} else if((otmp = uarms)){
					pline("%s directs your %s to surrender your shield!", Monnam(magr), mbodypart(mdef, ARM));
				}
				if(otmp){
					remove_worn_item(otmp, TRUE);
					obj_extract_self(otmp);
					if(otmp->otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm]) && !which_armor(mdef, W_ARMG)){
						petrifies = TRUE;
					}
					mpickobj(magr, otmp); //may free otmp
					if (roll_madness(MAD_TALONS)){
						You("panic after giving away one of your possessions!");
						HPanicking += 1 + rnd(6);
					}
				}
			} else if(youagr){
				char kbuf[BUFSZ];
				You("hotwire %s primary effectors!", hisherits(mdef));
				if((otmp = MON_WEP(mdef))){
					You("force %s to surrender %s weapon!", himherit(mdef), hisherits(mdef));
				} else if((otmp = MON_SWEP(mdef))){
					You("force %s to surrender %s secondary weapon!", himherit(mdef), hisherits(mdef));
				} else if((otmp = which_armor(mdef, W_ARMS))){
					You("force %s to surrender %s shield!", himherit(mdef), hisherits(mdef));
				}
				if(otmp){
					/* take the object away from the monster */
					obj_extract_self(otmp);
					if ((unwornmask = otmp->owornmask) != 0L) {
						mdef->misc_worn_check &= ~unwornmask;
						if (otmp->owornmask & W_WEP) {
							setmnotwielded(mdef,otmp);
							MON_NOWEP(mdef);
						}
						if (otmp->owornmask & W_SWAPWEP){
							setmnotwielded(mdef,otmp);
							MON_NOSWEP(mdef);
						}
						otmp->owornmask = 0L;
						update_mon_intrinsics(mdef, otmp, FALSE, FALSE);
					}

					if(near_capacity() < calc_capacity(otmp->owt)){
						You("take %s %s and drop it to the %s.",
							  s_suffix(mon_nam(mdef)), xname(otmp), surface(u.ux, u.uy));
						if(otmp->otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm]) && !uarmg && !Stone_resistance){
							Sprintf(kbuf, "stolen %s corpse", mons[otmp->corpsenm].mname);
							petrifies = TRUE;
						}
						dropy(otmp); //may free otmp
					} else {
					/* give the object to the character */
						if(otmp->otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm]) && !uarmg && !Stone_resistance){
							Sprintf(kbuf, "stolen %s corpse", mons[otmp->corpsenm].mname);
							petrifies = TRUE;
						}
						otmp = hold_another_object(otmp, "You took but dropped %s.", doname(otmp), "You steal: ");  //may free otmp
					}
					/* more take-away handling, after theft message */
					if (unwornmask & W_WEP || unwornmask & W_SWAPWEP) {		/* stole wielded weapon */
						possibly_unwield(mdef, FALSE);
					}
					if (petrifies) {
						instapetrify(kbuf);
					}
				}
			} else {
				if((otmp = MON_WEP(mdef))){
					if(vis)
						pline("%s surrenders %s weapon!", Monnam(mdef), hisherits(mdef));
				} else if((otmp = MON_SWEP(mdef))){
					if(vis)
						pline("%s surrenders %s secondary weapon!", Monnam(mdef), hisherits(mdef));
				} else if((otmp = which_armor(mdef, W_ARMS))){
					if(vis)
						pline("%s surrenders %s shield!", Monnam(mdef), hisherits(mdef));
				}
				if(otmp){
					/* take the object away from the monster */
					obj_extract_self(otmp);
					if ((unwornmask = otmp->owornmask) != 0L) {
						mdef->misc_worn_check &= ~unwornmask;
						if (otmp->owornmask & W_WEP) {
							setmnotwielded(mdef,otmp);
							MON_NOWEP(mdef);
						}
						if (otmp->owornmask & W_SWAPWEP){
							setmnotwielded(mdef,otmp);
							MON_NOSWEP(mdef);
						}
						otmp->owornmask = 0L;
						update_mon_intrinsics(mdef, otmp, FALSE, FALSE);
					}

					if(otmp->otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm]) && !which_armor(mdef, W_ARMG)){
						petrifies = TRUE;
					}
					mpickobj(magr, otmp); //may free otmp
					
					/* more take-away handling, after theft message */
					if (unwornmask & W_WEP || unwornmask & W_SWAPWEP) {		/* stole wielded weapon */
						possibly_unwield(mdef, FALSE);
					}
				}
			}
			
			extra_damage = d(dmg, 4);
		}break;
		case 12:
			if(youdef){
				int drops = 0;
				Your("primary and secondary effector wiring has been compromised!");
				pline("%s directs your body to drop your possessions!", Monnam(magr));
				if (*u.ushops) sellobj_state(SELL_NORMAL); //Shopkeepers will steal your stuff!
				for(otmp = invent; otmp; otmp = otmp2) {
					otmp2 = otmp->nobj;
					drops += drop(otmp);
				}
				if (drops && roll_madness(MAD_TALONS)){
					You("panic after dropping your %s!", drops == 1 ? "property" : "possessions");
					HPanicking += 1 + rnd(6);
				}
			} else {
				if(youagr){
					You("hotwire %s primary and secondary effectors!", hisherits(mdef));
					You("force %s to drop %s possessions!", himherit(mdef), hisherits(mdef));
				} else {
					pline("%s drops %s possessions!", Monnam(mdef), hisherits(mdef));
				}
				otmp2 = mdef->minvent;
				while((otmp = otmp2)){
					otmp2 = otmp->nobj;
					if(!(otmp->owornmask&(~(W_WEP|W_SWAPWEP)))){
						obj_extract_self(otmp);
						mdrop_obj(mdef, otmp, FALSE);
					}
				}
			}
			extra_damage = d(dmg, 4);
		break;
	}
	//needs a last thought fades away-type message?
	newres = xdamagey(magr, mdef, &noattack, extra_damage);
	//Finally handle cockatrice corpses :(
	if(!youdef){
		if (unwornmask & W_ARMG) {	/* stole worn gloves */
			mselftouch(mdef, (const char *)0, TRUE);
			if(mdef->mhp <= 0)
				newres |= MM_DEF_DIED;
		}
	}
	if(!youagr){
		if (petrifies) {
			minstapetrify(magr, youdef); //checks stone resistance
			if(magr->mhp <= 0)
				newres |= MM_AGR_DIED;
		}
	}
	return newres;
}

boolean
nearby_targets(magr)
struct monst *magr;
{
	struct monst *mon;
	int x = x(magr),
		y = y(magr);
	boolean youagr = (magr == &youmonst);
	
	if(!youagr && !magr->mpeaceful)
		if(distmin(x, y, u.ux, u.uy) <= 2)
			return TRUE;
	
	for(mon = fmon;mon;mon = mon->nmon){
		if(DEADMONSTER(mon))
			continue;
		if(youagr && mon->mpeaceful)
			continue;
		if(!youagr && (mon->mpeaceful == magr->mpeaceful))
			continue;
		if(distmin(x, y, mon->mx, mon->my) <= 2)
			return TRUE;
	}
	return FALSE;
}