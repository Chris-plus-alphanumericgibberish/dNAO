/*	SCCS Id: @(#)sit.c	3.4	2002/09/21	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"
#include "xhity.h"

int NDECL(dohomesit);

void
take_gold()
{
#ifndef GOLDOBJ
	if (u.ugold <= 0)  {
		You_feel("a strange sensation.");
	} else {
		You("notice you have no gold!");
		u.ugold = 0;
		flags.botl = 1;
	}
#else
        struct obj *otmp, *nobj;
	int lost_money = 0;
	for (otmp = invent; otmp; otmp = nobj) {
		nobj = otmp->nobj;
		if (otmp->oclass == COIN_CLASS) {
			lost_money = 1;
			delobj(otmp);
		}
	}
	if (!lost_money)  {
		You_feel("a strange sensation.");
	} else {
		You("notice you have no money!");
		flags.botl = 1;
	}
#endif
}

int
dosit()
{
	static const char sit_message[] = "sit on the %s.";
	register struct trap *trap;
	register int typ = levl[u.ux][u.uy].typ;


#ifdef STEED
	if (u.usteed) {
	    You("are already sitting on %s.", mon_nam(u.usteed));
	    return MOVE_CANCELLED;
	}
#endif

	if(!can_reach_floor())	{
	    if (Levitation)
		You("tumble in place.");
	    else
		You("are sitting on air.");
	    return MOVE_CANCELLED;
	} else if (is_pool(u.ux, u.uy, TRUE) && !Underwater) {  /* water walking */
	    goto in_water;
	}

	if ((trap = t_at(u.ux, u.uy)) != 0 ||
		   (u.utrap && (u.utraptype >= TT_LAVA))) {

	    if (u.utrap) {
		exercise(A_WIS, FALSE);	/* you're getting stuck longer */
		if(u.utraptype == TT_BEARTRAP) {
		    You_cant("sit down with your %s in the bear trap.", body_part(FOOT));
		    u.utrap++;
	        } else if(u.utraptype == TT_PIT) {
		    if(trap->ttyp == SPIKED_PIT) {
			You("sit down on a spike.  Ouch!");
			losehp(1, "sitting on an iron spike", KILLED_BY);
			exercise(A_STR, FALSE);
		    } else
			You("sit down in the pit.");
		    u.utrap += rn2(5);
		} else if(u.utraptype == TT_WEB) {
		    You("sit in the spider web and get entangled further!");
		    u.utrap += rn1(10, 5);
		} else if(u.utraptype == TT_SALIVA) {
		    You("sit in the gluey mass and get stuck worse!");
		    u.utrap += rn1(12, 5);
		} else if(u.utraptype == TT_LAVA) {
		    /* Must have fire resistance or they'd be dead already */
		    You("sit in the lava!");
		    u.utrap += rnd(4);
		    losehp(d(2,10), "sitting in lava", KILLED_BY);
		} else if(u.utraptype == TT_INFLOOR) {
		    You_cant("maneuver to sit!");
		    u.utrap++;
		}
	    } else {
	        You("sit down.");
			dotrap(trap, FORCEBUNGLE);
	    }
	}
	else if (is_lava(u.ux, u.uy)) {

	    /* must be WWalking */
	    You(sit_message, "lava");
	    burn_away_slime();
	    melt_frozen_air();
	    if (likes_lava(youracedata)) {
		pline_The("lava feels warm.");
		return MOVE_STANDARD;
	    }
	    pline_The("lava burns you!");
	    losehp(d((Fire_resistance ? 2 : 10), 10),
		   "sitting on lava", KILLED_BY);

	} else if(OBJ_AT(u.ux, u.uy)) {
	    register struct obj *obj;

	    obj = level.objects[u.ux][u.uy];
	    if (obj->otyp == WRITING_DESK){
			You("sit on the desk chair.");
			pline("It's reasonably comfortable.");
		}
	    else if (obj->otyp == BERGONIC_CHAIR){
			return sit_bergonic(obj);
		}
	    else if (obj->otyp == EXPENSIVE_BED || obj->otyp == BED || obj->otyp == BEDROLL || obj->otyp == GURNEY){
			if(obj->otyp == EXPENSIVE_BED){
				You("climb into the bed.");
			}
			else {
				You("sit on the %s.", obj->otyp == BED ? "bed" : obj->otyp == BEDROLL ? "bedroll" : obj->otyp == GURNEY ? "gurney" : "unidentified bedlike object");
			}
			if((u.nextsleep <= monstermoves && !(obj->otyp == BEDROLL && obj->nexthere)) || obj->otyp == EXPENSIVE_BED){
				if(yn("Take a nap?") == 'y'){
					u.nextsleep = moves+rnz(100)+180;
					u.lastslept = moves;
					fall_asleep(-rn1(180, 180), TRUE);
				}
			}
			else {
				if(obj->otyp == BEDROLL && obj->nexthere)
					pline("It's not very comfortable...");
				else
					pline("It's %scomfortable, but you're not tired.", obj->otyp == BED ? "" : "reasonably ");
			}
		}
	    else {
			You("sit on %s.", the(xname(obj)));
			if (!(Is_box(obj) || obj->obj_material == CLOTH))
				pline("It's not very comfortable...");
		}

	} else if(Underwater || Is_waterlevel(&u.uz)) {
	    if (Is_waterlevel(&u.uz))
		There("are no cushions floating nearby.");
	    else
		You("sit down on the muddy bottom.");
	} else if(is_pool(u.ux, u.uy, TRUE)) {
 in_water:
	    You("sit in the water.");
	    if (!rn2(10) && uarm)
		(void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst, FALSE);
	    if (!rn2(10) && uarmf && uarmf->otyp != WATER_WALKING_BOOTS)
		(void) rust_dmg(uarm, "armor", 1, TRUE, &youmonst, FALSE);
#ifdef SINKS
	} else if(IS_SINK(typ)) {

	    You(sit_message, defsyms[S_sink].explanation);
	    Your("%s gets wet.", humanoid(youracedata) ? "rump" : "underside");
#endif
	} else if(IS_ALTAR(typ)) {

	    You(sit_message, defsyms[S_altar].explanation);
	    altar_wrath(u.ux, u.uy);

	} else if(IS_GRAVE(typ)) {

	    You(sit_message, defsyms[S_grave].explanation);

	} else if(IS_SEAL(typ)) {

	    You(sit_message, defsyms[S_seal].explanation);

	} else if(typ == STAIRS) {

	    You(sit_message, "stairs");

	} else if(typ == LADDER) {

	    You(sit_message, "ladder");

	} else if (IS_FORGE(typ)) {
		You(sit_message, "forge");
		burn_away_slime();
		melt_frozen_air();
		if (likes_fire(youmonst.data) || Fire_resistance) {
			pline_The("forge feels nice and cozy.");
			return MOVE_STANDARD;
		}
		pline("Argh!  This forge is hot!");
		losehp(d(4, 4), /* lava damage */
			"sitting on a forge", KILLED_BY);
	} else if (is_ice(u.ux, u.uy)) {

	    You(sit_message, defsyms[S_ice].explanation);
	    if (!Cold_resistance) pline_The("ice feels cold.");
		roll_frigophobia();

	} else if (typ == DRAWBRIDGE_DOWN) {

	    You(sit_message, "drawbridge");

	} else if(IS_THRONE(typ)) {
		if(Role_if(PM_NOBLEMAN) && In_quest(&u.uz) && !(Race_if(PM_ELF) || Race_if(PM_DWARF))){
			You(sit_message, defsyms[S_throne].explanation);
			if((uarmc &&
			  ((!Race_if(PM_VAMPIRE) && uarmc->oartifact == ART_MANTLE_OF_HEAVEN) ||
			  ( Race_if(PM_VAMPIRE) && uarmc->oartifact == ART_VESTMENT_OF_HELL) ||
			  ( Race_if(PM_DROW) && flags.initgend && uarmc->oartifact == ART_WEB_OF_THE_CHOSEN) ||
			  ( Race_if(PM_DROW) && !flags.initgend && uarmc->oartifact == ART_CLOAK_OF_THE_CONSORT)
			  )) || u.uevent.qcompleted
			){
			if(~levl[u.ux][u.uy].looted & (NOBLE_GENO|NOBLE_KNOW|NOBLE_PETS|NOBLE_WISH)){
			  switch(dohomesit()){
				case NOBLE_GENO:
					pline("A voice echoes:");
					verbalize("By thy Imperious order, %s...",
						  flags.female ? "Dame" : "Sire");
					do_genocide(5);	/* REALLY|ONTHRONE, see do_genocide() */
					levl[u.ux][u.uy].looted |= NOBLE_GENO;
				break;
				case NOBLE_KNOW:
					You("use your noble insight!");
					if (invent) {
						/* rn2(5) agrees w/seffects() */
						identify_pack(rn2(5));
					}
					levl[u.ux][u.uy].looted |= NOBLE_KNOW;
				break;
				case NOBLE_PETS:{
					int cnt = rnd(10);
					struct monst *mtmp;
					
					pline("A voice echoes:");
					verbalize("Thy audience hath been summoned, %s!",
						  flags.female ? "Dame" : "Sire");
					while(cnt--){
						mtmp = makemon(courtmon(monsndx(youracedata)), u.ux, u.uy, MM_EDOG|MM_ADJACENTOK|MM_NOCOUNTBIRTH);
						if(mtmp) initedog(mtmp);
					}
					levl[u.ux][u.uy].looted |= NOBLE_PETS;
				}break;
				case NOBLE_WISH:{
					struct monst *mtmp;
					struct monst *mtmp2 = (struct monst*)0;
					struct monst *mtmp3 = (struct monst*)0;
					if (DimensionalLock || !(mtmp = makemon(&mons[PM_DJINNI], u.ux, u.uy, NO_MM_FLAGS))){
						pline("Nothing appears to your summon!");
						break;
					}
					if ((u.uevent.utook_castle & ARTWISH_EARNED) && !(u.uevent.utook_castle & ARTWISH_SPENT))
						mtmp2 = makemon(&mons[PM_PSYCHOPOMP], u.ux, u.uy, NO_MM_FLAGS);
					if ((u.uevent.uunknowngod & ARTWISH_EARNED) && !(u.uevent.uunknowngod & ARTWISH_SPENT))
						mtmp3 = makemon(&mons[PM_PRIEST_OF_AN_UNKNOWN_GOD], u.ux, u.uy, NO_MM_FLAGS);
					if (!Blind) {
						pline("%s appears in a cloud of smoke!", Amonnam(mtmp));
						if (mtmp2 || mtmp3)
							pline("It is accompanied by %s%s%s.",
							mtmp2 ? a_monnam(mtmp2) : "",
							(mtmp2 && mtmp3) ? " and " : "",
							mtmp3 ? a_monnam(mtmp3) : "");
						pline("%s speaks.", Monnam(mtmp));
					}
					else {
						You("smell acrid fumes.");
						pline("%s speaks.", Something);
					}
					verbalize("You have summoned me.  I will grant one wish!");
					int artwishes = u.uconduct.wisharti;
					makewish(allow_artwish() | WISH_VERBOSE);
					if (u.uconduct.wisharti > artwishes) {
						/* made artifact wish */
						if (mtmp2) {
							pline("You feel %s presence fade.", s_suffix(mon_nam(mtmp2)));
							u.uevent.utook_castle |= ARTWISH_SPENT;
						}
						else if (mtmp3) {
							pline("You feel %s presence fade.", s_suffix(mon_nam(mtmp3)));
							u.uevent.uunknowngod |= ARTWISH_SPENT;
						}
					}
					mongone(mtmp);
					if (mtmp2)	mongone(mtmp2);
					if (mtmp3)	mongone(mtmp3);
					levl[u.ux][u.uy].looted |= NOBLE_WISH;
				}break;
				default:
					You_feel("right at home.");
				break;
			 }
			} else {
					You_feel("right at home.");
			}
			} else {
					You_feel("that something is missing....");
			}
		} else {
			You(sit_message, defsyms[S_throne].explanation);
			if ((Is_stronghold(&u.uz)) ? (u.uevent.utook_castle) : (rnd(6) > 4)) {
			switch (rnd(13))  {
				case 1:
				(void) adjattrib(rn2(A_MAX), -rn1(4,3), FALSE);
				losehp(rnd(10), "cursed throne", KILLED_BY_AN);
				break;
				case 2:
				(void) adjattrib(rn2(A_MAX), 1, FALSE);
				break;
				case 3:
				pline("A%s electric shock shoots through your body!",
					  (Shock_resistance) ? "n" : " massive");
				losehp(Shock_resistance ? rnd(6) : rnd(30),
					   "electric chair", KILLED_BY_AN);
				/* damage inventory */
				if (!UseInvShock_res(&youmonst)){
					if (!rn2(5)) (void) destroy_item(&youmonst, RING_CLASS, AD_ELEC);
					if (!rn2(5)) (void) destroy_item(&youmonst, WAND_CLASS, AD_ELEC);
				}
				exercise(A_CON, FALSE);
				break;
				case 4:
				You_feel("much, much better!");
				if (Upolyd) {
					if (u.mh >= (u.mhmax - 5)){
						u.uhpmod += 4;
						calc_total_maxhp();
					}
					u.mh = u.mhmax;
				}
				else if(u.uhp >= (u.uhpmax - 5)){
					u.uhpmod += 4;
					calc_total_maxhp();
				}
				u.uhp = u.uhpmax;
				make_blinded(0L,TRUE);
				make_sick(0L, (char *) 0, FALSE, SICK_ALL);
				heal_legs();
				flags.botl = 1;
				break;
				case 5:
				take_gold();
				break;
				case 6:
				if(u.uluck + rn2(5) < 0 || DimensionalLock) {
					You_feel("your luck is changing.");
					change_luck(1);
				}
				else
				{
				struct monst *mtmp;
				struct monst *mtmp2 = (struct monst*)0;
				struct monst *mtmp3 = (struct monst*)0;
				if (!(mtmp = makemon(&mons[PM_DJINNI], u.ux, u.uy, NO_MM_FLAGS))){
					pline1(nothing_happens);
					break;
				}
				if ((u.uevent.utook_castle & ARTWISH_EARNED) && !(u.uevent.utook_castle & ARTWISH_SPENT))
					mtmp2 = makemon(&mons[PM_PSYCHOPOMP], u.ux, u.uy, NO_MM_FLAGS);
				if ((u.uevent.uunknowngod & ARTWISH_EARNED) && !(u.uevent.uunknowngod & ARTWISH_SPENT))
					mtmp3 = makemon(&mons[PM_PRIEST_OF_AN_UNKNOWN_GOD], u.ux, u.uy, NO_MM_FLAGS);
				if (!Blind) {
					pline("%s appears in a cloud of smoke!", Amonnam(mtmp));
					if (mtmp2 || mtmp3)
						pline("It is accompanied by %s%s%s.",
						mtmp2 ? a_monnam(mtmp2) : "",
						(mtmp2 && mtmp3) ? " and " : "",
						mtmp3 ? a_monnam(mtmp3) : "");
					pline("%s speaks.", Monnam(mtmp));
				}
				else {
					You("smell acrid fumes.");
					pline("%s speaks.", Something);
				}
				verbalize("You have summoned me.  I will grant one wish!");
				int artwishes = u.uconduct.wisharti;
				makewish(allow_artwish() | WISH_VERBOSE);
				if (u.uconduct.wisharti > artwishes) {
					/* made artifact wish */
					if (mtmp2) {
						pline("You feel %s presence fade.", s_suffix(mon_nam(mtmp2)));
						mongone(mtmp2);
						mtmp2 = (struct monst*) 0;
						u.uevent.utook_castle |= ARTWISH_SPENT;
					}
					else if (mtmp3) {
						pline("You feel %s presence fade.", s_suffix(mon_nam(mtmp3)));
						mongone(mtmp3);
						mtmp3 = (struct monst*) 0;
						u.uevent.uunknowngod |= ARTWISH_SPENT;
					}
				}
				pline("The djinni%s disappears with a puff of smoke.", (mtmp2 || mtmp3) ? " and their entourage" : "");
				mongone(mtmp);
				if (mtmp2)	mongone(mtmp2);
				if (mtmp3)	mongone(mtmp3);
				}
				break;
				case 7:
				{
				int cnt = rnd(10);

				pline("A voice echoes:");
				verbalize("Thy audience hath been summoned, %s!",
					  flags.female ? "Dame" : "Sire");
				while(cnt--)
					(void) makemon(courtmon(0), u.ux, u.uy, MM_ADJACENTOK);
				break;
				}
				case 8:
				pline("A voice echoes:");
				verbalize("By thy Imperious order, %s...",
					  flags.female ? "Dame" : "Sire");
				do_genocide(5);	/* REALLY|ONTHRONE, see do_genocide() */
				break;
				case 9:
				pline("A voice echoes:");
		verbalize("A curse upon thee for sitting upon this most holy throne!");
				if (Luck > 0)  {
					make_blinded(Blinded + rn1(100,250),TRUE);
				} else	    rndcurse();
				break;
				case 10:
				if (Luck < 0 || (HSee_invisible & INTRINSIC))  {
					if (level.flags.nommap) {
						pline(
						"A terrible drone fills your head!");
						make_confused(HConfusion + rnd(30),
										FALSE);
					} else {
						pline("An image forms in your mind.");
						do_mapping();
					}
				} else  {
					Your("vision becomes clear.");
					HSee_invisible |= TIMEOUT_INF;
					newsym(u.ux, u.uy);
				}
				break;
				case 11:
				if (Luck < 0)  {
					You_feel("threatened.");
					aggravate();
				} else  {

					You_feel("a wrenching sensation.");
					tele();		/* teleport him */
				}
				break;
				case 12:
				You("are granted an insight!");
				if (invent) {
					/* rn2(5) agrees w/seffects() */
					identify_pack(rn2(5));
				}
				break;
				case 13:
				Your("mind turns into a pretzel!");
				make_confused(HConfusion + rn1(7,16),FALSE);
				break;
				default:	impossible("throne effect");
					break;
			}
			} else {
			if (Is_stronghold(&u.uz) && !u.uevent.utook_castle)
			{
				u.uevent.utook_castle = 1;
				give_castle_trophy();
				You_feel("worthy.");
				return MOVE_STANDARD;
			}
			else if (is_prince(youracedata) || Role_if(PM_NOBLEMAN))
				You_feel("very comfortable here.");
			else
				You_feel("somehow out of place...");
			}

			if (!rn2(3) && IS_THRONE(levl[u.ux][u.uy].typ)) {
			/* may have teleported */
			levl[u.ux][u.uy].typ = ROOM;
			pline_The("throne vanishes in a puff of logic.");
			if(u.sealsActive&SEAL_DANTALION) unbind(SEAL_DANTALION,TRUE);
			newsym(u.ux,u.uy);
			}
		}
	} else if (lays_eggs(youracedata)) {
		struct obj *uegg;

		if (!flags.female) {
			pline("Males can't lay eggs!");
			return MOVE_CANCELLED;
		}

		if (YouHunger < (int)objects[EGG].oc_nutrition) {
			You("don't have enough energy to lay an egg.");
			return MOVE_CANCELLED;
		}

		uegg = mksobj(EGG, MKOBJ_NOINIT);
		uegg->spe = 1;
		uegg->quan = 1;
		uegg->owt = weight(uegg);
		uegg->corpsenm = egg_type_from_parent(u.umonnum, FALSE);
		uegg->known = uegg->dknown = 1;
		if(!Sterile){
			attach_egg_hatch_timeout(uegg);
		}
		You("lay an egg.");
		dropy(uegg);
		stackobj(uegg);
		morehungry((int)objects[EGG].oc_nutrition);
	} else if (u.uswallow)
		There("are no seats in here!");
	else
		pline("Having fun sitting on the %s?", surface(u.ux,u.uy));
	return MOVE_STANDARD;
}

