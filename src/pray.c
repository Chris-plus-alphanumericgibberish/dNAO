/*	SCCS Id: @(#)pray.c	3.4	2003/03/23	*/
/* Copyright (c) Benson I. Margulies, Mike Stephenson, Steve Linhart, 1989. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "xhity.h"
#include "gods.h"
#include "godlist.h"
#include "artifact.h"

extern const int monstr[];

STATIC_PTR int NDECL(prayer_done);
STATIC_DCL struct obj *NDECL(worst_cursed_item);
STATIC_DCL int NDECL(in_trouble);
STATIC_DCL void FDECL(fix_worst_trouble,(int));
STATIC_DCL void FDECL(pleased,(ALIGNTYP_P));
STATIC_DCL void FDECL(god_zaps_you, (int));
STATIC_DCL void FDECL(fry_by_god, (int));
STATIC_DCL void FDECL(consume_offering,(struct obj *));
STATIC_DCL void FDECL(eat_offering,(struct obj *, boolean, int));
STATIC_DCL void FDECL(burn_offering,(struct obj *, boolean));
STATIC_DCL boolean FDECL(water_prayer,(BOOLEAN_P));
STATIC_DCL boolean FDECL(blocked_boulder,(int,int));
static void NDECL(lawful_god_gives_angel);
static void FDECL(god_gives_pet,(int));
static void FDECL(god_gives_benefit,(ALIGNTYP_P));

/* simplify a few tests */
#define Cursed_obj(obj,typ) ((obj) && (obj)->otyp == (typ) && (obj)->cursed)

/*
 * Logic behind deities and altars and such:
 * + prayers are made to your god if not on an altar, and to the altar's god
 *   if you are on an altar
 * + If possible, your god answers all prayers, which is why bad things happen
 *   if you try to pray on another god's altar
 * + sacrifices work basically the same way, but the other god may decide to
 *   accept your allegiance, after which they are your god.  If rejected,
 *   your god takes over with your punishment.
 * + if you're in Gehennom, all messages come from Moloch
 */

/*
 *	Moloch, who dwells in Gehennom, is the "renegade" cruel god
 *	responsible for the theft of the Amulet from Marduk, the Creator.
 *	Moloch is unaligned.
 */
//definition of externs in you.h
const char	*Velka = "Velka, Goddess of Sin";
const char	*Moloch = "Moloch";
const char	*Morgoth = "Melkor";
const char	*MolochLieutenant = "Moloch, lieutenant of Melkor";
const char	*Silence = "The Silence";
const char	*Chaos = "Chaos";
const char	*DeepChaos = "Chaos, with Cosmos in chains";
const char	*tVoid = "the void";
const char	*Demiurge = "Yaldabaoth";
const char	*Sophia = "Pistis Sophia";
const char	*Other = "an alien god";
const char	*BlackMother = "the Black Mother";
const char	*Nodens = "Nodens";
const char	*DreadFracture = "the Dread Fracture";
const char	*AllInOne = "Yog-Sothoth";

static const char *godvoices[] = {
    "booms out",
    "thunders",
    "rings out",
    "booms",
};

static const char *yogvoices[] = {
    "bellow",
    "shout",
    "yell out",
    "whisper to yourself",
};

static const char *goatvoices[] = {
    "deep lowing all around you!",
    "bones crunching.",
    "a strange susurrus.",
    "the shrieking void!",
    "bleating all around!",
    "wind whipping through the trees!",
    "cacophonous bellowing!",
};

static const char *bokrugvoices[] = {
    "rising water.",
    "rippling waves.",
};

static const char *goattitles[] = {
    "the Black Goat",
    "the Black Goat of the Woods",
    "the Goat with a Thousand Young",
    "the Black Mother",
    "the Great Mother",
    "the Black Mist",
    "the Hungry Darkness",
    "the Fecund Darkness",
};

/* values calculated when prayer starts, and used when completed */
static int p_god;
static int p_trouble;
static int p_type;
/*
 -1 : rejection    
  0 : bad standing 
  1 : too soon     
  2 : cross-altar  
  3 : good prayer  
  */

#define PIOUS 20
#define DEVOUT 14
#define FERVENT 9
#define STRIDENT 4
#define ALIGNED 3
#define HALTING 1
#define NOMINAL 0
#define STRAYED -3
#define SINNED -8

/*
 * The actual trouble priority is determined by the order of the
 * checks performed in in_trouble() rather than by these numeric
 * values, so keep that code and these values synchronized in
 * order to have the values be meaningful.
 */

#define TROUBLE_STONED			19
#define TROUBLE_BLOOD_DROWN		18
#define TROUBLE_FROZEN_AIR		17
#define TROUBLE_SLIMED			16
#define TROUBLE_STRANGLED		15
#define TROUBLE_LAVA			14
#define TROUBLE_SICK			13
#define TROUBLE_STARVING		12
#define TROUBLE_HIT			 	11
#define TROUBLE_WIMAGE		 	10
#define TROUBLE_MORGUL		 	9
#define TROUBLE_MROT		 	8
#define TROUBLE_HPMOD		 	7
#define TROUBLE_LYCANTHROPE		 6
#define TROUBLE_COLLAPSING		 5
#define TROUBLE_STUCK_IN_WALL		 4
#define TROUBLE_CURSED_LEVITATION	 3
#define TROUBLE_UNUSEABLE_HANDS		 2
#define TROUBLE_CURSED_BLINDFOLD	 1

#define TROUBLE_PUNISHED	       (-1)
#define TROUBLE_FUMBLING	       (-2)
#define TROUBLE_CURSED_ITEMS	   (-3)
#define TROUBLE_SADDLE		       (-4)
#define TROUBLE_CARRY_CURSED	   (-5)
#define TROUBLE_TOHIT_CURSED	   (-6)
#define TROUBLE_AC_CURSED		   (-7)
#define TROUBLE_DAMAGE_CURSED	   (-8)
#define TROUBLE_BLIND		       (-9)
#define TROUBLE_POISONED	      (-10)
#define TROUBLE_WOUNDED_LEGS	  (-11)
#define TROUBLE_HUNGRY		      (-12)
#define TROUBLE_STUNNED		      (-13)
#define TROUBLE_CONFUSED	      (-14)
#define TROUBLE_HALLUCINATION	  (-15)

/* We could force rehumanize of polyselfed people, but we can't tell
   unintentional shape changes from the other kind. Oh well.
   3.4.2: make an exception if polymorphed into a form which lacks
   hands; that's a case where the ramifications override this doubt.
 */

/* Return 0 if nothing particular seems wrong, positive numbers for
   serious trouble, and negative numbers for comparative annoyances. This
   returns the worst problem. There may be others, and the gods may fix
   more than one.

This could get as bizarre as noting surrounding opponents, (or hostile dogs),
but that's really hard.
 */

STATIC_OVL int
in_trouble()
{
	struct obj *otmp;
	int i, j, count=0;

/* Borrowed from eat.c */

#define SATIATED	0
#define NOT_HUNGRY	1
#define HUNGRY		2
#define WEAK		3
#define FAINTING	4
#define FAINTED		5
#define STARVED		6

	/*
	 * major troubles
	 */
	if(Stoned) return(TROUBLE_STONED);
	if(Golded) return(TROUBLE_STONED);
	if(BloodDrown) return(TROUBLE_BLOOD_DROWN);
	if(FrozenAir) return(TROUBLE_FROZEN_AIR);
	if(Slimed) return(TROUBLE_SLIMED);
	if(Strangled) return(TROUBLE_STRANGLED);
	if(u.utrap && u.utraptype == TT_LAVA) return(TROUBLE_LAVA);
	if(Sick) return(TROUBLE_SICK);
	if(u.uhs >= WEAK && !Race_if(PM_INCANTIFIER)) return(TROUBLE_STARVING);
	if (Upolyd ? (u.mh <= 5 || u.mh*2 <= u.mhmax) :
		(u.uhp <= 5 || u.uhp*2 <= u.uhpmax)) return TROUBLE_HIT;
	if(u.wimage >= 10 && on_altar()) return(TROUBLE_WIMAGE);
	if(u.umorgul && on_altar()) return(TROUBLE_MORGUL);
	if(u.umummyrot && on_altar()) return(TROUBLE_MROT);
	if(u.uhpmod < -18 && on_altar()) return(TROUBLE_HPMOD);
	if(u.ulycn >= LOW_PM) return(TROUBLE_LYCANTHROPE);
	if(near_capacity() >= EXT_ENCUMBER && AMAX(A_STR)-ABASE(A_STR) > 3)
		return(TROUBLE_COLLAPSING);

	for (i= -1; i<=1; i++) for(j= -1; j<=1; j++) {
		if (!i && !j) continue;
		if (!isok(u.ux+i, u.uy+j) || IS_ROCK(levl[u.ux+i][u.uy+j].typ)
		    || (blocked_boulder(i,j) && !throws_rocks(youracedata) && !(u.sealsActive&SEAL_YMIR)))
			count++;
	}
	if (count == 8 && !Passes_walls)
		return(TROUBLE_STUCK_IN_WALL);

	if (stuck_ring(uleft, RIN_LEVITATION) ||
		stuck_ring(uright, RIN_LEVITATION))
		return(TROUBLE_CURSED_LEVITATION);
	if (nohands(youracedata) || !freehand()) {
	    /* for bag/box access [cf use_container()]...
	       make sure it's a case that we know how to handle;
	       otherwise "fix all troubles" would get stuck in a loop */
	    if (welded(uwep)) return TROUBLE_UNUSEABLE_HANDS;
	    if (Straitjacketed)
			return TROUBLE_UNUSEABLE_HANDS;
	    if (Upolyd && nohands(youracedata) && (!Unchanging ||
		    ((otmp = unchanger()) != 0 && otmp->cursed)))
		return TROUBLE_UNUSEABLE_HANDS;
	}
	if(Role_if(PM_BARD) && welded(uwep))
		return TROUBLE_UNUSEABLE_HANDS;
	if(Blindfolded && ublindf->cursed) return(TROUBLE_CURSED_BLINDFOLD);

	/*
	 * minor troubles
	 */
	if(Punished) return(TROUBLE_PUNISHED);
	if (Cursed_obj(uarmg, GAUNTLETS_OF_FUMBLING) ||
		Cursed_obj(uarmf, FUMBLE_BOOTS))
	    return TROUBLE_FUMBLING;
	if (worst_cursed_item()) return TROUBLE_CURSED_ITEMS;
#ifdef STEED
	if (u.usteed) {	/* can't voluntarily dismount from a cursed saddle */
	    otmp = which_armor(u.usteed, W_SADDLE);
	    if (Cursed_obj(otmp, SADDLE)) return TROUBLE_SADDLE;
	}
#endif

	if (u.ucarinc < 0) return(TROUBLE_CARRY_CURSED);
	if (u.uhitinc < 0) return(TROUBLE_TOHIT_CURSED);
	if (u.uacinc < 0) return(TROUBLE_AC_CURSED);
	if (u.udaminc < 0) return(TROUBLE_DAMAGE_CURSED);
	if (Blinded > 1 && haseyes(youracedata)) return(TROUBLE_BLIND);
	for(i=0; i<A_MAX; i++)
	    if(ABASE(i) < AMAX(i)) return(TROUBLE_POISONED);
	if(Wounded_legs
#ifdef STEED
		    && !u.usteed
#endif
				) return (TROUBLE_WOUNDED_LEGS);
	if(u.uhs >= HUNGRY && !Race_if(PM_INCANTIFIER)) return(TROUBLE_HUNGRY);
	if(HStun) return (TROUBLE_STUNNED);
	if(HConfusion) return (TROUBLE_CONFUSED);
	if(Hallucination) return(TROUBLE_HALLUCINATION);
	return(0);
}

/* select an item for TROUBLE_CURSED_ITEMS */
STATIC_OVL struct obj *
worst_cursed_item()
{
    register struct obj *otmp;

    /* if strained or worse, check for loadstone first */
    if (near_capacity() >= HVY_ENCUMBER) {
	for (otmp = invent; otmp; otmp = otmp->nobj)
	    if (Cursed_obj(otmp, LOADSTONE)) return otmp;
    }
	/* if weldproof, we don't care about cursed items other than a loadstone */
	if (Weldproof)
		return (struct obj *)0;
    /* weapon takes precedence if it is interfering
       with taking off a ring or putting on a shield */
    if (welded(uwep) && (uright || bimanual(uwep,youracedata))) {	/* weapon */
	otmp = uwep;
    /* gloves come next, due to rings */
    } else if (uarmg && uarmg->cursed) {		/* gloves */
	otmp = uarmg;
    /* then shield due to two handed weapons and spells */
    } else if (uarms && uarms->cursed) {		/* shield */
	otmp = uarms;
    /* then cloak due to body armor */
    } else if (uarmc && uarmc->cursed) {		/* cloak */
	otmp = uarmc;
    } else if (uarm && uarm->cursed) {			/* suit */
	otmp = uarm;
    } else if (uarmh && uarmh->cursed) {		/* helmet */
	otmp = uarmh;
    } else if (uarmf && uarmf->cursed) {		/* boots */
	otmp = uarmf;
#ifdef TOURIST
    } else if (uarmu && uarmu->cursed) {		/* shirt */
	otmp = uarmu;
#endif
    } else if (uamul && uamul->cursed) {		/* amulet */
	otmp = uamul;
    } else if (uleft && uleft->cursed) {		/* left ring */
	otmp = uleft;
    } else if (uright && uright->cursed) {		/* right ring */
	otmp = uright;
    } else if (ublindf && ublindf->cursed) {		/* eyewear */
	otmp = ublindf;	/* must be non-blinding lenses */
    /* if weapon wasn't handled above, do it now */
    } else if (welded(uwep)) {				/* weapon */
	otmp = uwep;
    /* active secondary weapon even though it isn't welded */
    } else if (uswapwep && uswapwep->cursed && u.twoweap) {
	otmp = uswapwep;
    /* all worn items ought to be handled by now */
    } else {
	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    if (!otmp->cursed) continue;
	    if (otmp->otyp == LOADSTONE || confers_luck(otmp))
		break;
	}
    }
    return otmp;
}

STATIC_OVL void
fix_worst_trouble(trouble)
register int trouble;
{
	int i;
	struct obj *otmp = 0;
	const char *what = (const char *)0;
	static NEARDATA const char leftglow[] = "left ring softly glows",
				   rightglow[] = "right ring softly glows";

	switch (trouble) {
	    case TROUBLE_STONED:
		    You_feel("more limber.");
		    Stoned = 0;
		    Golded = 0;
		    flags.botl = 1;
		    delayed_killer = 0;
		    break;
	    case TROUBLE_SLIMED:
		    pline_The("slime disappears.");
		    Slimed = 0;
		    flags.botl = 1;
		    delayed_killer = 0;
		    break;
	    case TROUBLE_BLOOD_DROWN:
		    pline_The("blood vanishes from your lungs.");
		    BloodDrown = 0;
		    flags.botl = 1;
		    delayed_killer = 0;
		    break;
	    case TROUBLE_FROZEN_AIR:
		    pline_The("frozen air vaporizes.");
		    FrozenAir = 0;
		    flags.botl = 1;
		    delayed_killer = 0;
		    break;
	    case TROUBLE_STRANGLED:
		    if (uamul && uamul->otyp == AMULET_OF_STRANGULATION) {
				Your("amulet vanishes!");
				useup(uamul);
		    }
			if(HStrangled){
				HStrangled = 0;
				Your("throat opens up!");
			}
			if(!Strangled) You("can breathe again.");
		    flags.botl = 1;
		    break;
	    case TROUBLE_LAVA:
		    You("are back on solid ground.");
		    /* teleport should always succeed, but if not,
		     * just untrap them.
		     */
		    if(!safe_teleds(FALSE))
			u.utrap = 0;
		    break;
	    case TROUBLE_STARVING:
		    losestr(-1);
		    /* fall into... */
	    case TROUBLE_HUNGRY:
		    Race_if(PM_CLOCKWORK_AUTOMATON) ? 
				Your("mainspring is miraculously wound!") :
				Your("%s feels content.", body_part(STOMACH));
		    reset_uhunger();
		    flags.botl = 1;
		    break;
	    case TROUBLE_SICK:
		    You_feel("better.");
		    make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		    break;
	    case TROUBLE_HIT:
		    /* "fix all troubles" will keep trying if hero has
		       5 or less hit points, so make sure they're always
		       boosted to be more than that */
		    You_feel("much better.");
		    if (Upolyd) {
				if (u.mhmax <= 5){
					u.uhpmod = 5+1;
					calc_total_maxhp();
				}
			u.mh = u.mhmax;
		    }
		    else if (u.uhpmax < u.ulevel * 5 + 11){
				u.uhpmod += rnd(5);
				calc_total_maxhp();
			}
			if(u.uhpmax < 6){
				if(u.uhprolled < 6)
					u.uhprolled = 6;	/* arbitrary */
				calc_total_maxhp();
				if(u.uhpmax < 6 && u.uhpmod < 0){
					u.uhpmod = 0;	/* arbitrary */
					calc_total_maxhp();
				}
				if(u.uhpmax < 6 && u.uhpbonus < 0){
					u.uhpbonus = 0;	/* arbitrary */
					calc_total_maxhp();
				}
			}
		    u.uhp = u.uhpmax;
		    flags.botl = 1;
		    break;
	    case TROUBLE_COLLAPSING:
		    ABASE(A_STR) = AMAX(A_STR);
		    flags.botl = 1;
		    break;
	    case TROUBLE_STUCK_IN_WALL:
		    Your("surroundings change.");
		    /* no control, but works on no-teleport levels */
		    (void) safe_teleds(FALSE);
		    break;
	    case TROUBLE_CURSED_LEVITATION:
		    if ((otmp = stuck_ring(uleft,RIN_LEVITATION)) !=0) {
				if (otmp == uleft) what = leftglow;
		    } else if ((otmp = stuck_ring(uright,RIN_LEVITATION))!=0) {
				if (otmp == uright) what = rightglow;
		    }
		    goto decurse;
	    case TROUBLE_UNUSEABLE_HANDS:
			if(Straitjacketed){
				otmp = uarm;
				goto decurse;
			}
		    else if (welded(uwep)) {
				otmp = uwep;
				goto decurse;
		    }
		    if (Upolyd && nohands(youracedata)) {
			if (!Unchanging) {
			    Your("shape becomes uncertain.");
			    rehumanize();  /* "You return to {normal} form." */
			} else if ((otmp = unchanger()) != 0 && otmp->cursed) {
			    /* otmp is an amulet of unchanging */
			    goto decurse;
			}
		    }
		    if (nohands(youracedata) || !freehand())
			impossible("fix_worst_trouble: couldn't cure hands.");
		    break;
	    case TROUBLE_CURSED_BLINDFOLD:
		    otmp = ublindf;
		    goto decurse;
	    case TROUBLE_WIMAGE:
			pline("The image of the weeping angel fades from your mind.");
		    u.wimage = 0;
		    break;
	    case TROUBLE_MORGUL:
			pline("The chill of death fades away.");
			//Destroy the blades.
		    u.umorgul = 0;
		    break;
	    case TROUBLE_MROT:
			pline("You stop shedding dust.");
		    u.umummyrot = 0;
		    break;
	    case TROUBLE_HPMOD:
			You_feel("restored to health.");
		    u.uhpmod = max(u.uhpmod, 0);
		    calc_total_maxhp();
		    break;
	    case TROUBLE_LYCANTHROPE:
		    you_unwere(TRUE);
		    break;
	/*
	 */
	    case TROUBLE_PUNISHED:
		    Your("chain disappears.");
		    unpunish();
		    break;
	    case TROUBLE_FUMBLING:
		    if (Cursed_obj(uarmg, GAUNTLETS_OF_FUMBLING))
			otmp = uarmg;
		    else if (Cursed_obj(uarmf, FUMBLE_BOOTS))
			otmp = uarmf;
		    goto decurse;
		    /*NOTREACHED*/
		    break;
	    case TROUBLE_CURSED_ITEMS:
		    otmp = worst_cursed_item();
		    if (otmp == uright) what = rightglow;
		    else if (otmp == uleft) what = leftglow;
decurse:
		    if (!otmp) {
			impossible("fix_worst_trouble: nothing to uncurse.");
			return;
		    }
		    uncurse(otmp);
		    if (!Blind) {
			Your("%s %s.", what ? what :
				(const char *)aobjnam(otmp, "softly glow"),
			     hcolor(NH_AMBER));
			otmp->bknown = TRUE;
		    }
		    update_inventory();
		    break;
	    case TROUBLE_POISONED:
		    if (Hallucination)
			pline("There's a tiger in your tank.");
		    else
			You_feel("in good health again.");
		    for(i=0; i<A_MAX; i++) {
			if(ABASE(i) < AMAX(i)) {
				ABASE(i) = AMAX(i);
				flags.botl = 1;
			}
		    }
		    (void) encumber_msg();
		    break;
	    case TROUBLE_CARRY_CURSED:
			Your("pack feels lighter.");
		    u.ucarinc = 0;
			inv_weight();
	    break;
	    case TROUBLE_TOHIT_CURSED:
			Your("curse is lifted.");
		    u.uhitinc = 0;
	    break;
	    case TROUBLE_AC_CURSED:
			Your("curse is lifted.");
		    u.uacinc = 0;
	    break;
	    case TROUBLE_DAMAGE_CURSED:
			Your("curse is lifted.");
		    u.udaminc = 0;
	    break;
	    case TROUBLE_BLIND:
		{
		    int num_eyes = eyecount(youracedata);
		    const char *eye = body_part(EYE);

		    Your("%s feel%s better.",
			 (num_eyes == 1) ? eye : makeplural(eye),
			 (num_eyes == 1) ? "s" : "");
		    u.ucreamed = 0;
		    make_blinded(0L,FALSE);
		    break;
		}
	    case TROUBLE_WOUNDED_LEGS:
		    heal_legs();
		    break;
	    case TROUBLE_STUNNED:
		    make_stunned(0L,TRUE);
		    break;
	    case TROUBLE_CONFUSED:
		    make_confused(0L,TRUE);
		    break;
	    case TROUBLE_HALLUCINATION:
		    pline ("Looks like you are back in Kansas.");
		    (void) make_hallucinated(0L,FALSE,0L);
		    break;
#ifdef STEED
	    case TROUBLE_SADDLE:
		    otmp = which_armor(u.usteed, W_SADDLE);
		    uncurse(otmp);
		    if (!Blind) {
			pline("%s %s %s.",
			      s_suffix(upstart(y_monnam(u.usteed))),
			      aobjnam(otmp, "softly glow"),
			      hcolor(NH_AMBER));
			otmp->bknown = TRUE;
		    }
		    break;
#endif
	}
}

