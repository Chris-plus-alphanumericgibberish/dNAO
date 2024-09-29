/*	SCCS Id: @(#)monmove.c	3.4	2002/04/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "mfndpos.h"
#include "artifact.h"
#include "xhity.h"

extern boolean notonhead;
extern struct attack noattack;

#ifdef OVL0

STATIC_DCL int FDECL(disturb,(struct monst *));
STATIC_DCL void FDECL(distfleeck,(struct monst *,int *,int *,int *));
STATIC_DCL int FDECL(m_arrival, (struct monst *));
STATIC_DCL void FDECL(watch_on_duty,(struct monst *));
void FDECL(ford_rises,(struct monst *));
boolean FDECL(check_shore,(int, int));

#endif /* OVL0 */
#ifdef OVLB

boolean /* TRUE : mtmp died */
mb_trapped(mtmp)
register struct monst *mtmp;
{
	if (flags.verbose) {
	    if (cansee(mtmp->mx, mtmp->my))
		pline("KABOOM!!  You see a door explode.");
	    else if (flags.soundok)
		You_hear("a distant explosion.");
	}
	wake_nearto_noisy(mtmp->mx, mtmp->my, 7*7);
	mtmp->mstun = 1;
	mtmp->mhp -= rnd(15);
	if(mtmp->mhp <= 0) {
		mondied(mtmp);
		if (mtmp->mhp > 0) /* lifesaved */
			return(FALSE);
		else
			return(TRUE);
	}
	return(FALSE);
}

#endif /* OVLB */
#ifdef OVL0

STATIC_OVL void
watch_on_duty(mtmp)
register struct monst *mtmp;
{
	int	x, y;

	if (mtmp->mpeaceful && !mtmp->mtame && in_town(u.ux + u.dx, u.uy + u.dy) &&
		!is_blind(mtmp) && m_canseeu(mtmp) && !rn2(3)) {

		//ifdef CONVICT
		if (Role_if(PM_CONVICT) && !Upolyd && !(ublindf && ublindf->otyp != LENSES)) {
			verbalize("%s yells: Hey!  You are the one from the wanted poster!",
				Amonnam(mtmp));
			(void)angry_guards(!(flags.soundok));
			stop_occupation();
			return;
		}
		//endif /* CONVICT */
		if ((
			(forcing_door(&x, &y)) ||
			(picking_lock(&x, &y) && (levl[x][y].doormask & D_LOCKED))
			) &&
			IS_DOOR(levl[x][y].typ)
			) {
			if (couldsee(mtmp->mx, mtmp->my)) {
				pline("%s yells:", Amonnam(mtmp));
				if (levl[x][y].looted & D_WARNED) {
					verbalize("Halt, thief!  You're under arrest!");
					(void)angry_guards(!(flags.soundok));
				}
				else {
					if (picking_lock(&x, &y))
						verbalize("Hey, stop picking that lock!");
					else
						verbalize("Hey, stop forcing that door!");
					levl[x][y].looted |= D_WARNED;
				}
				stop_occupation();
			}
		}
		else if (is_digging()) {
			/* chewing, wand/spell of digging are checked elsewhere */
			watch_dig(mtmp, digging.pos.x, digging.pos.y, FALSE);
		}
	}
}

#endif /* OVL0 */
#ifdef OVL1

int
dochugw(mtmp)
	register struct monst *mtmp;
{
	register int x = mtmp->mx, y = mtmp->my;
	boolean already_saw_mon = !occupation ? 0 : canspotmon(mtmp);
	int rd = dochug(mtmp);
#if 0
	/* part of the original warning code which was replaced in 3.3.1 */
	int dd;

	if(Warning && !rd && !mtmp->mpeaceful &&
			(dd = distu(mtmp->mx,mtmp->my)) < distu(x,y) &&
			dd < 100 && !canseemon(mtmp)) {
	    /* Note: this assumes we only want to warn against the monster to
	     * which the weapon does extra damage, as there is no "monster
	     * which the weapon warns against" field.
	     */
	    if (spec_ability(uwep, SPFX_WARN) && spec_dbon(uwep, &youmonst, mtmp, 1, (int *)0, (int *)0))
		warnlevel = 100;
	    else if ((int) (mtmp->m_lev / 4) > warnlevel)
		warnlevel = (mtmp->m_lev / 4);
	}
#endif /* 0 */

	/* a similar check is in monster_nearby() in hack.c */
	/* check whether hero notices monster and stops current activity */
	if (occupation && !rd && !Confusion &&
	    (!mtmp->mpeaceful || Hallucination) &&
	    /* it's close enough to be a threat */
	    distu(mtmp->mx,mtmp->my) <= (BOLT_LIM+1)*(BOLT_LIM+1) &&
	    /* and either couldn't see it before, or it was too far away */
	    (!already_saw_mon || !couldsee(x,y) ||
		distu(x,y) > (BOLT_LIM+1)*(BOLT_LIM+1)) &&
	    /* can see it now, or sense it and would normally see it */
	    (canseemon(mtmp) ||
		(sensemon(mtmp) && couldsee(mtmp->mx,mtmp->my))) &&
	    mtmp->mcanmove && mtmp->mnotlaugh &&
		(!(Xray_vision||(u.sealsActive&SEAL_ORTHOS)) || clear_path(u.ux, u.uy, mtmp->mx, mtmp->my)) &&
	    !noattacks(mtmp->data) && !onscary(u.ux, u.uy, mtmp))
		stop_occupation();

	return(rd);
}

#endif /* OVL1 */
#ifdef OVL2

boolean
onscary(x, y, mtmp)
int x, y;
struct monst *mtmp;
{
	struct obj *alignedfearobj = fear_arti_at(x, y);
	int wardAt = ward_at(x,y);
	struct monst *mat = m_at(x,y);
	
	//The PC isn't affected by wards etc.
	if(mtmp == &youmonst)
		return FALSE;
	
	/* Nitocris's wrappings are especially warded against Nyarlathotep, and accidently work vs. summons generally */
	if(u.ux == x && u.uy == y && !mtmp->mpeaceful && (get_mx(mtmp, MX_ESUM) || is_mask_of_nyarlathotep(mtmp->data)) && uarmc && uarmc->oartifact == ART_SPELL_WARDED_WRAPPINGS_OF_)
		return TRUE;
	if(u.ux == x && u.uy == y && !mtmp->mpeaceful && Withering_stake && quest_status.time_doing_quest < UH_QUEST_TIME_2 && !quest_status.moon_close && (mtmp->data->mflagsa&MA_ANIMAL || mtmp->data->mflagsa&MA_DEMIHUMAN || mtmp->data->mflagsa&MA_WERE) && !resists_fire(mtmp))
		return TRUE;
	if(!no_upos(mtmp) && mtmp->mux == x && mtmp->muy == y && !mtmp->mpeaceful && (get_mx(mtmp, MX_ESUM) || is_mask_of_nyarlathotep(mtmp->data)) && uarmc && uarmc->oartifact == ART_SPELL_WARDED_WRAPPINGS_OF_)
		return TRUE;
	if(mat && (get_mx(mtmp, MX_ESUM) || is_mask_of_nyarlathotep(mtmp->data)) && which_armor(mat, W_ARMC) && which_armor(mat, W_ARMC)->oartifact == ART_SPELL_WARDED_WRAPPINGS_OF_)
		return TRUE;

	if(no_upos(mtmp) && !mat)
		return FALSE;
	
	if(mat && rn2(100) < (80-mtmp->m_lev)){
		if(mtmp->msanctity && gender(mat) == 1){
			return TRUE;
		}
		if(mtmp->marachno && humanoid_upperbody(mat->data) && gender(mat) == 1){
			return TRUE;
		}
		if(mtmp->margent && gender(mat) == 0){
			return TRUE;
		}
	}
	if(!no_upos(mtmp) && mtmp->mux == x && mtmp->muy == y && rn2(100) < (80-mtmp->m_lev)){
		char curgen = is_neuter(youracedata) ? 2 : Upolyd ? u.mfemale : flags.female;
		if(mtmp->msanctity && curgen == 1){
			return TRUE;
		}
		if(mtmp->marachno && humanoid_upperbody(youracedata) && curgen == 1){
			return TRUE;
		}
		if(mtmp->margent && curgen == 0){
			return TRUE;
		}
	}

	return (boolean)(
				((
					sobj_at(SCR_SCARE_MONSTER, x, y)
				 || (alignedfearobj && !touch_artifact(alignedfearobj, mtmp, FALSE))
				 ) && scaryItem(mtmp)
				)
			 || (u.umonnum == PM_GHOUL && x == mtmp->mux && y == mtmp->muy && mtmp->mtyp == PM_GUG)
			 || (mat && mat->mtyp == PM_GHOUL && mtmp->mtyp == PM_GUG)
			 || (mat && mat->mtyp == PM_MOVANIC_DEVA && (is_animal(mtmp->data) || is_plant(mtmp->data)))
			 || (wardAt == HEPTAGRAM && scaryHept(num_wards_at(x,y), mtmp))
			 || (wardAt == GORGONEION && scaryGorg(num_wards_at(x,y), mtmp))
			 || (wardAt == CIRCLE_OF_ACHERON && scaryCircle(num_wards_at(x,y), mtmp))
			 || (wardAt == PENTAGRAM && scaryPent(num_wards_at(x,y), mtmp))
			 || (wardAt == HEXAGRAM && scaryHex(num_wards_at(x,y), mtmp))
			 || (wardAt == HAMSA && scaryHam(num_wards_at(x,y), mtmp))
			 || (wardAt == ELDER_SIGN && scarySign(num_wards_at(x,y), mtmp))
			 || (sobj_at(DIMENSIONAL_LOCK, x, y) && scarySign(1, mtmp))
			 || (wardAt == ELDER_ELEMENTAL_EYE && scaryEye(num_wards_at(x,y), mtmp))
			 || (wardAt == SIGN_OF_THE_SCION_QUEEN && scaryQueen(num_wards_at(x,y), mtmp))
			 || (wardAt == CARTOUCHE_OF_THE_CAT_LORD && scaryCat(num_wards_at(x,y), mtmp))
			 || (wardAt == WINGS_OF_GARUDA && scaryWings(num_wards_at(x,y), mtmp))
			 || (wardAt == YELLOW_SIGN && scaryYellow(num_wards_at(x,y), mtmp))
			 || (scaryElb(mtmp) && sengr_at("Elbereth", x, y))
			 || (scaryLol(mtmp) && sengr_at("Lolth", x, y) && (mtmp->m_lev < u.ulevel || u.ualign.record-- > 0))
			 || (scaryTou(mtmp) && toustefna_at(x,y))
			 || (scaryDre(mtmp) && dreprun_at(x,y))
			 || (scaryVei(mtmp) && veioistafur_at(x,y))
			 || (scaryThj(mtmp) && thjofastafur_at(x,y))
			 || (is_vampire(mtmp->data) && IS_ALTAR(levl[x][y].typ))
			);
}

boolean
scaryWings(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	if(standardUnwardable(mtmp))
		return FALSE;
	return 	wingWarded(mtmp->data);
}

boolean
scaryCat(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	if(standardUnwardable(mtmp) || catWardInactive)
		return FALSE;
	else if(mtmp->data->mlet == S_FELINE){ /* && mvitals[PM_KITTEN].died == 0*/
		mtmp->mpeaceful = TRUE;
		mtmp->mhp = mtmp->mhpmax;
		newsym(mtmp->mx, mtmp->my);
		return FALSE;
	}
	return 	catWarded(mtmp->data);
}

boolean
scaryTou(mtmp)
struct monst *mtmp;
{
	if(standardUnwardable(mtmp))
			return FALSE;
	return 	touWarded(mtmp->data);
}

boolean
scaryDre(mtmp)
struct monst *mtmp;
{
	if(standardUnwardable(mtmp))
		return FALSE;
	return 	dreWarded(mtmp->data);
}
boolean
scaryVei(mtmp)
struct monst *mtmp;
{
	if(standardUnwardable(mtmp))
		return FALSE;
	return 	veiWarded(mtmp->data);
}
boolean
scaryThj(mtmp)
struct monst *mtmp;
{
	if(standardUnwardable(mtmp))
		return FALSE;
	return 	thjWarded(mtmp->data);
}

boolean
scaryQueen(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	if(standardUnwardable(mtmp))
		return FALSE;
	return 	queenWarded(mtmp->data);
}

boolean
scaryEye(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	if(standardUnwardable(mtmp) ||
			(mtmp->mtyp == PM_CHAOS && rn2(2)) ||
			(mtmp->mtyp == PM_DEMOGORGON && rn2(3)) ||
			(mtmp->mtyp == PM_LAMASHTU && rn2(3)) ||
			(mtmp->mtyp == PM_ASMODEUS && rn2(9))
		) return FALSE;
	return ((mtmp->data->mflagsw&MW_ELDER_EYE_ELEM) || (complete >= 4 && (mtmp->data->mflagsw&MW_ELDER_EYE_ENERGY)) || (complete >= 7 && (mtmp->data->mflagsw&MW_ELDER_EYE_PLANES)));
}

boolean
scarySign(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	if(standardUnwardable(mtmp) || (mtmp->mtyp == PM_DEMOGORGON && rn2(3)))
		return FALSE;
	return ((mtmp->data->mflagsw&MW_ELDER_SIGN) || (complete >= 6 && (mtmp->data->mflagsw&MW_EYE_OF_YGG)));
}

boolean
scaryHam(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	else if(standardUnwardable(mtmp))
		return FALSE;
	return 	hamWarded(mtmp->data);
}

boolean
scaryHex(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	else if(standardUnwardable(mtmp) || mtmp->mpeaceful || 
			(mtmp->mtyp == PM_CHAOS && rn2(2)) ||
			(mtmp->mtyp == PM_DEMOGORGON && rn2(3)) ||
			(mtmp->mtyp == PM_LAMASHTU && rn2(3)) ||
			(mtmp->mtyp == PM_ASMODEUS && complete <= d(1,8))
		) return FALSE;
	return 	hexWarded(mtmp->data);
}

boolean
scaryPent(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	else if(standardUnwardable(mtmp) || 
			(mtmp->mtyp == PM_CHAOS && rn2(2)) ||
			(mtmp->mtyp == PM_DEMOGORGON && rn2(3)) ||
			(mtmp->mtyp == PM_LAMASHTU && rn2(3)) ||
			(mtmp->mtyp == PM_ASMODEUS && !rn2(9))
		) return FALSE;
	return 	pentWarded(mtmp->data);
}

boolean
scaryCircle(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	else if(standardUnwardable(mtmp))
		return FALSE;
	return circleWarded(mtmp->data);
}

boolean
scaryGorg(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	else if(gorgUnwardable(mtmp) 
		|| (mtmp->mtyp == PM_CHAOS && rn2(2))
		|| (mtmp->mtyp == PM_DEMOGORGON && rn2(3))
		|| (mtmp->mtyp == PM_LAMASHTU && rn2(3))
		|| (mtmp->mtyp == PM_ASMODEUS && rn2(9))
	)
		return FALSE;
	else if(mtmp->data->mlet == S_SNAKE && mvitals[PM_MEDUSA].killed == 0){
		mtmp->mpeaceful = TRUE;
		mtmp->mhp = mtmp->mhpmax;
		newsym(mtmp->mx, mtmp->my);
		return FALSE;
	}
	return d(1,100) <= 33*complete && gorgWarded(mtmp->data);
}

boolean
scaryHept(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	else if(heptUnwardable(mtmp) ||
			(mtmp->mtyp == PM_ELDER_PRIEST && complete <= d(2,4)+2) ||
			(mtmp->mtyp == PM_GREAT_CTHULHU && complete <= d(2,4)+2) ||
			(mtmp->mtyp == PM_CHAOS && rn2(2)) ||
			(mtmp->mtyp == PM_DEMOGORGON && rn2(3)) ||
			(mtmp->mtyp == PM_LAMASHTU && rn2(3)) ||
			(mtmp->mtyp == PM_ASMODEUS && complete <= d(1,10))
		) return FALSE;
	return heptWarded(mtmp->data);
	
}

boolean
scaryYellow(complete, mtmp)
int complete;
struct monst *mtmp;
{
	if(complete <= 0) return FALSE;
	else if(yellowUnwardable(mtmp))
		return FALSE;
	if(yellowWarded(mtmp->data)){
			mtmp->mcrazed = 1;
			return !rn2(10);
	}
	return FALSE;
}

boolean
scaryItem(mtmp)
struct monst *mtmp;
{
	if (mtmp->isshk || mtmp->isgd || mtmp->iswiz || is_blind(mtmp) ||
	    mtmp->mpeaceful || mtmp->data->mlet == S_HUMAN || 
	    is_lminion(mtmp) || mtmp->mtyp == PM_ANGEL ||
	    is_rider(mtmp->data) || mtmp->mtyp == PM_MINOTAUR)
		return(FALSE);
	return (boolean) (mtmp->mtyp != PM_ELDER_PRIEST) &&
					(mtmp->mtyp != PM_GREAT_CTHULHU) &&
					(mtmp->mtyp != PM_CHOKHMAH_SEPHIRAH) &&
					(mtmp->mtyp != PM_CHAOS || rn2(2)) &&
					(mtmp->mtyp != PM_DEMOGORGON || rn2(3)) &&
					(mtmp->mtyp != PM_LAMASHTU || rn2(3)) &&
					(mtmp->mtyp != PM_ASMODEUS);
}

boolean
scaryLol(mtmp)
struct monst *mtmp;
{
  if(Infuture) return FALSE;
  if(u.ualign.type == A_VOID) return FALSE;
  if(LOLTH_HIGH_POWER){
	if (mtmp->isshk || mtmp->isgd || mtmp->iswiz || is_blind(mtmp) ||
	    mtmp->mpeaceful || mtmp->data->mlet == S_HUMAN || 
	    (is_rider(mtmp->data))
	)
		return(FALSE);
	return (boolean) (mtmp->mtyp != PM_ELDER_PRIEST) &&
					(mtmp->mtyp != PM_GREAT_CTHULHU) &&
					(mtmp->mtyp != PM_CHOKHMAH_SEPHIRAH) &&
					(mtmp->mtyp != PM_CHAOS || rn2(2)) &&
					(mtmp->mtyp != PM_DEMOGORGON || !rn2(3)) &&
					(mtmp->mtyp != PM_LAMASHTU) &&
					(mtmp->mtyp != PM_ASMODEUS || !rn2(9));
  } else return(FALSE);
}

