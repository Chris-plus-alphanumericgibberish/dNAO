/*	SCCS Id: @(#)mkroom.c	3.4	2001/09/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Entry points:
 *	mkroom() -- make and stock a room of a given type
 *	nexttodoor() -- return TRUE if adjacent to a door
 *	has_dnstairs() -- return TRUE if given room has a down staircase
 *	has_upstairs() -- return TRUE if given room has an up staircase
 *	courtmon() -- generate a court monster
 *	save_rooms() -- save rooms into file fd
 *	rest_rooms() -- restore rooms from file fd
 */

 //Note: math.h must be included before hack.h bc it contains a yn() macro that is incompatible with the one in hack
#include <math.h>

#include "hack.h"

extern const int monstr[];

#ifdef OVLB
STATIC_DCL boolean FDECL(isbig, (struct mkroom *));
STATIC_DCL int FDECL(int_sqrt, (int));
STATIC_DCL boolean FDECL(issemispacious, (struct mkroom *));
STATIC_DCL void NDECL(mkshop), FDECL(mkzoo,(int)), NDECL(mkswamp);
STATIC_DCL struct monst *FDECL(prisoner,(int, int, int));
STATIC_DCL void NDECL(mktemple);
STATIC_DCL void NDECL(mkkamereltowers);
STATIC_DCL void NDECL(mkminorspire);
STATIC_DCL void NDECL(mkfishingvillage);
STATIC_DCL void NDECL(mkpluhomestead);
STATIC_DCL void FDECL(mkpluroom, (int));
STATIC_DCL void FDECL(mkelfhut, (int));
STATIC_DCL void FDECL(mkwraithclearing, (int));
STATIC_DCL void NDECL(mkstonepillars);
STATIC_DCL void FDECL(mkmordorfossil, (int));
STATIC_DCL void NDECL(mklavapool);
STATIC_DCL void FDECL(mkwell, (int));
STATIC_DCL void FDECL(mkfishinghut, (int));
STATIC_DCL void NDECL(mkpluvillage);
STATIC_DCL void NDECL(mkferrufort);
STATIC_DCL void NDECL(mkferrutower);
STATIC_DCL void NDECL(mkferrubarracks);
STATIC_DCL void NDECL(mkinvertzigg);
STATIC_DCL void FDECL(mkmch, (int));
STATIC_DCL void FDECL(mkwrk, (int));
STATIC_DCL void FDECL(mkaph, (int));
STATIC_DCL void FDECL(mklostitem, (int));
STATIC_DCL void FDECL(mklawfossil, (int));
STATIC_DCL void FDECL(mkcamp, (int));
STATIC_DCL void NDECL(mklolthsepulcher);
STATIC_DCL void NDECL(mkmivaultlolth);
STATIC_DCL void NDECL(mkvaultlolth);
STATIC_DCL void NDECL(mklolthgnoll);
STATIC_DCL void NDECL(mklolthgarden);
STATIC_DCL void NDECL(mklolthtroll);
STATIC_DCL void NDECL(mklolthtemple);
STATIC_DCL void NDECL(mklolthcell);
STATIC_DCL void NDECL(mklolthdown);
STATIC_DCL void NDECL(mklolthup);
STATIC_DCL void NDECL(mksgarden);
STATIC_DCL void FDECL(mkgarden, (struct mkroom *));
STATIC_DCL void FDECL(mklibrary, (struct mkroom *));
STATIC_DCL void FDECL(mkarmory, (struct mkroom *));
STATIC_DCL void NDECL(mkisland);
STATIC_DCL void NDECL(mkriver);
STATIC_DCL void NDECL(mkneuriver);
STATIC_DCL void NDECL(mkforest12river);
STATIC_DCL void FDECL(liquify, (int,int,int));
STATIC_DCL void FDECL(neuliquify, (int, int, int));
STATIC_DCL void FDECL(chaliquify, (int, int, int));
STATIC_DCL struct permonst * NDECL(morguemon);
STATIC_DCL struct permonst * NDECL(antholemon);
STATIC_DCL struct permonst * FDECL(squadmon, (struct d_level *));
STATIC_DCL void FDECL(save_room, (int,struct mkroom *));
STATIC_DCL void FDECL(rest_room, (int,struct mkroom *));
#endif /* OVLB */

#define sq(x) ((x)*(x))

extern const struct shclass shtypes[];	/* defined in shknam.c */

#ifdef OVLB

STATIC_OVL boolean
isbig(sroom)
register struct mkroom *sroom;
{
	register int area = (sroom->hx - sroom->lx + 1)
			   * (sroom->hy - sroom->ly + 1);
	return((boolean)( area > 20 ));
}

STATIC_OVL int
int_sqrt(num)
int num;
{
	return((boolean)( sqrt(num) ));
}

/* Returns true if room has both an X and Y size of at least x. */
STATIC_OVL boolean
atleastxspace(sroom, x)
register struct mkroom *sroom;
int x;
{
	return((boolean)( ((sroom->hx - sroom-> lx)+1) >= x &&
		(((sroom->hy - sroom->ly)+1) >=x ) ));
}

/* Returns true if room has both an X and Y size of at least five. */
boolean
isspacious(sroom)
register struct mkroom *sroom;
{
	return atleastxspace(sroom, 5);
}

/* Returns true if room has both an X and Y size of at least four. */
STATIC_OVL boolean
issemispacious(sroom)
register struct mkroom *sroom;
{
	return atleastxspace(sroom, 4);
}

void
mksepulcher()
{
	int x,y,tries=0;
	int i,j;
	boolean good=FALSE,okspot;
	while(!good && tries < 50){
		x = rn2(COLNO-7)+3;
		y = rn2(ROWNO-7)+3;
		tries++;
		okspot = TRUE;
		for(i=-1;i<2;i++) for(j=-1;j<2;j++) if(!isok(x+i,y+j) || levl[x+i][y+j].typ != STONE) okspot = FALSE;
		for(i=-2;i<3;i++) for(j=-2;j<3;j++) if(!isok(x+i,y+j) || levl[x+i][y+j].typ >= CORR) okspot = FALSE;
		if(okspot){
			good = TRUE;
			//Blank the inner area, to be safe
			for(i=-1;i<2;i++) for(j=-1;j<2;j++) levl[x+i][y+j].typ = STONE;
			//Make outer area walled
			levl[x+2][y+2].typ = BRCORNER;
			levl[x-2][y+2].typ = BLCORNER;
			for(i=-1;i<2;i++) levl[x+i][y+2].typ = HWALL;
			for(i=-1;i<2;i++) levl[x+i][y-2].typ = HWALL;
			for(i=-1;i<2;i++) levl[x+2][y+i].typ = VWALL;
			for(i=-1;i<2;i++) levl[x-2][y+i].typ = VWALL;
			levl[x+2][y-2].typ = TRCORNER;
			levl[x-2][y-2].typ = TLCORNER;
			//Make the inner area walled also
			levl[x+1][y+1].typ = BRCORNER;
			levl[x-1][y+1].typ = BLCORNER;
			levl[x+0][y+1].typ = HWALL;
			levl[x+0][y-1].typ = HWALL;
			levl[x+1][y+0].typ = VWALL;
			levl[x-1][y+0].typ = VWALL;
			levl[x+1][y-1].typ = TRCORNER;
			levl[x-1][y-1].typ = TLCORNER;
			//Make the center spot empty
			levl[x][y].typ = CORR;
			for(i=-2;i<3;i++) {
				for(j=-2;j<3;j++) {
					levl[x+i][y+j].wall_info |= W_NONDIGGABLE;
				}
			}
			for(i=-1;i<2;i++) {
				for(j=-1;j<2;j++) {
					levl[x+i][y+j].lit = 0;
					levl[x+i][y+j].wall_info |= W_NONDIGGABLE;
					levl[x+i][y+j].wall_info |= W_NONPASSWALL;
				}
			}
			makemon(&mons[PM_DREAD_SERAPH], x, y, 0);
		}
	}
}

STATIC_OVL
void
mklolthsepulcher()
{
	int x,y,tries=0;
	int i,j;
	boolean good=FALSE,okspot;
	while(!good && tries < 50){
		x = rn2(COLNO-7)+3;
		y = rn2(ROWNO-7)+3;
		tries++;
		okspot = TRUE;
		for(i=-2;i<3;i++) for(j=-2;j<3;j++) if(!isok(x+i,y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == CLOUD) || m_at(x+i, y+j)) okspot = FALSE;
		if(okspot){
			good = TRUE;
			for(i=-1;i<2;i++) for(j=-1;j<2;j++) levl[x+i][y+j].typ = STONE;
			levl[x+2][y+2].typ = BRCORNER;
			levl[x-2][y+2].typ = BLCORNER;
			for(i=-1;i<2;i++) levl[x+i][y+2].typ = HWALL;
			for(i=-1;i<2;i++) levl[x+i][y-2].typ = HWALL;
			for(i=-1;i<2;i++) levl[x+2][y+i].typ = VWALL;
			for(i=-1;i<2;i++) levl[x-2][y+i].typ = VWALL;
			levl[x+2][y-2].typ = TRCORNER;
			levl[x-2][y-2].typ = TLCORNER;
			
			levl[x+1][y+1].typ = BRCORNER;
			levl[x-1][y+1].typ = BLCORNER;
			levl[x+0][y+1].typ = HWALL;
			levl[x+0][y-1].typ = HWALL;
			levl[x+1][y+0].typ = VWALL;
			levl[x-1][y+0].typ = VWALL;
			levl[x+1][y-1].typ = TRCORNER;
			levl[x-1][y-1].typ = TLCORNER;
			
			levl[x][y].typ = CORR;
			for(i=-2;i<3;i++) {
				for(j=-2;j<3;j++) {
					levl[x+i][y+j].lit = 0;
					levl[x+i][y+j].wall_info |= W_NONDIGGABLE;
				}
			}
			makemon(&mons[PM_DREAD_SERAPH], x, y, 0);
		}
	}
}

void
mkmivault()
{
	int x,y,tries=0;
	int i,j;
	struct obj *otmp;
	struct obj *otmp2;
	struct monst *mon;
	boolean good=FALSE,okspot;
	struct permonst *ptr;
	while(!good && tries < 50){
		x = rn2(COLNO-6)+1;
		y = rn2(ROWNO-6)+1;
		tries++;
		okspot = TRUE;
		for(i=0;i<6;i++) for(j=0;j<6;j++) if(!isok(x+i,y+j) || levl[x+i][y+j].typ != STONE) okspot = FALSE;
		if(okspot){
			good = TRUE;
			for(i=0;i<6;i++) for(j=0;j<6;j++) levl[x+i][y+j].typ = MOAT;
			for(i=0;i<6;i++){
				levl[x][y+i].edge = 1;
				levl[x+i][y].edge = 1;
				levl[x+5][y+i].edge = 1;
				levl[x+i][y+5].edge = 1;
			}
			for(i=2;i<4;i++) for(j=2;j<4;j++) levl[x+i][y+j].typ = ROOM;
			levl[x+4][y+4].typ = BRCORNER;
			levl[x+1][y+4].typ = BLCORNER;
			for(i=2;i<4;i++) levl[x+i][y+4].typ = HWALL;
			for(i=2;i<4;i++) levl[x+i][y+1].typ = HWALL;
			for(i=2;i<4;i++) levl[x+4][y+i].typ = VWALL;
			for(i=2;i<4;i++) levl[x+1][y+i].typ = VWALL;
			levl[x+4][y+1].typ = TRCORNER;
			levl[x+1][y+1].typ = TLCORNER;
			
			
			otmp = mksobj_at(CHEST, x+2, y+2, NO_MKOBJ_FLAGS);
			for(i = d(2,4);i>0;i--) mkmivaultitem(otmp);
			otmp = mksobj_at(CHEST, x+2, y+3, NO_MKOBJ_FLAGS);
			for(i = d(2,4);i>0;i--) mkmivaultitem(otmp);
			otmp = mksobj_at(CHEST, x+3, y+2, NO_MKOBJ_FLAGS);
			for(i = d(2,4);i>0;i--) mkmivaultitem(otmp);
			otmp = mksobj_at(CHEST, x+3, y+3, NO_MKOBJ_FLAGS);
			for(i = d(2,4);i>0;i--) mkmivaultitem(otmp);
			

			for (i = 0; i < 10; i++){
				ptr = mivaultmon();
				mon = makemon(ptr, x+rnd(2)+1, y+rnd(2)+1, 0);
				if (mon) break;
			}

			// lovecraft monsters
			if (ptr->mtyp == PM_SHOGGOTH || ptr->mtyp == PM_NIGHTGAUNT
					|| ptr->mtyp == PM_DARK_YOUNG || ptr->mtyp == PM_HUNTING_HORROR || ptr->mtyp == PM_HUNTING_HORROR_TAIL){
				otmp2 = mksobj(FIGURINE, NO_MKOBJ_FLAGS);
				switch(rn2(4)){
					case 0:					
						otmp2->corpsenm = PM_DARK_YOUNG;
					break;
					case 1:
						otmp2->corpsenm = PM_SHOGGOTH;
						if(rn2(5)){
							otmp2->corpsenm = PM_PRIEST_OF_GHAUNADAUR;
						}
					break;
					case 2:
						otmp2->corpsenm = PM_DARKNESS_GIVEN_HUNGER;
					break;
					case 3:
						otmp2->corpsenm = PM_MASTER_MIND_FLAYER;
					break;
					case 4:
						otmp2->corpsenm = PM_DEEPEST_ONE;
					break;
				}
				bless(otmp2);
				add_to_container(otmp, otmp2);
			}
			// clockworks
			else if (ptr->mtyp == PM_JUGGERNAUT || ptr->mtyp == PM_ID_JUGGERNAUT 
					|| ptr->mtyp == PM_SCRAP_TITAN || ptr->mtyp == PM_HELLFIRE_COLOSSUS){
				if (rn2(2)){
					add_to_container(otmp, mksobj(ptr->mtyp == PM_ID_JUGGERNAUT ? SUBETHAIC_COMPONENT : HELLFIRE_COMPONENT, 0));
				} else {
					add_to_container(otmp, mksobj(SCRAP, 0));
				}
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
			}
			// spellcasters
			else if (ptr->mtyp == PM_TITAN || ptr->mtyp == PM_EYE_OF_DOOM 
					|| ptr->mtyp == PM_PRIEST_OF_GHAUNADAUR || ptr->mtyp == PM_PRIESTESS_OF_GHAUNADAUR
						|| ptr->mtyp == PM_SERPENT_MAN_OF_YOTH){
				add_to_container(otmp, mkobj(SPBOOK_CLASS, FALSE));
				add_to_container(otmp, mkobj(SPBOOK_CLASS, FALSE));
				if (rn2(2)) add_to_container(otmp, mkobj(SPBOOK_CLASS, FALSE));
				add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
				add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
				add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
				if (rn2(2)) add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
				if (rn2(2)) add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
				if (rn2(2)) add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
			}
			if (mon) {
			    mon->mstrategy |= STRAT_WAITFORU;
			    mon->mpeaceful = FALSE;
			    set_malign(mon);
			}
		}
	}
}

int misc_hell_vault[] = {
				VITAL_SOULSTONE, SPIRITUAL_SOULSTONE,
				VITAL_SOULSTONE, SPIRITUAL_SOULSTONE,
				ANTIMAGIC_RIFT, CATAPSI_VORTEX,
				MISOTHEISTIC_PYRAMID, DIMENSIONAL_LOCK
			};
int misc_devil_vault[] = {
				VITAL_SOULSTONE, SPIRITUAL_SOULSTONE,
				VITAL_SOULSTONE, SPIRITUAL_SOULSTONE,
				ANTIMAGIC_RIFT, CATAPSI_VORTEX,
				MISOTHEISTIC_PYRAMID, DIMENSIONAL_LOCK, 
				PRESERVATIVE_ENGINE
			};
int misc_demon_vault[] = {
				VITAL_SOULSTONE, SPIRITUAL_SOULSTONE,
				VITAL_SOULSTONE, SPIRITUAL_SOULSTONE,
				ANTIMAGIC_RIFT, CATAPSI_VORTEX,
				MISOTHEISTIC_PYRAMID, DIMENSIONAL_LOCK, 
				ARMOR_SALVE
			};
int misc_tan_vault[] = {
				ANTIMAGIC_RIFT, CATAPSI_VORTEX,
				MISOTHEISTIC_PYRAMID, DIMENSIONAL_LOCK,
				ARMOR_SALVE
			};
int misc_anc_vault[] = {
				ANTIMAGIC_RIFT, CATAPSI_VORTEX,
				MISOTHEISTIC_PYRAMID, DIMENSIONAL_LOCK
			};

int
get_vault_misc(vn)
int vn;
{
	int type;
	if(vn < LIMIT_VN_RANGE_1_TANNINIM){
		// Tannin
		type = ROLL_FROM(misc_tan_vault);
	}
	else if(vn < LIMIT_VN_RANGE_2_ANCIENT){
		// Ancient
		type = ROLL_FROM(misc_anc_vault);
	}
	else if(vn < LIMIT_VN_RANGE_3_ANGEL){
		// Angel
		type = ROLL_FROM(misc_hell_vault);
	}
	else if(vn < LIMIT_VN_RANGE_4_DEVIL){
		// Devil
		type = ROLL_FROM(misc_devil_vault);
	}
	else if(vn < LIMIT_VN_RANGE_5_DEMON){
		// Demon
		type = ROLL_FROM(misc_demon_vault);
	}
	else {
		impossible("Vault number %d out-of-ramge in get_misc", vn);
		type = ROLL_FROM(misc_hell_vault);
	}
	return type;
}

void
mkmivaultitem(container)
    struct obj *container;
{
	struct obj *otmp;
	int try_limit = 100;
	otmp = (struct obj *)0;
	if(!rn2(4)){
		do {
		if (otmp) delobj(otmp);
		otmp = mkobj(ARMOR_CLASS, FALSE);
		} while (--try_limit > 0 &&
		  !(objects[otmp->otyp].oc_magic || otmp->oartifact));
		add_to_container(container, otmp);
	} else {
		do {
			if(otmp && !Is_container(otmp)) delobj(otmp);
			otmp = mkobj(RANDOM_CLASS, TRUE);
			if(Is_container(otmp)){
				place_object(otmp, container->ox, container->oy);
			}
		} while (--try_limit > 0 &&
		  !(objects[otmp->otyp].oc_magic || otmp->oartifact));
		if(!Is_container(otmp)) add_to_container(container, otmp);
	}
}

struct obj *
mklolthvaultitem()
{
	struct obj *otmp;
	int try_limit = 1000;
	int type;
	boolean sobj = FALSE;

	const int lolth_armor[] = {HELMET, ARCHAIC_HELM, HIGH_ELVEN_HELM, DROVEN_HELM, CRYSTAL_HELM, HELM_OF_BRILLIANCE, 
							   PLATE_MAIL, ARCHAIC_PLATE_MAIL, HIGH_ELVEN_PLATE, DROVEN_PLATE_MAIL, CRYSTAL_PLATE_MAIL,
							   ELVEN_TOGA, NOBLE_S_DRESS,
							   PLAIN_DRESS,
							   ELVEN_CLOAK, DROVEN_CLOAK,
							   ELVEN_SHIELD, CRYSTAL_SHIELD, SHIELD_OF_REFLECTION,
							   GAUNTLETS, ARCHAIC_GAUNTLETS, HIGH_ELVEN_GAUNTLETS, CRYSTAL_GAUNTLETS, GAUNTLETS_OF_POWER,
							   ARMORED_BOOTS, ARCHAIC_BOOTS, HIGH_BOOTS, HEELED_BOOTS, CRYSTAL_BOOTS, 
							   FLYING_BOOTS, ELVEN_BOOTS, KICKING_BOOTS
							  };
	const int lolth_weapons[] = {
								DROVEN_BOLT, CHAKRAM,
								ELVEN_SPEAR, DROVEN_SPEAR, 
								ELVEN_DAGGER, DROVEN_DAGGER, STILETTO, 
								ELVEN_SICKLE,
								MOON_AXE,
								ELVEN_SHORT_SWORD, DROVEN_SHORT_SWORD,
								SCIMITAR, HIGH_ELVEN_WARSWORD, RAPIER, SABER,
								ELVEN_BROADSWORD, CRYSTAL_SWORD,
								DROVEN_GREATSWORD, 
								ELVEN_LANCE, DROVEN_LANCE,
								DROVEN_CROSSBOW,
								KHAKKHARA
								};
	otmp = (struct obj *)0;
	if(!rn2(3)) {
		type = lolth_armor[rn2(SIZE(lolth_armor))];
		sobj = TRUE;
	}
	else if(rn2(2)) {
		type = lolth_weapons[rn2(SIZE(lolth_weapons))];
		sobj = TRUE;
	}
	else if(rn2(2))
		type = SCOIN_CLASS;
	else if(rn2(2)){
		type = misc_hell_vault[rn2(SIZE(misc_hell_vault))];
		otmp = mksobj(type, NO_MKOBJ_FLAGS);
		return otmp;
	}
	else
		type = RANDOM_CLASS;
	do {
		if(otmp) delobj(otmp);
		otmp = sobj ? mksobj(type, MKOBJ_ARTIF) : mkobj(type, TRUE);
		if(!rn2(10) || ((objects[otmp->otyp].oc_magic || otmp->oartifact) && !rn2(3))){
			otmp = mk_special(otmp);
			if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp) || otmp->oclass == ARMOR_CLASS)
				otmp->spe = max_ints(d(3,3), otmp->spe);
		}
		else if(!rn2(4) || ((objects[otmp->otyp].oc_magic || otmp->oartifact) && !rn2(2))){
			otmp = mk_lolth_vault_special(otmp);
			if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp) || otmp->oclass == ARMOR_CLASS)
				otmp->spe = max_ints(d(3,3), otmp->spe);
		}
		else if(!rn2(4) || otmp->oartifact){
			otmp = mk_minor_special(otmp);
			if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp) || otmp->oclass == ARMOR_CLASS)
				otmp->spe = max_ints(d(1,7), otmp->spe);
		}
		else {
			if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp) || otmp->oclass == ARMOR_CLASS)
				otmp->spe = max_ints(d(2,3), otmp->spe);
		}
	} while (--try_limit > 0 &&
	  !(objects[otmp->otyp].oc_magic || otmp->oartifact || !check_oprop(otmp, OPROP_NONE) || Is_container(otmp)));

	return otmp;
}

struct obj *
mk_jrt_obj(oclass)
int oclass;
{
	switch(oclass){
		case WEAPON_CLASS:{
			const int jrt_weapons[] = {
						ARROW, ELVEN_ARROW, SILVER_ARROW, GOLDEN_ARROW, CHAKRAM,
						SPEAR, ELVEN_SPEAR, JAVELIN, TRIDENT,
						DAGGER, ELVEN_DAGGER, STILETTO, 
						SICKLE, ELVEN_SICKLE, 
						AXE, BATTLE_AXE, MOON_AXE,
						SHORT_SWORD, ELVEN_SHORT_SWORD, KHOPESH, KHOPESH,
						MACE, ELVEN_MACE, CLUB,
						BOW, ELVEN_BOW, SLING, ATLATL
						};
			return mksobj(ROLL_FROM(jrt_weapons), MKOBJ_ARTIF);
		}break;
		case ARMOR_CLASS:{
			const int jrt_armor[] = {
						HELMET, ARCHAIC_HELM, HELM_OF_BRILLIANCE, 
						ARCHAIC_PLATE_MAIL, SCALE_MAIL,
						ELVEN_TOGA, WAISTCLOTH,
						CLOAK, ELVEN_CLOAK, LEO_NEMAEUS_HIDE,
						BUCKLER, ELVEN_SHIELD, ROUNDSHIELD, TOWER_SHIELD, SHIELD_OF_REFLECTION,
						ARCHAIC_GAUNTLETS, GAUNTLETS_OF_DEXTERITY, GAUNTLETS_OF_POWER,
						ARCHAIC_BOOTS, HIGH_BOOTS, 
						FLYING_BOOTS, ELVEN_BOOTS, KICKING_BOOTS
						};
			return mksobj(ROLL_FROM(jrt_armor), MKOBJ_ARTIF);
		}break;
	}
	//default
	return mkobj(oclass, TRUE);
}

struct obj *
mk_v_obj(oclass, vn)
int oclass;
int vn;
{
	if(vn == VN_JRT){
		return mk_jrt_obj(oclass);
	}
	else{
		return mkobj(oclass, TRUE);
	}
}

struct obj *
mkhellvaultitem(vn)
int vn;
{
	struct obj *otmp;
	int try_limit = 1000;
	int type;
	otmp = (struct obj *)0;
	if(!rn2(3))
		type = ARMOR_CLASS;
	else if(rn2(2))
		type = WEAPON_CLASS;
	else if(!rn2(3))
		type = SCOIN_CLASS;
	else if(rn2(2)){
		type = get_vault_misc(vn);
		otmp = mksobj(type, NO_MKOBJ_FLAGS);
		if(otmp)
			return otmp;
		impossible("special %d obj failed??", type);
		type = RANDOM_CLASS;
	}
	else
		type = RANDOM_CLASS;
	do {
		if(otmp) delobj(otmp);
		otmp = mk_v_obj(type, vn);
		if(!rn2(10) || ((objects[otmp->otyp].oc_magic || otmp->oartifact) && !rn2(3))){
			otmp = mk_vault_special(otmp, vn);
			if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp) || otmp->oclass == ARMOR_CLASS)
				otmp->spe = max_ints(d(3,3), otmp->spe);
		}
		else if(!rn2(4) || ((objects[otmp->otyp].oc_magic || otmp->oartifact) && !rn2(2))){
			otmp = mk_vault_special(otmp, vn);
			if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp) || otmp->oclass == ARMOR_CLASS)
				otmp->spe = max_ints(d(3,3), otmp->spe);
		}
		else if(!rn2(4) || otmp->oartifact){
			otmp = mk_minor_special(otmp);
			if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp) || otmp->oclass == ARMOR_CLASS)
				otmp->spe = max_ints(d(1,7), otmp->spe);
		}
		else {
			if(otmp->oclass == WEAPON_CLASS || is_weptool(otmp) || otmp->oclass == ARMOR_CLASS)
				otmp->spe = max_ints(d(2,3), otmp->spe);
		}
	} while (--try_limit > 0 &&
	  !(objects[otmp->otyp].oc_magic || otmp->oartifact || !check_oprop(otmp, OPROP_NONE) || Is_container(otmp)));

	return otmp;
}

void
mkhellvaultitem_cnt(container, vn, bury)
struct obj *container;
int vn;
boolean bury;
{
	struct obj *otmp;
	
	otmp = mkhellvaultitem(vn);
	if(Is_container(otmp)){
		place_object(otmp, container->ox, container->oy);
		if(bury) bury_an_obj(otmp);
	}
	else add_to_container(container, otmp);
}

STATIC_OVL
void
mkmivaultlolth()
{
	int x,y,tries=0;
	int i,j;
	struct obj *otmp;
	struct obj *otmp2;
	struct monst *mon;
	boolean good=FALSE,okspot;
	struct permonst *ptr;
	while(!good && tries < 50){
		x = rn2(COLNO-4)+1;
		y = rn2(ROWNO-4)+1;
		tries++;
		okspot = TRUE;
		for(i=0;i<4;i++) for(j=0;j<4;j++) if(!isok(x+i,y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == CLOUD) || m_at(x+i, y+j)) okspot = FALSE;
		if(okspot){
			good = TRUE;
			for(i=0;i<4;i++) for(j=0;j<4;j++) levl[x+i][y+j].typ = CORR;
			for(i=0;i<4;i++){
				levl[x][y+i].edge = 1;
				levl[x+i][y].edge = 1;
				levl[x+3][y+i].edge = 1;
				levl[x+i][y+3].edge = 1;
			}
			levl[x+3][y+3].typ = BRCORNER;
			levl[x][y+3].typ = BLCORNER;
			for(i=1;i<3;i++) levl[x+i][y+3].typ = HWALL;
			for(i=1;i<3;i++) levl[x+i][y].typ = HWALL;
			for(i=1;i<3;i++) levl[x+3][y+i].typ = VWALL;
			for(i=1;i<3;i++) levl[x][y+i].typ = VWALL;
			levl[x+3][y].typ = TRCORNER;
			levl[x][y].typ = TLCORNER;
			
			
			otmp = mksobj_at(CHEST, x+1, y+1, NO_MKOBJ_FLAGS);
			for(i = d(2,4);i>0;i--) mkmivaultitem(otmp);
			otmp = mksobj_at(CHEST, x+1, y+2, NO_MKOBJ_FLAGS);
			for(i = d(2,4);i>0;i--) mkmivaultitem(otmp);
			otmp = mksobj_at(CHEST, x+2, y+1, NO_MKOBJ_FLAGS);
			for(i = d(2,4);i>0;i--) mkmivaultitem(otmp);
			otmp = mksobj_at(CHEST, x+2, y+2, NO_MKOBJ_FLAGS);
			for(i = d(2,4);i>0;i--) mkmivaultitem(otmp);
			
			for (i = 0; i < 10; i++){
				ptr = mivaultmon();
				mon = makemon(ptr, x+rnd(2)+1, y+rnd(2)+1, 0);
				if (mon) break;
			}

			// lovecraft monsters
			if (ptr->mtyp == PM_SHOGGOTH || ptr->mtyp == PM_NIGHTGAUNT 
				|| ptr->mtyp == PM_DARK_YOUNG || ptr->mtyp == PM_HUNTING_HORROR || ptr->mtyp == PM_HUNTING_HORROR_TAIL){
				otmp2 = mksobj(FIGURINE, NO_MKOBJ_FLAGS);
				switch(rn2(4)){
					case 0:					
						otmp2->corpsenm = PM_DARK_YOUNG;
					break;
					case 1:
						otmp2->corpsenm = PM_SHOGGOTH;
						if(rn2(5)){
							otmp2->corpsenm = PM_PRIEST_OF_GHAUNADAUR;
						}
					break;
					case 2:
						otmp2->corpsenm = PM_DARKNESS_GIVEN_HUNGER;
					break;
					case 3:
						otmp2->corpsenm = PM_MASTER_MIND_FLAYER;
					break;
					case 4:
						otmp2->corpsenm = PM_DEEPEST_ONE;
					break;
				}
				bless(otmp2);
				add_to_container(otmp, otmp2);
			}
			// clockworks
			else if (ptr->mtyp == PM_JUGGERNAUT || ptr->mtyp == PM_ID_JUGGERNAUT 
					|| ptr->mtyp == PM_SCRAP_TITAN || ptr->mtyp == PM_HELLFIRE_COLOSSUS){
				if (rn2(2)){
					add_to_container(otmp, mksobj(ptr->mtyp == PM_ID_JUGGERNAUT ? SUBETHAIC_COMPONENT : HELLFIRE_COMPONENT, 0));
				} else {
					add_to_container(otmp, mksobj(SCRAP, 0));
				}
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
				add_to_container(otmp, mksobj(CLOCKWORK_COMPONENT, 0));
			}
			// spellcasters
			else if (ptr->mtyp == PM_TITAN || ptr->mtyp == PM_EYE_OF_DOOM 
					|| ptr->mtyp == PM_PRIEST_OF_GHAUNADAUR || ptr->mtyp == PM_PRIESTESS_OF_GHAUNADAUR
						|| ptr->mtyp == PM_SERPENT_MAN_OF_YOTH){
				add_to_container(otmp, mkobj(SPBOOK_CLASS, FALSE));
				add_to_container(otmp, mkobj(SPBOOK_CLASS, FALSE));
				if (rn2(2)) add_to_container(otmp, mkobj(SPBOOK_CLASS, FALSE));
				add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
				add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
				add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
				if (rn2(2)) add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
				if (rn2(2)) add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
				if (rn2(2)) add_to_container(otmp, mkobj(SCROLL_CLASS, FALSE));
			}
			if (mon){
				mon->mstrategy |= STRAT_WAITFORU;
				mon->mpeaceful = FALSE;
				set_malign(mon);
			}
		}
	}
}

STATIC_OVL
void
mkvaultlolth()
{
	int x,y,tries=0;
	int i,j;
	struct obj *otmp;
	struct monst *mon;
	boolean good=FALSE,okspot;
	while(!good && tries < 50){
		x = rn2(COLNO-4)+1;
		y = rn2(ROWNO-4)+1;
		tries++;
		okspot = TRUE;
		for(i=0;i<4;i++) for(j=0;j<4;j++) if(!isok(x+i,y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == CLOUD) || m_at(x+i, y+j)) okspot = FALSE;
		if(okspot){
			good = TRUE;
			for(i=0;i<4;i++) for(j=0;j<4;j++) levl[x+i][y+j].typ = CORR;
			for(i=0;i<4;i++){
				levl[x][y+i].edge = 1;
				levl[x+i][y].edge = 1;
				levl[x+3][y+i].edge = 1;
				levl[x+i][y+3].edge = 1;
			}
			levl[x+3][y+3].typ = BRCORNER;
			levl[x][y+3].typ = BLCORNER;
			for(i=1;i<3;i++) levl[x+i][y+3].typ = HWALL;
			for(i=1;i<3;i++) levl[x+i][y].typ = HWALL;
			for(i=1;i<3;i++) levl[x+3][y+i].typ = VWALL;
			for(i=1;i<3;i++) levl[x][y+i].typ = VWALL;
			levl[x+3][y].typ = TRCORNER;
			levl[x][y].typ = TLCORNER;
			
			mkgold((long) rn1((10+rnd(10)) * level_difficulty(),10), x+1, y+1);
			mkgold((long) rn1((10+rnd(10)) * level_difficulty(),10), x+2, y+1);
			mkgold((long) rn1((10+rnd(10)) * level_difficulty(),10), x+1, y+2);
			mkgold((long) rn1((10+rnd(10)) * level_difficulty(),10), x+2, y+2);
			
			switch(rn2(3)){
				case 0:
					makemon(&mons[PM_SKELETON], x+rnd(2), y+rnd(2), 0);
				break;
				case 1:
					makemon(&mons[PM_DROW_MUMMY], x+rnd(2), y+rnd(2), 0);
				break;
				case 2:
					makemon(&mons[PM_HEDROW_ZOMBIE], x+rnd(2), y+rnd(2), 0);
					if(rn2(4)) makemon(&mons[PM_HEDROW_ZOMBIE], x+rnd(2), y+rnd(2), 0);
					if(!rn2(3)) makemon(&mons[PM_HEDROW_ZOMBIE], x+rnd(2), y+rnd(2), 0);
				break;
			}
			if(!rn2(8)){
				//Note: lack of adjacent flag means that some of these may not spawn (this is on purpose)
				makemon(&mons[PM_ALABASTER_MUMMY], x+rnd(2), y+rnd(2), 0);
				if(rn2(4)) makemon(&mons[PM_HEDROW_ZOMBIE], x+rnd(2), y+rnd(2), 0);
				if(!rn2(3)) makemon(&mons[PM_HEDROW_ZOMBIE], x+rnd(2), y+rnd(2), 0);
			}
		}
	}
}