/* "I am sometimes shocked by...  the nuns who never take a bath without
 * wearing a bathrobe all the time.  When asked why, since no man can see them,
 * they reply 'Oh, but you forget the good God'.  Apparently they conceive of
 * the Deity as a Peeping Tom, whose omnipotence enables Him to see through
 * bathroom walls, but who is foiled by bathrobes." --Bertrand Russell, 1943
 * Divine wrath, dungeon walls, and armor follow the same principle.
 */
STATIC_OVL void
god_zaps_you(godnum)
int godnum;
{
	if (u.uswallow) {
	    pline("Suddenly a bolt of lightning comes down at you from the heavens!");
	    pline("It strikes %s!", mon_nam(u.ustuck));
	    if (!resists_elec(u.ustuck)) {
		pline("%s fries to a crisp!", Monnam(u.ustuck));
		/* Yup, you get experience.  It takes guts to successfully
		 * pull off this trick on your god, anyway.
		 */
		xkilled(u.ustuck, 0);
	    } else
		pline("%s seems unaffected.", Monnam(u.ustuck));
	} else {
	    pline("Suddenly, a bolt of lightning strikes you!");
	    if (Reflecting) {
		shieldeff(u.ux, u.uy);
		if (Blind)
		    pline("For some reason you're unaffected.");
		else
		    (void) ureflects("%s reflects from your %s.", "It");
	    } else if (Shock_resistance) {
		shieldeff(u.ux, u.uy);
		pline("It seems not to affect you.");
	    } else
		fry_by_god(godnum);
	}

	pline("%s is not deterred...", godname(godnum));
	if (u.uswallow) {
	    pline("A wide-angle disintegration beam aimed at you hits %s!",
			mon_nam(u.ustuck));
	    if (!resists_disint(u.ustuck)) {
		pline("%s fries to a crisp!", Monnam(u.ustuck));
		xkilled(u.ustuck, 2); /* no corpse */
	    } else
		pline("%s seems unaffected.", Monnam(u.ustuck));
	} else {
	    pline("A wide-angle disintegration beam hits you!");

	    /* disintegrate shield and body armor before disintegrating
	     * the impudent mortal, like black dragon breath -3.
	     */
	    if (uarms && !(EReflecting & W_ARMS) &&
	    		!(EDisint_resistance & W_ARMS))
		(void) destroy_arm(uarms);
	    if (uarmc && !(EReflecting & W_ARMC) &&
	    		!(EDisint_resistance & W_ARMC))
		(void) destroy_arm(uarmc);
	    if (uarm && !(EReflecting & W_ARM) &&
	    		!(EDisint_resistance & W_ARM) && !uarmc)
		(void) destroy_arm(uarm);
#ifdef TOURIST
	    if (uarmu && 
			!(EReflecting & W_ARMU) && !(EDisint_resistance & W_ARMU) &&
			!(uarm && arm_blocks_upper_body(uarm->otyp)) && !uarmc
		) (void) destroy_arm(uarmu);
#endif
	    if (!Disint_resistance)
		fry_by_god(godnum);
	    else {
		You("bask in its %s glow for a minute...", NH_BLACK);
		godvoice(godnum, "I believe it not!");
	    }
	    if (Is_astralevel(&u.uz) || Is_sanctum(&u.uz)) {
			char buf[BUFSZ];
		/* one more try for high altars */
			godvoice(godnum, "Thou cannot escape my wrath, mortal!");
			(void) summon_god_minion(godnum, FALSE);
			(void) summon_god_minion(godnum, FALSE);
			(void) summon_god_minion(godnum, FALSE);
			(void) summon_god_minion(godnum, FALSE);
			(void) summon_god_minion(godnum, FALSE);
			Sprintf(buf, "Destroy %s, my servants!", uhim());
			godvoice(godnum, buf);
	    }
	}
}

STATIC_OVL void
fry_by_god(godnum)
int godnum;
{
	char killerbuf[64];

	You("fry to a crisp.");
	killer_format = KILLED_BY;
	Sprintf(killerbuf, "the wrath of %s", godname(godnum));
	killer = killerbuf;
	done(DIED);
}

void
angrygods(godnum)
int godnum;
{
	register int	maxanger;
	char buf[BUFSZ];

	if(godnum == GOD_THE_VOID || godnum == GOD_BOKRUG__THE_WATER_LIZARD) {
		/* the void does not get angry */
		/* Bokrug DOES get angry, but has really bad aim. */
		return;
	}

	/* removes all divine protection */
	u.ublessed = 0;
	
	if(u.ualign.god == godnum){
		u.lastprayed = moves;
		u.lastprayresult = PRAY_ANGER;
		u.reconciled = REC_NONE;
	}
	/* changed from tmp = anger + abs (u.uluck) -- rph */
	/* added test for alignment diff -dlc */
	if(u.ualign.god != godnum){
	    maxanger =  3*godlist[godnum].anger +
		((Luck > 0 || u.ualign.record <= STRAYED) ? -Luck/3 : -Luck);
	} else {
	    maxanger =  3*godlist[godnum].anger +
		((Luck > 0 || u.ualign.record >= STRIDENT) ? -Luck/3 : -Luck);
	}
	
	if (maxanger < 1) maxanger = 1; /* possible if bad align & good luck */
	else if (maxanger > 15) maxanger = 15;	/* be reasonable */
	
	if(godnum == GOD_THE_BLACK_MOTHER){
		/* anger goat-following creatures on the level */
		struct monst *mtmp;
		for(mtmp = migrating_mons; mtmp; mtmp = mtmp->nmon){
			if(mtmp->mux == u.uz.dnum && mtmp->muy == u.uz.dlevel && (mtmp->mtyp == PM_BLESSED || mtmp->mtyp == PM_MOUTH_OF_THE_GOAT)){
				untame(mtmp, 0);
				//Does not re-set alignment value (as if you attacked a peaceful)
			}
		}
		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon){
			if(mtmp->mfaction == GOATMOM_FACTION && !mtmp->mtame){
				mtmp->mpeaceful = 0;
				//Does not re-set alignment value (as if you attacked a peaceful)
				newsym(mtmp->mx, mtmp->my);
			}
		}
		/* lose 20% of accumulated credit */
		if (u.shubbie_credit > 0)
			u.shubbie_credit = u.shubbie_credit * 4 / 5;
		/* do NOT go to standard smites */
		godvoice(godnum,"");
		return;
	}
	
	switch (rn2(maxanger)) {
	    case 0:
	    case 1:
			if(godnum == GOD_THE_BLACK_MOTHER){
				You_feel("that %s is %s.", godname(godnum),
					Hallucination ? "peckish" : "hungry");
			} else {
				You_feel("that %s is %s.", godname(godnum),
			    Hallucination ? "bummed" : "displeased");
			}
			break;
	    case 2:
	    case 3:
			Sprintf(buf,"Thou %s, %s. Thou must relearn thy lessons!",
			    (ugod_is_angry() && godnum == u.ualign.god)
				? "hast strayed from the path" :
						"art arrogant",
			      youracedata->mlet == S_HUMAN ? "mortal" : "creature");
			godvoice(godnum,buf);
			(void) adjattrib(A_WIS, -1, FALSE);
			losexp((char *)0,TRUE,FALSE,TRUE);
			break;
	    case 6:	if (!Punished) {
			    gods_angry(godnum);
			    punish((struct obj *)0);
			    break;
			} /* else fall thru */
	    case 4:
	    case 5:	gods_angry(godnum);
			if (!Blind && !Antimagic)
			    pline("%s glow surrounds you.",
				  An(hcolor(NH_BLACK)));
			rndcurse();
			break;
	    case 7:
	    case 8:	
			Sprintf(buf,"Thou durst %s me? Then die, %s!",
				  (on_altar() &&
				   ((god_at_altar(u.ux,u.uy)) != godnum)) ?
				  "scorn":"call upon",
			      youracedata->mlet == S_HUMAN ? "mortal" : "creature"
			);
			godvoice(godnum, buf);
			(void) summon_god_minion(godnum, FALSE);
			break;

	    default:	
			gods_angry(godnum);
			god_zaps_you(godnum);
			break;
	}
	u.ublesscnt = rnz(300);
	return;
}

/* chance for god to give you a gift */
/* returns TRUE if your god should give you a gift */
boolean
maybe_god_gives_gift()
{
	/* previous: 1 in (10 + (2 * gifts * (gifts+wishes))) */
	/* Role_if(PM_PRIEST) ? !rn2(10 + (2 * u.ugifts * u.ugifts)) : !rn2(10 + (2 * u.ugifts * nartifacts)) */
	/* the average giftable artifact has a value of 4 (TIER_B), plus any bonuses for the player being good with it */
	/* uartisval isn't increased for priests when wishing */

	//New:
	return !rn2(10 + (u.uartisval * u.uartisval * 2 / 25));
	//Old:
	//int nartifacts = u.ugifts + (Role_if(PM_PRIEST) ? 0 : u.uconduct.wisharti);
	//return !rn2(10 + (2 * u.ugifts * nartifacts));
}


/* helper to print "str appears at your feet", or appropriate */
void
at_your_feet(str)
	const char *str;
{
	if (Blind) str = Something;
	if (u.uswallow) {
	    /* barrier between you and the floor */
	    pline("%s %s into %s %s.", str, vtense(str, "drop"),
		  s_suffix(mon_nam(u.ustuck)), mbodypart(u.ustuck, STOMACH));
	} else {
	    pline("%s %s %s your %s!", str,
		  Blind ? "lands" : vtense(str, "appear"),
		  Levitation ? "beneath" : "at",
		  makeplural(body_part(FOOT)));
	}
}

STATIC_OVL void
pleased(godnum)
int godnum;
{
	/* don't use p_trouble, worst trouble may get fixed while praying */
	int trouble = in_trouble();	/* what's your worst difficulty? */
	int pat_on_head = 0, kick_on_butt;

	You_feel("that %s is %s.", godname(godnum),
	    u.ualign.record >= DEVOUT ?
	    Hallucination ? "pleased as punch" : "well-pleased" :
	    u.ualign.record >= STRIDENT ?
	    Hallucination ? "ticklish" : "pleased" :
	    Hallucination ? "full" : "satisfied");

	/* not your deity */
	if (on_altar() && p_god != u.ualign.god) {
		adjalign(-1);
		return;
	} else if (u.ualign.record < 2 && trouble <= 0) adjalign(1);

	/* depending on your luck & align level, the god you prayed to will:
	   - fix your worst problem if it's major.
	   - fix all your major problems.
	   - fix your worst problem if it's minor.
	   - fix all of your problems.
	   - do you a gratuitous favor.

	   if you make it to the the last category, you roll randomly again
	   to see what they do for you.

	   If your luck is at least 0, then you are guaranteed rescued
	   from your worst major problem. */

	if (!trouble && u.ualign.record >= DEVOUT) {
	    /* if hero was in trouble, but got better, no special favor */
	    if (p_trouble == 0) pat_on_head = 1;
	} else {
	    int action = rn1(Luck + (on_altar() ? 3 + on_shrine() : 2), 1);
	    /* pleased Lawful gods often send you a helpful angel if you're
	       getting the crap beat out of you */
	    if ((u.uhp < 5 || (u.uhp*7 < u.uhpmax)) &&
		 u.ualign.type == A_LAWFUL && rn2(3)) lawful_god_gives_angel();

	    if (!on_altar()) action = min(action, 3);
	    if (u.ualign.record < STRIDENT)
		action = (u.ualign.record > 0 || !rnl(2)) ? 1 : 0;

	    switch(min(action,5)) {
	    case 5: pat_on_head = 1;
	    case 4: do fix_worst_trouble(trouble);
		    while ((trouble = in_trouble()) != 0);
		    break;

	    case 3: fix_worst_trouble(trouble);
	    case 2: while ((trouble = in_trouble()) > 0)
		    fix_worst_trouble(trouble);
		    break;

	    case 1: if (trouble > 0) fix_worst_trouble(trouble);
	    case 0: break; /* your god blows you off, too bad */
	    }
	}

    /* note: can't get pat_on_head unless all troubles have just been
       fixed or there were no troubles to begin with; hallucination
       won't be in effect so special handling for it is superfluous */
    if(pat_on_head){
		//Note: Luck > 10 means that you have a luckitem in open inventory.  To avoid crowning, just drop the luckitem.
	    if (Luck > 10 && u.ualign.record >= PIOUS && !u.uevent.uhand_of_elbereth && u.uevent.qcompleted){
			gcrownu();
		}
		else if(Pantheon_if(PM_VALKYRIE) && u.ualign.record >= PIOUS 
			&& uwep && is_spear(uwep) && !uwep->oartifact && uwep->spe >= 5 
			&& !art_already_exists(ART_GUNGNIR) && (galign(godnum) == A_LAWFUL || galign(godnum) == A_NEUTRAL)
		){
			pline("Secret runes are engraved on your %s.", xname(uwep));
			oname(uwep, artilist[ART_GUNGNIR].name);
		}
		else switch(rn2((Luck + 6)>>1)) {
		case 0:	break;
		case 1:
			if (uwep && (welded(uwep) || uwep->oclass == WEAPON_CLASS ||
				 is_weptool(uwep))) {
			char repair_buf[BUFSZ];

			*repair_buf = '\0';
			if (uwep->oeroded || uwep->oeroded2)
				Sprintf(repair_buf, " and %s now as good as new",
					otense(uwep, "are"));

			if (uwep->cursed) {
				uncurse(uwep);
				uwep->bknown = TRUE;
				if (!Blind)
				Your("%s %s%s.", aobjnam(uwep, "softly glow"),
					 hcolor(NH_AMBER), repair_buf);
				else You_feel("the power of %s over your %s.",
				u_gname(), xname(uwep));
				*repair_buf = '\0';
			} else if (!uwep->blessed) {
				bless(uwep);
				uwep->bknown = TRUE;
				if (!Blind)
				Your("%s with %s aura%s.",
					 aobjnam(uwep, "softly glow"),
					 an(hcolor(NH_LIGHT_BLUE)), repair_buf);
				else You_feel("the blessing of %s over your %s.",
				u_gname(), xname(uwep));
				*repair_buf = '\0';
			}

			/* fix any rust/burn/rot damage, but don't protect
			   against future damage */
			if (uwep->oeroded || uwep->oeroded2) {
				uwep->oeroded = uwep->oeroded2 = 0;
				/* only give this message if we didn't just bless
				   or uncurse (which has already given a message) */
				if (*repair_buf)
				Your("%s as good as new!",
					 aobjnam(uwep, Blind ? "feel" : "look"));
			}
			update_inventory();
			}
			break;
		case 3:
			/* takes only 1 hint to get the music to enter the stronghold */
			if (!u.uevent.uopened_dbridge) {
			if (u.uevent.uheard_tune < 2) {
				char buf[BUFSZ];
				Sprintf(buf, "Hark, %s! To enter the castle, thou must play the right tune!", youracedata->mlet == S_HUMAN ? "mortal" : "creature");
				godvoice(godnum, buf);
				You_hear("a divine music...");
				pline("It sounds like:  \"%s\".", tune);
				u.uevent.uheard_tune = 2;
				break;
			}
			}
			/* Otherwise, falls into next case */
		case 2:
			if (!Blind)
			You("are surrounded by %s glow.", an(hcolor(NH_GOLDEN)));
			/* if any levels have been lost (and not yet regained),
			   treat this effect like blessed full healing */
			if (u.ulevel < u.ulevelmax) {
				// u.ulevelmax -= 1;	/* see potion.c */
				pluslvl(FALSE);
			} else {
				u.uhpbonus += 5;
				calc_total_maxhp();
			}
			u.uhp = u.uhpmax;
			if (Upolyd) u.mh = u.mhmax;
			ABASE(A_STR) = AMAX(A_STR);
			if(Race_if(PM_INCANTIFIER)){
				if (u.uen < u.uenmax*.45) u.uen += 400;
				newuhs(TRUE);
			} else {
				if (u.uhunger < get_uhungermax()*.45) u.uhunger = get_uhungermax()*.45;
				u.uhs = NOT_HUNGRY;
			}
			if (u.uluck < 0) u.uluck = 0;
			make_blinded(0L,TRUE);
			flags.botl = 1;
			break;
		case 4: {
			register struct obj *otmp;
			int any = 0;

			if (Blind)
			You_feel("the power of %s.", godname(godnum));
			else You("are surrounded by %s aura.",
				 an(hcolor(NH_LIGHT_BLUE)));
			for(otmp=invent; otmp; otmp=otmp->nobj) {
			if (otmp->cursed) {
				uncurse(otmp);
				if (!Blind) {
				Your("%s %s.", aobjnam(otmp, "softly glow"),
					 hcolor(NH_AMBER));
				otmp->bknown = TRUE;
				++any;
				}
			}
			}
			if (any) update_inventory();
			if(u.sealsActive&SEAL_MARIONETTE) unbind(SEAL_MARIONETTE,TRUE);
			break;
		}
		case 5: {
			const char *msg="Thou hast pleased me with thy progress, and thus I grant thee the gift of %s! Use it wisely in my name!";
			char buf[BUFSZ];
			if (!(HTelepat & INTRINSIC))  {
			HTelepat |= FROMOUTSIDE;
			Sprintf(buf, msg, "Telepathy");
			if (Blind) see_monsters();
			} else if (!(HFast & INTRINSIC))  {
			HFast |= FROMOUTSIDE;
			Sprintf(buf, msg, "Speed");
			} else if (!(HStealth & INTRINSIC))  {
			HStealth |= FROMOUTSIDE;
			Sprintf(buf, msg, "Stealth");
			} else if(!(u.wardsknown & WARD_HAMSA)){
				u.wardsknown |= WARD_HAMSA;
				Sprintf(buf, msg, "the Hamsa ward");
			} else if(!(u.wardsknown & WARD_HEXAGRAM)){
				u.wardsknown |= WARD_HEXAGRAM;
				Sprintf(buf, msg, "the Hexagram ward");
			}else {
			if (!(HProtection & INTRINSIC))  {
				HProtection |= FROMOUTSIDE;
				if (!u.ublessed)  u.ublessed = rn1(3, 2);
			} else u.ublessed++;
			Sprintf(buf, msg, "my protection");
			}
			godvoice(godnum, buf);
			break;
		}
		case 7:
		case 8:
		case 9:		/* KMH -- can occur during full moons */
		case 6:	{
			struct obj *otmp;
			int sp_no, trycnt = u.ulevel + 1;

			at_your_feet("An object");
			/* not yet known spells given preference over already known ones */
			/* Also, try to grant a spell for which there is a skill slot */
			otmp = mkobj(SPBOOK_CLASS, TRUE);
			while (--trycnt > 0) {
			if (otmp->otyp != SPE_BLANK_PAPER) {
				for (sp_no = 0; sp_no < MAXSPELL; sp_no++)
				if (spl_book[sp_no].sp_id == otmp->otyp) break;
				if (sp_no == MAXSPELL &&
				!P_RESTRICTED(spell_skilltype(otmp->otyp)))
				break;	/* usable, but not yet known */
			} else {
				if (!objects[SPE_BLANK_PAPER].oc_name_known ||
					carrying(MAGIC_MARKER)) break;
			}
			otmp->otyp = rnd_class(bases[SPBOOK_CLASS], SPE_BLANK_PAPER);
			}
			bless(otmp);
			place_object(otmp, u.ux, u.uy);
			break;
		}
		default:	impossible("Confused deity!");
			break;
		}
	}
	
	/*Scare hostile monsters on level*/
	{
		struct monst *tmpm;
		for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
			if(!tmpm->mpeaceful){
				monflee(tmpm, 77, TRUE, TRUE);
			}
		}
	}
	
	u.ublesscnt = rnz(350);
	kick_on_butt = u.uevent.udemigod ? 1 : 0;