int
sit_bergonic(chair)
struct obj *chair;
{
	You("sit on %s.", the(xname(chair)));
	if(chair->spe <= 0){
		pline("It's reasonably comfortable.");
	}
	else {
		chair->spe--;
		pline("The chair's electrodes stick into your %s.", body_part(HEAD));
		losehp(reduce_dmg(&youmonst, d(2,4), TRUE, FALSE), "electrodes", KILLED_BY);
		pline("A%s electric shock shoots through your body!",
			  (Shock_resistance) ? "n" : " massive");
		losehp(Shock_resistance ? rnd(6) : rnd(30),
			   "electric chair", KILLED_BY_AN);
		/* damage inventory */
		if (!UseInvShock_res(&youmonst)){
			if (!rn2(5)) (void) destroy_item(&youmonst, RING_CLASS, AD_ELEC);
			if (!rn2(5)) (void) destroy_item(&youmonst, WAND_CLASS, AD_ELEC);
		}
		exercise(A_CON, FALSE);
		switch(rn2(12)){
			case 0:
			case 10:
				pline("That stimulated your mind!");
				//Note: 2/3rds int, otherwise 50/50 wis/cha
				(void) adjattrib(rn2(3) ? A_INT : rn2(2) ? A_WIS : A_CHA, 1, FALSE);
			break;
			case 1:
			case 11:
				pline("That fried your mind!");
				//Note: 2/3rds wis, otherwise 50/50 int/cha
				(void) adjattrib(rn2(3) ? A_WIS : rn2(2) ? A_INT : A_CHA, -1, FALSE);
				make_confused(HConfusion+d(10,10), FALSE);
			break;
			case 2:
				pline("That was mind-expending!");
				if (!BClairvoyant)
					do_vicinity_map(u.ux,u.uy);
				/* at present, only one thing blocks clairvoyance */
				else if (uarmh && uarmh->otyp == CORNUTHAUM)
					You("sense a pointy hat on top of your %s.",
					body_part(HEAD));
				incr_itimeout(&HClairvoyant, rn1(500,500));
			break;
			case 3:
				pline("That was an enlightening experience!");
				(void)doenlightenment();
			break;
			case 4:
				pline("Your make some insightful connections!");
				if (invent) {
					/* rn2(5) agrees w/seffects() */
					identify_pack(rn2(5));
				}
				know_random_obj(2 + rn2(3) + rn2(5));
			break;
			case 5:
				pline("Your mind broadcasts maddening dreams!");
				aggravate();
			break;
			case 6:
				pline("The chairs electrodes glow red-hot!");
				/* damage inventory */
				if (!UseInvFire_res(&youmonst)){
					destroy_item(&youmonst, POTION_CLASS, AD_FIRE);
					if (!rn2(6)) destroy_item(&youmonst, SCROLL_CLASS, AD_FIRE);
					if (!rn2(10)) destroy_item(&youmonst, SPBOOK_CLASS, AD_FIRE);
				}
				if(!Fire_resistance)
					losehp(rnd(20), "red-hot electrodes", KILLED_BY);
				/* other effects */
				burn_away_slime();
				melt_frozen_air();
			break;
			case 7:
				if(uarmh && uarmh->spe < 5){
					pline("Your %s vibrates slightly.", xname(uarmh));
					uarmh->spe++;
					adj_abon(uarmh, 1);
				}
				else {
					pline("Your fore%s tingles for a moment.", body_part(HEAD));
				}
			break;
			case 8:
			{
				int duration = rn1(200, 200);
				pline("You fall into a nightmare-filled slumber!");
				fall_asleep(-duration, TRUE);
				make_confused(HConfusion + duration-10, FALSE);
				make_hallucinated(HHallucination + duration-10, FALSE, 0L);
				
			}
			break;
		}
	}
	return MOVE_STANDARD;
}