boolean
scaryElb(mtmp)
struct monst *mtmp;
{
  if(Infuture) return FALSE;
  if(ELBERETH_HIGH_POWER){
	if (mtmp->isshk || mtmp->isgd || mtmp->iswiz || is_blind(mtmp) ||
	    mtmp->mpeaceful || mtmp->data->mlet == S_HUMAN || 
	    is_lminion(mtmp) || mtmp->mtyp == PM_ANGEL ||
	    (is_rider(mtmp->data) && !(mtmp->mtyp == PM_NAZGUL)) || 
		mtmp->mtyp == PM_MINOTAUR)
		return(FALSE);
	return (boolean) (mtmp->mtyp != PM_ELDER_PRIEST) &&
					(mtmp->mtyp != PM_GREAT_CTHULHU) &&
					(mtmp->mtyp != PM_CHOKHMAH_SEPHIRAH) &&
					(mtmp->mtyp != PM_CHAOS || rn2(2)) &&
					(mtmp->mtyp != PM_DEMOGORGON || !rn2(3)) &&
					(mtmp->mtyp != PM_LAMASHTU) &&
					(mtmp->mtyp != PM_ASMODEUS || !rn2(9));
  }
  else{
	if (mtmp->isshk || mtmp->isgd || mtmp->iswiz || is_blind(mtmp) ||
	    mtmp->mpeaceful || mtmp->data->mlet == S_HUMAN || 
	    is_lminion(mtmp) || mtmp->mtyp == PM_ANGEL ||
	    (is_rider(mtmp->data) && !(mtmp->mtyp == PM_NAZGUL)) || 
		mtmp->mtyp == PM_MINOTAUR)
		return(FALSE);
	return (boolean) mtmp->data->mlet == S_ORC || mtmp->data->mlet == S_OGRE 
				|| mtmp->data->mlet == S_TROLL || mtmp->mtyp == PM_ELVEN_WRAITH || mtmp->mtyp == PM_NAZGUL;
  }
}
#endif /* OVL2 */
#ifdef OVL0

/* regenerate lost hit points */
void
mon_regen(mon, digest_meal)
struct monst *mon;
boolean digest_meal;
{
	if(mon->mtrapped && t_at(mon->mx, mon->my) && t_at(mon->mx, mon->my)->ttyp == VIVI_TRAP)
		return;
	
	if(mon->mtame && u.ufirst_life && mon->mhp < mon->mhpmax)
		mon->mhp++;
		
	if(is_alabaster_mummy(mon->data) 
		&& mon->mvar_syllable == SYLLABLE_OF_LIFE__HOON
	){
		mon->mhp += 10;
	}
	
	if (mon->mspec_used) mon->mspec_used--;

	if(mon->mspec_used && uring_art(ART_LOMYA)){
		mon->mspec_used--;
	}
	if (digest_meal) {
	    if (mon->meating) mon->meating--;
	}

	if(mon->entangled_oid){
		/* Razor wire deals damage */
		if(mon->entangled_otyp == RAZOR_WIRE){
			int beat;
			mon->mhp -= rnd(6);
			if(hates_silver(mon->data) && entangle_material(mon, SILVER))
				mon->mhp -= rnd(20);
			if(hates_iron(mon->data) && (entangle_material(mon, IRON) || entangle_material(mon, GREEN_STEEL)))
				mon->mhp -= rnd(mon->m_lev);
			if(hates_unholy_mon(mon) && entangle_material(mon, GREEN_STEEL))
				mon->mhp -= d(2,9);
			beat = entangle_beatitude(mon, -1);
			if(hates_unholy_mon(mon) && beat)
				mon->mhp -= beat == 2 ? d(2,9) : rnd(9);
			beat = entangle_beatitude(mon, 0);
			if(hates_unblessed_mon(mon) && beat)
				mon->mhp -= beat == 2 ? d(2,8) : rnd(8);
			beat = entangle_beatitude(mon, 1);
			if(hates_holy_mon(mon) && beat)
				mon->mhp -= beat == 2 ? rnd(20) : rnd(4);
			if(mon->mhp <= 0){
				mon->mhp = 0;
				if(canspotmon(mon))
					pline("%s is sliced to ribbons in %s struggles!", Monnam(mon), hisherits(mon));
				mondied(mon);
				if(DEADMONSTER(mon))
					return; //Didn't lifesave
			}
		}
		entangle_effects(mon);
	}
	/* Clouds on Lolth's level deal damage */
	if(Is_lolth_level(&u.uz) && levl[mon->mx][mon->my].typ == CLOUD){
		if (!(nonliving(mon->data) || breathless_mon(mon))){
			if (haseyes(mon->data) && mon->mcansee){
				mon->mblinded = 1;
				mon->mcansee = 0;
			}
			if (!resists_poison(mon)) {
				pline("%s coughs!", Monnam(mon));
				mon->mhp -= (d(3,8) + ((Amphibious && !flaming(youracedata)) ? 0 : rnd(6)));
			} else if (!(amphibious_mon(mon) && !flaming(youracedata))){
				/* NB: Amphibious includes Breathless */
				if (!(amphibious_mon(mon) && !flaming(youracedata))) mon->mhp -= rnd(6);
			}
			if(mon->mhp <= 0){
				monkilled(mon, "gas cloud", AD_DRST);
				if(mon->mhp <= 0) return;	/* not lifesaved */
			}
		} else {
			/* NB: Amphibious includes Breathless */
			mon->mhp -= rnd(6);
			if(mon->mhp <= 0){
				monkilled(mon, "suffocating cloud", AD_DRST);
				if(mon->mhp <= 0) return;	/* not lifesaved */
			}
		}
	}
	/*Cthulhu's mind blast*/
	if(mon->mdreams && (mon->msleeping || !rn2(70)) && !rn2(5)){
		int dmg;
		int nd = 1;
		if(on_level(&rlyeh_level,&u.uz))
			nd = 5;
		dmg = d(nd,15);
		if(Half_spel(mon))
			dmg = (dmg+1) / 2;
		if(m_losehp(mon, dmg, FALSE, "fevered dreams"))
			return; //Died.
		if(!mon->msleeping && !resists_sleep(mon)){
			mon->msleeping = 1;
			slept_monst(mon);
		}
	}
	
	//Degeneration cases block normal healing. Only one will take effect (bug?).
	/*Blib's image degrades from loss of artifact*/
	if(mon->mtyp == PM_BLIBDOOLPOOLP__GRAVEN_INTO_FLESH && !mon_has_arti(mon, 0) && quest_status.touched_artifact && mon->mhp > 1){
		mon->mhp -= 1;
		return;
	}
	if(mon->mtyp == PM_CYCLOPS && !mon_has_arti(mon, 0) && mon->mhp > 1){
		mon->mhp -= 1;
		return;
	}
	/*Degen from drowning in blood*/
	if(mon->mbdrown > 0){
		mon->mbdrown--;
		water_damage(mon->minvent, FALSE, FALSE, WD_BLOOD, mon);
		mon->mhp -= 99;
		if(mon->mhp <= 0){
			pline("%s drowns in blood!", Monnam(mon));
			mondied(mon);
		}
		else if(!resist(mon, RING_CLASS, 0, NOTELL)){
			mon->mberserk = 1;
		}
		return;
	}
	/*The Changed degenerate due to damage*/
	if(mon->mhp < mon->mhpmax/2 && is_changed_mtyp(mon->mtyp)){
		mon->mhp -= 1;
		create_gas_cloud(mon->mx+rn2(3)-1, mon->my+rn2(3)-1, rnd(3), rnd(3)+1, FALSE);
		if(mon->mhp <= 0){
			mondied(mon);
		}
		return;
	}
	/*Invidiaks degenerate due to light*/
	if(mon->mtyp == PM_INVIDIAK && !isdark(mon->mx, mon->my)){
		mon->mhp -= 1;
		if(mon->mhp <= 0){
			mondied(mon);
		}
		return;
	}

	//Normal healing cases
	if(mon->mtyp == PM_ALIDER){
		struct monst *mtmp;
		int healup = 0;
		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon){
			if(!DEADMONSTER(mtmp) && (mtmp != mon) && (mtmp->mpeaceful == mon->mpeaceful)){
				if(mtmp->mtyp == PM_MYRKALFAR_MATRON)
					healup += 3;
				else if((!mon->mcan && free_android(mtmp->data))
					|| mtmp->mtyp == PM_MYRKALFR || mtmp->mtyp == PM_MYRKALFAR_WARRIOR
					|| mtmp->mtyp == PM_ARCADIAN_AVENGER || mtmp->mtyp == PM_PORO_AULON
					|| mtmp->mfaction == EILISTRAEE_SYMBOL
					|| (mtmp->mtyp == PM_ELVENKING && mtmp->mfaction == QUEST_FACTION)
					|| (is_drow(mtmp->data) && mtmp->mtame && mon->mtame)
				)
					healup += 2;
				else if(mtmp->mfaction == LAST_BASTION_SYMBOL || (mtmp->mtame && mon->mtame))
					healup += 1;
			}
		}
		if(mon->mtame || (Race_if(PM_MYRKALFR) && Role_if(PM_ANACHRONONAUT)))
			healup += (!mon->mcan && Race_if(PM_ANDROID)) ? 3 : Race_if(PM_MYRKALFR) ? 2 : 1;
		if(mon->mcan) healup /= 2;
		if(healup){
			set_mcan(mon, FALSE);
			mon->mdoubt = FALSE;
			mon->mhp += healup;
		}
		else {
			if(!rn2(8)) set_mcan(mon, TRUE);
			if(mon->mcan) mon->mdoubt = TRUE;
		}
	}
	if(mon->mhp < mon->mhpmax){
		int perX = 0;
		if(is_uvuudaum(mon->data)){
			perX += 25*HEALCYCLE; //Fast healing
		} else {
			perX += mon->m_lev;
		}
		perX = hd_size(mon->data)*perX/8; //Adjust so that large and small monsters heal to full at about the same rate.
		//Worn Vilya bonus ranges from (penalty) to +7 HP per 10 turns
		if(uring_art(ART_VILYA)){
			perX += heal_vilya()*HEALCYCLE/10;
		}
		perX -= mon_healing_penalty(mon);
		if(!nonliving(mon->data)){
			if(perX < 1)
				perX = 1;
			if (mon->mhp < mon->mhpmax){
				//recover 1/HEALCYCLEth hp per turn:
				mon->mhp += perX/HEALCYCLE;
				//Now deal with any remainder
				if(((moves)*(perX%HEALCYCLE))/HEALCYCLE > ((moves-1)*(perX%HEALCYCLE))/HEALCYCLE)
					mon->mhp += 1;
			}
		}
		if(mon_resistance(mon,REGENERATION))
			mon->mhp+=1;
		if(mon->mtyp == PM_TWIN_SIBLING && check_mutation(CRAWLING_FLESH))
			mon->mhp+=1;
		struct obj *arm = which_armor(mon, W_ARM);
		// regeneration tech
		if (arm && arm->otyp == IMPERIAL_ELVEN_ARMOR && check_imp_mod(arm, IEA_FAST_HEAL)){
			mon->mhp+=1;
		}
		if(uwep && uwep->oartifact == ART_SINGING_SWORD && uwep->osinging == OSING_HEALING && !mindless_mon(mon) && !is_deaf(mon) && mon->mtame)
			mon->mhp += 1;
	}
	mon->mhp = min(mon->mhpmax, mon->mhp);
	
}

/*
 * Possibly awaken the given monster.  Return a 1 if the monster has been
 * jolted awake.
 */
STATIC_OVL int
disturb(mtmp)
	register struct monst *mtmp;
{
	/*
	 * + Ettins are hard to surprise.
	 * + Nymphs, jabberwocks, and leprechauns do not easily wake up.
	 *
	 * Wake up if:
	 *	in direct LOS						AND
	 *	within 10 squares					AND
	 *	not stealthy or (mon is an ettin and 9/10)		AND
	 *	(mon is not a nymph, jabberwock, or leprechaun) or 1/50	AND
	 *	Aggravate or mon is (dog or human) or
	 *	    (1/7 and mon is not mimicing furniture or object)
	 */
	if(couldsee(mtmp->mx,mtmp->my) &&
		distu(mtmp->mx,mtmp->my) <= 100 &&
		(!Stealth || (mtmp->mtyp == PM_ETTIN && rn2(10))) &&
		(!(mtmp->data->mlet == S_NYMPH
			|| mtmp->mtyp == PM_JABBERWOCK
			|| mtmp->data->mlet == S_LEPRECHAUN) || !rn2(50)) &&
		(Aggravate_monster
			|| (mtmp->data->mlet == S_DOG ||
				mtmp->data->mlet == S_HUMAN)
			|| (sensitive_ears(mtmp->data) && !is_deaf(mtmp))
			|| (!rn2(7) && mtmp->m_ap_type != M_AP_FURNITURE &&
				mtmp->m_ap_type != M_AP_OBJECT) )) {
		mtmp->msleeping = 0;
		return(1);
	}
	return(0);
}

/* monster begins fleeing for the specified time, 0 means untimed flee
 * if first, only adds fleetime if monster isn't already fleeing
 * if fleemsg, prints a message about new flight, otherwise, caller should */
void
monflee(mtmp, fleetime, first, fleemsg)
struct monst *mtmp;
int fleetime;
boolean first;
boolean fleemsg;
{
	int j;
	if (u.ustuck == mtmp) {
	    if (u.uswallow)
		expels(mtmp, mtmp->data, TRUE);
	    else if (!sticks(&youmonst)) {
		unstuck(mtmp);	/* monster lets go when fleeing */
		You("get released!");
	    }
	}
	/*Clear track so they can actually move away in narrow spaces*/
	for (j = 0; j < MTSZ; j++){
		mtmp->mtrack[j].x = 0;
		mtmp->mtrack[j].y = 0;
	}

	if(mtmp->mtyp == PM_VROCK){
		struct monst *tmpm;
		if(!(mtmp->mspec_used || mtmp->mcan)){
			pline("%s screeches.", Monnam(mtmp));
			mtmp->mspec_used = 10;
			for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
				if(tmpm != mtmp && !DEADMONSTER(tmpm)){
					if(tmpm->mpeaceful != mtmp->mpeaceful && !resist(tmpm, 0, 0, FALSE)){
						tmpm->mconf = 1;
					}
				}
			}
			if(!mtmp->mpeaceful){
				make_stunned(HStun + 10, TRUE);
			}
		}
	}
	
	if (!first || !mtmp->mflee) {
	    /* don't lose untimed scare */
	    if (!fleetime)
		mtmp->mfleetim = 0;
	    else if (!mtmp->mflee || mtmp->mfleetim) {
			fleetime += mtmp->mfleetim;
			/* ensure monster flees long enough to visibly stop fighting */
			if (fleetime == 1) fleetime++;
				mtmp->mfleetim = min(fleetime, 127);
	    }
		if( !mtmp->mflee && mtmp->mtyp == PM_GIANT_TURTLE){
		 mtmp->mcanmove=0;
		 // mtmp->mfrozen = mtmp->mfleetim;
		 if(canseemon(mtmp)) 
		   pline("%s hides in %s shell!",Monnam(mtmp),mhis(mtmp));
		} else if (!mtmp->mflee && fleemsg && canseemon(mtmp) && !mtmp->mfrozen) 
			mtmp->mtyp == PM_BANDERSNATCH ? pline("%s becomes frumious!", (Monnam(mtmp)))
												 : pline("%s turns to flee!", (Monnam(mtmp)));
	    mtmp->mflee = 1;
	}
}

STATIC_OVL void
distfleeck(mtmp,inrange,nearby,scared)
register struct monst *mtmp;
int *inrange, *nearby, *scared;
{
	int seescaryx, seescaryy;

	*inrange = !no_upos(mtmp) && (dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <=
							(BOLT_LIM * BOLT_LIM));
	*nearby = *inrange && monnear(mtmp, mtmp->mux, mtmp->muy);

	/* Note: if your image is displaced, the monster sees the Elbereth
	 * at your displaced position, thus never attacking your displaced
	 * position, but possibly attacking you by accident.  If you are
	 * invisible, it sees the Elbereth at your real position, thus never
	 * running into you by accident but possibly attacking the spot
	 * where it guesses you are.
	 */
	if (is_blind(mtmp) || (Invis && !mon_resistance(mtmp,SEE_INVIS))) {
		seescaryx = mtmp->mux;
		seescaryy = mtmp->muy;
	} else {
		seescaryx = u.ux;
		seescaryy = u.uy;
	}
	*scared = (*nearby && (onscary(seescaryx, seescaryy, mtmp) ||
			       (!mtmp->mpeaceful &&
				    in_your_sanctuary(mtmp, 0, 0))));
	
	if(*scared && get_mx(mtmp, MX_ESUM)) {
		abjure_summon(mtmp, timer_duration_remaining(get_timer(mtmp->timed, DESUMMON_MON))/2);
		/*Abjuration may desummon monster*/
		if(DEADMONSTER(mtmp))
			return;
	}
	
	if(mtmp->mtyp == PM_DAUGHTER_OF_BEDLAM && !rn2(20)) *scared = TRUE;
	else if(mtmp->mtyp == PM_CARCOSAN_COURTIER && *nearby && !mtmp->mflee && (u.uinsight < 25 || mtmp->m_id%2)) *scared = TRUE;
	else if(*nearby && !mtmp->mflee && fleetflee(mtmp->data) && (mtmp->data->mmove > youracedata->mmove || noattacks(mtmp->data))) *scared = TRUE;
	
	if(*scared) {
		if (rn2(7))
		    monflee(mtmp, rnd(10), TRUE, TRUE);
		else
		    monflee(mtmp, rnd(100), TRUE, TRUE);
	}

}

/* perform a special one-time action for a monster; returns -1 if nothing
   special happened, 0 if monster uses up its turn, 1 if monster is killed */
STATIC_OVL int
m_arrival(mon)
struct monst *mon;
{
	mon->mstrategy &= ~STRAT_ARRIVE;	/* always reset */

	return -1;
}

