/*	SCCS Id: @(#)attrib.c	3.4	2002/10/07	*/
/*	Copyright 1988, 1989, 1990, 1992, M. Stephenson		  */
/* NetHack may be freely redistributed.  See license for details. */

/*  attribute modification routines. */

#include <limits.h>
#include "math.h"
#include "hack.h"
#include "artifact.h"

/* #define DEBUG */	/* uncomment for debugging info */

#ifdef OVLB

/* globals that let other parts of the code *temporarily* hard-set the player's stat */
int override_str = 0;

	/* part of the output on gain or loss of attribute */
static
const char	* const plusattr[] = {
	"strong", "smart", "wise", "agile", "tough", "charismatic"
},
		* const minusattr[] = {
	"weak", "stupid", "foolish", "clumsy", "fragile", "repulsive"
};


static
const struct innate {
	schar	ulevel;
	long	*ability;
	const char *gainstr, *losestr;
}
	arc_abil[] = { {	 1, &(HStealth), "", "" },
		     {   1, &(HFast), "", "" },
		     {  10, &(HSearching), "perceptive", "" },
		     {	 0, 0, 0, 0 } },

	ana_abil[] = { {   7, &(HFast), "quick", "slow" },
		     {	15, &(HWarning), "precognitive", "" },
		     {	 0, 0, 0, 0 } },

	bar_abil[] = { {	 1, &(HPoison_resistance), "", "" },
		     {   7, &(HFast), "quick", "slow" },
		     {  15, &(HStealth), "stealthy", "" },
		     {	 0, 0, 0, 0 } },
#ifdef BARD
	brd_abil[] = { {	5, &(HSleep_resistance), "awake", "tired" },
		     {	 10, &(HStealth), "stealthy", "noisy" },
		     {	 0, 0, 0, 0 } },
#endif
	bin_abil[] = {
		     {	 0, 0, 0, 0 } },

	cav_abil[] = { {	 7, &(HFast), "quick", "slow" },
		     {	15, &(HWarning), "sensitive", "" },
		     {	 0, 0, 0, 0 } },

#ifdef CONVICT
	con_abil[] = { {   1, &(HSick_resistance), "", "" },
	         {	 7, &(HPoison_resistance), "healthy", "" },
		     {  20, &(HSearching), "perceptive", "unaware" },
		     {	 0, 0, 0, 0 } },
#endif	/* CONVICT */
	hea_abil[] = { {	 1, &(HPoison_resistance), "", "" },
		     {	 1, &(HSick_resistance), "", "" },
		     {	15, &(HWarning), "sensitive", "" },
		     {	 0, 0, 0, 0 } },

	kni_abil[] = { {	 7, &(HFast), "quick", "slow" },
		     {	 0, 0, 0, 0 } },

	mon_abil[] = { {   1, &(HFast), "", "" },
		     {   1, &(HSleep_resistance), "", "" },
		     {   1, &(HSee_invisible), "", "" },
		     {   3, &(HPoison_resistance), "healthy", "sickly" },
		     {   5, &(HStealth), "stealthy", "noisy" },
		     {   7, &(HWarning), "sensitive", "insensitive" },
		     {   9, &(HSearching), "perceptive", "unaware" },
		     {  11, &(HFire_resistance), "cool", "warmer" },
		     {  13, &(HCold_resistance), "warm", "cooler" },
		     {  15, &(HShock_resistance), "insulated", "conductive" },
		     {  17, &(HTeleport_control), "controlled","uncontrolled" },
		     {  19, &(HAcid_resistance), "thick-skinned","soft-skinned" },
		     {  21, &(HWwalking), "light on your feet","heavy" },
		     {  23, &(HSick_resistance), "immunized","immunocompromised" },
		     {  25, &(HDisint_resistance), "firm","less firm" },
		     {  27, &(HStone_resistance), "limber","stiff" },
		     {  29, &(HAntimagic), "skeptical","credulous" },
		     {  30, &(HDrain_resistance), "above earthly concerns","not so above it all" },
		     {   0, 0, 0, 0 } },

	mad_abil[] = { {  1, &(HTelepat), "", "" },
			 {  7, &(HSearching), "perceptive", "unaware" },
		     {	15, &(HWarning), "sensitive", "" },
		     {	30, &(HClairvoyant), "clairvoyant", "" },
		     {	 0, 0, 0, 0 } },

	elnob_abil[] = { {	 7, &(HFast), "quick", "slow" },
			{	15, &(HWarning), "sensitive", "" },
		     {  20, &(HFire_resistance), "cool", "warmer" },
		     {	 0, 0, 0, 0 } },

	pir_abil[] = {	{1, &(HSwimming), "", ""  },
			 {	7, &(HStealth), "stealthy", ""  },	/* with cat-like tread ... */
		     {  11, &(HFast), "quick", "slow" },
		     {	 0, 0, 0, 0 } },

	pri_abil[] = { {	15, &(HWarning), "sensitive", "" },
		     {  20, &(HFire_resistance), "cool", "warmer" },
		     {	 0, 0, 0, 0 } },

	ran_abil[] = { {   1, &(HSearching), "", "" },
		     {	 7, &(HStealth), "stealthy", "" },
		     {	15, &(HSee_invisible), "your vision sharpen", "your vision blur" },
		     {	 0, 0, 0, 0 } },

	rog_abil[] = { {	 1, &(HStealth), "", ""  },
		     {  10, &(HSearching), "perceptive", "" },
		     {	 0, 0, 0, 0 } },

	sam_abil[] = { {	 1, &(HFast), "", "" },
		     {  15, &(HStealth), "stealthy", "" },
		     {	 0, 0, 0, 0 } },

	tou_abil[] = { {	10, &(HSearching), "perceptive", "" },
		     {	20, &(HPoison_resistance), "hardy", "" },
		     {	 0, 0, 0, 0 } },

	unh_abil[] = { {	 1, &(HStealth), "", "" },
		     {   7, &(HFast), "quick", "slow" },
		     {  14, &(HDrain_resistance), "full of vigor","not so vigorous" },
		     {  21, &(HSearching), "perceptive", "" },
		     {	 0, 0, 0, 0 } },

	val_abil[] = { {	 1, &(HCold_resistance), "", "" },
		     {	 1, &(HStealth), "", "" },
		     {   7, &(HFast), "quick", "slow" },
		     {	 0, 0, 0, 0 } },

	wiz_abil[] = { {	15, &(HWarning), "sensitive", "" },
		     {  17, &(HTeleport_control), "controlled","uncontrolled" },
		     {	 0, 0, 0, 0 } },

	/* Intrinsics conferred by race */
	elf_abil[] = { {	4, &(HSleep_resistance), "awake", "tired" },
		     {	 0, 0, 0, 0 } },

	orc_abil[] = { {	1, &(HPoison_resistance), "", "" },
		     {  15, &(HAntimagic), "magic resistant","magic-sensitive" },
		     {	 0, 0, 0, 0 } },

	clk_abil[] = { {	1, &(HPoison_resistance), "", "" },
		     {	 1, &(HSick_resistance), "", "" },
		     {	 1, &(HStone_resistance), "", "" },
		     {	 5, &(HShock_resistance), "shock resistant", "less shock resistant" },
		     {	 10, &(HCold_resistance), "cold resistant", "less cold resistant" },
		     {	 15, &(HFire_resistance), "heat resistant", "less heat resistant" },
		     {	 0, 0, 0, 0 } },

	and_abil[] = { {	1, &(HPoison_resistance), "", "" },
		     {	 1, &(HSick_resistance), "", "" },
		     {	 1, &(HStone_resistance), "", "" },
		     {	 1, &(HCold_resistance), "", "" },
		     {   5, &(HStealth), ">Initiating short-range camouflage<", ">Short-range camouflage damaged<" },
		     {  10, &(HShock_resistance), ">Initiating ion-channel re-direction<", ">Ion-channel re-direction non-operational<" },
		     {  15, &(HFire_resistance), ">Activating asymmetrical heat sink<", ">Asymmetrical heat sink destroyed<" },
		     {	 0, 0, 0, 0 } },

	vam_abil[] = { {	1, &(HPoison_resistance), "", "" },
			 {	 1, &(HSleep_resistance), "", "" },
			 {	11, &(HCold_resistance), "the chill of the grave", "the warmth of life" },
		     {	 21, &(HPolymorph_control), "in control", "out of control" },
		     {	 0, 0, 0, 0 } },

	hlf_abil[] = { {	14, &(HFlying), "wings sprout from your back", "your wings shrivel and die" },
		     {	 0, 0, 0, 0 } },

	hlf_cold_abil[] = { {	15, &(HMagical_breathing), "able to breathe underwater", "unable to breathe underwater" },
			  {	15, &(HSwimming), "able to swim", "unable to swim" },
			  {	15, &(HWaterproof), "waterproof", "less waterproof" },
			  {	 0, 0, 0, 0 } },

	/* Not an actual intrinsic, handled elsewhere */
	hlf_fire_abil[] = { {	15, &(HNo_prop), "slime resistant", "less slime resistant" },
			  {	15, &(HNo_prop), "bigger and and stronger", "smaller and weaker" },
			  {	 0, 0, 0, 0 } },

	hlf_acid_abil[] = { {	15, &(HStone_resistance), "limber", "stiff" },
			  {	 0, 0, 0, 0 } },

	hlf_slee_abil[] = { {	15, &(HFree_action), "freed", "a loss of freedom" },
			  {	15, &(HHalluc_resistance), "resistant to hallucinations", "less resistant to hallucinations" },
			  {	 0, 0, 0, 0 } },

	hlf_drst_abil[] = { {	15, &(HSick_resistance), "immunized", "immunocompromized" },
			  {	 0, 0, 0, 0 } },

	/* Not an actual intrinsic, handled elsewhere */
	hlf_elec_abil[] = { {	15, &(HNo_prop), "lightning-fast", "slower" },
			  {	 0, 0, 0, 0 } },

	hlf_magm_abil[] = { {	15, &(HHalf_spell_damage), "resistant to spells", "less resistant to spells" },
			  {	 0, 0, 0, 0 } },

	hlf_disn_abil[] = { {	15, &(HDrain_resistance), "resistant to level drain", "less resistant to level drain" },
			  {	 0, 0, 0, 0 } },

	hlf_rbre_abil[] = { {	15, &(HDisplaced), "your scales shimmer and shift", "your scales cease to shimmer" },
			  {	15, &(HSee_invisible), "your vision sharpen", "your vision blur" },
			  {	 0, 0, 0, 0 } },

	yki_abil[] = { {	1, &(HCold_resistance), "", "" },
		     {  11, &(HFire_resistance), "cool", "warmer" },
		     {	 0, 0, 0, 0 } },

	inc_abil[] = { {	1, &(HAntimagic), "", "" },
		     {	 0, 0, 0, 0 } };

