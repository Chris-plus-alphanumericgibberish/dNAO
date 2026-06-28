#include "hack.h"
#include "xhity.h"
#include "seduce.h"

# ifdef SEDUCE
STATIC_DCL void FDECL(mayberem, (struct obj *, const char *, BOOLEAN_P));
STATIC_DCL void FDECL(lrdmayberem, (struct obj *, const char *, BOOLEAN_P));
STATIC_DCL void FDECL(mlcmayberem, (struct obj *, const char *, BOOLEAN_P));
STATIC_DCL void FDECL(mayberem_common, (struct obj *, const char *, BOOLEAN_P));
STATIC_DCL void FDECL(palemayberem, (struct obj *, const char *, BOOLEAN_P));
STATIC_DCL boolean FDECL(sedu_helpless, (struct monst *));
STATIC_DCL int FDECL(sedu_refuse, (struct monst *));
STATIC_DCL void FDECL(sedu_undress, (struct monst *));
STATIC_DCL void FDECL(sedu_adornment_ring, (struct monst *));
STATIC_DCL void FDECL(sedu_minion, (struct monst *));
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
	boolean tiefling = (youagr && (check_mutation(TT_ATTRACTIVE_1) || check_mutation(TT_ATTRACTIVE_2)));
	boolean tiefling_nymph = (youagr && check_mutation(TT_ATTRACTIVE_1));
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
			|| pagr->mtyp == PM_CARMILLA || pagr->mtyp == PM_VLAD_THE_IMPALER || pagr->mtyp == PM_LEVISTUS
			|| tiefling
	){
		if(genagr == 1 - gendef)
			return 1;
		else
			return (tiefling_nymph || pagr->mlet == S_NYMPH || pagr->mtyp == PM_LEVISTUS) ? 2 : 0;
	}
	else if(pagr->mtyp == PM_MOTHER_LILITH || pagr->mtyp == PM_BELIAL){
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
	if (Blind) pline("It caresses you...");
	else You_feel("very attracted to %s.", mon_nam(mon));

	/* interact with adornment ring */
	sedu_adornment_ring(mon);

	/* undress the player */
	sedu_undress(mon);

	/* if the player is still dressed, refuse */	
	if (uarm || uarmc || uwep || (u.umartial && uarmg) || (Role_if(PM_MONK) && uarmf)) {
		if (sedu_refuse(mon)) {
			/* seducer was refused */
			return 1;
		}
	}
	else if(mon->mtyp == PM_VLAD_THE_IMPALER || mon->mtyp == PM_CARMILLA){
		struct attack vampire_bite = {AT_BITE, AD_VAMP, 1, 6, 0};
		xmeleehity(mon, &youmonst, &vampire_bite, (struct obj **)0, -1, 0, FALSE, 0);
	}
	else {
		/* perform effect */
		int quantity = 0;
		if (mon->mtyp == PM_GRAZ_ZT) quantity = 6;
		else if(mon->mtyp == PM_MALCANTHET) quantity = 4;
		else if(mon->mtyp == PM_MOTHER_LILITH) quantity = 6;
		else if(mon->mtyp == PM_BELIAL) quantity = 4;
		else if(mon->mtyp == PM_AVATAR_OF_LOLTH) quantity = 8;
		else if(mon->mtyp == PM_UNEARTHLY_DROW) quantity = 1;
		else if(mon->mtyp == PM_SUCCUBUS || mon->mtyp == PM_INCUBUS) quantity = 1;
		if(quantity > 0){
			if(is_lord(mon->data) || is_prince(mon->data)){
				verbalize("Kill %s, my minion%s!", flags.female ? "her" : "him", quantity > 1 ? "s" : "");
			}
			else {
				verbalize("Kill %s!", flags.female ? "her" : "him");
			}
		}
		for (int i = 0; i < quantity; i++) {
			sedu_minion(mon);
		}
		if(mon->mtyp == PM_SUCCUBUS || mon->mtyp == PM_INCUBUS)
			mon->mspec_used = rnd(100);
		else
			mon->mspec_used = rnd(10);
	}

	/* possibly exit early, skipping teleport and continuing to make attacks! */
	if (mon->mtyp == PM_BELIAL || mon->mtyp == PM_MOTHER_LILITH)
		return 0;

	/* teleport */
	if (!tele_restrict(mon)) (void) rloc(mon, TRUE);

	return 1;
}