STATIC_OVL void
jrt_eladrin_spawn_equip(tmpm, mtyp)
struct monst *tmpm;
int mtyp;
{
	struct obj *otmp;
	otmp = mongets(tmpm, KHOPESH, MKOBJ_NOINIT);
	set_material_gm(otmp, COPPER);
	add_oprop(otmp, OPROP_HOLYW);
	if(mtyp != PM_TULANI_ELADRIN){
		otmp = mongets(tmpm, KHOPESH, MKOBJ_NOINIT);
		set_material_gm(otmp, COPPER);
		add_oprop(otmp, OPROP_HOLYW);
	}

	otmp = mongets(tmpm, ARCHAIC_HELM, MKOBJ_NOINIT);
	set_material_gm(otmp, COPPER);
	add_oprop(otmp, OPROP_HOLY);

	otmp = mongets(tmpm, WAISTCLOTH, MKOBJ_NOINIT);
	set_material_gm(otmp, CLOTH);
	add_oprop(otmp, OPROP_HOLY);
	otmp->obj_color = CLR_WHITE;

}

void
dracae_eladrin_spawn_equip(mtmp, mtyp)
struct monst *mtmp;
int mtyp;
{
	struct obj *otmp;
	int size = mons[mtyp].msize;
	if(mtyp != PM_UISCERRE_ELADRIN){
		otmp = mksobj(LONG_SWORD, MKOBJ_NOINIT);
		set_material_gm(otmp, DRAGON_HIDE);
		otmp->objsize = size;
		add_oprop(otmp, OPROP_ACIDW);
		fix_object(otmp);
		(void) mpickobj(mtmp, otmp);
	}
	if(mtyp == PM_COURE_ELADRIN || mtyp == PM_BRALANI_ELADRIN
	 || mtyp == PM_FIRRE_ELADRIN || mtyp == PM_GHAELE_ELADRIN
	 || mtyp == PM_GAE_ELADRIN || mtyp == PM_BRIGHID_ELADRIN
	 || mtyp == PM_CAILLEA_ELADRIN
	){
		//Note: 2/3 then 1/6 / 1/6 is the intended behavior
		if(rn2(3)){
			otmp = mksobj(SHORT_SWORD, MKOBJ_NOINIT);
			set_material_gm(otmp, DRAGON_HIDE);
			otmp->objsize = size;
			add_oprop(otmp, OPROP_ACIDW);
			fix_object(otmp);
			(void) mpickobj(mtmp, otmp);
		}
		else if(rn2(2)){
			otmp = mksobj(KITE_SHIELD, MKOBJ_NOINIT);
			set_material_gm(otmp, SHELL_MAT);
			otmp->objsize = size;
			fix_object(otmp);
			(void) mpickobj(mtmp, otmp);
		}
		//else nothing
	}
	else if(mtyp == PM_NOVIERE_ELADRIN || mtyp == PM_SHIERE_ELADRIN){
		otmp = mksobj(KITE_SHIELD, MKOBJ_NOINIT);
		set_material_gm(otmp, SHELL_MAT);
		otmp->objsize = size;
		fix_object(otmp);
		(void) mpickobj(mtmp, otmp);
	}
	//Tulani, Uiscerre

	int armors[] = {PLATE_MAIL, GAUNTLETS, ARMORED_BOOTS};
	for(int i = 0; i < SIZE(armors); i++){
		if(mtyp == PM_UISCERRE_ELADRIN && armors[i] == ARMORED_BOOTS)
			continue;
		otmp = mksobj(armors[i], NO_MKOBJ_FLAGS);
		set_material_gm(otmp, SHELL_MAT);
		otmp->objsize = size;
		set_obj_shape(otmp, mtmp->data->mflagsb);
		fix_object(otmp);
		(void) mpickobj(mtmp, otmp);
	}
}

/* returns 1 if monster died moving, 0 otherwise */
/* The whole dochugw/m_move/distfleeck/mfndpos section is serious spaghetti
 * code. --KAA
 */
int
dochug(mtmp)
register struct monst *mtmp;
{
	register struct permonst *mdat;
	struct monst *gazemon, *nxtmon;
	char buf[BUFSZ];
	register int tmp=0;
	int inrange, nearby, scared;
#ifdef GOLDOBJ
        struct obj *ygold = 0, *lepgold = 0;
#endif

	
/*	Pre-movement adjustments	*/

	mdat = mtmp->data;
	
	mtmp->mattackedu = 0; /*Clear out attacked bit*/
	mtmp->mnoise = 0; /*Clear out noise bit*/
	
	if(mdat->mtyp == PM_GNOLL_MATRIARCH){
		if(!rn2(20)){
			makemon(&mons[PM_GNOLL], mtmp->mx, mtmp->my, NO_MINVENT|MM_ADJACENTOK|MM_ADJACENTSTRICT);
		}
	}
	if(mdat->mtyp == PM_FORD_GUARDIAN){
		if(!rn2(2) && distmin(mtmp->mux, mtmp->muy, mtmp->mx, mtmp->my) < 4 && distmin(u.ux, u.uy, mtmp->mx, mtmp->my) < 4 && !(mtmp->mstrategy&STRAT_WAITFORU)){
			ford_rises(mtmp);
		}
	}
	if(mdat->mtyp == PM_LEGION){
		rn2(7) ? makemon(mkclass(S_ZOMBIE, G_NOHELL|G_HELL), mtmp->mx, mtmp->my, NO_MINVENT|MM_ADJACENTOK|MM_ADJACENTSTRICT): 
				 makemon(&mons[PM_LEGIONNAIRE], mtmp->mx, mtmp->my, NO_MINVENT|MM_ADJACENTOK|MM_ADJACENTSTRICT);
	}
	if(needs_familiar(mtmp) && !mtmp->mspec_used){
		if((mdat->mtyp == PM_COVEN_LEADER && !rn2(4))
			|| (mdat->mtyp == PM_WITCH && !rn2(20))
		){
			struct monst *familliar;
			if(canseemon(mtmp))
				pline("%s concentrates.", Monnam(mtmp));
			familliar = makemon(&mons[PM_WITCH_S_FAMILIAR], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH);
			if(familliar){
				//Heal up
				mtmp->mhp += mtmp->m_lev;
				if(mtmp->mhp > mtmp->mhpmax)
					mtmp->mhp = mtmp->mhpmax;
				//Sync new familiar
				familliar->m_lev = mtmp->m_lev;
				familliar->mhp = mtmp->mhp;
				familliar->mhpmax = mtmp->mhpmax;
				familliar->mvar_witchID = (long)mtmp->m_id;
				familliar->mpeaceful = mtmp->mpeaceful;
				if(mtmp->mtame){
					familliar = tamedog_core(familliar, (struct obj *)0, TRUE);
				}
				//Stop running
				if(mtmp->mflee && mtmp->mhp > mtmp->mhpmax/2){
					mtmp->mflee = 0;
					mtmp->mfleetim = 0;
				}
			}
		}
	}
	if(mtmp->mtyp == PM_STAR_VAMPIRE){
		if(mtmp->mvar_star_vampire_blood > 0){
			mtmp->mvar_star_vampire_blood--;
			mtmp->mhp += 20;
			mtmp->mhp = min(mtmp->mhp, mtmp->mhpmax);
		}
		if(mtmp->mvar_star_vampire_blood <= 0 && !mtmp->perminvis){
			mtmp->minvis = TRUE;
			mtmp->perminvis = TRUE;
			newsym(mtmp->mx,mtmp->my);
		}
	}
	if (mtmp->mstrategy & STRAT_ARRIVE){
	    int res = m_arrival(mtmp);
	    if (res >= 0) return res;
	}

	/* check for waitmask status change */
	if ((mtmp->mstrategy & STRAT_WAITFORU) &&
		(m_canseeu(mtmp) || mtmp->mhp < mtmp->mhpmax))
	    mtmp->mstrategy &= ~STRAT_WAITFORU;

	/* update quest status flags */
	quest_stat_check(mtmp);
	
	if(mdat->mtyp == PM_CENTER_OF_ALL 
		&& !mtmp->mtame 
		&& !Is_astralevel(&u.uz)
		&& (near_capacity()>UNENCUMBERED || u.ulevel < 14 || mtmp->mpeaceful) 
		&& (near_capacity()>SLT_ENCUMBER || mtmp->mpeaceful || u.uinsight < 2 || (u.uinsight < 32 && !rn2(u.uinsight))) 
		&& (near_capacity()>MOD_ENCUMBER || !rn2(4))
	){
		int nlev;
		d_level flev;

		if (!(mon_has_amulet(mtmp) || In_endgame(&u.uz))) {
			nlev = random_teleport_level();
			// pline("going to %d",nlev);
			if (nlev != depth(&u.uz)) {
				struct engr *oep = engr_at(mtmp->mx,mtmp->my);
				if(!oep){
					make_engr_at(mtmp->mx, mtmp->my,
					 "", 0L, DUST);
					oep = engr_at(mtmp->mx,mtmp->my);
				}
				if(oep){
					oep->ward_id = FOOTPRINT;
					oep->halu_ward = 1;
					oep->ward_type = BURN;
					oep->complete_wards = 1;
				}
				get_level(&flev, nlev);
				migrate_to_level(mtmp, ledger_no(&flev), MIGR_RANDOM,
					(coord *)0);
				return 0;
			}
		}
	}

   if (mdat->mtyp != PM_GIANT_TURTLE || !mtmp->mflee)
	if (!(mtmp->mcanmove && mtmp->mnotlaugh) || (mtmp->mstrategy & STRAT_WAITMASK)) {
	    if (Hallucination) newsym(mtmp->mx,mtmp->my);
	    if (mtmp->mcanmove && mtmp->mnotlaugh && (mtmp->mstrategy & STRAT_CLOSE) &&
	       !mtmp->msleeping && monnear(mtmp, u.ux, u.uy))
		quest_talk(mtmp);	/* give the leaders a chance to speak */
	    return(0);	/* other frozen monsters can't do anything */
	}

	/* there is a chance we will wake it */
	if (mtmp->msleeping && !disturb(mtmp)) {
		if (Hallucination) newsym(mtmp->mx,mtmp->my);
		return(0);
	}

	/* not frozen or sleeping: wipe out texts written in the dust */
	wipe_engr_at(mtmp->mx, mtmp->my, 1);

	/* confused monsters get unconfused with small probability */
	if (mtmp->mconf && !rn2(50)) mtmp->mconf = 0;

	/* berserk monsters calm down with small probability */
	if (mtmp->mberserk && !rn2(50)) mtmp->mberserk = 0;
	if (mtmp->mdisrobe && !rn2(50)) mtmp->mdisrobe = 0;
	if (mtmp->menvy && !rn2(999)) mtmp->menvy = 0;
	if (mtmp->mdoubt && !rn2(300)) mtmp->mdoubt = 0;
	if (mtmp->mscorpions && !rn2(20)) mtmp->mscorpions = 0;
	
	if(mtmp->msleeping && (mtmp->mformication || mtmp->mscorpions) && rn2(mtmp->m_lev)){
		//Awakens from the bugs. High level is good for it here.
		mtmp->msleeping = 0;
	}

	if (mtmp->mcrazed){
		if(!rn2(4)){
			mtmp->mconf = 1;
			(void) set_apparxy(mtmp);
		}
		if(!rn2(4)){
			mtmp->mberserk = 1;
			(void) set_apparxy(mtmp);
		}
		if(!rn2(10)){
			mtmp->mnotlaugh=0;
			mtmp->mlaughing=rnd(5);
		}
	}
	if(mtmp->mspores){
		if(!rn2(mtmp->m_lev)){
			mtmp->mconf = 1;
			(void) set_apparxy(mtmp);
			if(!rn2(mtmp->m_lev)){
				mtmp->mberserk = 1;
				(void) set_apparxy(mtmp);
			}
		}
	}
	if(mtmp->mrage){
		extern const int monstr[];
		if(!rn2(4)){
			mtmp->mberserk = 1;
			(void) set_apparxy(mtmp);
		}
		mtmp->encouraged = max(mtmp->encouraged, min(20, monstr[mtmp->mtyp]));
	}
	
	if(mtmp->mdisrobe){
		if(mon_throw_armor(mtmp))
			return 0;
	} else if(mtmp->mnudist){
		if(mon_strip_armor(mtmp))
			return 0;
	}
	
	if(mtmp->mtyp == PM_MAD_SEER){
		if(!rn2(80)){
			mtmp->mnotlaugh=0;
			mtmp->mlaughing=rnd(5);
		}
	}
	
	if(mtmp->mtyp == PM_QUIVERING_BLOB && !mtmp->mflee){
		if(!rn2(20)){
			monflee(mtmp, 0, TRUE, TRUE);
		}
	}
	if(mtmp->mtyp == PM_GRUE){
		if(!mtmp->mflee && !isdark(mtmp->mx, mtmp->my)){
			monflee(mtmp, 0, TRUE, TRUE);
		} else if(mtmp->mflee && isdark(mtmp->mx, mtmp->my)){
			mtmp->mflee = 0;
			mtmp->mfleetim = 0;
		}
	}
	
	/* stunned monsters get un-stunned with larger probability */
	if (mtmp->mstun && !rn2(10)) mtmp->mstun = 0;

	/* some monsters teleport */
	if (mon_resistance(mtmp,TELEPORT)
		&& (mtmp->mflee || !rn2(5))
		&& (!mtmp->mpeaceful || mtmp->mtame)
		&& !rn2(40)
		&& !mtmp->iswiz
		&& !(noactions(mtmp))
		&& !notel_level()
	) {
		if (rloc(mtmp, TRUE))
			return(0);
	}
	
	if(mtmp->mtyp == PM_DRACAE_ELADRIN){
		if(!mtmp->mvar_dracaePreg){
			struct permonst *ptr = mkclass(S_CHA_ANGEL, G_PLANES);
			if(ptr)
				mtmp->mvar_dracaePreg = monsndx(ptr);
		}
		else if(mtmp->mvar_dracaePregTimer < 6){
			mtmp->mvar_dracaePregTimer += rnd(3);
		} else if(!mtmp->mpeaceful){
			int ox = mtmp->mx, oy = mtmp->my;
			rloc(mtmp, TRUE);
			if(mtmp->mx != ox || mtmp->my != oy){
				int type = mtmp->mvar_dracaePreg;
				mtmp->mvar_dracaePreg = 0;
				mtmp->mvar_dracaePregTimer = 0;
				mtmp = makemon(&mons[type], ox, oy, NO_MINVENT|MM_NOCOUNTBIRTH);
				if(mtmp){
					dracae_eladrin_spawn_equip(mtmp, mtmp->mtyp);
					m_dowear(mtmp, TRUE);
					m_level_up_intrinsic(mtmp);
				}
			}
			return 0;
		}
	}
	if(mtmp->mtyp == PM_MOTHERING_MASS){
		if(!mtmp->mvar_dracaePreg){
			struct permonst *ptr = mkclass(S_CHA_ANGEL, G_PLANES);
			if(ptr)
				mtmp->mvar_dracaePreg = monsndx(ptr);
		}
		else if(mtmp->mvar_dracaePregTimer < 6){
			mtmp->mvar_dracaePregTimer += rnd(3);
		} else if(!mtmp->mpeaceful){
			int i;
			int ox = mtmp->mx, oy = mtmp->my;
			int type = mtmp->mvar_dracaePreg;
			int etyp = counter_were(type);
			mtmp->mvar_dracaePreg = 0;
			mtmp->mvar_dracaePregTimer = 0;
			for(i = rnd(4); i; i--){
				if(etyp)
					mtmp = makemon(&mons[etyp], ox, oy, NO_MINVENT|MM_ADJACENTOK|MM_ADJACENTSTRICT|MM_NOCOUNTBIRTH);
				else return 0;
				if(mtmp){
					struct obj *otmp;
					dracae_eladrin_spawn_equip(mtmp, type);
					m_level_up_intrinsic(mtmp);
				}
			}
			return 0;
		}
	}
	if(mtmp->mtyp == PM_JRT_NETJER && !mtmp->mpeaceful && !mtmp->mcan && !rn2(3)){
		int type;
		int ox = mtmp->mx, oy = mtmp->my;
		struct monst *tmpm;
		boolean printed = FALSE;
		if(canseemon(mtmp)){
			pline("Stars twinkle around %s.", mon_nam(mtmp));
			printed = TRUE;
		}
		rloc(mtmp, TRUE);
		if(mtmp->mx != ox || mtmp->my != oy){
			if(rn2(3)){
				type = PM_COURE_ELADRIN;
			}
			else {
				if(dungeon_topology.alt_tulani == CAILLEA_CASTE)
					type = PM_CAILLEA_ELADRIN;
				else
					type = PM_TULANI_ELADRIN;
			}
			tmpm = makemon(&mons[type], ox, oy, NO_MINVENT|MM_NOCOUNTBIRTH);
			if(tmpm){
				struct obj *otmp;
				if(canseemon(tmpm))
					pline("%stars coalesce into %s!", printed ? "The s" : "S", an(mons[type].mname));
				
				jrt_eladrin_spawn_equip(tmpm, type);
				tmpm->mpeaceful = mtmp->mpeaceful;
				set_malign(tmpm);
				if(has_template(mtmp, POISON_TEMPLATE))
					set_template(tmpm, POISON_TEMPLATE);
				else if(has_template(mtmp, MAD_TEMPLATE))
					set_template(tmpm, FALLEN_TEMPLATE);
				else if(has_template(mtmp, FALLEN_TEMPLATE))
					set_template(tmpm, FALLEN_TEMPLATE);
				if(mtmp->mfaction)
					set_faction(tmpm, mtmp->mfaction);
			}
			return 0;
		}
	}
	if(mtmp->mtyp == PM_SHALOSH_TANNAH && !rn2(9)){
		struct monst *tmpm;
		int i = rnd(3);
		if(canseemon(mtmp))
			pline(i > 1 ? "A pustule bursts in a spray of acid and strange larvae tumble out!" : "A pustule bursts in a spray of acid and a strange larva tumbles out!");
		explode(mtmp->mx, mtmp->my, AD_ACID, MON_EXPLODE, d(6,6), EXPL_NOXIOUS, i/3+1);
		for(; i > 0; i--){
			tmpm = makemon(&mons[PM_STRANGE_LARVA], mtmp->mx, mtmp->my, NO_MINVENT|MM_ADJACENTOK|MM_ADJACENTSTRICT|MM_NOCOUNTBIRTH);
			if(tmpm){
				switch(rn2(4)){
					case 0:
					tmpm->mvar_tanninType = PM_VROCK;
					break;
					case 1:
					tmpm->mvar_tanninType = PM_HEZROU;
					break;
					case 2:
					tmpm->mvar_tanninType = PM_NALFESHNEE;
					break;
					case 3:
					tmpm->mvar_tanninType = PM_MARILITH;
					tmpm->female = 1;
					break;
				}
			}
		}
	}
	
	
	if(mdat->mtyp == PM_OONA && !mtmp->mspec_used){
		nearby = FALSE;
		struct monst *tmpm;
		for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
			if(tmpm != mtmp && !DEADMONSTER(tmpm)){
				if ( mtmp->mpeaceful != tmpm->mpeaceful && distmin(mtmp->mx,mtmp->my,tmpm->mx,tmpm->my) < 4 && resists_oona(tmpm)) {
					nearby=TRUE;
				}
			}
		}
		if(!mtmp->mpeaceful && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) < 4 && Oona_resistance){
			nearby=TRUE;
		}
	}

	if (is_commander(mdat) && mfind_target(mtmp, FALSE, TRUE))
		m_command(mtmp);

	if (((mdat->msound == MS_SHRIEK || mdat->msound == MS_HOWL) && !um_dist(mtmp->mx, mtmp->my, 1))
		|| (mdat->msound == MS_SHOG && !rn2(8))
		|| (mdat->msound == MS_JUBJUB && !rn2(10) && (!um_dist(mtmp->mx, mtmp->my, 3) || !rn2(10)))
		|| (mdat->msound == MS_TRUMPET && !rn2(10) && !um_dist(mtmp->mx, mtmp->my, 3))
		|| (mdat->msound == MS_DREAD && !rn2(4))
		|| (mdat->msound == MS_OONA && (nearby || !rn2(6)))
		|| (mdat->msound == MS_SONG && !rn2(6))
		|| (mdat->mtyp == PM_RHYMER && !mtmp->mspec_used && !rn2(6))
		|| (mdat->msound == MS_INTONE && !rn2(6))
		|| (mdat->msound == MS_FLOWER && !rn2(6))
		|| (mdat->msound == MS_SECRETS && !mtmp->mspec_used && u.uencouraged < Insanity/5+1)
		|| (mdat->msound == MS_SCREAM && !rn2(7))
		|| (mdat->msound == MS_HOWL && !rn2(7))
		|| (mdat->msound == MS_HARROW && !mtmp->mspec_used)
		|| (mdat->msound == MS_APOC && !mtmp->mspec_used)
	) m_respond(mtmp);

	if(!mtmp->mblinded && !mon_resistance(mtmp, GAZE_RES)) for (gazemon = fmon; gazemon; gazemon = nxtmon){
		nxtmon = gazemon->nmon;
		if(DEADMONSTER(gazemon))
			continue;
		if (gazemon != mtmp
			&& mon_can_see_mon(mtmp, gazemon)
			&& clear_path(mtmp->mx, mtmp->my, gazemon->mx, gazemon->my)
		){
			int i;
			if(gazemon->mtyp == PM_MEDUSA && resists_ston(mtmp)
			) continue;
			
			if (hideablewidegaze(gazemon->data) && hiddenwidegaze(gazemon))
				continue;

			if (controlledwidegaze(gazemon->data)
				&& !mm_aggression(gazemon, mtmp)
			) continue;
			
			for(i = 0; i < NATTK; i++)
				 if(gazemon->data->mattk[i].aatyp == AT_WDGZ) {
					if((gazemon->data->mattk[i].adtyp ==  AD_CONF 
						|| gazemon->data->mattk[i].adtyp ==  AD_WISD 
					) && (dmgtype_fromattack(mtmp->data, AD_CONF, AT_WDGZ)
						|| dmgtype_fromattack(mtmp->data, AD_WISD, AT_WDGZ)
					)) continue;
					/*
					if(canseemon(mtmp) && canseemon(gazemon)){
						Sprintf(buf,"%s can see", Monnam(mtmp));
						pline("%s %s...", buf, mon_nam(gazemon));
					}*/
					if (xgazey(gazemon, mtmp, &gazemon->data->mattk[i], -1) & MM_DEF_DIED)
						return (1);	/* mtmp died from seeing something */
					break;
				 }
		}
	}
	
	if (mtmp->mhp <= 0) return(1); /* m_respond gaze can kill medusa */

	/* fleeing monsters might regain courage */
	if (mtmp->mflee && !mtmp->mfleetim
	   && mtmp->mhp == mtmp->mhpmax && !rn2(25)){
		mtmp->mflee = 0;
        if(canseemon(mtmp) && (mdat->mtyp == PM_GIANT_TURTLE) && !(mtmp->mfrozen)){
         pline("%s comes out of %s shell.", Monnam(mtmp), mhis(mtmp));
         mtmp->mcanmove=1;
		}
    }
   else if (mdat->mtyp == PM_GIANT_TURTLE && !mtmp->mcanmove)
     return (0);

	set_apparxy(mtmp);
	/* Must be done after you move and before the monster does.  The
	 * set_apparxy() call in m_move() doesn't suffice since the variables
	 * inrange, etc. all depend on stuff set by set_apparxy().
	 */

	/* Monsters that want to acquire things */
	/* may teleport, so do it before inrange is set */
	if(is_covetous(mdat) && (mdat->mtyp!=PM_DEMOGORGON || !rn2(3)) 
		&& mdat->mtyp!=PM_ELDER_PRIEST /*&& mdat->mtyp!=PM_SHAMI_AMOURAE*/
		&& mdat->mtyp!=PM_LEGION /*&& mdat->mtyp!=PM_SHAMI_AMOURAE*/
		&& !(noactions(mtmp))
		&& !(mtmp->mpeaceful && !mtmp->mtame) /*Don't telespam the player if peaceful*/
		&& !(mtmp == u.usteed) /*Steeds can't use tactics*/
	) (void) tactics(mtmp);
	
	if(mdat->mtyp == PM_GREAT_CTHULHU && !rn2(20)){
		if(tactics(mtmp))
			return 0;
	}
	
	/* check distance and scariness of attacks */
	distfleeck(mtmp,&inrange,&nearby,&scared);
	/* summoned monster was abjured as a result of wards etc. */
	if(DEADMONSTER(mtmp))
		return 1;

	if(find_defensive(mtmp)) {
		if (use_defensive(mtmp) != 0)
			return 1;
	} else if(find_misc(mtmp)) {
		if (use_misc(mtmp) != 0)
			return 1;
	}
	
	if((is_drow(mtmp->data) || mtmp->mtyp == PM_LUGRIBOSSK || mtmp->mtyp == PM_MAANZECORIAN)
		&& (!mtmp->mpeaceful || Darksight)
		&& !(mtmp->mpeaceful && !mtmp->mtame && mtmp->mfaction == PEN_A_SYMBOL)
		&& (levl[mtmp->mx][mtmp->my].lit == 1 || (viz_array[mtmp->my][mtmp->mx]&TEMP_LIT1 && !(viz_array[mtmp->my][mtmp->mx]&TEMP_DRK1)))
		&& !mtmp->mcan && mtmp->mspec_used < 4
		&& !(noactions(mtmp))
		&& !(mindless_mon(mtmp))
	){
		if(cansee(mtmp->mx,mtmp->my)) pline("%s invokes the darkness.",Monnam(mtmp));
	    do_clear_area(mtmp->mx,mtmp->my, 5, set_lit, (genericptr_t)0);
		vision_full_recalc = 1;
	    if(mtmp->mtyp == PM_HEDROW_WARRIOR) mtmp->mspec_used += d(4,4);
		else mtmp->mspec_used += max(10 - mtmp->m_lev,2);
		return 0;
	}
	
	if((!mtmp->mpeaceful || !Darksight)
		&& (levl[mtmp->mx][mtmp->my].lit == 0 || viz_array[mtmp->my][mtmp->mx]&TEMP_DRK1)
		&& !(noactions(mtmp))
		&& !(mindless_mon(mtmp))
		&& !darksight(mtmp->data)
		&& which_armor(mtmp, W_ARM)
	){
		struct obj *otmp = which_armor(mtmp, W_ARM);
		if(otmp->otyp == LANTERN_PLATE_MAIL && !otmp->lamplit && !otmp->cursed){
			if (canseemon(mtmp)) {
				pline("%s lights %s %s.", Monnam(mtmp), mhis(mtmp),
					xname(otmp));
			}	    	
			begin_burn(otmp);
			return 0;
		}
	}

	if (mtmp->mtyp == PM_NURSE || mtmp->mtyp == PM_HEALER || mtmp->mtyp == PM_CLAIRVOYANT_CHANGED){
	    // && !uarmu && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf){
		struct monst *patient = 0;
		int i, j, x, y, rot = rn2(3);
		for(i = -1; i < 2; i++){
			for(j = -1; j < 2; j++){
				x = mtmp->mx+(i+rot)%3;
				y = mtmp->my+(j+rot)%3;
				if(mtmp->mx == x && mtmp->my == y)
					continue;
				if(!isok(x, y))
					continue;
				patient = m_u_at(x,y);
				if(!patient)
					continue;
				if(patient == &youmonst){
					//Note: body armor blocks healing, healing is also reduced by the rolled DR
					if(!mtmp->mpeaceful || nonliving(youracedata) || (uarm && arm_blocks_upper_body(uarm->otyp)))
						continue;
					if((Upolyd && u.mh < u.mhmax) || (!Upolyd && u.uhp < u.uhpmax)){
						nurse_heal(mtmp, patient, TRUE);
						return 0;
					}
				}
				else {
					//Note: body armor blocks healing, healing is also reduced by the rolled DR
					if(nonliving(patient->data) || patient->mpeaceful != mtmp->mpeaceful || (which_armor(patient, W_ARM) && arm_blocks_upper_body(which_armor(patient, W_ARM)->otyp)))
						continue;
					if(patient->mhp < patient->mhpmax){
						nurse_heal(mtmp, patient, canseemon(mtmp) || canseemon(patient));
						return 0;
					}
				}
			}
		}
	}
	
	if (mtmp->mtyp == PM_ITINERANT_PRIESTESS && u.uinsight >= 40 && !straitjacketed_mon(mtmp)){
		struct monst *patient = 0;
		int i, j, x, y, rot = rn2(3);
		for(i = -1; i < 2; i++){
			for(j = -1; j < 2; j++){
				x = mtmp->mx+(i);
				y = mtmp->my+(j);
				if(mtmp->mx == x && mtmp->my == y)
					continue;
				if(!isok(x, y))
					continue;
				patient = m_u_at(x,y);
				if(!patient)
					continue;
				if(patient == &youmonst){
					if(!mtmp->mpeaceful || nonliving(youracedata) || hates_holy(youracedata))
						continue;
					if((Upolyd && u.mh < u.mhmax) || (!Upolyd && u.uhp < u.uhpmax)){
						itiner_heal(mtmp, patient, TRUE);
						return 0;
					}
				}
				else {
					if(nonliving(patient->data) || patient->mpeaceful != mtmp->mpeaceful || hates_holy_mon(patient))
						continue;
					if(patient->mhp < patient->mhpmax){
						itiner_heal(mtmp, patient, canseemon(mtmp) || canseemon(patient));
						return 0;
					}
				}
			}
		}
	}
	
	if(mtmp->mtyp == PM_OPERATOR || mtmp->mtyp == PM_PARASITIZED_OPERATOR){
		struct monst *repairee = 0;
		int i, j, x, y, rot = rn2(3);
		for(i = -1; i < 2; i++){
			for(j = -1; j < 2; j++){
				x = mtmp->mx+(i+rot)%3;
				y = mtmp->my+(j+rot)%3;
				if(!isok(x, y))
					continue;
				repairee = m_u_at(x,y);
				if(!repairee)
					continue;
				if(repairee == &youmonst){
					if(!uandroid)
						continue;
					if(mtmp->mpeaceful && ((Upolyd && u.mh < u.mhmax) || (!Upolyd && u.uhp < u.uhpmax))){
						repair(mtmp, repairee, TRUE);
						return 0;
					}
				}
				else {
					if(!is_android(repairee->data))
						continue;
					if(repairee->mpeaceful == mtmp->mpeaceful && repairee->mhp < repairee->mhpmax){
						repair(mtmp, repairee, canseemon(mtmp) || canseemon(repairee));
						return 0;
					}
				}
			}
		}
	}