#define	next_check u.exerchkturn
STATIC_DCL void NDECL(exerper);
STATIC_DCL void FDECL(postadjabil, (long *));

/* adjust an attribute; return TRUE if change is made, FALSE otherwise */
boolean
adjattrib(ndx, incr, msgflg)
	int	ndx, incr;
	int	msgflg;	    /* positive => no message, zero => message, and */
{			    /* negative => conditional (msg if change made) */
	if (Fixed_abil || !incr) return FALSE;

	if ((ndx == A_INT || ndx == A_WIS)
				&& uarmh && uarmh->otyp == DUNCE_CAP) {
		if (msgflg == 0)
		    Your("cap constricts briefly, then relaxes again.");
		return FALSE;
	}

	if (incr > 0) {
	    if ((AMAX(ndx) >= ATTRMAX(ndx)) && (ACURR(ndx) >= AMAX(ndx))) {
		if (msgflg == 0 && flags.verbose)
		    pline("You're already as %s as you can get.",
			  plusattr[ndx]);
		ABASE(ndx) = AMAX(ndx) = ATTRMAX(ndx); /* just in case */
		return FALSE;
	    }

	    ABASE(ndx) += incr;
	    if(ABASE(ndx) > AMAX(ndx)) {
		incr = ABASE(ndx) - AMAX(ndx);
		AMAX(ndx) += incr;
		if(AMAX(ndx) > ATTRMAX(ndx))
		    AMAX(ndx) = ATTRMAX(ndx);
		ABASE(ndx) = AMAX(ndx);
	    }
	} else {
	    if (ABASE(ndx) <= ATTRMIN(ndx)) {
			if(ndx == A_WIS && u.wimage >= 10){
				int temparise = u.ugrave_arise;
				pline("The image of the weeping angel is taking over your body!");
				u.ugrave_arise = PM_WEEPING_ANGEL;
				done(WEEPING);
				u.ugrave_arise = temparise;
			} else if(u.umorgul
				&& (ndx == A_WIS || ndx == A_CON || ndx == A_CHA)
				&& ABASE(A_WIS) <= ATTRMIN(A_WIS)
				&& ABASE(A_CON) <= ATTRMIN(A_CON)
				&& ABASE(A_CHA) <= ATTRMIN(A_CHA)
			){
				int temparise = u.ugrave_arise;
				You_feel("something cold pierce your %s!", body_part(HEART));
				u.ugrave_arise = PM_WRAITH;
				killer_format = KILLED_BY;
				killer = "a shard of a morgul-blade";
				done(DIED);
				u.ugrave_arise = temparise;
			} else if(u.umummyrot
				&& (ndx == A_CHA || ndx == A_CON)
				&& ABASE(A_CON) <= ATTRMIN(A_CON)
				&& ABASE(A_CHA) <= ATTRMIN(A_CHA)
			){
				int temparise = u.ugrave_arise;
				You("crack apart and turn to dust!");
				u.ugrave_arise = 0;
				killer_format = KILLED_BY;
				killer = "mummy rot";
				done(DISINTEGRATED);
				u.ugrave_arise = temparise;
			} else {
				if (msgflg == 0 && flags.verbose)
					pline("You're already as %s as you can get.",
					  minusattr[ndx]);
			}
			ABASE(ndx) = ATTRMIN(ndx); /* just in case */
			return FALSE;
	    }

	    ABASE(ndx) += incr;
	    if(ABASE(ndx) < ATTRMIN(ndx)) {
		incr = ABASE(ndx) - ATTRMIN(ndx);
		ABASE(ndx) = ATTRMIN(ndx);
		AMAX(ndx) += incr;
		if(AMAX(ndx) < ATTRMIN(ndx))
		    AMAX(ndx) = ATTRMIN(ndx);
	    }
	}
	if (msgflg <= 0)
	    You_feel("%s%s!",
		  (incr > 1 || incr < -1) ? "very ": "",
		  (incr > 0) ? plusattr[ndx] : minusattr[ndx]);
	flags.botl = 1;
	if (moves > 1 && (ndx == A_STR || ndx == A_CON))
		(void)encumber_msg();
	return TRUE;
}

void
gainstr(otmp, incr)
	register struct obj *otmp;
	register int incr;
{
	int num = 1;

	if(incr) num = incr;
	else {
	    if(ABASE(A_STR) < STR18(85)) num = (rn2(4) ? 1 : rnd(6) );
	}
	(void) adjattrib(A_STR, (otmp && otmp->cursed) ? -num : num, TRUE);
}

void
losestr(num)	/* may kill you; cause may be poison or monster like 'a' */
	register int num;
{
	int ustr = ABASE(A_STR) - num;

	while(ustr < 3) {
	    ++ustr;
	    --num;
	    if (Upolyd) {
		u.mh -= 6;
	    } else {
		u.uhp -= 6;
	    }
		u.uhpmod -= 6;
	}
	calc_total_maxhp();
	(void) adjattrib(A_STR, -num, TRUE);
}

void
change_luck(n)
	register schar n;
{
	u.uluck += n;
	if (u.uluck < 0 && u.uluck < LUCKMIN)	u.uluck = LUCKMIN;
	if (u.uluck > 0 && u.uluck > LUCKMAX)	u.uluck = LUCKMAX;
}

int
stone_luck(parameter)
boolean parameter; /* So I can't think up of a good name.  So sue me. --KAA */
{
	register struct obj *otmp;
	register long bonchance = 0;

	for (otmp = invent; otmp; otmp = otmp->nobj)
	    if (confers_luck(otmp)) {
		if (otmp->cursed) bonchance -= otmp->quan;
		else if (otmp->blessed) bonchance += otmp->quan;
		else if (parameter) bonchance += otmp->quan;
	    }
// #ifdef CONVICT
	// if(Role_if(PM_CONVICT)) bonchance -= (u.ulevel-1)/10 + 1; /* a Convict's karmic burden becomes 
																// only more heavy as they level up */
// #endif	/* CONVICT */
	return (int)bonchance;
}

boolean
has_luckitem()
{
	register struct obj *otmp;

	for (otmp = invent; otmp; otmp = otmp->nobj)
	    if (confers_luck(otmp)) return TRUE;
	return FALSE;
}

/* there has just been an inventory change affecting a luck-granting item */
void
set_moreluck()
{
	int stoneluck = stone_luck(TRUE);
	if (!has_luckitem()) u.moreluck = 0;
	else if (stoneluck >= 0){
		if(Role_if(PM_CONVICT) && stoneluck < LUCKADD)
			u.moreluck = stoneluck;
		else u.moreluck = LUCKADD;
	} else u.moreluck = -LUCKADD;
}

int
stone_health()
{
	struct obj *otmp;
	int healthup = 0;

	for (otmp = invent; otmp; otmp = otmp->nobj)
	    if (otmp->otyp == VITAL_SOULSTONE) {
			if (otmp->cursed) healthup += otmp->quan * 6;
			else if (otmp->blessed) healthup += otmp->quan * 7;
			else healthup += otmp->quan;
	    }
	return (int)healthup;
}

int
stone_energy()
{
	struct obj *otmp;
	int energyup = 0;

	for (otmp = invent; otmp; otmp = otmp->nobj)
	    if (otmp->otyp == SPIRITUAL_SOULSTONE) {
			if (otmp->cursed) energyup += otmp->quan * 6;
			else if (otmp->blessed) energyup += otmp->quan * 7;
			else energyup += otmp->quan;
	    }
	return (int)energyup;
}

#endif /* OVLB */
#ifdef OVL1

void
restore_attrib()
{
	int	i;

	for(i = 0; i < A_MAX; i++) {	/* all temporary losses/gains */

	   if(ATEMP(i) && ATIME(i)) {
		if(!(--(ATIME(i)))) { /* countdown for change */
		    ATEMP(i) += ATEMP(i) > 0 ? -1 : 1;

		    if(ATEMP(i)) /* reset timer */
			ATIME(i) = 100 / ACURR(A_CON);
		}
	   }
	}
	(void)encumber_msg();
}

#endif /* OVL1 */
#ifdef OVLB

#define AVAL	50		/* tune value for exercise gains */

void
exercise(i, inc_or_dec)
int	i;
boolean	inc_or_dec;
{
#ifdef DEBUG
	pline("Exercise:");
#endif
	/* Mechanoids can't exercise abilities */
	if(umechanoid) return;
	/* no physical exercise while polymorphed; the body's temporary */
	if (Upolyd && i != A_WIS && i != A_INT) return;

	//Prevent abuse.
	if(check_preservation(PRESERVE_PREVENT_ABUSE) && !inc_or_dec)
		return;

	if(abs(AEXE(i)) < AVAL) {
		/*
		 *	Law of diminishing returns (Part I):
		 *
		 *	Gain is harder at higher attribute values.
		 *	79% at "3" --> 0% at "18"
		 *	Loss is even at all levels (50%).
		 *
		 *	Note: *YES* ACURR is the right one to use.
		 */
		AEXE(i) += (inc_or_dec)
			? ((rn2(19) > ACURR(i)) || (AEXE(i) < 0 && rn2(AEXE(i)*-1 + 1)))
			: -rn2(2);
#ifdef DEBUG
		pline("%s, %s AEXE = %d",
			(i == A_STR) ? "Str" : (i == A_WIS) ? "Wis" :
			(i == A_DEX) ? "Dex" : (i == A_INT) ? "Int" : "Con",
			(inc_or_dec) ? "inc" : "dec", AEXE(i));
#endif
	}
	if (moves > 0 && (i == A_STR || i == A_CON)) (void)encumber_msg();
}

/* hunger values - from eat.c */
#define SATIATED	0
#define NOT_HUNGRY	1
#define HUNGRY		2
#define WEAK		3
#define FAINTING	4
#define FAINTED		5
#define STARVED		6

