#include "hack.h"

#include "seduce.h"

# ifdef SEDUCE
STATIC_DCL void FDECL(mayberem, (struct obj *, const char *, BOOLEAN_P));
STATIC_DCL void FDECL(lrdmayberem, (struct obj *, const char *, BOOLEAN_P));
STATIC_DCL void FDECL(mlcmayberem, (struct obj *, const char *, BOOLEAN_P));
STATIC_DCL void FDECL(mayberem_common, (struct obj *, const char *, BOOLEAN_P));
STATIC_DCL void FDECL(sflmayberem, (struct obj *, const char *, BOOLEAN_P));
STATIC_DCL void FDECL(palemayberem, (struct obj *, const char *, BOOLEAN_P));
STATIC_DCL boolean FDECL(sedu_helpless, (struct monst *));
STATIC_DCL int FDECL(sedu_refuse, (struct monst *));
STATIC_DCL boolean FDECL(sedu_roll, (struct monst *, boolean));
STATIC_DCL void FDECL(sedu_payment, (struct monst *));
STATIC_DCL void FDECL(sedu_undress, (struct monst *));
STATIC_DCL void FDECL(sedu_adornment_ring, (struct monst *));
STATIC_DCL void FDECL(sedu_knife, (struct monst *));
STATIC_DCL void FDECL(sedu_wornout, (struct monst *, boolean));
STATIC_DCL void FDECL(sedu_timestandsstill, (struct monst *, boolean));
STATIC_DCL int FDECL(sedu_select_effect, (struct monst *, boolean));
STATIC_DCL void FDECL(seduce_effect, (struct monst *, int));
# endif

static const char tools[] = { TOOL_CLASS, 0 };

#ifdef OVL1
int
could_seduce(magr,mdef,mattk)
struct monst *magr, *mdef;
struct attack *mattk;
/* returns 0 if seduction impossible,
 *	   1 if fine,
 *	   2 if wrong gender for nymph */
{
	register struct permonst *pagr;
	boolean agrinvis, defperc;
	boolean youagr = &youmonst == magr;
	boolean youdef = &youmonst == mdef;
	xchar genagr, gendef;
	
	if(youdef || youagr){
		if(Chastity) return 0;
		
		if ((uleft  && uleft->otyp  == find_engagement_ring()) ||
			(uright && uright->otyp == find_engagement_ring()))
			return 0;
	}
	
	if (!intelligent_mon(mdef)) return (0);
	if(youagr) {
		pagr = youracedata;
		agrinvis = (Invis != 0);
		genagr = poly_gender();
	} else {
		pagr = magr->data;
		agrinvis = magr->minvis;
		genagr = gender(magr);
	}
	if(youdef) {
		defperc = (See_invisible(magr->mx,magr->my) != 0);
		gendef = poly_gender();
	} else {
		defperc = mon_resistance(mdef,SEE_INVIS);
		gendef = gender(mdef);
	}
	
	if(mattk && mattk->adtyp != AD_SSEX && mattk->adtyp != AD_LSEX && 
		mattk->adtyp != AD_SEDU && mattk->adtyp != AD_SITM
	) return 0;
	
	
	if(agrinvis && !defperc && magr->mtyp != PM_PHANTASM && magr->mtyp != PM_NEVERWAS
#ifdef SEDUCE
		&& mattk && mattk->adtyp != AD_SSEX && mattk->adtyp != AD_LSEX
#endif
		)
		return 0;

	if(pagr->mtyp == PM_SMALL_GOAT_SPAWN || pagr->mtyp == PM_GOAT_SPAWN || pagr->mtyp == PM_GIANT_GOAT_SPAWN || pagr->mtyp == PM_BLESSED || has_template(magr, MISTWEAVER) || pagr->mtyp == PM_PHANTASM || pagr->mtyp == PM_BEAUTEOUS_ONE)
		return 1;
	
	if(pagr->mlet == S_NYMPH || pagr->mtyp == PM_INCUBUS || pagr->mtyp == PM_SUCCUBUS
			|| pagr->mtyp == PM_CARMILLA || pagr->mtyp == PM_VLAD_THE_IMPALER || pagr->mtyp == PM_LEVISTUS){
		if(genagr == 1 - gendef)
			return 1;
		else
			return (pagr->mlet == S_NYMPH || pagr->mtyp == PM_LEVISTUS) ? 2 : 0;
	}
	else if(pagr->mtyp == PM_MOTHER_LILITH || pagr->mtyp == PM_BELIAL
		 /*|| pagr->mtyp == PM_SHAMI_AMOURAE*/){
		if(genagr == 1 - gendef) return 1;
		else return 0;
	}
	else if(pagr->mtyp == PM_FIERNA) return 2;
	else if(pagr->mtyp == PM_ALRUNES) return (genagr == gendef) ? 1 : 2;
	else if(pagr->mtyp == PM_MALCANTHET || pagr->mtyp == PM_GRAZ_ZT
		 || pagr->mtyp == PM_PALE_NIGHT || pagr->mtyp == PM_AVATAR_OF_LOLTH) 
			return 1;
	else return 0;
}

#endif /* OVL1 */
#ifdef OVLB

#ifdef SEDUCE

/* combined seduction function for most creatures */
/* Pale Night is entirely outsourced to its own function */
/* returns 1 if monster should stop attacking */
int
doseduce(mon)
struct monst * mon;
{
	boolean helpless;
	boolean badeffect;
	int effect;

	/* their identity is revealed! */
	if(mon->mappearance) {
		/* the "real delusions" madness could have poly'd foocubus, check that we aren't being seduced by a dragon */
		int oldmtyp = mon->mtyp;
		seemimic_ambush(mon);
		if (oldmtyp != mon->mtyp)
			return 0;
	}

	/* Pale Night is too different to use the common shared seduce, bleh */
	if (mon->mtyp == PM_PALE_NIGHT) {
		dopaleseduce(mon);
		return 1;
	}

	/* monster may be worn out (with an exception for the avatar of lolth?) */
	if (mon->mcan || (mon->mspec_used && !(
		mon->mtyp == PM_AVATAR_OF_LOLTH
	))) {
		/* message */
		if (mon->mtyp == PM_INCUBUS || mon->mtyp == PM_SUCCUBUS) {
			pline("%s acts as though %s has got a %sheadache.",
					Monnam(mon), mhe(mon),
					mon->mcan ? "severe " : "");
		}
		else {
			pline("%s is uninterested in you.", Monnam(mon));
		}
		/* return early */
		return 0;
	}

	/* response to player being unconscious */
	if ((helpless = unconscious()) && sedu_helpless(mon)) {
		return 0;
	}

	/* fluff message */
	if (TRUE) {
		if (Blind) pline("It caresses you...");
		else You_feel("very attracted to %s.", mon_nam(mon));
	}
	else {
		/* code for unfinished monster, PM_SHAMI_AMOURAE */
		if (Blind) You_feel("Something grab you...");
		else pline("%s grabs you.", Monnam(mon));
	}

	/* interact with adornment ring */
	sedu_adornment_ring(mon);

	/* undress the player */
	sedu_undress(mon);

	/* roll for good/bad effect */
	/* happens before sedu_refuse(), as a Lolth bad effect is unrefusable, but a good effect is */
	badeffect = sedu_roll(mon, helpless);

	/* if the player is still dressed, refuse */	
	if (uarm || uarmc || (uwep && uwep->oartifact==ART_TENSA_ZANGETSU)) {
		if (sedu_refuse(mon)) {
			/* seducer was refused */
			return 1;
		}
		else {
			/* seducer acts against you */
			badeffect = TRUE;
		}
	}
	else if (u.ualign.type == A_CHAOTIC || u.ualign.type == A_NONE)
		adjalign(1);

	/* select sedu effect */
	effect = sedu_select_effect(mon, badeffect);

	/* "time stands still" message, or replacement */
	sedu_timestandsstill(mon, badeffect);

	/* clockworks get a bit harder of a time with Malcanthet */
	if (mon->mtyp == PM_MALCANTHET && !badeffect && uclockwork) {
		/* even for a good effect, she attacks you a bit before giving her boon */
		pline("She becomes very angry when she discovers your mechanical nature.");
		pline("She claws at you...");
		losehp(d(4, 4), "an angry paramour", KILLED_BY);
		pline("...but you manage to distract her before she does serious harm.");
	}

	/* perform effect */
	seduce_effect(mon, effect);
	/*Some effects result in theft, some thefts result in death!*/
	if(DEADMONSTER(mon))
		return 1;

	/* knife to the ribs -- Belial, Lilith only */
	if (mon->mtyp == PM_BELIAL || mon->mtyp == PM_MOTHER_LILITH)
		sedu_knife(mon);

	/* handle payment -- only foocubi */
	if (mon->mtyp == PM_SUCCUBUS || mon->mtyp == PM_INCUBUS)
		sedu_payment(mon);

	/* worn out -- set mspec used */
	sedu_wornout(mon, badeffect);

	/* possibly exit early, skipping teleport and continuing to make attacks! */
	if (badeffect && (mon->mtyp == PM_BELIAL || mon->mtyp == PM_MOTHER_LILITH))
		return 0;

	/* teleport */
	if (!tele_restrict(mon)) (void) rloc(mon, TRUE);

	return 1;
}

/* monster that uses this is still unwritten
 * and so this hasn't been totally integrated into
 * the reworked combined seduce */
int
dosflseduce(mon)
register struct monst *mon;
{
	boolean fem = TRUE;
	//char qbuf[QBUFSZ];
	boolean helpless = FALSE;
	if (mon->mcan || mon->mspec_used) {
		pline("%s is uninterested in you.", Monnam(mon));
		return 0;
	}

	if (unconscious()) {
			You("are having a horrible dream.");
			helpless = TRUE;
	}

	if (Blind) You_feel("Something grab you...");
	else pline("%s grabs you.", mon_nam(mon));

	sedu_undress(mon);

	if (uarm || uarmc || (uwep && uwep->oartifact==ART_TENSA_ZANGETSU)) {
		verbalize("You can't resist forever!");
		pline("She claws at you!");
		losehp(d(4, 10), "a jilted paramour", KILLED_BY);
		return 0;
	}
	if (u.ualign.type == A_CHAOTIC)
		adjalign(1);
	/* by this point you have discovered mon's identity, blind or not... */
	pline("Time stands still while you and %s lie in each other's arms...",
		noit_mon_nam(mon));
	pline("Suddenly, %s becomes violent!",
		noit_Monnam(mon));
	if (helpless || (25 + rn2(100)) > (ACURR(A_CHA) + ACURR(A_STR))) {
		int turns = d(1, 4);
		if(!helpless) You("are taken off guard!");
		nomul(-(turns), "knocked reeling by a violent lover");
		nomovemsg = You_can_move_again;
		mon->mspec_used = turns;
		return 0;
	} else {
		pline("But you gain the upper hand!");
	    mon->mcanmove = 0;
	    mon->mfrozen = d(1,4)+1;
	    return 3;
	}
	return 0;
}