STATIC_OVL
void
mklolthgnoll()
{
	int x,y,tries=0;
	int i,j, rmtypb = nroom+ROOMOFFSET, trycount, madedoor;
	struct obj *otmp;
	struct monst *mon;
	boolean good=FALSE,okspot;
	while(!good && tries < 50){
		x = rn2(COLNO-10)+1;
		y = rn2(ROWNO-10)+1;
		tries++;
		okspot = TRUE;
		for(i=0;i<10;i++) for(j=0;j<10;j++) if(!isok(x+i,y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == CLOUD) || m_at(x+i, y+j)) okspot = FALSE;
		if(okspot){
			good = TRUE;
			for(i=0;i<10;i++) for(j=0;j<10;j++){
				levl[x+i][y+j].typ = CORR;
				levl[x+i][y+j].roomno = rmtypb;
			}
			for(i=0;i<10;i++){
				levl[x][y+i].edge = 1;
				levl[x+i][y].edge = 1;
				levl[x+9][y+i].edge = 1;
				levl[x+i][y+9].edge = 1;
			}
			levl[x+9][y+9].typ = BRCORNER;
			levl[x][y+9].typ = BLCORNER;
			for(i=1;i<9;i++) levl[x+i][y+9].typ = HWALL;
			for(i=1;i<9;i++) levl[x+i][y].typ = HWALL;
			for(i=1;i<9;i++) levl[x+9][y+i].typ = VWALL;
			for(i=1;i<9;i++) levl[x][y+i].typ = VWALL;
			levl[x+9][y].typ = TRCORNER;
			levl[x][y].typ = TLCORNER;
			
			rooms[nroom].lx = x + 1;
			rooms[nroom].hx = x + 8;
			rooms[nroom].ly = y + 1;
			rooms[nroom].hy = y + 8;
			rooms[nroom].rtype = BARRACKS;
			rooms[nroom].doorct = 0;
			rooms[nroom].fdoor = 0;
			rooms[nroom].nsubrooms = 0;
			rooms[nroom].irregular = FALSE;
			rooms[nroom].solidwall = 0;
			nroom++;
			
			for(i = 2; i < 8; i++) for(j = 2; j < 8; j++){
				if(rn2(10)){
					mon = makemon(&mons[PM_GNOLL], x+i, y+j, 0);
					if(mon) mon->msleeping = 1;
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(8);
				if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
					madedoor++;
					levl[x+i][y].typ = DOOR;
					levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= 8 && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(8);
				if(isok(x+i,y+10) && levl[x+i][y+10].typ == ROOM){
					madedoor++;
					levl[x+i][y+9].typ = DOOR;
					levl[x+i][y+9].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= 8 && madedoor < 2; i++){
					if(isok(x+i,y+10) && levl[x+i][y+10].typ == ROOM){
						madedoor++;
						levl[x+i][y+9].typ = DOOR;
						levl[x+i][y+9].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(8);
				if(isok(x-1,y+i) && levl[x-1][y+i].typ == ROOM){
					madedoor++;
					levl[x][y+i].typ = DOOR;
					levl[x][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= 8 && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(8);
				if(isok(x+10,y+i) && levl[x+10][y+i].typ == ROOM){
					madedoor++;
					levl[x+9][y+i].typ = DOOR;
					levl[x+9][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= 8 && madedoor < 2; i++){
					if(isok(x+10,y+i) && levl[x+10][y+i].typ == ROOM){
						madedoor++;
						levl[x+9][y+i].typ = DOOR;
						levl[x+9][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
		}
	}
}


STATIC_OVL
void
mklolthgarden()
{
	int x,y,tries=0, width= rn1(4,5), height=rn1(4,5);
	int i,j, rmtypb = nroom+ROOMOFFSET, trycount, madedoor;
	struct obj *otmp;
	struct monst *mon;
	boolean good=FALSE,okspot;
	while(!good && tries < 50){
		x = rn2(COLNO-width)+1;
		y = rn2(ROWNO-height)+1;
		tries++;
		okspot = TRUE;
		for(i=0;i<width;i++) for(j=0;j<height;j++) if(!isok(x+i,y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == CLOUD) || m_at(x+i, y+j)) okspot = FALSE;
		if(okspot){
			good = TRUE;
			for(i=0;i<width;i++) for(j=0;j<height;j++){
				levl[x+i][y+j].typ = ROOM;
			}
			levl[x+(width-1)][y+(height-1)].typ = BRCORNER;
			levl[x][y+(height-1)].typ = BLCORNER;
			for(i=1;i<(width-1);i++) levl[x+i][y+(height-1)].typ = HWALL;
			for(i=1;i<(width-1);i++) levl[x+i][y].typ = HWALL;
			for(i=1;i<(height-1);i++) levl[x+(width-1)][y+i].typ = VWALL;
			for(i=1;i<(height-1);i++) levl[x][y+i].typ = VWALL;
			levl[x+(width-1)][y].typ = TRCORNER;
			levl[x][y].typ = TLCORNER;
			
			rooms[nroom].lx = x + 1;
			rooms[nroom].hx = x + width - 2;
			rooms[nroom].ly = y + 1;
			rooms[nroom].hy = y + height - 2;
			rooms[nroom].rtype = GARDEN;
			rooms[nroom].doorct = 0;
			rooms[nroom].fdoor = 0;
			rooms[nroom].nsubrooms = 0;
			rooms[nroom].irregular = FALSE;
			rooms[nroom].solidwall = 0;
			mkgarden(&rooms[nroom]);
			nroom++;
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
					madedoor++;
					levl[x+i][y].typ = DOOR;
					levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
					madedoor++;
					levl[x+i][y+height-1].typ = DOOR;
					levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
						madedoor++;
						levl[x+i][y+height-1].typ = DOOR;
						levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x-1,y+i) && levl[x-1][y+i].typ == ROOM){
					madedoor++;
					levl[x][y+i].typ = DOOR;
					levl[x][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
					madedoor++;
					levl[x+width-1][y+i].typ = DOOR;
					levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
						madedoor++;
						levl[x+width-1][y+i].typ = DOOR;
						levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
		}
	}
}


STATIC_OVL
void
mklolthtroll()
{
	int x,y,tries=0, width= 6, height=6;
	int i,j, rmtypb = nroom+ROOMOFFSET, trycount, madedoor;
	struct obj *otmp;
	struct monst *mon;
	boolean good=FALSE,okspot;
	while(!good && tries < 50){
		x = rn2(COLNO-width)+1;
		y = rn2(ROWNO-height)+1;
		tries++;
		okspot = TRUE;
		for(i=0;i<width;i++) for(j=0;j<height;j++) if(!isok(x+i,y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == CLOUD) || m_at(x+i, y+j)) okspot = FALSE;
		if(okspot){
			good = TRUE;
			for(i=0;i<width;i++) for(j=0;j<height;j++){
				levl[x+i][y+j].typ = CORR;
			}
			levl[x+(width-1)][y+(height-1)].typ = BRCORNER;
			levl[x][y+(height-1)].typ = BLCORNER;
			for(i=1;i<(width-1);i++) levl[x+i][y+(height-1)].typ = HWALL;
			for(i=1;i<(width-1);i++) levl[x+i][y].typ = HWALL;
			for(i=1;i<(height-1);i++) levl[x+(width-1)][y+i].typ = VWALL;
			for(i=1;i<(height-1);i++) levl[x][y+i].typ = VWALL;
			levl[x+(width-1)][y].typ = TRCORNER;
			levl[x][y].typ = TLCORNER;
			
			for(i = 0; i < 10; i++) makemon(mkclass(S_TROLL, G_HELL|G_NOHELL), x+3, y+3, MM_ADJACENTOK);
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
					madedoor++;
					levl[x+i][y].typ = DOOR;
					levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
					madedoor++;
					levl[x+i][y+height-1].typ = DOOR;
					levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
						madedoor++;
						levl[x+i][y+height-1].typ = DOOR;
						levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x-1,y+i) && levl[x-1][y+i].typ == ROOM){
					madedoor++;
					levl[x][y+i].typ = DOOR;
					levl[x][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
					madedoor++;
					levl[x+width-1][y+i].typ = DOOR;
					levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
						madedoor++;
						levl[x+width-1][y+i].typ = DOOR;
						levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
		}
	}
}


STATIC_OVL
void
mklolthtemple()
{
	int x,y,tries=0, width= 7, height=7;
	int i,j, rmtypb = nroom+ROOMOFFSET, trycount, madedoor;
	struct obj *otmp;
	struct monst *mon;
	boolean good=FALSE,okspot;
	while(!good && tries < 50){
		x = rn2(COLNO-width)+1;
		y = rn2(ROWNO-height)+1;
		tries++;
		okspot = TRUE;
		for(i=0;i<width;i++) for(j=0;j<height;j++) if(!isok(x+i,y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == CLOUD) || m_at(x+i, y+j)) okspot = FALSE;
		if(okspot){
			good = TRUE;
			levl[x+(width-1)][y+(height-1)].typ = BRCORNER;
			levl[x][y+(height-1)].typ = BLCORNER;
			for(i=1;i<(width-1);i++) levl[x+i][y+(height-1)].typ = HWALL;
			for(i=1;i<(width-1);i++) levl[x+i][y].typ = HWALL;
			for(i=1;i<(height-1);i++) levl[x+(width-1)][y+i].typ = VWALL;
			for(i=1;i<(height-1);i++) levl[x][y+i].typ = VWALL;
			levl[x+(width-1)][y].typ = TRCORNER;
			levl[x][y].typ = TLCORNER;
			for(i=1;i<width-1;i++) for(j=1;j<height-1;j++){
				levl[x+i][y+j].typ = CORR;
				levl[x+i][y+j].roomno = rmtypb;
			}
			for(i=2;i<width-2;i++) for(j=2;j<height-2;j++){
				levl[x+i][y+j].typ = ROOM;
				levl[x+i][y+j].roomno = rmtypb;
			}
			rooms[nroom].lx = x + 1;
			rooms[nroom].hx = x + width - 2;
			rooms[nroom].ly = y + 1;
			rooms[nroom].hy = y + height - 2;
			rooms[nroom].rtype = TEMPLE;
			rooms[nroom].doorct = 0;
			rooms[nroom].fdoor = 0;
			rooms[nroom].nsubrooms = 0;
			rooms[nroom].irregular = FALSE;
			rooms[nroom].solidwall = 0;
			
			coord shrine_spot = {x+width/2, y+height/2};
			add_altar(shrine_spot.x, shrine_spot.y, u.ualign.type, TRUE, u.ualign.god);
			priestini(&u.uz, &rooms[nroom], shrine_spot.x, shrine_spot.y, FALSE);
			level.flags.has_temple = 1;
			
			nroom++;
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
					madedoor++;
					levl[x+i][y].typ = DOOR;
					levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
					madedoor++;
					levl[x+i][y+height-1].typ = DOOR;
					levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
						madedoor++;
						levl[x+i][y+height-1].typ = DOOR;
						levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x-1,y+i) && levl[x-1][y+i].typ == ROOM){
					madedoor++;
					levl[x][y+i].typ = DOOR;
					levl[x][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
					madedoor++;
					levl[x+width-1][y+i].typ = DOOR;
					levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
						madedoor++;
						levl[x+width-1][y+i].typ = DOOR;
						levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
		}
	}
}

STATIC_OVL
void
mklolthcell()
{
	int x,y,tries=0, width= 3, height=3;
	int i,j, rmtypb = nroom+ROOMOFFSET, trycount, madedoor;
	struct obj *obj;
	struct monst *mon, *warden = 0;
	boolean good=FALSE,okspot;
	int mtyp;
	if(!rn2(4)){
		width = height = 4;
	}
	while(!good && tries < 50){
		x = rn2(COLNO-width)+1;
		y = rn2(ROWNO-height)+1;
		tries++;
		okspot = TRUE;
		for(i=0;i<width;i++) for(j=0;j<height;j++) if(!isok(x+i,y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == CLOUD) || m_at(x+i, y+j)) okspot = FALSE;
		if(okspot){
			good = TRUE;
			for(i=0;i<width;i++) for(j=0;j<height;j++){
				levl[x+i][y+j].typ = STONE;
			}
			warden = makemon(&mons[PM_STINKING_CLOUD], x, y, MM_NOCOUNTBIRTH|MM_ADJACENTOK);
			if(warden){
				warden->mhpmax = 8*warden->m_lev;
				warden->mhpmax += rn2(8);
				warden->mhp = warden->mhpmax;
			}
			for(i=0;i<width;i++) for(j=0;j<height;j++){
				levl[x+i][y+j].typ = CORR;
			}
			levl[x+(width-1)][y+(height-1)].typ = BRCORNER;
			levl[x][y+(height-1)].typ = BLCORNER;
			for(i=1;i<(width-1);i++) levl[x+i][y+(height-1)].typ = IRONBARS;
			for(i=1;i<(width-1);i++) levl[x+i][y].typ = IRONBARS;
			for(i=1;i<(height-1);i++) levl[x+(width-1)][y+i].typ = IRONBARS;
			for(i=1;i<(height-1);i++) levl[x][y+i].typ = IRONBARS;
			levl[x+(width-1)][y].typ = TRCORNER;
			levl[x][y].typ = TLCORNER;
			for(i=1;i<width-1;i++){
				for(j=1;j<height-1;j++){
					mon = prisoner(PM_AVATAR_OF_LOLTH, i+x, j+y);
					if(mon){
						mon->mpetitioner = FALSE; //Not actually dead yet.
						if(warden){
							for(obj = mon->minvent; obj; obj = mon->minvent){
								//The wardens are pinatas so give them an extra HP per item they hold.
								warden->mhpmax++;
								warden->mhp++;
								warden->m_lev = warden->mhpmax/8;
								mon->misc_worn_check &= ~obj->owornmask;
								update_mon_intrinsics(mon, obj, FALSE, FALSE);
								if (obj->owornmask & W_WEP){
									setmnotwielded(mon,obj);
									MON_NOWEP(mon);
								}
								if (obj->owornmask & W_SWAPWEP){
									setmnotwielded(mon,obj);
									MON_NOSWEP(mon);
								}
								obj->owornmask = 0L;
								obj_extract_self(obj);
								mpickobj(warden, obj);
							}
						}
						(void)mongets(mon, SHACKLES, NO_MKOBJ_FLAGS);
						mon->entangled = SHACKLES;
					}
				}
			}
		}
	}
}

STATIC_OVL
void
mklolthtreasure()
{
	int x,y,tries=0, width= 5, height=5;
	int i,j, rmtypb = nroom+ROOMOFFSET, trycount, madedoor;
	struct obj *otmp, *container;
	struct monst *mon;
	boolean good=FALSE,okspot;
	while(!good && tries < 50){
		x = rn2(COLNO-width)+1;
		y = rn2(ROWNO-height)+1;
		tries++;
		okspot = TRUE;
		for(i=0;i<width;i++) for(j=0;j<height;j++) if(!isok(x+i,y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == CLOUD) || m_at(x+i, y+j)) okspot = FALSE;
		if(okspot){
			good = TRUE;
			for(i=0;i<width;i++) for(j=0;j<height;j++){
				levl[x+i][y+j].typ = CORR;
			}
			levl[x+(width-1)][y+(height-1)].typ = BRCORNER;
			levl[x][y+(height-1)].typ = BLCORNER;
			for(i=1;i<(width-1);i++) levl[x+i][y+(height-1)].typ = HWALL;
			for(i=1;i<(width-1);i++) levl[x+i][y].typ = HWALL;
			for(i=1;i<(height-1);i++) levl[x+(width-1)][y+i].typ = VWALL;
			for(i=1;i<(height-1);i++) levl[x][y+i].typ = VWALL;
			levl[x+(width-1)][y].typ = TRCORNER;
			levl[x][y].typ = TLCORNER;
			
			makemon(&mons[PM_HEDROW_WIZARD], x+2, y+2, MM_ADJACENTOK);
			makemon(&mons[PM_DROW_MATRON], x+2, y+2, MM_ADJACENTOK);
			makemon(&mons[PM_DROW_MATRON], x+2, y+2, MM_ADJACENTOK);
			makemon(&mons[PM_DROW_MATRON], x+2, y+2, MM_ADJACENTOK);
			makemon(&mons[PM_DROW_MATRON], x+2, y+2, MM_ADJACENTOK);
			
			container = mksobj_at(CHEST, x+2, y+2, NO_MKOBJ_FLAGS);
			container->olocked = 1;
			container->otrapped = 1;
			otmp = mksobj(POT_FULL_HEALING, NO_MKOBJ_FLAGS);
			(void) add_to_container(container, otmp);
			otmp = mksobj(POT_FULL_HEALING, NO_MKOBJ_FLAGS);
			(void) add_to_container(container, otmp);
			for(i = d(2,8); i > 0; i--){
				otmp = mksobj(rn2(LAST_GEM-DILITHIUM_CRYSTAL)+DILITHIUM_CRYSTAL, NO_MKOBJ_FLAGS);
				(void) add_to_container(container, otmp);
			}
			for(i = d(2,4); i > 0; i--){
				otmp = mkgold((long)rn1(depth(&u.uz) * 20 + 100, 175), x+2, y+2);
				remove_object(otmp);
				(void) add_to_container(container, otmp);
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
					madedoor++;
					levl[x+i][y].typ = DOOR;
					levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
					madedoor++;
					levl[x+i][y+height-1].typ = DOOR;
					levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
						madedoor++;
						levl[x+i][y+height-1].typ = DOOR;
						levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x-1,y+i) && levl[x-1][y+i].typ == ROOM){
					madedoor++;
					levl[x][y+i].typ = DOOR;
					levl[x][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
					madedoor++;
					levl[x+width-1][y+i].typ = DOOR;
					levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
						madedoor++;
						levl[x+width-1][y+i].typ = DOOR;
						levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
		}
	}
}


STATIC_OVL
void
mklolthup()
{
	int x,y,tries=0,madedoor,trycount;
	int i,j, width=5, height=5;
	struct obj *otmp;
	struct monst *mon;
	boolean good=FALSE,okspot;
	while(!good){
		x = rn2(COLNO-width)+1;
		y = rn2(ROWNO-height)+1;
		tries++;
		okspot = TRUE;
		for(i=0;i<width;i++) for(j=0;j<height;j++) if(!isok(x+i,y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == CLOUD) || m_at(x+i, y+j)) okspot = FALSE;
		if(okspot){
			good = TRUE;
			for(i=0;i<width;i++) for(j=0;j<height;j++){
				levl[x+i][y+j].typ = CORR;
			}
			levl[x+(width-1)][y+(height-1)].typ = BRCORNER;
			levl[x][y+(height-1)].typ = BLCORNER;
			for(i=1;i<(width-1);i++) levl[x+i][y+(height-1)].typ = HWALL;
			for(i=1;i<(width-1);i++) levl[x+i][y].typ = HWALL;
			for(i=1;i<(height-1);i++) levl[x+(width-1)][y+i].typ = VWALL;
			for(i=1;i<(height-1);i++) levl[x][y+i].typ = VWALL;
			levl[x+(width-1)][y].typ = TRCORNER;
			levl[x][y].typ = TLCORNER;
			
			xupstair = x+2;
			yupstair = y+2;
			
			levl[x+2][y+2].typ = STAIRS;
			levl[x+2][y+2].ladder = LA_UP;
			
			makemon(&mons[PM_VROCK], x+1, y+1, NO_MM_FLAGS);
			makemon(&mons[PM_VROCK], x+1, y+3, NO_MM_FLAGS);
			makemon(&mons[PM_VROCK], x+3, y+3, NO_MM_FLAGS);
			makemon(&mons[PM_VROCK], x+3, y+1, NO_MM_FLAGS);
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
					madedoor++;
					levl[x+i][y].typ = DOOR;
					levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
					madedoor++;
					levl[x+i][y+height-1].typ = DOOR;
					levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
						madedoor++;
						levl[x+i][y+height-1].typ = DOOR;
						levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x-1,y+i) && levl[x-1][y+i].typ == ROOM){
					madedoor++;
					levl[x][y+i].typ = DOOR;
					levl[x][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
					madedoor++;
					levl[x+width-1][y+i].typ = DOOR;
					levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
						madedoor++;
						levl[x+width-1][y+i].typ = DOOR;
						levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
		}
	}
}

STATIC_OVL
void
mklolthdown()
{
	int x,y,tries=0, madedoor, trycount;
	int i,j, width=5, height=5;
	struct obj *otmp;
	struct monst *mon;
	boolean good=FALSE,okspot;
	while(!good){
		x = rn2(COLNO-width)+1;
		y = rn2(ROWNO-height)+1;
		tries++;
		okspot = TRUE;
		for(i=0;i<width;i++) for(j=0;j<height;j++) if(!isok(x+i,y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == CLOUD) || m_at(x+i, y+j)) okspot = FALSE;
		if(okspot){
			good = TRUE;
			for(i=0;i<width;i++) for(j=0;j<height;j++){
				levl[x+i][y+j].typ = CORR;
			}
			levl[x+(width-1)][y+(height-1)].typ = BRCORNER;
			levl[x][y+(height-1)].typ = BLCORNER;
			for(i=1;i<(width-1);i++) levl[x+i][y+(height-1)].typ = HWALL;
			for(i=1;i<(width-1);i++) levl[x+i][y].typ = HWALL;
			for(i=1;i<(height-1);i++) levl[x+(width-1)][y+i].typ = VWALL;
			for(i=1;i<(height-1);i++) levl[x][y+i].typ = VWALL;
			levl[x+(width-1)][y].typ = TRCORNER;
			levl[x][y].typ = TLCORNER;
			
			xdnstair = x+2;
			ydnstair = y+2;

			levl[x+2][y+2].typ = STAIRS;
			levl[x+2][y+2].ladder = LA_DOWN;
			
			makemon(&mons[PM_MARILITH], x+2, y+2, NO_MM_FLAGS);
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
					madedoor++;
					levl[x+i][y].typ = DOOR;
					levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(width-2);
				if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
					madedoor++;
					levl[x+i][y+height-1].typ = DOOR;
					levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (width-2) && madedoor < 2; i++){
					if(isok(x+i,y+height) && levl[x+i][y+height].typ == ROOM){
						madedoor++;
						levl[x+i][y+height-1].typ = DOOR;
						levl[x+i][y+height-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x-1,y+i) && levl[x-1][y+i].typ == ROOM){
					madedoor++;
					levl[x][y+i].typ = DOOR;
					levl[x][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+i,y-1) && levl[x+i][y-1].typ == ROOM){
						madedoor++;
						levl[x+i][y].typ = DOOR;
						levl[x+i][y].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
			
			madedoor = 0;
			trycount = 0;
			while(madedoor < 2 && trycount<50){
				i = rnd(height-2);
				if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
					madedoor++;
					levl[x+width-1][y+i].typ = DOOR;
					levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				trycount++;
			}
			if(madedoor == 0){
				for(i = 1; i <= (height-2) && madedoor < 2; i++){
					if(isok(x+width,y+i) && levl[x+width][y+i].typ == ROOM){
						madedoor++;
						levl[x+width-1][y+i].typ = DOOR;
						levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					}
				}
			}
		}
	}
}

static const int serpents[] = {PM_PYTHON, PM_COBRA, PM_PIT_VIPER, PM_PYTHON, PM_COBRA, PM_PIT_VIPER, PM_PYTHON, PM_COBRA, PM_PIT_VIPER, 
	PM_LONG_WORM, PM_PURPLE_WORM,
	PM_COUATL, PM_LILLEND,
	PM_RED_NAGA, PM_BLACK_NAGA, PM_GOLDEN_NAGA, PM_GUARDIAN_NAGA, 
	PM_GIANT_EEL, PM_ELECTRIC_EEL,
	PM_SALAMANDER,
	PM_MARILITH
	};

STATIC_OVL
void
mkkamereltowers()
{
	int x=0,y=0,tx, ty, tries=0;
	int i,j, c, edge;
	boolean left = rn2(2), good=FALSE, okspot;
	int slant = rn2(3);
	struct obj *otmp;
	struct monst *mtmp;
	if(left){
		//Make shallow sea
		edge = rn1(20, 20);
		for(j = 0; j < ROWNO; j++){
			for(i = 0; i < edge; i++){
				if(isok(x+i,y+j) && 
					(levl[x+i][y+j].typ == TREE 
					|| levl[x+i][y+j].typ == ROOM 
					|| levl[x+i][y+j].typ == GRASS 
					|| levl[x+i][y+j].typ == SOIL 
					|| levl[x+i][y+j].typ == SAND 
					|| levl[x+i][y+j].typ == PUDDLE)
				){
					if(levl[x+i][y+j].typ!=TREE || edge - i > rn2(6)) levl[x+i][y+j].typ = PUDDLE;
					levl[x+i][y+j].lit = 1;
					if(t_at(x+i, y+j))
						rloc_trap(t_at(x+i, y+j));
					if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
				}
			}
			if(rn2(4)) edge += rn2(3)-slant;
		}
		
		//Build central tower
		while(!good && tries < 500){
			x=4+rnd(3)+rn2(3);
			y=6+rn2(10);
			
			tries++;
			okspot = TRUE;
			for(i=-3;i<=3;i++)
				for(j=-3;j<=3;j++){
					if(!isok(x+i,y+j) || levl[x+i][y+j].typ != PUDDLE)
						okspot = FALSE;
				}
			if(okspot){
				good = TRUE;
			} else continue;
			
			//Good to go.  Mark tower now
			level.flags.has_kamerel_towers = 1;
			
			for(i=-3;i<=3;i++)
				for(j=-3;j<=3;j++){
					if(dist2(x+i,y+j,x,y)<=14){
						levl[x+i][y+j].typ = HWALL;
						levl[x+i][y+j].lit = 1;
						if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
					}
				}
			for(i=-2;i<=2;i++)
				for(j=-2;j<=2;j++){
					if(dist2(x+i,y+j,x,y)<=5){
						levl[x+i][y+j].typ = CORR;
						levl[x+i][y+j].lit = 0;
						if(i != 0 || j != 0){
							mkcorpstat(STATUE, (struct monst *) 0, &mons[serpents[rn2(SIZE(serpents))]], x+i, y+j, FALSE);
							mkcorpstat(STATUE, (struct monst *) 0, &mons[PM_AMM_KAMEREL], x+i, y+j, TRUE);
						}
					}
				}
			for(i=-3;i<=3;i++)
				for(j=-3;j<=3;j++){
					if(levl[x+i][y+j].typ == HWALL){
						otmp = mksobj(MIRROR, MKOBJ_NOINIT);
						otmp->objsize = MZ_GIGANTIC;
						fix_object(otmp);
						place_object(otmp, x+i, y+j);
					}
				}
			wallification(x-3, y-3, x+3, y+3);
			
			tries = 0;
			otmp = 0;
			while(!otmp && tries < 20){
				switch(rn2(4)) {
				case 0:
					otmp = oname(mksobj(DOUBLE_LIGHTSABER, MKOBJ_NOINIT), artiname(ART_INFINITY_S_MIRRORED_ARC));
					break;
				case 1:
					otmp = oname(mksobj(MIRRORBLADE, MKOBJ_NOINIT), artiname(ART_SANSARA_MIRROR));
					break;
				case 2:
					otmp = oname(mksobj(KHAKKHARA, MKOBJ_NOINIT), artiname(ART_STAFF_OF_TWELVE_MIRRORS));
					break;
				case 3:
					otmp = oname(mksobj(MASK, MKOBJ_NOINIT), artiname(ART_MIRRORED_MASK));
					break;
				}
				if (otmp->oartifact) {
					otmp->cursed = 0;
					otmp->blessed = 0;
					if (otmp->oclass == WEAPON_CLASS || is_weptool(otmp))
						otmp->spe = 1;
					if (is_lightsaber(otmp))
						otmp->age = 0;
					place_object(otmp, x, y);
				}
				else {
					obfree(otmp, (struct obj *)0);
					otmp = 0;
					tries++;
					continue;
				}
			}
			//record central tower location
			tx = x;
			ty = y;
		}
	} else {
		edge = COLNO - rn1(20, 20);
		for(j = 0; j < ROWNO; j++){
			for(i = COLNO; i > edge; i--){
				if(isok(x+i,y+j) && 
					(levl[x+i][y+j].typ == TREE || levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == GRASS || levl[x+i][y+j].typ == PUDDLE)
				){
					if(levl[x+i][y+j].typ!=TREE || i - edge > rn2(6)) levl[x+i][y+j].typ = PUDDLE;
					levl[x+i][y+j].lit = 1;
					if(t_at(x+i, y+j))
						rloc_trap(t_at(x+i, y+j));
					if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
				}
			}
			if(rn2(4)) edge += rn2(3)-slant;
		}
		
		//Build central tower
		while(!good && tries < 500){
			x=COLNO - (4+rnd(3)+rn2(3));
			y=6+rn2(10);
			
			tries++;
			okspot = TRUE;
			for(i=-3;i<=3;i++)
				for(j=-3;j<=3;j++){
					if(!isok(x+i,y+j) || levl[x+i][y+j].typ != PUDDLE)
						okspot = FALSE;
				}
			if(okspot){
				good = TRUE;
			} else continue;
			
			//Good to go.  Mark tower now
			level.flags.has_kamerel_towers = 1;
			
			for(i=-3;i<=3;i++)
				for(j=-3;j<=3;j++){
					if(dist2(x+i,y+j,x,y)<=14){
						levl[x+i][y+j].typ = HWALL;
						levl[x+i][y+j].lit = 1;
						if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
					}
				}
			for(i=-2;i<=2;i++)
				for(j=-2;j<=2;j++){
					if(dist2(x+i,y+j,x,y)<=5){
						levl[x+i][y+j].typ = CORR;
						levl[x+i][y+j].lit = 0;
						if(i != 0 || j != 0){
							mkcorpstat(STATUE, (struct monst *) 0, &mons[serpents[rn2(SIZE(serpents))]], x+i, y+j, FALSE);
							mkcorpstat(STATUE, (struct monst *) 0, &mons[PM_AMM_KAMEREL], x+i, y+j, TRUE);
						}
					}
				}
			for(i=-3;i<=3;i++)
				for(j=-3;j<=3;j++){
					if(levl[x+i][y+j].typ == HWALL){
						otmp = mksobj(MIRROR, MKOBJ_NOINIT);
						otmp->objsize = MZ_GIGANTIC;
						fix_object(otmp);
						place_object(otmp, x+i, y+j);
					}
				}
			wallification(x-3, y-3, x+3, y+3);
			tries = 0;
			otmp = 0;
			while(!otmp && tries < 20){
				switch(rn2(4)) {
				case 0:
					otmp = oname(mksobj(DOUBLE_LIGHTSABER, MKOBJ_NOINIT), artiname(ART_INFINITY_S_MIRRORED_ARC));
					break;
				case 1:
					otmp = oname(mksobj(MIRRORBLADE, MKOBJ_NOINIT), artiname(ART_SANSARA_MIRROR));
					break;
				case 2:
					otmp = oname(mksobj(KHAKKHARA, MKOBJ_NOINIT), artiname(ART_STAFF_OF_TWELVE_MIRRORS));
					break;
				case 3:
					otmp = oname(mksobj(MASK, MKOBJ_NOINIT), artiname(ART_MIRRORED_MASK));
					break;
				}
				if (otmp->oartifact) {
					otmp->cursed = 0;
					otmp->blessed = 0;
					if (otmp->oclass == WEAPON_CLASS || is_weptool(otmp))
						otmp->spe = 1;
					if (is_lightsaber(otmp))
						otmp->age = 0;
					place_object(otmp, x, y);
				}
				else {
					obfree(otmp, (struct obj *)0);
					otmp = 0;
					tries++;
					continue;
				}
			}
			//record central tower location
			tx = x;
			ty = y;
		}
	}
	
	//Build 3 square towers
	c = 1 + rn2(3);
	while(c > 0){
		good = FALSE;
		tries = 0;
		while(!good && tries < 50){
			x=tx+rn2(17)-8;
			y=ty+rn2(17)-8;
			
			tries++;
			okspot = TRUE;
			for(i=-3;i<=3;i++)
				for(j=-3;j<=3;j++){ //+/-3: walkable perimeter
					if(!isok(x+i,y+j) || levl[x+i][y+j].typ != PUDDLE)
						okspot = FALSE;
				}
			if(okspot){
				good = TRUE;
			} else continue;
			for(i=-2;i<=2;i++)
				for(j=-2;j<=2;j++){
					levl[x+i][y+j].typ = HWALL;
					levl[x+i][y+j].lit = 1;
					if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
				}
			for(i=-1;i<=1;i++)
				for(j=-1;j<=1;j++){
					levl[x+i][y+j].typ = CORR;
					levl[x+i][y+j].lit = 0;
					if(i != 0 || j != 0){
						mkcorpstat(STATUE, (struct monst *) 0, &mons[serpents[rn2(3)]], x+i, y+j, FALSE);
						mkcorpstat(STATUE, (struct monst *) 0, &mons[PM_AMM_KAMEREL], x+i, y+j, TRUE);
					}
				}
			for(i=-2;i<=2;i++)
				for(j=-2;j<=2;j++){
					if(levl[x+i][y+j].typ == HWALL){
						otmp = mksobj(MIRROR, MKOBJ_NOINIT);
						otmp->objsize = MZ_GIGANTIC;
						fix_object(otmp);
						place_object(otmp, x+i, y+j);
					}
				}
			switch(rnd(7)){
				case 1:
				mtmp = makemon(&mons[PM_AMM_KAMEREL], x, y, MM_ADJACENTOK);
				if (mtmp){
					mtmp->m_lev += 4;
					mtmp->mhpmax += 4*8;
					mtmp->mhp = mtmp->mhpmax;
					mtmp->mstrategy |= STRAT_WAITFORU;
					otmp = mkobj(RANDOM_CLASS, TRUE);
					(void) mpickobj(mtmp, otmp);
					otmp = mkobj(RANDOM_CLASS, TRUE);
					(void) mpickobj(mtmp, otmp);
					otmp = mkobj(RANDOM_CLASS, TRUE);
					(void) mpickobj(mtmp, otmp);
					otmp = mkobj(RANDOM_CLASS, TRUE);
					(void) mpickobj(mtmp, otmp);
				}
				break;
				case 2:
					otmp = mksobj(!rn2(3) ? MIRRORBLADE : rn2(2) ? KHAKKHARA : KAMEREL_VAJRA, MKOBJ_NOINIT);
					otmp->spe = 1;
					place_object(otmp, x, y);
				break;
				case 3:
					mksobj_at(MIRROR, x, y, MKOBJ_NOINIT);
				break;
				case 4:
					//Nothing
				break;
				case 5:{
					struct trap *ttmp = maketrap(x, y, SPIKED_PIT);
					ttmp->tseen = 1;/*Tower floor has been destroyed*/
				}break;
				case 6:
					levl[x][y].typ = HWALL;
					otmp = mksobj(MIRROR, MKOBJ_NOINIT);
					otmp->objsize = MZ_GIGANTIC;
					fix_object(otmp);
					place_object(otmp, x, y);
				break;
				case 7:
					mkobj_at(SPBOOK_CLASS, x, y, NO_MKOBJ_FLAGS);
				break;
			}
			
			wallification(x-2, y-2, x+2, y+2);
		}
		c--;
	}
	//Build 1 square towers
	c = rnd(3) + rn2(3);
	while(c > 0){
		good = FALSE;
		tries = 0;
		while(!good && tries < 50){
			x=tx+rn2(21)-10;
			y=ty+rn2(21)-10;
			
			tries++;
			okspot = TRUE;
			for(i=-2;i<=2;i++)
				for(j=-2;j<=2;j++){ //+/-2: walkable perimeter
					if(!isok(x+i,y+j) || levl[x+i][y+j].typ != PUDDLE)
						okspot = FALSE;
				}
			if(okspot){
				good = TRUE;
			} else continue;
			for(i=-1;i<=1;i++)
				for(j=-1;j<=1;j++){
					levl[x+i][y+j].typ = HWALL;
					levl[x+i][y+j].lit = 1;
					if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
				}
			levl[x][y].typ = CORR;
			levl[x][y].lit = 0;
			for(i=-1;i<=1;i++)
				for(j=-1;j<=1;j++){
					if(levl[x+i][y+j].typ == HWALL){
						otmp = mksobj(MIRROR, MKOBJ_NOINIT);
						otmp->objsize = MZ_GIGANTIC;
						fix_object(otmp);
						place_object(otmp, x+i, y+j);
					}
				}
			switch(rnd(5)){
				case 1:
					mtmp = makemon(&mons[PM_AMM_KAMEREL], x, y, MM_ADJACENTOK);
					if (mtmp){
						otmp = mkobj(RANDOM_CLASS, TRUE);
						(void) mpickobj(mtmp, otmp);
					}
				break;
				case 2:
					mksobj_at(MIRRORBLADE, x, y, MKOBJ_NOINIT);
				break;
				case 3:
					mksobj_at(MIRROR, x, y, MKOBJ_NOINIT);
				break;
				case 4:
					//Nothing
				break;
				case 5:{
					struct trap *ttmp = maketrap(x, y, SPIKED_PIT);
					ttmp->tseen = 1;/*Tower floor has been destroyed*/
				}break;
			}
			
			wallification(x-1, y-1, x+1, y+1);
		}
		c--;
	}
	//Place Fractured guards
	c = rnd(4) + rn2(4);
	while(c > 0){
		good = FALSE;
		tries = 0;
		while(!good && tries < 50){
			x=tx+rn2(25)-12;
			y=ty+rn2(25)-12;
			
			tries++;
			okspot = TRUE;
			if(!isok(x,y) || levl[x][y].typ != PUDDLE || m_at(x,y) || t_at(x,y))
				okspot = FALSE;
			if(okspot){
				good = TRUE;
			} else continue;
			mtmp = makemon(&mons[PM_HUDOR_KAMEREL], x, y, MM_ADJACENTOK);
		}
		c--;
	}
}

STATIC_OVL
void
mkminorspire()
{
	int x,y,ix, iy, tries=0;
	int i,j, c;
	boolean good=FALSE, okspot;
	struct obj *otmp;
	while(!good && tries < 50){
		x = rn2(COLNO-6)+3;
		y = rn2(ROWNO-5)+2;
		tries++;
		okspot = TRUE;
		for(i=0;i<3;i++)
			for(j=0;j<3;j++){
				if(!isok(x+i,y+j))
					okspot = FALSE;
			}
		if(okspot){
			good = TRUE;
		} else continue;
		
		level.flags.has_minor_spire = 1; /*Spawn kamerel survivors on this level*/
		ix = x;
		iy = y;
		for(i=-10;i<=10;i++){
			for(j=-10;j<=10;j++){
				if(isok(ix+i,iy+j) && dist2(ix, iy, ix+i, iy+j)<105){
					if(levl[ix+i][iy+j].typ == ROOM 
					|| levl[ix+i][iy+j].typ == GRASS 
					|| levl[ix+i][iy+j].typ == SOIL 
					|| levl[ix+i][iy+j].typ == SAND 
					|| (dist2(ix, iy, ix+i, iy+j)) < (rnd(8)*rnd(8)+36))
						levl[ix+i][iy+j].typ = PUDDLE;
					levl[ix+i][iy+j].lit = 1;
					if(t_at(x+i, y+j))
						rloc_trap(t_at(x+i, y+j));
					if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
				}
			}
		}
		// for(c=rnd(8)+10; c > 0; c--){
			// ix = x+rn2(21)-10;
			// iy = y+rn2(21)-10;
			// for(i=-4;i<=4;i++){
				// for(j=-4;j<=4;j++){
					// if(isok(ix+i,iy+j) && dist2(ix, iy, ix+i, iy+j)<16){
						// levl[ix+i][iy+j].typ = PUDDLE;
						// levl[ix+i][iy+j].lit = 1;
					// }
				// }
			// }
		// }
		for(i=-1;i<=1;i++){
			for(j=-1;j<=1;j++){
				levl[x+i][y+j].typ = HWALL;
				levl[x+i][y+j].lit = 1;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
		wallification(x-1, y-1, x+1, y+1);
		levl[x][y].wall_info |= W_NONDIGGABLE;
		levl[x][y].lit = 0;
		
		mksobj_at(ROBE, x, y, NO_MKOBJ_FLAGS);
		switch(rn2(6)){
			case 4:
			    otmp = mksobj(LONG_SWORD, MKOBJ_NOINIT);
				otmp = oname(otmp, artiname(ART_MIRROR_BRAND));
				if(!otmp->oartifact){
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					mksobj_at(SHIELD_OF_REFLECTION, x, y, NO_MKOBJ_FLAGS);
				}
				place_object(otmp, x, y);
			break;
			case 3:
			    otmp = mksobj(PLATE_MAIL, MKOBJ_NOINIT);
				otmp = oname(otmp, artiname(ART_SOULMIRROR));
				if(!otmp->oartifact){
					set_material_gm(otmp, MITHRIL);
					fix_object(otmp);
					mksobj_at(AMULET_OF_REFLECTION, x, y, MKOBJ_NOINIT);
				}
				place_object(otmp, x, y);
				if(find_sawant()){
					otmp = mksobj(find_sawant(), MKOBJ_NOINIT);
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					place_object(otmp, x, y);
					
					otmp = mksobj(BUCKLER, MKOBJ_NOINIT);
					set_material_gm(otmp, MITHRIL);
					fix_object(otmp);
					place_object(otmp, x, y);
				} else {
					mksobj_at(KHAKKHARA, x, y, MKOBJ_NOINIT);
				}
			break;
			case 2:
			case 1:
				if(find_sawant()){
					otmp = mksobj(find_sawant(), MKOBJ_NOINIT);
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					place_object(otmp, x, y);
					mksobj_at(SHIELD_OF_REFLECTION, x, y, MKOBJ_NOINIT);
					break;
				}
			default:
				mksobj_at(KHAKKHARA, x, y, MKOBJ_NOINIT);
				mksobj_at(AMULET_OF_REFLECTION, x, y, MKOBJ_NOINIT);
			break;
		}
		//Place Kamerel survivors
		c = rnd(4) + rn2(4);
		while(c > 0){
			good = FALSE;
			tries = 0;
			while(!good && tries < 50){
				x=ix+rn2(25)-12;
				y=iy+rn2(25)-12;
				
				tries++;
				okspot = TRUE;
				if(!isok(x,y) || levl[x][y].typ != PUDDLE || m_at(x,y) || t_at(x,y))
					okspot = FALSE;
				if(okspot){
					good = TRUE;
				} else continue;
				makemon(&mons[PM_HUDOR_KAMEREL], x, y, MM_ADJACENTOK);
			}
			c--;
		}
		c = rnd(3) + rn2(3);
		while(c > 0){
			good = FALSE;
			tries = 0;
			while(!good && tries < 50){
				x=ix+rn2(25)-12;
				y=iy+rn2(25)-12;
				
				tries++;
				okspot = TRUE;
				if(!isok(x,y) || levl[x][y].typ != PUDDLE || m_at(x,y) || t_at(x,y))
					okspot = FALSE;
				if(okspot){
					good = TRUE;
				} else continue;
				makemon(&mons[PM_SHARAB_KAMEREL], x, y, MM_ADJACENTOK);
			}
			c--;
		}
	}
}

STATIC_OVL
void
mkfishingvillage()
{
	int x=0,y=0,tx, ty, tries=0;
	int i,j, c, edge;
	int left = rn2(2);
	boolean good=FALSE, okspot;
	int slant = rn2(3);
	int shelf = rn1(5, 5);
	struct obj *otmp;
	if(left){
		//Make sea
		edge = rn1(20, 20);
		for(j = 0; j < ROWNO; j++){
			for(i = 0; i < edge; i++){
				if(isok(x+i,y+j) && 
					(levl[x+i][y+j].typ == TREE 
					|| levl[x+i][y+j].typ == ROOM 
					|| levl[x+i][y+j].typ == GRASS 
					|| levl[x+i][y+j].typ == SOIL 
					|| levl[x+i][y+j].typ == SAND 
					|| levl[x+i][y+j].typ == PUDDLE)
				){
					if(i < edge-shelf){
						levl[x+i][y+j].typ = MOAT;
						if(t_at(x+i, y+j))
							rloc_trap(t_at(x+i, y+j));
						if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			} else
						if(levl[x+i][y+j].typ!=TREE || edge - i > rn2(6)) levl[x+i][y+j].typ = PUDDLE;
					levl[x+i][y+j].lit = 1;
				}
			}
			if(rn2(4)) edge += rn2(3)-slant;
		}
	} else {
		edge = COLNO - rn1(20, 20);
		for(j = 0; j < ROWNO; j++){
			for(i = COLNO; i > edge; i--){
				if(isok(x+i,y+j) && 
					(levl[x+i][y+j].typ == TREE 
					|| levl[x+i][y+j].typ == GRASS 
					|| levl[x+i][y+j].typ == SOIL 
					|| levl[x+i][y+j].typ == SAND 
					|| levl[x+i][y+j].typ == PUDDLE)
				){
					if(i > edge + shelf){
						levl[x+i][y+j].typ = MOAT;
						if(t_at(x+i, y+j))
							rloc_trap(t_at(x+i, y+j));
						if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
					} else
						if(levl[x+i][y+j].typ!=TREE || i - edge > rn2(6)) levl[x+i][y+j].typ = PUDDLE;
					levl[x+i][y+j].lit = 1;
				}
			}
			if(rn2(4)) edge += rn2(3)-slant;
		}
	}
	
	{
	int n = 4 + rnd(4) + rn2(4);
	for(; n > 0; n--)
		mkfishinghut(left);
	}
	mkwell(left);
}

STATIC_OVL
void
mkfishinghut(left)
	int left;
{
	int x,y,tries=0;
	int i,j, pathto = 0;
	boolean good=FALSE, okspot, accessible;
	struct obj *otmp;
	while(!good && tries < 500){
		x = rn2(COLNO/2)+1;
		if(!left)
			x += COLNO/2;
		y = rn2(ROWNO-5);
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=-1;i<6;i++)
			for(j=-1;j<6;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || 
					!(levl[x+i][y+j].typ == TREE 
					|| levl[x+i][y+j].typ == PUDDLE 
					|| levl[x+i][y+j].typ == GRASS 
					|| levl[x+i][y+j].typ == SOIL 
					|| levl[x+i][y+j].typ == SAND 
					|| levl[x+i][y+j].typ == MOAT
					 )
				) okspot = FALSE;
			}
		pathto = 0;
		if(left){
			if(isok(x+1,y-1) && levl[x+1][y-1].typ == PUDDLE) pathto++;
			if(isok(x-1,y+1) && levl[x-1][y+1].typ == PUDDLE) pathto++;
			if(isok(x-1,y+2) && levl[x-1][y+2].typ == PUDDLE) pathto++;
			if(isok(x+1,y+4) && levl[x+1][y+4].typ == PUDDLE) pathto++;
		} else {
			if(isok(x+2,y-1) && levl[x+2][y-1].typ == PUDDLE) pathto++;
			if(isok(x+4,y+1) && levl[x+4][y+1].typ == PUDDLE) pathto++;
			if(isok(x+4,y+2) && levl[x+4][y+2].typ == PUDDLE) pathto++;
			if(isok(x+2,y+4) && levl[x+2][y+4].typ == PUDDLE) pathto++;
		}
		if(pathto) accessible = TRUE;
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				levl[x+i][y+j].typ = HWALL;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
		for(i=1;i<3;i++){
			for(j=1;j<3;j++){
				levl[x+i][y+j].typ = CORR;
				levl[x+i][y+j].lit = 0;
				if(!rn2(9)) mksobj_at(SPEAR, x+1, y+1, NO_MKOBJ_FLAGS);
				if(!rn2(9)){
					otmp = mksobj(SLIME_MOLD, NO_MKOBJ_FLAGS);
					otmp->spe = fruitadd("salted fish");
					otmp->quan = (long) rnd(4);
					otmp->owt = weight(otmp);
					place_object(otmp, x+i, y+j);
				}
				if(!rn2(9)) mksobj_at(POT_BOOZE, x+1, y+1, NO_MKOBJ_FLAGS);
				if(!rn2(9)) mksobj_at(CRAM_RATION, x+1, y+1, NO_MKOBJ_FLAGS);
				if(!rn2(100)){
					otmp = mkobj(AMULET_CLASS, TRUE);
					set_material_gm(otmp, GOLD);
					otmp->owt = weight(otmp);
					place_object(otmp, x+i, y+j);
				}
				if(!rn2(100)){
					otmp = mksobj(find_gcirclet(), NO_MKOBJ_FLAGS);
					set_material_gm(otmp, GOLD);
					otmp->owt = weight(otmp);
					place_object(otmp, x+i, y+j);
				}
			}
		}
		
		for(i = 1+rn2(3);i>0;i--){
			makemon(&mons[PM_DEEP_ONE], x+rnd(2), y+rnd(2), MM_ADJACENTOK);
		}
		
		wallification(x, y, x+3, y+3);
		
		pathto = rn2(pathto);
		if(left){
			if(isok(x+1,y-1) && levl[x+1][y-1].typ == PUDDLE && !(pathto--)) 
				levl[x+1][y].typ = DOOR, levl[x+1][y].doormask = rn2(3) ? D_LOCKED : D_CLOSED;
			if(isok(x-1,y+1) && levl[x-1][y+1].typ == PUDDLE && !(pathto--)) 
				levl[x][y+1].typ = DOOR, levl[x][y+1].doormask = rn2(3) ? D_LOCKED : D_CLOSED;
			if(isok(x-1,y+2) && levl[x-1][y+2].typ == PUDDLE && !(pathto--)) 
				levl[x][y+2].typ = DOOR, levl[x][y+2].doormask = rn2(3) ? D_LOCKED : D_CLOSED;
			if(isok(x+1,y+4) && levl[x+1][y+4].typ == PUDDLE && !(pathto--)) 
				levl[x+1][y+3].typ = DOOR, levl[x+1][y+3].doormask = rn2(3) ? D_LOCKED : D_CLOSED;
		} else {
			if(isok(x+2,y-1) && levl[x+2][y-1].typ == PUDDLE && !(pathto--)) 
				levl[x+2][y].typ = DOOR, levl[x+2][y].doormask = rn2(3) ? D_LOCKED : D_CLOSED;
			if(isok(x+4,y+1) && levl[x+4][y+1].typ == PUDDLE && !(pathto--)) 
				levl[x+3][y+1].typ = DOOR, levl[x+3][y+1].doormask = rn2(3) ? D_LOCKED : D_CLOSED;
			if(isok(x+4,y+2) && levl[x+4][y+2].typ == PUDDLE && !(pathto--)) 
				levl[x+3][y+2].typ = DOOR, levl[x+3][y+2].doormask = rn2(3) ? D_LOCKED : D_CLOSED;
			if(isok(x+2,y+4) && levl[x+2][y+4].typ == PUDDLE && !(pathto--)) 
				levl[x+2][y+3].typ = DOOR, levl[x+2][y+3].doormask = rn2(3) ? D_LOCKED : D_CLOSED;
		}
	}
}

STATIC_OVL
void
mkwell(left)
	int left;
{
	int x,y,tries=0;
	int i,j, pathto = 0;
	boolean good=FALSE, okspot, accessible;
	struct obj *otmp;
	while(!good && tries < 500){
		x = rn2(COLNO/2)+1;
		if(!left)
			x += COLNO/2;
		y = rn2(ROWNO-5);
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=-1;i<2;i++)
			for(j=-1;j<2;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || 
					!(levl[x+i][y+j].typ == TREE 
					|| levl[x+i][y+j].typ == POOL 
					|| levl[x+i][y+j].typ == MOAT 
					|| levl[x+i][y+j].typ == PUDDLE 
					|| levl[x+i][y+j].typ == GRASS
					|| levl[x+i][y+j].typ == SOIL
					|| levl[x+i][y+j].typ == SAND
					)
				) okspot = FALSE;
			}
		pathto = 0;
		if(isok(x,y-2) && (levl[x][y-2].typ == PUDDLE 
							|| levl[x][y-2].typ == SOIL
							|| levl[x][y-2].typ == SAND
							|| levl[x][y-2].typ == GRASS))
			pathto++;
		if(isok(x,y+2) && (levl[x][y+2].typ == PUDDLE 
							|| levl[x][y+2].typ == SOIL
							|| levl[x][y+2].typ == SAND
							|| levl[x][y+2].typ == GRASS))
			pathto++;
		if(isok(x-2,y) && (levl[x-2][y].typ == PUDDLE 
							|| levl[x-2][y].typ == SOIL
							|| levl[x-2][y].typ == SAND
							|| levl[x-2][y].typ == GRASS))
			pathto++;
		if(isok(x+2,y) && (levl[x+2][y].typ == PUDDLE 
							|| levl[x+2][y].typ == SOIL
							|| levl[x+2][y].typ == SAND
							|| levl[x+2][y].typ == GRASS))
			pathto++;
		if(pathto) accessible = TRUE;
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		for(i=-1;i<2;i++){
			for(j=-1;j<2;j++){
				levl[x+i][y+j].typ = CORR;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
				levl[x+i][y+j].lit = 1;
			}
		}
		
		levl[x][y].typ = POOL;
		mksobj_at(RAKUYO, x, y, MKOBJ_ARTIF);
	}
}

STATIC_OVL
void
mkpluhomestead()
{
	int x,y,tries=0;
	int i,j, pathto = 0;
	boolean good=FALSE, okspot, accessible;
	while(!good && tries < 500){
		x = rn2(COLNO-6)+1;
		y = rn2(ROWNO-5);
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=0;i<5;i++)
			for(j=0;j<5;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == TREE))
					okspot = FALSE;
			}
		pathto = 0;
		if(isok(x+2,y-1) && levl[x+2][y-1].typ == GRASS) pathto++;
		if(isok(x+2,y+5) && levl[x+2][y+5].typ == GRASS) pathto++;
		if(isok(x+5,y+2) && levl[x+5][y+2].typ == GRASS) pathto++;
		if(isok(x-1,y+2) && levl[x-1][y+2].typ == GRASS) pathto++;
		if(pathto) accessible = TRUE;
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		for(i=0;i<5;i++){
			for(j=0;j<5;j++){
				levl[x+i][y+j].typ = HWALL;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
		for(i=1;i<4;i++){
			for(j=1;j<4;j++){
				levl[x+i][y+j].typ = CORR;
				if(!rn2(3)) mkobj_at((rn2(2) ? WEAPON_CLASS : rn2(2) ? TOOL_CLASS : ARMOR_CLASS), x+i, y+j, NO_MKOBJ_FLAGS);
			}
		}
		i = rnd(3)+rn2(2);
		for(;i>0;i--){
			makemon(&mons[PM_PLUMACH_RILMANI], x+rnd(3), y+rnd(3), MM_ADJACENTOK);
		}
		
		wallification(x, y, x+4, y+4);
		
		pathto = rn2(pathto);
		if(isok(x+2,y-1) && levl[x+2][y-1].typ == GRASS && !(pathto--))
			levl[x+2][y+0].typ = DOOR, levl[x+2][y+0].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
		if(isok(x+2,y+5) && levl[x+2][y+5].typ == GRASS && !(pathto--))
			levl[x+2][y+4].typ = DOOR, levl[x+2][y+4].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
		if(isok(x+5,y+2) && levl[x+5][y+2].typ == GRASS && !(pathto--))
			levl[x+4][y+2].typ = DOOR, levl[x+4][y+2].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
		if(isok(x-1,y+2) && levl[x-1][y+2].typ == GRASS && !(pathto--))
			levl[x+0][y+2].typ = DOOR, levl[x+0][y+2].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
	}
}

STATIC_OVL
void
mkpluroom(width)
int width;
{
	int x,y,tries=0;
	int i,j, pathto = 0;
	boolean good=FALSE, okspot, accessible;
	if(width < 3){
		//too small to place
		return;
	}
	while(!good && tries < 500){
		x = rn2(COLNO-width-1)+1;
		y = rn2(ROWNO-width-1)+1;
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=0;i<width;i++)
			for(j=0;j<width;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == STONE || IS_WALL(levl[x+i][y+j].typ)))
					okspot = FALSE;
			}
		pathto = 0;
		for(i = 1; i < width-1; i++)
			if(isok(x+i,y-1) && (levl[x+i][y-1].typ == SOIL || levl[x+i][y-1].typ == ROOM)) pathto++;
		for(i = 1; i < width-1; i++)
			if(isok(x+i,y+width) && (levl[x+i][y+width].typ == SOIL || levl[x+i][y+width].typ == ROOM)) pathto++;
		for(i = 1; i < width-1; i++)
			if(isok(x+width,y+i) && (levl[x+width][y+i].typ == SOIL || levl[x+width][y+i].typ == ROOM)) pathto++;
		for(i = 1; i < width-1; i++)
			if(isok(x-1,y+i) && (levl[x-1][y+i].typ == SOIL || levl[x-1][y+i].typ == ROOM)) pathto++;
		if(pathto) accessible = TRUE;
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		for(i=0;i<width;i++){
			for(j=0;j<width;j++){
				levl[x+i][y+j].typ = HWALL;
				levl[x+i][y+j].lit = 1;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
		for(i=1;i<width-1;i++){
			for(j=1;j<width-1;j++){
				levl[x+i][y+j].typ = ROOM;
				if(!rn2(9))
					makemon(&mons[rn2(5) ? PM_PLUMACH_RILMANI : 
								  rn2(5) ? PM_FERRUMACH_RILMANI : 
								  rn2(3) ? PM_CUPRILACH_RILMANI :
								  PM_ARGENACH_RILMANI
								  ], x+i, y+j, 0);
				else if(!rn2(9)) makemon(mkclass(S_GOLEM, G_NOHELL), x+i, y+j, 0);
			}
		}
		
		pathto = rnd(pathto);
		int extra_door = 10;
		for(i = 1; i < width-1; i++){
			if(isok(x+i,y-1) && (levl[x+i][y-1].typ == SOIL || levl[x+i][y-1].typ == ROOM)){
				pathto--;
				if(pathto == 0 || !rn2(extra_door)){
					levl[x+i][y+0].typ = DOOR, levl[x+i][y+0].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					pathto = 0;
					break;
				}
			}
		}
		for(i = 1; i < width-1; i++){
			if(isok(x+i,y+width) && (levl[x+i][y+width].typ == SOIL || levl[x+i][y+width].typ == ROOM)){
				pathto--;
				if(pathto == 0 || !rn2(extra_door)){
					levl[x+i][y+width-1].typ = DOOR, levl[x+i][y+width-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					pathto = 0;
					break;
				}
			}
		}
		for(i = 1; i < width-1; i++){
			if(isok(x+width,y+i) && (levl[x+width][y+i].typ == SOIL || levl[x+width][y+i].typ == ROOM)){
				pathto--;
				if(pathto == 0 || !rn2(extra_door)){
					levl[x+width-1][y+i].typ = DOOR, levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					pathto = 0;
					break;
				}
			}
		}
		for(i = 1; i < width-1; i++){
			if(isok(x-1,y+i) && (levl[x-1][y+i].typ == SOIL || levl[x-1][y+i].typ == ROOM)){
				pathto--;
				if(pathto == 0 || !rn2(extra_door)){
					levl[x+0][y+i].typ = DOOR, levl[x+0][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					pathto = 0;
					break;
				}
			}
		}
		if(width > 3){
			int chance = max(3, 9-width);
			if(!rn2(chance)){
				mkobj_at(0, x+rnd(width-2), y+rnd(width-2), MKOBJ_ARTIF);
			}
			if(!rn2(60)){
				tries = 0;
				while(tries++ < 100){
					i = rnd(width-2);
					j = rnd(width-2);
					if(levl[x+i+1][y+j].typ != DOOR
					&& levl[x+i-1][y+j].typ != DOOR
					&& levl[x+i][y+j+1].typ != DOOR
					&& levl[x+i][y+j-1].typ != DOOR
					){
						levl[x+i][y+j].typ = SINK;
						level.flags.nsinks++;
						break;
					}
				}
			}
			else if(!rn2(60)){
				tries = 0;
				while(tries++ < 100){
					i = rnd(width-2);
					j = rnd(width-2);
					if(levl[x+i+1][y+j].typ != DOOR
					&& levl[x+i-1][y+j].typ != DOOR
					&& levl[x+i][y+j+1].typ != DOOR
					&& levl[x+i][y+j-1].typ != DOOR
					){
						add_altar(x+i, y+j, A_NEUTRAL, FALSE, GOD_NONE);
						break;
					}
				}
			}
		}
		else {
			if(!rn2(9)){
				mkobj_at(0, x+1, y+1, MKOBJ_ARTIF);
			}
		}
	}
}

STATIC_OVL
void
mkfountainsquare(left)
int left;
{
	int x,y,tries=0;
	int i,j;
	boolean good=FALSE, okspot;
	while(!good && tries < 1500){
		if(left){
			x = rn2(COLNO/2);
		}
		else {
			x = rn2(COLNO/2)+COLNO/2;
		}
		y = rn2(ROWNO-3);
		tries++;
		okspot = TRUE;
		for(i=0;i<3;i++)
			for(j=0;j<3;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j)
					|| *in_rooms(x+i, y+j, BARRACKS) || *in_rooms(x+i, y+j, SHOPBASE) || *in_rooms(x+i, y+j, COURT)
					|| !(levl[x+i][y+j].typ == ROOM))
					okspot = FALSE;
			}
		if(okspot){
			good = TRUE;
		} else continue;
		// for(i=0;i<3;i++){
			// for(j=0;j<3;j++){
				// levl[x+i][y+j].typ = CORR;
				// if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			// }
		// }
		levl[x+1][y+1].typ = FOUNTAIN;
		/* Is it a "blessed" fountain? (affects drinking from fountain) */
		if (!rn2(7)) levl[x+1][y+1].blessedftn = 1;
		level.flags.nfountains++;
	}
}

STATIC_OVL
void
mktowntree()
{
	int x,y,tries=0;
	int i,j;
	boolean good=FALSE, okspot;
	while(!good && tries < 1500){
		x = rn2(COLNO-3)+1;
		y = rn2(ROWNO-3);
		tries++;
		okspot = TRUE;
		for(i=0;i<3;i++)
			for(j=0;j<3;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j)
					|| *in_rooms(x+i, y+j, BARRACKS) || *in_rooms(x+i, y+j, SHOPBASE) || *in_rooms(x+i, y+j, COURT)
					|| !(levl[x+i][y+j].typ == ROOM))
					okspot = FALSE;
			}
		if(okspot){
			good = TRUE;
		} else continue;
		levl[x+1][y+1].typ = TREE;
		levl[x+1][y+1].looted = 0;
		if(m_at(x+1, y+1)) rloc(m_at(x+1, y+1), TRUE);
	}
}

STATIC_OVL
void
mksumbuilding(width)
int width;
{
	int x,y,tries=0;
	int i,j, pathto = 0;
	boolean good=FALSE, okspot, accessible;
	if(width < 3){
		//too small to place
		return;
	}
	while(!good && tries < 500){
		x = rn2(COLNO-width-2)+1;
		y = rn2(ROWNO-width-2)+1;
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=-1;i<width+1;i++)
			for(j=-1;j<width+1;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || *in_rooms(x+i, y+j, BARRACKS) || !(levl[x+i][y+j].typ == ROOM))
					okspot = FALSE;
			}
		pathto = 0;
		for(i = 1; i < width-1; i++)
			if(isok(x+i,y-1) && (levl[x+i][y-1].typ == SOIL || levl[x+i][y-1].typ == ROOM)) pathto++;
		for(i = 1; i < width-1; i++)
			if(isok(x+i,y+width) && (levl[x+i][y+width].typ == SOIL || levl[x+i][y+width].typ == ROOM)) pathto++;
		for(i = 1; i < width-1; i++)
			if(isok(x+width,y+i) && (levl[x+width][y+i].typ == SOIL || levl[x+width][y+i].typ == ROOM)) pathto++;
		for(i = 1; i < width-1; i++)
			if(isok(x-1,y+i) && (levl[x-1][y+i].typ == SOIL || levl[x-1][y+i].typ == ROOM)) pathto++;
		if(pathto) accessible = TRUE;
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		for(i=0;i<width;i++){
			for(j=0;j<width;j++){
				levl[x+i][y+j].typ = HWALL;
				levl[x+i][y+j].lit = 1;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
		for(i=1;i<width-1;i++){
			for(j=1;j<width-1;j++){
				levl[x+i][y+j].typ = ROOM;
				if(!rn2(9))
					makemon(&mons[rn2(5) ? PM_PLUMACH_RILMANI : 
								  rn2(5) ? PM_FERRUMACH_RILMANI : 
								  rn2(3) ? PM_CUPRILACH_RILMANI :
								  PM_ARGENACH_RILMANI
								  ], x+i, y+j, 0);
				else if(!rn2(9)) makemon(mkclass(S_GOLEM, G_NOHELL), x+i, y+j, 0);
			}
		}
		if(width > 3){
			int chance = max(3, 9-width);
			if(!rn2(chance)){
				mkobj_at(0, x+rnd(width-1), y+rnd(width-1), MKOBJ_ARTIF);
			}
			if(!rn2(60)){
				tries = 0;
				while(tries++ < 100){
					i = rnd(width-2);
					j = rnd(width-2);
					if(levl[x+i+1][y+j].typ != DOOR
					&& levl[x+i-1][y+j].typ != DOOR
					&& levl[x+i][y+j+1].typ != DOOR
					&& levl[x+i][y+j-1].typ != DOOR
					){
						levl[x+i][y+j].typ = SINK;
						level.flags.nsinks++;
						break;
					}
				}
			}
			else if(!rn2(60)){
				tries = 0;
				while(tries++ < 100){
					i = rnd(width-2);
					j = rnd(width-2);
					if(levl[x+i+1][y+j].typ != DOOR
					&& levl[x+i-1][y+j].typ != DOOR
					&& levl[x+i][y+j+1].typ != DOOR
					&& levl[x+i][y+j-1].typ != DOOR
					){
						add_altar(x+i, y+j, A_NEUTRAL, FALSE, GOD_NONE);
						break;
					}
				}
			}
		}
		else {
			if(!rn2(9)){
				mkobj_at(0, x+1, y+1, MKOBJ_ARTIF);
			}
		}
		
		pathto = rnd(pathto);
		for(i = 1; i < width-1; i++){
			if(isok(x+i,y-1) && (levl[x+i][y-1].typ == SOIL || levl[x+i][y-1].typ == ROOM)){
				pathto--;
				if(pathto == 0){
					levl[x+i][y+0].typ = DOOR, levl[x+i][y+0].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					pathto = 0;
					break;
				}
			}
		}
		for(i = 1; i < width-1; i++){
			if(isok(x+i,y+width) && (levl[x+i][y+width].typ == SOIL || levl[x+i][y+width].typ == ROOM)){
				pathto--;
				if(pathto == 0){
					levl[x+i][y+width-1].typ = DOOR, levl[x+i][y+width-1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					pathto = 0;
					break;
				}
			}
		}
		for(i = 1; i < width-1; i++){
			if(isok(x+width,y+i) && (levl[x+width][y+i].typ == SOIL || levl[x+width][y+i].typ == ROOM)){
				pathto--;
				if(pathto == 0){
					levl[x+width-1][y+i].typ = DOOR, levl[x+width-1][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					pathto = 0;
					break;
				}
			}
		}
		for(i = 1; i < width-1; i++){
			if(isok(x-1,y+i) && (levl[x-1][y+i].typ == SOIL || levl[x-1][y+i].typ == ROOM)){
				pathto--;
				if(pathto == 0){
					levl[x+0][y+i].typ = DOOR, levl[x+0][y+i].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
					pathto = 0;
					break;
				}
			}
		}
	}
}

STATIC_OVL
void
mkelfhut(left)
int left;
{
	int x,y,tries=0;
	int i,j, pathto = 0;
	boolean good=FALSE, okspot, accessible;
	while(!good && tries < 500){
		if(left){
			x = rn2(COLNO/2);
			y = rn2(ROWNO-3);
		} else {
			x = rn2(COLNO-10)+1;
			y = rn2(ROWNO-3);
		}
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=0;i<4;i++)
			for(j=0;j<4;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == TREE || IS_WALL(levl[x+i][y+j].typ)))
					okspot = FALSE;
			}
		pathto = 0;
		if(isok(x+1,y-1) && (levl[x+1][y-1].typ == SOIL || levl[x+1][y-1].typ == ROOM)) pathto++;
		if(isok(x+2,y-1) && (levl[x+2][y-1].typ == SOIL || levl[x+2][y-1].typ == ROOM)) pathto++;
		if(isok(x+1,y+4) && (levl[x+1][y+4].typ == SOIL || levl[x+1][y+4].typ == ROOM)) pathto++;
		if(isok(x+2,y+4) && (levl[x+2][y+4].typ == SOIL || levl[x+2][y+4].typ == ROOM)) pathto++;
		if(isok(x+4,y+1) && (levl[x+4][y+1].typ == SOIL || levl[x+4][y+1].typ == ROOM)) pathto++;
		if(isok(x+4,y+2) && (levl[x+4][y+2].typ == SOIL || levl[x+4][y+2].typ == ROOM)) pathto++;
		if(isok(x-1,y+1) && (levl[x-1][y+1].typ == SOIL || levl[x-1][y+1].typ == ROOM)) pathto++;
		if(isok(x-1,y+2) && (levl[x-1][y+2].typ == SOIL || levl[x-1][y+2].typ == ROOM)) pathto++;
		if(pathto) accessible = TRUE;
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				levl[x+i][y+j].typ = HWALL;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
		for(i=1;i<3;i++){
			for(j=1;j<3;j++){
				levl[x+i][y+j].typ = ROOM;
				// if(!rn2(3)) mkobj_at((rn2(2) ? WEAPON_CLASS : rn2(2) ? TOOL_CLASS : ARMOR_CLASS), x+i, y+j, NO_MKOBJ_FLAGS);
			}
		}
		i = rnd(3);
		for(;i>0;i--){
			makemon(&mons[PM_GREEN_ELF], x+rnd(2), y+rnd(2), MM_ADJACENTOK);
			if(!rn2(3))
				mkobj_at(RANDOM_CLASS, x+rnd(2), y+rnd(2), MKOBJ_ARTIF);
		}
		
		// wallification(x, y, x+3, y+3);//Can be adjacent, do wallification after all huts placed
		
		pathto = rn2(pathto);
		if(isok(x+1,y-1) && (levl[x+1][y-1].typ == SOIL || levl[x+1][y-1].typ == ROOM) && !(pathto--))
			levl[x+1][y+0].typ = DOOR, levl[x+1][y+0].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
		if(isok(x+2,y-1) && (levl[x+2][y-1].typ == SOIL || levl[x+2][y-1].typ == ROOM) && !(pathto--))
			levl[x+2][y+0].typ = DOOR, levl[x+2][y+0].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
		if(isok(x+1,y+4) && (levl[x+1][y+4].typ == SOIL || levl[x+1][y+4].typ == ROOM) && !(pathto--))
			levl[x+1][y+3].typ = DOOR, levl[x+1][y+3].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
		if(isok(x+2,y+4) && (levl[x+2][y+4].typ == SOIL || levl[x+2][y+4].typ == ROOM) && !(pathto--))
			levl[x+2][y+3].typ = DOOR, levl[x+2][y+3].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
		if(isok(x+4,y+1) && (levl[x+4][y+1].typ == SOIL || levl[x+4][y+1].typ == ROOM) && !(pathto--))
			levl[x+3][y+1].typ = DOOR, levl[x+3][y+1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
		if(isok(x+4,y+2) && (levl[x+4][y+2].typ == SOIL || levl[x+4][y+2].typ == ROOM) && !(pathto--))
			levl[x+3][y+2].typ = DOOR, levl[x+3][y+2].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
		if(isok(x-1,y+1) && (levl[x-1][y+1].typ == SOIL || levl[x-1][y+1].typ == ROOM) && !(pathto--))
			levl[x+0][y+1].typ = DOOR, levl[x+0][y+1].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
		if(isok(x-1,y+2) && (levl[x-1][y+2].typ == SOIL || levl[x-1][y+2].typ == ROOM) && !(pathto--))
			levl[x+0][y+2].typ = DOOR, levl[x+0][y+2].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
	}
}

STATIC_OVL
void
mkwraithclearing(right)
int right;
{
	int x,y,tries=0;
	int i,j;
	boolean good=FALSE, okspot, accessible;
	while(!good && tries < 500){
		if(right){
			x = COLNO/2 + rn2(COLNO/2)-2;
			y = rn2(ROWNO-3);
		} else {
			x = rn2(COLNO-3);
			y = rn2(ROWNO-3);
		}
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=0;i<2;i++)
			for(j=0;j<2;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == TREE))
					okspot = FALSE;
			}
		for(i=-1;i<3 && !accessible;i++)
			for(j=-1;j<3 && !accessible;j++){
				if(isok(x+i,y+j) && ZAP_POS(levl[x+i][y+j].typ))
					accessible = TRUE;
			}
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		for(i=0;i<2;i++){
			for(j=0;j<2;j++){
				levl[x+i][y+j].typ = SOIL;
			}
		}
		for(i=-1;i<3;i++)
			for(j=-1;j<3;j++) if(isok(x+i,y+j)){
				if(levl[x+i][y+j].typ == GRASS){
					levl[x+i][y+j].typ = SOIL;
				} else if(levl[x+i][y+j].typ == TREE){
					if(rn2(2)){
						levl[x+i][y+j].typ = SOIL;
					} else {
						levl[x+i][y+j].typ = DEADTREE;
					}
				}
			}
		for(i=-2;i<4;i++)
			for(j=-2;j<4;j++) if(isok(x+i,y+j)){
				if(levl[x+i][y+j].typ == GRASS && rn2(2)){
					levl[x+i][y+j].typ = SOIL;
				} else if(levl[x+i][y+j].typ == TREE && rn2(2)){
					levl[x+i][y+j].typ = DEADTREE;
				}
			}
		if(!rn2(4)){
			rn2(4) ? 
			makemon(mkclass(S_WRAITH, Inhell ? G_HELL : G_NOHELL), x+rn2(2), y+rn2(2), MM_ADJACENTOK) :
			makemon(&mons[PM_SHADE], x+rn2(2), y+rn2(2), MM_ADJACENTOK);
		}
	}
}

STATIC_OVL
void
mkstonepillars()
{
	int x,y,tries=0;
	int i,j;
	boolean good=FALSE, okspot;
	while(!good && tries < 500){
		x = rn2(COLNO-3)+1;
		y = rn2(ROWNO-3);
		tries++;
		okspot = TRUE;
		for(i=0;i<4;i++)
			for(j=0;j<4;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == ROOM || levl[x+i][y+j].typ == LAVAPOOL))
					okspot = FALSE;
			}
		for(i=1;i<3;i++)
			for(j=1;j<3;j++){
				if(m_at(x+i, y+j))
					okspot = FALSE;
			}
		if(okspot){
			good = TRUE;
		} else continue;
		
		for(i=1;i<3;i++){
			for(j=1;j<3;j++){
				if(rn2(3)){
					levl[x+i][y+j].typ = STONE;
					while(level.objects[x+i][y+j])
						rloco(level.objects[x+i][y+j]);
				}
			}
		}
	}
}

void
mkmordorfossil(typ)
int typ;
{
	int x, y, tries = 0, good = FALSE;
	struct obj *otmp;
	while(!good && tries < 500){
		x = rn2(COLNO-4)+3;
		y = rn2(ROWNO-2)+1;
		tries++;
		if(isok(x,y) && levl[x][y].typ == typ)
			good = TRUE;
		else continue;
		
		otmp = mksobj(FOSSIL, NO_MKOBJ_FLAGS);
		switch(rnd(5)){
			case 1:
			case 2:
			otmp->corpsenm = PM_WHITE_DRAGON;
			mksobj_at(WHITE_DRAGON_SCALES, x, y, MKOBJ_NOINIT);
			break;
			case 3:
			case 4:
			otmp->corpsenm = PM_GREEN_DRAGON;
			mksobj_at(GREEN_DRAGON_SCALES, x, y, MKOBJ_NOINIT);
			break;
			case 5:
			switch(rnd(2)){
				case 1:
				otmp->corpsenm = PM_RED_DRAGON;
				mksobj_at(RED_DRAGON_SCALES, x, y, MKOBJ_NOINIT);
				break;
				case 2:
				otmp->corpsenm = PM_BLACK_DRAGON;
				mksobj_at(BLACK_DRAGON_SCALES, x, y, MKOBJ_NOINIT);
				break;
			}
			break;
		}
		fix_object(otmp);
		place_object(otmp, x, y);
		if(typ == ROOM || typ == SOIL || typ == GRASS || typ == SAND || typ == CORR){
			bury_objs(x,y);
		}
	}
}
	
STATIC_OVL
void
mklavapool()
{
	int x,y,ix, iy;
	int i,j, c;
	boolean good=FALSE, okspot;
	struct obj *otmp;
	x = COLNO/2;
	y = ROWNO/2;
		
	ix = x;
	iy = y;
	for(i=-10;i<=10;i++){
		for(j=-10;j<=10;j++){
			if(isok(ix+i,iy+j) && dist2(ix, iy, ix+i, iy+j)<105){
				if((dist2(ix, iy, ix+i, iy+j)) < (rnd(8)*rnd(8)+25) && levl[ix+i][iy+j].typ != STAIRS)
					levl[ix+i][iy+j].typ = ROOM;
				levl[ix+i][iy+j].lit = 1;
				if(t_at(x+i, y+j))
					rloc_trap(t_at(x+i, y+j));
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
	}
	for(i=-7;i<=7;i++){
		for(j=-7;j<=7;j++){
			if(isok(ix+i,iy+j) && dist2(ix, iy, ix+i, iy+j)<105){
				if((dist2(ix, iy, ix+i, iy+j)) < (rnd(6)*rnd(6)+16) && levl[ix+i][iy+j].typ != STAIRS)
					levl[ix+i][iy+j].typ = LAVAPOOL;
				levl[ix+i][iy+j].lit = 1;
				if(t_at(x+i, y+j))
					rloc_trap(t_at(x+i, y+j));
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
	}
}

#define VALAVI_CAMP		0
#define FORMIAN_CAMP1	1
#define FORMIAN_CAMP2	2
#define THRIAE_CAMP		3

STATIC_OVL
void
mkcamp(type)
	int type;
{
	int x,y,tries=0;
	int r = 4;
	int i,j, pathto = 0;
	boolean good=FALSE, okspot, accessible;
	while(!good && tries < 500){
		x = rn2(COLNO-2*r)+1+r;
		y = rn2(ROWNO-2*r)+0+r;
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=-1*(r-1);i<=(r-1);i++){
			for(j=-1*(r-1);j<=(r-1);j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == TREE || levl[x+i][y+j].typ == GRASS))
					okspot = FALSE;
				if(on_level(&u.uz, &arcadia1_level)){
					if(
						((y+j) == 11 && x+i >= 50) ||
						((y+j) >= 11 && x+i == 50)
					) okspot = FALSE;
				} else if((y+j) == 11){
					okspot = FALSE;
				}
			}
		}
		for(i=-1*r;i<=r;i++){
			for(j=-1*r;j<=r;j++){
				if(isok(x+i,y+j) && levl[x+i][y+j].typ == DOOR)
					okspot = FALSE;
			}
		}
		pathto = 0;
		if(isok(x+r,y+0) && levl[x+r][y+0].typ == GRASS) pathto++;
		if(isok(x-r,y+0) && levl[x-r][y+0].typ == GRASS) pathto++;
		if(isok(x+0,y+r) && levl[x+0][y+r].typ == GRASS) pathto++;
		if(isok(x+0,y-r) && levl[x+0][y-r].typ == GRASS) pathto++;
		if(pathto) accessible = TRUE;
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		for(i=-1*(r-1);i<=(r-1);i++){
			for(j=-1*(r-1);j<=(r-1);j++){
				if(dist2(x+i, y+j, x, y) <= r*r+1){
					levl[x+i][y+j].typ = HWALL;
					if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
					while(level.objects[x+i][y+j])
						rloco(level.objects[x+i][y+j]);
				}
			}
		}
		
		for(i=-1*(r-1);i<=(r-1);i++){
			for(j=-1*(r-1);j<=(r-1);j++){
				if(dist2(x+i, y+j, x, y) <= (r-2)*(r-2)+1){
					levl[x+i][y+j].typ = ROOM;
				}
			}
		}
		
		wallification(x-r, y-r, x+r, y+r);
		
		flood_fill_rm(x, y, nroom+ROOMOFFSET, TRUE, TRUE);
		if(type == VALAVI_CAMP)
			add_room(x-r+1, y-r+1, x+r-1, y+r-1, TRUE, CERAMICSHOP, TRUE);
		else if(type == FORMIAN_CAMP1)
			add_room(x-r+1, y-r+1, x+r-1, y+r-1, TRUE, PETSHOP, TRUE);
		else if(type == FORMIAN_CAMP2)
			add_room(x-r+1, y-r+1, x+r-1, y+r-1, TRUE, ACIDSHOP, TRUE);
		else if(type == THRIAE_CAMP)
			add_room(x-r+1, y-r+1, x+r-1, y+r-1, TRUE, JELLYSHOP, TRUE);
		rooms[nroom - 1].irregular = TRUE;
		
		pathto = rn2(pathto);
		if(isok(x+0,y-r) && levl[x+0][y-r].typ == GRASS && !(pathto--)){
			levl[x+0][y-r+1].typ = DOOR, levl[x+0][y-r+1].doormask = D_CLOSED;
			add_door(x+0,y-r+1,&rooms[nroom - 1]);
		} else if(isok(x+0,y+r) && levl[x+0][y+r].typ == GRASS && !(pathto--)){
			levl[x+0][y+r-1].typ = DOOR, levl[x+0][y+r-1].doormask = D_CLOSED;
			add_door(x+0,y+r-1,&rooms[nroom - 1]);
		} else if(isok(x+r,y+0) && levl[x+r][y+0].typ == GRASS && !(pathto--)){
			levl[x+r-1][y+0].typ = DOOR, levl[x+r-1][y+0].doormask = D_CLOSED;
			add_door(x+r-1,y+0,&rooms[nroom - 1]);
		} else if(isok(x-r,y+0) && levl[x-r][y+0].typ == GRASS && !(pathto--)){
			levl[x-r+1][y+0].typ = DOOR, levl[x-r+1][y+0].doormask = D_CLOSED;
			add_door(x-r+1,y+0,&rooms[nroom - 1]);
		}
		fill_room(&rooms[nroom - 1], FALSE);
	}
}

STATIC_OVL
void
mkpluvillage()
{
	int x,y,tries=0, roomtypb;
	int i,j, n,ni;
	int nshacks, sizebig1, sizebig2, sizetot;
	struct obj *otmp;
	struct monst *mon;
	boolean good=FALSE, okspot, accessible, throne = 0;
	struct mkroom *croom;
	while(!good && tries < 50){
		nshacks = rnd(3) + rn2(3);
		// nshacks = rnd(5);
		if(rn2(2)){
			sizebig1 = 1+rnd(3)+2;
			sizebig2 = 2+rnd(3)+2;
		} else {
			sizebig1 = 2+rnd(3)+2;
			sizebig2 = 1+rnd(3)+2;
		}
		sizetot = sizebig1 + nshacks*5 + sizebig2 + 1;
		x = rn2(COLNO-sizetot)+1;
		y = rn2(ROWNO-11);
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=0;i<=sizetot+1;i++)
			for(j=0;j<12;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == GRASS || levl[x+i][y+j].typ == TREE))
					okspot = FALSE;
				if(isok(x+i,y+j) && levl[x+i][y+j].typ == GRASS)
					accessible = TRUE;
			}
		if(okspot && accessible){
			good = TRUE;
			//Clear village green
			for(i=sizebig1;i<(sizetot-sizebig2);i++) 
				for(j=1;j<10;j++){
					levl[x+i][y+j].typ = HWALL;
					if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
				}
			for(i=sizebig1;i<(sizetot-sizebig2);i++) 
				for(j=1;j<10;j++){
					levl[x+i][y+j].typ = GRASS;
					levl[x+i][y+j].lit = 1;
				}
			
			//Make left-hand big building
			roomtypb = nroom;
			
			levl[x+sizebig1][y+4+3].typ = BRCORNER;
			levl[x+sizebig1][y+4+3].lit = 1;
			levl[x][y+4+3].typ = BLCORNER;
			levl[x][y+4+3].lit = 1;
			for(i=1;i<sizebig1;i++){
				levl[x+i][y+4+3].typ = HWALL;
				levl[x+i][y+4+3].lit = 1;
				levl[x+i][y+3].typ = HWALL;
				levl[x+i][y+3].lit = 1;
			}
			for(i=1+3;i<4+3;i++){
				levl[x+sizebig1][y+i].typ = VWALL;
				levl[x+sizebig1][y+i].lit = 1;
				levl[x][y+i].typ = VWALL;
				levl[x][y+i].lit = 1;
			}
			levl[x+sizebig1][y+5].typ = DOOR;
			levl[x+sizebig1][y+5].doormask = rn2(3) ? D_CLOSED : D_LOCKED;

			levl[x+sizebig1][y+3].typ = TRCORNER;
			levl[x+sizebig1][y+3].lit = 1;
			levl[x][y+3].typ = TLCORNER;
			levl[x][y+3].lit = 1;
			
			switch(rn2(7)){
				case 0: //Random store
					for(i=1;i<sizebig1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = ROOM;
							levl[x+i][y+j].lit = 1;
						}
					}
					flood_fill_rm(x+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+1, y+4, x+sizebig1-2, y+6, TRUE, SHOPBASE + rn2(UNIQUESHOP - SHOPBASE), TRUE);
				break;
				case 1: //Temple
					for(i=1;i<sizebig1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = ROOM;
							levl[x+i][y+j].lit = 1;
						}
					}
					flood_fill_rm(x+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+1, y+4, x+sizebig1-2, y+6, TRUE, TEMPLE, TRUE);
					add_altar(x+2, y+5, A_NEUTRAL, TRUE, align_to_god(A_NEUTRAL));
					priestini(&u.uz, &rooms[nroom - 1], x+2, y+5, FALSE);
					level.flags.has_temple = 1;
				break;
				case 2: //Garrison
					for(i=1;i<sizebig1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = ROOM;
							levl[x+i][y+j].lit = 1;
						}
					}
					flood_fill_rm(x+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+1, y+4, x+sizebig1-2, y+6, TRUE, BARRACKS, TRUE);
				break;
				case 3: //Courtroom
					for(i=1;i<sizebig1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = ROOM;
							levl[x+i][y+j].lit = 1;
						}
					}
					flood_fill_rm(x+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					throne = 1;
					add_room(x+1, y+4, x+sizebig1-2, y+6, TRUE, COURT, TRUE);
				break;
				case 4://Normal
					for(i=1;i<sizebig1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = CORR;
							levl[x+i][y+j].lit = 1;
							if(rn2(2)) makemon(&mons[PM_PLUMACH_RILMANI], x+i, y+j, 0);
							if(rn2(2)) mkobj_at((rn2(2) ? WEAPON_CLASS : rn2(2) ? TOOL_CLASS : ARMOR_CLASS), x+i, y+j, NO_MKOBJ_FLAGS);
						}
					}
					flood_fill_rm(x+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+1, y+4, x+sizebig1-2, y+6, TRUE, OROOM, TRUE);
				break;
				case 5: //Gold vault
					for(i=1;i<sizebig1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = CORR;
							levl[x+i][y+j].lit = 1;
							if(rn2(2)) makemon(&mons[PM_GOLD_GOLEM], x+i, y+j, 0);
							mkgold((long) rn1((10+rnd(10)) * level_difficulty(),10), x+i, y+j);
						}
					}
					flood_fill_rm(x+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+1, y+4, x+sizebig1-2, y+6, TRUE, OROOM, TRUE);
				break;
				case 6://Tool shed
					for(i=1;i<sizebig1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = CORR;
							levl[x+i][y+j].lit = 1;
							mkobj_at(TOOL_CLASS, x+i, y+j, NO_MKOBJ_FLAGS);
						}
					}
					flood_fill_rm(x+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+1, y+4, x+sizebig1-2, y+6, TRUE, OROOM, TRUE);
				break;
			}
			add_door(x+sizebig1,y+5,&rooms[roomtypb]);
			fill_room(&rooms[roomtypb], FALSE);
			
			//Make north and south shacks
			for(n = 0; n<nshacks; n++){
				ni = sizebig1 + 2 + n*5;
				//make north shack
				levl[x+ni+3][y+3].typ = BRCORNER;
				levl[x+ni][y+3].typ = BLCORNER;
				for(i=ni+1;i<ni+3;i++){
					levl[x+i][y+3].typ = HWALL;
					levl[x+i][y].typ = HWALL;
				}
				if(rn2(2)){
					levl[x+ni+2][y+3].typ = DOOR;
					levl[x+ni+2][y+3].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				} else {
					levl[x+ni+1][y+3].typ = DOOR;
					levl[x+ni+1][y+3].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				for(i=ni+1;i<ni+3;i++){
					for(j=1;j<3;j++){
						levl[x+i][y+j].typ = CORR;
						levl[x+i][y+j].lit = 1;
						if(rn2(2)) makemon(&mons[PM_PLUMACH_RILMANI], x+i, y+j, 0);
						if(rn2(2)) mkobj_at((rn2(2) ? WEAPON_CLASS : rn2(2) ? TOOL_CLASS : ARMOR_CLASS), x+i, y+j, NO_MKOBJ_FLAGS);
					}
				}
				for(i=1;i<3;i++){
					levl[x+ni+3][y+i].typ = VWALL;
					levl[x+ni][y+i].typ = VWALL;
				}
				levl[x+ni+3][y].typ = TRCORNER;
				levl[x+ni][y].typ = TLCORNER;
				
				//make south shack
				levl[x+ni+3][y+7+3].typ = BRCORNER;
				levl[x+ni][y+7+3].typ = BLCORNER;
				for(i=ni+1;i<ni+3;i++){
					levl[x+i][y+7+3].typ = HWALL;
					levl[x+i][y+7].typ = HWALL;
				}
				if(rn2(2)){
					levl[x+ni+2][y+7].typ = DOOR;
					levl[x+ni+2][y+7].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				} else {
					levl[x+ni+1][y+7].typ = DOOR;
					levl[x+ni+1][y+7].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				for(i=ni+1;i<ni+3;i++){
					for(j=1;j<3;j++){
						levl[x+i][y+7+j].typ = CORR;
						levl[x+i][y+j].lit = 1;
						if(rn2(2)) makemon(&mons[PM_PLUMACH_RILMANI], x+i, y+7+j, 0);
						if(rn2(2)) mkobj_at((rn2(2) ? WEAPON_CLASS : rn2(2) ? TOOL_CLASS : ARMOR_CLASS), x+i, y+7+j, NO_MKOBJ_FLAGS);
					}
				}
				for(i=1;i<3;i++){
					levl[x+ni+3][y+7+i].typ = VWALL;
					levl[x+ni][y+7+i].typ = VWALL;
				}
				levl[x+ni+3][y+7].typ = TRCORNER;
				levl[x+ni][y+7].typ = TLCORNER;
			}
			
			//Make right big building
			roomtypb = nroom;
			
			levl[x+sizetot+1][y+4+3].typ = BRCORNER;
			levl[x+sizetot+1][y+4+3].lit = 1;
			levl[x+sizetot+1-sizebig2][y+4+3].typ = BLCORNER;
			levl[x+sizetot+1-sizebig2][y+4+3].lit = 1;
			for(i=sizetot+1-sizebig2+1;i<sizetot+1;i++){
				levl[x+i][y+4+3].typ = HWALL;
				levl[x+i][y+4+3].lit = 1;
				levl[x+i][y+3].typ = HWALL;
				levl[x+i][y+3].lit = 1;
			}
			for(i=1+3;i<4+3;i++){
				levl[x+sizetot+1][y+i].typ = VWALL;
				levl[x+sizetot+1][y+i].lit = 1;
				levl[x+sizetot+1-sizebig2][y+i].typ = VWALL;
				levl[x+sizetot+1-sizebig2][y+i].lit = 1;
			}
			
			levl[x+sizetot+1-sizebig2][y+5].typ = DOOR;
			levl[x+sizetot+1-sizebig2][y+5].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
			
			levl[x+sizetot+1][y+3].typ = TRCORNER;
			levl[x+sizetot+1][y+3].lit = 1;
			levl[x+sizetot+1-sizebig2][y+3].typ = TLCORNER;
			levl[x+sizetot+1-sizebig2][y+3].lit = 1;		
			
			// switch(rn2(6)){
			switch(rn2(7)){
				case 0: //Shop
					for(i=sizetot+1-sizebig2+1;i<sizetot+1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = ROOM;
							levl[x+i][y+j].lit = 1;
						}
					}
					flood_fill_rm(x+sizetot+1-1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+sizetot+1-sizebig2+1, y+4, x+sizetot+1-1, y+6, TRUE, SHOPBASE + rn2(UNIQUESHOP - SHOPBASE), TRUE);
				break;
				case 1: //Temple
					for(i=sizetot+1-sizebig2+1;i<sizetot+1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = ROOM;
							levl[x+i][y+j].lit = 1;
						}
					}
					flood_fill_rm(x+sizetot+1-sizebig2+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+sizetot+1-sizebig2+1, y+4, x+sizetot+1-1, y+6, TRUE, TEMPLE, TRUE);
					add_altar(x+sizetot+1-2, y+5, A_NEUTRAL, TRUE, align_to_god(A_NEUTRAL));
					priestini(&u.uz, &rooms[nroom - 1], x+sizetot+1-2, y+5, FALSE);
					level.flags.has_temple = 1;
				break;
				case 2: //Garrison
					for(i=sizetot+1-sizebig2+1;i<sizetot+1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = ROOM;
							levl[x+i][y+j].lit = 1;
						}
					}
					flood_fill_rm(x+sizetot+1-sizebig2+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+sizetot+1-sizebig2+1, y+4, x+sizetot+1-1, y+6, TRUE, BARRACKS, TRUE);
				break;
				case 3: //Courtroom
					if(!throne){
						for(i=sizetot+1-sizebig2+1;i<sizetot+1;i++){
							for(j=1+3;j<4+3;j++){
								levl[x+i][y+j].typ = ROOM;
								levl[x+i][y+j].lit = 1;
							}
						}
						flood_fill_rm(x+sizetot+1-sizebig2+1, y+5,
							  nroom+ROOMOFFSET, TRUE, TRUE);
						add_room(x+sizetot+1-sizebig2+1, y+4, x+sizetot+1-1, y+6, TRUE, COURT, TRUE);
				break;
					}
				case 4: //Normal
					for(i=sizetot+1-sizebig2+1;i<sizetot+1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = CORR;
							levl[x+i][y+j].lit = 1;
							if(rn2(2)) makemon(&mons[PM_PLUMACH_RILMANI], x+i, y+j, 0);
							if(rn2(2)) mkobj_at((rn2(2) ? WEAPON_CLASS : rn2(2) ? TOOL_CLASS : ARMOR_CLASS), x+i, y+j, NO_MKOBJ_FLAGS);
						}
					}
					flood_fill_rm(x+sizetot+1-sizebig2+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+sizetot+1-sizebig2+1, y+4, x+sizetot+1-1, y+6, TRUE, OROOM, TRUE);
				break;
				case 5: //Gold vault
					for(i=sizetot+1-sizebig2+1;i<sizetot+1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = CORR;
							levl[x+i][y+j].lit = 1;
							if(rn2(2)) makemon(&mons[PM_GOLD_GOLEM], x+i, y+j, 0);
							mkgold((long) rn1((10+rnd(10)) * level_difficulty(),10), x+i, y+j);
						}
					}
					flood_fill_rm(x+sizetot+1-sizebig2+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+sizetot+1-sizebig2+1, y+4, x+sizetot+1-1, y+6, TRUE, OROOM, TRUE);
				break;
				case 6: //Tool shed
					for(i=sizetot+1-sizebig2+1;i<sizetot+1;i++){
						for(j=1+3;j<4+3;j++){
							levl[x+i][y+j].typ = CORR;
							levl[x+i][y+j].lit = 1;
							mkobj_at(TOOL_CLASS, x+i, y+j, NO_MKOBJ_FLAGS);
						}
					}
					flood_fill_rm(x+sizetot+1-sizebig2+1, y+5,
						  nroom+ROOMOFFSET, TRUE, TRUE);
					add_room(x+sizetot+1-sizebig2+1, y+4, x+sizetot+1-1, y+6, TRUE, OROOM, TRUE);
				break;
			}
			add_door(x+sizetot+1-sizebig2,y+5,&rooms[roomtypb]);
			fill_room(&rooms[roomtypb], FALSE);
		}
	}
}