STATIC_OVL void
exerper()
{
	if(!(moves % 10)) {
		/* Hunger Checks */

		int hs = (YouHunger > get_satiationlimit()) ? SATIATED :
			 (YouHunger > 150*get_uhungersizemod()) ? NOT_HUNGRY :
			 (YouHunger > 50*get_uhungersizemod()) ? HUNGRY :
			 (YouHunger > 0) ? WEAK : FAINTING;

#ifdef DEBUG
		pline("exerper: Hunger checks");
#endif
		switch (hs) {
		    case SATIATED:	if(!is_vampire(youracedata) && !Race_if(PM_INCANTIFIER))  /* undead/magic metabolism */
							exercise(A_DEX, FALSE);
					if (Role_if(PM_MONK))
					    exercise(A_WIS, FALSE);
					break;
		    case NOT_HUNGRY:	exercise(A_CON, TRUE); break;
		    case WEAK:		exercise(A_STR, FALSE);
					if (Role_if(PM_MONK))	/* fasting */
					    exercise(A_WIS, TRUE);
					break;
		    case FAINTING:
		    case FAINTED:	exercise(A_CON, FALSE); break;
		}

		/* Encumberance Checks */
#ifdef DEBUG
		pline("exerper: Encumber checks");
#endif
		switch (near_capacity()) {
		    case MOD_ENCUMBER:	exercise(A_STR, TRUE); break;
		    case HVY_ENCUMBER:	exercise(A_STR, TRUE);
					exercise(A_DEX, FALSE); break;
		    case EXT_ENCUMBER:	exercise(A_DEX, FALSE);
					exercise(A_CON, FALSE); break;
		}

	}

	/* status checks */
	if(!(moves % 5)) {
#ifdef DEBUG
		pline("exerper: Status checks");
#endif
		if ((HClairvoyant & (INTRINSIC|TIMEOUT)) &&
			!BClairvoyant)                      exercise(A_WIS, TRUE);
		if (HRegeneration)			exercise(A_STR, TRUE);

		if(Sick || Vomiting)     exercise(A_CON, FALSE);
		if(Confusion || Hallucination)		exercise(A_WIS, FALSE);
		if((Wounded_legs 
#ifdef STEED
		    && !u.usteed
#endif
			    ) || Fumbling || HStun)	exercise(A_DEX, FALSE);
	}
}

void
exerchk()
{
	int	i, mod_val;
	
	if(umechanoid) return; /* Mechanoids can't excercise abilities */
	/*	Check out the periodic accumulations */
	exerper();
	
#ifdef DEBUG
	if(moves >= next_check)
		pline("exerchk: ready to test. multi = %d.", multi);
#endif
	/*	Are we ready for a test?	*/
	if(moves >= next_check && !multi) {
#ifdef DEBUG
	    pline("exerchk: testing.");
#endif
	    /*
	     *	Law of diminishing returns (Part II):
	     *
	     *	The effects of "exercise" and "abuse" wear
	     *	off over time.  Even if you *don't* get an
	     *	increase/decrease, you lose some of the
	     *	accumulated effects.
	     */
	    for(i = 0; i < A_MAX; AEXE(i++) /= 2) {

#ifdef DEBUG
		pline("exerchk: testing %s (%d, max %d).",
			(i == A_STR) ? "Str" : (i == A_WIS) ? "Wis" : 
			(i == A_DEX) ? "Dex" : (i == A_INT) ? "Int" : 
			(i == A_CON) ? "Con" : (i == A_CHA) ? "Cha" : 
			"Err", AEXE(i), AMAX(i));
#endif

		if(ABASE(i) >= 18 || (!AEXE(i) && ABASE(i) >= AMAX(i))) continue;
		// if(i == A_CHA) continue;/* can't exercise cha */

#ifdef DEBUG
		pline("passed");
#endif

		/*
		 *	Law of diminishing returns (Part III):
		 *
		 *	You don't *always* gain by exercising.
		 */

		 //	[MRS 92/10/28 - Treat Wisdom specially for balance.]
		// if(rn2(AVAL) > ((i != A_WIS) ? abs(AEXE(i)*2/3) : abs(AEXE(i))))
		    // continue;
		if(i == A_STR && u.uhs >= 3 && AEXE(i) >= 0)
			continue;
		if(!(ABASE(i) < AMAX(i) && AEXE(i) >= 0) && rn2(AVAL) > (abs(AEXE(i)*2/3)) )
		    continue;
		mod_val = AEXE(i) ? sgn(AEXE(i)) : 1;

#ifdef DEBUG
		pline("exerchk: changing %d.", i);
#endif
		if(adjattrib(i, mod_val, -1)) {
#ifdef DEBUG
		    pline("exerchk: changed %d.", i);
#endif
		    /* if you actually changed an attrib - zero accumulation */
		    AEXE(i) = 0;
		    /* then print an explanation */
		    switch(i) {
		    case A_STR: You((mod_val >0) ?
				    "must have been exercising." :
				    "must have been abusing your body.");
				if(mod_val < 0)	AMAX(i) += mod_val; /* permanent drain */
			break;
		    case A_DEX: You((mod_val >0) ?
				    "must have been working on your reflexes." :
				    "haven't been working on reflexes lately.");
				if(mod_val < 0)	AMAX(i) += mod_val; /* permanent drain */
			break;
		    case A_CON: You((mod_val >0) ?
				    "must be leading a healthy life-style." :
					u.umummyrot ? "are gradually rotting to dust!"
					: u.umorgul ? "have the chill of death about you."
				    : "haven't been watching your health.");
				if(mod_val < 0)	AMAX(i) += mod_val; /* permanent drain */
			break;
		    case A_INT: You((mod_val >0) ?
					"must have been really concentrating lately." :
					"haven't been thinking things through.");
				if(mod_val < 0)	AMAX(i) += mod_val; /* permanent drain */
			break;
		    case A_WIS: You((mod_val >0) ?
				    "must have been very observant." :
				    u.wimage >= 10 ? "are being consumed by the image of the weeping angel!" 
					: u.umorgul ? "have the chill of death about you."
					: "haven't been paying attention.");
				if(mod_val < 0)	AMAX(i) += mod_val; /* permanent drain */
			break;
		    case A_CHA: You((mod_val >0) ?
				    "must have been very charming lately." :
					u.umummyrot ? "are gradually rotting to dust!"
					: u.umorgul ? "have the chill of death about you."
				    : "haven't been watching your behavior.");
				if(mod_val < 0)	AMAX(i) += mod_val; /* permanent drain */
			break;
		    }
		}
	    }
	    next_check += rn1(200,800);
#ifdef DEBUG
	    pline("exerchk: next check at %ld.", next_check);
#endif
	}
}

/* OBSOLETE: next_check will otherwise have its initial 600L after a game restore */
/* next_check is now stored in the you struct, and is persistent from session to session. */
void
reset_attribute_clock()
{
	/* if (moves > 600L) next_check = moves + rn1(50,800); */
}


void
init_attr(np)
	register int	np;
{
	register int	i, x, tryct;


	for(i = 0; i < A_MAX; i++) {
	    ABASE(i) = AMAX(i) = urole.attrbase[i];
		if(ABASE(i) > ATTRMAX(i)) ABASE(i) = AMAX(i) = ATTRMAX(i);
	    ATEMP(i) = ATIME(i) = 0;
	    np -= ABASE(i);
	}

	tryct = 0;
	while(np > 0 && tryct < 100) {

	    x = rn2(100);
	    for (i = 0; (i < A_MAX) && ((x -= urole.attrdist[i]) > 0); i++) ;
	    if(i >= A_MAX) continue; /* impossible */

	    if(ABASE(i) >= ATTRMAX(i)) {

		tryct++;
		continue;
	    }
	    tryct = 0;
	    ABASE(i)++;
	    AMAX(i)++;
	    np--;
	}

	tryct = 0;
	while(np < 0 && tryct < 100) {		/* for redistribution */

	    x = rn2(100);
	    for (i = 0; (i < A_MAX) && ((x -= urole.attrdist[i]) > 0); i++) ;
	    if(i >= A_MAX) continue; /* impossible */

	    if(ABASE(i) <= ATTRMIN(i)) {

		tryct++;
		continue;
	    }
	    tryct = 0;
	    ABASE(i)--;
	    AMAX(i)--;
	    np++;
	}
	if(Race_if(PM_INCANTIFIER)){
	    for (i = 0; (i < A_MAX); i++){
			if(ABASE(A_INT) < ABASE(i)){
				ABASE(A_INT) = min(18, ABASE(i));
				AMAX(A_INT) = min(18, AMAX(i));
			}
		}
	} else if(Race_if(PM_ELF)) {
		if(ABASE(A_DEX) < ABASE(A_STR)){
			int d = ABASE(A_DEX);
			ABASE(A_DEX) = ABASE(A_STR);
			ABASE(A_STR) = d;
			AMAX(A_DEX) = ABASE(A_DEX);
			AMAX(A_STR) = ABASE(A_STR);
		}
	}
}

void
init_mask_attr(np, mask)
	int	np;
	struct obj *mask;
{
	int	i, x, tryct;
	struct Role *mrole = pm2role(mask->mp->mskrolenum);

	for(i = 0; i < A_MAX; i++) {
	    ABASE(i) = AMAX(i) = urole.attrbase[i];
	    ATEMP(i) = ATIME(i) = 0;
	    np -= urole.attrbase[i];
	}

	tryct = 0;
	while(np > 0 && tryct < 100) {

	    x = rn2(100);
	    for (i = 0; (i < A_MAX) && ((x -= urole.attrdist[i]) > 0); i++) ;
	    if(i >= A_MAX) continue; /* impossible */

	    if(ABASE(i) >= ATTRMAX(i)) {

		tryct++;
		continue;
	    }
	    tryct = 0;
	    ABASE(i)++;
	    AMAX(i)++;
	    np--;
	}

	tryct = 0;
	while(np < 0 && tryct < 100) {		/* for redistribution */

	    x = rn2(100);
	    for (i = 0; (i < A_MAX) && ((x -= urole.attrdist[i]) > 0); i++) ;
	    if(i >= A_MAX) continue; /* impossible */

	    if(ABASE(i) <= ATTRMIN(i)) {

		tryct++;
		continue;
	    }
	    tryct = 0;
	    ABASE(i)--;
	    AMAX(i)--;
	    np++;
	}
}

void
redist_attr()
{
	register int i, tmp;

	for(i = 0; i < A_MAX; i++) {
	    if (i==A_INT || i==A_WIS) continue;
		/* Polymorphing doesn't change your mind */
	    tmp = AMAX(i);
	    AMAX(i) += (rn2(5)-2);
	    if (AMAX(i) > ATTRMAX(i)) AMAX(i) = ATTRMAX(i);
	    if (AMAX(i) < ATTRMIN(i)) AMAX(i) = ATTRMIN(i);
	    ABASE(i) = ABASE(i) * AMAX(i) / tmp;
	    /* ABASE(i) > ATTRMAX(i) is impossible */
	    if (ABASE(i) < ATTRMIN(i)) ABASE(i) = ATTRMIN(i);
	}
	(void)encumber_msg();
}