#ifdef ELBERETH
	if (u.uevent.uhand_of_elbereth) kick_on_butt++;
#endif
	if (kick_on_butt) u.ublesscnt += kick_on_butt * rnz(1000);

	return;
}

/* either blesses or curses water on the altar,
 * returns true if it found any water here.
 */
STATIC_OVL boolean
water_prayer(bless_water)
    boolean bless_water;
{
    register struct obj* otmp;
    register long changed = 0;
	int non_water_changed = 0;
    boolean other = FALSE, bc_known = !(Blind || Hallucination);

    for(otmp = level.objects[u.ux][u.uy]; otmp; otmp = otmp->nexthere) {
	/* turn water into (un)holy water */
	if (otmp->otyp == POT_WATER &&
		(bless_water ? !otmp->blessed : !otmp->cursed)) {
	    otmp->blessed = bless_water;
	    otmp->cursed = !bless_water;
	    otmp->bknown = bc_known;
	    changed += otmp->quan;
	} else if(otmp->oclass == POTION_CLASS)
	    other = TRUE;
	else if (((bless_water) ? 
				(check_oprop(otmp, OPROP_HOLY) || check_oprop(otmp, OPROP_HOLYW) || check_oprop(otmp, OPROP_LESSER_HOLYW)) : 
				(check_oprop(otmp, OPROP_UNHY) || check_oprop(otmp, OPROP_UNHYW) || check_oprop(otmp, OPROP_LESSER_UNHYW))) 
		&& (bless_water ? !otmp->blessed : !otmp->cursed)
	){
		otmp->blessed = bless_water;
		otmp->cursed = !bless_water;
		otmp->bknown = bc_known;
		if (!Blind) {
			if (strncmpi(The(xname(otmp)), "The ", 4))
			{
				pline("On the altar, %s glow%s %s for a moment.",
					xname(otmp), (otmp->quan > 1L ? "" : "s"),
					(bless_water ? hcolor(NH_LIGHT_BLUE) : hcolor(NH_BLACK)));
			}
			else
			{
				pline("%s on the altar glow%s %s for a moment.",
					The(xname(otmp)), (otmp->quan > 1L ? "" : "s"),
					(bless_water ? hcolor(NH_LIGHT_BLUE) : hcolor(NH_BLACK)));
			}
		}
	}
    }
    if(!Blind && changed) {
	pline("%s potion%s on the altar glow%s %s for a moment.",
	      ((other && changed > 1L) ? "Some of the" :
					(other ? "One of the" : "The")),
	      ((other || changed > 1L) ? "s" : ""), (changed > 1L ? "" : "s"),
	      (bless_water ? hcolor(NH_LIGHT_BLUE) : hcolor(NH_BLACK)));
    }
    return((boolean)(changed > 0L));
}

void
godvoice(godnum, words)
int godnum;
const char *words;
{
    const char *quot = "";
    if(words)
	quot = "\"";
    else
	words = "";
	
	if(godnum == GOD_THE_VOID){
		You("think you hear a voice in the distance: %s%s%s", quot, words, quot);
	} else if(godnum == GOD_THE_SILENCE){
		You_hear("silence.");
	} else if(godnum == GOD_AN_ALIEN_GOD){
		if(Hallucination){
			pline("The world speaks to you: %s%s%s", quot, words, quot);
		} else {
			pline("The world quakes around you.  Perhaps it is the voice of a god?");
		}
		do_earthquake(u.ux, u.uy, 10, 2, FALSE, (struct monst *)0);
	} else if(godnum == GOD_THE_BLACK_MOTHER){
		You_hear("%s", goatvoices[rn2(SIZE(goatvoices))]);
	} else if(godnum == GOD_BOKRUG__THE_WATER_LIZARD){
		You_hear("%s", bokrugvoices[rn2(SIZE(bokrugvoices))]);
	} else if(godnum == GOD_THE_DREAD_FRACTURE){
		You_hear("a voice like glass breaking: %s%s%s", quot, words, quot);
	} else if(godnum == GOD_YOG_SOTHOTH){
		if(is_silent(youracedata)) You_hear("your voice %s: %s%s%s", godvoices[rn2(SIZE(godvoices))], quot, words, quot);
		else You("%s: %s%s%s", yogvoices[rn2(SIZE(yogvoices))],  quot, words, quot);
	} else {
		pline_The("voice of %s %s: %s%s%s", godname(godnum),
		  godvoices[rn2(SIZE(godvoices))], quot, words, quot);
	}
}

/* god voices their displeasure with you (but doesn't do anything) */
void
gods_angry(godnum)
int godnum;
{
    godvoice(godnum, "Thou hast angered me.");
}

/* This god is upset with you. */
void
gods_upset(godnum)
int godnum;
{
	if(godnum == GOD_THE_VOID) return;

	if (godnum == u.ualign.god)
		godlist[godnum].anger++;
	else {
		godlist[godnum].anger++;
		/* anger other gods calms your god */
		if (godlist[u.ualign.god].anger > 0)
			godlist[u.ualign.god].anger--;
	}

	if(godnum == GOD_BOKRUG__THE_WATER_LIZARD)
		return;

	angrygods(godnum);
}

static NEARDATA const char sacrifice_types[] = { FOOD_CLASS, AMULET_CLASS, 0 };

STATIC_OVL void
eat_offering(otmp, silently, eatflag)
register struct obj *otmp;
boolean silently;
int eatflag;
{
	xchar x, y;
	get_obj_location(otmp, &x, &y, BURIED_TOO);
	if(!silently){
		if (Hallucination)
		switch (rn2(25)) {
			case 0:
			Your("sacrifice sprouts wings and a propeller and roars away!");
			break;
			case 1:
			Your("sacrifice puffs up, swelling bigger and bigger, and pops!");
			break;
			case 2:
			Your("sacrifice collapses into a cloud of dancing particles and fades away!");
			case 3:
			Your("sacrifice scarcifies!");
			case 4:
			You("can't find your sacrifice.");
			You("must have misplaced it!");
			break;
			case 5:
			Your("sacrifice is consumed with doubt!");
			break;
			case 6:
			Your("sacrifice rots away!");
			break;
			case 7:
			Your("sacrifice is homogenized!");
			break;
			case 8:
			Your("sacrifice is vaporized!");
			break;
			case 9:
			Your("sacrifice is consumed in a flash!");
			break;
			case 10:
			Your("sacrifice is consumed by the altar!");
			break;
			case 11:
			You("consume the sacrifice!");
			break;
			case 12:
			Your("sacrifice is rejected!");
			break;
			case 13:
			Your("sacrifice is dejected!");
			break;
			case 14:
			Your("sacrifice is consumed in a rout!");
			break;
			case 15:
			Your("sacrifice is consumed in a lash of fight!");
			break;
			case 16:
			Your("sacrifice is lame!");
			break;
			case 17:
			You("are consumed in a %s!",
			  u.ualign.type != A_LAWFUL ? "flash of light" : "burst of flame");
			break;
			case 18:
			Your("sacrifice is consumed by jealousy!");
			break;
			case 19:
			Your("sacrifice is consumed by embarrassment!");
			break;
			case 20:
			Your("sacrifice is consumed by self-hate!");
			break;
			case 21:
			Your("sacrifice was eaten by fairies!");
			break;
			case 22:
			Your("sacrifice is vanishes in a dash at night!");
			case 23:
			u.ualign.type == A_LAWFUL ?
				Your("sacrifice is consumed in a flash of %s light!", hcolor(0)):
				Your("sacrifice is consumed in a burst of %s flame!", hcolor(0));
			break;
			case 24:
			Your("sacrifice is consumed by trout!");
			break;
		}
		else if(Blind || (!carried(otmp) && !cansee(x,y)))
		You_hear("crunching noises.");
		else if(!carried(otmp) && cansee(x,y)){
			if(u.veil)
				pline("Mist moves over %s. When the mist clears %s is gone!", an(singular(otmp, xname)), the(singular(otmp, xname)));
			else pline("A mouth forms from the mist and eats %s!", an(singular(otmp, xname)));
		}
		else {
			// If it's carried, it should have lifted the veil.
			if(u.veil) impossible("Offered to the goat with an intact veil?");
			pline("A mouth forms from the mist and eats your sacrifice!");
		}
	}
	if(u.sealsActive&SEAL_BALAM && eatflag != GOAT_EAT_PASSIVE){
		struct permonst *ptr = &mons[otmp->corpsenm];
		if(!(is_animal(ptr) || nohands(ptr))) unbind(SEAL_BALAM,TRUE);
	}
	if(u.sealsActive&SEAL_YMIR && eatflag != GOAT_EAT_PASSIVE){
		struct permonst *ptr = &mons[otmp->corpsenm];
		if(is_giant(ptr)) unbind(SEAL_YMIR,TRUE);
	}
    if (carried(otmp)) useup(otmp);
    else useupf(otmp, 1L);
    exercise(A_WIS, TRUE);
}

STATIC_OVL void
burn_offering(otmp, silently)
register struct obj *otmp;
boolean silently;
{
	xchar x, y;
	get_obj_location(otmp, &x, &y, BURIED_TOO);
	if(!silently){
		Your("sacrifice is consumed in a flash of silver flame!");
	}
	if(u.sealsActive&SEAL_BALAM){
		struct permonst *ptr = &mons[otmp->corpsenm];
		if(!(is_animal(ptr) || nohands(ptr))) unbind(SEAL_BALAM,TRUE);
	}
	if(u.sealsActive&SEAL_YMIR){
		struct permonst *ptr = &mons[otmp->corpsenm];
		if(is_giant(ptr)) unbind(SEAL_YMIR,TRUE);
	}
    if (carried(otmp)) useup(otmp);
    else useupf(otmp, 1L);
    exercise(A_CHA, TRUE);
}

STATIC_OVL void
consume_offering(otmp)
register struct obj *otmp;
{
    if (Hallucination)
	switch (rn2(25)) {
	    case 0:
		Your("sacrifice sprouts wings and a propeller and roars away!");
		break;
	    case 1:
		Your("sacrifice puffs up, swelling bigger and bigger, and pops!");
		break;
	    case 2:
		Your("sacrifice collapses into a cloud of dancing particles and fades away!");
	    case 3:
		Your("sacrifice scarcifies!");
	    case 4:
		You("can't find your sacrifice.");
		You("must have misplaced it!");
		break;
	    case 5:
		Your("sacrifice is consumed with doubt!");
		break;
	    case 6:
		Your("sacrifice rots away!");
		break;
		case 7:
		godvoice(GOD_NONE, "Hey! I ordered the chicken!");
		break;
		case 8:
		godvoice(GOD_NONE, "Oh, gross! Honey, the pets left another dead critter on the doorstep!");
		break;
		case 9:
		Your("sacrifice is consumed in a flash!");
		break;
		case 10:
		Your("sacrifice is consumed by the altar!");
		break;
		case 11:
		You("consume the sacrifice!");
		break;
		case 12:
		Your("sacrifice is rejected!");
		break;
		case 13:
		Your("sacrifice is dejected!");
		break;
		case 14:
		godvoice(GOD_NONE, "This better be kosher!");
		break;
		case 15:
		Your("sacrifice is consumed in a lash of fight!");
		break;
		case 16:
		Your("sacrifice is lame!");
		break;
		case 17:
		You("are consumed in a %s!",
	      u.ualign.type != A_LAWFUL ? "flash of light" : "burst of flame");
    	break;
	    case 18:
		Your("sacrifice is consumed with jealousy!");
		break;
	    case 19:
		Your("sacrifice is consumed with embarrassment!");
		break;
	    case 20:
		Your("sacrifice is consumed with self-hate!");
		break;
	    case 21:
		Your("sacrifice was stolen by fairies!");
		break;
	    case 22:
		Your("sacrifice is vanishes in a dash at night!");
	    case 23:
		u.ualign.type == A_LAWFUL ?
			Your("sacrifice is consumed in a flash of %s light!", hcolor(0)):
			Your("sacrifice is consumed in a burst of %s flame!", hcolor(0));
		break;
	    case 24:
		Your("sacrifice is consumed by trout!");
		break;
	}
    else if (Blind && u.ualign.type == A_LAWFUL)
	Your("sacrifice disappears!");
    else Your("sacrifice is consumed in a %s!",
	      u.ualign.type == A_LAWFUL ? "flash of light" : "burst of flame");
	if(u.sealsActive&SEAL_BALAM){
		struct permonst *ptr = &mons[otmp->corpsenm];
		if(!(is_animal(ptr) || nohands(ptr))) unbind(SEAL_BALAM,TRUE);
	}
	if(u.sealsActive&SEAL_YMIR){
		struct permonst *ptr = &mons[otmp->corpsenm];
		if(is_giant(ptr)) unbind(SEAL_YMIR,TRUE);
	}
    if (carried(otmp)) useup(otmp);
    else useupf(otmp, 1L);
    exercise(A_WIS, TRUE);
}

void
god_gives_pet(godnum)
int godnum;
{
/*
    register struct monst *mtmp2;
    register struct permonst *pm;
 */
    const int *minions = god_minions(godnum);
    int mtyp=NON_PM, mlev, num = 0, first, last;
	struct monst *mon = (struct monst *)0;
	
	mlev = level_difficulty();
	
	for (first = 0; minions[first] != NON_PM; first++)
	    if (!(mvitals[minions[first]].mvflags & G_GONE && !In_quest(&u.uz)) && monstr[minions[first]] > mlev-5) break;
	if(minions[first] == NON_PM){ //All minions too weak, or no minions
		if(first == 0) return;
		else mtyp = minions[first-1];
	}
	else for (last = first; minions[last] != NON_PM; last++)
	    if (!(mvitals[minions[last]].mvflags & G_GONE && !In_quest(&u.uz))) {
			/* consider it */
			if(monstr[minions[last]] > mlev*2) break;
			num += min(1,mons[minions[last]].geno & G_FREQ);
	    }

	if(!num){ //All minions too strong, or gap between weak and strong minions
		if(first == 0) return;
		else mtyp = minions[first-1];
	}
/*	Assumption:	minions are presented in ascending order of strength. */
	else{
		for(num = rnd(num); num > 0; first++) if (!(mvitals[minions[first]].mvflags & G_GONE && !In_quest(&u.uz))) {
			/* skew towards lower value monsters at lower exp. levels */
			num -= min(1, mons[minions[first]].geno & G_FREQ);
			if (num && adj_lev(&mons[minions[first]]) > (u.ulevel*2)) {
				/* but not when multiple monsters are same level */
				if (mons[first].mlevel != mons[first+1].mlevel)
				num--;
			}
	    }
		first--; /* correct an off-by-one error */
		mtyp = minions[first];
	}


    if (mtyp == NON_PM) {
		mon = (struct monst *)0;
    }
	else mon = make_pet_minion(mtyp, godnum, FALSE);
	
    if (mon) {
	switch (galign(godnum)) {
	   case A_LAWFUL:
		if (u.uhp > (u.uhpmax / 10)) godvoice(godnum, "My minion shall serve thee!");
		else godvoice(godnum, "My minion shall save thee!");
	   break;
	   case A_NEUTRAL:
	   case A_VOID:
		pline("%s", Blind ? "You hear the earth rumble..." :
		 "A cloud of gray smoke gathers around you!");
	   break;
	   case A_CHAOTIC:
	   case A_NONE:
		pline("%s", Blind ? "You hear an evil chuckle!" :
		 "A miasma of stinking vapors coalesces around you!");
	   break;
	}
	return;
    }
}

static void
lawful_god_gives_angel()
{
/*
    register struct monst *mtmp2;
    register struct permonst *pm;
*/
    int mtyp;
    int mon;

	god_gives_pet(align_to_god(A_LAWFUL));
}