#define INSIDE_CONNECTED	1
#define LINKED_RIGHT		2
#define LINKED_LEFT			4
#define LINKED_ROOMED		8
#define OUTSIDE_DOOR		16
#define INSIDE_DOOR			32

struct roomcon{
	int status;
	struct roomcon *left;
	struct roomcon *right;
};

STATIC_OVL
void
connectRight(curroom)
	struct roomcon *curroom;
{
	curroom->status |= LINKED_RIGHT;
	curroom->right->status |= LINKED_LEFT;
	if((curroom->right->status&INSIDE_CONNECTED) != 0){
		curroom->status |= INSIDE_CONNECTED;
		if(curroom->status&LINKED_LEFT){
			curroom = curroom->left;
			while((curroom->status&INSIDE_CONNECTED) == 0){
				curroom->status |= INSIDE_CONNECTED;
				if(!(curroom->status&LINKED_LEFT))
					break;
				else curroom = curroom->left;
			}
		}
	}
}

STATIC_OVL
void
connectLeft(curroom)
	struct roomcon *curroom;
{
	curroom->status |= LINKED_LEFT;
	curroom->left->status |= LINKED_RIGHT;
	if((curroom->left->status&INSIDE_CONNECTED) != 0){
		curroom->status |= INSIDE_CONNECTED;
		if(curroom->status&LINKED_RIGHT){
			curroom = curroom->right;
			while((curroom->status&INSIDE_CONNECTED) == 0){
				curroom->status |= INSIDE_CONNECTED;
				if(!(curroom->status&LINKED_RIGHT))
					break;
				else curroom = curroom->right;
			}
		}
	}
}