#define can_recharge_mon(mtmp) (mtmp && (mtmp->mtyp == PM_ANDROID || mtmp->mtyp == PM_GYNOID || mtmp->mtyp == PM_OPERATOR || mtmp->mtyp == PM_ALIDER ||\
		mtmp->mtyp == PM_PARASITIZED_ANDROID || mtmp->mtyp == PM_PARASITIZED_GYNOID || mtmp->mtyp == PM_PARASITIZED_OPERATOR))
#define ovar1_rechargeable(otmp) (otmp && (is_vibroweapon(otmp) || is_blaster(otmp)) && otmp->ovar1_charges <= 0 && (!(otmp->otyp == HAND_BLASTER || otmp->otyp == ARM_BLASTER) || otmp->recharged < 4))
#define age_rechargeable(otmp) (otmp && is_lightsaber(otmp) && otmp->age <= 0 && otmp->oartifact != ART_INFINITY_S_MIRRORED_ARC && otmp->otyp != KAMEREL_VAJRA)
#define can_be_recharged(otmp) (ovar1_rechargeable(otmp) || age_rechargeable(otmp))


	if (can_recharge_mon(mtmp) && !mtmp->mcan && !mtmp->mspec_used && !(noactions(mtmp)) && !(mindless_mon(mtmp)) && !rn2(20)){
		struct obj * rechargee = (struct obj *) 0;
		struct obj * otmp = (struct obj *) 0;
		if (MON_WEP(mtmp) && can_be_recharged(MON_WEP(mtmp)))
			rechargee = MON_WEP(mtmp);
		else if (MON_SWEP(mtmp) && can_be_recharged(MON_SWEP(mtmp)))
			rechargee = MON_SWEP(mtmp);
		else {
			for(otmp = mtmp->minvent; otmp; otmp = otmp->nobj){
				if (can_be_recharged(otmp)) {
					rechargee = otmp;
					break;
				}
			}
		}
		if (rechargee){
			if(canspotmon(mtmp)){
				if(mtmp->mtyp == PM_ALIDER) pline("%s performs a ritual of recharging.",Monnam(mtmp));
				else pline("%s uses %s on-board recharger.",Monnam(mtmp), hisherits(mtmp));
			}
			if (age_rechargeable(rechargee)){
				rechargee->age = 75000;
			} else if(rechargee->otyp == MASS_SHADOW_PISTOL){
				rechargee->ovar1_charges = 800L + rn2(200);
			} else if(rechargee->otyp == RAYGUN){
				rechargee->ovar1_charges = (8 + rn2(8))*10L;
			} else {
				rechargee->ovar1_charges = 80L + rn2(20);
			}
			if(rechargee->recharged < 7) rechargee->recharged++;
			mtmp->mspec_used = 10;
			return 0;
		}
	}

	if((mtmp->mtyp == PM_PORO_AULON || mtmp->mtyp == PM_ALIDER || mtmp->mtyp == PM_OONA)
		&& !mtmp->mcan && !mtmp->mspec_used
		&& !(noactions(mtmp))
		&& !(mindless_mon(mtmp))
		&& !rn2(20)
	){
		struct obj * otmp = which_armor(mtmp, W_ARM);
		if(otmp && otmp->otyp == EILISTRAN_ARMOR){
			if(otmp->altmode != EIL_MODE_ON)
				otmp->altmode = EIL_MODE_ON;
			if(otmp->ovar1_eilistran_charges <= 40){
				if(canspotmon(mtmp))
					pline("%s replaces some worn out components in %s armor.",Monnam(mtmp), hisherits(mtmp));
				otmp->ovar1_eilistran_charges += 60;
				return 0;
			}
		}
	}

	if(mtmp->mtyp == PM_PHALANX
		&& !mtmp->mcan
		&& !(noactions(mtmp))
	){
		struct obj *otmp;
		int sprcnt = 0;
		for(otmp = mtmp->minvent; otmp; otmp = otmp->nobj)
			if(otmp->otyp == SPEAR)
				sprcnt++;
		if(sprcnt < 3){
			if(canspotmon(mtmp)) pline("%s pulls a spear out of its rotting mass.",Monnam(mtmp));
			(void) mongets(mtmp, SPEAR, NO_MKOBJ_FLAGS);
			init_mon_wield_item(mtmp);
			return 0;
		}
	}

	/* Demonic Blackmail! */
	if(nearby && mdat->msound == MS_BRIBE &&
#ifdef CONVICT
       (monsndx(mdat) != PM_PRISON_GUARD) &&
#endif /* CONVICT */
       !no_upos(mtmp) &&
	   mtmp->mpeaceful && !mtmp->mtame && !u.uswallow) {
		if (mtmp->mux != u.ux || mtmp->muy != u.uy) {
			pline("%s whispers at thin air.", Monnam(mtmp));

			if (is_demon(youracedata)) {
			  /* "Good hunting, brother" */
			    if (!tele_restrict(mtmp)) (void) rloc(mtmp, TRUE);
			} else {
			    char prompt[BUFSZ];
			    Snprintf(prompt, BUFSZ, "Tell %s your real location?", mon_nam(mtmp));
			    if (yn(prompt) == 'y') {
				coord cc;
				enexto(&cc, u.ux, u.uy, mtmp->data);
			        rloc_to(mtmp, cc.x, cc.y);
				mtmp->mux = u.ux;
				mtmp->muy = u.uy;
				if (demon_talk(mtmp)) return 1;
			    } else {
			        mtmp->minvis = mtmp->perminvis = 0;
				/* Why?  For the same reason in real demon talk */
				pline("%s gets angry!", Amonnam(mtmp));
				mtmp->mpeaceful = 0;
				set_malign(mtmp);
				newsym(mtmp->mx, mtmp->my);
				/* since no way is an image going to pay it off */
			    }
			}
		} else if(demon_talk(mtmp)) return(1);	/* you paid it off */
	}

#ifdef CONVICT
	/* Prison guard extortion */
    if(nearby && (monsndx(mdat) == PM_PRISON_GUARD) && !mtmp->mpeaceful
	 && !mtmp->mtame && !u.uswallow && (!mtmp->mspec_used)) {
        long gdemand = 500 * u.ulevel;
        long goffer = 0;

        pline("%s demands %ld %s to avoid re-arrest.", Amonnam(mtmp),
         gdemand, currency(gdemand));
        if ((goffer = bribe(mtmp)) >= gdemand) {
            verbalize("Good.  Now beat it, scum!");
            mtmp->mpeaceful = 1;
            set_malign(mtmp);
        } else {
            pline("I said %ld!", gdemand);
            mtmp->mspec_used = 1000;
        }
    }