STATIC_OVL
void
postadjabil(ability)
long *ability;
{
	if (!ability) return;
	if (ability == &(HWarning) || ability == &(HSee_invisible))
		see_monsters();
}

void
adjabil(oldlevel,newlevel)
int oldlevel, newlevel;
{
	register const struct innate *abil, *rabil, *sabil;
	long mask = FROMEXPER;

	// set default values
	abil = 0;
	rabil = 0;
	sabil = 0;

	switch (Role_switch) {
	case PM_ARCHEOLOGIST:   abil = arc_abil;	break;
	case PM_ANACHRONONAUT:    abil = ana_abil;	break;
	case PM_BARBARIAN:      abil = bar_abil;	break;
#ifdef BARD
	case PM_BARD:           abil = brd_abil;	break;
#endif
	case PM_EXILE:      	abil = bin_abil;	break;
	case PM_CAVEMAN:        abil = cav_abil;	break;
#ifdef CONVICT
	case PM_CONVICT:        abil = con_abil;	break;
#endif	/* CONVICT */
	case PM_MADMAN:        abil = mad_abil;	break;
	case PM_HEALER:         abil = hea_abil;	break;
	case PM_KNIGHT:         abil = kni_abil;	break;
	case PM_MONK:           abil = mon_abil;	break;
	case PM_NOBLEMAN:
		switch (Race_switch)
		{
		case PM_HALF_DRAGON:
			abil = kni_abil;
			break;
		case PM_DWARF:
			abil = kni_abil;
			break;
		case PM_DROW:
			if (flags.initgend)
				abil = pri_abil;
			else
				abil = 0;
			break;
		case PM_ELF:
			abil = elnob_abil;
			break;
		default:
			abil = 0;
			break;
		}
	break;
	case PM_PIRATE:         abil = pir_abil;	break;
	case PM_PRIEST:         abil = pri_abil;	break;
	case PM_RANGER:         abil = ran_abil;	break;
	case PM_ROGUE:          abil = rog_abil;	break;
	case PM_SAMURAI:        abil = sam_abil;	break;
#ifdef TOURIST
	case PM_TOURIST:        abil = tou_abil;	break;
#endif
	case PM_UNDEAD_HUNTER:   abil = unh_abil;	break;
	case PM_VALKYRIE:       abil = val_abil;	break;
	case PM_WIZARD:         abil = wiz_abil;	break;
	default:                abil = 0;		break;
	}

	switch (Race_switch) {
	case PM_ELF:            rabil = elf_abil;	break;
	case PM_DROW:           rabil = elf_abil;	break;
	case PM_MYRKALFR:       rabil = elf_abil;	break;
	case PM_ORC:            rabil = orc_abil;	break;
	case PM_CLOCKWORK_AUTOMATON:rabil = clk_abil;	break;
	case PM_ANDROID:		rabil = and_abil;	break;
	case PM_INCANTIFIER:	rabil = inc_abil;	break;
	case PM_VAMPIRE:		rabil = vam_abil;	break;
	case PM_HALF_DRAGON:	rabil = hlf_abil;	break;
	case PM_YUKI_ONNA:		rabil = yki_abil;	break;
	case PM_HUMAN:
	case PM_DWARF:
	case PM_GNOME:
	default:                rabil = 0;		break;
	}

	if (Race_if(PM_HALF_DRAGON)) {
		switch (flags.HDbreath) {
		case AD_COLD: sabil = hlf_cold_abil; break;
		case AD_FIRE: sabil = hlf_fire_abil; break;
		case AD_ACID: sabil = hlf_acid_abil; break;
		case AD_SLEE: sabil = hlf_slee_abil; break;
		case AD_DRST: sabil = hlf_drst_abil; break;
		case AD_ELEC: sabil = hlf_elec_abil; break;
		case AD_MAGM: sabil = hlf_magm_abil; break;
		case AD_DISN: sabil = hlf_disn_abil; break;
		case AD_RBRE: sabil = hlf_rbre_abil; break;
		}
	}

	while (abil || rabil || sabil) {
	    long prevabil;
	    /* Have we finished with the intrinsics list? */
	    if (!abil || !abil->ability) {
	    	/* Try the race intrinsics */
		if (!rabil || !rabil->ability) {
		    /* Then the species intrinsics */
		    if (!sabil || !sabil->ability) break;
		    abil = sabil;
		    sabil = 0;
		    mask = FROMRACE;
		} else {
		    abil = rabil;
		    rabil = 0;
		    mask = FROMRACE;
		}
	    }
		prevabil = *(abil->ability);
		if(oldlevel < abil->ulevel && newlevel >= abil->ulevel) {
			/* Abilities gained at level 1 can never be lost
			 * via level loss, only via means that remove _any_
			 * sort of ability.  A "gain" of such an ability from
			 * an outside source is devoid of meaning, so we set
			 * FROMOUTSIDE to avoid such gains.
			 */
			if (abil->ulevel == 1)
				*(abil->ability) |= (mask|FROMOUTSIDE);
			else
				*(abil->ability) |= mask;
			if(!(*(abil->ability) & INTRINSIC & ~mask)) {
			    if(*(abil->gainstr)){
					if(Race_if(PM_ANDROID) && mask == FROMRACE)
						pline("%s", abil->gainstr);
					else You_feel("%s!", abil->gainstr);
				}
			}
		} else if (oldlevel >= abil->ulevel && newlevel < abil->ulevel) {
			*(abil->ability) &= ~mask;
			if(!(*(abil->ability) & INTRINSIC)) {
			    if(*(abil->losestr)){
					if(Race_if(PM_ANDROID) && mask == FROMRACE)
						pline("%s", abil->losestr);
					else You_feel("%s!", abil->losestr);
			    } else if(*(abil->gainstr))
				You_feel("less %s!", abil->gainstr);
			}
		}
	    if (prevabil != *(abil->ability))	/* it changed */
		postadjabil(abil->ability);
	    abil++;
	}

	if (oldlevel > 0) {
		int skillslots;
	    if (newlevel > oldlevel){
			skillslots = newlevel - oldlevel;
			if(Race_if(PM_HUMAN) || Race_if(PM_ANDROID)){
				if(!(skillslots%2)) skillslots *= 1.5;
				else if(!(newlevel%2)) skillslots = skillslots*1.5 + 1;
				else skillslots *= 1.5;
			}
			add_weapon_skill(skillslots);
		}
	    else{
			skillslots = oldlevel - newlevel;
			if(Race_if(PM_HUMAN) || Race_if(PM_ANDROID)){
				if(!(skillslots%2)) skillslots *= 1.5;
				else if(!(oldlevel%2)) skillslots = skillslots*1.5 + 1;
				else skillslots *= 1.5;
			}
			lose_weapon_skill(skillslots);
		}
	}
	boolean message = FALSE;
	if (newlevel >= 14 && oldlevel < 14){
		for (int i = 0; i < P_NUM_SKILLS; i++) {
			if (roleSkill(i)){
				message = TRUE;
				expert_weapon_skill(i);
			}
	    }
		if (message) You_feel("like you've unlocked new potential!");
	}
}


int
newhp()
{
	int	hp;


	if (u.ulevel == 0) {
	    /* Initialize hit points */
	    hp = urole.hpadv.infix + urace.hpadv.infix;
	    if (urole.hpadv.inrnd > 1) hp += rnd(urole.hpadv.inrnd);
		else if(urole.hpadv.inrnd > 0) hp += rn2(urole.hpadv.inrnd);
	    if (urace.hpadv.inrnd > 1) hp += rnd(urace.hpadv.inrnd);
		else if(urace.hpadv.inrnd > 0) hp += rn2(urace.hpadv.inrnd);

	    /* Initialize alignment stuff */
	    u.ualign.type = aligns[flags.initalign].value;
		u.ualign.god = align_to_god(u.ualign.type);
	    u.ualign.record = urole.initrecord;

		return hp;
	} else {
	    if (u.ulevel < urole.xlev) {
	    	hp = urole.hpadv.lofix + urace.hpadv.lofix;
	    	if (urole.hpadv.lornd > 1) hp += rnd(urole.hpadv.lornd);
	    	else if (urole.hpadv.lornd > 0) hp += rn2(2); //0-1
	    	if (urace.hpadv.lornd > 1) hp += rnd(urace.hpadv.lornd);
	    	else if (urace.hpadv.lornd > 0) hp += rn2(2); //0-1
	    } else {
	    	hp = urole.hpadv.hifix + urace.hpadv.hifix;
	    	if (urole.hpadv.hirnd > 1) hp += rnd(urole.hpadv.hirnd);
	    	else if (urole.hpadv.hirnd > 0) hp += rn2(2); //0-1
	    	if (urace.hpadv.hirnd > 1) hp += rnd(urace.hpadv.hirnd);
	    	else if (urace.hpadv.hirnd > 0) hp += rn2(2); //0-1
	    }
	}
	
	return((hp <= 0) ? 1 : hp);
}

int
newen()
{
	int	en;

	
	if (u.ulevel == 0) {
	    /* Initialize energy points */
	    en = urole.enadv.infix + urace.enadv.infix;
	    if (urole.enadv.inrnd > 1) en += rnd(urole.enadv.inrnd);
		else if(urole.enadv.inrnd > 0) en += rn2(urole.enadv.inrnd);
	    if (urace.enadv.inrnd > 1) en += rnd(urace.enadv.inrnd);
		else if(urace.enadv.inrnd > 0) en += rn2(urace.enadv.inrnd);

		return en;
	} else {
	    if (u.ulevel < urole.xlev) {
	    	en = urole.enadv.lofix + urace.enadv.lofix;
	    	if (urole.enadv.lornd > 1) en += rnd(urole.enadv.lornd);
	    	else if (urole.enadv.lornd > 0) en += rn2(2); //0-1
	    	if (urace.enadv.lornd > 1) en += rnd(urace.enadv.lornd);
	    	else if (urace.enadv.lornd > 0) en += rn2(2); //0-1
	    } else {
	    	en = urole.enadv.hifix + urace.enadv.hifix;
	    	if (urole.enadv.hirnd > 1) en += rnd(urole.enadv.hirnd);
	    	else if (urole.enadv.hirnd > 0) en += rn2(2); //0-1
	    	if (urace.enadv.hirnd > 1) en += rnd(urace.enadv.hirnd);
	    	else if (urace.enadv.hirnd > 0) en += rn2(2); //0-1
	    }
	}
	
	return((en <= 0) ? 1 : en);
}