STATIC_OVL
void
connectInner(inroom)
	struct roomcon *inroom;
{
	struct roomcon *curroom = inroom;
	curroom->status |= INSIDE_CONNECTED;
	curroom->status |= INSIDE_DOOR;
	if(curroom->status&LINKED_RIGHT){
		curroom = curroom->right;
		while((curroom->status&INSIDE_CONNECTED) == 0){
			curroom->status |= INSIDE_CONNECTED;
			if(!(curroom->status&LINKED_RIGHT))
				break;
			else curroom = curroom->right;
		}
	}
	
	curroom = inroom;
	if(curroom->status&LINKED_LEFT){
		curroom = curroom->left;
		while((curroom->status&INSIDE_CONNECTED) == 0){
			curroom->status |= INSIDE_CONNECTED;
			if(!(curroom->status&LINKED_LEFT))
				break;
			else curroom = curroom->left;
		}
	}
}

STATIC_OVL
void
mkferrufort()
{
	int wallln = rn2(2) ? 4 : 3;
	int x=0,y=0,tx, ty, tries=0;
	int i,j, c;
	int wallrange = wallln*4+1;
	int allrange = wallrange+2;
	boolean good=FALSE, okspot, accessible, throne = 0;
	
	struct roomcon connect[wallln][wallln];
		
	for(i = 0; i < wallln; i++)
		for(j = 0; j < wallln; j++){
			connect[i][j].status = 0;
			connect[i][j].left = 0;
			connect[i][j].right = 0;
		}
	for(i = 1; i < wallln-1; i++)
		for(j = 1; j < wallln-1; j++){
			connect[i][j].status = ~0;
		}
	i = 0;
	j = 0;
	
	connect[0][0].left = &connect[0][1];
	connect[0][0].right= &connect[1][0];
	
	for(i=1; i < wallln - 1; i++){
		connect[i][j].left = &connect[i-1][j];
		connect[i][j].right= &connect[i+1][j];
	}
	
	connect[i][j].left = &connect[i-1][j];
	connect[i][j].right= &connect[i][j+1];
	
	for(j=1; j < wallln-1; j++){
		connect[i][j].left = &connect[i][j-1];
		connect[i][j].right= &connect[i][j+1];
	}
	
	connect[i][j].left = &connect[i][j-1];
	connect[i][j].right= &connect[i-1][j];
	
	for(i=wallln - 1; i > 0; i--){
		connect[i][j].left = &connect[i+1][j];
		connect[i][j].right= &connect[i-1][j];
	}
	
	connect[i][j].left = &connect[i+1][j];
	connect[i][j].right= &connect[i][j-1];
	
	for(j=wallln - 1; j > 0; j--){
		connect[i][j].left = &connect[i][j+1];
		connect[i][j].right= &connect[i][j-1];
	}
	
	while(tries < 500){
		x = rn2(COLNO-allrange)+1;
		y = rn2(ROWNO-allrange);
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=0;i<allrange;i++)
			for(j=0;j<allrange;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == GRASS || levl[x+i][y+j].typ == TREE))
					okspot = FALSE;
				if(isok(x+i,y+j) && levl[x+i][y+j].typ == GRASS //&&
//					(i == 0 || j == 0 || i+1 == allrange || j+1 == allrange)
				)
					accessible = TRUE;
			}
			
		if(okspot && accessible){
			//clear around the fort and make a solid block of walls
			for(i=0;i<allrange;i++)
				for(j=0;j<allrange;j++){
					if(i>0 && i+1 < allrange && j>0 && j+1<allrange){
						levl[x+i][y+j].typ = HWALL;
						levl[x+i][y+j].lit = 1;
						if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
					}
					else
						levl[x+i][y+j].typ = ROOM;
				}
			//Update coordinates: there is a perimeter of clear terain now, that's all we need.
			//Use wallrange for all purposes now.
			x++;y++;
			
			//carve the block into grid of 3x3 rooms
			for(tx=0;tx<wallln;tx++)
				for(ty=0;ty<wallln;ty++){
					for(i=0;i<3;i++)
						for(j=0;j<3;j++)
							levl[tx*4+x+i+1][ty*4+y+j+1].typ = CORR;
				}
			
			//replace the center rooms with a courtyard
			for(i=5;i<wallrange-5;i++)
				for(j=5;j<wallrange-5;j++)
					levl[x+i][y+j].typ = GRASS;
			
			//Make the walls look pretty
			wallification(x, y, x+wallrange-1, y+wallrange-1);
			
			//place outer door
			if(rn2(2)){
				//generate x and y of *room*
				int doorx = rn2(wallln);
				int doory = rn2(2) ? (wallln-1) : 0;
				connect[doorx][doory].status = OUTSIDE_DOOR;
				
				//generate true x and y
				tx = x+1+doorx*4+1;
				if(doory)
					ty = y+wallln*4;
				else 
					ty = y;
				levl[tx][ty].typ = DOOR;
				levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
			} else {
				int doorx = rn2(2) ? (wallln-1) : 0;
				int doory = rn2(wallln);
				connect[doorx][doory].status = OUTSIDE_DOOR;
				
				//generate true x and y
				if(doorx)
					tx = x+wallln*4;
				else 
					tx = x;
				ty = y+1+doory*4+1;
				
				levl[tx][ty].typ = DOOR;
				levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
			}
			// c = TRUE;
			// while(c){
				// if((doorx != 0 && doory != wallln-1) || (doory != 0 && doory != wallln-1) ){
				// }
			// }
			{
			int roomsleft;
			int roomn;
			int tries = 5000;
			do{
				roomsleft = 0;
				for(i = 0; i < wallln; i++)
					for(j = 0; j < wallln; j++){
						if((connect[i][j].status&INSIDE_CONNECTED) == 0)
							roomsleft++;
					}
				
				if(!roomsleft)
					break;
				
				roomn = rn2(roomsleft);
				
				for(i = 0; i < wallln; i++){
					for(j = 0; j < wallln; j++){
						if((connect[i][j].status&INSIDE_CONNECTED) == 0 && roomn-- <=0 )
							goto nestbreak;
					}
				}
nestbreak:
				if((i == 0 || i == (wallln-1)) && (j == 0 || j == (wallln-1))){
					if(rn2(2)){
						connectRight(&(connect[i][j]));
					} else {
						connectLeft(&(connect[i][j]));
					}
				} else {
					if(!rn2(3)){
						connectInner(&(connect[i][j]));
					} else if(rn2(2)){
						connectRight(&(connect[i][j]));
					} else {
						connectLeft(&(connect[i][j]));
					}
				}
			} while(roomsleft && tries-- > 0);
			// curroom->status = 0
			// curroom->left = 0;
			// curroom->right = 0;
			i = 0;
			j = 0;
			//down
			if(connect[0][0].status&LINKED_LEFT){
				//generate true x and y
				tx = x+i*4+2;
				ty = y+j*4+4;
				levl[tx][ty].typ = DOOR;
				levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
			}
			
			for(i=1; i < wallln - 1; i++){
				//left
				if(connect[i][j].status&LINKED_LEFT){
					//generate true x and y
					tx = x+i*4;
					ty = y+j*4+2;
					levl[tx][ty].typ = DOOR;
					levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				//down
				if(connect[i][j].status&INSIDE_DOOR){
					//generate true x and y
					tx = x+i*4+2;
					ty = y+j*4+4;
					levl[tx][ty].typ = DOOR;
					levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
			}
			
			// i = max;
			// j = 0;
			//left
			if(connect[i][j].status&LINKED_LEFT){
				//generate true x and y
				tx = x+i*4;
				ty = y+j*4+2;
				levl[tx][ty].typ = DOOR;
				levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
			}
			
			for(j=1; j < wallln-1; j++){
				//up
				if(connect[i][j].status&LINKED_LEFT){
					//generate true x and y
					tx = x+i*4+2;
					ty = y+j*4;
					levl[tx][ty].typ = DOOR;
					levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				//left
				if(connect[i][j].status&INSIDE_DOOR){
					//generate true x and y
					tx = x+i*4;
					ty = y+j*4+2;
					levl[tx][ty].typ = DOOR;
					levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
			}
			
			// i = max;
			// j = max;
			//up
			if(connect[i][j].status&LINKED_LEFT){
				//generate true x and y
				tx = x+i*4+2;
				ty = y+j*4;
				levl[tx][ty].typ = DOOR;
				levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
			}
			
			for(i=wallln - 2; i > 0; i--){
				//right
				if(connect[i][j].status&LINKED_LEFT){
					//generate true x and y
					tx = x+i*4+4;
					ty = y+j*4+2;
					levl[tx][ty].typ = DOOR;
					levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				//up
				if(connect[i][j].status&INSIDE_DOOR){
					//generate true x and y
					tx = x+i*4+2;
					ty = y+j*4;
					levl[tx][ty].typ = DOOR;
					levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
			}
			
			// i = 0;
			// j = max;
			//right
			if(connect[i][j].status&LINKED_LEFT){
				//generate true x and y
				tx = x+i*4+4;
				ty = y+j*4+2;
				levl[tx][ty].typ = DOOR;
				levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
			}
			
			for(j=wallln - 2; j > 0; j--){
				//down
				if(connect[i][j].status&LINKED_LEFT){
					//generate true x and y
					tx = x+i*4+2;
					ty = y+j*4+4;
					levl[tx][ty].typ = DOOR;
					levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
				//right
				if(connect[i][j].status&INSIDE_DOOR){
					//generate true x and y
					tx = x+i*4+4;
					ty = y+j*4+2;
					levl[tx][ty].typ = DOOR;
					levl[tx][ty].doormask = rn2(3) ? D_CLOSED : D_LOCKED;
				}
			}
			
			// i = 0;
			// j = 0;
			
			}
			break;
		}
	}
}

STATIC_OVL
void
mkferrutower()
{
	int x,y,tries=0, roomtypb = nroom;
	int i,j;
	boolean good=FALSE, okspot, accessible;
	int size = 8;
	if(!rn2(20))
		size += rnd(4);
	while(!good && tries < 500){
		x = rn2(COLNO-size)+1;
		y = rn2(ROWNO-size);
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=0;i<size;i++)
			for(j=0;j<size;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == TREE || levl[x+i][y+j].typ == GRASS))
					okspot = FALSE;
			}
		if(!okspot)
			continue;
		
		for(i=0;i<size;i++){
			if(isok(x+i,y) && levl[x+i][y].typ == GRASS)
				accessible = TRUE;
			if(isok(x+i,y+size-1) && levl[x+i][y+size-1].typ == GRASS)
				accessible = TRUE;
			if(isok(x+size-1,y+i) && levl[x+size-1][y+i].typ == GRASS)
				accessible = TRUE;
			if(isok(x,y+i) && levl[x][y+i].typ == GRASS)
				accessible = TRUE;
		}
		
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				levl[x+i][y+j].typ = ROOM;
			}
		}
		for(i=1;i<size-1;i++){
			for(j=1;j<size-1;j++){
				levl[x+i][y+j].typ = HWALL;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
		for(i=2;i<size-2;i++){
			for(j=2;j<size-2;j++){
				levl[x+i][y+j].typ = ROOM;
			}
		}
		
		wallification(x, y, x+size-1, y+size-1);
		
		if(rn2(2)){
			i = rnd(size-4)+1;
			j = rn2(2) ? size-2 : 1;
		} else {
			i = rn2(2) ? size-2 : 1;
			j = rnd(size-4)+1;
		}
		levl[x+i][y+j].typ = DOOR;
		levl[x+i][y+j].doormask = D_LOCKED;
		
		flood_fill_rm(x+size/2, y+size/2,
			  nroom+ROOMOFFSET, TRUE, TRUE);
		add_room(x+2, y+2, x+size-3, y+size-3, TRUE, BARRACKS, TRUE);
		add_door(x+i,y+j,&rooms[roomtypb]);
		fill_room(&rooms[roomtypb], FALSE);
	}
}

STATIC_OVL
void
mkferrubarracks()
{
	int x,y,tries=0, roomtypb = nroom;
	int i,j;
	boolean good=FALSE, okspot, accessible;
	int size;
	while(!good && tries < 1500){
		size = 6+rnd(2)+rn2(2);
		x = rn2(COLNO-size-4)+2;
		y = rn2(ROWNO-size-4)+2;
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		if(!rn2(3)){
			for(i=0;i<size;i++)
				for(j=0;j<size;j++){
					if(!isok(x+i,y+j) || t_at(x+i, y+j) || 
						!(IS_WALL(levl[x+i][y+j].typ) || levl[x+i][y+j].typ == STONE ||
						 ((i==0 || i == size-1 || j==0 || j == size-1) && levl[x+i][y+j].typ == ROOM))
					)
						okspot = FALSE;
				}
		}
		else {
			for(i=0;i<size;i++)
				for(j=0;j<size;j++){
					if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == ROOM))
						okspot = FALSE;
				}
		}
		if(!okspot)
			continue;
		
		for(i=-1;i<size+1;i++)
			for(j=-1;j<size+1;j++){
				if(isok(x+i,y+j) && levl[x+i][y+j].typ == ROOM)
					accessible = TRUE;
			}
		
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				levl[x+i][y+j].typ = ROOM;
				levl[x+i][y+j].lit = 1;
			}
		}
		for(i=1;i<size-1;i++){
			for(j=1;j<size-1;j++){
				levl[x+i][y+j].typ = HWALL;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
		for(i=2;i<size-2;i++){
			for(j=2;j<size-2;j++){
				levl[x+i][y+j].typ = ROOM;
			}
		}
		
		wallification(x, y, x+size-1, y+size-1);
		
		if(rn2(2)){
			i = rnd(size-4)+1;
			j = rn2(2) ? size-2 : 1;
		} else {
			i = rn2(2) ? size-2 : 1;
			j = rnd(size-4)+1;
		}
		levl[x+i][y+j].typ = DOOR;
		levl[x+i][y+j].doormask = D_LOCKED;
		
		flood_fill_rm(x+size/2, y+size/2,
			  nroom+ROOMOFFSET, TRUE, TRUE);
		add_room(x+2, y+2, x+size-3, y+size-3, TRUE, !rn2(3) ? ARMORSHOP : rn2(2) ? POTIONSHOP : rn2(2) ? BARRACKS : COURT, TRUE);
		add_door(x+i,y+j,&rooms[roomtypb]);
		fill_room(&rooms[roomtypb], FALSE);
	}
}

char timaresh_template_large[12][12] = 
{{'v','v','.','.','.','.','.','.','.','.','v','v'},
 {'v','.','.','-','-','-','-','-','-','.','.','v'},
 {'.','.','-','-','.','.','.','.','-','-','.','.'},
 {'.','-','-','.','-','-','-','-','.','-','-','.'},
 {'.','-','.','-','-','.','.','-','-','.','-','.'},
 {'.','-','.','-','.','.','.','.','-','.','-','.'},
 {'.','-','.','-','.','.','.','.','-','.','-','.'},
 {'.','-','.','.','-','.','.','-','-','.','-','.'},
 {'.','-','-','-','-','-','-','-','.','-','-','.'},
 {'.','.','+','.','.','.','.','.','-','-','.','.'},
 {'v','.','-','-','-','-','-','-','-','.','.','v'},
 {'v','.','.','.','.','.','.','.','.','.','v','v'}};

char timaresh_template_medium[12][12] = 
{{'v','v','.','.','.','.','.','.','.','v','v'},
 {'v','.','.','-','-','-','-','-','.','.','v'},
 {'.','.','-','-','.','.','.','-','-','.','.'},
 {'.','-','-','.','-','-','-','.','-','-','.'},
 {'.','-','.','-','-','.','-','-','.','-','.'},
 {'.','-','.','-','.','.','.','-','.','-','.'},
 {'.','-','.','.','-','.','-','-','.','-','.'},
 {'.','-','-','-','-','-','-','.','-','-','.'},
 {'.','.','+','.','.','.','.','-','-','.','.'},
 {'v','.','-','-','-','-','-','-','.','.','v'},
 {'v','.','.','.','.','.','.','.','.','v','v'}};

