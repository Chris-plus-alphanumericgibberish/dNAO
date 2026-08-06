/*	SCCS Id: @(#)minion.c	3.4	2003/01/09	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#include "artifact.h"

extern const int monstr[];

/* Select which of godnum's minions to summon based on current level difficulty.
 * Returns a PM index, or NON_PM if no suitable minion exists. */
static int
select_god_minion(int godnum)
{
	const int *minions = god_minions(godnum);
	int mtyp = NON_PM, mlev, num = 0, first, last;

	mlev = level_difficulty();

	for (first = 0; minions[first] != NON_PM; first++)
		if (!(mvitals[minions[first]].mvflags & G_GONE && !In_quest(&u.uz)) && monstr[minions[first]] > mlev/2) break;

	mlev = (mlev + u.ulevel) / 2;
	if (minions[first] == NON_PM) { /* all minions too weak, or no minions */
		if (first == 0) return NON_PM;
		else mtyp = minions[first-1];
	} else {
		for (last = first; minions[last] != NON_PM; last++)
			if (!(mvitals[minions[last]].mvflags & G_GONE && !In_quest(&u.uz))) {
				if (monstr[minions[last]] > mlev+5) break;
				num += min(1, mons[minions[last]].geno & G_FREQ);
			}

		if (!num) { /* all minions too strong, or gap between weak and strong */
			if (first == 0) return NON_PM;
			else mtyp = minions[first-1];
		} else {
/*			Assumption: minions are presented in ascending order of strength. */
			for (num = rnd(num); num > 0; first++)
				if (!(mvitals[minions[first]].mvflags & G_GONE && !In_quest(&u.uz))) {
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
	}
	return mtyp;
}

/* Resolve the effective alignment and god number for a summoner. ptr must be non-NULL. */
static void
msummon_resolve(struct monst *mon, struct permonst *ptr, aligntyp *atyp_out, int *gnum_out)
{
	aligntyp atyp = (ptr->maligntyp == A_NONE) ? A_NONE : sgn(ptr->maligntyp);
	int gnum = (ptr->maligntyp == A_NONE) ? GOD_MOLOCH : align_to_god(sgn(ptr->maligntyp));

	if (gnum == GOD_THE_COLLEGE)
		gnum = GOD_PTAH;
	else if (gnum == GOD_THE_CHOIR)
		gnum = GOD_THOTH;
	else if (gnum == GOD_DEFILEMENT)
		gnum = GOD_ANHUR;

	if (mon) {
		if (get_mx(mon, MX_EPRI)) {
			atyp = EPRI(mon)->shralign;
			gnum = EPRI(mon)->godnum;
		} else if (get_mx(mon, MX_EMIN)) {
			atyp = EMIN(mon)->min_align;
			gnum = EMIN(mon)->godnum;
		}
	}
	*atyp_out = atyp;
	*gnum_out = gnum;
}

/*
 * Determine what mon would summon without creating anything.
 * Returns FALSE if nothing would be summoned.
 * On TRUE: *dtype_out is a valid PM index, *cnt_out >= 1.
 */
boolean
msummon_select(struct monst *mon, struct permonst *ptr, int *dtype_out, int *cnt_out)
{
	int dtype = NON_PM, cnt = 0;
	aligntyp atyp;
	int gnum;

	if (mon && !ptr)
		ptr = mon->data;
	else if (!ptr)
		ptr = &mons[PM_WIZARD_OF_YENDOR];

	msummon_resolve(mon, ptr, &atyp, &gnum);

	if (ptr->mtyp == PM_SHAKTARI) {
		dtype = PM_MARILITH;
		cnt = d(1,6);
	} else if (ptr->mtyp == PM_BAALPHEGOR && rn2(4)) {
		dtype = rn2(4) ? PM_METAMORPHOSED_NUPPERIBO : PM_ANCIENT_NUPPERIBO;
		cnt = d(4,4);
	} else if (is_ancient(ptr) && ptr->mtyp != PM_BAALPHEGOR) {
		dtype = rn2(4) ? PM_METAMORPHOSED_NUPPERIBO : PM_ANCIENT_NUPPERIBO;
		cnt = d(1,4);
	} else if (is_dprince(ptr) || (ptr->mtyp == PM_WIZARD_OF_YENDOR)) {
		dtype = (!rn2(20)) ? dprince(ptr, atyp) :
				(!rn2(4)) ? dlord(ptr, atyp) : ndemon(atyp);
		cnt = (!rn2(4) && is_normal_demon(&mons[dtype])) ? 2 : 1;
	} else if (is_dlord(ptr)) {
		dtype = (!rn2(50)) ? dprince(ptr, atyp) :
				(!rn2(20)) ? dlord(ptr, atyp) : ndemon(atyp);
		cnt = (!rn2(4) && is_normal_demon(&mons[dtype])) ? 2 : 1;
	} else if (is_normal_demon(ptr)) {
		dtype = (!rn2(20) && Inhell && is_greater_demon(ptr)) ? dlord(ptr, atyp) :
				((mons[monsndx(ptr)].geno & G_UNIQ) || !rn2(6)) ? ndemon(atyp) : monsndx(ptr);
		cnt = 1;
	} else if (!mon) {
		/* if no mon, skip all these special cases which might query mon */;
	} else if (is_lminion(mon)) {
		if (is_keter(mon->data)) {
			if (mon->mtyp == PM_MALKUTH_SEPHIRAH && rn2(8)) return FALSE;
			dtype = PM_MALKUTH_SEPHIRAH;
			cnt = (!rn2(4) && !is_lord(&mons[dtype])) ? 2 : 1;
		} else if (In_endgame(&u.uz)) {
			dtype = (is_lord(ptr) && !rn2(20)) ? llord() :
				(is_lord(ptr) || (mons[monsndx(ptr)].geno & G_UNIQ) || !rn2(6)) ? lminion() : monsndx(ptr);
			cnt = (!rn2(4) && !is_lord(&mons[dtype])) ? 2 : 1;
		}
	} else if (is_nminion(mon) && In_endgame(&u.uz)) {
		dtype = (is_lord(ptr) && !rn2(20)) ? nlord() :
			(is_lord(ptr) || (mons[monsndx(ptr)].geno & G_UNIQ) || !rn2(6)) ? nminion() : monsndx(ptr);
		cnt = (!rn2(4) && !is_lord(&mons[dtype])) ? 2 : 1;
	} else if (is_cminion(mon) && In_endgame(&u.uz)) {
		dtype = (is_lord(ptr) && !rn2(20)) ? clord() : cminion();
		cnt = (!rn2(4) && !is_lord(&mons[dtype])) ? 2 : 1;
	} else if (ptr->mtyp == PM_ANGEL) {
		if (rn2(6)) {
			dtype = select_god_minion(gnum);
			if (dtype == NON_PM) return FALSE;
			cnt = 1;
		} else {
			dtype = PM_ANGEL;
			cnt = (!rn2(4) && !is_lord(&mons[dtype])) ? 2 : 1;
		}
	}

	if (dtype == NON_PM) return FALSE;
	*dtype_out = dtype;
	*cnt_out = cnt;
	return TRUE;
}

/* mon summons a monster
 *
 * if mon is null, treat as if as being summoned by a far-off Wizard of Yendor
 * Monster hell-p function
 */
void
msummon(struct monst *mon, struct permonst *ptr)
{
	int dtype, cnt;
	aligntyp atyp;
	int gnum;
	struct monst *mtmp;

	/* Wielded Demonbane prevents demons from gating in others. From Sporkhack*/
	if (uwep && uwep->oartifact && arti_worn_prop(uwep, ARTP_NOCALL) && mon && is_demon(mon->data)) {
		pline("%s looks puzzled for a moment.", Monnam(mon));
		return;
	}
	if (DimensionalLock)
		return;

	if (mon && !ptr)
		ptr = mon->data;
	else if (!ptr)
		ptr = &mons[PM_WIZARD_OF_YENDOR];

	msummon_resolve(mon, ptr, &atyp, &gnum);

	if (!msummon_select(mon, ptr, &dtype, &cnt)) return;

	/* sanity checks */
	if (cnt > 1 && (mons[dtype].geno & G_UNIQ)) cnt = 1;
	/*
	 * If this daemon is unique and being re-summoned (the only way we
	 * could get this far with an extinct dtype), try another.
	 */
	if (mvitals[dtype].mvflags & G_GONE) {
		dtype = ndemon(atyp);
		if (dtype == NON_PM) return;
	}

	while (cnt > 0) {
		int mmflags = ((mons[dtype].geno & G_UNIQ) ? NO_MM_FLAGS : MM_ESUM|MM_NOCOUNTBIRTH);
		mtmp = makemon(&mons[dtype], u.ux, u.uy, mmflags);
		if (mtmp) {
			if (mmflags & MM_ESUM)
				mark_mon_as_summoned(mtmp, mon, ESUMMON_PERMANENT, 0);

			if (dtype == PM_ANGEL) {
				/* alignment should match the summoner */
				add_mx(mtmp, MX_EMIN);
				EMIN(mtmp)->min_align = atyp;
				EMIN(mtmp)->godnum = gnum;
				if (mon && mon->isminion) mtmp->isminion = TRUE;
				mtmp->mpeaceful = mon && mon->mpeaceful;
			}

			/* some templates are passed from summoner to summon */
			if (mon && (
				get_template(mon) == FRACTURED ||
				get_template(mon) == VAMPIRIC ||
				get_template(mon) == PSEUDONATURAL ||
				get_template(mon) == CRANIUM_RAT ||
				get_template(mon) == MISTWEAVER ||
				get_template(mon) == YELLOW_TEMPLATE ||
				get_template(mon) == DREAM_LEECH ||
				(get_template(mon) == MAD_TEMPLATE && is_angel(mtmp->data)) ||
				(get_template(mon) == FALLEN_TEMPLATE && is_angel(mtmp->data))
			) && mtemplate_accepts_mtyp(get_template(mon), mtmp->mtyp)
			) {
				set_template(mtmp, get_template(mon));
			}
			/* as are factions */
			if (mon && mon->mfaction) {
				set_faction(mtmp, mon->mfaction);
			}
			if (!is_lord(mtmp->data) && !is_prince(mtmp->data)) {
				mtmp->mpeaceful = mon && mon->mpeaceful;
				set_malign(mtmp);
			}
		}
		cnt--;
	}
}

/* Create one standard (non-summon-tracked) monster of dtype at (x, y),
 * propagating faction from summoner if non-NULL.
 * If summoner is itself a summoned monster, the new monster is registered
 * as a summon of summoner's summoner so the grandparent-kill-dismisses chain
 * remains intact.
 * Returns the new monster, or NULL if extinct or placement fails. */
struct monst *
msummon_place(int dtype, int x, int y, struct monst *summoner)
{
	struct monst *spawned;
	struct monst *grandparent = (struct monst *)0;

	if (mvitals[dtype].mvflags & G_GONE)
		return (struct monst *)0;

	if (summoner && get_mx(summoner, MX_ESUM) && !(mons[dtype].geno & G_UNIQ))
		grandparent = summoner->mextra_p->esum_p->summoner;

	spawned = makemon(&mons[dtype], x, y, grandparent ? MM_ESUM : NO_MM_FLAGS);
	if (spawned) {
		if (summoner && summoner->mfaction)
			set_faction(spawned, summoner->mfaction);
		spawned->mgroup_summoned = TRUE;
		if (grandparent)
			mark_mon_as_summoned(spawned, grandparent, ESUMMON_PERMANENT, 0);
	}
	return spawned;
}

/* Given a summoner mon, select what it would summon and create one standard
 * (non-summon-tracked) monster of that type at (x, y).
 * Returns the new monster, or NULL on any failure including mon == NULL. */
struct monst *
msummon_create_at(struct monst *mon, int x, int y)
{
	int dtype, cnt;

	if (!mon) return (struct monst *)0;

	if (!msummon_select(mon, (struct permonst *)0, &dtype, &cnt))
		return (struct monst *)0;

	return msummon_place(dtype, x, y, mon);
}

/* Create a summoning vortex at (x,y) that will spawn dtype when it desummons. */
struct monst *
msummon_vortex(int dtype, int x, int y, struct monst *summoner, int base_duration)
{
	struct monst *vortex = makemon(&mons[PM_SUMMONING_VORTEX], x, y,
	                               MM_ESUM|MM_ADJACENTSTRICT);
	if (vortex) {
		vortex->mvar1_summon_ID = dtype;
		mark_mon_as_summoned(vortex, summoner, base_duration +
		                     ((mons[dtype].geno & G_UNIQ) ? 9 : dtype == PM_PIT_FIEND ? 3 : 1), 0);
		if (summoner && summoner->mfaction)
			vortex->mfaction = summoner->mfaction;
	}
	return vortex;
}

static void
scatter_coord(int cx, int cy, int scatter, int *ox, int *oy)
{
	int x = cx - scatter + rn2(2*scatter + 1);
	int y = cy - scatter + rn2(2*scatter + 1);
	if (!isok(x, y)) { x = cx; y = cy; }
	*ox = x;
	*oy = y;
}

static void
count_vortex(struct monst *v, int *seen, int *sensed)
{
	if (!v) return;
	if (canseemon(v)) (*seen)++;
	else if (sensemon(v)) (*sensed)++;
}

void
mcall_group(struct monst *mon, int x, int y, int scatter)
{
	int least_demons[] = {PM_MANES, PM_QUASIT};
	int lesser_demons[] = {PM_SUCCUBUS, PM_INCUBUS};
	int greater_demons[] = {PM_VROCK, PM_HEZROU, PM_NALFESHNEE};
	int true_demons[] = {PM_MARILITH, PM_ALKILITH, PM_BALROG};
	int least_devils[] = {PM_LEMURE, PM_IMP};
	int lesser_devils[] = {PM_HORNED_DEVIL, PM_ERINYS, PM_BARBED_DEVIL};
	int greater_devils[] = {PM_BONE_DEVIL, PM_ICE_DEVIL, PM_PIT_FIEND};
	int seen_cnt = 0, sensed_cnt = 0;
	mon->mgroup_summoned = TRUE;
	if(uwep && uwep->oartifact && arti_worn_prop(uwep, ARTP_NOCALL) && is_demon(mon->data))
		return;
	switch(mon->mtyp){
		//Nuncio
		//case PM_AFREET: None
		// case PM_CHAIN_DEVIL:
		// break;
		// case PM_INTERLOCUTOR_DEVIL:
		// break;
		// case PM_DAUGHTER_OF_BEDLAM:
		// break;
		// case PM_WALKING_DELIRIUM:
		// break;
		//Demons
		case PM_SUCCUBUS:
		case PM_INCUBUS:
			if(Inhell && rn2(10) < 4){
				struct monst *balrog = makemon(&mons[PM_BALROG], mon->mx, mon->my,
				                             MM_ESUM|MM_ADJACENTOK);
				if (balrog)
					mark_mon_as_summoned(balrog, mon, ESUMMON_PERMANENT, 0);
			}
		break;
		case PM_VROCK:
			if(Inhell && rn2(10) < 5){
				int i;
				if(rn2(3)) {
					for(i = rnd(6); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_VROCK, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else {
					for(i = d(2,10); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_MANES, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
			}
		break;
		case PM_HEZROU:
			if(Inhell && rn2(2)){
				if(!rn2(3)) {
					int dtype = ROLL_FROM(least_demons);
					for(int i = d(4,10); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else if(rn2(2)) {
					int dtype = ROLL_FROM(lesser_demons);
					for(int i = rnd(10); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else {
					int dtype = ROLL_FROM(greater_demons);
					for(int i = rnd(4); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
			}
		break;
		case PM_LILITU:
		break;
		case PM_NALFESHNEE:
			if(Inhell){
				int j;
				for(j = 0; j < 2; j++) {
					if(rn2(2)) {
						if(rn2(2)) {
							for(int i = rnd(6); i > 0; i--) {
								int vx, vy;
								scatter_coord(x, y, scatter, &vx, &vy);
								count_vortex(msummon_vortex(PM_VROCK, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
							}
						}
						else {
							int vx, vy;
							scatter_coord(x, y, scatter, &vx, &vy);
							count_vortex(msummon_vortex(PM_HEZROU, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
						}
					}
				}
			}
		break;
		case PM_OSSIFRUGE:
			if(Inhell && rn2(10) < 5){
				int i;
				if(rn2(3)) {
					for(i = rnd(6); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_OSSIFRUGE, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else {
					for(i = d(3,6); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_VROCK, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
			}
		break;
		case PM_INVIDIAK:
		break;
		case PM_MARILITH:
			//Note: 35% chance to reset once per 10 (global) turns
			if(Inhell){
				if(!rn2(4)) {
					int dtype = ROLL_FROM(least_demons);
					for(int i = d(2,10); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else if(!rn2(3)) {
					int dtype = ROLL_FROM(lesser_demons);
					for(int i = rnd(6); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else if(rn2(2)) {
					int dtype = ROLL_FROM(greater_demons);
					for(int i = rnd(4); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else {
					int vx, vy;
					scatter_coord(x, y, scatter, &vx, &vy);
					count_vortex(msummon_vortex(PM_MARILITH, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
				}
			}
		break;
		// case PM_ALKILITH:
		// 	//Note: also per round summons
		// break;
		case PM_BALROG:
			//Note: resets once per 10 (global) turns
			if(Inhell){
				if(!rn2(4)) {
					int dtype = ROLL_FROM(least_demons);
					for(int i = d(1,8); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else if(!rn2(3)) {
					int dtype = ROLL_FROM(lesser_demons);
					for(int i = rnd(6); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else if(rn2(2)) {
					int dtype = ROLL_FROM(greater_demons);
					for(int i = rnd(4); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else {
					int vx, vy;
					scatter_coord(x, y, scatter, &vx, &vy);
					count_vortex(msummon_vortex(rn2(2) ? PM_MARILITH : PM_BALROG, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
				}
			}
		break;
		case PM_SHAYATEEN:
			//Note: resets once per 10 (global) turns
			if(Inhell){
				if(!rn2(3)) {
					int dtype = ROLL_FROM(lesser_demons);
					for(int i = d(4,10); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else if(rn2(2)) {
					int dtype = ROLL_FROM(greater_demons);
					for(int i = rnd(6); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else {
					int dtype = rn2(2) ? PM_MARILITH : PM_BALROG;
					for(int i = 2; i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
			}
		break;
		//Devils
		case PM_HORNED_DEVIL:
			if(Inhell && rn2(2)){
				for(int i = rnd(6); i > 0; i--) {
					int vx, vy;
					scatter_coord(x, y, scatter, &vx, &vy);
					count_vortex(msummon_vortex(PM_HORNED_DEVIL, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
				}
			}
		break;
		case PM_ERINYS:
			if(Inhell && rn2(2)){
				if(rn2(2)) {
					for(int i = rnd(4); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_HORNED_DEVIL, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else {
					for(int i = rnd(8); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_IMP, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
			}
		break;
		case PM_BARBED_DEVIL:
			if(Inhell && rn2(2)){
				if(rn2(2)) {
					for(int i = d(2,6); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_HORNED_DEVIL, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else {
					for(int i = rnd(4); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_BARBED_DEVIL, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
			}
		break;
		case PM_BONE_DEVIL:
			if(Inhell && rn2(2)){
				if(rn2(2)) {
					for(int i = d(1,100); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_NUPPERIBO, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else {
					for(int i = rnd(2); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_BONE_DEVIL, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
			}
		break;
		// case PM_YOCHLOL:
		// break;
		case PM_ICE_DEVIL:
			if(Inhell && rn2(2)){
				//Note: Also 25% to summon pit fiend when "losing"
				if(!rn2(3)) {
					for(int i = d(2,6); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_HORNED_DEVIL, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else if(rn2(2)){
					for(int i = d(2,4); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_BONE_DEVIL, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
				else {
					for(int i = rnd(2); i > 0; i--) {
						int vx, vy;
						scatter_coord(x, y, scatter, &vx, &vy);
						count_vortex(msummon_vortex(PM_ICE_DEVIL, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
					}
				}
			}
		break;
		case PM_PIT_FIEND:
			if(Inhell){
				int entourage[] = {PM_ERINYS, PM_BONE_DEVIL, PM_ICE_DEVIL};
				/* star pattern: 2 knight's-move above, 2 downward-hooking left/right, 1 three below */
				int dx[] = { -1,  1, -2,  2,  0 };
				int dy[] = { -2, -2,  1,  1,  3 };
				int i;
				for (i = 0; i < 5; i++) {
					int vx = mon->mx + dx[i], vy = mon->my + dy[i];
					if (isok(vx, vy))
						count_vortex(msummon_vortex(ROLL_FROM(entourage), vx, vy, mon, 5), &seen_cnt, &sensed_cnt);
				}
			}
		break;
		// case PM_FALLEN_ANGEL:
		// break;
		// case PM_APOCALYPSE_ANGEL:
		// break;
		case PM_NESSIAN_PIT_FIEND:
			if(Inhell){
				int entourage[] = {PM_FALLEN_ANGEL, PM_PIT_FIEND, PM_ICE_DEVIL};
				/* star pattern: 2 knight's-move above, 2 downward-hooking left/right, 1 three below */
				int dx[] = { -1,  1, -2,  2,  0 };
				int dy[] = { -2, -2,  1,  1,  3 };
				int i;
				for (i = 0; i < 5; i++) {
					int vx = mon->mx + dx[i], vy = mon->my + dy[i];
					if (isok(vx, vy))
						count_vortex(msummon_vortex(ROLL_FROM(entourage), vx, vy, mon, 5), &seen_cnt, &sensed_cnt);
				}
			}
		break;
		//Ancients
		// case PM_ANCIENT_OF_BLESSINGS:
		// break;
		// case PM_ANCIENT_OF_VITALITY:
		// break;
		// case PM_ANCIENT_OF_CORRUPTION:
		// break;
		// case PM_ANCIENT_OF_THE_BURNING_WASTES:
		// break;
		// case PM_ANCIENT_OF_THOUGHT:
		// break;
		// case PM_ANCIENT_OF_ICE:
		// break;
		// case PM_ANCIENT_OF_DEATH:
		// break;
		// case PM_STRANGE_LARVA:
		// break;
		//Tanninim
		// case PM_AKKABISH_TANNIN:
		// break;
		// case PM_SHALOSH_TANNAH:
		// break;
		// case PM_NACHASH_TANNIN:
		// break;
		// case PM_KHAAMNUN_TANNIN:
		// break;
		// case PM_RAGLAYIM_TANNIN:
		// break;
		// case PM_TERAPHIM_TANNAH:
		// break;
		// case PM_SARTAN_TANNIN:
		// break;
		//Misc
		// case PM_DAMNED_PIRATE:
		// break;
	}
	{
		int total = seen_cnt + sensed_cnt;
		if (total)
			pline("You %s %s summoning %s form.",
			      seen_cnt ? "see" : "sense",
			      total == 1 ? "a" : "some",
			      total == 1 ? "vortex" : "vortices");
	}
}

void
mcall_pit_fiend(struct monst *mon, int x, int y, int scatter)
{
	int lesser_devils[] = {PM_HORNED_DEVIL, PM_ERINYS, PM_BARBED_DEVIL};
	int greater_devils[] = {PM_BONE_DEVIL, PM_ICE_DEVIL, PM_PIT_FIEND};
	int mod = mon->mtyp == PM_NESSIAN_PIT_FIEND ? 2 : 1;
	int seen_cnt = 0, sensed_cnt = 0;
	if(rn2(2)) {
		int dtype = ROLL_FROM(lesser_devils);
		for(int i = 2*mod; i > 0; i--) {
			int vx, vy;
			scatter_coord(x, y, scatter, &vx, &vy);
			count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
		}
	}
	else {
		int dtype = ROLL_FROM(greater_devils);
		for(int i = 1*mod; i > 0; i--) {
			int vx, vy;
			scatter_coord(x, y, scatter, &vx, &vy);
			count_vortex(msummon_vortex(dtype, vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
		}
	}
	{
		int total = seen_cnt + sensed_cnt;
		if (total)
			pline("You %s %s summoning %s form.",
			      seen_cnt ? "see" : "sense",
			      total == 1 ? "a" : "some",
			      total == 1 ? "vortex" : "vortices");
	}
}

void
mcall_alkilith(struct monst *mon, int x, int y, int scatter)
{
	int seen_cnt = 0, sensed_cnt = 0;
	if (!rn2(66)) {
		struct permonst *ptr = mkclass(rn2(2) ? S_DEMON : S_IMP, G_HELL);
		if (ptr) {
			int vx, vy;
			scatter_coord(x, y, scatter, &vx, &vy);
			count_vortex(msummon_vortex(monsndx(ptr), vx, vy, mon, 1), &seen_cnt, &sensed_cnt);
		}
	}
	{
		int total = seen_cnt + sensed_cnt;
		if (total)
			pline("You %s %s summoning %s form.",
			      seen_cnt ? "see" : "sense",
			      total == 1 ? "a" : "some",
			      total == 1 ? "vortex" : "vortices");
	}
}

struct monst *
summon_god_minion(int godnum, int talk)
{
	aligntyp alignment = galign(godnum);
	int mtyp = select_god_minion(godnum);
	struct monst *mon;

	if (mtyp == NON_PM) return (struct monst *)0;

	mon = makemon(&mons[mtyp], u.ux, u.uy, NO_MM_FLAGS);
	if (mon) {
		add_mx(mon, MX_EMIN);
		mon->isminion = TRUE;
		EMIN(mon)->min_align = alignment;
		EMIN(mon)->godnum = godnum;
	}

	if (mon) {
		if (talk) {
			godvoice(godnum, "Thou shalt pay for thy indiscretion!");
			if (!Blind)
				pline("%s appears before you.", An(Hallucination ? rndmonnam() : mon->data->mname));
		}
		if (mon->mpeaceful) {
			mon->mpeaceful = FALSE;
			if (godnum != u.ugodbase[UGOD_CURRENT])
				set_malign(mon);
			/* else don't call set_malign(); player was naughty */
			newsym(mon->mx, mon->my);
		}
		mon->msleeping = 0;
		mon->mcanmove = 1;

		/* fix house setting */
		if (is_drow(mon->data)) {
			int faction = god_faction(godnum);
			if (faction != -1) {
				struct obj *otmp;
				set_faction(mon, faction);
				for (otmp = mon->minvent; otmp; otmp = otmp->nobj) {
					if (otmp->otyp == find_signet_ring() || is_readable_armor_otyp(otmp->otyp))
						otmp->oward = faction;
				}
			}
		}

		if (godnum == GOD_THE_DREAD_FRACTURE && !has_template(mon, FRACTURED)) {
			set_template(mon, FRACTURED);
			mon->m_lev += 4;
			mon->mhpmax = d(mon->m_lev, 8);
			mon->mhp = mon->mhpmax;
			newsym(mon->mx, mon->my);
		}

		if (godnum == GOD_XOLOTL && !has_template(mon, SKELIFIED)) {
			set_template(mon, SKELIFIED);
			newsym(mon->mx, mon->my);
		}
	}

	return mon;
}

struct monst *
summon_minion(alignment, talk, devils, angels)
aligntyp alignment;
boolean talk;
boolean devils;
boolean angels;
{
    register struct monst *mon;
    int mtyp;

    switch ((int)alignment) {
	case A_LAWFUL:
	case A_VOID:
	    mtyp = devils ? ndemon(alignment) : lminion();
	    break;
	case A_NEUTRAL:
	    mtyp = angels ? nminion() : (PM_AIR_ELEMENTAL + rn2(8));
	    break;
	case A_CHAOTIC:
	    mtyp = angels ? cminion() : ndemon(alignment);
	    break;
	case A_NONE:
	    mtyp = angels ? PM_FALLEN_ANGEL : ndemon(alignment);
	    break;
	default:
//	    impossible("unaligned player?");
		pline("Odd alignment in minion summoning: %d",(int)alignment);
	    mtyp = ndemon(A_NONE);
	    break;
    }
    if (mtyp == NON_PM) {
		mon = 0;
    } else {
		mon = makemon(&mons[mtyp], u.ux, u.uy, MM_ESUM);
		if (mon) {
			int gnum = align_to_god(alignment);
			add_mx(mon, MX_EMIN);
			mon->isminion = TRUE;
			EMIN(mon)->min_align = alignment;
			if(gnum == GOD_THE_COLLEGE)
				gnum = GOD_PTAH;
			else if(gnum == GOD_THE_CHOIR)
				gnum = GOD_THOTH;
			else if(gnum == GOD_DEFILEMENT)
				gnum = GOD_ANHUR;
			EMIN(mon)->godnum = gnum;
			
			mark_mon_as_summoned(mon, (struct monst *)0, ESUMMON_PERMANENT, 0);
		}
	}
    if (mon) {
		if (talk) {
			pline_The("voice of %s booms:", align_gname(alignment));
			verbalize("Thou shalt pay for thy indiscretion!");
			if (!Blind)
			pline("%s appears before you.", Amonnam(mon));
		}
		mon->mpeaceful = FALSE;
		/* don't call set_malign(); player was naughty */
    }
	return mon;
}

#define Athome	(Inhell && !mtmp->cham)

int
demon_talk(mtmp)		/* returns 1 if it won't attack. */
register struct monst *mtmp;
{
	long cash, demand, offer;

	if (uwep && (
			   uwep->oartifact == ART_EXCALIBUR 
			|| uwep->oartifact == ART_LANCE_OF_LONGINUS
		) ) {
	    pline("%s looks very angry.", Amonnam(mtmp));
	    untame(mtmp, 0);
	    set_malign(mtmp);
	    newsym(mtmp->mx, mtmp->my);
	    return 0;
	}

	/* Slight advantage given. */
	if ((is_dprince(mtmp->data) || is_dlord(mtmp->data)) && mtmp->minvis) {
	    mtmp->minvis = mtmp->perminvis = 0;
	    if (!Blind) pline("%s appears before you.", Amonnam(mtmp));
	    newsym(mtmp->mx,mtmp->my);
	}
	if (youracedata->mlet == S_DEMON) {	/* Won't blackmail their own. */
	    pline("%s says, \"Good hunting, %s.\"",
		  Amonnam(mtmp), flags.female ? "Sister" : "Brother");
	    if (!tele_restrict(mtmp)) (void) rloc(mtmp, TRUE);
	    return(1);
	}
#ifndef GOLDOBJ
	cash = u.ugold;
#else
	cash = money_cnt(invent);
#endif
	demand = (cash * (rnd(80) + 20 * Athome)) /
	    (100 * (1 + (sgn(u.ualign.type) == sgn(mtmp->data->maligntyp))));

	if (!demand) {		/* you have no gold */
	    mtmp->mpeaceful = 0;
	    set_malign(mtmp);
	    newsym(mtmp->mx, mtmp->my);
	    return 0;
	} else {
		if(mtmp->mtyp == PM_ASMODEUS && demand < 9000) demand = 9000 + rn2(1000);
		else if(demand < 2000) demand = max(1000+rnd(1000), demand); //demons can't be bribed with chump change.
	    /* make sure that the demand is unmeetable if the monster
	       has the Amulet, preventing monster from being satisified
	       and removed from the game (along with said Amulet...) */
	    if (mon_has_amulet(mtmp))
		demand = cash + (long)rn1(1000,40);

	    pline("%s demands %ld %s for safe passage.",
		  Amonnam(mtmp), demand, currency(demand));

	    if ((offer = bribe(mtmp)) >= demand) {
		pline("%s vanishes, laughing about cowardly mortals.",
		      Amonnam(mtmp));
	    } else if (offer > 0L && (long)rnd(40) > (demand - offer)) {
		pline("%s scowls at you menacingly, then vanishes.",
		      Amonnam(mtmp));
	    } else {
		pline("%s gets angry...", Amonnam(mtmp));
		mtmp->mpeaceful = 0;
		set_malign(mtmp);
		newsym(mtmp->mx, mtmp->my);
		return 0;
	    }
	}
	mongone(mtmp);
	return(1);
}

long
bribe(mtmp)
struct monst *mtmp;
{
	char buf[BUFSZ];
	long offer;
#ifdef GOLDOBJ
	long umoney = money_cnt(invent);
#endif

	getlin("How much will you offer?", buf);
	if (sscanf(buf, "%ld", &offer) != 1) offer = 0L;

	/*Michael Paddon -- fix for negative offer to monster*/
	/*JAR880815 - */
	if (offer < 0L) {
		You("try to shortchange %s, but fumble.",
			mon_nam(mtmp));
		return 0L;
	} else if (offer == 0L) {
		You("refuse.");
		return 0L;
#ifndef GOLDOBJ
	} else if (offer >= u.ugold) {
		You("give %s all your gold.", mon_nam(mtmp));
		offer = u.ugold;
	} else {
		You("give %s %ld %s.", mon_nam(mtmp), offer, currency(offer));
	}
	u.ugold -= offer;
	mtmp->mgold += offer;
#else
	} else if (offer >= umoney) {
		You("give %s all your gold.", mon_nam(mtmp));
		offer = umoney;
	} else {
		You("give %s %ld %s.", mon_nam(mtmp), offer, currency(offer));
	}
	(void) money2mon(mtmp, offer);
#endif
	flags.botl = 1;
	return(offer);
}

int demonPrinces[] = {
	PM_DEMOGORGON,
	PM_LAMASHTU,
	PM_OBOX_OB,
	PM_DAGON,
	PM_PALE_NIGHT,
	PM_ORCUS,
	PM_GRAZ_ZT,
	PM_MALCANTHET
};

int
dprince(ptr, atyp)
struct permonst *ptr;
aligntyp atyp;
{
	int tryct, pm;
	boolean check_alliances = TRUE;
	
	if (!ptr) check_alliances = FALSE;
	
	if (check_alliances){
		/*Specific alliances go here*/
		if(ptr->mtyp == PM_BAPHOMET && !(mvitals[PM_PALE_NIGHT].mvflags & G_GONE))
			return PM_PALE_NIGHT;
		if(ptr->mtyp == PM_PALE_NIGHT && !(mvitals[PM_ASCODEL].mvflags & G_GONE) && !rn2(4))
			return PM_ASCODEL;
		if(ptr->mtyp == PM_PALE_NIGHT && !(mvitals[PM_GRAZ_ZT].mvflags & G_GONE))
			return PM_GRAZ_ZT;
		if(ptr->mtyp == PM_DEMOGORGON && !(mvitals[PM_DAGON].mvflags & G_GONE))
			return PM_DAGON;
	}
	if(atyp == A_NONE) atyp = !rn2(3) ? A_LAWFUL : A_CHAOTIC;
	
	if(atyp == A_LAWFUL){
		if(!(mvitals[PM_ASMODEUS].mvflags & G_GONE)) return PM_ASMODEUS;
		else return dlord(ptr, atyp);
	} else if(atyp == A_CHAOTIC) {
		for (tryct = 0; tryct < 20; tryct++) {
			pm = demonPrinces[rn2(SIZE(demonPrinces))];
			
			if (mvitals[pm].mvflags & G_GONE)
				continue;
			
			if (check_alliances && (
				(ptr->mtyp == PM_GRAZ_ZT && pm == PM_MALCANTHET) ||
				(ptr->mtyp == PM_MALCANTHET && pm == PM_GRAZ_ZT) ||
				(ptr->mtyp == PM_OBOX_OB && pm == PM_DEMOGORGON) ||
				(ptr->mtyp == PM_DEMOGORGON && pm == PM_OBOX_OB) ||
				(ptr->mtyp == PM_LAMASHTU && pm == PM_DEMOGORGON) ||
				(ptr->mtyp == PM_DEMOGORGON && pm == PM_LAMASHTU))
			)
				continue;
			
			return(pm);
		}
	}
	return(dlord(ptr, atyp));	/* approximate */
}

int demonLords[] = {
	PM_YEENOGHU,
	PM_BAPHOMET,
	PM_KOSTCHTCHIE,
	PM_ZUGGTMOY,
	PM_JUIBLEX,
	PM_ALRUNES,
	PM_ALDINACH
};

int lordsOfTheNine[] = {
	PM_MEPHISTOPHELES,
	PM_BAALZEBUB,
	PM_CRONE_LILITH,
	PM_CREATURE_IN_THE_ICE,
	PM_BELIAL,
	PM_MAMMON,
	PM_DISPATER,
	PM_BAEL
};

int
dlord(ptr, atyp)
struct permonst *ptr;
aligntyp atyp;
{
	int tryct, pm;
	boolean check_alliances = TRUE;
	
	if (!ptr) check_alliances = FALSE;
	
	if (check_alliances){
		/*Specific alliances go here*/
		if(ptr->mtyp == PM_MEPHISTOPHELES && !(mvitals[PM_BAALPHEGOR].mvflags & G_GONE))
			return PM_BAALPHEGOR;
		if(ptr->mtyp == PM_CRONE_LILITH && !(mvitals[PM_MOTHER_LILITH].mvflags & G_GONE))
			return PM_MOTHER_LILITH;
		if(ptr->mtyp == PM_CRONE_LILITH && !(mvitals[PM_DAUGHTER_LILITH].mvflags & G_GONE))
			return PM_DAUGHTER_LILITH;
		if(ptr->mtyp == PM_MOTHER_LILITH && !(mvitals[PM_DAUGHTER_LILITH].mvflags & G_GONE))
			return PM_DAUGHTER_LILITH;
		if(ptr->mtyp == PM_BELIAL && !(mvitals[PM_FIERNA].mvflags & G_GONE))
			return PM_FIERNA;
		if(ptr->mtyp == PM_MAMMON && !(mvitals[PM_GLASYA].mvflags & G_GONE) && !rn2(20))
			return PM_GLASYA;
	
		if(ptr->mtyp == PM_PALE_NIGHT && !(mvitals[PM_BAPHOMET].mvflags & G_GONE))
			return PM_BAPHOMET;
		if(ptr->mtyp == PM_OBOX_OB && !(mvitals[PM_ALDINACH].mvflags & G_GONE))
			return PM_ALDINACH;
	}
	if(atyp == A_NONE) atyp = rn2(2) ? A_LAWFUL : A_CHAOTIC;

	if(atyp == A_LAWFUL){
		for (tryct = 0; tryct < 20; tryct++) {
			pm = lordsOfTheNine[rn2(SIZE(lordsOfTheNine))];
			if(pm == PM_CRONE_LILITH) pm = !rn2(3) ? PM_CRONE_LILITH : !rn2(2) ? PM_MOTHER_LILITH : PM_DAUGHTER_LILITH;
			
			if (mvitals[pm].mvflags & G_GONE)
				continue;
			
			if (check_alliances && (
				(ptr->mtyp == PM_MEPHISTOPHELES && pm == PM_BAALZEBUB) ||
				(ptr->mtyp == PM_BAALZEBUB && pm == PM_MEPHISTOPHELES))
			) 
				continue;
			
			if(pm == PM_CREATURE_IN_THE_ICE){
					mvitals[pm].mvflags |= G_GONE;
					pm = rn2(2) ? PM_LEVISTUS : PM_LEVIATHAN;
			}
			
			return(pm);
		}
	} else if(atyp == A_CHAOTIC) {
		for (tryct = 0; tryct < 20; tryct++) {
			pm = demonLords[rn2(SIZE(demonLords))];
			
			if (mvitals[pm].mvflags & G_GONE)
				continue;
			
			if (check_alliances && (
				(ptr->mtyp == PM_BAPHOMET && pm == PM_YEENOGHU) ||
				(ptr->mtyp == PM_YEENOGHU && pm == PM_BAPHOMET) ||
				(ptr->mtyp == PM_ZUGGTMOY && pm == PM_JUIBLEX) ||
				(ptr->mtyp == PM_JUIBLEX && pm == PM_ZUGGTMOY))
			)
				continue;
			
			return(pm);
		}
	}
	
	return(ndemon(atyp));	/* approximate */
}

/* create lawful (good) lord */
int
llord()
{
	switch(rnd(2)){
	case 1:
		if (!(mvitals[PM_THRONE_ARCHON].mvflags & G_GONE && !In_quest(&u.uz)))
			return(PM_THRONE_ARCHON);
	break;
	case 2:
		if (!(mvitals[PM_LIGHT_ARCHON].mvflags & G_GONE && !In_quest(&u.uz)))
			return(PM_LIGHT_ARCHON);
	break;
	}
	
	return(lminion());	/* approximate */
}

int
lminion()
{
	int	tryct;
	struct	permonst *ptr;

	for (tryct = 0; tryct < 20; tryct++) {
	    ptr = mkclass(S_LAW_ANGEL,G_NOHELL|G_HELL|G_PLANES);
	    if (ptr && !is_lord(ptr) && is_angel(ptr))
			return(monsndx(ptr));
	}

	return NON_PM;
}

/* create neutral (good) lord */
int
nlord()
{
	switch(rnd(4)){
	case 1:
		if (!(mvitals[PM_MAHADEVA].mvflags & G_GONE && !In_quest(&u.uz)))
			return(PM_MAHADEVA);
	break;
	case 2:
	case 3:
	case 4:
		if (!(mvitals[PM_SURYA_DEVA].mvflags & G_GONE && !In_quest(&u.uz)))
			return(PM_SURYA_DEVA);
	break;
	}
	
	return(nminion());	/* approximate */
}

int
nminion()
{
	int	tryct;
	struct	permonst *ptr;

	for (tryct = 0; tryct < 20; tryct++) {
	    ptr = mkclass(S_NEU_ANGEL,G_NOHELL|G_HELL|G_PLANES);
	    if (ptr && !is_lord(ptr) && is_angel(ptr))
			return(monsndx(ptr));
	}

	return NON_PM;
}

/* create chaotic (good) lord */
int
clord()
{
	if (!(mvitals[PM_TULANI_ELADRIN].mvflags & G_GONE && !In_quest(&u.uz)))
		return(PM_TULANI_ELADRIN);

	return(cminion());	/* approximate */
}

int
cminion()
{
	int	tryct;
	struct	permonst *ptr;

	for (tryct = 0; tryct < 20; tryct++) {
	    ptr = mkclass(S_CHA_ANGEL,G_NOHELL|G_HELL|G_PLANES);
	    if (ptr && !is_lord(ptr) && is_angel(ptr))
			return(monsndx(ptr));
	}

	return NON_PM;
}

int
ndemon(atyp)
aligntyp atyp;
{
	int	tryct;
	struct	permonst *ptr;
	
	for (tryct = 0; tryct < 20; tryct++) {
	    ptr = mkclass(S_DEMON, G_NOHELL|G_HELL);
	    if (ptr && is_normal_demon(ptr) &&
		    (atyp == A_NONE || sgn(ptr->maligntyp) == sgn(atyp)))
		return(monsndx(ptr));
	}

	return NON_PM;
}

/*minion.c*/