#endif /* CONVICT */

	/* the watch will look around and see if you are up to no good :-) */
	if (mdat->mtyp == PM_WATCHMAN || mdat->mtyp == PM_WATCH_CAPTAIN)
		watch_on_duty(mtmp);
	if(mdat->mtyp == PM_NURSE || mdat->mtyp == PM_HEALER || mdat->mtyp == PM_CLAIRVOYANT_CHANGED || mdat->mtyp == PM_VEIL_RENDER){
		int i, j;
		struct trap *ttmp;
		struct monst *tmon;
		for(i=-1; i<2; i++)
			for(j=-1; j<2; j++)
				if(isok(mtmp->mx+i,mtmp->my+j)){
					ttmp = t_at(mtmp->mx+i,mtmp->my+j);
					tmon = m_at(mtmp->mx+i,mtmp->my+j);
					if(tmon){
						if(tmon->mtrapped && ttmp && ttmp->ttyp == VIVI_TRAP){
							if(canspotmon(mtmp))
								pline("%s frees a vivisected prisoner from an essence trap!", Monnam(mtmp));
							tmon->mpeaceful = mtmp->mpeaceful;
							tmon->mtrapped = 0;
							if(mtmp->mtame){
								reward_untrap(ttmp, tmon);
								u.uevent.uaxus_foe = 1;
								pline("An alarm sounds!");
								aggravate();
							}
							deltrap(ttmp);
							newsym(mtmp->mx+i,mtmp->my+j);
							return 1;
						}
						else if(tmon->mtyp == PM_JRT_NETJER && has_template(tmon, POISON_TEMPLATE) && mon_can_see_mon(mtmp, tmon)){
							struct obj *armor = which_armor(tmon, W_ARM);
							struct obj *under = which_armor(tmon, W_ARMU);
							if((!armor || !arm_blocks_upper_body(armor->otyp)) && (!under || !arm_blocks_upper_body(under->otyp)) && helpless_still(tmon)){
								pline("%s extracts the fang from %s heart!", Monnam(mtmp), s_suffix(mon_nam(tmon)));
								set_template(tmon, 0);
								struct monst *newmon = tamedog_core(tmon, (struct obj *)0, TRUE);
								if(newmon){
									tmon = newmon;
									newsym(tmon->mx, tmon->my);
									tmon->mpeaceful = mtmp->mpeaceful;
									if(mtmp->mtame){
										pline("%s comes to %s senses, and is incredibly grateful for the aid!", Monnam(tmon), mhis(tmon));
										if(get_mx(tmon, MX_EDOG)){
											EDOG(tmon)->loyal = 1;
										}
									}
								}
							}
						}
					}
				}
	}
	if(mdat->mtyp == PM_TOVE && !rn2(20)){
		struct trap *ttmp = t_at(mtmp->mx, mtmp->my);
		struct rm *lev = &levl[mtmp->mx][mtmp->my];
		schar typ;
		boolean nohole = !Can_dig_down(&u.uz);
		if (!(
			!isok(mtmp->mx,mtmp->my) || 
			(ttmp && ttmp->ttyp == MAGIC_PORTAL) ||
		   /* ALI - artifact doors from slash'em */
			(IS_DOOR(levl[mtmp->mx][mtmp->my].typ) && artifact_door(mtmp->mx, mtmp->my)) ||
			(boulder_at(mtmp->mx, mtmp->my))
			)
			&& (
				levl[mtmp->mx][mtmp->my].typ == CORR ||
				levl[mtmp->mx][mtmp->my].typ == DOOR ||
				levl[mtmp->mx][mtmp->my].typ == ROOM ||
				levl[mtmp->mx][mtmp->my].typ == ICE ||
				levl[mtmp->mx][mtmp->my].typ == GRASS ||
				levl[mtmp->mx][mtmp->my].typ == SOIL ||
				levl[mtmp->mx][mtmp->my].typ == SAND ||
				levl[mtmp->mx][mtmp->my].typ == PUDDLE ||
				//Note: on levels other than air and lolth, clouds are assumed to be at ground level
				(levl[mtmp->mx][mtmp->my].typ == CLOUD 
					&& !Is_airlevel(&u.uz) && !Is_lolth_level(&u.uz))
			)
		){
			typ = fillholetyp(mtmp->mx,mtmp->my);
			if (canseemon(mtmp))
				pline("%s gyres and gimbles into the %s.", Monnam(mtmp),surface(mtmp->mx,mtmp->my));
			if (typ != ROOM) {
				lev->typ = typ;
				if (ttmp) {
					if (delfloortrap(ttmp)) ttmp = (struct trap *)0;
				}
				/* if any objects were frozen here, they're released now */
				unearth_objs(mtmp->mx, mtmp->my);

					if(cansee(mtmp->mx, mtmp->my))
						pline_The("hole fills with %s!",
						  typ == LAVAPOOL ? "lava" : "water");
				if (!Levitation && !Flying && mtmp->mx==u.ux && mtmp->my==u.uy) {
					if (typ == LAVAPOOL)
					(void) lava_effects(TRUE);
					else if (!Wwalking)
					(void) drown();
				}
			}
			if (nohole || !ttmp || (ttmp->ttyp != PIT && ttmp->ttyp != SPIKED_PIT && ttmp->ttyp != TRAPDOOR))
				digactualhole(mtmp->mx, mtmp->my, mtmp, PIT, FALSE, TRUE);
			else
				digactualhole(mtmp->mx, mtmp->my, mtmp, HOLE, FALSE, TRUE);
		}
		if(DEADMONSTER(mtmp)) //Oops!
			return 1;
		else return 0;
	}
	if (has_mind_blast_mon(mtmp) && !Invulnerable && !rn2(mdat->mtyp == PM_CLAIRVOYANT_CHANGED ? 3 : mdat->mtyp == PM_ELDER_BRAIN ? 10 : 20) && !Catapsi) {
		boolean reducedFlayerMessages = (((Role_if(PM_NOBLEMAN) && Race_if(PM_DROW) && flags.initgend) || Role_if(PM_ANACHRONONAUT)) && In_quest(&u.uz));
		struct monst *m2, *nmon = (struct monst *)0;
		int dmg = 0;
		int power = 0;

		if (canseemon(mtmp)){
			if(mtmp->mtyp == PM_FOETID_ANGEL)
				pline("%s black tar bubbles.", s_suffix(Monnam(mtmp)));
			else
				pline("%s concentrates.", Monnam(mtmp));
		}
		// if (distu(mtmp->mx, mtmp->my) > BOLT_LIM * BOLT_LIM) {
			// You("sense a faint wave of psychic energy.");
		// }
		if(!reducedFlayerMessages) pline("A wave of psychic energy pours over you!");
		if (mtmp->mpeaceful && !mtmp->mberserk &&
		    (!Conflict || resist(mtmp, RING_CLASS, 0, 0))){
			if(!reducedFlayerMessages) pline("It feels quite soothing.");
		} else if (!Tele_blind){
			register boolean m_sen = tp_sensemon(mtmp);

			if(mdat->mtyp == PM_ELDER_BRAIN){
				lift_veil();
				quest_chat(mtmp);
			}
			if(mdat->mtyp == PM_CLAIRVOYANT_CHANGED){
				if(m_sen || (Blind_telepat && rn2(2)) || !rn2(10)){
					power++;
					dmg += rnd(15);
				}
				for(int i = 1; i < 7; i++)
					if(!rn2(20) && (m_sen || (Blind_telepat && rn2(2)) || !rn2(10))){
						dmg += rnd(15);
						power++;
					}
				if(power){
					pline("%s %s %s!",
						power == 1 ? "A mad voice" : power == 2 ? "A duet of mad voices" : power < 7 ? "Mad voices" : "A cacophony of mad voices",
						power == 1 
							? (!rn2(6) ? "screams" : !rn2(5) ? "babbles" : !rn2(4) ? "shouts" : !rn2(3) ? "whispers" : rn2(2) ? "courses" : "cries")
							: (!rn2(6) ? "scream" : !rn2(5) ? "babble" : !rn2(4) ? "shout" : !rn2(3) ? "whisper" : rn2(2) ? "course" : "cry"),
						m_sen ? "through your telepathy" :
						Blind_telepat ? "through your latent telepathy" : "into your mind");
				}
			}
			else if(mdat->mtyp == PM_FOETID_ANGEL){
				pline("It screams %s!",
					m_sen ? "at you through your telepathy" :
					Blind_telepat ? "at you through your latent telepathy" : "into your mind");
			}
			else if (m_sen || (Blind_telepat && rn2(2)) || !rn2(10)) {
				pline("It locks on to your %s!",
					m_sen ? "telepathy" :
					Blind_telepat ? "latent telepathy" : "mind");
				dmg = (mdat->mtyp == PM_GREAT_CTHULHU || mdat->mtyp == PM_LUGRIBOSSK || mdat->mtyp == PM_MAANZECORIAN) ? d(5,15) : (mdat->mtyp == PM_ELDER_BRAIN) ? d(3,15) : rnd(15);
				if(mtmp->mtyp == PM_MAD_SEER)
					mtmp->mspec_used += dmg;
			}
			if(dmg){
				dmg = reduce_dmg(&youmonst,dmg,FALSE,TRUE);
				losehp(dmg, "psychic blast", KILLED_BY_AN);
				if(mdat->mtyp == PM_SEMBLANCE) make_hallucinated(HHallucination + dmg, FALSE, 0L);
				if(mdat->mtyp == PM_GREAT_CTHULHU){
					make_stunned(HStun + dmg*10, TRUE);
					u.umadness |= MAD_DREAMS;
				}
				if(mdat->mtyp == PM_FOETID_ANGEL){
					make_doubtful((long) u.uinsight,TRUE);
				}
				if (mdat->mtyp == PM_ELDER_BRAIN) {
					for (m2 = fmon; m2; m2 = nmon) {
						nmon = m2->nmon;
						if (!DEADMONSTER(m2) && (m2->mpeaceful == mtmp->mpeaceful) && mon_resistance(m2,TELEPAT) && (m2!=mtmp))
						{
							m2->msleeping = 0;
							if (!m2->mcanmove && !rn2(5)) {
								m2->mfrozen = 0;
								if (m2->mtyp != PM_GIANT_TURTLE || !(m2->mflee))
									m2->mcanmove = 1;
							}
							m2->mux = u.ux;
							m2->muy = u.uy;
							m2->encouraged = max(m2->encouraged, dmg / 3);
						}
					}
				}
				if(mdat->mtyp == PM_CLAIRVOYANT_CHANGED){
					if(power >= 3){
						You("stagger!");
						make_stunned(HStun + dmg, TRUE);
					}
					if(power >= 4 && !Babble && !Screaming){
						if(rn2(3)){
							if(!BlockableClearThoughts)
								You("scream in pain!");
							HScreaming = 2;
						}
						else if(rn2(2)){
							if(BlockableClearThoughts)
								You_feel("a little frightened.");
							else
								You("begin screaming in terror and madness!");
							HScreaming = 1+rnd((dmg)/5+1)+rnd((dmg)/5+1);
						}
						else {
							if(BlockableClearThoughts)
								You_feel("a little incoherent.");
							else
								You("begin babbling incoherently!");
							HBabble = 1+rnd((dmg)/5+1)+rnd((dmg)/5+1);
						}
					}
					if(power >= 7 && !Vomiting){
						You("feel ill!");
						make_vomiting(rn1(15,10), TRUE);
					}
				}
				if(has_template(mtmp, DREAM_LEECH)){
					if (!Sleep_resistance){
						fall_asleep(-2*dmg, TRUE);
					}
				}
				nomul(0, NULL);
			}
		}
		for(m2=fmon; m2; m2 = nmon) {
			nmon = m2->nmon;
			if (DEADMONSTER(m2)) continue;
			if (m2->mpeaceful == mtmp->mpeaceful) continue;
			if (mindless_mon(m2)) continue;
			if (m2 == mtmp) continue;
			if (m2->mstrategy&STRAT_WAITMASK) continue;
			if (nonthreat(m2)) continue;
			power = 0;
			dmg = 0;
			if(mdat->mtyp == PM_CLAIRVOYANT_CHANGED){
				if (species_is_telepathic(m2->data) ||
					(mon_resistance(m2,TELEPAT) &&
					(rn2(2) || m2->mblinded)) || 
					!rn2(10)
				) {
					power++;
					dmg += rnd(15);
				}
				for(int i = 1; i < 7; i++)
					if (!rn2(20) && (species_is_telepathic(m2->data) ||
						(mon_resistance(m2,TELEPAT) &&
						(rn2(2) || m2->mblinded)) || 
						!rn2(10))
					) {
						dmg += rnd(15);
						power++;
					}
				if(power)
					if (cansee(m2->mx, m2->my))
						pline("%s looks spooked.", Monnam(m2));
			}
			else {
				if (species_is_telepathic(m2->data) ||
					(mon_resistance(m2,TELEPAT) &&
					(rn2(2) || m2->mblinded)) || 
					!rn2(10)
				) {
					if (cansee(m2->mx, m2->my))
						pline("It locks on to %s.", mon_nam(m2));
					if(mdat->mtyp == PM_GREAT_CTHULHU || mdat->mtyp == PM_LUGRIBOSSK || mdat->mtyp == PM_MAANZECORIAN)
						dmg = d(5,15);
					else if(mdat->mtyp == PM_ELDER_BRAIN){
						dmg = d(3,15);
					} else {
						dmg = rnd(15);
					}
					if(mtmp->mtyp == PM_MAD_SEER)
						mtmp->mspec_used += dmg;
				}
				if(dmg){
					if(mon_resistance(m2, HALF_SPDAM))
						dmg = (dmg+1) / 2;
					m2->mhp -= dmg;
					if (m2->mhp <= 0)
						monkilled(m2, "", AD_DRIN);
					else {
						m2->msleeping = 0;
					}
					if(!DEADMONSTER(m2)){
						if(mdat->mtyp == PM_GREAT_CTHULHU) 
							m2->mconf=TRUE;
						else if(mdat->mtyp == PM_FOETID_ANGEL){
							m2->mdoubt = TRUE;
						}
						else if(mdat->mtyp == PM_CLAIRVOYANT_CHANGED){
							if(power >= 3){
								m2->mconf=TRUE;
							}
							if(power >= 4){
								if(rn2(3)){
									//Scream in pain
									if (!is_silent_mon(m2)){
										if (canseemon(m2))
											pline("%s %s in pain!", Monnam(m2), humanoid_torso(m2->data) ? "screams" : "shrieks");
										else You_hear("%s %s in pain!", m2->mtame ? noit_mon_nam(m2) : mon_nam(m2), humanoid_torso(m2->data) ? "screaming" : "shrieking");
									}
									else {
										if (canseemon(m2))
											pline("%s writhes in pain!", Monnam(m2));
									}
									m2->movement = max(m2->movement - 12, -12);
								}
								else if(rn2(2)){
									//Laugh in madness
									if (!is_silent_mon(m2)){
										if (canseemon(m2))
											pline("%s begins laughing hysterically!", Monnam(m2));
										else You_hear("%s begin laughing hysterically!", m2->mtame ? noit_mon_nam(m2) : mon_nam(m2));
									}
									else {
										if (canseemon(m2))
											pline("%s begins tembling hysterically!", Monnam(m2));
									}
									m2->mlaughing = min(100, dmg);
									m2->mnotlaugh = FALSE;
								}
								else {
									//Babble (set cooldown)
									if (!is_silent_mon(m2)){
										if (canseemon(m2))
											pline("%s %s!", Monnam(m2), humanoid_torso(m2->data) ? "babbles incoherently" : "makes confused noises");
										else You_hear("%s %s in!", m2->mtame ? noit_mon_nam(m2) : mon_nam(m2), humanoid_torso(m2->data) ? "babbling incoherently" : "making confused noises");
										m2->mspec_used += 2*dmg;
									}
								}
								if(power >= 7){
									if(canseemon(m2))
										pline("%s vomits!", Monnam(m2));
									m2->mcanmove = 0;
									if ((m2->mfrozen + 3) > 127)
										m2->mfrozen = 127;
									else m2->mfrozen += 3;
									if (get_mx(m2, MX_EDOG)) {
										EDOG(m2)->hungrytime = max(0, EDOG(m2)->hungrytime-20);
									}
								}
							}
						} else if(mdat->mtyp == PM_ELDER_BRAIN){
							m2->mstdy = max(dmg, m2->mstdy);
						} else {
							if(mdat->mtyp == PM_SEMBLANCE) 
								m2->mconf=TRUE;
						}
						if(has_template(mtmp, DREAM_LEECH)){
							if (!resists_sleep(m2)){
								m2->msleeping = 1;
								slept_monst(m2);
							}
						}
					}
				}
			}
		}
	}

	if(!mtarget_adjacent(mtmp)){ /* don't fight at range if there's a melee target */
		/* Look for other monsters to fight (at a distance) */
		struct monst *mtmp2 = mfind_target(mtmp, FALSE, TRUE);
		if (mtmp2 && 
			(mtmp2 != &youmonst || 
				dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) > 2) &&
			(mtmp2 != mtmp)
		){
			int res;
			mon_ranged_gazeonly = 1;//State variable
			res = (mtmp2 == &youmonst) ? mattacku(mtmp)
									: mattackm(mtmp, mtmp2);

			if (res & MM_AGR_DIED)
				return 1; /* Oops. */

			if(!(mon_ranged_gazeonly) && (res & MM_HIT))
				return 0; /* that was our move for the round */
		}
	}

/*	Now the actual movement phase	*/