int
dopaleseduce(mon)
register struct monst *mon;
{
	register struct obj *ring, *nring;
	boolean fem = !poly_gender(); /* male = 0, fem = 1, neuter = 2 */
	boolean helpless = FALSE;
	char qbuf[QBUFSZ];
	
	if(!monnear(mon, u.ux,u.uy)) return 0;
	
	if((ward_at(u.ux,u.uy) == ELDER_SIGN && num_wards_at(u.ux, u.uy) == 6) || 
		(ward_at(u.ux,u.uy) == ELDER_ELEMENTAL_EYE && num_wards_at(u.ux, u.uy) == 7) || 
		ward_at(u.ux,u.uy) == PENTAGRAM || ward_at(u.ux,u.uy) == HEPTAGRAM ||
		ward_at(u.ux,u.uy) == HEXAGRAM || 
		(scaryElb(mon) && sengr_at("Elbereth", u.ux, u.uy)) || 
		(scaryLol(mon) && sengr_at("Lolth", u.ux, u.uy))
	) return 0;
	
	if (unconscious()) {/*Note: is probably not going to be possible to be unconscious and enter this function*/
		You("are having a horrible dream.");
		boolean helpless = TRUE;
	}

	if(mon->mvar_paleWarning == 1){
		if (Blind) You_feel("cloth against your %s...",body_part(BODY_SKIN));
		else{
			pline("The shroud dances as if in the wind. The %s figure beneath is almost exposed!", fem ? "shapely feminine" : "shapely masculine");
			You_feel("very attracted to %s.", mon_nam(mon));
		}
	}
	else{
		mon->mvar_paleWarning = 1;
		if (Blind) You_feel("the brush of cloth...");
		else{
			You("see a %s form behind the shroud. It beckons you forwards.", fem ? "lithe, feminine," : "toned, masculine,");
			if(rnd(10) + ACURR(A_WIS) - 10 > 6) You_feel("that it would be wise to stay away.");
		}
		return 0;
	}

	if (rn2(66) > 2*ACURR(A_WIS) - ACURR(A_INT) || helpless) {
		int lifesaved = 0;
		int wdmg = (int)(d(1,10)) + 1;
		
		sedu_undress(mon);

		if(rn2( (int)(ACURR(A_WIS)/2))){
			boolean loopingDeath = TRUE;
			while(loopingDeath) {
				boolean has_lifesaving = Lifesaved;
				if (lifesaved){
					pline("There is something horrible lurking in your memory... the mere thought of it is consuming your mind from within!");
				}
				else{
					pline("As you pass through the shroud, your every sense goes mad.");
					Your("whole world becomes an unbearable symphony of agony.");
				}
				killer = "seeing something not meant for mortal eyes";
				killer_format = KILLED_BY;
				done(DIED);
				lifesaved++;
				/* avoid looping on "die(y/n)?" */
				if (lifesaved && (discover || wizard || has_lifesaving)) {
					if (has_lifesaving) {
						/* used up AMULET_OF_LIFE_SAVING; still
						   subject to dying from memory */
						if(rn2( (int)(ACURR(A_WIS)/2)) < 4) loopingDeath = FALSE;
					} else {
						/* explicitly chose not to die */
						loopingDeath = FALSE;
					}
				}
			}
		}
		You("find yourself staggering away from %s, with no memory of why.", fem ? "her" : "him");
		make_stunned(HStun + 12, TRUE);
		while( ABASE(A_WIS) > ATTRMIN(A_WIS) && wdmg > 0){
			wdmg--;
			(void) adjattrib(A_WIS, -1, TRUE);
			exercise(A_WIS, FALSE);
		}
		if(u.sealsActive&SEAL_HUGINN_MUNINN){
			unbind(SEAL_HUGINN_MUNINN,TRUE);
		} else {
			if(AMAX(A_WIS) > ABASE(A_WIS)) AMAX(A_WIS) = (int)((AMAX(A_WIS) - ABASE(A_WIS))/2 + 1); //permanently drain wisdom
			forget(25);	/* lose 25% of memory */
		}
	} else {
		You("hang back from the %s form beneath the shroud. It poses enticingly.", fem ? "voluptuous feminine" : "muscular masculine");
	}
	return 1;
}