int
dosacrifice()
{
    register struct obj *otmp;
    int value = 0;
    int pm;
    aligntyp altaralign = (a_align(u.ux,u.uy));
	int altargod = god_at_altar(u.ux, u.uy);
	char buf[BUFSZ];
    if (!on_altar() || u.uswallow) {
		You("are not standing on an altar.");
		return MOVE_CANCELLED;
    }

	if(Role_if(PM_ANACHRONONAUT) && flags.questprogress != 2 && u.uevent.qcompleted && u.uhave.questart && Is_astralevel(&u.uz)){
		You("worry that you have not yet completed your mission.");
	}
	
    if (In_endgame(&u.uz)) {
		if (!(otmp = getobj(sacrifice_types, "sacrifice"))) return MOVE_CANCELLED;
	} else {
		if (!(otmp = floorfood("sacrifice", 1))) return MOVE_CANCELLED;
    }
    /*
      Was based on nutritional value and aging behavior (< 50 moves).
      Sacrificing a food ration got you max luck instantly, making the
      gods as easy to please as an angry dog!

      Now only accepts corpses, based on the game's evaluation of their
      toughness.  Human and pet sacrifice, as well as sacrificing unicorns
      of your alignment, is strongly discouraged.
     */
	
	if(goat_mouth_at(u.ux, u.uy)){
		if(u.veil){
			You("feel reality threatening to slip away!");
			if (yn("Are you sure you want to make an offering?") != 'y'){
				return(0);
			}
			else pline("So be it.");
			u.veil = FALSE;
			change_uinsight(1);
		}
		goat_eat(otmp, GOAT_EAT_OFFERED);
		return MOVE_STANDARD;
	}
	if(bokrug_idol_at(u.ux, u.uy)){
		bokrug_offer(otmp);
		return MOVE_STANDARD;
	}
	
	if(Role_if(PM_ANACHRONONAUT) && otmp->otyp != AMULET_OF_YENDOR && flags.questprogress!=2){
		You("do not give offerings to the God of the future.");
		return MOVE_CANCELLED;
	}
	
	if(u.ualign.god == GOD_BOKRUG__THE_WATER_LIZARD 
		&& (a_gnum(u.ux, u.uy) == GOD_BOKRUG__THE_WATER_LIZARD 
			|| (a_align(u.ux, u.uy) == A_NONE && a_gnum(u.ux, u.uy) == GOD_NONE))
	){
		if (otmp->otyp == CORPSE)
			feel_cockatrice(otmp, TRUE);
		pline1(nothing_happens);
		return MOVE_STANDARD;
	}
	
	if (a_gnum(u.ux, u.uy) == GOD_THE_BLACK_MOTHER && u.shubbie_atten) {
		if (otmp->otyp == CORPSE)
			feel_cockatrice(otmp, TRUE);
		pline1(nothing_happens);
		return MOVE_STANDARD;
	}

	if(Misotheism && !(otmp->otyp == AMULET_OF_YENDOR && Is_astralevel(&u.uz))){
		if (otmp->otyp == CORPSE)
			feel_cockatrice(otmp, TRUE);
		pline1(nothing_happens);
		return MOVE_STANDARD;
	}

#define MAXVALUE 24 /* Highest corpse value (besides Wiz) */

    if (otmp->otyp == CORPSE) {
		register struct permonst *ptr = &mons[otmp->corpsenm];
		struct monst *mtmp;
		extern const int monstr[];

		/* KMH, conduct */
		u.uconduct.gnostic++;

		/* you're handling this corpse, even if it was killed upon the altar */
		feel_cockatrice(otmp, TRUE);

		if ((otmp->corpsenm == PM_ACID_BLOB
			|| (monstermoves <= peek_at_iced_corpse_age(otmp) + 50)
			) && mons[otmp->corpsenm].mlet != S_PLANT
		) {
			value = monstr[otmp->corpsenm] + 1;
			if (otmp->oeaten)
			value = eaten_stat(value, otmp);
		}

		if (your_race(ptr) && !is_animal(ptr) && !mindless(ptr) && u.ualign.type != A_VOID) {
			if (is_demon(youracedata)) {
				You("find the idea very satisfying.");
				exercise(A_WIS, TRUE);
			} else if ((u.ualign.type != A_CHAOTIC && u.ualign.type != A_NONE) || altaralign != A_CHAOTIC) {
				if((u.ualign.record >= 20 || ACURR(A_WIS) >= 20 || u.ualign.record >= rnd(20-ACURR(A_WIS))) && !roll_madness(MAD_CANNIBALISM)){
					char buf[BUFSZ];
					Sprintf(buf, "You feel a deep sense of kinship to %s!  Sacrifice %s anyway?",
						the(xname(otmp)), (otmp->quan == 1L) ? "it" : "one");
					if (yn_function(buf,ynchars,'n')=='n') return MOVE_CANCELLED;
				}
				pline("You'll regret this infamous offense!");
				exercise(A_WIS, FALSE);
			}

			if (altaralign != A_CHAOTIC && altaralign != A_NONE) {
				/* curse the lawful/neutral altar */
				if(Race_if(PM_INCANTIFIER)) pline_The("altar is stained with human blood, the blood of your birth race.");
				else pline_The("altar is stained with %s blood.", urace.adj);
				if(!Is_astralevel(&u.uz)) {
					a_align(u.ux, u.uy) = A_CHAOTIC;
					a_gnum(u.ux, u.uy) = GOD_NONE;
				}
				angry_priest();
			} else {
				struct monst *dmon;
				const char *demonless_msg;

				/* Human sacrifice on a chaotic or unaligned altar */
				/* is equivalent to demon summoning */
				if (altaralign == A_CHAOTIC && u.ualign.type != A_CHAOTIC) {
					pline(
					 "The blood floods the altar, which vanishes in %s cloud!",
					  an(hcolor(NH_BLACK)));
					levl[u.ux][u.uy].typ = ROOM;
					levl[u.ux][u.uy].altar_num = 0;
					/* todo: notice that the altar is gone in #overview */
					newsym(u.ux, u.uy);
					angry_priest();
					demonless_msg = "cloud dissipates";
				} else {
					/* either you're chaotic or altar is Moloch's or both */
					pline_The("blood covers the altar!");
					change_luck(altaralign == A_NONE ? -2 : 2);
					demonless_msg = "blood coagulates";
				}
				if ((pm = dlord((struct permonst *) 0, altaralign)) != NON_PM &&
					(dmon = makemon(&mons[pm], u.ux, u.uy, NO_MM_FLAGS))) {
					You("have summoned %s!", a_monnam(dmon));
					if (sgn(u.ualign.type) == sgn(dmon->data->maligntyp))
					dmon->mpeaceful = TRUE;
					You("are terrified, and unable to move.");
					nomul(-3, "being terrified of a demon");
				} else pline_The("%s.", demonless_msg);
			}

			if (u.ualign.type != A_CHAOTIC && u.ualign.type != A_NONE) {
				adjalign(-5);
				godlist[u.ualign.god].anger += 3;
				(void) adjattrib(A_WIS, -1, TRUE);
				if (!Inhell) angrygods(u.ualign.god);
				change_luck(-5);
			} else {
				adjalign(5);
				/* create Dirge from player's longsword here if possible */
				if (Role_if(PM_KNIGHT) && godlist[u.ualign.god].anger == 0 && u.ualign.record > 0
					&& uwep && (uwep->oclass == WEAPON_CLASS || is_weptool(uwep))
					&& !is_lightsaber(uwep)
					&& !uwep->oartifact && !(uarmh && uarmh->otyp == HELM_OF_OPPOSITE_ALIGNMENT)
					&& !art_already_exists(ART_DIRGE)
				) {
					pline("Your %s melts in your hand and transforms into something new!", xname(uwep));
					uwep = oname(uwep, artiname(ART_DIRGE));
					discover_artifact(ART_DIRGE);

					if (uwep->spe < 0)
						uwep->spe = 0;
					uwep->oeroded = uwep->oeroded2 = 0;
					uwep->oerodeproof = TRUE;

					exercise(A_WIS, TRUE);

					char llog[BUFSZ+22];
					Sprintf(llog, "was given %s", the(artilist[uwep->oartifact].name));
					livelog_write_string(llog);
				}
			}	
			if (carried(otmp)) useup(otmp);
			else useupf(otmp, 1L);
	
			return MOVE_STANDARD;
		} else if (get_ox(otmp, OX_EMON)
				&& ((mtmp = get_mtraits(otmp, FALSE)) != (struct monst *)0)
				&& mtmp->mtame) {
			/* mtmp is a temporary pointer to a tame monster's attributes,
			 * not a real monster */
			pline("So this is how you repay loyalty?");
			adjalign(-3);
			value = -1;
			HAggravate_monster |= TIMEOUT_INF;
		} else if (is_undead(ptr)) { /* Not demons--no demon corpses */
			if (u.ualign.type != A_CHAOTIC && u.ualign.type != A_NONE)
			value += 1;
		} else if (is_unicorn(ptr)) {
			int unicalign = sgn(ptr->maligntyp);

			/* If same as altar, always a very bad action. */
			if (unicalign == altaralign) {
			pline("Such an action is an insult to %s!",
				  (u.ualign.type == A_NONE) ? "me" : (unicalign == A_CHAOTIC)
				  ? "chaos" : unicalign ? "law" : "balance");
			(void) adjattrib(A_WIS, -1, TRUE);
			value = -5;
			} else if (u.ualign.type == altaralign) {
			/* If different from altar, and altar is same as yours, */
			/* it's a very good action */
			if (u.ualign.record < ALIGNLIM)
				You_feel("appropriately %s.", align_str(u.ualign.type));
			else You_feel("you are thoroughly on the right path.");
			adjalign(5);
			value += 3;
			} else
			/* If sacrificing unicorn of your alignment to altar not of */
			/* your alignment, your god gets angry and it's a conversion */
			if (unicalign == u.ualign.type) {
				u.ualign.record = -1;
				value = 1;
			} else value += 3;
		}
    } /* corpse */

    if (otmp->otyp == AMULET_OF_YENDOR) {
		if (!Is_astralevel(&u.uz)) {
			if (Hallucination)
				You_feel("homesick.");
			else
				You_feel("an urge to return to the surface.");
			return MOVE_STANDARD;
		} else {
			/* The final Test.	Did you win? */
			if(uamul == otmp) Amulet_off();
			u.uevent.ascended = 1;
			if(carried(otmp)) useup(otmp); /* well, it's gone now */
			else useupf(otmp, 1L);
			You("offer the Amulet of Yendor to %s...", a_gname());
			if(!Role_if(PM_EXILE)){
				if (u.ualign.type != altaralign) {
					/* And the opposing team picks you up and
					   carries you off on their shoulders */
					adjalign(-99);
					pline("%s accepts your gift, and gains dominion over %s...",
						  a_gname(), u_gname());
					pline("%s is enraged...", u_gname());
					pline("Fortunately, %s permits you to live...", a_gname());
					pline("A cloud of %s smoke surrounds you...",
						  hcolor((const char *)"orange"));
					done(ESCAPED);
				} else { /* super big win */
					adjalign(10);
#ifdef RECORD_ACHIEVE
					achieve.ascended = 1;
					give_ascension_trophy();
#endif
					pline("An invisible choir sings, and you are bathed in radiance...");
					godvoice(altargod, "Congratulations, mortal!");
					display_nhwindow(WIN_MESSAGE, FALSE);
					verbalize("In return for thy service, I grant thee the gift of Immortality!");
					You("ascend to the status of Demigod%s...",
						flags.female ? "dess" : "");
					done(ASCENDED);
				}
			} else {
				if (altaralign == A_LAWFUL) {
					/* And the opposing team picks you up and
					carries you off on their shoulders */
					adjalign(-99);
					pline("%s accepts your gift, and regains complete control over his creation.", a_gname());
					pline("In that instant, you lose all your powers as %s shuts the Gate.", a_gname());
					pline("Fortunately, %s permits you to live...", a_gname());
					pline("Occasionally, you may even be able to remember that you have forgotten something.");
					pline("A cloud of %s smoke surrounds you...",
						hcolor((const char *)"orange"));
					done(ESCAPED);
				} else if(altaralign == A_CHAOTIC) {
					/* And the opposing team picks you up squishes you like a bug */
					adjalign(-99);
					pline("%s accepts your gift, and gains complete control over creation.", a_gname());
					pline("In the next instant, she destroys it.");
					pline("You are functionally dead, your soul shorn from its earthly husk...");
					pline("...as well as everything that made you YOU.");
					killer_format = KILLED_BY;
					killer = "the end of the world."; //8-bit theater
					done(DISINTEGRATED);
				} else { /* super big win */
					adjalign(10);
#ifdef RECORD_ACHIEVE
					achieve.ascended = 1;
					give_ascension_trophy();
#endif
					pline("From the threshold of the Gate, you look back at the world");
					pline("You don't know what awaits you in the Void,");
					pline("but whatever happens, the way shall remain open behind you,");
					pline("that others may make their own choice.");
					done(ASCENDED);
				}
			}
		}
    } /* real Amulet */

    if (otmp->otyp == FAKE_AMULET_OF_YENDOR) {
	    if (flags.soundok)
		You_hear("a nearby thunderclap.");
	    if (!otmp->known) {
		You("realize you have made a %s.",
		    Hallucination ? "boo-boo" : "mistake");
		otmp->known = TRUE;
		change_luck(-1);
		return MOVE_STANDARD;
	    } else {
		/* don't you dare try to fool the gods */
		if(u.ualign.type != A_VOID){
			change_luck(-3);
			adjalign(-1);
			godlist[u.ualign.god].anger += 3;
			value = -3;
			u.lastprayresult = PRAY_ANGER;
			u.lastprayed = moves;
			u.reconciled = REC_NONE;
		}
	    }
    } /* fake Amulet */

    if (value == 0) {
	pline1(nothing_happens);
	return MOVE_STANDARD;
    }

    if (altaralign != u.ualign.type &&
	(Is_astralevel(&u.uz) || Is_sanctum(&u.uz))) {
	/*
	 * REAL BAD NEWS!!! High altars cannot be converted.  Even an attempt
	 * gets the god who owns it truely pissed off.
	 */
	You_feel("the air around you grow charged...");
	pline("Suddenly, you realize that %s has noticed you...", a_gname());
		godvoice(altargod, "So, mortal!  You dare desecrate my High Temple!");
	/* Throw everything we have at the player */
	god_zaps_you(altargod);
    } else if (value < 0) { /* I don't think the gods are gonna like this... */
		gods_upset(altargod);
    } else {
	int saved_anger = godlist[u.ualign.god].anger;
	int saved_cnt = u.ublesscnt;
	int saved_luck = u.uluck;

	/* Sacrificing at an altar of a different alignment OR a different god */
	if ((a_gnum(u.ux, u.uy) != GOD_NONE) ?
			(a_gnum(u.ux, u.uy) != u.ualign.god) : (u.ualign.type != altaralign)
		){
	    /* Is this a conversion ? */
	    /* An unaligned altar in Gehennom will always elicit rejection. */
	    if ((ugod_is_angry() && u.ualign.type != A_VOID) || (altaralign == A_NONE && Inhell)) {
		if(u.ugodbase[UGOD_CURRENT] == u.ugodbase[UGOD_ORIGINAL] && god_accepts_you(altargod)) {
		    You("have a strong feeling that %s is angry...", u_gname());
			if(otmp->otyp == CORPSE && is_rider(&mons[otmp->corpsenm])){
				pline("A pulse of darkness radiates from your sacrifice!");
				angrygods(altargod);
				return MOVE_STANDARD;
			}
			consume_offering(otmp);
		    pline("%s accepts your allegiance.", a_gname());

		    /* The player wears a helm of opposite alignment? */
		    if (uarmh && uarmh->otyp == HELM_OF_OPPOSITE_ALIGNMENT)
				u.ugodbase[UGOD_CURRENT] = altargod;
		    else {
				u.ualign.god = u.ugodbase[UGOD_CURRENT] = altargod;
				u.ualign.type = altaralign;
			}
		    u.ublessed = 0;
		    flags.botl = 1;

		    You("have a sudden sense of a new direction.");
		    /* Beware, Conversion is costly */
		    change_luck(-3);
		    u.ublesscnt += 300;
		    u.lastprayed = moves;
			u.reconciled = REC_NONE;
		    u.lastprayresult = PRAY_CONV;
		    adjalign((int)(galign(u.ugodbase[UGOD_ORIGINAL]) * (ALIGNLIM / 2)));
			if(Race_if(PM_DROW)){
				switch(galign(u.ugodbase[UGOD_ORIGINAL])){
					case A_LAWFUL:
						if(Role_if(PM_NOBLEMAN)){
							if(flags.initgend){
								u.uhouse = VER_TAS_SYMBOL;
							} else {
								u.uhouse = VER_TAS_SYMBOL;
							}
						} else {
							if(flags.initgend){
								u.uhouse = EILISTRAEE_SYMBOL;
							} else {
								u.uhouse = EDDER_SYMBOL;
							}
						}
					break;
					case A_NEUTRAL:
						if(Role_if(PM_NOBLEMAN)){
							if(flags.initgend){
								u.uhouse = KIARANSALEE_SYMBOL;
							} else {
								u.uhouse = u.start_house; /*huh?*/
							}
						} else {
							if(flags.initgend){
								u.uhouse = KIARANSALEE_SYMBOL;
							} else {
								u.uhouse = u.start_house; /*huh?*/
							}
						}
					break;
					case A_CHAOTIC:
						if(Role_if(PM_NOBLEMAN)){
							if(flags.initgend){
								u.uhouse = u.start_house; /*huh?*/
							} else {
								u.uhouse = GHAUNADAUR_SYMBOL;
							}
						} else {
							if(flags.initgend){
								u.uhouse = u.start_house; /*huh?*/
							} else {
								u.uhouse = u.start_house; /*Converted to direct Lolth worship*/
							}
						}
					break;
				}
			}
		} else {
			//???  || u.detestation_ritual
			if(altaralign == A_VOID){
				consume_offering(otmp);
				if (!Inhell){
					godvoice(u.ualign.god, "Suffer, infidel!");
					godlist[u.ualign.god].anger += 3;
					adjalign(-5);
					u.lastprayed = moves;
					u.lastprayresult = PRAY_ANGER;
					u.reconciled = REC_NONE;
					change_luck(-5);
					(void) adjattrib(A_WIS, -2, TRUE);
					angrygods(u.ualign.god);
				} else {
					pline("Silence greets your offering.");
				}
			} else {
				godlist[u.ualign.god].anger += 3;
				adjalign(-5);
				u.lastprayed = moves;
				u.lastprayresult = PRAY_ANGER;
				u.reconciled = REC_NONE;
				Strcpy(buf, a_gname());
				pline("%s rejects your sacrifice!", upstart(buf));
				godvoice(altargod, "Suffer, infidel!");
				change_luck(-5);
				(void) adjattrib(A_WIS, -2, TRUE);
				if (!Inhell) angrygods(u.ualign.god);
			}
		}
		return MOVE_STANDARD;
	    } else {
		if(otmp->otyp == CORPSE && is_rider(&mons[otmp->corpsenm])){
			pline("A pulse of darkness radiates from your sacrifice!");
			angrygods(altargod);
			return MOVE_STANDARD;
		}
		consume_offering(otmp);
		if(Role_if(PM_EXILE) && u.ualign.type != A_VOID && altaralign != A_VOID){
			You("sense a conference between %s and %s.",
				u_gname(), a_gname());
			pline("But nothing else occurs.");
		} else if(u.ualign.god == GOD_BOKRUG__THE_WATER_LIZARD){
			You("sense %s prepare for a conflict....",
				a_gname());
			pline("But nothing else occurs.");
		} else {
			You("sense a conflict between %s and %s.",
				u_gname(), a_gname());
			if (rn2(8 + u.ulevel) > 5) {
				struct monst *pri;
				You_feel("the power of %s increase.", u_gname());
				exercise(A_WIS, TRUE);
				change_luck(1);
				a_align(u.ux, u.uy) = u.ualign.type;
				a_gnum(u.ux, u.uy) = GOD_NONE;
				if (!Blind)
				pline_The("altar glows %s.",
					  hcolor(
					  u.ualign.type == A_LAWFUL ? NH_WHITE :
					  u.ualign.type ? NH_BLACK : (const char *)"gray"));
				if(Role_if(PM_EXILE) && In_quest(&u.uz) && u.uz.dlevel == nemesis_level.dlevel){
					int door = 0, ix, iy;
					if(altaralign == A_CHAOTIC) door = 1;
					else if(altaralign == A_NEUTRAL) door = 2;
					else if(altaralign == A_LAWFUL) door = 3;
					else if(!u.uevent.qcompleted && altaralign == A_NONE){
						pline("The muted tension that filled the wind fades away.");
						pline("You here again the thousand-tounged whisperers,");
						pline("though you cannot make out the name they repeat.");
						makemon(&mons[PM_ACERERAK],u.ux,u.uy,MM_ADJACENTOK);
						pline("Someone now stands beside you!");
					}
					if(door){
						for(ix = 1; ix < COLNO; ix++){
							for(iy = 0; iy < ROWNO; iy++){
								if(IS_DOOR(levl[ix][iy].typ) && artifact_door(ix,iy) == door){
									You_hear("a door open.");
									levl[ix][iy].typ = ROOM;
									unblock_point(ix,iy);
								}
							}
						}
					}
				}
				// if (rnl(u.ulevel) > 6 && u.ualign.record > 0 &&
				   // rnd(u.ualign.record) > (3*ALIGNLIM)/4)
				if(!gods_are_friendly(altargod, u.ualign.god)){
					if(u.ulevel > 20) summon_god_minion(altargod, FALSE);
					if(u.ulevel >= 14) summon_god_minion(altargod, FALSE);
					(void) summon_god_minion(altargod, TRUE);
				}
				/* anger priest; test handles bones files */
				if((pri = findpriest(temple_occupied(u.urooms))) && !p_coaligned(pri))
					angry_priest();
			} else {
				pline("Unluckily, you feel the power of %s decrease.", u_gname());
				change_luck(-1);
				exercise(A_WIS, FALSE);
				if(!gods_are_friendly(altargod, u.ualign.god)){
					if(u.ulevel > 20) summon_god_minion(altargod, TRUE);
					if(u.ulevel > 10) summon_god_minion(altargod, TRUE);
					(void) summon_god_minion(altargod, TRUE);
				}
			}
		}
		return MOVE_STANDARD;
	    }
	}

	if(otmp->otyp == CORPSE && is_rider(&mons[otmp->corpsenm])){
		pline("A pulse of darkness radiates from your sacrifice!");
		angrygods(altargod);
		return MOVE_STANDARD;
	}
	consume_offering(otmp);
	/* OK, you get brownie points. */
	if(godlist[u.ualign.god].anger > 0) {
	    godlist[u.ualign.god].anger -=
		((value * (u.ualign.type == A_CHAOTIC ? 2 : 3)) / MAXVALUE);
	    if(godlist[u.ualign.god].anger < 0) godlist[u.ualign.god].anger = 0;
	    if(godlist[u.ualign.god].anger != saved_anger) {
		if (godlist[u.ualign.god].anger) {
		    pline("%s seems %s.", u_gname(),
			  Hallucination ? "groovy" : "slightly mollified");

		    if ((int)u.uluck < 0) change_luck(1);
		} else {
		    pline("%s seems %s.", u_gname(), Hallucination ?
			  "cosmic (not a new fact)" : "mollified");

		    if ((int)u.uluck < 0) u.uluck = 0;
		    u.reconciled = REC_MOL;
		}
	    } else { /* not satisfied yet */
		if (Hallucination)
		    pline_The("gods seem tall.");
		else You("have a feeling of inadequacy.");
	    }
	} else if(ugod_is_angry()) {
	    if(value > MAXVALUE) value = MAXVALUE;
	    if(value > -u.ualign.record) value = -u.ualign.record;
	    adjalign(value);
	    You_feel("partially absolved.");
	} else if (u.ublesscnt > 0) {
	    u.ublesscnt -=
		((value * (u.ualign.type == A_CHAOTIC ? 500 : 300)) / MAXVALUE);
	    if(u.ublesscnt < 0) u.ublesscnt = 0;
	    if(u.ublesscnt != saved_cnt) {
		if (u.ublesscnt) {
		    if (Hallucination)
			You("realize that the gods are not like you and I.");
		    else
			You("have a hopeful feeling.");
		    if ((int)u.uluck < 0) change_luck(1);
		} else {
		    if (Hallucination)
			pline("Overall, there is a smell of fried onions.");
		    else
			You("have a feeling of reconciliation.");
		    if ((int)u.uluck < 0) u.uluck = 0;
			
			u.reconciled = REC_REC;
		}
	    }
	} else {
		//pline("looking into an artifact gift.  %d gift val accumulated. %d gifts have been given, on level %d, and your luck %d.", u.uartisval, (int)u.ugifts, u.ulevel, (int)u.uluck);
	    /* you were already in pretty good standing */
	    /* The player can gain an artifact */
	    /* The chance goes down as the number of artifacts goes up */
		/* Priests now only count gifts in this calculation, found artifacts are excluded */
		if (u.ulevel > 2 && u.uluck >= 0 && maybe_god_gives_gift()) {
		otmp = mk_artifact((struct obj *)0, (a_align(u.ux,u.uy)));
		if (otmp) {
		    if (otmp->spe < 0) otmp->spe = 0;
		    if (otmp->cursed) uncurse(otmp);
			if(otmp->oartifact != ART_PEN_OF_THE_VOID)
			    otmp->oerodeproof = TRUE;
		    dropy(otmp);
		    at_your_feet("An object");
		    godvoice(u.ualign.god, "Use my gift wisely!");
			otmp->gifted = u.ualign.god;
			u.ugifts++;
			u.uartisval += arti_value(otmp);
		    u.ublesscnt = rnz(300 + (u.uartisval * 10));
			//u.ublesscnt = rnz(300 + (u.ugifts * 50));
			u.lastprayed = moves;
			u.reconciled = REC_NONE;
			u.lastprayresult = PRAY_GIFT;
		    exercise(A_WIS, TRUE);
		    if (!flags.debug && otmp->oartifact) {
				char llog[BUFSZ+22];
				Sprintf(llog, "was given %s", the(artilist[otmp->oartifact].name));
				livelog_write_string(llog);
		    }
		    /* make sure we can use this weapon */
		    unrestrict_weapon_skill(weapon_type(otmp));
		    discover_artifact(otmp->oartifact);
			if(otmp->oartifact == ART_BLADE_SINGER_S_SABER){
				unrestrict_weapon_skill(P_SABER);
				unrestrict_weapon_skill(P_DAGGER);
				unrestrict_weapon_skill(P_TWO_WEAPON_COMBAT);
			} else if(otmp->oartifact == ART_BEASTMASTER_S_DUSTER){
				unrestrict_weapon_skill(P_BEAST_MASTERY);
			} else if(otmp->oartifact == ART_GRANDMASTER_S_ROBE || otmp->oartifact == ART_PREMIUM_HEART || otmp->oartifact == ART_GODHANDS){
				unrestrict_weapon_skill(P_BARE_HANDED_COMBAT);
				u.umartial = TRUE;
			} else if(otmp->oartifact == ART_RHONGOMYNIAD){
				unrestrict_weapon_skill(P_RIDING);
				mksobj_at(SADDLE, u.ux, u.uy, MKOBJ_NOINIT);
				u.umartial = TRUE;
			} else if(otmp->oartifact == ART_GOLDEN_SWORD_OF_Y_HA_TALLA){
				unrestrict_weapon_skill(P_WHIP);
			} else if(otmp->oartifact == ART_HELLRIDER_S_SADDLE){
				unrestrict_weapon_skill(P_RIDING);
			}
			
			if(is_shield(otmp)){
				unrestrict_weapon_skill(P_SHIELD);
			}
			return MOVE_STANDARD;
		}
	    } else if (rnl((30 + u.ulevel)*10) < 10) {
			/* no artifact, but maybe a helpful pet? */
			/* WAC is now some generic benefit (includes pets) */
			god_gives_benefit(altaralign);
		    return MOVE_STANDARD;
	    }
	    change_luck((value * LUCKMAX) / (MAXVALUE * 2));
	    if ((int)u.uluck < 0) u.uluck = 0;
	    if (u.uluck != saved_luck) {
		if (Blind)
		    You("think %s brushed your %s.",something, body_part(FOOT));
		else You(Hallucination ?
		    "see crabgrass at your %s.  A funny thing in a dungeon." :
		    "glimpse a four-leaf clover at your %s.",
		    makeplural(body_part(FOOT)));
	    }
		u.reconciled = REC_REC;
	}
    }
    return MOVE_STANDARD;
}