/* returns TRUE if the caller should print a message */
boolean
rndcurse()			/* curse a few inventory items at random! */
{
	int	nobj = 0;
	int	cnt, onum;
	struct	obj	*otmp;
	boolean did_curse = FALSE;
	static const char mal_aura[] = "feel a malignant aura surround %s.";
	
	if(Curse_res(&youmonst, TRUE))
		return FALSE;

	if(Antimagic) {
	    shieldeff(u.ux, u.uy);
	    You(mal_aura, "you");
	}

	for (otmp = invent; otmp; otmp = otmp->nobj) {
#ifdef GOLDOBJ
	    /* gold isn't subject to being cursed or blessed */
	    if (otmp->oclass == COIN_CLASS) continue;
#endif
	    nobj++;
	}
	if (nobj) {
	    for (cnt = rnd(6/((!!Antimagic) + (!!Half_spell_damage) + (!!u.uvaul_duration) + 1));
		 cnt > 0; cnt--)  {
		onum = rnd(nobj);
		for (otmp = invent; otmp; otmp = otmp->nobj) {
#ifdef GOLDOBJ
		    /* as above */
		    if (otmp->oclass == COIN_CLASS) continue;
#endif
		    if (--onum == 0) break;	/* found the target */
		}
		/* the !otmp case should never happen; picking an already
		   cursed item happens--avoid "resists" message in that case */
		if (!otmp || otmp->cursed) continue;	/* next target */

		if(otmp->oartifact && arti_gen_prop(otmp, ARTG_MAJOR) &&
		   rn2(10) < 8) {
		    pline("%s!", Tobjnam(otmp, "resist"));
		    continue;
		}

		if(otmp->blessed)
			unbless(otmp);
		else
			curse(otmp);
	    }
		did_curse = TRUE;
	    update_inventory();
	}

#ifdef STEED
	/* treat steed's saddle as extended part of hero's inventory */
	if (u.usteed && !rn2(4) &&
		(otmp = which_armor(u.usteed, W_SADDLE)) != 0 &&
		!otmp->cursed) {	/* skip if already cursed */
	    if (otmp->blessed)
		unbless(otmp);
	    else
		curse(otmp);
	    if (!Blind) {
		pline("%s %s %s.",
		      s_suffix(upstart(y_monnam(u.usteed))),
		      aobjnam(otmp, "glow"),
		      hcolor(otmp->cursed ? NH_BLACK : (const char *)"brown"));
		otmp->bknown = TRUE;
	    }
		did_curse = TRUE;
	}
#endif	/*STEED*/
	if(did_curse){
		IMPURITY_UP(u.uimp_curse)
	}
	return did_curse;
}