int
dotent(mon,dmg)
register struct monst *mon;
int dmg;
{
	char buf[BUFSZ];
	register struct obj *otmp;
	int i; //multipurpose local variable
	int n, ln, trycount; //loop control variables for attacks;
	int allreadydone = 0; //don't repeat the same special case;
	struct attack bodyblow = {AT_TENT, AD_WRAP, 2, 10};
	struct attack headshot = {AT_TENT, AD_DRIN, 2, 10};
	struct attack handshit = {AT_TENT, AD_DRDX, 2, 10};
	struct attack legblast = {AT_TENT, AD_LEGS, 2, 10};

	n = 4; //4 actions
	ln = n;
	trycount = 0;

	if(check_res_engine(&youmonst, AD_TENT)){
		if(canspotmon(mon))
			pline("The tentacles squirm against the forcefield.");
		if(check_res_engine(&youmonst, AD_TENT)){
			if(canspotmon(mon))
				pline("The field holds!");
			return 1;
		}
		else {
			if(canspotmon(mon))
				pline("The field fails!");
			n = 1; //2 actions
			ln = n;
		}
	}
/* First it makes one attempt to remove body armor.  It starts with the cloak,
 * followed by body armor and then the shirt.  It can only remove one per round.
 * After attempting to remove armor, it chooses random targets for the rest of its attacks.
 * These attacks are likely to be useless untill it gets rid of some armor.
 */
		if(!uarmc && !uarm && (!uwep || uwep->oartifact!=ART_TENSA_ZANGETSU)){
			if(uarmu && n){
				n--;
				if(!slips_free(mon, &youmonst,  &bodyblow, -1)){
					You_feel("the tentacles squirm under your underclothes.");
					if( d(1,100) > 15){
						if(!Preservation){
							pline("The tentacles begin to tear at your underclothes!");
							 if(uarmu->spe > 1){
								for(i=rn2(4); i>=0; i--)
									drain_item(uarmu);
								Your("%s less effective.", aobjnam(uarmu, "seem"));
							 }
							 else{
								tent_destroy_arm(uarmu);
								if(!uarmu) change_usanity(u_sanity_loss_minor(mon)/2, FALSE); /*Forces a san check*/
							 }
						}
					}
					else{
						pline("The tentacles pull your underclothes off!");
						otmp = uarmu;
						if (donning(otmp)) cancel_don();
						(void) Shirt_off();
						freeinv(otmp);
						(void) mpickobj(mon,otmp);
						change_usanity(u_sanity_loss_minor(mon)/2, FALSE); /*Forces a san check*/
						if(roll_madness(MAD_TALONS)){
							You("panic after having your underclothes pulled off!");
							HPanicking += 1+rnd(6);
						}
					}
				}
			}
		}
		if(!uarmc){
		 if(uwep && uwep->oartifact==ART_TENSA_ZANGETSU){
			n--;
			You_feel("the tentacles tear uselessly at your regenerating shihakusho.");
		 }
		 else if(uarm && n){
			 n--;
			 if(!slips_free(mon, &youmonst,  &bodyblow, -1)){
				You_feel("the tentacles squirm under your armor.");
				if( d(1,100) > 25){
					if(!Preservation){
						pline("The tentacles begin to tear at your armor!");
						if(uarm->spe > 1){
						for(i=rn2(4); i>=0; i--)
							drain_item(uarm);
							Your("%s less effective.", aobjnam(uarm, "seem"));
						}
						else{
							tent_destroy_arm(uarm);
							if(!uarm) change_usanity(u_sanity_loss_minor(mon)/2, FALSE); /*Forces a san check*/
						}
					}
				}
				else{
					pline("The tentacles shuck you out of your armor!");
					otmp = uarm;
					if (donning(otmp)) cancel_don();
					(void) Armor_gone();
					freeinv(otmp);
					(void) mpickobj(mon,otmp);
					change_usanity(u_sanity_loss_minor(mon)/2, FALSE); /*Forces a san check*/
					if(roll_madness(MAD_TALONS)){
						You("panic after having your armor removed!");
						HPanicking += 1+rnd(6);
					}
				}
		  }
		 }
		}
		if(uarmc && n){
			n--;
			if(!slips_free(mon, &youmonst,  &bodyblow, -1)){
				You_feel("the tentacles work their way under your cloak.");
				if( d(1,100) > 66){
					if(!Preservation){
						pline("The tentacles begin to tear at the cloak!");
						if(uarmc->spe > 1){
							for(i=rn2(4); i>=0; i--)
								drain_item(uarmc);
							Your("%s less effective.", aobjnam(uarmc, "seem"));
						}
						else{
							tent_destroy_arm(uarmc);
						}
					}
				}
				else{
					pline("The tentacles strip off your cloak!");
					otmp = uarmc;
					if (donning(otmp)) cancel_don();
					(void) Cloak_off();
					freeinv(otmp);
					(void) mpickobj(mon,otmp);
					if(roll_madness(MAD_TALONS)){
						You("panic after having your cloak taken!");
						HPanicking += 1+rnd(6);
					}
				}
			}
		}
		while(n > 0 && trycount++ < 50){
		   switch(d(1,12)){
			case 1:
			if(allreadydone&(0x1<<1)) break;
			allreadydone |= 0x1<<1;
			if(uarmf){
				n--;
				if(!slips_free(mon, &youmonst,  &legblast, -1)){
					You_feel("the tentacles squirm into your boots.");
					if( d(1,100) > 66){
						if(!Preservation){
							pline("The tentacles begin to tear at your boots!");
							if(uarmf->spe > 1){
								for(i=rn2(4); i>=0; i--)
									drain_item(uarmf);
								Your("%s less effective.", aobjnam(uarmf, "seem"));
							}
							else{
								tent_destroy_arm(uarmf);
							}
						}
					}
					else{
						if (uarmf->otyp != find_bboots()){
							pline("The tentacles suck off your boots!");
							otmp = uarmf;
							if (donning(otmp)) cancel_don();
							(void) Boots_off();
							freeinv(otmp);
							(void) mpickobj(mon,otmp);
							if(roll_madness(MAD_TALONS)){
								You("panic after having your boots sucked off!");
								HPanicking += 1+rnd(6);
							}
						}
					}
				}
			}
			break;
			case 2:
			if(allreadydone&(0x1<<2)) break;
			allreadydone |= 0x1<<2;
			if(uwep){
				n--;
				You_feel("the tentacles wrap around your weapon.");
				if( d(1,50) > ACURR(A_STR)){
					pline("The tentacles yank your weapon out of your grasp!");
					otmp = uwep;
					uwepgone();
					freeinv(otmp);
					(void) mpickobj(mon,otmp);
					if(roll_madness(MAD_TALONS)){
						You("panic after having your weapon taken!");
						HPanicking += 1+rnd(6);
					}
				 }
				 else{
					You("keep a tight grip on your weapon!");
				 }
			}
			break;
			case 3:
			if(allreadydone&(0x1<<3)) break;
			allreadydone |= 0x1<<3;
			if(uarmg){
				n--;
				if(!slips_free(mon, &youmonst,  &handshit, -1)){
					You_feel("the tentacles squirm into your gloves.");
					if( (d(1,30) <= ACURR(A_STR) || uwep)){
						if(!Preservation){
							pline("The tentacles begin to tear at your gloves!");
							if(uarmg->spe > 1){
								for(i=rn2(4); i>=0; i--)
									drain_item(uarmg);
								Your("%s less effective.", aobjnam(uarmg, "seem"));
							}
							else{
								tent_destroy_arm(uarmg);
							}
						}
					}
					else{
						pline("The tentacles suck your gloves off!");
						otmp = uarmg;
						if (donning(otmp)) cancel_don();
						(void) Gloves_off();
						freeinv(otmp);
						(void) mpickobj(mon,otmp);
						if(roll_madness(MAD_TALONS)){
							You("panic after having your gloves sucked off!");
							HPanicking += 1+rnd(6);
						}
					}
				}
			}
			break;
			case 4:
			if(allreadydone&(0x1<<4)) break;
			allreadydone |= 0x1<<4;
			if(uarms){
				n--;
				You_feel("the tentacles wrap around your shield.");
				if( d(1,80) > ACURR(A_STR)){
					pline("The tentacles pull your shield out of your grasp!");
					otmp = uarms;
					if (donning(otmp)) cancel_don();
					Shield_off();
					freeinv(otmp);
					(void) mpickobj(mon,otmp);
					if(roll_madness(MAD_TALONS)){
						You("panic after having your shield taken!");
						HPanicking += 1+rnd(6);
					}
				 }
				 else{
					You("keep a tight grip on your shield!");
				 }
			}
			break;
			case 5:
			if(allreadydone&(0x1<<5)) break;
			allreadydone |= 0x1<<5;
			if(uarmh){
				n--;
				if(!slips_free(mon, &youmonst,  &headshot, -1)){
					You_feel("the tentacles squirm under your helmet.");
					if( d(1,100) > 90){
						if(!Preservation){
							pline("The tentacles begin to tear at your helmet!");
							 if(uarmh->spe > 1){
								for(i=rn2(4); i>=0; i--)
									drain_item(uarmh);
								Your("%s less effective.", aobjnam(uarmh, "seem"));
							 }
							 else{
								tent_destroy_arm(uarmh);
								if(!uarmh) change_usanity(u_sanity_loss_minor(mon)/2, FALSE); /*Forces a san check*/
							 }
						}
					}
					else{
						pline("The tentacles pull your helmet off!");
						otmp = uarmh;
						if (donning(otmp)) cancel_don();
						(void) Helmet_off();
						freeinv(otmp);
						(void) mpickobj(mon,otmp);
						change_usanity(u_sanity_loss_minor(mon)/2, FALSE); /*Forces a san check*/
						if(roll_madness(MAD_TALONS)){
							You("panic after having your helmet stolen!");
							HPanicking += 1+rnd(6);
						}
					}
				}
			}
			break;
			case 6:
				if(u.uenmax == 0) 
			break;
				if(allreadydone&(0x1<<6)) break;
				allreadydone |= 0x1<<6;
				n--; //else commit to the attack.
				if(uarmc || uarm || uarmu || (uwep && uwep->oartifact==ART_TENSA_ZANGETSU)){
					You_feel("the tentacles sucking on your %s", uarm ? "armor" : "clothes");
			break;  //blocked
				} //else
				You_feel("little mouths sucking on your exposed %s.",body_part(BODY_SKIN));
				u.uen = 0;
				if(Half_physical_damage) u.uenbonus -= (int) max(.1*u.uenmax,5);
				else u.uenbonus -= (int) max(.2*u.uenmax, 10);
				calc_total_maxen();
				change_usanity(u_sanity_loss_minor(mon), FALSE); /*Forces a san check*/
			break;
			case 7:
				if(allreadydone&(0x1<<7)) break;
				allreadydone |= 0x1<<7;
				n--;
				if(uarmh){
					You_feel("the tentacles squirm over your helmet");
			break; //blocked
				} //else
				You_feel("the tentacles bore into your skull!");
				i = d(1,6);
				if(u.sealsActive&SEAL_HUGINN_MUNINN){
					unbind(SEAL_HUGINN_MUNINN,TRUE);
				} else {
					(void) adjattrib(A_INT, -i, 1);
					while(i-- > 0){
						if(i%2) losexp("brain damage",FALSE,TRUE,FALSE);
						forget(10);	/* lose 10% of memory per point lost*/
						exercise(A_WIS, FALSE);
					}
				}
				//begin moved brain removal messages
				Your("brain is cored like an apple!");
				if (ABASE(A_INT) <= 3) {
					int lifesaved = 0;
					struct obj *wore_amulet = uamul;
					while(1) {
							/* avoid looping on "die(y/n)?" */
							if (lifesaved && (discover || wizard)) {
								if (wore_amulet && !uamul) {
									/* used up AMULET_OF_LIFE_SAVING; still
									   subject to dying from brainlessness */
									wore_amulet = 0;
								} else {
									/* explicitly chose not to die;
									   arbitrarily boost intelligence */
									ABASE(A_INT) = ATTRMIN(A_INT) + 2;
									You_feel("like a scarecrow.");
									break;
								}
							}
						if (lifesaved)
							pline("Unfortunately your brain is still gone.");
						else
							Your("last thought fades away.");
						killer = "destruction of the brain and spinal cord";
						killer_format = KILLED_BY;
						done(DIED);
						lifesaved++;
					}
				}
				losehp(Half_physical_damage ? dmg/2 + 1 : dmg, "head trauma", KILLED_BY);
				change_usanity(u_sanity_loss_minor(mon), TRUE); /*Forces a san check*/
			break;
			case 8:
				if(allreadydone&(0x1<<8)) break;
				allreadydone |= 0x1<<8;
				n--;
				if(uarmc || uarm || (uwep && uwep->oartifact==ART_TENSA_ZANGETSU)){
					You_feel("a tentacle squirm over your %s.", uarmc ? "cloak" : (uwep && uwep->oartifact==ART_TENSA_ZANGETSU) ? "shihakusho" : "armor");
			break;  //blocked
				} //else
				You_feel("the tentacles drill through your unprotected %s and into your soul!",body_part(BODY_FLESH));
				if (!Drain_resistance) {
					losexp("soul-shredding tentacles",FALSE,FALSE,FALSE);
					losexp("soul-shredding tentacles",FALSE,FALSE,FALSE);
					losexp("soul-shredding tentacles",FALSE,FALSE,FALSE);
					i = d(1,4);
					while(i-- > 0){
						losexp("soul-shredding tentacles",FALSE,FALSE,TRUE);
						exercise(A_WIS, FALSE);
						exercise(A_WIS, FALSE);
						exercise(A_WIS, FALSE);
					}
					(void) adjattrib(A_CON, -4, 1);
					You_feel("violated and very fragile. Your soul seems a thin and tattered thing.");
				} else {
					(void) adjattrib(A_CON, -2, 1);
					You_feel("a bit fragile, but strangely whole.");
				}
				losehp(Half_physical_damage ? dmg/4+1 : dmg/2+1, "drilling tentacles", KILLED_BY);
				change_usanity(u_sanity_loss_minor(mon), FALSE); /*Forces a san check*/
			break;
			case 9:
				if(allreadydone&(0x1<<9)) break;
				allreadydone |= 0x1<<9;
				n--;
				if(uarmc || uarm  || (uwep && uwep->oartifact==ART_TENSA_ZANGETSU)){
					You_feel("the tentacles press into your %s", uarmc ? "cloak" : (uwep && uwep->oartifact==ART_TENSA_ZANGETSU) ? "shihakusho" : "armor");
			break; //blocked
				} //else
				You_feel("the tentacles spear into your unarmored body!");
				losehp(Half_physical_damage ? dmg : 4*dmg, "impaled by tentacles", NO_KILLER_PREFIX);
				(void) adjattrib(A_STR, -6, 1);
				(void) adjattrib(A_CON, -3, 1);
				You_feel("weak and helpless in their grip!");
				change_usanity(u_sanity_loss_minor(mon), FALSE); /*Forces a san check*/
			break;
			case 10:
				if(allreadydone&(0x1<<10)) break;
				allreadydone |= 0x1<<10;
			case 11:
				if(allreadydone&(0x1<<11)) break;
				allreadydone |= 0x1<<11;
			case 12:
				// if(allreadydone&(0x1<<12)) break; //earth any remaining attempts
				// allreadydone |= 0x1<<12;
				if(uarmc || (uwep && uwep->oartifact==ART_TENSA_ZANGETSU)) {
					n--;//while you have your cloak, this burns attacks at a high rate.
					You_feel("the tentacles writhe over your %s.", uarmc ? "cloak" : "shihakusho");
			break;
				} //else
				if(invent && !uarmc && !uarm && !uarmu && !uarmf && !uarmg && !uarms && !uarmh && !uwep
					){ //only steal if you have at least one item and everything else of interest is already gone.
					n = 0;
					You_feel("the tentacles pick through your remaining possessions.");
					for(int i = 0; i<4; i++){
						buf[0] = '\0';
						steal(mon, buf,FALSE, FALSE);
						/*The elder priest is petrification immune, but if the attacker somehow dies from the theft we should return.*/
						if(DEADMONSTER(mon))
							return 1;
					}
				}
			break;
		   }
		}
	if(roll_madness(MAD_HELMINTHOPHOBIA)){
		You("panic from the squirming tentacles!");
		HPanicking += 1+rnd(6);
	}
	return 1;
}

STATIC_OVL void
mayberem(obj, str, helpless)
register struct obj *obj;
const char *str;
boolean helpless;
{
	mayberem_common(obj, str, !(rn2(20) < (ACURR(A_CHA) + (check_mutation(TENDRIL_HAIR) ? 10 : 0))));
}

STATIC_OVL void
lrdmayberem(obj, str, helpless)
register struct obj *obj;
const char *str;
boolean helpless;
{
	mayberem_common(obj, str, !(rn2(60) < (ACURR(A_CHA) + (check_mutation(TENDRIL_HAIR) ? 30 : 0))));
}

STATIC_OVL void
mlcmayberem(obj, str, helpless)
register struct obj *obj;
const char *str;
boolean helpless;
{
	mayberem_common(obj, str, helpless || !(rn2(60) < (ACURR(A_CHA) + (check_mutation(TENDRIL_HAIR) ? 30 : 0))));
}

STATIC_OVL void
mayberem_common(obj, str, dontask)
register struct obj *obj;
const char *str;
boolean dontask;
{
	char qbuf[QBUFSZ];

	if (!obj || !obj->owornmask) return;

	if (!dontask) {
		Sprintf(qbuf,"\"Shall I remove your %s, %s?\"",
			str,
			(!rn2(2) ? "lover" : !rn2(2) ? "dear" : "sweetheart"));
		if (yn(qbuf) == 'n') return;
	} else {
		char hairbuf[BUFSZ];

		Sprintf(hairbuf, "let me run my fingers through your %s",
			body_part(HAIR));
		verbalize("Take off your %s; %s.", str,
			(obj == uarm)  ? "let's get a little closer" :
			(obj == uarmc || obj == uarms) ? "it's in the way" :
			(obj == uarmf) ? "let me rub your feet" :
			(obj == uarmg) ? "they're too clumsy" :
#ifdef TOURIST
			(obj == uarmu) ? "let me massage you" :
#endif
			/* obj == uarmh */
			hairbuf);
	}
	remove_worn_item(obj, TRUE);
}