int
dopaleseduce(mon)
register struct monst *mon;
{
	boolean fem = !poly_gender(); /* male = 0, fem = 1, neuter = 2 */
	boolean helpless = FALSE;
	
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
		helpless = TRUE;
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
		int wdmg = (int)(d(1,10)) + 1;
		
		sedu_undress(mon);

		if(rn2( (int)(ACURR(A_WIS)))){
			pline("As you pass through the shroud, your every sense goes mad.");
			Your("whole world becomes an unbearable symphony of agony.");
			//TRANSCENDENCE_IMPURITY_UP(FALSE)
			u.umadness |= MAD_PALE_NIGHT;
			if(roll_madness(MAD_PALE_NIGHT)){
				killer = "seeing something not meant for mortal eyes";
				killer_format = KILLED_BY;
				done(DIED);
			}
			else {
				change_usanity(-rnd(100), TRUE);
			}
		}
		You("find yourself staggering away from %s, with no memory of why.", fem ? "her" : "him");
		make_stunned(HStun + 12, TRUE);
		while( ABASE(A_WIS) > ATTRMIN(A_WIS) && wdmg > 0){
			wdmg--;
			(void) adjattrib(A_WIS, -1, TRUE);
			(void) adjattrib(A_INT, -1, TRUE);
			(void) adjattrib(A_CHA, -1, TRUE);
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
						if (!u.uconduct.killer){
							//Pcifist PCs aren't combatants so if something kills them up "killed peaceful" type impurities
							IMPURITY_UP(u.uimp_murder)
							IMPURITY_UP(u.uimp_bloodlust)
						}
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
		if(obj == uwep || obj == uswapwep){
			Sprintf(qbuf,"\"Put away your %s, %s?\"",
				str,
				(!rn2(2) ? "lover" : !rn2(2) ? "dear" : "sweetheart")
			);
		}
		else {
			Sprintf(qbuf,"\"Shall I remove your %s, %s?\"",
				str,
				(!rn2(2) ? "lover" : !rn2(2) ? "dear" : "sweetheart")
			);
		}
		if (yn(qbuf) == 'n') return;
	} else {
		char hairbuf[BUFSZ];
		if(obj == uwep || obj == uswapwep){
			verbalize("Put away your %s before you hurt someone.", str);
		}
		else {
			Sprintf(hairbuf, "let me run my fingers through your %s",
				body_part(HAIR));
			verbalize("Take off your %s; %s.", str,
				(obj == uarm)  ? "let's get a little closer" :
				(obj == uarmc || obj == uarms) ? "it's in the way" :
				(obj == uarmf) ? "let me rub your feet" :
				(obj == uarmg) ? "they're too clumsy" :
				(obj == uarmu) ? "let me massage you" :
				/* obj == uarmh */
				hairbuf
			);
		}
	}
	IMPURITY_UP(u.uimp_seduction)
	remove_worn_item(obj, TRUE);
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
			if(obj == uwep || obj == uswapwep){
				Your("%s to dust in your %s!", aobjnam(obj, "turn"), bimanual_mon(obj, &youmonst) ? makeplural(body_part(HAND)) : body_part(HAND));
				if(obj == uwep) uwepgone();
				else if(obj == uswapwep) uswapwepgone();
				useup(obj);
			}
			else {
				destroy_arm(obj);
			}
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

void
sedu_undress(mon)
struct monst * mon;
{
	/* check no-clothes case */
	if (!uarm && !uarmc && !uarmf && !uarmg && !uarms && !uarmh && !uwep && !uswapwep
#ifdef TOURIST
		&& !uarmu
#endif
		) {
		/* message */
		switch (mon->mtyp)
		{
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
		undressfunc(uwep, "weapon", helpless);

		/* undress player */
		undressfunc(uswapwep, "backup weapon", helpless);

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
		takesring = mon->female;
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

/*
 * Handles monsters stealing from monsters, whether straight theft or due to seduction.
 *
 * Returns true if the code returned early
 */

boolean
msteal_m(struct monst *magr, struct monst *mdef, struct attack *attk, int *result)
{
	const long equipmentmask = ~(W_WEP|W_SWAPWEP);
	boolean seduct_type;
	struct obj * otmp = 0;
	boolean vis = canspotmon(mdef) || canspotmon(magr);
	int nitems = 0;
	boolean goatspawn = (magr->data->mtyp == PM_SMALL_GOAT_SPAWN || magr->data->mtyp == PM_GOAT_SPAWN || magr->data->mtyp == PM_GIANT_GOAT_SPAWN || magr->data->mtyp == PM_BLESSED);
	boolean noflee = (magr->isshk && magr->mpeaceful);
	boolean mi_only = is_chuul(magr->data);

	if(magr == mdef){
		/* can't steal from yourself*/
		*result |= MM_MISS;
		return TRUE;
	}

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
			if (mdef->mhp <= 0){
				*result |= (MM_HIT | MM_DEF_DIED | ((grow_up(magr, mdef)) ? 0 : MM_AGR_DIED));
				return TRUE;
			}
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
				mdef->mequipping = max(mdef->mequipping, delay);
			}
			m_dowear(magr, FALSE);
			if (mdef->mhp <= 0){
				*result |= (MM_HIT | MM_DEF_DIED | ((grow_up(magr, mdef)) ? 0 : MM_AGR_DIED));
				return TRUE;
			}
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

void
sedu_minion(struct monst *mon)
{
	switch(mon->mtyp) {
		case PM_MALCANTHET:{
			int low_air_types[] = {PM_OSSIFRUGE, PM_BALROG, PM_LILITU};
			makemon(&mons[ROLL_FROM(low_air_types)], u.ux, u.uy, MM_ADJACENTOK);
		}break;
		case PM_GRAZ_ZT:
			makemon(&mons[PM_MARILITH], u.ux, u.uy, MM_ADJACENTOK);
		break;
		case PM_AVATAR_OF_LOLTH:
			makemon(&mons[PM_DEMONIC_BLACK_WIDOW], u.ux, u.uy, MM_ADJACENTOK);
		break;
		case PM_BELIAL:
			makemon(&mons[PM_BARBED_DEVIL], u.ux, u.uy, MM_ADJACENTOK);
		break;
		case PM_MOTHER_LILITH:{
			int malblg_types[] = {PM_BLACK_DRAGON, PM_PIT_FIEND, PM_MASTER_LICH, PM_IRON_GOLEM};
			makemon(&mons[rn2(9) ? PM_BONE_DEVIL : ROLL_FROM(malblg_types)], u.ux, u.uy, MM_ADJACENTOK);
		}break;
		case PM_INCUBUS:
		case PM_SUCCUBUS:
			makemon(&mons[PM_VROCK], u.ux, u.uy, MM_ADJACENTOK);
		break;
		case PM_UNEARTHLY_DROW:
			makemon(&mons[PM_MIRKWOOD_ELDER], u.ux, u.uy, MM_ADJACENTOK);
		break;
		default:
			makemon(&mons[PM_VROCK], u.ux, u.uy, MM_ADJACENTOK);
		break;
	}
}

#endif  /* SEDUCE */

#endif /* OVLB */

/*mhitu.c*/