/* returns TRUE if the caller should print a message */
boolean
mrndcurse(mtmp)			/* curse a few inventory items at random! */
register struct monst *mtmp;
{
	int	nobj = 0;
	int	cnt, onum;
	struct	obj	*otmp;
	static const char mal_aura[] = "feel a malignant aura surround %s.";

	boolean resists = resist(mtmp, 0, 0, FALSE);
	boolean visible = canseemon(mtmp);
	
	if(Curse_res(mtmp, TRUE))
		return FALSE;

	if(resists) {
	    shieldeff(mtmp->mx, mtmp->my);
		if (visible) You(mal_aura, mon_nam(mtmp));
	}

	for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
#ifdef GOLDOBJ
	    /* gold isn't subject to being cursed or blessed */
	    if (otmp->oclass == COIN_CLASS) continue;
#endif
	    nobj++;
	}
	if (nobj) {
	    for (cnt = rnd(6/((!!resists) + 1));
		 cnt > 0; cnt--)  {
		onum = rnd(nobj);
		for (otmp = mtmp->minvent; otmp; otmp = otmp->nobj) {
#ifdef GOLDOBJ
		    /* as above */
		    if (otmp->oclass == COIN_CLASS) continue;
#endif
		    if (--onum == 0) break;	/* found the target */
		}
		/* the !otmp case should never happen; picking an already
		   cursed item happens--avoid "resists" message in that case */
		if (!otmp || otmp->cursed) continue;	/* next target */

		if(otmp->oartifact && arti_gen_prop(otmp, ARTG_MAJOR) &&
		   rn2(10) < 8) {
			if (visible) pline("%s!", Tobjnam(otmp, "resist"));
		    continue;
		}

		if(otmp->blessed)
			unbless(otmp);
		else
			curse(otmp);
	    }
	    update_inventory(); //Is this needed??? Maybe catches cursed saddles?
		return TRUE;
	}
	return FALSE;
}