STATIC_OVL void
sflmayberem(obj, str, helpless)
register struct obj *obj;
const char *str;
boolean helpless;
{
	char qbuf[QBUFSZ];
	int her_strength;

	if (!obj || !obj->owornmask) return;
	
	her_strength = 25 + rn2(100);
	if (!helpless && her_strength < ACURR(A_STR)) {
		Sprintf(qbuf,"She tries to take off your %s, allow her?",
			str);
		if (yn(qbuf) == 'n') return;
	} else if(her_strength > ACURR(A_STR)*2){
		Sprintf(qbuf,"She tries to rip open your %s!",
			str);
		her_strength -= ACURR(A_STR);
		if(Preservation){
			pline("But, no harm is done!");
		} 
		else for(; her_strength >= 0; her_strength--){
			if(obj->spe > -1*a_acdr(objects[(obj)->otyp])){
				damage_item(obj);
//				Your("%s less effective.", aobjnam(obj, "seem"));
			}
			else if(!obj->oartifact){
				claws_destroy_arm(obj);
			}
			else{
				remove_worn_item(obj, TRUE);
			}
		}
		return;
	}
	remove_worn_item(obj, TRUE);
	Sprintf(qbuf,"She removes your %s!",
		str);
}

STATIC_OVL void
palemayberem(obj, str, helpless)
register struct obj *obj;
const char *str;
boolean helpless;
{
	char qbuf[QBUFSZ];
	int its_cha;

	if (!obj || !obj->owornmask) return;
	
	its_cha = rn2(40);
	if (helpless || its_cha >= ACURR(A_CHA)) {
		if(!obj->oartifact || !rn2(10)){
			destroy_arm(obj);
		}
	}
}


/* prints a message about the player being unconscious */
/* returns TRUE if seducer stops */
boolean
sedu_helpless(mon)
struct monst * mon;
{
	/* seducers that will return early */
	if (mon->mtyp == PM_INCUBUS || mon->mtyp == PM_SUCCUBUS) {
		pline("%s seems dismayed at your lack of response.", Monnam(mon));
		return 1;
	}
	else if (mon->mtyp == PM_MOTHER_LILITH || mon->mtyp == PM_BELIAL) {
		pline("%s seems bored by your lack of response.", Monnam(mon));
		return 1;
	}
	/* seducers that will continue */
	else if (mon->mtyp == PM_AVATAR_OF_LOLTH) {
		pline("%s seems pleased at your lack of response.", Monnam(mon));
	}
	else if (mon->mtyp == PM_GRAZ_ZT || mon->mtyp == PM_MALCANTHET) {
		You("are having a strange dream.");
	}
	/*
	else if (mon->mtyp == PM_SHAMI_AMOURAE) {
		You("are having a horrible dream.");
	}
	*/
	return 0;
}

/* returns 1 if mon tried teleporting after being refused, ending sedu */
/* returns 0 if monster should continue sedu, and give a bad effect */
int
sedu_refuse(mon)
struct monst * mon;
{
	switch(mon->mtyp) {
		case PM_AVATAR_OF_LOLTH:
				if(flags.female){
					verbalize("You're such a sweet lady, I wish you were more open to new things...");
				} else {
					verbalize("How dare you refuse me!");
					return 0; /* don't fall down to the general "teleport and return 1" case */
				}
			break;
		case PM_MALCANTHET:
				verbalize("How dare you refuse me!");
				pline("She claws at you!");
				losehp(d(4, 4), "a jilted paramour", KILLED_BY);
			break;
		case PM_GRAZ_ZT:
				verbalize("How dare you refuse me!");
				pline("He punches you!");
				losehp(d(3, 8), "an enraged paramour", KILLED_BY);
			break;
		/*
		case PM_SHAMI_AMOURAE:
				verbalize("You can't resist forever!");
				pline("She claws at you!");
				losehp(d(4, 10), "a jilted paramour", KILLED_BY);
			return 1; // don't attempt to teleport
		*/
		case PM_INCUBUS:
		case PM_SUCCUBUS:
		case PM_MOTHER_LILITH:
		case PM_BELIAL:
		default:
				verbalize("You're such a %s; I wish...",
					flags.female ? "sweet lady" : "nice guy");
				/* unbind Enki. (unless you refused a demon lord) */
				if (is_normal_demon(mon->data))
					if(u.sealsActive&SEAL_ENKI) unbind(SEAL_ENKI,TRUE);
			break;
	}
	
	if (!tele_restrict(mon)) (void) rloc(mon, TRUE);
	return 1;
}

/* returns TRUE if your roll doesn't beat the monster and you should get a bad sedu effect */
boolean
sedu_roll(mon, helpless)
struct monst * mon;
boolean helpless;
{
	switch(mon->mtyp) {
		case PM_AVATAR_OF_LOLTH:
			return (helpless || rn2(120) > ACURR(A_CHA) + ACURR(A_WIS));
		case PM_MOTHER_LILITH:
		case PM_BELIAL:
			return (Sterile || rn2(139) > ACURR(A_CHA) + ACURR(A_INT));
		case PM_MALCANTHET:
		case PM_GRAZ_ZT:
			return (Sterile || helpless || rn2(120) > ACURR(A_CHA) + ACURR(A_CON) + ACURR(A_INT));
		/*
		case PM_SHAMI_AMOURAE:
			return (helpless || (25 + rn2(100)) > (ACURR(A_CHA) + ACURR(A_STR)));
		*/
		case PM_INCUBUS:
		case PM_SUCCUBUS:
		default:
			return (Sterile || rn2(35) > ACURR(A_CHA) + ACURR(A_INT));
	}
}

void
sedu_payment(mon)
struct monst * mon;
{
	if (mon->mtame) {
		/* don't charge */
		return;
	}

	if (rn2(20) < ACURR(A_CHA)) {
		pline("%s demands that you pay %s, but you refuse...",
			noit_Monnam(mon),
			Blind ? (mon->female ? "her" : "him") : mhim(mon));
	} else if (u.umonnum == PM_LEPRECHAUN)
		pline("%s tries to take your money, but fails...",
				noit_Monnam(mon));
	else {
#ifndef GOLDOBJ
		long cost;

		if (u.ugold > (long)LARGEST_INT - 10L)
			cost = (long) rnd(LARGEST_INT) + 500L;
		else
			cost = (long) rnd((int)u.ugold + 10) + 500L;
		if (mon->mpeaceful) {
			cost /= 5L;
			if (!cost) cost = 1L;
		}
		if (cost > u.ugold) cost = u.ugold;
		if (!cost) verbalize("It's on the house!");
		else {
			pline("%s takes %ld %s for services rendered!",
				noit_Monnam(mon), cost, currency(cost));
			u.ugold -= cost;
			mon->mgold += cost;
			flags.botl = 1;
		}
#else
		long cost;
				long umoney = money_cnt(invent);

		if (umoney > (long)LARGEST_INT - 10L)
			cost = (long) rnd(LARGEST_INT) + 500L;
		else
			cost = (long) rnd((int)umoney + 10) + 500L;
		if (mon->mpeaceful) {
			cost /= 5L;
			if (!cost) cost = 1L;
		}
		if (cost > umoney) cost = umoney;
		if (!cost) verbalize("It's on the house!");
		else { 
			pline("%s takes %ld %s for services rendered!",
				noit_Monnam(mon), cost, currency(cost));
					money2mon(mon, cost);
			flags.botl = 1;
		}
#endif
	}
}

void
sedu_undress(mon)
struct monst * mon;
{
	/* check no-clothes case */
	if (!uarm && !uarmc && !uarmf && !uarmg && !uarms && !uarmh
#ifdef TOURIST
		&& !uarmu
#endif
		) {
		/* message */
		switch (mon->mtyp)
		{
			/*
		case PM_SHAMI_AMOURAE:
			if (canseemon(mon))
			pline("%s stares at you.",
				Monnam(mon));
			break;
			*/
		case PM_MALCANTHET:
		case PM_GRAZ_ZT:
			if (!Blind)	/* if Blind, we already got "It caresses you..." */
				pline("%s caresses your body.", Monnam(mon));
			break;

		case PM_PALE_NIGHT:
			You("move to embrace %s, brushing aside the gossamer shroud hiding %s body from you.",
				noit_Monnam(mon), hisherits(mon));
			break;

		default:
			pline("%s murmurs sweet nothings into your ear.",
				Blind ? (mon->female ? "She" : "He") : Monnam(mon));
			break;
		}
	}
	else {
		void FDECL((*undressfunc), (register struct obj *, const char *, boolean)) = 0;
		boolean helpless = unconscious();

		/* message and select correct function */
		switch (mon->mtyp)
		{
			/*
		case PM_SHAMI_AMOURAE:
			undressfunc = &dosflseduce;
			pline("%s growls into your ear, while tearing at your clothing.",
				Blind ? (mon->female ? "She" : "He") : Monnam(mon));
			break;
			*/
		case PM_MALCANTHET:
		case PM_GRAZ_ZT:
			undressfunc = (void*)&mlcmayberem;
			pline("%s starts undressing you.",
				Blind ? (mon->female ? "She" : "He") : Monnam(mon));
			break;

		case PM_PALE_NIGHT:
			undressfunc = (void*)&palemayberem;
			You("move to embrace %s, brushing aside the gossamer shroud hiding %s body from you.",
				noit_Monnam(mon), (poly_gender() ? "his" : "her"));	/* Pale Night's apparent gender is based on yours */
			break;

		case PM_AVATAR_OF_LOLTH:
		case PM_MOTHER_LILITH:
		case PM_BELIAL:
			undressfunc = (void*)&lrdmayberem;
			/* fall through to default message */
		default:
			if (!undressfunc)
				undressfunc = (void*)&mayberem;
			pline("%s murmurs in your ear, while helping you undress.",
				Blind ? (mon->female ? "She" : "He") : Monnam(mon));
			break;
		}
		/* undress player */
		undressfunc(uarmc, cloak_simple_name(uarmc), helpless);

		if (!uarmc)
			undressfunc(uarm, "suit", helpless);

		if (mon->mtyp != PM_GRAZ_ZT) /* his seduces can replace your boots */
			undressfunc(uarmf, "boots", helpless);

		if (!uwep || !welded(uwep))
			undressfunc(uarmg, "gloves", helpless);

		undressfunc(uarms, "shield", helpless);

		if (mon->mtyp != PM_GRAZ_ZT) /* his seduces can replace your hat */
			undressfunc(uarmh, "helmet", helpless);
#ifdef TOURIST
		if (!uarmc && !uarm)
			undressfunc(uarmu, "clothes", helpless);
#endif
	}

	return;
}

void
sedu_adornment_ring(mon)
struct monst * mon;
{
	struct obj * ring;
	struct obj * nring;
	char qbuf[BUFSZ];
	char buf[BUFSZ];

	boolean takesring;
	boolean resist;
	boolean found = FALSE;