/* determine prayer results in advance; also used for enlightenment */
boolean
can_pray(praying)
boolean praying;	/* false means no messages should be given */
{
    int alignment;

    p_god = on_altar() ? (god_at_altar(u.ux,u.uy)) : u.ualign.god;
    p_trouble = in_trouble();

    if (is_demon(youracedata) && (galign(p_god) == A_LAWFUL || galign(p_god) == A_NEUTRAL)) {
	if (praying)
	    pline_The("very idea of praying to a %s god is repugnant to you.",
		  godlist[p_god].alignment ? "lawful" : "neutral");
	return FALSE;
    }

    if (praying)
	You("begin praying to %s.", godname(p_god));
	
	// Dungeon currently cut off from the divine
	if(praying && Misotheism){
		p_type = 1;	
		return TRUE;
	}

    if (u.ualign.type && u.ualign.type == -galign(p_god))
	alignment = -u.ualign.record;		/* Opposite alignment altar */
    else if (u.ualign.type != galign(p_god))
	alignment = u.ualign.record / 2;	/* Different alignment altar */
    else alignment = u.ualign.record;

    if ((int)Luck < 0 || godlist[u.ualign.god].anger || alignment < 0)
        p_type = 0;             /* too naughty... */
    else if (u.ualign.god == GOD_BOKRUG__THE_WATER_LIZARD)
		p_type = 1;		/* always too soon */
    else if ((p_trouble > 0) ? (u.ublesscnt > 200) : /* big trouble */
		(p_trouble < 0) ? (u.ublesscnt > 100) : /* minor difficulties */
		(u.ublesscnt > 0) /* not in trouble */
	)
		p_type = 1;		/* too soon... */
    else /* alignment >= 0 */ {
	if(on_altar() && u.ualign.god != p_god)
	    p_type = 2;
	else
	    p_type = 3;
    }

    if (is_undead(youracedata) && (!Inhell || hell_safe_prayer(p_god)) &&
		(gholiness(p_god) == HOLY_HOLINESS || (gholiness(p_god) == NEUTRAL_HOLINESS && !rn2(10))))
	p_type = -1;
    /* Note:  when !praying, the random factor for neutrals makes the
       return value a non-deterministic approximation for enlightenment.
       This case should be uncommon enough to live with... */

    return !praying ? (boolean)(p_type == 3 && (!Inhell || hell_safe_prayer(p_god))) : TRUE;
}

int
dopray()
{
	if(Role_if(PM_ANACHRONONAUT) && flags.questprogress!=2){
		pline("There is but one God in the future.");
		pline("And to It, you do not pray.");
		return MOVE_CANCELLED;
	}
	
	if(flat_mad_turn(MAD_APOSTASY)){
		pline("You can't bring yourself to pray.");
		return MOVE_CANCELLED;
	}
	
	if(Doubt){
		pline("You're suffering a crisis of faith.");
		return MOVE_CANCELLED;
	}
	
    /* Confirm accidental slips of Alt-P */
    if (flags.prayconfirm)
	if (yn("Are you sure you want to pray?") == 'n')
	    return MOVE_CANCELLED;

	if(u.sealsActive&SEAL_AMON) unbind(SEAL_AMON,TRUE);
    u.uconduct.gnostic++;
    /* Praying implies that the hero is conscious and since we have
       no deafness attribute this implies that all verbalized messages
       can be heard.  So, in case the player has used the 'O' command
       to toggle this accessible flag off, force it to be on. */
    flags.soundok = 1;

    /* set up p_type and p_alignment */
    if (!can_pray(TRUE)) return MOVE_CANCELLED;
	
	u.lastprayed = moves;
	u.lastprayresult = PRAY_INPROG;
	u.reconciled = REC_NONE;
#ifdef WIZARD
    if (wizard && p_type >= 0) {
	if (yn("Force the gods to be pleased?") == 'y') {
	    u.ublesscnt = 0;
	    if (u.uluck < 0) u.uluck = 0;
	    if (u.ualign.record <= 0) u.ualign.record = 1;
	    godlist[u.ualign.god].anger = 0;
	    if(p_type < 2) p_type = 3;
	}
    }
#endif
    nomul(-3, "praying");
    nomovemsg = "You finish your prayer.";
    afternmv = prayer_done;

    if((p_type == 3 && (!Inhell || hell_safe_prayer(p_god)))
	|| (uwep && uwep->oartifact == ART_LANCE_OF_LONGINUS && rn2(7))
	){
	/* if you've been true to your god you can't die while you pray */
	if (!Blind)
	    You("are surrounded by a shimmering light.");
	u.uinvulnerable = TRUE;
    }

    return MOVE_STANDARD;
}

STATIC_PTR int
prayer_done()		/* M. Stephenson (1.0.3b) */
{
    aligntyp alignment = galign(p_god);

    u.uinvulnerable = FALSE;
	u.lastprayresult = PRAY_GOOD;
	if(u.ualign.god == GOD_BOKRUG__THE_WATER_LIZARD){
		u.lastprayresult = PRAY_IGNORED;
		Your("prayer goes unanswered.");
		return MOVE_STANDARD; //I think this is meaningless?
	}
    if(p_type == -1) {
		godvoice(p_god,
			 gholiness(p_god) == HOLY_HOLINESS ?
			 "Vile creature, thou durst call upon me?" :
			 "Walk no more, perversion of nature!");
		You_feel("like you are falling apart.");
	/* KMH -- Gods have mastery over unchanging */
	/* but not racial vampirism or the helm of undeath */
	if (!Race_if(PM_VAMPIRE) && !(HUnchanging & FROMOUTSIDE)) {
		u.lastprayresult = PRAY_GOOD;
		rehumanize();
		losehp(rnd(20), "residual undead turning effect", KILLED_BY_AN);
	} else {
		u.lastprayresult = PRAY_BAD;
	   /* Starting vampires are inherently vampiric */
	   losehp(rnd(20), "undead turning effect", KILLED_BY_AN);
	   pline("You get the idea that %s will be of %s help to you.",
			godname(p_god),
			gholiness(p_god) == HOLY_HOLINESS ?
			"little" :
			"at best sporadic");
	}
	exercise(A_CON, FALSE);
	if(on_altar()){
		(void) water_prayer(FALSE);
		change_luck(-3);
		gods_upset(p_god);
	}
	return MOVE_STANDARD;
    }
    if (Inhell && !hell_safe_prayer(p_god)){
		pline("Since you are in Gehennom, %s won't help you.", godname(p_god));
		/* haltingly aligned is least likely to anger */
		if (u.ualign.record <= 0 || rnl(u.ualign.record))
			angrygods(u.ualign.god);
		return MOVE_CANCELLED;
    }

    if (p_type == 0) {
        if(on_altar() && u.ualign.god != p_god)
            (void) water_prayer(FALSE);
        angrygods(u.ualign.god);       /* naughty */
    } else if (p_type == 1) {
		if(on_altar() && u.ualign.god != p_god)
			(void) water_prayer(FALSE);
		if(u.ualign.type != A_VOID){
			u.ublesscnt += rnz(250);
			change_luck(-3);
			gods_upset(u.ualign.god);
		}
    } else if(p_type == 2) {
		if(water_prayer(FALSE)) {
			/* attempted water prayer on a non-coaligned altar */
			u.ublesscnt += rnz(250);
			change_luck(-3);
			if(u.ualign.type != A_VOID) gods_upset(u.ualign.god);
		} else pleased(p_god);
    } else {
	/* coaligned */
	if(on_altar())
	    (void) water_prayer(TRUE);
	pleased(p_god); /* nice */
    }
    return MOVE_STANDARD;
}

STATIC_OVL
int
turn_level(mtmp)
struct monst *mtmp;
{
	boolean youdef = mtmp == &youmonst;
	int xlev = youdef ? u.ulevel : mtmp->data->mlevel;
	if(youdef){
		if(youracedata->mlet == S_ZOMBIE)
			xlev = 6;
		else if(youracedata->mlet == S_MUMMY)
			xlev = 8;
		else if(youracedata->mlet == S_WRAITH)
			xlev = 10;
		else if(youracedata->mlet == S_VAMPIRE)
			xlev = 12;
		else if(youracedata->mlet == S_GHOST)
			xlev = 14;
		else if(youracedata->mlet == S_SHADE)
			xlev = 16;
		else if(youracedata->mlet == S_LICH)
			xlev = 18;
	}
	else {
		/* lichs are tougher than zombies, but monster level generally doesn't help much. */
		if(mtmp->data->mlet == S_ZOMBIE || has_template(mtmp, ZOMBIFIED) || has_template(mtmp, SKELIFIED))
			xlev = 6;
		else if(mtmp->data->mlet == S_MUMMY || has_template(mtmp, M_BLACK_WEB))
			xlev = 8;
		else if(mtmp->data->mlet == S_WRAITH || has_template(mtmp, FRACTURED))
			xlev = 10;
		else if(mtmp->data->mlet == S_VAMPIRE || has_template(mtmp, VAMPIRIC))
			xlev = 12;
		else if(mtmp->data->mlet == S_GHOST)
			xlev = 14;
		else if(mtmp->data->mlet == S_SHADE)
			xlev = 16;
		else if(mtmp->data->mlet == S_LICH)
			xlev = 18;
	}
	return xlev;
}

int
doturn()
{	/* Knights & Priest(esse)s only please */
	struct monst *mtmp, *mtmp2;
	int once, range, xlev;
	short fast = 0;

	if (!Role_if(PM_PRIEST) && !Role_if(PM_KNIGHT) && !Race_if(PM_VAMPIRE) && !(Role_if(PM_NOBLEMAN) && Race_if(PM_ELF))){
		/* Try to use turn undead spell. */
		if (objects[SPE_TURN_UNDEAD].oc_name_known) {
		    register int sp_no;
		    for (sp_no = 0; sp_no < MAXSPELL &&
			 spl_book[sp_no].sp_id != NO_SPELL &&
			 spl_book[sp_no].sp_id != SPE_TURN_UNDEAD; sp_no++);

		    if (sp_no < MAXSPELL &&
			spl_book[sp_no].sp_id == SPE_TURN_UNDEAD)
			    return spelleffects(sp_no, TRUE, 0);
		}

		You("don't know how to turn undead!");
		return MOVE_CANCELLED;
	}
	if(Misotheism){
		pline("Nothing happens!");
		return MOVE_CANCELLED;
	}

	if(!Race_if(PM_VAMPIRE)) u.uconduct.gnostic++;

	if(!Race_if(PM_VAMPIRE) && u.uen >= 30 && yn("Use abbreviated liturgy?") == 'y'){
		fast = 1;
	}
	
	if ((u.ualign.type != A_CHAOTIC && u.ualign.type != A_NONE && !Race_if(PM_VAMPIRE) &&
		    (is_demon(youracedata) || is_undead(youracedata))) ||
				godlist[u.ualign.god].anger > 6 /* "Die, mortal!" */) {

		pline("For some reason, %s seems to ignore you.", u_gname());
		aggravate();
		exercise(A_WIS, FALSE);
		return MOVE_INSTANT;
	}

	if (Inhell && !hell_safe_prayer(u.ualign.god) && !Race_if(PM_VAMPIRE)) {
	    pline("Since you are in Gehennom, %s won't help you.", u_gname());
	    aggravate();
	    return MOVE_INSTANT;
	}
	if(!Race_if(PM_VAMPIRE)) pline("Calling upon %s, you chant holy scripture.", u_gname());
	else You("focus your vampiric aura!");
	exercise(A_WIS, TRUE);

	/* note: does not perform unturn_dead() on victims' inventories */
	range = BOLT_LIM + (u.ulevel / 5);	/* 5 to 11 */
	range *= range;
	once = 0;
	for(mtmp = fmon; mtmp; mtmp = mtmp2) {
	    mtmp2 = mtmp->nmon;

	    if (DEADMONSTER(mtmp)) continue;
	    if (!couldsee(mtmp->mx,mtmp->my) ||
			(Race_if(PM_VAMPIRE) && !canseemon(mtmp)) ||
			distu(mtmp->mx,mtmp->my) > range
		) continue;

	    if (!mtmp->mpeaceful && (is_undead(mtmp->data) ||
		   (is_demon(mtmp->data) && (u.ulevel > (MAXULEV/2))))) {

		    mtmp->msleeping = 0;
		    if (Confusion) {
				if (!once++){
					if(!Race_if(PM_VAMPIRE)) pline("Unfortunately, your voice falters.");
					else pline("Unfortunately, your concentration falters.");
				}
				if(mtmp->mtyp != PM_BANDERSNATCH) mtmp->mflee = 0;
				mtmp->mfrozen = 0;
				mtmp->mcanmove = 1;
				//Knows your position now!
				mtmp->mux = u.ux;
				mtmp->muy = u.uy;
		    } else if (!resist(mtmp, '\0', 0, TELL)) {
				if(is_undead(mtmp->data)){
					xlev = turn_level(mtmp);
					if (u.ulevel >= xlev && !resist(mtmp, '\0', 0, NOTELL)) {
						if (u.ualign.type == A_CHAOTIC || u.ualign.type == A_NONE || Race_if(PM_VAMPIRE)){
							mtmp->mpeaceful = 1;
							set_malign(mtmp);
							if(PM_VAMPIRE){
								//Should be temporary, or it will de-tame all your minions :(
								tamedog(mtmp, (struct obj *)0);
							}
						} else { /* damn them */
							killed(mtmp);
						}
					}
				}
				/* else flee */
				if(!DEADMONSTER(mtmp))
					monflee(mtmp, 0, FALSE, TRUE);
		    }
	    }
	}
	//Altered turn undead to consume energy if possible, otherwise take full time.
	if(fast){
		losepw(30);
		nomul(-1, "trying to turn the undead");
	}
	else
		nomul(-5, "trying to turn the undead");
	return MOVE_STANDARD;
}

const char *
a_gname()
{
    return(a_gname_at(u.ux, u.uy));
}

/* 
 * returns the name of an altar's deity
 * if the altar is undedicated, returns the name of the responsible god
 */
const char *
a_gname_at(x,y)
xchar x, y;
{
    if(!IS_ALTAR(levl[x][y].typ)) return((char *)0);
	
	return godname(god_at_altar(x, y));
}

const char *
u_gname()  /* returns the name of the player's deity */
{
    return godname(u.ualign.god);
}

const char * const hallu_gods[] = {
	"goodness",
	
	"the Yggdrasil Entity",
	"the Unbounded Blue",
	"the Silencer in the stars",
	"the Drowned",
	
	//TV tropes
	"the angels",
	"the devils",
	"the squid",
	
	//Games
	"Armok",
	"_Hylia",
	"_the three golden goddesses",
	"_the Lady of Pain",
	"the Outsider",
	"Yevon",
	"Bhunivelze",
	"_Etro",
	"the Transcendent One",
	"_the Mana Tree",
	"Golden Silver",
	"_Luna",
	"Arceus",
	"the Composer",
	"the Conductor",
	"Chakravartin",
	"Chattur'gha",
	"Ulyaoth",
	"_Xel'lo'tath",
	"Mantorok",
	"_Martel",
	
	//Literature
	"Mahasamatman",
	"Azathoth",
	"Ubbo-Sathla",
	"Galactus",
	"Sauron",
	"Morgoth",
	"Eru Iluvatar",
	"the Powers that Be",
	"the Lone Power",
	"Om",
	"small gods",
	"Orannis",
	
	"The Water Phoenix King",
	"_Ailari, Goddess of Safe Journeys",
	"_Yuenki, Dark Lady of Apotheosis",
	"_Zeth Kahl, Dark Empress of The Gift Given to Bind",
	
	//Anime
	"the Data Overmind",
	"the Sky Canopy Domain",
	
	"the Truth and the Gate",
	
	"Ceiling Cat", /* Lolcats */
	"Zoamelgustar", /* Slayers */
	"Brohm", /* Ultima */
	"Xenu", /* Scientology */
	"the God of Cabbage", /* K-On! */
	"Bill Cipher", /* Gravity Falls */
	"Gades", "Amon", "_Erim", "Daos", /* Lufia series */
        "_Beatrice", /* Umineko no Naku Koro ni */
	"Spongebob Squarepants", /* should be obvious, right? :) */
	"Chiyo-chichi", /* Azumanga Daioh */
	"The Wondrous Mambo god", /* Xenogears */
	
	//Internet
	"SCP-343",
	"the Slender Man",
	"the Powers What Is",
	
	//atheism
	"the universe",
	"the Flying Spaghetti Monster",
	"_the Invisible Pink Unicorn",
	"last thursday",
	
	//Economic systems
	"Capitalism",
	"Communism",

	//Ok, so I have no idea what this is....
	//<Muad> same thing as the UN
	//<Muad> if that fits the context
	"the U.N.O.",
	
	//Vague
	"something",
	"someone",
	"whatever",
	"thing",
	"thingy",
	"whatchamacallit",
	"whosamawhatsit",
	"some guy",
	"_some gal",
	
	//nethack
	"the gnome with the wand of death",
	"the DevTeam",
	"Dion Nicolaas",
	"marvin",
	
	"stth the first demigod",
	"stth the wizard",
	"_stth the valkyrie",
	"stth the barbarian",
	"stth the tourist",
	"stth the healer",
	"stth the pirate", /* <stth> LOL RIGHT... FIRST DNETHACK STREAK
						  <stth> tiny 2-game streak, but it's a streak lol 
					   */
	"allihaveismymind the second demigod",
	"allihaveismymind the vanilla valkyrie",
	"Khor the third demigod",
	"Khor the noble",
	"Khor the priest",
	"Khor the ranger",
	"Khor the monk",
	"Khor the gnomish ranger",
	"ChrisANG the fourth demigod",
	"ChrisANG the binder",
	"FIQ the fifth demigod",
	"Tariru the elven noble",
	"Tariru the dwarf knight",
	"Tariru the samurai",
	"Tariru the archeologist",
	"Tariru the droven noble",
	"VoiceOfReason the knight",
	"VoiceOfReason the dwarven noble",
	"HBane the anachrononaut",
	
	"Dudley",
	"the RNG"
};