/* remove a random INTRINSIC ability
 * that was gained via eating corpses
 * or some other re-appliable source */
void
attrcurse()
{
	switch(rnd(10)) {
	case 1 : if (HFire_resistance & TIMEOUT) {
			HFire_resistance &= ~TIMEOUT;
			You_feel("warmer.");
			break;
		}
	case 2 : if (HTeleportation & TIMEOUT) {
			HTeleportation &= ~TIMEOUT;
			You_feel("less jumpy.");
			break;
		}
	case 3 : if (HPoison_resistance & TIMEOUT) {
			HPoison_resistance &= ~TIMEOUT;
			You_feel("a little sick!");
			break;
		}
	case 4 : if (HTelepat & TIMEOUT) {
			HTelepat &= ~TIMEOUT;
			if (Blind && !Blind_telepat)
			    see_monsters();	/* Can't sense mons anymore! */
			Your("senses fail!");
			break;
		}
	case 5 : if (HCold_resistance & TIMEOUT) {
			HCold_resistance &= ~TIMEOUT;
			You_feel("cooler.");
			break;
		}
	case 6 : if (HInvis & TIMEOUT) {
			HInvis &= ~TIMEOUT;
			You_feel("paranoid.");
			break;
		}
	case 7 : if (HSee_invisible & TIMEOUT) {
			HSee_invisible &= ~TIMEOUT;
			You("%s!", Hallucination ? "tawt you taw a puttie tat"
						: "thought you saw something");
			break;
		}
	case 8 : if (HFast & TIMEOUT) {
			HFast &= ~TIMEOUT;
			You_feel("slower.");
			break;
		}
	case 9 : if (HStealth & TIMEOUT) {
			HStealth &= ~TIMEOUT;
			You_feel("clumsy.");
			break;
		}
	case 10: if (HAggravate_monster & TIMEOUT) {
			HAggravate_monster &= ~TIMEOUT;
			You_feel("less attractive.");
			break;
		}
	default: break;
	}
}

int
dohomesit()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "What do you command?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	if( !(levl[u.ux][u.uy].looted & NOBLE_GENO) ){
		Sprintf(buf, "Order a genocide");
		any.a_int = NOBLE_GENO;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'g', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	
	if(!(levl[u.ux][u.uy].looted & NOBLE_KNOW)){
		Sprintf(buf, "Use your insight");
		any.a_int = NOBLE_KNOW;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'i', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	
	if(!(levl[u.ux][u.uy].looted & NOBLE_PETS)){
		Sprintf(buf, "Grant an audience");
		any.a_int = NOBLE_PETS;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'a', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	
	if(!(levl[u.ux][u.uy].looted & NOBLE_WISH)){
		Sprintf(buf, "Demand tribute");
		any.a_int = NOBLE_WISH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'W', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	end_menu(tmpwin, "Your command?");

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

/*sit.c*/