	switch (mon->mtyp)
	{
	case PM_AVATAR_OF_LOLTH:
		takesring = FALSE;
		resist = (!unconscious() && rn2(40) < ACURR(A_CHA));
		break;
	case PM_MOTHER_LILITH:
		takesring = TRUE;
		resist = FALSE;
		break;
	case PM_BELIAL:
		takesring = TRUE;
		resist = FALSE;
		break;
	case PM_MALCANTHET:
		if (poly_gender() == 1) {
			/* ufem */
			takesring = TRUE;
			resist = (rn2(45) < ACURR(A_CHA));
		}
		else {
			takesring = FALSE;
			resist = FALSE;
		}
		break;
	case PM_GRAZ_ZT:
		if (poly_gender() == 0) {
			/* umal */
			takesring = TRUE;
			resist = (rn2(45) < ACURR(A_CHA));
		}
		else {
			takesring = FALSE;
			resist = FALSE;
		}
		break;
	default:
		takesring = !mon->female;
		resist = (rn2(20) < ACURR(A_CHA));
		break;
	}

	for (ring = invent; !found && ring; ring = nring) {
		nring = ring->nobj;
		if (ring->otyp != RIN_ADORNMENT) continue;

		/* adornment ring found */

		if (takesring) {
			found = TRUE;
			if (resist) {
				Sprintf(buf, "\"That %%s looks pretty.  %s\"",
					(mon->mtyp == PM_MALCANTHET || mon->mtyp == PM_GRAZ_ZT) ? "Give it to me." : "May I have it?");

				Sprintf(qbuf, buf,
					safe_qbuf("", strlen(buf),
					xname(ring), simple_typename(ring->otyp), "ring"));
				makeknown(RIN_ADORNMENT);
				if (yn(qbuf) == 'n') continue;
			}
			else {
				pline("%s decides %s'd like your %s, and takes it.",
					Blind ? (mon->female ? "She" : "He") : Monnam(mon),
					(mon->female ? "she" : "he"),
					xname(ring));
			}
			makeknown(RIN_ADORNMENT);
			if (ring == uleft || ring == uright) Ring_gone(ring);
			if (ring == uwep) setuwep((struct obj *)0);
			if (ring == uswapwep) setuswapwep((struct obj *)0);
			if (ring == uquiver) setuqwep((struct obj *)0);
			freeinv(ring);
			(void)mpickobj(mon, ring);
		}
		else {
			if (uleft && uright && uleft->otyp == RIN_ADORNMENT
				&& (uright->otyp == RIN_ADORNMENT || (uarmg && uarmg->oartifact == ART_CLAWS_OF_THE_REVENANCER)))
				break;
			if (ring == uleft || ring == uright) continue;
			found = TRUE;
			if (resist) {
				Sprintf(qbuf, "\"That %s looks pretty.  Would you wear it for me?\"",
					safe_qbuf("",
					sizeof("\"That  looks pretty.  Would you wear it for me?\""),
					xname(ring), simple_typename(ring->otyp), "ring"));
				makeknown(RIN_ADORNMENT);
				if (yn(qbuf) == 'n') continue;
			}
			else {
				pline("%s decides you'd look prettier wearing your %s,",
					Blind ? (mon->female ? "She" : "He") : Monnam(mon), xname(ring));
				pline("and puts it on your finger.");
			}
			makeknown(RIN_ADORNMENT);
			if (!uright && !(uarmg && uarmg->oartifact == ART_CLAWS_OF_THE_REVENANCER)) {
				pline("%s puts %s on your right %s.",
					Blind ? (mon->female ? "She" : "He") : Monnam(mon), the(xname(ring)), body_part(HAND));
				setworn(ring, RIGHT_RING);
			}
			else if (!uleft) {
				pline("%s puts %s on your left %s.",
					Blind ? (mon->female ? "She" : "He") : Monnam(mon), the(xname(ring)), body_part(HAND));
				setworn(ring, LEFT_RING);
			}
			else if (uright && uright->otyp != RIN_ADORNMENT && !(uarmg && uarmg->oartifact == ART_CLAWS_OF_THE_REVENANCER)) {
				Strcpy(buf, xname(uright));
				pline("%s replaces your %s with your %s.",
					Blind ? (mon->female ? "She" : "He") : Monnam(mon), buf, xname(ring));
				Ring_gone(uright);
				setworn(ring, RIGHT_RING);
			}
			else if (uleft && uleft->otyp != RIN_ADORNMENT) {
				Strcpy(buf, xname(uleft));
				pline("%s replaces your %s with your %s.",
					Blind ? (mon->female ? "She" : "He") : Monnam(mon), buf, xname(ring));
				Ring_gone(uleft);
				setworn(ring, LEFT_RING);
			}
			else impossible("ring replacement");
			Ring_on(ring);
			prinv((char *)0, ring, 0L);
		}
	}
	return;
}

/* some monsters try to knife you, how rude */
void
sedu_knife(mon)
struct monst * mon;
{
	const char * knife = uclockwork ? "knife to the gears" : "knife to the ribs";
	if (ACURR(A_CHA) + rn1(4, 3) < 24){
		pline("Before you can get up, %s slips a knife %s!",
			noit_Monnam(mon), uclockwork ? "into your gears" : "between your ribs");
		if (Half_physical_damage) losehp(rn1(5, 6), knife, KILLED_BY);
		else losehp(rn1(10, 6), knife, KILLED_BY);
	}
	else{
		pline("As you get up, %s tries to knife you, but is too distracted to do it properly!",
		noit_Monnam(mon));
		if (Half_physical_damage) losehp(rn1(1, 6), knife, KILLED_BY);
		else losehp(rn1(2, 6), knife, KILLED_BY);
	}
	return;
}

/* sets mspecused */
void
sedu_wornout(mon, badeffect)
struct monst * mon;
boolean badeffect;
{
	switch(mon->mtyp)
	{
		case PM_BELIAL:
		case PM_MOTHER_LILITH:
			if (!badeffect)
				mon->mspec_used = rnd(39) + 13;
			else
				mon->mspec_used = rnd(13) + 3;
			break;

		case PM_MALCANTHET:
		case PM_GRAZ_ZT:
			if (!badeffect)
				mon->mspec_used = rnd(39)+13;
			break;

		case PM_AVATAR_OF_LOLTH:
			if (!badeffect)
				mon->mspec_used = rnd(100);
			break;
		default:
			if (!badeffect)
				mon->mspec_used = rnd(100);
			if (!rn2(25))
				set_mcan(mon, TRUE);
			break;
	}
	return;
}

/* prints a message */
void
sedu_timestandsstill(mon, badeffect)
struct monst * mon;
boolean badeffect;
{

	/* lolth transforms */
	if (mon->mtyp == PM_AVATAR_OF_LOLTH) {
		if (badeffect) {
			if (Blind) You("suddenly find yourself in the arms of a giant spider!");
			else pline("She suddenly becomes a giant spider and seizes you with her legs!");
		}
		else {
			if(Blind) pline("An elf-maid clasps herself to you!");
			else pline("She becomes a beautiful dark-skinned elf-maid!");
		}
	}
	else {
		/* line 1 */
		if(!uclockwork){
			pline("Time stands still while you and %s lie in each other's arms...",
				noit_mon_nam(mon));
		}
		else{
			pline("You and %s lie down together...",
				noit_mon_nam(mon));
		}
		/* line 2 */
		if (badeffect) {
			if (uclockwork) {
				pline("%s looks briefly confused...",
					noit_Monnam(mon));
			}
		}
		else {
			if (uclockwork)
				pline("Time stands still while you and %s lie in each other's arms...",
					noit_mon_nam(mon));
			else
				You("seem to have enjoyed it more than %s...",
					noit_mon_nam(mon));
		}
	}
}

int
sedu_select_effect(mon, badeffect)
struct monst * mon;
boolean badeffect;
{
	if (badeffect) {
		switch(mon->mtyp) {
			case PM_AVATAR_OF_LOLTH:
				return SEDU_LOLTHATTACK;
			case PM_MALCANTHET:
				if(uclockwork)
					return SEDU_DAMAGECLK;
				switch(rn2(8)){
					case 0: return SEDU_DRAINEN;
					case 1: return SEDU_DUMPS;
					case 2: return SEDU_BURDEN;
					case 3: return SEDU_DULLSENSES;
					case 4: return SEDU_EXHAUSTED;
					case 5: return SEDU_CURSE;
					case 6: return SEDU_GREMLIN;
					case 7: return SEDU_PARALYZE;
				}
			case PM_GRAZ_ZT:
				if(uclockwork)
					return SEDU_DAMAGECLK;
				switch(rn2(6)){
					case 0: return SEDU_STEALSIX;
					case 1: return SEDU_BADWEAP;
					case 2: return SEDU_BADHAT;
					case 3: return SEDU_BADBOOTS;
					case 4: return SEDU_BADAMU;
					case 5: return SEDU_PUNISH;
				}
			case PM_MOTHER_LILITH:
			case PM_BELIAL:
			case PM_INCUBUS:
			case PM_SUCCUBUS:
			default:
				if(uclockwork) {
					if (!rn2(5) && !Drain_resistance)
						return SEDU_SUCKSOUL;
					else
						return SEDU_STEALONE;
				}
				switch(rn2(5)){
					case 0: return SEDU_DRAINEN;
					case 1: return SEDU_DUMPS;
					case 2: return SEDU_DULLSENSES;
					case 3: return SEDU_DRAINLVL;
					case 4: return SEDU_EXHAUSTED;
				}
		}
	}
	else {
		switch(mon->mtyp) {
			case PM_AVATAR_OF_LOLTH:
					switch(rn2(4)){
						case 0: return SEDU_WISH;
						case 1: return SEDU_BLESS;
						case 2: return SEDU_EDUCATE;
						case 3: return SEDU_RESTOREHP;
					}
			case PM_MALCANTHET:
				if(poly_gender() == 1 && (ACURR(A_CHA) < rn2(35)))
					return SEDU_JEALOUS;	// bad effect
				switch(uclockwork ? rn2(4) : rn2(8)){
					case 0: return SEDU_PROTECT;
					case 1: return SEDU_BLESS;
					case 2: return SEDU_LIFEFONT;
					case 3: return SEDU_CARRYCAP;
					case 4: return SEDU_POISRES;
					case 5: return SEDU_ACIDRES;
					case 6: return SEDU_SICKRES;
					case 7: return SEDU_RAISESTATS;
				}
			case PM_GRAZ_ZT:
				if(poly_gender() == 0 && (ACURR(A_CHA) < rn2(35)))
					return SEDU_JEALOUS;	// bad effect
				switch(rn2(6)){
					case 0: return SEDU_WISH;
					case 1: return SEDU_GENOCIDE;
					case 2: return SEDU_SIXMAGICS;
					case 3: return SEDU_SIXTRUTHS;
					case 4: return SEDU_SIXFOLLOWERS;
					case 5: return SEDU_LIFESAVING;
				}
			case PM_MOTHER_LILITH:
			case PM_BELIAL:
			case PM_INCUBUS:
			case PM_SUCCUBUS:
			default:
				if (uclockwork) {
					if (!rn2(5))
						return SEDU_EDUCATE;
					else if (u.uhunger < .5*get_uhungermax() && !Race_if(PM_INCANTIFIER))
						return SEDU_WIND;
					else
						return SEDU_NOTHING;
				}
				switch(rn2(5)){
					case 0: return SEDU_GAINEN;
					case 1: return SEDU_GOODENOUGH;
					case 2: return SEDU_REMEMBER;
					case 3: return SEDU_EDUCATE;
					case 4: return SEDU_RESTOREHP;
				}
		}
	}
	/* should not be reached */
	return 0;
}