int
maxhp(incon)
int incon;
{
	int	hp;
	int perlevel;


	/* Initialize hit points */
	hp = urole.hpadv.infix + urace.hpadv.infix;
	
	hp += urole.hpadv.inrnd;
	hp += urace.hpadv.inrnd;

	if (u.ulevel <= urole.xlev) {
		perlevel = urole.hpadv.lofix + urace.hpadv.lofix;
		perlevel += urole.hpadv.lornd;
		perlevel += urace.hpadv.lornd;
		
		hp += (u.ulevel-1)*perlevel;
	} else {
		perlevel = urole.hpadv.lofix + urace.hpadv.lofix;
		perlevel += urole.hpadv.lornd;
		perlevel += urace.hpadv.lornd;
		
		hp += (urole.xlev-1)*perlevel; //Full bonus from below cutoff
		
		perlevel = urole.hpadv.hifix + urace.hpadv.hifix;
		perlevel += urole.hpadv.hirnd;
		perlevel += urace.hpadv.hirnd;
		
		hp += (u.ulevel-urole.xlev)*perlevel;
	}
	
	if(incon) hp += u.ulevel*conplus(ACURR(A_CON));
	
	return((hp <= u.ulevel) ? u.ulevel : hp);
}

int
maxen()
{
	int	en;
	int perlevel;


	/* Initialize hit points */
	en = urole.enadv.infix + urace.enadv.infix;
	
	en += urole.enadv.inrnd;
	en += urace.enadv.inrnd;

	if (u.ulevel <= urole.xlev) {
		perlevel = urole.enadv.lofix + urace.enadv.lofix;
		perlevel += urole.enadv.lornd;
		perlevel += urace.enadv.lornd;
		
		en += (u.ulevel-1)*perlevel;
	} else {
		perlevel = urole.enadv.lofix + urace.enadv.lofix;
		perlevel += urole.enadv.lornd;
		perlevel += urace.enadv.lornd;
		
		en += (urole.xlev-1)*perlevel; //Full bonus from below cutoff
		
		perlevel = urole.enadv.hifix + urace.enadv.hifix;
		perlevel += urole.enadv.hirnd;
		perlevel += urace.enadv.hirnd;
		
		en += (u.ulevel-urole.xlev)*perlevel;
	}
	
	return((en <= u.ulevel) ? u.ulevel : en);
}

double
conplus(con)
	int con;
{
	double conplus;
	
	if (con <= 3) conplus = -2;
	else if (con <= 4) conplus = -1.5;
	else if (con <= 6) conplus = -1;
	else if (con < 13) conplus = 0;
	else conplus = (con-11)/4.0;
	
	return conplus;
}

void
calc_total_maxhp()
{
	int ulev;
	int * hpmax;
	int * hp;
	int * hprolled;
	int hpcap;
	int rawmax;
	int maxbonus;
	int adjbonus;
	int uhpbonus = u.uhpbonus;
	if(check_mutation(SHUB_RADIANCE))
		uhpbonus -= Insanity;

	if (Upolyd) {
		ulev = (int)(mons[u.umonnum].mlevel);
		hp = &u.mh;
		hpmax = &u.mhmax;
		hprolled = &u.mhrolled;
		hpcap = 24 + 2*mons[u.umonnum].mlevel*hd_size(&mons[u.umonnum]);
	} else {
		ulev = u.ulevel;
		hp = &u.uhp;
		hpmax = &u.uhpmax;
		hprolled = &u.uhprolled;
		hpcap = 24 + 2*maxhp(1);
	}
	
	if(uhpbonus > 0){
		rawmax = *hprolled + ulev*conplus(ACURR(A_CON));
		
		/*Calculate Metamorphosis *before* the max bonus is determined*/
		if(active_glyph(CLOCKWISE_METAMORPHOSIS)){
			rawmax *= 1.3;
			hpcap *= 1.3;
		}
		/*Calculate Eyes *before* the max bonus is determined*/
		if(active_glyph(ROTTEN_EYES)){
			rawmax *= 1.05;
			hpcap *= 1.05;
		}
		
		maxbonus = hpcap - rawmax;
		
		/*Calculate Malcanthet's multiplier *after* the max bonus is determined*/
		if(u.uhpmultiplier)
			rawmax = rawmax + (rawmax * u.uhpmultiplier / 10); /*Multiplier is in units of tenths*/
		
		if(maxbonus > 0){
			adjbonus = round(2.0*maxbonus/(1+exp(-(4.0/(2.0*maxbonus))*uhpbonus)) - maxbonus);
		}
		else adjbonus = 0;
		
		*hpmax = rawmax + adjbonus + u.uhpmod;
	} else {
		rawmax = *hprolled + ulev*conplus(ACURR(A_CON));
		
		if(active_glyph(CLOCKWISE_METAMORPHOSIS)){
			rawmax *= 1.3;
			hpcap *= 1.3;
		}

		if(active_glyph(ROTTEN_EYES)){
			rawmax *= 1.05;
			hpcap *= 1.05;
		}
		
		if(u.uhpmultiplier)
			rawmax = rawmax + (rawmax * u.uhpmultiplier / 10); /*Multiplier is in units of tenths*/
		
		*hpmax = rawmax + uhpbonus + u.uhpmod;
	}
	
	if(*hpmax < 1) *hpmax = 1;
	// *hpmax += min(nxtra, max(0, 6*nxtra/5 - 6*nxtra*(*hpmax)*(*hpmax)/(5*hpcap*hpcap)));
	if(*hp > *hpmax)
		*hp = *hpmax;
	
	flags.botl = 1;
}

void
calc_total_maxen()
{
	int en;
	int uenbonus = u.uenbonus;
	if(check_mutation(SHUB_RADIANCE))
		uenbonus += Insanity;

	en = u.uenrolled + (u.ulevel*ACURR(A_INT))/4;
	
	if(active_glyph(FORMLESS_VOICE))
		en *= 1.3;
	
	if(u.uenmultiplier)
		en = en + (en * u.uhpmultiplier / 10); /*Multiplier is in units of tenths*/
	
	u.uenmax = en + uenbonus;
	
	if(u.uenmax < 0) u.uenmax = 0;
	// *hpmax += min(nxtra, max(0, 6*nxtra/5 - 6*nxtra*(*hpmax)*(*hpmax)/(5*hpcap*hpcap)));
	if(u.uen > u.uenmax)
		u.uen = u.uenmax;
	
	flags.botl = 1;
}

#endif /* OVLB */
#ifdef OVL0

schar
acurr(x, mon)
int x;
struct monst *mon;
{
	boolean is_player = !mon; 
	struct permonst *dat = (is_player? youracedata: mon->data);
	struct obj *arm = (is_player ? uarm : which_armor(mon, W_ARM));
	struct obj *armu = (is_player ? uarmu : which_armor(mon, W_ARMU));
	struct obj *armc = (is_player ? uarmc : which_armor(mon, W_ARMC));
	struct obj *armg = (is_player ? uarmg : which_armor(mon, W_ARMG));
	struct obj *arms = (is_player ? uarms : which_armor(mon, W_ARMS));
	struct obj *armh = (is_player ? uarmh : which_armor(mon, W_ARMH));
	struct obj *wep = (is_player ? uwep : MON_WEP(mon));
	struct obj *swapwep = (is_player ? uswapwep : MON_SWEP(mon));
    const struct artifact *oart = (struct artifact *) 0;
	if(wep){
		oart = get_artifact(wep);
	}
	
	int tmp;
	if(is_player){
		tmp = (u.abon.a[x] + u.atemp.a[x] + u.acurr.a[x]);
	} else {
		tmp = (mon->abon.a[x] + mon->atemp.a[x] + mon->acurr.a[x]);
	}
	if(is_player){
		if (x == A_STR && override_str)
			return override_str;
		if(u.ufirst_light)
			tmp++;
		if(u.ufirst_sky)
			tmp++;
		if(u.ufirst_life)
			tmp++;
		if(u.ufirst_know && (x == A_INT || x == A_WIS || x == A_CHA))
			tmp+=2;
		if(uring_art(ART_SHARD_FROM_MORGOTH_S_CROWN)){
			tmp += 6;
		}
	}
	if (x == A_CHA && tmp < 18 && dat && (dat->mlet == S_NYMPH ||
		dat->mtyp==PM_SUCCUBUS || dat->mtyp == PM_INCUBUS))
		tmp = 18;
	if(x == A_WIS){
		if(arm && arti_chawis(arm, FALSE) && armc){
			tmp += arm->spe;
		}
		if(armu && arti_chawis(armu, FALSE) && (armc || (arm && arm_blocks_upper_body(arm->otyp)))){
			tmp += armu->spe;
		}
	}
	