const char *
align_gname(alignment)
aligntyp alignment;
{
	return godname(align_to_god(alignment));
}

/* deity's title */
const char *
gtitle(godnum)
int godnum;
{
	const char *gnam, *result = "god";
	gnam = godlist[godnum].name;
	if (gnam && *gnam == '_')
		result = "goddess";
	return result;
}

void
altar_wrath(x, y)
register int x, y;
{
	if(god_at_altar(x, y) == u.ualign.god) {
		godvoice(u.ualign.god, "How darest thou desecrate my altar!");
	(void) adjattrib(A_WIS, -1, FALSE);
	} else {
		pline("A voice (could it be %s?) whispers:", godname(god_at_altar(x, y)));
		verbalize("Thou shalt pay, infidel!");
		change_luck(-1);
	}
}

/* assumes isok() at one space away, but not necessarily at two */
STATIC_OVL boolean
blocked_boulder(dx,dy)
int dx,dy;
{
    register struct obj *otmp;
    long count = 0L;

    for(otmp = level.objects[u.ux+dx][u.uy+dy]; otmp; otmp = otmp->nexthere) {
	if(is_boulder(otmp))
	    count += otmp->quan;
    }

    switch(count) {
	case 0: return FALSE; /* no boulders--not blocked */
	case 1: break; /* possibly blocked depending on if it's pushable */
	default: return TRUE; /* >1 boulder--blocked after they push the top
	    one; don't force them to push it first to find out */
    }

    if (!isok(u.ux+2*dx, u.uy+2*dy))
	return TRUE;
    if (IS_ROCK(levl[u.ux+2*dx][u.uy+2*dy].typ))
	return TRUE;
    if (boulder_at(u.ux+2*dx, u.uy+2*dy))
	return TRUE;

    return FALSE;
}


/*
 * A candle on a coaligned altar burns brightly or dimly
 * depending on your prayer status.
 */
 
int
candle_on_altar(candle) 
struct obj *candle;
{
  if (candle->where != OBJ_FLOOR 
     || !IS_ALTAR(levl[candle->ox][candle->oy].typ)
     ||  (a_align(candle->ox, candle->oy)) != u.ualign.type) {

     return 0;
  }

  can_pray(FALSE);

  /* Return a value indicating the chances of successful prayer */
 
  return (p_type > 2 ? 1 : p_type - 1);
}

/* Give away something */
void
god_gives_benefit(godnum)
int godnum;
{
	register struct obj *otmp;
	const char *what = (const char *)0;
	int i, ii, lim, timeout;
	
	if (rnl((30 + u.ulevel)*10) < 10) god_gives_pet(godnum);
	else {
		switch (rn2(6)) {
			case 0: // randomly increment an ability score
				i = rn2(A_MAX);
				for (ii = 0; ii < A_MAX; ii++) {
					lim = AMAX(i);
					if (i == A_STR && u.uhs >= 3) --lim;
					if (ABASE(i) < lim) {
						ABASE(i) = lim;
						pline("Wow! You feel good!");
						break;
					}
					if (++i >= A_MAX) i = 0;
				}
				
				if (ii == A_MAX){
					i = rn2(A_MAX);
					for (ii = A_MAX; ii > 0; ii--) {
						if (adjattrib(i, 1, (ii == 1) ? 0 : -1))
							break;
						i = (i+1)%6;
					}
				}
				flags.botl = 1;
				break;
			case 1: // increase weapon enchantment
				otmp = (struct obj *)0;
				/* select object to enchant */
				if (uwep && uwep->spe < 5 && (uwep->oclass == WEAPON_CLASS || is_weptool(uwep)))
					otmp = uwep;
				else if (uswapwep && uswapwep->spe < 5 && (uswapwep->oclass == WEAPON_CLASS || is_weptool(uswapwep)))
					otmp = uswapwep;
				else if (martial_bonus() && uarmg && (uarmg->oartifact || !uwep) && uarmg->spe < 5)
					otmp = uarmg;
				else if (martial_bonus() && uarmf && (uarmf->oartifact || !uwep) && uarmf->spe < 5)
					otmp = uarmf;
				else if (uleft && uleft->otyp != RIN_WISHES && objects[uleft->otyp].oc_charged && uleft->spe < 5)
					otmp = uleft;
				else if (uright && uright->otyp != RIN_WISHES && objects[uright->otyp].oc_charged && uright->spe < 5)
					otmp = uright;
				/* enchant it */
				if (otmp) {
					otmp->spe++;
					if (!Blind) {
						Your("%s %s %s for a moment.",
							xname(otmp),
							vtense(xname(otmp), "glow"),
							hcolor(otmp->oclass == ARMOR_CLASS ? NH_SILVER :
								otmp->oclass == RING_CLASS ? NH_WHITE : NH_BLUE)
							);
					}
				}
				break;
			case 2: // identify an item
				if (uwep && not_fully_identified(uwep)) identify(uwep);
				else if (uswapwep && not_fully_identified(uswapwep)) identify(uswapwep);
				else if (uamul && not_fully_identified(uamul)) identify(uamul);
				else if (uleft && not_fully_identified(uleft)) identify(uleft);
				else if (uright && not_fully_identified(uright)) identify(uright);
				else if (uarmc && not_fully_identified(uarmc)) identify(uarmc);
				else if (uarm && not_fully_identified(uarm)) identify(uarm);
				else if (uarmu && not_fully_identified(uarmu)) identify(uarmu);
				else if (uarmh && not_fully_identified(uarmh)) identify(uarmh);
				else if (uarmg && not_fully_identified(uarmg)) identify(uarmf);
				else if (uarmf && not_fully_identified(uarmf)) identify(uarmf);
				else if (uarms && not_fully_identified(uarms)) identify(uarms);
				else {
					for(otmp=invent; otmp; otmp=otmp->nobj)
					if (not_fully_identified(otmp)){
						identify(otmp);
						break;
					}
				}
				break;
			case 3: // give an intrinsic for 500-1500 turns, first of pois/slee/fire/cold/shock
				timeout = rn1(1000, 500);

				if(!(HPoison_resistance & INTRINSIC)) {
					You_feel(Poison_resistance ? "especially healthy." : "healthy.");
					HPoison_resistance |= FROMOUTSIDE;
					break;
				}
				
				if(!(HSleep_resistance)) {
					You_feel("wide awake.");				
					give_intrinsic(SLEEP_RES, timeout);
				}
				
				if(!(HFire_resistance)) {
					You(Hallucination ? "be chillin'." : "feel a momentary chill.");				
					give_intrinsic(FIRE_RES, timeout);
				}

				if(!(HCold_resistance)) {
					You_feel("full of hot air.");
					give_intrinsic(COLD_RES, timeout);
				}
						
				if(!(HShock_resistance)) {
					if (Hallucination)
						rn2(2) ? You_feel("grounded in reality.") : Your("health currently feels amplified!");
					else You_feel("well grounded.");
					
					give_intrinsic(SHOCK_RES, timeout);
				}
				break;
			case 4: // repair an item, or make one rustproof
				otmp = (struct obj *)0;
				/* select most damaged item */
				for (i = 3; i >= 0 && !otmp; i--){
#define select_item_to_repair_or_erodeproof(item) \
(item && ((i > 0) ? (item->oeroded == i || item->oeroded == i) : (!item->oerodeproof))) otmp = item

					if      select_item_to_repair_or_erodeproof(uwep);
					else if select_item_to_repair_or_erodeproof(uswapwep);
					else if select_item_to_repair_or_erodeproof(uarmc);
					else if select_item_to_repair_or_erodeproof(uarm);
					else if select_item_to_repair_or_erodeproof(uarmu);
					else if select_item_to_repair_or_erodeproof(uarmh);
					else if select_item_to_repair_or_erodeproof(uarmg);
					else if select_item_to_repair_or_erodeproof(uarmf);
					else if select_item_to_repair_or_erodeproof(uarms);
#undef select_item_to_repair_or_erodeproof
				}
				/* repair by 1 level, or make erodeproof */
				if (otmp) {
					otmp->rknown = TRUE;
					if (otmp->oeroded > 0){
						otmp->oeroded--;
					} else if (otmp->oeroded2 > 0){
						otmp->oeroded2--;
					} else if (!otmp->oeroded && !otmp->oeroded2 && !otmp->oerodeproof) {
						otmp->oerodeproof = TRUE;
					}
					/* message */
					if (!Blind) {
						Your("%s %s %s!",
							xname(otmp),
							vtense(xname(otmp), "look"),
							(otmp->oeroded || otmp->oeroded2) ? "better" :
								(!otmp->oerodeproof) ? "as good as new" :
								"better than ever"
							);
					}
				}
				break;
			case 5: // bless/curse an item
#define wrongbuc(obj) ((hates_unholy(youracedata) && hates_holy(youracedata)) ? \
						(obj->blessed || obj->cursed) : \
						(hates_unblessed(youracedata) ? (obj->blessed || obj->cursed) : \
						(hates_unholy(youracedata) ? !obj->blessed : \
						(hates_holy(youracedata) ? !obj->cursed : !obj->blessed))))

				/* weapon takes precedence if it interferes with taking off a ring or shield */				
				if (uwep && wrongbuc(uwep)) otmp = uwep;
				else if (uswapwep && wrongbuc(uswapwep)) otmp = uswapwep;
				/* gloves come next, due to rings */
				else if (uarmg && wrongbuc(uarmg)) otmp = uarmg;
				/* then shield due to two handed weapons and spells */
				else if (uarms && wrongbuc(uarms)) otmp = uarms;
				/* then cloak due to body armor */
				else if (uarmc && wrongbuc(uarmc)) otmp = uarmc;
				else if (uarm && wrongbuc(uarm)) otmp = uarm;
				else if (uarmh && wrongbuc(uarmh)) otmp = uarmh;
				else if (uarmf && wrongbuc(uarmf)) otmp = uarmf;
				else if (uarmu && wrongbuc(uarmu)) otmp = uarmu;
				else if (uamul && wrongbuc(uamul)) otmp = uamul;
				else if (uleft && wrongbuc(uleft))  otmp = uleft;
				else if (uright && wrongbuc(uright)) otmp = uright;
				else {
					for(otmp=invent; otmp; otmp=otmp->nobj)
					if (wrongbuc(otmp)) break;
				}
				if (!otmp)
					break;	/* no item found */
				if (hates_unholy(youracedata) && hates_holy(youracedata)){
					uncurse(otmp);
					unbless(otmp);
				}
				else if (hates_holy(youracedata))
					curse(otmp);
				else if (hates_unholy(youracedata))
					bless(otmp);
				else
					bless(otmp);
			
				otmp->bknown = TRUE;
				if (!Blind)
					Your("%s %s.", what ? what : (const char *) aobjnam(otmp, "softly glow"), 
								hcolor(otmp->blessed ? NH_LIGHT_BLUE : \
								(otmp->cursed ? NH_BLACK : NH_AMBER)));
#undef wrongbuc
				break;
			default: impossible("bad god_gives_benefit benefit?");
		}
	}
}

STATIC_OVL int
goat_resurrect(otmp, eatflag)
struct obj *otmp;
int eatflag;
{
	struct monst *revived = 0;
	if(goat_monster(&mons[otmp->corpsenm])){
		pline("%s twitches.", The(xname(otmp)));
		revived = revive(otmp, FALSE);
		if(eatflag == GOAT_EAT_OFFERED){
			//She grows angry at you, and may smite you.
			gods_upset(GOD_THE_BLACK_MOTHER);
		}
		else if(eatflag == GOAT_EAT_MARKED){
			//She grows angry at you, but doesn't actually smite you.
			godlist[GOD_THE_BLACK_MOTHER].anger++;
			gods_angry(GOD_THE_BLACK_MOTHER);
		}
	}

	if(revived)
		return TRUE;
	return FALSE;
}

STATIC_OVL int
goat_rider(otmp, eatflag)
struct obj *otmp;
int eatflag;
{
	int cn = otmp->corpsenm;
	struct monst *revived = 0;
	if(is_rider(&mons[otmp->corpsenm])){
		pline("A pulse of darkness radiates from %s!", the(xname(otmp)));
		revived = revive(otmp, FALSE);
		if(eatflag == GOAT_EAT_OFFERED){
			//She grows angry at you, and may smite you.
			gods_upset(GOD_THE_BLACK_MOTHER);
		}
		else if(eatflag == GOAT_EAT_MARKED){
			//She grows angry at you, but doesn't actually smite you.
			godlist[GOD_THE_BLACK_MOTHER].anger++;
			gods_angry(GOD_THE_BLACK_MOTHER);
		}
	}
	if(revived)
		return TRUE;
	return FALSE;
}

STATIC_OVL int
fire_rider(otmp)
struct obj *otmp;
{
	int cn = otmp->corpsenm;
	struct monst *revived = 0;
	if(is_rider(&mons[otmp->corpsenm])){
		pline("A pulse of darkness radiates from %s!", the(xname(otmp)));
		revived = revive(otmp, FALSE);
		//Grows angry at you, but doesn't actually smite you.
		godlist[GOD_THE_SILVER_FLAME].anger++;
		gods_angry(GOD_THE_SILVER_FLAME);
	}
	if(revived)
		return TRUE;
	return FALSE;
}