void
seduce_effect(mon, effect_num)
struct monst * mon;
int effect_num;
{
	char qbuf[QBUFSZ];
	struct obj *key;
	int turns = 0;
	char class_list[MAXOCLASSES + 2];
	int tmp;
	char buf[BUFSZ];
	struct obj * optr;
	const char * s;

	boolean greater = /* lilith/belial seduce */
		(mon->mtyp == PM_MOTHER_LILITH || mon->mtyp == PM_BELIAL);
	boolean greatest = /* malcanthet/graz'zt seduce */
		(mon->mtyp == PM_MALCANTHET || mon->mtyp == PM_GRAZ_ZT);

	if (effect_num == 0) {
		/* all effects, if any, were to be handled elsewhere */
		/* probably you were a clockwork */
		return;
	}
	else if (effect_num < 0)
	{
		switch (effect_num)
		{
		case SEDU_DRAINEN:
			You_feel("drained of energy.");
			if (u.uen > 0)
				losepw(u.uen * 99 / 100);
			tmp = (greater || greatest ? 90 : 10) / (Half_physical_damage ? 2 : 1);
			u.uenbonus -= rnd(tmp);
			exercise(A_CON, FALSE);
			calc_total_maxen();
			break;
		case SEDU_DUMPS:
			You("are down in the dumps.");
			if (greatest) {
				u.uhpmod -= Half_physical_damage ? 25 : 50;
				calc_total_maxhp();
				(void)adjattrib(A_CON, -2, TRUE);
				(void)adjattrib(A_STR, -2, TRUE);
				if (diseasemu(mon->data))
					You("seem to have caught a disease!");
			}
			else if (greater) {
				(void)adjattrib(A_CON, -6, TRUE);
				(void)adjattrib(A_WIS, -3, TRUE);
				exercise(A_CON, FALSE);
			}
			else {
				(void)adjattrib(A_CON, -1, TRUE);
				exercise(A_CON, FALSE);
			}
			flags.botl = 1;
			break;
		case SEDU_DULLSENSES:
			s = greatest ? "mind" : "senses";
			Your("%s %s dulled.", s, vtense(s, "are"));
			if (greatest) {
				if (u.sealsActive&SEAL_HUGINN_MUNINN){
					unbind(SEAL_HUGINN_MUNINN, TRUE);
				}
				else {
					(void)adjattrib(A_INT, -3, TRUE);
					(void)adjattrib(A_WIS, -3, TRUE);
					forget(30);
					exercise(A_WIS, FALSE);
					exercise(A_WIS, FALSE);
					exercise(A_WIS, FALSE);
				}
			}
			else if (greater) {
				(void)adjattrib(A_WIS, -9, TRUE);
				exercise(A_WIS, FALSE);
				exercise(A_WIS, FALSE);
				exercise(A_WIS, FALSE);
			}
			else {
				(void)adjattrib(A_WIS, -1, TRUE);
				exercise(A_WIS, FALSE);
			}
			flags.botl = 1;
			break;
		case SEDU_DRAINLVL:
			if (!Drain_resistance) {
				You_feel("out of shape.");
				if (greater) {
					losexp("overexertion", FALSE, FALSE, FALSE);
					losexp("overexertion", FALSE, FALSE, FALSE);
				}
				losexp("overexertion", TRUE, FALSE, FALSE);
			}
			else {
				You("have a curious feeling...");
			}
			break;
		case SEDU_EXHAUSTED:
			if (greater) {
				if (!Drain_resistance) {
					if (greatest)
						losexp("exhaustion", FALSE, FALSE, FALSE);
					losexp("exhaustion", TRUE, FALSE, FALSE);
				}
				exercise(A_STR, FALSE);
				exercise(A_STR, FALSE);
			}
			You_feel("exhausted.");
			exercise(A_STR, FALSE);
			tmp = rn1(greater || greatest ? 20 : 10, 6);
			if (Half_physical_damage) tmp = (tmp + 1) / 2;
			losehp(tmp, "exhaustion", KILLED_BY);
			break;


		case SEDU_SUCKSOUL:
			if (!Drain_resistance){
				pline("...then tries to suck out your soul with %s!",
					mon->mtyp == PM_AVATAR_OF_LOLTH ? "her fangs" : "a kiss");
				losexp("stolen soul", FALSE, FALSE, FALSE);

				if (mon->mtyp == PM_AVATAR_OF_LOLTH)
					losexp("stolen soul", FALSE, FALSE, FALSE);

				if (greater) {
					losexp("stolen soul", FALSE, FALSE, FALSE);
					losexp("stolen soul", TRUE, FALSE, FALSE);
				}
			}
			break;

		case SEDU_STEALONE:
			buf[0] = '\0';
			steal(mon, buf, FALSE, FALSE);
			break;

		case SEDU_STEALEIGHT:
			/* flavoured for Lolth */
			pline("...then starts picking through your things!");
			for (tmp = 0; tmp < 8; tmp++) {
				buf[0] = '\0';
				steal(mon, buf, FALSE, TRUE);
				/*Lolth is petrification immune, but if the attacker somehow dies from the theft we should return.*/
				if(DEADMONSTER(mon))
					return;
			}
			break;
		
		case SEDU_DAMAGECLK:
			/* flavoured for Malcanthet and Graz'zt */
			pline("...but %s becomes enraged when %s discovers you're mechanical!",
				mhe(mon), mhe(mon));
			verbalize("How dare you trick me!");

			if (mon->female) {
				pline("She attacks your keyhole with her barbed tail!");
				losehp(d(4, 12), "an enraged demoness", KILLED_BY);
				morehungry(d(2,12)*10);
				pline("She claws your face!");
				losehp(d(4, 4), "an enraged demoness", KILLED_BY);
				(void) adjattrib(A_CHA, -1*d(2,4), TRUE);
				}
			else {
				pline("He viciously bites you!");
				losehp(d(4, 8), "an enraged demon prince", KILLED_BY);
				(void) adjattrib(A_CHA, -1*d(1,8), TRUE);
				pline("He drips acid into your inner workings!");
				if(!HAcid_resistance){
					losehp(d(6, 8), "an enraged demon prince", KILLED_BY);
					morehungry(d(3,8));
				}
			}
			AMAX(A_CHA) = ABASE(A_CHA); //stat drain is permanent!
			break;

		case SEDU_LOLTHATTACK:
			/* all-in-one lolth bad-sedu effects */
			if (TRUE) {
				struct trap *ttmp2 = maketrap(u.ux, u.uy, WEB);
				if (ttmp2) {
					pline("She wraps you tight in her webs!");
					dotrap(ttmp2, NOWEBMSG);
	#ifdef STEED
					if (u.usteed && u.utrap) {
						/* you, not steed, are trapped */
						dismount_steed(DISMOUNT_FELL);
					}
	#endif
				}
			}
			if (uclockwork){
				pline("%s pauses in momentary confusion...",
					noit_Monnam(mon));
				if (rn2(5) && !Drain_resistance){
					seduce_effect(mon, SEDU_SUCKSOUL);
				}
				else {
					seduce_effect(mon, SEDU_STEALEIGHT);
				}
			}
			else {
				int tmp;
				pline("After wrapping you up, she bites into your helpless form!");
				exercise(A_STR, FALSE);
				tmp = d(6, 8);
				if (Half_physical_damage) tmp = (tmp + 1) / 2;
				losehp(tmp, "Lolth's bite", KILLED_BY);

				seduce_effect(mon, rn2(2) ? SEDU_VAMP : SEDU_POISONBITE);
			}
			break;

		case SEDU_VAMP:
			if (has_blood(youracedata)) {
				Your("blood is being drained!");
				/* Get 1/20th of full corpse value
				* Therefore 4 bites == 1 drink
				*/
				if (get_mx(mon, MX_EDOG))
					EDOG(mon)->hungrytime += ((int)((youracedata)->cnutrit / 20) + 1);
			}
			if (!mon->mcan && !rn2(3) && !Drain_resistance) {
				losexp("life force drain", FALSE, FALSE, FALSE);
				losexp("life force drain", TRUE, FALSE, FALSE);
			}
			break;
		case SEDU_POISONBITE:
			pline("%s injects you with %s poison!", (mon->female ? "She" : "He"), (mon->female ? "her" : "his"));
			if (Poison_resistance) pline_The("poison doesn't seem to affect you.");
			else {
				(void)adjattrib(A_CON, -4, TRUE);
				exercise(A_CON, FALSE);
				flags.botl = 1;
				if (!Upolyd || Unchanging){
					if (mon->mtyp == PM_AVATAR_OF_LOLTH)
						killer = "the poisoned kiss of Lolth's fangs";
					else
						killer = "a poisoned kiss of fangs";
					killer_format = KILLED_BY;
					done(DIED);
				}
				else {
					rehumanize();
					change_gevurah(1); //cheated death.
				}
			}
			break;

		case SEDU_BURDEN:
			Your("pack feels heavier.");
			(void)adjattrib(A_STR, -2, TRUE);
			u.ucarinc -= 100;
			break;

		case SEDU_CURSE:
			verbalize("If thou art as terrible a fighter as thou art a lover, death shall find you soon.");
			u.uacinc -= 10;
			u.udaminc -= 10;
			u.uhitinc -= 10;
			break;

		case SEDU_GREMLIN:
			You_feel("robbed... but your possessions are still here...?");
			attrcurse();
			break;

		case SEDU_PARALYZE:
			if (Levitation || Weightless || Is_waterlevel(&u.uz))
				You("are motionlessly suspended.");
#ifdef STEED
			else if (u.usteed)
				You("are frozen in place!");
#endif
			else
				You("are paralyzed!");
			pline("She has immobilized you with her magic!");
			nomul(-(rn1(10, 25)), "immobilized by night-terrors");
			nomovemsg = You_can_move_again;
			exercise(A_DEX, FALSE);
			break;

		case SEDU_STEALSIX:
			verbalize("Surely you don't need all this junk?!");
			for (tmp = 0; tmp < 6; tmp++) {
				buf[0] = '\0';
				steal(mon, buf, FALSE, FALSE);
				/*Graz'zt is petrification immune, but if the attacker somehow dies from the theft we should return.*/
				if(DEADMONSTER(mon))
					return;
			}
			break;

		case SEDU_BADWEAP:
			if (u.twoweap){
				verbalize("You're going to hurt yourself with those.");
				u.twoweap = FALSE;
				optr = uswapwep;
				setuswapwep((struct obj *)0);
				freeinv(optr);
				(void)mpickobj(mon, optr);

				optr = uwep;
				setuwep((struct obj *)0);
				freeinv(optr);
				(void)mpickobj(mon, optr);
			}
			else if (uwep && !(uwep->otyp != BAR)){
				verbalize("You're going to hurt yourself with that.");
				optr = uwep;
				setuwep((struct obj *)0);
				freeinv(optr);
				(void)mpickobj(mon, optr);
			}
			if (!uwep){
				buf[0] = '\0';
				steal(mon, buf, FALSE, FALSE);
				/*Graz'zt is petrification immune, but if the attacker somehow dies from the theft we should return.*/
				if(DEADMONSTER(mon))
					return;
				optr = mksobj(BAR, NO_MKOBJ_FLAGS);
				curse(optr);
				optr->spe = -6;
				verbalize("This will keep you out of trouble.");
				(void)hold_another_object(optr, u.uswallow ?
					"Fortunately, you're out of reach! %s away." :
					"Fortunately, you can't hold anything more! %s away.",
					The(aobjnam(optr,
					Weightless || u.uinwater ?
					"slip" : "drop")),
					(const char *)0);
				if (carried(optr)){
					setuwep(optr);
				}
			}
			else{
				verbalize("You're so helpless!");
				losexp("dark speech", FALSE, TRUE, FALSE);
			}
			break;
		case SEDU_BADHAT:
			if (uarmh && uarmh->otyp != DUNCE_CAP){
				Helmet_off();
			}
			if (!uarmh){
				verbalize("This should greatly improve your intellect.");
				buf[0] = '\0';
				steal(mon, buf, FALSE, FALSE);
				/*Graz'zt is petrification immune, but if the attacker somehow dies from the theft we should return.*/
				if(DEADMONSTER(mon))
					return;
				optr = mksobj(DUNCE_CAP, NO_MKOBJ_FLAGS);
				curse(optr);
				optr->spe = -6;
				(void)hold_another_object(optr, u.uswallow ?
					"Fortunately, you're out of reach! %s away." :
					"Fortunately, you can't hold anything more! %s away.",
					The(aobjnam(optr,
					Weightless || u.uinwater ?
					"slip" : "drop")),
					(const char *)0);
				if (carried(optr)){
					setworn(optr, W_ARMH);
					Helmet_on();
				}
			}
			else{
				verbalize("You're so stupid!");
				losexp("dark speech", FALSE, TRUE, FALSE);
			}

			break;
		case SEDU_BADBOOTS:
			if (uarmf && uarmf->otyp != FUMBLE_BOOTS){
				Boots_off();
			}
			if (!uarmf){
				verbalize("These boots will improve your looks.");
				buf[0] = '\0';
				steal(mon, buf, FALSE, FALSE);
				/*Graz'zt is petrification immune, but if the attacker somehow dies from the theft we should return.*/
				if(DEADMONSTER(mon))
					return;
				optr = mksobj(FUMBLE_BOOTS, NO_MKOBJ_FLAGS);
				curse(optr);
				optr->spe = -6;
				(void)hold_another_object(optr, u.uswallow ?
					"Fortunately, you're out of reach! %s away." :
					"Fortunately, you can't hold anything more! %s away.",
					The(aobjnam(optr,
					Weightless || u.uinwater ?
					"slip" : "drop")),
					(const char *)0);
				if (carried(optr)){
					setworn(optr, W_ARMF);
					Boots_on();
				}
			}
			else{
				verbalize("You're so clumsy!");
				losexp("dark speech", FALSE, TRUE, FALSE);
			}
			break;
		case SEDU_BADAMU:
			if (uamul && uamul->otyp != AMULET_OF_RESTFUL_SLEEP){
				Amulet_off();
			}
			if (!uamul){
				verbalize("You need to take things more slowly.");
				buf[0] = '\0';
				steal(mon, buf, FALSE, FALSE);
				/*Graz'zt is petrification immune, but if the attacker somehow dies from the theft we should return.*/
				if(DEADMONSTER(mon))
					return;
				optr = mksobj(AMULET_OF_RESTFUL_SLEEP, NO_MKOBJ_FLAGS);
				curse(optr);
				(void)hold_another_object(optr, u.uswallow ?
					"Fortunately, you're out of reach! %s away." :
					"Fortunately, you can't hold anything more! %s away.",
					The(aobjnam(optr,
					Weightless || u.uinwater ?
					"slip" : "drop")),
					(const char *)0);
				if (carried(optr)){
					setworn(optr, W_AMUL);
					Amulet_on();
				}
			}
			else{
				verbalize("You're so lazy!");
				losexp("dark speech", FALSE, TRUE, FALSE);
			}
			break;
		case SEDU_PUNISH:
			punish((struct obj *)0);
			if(Punished)
				uball->owt = min(uball->owt+320, 1600);
			verbalize("Stay here.");
			break;
		case SEDU_JEALOUS:
			/* shared between Malcanthet and Graz'zt */
			if (mon->female) {
				if (rn2(2) || uarmh){
					pline("She jealously attacks you with her barbed tail!");
					losehp(d(4, 12), "a jealous demoness", KILLED_BY);
				}
				else{
					pline("She jealously claws your face!");
					losehp(d(4, 4), "a jealous demoness", KILLED_BY);
					(void)adjattrib(A_CHA, -1 * d(2, 4), TRUE);
					AMAX(A_CHA) = ABASE(A_CHA); //permanent drain!
				}
			}
			else {
				if(rn2(2) || uarmh || HAcid_resistance){
					pline("He viciously bites you in jealousy!");
					losehp(d(4, 8), "a jealous demon prince", KILLED_BY);
				}
				else{
					pline("He jealously drips acid on your face!");
					losehp(d(6, 8), "a jealous demon prince", KILLED_BY);
					(void) adjattrib(A_CHA, -1*d(1,8), TRUE);
					AMAX(A_CHA) = ABASE(A_CHA); //permanent drain!
				}
			}
			break;
		}
	}
	else {
		switch (effect_num)
		{
		case SEDU_GAINEN:
			You_feel("raised to your full potential.");
			exercise(A_CON, TRUE);
			if (greater)
				exercise(A_CON, TRUE);
			u.uenbonus += rnd(10) + 5;
			calc_total_maxen();
			u.uen = min(u.uen + 400, u.uenmax);
			break;
		case SEDU_GOODENOUGH:
			You_feel("good enough to do it again.");
			(void)adjattrib(A_CON, 1 + greater, TRUE);
			exercise(A_CON, TRUE);
			if (greater) {
				exercise(A_CON, TRUE);
				exercise(A_CON, TRUE);
			}
			flags.botl = 1;
			break;
		case SEDU_REMEMBER:
			You("will always remember %s...", noit_mon_nam(mon));
			(void)adjattrib(A_WIS, 1 + greater, TRUE);
			exercise(A_WIS, TRUE);
			if (greater) {
				exercise(A_WIS, TRUE);
				exercise(A_WIS, TRUE);
			}
			flags.botl = 1;
			break;
		case SEDU_EDUCATE:
			pline("That was a very educational experience.");
			pluslvl(FALSE);
			exercise(A_WIS, TRUE);
			if (greater) {
				pluslvl(FALSE);
				exercise(A_WIS, TRUE);
			}
			break;
		case SEDU_RESTOREHP:
			You_feel("restored to health!");
			u.uhp = u.uhpmax;
			if (Upolyd) u.mh = u.mhmax;
			exercise(A_STR, TRUE);
			if (greater) {
				u.uenbonus += rnd(10) + 5;
				calc_total_maxen();
				u.uen = min(u.uen + 400, u.uenmax);
			}
			flags.botl = 1;
			break;

		case SEDU_WIND:
			if (!uclockwork || Race_if(PM_INCANTIFIER)) {
				impossible("Can't wind you!");
				break;
			}
			You("persuade %s to wind your clockwork.",
				noit_mon_nam(mon));
			struct obj *key;
			int turns = 0;

			Strcpy(class_list, tools);
			key = getobj(class_list, "wind with");
			if (!key){
				pline1(Never_mind);
			}
			else {
				turns = ask_turns(mon, 0, 0);
				if (!turns){
					pline1(Never_mind);
				}
				else {
					/* 80% to 120% of asked for, in increments of 10% */
					turns = turns * (8 + rn2(5)) / 10;
					lesshungry(turns * 10);
					You("notice %s wound your clockwork %d times.", noit_mon_nam(mon), turns);
				}
			}
			break;

		case SEDU_NOTHING:
			pline("%s %s happy, but confused.",
				noit_Monnam(mon),
				Blind ? "seems" : "looks"
				);
			break;

		case SEDU_WISH:
			verbalize("Tell me your greatest desire!");
			makewish(WISH_VERBOSE);	// can not grant artifacts
			break;

		case SEDU_BLESS:
			verbalize("Go forth and slay thy enemies with my blessing!");
			u.udaminc += d(1, 10);
			u.uhitinc += d(1, 10);
			break;

		case SEDU_PROTECT:
			verbalize("Thou art wonderful! My favor shall protect you from harm!");
			/* Well, she's mixing thous and yous in these pronouncements, */
			/* But apparently she's ALSO overenthused enough to bless somebody who's fighting her, so... */
			u.uacinc += d(1, 10);
			break;

		case SEDU_RAISESTATS:
			You_feel("raised to your full potential.");
			/* raises all stats by 2 and exercises them 3 times each */
			for (tmp = 0; tmp < 18; tmp++) {
				if (tmp % 3 == 0)
					adjattrib(tmp / 3, 2, TRUE);
				exercise(tmp / 3, TRUE);
			}
			flags.botl = 1;
			break;

		case SEDU_LIFEFONT:
			verbalize("Truly thou art as a fountain of life!");
			u.uhpmultiplier += 2;
			u.uenmultiplier += 2;
			break;

		case SEDU_CARRYCAP:
			You_feel("as though you could lift mountains!");
			u.ucarinc += d(1, 4) * 50;
			break;

		case SEDU_POISRES:
			if (!(HPoison_resistance & INTRINSIC)) {
				You_feel("healthy.");
				HPoison_resistance |= TIMEOUT_INF;
			}
			else pline("but that's about it.");
			break;
		case SEDU_ACIDRES:
			if (!(HAcid_resistance & INTRINSIC)) {
				if (Hallucination) You("like you've gone back to the basics.");
				else Your("health seems insoluble.");
				HAcid_resistance |= TIMEOUT_INF;
			}
			else pline("but that's about it.");
			break;
		case SEDU_SICKRES:
			if (!(HSick_resistance & INTRINSIC) && !rn2(4)) {
				You(Hallucination ? "feel alright." :
					"feel healthier than you ever have before.");
				HSick_resistance |= TIMEOUT_INF;
			}
			else pline("but that's about it.");
			break;

		case SEDU_GENOCIDE:
			verbalize("Tell me, whom shall I kill?");
			do_genocide(1);
			break;

		case SEDU_SIXMAGICS:{
								int i, j;
								verbalize("I grant you six magics!");
								/* get six enchantable/chargable pieces of gear that are less than +6 */
								struct obj * gear[] = {
									uwep,
									uarm,
									uarmc,
									uarms,
									uswapwep,
									uarmh,
									uarmg,
									uarmf,
									uquiver,
									uarmu,
									uright,
									uleft,
									(struct obj *)0
								};
								int len = SIZE(gear);
								struct obj * otmp;
								for (i = 6; i > 0 && len > 0;) {
									j = rn2(len);
									otmp = gear[j];
									if (otmp && (
										otmp->oclass == WEAPON_CLASS ||
										otmp->oclass == ARMOR_CLASS ||
										(otmp->oclass == TOOL_CLASS && is_weptool(otmp)) ||
										(otmp->oclass == RING_CLASS && objects[otmp->otyp].oc_charged && otmp->otyp != RIN_WISHES)
										) &&
										otmp->spe < 6) {
										otmp->spe = 6;
										i--;
									}
									else {
										for (; j < len; j++)
											gear[j] = gear[j + 1];
										len--;
									}
								}}
			break;

		case SEDU_SIXTRUTHS:
			verbalize("I grant you six truths!");
			for (tmp = 0; tmp < 6; tmp++) {
				optr = mksobj(POT_ENLIGHTENMENT, NO_MKOBJ_FLAGS);
				bless(optr);
				hold_another_object(optr, u.uswallow ?
					"Oops!  %s out of your reach!" :
					(Weightless ||
					Is_waterlevel(&u.uz) ||
					levl[u.ux][u.uy].typ < IRONBARS ||
					levl[u.ux][u.uy].typ >= ICE) ?
					"Oops!  %s away from you!" :
					"Oops!  %s to the floor!",
					The(aobjnam(optr,
					Weightless || u.uinwater ?
					"slip" : "drop")),
					(const char *)0);
			}
			break;

		case SEDU_SIXFOLLOWERS:
			verbalize("I grant you six followers!");
			for (tmp = 0; tmp < 6; tmp++) {
				optr = mksobj(FIGURINE, NO_MKOBJ_FLAGS);
				bless(optr);
				hold_another_object(optr, u.uswallow ?
					"Oops!  %s out of your reach!" :
					(Weightless ||
					Is_waterlevel(&u.uz) ||
					levl[u.ux][u.uy].typ < IRONBARS ||
					levl[u.ux][u.uy].typ >= ICE) ?
					"Oops!  %s away from you!" :
					"Oops!  %s to the floor!",
					The(aobjnam(optr,
					Weightless || u.uinwater ?
					"slip" : "drop")),
					(const char *)0);
			}
			break;

		case SEDU_LIFESAVING:
			verbalize("I grant you life!");
			optr = mksobj(AMULET_OF_LIFE_SAVING, NO_MKOBJ_FLAGS);
			bless(optr);
			(void)hold_another_object(optr, u.uswallow ?
				"Oops!  %s out of your reach!" :
				(Weightless ||
				Is_waterlevel(&u.uz) ||
				levl[u.ux][u.uy].typ < IRONBARS ||
				levl[u.ux][u.uy].typ >= ICE) ?
				"Oops!  %s away from you!" :
				"Oops!  %s to the floor!",
				The(aobjnam(optr,
				Weightless || u.uinwater ?
				"slip" : "drop")),
				(const char *)0);
			if (carried(optr)){
				if (!uamul){
					setworn(optr, W_AMUL);
					Amulet_on();
				}
			}
			break;
		}
	}
	return;
}