	if(x == A_CHA){
		if(armc && arti_chawis(armc, FALSE)){
			tmp += armc->spe;
		}
		if(arm && arti_chawis(arm, FALSE) && !armc){
			tmp += arm->spe;
		}
		if(armu && arti_chawis(armu, FALSE) && !armc && !(arm && arm_blocks_upper_body(arm->otyp))){
			tmp += armu->spe;
		}
		//If dress is "on top" i.e., not blocked by body armor (cloak is fine)
		if(armu && armu->otyp == PLAIN_DRESS && !(arm && arm_blocks_upper_body(arm->otyp))){
			tmp += armu->spe;
		}
		if(arm && (arm->otyp == PLAIN_DRESS || arm->otyp == NOBLE_S_DRESS)){
			tmp += arm->spe;
		}
		if(wep && wep->oartifact == ART_SODE_NO_SHIRAYUKI){
			tmp += wep->spe;
		}
		if(uarmh && uarmh->oartifact == ART_ENFORCED_MIND){
			tmp += uarmh->spe;
		}
	}
	if (x == A_STR) {
		if(is_player && Race_if(PM_ORC)){
			tmp += u.ulevel/3;
			if(tmp > 18) tmp = STR19(tmp);
		}
		if ((armg && (armg->otyp == GAUNTLETS_OF_POWER || (armg->otyp == IMPERIAL_ELVEN_GAUNTLETS && check_imp_mod(armg, IEA_GOPOWER)))) || 
			(wep &&((wep->oartifact == ART_SCEPTRE_OF_MIGHT) || 
					 (wep->oartifact == ART_PEN_OF_THE_VOID && wep->ovar1&SEAL_YMIR && mvitals[PM_ACERERAK].died > 0) ||
					 (oart && (oart->inv_prop == GITH_ART || oart->inv_prop == ZERTH_ART || oart->inv_prop == AMALGUM_ART) && artinstance[ART_SKY_REFLECTED].ZerthUpgrades&ZPROP_POWER) ||
					 (wep->oartifact == ART_STORMBRINGER) ||
					 (wep->oartifact == ART_OGRESMASHER)
			)) ||
			// (swapwep && swapwep->oartifact == ART_STORMBRINGER) ||
			(swapwep && swapwep->oartifact == ART_OGRESMASHER) ||
			(arms && arms->oartifact == ART_GOLDEN_KNIGHT)
		) return(STR19(25));
		else return((schar)((tmp >= STR19(25)) ? STR19(25) : (tmp <= 3) ? 3 : tmp));
	} else if (x == A_CON) {
		if (
			(uwep && uwep->oartifact == ART_OGRESMASHER)
			|| (uswapwep && uswapwep->oartifact == ART_OGRESMASHER)
			|| (uwep && uwep->oartifact == ART_STORMBRINGER)
			|| (uarmg && uarmg->oartifact == ART_GREAT_CLAWS_OF_URDLEN)
			|| (uarmg && uarmg->oartifact == ART_RED_CORDS_OF_ILMATER)
		) return(25);
		if(is_player && Race_if(PM_ORC)){
			tmp += u.ulevel/3;
		}
	} else if (x == A_DEX) {
		if(is_player && mad_turn(MAD_HOST))
			return 3;
		if (
			(armg && armg->oartifact == ART_GODHANDS)
		) return(25);
		if (
			(armg && (armg->oartifact == ART_PREMIUM_HEART || (armg->otyp == IMPERIAL_ELVEN_GAUNTLETS && check_imp_mod(armg, IEA_GODEXTERITY))))
		) tmp += armg->spe;
		if(is_player && Race_if(PM_ORC)){
			tmp += u.ulevel/3;
		}
	} else if (x == A_CHA) {
		if(is_player && u.umadness&MAD_ROTTING && !BlockableClearThoughts){
			tmp -= (NightmareAware_Insanity)/5;
		}
	} else if (x == A_INT || x == A_WIS) {
		/* yes, this may raise int/wis if player or monster is sufficiently
		 * stupid.  there are lower levels of cognition than "dunce".
		 */
		if (armh && armh->otyp == DUNCE_CAP) return(6);
		else if(is_player && u.sealsActive&SEAL_HUGINN_MUNINN) return 25;
	}
	//Clamp
	int max = 25;
	if(x == A_INT || x == A_WIS){
		tmp -= u.mental_scores_down/3;
		if(preservation_count() > 5)
			max -= 2;
		else if(preservation_count() > 2)
			max -= 1;
		if(parasite_count() > 5)
			max -= 2;
		else if(parasite_count() > 2)
			max -= 1;
		if(max < 3) max = 3;
	}
	return((schar)((tmp >= max) ? max : (tmp <= 3) ? 3 : tmp));

}

schar
acurr_player(x)
int x;
{
	return acurr(x, (struct monst *) 0);
}

void
change_usanity(delta, check)
int delta;
boolean check;
{
	int nightmare_starting_sanity = NightmareAware_Sanity;
	int starting_sanity = u.usanity;
	int starting_insanity = NightmareAware_Insanity;
	if(discover || wizard)
		pline("Sanity change: %d + %d", u.usanity, delta);
	u.usanity += delta;
	if(!u.umadness && u.usanity < 50)
		u.usanity = 50;
	else if(u.usanity < 0)
		u.usanity = 0;
	if(u.usanity > 100)
		u.usanity = 100;
	if(discover || wizard)
		pline("= %d", u.usanity);

	/* possibly (de)activate glyphs */
	long int thought;
	for (thought = 1L; thought <= u.thoughts; thought = thought << 1) {
		if ((u.thoughts&thought) &&
			active_glyph(thought) != was_active_glyph(thought, u.uinsight, u.usanity - delta)
			) {
			change_glyph_active(thought, active_glyph(thought));
		}
	}
	
	if(check && delta < 0 && ((-delta > rn2(ACURR(A_WIS))) || -delta >= starting_sanity/10) && rn2(100) >= nightmare_starting_sanity 
		&& !Panicking && !StumbleBlind && !StaggerShock && !Babble && !Screaming && !FaintingFits
	){
		switch(rn2(5)){
			case 0:
				if(BlockableClearThoughts)
					You_feel("a little panicky.");
				else
					You("panic in your insanity!");
				HPanicking = 1+rnd((starting_insanity)/10+1)+rnd((starting_insanity)/10+1);
			break;
			case 1:
				if(BlockableClearThoughts)
					You_feel("a little off balance.");
				else
					You("stumble blindly in your insanity!");
				HStumbleBlind = 1+rnd((starting_insanity)/10+1)+rnd((starting_insanity)/10+1);
			break;
			case 2:
				if(BlockableClearThoughts)
					You_feel("a little shocked.");
				else
					You("stagger in shock!");
				HStaggerShock = 1+rnd((starting_insanity)/10+1)+rnd((starting_insanity)/10+1);
			break;
			case 3:
				if(BlockableClearThoughts)
					You_feel("a little incoherent.");
				else
					You("begin babbling incoherently!");
				HBabble = 1+rnd((starting_insanity)/10+1)+rnd((starting_insanity)/10+1);
			break;
			case 4:
				if(BlockableClearThoughts)
					You_feel("a little frightened.");
				else
					You("begin screaming in terror and madness!");
				HScreaming = 1+rnd((starting_insanity)/10+1)+rnd((starting_insanity)/10+1);
			break;
			/*Dummied out for being unusually nasty and non-interactive*/
			// case 5:
				// if(BlockableClearThoughts)
					// You_feel("a little faint.");
				// else
					// You(Hallucination ? "have a case of the vapors!" : "feel faint!");
				// HFaintingFits = 1+rnd((starting_insanity)/10+1)+rnd((starting_insanity)/10+1);
			// break;
		}
		nomul(0, NULL);
	}
	calc_total_maxen();
	calc_total_maxhp();
}

void
change_uinsight(delta)
int delta;
{
	if(u.veil)
		return;
	if(discover || wizard)
		pline("Insight change: %d + %d", u.uinsight, delta);
	reset_rndmonst(NON_PM);
	u.uinsight += delta;
	if(u.uinsight < 0)
		u.uinsight = 0;
	if(discover || wizard)
		pline("= %d", u.uinsight);

	/* possibly (de)activate glyphs */
	long int thought;
	for (thought = 1L; thought <= u.thoughts; thought = thought << 1) {
		if ((u.thoughts&thought) &&
			active_glyph(thought) != was_active_glyph(thought, u.uinsight-delta, u.usanity)
			) {
			change_glyph_active(thought, active_glyph(thought));
		}
	}
}

boolean
check_insight()
{
	int insight;
	if(u.uinsight > INSIGHT_RATE/20)
		insight = INSIGHT_RATE/20;
	else insight = u.uinsight;
	
	return insight > rn2(INSIGHT_RATE);
}

int
roll_generic_madness(clearable)
boolean clearable;
{
	int sanlevel;
	int usan = u.usanity;
	if((clearable && BlockableClearThoughts) || TimeStop)
		return 0;

	sanlevel = (int)(((float)rand()/(float)(RAND_MAX)) * ((float)rand()/(float)(RAND_MAX)) * 100);
	
	//Note: Clear Thoughts plus Walking Nightmare yields partial resistance rather than complete.
	if(clearable)
		usan = NightmareAware_Sanity;

	if(usan < sanlevel)
		return 1;
	return 0;
}

int
roll_generic_flat_madness(clearable)
int clearable;
{
	int sanlevel;
	int usan = u.usanity;
	if((clearable && BlockableClearThoughts) || TimeStop)
		return 0;

	//Note: Clear Thoughts plus Walking Nightmare yields partial resistance rather than complete.
	if(clearable)
		usan = NightmareAware_Sanity;

	if(usan < rnd(100))
		return 1;
	return 0;
}

int
count_madnesses()
{
	int count = 0;
	int i;
	for(i=0; i < 32; i++){
		if(u.umadness&(0x1L<<i))
			count++;
	}
	return 0;
}

int
roll_madness(madness)
long int madness;
{
	int sanlevel;
	int usan = u.usanity;
	if((BlockableClearThoughts && madness != MAD_GOAT_RIDDEN) || TimeStop)
		return 0;
	if(madness == MAD_NON_EUCLID && DimensionalLock)
		return 0;
	
	//Scorpions duplicates the effects of formications.
	if(madness == MAD_FORMICATION && (u.umadness&MAD_SCORPIONS))
		madness = MAD_SCORPIONS;
	
	if(!(u.umadness&madness))
		return 0;
	
	sanlevel = (int)(((float)rand()/(float)(RAND_MAX)) * ((float)rand()/(float)(RAND_MAX)) * 100);
	
	//Note: Clear Thoughts plus Walking Nightmare yields partial resistance rather than complete.
	if(madness != MAD_GOAT_RIDDEN)
		usan = NightmareAware_Sanity;

	if(usan < sanlevel)
		return 1;
	return 0;
}

int
mad_turn(madness)
long int madness;
{
	int sanlevel;
	int usan = u.usanity;
	unsigned long hashed = hash((unsigned long) (moves + nonce + hash((unsigned long)madness))); //Offset the different madnesses before hashing
	if((BlockableClearThoughts && madness != MAD_GOAT_RIDDEN && madness != MAD_CANNIBALISM) || TimeStop)
		return 0;
	if(madness == MAD_NON_EUCLID && DimensionalLock)
		return 0;

	if(!(u.umadness&madness))
		return 0;
	
	sanlevel = max_ints(1,(int)(((float)hashed/ULONG_MAX) * ((float)hash(hashed)/ULONG_MAX) * 100));
	
	//Note: Clear Thoughts plus Walking Nightmare yields partial resistance rather than complete.
	if(madness != MAD_GOAT_RIDDEN && madness != MAD_CANNIBALISM)
		usan = NightmareAware_Sanity;

	if(usan < sanlevel)
		return 1;
	return 0;
}