#ifndef GOLDOBJ
	if(!nearby || (mtmp->mflee && mtmp->mtyp != PM_BANDERSNATCH) || scared ||
	   mtmp->mconf || mtmp->mstun || (mtmp->minvis && !rn2(3)) ||
	   (mdat->mlet == S_LEPRECHAUN && !u.ugold && (mtmp->mgold || rn2(2))) ||
#else
    if (mdat->mlet == S_LEPRECHAUN) {
	    ygold = findgold(invent);
	    lepgold = findgold(mtmp->minvent);
	}
	if(!nearby || (mtmp->mflee && mtmp->mtyp != PM_BANDERSNATCH) || scared ||
	   mtmp->mconf || mtmp->mstun || (mtmp->minvis && !rn2(3)) ||
	   (mdat->mlet == S_LEPRECHAUN && !ygold && (lepgold || rn2(2))) ||
#endif
	   (is_wanderer(mdat) && !rn2(4)) || (Conflict && !mtmp->iswiz) ||
	   (is_blind(mtmp) && !rn2(4)) || mtmp->mpeaceful
	) {
		/* Possibly cast an undirected spell if not attacking you */
		/* note that most of the time castmu() will pick a directed
		   spell and do nothing, so the monster moves normally */
		/* arbitrary distance restriction to keep monster far away
		   from you from having cast dozens of sticks-to-snakes
		   or similar spells by the time you reach it */
		if (dist2(mtmp->mx, mtmp->my, u.ux, u.uy) <= 49 && !mtmp->mspec_used) {
		    struct attack *a;
			int index = 0, subout[SUBOUT_ARRAY_SIZE] = {0}, tohitmod = 0;
			int prev[4] = {0};
			struct attack prev_attk = noattack;
			
			while(TRUE){
				a = getattk(mtmp, &youmonst, prev, &index, &prev_attk, FALSE, subout, &tohitmod);
				if(a->aatyp == 0 && a->adtyp == 0 && a->damn == 0 && a->damd == 0)
					break;
				if ((a->aatyp == AT_MAGC || a->aatyp == AT_MMGC) && (a->adtyp == AD_SPEL || a->adtyp == AD_CLRC)) {
					if (mtmp->mux==u.ux && mtmp->muy==u.uy && couldsee(mtmp->mx, mtmp->my) && !mtmp->mpeaceful && 
						dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= BOLT_LIM*BOLT_LIM
					){
						if (xcasty(mtmp, &youmonst, a, mtmp->mux, mtmp->muy, 0)){
							tmp = 3;
							// if(mdat->mtyp != PM_DEMOGORGON) break;
							if(DEADMONSTER(mtmp) || MIGRATINGMONSTER(mtmp))
								return 1; //Oops!
						}
					} else {
						if (xcasty(mtmp, (struct monst *)0, a, 0, 0, 0)){
							tmp = 3;
							// if(mdat->mtyp != PM_DEMOGORGON) break;
							if(DEADMONSTER(mtmp) || MIGRATINGMONSTER(mtmp))
								return 1; //Oops!
						}
					}
				}
		    }
		}

		tmp = m_move(mtmp, 0);
		distfleeck(mtmp,&inrange,&nearby,&scared);	/* recalc */
		if(DEADMONSTER(mtmp))
			return 1; //Summoned monster was abjured.

		switch (tmp) {
		    case 0:	/* no movement, but it can still attack you */
		    case 3:	/* absolutely no movement */
				/* for pets, case 0 and 3 are equivalent */
			/* vault guard might have vanished */
			if (mtmp->isgd && (mtmp->mhp < 1 ||
					    (mtmp->mx == 0 && mtmp->my == 0)))
			    return 1;	/* behave as if it died */
			/* During hallucination, monster appearance should
			 * still change - even if it doesn't move.
			 */
			if(Hallucination) newsym(mtmp->mx,mtmp->my);
			break;
		    case 1:	/* monster moved */
			/* Maybe it stepped on a trap and fell asleep... */
			if (mtmp->msleeping || !(mtmp->mcanmove && mtmp->mnotlaugh)) return(0);
			/* Long worms thrash around */
			if(mtmp->wormno && (!mtmp->mpeaceful || Conflict || mtmp->mberserk)) wormhitu(mtmp);
			if(!nearby &&
			  (ranged_attk(mdat) || find_offensive(mtmp))){
				if(mdat->mtyp == PM_GREAT_CTHULHU || mdat->mtyp == PM_WATCHER_IN_THE_WATER || mdat->mtyp == PM_KETO || mdat->mtyp == PM_ARCADIAN_AVENGER){
					break;
			    } else return(0);
			    // return(0);
			}
 			else if(u.uswallow && mtmp == u.ustuck) {
			    /* a monster that's digesting you can move at the
			     * same time -dlc
			     */
			    return(!!(mattacku(mtmp)&MM_AGR_DIED));
			} else
				return(0);
			/*NOTREACHED*/
			break;
		    case 2:	/* monster died */
			return(1);
		}
	}
/*	Now, attack the player if possible - one attack set per monst	*/

	if (!mtmp->mpeaceful || mtmp->mberserk ||
	    (Conflict && !resist(mtmp, RING_CLASS, 0, 0))) {
	    if(inrange && !noattacks(mdat) && u.uhp > 0 && !scared && tmp != 3)
			if((mattacku(mtmp)&MM_AGR_DIED)) return(1); /* monster died (e.g. exploded) */
	}
	/* special speeches for quest monsters */
	if (!mtmp->msleeping && mtmp->mcanmove && mtmp->mnotlaugh && nearby)
	    quest_talk(mtmp);
	/* extra emotional attack for vile monsters */
	if (inrange && mtmp->data->msound == MS_CUSS && !mtmp->mpeaceful &&
		couldsee(mtmp->mx, mtmp->my) && ((!mtmp->minvis && !rn2(5)) || 
										mtmp->mtyp == PM_SIR_GARLAND || mtmp->mtyp == PM_GARLAND ||
										(mtmp->mtyp == PM_CHAOS && (mtmp->mvar_conversationTracker < 5 || !rn2(5)) )|| 
										mtmp->mtyp == PM_APOLLYON
	) )
	    cuss(mtmp);

	return(tmp == 2);
}

boolean
check_shore(x,y)
int x;
int y;
{
	int i;
	for(i = 1; i < 2; i++){
		if(isok(x+i,y+i) && !is_pool(x+i,y+i,FALSE) && ZAP_POS(levl[x+i][y+i].typ))
			return TRUE;
	}
	return FALSE;
}

void
ford_rises(guardian)
struct monst *guardian;
{
	int elm = rnd(4);
	int n = 0, i, cn;
	int lim_x, lim;
	int high_box_x, high_box_y;
	int low_box_x, low_box_y;
	int c, cx, cy;

#define	CHECK_COUNT	if(isok(cx, cy) && is_pool(cx, cy, TRUE)){\
				if(check_shore(cx, cy))\
					n++;\
			}

	for(i = 1; i < 5 || (i < COLNO/2 && n < elm); i++){
		high_box_x = guardian->mx + i;
		high_box_y = guardian->my + i;
		low_box_x = guardian->mx - i;
		low_box_y = guardian->my - i;
		for(c = -i; c <= i; c++){
			cx = low_box_x;
			cy = guardian->my+c;
			CHECK_COUNT

			cx = high_box_x;
			cy = guardian->my+c;
			CHECK_COUNT

			cx = guardian->mx+c;
			cy = low_box_y;
			CHECK_COUNT

			cx = guardian->mx+c;
			cy = high_box_y;
			CHECK_COUNT
		}
	}
	if(!n)
		return;

#define	CHECK_SPAWN	if(isok(cx, cy) && is_pool(cx, cy, TRUE)){\
				if(check_shore(cx, cy)){\
					if(!cn){\
						makemon(&mons[PM_FORD_ELEMENTAL], cx, cy, NO_MINVENT|MM_ADJACENTOK);\
						i = COLNO;/*break out of outer loop*/\
						break;\
					}\
					else cn--;\
				}\
			}

	pline("The waters of the ford rise to the aid of the guardian!");
	for(; elm > 0; elm--){
		cn = rn2(n);
		for(i = 1; i < COLNO/2; i++){
			high_box_x = guardian->mx + i;
			high_box_y = guardian->my + i;
			low_box_x = guardian->mx - i;
			low_box_y = guardian->my - i;
			for(c = -i; c <= i; c++){
				cx = low_box_x;
				cy = guardian->my+c;
				CHECK_SPAWN

				cx = high_box_x;
				cy = guardian->my+c;
				CHECK_SPAWN

				cx = guardian->mx+c;
				cy = low_box_y;
				CHECK_SPAWN

				cx = guardian->mx+c;
				cy = high_box_y;
				CHECK_SPAWN
			}
		}
	}
}

static NEARDATA const char practical[] = { WEAPON_CLASS, ARMOR_CLASS, GEM_CLASS, FOOD_CLASS, 0 };
static NEARDATA const char magical[] = {
	AMULET_CLASS, POTION_CLASS, SCROLL_CLASS, WAND_CLASS, RING_CLASS,
	SPBOOK_CLASS, TILE_CLASS, SCOIN_CLASS, 0 };
static NEARDATA const char indigestion[] = { BALL_CLASS, ROCK_CLASS, 0 };
static NEARDATA const char boulder_class[] = { ROCK_CLASS, 0 };
static NEARDATA const char gem_class[] = { GEM_CLASS, 0 };
static NEARDATA const char tool_class[] = { TOOL_CLASS, 0 };

boolean
itsstuck(mtmp)
register struct monst *mtmp;
{
	if (sticks(&youmonst) && mtmp==u.ustuck && !u.uswallow) {
		pline("%s cannot escape from you!", Monnam(mtmp));
		return(TRUE);
	}
	return(FALSE);
}

/*
 * smith_move: return 1: moved  0: didn't  -1: let m_move do it  -2: died
 */
int
smith_move(smith)
register struct monst *smith;
{
	register xchar gx,gy,omx,omy;
	schar temple;
	boolean avoid = TRUE;

	omx = smith->mx;
	omy = smith->my;

	if(!on_level(&(ESMT(smith)->frglevel), &u.uz))
		return -1;

	gx = ESMT(smith)->frgpos.x;
	gy = ESMT(smith)->frgpos.y;

	gx += rn1(3,-1);	/* mill around the altar */
	gy += rn1(3,-1);

	if(!smith->mpeaceful || smith->mtame || smith->mberserk ||
	   (Conflict && !resist(smith, RING_CLASS, 0, 0))
	)
	   return -1;
	else if(Invis) avoid = FALSE;

	return(move_special(smith,FALSE,TRUE,FALSE,avoid,omx,omy,gx,gy));
}

/* Return values:
 * 0: did not move, but can still attack and do other stuff.
 * 1: moved, possibly can attack.
 * 2: monster died.
 * 3: did not move, and can't do anything else either.
 */