/* must be non-zero */
#define GOATBOON_GOAT_MILK	1
#define GOATBOON_RENEWAL	2
#define GOATBOON_REMOVE_CURSE	3
#define GOATBOON_LUCK	4
#define GOATBOON_DARK_YOUNG	5
#define GOATBOON_ACID	6
#define GOATBOON_DROOL	7
#define GOATBOON_RED_WORD	8
int
dogoat_menu(greater_boon)
boolean greater_boon;	/* you have shown devotion enough to ask for a greater boon */
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	menu_item *selected;
	char inclet = 'a';
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Ask for what?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	*buf = '\0';
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	Sprintf(buf, "her Healing");
	any.a_int = GOATBOON_GOAT_MILK;
	add_menu(tmpwin, NO_GLYPH, &any,
		inclet++, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	Sprintf(buf, "her Vitality");
	any.a_int = GOATBOON_RENEWAL;
	add_menu(tmpwin, NO_GLYPH, &any,
		inclet++, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	Sprintf(buf, "her Blessing");
	any.a_int = GOATBOON_REMOVE_CURSE;
	add_menu(tmpwin, NO_GLYPH, &any,
		inclet++, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	Sprintf(buf, "her Fortune");
	any.a_int = GOATBOON_LUCK;
	add_menu(tmpwin, NO_GLYPH, &any,
		inclet++, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	Sprintf(buf, "her Intervention");
	any.a_int = GOATBOON_DARK_YOUNG;
	add_menu(tmpwin, NO_GLYPH, &any,
		inclet++, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	/* the following boons are permanent boosters, and so require devotion */
	if (greater_boon) {

		Sprintf(buf, "her Bite");
		any.a_int = GOATBOON_ACID;
		add_menu(tmpwin, NO_GLYPH, &any,
			inclet++, 0, ATR_NONE, buf,
			MENU_UNSELECTED);

		Sprintf(buf, "her Hunger");
		any.a_int = GOATBOON_DROOL;
		add_menu(tmpwin, NO_GLYPH, &any,
			inclet++, 0, ATR_NONE, buf,
			MENU_UNSELECTED);

		if (!flags.made_know) {
			Sprintf(buf, "her Knowledge");
			any.a_int = GOATBOON_RED_WORD;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet++, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}

	}
	
	end_menu(tmpwin, "You have the Goat's attention...");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if(n > 0){
		int picked = selected[0].item.a_int;
		free(selected);
		return picked;
	}
	return 0;
}

int
commune_with_goat()
{
	const char * goatname = goattitles[rn2(SIZE(goattitles))];
	const char blessable_classes[] = { WEAPON_CLASS, TOOL_CLASS, ARMOR_CLASS, 0 };

	/* you must be an active cultist */
	if (!u.shubbie_atten) {
		pline1(nothing_happens);
		return MOVE_STANDARD;
	}
	/* not in the Future */
	if (Infuture) {
		You("only draw It's attention!");
		aggravate();
		return MOVE_STANDARD;
	}
	/* in good standing */
	if (godlist[GOD_THE_BLACK_MOTHER].anger) {
		You_feel("a fraction of %s %s.",
			s_suffix(goatname),
			godlist[GOD_THE_BLACK_MOTHER].anger > 6 ? "wrath" :
			godlist[GOD_THE_BLACK_MOTHER].anger > 3 ? "anger" :
			                                          "displeasure");
		return MOVE_STANDARD;
	}
	/* must have accumulated enough cult credit */
	if (u.shubbie_credit < 50) {
		/* not enough credit, give a message based on how much you have */
		You_feel("a %s of %s attention.",
			(u.shubbie_credit < 25 ? "flicker" : "tendril"),
			s_suffix(goatname)
			);
		/* taxes sanity! (a tiny bit) */
		change_usanity(-3, TRUE);
		return MOVE_STANDARD;
	}

	/* if you are devout enough (total accumulated credit), allow selecting permanent boons and boost the power of regular ones */
	/* devotion required increases as per standard gift giving formula */
	if(wizard)
		pline("boon threshold: %d", 25*(10 + (u.uartisval * u.uartisval * 2 / 25)));
	boolean greater_boon = u.shubbie_devotion > 25*(10 + (u.uartisval * u.uartisval * 2 / 25));
	struct obj * otmp = (struct obj *)0;
	int menu_result = dogoat_menu(greater_boon);
	int i;
	int cost = 0;	/* credit and sanity/2 cost for the chosen boon */

	switch (menu_result) {
		case GOATBOON_GOAT_MILK:
			cost = 25;
			/* as the potion of goat's milk */
			otmp = mksobj(POT_GOAT_S_MILK, MKOBJ_NOINIT);
			otmp->blessed = otmp->cursed = 0;
			otmp->quan = 20L;			/* do not let useup get it */
			otmp->blessed = greater_boon;
			(void) peffects(otmp, TRUE);
			obfree(otmp, (struct obj *)0);	/* now, get rid of it */
			break;

		case GOATBOON_RENEWAL:
			cost = 15;
			/* cures Sterility, otherwise as the potion of restore ability (minus sanity recovery) */
			if(HSterile) {
				You_feel("fertile.");
				HSterile = 0L;
			}
			else {
				otmp = mksobj(POT_RESTORE_ABILITY, MKOBJ_NOINIT);
				otmp->blessed = otmp->cursed = 0;
				otmp->quan = 20L;			/* do not let useup get it */
				otmp->blessed = greater_boon;
				/* potions of restore ability recover sanity, so pre-decrement sanity */
				change_usanity(otmp->blessed ? -20 : -5, FALSE);
				(void) peffects(otmp, TRUE);
				obfree(otmp, (struct obj *)0);	/* now, get rid of it */
			}
			break;

		case GOATBOON_REMOVE_CURSE:
			cost = 10;
			/* as the spell of remove curse */
			otmp = mksobj(SPE_REMOVE_CURSE, MKOBJ_NOINIT);
			otmp->blessed = otmp->cursed = 0;
			otmp->quan = 20L;			/* do not let useup get it */
			otmp->blessed = greater_boon;
			(void) seffects(otmp);
			obfree(otmp, (struct obj *)0);	/* now, get rid of it */
			break;

		case GOATBOON_LUCK:
			cost = 20;
			/* removes all negative luck, and adds a few extra */
			You_feel("%slucky.",
				greater_boon ? "very " : "");
			if (u.uluck < 0) u.uluck = 0;
			change_luck(greater_boon ? 7 : 3);
			break;

		case GOATBOON_DARK_YOUNG:
			cost = 25;
			/* Temporarily summons 1-3 tame dark young. Since they are temporary, they don't count against your pet cap */
			for (i = 0; i < (greater_boon ? 3 : rnd(3)); i++) {
				struct monst * mtmp = make_pet_minion(PM_DARK_YOUNG, GOD_THE_BLACK_MOTHER, TRUE);
				mark_mon_as_summoned(mtmp, (struct monst *)0, rnz(u.shubbie_credit), 0);
				seemimic(mtmp);
			}
			break;

		case GOATBOON_ACID:
			cost = 25;
			/* gives your wielded (nonartifact) weapon the Acrid (+2d6 acid damage) property */
			otmp = getobj(blessable_classes, "give the Goat's bite");
			if(otmp && goat_acidable(otmp)) {
				if(!Blind) pline("Acid drips from your weapon!");
				add_oprop(otmp, OPROP_LESSER_ACIDW);
				otmp->oeroded = 0;
				otmp->oeroded2 = 0;
				otmp->oerodeproof = 1;
				u.ugifts++;
				u.uartisval += TIER_B;
			}
			else {
				cost = 0;
			}
			break;

		case GOATBOON_DROOL:
			cost = 40;

			/* gives your wielded weapon the Drooling property, which does various +dmg effects and marks mons for eating */
			otmp = getobj(blessable_classes, "give the Goat's hunger");
			if(otmp && goat_droolable(otmp)){
				if(!Blind) pline("...your %s %s drooling.", xname(otmp), vtense(xname(otmp), "are"));
				remove_oprop(otmp, OPROP_LESSER_ACIDW);
				add_oprop(otmp, OPROP_GOATW);
				otmp->oeroded = 0;
				otmp->oeroded2 = 0;
				otmp->oerodeproof = 1;
				u.ugifts++;
				u.uartisval += TIER_S;
			}
			else {
				cost = 0;
			}
			break;

		case GOATBOON_RED_WORD:
			cost = 40;
			/* gives the Word of Knowledge */
			otmp = mksobj(WORD_OF_KNOWLEDGE, MKOBJ_NOINIT);
			dropy(otmp);
			at_your_feet("An object");
			u.ugifts++;
			u.uartisval += TIER_A;
			break;

		default:
			pline("The connection fades.");
			break;
	}

	u.shubbie_credit -= cost;
	change_usanity(-cost/2, TRUE);
	
	return MOVE_STANDARD;
}

/* must be non-zero */
#define FLAMEBOON_SMELT_IMPURITY	1
#define FLAMEBOON_SMELT_IMPERFECTION	2
#define FLAMEBOON_SMELT_IMMOBILITY	3
#define FLAMEBOON_SPREAD_FIRE	4
#define FLAMEBOON_REFLECTION	5
#define FLAMEBOON_CURSE_GLAZE	6
// #define FLAMEBOON_SHAPE_CHANGERS	7
#define FLAMEBOON_BURN_MORTALITY	8
#define FLAMEBOON_BURN_UNDEATH	9
#define FLAMEBOON_BURN_SPIRIT	10
#define FLAMEBOON_BURN_BEATITUDE	11
int
dosflm_menu(greater_boon)
boolean greater_boon;	/* you have shown devotion enough to ask for a greater boon */
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	menu_item *selected;
	char inclet = 'a';
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "What will you do?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	*buf = '\0';
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	Sprintf(buf, "Smelt away impurity");
	any.a_int = FLAMEBOON_SMELT_IMPURITY;
	add_menu(tmpwin, NO_GLYPH, &any,
		inclet++, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	Sprintf(buf, "Smelt away imperfection");
	any.a_int = FLAMEBOON_SMELT_IMPERFECTION;
	add_menu(tmpwin, NO_GLYPH, &any,
		inclet++, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	Sprintf(buf, "Smelt away immobility");
	any.a_int = FLAMEBOON_SMELT_IMMOBILITY;
	add_menu(tmpwin, NO_GLYPH, &any,
		inclet++, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	Sprintf(buf, "Spread the reflected silver flame");
	any.a_int = FLAMEBOON_SPREAD_FIRE;
	add_menu(tmpwin, NO_GLYPH, &any,
		inclet++, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	Sprintf(buf, "Burn aura");
	any.a_int = FLAMEBOON_BURN_BEATITUDE;
	add_menu(tmpwin, NO_GLYPH, &any,
		inclet++, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	/* the following boons are permanent boosters, and so require devotion */
	if (greater_boon) {
		Sprintf(buf, "Mirror-finish a surface");
		any.a_int = FLAMEBOON_REFLECTION;
		add_menu(tmpwin, NO_GLYPH, &any,
			inclet++, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		Sprintf(buf, "Burn on a curse-proof glaze");
		any.a_int = FLAMEBOON_CURSE_GLAZE;
		add_menu(tmpwin, NO_GLYPH, &any,
			inclet++, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		Sprintf(buf, "Focus the light to reveal mortality");
		any.a_int = FLAMEBOON_BURN_MORTALITY;
		add_menu(tmpwin, NO_GLYPH, &any,
			inclet++, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		Sprintf(buf, "Focus the light to reveal true death");
		any.a_int = FLAMEBOON_BURN_UNDEATH;
		add_menu(tmpwin, NO_GLYPH, &any,
			inclet++, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		Sprintf(buf, "Focus the light to reveal the unworthy");
		any.a_int = FLAMEBOON_BURN_SPIRIT;
		add_menu(tmpwin, NO_GLYPH, &any,
			inclet++, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	
	end_menu(tmpwin, "You stare into the silver flame...");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if(n > 0){
		int picked = selected[0].item.a_int;
		free(selected);
		return picked;
	}
	return 0;
}

int
commune_with_silver_flame()
{
	/* you must be an active cultist (redirects to mirror if you haven't, so this shouldn't actually happen) */
	if (!u.silver_atten) {
		pline1(nothing_happens);
		return MOVE_STANDARD;
	}
	/* not in the Future */
	if (Infuture) {
		You("see a distant silver light.");
		return MOVE_STANDARD;
	}
	/* in good standing */
	if (godlist[GOD_THE_SILVER_FLAME].anger) {
		//Import bad prayer effects (where she drools on you if you're prayer timeout is bad)
		You("are burned by the silver light!");
		losehp(rnd(20), "angry silver light", KILLED_BY);
		return MOVE_STANDARD;
	}
	/* must have accumulated enough cult credit */
	if (u.silver_credit < 50) {
		/* not enough credit, give a message based on how much you have */
		pline("The silver flame is distant and uninterested.");
		return MOVE_STANDARD;
	}

	/* if you are devout enough (total accumulated credit), allow selecting permanent boons and boost the power of regular ones */
	/* devotion required increases as per standard gift giving formula */
	if(wizard)
		pline("boon threshold: %d", 25*(10 + (u.uartisval * u.uartisval * 2 / 25)));
	boolean greater_boon = u.silver_devotion > 25*(10 + (u.uartisval * u.uartisval * 2 / 25));
	struct obj * otmp = (struct obj *)0;
	int menu_result = dosflm_menu(greater_boon);
	int i;
	int cost = 0;	/* credit and sanity/2 cost for the chosen boon */
	const char sflm_classes[] = { WEAPON_CLASS, TOOL_CLASS, ARMOR_CLASS, 0 };
	const char all_classes[] = { ALL_CLASSES, 0 };

	switch (menu_result) {
		case FLAMEBOON_SMELT_IMPURITY:
			otmp = getobj(all_classes, "smelt silver in the silver light");
			if(otmp && sflm_smeltable_silver(otmp)){
				cost = 50;
				You("smelt %s to silver with the reflected light of the silver flame.", doname(otmp));
				set_material(otmp, SILVER);
			}
			else pline("Nothing happens.");
			break;

		case FLAMEBOON_SMELT_IMPERFECTION:
			otmp = getobj(all_classes, "smelt platinum in the silver light");
			if(otmp && sflm_smeltable_platinum(otmp)){
				cost = 50;
				You("smelt %s to platinum with the reflected light of the silver flame.", doname(otmp));
				set_material(otmp, PLATINUM);
			}
			else pline("Nothing happens.");
			break;

		case FLAMEBOON_SMELT_IMMOBILITY:
			otmp = getobj(all_classes, "smelt mithril in the silver light");
			if(otmp && sflm_smeltable_mithril(otmp)){
				cost = 50;
				You("smelt %s to mithril with the reflected light of the silver flame.", doname(otmp));
				set_material(otmp, MITHRIL);
			}
			else pline("Nothing happens.");
			break;

		case FLAMEBOON_SPREAD_FIRE:
			otmp = getobj(sflm_classes, "offer to the flame");
			if(otmp){
				if(sflm_offerable(otmp)){
					cost = 25;
					pline("The silver light reflects from your mirror and takes up residence within %s.", doname(otmp));
					add_oprop(otmp, OPROP_SFLMW);
				}
				else pline("Nothing happens.");
			}
			break;

		case FLAMEBOON_BURN_BEATITUDE:
			otmp = getobj(all_classes, "burn in the silver flame");
			if(otmp){
				if(otmp->blessed || otmp->cursed){
					cost = 10;
					You("burn %s with the reflected light of the silver flame.", doname(otmp));
					unbless(otmp);
					uncurse(otmp);
				}
				else pline("Nothing happens.");
			}
			break;

		case FLAMEBOON_REFLECTION:
			otmp = getobj(sflm_classes, "mirror-finish");
			if(otmp){
				if(sflm_mirrorable(otmp)){
					cost = 50;
					You("melt %s to a mirror finish.", doname(otmp));
					add_oprop(otmp, OPROP_REFL);
					u.uartisval += TIER_B; /*Theory: You now want reflection in the asc kit to deal with holy missiles, so the ability to add it to anything is pretty great. */
				}
				else pline("Nothing happens.");
			}
			break;

		case FLAMEBOON_CURSE_GLAZE:
			otmp = getobj(sflm_classes, "curse-proof glaze");
			if(otmp){
				if(sflm_glazeable(otmp)){
					cost = 25;
					You("add a curse-proof glaze to %s.", doname(otmp));
					add_oprop(otmp, OPROP_CGLZ);
					/*Theory: You need 4-5 of these, which adds to TIER_A. */
					if(arm_blocks_upper_body(otmp->otyp))
						u.uartisval += TIER_D;
					else u.uartisval += TIER_F;
				}
				else pline("Nothing happens.");
			}
			break;

		// case FLAMEBOON_SHAPE_CHANGERS:
			// cost = 25;
			// break;

		case FLAMEBOON_BURN_MORTALITY:
			otmp = getobj(sflm_classes, "reveal mortality");
			if(otmp){
				if(sflm_mortalable(otmp)){
					cost = 50;
					pline("The silver light is focused by your mirror!");
					add_oprop(otmp, OPROP_MORTW);
					u.uartisval += TIER_B; /*Theory: Life drain is actually not all that powerful, but the Wizard and his summons are still affected. */
				}
				else pline("Nothing happens.");
			}
			break;

		case FLAMEBOON_BURN_UNDEATH:
			otmp = getobj(sflm_classes, "reveal true death");
			if(otmp){
				if(sflm_truedeathable(otmp)){
					cost = 50;
					pline("The silver light is focused by your mirror!");
					add_oprop(otmp, OPROP_TDTHW);
					u.uartisval += TIER_A; /*Theory: Nasty stuff like liches and pharaohs is affected, plus it deals a lot of damage to them. */
				}
				else pline("Nothing happens.");
			}
			break;

		case FLAMEBOON_BURN_SPIRIT:
			otmp = getobj(sflm_classes, "reveal the unworthy");
			if(otmp){
				if(sflm_unworthyable(otmp)){
					cost = 50;
					pline("The silver light is focused by your mirror!");
					add_oprop(otmp, OPROP_SFUWW);
					u.uartisval += TIER_S; /*Theory: This specifically affects the nastiest late game enemies. */
				}
				else pline("Nothing happens.");
			}
			break;
	}
	
	if(!cost)
		pline("The silver light recedes.");

	u.silver_credit -= cost;
	change_usanity(-cost/2, TRUE);
	
	return MOVE_STANDARD;
}

boolean
goat_mouth_at(x, y)
int x, y;
{
	struct monst *mtmp;
	for(mtmp = migrating_mons; mtmp; mtmp = mtmp->nmon){
		if(mtmp->mux == u.uz.dnum && mtmp->muy == u.uz.dlevel && mtmp->mtyp == PM_MOUTH_OF_THE_GOAT && !DEADMONSTER(mtmp)){
			xchar xlocale, ylocale, xyloc;
			xyloc	= mtmp->mtrack[0].x;
			xlocale = mtmp->mtrack[1].x;
			ylocale = mtmp->mtrack[1].y;
			if(xyloc == MIGR_EXACT_XY && xlocale == x && ylocale == y)
				return TRUE;
		}
	}
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon){
		if(mtmp->mtyp == PM_MOUTH_OF_THE_GOAT && distu(mtmp->mx,mtmp->my) <= 2 && !DEADMONSTER(mtmp)){
			return TRUE;
		}
	}
	return FALSE;
}

boolean
bokrug_idol_at(x, y)
int x, y;
{
	struct obj *otmp;
	for (otmp = level.objects[x][y]; otmp; otmp = otmp->nexthere) {
		if (otmp->oartifact == ART_IDOL_OF_BOKRUG__THE_WATER_)
			return TRUE;
	}
	return FALSE;
}

void
bokrug_offer(otmp)
struct obj *otmp;
{
	if(otmp->otyp == AMULET_OF_YENDOR){
		/* The final Test.	Did you win? */
		if(uamul == otmp) Amulet_off();
		u.uevent.ascended = 1;
		if(carried(otmp)) useup(otmp); /* well, it's gone now */
		else useupf(otmp, 1L);
		You("offer the Amulet of Yendor to Bokrug...");
		adjalign(10);
#ifdef RECORD_ACHIEVE
		achieve.ascended = 1;
		give_ascension_trophy();
#endif
		You("sink into cool lake water...");
		You("don't drown. You have achieved the status of Demigod%s!",
			flags.female ? "dess" : "");
		give_bokrug_trophy();
		done(ASCENDED);
	}
	//else
	pline1(nothing_happens);
}

void
goat_eat(otmp, eatflag)
struct obj *otmp;
int eatflag;
{
    int value = 0;
	struct permonst *ptr = &mons[otmp->corpsenm];
	struct monst *mtmp;
	extern const int monstr[];
	xchar x, y;
	
	get_obj_location(otmp, &x, &y, BURIED_TOO);
	
	if(goat_resurrect(otmp, eatflag)){
		//otmp is now gone, and resurrect may have printed messages
		return;
	}
	
	if(goat_rider(otmp, eatflag)){
		//otmp is now gone, and rider may have printed messages
		return;
	}

	if(otmp->otyp == AMULET_OF_YENDOR){
		pline("The Amulet proves inedible.");
		return;
	}

	if ((otmp->corpsenm == PM_ACID_BLOB
		|| (monstermoves <= peek_at_iced_corpse_age(otmp) + 50)
		) && mons[otmp->corpsenm].mlet != S_PLANT
	) {
		value = monstr[otmp->corpsenm] + 1;
		if (otmp->oeaten)
		value = eaten_stat(value, otmp);
	} else {
		//minimum, but still eat.
		value = 1;
	}

	if (eatflag != GOAT_EAT_PASSIVE && your_race(ptr) && !is_animal(ptr) && !mindless(ptr) && u.ualign.type != A_VOID && !Role_if(PM_ANACHRONONAUT)) {
	//No demon summoning.  Your god just smites you, and sac continues.
		if (u.ualign.type != A_CHAOTIC && u.ualign.type != A_NONE) {
			adjalign(-5);
			godlist[u.ualign.god].anger += 3;
			(void) adjattrib(A_WIS, -1, TRUE);
			if (!Inhell) angrygods(u.ualign.god);
			change_luck(-5);
		} else adjalign(5);
	//Pets are just eaten like anything else.  Your god doesn't know you did it, and the goat doesn't care.
	} else if (is_undead(ptr)) { /* Not demons--no demon corpses */
		if (u.ualign.type != A_CHAOTIC && u.ualign.type != A_NONE)
			value += 1;
	//Unicorns are resurrected.
	}
    /* corpse */
	//Value can't be 0
	//Value can't be -1
	char goatname[BUFSZ];
	Strcpy(goatname, goattitles[rn2(SIZE(goattitles))]);
	/* Sacrificing at an altar of a different alignment */
	/* Never a conversion */
	/* never an altar conversion*/
	
	/* Rider handled */
	eat_offering(otmp, eatflag == GOAT_EAT_MARKED && !(u.ualign.type != A_CHAOTIC && u.ualign.type != A_NONE && u.ualign.type != A_VOID && !Role_if(PM_ANACHRONONAUT)) && goat_seenonce, eatflag);
	if(eatflag == GOAT_EAT_MARKED)
		goat_seenonce = TRUE;
	if(eatflag != GOAT_EAT_PASSIVE && u.ualign.type != A_CHAOTIC && u.ualign.type != A_NONE && u.ualign.type != A_VOID && !Role_if(PM_ANACHRONONAUT)) {
		adjalign(-value);
		godlist[u.ualign.god].anger += 1;
		(void) adjattrib(A_WIS, -1, TRUE);
		if (!Inhell) angrygods(u.ualign.god);
		change_luck(-1);
	}

	/* off floor -- the only possible effect is creating Goat's Milk. You do not get credit, return early */
	if (eatflag == GOAT_EAT_PASSIVE) {
		if(isok(x,y) && rnl((30 + u.ulevel)*10) < 10){
			mksobj_at(POT_GOAT_S_MILK, x, y, MKOBJ_NOINIT);
		}
		return;
	}

	/* anger must be paid off before credit can be built, return early */
	if(godlist[GOD_THE_BLACK_MOTHER].anger) {
		/* goatmom can be quickly appeased by direct offerings; marked creatures count less */
		if (eatflag == GOAT_EAT_OFFERED || (rn2(MAXVALUE) < value)) {
			godlist[GOD_THE_BLACK_MOTHER].anger--;
			godvoice(GOD_THE_BLACK_MOTHER, "");
			if (godlist[GOD_THE_BLACK_MOTHER].anger == 0)
				You_feel("like %s is appeased.", goatname);
		}
		return;
	}

	/* direct offerings make all goat-aligned creatures on the level peaceful. This intentionally happens before the holy-symbol check. */
	if(eatflag == GOAT_EAT_OFFERED) {
		struct monst *mtmp;
		for(mtmp = migrating_mons; mtmp; mtmp = mtmp->nmon){
			if(mtmp->mux == u.uz.dnum && mtmp->muy == u.uz.dlevel && (mtmp->mtyp == PM_BLESSED || mtmp->mtyp == PM_MOUTH_OF_THE_GOAT || has_template(mtmp, MISTWEAVER))){
				mtmp->mpeaceful = 1;
				set_malign(mtmp);
			}
		}
		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon){
			if(mtmp->mfaction == GOATMOM_FACTION){
				mtmp->mpeaceful = 1;
				set_malign(mtmp);
				newsym(mtmp->mx, mtmp->my);
			}
		}
	}
	/* the player must carry a holy symbol to gain credit. Chance to give one, if missing. return early */
	if(!has_object_type(invent, HOLY_SYMBOL_OF_THE_BLACK_MOTHE)){
		struct obj *otmp;
		if(u.shubbie_atten ? !rn2(10+u.ugifts) : !rn2(4)){
			otmp = mksobj(HOLY_SYMBOL_OF_THE_BLACK_MOTHE, MKOBJ_NOINIT);
			dropy(otmp);
			at_your_feet("An object");
			//event: only increment this once.
			if(!u.shubbie_atten){
				u.ugifts++;
				u.shubbie_atten = 1;
			}
		}
		return;
	}

	/* at this point, gain credit */

	/* direct offerings are worth more than kills with drooling weapons */
	value = value * (eatflag == GOAT_EAT_MARKED ? 2 : 6) / 3;

	/* credit gain suffers diminishing returns, less harshly if you are goat-ridden */
	int dim_return_factor = (u.umadness & MAD_GOAT_RIDDEN) ? 100 : 50;
	int full_value = value;
	value = max(1, value * dim_return_factor / (dim_return_factor + u.shubbie_credit));
	u.shubbie_credit += value;
	u.shubbie_devotion += eatflag == GOAT_EAT_OFFERED ? full_value : value;
	if (wizard) {
		/* debug */
		pline("ShubbieCredit = %ld [+%d base %d], ShubbieDevotion = %ld",
			u.shubbie_credit,
			value,
			full_value,
			u.shubbie_devotion
			);
	}
	return;
}
void
flame_consume(mtmp, otmp)
struct monst *mtmp;
struct obj *otmp;
{
    int value = 0;
	struct permonst *ptr = mtmp ? mtmp->data : otmp ? &mons[otmp->corpsenm] : 0;
	extern const int monstr[];
	
	if(!ptr){
		impossible("flame eat with no subject");
		return;
	}
	
	if(otmp && fire_rider(otmp)){
		//otmp is now gone, and rider may have printed messages
		return;
	}

	if(otmp && otmp->otyp == AMULET_OF_YENDOR){
		pline("The Amulet proves fireproof.");
		return;
	}

	value = monstr[ptr->mtyp] + 1;
	if (otmp && otmp->oeaten)
		value = eaten_stat(value, otmp);

	if (your_race(ptr) && !is_animal(ptr) && !mindless(ptr) && u.ualign.type != A_VOID && !Role_if(PM_ANACHRONONAUT)) {
	//No demon summoning.  Your god just smites you, and sac continues.
		if (u.ualign.type != A_LAWFUL && u.ualign.type != A_NONE) {
			adjalign(-5);
			godlist[u.ualign.god].anger += 3;
			(void) adjattrib(A_WIS, -1, TRUE);
			if (!Inhell) angrygods(u.ualign.god);
			change_luck(-5);
		} else adjalign(5);
	//Pets are burned like anything else.
	} else if (is_undead(ptr)) { /* Not demons--no demon corpses */
		if (u.ualign.type != A_CHAOTIC && u.ualign.type != A_NONE)
			value += 1;
	//Unicorns are burned like anything else.
	}
    /* corpse */
	//Value can't be 0
	//Value can't be -1
	/* Sacrificing at an altar of a different alignment */
	/* Never a conversion */
	/* never an altar conversion*/
	
	/* Rider handled */
	if(otmp)
		burn_offering(otmp, !(u.ualign.type != A_LAWFUL && u.ualign.type != A_NONE && u.ualign.type != A_VOID && !Role_if(PM_ANACHRONONAUT)));
	if(u.ualign.type != A_LAWFUL && u.ualign.type != A_NONE && u.ualign.type != A_VOID && !Role_if(PM_ANACHRONONAUT)) {
		adjalign(-value);
		godlist[u.ualign.god].anger += 1;
		(void) adjattrib(A_CHA, -1, TRUE);
		if (!Inhell) angrygods(u.ualign.god);
		change_luck(-1);
	}

	/* anger must be paid off before credit can be built, return early */
	if(godlist[GOD_THE_SILVER_FLAME].anger) {
		if (rn2(MAXVALUE) < value) {
			godlist[GOD_THE_SILVER_FLAME].anger--;
			if (godlist[GOD_THE_SILVER_FLAME].anger == 0)
				You_feel("like the silver flame is appeased.");
		}
		return;
	}

	/* at this point, gain credit */

	/* credit gain suffers diminishing returns, less harshly if you have a lot of insight */
	int dim_return_factor = max(1, u.uinsight);
	if (wizard) {
		/* debug */
		pline("FlameCredit = %ld [+%ld base %d], FlameDevotion = %ld",
			u.silver_credit + max(1, value * dim_return_factor / (dim_return_factor + u.silver_credit)),
			max(1, value * dim_return_factor / (dim_return_factor + u.silver_credit)),
			value,
			u.silver_devotion + max(1, value * dim_return_factor / (dim_return_factor + u.silver_credit))
			);
	}
	value = max(1, value * dim_return_factor / (dim_return_factor + u.silver_credit));
	u.silver_credit += value;
	u.silver_devotion += value;
	return;
}

/* declare the global godlist pointer */
struct god * godlist;

/* save and restore god list */
void
init_gods()
{
	extern const struct god base_godlist[];

	godlist = malloc(sizeof(struct god) * (MAX_GOD+1));
	memcpy(godlist, base_godlist, sizeof(struct god) * (MAX_GOD+1));
}

void
save_gods(fd)
int fd;
{
	bwrite(fd, (genericptr_t) godlist, sizeof(struct god) * (MAX_GOD+1));
}

void
restore_gods(fd)
int fd;
{
	extern const struct god base_godlist[];
	int i;

	godlist = malloc(sizeof(struct god) * (MAX_GOD+1));
	mread(fd, (genericptr_t) godlist, sizeof(struct god) * (MAX_GOD+1));
	/* fix name pointers -- assumes that god names do NOT get changed during the game */
	for (i=1; i<MAX_GOD; i++)
		godlist[i].name = base_godlist[i].name;
}

aligntyp
galign(godnum)
int godnum;
{
	return godlist[godnum].alignment;
}

int
gholiness(godnum)
int godnum;
{
	return godlist[godnum].holiness;
}

/* transitory function, hopefully, to convert an alignment into the most likely candidate god */
int
align_to_god(alignmnt)
aligntyp alignmnt;
{
	const char * name;
	switch(alignmnt) {
		case A_LAWFUL:
			return urole.lgod;
		case A_NEUTRAL:
			return urole.ngod;
		case A_CHAOTIC:
			return urole.cgod;
		case A_VOID:
			return GOD_THE_VOID;
		case A_NONE:
			return GOD_MOLOCH;
	}
	
	impossible("no matching god for align %d?", alignmnt);
	return GOD_NONE;
}

/* gets god name, with no hallu check */
const char *
godname_full(godnum)
int godnum;
{
	/* special cases that give alternative names for particular gods */

	if (godnum == GOD_NONE) {
		return "no one";
	}
	
	if (godnum == GOD_EDDERGUD && !(
		Race_if(PM_DROW) &&
		flags.stag && !flags.initgend
	))
		return "the black web";

	if (godnum == GOD_ILSENSINE && (
		Role_if(PM_ANACHRONONAUT)
	))
	{
		if (flags.questprogress == 2)
			return "_Ilsensine the Banished One";
		if (flags.questprogress == 0)
			return godlist[roles[flags.pantheon].lgod].name;
	}

	if (godnum == GOD_CHAOS && (
		In_FF_quest(&u.uz) && on_level(&chaose_level,&u.uz)
	))
		return "Chaos, with Cosmos in chains";

	if (godnum == GOD_MOLOCH && (
		In_mordor_quest(&u.uz) && on_level(&u.uz, &borehole_4_level)
	))
		return "Moloch, lieutenant of Melkor";

	/* general case: give the god's name as per godlist */
	return godlist[godnum].name;
}

/* gets god name OR a hallu god if appropriate */
const char *
godname(godnum)
int godnum;
{
    const char *gnam;

	if (Hallucination) {
		gnam = hallu_gods[rn2(SIZE(hallu_gods))];
		if (*gnam == '_') ++gnam;
		return gnam;
	}
	gnam = godname_full(godnum);
    if (*gnam == '_') ++gnam;
    return gnam;
}

int
god_faction(godnum)
int godnum;
{
	switch(godnum) {
		case GOD_EDDERGUD: return EDDER_SYMBOL;
		case GOD_VHAERAUN: return MAGTHERE;
		case GOD_LOLTH: return LOLTH_SYMBOL;
		case GOD_GHAUNADAUR: return GHAUNADAUR_SYMBOL;
		case GOD_VER_TAS: return VER_TAS_SYMBOL;
		case GOD_EILISTRAEE: return EILISTRAEE_SYMBOL;
		case GOD_KIARANSALI: return KIARANSALEE_SYMBOL;
		case GOD_THE_BLACK_MOTHER: return GOATMOM_FACTION;
		case GOD_ILSENSINE: return ILSENSINE_FACTION;
		case GOD_YALDABAOTH: return SEROPAENES_FACTION;
	}
	return -1;
}


const int *
god_minions(godnum)
int godnum;
{
	/* special cases */
	if (godnum == GOD_MOLOCH) {
		/* randomly between Ldevils and Cdemons (from godlist.h) */
		static const int LdevilsMinions[] = {Ldevils};
		static const int CdemonsMinions[] = {Cdemons};
		return rn2(2) ? LdevilsMinions : CdemonsMinions;
	}

	/* Once you know of Ilsensine, Ilsensine knows of you, even in the past.
	 * Until then, though, you get the standard minions for the god Ilsensine was */
	if (godnum == GOD_ILSENSINE && (
		Role_if(PM_ANACHRONONAUT) && flags.questprogress == 0
	)) {
		return godlist[roles[flags.pantheon].lgod].minionlist;
	}

	if (godnum == GOD_LOLTH && (
		Race_if(PM_DROW) && flags.initgend
	)) {
		/* female drow get gloves-off demon-Lolth minions */
		static const int NastyLolthMinions[] = {PM_SPROW,PM_YOCHLOL,PM_ANGEL,PM_MARILITH,NON_PM};
		return NastyLolthMinions;
	}
	
	return godlist[godnum].minionlist;
}

struct monst *
god_priest(godnum, sx, sy, sanctum)
int godnum;
int sx, sy;
int sanctum;   /* is it the seat of the high priest? */
{
	struct monst *priest;
	
	if(on_level(&sanctum_level, &u.uz)) {
		/* the Elder Priest guards the only altar in the sanctum */
		priest = makemon(&mons[PM_ELDER_PRIEST], sx + 1, sy, NO_MM_FLAGS);
	}
	else if(Role_if(PM_EXILE) && sanctum) {
		/* the astral altars are untended for a binder */
		priest = (struct monst *) 0;
	}
	else if(In_mordor_depths(&u.uz)){
		/* the High Shaman of moloch in Chaos3 */
		priest = makemon(&mons[PM_HIGH_SHAMAN], sx + 1, sy, NO_MM_FLAGS);
	}
	else if(Is_bridge_temple(&u.uz)){
		/* the Blasphemous Lurker in Neutral */
		priest = makemon(&mons[PM_BLASPHEMOUS_LURKER], sx, sy, NO_MM_FLAGS);
	}
	else {
		struct obj *otmp;
		priest = makemon(&mons[sanctum ? PM_HIGH_PRIEST : PM_ALIGNED_PRIEST],
			sx + 1, sy, NO_MM_FLAGS);
		if (priest) {
			/* special cases */
			switch (godnum) {
				case GOD_MOLOCH:
					give_mintrinsic(priest, POISON_RES);
					break;
				case GOD_OROME:
					priest->female = FALSE;
					break;
				case GOD_YAVANNA:
					priest->female = TRUE;
					break;
				case GOD_TULKAS:
					priest->female = FALSE;
					break;
				case GOD_VARDA_ELENTARI:
				case GOD_VAIRE:
				case GOD_NESSA:
					priest->female = TRUE;
					break;
				case GOD_MANWE_SULIMO:
				case GOD_MANDOS:
				case GOD_LORIEN:
					priest->female = FALSE;
					break;
				case GOD_EDDERGUD:
					priest->female = FALSE;
					if(!sanctum){
						newcham(priest,PM_DROW_ALIENIST,FALSE,FALSE);
						set_faction(priest, XAXOX);
						otmp = mongets(priest, DROVEN_CHAIN_MAIL, MKOBJ_NOINIT);
						if(otmp){
							otmp->ohaluengr = TRUE;
							otmp->oward = XAXOX;
						}
						otmp = mongets(priest, find_signet_ring(), MKOBJ_NOINIT);
						if(otmp){
							otmp->ohaluengr = TRUE;
							otmp->oward = EDDER_SYMBOL;
						}
						mongets(priest, DROVEN_HELM, MKOBJ_NOINIT);
						mongets(priest, HIGH_BOOTS, MKOBJ_NOINIT);
						otmp = mongets(priest, KHAKKHARA, MKOBJ_NOINIT);
						if(otmp){
							otmp->blessed = FALSE;
							otmp->cursed = TRUE;
							otmp->oerodeproof = TRUE;
							otmp->spe = 5;
							add_oprop(otmp, OPROP_BLADED);
							set_material_gm(otmp, OBSIDIAN_MT);
						}
						m_dowear(priest, TRUE);
						init_mon_wield_item(priest);
						m_level_up_intrinsic(priest);
					}
					break;
				case GOD_VHAERAUN:
					priest->female = FALSE;
					if(!sanctum){
						newcham(priest,PM_HEDROW_BLADEMASTER,FALSE,FALSE);
						set_faction(priest, LOLTH_SYMBOL);
						otmp = mongets(priest, DROVEN_PLATE_MAIL, MKOBJ_NOINIT);
						if(otmp){
							otmp->ohaluengr = TRUE;
							otmp->oward = MAGTHERE;
						}
						otmp = mongets(priest, find_signet_ring(), MKOBJ_NOINIT);
						if(otmp){
							otmp->ohaluengr = TRUE;
							otmp->oward = MAGTHERE;
						}
						mongets(priest, DROVEN_HELM, MKOBJ_NOINIT);
						otmp = mongets(priest, GAUNTLETS, MKOBJ_NOINIT);
						if(otmp) set_material_gm(otmp, SHADOWSTEEL);
						otmp = mongets(priest, ARMORED_BOOTS, MKOBJ_NOINIT);
						if(otmp) set_material_gm(otmp, SHADOWSTEEL);
						otmp = mongets(priest, DROVEN_GREATSWORD, MKOBJ_NOINIT);
						if(otmp){
							otmp->oerodeproof = TRUE;
							otmp->spe = 5;
							otmp->opoisoned = rn2(4) ? OPOISON_SLEEP : OPOISON_PARAL;
						}
						m_dowear(priest, TRUE);
						init_mon_wield_item(priest);
						m_level_up_intrinsic(priest);
					}
					break;
#define DROW_PRIESTESS_GEAR(sym) {\
	otmp = mongets(priest, DROVEN_PLATE_MAIL, MKOBJ_NOINIT);\
	if(otmp){\
		otmp->ohaluengr = TRUE;\
		otmp->oward = sym;\
	}\
	otmp = mongets(priest, find_signet_ring(), MKOBJ_NOINIT);\
	if(otmp){\
		otmp->ohaluengr = TRUE;\
		otmp->oward = sym;\
	}\
	mongets(priest, DROVEN_HELM, MKOBJ_NOINIT);\
	mongets(priest, HIGH_BOOTS, MKOBJ_NOINIT);\
	otmp = mongets(priest, VIPERWHIP, MKOBJ_NOINIT);\
	if(otmp){\
		otmp->oerodeproof = TRUE;\
		otmp->spe = 2;\
		otmp->opoisoned = rn2(4) ? OPOISON_BASIC : OPOISON_PARAL;\
		otmp->opoisonchrgs = 1;\
		otmp->ovar1 = 1+rnd(3);\
	}\
	mongets(priest, KHAKKHARA, MKOBJ_NOINIT);\
	m_dowear(priest, TRUE);\
	init_mon_wield_item(priest);\
	m_level_up_intrinsic(priest);\
}
				case GOD_LOLTH:
					if (!flags.initgend){
						priest->female = FALSE;
						if(!sanctum){
							newcham(priest,PM_HEDROW_WIZARD,FALSE,FALSE);
							set_faction(priest, LOLTH_SYMBOL);
							(void)mongets(priest, DROVEN_CROSSBOW, MKOBJ_NOINIT);
							m_initthrow(priest, DROVEN_BOLT, 24, MKOBJ_NOINIT);
						}
					}
					else {
						priest->female = TRUE;
						if(!sanctum) {
							newcham(priest,PM_DROW_MATRON,FALSE,FALSE);
							set_faction(priest, LOLTH_SYMBOL);
							DROW_PRIESTESS_GEAR(LOLTH_SYMBOL)
						}
					}
					break;
				case GOD_EILISTRAEE:
					priest->female = TRUE;
					if(!sanctum){
						newcham(priest,PM_STJARNA_ALFR,FALSE,FALSE);
						otmp = mongets(priest, DROVEN_PLATE_MAIL, MKOBJ_NOINIT);
						if(otmp){
							set_material_gm(otmp, MITHRIL);
							otmp->ohaluengr = TRUE;
							otmp->oward = EILISTRAEE_SYMBOL;
						}
						otmp = mongets(priest, find_signet_ring(), MKOBJ_NOINIT);
						if(otmp){
							set_material_gm(otmp, MITHRIL);
							otmp->ohaluengr = TRUE;
							otmp->oward = EILISTRAEE_SYMBOL;
						}
						otmp = mongets(priest, DROVEN_HELM, MKOBJ_NOINIT);
						if(otmp) set_material_gm(otmp, MITHRIL);
						mongets(priest, ELVEN_BOOTS, MKOBJ_NOINIT);
						otmp = mongets(priest, HIGH_ELVEN_WARSWORD, MKOBJ_NOINIT);
						if(otmp){
							otmp->spe = 2;
						}
						(void)mongets(priest, ELVEN_BOW, MKOBJ_NOINIT);
						m_initthrow(priest, ELVEN_ARROW, 24, MKOBJ_NOINIT);
						m_dowear(priest, TRUE);
						init_mon_wield_item(priest);
						m_level_up_intrinsic(priest);
					}
					break;
				case GOD_VER_TAS:
					priest->female = TRUE;
					if(!sanctum){
						newcham(priest,PM_DROW_MATRON,FALSE,FALSE);
						set_faction(priest, VER_TAS_SYMBOL);
						DROW_PRIESTESS_GEAR(VER_TAS_SYMBOL)
					}
					break;
				case GOD_PEN_A:
					priest->female = TRUE;
					if(!sanctum){
						newcham(priest,rn2(2) ? PM_DROW : PM_DRIDER,FALSE,FALSE);
						set_faction(priest, PEN_A_SYMBOL);
					}
					break;
				case GOD_GHAUNADAUR:
					if(!sanctum){
						if(rn2(2)){
							priest->female = TRUE;
							newcham(priest,PM_PRIESTESS_OF_GHAUNADAUR,FALSE,FALSE);
							set_faction(priest, GHAUNADAUR_SYMBOL);
							DROW_PRIESTESS_GEAR(GHAUNADAUR_SYMBOL)
						}
						else {
							priest->female = FALSE;
							newcham(priest,PM_HEDROW_BLADEMASTER,FALSE,FALSE);
							set_faction(priest, GHAUNADAUR_SYMBOL);
							otmp = mongets(priest, DROVEN_PLATE_MAIL, MKOBJ_NOINIT);
							if(otmp){
								otmp->ohaluengr = TRUE;
								otmp->oward = GHAUNADAUR_SYMBOL;
							}
							otmp = mongets(priest, find_signet_ring(), MKOBJ_NOINIT);
							if(otmp){
								otmp->ohaluengr = TRUE;
								otmp->oward = GHAUNADAUR_SYMBOL;
							}
							mongets(priest, DROVEN_HELM, MKOBJ_NOINIT);
							otmp = mongets(priest, GAUNTLETS, MKOBJ_NOINIT);
							if(otmp) set_material_gm(otmp, SHADOWSTEEL);
							otmp = mongets(priest, ARMORED_BOOTS, MKOBJ_NOINIT);
							if(otmp) set_material_gm(otmp, SHADOWSTEEL);
							otmp = mongets(priest, DROVEN_GREATSWORD, MKOBJ_NOINIT);
							if(otmp){
								otmp->oerodeproof = TRUE;
								otmp->spe = 5;
								otmp->opoisoned = rn2(4) ? OPOISON_ACID : OPOISON_PARAL;
							}
							m_dowear(priest, TRUE);
							init_mon_wield_item(priest);
							m_level_up_intrinsic(priest);
						}
					}
					break;
				case GOD_KIARANSALI:
					priest->female = TRUE;
					if(!sanctum){
						newcham(priest,PM_DROW_MATRON,FALSE,FALSE);
						set_faction(priest, KIARANSALEE_SYMBOL);
						DROW_PRIESTESS_GEAR(KIARANSALEE_SYMBOL)
					}
					break;
			}
		}
	}
	return priest;
}

/* 
 * returns the god you will communicate with at the altar at x,y
 * unlike a_gnum(), which is a direct access of altars[].godnum, (and can be used as an lvalue)
 * this comes up with a god for un-dedicated altars
 * 
 * ex: the deep blue sea is responsible for "a neutral altar" in a Pirate game
 */
int
god_at_altar(x, y)
int x, y;
{
	if(levl[x][y].typ != ALTAR) {
		if(goat_mouth_at(x, y))
			return GOD_THE_BLACK_MOTHER;
		else if(bokrug_idol_at(x, y))
			return GOD_BOKRUG__THE_WATER_LIZARD;
		else
			return GOD_NONE;
	}
	
	if (a_gnum(x, y) != GOD_NONE)
		return a_gnum(x, y);

	if (a_align(x, y) == galign(u.ualign.god))
		return u.ualign.god;

	return align_to_god(a_align(x, y));
}


/*
 * Returns TRUE if the two gods are on good terms with each other;
 * No minions sent if converting an altar from one to another.
 * Any attending priests will still get upset, though!
 */
boolean
gods_are_friendly(god1, god2)
int god1, god2;
{
	/* elf-gods are friendly with each other */
	/* NOTE: assumes order of elfgods in godlist.h */
	if ((GOD_OROME <= god1 && god1 <= GOD_LORIEN)
		&& (GOD_OROME <= god2 && god2 <= GOD_LORIEN))
	{
		return TRUE;
	}

	return FALSE;
}

/*
 * Returns TRUE if god will accept your worship/allegiance
 */
boolean
god_accepts_you(godnum)
int godnum;
{
	if (galign(godnum) == A_NONE)
		return FALSE;
	if (galign(godnum) == A_VOID && !Role_if(PM_EXILE))
		return FALSE;
	if (godnum == GOD_ILSENSINE)
		return FALSE;
	if (godnum == GOD_ZO_KALAR && u.detestation_ritual&RITUAL_LAW)
		return FALSE;
	if (godnum == GOD_LOBON && u.detestation_ritual&RITUAL_NEUTRAL)
		return FALSE;
	if (godnum == GOD_TAMASH && u.detestation_ritual&RITUAL_CHAOS)
		return FALSE;
	if ((godnum == GOD_LOLTH || godnum == GOD_VHAERAUN || godnum == GOD_VER_TAS || godnum == GOD_KIARANSALI || godnum == GOD_KEPTOLO)
		&& !Race_if(PM_DROW))
		return FALSE;
	if ((godnum == GOD_ILNEVAL || godnum == GOD_LUTHIC || godnum == GOD_GRUUMSH)
		&& !Race_if(PM_ORC))
		return FALSE;
	if (godnum == GOD_VELKA__GODDESS_OF_SIN)
		return FALSE;

	return TRUE;
}

/*pray.c*/