int
flat_mad_turn(madness)
long int madness;
{
	int sanlevel;
	int usan = u.usanity;
	unsigned long hashed = hash((unsigned long) (moves + nonce + hash((unsigned long)madness))); //Offset the different madnesses before hashing
	if((BlockableClearThoughts && madness != MAD_GOAT_RIDDEN && madness != MAD_CANNIBALISM) || TimeStop)
		return 0;
	if(madness == MAD_NON_EUCLID && DimensionalLock)
		return 0;
	
	if(!(u.umadness&madness))
		return 0;
	
	//Note: Clear Thoughts plus Walking Nightmare yields partial resistance rather than complete.
	if(madness != MAD_GOAT_RIDDEN && madness != MAD_CANNIBALISM)
		usan = NightmareAware_Sanity;

	if(usan < hashed%100)
		return 1;
	return 0;
}

int
mad_monster_turn(mon, madness)
struct monst *mon;
long int madness;
{
	int sanlevel;
	int usan = u.usanity;
	unsigned long hashed = hash((unsigned long) (moves + nonce + hash((unsigned long)madness + mon->m_id))); //Offset the different madnesses before hashing
	if((BlockableClearThoughts && madness != MAD_GOAT_RIDDEN) || TimeStop)
		return 0;
	if(madness == MAD_NON_EUCLID && DimensionalLock)
		return 0;
	
	if(!(u.umadness&madness))
		return 0;
	
	sanlevel = max_ints(1,(int)(((float)hashed/ULONG_MAX) * ((float)hash(hashed)/ULONG_MAX) * 100));
	
	//Note: Clear Thoughts plus Walking Nightmare yields partial resistance rather than complete.
	if(madness != MAD_GOAT_RIDDEN)
		usan = NightmareAware_Sanity;

	if(usan < sanlevel)
		return 1;
	return 0;
}

void
roll_frigophobia()
{
	if(roll_madness(MAD_FRIGOPHOBIA)){
		if(!Cold_resistance){
			You("panic from the cold!");
			HPanicking += 1+rnd(6);
		}
		else if(roll_madness(MAD_FRIGOPHOBIA)){//Very scared of ice
			You("panic from the chill!");
			HPanicking += 1+rnd(3);
		}
	}
}


/* Airless Void-aware frigophobia */
void
roll_av_frigophobia()
{
	if(roll_madness(MAD_FRIGOPHOBIA)){
		if(FrozenAir && !Breathless){
			You("panic in the airless void!");
			HPanicking += 1+rnd(8);
		} else if(!Cold_resistance){
			You("panic in the cold!");
			HPanicking += 1+rnd(6);
		}
		else if(roll_madness(MAD_FRIGOPHOBIA)){//Very scared of ice
			You("panic beneath the airless heavens!");
			HPanicking += 1+rnd(4);
		}
	}
}

void
give_madness(mon)
struct monst *mon;
{
	int mm = monsndx(mon->data);
	switch(mm){
		case PM_WALKING_DELIRIUM:
			u.umadness |= MAD_DELUSIONS;
		break;
		case PM_VERMIURGE:
			u.umadness |= MAD_VERMIN;//You are stung by vermin
		break;
		case PM_AKKABISH_TANNIN:
			u.umadness |= MAD_FORMICATION;
		break;
		case PM_SHALOSH_TANNAH:
			u.umadness |= MAD_HOST;
		break;
		case PM_NACHASH_TANNIN:
			u.umadness |= MAD_SCIAPHILIA;
		break;
		case PM_KHAAMNUN_TANNIN:
			u.umadness |= MAD_FORGETFUL;
		break;
		case PM_RAGLAYIM_TANNIN:
			u.umadness |= MAD_TOO_BIG;
		break;
		case PM_SARTAN_TANNIN:
			u.umadness |= MAD_ROTTING;
		break;
		case PM_ALDINACH:
			u.umadness |= MAD_REAL_DELUSIONS;
		break;
		case PM_ALRUNES:
		case PM_HATEFUL_WHISPERS:
			u.umadness |= MAD_SANCTITY;
		break;
		case PM_JUIBLEX:
			u.umadness |= MAD_GLUTTONY;
		break;
		case PM_ZUGGTMOY:
			u.umadness |= MAD_SPORES;
		break;
		case PM_KOSTCHTCHIE:
			u.umadness |= MAD_FRIGOPHOBIA;
		break;
		case PM_BAPHOMET:
			u.umadness |= MAD_RAGE;
		break;
		case PM_YEENOGHU:
			u.umadness |= MAD_CANNIBALISM;
		break;
		case PM_MALCANTHET:
			u.umadness |= MAD_NUDIST;
		break;
		case PM_SHAKTARI:
			u.umadness |= MAD_OPHIDIOPHOBIA;
		break;
		case PM_AVATAR_OF_LOLTH:
			u.umadness |= MAD_ARACHNOPHOBIA;
		break;
		case PM_GRAZ_ZT:
			u.umadness |= MAD_ARGENT_SHEEN;
		break;
		case PM_ORCUS:
			u.umadness |= MAD_SUICIDAL;
		break;
		case PM_DAGON:
			u.umadness |= MAD_THALASSOPHOBIA;
		break;
		case PM_OBOX_OB:
			u.umadness |= MAD_ENTOMOPHOBIA;
		break;
		case PM_LAMASHTU:
			u.umadness |= MAD_TALONS;
		break;
		case PM_DEMOGORGON:
			u.umadness |= MAD_PARANOIA;
		break;
		case PM_ELDER_PRIEST:
			u.umadness |= MAD_HELMINTHOPHOBIA;
		break;
		case PM_GREAT_CTHULHU:
			// u.umadness |= ;
		break;
	}
	check_madman_trophy();
}

/* A monster has seen/whatever you and may contract madnesses
 */
void
you_inflict_madness(mon)
struct monst *mon;
{
	if(mon->seenmadnesses != u.umadness){
		unsigned long long int madflag;
		for(madflag = 0x1L; madflag <= LAST_MADNESS; madflag = madflag << 1){
			if(u.umadness&madflag && !(mon->seenmadnesses&madflag) && roll_generic_madness(FALSE)){
				mon->seenmadnesses |= madflag;
				if(d(2,u.ulevel) >= mon->m_lev){
					if(u.specialSealsActive&SEAL_YOG_SOTHOTH){
						yog_credit(mon->m_lev);
					}
					if(madflag == MAD_DELUSIONS
					 || madflag == MAD_REAL_DELUSIONS
					 || madflag == MAD_SPIRAL
					){
						mon->mcrazed = 1;
					}
					else if(madflag == MAD_SANCTITY){
						mon->msanctity = 1;
					}
					else if(madflag == MAD_FORMICATION){
						mon->mformication = 1;
					}
					else if(madflag == MAD_SCORPIONS){
						mon->mscorpions = 1;
					}
					else if(madflag == MAD_SPORES){
						mon->mspores = 1;
					}
					else if(madflag == MAD_GLUTTONY){
						mon->mgluttony = 1;
					}
					else if(madflag == MAD_FRIGOPHOBIA){
						mon->mfrigophobia = 1;
					}
					else if(madflag == MAD_CANNIBALISM){
						mon->mcannibal = 1;
					}
					else if(madflag == MAD_RAGE){
						mon->mrage = 1;
						mon->mberserk = 1;
						mon->mcrazed = 1;
					}
					else if(madflag == MAD_ARGENT_SHEEN){
						mon->margent = 1;
					}
					else if(madflag == MAD_SUICIDAL){
						mon->msuicide = 1;
					}
					else if(madflag == MAD_NUDIST){
						mon->mnudist = 1;
					}
					else if(madflag == MAD_OPHIDIOPHOBIA){
						mon->mophidio = 1;
					}
					else if(madflag == MAD_ARACHNOPHOBIA){
						mon->marachno = 1;
					}
					else if(madflag == MAD_ENTOMOPHOBIA){
						mon->mentomo = 1;
					}
					else if(madflag == MAD_THALASSOPHOBIA){
						mon->mthalasso = 1;
					}
					else if(madflag == MAD_HELMINTHOPHOBIA){
						mon->mhelmintho = 1;
					}
					else if(madflag == MAD_PARANOIA){
						mon->mparanoid = 1;
					}
					else if(madflag == MAD_TALONS){
						mon->mtalons = 1;
					}
					else if(madflag == MAD_DREAMS){
						mon->mdreams = 1;
						if(!resists_sleep(mon) && !mon->msleeping){
							mon->msleeping = 1;
							slept_monst(mon);
						}
					}
					else if(madflag == MAD_SCIAPHILIA){
						mon->msciaphilia = 1;
					}
					else if(madflag == MAD_FORGETFUL){
						mon->mforgetful = 1;
					}
					else if(madflag == MAD_APOSTASY){
						mon->mapostasy = 1;
					}
					else if(madflag == MAD_TOO_BIG){
						mon->mtoobig = 1;
					}
					else if(madflag == MAD_ROTTING){
						mon->mrotting = 1;
					}
					else if(madflag == MAD_FRENZY){
						mon->mhp = 1;
					}
					else if(madflag == MAD_VERMIN){
						mon->mvermin = 1;
					}
					// MAD_HOST:
					// MAD_COLD_NIGHT:
					// MAD_OVERLORD:
					// MAD_THOUSAND_MASKS:
				}
			}
		}
	}
}


/* condense clumsy ACURR(A_STR) value into value that fits into game formulas
 */
schar
acurrstr(str)
	int str;
{
	if (str <= 18) return((schar)str);
	if (str <= 41) return((schar)(19 + str / 10)); /* map to 19-21 */
	else return((schar)(str - 20));
}

#endif /* OVL0 */
#ifdef OVL2

/* avoid possible problems with alignment overflow, and provide a centralized
 * location for any future alignment limits
 */
void
adjalign(n)
register int n;
{
	register int newalign = u.ualign.record + n;

	if(n < 0) {
		IMPURITY_UP(u.uimp_bloodlust)
		if(newalign < u.ualign.record)
			u.ualign.record = newalign;
		if(u.ualign.record > ALIGNLIM)
			u.ualign.record = ALIGNLIM;
	} else
		if(newalign > u.ualign.record) {
			u.ualign.record = newalign;
			if(u.ualign.record > ALIGNLIM)
				u.ualign.record = ALIGNLIM;
		}
}

void
unSetFightingForm(fform)
int fform;
{
	int i, first, last;
	if(fform > LAST_FFORM || fform < 0)
		impossible("Attempting to set fighting form number %d?", fform);
	
	if (fform >= FIRST_LS_FFORM && fform <= LAST_LS_FFORM){
		first = FIRST_LS_FFORM;
		last = LAST_LS_FFORM;
	} else if (fform >= FIRST_KNI_FFORM && fform <= LAST_KNI_FFORM){
		first = FIRST_KNI_FFORM;
		last = LAST_KNI_FFORM;
	} else {
		first = 0;
		last = FFORM_LISTSIZE*32;
	}

	/* this code assumes that each batch of 32 fighting forms are mutually exclusive, but not with other batches of 32 */
	for(i=first/32; i <= last/32; i++)
		u.fightingForm[i] = 0L;

}