int
m_move(mtmp, after)
register struct monst *mtmp;
register int after;
{
	int appr;
	xchar gx,gy,nix,niy,chcnt;
	int chi;	/* could be schar except for stupid Sun-2 compiler */
	boolean likegold=0, likegems=0, likeobjs=0, likemagic=0, likefood=0, likemirrors=0, conceals=0;
	boolean likerock=0, can_tunnel=0;
	boolean can_open=0, can_unlock=0, doorbuster=0;
	boolean uses_items=0, setlikes=0;
	boolean avoid=FALSE;
	struct permonst *ptr;
	struct monst *mtoo;
	schar mmoved = 0;	/* not strictly nec.: chi >= 0 will do */
	long info[9];
	long flag;
	int  omx = mtmp->mx, omy = mtmp->my;
	struct obj *mw_tmp;

	if (stationary_mon(mtmp) || sessile(mtmp->data)) return(0);
	if(mtmp->mtrapped) {
	    int i = mintrap(mtmp);
	    if(i >= 2) { newsym(mtmp->mx,mtmp->my); return(2); }/* it died */
	    if(i == 1) return(0);	/* still in trap, so didn't move */
	}
	ptr = mtmp->data; /* mintrap() can change mtmp->data -dlc */

	if (mtmp->meating) {
		if(mtmp->meating < 0){
			pline("ERROR RECOVERY: Negative meating timer (%d) set to 0", mtmp->meating);
			mtmp->meating = 0;
		}
		if(mtmp->meating > 1000){
			pline("ERROR RECOVERY: Overlong meating timer (%d) truncated to 100 turns", mtmp->meating);
			mtmp->meating = 0;
		}
	    else mtmp->meating--;
	    return 3;			/* still eating */
	}
	if (hides_under(ptr) && OBJ_AT(mtmp->mx, mtmp->my) && !mtmp->mtame && rn2(10))
	    return 0;		/* do not leave hiding place */

	set_apparxy(mtmp);
	/* where does mtmp think you are? */
	/* Not necessary if m_move called from this file, but necessary in
	 * other calls of m_move (ex. leprechauns dodging)
	 */
#ifdef REINCARNATION
	if (!Is_rogue_level(&u.uz))
#endif
	    can_tunnel = tunnels(ptr);
	can_open = !(nohands(ptr) || verysmall(ptr) || straitjacketed_mon(mtmp));
	can_unlock = ((can_open && (m_carrying(mtmp, SKELETON_KEY)||m_carrying(mtmp, UNIVERSAL_KEY))) ||
		      mtmp->iswiz || is_rider(ptr) || ptr->mtyp==PM_DREAD_SERAPH);
	doorbuster = species_busts_doors(ptr);
	if(mtmp->wormno) goto not_special;
	/* my dog gets special treatment */
	if(mtmp->mtame) {
	    mmoved = dog_move(mtmp, after);
	    goto postmov;
	}

	/* likewise for shopkeeper */
	if(mtmp->isshk) {
	    mmoved = shk_move(mtmp);
	    if(mmoved == -2) return(2);
	    if(mmoved >= 0) goto postmov;
	    mmoved = 0;		/* follow player outside shop */
	}

	/* and for the guard */
	if(mtmp->isgd) {
	    mmoved = gd_move(mtmp);
	    if(mmoved == -2) return(2);
	    if(mmoved >= 0) goto postmov;
	    mmoved = 0;
	}

	/* and the acquisitive monsters get special treatment */
	if(is_covetous(ptr)) {
	    xchar tx = STRAT_GOALX(mtmp->mstrategy),
		  ty = STRAT_GOALY(mtmp->mstrategy);
	    struct monst *intruder = m_at(tx, ty);
	    /*
	     * if there's a monster on the object or in possesion of it,
	     * attack it.
	     */
	    if((dist2(mtmp->mx, mtmp->my, tx, ty) < 2) &&
	       intruder && (intruder != mtmp)) {
			notonhead = (intruder->mx != tx || intruder->my != ty);
			if(mattackm(mtmp, intruder)&(MM_AGR_DIED)) return(2);
			mmoved = 1;
			goto postmov;
		} else if(mtmp->mtyp != PM_DEMOGORGON 
			   && mtmp->mtyp!=PM_ELDER_PRIEST
			   /*&& mtmp->mtyp!=PM_SHAMI_AMOURAE*/
			   ){
			mmoved = 0;
		    goto postmov;
		}
//	    goto postmov;
	}

	/* and for the priest */
	if(mtmp->ispriest) {
	    mmoved = pri_move(mtmp);
	    if(mmoved == -2) return(2);
	    if(mmoved >= 0) goto postmov;
	    mmoved = 0;
	}

	/* and for smiths */
	if(HAS_ESMT(mtmp)) {
	    mmoved = smith_move(mtmp);
	    if(mmoved == -2) return(2);
	    if(mmoved >= 0) goto postmov;
	    mmoved = 0;
	}

#ifdef MAIL
	if(ptr->mtyp == PM_MAIL_DAEMON) {
	    if(flags.soundok && canseemon(mtmp))
		verbalize("I'm late!");
	    mongone(mtmp);
	    return(2);
	}
#endif

	/* teleport if that lies in our nature */
	if((mteleport(ptr)) && !rn2(5) && !mtmp->mcan &&
	   !tele_restrict(mtmp) && !(noactions(mtmp))
	) {
	    if(mtmp->mhp < 7 || mtmp->mpeaceful || rn2(2)) {
			if (!rloc(mtmp, TRUE)) {
				/* rloc failed, probably due to a full level; don't set mmoved=1 so can still attack later */
				goto postmov;	
			}
		}
	    else
			mnexto(mtmp);
	    mmoved = 1;
	    goto postmov;
	}
not_special:
	if(u.uswallow && !mtmp->mflee && u.ustuck != mtmp) return(1);
	omx = mtmp->mx;
	omy = mtmp->my;
	gx = mtmp->mux;
	gy = mtmp->muy;
	appr = ((mtmp->mflee && mtmp->mtyp != PM_BANDERSNATCH) || mtmp->mtyp == PM_WATERSPOUT) ? -1 : 1;
	if (mtmp->mconf || (u.uswallow && mtmp == u.ustuck))
		appr = 0;
	else {
#ifdef GOLDOBJ
		struct obj *lepgold, *ygold;
#endif
		boolean should_see = mon_can_see_you(mtmp);
		
		if ((youmonst.m_ap_type == M_AP_OBJECT && youmonst.mappearance == STRANGE_OBJECT) || u.uundetected ||
		    (youmonst.m_ap_type == M_AP_OBJECT && youmonst.mappearance == GOLD_PIECE && !likes_gold(ptr)) ||
		    (mtmp->mpeaceful && !mtmp->isshk) ||  /* allow shks to follow */
		    ((monsndx(ptr) == PM_STALKER || is_bat(ptr) || monsndx(ptr) == PM_HUNTING_HORROR ||
		      ptr->mlet == S_LIGHT) && !rn2(3)))
			appr = 0;

		if(monsndx(ptr) == PM_LEPRECHAUN && (appr == 1) &&
#ifndef GOLDOBJ
		   (mtmp->mgold > u.ugold))
#else
		   ( (lepgold = findgold(mtmp->minvent)) && 
                   (lepgold->quan > ((ygold = findgold(invent)) ? ygold->quan : 0L)) ))
#endif
			appr = -1;

		if(monsndx(ptr) == PM_LUCKSUCKER && (appr == 1) && mtmp->mvar_lucksucker > 0 && u.uluck < mtmp->mvar_lucksucker)
			appr = -1;

		/* monsters can track you */
		if (!should_see && (goodsmeller(ptr) || (mtmp)->mcansee)) {
			register coord *cp;
			/* good trackers can follow your trail up to 200 turns, others just for your few most recent steps */
			cp = gettrack(omx, omy, can_track(ptr) ? 0 : rnd(ACURR_MON(A_INT, mtmp)/3));
			if (cp) {
				gx = cp->x;
				gy = cp->y;
			}
		}
	}
	/* if monster has no idea where you could be, set appr to 0 */
	if (gx == 0 && gy == 0)
		appr = 0;

	if (( !mtmp->mpeaceful || mtmp->mtyp == PM_MAID || !rn2(10) )
#ifdef REINCARNATION
				    && (!Is_rogue_level(&u.uz))
#endif
	) {
	    boolean in_line = lined_up(mtmp) &&
		!no_upos(mtmp) &&
		(distmin(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <=
		    (throws_rocks(youracedata) ? 20 : ACURRSTR/2+1)
		);

	    if (appr == 0 || !in_line) {
			/* Monsters in combat or fleeing won't pick stuff up, avoiding the
			 * situation where you toss arrows at it and it has nothing
			 * better to do than pick the arrows up.
			 */
			register int pctload = (curr_mon_load(mtmp) * 100) /
				max_mon_load(mtmp);
			
			if(!mtmp->menvy){
				/* look for gold or jewels nearby */
				likegold = (likes_gold(ptr) && pctload < 95);
				likegems = (likes_gems(ptr) && pctload < 85);
				uses_items = (!mindless_mon(mtmp) && !is_animal(ptr)
					&& pctload < 75);
				likeobjs = (!mad_no_armor(mtmp) && likes_objs(ptr) && pctload < 75);
				likemagic = (likes_magic(ptr) && pctload < 85);
				likerock = (throws_rocks(ptr) && pctload < 50 && !In_sokoban(&u.uz));
				likefood = mtmp->mgluttony;
				likemirrors = (mtmp->margent && pctload < 100);
				conceals = hides_under(ptr);
				setlikes = TRUE;
			}
	    }
	}

#define SQSRCHRADIUS	5

	{
	register int minr = SQSRCHRADIUS;	/* not too far away */
	register struct obj *otmp;
	register int xx, yy;
	int oomx, oomy, lmx, lmy;
	
	/* cut down the search radius if it thinks character is closer. */
	if(distmin(mtmp->mux, mtmp->muy, omx, omy) < SQSRCHRADIUS &&
		!no_upos(mtmp) &&
	    !mtmp->mpeaceful) minr--;
	/* guards shouldn't get too distracted */
	if(!mtmp->mpeaceful && is_mercenary(ptr)) minr = 1;

	if((likegold || likegems || likeobjs || likemagic || likerock || likefood || likemirrors || conceals)
	      && (!*in_rooms(omx, omy, SHOPBASE) || (!rn2(25) && !mtmp->isshk))) {
	look_for_obj:
	    oomx = min(COLNO-1, omx+minr);
	    oomy = min(ROWNO-1, omy+minr);
	    lmx = max(1, omx-minr);
	    lmy = max(0, omy-minr);
	    for(otmp = fobj; otmp; otmp = otmp->nobj) {
		/* monsters may pick rocks up, but won't go out of their way
		   to grab them; this might hamper sling wielders, but it cuts
		   down on move overhead by filtering out most common item */
		if (otmp->otyp == ROCK) continue;
		/* skip objects not in line-of-sight */
		if (!((mtmp->mx == otmp->ox && mtmp->my == otmp->oy)
			|| clear_path(mtmp->mx, mtmp->my, otmp->ox, otmp->oy))) continue;
		xx = otmp->ox;
		yy = otmp->oy;
		/* Nymphs take everything.  Most other creatures should not
		 * pick up corpses except as a special case like in
		 * searches_for_item().  We need to do this check in
		 * mpickstuff() as well.
		 */
		if(xx >= lmx && xx <= oomx && yy >= lmy && yy <= oomy) {
		    /* don't get stuck circling around an object that's underneath
		       an immobile or hidden monster; paralysis victims excluded */
		    if ((mtoo = m_at(xx,yy)) != 0 &&
			(mtoo->msleeping || mtoo->mundetected ||
			 (mtoo->mappearance && !mtoo->iswiz) ||
			 !mtoo->data->mmove)) continue;
			
			if ((
				(likegold && (
					otmp->oclass == COIN_CLASS)) ||
				(likeobjs && (
					index(practical, otmp->oclass) &&
					(otmp->otyp != CORPSE || (ptr->mlet == S_NYMPH && !is_rider(&mons[otmp->corpsenm]))))) ||
				(likemagic && (
					index(magical, otmp->oclass))) ||
				(uses_items && (
					searches_for_item(mtmp, otmp))) ||
				(likerock && (
					is_boulder(otmp))) ||
				(likegems && (
					otmp->oclass == GEM_CLASS && otmp->obj_material != MINERAL)) ||
				(likefood && (
					otmp->oclass == FOOD_CLASS || (otmp->obj_material == VEGGY && herbivorous(ptr)) || (otmp->obj_material == FLESH && carnivorous(ptr)) )) ||
				(likemirrors && (
					otmp->otyp == MIRROR || item_has_property(otmp, REFLECTING) )) ||
				(conceals && (
					!cansee(otmp->ox, otmp->oy))) ||
				((ptr->mtyp == PM_GELATINOUS_CUBE || ptr->mtyp == PM_ANCIENT_OF_CORRUPTION) && (
					!index(indigestion, otmp->oclass) && !(otmp->otyp == CORPSE && touch_petrifies(&mons[otmp->corpsenm]))))
				)
				&&
				(
				/* can carry obj */
				can_carry(mtmp, otmp) &&
				/* not covered by a boulder */
				(throws_rocks(ptr) || !boulder_at(xx, yy)) &&
				/* unicorns only like gems */
				!(is_unicorn(ptr) && otmp->obj_material != GEMSTONE) &&
				/* Don't get stuck circling an Elbereth */
				!(onscary(xx, yy, mtmp)) &&
				/* Don't go for untouchable artifacts */
				!(otmp->oartifact && !touch_artifact(otmp, mtmp, TRUE)) &&
				/* Don't go for underwater items */
				!is_pool(xx, yy, FALSE)
				)
				)
			{
				minr = distmin(omx,omy,xx,yy);
				oomx = min(COLNO-1, omx+minr);
				oomy = min(ROWNO-1, omy+minr);
				lmx = max(1, omx-minr);
				lmy = max(0, omy-minr);
				gx = otmp->ox;
				gy = otmp->oy;
				if(mtmp->mtyp == PM_MAID && appr == 0) appr = 1;
				if (gx == omx && gy == omy) {
					mmoved = 3; /* actually unnecessary */
					goto postmov;
				}
		    }
		}
	    }
	} else if(likegold) {
	    /* don't try to pick up anything else, but use the same loop */
	    uses_items = 0;
	    likegems = likeobjs = likemagic = likerock = likefood = likemirrors = conceals = 0;
	    goto look_for_obj;
	}

	if(minr < SQSRCHRADIUS && appr == -1) {
	    if(!no_upos(mtmp) && distmin(omx,omy,mtmp->mux,mtmp->muy) <= 3) {
		gx = mtmp->mux;
		gy = mtmp->muy;
	    } else
		appr = 1;
	}
	}

	/* don't tunnel if hostile and close enough to prefer a weapon */
	if (can_tunnel && needspick(ptr) &&
	    ((!mtmp->mpeaceful || Conflict || mtmp->mberserk) && !no_upos(mtmp) && 
	     dist2(mtmp->mx, mtmp->my, mtmp->mux, mtmp->muy) <= 8))
	    can_tunnel = FALSE;
	/* can't tunnel if stuck in a straitjacket */
	if (can_tunnel && needspick(ptr) && straitjacketed_mon(mtmp))
	    can_tunnel = FALSE;

	nix = omx;
	niy = omy;
	flag = 0L;
	if (mtmp->mpeaceful && !mtmp->mberserk && (!Conflict || resist(mtmp, RING_CLASS, 0, 0)))
	    flag |= (ALLOW_SANCT | ALLOW_SSM);
	else flag |= ALLOW_U;
	if (is_minion(ptr) || is_rider(ptr)) flag |= ALLOW_SANCT;
	/* unicorn may not be able to avoid hero on a noteleport level */
	if (notonline(ptr) && ((mon_resistance(mtmp,TELEPORT) || is_unicorn(ptr)) && !notel_level())) flag |= NOTONL;
	if (mon_resistance(mtmp,PASSES_WALLS)) flag |= (ALLOW_WALL | ALLOW_ROCK);
	if (passes_bars(mtmp) && !Is_illregrd(&u.uz) ) flag |= ALLOW_BARS;
	if (can_tunnel) flag |= ALLOW_DIG;
	if (is_human(ptr) || ptr->mtyp == PM_MINOTAUR) flag |= ALLOW_SSM;
	if (is_undead(ptr) && ptr->mlet != S_GHOST && ptr->mlet != S_SHADE) flag |= NOGARLIC;
	if (throws_rocks(ptr)) flag |= ALLOW_ROCK;
	if (can_open) flag |= OPENDOOR;
	if (can_unlock) flag |= UNLOCKDOOR;
	if (doorbuster) flag |= BUSTDOOR;
	{
	    register int i, j, nx, ny, nearer;
		int leader_target = FALSE;
	    int jcnt, cnt;
	    int ndist, nidist;
	    register coord *mtrk;
	    coord poss[9];

	    cnt = mfndpos(mtmp, poss, info, flag);
	    chcnt = 0;
	    jcnt = min(MTSZ, cnt-1);
	    chi = -1;
	    nidist = dist2(nix,niy,gx,gy);
	    /* allow monsters be shortsighted on some levels for balance */
	    if(!mtmp->mpeaceful && level.flags.shortsighted &&
	       nidist > (couldsee(nix,niy) ? 144 : 36) && appr == 1) appr = 0;
	    if (notonline(ptr)){
			int curappr = appr;
			if(appr == 1){
				appr = -1; //If approaching, default to running away instead
			}
			/* on noteleport levels, perhaps we cannot avoid hero */
			for(i = 0; i < cnt; i++){
				if(!(info[i] & NOTONL)) avoid=TRUE;
				else appr = curappr; //If was approaching and swithced to running away, reinstates approaching
			}
	    }
		
		if(appr == 0){
			struct monst *m2 = (struct monst *)0;
			int distminbest = BOLT_LIM;
			for(m2=fmon; m2; m2 = m2->nmon){
				if(distmin(mtmp->mx,mtmp->my,m2->mx,m2->my) < distminbest
						&& mm_aggression(mtmp,m2)
						&& (clear_path(mtmp->mx, mtmp->my, m2->mx, m2->my) || !rn2(10))
						&& mon_can_see_mon(mtmp, m2)
				){
					distminbest = distmin(mtmp->mx,mtmp->my,m2->mx,m2->my);
					leader_target = FALSE;
					gx = m2->mx;
					gy = m2->my;
					mtmp->mux = m2->mx;
					mtmp->muy = m2->my;
					appr = 1;
				}
			}//End target closest hostile
			
			if(Role_if(PM_ANACHRONONAUT) && !mtmp->mpeaceful && In_quest(&u.uz) && !quest_status.leader_is_dead){
				for(m2=fmon; m2; m2 = m2->nmon){
					if(m2->m_id == quest_status.leader_m_id){
						if(distminbest >= SQSRCHRADIUS){
							/*make a beeline for the leader*/
							leader_target = TRUE;
							gx = m2->mx;
							gy = m2->my;
							mtmp->mux = m2->mx;
							mtmp->muy = m2->my;
							appr = 1;
						}
						break;
					}
			}
			}//End target ana leader
		}
		
		//Loop to place breaching charge on ana quest home
		if(Role_if(PM_ANACHRONONAUT) && !mtmp->mpeaceful && In_quest(&u.uz) && Is_qstart(&u.uz) && !quest_status.leader_is_dead){
			if(
				(mtmp->mtyp == PM_DEEP_ONE && !rn2(1000))
				|| (mtmp->mtyp == PM_DEEPER_ONE && !rn2(500))
				|| (mtmp->mtyp == PM_DEEPEST_ONE && !rn2(100))
			){
				int i, j, count=0, tx = 0, ty = 0;
				if(leader_target){
					tx = sgn(gx - mtmp->mx); 
					ty = sgn(gy - mtmp->my); 
				}
				
				if(!leader_target || !isok(tx,ty) || !IS_WALL(levl[tx][ty].typ)){
					tx = 0;
					ty = 0;
					for(i = -1; i < 2; i++) for(j = -1; j < 2; j++){
						if(isok(mtmp->mx+i, mtmp->my+j) && IS_WALL(levl[mtmp->mx+i][mtmp->my+j].typ)){
							count++;
						}
					}
					if(count){
						count = rnd(count);
						for(i = -1; i < 2 && count > 0; i++) for(j = -1; j < 2 && count > 0; j++){
							tx = mtmp->mx+i;
							ty = mtmp->my+j;
							if(isok(tx, ty) && IS_WALL(levl[tx][ty].typ)){
								if(--count == 0)
									break;
							}
						}
					}
				}
				if((tx || ty) && isok(tx, ty)){
					struct obj *breacher;
					breacher = mksobj(STICK_OF_DYNAMITE, MKOBJ_NOINIT);
					breacher->quan = 1L;
					breacher->cursed = 0;
					breacher->blessed = 0;
					breacher->age = rn1(10,10);
					fix_object(breacher);
					place_object(breacher, tx, ty);
					begin_burn(breacher);
					if(canseemon(mtmp))
						pline("%s plants a breaching charge!", Monnam(mtmp));
				}
			} else if((mtmp->mtyp == PM_DEEP_ONE && !rn2(6))
				|| (mtmp->mtyp == PM_DEEPER_ONE && !rn2(3))
				|| (mtmp->mtyp == PM_DEEPEST_ONE)
			){
				int i, j, count=0;
				struct trap *ttmp;
				for(i = -1; i < 2; i++) for(j = -1; j < 2; j++){
					if(isok(mtmp->mx+i, mtmp->my+j) && (ttmp = t_at(mtmp->mx+i, mtmp->my+j)) && (ttmp->ttyp == PIT || ttmp->ttyp == WEB)){
						count++;
					}
				}
				if(count){
					count = rn2(count);
					for(i = -1; i < 2; i++) for(j = -1; j < 2; j++){
						if(isok(mtmp->mx+i, mtmp->my+j)  && (ttmp = t_at(mtmp->mx+i, mtmp->my+j)) && (ttmp->ttyp == PIT || ttmp->ttyp == WEB)){
							if(count-- == 0){
								if(ttmp->ttyp == PIT){
									if(canseemon(mtmp))
										pline("%s fills in a trench!", Monnam(mtmp));
								}
								else if(ttmp->ttyp == WEB){
									if(canseemon(mtmp))
										pline("%s clears some webbing!", Monnam(mtmp));
								}
									delfloortrap(t_at(mtmp->mx+i, mtmp->my+j));
							}
						}
					}
				}
			}
		}
		
	    for(i=0; i < cnt; i++) {
			if (avoid && (info[i] & NOTONL)) continue;
			nx = poss[i].x;
			ny = poss[i].y;

			if (appr != 0) {
				mtrk = &mtmp->mtrack[0];
				for(j=0; j < jcnt; mtrk++, j++)
				if(nx == mtrk->x && ny == mtrk->y)
					if(rn2(4*(cnt-j)))
					goto nxti;
			}

			nearer = ((ndist = dist2(nx,ny,gx,gy)) < nidist);

			if((appr == 1 && nearer) || (appr == -1 && !nearer) ||
			   (!appr && !rn2(++chcnt)) || !mmoved) {
				nix = nx;
				niy = ny;
				nidist = ndist;
				chi = i;
				mmoved = 1;
			}
			nxti:	;
	    }
	}

	if(mmoved) {
	    register int j;

	    if (mmoved==1 && (u.ux != nix || u.uy != niy) && itsstuck(mtmp))
		return(3);

	    if (((IS_ROCK(levl[nix][niy].typ) && may_dig(nix,niy)) ||
		 closed_door(nix, niy)) &&
		mmoved==1 && can_tunnel && needspick(ptr)) {
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
			if (mon_wield_item(mtmp))
				return(3);	/* did not move, spent turn wielding item */
			else
				return(0);	/* can't move into that position, but didn't take time wielding item */
		}
	    }
	    /* If ALLOW_U is set, either it's trying to attack you, or it
	     * thinks it is.  In either case, attack this spot in preference to
	     * all others.
	     */
	/* Actually, this whole section of code doesn't work as you'd expect.
	 * Most attacks are handled in dochug().  It calls distfleeck(), which
	 * among other things sets nearby if the monster is near you--and if
	 * nearby is set, we never call m_move unless it is a special case
	 * (confused, stun, etc.)  The effect is that this ALLOW_U (and
	 * mfndpos) has no effect for normal attacks, though it lets a confused
	 * monster attack you by accident.
	 */
	    if(info[chi] & ALLOW_U) {
		nix = mtmp->mux;
		niy = mtmp->muy;
	    }
	    if (nix == u.ux && niy == u.uy) {
		mtmp->mux = u.ux;
		mtmp->muy = u.uy;
		return(0);
	    }
	    /* The monster may attack another based on 1 of 2 conditions:
	     * 1 - It may be confused.
	     * 2 - It may mistake the monster for your (displaced) image.
	     * Pets get taken care of above and shouldn't reach this code.
	     * Conflict gets handled even farther away (movemon()).
	     */
		/* Actually, it seems to be THREE conditions:
		 * 3 - It may hate the other monster, as per mm_aggression in mon.c.
		 * Pet-to-pet combat seems to be handled elsewhere, though
		 */
	    if((info[chi] & ALLOW_M) ||
		   (nix == mtmp->mux && niy == mtmp->muy)) {
		struct monst *mtmp2;
		int mstatus;
		mtmp2 = m_at(nix,niy);

		notonhead = mtmp2 && (nix != mtmp2->mx || niy != mtmp2->my);
		/* note: mstatus returns 0 if mtmp2 is nonexistent */
		mstatus = mattackm(mtmp, mtmp2);
		
		if (mstatus & MM_AGR_DIED)		/* aggressor died */
		    return 2;

		if ((mstatus & MM_HIT) && !(mstatus & MM_DEF_DIED)  &&
		    rn2(4) && mtmp2->movement >= NORMAL_SPEED
		){
			mtmp2->movement -= NORMAL_SPEED;
		    notonhead = 0;
		    mstatus = mattackm(mtmp2, mtmp);	/* return attack */
		    if (mstatus & MM_DEF_DIED)
			return 2;
		}
		return 3;
	    }

	    if (!m_in_out_region(mtmp,nix,niy))
	        return 3;
	    remove_monster(omx, omy);
	    place_monster(mtmp, nix, niy);
	    for(j = MTSZ-1; j > 0; j--)
		mtmp->mtrack[j] = mtmp->mtrack[j-1];
	    mtmp->mtrack[0].x = omx;
	    mtmp->mtrack[0].y = omy;
		if(mtmp->mtyp == PM_HEMORRHAGIC_THING){
			//The thing leaves bloody footprints, that appear *after* the monster has passed by
			if(isok(mtmp->mtrack[1].x, mtmp->mtrack[1].y) && mtmp->mtrack[1].x != 0){
				struct engr *oep = engr_at(mtmp->mtrack[1].x, mtmp->mtrack[1].y);
				if(!oep){
					make_engr_at(mtmp->mtrack[1].x, mtmp->mtrack[1].y,
					 "", 0L, DUST);
					oep = engr_at(mtmp->mtrack[1].x, mtmp->mtrack[1].y);
				}
				if (oep && (oep->ward_type == 0)){	/* cannot replace an existing ward here */
					oep->ward_id = FOOTPRINT;
					oep->halu_ward = 1;
					oep->ward_type = ENGR_BLOOD;
					oep->complete_wards = 1;
				}
			}
		}
	    /* Place a segment at the old position. */
	    if (mtmp->wormno) worm_move(mtmp);
		
		if(mtmp->mtyp == PM_SURYA_DEVA){
			struct monst *blade;
			for(blade = fmon; blade; blade = blade->nmon)
				if(blade->mtyp == PM_DANCING_BLADE && mtmp->m_id == blade->mvar_suryaID && !DEADMONSTER(blade))
					break;
			if(blade){
				int bx = blade->mx, by = blade->my;
				remove_monster(bx, by);
				place_monster(blade, omx, omy);
				newsym(omx,omy);
				newsym(bx,by);
			}
		}
	} else {
	    if(is_unicorn(ptr) && rn2(2) && !tele_restrict(mtmp) && !noactions(mtmp))
		{
			if(rloc(mtmp, TRUE))
				return(1);
	    }
	    if(mtmp->wormno) worm_nomove(mtmp);
	}