char timaresh_template_small[12][12] = 
{{'v','.','.','.','.','.','.','.','v'},
 {'.','.','-','-','-','-','-','.','.'},
 {'.','-','-','.','.','.','-','-','.'},
 {'.','-','.','-','-','-','.','-','.'},
 {'.','-','.','.','.','-','.','-','.'},
 {'.','-','-','-','-','-','.','-','.'},
 {'.','.','+','.','.','.','-','-','.'},
 {'v','.','-','-','-','-','-','.','.'},
 {'v','.','.','.','.','.','.','.','v'}};

STATIC_OVL
void
mktimaresh()
{
	int x,y,tries=0;
	int i,j;
	boolean good=FALSE, okspot, accessible;
	int size = rn2(2) ? 9 : rn2(2) ? 11 : 12;
	while(!good && tries < 1500){
		if(rn2(2)){
			x = rn2(COLNO/4)+2;
		}
		else {
			x = rn2(COLNO/4) + 3*COLNO/4-size-2;
		}
		y = rn2(ROWNO-size-4)+2;
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		if(!rn2(3)){
			for(i=0;i<size;i++)
				for(j=0;j<size;j++){
					if(!isok(x+i,y+j) || t_at(x+i, y+j) || 
					   !(IS_WALL(levl[x+i][y+j].typ) || levl[x+i][y+j].typ == STONE || levl[x+i][y+j].typ == ROOM)
					)
						okspot = FALSE;
				}
		}
		else {
			for(i=0;i<size;i++)
				for(j=0;j<size;j++){
					if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == ROOM))
						okspot = FALSE;
				}
		}
		if(!okspot)
			continue;
		
		for(i=-1;i<size+1;i++)
			for(j=-1;j<size+1;j++){
				if(isok(x+i,y+j) && levl[x+i][y+j].typ == ROOM)
					accessible = TRUE;
			}
		
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		char (*timaresh_template)[12][12];
		switch(size){
			case 9:
				timaresh_template = &timaresh_template_small;
			break;
			case 11:
				timaresh_template = &timaresh_template_medium;
			break;
			case 12:
				timaresh_template = &timaresh_template_large;
			break;
			default:
			impossible("Bad timaresh size (%d)!", size);
			return;
		}
		int typ;
		struct obj *otmp;
		struct monst *mtmp;
		char template_val;
		char upsidedown = rn2(2);
		int tempx, tempy;
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				levl[x+i][y+j].lit = 1;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
				if(x < COLNO/2)
					tempx = (size-1)-i;
				else
					tempx = i;

				if(upsidedown)
					tempy = (size-1)-j;
				else
					tempy = j;
				template_val = (*timaresh_template)[tempy][tempx];
				
				switch(template_val){
					default:
						impossible("Unknown template symbol %c?", template_val);
					break;
					case '.':
						levl[x+i][y+j].typ = ROOM;
						if(i == size/2 && j == size/2){
							mtmp = makemon(&mons[PM_AURUMACH_RILMANI], x+i, y+j, MM_ADJACENTOK);
							if (mtmp){
								mtmp = christen_monst(mtmp, "Zenodach, Librarian of Timaresh"); //With apologies to Zenodotus
								if(mtmp->mpeaceful){
									mtmp->mpeaceful = FALSE;
									set_malign(mtmp);
								}
								set_template(mtmp, FRACTURED);
								mtmp->m_lev += 8;
								mtmp->mhpmax += 8*8;
								mtmp->mhp = mtmp->mhpmax;
							}
						}
						else if(i > 1 && i < size-2 && j > 1 && j < size-2){
							if(rn2(2)){
								mtmp = makemon(&mons[PM_AMM_KAMEREL], x+i, y+j, MM_ADJACENTOK);
								if (mtmp){
									if(!has_template(mtmp, FRACTURED))
										set_template(mtmp, FRACTURED);
									mtmp->m_lev += 4;
									mtmp->mhpmax += 4*8;
									mtmp->mhp = mtmp->mhpmax;
								}
							}
							if(rn2(2)){
								mkobj_at(rn2(5) ? SCROLL_CLASS : SPBOOK_CLASS, x+i, y+j, NO_MKOBJ_FLAGS);
							}
						}
					break;
					case '-':
						levl[x+i][y+j].typ = HWALL;
						otmp = mksobj(MIRROR, MKOBJ_NOINIT);
						otmp->objsize = MZ_GIGANTIC;
						fix_object(otmp);
						place_object(otmp, x+i, y+j);
					break;
					case '+':
						levl[x+i][y+j].typ = DOOR;
						levl[x+i][y+j].doormask = D_LOCKED|D_TRAPPED;
					break;
					case 'v':
						//"variable": do nothing
					break;
				}
				if(i == size/2 && j == size/2){
					switch(rn2(4)){
						case 0:
							if ((otmp = mksobj_at(MISOTHEISTIC_FRAGMENT, x+i, y+j, MKOBJ_NOINIT)) != 0) {
								otmp->quan = rnd(3);
								otmp->owt = weight(otmp);
							}
						break;
						case 1:
							if ((otmp = mksobj_at(ROD_OF_FORCE, x+i, y+j, MKOBJ_NOINIT)) != 0) {
								set_material_gm(otmp, rn2(3) ? GLASS : SILVER);
							}
						break;
						case 2:
							if((otmp = mksobj_at(ANTIMAGIC_RIFT, x+i, y+j, MKOBJ_NOINIT)) != 0){
								otmp->quan = rnd(3);
								otmp->owt = weight(otmp);
							}
						break;
						case 3:
							if((otmp = mksobj_at(CATAPSI_VORTEX, x+i, y+j, MKOBJ_NOINIT)) != 0){
								otmp->quan = rnd(3);
								otmp->owt = weight(otmp);
							}
						break;
					}
				}
			}
		}
		
		wallification(x, y, x+size-1, y+size-1);
	}
}

STATIC_OVL
void
mkinvertzigg()
{
	int x,y,tries=0;
	int i,j;
	boolean good=FALSE, okspot, accessible;
	int size = 15;
	struct obj *chest, *otmp;
	while(!good && tries < 500){
		x = rn2(COLNO-size)+1;
		y = rn2(ROWNO-size);
		tries++;
		okspot = TRUE;
		accessible = FALSE;
		for(i=0;i<size;i++)
			for(j=0;j<size;j++){
				if(!isok(x+i,y+j) || t_at(x+i, y+j) || !(levl[x+i][y+j].typ == TREE || levl[x+i][y+j].typ == GRASS))
					okspot = FALSE;
			}
		if(!okspot)
			continue;
		
		for(i=0;i<size;i++){
			if(isok(x+i,y) && levl[x+i][y].typ == GRASS)
				accessible = TRUE;
			if(isok(x+i,y+size-1) && levl[x+i][y+size-1].typ == GRASS)
				accessible = TRUE;
			if(isok(x+size-1,y+i) && levl[x+size-1][y+i].typ == GRASS)
				accessible = TRUE;
			if(isok(x,y+i) && levl[x][y+i].typ == GRASS)
				accessible = TRUE;
		}
		
		if(okspot && accessible){
			good = TRUE;
		} else continue;
		
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				levl[x+i][y+j].typ = GRASS;
				levl[x+i][y+j].lit = 1;
			}
		}
		
		for(i=1;i<size-1;i++){
			for(j=1;j<size-1;j++){
				levl[x+i][y+j].typ = HWALL;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
		for(i=2;i<size-2;i++){
			for(j=2;j<size-2;j++){
				levl[x+i][y+j].typ = CORR;
				levl[x+i][y+j].lit = 0;
			}
		}
		
		for(i=4;i<size-4;i++){
			for(j=4;j<size-4;j++){
				levl[x+i][y+j].typ = HWALL;
				if(m_at(x+i, y+j)) rloc(m_at(x+i, y+j), TRUE);
			}
		}
		for(i=5;i<size-5;i++){
			for(j=5;j<size-5;j++){
				levl[x+i][y+j].typ = ROOM;
			}
		}
		
		wallification(x, y, x+size-1, y+size-1);
		
		if(rn2(2)){
			i = rnd(size-4)+1;
			j = rn2(2) ? size-2 : 1;
		} else {
			i = rn2(2) ? size-2 : 1;
			j = rnd(size-4)+1;
		}
		levl[x+i][y+j].typ = DOOR;
		levl[x+i][y+j].doormask = D_LOCKED;
		
		if(rn2(2)){
			i = rnd(size-10)+4;
			j = rn2(2) ? size-5 : 4;
		} else {
			i = rn2(2) ? size-5 : 4;
			j = rnd(size-10)+4;
		}
		levl[x+i][y+j].typ = DOOR;
		levl[x+i][y+j].doormask = D_LOCKED;
		
		if(rn2(3)){
			chest = mksobj_at(CHEST, x+size/2, y+size/2, MKOBJ_ARTIF);
			set_material_gm(chest, IRON);
			fix_object(chest);
			if ((otmp = mksobj(TORCH, 0)) != 0) {
				otmp->quan = rnd(6);
				otmp->owt = weight(otmp);
				add_to_container(chest, otmp);
			}
			if ((otmp = mksobj(SHADOWLANDER_S_TORCH, 0)) != 0) {
				otmp->quan = rnd(6);
				otmp->spe = rn2(3);
				otmp->owt = weight(otmp);
				add_to_container(chest, otmp);
			}
			/*Chance for beads of force*/
			/*Rod of force*/
			if ((otmp = mksobj(ROD_OF_FORCE, 0)) != 0) {
				add_to_container(chest, otmp);
			}
			/*wand of striking*/
			if ((otmp = mksobj(WAN_STRIKING, 0)) != 0) {
				add_to_container(chest, otmp);
			}
			if ((otmp = mksobj(POT_WATER, MKOBJ_NOINIT)) != 0) {
				otmp->blessed = 0;
				otmp->cursed = 1;
				otmp->quan = rnd(3);
				otmp->owt = weight(otmp);
				add_to_container(chest, otmp);
			}
			bury_an_obj(chest);
		} else {
			add_altar(x+size/2, y+size/2, A_NONE, FALSE, GOD_THE_DREAD_FRACTURE);

			if ((otmp = mksobj_at(MISOTHEISTIC_FRAGMENT, x+size/2, y+size/2, MKOBJ_NOINIT)) != 0) {
				otmp->quan = rnd(3);
				otmp->owt = weight(otmp);
			}
			if(!rn2(4) && (otmp = mksobj_at(ANTIMAGIC_RIFT, x+size/2, y+size/2, MKOBJ_NOINIT)) != 0){
				otmp->quan = rnd(3);
				otmp->owt = weight(otmp);
			}
			else if(!rn2(3) && (otmp = mksobj_at(CATAPSI_VORTEX, x+size/2, y+size/2, MKOBJ_NOINIT)) != 0){
				otmp->quan = rnd(3);
				otmp->owt = weight(otmp);
			}
			else if(!rn2(2)){
				int black_gems[] = {BLACK_OPAL, JET, OBSIDIAN};
				for(int i = rn2(SIZE(black_gems)); i >= 0; i--){
					if ((otmp = mksobj_at(black_gems[i], x+size/2, y+size/2, MKOBJ_NOINIT)) != 0) {
						otmp->quan = rnd(3);
						otmp->owt = weight(otmp);
					}
				}
			}
			else if ((otmp = mksobj_at(WORTHLESS_PIECE_OF_BLACK_GLASS, x+size/2, y+size/2, MKOBJ_NOINIT)) != 0) {
				otmp->quan = d(3,3);
				otmp->owt = weight(otmp);
			}
		}
		if(!toostrong(PM_SHATTERED_ZIGGURAT_WIZARD, (level_difficulty() + u.ulevel) / 2 + 5)){
			makemon(&mons[PM_SHATTERED_ZIGGURAT_WIZARD], x+size/2, y+size/2, MM_ADJACENTOK);
			for(i = rnd(6)+rnd(4); i > 0; i--)
				makemon(&mons[PM_SHATTERED_ZIGGURAT_KNIGHT], x+size/2, y+size/2, MM_ADJACENTOK);
		} else {
			for(i = rnd(4); i > 0; i--)
				makemon(&mons[PM_SHATTERED_ZIGGURAT_KNIGHT], x+size/2, y+size/2, MM_ADJACENTOK);
		}
		for(i = rn1(6,6); i > 0; i--)
			makemon(&mons[PM_SHATTERED_ZIGGURAT_CULTIST], x+size/2, y+size/2, MM_ADJACENTOK);
	}
}

STATIC_OVL
void
mkmch(typ)
int typ;
{
	//Note: See SCP-2406 for the inspiration for this.
	int x, y, tries = 0, ncrew, good = FALSE;
	struct obj *statue, *smallstatue, *otmp;
	while(!good && tries < 500){
		x = rn2(COLNO-4)+3;
		y = rn2(ROWNO-2)+1;
		tries++;
		if(isok(x,y) && levl[x][y].typ == typ)
			good = TRUE;
		else continue;
		
		statue = mksobj_at(STATUE, x, y, MKOBJ_NOINIT);
		if(statue){
			statue->corpsenm = PM_COLOSSAL_CLOCKWORK_WAR_MACHINE;
			otmp = mksobj(ARCHAIC_PLATE_MAIL, NO_MKOBJ_FLAGS);
			if(otmp){
				otmp->cursed = TRUE;
				otmp->blessed = FALSE;
				otmp->objsize = MZ_GIGANTIC;
				otmp->spe = max_ints(otmp->spe,rnd(3)*rnd(3));
				otmp->oeroded2 = rn2(3);
				fix_object(otmp);
				add_to_container(statue, otmp);
			}
			
			for(tries = rn1(5,5); tries > 0; tries--){
				otmp = mksobj(CLOCKWORK_COMPONENT, MKOBJ_NOINIT);
				if(otmp){
					add_to_container(statue, otmp);
				}
			}
			if(!rn2(4)){
				for(tries = d(1,4); tries > 0; tries--){
					otmp = mksobj(HELLFIRE_COMPONENT, MKOBJ_NOINIT);
					if(otmp){
						add_to_container(statue, otmp);
					}
				}
			}
			if(!rn2(4)){
				for(tries = d(1,4); tries > 0; tries--){
					otmp = mksobj(SUBETHAIC_COMPONENT, MKOBJ_NOINIT);
					if(otmp){
						add_to_container(statue, otmp);
					}
				}
			}
			otmp = mksobj(UPGRADE_KIT, MKOBJ_NOINIT);
			if(otmp){
				add_to_container(statue, otmp);
			}
			if(!rn2(20)){
				otmp = mksobj(WAN_FIRE, NO_MKOBJ_FLAGS);
				if(otmp){
					add_to_container(statue, otmp);
				}
				for(tries = d(2,4); tries > 0; tries--){
					otmp = mksobj(POT_OIL, NO_MKOBJ_FLAGS);
					if(otmp){
						add_to_container(statue, otmp);
					}
				}
			}
			if(!rn2(20)){
				otmp = mksobj(WAN_LIGHTNING, NO_MKOBJ_FLAGS);
				if(otmp){
					add_to_container(statue, otmp);
				}
				for(tries = d(2,4); tries > 0; tries--){
					otmp = mksobj(POT_ACID, NO_MKOBJ_FLAGS);
					if(otmp){
						add_to_container(statue, otmp);
					}
				}
			}
			if(!rn2(20)){
				otmp = mksobj(WAN_STRIKING, NO_MKOBJ_FLAGS);
				if(otmp){
					add_to_container(statue, otmp);
				}
			}
			if(!rn2(50)){
				otmp = mksobj(WAN_DEATH, NO_MKOBJ_FLAGS);
				if(otmp){
					add_to_container(statue, otmp);
				}
			}
			
			////////////////////
			for(ncrew = rnd(8); ncrew > 0; ncrew--){
				smallstatue = mksobj(STATUE, MKOBJ_NOINIT);
				if(smallstatue){
					smallstatue->corpsenm = PM_CLOCKWORK_AUTOMATON;
					otmp = mksobj(ARCHAIC_PLATE_MAIL, NO_MKOBJ_FLAGS);
					if(otmp){
						if(rn2(5)){
							otmp->cursed = TRUE;
							otmp->blessed = FALSE;
							otmp->oeroded2 = rn2(3);
						}
						otmp->spe = max_ints(otmp->spe,rnd(3)+rnd(3));
						fix_object(otmp);
						add_to_container(smallstatue, otmp);
					}
					for(tries = rn1(2,2); tries > 0; tries--){
						otmp = mksobj(CLOCKWORK_COMPONENT, MKOBJ_NOINIT);
						if(otmp){
							add_to_container(smallstatue, otmp);
						}
					}
					if(!rn2(4)){
						for(tries = rnd(2); tries > 0; tries--){
							otmp = mksobj(HELLFIRE_COMPONENT, MKOBJ_NOINIT);
							if(otmp){
								add_to_container(smallstatue, otmp);
							}
						}
					}
					if(!rn2(4)){
						for(tries = rnd(2); tries > 0; tries--){
							otmp = mksobj(SUBETHAIC_COMPONENT, MKOBJ_NOINIT);
							if(otmp){
								add_to_container(smallstatue, otmp);
							}
						}
					}
					smallstatue->owt = weight(smallstatue);
					////////////////////
					add_to_container(statue, smallstatue);
				}
			}
			////////////////////
			otmp = mksobj(SPIKE, MKOBJ_NOINIT);
			if(otmp){
				otmp->cursed = TRUE;
				otmp->blessed = FALSE;
				otmp->objsize = MZ_GIGANTIC;
				otmp->spe = 5;
				set_material_gm(otmp, BONE);
				otmp->quan = rn1(5,5);
				otmp->opoisoned = (OPOISON_PARAL|OPOISON_ACID);
				fix_object(otmp);
				add_to_container(statue, otmp);
			}
			////////////////////
			statue->owt = weight(statue);
		}
	}
}