void
setFightingForm(fform)
int fform;
{
	/* this code assumes that each batch of 32 fighting forms are mutually exclusive, but not with other batches of 32 */
	unSetFightingForm(fform);
	u.fightingForm[(fform-1)/32] |= (0x1L << ((fform-1)%32));
}

boolean
activeFightingForm(fform)
int fform;
{
	return (selectedFightingForm(fform) && !blockedFightingForm(fform));
}

boolean
activeMentalEdge(fform)
int fform;
{
	return (artinstance[ART_SILVER_SKY].GithStyle == fform && !blockedMentalEdge(fform));
}

boolean
selectedFightingForm(fform)
int fform;
{
	int i;
	if(fform > LAST_FFORM || fform < 0)
		impossible("Attempting to check fighting form number %d?", fform);
	
	if(!fform){
		//Check if there are ANY fighting forms set at all
		int i;
		for(i=0; i < FFORM_LISTSIZE; i++){
			if(u.fightingForm[i])
				return FALSE; //Found (at least one) fighting form, return FALSE
		}
		return TRUE; //Found no fighting forms, return TRUE
	}
	
	return !!(u.fightingForm[(fform-1)/32] & (0x1L << ((fform-1)%32)));
}

int
getFightingFormSkill(fform)
int fform;
{
	switch(fform){
		case FFORM_SHII_CHO:
			return P_SHII_CHO;
		break;
		case FFORM_MAKASHI:
			return P_MAKASHI;
		break;
		case FFORM_SORESU:
			return P_SORESU;
		break;
		case FFORM_ATARU:
			return P_ATARU;
		break;
		case FFORM_DJEM_SO:
			return P_DJEM_SO;
		break;
		case FFORM_SHIEN:
			return P_SHIEN;
		break;
		case FFORM_NIMAN:
			return P_NIMAN;
		break;
		case FFORM_JUYO:
			return P_JUYO;
		break;
		case FFORM_SHIELD_BASH:
			return P_SHIELD_BASH;
		break;
		case FFORM_GREAT_WEP:
			return P_GREAT_WEP;
		break;
		case FFORM_HALF_SWORD:
			return P_GENERIC_KNIGHT_FORM;
		break;
		case FFORM_POMMEL:
			return P_GENERIC_KNIGHT_FORM;
		break;
		case FFORM_KNI_SACRED:
			return P_KNI_SACRED;
		break;
		case FFORM_KNI_RUNIC:
			return P_KNI_RUNIC;
		break;
		case FFORM_KNI_ELDRITCH:
			return P_KNI_ELDRITCH;
		break;
		default:
			impossible("Attempting to get skill of fighting form number %d?", fform);
			return P_NONE;
		break;
	}
	return P_NONE; //Never reached
}

const char *
nameOfFightingForm(fform)
int fform;
{
	switch (fform)
	{
		case FFORM_SHII_CHO: return "Shii Cho";
		case FFORM_MAKASHI:  return "Makashi";
		case FFORM_SORESU:   return "Soresu";
		case FFORM_ATARU:    return "Ataru";
		case FFORM_DJEM_SO:  return "Djem So";
		case FFORM_SHIEN:    return "Shien";
		case FFORM_JUYO:     return "Juyo";
		case FFORM_NIMAN:    return "Niman";
		case FFORM_SHIELD_BASH:	return "Shield Bash";
		case FFORM_GREAT_WEP:	return "Great Weapon Fighting";
		case FFORM_HALF_SWORD:	return "Half-sword style";
		case FFORM_POMMEL:	return "Pommeling style";
		case FFORM_KNI_SACRED:	return "Sacred style";
		case FFORM_KNI_RUNIC:	return "Runic style";
		case FFORM_KNI_ELDRITCH:return "Eldritch style";
		default:
			impossible("bad fform %d", fform);
	}
	return "None";
}

const char *
nameOfMentalEdge(edge)
int edge;
{
	switch (edge)
	{
		case GSTYLE_PENETRATE: return "Penetrating Edge of Hatred";
		case GSTYLE_COLD:  return "Cold Edge of Wrath";
		case GSTYLE_DEFENSE:   return "Defensive Edge of Leadership";
		case GSTYLE_ANTIMAGIC:    return "Anti-magic Edge of Serenity";
		case GSTYLE_RESONANT:  return "Resonant Edge of Fellowship";
		default:
			impossible("bad gstyle %d", edge);
	}
	return "None";
}

void
validateLightsaberForm()
{
	int i;
	for(i=FFORM_SHII_CHO; i <= FFORM_JUYO; i++)
		if(selectedFightingForm(i) && FightingFormSkillLevel(i) >= P_BASIC)
			return;

	if(uwep && uwep->oartifact == ART_INFINITY_S_MIRRORED_ARC)
		setFightingForm(FFORM_NIMAN);
	else setFightingForm(FFORM_SHII_CHO);
}

/* returns TRUE if fform is blocked by currently worn armor */
boolean
blockedFightingForm(fform)
int fform;
{
	switch (fform) {
		/* always available */
		case NO_FFORM:
			return FALSE;
		case FFORM_SHII_CHO:
		case FFORM_KNI_SACRED:
			return (FightingFormSkillLevel(fform) <= P_ISRESTRICTED);
		/* affected by spell success rate, handled elsewhere */
		case FFORM_KNI_ELDRITCH:
			return FALSE;
		/* blocked by heavy armor */
		case FFORM_MAKASHI:
		case FFORM_SORESU:
		case FFORM_ATARU:
			return (uarm && !(is_light_armor(uarm) || is_medium_armor(uarm)));
		/* blocked by medium armor */
		case FFORM_DJEM_SO:
		case FFORM_SHIEN:
		case FFORM_JUYO:
			return (uarm && !(is_light_armor(uarm)));
		/* blocked by metal armor */
		case FFORM_NIMAN:
			return (uarm && (metal_blocks_spellcasting(uarm)));
		/* requires longsword and free hand */
		case FFORM_HALF_SWORD:
			return !(uwep && uwep->otyp == LONG_SWORD && !uarms && !(u.twoweap && !bimanual(uwep, youracedata)));
		/* require longsword*/
		case FFORM_POMMEL:
			return !(uwep && uwep->otyp == LONG_SWORD);
		/* require longsword*/
		case FFORM_KNI_RUNIC:
			return !(uwep && uwep->otyp == LONG_SWORD && FightingFormSkillLevel(fform) > P_ISRESTRICTED);
		/* requires shield */
		case FFORM_SHIELD_BASH:
			return (!uarms);
		/* requires two-handed weapon */
		case FFORM_GREAT_WEP:
			return !(uwep && (bimanual(uwep, youracedata) || bimanual_mod(uwep, &youmonst) > 1));
		default:
			impossible("Attempting to get blockage of fighting form number %d?", fform);
			break;
	}
	return FALSE;
}

boolean
blockedMentalEdge(edge)
int edge;
{
	boolean ok = FALSE;
    const struct artifact *oart = (struct artifact *) 0;
	if(uwep){
		oart = get_artifact(uwep);
		if(oart && (oart->inv_prop == GITH_ART || oart->inv_prop == AMALGUM_ART))
			ok = TRUE;
	}
	if(uswapwep){
		oart = get_artifact(uswapwep);
		if(oart && (oart->inv_prop == GITH_ART || oart->inv_prop == AMALGUM_ART))
			ok = TRUE;
	}
	if(!ok)
		return TRUE;

	switch(edge){
		case GSTYLE_PENETRATE:
			return u.usanity > 50 || u.ulevel < 14;
		break;
		case GSTYLE_COLD:
			return u.usanity > 50 || u.ulevel < 14 || u.uinsight < 9;
		break;
		case GSTYLE_DEFENSE:
			return u.usanity < 50 || u.ulevel < 14;
		break;
		case GSTYLE_ANTIMAGIC:
			return u.usanity < 50 || u.ulevel < 14;
		break;
		case GSTYLE_RESONANT:
			return u.usanity < 50 || u.ulevel < 30 || u.uinsight < 81;
		break;
		default:
			impossible("Attempting to get blockage of mental edge number %d?", edge);
		break;
	}
	return TRUE; // Should never be reached
}

#endif /* OVL2 */

/** Returns the hitpoints of your current form. */
int
uhp()
{
    return (Upolyd ? u.mh : u.uhp);
}

/** Returns the maximal hitpoints of your current form. */
int
uhpmax()
{
    return (Upolyd ? u.mhmax : u.uhpmax);
}

void
check_brainlessness()
{
	if (ABASE(A_INT) <= 3) {
		int lifesaved = 0;
		struct obj *wore_amulet = uamul;

		while (1) {
			/* avoid looping on "die(y/n)?" */
			if (lifesaved && (discover || wizard)) {
				if (wore_amulet && !uamul) {
					/* used up AMULET_OF_LIFE_SAVING; still
					subject to dying from brainlessness */
					wore_amulet = 0;
				}
				else {
					/* explicitly chose not to die;
					arbitrarily boost intelligence */
					ABASE(A_INT) = ATTRMIN(A_INT) + 2;
					You_feel("like a scarecrow.");
					/* break deathloop */
					break;
				}
			}
			if (lifesaved)
				pline("Unfortunately your brain is still gone.");
			else
				Your("last thought fades away.");
			killer = "brainlessness";
			killer_format = KILLED_BY;
			done(DIED);
			lifesaved++;
		}
	}
}

boolean
change_hod(delta)
int delta;
{
	if(delta < 0 || !Infuture){
		u.hod += delta;
		return TRUE;
	}
	if(u.hod<0) u.hod = 0;
	return FALSE;
}

boolean
change_chokhmah(delta)
int delta;
{
	if(delta < 0 || !Infuture){
		u.chokhmah += delta;
		return TRUE;
	}
	return FALSE;
}

boolean
change_gevurah(delta)
int delta;
{
	if(delta < 0 || !Infuture){
		u.gevurah += delta;
		return TRUE;
	}
	return FALSE;
}

boolean
change_keter(delta)
int delta;
{
	if(delta < 0 || !Infuture){
		u.keter += delta;
		return TRUE;
	}
	return FALSE;
}
/*attrib.c*/