postmov:
	if(mmoved == 1 || mmoved == 3) {
	    boolean canseeit = cansee(mtmp->mx, mtmp->my);

	    if(mmoved == 1) {
		newsym(omx,omy);		/* update the old position */
		if (mintrap(mtmp) >= 2) {
		    if(mtmp->mx) newsym(mtmp->mx,mtmp->my);
		    return(2);	/* it died */
		}
		ptr = mtmp->data;

		/* open a door, or crash through it, if you can */
		if(IS_DOOR(levl[mtmp->mx][mtmp->my].typ)
			&& !mon_resistance(mtmp,PASSES_WALLS) /* doesn't need to open doors */
			&& !can_tunnel /* taken care of below */
		      ) {
		    struct rm *here = &levl[mtmp->mx][mtmp->my];
		    boolean btrapped = (here->doormask & D_TRAPPED);

		    if(here->doormask & (D_LOCKED|D_CLOSED) && amorphous(ptr)) {
			if (flags.verbose && canseemon(mtmp))
			    pline("%s %s under the door.", Monnam(mtmp),
				  (ptr->mtyp == PM_FOG_CLOUD ||
				   ptr->mtyp == PM_YELLOW_LIGHT)
				  ? "flows" : "oozes");
		    } else if(here->doormask & D_LOCKED && can_unlock) {
			if(btrapped) {
			    here->doormask = D_NODOOR;
			    newsym(mtmp->mx, mtmp->my);
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			    if(mb_trapped(mtmp)) return(2);
			} else {
			    if (flags.verbose) {
				if (canseeit)
				   You("see a door unlock and open.");
				else if (flags.soundok)
				   You_hear("a door unlock and open.");
			    }
			    here->doormask = D_ISOPEN;
			    /* newsym(mtmp->mx, mtmp->my); */
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			}
		    } else if (here->doormask == D_CLOSED && can_open) {
			if(btrapped) {
			    here->doormask = D_NODOOR;
			    newsym(mtmp->mx, mtmp->my);
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			    if(mb_trapped(mtmp)) return(2);
			} else {
			    if (flags.verbose) {
				if (canseeit)
				     You("see a door open.");
				else if (flags.soundok)
				     You_hear("a door open.");
			    }
			    here->doormask = D_ISOPEN;
			    /* newsym(mtmp->mx, mtmp->my); */  /* done below */
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			}
		    } else if (here->doormask & (D_LOCKED|D_CLOSED)) {
			/* mfndpos guarantees this must be a doorbuster */
			if(btrapped) {
			    here->doormask = D_NODOOR;
			    newsym(mtmp->mx, mtmp->my);
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			    if(mb_trapped(mtmp)) return(2);
			} else {
			    if (flags.verbose) {
				if (canseeit)
				    You("see a door crash open.");
				else if (flags.soundok)
				    You_hear("a door crash open.");
			    }
			    if (here->doormask & D_LOCKED && !rn2(2))
				    here->doormask = D_NODOOR;
			    else here->doormask = D_BROKEN;
			    /* newsym(mtmp->mx, mtmp->my); */ /* done below */
			    unblock_point(mtmp->mx,mtmp->my); /* vision */
			}
			/* if it's a shop door, schedule repair */
			if (*in_rooms(mtmp->mx, mtmp->my, SHOPBASE))
			    add_damage(mtmp->mx, mtmp->my, 0L);
		    }
		} else if (levl[mtmp->mx][mtmp->my].typ == IRONBARS && !Is_illregrd(&u.uz)) {
		    if ( (dmgtype(ptr,AD_RUST) && ptr->mtyp != PM_NAIAD) || dmgtype(ptr,AD_CORR)) {
				if (canseemon(mtmp)) {
					pline("%s eats through the iron bars.", 
					Monnam(mtmp)); 
				}
				dissolve_bars(mtmp->mx, mtmp->my);
				if(mtmp->mtyp == PM_RUST_MONSTER && get_mx(mtmp, MX_EDOG)){
					EDOG(mtmp)->hungrytime += 5*objects[BAR].oc_nutrition;
				}
			}
		    else if (flags.verbose && canseemon(mtmp))
				Norep("%s %s %s the iron bars.", Monnam(mtmp),
				  /* pluralization fakes verb conjugation */
				  makeplural(locomotion(mtmp, "pass")),
				  mon_resistance(mtmp,PASSES_WALLS) ? "through" : "between");
		}
		if (mtmp->mtemplate == WORLD_SHAPER) {
			mworldshape(mtmp, omx, omy);
		}
		/* possibly dig */
		if (can_tunnel && mdig_tunnel(mtmp))
			return(2);  /* mon died (position already updated) */

		/* set also in domove(), hack.c */
		if (u.uswallow && mtmp == u.ustuck &&
					(mtmp->mx != omx || mtmp->my != omy)) {
		    /* If the monster moved, then update */
		    u.ux0 = u.ux;
		    u.uy0 = u.uy;
		    u.ux = mtmp->mx;
		    u.uy = mtmp->my;
		    swallowed(0);
		} else
		newsym(mtmp->mx,mtmp->my);
	    }
	    if(OBJ_AT(mtmp->mx, mtmp->my) && mtmp->mcanmove && mtmp->mnotlaugh) {
		/* recompute the likes tests, in case we polymorphed
		 * or if the "likegold" case got taken above */
		if (setlikes) {
		    register int pctload = (curr_mon_load(mtmp) * 100) /
			max_mon_load(mtmp);

		    /* look for gold or jewels nearby */
		    likegold = (likes_gold(ptr) && pctload < 95);
		    likegems = (likes_gems(ptr) && pctload < 85);
		    uses_items = (!mindless_mon(mtmp) && !is_animal(ptr)
				  && pctload < 75);
		    likeobjs = (!mad_no_armor(mtmp) && likes_objs(ptr) && pctload < 75);
		    likemagic = (likes_magic(ptr) && pctload < 85);
		    likerock = (throws_rocks(ptr) && pctload < 50 &&
				!In_sokoban(&u.uz));
		    conceals = hides_under(ptr);
		}

		if(ptr->mtyp == PM_NACHASH_TANNIN){
			mvanishobj(mtmp, mtmp->mx, mtmp->my);
		}

		/* Maybe a cube ate just about anything */
		if (ptr->mtyp == PM_GELATINOUS_CUBE || ptr->mtyp == PM_ANCIENT_OF_CORRUPTION) {
		    if (meatobj(mtmp) == 2) return 2;	/* it died */
		}
		/* Maybe a rock mole just ate some metal object */
		else if (metallivorous(ptr)) {
		    if (meatmetal(mtmp) == 2) return 2;	/* it died */
		}
		/* Maybe a gluttonous monster just ate some food */
		else if (mtmp->mgluttony || mtmp->mcannibal) {
		    if (meatgluttony(mtmp) == 2) return 2;	/* it died */
		}

		if(g_at(mtmp->mx,mtmp->my) && likegold) mpickgold(mtmp);

		if(!*in_rooms(mtmp->mx, mtmp->my, SHOPBASE) || !rn2(25)) {
		    boolean picked = FALSE;

		    if(likeobjs) picked |= mpickstuff(mtmp, practical);
		    if(likemagic) picked |= mpickstuff(mtmp, magical);
		    if(likerock) picked |= mpickstuff(mtmp, boulder_class);
		    if(likegems) picked |= mpickstuff(mtmp, gem_class);
		    if(likemirrors) picked |= mpickstuff(mtmp, tool_class);
		    if(uses_items) picked |= mpickstuff(mtmp, (char *)0);
		    if(picked) mmoved = 3;
		}

		if(mtmp->minvis) {
		    newsym(mtmp->mx, mtmp->my);
		    if (mtmp->wormno) see_wsegs(mtmp);
		}
	    }

	    if(hides_under(ptr) || is_underswimmer(ptr)) {
		/* Always set--or reset--mundetected if it's already hidden
		   (just in case the object it was hiding under went away);
		   usually set mundetected unless monster can't move.  */
		if (mtmp->mundetected ||
			(mtmp->mcanmove && mtmp->mnotlaugh && !mtmp->msleeping && rn2(5)))
		    mtmp->mundetected = (!is_underswimmer(ptr)) ?
			OBJ_AT(mtmp->mx, mtmp->my) :
			(is_pool(mtmp->mx, mtmp->my, FALSE) && !Is_waterlevel(&u.uz));
		newsym(mtmp->mx, mtmp->my);
	    }
	    if (mtmp->isshk) {
		after_shk_move(mtmp);
	    }
	}
	return(mmoved);
}

#endif /* OVL0 */
#ifdef OVL2

/* break iron bars at the given location */
void
break_iron_bars(x, y, heard)
int x, y;			/* coordinates of iron bars */
boolean heard;		/* print You_hear() message? */
{
	int numbars;
	struct obj *obj;

	if (levl[x][y].typ != IRONBARS) {
		impossible("Breaking non-existant iron bars @ (%d,%d)", x, y);
		return;
	}

	if (heard)
		You_hear("a sharp crack!");
	levl[x][y].typ = (Is_special(&u.uz) || *in_rooms(x, y, 0)) ? ROOM : CORR;

	for (numbars = d(2, 4) - 1; numbars > 0; numbars--){
		obj = mksobj_at(BAR, x, y, MKOBJ_NOINIT);
		set_material_gm(obj, Is_illregrd(&u.uz) ? METAL : IRON);
		obj->spe = 0;
		obj->cursed = obj->blessed = FALSE;
	}
	newsym(x, y);
	return;
}

//Malcolm Ryan's bar eating patch
void
dissolve_bars(x, y)
register int x, y;
{
    levl[x][y].typ = (Is_special(&u.uz) || *in_rooms(x,y,0)) ? ROOM : CORR; 
    newsym(x, y);    
}


boolean
closed_door(x, y)
register int x, y;
{
	return((boolean)(IS_DOOR(levl[x][y].typ) &&
			(levl[x][y].doormask & (D_LOCKED | D_CLOSED))));
}

boolean
accessible(x, y)
register int x, y;
{
	return((boolean)(ACCESSIBLE(levl[x][y].typ) && !closed_door(x, y)));
}

#endif /* OVL2 */
#ifdef OVL0

/* decide where the monster thinks you are standing */
void
set_apparxy(mtmp)
register struct monst *mtmp;
{
	boolean notseen, gotu = FALSE;
	register int disp, mx = mtmp->mux, my = mtmp->muy;
#ifdef GOLDOBJ
	long umoney = money_cnt(invent);
#endif

	/*
	 * do cheapest and/or most likely tests first
	 */

	/* pet knows your smell; grabber still has hold of you */
	if (mtmp->mtame || mtmp == u.ustuck) goto found_you;

	/* monsters which know where you are don't suddenly forget,
	   if you haven't moved away.  Assuming they aren't crazy. */
	if (mx == u.ux && my == u.uy && (!mtmp->mcrazed || rn2(4))) goto found_you;

	notseen = !mon_can_see_you(mtmp);
	/* Can also learn your position via hearing you */
	if(couldsee(mtmp->mx,mtmp->my) &&
		distu(mtmp->mx,mtmp->my) <= 100 &&
		((sensitive_ears(mtmp->data) || !rn2(7)) && !is_deaf(mtmp)) &&
		(!Stealth || (mtmp->mtyp == PM_ETTIN && rn2(10)))
	) {
		notseen = FALSE;
	}
	/* Or by magical means */
	if(Aggravate_monster 
			|| (Withering_stake && mvitals[PM_MOON_S_CHOSEN].died && (mtmp->data->mflagsa&MA_ANIMAL || mtmp->data->mflagsa&MA_DEMIHUMAN || mtmp->data->mflagsa&MA_WERE))
	) {
		notseen = FALSE;
	}
	/* add cases as required.  eg. Displacement ... */
	if(notseen){
		if((distmin(mtmp->mx,mtmp->my,mx,my) <= 1 && distmin(mtmp->mx,mtmp->my,u.ux,u.uy) > 1) || !rn2(100)){
			/*well, now it has NO clue where you are...*/
			mtmp->mux = 0;
			mtmp->muy = 0;
		}
		if(is_wanderer(mtmp->data) && !rn2(20)){
			/*x goes from 1 to colno-1, y goes from 0 to rowno-1*/
			mtmp->mux = rnd(COLNO-1);
			mtmp->muy = rn2(ROWNO);
		}
		/* Otherwise, don't change the current */
		return;
	} else if ( Underwater) {
		disp = 1;
	} else if (Displaced || mtmp->mcrazed) {
	    disp = couldsee(mx, my) ? 2 : 1;
	} else disp = 0;
	if (!disp) goto found_you;

	/* without something like the following, invis. and displ.
	   are too powerful */
	gotu = Displaced ? !rn2(4) : FALSE;

#if 0		/* this never worked as intended & isn't needed anyway */
	/* If invis but not displaced, staying around gets you 'discovered' */
	gotu |= (!Displaced && u.dx == 0 && u.dy == 0);
#endif

	if (!gotu) {
	    register int try_cnt = 0;
	    do {
		if (++try_cnt > 200) goto found_you;		/* punt */
		mx = u.ux - disp + rn2(2*disp+1);
		my = u.uy - disp + rn2(2*disp+1);
	    } while (!isok(mx,my)
		  || (disp != 2 && mx == mtmp->mx && my == mtmp->my)
		  || ((mx != u.ux || my != u.uy) &&
		      !mon_resistance(mtmp,PASSES_WALLS) &&
		      (!ACCESSIBLE(levl[mx][my].typ) ||
		       (closed_door(mx, my) && !can_ooze(mtmp))))
		  || !couldsee(mx, my));
	} else {
found_you:
	    mx = u.ux;
	    my = u.uy;
	}

	mtmp->mux = mx;
	mtmp->muy = my;
}

boolean
can_ooze(mtmp)
struct monst *mtmp;
{
	struct obj *chain, *obj;

	if (!amorphous(mtmp->data)) return FALSE;
	if (mtmp == &youmonst) {
#ifndef GOLDOBJ
		if (u.ugold > 100L) return FALSE;
#endif
		chain = invent;
	} else {
#ifndef GOLDOBJ
		if (mtmp->mgold > 100L) return FALSE;
#endif
		chain = mtmp->minvent;
	}
	for (obj = chain; obj; obj = obj->nobj) {
		int typ = obj->otyp;

#ifdef GOLDOBJ
                if (typ == COIN_CLASS && obj->quan > 100L) return FALSE;
#endif
		if (obj->oclass != GEM_CLASS &&
		    !(typ >= ARROW && typ <= BOOMERANG) &&
		    !(typ >= DAGGER && typ <= CRYSKNIFE) &&
		    typ != SLING &&
		    !is_cloak(obj) && typ != FEDORA &&
		    !is_gloves(obj) && typ != JACKET &&
#ifdef TOURIST
		    typ != CREDIT_CARD && !is_shirt(obj) &&
#endif
		    !(typ == CORPSE && verysmall(&mons[obj->corpsenm])) &&
		    typ != FORTUNE_COOKIE && typ != CANDY_BAR &&
		    typ != PANCAKE && typ != LEMBAS_WAFER &&
		    !(typ >= LUMP_OF_ROYAL_JELLY && typ <= HONEYCOMB) &&
		    obj->oclass != AMULET_CLASS &&
		    obj->oclass != RING_CLASS &&
#ifdef WIZARD
		    obj->oclass != VENOM_CLASS &&
#endif
		    typ != SACK && typ != BAG_OF_HOLDING &&
		    typ != BAG_OF_TRICKS && !Is_candle(obj) &&
		    typ != OILSKIN_SACK && typ != LEASH &&
		    typ != STETHOSCOPE && typ != BLINDFOLD && 
		    typ != ANDROID_VISOR && 
			typ != TOWEL && typ != R_LYEHIAN_FACEPLATE &&
		    typ != WHISTLE && typ != MAGIC_WHISTLE &&
		    typ != MAGIC_MARKER && typ != TIN_OPENER &&
		    typ != SKELETON_KEY && typ != UNIVERSAL_KEY &&
			typ != LOCK_PICK
		) return FALSE;
		if (Is_container(obj) && obj->cobj) return FALSE;
		    
	}
	return TRUE;
}

void
remove_monster(x, y)
int x;
int y;
{
	struct monst * mtmp = level.monsters[x][y];
	
	if(mtmp){
		if (opaque(mtmp->data))
			unblock_point(mtmp->mx, mtmp->my);
		mtmp = (struct monst *)0;
		level.monsters[x][y] = (struct monst *)0;
	}
	return;
}

#endif /* OVL0 */

/*monmove.c*/