/*
 * Handles monsters stealing from monsters, whether straight theft or due to seduction.
 *
 * Returns true if the code returned early
 */

boolean
msteal_m(magr, mdef, attk, result)
struct monst *magr;
struct monst *mdef;
struct attack *attk;
int *result;
{
	const long equipmentmask = ~(W_WEP|W_SWAPWEP);
	boolean seduct_type;
	struct obj * otmp = 0;
	boolean vis = canspotmon(mdef) || canspotmon(magr);
	int nitems = 0;
	boolean goatspawn = (magr->data->mtyp == PM_SMALL_GOAT_SPAWN || magr->data->mtyp == PM_GOAT_SPAWN || magr->data->mtyp == PM_GIANT_GOAT_SPAWN || magr->data->mtyp == PM_BLESSED);
	boolean noflee = (magr->isshk && magr->mpeaceful);
	boolean mi_only = is_chuul(magr->data);
	if(attk->adtyp == AD_SITM){
		/* select item from defender's inventory */
		for (otmp = mdef->minvent; otmp; otmp = otmp->nobj)
			if ((!magr->mtame || !otmp->cursed) && !(otmp->owornmask&equipmentmask) && (!mi_only || is_magic_obj(otmp)))
				nitems++;
		if(nitems){
			nitems = rnd(nitems);
			for (otmp = mdef->minvent; otmp; otmp = otmp->nobj)
				if ((!magr->mtame || !otmp->cursed) && !(otmp->owornmask&equipmentmask) && (!mi_only || is_magic_obj(otmp)))
					if(--nitems <= 0)
						break;
		}

		if (otmp) {
			char onambuf[BUFSZ], mdefnambuf[BUFSZ];

			/* make a special x_monnam() call that never omits
			the saddle, and save it for later messages */
			Strcpy(mdefnambuf, x_monnam(mdef, ARTICLE_THE, (char *)0, 0, FALSE));
			if (u.usteed == mdef &&
				otmp == which_armor(mdef, W_SADDLE))
				/* "You can no longer ride <steed>." */
				dismount_steed(DISMOUNT_POLY);
			obj_extract_self(otmp);
			if (otmp->owornmask) {
				mdef->misc_worn_check &= ~otmp->owornmask;
				if (otmp->owornmask & W_WEP)
					setmnotwielded(mdef, otmp);
				otmp->owornmask = 0L;
				update_mon_intrinsics(mdef, otmp, FALSE, FALSE);
			}
			/* add_to_minv() might free otmp [if it merges] */
			if (vis)
				Strcpy(onambuf, doname(otmp));
			(void)add_to_minv(magr, otmp);
			if (vis) {
				pline("%s steals %s from %s!", Monnam(magr),
					onambuf, mdefnambuf);
			}
			possibly_unwield(mdef, FALSE);
			mdef->mstrategy &= ~STRAT_WAITFORU;
			mselftouch(mdef, (const char *)0, FALSE);
			if (mdef->mhp <= 0)
				*result |= (MM_HIT | MM_DEF_DIED | ((grow_up(magr, mdef)) ? 0 : MM_AGR_DIED));
				return TRUE;
			if(goatspawn)
				*result |= MM_AGR_STOP;
			else if (magr->data->mlet == S_NYMPH && !noflee &&
				!tele_restrict(magr)
			){
				(void)rloc(magr, TRUE);
				*result |= MM_AGR_STOP;
				// if (vis && !canspotmon(magr))
					// pline("%s suddenly disappears!", buf);
			}
			m_dowear(magr, FALSE);
		}
	}
	else if((seduct_type = could_seduce(magr, mdef, attk))){
		boolean seduce = seduct_type == 1;
		struct obj *stealoid = 0, **minvent_ptr;
		
		if(seduce && (attk->adtyp == AD_SSEX || attk->adtyp == AD_LSEX)){
			minvent_ptr = &mdef->minvent;
			while ((otmp = *minvent_ptr) != 0){
				if (otmp->owornmask & (W_ARM|W_ARMU)){
					if (stealoid){ /*Steal suit or undershirt*/
						minvent_ptr = &otmp->nobj;
					}
					else {
						*minvent_ptr = otmp->nobj;	/* take armor out of minvent */
						stealoid = otmp;
						stealoid->nobj = (struct obj *)0;
					}
				} else {
					minvent_ptr = &otmp->nobj;
				}
			}
			*minvent_ptr = stealoid;	/* put armor back into minvent */
			otmp = stealoid;
		}
		else {
			/* select item from defender's inventory */
			for (otmp = mdef->minvent; otmp; otmp = otmp->nobj)
				if ((!magr->mtame || !otmp->cursed) && (seduce || !(otmp->owornmask&equipmentmask)))
					nitems++;
			if(nitems){
				nitems = rnd(nitems);
				for (otmp = mdef->minvent; otmp; otmp = otmp->nobj)
					if ((!magr->mtame || !otmp->cursed) && (seduce || !(otmp->owornmask&equipmentmask)))
						if(--nitems <= 0)
							break;
			}
		}
		if (otmp) {
			int delay = 0;
			if(seduce && (otmp->owornmask&(W_ARM|W_ARMU))){
				long unwornmask;
				//Stealing everything
				// minvent_ptr = &mdef->minvent;
				// while ((otmp = *minvent_ptr) != 0)
					// if (otmp->owornmask & (W_ARM|W_ARMU)){
						// if (stealoid) /*Steal suit or undershirt*/
							// continue;
						// *minvent_ptr = otmp->nobj;	/* take armor out of minvent */
						// stealoid = otmp;
						// stealoid->nobj = (struct obj *)0;
					// } else {
						// minvent_ptr = &otmp->nobj;
					// }
				// *minvent_ptr = stealoid;	/* put armor back into minvent */
				// otmp = stealoid;
				if(vis)
					pline("%s seduces %s and %s starts to take off %s clothes.",
						Monnam(magr), mon_nam(mdef), mhe(mdef), mhis(mdef));
				while ((otmp = mdef->minvent) != 0) {
					/* take the object away from the monster */
					if(otmp->oclass == ARMOR_CLASS && objects[otmp->otyp].oc_delay)
						delay = max(delay, objects[otmp->otyp].oc_delay);
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
					if (vis) {
						pline("%s hands %s %s!", Monnam(mdef), mon_nam(magr), doname(otmp));
					}
					(void) mpickobj(magr, otmp);
				}
			}
			else {
				//Stealing just the one thing
				char onambuf[BUFSZ], mdefnambuf[BUFSZ];

				/* make a special x_monnam() call that never omits
				the saddle, and save it for later messages */
				Strcpy(mdefnambuf, x_monnam(mdef, ARTICLE_THE, (char *)0, 0, FALSE));
				if (u.usteed == mdef &&
					otmp == which_armor(mdef, W_SADDLE))
					/* "You can no longer ride <steed>." */
					dismount_steed(DISMOUNT_POLY);
				obj_extract_self(otmp);
				if(otmp->oclass == ARMOR_CLASS && objects[otmp->otyp].oc_delay)
					delay = objects[otmp->otyp].oc_delay;
				if (otmp->owornmask) {
					mdef->misc_worn_check &= ~otmp->owornmask;
					if (otmp->owornmask & W_WEP)
						setmnotwielded(mdef, otmp);
					otmp->owornmask = 0L;
					update_mon_intrinsics(mdef, otmp, FALSE, FALSE);
				}
				/* add_to_minv() might free otmp [if it merges] */
				if (vis)
					Strcpy(onambuf, doname(otmp));
				(void)add_to_minv(magr, otmp);
				if (vis) {
					if(seduce){
						pline("%s seduces %s and steals %s!", Monnam(magr), mdefnambuf, onambuf);
					}
					else {
						pline("%s charms %s. %s hands over %s!", Monnam(magr), mdefnambuf, SheHeIt(mdef), onambuf);
					}
				}
			}
			possibly_unwield(mdef, FALSE);
			mdef->mstrategy &= ~STRAT_WAITFORU;
			mselftouch(mdef, (const char *)0, FALSE);
			if(delay){
				mdef->mfrozen = max(mdef->mfrozen, delay);
				mdef->mcanmove = FALSE;
			}
			m_dowear(magr, FALSE);
			if (mdef->mhp <= 0)
				*result |= (MM_HIT | MM_DEF_DIED | ((grow_up(magr, mdef)) ? 0 : MM_AGR_DIED));
				return TRUE;
			if(goatspawn)
				*result |= MM_AGR_STOP;
			else if (magr->data->mlet == S_NYMPH && !noflee &&
				!tele_restrict(magr)
			){
				(void)rloc(magr, TRUE);
				*result |= MM_AGR_STOP;
				// if (vis && !canspotmon(magr))
					// pline("%s suddenly disappears!", buf);
			}
		}
	}
	return FALSE;
}



#endif  /* SEDUCE */

#endif /* OVLB */

/*mhitu.c*/
