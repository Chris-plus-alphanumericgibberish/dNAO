/*	SCCS Id: @(#)makemon.c	3.4	2003/09/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <math.h>
#include "hack.h"

#ifdef REINCARNATION
#include <ctype.h>
#endif

struct monst zeromonst;

/* this assumes that a human quest leader or nemesis is an archetype
   of the corresponding role; that isn't so for some roles (tourist
   for instance) but is for the priests and monks we use it for... */
#define quest_mon_represents_role(mptr,role_pm) \
		(mptr->mlet == S_HUMAN && Role_if(role_pm) && \
		  (mptr->mtyp == urole.ldrnum || mptr->msound == MS_NEMESIS))

#ifdef OVL0
STATIC_DCL int FDECL(makemon_set_template, (struct permonst **, int, boolean));
STATIC_DCL int FDECL(makemon_get_permonst_faction, (struct permonst *, int));
STATIC_DCL void FDECL(makemon_set_monster_faction, (struct monst *));
STATIC_DCL boolean FDECL(uncommon, (int));
STATIC_DCL int FDECL(align_shift, (struct permonst *));
#endif /* OVL0 */
STATIC_DCL struct permonst * NDECL(roguemonst);
STATIC_DCL boolean FDECL(wrong_elem_type, (struct permonst *));
STATIC_DCL void FDECL(m_initweap,(struct monst *, int, int, boolean));
STATIC_DCL int FDECL(permonst_max_lev,(struct permonst *));
#ifdef OVL1
STATIC_DCL void FDECL(m_initinv,(struct monst *, int, int, boolean));
#endif /* OVL1 */

extern const int monstr[];
int curhouse = 0;
int zombiepm = -1;
int skeletpm = -1;

#define m_initsgrp(mtmp, x, y)	m_initgrp(mtmp, x, y, 3)
#define m_initlgrp(mtmp, x, y)	m_initgrp(mtmp, x, y, 10)

#ifdef OVLB
boolean
is_home_elemental(ptr)
register struct permonst *ptr;
{
	if (ptr->mlet == S_ELEMENTAL) switch (monsndx(ptr)) {
		case PM_AIR_ELEMENTAL: return Is_airlevel(&u.uz);
		case PM_LIGHTNING_PARAELEMENTAL: return Is_airlevel(&u.uz);
		case PM_FIRE_ELEMENTAL: return Is_firelevel(&u.uz);
		case PM_POISON_PARAELEMENTAL: return Is_firelevel(&u.uz);
		case PM_EARTH_ELEMENTAL: return Is_earthlevel(&u.uz);
		case PM_ACID_PARAELEMENTAL: return Is_earthlevel(&u.uz);
		case PM_WATER_ELEMENTAL: return Is_waterlevel(&u.uz);
		case PM_ICE_PARAELEMENTAL: return Is_waterlevel(&u.uz);
	}
	else if((ptr->mlet == S_CHA_ANGEL || ptr->mlet == S_NEU_ANGEL || ptr->mlet == S_LAW_ANGEL ) && is_minion(ptr))
		return Is_astralevel(&u.uz);
	return FALSE;
}

/*
 * Return true if the given monster cannot exist on this elemental level.
 */
STATIC_OVL boolean
wrong_elem_type(ptr)
    register struct permonst *ptr;
{
    if (ptr->mlet == S_ELEMENTAL) {
	return((boolean)(!is_home_elemental(ptr)));
    } else if (Is_earthlevel(&u.uz)) {
	/* no restrictions? */
    } else if (Is_waterlevel(&u.uz)) {
	/* just monsters that can swim */
	if(!species_swims(ptr)) return TRUE;
    } else if (Is_firelevel(&u.uz)) {
	if (!pm_resistance(ptr,MR_FIRE)) return TRUE;
    } else if (Is_airlevel(&u.uz)) {
	if(!(species_flies(ptr) && ptr->mlet != S_TRAPPER) && !species_floats(ptr)
	   && !amorphous(ptr) && !noncorporeal(ptr) && !is_whirly(ptr))
	    return TRUE;
    }
    return FALSE;
}

void
m_initgrp(mtmp, x, y, n)	/* make a group just like mtmp */
register struct monst *mtmp;
register int x, y, n;
{
	coord mm;
	int mndx;
	register int cnt = rnd(n);
	struct monst *mon;
#if defined(__GNUC__) && (defined(HPUX) || defined(DGUX))
	/* There is an unresolved problem with several people finding that
	 * the game hangs eating CPU; if interrupted and restored, the level
	 * will be filled with monsters.  Of those reports giving system type,
	 * there were two DG/UX and two HP-UX, all using gcc as the compiler.
	 * hcroft@hpopb1.cern.ch, using gcc 2.6.3 on HP-UX, says that the
	 * problem went away for him and another reporter-to-newsgroup
	 * after adding this debugging code.  This has almost got to be a
	 * compiler bug, but until somebody tracks it down and gets it fixed,
	 * might as well go with the "but it went away when I tried to find
	 * it" code.
	 */
	int cnttmp,cntdiv;
	
	if(!mtmp) return; //called with bum monster, return;
	
	cnttmp = cnt;
# ifdef DEBUG
	pline("init group call x=%d,y=%d,n=%d,cnt=%d.", x, y, n, cnt);
# endif
	cntdiv = ((u.ulevel < 3) ? 4 : (u.ulevel < 5) ? 2 : 1);
#endif
	/* Tuning: cut down on swarming at low character levels [mrs] */
	cnt /= (u.ulevel < 3) ? 4 : (u.ulevel < 5) ? 2 : 1;
#if defined(__GNUC__) && (defined(HPUX) || defined(DGUX))
	if (cnt != (cnttmp/cntdiv)) {
		pline("cnt=%d using %d, cnttmp=%d, cntdiv=%d", cnt,
			(u.ulevel < 3) ? 4 : (u.ulevel < 5) ? 2 : 1,
			cnttmp, cntdiv);
	}
#endif
	if(!cnt) cnt++;
#if defined(__GNUC__) && (defined(HPUX) || defined(DGUX))
	if (cnt < 0) cnt = 1;
	if (cnt > 10) cnt = 10;
#endif

	mm.x = x;
	mm.y = y;
	mndx = monsndx(mtmp->data);
	while(cnt--) {
		if (peace_minded(mtmp->data) && 
			!(is_auton(mtmp->data) && u.uevent.uaxus_foe)) continue;
		/* Don't create groups of peaceful monsters since they'll get
		 * in our way.  If the monster has a percentage chance so some
		 * are peaceful and some are not, the result will just be a
		 * smaller group.
		 * Exception: autons that would have been peaceful, except you killed Axus so they're going to be hostile.
		 */
		 	/* if caller wants random locations, do one here */
		if(x == 0 && y == 0) {
			int tryct = 0;	/* careful with bigrooms */
			struct monst fakemon = {0};
			set_mon_data_core(&fakemon, mtmp->data); /* set up for goodpos */
			do {
				mm.x = rn1(COLNO-3,2);
				mm.y = rn2(ROWNO);
			} while(!goodpos(mm.x, mm.y, &fakemon, NO_MM_FLAGS) ||
				(tryct++ < 200 && ((tryct < 100 && couldsee(mm.x, mm.y)) || (tryct < 150 && cansee(mm.x, mm.y)) || distmin(mm.x,mm.y,u.ux,u.uy) < BOLT_LIM)));
			if(!goodpos(mm.x, mm.y, &fakemon, NO_MM_FLAGS))
				return;
		}
		if (enexto(&mm, mm.x, mm.y, mtmp->data)) {
			/* exception to making monsters like the original: Yith are infiltrators, so only the original is Yith. */
			if (mtmp->mtemplate != YITH)
		    	mon = makemon_full(mtmp->data, mm.x, mm.y, MM_NOGROUP, mtmp->mtemplate, mtmp->mfaction);
			else
				mon = makemon_full(mtmp->data, mm.x, mm.y, MM_NOGROUP, 0, -1);

		    if (mon) {
			    mon->mpeaceful = FALSE;
			    mon->mavenge = 0;
			    set_malign(mon);

				/* if auton was going to be peaceful but you killed Axus, make it hostile and worth negative alignment */
				if (is_auton(mon->data) && peace_minded(mtmp->data) && u.uevent.uaxus_foe) {
					mon->mpeaceful = 1;
					mon->mavenge = 0;
					set_malign(mon);
					mon->mpeaceful = 0;
				}
		    }
		    /* Undo the second peace_minded() check in makemon(); if the
		     * monster turned out to be peaceful the first time we
		     * didn't create it at all; we don't want a second check.
		     */
		}
	}
}

void
m_initthrow(mtmp,otyp,oquan,mkobjflags)
struct monst *mtmp;
int otyp,oquan;
int mkobjflags;
{
	register struct obj *otmp;

	otmp = mksobj(otyp, mkobjflags);
	otmp->quan = (long) rn1(oquan, (oquan+1)/2);
	otmp->owt = weight(otmp);
	otmp->objsize = mtmp->data->msize;
	if(otyp == ORCISH_ARROW) otmp->opoisoned = OPOISON_BASIC;
	else if(otyp == DROVEN_BOLT && is_drow(mtmp->data)) otmp->opoisoned = OPOISON_SLEEP;
	fix_object(otmp);
	(void) mpickobj(mtmp, otmp);
}

#endif /* OVLB */
#ifdef OVL2

static const int ANGELTWOHANDERCUT = 4;
static NEARDATA int angelwepsbase[] = {
	SABER, LONG_SWORD, LONG_SWORD, SABER,
	TWO_HANDED_SWORD, GLAIVE, LANCE,
	LUCERN_HAMMER,
};
static NEARDATA int angelweps[] = {
	ART_DEMONBANE, ART_SUNSWORD, ART_SWORD_OF_ERATHAOL, ART_SABER_OF_SABAOTH,
	ART_SWORD_OF_ONOEL, ART_GLAIVE_OF_SHAMSIEL, ART_LANCE_OF_URIEL,
	ART_HAMMER_OF_BARQUIEL,
};

STATIC_OVL void
m_initweap(mtmp, mkobjflags, faction, goodequip)
register struct monst *mtmp;
int mkobjflags;
int faction;
boolean goodequip;
{
	register struct permonst *ptr = mtmp->data;
	register int mm = monsndx(ptr);
	struct obj *otmp;
	int chance = 0;

#define MAYBE_MERC(otmp)	if(!rn2(100)) set_material_gm(otmp, MERCURIAL);
#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) return;
#endif
	if(mtmp->mtyp == PM_BLESSED){
		//The blessed don't start with weapons.
		return;
	}
/*
 *	first a few special cases:
 *
 *		giants get a boulder to throw sometimes.
 *		ettins get clubs
 *		kobolds get darts to throw
 *		centaurs get some sort of bow & arrows or bolts
 *		soldiers get all sorts of things.
 */
	if(mm==PM_ARGENTUM_GOLEM){
	    struct obj *otmp = mksobj(SILVER_ARROW, mkobjflags);
	    otmp->blessed = FALSE;
	    otmp->cursed = FALSE;
		otmp->quan += 18+rnd(8);
		(void) mpickobj(mtmp,otmp);
		switch(rnd(12)){
			case 1:
				mongets(mtmp,SABER, mkobjflags);
			break;
			case 2:
				mongets(mtmp,TRIDENT, mkobjflags);
			break;
			case 3:
				mongets(mtmp,AXE, mkobjflags);
			break;
			case 4:
				mongets(mtmp,STILETTO, mkobjflags);
			break;
			case 5:
				mongets(mtmp,ATHAME, mkobjflags);
			break;
			case 6:
				mongets(mtmp,SHORT_SWORD, mkobjflags);
			break;
			case 7:
				mongets(mtmp,SCIMITAR, mkobjflags);
			break;
			case 8:
				mongets(mtmp,RAPIER, mkobjflags);
			break;
			case 9:
				mongets(mtmp,LONG_SWORD, mkobjflags);
			break;
			case 10:
				mongets(mtmp,TWO_HANDED_SWORD, mkobjflags);
			break;
			case 11:
				mongets(mtmp,MACE, mkobjflags);
			break;
			case 12:
				mongets(mtmp,MORNING_STAR, mkobjflags);
			break;
		}
		return;
	}
	if(mm==PM_IRON_GOLEM){
		switch(rnd(3)){
			case 1:
				mongets(mtmp,GLAIVE, mkobjflags);
				mongets(mtmp,BATTLE_AXE, mkobjflags);
			break;
			case 2:
				mongets(mtmp,SPETUM, mkobjflags);
				mongets(mtmp,DWARVISH_MATTOCK, mkobjflags);
			break;
			case 3:
				mongets(mtmp,BEC_DE_CORBIN, mkobjflags);
				mongets(mtmp,WAR_HAMMER, mkobjflags);
			break;
		}
		return;
	}
	if(mm==PM_GREEN_STEEL_GOLEM){
		switch(rnd(3)){
			case 1:
				mongets(mtmp,HALBERD, mkobjflags);
				mongets(mtmp,WAR_HAMMER, mkobjflags);
			break;
			case 2:
				mongets(mtmp,LUCERN_HAMMER, mkobjflags);
				mongets(mtmp,BATTLE_AXE, mkobjflags);
			break;
			case 3:
				mongets(mtmp,POLEAXE, mkobjflags);
				mongets(mtmp,LONG_SWORD, mkobjflags);
			break;
		}
		return;
	}
	if(mm==PM_GROVE_GUARDIAN){
		switch(rnd(6)){
			case 1:
				otmp = mksobj(MOON_AXE, mkobjflags);
				(void) mpickobj(mtmp,otmp);
			break;
			case 2:
				otmp = mksobj(KHAKKHARA, mkobjflags);
				(void) mpickobj(mtmp,otmp);
			break;
			case 3:
				mongets(mtmp,HIGH_ELVEN_WARSWORD, mkobjflags);
			break;
			case 4:
				mongets(mtmp,RAPIER, mkobjflags);
			break;
			case 5:
				mongets(mtmp,ELVEN_BROADSWORD, mkobjflags);
			break;
			case 6:
				mongets(mtmp,AKLYS, mkobjflags);
			break;
		}
		return;
	}
	if(mm==PM_FORD_GUARDIAN){
		otmp = mksobj(ELVEN_BROADSWORD, mkobjflags|MKOBJ_ARTIF);
		set_material_gm(otmp, SILVER);
		otmp->objsize = MZ_LARGE;
		add_oprop(otmp, OPROP_WRTHW);
		otmp->wrathdata = (PM_ORC<<2)+1; //"Plus 2/4 vs orcs"
		otmp->spe = abs(otmp->spe);
		bless(otmp);
		fix_object(otmp);
		(void) mpickobj(mtmp,otmp);
		return;
	}
	if(mm==PM_GLASS_GOLEM){
		switch(rnd(12)){
			case 1:
				mongets(mtmp,SABER, mkobjflags);
			break;
			case 2:
				mongets(mtmp,SICKLE, mkobjflags);
			break;
			case 3:
				mongets(mtmp,AXE, mkobjflags);
			break;
			case 4:
				mongets(mtmp,STILETTO, mkobjflags);
			break;
			case 5:
				mongets(mtmp,CRYSTAL_SWORD, mkobjflags);
			break;
			case 6:
				mongets(mtmp,BROADSWORD, mkobjflags);
			break;
			case 7:
				mongets(mtmp,SCIMITAR, mkobjflags);
			break;
			case 8:
				mongets(mtmp,RAPIER, mkobjflags);
			break;
			case 9:
				mongets(mtmp,LONG_SWORD, mkobjflags);
			break;
			case 10:
				mongets(mtmp,TWO_HANDED_SWORD, mkobjflags);
			break;
			case 11:
				mongets(mtmp,PARTISAN, mkobjflags);
			break;
			case 12:
				mongets(mtmp,GLAIVE, mkobjflags);
			break;
		}
		return;
	}
	if(mm==PM_TREASURY_GOLEM){
		switch(rnd(3)){
			case 1:
				mongets(mtmp,GLAIVE, mkobjflags);
				mongets(mtmp,BATTLE_AXE, mkobjflags);
			break;
			case 2:
				mongets(mtmp,SPETUM, mkobjflags);
				mongets(mtmp,DWARVISH_MATTOCK, mkobjflags);
			break;
			case 3:
				mongets(mtmp,BEC_DE_CORBIN, mkobjflags);
				mongets(mtmp,WAR_HAMMER, mkobjflags);
			break;
		}
		return;
	}
	if(mm==PM_GOLD_GOLEM){
		switch(rnd(6)){
			case 1:
				mongets(mtmp,STILETTO, mkobjflags);
			break;
			case 2:
				mongets(mtmp,AXE, mkobjflags);
			break;
			case 3:
				mongets(mtmp,SICKLE, mkobjflags);
			break;
			case 4:
				mongets(mtmp,SHORT_SWORD, mkobjflags);
			break;
			case 5:
				mongets(mtmp,SABER, mkobjflags);
			break;
			case 6:
				mongets(mtmp,WAR_HAMMER, mkobjflags);
			break;
		}
		return;
	}

/*	if(mm==PM_SERVANT_OF_THE_UNKNOWN_GOD){
		mongets(mtmp, CLUB, mkobjflags);
		return;
	}
*/
	switch (ptr->mlet) {
	    case S_GIANT:
		if (rn2(2)) (void)mongets(mtmp, (mm != PM_ETTIN) ?
				    BOULDER : CLUB, mkobjflags);
		if(mm == PM_LORD_SURTUR){
			otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
			otmp = oname(otmp, artiname(ART_SOL_VALTIVA));
			otmp->oerodeproof = TRUE;
			otmp->blessed = FALSE;
			otmp->cursed = FALSE;
			(void) mpickobj(mtmp,otmp);
		}
		break;
		case S_IMP:{
		    int w1 = 0, w2 = 0;
			switch (mm){
			case PM_TENGU:
				if((Role_if(PM_SAMURAI) && In_quest(&u.uz)) || !rn2(20)){
					if(mtmp->female){
						(void)mongets(mtmp, NAGINATA, mkobjflags);
						(void)mongets(mtmp, KNIFE, mkobjflags);
						(void)mongets(mtmp, YUMI, mkobjflags);
						m_initthrow(mtmp, YA, 10, mkobjflags);
					} else {
						(void)mongets(mtmp, KATANA, mkobjflags);
						(void)mongets(mtmp, WAKIZASHI, mkobjflags);
						(void)mongets(mtmp, YUMI, mkobjflags);
						m_initthrow(mtmp, YA, 10, mkobjflags);
					}
					(void)mongets(mtmp, BANDED_MAIL, mkobjflags);
					(void)mongets(mtmp, HELMET, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
				}
			break;
			case PM_IMP:
				if(Inhell || goodequip){
					chance = rn2(4);
					if(chance == 4){
						(void)mongets(mtmp, KNIFE, mkobjflags);
						(void)mongets(mtmp, FEDORA, mkobjflags);
						(void)mongets(mtmp, JACKET, mkobjflags);
					} else if(chance > 1){
						(void)mongets(mtmp, SHORT_SWORD, mkobjflags);
						(void)mongets(mtmp, LEATHER_HELM, mkobjflags);
						(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					} else {
						(void)mongets(mtmp, RAPIER, mkobjflags);
						(void)mongets(mtmp, FEDORA, mkobjflags);
						(void)mongets(mtmp, GENTLEMAN_S_SUIT, mkobjflags);
						(void)mongets(mtmp, RUFFLED_SHIRT, mkobjflags);
					}
				}
			break;
			case PM_LEGION_DEVIL_GRUNT:
			      w1 = !rn2(4) ? PARTISAN : !rn2(3) ? HALBERD : rn2(2) ? BILL_GUISARME : BEC_DE_CORBIN;
			      w2 = rn2(2) ? DAGGER : KNIFE;
				  (void)mongets(mtmp, CROSSBOW, mkobjflags);
				  m_initthrow(mtmp, CROSSBOW_BOLT, 5, mkobjflags);
				  (void)mongets(mtmp, SCALE_MAIL, mkobjflags);
				  (void)mongets(mtmp, GLOVES, mkobjflags);
				  (void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
			break;
			case PM_LEGION_DEVIL_SOLDIER:
			      w1 = !rn2(4) ? PARTISAN : !rn2(3) ? HALBERD : rn2(2) ? BILL_GUISARME : BEC_DE_CORBIN;
			      w2 = rn2(2) ? DAGGER : KNIFE;
				  (void)mongets(mtmp, CROSSBOW, mkobjflags);
				  m_initthrow(mtmp, CROSSBOW_BOLT, 10, mkobjflags);
				  if ((otmp = mongets(mtmp, rnd(WAN_LIGHTNING - WAN_CREATE_MONSTER) + WAN_CREATE_MONSTER, mkobjflags)))
					  otmp->recharged = rnd(7);
				  (void)mongets(mtmp, CHAIN_MAIL, mkobjflags);
				  (void)mongets(mtmp, GLOVES, mkobjflags);
				  (void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
			break;
			case PM_LEGION_DEVIL_SERGEANT:
			      w1 = !rn2(4) ? PARTISAN : !rn2(3) ? HALBERD : rn2(2) ? BILL_GUISARME : BEC_DE_CORBIN;
			      w2 = rn2(2) ? DAGGER : KNIFE;
				  (void)mongets(mtmp, CROSSBOW, mkobjflags);
				  m_initthrow(mtmp, CROSSBOW_BOLT, 20, mkobjflags);
				  if ((otmp = mongets(mtmp, rnd_attack_wand(mtmp), mkobjflags)))
					  otmp->recharged = rnd(7);
				  if ((otmp = mongets(mtmp, rnd(WAN_LIGHTNING - WAN_CREATE_MONSTER) + WAN_CREATE_MONSTER, mkobjflags)))
					  otmp->recharged = rnd(7);
				  (void)mongets(mtmp, SPLINT_MAIL, mkobjflags);
				  (void)mongets(mtmp, GAUNTLETS, mkobjflags);
				  (void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
			break;
			case PM_LEGION_DEVIL_CAPTAIN:
			      w1 = !rn2(4) ? PARTISAN : !rn2(3) ? HALBERD : rn2(2) ? BILL_GUISARME : BEC_DE_CORBIN;
			      w2 = rn2(2) ? DAGGER : KNIFE;
				  (void)mongets(mtmp, BOW, mkobjflags);
				  m_initthrow(mtmp, ARROW, 30, mkobjflags);
				  if ((otmp = mongets(mtmp, rnd_attack_wand(mtmp), mkobjflags)))
					  otmp->recharged = rnd(7);
				  if ((otmp = mongets(mtmp, rnd_utility_wand(mtmp), mkobjflags)))
					  otmp->recharged = rnd(7);
				  if ((otmp = mongets(mtmp, rnd(WAN_LIGHTNING - WAN_CREATE_MONSTER) + WAN_CREATE_MONSTER, mkobjflags)))
					  otmp->recharged = rnd(7);
				  (void)mongets(mtmp, PLATE_MAIL, mkobjflags);
				  (void)mongets(mtmp, GAUNTLETS, mkobjflags);
				  (void)mongets(mtmp, ARMORED_BOOTS, mkobjflags);
				  (void)mongets(mtmp, CLOAK, mkobjflags);
			break;
			case PM_GLASYA:
				otmp = mksobj(BULLWHIP, mkobjflags|MKOBJ_NOINIT);
				otmp = oname(otmp, artiname(ART_CARESS));
				otmp->spe = 9;
				otmp->cursed = TRUE;
				(void) mpickobj(mtmp,otmp);
				otmp = mksobj(find_gcirclet(), mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 9;
				otmp->cursed = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(GENTLEWOMAN_S_DRESS, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 1;
				otmp->cursed = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(VICTORIAN_UNDERWEAR, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 1;
				otmp->cursed = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(STILETTOS, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, GOLD);
				otmp->spe = 1;
				otmp->cursed = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(LONG_GLOVES, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, GOLD);
				otmp->spe = 1;
				otmp->cursed = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				// otmp = mksobj(RAZOR_WIRE, mkobjflags|MKOBJ_NOINIT);
				// set_material_gm(otmp, GOLD);
				// otmp->cursed = TRUE;
				// fix_object(otmp);
				// (void) mpickobj(mtmp,otmp);
				// otmp = mksobj(RAZOR_WIRE, mkobjflags|MKOBJ_NOINIT);
				// set_material_gm(otmp, GOLD);
				// otmp->cursed = TRUE;
				// fix_object(otmp);
				// (void) mpickobj(mtmp,otmp);
				// otmp = mksobj(RAZOR_WIRE, mkobjflags|MKOBJ_NOINIT);
				// set_material_gm(otmp, GOLD);
				// otmp->cursed = TRUE;
				// fix_object(otmp);
				// (void) mpickobj(mtmp,otmp);
			break;
			}
		    if (w1) (void)mongets(mtmp, w1, mkobjflags);
		    if (w2) (void)mongets(mtmp, w2, mkobjflags);
		break;}
	    case S_HUMAN:
			if(mm == PM_ELDER_PRIEST){
				otmp = mksobj(QUARTERSTAFF, mkobjflags);
				bless(otmp);
				otmp->spe = 3;
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
				bless(otmp);
				otmp->spe = 3;
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(POT_FULL_HEALING, mkobjflags);
				bless(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(POT_EXTRA_HEALING, mkobjflags);
				bless(otmp);
				otmp->quan = 3;
				otmp->owt = weight(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(DRUM, mkobjflags);
				bless(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(FLUTE, mkobjflags);
				bless(otmp);
				(void) mpickobj(mtmp, otmp);
				return;//no random stuff!
			}
			else if(mm == PM_SILENT_ONE){
				otmp = mksobj(QUARTERSTAFF, mkobjflags);
				bless(otmp);
				otmp->spe = 5;
				otmp->objsize = MZ_LARGE;
				add_oprop(otmp, OPROP_PSIOW);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(ROBE, mkobjflags);
				bless(otmp);
				otmp->spe = 5;
				set_material_gm(otmp, METAL);
				(void) mpickobj(mtmp, otmp);
			}
			else if(mm == PM_GOAT_SPAWN) {
				int threshold = rnd(10)+rn2(11);
				if(mtmp->female && (In_lost_cities(&u.uz)) && u.uinsight > threshold){
					set_template(mtmp, MISTWEAVER);
					mtmp->m_insight_level = threshold;
				}
			}
			else if(mm == PM_DAEMON){
				otmp = mksobj(SABER, mkobjflags);
				otmp = oname(otmp, artiname(ART_ICONOCLAST));
				otmp->spe = 9;
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);

				otmp = mksobj(JACKET, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, LEATHER);
			    curse(otmp);
			    otmp->oerodeproof = TRUE;
			    otmp->spe = 9;
			    (void) mpickobj(mtmp, otmp);
				otmp = mksobj(GLOVES, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, LEATHER);
			    curse(otmp);
			    otmp->oerodeproof = TRUE;
			    otmp->spe = 9;
			    (void) mpickobj(mtmp, otmp);
				otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, LEATHER);
			    curse(otmp);
			    otmp->oerodeproof = TRUE;
			    otmp->spe = 9;
			    (void) mpickobj(mtmp, otmp);
				// (void)mongets(mtmp, ELVEN_MITHRIL_COAT, mkobjflags);
				// (void)mongets(mtmp, ORCISH_SHIELD, mkobjflags);
				// (void)mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
				// (void)mongets(mtmp, DROVEN_HELM, mkobjflags);
				// (void)mongets(mtmp, ELVEN_BOOTS, mkobjflags);
				// (void)mongets(mtmp, ELVEN_DAGGER, mkobjflags);
				// (void)mongets(mtmp, ORCISH_DAGGER, mkobjflags);
				// (void)mongets(mtmp, DAGGER, mkobjflags);
				// (void)mongets(mtmp, STILETTO, mkobjflags);

				otmp = mksobj(POT_FULL_HEALING, mkobjflags);
				bless(otmp);
				(void) mpickobj(mtmp, otmp);
				// otmp = mksobj(POT_EXTRA_HEALING, mkobjflags);
				// bless(otmp);
				// otmp->quan = 3;
				// (void) mpickobj(mtmp, otmp);
			}
			else if(mm == PM_BASTARD_OF_THE_BOREAL_VALLEY){
				otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, METAL);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);

				otmp = mksobj(PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, METAL);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				
				otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, METAL);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				
				otmp = mksobj(PONTIFF_S_CROWN, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);

				otmp = mksobj(SCYTHE, mkobjflags|MKOBJ_NOINIT);
				otmp = oname(otmp, artiname(ART_FRIEDE_S_SCYTHE));
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				
				otmp = mksobj(SCYTHE, mkobjflags|MKOBJ_NOINIT);
				otmp = oname(otmp, artiname(ART_PROFANED_GREATSCYTHE));
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
			} else if(ptr->mtyp == PM_CORVIAN) {
				switch(rnd(5)){
					case 1:
						(void)mongets(mtmp, KNIFE, mkobjflags);
					break;
					case 2:
						(void)mongets(mtmp, QUARTERSTAFF, mkobjflags);
					break;
					case 3:
						(void)mongets(mtmp, SCYTHE, mkobjflags);
					break;
					case 4:
						(void)mongets(mtmp, SICKLE, mkobjflags);
					break;
					case 5:
						(void)mongets(mtmp, TRIDENT, mkobjflags);
					break;
				}
			} else if(ptr->mtyp == PM_CORVIAN_KNIGHT) {
				//Note: monster inventories are last-in-first-out, so the offhand weapon needs to be first
				switch(rnd(8)){
					case 1:
						(void)mongets(mtmp, SET_OF_CROW_TALONS, mkobjflags);
						(void)mongets(mtmp, SET_OF_CROW_TALONS, mkobjflags);
					break;
					case 2:
						(void)mongets(mtmp, SET_OF_CROW_TALONS, mkobjflags);
						(void)mongets(mtmp, CROW_QUILL, mkobjflags);
					break;
					case 3:
						otmp = mksobj(MACE, mkobjflags|MKOBJ_NOINIT);
						set_material_gm(otmp, METAL);
						otmp->objsize = MZ_HUGE;
						add_oprop(otmp, OPROP_COLDW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
					case 4:
						otmp = mksobj(LONG_SWORD, mkobjflags|MKOBJ_NOINIT);
						set_material_gm(otmp, SILVER);
						add_oprop(otmp, OPROP_LESSER_COLDW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
					case 5:
						otmp = mksobj(SCIMITAR, mkobjflags|MKOBJ_NOINIT);
						set_material_gm(otmp, SILVER);
						add_oprop(otmp, OPROP_FIREW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						
						otmp = mksobj(SCIMITAR, mkobjflags|MKOBJ_NOINIT);
						set_material_gm(otmp, SILVER);
						add_oprop(otmp, OPROP_LESSER_WATRW);
						add_oprop(otmp, OPROP_LESSER_MAGCW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
					case 6:
						(void)mongets(mtmp, SET_OF_CROW_TALONS, mkobjflags);
						otmp = mksobj(RAPIER, mkobjflags|MKOBJ_NOINIT);
						set_material_gm(otmp, METAL);
						add_oprop(otmp, OPROP_COLDW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
					case 7:
						(void)mongets(mtmp, KITE_SHIELD, mkobjflags);
						otmp = mksobj(SABER, mkobjflags|MKOBJ_NOINIT);
						set_material_gm(otmp, SILVER);
						add_oprop(otmp, OPROP_COLDW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
					case 8:
						otmp = mksobj(SCYTHE, mkobjflags|MKOBJ_NOINIT);
						set_material_gm(otmp, SILVER);
						add_oprop(otmp, OPROP_LESSER_COLDW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
				}
				otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, METAL);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);

				otmp = mksobj(PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, METAL);
				otmp->bodytypeflag = MB_ANIMAL;
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				
				otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, METAL);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				
				otmp = mksobj(HELMET, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, METAL);
				otmp->bodytypeflag = MB_LONGHEAD;
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
			} else if(ptr->mtyp == PM_SHATTERED_ZIGGURAT_CULTIST) {
			    otmp = mksobj(TORCH, mkobjflags|MKOBJ_NOINIT);
				otmp->age = (long) rn1(500,1000);
			    (void) mpickobj(mtmp, otmp);
				begin_burn(otmp);
				(void)mongets(mtmp, WHITE_FACELESS_ROBE, mkobjflags);
				(void)mongets(mtmp, LOW_BOOTS, mkobjflags);
			}
			else if(ptr->mtyp == PM_SHATTERED_ZIGGURAT_KNIGHT) {
			    otmp = mksobj(SHADOWLANDER_S_TORCH, mkobjflags|MKOBJ_NOINIT);
			    (void) mpickobj(mtmp, otmp);
				otmp->age = (long) rn1(500,1000);
				begin_burn(otmp);
				(void)mongets(mtmp, HELMET, mkobjflags);
				(void)mongets(mtmp, BLACK_FACELESS_ROBE, mkobjflags);
				(void)mongets(mtmp, CHAIN_MAIL, mkobjflags);
				(void)mongets(mtmp, GLOVES, mkobjflags);
				(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
			}
			else if(ptr->mtyp == PM_SHATTERED_ZIGGURAT_WIZARD) {
			    otmp = mksobj(SHADOWLANDER_S_TORCH, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = rnd(3);
				otmp->age = (long) rn1(1000,2000);
			    (void) mpickobj(mtmp, otmp);
				begin_burn(otmp);
				(void)mongets(mtmp, LEATHER_HELM, mkobjflags);
				(void)mongets(mtmp, SMOKY_VIOLET_FACELESS_ROBE, mkobjflags);
				(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				(void)mongets(mtmp, GLOVES, mkobjflags);
				(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
			}
			else if(ptr->mtyp == PM_AZTEC_WARRIOR) {
			    otmp = mksobj(TORCH, mkobjflags|MKOBJ_NOINIT);
				otmp->age = (long) rn1(500,1000);
			    (void) mpickobj(mtmp, otmp);
				begin_burn(otmp);
				
				(void)mongets(mtmp, MACUAHUITL, mkobjflags);
				(void)mongets(mtmp, STUDDED_LEATHER_ARMOR, mkobjflags);
				(void)mongets(mtmp, ICHCAHUIPILLI, mkobjflags);
				otmp = mksobj(HELMET, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, WOOD);
				(void) mpickobj(mtmp,otmp);

			}
			else if(ptr->mtyp == PM_AZTEC_SPEARTHROWER) {
			    otmp = mksobj(TORCH, mkobjflags|MKOBJ_NOINIT);
				otmp->age = (long) rn1(500,1000);
			    (void) mpickobj(mtmp, otmp);
				begin_burn(otmp);
				
				(void)mongets(mtmp, ATLATL, mkobjflags);	
				m_initthrow(mtmp, JAVELIN, rnd(10), mkobjflags);
				
				(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				(void)mongets(mtmp, ICHCAHUIPILLI, mkobjflags);
				otmp = mksobj(HELMET, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, WOOD);
				(void) mpickobj(mtmp,otmp);
			}
			else if(ptr->mtyp == PM_AZTEC_PRIEST) {
			    otmp = mksobj(TORCH, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = rnd(3);
				otmp->age = (long) rn1(1000,2000);
			    (void) mpickobj(mtmp, otmp);
				begin_burn(otmp);
				
				(void)mongets(mtmp, TECPATL, mkobjflags);
				(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				(void)mongets(mtmp, ICHCAHUIPILLI, mkobjflags);
				otmp = mksobj(HELMET, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, WOOD);
				(void) mpickobj(mtmp,otmp);
			}
			else if (ptr->mtyp == PM_CROESUS) {
				otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				otmp = mksobj(PLATE_MAIL, mkobjflags);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				otmp = mksobj(GAUNTLETS, mkobjflags);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				otmp = mksobj(LOW_BOOTS, mkobjflags);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				otmp = mksobj(AMULET_OF_REFLECTION, mkobjflags);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
			} else if(is_mercenary(ptr)) {
				int w1 = 0, w2 = 0;
				switch (mm) {
				case PM_WATCHMAN:
				case PM_SOLDIER:
				  if (!rn2(3)) {
					  w1 = !rn2(4) ? PARTISAN : !rn2(3) ? HALBERD : rn2(2) ? BILL_GUISARME : BEC_DE_CORBIN;
					  w2 = rn2(2) ? DAGGER : KNIFE;
				  } else if(!rn2(2)){
					  w1 = CROSSBOW;
					  m_initthrow(mtmp, CROSSBOW_BOLT, 24, mkobjflags);
				  } else w1 = rn2(2) ? SPEAR : SHORT_SWORD;
				  break;
				case PM_SERGEANT:
				  w1 = rn2(2) ? FLAIL : MACE;
				  if(rn2(2)){
					  w2 = CROSSBOW;
					  m_initthrow(mtmp, CROSSBOW_BOLT, 36, mkobjflags);
				  }
				  break;
				case PM_LIEUTENANT:
				  w1 = rn2(2) ? BROADSWORD : LONG_SWORD;
				  if(rn2(2)){
					  w2 = BOW;
					  m_initthrow(mtmp, ARROW, 36, mkobjflags);
				  }
				  else{
					  w2 = SHORT_SWORD;
				  }
				  break;
#ifdef CONVICT
				case PM_PRISON_GUARD:
#endif /* CONVICT */
				case PM_CAPTAIN:
				case PM_WATCH_CAPTAIN:
				  w1 = rn2(2) ? LONG_SWORD : SABER;
				  if(rn2(2)){
					  w2 = BOW;
					  if(w1 != SABER) m_initthrow(mtmp, !rn2(4) ? GOLDEN_ARROW : ARROW, 48, mkobjflags);
					  else m_initthrow(mtmp, ARROW, 48, mkobjflags);
				  }
				  else {
					  if (!rn2(5))
						  w2 = LONG_SWORD;
					  else
						  w2 = SHORT_SWORD;
				  }
				  break;
				default:
				  if (!rn2(4)) w1 = DAGGER;
				  if (!rn2(7)) w2 = SPEAR;
				  break;
				}
				if (w1) (void)mongets(mtmp, w1, mkobjflags);
				if (!w2 && w1 != DAGGER && !rn2(4)) w2 = KNIFE;
				if (w2) (void)mongets(mtmp, w2, mkobjflags);
			} else if(is_drow(ptr)){
				mtmp->mspec_used = d(4,4);
				if(mm == PM_DROW_NOVICE){
					otmp = mksobj(PLAIN_DRESS, mkobjflags);
					otmp->spe = 0;
					// otmp->oeroded2 = 1;
					otmp->opoisoned = OPOISON_FILTH;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(CRAM_RATION, mkobjflags);
					otmp->quan = d(2,3);
					otmp->owt = weight(otmp);
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(CRAM_RATION, mkobjflags);
					otmp->quan = 1;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oeaten = 100;
					otmp->owt = weight(otmp);
					(void) mpickobj(mtmp, otmp);
				} else if(mm == PM_A_SALOM){
					otmp = mksobj(NOBLE_S_DRESS, mkobjflags);
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					(void) mongets(mtmp, DROVEN_CLOAK, mkobjflags);
					(void)mongets(mtmp, DROVEN_HELM, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
					(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
					otmp = mksobj(VIPERWHIP, mkobjflags);
					otmp->spe = 4;
					otmp->opoisoned = OPOISON_FILTH;
					otmp->opoisonchrgs = 30;
					otmp->ovar1 = 1;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(BUCKLER, mkobjflags);
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(WAN_DEATH, mkobjflags);
					otmp->spe = rnd(4);
					otmp->recharged = 4;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(WAN_DIGGING, mkobjflags);
					otmp->spe = rnd(6)+6;
					otmp->recharged = 2;
					(void) mpickobj(mtmp, otmp);
					/*amulet*/
					otmp = mksobj(AMULET_OF_LIFE_SAVING, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oerodeproof = TRUE;
					(void) mpickobj(mtmp, otmp);
				} else if(mm == PM_ECLAVDRA){
					/*The Tentacle Rod*/
					otmp = mksobj(FLAIL, mkobjflags);
					otmp = oname(otmp, artiname(ART_TENTACLE_ROD));
					otmp->oerodeproof = TRUE;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->spe = d(2,3);
					(void) mpickobj(mtmp,otmp);
					/*Plate Mail*/
					otmp = mksobj(CRYSTAL_PLATE_MAIL, mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = LOLTH_SYMBOL;
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->spe = 3;
					(void) mpickobj(mtmp, otmp);
					/*Dress*/
					otmp = mksobj(PLAIN_DRESS, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 1;
					(void) mpickobj(mtmp, otmp);
					/*Ring*/
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					/*Cloak*/
					otmp = mksobj(CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*Helm*/
					otmp = mksobj(DROVEN_HELM, mkobjflags);
					set_material_gm(otmp, SILVER);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*boots*/
					otmp = mksobj(SHOES, mkobjflags);
					set_material_gm(otmp, SILVER);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
				} else if(mm == PM_SEYLL_AUZKOVYN){
					/*Plate Mail*/
					otmp = mksobj(ELVEN_MITHRIL_COAT, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*Dress*/
					otmp = mksobj(VICTORIAN_UNDERWEAR, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*Ring*/
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					/*Cloak*/
					otmp = mksobj(CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*Helm*/
					otmp = mksobj(HIGH_ELVEN_HELM, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*boots*/
					otmp = mksobj(ELVEN_BOOTS, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
				} else if(mm == PM_DARUTH_XAXOX){
					/*Body*/
					otmp = mksobj(JACKET, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 14;
					(void) mpickobj(mtmp, otmp);
					/*Shirt*/
					otmp = mksobj(RUFFLED_SHIRT, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
					/*Ring*/
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					/*Cloak*/
					otmp = mksobj(CLOAK_OF_PROTECTION, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
					/*Helm*/
					otmp = mksobj(CORNUTHAUM, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					/*boots*/
					otmp = mksobj(LOW_BOOTS, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
				} else if(mm == PM_DROW_ALIENIST){
					/*Chain Mail*/
					otmp = mksobj(DROVEN_CHAIN_MAIL, mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*Ring*/
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					/*Cloak*/
					otmp = mksobj(CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*Helm*/
					otmp = mksobj(DROVEN_HELM, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*boots*/
					otmp = mksobj(HIGH_BOOTS, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*weapon*/
					otmp = mksobj(KHAKKHARA, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 5;
					add_oprop(otmp, OPROP_BLADED);
					set_material_gm(otmp, OBSIDIAN_MT);
					(void) mpickobj(mtmp, otmp);
				} else if(is_yochlol(mtmp->data)){
					/*weapon*/
					otmp = mksobj(RAPIER, mkobjflags);
					set_material_gm(otmp, OBSIDIAN_MT);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 2;
					(void) mpickobj(mtmp, otmp);
					/*Plate Mail*/
					otmp = mksobj(DROVEN_CHAIN_MAIL, mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = LOLTH_SYMBOL;
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->spe = 2;
					(void) mpickobj(mtmp, otmp);
					/*Dress*/
					otmp = mksobj(PLAIN_DRESS, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 2;
					(void) mpickobj(mtmp, otmp);
					/*Ring*/
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					/*Cloak*/
					otmp = mksobj(CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*Helm*/
					otmp = mksobj(DROVEN_HELM, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*boots*/
					otmp = mksobj(GAUNTLETS, mkobjflags);
					set_material_gm(otmp, SHADOWSTEEL);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					/*boots*/
					otmp = mksobj(HIGH_BOOTS, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
				} else if(mm == PM_HEDROW_MASTER_WIZARD){
					otmp = mksobj(DROVEN_CHAIN_MAIL, mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					otmp->oerodeproof = TRUE;
					otmp->spe = 3;
					(void) mpickobj(mtmp, otmp);
					(void) mongets(mtmp, DROVEN_CLOAK, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
					(void)mongets(mtmp, BUCKLER, mkobjflags);
					(void)mongets(mtmp, DROVEN_SHORT_SWORD, mkobjflags);
				} else if(mm == PM_HEDROW_WIZARD){
					otmp = mksobj(DROVEN_CHAIN_MAIL, mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					otmp->oerodeproof = TRUE;
					(void) mpickobj(mtmp, otmp);
					(void) mongets(mtmp, DROVEN_CLOAK, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
					(void)mongets(mtmp, DROVEN_SHORT_SWORD, mkobjflags);
				} else if(mm == PM_SISTER){
					otmp = mksobj(NOBLE_S_DRESS, mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					(void) mongets(mtmp, DROVEN_CLOAK, mkobjflags);
					(void) mongets(mtmp, DROVEN_HELM, mkobjflags);
					(void) mongets(mtmp, GLOVES, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					otmp = mksobj(DAGGER, mkobjflags);
					set_material_gm(otmp, SILVER);
					otmp->oerodeproof = TRUE;
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					(void)mongets(mtmp, BUCKLER, mkobjflags);
					otmp = mksobj(VIPERWHIP, mkobjflags);
					otmp->spe = 2;
					otmp->opoisoned = rn2(4) ? OPOISON_BASIC : OPOISON_PARAL;
					otmp->opoisonchrgs = 1;
					otmp->ovar1 = rnd(3);
					(void) mpickobj(mtmp, otmp);
				} else if(mm == PM_MOTHER){
					otmp = mksobj(NOBLE_S_DRESS, mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					otmp->oerodeproof = TRUE;
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(DROVEN_CLOAK, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(DROVEN_HELM, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(GLOVES, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(HIGH_BOOTS, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(DAGGER, mkobjflags);
					set_material_gm(otmp, SILVER);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->quan = 6;
					otmp->spe = 5;
					otmp->owt = weight(otmp);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(BUCKLER, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(SABER, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(VIPERWHIP, mkobjflags);
					otmp->spe = 2;
					otmp->opoisoned = rn2(4) ? OPOISON_BASIC : OPOISON_PARAL;
					otmp->opoisonchrgs = 30;
					otmp->ovar1 = 5;
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
				} else if(mm == PM_STJARNA_ALFR){
					otmp = mksobj(DROVEN_PLATE_MAIL, mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = EILISTRAEE_SYMBOL;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					(void) mongets(mtmp, ELVEN_CLOAK, mkobjflags);
					(void)mongets(mtmp, DROVEN_HELM, mkobjflags);
					(void)mongets(mtmp, ELVEN_BOOTS, mkobjflags);
					(void)mongets(mtmp, ELVEN_DAGGER, mkobjflags);
					if(!rn2(20)){
						(void)mongets(mtmp, KHAKKHARA, mkobjflags);
						(void)mongets(mtmp, ELVEN_DAGGER, mkobjflags);
						(void)mongets(mtmp, ELVEN_DAGGER, mkobjflags);
						(void)mongets(mtmp, ELVEN_DAGGER, mkobjflags);
					} else {
						(void)mongets(mtmp, ELVEN_BROADSWORD, mkobjflags);
						(void)mongets(mtmp, ELVEN_SHIELD, mkobjflags);
					}
					(void)mongets(mtmp, ELVEN_BOW, mkobjflags);
					m_initthrow(mtmp, ELVEN_ARROW, 24, mkobjflags);
					if (!rn2(50)) (void)mongets(mtmp, CRYSTAL_BALL, mkobjflags);
				} else if(mm == PM_HEDROW_BLADEMASTER){
					otmp = mksobj(DROVEN_PLATE_MAIL, mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					(void) mongets(mtmp, DROVEN_CLOAK, mkobjflags);
					(void)mongets(mtmp, DROVEN_HELM, mkobjflags);
					otmp = mongets(mtmp, GAUNTLETS, mkobjflags);
					if(otmp) set_material_gm(otmp, SHADOWSTEEL);
					otmp = mongets(mtmp, ARMORED_BOOTS, mkobjflags);
					if(otmp) set_material_gm(otmp, SHADOWSTEEL);
					(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
					otmp = mongets(mtmp, DROVEN_GREATSWORD, mkobjflags);
					if(otmp){
						otmp->spe = 4;
						otmp->opoisoned = rn2(4) ? OPOISON_SLEEP : OPOISON_PARAL;
					}
				} else if(mm == PM_HOUSELESS_DROW){
					(void) mongets(mtmp, DROVEN_CLOAK, mkobjflags);
					(void) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					(void) mongets(mtmp, LOW_BOOTS, mkobjflags);
					switch(rn2(6)){
						case 0:
							otmp = mongets(mtmp, SICKLE, mkobjflags);
							set_material_gm(otmp, OBSIDIAN_MT);
						break;
						case 1:
							otmp = mongets(mtmp, SCYTHE, mkobjflags);
							set_material_gm(otmp, OBSIDIAN_MT);
						break;
						case 2:
							(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
						break;
						case 3:
							(void)mongets(mtmp, CLUB, mkobjflags);
						break;
						case 4:
							(void)mongets(mtmp, DROVEN_SHORT_SWORD, mkobjflags);
						break;
						case 5:
							(void)mongets(mtmp, DROVEN_SPEAR, mkobjflags);
						break;
					}
				} else if(mm == PM_DROW_MATRON || mm == PM_DROW_MATRON_MOTHER || is_yochlol(mtmp->data)){
					otmp = mksobj(DROVEN_PLATE_MAIL, mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = LOLTH_SYMBOL;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					(void) mongets(mtmp, DROVEN_CLOAK, mkobjflags);
					(void)mongets(mtmp, DROVEN_HELM, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
					if(!rn2(20)){
						otmp = mksobj(VIPERWHIP, mkobjflags);
						otmp->spe = 2;
						otmp->opoisoned = rn2(4) ? OPOISON_BASIC : OPOISON_PARAL;
						otmp->opoisonchrgs = 1;
						otmp->ovar1 = 1+rnd(3);
						(void) mpickobj(mtmp, otmp);
						(void)mongets(mtmp, KHAKKHARA, mkobjflags);
						(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
						(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
						(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
					} else {
						otmp = mksobj(VIPERWHIP, mkobjflags);
						otmp->spe = 2;
						otmp->opoisoned = rn2(4) ? OPOISON_BASIC : OPOISON_PARAL;
						otmp->opoisonchrgs = 1;
						otmp->ovar1 = rnd(3);
						(void) mpickobj(mtmp, otmp);
						(void)mongets(mtmp, DROVEN_SHORT_SWORD, mkobjflags);
						otmp = mongets(mtmp, KITE_SHIELD, mkobjflags);
						if(otmp) set_material_gm(otmp, SHADOWSTEEL);
					}
					(void)mongets(mtmp, DROVEN_CROSSBOW, mkobjflags);
					m_initthrow(mtmp, DROVEN_BOLT, 24, mkobjflags);
				}
				else {
					otmp = mksobj(DROVEN_CHAIN_MAIL, mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = curhouse;
					(void) mpickobj(mtmp, otmp);
					(void) mongets(mtmp, DROVEN_CLOAK, mkobjflags);
					if (rn2(2)) (void)mongets(mtmp, DROVEN_HELM, mkobjflags);
					else if (!rn2(4)) (void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					if (rn2(2)) (void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
					if (could_twoweap(ptr)) (void)mongets(mtmp, DROVEN_SHORT_SWORD, mkobjflags);
					switch (rn2(3)) {
					case 0:
						if (!rn2(4)) (void)mongets(mtmp, BUCKLER, mkobjflags);
						if (rn2(3)) (void)mongets(mtmp, DROVEN_SHORT_SWORD, mkobjflags);
						(void)mongets(mtmp, DROVEN_CROSSBOW, mkobjflags);
						m_initthrow(mtmp, DROVEN_BOLT, 24, mkobjflags);
					break;
					case 1:
						(void)mongets(mtmp, DROVEN_SHORT_SWORD, mkobjflags);
						if (rn2(2)) (void)mongets(mtmp, KITE_SHIELD, mkobjflags);
						(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
						(void)mongets(mtmp, DROVEN_DAGGER, mkobjflags);
					break;
					case 2:
						if (!rn2(4)) {
							(void)mongets(mtmp, DROVEN_SHORT_SWORD, mkobjflags);
							(void)mongets(mtmp, BUCKLER, mkobjflags);
						} else if(!rn2(3)){
							(void)mongets(mtmp, DROVEN_SPEAR, mkobjflags);
						} else if(!rn2(2)){
							(void)mongets(mtmp, MORNING_STAR, mkobjflags);
						} else {
							(void)mongets(mtmp, DROVEN_GREATSWORD, mkobjflags);
						}
					break;
					}
					switch(rn2(5)){
						case 0:
							if(rn2(3)) (void)mongets(mtmp, CRAM_RATION, mkobjflags);
						break;
						case 1:
							if(!rn2(3)) (void)mongets(mtmp, FOOD_RATION, mkobjflags);
						break;
						case 2:
							otmp = mksobj(CORPSE, mkobjflags);
							otmp->corpsenm = PM_LICHEN;
							fix_object(otmp);
							stop_corpse_timers(otmp);
							(void) mpickobj(mtmp, otmp);
						break;
						case 3:
							otmp = mksobj(TIN, mkobjflags);
							if(rnd(20) > 4) otmp->corpsenm = PM_LICHEN;
							else if(rnd(20) > 5) otmp->corpsenm = PM_SHRIEKER;
							else otmp->corpsenm = PM_VIOLET_FUNGUS;
							otmp->quan = rnd(8);
							otmp->owt = weight(otmp);
							(void) mpickobj(mtmp, otmp);
						break;
						case 4:
							otmp = mksobj(SLIME_MOLD, mkobjflags);
							otmp->spe = fruitadd("mushroom cake");
							(void) mpickobj(mtmp, otmp);
						break;
					}
				}
			} else if (is_elf(ptr) && ptr->mtyp != PM_HALF_ELF_RANGER) {
				if(mm == PM_GALADRIEL){
					/*Dress*/
					otmp = mksobj(ELVEN_TOGA, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(VICTORIAN_UNDERWEAR, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 3;
					(void) mpickobj(mtmp, otmp);
					/*Ring*/
					otmp = mksobj(find_silver_ring(), mkobjflags);
					(void) mpickobj(mtmp, otmp);
					/*boots*/
					otmp = mksobj(ELVEN_BOOTS, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					/*phial*/
					otmp = mksobj(POT_STARLIGHT, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					(void) mpickobj(mtmp, otmp);
				} else if(mm == PM_CELEBORN){
					/*Plate Mail*/
					otmp = mksobj(HIGH_ELVEN_WARSWORD, mkobjflags);
					add_oprop(otmp, OPROP_WRTHW);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					/*Plate Mail*/
					otmp = mksobj(ELVEN_MITHRIL_COAT, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					/*Cloak*/
					otmp = mksobj(ELVEN_CLOAK, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 3;
					(void) mpickobj(mtmp, otmp);
					/*Helm*/
					otmp = mksobj(HIGH_ELVEN_HELM, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 3;
					(void) mpickobj(mtmp, otmp);
					/*boots*/
					otmp = mksobj(ELVEN_BOOTS, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					otmp->spe = 3;
					(void) mpickobj(mtmp, otmp);
				} else if(mm == PM_ALABASTER_ELF){
					if(rn2(3)){
						otmp = mksobj(ELVEN_SICKLE, mkobjflags);
						set_material_gm(otmp, METAL);
						otmp->objsize = MZ_HUGE;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					} else if(rn2(3)){
						otmp = mksobj(ELVEN_DAGGER, mkobjflags);
						set_material_gm(otmp, METAL);
						otmp->objsize = MZ_LARGE;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						
						otmp = mksobj(ELVEN_SICKLE, mkobjflags);
						set_material_gm(otmp, METAL);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					} else {
						otmp = mksobj(ELVEN_BROADSWORD, mkobjflags);
						set_material_gm(otmp, METAL);
						if(rn2(2)) otmp->objsize = MZ_LARGE;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					}
					
					(void)mongets(mtmp, ELVEN_BOW, mkobjflags);
					m_initthrow(mtmp, ELVEN_ARROW, 12, mkobjflags);
					
					(void)mongets(mtmp, SHEMAGH, mkobjflags);
					(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					(void)mongets(mtmp, GLOVES, mkobjflags);
					(void)mongets(mtmp, ELVEN_BOOTS, mkobjflags);
				} else if(mm == PM_ALABASTER_ELF_ELDER){
					(void)mongets(mtmp, QUARTERSTAFF, mkobjflags);
					
					(void)mongets(mtmp, SHEMAGH, mkobjflags);
					(void)mongets(mtmp, ELVEN_TOGA, mkobjflags);
					(void)mongets(mtmp, ELVEN_BOOTS, mkobjflags);
				} else if(mm == PM_MYRKALFR){
					mtmp->m_lev = 4;
					mtmp->mhp = mtmp->mhpmax = 6*mtmp->m_lev;
					if(mtmp->female){
						otmp = mksobj(NOBLE_S_DRESS, mkobjflags);
						set_material_gm(otmp, PLASTIC);
						otmp->obj_color = CLR_WHITE;
						otmp->ohaluengr = TRUE;
						otmp->oward = LAST_BASTION_SYMBOL;
						add_oprop(otmp, OPROP_REFL);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						(void)mongets(mtmp, BODYGLOVE, mkobjflags);
						(void)mongets(mtmp, HEELED_BOOTS, mkobjflags);
						otmp = mongets(mtmp, GLOVES, mkobjflags);
						if(otmp){
							otmp->obj_color = CLR_WHITE;
						}
					}
					else {
						(void)mongets(mtmp, CRYSTAL_HELM, mkobjflags);
						(void)mongets(mtmp, JUMPSUIT, mkobjflags);
						(void)mongets(mtmp, BODYGLOVE, mkobjflags);
						otmp = mongets(mtmp, HIGH_BOOTS, mkobjflags);
						if(otmp){
							otmp->obj_color = CLR_WHITE;
						}
						otmp = mongets(mtmp, GLOVES, mkobjflags);
						if(otmp){
							otmp->obj_color = CLR_WHITE;
						}
					}
				} else if(mm == PM_MYRKALFAR_WARRIOR){
					mtmp->m_lev += rn2(6);
					mtmp->mhp = mtmp->mhpmax = mtmp->m_lev*8 - 1;
					otmp = mksobj(SNIPER_RIFLE, mkobjflags);
					otmp->spe = 4;
					otmp->cursed = FALSE; //Either uncurses or has no effect.
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(BULLET, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = 0;
					otmp->quan += 60;
					otmp->owt = weight(otmp);
					otmp->cursed = FALSE; //Either uncurses or has no effect.
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(SILVER_BULLET, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = 3;
					otmp->quan += 30;
					otmp->owt = weight(otmp);
					otmp->cursed = FALSE; //Either uncurses or has no effect.
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(VIBROBLADE, mkobjflags);
					otmp->spe = rn1(3,3);
					otmp->ovar1 = 50 + d(5,10);
					otmp->recharged = rn1(3,3);
					otmp->cursed = FALSE; //Either uncurses or has no effect.
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = LAST_BASTION_SYMBOL;
					otmp->cursed = FALSE; //Either uncurses or has no effect.
					(void) mpickobj(mtmp, otmp);
					
					otmp = mongets(mtmp, DROVEN_CLOAK, mkobjflags);
					if(otmp){
						otmp->spe = rn1(2,2);
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
					otmp = mongets(mtmp, CRYSTAL_HELM, mkobjflags);
					if(otmp){
						otmp->spe += rn2(3);
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
					otmp = mongets(mtmp, PLASTEEL_ARMOR, mkobjflags);
					if(otmp){
						otmp->spe += rn2(3);
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
					otmp = mongets(mtmp, BODYGLOVE, mkobjflags);
					otmp = mongets(mtmp, ORIHALCYON_GAUNTLETS, mkobjflags);
					if(otmp){
						otmp->spe += rn2(3);
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
					otmp = mongets(mtmp, ELVEN_BOOTS, mkobjflags);
					if(otmp){
						otmp->spe += rn2(3);
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
				} else if(mm == PM_MYRKALFAR_MATRON){
					mtmp->m_lev += rn2(6);
					mtmp->mhp = mtmp->mhpmax = mtmp->m_lev*8 - 1;
					struct obj *gem;
					give_mintrinsic(mtmp, TELEPAT);
					otmp = mksobj(ARM_BLASTER, mkobjflags);
					otmp->spe = 4;
					otmp->ovar1 = 50 + d(5,10);
					otmp->recharged = 4;
					otmp->cursed = FALSE; //Either uncurses or has no effect.
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(LIGHTSABER, mkobjflags);
					otmp->spe = 4;
					otmp->ovar1 = !rn2(4) ? 38L : !rn2(3) ? 18L : rn2(2) ? 10L : 26L;
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					gem = otmp->cobj;
					obj_extract_self(gem);
					gem = poly_obj(gem,!rn2(4) ? MORGANITE : !rn2(3) ? RUBY : rn2(2) ? GARNET : JASPER);
					add_to_container(otmp, gem);
					otmp->cursed = FALSE; //Either uncurses or has no effect.
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(find_signet_ring(), mkobjflags);
					otmp->ohaluengr = TRUE;
					otmp->oward = LAST_BASTION_SYMBOL;
					otmp->cursed = FALSE; //Either uncurses or has no effect.
					(void) mpickobj(mtmp, otmp);
					
					otmp = mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
					if(otmp){
						otmp->spe = 4;
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
					otmp = mongets(mtmp, DROVEN_PLATE_MAIL, mkobjflags);
					if(otmp) {
						otmp->ohaluengr = TRUE;
						otmp->oward = LAST_BASTION_SYMBOL;
						set_material_gm(otmp, PLASTIC);
						otmp->obj_color = CLR_BLACK;
						otmp->cursed = FALSE; //Either uncurses or has no effect.
						otmp->spe = 8;
					}
					
					otmp = mongets(mtmp, DROVEN_HELM, mkobjflags);
					if(otmp) {
						set_material_gm(otmp, PLASTIC);
						otmp->obj_color = CLR_BLACK;
						otmp->spe = 8;
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
					
					otmp = mongets(mtmp, BODYGLOVE, mkobjflags);
					if(otmp){
						otmp->spe = 4;
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
					
					otmp = mksobj(GAUNTLETS_OF_DEXTERITY, mkobjflags);
					if(otmp){
						otmp->obj_color = CLR_BLACK;
						otmp->spe = 8;
						otmp->cursed = FALSE; //Either uncurses or has no effect.
						(void) mpickobj(mtmp, otmp);
					}

					otmp = mongets(mtmp, ELVEN_BOOTS, mkobjflags);
					if(otmp){
						otmp->obj_color = CLR_BLACK;
						otmp->spe = 8;
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
				} else if(Infuture && mm == PM_ELVENKING){ /* Give the elvenking in the quest a special setup */
					struct obj *gem;
					mtmp->m_lev += 7;
					mtmp->mhp = mtmp->mhpmax = mtmp->m_lev*8 - 1;
					
					give_mintrinsic(mtmp, TELEPAT);
					give_mintrinsic(mtmp, REGENERATION);
					give_mintrinsic(mtmp, POISON_RES);
					otmp = mksobj(LIGHTSABER, mkobjflags);
					otmp->spe = 3;
					otmp->ovar1 = !rn2(4) ? 2L : !rn2(3) ? 9L : rn2(2) ? 21L : 22L;
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					gem = otmp->cobj;
					obj_extract_self(gem);
					gem = poly_obj(gem,!rn2(3) ? EMERALD : rn2(2) ? GREEN_FLUORITE : JADE);
					add_to_container(otmp, gem);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(HAND_BLASTER, mkobjflags);
					otmp->spe = 3;
					otmp->ovar1 = 50 + d(5,10);
					otmp->recharged = rnd(4);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mongets(mtmp, ELVEN_SHIELD, mkobjflags);
					if(otmp){
						set_material_gm(otmp, MITHRIL);
						otmp->spe = 7;
						bless(otmp);
					}
					
					otmp = mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
					if(otmp){
						otmp->spe = 3;
						bless(otmp);
					}
					otmp = mongets(mtmp, HIGH_ELVEN_PLATE, mkobjflags);
					if(otmp){
						otmp->spe = 7;
						bless(otmp);
					}
					otmp = mongets(mtmp, HIGH_ELVEN_HELM, mkobjflags);
					if(otmp){
						otmp->spe = 7;
						bless(otmp);
					}
					otmp = mongets(mtmp, BODYGLOVE, mkobjflags);
					if(otmp){
						otmp->spe = 3;
						bless(otmp);
					}
					
					otmp = mksobj(GAUNTLETS_OF_DEXTERITY, mkobjflags);
					if(otmp){
						set_material_gm(otmp, MITHRIL);
						otmp->spe = 7;
						bless(otmp);
					}
					(void) mpickobj(mtmp, otmp);
					
					otmp = mongets(mtmp, ELVEN_BOOTS, mkobjflags);
					if(otmp){
						otmp->spe = 7;
						bless(otmp);
					}
				} else if(In_mordor_quest(&u.uz) 
					&& !In_mordor_forest(&u.uz)
					&& !Is_ford_level(&u.uz)
					&& !In_mordor_fields(&u.uz)
					&& in_mklev
				){
					(void)mongets(mtmp, SHACKLES, mkobjflags);
					mtmp->entangled = SHACKLES;
					return;
				} else {
					if(Infuture){
						if(in_mklev){
							mtmp->m_lev = 4;
							mtmp->mhpmax = 6*mtmp->m_lev;
							(void)mongets(mtmp, CUTTING_LASER, mkobjflags);
							(void)mongets(mtmp, CRYSTAL_HELM, mkobjflags);
							(void)mongets(mtmp, JUMPSUIT, mkobjflags);
							(void)mongets(mtmp, BODYGLOVE, mkobjflags);
							otmp = mongets(mtmp, HIGH_BOOTS, mkobjflags);
							if(otmp){
								otmp->obj_color = CLR_WHITE;
							}
							otmp = mongets(mtmp, GLOVES, mkobjflags);
							if(otmp){
								otmp->obj_color = CLR_WHITE;
							}
						}
						else if(rn2(10)){
							(void) mongets(mtmp, ELVEN_CLOAK, mkobjflags);
							(void) mongets(mtmp, ELVEN_TOGA, mkobjflags);
							(void) mongets(mtmp, ELVEN_BOOTS, mkobjflags);
						}
						else if(rn2(20)){
							(void) mongets(mtmp, T_SHIRT, mkobjflags);
							(void) mongets(mtmp, WAISTCLOTH, mkobjflags);
							otmp = mongets(mtmp, find_gcirclet(), mkobjflags);
							if(otmp){
								set_material_gm(otmp, VEGGY);
								switch(rn2(4)){
									case 0:
										otmp->obj_color = CLR_YELLOW;
									break;
									case 1:
										otmp->obj_color = CLR_RED;
									break;
									case 2:
										otmp->obj_color = CLR_BRIGHT_MAGENTA;
									break;
									case 3:
										otmp->obj_color = CLR_BLUE;
									break;
								}
								otmp->oeroded2 = 1;
							}
						}
						else {
							(void) mongets(mtmp, JUMPSUIT, mkobjflags);
							(void) mongets(mtmp, BODYGLOVE, mkobjflags);
							(void) mongets(mtmp, RAYGUN, mkobjflags);
						}
					}
					else {
						if (rn2(2))
						(void) mongets(mtmp, (goodequip || rn2(2)) ? ELVEN_MITHRIL_COAT : ELVEN_CLOAK, mkobjflags);
						if (goodequip || rn2(2)) (void)mongets(mtmp, (mm == PM_ELVENKING || mm == PM_ELF_LORD || mm == PM_ELVENQUEEN || mm == PM_ELF_LADY) ? HIGH_ELVEN_HELM : ELVEN_HELM, mkobjflags);
						if (goodequip || !rn2(4)) (void)mongets(mtmp, ELVEN_BOOTS, mkobjflags);
						if (goodequip || rn2(2)) (void)mongets(mtmp, ELVEN_DAGGER, mkobjflags);
						if ((mm == PM_ELVENKING || mm == PM_ELF_LORD || mm == PM_ELVENQUEEN || mm == PM_ELF_LADY) ? TRUE : !rn2(10))
							(void)mongets(mtmp, (rn2(2) ? FLUTE : HARP), mkobjflags);
						if (goodequip || rnd(100) < mtmp->m_lev)
							(void)mongets(mtmp, POT_STARLIGHT, mkobjflags);
							
						if(In_quest(&u.uz) && Role_if(PM_KNIGHT)){
							if (mm == PM_ELF_LORD) {
								(void)mongets(mtmp, CRYSTAL_SWORD, mkobjflags);
							}
						} 
						else switch (rn2(3)) {
						case 0:
							if (!rn2(4)) (void)mongets(mtmp, ELVEN_SHIELD, mkobjflags);
							else if (could_twoweap(ptr)) (void)mongets(mtmp, ELVEN_DAGGER, mkobjflags);
							if (goodequip || rn2(3)) (void)mongets(mtmp, ELVEN_SHORT_SWORD, mkobjflags);
							(void)mongets(mtmp, ELVEN_BOW, mkobjflags);
							m_initthrow(mtmp, ELVEN_ARROW, 12, mkobjflags);
							break;
						case 1:
							(void)mongets(mtmp, ELVEN_BROADSWORD, mkobjflags);
							if (rn2(2)) (void)mongets(mtmp, ELVEN_SHIELD, mkobjflags);
							else if (could_twoweap(ptr)) (void)mongets(mtmp, ELVEN_SHORT_SWORD, mkobjflags);
							break;
						case 2:
							if (goodequip || rn2(2)) {
							(void)mongets(mtmp, ELVEN_SPEAR, mkobjflags);
							(void)mongets(mtmp, ELVEN_SHIELD, mkobjflags);
							}
							break;
						}
						if (mm == PM_ELVENKING) {
						if (rn2(3) || (in_mklev && Is_earthlevel(&u.uz)))
							(void)mongets(mtmp, PICK_AXE, mkobjflags);
							if (In_mordor_quest(&u.uz) || !rn2(50)) (void)mongets(mtmp, CRYSTAL_BALL, mkobjflags);
						}
					}
				}
			} else if (ptr->msound == MS_PRIEST ||
				quest_mon_represents_role(ptr,PM_PRIEST)) {
				otmp = mksobj(MACE, mkobjflags|MKOBJ_NOINIT);
				if(otmp) {
				otmp->spe = rnd(3);
				if(!rn2(2)) curse(otmp);
				(void) mpickobj(mtmp, otmp);
				}
			} else if(ptr->mtyp == PM_APPRENTICE_WITCH){
				struct monst *familliar;
				familliar = makemon(&mons[PM_WITCH_S_FAMILIAR], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH);
				if(familliar){
					familliar->m_lev = mtmp->m_lev;
					familliar->mhp = mtmp->mhp;
					familliar->mhpmax = mtmp->mhpmax;
					familliar->mvar_witchID = (long)mtmp->m_id;
					familliar->mpeaceful = mtmp->mpeaceful;
				}
				otmp = mongets(mtmp, HIGH_BOOTS, mkobjflags);
				if(otmp)
					otmp->obj_color = CLR_BLACK;
				mongets(mtmp, PLAIN_DRESS, mkobjflags);
				mongets(mtmp, WITCH_HAT, mkobjflags);
				mongets(mtmp, KNIFE, mkobjflags);
			} else if(ptr->mtyp == PM_WITCH){
				struct monst *familliar;
				familliar = makemon(&mons[PM_WITCH_S_FAMILIAR], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH);
				if(familliar){
					familliar->m_lev = mtmp->m_lev;
					familliar->mhp = mtmp->mhp;
					familliar->mhpmax = mtmp->mhpmax;
					familliar->mvar_witchID = (long)mtmp->m_id;
					familliar->mpeaceful = mtmp->mpeaceful;
				}
				otmp = mongets(mtmp, HIGH_BOOTS, mkobjflags);
				if(otmp) otmp->obj_color = CLR_BLACK;
				mongets(mtmp, PLAIN_DRESS, mkobjflags);
				otmp = mongets(mtmp, CLOAK, mkobjflags);
				if(otmp) otmp->obj_color = CLR_BLACK;
				otmp = mongets(mtmp, GLOVES, mkobjflags);
				if(otmp) otmp->obj_color = CLR_BLACK;
				mongets(mtmp, WITCH_HAT, mkobjflags);
				mongets(mtmp, rn2(10) ? STILETTO : ATHAME, mkobjflags);
			} else if(ptr->mtyp == PM_COVEN_LEADER){
				struct monst *familliar;
				familliar = makemon(&mons[PM_WITCH_S_FAMILIAR], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH);
				if(familliar){
					familliar->m_lev = mtmp->m_lev;
					familliar->mhp = mtmp->mhp;
					familliar->mhpmax = mtmp->mhpmax;
					familliar->mvar_witchID = (long)mtmp->m_id;
					familliar->mpeaceful = mtmp->mpeaceful;
				}
				otmp = mongets(mtmp, HIGH_BOOTS, mkobjflags);
				if(otmp) otmp->obj_color = CLR_BLACK;
				mongets(mtmp, PLAIN_DRESS, mkobjflags);
				otmp = mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				if(otmp) otmp->obj_color = CLR_BLACK;
				otmp = mongets(mtmp, CLOAK, mkobjflags);
				if(otmp) otmp->obj_color = CLR_BLACK;
				otmp = mongets(mtmp, GLOVES, mkobjflags);
				if(otmp) otmp->obj_color = CLR_BLACK;
				mongets(mtmp, WITCH_HAT, mkobjflags);
				mongets(mtmp, QUARTERSTAFF, mkobjflags);
			} else if (mm == PM_HUMAN){
				if(Infuture){
					mtmp->m_lev = 4;
					mtmp->mhpmax = 6*mtmp->m_lev;
					(void)mongets(mtmp, CRYSTAL_HELM, mkobjflags);
					(void)mongets(mtmp, JUMPSUIT, mkobjflags);
					(void)mongets(mtmp, BODYGLOVE, mkobjflags);
					otmp = mongets(mtmp, HIGH_BOOTS, mkobjflags);
					if(otmp){
						otmp->obj_color = CLR_WHITE;
					}
					otmp = mongets(mtmp, GLOVES, mkobjflags);
					if(otmp){
						otmp->obj_color = CLR_WHITE;
					}
				}
			} else if(mm >= PM_STUDENT && mm <= PM_APPRENTICE){
				if(mm == PM_STUDENT){
					(void)mongets(mtmp, PICK_AXE, mkobjflags);
					(void)mongets(mtmp, SCR_BLANK_PAPER, mkobjflags);
					(void)mongets(mtmp, JACKET, mkobjflags);
					(void)mongets(mtmp, LOW_BOOTS, mkobjflags);
					(void)mongets(mtmp, FEDORA, mkobjflags);
				} else if (mm == PM_TROOPER){
					mtmp->m_lev += rn2(6);
					mtmp->mhp = mtmp->mhpmax = mtmp->m_lev*8 - 1;
					if(!rn2(10)){
						otmp = mksobj(ARM_BLASTER, mkobjflags);
						if(otmp){
							otmp->spe = 0;
							otmp->ovar1 = d(5,10);
							otmp->recharged = 4;
							otmp->cursed = FALSE; //Either uncurses or has no effect.
							(void) mpickobj(mtmp, otmp);
						}
						
						otmp = mksobj(DISKOS, mkobjflags);
						if(otmp){
							otmp->spe = rn1(4,4);
							bless(otmp);
							(void) mpickobj(mtmp, otmp);
						}
						
						otmp = mksobj(HAND_BLASTER, mkobjflags);
						if(otmp){
							otmp->spe = rn1(4,4);
							otmp->ovar1 = 50 + d(5,10);
							otmp->recharged = rn1(2,2);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
							(void) mpickobj(mtmp, otmp);
						}
						
						otmp = mksobj(BUCKLER, mkobjflags);
						if(otmp){
							otmp->spe = rn1(2,2);
							set_material_gm(otmp, PLASTIC);
							otmp->obj_color = CLR_GRAY;
							otmp->cursed = FALSE; //Either uncurses or has no effect.
							(void) mpickobj(mtmp, otmp);
						}
						
						otmp = mongets(mtmp, CLOAK, mkobjflags);
						if(otmp){
							otmp->obj_color = CLR_GRAY;
							otmp->spe = rn1(4,4);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
						}
						otmp = mongets(mtmp, PLASTEEL_HELM, mkobjflags);
						if(otmp){
							otmp->obj_color = CLR_GRAY;
							otmp->spe = rn1(4,4);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
						}
						otmp = mongets(mtmp, PLASTEEL_ARMOR, mkobjflags);
						if(otmp){
							otmp->obj_color = CLR_GRAY;
							otmp->spe = rn1(4,4);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
						}
						(void)mongets(mtmp, BODYGLOVE, mkobjflags);
						otmp = mongets(mtmp, PLASTEEL_GAUNTLETS, mkobjflags);
						if(otmp){
							otmp->obj_color = CLR_GRAY;
							otmp->spe = rn1(4,4);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
						}
						otmp = mongets(mtmp, PLASTEEL_BOOTS, mkobjflags);
						if(otmp){
							otmp->obj_color = CLR_GRAY;
							otmp->spe = rn1(4,4);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
						}
					}
					else {
						otmp = mksobj(ARM_BLASTER, mkobjflags);
						if(otmp){
							otmp->spe = 0;
							otmp->ovar1 = d(5,10);
							otmp->recharged = 4;
							otmp->cursed = FALSE; //Either uncurses or has no effect.
							(void) mpickobj(mtmp, otmp);
						}
						
						otmp = mksobj(rn2(4) ? VIBROBLADE : FORCE_PIKE, mkobjflags);
						if(otmp){
							otmp->spe = rn1(3,3);
							otmp->ovar1 = 50 + d(5,10);
							otmp->recharged = rn1(3,3);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
							(void) mpickobj(mtmp, otmp);
						}
						
						otmp = mksobj(HAND_BLASTER, mkobjflags);
						if(otmp){
							otmp->spe = 0;
							otmp->ovar1 = 50 + d(5,10);
							otmp->recharged = rn1(2,2);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
							(void) mpickobj(mtmp, otmp);
						}
						
						otmp = mksobj(BUCKLER, mkobjflags);
						if(otmp){
							otmp->spe = rn1(2,2);
							set_material_gm(otmp, PLASTIC);
							otmp->obj_color = CLR_WHITE;
							otmp->cursed = FALSE; //Either uncurses or has no effect.
							(void) mpickobj(mtmp, otmp);
						}
						
						otmp = mongets(mtmp, CLOAK, mkobjflags);
						if(otmp){
							otmp->spe += rn2(3);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
						}
						otmp = mongets(mtmp, PLASTEEL_HELM, mkobjflags);
						if(otmp){
							otmp->spe += rn2(3);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
						}
						otmp = mongets(mtmp, PLASTEEL_ARMOR, mkobjflags);
						if(otmp){
							otmp->spe += rn2(3);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
						}
						otmp = mongets(mtmp, BODYGLOVE, mkobjflags);
						if(otmp){
							otmp->spe += rn2(3);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
						}
						otmp = mongets(mtmp, PLASTEEL_GAUNTLETS, mkobjflags);
						if(otmp){
							otmp->spe += rn2(3);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
						}
						otmp = mongets(mtmp, PLASTEEL_BOOTS, mkobjflags);
						if(otmp){
							otmp->spe += rn2(3);
							otmp->cursed = FALSE; //Either uncurses or has no effect.
						}
					}
				} else if (mm == PM_CHIEFTAIN){
					(void)mongets(mtmp, rn2(2) ? TWO_HANDED_SWORD : BATTLE_AXE, mkobjflags);
					(void)mongets(mtmp, CHAIN_MAIL, mkobjflags);
					(void)mongets(mtmp, GLOVES, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
				} else if (mm == PM_RHYMER){
					static int trotyp[] = {
						FLUTE, TOOLED_HORN, HARP,
						BELL, BUGLE, DRUM
					};
					(void)mongets(mtmp, trotyp[rn2(SIZE(trotyp))], mkobjflags);
					(void)mongets(mtmp, !rn2(10) ? ELVEN_MITHRIL_COAT : ELVEN_TOGA, mkobjflags);
					(void)mongets(mtmp, LOW_BOOTS, mkobjflags);
				} else if (mm == PM_NEANDERTHAL){
					static long long int drgnprop[] = {
						OPROP_FIREW,
						OPROP_ACIDW,
						OPROP_ELECW,
						OPROP_COLDW
					};
					static int drgnscl1[] = {
						RED_DRAGON_SCALE_MAIL,
						GREEN_DRAGON_SCALE_MAIL,
						BLUE_DRAGON_SCALE_MAIL,
						WHITE_DRAGON_SCALE_MAIL,
						BLACK_DRAGON_SCALE_MAIL
					};
					static int drgnscl2[] = {
						RED_DRAGON_SCALE_SHIELD,
						GREEN_DRAGON_SCALE_SHIELD,
						BLUE_DRAGON_SCALE_SHIELD,
						WHITE_DRAGON_SCALE_SHIELD,
						BLACK_DRAGON_SCALE_SHIELD
					};
					otmp = mksobj(rn2(2) ? CLUB : AKLYS, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = rn2(4);
					set_material_gm(otmp, DRAGON_HIDE);
					add_oprop(otmp, drgnprop[rn2(SIZE(drgnprop))]);
					(void) mpickobj(mtmp, otmp);
					(void)mongets(mtmp, drgnscl1[rn2(SIZE(drgnscl1))], mkobjflags);
					(void)mongets(mtmp, drgnscl2[rn2(SIZE(drgnscl2))], mkobjflags);
					otmp = mksobj(LEATHER_HELM, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = rn2(4);
					set_material_gm(otmp, DRAGON_HIDE);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(CLOAK, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = rn2(4);
					set_material_gm(otmp, DRAGON_HIDE);
					(void) mpickobj(mtmp, otmp);
// ifdef CONVICT
				} else if (mm == PM_INMATE){
					(void)mongets(mtmp, rn2(2) ? HEAVY_IRON_BALL : SPOON, mkobjflags);
					(void)mongets(mtmp, STRIPED_SHIRT, mkobjflags);
// endif
				} else if (mm == PM_ATTENDANT){
					(void) mongets(mtmp, SCALPEL, mkobjflags);
					(void) mongets(mtmp, HEALER_UNIFORM, mkobjflags);
					(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
					(void) mongets(mtmp, GLOVES, mkobjflags);
					(void) mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
					(void) mongets(mtmp, POT_HEALING, mkobjflags);
					(void)mongets(mtmp, POT_HEALING, mkobjflags);
				} else if (mm == PM_PAGE){
					(void) mongets(mtmp, SHORT_SWORD, mkobjflags);
					(void) mongets(mtmp, BUCKLER, mkobjflags);
					(void) mongets(mtmp, CHAIN_MAIL, mkobjflags);
					(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
					(void) mongets(mtmp, GLOVES, mkobjflags);
				} else if (mm == PM_ABBOT){
					(void) mongets(mtmp, CLOAK, mkobjflags);
				} else if (mm == PM_LADY_CONSTANCE){
					otmp = mongets(mtmp, STILETTO, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						otmp->spe = 7;
						set_material_gm(otmp, MINERAL);
						add_oprop(otmp, OPROP_LESSER_WATRW);
						add_oprop(otmp, OPROP_PSIOW);
					}
					otmp = mongets(mtmp, GENTLEMAN_S_SUIT, mkobjflags|MKOBJ_NOINIT);
					if(otmp) otmp->spe = 3;
					otmp = mongets(mtmp, RUFFLED_SHIRT, mkobjflags|MKOBJ_NOINIT);
					if(otmp) otmp->spe = 3;
					otmp = mongets(mtmp, HIGH_BOOTS, mkobjflags|MKOBJ_NOINIT);
					if(otmp) otmp->spe = 3;
				} else if (mm == PM_PATIENT){
					otmp = mongets(mtmp, STRAITJACKET, mkobjflags);
					if(otmp) curse(otmp);
				} else if (mm == PM_CONTAMINATED_PATIENT){
					switch(rnd(4)){
						case 1:
							otmp = mongets(mtmp, STRAITJACKET, mkobjflags);
							if(otmp){
								if(otmp->cursed) uncurse(otmp);
								otmp->oeroded3 = 1;
								otmp->obj_color = CLR_YELLOW;
							}
							otmp = mongets(mtmp, SCALPEL, mkobjflags);
							if (otmp)
								otmp->opoisoned = OPOISON_FILTH;
						break;
						case 2:
							otmp = mongets(mtmp, HEALER_UNIFORM, mkobjflags);
							if(otmp){
								otmp->obj_color = CLR_YELLOW;
								otmp->opoisoned = OPOISON_FILTH;
							}
							otmp = mongets(mtmp, SCALPEL, mkobjflags);
							if (otmp)
								otmp->opoisoned = OPOISON_FILTH;
						break;
						case 3:
							otmp = mongets(mtmp, STRAITJACKET, mkobjflags);
							if(otmp){
								curse(otmp);
								otmp->obj_color = CLR_YELLOW;
							}
						break;
						case 4:
							otmp = mongets(mtmp, ROBE, mkobjflags);
							if(otmp){
								otmp->obj_color = CLR_YELLOW;
							}
							otmp = mongets(mtmp, STILETTO, mkobjflags);
							if (otmp)
								otmp->opoisoned = OPOISON_BASIC;
						break;
					}
				} else if (mm == PM_SERVANT){
					if(mtmp->female){
						(void) mongets(mtmp, PLAIN_DRESS, mkobjflags);
						(void) mongets(mtmp, STILETTOS, mkobjflags);
					} else{
						(void) mongets(mtmp, RUFFLED_SHIRT, mkobjflags);
						(void) mongets(mtmp, LOW_BOOTS, mkobjflags);
					}
				} else if (mm == PM_ACOLYTE){
					(void) mongets(mtmp, !rn2(10) ? KHAKKHARA : MACE, mkobjflags);
					(void) mongets(mtmp, LEATHER_HELM, mkobjflags);
					(void) mongets(mtmp, CLOAK, mkobjflags);
					(void) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
				} else if (mm == PM_PIRATE_BROTHER){
					(void)mongets(mtmp, SCIMITAR, mkobjflags);
					(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					(void)mongets(mtmp, FLINTLOCK, mkobjflags);
					otmp = mksobj(BULLET, mkobjflags|MKOBJ_NOINIT);
					otmp->quan += 10;
					otmp->owt = weight(otmp);
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_HUNTER){
					(void)mongets(mtmp, SHORT_SWORD, mkobjflags);
					(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					(void)mongets(mtmp, LEATHER_HELM, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					(void)mongets(mtmp, BOW, mkobjflags);
					otmp = mksobj(ARROW, mkobjflags|MKOBJ_NOINIT);
					otmp->quan += 20;
					otmp->owt = weight(otmp);
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_THUG){
					(void)mongets(mtmp, CLUB, mkobjflags);
					(void)mongets(mtmp, DAGGER, mkobjflags);
					(void)mongets(mtmp, GLOVES, mkobjflags);
					(void)mongets(mtmp, JACKET, mkobjflags);
					(void)mongets(mtmp, LOW_BOOTS, mkobjflags);
				} else if (mm == PM_NINJA){
					(void)mongets(mtmp, BROADSWORD, mkobjflags);
					chance = d(1,100);
					if(chance > 95) (void)mongets(mtmp, SHURIKEN, mkobjflags);
					else if(chance > 75) (void)mongets(mtmp, DART, mkobjflags);
					else{ 
						(void) mongets(mtmp, DAGGER, mkobjflags);
						(void) mongets(mtmp, DAGGER, mkobjflags);
					}
				} else if (mm == PM_ROSHI){
					(void)mongets(mtmp, QUARTERSTAFF, mkobjflags);
					(void)mongets(mtmp, SEDGE_HAT, mkobjflags);
					(void)mongets(mtmp, CLOAK, mkobjflags);
					(void)mongets(mtmp, LOW_BOOTS, mkobjflags);
				} else if (mm == PM_GUIDE){
					(void)mongets(mtmp, CLOAK, mkobjflags);
					(void)mongets(mtmp, LOW_BOOTS, mkobjflags);
				} else if (mm == PM_WARRIOR){
					(void)mongets(mtmp, !rn2(10) ? LONG_SWORD : ATGEIR, mkobjflags);
					m_initthrow(mtmp, JAVELIN, d(4,4), mkobjflags);
					(void)mongets(mtmp, BUCKLER, mkobjflags);
					(void)mongets(mtmp, LEATHER_HELM, mkobjflags);
					(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
				} else if (mm == PM_APPRENTICE){
					//Note: may be 0 if zombie
					int itm = rnd_attack_wand(mtmp);
					if(itm){
						otmp = mksobj(itm, mkobjflags);
						otmp->blessed = FALSE;
						otmp->cursed = FALSE;
						(void) mpickobj(mtmp, otmp);
					}
					(void)mongets(mtmp, CLOAK, mkobjflags);
					(void)mongets(mtmp, LOW_BOOTS, mkobjflags);
				}
			} else if(mm >= PM_LORD_CARNARVON && mm <= PM_NEFERET_THE_GREEN){
				if(mm == PM_LORD_CARNARVON){
					otmp = mksobj(PISTOL, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(BULLET, mkobjflags|MKOBJ_NOINIT);
					otmp->quan += 20;
					bless(otmp);
					otmp->spe = 7;
					otmp->owt = weight(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(GENTLEMAN_S_SUIT, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					otmp->owt = weight(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(FEDORA, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					otmp->owt = weight(otmp);
					(void) mpickobj(mtmp, otmp);
					
					(void)mongets(mtmp, LOW_BOOTS, mkobjflags);
				} else if (mm == PM_SARA__THE_LAST_ORACLE){
					otmp = mksobj(BLINDFOLD, mkobjflags);
					otmp->spe = 0;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(STRAITJACKET, mkobjflags);
					otmp->spe = 5;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_PELIAS){
					otmp = mksobj(TWO_HANDED_SWORD, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ROBE, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_PINDAR){
					otmp = mksobj(ELVEN_MITHRIL_COAT, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ROBE, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_SHAMAN_KARNOV){
					otmp = mksobj(CLUB, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(LEO_NEMAEUS_HIDE, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(rnd_class(GRAY_DRAGON_SCALE_MAIL, YELLOW_DRAGON_SCALE_MAIL), mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_ROBERT_THE_LIFER){
					otmp = mksobj(HEAVY_IRON_BALL, mkobjflags|MKOBJ_NOINIT);
					curse(otmp);
					otmp->spe = -5;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(STRIPED_SHIRT, mkobjflags|MKOBJ_NOINIT);
					curse(otmp);
					otmp->spe = -5;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_HIPPOCRATES){
					otmp = mksobj(SCALPEL, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(HEALER_UNIFORM, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_KING_ARTHUR){
					otmp = mksobj(LONG_SWORD, mkobjflags|MKOBJ_NOINIT);
					set_material_gm(otmp, SILVER);
					add_oprop(otmp, OPROP_AXIOW);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(KITE_SHIELD, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 3;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 3;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(find_gcirclet(), mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 3;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 3;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
					set_material_gm(otmp, IRON);
					bless(otmp);
					otmp->spe = 3;
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_GRAND_MASTER){
					otmp = mksobj(ROBE, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_OLD_FORTUNE_TELLER){
					otmp = mksobj(ATHAME, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ROBE, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_ARCH_PRIEST){
					otmp = mksobj(KHAKKHARA, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ROBE, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(HELM_OF_BRILLIANCE, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_CASSILDA_THE_IRON_MAIDEN){
					otmp = mongets(mtmp, CRYSTAL_HELM, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						curse(otmp);
						set_material_gm(otmp, IRON);
						otmp->obj_color = CLR_BLACK;
					}
					otmp = mongets(mtmp, CLOAK, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						curse(otmp);
						set_material_gm(otmp, IRON);
						otmp->obj_color = CLR_BLACK;
					}
					otmp = mongets(mtmp, STRAITJACKET, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						curse(otmp);
						set_material_gm(otmp, IRON);
						otmp->obj_color = CLR_BLACK;
					}
					otmp = mongets(mtmp, VICTORIAN_UNDERWEAR, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						curse(otmp);
						set_material_gm(otmp, IRON);
						otmp->obj_color = CLR_BLACK;
					}
					otmp = mongets(mtmp, GAUNTLETS_OF_FUMBLING, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						curse(otmp);
						set_material_gm(otmp, IRON);
						otmp->obj_color = CLR_BLACK;
					}
					otmp = mongets(mtmp, SHACKLES, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						mtmp->misc_worn_check |= W_ARMF;
						otmp->owornmask |= W_ARMF;
						curse(otmp);
						update_mon_intrinsics(mtmp, otmp, TRUE, TRUE);
						set_material_gm(otmp, IRON);
						otmp->obj_color = CLR_BLACK;
					}
					otmp = mongets(mtmp, CHAIN, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						curse(otmp);
						set_material_gm(otmp, IRON);
						otmp->obj_color = CLR_BLACK;
						add_oprop(otmp, OPROP_LIVEW);
						add_oprop(otmp, OPROP_PSECW);
					}
					otmp = mongets(mtmp, CHAIN, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						curse(otmp);
						set_material_gm(otmp, IRON);
						otmp->obj_color = CLR_BLACK;
						add_oprop(otmp, OPROP_LIVEW);
						add_oprop(otmp, OPROP_PSECW);
					}
				} else if (mm == PM_MAYOR_CUMMERBUND){
					int spe2;
					otmp = mksobj(SCIMITAR, mkobjflags|MKOBJ_NOINIT);
					curse(otmp);
					otmp->oerodeproof = TRUE;
					spe2 = d(1,3);
					otmp->spe = max(otmp->spe, spe2);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(JACKET, mkobjflags|MKOBJ_NOINIT);
					otmp->oerodeproof = TRUE;
					spe2 = d(2,3);
					otmp->spe = max(otmp->spe, spe2);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(BUCKLER, mkobjflags|MKOBJ_NOINIT);
					otmp->oerodeproof = TRUE;
					spe2 = d(1,3);
					otmp->spe = max(otmp->spe, spe2);
					(void) mpickobj(mtmp, otmp);

					(void)mongets(mtmp, CLOAK, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					(void)mongets(mtmp, GLOVES, mkobjflags);
				} else if (mm == PM_ORION){
					otmp = mksobj(BOW, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(SILVER_ARROW, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					otmp->quan = 30;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(LEATHER_ARMOR, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ELVEN_BOOTS, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_MASTER_OF_THIEVES){
					otmp = mksobj(STILETTO, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 4;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(DAGGER, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 4;
					otmp->quan = 10;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(GAUNTLETS_OF_DEXTERITY, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(LEATHER_ARMOR, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_LORD_SATO){
					otmp = mksobj(KATANA, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 4;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(WAKIZASHI, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 4;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(SPLINT_MAIL, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 4;
					otmp->oerodeproof = 1;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(WAR_HAT, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_TWOFLOWER){
					otmp = mksobj(LENSES, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(HAWAIIAN_SHIRT, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(HAWAIIAN_SHORTS, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(LOW_BOOTS, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 0;
					(void) mpickobj(mtmp, otmp);
				} else if (mm == PM_NORN){
					/* Nothing */
				} else if (mm == PM_NEFERET_THE_GREEN){
					otmp = mksobj(FLAIL, mkobjflags);
					otmp->spe = 7;
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(SHEPHERD_S_CROOK, mkobjflags);
					otmp->spe = 7;
					set_material_gm(otmp, GOLD);
					otmp->objsize = MZ_SMALL;
					fix_object(otmp);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ELVEN_BOW, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(GOLDEN_ARROW, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 7;
					otmp->quan = 20;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(WAISTCLOTH, mkobjflags|MKOBJ_NOINIT);
					set_material_gm(otmp, MITHRIL);
					bless(otmp);
					otmp->spe = 7;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(GLOVES, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(SHOES, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(PRAYER_WARDED_WRAPPING, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(find_gcirclet() == HELM_OF_OPPOSITE_ALIGNMENT ? HELM_OF_BRILLIANCE : find_gcirclet(), mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					otmp->spe = 5;
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(AMULET_OF_LIFE_SAVING, mkobjflags|MKOBJ_NOINIT);
					bless(otmp);
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
				}
// #ifdef CONVICT
			} else if (mm == PM_WARDEN_ARIANNA) {
				otmp = mksobj(CHAIN, mkobjflags|MKOBJ_NOINIT);
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				// otmp->spe = -6;
				(void) mpickobj(mtmp,otmp);
				otmp = mksobj(CHAIN, mkobjflags|MKOBJ_NOINIT);
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				// otmp->spe = -6;
				(void) mpickobj(mtmp,otmp);
				otmp = mksobj(CHAIN, mkobjflags|MKOBJ_NOINIT);
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				// otmp->spe = -6;
				(void) mpickobj(mtmp,otmp);
				(void)mongets(mtmp, HEAVY_IRON_BALL, mkobjflags);
				(void)mongets(mtmp, HEAVY_IRON_BALL, mkobjflags);
			} else if (mm == PM_MINER) {
				(void)mongets(mtmp, PICK_AXE, mkobjflags);
				otmp = mksobj(LANTERN, mkobjflags);
				(void) mpickobj(mtmp, otmp);
				begin_burn(otmp);
// #endif /* CONVICT */
			} else if (mm == PM_DOCTOR_ARCHER) {
				otmp = mksobj(SCALPEL, mkobjflags|MKOBJ_NOINIT);
				otmp->opoisoned = OPOISON_FILTH;
				set_material_gm(otmp, SILVER);
				otmp->spe = 7;
				(void) mpickobj(mtmp,otmp);
				otmp = mksobj(SCALPEL, mkobjflags|MKOBJ_NOINIT);
				otmp->opoisoned = OPOISON_FILTH;
				set_material_gm(otmp, IRON);
				otmp->spe = 5;
				(void) mpickobj(mtmp,otmp);
				otmp = mongets(mtmp, HEALER_UNIFORM, mkobjflags);
				if(otmp) otmp->obj_color = CLR_YELLOW;
				otmp = mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
				if(otmp) otmp->obj_color = CLR_YELLOW;
				otmp = mongets(mtmp, LOW_BOOTS, mkobjflags);
				if(otmp) otmp->obj_color = CLR_YELLOW;
			} else if (mm == PM_ASHIKAGA_TAKAUJI) {
					otmp = mksobj(HELMET, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = 4;
					otmp->oerodeproof = 1;
					set_material_gm(otmp, IRON);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(SPLINT_MAIL, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = 4;
					otmp->oerodeproof = 1;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = 4;
					otmp->oerodeproof = 1;
					set_material_gm(otmp, IRON);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = 4;
					otmp->oerodeproof = 1;
					set_material_gm(otmp, IRON);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
			} else {
				switch(mm){
				case PM_SIR_GARLAND:
					(void) mongets(mtmp, LONG_SWORD, mkobjflags);
					(void) mongets(mtmp, LANCE, mkobjflags);
					(void) mongets(mtmp, PLATE_MAIL, mkobjflags);
				break;
				case PM_GARLAND:
					(void) mongets(mtmp, RUNESWORD, mkobjflags);
					(void) mongets(mtmp, PLATE_MAIL, mkobjflags);
				break;
				case PM_REBEL_RINGLEADER:
					otmp = mksobj(LONG_SWORD, mkobjflags);
					otmp->cursed = 0;
					otmp->blessed = 1;
					if(otmp->spe < 3) otmp->spe = 3;
					set_material_gm(otmp, IRON);
					otmp = oname(otmp, artiname(ART_AVENGER));
					add_oprop(otmp, OPROP_HOLYW);
					add_oprop(otmp, OPROP_UNHYW);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					(void) mongets(mtmp, HELMET, mkobjflags);
					otmp = mksobj(GAUNTLETS, mkobjflags);
					set_material_gm(otmp, LEATHER);
					(void) mpickobj(mtmp, otmp);
					(void) mongets(mtmp, GRAY_DRAGON_SCALE_MAIL, mkobjflags);
					(void) mongets(mtmp, KITE_SHIELD, mkobjflags);
					(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
				break;
				case PM_ADVENTURING_WIZARD:
					(void) mongets(mtmp, QUARTERSTAFF, mkobjflags);
					(void) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					(void) mongets(mtmp, ROBE, mkobjflags);
					(void) mongets(mtmp, LOW_BOOTS, mkobjflags);
				break;
				case PM_MILITANT_CLERIC:
					(void) mongets(mtmp, MACE, mkobjflags);
					(void) mongets(mtmp, HELMET, mkobjflags);
					(void) mongets(mtmp, GAUNTLETS, mkobjflags);
					(void) mongets(mtmp, PLATE_MAIL, mkobjflags);
					(void) mongets(mtmp, KITE_SHIELD, mkobjflags);
					otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
					set_material_gm(otmp, IRON);
					(void) mpickobj(mtmp, otmp);
				break;
				case PM_HALF_ELF_RANGER:
					(void) mongets(mtmp, ELVEN_SHORT_SWORD, mkobjflags);
					(void) mongets(mtmp, ELVEN_SHORT_SWORD, mkobjflags);
					(void) mongets(mtmp, ELVEN_MITHRIL_COAT, mkobjflags);
					(void) mongets(mtmp, GLOVES, mkobjflags);
					(void) mongets(mtmp, ELVEN_HELM, mkobjflags);
					(void) mongets(mtmp, ELVEN_CLOAK, mkobjflags);
					(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
					(void) mongets(mtmp, ELVEN_BOW, mkobjflags);
					otmp = mksobj(SILVER_ARROW, mkobjflags);
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					otmp->quan += 10;
					otmp->spe = 1;
					otmp->owt = weight(otmp);
					(void) mpickobj(mtmp,otmp);
					otmp = mksobj(ARROW, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = FALSE;
					otmp->quan += 20;
					otmp->owt = weight(otmp);
					(void) mpickobj(mtmp,otmp);
				break;
				case PM_PEASANT:
					switch(rn2(6)){
						case 0:
							(void)mongets(mtmp, SICKLE, mkobjflags);
						break;
						case 1:
							(void)mongets(mtmp, SCYTHE, mkobjflags);
						break;
						case 2:
							(void)mongets(mtmp, KNIFE, mkobjflags);
						break;
						case 3:
							(void)mongets(mtmp, CLUB, mkobjflags);
						break;
						case 4:
							(void)mongets(mtmp, AXE, mkobjflags);
						break;
						case 5:
							(void)mongets(mtmp, VOULGE, mkobjflags);
						break;
					}
				break;
				case PM_NURSE:
					if(Role_if(PM_MADMAN) && In_quest(&u.uz)){
						otmp = mongets(mtmp, SCALPEL, mkobjflags);
						if(otmp) otmp->opoisoned = OPOISON_FILTH;

						otmp = mongets(mtmp, HEALER_UNIFORM, mkobjflags);
						if(otmp) otmp->obj_color = CLR_YELLOW;
					}
					else if(Infuture){
						otmp = mongets(mtmp, LIGHTSABER, mkobjflags);
						set_material_gm(otmp, METAL);
						otmp->objsize = MZ_TINY;
						bless(otmp);
						fix_object(otmp);
						otmp->spe = 3;
						otmp = mongets(mtmp, PISTOL, mkobjflags);
						set_material_gm(otmp, SILVER);
						bless(otmp);
						otmp->spe = 3;
						otmp = mongets(mtmp, BULLET, mkobjflags);
						bless(otmp);
						otmp->quan = rn1(20,20);
						set_material_gm(otmp, SILVER);
						fix_object(otmp);
						otmp->spe = 3;
						
						otmp = mongets(mtmp, SHOES, mkobjflags);
						set_material_gm(otmp, CLOTH);
						otmp->obj_color = CLR_WHITE;
						otmp->spe = 7;
						otmp = mongets(mtmp, HEALER_UNIFORM, mkobjflags);
						otmp->spe = 7;
						otmp = mongets(mtmp, GLOVES, mkobjflags);
						set_material_gm(otmp, CLOTH);
						otmp->obj_color = CLR_WHITE;
						otmp->spe = 7;
						otmp = mongets(mtmp, CLOAK_OF_PROTECTION, mkobjflags);
						set_material_gm(otmp, CLOTH);
						otmp->obj_color = CLR_WHITE;
						otmp->spe = 7;
					}
					else {
						(void) mongets(mtmp, SCALPEL, mkobjflags);
						(void) mongets(mtmp, HEALER_UNIFORM, mkobjflags);
					}
				return;
				break;
				case PM_MAID: //ninja maids
					chance = d(1,100);
					if(chance > 75) (void)mongets(mtmp, SHURIKEN, mkobjflags);
					else if(chance > 50) (void)mongets(mtmp, DART, mkobjflags);
					if(chance > 99){
						otmp = mksobj(DAGGER, mkobjflags);
						set_material_gm(otmp, SILVER);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					}
					(void) mongets(mtmp, DAGGER, mkobjflags);
					(void) mongets(mtmp, DAGGER, mkobjflags);
					(void) mongets(mtmp, DAGGER, mkobjflags);
					(void) mongets(mtmp, DAGGER, mkobjflags);
					chance = d(1,100);
					if(Role_if(PM_SAMURAI))
						if(chance > 95) (void)mongets(mtmp, NAGINATA, mkobjflags);
						else if(chance > 75) (void)mongets(mtmp, KATANA, mkobjflags);
						else if(chance > 50) (void)mongets(mtmp, BROADSWORD, mkobjflags);
						else (void)mongets(mtmp, STILETTO, mkobjflags);
					else if(chance > 70) (void)mongets(mtmp, KATANA, mkobjflags);
					else (void)mongets(mtmp, STILETTO, mkobjflags);
				break;
				}
			}
		break;

		break;
		case S_GHOST:{
			switch (mm){
			case PM_GARO:
				if(rnd(100) > 75) (void)mongets(mtmp, SHURIKEN, mkobjflags);
				(void)mongets(mtmp, STILETTO, mkobjflags);
				(void)mongets(mtmp, STILETTO, mkobjflags);
				return;//no random stuff
			break;
			case PM_GARO_MASTER:
				(void)mongets(mtmp, SHURIKEN, mkobjflags);
				otmp = mksobj(STILETTO, mkobjflags);
				curse(otmp);
				otmp->spe = 4;
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(STILETTO, mkobjflags);
				curse(otmp);
				otmp->spe = 4;
				(void) mpickobj(mtmp, otmp);
				return;//no random stuff
			break;
			case PM_BLACKBEARD_S_GHOST:{
				int spe2;
				otmp = mksobj(SCIMITAR, mkobjflags|MKOBJ_NOINIT);
				curse(otmp);
				otmp->oerodeproof = TRUE;
				otmp->oeroded = 1;
				spe2 = d(2,3);
				otmp->spe = max(otmp->spe, spe2);
				(void) mpickobj(mtmp, otmp);
			break;}
			}
		break;}
	    case S_NEU_OUTSIDER:{
			if(ptr->mtyp == PM_PLUMACH_RILMANI){
				if(!rn2(3)) otmp = mksobj(MACE, mkobjflags);
				else otmp = mksobj(rn2(3) ? AXE : rn2(3) ? SICKLE : SCYTHE, mkobjflags);
			    otmp->cursed = 0;
			    otmp->blessed = 0;
				set_material_gm(otmp, LEAD);
				fix_object(otmp);
			    (void) mpickobj(mtmp, otmp);
				if(otmp->otyp == MACE && !rn2(3)){
					otmp = mksobj(KITE_SHIELD, mkobjflags);
					otmp->cursed = 0;
					otmp->blessed = 0;
					set_material_gm(otmp, LEAD);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
				}
			}
			if(ptr->mtyp == PM_FERRUMACH_RILMANI){
				otmp = mksobj(rn2(2) ? HALBERD : BATTLE_AXE, mkobjflags);
				otmp->cursed = 0;
				otmp->blessed = 0;
				if(otmp->spe < 1) otmp->spe = 1;
				set_material_gm(otmp, IRON);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			}
			if(ptr->mtyp == PM_STANNUMACH_RILMANI){
				otmp = mksobj(KHAKKHARA, mkobjflags);
				otmp->cursed = 0;
				otmp->blessed = 0;
				if(otmp->spe < 1) otmp->spe = 1;
				set_material_gm(otmp, METAL);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			}
			if(ptr->mtyp == PM_CUPRILACH_RILMANI){
				otmp = mksobj(SHORT_SWORD, mkobjflags);
				otmp->cursed = 0;
				otmp->blessed = 0;
				if(otmp->spe < 2) otmp->spe = 2;
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				if(!rn2(3)){
					otmp = mksobj(BUCKLER, mkobjflags);
					otmp->cursed = 0;
					otmp->blessed = 0;
					if(otmp->spe < 2) otmp->spe = 2;
					set_material_gm(otmp, COPPER);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
				} else if(rn2(2)){
					otmp = mksobj(SHORT_SWORD, mkobjflags);
					otmp->cursed = 0;
					otmp->blessed = 0;
					if(otmp->spe < 2) otmp->spe = 2;
					set_material_gm(otmp, COPPER);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
				}
			}
			if(ptr->mtyp == PM_ARGENACH_RILMANI){
				otmp = mksobj(rn2(3) ? BROADSWORD : rn2(3) ? BATTLE_AXE : HALBERD, mkobjflags);
			    otmp->cursed = 0;
			    otmp->blessed = 0;
				if(otmp->spe < 3) otmp->spe = 3;
				set_material_gm(otmp, SILVER);
				fix_object(otmp);
			    (void) mpickobj(mtmp, otmp);
				if(otmp->otyp == BROADSWORD){
					otmp = mksobj(rn2(6) ? KITE_SHIELD : SHIELD_OF_REFLECTION, mkobjflags);
					otmp->cursed = 0;
					otmp->blessed = 0;
					if(otmp->spe < 3) otmp->spe = 3;
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
				}
			}
			if(ptr->mtyp == PM_HYDRARGYRUMACH_RILMANI){
				otmp = mksobj(VOULGE, mkobjflags);
			    otmp->cursed = 0;
			    otmp->blessed = 0;
				set_material_gm(otmp, METAL);
				fix_object(otmp);
			    (void) mpickobj(mtmp, otmp);
				otmp = mksobj(LONG_SWORD, mkobjflags);
			    otmp->cursed = 0;
			    otmp->blessed = 0;
				set_material_gm(otmp, METAL);
				fix_object(otmp);
			    (void) mpickobj(mtmp, otmp);
			}
			if(ptr->mtyp == PM_AURUMACH_RILMANI){
				otmp = mksobj(HALBERD, mkobjflags);
				otmp->objsize = MZ_LARGE;
			    otmp->cursed = 0;
			    otmp->blessed = 0;
				if(otmp->spe < 4) otmp->spe = 4;
				add_oprop(otmp, OPROP_CONCW);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
			    (void) mpickobj(mtmp, otmp);
				otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
				otmp->objsize = MZ_LARGE;
			    otmp->cursed = 0;
			    otmp->blessed = 0;
				if(otmp->spe < 4) otmp->spe = 4;
				add_oprop(otmp, OPROP_CONCW);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
			    (void) mpickobj(mtmp, otmp);
				if(!rn2(3)){
					otmp = mksobj(HELMET, mkobjflags);
					otmp->objsize = MZ_LARGE;
					otmp->cursed = 0;
					otmp->blessed = 0;
					add_oprop(otmp, OPROP_CONC);
					if(otmp->spe < 4) otmp->spe = 4;
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(PLATE_MAIL, mkobjflags);
					otmp->objsize = MZ_LARGE;
					otmp->cursed = 0;
					otmp->blessed = 0;
					add_oprop(otmp, OPROP_CONC);
					if(otmp->spe < 4) otmp->spe = 4;
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(GAUNTLETS, mkobjflags);
					otmp->objsize = MZ_LARGE;
					otmp->cursed = 0;
					otmp->blessed = 0;
					add_oprop(otmp, OPROP_CONC);
					if(otmp->spe < 4) otmp->spe = 4;
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ARMORED_BOOTS, mkobjflags);
					otmp->objsize = MZ_LARGE;
					otmp->cursed = 0;
					otmp->blessed = 0;
					add_oprop(otmp, OPROP_CONC);
					if(otmp->spe < 4) otmp->spe = 4;
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
				}
			}
			if(ptr->mtyp == PM_CENTER_OF_ALL){
				struct obj *otmp = mksobj(BARDICHE, mkobjflags);
				otmp->objsize = MZ_LARGE;
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				otmp->spe = 4;
				add_oprop(otmp, OPROP_CONCW);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);

				otmp = mksobj(SKELETON_KEY, mkobjflags);
				otmp = oname(otmp, artiname(ART_FIRST_KEY_OF_NEUTRALITY));
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);
				
				otmp = mksobj(WAR_HAT, mkobjflags|MKOBJ_NOINIT);
				otmp->objsize = MZ_LARGE;
				add_oprop(otmp, OPROP_CONC);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(LEATHER_ARMOR, mkobjflags|MKOBJ_NOINIT);
				otmp->objsize = MZ_LARGE;
				add_oprop(otmp, OPROP_CONC);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(ROBE, mkobjflags|MKOBJ_NOINIT);
				otmp->objsize = MZ_LARGE;
				add_oprop(otmp, OPROP_CONC);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
				otmp->objsize = MZ_LARGE;
				add_oprop(otmp, OPROP_CONC);
				set_material_gm(otmp, LEATHER);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_NOINIT);
				otmp->objsize = MZ_LARGE;
				add_oprop(otmp, OPROP_CONC);
				(void) mpickobj(mtmp, otmp);
			}
			if(ptr->mtyp == PM_AMM_KAMEREL){
				if(rn2(10)){//Physical fighter, no magic
					set_mcan(mtmp, TRUE);
					if(rn2(10)){//Warrior
						mongets(mtmp, MIRRORBLADE, mkobjflags);
						otmp = mksobj(ROUNDSHIELD, mkobjflags);
						switch(rn2(3)){
							case 0:
								set_material_gm(otmp, COPPER);
							break;
							case 1:
								set_material_gm(otmp, GLASS);
							break;
							case 2:
								set_material_gm(otmp, SILVER);
							break;
						}
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						
						otmp = mksobj(BANDED_MAIL, mkobjflags);
						switch(rn2(3)){
							case 0:
								set_material_gm(otmp, COPPER);
							break;
							case 1:
								set_material_gm(otmp, GLASS);
							break;
							case 2:
								set_material_gm(otmp, SILVER);
							break;
						}
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					} else { //Fighter driver
						mtmp->m_lev += 3;
						mtmp->mhpmax += d(3,8);
						mtmp->mhp = mtmp->mhpmax;
						mtmp->mspeed = MFAST;
						mtmp->permspeed = MFAST;
						
						otmp = mksobj(MIRRORBLADE, mkobjflags);
						if(otmp->spe < 1) otmp->spe = 1;
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						
						
						otmp = mksobj(ROUNDSHIELD, mkobjflags);
						switch(rn2(3)){
							case 0:
								set_material_gm(otmp, COPPER);
							break;
							case 1:
								set_material_gm(otmp, GLASS);
							break;
							case 2:
								set_material_gm(otmp, SILVER);
							break;
						}
						if(otmp->spe < 1) otmp->spe = 1;
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						
						otmp = mksobj(PLATE_MAIL, mkobjflags);
						switch(rn2(3)){
							case 0:
								set_material_gm(otmp, COPPER);
							break;
							case 1:
								set_material_gm(otmp, GLASS);
							break;
							case 2:
								set_material_gm(otmp, SILVER);
							break;
						}
						otmp->objsize = MZ_SMALL;
						if(otmp->spe < 1) otmp->spe = 1;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					}
				} else {
					if(rn2(10)){ //Wizard driver
						mtmp->m_lev += 3;
						mtmp->mhpmax += d(3,8);
						mtmp->mhp = mtmp->mhpmax;
						otmp = mksobj(MIRROR, mkobjflags|MKOBJ_NOINIT);
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						switch(rn2(3)){
							case 0:
								(void) mongets(mtmp, ELVEN_CLOAK, mkobjflags);
							break;
							case 1:
								(void) mongets(mtmp, DWARVISH_CLOAK, mkobjflags);
							break;
							case 2:
								(void) mongets(mtmp, CLOAK_OF_INVISIBILITY, mkobjflags);
							break;
						}
						
						otmp = mksobj(STILETTO, mkobjflags);
						switch(rn2(3)){
							case 0:
								set_material_gm(otmp, COPPER);
							break;
							case 1:
								set_material_gm(otmp, GLASS);
							break;
							case 2:
								set_material_gm(otmp, SILVER);
							break;
						}
						if(otmp->spe < 1) otmp->spe = 1;
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					} else { //Wizard leader
						mtmp->m_lev += 7;
						mtmp->mhpmax += d(7,8);
						mtmp->mhp = mtmp->mhpmax;
						otmp = mksobj(MIRROR, mkobjflags|MKOBJ_NOINIT);
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						(void) mongets(mtmp, CLOAK_OF_PROTECTION, mkobjflags);
						
						otmp = mksobj(STILETTO, mkobjflags);
						switch(rn2(3)){
							case 0:
								set_material_gm(otmp, COPPER);
							break;
							case 1:
								set_material_gm(otmp, GLASS);
							break;
							case 2:
								set_material_gm(otmp, SILVER);
							break;
						}
						if(otmp->spe < 1) otmp->spe = 1;
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					}
				}
			}
			if(ptr->mtyp == PM_HUDOR_KAMEREL){
				otmp = mksobj(MIRROR, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, GLASS);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			}
			if(ptr->mtyp == PM_SHARAB_KAMEREL){
				otmp = mksobj(MIRROR, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, GLASS);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			}
			if(ptr->mtyp == PM_ARA_KAMEREL){
				otmp = mksobj(KAMEREL_VAJRA, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				otmp->spe = 1;
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(MIRROR, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			}
		break;}
	    case S_LAW_ANGEL:
	    case S_NEU_ANGEL:
	    case S_CHA_ANGEL:
		{
		int spe2;
			if(In_mordor_quest(&u.uz) 
				&& !In_mordor_forest(&u.uz)
				&& !Is_ford_level(&u.uz)
				&& !In_mordor_fields(&u.uz)
				&& in_mklev
			){
				(void)mongets(mtmp, SHACKLES, mkobjflags);
				mtmp->entangled = SHACKLES;
				return;
			}
/*			if(ptr->mtyp == PM_DESTROYER){
				struct obj *otmp = mksobj(BROADSWORD, mkobjflags);
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 10;
				(void) mpickobj(mtmp,otmp);

				otmp = mksobj(BROADSWORD, mkobjflags);
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 10;
				(void) mpickobj(mtmp,otmp);

				otmp = mksobj(RIN_CONFLICT, mkobjflags);
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				(void) mpickobj(mtmp,otmp);

				otmp = mksobj(RIN_INCREASE_DAMAGE, mkobjflags);
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 19;
				(void) mpickobj(mtmp,otmp);

				otmp = mksobj(SKELETON_KEY, mkobjflags);
				otmp = oname(otmp, artiname(ART_THIRD_KEY_OF_LAW));
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);
				return;

			}
			else if(ptr->mtyp == PM_DANCER){
				struct obj *otmp = mksobj(STILETTO, mkobjflags);
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 19;
				(void) mpickobj(mtmp,otmp);

				otmp = mksobj(STILETTO, mkobjflags);
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 19;
				(void) mpickobj(mtmp,otmp);

				otmp = mksobj(RIN_INCREASE_DAMAGE, mkobjflags);
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->spe = 28;
				(void) mpickobj(mtmp,otmp);
				return;

			}
*/
			if(ptr->mtyp == PM_ARCADIAN_AVENGER){
				(void)mongets(mtmp, SHORT_SWORD, mkobjflags);
				(void)mongets(mtmp, SHORT_SWORD, mkobjflags);
				return;//no random stuff
			} else if(ptr->mtyp == PM_JUSTICE_ARCHON){
				(void) mongets(mtmp, GENTLEWOMAN_S_DRESS, mkobjflags);
				
				(void) mongets(mtmp, HELMET, mkobjflags);
				
				(void) mongets(mtmp, GAUNTLETS, mkobjflags);
				
				otmp = mongets(mtmp, ARMORED_BOOTS, mkobjflags);
				if(otmp) set_material_gm(otmp, IRON);
				
				(void) mongets(mtmp, LONG_SWORD, mkobjflags);

				(void) mongets(mtmp, KITE_SHIELD, mkobjflags);
			} else if(ptr->mtyp == PM_SWORD_ARCHON){
					//Nothing
			} else if(ptr->mtyp == PM_SHIELD_ARCHON){
				(void) mongets(mtmp, SCALE_MAIL, mkobjflags);

				(void) mongets(mtmp, HELMET, mkobjflags);

				(void) mongets(mtmp, GAUNTLETS, mkobjflags);

				otmp = mongets(mtmp, ARMORED_BOOTS, mkobjflags);
				if(otmp) set_material_gm(otmp, IRON);

				(void) mongets(mtmp, LANCE, mkobjflags);

				(void) mongets(mtmp, WAR_HAMMER, mkobjflags);

				(void) mongets(mtmp, KITE_SHIELD, mkobjflags);
				if(otmp){
					otmp->objsize = MZ_LARGE;
					otmp->spe = max(otmp->spe, 3);
					fix_object(otmp);
				}
			} else if(ptr->mtyp == PM_TRUMPET_ARCHON){
				otmp = mongets(mtmp, CLOAK, mkobjflags);
				if(otmp) otmp->spe = max(otmp->spe, 3);

				(void) mongets(mtmp, GLOVES, mkobjflags);

				otmp = mongets(mtmp, ARMORED_BOOTS, mkobjflags);
				if(otmp) set_material_gm(otmp, SILVER);

				otmp = mongets(mtmp, TWO_HANDED_SWORD, mkobjflags);
				if(otmp){
					otmp->spe = max(otmp->spe, 3);
					set_material_gm(otmp, SILVER);
				}
			} else if(ptr->mtyp == PM_THRONE_ARCHON){
				int artnum = rn2(8);
	
			    /* create minion stuff; can't use mongets */
			    otmp = mksobj(angelwepsbase[artnum], mkobjflags);
	
			    /* make it special */
				otmp = oname(otmp, artiname(angelweps[artnum]));
			    bless(otmp);
			    otmp->spe = 7;
				otmp->objsize = MZ_LARGE;
				set_material_gm(otmp, SILVER);
				if(!otmp->oartifact){
					if(rn2(2)){
						add_oprop(otmp, OPROP_HOLYW);
					}
					else {
						add_oprop(otmp, OPROP_AXIOW);
					}
					if(In_endgame(&u.uz) || goodequip){
						add_oprop(otmp, rn2(7) ? OPROP_FIREW : OPROP_ELECW);
					}
				}
				fix_object(otmp);
			    (void) mpickobj(mtmp, otmp);

			    (void) mongets(mtmp, SHIELD_OF_REFLECTION, mkobjflags);

				(void) mongets(mtmp, ARCHAIC_PLATE_MAIL, mkobjflags);

				otmp = mongets(mtmp, find_gcirclet(), mkobjflags);
				if(otmp){
					otmp->oerodeproof = TRUE;
					set_material_gm(otmp, COPPER);
				}

				(void) mongets(mtmp, ARCHAIC_GAUNTLETS, mkobjflags);

				(void) mongets(mtmp, ARCHAIC_BOOTS, mkobjflags);
			} else if(ptr->mtyp == PM_LIGHT_ARCHON){
				int artnum = rn2(8);
	
			    /* create minion stuff; can't use mongets */
			    otmp = mksobj(angelwepsbase[artnum], mkobjflags|MKOBJ_NOINIT);
	
			    /* make it special */
				otmp = oname(otmp, artiname(angelweps[artnum]));
			    bless(otmp);
			    otmp->spe = 7;
				otmp->objsize = MZ_LARGE;
				set_material_gm(otmp, SILVER);
				fix_object(otmp);
			    (void) mpickobj(mtmp, otmp);
	
				if(artnum < ANGELTWOHANDERCUT){
					otmp = mongets(mtmp, SHIELD_OF_REFLECTION, mkobjflags);
					if(otmp) set_material_gm(otmp, GLASS);
				} else {
					otmp = mongets(mtmp, AMULET_OF_REFLECTION, mkobjflags);
					if(otmp) set_material_gm(otmp, GLASS);
				}
				
				otmp = mongets(mtmp, HELM_OF_BRILLIANCE, mkobjflags);
				if(otmp) set_material_gm(otmp, GLASS);
				
				otmp = mongets(mtmp, PLATE_MAIL, mkobjflags);
				if(otmp) set_material_gm(otmp, GLASS);
			} else if(ptr->mtyp == PM_MONADIC_DEVA){
				otmp = mongets(mtmp, TWO_HANDED_SWORD, mkobjflags);
				if(otmp){
					spe2 = rn2(4);
					otmp->spe = max(otmp->spe, spe2);
					if(In_endgame(&u.uz) || goodequip || !rn2(20)){
						add_oprop(otmp, OPROP_FIREW);
					}
					else if(!rn2(4)){
						add_oprop(otmp, OPROP_LESSER_FIREW);
					}
				}
			} else if(ptr->mtyp == PM_MOVANIC_DEVA){
				otmp = mongets(mtmp, MORNING_STAR, mkobjflags);
				if(otmp){
					spe2 = rn2(4);
					otmp->spe = max(otmp->spe, spe2);
				}

				(void)mongets(mtmp, BUCKLER, mkobjflags);

				(void)mongets(mtmp, ROBE, mkobjflags);
			} else if(ptr->mtyp == PM_ASTRAL_DEVA){
				otmp = mongets(mtmp, MACE, mkobjflags);
				if(otmp){
					otmp->spe = 7;

					otmp = mongets(mtmp, CHAIN_MAIL, mkobjflags);
					otmp->spe = 7;
				}
			} else if(ptr->mtyp == PM_GRAHA_DEVA){
				otmp = mongets(mtmp, TWO_HANDED_SWORD, mkobjflags);
				if(otmp){
					set_material_gm(otmp, COPPER);
					otmp->spe = 9;
				}

			    otmp = mongets(mtmp, ARCHAIC_PLATE_MAIL, mkobjflags);
				if(otmp){
					otmp->oerodeproof = TRUE;
					otmp->spe = 7;
				}

			    otmp = mongets(mtmp, ARCHAIC_BOOTS, mkobjflags);
			    if(otmp) otmp->oerodeproof = TRUE;

			    otmp = mongets(mtmp, ARCHAIC_GAUNTLETS, mkobjflags);
			    if(otmp) otmp->oerodeproof = TRUE;
			} else if(ptr->mtyp == PM_SURYA_DEVA){
				struct monst *dancer;
				int mmflags = MM_ADJACENTOK|MM_NOCOUNTBIRTH;
				if (get_mx(mtmp, MX_ESUM))
					mmflags |= MM_ESUM;
				dancer = makemon(&mons[PM_DANCING_BLADE], mtmp->mx, mtmp->my, mmflags);
				if(dancer){
					dancer->mvar_suryaID = (long)mtmp->m_id;
					dancer->mpeaceful = mtmp->mpeaceful;
					if (mmflags&MM_ESUM)
						mark_mon_as_summoned(dancer, mtmp, ESUMMON_PERMANENT, 0);
				}

			    otmp = mongets(mtmp, PLATE_MAIL, mkobjflags);
				if(otmp){
					set_material_gm(otmp, GOLD);
					otmp->spe = 7;
				}

			    otmp = mongets(mtmp, ARMORED_BOOTS, mkobjflags);
			    if(otmp) set_material_gm(otmp, GOLD);

			    otmp = mongets(mtmp, GAUNTLETS_OF_POWER, mkobjflags);
			    if(otmp) set_material_gm(otmp, GOLD);

			    otmp = mongets(mtmp, AMULET_OF_REFLECTION, mkobjflags);
			    if(otmp) set_material_gm(otmp, GOLD);
			} else if(ptr->mtyp == PM_MAHADEVA){
				otmp = mongets(mtmp, SCIMITAR, mkobjflags);
				if(otmp){
					otmp->oerodeproof = TRUE;
					otmp->spe = 9;
				}

				otmp = mongets(mtmp, SCIMITAR, mkobjflags);
				if(otmp){
					otmp->oerodeproof = TRUE;
					otmp->spe = 9;
				}

				otmp = mongets(mtmp, SCIMITAR, mkobjflags);
				if(otmp){
					otmp->oerodeproof = TRUE;
					otmp->spe = 9;
				}

				otmp = mongets(mtmp, SCIMITAR, mkobjflags);
				if(otmp){
					otmp->oerodeproof = TRUE;
					otmp->spe = 9;
				}

			    otmp = mongets(mtmp, ARCHAIC_PLATE_MAIL, mkobjflags);
				if(otmp){
					otmp->oerodeproof = TRUE;
					otmp->spe = 9;
				}
			} else if(ptr->mtyp == PM_COURE_ELADRIN){
				(void)mongets(mtmp, GLOVES, mkobjflags);
				(void)mongets(mtmp, JACKET, mkobjflags);
				(void)mongets(mtmp, LOW_BOOTS, mkobjflags);
				(void)mongets(mtmp, BOW, mkobjflags);
				m_initthrow(mtmp, ARROW, d(4,4), mkobjflags);
				switch(rnd(4)){
					case 1:
					(void)mongets(mtmp, RAPIER, mkobjflags);
					break;
					case 2:
					(void)mongets(mtmp, SCIMITAR, mkobjflags);
					(void)mongets(mtmp, KITE_SHIELD, mkobjflags);
					break;
					case 3:
					(void)mongets(mtmp, SHORT_SWORD, mkobjflags);
					(void)mongets(mtmp, BUCKLER, mkobjflags);
					break;
					case 4:
					(void)mongets(mtmp, MOON_AXE, mkobjflags);
					break;
				}
			} else if(ptr->mtyp == PM_NOVIERE_ELADRIN){
				(void)mongets(mtmp, GLOVES, mkobjflags);
				(void)mongets(mtmp, JACKET, mkobjflags);
				(void)mongets(mtmp, LOW_BOOTS, mkobjflags);
				(void)mongets(mtmp, LEATHER_HELM, mkobjflags);
				(void)mongets(mtmp, ELVEN_SPEAR, mkobjflags);
				(void)mongets(mtmp, rn2(2) ? ELVEN_SICKLE : RAPIER, mkobjflags);
			} else if(ptr->mtyp == PM_BRALANI_ELADRIN){
				(void)mongets(mtmp, CHAIN_MAIL, mkobjflags);
				(void)mongets(mtmp, CLOAK, mkobjflags);
				(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
				(void)mongets(mtmp, HELMET, mkobjflags);
				(void)mongets(mtmp, DWARVISH_SPEAR, mkobjflags);
				(void)mongets(mtmp, DWARVISH_SPEAR, mkobjflags);
				(void)mongets(mtmp, DWARVISH_SHORT_SWORD, mkobjflags);
			} else if(ptr->mtyp == PM_FIRRE_ELADRIN){
				(void)mongets(mtmp, ELVEN_MITHRIL_COAT, mkobjflags);
				(void)mongets(mtmp, ELVEN_SHIELD, mkobjflags);
				(void)mongets(mtmp, ELVEN_CLOAK, mkobjflags);
				(void)mongets(mtmp, ELVEN_BOOTS, mkobjflags);
				(void)mongets(mtmp, ELVEN_HELM, mkobjflags);
				(void)mongets(mtmp, ELVEN_SPEAR, mkobjflags);
				(void)mongets(mtmp, ELVEN_BROADSWORD, mkobjflags);
			} else if(ptr->mtyp == PM_SHIERE_ELADRIN){
				(void)mongets(mtmp, CRYSTAL_PLATE_MAIL, mkobjflags);
				(void)mongets(mtmp, CRYSTAL_SHIELD, mkobjflags);
				(void)mongets(mtmp, CRYSTAL_BOOTS, mkobjflags);
				(void)mongets(mtmp, CRYSTAL_SWORD, mkobjflags);
				(void)mongets(mtmp, WAN_STRIKING, mkobjflags);
			} else if(ptr->mtyp == PM_GHAELE_ELADRIN){
				(void)mongets(mtmp, ARCHAIC_PLATE_MAIL, mkobjflags);
				(void)mongets(mtmp, ROUNDSHIELD, mkobjflags);
				(void)mongets(mtmp, ARCHAIC_BOOTS, mkobjflags);
				(void)mongets(mtmp, ARCHAIC_HELM, mkobjflags);
				otmp = mksobj(LONG_SWORD, mkobjflags);
				set_material_gm(otmp, COPPER);
			    (void) mpickobj(mtmp, otmp);
			} else if(ptr->mtyp == PM_TULANI_ELADRIN){
#define HOLY_TULANI_ARMOR(item) otmp = mongets(mtmp, item, mkobjflags);\
			if(otmp){\
				set_material_gm(otmp, mat);\
				if(stone) otmp->ovar1 = stone;\
				add_oprop(otmp, OPROP_HOLY);\
				if(is_gloves(otmp) || is_boots(otmp))\
					add_oprop(otmp, OPROP_HOLYW);\
				bless(otmp);\
			}

				int mat = 0;
				int stone = 0;
				switch(((In_endgame(&u.uz) || goodequip) && !rn2(4)) ? 0 : rn2(20)){
				case 0:
				mat = !rn2(5) ? GEMSTONE : !rn2(4) ? OBSIDIAN_MT : GLASS;
				if(mat == GEMSTONE)
					stone = !rn2(4) ? DIAMOND : !rn2(3) ? STAR_SAPPHIRE : OPAL;
				HOLY_TULANI_ARMOR(CRYSTAL_BOOTS)
				(void)mongets(mtmp, ELVEN_CLOAK, mkobjflags);
				HOLY_TULANI_ARMOR(CRYSTAL_PLATE_MAIL)
				HOLY_TULANI_ARMOR(CRYSTAL_GAUNTLETS)
				HOLY_TULANI_ARMOR(HELMET)
#undef HOLY_TULANI_ARMOR
				break;
				case 1:
					if(!mat) mat = MITHRIL;
				case 2:
					if(!mat) mat = COPPER;
				case 3:
					if(!mat) mat = SILVER;
				case 4:
					if(!mat) mat = GLASS;
				(void) mongets(mtmp, ELVEN_BOOTS, mkobjflags);
				(void) mongets(mtmp, ELVEN_CLOAK, mkobjflags);
				otmp = mongets(mtmp, HIGH_ELVEN_PLATE, mkobjflags);
				if(otmp) set_material_gm(otmp, mat);
				otmp = mongets(mtmp, HIGH_ELVEN_GAUNTLETS, mkobjflags);
				if(otmp) set_material_gm(otmp, mat);
				otmp = mongets(mtmp, HIGH_ELVEN_HELM, mkobjflags);
				if(otmp) set_material_gm(otmp, mat);
				break;
				default:
				(void)mongets(mtmp, CRYSTAL_PLATE_MAIL, mkobjflags);
				(void)mongets(mtmp, ELVEN_CLOAK, mkobjflags);
				(void)mongets(mtmp, CRYSTAL_BOOTS, mkobjflags);
				(void)mongets(mtmp, CRYSTAL_GAUNTLETS, mkobjflags);
				(void)mongets(mtmp, CRYSTAL_HELM, mkobjflags);
				break;
				}
			} else if(ptr->mtyp == PM_GAE_ELADRIN){
				int gemstone = rn2(3) ? EMERALD : !rn2(4) ? RUBY : !rn2(3) ? JACINTH : rn2(2) ? TOPAZ : DIAMOND;
				switch(rnd(6)){
					case 1:
					otmp = mksobj(CRYSTAL_PLATE_MAIL, mkobjflags);
					set_material_gm(otmp, GEMSTONE);
					otmp->ovar1 = gemstone;
					fix_object(otmp);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(ELVEN_CLOAK, mkobjflags);
					add_oprop(otmp, OPROP_MAGC);
					fix_object(otmp);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(CRYSTAL_GAUNTLETS, mkobjflags);
					set_material_gm(otmp, GEMSTONE);
					otmp->ovar1 = gemstone;
					fix_object(otmp);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(rn2(4) ? ARCHAIC_HELM : find_gcirclet(), mkobjflags);
					if(gemstone == EMERALD && rn2(2)){
						set_material_gm(otmp, GEMSTONE);
						otmp->ovar1 = gemstone;
					} else {
						set_material_gm(otmp, GOLD);
					}
					fix_object(otmp);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(CRYSTAL_SWORD, mkobjflags);
					set_material_gm(otmp, GEMSTONE);
					otmp->ovar1 = gemstone;
					fix_object(otmp);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(SICKLE, mkobjflags);
					set_material_gm(otmp, GEMSTONE);
					otmp->ovar1 = gemstone;
					fix_object(otmp);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 2:
					otmp = mksobj(CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
					set_material_gm(otmp, CLOTH);
					fix_object(otmp);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					(void)mongets(mtmp, QUARTERSTAFF, mkobjflags);
					break;
					case 3:
					otmp = mksobj(ARCHAIC_GAUNTLETS, mkobjflags);
					add_oprop(otmp, OPROP_MAGC);
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 4:
					otmp = mksobj(AMULET_OF_LIFE_SAVING, mkobjflags);
					add_oprop(otmp, OPROP_MAGC);
					add_oprop(otmp, OPROP_REFL);
					set_material_gm(otmp, GEMSTONE);
					otmp->ovar1 = gemstone;
					fix_object(otmp);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 5:
						otmp = mksobj(find_gcirclet(), mkobjflags);
						set_material_gm(otmp, VEGGY);
						add_oprop(otmp, OPROP_ANAR);
						add_oprop(otmp, OPROP_HOLY);
						bless(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(CLOAK, mkobjflags);
						set_material_gm(otmp, VEGGY);
						add_oprop(otmp, OPROP_ANAR);
						add_oprop(otmp, OPROP_HOLY);
						bless(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(QUARTERSTAFF, mkobjflags);
						set_material_gm(otmp, WOOD);
						add_oprop(otmp, OPROP_ANARW);
						add_oprop(otmp, OPROP_HOLYW);
						bless(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
					case 6:
					break;
				}
			} else if(ptr->mtyp == PM_BRIGHID_ELADRIN){
				otmp = mongets(mtmp, CRYSTAL_SWORD, mkobjflags);
				if(otmp){
					add_oprop(otmp, OPROP_LESSER_FIREW);
					set_material_gm(otmp, OBSIDIAN_MT);
					otmp->oerodeproof = TRUE;
				}
				int armors[] = {CRYSTAL_SHIELD, CRYSTAL_PLATE_MAIL, CRYSTAL_GAUNTLETS, CRYSTAL_BOOTS};
				for(int i = 0; i < SIZE(armors); i++){
					otmp = mongets(mtmp, armors[i], mkobjflags);
					if(otmp){
						set_material_gm(otmp, OBSIDIAN_MT);
						otmp->oerodeproof = TRUE;
					}
				}
			} else if(ptr->mtyp == PM_CAILLEA_ELADRIN){
#define CAILLEA_ARMOR(item) otmp = mongets(mtmp, item, mkobjflags);\
				if(otmp){\
					otmp->obj_color = CLR_BLACK;\
					add_oprop(otmp, OPROP_COLD);\
					add_oprop(otmp, OPROP_LESSER_COLDW);\
				}
				
				CAILLEA_ARMOR(HIGH_BOOTS)
				if(mtmp->female){
					CAILLEA_ARMOR(PLAIN_DRESS)
					CAILLEA_ARMOR(LONG_GLOVES)
				}
				else {
					CAILLEA_ARMOR(JACKET)
					CAILLEA_ARMOR(GLOVES)
				}
#undef CAILLEA_ARMOR
				int pom = phase_of_the_moon();
				if(pom == 4){
					otmp = mongets(mtmp, MOON_AXE, mkobjflags);
					if(otmp){
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
					}
					otmp = mongets(mtmp, MOON_AXE, mkobjflags);
					if(otmp){
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
					}
				}
				else if(pom == 1 || pom == 7){
					int typ = rn2(4) ? SICKLE : KHOPESH;
					otmp = mongets(mtmp, typ, mkobjflags);
					if(otmp) set_material_gm(otmp, SILVER);
					otmp = mongets(mtmp, typ, mkobjflags);
					if(otmp) set_material_gm(otmp, SILVER);
				}
				else if(pom == 2 || pom == 6) {
					otmp = mongets(mtmp, AXE, mkobjflags);
					if(otmp) set_material_gm(otmp, SILVER);
					otmp = mongets(mtmp, AXE, mkobjflags);
					if(otmp) set_material_gm(otmp, SILVER);
				}
				else if(pom == 3 || pom == 5) {
					otmp = mongets(mtmp, MOON_AXE, mkobjflags);
					if(otmp){
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
					}
					otmp = mongets(mtmp, MOON_AXE, mkobjflags);
					if(otmp){
						otmp->objsize = MZ_SMALL;
						fix_object(otmp);
					}
				}
				else { //new moon
					int moontype = rnd(4);
					for(int i = 2; i > 0; i--){
						otmp = mongets(mtmp, MOON_AXE, mkobjflags);
						if(otmp){
							switch(moontype){
								default:
								break;
								case 1:
									set_material_gm(otmp, GOLD);
								break;
								case 2:
									set_material_gm(otmp, OBSIDIAN_MT);
									otmp->ovar1 = FULL_MOON;
								break;
							}
							otmp->objsize = MZ_SMALL;
							fix_object(otmp);
						}
					}
				}
			} else if(ptr->mtyp == PM_POLYPOID_BEING){
				int masktypes[] = {PM_ELVENKING, PM_ELVENQUEEN, PM_ALABASTER_ELF_ELDER, PM_GROVE_GUARDIAN, 
								   PM_TULANI_ELADRIN, PM_GAE_ELADRIN, PM_LILLEND, 
								   PM_DARK_YOUNG, PM_GOAT_SPAWN, PM_TITAN};
				int i;
				for(i = d(3,3); i > 0; i--){
					otmp = mksobj(MASK, mkobjflags|MKOBJ_NOINIT);
					otmp->corpsenm = masktypes[rn2(SIZE(masktypes))];
					set_material_gm(otmp, WOOD);
					bless(otmp);
					(void) mpickobj(mtmp, otmp);
				}
			} else if(ptr->mtyp == PM_KUKER){
				switch(rnd(4)){
					case 1:
					case 2:
						otmp = mksobj(SCIMITAR, mkobjflags|MKOBJ_NOINIT);
					break;
					case 3:
						otmp = mksobj(QUARTERSTAFF, mkobjflags|MKOBJ_NOINIT);
					break;
					case 4:
						otmp = mksobj(SPEAR, mkobjflags|MKOBJ_NOINIT);
					break;
				}
				add_oprop(otmp, OPROP_HOLYW);
				add_oprop(otmp, OPROP_LESSER_FIREW);
				otmp->spe = 7;
				set_material_gm(otmp, WOOD);
				bless(otmp);
				(void) mpickobj(mtmp, otmp);
				(void)mongets(mtmp, BELL, mkobjflags);
			} else if(ptr->mtyp == PM_GWYNHARWYF){
				(void)mongets(mtmp, CLOAK, mkobjflags);
				(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
				(void)mongets(mtmp, SCIMITAR, mkobjflags);
			} else if(ptr->mtyp == PM_OONA){
					//Note: Adjustments to how Oona's melee attacks were handled made her very weak without a weapon
					//Also note: monster inventories are last-in-first-out, and oproperty weapons are favored, so the offhand weapon needs to be first
					otmp = mksobj(STILETTO, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = 3;
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					switch(u.oonaenergy){
						case AD_COLD:
							add_oprop(otmp, OPROP_OONA_FIREW);
						break;
						case AD_FIRE:
							add_oprop(otmp, OPROP_OONA_ELECW);
						break;
						case AD_ELEC:
							add_oprop(otmp, OPROP_OONA_COLDW);
						break;
					}
					set_material_gm(otmp, PLATINUM);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(LONG_SWORD, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = 3;
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					switch(u.oonaenergy){
						case AD_COLD:
							add_oprop(otmp, OPROP_OONA_ELECW);
						break;
						case AD_FIRE:
							add_oprop(otmp, OPROP_OONA_COLDW);
						break;
						case AD_ELEC:
							add_oprop(otmp, OPROP_OONA_FIREW);
						break;
					}
					set_material_gm(otmp, PLATINUM);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
			} else if(ptr->mtyp == PM_LILLEND){
				static char lama_count = 0;
				if(Is_lamashtu_level(&u.uz) && lama_count < 2){
					otmp = mongets(mtmp, MASK, mkobjflags);
					if(otmp) otmp->corpsenm = lama_count == 0 ? PM_DEMOGORGON : PM_DAGON; //PM_OBOX_OB
					otmp = mongets(mtmp, MASK, mkobjflags);
					if(otmp) otmp->corpsenm = lama_count == 0 ? PM_NESSIAN_PIT_FIEND : PM_KHAAMNUN_TANNIN; //Occularus
					otmp = mongets(mtmp, MASK, mkobjflags);
					if(otmp) otmp->corpsenm = lama_count == 0 ? PM_SHAYATEEN : PM_RAGLAYIM_TANNIN;  //PM_AKKABISH_TANNIN
					otmp = mongets(mtmp, MASK, mkobjflags);
					if(otmp) otmp->corpsenm = PM_LETHE_ELEMENTAL;
					otmp = mongets(mtmp, MASK, mkobjflags);
					if(otmp) otmp->corpsenm = PM_SHOGGOTH;
					otmp = mongets(mtmp, MASK, mkobjflags);
					if(otmp) otmp->corpsenm = lama_count == 0 ? PM_DEATH_KNIGHT : PM_SARTAN_TANNIN;

					//WEAPON
					if(lama_count == 0){
						otmp = mongets(mtmp, SCYTHE, mkobjflags);
						if(otmp){
							set_material_gm(otmp, DRAGON_HIDE);
							add_oprop(otmp, OPROP_FLAYW);
							add_oprop(otmp, OPROP_VORPW);
						}
					}
					else if(lama_count == 1){
						otmp = mongets(mtmp, BULLWHIP, mkobjflags);
						if(otmp){
							set_material_gm(otmp, SHELL_MAT);
							add_oprop(otmp, OPROP_FLAYW);
						}
						otmp = mongets(mtmp, BULLWHIP, mkobjflags);
						if(otmp){
							set_material_gm(otmp, SHELL_MAT);
							add_oprop(otmp, OPROP_FLAYW);
						}
					}
					
					otmp = mongets(mtmp, HELMET, mkobjflags);
					if(otmp){
						set_material_gm(otmp, SILVER);
						add_oprop(otmp, OPROP_LIFE);
						add_oprop(otmp, OPROP_HOLY);
					}
					otmp = mongets(mtmp, PLATE_MAIL, mkobjflags);
					if(otmp){
						set_material_gm(otmp, SILVER);
						add_oprop(otmp, OPROP_REFL);
						add_oprop(otmp, OPROP_LIFE);
						add_oprop(otmp, OPROP_HOLY);
					}
					otmp = mongets(mtmp, GAUNTLETS_OF_POWER, mkobjflags);
					if(otmp){
						set_material_gm(otmp, SILVER);
						add_oprop(otmp, OPROP_HOLY);
						add_oprop(otmp, OPROP_HOLYW);
					}
					otmp = mongets(mtmp, CLOAK_OF_PROTECTION, mkobjflags);
					if(otmp){
						add_oprop(otmp, OPROP_WOOL);
						otmp->obj_color = CLR_WHITE;
					}
					
					lama_count++;
				}
				else if((In_endgame(&u.uz) || goodequip)){
					const int generic_nasties[] = {
						PM_BLACK_DRAGON, PM_COCKATRICE, PM_STORM_GIANT,
						PM_MINOTAUR, PM_UMBER_HULK, PM_MASTER_MIND_FLAYER,
						PM_IRON_GOLEM, PM_GREEN_SLIME, PM_GIANT_TURTLE,
						PM_GREMLIN, PM_VAMPIRE_LORD, PM_VAMPIRE_LADY,
						PM_ARCH_LICH,
						PM_DISENCHANTER, PM_MANTICORE, PM_SCRAP_TITAN,
						PM_GUG, PM_BEBELITH, PM_DAUGHTER_OF_BEDLAM,
						PM_ANCIENT_NAGA,PM_GUARDIAN_NAGA, PM_SERPENT_NECKED_LIONESS, 
						PM_EDDERKOP, PM_HELLFIRE_COLOSSUS, PM_GREEN_STEEL_GOLEM,
						PM_NIGHTGAUNT
					};
					const int earth_nasties[] = {
						PM_EARTH_ELEMENTAL, PM_TERRACOTTA_SOLDIER, PM_ACID_PARAELEMENTAL,
						PM_HEAVEN_PIERCER, PM_LONG_WORM, PM_OREAD
					};
					const int air_nasties[] = {
						PM_AIR_ELEMENTAL, PM_LIGHTNING_PARAELEMENTAL, PM_MORTAI,
						PM_LIVING_MIRAGE, PM_YUKI_ONNA, PM_RAVEN
					};
					const int fire_nasties[] = {
						PM_FIRE_ELEMENTAL, PM_POISON_PARAELEMENTAL, PM_SALAMANDER,
						PM_DRYAD, PM_SUNFLOWER, PM_BURNING_FERN
					};
					const int water_nasties[] = {
						PM_WATER_ELEMENTAL, PM_ICE_PARAELEMENTAL, PM_FORD_ELEMENTAL,
						PM_NAIAD, PM_DEEPEST_ONE, PM_JELLYFISH,
						PM_GREAT_WHITE_SHARK
					};
					if(Is_earthlevel(&u.uz)){
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(generic_nasties);
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(generic_nasties);
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(earth_nasties);
					}
					else if(Is_airlevel(&u.uz)){
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(generic_nasties);
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(generic_nasties);
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(air_nasties);
					}
					else if(Is_firelevel(&u.uz)){
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(generic_nasties);
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(generic_nasties);
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(fire_nasties);
					}
					else if(Is_waterlevel(&u.uz)){
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(generic_nasties);
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(generic_nasties);
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(water_nasties);
					}
					else {
						const int astral_nasties[] = {
							PM_VALAVI, PM_THRIAE,
							PM_ELOCATOR, PM_AMMIT,
							PM_ALEAX, PM_ANGEL,
							PM_FALLEN_ANGEL, PM_SWORD_ARCHON,
							PM_THRONE_ARCHON, PM_LIGHT_ARCHON,
							PM_ASTRAL_DEVA, PM_MAHADEVA,
							PM_TULANI_ELADRIN, PM_GAE_ELADRIN,
							PM_BRIGHID_ELADRIN, PM_UISCERRE_ELADRIN,
							PM_CAILLEA_ELADRIN, PM_DRACAE_ELADRIN,
							PM_HOD_SEPHIRAH
						};
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(generic_nasties);
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(generic_nasties);

						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(astral_nasties);
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = ROLL_FROM(astral_nasties);

						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = rn2(2) ? ROLL_FROM(air_nasties) : ROLL_FROM(earth_nasties);
						otmp = mongets(mtmp, MASK, mkobjflags);
						if(otmp) otmp->corpsenm = rn2(2) ? ROLL_FROM(fire_nasties) : ROLL_FROM(water_nasties);
					}

					otmp = mongets(mtmp, ELVEN_BOW, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						otmp->oerodeproof = TRUE;
						spe2 = 6;
						otmp->spe = max(otmp->spe, spe2);
						add_oprop(otmp, OPROP_LESSER_ANARW);
					}
					otmp = mongets(mtmp, ELVEN_ARROW, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						otmp->quan = 24+rnd(30);
						set_material_gm(otmp, SILVER);
						fix_object(otmp);
					}

					(void)mongets(mtmp, HARP, mkobjflags);

					otmp = mongets(mtmp, LONG_SWORD, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						spe2 = 6;
						otmp->spe = max(otmp->spe, spe2);
						set_material_gm(otmp, SILVER);
						add_oprop(otmp, OPROP_ANARW);
					}

					if(!rn2(6)){
						otmp = mongets(mtmp, HELMET, mkobjflags);
						if(otmp) set_material_gm(otmp, SILVER);
						otmp = mongets(mtmp, rn2(2) ? PLATE_MAIL : rn2(2) ? SCALE_MAIL : BANDED_MAIL, mkobjflags);
						if(otmp) set_material_gm(otmp, SILVER);
						otmp = mongets(mtmp, rn2(10) ? GAUNTLETS : GAUNTLETS_OF_POWER, mkobjflags);
						if(otmp) set_material_gm(otmp, SILVER);
					}
					else {
						otmp = mongets(mtmp, ROBE, mkobjflags);
						if(otmp){
							set_material_gm(otmp, CLOTH);
							if(!rn2(3))
								add_oprop(otmp, OPROP_WOOL);
						}
						otmp = mongets(mtmp, WAISTCLOTH, mkobjflags);
						if(otmp){
							set_material_gm(otmp, CLOTH);
							if(!rn2(3))
								add_oprop(otmp, OPROP_WOOL);
						}
					}
				}
				else {
					(void)mongets(mtmp, MASK, mkobjflags);
					(void)mongets(mtmp, MASK, mkobjflags);
					(void)mongets(mtmp, MASK, mkobjflags);
					(void)mongets(mtmp, MASK, mkobjflags);
					(void)mongets(mtmp, MASK, mkobjflags);
					(void)mongets(mtmp, MASK, mkobjflags);
					otmp = mongets(mtmp, ELVEN_BOW, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						otmp->oerodeproof = TRUE;
						spe2 = 3;
						otmp->spe = max(otmp->spe, spe2);
					}
					m_initthrow(mtmp, ELVEN_ARROW, 12+rnd(30), mkobjflags);
					(void)mongets(mtmp, HARP, mkobjflags);
					otmp = mongets(mtmp, LONG_SWORD, mkobjflags|MKOBJ_NOINIT);
					if(otmp){
						otmp->oerodeproof = TRUE;
						spe2 = 3;
						otmp->spe = max(otmp->spe, spe2);
					}
				}
			} else {
				//Generic angel artifact set
				int artnum = rn2(8);
	
			    /* create minion stuff; can't use mongets */
			    otmp = mksobj(angelwepsbase[artnum], mkobjflags);
	
			    /* maybe make it special */
			    if (!rn2(20) || is_lord(ptr))
					otmp = oname(otmp, artiname(angelweps[artnum]));
			    set_material_gm(otmp, SILVER);
			    bless(otmp);
			    if(is_lord(ptr)) spe2 = 7;
				else spe2 = rn2(4);
			    otmp->spe = max(otmp->spe, spe2);
			    (void) mpickobj(mtmp, otmp);
	
				if(artnum < ANGELTWOHANDERCUT){
					otmp = mksobj(!rn2(4) || is_lord(ptr) ?
						  SHIELD_OF_REFLECTION : KITE_SHIELD,
						  mkobjflags);
					otmp->spe = 0;
					otmp->cursed = FALSE;
					set_material_gm(otmp, SILVER);
					(void) mpickobj(mtmp, otmp);
				} else if(is_lord(ptr) || !rn2(20)){
					otmp = mksobj(AMULET_OF_REFLECTION, mkobjflags);
					otmp->cursed = FALSE;
					otmp->oerodeproof = TRUE;
					set_material_gm(otmp, SILVER);
					otmp->spe = 0;
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
				}
				//Generic angel armor
				if(Inhell || In_endgame(&u.uz) || goodequip){
					if(rn2(3)){
						otmp = mongets(mtmp, rn2(2) ? PLATE_MAIL : rn2(2) ? SCALE_MAIL : BANDED_MAIL, mkobjflags);
						if(otmp) set_material_gm(otmp, SILVER);
						otmp = mongets(mtmp, rn2(10) ? GAUNTLETS : GAUNTLETS_OF_POWER, mkobjflags);
						if(otmp) set_material_gm(otmp, SILVER);
						otmp = mongets(mtmp, ARMORED_BOOTS, mkobjflags);
						if(otmp) set_material_gm(otmp, SILVER);
					} else if(rn2(2)){
						(void) mongets(mtmp, rn2(2) ? PLATE_MAIL : rn2(2) ? SCALE_MAIL : BANDED_MAIL, mkobjflags);
						(void) mongets(mtmp, rn2(10) ? GAUNTLETS : GAUNTLETS_OF_POWER, mkobjflags);
						(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
					} else if(rn2(2)){
						otmp = mongets(mtmp, ROBE, mkobjflags);
						if(otmp){
							set_material_gm(otmp, CLOTH);
							if(!rn2(3))
								add_oprop(otmp, OPROP_WOOL);
						}
						otmp = mongets(mtmp, GLOVES, mkobjflags);
						if(otmp) set_material_gm(otmp, CLOTH);
						otmp = mongets(mtmp, SHOES, mkobjflags);
						if(otmp) set_material_gm(otmp, CLOTH);
					}
					else {
						otmp = mongets(mtmp, rn2(3) ? ROBE : WAISTCLOTH, mkobjflags);
						if(otmp){
							set_material_gm(otmp, CLOTH);
							otmp->oerodeproof = TRUE;
							if(!rn2(3))
								add_oprop(otmp, OPROP_WOOL);
						}
					}
				}
				else {
					otmp = mongets(mtmp, rn2(3) ? ROBE : WAISTCLOTH, mkobjflags);
					if(otmp) set_material_gm(otmp, CLOTH);
				}
			}
		}
		break;
		
		case S_GNOME:
		if(is_gnome(ptr)){
			int bias;
	
			bias = is_lord(ptr) + is_prince(ptr) * 2 + extra_nasty(ptr);
			switch(rnd(10 - (2 * bias))) {
			    case 1:
					m_initthrow(mtmp, DART, 12, mkobjflags);
				break;
			    case 2:
				    (void) mongets(mtmp, CROSSBOW, mkobjflags);
				    m_initthrow(mtmp, CROSSBOW_BOLT, 12, mkobjflags);
				break;
			    case 3:
				    (void) mongets(mtmp, SLING, mkobjflags);
				    m_initthrow(mtmp, ROCK, 12, mkobjflags);
					(void) mongets(mtmp, AKLYS, mkobjflags);
				break;
			    case 4:
					m_initthrow(mtmp, DAGGER, 3, mkobjflags);
				break;
			    case 5:
					(void) mongets(mtmp, AKLYS, mkobjflags);
				    (void) mongets(mtmp, SLING, mkobjflags);
				    m_initthrow(mtmp, ROCK, 3, mkobjflags);
				break;
			    default:
					(void) mongets(mtmp, AKLYS, mkobjflags);
				break;
			}
		} else {
			if(mm == PM_CLOCKWORK_AUTOMATON){
				(void) mongets(mtmp, ARCHAIC_HELM, mkobjflags);
				(void) mongets(mtmp, ARCHAIC_PLATE_MAIL, mkobjflags);
				(void) mongets(mtmp, ARCHAIC_GAUNTLETS, mkobjflags);
				(void) mongets(mtmp, ARCHAIC_BOOTS, mkobjflags);
				otmp = mksobj(MORNING_STAR, mkobjflags);
				set_material_gm(otmp, COPPER);
				(void) mpickobj(mtmp, otmp);
			} else if(mm == PM_GYNOID){
				otmp = mksobj(WHITE_VIBROSWORD, mkobjflags);
				fully_identify_obj(otmp);
				otmp->spe = abs(otmp->spe);
				(void) mpickobj(mtmp, otmp);
				(void) mongets(mtmp, PLAIN_DRESS, mkobjflags);
				(void) mongets(mtmp, LONG_GLOVES, mkobjflags);
				(void) mongets(mtmp, HEELED_BOOTS, mkobjflags);
				(void) mongets(mtmp, ANDROID_VISOR, mkobjflags);
			} else if(mm == PM_ANDROID){
				otmp = mksobj(GOLD_BLADED_VIBROSWORD, mkobjflags);
				fully_identify_obj(otmp);
				otmp->spe = abs(otmp->spe);
				(void) mpickobj(mtmp, otmp);
				(void) mongets(mtmp, JACKET, mkobjflags);
				(void) mongets(mtmp, GLOVES, mkobjflags);
				(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
				(void) mongets(mtmp, ANDROID_VISOR, mkobjflags);
			} else if(mm == PM_PARASITIZED_GYNOID){
				switch(rn2(9)){
					case 0:
					(void) mongets(mtmp, FORCE_PIKE, mkobjflags);
					(void) mongets(mtmp, VIBROBLADE, mkobjflags);
					break;
					case 1:
					otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
					otmp->objsize = MZ_LARGE;
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(SHORT_SWORD, mkobjflags);
					otmp->objsize = MZ_LARGE;
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 2:
					otmp = mksobj(SPEAR, mkobjflags);
					otmp->objsize = MZ_LARGE;
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(SHORT_SWORD, mkobjflags);
					otmp->objsize = MZ_LARGE;
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 3:
					otmp = mksobj(BATTLE_AXE, mkobjflags);
					otmp->objsize = MZ_LARGE;
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(RAPIER, mkobjflags);
					add_oprop(otmp, OPROP_LESSER_FLAYW);
					(void) mpickobj(mtmp, otmp);
					break;
					case 4:
					otmp = mksobj(SPEAR, mkobjflags);
					set_material_gm(otmp, METAL);
					add_oprop(otmp, OPROP_LESSER_ELECW);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(SABER, mkobjflags);
					set_material_gm(otmp, METAL);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 5:
					otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
					set_material_gm(otmp, METAL);
					add_oprop(otmp, OPROP_LESSER_ELECW);
					otmp->objsize = MZ_LARGE;
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(SABER, mkobjflags);
					set_material_gm(otmp, METAL);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 6:
					(void) mongets(mtmp, DOUBLE_FORCE_BLADE, mkobjflags);
					(void) mongets(mtmp, VIBROBLADE, mkobjflags);
					break;
					case 7:
					(void) mongets(mtmp, FORCE_PIKE, mkobjflags);
					(void) mongets(mtmp, rn2(2) ? FORCE_WHIP : FORCE_SWORD, mkobjflags);
					break;
					case 8:
					otmp = mongets(mtmp, KATANA, mkobjflags);
					if(otmp){
						set_material_gm(otmp, METAL);
						add_oprop(otmp, OPROP_LESSER_ELECW);
						fix_object(otmp);
					}
					otmp = mongets(mtmp, VIBROBLADE, mkobjflags);
					if(otmp){
						otmp->objsize = MZ_HUGE;
						fix_object(otmp);
					}
					break;
				}
				switch(rn2(9)){
					case 0:
					(void) mongets(mtmp, PLAIN_DRESS, mkobjflags);
					(void) mongets(mtmp, LONG_GLOVES, mkobjflags);
					mongets(mtmp, rn2(2) ? HEELED_BOOTS : rn2(2) ? HIGH_BOOTS : STILETTOS, mkobjflags);
					break;
					case 1:
					otmp = mksobj(NOBLE_S_DRESS, mkobjflags);
					set_material_gm(otmp, PLASTIC);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					(void) mongets(mtmp, PLASTEEL_GAUNTLETS, mkobjflags);
					rn2(2) ? mongets(mtmp, HEELED_BOOTS, mkobjflags) : mongets(mtmp, HIGH_BOOTS, mkobjflags);
					break;
					case 2:
					(void) mongets(mtmp, GENTLEWOMAN_S_DRESS, mkobjflags);
					(void) mongets(mtmp, VICTORIAN_UNDERWEAR, mkobjflags);
					(void) mongets(mtmp, LONG_GLOVES, mkobjflags);
					mongets(mtmp, rn2(2) ? HEELED_BOOTS : STILETTOS, mkobjflags);
					break;
					case 3:
					(void) mongets(mtmp, BODYGLOVE, mkobjflags);
					break;
					case 4:
					(void) mongets(mtmp, JUMPSUIT, mkobjflags);
					mongets(mtmp, rn2(2) ? LOW_BOOTS : rn2(2) ? HIGH_BOOTS : HEELED_BOOTS, mkobjflags);
					break;
					case 5:
					// (void) mongets(mtmp, PLASTEEL_HELM, mkobjflags); //Broken
					(void) mongets(mtmp, PLASTEEL_ARMOR, mkobjflags);
					(void) mongets(mtmp, PLASTEEL_BOOTS, mkobjflags);
					(void) mongets(mtmp, PLASTEEL_GAUNTLETS, mkobjflags);
					break;
					case 6:
					(void) mongets(mtmp, PLAIN_DRESS, mkobjflags);
					(void) mongets(mtmp, ALCHEMY_SMOCK, mkobjflags);
					(void) mongets(mtmp, LONG_GLOVES, mkobjflags);
					mongets(mtmp, rn2(2) ? HEELED_BOOTS : STILETTOS, mkobjflags);
					break;
					case 7:
					(void) mongets(mtmp, HEALER_UNIFORM, mkobjflags);
					(void) mongets(mtmp, LONG_GLOVES, mkobjflags);
					mongets(mtmp, rn2(2) ? LOW_BOOTS : rn2(2) ? HIGH_BOOTS : HEELED_BOOTS, mkobjflags);
					break;
					case 8:
					(void) mongets(mtmp, rn2(2) ? HAWAIIAN_SHIRT : T_SHIRT, mkobjflags);
					(void) mongets(mtmp, rn2(3) ? HAWAIIAN_SHORTS : WAISTCLOTH, mkobjflags);
					if(rn2(2)) mongets(mtmp, LOW_BOOTS, mkobjflags);
					break;
				}
			} else if(mm == PM_PARASITIZED_ANDROID){
				switch(rn2(6)){
					case 0:
					(void) mongets(mtmp, VIBROBLADE, mkobjflags);
					break;
					case 1:
					otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
					otmp->objsize = MZ_LARGE;
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 2:
					otmp = mksobj(SPEAR, mkobjflags);
					otmp->objsize = MZ_LARGE;
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 3:
					(void) mongets(mtmp, FORCE_PIKE, mkobjflags);
					break;
					case 4:
					otmp = mksobj(SPEAR, mkobjflags);
					set_material_gm(otmp, METAL);
					add_oprop(otmp, OPROP_LESSER_ELECW);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 5:
					(void) mongets(mtmp, DOUBLE_FORCE_BLADE, mkobjflags);
					break;
				}
				switch(rn2(5)){
					case 0:
					(void) mongets(mtmp, JACKET, mkobjflags);
					(void) mongets(mtmp, GLOVES, mkobjflags);
					(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
					break;
					case 1:
					(void) mongets(mtmp, GENTLEMAN_S_SUIT, mkobjflags);
					(void) mongets(mtmp, GLOVES, mkobjflags);
					(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
					break;
					case 2:
					(void) mongets(mtmp, BODYGLOVE, mkobjflags);
					break;
					case 3:
					(void) mongets(mtmp, JUMPSUIT, mkobjflags);
					break;
					case 4:
					(void) mongets(mtmp, rn2(2) ? HAWAIIAN_SHIRT : T_SHIRT, mkobjflags);
					(void) mongets(mtmp, rn2(3) ? HAWAIIAN_SHORTS : WAISTCLOTH, mkobjflags);
					if(rn2(2)) mongets(mtmp, LOW_BOOTS, mkobjflags);
					break;
				}
			} else if(mm == PM_PARASITIZED_OPERATOR || mm == PM_OPERATOR){
				switch(rn2(4)){
					case 0:
					otmp = mksobj(FORCE_PIKE, mkobjflags);
					set_material_gm(otmp, METAL);
					add_oprop(otmp, OPROP_LESSER_ELECW);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 1:
					otmp = mksobj(LONG_SWORD, mkobjflags);
					set_material_gm(otmp, METAL);
					add_oprop(otmp, OPROP_ELECW);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 2:
					otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
					set_material_gm(otmp, METAL);
					add_oprop(otmp, OPROP_ELECW);
					otmp->objsize = MZ_LARGE;
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
					case 3:
					otmp = mksobj(NAGINATA, mkobjflags);
					set_material_gm(otmp, METAL);
					add_oprop(otmp, OPROP_FLAYW);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					break;
				}
				switch(rn2(6)){
					case 0:
					case 1:
					(void) mongets(mtmp, rn2(2) ? PLAIN_DRESS : JACKET, mkobjflags);
					(void) mongets(mtmp, rn2(2) ? GLOVES : LONG_GLOVES, mkobjflags);
					(void) mongets(mtmp, rn2(2) ? HEELED_BOOTS : rn2(2) ? HIGH_BOOTS : STILETTOS, mkobjflags);
					break;
					case 2:
					case 3:
					(void) mongets(mtmp, rn2(2) ? JUMPSUIT : BODYGLOVE, mkobjflags);
					break;
					case 4:
					(void) mongets(mtmp, PLAIN_DRESS, mkobjflags);
					(void) mongets(mtmp, ALCHEMY_SMOCK, mkobjflags);
					(void) mongets(mtmp, rn2(2) ? GLOVES : LONG_GLOVES, mkobjflags);
					(void) mongets(mtmp, rn2(2) ? HEELED_BOOTS : STILETTOS, mkobjflags);
					break;
					case 5:
					(void) mongets(mtmp, HEALER_UNIFORM, mkobjflags);
					(void) mongets(mtmp, rn2(2) ? GLOVES : LONG_GLOVES, mkobjflags);
					(void) mongets(mtmp, rn2(2) ? LOW_BOOTS : rn2(2) ? HIGH_BOOTS : HEELED_BOOTS, mkobjflags);
					break;
				}
			} else if(mm == PM_COMMANDER){
				struct obj *otmp;
				otmp = mksobj(PISTOL, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, SILVER);
				add_oprop(otmp, OPROP_HOLYW);
				otmp->spe = 3;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				m_initthrow(mtmp, SILVER_BULLET, d(20,20), mkobjflags);
				
				otmp = mksobj(VIBROBLADE, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, SILVER);
				otmp->objsize = MZ_SMALL;
				add_oprop(otmp, OPROP_HOLYW);
				otmp->ovar1 = 80 + d(2,10);
				otmp->spe = 3;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(GENTLEWOMAN_S_DRESS, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, SILVER);
				add_oprop(otmp, OPROP_HOLY);
				add_oprop(otmp, OPROP_AXIO);
				add_oprop(otmp, OPROP_REFL);
				otmp->spe = 3;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(rn2(2) ? HEELED_BOOTS : STILETTOS, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, SILVER);
				add_oprop(otmp, OPROP_HOLY);
				add_oprop(otmp, OPROP_HOLYW);
				add_oprop(otmp, OPROP_AXIO);
				add_oprop(otmp, OPROP_AXIOW);
				add_oprop(otmp, OPROP_REFL);
				otmp->spe = 3;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(LONG_GLOVES, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, SILVER);
				add_oprop(otmp, OPROP_HOLY);
				add_oprop(otmp, OPROP_HOLYW);
				add_oprop(otmp, OPROP_AXIO);
				add_oprop(otmp, OPROP_AXIOW);
				add_oprop(otmp, OPROP_REFL);
				otmp->spe = 3;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			}
		}

		break;

		case S_HUMANOID:
		if (mm == PM_HOBBIT) {
			if(In_mordor_quest(&u.uz) 
				&& !In_mordor_forest(&u.uz)
				&& !Is_ford_level(&u.uz)
				&& !In_mordor_fields(&u.uz)
				&& in_mklev
			){
				(void)mongets(mtmp, SHACKLES, mkobjflags);
				mtmp->entangled = SHACKLES;
				return;
			}
		    switch (rn2(3)) {
			case 0:
			    (void)mongets(mtmp, DAGGER, mkobjflags);
			    break;
			case 1:
			    (void)mongets(mtmp, ELVEN_DAGGER, mkobjflags);
			    break;
			case 2:
			    (void)mongets(mtmp, SLING, mkobjflags);
			    m_initthrow(mtmp, ROCK, 6, mkobjflags);
			    break;
		      }
		    if (!rn2(10)) (void)mongets(mtmp, ELVEN_MITHRIL_COAT, mkobjflags);
		    if (!rn2(10)) (void)mongets(mtmp, DWARVISH_CLOAK, mkobjflags);
		} else if(mm == PM_CHANGED) {
			if(!rn2(10)){
				otmp = mksobj(HAND_BLASTER, mkobjflags);
				set_material_gm(otmp, BONE);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);

				otmp = mksobj(STILETTO, mkobjflags);
				set_material_gm(otmp, BONE);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			} else if(!rn2(9)){
				otmp = mksobj(PISTOL, mkobjflags);
				set_material_gm(otmp, BONE);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(BULLET, mkobjflags);
				set_material_gm(otmp, BONE);
				otmp->quan += rn1(20,20);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);

				otmp = mksobj(STILETTO, mkobjflags);
				set_material_gm(otmp, BONE);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			} else if(!rn2(8)){
				otmp = mksobj(FORCE_SWORD, mkobjflags);
				set_material_gm(otmp, BONE);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			} else if(!rn2(7)){
				otmp = mksobj(FORCE_WHIP, mkobjflags);
				set_material_gm(otmp, BONE);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			} else {
				otmp = mksobj(STILETTO, mkobjflags);
				set_material_gm(otmp, BONE);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			}
		} else if(mm == PM_CLAIRVOYANT_CHANGED) {
			struct obj *gem;
			otmp = mksobj(LIGHTSABER, mkobjflags);
			set_material_gm(otmp, BONE);
			fix_object(otmp);
			otmp->spe = 4;
			otmp->ovar1 = 39L;
			gem = otmp->cobj;
			obj_extract_self(gem);
			gem = poly_obj(gem,STAR_SAPPHIRE);
			add_to_container(otmp, gem);
			add_oprop(otmp, OPROP_PSIOW);
			add_oprop(otmp, OPROP_LESSER_MAGCW);
			add_oprop(otmp, OPROP_LIVEW);
			otmp->blessed = TRUE;
			otmp->cursed = FALSE;
			(void) mpickobj(mtmp, otmp);
		} else if(mm == PM_SMALL_GOAT_SPAWN) {
			int threshold = rnd(10)+rn2(11);
			if(mtmp->female && (In_lost_cities(&u.uz)) && u.uinsight > threshold){
				set_template(mtmp, MISTWEAVER);
				mtmp->m_insight_level = threshold;
			}
		} else if(mm == PM_GNOLL) {
			switch(rnd(4)){
			case 1:
			(void)mongets(mtmp, FLAIL, mkobjflags);
			if(rn2(2)) (void)mongets(mtmp, KNIFE, mkobjflags);
			break;
			case 2:
			(void)mongets(mtmp, BOW, mkobjflags);
			m_initthrow(mtmp, ARROW, d(8,4), mkobjflags);
			break;
			case 3:
			(void)mongets(mtmp, BATTLE_AXE, mkobjflags);
			break;
			case 4:
			(void)mongets(mtmp, SPEAR, mkobjflags);
			(void)mongets(mtmp, SPEAR, mkobjflags);
			(void)mongets(mtmp, SPEAR, mkobjflags);
			break;
			}
			(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
			if(!rn2(2))(void)mongets(mtmp, GLOVES, mkobjflags);
			if(!rn2(4))(void)mongets(mtmp, CLOAK, mkobjflags);
		} else if(mm == PM_DEEP_ONE || mm == PM_DEEPER_ONE) {
		 if(Infuture){
			if(mm == PM_DEEPER_ONE){
				switch (rn2(3)) {
					case 0:
						(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					break;
					case 1:
						(void)mongets(mtmp, STUDDED_LEATHER_ARMOR, mkobjflags);
					break;
					case 2:
						(void)mongets(mtmp, PLASTEEL_ARMOR, mkobjflags);
					break;
				}
				(void)mongets(mtmp, FLACK_HELMET, mkobjflags);
				(void)mongets(mtmp, GLOVES, mkobjflags);
				switch (rn2(6)) {
					case 0:
						otmp = mksobj(ASSAULT_RIFLE, mkobjflags);
						otmp->oeroded = 1;
						otmp->spe = rnd(4);
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						
						otmp = mksobj(STILETTO, mkobjflags);
						set_material_gm(otmp, MINERAL);
						if(!rn2(2))
							add_oprop(otmp, OPROP_LESSER_PSIOW);
						if(!rn2(20))
							add_oprop(otmp, rn2(2) ? OPROP_LESSER_WATRW : OPROP_PHSEW);
						(void) mpickobj(mtmp, otmp);
					break;
					case 1:
						otmp = mksobj(PISTOL, mkobjflags);
						otmp->oeroded = 1;
						otmp->spe = rnd(4);
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, BULLET, mkobjflags);
						
						otmp = mksobj(LONG_SWORD, mkobjflags);
						set_material_gm(otmp, MINERAL);
						if(!rn2(2))
							add_oprop(otmp, OPROP_LESSER_PSIOW);
						if(!rn2(20))
							add_oprop(otmp, rn2(2) ? OPROP_LESSER_WATRW : OPROP_PHSEW);
						(void) mpickobj(mtmp, otmp);
					break;
					case 2:
						otmp = mksobj(HEAVY_MACHINE_GUN, mkobjflags);
						otmp->oeroded = 1;
						otmp->spe = rnd(4);
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						
						otmp = mksobj(STILETTO, mkobjflags);
						set_material_gm(otmp, MINERAL);
						if(!rn2(2))
							add_oprop(otmp, OPROP_LESSER_PSIOW);
						if(!rn2(20))
							add_oprop(otmp, rn2(2) ? OPROP_LESSER_WATRW : OPROP_PHSEW);
						(void) mpickobj(mtmp, otmp);
					break;
					case 3:
						otmp = mksobj(SUBMACHINE_GUN, mkobjflags);
						otmp->oeroded = 1;
						otmp->spe = rnd(4);
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						
						otmp = mksobj(SHORT_SWORD, mkobjflags);
						set_material_gm(otmp, MINERAL);
						if(!rn2(2))
							add_oprop(otmp, OPROP_LESSER_PSIOW);
						if(!rn2(20))
							add_oprop(otmp, rn2(2) ? OPROP_LESSER_WATRW : OPROP_PHSEW);
						(void) mpickobj(mtmp, otmp);
					break;
					case 4:
						otmp = mksobj(SHOTGUN, mkobjflags);
						otmp->oeroded = 1;
						otmp->spe = rnd(4);
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, SHOTGUN_SHELL, mkobjflags);
						(void)mongets(mtmp, SHOTGUN_SHELL, mkobjflags);
						
						otmp = mksobj(SHORT_SWORD, mkobjflags);
						set_material_gm(otmp, MINERAL);
						if(!rn2(2))
							add_oprop(otmp, OPROP_LESSER_PSIOW);
						if(!rn2(20))
							add_oprop(otmp, rn2(2) ? OPROP_LESSER_WATRW : OPROP_PHSEW);
						(void) mpickobj(mtmp, otmp);
					break;
					case 5:
						otmp = mksobj(SNIPER_RIFLE, mkobjflags);
						otmp->oeroded = 1;
						otmp->spe = rnd(4);
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						
						otmp = mksobj(CLUB, mkobjflags);
						set_material_gm(otmp, MINERAL);
						otmp->spe = 3 + rnd(4);
						add_oprop(otmp, OPROP_LESSER_PSIOW);
						if(!rn2(10))
							add_oprop(otmp, OPROP_LESSER_WATRW);
						if(!rn2(10))
							add_oprop(otmp, OPROP_PHSEW);
						(void) mpickobj(mtmp, otmp);
					break;
				}
				if(!rn2(4)){
					if(rn2(3)) (void)mongets(mtmp, FRAG_GRENADE, mkobjflags);
					else if(rn2(2)) (void)mongets(mtmp, GAS_GRENADE, mkobjflags);
					else {
						(void)mongets(mtmp, ROCKET_LAUNCHER, mkobjflags);
						(void)mongets(mtmp, ROCKET, mkobjflags);
					}
				}
			} else {
				switch (rn2(3)) {
					case 0:
						(void)mongets(mtmp, JACKET, mkobjflags);
					break;
					case 1:
						(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					break;
					case 2:
						(void)mongets(mtmp, PLASTEEL_ARMOR, mkobjflags);
					break;
				}
				(void)mongets(mtmp, FLACK_HELMET, mkobjflags);
				(void)mongets(mtmp, GLOVES, mkobjflags);
				switch (rn2(6)) {
					case 0:
						otmp = mksobj(ASSAULT_RIFLE, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						
						otmp = mksobj(KNIFE, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
					break;
					case 1:
						otmp = mksobj(PISTOL, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, BULLET, mkobjflags);
						
						otmp = mksobj(SCIMITAR, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
					break;
					case 2:
						otmp = mksobj(HEAVY_MACHINE_GUN, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						
						otmp = mksobj(KNIFE, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
					break;
					case 3:
						otmp = mksobj(SUBMACHINE_GUN, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						
						otmp = mksobj(SHORT_SWORD, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
					break;
					case 4:
						otmp = mksobj(SHOTGUN, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, SHOTGUN_SHELL, mkobjflags);
						(void)mongets(mtmp, SHOTGUN_SHELL, mkobjflags);
						
						otmp = mksobj(KNIFE, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
					break;
					case 5:
						otmp = mksobj(SNIPER_RIFLE, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
						
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						(void)mongets(mtmp, BULLET, mkobjflags);
						
						otmp = mksobj(SHORT_SWORD, mkobjflags);
						otmp->oeroded = 3;
						(void) mpickobj(mtmp, otmp);
					break;
				}
				if(!rn2(6)){
					if(rn2(3)) (void)mongets(mtmp, FRAG_GRENADE, mkobjflags);
					else if(rn2(2)) (void)mongets(mtmp, GAS_GRENADE, mkobjflags);
					else {
						(void)mongets(mtmp, ROCKET_LAUNCHER, mkobjflags);
						(void)mongets(mtmp, ROCKET, mkobjflags);
					}
				}
			}
		 } else if(!on_level(&rlyeh_level,&u.uz)){
		    switch (rn2(3)) {
				case 0:
					(void)mongets(mtmp, JACKET, mkobjflags);
				break;
				case 1:
					(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				break;
				case 2:
					otmp = mksobj(CHAIN_MAIL, mkobjflags);
					if (is_rustprone(otmp))
						otmp->oeroded = 2;
					(void) mpickobj(mtmp, otmp);
				break;
			}
			chance = d(4, 2) / 3;
			while (chance > 0){
				switch (rn2(6)) {
				case 0:
					otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
					if (is_rustprone(otmp))
						otmp->oeroded = 3;
					(void)mpickobj(mtmp, otmp);
				break;
				case 1:
					otmp = mksobj(SCIMITAR, mkobjflags);
					if (is_rustprone(otmp))
						otmp->oeroded = 3;
					(void)mpickobj(mtmp, otmp);
				break;
				case 2:
					otmp = mksobj(TRIDENT, mkobjflags);
					if (is_rustprone(otmp))
						otmp->oeroded = 3;
					(void)mpickobj(mtmp, otmp);
				break;
				case 3:
					otmp = mksobj(SHORT_SWORD, mkobjflags);
					if (is_rustprone(otmp))
						otmp->oeroded = 3;
					(void)mpickobj(mtmp, otmp);
				break;
				case 4:
					otmp = mksobj(DAGGER, mkobjflags);
					if (is_rustprone(otmp))
						otmp->oeroded = 3;
					(void)mpickobj(mtmp, otmp);
				break;
				case 5:
					otmp = mksobj(SPEAR, mkobjflags);
					if (is_rustprone(otmp))
						otmp->oeroded = 3;
					(void)mpickobj(mtmp, otmp);
				break;
				}
				chance--;
			}
		 }else{
			otmp = mksobj(TRIDENT, mkobjflags);
			otmp->oerodeproof = 1;
			otmp->spe = mm==PM_DEEPER_ONE ? 6 : 3;
			otmp->blessed = FALSE;
			otmp->cursed = FALSE;
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(RANSEUR, mkobjflags);
			otmp->oerodeproof = 1;
			otmp->spe = mm==PM_DEEPER_ONE ? 6 : 3;
			otmp->blessed = FALSE;
			otmp->cursed = FALSE;
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(JAVELIN, mkobjflags);
			otmp->oeroded = 3;
			otmp->quan = mm==PM_DEEPER_ONE ? 6 : 3;
			otmp->owt = weight(otmp);
			otmp->spe = mm==PM_DEEPER_ONE ? 6 : 3;
			otmp->blessed = FALSE;
			otmp->cursed = FALSE;
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(ARCHAIC_PLATE_MAIL, mkobjflags);
			otmp->spe = mm==PM_DEEPER_ONE ? 6 : 3;
			otmp->blessed = FALSE;
			otmp->cursed = TRUE;
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(OILSKIN_CLOAK, mkobjflags);
			otmp->spe = mm==PM_DEEPER_ONE ? 6 : 3;
			otmp->blessed = FALSE;
			otmp->cursed = FALSE;
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(ORCISH_SHIELD, mkobjflags);
			otmp->oeroded = 3;
			otmp->spe = mm==PM_DEEPER_ONE ? 6 : 3;
			otmp->blessed = FALSE;
			otmp->cursed = FALSE;
			(void) mpickobj(mtmp, otmp);
			
			(void) mongets(mtmp, rnd_attack_potion(mtmp), mkobjflags);
			(void) mongets(mtmp, rnd_attack_potion(mtmp), mkobjflags);
			if(mm == PM_DEEPER_ONE){
				//Note: may be 0 if zombie
				int itm = rnd_attack_wand(mtmp);
				if(itm){
					otmp = mksobj(itm, mkobjflags);
					otmp->spe = 1;
					otmp->blessed = FALSE;
					otmp->cursed = FALSE;
					(void) mpickobj(mtmp, otmp);
				}
			}
		 }
		} else if((mm == PM_MIND_FLAYER || mm == PM_MASTER_MIND_FLAYER)){
			if(on_level(&rlyeh_level,&u.uz)){
				otmp = mksobj(BULLWHIP, mkobjflags);
				otmp->oerodeproof = 1;
				otmp->spe = mm==PM_MASTER_MIND_FLAYER ? 4 : 0;
				otmp->blessed = FALSE;
				otmp->cursed = 1;
				otmp->objsize = mm==PM_MASTER_MIND_FLAYER ? MZ_LARGE : MZ_MEDIUM;
				otmp->oerodeproof = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(SCALE_MAIL, mkobjflags);
				otmp->spe = mm==PM_MASTER_MIND_FLAYER ? 4 : 1;
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->objsize = mm==PM_MASTER_MIND_FLAYER ? MZ_LARGE : MZ_MEDIUM;
				otmp->oerodeproof = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(LEATHER_HELM, mkobjflags);
				otmp->spe = mm==PM_MASTER_MIND_FLAYER ? 4 : 1;
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->objsize = mm==PM_MASTER_MIND_FLAYER ? MZ_LARGE : MZ_MEDIUM;
				otmp->oerodeproof = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(GLOVES, mkobjflags);
				otmp->spe = mm==PM_MASTER_MIND_FLAYER ? 4 : 1;
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->objsize = mm==PM_MASTER_MIND_FLAYER ? MZ_LARGE : MZ_MEDIUM;
				otmp->oerodeproof = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(HIGH_BOOTS, mkobjflags);
				otmp->spe = mm==PM_MASTER_MIND_FLAYER ? 4 : 1;
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->objsize = mm==PM_MASTER_MIND_FLAYER ? MZ_LARGE : MZ_MEDIUM;
				otmp->oerodeproof = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(OILSKIN_CLOAK, mkobjflags);
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->objsize = mm==PM_MASTER_MIND_FLAYER ? MZ_LARGE : MZ_MEDIUM;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				(void) mongets(mtmp, rnd_attack_wand(mtmp), mkobjflags);
				(void) mongets(mtmp, rnd_utility_wand(mtmp), mkobjflags);
				
				(void) mongets(mtmp, rnd_utility_potion(mtmp), mkobjflags);
				(void) mongets(mtmp, rnd_utility_potion(mtmp), mkobjflags);
				
				(void) mongets(mtmp, rnd_attack_potion(mtmp), mkobjflags);
				(void) mongets(mtmp, rnd_attack_potion(mtmp), mkobjflags);
				(void) mongets(mtmp, rnd_attack_potion(mtmp), mkobjflags);
				
				if(mm == PM_MASTER_MIND_FLAYER || !rn2(3)) mongets(mtmp, R_LYEHIAN_FACEPLATE, mkobjflags);
				
				if(mm == PM_MASTER_MIND_FLAYER && !rn2(20))
					 mongets(mtmp, CRYSTAL_SKULL, mkobjflags);
				if(mm == PM_MIND_FLAYER && !rn2(60))
					 mongets(mtmp, CRYSTAL_SKULL, mkobjflags);
			} else if(Infuture){
				if(mm == PM_MASTER_MIND_FLAYER){
					struct obj *gem;
					give_mintrinsic(mtmp, POISON_RES);
					give_mintrinsic(mtmp, REGENERATION);
					mtmp->m_lev += 6;
					mtmp->mhpmax = mtmp->m_lev*8-1;
					mtmp->mhp = mtmp->mhpmax;
					otmp = mksobj(DOUBLE_LIGHTSABER, mkobjflags);
					otmp->oerodeproof = 1;
					otmp->spe = 4;
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					gem = otmp->cobj;
					obj_extract_self(gem);
					gem = poly_obj(gem,!rn2(3) ? RUBY : rn2(2) ? GREEN_FLUORITE : WORTHLESS_PIECE_OF_RED_GLASS);
					add_to_container(otmp, gem);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ROBE, mkobjflags);
					otmp->spe = 4;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->objsize = MZ_LARGE;
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(SCALE_MAIL, mkobjflags);
					set_material_gm(otmp, OBSIDIAN_MT);
					otmp->spe = 4;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->objsize = MZ_LARGE;
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(GAUNTLETS, mkobjflags);
					set_material_gm(otmp, OBSIDIAN_MT);
					otmp->spe = 4;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->objsize = MZ_LARGE;
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ARMORED_BOOTS, mkobjflags);
					set_material_gm(otmp, OBSIDIAN_MT);
					otmp->spe = 4;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					otmp->objsize = MZ_LARGE;
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					
					mongets(mtmp, R_LYEHIAN_FACEPLATE, mkobjflags);
					if(!rn2(3))
						mongets(mtmp, CRYSTAL_SKULL, mkobjflags);
				} else {
					struct obj *gem;
					give_mintrinsic(mtmp, POISON_RES);
					mtmp->m_lev += 2;
					mtmp->mhpmax = mtmp->m_lev*8-1;
					mtmp->mhp = mtmp->mhpmax;
					otmp = mksobj(LIGHTSABER, mkobjflags);
					otmp->oerodeproof = 1;
					otmp->spe = 1;
					otmp->ovar1 = !rn2(4) ? 6L : !rn2(3) ? 10L : rn2(2) ? 35L : 37L;
					otmp->blessed = TRUE;
					otmp->cursed = FALSE;
					gem = otmp->cobj;
					obj_extract_self(gem);
					gem = poly_obj(gem,!rn2(3) ? RUBY : rn2(2) ? GREEN_FLUORITE : WORTHLESS_PIECE_OF_RED_GLASS);
					add_to_container(otmp, gem);
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(ROBE, mkobjflags);
					otmp->spe = 1;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					(void) mpickobj(mtmp, otmp);
					
					otmp = mksobj(HIGH_BOOTS, mkobjflags);
					otmp->spe = 1;
					otmp->blessed = FALSE;
					otmp->cursed = TRUE;
					(void) mpickobj(mtmp, otmp);
					if(!rn2(30))
						mongets(mtmp, CRYSTAL_SKULL, mkobjflags);
				}
			} else {
				mongets(mtmp, QUARTERSTAFF, mkobjflags);
				mongets(mtmp, CLOAK, mkobjflags);
				mongets(mtmp, GLOVES, mkobjflags);
				mongets(mtmp, HIGH_BOOTS, mkobjflags);
				mongets(mtmp, LEATHER_HELM, mkobjflags);
				if(mm == PM_MASTER_MIND_FLAYER && !rn2(3)) mongets(mtmp, R_LYEHIAN_FACEPLATE, mkobjflags);
				else if(mm == PM_MIND_FLAYER && !rn2(20)) mongets(mtmp, R_LYEHIAN_FACEPLATE, mkobjflags);
				if(mm == PM_MASTER_MIND_FLAYER && !rn2(90)) mongets(mtmp, CRYSTAL_SKULL, mkobjflags);
				else if(mm == PM_MIND_FLAYER && !rn2(120)) mongets(mtmp, CRYSTAL_SKULL, mkobjflags);
			}
		} else if(mm == PM_GITHYANKI_PIRATE){
			if(!rn2(40)){
				otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
				set_material_gm(otmp, SILVER);
				add_oprop(otmp, OPROP_PSIOW);
				add_oprop(otmp, OPROP_VORPW);
				add_oprop(otmp, OPROP_GSSDW);
				otmp->spe = 5;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(ARCHAIC_HELM, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 3;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(ARCHAIC_PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 3;
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(ARCHAIC_GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 3;
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(ARCHAIC_BOOTS, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 3;
				(void) mpickobj(mtmp, otmp);
			} else {
				otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
				set_material_gm(otmp, SILVER);
				add_oprop(otmp, OPROP_PSIOW);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				(void)mongets(mtmp, ARCHAIC_HELM, mkobjflags);
				(void)mongets(mtmp, ARCHAIC_PLATE_MAIL, mkobjflags);
				(void)mongets(mtmp, ARCHAIC_GAUNTLETS, mkobjflags);
				(void)mongets(mtmp, ARCHAIC_BOOTS, mkobjflags);
			}
		} else if (ptr->mtyp == PM_ELOCATOR) {
		    otmp = mongets(mtmp, rn2(11) ? ROBE : CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
			if(otmp) otmp->obj_color = CLR_ORANGE;
		} else if (is_dwarf(ptr)) { //slightly rearanged code so more dwarves get helms -D_E
			if(In_mordor_quest(&u.uz) 
				&& !In_mordor_forest(&u.uz)
				&& !Is_ford_level(&u.uz)
				&& !In_mordor_fields(&u.uz)
				&& in_mklev
			){
				(void)mongets(mtmp, SHACKLES, mkobjflags);
				mtmp->entangled = SHACKLES;
				return;
			}
			if(Infuture){
				if(in_mklev){
					mtmp->m_lev += 20;
					mtmp->mhp = mtmp->mhpmax = mtmp->m_lev*8 - 1;
					otmp = mksobj(ARM_BLASTER, mkobjflags);
					if(otmp){
						otmp->spe = 0;
						otmp->ovar1 = d(5,10);
						otmp->recharged = 4;
						(void) mpickobj(mtmp, otmp);
					}
					
					otmp = mksobj(DISKOS, mkobjflags);
					if(otmp){
						otmp->spe = rn1(4,4);
						(void) mpickobj(mtmp, otmp);
					}

					otmp = mongets(mtmp, PISTOL, mkobjflags);
					if(otmp){
						otmp->spe = 7;
						add_oprop(otmp, OPROP_BLADED);
						otmp = mksobj(SILVER_BULLET, mkobjflags);
						if(otmp){
							otmp->quan += rn1(100,100);
							otmp->spe = 1+rn2(3);
							fix_object(otmp);
							(void) mpickobj(mtmp, otmp);
						}
					}

					otmp = mksobj(BUCKLER, mkobjflags);
					if(otmp){
						otmp->spe = rn1(2,2);
						set_material_gm(otmp, PLASTIC);
						otmp->obj_color = CLR_GRAY;
						(void) mpickobj(mtmp, otmp);
					}
					
					otmp = mongets(mtmp, DWARVISH_CLOAK, mkobjflags);
					if(otmp){
						otmp->spe = rn1(4,4);
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
					otmp = mongets(mtmp, PLASTEEL_HELM, mkobjflags);
					if(otmp){
						otmp->obj_color = CLR_GRAY;
						otmp->spe = rn1(4,4);
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
					otmp = mongets(mtmp, PLASTEEL_ARMOR, mkobjflags);
					if(otmp){
						otmp->obj_color = CLR_GRAY;
						otmp->spe = rn1(4,4);
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
					(void)mongets(mtmp, BODYGLOVE, mkobjflags);
					otmp = mongets(mtmp, PLASTEEL_GAUNTLETS, mkobjflags);
					if(otmp){
						otmp->obj_color = CLR_GRAY;
						otmp->spe = rn1(4,4);
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
					otmp = mongets(mtmp, PLASTEEL_BOOTS, mkobjflags);
					if(otmp){
						otmp->obj_color = CLR_GRAY;
						otmp->spe = rn1(4,4);
						otmp->cursed = FALSE; //Either uncurses or has no effect.
					}
				}
				else {
					(void)mongets(mtmp, SHOES, mkobjflags);
					(void)mongets(mtmp, DWARVISH_CLOAK, mkobjflags);
					if(is_prince(ptr) || (is_lord(ptr) && !rn2(3))){
						otmp = mongets(mtmp, RIFLE, mkobjflags);
						if(otmp)
							add_oprop(otmp, OPROP_BLADED);
						(void)mongets(mtmp, DWARVISH_MATTOCK, mkobjflags);
					}
					else if (!rn2(4)) {
						(void)mongets(mtmp, DWARVISH_SHORT_SWORD, mkobjflags);
					} else {
						if(!rn2(3)) (void)mongets(mtmp, DWARVISH_SPEAR, mkobjflags);
						else (void)mongets(mtmp, DAGGER, mkobjflags);
					}
					(void)mongets(mtmp, !rn2(3) ? PICK_AXE : AXE, mkobjflags);
					if(is_prince(ptr) || (is_lord(ptr) && !rn2(3))) (void)mongets(mtmp, DWARVISH_MITHRIL_COAT, mkobjflags);
					else (void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				}
			} else {
				if (goodequip || rn2(7)) (void)mongets(mtmp, DWARVISH_CLOAK, mkobjflags);
				if (goodequip || rn2(7)) (void)mongets(mtmp, SHOES, mkobjflags);
				if (!rn2(4)) {
					(void)mongets(mtmp, DWARVISH_SHORT_SWORD, mkobjflags);
				} else {
					if(!rn2(3)) (void)mongets(mtmp, DWARVISH_SPEAR, mkobjflags);
					else (void)mongets(mtmp, DAGGER, mkobjflags);
				}
				/* note: you can't use a mattock with a shield */
				if(!Is_minetown_level(&u.uz)){
					if (!rn2(3)) (void)mongets(mtmp, DWARVISH_MATTOCK, mkobjflags);
					else {
						(void)mongets(mtmp, !rn2(3) ? PICK_AXE : AXE, mkobjflags);
						if (!could_twoweap(ptr))
							(void)mongets(mtmp, DWARVISH_ROUNDSHIELD, mkobjflags);
						else
							mongets(mtmp, DWARVISH_SHORT_SWORD, mkobjflags);
					}
				}
				if (In_mines_quest(&u.uz) && !Is_minetown_level(&u.uz)) {
				/* MRKR: Dwarves in dark mines have their lamps on. */
					otmp = mksobj(DWARVISH_HELM, mkobjflags);
					(void) mpickobj(mtmp, otmp);
						if (!levl[mtmp->mx][mtmp->my].lit) {
						begin_burn(otmp);
					}
				}
				else {
					(void)mongets(mtmp, DWARVISH_HELM, mkobjflags);
					/* CM: Dwarves OUTSIDE the mines have booze. */
					mongets(mtmp, POT_BOOZE, mkobjflags);
				}
				if (goodequip || !rn2(3)){
					if(is_prince(ptr) || (is_lord(ptr) && !rn2(3))) (void)mongets(mtmp, DWARVISH_MITHRIL_COAT, mkobjflags);
					else (void)mongets(mtmp, CHAIN_MAIL, mkobjflags);
				}
			}
		}
		break;
	    case S_KETER:
			if (mm == PM_CHOKHMAH_SEPHIRAH){
				otmp = mksobj(LONG_SWORD,
						  mkobjflags|(rn2(13) ? NO_MKOBJ_FLAGS : MKOBJ_ARTIF));
				otmp->spe = 7;
				otmp->oerodeproof = 1;
				(void) mpickobj(mtmp, otmp);
			}
		break;
	    case S_ORC:
		if(goodequip || rn2(2)) (void)mongets(mtmp, ORCISH_HELM, mkobjflags);
		switch (mm) {
		    case PM_ORC_OF_THE_AGES_OF_STARS:
				otmp = mongets(mtmp, HIGH_ELVEN_WARSWORD, mkobjflags);
				if(otmp) MAYBE_MERC(otmp)
				(void)mongets(mtmp, HIGH_ELVEN_WARSWORD, mkobjflags);
				(void)mongets(mtmp, HIGH_ELVEN_HELM, mkobjflags);
				(void)mongets(mtmp, HIGH_ELVEN_PLATE, mkobjflags);
				(void)mongets(mtmp, ORCISH_CLOAK, mkobjflags);
				(void)mongets(mtmp, HIGH_ELVEN_GAUNTLETS, mkobjflags);
				(void)mongets(mtmp, ELVEN_BOOTS, mkobjflags);
			    (void)mongets(mtmp, DRUM, mkobjflags);
			break;
		    case PM_ANGBAND_ORC:
				if(In_mordor_quest(&u.uz)){
					otmp = mksobj(STILETTO, mkobjflags);
					add_oprop(otmp, OPROP_MORGW);
					otmp->opoisoned = OPOISON_BASIC;
					set_material_gm(otmp, METAL);
					fix_object(otmp);
					curse(otmp);
					(void) mpickobj(mtmp, otmp);
					
					(void)mongets(mtmp, KNIFE, mkobjflags);
					(void)mongets(mtmp, WHITE_DRAGON_SCALES, mkobjflags);
					(void)mongets(mtmp, ORCISH_HELM, mkobjflags);
					(void)mongets(mtmp, ORCISH_CLOAK, mkobjflags);
					(void)mongets(mtmp, ORCISH_SHIELD, mkobjflags);
					(void)mongets(mtmp, GLOVES, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					if(!rn2(3)) mongets(mtmp, WAN_MAGIC_MISSILE, mkobjflags);
				} else {
					(void)mongets(mtmp, SCIMITAR, mkobjflags);
					if(rn2(2))
						(void)mongets(mtmp, SCIMITAR, mkobjflags);
					else
						(void)mongets(mtmp, ORCISH_SHIELD, mkobjflags);
					(void)mongets(mtmp, KNIFE, mkobjflags);
					(void)mongets(mtmp, ORCISH_HELM, mkobjflags);
					(void)mongets(mtmp, ORCISH_CHAIN_MAIL, mkobjflags);
					(void)mongets(mtmp, ORCISH_CLOAK, mkobjflags);
					(void)mongets(mtmp, GLOVES, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
				}
			break;
		    case PM_MORDOR_MARSHAL:
		    case PM_ORC_CAPTAIN:
				if(In_mordor_quest(&u.uz)){
					(void)mongets(mtmp, SCIMITAR, mkobjflags);
					(void)mongets(mtmp, GREEN_DRAGON_SCALES, mkobjflags);
					(void)mongets(mtmp, ORCISH_HELM, mkobjflags);
					if(rn2(2)) (void)mongets(mtmp, ORCISH_CLOAK, mkobjflags);
					(void)mongets(mtmp, ORCISH_SHIELD, mkobjflags);
					(void)mongets(mtmp, GLOVES, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					break;
				}
				//else fall through
		    case PM_MORDOR_ORC_ELITE:
				if(In_mordor_quest(&u.uz)){
					(void)mongets(mtmp, SCIMITAR, mkobjflags);
					(void)mongets(mtmp, ORCISH_CHAIN_MAIL, mkobjflags);
					(void)mongets(mtmp, ORCISH_HELM, mkobjflags);
					if(rn2(2)) (void)mongets(mtmp, ORCISH_CLOAK, mkobjflags);
					(void)mongets(mtmp, ORCISH_SHIELD, mkobjflags);
					(void)mongets(mtmp, GLOVES, mkobjflags);
					(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
					break;
				}
				//else fall through
		    case PM_MORDOR_ORC:
			if(goodequip || !rn2(3)) (void)mongets(mtmp, SCIMITAR, mkobjflags);
			if(!rn2(3)) (void)mongets(mtmp, ORCISH_SHIELD, mkobjflags);
			else if (could_twoweap(ptr))
						(void)mongets(mtmp, SCIMITAR, mkobjflags);
			if(goodequip || !rn2(3)) (void)mongets(mtmp, KNIFE, mkobjflags);
			if(goodequip || !rn2(3)) (void)mongets(mtmp, ORCISH_CHAIN_MAIL, mkobjflags);
//ifdef BARD
			if (mm == PM_ORC_CAPTAIN ? !rn2(10) : !rn2(50)){
			    (void)mongets(mtmp, DRUM, mkobjflags);
				if(!rn2(3)) {
					(void)mongets(mtmp, ORCISH_BOW, mkobjflags);
					m_initthrow(mtmp, ORCISH_ARROW, 12, mkobjflags);
				}
			}
//endif
			break;
		    case PM_URUK_CAPTAIN:
		    case PM_URUK_HAI:
			if(goodequip || !rn2(3)) (void)mongets(mtmp, ORCISH_CLOAK, mkobjflags);
			if(goodequip || !rn2(3)) (void)mongets(mtmp, ORCISH_SHORT_SWORD, mkobjflags);
			if(goodequip || !rn2(3)) (void)mongets(mtmp, SHOES, mkobjflags);
			if(!rn2(3)) (void)mongets(mtmp, URUK_HAI_SHIELD, mkobjflags);
			else if (could_twoweap(ptr))
						(void)mongets(mtmp, ORCISH_SHORT_SWORD, mkobjflags);
//ifdef BARD
			if (mm == PM_URUK_CAPTAIN ? !rn2(10) : !rn2(50)){
			    (void)mongets(mtmp, DRUM, mkobjflags);
				(void)mongets(mtmp, CROSSBOW, mkobjflags);
				m_initthrow(mtmp, CROSSBOW_BOLT, 12, mkobjflags);
			} else {
				if(goodequip || !rn2(3)) {
					(void)mongets(mtmp, CROSSBOW, mkobjflags);
					m_initthrow(mtmp, CROSSBOW_BOLT, 12, mkobjflags);
				}
			}
//endif
			break;
		    default:
			if (mm != PM_ORC_SHAMAN && mm != PM_MORDOR_SHAMAN && rn2(2))
			  (void)mongets(mtmp, (mm == PM_GOBLIN || rn2(2) == 0)
						   ? ORCISH_DAGGER : SCIMITAR, mkobjflags);
		}
		break;
	    case S_OGRE:
			if(In_mordor_quest(&u.uz)){
				if(mm == PM_OGRE_KING){
					if(!rn2(4))
						(void) mongets(mtmp, BATTLE_AXE, mkobjflags);
					else 
						(void) mongets(mtmp, AXE, mkobjflags);
					(void) mongets(mtmp, ORCISH_CHAIN_MAIL, mkobjflags);
					if(rn2(2))
						(void) mongets(mtmp, ORCISH_HELM, mkobjflags);
					break;
				} else if(mm == PM_OGRE){
					if(!rn2(6))
						(void) mongets(mtmp, BATTLE_AXE, mkobjflags);
					else 
						(void) mongets(mtmp, AXE, mkobjflags);
					(void) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					if(rn2(2))
						(void) mongets(mtmp, ORCISH_HELM, mkobjflags);
					break;
				}
				// Siege Ogres etc. continue
			}
		if (mm == PM_OGRE_EMPEROR){
		    (void) mongets(mtmp, TSURUGI, mkobjflags);
			(void)mongets(mtmp, GAUNTLETS, mkobjflags);
			(void)mongets(mtmp, BANDED_MAIL, mkobjflags);
			(void)mongets(mtmp, WAR_HAT, mkobjflags);
			(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
		} else if (!rn2(mm == PM_OGRE_KING ? 3 : mm == PM_OGRE_LORD ? 6 : 12))
		    (void) mongets(mtmp, BATTLE_AXE, mkobjflags);
		else
		    (void) mongets(mtmp, CLUB, mkobjflags);
		if(mm == PM_SIEGE_OGRE){
		    struct obj *otmp = mksobj(ARROW, mkobjflags);
		    otmp->blessed = FALSE;
		    otmp->cursed = FALSE;
			otmp->quan = 240;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp,otmp);
		}
		break;
	    case S_TROLL:
		if (!rn2(2)) switch (rn2(4)) {
		    case 0: (void)mongets(mtmp, RANSEUR, mkobjflags); break;
		    case 1: (void)mongets(mtmp, PARTISAN, mkobjflags); break;
		    case 2: (void)mongets(mtmp, GLAIVE, mkobjflags); break;
		    case 3: (void)mongets(mtmp, SPETUM, mkobjflags); break;
		}
		break;
	    case S_KOBOLD:
		if(ptr->mtyp == PM_GREAT_HIGH_SHAMAN_OF_KURTULMAK){
			(void)mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
			(void)mongets(mtmp, SHORT_SWORD, mkobjflags);
			(void)mongets(mtmp, BUCKLER, mkobjflags);
			m_initthrow(mtmp, CROSSBOW_BOLT, 36, mkobjflags);
			m_initthrow(mtmp, DART, 36, mkobjflags);
		} else {
			if (!rn2(4)) m_initthrow(mtmp, DART, 12, mkobjflags);
			else if(!rn2(3)) (void)mongets(mtmp, SHORT_SWORD, mkobjflags);
			else if(!rn2(2)) (void)mongets(mtmp, CLUB, mkobjflags);
		}
		break;
	    case S_NYMPH:
			if(ptr->mtyp == PM_THRIAE || ptr->mtyp == PM_OCEANID || ptr->mtyp == PM_SELKIE){
				switch (rn2(6)) {
				/* MAJOR fall through ... */
				case 0: (void) mongets(mtmp, WAN_MAGIC_MISSILE, mkobjflags);
				case 1: (void) mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
				case 2: (void) mongets(mtmp, POT_HEALING, mkobjflags);
					break;
				case 3: (void) mongets(mtmp, WAN_STRIKING, mkobjflags);
				case 4: (void) mongets(mtmp, POT_HEALING, mkobjflags);
				case 5: (void) mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
				}
				if(ptr->mtyp == PM_OCEANID){
					(void)mongets(mtmp, POT_OBJECT_DETECTION, mkobjflags);
					(void)mongets(mtmp, MIRROR, mkobjflags);
				}
				if(ptr->mtyp == PM_THRIAE)
				(void)mongets(mtmp, (rn2(2) ? FLUTE : HARP), mkobjflags);
			} else if(ptr->mtyp == PM_INTONER){
				int intoner_color = HI_GLASS;
				switch(rn2(6)){
					case 0:
						intoner_color = CLR_GRAY;
						otmp = mksobj(LONG_SWORD, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						set_material_gm(otmp, DRAGON_HIDE);
						add_oprop(otmp, OPROP_AXIOW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(GRAY_DRAGON_SCALE_MAIL, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						set_material_gm(otmp, GLASS);
						add_oprop(otmp, OPROP_AXIO);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(LONG_GLOVES, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						otmp->obj_color = intoner_color;
						add_oprop(otmp, OPROP_AXIO);
						(void) mpickobj(mtmp, otmp);
					break;
					case 1:
						intoner_color = CLR_WHITE;
						otmp = mksobj(CHAKRAM, mkobjflags|MKOBJ_NOINIT);
						otmp->objsize = MZ_HUGE;
						otmp->spe = 4;
						set_material_gm(otmp, METAL);
						add_oprop(otmp, OPROP_AXIOW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(WHITE_DRAGON_SCALE_MAIL, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						set_material_gm(otmp, GLASS);
						add_oprop(otmp, OPROP_AXIO);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(LONG_GLOVES, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						otmp->obj_color = intoner_color;
						add_oprop(otmp, OPROP_AXIO);
						(void) mpickobj(mtmp, otmp);
					break;
					case 2:
						intoner_color = CLR_BLUE;
						otmp = mksobj(LONG_SWORD, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						set_material_gm(otmp, METAL);
						add_oprop(otmp, OPROP_AXIOW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(BLUE_DRAGON_SCALE_MAIL, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						set_material_gm(otmp, GLASS);
						add_oprop(otmp, OPROP_AXIO);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(LONG_GLOVES, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						otmp->obj_color = intoner_color;
						add_oprop(otmp, OPROP_AXIO);
						(void) mpickobj(mtmp, otmp);
					break;
					case 3:
						intoner_color = CLR_MAGENTA;
						otmp = mksobj(SCIMITAR, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						set_material_gm(otmp, METAL);
						add_oprop(otmp, OPROP_AXIOW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(SCIMITAR, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						set_material_gm(otmp, METAL);
						add_oprop(otmp, OPROP_AXIOW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(BLACK_DRAGON_SCALE_MAIL, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						set_material_gm(otmp, GLASS);
						add_oprop(otmp, OPROP_AXIO);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(LONG_GLOVES, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						otmp->obj_color = intoner_color;
						add_oprop(otmp, OPROP_AXIO);
						(void) mpickobj(mtmp, otmp);
					break;
					case 4:
						intoner_color = CLR_GREEN;
						otmp = mksobj(GREEN_DRAGON_SCALE_MAIL, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						set_material_gm(otmp, GLASS);
						add_oprop(otmp, OPROP_AXIO);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						add_oprop(otmp, OPROP_AXIO);
						add_oprop(otmp, OPROP_AXIOW);
						(void) mpickobj(mtmp, otmp);
					break;
					case 5:
						intoner_color = CLR_YELLOW;
						otmp = mksobj(TWO_HANDED_SWORD, mkobjflags|MKOBJ_NOINIT);
						otmp->objsize = MZ_LARGE;
						otmp->spe = 4;
						set_material_gm(otmp, METAL);
						add_oprop(otmp, OPROP_AXIOW);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(YELLOW_DRAGON_SCALE_MAIL, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						set_material_gm(otmp, GLASS);
						add_oprop(otmp, OPROP_AXIO);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(LONG_GLOVES, mkobjflags|MKOBJ_NOINIT);
						otmp->spe = 4;
						otmp->obj_color = intoner_color;
						add_oprop(otmp, OPROP_AXIO);
						(void) mpickobj(mtmp, otmp);
					break;
				}
				otmp = mksobj(HEELED_BOOTS, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 4;
				add_oprop(otmp, OPROP_AXIO);
				otmp->obj_color = intoner_color;
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(WAR_HAT, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 4;
				set_material_gm(otmp, GLASS);
				otmp->obj_color = intoner_color;
				add_oprop(otmp, OPROP_AXIO);
				add_oprop(otmp, OPROP_REFL);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			} else if(ptr->mtyp == PM_DEMINYMPH){
				if(faction == GOATMOM_FACTION){
					//Cultist of the Black Goat
					otmp = mksobj(VIPERWHIP, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = 3;
					otmp->ovar1 = 4;
					otmp->opoisoned = OPOISON_ACID;
					otmp->opoisonchrgs = 3;
					set_material_gm(otmp, BONE);
					(void) mpickobj(mtmp, otmp);
					int threshold = rnd(10)+rn2(11);
					if(mtmp->female && u.uinsight > threshold){
						set_template(mtmp, MISTWEAVER);
						mtmp->m_insight_level = threshold;
					} else {
						otmp = mksobj(WAR_HAT, mkobjflags);
						otmp->spe = 2;
						set_material_gm(otmp, BONE);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					}
					otmp = mksobj(PLATE_MAIL, mkobjflags);
					otmp->spe = 2;
					set_material_gm(otmp, BONE);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(GAUNTLETS, mkobjflags);
					otmp->spe = 2;
					set_material_gm(otmp, BONE);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(ARMORED_BOOTS, mkobjflags);
					otmp->spe = 2;
					set_material_gm(otmp, BONE);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(ROUNDSHIELD, mkobjflags);
					otmp->spe = 2;
					set_material_gm(otmp, BONE);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
				} else switch(rn2(16)){
					//Archeologist
					case 0:
						otmp = mksobj(BULLWHIP, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 2+rn2(3);
						MAYBE_MERC(otmp)
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(DWARVISH_MATTOCK, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(JACKET, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(FEDORA, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
					break;
					//Barbarian
					case 1:
						if(rn2(2)){
							otmp = mksobj(TWO_HANDED_SWORD, mkobjflags|MKOBJ_ARTIF);
							otmp->spe = 0+rnd(3)+rn2(3);
							MAYBE_MERC(otmp)
							(void) mpickobj(mtmp, otmp);
							otmp = mksobj(AXE, mkobjflags|MKOBJ_ARTIF);
							otmp->spe = 0+rn2(4);
							(void) mpickobj(mtmp, otmp);
						} else {
							otmp = mksobj(BATTLE_AXE, mkobjflags|MKOBJ_ARTIF);
							otmp->spe = 0+rnd(3)+rn2(3);
							MAYBE_MERC(otmp)
							(void) mpickobj(mtmp, otmp);
							otmp = mksobj(SHORT_SWORD, mkobjflags|MKOBJ_ARTIF);
							otmp->spe = 0+rn2(4);
							(void) mpickobj(mtmp, otmp);
						}
						otmp = mksobj(RING_MAIL, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
					break;
					//Bard
					case 2:
						otmp = mksobj(RAPIER, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						MAYBE_MERC(otmp)
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(CLOAK, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						mongets(mtmp, HARP, mkobjflags);
						mongets(mtmp, POT_BOOZE, mkobjflags);
						mongets(mtmp, POT_BOOZE, mkobjflags);
						mongets(mtmp, POT_BOOZE, mkobjflags);
					break;
					//Caveman
					case 3:
						otmp = mksobj(CLUB, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(LEATHER_ARMOR, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(SLING, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 2+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(FLINT, mkobjflags|MKOBJ_ARTIF);
						otmp->quan = rnd(5)+rnd(5)+rnd(5)+rnd(5)+rnd(5);
						otmp->owt = weight(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
					//Healer
					case 4:
						otmp = mksobj(SCALPEL, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(QUARTERSTAFF, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(GLOVES, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(HEALER_UNIFORM, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(LOW_BOOTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						mongets(mtmp, STETHOSCOPE, mkobjflags);
						mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
						mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
						mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
						mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
						mongets(mtmp, POT_HEALING, mkobjflags);
						mongets(mtmp, POT_HEALING, mkobjflags);
						mongets(mtmp, POT_HEALING, mkobjflags);
						mongets(mtmp, POT_HEALING, mkobjflags);
						mongets(mtmp, WAN_SLEEP, mkobjflags);
					break;
					//Knight
					case 5:
						otmp = mksobj(LONG_SWORD, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						MAYBE_MERC(otmp)
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(CHAIN_MAIL, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(HELMET, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(KITE_SHIELD, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(GLOVES, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
					break;
					//Monk
					case 6:
						otmp = mksobj(GLOVES, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 2+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(ROBE, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						otmp->obj_color = CLR_ORANGE;
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(SEDGE_HAT, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
						mongets(mtmp, POT_HEALING, mkobjflags);
						mongets(mtmp, POT_HEALING, mkobjflags);
						mongets(mtmp, POT_HEALING, mkobjflags);
						mongets(mtmp, WAN_SLEEP, mkobjflags);
					break;
					//"Madman"/Dreamlands noble
					case 7:
						switch(rnd(10)){
							case 1:
								//Yellow Sign Noble
								otmp = mksobj(RAPIER, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 2+rn2(3);
								set_material_gm(otmp, GOLD);
								(void) mpickobj(mtmp, otmp);
								otmp = mksobj(VICTORIAN_UNDERWEAR, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 0+rn2(4);
								otmp->obj_color = CLR_YELLOW;
								(void) mpickobj(mtmp, otmp);
								otmp = mksobj(GENTLEWOMAN_S_DRESS, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 2+rn2(3);
								otmp->obj_color = CLR_YELLOW;
								(void) mpickobj(mtmp, otmp);
								otmp = mksobj(STILETTOS, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 0+rn2(4);
								set_material_gm(otmp, GOLD);
								(void) mpickobj(mtmp, otmp);
								otmp = mksobj(CLOAK, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 1+rn2(3);
								otmp->obj_color = CLR_YELLOW;
								(void) mpickobj(mtmp, otmp);
								otmp = mksobj(GLOVES, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 1+rn2(3);
								otmp->obj_color = CLR_YELLOW;
								(void) mpickobj(mtmp, otmp);
								otmp = mksobj(BUCKLER, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 1+rn2(3);
								set_material_gm(otmp, GOLD);
								(void) mpickobj(mtmp, otmp);
								otmp = mksobj(find_gcirclet(), mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 1+rn2(3);
								set_material_gm(otmp, GOLD);
								(void) mpickobj(mtmp, otmp);
								set_template(mtmp, DREAM_LEECH);
							break;
							case 2:
								//Pseudonatural
								otmp = mksobj(rn2(3) ? PLAIN_DRESS : BODYGLOVE, mkobjflags|MKOBJ_ARTIF);
								set_material_gm(otmp, FLESH);
								otmp->obj_color = CLR_RED;
								fix_object(otmp);
								otmp->spe = 1+rn2(3);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_ARTIF);
								set_material_gm(otmp, BONE);
								fix_object(otmp);
								otmp->spe = 1+rn2(3);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(PLATE_MAIL, mkobjflags|MKOBJ_ARTIF);
								set_material_gm(otmp, BONE);
								fix_object(otmp);
								otmp->spe = 1+rn2(3);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(rn2(10) ? GAUNTLETS : GAUNTLETS_OF_POWER, mkobjflags|MKOBJ_ARTIF);
								set_material_gm(otmp, BONE);
								fix_object(otmp);
								otmp->spe = 1+rn2(3);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(FACELESS_HELM, mkobjflags|MKOBJ_ARTIF);
								set_material_gm(otmp, BONE);
								fix_object(otmp);
								otmp->spe = 1+rn2(3);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(rn2(10) ? STILETTO : TWO_HANDED_SWORD, mkobjflags|MKOBJ_ARTIF);
								if(!rn2(10))
									add_oprop(otmp, rn2(5) ? OPROP_PSECW : rn2(4) ? OPROP_ASECW : OPROP_LIVEW);
								set_material_gm(otmp, BONE);
								MAYBE_MERC(otmp)
								fix_object(otmp);
								otmp->spe = 1+rn2(3);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(PISTOL, mkobjflags|MKOBJ_ARTIF);
								set_material_gm(otmp, BONE);
								fix_object(otmp);
								otmp->spe = 1+rn2(3);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(BULLET, mkobjflags|MKOBJ_ARTIF);
								set_material_gm(otmp, BONE);
								otmp->quan += rn1(20,20);
								otmp->spe = 1+rn2(3);
								fix_object(otmp);
								(void) mpickobj(mtmp, otmp);
								
								set_template(mtmp, PSEUDONATURAL);
							break;
							case 3:
								//Mistweaver
								otmp = mksobj(VIPERWHIP, mkobjflags|MKOBJ_NOINIT);
								otmp->spe = 3;
								otmp->ovar1 = 4;
								otmp->opoisoned = OPOISON_ACID;
								otmp->opoisonchrgs = 3;
								set_material_gm(otmp, BONE);
								if(!rn2(20))
									add_oprop(otmp, OPROP_LIVEW);
								if(!rn2(10))
									add_oprop(otmp, OPROP_ASECW);
								if(!rn2(20))
									add_oprop(otmp, OPROP_GOATW);
								MAYBE_MERC(otmp)
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(GLOVES, mkobjflags|MKOBJ_ARTIF);
								if(!otmp->oartifact) set_material_gm(otmp, LEATHER);
								fix_object(otmp);
								otmp->spe = 1+rn2(3);
								otmp->obj_color = CLR_RED;
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(ROBE, mkobjflags|MKOBJ_ARTIF);
								if(!otmp->oartifact) set_material_gm(otmp, LEATHER);
								fix_object(otmp);
								otmp->spe = 1+rn2(3);
								otmp->obj_color = CLR_RED;
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(LEATHER_ARMOR, mkobjflags|MKOBJ_ARTIF);
								fix_object(otmp);
								otmp->spe = 1+rn2(3);
								otmp->obj_color = CLR_RED;
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(SHOES, mkobjflags|MKOBJ_ARTIF);
								if(!otmp->oartifact) set_material_gm(otmp, LEATHER);
								fix_object(otmp);
								otmp->spe = 1+rn2(3);
								otmp->obj_color = CLR_RED;
								(void) mpickobj(mtmp, otmp);

								set_template(mtmp, MISTWEAVER);
								set_faction(mtmp, GOATMOM_FACTION);
							break;
							case 4:{
								//Drow
								int house = !rn2(10) ? PEN_A_SYMBOL : !rn2(3) ? EILISTRAEE_SYMBOL : rn2(LAST_FALLEN_HOUSE+1-FIRST_FALLEN_HOUSE)+FIRST_FALLEN_HOUSE;
								int mat = !rn2(3) ? MITHRIL : rn2(2) ? SILVER : METAL;
								
								otmp = mksobj(rn2(2) ? NOBLE_S_DRESS : DROVEN_PLATE_MAIL, mkobjflags|MKOBJ_ARTIF);
								set_material_gm(otmp, mat);
								otmp->ohaluengr = TRUE;
								otmp->oward = house;
								otmp->spe = 1+rn2(3);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(find_signet_ring(), mkobjflags);
								otmp->ohaluengr = TRUE;
								otmp->oward = house;
								(void) mpickobj(mtmp, otmp);
								
								otmp = mksobj(DROVEN_CLOAK, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 1+rn2(3);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(DROVEN_HELM, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 1+rn2(3);
								set_material_gm(otmp, mat);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 1+rn2(3);
								set_material_gm(otmp, mat);
								(void) mpickobj(mtmp, otmp);

								otmp = mongets(mtmp, HIGH_BOOTS, mkobjflags|MKOBJ_ARTIF);
								if(otmp) otmp->spe = 1+rn2(3);

								otmp = mongets(mtmp, DROVEN_DAGGER, mkobjflags);
								if(otmp) otmp->spe = 1+rn2(3);
								
								if(rn2(4)){
									otmp = mksobj(VIPERWHIP, mkobjflags|MKOBJ_ARTIF);
									MAYBE_MERC(otmp)
									otmp->spe = 2+rn2(3);
									otmp->opoisoned = rn2(4) ? OPOISON_BASIC : OPOISON_PARAL;
									otmp->opoisonchrgs = 6;
									otmp->ovar1 = rnd(3)+rn2(3);
									(void) mpickobj(mtmp, otmp);

									otmp = mongets(mtmp, KITE_SHIELD, mkobjflags);
									if(otmp){
										otmp->spe = 1+rn2(3);
										set_material_gm(otmp, mat);
									}
								}
								else {
									otmp = mksobj(DROVEN_GREATSWORD, mkobjflags|MKOBJ_ARTIF);
									MAYBE_MERC(otmp)
									otmp->spe = 2+rn2(3);
									(void) mpickobj(mtmp, otmp);
								}
							}
							break;
							case 5:{
								//Hunter
								int weapon = rn2(3) ? RAKUYO : BLADE_OF_MERCY;
								if(weapon == BLADE_OF_MERCY){
									if(rn2(3)){
										otmp = mksobj(BLADE_OF_MERCY, mkobjflags|MKOBJ_ARTIF);
										otmp->spe = 5;
										(void) mpickobj(mtmp, otmp);
									}
									else {
										otmp = mksobj(BLADE_OF_PITY, mkobjflags|MKOBJ_ARTIF);
										otmp->spe = 5;
										(void) mpickobj(mtmp, otmp);
										otmp = mksobj(BLADE_OF_GRACE, mkobjflags|MKOBJ_ARTIF);
										otmp->spe = 5;
										(void) mpickobj(mtmp, otmp);
									}
								}
								else {
									if(rn2(3)){
										otmp = mksobj(RAKUYO, mkobjflags|MKOBJ_ARTIF);
										otmp->spe = 5;
										(void) mpickobj(mtmp, otmp);
									}
									else {
										otmp = mksobj(RAKUYO_DAGGER, mkobjflags|MKOBJ_ARTIF);
										otmp->spe = 5;
										(void) mpickobj(mtmp, otmp);
										otmp = mksobj(RAKUYO_SABER, mkobjflags|MKOBJ_ARTIF);
										otmp->spe = 5;
										(void) mpickobj(mtmp, otmp);
									}
								}

								otmp = mksobj(RUFFLED_SHIRT, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 0+rn2(4);
								(void) mpickobj(mtmp, otmp);
								
								otmp = mksobj(LEATHER_ARMOR, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 2+rn2(4);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(LEATHER_HELM, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 2+rn2(4);
								(void) mpickobj(mtmp, otmp);

								otmp = mksobj(CLOAK, mkobjflags|MKOBJ_ARTIF);
								otmp->spe = 1+rn2(3);
								(void) mpickobj(mtmp, otmp);

								otmp = mongets(mtmp, HIGH_BOOTS, mkobjflags|MKOBJ_ARTIF);
								if(otmp) otmp->spe = 1+rn2(3);

								otmp = mongets(mtmp, BUCKLER, mkobjflags|MKOBJ_ARTIF);
								if(otmp) otmp->spe = 1+rn2(3);
								
								if(weapon == BLADE_OF_MERCY){
									otmp = mongets(mtmp, MASK, mkobjflags|MKOBJ_ARTIF);
									if(otmp){
										otmp->corpsenm = PM_CROW;
										set_material_gm(otmp, METAL);
									}
								}
							}
							break;
							case 6:{
								//Club-claw
								long long oprop;
								switch(rnd(10)){
									case 1:
										oprop = OPROP_ELECW;
									break;
									case 2:
										oprop = OPROP_ACIDW;
									break;
									case 3:
										oprop = OPROP_MAGCW;
									break;
									case 4:
										oprop = OPROP_WATRW;
									break;
									case 5:
										oprop = OPROP_VORPW;
									break;
									case 6:
										oprop = OPROP_DRANW;
									break;
									case 7:
										oprop = OPROP_PSIOW;
									break;
									case 8:
										oprop = OPROP_FIREW;
									break;
									case 9:
										oprop = OPROP_COLDW;
									break;
									case 10:
										oprop = OPROP_LIVEW;
									break;
								}
								otmp = mksobj(CLUB, mkobjflags|MKOBJ_NOINIT);
								set_material_gm(otmp, BONE);
								add_oprop(otmp, OPROP_CCLAW);
								add_oprop(otmp, oprop);
								otmp->objsize = MZ_HUGE;
								otmp->spe = 2+rn2(3);
								fix_object(otmp);
								MAYBE_MERC(otmp)
								(void) mpickobj(mtmp, otmp);
								
								otmp = mksobj(WAISTCLOTH, mkobjflags|MKOBJ_ARTIF);
								otmp->oeroded3 = 3;
								otmp->obj_color = CLR_BROWN;
								otmp->spe = rnd(6);
								(void) mpickobj(mtmp, otmp);
								
								otmp = mksobj(rn2(20) ? ROBE : CLOAK_OF_MAGIC_RESISTANCE, mkobjflags|MKOBJ_ARTIF);
								otmp->oeroded3 = 3;
								otmp->spe = rnd(6);
								(void) mpickobj(mtmp, otmp);
								
								otmp = mksobj(find_gcirclet(), mkobjflags|MKOBJ_ARTIF);
								set_material_gm(otmp, VEGGY);
								otmp->obj_color = CLR_BROWN;
								otmp->spe = rnd(6);
								(void) mpickobj(mtmp, otmp);
								
								otmp = mksobj(MASK, mkobjflags|MKOBJ_ARTIF);
								otmp->oeroded3 = 1;
								set_material_gm(otmp, MINERAL);
								otmp->obj_color = CLR_WHITE;
								otmp->corpsenm = PM_DAUGHTER_OF_BEDLAM;
								(void) mpickobj(mtmp, otmp);
								
							}break;
							case 7:{
								//Were claw
								long long oprop;
								switch(rnd(20)){
									case 1:
										oprop = OPROP_ELECW;
									break;
									case 2:
										oprop = OPROP_ACIDW;
									break;
									case 3:
										oprop = OPROP_MAGCW;
									break;
									case 4:
										oprop = OPROP_WATRW;
									break;
									case 5:
										oprop = OPROP_VORPW;
									break;
									case 6:
										oprop = OPROP_DRANW;
									break;
									case 7:
										oprop = OPROP_PSIOW;
									break;
									case 8:
										oprop = OPROP_FIREW;
									break;
									case 9:
										oprop = OPROP_COLDW;
									break;
									case 10:
										oprop = OPROP_LIVEW;
									break;
									default:
										oprop = 0;
									break;
								}
								otmp = mksobj(BESTIAL_CLAW, mkobjflags|MKOBJ_NOINIT);
								add_oprop(otmp, oprop);
								MAYBE_MERC(otmp)
								otmp->spe = 2+rn2(4)+rn2(3);
								(void) mpickobj(mtmp, otmp);
								mtmp->mcrazed = TRUE;
							}break;
							case 8:{
								//Samurai
								if(rn2(2)){
									otmp = mksobj(NAGINATA, mkobjflags|MKOBJ_ARTIF);
									add_oprop(otmp, OPROP_RAKUW);
									otmp->spe = 0+rn2(4);
									MAYBE_MERC(otmp)
									(void) mpickobj(mtmp, otmp);
									otmp = mksobj(STILETTO, mkobjflags|MKOBJ_ARTIF);
									add_oprop(otmp, OPROP_RAKUW);
									otmp->spe = 0+rn2(4);
									(void) mpickobj(mtmp, otmp);

									otmp = mksobj(SHOES, mkobjflags|MKOBJ_ARTIF);
									set_material_gm(otmp, WOOD);
									otmp->spe = 2+rn2(3);
									bless(otmp);
									(void) mpickobj(mtmp, otmp);
									otmp = mksobj(ROBE, mkobjflags|MKOBJ_ARTIF);
									otmp->obj_color = CLR_BRIGHT_BLUE;
									otmp->spe = 2+rn2(3);
									bless(otmp);
									(void) mpickobj(mtmp, otmp);
									otmp = mksobj(SEDGE_HAT, mkobjflags|MKOBJ_ARTIF);
									otmp->obj_color = CLR_ORANGE;
									otmp->spe = 2+rn2(3);
									bless(otmp);
									(void) mpickobj(mtmp, otmp);
								}
								else {
									otmp = mksobj(WAKIZASHI, mkobjflags|MKOBJ_ARTIF);
									add_oprop(otmp, OPROP_RAKUW);
									otmp->spe = 0+rn2(4);
									(void) mpickobj(mtmp, otmp);

									otmp = mksobj(KATANA, mkobjflags|MKOBJ_ARTIF);
									add_oprop(otmp, OPROP_RAKUW);
									otmp->spe = 0+rn2(4);
									MAYBE_MERC(otmp)
									(void) mpickobj(mtmp, otmp);

									otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_ARTIF);
									otmp->oerodeproof = TRUE;
									otmp->spe = 2+rn2(3);
									bless(otmp);
									(void) mpickobj(mtmp, otmp);
									otmp = mksobj(SPLINT_MAIL, mkobjflags|MKOBJ_ARTIF);
									otmp->oerodeproof = TRUE;
									otmp->spe = 2+rn2(3);
									bless(otmp);
									(void) mpickobj(mtmp, otmp);
									otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_ARTIF);
									otmp->oerodeproof = TRUE;
									otmp->spe = 2+rn2(3);
									bless(otmp);
									(void) mpickobj(mtmp, otmp);
									otmp = mksobj(HELMET, mkobjflags|MKOBJ_ARTIF);
									otmp->oerodeproof = TRUE;
									otmp->spe = 2+rn2(3);
									bless(otmp);
									(void) mpickobj(mtmp, otmp);
									otmp = mksobj(MASK, mkobjflags|MKOBJ_ARTIF);
									otmp->oerodeproof = TRUE;
									otmp->corpsenm = PM_TENGU;
									otmp->obj_color = CLR_RED;
									bless(otmp);
									(void) mpickobj(mtmp, otmp);
								}
							}break;
							case 9:{
								//Patient, Ilsensine-touched or Fulvous
								otmp = mongets(mtmp, STRAITJACKET, mkobjflags);
								if(otmp) curse(otmp);
								if(rn2(2)){
									set_template(mtmp, CRANIUM_RAT);
									if(!rn2(10)) //1/20th total
										(void) mongets(mtmp, SPE_SECRETS, mkobjflags);
									else {
										otmp = mkobj( SPBOOK_CLASS, FALSE );
										curse(otmp);
										(void) mpickobj(mtmp, otmp);
									}
								}
								else {
									if(otmp) otmp->obj_color = CLR_YELLOW;
									set_template(mtmp, YELLOW_TEMPLATE);
									otmp = mkobj( SCROLL_CLASS, TRUE );
									if(otmp){
										otmp->obj_color = CLR_YELLOW;
										curse(otmp);
										(void) mpickobj(mtmp, otmp);
									}
									otmp = mkobj( SCROLL_CLASS, TRUE );
									if(otmp){
										otmp->obj_color = CLR_YELLOW;
										curse(otmp);
										(void) mpickobj(mtmp, otmp);
									}
									otmp = mkobj( SCROLL_CLASS, TRUE );
									if(otmp){
										otmp->obj_color = CLR_YELLOW;
										curse(otmp);
										(void) mpickobj(mtmp, otmp);
									}
								}
							}break;
							case 10:{
								//Black web drow
								otmp = mongets(mtmp, ARMORED_BOOTS, mkobjflags);
								if(otmp){
									set_material_gm(otmp, SHADOWSTEEL);
									otmp->spe = 2+rn2(3);
									otmp->oerodeproof = TRUE;
								}
								otmp = mongets(mtmp, PLATE_MAIL, mkobjflags);
								if(otmp){
									set_material_gm(otmp, SHADOWSTEEL);
									otmp->spe = 2+rn2(3);
									otmp->oerodeproof = TRUE;
								}
								otmp = mongets(mtmp, rn2(10) ? GAUNTLETS : GAUNTLETS_OF_POWER, mkobjflags);
								if(otmp){
									set_material_gm(otmp, SHADOWSTEEL);
									otmp->spe = 2+rn2(3);
									otmp->oerodeproof = TRUE;
								}
								otmp = mongets(mtmp, rn2(10) ? find_vhelm() : GAUNTLETS_OF_POWER, mkobjflags);
								if(otmp){
									set_material_gm(otmp, SHADOWSTEEL);
									otmp->spe = 2+rn2(3);
									otmp->oerodeproof = TRUE;
								}
								(void) mongets(mtmp, MUMMY_WRAPPING, mkobjflags);
								set_template(mtmp, M_BLACK_WEB);
								otmp = mongets(mtmp, KHAKKHARA, mkobjflags);
								if(otmp){
									set_material_gm(otmp, OBSIDIAN_MT);
									add_oprop(otmp, OPROP_BLADED);
									otmp->spe = 2+rn2(3);
									otmp->oerodeproof = TRUE;
								}
								otmp = mongets(mtmp, DROVEN_CROSSBOW, mkobjflags);
								if(otmp){
									set_material_gm(otmp, OBSIDIAN_MT);
									otmp->spe = 2+rn2(3);
									otmp->oerodeproof = TRUE;
								}
								m_initthrow(mtmp, DROVEN_BOLT, 20, mkobjflags);
							}break;
						}
					break;
					//Noble
					case 8:
						otmp = mksobj(RAPIER, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 2+rn2(3);
						MAYBE_MERC(otmp)
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(VICTORIAN_UNDERWEAR, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(GENTLEWOMAN_S_DRESS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 2+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(CLOAK, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(GLOVES, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(BUCKLER, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(find_gcirclet(), mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
					break;
					//Pirate
					case 9:
						otmp = mksobj(SCIMITAR, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						MAYBE_MERC(otmp)
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(FLINTLOCK, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(JACKET, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(RUFFLED_SHIRT, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(BUCKLER, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(BULLET, mkobjflags|MKOBJ_ARTIF);
						otmp->quan = rnd(5)+rnd(5)+rnd(5)+rnd(5);
						otmp->owt = weight(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
					//Priest
					case 10:
						otmp = mksobj(MACE, mkobjflags|MKOBJ_ARTIF);
						MAYBE_MERC(otmp)
						bless(otmp);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(ROBE, mkobjflags|MKOBJ_ARTIF);
						bless(otmp);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(BUCKLER, mkobjflags|MKOBJ_ARTIF);
						bless(otmp);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(POT_WATER, mkobjflags|MKOBJ_ARTIF);
						bless(otmp);
						otmp->quan = rnd(4);
						otmp->owt = weight(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
					//Ranger
					case 11:
						otmp = mksobj(SHORT_SWORD, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						MAYBE_MERC(otmp)
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(BOW, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(ARROW, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 2+rn2(3);
						otmp->quan = rnd(19)+rnd(19)+rnd(19)+rnd(19)+rnd(19);
						otmp->owt = weight(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(CLOAK_OF_DISPLACEMENT, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 2+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
					break;
					//Rogue
					case 12:
						otmp = mksobj(SHORT_SWORD, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						MAYBE_MERC(otmp)
						otmp->opoisoned = OPOISON_BASIC;
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(DAGGER, mkobjflags);
						otmp->spe = 0+rn2(4);
						otmp->quan = rnd(4)+rnd(4)+rnd(4)+rnd(4);
						otmp->opoisoned = OPOISON_BASIC;
						otmp->owt = weight(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(LEATHER_ARMOR, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(LOW_BOOTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						mongets(mtmp, LOCK_PICK, mkobjflags);
					break;
					//Samurai
					case 13:
						otmp = mksobj(NAGINATA, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						MAYBE_MERC(otmp)
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(STILETTO, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(YUMI, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(YA, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						otmp->quan = rnd(9)+rnd(9)+rnd(9)+rnd(9)+rnd(9);
						otmp->owt = weight(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(GENTLEWOMAN_S_DRESS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 2+rn2(3);
						bless(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(ROBE, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 2+rn2(3);
						bless(otmp);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(LOW_BOOTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 2+rn2(3);
						bless(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
					//Tourist
					case 14:
						otmp = mksobj(DART, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 2+rn2(3);
						otmp->quan = rnd(9)+rnd(9)+rnd(9)+rnd(9)+rnd(9);
						otmp->owt = weight(otmp);
						otmp->opoisoned = OPOISON_SLEEP;
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(STILETTO, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						MAYBE_MERC(otmp)
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(STILETTO, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(HAWAIIAN_SHIRT, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(HAWAIIAN_SHORTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(LOW_BOOTS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
						mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
						mongets(mtmp, SCR_MAGIC_MAPPING, mkobjflags);
						mongets(mtmp, SCR_MAGIC_MAPPING, mkobjflags);
						mongets(mtmp, SCR_MAGIC_MAPPING, mkobjflags);
						mongets(mtmp, SCR_MAGIC_MAPPING, mkobjflags);
						mongets(mtmp, EXPENSIVE_CAMERA, mkobjflags);
						mongets(mtmp, CREDIT_CARD, mkobjflags);
					break;
					//Wizard
					case 15:
						otmp = mksobj(QUARTERSTAFF, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 1+rn2(3);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(ATHAME, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = -1+rn2(4);
						MAYBE_MERC(otmp)
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(CLOAK_OF_MAGIC_RESISTANCE, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						otmp = mksobj(ARMOR_CLASS, mkobjflags|MKOBJ_ARTIF);
						otmp->spe = 0+rn2(4);
						(void) mpickobj(mtmp, otmp);
						mongets(mtmp, WAN_STRIKING, mkobjflags);
						switch(rn2(5)){
							case 0:
								mongets(mtmp, WAN_FIRE, mkobjflags);
							break;
							case 1:
								mongets(mtmp, WAN_COLD, mkobjflags);
							break;
							case 2:
								mongets(mtmp, WAN_SLEEP, mkobjflags);
							break;
							case 3:
								mongets(mtmp, WAN_LIGHTNING, mkobjflags);
							break;
							case 4:
								mongets(mtmp, WAN_CREATE_MONSTER, mkobjflags);
							break;
						}
					break;
				}
			} else {//not shopkeepers, deminymphs, or intoners
				int threshold = rnd(10)+rn2(11);
				if(mtmp->female && (faction == GOATMOM_FACTION) && u.uinsight > threshold){
					set_template(mtmp, MISTWEAVER);
					mtmp->m_insight_level = threshold;
				}
			}
		break;
	    case S_CENTAUR:
		if(ptr->mtyp == PM_DRIDER){
			chance = rnd(10);
			if(chance >= 7) mongets(mtmp, DROVEN_PLATE_MAIL, mkobjflags);
			else if(chance >= 6) mongets(mtmp, ELVEN_MITHRIL_COAT, mkobjflags);
			else if(chance >= 4) mongets(mtmp, ORCISH_CHAIN_MAIL, mkobjflags);
			(void)mongets(mtmp, DROVEN_CLOAK, mkobjflags);
			(void)mongets(mtmp, DROVEN_LANCE, mkobjflags);
			if(chance >= 5) (void)mongets(mtmp, DROVEN_SHORT_SWORD, mkobjflags);
			(void)mongets(mtmp, DROVEN_CROSSBOW, mkobjflags);
			m_initthrow(mtmp, DROVEN_BOLT, 20, mkobjflags);
		} else if(ptr->mtyp == PM_PRIESTESS_OF_GHAUNADAUR){
			chance = rnd(10);
			if(chance >= 9) mongets(mtmp, CRYSTAL_PLATE_MAIL, mkobjflags);
			else if(chance >= 6) mongets(mtmp, CLOAK_OF_PROTECTION, mkobjflags);
			else if(chance == 5) mongets(mtmp, CONSORT_S_SUIT, mkobjflags);
			(void)mongets(mtmp, CRYSTAL_SWORD, mkobjflags);
		} else if(ptr->mtyp == PM_ALIDER){
			otmp = mksobj(WHITE_VIBROZANBATO, mkobjflags);
			otmp->spe = 8;
			otmp->ovar1 = 50 + d(5,10);
			otmp->blessed = TRUE;
			set_material_gm(otmp, SILVER);
			otmp->cursed = FALSE;
			otmp->recharged = rn1(3,3);
			otmp->ovar1 = 50 + d(5,10);
			otmp = oname(otmp, "The Mk 2 Crescent Blade");
			otmp->oartifact = ART_CRESCENT_BLADE;
			fix_object(otmp);
			fully_identify_obj(otmp);
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(HAND_BLASTER, mkobjflags);
			otmp->spe = 8;
			otmp->ovar1 = 50 + d(5,10);
			otmp->recharged = rn2(3);
			bless(otmp);
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(find_signet_ring(), mkobjflags);
			otmp->ohaluengr = TRUE;
			otmp->oward = LAST_BASTION_SYMBOL;
			bless(otmp);
			(void) mpickobj(mtmp, otmp);
			
			otmp = mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
			if(otmp){
				otmp->obj_color = CLR_RED;
				otmp->spe = 4;
				bless(otmp);
			}
			otmp = mongets(mtmp, PLASTEEL_HELM, mkobjflags);
			if(otmp){
				otmp->obj_color = CLR_RED;
				otmp->spe = 4;
				bless(otmp);
			}
			otmp = mongets(mtmp, PLASTEEL_ARMOR, mkobjflags);
			if(otmp){
				otmp->obj_color = CLR_RED;
				otmp->spe = 4;
				bless(otmp);
			}
			otmp = mongets(mtmp, BODYGLOVE, mkobjflags);
			if(otmp){
				otmp->spe = 4;
				bless(otmp);
			}
			otmp = mongets(mtmp, GAUNTLETS_OF_POWER, mkobjflags);
			if(otmp){
				set_material_gm(otmp, PLASTIC);
				otmp->obj_color = CLR_RED;
				otmp->spe = 4;
				bless(otmp);
			}
		}
		if (rn2(2)) {
		    if(ptr->mtyp == PM_FOREST_CENTAUR || ptr->mtyp == PM_PLAINS_CENTAUR) {
				(void)mongets(mtmp, BOW, mkobjflags);
				m_initthrow(mtmp, ARROW, 12, mkobjflags);
		    } else if(ptr->mtyp == PM_MOUNTAIN_CENTAUR) {
				(void)mongets(mtmp, CROSSBOW, mkobjflags);
				m_initthrow(mtmp, CROSSBOW_BOLT, 12, mkobjflags);
		    }
		}
		if(ptr->mtyp == PM_FOREST_CENTAUR || ptr->mtyp == PM_PLAINS_CENTAUR || ptr->mtyp == PM_PLAINS_CENTAUR){
			int threshold = rnd(10)+rn2(11);
			if(mtmp->female && (faction == GOATMOM_FACTION) && u.uinsight > threshold){
				set_template(mtmp, MISTWEAVER);
				mtmp->m_insight_level = threshold;
			}
		}
		if(ptr->mtyp == PM_MOUNTAIN_CENTAUR){
			chance = rnd(10);
			if(chance == 10){
				mongets(mtmp, CHAIN_MAIL, mkobjflags);
				if(has_head_mon(mtmp)) mongets(mtmp, HELMET, mkobjflags);
			} else if(chance >= 7){
				mongets(mtmp, SCALE_MAIL, mkobjflags);
				if(has_head_mon(mtmp)) mongets(mtmp, HELMET, mkobjflags);
			} else if(chance >= 5){
				mongets(mtmp, STUDDED_LEATHER_ARMOR, mkobjflags);
				if(has_head_mon(mtmp)) mongets(mtmp, LEATHER_HELM, mkobjflags);
			} else mongets(mtmp, LEATHER_ARMOR, mkobjflags);
		}
		if(ptr->mtyp == PM_FORMIAN_TASKMASTER){
			switch (rn2(6)) {
			/* MAJOR fall through ... */
			case 0: (void) mongets(mtmp, WAN_MAGIC_MISSILE, mkobjflags);
			case 1: (void) mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
			case 2: (void) mongets(mtmp, POT_HEALING, mkobjflags);
				break;
			case 3: (void) mongets(mtmp, WAN_STRIKING, mkobjflags);
			case 4: (void) mongets(mtmp, POT_HEALING, mkobjflags);
			case 5: (void) mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
			}
			mongets(mtmp, ARCHAIC_PLATE_MAIL, mkobjflags);
			mongets(mtmp, ARCHAIC_GAUNTLETS, mkobjflags);
			mongets(mtmp, ARCHAIC_HELM, mkobjflags);
			otmp = mksobj(LANCE, mkobjflags);
			set_material_gm(otmp, COPPER);
			fix_object(otmp);
			(void) mpickobj(mtmp, otmp);
		}
		break;
	    case S_VAMPIRE:
			switch(ptr->mtyp){
				case PM_VAMPIRE:
					if(Infuture){
						otmp = mongets(mtmp, SUBMACHINE_GUN, mkobjflags);
						if(otmp){
							otmp->spe = rnd(7);
							curse(otmp);
							if(!rn2(20))
								add_oprop(otmp, OPROP_UNHYW);
						}

						otmp = mongets(mtmp, BULLET, mkobjflags);
						if(otmp){
							otmp->spe = rnd(7);
							otmp->quan = rn1(20,20);
							otmp->owt = weight(otmp);
							curse(otmp);
						}

						otmp = mongets(mtmp, FORCE_PIKE, mkobjflags);
						if(otmp){
							otmp->spe = rnd(7);
							curse(otmp);
							if(!rn2(20))
								add_oprop(otmp, OPROP_UNHYW);
						}

						otmp = mongets(mtmp, mtmp->female ? GENTLEMAN_S_SUIT : GENTLEWOMAN_S_DRESS, mkobjflags);
						if(otmp) otmp->spe = rnd(7);
						otmp = mongets(mtmp, mtmp->female ? STILETTOS : HIGH_BOOTS, mkobjflags);
						if(otmp) otmp->spe = rnd(7);
					}
					else {
						if(mtmp->female){
							(void) mongets(mtmp, STILETTO, mkobjflags);
							(void) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
							(void) mongets(mtmp, GLOVES, mkobjflags);
							(void) mongets(mtmp, LOW_BOOTS, mkobjflags);
						}
						else {
							(void) mongets(mtmp, rn2(20) ? SPEAR : LONG_SWORD, mkobjflags);
							(void) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
							(void) mongets(mtmp, GLOVES, mkobjflags);
							(void) mongets(mtmp, LOW_BOOTS, mkobjflags);
						}
					}
				break;
				case PM_VAMPIRE_LORD:
					if(Infuture){
						otmp = mongets(mtmp, SUBMACHINE_GUN, mkobjflags);
						if(otmp) {
							otmp->spe = rn1(4,3);
							curse(otmp);
							if(!rn2(10))
								add_oprop(otmp, OPROP_UNHYW);
						}

						otmp = mongets(mtmp, RAPIER, mkobjflags);
						if(otmp) {
							otmp->spe = 7;
							curse(otmp);
							add_oprop(otmp, OPROP_UNHYW);
							if(!rn2(4)) add_oprop(otmp, OPROP_PHSEW);
						}

						otmp = mongets(mtmp, ARCHAIC_BOOTS, mkobjflags);
						if(otmp) otmp->spe = rn1(4,3);
						otmp = mongets(mtmp, mtmp->female ? VICTORIAN_UNDERWEAR : RUFFLED_SHIRT, mkobjflags);
						if(otmp) otmp->spe = rn1(4,3);
						otmp = mongets(mtmp, ARCHAIC_PLATE_MAIL, mkobjflags);
						if(otmp) otmp->spe = rn1(4,3);
						otmp = mongets(mtmp, find_opera_cloak(), mkobjflags);
						if(otmp) otmp->spe = rn1(4,3);
						otmp = mongets(mtmp, ARCHAIC_GAUNTLETS, mkobjflags);
						if(otmp) otmp->spe = rn1(4,3);
						otmp = mongets(mtmp, ARCHAIC_HELM, mkobjflags);
						if(otmp) otmp->spe = rn1(4,3);
					}
					else {
						/* Knight */
						if(!rn2(4)){
							(void) mongets(mtmp, LONG_SWORD, mkobjflags);
							(void) mongets(mtmp, PLATE_MAIL, mkobjflags);
							if(!rn2(20)) mongets(mtmp, RUFFLED_SHIRT, mkobjflags);
							if(!rn2(20)) mongets(mtmp, find_opera_cloak(), mkobjflags);
							(void) mongets(mtmp, rn2(2) ? GAUNTLETS : GLOVES, mkobjflags);
							(void) mongets(mtmp, rn2(2) ? ARMORED_BOOTS : HIGH_BOOTS, mkobjflags);
						}
						/* Noble */
						else {
							(void) mongets(mtmp, rn2(2) ? LONG_SWORD : RAPIER, mkobjflags);
							(void) mongets(mtmp, GENTLEMAN_S_SUIT, mkobjflags);
							if(!rn2(20)) mongets(mtmp, RUFFLED_SHIRT, mkobjflags);
							if(!rn2(20)) mongets(mtmp, find_opera_cloak(), mkobjflags);
							(void) mongets(mtmp, GLOVES, mkobjflags);
							(void) mongets(mtmp, rn2(2) ? HIGH_BOOTS : LOW_BOOTS, mkobjflags);
						}
					}
				break;
				case PM_VAMPIRE_LADY:
					if(Infuture){
						otmp = mongets(mtmp, ARCHAIC_BOOTS, mkobjflags);
						if(otmp) otmp->spe = rn1(4,3);
						otmp = mongets(mtmp, mtmp->female ? VICTORIAN_UNDERWEAR : RUFFLED_SHIRT, mkobjflags);
						if(otmp) otmp->spe = rn1(4,3);
						otmp = mongets(mtmp, NOBLE_S_DRESS, mkobjflags);
						if(otmp){
							otmp->spe = rn1(4,3);
							set_material_gm(otmp, COPPER);
						}
						otmp = mongets(mtmp, find_opera_cloak(), mkobjflags);
						if(otmp) otmp->spe = rn1(4,3);
						otmp = mongets(mtmp, ARCHAIC_GAUNTLETS, mkobjflags);
						if(otmp) otmp->spe = rn1(4,3);
						otmp = mongets(mtmp, ARCHAIC_HELM, mkobjflags);
						if(otmp) otmp->spe = rn1(4,3);
					}
					else {
						otmp = mongets(mtmp, FEDORA, mkobjflags);
						if(otmp)
							otmp->obj_color = CLR_WHITE;
						otmp = mongets(mtmp, GENTLEWOMAN_S_DRESS, mkobjflags);
						if(otmp)
							otmp->obj_color = CLR_WHITE;
						if(!rn2(20)) mongets(mtmp, VICTORIAN_UNDERWEAR, mkobjflags);
						if(!rn2(100)) mongets(mtmp, find_opera_cloak(), mkobjflags);
						otmp = mongets(mtmp, GLOVES, mkobjflags);
						if(otmp)
							otmp->obj_color = CLR_WHITE;
						(void) mongets(mtmp, rn2(2) ? STILETTOS : LOW_BOOTS, mkobjflags);
					}
				break;
				case PM_MINA_HARKER:
					(void) mongets(mtmp, STILETTO, mkobjflags);
					(void) mongets(mtmp, BOW, mkobjflags);
					m_initthrow(mtmp, ARROW, 20, mkobjflags);
					(void) mongets(mtmp, GENTLEWOMAN_S_DRESS, mkobjflags);
					(void) mongets(mtmp, VICTORIAN_UNDERWEAR, mkobjflags);
					(void) mongets(mtmp, STILETTOS, mkobjflags);
				break;
				case PM_ILONA_SZILAGY:
					(void) mongets(mtmp, LONG_SWORD, mkobjflags);
					(void) mongets(mtmp, GENTLEWOMAN_S_DRESS, mkobjflags);
					(void) mongets(mtmp, LOW_BOOTS, mkobjflags);
				break;
				case PM_CARMILLA:
					(void) mongets(mtmp, RAPIER, mkobjflags);
					(void) mongets(mtmp, BOW, mkobjflags);
					m_initthrow(mtmp, ARROW, 20, mkobjflags);
					(void) mongets(mtmp, VICTORIAN_UNDERWEAR, mkobjflags);
				break;
				case PM_VLAD_THE_IMPALER:
					(void) mongets(mtmp, SCIMITAR, mkobjflags);
					(void) mongets(mtmp, ROUNDSHIELD, mkobjflags);
					(void) mongets(mtmp, HELMET, mkobjflags);
					if(!rn2(8)) mongets(mtmp, find_opera_cloak(), mkobjflags);
					(void) mongets(mtmp, PLATE_MAIL, mkobjflags);
					(void) mongets(mtmp, RUFFLED_SHIRT, mkobjflags);
					(void) mongets(mtmp, GAUNTLETS, mkobjflags);
					(void) mongets(mtmp, ARMORED_BOOTS, mkobjflags);
				break;
			}
		break;
	    case S_WRAITH:
		if(mm == PM_NAZGUL){
			if(Infuture){
#define NazFutArmor(typ) \
				otmp = mongets(mtmp, typ, mkobjflags);\
				if(otmp){\
					otmp->obj_color = CLR_BLACK;\
					otmp->spe = rnd(7);\
					add_oprop(otmp, OPROP_UNHY);\
					curse(otmp);\
				}
				NazFutArmor(PLASTEEL_BOOTS);
				NazFutArmor(BODYGLOVE);
				NazFutArmor(PLASTEEL_ARMOR);
				NazFutArmor(PLASTEEL_GAUNTLETS);
				NazFutArmor(PLASTEEL_HELM);
				NazFutArmor(rn2(3) ? CLOAK_OF_PROTECTION : CLOAK_OF_MAGIC_RESISTANCE);
#undef NazFutArmor
				otmp = mongets(mtmp, STILETTO, mkobjflags);
				if(otmp){
					otmp->spe = 9;
					add_oprop(otmp, OPROP_UNHYW);
					add_oprop(otmp, OPROP_MORGW);
					add_oprop(otmp, OPROP_PHSEW);
					add_oprop(otmp, OPROP_FLAYW);
					curse(otmp);
				}
				otmp = mongets(mtmp, 
					!rn2(4) ? FORCE_PIKE : !rn2(3) ? FORCE_SWORD : rn2(2) ? FORCE_WHIP : DOUBLE_FORCE_BLADE,
					mkobjflags);
				if(otmp){
					otmp->obj_color = CLR_BLACK;
					otmp->spe = 9;
					add_oprop(otmp, OPROP_UNHYW);
					add_oprop(otmp, OPROP_MORGW);
					curse(otmp);
				}
				(void) mongets(mtmp, CHUNK_OF_FOSSIL_DARK, mkobjflags);
			} else {
				otmp = mksobj(STILETTO, mkobjflags);
				add_oprop(otmp, OPROP_MORGW);
				otmp->opoisoned = OPOISON_BASIC;
				set_material_gm(otmp, METAL);
				fix_object(otmp);
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(LONG_SWORD, mkobjflags);
				add_oprop(otmp, OPROP_UNHYW);
				otmp->oeroded = 1;
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
			}
		} else if(mm == PM_BRIGHT_WALKER) {
			(void) mongets(mtmp, BROADSWORD, mkobjflags);
			(void) mongets(mtmp, KITE_SHIELD, mkobjflags);
			(void) mongets(mtmp, GAUNTLETS, mkobjflags);
			(void) mongets(mtmp, PLATE_MAIL, mkobjflags);
			(void) mongets(mtmp, ARMORED_BOOTS, mkobjflags);
		} else if(mm == PM_NECROMANCER) {
			otmp = mksobj(LONG_SWORD, mkobjflags);
			add_oprop(otmp, OPROP_UNHYW);
			add_oprop(otmp, OPROP_MORGW);
			otmp->opoisoned = OPOISON_BASIC;
			otmp->oerodeproof = TRUE;
			curse(otmp);
			(void) mpickobj(mtmp, otmp);
		} else if(mm == PM_BARROW_WIGHT) {
			if(Infuture){
				otmp = mksobj(STILETTO, mkobjflags);
				if(otmp){
					set_material_gm(otmp, MINERAL);
					otmp->spe = rnd(3);
					if(!rn2(5)){
						otmp->spe += rnd(3);
						add_oprop(otmp, OPROP_WATRW);
					}
					if(!rn2(2)){
						otmp->spe += rnd(3);
						add_oprop(otmp, OPROP_PHSEW);
					}
					if(!rn2(2)){
						otmp->spe += rnd(3);
						add_oprop(otmp, OPROP_UNHYW);
						curse(otmp);
					}
				}
				(void) mpickobj(mtmp, otmp);
				otmp = mongets(mtmp, MUMMY_WRAPPING, mkobjflags);
				if(otmp){
					otmp->oeroded3 = 1;
					otmp->spe = rnd(7);
				}
				if(!rn2(10)){
					otmp = mksobj(STATUE, MKOBJ_NOINIT);
					otmp->corpsenm = PM_PARASITIC_MIND_FLAYER;
					fix_object(otmp);
					mpickobj(mtmp, otmp);
					if(which_armor(mtmp, W_ARMH)){
						struct obj *hlm = which_armor(mtmp, W_ARMH);
						m_lose_armor(mtmp, hlm);
					}
					//Equip it to the head slot
					mtmp->misc_worn_check |= W_ARMH;
					otmp->owornmask |= W_ARMH;
					update_mon_intrinsics(mtmp, otmp, TRUE, TRUE);
				} else {
					otmp = mongets(mtmp, MASK, mkobjflags);
					if(otmp) otmp->corpsenm = PM_MIND_FLAYER;
				}
			}
			else {
				otmp = mksobj(STILETTO, mkobjflags);
				if(!rn2(5)) set_material_gm(otmp, SILVER);
				else if(!rn2(4)) set_material_gm(otmp, GOLD);
				fix_object(otmp);
				if(!rn2(20)) add_oprop(otmp, OPROP_LESSER_HOLYW);
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(LONG_SWORD, mkobjflags);
				if(!rn2(5)) set_material_gm(otmp, SILVER);
				else if(!rn2(4)) set_material_gm(otmp, GOLD);
				fix_object(otmp);
				if(!rn2(20)) add_oprop(otmp, OPROP_LESSER_HOLYW);
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
			}
		} else if(mm == PM_ZARIELITE_ZEALOT) {
			otmp = mongets(mtmp, SPEAR, mkobjflags);
			if(otmp){
				set_material_gm(otmp, SILVER);
				add_oprop(otmp, OPROP_LESSER_HOLYW);
				otmp->spe = 7;
				bless(otmp);
			}
			otmp = mongets(mtmp, TOWER_SHIELD, mkobjflags);
			if(otmp){
				set_material_gm(otmp, WOOD);
				otmp->spe = 7;
				bless(otmp);
			}

			otmp = mongets(mtmp, HELMET, mkobjflags);
			if(otmp){
				set_material_gm(otmp, COPPER);
				otmp->spe = 7;
				bless(otmp);
			}
			otmp = mongets(mtmp, SCALE_MAIL, mkobjflags);
			if(otmp){
				set_material_gm(otmp, COPPER);
				otmp->spe = 7;
				bless(otmp);
			}
			otmp = mongets(mtmp, STUDDED_LEATHER_CLOAK, mkobjflags);
			if(otmp){
				otmp->spe = 7;
				bless(otmp);
			}
			otmp = mongets(mtmp, GLOVES, mkobjflags);
			if(otmp){
				set_material_gm(otmp, LEATHER);
				otmp->spe = 7;
				bless(otmp);
			}
			otmp = mongets(mtmp, HIGH_BOOTS, mkobjflags);
			if(otmp){
				set_material_gm(otmp, LEATHER);
				otmp->spe = 7;
				bless(otmp);
			}
		} else if(mm == PM_ZARIELITE_HERETIC) {
			otmp = mongets(mtmp, SPEAR, mkobjflags);
			if(otmp){
				set_material_gm(otmp, IRON);
				add_oprop(otmp, OPROP_LESSER_UNHYW);
				otmp->obj_color = CLR_BLACK;
				otmp->spe = 7;
				curse(otmp);
			}
			otmp = mongets(mtmp, TOWER_SHIELD, mkobjflags);
			if(otmp){
				set_material_gm(otmp, WOOD);
				otmp->spe = 7;
				curse(otmp);
			}

			otmp = mongets(mtmp, HELMET, mkobjflags);
			if(otmp){
				set_material_gm(otmp, COPPER);
				otmp->spe = 7;
				curse(otmp);
			}
			otmp = mongets(mtmp, SCALE_MAIL, mkobjflags);
			if(otmp){
				set_material_gm(otmp, COPPER);
				otmp->spe = 7;
				curse(otmp);
			}
			otmp = mongets(mtmp, STUDDED_LEATHER_CLOAK, mkobjflags);
			if(otmp){
				otmp->spe = 7;
				curse(otmp);
			}
			otmp = mongets(mtmp, GLOVES, mkobjflags);
			if(otmp){
				set_material_gm(otmp, LEATHER);
				otmp->spe = 7;
				curse(otmp);
			}
			otmp = mongets(mtmp, HIGH_BOOTS, mkobjflags);
			if(otmp){
				set_material_gm(otmp, LEATHER);
				otmp->spe = 7;
				curse(otmp);
			}
		}
		break;
	    case S_ZOMBIE:
		if(mm == PM_UNDEAD_KNIGHT){
			otmp = mksobj(LONG_SWORD, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded = 1;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(KITE_SHIELD, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded = 1;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, IRON);
			otmp->oeroded = 1;
			fix_object(otmp);
			(void) mpickobj(mtmp,otmp);

			otmp = mksobj(SCALE_MAIL, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded = 2;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, IRON);
			otmp->oeroded = 1;
			fix_object(otmp);
			(void) mpickobj(mtmp,otmp);
		} else if(mm == PM_WARRIOR_OF_SUNLIGHT){
			otmp = mksobj(LONG_SWORD, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(KITE_SHIELD, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, IRON);
			otmp->spe = 3;
			(void) mpickobj(mtmp,otmp);

			otmp = mksobj(PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(HELMET, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, IRON);
			otmp->spe = 3;
			(void) mpickobj(mtmp,otmp);
		} else if(mm == PM_UNDEAD_MAIDEN){
			otmp = mksobj(STILETTO, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded = 1;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(BELL, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, GOLD);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(LOW_BOOTS, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded2 = 1;
			(void) mpickobj(mtmp,otmp);

			otmp = mksobj(ROBE, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, CLOTH);
			otmp->obj_color = CLR_WHITE;
			otmp->oeroded2 = 1;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(GLOVES, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, CLOTH);
			otmp->obj_color = CLR_WHITE;
			otmp->oeroded2 = 1;
			(void) mpickobj(mtmp,otmp);
			
			otmp = mksobj(WIDE_HAT, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, CLOTH);
			otmp->obj_color = CLR_WHITE;
			otmp->oeroded2 = 1;
			(void) mpickobj(mtmp,otmp);
		} else if(mm == PM_KNIGHT_OF_THE_PRINCESS_S_GUARD){
			otmp = mksobj(SPEAR, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, SILVER);
			otmp->objsize = MZ_LARGE;
			fix_object(otmp);
			bless(otmp);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(BELL, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, GOLD);
			bless(otmp);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 3;
			(void) mpickobj(mtmp,otmp);

			otmp = mksobj(ROBE, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, CLOTH);
			otmp->obj_color = CLR_WHITE;
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(HELMET, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, LEATHER);
			otmp->spe = 3;
			(void) mpickobj(mtmp,otmp);
		} else if(mm == PM_BLUE_SENTINEL){
			otmp = mksobj(SHORT_SWORD, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded = 1;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(HALBERD, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded = 1;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(ROUNDSHIELD, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, IRON);
			otmp->objsize = MZ_SMALL;
			fix_object(otmp);
			otmp->oeroded = 1;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, IRON);
			otmp->oeroded = 1;
			fix_object(otmp);
			(void) mpickobj(mtmp,otmp);

			otmp = mksobj(PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded = 2;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(find_vhelm(), mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded = 1;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, IRON);
			otmp->oeroded = 1;
			fix_object(otmp);
			(void) mpickobj(mtmp,otmp);
		} else if(mm == PM_DARKMOON_KNIGHT){
			otmp = mksobj(LONG_SWORD, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(STILETTO, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, COPPER);
			otmp->spe = 3;
			(void) mpickobj(mtmp,otmp);

			otmp = mksobj(PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, COPPER);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(find_vhelm(), mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, COPPER);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, COPPER);
			otmp->spe = 3;
			(void) mpickobj(mtmp,otmp);
		} else if(mm == PM_UNDEAD_REBEL){
			otmp = mksobj(STILETTO, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded = 1;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(CLUB, mkobjflags|MKOBJ_NOINIT);
			add_oprop(otmp, OPROP_OCLTW);
			otmp->oeroded = 1;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded2 = 1;
			(void) mpickobj(mtmp,otmp);

			otmp = mksobj(LEATHER_ARMOR, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded2 = 1;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(GLOVES, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded2 = 1;
			(void) mpickobj(mtmp,otmp);
			
			otmp = mksobj(LEATHER_HELM, mkobjflags|MKOBJ_NOINIT);
			otmp->oeroded2 = 1;
			(void) mpickobj(mtmp,otmp);
		} else if(mm == PM_OCCULTIST){
			otmp = mksobj(CLUB, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, DRAGON_HIDE);
			add_oprop(otmp, OPROP_OCLTW);
			otmp->objsize = MZ_HUGE;
			fix_object(otmp);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(TOWER_SHIELD, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, DRAGON_HIDE);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, DRAGON_HIDE);
			(void) mpickobj(mtmp,otmp);

			otmp = mksobj(PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, DRAGON_HIDE);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(find_vhelm(), mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, DRAGON_HIDE);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, DRAGON_HIDE);
			(void) mpickobj(mtmp,otmp);
		} else if(mm == PM_PARDONER){
			otmp = mksobj(RAPIER, mkobjflags|MKOBJ_NOINIT);
			add_oprop(otmp, OPROP_LESSER_MAGCW);
			set_material_gm(otmp, METAL);
			otmp->spe = 3;
			bless(otmp);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(STILETTO, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, METAL);
			otmp->spe = 3;
			bless(otmp);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_NOINIT);
			otmp->obj_color = CLR_BLACK;
			otmp->spe = 3;
			(void) mpickobj(mtmp,otmp);

			otmp = mksobj(JACKET, mkobjflags|MKOBJ_NOINIT);
			otmp->obj_color = CLR_BLACK;
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(HELMET, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, GOLD);
			otmp->spe = 3;
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(GLOVES, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, CLOTH);
			otmp->obj_color = CLR_BLACK;
			otmp->spe = 3;
			(void) mpickobj(mtmp,otmp);
		} else if(mm == PM_PHALANX){
			(void) mongets(mtmp, SPEAR, mkobjflags);
			otmp = mksobj(ROUNDSHIELD, mkobjflags|MKOBJ_NOINIT);
			set_material_gm(otmp, LEATHER);
			otmp->objsize = MZ_LARGE;
			fix_object(otmp);
			(void) mpickobj(mtmp,otmp);
				
			(void) mongets(mtmp, SPEAR, mkobjflags);
			(void) mongets(mtmp, SPEAR, mkobjflags);
		} else if(mm == PM_SKELETAL_PIRATE){
				otmp = rn2(2) ? mksobj(SCIMITAR, mkobjflags|MKOBJ_NOINIT) : mksobj(KNIFE, mkobjflags|MKOBJ_NOINIT);
				// curse(otmp);
				otmp->oeroded = 1;
				(void) mpickobj(mtmp, otmp);
				
				otmp = rn2(2) ? mksobj(HIGH_BOOTS, mkobjflags|MKOBJ_NOINIT) : mksobj(JACKET, mkobjflags|MKOBJ_NOINIT);
				// curse(otmp);
				otmp->oeroded2 = 1;
				(void) mpickobj(mtmp, otmp);
				
				otmp = rn2(2) ? mksobj(FLINTLOCK, mkobjflags|MKOBJ_NOINIT) : mksobj(KNIFE, mkobjflags|MKOBJ_NOINIT);
				// curse(otmp);
				otmp->oeroded = 1;
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(BULLET, mkobjflags|MKOBJ_NOINIT);
				otmp->quan += rnd(10);
				otmp->oeroded = 1;
				otmp->owt = weight(otmp);
				(void) mpickobj(mtmp, otmp);
				break;
		} else {
			if (!rn2(4)) (void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
			if (!rn2(4))
				(void)mongets(mtmp, (rn2(3) ? KNIFE : SHORT_SWORD), mkobjflags);
		}
		break;
	    case S_LIZARD:
		if (mm == PM_SALAMANDER)
			(void)mongets(mtmp, (rn2(7) ? SPEAR : rn2(3) ?
					     TRIDENT : STILETTO), mkobjflags);
		break;
	    case S_DEMON:

		if(mm>=PM_FIERNA) return; //Lords handled above, no random cursed stuff!
		switch (mm) {
			case PM_DAMNED_PIRATE:
				otmp = mksobj(SCIMITAR, mkobjflags|MKOBJ_NOINIT);
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(LEATHER_ARMOR, mkobjflags|MKOBJ_NOINIT);
				curse(otmp);
				otmp->oeroded = 1;
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(FLINTLOCK, mkobjflags|MKOBJ_NOINIT);
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(BULLET, mkobjflags|MKOBJ_NOINIT);
				otmp->quan += 10;
				otmp->owt = weight(otmp);
				(void) mpickobj(mtmp, otmp);
				
				return; //bypass general weapons
			break;
		    case PM_HORNED_DEVIL:
				(void)mongets(mtmp, rn2(4) ? TRIDENT : BULLWHIP, mkobjflags);
				if(Inhell || goodequip){
					mongets(mtmp, SPLINT_MAIL, mkobjflags);
					mongets(mtmp, GAUNTLETS, mkobjflags);
					mongets(mtmp, ARMORED_BOOTS, mkobjflags);
				}
			break;
		    case PM_ERINYS:{
				if(In_mordor_quest(&u.uz) 
					&& !In_mordor_forest(&u.uz)
					&& !Is_ford_level(&u.uz)
					&& !In_mordor_fields(&u.uz)
					&& in_mklev
				){
					(void)mongets(mtmp, SHACKLES, mkobjflags);
					mtmp->entangled = SHACKLES;
					mtmp->m_lev = 14;
					mtmp->mhpmax = 7*8 + d(7,8);
					mtmp->mhp = mtmp->mhpmax;
					return;
				}
				if(!Inhell && !goodequip) chance = 3; //Leather armor
				else chance = rnd(10);
				if(chance >= 9){
					mongets(mtmp, HELMET, mkobjflags);
					mongets(mtmp, PLATE_MAIL, mkobjflags);
					mongets(mtmp, KITE_SHIELD, mkobjflags);
					mongets(mtmp, GAUNTLETS, mkobjflags);
					otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
					set_material_gm(otmp, IRON);
					(void) mpickobj(mtmp, otmp);
					mongets(mtmp, LONG_SWORD, mkobjflags);
					// mongets(mtmp, IRON_BANDS, mkobjflags);
				} else if(chance >= 6){
					mongets(mtmp, HELMET, mkobjflags);
					mongets(mtmp, CHAIN_MAIL, mkobjflags);
					mongets(mtmp, GLOVES, mkobjflags);
					mongets(mtmp, HIGH_BOOTS, mkobjflags);
					mongets(mtmp, TWO_HANDED_SWORD, mkobjflags);
					// mongets(mtmp, ROPE_OF_ENTANGLING, mkobjflags);
				} else if(chance >= 3){
					mongets(mtmp, LEATHER_HELM, mkobjflags);
					mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					mongets(mtmp, GLOVES, mkobjflags);
					mongets(mtmp, HIGH_BOOTS, mkobjflags);
					mongets(mtmp, RAPIER, mkobjflags);
					mongets(mtmp, STILETTO, mkobjflags);
					mongets(mtmp, BOW, mkobjflags);
					m_initthrow(mtmp, ARROW, 20, mkobjflags);
					// mongets(mtmp, ROPE_OF_ENTANGLING, mkobjflags);
				} else {
					mongets(mtmp, find_gcirclet(), mkobjflags);
					mongets(mtmp, GENTLEWOMAN_S_DRESS, mkobjflags);
					mongets(mtmp, VICTORIAN_UNDERWEAR, mkobjflags);
					otmp = mksobj(STILETTOS, mkobjflags);
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					mongets(mtmp, CRYSTAL_GAUNTLETS, mkobjflags);
					mongets(mtmp, STILETTO, mkobjflags);
					// mongets(mtmp, RAZOR_WIRE, mkobjflags);
				}
			}break;
		    case PM_BARBED_DEVIL:
				(void)mongets(mtmp, SHORT_SWORD, mkobjflags);
				if(Inhell || goodequip){
					mongets(mtmp, SPEAR, mkobjflags);
					mongets(mtmp, SPEAR, mkobjflags);
					mongets(mtmp, HELMET, mkobjflags);
					mongets(mtmp, TOWER_SHIELD, mkobjflags);
					mongets(mtmp, SPLINT_MAIL, mkobjflags);
					mongets(mtmp, GAUNTLETS, mkobjflags);
					mongets(mtmp, ARMORED_BOOTS, mkobjflags);
				}
			break;
		    case PM_VROCK:
					if (!rn2(2) || Inhell || goodequip) switch (rn2(12)) {
						case 0: (void)mongets(mtmp, RANSEUR, mkobjflags); break;
						case 1: (void)mongets(mtmp, PARTISAN, mkobjflags); break;
						case 2: (void)mongets(mtmp, GLAIVE, mkobjflags); break;
						case 3: (void)mongets(mtmp, SPETUM, mkobjflags); break;
						case 4: (void)mongets(mtmp, HALBERD, mkobjflags); break;
						case 5: (void)mongets(mtmp, BARDICHE, mkobjflags); break;
						case 6: (void)mongets(mtmp, VOULGE, mkobjflags); break;
						case 7: (void)mongets(mtmp, FAUCHARD, mkobjflags); break;
						case 8: (void)mongets(mtmp, GUISARME, mkobjflags); break;
						case 9: (void)mongets(mtmp, BILL_GUISARME, mkobjflags); break;
						case 10: (void)mongets(mtmp, LUCERN_HAMMER, mkobjflags); break;
						case 11: (void)mongets(mtmp, BEC_DE_CORBIN, mkobjflags); break;
					}
					if(Inhell || goodequip){
						(void)mongets(mtmp, PLATE_MAIL, mkobjflags);
					}
			break;
		    case PM_BONE_DEVIL:
				(void)mongets(mtmp, rn2(4) ? TRIDENT : WAR_HAMMER, mkobjflags);
				if(Inhell || goodequip){
					otmp = mongets(mtmp, HELMET, mkobjflags);
					if(otmp) set_material_gm(otmp, BONE);
					otmp = mongets(mtmp, ROUNDSHIELD, mkobjflags);
					if(otmp) set_material_gm(otmp, BONE);
					otmp = mongets(mtmp, SPLINT_MAIL, mkobjflags);
					if(otmp) set_material_gm(otmp, BONE);
					otmp = mongets(mtmp, GAUNTLETS, mkobjflags);
					if(otmp) set_material_gm(otmp, BONE);
					otmp = mongets(mtmp, ARMORED_BOOTS, mkobjflags);
					if(otmp) set_material_gm(otmp, BONE);
				}
			break;
			case PM_ICE_DEVIL:
				if (!rn2(4)) {
					(void)mongets(mtmp, TRIDENT, mkobjflags);
					if(!rn2(2))
						(void)mongets(mtmp, SHORT_SWORD, mkobjflags);
				}
				else (void)mongets(mtmp, GLAIVE, mkobjflags);
			break;
			case PM_LILITU:
				if(faction == GOATMOM_FACTION){
					//Cultist of the Black Goat
					otmp = mksobj(VIPERWHIP, mkobjflags|MKOBJ_NOINIT);
					otmp->spe = 6;
					otmp->ovar1 = 4;
					otmp->opoisoned = OPOISON_ACID;
					otmp->opoisonchrgs = 3;
					set_material_gm(otmp, BONE);
					(void) mpickobj(mtmp, otmp);
					int threshold = rnd(10)+rn2(11);
					if(mtmp->female && u.uinsight > threshold){
						set_template(mtmp, MISTWEAVER);
						mtmp->m_insight_level = threshold;
					} else {
						otmp = mksobj(WAR_HAT, mkobjflags);
						otmp->spe = 4;
						set_material_gm(otmp, BONE);
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					}
					otmp = mksobj(PLATE_MAIL, mkobjflags);
					otmp->spe = 4;
					set_material_gm(otmp, BONE);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
					otmp = mksobj(ROUNDSHIELD, mkobjflags);
					otmp->spe = 4;
					set_material_gm(otmp, BONE);
					fix_object(otmp);
					(void) mpickobj(mtmp, otmp);
				}
				else if(Inhell || goodequip){
					int kit = rn2(6);
					switch(kit){
						case 0:
							otmp = mksobj(MACE, mkobjflags|MKOBJ_NOINIT);
						break;
						case 1:
							otmp = mksobj(KHAKKHARA, mkobjflags|MKOBJ_NOINIT);
							set_material_gm(otmp, IRON);
						break;
						case 2:
							otmp = mksobj(QUARTERSTAFF, mkobjflags|MKOBJ_NOINIT);
						break;
						case 3:
							otmp = 0; //Monk fists
						break;
						case 4:
							otmp = mksobj(ATHAME, mkobjflags|MKOBJ_NOINIT);
						break;
						case 5:
							otmp = mksobj(SHEPHERD_S_CROOK, mkobjflags|MKOBJ_NOINIT);
							set_material_gm(otmp, GOLD);
							otmp->objsize = MZ_SMALL;
							fix_object(otmp);
							curse(otmp);
							otmp->spe = 6;
							otmp->oerodeproof = TRUE;
							(void) mpickobj(mtmp, otmp);
							
							otmp = mksobj(FLAIL, mkobjflags|MKOBJ_NOINIT);
							set_material_gm(otmp, GOLD);
						break;
					}
					if(otmp){
						curse(otmp);
						otmp->spe = 6;
						otmp->oerodeproof = TRUE;
						MAYBE_MERC(otmp)
						(void) mpickobj(mtmp, otmp);
					}
					
					switch(kit){
						case 0:
						case 1:
							otmp = mongets(mtmp, rn2(7) ? ROBE : rn2(3) ? CLOAK_OF_PROTECTION : CLOAK_OF_MAGIC_RESISTANCE, mkobjflags|MKOBJ_NOINIT);
							if(otmp){
								curse(otmp);
								otmp->spe = 4;
							}
							if(kit == 3){
								otmp = mongets(mtmp, SEDGE_HAT, mkobjflags|MKOBJ_NOINIT);
								if(otmp){
									curse(otmp);
									otmp->spe = 4;
								}
							}
						break;
						case 2:
							otmp = mongets(mtmp, HEALER_UNIFORM, mkobjflags|MKOBJ_NOINIT);
							if(otmp){
								curse(otmp);
								otmp->spe = 4;
							}
							// if(rn2(2)){
								// mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
								// mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
								// mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
								// mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
							// }
						break;
						case 3:
							otmp = mongets(mtmp, ROBE, mkobjflags|MKOBJ_NOINIT);
							if(otmp){
								otmp->obj_color = CLR_ORANGE;
								curse(otmp);
								otmp->spe = 4;
							}
							otmp = mongets(mtmp, SEDGE_HAT, mkobjflags|MKOBJ_NOINIT);
							if(otmp){
								curse(otmp);
								otmp->spe = 4;
							}
						break;
						case 4:
							otmp = mongets(mtmp, CLOAK, mkobjflags|MKOBJ_NOINIT);
							if(otmp){
								otmp->obj_color = CLR_BLACK;
								curse(otmp);
								otmp->spe = 4;
							}
							otmp = mongets(mtmp, PLAIN_DRESS, mkobjflags|MKOBJ_NOINIT);
							if(otmp){
								curse(otmp);
								otmp->spe = 4;
							}
							otmp = mongets(mtmp, WITCH_HAT, mkobjflags|MKOBJ_NOINIT);
							if(otmp){
								curse(otmp);
								otmp->spe = 4;
							}
						break;
						case 5:
							otmp = mksobj(MASK, mkobjflags);
							otmp->corpsenm = PM_SUCCUBUS;
							set_material_gm(otmp, GOLD);
							fix_object(otmp);
							curse(otmp);
							(void) mpickobj(mtmp, otmp);
							
							otmp = mksobj(SCALE_MAIL, mkobjflags|MKOBJ_NOINIT);
							otmp->spe = 4;
							set_material_gm(otmp, GOLD);
							curse(otmp);
							(void) mpickobj(mtmp,otmp);
							
							otmp = mksobj(rnd_good_amulet(), mkobjflags);
							set_material_gm(otmp, GOLD);
							fix_object(otmp);
							curse(otmp);
							(void) mpickobj(mtmp, otmp);
						break;
					}
				}
			break;
			case PM_NALFESHNEE:
				otmp = mksobj(KHAKKHARA, mkobjflags|MKOBJ_NOINIT);
				curse(otmp);
				otmp->oerodeproof = TRUE;
				set_material_gm(otmp, IRON);
				(void) mpickobj(mtmp, otmp);
			break;
		    case PM_MARILITH:{
				if(Inhell || goodequip){
					chance = rnd(10);
					if(chance >= 9){
						mongets(mtmp, PLATE_MAIL, mkobjflags);
						mongets(mtmp, rn2(20) ? GAUNTLETS : GAUNTLETS_OF_POWER, mkobjflags);
						mongets(mtmp, HELMET, mkobjflags);
					}
					else if(chance >= 6){
						mongets(mtmp, CHAIN_MAIL, mkobjflags);
						mongets(mtmp, GAUNTLETS, mkobjflags);
						mongets(mtmp, HELMET, mkobjflags);
					}
					else if(chance >= 3){
						mongets(mtmp, STUDDED_LEATHER_ARMOR, mkobjflags);
						mongets(mtmp, GLOVES, mkobjflags);
						mongets(mtmp, LEATHER_HELM, mkobjflags);
					}
					else {
						mongets(mtmp, LEATHER_ARMOR, mkobjflags);
						mongets(mtmp, GLOVES, mkobjflags);
						mongets(mtmp, LEATHER_HELM, mkobjflags);
					}
					switch(rn2(3)){
						case 0:
							mongets(mtmp, TRIDENT, mkobjflags);
						break;
						case 1:
							mongets(mtmp, STILETTO, mkobjflags);
						break;
						case 2:
							mongets(mtmp, SICKLE, mkobjflags);
						break;
						
					}
					switch(rn2(3)){
						case 0:
							mongets(mtmp, AXE, mkobjflags);
						break;
						case 1:
							mongets(mtmp, SHORT_SWORD, mkobjflags);
						break;
						case 2:
							mongets(mtmp, MACE, mkobjflags);
						break;
						
					}
					switch(rn2(3)){
						case 0:
							mongets(mtmp, RAPIER, mkobjflags);
						break;
						case 1:
							mongets(mtmp, RUNESWORD, mkobjflags);
						break;
						case 2:
							mongets(mtmp, BROADSWORD, mkobjflags);
						break;
						
					}
					switch(rn2(3)){
						case 0:
							mongets(mtmp, LONG_SWORD, mkobjflags);
						break;
						case 1:
							mongets(mtmp, KATANA, mkobjflags);
						break;
						case 2:
							mongets(mtmp, SABER, mkobjflags);
						break;
						
					}
					switch(rn2(2)){
						case 0:
							mongets(mtmp, SCIMITAR, mkobjflags);
						break;
						case 1:
							mongets(mtmp, MORNING_STAR, mkobjflags);
						break;
						
					}
					switch(rn2(2)){
						case 0:
							mongets(mtmp, WAR_HAMMER, mkobjflags);
						break;
						case 1:
							mongets(mtmp, FLAIL, mkobjflags);
						break;
						
					}
				}
				else if(In_mordor_quest(&u.uz) 
					&& !In_mordor_forest(&u.uz)
					&& !Is_ford_level(&u.uz)
					&& !In_mordor_fields(&u.uz)
					&& in_mklev
				){
					mongets(mtmp, PLAIN_DRESS, mkobjflags);
					otmp = mongets(mtmp, SCALE_MAIL, mkobjflags);
					if(otmp) set_material_gm(otmp, GOLD);
					otmp = mongets(mtmp, ARCHAIC_GAUNTLETS, mkobjflags);
					if(otmp) set_material_gm(otmp, GOLD);
					
					mongets(mtmp, STILETTO, mkobjflags);
					mongets(mtmp, SHORT_SWORD, mkobjflags);
					mongets(mtmp, ELVEN_SHORT_SWORD, mkobjflags);
					mongets(mtmp, DWARVISH_SHORT_SWORD, mkobjflags);
					mongets(mtmp, KHOPESH, mkobjflags);
					mongets(mtmp, WAKIZASHI, mkobjflags);
				}
			}break;
		    case PM_PIT_FIEND:
				otmp = mksobj(TRIDENT, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = rnd(9);
				otmp->objsize = MZ_HUGE;
				(void) mpickobj(mtmp, otmp);
			break;
		    case PM_FALLEN_ANGEL:
				otmp = mongets(mtmp, LONG_SWORD, mkobjflags);
				//Note: the kits assume that the sword is otmp
				if(Inhell || goodequip){
					chance = rnd(6);
					if(chance == 6){
						//Set sword to silver
						if(otmp) set_material_gm(otmp, SILVER);
						otmp = mongets(mtmp, KITE_SHIELD, mkobjflags);
						if(otmp) set_material_gm(otmp, SILVER);
						otmp = mongets(mtmp, PLATE_MAIL, mkobjflags);
						if(otmp) set_material_gm(otmp, SILVER);
						otmp = mongets(mtmp, rn2(10) ? GAUNTLETS : GAUNTLETS_OF_POWER, mkobjflags);
						if(otmp) set_material_gm(otmp, SILVER);
						otmp = mongets(mtmp, ARMORED_BOOTS, mkobjflags);
						if(otmp) set_material_gm(otmp, SILVER);
					} else if(chance == 5){
						otmp = mongets(mtmp, ROBE, mkobjflags);
						if(otmp) set_material_gm(otmp, CLOTH);
						otmp = mongets(mtmp, GLOVES, mkobjflags);
						if(otmp) set_material_gm(otmp, CLOTH);
						otmp = mongets(mtmp, SHOES, mkobjflags);
						if(otmp) set_material_gm(otmp, CLOTH);
					} else if(chance == 4){
						if(mtmp->female){
							otmp = mongets(mtmp, GLOVES, mkobjflags);
							if(otmp) set_material_gm(otmp, CLOTH);
							otmp = mongets(mtmp, GENTLEWOMAN_S_DRESS, mkobjflags);
							if(otmp) set_material_gm(otmp, CLOTH);
							otmp = mongets(mtmp, VICTORIAN_UNDERWEAR, mkobjflags);
							if(otmp) set_material_gm(otmp, CLOTH);
							otmp = mongets(mtmp, STILETTOS, mkobjflags);
							if(otmp) set_material_gm(otmp, SILVER);
						} else {
							otmp = mongets(mtmp, GLOVES, mkobjflags);
							if(otmp) set_material_gm(otmp, LEATHER);
							otmp = mongets(mtmp, GENTLEMAN_S_SUIT, mkobjflags);
							if(otmp) set_material_gm(otmp, CLOTH);
							otmp = mongets(mtmp, RUFFLED_SHIRT, mkobjflags);
							if(otmp) set_material_gm(otmp, CLOTH);
							otmp = mongets(mtmp, SHOES, mkobjflags);
							if(otmp) set_material_gm(otmp, LEATHER);
						}
					// chance 3
					} else if(chance == 2){
						//Set sword to be filth-crusted
						if(otmp) otmp->opoisoned = OPOISON_FILTH;
						otmp = mongets(mtmp, ROBE, mkobjflags);
						if(otmp){
							set_material_gm(otmp, CLOTH);
							otmp->oeroded3 = 1;
						}
					} else if(chance == 1){
						//Set sword to black iron
						if(otmp){
							set_material_gm(otmp, IRON);
							otmp->obj_color = CLR_BLACK;
						}
						otmp = mongets(mtmp, KITE_SHIELD, mkobjflags);
						if(otmp){
							set_material_gm(otmp, IRON);
							otmp->obj_color = CLR_BLACK;
						}
						otmp = mongets(mtmp, PLATE_MAIL, mkobjflags);
						if(otmp){
							set_material_gm(otmp, IRON);
							otmp->obj_color = CLR_BLACK;
						}
						otmp = mongets(mtmp, rn2(10) ? GAUNTLETS : GAUNTLETS_OF_POWER, mkobjflags);
						if(otmp){
							set_material_gm(otmp, IRON);
							otmp->obj_color = CLR_BLACK;
						}
						otmp = mongets(mtmp, ARMORED_BOOTS, mkobjflags);
						if(otmp){
							set_material_gm(otmp, IRON);
							otmp->obj_color = CLR_BLACK;
						}
					}
				}
			break;
		    case PM_ANCIENT_OF_BLESSINGS:
				for(int i = 4; i > 0; i--){
					otmp = mksobj(SHORT_SWORD, mkobjflags);
					set_material_gm(otmp, GOLD);
					if(rn2(2)){
						bless(otmp);
					} else {
						curse(otmp);
					}
					(void) mpickobj(mtmp, otmp);
				}
			break;
		    case PM_ANCIENT_OF_ICE:
				otmp = mksobj(MACE, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, METAL);
				fix_object(otmp);
				otmp->spe = rnd(8);
				(void) mpickobj(mtmp, otmp);
				(void) mongets(mtmp, CRYSTAL_HELM, mkobjflags);
				(void) mongets(mtmp, CRYSTAL_PLATE_MAIL, mkobjflags);
				(void) mongets(mtmp, CRYSTAL_GAUNTLETS, mkobjflags);
				(void) mongets(mtmp, CRYSTAL_BOOTS, mkobjflags);
			break;
		    case PM_ANCIENT_OF_DEATH:
				otmp = mksobj(SCYTHE, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 8;
				(void) mpickobj(mtmp, otmp);
				return; //bypass general weapons
			break;
		    case PM_BALROG:
				if(In_mordor_quest(&u.uz)){
					switch(rn2(3)){
						case 0:
						(void)mongets(mtmp, MACE, mkobjflags);
						break;
						case 1:
						(void)mongets(mtmp, BROADSWORD, mkobjflags);
						break;
						case 2:
						(void)mongets(mtmp, AXE, mkobjflags);
						break;
					}
					(void)mongets(mtmp, BULLWHIP, mkobjflags);
					otmp = mksobj(SHIELD_OF_REFLECTION, mkobjflags|MKOBJ_NOINIT);
					set_material_gm(otmp, PLATINUM);
					otmp->objsize = MZ_LARGE;
					fix_object(otmp);
					mpickobj(mtmp, otmp);
				} else {
					(void)mongets(mtmp, BULLWHIP, mkobjflags);
					(void)mongets(mtmp, BROADSWORD, mkobjflags);
					return; //bypass general weapons
				}
			break;
		    case PM_NESSIAN_PIT_FIEND:
				otmp = mksobj(rn2(2) ? MACE : BATTLE_AXE, mkobjflags|MKOBJ_NOINIT);
				otmp->objsize = MZ_HUGE;
				otmp->spe = 9;
				(void) mpickobj(mtmp, otmp);
			break;
		    case PM_SHAYATEEN:
				otmp = mongets(mtmp, BATTLE_AXE, mkobjflags);
				if(otmp) otmp->spe = 6;
				otmp = mongets(mtmp, BATTLE_AXE, mkobjflags);
				if(otmp) otmp->spe = 6;
				otmp = mongets(mtmp, BATTLE_AXE, mkobjflags);
				if(otmp) otmp->spe = 6;
				otmp = mongets(mtmp, BATTLE_AXE, mkobjflags);
				if(otmp) otmp->spe = 6;
			break;
		}
		/* prevent djinnis and mail daemons from leaving objects when
		 * they vanish
		 */
		if (!is_demon(ptr) && mm != PM_DAMNED_PIRATE) break;
		/* fall thru */
/*
 *	Now the general case, Some chance of getting some type
 *	of weapon for "normal" monsters.  Certain special types
 *	of monsters will get a bonus chance or different selections.
 */
	    default:
	      {
		int bias;

		bias = is_lord(ptr) + is_prince(ptr) * 2 + extra_nasty(ptr);
		switch(rnd(14 - (2 * bias))) {
		    case 1:
			if(strongmonst(ptr)) (void) mongets(mtmp, BATTLE_AXE, mkobjflags);
			else m_initthrow(mtmp, DART, 12, mkobjflags);
			break;
		    case 2:
			if(strongmonst(ptr))
			    (void) mongets(mtmp, TWO_HANDED_SWORD, mkobjflags);
			else {
			    (void) mongets(mtmp, CROSSBOW, mkobjflags);
			    m_initthrow(mtmp, CROSSBOW_BOLT, 12, mkobjflags);
			}
			break;
		    case 3:
			(void) mongets(mtmp, BOW, mkobjflags);
			m_initthrow(mtmp, ARROW, 12, mkobjflags);
			break;
		    case 4:
			if(strongmonst(ptr)) (void) mongets(mtmp, LONG_SWORD, mkobjflags);
			else m_initthrow(mtmp, DAGGER, 3, mkobjflags);
			break;
		    case 5:
			if(strongmonst(ptr))
			    (void) mongets(mtmp, LUCERN_HAMMER, mkobjflags);
			else (void) mongets(mtmp, AKLYS, mkobjflags);
			break;
		    default:
			break;
		}
	      }
	      break;
	}
	if ((int) mtmp->m_lev > rn2(75))
		(void) mongets(mtmp, rnd_offensive_item(mtmp), mkobjflags);
}

#endif /* OVL2 */
#ifdef OVL1

#ifdef GOLDOBJ
/*
 *   Makes up money for monster's inventory.
 *   This will change with silver & copper coins
 */
void 
mkmonmoney(mtmp, amount)
struct monst *mtmp;
long amount;
{
    struct obj *gold = mksobj(GOLD_PIECE, mkobjflags|MKOBJ_NOINIT);
    gold->quan = amount;
	u.spawnedGold += gold->quan;
    add_to_minv(mtmp, gold);
}
#endif

STATIC_OVL void
m_initinv(mtmp, mkobjflags, faction, goodequip)
register struct	monst	*mtmp;
int mkobjflags;
int faction;
boolean goodequip;
{
	register int cnt;
	int chance;
	register struct obj *otmp;
	register struct permonst *ptr = mtmp->data;
#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) return;
#endif
/*
 *	Soldiers get armour & rations - armour approximates their ac.
 *	Nymphs may get mirror or potion of object detection.
 */
	switch(ptr->mlet) {

	    case S_HUMAN:
		if(is_mercenary(ptr)) {
		    register int mac;
			
		    switch(monsndx(ptr)) {
			case PM_GUARD: mac = -1; break;
#ifdef CONVICT
			case PM_PRISON_GUARD: mac = -2; break;
#endif /* CONVICT */
			case PM_SOLDIER: mac = 3; break;
			case PM_MYRMIDON_HOPLITE: mac = 3; break;
			case PM_SERGEANT: mac = 0; break;
			case PM_MYRMIDON_LOCHIAS: mac = 0; break;
			case PM_LIEUTENANT: mac = -2; break;
			case PM_MYRMIDON_YPOLOCHAGOS: mac = 0; break;
			case PM_CAPTAIN: mac = -3; break;
			case PM_MYRMIDON_LOCHAGOS: mac = -3; break;
			case PM_WATCHMAN: mac = 3; break;
			case PM_WATCH_CAPTAIN: mac = -2; break;
			default: impossible("odd mercenary %d?", monsndx(ptr));
				mac = 0;
				break;
		    }
			
			if(In_law(&u.uz) && is_army_pm(monsndx(ptr))){
				if(ptr->mtyp == PM_CAPTAIN || ptr->mtyp == PM_LIEUTENANT){
					mongets(mtmp, HARMONIUM_PLATE, mkobjflags);
					mongets(mtmp, HARMONIUM_HELM, mkobjflags);
					mongets(mtmp, HARMONIUM_GAUNTLETS, mkobjflags);
					mongets(mtmp, HARMONIUM_BOOTS, mkobjflags);
				} else {
					mongets(mtmp, HARMONIUM_SCALE_MAIL, mkobjflags);
					mongets(mtmp, HARMONIUM_HELM, mkobjflags);
					mongets(mtmp, HARMONIUM_GAUNTLETS, mkobjflags);
					mongets(mtmp, HARMONIUM_BOOTS, mkobjflags);
				}
			} else {
				if (mac < -1 && rn2(5))
					mac += arm_ac_bonus(mongets(mtmp, PLATE_MAIL, mkobjflags));
				else if (mac < 3 && rn2(5))
					mac += arm_ac_bonus(mongets(mtmp, (rn2(3)) ?
							SPLINT_MAIL : BANDED_MAIL, mkobjflags));
				else if (rn2(5))
					mac += arm_ac_bonus(mongets(mtmp, (rn2(3)) ?
							RING_MAIL : STUDDED_LEATHER_ARMOR, mkobjflags));
				else
					mac += arm_ac_bonus(mongets(mtmp, LEATHER_ARMOR, mkobjflags));

				if (mac < 10 && rn2(3))
				mac += arm_ac_bonus(mongets(mtmp, HELMET, mkobjflags));
				else if (mac < 10 && rn2(2))
				mac += arm_ac_bonus(mongets(mtmp, LEATHER_HELM, mkobjflags));
				else if (mac < 10 && !rn2(10))
				mac += arm_ac_bonus(mongets(mtmp, WAR_HAT, mkobjflags));
				
				if (mac < 10 && rn2(3))
				mac += arm_ac_bonus(mongets(mtmp, BUCKLER, mkobjflags));
				else if (mac < 10 && rn2(2))
				mac += arm_ac_bonus(mongets(mtmp, KITE_SHIELD, mkobjflags));
				else if (mac < 10 && rn2(2))
				mac += arm_ac_bonus(mongets(mtmp, ROUNDSHIELD, mkobjflags));
			
				if (mac < 10 && rn2(3))
				mac += arm_ac_bonus(mongets(mtmp, LOW_BOOTS, mkobjflags));
				else if (mac < 10 && rn2(2))
				mac += arm_ac_bonus(mongets(mtmp, HIGH_BOOTS, mkobjflags));
				else if (mac < 10 && rn2(2))
				mac += arm_ac_bonus(mongets(mtmp, ARMORED_BOOTS, mkobjflags));
			
				if (mac < 10 && rn2(3))
				mac += arm_ac_bonus(mongets(mtmp, GLOVES, mkobjflags));
				else if (mac < 10 && rn2(2))
				mac += arm_ac_bonus(mongets(mtmp, GAUNTLETS, mkobjflags));
			
				if (mac < 10 && rn2(2))
				mac += arm_ac_bonus(mongets(mtmp, CLOAK, mkobjflags));
			}
			if(ptr->mtyp != PM_GUARD &&
#ifdef CONVICT
			ptr->mtyp != PM_PRISON_GUARD &&
#endif /* CONVICT */
			ptr->mtyp != PM_WATCHMAN &&
			ptr->mtyp != PM_WATCH_CAPTAIN) {
			if(!(level.flags.has_barracks || In_law(&u.uz) || in_mklev || is_undead(ptr))){
				if (!rn2(3)) (void) mongets(mtmp, K_RATION, mkobjflags);
				if (!rn2(2)) (void) mongets(mtmp, C_RATION, mkobjflags);
			}
			if (ptr->mtyp != PM_SOLDIER && !rn2(3))
				(void) mongets(mtmp, BUGLE, mkobjflags);
			} else
			   if (ptr->mtyp == PM_WATCHMAN && rn2(3))
				(void) mongets(mtmp, WHISTLE, mkobjflags);
		} else if (ptr->mtyp == PM_SHOPKEEPER) {
		    (void) mongets(mtmp,SKELETON_KEY, mkobjflags);
			if(Infuture){
				(void) mongets(mtmp, SHOTGUN, mkobjflags);
				(void) mongets(mtmp, SHOTGUN_SHELL, mkobjflags);
				(void) mongets(mtmp, SHOTGUN_SHELL, mkobjflags);
				(void) mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
				(void) mongets(mtmp, POT_HEALING, mkobjflags);
			} else switch (rn2(6)) {
				/* MAJOR fall through ... */
				case 0: (void) mongets(mtmp, WAN_MAGIC_MISSILE, mkobjflags);
				case 1: (void) mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
				case 2: (void) mongets(mtmp, POT_HEALING, mkobjflags);
					break;
				case 3: (void) mongets(mtmp, WAN_STRIKING, mkobjflags);
				case 4: (void) mongets(mtmp, POT_HEALING, mkobjflags);
				case 5: (void) mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
		    }
		} else if (ptr->msound == MS_PRIEST ||
			quest_mon_represents_role(ptr,PM_PRIEST)
		) {
		    (void) mongets(mtmp, rn2(7) ? ROBE : rn2(3) ? CLOAK_OF_PROTECTION : CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
		    (void) mongets(mtmp, BUCKLER, mkobjflags);
			if(In_moloch_temple(&u.uz))
				give_mintrinsic(mtmp, POISON_RES);
#ifndef GOLDOBJ
		    mtmp->mgold = (long)rn1(10,20);
			u.spawnedGold += mtmp->mgold;
#else
		    mkmonmoney(mtmp,(long)rn1(10,20));
#endif
		} else if (quest_mon_represents_role(ptr,PM_MONK)) {
		    (void) mongets(mtmp, rn2(11) ? ROBE : CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
		} else if(ptr->mtyp == PM_MEDUSA){
		 struct engr *oep = engr_at(mtmp->mx,mtmp->my);
		 int i;
		 for(i=0; i<3; i++){
			if(!oep){
				make_engr_at(mtmp->mx, mtmp->my,
			     "", 0L, DUST);
				oep = engr_at(mtmp->mx,mtmp->my);
			}
			oep->ward_id = GORGONEION;
			oep->halu_ward = 0;
			oep->ward_type = BURN;
			oep->complete_wards = 1;
			rloc_engr(oep);
			oep = engr_at(mtmp->mx,mtmp->my);
		 }
		} else if(ptr->mtyp == PM_ILLURIEN_OF_THE_MYRIAD_GLIMPSES && !(u.uevent.ukilled_illurien)){
			otmp = mksobj(SPE_SECRETS, mkobjflags);
			otmp->blessed = FALSE;
			otmp->cursed = FALSE;
			(void) mpickobj(mtmp, otmp);
		} else if(is_drow(ptr)){
			if(ptr->mtyp == PM_MINDLESS_THRALL){
				struct obj *otmp;
				otmp = mksobj(DROVEN_CLOAK, mkobjflags);
				otmp->oerodeproof = FALSE;
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = -d(1,3);
				otmp->ovar1 = 1;
				(void) mpickobj(mtmp,otmp);
			}
			else if( ptr->mtyp == PM_A_GONE ){
				struct obj *otmp;
				otmp = mksobj(DROVEN_CLOAK, mkobjflags);
				otmp = oname(otmp, artiname(ART_WEB_OF_THE_CHOSEN));
				otmp->oerodeproof = FALSE;
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = -d(1,3);
				otmp->ovar1 = 3;
				(void) mpickobj(mtmp,otmp);
			}
		} else if (ptr->mtyp == PM_HUMAN_WEREWOLF){
			if (In_lost_cities(&u.uz)){
				//Yahar'gul Black Set, badly translated into nethack
				otmp = mksobj(HELMET, mkobjflags);
				set_material_gm(otmp, IRON);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);

				otmp = mksobj(LEATHER_ARMOR, mkobjflags);
				(void) mpickobj(mtmp, otmp);

				otmp = mksobj(GLOVES, mkobjflags);
				(void) mpickobj(mtmp, otmp);
			
				otmp = mksobj(HIGH_BOOTS, mkobjflags);
				(void) mpickobj(mtmp, otmp);

				otmp = mksobj(BESTIAL_CLAW, mkobjflags);
				otmp->spe = rn2(4);
				(void) mpickobj(mtmp, otmp);
				
				if (!rn2(3)){
					mtmp->mcrazed = TRUE;
					otmp = mksobj(BEAST_S_EMBRACE_GLYPH, mkobjflags);
					(void) mpickobj(mtmp, otmp);
				}
			}
		}
		
		break;
//		case S_IMP:
//			switch(monsndx(ptr)) {
//			case 
//			}
//		break;
		case S_ANT:
			if(In_law(&u.uz)){
				//Civilized ants
				if(mtmp->mtyp == PM_SOLDIER_ANT){
					chance = rnd(10);
					if(chance == 10) mongets(mtmp, PLATE_MAIL, mkobjflags);
					else if(chance >= 8) mongets(mtmp, CHAIN_MAIL, mkobjflags);
					else if(chance >= 5) mongets(mtmp, SCALE_MAIL, mkobjflags);
					if(chance >= 5) mongets(mtmp, HELMET, mkobjflags);
				} else if(mtmp->mtyp == PM_KILLER_BEE){
					if(!rn2(4)){
						mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					} else if(!rn2(3)){
						mongets(mtmp, PLAIN_DRESS, mkobjflags);
					}
				} else if(mtmp->mtyp == PM_QUEEN_BEE){
					chance = rnd(10);
					if(chance == 10) mongets(mtmp, PLATE_MAIL, mkobjflags);
					else if(chance >= 8) mongets(mtmp, SCALE_MAIL, mkobjflags);
					else if(chance >= 5) mongets(mtmp, GENTLEWOMAN_S_DRESS, mkobjflags);
					if(chance >= 8) mongets(mtmp, HELMET, mkobjflags);
				}
			}
			if(ptr->mtyp == PM_VALAVI){
				switch (rnd(2)) {
					case 1: (void) mongets(mtmp, POT_EXTRA_HEALING, mkobjflags);
					case 2: (void) mongets(mtmp, POT_HEALING, mkobjflags);
				}
				if(rn2(3)){
					mongets(mtmp, SHEPHERD_S_CROOK, mkobjflags);
					mongets(mtmp, KNIFE, mkobjflags);
					mongets(mtmp, KNIFE, mkobjflags);
				} else {
					mongets(mtmp, SCIMITAR, mkobjflags);
					switch (rnd(3)) {
						case 1:
							(void) mongets(mtmp, ROUNDSHIELD, mkobjflags);
						break;
						case 2:
							(void) mongets(mtmp, BUCKLER, mkobjflags);
						break;
						case 3:
							(void) mongets(mtmp, SCIMITAR, mkobjflags);
						break;
					}
					mongets(mtmp, SCIMITAR, mkobjflags);
					mongets(mtmp, SCIMITAR, mkobjflags);
				}
		    }
		break;
		case S_DOG:
			//Escaped war-dog
			if(mtmp->mtyp == PM_LARGE_DOG){
				chance = rnd(100);
				if(chance == 100) mongets(mtmp, PLATE_MAIL, mkobjflags);
				else if(chance >= 96) mongets(mtmp, SCALE_MAIL, mkobjflags);
				else if(chance >= 90) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				if(chance >= 96) mongets(mtmp, HELMET, mkobjflags);
			//Escaped orcish mount
			} else if(mtmp->mtyp == PM_WARG){
				chance = rnd(10);
				if(chance == 10) mongets(mtmp, ORCISH_CHAIN_MAIL, mkobjflags);
				else if(chance >= 8) mongets(mtmp, ORCISH_RING_MAIL, mkobjflags);
				else if(chance >= 5) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				if(chance >= 5) mongets(mtmp, ORCISH_HELM, mkobjflags);
				if(!rn2(20)){
					otmp = mksobj(SADDLE, mkobjflags);
					if (otmp) {
						if (mpickobj(mtmp, otmp)) break;//panic("merged saddle?");
						mtmp->misc_worn_check |= W_SADDLE;
						otmp->owornmask = W_SADDLE;
						otmp->leashmon = mtmp->m_id;
						update_mon_intrinsics(mtmp, otmp, TRUE, TRUE);
					}
				}
			} else if(mtmp->mtyp == PM_WATCHDOG_OF_THE_BOREAL_VALLEY){
				otmp = mksobj(ARMORED_BOOTS, mkobjflags|MKOBJ_NOINIT);
				otmp->objsize = MZ_HUGE;
				set_material_gm(otmp, METAL);
				add_oprop(otmp, OPROP_COLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				
				otmp = mksobj(PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
				otmp->objsize = MZ_HUGE;
				set_material_gm(otmp, METAL);
				add_oprop(otmp, OPROP_COLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				
				otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
				otmp->objsize = MZ_HUGE;
				set_material_gm(otmp, METAL);
				add_oprop(otmp, OPROP_COLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				
				otmp = mksobj(HELMET, mkobjflags|MKOBJ_NOINIT);
				otmp->objsize = MZ_HUGE;
				set_material_gm(otmp, METAL);
				add_oprop(otmp, OPROP_COLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
			} else if (ptr->mtyp == PM_WEREWOLF){
				if (In_lost_cities(&u.uz)){
					//Yahar'gul Black Set, badly translated into nethack
					otmp = mksobj(HELMET, mkobjflags);
					set_material_gm(otmp, IRON);
					(void) mpickobj(mtmp, otmp);

					otmp = mksobj(LEATHER_ARMOR, mkobjflags);
					(void) mpickobj(mtmp, otmp);

					otmp = mksobj(GLOVES, mkobjflags);
					(void) mpickobj(mtmp, otmp);
			
					otmp = mksobj(HIGH_BOOTS, mkobjflags);
					(void) mpickobj(mtmp, otmp);

					otmp = mksobj(BESTIAL_CLAW, mkobjflags);
					otmp->spe = rn2(4);
					(void) mpickobj(mtmp, otmp);
				
					if (!rn2(4)){
						mtmp->mcrazed = TRUE;
						otmp = mksobj(BEAST_S_EMBRACE_GLYPH, mkobjflags);
						(void) mpickobj(mtmp, otmp);
					}
				}
			}
			
		case S_QUADRUPED:
			//Escaped war-elephant
			if(mtmp->mtyp == PM_MUMAK){
				chance = rnd(100);
				if(chance == 100) mongets(mtmp, ARCHAIC_PLATE_MAIL, mkobjflags);
				else if(chance >= 96) mongets(mtmp, ORCISH_CHAIN_MAIL, mkobjflags);
				else if(chance >= 90) mongets(mtmp, ORCISH_RING_MAIL, mkobjflags);
				if(chance == 100) mongets(mtmp, ARCHAIC_HELM, mkobjflags);
				else if(chance >= 90) mongets(mtmp, ORCISH_HELM, mkobjflags);
				if(chance >= 95){
					otmp = mksobj(SADDLE, mkobjflags);
					if (otmp) {
						if (mpickobj(mtmp, otmp)) break;//panic("merged saddle?");
						mtmp->misc_worn_check |= W_SADDLE;
						otmp->owornmask = W_SADDLE;
						otmp->leashmon = mtmp->m_id;
						update_mon_intrinsics(mtmp, otmp, TRUE, TRUE);
					}
				}
			}
			if(mtmp->mtyp == PM_DEEP_DWELLER && Infuture){
				otmp = mongets(mtmp, BARNACLE_ARMOR, mkobjflags);
				if(otmp){
					otmp->spe = rn1(4,3);
					add_oprop(otmp, OPROP_GRES);
				}
			}
			if(mtmp->mtyp == PM_BLESSED){
				mongets(mtmp, ROBE, mkobjflags);
			}
		break;
		case S_RODENT:
			//6) There is obviously no "underground kingdom beneath London, inhabited by huge, intelligent rodents."
			if(mtmp->mtyp == PM_ENORMOUS_RAT){
				chance = rnd(100);
				if(chance == 100){
					if(mtmp->female) mongets(mtmp, GENTLEWOMAN_S_DRESS, mkobjflags);
					else mongets(mtmp, GENTLEMAN_S_SUIT, mkobjflags);
					mongets(mtmp, FEDORA, mkobjflags);
				}
				else if(chance >= 90) mongets(mtmp, JACKET, mkobjflags);
			}
		break;
		case S_SPIDER:
			//Escaped drow pet
			if(mtmp->mtyp == PM_GIANT_SPIDER){
				chance = rnd(100);
				if(chance == 100) mongets(mtmp, DROVEN_PLATE_MAIL, mkobjflags);
				else if(chance >= 96) mongets(mtmp, DROVEN_CHAIN_MAIL, mkobjflags);
				else if(chance >= 90) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				if(chance >= 96) mongets(mtmp, DROVEN_HELM, mkobjflags);
				else if(chance >= 90) mongets(mtmp, LEATHER_HELM, mkobjflags);
				if(chance >= 96) mongets(mtmp, DROVEN_CLOAK, mkobjflags);
				if(!rn2(20)){
					otmp = mksobj(SADDLE, mkobjflags);
					if (otmp) {
						if (mpickobj(mtmp, otmp)) break;//panic("merged saddle?");
						mtmp->misc_worn_check |= W_SADDLE;
						otmp->owornmask = W_SADDLE;
						otmp->leashmon = mtmp->m_id;
						update_mon_intrinsics(mtmp, otmp, TRUE, TRUE);
					}
				}
			}
		break;
		case S_UNICORN:
			//Escaped warhorse
			if(mtmp->mtyp == PM_WARHORSE){
				chance = rnd(10);
				if(chance == 10) mongets(mtmp, PLATE_MAIL, mkobjflags);
				else if(chance >= 8) mongets(mtmp, CHAIN_MAIL, mkobjflags);
				else if(chance >= 5) mongets(mtmp, SCALE_MAIL, mkobjflags);
				if(chance >= 5) mongets(mtmp, HELMET, mkobjflags);
				if(!rn2(4)){
					otmp = mksobj(SADDLE, mkobjflags);
					if (otmp) {
						if (mpickobj(mtmp, otmp)) break;//panic("merged saddle?");
						mtmp->misc_worn_check |= W_SADDLE;
						otmp->owornmask = W_SADDLE;
						otmp->leashmon = mtmp->m_id;
						update_mon_intrinsics(mtmp, otmp, TRUE, TRUE);
					}
				}
			}
		break;
		case S_BAT:
			if(Infuture){
				if(ptr->mtyp == PM_GIANT_BAT 
					|| ptr->mtyp == PM_BATTLE_BAT
					|| ptr->mtyp == PM_WARBAT
				){
					mongets(mtmp, PLASTEEL_ARMOR, mkobjflags);
				}
			}
			if(mtmp->mtyp == PM_CHIROPTERAN){
				if(Infuture){
					otmp = mongets(mtmp, ROBE, mkobjflags);
					if(otmp) otmp->obj_color = CLR_ORANGE;
					(void) mongets(mtmp, SEDGE_HAT, mkobjflags);
					if(!rn2(10)){
						otmp = mongets(mtmp, AMULET_VERSUS_CURSES, mkobjflags);
						if(otmp) set_material_gm(otmp, WOOD);
					}
				}
				else {
					chance = rnd(100);
					if(chance >= 75) mongets(mtmp, STUDDED_LEATHER_ARMOR, mkobjflags);
					else mongets(mtmp, LEATHER_ARMOR, mkobjflags);
					mongets(mtmp, LEATHER_HELM, mkobjflags);
					chance = rnd(100);
					if(chance >= 75) mongets(mtmp, SCYTHE, mkobjflags);
					else if(chance >= 50) mongets(mtmp, FAUCHARD, mkobjflags);
					else mongets(mtmp, QUARTERSTAFF, mkobjflags);
				}
			}
		break;
		case S_LIZARD:
			//Escaped drow pet
			if(mtmp->mtyp == PM_CAVE_LIZARD){
				chance = rnd(100);
				if(chance == 100) mongets(mtmp, DROVEN_PLATE_MAIL, mkobjflags);
				else if(chance >= 96) mongets(mtmp, DROVEN_CHAIN_MAIL, mkobjflags);
				else if(chance >= 80) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				if(chance >= 96) mongets(mtmp, DROVEN_HELM, mkobjflags);
				else if(chance >= 80) mongets(mtmp, LEATHER_HELM, mkobjflags);
				if(chance >= 96) mongets(mtmp, DROVEN_CLOAK, mkobjflags);
				if(!rn2(20)){
					otmp = mksobj(SADDLE, mkobjflags);
					if (otmp) {
						if (mpickobj(mtmp, otmp)) break;//panic("merged saddle?");
						mtmp->misc_worn_check |= W_SADDLE;
						otmp->owornmask = W_SADDLE;
						otmp->leashmon = mtmp->m_id;
						update_mon_intrinsics(mtmp, otmp, TRUE, TRUE);
					}
				}
			//Escaped drow mount
			} else if(mtmp->mtyp == PM_LARGE_CAVE_LIZARD){
				chance = rnd(100);
				if(chance == 98) mongets(mtmp, DROVEN_PLATE_MAIL, mkobjflags);
				else if(chance >= 90) mongets(mtmp, DROVEN_CHAIN_MAIL, mkobjflags);
				else if(chance >= 70) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				if(chance >= 90) mongets(mtmp, DROVEN_HELM, mkobjflags);
				else if(chance >= 70) mongets(mtmp, LEATHER_HELM, mkobjflags);
				if(chance >= 90) mongets(mtmp, DROVEN_CLOAK, mkobjflags);
				if(!rn2(4)){
					otmp = mksobj(SADDLE, mkobjflags);
					if (otmp) {
						if (mpickobj(mtmp, otmp)) break;//panic("merged saddle?");
						mtmp->misc_worn_check |= W_SADDLE;
						otmp->owornmask = W_SADDLE;
						otmp->leashmon = mtmp->m_id;
						update_mon_intrinsics(mtmp, otmp, TRUE, TRUE);
					}
				}
			}
		break;
	    case S_NYMPH:
		if(ptr->mtyp == PM_NIMUNE){
			(void) mongets(mtmp, GENTLEWOMAN_S_DRESS, mkobjflags);
			(void) mongets(mtmp, VICTORIAN_UNDERWEAR, mkobjflags);
			(void) mongets(mtmp, LONG_GLOVES, mkobjflags);
			(void) mongets(mtmp, STILETTOS, mkobjflags);
		} else if(Infuture && ptr->mtyp != PM_INTONER && ptr->mtyp != PM_DEMINYMPH && ptr->mtyp != PM_NEVERWAS){
			if(rn2(3)){
				(void) mongets(mtmp, ELVEN_CLOAK, mkobjflags);
				(void) mongets(mtmp, ELVEN_TOGA, mkobjflags);
				(void) mongets(mtmp, ELVEN_BOOTS, mkobjflags);
			}
			else if(rn2(3)){
				(void) mongets(mtmp, T_SHIRT, mkobjflags);
				(void) mongets(mtmp, WAISTCLOTH, mkobjflags);
				otmp = mongets(mtmp, find_gcirclet(), mkobjflags);
				if(otmp){
					set_material_gm(otmp, VEGGY);
					switch(rn2(4)){
						case 0:
							otmp->obj_color = CLR_YELLOW;
						break;
						case 1:
							otmp->obj_color = CLR_RED;
						break;
						case 2:
							otmp->obj_color = CLR_BRIGHT_MAGENTA;
						break;
						case 3:
							otmp->obj_color = CLR_BLUE;
						break;
					}
					otmp->oeroded2 = 1;
				}
			}
			else {
				(void) mongets(mtmp, HAWAIIAN_SHIRT, mkobjflags);
				(void) mongets(mtmp, HAWAIIAN_SHORTS, mkobjflags);
				(void) mongets(mtmp, EXPENSIVE_CAMERA, mkobjflags);
			}
		} else {
			if(!rn2(2)) (void) mongets(mtmp, MIRROR, mkobjflags);
			if(!rn2(2)) (void) mongets(mtmp, POT_OBJECT_DETECTION, mkobjflags);
		}
		break;
		case S_CENTAUR:
 		break;
		case S_FUNGUS:
		{
			int i = 0;
			int n = 0;
			int id = 0;
			int rng = 0;
			switch(monsndx(ptr)){
				case PM_MIGO_WORKER:
					rng = LAST_GEM-DILITHIUM_CRYSTAL;
					n = rn2(8);
					for(i=0; i<n; i++) (void)mongets(mtmp, rn2(rng)+DILITHIUM_CRYSTAL, mkobjflags);
					n = rn2(10);
					for(i=0; i<n; i++) (void)mongets(mtmp, FLINT, mkobjflags);
					n = rn2(20);
					for(i=0; i<n; i++) (void)mongets(mtmp, ROCK, mkobjflags);
				break;
				case PM_MIGO_SOLDIER:
					if(!rn2(4)){
						otmp = mksobj(WAN_LIGHTNING, mkobjflags);
						bless(otmp);
						otmp->recharged=7;
						otmp->spe = 2;
						(void) mpickobj(mtmp, otmp);
					}
					n = rn2(200)+200;
					for(i=0; i<n; i++) (void)mongets(mtmp, GOLD_PIECE, mkobjflags);
				break;
				case PM_MIGO_PHILOSOPHER:
					if(!rn2(6)){ 
						otmp = mksobj(WAN_LIGHTNING, mkobjflags);
						bless(otmp);
						otmp->recharged=7;
						otmp->spe = 3;
						(void) mpickobj(mtmp, otmp);
					}
					rng = LAST_GEM-DILITHIUM_CRYSTAL;
					n = rn2(3);
					for(i=0; i<n; i++) (void)mongets(mtmp, rn2(rng)+DILITHIUM_CRYSTAL, mkobjflags);
					rng = SCR_STINKING_CLOUD-SCR_ENCHANT_ARMOR;
					n = rn2(4)+3;
					for(i=0; i<n; i++) (void)mongets(mtmp, rn2(rng)+SCR_ENCHANT_ARMOR, mkobjflags);
				break;
				case PM_MIGO_QUEEN:
					
					otmp = mksobj(WAN_LIGHTNING, mkobjflags);
					bless(otmp);
					otmp->recharged=2;
					otmp->spe = 6;
					(void) mpickobj(mtmp, otmp);
					
					rng = LAST_GEM-DILITHIUM_CRYSTAL;
					n = rn2(10);
					for(i=0; i<n; i++) (void)mongets(mtmp, rn2(rng)+DILITHIUM_CRYSTAL, mkobjflags);
					rng = SCR_STINKING_CLOUD-SCR_ENCHANT_ARMOR;
					n = rn2(4);
					for(i=0; i<n; i++) (void)mongets(mtmp, rn2(rng)+SCR_ENCHANT_ARMOR, mkobjflags);
				break;
			}
		}
		break;
		case S_GNOME:
			if (is_gnome(ptr)) {
				if (In_mines_quest(&u.uz)) {
				/* cm: Gnomes in dark mines have candles lit. */
				    otmp = mksobj(GNOMISH_POINTY_HAT, mkobjflags);
				    (void) mpickobj(mtmp, otmp);
					if (!levl[mtmp->mx][mtmp->my].lit) {
						begin_burn(otmp);
				    }	
				}
				else//Outside the mines, only one in 6 gnomes have hats.
			    	if(!rn2(6)) (void)mongets(mtmp, GNOMISH_POINTY_HAT, mkobjflags);
			} else if(ptr->mtyp == PM_PARASITIZED_COMMANDER) {
				otmp = mksobj(PISTOL, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, SILVER);
				add_oprop(otmp, OPROP_HOLYW);
				otmp->spe = 3;
				fix_object(otmp);
				place_object(otmp, mtmp->mx, mtmp->my);	/* drop on ground */
				
				otmp = mksobj(VIBROBLADE, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, SILVER);
				otmp->objsize = MZ_SMALL;
				add_oprop(otmp, OPROP_HOLYW);
				otmp->ovar1 = 50 + d(5,10);
				otmp->spe = 3;
				fix_object(otmp);
				place_object(otmp, mtmp->mx, mtmp->my); /* drop on ground */
				
				otmp = mksobj(GENTLEWOMAN_S_DRESS, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, SILVER);
				add_oprop(otmp, OPROP_HOLY);
				add_oprop(otmp, OPROP_HOLYW);
				add_oprop(otmp, OPROP_AXIO);
				add_oprop(otmp, OPROP_AXIOW);
				add_oprop(otmp, OPROP_REFL);
				otmp->spe = 3;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(rn2(2) ? HEELED_BOOTS : STILETTOS, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, SILVER);
				add_oprop(otmp, OPROP_HOLY);
				add_oprop(otmp, OPROP_HOLYW);
				add_oprop(otmp, OPROP_AXIO);
				add_oprop(otmp, OPROP_AXIOW);
				add_oprop(otmp, OPROP_REFL);
				otmp->spe = 3;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(LONG_GLOVES, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, SILVER);
				add_oprop(otmp, OPROP_HOLY);
				add_oprop(otmp, OPROP_HOLYW);
				add_oprop(otmp, OPROP_AXIO);
				add_oprop(otmp, OPROP_AXIOW);
				add_oprop(otmp, OPROP_REFL);
				otmp->spe = 3;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			}
		break;
	    case S_GIANT:
		if (ptr->mtyp == PM_MINOTAUR) {
		    if (!rn2(3) || (in_mklev && Is_earthlevel(&u.uz)))
			(void) mongets(mtmp, WAN_DIGGING, mkobjflags);
		} else if(ptr->mtyp == PM_DEEPEST_ONE
			|| ptr->mtyp == PM_FATHER_DAGON
			|| ptr->mtyp == PM_MOTHER_HYDRA) {
			 if(on_level(&rlyeh_level,&u.uz) || ptr->mlevel>=50){
				otmp = mksobj(TRIDENT, mkobjflags);
				otmp->oerodeproof = 1;
				otmp->spe = 9;
					otmp->objsize = MZ_HUGE;
					fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(RANSEUR, mkobjflags);
				otmp->oerodeproof = 1;
				otmp->spe = 9;
					otmp->objsize = MZ_HUGE;
					fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(CROSSBOW, mkobjflags);
					otmp->objsize = MZ_HUGE;
					fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(CROSSBOW_BOLT, mkobjflags);
				otmp->oerodeproof = 1;
				otmp->quan = 18;
				otmp->owt = weight(otmp);
				otmp->spe = 9;
					otmp->objsize = MZ_HUGE;
					fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(SHIELD_OF_REFLECTION, mkobjflags);
				set_material_gm(otmp, COPPER);
				otmp->oerodeproof = 1;
				otmp->spe = 9;
					otmp->objsize = MZ_HUGE;
					fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				(void) mongets(mtmp, rnd_attack_potion(mtmp), mkobjflags);
				(void) mongets(mtmp, rnd_attack_potion(mtmp), mkobjflags);
				//Note: may be 0 if zombie
				int itm = rnd_utility_potion(mtmp);
				if(itm){
					otmp = mksobj(itm, mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = FALSE;
					(void) mpickobj(mtmp, otmp);
				}
				
				itm = rnd_utility_potion(mtmp);
				if(itm){
					otmp = mksobj(rnd_utility_potion(mtmp), mkobjflags);
					otmp->blessed = FALSE;
					otmp->cursed = FALSE;
					(void) mpickobj(mtmp, otmp);
				}
			} else if(Infuture){
				otmp = mksobj(QUARTERSTAFF, mkobjflags);
				otmp->spe = 7;
				add_oprop(otmp, OPROP_PSIOW);
				add_oprop(otmp, OPROP_PHSEW);
				add_oprop(otmp, OPROP_WATRW);
				otmp->objsize = MZ_GIGANTIC;
				set_material_gm(otmp, MINERAL);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(SNIPER_RIFLE, mkobjflags);
				otmp->objsize = MZ_HUGE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(BULLET, mkobjflags);
				otmp->oerodeproof = 1;
				otmp->quan = 18;
				otmp->owt = weight(otmp);
				otmp->spe = 7;
				otmp->objsize = MZ_HUGE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(PLATE_MAIL, mkobjflags);
				otmp->spe = 5;
				otmp->objsize = MZ_HUGE;
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				otmp = mksobj(GAUNTLETS, mkobjflags);
				otmp->spe = 5;
				otmp->objsize = MZ_HUGE;
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				otmp = mksobj(ARMORED_BOOTS, mkobjflags);
				otmp->spe = 5;
				otmp->objsize = MZ_HUGE;
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
				otmp = mksobj(AMULET_OF_LIFE_SAVING, mkobjflags);
				otmp->objsize = MZ_HUGE;
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				(void) mpickobj(mtmp,otmp);
			} else {
				switch (rn2(6)) {
					case 0:
						otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
						otmp->oerodeproof = 1;
						otmp->spe = 3;
						otmp->objsize = MZ_HUGE;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					break;
					case 1:
						otmp = mksobj(SCIMITAR, mkobjflags);
						otmp->oerodeproof = 1;
						otmp->spe = 3;
						otmp->objsize = MZ_HUGE;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					//Intentional fall-through for twoweaponing
					case 2:
						otmp = mksobj(SCIMITAR, mkobjflags);
						otmp->oerodeproof = 1;
						otmp->spe = 3;
						otmp->objsize = MZ_HUGE;
						fix_object(otmp);
						(void)mpickobj(mtmp, otmp);
					break;
					case 3:
						otmp = mksobj(TRIDENT, mkobjflags);
						otmp->oerodeproof = 1;
						otmp->spe = 3;
						otmp->objsize = MZ_HUGE;
						fix_object(otmp);
						(void) mpickobj(mtmp, otmp);
					//Intentional fall-through for twoweaponing
					case 4:
						otmp = mksobj(KNIFE, mkobjflags);
						otmp->oerodeproof = 1;
						otmp->spe = 3;
						otmp->objsize = MZ_HUGE;
						fix_object(otmp);
						(void)mpickobj(mtmp, otmp);
					break;
				}
		 }
		} else if(monsndx(ptr) == PM_TWITCHING_FOUR_ARMED_CHANGED) {
			struct obj *gem;
			otmp = mksobj(LIGHTSABER, mkobjflags);
			otmp->spe = 4;
			set_material_gm(otmp, BONE);
			otmp->ovar1 = !rn2(4) ? 38L : !rn2(3) ? 18L : rn2(2) ? 10L : 26L;
			otmp->blessed = TRUE;
			otmp->cursed = FALSE;
			fix_object(otmp);
			gem = otmp->cobj;
			obj_extract_self(gem);
			gem = poly_obj(gem,!rn2(4) ? MORGANITE : !rn2(3) ? RUBY : rn2(2) ? GARNET : JASPER);
			add_to_container(otmp, gem);
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(LIGHTSABER, mkobjflags);
			otmp->spe = 3;
			set_material_gm(otmp, BONE);
			otmp->ovar1 = !rn2(4) ? 2L : !rn2(3) ? 9L : rn2(2) ? 21L : 22L;
			otmp->blessed = TRUE;
			otmp->cursed = FALSE;
			fix_object(otmp);
			gem = otmp->cobj;
			obj_extract_self(gem);
			gem = poly_obj(gem,!rn2(3) ? EMERALD : rn2(2) ? GREEN_FLUORITE : JADE);
			add_to_container(otmp, gem);
			(void) mpickobj(mtmp, otmp);
		} else if(ptr->mtyp == PM_GIANT_GOAT_SPAWN) {
			int threshold = rnd(10)+rn2(11);
			if(mtmp->female && (In_lost_cities(&u.uz)) && u.uinsight > threshold){
				set_template(mtmp, MISTWEAVER);
				mtmp->m_insight_level = threshold;
			}
		} else if(ptr->mtyp == PM_LURKING_ONE) {
			int i;
			long long oprop = 0;
			int spe = rn2(4);
			if(!rn2(20)) switch(rnd(10)){
				case 1:
				case 2:
					oprop = OPROP_ELECW;
				break;
				case 3:
					oprop = OPROP_ACIDW;
				break;
				case 4:
				case 5:
				case 6:
					oprop = OPROP_MAGCW;
				break;
				case 7:
					oprop = OPROP_WATRW;
				break;
				case 8:
				case 9:
				case 10:
					oprop = OPROP_PSIOW;
				break;
			}
			if(!oprop && !rn2(10))
				spe = 10;
			for(i = 2; i > 0; i--){
				otmp = mksobj(CLUB, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, BONE);
				add_oprop(otmp, OPROP_CCLAW);
				add_oprop(otmp, oprop);
				otmp->objsize = mtmp->data->msize;
				otmp->spe = spe;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			}
		} else if (is_giant(ptr)) {
		    for (cnt = rn2((int)(mtmp->m_lev / 2)); cnt; cnt--) {
			otmp = mksobj(rnd_class(DILITHIUM_CRYSTAL,LUCKSTONE-1), mkobjflags|MKOBJ_NOINIT);
			otmp->quan = (long) rn1(2, 3);
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
		    }
			if(ptr->mtyp == PM_GIANT || ptr->mtyp == PM_STONE_GIANT || ptr->mtyp == PM_HILL_GIANT){
				if(!rn2(4)) mongets(mtmp, CLUB, mkobjflags);
			} else if(ptr->mtyp == PM_FIRE_GIANT){
				mongets(mtmp, CLUB, mkobjflags);
				mongets(mtmp, LEATHER_ARMOR, mkobjflags);
			} else if(ptr->mtyp == PM_FROST_GIANT){
				mongets(mtmp, BROADSWORD, mkobjflags);
				mongets(mtmp, HELMET, mkobjflags);
				mongets(mtmp, SCALE_MAIL, mkobjflags);
			} else if(ptr->mtyp == PM_STORM_GIANT){
				mongets(mtmp, TWO_HANDED_SWORD, mkobjflags);
				mongets(mtmp, HELMET, mkobjflags);
				mongets(mtmp, CHAIN_MAIL, mkobjflags);
				mongets(mtmp, GLOVES, mkobjflags);
				mongets(mtmp, SHOES, mkobjflags);
			}
		} 
		break;
	    case S_NAGA:
			if(ptr->mtyp == PM_ANCIENT_NAGA){
				mongets(mtmp, LONG_SWORD, mkobjflags);
				mongets(mtmp, LONG_SWORD, mkobjflags);
				mongets(mtmp, ARCHAIC_PLATE_MAIL, mkobjflags);
				mongets(mtmp, ARCHAIC_HELM, mkobjflags);
				mongets(mtmp, ARCHAIC_GAUNTLETS, mkobjflags);
			} else if(ptr->mtyp == PM_GUARDIAN_NAGA || ptr->mtyp == PM_GUARDIAN_NAGA_HATCHLING){
				chance = rnd(10);
				if(chance >= 7){
					mongets(mtmp, ARCHAIC_PLATE_MAIL, mkobjflags);
					mongets(mtmp, ARCHAIC_HELM, mkobjflags);
				}
			}
		break;
	    case S_WRAITH:
		if (ptr->mtyp == PM_NAZGUL) {
			otmp = mksobj(RIN_INVISIBILITY, mkobjflags|MKOBJ_NOINIT);
			curse(otmp);
			(void) mpickobj(mtmp, otmp);
		}
		break;
	    case S_LICH:
#define LichFutArmor(typ) \
				otmp = mongets(mtmp, typ, mkobjflags);\
				if(otmp){\
					set_material_gm(otmp, PLASTIC);\
					otmp->obj_color = CLR_BLACK;\
					otmp->spe = rn1(4,3);\
					add_oprop(otmp, OPROP_UNHY);\
					add_oprop(otmp, OPROP_HEAL);\
					if(is_gloves(otmp))\
						add_oprop(otmp, OPROP_UNHYW);\
					curse(otmp);\
				}
		if (ptr->mtyp == PM_DEATH_KNIGHT){
			if(Infuture){
				LichFutArmor(PLASTEEL_BOOTS);
				LichFutArmor(PLASTEEL_ARMOR);
				LichFutArmor(GAUNTLETS_OF_POWER);
				LichFutArmor(PLASTEEL_HELM);
				otmp = mongets(mtmp, BODYGLOVE, mkobjflags);
				if(otmp){
					otmp->oeroded2 = 3;
				}
				otmp = mongets(mtmp, rn2(2) ? CLOAK_OF_MAGIC_RESISTANCE : CLOAK_OF_PROTECTION, mkobjflags);
				if(otmp){
					otmp->obj_color = CLR_BLACK;
					otmp->spe = rn1(4,3);
					add_oprop(otmp, OPROP_UNHY);
					add_oprop(otmp, OPROP_DISN);
					curse(otmp);
				}
				otmp = mongets(mtmp, BEAMSWORD, mkobjflags);
				if(otmp){
					struct obj *gem;
					gem = otmp->cobj;
					obj_extract_self(gem);
					gem = poly_obj(gem,!rn2(4) ? CHUNK_OF_FOSSIL_DARK : !rn2(3) ? JET : rn2(2) ? OBSIDIAN : rn2(2) ? CATAPSI_VORTEX : ANTIMAGIC_RIFT);
					add_to_container(otmp, gem);
					otmp->spe = rn1(4,3);
					add_oprop(otmp, OPROP_AXIOW);
					uncurse(otmp);
				}
			}
			else {
				(void)mongets(mtmp, RUNESWORD, mkobjflags);
				(void)mongets(mtmp, PLATE_MAIL, mkobjflags);
			}
		} else if (ptr->mtyp == PM_BAELNORN){
			if(Infuture){
#define BaelFutArmor(typ) \
				otmp = mongets(mtmp, typ, mkobjflags);\
				if(otmp){\
					set_material_gm(otmp, MITHRIL);\
					otmp->spe = rn1(4,3);\
					add_oprop(otmp, OPROP_UNHY);\
					add_oprop(otmp, OPROP_HEAL);\
					if(is_gloves(otmp))\
						add_oprop(otmp, OPROP_UNHYW);\
					curse(otmp);\
				}
				BaelFutArmor(ELVEN_BOOTS);
				BaelFutArmor(JUMPSUIT);
				BaelFutArmor(HIGH_ELVEN_GAUNTLETS);
				BaelFutArmor(HIGH_ELVEN_HELM);
				otmp = mongets(mtmp, BODYGLOVE, mkobjflags);
				if(otmp){
					otmp->oeroded2 = 3;
				}
				otmp = mongets(mtmp, rn2(2) ? CLOAK_OF_MAGIC_RESISTANCE : CLOAK_OF_PROTECTION, mkobjflags);
				if(otmp){
					otmp->obj_color = CLR_WHITE;
					otmp->spe = rn1(4,3);
					add_oprop(otmp, OPROP_UNHY);
					add_oprop(otmp, OPROP_DISN);
					curse(otmp);
				}
#undef BaelFutArmor
			}
			else {
				mongets(mtmp, HIGH_ELVEN_HELM, mkobjflags);
				otmp = mongets(mtmp, ROBE, mkobjflags);
				if(otmp){
					set_material_gm(otmp, VEGGY);
				}
				mongets(mtmp, HIGH_ELVEN_PLATE, mkobjflags);
				mongets(mtmp, HIGH_ELVEN_GAUNTLETS, mkobjflags);
				otmp = mongets(mtmp, ELVEN_BOOTS, mkobjflags);
				if(otmp){
					set_material_gm(otmp, MITHRIL);
				}
			}
		} else if (ptr->mtyp == PM_MASTER_LICH){
			if(Infuture){
				LichFutArmor(PLASTEEL_BOOTS);
				LichFutArmor(PLASTEEL_ARMOR);
				LichFutArmor(PLASTEEL_GAUNTLETS);
				LichFutArmor(PLASTEEL_HELM);
				otmp = mongets(mtmp, BODYGLOVE, mkobjflags);
				if(otmp){
					otmp->oeroded2 = 3;
				}
				otmp = mongets(mtmp, ROBE, mkobjflags);
				if(otmp){
					otmp->obj_color = CLR_BLACK;
					otmp->spe = rn1(4,3);
					add_oprop(otmp, OPROP_UNHY);
					add_oprop(otmp, OPROP_MAGC);
					curse(otmp);
				}
			}
			else if(!rn2(13)) (void)mongets(mtmp, (rn2(7) ? ATHAME : WAN_NOTHING), mkobjflags);
		}
		else if (ptr->mtyp == PM_ARCH_LICH) {
			if(Infuture){
				LichFutArmor(PLASTEEL_BOOTS);
				LichFutArmor(PLASTEEL_ARMOR);
				LichFutArmor(PLASTEEL_GAUNTLETS);
				LichFutArmor(PLASTEEL_HELM);
				otmp = mongets(mtmp, BODYGLOVE, mkobjflags);
				if(otmp){
					otmp->oeroded2 = 3;
				}
				otmp = mongets(mtmp, ROBE, mkobjflags);
				if(otmp){
					otmp->obj_color = CLR_BLACK;
					otmp->spe = rn1(4,3);
					add_oprop(otmp, OPROP_UNHY);
					add_oprop(otmp, OPROP_MAGC);
					add_oprop(otmp, OPROP_DISN);
					curse(otmp);
				}
			}
			else if(!rn2(3)){
				otmp = mksobj(rn2(3) ? ATHAME : QUARTERSTAFF, mkobjflags|(rn2(13) ? NO_MKOBJ_FLAGS : MKOBJ_ARTIF));
				if (otmp->spe < 2) otmp->spe = rnd(3);
				if (!rn2(4)) otmp->oerodeproof = 1;
				(void) mpickobj(mtmp, otmp);
			}
		} else if(ptr->mtyp == PM_ALHOON){
			struct obj *otmp = mksobj(SKELETON_KEY, mkobjflags);
			otmp = oname(otmp, artiname(ART_SECOND_KEY_OF_NEUTRALITY));
			if(!otmp->oartifact) otmp = oname(otmp, artiname(ART_THIRD_KEY_OF_NEUTRALITY));
			otmp->blessed = FALSE;
			otmp->cursed = FALSE;
			(void) mpickobj(mtmp,otmp);
		} else if(ptr->mtyp == PM_ACERERAK){
			struct obj *otmp = mksobj(ATHAME, mkobjflags);
			otmp = oname(otmp, artiname(ART_PEN_OF_THE_VOID));
			otmp->blessed = FALSE;
			otmp->cursed = TRUE;
			otmp->spe = 5;
			(void) mpickobj(mtmp,otmp);
		} else if(ptr->mtyp == PM_DOKKALFAR_ETERNAL_MATRIARCH){
			if(Infuture){
				/*Weapon*/
				otmp = mksobj(DROVEN_GREATSWORD, mkobjflags);
				set_material_gm(otmp, SHADOWSTEEL);
				add_oprop(otmp, OPROP_WRTHW);
				add_oprop(otmp, OPROP_PHSEW);
				add_oprop(otmp, OPROP_UNHYW);
				curse(otmp);
				otmp->spe = 9;
				(void) mpickobj(mtmp, otmp);
				/*Plate Mail*/
				otmp = mksobj(DROVEN_PLATE_MAIL, mkobjflags);
				set_material_gm(otmp, SHADOWSTEEL);
				otmp->ohaluengr = TRUE;
				curse(otmp);
				otmp->spe = 9;
				otmp->oward = curhouse;
				(void) mpickobj(mtmp, otmp);
				/*Dress*/
				otmp = mksobj(PLAIN_DRESS, mkobjflags);
				curse(otmp);
				otmp->obj_color = CLR_WHITE;
				otmp->oeroded2 = 3;
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				/*Cloak*/
				otmp = mksobj(DROVEN_CLOAK, mkobjflags);
				curse(otmp);
				add_oprop(otmp, OPROP_MAGC);
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				/*Helm*/
				otmp = mksobj(FACELESS_HELM, mkobjflags);
				set_material_gm(otmp, SHADOWSTEEL);
				curse(otmp);
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				/*Gauntlets*/
				otmp = mksobj(GAUNTLETS_OF_POWER, mkobjflags);
				set_material_gm(otmp, SHADOWSTEEL);
				curse(otmp);
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				/*boots*/
				otmp = mksobj(ARMORED_BOOTS, mkobjflags);
				set_material_gm(otmp, SHADOWSTEEL);
				curse(otmp);
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				(void) mongets(mtmp, CHUNK_OF_FOSSIL_DARK, mkobjflags);
			}
			else {
				/*Weapon*/
				otmp = mksobj(HIGH_ELVEN_WARSWORD, mkobjflags);
				add_oprop(otmp, OPROP_WRTHW);
				MAYBE_MERC(otmp)
				otmp->objsize = MZ_LARGE;
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->spe = 9;
				(void) mpickobj(mtmp, otmp);
				/*Plate Mail*/
				otmp = mksobj(DROVEN_PLATE_MAIL, mkobjflags);
				set_material_gm(otmp, MITHRIL);
				otmp->obj_color = CLR_BLACK;
				otmp->ohaluengr = TRUE;
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->spe = 9;
				otmp->oward = curhouse;
				(void) mpickobj(mtmp, otmp);
				/*Dress*/
				otmp = mksobj(PLAIN_DRESS, mkobjflags);
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->oerodeproof = TRUE;
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				/*Ring*/
				otmp = mksobj(find_signet_ring(), mkobjflags);
				otmp->ohaluengr = TRUE;
				otmp->oward = curhouse;
				(void) mpickobj(mtmp, otmp);
				/*Cloak*/
				otmp = mksobj(CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->oerodeproof = TRUE;
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				/*Helm*/
				otmp = mksobj(DROVEN_HELM, mkobjflags);
				set_material_gm(otmp, MITHRIL);
				otmp->obj_color = CLR_BLACK;
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->oerodeproof = TRUE;
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				/*Gauntlets*/
				otmp = mksobj(GAUNTLETS_OF_POWER, mkobjflags);
				set_material_gm(otmp, MITHRIL);
				otmp->obj_color = CLR_BLACK;
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->oerodeproof = TRUE;
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				/*boots*/
				otmp = mksobj(ARMORED_BOOTS, mkobjflags);
				set_material_gm(otmp, MITHRIL);
				otmp->obj_color = CLR_BLACK;
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->oerodeproof = TRUE;
				otmp->spe = 3;
				(void) mpickobj(mtmp, otmp);
			}
		}
		else {
			if(Infuture){
				(void) mongets(mtmp, ARCHAIC_HELM, mkobjflags);
				(void) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				(void) mongets(mtmp, ARCHAIC_GAUNTLETS, mkobjflags);
				(void) mongets(mtmp, LOW_BOOTS, mkobjflags);
			}
		}
		break;
	    case S_YETI:
			if(ptr->mtyp == PM_GUG){
				mongets(mtmp, CLUB, mkobjflags);
			}
		break;
	    case S_MUMMY:
		if(ptr->mtyp == PM_MUMMIFIED_ANDROID || ptr->mtyp == PM_MUMMIFIED_GYNOID){
			otmp = mksobj(MUMMY_WRAPPING, mkobjflags);
			set_material_gm(otmp, LEATHER);
			fix_object(otmp);
			(void) mpickobj(mtmp, otmp);
		} else if(ptr->mtyp == PM_NITOCRIS){
			otmp = mksobj(MUMMY_WRAPPING, mkobjflags|MKOBJ_NOINIT);
			otmp = oname(otmp, artiname(ART_SPELL_WARDED_WRAPPINGS_OF_));		
			otmp->blessed = FALSE;
			otmp->cursed = TRUE;
			otmp->spe = 7;
			otmp->oerodeproof = TRUE;
			(void) mpickobj(mtmp, otmp);
		} else if(ptr->mtyp == PM_PHARAOH || ptr->mtyp == PM_PRIEST_MUMMY){
			(void)mongets(mtmp, PRAYER_WARDED_WRAPPING, mkobjflags);
		} else {
			(void)mongets(mtmp, ptr->mtyp == PM_DROW_MUMMY ? DROVEN_CLOAK : MUMMY_WRAPPING, mkobjflags);
		}
		
		if(ptr->mtyp == PM_DROW_MUMMY){
			if(!rn2(10)){
				otmp = mksobj(find_signet_ring(), mkobjflags|MKOBJ_NOINIT);
				otmp->ohaluengr = TRUE;
				otmp->oward = mtmp->mfaction;
				(void) mpickobj(mtmp, otmp);
			}
		} else if(ptr->mtyp == PM_SOLDIER_MUMMY){
			if(rn2(10)){
				mongets(mtmp, SPEAR, mkobjflags);
				otmp = mongets(mtmp, TOWER_SHIELD, mkobjflags);
				if(otmp) set_material_gm(otmp, WOOD);
			}
			else {
				mongets(mtmp, KHOPESH, mkobjflags);
				otmp = mongets(mtmp, TOWER_SHIELD, mkobjflags);
				if(otmp) set_material_gm(otmp, WOOD);
			}
			mongets(mtmp, WAISTCLOTH, mkobjflags);
		} else if(ptr->mtyp == PM_PRIEST_MUMMY){
			otmp = mongets(mtmp, !rn2(20) ? LEO_NEMAEUS_HIDE : CLOAK, mkobjflags);
			if(otmp) set_material_gm(otmp, LEATHER);
			otmp = mongets(mtmp, WAISTCLOTH, mkobjflags);
			if(otmp){
				set_material_gm(otmp, CLOTH);
				otmp->obj_color = CLR_WHITE;
			}
		} else if(ptr->mtyp == PM_PHARAOH){
			otmp = mksobj(FLAIL, mkobjflags);
			if (otmp->spe < 2) otmp->spe = rnd(3);
			set_material_gm(otmp, GOLD);
			fix_object(otmp);
			curse(otmp);
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(SHEPHERD_S_CROOK, mkobjflags);
			if (otmp->spe < 2) otmp->spe = rnd(3);
			set_material_gm(otmp, GOLD);
			otmp->objsize = MZ_SMALL;
			fix_object(otmp);
			curse(otmp);
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(MASK, mkobjflags);
			otmp->corpsenm = PM_PHARAOH;
			set_material_gm(otmp, GOLD);
			fix_object(otmp);
			curse(otmp);
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(rnd_good_amulet(), mkobjflags);
			set_material_gm(otmp, GOLD);
			fix_object(otmp);
			curse(otmp);
			(void) mpickobj(mtmp, otmp);
		} else if(ptr->mtyp == PM_NITOCRIS){
			otmp = mksobj(SCALE_MAIL, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 9;
			set_material_gm(otmp, GOLD);
			fix_object(otmp);
			(void) mpickobj(mtmp,otmp);
			
			otmp = mksobj(SHOES, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 9;
			set_material_gm(otmp, GOLD);
			fix_object(otmp);
			(void) mpickobj(mtmp,otmp);
			
			otmp = mksobj(GLOVES, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 9;
			set_material_gm(otmp, GOLD);
			fix_object(otmp);
			(void) mpickobj(mtmp,otmp);
			
			otmp = mksobj(SICKLE, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 9;
			set_material_gm(otmp, GOLD);
			fix_object(otmp);
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(SPEAR, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 9;
			otmp->objsize = MZ_SMALL;
			set_material_gm(otmp, GOLD);
			fix_object(otmp);
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(find_gcirclet() == HELM_OF_OPPOSITE_ALIGNMENT ? HELM_OF_BRILLIANCE : find_gcirclet(), mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 9;
			set_material_gm(otmp, GOLD);
			fix_object(otmp);
			(void) mpickobj(mtmp, otmp);
			
			otmp = mksobj(MASK, mkobjflags);
			otmp->corpsenm = PM_COBRA;
			set_material_gm(otmp, GOLD);
			fix_object(otmp);
			curse(otmp);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(rnd_good_amulet(), mkobjflags);
			set_material_gm(otmp, GOLD);
			fix_object(otmp);
			curse(otmp);
			(void) mpickobj(mtmp, otmp);
			return; //no random junk
		} else if(ptr->mtyp == PM_ALABASTER_MUMMY){
			otmp = mksobj(MASK, mkobjflags);
			otmp->corpsenm = PM_ALABASTER_ELF;
			set_material_gm(otmp, MINERAL);
			fix_object(otmp);
			curse(otmp);
			(void) mpickobj(mtmp, otmp);
			
			switch(rnd(6)){
				case 1:
					mtmp->mvar_syllable = SYLLABLE_OF_STRENGTH__AESH;
				break;
				case 2:
					mtmp->mvar_syllable = SYLLABLE_OF_GRACE__UUR;
				break;
				case 3:
					mtmp->mvar_syllable = SYLLABLE_OF_LIFE__HOON;
				break;
				case 4:
					mtmp->mvar_syllable = SYLLABLE_OF_SPIRIT__VAUL;
					give_mintrinsic(mtmp, DISPLACED);
				break;
				case 5:
					mtmp->mvar_syllable = SYLLABLE_OF_POWER__KRAU;
				break;
				case 6:
					mtmp->mvar_syllable = SYLLABLE_OF_THOUGHT__NAEN;
				break;
			}
		}
		break;
		case S_ZOMBIE:
			if(ptr->mtyp == PM_HEDROW_ZOMBIE && !rn2(10)){
				otmp = mksobj(find_signet_ring(), mkobjflags|MKOBJ_NOINIT);
				otmp->ohaluengr = TRUE;
				otmp->oward = mtmp->mfaction;
				(void) mpickobj(mtmp, otmp);
			} else if(ptr->mtyp == PM_HUNGRY_DEAD){
				/* create an attached blob of preserved organs. Killing the blob will kill this hungry dead */
				struct monst *blbtmp;
				if ((blbtmp = makemon(&mons[PM_BLOB_OF_PRESERVED_ORGANS], mtmp->mx, mtmp->my, MM_ADJACENTOK | MM_NOCOUNTBIRTH))) {
					/* blob created, link it */
					blbtmp->mvar_huskID = (long)mtmp->m_id;
				}
				else {
					/* blob creation failed; dramatically weaken this hungry dead to compensate */
					mtmp->mhpmax = max(1, mtmp->mhpmax/8);
				}
			}
		break;
	    case S_QUANTMECH:
		if (!rn2(20)) {
			otmp = mksobj(BOX, mkobjflags|MKOBJ_NOINIT);
			otmp->spe = 1; /* flag for special box */
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
		}
		break;
	    case S_LEPRECHAUN:
#ifndef GOLDOBJ
		mtmp->mgold = (long) d(level_difficulty(), 30);
		u.spawnedGold += mtmp->mgold;
#else
		mkmonmoney(mtmp, (long) d(level_difficulty(), 30));
#endif
		break;
		case S_LAW_ANGEL:
		case S_NEU_ANGEL:
		case S_CHA_ANGEL:
			if(ptr->mtyp == PM_WARDEN_ARCHON){
				otmp = mongets(mtmp, PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
			    if(otmp) otmp->oerodeproof = TRUE;

				otmp = mongets(mtmp, HELMET, mkobjflags|MKOBJ_NOINIT);
			    if(otmp) otmp->oerodeproof = TRUE;

				otmp = mongets(mtmp, GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
			    if(otmp) otmp->oerodeproof = TRUE;

				otmp = mongets(mtmp, SHOES, mkobjflags|MKOBJ_NOINIT);
			    if(otmp) otmp->oerodeproof = TRUE;
			} else if(ptr->mtyp == PM_SWORD_ARCHON){
				if(In_mordor_quest(&u.uz) 
					&& !In_mordor_forest(&u.uz)
					&& !Is_ford_level(&u.uz)
					&& !In_mordor_fields(&u.uz)
					&& in_mklev
				){
					(void)mongets(mtmp, SHACKLES, mkobjflags);
					mtmp->entangled = SHACKLES;
					return;
				} //else
				otmp = mongets(mtmp, rn2(3) ? ROBE : WAISTCLOTH, mkobjflags|MKOBJ_NOINIT);
				if(otmp) otmp->oerodeproof = TRUE;
			} else if(ptr->mtyp == PM_HARROWER_OF_ZARIEL){
				otmp = mksobj(rn2(3) ? ROBE : WAISTCLOTH, mkobjflags|MKOBJ_NOINIT);
				otmp->oeroded3 = 3;
				otmp->oeroded = 1;
				otmp->spe = 7;
				bless(otmp);
				(void) mpickobj(mtmp, otmp);

				otmp = mongets(mtmp, SHACKLES, MKOBJ_NOINIT);
				if(otmp){
					mtmp->misc_worn_check |= W_ARMG;
					otmp->owornmask |= W_ARMG;
					curse(otmp);
					update_mon_intrinsics(mtmp, otmp, TRUE, TRUE);
				}
				otmp = mongets(mtmp, SHACKLES, MKOBJ_NOINIT);
				if(otmp){
					mtmp->misc_worn_check |= W_ARMF;
					otmp->owornmask |= W_ARMF;
					curse(otmp);
					update_mon_intrinsics(mtmp, otmp, TRUE, TRUE);
				}
			} else if(ptr->mtyp == PM_UISCERRE_ELADRIN){
					otmp = mongets(mtmp, BANDED_MAIL, mkobjflags);
					if(otmp) set_material_gm(otmp, COPPER);
					otmp = mongets(mtmp, HELMET, mkobjflags);
					if(otmp) set_material_gm(otmp, COPPER);
			}
		break;
	    case S_DEMON:
	    	/* moved here from m_initweap() because these don't
		   have AT_WEAP so m_initweap() is not called for them */
			switch(monsndx(ptr)){
///////////////////////////////
		    case PM_KARY__THE_FIEND_OF_FIRE:
				mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				mongets(mtmp, LONG_SWORD, mkobjflags);
				mongets(mtmp, SCIMITAR, mkobjflags);
				mongets(mtmp, SCIMITAR, mkobjflags);
				mongets(mtmp, SCIMITAR, mkobjflags);
				mongets(mtmp, SCIMITAR, mkobjflags);
				mongets(mtmp, SCIMITAR, mkobjflags);
			break;
///////////////////////////////
		    case PM_CATHEZAR:
				mongets(mtmp, CHAIN, mkobjflags);
				mongets(mtmp, CHAIN, mkobjflags);
				mongets(mtmp, CHAIN, mkobjflags);
				mongets(mtmp, CHAIN, mkobjflags);
				
				mongets(mtmp, CHAIN, mkobjflags);
				mongets(mtmp, CHAIN, mkobjflags);
				mongets(mtmp, CHAIN, mkobjflags);
				
				mongets(mtmp, CHAIN, mkobjflags);
				mongets(mtmp, CHAIN, mkobjflags);
				mongets(mtmp, CHAIN, mkobjflags);
			break;
///////////////////////////////
			case PM_ALDINACH:
				otmp = mksobj(MASK, mkobjflags|MKOBJ_NOINIT);
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				otmp->corpsenm = PM_ALDINACH;
				(void) mpickobj(mtmp,otmp);
			break;
			case PM_KOSTCHTCHIE:
				otmp = mksobj(CLUB, mkobjflags);
				otmp = oname(otmp, artiname(ART_WRATHFUL_WIND));
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);
				(void)mongets(mtmp, ORIHALCYON_GAUNTLETS, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_YEENOGHU:
				otmp = mksobj(FLAIL, mkobjflags);
				otmp = oname(otmp, artiname(ART_THREE_HEADED_FLAIL));
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);
				(void)mongets(mtmp, GAUNTLETS_OF_DEXTERITY, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_BAPHOMET:
				otmp = mksartifact(ART_HEARTCLEAVER);
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);
				(void)mongets(mtmp, SPE_MAGIC_MAPPING, mkobjflags);
				(void)mongets(mtmp, SCR_MAGIC_MAPPING, mkobjflags);
				(void)mongets(mtmp, SCR_MAGIC_MAPPING, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_JUIBLEX:
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_ZUGGTMOY:
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_ALRUNES:
				if(rn2(2)){
					(void) mongets(mtmp, HELMET, mkobjflags);
					(void) mongets(mtmp, PLATE_MAIL, mkobjflags);
					(void) mongets(mtmp, GAUNTLETS_OF_POWER, mkobjflags);
					(void) mongets(mtmp, ARMORED_BOOTS, mkobjflags);
					(void) mongets(mtmp, LONG_SWORD, mkobjflags);
					(void) mongets(mtmp, KITE_SHIELD, mkobjflags);
				} else {
					(void) mongets(mtmp, CRYSTAL_BOOTS, mkobjflags);
					(void) mongets(mtmp, CRYSTAL_GAUNTLETS, mkobjflags);
					(void) mongets(mtmp, CRYSTAL_PLATE_MAIL, mkobjflags);
				}
			break;
///////////////////////////////
			case PM_ORCUS:
				otmp = mksobj(WAN_DEATH, mkobjflags);
				otmp = oname(otmp, artiname(ART_WAND_OF_ORCUS));
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);
			break;
			case PM_MALCANTHET:
				otmp = mksobj(BULLWHIP, mkobjflags|MKOBJ_NOINIT);
				otmp = oname(otmp, artiname(ART_THORNS));
				otmp->obj_color = CLR_RED;
				otmp->spe = 8;
				otmp->cursed = TRUE;
				(void) mpickobj(mtmp,otmp);
				(void) mongets(mtmp, AMULET_OF_DRAIN_RESISTANCE, mkobjflags);
				(void) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				(void) mongets(mtmp, STILETTOS, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_AVATAR_OF_LOLTH:
				otmp = mksobj(VIPERWHIP, mkobjflags);
				otmp = oname(otmp, artiname(ART_SCOURGE_OF_LOLTH));
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->spe = 8;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				/*Plate Mail*/
				otmp = mksobj(CRYSTAL_PLATE_MAIL, mkobjflags);
				otmp->bodytypeflag = (MB_HUMANOID|MB_ANIMAL);
				otmp->ohaluengr = TRUE;
				otmp->oward = LOLTH_SYMBOL;
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->spe = 7;
				(void) mpickobj(mtmp, otmp);
				/*Gauntlets*/
				otmp = mksobj(ORIHALCYON_GAUNTLETS, mkobjflags);
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->oerodeproof = TRUE;
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				/*Cloak*/
				otmp = mksobj(DROVEN_CLOAK, mkobjflags);
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->oerodeproof = TRUE;
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				/*Helm*/
				otmp = mksobj(DROVEN_HELM, mkobjflags);
				otmp->blessed = TRUE;
				otmp->cursed = FALSE;
				otmp->oerodeproof = TRUE;
				otmp->spe = 5;
				(void) mpickobj(mtmp, otmp);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_GRAZ_ZT:
				otmp = mksobj(TWO_HANDED_SWORD, mkobjflags);
				otmp = oname(otmp, artiname(ART_DOOMSCREAMER));
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);
				(void) mongets(mtmp, AMULET_OF_REFLECTION, mkobjflags);
				(void) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
///////////////////////////////
			case PM_BAEL:
				otmp = mksobj(FLAIL, mkobjflags|MKOBJ_NOINIT);
				add_oprop(otmp, OPROP_FLAYW);
				set_material_gm(otmp, GOLD);
				otmp->spe = 6;
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(SCIMITAR, mkobjflags|MKOBJ_NOINIT);
				add_oprop(otmp, OPROP_FLAYW);
				set_material_gm(otmp, GOLD);
				otmp->spe = 6;
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(SABER, mkobjflags|MKOBJ_NOINIT);
				add_oprop(otmp, OPROP_FLAYW);
				set_material_gm(otmp, GOLD);
				otmp->spe = 6;
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(SHORT_SWORD, mkobjflags|MKOBJ_NOINIT);
				add_oprop(otmp, OPROP_FLAYW);
				set_material_gm(otmp, GOLD);
				otmp->spe = 6;
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(SCALPEL, mkobjflags|MKOBJ_NOINIT);
				add_oprop(otmp, OPROP_FLAYW);
				set_material_gm(otmp, GOLD);
				otmp->spe = 6;
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(STILETTO, mkobjflags|MKOBJ_NOINIT);
				add_oprop(otmp, OPROP_FLAYW);
				set_material_gm(otmp, GOLD);
				otmp->spe = 6;
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
				otmp = mongets(mtmp, GAUNTLETS_OF_POWER, mkobjflags);
				if(otmp) set_material_gm(otmp, COPPER);
				otmp = mongets(mtmp, PLATE_MAIL, mkobjflags);
				if(otmp){
					set_material_gm(otmp, COPPER);
					otmp->spe = 9;
				}

				otmp = mksobj(TWO_HANDED_SWORD, mkobjflags|MKOBJ_NOINIT);
				otmp = oname(otmp, artiname(ART_GENOCIDE));
				otmp->spe = 9;
				curse(otmp);
				otmp->oerodeproof = TRUE;
				(void) mpickobj(mtmp, otmp);
			break;
		    case PM_DISPATER:
//				(void)mongets(mtmp, WAN_STRIKING, mkobjflags);
//				(void) mongets(mtmp, MACE, mkobjflags);
				otmp = mksobj(MACE, mkobjflags);
				otmp = oname(otmp, artiname(ART_ROD_OF_DIS));		
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 2;
				otmp->oerodeproof = TRUE;
				(void) mpickobj(mtmp, otmp);
				(void) mongets(mtmp, RUFFLED_SHIRT, mkobjflags);
				(void) mongets(mtmp, GENTLEMAN_S_SUIT, mkobjflags);
				(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
				(void) mongets(mtmp, GLOVES, mkobjflags);
				(void) mongets(mtmp, SPE_CHARM_MONSTER, mkobjflags);
				(void) mongets(mtmp, SCR_TAMING, mkobjflags);
				(void) mongets(mtmp, SCR_TAMING, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
		    case PM_MAMMON:
//				(void)mongets(mtmp, WAN_STRIKING, mkobjflags);
				otmp = mksobj(SHORT_SWORD, mkobjflags);
				otmp = oname(otmp, artiname(ART_AVARICE));		
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 3;
				otmp->oerodeproof = TRUE;
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(LEATHER_ARMOR, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, DRAGON_HIDE);
				otmp->bodytypeflag = (ptr->mflagsb&MB_BODYTYPEMASK);
				otmp->objsize = MZ_LARGE;
				otmp->cursed = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(LEATHER_HELM, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, DRAGON_HIDE);
				otmp->objsize = MZ_LARGE;
				otmp->cursed = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, DRAGON_HIDE);
				otmp->objsize = MZ_LARGE;
				otmp->cursed = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				(void) mongets(mtmp, POT_PARALYSIS, mkobjflags);
				(void) mongets(mtmp, SPE_DETECT_TREASURE, mkobjflags);
				(void) mongets(mtmp, SCR_GOLD_DETECTION, mkobjflags);
				(void) mongets(mtmp, SCR_GOLD_DETECTION, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_FIERNA:
				(void)mongets(mtmp, WAN_DIGGING, mkobjflags);
				(void) mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				otmp = mksobj(CHAIN, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, GOLD);
				otmp->cursed = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_NAOME:
				otmp = mksobj(ARCHAIC_HELM, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 4;
				curse(otmp);
				otmp->objsize = mtmp->data->msize;
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(ARCHAIC_PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 4;
				curse(otmp);
				otmp->objsize = mtmp->data->msize;
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(BUCKLER, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 4;
				curse(otmp);
				otmp->objsize = mtmp->data->msize;
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_BELIAL:
				otmp = mksobj(TRIDENT, mkobjflags);
				otmp = oname(otmp, artiname(ART_FIRE_OF_HEAVEN));		
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 4;
				otmp->oerodeproof = TRUE;
				(void) mpickobj(mtmp, otmp);
				(void)mongets(mtmp, WAN_DIGGING, mkobjflags);
				(void)mongets(mtmp, RUFFLED_SHIRT, mkobjflags);
				(void)mongets(mtmp, JACKET, mkobjflags);
				(void) mongets(mtmp, HIGH_BOOTS, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
////////////////////////////////////////
			case PM_LEVIATHAN:
				otmp = mksobj(DUNCE_CAP, mkobjflags);
				otmp = oname(otmp, artiname(ART_DIADEM_OF_AMNESIA));		
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 9;
				otmp->bodytypeflag = MB_LONGHEAD;
				otmp->objsize = MZ_HUGE;
				otmp->oerodeproof = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				(void)mongets(mtmp, WAN_CANCELLATION, mkobjflags);
				(void)mongets(mtmp, POT_AMNESIA, mkobjflags);
				(void)mongets(mtmp, POT_AMNESIA, mkobjflags);
				(void)mongets(mtmp, POT_AMNESIA, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_LEVISTUS:
				otmp = mksobj(RAPIER, mkobjflags|MKOBJ_NOINIT);
				otmp = oname(otmp, artiname(ART_SHADOWLOCK));		
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 4;
				otmp->oerodeproof = TRUE;
				(void) mpickobj(mtmp, otmp);
				(void)mongets(mtmp, RUFFLED_SHIRT, mkobjflags);
				(void)mongets(mtmp, GENTLEMAN_S_SUIT, mkobjflags);
				(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
				(void)mongets(mtmp, GLOVES, mkobjflags);
				(void)mongets(mtmp, CLOAK_OF_PROTECTION, mkobjflags);
				(void)mongets(mtmp, WAN_CANCELLATION, mkobjflags);
				(void)mongets(mtmp, POT_AMNESIA, mkobjflags);
				(void)mongets(mtmp, POT_AMNESIA, mkobjflags);
				(void)mongets(mtmp, POT_AMNESIA, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_DAUGHTER_LILITH:
				(void)mongets(mtmp, OILSKIN_CLOAK, mkobjflags);
				otmp = mksobj(DAGGER, mkobjflags);
				otmp = oname(otmp, artiname(ART_THUNDER_S_VOICE));
				set_material_gm(otmp, SILVER);
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 6;
				otmp->oerodeproof = TRUE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_MOTHER_LILITH:
				(void)mongets(mtmp, CLOAK_OF_MAGIC_RESISTANCE, mkobjflags);
				otmp = mksobj(ATHAME, mkobjflags);
				otmp = oname(otmp, artiname(ART_SERPENT_S_TOOTH));		
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 6;
				otmp->oerodeproof = TRUE;
				(void) mpickobj(mtmp, otmp);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_CRONE_LILITH:
				(void)mongets(mtmp, BOULDER, mkobjflags);
				(void)mongets(mtmp, BOULDER, mkobjflags);
				(void)mongets(mtmp, BOULDER, mkobjflags);
				(void)mongets(mtmp, QUARTERSTAFF, mkobjflags);
				otmp = mksobj(UNICORN_HORN, mkobjflags);
				otmp = oname(otmp, artiname(ART_UNBLEMISHED_SOUL));		
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 6;
				otmp->oerodeproof = TRUE;
				(void) mpickobj(mtmp, otmp);
				(void)mongets(mtmp, CRYSTAL_BALL, mkobjflags);
				(void)mongets(mtmp, ROBE, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_BAALZEBUB:
				otmp = mksobj(LONG_SWORD, mkobjflags);
				otmp = oname(otmp, artiname(ART_WRATH_OF_HEAVEN));		
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 7;
				otmp->oerodeproof = TRUE;
				(void) mpickobj(mtmp, otmp);

				otmp = mksobj(HELM_OF_TELEPATHY, mkobjflags);
				otmp = oname(otmp, artiname(ART_ALL_SEEING_EYE_OF_THE_FLY));		
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 7;
				otmp->oerodeproof = TRUE;
				set_material_gm(otmp, GOLD);
				otmp->objsize = MZ_LARGE;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(ARCHAIC_PLATE_MAIL, mkobjflags|MKOBJ_NOINIT);
				otmp->cursed = TRUE;
				set_material_gm(otmp, GOLD);
				otmp->objsize = MZ_LARGE;
				otmp->spe = 7;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(ARCHAIC_GAUNTLETS, mkobjflags|MKOBJ_NOINIT);
				otmp->cursed = TRUE;
				set_material_gm(otmp, GOLD);
				otmp->objsize = MZ_LARGE;
				otmp->spe = 7;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
				otmp = mksobj(ARCHAIC_BOOTS, mkobjflags|MKOBJ_NOINIT);
				otmp->cursed = TRUE;
				set_material_gm(otmp, GOLD);
				otmp->objsize = MZ_LARGE;
				otmp->spe = 7;
				fix_object(otmp);
				(void) mpickobj(mtmp, otmp);
			break;
			case PM_MEPHISTOPHELES:
				otmp = mksobj(RANSEUR, mkobjflags);
				otmp = oname(otmp, artiname(ART_COLD_SOUL));
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 8;
				otmp->oerodeproof = TRUE;
				(void) mpickobj(mtmp, otmp);
				(void)mongets(mtmp, HELM_OF_BRILLIANCE, mkobjflags);
				(void)mongets(mtmp, ROBE, mkobjflags);
				(void)mongets(mtmp, RUFFLED_SHIRT, mkobjflags);
				(void)mongets(mtmp, GENTLEMAN_S_SUIT, mkobjflags);
				(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
				(void)mongets(mtmp, GLOVES, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
			case PM_BAALPHEGOR:
				otmp = mksobj(QUARTERSTAFF, mkobjflags);
				otmp = oname(otmp, artiname(ART_SCEPTRE_OF_THE_FROZEN_FLOO));		
				otmp->blessed = FALSE;
				otmp->cursed = TRUE;
				otmp->spe = 8;
				otmp->oerodeproof = TRUE;
				(void) mpickobj(mtmp, otmp);
				(void)mongets(mtmp, AMULET_OF_REFLECTION, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
/////////////////////////////////////////
			case PM_ASMODEUS:
//				(void)mongets(mtmp, WAN_COLD, mkobjflags);
//				(void)mongets(mtmp, WAN_FIRE, mkobjflags);
				otmp = mongets(mtmp, ROBE, mkobjflags);
				if(otmp) otmp->obj_color = CLR_BLACK;
				otmp = mongets(mtmp, GENTLEMAN_S_SUIT, mkobjflags);
				if(otmp) otmp->obj_color = CLR_BLACK;
				otmp = mongets(mtmp, SPEED_BOOTS, mkobjflags);
				if(otmp) otmp->obj_color = CLR_BLACK;
				(void)mongets(mtmp, SCR_CHARGING, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
				// (void) mongets(mtmp, POT_FULL_HEALING, mkobjflags);
			break;
////////////////////////////////////////
		    case PM_GOOD_NEIGHBOR:
				(void)mongets(mtmp, PLAIN_DRESS, mkobjflags);
				(void)mongets(mtmp, LEATHER_ARMOR, mkobjflags);
				(void)mongets(mtmp, ROBE, mkobjflags);
				(void)mongets(mtmp, HIGH_BOOTS, mkobjflags);
				(void)mongets(mtmp, WITCH_HAT, mkobjflags);
			break;
////////////////////////////////////////
		    case PM_HMNYW_PHARAOH:
				otmp = mksobj(SICKLE, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 9;
				set_material_gm(otmp, METAL);
				otmp->objsize = MZ_LARGE;
				fix_object(otmp);
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(QUARTERSTAFF, mkobjflags|MKOBJ_NOINIT);
				otmp->spe = 9;
				set_material_gm(otmp, METAL);
				otmp->objsize = MZ_SMALL;
				fix_object(otmp);
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
			break;
////////////////////////////////////////
		    case PM_LUNGORTHIN:
				otmp = mksobj(BULLWHIP, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, MINERAL);
				add_oprop(otmp, OPROP_FIREW);
				add_oprop(otmp, OPROP_LESSER_UNHYW);
				otmp->objsize = MZ_GIGANTIC;
				otmp->spe = 4;
				curse(otmp);
				fix_object(otmp);
				mpickobj(mtmp, otmp);
				
				otmp = mksobj(BATTLE_AXE, mkobjflags|MKOBJ_NOINIT);
				otmp = oname(otmp, artiname(ART_HEART_OF_SHADOW));
				add_oprop(otmp, OPROP_UNHYW);
				otmp->spe = 4;
				curse(otmp);
				fix_object(otmp);
				mpickobj(mtmp, otmp);
				
				otmp = mksobj(SKELETON_KEY, mkobjflags);
				otmp = oname(otmp, artiname(ART_THIRD_KEY_OF_CHAOS));
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);

			break;
////////////////////////////////////////
		    case PM_DURIN_S_BANE:
				(void)mongets(mtmp, BULLWHIP, mkobjflags);
				otmp = mksobj(BULLWHIP, mkobjflags|MKOBJ_NOINIT);
				set_material_gm(otmp, IRON);
				fix_object(otmp);
				curse(otmp);
				otmp->spe = 9;
				(void) mpickobj(mtmp, otmp);
				(void)mongets(mtmp, POT_SPEED, mkobjflags);
				(void)mongets(mtmp, POT_PARALYSIS, mkobjflags);
				return; //bypass general weapons
			break;
////////////////////////////////////////
			case PM_CHAOS:
				{
				struct	monst *mlocal;
				/* create special stuff; can't use mongets */
				// otmp = mksobj(CRYSTAL_BALL, mkobjflags);
				// otmp = oname(otmp, artiname(ART_BLACK_CRYSTAL));
				// bless(otmp);
				// otmp->oerodeproof = TRUE;
				// (void) mpickobj(mtmp, otmp);

				otmp = mksobj(SKELETON_KEY, mkobjflags);
				otmp = oname(otmp, artiname(ART_THIRD_KEY_OF_CHAOS));
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);

				// mlocal = makemon(&mons[PM_KRAKEN], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				// otmp = mksobj(CRYSTAL_BALL, mkobjflags);
				// otmp = oname(otmp, artiname(ART_WATER_CRYSTAL));		
				// bless(otmp);
				// otmp->oerodeproof = TRUE;
				// if(mlocal) (void) mpickobj(mlocal, otmp);

				// mlocal = makemon(&mons[PM_MARILITH], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				// otmp = mksobj(CRYSTAL_BALL, mkobjflags);
				// otmp = oname(otmp, artiname(ART_FIRE_CRYSTAL));		
				// bless(otmp);
				// otmp->oerodeproof = TRUE;
				// if(mlocal) (void) mpickobj(mlocal, otmp);

				// mlocal = makemon(&mons[PM_TIAMAT], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				// otmp = mksobj(CRYSTAL_BALL, mkobjflags);
				// otmp = oname(otmp, artiname(ART_AIR_CRYSTAL));		
				// bless(otmp);
				// otmp->oerodeproof = TRUE;
				// if(mlocal) (void) mpickobj(mlocal, otmp);

				// mlocal = makemon(&mons[PM_LICH], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				// otmp = mksobj(CRYSTAL_BALL, mkobjflags);
				// otmp = oname(otmp, artiname(ART_EARTH_CRYSTAL));		
				// bless(otmp);
				// otmp->oerodeproof = TRUE;
				// if(mlocal) (void) mpickobj(mlocal, otmp);
				}
			break;
			case PM_GREAT_CTHULHU:
//				otmp = mksobj(UNIVERSAL_KEY, mkobjflags);
//				otmp = oname(otmp, artiname(ART_SILVER_KEY));
//				otmp->blessed = FALSE;
//				otmp->cursed = FALSE;
//				(void) mpickobj(mtmp,otmp);
			break;

			}
		break;
	    case S_VAMPIRE:
			if(ptr->mtyp == PM_PARAI){
				mongets(mtmp, PLAIN_DRESS, mkobjflags);
			}
			else switch(rn2(6)) {
				case 1:
				(void)mongets(mtmp, POT_BLOOD, mkobjflags);
				case 2:
				(void)mongets(mtmp, POT_BLOOD, mkobjflags);
				case 3:
				(void)mongets(mtmp, POT_BLOOD, mkobjflags);
				case 4:
				(void)mongets(mtmp, POT_BLOOD, mkobjflags);
				default:
				break;
			}
		break;
		case S_EYE:
			if(ptr->mtyp == PM_AXUS){
				struct obj *otmp = mksobj(SKELETON_KEY, mkobjflags);
				otmp = oname(otmp, artiname(ART_FIRST_KEY_OF_LAW));
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);
			}
		break;
		case S_GOLEM:
			if(ptr->mtyp == PM_ARSENAL){
				struct obj *otmp = mksobj(SKELETON_KEY, mkobjflags);
				otmp = oname(otmp, artiname(ART_SECOND_KEY_OF_LAW));
				otmp->blessed = FALSE;
				otmp->cursed = FALSE;
				(void) mpickobj(mtmp,otmp);
			}
		break;
	    default:
		break;
	}

	/* Your saddled steeds are waiting */
	if(Race_if(PM_DROW) && in_mklev && Is_qstart(&u.uz) && (ptr->mtyp == PM_SPROW || ptr->mtyp == PM_DRIDER || ptr->mtyp == PM_CAVE_LIZARD || ptr->mtyp == PM_LARGE_CAVE_LIZARD)){
		struct obj *otmp = mksobj(SADDLE, mkobjflags);
		if (!get_mx(mtmp, MX_EDOG))
			add_mx(mtmp, MX_EDOG);
		initedog(mtmp);
		EDOG(mtmp)->loyal = TRUE;
		if (otmp) {
			if (mpickobj(mtmp, otmp)) panic("merged saddle?");
			mtmp->misc_worn_check |= W_SADDLE;
			otmp->dknown = otmp->bknown = otmp->rknown = 1;
			otmp->owornmask = W_SADDLE;
			otmp->leashmon = mtmp->m_id;
			update_mon_intrinsics(mtmp, otmp, TRUE, TRUE);
		}
		if(ptr->mtyp == PM_SPROW || ptr->mtyp == PM_DRIDER){
			otmp = mksobj(DROVEN_PLATE_MAIL, mkobjflags);
			otmp->oward = (long)u.start_house;
			otmp->oerodeproof = TRUE;
			otmp->bodytypeflag = (ptr->mflagsb&MB_BODYTYPEMASK);
			otmp->blessed = FALSE;
			otmp->cursed = TRUE;
			otmp->spe = 3;
			(void) mpickobj(mtmp,otmp);
			otmp = mksobj(DROVEN_HELM, mkobjflags);
			otmp->oerodeproof = TRUE;
			otmp->blessed = FALSE;
			otmp->cursed = TRUE;
			otmp->spe = 3;
			(void) mpickobj(mtmp,otmp);
			otmp = mksobj(DROVEN_CLOAK, mkobjflags);
			otmp->oerodeproof = TRUE;
			otmp->blessed = FALSE;
			otmp->cursed = TRUE;
			otmp->spe = 3;
			(void) mpickobj(mtmp,otmp);
		}
	}

	/* ordinary soldiers rarely have access to magic (or gold :-) */
	if (ptr->mtyp == PM_SOLDIER && rn2(13)) return;

	if ((int) mtmp->m_lev > rn2(50))
		(void) mongets(mtmp, rnd_defensive_item(mtmp), mkobjflags);
	if ((int) mtmp->m_lev > rn2(100))
		(void) mongets(mtmp, rnd_misc_item(mtmp), mkobjflags);
#ifndef GOLDOBJ
	if (likes_gold(ptr) && !mtmp->mgold && !rn2(5))
		mtmp->mgold =
		      (long) d(level_difficulty(), mtmp->minvent ? 5 : 10);
#else
	if (likes_gold(ptr) && !findgold(mtmp->minvent) && !rn2(5))
		mkmonmoney(mtmp, (long) d(level_difficulty(), mtmp->minvent ? 5 : 10));
#endif
}

/* Note: for long worms, always call cutworm (cutworm calls clone_mon) */
struct monst *
clone_mon(mon, x, y)
struct monst *mon;
xchar x, y;	/* clone's preferred location or 0 (near mon) */
{
	coord mm;
	struct monst *m2;

	/* may be too weak or have been extinguished for population control */
	if (mon->mhp <= 1 || (mvitals[monsndx(mon->data)].mvflags & G_EXTINCT && !In_quest(&u.uz)))
	    return (struct monst *)0;

	if (x == 0) {
	    mm.x = mon->mx;
	    mm.y = mon->my;
	    if (!enexto(&mm, mm.x, mm.y, mon->data) || MON_AT(mm.x, mm.y))
		return (struct monst *)0;
	} else if (!isok(x, y)) {
	    return (struct monst *)0;	/* paranoia */
	} else {
	    mm.x = x;
	    mm.y = y;
	    if (MON_AT(mm.x, mm.y)) {
		if (!enexto(&mm, mm.x, mm.y, mon->data) || MON_AT(mm.x, mm.y))
		    return (struct monst *)0;
	    }
	}
	m2 = malloc(sizeof(struct monst));
	*m2 = *mon;			/* copy condition of old monster */
	m2->nmon = fmon;
	fmon = m2;
	m2->m_id = flags.ident++;
	if (!m2->m_id) m2->m_id = flags.ident++;	/* ident overflowed */
	m2->mx = mm.x;
	m2->my = mm.y;

	m2->minvent = (struct obj *) 0; /* objects don't clone */
	m2->ispolyp = FALSE; //real
	m2->mclone = TRUE; //no death drop
	m2->mleashed = FALSE;
#ifndef GOLDOBJ
	m2->mgold = 0L;
#endif
	/* Max HP the same, but current HP halved for both.  The caller
	 * might want to override this by halving the max HP also.
	 * When current HP is odd, the original keeps the extra point.
	 */
	m2->mhpmax = mon->mhpmax;
	m2->mhp = mon->mhp / 2;
	mon->mhp -= m2->mhp;

	/* duplicate timers */
	if (mon->timed) {
		m2->timed = (struct timer *) 0;
		split_timers(mon->timed, TIMER_MONSTER, (genericptr_t)m2);
	}

	/* place the monster -- we want to do this before any display things happen */
	place_monster(m2, m2->mx, m2->my);

	/* since shopkeepers and guards will only be cloned if they've been
	 * polymorphed away from their original forms, the clone doesn't have
	 * room for the extra information.  we also don't want two shopkeepers
	 * around for the same shop.
	 */
	/* handle all mextra fields */
	m2->mextra_p = (union mextra *)0;	/* needs its own */
	/* DO copy these */
	cpy_mx(mon, m2, MX_EDOG);
	cpy_mx(mon, m2, MX_EHOR);
	cpy_mx(mon, m2, MX_EMIN);
	cpy_mx(mon, m2, MX_ENAM);
	/* DON'T copy these */
	m2->isshk = FALSE;		// MX_ESHK
	m2->ispriest = FALSE;	// MX_EPRI
	m2->isgd = FALSE;		// MX_EVGD

	/* handle monster lightsources */
	if (emits_light_mon(m2))
	    new_light_source(LS_MONSTER, (genericptr_t)m2, emits_light_mon(m2));

	/* not all clones caused by player are tame or peaceful */
	if (!flags.mon_moving) {
	    if (mon->mtame) {
			m2->mtame = rn2(max(2 + u.uluck, 2)) ? mon->mtame : 0;
			if (!m2->mtame)
				untame(m2, 1);
		}
	    else if (mon->mpeaceful)
			m2->mpeaceful = rn2(max(2 + u.uluck, 2)) ? 1 : 0;
	}
	set_malign(m2);

	/* display the new monster */
	newsym(m2->mx,m2->my);
	return m2;
}

/* cloneu()
 * 
 * clones the player, making a tame creature
 */
struct monst *
cloneu()
{
	register struct monst *mon;
	int mndx = monsndx(youracedata);

	if (u.mh <= 1) return(struct monst *)0;
	if (mvitals[mndx].mvflags & G_EXTINCT && !In_quest(&u.uz)) return(struct monst *)0;
	mon = makemon(youracedata, u.ux, u.uy, NO_MINVENT | MM_EDOG | MM_NOGROUP);
	if (mon) {
		mon = christen_monst(mon, plname);
		initedog(mon);
		mon->m_lev = youracedata->mlevel;
		mon->mhpmax = u.mhmax;
		mon->mhp = u.mh / 2;
		mon->mclone = 1;
		u.mh -= mon->mhp;
		flags.botl = 1;
	}
	return(mon);
}


/*
 * Propagate a species
 *
 * Once a certain number of monsters are created, don't create any more
 * at random (i.e. make them extinct).  The previous (3.2) behavior was
 * to do this when a certain number had _died_, which didn't make
 * much sense.
 *
 * Returns FALSE propagation unsuccessful
 *         TRUE  propagation successful
 */
boolean
propagate(mndx, tally, ghostly)
int mndx;
boolean tally;
boolean ghostly;
{
	boolean result;
	uchar lim = mbirth_limit(mndx);
	boolean gone = (mvitals[mndx].mvflags & (tally ? G_GENOD : G_GONE)) && !In_quest(&u.uz); /* genocided or extinct */

	result = (((int) mvitals[mndx].born < lim) && !gone) ? TRUE : FALSE;

	/* if it's unique, don't ever make it again */
	if (mons[mndx].geno & G_UNIQ) mvitals[mndx].mvflags |= G_EXTINCT;

	if (mvitals[mndx].born < 255 && tally && (!ghostly || (ghostly && result)))
		 mvitals[mndx].born++;
	if ((int) mvitals[mndx].born >= lim && !(mons[mndx].geno & G_NOGEN) &&
		!(mvitals[mndx].mvflags & G_EXTINCT && !In_quest(&u.uz))) {
#if defined(DEBUG) && defined(WIZARD)
		if (wizard) pline("Automatically extinguished %s.",
					makeplural(mons[mndx].mname));
#endif
		mvitals[mndx].mvflags |= G_EXTINCT;
		reset_rndmonst(mndx);
	}
	return result;
}

/* 
 * For a given ptr, attempt to give it a template and/or faction.
 * If template/faction are not -1, use given. If they are -1, determine appropriate.
 * 
 * Returns template applied.
 */
int
makemon_set_template(ptrptr, template, randmonst)
struct permonst ** ptrptr;	/* standard ptr, but one layer referenced so that we can change what ptr is for the caller */
int template;
boolean randmonst;
{
	struct permonst * ptr = *ptrptr;
	int mkmon_template = 0;

	if (template != -1) {
		mkmon_template = template;
	}
	else {
		/* most general case is at the end of the if-else chain */
		/* `zombiepm` or `skeletpm` are set to deliberately make a certain pm into zombies/skeletons */
		if (zombiepm >=0 && ptr->mtyp == zombiepm) {
			mkmon_template = ZOMBIFIED;
		}
		else if (skeletpm >=0 && ptr->mtyp == skeletpm) {
			mkmon_template = SKELIFIED;
		}
		/* in the Elf quest against the Necromancer, the "PM_ELF" enemies are meant to be groups of elf zombies. */
		else if(In_quest(&u.uz) && urole.neminum == PM_NECROMANCER && ptr->mtyp == PM_ELF){
			mkmon_template = ZOMBIFIED;
		}
		/* Echo is always given the skeleton template */
		else if(ptr->mtyp == PM_ECHO){
			mkmon_template = SKELIFIED;
		}
		/* in Cania, creatures that aren't native to Gehennom are vitrified */ 
		else if((ptr->geno&G_HELL) == 0 && Is_mephisto_level(&u.uz)){
			mkmon_template = CRYSTALFIED;
		}
		/* Kamerel tend to be fractured */ 
		else if(is_kamerel(ptr)){
			if(level.flags.has_kamerel_towers && (ptr->mtyp != PM_ARA_KAMEREL || rn2(2))){
				mkmon_template = FRACTURED;
			} else if(!level.flags.has_minor_spire && ptr->mtyp != PM_ARA_KAMEREL
				&& (ptr->mtyp != PM_HUDOR_KAMEREL || rn2(2))
				&& (ptr->mtyp != PM_SHARAB_KAMEREL || !rn2(4))
			){
				mkmon_template = FRACTURED;
			}
		}
		/* on the Plane of Earth, Mahadevae are Worldshapers, capable of travelling through the rock */
		else if(Is_earthlevel(&u.uz) && ptr->mtyp == PM_MAHADEVA) {
			mkmon_template = WORLD_SHAPER;
		}
		/* insight check: making pseudonatural creatures out of anything reasonable */
		else if(randmonst && can_undead(ptr) && check_insight()){
			mkmon_template = PSEUDONATURAL;
		}
		/* insight check: adding brains to rats */
		else if(randmonst && is_rat(ptr) && check_insight()){
			mkmon_template = CRANIUM_RAT;
		}
		/* insight check: making yith */
		else if(randmonst && check_insight() && (level_difficulty()+u.ulevel)/2+5 > monstr[PM_DEMILICH]){
			mkmon_template = YITH;
		}
		/* most general case at bottom -- creatures randomly being zombified */
		else if(randmonst && can_undead(ptr) && !Is_rogue_level(&u.uz)
			&& !Infuture
		){
			if(In_mines(&u.uz)){
				if(Race_if(PM_GNOME) && Role_if(PM_RANGER) && rn2(10) <= 5){
					mkmon_template = ZOMBIFIED;
				} else if(!rn2(10)){
					mkmon_template = ZOMBIFIED;
				}
			} else if(!rn2(100)){
				mkmon_template = ZOMBIFIED;
			}
		}
		else
			mkmon_template = 0;
	}

	/* Apply template to the dereferenced ptrptr */
	if (mkmon_template) {
		*ptrptr = permonst_of(ptr->mtyp, mkmon_template);
	}

	return mkmon_template;
}

/*
 * Factions do not affect monster data, so just selects a faction for the ptr.
 *
 * Returns faction chosen.
 */
int
makemon_get_permonst_faction(ptr, faction)
struct permonst * ptr;
int faction;
{
	int out_faction = 0;

	if (faction != -1)
		return faction;
	
	if(Infuture)
		return 0;

	if(is_drow(ptr) && ptr->mtyp != PM_CHANGED  && ptr->mtyp != PM_WARRIOR_CHANGED){
		if(curhouse) {
			out_faction = curhouse;
		} else if((ptr->mtyp == urole.ldrnum && ptr->mtyp != PM_ECLAVDRA) || 
			(ptr->mtyp == urole.guardnum && ptr->mtyp != PM_DROW_MATRON_MOTHER && ptr->mtyp != PM_HEDROW_MASTER_WIZARD)
		){
			if(Race_if(PM_DROW) && !Role_if(PM_EXILE)) out_faction = u.start_house;
			else out_faction = LOLTH_SYMBOL;
		} else if(Is_lolth_level(&u.uz)){
			out_faction = LOLTH_SYMBOL;
		} else if(ptr->mtyp == PM_MINDLESS_THRALL || ptr->mtyp == PM_A_GONE || ptr->mtyp == PM_HOUSELESS_DROW){
			out_faction = PEN_A_SYMBOL;
		} else if(ptr->mtyp == PM_DOKKALFAR_ETERNAL_MATRIARCH){
			out_faction = Infuture ? EDDER_SYMBOL : LOST_HOUSE;
		} else if(ptr->mtyp == PM_ECLAVDRA || ptr->mtyp == PM_AVATAR_OF_LOLTH || is_yochlol(ptr)){
			out_faction = LOLTH_SYMBOL;
		} else if(ptr->mtyp == PM_DROW_MATRON_MOTHER){
			if(Race_if(PM_DROW) && !Role_if(PM_EXILE)) out_faction = (Role_if(PM_NOBLEMAN) && !flags.initgend) ? (((u.start_house-FIRST_FALLEN_HOUSE+1)%(LAST_HOUSE-FIRST_HOUSE+1))+FIRST_HOUSE) : LOLTH_SYMBOL;
			else out_faction = LOLTH_SYMBOL;
		} else if(ptr->mtyp == PM_SEYLL_AUZKOVYN || ptr->mtyp == PM_STJARNA_ALFR){
			out_faction = EILISTRAEE_SYMBOL;
		} else if(ptr->mtyp == PM_PRIESTESS_OF_GHAUNADAUR){
			out_faction = GHAUNADAUR_SYMBOL;
		} else if(ptr->mtyp == PM_DARUTH_XAXOX || ptr->mtyp == PM_DROW_ALIENIST){
			out_faction = XAXOX;
		} else if(ptr->mtyp == PM_EMBRACED_DROWESS || (ptr->mtyp == PM_DROW_MUMMY && In_quest(&u.uz) && !flags.initgend)){
			out_faction = EDDER_SYMBOL;
		} else if(ptr->mtyp == PM_A_SALOM){
			out_faction = VER_TAS_SYMBOL;
		} else if(ptr->mtyp == PM_GROMPH){
			out_faction = SORCERE;
		} else if(ptr->mtyp == PM_DANTRAG){
			out_faction = MAGTHERE;
		} else if(ptr->mtyp == PM_HEDROW_BLADEMASTER){
			out_faction = MAGTHERE;
		} else if(ptr->mtyp == PM_HEDROW_MASTER_WIZARD){
			out_faction = SORCERE;
		} else if(ptr->mlet != S_HUMAN && !((ptr->mtyp == PM_SPROW || ptr->mtyp == PM_DRIDER) && in_mklev && In_quest(&u.uz) && Is_qstart(&u.uz))){
			if(ptr->mtyp == PM_DROW_MUMMY){
				if(!(rn2(10))){
					if(!rn2(6)) out_faction = LOLTH_SYMBOL;
					else if(rn2(5) < 2) out_faction = EILISTRAEE_SYMBOL;
					else out_faction = KIARANSALEE_SYMBOL;
				} else if(!(rn2(4))) out_faction = rn2(LAST_HOUSE+1-FIRST_HOUSE)+FIRST_HOUSE;
				else out_faction = rn2(LAST_FALLEN_HOUSE+1-FIRST_FALLEN_HOUSE)+FIRST_FALLEN_HOUSE;
			}
			else if(ptr->mtyp == PM_HEDROW_ZOMBIE){
				if(!rn2(6)) out_faction = SORCERE;
				else if(!rn2(5)) out_faction = MAGTHERE;
				else if(!(rn2(4))) out_faction = rn2(LAST_HOUSE+1-FIRST_HOUSE)+FIRST_HOUSE;
				else out_faction = rn2(LAST_FALLEN_HOUSE+1-FIRST_FALLEN_HOUSE)+FIRST_FALLEN_HOUSE;
			}
			else out_faction = rn2(LAST_FALLEN_HOUSE+1-FIRST_FALLEN_HOUSE)+FIRST_FALLEN_HOUSE;
		} else if(In_quest(&u.uz)){
			if(Race_if(PM_DROW) && Role_if(PM_EXILE)){
				out_faction = PEN_A_SYMBOL;
			} else if((Race_if(PM_DROW)) && (in_mklev || flags.stag || rn2(3))){
				if(Is_qstart(&u.uz)) out_faction = u.start_house;
				else if(Role_if(PM_NOBLEMAN)){
					if(flags.initgend) out_faction = u.start_house;
					else out_faction = (((u.start_house - FIRST_FALLEN_HOUSE)+FIRST_HOUSE)%(LAST_HOUSE-FIRST_HOUSE))+FIRST_HOUSE;
				} else if((&u.uz)->dlevel <= qlocate_level.dlevel){
					out_faction = rn2(2) ? u.start_house : flags.initgend ? EILISTRAEE_SYMBOL : EDDER_SYMBOL;
				} else {
					out_faction = flags.initgend ? EILISTRAEE_SYMBOL : EDDER_SYMBOL;
				}
			} else out_faction = rn2(LAST_HOUSE+1-FIRST_HOUSE)+FIRST_HOUSE;
		} else {
			out_faction = rn2(LAST_HOUSE+1-FIRST_HOUSE)+FIRST_HOUSE;
		}
	}
	else if(goat_monster(ptr)){
		out_faction = GOATMOM_FACTION;
	}
	else if(ptr->mtyp == PM_LAMASHTU){
		out_faction = LAMASHTU_FACTION;
	}
	return out_faction;
}

/*
 * In some cases, we want to roll peace-minded before setting faction, for stuff like the Wizard's army.
 * This function handles those cases, while makemon_get_permonst_faction() handles factions for
 * cases like the drow where every monster of that type should get a faction, and where **peace_minded()**
 * takes the faction into account when deciding peacefulness.
 */
void
makemon_set_monster_faction(mon)
struct monst * mon;
{
	boolean peaceful = mon->mpeaceful;
	int out_faction = 0;
	
	if(Infuture && !peaceful)
		out_faction = ILSENSINE_FACTION;
	else if(is_mind_flayer(mon->data))
		out_faction = ILSENSINE_FACTION;
	else if(In_quest(&u.uz) && Role_if(PM_EXILE) && !peaceful)
		out_faction = SEROPAENES_FACTION;
	else if((In_quest(&u.uz) && Role_if(PM_MADMAN) && !peaceful)
		|| yellow_monster(mon)
	)
		out_faction = YELLOW_FACTION;
	else if((In_quest(&u.uz) || Is_stronghold(&u.uz) || u.uz.dnum == temple_dnum || u.uz.dnum == tower_dnum || Is_astralevel(&u.uz)) && !peaceful)
		out_faction = YENDORIAN_FACTION;

	else if(in_mklev && In_quest(&u.uz) && peaceful && (
	   (urole.ldrnum == PM_OLD_FORTUNE_TELLER && (mon->mtyp == PM_KNIGHT || mon->mtyp == PM_MAID))
	|| (urole.ldrnum == PM_KING_ARTHUR && Role_if(PM_KNIGHT) && (mon->mtyp == PM_KNIGHT))
	|| (Role_if(PM_EXILE) && (mon->mtyp == PM_PEASANT))
	|| (urole.ldrnum == PM_DAMAGED_ARCADIAN_AVENGER && (mon->mtyp == PM_GNOME || mon->mtyp == PM_GNOME_LORD || mon->mtyp == PM_GNOME_KING
			|| mon->mtyp == PM_TINKER_GNOME || mon->mtyp == PM_GNOMISH_WIZARD))
	)){
		out_faction = QUEST_FACTION;
	}
	
	set_faction(mon, out_faction);
}

/*
 * Shorter function call with "use defaults" filled in for makemon_full()'s template and faction parameters.
 */
struct monst *
makemon(ptr, x, y, mmflags)
struct permonst *ptr;
register int	x, y;
register int	mmflags;
{
	return makemon_full(ptr, x, y, mmflags, -1, -1);	/* -1 meaning "default" */
}

/*
 * Determines monster type (if null),
 *   location (if [0,0]),    (0,0 means random, u.ux,u.uy means near player if !in_mklev)
 *   template (if -1),       (-1 means default for mon+locale, 0 means no template and no random chance at one)
 *   and faction (if -1),    (-1 means default for mon+locale, 0 means no template and no random chance at one)
 */
struct monst *
makemon_full(ptr, x, y, mmflags, template, faction)
struct permonst *ptr;
register int	x, y;
register int	mmflags;
int template;
int faction;
{
	int out_template = template;
	int out_faction = faction;
	boolean randmonst = !ptr;
	boolean givenpos = (x != 0 || y != 0);
	boolean byyou = (x == u.ux && y == u.uy);
	unsigned gpflags = (mmflags & MM_IGNOREWATER) ? MM_IGNOREWATER : 0;

	/* if a monster is being randomly chosen, use its bigger spawning group */
	if (!ptr && !(mmflags & MM_NOGROUP))
		mmflags |= MM_BIGGROUP;

	/* handle subs for high-caste tulani  */
	if(ptr && is_high_caste_eladrin(ptr) && in_mklev){
		if(Is_lamashtu_level(&u.uz)){
			if(ptr->mtyp != PM_TULANI_ELADRIN && ptr->mtyp != PM_GAE_ELADRIN){
				switch(dungeon_topology.alt_tulani){
					case TULANI_CASTE:
					case GAE_CASTE:
						ptr = &mons[!rn2(3) ? PM_BRIGHID_ELADRIN : rn2(2) ? PM_UISCERRE_ELADRIN : PM_CAILLEA_ELADRIN];
					break;
					case BRIGHID_CASTE:
						ptr = &mons[PM_BRIGHID_ELADRIN];
					break;
					case UISCERRE_CASTE:
						ptr = &mons[PM_UISCERRE_ELADRIN];
					break;
					case CAILLEA_CASTE:
						ptr = &mons[PM_CAILLEA_ELADRIN];
					break;
				}
			}
		} else {
			switch(dungeon_topology.alt_tulani){
				case TULANI_CASTE:
					ptr = &mons[PM_TULANI_ELADRIN];
				break;
				case GAE_CASTE:
					ptr = &mons[PM_GAE_ELADRIN];
				break;
				case BRIGHID_CASTE:
					ptr = &mons[PM_BRIGHID_ELADRIN];
				break;
				case UISCERRE_CASTE:
					ptr = &mons[PM_UISCERRE_ELADRIN];
				break;
				case CAILLEA_CASTE:
					ptr = &mons[PM_CAILLEA_ELADRIN];
				break;
			}
		}
	}

	/* if caller both a random creature and a random location, try both at once first */
	if(!ptr && x == 0 && y == 0){
		int tryct = 0;	/* careful with bigrooms */
		struct monst fakemon = {0};
		do{
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);
			ptr = rndmonst();
			if(!ptr) {
	#ifdef DEBUG
				pline("Warning: no monster.");
	#endif
				return((struct monst *) 0);	/* no more monsters! */
			}
			out_template = makemon_set_template(&ptr, template, randmonst);
			set_mon_data_core(&fakemon, ptr); /* set up for goodpos */
			gpflags = (mmflags & MM_IGNOREWATER) ? MM_IGNOREWATER : 0;
		} while((!goodpos(x, y, &fakemon, gpflags) 
				|| (tryct < 50 && !in_mklev && couldsee(x, y)) 
				|| ((tryct < 100 && !in_mklev && cansee(x, y))) 
				|| (!in_mklev && distmin(x,y,u.ux,u.uy) < BOLT_LIM)
			) && tryct++ < 200);
		if(tryct >= 200){
			//That failed, return to the default way of handling things
			ptr = (struct permonst *)0;
			x = y = 0;
		} else if(PM_ARCHEOLOGIST <= monsndx(ptr) && monsndx(ptr) <= PM_WIZARD && !(mmflags & MM_EDOG)){
			return mk_mplayer(ptr, x, y, FALSE);
		}
	}
	
	/* if caller wants random location, do it here */
	if(x == 0 && y == 0) {
		int tryct = 0;	/* careful with bigrooms */
		struct monst fakemon = {0};
		if(ptr) out_template = makemon_set_template(&ptr, template, randmonst);
		if(ptr) set_mon_data_core(&fakemon, ptr); /* set up for goodpos */
		do {
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);
			tryct++;
		} while((tryct < 400 && (!goodpos(x, y, ptr ? &fakemon : (struct monst *)0, Is_waterlevel(&u.uz) ? gpflags|MM_IGNOREWATER : gpflags)))
			|| (tryct < 300 && distmin(x,y,u.ux,u.uy) < BOLT_LIM)
			|| (tryct < 200 && cansee(x, y))
			|| (tryct < 100 && couldsee(x, y))
			);
		if(tryct >= 400){
			return((struct monst *)0);
		}
		if(ptr && PM_ARCHEOLOGIST <= monsndx(ptr) && monsndx(ptr) <= PM_WIZARD && !(mmflags & MM_EDOG)){
			return mk_mplayer(ptr, x, y, FALSE);
		}
	} else if (byyou && !in_mklev) {
		coord bypos;
		if(ptr) out_template = makemon_set_template(&ptr, template, randmonst);
		if(enexto_core(&bypos, u.ux, u.uy, ptr, gpflags)) {
			x = bypos.x;
			y = bypos.y;
		}
		else if (enexto_core(&bypos, u.ux, u.uy, ptr, gpflags|MM_IGNOREWATER)) {
			x = bypos.x;
			y = bypos.y;
		}
		else
			return((struct monst *)0);
	} else if (givenpos && ptr) {
		/* need to check that the given position is safe */
		struct monst fakemon = { 0 };
		out_template = makemon_set_template(&ptr, template, randmonst);
		set_mon_data_core(&fakemon, ptr); /* set up for goodpos */
		if (!goodpos(x, y, &fakemon, gpflags)){
			if ((mmflags & MM_ADJACENTOK) != 0) {
				coord bypos;
				if (enexto_core(&bypos, x, y, ptr, gpflags)) {
					if (!(mmflags & MM_ADJACENTSTRICT) || (
						bypos.x - x <= 1 && bypos.x - x >= -1 &&
						bypos.y - y <= 1 && bypos.y - y >= -1
						)){
						x = bypos.x;
						y = bypos.y;
					}
					else return((struct monst *) 0);
				}
				else
					return((struct monst *) 0);
			}
			else
				return((struct monst *) 0);
		}
	}
	/* Does monster already exist at the position? */
	if(MON_AT(x, y)) {
		if ((mmflags & MM_ADJACENTOK) != 0) {
			coord bypos;
			if(enexto_core(&bypos, x, y, ptr, gpflags)) {
				if( !(mmflags & MM_ADJACENTSTRICT) || (
					bypos.x - x <= 1 && bypos.x - x >= -1 &&
					bypos.y - y <= 1 && bypos.y - y >= -1
				)){
					x = bypos.x;
					y = bypos.y;
				}
				else return((struct monst *) 0);
			} else
				return((struct monst *) 0);
		} else 
			return((struct monst *) 0);
	}
	
	if(ptr){
		/* if you are to make a specific monster and it has
		   already been genocided, return */
		if (mvitals[ptr->mtyp].mvflags & G_GENOD && !In_quest(&u.uz)) return((struct monst *) 0);
#if defined(WIZARD) && defined(DEBUG)
		if (wizard && (mvitals[ptr->mtyp].mvflags & G_EXTINCT && !countbirth))
		    pline("Explicitly creating extinct monster %s.",
			mons[ptr->mtyp].mname);
#endif
		/* set template if it wasn't already set */
		out_template = makemon_set_template(&ptr, out_template, randmonst);
	} else {
		/* make a random (common) monster that can survive here.
		 * (the special levels ask for random monsters at specific
		 * positions, causing mass drowning on the medusa level,
		 * for instance.)
		 */
		int tryct = 0;	/* maybe there are no good choices */
		struct monst fakemon = {0};
		do {
			if(!(ptr = rndmonst())) {
#ifdef DEBUG
			    pline("Warning: no monster.");
#endif
			    return((struct monst *) 0);	/* no more monsters! */
			}
			out_template = makemon_set_template(&ptr, template, randmonst);
			set_mon_data_core(&fakemon, ptr); /* set up for goodpos */
		} while(!goodpos(x, y, &fakemon, gpflags) && tryct++ < 150);
		if(tryct >= 150){
			return((struct monst *) 0);	/* no more monsters! */
		}
	}

	/* determine faction -- since this does not affect ptr (and therefore location),
	 * it can just be done at the very end */
	out_faction = makemon_get_permonst_faction(ptr, faction);

	return makemon_core(ptr, x, y, mmflags, out_template, out_faction);
}

/* 
 * Called with known ptr, [x,y] coord, template, and faction.
 *
 * In case we make a monster group, only return the one at [x,y].
 */
struct monst *
makemon_core(ptr, x, y, mmflags, template, faction)
struct permonst *ptr;
register int	x, y;
register int	mmflags;
int template;
int faction;
{
	register struct monst *mtmp, *tmpm;
	int mndx = ptr->mtyp;
	int mcham, ct, mitem, num;
	boolean allow_minvent = ((mmflags & NO_MINVENT) == 0);
	boolean countbirth = ((mmflags & MM_NOCOUNTBIRTH) == 0 && !In_quest(&u.uz));
	boolean goodequip = ((mmflags & MM_GOODEQUIP) != 0);
	boolean randmonst = !ptr;
	boolean unsethouse = FALSE;
	int mkobjflags = NO_MKOBJ_FLAGS | ((mmflags & MM_ESUM) ? MKOBJ_SUMMON : 0) | (goodequip ? MKOBJ_GOODEQUIP : 0);

	/* if monster is destined to be a summon or pet, it shouldn't get a group */
	if (mmflags & (MM_EDOG|MM_ESUM))
		mmflags |= MM_NOGROUP;


	if(allow_minvent) allow_minvent = !(mons[mndx].maligntyp < 0 && Is_illregrd(&u.uz) && in_mklev);
	(void) propagate(mndx, countbirth, FALSE);
	mtmp = malloc(sizeof(struct monst));
	*mtmp = zeromonst;		/* clear all entries in structure */
	mtmp->nmon = fmon;
	fmon = mtmp;
	mtmp->m_id = flags.ident++;
	if (!mtmp->m_id) mtmp->m_id = flags.ident++;	/* ident overflowed */
	mtmp->mcansee = mtmp->mcanhear = mtmp->mcanmove = mtmp->mnotlaugh = TRUE;
	mtmp->mblinded = mtmp->mfrozen = mtmp->mlaughing = 0;
	mtmp->mvar1 = mtmp->mvar2 = mtmp->mvar3 = 0;
	mtmp->mtyp = mndx;
	/* might have been called saying to add an mx */
	if (mmflags & MM_EDOG)
		add_mx(mtmp, MX_EDOG);
	if (mmflags & MM_ESUM) {
		/* set up mtmp as summoned indefinitely
		   caller is responsible for setting summoner, duration and removing permanence if desired */
		add_mx(mtmp, MX_ESUM);
		mtmp->mextra_p->esum_p->summoner = (struct monst *)0;
		mtmp->mextra_p->esum_p->sm_id = 0;
		mtmp->mextra_p->esum_p->sm_o_id = 0;
		mtmp->mextra_p->esum_p->summonstr = 0;
		mtmp->mextra_p->esum_p->staleptr = 0;
		mtmp->mextra_p->esum_p->permanent = 1;
		mtmp->mextra_p->esum_p->sticky = 0;
		start_timer(ESUMMON_PERMANENT, TIMER_MONSTER, DESUMMON_MON, (genericptr_t)mtmp);
	}

	if (is_horror(ptr)) {
		add_mx(mtmp, MX_EHOR);
		if (mndx == PM_NAMELESS_HORROR) {
			extern char * nameless_horror_name;
			int plslev = rn2(12);
			EHOR(mtmp)->basehorrordata = *ptr;
			nameless_horror_name = EHOR(mtmp)->randname;
			make_horror(&(EHOR(mtmp)->basehorrordata), 37 + plslev, 15 + plslev);
			nameless_horror_name = (char *)0;
			ptr = &(EHOR(mtmp)->basehorrordata);
			EHOR(mtmp)->currhorrordata = *ptr;
		}
		else {
			EHOR(mtmp)->basehorrordata = *ptr;
			EHOR(mtmp)->currhorrordata = *ptr;
		}
	}
	set_mon_data(mtmp, mndx);
	
	mtmp->mstr = d(3,6);
	if(strongmonst(mtmp->data)) mtmp->mstr += 10;
	mtmp->mdex = d(3,6);
	if(is_elf(mtmp->data) && mtmp->mstr > mtmp->mdex){
		short swap = mtmp->mstr;
		mtmp->mstr = mtmp->mdex;
		mtmp->mdex = swap;
	}
	// mtmp->mcon = d(3,6);
	//More tightly constrain to "average"
	mtmp->mcon = 7 + d(1,6);
	if(is_animal(mtmp->data)) mtmp->mint = 3;
	else if(mindless_mon(mtmp)) mtmp->mint = 0;
	else if(is_magical(mtmp->data)) mtmp->mint = 13+rnd(5);
	else mtmp->mint = d(3,6);
	mtmp->mwis = d(3,6);
	mtmp->mcha = d(3,6);
	if(mtmp->data->mlet == S_NYMPH){
		if(mtmp->mtyp == PM_DEMINYMPH) mtmp->mcha = (mtmp->mcha + 25)/2;
		else mtmp->mcha = 25;
	}
	
	if (ptr->mtyp == urole.ldrnum)
	    quest_status.leader_m_id = mtmp->m_id;
	mtmp->m_lev = adj_lev(ptr);
	
	if(is_eladrin(ptr) && mtmp->m_lev <= u.ulevel){
		int delta = (u.ulevel - mtmp->m_lev) + 1;
		mtmp->m_lev += rnd(delta*2) - delta;
	}
	
	mtmp->m_insight_level = 0;

	if(mtmp->mtyp == PM_LURKING_ONE)
		mtmp->m_insight_level = 20+rn2(21);
	else if(mtmp->mtyp == PM_BLASPHEMOUS_LURKER)
		mtmp->m_insight_level = 40;
	else if(mtmp->mtyp == PM_PARASITIZED_DOLL)
		mtmp->m_insight_level = rnd(20);
	else if(mtmp->mtyp == PM_LIVING_DOLL){
		mtmp->mvar_dollTypes = init_doll_sales();
		mtmp->m_insight_level = rnd(20);
	}
	
	else if(mtmp->mtyp == PM_BESTIAL_DERVISH)
		mtmp->m_insight_level = 20+rn2(10);
	else if(mtmp->mtyp == PM_ETHEREAL_DERVISH)
		mtmp->m_insight_level = 20+rn2(10);
	else if(mtmp->mtyp == PM_SPARKLING_LAKE)
		mtmp->m_insight_level = 18+rn2(9);
	else if(mtmp->mtyp == PM_FLASHING_LAKE)
		mtmp->m_insight_level = 16+rn2(8);
	else if(mtmp->mtyp == PM_SMOLDERING_LAKE)
		mtmp->m_insight_level = 11+rn2(6);
	else if(mtmp->mtyp == PM_FROSTED_LAKE)
		mtmp->m_insight_level = 12+rn2(6);
	else if(mtmp->mtyp == PM_BLOOD_SHOWER)
		mtmp->m_insight_level = 14+rn2(7);
	else if(mtmp->mtyp == PM_MANY_TALONED_THING)
		mtmp->m_insight_level = 16+rn2(8);
	else if(mtmp->mtyp == PM_DEEP_BLUE_CUBE)
		mtmp->m_insight_level = 10+rn2(5);
	else if(mtmp->mtyp == PM_PITCH_BLACK_CUBE)
		mtmp->m_insight_level = 22+rn2(11);
	else if(mtmp->mtyp == PM_PERFECTLY_CLEAR_CUBE)
		mtmp->m_insight_level = 30+rn2(11);
	else if(mtmp->mtyp == PM_PRAYERFUL_THING)
		mtmp->m_insight_level = 25+rn2(13);
	else if(mtmp->mtyp == PM_HEMORRHAGIC_THING)
		mtmp->m_insight_level = 15+rn2(8);
	else if(mtmp->mtyp == PM_MANY_EYED_SEEKER)
		mtmp->m_insight_level = 17+rn2(9);
	else if(mtmp->mtyp == PM_VOICE_IN_THE_DARK)
		mtmp->m_insight_level = 19+rn2(10);
	else if(mtmp->mtyp == PM_TINY_BEING_OF_LIGHT)
		mtmp->m_insight_level = 13+rn2(7);
	else if(mtmp->mtyp == PM_MAN_FACED_MILLIPEDE)
		mtmp->m_insight_level = 5+rn2(3);
	else if(mtmp->mtyp == PM_MIRRORED_MOONFLOWER)
		mtmp->m_insight_level = 10+rn2(5);
	else if(mtmp->mtyp == PM_CRIMSON_WRITHER)
		mtmp->m_insight_level = 14+rn2(7);
	else if(mtmp->mtyp == PM_RADIANT_PYRAMID)
		mtmp->m_insight_level = 12+rn2(6);
	else if(mtmp->mtyp == PM_GROTESQUE_PEEPER)
		mtmp->m_insight_level = 16+rn2(8);
	
	else if(mtmp->mtyp == PM_BRIGHT_WALKER)
		mtmp->m_insight_level = 10;
	
	else if(mtmp->mtyp == PM_KUKER)
		mtmp->m_insight_level = rnd(20)+rn2(21);
	
	else if(mtmp->mtyp == PM_BLESSED)
		mtmp->m_insight_level = 40;

	else if(mtmp->mtyp == PM_MOUTH_OF_THE_GOAT)
		mtmp->m_insight_level = 60;
	else if(mtmp->mtyp == PM_GOOD_NEIGHBOR)
		mtmp->m_insight_level = 40;
	else if(mtmp->mtyp == PM_HMNYW_PHARAOH)
		mtmp->m_insight_level = 40;
	else if(mtmp->mtyp == PM_STRANGER)
		mtmp->m_insight_level = 55;
	
	else if(mtmp->mtyp == PM_POLYPOID_BEING)
		mtmp->m_insight_level = 40;

	else if(mtmp->mtyp == PM_APHANACTONAN_AUDIENT)
		mtmp->m_insight_level = rnd(16);
	else if(mtmp->mtyp == PM_APHANACTONAN_ASSESSOR)
		mtmp->m_insight_level = 16+d(4,4);
	
	if(mtmp->mtyp == PM_CHOKHMAH_SEPHIRAH)
		mtmp->m_lev += u.chokhmah;
	if (is_golem(ptr)) {
	    mtmp->mhpmax = mtmp->mhp = golemhp(mndx);
	} else if (rider_hp(ptr)) {
	    /* We want low HP, but a high mlevel so they can attack well */
	    mtmp->mhpmax = mtmp->mhp = d(10,8);
	} else if (ptr->mlevel > 49 || ptr->geno & G_UNIQ) {
	    /* "special" fixed hp monster
	     * the hit points are encoded in the mlevel in a somewhat strange
	     * way to fit in the 50..127 positive range of a signed character
	     * above the 1..49 that indicate "normal" monster levels */
//	    mtmp->mhpmax = mtmp->mhp = 2*(ptr->mlevel - 6);
	    mtmp->mhpmax = mtmp->mhp = max(4, 8*(ptr->mlevel));
	    // mtmp->m_lev = mtmp->mhp / 4;	/* approximation */
	} else if (is_ancient(mtmp) || is_tannin(mtmp)) {
		mtmp->mhpmax = mtmp->mhp = max(4, 8*(ptr->mlevel));
	} else if (ptr->mlet == S_DRAGON && mndx >= PM_GRAY_DRAGON) {
	    /* adult dragons */
	    mtmp->mhpmax = mtmp->mhp = (int) (In_endgame(&u.uz) ?
		(8 * mtmp->m_lev) : (4 * mtmp->m_lev + d((int)mtmp->m_lev, 4)));
	} else if (!mtmp->m_lev) {
	    mtmp->mhpmax = mtmp->mhp = rnd(4);
	} else {
		if(Infuture){
			mtmp->mhpmax = mtmp->mhp = mtmp->m_lev*8 - 1;
		} else {
		    mtmp->mhpmax = mtmp->mhp = d((int)mtmp->m_lev, 8);
		    if (is_home_elemental(ptr))
			mtmp->mhpmax = (mtmp->mhp *= 3);
		}
	}

	if (is_female(ptr)) mtmp->female = TRUE;
	else if (is_male(ptr)) mtmp->female = FALSE;
	else mtmp->female = rn2(2);	/* ignored for neuters */

	// if (ptr->mtyp == urole.ldrnum)		/* leader knows about portal */
	    // mtmp->mtrapseen |= (1L << (MAGIC_PORTAL-1));
	// if (ptr->mtyp == PM_OONA)  /* don't trigger statue traps */
		// mtmp->mtrapseen |= (1L << (STATUE_TRAP-1));
	if (In_sokoban(&u.uz) && !mindless(ptr))  /* know about traps here */
	    mtmp->mtrapseen = (1L << (PIT - 1)) | (1L << (HOLE - 1));
	if (In_endgame(&u.uz))  /* know about fire traps here */
	    mtmp->mtrapseen = (1L << (PIT - 1)) | (1L << (FIRE_TRAP - 1) | (1L << (MAGIC_PORTAL-1)));
	if (ptr->mtyp == urole.ldrnum || ptr->mtyp == urole.guardnum)		/* leader and guards knows about all traps */
	    mtmp->mtrapseen |= ~0;
	mtmp->mtrapseen |= (1L << (STATUE_TRAP-1)); /* all monsters should avoid statue traps */

	place_monster(mtmp, x, y);
	mtmp->mpeaceful  = FALSE;
	mtmp->mtraitor  = FALSE;
	mtmp->mferal  = FALSE;
	mtmp->mcrazed  = FALSE;
	mtmp->mclone  = FALSE;

	mtmp->mspec_used = 3;
	mtmp->encouraged = 0;
	
	//"Living" creatures generated in heaven or hell are in fact already dead (and should not leave corpses).
	if((In_hell(&u.uz) || In_endgame(&u.uz)) 
		&& !is_rider(mtmp->data) 
		&& !nonliving(mtmp->data)
	)
		mtmp->mpetitioner = TRUE;
	
	/* apply chosen factions and templates */
	if (faction != 0) {
		set_faction(mtmp, faction);

		/* Ok, here's the deal: I'm using a global to coordinate the house of a group of drow. 
		 * Although armor can be done by accessing mfaction on mtmp, peacemindedness only gets a ptr,
		 * and we need to coordinate drow that are showing up in Aurumach Rilmani throne rooms.
		 */
		if (is_drow(mtmp->data) && !curhouse) {
			set_curhouse(faction);
			unsethouse = TRUE;
		}
	}
	if (template != 0){
		/* apply template */
		set_template(mtmp, template);
		/* update ptr to mtmp's new data */
		ptr = mtmp->data;
		/* special case: some templates increase the level of the creatures made */
		int plslvl = 0;
		switch (template) {
		case FRACTURED:
			plslvl = 4; break;
		case YITH:
			plslvl = 2; break;
		}
		if (plslvl) {
			mtmp->m_lev += plslvl;
			mtmp->mhpmax += d(plslvl, 8);
			mtmp->mhp = mtmp->mhpmax;
		}
		/* update symbol */
		newsym(mtmp->mx,mtmp->my);
		/* zombies and other derived undead are much less likely to have their items */
		if (is_undead(mtmp->data))
			allow_minvent = rn2(2);
	}
	
	//One-off templates like Yith should be unset immediately after being applied
	if(template == YITH){
		template = 0;
	}
	
	if(Race_if(PM_DROW) && in_mklev && Is_qstart(&u.uz) && 
		(ptr->mtyp == PM_SPROW || ptr->mtyp == PM_DRIDER || ptr->mtyp == PM_CAVE_LIZARD || ptr->mtyp == PM_LARGE_CAVE_LIZARD)
	) mtmp->mpeaceful = TRUE;
	else mtmp->mpeaceful = (mmflags & MM_ANGRY) ? FALSE : peace_minded(ptr);
	
	if(mtmp->mfaction <= 0)
		makemon_set_monster_faction(mtmp);
	if(mndx == PM_CHAOS){
		mtmp->mhpmax = 15*mtmp->mhpmax;
		mtmp->mhp = mtmp->mhpmax;
	} else if(mndx == PM_KARY__THE_FIEND_OF_FIRE){
		mtmp->mhpmax = 10*mtmp->mhpmax;
		mtmp->mhp = mtmp->mhpmax;
	} else if(mndx == PM_LICH__THE_FIEND_OF_EARTH){
		mtmp->mhpmax = 10*mtmp->mhpmax;
		mtmp->mhp = mtmp->mhpmax;
	} else if(mndx == PM_KRAKEN__THE_FIEND_OF_WATER){
		mtmp->mhpmax = 10*mtmp->mhpmax;
		mtmp->mhp = mtmp->mhpmax;
	} else if(mndx == PM_TIAMAT__THE_FIEND_OF_WIND){
		mtmp->mhpmax = 10*mtmp->mhpmax;
		mtmp->mhp = mtmp->mhpmax;
	}
	
	switch(ptr->mlet) {
		case S_MIMIC:
			set_mimic_sym(mtmp);
			break;
		case S_QUADRUPED:
			if(mtmp->mtyp == PM_DARK_YOUNG) set_mimic_sym(mtmp);
			break;
		case S_SPIDER:
		case S_SNAKE:
			if(in_mklev && ptr->mtyp != PM_YURIAN && ptr->mtyp != PM_FIRST_WRAITHWORM)
			    if(x && y)
				(void) mkobj_at(0, x, y, MKOBJ_ARTIF);
			if(hides_under(ptr) && OBJ_AT(x, y))
			    mtmp->mundetected = TRUE;
		break;
		case S_LIGHT:
		case S_ELEMENTAL:
			if (mndx == PM_STALKER || mndx == PM_BLACK_LIGHT) {
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
			}
		break;
		case S_EYE:
			if(!(mmflags & MM_NOGROUP)){
			if (mndx == PM_QUINON){
				mtmp->movement = d(1,6);
				if(mmflags & MM_BIGGROUP){
					makemon_full(&mons[PM_TRITON], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
					for(num = rnd(6); num >= 0; num--) makemon_full(&mons[PM_DUTON], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
					tmpm = makemon_full(&mons[PM_MONOTON], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
					if(tmpm) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
				}
				makemon_full(&mons[PM_QUATON], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
			} else if (mndx == PM_QUATON){
				mtmp->movement = d(1,7);
				if(mmflags & MM_BIGGROUP){
					for(num = rn1(6,5); num >= 0; num--) makemon_full(&mons[PM_MONOTON], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
				}
				makemon_full(&mons[PM_TRITON], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
			} else if (mndx == PM_TRITON){
				mtmp->movement = d(1,8);
				if(mmflags & MM_BIGGROUP){
					tmpm = makemon_full(&mons[PM_MONOTON], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
					if(tmpm) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
				}
				makemon_full(&mons[PM_DUTON], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
			} else if (mndx == PM_DUTON){
				mtmp->movement = d(1,9);
				if((mmflags & MM_BIGGROUP) && rn2(2)){
					tmpm = makemon_full(&mons[PM_MONOTON], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
					if(tmpm) m_initsgrp(tmpm, mtmp->mx, mtmp->my);
				}
				else makemon_full(&mons[PM_MONOTON], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
			} else if (mndx == PM_MONOTON){
				mtmp->movement = d(1,10);
			} else if (mndx == PM_BEHOLDER){
				if(mmflags & MM_BIGGROUP){
					tmpm = makemon_full(&mons[PM_GAS_SPORE], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
					if(tmpm) m_initsgrp(tmpm, mtmp->mx, mtmp->my);
				}
			}
			}
/*			if(mndx == PM_VORLON_MISSILE){
				mtmp->mhpmax = 3;
				mtmp->mhp = 3;
			}
*/		break;
		case S_IMP:
			if(!(mmflags & MM_NOGROUP)){
			if ((mmflags & MM_BIGGROUP) && mndx == PM_LEGION_DEVIL_SERGEANT){
				for(num = 10; num > 0; num--) makemon_full(&mons[PM_LEGION_DEVIL_GRUNT], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
				for(num = 3; num > 0; num--) makemon_full(&mons[PM_LEGION_DEVIL_SOLDIER], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			} else if ((mmflags & MM_BIGGROUP) && mndx == PM_LEGION_DEVIL_CAPTAIN){
				for(num = 20; num > 0; num--) makemon_full(&mons[PM_LEGION_DEVIL_GRUNT], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
				for(num = 8; num > 0; num--) makemon_full(&mons[PM_LEGION_DEVIL_SOLDIER], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
				for(num = 2; num > 0; num--) makemon_full(&mons[PM_LEGION_DEVIL_SERGEANT], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			}
			}
		break;
		case S_LAW_ANGEL:
		case S_NEU_ANGEL:
		case S_CHA_ANGEL:
			if(is_weeping(mtmp->data)){
				mtmp->mvar1 = 0;
				mtmp->mvar2 = 0;
				mtmp->mvar3 = 0;
				if (mmflags & MM_BIGGROUP){
					if(u.uevent.invoked) m_initlgrp(mtmp, 0, 0);
					else mtmp->mvar3 = 1; //Set to 1 to initiallize
				}
			} else if(mtmp->mtyp == PM_ARCADIAN_AVENGER){
				if(mmflags & MM_BIGGROUP){
					tmpm = makemon_full(&mons[PM_ARCADIAN_AVENGER], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOCOUNTBIRTH, template, faction);
					if(tmpm) m_initsgrp(tmpm, mtmp->mx, mtmp->my);
				}
			} else if(mndx == PM_KETO || mndx == PM_DRACAE_ELADRIN){ 
				mtmp->mhpmax = 3*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			} else if(ptr->mtyp == PM_HARROWER_OF_ZARIEL){
				mtmp->m_lev *= 1.5;
				mtmp->mhpmax = max(4, 8*mtmp->m_lev);
				mtmp->mhp = mtmp->mhpmax;
			} else if(mndx == PM_KUKER){ 
				mtmp->mhpmax = mtmp->m_lev*8 - 4; //Max HP
				mtmp->mhp = mtmp->mhpmax;
			}

			if(in_mklev && is_angel(mtmp->data) && Is_demogorgon_level(&u.uz)){
				set_template(mtmp, MAD_TEMPLATE);
				mtmp->m_lev += (mtmp->data->mlevel)/2;
				mtmp->mhpmax = max(4, 8*mtmp->m_lev);
				mtmp->mhp = mtmp->mhpmax;
			}

			if(in_mklev && is_angel(mtmp->data) && Is_lamashtu_level(&u.uz)){
				set_faction(mtmp, LAMASHTU_FACTION);
				if(is_eladrin(mtmp->data)){
					set_template(mtmp, ILLUMINATED);
				}
			}
		break;
	    case S_GIANT:
			if(!(mmflags & MM_NOGROUP)){
				if ((mmflags & MM_BIGGROUP) && mndx == PM_DEEPEST_ONE){
					for(num = rn1(3,3); num >= 0; num--) makemon_full(&mons[PM_DEEPER_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
					for(num = rn1(10,10); num >= 0; num--) makemon_full(&mons[PM_DEEP_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
				}
			}
		break;
		case S_HUMAN:
			if(!(mmflags & MM_NOGROUP)){
				if(mmflags & MM_BIGGROUP){
					if (mndx == PM_DROW_MATRON){
						tmpm = makemon_full(&mons[PM_HEDROW_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
						if(tmpm) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
					} else if (mndx == PM_DOKKALFAR_ETERNAL_MATRIARCH){
						if(Infuture){
							tmpm = makemon_full(&mons[PM_EMBRACED_DROWESS], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
							if(tmpm) m_initsgrp(tmpm, mtmp->mx, mtmp->my);
							tmpm = makemon_full(&mons[PM_MIND_FLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
							if(tmpm) m_initsgrp(tmpm, mtmp->mx, mtmp->my);
							tmpm = makemon_full(&mons[PM_EDDERKOP], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
							if(tmpm) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
							tmpm = makemon_full(&mons[PM_HEDROW_ZOMBIE], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
							if(tmpm) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
							tmpm = makemon_full(&mons[PM_DROW_MUMMY], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
							if(tmpm) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
						} else {
							tmpm = makemon_full(&mons[PM_DROW_MATRON], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
							if(tmpm) m_initsgrp(tmpm, mtmp->mx, mtmp->my);
							tmpm = makemon_full(&mons[PM_HEDROW_WARRIOR], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
							if(tmpm) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
							tmpm = makemon_full(&mons[PM_DROW_MUMMY], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
							if(tmpm) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
							tmpm = makemon_full(&mons[PM_HEDROW_ZOMBIE], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
							if(tmpm) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
						}
					} else if (mndx == PM_ELVENKING || mndx == PM_ELVENQUEEN){
						for(num = rnd(2); num >= 0; num--) makemon_full(&mons[rn2(2) ? PM_ELF_LORD : PM_ELF_LADY], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
						for(num = rn1(6,3); num >= 0; num--) makemon_full(&mons[PM_GREY_ELF], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
					} else if (mndx == PM_ALABASTER_ELF_ELDER){
						if(rn2(2)){
							tmpm = makemon_full(&mons[PM_ALABASTER_ELF_ELDER], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
							if(tmpm) m_initsgrp(tmpm, mtmp->mx, mtmp->my);
						}
						tmpm = makemon_full(&mons[PM_ALABASTER_ELF], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
						if(tmpm) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
					} else if (mndx == PM_ELF && In_quest(&u.uz) && urole.neminum == PM_NECROMANCER) {
						m_initlgrp(mtmp, mtmp->mx, mtmp->my);
					}
				}
			}
		break;
		case S_HUMANOID:
			if(!(mmflags & MM_NOGROUP)){
			if(mmflags & MM_BIGGROUP){
				if (mndx == PM_DEEPER_ONE){
					for(num = rn1(10,3); num >= 0; num--) makemon_full(&mons[PM_DEEP_ONE], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
				}
				if(Infuture){
					if (mndx == PM_MIND_FLAYER){
						m_initsgrp(mtmp, mtmp->mx, mtmp->my);
					}
					if (mndx == PM_MASTER_MIND_FLAYER){
						for(num = d(2,3); num >= 0; num--) makemon_full(&mons[PM_MIND_FLAYER], mtmp->mx, mtmp->my, MM_ADJACENTOK|MM_NOGROUP, template, faction);
					}
				}
			}
			}
		break;
		case S_FUNGUS:
			if(!(mmflags & MM_NOGROUP)){
			if ((mmflags & MM_BIGGROUP) && mndx == PM_MIGO_QUEEN){
				for(num = rn2(2)+1; num >= 0; num--) makemon_full(&mons[PM_MIGO_PHILOSOPHER], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
				for(num = rn2(3)+3; num >= 0; num--) makemon_full(&mons[PM_MIGO_SOLDIER], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
				for(num = rn2(5)+5; num >= 0; num--) makemon_full(&mons[PM_MIGO_WORKER], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);

			}
			}
			if (mndx == PM_PHANTOM_FUNGUS) {
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
			}
		break;
		case S_GNOME:
			if(mndx == PM_CLOCKWORK_SOLDIER || mndx == PM_CLOCKWORK_DWARF || 
			   mndx == PM_FABERGE_SPHERE || mndx == PM_FIREWORK_CART || 
			   mndx == PM_JUGGERNAUT || mndx == PM_ID_JUGGERNAUT
			) mtmp->mvar_vector = rn2(8);
			
			if(mndx == PM_ID_JUGGERNAUT) {
				mtmp->perminvis = TRUE;
				mtmp->minvis = TRUE;
			}
		break;
		case S_WRAITH:
			if (mndx == PM_PHANTASM){
				mtmp->mhpmax = mtmp->mhp = d(1,10);
				if(Infuture){
					if(rn2(2)) {
						mtmp->perminvis = TRUE;
						mtmp->minvis = TRUE;
					}
					if(!rn2(3)){
						mtmp->mspeed = MFAST;
						mtmp->permspeed = MFAST;
					}
					else if(rn2(2)){
						mtmp->mspeed = MSLOW;
						mtmp->permspeed = MSLOW;
					}
					
					if(!rn2(100)){
						set_template(mtmp, PSEUDONATURAL);
					}
					else if(!rn2(99)){
						set_template(mtmp, DREAM_LEECH);
					}
					else if(!rn2(98)){
						set_template(mtmp, FRACTURED);
					}
					else if(!rn2(10)) {
						set_template(mtmp, M_BLACK_WEB);
					}
					if(!rn2(100)) {
						mtmp->mhpmax = mtmp->mhp = 1000;
					}
				}
				else {
					if(rn2(2)) {
						mtmp->perminvis = TRUE;
						mtmp->minvis = TRUE;
					}
					if(!rn2(3)){
						mtmp->mspeed = MFAST;
						mtmp->permspeed = MFAST;
					}
					else if(rn2(2)){
						mtmp->mspeed = MSLOW;
						mtmp->permspeed = MSLOW;
					}
					else{
						mtmp->mspeed = 0;
						mtmp->permspeed = 0;
					}
					if(rn2(2)){
						mtmp->mflee = 1;
						mtmp->mfleetim = d(1,10);
					}
					if(rn2(2)){
						if(rn2(4)){
							mtmp->mcansee = 0;
							mtmp->mblinded = rn2(30);
						}
						else{
							mtmp->mcansee = 0;
							mtmp->mblinded = 0;
						}
					}
					if(rn2(2)){
						if(rn2(2)){
							mtmp->mconf = 1;
						}else if(rn2(2)){
							mtmp->mstun = 1;
						}else{
							mtmp->mcrazed = 1;
						}
					}
					if(rn2(2)){
						if(rn2(4)){
							mtmp->mcanmove = 0;
							mtmp->mfrozen = d(1,10);
						}else{
							mtmp->mcanmove = 0;
							mtmp->mfrozen = 0;
						}
					}
					if(!rn2(9)){
						mtmp->mnotlaugh = 0;
						// You_hear("soft laughter.");
						mtmp->mlaughing = d(2,4);
					}
					if(!rn2(8)){
						mtmp->msleeping = 1;
					}
					if(!rn2(8)){
						mtmp->mpeaceful = 1;
					}
				}
			}
		break;
		case S_TRAPPER:
			if(!(mmflags & MM_NOGROUP)){
				if(mndx==PM_METROID_QUEEN) 
					if(mmflags & MM_BIGGROUP) for(num = 6; num >= 0; num--) makemon_full(&mons[PM_METROID], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
					// if(mmflags & MM_BIGGROUP) for(num = 6; num >= 0; num--) makemon_full(&mons[PM_BABY_METROID], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
					// else for(num = 6; num >= 0; num--) makemon_full(&mons[PM_METROID], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			}
		break;
//		case S_VAMPIRE:
//			{
//				if(mndx == PM_STAR_VAMPIRE){
//				    mtmp->minvis = TRUE;
//				    mtmp->perminvis = TRUE;
//				}
//			}
//		break;
		case S_BLOB:
			if (mndx == PM_SHOGGOTH || mndx == PM_PRIEST_OF_GHAUNADAUR || mndx == PM_MOTHERING_MASS){
				mtmp->mhpmax = 3*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			}
			if (mndx == PM_CRYSTAL_OOZE){
				if (is_pool(x, y, FALSE))
					mtmp->mundetected = TRUE;
			}
			if (mndx == PM_PERFECTLY_CLEAR_CUBE) {
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
			}
		break;
	    case S_KETER:
			if (mndx == PM_CHOKHMAH_SEPHIRAH){
				mtmp->mhpmax = 1+u.chokhmah*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			}
			if(!(mmflags & MM_NOGROUP)){
			if(mndx != PM_MALKUTH_SEPHIRAH && mndx != PM_DAAT_SEPHIRAH && mndx != PM_BINAH_SEPHIRAH){
				coord mm;
				mm.x = xdnstair;
				mm.y = ydnstair;
				makeketer(&mm);
				
				mm.x = xupstair;
				mm.y = yupstair;
				makeketer(&mm);
			}
			}
		break;
		case S_EEL:
			if (is_pool(x, y, FALSE))
			    mtmp->mundetected = TRUE;
			if(mndx == PM_WATCHER_IN_THE_WATER){ 
				mtmp->mhpmax = 3*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
				mtmp->mcanmove = 0;
				mtmp->mfrozen = 3;
			}
		break;
		case S_LEPRECHAUN:
			mtmp->msleeping = 1;
			break;
		case S_JABBERWOCK:
		case S_NYMPH:
			if (rn2(5) && !u.uhave.amulet 
				&& mndx != PM_NIMUNE && mndx != PM_INTONER && mndx != PM_AGLAOPE 
				&& !Infuture
			){
				mtmp->msleeping = 1;
			}
		break;
		case S_ORC:
			if (Race_if(PM_ELF)) mtmp->mpeaceful = FALSE;
			else if(mndx == PM_BOLG){
				mtmp->mhpmax = 3*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			}
			if(!(mmflags & MM_NOGROUP)){
			if ((mmflags & MM_BIGGROUP) && mndx == PM_ORC_CAPTAIN){
				for(num = rn1(10,3); num >= 0; num--) makemon_full(rn2(3) ? &mons[PM_HILL_ORC] : &mons[PM_MORDOR_ORC], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			}
			if ((mmflags & MM_BIGGROUP) && mndx == PM_URUK_CAPTAIN){
				for(num = rn1(10,3); num >= 0; num--) makemon_full(&mons[PM_URUK_HAI], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			}
			if ((mmflags & MM_BIGGROUP) && mndx == PM_ORC_SHAMAN){
				for(num = rnd(3); num >= 0; num--) makemon_full(&mons[PM_HILL_ORC], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			}
			if ((mmflags & MM_BIGGROUP) && mndx == PM_MORDOR_MARSHAL){
				for(num = rn1(10,3); num >= 0; num--) makemon_full(rn2(3) ? &mons[PM_MORDOR_ORC] : &mons[PM_MORDOR_ORC_ELITE], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			}
			if ((mmflags & MM_BIGGROUP) && mndx == PM_MORDOR_SHAMAN){
				for(num = rnd(3); num >= 0; num--) makemon_full(&mons[PM_MORDOR_ORC_ELITE], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			}
			if ((mmflags & MM_BIGGROUP) && mndx == PM_ORC_OF_THE_AGES_OF_STARS){
				for(num = rn1(10,3); num >= 0; num--) makemon_full(&mons[PM_ANGBAND_ORC], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			}
			}
		break;
		case S_UNICORN:
			if (is_unicorn(ptr) && u.ualign.type != A_VOID &&
					sgn(u.ualign.type) == sgn(ptr->maligntyp))
				mtmp->mpeaceful = TRUE;
//			if(mndx == PM_PINK_UNICORN){
//			    mtmp->minvis = TRUE;
//			    mtmp->perminvis = TRUE;
//			}
		break;
		case S_UMBER:
			if(mndx == PM_UVUUDAUM){
				mtmp->m_lev = 38;
				mtmp->mhpmax = d(38, 8);
				if(mtmp->mhpmax < 38*4.5)
					mtmp->mhpmax = (int)(38*4.5);
				mtmp->mhp = mtmp->mhpmax;
			}
			else if(mndx == PM_ASPECT_OF_THE_SILENCE){
				struct obj *otmp = mksobj(SKELETON_KEY, mkobjflags);
			    mtmp->minvis = TRUE;
			    mtmp->perminvis = TRUE;
				otmp = oname(otmp, artiname(ART_THIRD_KEY_OF_CHAOS));
				if(otmp->oartifact){
					place_object(otmp, mtmp->mx, mtmp->my);
					rloco(otmp);
				} else {
					obfree(otmp, (struct obj *) 0);
				}
				if (in_mklev) {
					pline("A terrible silence reigns!");
					com_pager(202);
					mtmp->msleeping = 1;
				}
				else {
					pline("A terrible silence falls!");
					com_pager(203);
				}
				nomul(0, NULL);
			}
		break;
		case S_BAT:
			if (Inhell && is_bat(ptr))
			    mon_adjust_speed(mtmp, 2, (struct obj *)0);

			if (mndx == PM_CHIROPTERAN && (mmflags & MM_BIGGROUP) && !(mmflags & MM_NOGROUP)) {
				tmpm = makemon_full(&mons[PM_WARBAT], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
				if(tmpm && !rn2(3)) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
				tmpm = makemon_full(&mons[PM_BATTLE_BAT], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
				if(tmpm) m_initlgrp(tmpm, mtmp->mx, mtmp->my);
			}
		break;
		case S_GOLEM:
			if(mndx == PM_GROVE_GUARDIAN){
				if(!(mmflags & MM_NOGROUP)){
					if (mmflags & MM_BIGGROUP){
						if(!rn2(10)) makemon_full(&mons[rn2(2) ? PM_ELVENKING : PM_ELVENQUEEN], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
						if(rn2(4)) makemon_full(&mons[rn2(2) ? PM_ELF_LORD : PM_ELF_LADY], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
						for(num = rn2(5); num >= 0; num--) makemon_full(&mons[PM_GREY_ELF], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
						for(num = d(2,4); num >= 0; num--) makemon_full(&mons[PM_WOODLAND_ELF], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
					}
				}
			}
		break;
		case S_NEU_OUTSIDER:
			if(mndx == PM_CENTER_OF_ALL){
			    mtmp->minvis = TRUE;
			    mtmp->perminvis = TRUE;
			}
			if(mndx == PM_SHARAB_KAMEREL){
				set_mimic_sym(mtmp);
			}
			if(mndx == PM_HUDOR_KAMEREL){
				if(is_pool(mtmp->mx,mtmp->my, TRUE)){
					mtmp->minvis = TRUE;
					mtmp->perminvis = TRUE;
				}
			}
		break;
		case S_PUDDING:
			if(mndx == PM_DARKNESS_GIVEN_HUNGER){
				mtmp->mhpmax = 3*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			    mtmp->minvis = TRUE;
			    mtmp->perminvis = TRUE;
			}
			if(mndx == PM_LIVING_MIRAGE){
			    // mtmp->minvis = TRUE;
			    // mtmp->perminvis = TRUE;
				set_mimic_sym(mtmp);
			}
		break;
		case S_DRAGON:
			if(mndx == PM_CHROMATIC_DRAGON){
				mtmp->mhpmax = 3*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			}
			else if(mndx == PM_PLATINUM_DRAGON){
				mtmp->mhpmax = 3*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			}
		break;
		case S_PLANT:
			if(mndx == PM_RAZORVINE){
				mtmp->mhpmax = .5*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			}
		break;
		case S_ZOMBIE:
			if (mndx == PM_DREAD_SERAPH){
				mtmp->m_lev = max(mtmp->m_lev,30);
				mtmp->mhpmax = 4*8*mtmp->m_lev;
				mtmp->mhp = mtmp->mhpmax;
			}
		break;
		case S_DEMON:
//			pline("%d\n",mtmp->mhpmax);
			if(mndx == PM_JUIBLEX){
				mtmp->mhpmax = 4*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			}
			else if(mndx == PM_ZUGGTMOY){
				mtmp->mhpmax = 3*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			}
			else if(mndx == PM_ASMODEUS){
				mtmp->mhpmax = 2*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			}
			else if(mndx == PM_DEMOGORGON){
				mtmp->mhpmax = 2*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
				pline("Demogorgon, Prince of Demons, is near!");
				com_pager(200);
			}
			else if(mndx == PM_LAMASHTU){
				mtmp->mhpmax = 2*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
				pline("Lamashtu, the Demon Queen, is near!");
				com_pager(201);
			}
			else if(mndx == PM_DURIN_S_BANE){
				mtmp->mhpmax = 3*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			}
			else if(mndx == PM_LUNGORTHIN){
				mtmp->mhpmax = 3*mtmp->mhpmax;
				mtmp->mhp = mtmp->mhpmax;
			}
			else if(mndx == PM_PALE_NIGHT){
				mtmp->mvar_paleWarning = 0;
			}
			if(mndx == PM_INCUBUS){
				if(Is_grazzt_level(&u.uz)){
					if(rn2(2)){
						set_faction(mtmp, INCUBUS_FACTION);
					}
				}
			}
			if(mndx == PM_SUCCUBUS){
				if(Is_malcanthet_level(&u.uz)){
					if(rn2(2)){
						set_faction(mtmp, SUCCUBUS_FACTION);
					}
				}
				else if(In_mordor_quest(&u.uz) 
					&& !In_mordor_forest(&u.uz)
					&& !Is_ford_level(&u.uz)
					&& !In_mordor_fields(&u.uz)
					&& in_mklev
				){
					set_faction(mtmp, SUCCUBUS_FACTION);
				}
			}
			if(mndx == PM_ANCIENT_OF_DEATH){
			    mtmp->minvis = TRUE;
			    mtmp->perminvis = TRUE;
			}
			if(mndx == PM_HELLCAT){
				if(!isdark(mtmp->mx,mtmp->my)){
					mtmp->minvis = TRUE;
					mtmp->perminvis = TRUE;
				}
			    mtmp->invis_blkd = TRUE;
			}
			if(mndx == PM_GRUE){
				if(isdark(mtmp->mx,mtmp->my)){
					mtmp->minvis = TRUE;
					mtmp->perminvis = TRUE;
				}
			    mtmp->invis_blkd = TRUE;
			}
			if(mndx == PM_INVIDIAK){
				if(isdark(mtmp->mx,mtmp->my)){
					mtmp->minvis = TRUE;
					mtmp->perminvis = TRUE;
				}
			    mtmp->invis_blkd = TRUE;
			}
			if(mndx == PM_WALKING_DELIRIUM && !ClearThoughts){
				mtmp->mappearance = select_newcham_form(mtmp);
				mtmp->m_ap_type = M_AP_MONSTER;
			}
//			pline("%d\n",mtmp->mhpmax);
		break;
	}
	if (mtmp->mtemplate == ZOMBIFIED && (mmflags & MM_BIGGROUP)) {
		/* zombies get a group, with size dependent on how nasty the monster is for the current floor */
		int groupsz = max(2, min(12, 3*level_difficulty()/(monstr[ptr->mtyp]+2)));
		m_initgrp(mtmp, mtmp->mx, mtmp->my, groupsz);
	}
	if ((ct = emits_light_mon(mtmp)) > 0)
		new_light_source(LS_MONSTER, (genericptr_t)mtmp, ct);
	mitem = 0;	/* extra inventory item for this monster */

	if ((mcham = pm_to_cham(mndx)) != CHAM_ORDINARY) {
		/* If you're protected with a ring, don't create
		 * any shape-changing chameleons -dgk
		 */
		if (Protection_from_shape_changers)
			mtmp->cham = CHAM_ORDINARY;
		else {
			mtmp->cham = mcham;
			(void)newcham(mtmp, select_newcham_form(mtmp), FALSE, FALSE);
		}
	} else if (mndx == PM_WIZARD_OF_YENDOR) {
		mtmp->iswiz = TRUE;
		mtmp->mspec_used = 0; /*Wizard can cast spells right off the bat*/
		flags.no_of_wizards++;
		if (flags.no_of_wizards == 1 && Is_earthlevel(&u.uz))
			mitem = SPE_DIG;
	} else if (mndx == PM_DJINNI) {
		flags.djinni_count++;
	} else if (mndx == PM_GHOST) {
		flags.ghost_count++;
		if (!(mmflags & MM_NONAME))
			mtmp = christen_monst(mtmp, rndghostname());
	} else if (mndx == PM_VLAD_THE_IMPALER) {
		mitem = CANDELABRUM_OF_INVOCATION;
	} else if (ptr->msound == MS_NEMESIS && !(Race_if(PM_DROW) && !Role_if(PM_NOBLEMAN)) ){
		flags.made_bell = TRUE;
		mitem = BELL_OF_OPENING;
	} else if (mndx == PM_DEATH) {
		if(Role_if(PM_EXILE)){
			makemon_full(&mons[PM_BINAH_SEPHIRAH], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			makemon_full(&mons[PM_BINAH_SEPHIRAH], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			makemon_full(&mons[PM_BINAH_SEPHIRAH], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
		}
	} else if (mndx == PM_FAMINE) {
		if(Role_if(PM_EXILE)){
			makemon_full(&mons[PM_BINAH_SEPHIRAH], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			makemon_full(&mons[PM_BINAH_SEPHIRAH], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			makemon_full(&mons[PM_BINAH_SEPHIRAH], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
		}
	} else if (mndx == PM_PESTILENCE) {
		mitem = POT_SICKNESS;
		if(Role_if(PM_EXILE)){
			makemon_full(&mons[PM_BINAH_SEPHIRAH], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			makemon_full(&mons[PM_BINAH_SEPHIRAH], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
			makemon_full(&mons[PM_BINAH_SEPHIRAH], mtmp->mx, mtmp->my, MM_ADJACENTOK, template, faction);
		}
	}
	if (mitem) (void) mongets(mtmp, mitem, mkobjflags);
	
	if(in_mklev) {
		if(((is_ndemon(ptr)) ||
		    (mndx == PM_WUMPUS) ||
		    (mndx == PM_LONG_WORM) ||
		    (mndx == PM_GIANT_EEL)) && !u.uhave.amulet && rn2(5))
			mtmp->msleeping = TRUE;
	} else {
		newsym(mtmp->mx,mtmp->my);
		set_apparxy(mtmp);
	}
	if((is_dprince(ptr) || is_dlord(ptr)) && ptr->msound == MS_BRIBE) {
	    mtmp->mpeaceful = mtmp->minvis = mtmp->perminvis = 1;
	    mtmp->mavenge = 0;
	    if (uwep && (
			uwep->oartifact == ART_EXCALIBUR
			|| uwep->oartifact == ART_LANCE_OF_LONGINUS
		) ) mtmp->mpeaceful = mtmp->mtame = FALSE;
	}
#ifndef DCC30_BUG
	if((mndx == PM_LONG_WORM || mndx == PM_HUNTING_HORROR) && 
		(mtmp->wormno = get_wormno()) != 0)
#else
	/* DICE 3.0 doesn't like assigning and comparing mtmp->wormno in the
	 * same expression.
	 */
	if ((mndx == PM_LONG_WORM || mndx == PM_HUNTING_HORROR) &&
		(mtmp->wormno = get_wormno(), mtmp->wormno != 0))
#endif
	{
	    /* we can now create worms with tails - 11/91 */
	    initworm(mtmp, mndx == PM_HUNTING_HORROR ? 2 : rn2(5));
	    if (count_wsegs(mtmp)) place_worm_tail_randomly(mtmp, x, y);
	}
	/* Delusions madness can hide the appearance of a monster */
	if (roll_madness(MAD_DELUSIONS) && mtmp->m_ap_type == M_AP_NOTHING && !(
			mtmp->mtyp == PM_LIVING_DOLL || mtmp->data->msound == MS_GLYPHS))
	{
		struct permonst *delusion = rndmonst();
		if(delusion){
			mtmp->m_ap_type = M_AP_MONSTER;
			mtmp->mappearance = delusion->mtyp;
			/* less commonly have very out-of-place appearances */
			while (mtmp->mappearance == mtmp->mtyp || !rn2(20))
				mtmp->mappearance = rn2(SPECIAL_PM);
		}
	}
	set_malign(mtmp);		/* having finished peaceful changes */
	if(u.uevent.uaxus_foe && (mndx <= PM_QUINON && mndx >= PM_MONOTON)){
		mtmp->mpeaceful = mtmp->mtame = FALSE;
	}
	if(mmflags & MM_BIGGROUP) {
	    if ((ptr->geno & G_SGROUP) && rn2(2)) {
			m_initsgrp(mtmp, mtmp->mx, mtmp->my);
	    } else if (ptr->geno & G_LGROUP) {
			if(mndx != PM_MALKUTH_SEPHIRAH){ /* Malkuths are tough, but are generated in large numbers by shopkeeper code */
				if(rn2(3))  m_initlgrp(mtmp, mtmp->mx, mtmp->my);
				else	    m_initsgrp(mtmp, mtmp->mx, mtmp->my);
			} else {
				if(!rn2(3)) m_initsgrp(mtmp, mtmp->mx, mtmp->my);
			}
	    }
	}
	
	if (allow_minvent) {
	    if(is_armed_mon(mtmp))
			m_initweap(mtmp, mkobjflags, faction, goodequip);	/* equip with weapons / armor */
	    m_initinv(mtmp, mkobjflags, faction, goodequip);  /* add on a few special items incl. more armor */
	    m_dowear(mtmp, TRUE);
		init_mon_wield_item(mtmp);
	} else {
	    /* no initial inventory is allowed */
	    if (mtmp->minvent) discard_minvent(mtmp);
	    mtmp->minvent = (struct obj *)0;    /* caller expects this */
	}
	/* set weaponcheck for weapon-toting monsters */
	if (is_armed_mon(mtmp))
		mtmp->weapon_check = NEED_WEAPON;

	/* finished with these globals */
	zombiepm = -1;
	skeletpm = -1;

	if(unsethouse){
		/*At this point, we have FINALLY created the inventory for the initial creature and all its associates, so the global should be unset now.*/
		curhouse = 0;
	}
	if ((ptr->mflagst & MT_WAITMASK) && !(mmflags & MM_NOWAIT) && !u.uevent.invoked) {
		if (ptr->mflagst & MT_WAITFORU)
			mtmp->mstrategy |= STRAT_WAITFORU;
		if (ptr->mflagst & MT_CLOSE)
			mtmp->mstrategy |= STRAT_CLOSE;
	}

	if (!in_mklev)
	    newsym(mtmp->mx,mtmp->my);	/* make sure the mon shows up */

	return(mtmp);
}

void
set_curhouse(house)
	int house;
{
	curhouse = house;
}

int
mbirth_limit(mndx)
int mndx;
{
	/* assert(MAXMONNO < 255); */
	return (mndx == PM_NAZGUL ? 9 : /*mndx == PM_ERINYS ? 3 :*/ mndx == PM_GARO_MASTER ? 1 : mndx == PM_METROID ? 21
		: mndx == PM_ALPHA_METROID ? 45 : mndx == PM_GAMMA_METROID ? 48 : mndx == PM_ZETA_METROID ? 9 
		: mndx == PM_OMEGA_METROID ? 12 : mndx == PM_METROID_QUEEN ? 3 : mndx == PM_ARGENTUM_GOLEM ? 8 
		: mndx == PM_ALHOON ? 2 : mndx == PM_CENTER_OF_ALL ? 1 : mndx == PM_SOLDIER ? 250
		: mndx == PM_ANCIENT_OF_ICE ? 8 : mndx == PM_ANCIENT_OF_DEATH ? 4
		: mndx == PM_SOLDIER_ANT ? 250 : MAXMONNO); 
}

/* used for wand/scroll/spell of create monster */
/* returns TRUE iff you know monsters have been created */
boolean
create_critters(cnt, mptr)
int cnt;
struct permonst *mptr;		/* usually null; used for confused reading */
{
	coord c;
	int x, y;
	struct monst *mon;
	boolean known = FALSE;
#ifdef WIZARD
	boolean ask = wizard;
#endif

	while (cnt--) {
#ifdef WIZARD
	    if (ask) {
		if (create_particular(-1, -1, TRUE, 0, 0, 0)) {
		    known = TRUE;
		    continue;
		}
		else ask = FALSE;	/* ESC will shut off prompting */
	    }
#endif
	    x = u.ux,  y = u.uy;
	    /* if in water, try to encourage an aquatic monster
	       by finding and then specifying another wet location */
	    if (!mptr && u.uinwater && enexto(&c, x, y, &mons[PM_GIANT_EEL]))
		x = c.x,  y = c.y;

	    mon = makemon(mptr, x, y, NO_MM_FLAGS);
	    if (mon && canspotmon(mon)) known = TRUE;
	}
	return known;
}

#endif /* OVL1 */
#ifdef OVL0

STATIC_OVL boolean
uncommon(mndx)
int mndx;
{
	if (mons[mndx].geno & (G_NOGEN | G_UNIQ)) return TRUE;
	if (mvitals[mndx].mvflags & G_GONE && !In_quest(&u.uz)) return TRUE;
	if (Inhell)
		return((mons[mndx].geno & G_PLANES) != 0);
	else if (In_endgame(&u.uz))
		return((mons[mndx].geno & G_HELL) != 0);
	else
		return((mons[mndx].geno & (G_HELL|G_PLANES)) != 0);
}

/*
 *	shift the probability of a monster's generation by
 *	comparing the dungeon alignment and monster alignment.
 *	return an integer in the range of 0-5.
 */
STATIC_OVL int
align_shift(ptr)
register struct permonst *ptr;
{
    static NEARDATA long oldmoves = 0L;	/* != 1, starting value of moves */
    static NEARDATA s_level *lev;
    register int alshift;

    if(oldmoves != moves) {
	lev = Is_special(&u.uz);
	oldmoves = moves;
    }
    switch((lev) ? lev->flags.align : dungeons[u.uz.dnum].flags.align) {
    default:	/* just in case */
    case AM_NONE:	alshift = 0;
			break;
    case AM_LAWFUL:	alshift = (ptr->maligntyp+20)/(2*ALIGNWEIGHT);
			break;
    case AM_NEUTRAL:	alshift = (20 - abs(ptr->maligntyp))/ALIGNWEIGHT;
			break;
    case AM_CHAOTIC:	alshift = (-(ptr->maligntyp-20))/(2*ALIGNWEIGHT);
			break;
    }
    return alshift;
}

static NEARDATA struct {
	int choice_count;
	int mchoices[SPECIAL_PM];	/* not at ALL sure the monsters fit into a char, and in this age trying to save such a
								   small amount of memory seems outdated */
} rndmonst_state = { -1, {0} };

static int roguemons[] = {
	/*A*/
	PM_BAT,
	PM_PLAINS_CENTAUR,
	PM_RED_DRAGON,
	PM_STALKER,
	PM_LICHEN,
	PM_GNOME,
	/*H*/
	/*I(stalker, above)*/
	PM_JABBERWOCK,
	/*K*/
	/*L*/
	/*M*/
	/*N*/
	PM_OGRE,
	/*P*/
	/*Q*/
	PM_RUST_MONSTER,
	PM_PIT_VIPER,
	PM_TROLL,
	PM_UMBER_HULK,
	PM_VAMPIRE,
	PM_WRAITH,
	PM_XORN,
	PM_YETI,
	PM_ZOMBIE
};

STATIC_OVL
struct permonst *
roguemonst()
{
	int mn, tries=0;
	int zlevel, minmlev, maxmlev;
	
	zlevel = level_difficulty();
	/* determine the level of the weakest monster to make. */
	if(u.uevent.udemigod) minmlev = zlevel / 3;
	else minmlev = zlevel / 6;
	/* determine the level of the strongest monster to make. */
	maxmlev = (zlevel + u.ulevel) / 2;
	
	do mn = roguemons[rn2(SIZE(roguemons))];
	while(tooweak(mn, minmlev) || toostrong(mn,maxmlev) || tries++ > 40);
	return &mons[mn];
}

/* select a random monster type */
struct permonst *
rndmonst()
{
	register struct permonst *ptr;
	register int mndx, ct;
	int zlevel, minmlev, maxmlev;
	
	zlevel = level_difficulty();
	/* determine the level of the weakest monster to make. */
	if(u.uevent.udemigod) minmlev = zlevel / 3;
	else minmlev = zlevel / 6;
	/* determine the level of the strongest monster to make. */
	maxmlev = (zlevel + u.ulevel) / 2;

	if(u.ukinghill){ /* You have pirate quest artifact in open inventory */
		if(rnd(100)>80){
			if(In_endgame(&u.uz)) return &mons[PM_GITHYANKI_PIRATE];
			else if(Inhell) return &mons[PM_DAMNED_PIRATE];
			else return &mons[PM_SKELETAL_PIRATE];
		}
	}
	
	/*If Center of All hasn't been made yet, he may appear during level gen.
	  During normal play, he may also arvive on level from elsewhere. */
	if(in_mklev && mvitals[PM_CENTER_OF_ALL].born == 0 && !rn2(COA_PROB)){
	    return &mons[PM_CENTER_OF_ALL]; /*center of all may be created at any time */
	}

	if (u.uz.dnum == quest_dnum && (ptr = qt_montype()) != 0){
		if(ptr->mtyp == PM_LONG_WORM_TAIL) return (struct permonst *) 0;
	    else if(Role_if(PM_ANACHRONONAUT) || rn2(7)) return ptr;
		//else continue to random generation
	}
	else if (In_neu(&u.uz) && 
		(
			Is_rlyeh(&u.uz) ||  Is_sumall(&u.uz) || Is_gatetown(&u.uz) || 
			(rn2(10) && (In_outlands(&u.uz)))
		)
	){
	    if(!in_mklev) return neutral_montype();
		else return (struct permonst *)0;/*NOTE: ugly method to stop monster generation during level creation, since I can't find a better way*/
	}
	else if (In_cha(&u.uz)){
		if(In_mordor_quest(&u.uz)){
			return chaos3_montype();
		} else if(In_mithardir_quest(&u.uz)){
			return chaos2_montype();
		} else {
		    return chaos_montype();
		}
	}
	else if (In_law(&u.uz)){
	    return law_montype();
	}
	else if (In_mines(&u.uz)){
		int roll = d(1,10);
		if(Race_if(PM_GNOME) && Role_if(PM_RANGER) && rn2(2)){
			switch(roll){
				case 1:	case 2: case 3: case 4: return mkclass(S_KOBOLD, G_NOHELL); break;
				case 5:	case 6: return rn2(6) ? &mons[PM_GNOME] : &mons[PM_DWARF]; break;
				case 7: return &mons[PM_IMP]; break;
				default: break; //proceed with normal generation
			}
		} else {
			switch(roll){
				case 1:	case 2: case 3: case 4: return mkclass(S_GNOME, G_NOHELL); break;
				case 5:	case 6: return &mons[PM_DWARF]; break;
				case 7: return rn2(6) ? &mons[PM_GNOME] : &mons[PM_DWARF]; break;
				default: break; //proceed with normal generation
			}
		}
	}
	else if(Is_juiblex_level(&u.uz)){
		if(rn2(2)) return rn2(2) ? mkclass(S_BLOB, G_NOHELL|G_HELL) : mkclass(S_PUDDING, G_NOHELL|G_HELL);
		//else default
	}
	else if(Is_zuggtmoy_level(&u.uz)){
		if(rn2(2)) return rn2(3) ? mkclass(S_FUNGUS, G_NOHELL|G_HELL) : mkclass(S_PLANT, G_NOHELL|G_HELL);
		//else default
	}
	else if(Is_yeenoghu_level(&u.uz)){
		int roll = d(1,20);
		switch(roll){
			case 1:	case 2: case 3: case 4: 
			case 5:	case 6: case 7: case 8: return &mons[PM_GNOLL]; break;
			case 9:	return &mons[PM_ANUBITE]; break;
			case 10: case 11: return &mons[PM_GNOLL_GHOUL]; break;
			case 12: return &mons[PM_GNOLL_MATRIARCH]; break;
			default: break; //proceed with normal generation
		}
	}
	else if(Is_baphomet_level(&u.uz)){
		int roll = d(1,10);
		switch(roll){
			case 1:	case 2: return &mons[PM_MINOTAUR]; break;
			case 3: return &mons[PM_MINOTAUR_PRIESTESS]; break;
			default: break; //proceed with normal generation
		}
	}
	else if(Is_night_level(&u.uz)){
		if(rn2(2)) return rn2(2) ? mkclass(S_ZOMBIE, G_NOHELL|G_HELL) : &mons[PM_SKELETON];
		if(!rn2(20)) return mkclass(S_LICH, G_NOHELL|G_HELL);
		//else default
	}
	else if(Is_malcanthet_level(&u.uz)){
		if(!rn2(6)) return rn2(3) ? &mons[PM_SUCCUBUS] : &mons[PM_INCUBUS];
		//else default
	}
	else if(Is_grazzt_level(&u.uz)){
		if(!rn2(6)) return !rn2(3) ? &mons[PM_SUCCUBUS] : &mons[PM_INCUBUS];
		//else default
	}
	else if(Is_orcus_level(&u.uz)){
		if(rn2(2)) return !rn2(3) ? mkclass(S_WRAITH, G_NOHELL|G_HELL) : &mons[PM_SHADE];
		if(!rn2(20)) return mkclass(S_LICH, G_NOHELL|G_HELL);
		//else default
	}
	else if(Is_lolth_level(&u.uz)){
		int roll = d(1,10);
		if(roll == 10) return &mons[PM_DROW_MATRON];
		if(roll > 4) return mkclass(S_SPIDER, G_NOHELL|G_HELL);
	}
	else if(Is_demogorgon_level(&u.uz)){
		if(rn2(3)) return mkclass(S_DEMON, G_NOHELL|G_HELL) ;
		//else default
	}
	else if(Is_dagon_level(&u.uz)){
		if(rn2(2)){
			return rn2(3) ? &mons[PM_KRAKEN] : rn2(2) ? &mons[PM_SHARK] : &mons[PM_ELECTRIC_EEL];
		}
		//else default
	}

	if(u.hod && !rn2(10) && rn2(40+u.hod) > 50){
		u.hod-=10;
		if(u.hod<0) u.hod = 0;
		if(!tooweak(PM_HOD_SEPHIRAH, minmlev)){
			return &mons[PM_HOD_SEPHIRAH];
		}
		else u.keter++;
	}
	if(u.gevurah && !rn2(20) && rn2(u.gevurah + 94) > 100){
		/* Notes on frequency: cheating death via lifesaving counts as +4
			cheating death via rehumanization counts as +1*/
		if(!tooweak(PM_GEVURAH_SEPHIRAH, minmlev)){
			return &mons[PM_GEVURAH_SEPHIRAH];
		}
		else{
			u.gevurah -= 4;
			if(u.gevurah<0) u.gevurah = 0;
			u.keter++;
			return &mons[PM_CHOKHMAH_SEPHIRAH];
		}
	}
	if(u.keter && !rn2(100) && rn2(u.keter+10) > 10){
		u.chokhmah++;
		return &mons[PM_CHOKHMAH_SEPHIRAH];
	}
	if (u.uz.dnum == tower_dnum)
		switch(rn2(10)){
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				return mkclass(S_ZOMBIE, G_NOHELL);
			break;
			case 5:
			case 6:
				return mkclass(S_IMP, G_NOHELL);
			break;
			case 7:
				return mkclass(S_DOG,G_NOHELL);
			break;
			case 8:
				return mkclass(S_VAMPIRE, G_NOHELL);
			break;
			case 9:
				return mkclass(S_DEMON, G_HELL);
			break;
		}
	if (u.uz.dnum == tomb_dnum)
		return rn2(2) ? mkclass(S_ZOMBIE, G_NOHELL) : mkclass(S_MUMMY, G_NOHELL);

	if (u.uz.dnum == temple_dnum)
		return rn2(4) ? mkclass(S_ZOMBIE, G_NOHELL) : mkclass(S_BLOB, G_NOHELL);
	
	if(In_sea(&u.uz)){
		if (Is_sunsea(&u.uz))
			return rn2(3) ? &mons[PM_JELLYFISH] : rn2(2) ? &mons[PM_SHARK] : &mons[PM_GIANT_EEL];
		else if(Is_paradise(&u.uz)){
			switch(rn2(10)){
				case 0:
				case 1:
				case 2:
				case 3:
					return mkclass(S_EEL, G_NOHELL);
				break;
				case 4:
				case 5:
					return &mons[PM_PARROT];
				break;
				case 6:
					return mkclass(S_SNAKE, G_NOHELL);
				break;
				case 7:
					return &mons[PM_MONKEY];
				break;
				case 8:
					return mkclass(S_LIZARD,G_NOHELL);
				break;
				case 9:
					return mkclass(S_SPIDER,G_NOHELL);
				break;
			}
		}
		else if(Is_sunkcity(&u.uz)){
			switch(rn2(10)){
				case 0:
				case 1:
				case 2:
				case 3:
					return mkclass(S_EEL, G_NOHELL);
				break;
				case 4:
				case 5:
					return &mons[PM_DEEP_ONE];
				break;
				case 6:
					return &mons[PM_DEEPER_ONE];
				break;
				case 7:
				case 8:
					return &mons[PM_SOLDIER];
				break;
				case 9:
					return mkclass(S_DOG,G_NOHELL);
				break;
			}
		}
		else if(Is_peanut(&u.uz)){
			switch(rn2(10)){
				case 0:
				case 1:
					return mkclass(S_EEL, G_NOHELL);
				break;
				case 2:
				case 3:
				case 4:
					return &mons[PM_PARROT];
				break;
				case 5:
				case 6:
				case 7:
					return &mons[PM_SKELETAL_PIRATE];
				break;
				case 8:
					return &mons[PM_MONKEY];
				break;
				case 9:
					return mkclass(S_LIZARD,G_NOHELL);
				break;
			}
		}
	}
	if(Is_rogue_level(&u.uz))
		return roguemonst();

	if (rndmonst_state.choice_count < 0) {	/* need to recalculate */
	    int zlevel, minmlev, maxmlev;
	    boolean elemlevel;

	    rndmonst_state.choice_count = 0;
	    /* look for first common monster */
	    for (mndx = LOW_PM; mndx < SPECIAL_PM; mndx++) {
		if (!uncommon(mndx)) break;
		rndmonst_state.mchoices[mndx] = 0;
	    }		
	    if (mndx == SPECIAL_PM) {
		/* evidently they've all been exterminated */
#ifdef DEBUG
		pline("rndmonst: no common mons!");
#endif
		return (struct permonst *)0;
	    } /* else `mndx' now ready for use below */
	    zlevel = level_difficulty();
	    /* determine the level of the weakest monster to make. */
	    minmlev = zlevel / 6;
	    /* determine the level of the strongest monster to make. */
	    maxmlev = (zlevel + u.ulevel) / 2;
	    elemlevel = In_endgame(&u.uz) && !Is_astralevel(&u.uz);

/*
 *	Find out how many monsters exist in the range we have selected.
 */
	    /* (`mndx' initialized above) */
	    for ( ; mndx < SPECIAL_PM; mndx++) {
		ptr = &mons[mndx];
		rndmonst_state.mchoices[mndx] = 0;
		if (tooweak(mndx, minmlev) || toostrong(mndx, maxmlev))
		    continue;
		if (elemlevel && wrong_elem_type(ptr)) continue;
		if (uncommon(mndx)) continue;
		if (Inhell && (ptr->geno & G_NOHELL)) continue;
		if (!In_endgame(&u.uz) && ((ptr->geno & (G_PLANES|G_HELL|G_NOHELL)) == G_PLANES)) continue;
		ct = (int)(ptr->geno & G_FREQ) + align_shift(ptr);
		if (ct < 0 || ct > 127)
		    panic("rndmonst: bad count [#%d: %d]", mndx, ct);
		rndmonst_state.choice_count += ct;
		rndmonst_state.mchoices[mndx] = (char)ct;
	    }
/*
 *	    Possible modification:  if choice_count is "too low",
 *	    expand minmlev..maxmlev range and try again.
 */
	} /* choice_count+mchoices[] recalc */

	if (rndmonst_state.choice_count <= 0) {
	    /* maybe no common mons left, or all are too weak or too strong */
#ifdef DEBUG
	    Norep("rndmonst: choice_count=%d", rndmonst_state.choice_count);
#endif
	    return (struct permonst *)0;
	}

/*
 *	Now, select a monster at random.
 */
	ct = rnd(rndmonst_state.choice_count);
	for (mndx = LOW_PM; mndx < SPECIAL_PM; mndx++)
	    if ((ct -= (int)rndmonst_state.mchoices[mndx]) <= 0) break;

	if (mndx == SPECIAL_PM || uncommon(mndx)) {	/* shouldn't happen */
	    impossible("rndmonst: bad `mndx' [#%d]", mndx);
	    return (struct permonst *)0;
	}
	return &mons[mndx];
}

/* select a random monster type for a shapeshifter to turn into */
/* optional: give a function that takes an mtyp and returns TRUE if it meets conditions */
int
rndshape(extra_req)
boolean FDECL((*extra_req), (int));
{
	register struct permonst *ptr;
	register int mndx, ct;
	int zlevel, minmlev, maxmlev;
	int choice_count;
	int mchoices[SPECIAL_PM];
	boolean elemlevel;
	
	zlevel = level_difficulty();
	
	/*increase difficulty to allow mildly out of depth monsters */
	zlevel += 12;
	
	/* determine the level of the weakest monster to make. */
	minmlev = zlevel / 6;
	/* determine the level of the strongest monster to make. */
	maxmlev = (zlevel + u.ulevel) / 2;

#ifdef REINCARNATION
	boolean upper;
#endif

	choice_count = 0;
	/* look for first common monster */
	for (mndx = LOW_PM; mndx < SPECIAL_PM; mndx++) {
		if (!uncommon(mndx)) break;
		mchoices[mndx] = 0;
	}		
	if (mndx == SPECIAL_PM) {
	/* evidently they've all been exterminated */
#ifdef DEBUG
	pline("rndmonst: no common mons!");
#endif
	return NON_PM;
	} /* else `mndx' now ready for use below */
#ifdef REINCARNATION
	upper = Is_rogue_level(&u.uz);
#endif
	elemlevel = In_endgame(&u.uz) && !Is_astralevel(&u.uz);

/*
*	Find out how many monsters exist in the range we have selected.
*/
	/* (`mndx' initialized above) */
	for ( ; mndx < SPECIAL_PM; mndx++) {
	ptr = &mons[mndx];
	mchoices[mndx] = 0;
	if (tooweak(mndx, minmlev) || toostrong(mndx, maxmlev))
		continue;
	if (extra_req && !(*extra_req)(mndx))
		continue;
#ifdef REINCARNATION
	if (upper && !isupper(def_monsyms[(int)(ptr->mlet)])) continue;
#endif
	if (elemlevel && wrong_elem_type(ptr)) continue;
	if (uncommon(mndx)) continue;
	ct = (int)(ptr->geno & G_FREQ)/2 + align_shift(ptr);
	if (ct < 0 || ct > 127)
		panic("rndmonst: bad count [#%d: %d]", mndx, ct);
	choice_count += ct;
	mchoices[mndx] = (char)ct;
	}
/*
 *	    Possible modification:  if choice_count is "too low",
 *	    expand minmlev..maxmlev range and try again.
 */

	if (choice_count <= 0) {
	    /* maybe no common mons left, or all are too weak or too strong */
#ifdef DEBUG
	    Norep("rndmonst: choice_count=%d", choice_count);
#endif
	    return NON_PM;
	}

/*
 *	Now, select a monster at random.
 */
	ct = rnd(choice_count);
	for (mndx = LOW_PM; mndx < SPECIAL_PM; mndx++)
	    if ((ct -= (int)mchoices[mndx]) <= 0) break;

	if (mndx == SPECIAL_PM || uncommon(mndx)) {	/* shouldn't happen */
	    impossible("rndmonst: bad `mndx' [#%d]", mndx);
	    return NON_PM;
	}
	return mndx;
}



/* called when you change level (experience or dungeon depth) or when
   monster species can no longer be created (genocide or extinction) */
void
reset_rndmonst(mndx)
int mndx;	/* particular species that can no longer be created */
{
	/* cached selection info is out of date */
	if (mndx == NON_PM) {
	    rndmonst_state.choice_count = -1;	/* full recalc needed */
	} else if (mndx < SPECIAL_PM) {
	    rndmonst_state.choice_count -= rndmonst_state.mchoices[mndx];
	    rndmonst_state.mchoices[mndx] = 0;
	} /* note: safe to ignore extinction of unique monsters */
}

#endif /* OVL0 */
#ifdef OVL1

/*	The routine below is used to make one of the multiple types
 *	of a given monster class.  The second parameter specifies a
 *	special casing bit mask to allow the normal genesis
 *	masks to be deactivated.  Returns 0 if no monsters
 *	in that class can be made.
 */

struct permonst *
mkclass(class,spc)
char	class;
int	spc;
{
	register int	first, last, num = 0;
	int maxmlev, mask = (G_PLANES | G_NOHELL | G_HELL | G_NOGEN | G_UNIQ) & ~spc;

	maxmlev = (level_difficulty() + u.ulevel)/2+1;
	if(class < 1 || class >= MAXMCLASSES) {
	    impossible("mkclass called with bad class!");
	    return((struct permonst *) 0);
	}
	if(Race_if(PM_DROW) && (Role_if(PM_PRIEST) || Role_if(PM_ROGUE) || Role_if(PM_RANGER) || Role_if(PM_WIZARD)) &&
		!flags.initgend && flags.stag == 0 && In_quest(&u.uz) && class == S_MUMMY && !(mvitals[PM_DROW_MUMMY].mvflags & G_GENOD && !In_quest(&u.uz))
	) return &mons[PM_DROW_MUMMY];
	if(Race_if(PM_DROW) && (Role_if(PM_PRIEST) || Role_if(PM_ROGUE) || Role_if(PM_RANGER) || Role_if(PM_WIZARD)) &&
		!flags.initgend && flags.stag == 0 && In_quest(&u.uz) && class == S_ZOMBIE && !(mvitals[PM_HEDROW_ZOMBIE].mvflags & G_GENOD && !In_quest(&u.uz))
	) return &mons[PM_HEDROW_ZOMBIE];
	if(class == S_ZOMBIE)
		return mkzombie();
/*	Assumption #1:	monsters of a given class are contiguous in the
 *			mons[] array.
 */
	for (first = LOW_PM; first < SPECIAL_PM; first++)
	    if (mons[first].mlet == class && !(mons[first].geno & mask)) break;
	if (first == SPECIAL_PM) return (struct permonst *) 0;

	for (last = first;
		last < SPECIAL_PM && mons[last].mlet == class; last++)
	    if (!(mvitals[last].mvflags & G_GONE && !In_quest(&u.uz)) && !(mons[last].geno & mask)
					&& !is_placeholder(&mons[last])) {
		/* consider it */
		if(num && toostrong(last, maxmlev) &&
		   monstr[last] != monstr[last-1] && (rn2(2) || monstr[last] > maxmlev+5)
		) break;
		num += mons[last].geno & G_FREQ;
	    }

	if(!num) return((struct permonst *) 0);

/*	Assumption #2:	monsters of a given class are presented in ascending
 *			order of strength.
 */
	for(num = rnd(num); num > 0; first++)
	    if (!(mvitals[first].mvflags & G_GONE && !In_quest(&u.uz)) && !(mons[first].geno & mask)
					&& !is_placeholder(&mons[first])) {
		/* skew towards lower value monsters at lower exp. levels */
		num -= mons[first].geno & G_FREQ;
		if (num && adj_lev(&mons[first]) > (u.ulevel*2)) {
		    /* but not when multiple monsters are same level */
		    if (mons[first].mlevel != mons[first+1].mlevel)
			num--;
		}
	    }
	first--; /* correct an off-by-one error */

	return(&mons[first]);
}

static const int standardZombies[] = {
					  PM_KOBOLD, //1
					  PM_HUMAN, //2
					  PM_GNOME, //3
					  PM_ORC, //3
					  PM_DWARF, //4
					  PM_DOG, //?
					  PM_URUK_HAI, //5
					  PM_ELF, //6
					  PM_OGRE, //7
					  PM_GIANT, //8
					  PM_ETTIN, //13
					  PM_HALF_DRAGON, //16
					  PM_MASTODON //23
					};

static const int orcusZombies[] = {
					  PM_WEREWOLF,
					  PM_MANTICORE,
					  PM_TITANOTHERE,
					  PM_BALUCHITHERIUM,
					  PM_MASTODON,
					  PM_LONG_WORM,
					  PM_PURPLE_WORM,
					  PM_JABBERWOCK
					};

static const int orcusSkeletons[] = {
					  PM_WINGED_KOBOLD,
					  PM_SOLDIER,
					  PM_HILL_ORC,
					  PM_DWARF,
					  PM_BUGBEAR,
					  PM_DOG,
					  PM_URUK_HAI,
					  PM_ELF,
					  PM_OGRE,
					  PM_GIANT,
					  PM_ETTIN,
					  PM_HALF_DRAGON
					};

static const int granfaloonZombies[] = {
						// PM_NOBLEMAN,
						// PM_NOBLEWOMAN,
						// PM_PRIEST,
						// PM_PRIESTESS,
						PM_CAPTAIN,
						PM_DWARF_KING,
						PM_DWARF_QUEEN,
						PM_GNOME_KING,
						PM_GNOME_QUEEN,
						PM_ELVENKING,
						PM_ELVENQUEEN
						// PM_DROW_MATRON
					};
static const int elfZombies[] = {
						PM_WOODLAND_ELF,
						PM_GREEN_ELF,
						PM_GREY_ELF,
						PM_ELF_LORD,
						PM_ELF_LADY,
						PM_HILL_ORC,
						PM_MORDOR_ORC,
						PM_ANGBAND_ORC
					};

static const int futureZombies[] = {
						//Chiros
						PM_GIANT_BAT,
						PM_BATTLE_BAT,
						PM_WARBAT,
						PM_CHIROPTERAN,
						//Clock?
						//Dwarves
						PM_DWARF,
						PM_DWARF,
						PM_DWARF_LORD,
						PM_DWARF_LORD,
						PM_DWARF_CLERIC,
						//Dragons
						PM_HALF_DRAGON,
						PM_MAN_SERPENT_SOLDIER,
						PM_MAN_SERPENT_MAGE,
						PM_MAN_SERPENT_GIANT,
						PM_HALF_STONE_DRAGON,
						PM_PISACA,
						//No drow
						//Elves
						PM_WOODLAND_ELF,
						PM_GREEN_ELF,
						PM_GREEN_ELF,
						PM_GREY_ELF,
						PM_HIGH_ELF,
						//Gnomes
						PM_GNOME,
						PM_GNOME_LORD,
						PM_GNOME_LADY,
						PM_GNOME_KING,
						PM_GNOME_QUEEN,
						//Humans
						PM_HUMAN,
						PM_SOLDIER,
						PM_SERGEANT,
						//Incants
						PM_INCANTIFIER,
						//Orcs
						PM_HILL_ORC,
						PM_MORDOR_ORC,
						PM_ORC_CAPTAIN,
						PM_MORDOR_ORC_ELITE,
						PM_MORDOR_MARSHAL,
						PM_ANGBAND_ORC,
						//Nymphs
						PM_DRYAD,
						PM_YUKI_ONNA,
						PM_THRIAE,
						PM_SELKIE,
						PM_OCEANID,
						PM_QUICKLING
					};

struct permonst *
mkzombie()
{
	register int	first, last, num = 0;
	int maxmlev;

	maxmlev = (u.ulevel + level_difficulty())/2;
	if(Is_orcus_level(&u.uz)){
		if(!rn2(2)){
			skeletpm = orcusSkeletons[rn2(SIZE(orcusSkeletons))];
			return &mons[skeletpm];
		} else {
			zombiepm = orcusZombies[rn2(SIZE(orcusZombies))];
			return &mons[zombiepm];
		}
	}
	if(In_quest(&u.uz) && Role_if(PM_EXILE)){
		(urace.zombienum != NON_PM) ? (zombiepm = urace.zombienum) : (zombiepm = PM_HUMAN);
		return &mons[zombiepm];
	}
	if(In_quest(&u.uz) && Role_if(PM_PIRATE)){
		zombiepm = PM_HUMAN;
		return &mons[zombiepm];
	}
	if(In_quest(&u.uz) && Role_if(PM_PRIEST) && urole.neminum == PM_LEGION && mvitals[PM_LEGION].died == 0){
		zombiepm = granfaloonZombies[rn2(SIZE(granfaloonZombies))];
		return &mons[zombiepm];
	}
	if(In_quest(&u.uz) && urole.neminum == PM_NECROMANCER){
		zombiepm = elfZombies[rn2(SIZE(elfZombies))];
		return &mons[zombiepm];
	}
	if(In_quest(&u.uz) && Role_if(PM_ANACHRONONAUT)){
		zombiepm = futureZombies[rn2(SIZE(futureZombies))];
		return &mons[zombiepm];
	}
	if(u.uz.dnum == tower_dnum){
		zombiepm = rn2(3) ? PM_PEASANT : PM_WOLF;
		return &mons[zombiepm];
	}
	// if(Race_if(PM_DROW) && (Role_if(PM_PRIEST) || Role_if(PM_ROGUE) || Role_if(PM_RANGER) || Role_if(PM_WIZARD)) &&
		// !flags.initgend && flags.stag == 0 && In_quest(&u.uz) && class == S_ZOMBIE && !(mvitals[PM_HEDROW_ZOMBIE].mvflags & G_GENOD && !In_quest(&u.uz))
	// ) return &mons[PM_HEDROW_ZOMBIE];
	// if(class == S_ZOMBIE)
		// return mkzombie();

	first = 0;

	for (last = first; last < SIZE(standardZombies); last++)
	    if (!(mvitals[standardZombies[last]].mvflags & G_GENOD && !In_quest(&u.uz)) ) {
		/* consider it */
		if(num && toostrong(standardZombies[last], maxmlev) &&
		   monstr[standardZombies[last]] != monstr[standardZombies[last-1]] && (rn2(2) || monstr[standardZombies[last]] > maxmlev+5)
		) break;
		num += mons[standardZombies[last]].geno & G_FREQ;
	    }

	if(!num) return((struct permonst *) 0);

/*	Assumption #2:	monsters of a given class are presented in ascending
 *			order of strength.
 */
	for(num = rnd(num); num > 0; first++)
	    if (!(mvitals[standardZombies[first]].mvflags & G_GENOD && !In_quest(&u.uz)) ) {
			num -= mons[standardZombies[first]].geno & G_FREQ;
	    }
	first--; /* correct an off-by-one error */
	
	zombiepm = standardZombies[first];
	 
	return(&mons[standardZombies[first]]);
}

int
adj_lev(ptr)	/* adjust strength of monsters based on u.uz and u.ulevel */
register struct permonst *ptr;
{
	int	tmp, tmp2;
	int scaling_mult = 1;
	if(is_eladrin(ptr)){
		scaling_mult = 2;
	}

	if (ptr->mtyp == PM_WIZARD_OF_YENDOR) {
		/* does not depend on other strengths, but does get stronger
		 * every time he is killed
		 */
		tmp = ptr->mlevel + mvitals[PM_WIZARD_OF_YENDOR].died;
		if (tmp > 49) tmp = 49;
		return tmp;
	}

	if((tmp = ptr->mlevel) > 49){
//		pline("HD: %d", ptr->mlevel);
		return tmp; /* "special" demons/devils */
	}
	tmp2 = (level_difficulty() - tmp);
	if(tmp2 < 0) tmp--;		/* if mlevel > u.uz decrement tmp */
	else tmp += ((scaling_mult*tmp2) / 5);		/* else increment 1 per five diff */

	tmp2 = (u.ulevel - ptr->mlevel);	/* adjust vs. the player */
	if(tmp2 > 0) tmp += ((scaling_mult*tmp2) / 4);		/* level as well */

	tmp2 = permonst_max_lev(ptr);	/* crude upper limit */
	if (tmp2 > 49) tmp2 = 49;		/* hard upper limit */
	return((tmp > tmp2) ? tmp2 : (tmp > 0 ? tmp : 0)); /* 0 lower limit */
}

#endif /* OVL1 */
#ifdef OVLB

void
give_grown_equipment(mtmp)
struct monst *mtmp;
{
	struct obj *otmp;
	switch(mtmp->mtyp){
		case PM_VROCK:
			switch(rn2(3)){
				case 0:
					otmp = mongets(mtmp, HALBERD, MKOBJ_NOINIT);
				break;
				case 1:
					otmp = mongets(mtmp, PARTISAN, MKOBJ_NOINIT);
				break;
				case 2:
					otmp = mongets(mtmp, FAUCHARD, MKOBJ_NOINIT);
				break;
			}
			if(otmp){
				otmp->opoisoned = OPOISON_ACID;
				set_material_gm(otmp, CHITIN);
				fix_object(otmp);
				otmp->spe = 3;
			}
			
			otmp = mongets(mtmp, PLATE_MAIL, MKOBJ_NOINIT);
			if(otmp){
				set_material_gm(otmp, CHITIN);
				fix_object(otmp);
				otmp->spe = 3;
			}
		break;
		case PM_MARILITH:
			otmp = mongets(mtmp, PLATE_MAIL, MKOBJ_NOINIT);
			if(otmp){
				set_material_gm(otmp, CHITIN);
				fix_object(otmp);
				otmp->spe = 3;
			}
			
			otmp = mongets(mtmp, HELMET, MKOBJ_NOINIT);
			if(otmp){
				set_material_gm(otmp, CHITIN);
				fix_object(otmp);
				otmp->spe = 3;
			}
			
			otmp = mongets(mtmp, GAUNTLETS, MKOBJ_NOINIT);
			if(otmp){
				set_material_gm(otmp, CHITIN);
				fix_object(otmp);
				otmp->spe = 3;
			}
			
			for(int i = 0; i < 6; i++){
				otmp = mksobj(BESTIAL_CLAW, MKOBJ_NOINIT);
				otmp->opoisoned = OPOISON_ACID;
				set_material_gm(otmp, CHITIN);
				fix_object(otmp);
				otmp->spe = 3;
				(void) mpickobj(mtmp, otmp);
			}
		break;
	}
	m_dowear(mtmp, TRUE);
}

struct permonst *
grow_up(mtmp, victim)	/* `mtmp' might "grow up" into a bigger version */
struct monst *mtmp, *victim;
{
	int oldtype, newtype, max_increase, cur_increase,
	    lev_limit, hp_threshold;
	struct permonst *ptr = mtmp->data;
	struct monst *bardmon;

	/* monster died after killing enemy but before calling this function */
	/* currently possible if killing a gas spore */
	if (mtmp->mhp <= 0)
	    return ((struct permonst *)0);

	/*Update wrathful weapons here*/
	if(victim && mtmp != victim){ //Currently used to give some monsters the final HP during special growing-up code.
		if(MON_WEP(mtmp) && check_oprop(MON_WEP(mtmp), OPROP_WRTHW)){
			struct obj *otmp = MON_WEP(mtmp);
			if(wrath_target(otmp, victim)){
				if((otmp->wrathdata&0x3L) < 3) otmp->wrathdata++;
			}
			else {
				if(has_template(victim, ZOMBIFIED)){
					otmp->wrathdata = PM_ZOMBIE<<2;
				} else if(has_template(victim, SKELIFIED)){
					otmp->wrathdata = PM_SKELETON<<2;
				} else if(has_template(victim, VAMPIRIC)){
					otmp->wrathdata = PM_VAMPIRE<<2;
				} else if(has_template(victim, PSEUDONATURAL)){
					otmp->wrathdata = PM_MIND_FLAYER<<2;
				} else {
					otmp->wrathdata = monsndx(victim->data)<<2;
				}
			}
		}
		if(MON_SWEP(mtmp) && check_oprop(MON_SWEP(mtmp), OPROP_WRTHW)){
			struct obj *otmp = MON_SWEP(mtmp);
			if(wrath_target(otmp, victim)){
				if((otmp->wrathdata&0xFF) < 3) otmp->wrathdata++;
			}
			else {
				if(has_template(victim, ZOMBIFIED)){
					otmp->wrathdata = PM_ZOMBIE<<2;
				} else if(has_template(victim, SKELIFIED)){
					otmp->wrathdata = PM_SKELETON<<2;
				} else if(has_template(victim, VAMPIRIC)){
					otmp->wrathdata = PM_VAMPIRE<<2;
				} else if(has_template(victim, PSEUDONATURAL)){
					otmp->wrathdata = PM_MIND_FLAYER<<2;
				} else {
					otmp->wrathdata = monsndx(victim->data)<<2;
				}
			}
		}
		// mtmp killed a mummy and suffers from its curse.
		if(attacktype_fordmg(victim->data, AT_NONE, AD_MROT)){
			mummy_curses_x(victim, mtmp);
			// Mummy curse killed mtmp
			if(DEADMONSTER(mtmp))
				return (struct permonst *)0;
		}
	}
	
	if(mtmp->m_lev > 50 || ptr->mtyp == PM_CHAOS) return ptr;
	/* note:  none of the monsters with special hit point calculations
	   have both little and big forms */
	oldtype = monsndx(ptr);
	
	if(mtmp->mtyp == PM_STRANGE_LARVA && mtmp->mvar_tanninType)
		newtype = mtmp->mvar_tanninType;
	else
		newtype = little_to_big(oldtype, (boolean)mtmp->female);

	/* growth limits differ depending on method of advancement */
	if (victim) {		/* killed a monster */
	    /*
	     * The HP threshold is the maximum number of hit points for the
	     * current level; once exceeded, a level will be gained.
	     * Possible bug: if somehow the hit points are already higher
	     * than that, monster will gain a level without any increase in HP.
	     */
	    hp_threshold = mtmp->m_lev * 8;		/* normal limit */
	    if (!mtmp->m_lev)
			hp_threshold = 4;
	    else if (is_golem(ptr))	/* strange creatures */
			hp_threshold = ((mtmp->mhpmax / 10) + 1) * 10 - 1;
	    else if (is_home_elemental(ptr) || 
			ptr->mtyp == PM_DARKNESS_GIVEN_HUNGER ||
			ptr->mtyp == PM_WATCHER_IN_THE_WATER ||
			ptr->mtyp == PM_KETO ||
			ptr->mtyp == PM_DRACAE_ELADRIN ||
			ptr->mtyp == PM_MOTHERING_MASS ||
			ptr->mtyp == PM_DURIN_S_BANE ||
			ptr->mtyp == PM_LUNGORTHIN ||
			ptr->mtyp == PM_CHROMATIC_DRAGON ||
			ptr->mtyp == PM_BOLG ||
			ptr->mtyp == PM_PRIEST_OF_GHAUNADAUR ||
			ptr->mtyp == PM_SHOGGOTH
		) hp_threshold *= 3;
	    else if (ptr->mtyp == PM_RAZORVINE) hp_threshold *= .5;
		else if(ptr->mtyp == PM_CHAOS) hp_threshold *= 15;
		else if(ptr->mtyp == PM_KARY__THE_FIEND_OF_FIRE) hp_threshold *= 10;
		else if(ptr->mtyp == PM_LICH__THE_FIEND_OF_EARTH) hp_threshold *= 10;
		else if(ptr->mtyp == PM_KRAKEN__THE_FIEND_OF_WATER) hp_threshold *= 10;
		else if(ptr->mtyp == PM_TIAMAT__THE_FIEND_OF_WIND) hp_threshold *= 10;
		else if(ptr->mtyp == PM_CHOKHMAH_SEPHIRAH) hp_threshold *= u.chokhmah;

	    lev_limit = mon_max_lev(mtmp);

	    /* If they can grow up, be sure the level is high enough for that */
	    if (oldtype != newtype && mons[newtype].mlevel > lev_limit)
		lev_limit = (int)mons[newtype].mlevel;
	    // /* number of hit points to gain; unlike for the player, we put
	       // the limit at the bottom of the next level rather than the top */
	    // max_increase = rnd((int)victim->m_lev + 1);
	    // if (mtmp->mhpmax + max_increase > hp_threshold + 1)
			// max_increase = max((hp_threshold + 1) - mtmp->mhpmax, 0);
	    // cur_increase = (max_increase > 0) ? rn2(max_increase)+1 : 0;
		if(mtmp->mhpmax < hp_threshold-8 || mtmp->m_lev < victim->m_lev + (d(2,5) + heal_mlevel_bonus())){ /*allow monsters to quickly gain hp up to around their HP limit*/
			max_increase = 1;
			cur_increase = 1;
			if(Role_if(PM_BARD) && mtmp->mtame && canseemon(mtmp)){
				u.pethped = TRUE;
			}
			for(bardmon = fmon; bardmon; bardmon = bardmon->nmon){
				if(is_bardmon(bardmon->data) 
					&& !is_bardmon(mtmp->data) 
					&& !DEADMONSTER(bardmon) 
					&& ((bardmon->mtame > 0) == (mtmp->mtame > 0)) && bardmon->mpeaceful == mtmp->mpeaceful
					&& mon_can_see_mon(bardmon,mtmp)
				) grow_up(bardmon, mtmp);
			}
		} else {
			max_increase = 0;
			cur_increase = 0;
		}
		//Gynoid pets recover from kills
		if(mtmp->mtyp == PM_GYNOID && mtmp->mhp < mtmp->mhpmax){
			mtmp->mhp += victim->m_lev;
			if(mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
		}
	} else {
	    /* a gain level potion or wraith corpse; always go up a level
	       unless already at maximum (30 is player limt, so assume it is
		   the inate limit of gain level potions) */
	    max_increase = cur_increase = rnd(8);
	    hp_threshold = 0;	/* smaller than `mhpmax + max_increase' */
	    lev_limit = max(30, mtmp->m_lev);
	}

	mtmp->mhpmax += max_increase;
	mtmp->mhp += cur_increase;
	if (mtmp->mhpmax <= hp_threshold)
	    return ptr;		/* doesn't gain a level */
		
	if ((int)++mtmp->m_lev >= mons[newtype].mlevel && newtype != oldtype) {
	    ptr = &mons[newtype];
	    if (mvitals[newtype].mvflags & G_GENOD && !In_quest(&u.uz)) {	/* allow G_EXTINCT */
		if (sensemon(mtmp))
		    pline("As %s grows up into %s, %s %s!", mon_nam(mtmp),
			an(ptr->mname), mhe(mtmp),
			nonliving(ptr) ? "expires" : "dies");
		set_mon_data(mtmp, newtype);	/* keep mvitals[] accurate */
		mondied(mtmp);
		return (struct permonst *)0;
	    }
		set_mon_data(mtmp, newtype);	/* preserve intrinsics */
		if(oldtype == PM_STRANGE_LARVA)
			give_grown_equipment(mtmp);
	    newsym(mtmp->mx, mtmp->my);		/* color may change */
	    lev_limit = (int)mtmp->m_lev;	/* never undo increment */
		if(newtype == PM_METROID_QUEEN && mtmp->mtame){
			struct monst *baby;
			int tnum = d(1,6);
			int i;
			untame(mtmp, 1);
			for(i = 0; i < 6; i++){
				baby = makemon(&mons[PM_METROID], mtmp->mx, mtmp->my, MM_ADJACENTOK);
				if(tnum-->0) tamedog(baby,(struct obj *) 0);
			}
		}
	}
	/* sanity checks */
	if ((int)mtmp->m_lev > lev_limit) {
	    mtmp->m_lev--;	/* undo increment */
	    /* HP might have been allowed to grow when it shouldn't */
	    if (mtmp->mhpmax >= hp_threshold + 1) mtmp->mhpmax--;
	}
//	if( !(monsndx(mtmp)<PM_DJINNI && monsndx(mtmp)>PM_BALROG) ){
		// Some High level stuff is higher than this.
		// if (mtmp->mhpmax > 50*8) mtmp->mhpmax = 50*8;	  /* absolute limit */
		// if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;
//	}
	return ptr;
}

#endif /* OVLB */
#ifdef OVL1

struct obj *
mongets(mtmp, otyp, mkobjflags)
register struct monst *mtmp;
register int otyp;
int mkobjflags;
{
	register struct obj *otmp;
	int spe;

	if ((mtmp->mtemplate == ZOMBIFIED || mtmp->mtemplate == SKELIFIED) && rn2(2))
		return (struct obj *)0;
	if (!otyp)
		return (struct obj *)0;

	otmp = mksobj(otyp, mkobjflags);
	if (otmp) {
		/*Size and shape it to owner*/
		if ((otmp->oclass == ARMOR_CLASS || otmp->oclass == WEAPON_CLASS) && mtmp->mtyp != PM_HOBBIT)
			otmp->objsize = mtmp->data->msize;
		if (otmp->otyp == BULLWHIP && is_drow(mtmp->data) && mtmp->female)
			set_material_gm(otmp, SILVER);
		if (otmp->oclass == ARMOR_CLASS && !Is_dragon_scales(otmp)){
			if (is_suit(otmp) || otmp->otyp == BODYGLOVE) otmp->bodytypeflag = (mtmp->data->mflagsb&MB_BODYTYPEMASK);
			else if (is_helmet(otmp)) otmp->bodytypeflag = (mtmp->data->mflagsb&MB_HEADMODIMASK);
			else if (is_shirt(otmp)) otmp->bodytypeflag = (mtmp->data->mflagsb&MB_HUMANOID) ? MB_HUMANOID : (mtmp->data->mflagsb&MB_BODYTYPEMASK);
		}

		if (mtmp->data->mlet == S_GOLEM){
			if (otmp->oclass == WEAPON_CLASS || otmp->oclass == ARMOR_CLASS){
				if (mtmp->mtyp == PM_GOLD_GOLEM){
					set_material_gm(otmp, GOLD);
				}
				else if (mtmp->mtyp == PM_TREASURY_GOLEM){
					set_material_gm(otmp, GOLD);
				}
				else if (mtmp->mtyp == PM_GLASS_GOLEM){
					set_material_gm(otmp, GLASS);
				}
				else if (mtmp->mtyp == PM_GROVE_GUARDIAN){
					set_material_gm(otmp, SILVER);
				}
				else if (mtmp->mtyp == PM_IRON_GOLEM){
					set_material_gm(otmp, IRON);
				}
				else if (mtmp->mtyp == PM_GREEN_STEEL_GOLEM){
					set_material_gm(otmp, GREEN_STEEL);
				}
				else if (mtmp->mtyp == PM_ARGENTUM_GOLEM){
					set_material_gm(otmp, SILVER);
				}
			}
		}
		if (is_demon(mtmp->data) 
			&& (otmp->oclass == WEAPON_CLASS || otmp->oclass == ARMOR_CLASS)
			&& (otmp->obj_material == COPPER || otmp->obj_material == SILVER)
		){
			set_material_gm(otmp, IRON);
		}
		
		if (is_demon(mtmp->data)) {
			/* demons never get blessed objects */
			if (otmp->blessed) curse(otmp);
			if (mtmp->mtyp == PM_MARILITH && otmp->oclass == WEAPON_CLASS){
				int roll = rnd(3);
				otmp->spe = max(otmp->spe, roll);

				roll = rn2(100);
				if (roll < 25)
					set_material(otmp, IRON);
				else if (roll < 50)
					set_material(otmp, GOLD);
				else if (roll < 75)
					set_material(otmp, MITHRIL);
				else if (roll < 85)
					set_material(otmp, OBSIDIAN_MT);
				else if (roll < 95)
					set_material(otmp, GEMSTONE);
				else if (roll < 100)
					set_material(otmp, SILVER);

				MAYBE_MERC(otmp)

				if (rn2(100) < 15){
					switch (rnd(8)){
					case 1:
						add_oprop(otmp, OPROP_FLAYW);
						break;
					case 2:
						add_oprop(otmp, OPROP_FIREW);
						break;
					case 3:
						add_oprop(otmp, OPROP_COLDW);
						break;
					case 4:
						add_oprop(otmp, OPROP_ELECW);
						break;
					case 5:
						add_oprop(otmp, OPROP_ACIDW);
						break;
					case 6:
						add_oprop(otmp, OPROP_MAGCW);
						break;
					case 7:
						add_oprop(otmp, OPROP_ANARW);
						break;
					case 8:
						add_oprop(otmp, OPROP_UNHYW);
						curse(otmp);
						break;
					}
				}
			}
		}

		if (is_minion(mtmp->data)) {
			/* angelic minions don't get cursed, bad, or rusting objects */
			otmp->cursed = FALSE;
			otmp->blessed = TRUE;
			if (otmp->spe < 0) otmp->spe *= -1;
			otmp->oerodeproof = TRUE;
			if(In_endgame(&u.uz) || (mkobjflags&MKOBJ_GOODEQUIP)){
				if(otmp->oclass == ARMOR_CLASS || otmp->oclass == WEAPON_CLASS || is_weptool(otmp)){
					int min = 3;
					if(Is_astralevel(&u.uz)){
						min = 7;
						if(!rn2(10)){
							if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp))
								add_oprop(otmp, OPROP_HOLYW);
							else {
								add_oprop(otmp, OPROP_HOLY);
								add_oprop(otmp, OPROP_WOOL);
								add_oprop(otmp, OPROP_LIFE);
							}
							if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp)){
								if(rn2(2)){
									add_oprop(otmp, OPROP_ELECW);
								}
								else {
									add_oprop(otmp, OPROP_FIREW);
								}
							}
						}
						else if(!rn2(3)){
							if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp))
								add_oprop(otmp, OPROP_HOLYW);
							else
								add_oprop(otmp, OPROP_HOLY);
						}
					}
					if(otmp->spe < min){
						otmp->spe = min;
					}
				}
			}
		}
		if (is_mplayer(mtmp->data) && is_sword(otmp)) {
			otmp->spe = (3 + rn2(4));
		}
		fix_object(otmp);

		if (otmp->otyp == CANDELABRUM_OF_INVOCATION) {
			otmp->spe = 0;
			otmp->age = 0L;
			otmp->lamplit = FALSE;
			otmp->blessed = otmp->cursed = FALSE;
		}
		else if (otmp->otyp == BELL_OF_OPENING) {
			otmp->blessed = otmp->cursed = FALSE;
		}
		else if (otmp->otyp == SPE_BOOK_OF_THE_DEAD) {
			otmp->blessed = FALSE;
			otmp->cursed = TRUE;
		}

		/* leaders don't tolerate inferior quality battle gear */
		if (is_prince(mtmp->data)) {
			if (otmp->oclass == WEAPON_CLASS && otmp->spe < 1)
				otmp->spe = 1;
			else if (otmp->oclass == ARMOR_CLASS && otmp->spe < 0)
				otmp->spe = 0;
		}

		spe = otmp->spe;
		if (mpickobj(mtmp, otmp)) {
			/* freed otmp, cannot be modified by caller, oops */
			return (struct obj *)0;
		}
		
	}
	return otmp;
}

#endif /* OVL1 */
#ifdef OVLB

int
golemhp(type)
int type;
{
	switch(type) {
		case PM_STRAW_GOLEM: return 20;
		case PM_PAPER_GOLEM: return 20;
		case PM_ROPE_GOLEM: return 30;
		case PM_LEATHER_GOLEM: return 40;
		case PM_GOLD_GOLEM: return 40;
		case PM_WOOD_GOLEM: return 50;
		case PM_LIVING_LECTERN: return 50;
		case PM_GROVE_GUARDIAN: return 60;
		case PM_FLESH_GOLEM: return 40;
		case PM_BRAIN_GOLEM: return 40;
		case PM_DANCING_BLADE: return 49;
		case PM_SPELL_GOLEM: return 20;
//		case PM_SAURON_THE_IMPRISONED: return 45;
		case PM_CLAY_GOLEM: return 50;
		case PM_CHAIN_GOLEM: return 50;
		case PM_TREASURY_GOLEM: return 60;
		case PM_STONE_GOLEM: return 60;
		case PM_SENTINEL_OF_MITHARDIR: return 60;
		case PM_GLASS_GOLEM: return 60;
		case PM_ARGENTUM_GOLEM: return 70;
		case PM_IRON_GOLEM: return 80;
		case PM_SEMBLANCE: return 80;
		case PM_ARSENAL: return 88;
		case PM_GREEN_STEEL_GOLEM: return 99;
		case PM_RETRIEVER: return 120;
		case PM_LIVING_DOLL: return 45+d(5,8);
		case PM_PARASITIZED_DOLL: return 45+d(20,8);
		case PM_FORD_GUARDIAN: return 135;//Max HP
//		case PM_HEAD_OF_THE_UNKNOWN_GOD: return 65;
//		case PM_BODY_OF_THE_UNKNOWN_GOD: return 65;
//		case PM_LEGS_OF_THE_UNKNOWN_GOD: return 65;
//		case PM_EYE_OF_THE_UNKNOWN_GOD: return 65;
//		case PM_SERVANT_OF_THE_UNKNOWN_GOD: return 65;
//		case PM_SCOURGE_OF_THE_UNKNOWN_GOD: return 65;
		default: return 0;
	}
}

#endif /* OVLB */
#ifdef OVL1

/*
 * Returns true if their IS a conflict (which causes peace minded to return false: the monster is not peaceful)
 */

STATIC_OVL
boolean
conflicting_unaligned_alignment(gnum, ptr)
int gnum;
struct permonst *ptr;
{
	switch(gnum){
		case GOD_THE_SILENCE:
			if(ptr->mlet == S_BLOB || ptr->mlet == S_PUDDING)
				return FALSE;
			else return TRUE;
		break;
		case GOD_CHAOS:
			if(ptr->maligntyp < 0)
				return FALSE;
			else return TRUE;
		break;
		case GOD_MOLOCH:
			if(ptr->maligntyp < 0 || is_demon(ptr))
				return FALSE;
			else return TRUE;
		break;
		case GOD_AN_ALIEN_GOD:
			if(ptr->maligntyp == 0)
				return FALSE;
			else return TRUE;
		break;
		case GOD_THE_BLACK_MOTHER:
			if(goat_monster(ptr))
				return FALSE;
			else return TRUE;
		break;
		case GOD_NODENS:
			if(ptr->mtyp == PM_NIGHTGAUNT)
				return FALSE;
			else if(ptr->mtyp == PM_DREAM_QUASIELEMENTAL)
				return FALSE;
			else if(is_aquatic(ptr) && !is_primordial(ptr) && !is_great_old_one(ptr) && !is_alien(ptr))
				return FALSE;
			else return TRUE;
		break;
		case GOD_BAST:
			if(ptr->mlet == S_FELINE && ptr->mtyp != PM_MANTICORE)
				return FALSE;
			else return TRUE;
		break;
		case GOD_THE_DREAD_FRACTURE:
			return TRUE;
		break;
		case GOD_YOG_SOTHOTH:
			if(likes_magic(ptr)) /*Yog is all wizards*/
				return FALSE;
			else return TRUE;
		break;
		case GOD_BOKRUG__THE_WATER_LIZARD:
			if(ptr->mlet == S_LIZARD || is_aquatic(ptr)) /*Note: Beings of Ib are aquatic*/
				return FALSE;
			else return TRUE;
		break;
	}
	// Default to hostile
	return TRUE;
}

/*
 *	Alignment vs. yours determines monster's attitude to you.
 *	( some "animal" types are co-aligned, but also hungry )
 */
boolean
peace_minded(ptr)
register struct permonst *ptr;
{
	int mndx = monsndx(ptr);
	aligntyp mal = ptr->maligntyp, ual = u.ualign.type;
	
	if(Infuture && !in_mklev) return FALSE;
	
	if(Race_if(PM_CLOCKWORK_AUTOMATON) && (mndx == PM_TINKER_GNOME || mndx == PM_HOOLOOVOO) ) return TRUE;
	
	if(mndx == PM_CENTER_OF_ALL && !u.uevent.sum_entered ) return TRUE;
	
	if(goat_monster(ptr) && u.shubbie_atten && !godlist[GOD_THE_BLACK_MOTHER].anger) return TRUE;
	
	//The painting is normally peaceful
	if(In_quest(&u.uz) && Race_if(PM_HALF_DRAGON) && Role_if(PM_NOBLEMAN) && flags.initgend && u.uevent.qcompleted){
		return((boolean)(!!rn2(6 + (u.ualign.record < -5 ? -5 : u.ualign.record))));
	}

	if(Race_if(PM_DROW) && 
		((ual == A_CHAOTIC && (!Role_if(PM_NOBLEMAN) || flags.initgend)) || (ual == A_NEUTRAL && !flags.initgend)) && /*Males can be neutral or chaotic, but a chaotic male nobleman converted to a different god*/
		mndx == PM_AVATAR_OF_LOLTH && 
		urole.cgod != GOD_LOLTH &&
		u.ualign.record >= 20
	) return TRUE;
	
	if(curhouse && 
		(curhouse == u.uhouse || allied_faction(curhouse,u.uhouse))
	) return TRUE;
	
	if(u.uhouse &&
		u.uhouse == EILISTRAEE_SYMBOL
		&& is_elf(ptr) && !is_drow(ptr)
	) return TRUE;
	
	if(u.uhouse &&
		(u.uhouse == XAXOX || u.uhouse == EDDER_SYMBOL)
		&& ptr->mtyp == PM_EDDERKOP
	) return TRUE;
	
	if(u.uhouse &&
		u.uhouse == GHAUNADAUR_SYMBOL
		&& (ptr->mlet == S_PUDDING || ptr->mlet == S_BLOB || ptr->mlet == S_JELLY)
		&& mindless(ptr)
	) return TRUE;
	
	if(u.uhouse &&
		u.uhouse == GHAUNADAUR_SYMBOL
		&& (ptr->mtyp == PM_SHOGGOTH || ptr->mtyp == PM_PRIEST_OF_GHAUNADAUR || ptr->mtyp == PM_PRIESTESS_OF_GHAUNADAUR)
	) return TRUE;
	
	if (ptr->mtyp == urole.ldrnum || ptr->msound == MS_GUARDIAN)
		return TRUE;
	if (ptr->msound == MS_NEMESIS)	return FALSE;
	
	if (always_peaceful(ptr)) return TRUE;
	if(!u.uevent.invoked && mndx==PM_UVUUDAUM && !Infuture) return TRUE;
	
	if(ual == A_VOID) return FALSE;
	
	//Law quest uniques
	if (is_auton(ptr)){
		/* u.uevent.uaxus_foe must be checked elsewhere
		* it will make autons hostile AND penalize alignment as though they had been generated peaceful */
		if (sgn(mal) == sgn(ual) && (u.ualign.record >= 10 || u.uevent.uaxus_foe))
			return TRUE;
		else
			return FALSE;
	}
	if (mndx==PM_APOLLYON && u.ualign.record >= 0 && sgn(mal) == sgn(ual)) return TRUE;
	if (mndx==PM_OONA && u.ualign.record >= 20 && u.ualign.sins < 10 && sgn(mal) == sgn(ual)) return TRUE;
	
	//Always hostility, with exception for vampireness and law quest insects
	if (always_hostile(ptr) && 
		(u.uz.dnum != law_dnum || !(is_social_insect(ptr) || is_mercenary(ptr))
		|| (!on_level(&arcadia1_level,&u.uz) && !on_level(&arcadia2_level,&u.uz) && !on_level(&arcadia3_level,&u.uz))
		) && (!is_vampire(ptr) || !is_vampire(youracedata))
		) return FALSE;

	if(Role_if(PM_VALKYRIE) && (mndx==PM_CROW || mndx==PM_RAVEN)) return TRUE;
	
	if (race_peaceful(ptr)) return TRUE;
	if (race_hostile(ptr)) return FALSE;

	/* the monster is hostile if its alignment is different from the
	 * player's 
	 * Normal: Done by comparing the signs on the alignments.
	 * A_NONE: Done by a special function.
	 */
	if(ual == A_NONE){
		if(conflicting_unaligned_alignment(u.ualign.god, ptr))
			return FALSE;
	}
	else if (sgn(mal) != sgn(ual)) return FALSE;

	/* Negative monster hostile to player with Amulet. */
	if (mal < A_NEUTRAL && u.uhave.amulet) return FALSE;

	/* minions are hostile to players that have strayed at all */
	if (is_minion(ptr)) return((boolean)(u.ualign.record >= 0));

	/* Last case:  a chance of a co-aligned monster being
	 * hostile.  This chance is greater if the player has strayed
	 * (u.ualign.record negative) or the monster is not strongly aligned.
	 */
	return((boolean)(!!rn2(16 + (u.ualign.record < -15 ? -15 : u.ualign.record)) &&
		!!rn2(2 + abs(mal))));
}

/* Set malign to have the proper effect on player alignment if monster is
 * killed.  Negative numbers mean it's bad to kill this monster; positive
 * numbers mean it's good.  Since there are more hostile monsters than
 * peaceful monsters, the penalty for killing a peaceful monster should be
 * greater than the bonus for killing a hostile monster to maintain balance.
 * Rules:
 *   it's bad to kill peaceful monsters, potentially worse to kill always-
 *	peaceful monsters
 *   it's never bad to kill a hostile monster, although it may not be good
 */
void
set_malign(mtmp)
struct monst *mtmp;
{
	schar mal = mtmp->data->maligntyp;
	boolean coaligned;

	if (get_mx(mtmp, MX_EPRI) || get_mx(mtmp, MX_EMIN)) {
		/* some monsters have individual alignments; check them */
		if (get_mx(mtmp, MX_EPRI))
			mal = EPRI(mtmp)->shralign;
		else if (get_mx(mtmp, MX_EMIN))
			mal = EMIN(mtmp)->min_align;
		/* unless alignment is none, set mal to -5,0,5 */
		/* (see align.h for valid aligntyp values)     */
		if(mal != A_NONE)
			mal *= 5;
	}

	coaligned = (sgn(mal) == sgn(u.ualign.type));
	if (mtmp->mtyp == urole.ldrnum) {
		mtmp->malign = -20;
	} else if (mal == A_NONE) {
		if (mtmp->mpeaceful)
			mtmp->malign = 0;
		else
			mtmp->malign = 20;	/* really hostile */
	} else if (always_peaceful(mtmp->data)) {
		int absmal = abs(mal);
		if (mtmp->mpeaceful)
			mtmp->malign = -3*max(5,absmal);
		else
			mtmp->malign = 3*max(5,absmal); /* renegade */
	} else if (always_hostile(mtmp->data)) {
		int absmal = abs(mal);
		if (coaligned)
			mtmp->malign = 0;
		else
			mtmp->malign = max(5,absmal);
	} else if (coaligned) {
		int absmal = abs(mal);
		if (mtmp->mpeaceful)
			mtmp->malign = -3*max(3,absmal);
		else	/* renegade */
			mtmp->malign = max(3,absmal);
	} else {	/* not coaligned and therefore hostile */
		//Neutral murder bugfix: neutral murderables should be worth 1 algnment if hostile.
		if (!mal && murderable_mon(mtmp))
			mtmp->malign = 1;
		else
			mtmp->malign = abs(mal);
	}
}

#endif /* OVL1 */
#ifdef OVLB

static NEARDATA char syms[] = {
	MAXOCLASSES, MAXOCLASSES+1, RING_CLASS, WAND_CLASS, WEAPON_CLASS,
	FOOD_CLASS, COIN_CLASS, SCROLL_CLASS, POTION_CLASS, ARMOR_CLASS,
	AMULET_CLASS, TOOL_CLASS, ROCK_CLASS, GEM_CLASS, SPBOOK_CLASS,
	S_MIMIC_DEF, S_MIMIC_DEF, S_MIMIC_DEF,
};

void
set_mimic_sym(mtmp)		/* KAA, modified by ERS */
register struct monst *mtmp;
{
	int typ, roomno, rt;
	unsigned appear, ap_type;
	int s_sym;
	struct obj *otmp;
	int mx, my;

	if (!mtmp) return;
	
	mx = mtmp->mx; my = mtmp->my;
	typ = levl[mx][my].typ;
					/* only valid for INSIDE of room */
	roomno = levl[mx][my].roomno - ROOMOFFSET;
	if (roomno >= 0)
		rt = rooms[roomno].rtype;
#ifdef SPECIALIZATION
	else if (IS_ROOM(typ))
		rt = OROOM,  roomno = 0;
#endif
	else	rt = 0;	/* roomno < 0 case for GCC_WARN */

	if (mtmp->mtyp == PM_DARK_YOUNG) {
		ap_type = M_AP_FURNITURE;
		appear = S_deadtree;
	} else if (mtmp->mtyp == PM_SHARAB_KAMEREL) {
		ap_type = M_AP_FURNITURE;
		appear = S_puddle;
	} else if (mtmp->mtyp == PM_LIVING_MIRAGE) {
		ap_type = M_AP_FURNITURE;
		appear = S_puddle;
	} else if (OBJ_AT(mx, my)) {
		ap_type = M_AP_OBJECT;
		appear = level.objects[mx][my]->otyp;
	} else if (IS_DOOR(typ) || IS_WALL(typ) ||
		   typ == SDOOR || typ == SCORR) {
		ap_type = M_AP_FURNITURE;
		/*
		 *  If there is a wall to the left that connects to this
		 *  location, then the mimic mimics a horizontal closed door.
		 *  This does not allow doors to be in corners of rooms.
		 */
		if (mx != 0 &&
			(levl[mx-1][my].typ == HWALL    ||
			 levl[mx-1][my].typ == TLCORNER ||
			 levl[mx-1][my].typ == TRWALL   ||
			 levl[mx-1][my].typ == BLCORNER ||
			 levl[mx-1][my].typ == TDWALL   ||
			 levl[mx-1][my].typ == CROSSWALL||
			 levl[mx-1][my].typ == TUWALL    ))
		    appear = S_hcdoor;
		else
		    appear = S_vcdoor;

		if(!mtmp->minvis || See_invisible(mtmp->mx,mtmp->my))
		    block_point(mx,my);	/* vision */
	} else if (level.flags.is_maze_lev && rn2(2)) {
		ap_type = M_AP_OBJECT;
		appear = STATUE;
	} else if (roomno < 0) {
		ap_type = M_AP_OBJECT;
		appear = BOULDER;
		if(!mtmp->minvis || See_invisible(mtmp->mx,mtmp->my))
		    block_point(mx,my);	/* vision */
	} else if (rt == ZOO || rt == VAULT) {
		ap_type = M_AP_OBJECT;
		appear = GOLD_PIECE;
	} else if (rt == DELPHI) {
		if (rn2(2)) {
			ap_type = M_AP_OBJECT;
			appear = STATUE;
		} else {
			ap_type = M_AP_FURNITURE;
			appear = S_fountain;
		}
	} else if (rt == TEMPLE) {
		ap_type = M_AP_FURNITURE;
		appear = S_altar;
	/*
	 * We won't bother with beehives, morgues, barracks, throne rooms
	 * since they shouldn't contain too many mimics anyway...
	 */
	} else if (rt >= SHOPBASE) {
		s_sym = get_shop_item(rt - SHOPBASE);
		if (s_sym < 0) {
			ap_type = M_AP_OBJECT;
			appear = -s_sym;
		} else {
			if (s_sym == RANDOM_CLASS)
				s_sym = syms[rn2((int)sizeof(syms)-2) + 2];
			goto assign_sym;
		}
	} else {
		s_sym = syms[rn2((int)sizeof(syms))];
assign_sym:
		if (s_sym >= MAXOCLASSES) {
			ap_type = M_AP_FURNITURE;
			appear = s_sym == MAXOCLASSES ? S_upstair : S_dnstair;
		} else if (s_sym == COIN_CLASS) {
			ap_type = M_AP_OBJECT;
			appear = GOLD_PIECE;
		} else {
			ap_type = M_AP_OBJECT;
			if (s_sym == S_MIMIC_DEF) {
				appear = STRANGE_OBJECT;
			} else {
				otmp = mkobj( (char) s_sym, FALSE );
				appear = otmp->otyp;
				/* make sure container contents are free'ed */
				obfree(otmp, (struct obj *) 0);
			}
		}
	}
	mtmp->m_ap_type = ap_type;
	mtmp->mappearance = appear;
}

/* release a monster from a bag of tricks */
int
bagotricks(bag, tipping, seencount)
struct obj *bag;
boolean tipping; /* caller emptying entire contents; affects shop handling */
int *seencount;  /* secondary output */
{
	int moncount = 0;

	if (!bag || bag->otyp != BAG_OF_TRICKS) {
		impossible("bad bag o' tricks");
	} else if (bag->spe < 1) {
		/* if tipping known empty bag, give normal empty container message */
		pline1(tipping ? "It's empty." : nothing_happens);
		/* now known to be empty if sufficiently discovered */
	} else {
		struct monst *mtmp;
		int creatcnt = 1, seecount = 0;

		consume_obj_charge(bag, TRUE);

		if (!rn2(23))
			creatcnt += rnd(7);
		do {
			mtmp = makemon((struct permonst *) 0, u.ux, u.uy, NO_MM_FLAGS);
			if (mtmp) {
				++moncount;
				if (canspotmon(mtmp))
					++seecount;
			}
		} while (--creatcnt > 0);
		if (seecount) {
			if (seencount)
				*seencount += seecount;
			if (bag->dknown)
				makeknown(BAG_OF_TRICKS);
		} else if (!tipping) {
			pline1(!moncount ? nothing_happens : "Nothing seems to happen.");
		}
	}
	return moncount;
}

long
init_doll_sales()
{
	int i, j;
	long tmp, dollTypes = 0;
	long dolltypes[] = {
		DOLLMAKER_EFFIGY,
		DOLLMAKER_JUMPING,
		DOLLMAKER_FRIENDSHIP,
		DOLLMAKER_CHASTITY,
		DOLLMAKER_CLEAVING,
		DOLLMAKER_SATIATION,
		DOLLMAKER_HEALTH,
		DOLLMAKER_HEALING,
		DOLLMAKER_DESTRUCTION,
		DOLLMAKER_MEMORY,
		DOLLMAKER_BINDING,
		DOLLMAKER_PRESERVATION,
		DOLLMAKER_QUICK_DRAW,
		DOLLMAKER_WAND_CHARGE,
		DOLLMAKER_STEALING,
		DOLLMAKER_MOLLIFICATION,
		DOLLMAKER_CLEAR_THOUGHT,
		DOLLMAKER_MIND_BLASTS
	};
	for(i = 0; i < SIZE(dolltypes); i++){
		j = rn2(SIZE(dolltypes));
		tmp = dolltypes[i];
		dolltypes[i] = dolltypes[j];
		dolltypes[j] = tmp;
	}
	for(i = rn1(3, SIZE(dolltypes)/2); i > 0; i--)
		dollTypes |= dolltypes[i];
	return dollTypes;
}

STATIC_OVL
int
permonst_max_lev(ptr)
struct permonst *ptr;
{
	int lev_limit = 3 * (int)ptr->mlevel / 2;

	if(ptr->mtyp == PM_SECRET_WHISPERER || ptr->mtyp == PM_TRUTH_SEER 
	|| ptr->mtyp == PM_DREAM_EATER || ptr->mtyp == PM_VEIL_RENDER
	)
		lev_limit = min(45, u.uinsight);

	if (is_mplayer(ptr) || ptr->mtyp == PM_BYAKHEE || ptr->mtyp == PM_LILLEND || ptr->mtyp == PM_ERINYS || ptr->mtyp == PM_MAID
	|| ptr->mtyp == PM_CROW_WINGED_HALF_DRAGON || ptr->mtyp == PM_BASTARD_OF_THE_BOREAL_VALLEY
	|| ptr->mtyp == PM_UNDEAD_KNIGHT || ptr->mtyp == PM_WARRIOR_OF_SUNLIGHT
	|| ptr->mtyp == PM_UNDEAD_MAIDEN || ptr->mtyp == PM_KNIGHT_OF_THE_PRINCESS_S_GUARD
	|| ptr->mtyp == PM_BLUE_SENTINEL || ptr->mtyp == PM_DARKMOON_KNIGHT
	|| ptr->mtyp == PM_UNDEAD_REBEL || ptr->mtyp == PM_PARDONER || ptr->mtyp == PM_OCCULTIST
	|| ptr->mtyp == PM_FORMIAN_CRUSHER
	|| ptr->mtyp == PM_DRIDER || ptr->mtyp == PM_SPROW
	|| ptr->mtyp == PM_DROW_MATRON || ptr->mtyp == PM_DROW_MATRON_MOTHER
	|| ptr->mtyp == PM_HEDROW_BLADEMASTER || ptr->mtyp == PM_DROW_ALIENIST
	|| ptr->mtyp == PM_ELVENKING || ptr->mtyp == PM_ELVENQUEEN
	|| ptr->mtyp == PM_DWARF_KING || ptr->mtyp == PM_DWARF_QUEEN
	|| ptr->mtyp == PM_CUPRILACH_RILMANI || ptr->mtyp == PM_STANNUMACH_RILMANI
	|| ptr->mtyp == PM_ARGENACH_RILMANI || ptr->mtyp == PM_AURUMACH_RILMANI
	|| ptr->mtyp == PM_ANDROID || ptr->mtyp == PM_GYNOID || ptr->mtyp == PM_OPERATOR || ptr->mtyp == PM_COMMANDER
	) lev_limit = 30;	/* same as player */
	else if (ptr->mtyp == PM_PLUMACH_RILMANI || ptr->mtyp == PM_FERRUMACH_RILMANI) lev_limit = 20;
	else if (is_eladrin(ptr) && ptr->mlevel <= 20) lev_limit = 30;
	else if (ptr->mtyp == PM_OONA) lev_limit = 60;
	else if (is_ancient(ptr)) lev_limit = 45;
	else if (lev_limit < 5) lev_limit = 5;	/* arbitrary */
	else if (lev_limit > 49) lev_limit = (ptr->mlevel > 49 ? ptr->mlevel : 49);
	
	return lev_limit;
}

int
mon_max_lev(mon)
struct monst *mon;
{
	int lev_limit = permonst_max_lev(mon->data);

	if (mon->ispolyp) lev_limit = max(lev_limit, 30);
	
	if(Role_if(PM_HEALER) && mon->mtame && lev_limit < 49)
		lev_limit = min_ints(49, lev_limit + heal_mlevel_bonus());

	return lev_limit;
}
#endif /* OVLB */

/*makemon.c*/