STATIC_OVL
void
mkwrk(typ)
int typ;
{
	int x, y, tries = 0, good = FALSE;
	struct obj *otmp;
	while(!good && tries < 500){
		x = rn2(COLNO-4)+3;
		y = rn2(ROWNO-2)+1;
		tries++;
		if(isok(x,y) && levl[x][y].typ == typ)
			good = TRUE;
		else continue;
		
		if(!rn2(10)){
			//Six-armed clockwork
			int nwep;
			otmp = mksobj_at(UPGRADE_KIT, x, y, NO_MKOBJ_FLAGS);
			for(tries = rn1(3,3); tries > 0; tries--)
				otmp = mksobj_at(CLOCKWORK_COMPONENT, x, y, NO_MKOBJ_FLAGS);
			otmp = mksobj_at(ARCHAIC_PLATE_MAIL, x, y, NO_MKOBJ_FLAGS);
			nwep = rnd(6);
			for(tries = 0; tries < nwep; tries++){
				otmp = mksobj_at(SCIMITAR, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
			}
			for(; nwep < 6; nwep++){
				otmp = mksobj_at(ROUNDSHIELD, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				otmp->objsize = MZ_SMALL;
				fix_object(otmp);
			}
			if(rn2(2)){
				otmp = mksobj_at(GAUNTLETS, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
				otmp = mksobj_at(GAUNTLETS, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
				otmp = mksobj_at(GAUNTLETS, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
			} else if(rn2(2)){
				otmp = mksobj_at(GAUNTLETS_OF_POWER, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
				otmp = mksobj_at(GAUNTLETS_OF_POWER, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
				otmp = mksobj_at(GAUNTLETS_OF_POWER, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
			} else {
				otmp = mksobj_at(ORIHALCYON_GAUNTLETS, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
				otmp = mksobj_at(ORIHALCYON_GAUNTLETS, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
				otmp = mksobj_at(ORIHALCYON_GAUNTLETS, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
			}
		} else if(rn2(9)){
			//Clockwork Horror
			if(rn2(10)){		//9 out of 10
				otmp = mksobj_at(UPGRADE_KIT, x, y, NO_MKOBJ_FLAGS);
				for(tries = rn1(3,3); tries > 0; tries--)
					otmp = mksobj_at(CLOCKWORK_COMPONENT, x, y, NO_MKOBJ_FLAGS);
				otmp = mksobj_at(!rn2(3) ? STILETTO : !rn2(2) ? AXE : WAR_HAMMER, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, COPPER);
				fix_object(otmp);
				mksobj_at(WORTHLESS_PIECE_OF_WHITE_GLASS + rn2(WORTHLESS_PIECE_OF_VIOLET_GLAS - WORTHLESS_PIECE_OF_WHITE_GLASS + 1), x, y, NO_MKOBJ_FLAGS);
			} else if(rn2(10)){	//9 out of 100
				otmp = mksobj_at(UPGRADE_KIT, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, SILVER);
				fix_object(otmp);
				for(tries = rn1(3,3); tries > 0; tries--){
					otmp = mksobj_at(CLOCKWORK_COMPONENT, x, y, NO_MKOBJ_FLAGS);
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
				}
				if(find_sawant()){
					otmp = mksobj(find_sawant(), NO_MKOBJ_FLAGS);
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					place_object(otmp, x, y);
				} else {
					otmp = mksobj_at(PISTOL, x, y, NO_MKOBJ_FLAGS);
					set_material_gm(otmp, SILVER);
					fix_object(otmp);
					otmp = mksobj_at(BULLET, x, y, NO_MKOBJ_FLAGS);
					set_material_gm(otmp, SILVER);
					otmp->quan += rn1(20,20);
					fix_object(otmp);
				}
				otmp = mksobj_at(VIBROBLADE, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, SILVER);
				fix_object(otmp);
				mksobj_at(AMBER + rn2(JADE - AMBER + 1), x, y, NO_MKOBJ_FLAGS);
			} else if(rn2(10)){	//9 out of 1000
				otmp = mksobj_at(UPGRADE_KIT, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				for(tries = rn1(3,3); tries > 0; tries--){
					otmp = mksobj_at(CLOCKWORK_COMPONENT, x, y, NO_MKOBJ_FLAGS);
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
				}
				if(find_gawant()){
					otmp = mksobj(find_gawant(), NO_MKOBJ_FLAGS);
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
					place_object(otmp, x, y);
				} else {
					otmp = mksobj_at(ARM_BLASTER, x, y, NO_MKOBJ_FLAGS);
					set_material_gm(otmp, GOLD);
					fix_object(otmp);
				}
				otmp = mksobj_at(FORCE_PIKE, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, GOLD);
				fix_object(otmp);
				mksobj_at(RUBY + rn2(AQUAMARINE - RUBY + 1), x, y, NO_MKOBJ_FLAGS);
			} else {			//1 out of 1000
				otmp = mksobj_at(UPGRADE_KIT, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, PLATINUM);
				fix_object(otmp);
				for(tries = rn1(3,3); tries > 0; tries--){
					otmp = mksobj_at(CLOCKWORK_COMPONENT, x, y, NO_MKOBJ_FLAGS);
					set_material_gm(otmp, PLATINUM);
					fix_object(otmp);
				}
				if(find_pawant()){
					otmp = mksobj(find_pawant(), NO_MKOBJ_FLAGS);
					set_material_gm(otmp, PLATINUM);
					fix_object(otmp);
					place_object(otmp, x, y);
				} else {
					otmp = mksobj_at(BFG, x, y, NO_MKOBJ_FLAGS);
					set_material_gm(otmp, PLATINUM);
					fix_object(otmp);
				}
				//Atma Weapon 1/20000
				otmp = mksobj_at(BEAMSWORD, x, y, MKOBJ_ARTIF);
				set_material_gm(otmp, PLATINUM);
				fix_object(otmp);
				mksobj_at(MAGICITE_CRYSTAL + rn2(STAR_SAPPHIRE - MAGICITE_CRYSTAL + 1), x, y, NO_MKOBJ_FLAGS);
			}
		} else {
			//Unknown rusted scrap pile
			otmp = mksobj_at(UPGRADE_KIT, x, y, NO_MKOBJ_FLAGS);
			if(otmp){
				set_material_gm(otmp, IRON);
				otmp->oeroded2 = 2;
				fix_object(otmp);
			}
			for(tries = rn1(2,2); tries > 0; tries--){
				otmp = mksobj_at(CLOCKWORK_COMPONENT, x, y, NO_MKOBJ_FLAGS);
				set_material_gm(otmp, IRON);
				otmp->oeroded2 = 2;
				fix_object(otmp);
			}
			for(tries = rn1(4,4); tries > 0; tries--){
				otmp = mksobj_at(SCRAP, x, y, NO_MKOBJ_FLAGS);
			}
		}
	}
}

STATIC_OVL
void
mkaph(junked)
int junked;
{
	int x, y, i, tries = 0, good = FALSE;
	struct obj *otmp;
	struct monst *mon;
	if(junked){
		for(i = rnd(2) + rn2(2); i > 0; i--){
			mon = makemon(&mons[PM_APHANACTONAN_AUDIENT], 0, 0, NO_MM_FLAGS);
			if(mon && !good){
				good = TRUE;
				x = mon->mx;
				y = mon->my;
				if(!rn2(20)){
					struct obj *statue;
					statue = mksobj_at(STATUE, x, y, NO_MKOBJ_FLAGS);
					if(statue){
						statue->corpsenm = PM_APHANACTONAN_ASSESSOR;
						fix_object(statue);
						otmp = mksobj(CLOCKWORK_COMPONENT, MKOBJ_NOINIT);
						if(otmp){
							otmp->quan = d(4,8)/2;
							otmp->oeroded2 = 3;
							otmp->owt = weight(otmp);
							add_to_container(statue, otmp);
						}
						otmp = mksobj(UPGRADE_KIT, NO_MKOBJ_FLAGS);
						if(otmp){
							add_to_container(statue, otmp);
						}
						if(!rn2(20)){
							otmp = mksobj(APHANACTONAN_ARCHIVE, NO_MKOBJ_FLAGS);
							if(otmp){
								add_to_container(statue, otmp);
							}
						}
					}
				}
				else {
					otmp = mksobj_at(CLOCKWORK_COMPONENT, x, y, NO_MKOBJ_FLAGS);
					if(otmp){
						otmp->quan = d(4,8)/2;
						otmp->oeroded2 = 3;
						otmp->owt = weight(otmp);
					}
					if(rn2(2)){
						otmp = mksobj_at(ARCHAIC_PLATE_MAIL, x, y, NO_MKOBJ_FLAGS);
						if(otmp){
							otmp->oeroded2 = 3;
							otmp->objsize = MZ_LARGE;
							fix_object(otmp);
						}
					}
					if(!rn2(4)){
						otmp = mksobj_at(UPGRADE_KIT, x, y, NO_MKOBJ_FLAGS);
						if(otmp){
							otmp->oeroded2 = 3;
						}
					}
				}
			}
		}
	}
	else {
		mon = makemon(&mons[PM_APHANACTONAN_ASSESSOR], 0, 0, NO_MM_FLAGS);
		if(mon){
			x = mon->mx;
			y = mon->my;
			makemon(&mons[PM_APHANACTONAN_AUDIENT], x, y, MM_ADJACENTOK);
			makemon(&mons[PM_APHANACTONAN_AUDIENT], x, y, MM_ADJACENTOK);
		}
	}
}

STATIC_OVL
void
mklostitem(typ)
int typ;
{
	int x, y, tries = 0, good = FALSE;
	struct obj *otmp;
	while(!good && tries < 500){
		x = rn2(COLNO-4)+3;
		y = rn2(ROWNO-2)+1;
		tries++;
		if(isok(x,y) && levl[x][y].typ == typ)
			good = TRUE;
		else continue;
		
		switch(rn2(3)){
			case 0:
				mkobj_at(WEAPON_CLASS, x, y, MKOBJ_ARTIF);
			break;
			case 1:
				mkobj_at(ARMOR_CLASS, x, y, MKOBJ_ARTIF);
			break;
			case 2:
				mkobj_at(TOOL_CLASS, x, y, MKOBJ_ARTIF);
			break;
		}
	}
}

void
mklawfossil(typ)
int typ;
{
	int x, y, tries = 0, good = FALSE;
	struct obj *otmp;
	while(!good && tries < 500){
		x = rn2(COLNO-4)+3;
		y = rn2(ROWNO-2)+1;
		tries++;
		if(isok(x,y) && levl[x][y].typ == typ)
			good = TRUE;
		else continue;
		
		otmp = mksobj(FOSSIL, NO_MKOBJ_FLAGS);
		otmp->corpsenm = PM_ANCIENT_NUPPERIBO;
		fix_object(otmp);
		place_object(otmp, x, y);
	}
}

void
place_lolth_vaults()
{
	int num, i;
	//"portal room" upstairs with 4 vrocks
	mklolthup();
	//"portal room" downstairs with 1 marilith
	mklolthdown();
	//trollguards: 6x6 room (4x4 + walls) with 10 trolls.  Gold and gems
	if(!rn2(3))
		mklolthtroll();
	//Gnoll barracks
	else if(rn2(2))
		mklolthgnoll();
	else {
		mklolthtroll();
		mklolthgnoll();
	}
	//(leveled?) werewolf cage?
	//Garden
	if(rn2(3)){
		mklolthgarden();
	}
	if(!rn2(3)){
		mklolthgarden();
	}
	//4 drow clerics and 1 drow wizard Trapped chest, 4 full healing potions, gold and gems
	mklolthtreasure();
	//Co-alligned temple.  Priest transforms and becomes hostile if entered.
	if(!rn2(3)){
		mklolthtemple();
	}
	//prison 0 - 6 prisons. Prisoners can be rescued for pets.
	num = rn2(3) + rn2(3) + rn2(3);
	for(i = 0; i < num; i++) mklolthcell();
	//sepulcher
	if(!rn2(4)){
		mklolthsepulcher();
	}
	//mivault
	num = rnd(3);
	for(i = 0; i < num; i++) mkmivaultlolth();
	//Vault
	num = rnd(6);
	for(i = 0; i < num; i++) mkvaultlolth();
}

void
place_chaos_forest_features()
{
	if(In_mordor_forest(&u.uz)){
		int i = 6 + d(2,6);
		mkforest12river();
		for(; i > 0; i--)
			mkelfhut(0);
		wallification(1,0,COLNO-1,ROWNO-1);
	} else if(Is_ford_level(&u.uz)){
		int i = 3 + d(2,3);
		for(; i > 0; i--)
			mkelfhut(1);
		i = 1 + d(3,4);
		for(; i > 0; i--)
			mkwraithclearing(1);
		wallification(1,0,COLNO-1,ROWNO-1);
	} else if(In_mordor_fields(&u.uz)){
		int i = 4 + d(4,4);
		for(; i > 0; i--)
			mkwraithclearing(0);
	} else if(on_level(&u.uz, &mordor_depths_2_level)){
		mklavapool();
	} else if(on_level(&u.uz, &mordor_depths_3_level)){
		int i = 20+ d(4,10);
		for(; i > 0; i--)
			mkstonepillars();
	}
	if(In_mordor_depths(&u.uz)){
		int i = d(3,4);
		for(;i>0;i--)
			mkmordorfossil(STONE);
		i = rn2(8);
		for(;i>0;i--)
			mkmordorfossil(ROOM);
	} else if(In_mordor_borehole(&u.uz)){
		int i = d(3,6);
		for(;i>0;i--)
			mkmordorfossil(STONE);
	}
}

void
place_neutral_features()
{
	if(!rn2(20)){
		mkkamereltowers();
		if(!rn2(12))
			mkfishingvillage();
	} else if(!rn2(12)){
		mkminorspire();
	} else if(!rn2(12)){
		mkfishingvillage();
	}

	if(!rn2(6)){
		mkneuriver();
	}
	
	// mkferrufort();
	
	if(!rn2(6)){
		mkpluvillage();
	// } else if(){
		// mkferrufort();
	}
	
	if(!rn2(12))
		mkinvertzigg();
	
	if(!rn2(6))
		mkferrutower();
	
	if(!rn2(3)){
		int n = rnd(4) + rn2(4);
		for(; n > 0; n--)
			mkpluhomestead();
	} 
	else if(!rn2(3)){
		int n = rnd(3);
		for(; n > 0; n--)
			mkpluhomestead();
	} 
}

void
place_sum_all_features()
{
	mktimaresh();
	int n = 1 + !rn2(3) + rn2(2);
	for(; n > 0; n--)
		mkferrubarracks();
	
	n = d(1,3);
	for(; n > 0; n--)
		mksumbuilding(6);
	n = d(1,3);
	for(; n > 0; n--)
		mksumbuilding(5);
	n = d(2,3);
	for(; n > 0; n--)
		mksumbuilding(4);
	n = d(3,6);
	for(; n > 0; n--)
		mksumbuilding(3);
	n = rn2(3);
	for(; n > 0; n--)
		mkfountainsquare(FALSE);
	n = rn2(3);
	for(; n > 0; n--)
		mkfountainsquare(TRUE);

	n = d(2,3);
	for(; n > 0; n--)
		mktowntree();
	for(int i = 4; i > 0; i--){
		n = d(1,3);
		for(; n > 0; n--)
			mkpluroom(5);
		n = d(2,3);
		for(; n > 0; n--)
			mkpluroom(4);
		n = d(3,6);
		for(; n > 0; n--)
			mkpluroom(3);
	}
}

void
place_law_features()
{
	int n;
	if(Is_path(&u.uz)){
		int cutoff = rn2(9);
		for(n = 4; n > 0; n--)
			mkaph(cutoff >= n);
		if(!rn2(10)){
		// if(1){
			n = 10-int_sqrt(rnd(99));
			for(; n > 0; n--)
				mkmch(ROOM);
		} else if(!rn2(10)){
			n = 10-int_sqrt(rnd(99));
			for(; n > 0; n--)
				mkmch(STONE);
		} else if(!rn2(4)){
			n = 5 - int_sqrt(rnd(24));
			for(; n > 0; n--)
				mkwrk(ROOM);
		} else {
			n = 5 - int_sqrt(rnd(24));
			for(; n > 0; n--)
				mkwrk(STONE);
		}
		n = rn2(3)+rn2(3);
		for(; n > 0; n--)
			mklostitem(STONE);
		
		if(!rn2(4)){
		// if(1){
			n = 5 - int_sqrt(rnd(24));
			for(; n > 0; n--)
				mklawfossil(STONE);
		}
		
	} else if(Is_arcadia_woods(&u.uz)){
		if(on_level(&u.uz, &arcadia1_level)){
			if(!rn2(3))
				mkcamp(VALAVI_CAMP);
			if(!rn2(3))
				mkcamp(FORMIAN_CAMP1);
			if(!rn2(3))
				mkcamp(FORMIAN_CAMP2);
			if(!rn2(3))
				mkcamp(THRIAE_CAMP);
		} else if(on_level(&u.uz, &arcadia2_level)){
			if(rn2(2))
				mkcamp(VALAVI_CAMP);
			if(rn2(2))
				mkcamp(FORMIAN_CAMP1);
			if(rn2(2))
				mkcamp(FORMIAN_CAMP2);
			if(rn2(2))
				mkcamp(THRIAE_CAMP);
		} else {
			if(!rn2(4))
				mkcamp(VALAVI_CAMP);
			if(!rn2(4))
				mkcamp(FORMIAN_CAMP1);
			if(!rn2(4))
				mkcamp(FORMIAN_CAMP2);
			if(!rn2(4))
				mkcamp(THRIAE_CAMP);
		}
	}
}

void
mkroom(roomtype)
/* make and stock a room of a given type */
int	roomtype;
{
    if (roomtype >= SHOPBASE)
		mkshop();	/* someday, we should be able to specify shop type */
    else switch(roomtype) {
	case COURT:	mkzoo(COURT); break;
	case ZOO:	mkzoo(ZOO); break;
	case BEEHIVE:	mkzoo(BEEHIVE); break;
	case MORGUE:	mkzoo(MORGUE); break;
	case ARMORY:	mkzoo(ARMORY); break;
	case BARRACKS:	mkzoo(BARRACKS); break;
	case SWAMP:	mkswamp(); break;
	case GARDEN:	mkgarden((struct mkroom *)0); break;
	case LIBRARY:	mklibrary((struct mkroom *)0); break;
	case TEMPLE:	mktemple(); break;
	case LEPREHALL:	mkzoo(LEPREHALL); break;
	case COCKNEST:	mkzoo(COCKNEST); break;
	case ANTHOLE:	mkzoo(ANTHOLE); break;
	case ISLAND: mkisland(); break;
	case STATUEGRDN: mksgarden(); break;
	case RIVER: mkriver(); break;
	case POOLROOM:	mkpoolroom(); break;
	case SLABROOM:	mkslabroom(); break;
	case ELSHAROOM:	mkzoo(ELSHAROOM); break;
	case HELL_VAULT:	mkhellvaultroom(); break;
	default:	impossible("Tried to make a room of type %d.", roomtype);
    }
}

STATIC_OVL void
mkshop()
{
	register struct mkroom *sroom;
	int i = -1;
#ifdef WIZARD
	char *ep = (char *)0;	/* (init == lint suppression) */

	/* first determine shoptype */
	if(wizard){
#ifndef MAC
		ep = nh_getenv("SHOPTYPE");
		if(ep){
			if(*ep == 'z' || *ep == 'Z'){
				mkzoo(ZOO);
				return;
			}
			if(*ep == 'm' || *ep == 'M'){
				mkzoo(MORGUE);
				return;
			}
			if(*ep == 'b' || *ep == 'B'){
				mkzoo(BEEHIVE);
				return;
			}
			if(*ep == 't' || *ep == 'T' || *ep == '\\'){
				mkzoo(COURT);
				return;
			}
			if(*ep == 's' || *ep == 'S'){
				mkzoo(BARRACKS);
				return;
			}
			if(*ep == 'a' || *ep == 'A'){
				mkzoo(ANTHOLE);
				return;
			}
			if(*ep == 'c' || *ep == 'C'){
				mkzoo(COCKNEST);
				return;
			}
			if(*ep == 'l' || *ep == 'L'){
				mkzoo(LEPREHALL);
				return;
			}
			if(*ep == 'o' || *ep == 'O'){
				mkpoolroom();
				return;
			}
			if(*ep == '_'){
				mktemple();
				return;
			}
			if(*ep == 'n'){
				mkgarden((struct mkroom *)0);
				return;
			}
			if(*ep == '\''){
				mklibrary((struct mkroom *)0);
				return;
			}
			if(*ep == '}'){
				mkswamp();
				return;
			}
			if(*ep == 'w' || *ep == 'W'){
				mkisland();
				return;
			}
			/* note: shops >= UNIQUESHOP don't have valid class symbols, nor are generated randomly */
			for(i=0; shtypes[i].name && i < (UNIQUESHOP - SHOPBASE); i++)
				if(shtypes[i].iprobs[0].itype >= 0)
				if(*ep == def_oc_syms[(int)shtypes[i].iprobs[0].itype])
				    goto gottype;
			if(*ep == 'g' || *ep == 'G')
				i = 0;
			else
				i = -1;
		}
#endif
	}
#ifndef MAC
gottype:
#endif
#endif
	for(sroom = &rooms[level.flags.sp_lev_nroom]; ; sroom++){
		if(sroom->hx < 0) return;
		if(sroom - rooms >= nroom) {
			pline("rooms not closed by -1?");
			return;
		}
		if(sroom->rtype != OROOM) continue;
		if(has_dnstairs(sroom) || has_upstairs(sroom))
			continue;
		if (sroom->rtype != JOINEDROOM && (
#ifdef WIZARD
		   (wizard && ep && sroom->doorct != 0) ||
#endif
			sroom->doorct == 1)) break;
	}
	if (!sroom->rlit) {
		int x, y;

		for(x = sroom->lx - 1; x <= sroom->hx + 1; x++)
		for(y = sroom->ly - 1; y <= sroom->hy + 1; y++)
			levl[x][y].lit = 1;
		sroom->rlit = 1;
	}

	if(i < 0) {			/* shoptype not yet determined */
	    register int j;

	    /* pick a shop type at random */
	    for (j = rnd(100), i = 0; (j -= shtypes[i].prob) > 0; i++)
		continue;

	    /* big rooms cannot be wand or book shops,
	     * - so make them general stores
	     */
	    if(isbig(sroom) && (shtypes[i].shoptype == WANDSHOP
				|| shtypes[i].shoptype == BOOKSHOP)) i = 0;
	}
	sroom->rtype = SHOPBASE + i;

	/* set room bits before stocking the shop */
#ifdef SPECIALIZATION
	topologize(sroom, FALSE); /* doesn't matter - this is a special room */
#else
	topologize(sroom);
#endif

	/* stock the room with a shopkeeper and artifacts */
	stock_room(i, sroom);
}

struct mkroom *
pick_room(nostairs, fullwalls)
register boolean nostairs, fullwalls;
/* pick an unused room, preferably with few doors */
{
	register struct mkroom *sroom;
	register int i = nroom;

	int minroom = level.flags.sp_lev_nroom;

	if (nroom == minroom)
		return (struct mkroom *)0;

	for(sroom = &rooms[minroom + rn2(nroom - minroom)]; i--; sroom++) {
		/* loop through rooms if needed */
		if(sroom == &rooms[nroom])
			sroom = &rooms[minroom + 1];
		/* if its not a room at all, end */
		if(sroom->hx < 0)
			return (struct mkroom *)0;
		/* if full walls are required, it must be an OROOM and not a JOINEDROOM */
		if(!(sroom->rtype == OROOM) && !(sroom->rtype == JOINEDROOM && !fullwalls))
			continue;
		/* if no stairs are required, it cannot have downstairs */
		if (has_upstairs(sroom) ||
			(has_dnstairs(sroom) && (nostairs || rn2(3)))) {
			continue;
		}
		/* prefer rooms with only one door */
		if (sroom->doorct == 1 || !rn2(sroom->doorct) || wizard)
			return sroom;
		else
			i += !rn2(3);	// 1/3 chance of +1 attempt
	}
	return (struct mkroom *)0;
}

/*
 * Some special rooms can be made in places that don't have full walls.
 */
boolean
special_room_requires_full_walls(type)
int type;
{
	switch (type)
	{
	case OROOM:
	case SWAMP:
	case MORGUE:
	case ARMORY:
	case RIVER:
	case POOLROOM:
	case SLABROOM:
	case JOINEDROOM:
		return FALSE;
	default:
		return TRUE;
	}
}

STATIC_OVL void
mkzoo(type)
int type;
{
	register struct mkroom *sroom;
	boolean fullwalls = special_room_requires_full_walls(type);

	if ((sroom = pick_room(FALSE,fullwalls)) != 0) {
		sroom->rtype = type;
		fill_zoo(sroom);
	}
}

STATIC_OVL boolean
cell_spot(sx, sy)
int sx,sy;
{
	int i,j;
	for(i=sx-1;i<sx+2;i++)
		for(j=sy-1;j<sy+2;j++){
			if(!isok(i,j) || !(IS_ROCK(levl[i][j].typ) || IS_WALL(levl[i][j].typ))){
				return FALSE;
			}
		}
	return TRUE;
}

STATIC_OVL struct monst *
prisoner(kingtype, sx, sy)
int kingtype;
int sx,sy;
{
	int mtyp = NON_PM;
	struct monst *mon;
	boolean polyps = FALSE;
	//Note: all throne rooms are part of rodney's forces, so everyone has prisoners.
	switch(kingtype){
		case 0:{
			int tries = 500;
			int zlevel, minmlev, maxmlev;
			
			zlevel = level_difficulty();
			/* determine the level of the weakest monster to make. */
			minmlev = (zlevel + u.ulevel) / 3 - 5;
			/* determine the level of the strongest monster to make. */
			maxmlev = (zlevel + u.ulevel) / 2 + 5;
			int prisoners[] = { PM_GRIMLOCK, PM_GARGOYLE, PM_WINGED_GARGOYLE, PM_HOBBIT, PM_DWARF, PM_BUGBEAR, 
				PM_DWARF_LORD, PM_DWARF_CLERIC, PM_DEEP_ONE, PM_DEEPER_ONE, PM_GNOLL, PM_GITHYANKI_PIRATE,
				PM_ELOCATOR,
				PM_IMP, PM_QUASIT, PM_TENGU, PM_URANIUM_IMP, 
				PM_KOBOLD, PM_LARGE_KOBOLD, 
				PM_KOBOLD_LORD, PM_KOBOLD_SHAMAN, PM_WINGED_KOBOLD, 
				PM_LEPRECHAUN, 
				PM_QUICKLING, PM_DRYAD,
				PM_NAIAD, PM_OREAD, PM_SWAMP_NYMPH, PM_YUKI_ONNA, PM_DEMINYMPH, 
				PM_GOBLIN, PM_HOBGOBLIN,
				PM_HILL_ORC, PM_MORDOR_ORC, PM_URUK_HAI, PM_ORC_SHAMAN, PM_ORC_CAPTAIN, PM_URUK_CAPTAIN,
				PM_ANGBAND_ORC, 
				PM_ANGEL, PM_KI_RIN,
				PM_JUSTICE_ARCHON, PM_SWORD_ARCHON, PM_SHIELD_ARCHON, PM_TRUMPET_ARCHON, PM_WARDEN_ARCHON, PM_THRONE_ARCHON,
				PM_MOVANIC_DEVA, PM_MONADIC_DEVA, PM_ASTRAL_DEVA, PM_GRAHA_DEVA, PM_SURYA_DEVA,
				PM_LILLEND,
				PM_COURE_ELADRIN, PM_NOVIERE_ELADRIN, PM_BRALANI_ELADRIN, PM_FIRRE_ELADRIN, PM_SHIERE_ELADRIN, PM_GHAELE_ELADRIN, PM_TULANI_ELADRIN,
				PM_PLAINS_CENTAUR, PM_FOREST_CENTAUR, PM_MOUNTAIN_CENTAUR,
				PM_SPROW, PM_DRIDER, PM_PRIESTESS_OF_GHAUNADAUR, PM_FORMIAN_CRUSHER, PM_FORMIAN_TASKMASTER,
				PM_GNOME, PM_GNOME_LORD, PM_GNOME_LADY, PM_TINKER_GNOME, PM_GNOMISH_WIZARD,
				PM_GIANT, PM_STONE_GIANT, PM_HILL_GIANT, PM_FIRE_GIANT, PM_FROST_GIANT, PM_STORM_GIANT,
				PM_ETTIN, PM_MINOTAUR, PM_MINOTAUR_PRIESTESS, 
				PM_ANCIENT_NAGA,
				PM_OGRE, PM_OGRE_MAGE, PM_OGRE_LORD,
				PM_QUANTUM_MECHANIC, 
				PM_SERPENT_MAN_OF_YOTH,
				PM_PISACA,
				PM_TROLL, PM_ICE_TROLL, PM_ROCK_TROLL, PM_WATER_TROLL, PM_OLOG_HAI, PM_WOOD_TROLL, 
				PM_VAMPIRE, PM_VAMPIRE_LADY, PM_VAMPIRE_LORD,
				PM_YETI, PM_SASQUATCH, PM_GUG, 
				PM_UNDEAD_KNIGHT, PM_WARRIOR_OF_SUNLIGHT, PM_UNDEAD_MAIDEN, PM_KNIGHT_OF_THE_PRINCESS_S_GUARD,
				PM_BLUE_SENTINEL, PM_DARKMOON_KNIGHT, PM_UNDEAD_REBEL, PM_PARDONER, PM_OCCULTIST, 
				PM_LIVING_DOLL,
				PM_PLUMACH_RILMANI, PM_FERRUMACH_RILMANI, PM_CUPRILACH_RILMANI, PM_ARGENACH_RILMANI,
				PM_PEASANT, PM_MINER, PM_WEREJACKAL, PM_WOODLAND_ELF, PM_DROW_CAPTAIN, PM_GREEN_ELF,
				PM_WERERAT, PM_GREY_ELF, PM_WEREWOLF, PM_HEDROW_WARRIOR, PM_ELF_LORD, PM_ELF_LADY,
				PM_DOPPELGANGER, PM_HEDROW_WIZARD, PM_NURSE, PM_MAID, PM_UNEARTHLY_DROW, PM_DROW_MATRON,
				PM_HEDROW_BLADEMASTER,
				PM_PRISONER, PM_MINDLESS_THRALL,
				PM_WATCHMAN, PM_WATCH_CAPTAIN, 
				PM_MARID, PM_DJINNI, PM_SANDESTIN,
				PM_HORNED_DEVIL, PM_ERINYS, PM_BARBED_DEVIL, PM_BONE_DEVIL, PM_ICE_DEVIL, 
				PM_SUCCUBUS, PM_INCUBUS, PM_VROCK, PM_DAUGHTER_OF_BEDLAM, PM_LILITU, PM_MARILITH,
				PM_FALLEN_ANGEL, 
				PM_SALAMANDER, 
				PM_ARCHEOLOGIST, PM_ARCHEOLOGIST, PM_BARBARIAN, PM_BARBARIAN, PM_HALF_DRAGON, PM_HALF_DRAGON, PM_BARD, PM_BARD, 
				PM_CONVICT, PM_CONVICT, PM_HEALER, PM_HEALER, PM_KNIGHT, PM_KNIGHT, PM_MONK, PM_MONK, 
				PM_MADMAN, PM_MADWOMAN, PM_PRIEST, PM_PRIESTESS, PM_NOBLEMAN, PM_NOBLEWOMAN, PM_PIRATE, PM_PIRATE,
				PM_RANGER, PM_RANGER, PM_ROGUE, PM_ROGUE, PM_SAMURAI, PM_SAMURAI, PM_TOURIST, PM_TOURIST,
				PM_VALKYRIE, PM_VALKYRIE, PM_WIZARD, PM_WIZARD,
				PM_SMALL_GOAT_SPAWN, PM_GOAT_SPAWN, PM_GIANT_GOAT_SPAWN,
				PM_APPRENTICE_WITCH, PM_WITCH
			};
			do mtyp = ROLL_FROM(prisoners);
			while((tooweak(mtyp, minmlev) || toostrong(mtyp,maxmlev)) && tries--);
		}break;
		case PM_DWARF_KING:
		case PM_DWARF_QUEEN:{
			int prisoners[] = {
				PM_MINER, PM_MINER, PM_WATCHMAN, PM_WATCHMAN, PM_WATCH_CAPTAIN, 
				PM_WOODLAND_ELF, PM_WOODLAND_ELF, PM_GREEN_ELF, PM_GREEN_ELF, PM_GREY_ELF,
				PM_HEDROW_WARRIOR, PM_HEDROW_WIZARD, PM_DROW_CAPTAIN,
				PM_HOBBIT, PM_HOBBIT,
				PM_DRYAD, PM_NAIAD, PM_OREAD, PM_YUKI_ONNA, PM_DEMINYMPH
			};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_KOBOLD_LORD:{
			int prisoners[] = {PM_GNOME, PM_GNOME, PM_GNOME, PM_GNOME, PM_GNOME_LORD, PM_GNOME_LADY, PM_TINKER_GNOME, PM_GNOMISH_WIZARD,
									PM_WOODLAND_ELF, PM_DWARF, PM_HOBBIT
								};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_ORC_CAPTAIN:{
			int prisoners[] = {PM_GNOME, PM_GNOME, PM_GNOME, PM_GNOME, PM_GNOME_LORD, PM_GNOME_LADY, PM_TINKER_GNOME, PM_GNOMISH_WIZARD,
									PM_GREEN_ELF, PM_GREEN_ELF, PM_GREY_ELF, PM_GREY_ELF, PM_ELF_LORD, PM_ELF_LADY,
									PM_NURSE, PM_MAID, PM_WATCHMAN, PM_WATCHMAN, PM_WATCHMAN,
									PM_HEDROW_WARRIOR, PM_HEDROW_WARRIOR, PM_DROW_CAPTAIN, PM_DROW_CAPTAIN, PM_DROW_MATRON,
									PM_HOBBIT, PM_HOBBIT, PM_HOBBIT, PM_HOBBIT,
									PM_DWARF, PM_DWARF, PM_DWARF_CLERIC, PM_DWARF_LORD,
									PM_DEMINYMPH
								};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_URUK_CAPTAIN:{
			int prisoners[] = {PM_GNOME, PM_GNOME, PM_GNOME, PM_GNOME, PM_GNOME_LORD, PM_GNOME_LADY, PM_TINKER_GNOME, PM_GNOMISH_WIZARD,
									PM_GREEN_ELF, PM_GREEN_ELF, PM_GREY_ELF, PM_GREY_ELF, PM_ELF_LORD, PM_ELF_LADY,
									PM_NURSE, PM_MAID, PM_WATCHMAN, PM_WATCHMAN, PM_WATCH_CAPTAIN,
									PM_NOBLEMAN, PM_NOBLEWOMAN, PM_RANGER, PM_RANGER, PM_ROGUE, PM_WIZARD, PM_KNIGHT, PM_KNIGHT,
									PM_HOBBIT, PM_HOBBIT, PM_HOBBIT, PM_HOBBIT,
									PM_DWARF, PM_DWARF, PM_DWARF_CLERIC, PM_DWARF_LORD,
									PM_DRYAD, PM_NAIAD, PM_OREAD, PM_DEMINYMPH
								};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_ORC_OF_THE_AGES_OF_STARS:{
			int prisoners[] = {
									PM_ELF_LORD, PM_ELF_LADY, PM_ELVENKING, PM_ELVENQUEEN,
									PM_WATCH_CAPTAIN, PM_WATCH_CAPTAIN,
									PM_HEDROW_BLADEMASTER, PM_DROW_CAPTAIN, PM_DROW_MATRON, PM_UNEARTHLY_DROW,
									PM_DWARF_CLERIC, PM_DWARF_LORD, PM_DWARF_QUEEN, PM_DWARF_KING,
									PM_NOBLEMAN, PM_NOBLEWOMAN, PM_RANGER, PM_RANGER, PM_ROGUE, PM_WIZARD, PM_KNIGHT, PM_KNIGHT, PM_VALKYRIE,
									PM_DEMINYMPH,
									PM_JUSTICE_ARCHON, PM_SWORD_ARCHON, PM_SHIELD_ARCHON, PM_TRUMPET_ARCHON,
									PM_MOVANIC_DEVA, PM_MONADIC_DEVA, PM_ASTRAL_DEVA, PM_GRAHA_DEVA,
									PM_LILLEND,
									PM_COURE_ELADRIN, PM_NOVIERE_ELADRIN, PM_BRALANI_ELADRIN, PM_FIRRE_ELADRIN, PM_SHIERE_ELADRIN, PM_GHAELE_ELADRIN, PM_TULANI_ELADRIN
								};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_GNOME_KING:
		case PM_GNOME_QUEEN:{
			int prisoners[] = {
				PM_MINER, PM_MINER, PM_WATCHMAN, PM_WATCHMAN, PM_WATCH_CAPTAIN, 
				PM_WOODLAND_ELF, PM_WOODLAND_ELF, PM_GREEN_ELF,
				PM_HOBBIT, PM_HOBBIT,
				PM_KOBOLD, PM_LARGE_KOBOLD,
			};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_TITAN:{
			int prisoners[] = {
									PM_JUSTICE_ARCHON, PM_SWORD_ARCHON, PM_SHIELD_ARCHON, PM_TRUMPET_ARCHON, PM_WARDEN_ARCHON, PM_THRONE_ARCHON,
									PM_MOVANIC_DEVA, PM_MONADIC_DEVA, PM_ASTRAL_DEVA, PM_GRAHA_DEVA, PM_SURYA_DEVA,
									PM_LILLEND,
									PM_COURE_ELADRIN, PM_NOVIERE_ELADRIN, PM_BRALANI_ELADRIN, PM_FIRRE_ELADRIN, PM_SHIERE_ELADRIN, PM_GHAELE_ELADRIN, PM_DRACAE_ELADRIN,
									PM_INCUBUS, PM_ERINYS, PM_LILITU, PM_DAUGHTER_OF_BEDLAM, PM_UNEARTHLY_DROW, PM_MARILITH, PM_PIT_FIEND, PM_FALLEN_ANGEL
								};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_DEEPEST_ONE:{
			int prisoners[] = {
									PM_HOBBIT, PM_HOBBIT,
									PM_NURSE, PM_MAID, PM_WATCHMAN, PM_WATCHMAN, PM_WATCH_CAPTAIN, PM_WATCH_CAPTAIN,
									PM_ARCHEOLOGIST, PM_ARCHEOLOGIST, PM_BARBARIAN, PM_BARBARIAN, PM_BARD, PM_BARD, PM_HEALER, PM_HEALER,
									PM_MONK, PM_MONK, PM_MADMAN, PM_MADMAN, PM_MADWOMAN, PM_MADWOMAN, PM_PRIEST, PM_PRIESTESS, PM_PIRATE, PM_PIRATE,
									PM_NOBLEMAN, PM_NOBLEWOMAN, PM_RANGER, PM_RANGER, PM_ROGUE, PM_ROGUE, PM_WIZARD, PM_WIZARD, PM_KNIGHT, PM_KNIGHT,
									PM_VALKYRIE, PM_VALKYRIE, PM_SAMURAI, PM_SAMURAI, PM_TOURIST, PM_TOURIST,
									PM_DEMINYMPH
								};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_OGRE_KING:{
			int prisoners[] = {
									PM_ELF_LORD, PM_ELF_LADY, PM_ELVENKING, PM_ELVENQUEEN,
									PM_WATCH_CAPTAIN, PM_WATCH_CAPTAIN,
									PM_HEDROW_BLADEMASTER, PM_DROW_CAPTAIN, PM_DROW_MATRON, PM_UNEARTHLY_DROW,
									PM_DWARF_CLERIC, PM_DWARF_LORD, PM_DWARF_QUEEN, PM_DWARF_KING,
									PM_NOBLEMAN, PM_NOBLEWOMAN, PM_RANGER, PM_RANGER, PM_ROGUE, PM_WIZARD, PM_KNIGHT, PM_KNIGHT, PM_VALKYRIE,
									PM_DEMINYMPH,
									PM_JUSTICE_ARCHON, PM_SWORD_ARCHON, PM_SHIELD_ARCHON,
									PM_MOVANIC_DEVA, PM_MONADIC_DEVA, PM_ASTRAL_DEVA,
									PM_LILLEND,
									PM_COURE_ELADRIN, PM_NOVIERE_ELADRIN, PM_BRALANI_ELADRIN, PM_FIRRE_ELADRIN, PM_SHIERE_ELADRIN, PM_GHAELE_ELADRIN
								};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_VAMPIRE_LADY:
		case PM_VAMPIRE_LORD:{
			int prisoners[] = {
									PM_WOODLAND_ELF, PM_GREEN_ELF, PM_GREY_ELF, PM_ELF_LADY,
									PM_NURSE, PM_MAID, PM_WATCHMAN, PM_WATCHMAN, PM_WATCH_CAPTAIN, PM_WATCH_CAPTAIN,
									PM_DROW_CAPTAIN, PM_DROW_CAPTAIN, PM_DROW_MATRON, PM_SPROW, PM_DRIDER,
									PM_HOBBIT, PM_HOBBIT,
									PM_ARCHEOLOGIST, PM_BARBARIAN, PM_BARD, PM_HEALER,
									PM_MONK, PM_MADWOMAN, PM_PRIESTESS, PM_PIRATE,
									PM_NOBLEWOMAN, PM_RANGER, PM_ROGUE, PM_WIZARD, PM_KNIGHT,
									PM_VALKYRIE, PM_SAMURAI, PM_TOURIST,
									PM_DWARF, PM_DWARF, PM_DWARF_CLERIC,
									PM_DEMINYMPH
								};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_ELVENKING:
		case PM_ELVENQUEEN:{
			int prisoners[] = {
									PM_WATCHMAN, PM_WATCHMAN, PM_WATCH_CAPTAIN,
									PM_KNIGHT, PM_KNIGHT, PM_NOBLEMAN, PM_NOBLEWOMAN,
									PM_HEDROW_WARRIOR, PM_HEDROW_WARRIOR, PM_DROW_CAPTAIN, PM_DROW_CAPTAIN, PM_DROW_MATRON,
									PM_HOBBIT, PM_HOBBIT, PM_HOBBIT, PM_HOBBIT,
									PM_DWARF, PM_DWARF, PM_DWARF_CLERIC, PM_DWARF_LORD,
									PM_DWARF, PM_DWARF, PM_DWARF_CLERIC, PM_DWARF_LORD,
									PM_HILL_ORC, PM_ORC_SHAMAN, PM_ORC_CAPTAIN, PM_ANGBAND_ORC,
									PM_UNEARTHLY_DROW, PM_UNEARTHLY_DROW, PM_LILITU, PM_MARILITH,
									PM_MIRKWOOD_SPIDER, PM_MIRKWOOD_SPIDER, PM_MIRKWOOD_ELDER,
									PM_DEMINYMPH
								};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_DROW_MATRON:{
			int prisoners[] = {
									PM_GREEN_ELF, PM_GREEN_ELF, PM_GREY_ELF, PM_GREY_ELF, PM_ELF_LORD, PM_ELF_LADY,
									PM_GREEN_ELF, PM_GREEN_ELF, PM_GREY_ELF, PM_GREY_ELF, PM_ELF_LORD, PM_ELF_LADY,
									PM_NURSE, PM_MAID, PM_WATCHMAN, PM_WATCHMAN, PM_WATCH_CAPTAIN,
									PM_HEDROW_WARRIOR, PM_HEDROW_WARRIOR, PM_DROW_CAPTAIN, PM_DROW_CAPTAIN, PM_DROW_MATRON,
									PM_DRIDER, PM_PRIESTESS_OF_GHAUNADAUR, PM_STJARNA_ALFR, PM_STJARNA_ALFR,
									PM_HOBBIT, PM_HOBBIT, PM_HOBBIT, PM_HOBBIT,
									PM_DWARF, PM_DWARF, PM_DWARF_CLERIC, PM_DWARF_LORD,
									PM_DWARF, PM_DWARF, PM_DWARF_CLERIC, PM_DWARF_LORD,
									PM_HILL_ORC, PM_ORC_SHAMAN, PM_ORC_CAPTAIN, PM_ANGBAND_ORC,
									PM_DEMINYMPH
								};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_EMBRACED_DROWESS:{
			int prisoners[] = {
									PM_GREEN_ELF, PM_GREEN_ELF, PM_GREY_ELF, PM_GREY_ELF, PM_ELF_LORD, PM_ELF_LADY,
									PM_HEDROW_WARRIOR, PM_HEDROW_WIZARD, PM_DROW_CAPTAIN, PM_DROW_CAPTAIN, PM_DROW_MATRON,
									PM_HEDROW_WARRIOR, PM_HEDROW_WIZARD, PM_DROW_CAPTAIN, PM_DROW_CAPTAIN, PM_DROW_MATRON,
									PM_DRIDER, PM_PRIESTESS_OF_GHAUNADAUR, PM_STJARNA_ALFR,
									PM_DRIDER, PM_PRIESTESS_OF_GHAUNADAUR, PM_STJARNA_ALFR,
									PM_DWARF, PM_DWARF, PM_DWARF_CLERIC, PM_DWARF_LORD,
									PM_UNEARTHLY_DROW, PM_UNEARTHLY_DROW, PM_LILITU, PM_MARILITH,
									PM_DEMINYMPH
								};
			mtyp = ROLL_FROM(prisoners);
		}break;
		case PM_AVATAR_OF_LOLTH:{
			int prisoners[] = {
				PM_WOODLAND_ELF, PM_GREEN_ELF, PM_GREY_ELF, PM_GREY_ELF, PM_ELF_LORD, PM_ELVENKING,
				PM_WOODLAND_ELF, PM_GREEN_ELF, PM_GREY_ELF, PM_GREY_ELF, PM_ELF_LADY, PM_ELVENQUEEN,
				PM_ALABASTER_ELF, PM_ALABASTER_ELF, PM_ALABASTER_ELF_ELDER,
				PM_HEDROW_WARRIOR, PM_HEDROW_WIZARD, PM_HEDROW_BLADEMASTER, PM_DROW_CAPTAIN, PM_DROW_MATRON,
				PM_HEDROW_WARRIOR, PM_HEDROW_WIZARD, PM_HEDROW_BLADEMASTER, PM_DROW_CAPTAIN, PM_DROW_MATRON,
				PM_SPROW, PM_DRIDER, PM_PRIESTESS_OF_GHAUNADAUR, PM_STJARNA_ALFR, PM_STJARNA_ALFR,
				PM_SPROW, PM_DRIDER, PM_PRIESTESS_OF_GHAUNADAUR, PM_STJARNA_ALFR, PM_STJARNA_ALFR,
				PM_HILL_ORC, PM_ORC_SHAMAN, PM_ORC_CAPTAIN, PM_ANGBAND_ORC,
				PM_DRYAD, PM_NAIAD, PM_OREAD, PM_SWAMP_NYMPH, PM_YUKI_ONNA, PM_THRIAE, PM_SELKIE, PM_OCEANID, PM_DEMINYMPH,
				PM_YURIAN, PM_FORMIAN_TASKMASTER,
				PM_YURIAN, PM_FORMIAN_TASKMASTER,
				PM_ICE_DEVIL, PM_NALFESHNEE, PM_MARILITH, PM_PIT_FIEND, PM_FALLEN_ANGEL,
				PM_ELOCATOR,
				PM_JUSTICE_ARCHON, PM_SWORD_ARCHON, PM_SHIELD_ARCHON, PM_TRUMPET_ARCHON, PM_WARDEN_ARCHON, PM_THRONE_ARCHON,
				PM_MOVANIC_DEVA, PM_MONADIC_DEVA, PM_ASTRAL_DEVA, PM_GRAHA_DEVA, PM_SURYA_DEVA,
				PM_LILLEND, PM_ANGEL, PM_ALEAX,
				PM_COURE_ELADRIN, PM_NOVIERE_ELADRIN, PM_BRALANI_ELADRIN, PM_FIRRE_ELADRIN, PM_SHIERE_ELADRIN, PM_GHAELE_ELADRIN, 
				PM_TULANI_ELADRIN, PM_GAE_ELADRIN, PM_BRIGHID_ELADRIN, PM_UISCERRE_ELADRIN, PM_CAILLEA_ELADRIN, PM_KUKER
			};
			mtyp = ROLL_FROM(prisoners);
		}break;
	}
	if(mtyp == PM_DRACAE_ELADRIN){
		if(dungeon_topology.eprecursor_typ != PRE_DRACAE){
			mtyp = PM_TULANI_ELADRIN;
		}
		if(dungeon_topology.eprecursor_typ == PRE_POLYP){
			polyps = TRUE;
		}
	}
	if(mtyp == PM_TULANI_ELADRIN && kingtype != PM_AVATAR_OF_LOLTH){
		switch(dungeon_topology.alt_tulani){
			case GAE_CASTE:
				mtyp = PM_GAE_ELADRIN;
			break;
			case BRIGHID_CASTE:
				mtyp = PM_BRIGHID_ELADRIN;
			break;
			case UISCERRE_CASTE:
				mtyp = PM_UISCERRE_ELADRIN;
			break;
			case CAILLEA_CASTE:
				mtyp = PM_CAILLEA_ELADRIN;
			break;
		}
	}
	if(mtyp != NON_PM){
		int equipLevel = ((kingtype == PM_TITAN || kingtype == PM_EMBRACED_DROWESS || kingtype == PM_AVATAR_OF_LOLTH || kingtype == PM_DEEPEST_ONE || kingtype == PM_ORC_OF_THE_AGES_OF_STARS) ? MM_GOODEQUIP : 0);
		mon = makemon(&mons[mtyp], sx, sy, NO_MM_FLAGS|equipLevel);
		if(mon){
			if(mon->mtyp == PM_SURYA_DEVA){
				struct monst *blade;
				for(blade = fmon; blade; blade = blade->nmon) if(blade->mtyp == PM_DANCING_BLADE && mon->m_id == blade->mvar_suryaID){
					mongone(blade);
					break;
				}
			}
			if(mon->mtyp == PM_WITCH || mon->mtyp == PM_APPRENTICE_WITCH){
				struct monst *rat;
				for(rat = fmon; rat; rat = rat->nmon) if(rat->mtyp == PM_WITCH_S_FAMILIAR && mon->m_id == rat->mvar_witchID){
					mongone(rat);
					break;
				}
			}
			if(kingtype == PM_VAMPIRE_LORD){
				mon->female = TRUE;
				set_template(mon, VAMPIRIC);
			}
			if(kingtype == PM_VAMPIRE_LADY){
				mon->female = FALSE;
				set_template(mon, VAMPIRIC);
			}
			if(polyps){
				mon->ispolyp = TRUE;
				mongets(mon, MASK, NO_MKOBJ_FLAGS);
				mongets(mon, MASK, NO_MKOBJ_FLAGS);
				mongets(mon, MASK, NO_MKOBJ_FLAGS);
				mongets(mon, MASK, NO_MKOBJ_FLAGS);
				mongets(mon, MASK, NO_MKOBJ_FLAGS);
			}
		}
		return mon;
	}
	else return 0;
}

STATIC_OVL void
mkcell(sx, sy, chest, kingtype, dx, dy)
int sx,sy;
struct obj *chest;
int kingtype;
int dx,dy;
{
	struct monst *mon;
	struct obj *obj;
	levl[sx][sy].typ = CORR;
	unblock_point(sx,sy);
	levl[sx-dx][sy-dy].typ = IRONBARS;
	unblock_point(sx,sy);
	mon = prisoner(kingtype, sx, sy);
	if(mon){
		for(obj = mon->minvent; obj; obj = mon->minvent){
			mon->misc_worn_check &= ~obj->owornmask;
			update_mon_intrinsics(mon, obj, FALSE, FALSE);
			if (obj->owornmask & W_WEP){
				setmnotwielded(mon,obj);
				MON_NOWEP(mon);
			}
			if (obj->owornmask & W_SWAPWEP){
				setmnotwielded(mon,obj);
				MON_NOSWEP(mon);
			}
			obj->owornmask = 0L;
			obj_extract_self(obj);
			add_to_container(chest, obj);
		}
		(void)mongets(mon, SHACKLES, NO_MKOBJ_FLAGS);
		mon->entangled = SHACKLES;
	}
}

void
fill_zoo(sroom)
struct mkroom *sroom;
{
	struct monst *mon;
	register int sx,sy,i;
	int sh, tx, ty, goldlim, type = sroom->rtype;
	int rmno = (sroom - rooms) + ROOMOFFSET;
	int ctype = 0, tries = 0;
	coord mm;
	int zlevel, minmlev, maxmlev;
	
	zlevel = level_difficulty();
	/* determine the level of the weakest monster to make. */
	minmlev = zlevel / 3;
	/* determine the level of the strongest monster to make. */
	maxmlev = (zlevel + u.ulevel) / 2;

#ifdef GCC_WARN
	tx = ty = goldlim = 0;
#endif

	sh = sroom->fdoor;
	switch(type) {
		case GARDEN:
			mkgarden(sroom);
			/* mkgarden() sets flags and we don't want other fillings */
		return; 
		case LIBRARY:
			mklibrary(sroom);
			/* mklibrary() sets flags and we don't want other fillings */
		return;
		case ARMORY:
			mkarmory(sroom);
			/* mkarmory() sets flags and we don't want other fillings */
		return;
	    case ELSHAROOM:{
			coord pos;
			struct permonst *pmon;
			struct monst *mtmp;
			int i = rnd(3), tried = 0;
			while ((tried++ < 50) && (i > 0) && somexy(sroom, &pos)) {
				if (!MON_AT(pos.x, pos.y)) {
					switch(rnd(4)){
						case 1: pmon = &mons[PM_NAIAD]; break;
						case 2: pmon = &mons[PM_NOVIERE_ELADRIN]; break;
						case 3: pmon = &mons[PM_COURE_ELADRIN]; break;
						case 4: pmon = &mons[PM_DEEP_ONE]; break;
					}
					mtmp = makemon(pmon, pos.x,pos.y, NO_MM_FLAGS);
					if (mtmp){
						mtmp->msleeping = 0;
						mtmp->mcanmove = 1;
						mtmp->mpeaceful = 1;
						set_malign(mtmp);
					}
					i--;
				}
			}
		}return;
	    case COURT:
		if(level.flags.is_maze_lev) {
		    for(tx = sroom->lx; tx <= sroom->hx; tx++)
			for(ty = sroom->ly; ty <= sroom->hy; ty++)
			    if(IS_THRONE(levl[tx][ty].typ))
				goto throne_placed;
		}
		i = 100;
		do {	/* don't place throne on top of stairs */
			(void) somexy(sroom, &mm);
			tx = mm.x; ty = mm.y;
		} while (occupied((xchar)tx, (xchar)ty) && --i > 0);
		
		levl[tx][ty].typ = THRONE;
		if(In_outlands(&u.uz)){
			ctype = PM_AURUMACH_RILMANI;
			mon = 0;
			if(!toostrong(ctype,maxmlev+5))
				mon = makemon(&mons[ctype], tx, ty, NO_MM_FLAGS|MM_NOCOUNTBIRTH);
			if(mon) {
				mon->msleeping = 1;
				if(ctype == PM_DROW_MATRON || ctype == PM_EMBRACED_DROWESS){
					set_curhouse(mon->mfaction);
				}
			}
		} else if(rn2(4)){
			/* recalculae minlev to be stricter about weak throne monsters */
			minmlev = zlevel * 2  / 3;
			/* allow mildly out-of-depth lords */
			maxmlev += 5;
			
			static const int kingnums[] = {
				PM_DWARF_KING,
				PM_DWARF_QUEEN,
				PM_KOBOLD_LORD,
				PM_ORC_CAPTAIN,
				PM_URUK_CAPTAIN,
				PM_ORC_OF_THE_AGES_OF_STARS,
				PM_GNOME_KING,
				PM_GNOME_QUEEN,
				PM_TITAN,
				PM_DEEPEST_ONE,
				PM_OGRE_KING,
				PM_VAMPIRE_LORD,
				PM_VAMPIRE_LADY,
				PM_ELVENKING,
				PM_ELVENQUEEN,
				PM_DROW_MATRON,
				PM_EMBRACED_DROWESS
			};
			
			do ctype = kingnums[rn2(SIZE(kingnums))];
			while((tooweak(ctype, minmlev) || toostrong(ctype,maxmlev)) && tries++ < 40);
			if(tries < 40){
				mon = makemon(&mons[ctype], tx, ty, NO_MM_FLAGS|MM_NOCOUNTBIRTH);
				if(mon) {
					mon->msleeping = 1;
					if (type==COURT) {
						//Set curhouse to coordinate equipment
						if(ctype == PM_DROW_MATRON || ctype == PM_EMBRACED_DROWESS){
							set_curhouse(mon->mfaction);
						}
						//Note: court monsters are always part of rodney's forces.
						set_faction(mon, YENDORIAN_FACTION);
						if(mon->mpeaceful){
							mon->mpeaceful = 0;
							set_malign(mon);
						}
					}
				}
			}
			else ctype = 0;
		} else ctype = 0;
		
	    throne_placed:
		break;
	    case BEEHIVE:
		tx = sroom->lx + (sroom->hx - sroom->lx + 1)/2;
		ty = sroom->ly + (sroom->hy - sroom->ly + 1)/2;
		if(sroom->irregular) {
		    /* center might not be valid, so put queen elsewhere */
		    if ((int) levl[tx][ty].roomno != rmno ||
			    levl[tx][ty].edge) {
			(void) somexy(sroom, &mm);
			tx = mm.x; ty = mm.y;
		    }
		}
		break;
	    case ZOO:
	    case LEPREHALL:
		goldlim = 500 * level_difficulty();
		break;
	}
	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	    for(sy = sroom->ly; sy <= sroom->hy; sy++) {
		if(type == COURT){
			if(ctype == PM_KOBOLD_LORD || ctype == PM_VAMPIRE_LORD || ctype == PM_VAMPIRE_LADY || ctype == PM_DROW_MATRON ||
				ctype == PM_EMBRACED_DROWESS || ctype == PM_DEEPEST_ONE || ctype == PM_ORC_OF_THE_AGES_OF_STARS
			){
				levl[tx][ty].lit = 0;
				sroom->rlit = 0;
			} else if(ctype != 0){
				levl[tx][ty].lit = 1;
				sroom->rlit = 1;
			}
		}
		if(sroom->irregular) {
		    if ((int) levl[sx][sy].roomno != rmno ||
			  levl[sx][sy].edge ||
			  (sroom->doorct &&
			   distmin(sx, sy, doors[sh].x, doors[sh].y) <= 1))
			continue;
		} else if(!SPACE_POS(levl[sx][sy].typ) ||
			  (sroom->doorct &&
			   ((sx == sroom->lx && doors[sh].x == sx-1) ||
			    (sx == sroom->hx && doors[sh].x == sx+1) ||
			    (sy == sroom->ly && doors[sh].y == sy-1) ||
			    (sy == sroom->hy && doors[sh].y == sy+1))))
		    continue;
		/* don't place monster on throne */
		if(type == COURT && IS_THRONE(levl[sx][sy].typ))
		    continue;
		if(!(Role_if(PM_NOBLEMAN) && In_quest(&u.uz) )){
			mon = makemon(
				(type == COURT) ? courtmon(ctype) :
				(type == BARRACKS) ? squadmon(&u.uz) :
				(type == MORGUE) ? morguemon() :
				(type == BEEHIVE) ?
				(sx == tx && sy == ty ? &mons[PM_QUEEN_BEE] :
				 &mons[PM_KILLER_BEE]) :
				(type == LEPREHALL) ? &mons[PM_LEPRECHAUN] :
				(type == COCKNEST) ? &mons[PM_COCKATRICE] :
				(type == ANTHOLE) ? antholemon() :
				(struct permonst *) 0,
			   sx, sy, NO_MM_FLAGS|MM_NOCOUNTBIRTH);
			if(mon) {
				mon->msleeping = 1;
				if(type == ZOO){
					//Note: zoo monsters are always part of rodney's forces.
					set_faction(mon, YENDORIAN_FACTION);
				}
				if (type==COURT) {
					//Note: court monsters are always part of rodney's forces, even if they are angels.
					set_faction(mon, YENDORIAN_FACTION);
					if(mon->mpeaceful){
						mon->mpeaceful = 0;
						set_malign(mon);
					}
					if(ctype == PM_EMBRACED_DROWESS && (mon->mtyp == PM_DROW_CAPTAIN || mon->mtyp == PM_DROW_MATRON)){
						set_template(mon, M_BLACK_WEB);
					}
				}
			}
		}
		switch(type) {
		    case ZOO:
		    case LEPREHALL:
			if(sroom->doorct)
			{
			    int distval = dist2(sx,sy,doors[sh].x,doors[sh].y);
			    i = sq(distval);
			}
			else
			    i = goldlim;
			if(i >= goldlim) i = 5*level_difficulty();
			goldlim -= i;
			(void) mkgold((long) rn1(i, 10), sx, sy);
			break;
		    case MORGUE:
			if(!(Role_if(PM_NOBLEMAN) && In_quest(&u.uz) )){
			if(!rn2(5))
			    (void) mk_tt_object(CORPSE, sx, sy);
			if(!rn2(10))	/* lots of treasure buried with dead */
			    (void) mksobj_at((rn2(3)) ? BOX : CHEST, sx, sy, NO_MKOBJ_FLAGS);
			if (!rn2(5))
			    make_grave(sx, sy, (char *)0);
			} else if(rn2(2)) make_grave(sx, sy, (char *)0);
			break;
		    case BEEHIVE:
			if(!rn2(3))
			    (void) mksobj_at(LUMP_OF_ROYAL_JELLY, sx, sy, NO_MKOBJ_FLAGS);
			if(rn2(3))
			    (void) mksobj_at(HONEYCOMB,  sx, sy, NO_MKOBJ_FLAGS);
			break;
		    case BARRACKS:
			if(In_outlands(&u.uz)){
				if(!rn2(10)){
					int i = 0;
					struct obj *chest, *otmp;
					chest = mksobj_at((rn2(3)) ? BOX : CHEST, sx, sy, NO_MKOBJ_FLAGS);
					set_material_gm(chest, IRON);
					fix_object(chest);
					for(i = rnd(4); i > 0; i--) switch(rnd(4)){
						case 1:
							if(find_riwant()){
								otmp = mksobj(find_riwant(), NO_MKOBJ_FLAGS);
								set_material_gm(otmp, IRON);
								fix_object(otmp);
								add_to_container(chest, otmp);
							}
						break;
						case 2:
							if(find_iring() > 0){
								otmp = mksobj(find_iring(), NO_MKOBJ_FLAGS);
								set_material_gm(otmp, IRON);
								fix_object(otmp);
								add_to_container(chest, otmp);
							}
						break;
						case 3:
							otmp = mksobj(UPGRADE_KIT, MKOBJ_NOINIT);
							set_material_gm(otmp, IRON);
							fix_object(otmp);
							add_to_container(chest, otmp);
						break;
						case 4:
							switch(rn2(4)){
								case 0:
									otmp = mksobj(WHISTLE, MKOBJ_NOINIT);
									fix_object(otmp);
								break;
								case 1:
									otmp = mksobj(MAGIC_WHISTLE, MKOBJ_NOINIT);
									set_material_gm(otmp, IRON);
									fix_object(otmp);
								break;
								case 2:
									otmp = mksobj(BELL, MKOBJ_NOINIT);
									set_material_gm(otmp, IRON);
									fix_object(otmp);
								break;
								case 3:
									otmp = mksobj(BUGLE, MKOBJ_NOINIT);
									set_material_gm(otmp, IRON);
									fix_object(otmp);
								break;
							}
							add_to_container(chest, otmp);
						break;
					}
					chest->owt = weight(chest);
				}
			} else if(!rn2(20)){
				/* the payroll and some loot */
				struct obj *chest, *gold;
				gold = mksobj(GOLD_PIECE, NO_MKOBJ_FLAGS);
				gold->quan = (long) rn1(9 * level_difficulty(), level_difficulty()); //1 - 10
				u.spawnedGold += gold->quan;
				gold->owt = weight(gold);
			    chest = mksobj_at((rn2(3)) ? BOX : CHEST, sx, sy, NO_MKOBJ_FLAGS);
				add_to_container(chest, gold);
				chest->owt = weight(chest);
			}
			break;
		    case COCKNEST:
			if(!rn2(3)) {
			    struct obj *sobj = mk_tt_object(STATUE, sx, sy);

			    if (sobj) {
				for (i = rn2(5); i; i--)
				    (void) add_to_container(sobj,
						mkobj(RANDOM_CLASS, FALSE));
				sobj->owt = weight(sobj);
			    }
			}
			break;
		    case ANTHOLE:
			if(In_law(&u.uz)){
				struct obj *otmp;
				switch(rn2(5)){
					case 0:
						(void) mksobj_at(FOOD_RATION, sx, sy, NO_MKOBJ_FLAGS);
					break;
					case 1:
						switch(rn2(6)){
							case 0:
								(void) mksobj_at(FORTUNE_COOKIE, sx, sy, NO_MKOBJ_FLAGS);
							break;
							case 1:
								(void) mksobj_at(CANDY_BAR, sx, sy, NO_MKOBJ_FLAGS);
							break;
							case 2:
								(void) mksobj_at(APPLE, sx, sy, NO_MKOBJ_FLAGS);
							break;
							case 3:
								(void) mksobj_at(ORANGE, sx, sy, NO_MKOBJ_FLAGS);
							break;
							case 4:
								(void) mksobj_at(PEAR, sx, sy, NO_MKOBJ_FLAGS);
							break;
							case 5:
								(void) mksobj_at(CREAM_PIE, sx, sy, NO_MKOBJ_FLAGS);
							break;
						}
					break;
					case 2:
						otmp = mksobj_at(SLIME_MOLD, sx, sy, NO_MKOBJ_FLAGS);
						otmp->spe = fruitadd("loaf of baked bread");
					break;
					case 3:
						otmp = mksobj_at(SLIME_MOLD, sx, sy, NO_MKOBJ_FLAGS);
						otmp->spe = fruitadd("honey drop");
					break;
					case 4:
						otmp = mksobj_at(TIN, sx, sy, NO_MKOBJ_FLAGS);
						switch(rn2(4)){
							case 0:
								otmp->corpsenm = PM_LICHEN;
							break;
							case 1:
								otmp->corpsenm = PM_DUNGEON_FERN_SPROUT;
							break;
							case 2:
								otmp->corpsenm = PM_SUNFLOWER;
							break;
							case 3:
								otmp->spe = 1;
							break;
						}
					break;
				}
			} else {
				if(!rn2(3))
					(void) mkobj_at(FOOD_CLASS, sx, sy, NO_MKOBJ_FLAGS);
			}
			break;
		}
	    }
	switch (type) {
	      case COURT:
		{
		  struct obj *chest, *gold;
		  (void) somexy(sroom, &mm);
		  gold = mksobj(GOLD_PIECE, NO_MKOBJ_FLAGS);
		  gold->quan = (long) rn1(50 * level_difficulty(), 10);
		  u.spawnedGold += gold->quan;
		  gold->owt = weight(gold);
		  /* the royal coffers */
		  chest = mksobj_at(CHEST, mm.x, mm.y, NO_MKOBJ_FLAGS);
		  chest->spe = 2; /* so it can be found later */
		  add_to_container(chest, gold);
		  chest->owt = weight(chest);
		  level.flags.has_court = 1;

		  sy = sroom->ly -2;
		  for(sx = sroom->lx+1; sx <= sroom->hx-1; sx++){
			if(!isok(sx, sy))
				break;//Won't get any less off the map.
			if(cell_spot(sx, sy) && rn2(4)){
				mkcell(sx,sy,chest,ctype,0,-1);
			}
		  }
		  sy = sroom->hy +2;
		  for(sx = sroom->lx+1; sx <= sroom->hx-1; sx++){
			if(!isok(sx, sy))
				break;//Won't get any less off the map.
			if(cell_spot(sx, sy) && rn2(4)){
				mkcell(sx,sy,chest,ctype,0,+1);
			}
		  }
		  sx = sroom->lx -2;
		  for(sy = sroom->ly+1; sy <= sroom->hy-1; sy++) {
			if(!isok(sx, sy))
				break;//Won't get any less off the map.
			if(cell_spot(sx, sy) && rn2(4)){
				mkcell(sx,sy,chest,ctype,-1,0);
			}
		  }
		  sx = sroom->hx +2;
		  for(sy = sroom->ly+1; sy <= sroom->hy-1; sy++) {
			if(!isok(sx, sy))
				break;//Won't get any less off the map.
			if(cell_spot(sx, sy) && rn2(4)){
				mkcell(sx,sy,chest,ctype,+1,0);
			}
		  }
		  break;
		}
	      case BARRACKS:
		  level.flags.has_barracks = 1;
		  break;
	      case ZOO:
		  level.flags.has_zoo = 1;
		  break;
	      case MORGUE:
		  level.flags.has_morgue = 1;
		  break;
	      case SWAMP:
		  level.flags.has_swamp = 1;
		  break;
	      case BEEHIVE:
		  level.flags.has_beehive = 1;
		  break;
	}
	set_curhouse(0);
}

/* make a swarm of undead around mm */
void
mkundead(mm, revive_corpses, mm_flags, mfaction)
coord *mm;
boolean revive_corpses;
int mm_flags;
long mfaction;
{
	int cnt = (level_difficulty() + 1)/10 + rnd(5);
	struct permonst *mdat;
	struct obj *otmp;
	struct monst *mon = (struct monst *)0;
	coord cc;

	while (cnt--) {
		cc.x = mm->x+rn2(3)-1;
		cc.y = mm->y+rn2(3)-1;
		if(revive_corpses && isok(cc.x, cc.y) && (otmp = sobj_at(CORPSE, cc.x, cc.y)) && (mon = revive(otmp,FALSE))){
			if(!get_template(mon) && mtemplate_accepts_mtyp(ZOMBIFIED, mon->mtyp))
				set_template(mon, ZOMBIFIED);
		}
		else {
			mdat = morguemon();
			if (enexto(&cc, mm->x, mm->y, mdat))
				mon = makemon(mdat, cc.x, cc.y, mm_flags);
		}
		if(mon)
			set_faction(mon, mfaction);
	}
	level.flags.graveyard = TRUE;	/* reduced chance for undead corpse */
}

/* make a swarm of undead around mm */
void
mk_yellow_undead(mm, revive_corpses, mm_flags, mfaction)
coord *mm;
boolean revive_corpses;
int mm_flags;
long mfaction;
{
	int cnt = (level_difficulty() + 1)/10 + rnd(5);
	struct permonst *mdat;
	struct obj *otmp;
	struct monst *mon = (struct monst *)0;
	coord cc;

	while (cnt--) {
		cc.x = mm->x+rn2(3)-1;
		cc.y = mm->y+rn2(3)-1;
		if(revive_corpses && isok(cc.x, cc.y) && (otmp = sobj_at(CORPSE, cc.x, cc.y)) && (mon = revive(otmp,FALSE))){
			if(!get_template(mon) && mtemplate_accepts_mtyp(YELLOW_TEMPLATE, mon->mtyp))
				set_template(mon, YELLOW_TEMPLATE);
			set_faction(mon, YELLOW_FACTION);
		}
		else {
			yellow_dead();
		}
		if(mon)
			set_faction(mon, mfaction);
	}
	level.flags.graveyard = TRUE;	/* reduced chance for undead corpse */
}

STATIC_OVL struct permonst *
morguemon()
{
	register int i = rn2(100), hd = rn2(level_difficulty());

	if(hd > 10 && i < 10)
		return((Inhell || In_endgame(&u.uz)) ? mkclass(S_DEMON,G_NOHELL|G_HELL) :
							   mkclass(S_MUMMY, Inhell ? G_HELL : G_NOHELL));
//						       &mons[ndemon(A_NONE)]); Was this, but I think this was nullpointering
	if(hd > 8 && i > 85)
		return(mkclass(S_VAMPIRE, Inhell ? G_HELL : G_NOHELL));

	return((i < 20) ? &mons[PM_GHOST]
			: (i < 40) ? &mons[PM_WRAITH] : mkclass(S_ZOMBIE, Inhell ? G_HELL : G_NOHELL));
}

STATIC_OVL struct permonst *
antholemon()
{
	int mtyp;

	/* Same monsters within a level, different ones between levels */
	if(!In_law(&u.uz)){
		switch (rn2(3)) {
		default:	mtyp = PM_GIANT_ANT; break;
		case 0:		mtyp = PM_SOLDIER_ANT; break;
		case 1:		mtyp = PM_FIRE_ANT; break;
		}
	} else {
		switch ((level_difficulty() + ((long)u.ubirthday)) % 3) {
		default:	mtyp = PM_GIANT_ANT; break;
		case 0:		mtyp = PM_SOLDIER_ANT; break;
		case 1:		mtyp = PM_FIRE_ANT; break;
		}
	}
	return ((mvitals[mtyp].mvflags & G_GONE && !In_quest(&u.uz)) ?
			(struct permonst *)0 : &mons[mtyp]);
}

/** Create a special room with trees, fountains and nymphs.
 * @author Pasi Kallinen
 */
STATIC_OVL void
mkgarden(croom)
struct mkroom *croom; /* NULL == choose random room */
{
    register int tryct = 0;
    boolean maderoom = FALSE;
    coord pos;
    register int i, tried;

    while ((tryct++ < 25) && !maderoom) {
	register struct mkroom *sroom = croom ? croom : &rooms[rn2(nroom)];
	
	if (sroom->hx < 0 || (!croom && (sroom->rtype != OROOM ||
	    !sroom->rlit || has_upstairs(sroom) || has_dnstairs(sroom))))
	    	continue;

	sroom->rtype = GARDEN;
	maderoom = TRUE;
	level.flags.has_garden = 1;

	tried = 0;
	if(!rn2(20)){
		i = 1;
		while ((tried++ < 50) && (i > 0) && somexy(sroom, &pos)) {
			struct permonst *pmon;
			if (!MON_AT(pos.x, pos.y)) {
				struct monst *mtmp = makemon(&mons[PM_MOUTH_OF_THE_GOAT], pos.x,pos.y, NO_MM_FLAGS);
				i--;
			}
		}
	}
	if(rn2(2)){
		i = 1;
		while ((tried++ < 50) && (i > 0) && somexy(sroom, &pos)) {
			struct permonst *pmon;
			if (!MON_AT(pos.x, pos.y)) {
			    struct monst *mtmp = makemon(&mons[PM_WEEPING_ANGEL], pos.x,pos.y, NO_MM_FLAGS);
			    if (mtmp) {
				mtmp->msleeping = 1;
				mtmp->mvar3 = 1;
			    }
			    i--;
			}
		}
	}
	tried = 0;
	i = rnd(4);
	while ((tried++ < 50) && (i > 0) && somexy(sroom, &pos)) {
	    struct permonst *pmon;
	    if (!MON_AT(pos.x, pos.y) && (pmon = mkclass(S_NYMPH,G_NOHELL))) {
		struct monst *mtmp = makemon(pmon, pos.x,pos.y, NO_MM_FLAGS);
		if (mtmp) mtmp->msleeping = 1;
		i--;
	    }
	}
	tried = 0;
	i = rn1(5,3);
	while ((tried++ < 50) && (i > 0) && somexy(sroom, &pos)) {
	    struct permonst *pmon;
	    if (!MON_AT(pos.x, pos.y) && (pmon = mkclass(S_PLANT,G_HELL|G_NOHELL))) {
		struct monst *mtmp = makemon(pmon, pos.x,pos.y, NO_MM_FLAGS);
		if (mtmp) mtmp->msleeping = 1;
		i--;
	    }
	}
	tried = 0;
	i = rn1(3,3);
	while ((tried++ < 50) && (i > 0) && somexy(sroom, &pos)) {
	    if ((levl[pos.x][pos.y].typ == ROOM || levl[pos.x][pos.y].typ == CORR) && !MON_AT(pos.x,pos.y) &&
		!nexttodoor(pos.x,pos.y)) {
		if (rn2(3))
		  levl[pos.x][pos.y].typ = TREE;
		else {
		    levl[pos.x][pos.y].typ = FOUNTAIN;
			/* Is it a "blessed" fountain? (affects drinking from fountain) */
			if (!rn2(7)) levl[pos.x][pos.y].blessedftn = 1;
		    level.flags.nfountains++;
		}
		i--;
	    }
	}
	for(pos.x=sroom->lx-1; pos.x <= sroom->hx+1; pos.x++){
		for(pos.y=sroom->ly-1; pos.y <= sroom->hy+1; pos.y++){
			if(levl[pos.x][pos.y].typ == ROOM){
				levl[pos.x][pos.y].typ = GRASS;
			}
		}
	}
    }
}

STATIC_OVL void
mklibrary(croom)
struct mkroom *croom; /* NULL == choose random room */
{
    register int tryct = 0;
    boolean maderoom = FALSE;
    coord pos;
    register int i, tried;

    while ((tryct++ < 25) && !maderoom) {
	register struct mkroom *sroom = croom ? croom : &rooms[rn2(nroom)];
	
	if (sroom->hx < 0 || (!croom && (sroom->rtype != OROOM)))
	    	continue;

	sroom->rtype = LIBRARY;
	maderoom = TRUE;
	level.flags.has_library = 1;
	
	tried = 0;
	i = rnd(4);
	while ((tried++ < 50) && (i > 0) && somexy(sroom, &pos)) {
	    struct permonst *pmon;
	    if (!MON_AT(pos.x, pos.y)) {
			struct monst *mtmp = makemon(&mons[PM_LIVING_LECTERN], pos.x,pos.y, NO_MM_FLAGS);
			if (mtmp) mtmp->msleeping = 1;
			i--;
	    }
	}
	tried = 0;
	i = rn1(3,3);
	while ((tried++ < 50) && (i > 0) && somexy(sroom, &pos)) {
	    if (levl[pos.x][pos.y].typ == ROOM && !MON_AT(pos.x,pos.y) &&
		!nexttodoor(pos.x,pos.y)) {
		if (rn2(3))
		  levl[pos.x][pos.y].typ = POOL;
		else {
		    levl[pos.x][pos.y].typ = FOUNTAIN;
		    level.flags.nfountains++;
		}
		i--;
	    }
	}
	for(pos.x=sroom->lx-1; pos.x <= sroom->hx+1; pos.x++){
		for(pos.y=sroom->ly-1; pos.y <= sroom->hy+1; pos.y++){
			if(levl[pos.x][pos.y].typ >= STONE && levl[pos.x][pos.y].typ <= DBWALL){
				if(rn2(6)) mksobj_at(SPE_BLANK_PAPER, pos.x, pos.y, NO_MKOBJ_FLAGS);
				else mkobj_at(SPBOOK_CLASS, pos.x, pos.y, NO_MKOBJ_FLAGS);
			}
		}
	}
	}
}

STATIC_OVL void
mkarmory(croom)
struct mkroom *croom; /* NULL == choose random room */
{
    register int tryct = 0;
	register struct obj *otmp;
    boolean maderoom = FALSE;
    coord pos;
    register int i, tried;

    while ((tryct++ < 25) && !maderoom) {
	register struct mkroom *sroom = croom ? croom : &rooms[rn2(nroom)];
	
	if (sroom->hx < 0 || (!croom && (sroom->rtype != OROOM || (atleastxspace(sroom, 3) && !atleastxspace(sroom, 6)))))
	    	continue;

	sroom->rtype = ARMORY;
	maderoom = TRUE;
	level.flags.has_armory = 1;
	
	tried = 0;
	i = rnd(2);
	while ((tried++ < 50) && (i > 0) && somexy(sroom, &pos)) {
	    struct permonst *pmon;
	    if (!MON_AT(pos.x, pos.y)) {
			struct monst *mtmp = makemon(&mons[PM_RUST_MONSTER], pos.x,pos.y, NO_MM_FLAGS);
			if (mtmp) mtmp->msleeping = 1;
			i--;
	    }
	}
	tried = 0;
	i = rn2(3);
	while ((tried++ < 50) && (i > 0) && somexy(sroom, &pos)) {
	    struct permonst *pmon;
	    if (!MON_AT(pos.x, pos.y)) {
			struct monst *mtmp = makemon(&mons[PM_BROWN_PUDDING], pos.x,pos.y, NO_MM_FLAGS);
			if (mtmp) mtmp->mstrategy |= STRAT_WAITFORU;
			i--;
	    }
	}
	for(pos.x=sroom->lx; pos.x <= sroom->hx; pos.x++){
		for(pos.y=sroom->ly; pos.y <= sroom->hy; pos.y++){
			if(!rn2(8)){
				otmp = mkobj_at(WEAPON_CLASS, pos.x, pos.y, NO_MKOBJ_FLAGS);
				if(rn2(3) && (is_rustprone(otmp) || is_flammable(otmp))){
					otmp->oeroded = rn2(4);
				} else if(is_corrodeable(otmp) || is_rottable(otmp)){
					otmp->oeroded2 = rn2(4);
				}
				if(rn2(2)){
					otmp->spe = rn2(3)+rn2(3);
				}
			}
			if(!rn2(8)){
				otmp = mkobj_at(ARMOR_CLASS, pos.x, pos.y, NO_MKOBJ_FLAGS);
				if(rn2(3) && (is_rustprone(otmp) || is_flammable(otmp))){
					otmp->oeroded = rn2(4);
				} else if(is_corrodeable(otmp) || is_rottable(otmp)){
					otmp->oeroded2 = rn2(4);
				}
				if(rn2(2)){
					otmp->spe = rn2(3)+rn2(2);
				}
			}
			if(!rn2(4)){
				otmp = mkobj_at(WEAPON_CLASS, pos.x, pos.y, NO_MKOBJ_FLAGS);
				if(rn2(3) && (is_rustprone(otmp) || is_flammable(otmp))){
					otmp->oeroded = rn2(4);
				} else if(is_corrodeable(otmp) || is_rottable(otmp)){
					otmp->oeroded2 = rn2(4);
				}
			}
			if(!rn2(4)){
				otmp = mkobj_at(ARMOR_CLASS, pos.x, pos.y, NO_MKOBJ_FLAGS);
				if(rn2(3) && (is_rustprone(otmp) || is_flammable(otmp))){
					otmp->oeroded = rn2(4);
				} else if(is_corrodeable(otmp) || is_rottable(otmp)){
					otmp->oeroded2 = rn2(4);
				}
			}
		}
	}
	}
}

STATIC_OVL void
mkswamp()	/* Michiel Huisjes & Fred de Wilde */
{
	register struct mkroom *sroom;
	register int sx,sy,i,eelct = 0;

	for(i=0; i<5; i++) {		/* turn up to 5 rooms swampy */
		sroom = &rooms[rn2(nroom)];
		if(sroom->hx < 0 || sroom->rtype != OROOM ||
		   has_upstairs(sroom) || has_dnstairs(sroom))
			continue;

		/* satisfied; make a swamp */
		sroom->rtype = SWAMP;
		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if(!OBJ_AT(sx, sy) &&
		   !MON_AT(sx, sy) && !t_at(sx,sy) && !nexttodoor(sx,sy)) {
		    if((sx+sy)%2) {
			levl[sx][sy].typ = POOL;
			if(!eelct || !rn2(4)) {
			    /* mkclass() won't do, as we might get kraken */
			    (void) makemon(rn2(5) ? &mons[PM_GIANT_EEL]
						  : rn2(2) ? &mons[PM_PIRANHA]
						  : &mons[PM_ELECTRIC_EEL],
						sx, sy, NO_MM_FLAGS);
			    eelct++;
			}
		    } else {
				levl[sx][sy].typ = PUDDLE;
				if(!rn2(4))	/* swamps tend to be moldy */
					(void) makemon(&mons[!rn2(4) ? PM_GREEN_MOLD : 
										 !rn2(3) ? PM_BROWN_MOLD: 
										 !rn2(2) ? PM_YELLOW_MOLD:
										PM_RED_MOLD],
							sx, sy, NO_MM_FLAGS);
				else if(!rn2(6)) /* swamp ferns like swamps */
					(void) makemon(&mons[PM_SWAMP_FERN],
							sx, sy, NO_MM_FLAGS);
				else if(!rn2(6)) /* many phantom fungi */
					(void) makemon(&mons[PM_PHANTOM_FUNGUS],
							sx, sy, NO_MM_FLAGS);
			}
		}
		level.flags.has_swamp = 1;
	}
}

STATIC_OVL void
mkriver()	/* John Harris */
{
	register int center, width, prog, fill, edge;
	register int x, y;
	level.flags.has_river = 1;
	if (!rn2(4)) {      /* Horizontal river */
		center = rn2(ROWNO-12)+6;
		width = rn2(4)+4;
		for (prog = 1; prog<COLNO; prog++) {
			edge = TRUE;
			for (fill=center-(width/2); fill<=center+(width/2) ; fill++) {
				/* edge is true the first time through this loop and the last */
				liquify(prog, fill, edge);
				edge = (fill == (center+(width/2)-1));
			}
			if (!rn2(3)) {
				if (!rn2(2) && width >4) {width--;}
				else if (width <7) {width++;}
			}
			if (!rn2(3)) {
				if (!rn2(2) && (center-width/2) >1) {center--;}
				else if ((center+width/2) < ROWNO-1) {center++;}
			}
			/* Make sure river doesn't stray off map */
			if (center < 4) {center = 4;}
			if (center > (ROWNO-5)) {center = ROWNO-5;}
		}
	}
	else {      /* Vertical river */
		center = rn2(COLNO-14)+7;
		width = rn2(4)+5;
		for (prog = 0; prog<ROWNO; prog++) {
			edge = TRUE;
			for (fill=center-(width/2); fill<=center+(width/2) ; fill++) {
				liquify(fill, prog, edge);
				edge = (fill == (center+(width/2)-1));
			}
			if (!rn2(3)) {
				if (!rn2(2) && width >5) {width--;}
				else if (width <8) {width++;}
			}
			if (!rn2(3)) {
				if (!rn2(2) && (center-width/2) >1) {center--;}
				else if ((center+width/2) < ROWNO-1) {center++;}
			}
			/* Sanity checking */
			if (center < 5) {center = 5;}
			if (center > (COLNO-6)) {center = COLNO-6;}
		}
	}
}

STATIC_OVL void
mkneuriver()	/* John Harris */
{
	register int center, width, prog, fill, edge;
	register int x, y;
	level.flags.has_river = 1;
	if (!rn2(4)) {      /* Horizontal river */
		center = rn2(ROWNO-12)+6;
		width = rn2(4)+4;
		for (prog = 1; prog<COLNO; prog++) {
			edge = TRUE;
			for (fill=center-(width/2); fill<=center+(width/2) ; fill++) {
				/* edge is true the first time through this loop and the last */
				neuliquify(prog, fill, edge);
				edge = (fill == (center+(width/2)-1));
			}
			if (!rn2(3)) {
				if (!rn2(2) && width >4) {width--;}
				else if (width <7) {width++;}
			}
			if (!rn2(3)) {
				if (!rn2(2) && (center-width/2) >1) {center--;}
				else if ((center+width/2) < ROWNO-1) {center++;}
			}
			/* Make sure river doesn't stray off map */
			if (center < 4) {center = 4;}
			if (center > (ROWNO-5)) {center = ROWNO-5;}
		}
	}
	else {      /* Vertical river */
		center = rn2(COLNO-14)+7;
		width = rn2(4)+5;
		for (prog = 0; prog<ROWNO; prog++) {
			edge = TRUE;
			for (fill=center-(width/2); fill<=center+(width/2) ; fill++) {
				neuliquify(fill, prog, edge);
				edge = (fill == (center+(width/2)-1));
			}
			if (!rn2(3)) {
				if (!rn2(2) && width >5) {width--;}
				else if (width <8) {width++;}
			}
			if (!rn2(3)) {
				if (!rn2(2) && (center-width/2) >1) {center--;}
				else if ((center+width/2) < ROWNO-1) {center++;}
			}
			/* Sanity checking */
			if (center < 5) {center = 5;}
			if (center > (COLNO-6)) {center = COLNO-6;}
		}
	}
}

STATIC_OVL void
mkforest12river()	/* John Harris */
{
	register int center, width, prog, fill, edge;
	register int x, y;
	level.flags.has_river = 1;
	center = rn2(COLNO/4-14)+7+3*COLNO/4;
	width = rn2(4)+5;
	for (prog = 0; prog<ROWNO; prog++) {
		edge = TRUE;
		for (fill=center-(width/2); fill<=center+(width/2) ; fill++) {
			chaliquify(fill, prog, edge);
			edge = (fill == (center+(width/2)-1));
		}
		if (!rn2(3)) {
			if (!rn2(2) && width >5) {width--;}
			else if (width <8) {width++;}
		}
		if (!rn2(3)) {
			if (!rn2(2) && (center-width/2) >1) {center--;}
			else if ((center+width/2) < ROWNO-1) {center++;}
		}
		/* Sanity checking */
		if (center < 5) {center = 5;}
		if (center > (COLNO-6)) {center = COLNO-6;}
	}
}

/* This isn't currently used anywhere. It liquifies the whole level. */
STATIC_OVL void
mksea()	/* John Harris */
{
	register int x, y;
	/*level.flags.has_river = 1;*/
	for (x=1 ; x <= COLNO-1 ; x++) {
		for (y=1 ; y <= ROWNO-1 ; y++) {
			liquify(x,y,FALSE);
		};
	}
}

STATIC_OVL void
liquify(x, y, edge)
register int x, y;
register int edge; /* Allows room walls to intrude slightly into river. */
{
	register int typ = levl[x][y].typ;
	register int monster = PM_JELLYFISH;
	/* Don't liquify shop walls */
	if (level.flags.has_shop && *in_rooms(x, y, SHOPBASE)) {return;}
	if (typ == STONE || (IS_WALL(typ) && !edge && rn2(3)))
		{levl[x][y].typ = POOL;}
	else if ((typ == SCORR || typ == CORR || IS_DOOR(typ)
					|| typ == SDOOR) && !IS_WALL(typ)) {
			levl[x][y].typ = ROOM;
	}
	/* Leave boulders scattered around, dislodged by erosion.
		Also, because they are fun to push into water.  Plunk! */
	if (levl[x][y].typ == ROOM && !rn2(13))
		(void) mksobj_at(BOULDER, x, y, NO_MKOBJ_FLAGS);
	/* Sea monsters */
	if (levl[x][y].typ == POOL){
		if(!rn2(85-depth(&u.uz))) {
			if (depth(&u.uz) > 19 && !rn2(3)) {monster = PM_ELECTRIC_EEL;}
				else if (depth(&u.uz) > 15 && !rn2(3)) {monster = PM_GIANT_EEL;}
				else if (depth(&u.uz) > 11 && !rn2(2)) {monster = PM_SHARK;}
				else if (depth(&u.uz) > 7 && rn2(4)) {monster = PM_PIRANHA;}
			(void) makemon(&mons[monster], x, y, NO_MM_FLAGS);
		}
		if(!rn2(140-depth(&u.uz))){
			mkobj_at(RANDOM_CLASS, x, y, NO_MKOBJ_FLAGS);
		}
		else if(!rn2(100-depth(&u.uz))){
		  (void) mkgold((long) rn1(10 * level_difficulty(),10), x, y);
		}
	}
	/* Underground rivers are relatively open spaces, so light them. */
	levl[x][y].lit = 1;
}

STATIC_OVL void
neuliquify(x, y, edge)
register int x, y;
register int edge; /* Allows room walls to intrude slightly into river. */
{
	register int typ = levl[x][y].typ;
	register int monster = PM_JELLYFISH;
	/* Don't liquify shop walls */
	if (level.flags.has_shop && *in_rooms(x, y, SHOPBASE)) {return;}
	
	/* Don't liquify other level features */
	if(typ != TREE && typ != GRASS && typ != SOIL && typ != SAND)
		return;
	
	if (typ!=TREE || (!edge && rn2(6))){
		if(typ == TREE) levl[x][y].typ = POOL;
		else levl[x][y].typ = PUDDLE;
	}
	// else if ((typ == SCORR || typ == CORR || IS_DOOR(typ)
					// || typ == SDOOR) && !IS_WALL(typ)) {
			// levl[x][y].typ = ROOM;
	// }
	/* Sea monsters */
	if (levl[x][y].typ == POOL){
		if(!rn2(85-depth(&u.uz))) {
			if (depth(&u.uz) > 19 && !rn2(3)) {monster = PM_ELECTRIC_EEL;}
				else if (depth(&u.uz) > 15 && !rn2(3)) {monster = PM_GIANT_EEL;}
				else if (depth(&u.uz) > 11 && !rn2(2)) {monster = PM_SHARK;}
				else if (depth(&u.uz) > 7 && rn2(4)) {monster = PM_PIRANHA;}
			(void) makemon(&mons[monster], x, y, NO_MM_FLAGS);
		}
		if(!rn2(140-depth(&u.uz))){
			mkobj_at(RANDOM_CLASS, x, y, NO_MKOBJ_FLAGS);
		}
		else if(!rn2(100-depth(&u.uz))){
		  (void) mkgold((long) rn1(10 * level_difficulty(),10), x, y);
		}
	}
	levl[x][y].lit = 1;
}

STATIC_OVL void
chaliquify(x, y, edge)
register int x, y;
register int edge; /* Allows room walls to intrude slightly into river. */
{
	register int typ = levl[x][y].typ;
	register int monster = PM_JELLYFISH;
	/* Don't liquify shop walls */
	if (level.flags.has_shop && *in_rooms(x, y, SHOPBASE)) {return;}
	
	/* Don't liquify other level features */
	if(typ != TREE && typ != GRASS && typ != SOIL && typ != SAND)
		return;
	
	if (typ!=TREE || (!edge && rn2(6))){
		if(m_at(x, y))
			rloc(m_at(x, y), TRUE);
		levl[x][y].typ = MOAT;
	}
	// else if ((typ == SCORR || typ == CORR || IS_DOOR(typ)
					// || typ == SDOOR) && !IS_WALL(typ)) {
			// levl[x][y].typ = ROOM;
	// }
	/* Sea monsters */
	if (levl[x][y].typ == MOAT){
		if(!rn2(85-depth(&u.uz))) {
			if (depth(&u.uz) > 19 && !rn2(3)) {monster = PM_ELECTRIC_EEL;}
				else if (depth(&u.uz) > 15 && !rn2(3)) {monster = PM_GIANT_EEL;}
				else if (depth(&u.uz) > 11 && !rn2(2)) {monster = PM_SHARK;}
				else if (depth(&u.uz) > 7 && rn2(4)) {monster = PM_PIRANHA;}
			(void) makemon(&mons[monster], x, y, NO_MM_FLAGS);
		}
		if(!rn2(140-depth(&u.uz))){
			mkobj_at(RANDOM_CLASS, x, y, NO_MKOBJ_FLAGS);
		}
		else if(!rn2(100-depth(&u.uz))){
		  (void) mkgold((long) rn1(10 * level_difficulty(),10), x, y);
		}
	}
	levl[x][y].lit = 1;
}

coord *
shrine_pos(roomno)
int roomno;
{
	static coord buf;
	struct mkroom *troom = &rooms[roomno - ROOMOFFSET];

	buf.x = troom->lx + ((troom->hx - troom->lx) / 2);
	buf.y = troom->ly + ((troom->hy - troom->ly) / 2);
	return(&buf);
}

STATIC_OVL void
mkisland() /* John Harris, modified from mktemple & mkshop,
				with ideas and aid from Pasi Kallinen.*/
{
	register struct mkroom *sroom;
	register struct rm *lev;
	register int x, y, dif, ptype, pxwidth, pywidth;
	register int txoff, tyoff, tspot, tdx, tdy;
	register int mx, my, montype;
	register struct obj *otmp;
	register struct obj *ogold;
	register int u_depth = depth(&u.uz);
	/*An island room has a row of water or lava along the inside of the
			wall of the room.  Because this can block progress, it has to be
			done carefully....
		In case someone wants to do something more with this, here's the
			rationale behind the choices I've made:
		Stairs inside the moat could prevent the player from leaving the level
			or room, so that should always be forbidden.
		A one-door room ensures this room isn't a necessary juncture between
			staircases.
		Deeper in the dungeon (past the mines, certainly) it becomes less
			likely that the player will be held back by a single room, so beyond
			level five the restrictions are relaxed a bit.
		In case the player teleports into the room or falls into it from above,
			we always generate a trap door inside it so the player can escape.
		Specifics of room placement & types:
		If we're on or before level four, island rooms can only be made
			in the same kinds of places as shops.  This prevents the player
			from being utterly blocked by the island, but it also makes them
			quite rare in that area.
		If we're at level five or below we're after the Mines, so a pick-axe
			is almost guarenteed, so we can place islands in more places.
		Starting with level six, if the room is big enough we may make some
			extra water.
		From level twelve down, we may use lava instead of water.
		Note: this code depends on the room being rectangular.
	*/
	for(sroom = &rooms[level.flags.sp_lev_nroom]; ; sroom++){
		if(sroom->hx < 0) return;  /* from mkshop: Signifies out of rooms? */
		if(sroom - rooms >= nroom) {
			pline("rooms not closed by -1?");
			return;
		}

		if(sroom->rtype != OROOM || !isspacious(sroom) ||
			has_dnstairs(sroom) || has_upstairs(sroom))
				continue;
		if( sroom->doorct == 1 || (u_depth > 4 && sroom->doorct != 0)) break;
	}

	level.flags.has_island = 1;
	sroom->rtype = ISLAND;

	if (!rn2(3) && u_depth > 11) {ptype = LAVAPOOL;}
	else {ptype = POOL;};

	if ( u_depth > 5 ) {
		pxwidth = ((sroom->hx - sroom-> lx) > 6) ? 2 : 1;
		pywidth = ((sroom->hy - sroom-> ly) > 6) ? 2 : 1;
	}
	else {pxwidth = pywidth = 1;}
	/* Reveal secret doors and liquify borders */
	for(x = sroom->lx; x <= sroom->hx; x++) {
		if (levl[x][sroom->hy+1].typ == SDOOR)
			{levl[x][sroom->hy+1].typ = DOOR;}
		if (levl[x][sroom->ly-1].typ == SDOOR)
			{levl[x][sroom->ly-1].typ = DOOR;}
		levl[x][sroom->hy].typ = ptype;
		levl[x][sroom->ly].typ = ptype;
		if (pywidth == 2) {
			levl[x][sroom->hy-1].typ = ptype;
			levl[x][sroom->ly+1].typ = ptype;
		}
	}
	for(y = (sroom->ly); y <= (sroom->hy); y++) {
		if (levl[sroom->hx+1][y].typ == SDOOR)
			{levl[sroom->hx+1][y].typ = DOOR;}
		if (levl[sroom->lx-1][y].typ == SDOOR)
			{levl[sroom->lx-1][y].typ = DOOR;}
		levl[sroom->hx][y].typ = ptype;
		levl[sroom->lx][y].typ = ptype;
		if (pxwidth == 2) {
			levl[sroom->hx-1][y].typ = ptype;
			levl[sroom->lx+1][y].typ = ptype;
		}
	}
	/* Find the center of the room */
	x = (((sroom->hx - sroom->lx) / 2) + sroom->lx);
	y = (((sroom->hy - sroom->ly) / 2) + sroom->ly);
	/* Make the treasure, add gold to it, bury & mark it */
	otmp = mksobj_at(CHEST, x, y, MKOBJ_ARTIF);
	set_material_gm(otmp, IRON);
	fix_object(otmp);
	ogold = mkgold((long)rn1(u_depth * 100 + 200, 250), x, y);
	remove_object(ogold);
	(void) add_to_container(otmp, ogold);
	bury_an_obj(otmp);
	make_engr_at(x,y,"X",0,ENGRAVE);

	/* Put a tree next to the spot
		(all desert islands have exactly one palm tree on them)*/
	tspot = rn2(8);
	if (tspot < 3) {tyoff = -1;}
	else if (tspot > 3 && tspot != 7) {tyoff = 1;}
	else tyoff = 0;
	if (tspot == 0 || tspot > 5) {txoff = -1;}
	else if (tspot > 1 && tspot < 5) {txoff = 1;}
	else txoff = 0;
	levl[x+txoff][y+tyoff].typ = TREE;
	/* Add a trap door in case the player gets stuck here. */
	do {
		tdx = rn2(sroom->hx - sroom->lx + 1 - pxwidth * 2) + sroom->lx + pxwidth;
		tdy = rn2(sroom->hy - sroom->ly + 1 - pywidth * 2) + sroom->ly + pywidth;
	} while (((tdx == x)&&(tdy == y)) ||
				((tdx == x + txoff) && (tdy == y + tyoff)));

	maketrap(tdx, tdy, TRAPDOOR);
	/* If level 9 or deeper, and the moat is water, maybe make a sea monster.
		(Eels earlier in the dungeon might be too hard and/or produce
		too many experience points.) */
	if (u_depth > 8 && ptype == POOL) {
		/* Make in the corners. */
		if (rn2(2)) {
			mx = (rn2(2)) ? sroom->hx : sroom->lx;
			my = (rn2(2)) ? sroom->hy : sroom->ly;
			(void) makemon(rn2(2) ? &mons[PM_GIANT_EEL]
				: &mons[PM_ELECTRIC_EEL], mx, my, NO_MM_FLAGS);
		}
	}
	level.flags.has_island = TRUE;
}

void
mkpoolroom()
{
	struct mkroom *sroom;
	int x, y;
	int u_depth = depth(&u.uz);
	for(sroom = &rooms[level.flags.sp_lev_nroom]; ; sroom++){
		if(sroom->hx < 0) return;  /* from mkshop: Signifies out of rooms? */
		if(sroom - rooms >= nroom) {
			pline("rooms not closed by -1?");
			return;
		}

		if(sroom->rtype != OROOM || !isspacious(sroom) || has_dnstairs(sroom) || has_upstairs(sroom))
				continue;

		else break;
	}
	
	sroom->rtype = POOLROOM;
	/* Place statues */
	for(x = sroom->lx+1; x <= sroom->hx-1; x++) {
		for(y = (sroom->ly)+1; y <= (sroom->hy)-1; y++) {
			levl[x][y].typ = POOL;
			if (u_depth > 8) {
				if (!rn2(16)) {
					(void) makemon(rn2(2) ? &mons[PM_GIANT_EEL]
						: &mons[PM_ELECTRIC_EEL], x, y, NO_MM_FLAGS);
				}
			}
		}
	}
	
}

void
mkslabroom()
{
	struct mkroom *sroom;
	int x, y, lx, ly;
	
	for(sroom = &rooms[level.flags.sp_lev_nroom]; ; sroom++){
		if(sroom->hx < 0) return;  /* from mkshop: Signifies out of rooms? */
		if(sroom - rooms >= nroom) {
			pline("rooms not closed by -1?");
			return;
		}

		if(sroom->rtype != OROOM || !isspacious(sroom))
				continue;
		else break;
	}
	
	sroom->rtype = SLABROOM;
	
	lx = sroom->lx;
	ly = sroom->ly;
	for(x = sroom->lx+2; x < sroom->lx+5; x++) {
		for(y = (sroom->ly)+2; y < (sroom->ly)+5; y++) {
			levl[x][y].typ = VWALL;
		}
	}
	x = lx+3; y = ly+3;
	levl[x][y].typ = ROOM;
	{
		int remaining = 0;
		if(!flags.made_first)
			remaining++;
		if(!flags.made_divide)
			remaining++;
		if(!flags.made_life)
			remaining++;
		if(remaining){
			remaining = rnd(remaining);
			if(!flags.made_first && !(--remaining))
				mksobj_at(FIRST_WORD, x, y, NO_MKOBJ_FLAGS);
			else if(!flags.made_divide && !(--remaining))
				mksobj_at(DIVIDING_WORD, x, y, NO_MKOBJ_FLAGS);
			else if(!flags.made_life && !(--remaining))
				mksobj_at(NURTURING_WORD, x, y, NO_MKOBJ_FLAGS);
		}
	}
	
	x = lx+2; y = ly+3;
	levl[x][y].typ = DOOR;
	x = lx+3; y = ly+2;
	levl[x][y].typ = DOOR;
	x = lx+4; y = ly+3;
	levl[x][y].typ = DOOR;
	x = lx+3; y = ly+4;
	levl[x][y].typ = DOOR;
	wallification(sroom->lx, sroom->ly, sroom->hx, sroom->hy);
}

void
mkhellvaultroom()
{
	struct mkroom *sroom;
	int x, y, lx, ly;
	
	for(sroom = &rooms[level.flags.sp_lev_nroom]; ; sroom++){
		if(sroom->hx < 0) return;  /* from mkshop: Signifies out of rooms? */
		if(sroom - rooms >= nroom) {
			pline("rooms not closed by -1?");
			return;
		}

		if(sroom->rtype != OROOM || !isspacious(sroom))
				continue;
		else break;
	}
	
	sroom->rtype = HELL_VAULT;
	
	lx = sroom->lx + rn2((sroom->hx - sroom->lx)+1 - 5);
	ly = sroom->ly + rn2((sroom->hy - sroom->ly)+1 - 5);
	for(x = lx+1; x < lx+4; x++) {
		for(y = ly+1; y < ly+4; y++) {
			levl[x][y].typ = VWALL;
		}
	}
	x = lx+2; y = ly+2;
	/* Put a hellish seal at m.x, m.y */
	levl[x][y].typ = HELLISH_SEAL;
	/*Pick monster*/
	if(VN_MAX >= VAULT_LIMIT){
		impossible("Vault exceeded [safe fallback triggered]");
		levl[x][y].vaulttype = rnd(VAULT_LIMIT-1);
	} else {
		levl[x][y].vaulttype = rnd(VN_MAX-1);
	}
	// level.flags.nseals++;
	/*Create statue on top*/
	mkHVstatue(x, y, levl[x][y].vaulttype);
	
	wallification(sroom->lx, sroom->ly, sroom->hx, sroom->hy);
}

STATIC_OVL void
mksgardenstatueat(x,y)
	int x;
	int y;
{
	struct trap *t;
	struct obj *otmp;
	t = t_at(x, y);
	if(!t || t->ttyp == STATUE_TRAP){
		otmp = mksobj_at(STATUE, x, y, MKOBJ_ARTIF);
		if(otmp){
			if(t && t->ttyp != MAGIC_PORTAL)
				deltrap(t);
			if( (depth(&u.uz) - monstr[otmp->corpsenm]) >= rn2(100)){
				t = maketrap(x, y, MAGIC_TRAP);
				if(t){
					t->ttyp = STATUE_TRAP;
				}
			}
		}
	}

}

STATIC_OVL void
mksgarden()
{
	register struct mkroom *sroom;
	register int x, y;
	for(sroom = &rooms[level.flags.sp_lev_nroom]; ; sroom++){
		if(sroom->hx < 0) return;  /* from mkshop: Signifies out of rooms? */
		if(sroom - rooms >= nroom) {
			pline("rooms not closed by -1?");
			return;
		}

		if(sroom->rtype != OROOM || !issemispacious(sroom))
				continue;
		else break;
	}
	sroom->rtype = STATUEGRDN;
	/* Place statues */
	for(x = sroom->lx; x <= sroom->hx; x++) {
		mksgardenstatueat(x, sroom->hy);
		mksgardenstatueat(x, sroom->ly);
	}
	for(y = (sroom->ly)+1; y <= (sroom->hy)-1; y++) {
		mksgardenstatueat(sroom->hx, y);
		mksgardenstatueat(sroom->lx, y);
	}
	
	for(x = sroom->lx+1; x <= sroom->hx-1; x++) {
		mksobj_at(FIGURINE, x, sroom->hy-1, MKOBJ_ARTIF);
		mksobj_at(FIGURINE, x, sroom->ly+1, MKOBJ_ARTIF);
	}
	for(y = (sroom->ly)+2; y <= (sroom->hy)-2; y++) {
		mksobj_at(FIGURINE, sroom->hx-1, y, MKOBJ_ARTIF);
		mksobj_at(FIGURINE, sroom->lx+1, y, MKOBJ_ARTIF);
	}
}

STATIC_OVL void
mktemple()
{
	register struct mkroom *sroom;
	coord *shrine_spot;
	aligntyp alignment;

	if(!(sroom = pick_room(TRUE, TRUE))) return;

	/* set up Priest and shrine */
	sroom->rtype = TEMPLE;
	/*
	 * In temples, shrines are blessed altars
	 * located in the center of the room
	 */
	shrine_spot = shrine_pos((sroom - rooms) + ROOMOFFSET);
	if (In_hell(&u.uz))
		alignment = A_NONE;	/* in gehennom, all altars are to moloch */
	else
		alignment = induced_align(80);
	add_altar(shrine_spot->x, shrine_spot->y, alignment, TRUE, align_to_god(alignment));
	priestini(&u.uz, sroom, shrine_spot->x, shrine_spot->y, FALSE);
	level.flags.has_temple = 1;
}

boolean
nexttodoor(sx,sy)
register int sx, sy;
{
	register int dx, dy;
	register struct rm *lev;
	for(dx = -1; dx <= 1; dx++) for(dy = -1; dy <= 1; dy++) {
		if(!isok(sx+dx, sy+dy)) continue;
		if(IS_DOOR((lev = &levl[sx+dx][sy+dy])->typ) ||
		    lev->typ == SDOOR)
			return(TRUE);
	}
	return(FALSE);
}

boolean
has_dnstairs(sroom)
register struct mkroom *sroom;
{
	if (sroom == dnstairs_room)
		return TRUE;
	if (sstairs.sx && !sstairs.up)
		return((boolean)(sroom == sstairs_room));
	return FALSE;
}

boolean
has_upstairs(sroom)
register struct mkroom *sroom;
{
	if (sroom == upstairs_room)
		return TRUE;
	if (sstairs.sx && sstairs.up)
		return((boolean)(sroom == sstairs_room));
	return FALSE;
}

#endif /* OVLB */
#ifdef OVL0

int
somex(croom)
register struct mkroom *croom;
{
	return rn2(croom->hx-croom->lx+1) + croom->lx;
}

int
somey(croom)
register struct mkroom *croom;
{
	return rn2(croom->hy-croom->ly+1) + croom->ly;
}

boolean
inside_room(croom, x, y)
struct mkroom *croom;
xchar x, y;
{
	return((boolean)(x >= croom->lx-1 && x <= croom->hx+1 &&
		y >= croom->ly-1 && y <= croom->hy+1));
}

boolean
somexy(croom, c)
struct mkroom *croom;
coord *c;
{
	int try_cnt = 0;
	int i;

	if (croom->irregular) {
	    i = (croom - rooms) + ROOMOFFSET;

	    while(try_cnt++ < 100) {
		c->x = somex(croom);
		c->y = somey(croom);
		if (!levl[c->x][c->y].edge &&
			(int) levl[c->x][c->y].roomno == i)
		    return TRUE;
	    }
	    /* try harder; exhaustively search until one is found */
	    for(c->x = croom->lx; c->x <= croom->hx; c->x++)
		for(c->y = croom->ly; c->y <= croom->hy; c->y++)
		    if (!levl[c->x][c->y].edge &&
			    (int) levl[c->x][c->y].roomno == i)
			return TRUE;
	    return FALSE;
	}

	if (!croom->nsubrooms) {
		c->x = somex(croom);
		c->y = somey(croom);
		return TRUE;
	}

	/* Check that coords doesn't fall into a subroom or into a wall */

	while(try_cnt++ < 100) {
		c->x = somex(croom);
		c->y = somey(croom);
		if (IS_WALL(levl[c->x][c->y].typ))
		    continue;
		for(i=0 ; i<croom->nsubrooms;i++)
		    if(inside_room(croom->sbrooms[i], c->x, c->y))
			goto you_lose;
		break;
you_lose:	;
	}
	if (try_cnt >= 100)
	    return FALSE;
	return TRUE;
}

/*
 * Search for a special room given its type (zoo, court, etc...)
 *	Special values :
 *		- ANY_SHOP
 *		- ANY_TYPE
 */

struct mkroom *
search_special(type)
schar type;
{
	register struct mkroom *croom;

	for(croom = &rooms[0]; croom->hx >= 0; croom++)
	    if((type == ANY_TYPE && croom->rtype != OROOM) ||
	       (type == ANY_SHOP && croom->rtype >= SHOPBASE) ||
	       croom->rtype == type)
		return croom;
	for(croom = &subrooms[0]; croom->hx >= 0; croom++)
	    if((type == ANY_TYPE && croom->rtype != OROOM) ||
	       (type == ANY_SHOP && croom->rtype >= SHOPBASE) ||
	       croom->rtype == type)
		return croom;
	return (struct mkroom *) 0;
}

#endif /* OVL0 */
#ifdef OVLB

struct permonst *
courtmon(kingnum)
	int kingnum;
{
	int i;
	switch(kingnum){
		case PM_DWARF:
		case PM_DWARF_KING:
		case PM_DWARF_QUEEN:
			i = rnd(100);
			if(i>95)
				return &mons[PM_DWARF_LORD];
			else if(i>90)
				return &mons[PM_DWARF_CLERIC];
			else if(i>50)
				return &mons[PM_DWARF];
			else if(i>20)
				return &mons[PM_GNOME];
			else if(i>17)
				return &mons[PM_GNOME_LORD];
			else if(i>14)
				return &mons[PM_GNOME_LADY];
			else if(i>10)
				return &mons[PM_GNOMISH_WIZARD];
			else if(i> 5)
				return &mons[PM_HOBBIT];
			else if(i> 0)
				return &mons[PM_LEPRECHAUN];
		break;
		
		case PM_KOBOLD:
		case PM_KOBOLD_LORD:
			i = rnd(100);
			if(i>95)
				return &mons[PM_KOBOLD_SHAMAN];
			else if(i>60)
				return &mons[PM_LARGE_KOBOLD];
			else if(i>10)
				return &mons[PM_KOBOLD];
			else if(i> 2)
				return &mons[PM_LEMURE];
			else if(i> 0)
				return &mons[PM_IMP];
		break;
		
		case PM_ORC:
		case PM_ORC_CAPTAIN:
			i = rnd(100);
			if(i>95)
				return &mons[PM_ORC_SHAMAN];
			else if(i>80)
				return &mons[PM_MORDOR_ORC];
			else if(i>60)
				return &mons[PM_HILL_ORC];
			else if(i>40)
				return &mons[PM_HOBGOBLIN];
			else if(i>10)
				return &mons[PM_GOBLIN];
			else if(i> 2)
				return &mons[PM_MANES];
			else if(i> 0)
				return &mons[PM_QUASIT];
		break;
		
		case PM_URUK_CAPTAIN:
			i = rnd(100);
			if(i>95)
				return &mons[PM_ORC_SHAMAN];
			else if(i>60)
				return &mons[PM_URUK_HAI];
			else if(i>40)
				return &mons[PM_HOBGOBLIN];
			else if(i>10)
				return &mons[PM_GOBLIN];
			else if(i> 2)
				return &mons[PM_LEMURE];
			else if(i> 0)
				return &mons[PM_IMP];
		break;
		
		case PM_ORC_OF_THE_AGES_OF_STARS:
			i = rnd(100);
			if(i>98)
				return &mons[PM_DEEP_DRAGON];
			else if(i>96)
				return &mons[PM_RED_DRAGON];
			else if(i>94)
				return &mons[PM_WHITE_DRAGON];
			else if(i>92)
				return &mons[PM_BLACK_DRAGON];
			else if(i>90)
				return &mons[PM_BLUE_DRAGON];
			else if(i>60)
				return &mons[PM_ANGBAND_ORC];
			else if(i>40)
				return &mons[PM_HEDROW_BLADEMASTER];
			else if(i>36)
				return &mons[PM_SUCCUBUS];
			else if(i>32)
				return &mons[PM_INCUBUS];
			else if(i>20)
				return &mons[PM_OLOG_HAI];
			else if(i>10)
				return &mons[PM_SIEGE_OGRE];
			else if(i> 0)
				return &mons[PM_HELL_HOUND];
		break;
		
		case PM_GNOME:
		case PM_GNOME_KING:
		case PM_GNOME_QUEEN:
			i = rnd(100);
			if(i>95)
				return &mons[PM_GNOME_LORD];
			else if(i>90)
				return &mons[PM_GNOME_LADY];
			else if(i>50)
				return &mons[PM_GNOME];
			else if(i>45)
				return &mons[PM_GNOMISH_WIZARD];
			else if(i>40)
				return &mons[PM_TINKER_GNOME];
			else if(i>20)
				return &mons[PM_DWARF];
			else if(i>17)
				return &mons[PM_DWARF_LORD];
			else if(i>14)
				return &mons[PM_DWARF_CLERIC];
			else if(i>10)
				return &mons[PM_HOBBIT];
			else if(i> 5)
				return &mons[PM_LEPRECHAUN];
			else if(i> 0)
				return &mons[PM_HOMUNCULUS];
		break;
		
		case PM_GIANT:
		case PM_TITAN:
			i = rnd(100);
			if(i>90)
				return &mons[PM_STORM_GIANT];
			else if(i>75)
				return &mons[PM_FIRE_GIANT];
			else if(i>60)
				return &mons[PM_FROST_GIANT];
			else if(i>35)
				return &mons[PM_HILL_GIANT];
			else if(i>20)
				return &mons[PM_STONE_GIANT];
			else if(i>17)
				return &mons[PM_GIANT];
			else if(i>15)
				return &mons[PM_SON_OF_TYPHON];
			else if(i>11)
				return &mons[PM_DRYAD];
			else if(i> 7)
				return &mons[PM_NAIAD];
			else if(i> 3)
				return &mons[PM_OREAD];
			else if(i> 0)
				return &mons[PM_DEMINYMPH];
		break;
		
		case PM_DEEP_ONE:
		case PM_DEEPEST_ONE:
			i = rnd(100);
			if(i>80)
				return &mons[PM_DEEPER_ONE];
			else if(i>30)
				return &mons[PM_DEEP_ONE];
			else if(i>25)
				return &mons[PM_MIND_FLAYER];
			else if(i>20)
				return &mons[PM_DARK_YOUNG];
			else if(i>15)
				return &mons[PM_UMBER_HULK];
			else if(i>11)
				return &mons[PM_STUMBLING_HORROR];
			else if(i> 7)
				return &mons[PM_WANDERING_HORROR];
			else if(i> 3)
				return &mons[PM_SHAMBLING_HORROR];
			else if(i> 0)
				return &mons[PM_HUMAN];
		break;
		
		case PM_OGRE:
		case PM_OGRE_KING:
			i = rnd(100);
			if(i>95)
				return &mons[PM_OGRE_MAGE];
			else if(i>85)
				return &mons[PM_OGRE_LORD];
			else if(i>50)
				return &mons[PM_OGRE];
			else if(i>40)
				return &mons[PM_ORC_CAPTAIN];
			else if(i>20)
				return &mons[PM_MORDOR_ORC];
			else if(i>15)
				return &mons[PM_TENGU];
			else if(i> 5)
				return &mons[PM_TROLL];
			else if(i> 0)
				return(mkclass(S_GIANT, Inhell ? G_HELL : G_NOHELL));
		break;
		
		case PM_VAMPIRE:
		case PM_VAMPIRE_LORD:
		case PM_VAMPIRE_LADY:
			i = rnd(100);
			if(i>90)
				return &mons[PM_VAMPIRE];
			else if(i>70)
				return(mkclass(S_ZOMBIE, Inhell ? G_HELL : G_NOHELL));
			else if(i>65)
				return(mkclass(S_MUMMY, Inhell ? G_HELL : G_NOHELL));
			else if(i>60)
				return &mons[PM_FLESH_GOLEM];
			else if(i>55)
				return &mons[PM_WEREWOLF];
			else if(i>50)
				return &mons[PM_FLOATING_EYE];
			else if(i>45)
				return &mons[PM_GARGOYLE];
			else if(i>40)
				return &mons[PM_WINGED_GARGOYLE];
			else if(i>35)
				return &mons[PM_QUASIT];
			else if(i>30)
				return &mons[PM_BLACK_PUDDING];
			else if(i>25)
				return &mons[PM_WRAITH];
			else if(i>20)
				return &mons[PM_PHANTASM];
			else if(i>15)
				return &mons[PM_GHOST];
			else if(i>10)
				return &mons[PM_MAID];
			else if(i> 5)
				return &mons[PM_WEEPING_WILLOW];
			else if(i> 0)
				return &mons[PM_SWAMP_NYMPH];
		break;
		
		case PM_ELF:
		case PM_ELVENKING:
		case PM_ELVENQUEEN:
			i = rnd(100);
			if(i>95)
				return &mons[PM_ELF_LORD];
			else if(i>90)
				return &mons[PM_ELF_LADY];
			else if(i>75)
				return &mons[PM_GREY_ELF];
			else if(i>60)
				return &mons[PM_GREEN_ELF];
			else if(i>50)
				return &mons[PM_WOODLAND_ELF];
			else if(i>40)
				return &mons[PM_QUICKLING];
			else if(i>32)
				return &mons[PM_DRYAD];
			else if(i>30)
				return &mons[PM_NAIAD];
			else if(i>20)
				return &mons[PM_COURE_ELADRIN];
			else if(i>18)
				return &mons[PM_NOVIERE_ELADRIN];
			else if(i>16)
				return &mons[PM_BRALANI_ELADRIN];
			else if(i>13)
				return &mons[PM_FIRRE_ELADRIN];
			else if(i>10)
				return &mons[PM_SHIERE_ELADRIN];
			else if(i> 5)
				return &mons[PM_DOPPELGANGER];
			else if(i> 0)
				return &mons[PM_WOOD_TROLL];
		break;
		
		case PM_DROW:
		case PM_DROW_MATRON:
			i = rnd(100);
			if(i>90)
				return &mons[PM_UNEARTHLY_DROW];
			if(i>80)
				return &mons[PM_HEDROW_WIZARD];
			if(i>50)
				return &mons[PM_HEDROW_WARRIOR];
			if(i>45)
				return &mons[PM_DROW_CAPTAIN];
			if(i>40)
				return &mons[PM_DROW];
			if(i>30)
				return &mons[PM_SPROW];
			if(i>25)
				return &mons[PM_SWAMP_NYMPH];
			if(i>15)
				return &mons[PM_GIANT_SPIDER];
			if(i>10)
				return &mons[PM_CAVE_LIZARD];
			if(i> 5)
				return &mons[PM_LARGE_CAVE_LIZARD];
			if(i> 0)
				return &mons[PM_QUASIT];
		break;
		
		case PM_EMBRACED_DROWESS:
			i = rnd(100);
			if(i>95)
				return &mons[PM_DROW_ALIENIST];
			else if(i>90)
				return &mons[PM_HEDROW_BLADEMASTER];
			else if(i>70)
				return &mons[PM_EDDERKOP];
			else if(i>60)
				return &mons[PM_HEDROW_WIZARD];
			else if(i>30)
				return &mons[PM_HEDROW_WARRIOR];
			else if(i> 20)
				return &mons[PM_DROW_MATRON];
			else if(i> 10)
				return &mons[PM_DROW_CAPTAIN];
			else if(i> 0)
				return &mons[PM_DROW_MUMMY];
		break;
		
		case PM_STANNUMACH_RILMANI:
			i = rnd(100);
			if(i>95)
				return &mons[PM_ARGENTUM_GOLEM];
			else if(i>90)
				return &mons[PM_CUPRILACH_RILMANI];
			else if(i>70)
				return &mons[PM_IRON_GOLEM];
			else if(i>30)
				return &mons[PM_FERRUMACH_RILMANI];
			else
				return &mons[PM_PLUMACH_RILMANI];
		break;
		
		case PM_AURUMACH_RILMANI:
			i = rnd(100);
			if(i>99)
				return &mons[PM_ARGENACH_RILMANI];
			else if(i>95)
				return &mons[PM_ARGENTUM_GOLEM];
			else if(i>90)
				return &mons[PM_CUPRILACH_RILMANI];
			else if(i>80)
				return &mons[PM_FERRUMACH_RILMANI];
			else if(i>60)
				return &mons[PM_PLUMACH_RILMANI];
			else if(i>50)
				return &mons[PM_PLAINS_CENTAUR];
			else if(i>45)
				return &mons[PM_DRYAD];
			else if(i>40)
				return &mons[PM_STONE_GIANT];
			else if(i>35)
				return &mons[PM_HILL_GIANT];
			else if(i>30)
				return &mons[PM_FROST_GIANT];
			else if(i>25)
				return &mons[PM_STORM_GIANT];
			else if(i>20)
				return &mons[PM_QUANTUM_MECHANIC];
			else if(i>15)
				return &mons[PM_SERPENT_MAN_OF_YOTH];
			else if(i>10)
				return &mons[PM_DEEP_ONE];
			else if(i>7)
				return &mons[PM_DEEPER_ONE];
			else if(i>5)
				return &mons[PM_MIND_FLAYER];
			else if(i>1)
				return &mons[PM_HOOLOOVOO];
			else
				return &mons[PM_UVUUDAUM];
		break;
		
		default:
			i = rn2(60) + rn2(3*level_difficulty());
			if (i > 100)		return(mkclass(S_DRAGON, Inhell ? G_HELL : G_NOHELL));
			else if (i > 95)	return(mkclass(S_GIANT, Inhell ? G_HELL : G_NOHELL));
			else if (i > 85)	return(mkclass(S_TROLL, Inhell ? G_HELL : G_NOHELL));
			else if (i > 75)	return(mkclass(S_CENTAUR, Inhell ? G_HELL : G_NOHELL));
			else if (i > 60)	return(mkclass(S_ORC, Inhell ? G_HELL : G_NOHELL));
			else if (i > 45)	return(&mons[PM_BUGBEAR]);
			else if (i > 30)	return(&mons[PM_HOBGOBLIN]);
			else if (i > 15)	return(mkclass(S_GNOME, Inhell ? G_HELL : G_NOHELL));
			else			return(mkclass(S_KOBOLD, Inhell ? G_HELL : G_NOHELL));
		break;
	
	}
	return &mons[PM_GNOME];
}

struct permonst *
mivaultmon()
{
	switch(rn2(22)){
		case 0:
			return(&mons[PM_SHOGGOTH]);
		break;
		case 1:
			return(&mons[PM_PRIEST_OF_GHAUNADAUR]);
		break;
		case 2:
			return(&mons[PM_EYE_OF_DOOM]);
		break;
		case 3:
			return(&mons[PM_SON_OF_TYPHON]);
		break;
		case 4:
			return(&mons[PM_NIGHTGAUNT]);
		break;
		case 5:
			return(&mons[PM_DARK_YOUNG]);
		break;
		case 6:
			return(&mons[PM_MIRKWOOD_ELDER]);
		break;
		case 7:
			return(&mons[PM_PURPLE_WORM]);
		break;
		case 8:
			return(&mons[PM_HUNTING_HORROR]);
		break;
		case 9:
			return(&mons[PM_BEBELITH]);
		break;
		case 10:
			return(&mons[PM_AMMIT]);
		break;
		case 11:
			return(&mons[PM_GIGANTIC_PSEUDODRAGON]);
		break;
		case 12:
			return(&mons[PM_JUGGERNAUT]);
		break;
		case 13:
			return(&mons[PM_ID_JUGGERNAUT]);
		break;
		case 14:
			return(&mons[PM_SCRAP_TITAN]);
		break;
		case 15:
			return(&mons[PM_HELLFIRE_COLOSSUS]);
		break;
		case 16:
			return(&mons[PM_TITAN]);
		break;
		case 17:
			return(&mons[PM_JABBERWOCK]);
		break;
		case 18:
			return(&mons[PM_GUARDIAN_NAGA]);
		break;
		case 19:
			return(&mons[PM_GROVE_GUARDIAN]);
		break;
		case 20:
			return(&mons[PM_IRON_GOLEM]);
		break;
		case 21:
			return(&mons[PM_BALROG]);
		break;
	}
	return &mons[PM_SHOGGOTH];
}

#define SQUADSIZE 4

static struct soldier_squad_probabilities {
    unsigned	pm;
    unsigned	prob;
} squadprob[] = {
    {PM_SOLDIER, 80}, {PM_SERGEANT, 15}, {PM_LIEUTENANT, 4}, {PM_CAPTAIN, 1}
}, neu_squadprob[] = {
    {PM_FERRUMACH_RILMANI, 80}, {PM_IRON_GOLEM, 15}, {PM_ARGENTUM_GOLEM, 4}, {PM_CUPRILACH_RILMANI, 1}
}, hell_squadprob[] = {
	{ PM_LEGION_DEVIL_GRUNT, 80 }, { PM_LEGION_DEVIL_SOLDIER, 15 }, { PM_LEGION_DEVIL_SERGEANT, 4 }, { PM_LEGION_DEVIL_CAPTAIN, 1 }
};

STATIC_OVL struct permonst *
squadmon(lev)		/* return soldier types appropriate for the current branch. */
d_level *lev;
{
	int sel_prob, i, cpro, mndx;
	struct soldier_squad_probabilities *squadies;

	if (In_outlands(lev))
		squadies = neu_squadprob;
	else if (In_hell(lev))
		squadies = hell_squadprob;
	else
		squadies = squadprob;

	sel_prob = rnd(80+level_difficulty());
	cpro = 0;
	
	for (i = 0; i < SQUADSIZE; i++) {
		cpro += squadies[i].prob;
	    if (cpro > sel_prob) {
			mndx = squadies[i].pm;
		goto gotone;
	    }
	}
	mndx = squadies[rn2(SQUADSIZE)].pm;

gotone:
//	if (!(mvitals[mndx].mvflags & G_GONE && !In_quest(&u.uz))) return(&mons[mndx]);
	if (!(mvitals[mndx].mvflags & G_GENOD && !In_quest(&u.uz))) return(&mons[mndx]);//empty if genocided
	else			    return((struct permonst *) 0);
}


/*
 * save_room : A recursive function that saves a room and its subrooms
 * (if any).
 */

STATIC_OVL void
save_room(fd, r)
int	fd;
struct mkroom *r;
{
	short i;
	/*
	 * Well, I really should write only useful information instead
	 * of writing the whole structure. That is I should not write
	 * the subrooms pointers, but who cares ?
	 */
	bwrite(fd, (genericptr_t) r, sizeof(struct mkroom));
	for(i=0; i<r->nsubrooms; i++)
	    save_room(fd, r->sbrooms[i]);
}

/*
 * save_rooms : Save all the rooms on disk!
 */

void
save_rooms(fd)
int fd;
{
	short i;

	/* First, write the number of rooms */
	bwrite(fd, (genericptr_t) &nroom, sizeof(nroom));
	for(i=0; i<nroom; i++)
	    save_room(fd, &rooms[i]);
}

STATIC_OVL void
rest_room(fd, r)
int fd;
struct mkroom *r;
{
	short i;

	mread(fd, (genericptr_t) r, sizeof(struct mkroom));
	for(i=0; i<r->nsubrooms; i++) {
		r->sbrooms[i] = &subrooms[nsubroom];
		rest_room(fd, &subrooms[nsubroom]);
		subrooms[nsubroom++].resident = (struct monst *)0;
	}
}

/*
 * rest_rooms : That's for restoring rooms. Read the rooms structure from
 * the disk.
 */

void
rest_rooms(fd)
int	fd;
{
	short i;

	mread(fd, (genericptr_t) &nroom, sizeof(nroom));
	nsubroom = 0;
	for(i = 0; i<nroom; i++) {
	    rest_room(fd, &rooms[i]);
	    rooms[i].resident = (struct monst *)0;
	}
	rooms[nroom].hx = -1;		/* restore ending flags */
	subrooms[nsubroom].hx = -1;
}
#endif /* OVLB */

/*mkroom.c*/
