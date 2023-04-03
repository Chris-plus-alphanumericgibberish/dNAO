/*	SCCS Id: @(#)apply.c	3.4	2003/11/18	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <math.h>
#include "hack.h"
#include "artifact.h"
#include "xhity.h"
#ifdef OVLB

static const char tools[] = { COIN_CLASS, CHAIN_CLASS, SCOIN_CLASS, TOOL_CLASS, WEAPON_CLASS, WAND_CLASS, 0 };
static const char tools_too[] = { COIN_CLASS, ALL_CLASSES, SCOIN_CLASS, TOOL_CLASS, POTION_CLASS,
				  WEAPON_CLASS, WAND_CLASS, GEM_CLASS, CHAIN_CLASS, 0 };
static const char apply_armor[] = { ARMOR_CLASS, 0 };
static const char imperial_repairs[] = { AMULET_CLASS, ARMOR_CLASS, RING_CLASS, WAND_CLASS, 0 };
static const char apply_corpse[] = { FOOD_CLASS, 0 };
static const char chain_class[] = { CHAIN_CLASS, 0 };
static const char apply_all[] = { ALL_CLASSES, CHAIN_CLASS, 0 };

#define TREPH_THOUGHTS 1
#define TREPH_CRYSTALS 2

#ifdef TOURIST
STATIC_DCL int FDECL(use_camera, (struct obj *));
#endif
STATIC_DCL int FDECL(do_present_item, (struct obj *));
STATIC_DCL int FDECL(use_towel, (struct obj *));
STATIC_DCL boolean FDECL(its_dead, (int,int,int *,struct obj*));
STATIC_DCL int FDECL(use_stethoscope, (struct obj *));
STATIC_DCL void FDECL(use_whistle, (struct obj *));
STATIC_DCL void FDECL(use_leash, (struct obj *));
STATIC_DCL int FDECL(use_mirror, (struct obj **));
STATIC_DCL void FDECL(use_candelabrum, (struct obj *));
STATIC_DCL void FDECL(use_candle, (struct obj **));
STATIC_DCL void FDECL(use_lamp, (struct obj *));
STATIC_DCL int FDECL(swap_aegis, (struct obj *));
STATIC_DCL int FDECL(aesculapius_poke, (struct obj *));
STATIC_DCL int FDECL(ilmater_touch, (struct obj *));
STATIC_DCL int FDECL(use_rakuyo, (struct obj *));
STATIC_DCL int FDECL(use_mercy_blade, (struct obj *));
STATIC_DCL int FDECL(use_force_blade, (struct obj *));
STATIC_DCL void FDECL(light_cocktail, (struct obj *));
STATIC_DCL void FDECL(light_torch, (struct obj *));
STATIC_DCL void FDECL(use_trephination_kit, (struct obj *));
STATIC_DCL void FDECL(use_tinning_kit, (struct obj *));
STATIC_DCL int FDECL(use_figurine, (struct obj **));
STATIC_DCL int FDECL(use_crystal_skull, (struct obj **));
STATIC_DCL void FDECL(use_grease, (struct obj *));
STATIC_DCL void FDECL(use_trap, (struct obj *));
STATIC_DCL void FDECL(use_stone, (struct obj *));
STATIC_DCL int FDECL(use_sensor, (struct obj *));
STATIC_DCL int NDECL(sensorMenu);
STATIC_DCL int FDECL(use_hypospray, (struct obj *));
STATIC_DCL int FDECL(use_droven_cloak, (struct obj **));
STATIC_DCL int FDECL(use_darkweavers_cloak, (struct obj *));
STATIC_PTR int NDECL(set_trap);		/* occupation callback */
STATIC_DCL int FDECL(use_pole, (struct obj *));
STATIC_DCL int FDECL(use_cream_pie, (struct obj *));
STATIC_DCL int FDECL(use_grapple, (struct obj *));
STATIC_DCL int FDECL(use_crook, (struct obj *));
STATIC_DCL int FDECL(use_dilithium, (struct obj *));
STATIC_DCL int FDECL(use_doll, (struct obj *));
STATIC_DCL int FDECL(use_doll_tear, (struct obj *));
STATIC_DCL int FDECL(use_pyramid, (struct obj *));
STATIC_DCL int FDECL(use_vortex, (struct obj *));
STATIC_DCL int FDECL(use_rift, (struct obj *));
STATIC_DCL int FDECL(do_break_wand, (struct obj *));
STATIC_DCL int FDECL(do_flip_coin, (struct obj *));
STATIC_DCL void FDECL(soul_crush_consequence, (struct obj *));
STATIC_DCL int FDECL(do_soul_coin, (struct obj *));
STATIC_DCL boolean FDECL(figurine_location_checks,
				(struct obj *, coord *, BOOLEAN_P));
STATIC_DCL boolean NDECL(uhave_graystone);
STATIC_DCL int FDECL(do_carve_obj, (struct obj *));
STATIC_PTR int FDECL(pick_rune, (BOOLEAN_P));
STATIC_DCL void FDECL(describe_rune, (int));
STATIC_PTR char NDECL(pick_carvee);
STATIC_PTR int FDECL(res_engine_menu, (struct obj *));
STATIC_PTR int NDECL(dotrephination_options);

#ifdef	AMIGA
void FDECL( amii_speaker, ( struct obj *, char *, int ) );
#endif

static const char no_elbow_room[] = "don't have enough elbow-room to maneuver.";

#ifdef TOURIST
STATIC_OVL int
use_camera(obj)
	struct obj *obj;
{
	register struct monst *mtmp;

	if(Underwater) {
		pline("Using your camera underwater would void the warranty.");
		return MOVE_CANCELLED;
	}
	if(!getdir((char *)0)) return MOVE_CANCELLED;

	if (obj->spe <= 0) {
		pline("%s", nothing_happens);
		return MOVE_STANDARD;
	}
	consume_obj_charge(obj, TRUE);

	if (obj->cursed && !rn2(2)) {
		(void) zapyourself(obj, TRUE);
	} else if (u.uswallow) {
		You("take a picture of %s %s.", s_suffix(mon_nam(u.ustuck)),
		    mbodypart(u.ustuck, STOMACH));
	} else if (u.dz) {
		You("take a picture of the %s.",
			(u.dz > 0) ? surface(u.ux,u.uy) : ceiling(u.ux,u.uy));
	} else if (!u.dx && !u.dy) {
		(void) zapyourself(obj, TRUE);
	} else if ((mtmp = bhit(u.dx, u.dy, COLNO, FLASHED_LIGHT,
				(int FDECL((*),(MONST_P,OBJ_P)))0,
				(int FDECL((*),(OBJ_P,OBJ_P)))0,
				obj, NULL)) != 0) {
		obj->ox = u.ux,  obj->oy = u.uy;
		(void) flash_hits_mon(mtmp, obj);
	}
	return MOVE_STANDARD;
}
#endif

STATIC_OVL int
do_present_item(obj)
	struct obj *obj;
{
	register struct monst *mtmp, *tm;
	const char *word = obj->oclass == RING_CLASS ? "ring" : obj->oclass == AMULET_CLASS ? "amulet" : "item";

	if(!getdir((char *)0)) return MOVE_CANCELLED;
	
	if(obj->oward == 0 && !(obj->ohaluengr)){
		exercise(A_WIS, FALSE);
		return MOVE_INSTANT;
	}
	
	if (u.uswallow) {
		You("display the %s engraving to %s %s.", s_suffix(word), s_suffix(mon_nam(u.ustuck)),
		    mbodypart(u.ustuck, STOMACH));
		pline("Nothing happens.");
		exercise(A_WIS, FALSE);
		return MOVE_INSTANT;
	} else if (u.dz) {
		You("display the %s engraving to the %s.", s_suffix(word),
			(u.dz > 0) ? surface(u.ux,u.uy) : ceiling(u.ux,u.uy));
		if(u.dz < 0 || obj->oartifact){
			pline("Nothing happens.");
			exercise(A_WIS, FALSE);
			return MOVE_INSTANT;
		}
		if(is_lava(u.ux, u.uy) || flags.beginner || !(u.ualign.type == A_CHAOTIC || Hallucination) ){
			pline("Nothing happens.");
			if(u.ualign.type == A_LAWFUL) exercise(A_WIS, FALSE);
			return MOVE_INSTANT;
		}else{
			You_feel("as though the engraving on the %s could fall right off!", word);
			if(yn("Give it a push?") == 'n'){
				pline("Nothing happens.");
				return MOVE_INSTANT;
			}
			else{
				struct engr *engrHere = engr_at(u.ux,u.uy);
				if(u.ualign.type == A_LAWFUL) exercise(A_WIS, FALSE);
				if (IS_ALTAR(levl[u.ux][u.uy].typ)) {
					altar_wrath(u.ux, u.uy);
					return MOVE_INSTANT;
				}
				if(!engrHere){
					make_engr_at(u.ux, u.uy,	"", (moves - multi), DUST); /* absense of text =  dust */
					engrHere = engr_at(u.ux,u.uy); /*note: make_engr_at does not return the engraving it made, it returns void instead*/
				}
				if(obj->ohaluengr == engrHere->halu_ward && obj->oward == engrHere->ward_id){
					pline("the engraving tumbles off the %s to join it's fellows.", word);
					engrHere->complete_wards += engrHere->halu_ward ? 0 : get_num_wards_added(engrHere->ward_id,engrHere->complete_wards);
					obj->ohaluengr = FALSE;
					obj->oward = FALSE;
				}
				else{
					pline("the engraving tumbles off the %s%s.", word, engrHere->ward_id ? "and covers the existing drawing" : "");
					engrHere->ward_id = obj->oward;
					engrHere->halu_ward = obj->ohaluengr;
					engrHere->complete_wards = engrHere->halu_ward ? 1 : get_num_wards_added(engrHere->ward_id,0);
					engrHere->ward_type = obj->blessed ? BURN : obj->cursed ? DUST : ENGRAVE;
					if( !(obj->ohaluengr) && !(u.wardsknown & get_wardID(engrHere->ward_id)) ){
						You("have learned a new warding sign!");
						u.wardsknown |= get_wardID(engrHere->ward_id);
					}
					obj->ohaluengr = FALSE;
					obj->oward = FALSE;
				}
			}
		}
	} else if (!u.dx && !u.dy) {
		if(!(obj->ohaluengr)){
			pline("A %s is engraved on the %s.", wardDecode[obj->oward], word);
			if( !(u.wardsknown & get_wardID(obj->oward)) ){
				You("have learned a new warding sign!");
				u.wardsknown |= get_wardID(obj->oward);
			}
		}
		else{
			pline("There is %s engraved on the %s.", fetchHaluWard((int)obj->oward), word);
		}
		return MOVE_STANDARD;
	} else if (isok(u.ux+u.dx, u.uy+u.dy) && (mtmp = m_at(u.ux+u.dx, u.uy+u.dy)) != 0) {
		You("display the %s engraving to %s.", s_suffix(word), mon_nam(mtmp));
		if (obj->cursed && rn2(3)) {
			pline("But the %s engraving is fogged over!", s_suffix(word));
			return MOVE_STANDARD;
		}
		if(!(obj->ohaluengr) || obj->oward == CERULEAN_SIGN){
			if(
				(obj->oward == HEPTAGRAM && scaryHept(1, mtmp)) ||
				(obj->oward == GORGONEION && scaryGorg(1, mtmp)) ||
				(obj->oward == CIRCLE_OF_ACHERON && scaryCircle(1, mtmp)) ||
				(obj->oward == PENTAGRAM && scaryPent(1, mtmp)) ||
				(obj->oward == HEXAGRAM && scaryHex(1, mtmp)) ||
				(obj->oward == HAMSA && scaryHam(1, mtmp)) ||
				( (obj->oward == ELDER_SIGN || obj->oward == CERULEAN_SIGN) && scarySign(obj->oartifact == ART_STAR_OF_HYPERNOTUS ? 6 : 1, mtmp)) ||
				(obj->oward == ELDER_ELEMENTAL_EYE && scaryEye(1, mtmp)) ||
				(obj->oward == SIGN_OF_THE_SCION_QUEEN && scaryQueen(1, mtmp)) ||
				(obj->oward == CARTOUCHE_OF_THE_CAT_LORD && scaryCat(1, mtmp)) ||
				(obj->oward == WINGS_OF_GARUDA && scaryWings(1, mtmp)) ||
/*				(obj->oward == SIGIL_OF_CTHUGHA && (1, mtmp)) ||
				(obj->oward == BRAND_OF_ITHAQUA && (1, mtmp)) ||
				(obj->oward == TRACERY_OF_KARAKAL && (1, mtmp)) || These wards curently don't have scaryX functions. */
				(obj->oward == YELLOW_SIGN && scaryYellow(1, mtmp)) ||
				(obj->oward == TOUSTEFNA && scaryTou(mtmp)) ||
				(obj->oward == DREPRUN && scaryDre(mtmp)) ||
/*				(obj->oward == OTTASTAFUR && (mtmp)) ||
				(obj->oward == KAUPALOKI && (mtmp)) || Unimplemented runes. */
				(obj->oward == VEIOISTAFUR && scaryVei(mtmp)) ||
				(obj->oward == THJOFASTAFUR && scaryThj(mtmp))
			){
				if (rn2(7))
					monflee(mtmp, rnd(10), TRUE, TRUE);
				else
					monflee(mtmp, rnd(100), TRUE, TRUE);
			}
		} else if(obj->ohaluengr && obj->oward >= FIRST_DROW_SYM && obj->oward <= LAST_DROW_SYM && 
			(is_elf(mtmp->data) || is_drow(mtmp->data) || mtmp->mtyp == PM_EDDERKOP)
		){
			if(flags.stag && 
				(mtmp->mfaction == u.start_house || allied_faction(mtmp->mfaction,u.start_house)) && 
				obj->oward == EDDER_SYMBOL && 
				!(mtmp->female)
			){
				verbalize("The revolution has begun!");
				for(tm = fmon; tm; tm = tm->nmon){
					if((is_drow(tm->data) && (obj->oward == tm->mfaction || allied_faction(obj->oward, tm->mfaction))) || 
						((obj->oward == EDDER_SYMBOL || obj->oward == XAXOX) &&  tm->mtyp == PM_EDDERKOP) ||
						((tm->mfaction == u.start_house || allied_faction(tm->mfaction,u.start_house)) && 
							obj->oward == EDDER_SYMBOL && !(tm->female))
					){
						if(is_drow(tm->data)) set_faction(tm, EDDER_SYMBOL);
						tm->housealert = 1;
						tm->mpeaceful = 1;
					} else if(is_drow(tm->data) && !(obj->oward == tm->mfaction || allied_faction(obj->oward, tm->mfaction)) && mtmp->female){
						tm->housealert = 1;
						tm->mpeaceful = 0;
					}
				}
			} else if((obj->oward == mtmp->mfaction || allied_faction(obj->oward, mtmp->mfaction)) || 
				(obj->oward == EILISTRAEE_SYMBOL && is_elf(mtmp->data)) || 
				((obj->oward == EDDER_SYMBOL || obj->oward == XAXOX) &&  mtmp->mtyp == PM_EDDERKOP)
			){
				if(mtmp->housealert && !(mtmp->mpeaceful)){
					verbalize("Die, spy!");
					for(tm = fmon; tm; tm = tm->nmon){
						if((is_drow(tm->data) && (obj->oward == tm->mfaction || allied_faction(obj->oward, tm->mfaction))) || 
							((obj->oward == EDDER_SYMBOL || obj->oward == XAXOX) &&  tm->mtyp == PM_EDDERKOP) ||
							(obj->oward == EILISTRAEE_SYMBOL && is_elf(tm->data))
						){
							tm->housealert = 1;
							tm->mpeaceful = 0;
						}
					}
				} else if(flags.female){
					if((obj->oward == XAXOX && uarm && uarm->oward && uarm->oward == obj->oward) ||
					   (obj->oward == EDDER_SYMBOL && uarm && uarm->oward && uarm->oward == obj->oward) ||
						!(uarm) || !(uarm->oward) || uarm->oward == obj->oward ||
						(
						 uarm->oward == LOLTH_SYMBOL && 
						 obj->oward != EILISTRAEE_SYMBOL &&   
						 obj->oward != XAXOX &&   
						 obj->oward != EDDER_SYMBOL
						)
					){
						verbalize("She's one of ours!");
						if(obj->oward != XAXOX && obj->oward != EDDER_SYMBOL) verbalize("Apologies, my lady!");
						for(tm = fmon; tm; tm = tm->nmon){
							if((is_drow(tm->data) && (obj->oward == tm->mfaction || allied_faction(obj->oward, tm->mfaction))) || 
								((obj->oward == EDDER_SYMBOL || obj->oward == XAXOX) &&  tm->mtyp == PM_EDDERKOP) ||
								(obj->oward == EILISTRAEE_SYMBOL && is_elf(tm->data))
							){
								tm->housealert = 1;
								tm->mpeaceful = 1;
							}
						}
					} else {
						verbalize("Die, spy!");
						for(tm = fmon; tm; tm = tm->nmon){
							if((is_drow(tm->data) && (obj->oward == tm->mfaction || allied_faction(obj->oward, tm->mfaction))) || 
								((obj->oward == EDDER_SYMBOL || obj->oward == XAXOX) &&  tm->mtyp == PM_EDDERKOP) ||
								(obj->oward == EILISTRAEE_SYMBOL && is_elf(tm->data))
							){
								tm->housealert = 1;
								tm->mpeaceful = 0;
							}
						}
					}
				} else {
					if(((obj->oward <= LAST_TOWER && obj->oward >= FIRST_TOWER) && 
						(!(uarm) || !(uarm->oward) || uarm->oward == obj->oward || allied_faction(uarm->oward,obj->oward))) ||
					   ((obj->oward == EDDER_SYMBOL || 
					     obj->oward == XAXOX || 
					     obj->oward == GHAUNADAUR_SYMBOL || 
					     obj->oward == LAST_BASTION_SYMBOL || 
						 obj->oward == EILISTRAEE_SYMBOL) && (!(uarm) || !(uarm->oward) || uarm->oward == obj->oward || allied_faction(uarm->oward,obj->oward))) ||
					   (uarm && uarm->oward && uarm->oward == obj->oward)
					){
						verbalize("He's one of ours!");
						verbalize("Move along, sir.");
						for(tm = fmon; tm; tm = tm->nmon){
							if((is_drow(tm->data) && (obj->oward == tm->mfaction || allied_faction(obj->oward, tm->mfaction))) || 
								((obj->oward == EDDER_SYMBOL || obj->oward == XAXOX) &&  tm->mtyp == PM_EDDERKOP) ||
								(obj->oward == EILISTRAEE_SYMBOL && is_elf(tm->data))
							){
								tm->mpeaceful = 1;
							}
						}
					} else {
						verbalize("Die, spy!");
						for(tm = fmon; tm; tm = tm->nmon){
							if((is_drow(tm->data) && (obj->oward == tm->mfaction || allied_faction(obj->oward, tm->mfaction))) || 
								((obj->oward == EDDER_SYMBOL || obj->oward == XAXOX) &&  tm->mtyp == PM_EDDERKOP) ||
								(obj->oward == EILISTRAEE_SYMBOL && is_elf(tm->data))
							){
								tm->housealert = 1;
								tm->mpeaceful = 0;
							}
						}
					}
				}
			} else {
				verbalize("Die!");
				mtmp->housealert = 1;
				for(tm = fmon; tm; tm = tm->nmon){
					if((is_drow(tm->data) && (mtmp->mfaction == tm->mfaction || allied_faction(mtmp->mfaction, tm->mfaction))) || 
						((mtmp->mfaction == EDDER_SYMBOL || mtmp->mfaction == XAXOX || tm->mtyp == PM_EDDERKOP) && 
							(tm->mfaction == EDDER_SYMBOL || tm->mfaction == XAXOX || tm->mtyp == PM_EDDERKOP)) ||
						((mtmp->mfaction == EILISTRAEE_SYMBOL || is_elf(mtmp->data)) && 
							(tm->mfaction == EILISTRAEE_SYMBOL || is_elf(tm->data)))
					){
						tm->housealert = 1;
						tm->mpeaceful = 0;
					}
				}
			}
		}
	}
	return MOVE_STANDARD;
}

STATIC_OVL int
use_towel(obj)
	struct obj *obj;
{
	if(!freehand()) {
		You("have no free %s!", body_part(HAND));
		return MOVE_CANCELLED;
	} else if (obj->owornmask) {
		You("cannot use it while you're wearing it!");
		return MOVE_CANCELLED;
	} else if (obj->cursed) {
		long old;
		switch (rn2(3)) {
		case 2:
		    old = Glib;
		    Glib += rn1(10, 3);
		    Your("%s %s!", makeplural(body_part(HAND)),
			(old ? "are filthier than ever" : "get slimy"));
		    return MOVE_STANDARD;
		case 1:
		    if (!ublindf) {
			old = u.ucreamed;
			u.ucreamed += rn1(10, 3);
			pline("Yecch! Your %s %s gunk on it!", body_part(FACE),
			      (old ? "has more" : "now has"));
			make_blinded(Blinded + (long)u.ucreamed - old, TRUE);
		    } else {
			const char *what = (ublindf->otyp == LENSES || ublindf->otyp == SUNGLASSES) ?
					    "lenses" : (ublindf->otyp == MASK || ublindf->otyp == LIVING_MASK || ublindf->otyp == R_LYEHIAN_FACEPLATE) ? "mask" : "blindfold";
			if (ublindf->cursed) {
			    You("push your %s %s.", what,
				rn2(2) ? "cock-eyed" : "crooked");
			} else {
			    struct obj *saved_ublindf = ublindf;
			    You("push your %s off.", what);
			    Blindf_off(ublindf);
			    dropx(saved_ublindf);
			}
		    }
		    return MOVE_STANDARD;
		case 0:
		    break;
		}
	}

	if (Glib) {
		Glib = 0;
		You("wipe off your %s.", makeplural(body_part(HAND)));
		return MOVE_STANDARD;
	} else if(u.ucreamed) {
		Blinded -= u.ucreamed;
		u.ucreamed = 0;

		if (!Blinded) {
			pline("You've got the glop off.");
			Blinded = 1;
			make_blinded(0L,TRUE);
		} else {
			Your("%s feels clean now.", body_part(FACE));
		}
		return MOVE_STANDARD;
	}

	Your("%s and %s are already clean.",
		body_part(FACE), makeplural(body_part(HAND)));

	return MOVE_CANCELLED;
}

/* maybe give a stethoscope message based on floor objects */
STATIC_OVL boolean
its_dead(rx, ry, resp, tobj)
int rx, ry, *resp;
struct obj* tobj;
{
	struct obj *otmp;
	struct trap *ttmp;

	if (!can_reach_floor()) return FALSE;

	/* additional stethoscope messages from jyoung@apanix.apana.org.au */
	if (Hallucination && sobj_at(CORPSE, rx, ry)) {
	    /* (a corpse doesn't retain the monster's sex,
	       so we're forced to use generic pronoun here) */
	    You_hear("a voice say, \"It's dead, Jim.\"");
	    *resp = 1;
	    return TRUE;
	} else if (Role_if(PM_HEALER) && ((otmp = sobj_at(CORPSE, rx, ry)) != 0 ||
				    (otmp = sobj_at(STATUE, rx, ry)) != 0 || (otmp = sobj_at(FOSSIL, rx, ry)) != 0)) {
	    /* possibly should check uppermost {corpse,statue} in the pile
	       if both types are present, but it's not worth the effort */
	    if (vobj_at(rx, ry)->otyp == STATUE) otmp = vobj_at(rx, ry);
	    if (otmp->otyp == CORPSE || otmp->otyp == FOSSIL) {
		You("determine that %s unfortunate being is %sdead.",
		    (rx == u.ux && ry == u.uy) ? "this" : "that", (otmp->otyp == FOSSIL) ? "very ":"");
	    } else {
		ttmp = t_at(rx, ry);
		pline("%s appears to be in %s health for a statue.",
		      The(mons[otmp->corpsenm].mname),
		      (ttmp && ttmp->ttyp == STATUE_TRAP) ?
			"extraordinary" : "excellent");
	    }
	    return TRUE;
	}

	/* listening to eggs is a little fishy, but so is stethoscopes detecting alignment
	 * The overcomplex wording is because all the monster-naming functions operate
	 * on actual instances of the monsters, and we're dealing with just an index
	 * so we can avoid things like "a owlbear", etc. */
	if ((otmp = sobj_at(EGG,rx,ry))) {
		if (Hallucination) {
			pline("You listen to the egg and guess... %s?",rndmonnam());
		} else {
			if (stale_egg(otmp) || otmp->corpsenm == NON_PM) {
				pline("The egg doesn't make much noise at all.");
			} else {
				pline("You listen to the egg and guess... %s?",mons[otmp->corpsenm].mname);
				learn_egg_type(otmp->corpsenm);
			}
		}
		return TRUE;
	}


	return FALSE;
}

static const char hollow_str[] = "a hollow sound.  This must be a secret %s!";

/* Strictly speaking it makes no sense for usage of a stethoscope to
   not take any time; however, unless it did, the stethoscope would be
   almost useless.  As a compromise, one use per turn is free, another
   uses up the turn; this makes curse status have a tangible effect. */
STATIC_OVL int
use_stethoscope(obj)
	register struct obj *obj;
{
	struct monst *mtmp;
	struct rm *lev;
	int rx, ry, res;
	boolean interference = (u.uswallow && is_whirly(u.ustuck->data) &&
				!rn2(Role_if(PM_HEALER) ? 10 : 3));

	if (nohands(youracedata)) {	/* should also check for no ears and/or deaf */
		You("have no hands!");	/* not `body_part(HAND)' */
		return MOVE_CANCELLED;
	} else if (!freehand()) {
		You("have no free %s.", body_part(HAND));
		return MOVE_CANCELLED;
	}
	if (!getdir((char *)0)) return MOVE_CANCELLED;

	res = MOVE_PARTIAL;

#ifdef STEED
	if (u.usteed && u.dz > 0) {
		if (interference) {
			pline("%s interferes.", Monnam(u.ustuck));
			mstatusline(u.ustuck);
		} else
			mstatusline(u.usteed);
		return res;
	} else
#endif
	if (u.uswallow && (u.dx || u.dy || u.dz)) {
		mstatusline(u.ustuck);
		return res;
	} else if (u.uswallow && interference) {
		pline("%s interferes.", Monnam(u.ustuck));
		mstatusline(u.ustuck);
		return res;
	} else if (u.dz) {
		if (Underwater)
		    You_hear("faint splashing.");
		else if (u.dz < 0 || !can_reach_floor())
		    You_cant("reach the %s.",
			(u.dz > 0) ? surface(u.ux,u.uy) : ceiling(u.ux,u.uy));
		else if (its_dead(u.ux, u.uy, &res, obj))
		    ;	/* message already given */
		else if (Is_stronghold(&u.uz))
		    You_hear("the crackling of hellfire.");
		else
		    pline_The("%s seems healthy enough.", surface(u.ux,u.uy));
		return res;
	} else if (obj->cursed && !rn2(2)) {
		You_hear("your %s %s.", body_part(HEART), body_part(BEAT));
		return res;
	}
	if (Stunned || (Confusion && !rn2(5))) confdir();
	if (!u.dx && !u.dy) {
		ustatusline();
		return res;
	}
	rx = u.ux + u.dx; ry = u.uy + u.dy;
	if (!isok(rx,ry)) {
		You_hear("a faint typing noise.");
		return MOVE_INSTANT;
	}
	if ((mtmp = m_at(rx,ry)) != 0) {
		mstatusline(mtmp);
		if (mtmp->mundetected) {
			mtmp->mundetected = 0;
			if (cansee(rx,ry)) newsym(mtmp->mx,mtmp->my);
		}
		if (!canspotmon(mtmp))
			map_invisible(rx,ry);
		return res;
	}
	if (glyph_is_invisible(levl[rx][ry].glyph)) {
		unmap_object(rx, ry);
		newsym(rx, ry);
		pline_The("invisible monster must have moved.");
	}
	lev = &levl[rx][ry];
	switch(lev->typ) {
	case SDOOR:
		You_hear(hollow_str, "door");
		cvt_sdoor_to_door(lev);		/* ->typ = DOOR */
		if (Blind) feel_location(rx,ry);
		else newsym(rx,ry);
		return res;
	case SCORR:
		You_hear(hollow_str, "passage");
		lev->typ = CORR;
		unblock_point(rx,ry);
		if (Blind) feel_location(rx,ry);
		else newsym(rx,ry);
		return res;
	}

	if (!its_dead(rx, ry, &res, obj))
	    You_hear("nothing special.");	/* not You_hear()  */
	return res;
}

static const char whistle_str[] = "produce a %s whistling sound.";

STATIC_OVL void
use_whistle(obj)
struct obj *obj;
{
	register struct monst *mtmp;

	You(whistle_str, obj->cursed ? "shrill" : "high");
	wake_nearby_noisy();
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    if (!DEADMONSTER(mtmp)) {
			if (get_mx(mtmp, MX_EDOG))
				EDOG(mtmp)->whistletime = moves;
	    }
	}
}

void
use_magic_whistle(obj)
struct obj *obj;
{
	register struct monst *mtmp, *nextmon;

	if(obj->cursed && !rn2(2)) {
		You("produce a high-pitched humming noise.");
		wake_nearby();
	} else {
		int pet_cnt = 0;
		You(whistle_str, Hallucination ? "normal" : "strange");
		for(mtmp = fmon; mtmp; mtmp = nextmon) {
		    nextmon = mtmp->nmon; /* trap might kill mon */
		    if (DEADMONSTER(mtmp)) continue;
		    if (mtmp->mtame) {
			if (mtmp->mtrapped) {
			    /* no longer in previous trap (affects mintrap) */
			    mtmp->mtrapped = 0;
			    fill_pit(mtmp->mx, mtmp->my);
			}
			mnexto(mtmp);
			if (canspotmon(mtmp)) ++pet_cnt;
			if (mintrap(mtmp) == 2) change_luck(-1);
		    }
		}
		if (pet_cnt > 0) makeknown(obj->otyp);
	}
}

boolean
um_dist(x,y,n)
register xchar x, y, n;
{
	return((boolean)(abs(u.ux - x) > n  || abs(u.uy - y) > n));
}

int
number_leashed()
{
	register int i = 0;
	register struct obj *obj;

	for(obj = invent; obj; obj = obj->nobj)
		if(obj->otyp == LEASH && obj->leashmon != 0) i++;
	return(i);
}

void
o_unleash(otmp)		/* otmp is about to be destroyed or stolen */
register struct obj *otmp;
{
	register struct monst *mtmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		if(mtmp->m_id == (unsigned)otmp->leashmon)
			mtmp->mleashed = 0;
	otmp->leashmon = 0;
}

void
m_unleash(mtmp, feedback)	/* mtmp is about to die, or become untame */
register struct monst *mtmp;
boolean feedback;
{
	register struct obj *otmp;

	if (feedback) {
	    if (canseemon(mtmp))
		pline("%s pulls free of %s leash!", Monnam(mtmp), mhis(mtmp));
	    else
		Your("leash falls slack.");
	}
	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == LEASH &&
				otmp->leashmon == (int)mtmp->m_id)
			otmp->leashmon = 0;
	mtmp->mleashed = 0;
}

void
unleash_all()		/* player is about to die (for bones) */
{
	register struct obj *otmp;
	register struct monst *mtmp;

	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(otmp->otyp == LEASH) otmp->leashmon = 0;
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		mtmp->mleashed = 0;
}

#define MAXLEASHED	2

/* ARGSUSED */
STATIC_OVL void
use_leash(obj)
struct obj *obj;
{
	coord cc;
	register struct monst *mtmp;
	int spotmon;

	if(!obj->leashmon && number_leashed() >= MAXLEASHED) {
		You("cannot leash any more pets.");
		return;
	}

	if(!get_adjacent_loc((char *)0, (char *)0, u.ux, u.uy, &cc)) return;

	if((cc.x == u.ux) && (cc.y == u.uy)) {
#ifdef STEED
		if (u.usteed && u.dz > 0) {
		    mtmp = u.usteed;
		    spotmon = 1;
		    goto got_target;
		}
#endif
		pline("Leash yourself?  Very funny...");
		return;
	}

	if(!(mtmp = m_at(cc.x, cc.y))) {
		There("is no creature there.");
		return;
	}

	spotmon = canspotmon(mtmp);
#ifdef STEED
 got_target:
#endif

	if(!mtmp->mtame) {
	    if(!spotmon)
		There("is no creature there.");
	    else
		pline("%s %s leashed!", Monnam(mtmp), (!obj->leashmon) ?
				"cannot be" : "is not");
	    return;
	}
	if(!obj->leashmon) {
		if(mtmp->mleashed) {
			pline("This %s is already leashed.",
			      spotmon ? l_monnam(mtmp) : "monster");
			return;
		}
		You("slip the leash around %s%s.",
		    spotmon ? "your " : "", l_monnam(mtmp));
		mtmp->mleashed = 1;
		obj->leashmon = (int)mtmp->m_id;
		mtmp->msleeping = 0;
		return;
	}
	if(obj->leashmon != (int)mtmp->m_id) {
		pline("This leash is not attached to that creature.");
		return;
	} else {
		if(obj->cursed) {
			pline_The("leash would not come off!");
			obj->bknown = TRUE;
			return;
		}
		mtmp->mleashed = 0;
		obj->leashmon = 0;
		You("remove the leash from %s%s.",
		    spotmon ? "your " : "", l_monnam(mtmp));
	}
	return;
}

struct obj *
get_mleash(mtmp)	/* assuming mtmp->mleashed has been checked */
register struct monst *mtmp;
{
	register struct obj *otmp;

	otmp = invent;
	while(otmp) {
		if(otmp->otyp == LEASH && otmp->leashmon == (int)mtmp->m_id)
			return(otmp);
		otmp = otmp->nobj;
	}
	return((struct obj *)0);
}

#endif /* OVLB */
#ifdef OVL1

boolean
next_to_u()
{
	register struct monst *mtmp;
	register struct obj *otmp;

	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		if(mtmp->mleashed) {
			if (distu(mtmp->mx,mtmp->my) > 2) mnexto(mtmp);
			if (distu(mtmp->mx,mtmp->my) > 2) {
			    for(otmp = invent; otmp; otmp = otmp->nobj)
				if(otmp->otyp == LEASH &&
					otmp->leashmon == (int)mtmp->m_id) {
				    if(otmp->cursed) return(FALSE);
				    You_feel("%s leash go slack.",
					(number_leashed() > 1) ? "a" : "the");
				    mtmp->mleashed = 0;
				    otmp->leashmon = 0;
				}
			}
		}
	}
#ifdef STEED
	/* no pack mules for the Amulet */
	if (u.usteed && mon_has_amulet(u.usteed)) return FALSE;
#endif
	return(TRUE);
}

#endif /* OVL1 */
#ifdef OVL0

void
check_leash(x, y)
register xchar x, y;
{
	register struct obj *otmp;
	register struct monst *mtmp;

	for (otmp = invent; otmp; otmp = otmp->nobj) {
	    if (otmp->otyp != LEASH || otmp->leashmon == 0) continue;
	    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		if ((int)mtmp->m_id == otmp->leashmon) break; 
	    }
	    if (!mtmp) {
		impossible("leash in use isn't attached to anything?");
		otmp->leashmon = 0;
		continue;
	    }
	    if (dist2(u.ux,u.uy,mtmp->mx,mtmp->my) >
		    dist2(x,y,mtmp->mx,mtmp->my)) {
		if (!um_dist(mtmp->mx, mtmp->my, 3)) {
		    ;	/* still close enough */
		} else if (otmp->cursed && !breathless_mon(mtmp)) {
		    if (um_dist(mtmp->mx, mtmp->my, 5) ||
			    (mtmp->mhp -= rnd(2)) <= 0) {
			long save_pacifism = u.uconduct.killer;

			Your("leash chokes %s to death!", mon_nam(mtmp));
			/* hero might not have intended to kill pet, but
			   that's the result of his actions; gain experience,
			   lose pacifism, take alignment and luck hit, make
			   corpse less likely to remain tame after revival */
			xkilled(mtmp, 0);	/* no "you kill it" message */
			/* life-saving doesn't ordinarily reset this */
			if (mtmp->mhp > 0) u.uconduct.killer = save_pacifism;
		    } else {
			pline("%s chokes on the leash!", Monnam(mtmp));
			/* tameness eventually drops to 1 here (never 0) */
			if (mtmp->mtame && rn2(mtmp->mtame)) mtmp->mtame--;
			if(mtmp->mtyp == PM_CROW && u.sealsActive&SEAL_MALPHAS) unbind(SEAL_MALPHAS,TRUE);
		    }
		} else {
		    if (um_dist(mtmp->mx, mtmp->my, 5)) {
			pline("%s leash snaps loose!", s_suffix(Monnam(mtmp)));
			m_unleash(mtmp, FALSE);
		    } else {
			You("pull on the leash.");
			if (!is_silent_mon(mtmp))
			    switch (rn2(3)) {
			    case 0:  growl(mtmp);   break;
			    case 1:  yelp(mtmp);    break;
			    default: whimper(mtmp); break;
			    }
		    }
		}
	    }
	}
}

#endif /* OVL0 */
#ifdef OVLB

#define WEAK	3	/* from eat.c */

static const char look_str[] = "look %s.";

STATIC_OVL int
use_mirror(obj_p)
struct obj **obj_p;
{
	register struct monst *mtmp;
	register char mlet;
	boolean vis;
	struct obj *obj = *obj_p;

	if(!getdir((char *)0)) return MOVE_CANCELLED;
	if(obj->cursed && !rn2(2)) {
		if (!Blind)
			pline_The("mirror fogs up and doesn't reflect!");
		return MOVE_STANDARD;
	}
	if(!u.dx && !u.dy && !u.dz) {
		if(obj->oartifact == ART_HAND_MIRROR_OF_CTHYLLA && obj->age < moves && !Blind){
			pline("An incomprehensible sight meets your eyes!");
			losehp(d(15,15), "looking into Cthylla's hand-mirror", KILLED_BY);
			obj->age = monstermoves + (long)(rnz(100)*(Role_if(PM_PRIEST) ? .8 : 1));
		} else if(!Blind) {
			if (youracedata->mlet == S_VAMPIRE || Invisible) {
				You("don't have a reflection.");
				vis = FALSE;
		    } else if (u.umonnum == PM_FLOATING_EYE && ward_at(u.ux, u.uy) != HAMSA) {
				if (!Free_action) {
					pline("%s", Hallucination ?
						  "Yow!  The mirror stares back!" :
						  "Yikes!  You've frozen yourself!");
					nomul(-rnd((MAXULEV+6) - u.ulevel), "frozen by your own reflection");
					vis = FALSE;
				}
				else {
					You("stiffen momentarily under your gaze.");
					vis = TRUE;
				}
			} else if (u.umonnum == PM_UMBER_HULK && ward_at(u.ux, u.uy) != HAMSA) {
				pline("Huh?  That doesn't look like you!");
				make_confused(HConfusion + d(3,4),FALSE);
				vis = FALSE;
		    } else if (u.sealsActive&SEAL_IRIS){
				pline("What?  Who is that in the mirror!?");
				unbind(SEAL_IRIS,TRUE);
				vis = FALSE;
		    } else if (roll_madness(MAD_ARGENT_SHEEN)) {
				You("admire your reflection in the mirror.");
				nomul(-1*rnd(6), "posing in front of a mirror.");
				vis = FALSE;
		    } else if (Hallucination) {
				You(look_str, hcolor((char *)0));
				vis = TRUE;
		    } else if (Sick) {
				You(look_str, "peaked");
				vis = TRUE;
		    } else if (u.uhs >= WEAK) {
				You(look_str, "undernourished");
				vis = TRUE;
			} else {
				You("look as %s as ever.",
					ACURR(A_CHA) > 14 ?
					(poly_gender() == 1 ? "beautiful" : "handsome") :
					"ugly");
				vis = TRUE;
			}
			if (vis){
				signs_mirror();
			}
			if(u.uinsight >= 20 && !obj->oartifact){
				// if(wizard)
					// pline("silver flame d: %d, l: %d, x:%d, y:%d", u.silver_flame_z.dnum, u.silver_flame_z.dlevel, u.s_f_x, u.s_f_y);
				if(u.uz.dnum == u.silver_flame_z.dnum){
					if(u.silver_flame_z.dlevel > u.uz.dlevel){
						if(!u.silver_atten)
							You("notice a silver light %sbelow you.", ((u.silver_flame_z.dlevel-u.uz.dlevel) > 10) ? "deep " : "");
					}
					else if(u.silver_flame_z.dlevel < u.uz.dlevel){
						if(!u.silver_atten)
							You("notice a silver light %sabove you.", ((u.silver_flame_z.dlevel-u.uz.dlevel) > 10) ? "high " : "");
					}
					else {
						int dx = u.ux - u.s_f_x, 
							dy = u.uy - u.s_f_y;
						int absx = abs(dx),
							absy = abs(dy);
						if(!dx && !dy){
							if(u.silver_atten)
								You("have returned to the silver flame.");
							else
								pline("A volcanic pillar of silver flame spouts forth here, rising arrow-straight from the profound and inconceivable depths to the empty and unknown heavens!");
							if(yn("Offer an implement to the fire?") == 'y'){
								const char sflm_classes[] = { WEAPON_CLASS, TOOL_CLASS, ARMOR_CLASS, 0 };
								struct obj *sflm_obj = getobj(sflm_classes, "offer to the flame");
								if(sflm_obj){
									if(sflm_offerable(sflm_obj)){
										pline("The silver light reflects from your mirror and takes up residence within %s.", doname(sflm_obj));
										add_oprop(sflm_obj, OPROP_SFLMW);
										u.silver_atten = TRUE;
										*obj_p = poly_obj(obj, PURIFIED_MIRROR);
									}
									else pline("Nothing happens.");
								}
							}
						}
						else if(absx <= 4 && absy <= 4){
							if(absx && absy){
								You("see a column of silver fire %d step%s %s and %d step%s %s.", absx, absx > 1 ? "s" : "", dx > 0 ? "west" : "east",
																								   absy, absy > 1 ? "s" : "", dy > 0 ? "north" : "south");
							}
							else {
								//Note: One of the distances is 0, figure out which one isn't
								int absd = absx ? absx : absy;
								You("see a column of silver fire %d step%s %s.", absd, absd > 1 ? "s" : "", dx > 0 ? "west" : dx < 0 ? "east" : dy > 0 ? "north" : "south");
							}
						}
						else {
							char *distword = (absx > 10 || absy > 10) ? "far " : "";
							char *dirword;
							//Note: at least one of absx and absy is > 0 (> 4 in fact)
							if(dy > 0){
								//North, or maybe east or west
								if(absy > 2*absx)
									dirword = "north";
								else if(absx > 2*absy){
									if(dx > 0)
										dirword = "west";
									else
										dirword = "east";
								}
								else {
									if(dx > 0)
										dirword = "north-west";
									else
										dirword = "north-east";
								}
							}
							else {
								//South, or maybe east or west
								if(absy > 2*absx)
									dirword = "south";
								else if(absx > 2*absy){
									if(dx > 0)
										dirword = "west";
									else
										dirword = "east";
								}
								else {
									if(dx > 0)
										dirword = "south-west";
									else
										dirword = "south-east";
								}
							}
							You("notice a silver light %sto the %s.", distword, dirword);
						}
					}
				}
			}
		} else {
			You_cant("see your %s %s.",
				ACURR(A_CHA) > 14 ?
				(poly_gender()==1 ? "beautiful" : "handsome") :
				"ugly",
				body_part(FACE));
		}
		return MOVE_STANDARD;
	}
	if(u.uswallow) {
		if (!Blind) You("reflect %s %s.", s_suffix(mon_nam(u.ustuck)),
		    mbodypart(u.ustuck, STOMACH));
		return MOVE_STANDARD;
	}
	if(Underwater) {
		You(Hallucination ?
		    "give the fish a chance to fix their makeup." :
		    "reflect the murky water.");
		return MOVE_STANDARD;
	}
	if(u.dz) {
		if (!Blind)
		    You("reflect the %s.",
			(u.dz > 0) ? surface(u.ux,u.uy) : ceiling(u.ux,u.uy));
		return MOVE_STANDARD;
	}
	mtmp = bhit(u.dx, u.dy, COLNO, INVIS_BEAM,
		    (int FDECL((*),(MONST_P,OBJ_P)))0,
		    (int FDECL((*),(OBJ_P,OBJ_P)))0,
		    obj, NULL);
	if (!mtmp || !haseyes(mtmp->data))
		return MOVE_STANDARD;

	vis = canseemon(mtmp);
	mlet = mtmp->data->mlet;
	if (mtmp->msleeping) {
		if (vis)
		    pline ("%s is too tired to look at your mirror.",
			    Monnam(mtmp));
	} else if (is_blind(mtmp)) {
	    if (vis)
		pline("%s can't see anything right now.", Monnam(mtmp));
	/* some monsters do special things */
	} else if (mlet == S_VAMPIRE || mlet == S_GHOST || mlet == S_SHADE) {
	    if (vis)
		pline ("%s doesn't have a reflection.", Monnam(mtmp));
	} else if(obj->oartifact == ART_HAND_MIRROR_OF_CTHYLLA && obj->age < moves &&
				(!mtmp->minvis || mon_resistance(mtmp,SEE_INVIS))
	){
		obj->age = monstermoves + (long)(rnz(100)*(Role_if(PM_PRIEST) ? .8 : 1));
		if (vis)
		    pline("%s is blasted by its reflection.", Monnam(mtmp));
		monflee(mtmp, d(2,4), FALSE, FALSE);
		mtmp->mhp -= d(15,15);
		if (mtmp->mhp <= 0){
			mtmp->mhp = 0;
			xkilled(mtmp, 1);
		}
	} else if(!mtmp->mcan && !mtmp->minvis &&
					mtmp->mtyp == PM_MEDUSA  && 
					ward_at(mtmp->mx,mtmp->my) != HAMSA) {
		if (mon_reflects(mtmp, "The gaze is reflected away by %s %s!"))
			return MOVE_STANDARD;
	} else if(!mtmp->mcan && !mtmp->minvis &&
					mtmp->mtyp == PM_FLOATING_EYE && 
					ward_at(mtmp->mx,mtmp->my) != HAMSA) {
		int tmp = d(min(MAX_BONUS_DICE, (int)mtmp->m_lev), (int)mtmp->data->mattk[0].damd);
		if (!rn2(4)) tmp = 120;
		if (vis)
			pline("%s is frozen by its reflection.", Monnam(mtmp));
		else You_hear("%s stop moving.",something);
		mtmp->mcanmove = 0;
		if ( (int) mtmp->mfrozen + tmp > 127)
			mtmp->mfrozen = 127;
		else mtmp->mfrozen += tmp;
	} else if(!mtmp->mcan && !mtmp->minvis &&
					mtmp->mtyp == PM_UMBER_HULK && 
					ward_at(mtmp->mx,mtmp->my) != HAMSA) {
		if (vis)
			pline ("%s confuses itself!", Monnam(mtmp));
		mtmp->mconf = 1;
	} else if(!mtmp->mcan && !mtmp->minvis && (mlet == S_NYMPH
				     || mtmp->mtyp==PM_SUCCUBUS)) {
		if (vis) {
		    pline ("%s admires herself in your mirror.", Monnam(mtmp));
		    pline ("She takes it!");
		} else pline ("It steals your mirror!");
		setnotworn(obj); /* in case mirror was wielded */
		freeinv(obj);
		(void) mpickobj(mtmp,obj);
		if (!tele_restrict(mtmp)) (void) rloc(mtmp, TRUE);
	} else if(!mtmp->mcan && !mtmp->minvis && is_weeping(mtmp->data)) {
		if (vis)
			pline ("%s stares at its reflection with a stony expression.", Monnam(mtmp));
		mtmp->mcanmove = 0;
		mtmp->mfrozen = 1;
	} else if (!is_unicorn(mtmp->data) && is_animal(mtmp->data) &&
			(!mtmp->minvis || mon_resistance(mtmp,SEE_INVIS)) && rn2(5)) {
		if (vis)
		    pline("%s is frightened by its reflection.", Monnam(mtmp));
		monflee(mtmp, d(2,4), FALSE, FALSE);
	} else if (!Blind) {
		if (mtmp->minvis && !See_invisible(mtmp->mx, mtmp->my))
		    ;
		else if ((mtmp->minvis && !mon_resistance(mtmp,SEE_INVIS))
			 || !haseyes(mtmp->data))
		    pline("%s doesn't seem to notice its reflection.",
			Monnam(mtmp));
		else
		    pline("%s ignores %s reflection.",
			  Monnam(mtmp), mhis(mtmp));
	}
	return MOVE_STANDARD;
}

void
use_bell(optr, spiritseal)
struct obj **optr;
int spiritseal;
{
	register struct obj *obj = *optr;
	struct monst *mtmp;
	boolean wakem = FALSE, learno = FALSE,
		ordinary = (obj->otyp != BELL_OF_OPENING || (!obj->spe && !spiritseal)),
		invoking = (obj->otyp == BELL_OF_OPENING &&
			(spiritseal ?
			 ((invocation_pos(obj->ox, obj->oy) && !On_stairs(obj->ox, obj->oy)) || (invocation_pos(u.ux, u.uy) && !On_stairs(u.ux, u.uy))) :
			 (invocation_pos(u.ux, u.uy) && !On_stairs(u.ux, u.uy))
			));

	if(!spiritseal) You("ring %s.", the(xname(obj)));
	
	if(Role_if(PM_EXILE) && obj->otyp == BELL_OF_OPENING && !spiritseal){
		pline("It makes a rather sad clonk.");
		return;
	}
	
	if (Underwater || (u.uswallow && ordinary)) {
#ifdef	AMIGA
	    amii_speaker( obj, "AhDhGqEqDhEhAqDqFhGw", AMII_MUFFLED_VOLUME );
#endif
	    pline("But the sound is muffled.");

	} else if (invoking && ordinary) {
	    /* needs to be recharged... */
	    pline("But it makes no sound.");
	    learno = TRUE;	/* help player figure out why */

	} else if (ordinary) {
#ifdef	AMIGA
	    amii_speaker( obj, "ahdhgqeqdhehaqdqfhgw", AMII_MUFFLED_VOLUME );
#endif
	    if (obj->cursed && !rn2(4) &&
		    /* note: once any of them are gone, we stop all of them */
		    !(mvitals[PM_DRYAD].mvflags & G_GONE && !In_quest(&u.uz)) &&
		    !(mvitals[PM_NAIAD].mvflags & G_GONE && !In_quest(&u.uz)) &&
		    !(mvitals[PM_OREAD].mvflags & G_GONE && !In_quest(&u.uz)) &&
		    (mtmp = makemon(mkclass(S_NYMPH, Inhell ? G_HELL : G_NOHELL),
					u.ux, u.uy, NO_MINVENT)) != 0) {
		You("summon %s!", a_monnam(mtmp));
		if (!obj_resists(obj, 0, 100)) {
		    pline("%s shattered!", Tobjnam(obj, "have"));
		    useup(obj);
		    *optr = 0;
		} else switch (rn2(3)) {
			default:
				break;
			case 1:
				mon_adjust_speed(mtmp, 2, (struct obj *)0, TRUE);
				break;
			case 2: /* no explanation; it just happens... */
				nomovemsg = "";
				nomul(-rnd(2), "ringing a bell");
				break;
		}
	    }
	    wakem = TRUE;

	} else {
	    /* charged Bell of Opening */
	    consume_obj_charge(obj, TRUE);
		
		if(uwep && uwep->oartifact == ART_SINGING_SWORD){
			uwep->ovar1_heard |= OHEARD_OPEN;
		}
		
	    if (u.uswallow) {
		if (!obj->cursed)
		    (void) openit();
		else
		    pline("%s", nothing_happens);

	    } else if (obj->cursed) {
		coord mm;

		mm.x = u.ux;
		mm.y = u.uy;
		pline("Graves open around you...");
		mkundead(&mm, FALSE, NO_MINVENT, 0);
		wakem = TRUE;

	    } else  if (invoking) {
		pline("%s an unsettling shrill sound...",
		      Tobjnam(obj, "issue"));
#ifdef	AMIGA
		amii_speaker( obj, "aefeaefeaefeaefeaefe", AMII_LOUDER_VOLUME );
#endif
		if(spiritseal && !obj->cursed) u.rangBell = moves;
		obj->age = moves;
		learno = TRUE;
		wakem = TRUE;

	    } else if (obj->blessed) {
		int res = 0;

#ifdef	AMIGA
		amii_speaker( obj, "ahahahDhEhCw", AMII_SOFT_VOLUME );
#endif
		if (uchain) {
		    unpunish();
		    res = 1;
		}
		res += openit();
		switch (res) {
		  case 0:  pline("%s", nothing_happens); break;
		  case 1:  pline("%s opens...", Something);
			   learno = TRUE; break;
		  default: pline("Things open around you...");
			   learno = TRUE; break;
		}

	    } else {  /* uncursed */
#ifdef	AMIGA
		amii_speaker( obj, "AeFeaeFeAefegw", AMII_OKAY_VOLUME );
#endif
		if (findit() != 0) learno = TRUE;
		else pline("%s", nothing_happens);
	    }

	}	/* charged BofO */

	if (learno) {
	    makeknown(BELL_OF_OPENING);
	    obj->known = 1;
	}
	if (wakem) wake_nearby_noisy();
}

STATIC_OVL void
use_candelabrum(obj)
register struct obj *obj;
{
	const char *s = (obj->spe != 1) ? "candles" : "candle";

	if(Underwater) {
		You("cannot make fire under water.");
		return;
	}
	if(obj->lamplit) {
		You("snuff the %s.", s);
		end_burn(obj, TRUE);
		return;
	}
	if(obj->spe <= 0) {
		pline("This %s has no %s.", xname(obj), s);
		return;
	}
	if(u.uswallow || obj->cursed) {
		if (!Blind)
		    pline_The("%s %s for a moment, then %s.",
			      s, vtense(s, "flicker"), vtense(s, "die"));
		return;
	}
	if(obj->spe < 7) {
		There("%s only %d %s in %s.",
		      vtense(s, "are"), obj->spe, s, the(xname(obj)));
		if (!Blind)
		    pline("%s lit.  %s dimly.",
			  obj->spe == 1 ? "It is" : "They are",
			  Tobjnam(obj, "shine"));
	} else {
		pline("%s's %s burn%s", The(xname(obj)), s,
			(Blind ? "." : " brightly!"));
	}
	if (!invocation_pos(u.ux, u.uy)) {
		pline_The("%s %s being rapidly consumed!", s, vtense(s, "are"));
		/* this used to be obj->age /= 2, rounding down; an age of
		   1 would yield 0, confusing begin_burn() and producing an
		   unlightable, unrefillable candelabrum; round up instead */
		obj->age = (obj->age + 1L) / 2L;

		/* to make absolutely sure the game doesn't become unwinnable as
		   a consequence of a broken candelabrum */
		if (obj->age == 0) {
			impossible("Candelabrum with candles but no fuel?");
			obj->age = 1;
		}
	} else {
		if(obj->spe == 7) {
		    if (Blind)
		      pline("%s a strange warmth!", Tobjnam(obj, "radiate"));
		    else
		      pline("%s with a strange light!", Tobjnam(obj, "glow"));
		}
		obj->known = 1;
	}
	begin_burn(obj);
}

STATIC_OVL void
use_candle(optr)
struct obj **optr;
{
	register struct obj *obj = *optr;
	register struct obj *otmp;
	const char *s = (obj->quan != 1) ? "candles" : "candle";
	char qbuf[QBUFSZ];
	boolean was_lamplit;

	if (u.uswallow){
		if(!is_whirly(u.ustuck->data)) {
			You(no_elbow_room);
			return;
		} else if(!obj->lamplit){
			You("can't get the %s to light.  It's quite hopeless under these conditions.", s);
			return;
		}
	}
	
	if(Underwater) {
		pline("Sorry, fire and water don't mix.");
		return;
	}

	otmp = carrying(CANDELABRUM_OF_INVOCATION);
	if(!otmp || obj->otyp==GNOMISH_POINTY_HAT || obj->otyp==CANDLE_OF_INVOCATION || otmp->spe == 7) {
		use_lamp(obj);
		return;
	}

	Sprintf(qbuf, "Attach %s", the(xname(obj)));
	Sprintf(eos(qbuf), " to %s?",
		safe_qbuf(qbuf, sizeof(" to ?"), the(xname(otmp)),
			the(simple_typename(otmp->otyp)), "it"));
	if(yn(qbuf) == 'n') {
		if (!obj->lamplit)
		    You("try to light %s...", the(xname(obj)));
		use_lamp(obj);
		return;
	} else {
		if ((long)otmp->spe + obj->quan > 7L)
		    obj = splitobj(obj, 7L - (long)otmp->spe);
		else *optr = 0;

		/* The candle's age field doesn't correctly reflect the amount
		   of fuel in it while it's lit, because the fuel is measured
		   by the timer. So to get accurate age updating, we need to
		   end the burn temporarily while attaching the candle. */
		was_lamplit = obj->lamplit;
		if (was_lamplit)
		    end_burn(obj, TRUE);

		You("attach %ld%s %s to %s.",
		    obj->quan, !otmp->spe ? "" : " more",
		    s, the(xname(otmp)));
		if (!otmp->spe || otmp->age > obj->age)
		    otmp->age = obj->age;
		otmp->spe += (int)obj->quan;
		if (otmp->lamplit && !was_lamplit)
		    pline_The("new %s magically %s!", s, vtense(s, "ignite"));
		else if (!otmp->lamplit && was_lamplit)
		    pline("%s out.", (obj->quan > 1L) ? "They go" : "It goes");
		if (obj->unpaid)
		    verbalize("You %s %s, you bought %s!",
			      otmp->lamplit ? "burn" : "use",
			      (obj->quan > 1L) ? "them" : "it",
			      (obj->quan > 1L) ? "them" : "it");
		if (obj->quan < 7L && otmp->spe == 7)
		    pline("%s now has seven%s candles attached.",
			  The(xname(otmp)), otmp->lamplit ? " lit" : "");
		/* candelabrum's light range might increase */
		if (otmp->lamplit) obj_merge_light_sources(otmp, otmp);
		/* candles are no longer a separate light source */
		/* candles are now gone */
		useupall(obj);
	}
}

boolean
snuff_candle(otmp)  /* call in drop, throw, and put in box, etc. */
register struct obj *otmp;
{
	register boolean candle = Is_candle(otmp);

	if ((candle || otmp->otyp == CANDELABRUM_OF_INVOCATION) &&
		otmp->lamplit) {
	    char buf[BUFSZ];
	    xchar x, y;
	    register boolean many = candle ? otmp->quan > 1L : otmp->spe > 1;

	    (void) get_obj_location(otmp, &x, &y, 0);
	    if (otmp->where == OBJ_MINVENT ? cansee(x,y) : !Blind)
		pline("%s %scandle%s flame%s extinguished.",
		      Shk_Your(buf, otmp),
		      (candle ? "" : "candelabrum's "),
		      (many ? "s'" : "'s"), (many ? "s are" : " is"));
	   end_burn(otmp, TRUE);
	   return(TRUE);
	}
	return(FALSE);
}

/* called when lit lamp is hit by water or put into a container or
   you've been swallowed by a monster; obj might be in transit while
   being thrown or dropped so don't assume that its location is valid */
boolean
snuff_lit(obj)
struct obj *obj;
{
	xchar x, y;

	if (obj->lamplit) {
	    if (obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP ||
		    obj->otyp == LANTERN || obj->otyp == LANTERN_PLATE_MAIL || 
			obj->otyp == POT_OIL ||
			obj->otyp == DWARVISH_HELM || obj->otyp == GNOMISH_POINTY_HAT) {
		(void) get_obj_location(obj, &x, &y, 0);
		if (obj->where == OBJ_MINVENT ? cansee(x,y) : !Blind)
		    pline("%s %s out!", Yname2(obj), otense(obj, "go"));
		end_burn(obj, TRUE);
		return TRUE;
	    }
	    if (snuff_candle(obj)) return TRUE;
	}
	return FALSE;
}

/* Called when potentially lightable object is affected by fire_damage().
	Return TRUE if object was lit and FALSE otherwise --ALI */
boolean
catch_lit(obj)
struct obj *obj;
{
	xchar x, y;

	if (!obj->lamplit && ignitable(obj)) {
	    if ((obj->otyp == MAGIC_LAMP ||
		 obj->otyp == CANDELABRUM_OF_INVOCATION) &&
		obj->spe == 0)
		return FALSE;
	    else if (obj->otyp != MAGIC_LAMP && obj->age == 0)
		return FALSE;
	    if (!get_obj_location(obj, &x, &y, 0))
		return FALSE;
	    if (obj->otyp == CANDELABRUM_OF_INVOCATION && obj->cursed)
		return FALSE;
	    if ((obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP ||
		 obj->otyp == LANTERN || obj->otyp == LANTERN_PLATE_MAIL ||
		 obj->otyp == DWARVISH_HELM || obj->otyp == GNOMISH_POINTY_HAT) && 
			obj->cursed && !rn2(2))
		return FALSE;
	    if (obj->where == OBJ_MINVENT ? cansee(x,y) : !Blind)
		pline("%s %s light!", Yname2(obj), otense(obj, "catch"));
	    if (obj->otyp == POT_OIL) makeknown(obj->otyp);
	    if (obj->unpaid && costly_spot(u.ux, u.uy) && (obj->where == OBJ_INVENT)) {
	        /* if it catches while you have it, then it's your tough luck */
		check_unpaid(obj);
	        verbalize("That's in addition to the cost of %s %s, of course.",
				Yname2(obj), obj->quan == 1 ? "itself" : "themselves");
		bill_dummy_object(obj);
	    }
	    begin_burn(obj);
	    return TRUE;
	}
	return FALSE;
}

STATIC_OVL int
swap_aegis(obj)
struct obj *obj;
{
	if(obj->owornmask){
		You("must take %s off to modify it.", the(xname(obj)));
		return MOVE_CANCELLED;
	} else if(obj->otyp == CLOAK){
		You("wrap %s up, making a serviceable shield.", the(xname(obj)));
		obj->otyp = ROUNDSHIELD;
		return MOVE_STANDARD;
	} else if(obj->otyp == ROUNDSHIELD){
		You("unwrap %s, making a cloak.", the(xname(obj)));
		obj->otyp = CLOAK;
		return MOVE_STANDARD;
	} else {
		pline("Aegis in unexpected state?");
		return MOVE_CANCELLED;
	}
}

STATIC_OVL int
ilmater_touch(obj)
struct obj *obj;
{
	struct monst *mon;
	// Allow the cords to be used from inventory, like a unicorn horn or a stethoscope.
	if(!getdir((char *)0)) {
		return MOVE_CANCELLED;
	}
	if(u.dz > 0){
		if(u.usteed)
			mon = u.usteed;
		else if(u.uswallow)
			mon = u.ustuck;
		else {
			You("doubt that will have any further effect.");
			return MOVE_CANCELLED;
		}
	}
	else if(u.dz < 0){
		if(u.uswallow)
			mon = u.ustuck;
		else {
			You("don't see anything up there to touch with your cords.");
			return MOVE_CANCELLED;
		}
	}
	else if(!u.dx && !u.dy){
		if(*hp(&youmonst) >= *hpmax(&youmonst))
			pline("Nothing happens.");
		else
			You("transfer your wounds to yourself.");
		return MOVE_STANDARD;
	}
	else if(!isok(u.ux + u.dx, u.uy + u.dy)){
		You("don't touch anything.");
		return MOVE_STANDARD;
	}
	else {
		mon = m_at(u.ux + u.dx, u.uy + u.dy);
	}
	if(!mon){
		You("don't touch anything.");
		return MOVE_STANDARD;
	}
	if(mon->mpeaceful){
		if(*hp(mon) >= *hpmax(mon))
			pline("Nothing happens.");
		else {
			You("transfer %s wounds to yourself.", s_suffix(mon_nam(mon)));
			int wounds = *hpmax(mon) - *hp(mon);
			wounds = min(wounds, *hp(&youmonst)/2);
			*hp(mon) += wounds;
			*hp(&youmonst) -= wounds;
			flags.botl = 1;
		}
		return MOVE_STANDARD;
	}
	else {
		if(*hp(&youmonst) >= *hpmax(&youmonst))
			pline("Nothing happens.");
		else {
			You("transfer your wounds to %s.", mon_nam(mon));
			int wounds = *hpmax(&youmonst) - *hp(&youmonst);
			wounds = min(wounds, *hp(mon)/2);
			*hp(&youmonst) += wounds;
			*hp(mon) -= wounds;
			flags.botl = 1;
		}
		return MOVE_STANDARD;
	}
	return MOVE_STANDARD;
}

STATIC_OVL int
aesculapius_poke(obj)
struct obj *obj;
{
	struct monst *mon;
	boolean shackles = obj->oartifact == ART_ESSCOOAHLIPBOOURRR;
	if(obj != uwep){
		if (!wield_tool(obj, "staff")) return MOVE_CANCELLED;
	}
	if(!getdir((char *)0)) {
		return MOVE_CANCELLED;
	}
	if(u.dz > 0){
		if(u.usteed)
			mon = u.usteed;
		else if(u.uswallow)
			mon = u.ustuck;
		else {
			You("doubt that will have any further effect.");
			return MOVE_CANCELLED;
		}
	}
	else if(u.dz < 0){
		if(u.uswallow)
			mon = u.ustuck;
		else {
			if(shackles)
				You("don't see anything up there to touch with your broken shackles.");
			else
				You("don't see anything up there to poke with your staff.");
			return MOVE_CANCELLED;
		}
	}
	else if(!u.dx && !u.dy){
		use_unicorn_horn(obj);
		return MOVE_STANDARD;
	}
	else if(!isok(u.ux + u.dx, u.uy + u.dy)){
		if(shackles)
			pline("Your broken shackles don't touch anything.");
		else
			pline("Your staff doesn't touch anything.");
		return MOVE_STANDARD;
	}
	else {
		mon = m_at(u.ux + u.dx, u.uy + u.dy);
	}
	if(!mon){
		if(shackles)
			pline("Your broken shackles don't touch anything!");
		else
			pline("Your staff doesn't touch anything.");
		return MOVE_STANDARD;
	}
	boolean good_effect = (mon->mpeaceful && !obj->cursed) || (!mon->mpeaceful && obj->cursed);
	if(good_effect){
		if (!mon->mcansee) {
		    mon->mcansee = 1;
		    mon->mblinded = 0;
		    if (canseemon(mon)) pline("%s can see again.", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else if (!mon->mcanhear) {
		    mon->mcanhear = 1;
		    mon->mdeafened = 0;
		    if (canseemon(mon)) pline("%s can hear again.", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else if (mon->mconf || mon->mstun) {
		    mon->mconf = mon->mstun = 0;
		    if (canseemon(mon))
				pline("%s seems steadier now.", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else if (mon->msleeping) {
		    mon->msleeping = 0;
		    if (canseemon(mon)) pline("%s wakes up!", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else if (!mon->mcanmove) {
		    mon->mcanmove = 1;
		    mon->mfrozen = 0;
		    if (canseemon(mon)) pline("%s can move again!", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else if (mon->mcan) {
			set_mcan(mon, FALSE);
		    if (canseemon(mon)) pline("%s looks special again!", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
				pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		}
		else if(has_template(mon, PLAGUE_TEMPLATE)){
			set_template(mon, 0);
			mon->mhpmax = max(3, (mon->m_lev * hd_size(mon->data))-1);
		    if (canseemon(mon)) pline("%s has been cured!", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
			if(rnd(!always_hostile(mon->data) ? 12 : 20) < ACURR(A_CHA)){
				struct monst *newmon = tamedog_core(mon, (struct obj *)0, TRUE);
				if(newmon){
					mon = newmon;
					newsym(mon->mx, mon->my);
					pline("%s is very grateful!", Monnam(mon));
				}
			}
		}
		else {
		    if (canseemon(mon)) pline("%s looks really healthy!", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		}
	}
	//bad effect
	else {
		if (mon->mcanhear) {
		    mon->mcanhear = 0;
		    mon->mdeafened = d(6,6);
		    if (canseemon(mon)) pline("%s is stricken deaf!", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else if (mon->mcansee) {
		    mon->mcansee = 0;
		    mon->mblinded = d(6,6);
		    if (canseemon(mon)) pline("%s is stricken blind!", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else if (!mon->mstun) {
		    mon->mstun = 1;
		    if (canseemon(mon))
				pline("%s wobbles!", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else if (!mon->mconf) {
		    mon->mconf = 1;
		    if (canseemon(mon))
				pline("%s seems confused!", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else if (mon->mcanmove && !mon_resistance(mon, FREE_ACTION)) {
		    mon->mcanmove = 0;
			mon->mfrozen = d(2,2);
		    if (canseemon(mon))
				pline("%s seems frozen!", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else if(!mon->mcan){
			set_mcan(mon, TRUE);
		    if (canseemon(mon)) pline("%s looks mediocre!", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else if (mon->mcanmove && !mon_resistance(mon, SICK_RES)) {
			int dmg = d(3, 12);
			if(!rn2(10))
				dmg += 100;
			if(m_losehp(mon, dmg, TRUE, "illness"));
		    else if (canseemon(mon))
				pline("%s looks slightly ill.", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		} else {
		    if (canseemon(mon)) pline("%s looks stubbornly healthy.", Monnam(mon));
			else {
				if(shackles)
					pline("Your broken shackles touch it!");
				else
					pline("Your staff touches it!");
				map_invisible(u.ux+u.dx,u.uy+u.dy);
			}
		}
	}
	return MOVE_STANDARD;
}

int
do_bloodletter(obj)
struct obj *obj;
{
	if(obj->oartifact != ART_BLOODLETTER || obj != uwep){
		pline("You must be wielding Bloodletter to do that.");
		return MOVE_CANCELLED;
	}
	if (artinstance[obj->oartifact].BLactive < monstermoves){
		pline("You must make an offering first.");
		return MOVE_CANCELLED; // unreachable as of now
	}
	
	You("slam the bloodied morning star down, releasing it of the tainted blood in a burst.");
	explode(u.ux, u.uy, AD_BLUD, 0, d(6, 6), EXPL_RED, 1);
	
	artinstance[obj->oartifact].BLactive = 0;
	
	return MOVE_STANDARD;
}


STATIC_OVL int
use_rakuyo(obj)
struct obj *obj;
{
	struct obj *dagger;
	if(obj != uwep){
		if(obj->otyp == RAKUYO) You("must wield %s to unlatch it.", the(xname(obj)));
		else You("must wield %s to latch it.", the(xname(obj)));
		return MOVE_CANCELLED;
	}
	
	if(obj->unpaid 
	|| (obj->otyp == RAKUYO_SABER && uswapwep && uswapwep->otyp == RAKUYO_DAGGER && uswapwep->unpaid)
	){
		You("need to buy it.");
		return MOVE_CANCELLED;
	}
	
	if(obj->otyp == RAKUYO){
		You("unlatch %s.",the(xname(obj)));
		obj->otyp = RAKUYO_SABER;
		obj->quan += 1;
		dagger = splitobj(obj, 1L);

		obj_extract_self(dagger);
		dagger->otyp = RAKUYO_DAGGER;
		fix_object(obj);
		fix_object(dagger);
		
		if (obj->oartifact && obj->oartifact == ART_BLADE_SINGER_S_SABER){
			artifact_exists(dagger, artiname(ART_BLADE_DANCER_S_DAGGER), FALSE);
			dagger = oname(dagger, artiname(ART_BLADE_DANCER_S_DAGGER));
		}

		dagger = hold_another_object(dagger, "You drop %s!",
				      doname(dagger), (const char *)0); /*shouldn't merge, but may drop*/
		if(dagger && !uswapwep && carried(dagger)){
			setuswapwep(dagger);
			if(!u.twoweap) dotwoweapon();
		}
	} else {
		if(!uswapwep || uswapwep->otyp != RAKUYO_DAGGER){
			You("need the matching dagger in your swap-weapon sheath or offhand.");
			return MOVE_CANCELLED;
		}
		if(!mergable_traits(obj, uswapwep) &&
			!((obj->oartifact && obj->oartifact == ART_BLADE_SINGER_S_SABER) &&
			(uswapwep->oartifact && uswapwep->oartifact == ART_BLADE_DANCER_S_DAGGER))
		){
			pline("They don't fit together!");
			return MOVE_CANCELLED;
		}
		if (u.twoweap) {
			u.twoweap = 0;
			update_inventory();
		}
		useupall(uswapwep);
		obj->otyp = RAKUYO;
		fix_object(obj);
		You("latch %s.",the(xname(obj)));
	}
	return MOVE_INSTANT;
}

STATIC_OVL int
use_mercy_blade(obj)
struct obj *obj;
{
	struct obj *dagger;
	if(obj != uwep){
		if(obj->otyp == BLADE_OF_MERCY) You("must wield %s to unlatch it.", the(xname(obj)));
		else You("must wield %s to latch it.", the(xname(obj)));
		return MOVE_CANCELLED;
	}
	
	if(obj->unpaid 
	|| (obj->otyp == BLADE_OF_MERCY && uswapwep && uswapwep->otyp == BLADE_OF_PITY && uswapwep->unpaid)
	){
		You("need to buy it.");
		return MOVE_CANCELLED;
	}
	
	if(obj->otyp == BLADE_OF_MERCY){
		You("unlatch %s.",the(xname(obj)));
		obj->otyp = BLADE_OF_GRACE;
		obj->quan += 1;
		dagger = splitobj(obj, 1L);

		obj_extract_self(dagger);
		dagger->otyp = BLADE_OF_PITY;
		fix_object(obj);
		fix_object(dagger);
		
		// if (obj->oartifact && obj->oartifact == ART_BLADE_SINGER_S_SABER){
			// artifact_exists(dagger, artiname(ART_BLADE_DANCER_S_DAGGER), FALSE);
			// dagger = oname(dagger, artiname(ART_BLADE_DANCER_S_DAGGER));
		// }

		dagger = hold_another_object(dagger, "You drop %s!",
				      doname(dagger), (const char *)0); /*shouldn't merge, but may drop*/
		if(dagger && !uswapwep && carried(dagger)){
			setuswapwep(dagger);
			if(!u.twoweap) dotwoweapon();
		}
	} else {
		if(!uswapwep || uswapwep->otyp != BLADE_OF_PITY){
			You("need the matching dagger in your swap-weapon sheath or offhand.");
			return MOVE_CANCELLED;
		}
		if(!mergable_traits(obj, uswapwep) &&
			!((obj->oartifact && obj->oartifact == ART_BLADE_SINGER_S_SABER) &&
			(uswapwep->oartifact && uswapwep->oartifact == ART_BLADE_DANCER_S_DAGGER))
		){
			pline("They don't fit together!");
			return MOVE_CANCELLED;
		}
		if (u.twoweap) {
			u.twoweap = 0;
			update_inventory();
		}
		useupall(uswapwep);
		obj->otyp = BLADE_OF_MERCY;
		fix_object(obj);
		You("latch %s.",the(xname(obj)));
	}
	return MOVE_INSTANT;
}

STATIC_OVL int
use_force_blade(obj)
struct obj *obj;
{
	struct obj *dagger;
	if(obj != uwep){
		if(obj->otyp == DOUBLE_FORCE_BLADE) You("must wield %s to unlatch it.", the(xname(obj)));
		else You("must wield %s to latch it.", the(xname(obj)));
		return MOVE_CANCELLED;
	}
	
	if(obj->unpaid 
	|| (obj->otyp == FORCE_BLADE && uswapwep && uswapwep->otyp == FORCE_BLADE && uswapwep->unpaid)
	){
		You("need to buy it.");
		return MOVE_CANCELLED;
	}
	
	if(obj->otyp == DOUBLE_FORCE_BLADE){
		You("unlatch %s.",the(xname(obj)));
		obj->otyp = FORCE_BLADE;
		fix_object(obj);
		obj->quan += 1;
	    dagger = splitobj(obj, 1L);
		obj_extract_self(dagger);
		fix_object(obj);
		dagger = hold_another_object(dagger, "You drop %s!",
				      doname(dagger), (const char *)0); /*shouldn't merge, but may drop*/
		if(dagger && !uswapwep && carried(dagger)){
			setuswapwep(dagger);
			if(!u.twoweap) dotwoweapon();
		}
	} else {
		if(!uswapwep || uswapwep->otyp != FORCE_BLADE){
			You("need the matching blade in your swap-weapon sheath or offhand.");
			return MOVE_CANCELLED;
		}
		if(!mergable_traits(obj, uswapwep)){
			pline("They don't fit together!");
			return MOVE_CANCELLED;
		}
		if (u.twoweap) {
			u.twoweap = 0;
			update_inventory();
		}
		obj->ovar1_charges = (obj->ovar1_charges + uswapwep->ovar1_charges)/2;
		useupall(uswapwep);
		obj->otyp = DOUBLE_FORCE_BLADE;
		fix_object(obj);
		You("latch %s.",the(xname(obj)));
		update_inventory();
	}
	return MOVE_INSTANT;
}

int
use_force_sword(obj)
struct obj *obj;
{
	if(obj->unpaid){
		You("need to buy it.");
		return MOVE_CANCELLED;
	}
	
	if(obj->otyp == FORCE_SWORD){
		You("unlock %s.",the(xname(obj)));
		obj->otyp = FORCE_WHIP;
	} else {
		You("lock %s.",the(xname(obj)));
		obj->otyp = FORCE_SWORD;
	}
	fix_object(obj);
	update_inventory();
	return MOVE_INSTANT;
}

STATIC_OVL void
use_lamp(obj)
struct obj *obj;
{
	char buf[BUFSZ];

	if(obj->oartifact == ART_INFINITY_S_MIRRORED_ARC){
		You("can't find an %s switch", litsaber(obj) ? "off" : "on");
		return;
	}
	if(Underwater && obj->oartifact != ART_HOLY_MOONLIGHT_SWORD) {
		pline("This is not a diving lamp.");
		return;
	}
	if(obj->lamplit) {
		if(obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP ||
		   obj->otyp == LANTERN || obj->otyp == LANTERN_PLATE_MAIL ||
		   obj->otyp == DWARVISH_HELM)
		    pline("%s lamp is now off.", Shk_Your(buf, obj));
		else if(is_lightsaber(obj)) {
		    if (obj->otyp == DOUBLE_LIGHTSABER || obj->oartifact == ART_ANNULUS) {
			/* Do we want to activate dual bladed mode? */
			if (!obj->altmode && (!obj->cursed || rn2(4))) {
			    You("ignite the second blade of %s.", yname(obj));
			    obj->altmode = TRUE;
			    return;
			} else obj->altmode = FALSE;
		    }
		    lightsaber_deactivate(obj, TRUE);
		    return;
		} else
		    You("snuff out %s.", yname(obj));
		end_burn(obj, TRUE);
		return;
	}
	/* magic lamps with an spe == 0 (wished for) cannot be lit */
	if ((!Is_candle(obj) && obj->age == 0 && obj->oartifact != ART_HOLY_MOONLIGHT_SWORD &&
			!(is_lightsaber(obj) && obj->oartifact == ART_ATMA_WEAPON && !Drain_resistance))
			|| (obj->otyp == MAGIC_LAMP && obj->spe == 0)
		) {
		if (obj->otyp == LANTERN || 
			obj->otyp == LANTERN_PLATE_MAIL || 
			obj->otyp == DWARVISH_HELM || 
			is_lightsaber(obj)
		)
			Your("%s has run out of power.", xname(obj));
		else pline("This %s has no %s.", xname(obj), obj->otyp != GNOMISH_POINTY_HAT ? "oil" : "wax");
		return;
	}
	if (is_gemable_lightsaber(obj) && !obj->cobj) {
		pline1(nothing_happens);
		return;
	}
	if(is_lightsaber(obj) && 
		obj->cursed && 
		obj->oartifact == ART_ATMA_WEAPON
	){
		if (!Drain_resistance) {
			pline("%s for a moment, then %s brightly.",
		      Tobjnam(obj, "flicker"), otense(obj, "burn"));
			losexp("life force drain",TRUE,TRUE,TRUE);
			obj->cursed = FALSE;
		}
	}
	if (obj->cursed && obj->otyp != ROD_OF_FORCE && (!rn2(2) || obj->otyp == CANDLE_OF_INVOCATION) && obj->oartifact != ART_HOLY_MOONLIGHT_SWORD) {
		pline("%s for a moment, then %s.",
		      Tobjnam(obj, "flicker"), otense(obj, "die"));
	} else {
		if(obj->otyp == OIL_LAMP || obj->otyp == MAGIC_LAMP ||
				obj->otyp == LANTERN || obj->otyp == LANTERN_PLATE_MAIL ||
				obj->otyp == DWARVISH_HELM) {
		    check_unpaid(obj);
		    pline("%s lamp is now on.", Shk_Your(buf, obj));
		} else if (is_lightsaber(obj)) {
			obj->lamplit = 1; //Make yname print out the color of the lightsaber
		    You("ignite %s.", yname(obj));
			obj->lamplit = 0;
		    unweapon = FALSE;
		} else if (obj->oartifact == ART_HOLY_MOONLIGHT_SWORD) {
			int biman;
			obj->lamplit = 1; //Check if the HMS will be two handed
			biman = bimanual(obj,youracedata);
			obj->lamplit = 0;
			if(biman && uarms){
				You_cant("invoke %s while wearing a shield!", yname(obj));
				return;
			}
		    You("invoke %s.", yname(obj));
			if(biman && u.twoweap){
				You("must now hold %s with both hands!", yname(obj)); 
				untwoweapon();
			}
		    unweapon = FALSE;
		} else {	/* candle(s) */
		    pline("%s flame%s %s%s",
			s_suffix(Yname2(obj)),
			plur(obj->quan), otense(obj, "burn"),
			Blind ? "." : " brightly!");
		    if (obj->unpaid && costly_spot(u.ux, u.uy) &&
			  obj->age == 20L * (long)objects[obj->otyp].oc_cost) {
			const char *ithem = obj->quan > 1L ? "them" : "it";
			verbalize("You burn %s, you bought %s!", ithem, ithem);
			bill_dummy_object(obj);
		    }
		}
		begin_burn(obj);
	}
}

STATIC_OVL void
light_cocktail(obj)
	struct obj *obj;	/* obj is a potion of oil */
{
	char buf[BUFSZ];
	const char *objnam =
//#ifdef FIREARMS
	    obj->otyp == POT_OIL ? "potion" : "stick";
//#else
//	    "potion";
//#endif

	if (u.uswallow){
		if(!is_whirly(u.ustuck->data)) {
			You(no_elbow_room);
			return;
		} else if(!obj->lamplit){
			You("can't get the %s to light.  It's quite hopeless under these conditions.", objnam);
			return;
		}
	}
	
	if(Underwater) {
		You("can't light this underwater!");
		return;
	}

	if (obj->lamplit) {
	    You("snuff the lit %s.", objnam);
	    end_burn(obj, TRUE);
	    /*
	     * Free & add to re-merge potion.  This will average the
	     * age of the potions.  Not exactly the best solution,
	     * but its easy.
	     */
	    freeinv(obj);
	    (void) addinv(obj);
	    return;
	} else if (Underwater) {
	    There("is not enough oxygen to sustain a fire.");
	    return;
	}

	You("light %s %s.%s", shk_your(buf, obj), objnam,
	    Blind ? "" : "  It gives off a dim light.");
	if (obj->unpaid && costly_spot(u.ux, u.uy)) {
	    /* Normally, we shouldn't both partially and fully charge
	     * for an item, but (Yendorian Fuel) Taxes are inevitable...
	     */
//#ifdef FIREARMS
	    if (obj->otyp != STICK_OF_DYNAMITE) {
//#endif
	    check_unpaid(obj);
	    verbalize("That's in addition to the cost of the potion, of course.");
//#ifdef FIREARMS
	    } else {
		const char *ithem = obj->quan > 1L ? "them" : "it";
		verbalize("You burn %s, you bought %s!", ithem, ithem);
	    }
//#endif
	    bill_dummy_object(obj);
	}
	makeknown(obj->otyp);
//#ifdef FIREARMS
	if (obj->otyp == STICK_OF_DYNAMITE) obj->yours=TRUE;
//#endif

	if (obj->quan > 1L) {
	    obj = splitobj(obj, 1L);
	    begin_burn(obj);	/* burn before free to get position */
	    obj_extract_self(obj);	/* free from inv */

	    /* shouldn't merge */
	    obj = hold_another_object(obj, "You drop %s!",
				      doname(obj), (const char *)0);
	} else
	    begin_burn(obj);
}

STATIC_OVL void
light_torch(obj)
	struct obj *obj;	/* obj is a shadowlander's torch */
{
	char buf[BUFSZ];
	const char *objnam = "torch";

	if (u.uswallow){
		if(!is_whirly(u.ustuck->data)) {
			You(no_elbow_room);
			return;
		} else if(!obj->lamplit && obj->otyp != SUNROD && rn2(4)){
			You("can't quite get the %s to light!", objnam);
			return;
		}
	}
	
	if(obj->lamplit && obj->otyp == SUNROD){
		You("can't snuff the lit %s!", objnam);
		return;
	}
	
	if(Underwater) {
		You("can't light this underwater!");
		return;
	}

	if (obj->lamplit) {
	    You("snuff the lit %s.", objnam);
	    end_burn(obj, TRUE);
	    /*
	     * Free & add to re-merge potion.  This will average the
	     * age of the potions.  Not exactly the best solution,
	     * but its easy.
	     */
	    freeinv(obj);
	    (void) addinv(obj);
	    return;
	} else if (Underwater) {
	    There("is not enough oxygen to sustain a fire.");
	    return;
	}
	if(obj->otyp == SHADOWLANDER_S_TORCH){
		You("light %s %s.%s", shk_your(buf, obj), objnam,
			Blind ? "" : "  It gives off dark shadows.");
	} else if(obj->otyp == SUNROD){
		You("light %s %s.%s", shk_your(buf, obj), objnam,
			Blind ? "" : "  It gives off brilliant light.");
	} else {
		You("light %s %s.%s", shk_your(buf, obj), objnam,
			Blind ? "" : "  It gives off bright flickering light.");
	}
	if (obj->unpaid && costly_spot(u.ux, u.uy)) {
		verbalize("You burn it, you bought it!");
	    bill_dummy_object(obj);
	}

	if (obj->quan > 1L) {
	    obj = splitobj(obj, 1L);
	    begin_burn(obj);	/* burn before free to get position */
	    obj_extract_self(obj);	/* free from inv */

	    /* shouldn't merge */
	    obj = hold_another_object(obj, "You drop %s!",
				      doname(obj), (const char *)0);
	} else
	    begin_burn(obj);
}

static NEARDATA const char cuddly[] = { TOOL_CLASS, GEM_CLASS, 0 };

int
dorub()
{
	struct obj *obj = getobj(cuddly, "rub");

	if (obj && obj->oclass == GEM_CLASS) {
	    if (is_graystone(obj)) {
		use_stone(obj);
		return MOVE_STANDARD;
	    } else {
		pline("Sorry, I don't know how to use that.");
		return MOVE_CANCELLED;
	    }
	}

	if (!obj) return MOVE_CANCELLED;

	if (obj->otyp == MAGIC_LAMP) {
	    if (obj->spe > 0 && !rn2(3)) {
		check_unpaid_usage(obj, TRUE);		/* unusual item use */
		djinni_from_bottle(obj);
		makeknown(MAGIC_LAMP);
		obj->otyp = OIL_LAMP;
		obj->spe = 0; /* for safety */
		obj->age = rn1(500,1000);
		if (obj->lamplit) begin_burn(obj);
		update_inventory();
	    } else if (rn2(2) && !Blind)
		You("see a puff of smoke.");
	    else pline1(nothing_happens);
	} else if (obj->otyp == LANTERN || obj->otyp == LANTERN_PLATE_MAIL || obj->otyp == DWARVISH_HELM) {
	    /* message from Adventure */
	    pline("Rubbing the electric lamp is not particularly rewarding.");
	    pline("Anyway, nothing exciting happens.");
	} else pline1(nothing_happens);
	return MOVE_STANDARD;
}

int
dojump()
{
	/* Physical jump */
	if(!Upolyd && Role_if(PM_MONK) && uwep && (uwep->otyp == QUARTERSTAFF || uwep->otyp == KHAKKHARA) && P_SKILL(P_QUARTERSTAFF) && P_SKILL(P_MARTIAL_ARTS)){
		int dist = min(P_SKILL(P_QUARTERSTAFF), P_SKILL(P_MARTIAL_ARTS));
		if(dist >= P_EXPERT)
			dist = P_SKILL(P_MARTIAL_ARTS);
		return jump(dist);
	}
	return jump(0);
}

int
jump(magic)
int magic; /* 0=Physical, otherwise skill level */
{
	coord cc;

	if (!magic && (nolimbs(youracedata) || slithy(youracedata))) {
		/* normally (nolimbs || slithy) implies !Jumping,
		   but that isn't necessarily the case for knights */
		You_cant("jump; you have no legs!");
		return MOVE_CANCELLED;
	} else if (!magic && !Jumping) {
		You_cant("jump very far.");
		return MOVE_CANCELLED;
	} else if (u.uswallow) {
		if (magic) {
			You("bounce around a little.");
			return MOVE_STANDARD;
		} else {
			pline("You've got to be kidding!");
		return MOVE_CANCELLED;
		}
		return MOVE_CANCELLED;
	} else if (u.uinwater) {
		if (magic) {
			You("swish around a little.");
			return MOVE_STANDARD;
		} else {
			pline("This calls for swimming, not jumping!");
		return MOVE_CANCELLED;
		}
		return MOVE_CANCELLED;
	} else if (u.ustuck) {
		if (u.ustuck->mtame && !Conflict && !u.ustuck->mberserk && !u.ustuck->mconf) {
		    You("pull free from %s.", mon_nam(u.ustuck));
		    u.ustuck = 0;
		    return MOVE_STANDARD;
		}
		if (magic) {
			You("writhe a little in the grasp of %s!", mon_nam(u.ustuck));
			return MOVE_STANDARD;
		} else {
			You("cannot escape from %s!", mon_nam(u.ustuck));
			return MOVE_CANCELLED;
		}

		return MOVE_CANCELLED;
	} else if (Levitation || Weightless || Is_waterlevel(&u.uz)) {
		if (magic) {
			You("flail around a little.");
			return MOVE_STANDARD;
		} else {
			You("don't have enough traction to jump.");
			return MOVE_CANCELLED;
		}
	} else if (!magic && near_capacity() > UNENCUMBERED) {
		You("are carrying too much to jump!");
		return MOVE_CANCELLED;
	} else if (!magic && (YouHunger <= 100 || ACURR(A_STR) < 6)) {
		You("lack the strength to jump!");
		return MOVE_CANCELLED;
	} else if (Wounded_legs) {
		long wl = (Wounded_legs & BOTH_SIDES);
		const char *bp = body_part(LEG);

		if (wl == BOTH_SIDES) bp = makeplural(bp);
#ifdef STEED
		if (u.usteed)
		    pline("%s is in no shape for jumping.", Monnam(u.usteed));
		else
#endif
		Your("%s%s %s in no shape for jumping.",
		     (wl == LEFT_SIDE) ? "left " :
			(wl == RIGHT_SIDE) ? "right " : "",
		     bp, (wl == BOTH_SIDES) ? "are" : "is");
		return MOVE_CANCELLED;
	}
#ifdef STEED
	else if (u.usteed && u.utrap) {
		pline("%s is stuck in a trap.", Monnam(u.usteed));
		return MOVE_CANCELLED;
	}
#endif

	pline("Where do you want to jump?");
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, TRUE, "the desired position") < 0)
		return MOVE_CANCELLED;	/* user pressed ESC */
	if (!magic && !(HJumping & ~INTRINSIC) && !EJumping && !(u.sealsActive&SEAL_OSE) &&
			distu(cc.x, cc.y) != 5) {
		/* The Knight jumping restriction still applies when riding a
		 * horse.  After all, what shape is the knight piece in chess?
		 */
		pline("Illegal move!");
		return MOVE_CANCELLED;
	} else if (distu(cc.x, cc.y) > (magic ? 6+magic*3 : 9)) {
		pline("Too far!");
		return MOVE_CANCELLED;
	} else if (!cansee(cc.x, cc.y)) {
		You("cannot see where to land!");
		return MOVE_CANCELLED;
	} else if (!isok(cc.x, cc.y)) {
		You("cannot jump there!");
		return MOVE_CANCELLED;
	} else {
	    coord uc;
	    int range, temp;

	    if(u.utrap)
		switch(u.utraptype) {
		case TT_BEARTRAP: {
		    long side = rn2(3) ? LEFT_SIDE : RIGHT_SIDE;
		    You("rip yourself free of the bear trap!  Ouch!");
			if (uarmf && uarmf->otyp == find_jboots()){
				int bootdamage = d(1,10);
				losehp(rnd(10), "jumping out of a bear trap", KILLED_BY);
				if(!Preservation){
					set_wounded_legs(side, rn1(100,50));
					if(bootdamage > uarmf->spe){
						claws_destroy_arm(uarmf);
					}else{
						for(; bootdamage >= 0; bootdamage--) drain_item(uarmf);
						Your("boots are damaged!");
					}
				}
			}
		    else{
				losehp(d(5,6), "jumping out of a bear trap", KILLED_BY);
				set_wounded_legs(side, rn1(1000,500));
			}
		    break;
		  }
		case TT_FLESH_HOOK: {
		    You("rip yourself free of the flesh hook!  Ouch!");
			losehp(d(13,3), "tearing free from a flesh hook", KILLED_BY);
		    break;
		  }
		case TT_PIT:
		    You("leap from the pit!");
		    break;
		case TT_WEB:
			if(Is_lolth_level(&u.uz)){
				You("cannot free yourself from the web!");
				return MOVE_CANCELLED;
			} else {
				You("tear the web apart as you pull yourself free!");
				deltrap(t_at(u.ux,u.uy));
			}
		    break;
		case TT_LAVA:
		    You("pull yourself above the lava!");
		    u.utrap = 0;
		    return MOVE_STANDARD;
		case TT_INFLOOR:
		    You("strain your %s, but you're still stuck in the floor.",
			makeplural(body_part(LEG)));
		    set_wounded_legs(LEFT_SIDE, rn1(10, 11));
		    set_wounded_legs(RIGHT_SIDE, rn1(10, 11));
		    return MOVE_STANDARD;
		}

	    /*
	     * Check the path from uc to cc, calling hurtle_step at each
	     * location.  The final position actually reached will be
	     * in cc.
	     */
	    uc.x = u.ux;
	    uc.y = u.uy;
	    /* calculate max(abs(dx), abs(dy)) as the range */
	    range = cc.x - uc.x;
	    if (range < 0) range = -range;
	    temp = cc.y - uc.y;
	    if (temp < 0) temp = -temp;
	    if (range < temp)
		range = temp;
	    (void) walk_path(&uc, &cc, hurtle_step, (genericptr_t)&range);

	    /* A little Sokoban guilt... */
	    if (In_sokoban(&u.uz))
		change_luck(-1);

	    teleds(cc.x, cc.y, TRUE);
	    nomul(-1, "jumping around");
	    nomovemsg = "";
	    morehungry(max_ints(1, rnd(25) * get_uhungersizemod()));
	    return MOVE_STANDARD;
	}
}

boolean
tinnable(corpse)
struct obj *corpse;
{
	if (corpse->otyp != CORPSE) return FALSE;
	if (corpse->oeaten && !(has_blood(&mons[corpse->corpsenm]) && corpse->odrained && corpse->oeaten == drainlevel(corpse))) return FALSE;
	if (!mons[corpse->corpsenm].cnutrit) return FALSE;
	return TRUE;
}

STATIC_OVL void
use_treph_crystals(obj)
register struct obj *obj;
{
	struct obj *cobj = getobj(tools, "trephinate");
	
	if(!cobj)
		return;
	if(cobj->otyp != CRYSTAL_SKULL){
		You("examine %s in minute detail. Eventually, you conclude that it is not in fact a skull.", the(xname(cobj)));
		return;
	}
	if(use_container(cobj, TRUE))
		obj->spe--;
}

STATIC_OVL void
use_treph_thoughts(obj)
register struct obj *obj;
{
	int otyp;
	struct obj *glyph;
	
	if(Upolyd && u.uinsight < 20){
		You("can't get at your own brain right now!");
		return;
	}
	
	otyp = dotrephination_menu();
	if(!otyp)
		return;
	
	glyph = mksobj(otyp, MKOBJ_NOINIT);
	
	if(glyph){
		remove_thought(otyp_to_thought(otyp));
		//Note: affecting your true brain, so use race-if.
		if(Race_if(PM_ANDROID)){
			set_material_gm(glyph, PLASTIC);
			fix_object(glyph);
		}
		if(Race_if(PM_CLOCKWORK_AUTOMATON)){
			set_material_gm(glyph, COPPER);
			fix_object(glyph);
		}
		if(Race_if(PM_WORM_THAT_WALKS)){
			set_material_gm(glyph, SHELL_MAT);
			fix_object(glyph);
		}
		hold_another_object(glyph, "You drop %s!", doname(glyph), (const char *)0);
		if(ACURR(A_WIS)>ATTRMIN(A_WIS)){
			adjattrib(A_WIS, -1, FALSE);
		}
		if(ACURR(A_INT)>ATTRMIN(A_INT)){
			adjattrib(A_INT, -1, FALSE);
		}
		if(ACURR(A_CON)>ATTRMIN(A_CON)){
			adjattrib(A_CON, -1, FALSE);
		}
		change_usanity(-10, FALSE);
		//Note: this is always the player's HP, not their polyform HP.
		u.uhp -= u.uhp/2; //Note: chopped, so 0 to 1/2 max-HP lost.
		obj->spe--;
	} else {
		impossible("Shard creation failed in use_trephination_kit??");
	}
	return;
}

STATIC_OVL void
use_trephination_kit(obj)
register struct obj *obj;
{
	boolean skulls = !!carrying(CRYSTAL_SKULL);
	
	if(!obj->spe){
		pline("The kit's medical supplies are exhausted.");
		return;
	}
	if(u.uinsight < 10 || !(u.thoughts || skulls)){
		You("examine the drills in the kit, but have know idea how to use them!");
		return;
	}
	
	if(skulls && u.thoughts){
		int pick = dotrephination_options();
		if(pick == 0){
			You("close the trephination kit.");
			return;
		}
		if(pick == TREPH_THOUGHTS){
			use_treph_thoughts(obj);
			return;
		}
		if(pick == TREPH_CRYSTALS){
			use_treph_crystals(obj);
			return;
		}
	}
	else if(skulls){
		use_treph_crystals(obj);
	}
	else {
		use_treph_thoughts(obj);
	}
	return;
}

STATIC_OVL void
use_tinning_kit(obj)
register struct obj *obj;
{
	register struct obj *corpse, *can=0, *bld=0;

	/* This takes only 1 move.  If this is to be changed to take many
	 * moves, we've got to deal with decaying corpses...
	 */
	if (obj->spe <= 0) {
		You("seem to be out of tins.");
		return;
	}
	if (!(corpse = floorfood("tin", 2))) return;
	if (corpse->otyp == CORPSE && corpse->oeaten && !(has_blood(&mons[corpse->corpsenm]) && corpse->odrained && corpse->oeaten == drainlevel(corpse))) {
		You("cannot tin %s which is partly eaten.",something);
		return;
	}
	if (!tinnable(corpse)) {
		You_cant("tin that!");
		return;
	}
	if (touch_petrifies(&mons[corpse->corpsenm])
		&& !Stone_resistance && !uarmg) {
	    char kbuf[BUFSZ];

	    if (poly_when_stoned(youracedata))
		You("tin %s without wearing gloves.",
			an(mons[corpse->corpsenm].mname));
	    else {
		pline("Tinning %s without wearing gloves is a fatal mistake...",
			an(mons[corpse->corpsenm].mname));
		Sprintf(kbuf, "trying to tin %s without gloves",
			an(mons[corpse->corpsenm].mname));
	    }
	    instapetrify(kbuf);
	}
	if (is_rider(&mons[corpse->corpsenm])) {
		if(Is_astralevel(&u.uz)) verbalize("Yes...  But War does not preserve its enemies...");
		(void) revive_corpse(corpse, REVIVE_MONSTER);
		return;
	}
	if (mons[corpse->corpsenm].cnutrit == 0) {
		pline("That's too insubstantial to tin.");
		return;
	}
	consume_obj_charge(obj, TRUE);
	if((has_blood(&mons[corpse->corpsenm]) && !corpse->odrained)
		|| !(Race_if(PM_VAMPIRE) || Race_if(PM_INCANTIFIER) || 
			umechanoid)
		|| yn("This corpse does not have blood. Tin it?") == 'y'
	){
		if ((can = mksobj(TIN, MKOBJ_NOINIT)) != 0) {
			static const char you_buy_it[] = "You tin it, you bought it!";

			can->corpsenm = corpse->corpsenm;
			can->cursed = obj->cursed;
			can->blessed = obj->blessed;
			can->owt = weight(can);
			can->known = 1;
			can->spe = -1;  /* Mark tinned tins. No spinach allowed... */
			if(has_blood(&mons[corpse->corpsenm]) && !corpse->odrained){
				if ((bld = mksobj(POT_BLOOD, MKOBJ_NOINIT)) != 0) {
					bld->corpsenm = corpse->corpsenm;
					bld->cursed = obj->cursed;
					bld->blessed = obj->blessed;
					bld->known = 1;
				}
			}
			if (carried(corpse)) {
			if (corpse->unpaid)
				verbalize(you_buy_it);
			useup(corpse);
			} else {
			if (costly_spot(corpse->ox, corpse->oy) && !corpse->no_charge)
				verbalize(you_buy_it);
			useupf(corpse, 1L);
			}
			can = hold_another_object(can, "You make, but cannot pick up, %s.",
						  doname(can), (const char *)0);
			if(bld) bld = hold_another_object(bld, "You make, but cannot pick up, %s.",
						  doname(bld), (const char *)0);
		} else impossible("Tinning failed.");
	}
}

void
use_unicorn_horn(obj)
struct obj *obj;
{
#define PROP_COUNT 6		/* number of properties we're dealing with */
#define ATTR_COUNT (A_MAX*3)	/* number of attribute points we might fix */
	int idx, val, val_limit,
	    trouble_count, unfixable_trbl, did_prop, did_attr;
	int trouble_list[PROP_COUNT + ATTR_COUNT];

	if (obj && obj->cursed) {
	    long lcount = (long) rnd(100);

	    switch (rn2(5)) {
	    case 0: make_sick(Sick ? Sick/3L + 1L : (long)rn1(ACURR(A_CON),20),
			xname(obj), TRUE, SICK_NONVOMITABLE);
		    break;
	    case 1: make_blinded(Blinded + lcount, TRUE);
		    break;
	    case 2: if (!Confusion)
			You("suddenly feel %s.",
			    Hallucination ? "trippy" : "confused");
		    make_confused(HConfusion + lcount, TRUE);
		    break;
	    case 3: make_stunned(HStun + lcount, TRUE);
		    break;
	    // case 4: (void) adjattrib(rn2(A_MAX), -1, FALSE);
		    // break;
	    case 4: (void) make_hallucinated(HHallucination + lcount, TRUE, 0L);
		    break;
	    }
	    return;
	}

/*
 * Entries in the trouble list use a very simple encoding scheme.
 */
#define prop2trbl(X)	((X) + A_MAX)
#define attr2trbl(Y)	(Y)
#define prop_trouble(X) trouble_list[trouble_count++] = prop2trbl(X)
#define attr_trouble(Y) trouble_list[trouble_count++] = attr2trbl(Y)

	trouble_count = unfixable_trbl = did_prop = did_attr = 0;

	/* collect property troubles */
	if (Sick) prop_trouble(SICK);
	if (Blinded > (long)u.ucreamed) prop_trouble(BLINDED);
	if (HHallucination) prop_trouble(HALLUC);
	if (Vomiting) prop_trouble(VOMITING);
	if (HConfusion) prop_trouble(CONFUSION);
	if (HStun) prop_trouble(STUNNED);

	unfixable_trbl = unfixable_trouble_count(TRUE);

	// /* collect attribute troubles */
	// for (idx = 0; idx < A_MAX; idx++) {
	    // val_limit = AMAX(idx);
	    // /* don't recover strength lost from hunger */
	    // if (idx == A_STR && u.uhs >= WEAK) val_limit--;
	    // /* don't recover more than 3 points worth of any attribute */
	    // if (val_limit > ABASE(idx) + 3) val_limit = ABASE(idx) + 3;

	    // for (val = ABASE(idx); val < val_limit; val++)
		// attr_trouble(idx);
	    // /* keep track of unfixed trouble, for message adjustment below */
	    // unfixable_trbl += (AMAX(idx) - val_limit);
	// }

	if (trouble_count == 0) {
	    pline1(nothing_happens);
	    return;
	} else if (trouble_count > 1) {		/* shuffle */
	    int i, j, k;

	    for (i = trouble_count - 1; i > 0; i--)
		if ((j = rn2(i + 1)) != i) {
		    k = trouble_list[j];
		    trouble_list[j] = trouble_list[i];
		    trouble_list[i] = k;
		}
	}

	/*
	 *		Chances for number of troubles to be fixed
	 *		 0	1      2      3      4	    5	   6	  7
	 *   blessed:  22.7%  22.7%  19.5%  15.4%  10.7%   5.7%   2.6%	 0.8%
	 *  uncursed:  35.4%  35.4%  22.9%   6.3%    0	    0	   0	  0
	 */
	val_limit = rn2( d(2, (obj && obj->blessed) ? 4 : 2) );
	if (val_limit > trouble_count) val_limit = trouble_count;

	/* fix [some of] the troubles */
	for (val = 0; val < val_limit; val++) {
	    idx = trouble_list[val];

	    switch (idx) {
	    case prop2trbl(SICK):
		make_sick(0L, (char *) 0, TRUE, SICK_ALL);
		did_prop++;
		break;
	    case prop2trbl(BLINDED):
		make_blinded((long)u.ucreamed, TRUE);
		did_prop++;
		break;
	    case prop2trbl(HALLUC):
		(void) make_hallucinated(0L, TRUE, 0L);
		did_prop++;
		break;
	    case prop2trbl(VOMITING):
		make_vomiting(0L, TRUE);
		did_prop++;
		break;
	    case prop2trbl(CONFUSION):
		make_confused(0L, TRUE);
		did_prop++;
		break;
	    case prop2trbl(STUNNED):
		make_stunned(0L, TRUE);
		did_prop++;
		break;
	    default:
		if (idx >= 0 && idx < A_MAX) {
		    ABASE(idx) += 1;
		    did_attr++;
		} else
		    panic("use_unicorn_horn: bad trouble? (%d)", idx);
		break;
	    }
	}

	if (did_attr)
	    pline("This makes you feel %s!",
		  (did_prop + did_attr) == (trouble_count + unfixable_trbl) ?
		  "great" : "better");
	else if (!did_prop)
	    pline("Nothing seems to happen.");

	flags.botl = (did_attr || did_prop);
#undef PROP_COUNT
#undef ATTR_COUNT
#undef prop2trbl
#undef attr2trbl
#undef prop_trouble
#undef attr_trouble
}

/*
 * Timer callback routine: turn figurine into monster
 */
void
fig_transform(arg, timeout)
genericptr_t arg;
long timeout;
{
	struct obj *figurine = (struct obj *)arg;
	struct monst *mtmp;
	coord cc;
	boolean cansee_spot, silent, okay_spot;
	boolean redraw = FALSE;
	char monnambuf[BUFSZ], carriedby[BUFSZ];

	if (!figurine) {
#ifdef DEBUG
	    pline("null figurine in fig_transform()");
#endif
	    return;
	}
	silent = (timeout != monstermoves); /* happened while away */
	okay_spot = get_obj_location(figurine, &cc.x, &cc.y, 0);
	if (figurine->where == OBJ_INVENT ||
	    figurine->where == OBJ_MINVENT)
		okay_spot = enexto(&cc, cc.x, cc.y,
				   &mons[figurine->corpsenm]);
	if (!okay_spot ||
	    !figurine_location_checks(figurine,&cc, TRUE)) {
		/* reset the timer to try again later */
		(void) start_timer((long)rnd(5000), TIMER_OBJECT,
				FIG_TRANSFORM, (genericptr_t)figurine);
		return;
	}

	cansee_spot = cansee(cc.x, cc.y);
	mtmp = make_familiar(figurine, cc.x, cc.y, TRUE);
	if (mtmp) {
	    Sprintf(monnambuf, "%s",an(m_monnam(mtmp)));
	    switch (figurine->where) {
		case OBJ_INVENT:
		    if (Blind)
			You_feel("%s %s from your pack!", something,
			    locomotion(mtmp,"drop"));
		    else
			You("see %s %s out of your pack!",
			    monnambuf,
			    locomotion(mtmp,"drop"));
		    break;

		case OBJ_FLOOR:
		    if (cansee_spot && !silent) {
			You("suddenly see a figurine transform into %s!",
				monnambuf);
			redraw = TRUE;	/* update figurine's map location */
		    }
		    break;

		case OBJ_MINVENT:
		    if (cansee_spot && !silent) {
			struct monst *mon;
			mon = figurine->ocarry;
			/* figurine carring monster might be invisible */
			if (canseemon(figurine->ocarry)) {
			    Sprintf(carriedby, "%s pack",
				     s_suffix(a_monnam(mon)));
			}
			else if (is_pool(mon->mx, mon->my, FALSE))
			    Strcpy(carriedby, "empty water");
			else
			    Strcpy(carriedby, "thin air");
			You("see %s %s out of %s!", monnambuf,
			    locomotion(mtmp, "drop"), carriedby);
		    }
		    break;
#if 0
		case OBJ_MIGRATING:
		    break;
#endif

		default:
		    impossible("figurine came to life where? (%d)",
				(int)figurine->where);
		break;
	    }
	}
	/* free figurine now */
	obj_extract_self(figurine);
	obfree(figurine, (struct obj *)0);
	if (redraw) newsym(cc.x, cc.y);
}

STATIC_OVL boolean
figurine_location_checks(obj, cc, quietly)
struct obj *obj;
coord *cc;
boolean quietly;
{
	xchar x,y;

	if (carried(obj) && u.uswallow) {
		if (!quietly)
			You("don't have enough room in here.");
		return FALSE;
	}
	x = cc->x; y = cc->y;
	if (!isok(x,y)) {
		if (!quietly)
			You("cannot put the figurine there.");
		return FALSE;
	}
	if (IS_ROCK(levl[x][y].typ) &&
	    !(species_passes_walls(&mons[obj->corpsenm]) && may_passwall(x,y))) {
		if (!quietly)
		    You("cannot place a figurine in %s!",
			IS_TREES(levl[x][y].typ) ? "a tree" : "solid rock");
		return FALSE;
	}
	if (boulder_at(x,y) && !species_passes_walls(&mons[obj->corpsenm])
			&& !throws_rocks(&mons[obj->corpsenm])) {
		if (!quietly)
			You("cannot fit the figurine on the %s.",xname(boulder_at(x,y)));
		return FALSE;
	}
	return TRUE;
}

STATIC_OVL int
use_figurine(optr)
struct obj **optr;
{
	register struct obj *obj = *optr;
	xchar x, y;
	coord cc;

	if (u.uswallow) {
		/* can't activate a figurine while swallowed */
		if (!figurine_location_checks(obj, (coord *)0, FALSE))
			return MOVE_CANCELLED;
	}
	if(!getdir((char *)0)) {
		return MOVE_CANCELLED;
	}
	x = u.ux + u.dx; y = u.uy + u.dy;
	cc.x = x; cc.y = y;
	/* Passing FALSE arg here will result in messages displayed */
	if (!figurine_location_checks(obj, &cc, FALSE))
		return MOVE_CANCELLED;
	You("%s and it transforms.",
	    (u.dx||u.dy) ? "set the figurine beside you" :
	    (Weightless || Is_waterlevel(&u.uz) ||
	     is_pool(cc.x, cc.y, TRUE)) ?
		"release the figurine" :
	    (u.dz < 0 ?
		"toss the figurine into the air" :
		"set the figurine on the ground"));
	(void) make_familiar(obj, cc.x, cc.y, FALSE);
	(void) stop_timer(FIG_TRANSFORM, obj->timed);
	useup(obj);
	*optr = 0;

	return MOVE_STANDARD;
}

STATIC_OVL int
use_crystal_skull(optr)
struct obj **optr;
{
	coord cc;
	xchar x, y;

	if(u.veil || !(Unblind_telepat || (Blind_telepat && Blind))){
		pline("It's just a clear glass skull.");
		return MOVE_CANCELLED;
	}
	
	if(!get_ox(*optr, OX_EMON)){
		pline("It's REALLY just a glass skull.");
		return MOVE_CANCELLED;
	}

	if(!getdir((char *)0)) {
		flags.move = multi = 0;
		return MOVE_CANCELLED;
	}

	x = u.ux + u.dx; y = u.uy + u.dy;
	if(!enexto(&cc, x, y, (struct permonst *)0)){
		pline("No room!");
		return MOVE_CANCELLED;
	}
	
	//Note: summoning from summoned crystal skulls doesn't work well
	if(get_ox(*optr, OX_ESUM) || obj_summon_out(*optr)){
		pline("The imprisoned mind is dreaming.");
		return MOVE_STANDARD;
	}

	if((*optr)->age > monstermoves){
		pline("The imprisoned mind is exhausted.");
		change_usanity(save_vs_sanloss() ? 0 : -1, FALSE);
		return MOVE_STANDARD;
	}
	
	if(rnd(20) > u.uinsight || u.uen < EMON(*optr)->m_lev){
		You_cant("maintain your focus on the crystal!");
		if(save_vs_sanloss())
			change_usanity(-1, FALSE);
		else
			change_usanity(-1*rnd(10), TRUE);
		return MOVE_STANDARD;
	}
	
	You("awaken the imprisoned mind!");
	u.uen -= EMON(*optr)->m_lev;
	flags.botl = 1;
	if(save_vs_sanloss())
		change_usanity(-1*rnd(8), TRUE);
	else
		change_usanity(-1*d(2,6), TRUE);

	x_uses_crystal_skull(optr, &youmonst, &cc);
	return MOVE_STANDARD;
}

void
x_uses_crystal_skull(optr, master, cc)
struct obj **optr;
struct monst *master;
coord *cc;
{
	struct obj *obj = *optr;
	struct obj *oinv, *otmp;
	struct monst *mtmp;
	

	mtmp = montraits(obj, cc);
	if(mtmp){
		obj->age = monstermoves + 250L + rn2(250L);
		/* if skull has been named, give same name to the monster */
		if (get_ox(obj, OX_ENAM))
			mtmp = christen_monst(mtmp, ONAME(obj));
		mtmp->movement = NORMAL_SPEED;
		add_mx(mtmp, MX_ESUM);
		start_timer(ESUMMON_PERMANENT, TIMER_MONSTER, DESUMMON_MON, (genericptr_t)mtmp);
		for(oinv = obj->cobj; oinv; oinv = oinv->nobj){
			//Invalid items that are in the skull (possibly as a result of special cases) are skipped and handled later.
			if(oinv->otyp == TREPHINATION_KIT || ensouled_item(oinv))
				continue;

			otmp = duplicate_obj(oinv);
			obj_extract_self(otmp);
			if(otmp->oclass == SCROLL_CLASS){
				otmp = poly_obj(otmp, SCR_BLANK_PAPER);
			}
			if(otmp->oclass == SPBOOK_CLASS){
				otmp = poly_obj(otmp, SPE_BLANK_PAPER);
			}
			if(!is_ammo(otmp)){
				if(otmp->quan > 3)
					otmp->quan = rnd(3);
				fix_object(otmp);
			}
			if(otmp->otyp == MAGIC_MARKER){
				otmp->recharged = max(1, otmp->recharged);
				otmp->spe = 0;
			}
			//If the item was not merged, check if anything special should be done with it (like equipping a saddle)
			if(!mpickobj(mtmp,otmp)){
				if(otmp->otyp == SADDLE && !(mtmp->misc_worn_check&W_SADDLE) && can_saddle(mtmp)){
					mtmp->misc_worn_check |= W_SADDLE;
					otmp->owornmask = W_SADDLE;
					otmp->leashmon = mtmp->m_id;
					update_mon_intrinsics(mtmp, otmp, TRUE, FALSE);
				}
			}
		}
		m_level_up_intrinsic(mtmp);
		if(master == &youmonst || master->mtame){
			mtmp = tamedog_core(mtmp, (struct obj *)0, TRUE);
			if(mtmp && EDOG(mtmp)){
				EDOG(mtmp)->dominated = TRUE;
				EDOG(mtmp)->hungrytime = monstermoves + 4500;
			}
		}
		else {
			if(master->mfaction)
				set_faction(mtmp, master->mfaction);
			mtmp->mpeaceful = master->mpeaceful;
		}
		mark_mon_as_summoned(mtmp, master, ESUMMON_PERMANENT, 0);
		mtmp->mextra_p->esum_p->sm_o_id = obj->o_id;
		//After being marked as summoned, extract invalid items from skull and add to inventory.
		// These objects are "really there"/will remain after the monster is defeated.
		for(oinv = obj->cobj; oinv; oinv = oinv->nobj){
			if(oinv->otyp == TREPHINATION_KIT || ensouled_item(oinv)){
				obj_extract_self(oinv);
				mpickobj(mtmp,oinv);
			}
		}

		m_dowear(mtmp, TRUE);
		init_mon_wield_item(mtmp);
	}
}

static NEARDATA const char lubricables[] = { ALL_CLASSES, ALLOW_NONE, 0 };
static NEARDATA const char need_to_remove_outer_armor[] =
			"need to remove your %s to grease your %s.";

STATIC_OVL void
use_grease(obj)
struct obj *obj;
{
	struct obj *otmp;
	char buf[BUFSZ];

	if (Glib) {
	    pline("%s from your %s.", Tobjnam(obj, "slip"),
		  makeplural(body_part(FINGER)));
	    dropx(obj);
	    return;
	}

	if (obj->spe > 0) {
		if ((obj->cursed || Fumbling) && !rn2(2)) {
			consume_obj_charge(obj, TRUE);

			pline("%s from your %s.", Tobjnam(obj, "slip"),
			      makeplural(body_part(FINGER)));
			dropx(obj);
			return;
		}
		otmp = getobj(lubricables, "grease");
		if (!otmp) return;
		if ((otmp->owornmask & WORN_ARMOR) && uarmc) {
			Strcpy(buf, xname(uarmc));
			You(need_to_remove_outer_armor, buf, xname(otmp));
			return;
		}
		if ((otmp->owornmask & WORN_SHIRT) && (uarmc || (uarm && arm_blocks_upper_body(uarm->otyp)))) {
			Strcpy(buf, uarmc ? xname(uarmc) : "");
			if (uarmc && uarm) Strcat(buf, " and ");
			Strcat(buf, uarm ? xname(uarm) : "");
			You(need_to_remove_outer_armor, buf, xname(otmp));
			return;
		}
		consume_obj_charge(obj, TRUE);

		if (otmp != &zeroobj) {
			You("cover %s with a thick layer of grease.",
			    yname(otmp));
			otmp->greased = 1;
			if (obj->cursed && !nohands(youracedata)) {
			    incr_itimeout(&Glib, rnd(15));
			    pline("Some of the grease gets all over your %s.",
				makeplural(body_part(HAND)));
			}
		} else {
			Glib += rnd(15);
			You("coat your %s with grease.",
			    makeplural(body_part(FINGER)));
		}
	} else {
	    if (obj->known)
		pline("%s empty.", Tobjnam(obj, "are"));
	    else
		pline("%s to be empty.", Tobjnam(obj, "seem"));
	}
	update_inventory();
}

static struct trapinfo {
	struct obj *tobj;
	xchar tx, ty;
	int time_needed;
	boolean force_bungle;
} trapinfo;

void
reset_trapset()
{
	trapinfo.tobj = 0;
	trapinfo.force_bungle = 0;
}

/* touchstones - by Ken Arnold */
STATIC_OVL void
use_stone(tstone)
struct obj *tstone;
{
    struct obj *obj;
    boolean do_scratch;
    const char *streak_color, *choices;
    char stonebuf[QBUFSZ];
    static const char scritch[] = "\"scritch, scritch\"";
    static const char allowall[3] = { COIN_CLASS, ALL_CLASSES, 0 };
    static const char justgems[3] = { ALLOW_NONE, GEM_CLASS, 0 };
#ifndef GOLDOBJ
    struct obj goldobj;
#endif

    /* in case it was acquired while blinded */
    if (!Blind) tstone->dknown = 1;
    /* when the touchstone is fully known, don't bother listing extra
       junk as likely candidates for rubbing */
    choices = (tstone->otyp == TOUCHSTONE && tstone->dknown &&
		objects[TOUCHSTONE].oc_name_known) ? justgems : allowall;
    Sprintf(stonebuf, "rub on the stone%s", plur(tstone->quan));
    if ((obj = getobj(choices, stonebuf)) == 0)
	return;
#ifndef GOLDOBJ
    if (obj->oclass == COIN_CLASS) {
	u.ugold += obj->quan;	/* keep botl up to date */
	goldobj = *obj;
	dealloc_obj(obj);
	obj = &goldobj;
    }
#endif

    if (obj == tstone && obj->quan == 1) {
	You_cant("rub %s on itself.", the(xname(obj)));
	return;
    }

    if (tstone->otyp == TOUCHSTONE && tstone->cursed &&
	    obj->oclass == GEM_CLASS && !is_graystone(obj) &&
	    !obj_resists(obj, 0, 100)) {
	if (Blind)
	    pline("You feel something shatter.");
	else if (Hallucination)
	    pline("Oh, wow, look at the pretty shards.");
	else
	    pline("A sharp crack shatters %s%s.",
		  (obj->quan > 1) ? "one of " : "", the(xname(obj)));
#ifndef GOLDOBJ
     /* assert(obj != &goldobj); */
#endif
	useup(obj);
	return;
    }

    if (Blind) {
	pline("%s", scritch);
	return;
    } else if (Hallucination) {
	pline("Oh wow, man: Fractals!");
	return;
    }

    do_scratch = FALSE;
    streak_color = 0;

    switch (obj->oclass) {
    case GEM_CLASS:	/* these have class-specific handling below */
    case RING_CLASS:
	if (tstone->otyp != TOUCHSTONE) {
	    do_scratch = TRUE;
	} else if (obj->oclass == GEM_CLASS && (tstone->blessed ||
		(!tstone->cursed &&
		    (Role_if(PM_ARCHEOLOGIST) || Race_if(PM_GNOME))))) {
	    makeknown(TOUCHSTONE);
	    makeknown(obj->otyp);
	    prinv((char *)0, obj, 0L);
	    return;
	} else {
	    /* either a ring or the touchstone was not effective */
	    if (obj->obj_material == GLASS) {
		do_scratch = TRUE;
		break;
	    }
	}
	streak_color = c_obj_colors[obj->obj_color];
	break;		/* gem or ring */

    default:
	switch (obj->obj_material) {
	case CLOTH:
	    pline("%s a little more polished now.", Tobjnam(tstone, "look"));
	    return;
	case LIQUID:
	    if (!obj->known)		/* note: not "whetstone" */
		You("must think this is a wetstone, do you?");
	    else
		pline("%s a little wetter now.", Tobjnam(tstone, "are"));
	    return;
	case WAX:
	    streak_color = "waxy";
	    break;		/* okay even if not touchstone */
	case WOOD:
	    streak_color = "wooden";
	    break;		/* okay even if not touchstone */
	case GOLD:
	    do_scratch = TRUE;	/* scratching and streaks */
	    streak_color = "golden";
	    break;
	case SILVER:
	    do_scratch = TRUE;	/* scratching and streaks */
	    streak_color = "silvery";
	    break;
	case GEMSTONE:
		if (obj->sub_material) {
			/* similare check as above */
			if (tstone->otyp != TOUCHSTONE) {
				do_scratch = TRUE;
			}
			else if (tstone->blessed || (!tstone->cursed &&
				(Role_if(PM_ARCHEOLOGIST) || Race_if(PM_GNOME)))) {
				makeknown(TOUCHSTONE);
				makeknown(obj->sub_material);
				prinv((char *)0, obj, 0L);
				return;
			}
			/* the touchstone was not effective */
			streak_color = c_obj_colors[obj->obj_color];
		}
		else {
			do_scratch = (tstone->otyp != TOUCHSTONE);
		}
		break;
	default:
	    /* Objects passing the is_flimsy() test will not
	       scratch a stone.  They will leave streaks on
	       non-touchstones and touchstones alike. */
	    if (is_flimsy(obj))
		streak_color = c_obj_colors[obj->obj_color];
	    else
		do_scratch = (tstone->otyp != TOUCHSTONE);
	    break;
	}
	break;		/* default oclass */
    }

    Sprintf(stonebuf, "stone%s", plur(tstone->quan));
    if (do_scratch)
	pline("You make %s%sscratch marks on the %s.",
	      streak_color ? streak_color : (const char *)"",
	      streak_color ? " " : "", stonebuf);
    else if (streak_color)
	pline("You see %s streaks on the %s.", streak_color, stonebuf);
    else
	pline("%s", scritch);
    return;
}

STATIC_OVL int
use_sensor(sensor)
struct obj *sensor;
{
	int scantype = 0;
	if(sensor->spe <= 0){
		pline("It seems inert.");
		return MOVE_CANCELLED;
	} else {
		scantype = sensorMenu();
		if(!scantype) return MOVE_CANCELLED;
		
		switch(scantype){
			case POT_MONSTER_DETECTION:
				monster_detect(sensor, 0);
			break;
			case POT_OBJECT_DETECTION:
				object_detect(sensor, 0);
			break;
			case SPE_CLAIRVOYANCE:
				do_vicinity_map(u.ux,u.uy);
			break;
			case WAN_PROBING:{
				struct obj *pobj;
				if (!getdir((char *)0)) return MOVE_CANCELLED;
				if (!isok(u.ux+u.dx,u.uy+u.dy)) return MOVE_CANCELLED;
				if (u.dz < 0) {
					You("scan the %s thoroughly.  It seems it is %s.", ceiling(u.ux,u.uy), an(ceiling(u.ux,u.uy)));
				} else if(u.dz > 0) {
					pobj = level.objects[u.ux][u.uy];
					for(; pobj; pobj = pobj->nexthere){
						/* target object has now been "seen (up close)" */
						pobj->dknown = 1;
						if (Is_container(pobj) || pobj->otyp == STATUE) {
							if (!pobj->cobj)
							pline("%s empty.", Tobjnam(pobj, "are"));
							else {
							struct obj *o;
							/* view contents (not recursively) */
							for (o = pobj->cobj; o; o = o->nobj)
								o->dknown = 1;	/* "seen", even if blind */
							(void) display_cinventory(pobj);
							}
						}
					}
					You("probe beneath the %s.", surface(u.ux,u.uy));
					display_binventory(u.ux, u.uy, TRUE);
				} else {
					struct monst *mat = m_at(u.ux+u.dx,u.uy+u.dy);
					if(mat) probe_monster(mat);
					pobj = level.objects[u.ux+u.dx][u.uy+u.dy];
					for(; pobj; pobj = pobj->nexthere){
						/* target object has now been "seen (up close)" */
						pobj->dknown = 1;
						if (Is_container(pobj) || pobj->otyp == STATUE) {
							if (!pobj->cobj)
							pline("%s empty.", Tobjnam(pobj, "are"));
							else {
							struct obj *o;
							/* view contents (not recursively) */
							for (o = pobj->cobj; o; o = o->nobj)
								o->dknown = 1;	/* "seen", even if blind */
							(void) display_cinventory(pobj);
							}
						}
					}
				}
			} break;
			case WAN_SECRET_DOOR_DETECTION:
				findit();
			break;
		}
	}
	sensor->spe--;
	return MOVE_STANDARD;
}

STATIC_OVL int
sensorMenu()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet;
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	
	Sprintf(buf, "Functions");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	incntlet = 'c';
	Sprintf(buf, "Scan for creatures");
	any.a_int = POT_MONSTER_DETECTION;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);

	incntlet = 'd';
	Sprintf(buf, "Scan for secret doors");
	any.a_int = WAN_SECRET_DOOR_DETECTION;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);

	incntlet = 'o';
	Sprintf(buf, "Scan for objects");
	any.a_int = POT_OBJECT_DETECTION;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);

	incntlet = 'p';
	Sprintf(buf, "Focused probe");
	any.a_int = WAN_PROBING;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);

	incntlet = 't';
	Sprintf(buf, "Survey terrain");
	any.a_int = SPE_CLAIRVOYANCE;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	
	end_menu(tmpwin, "Choose function:");

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

STATIC_OVL int
use_hypospray(hypo)
struct obj *hypo;
{
	struct obj *amp = getobj(tools, "inject");
	int i, ii, nothing=0;
	if(!amp) return MOVE_CANCELLED;
	if(amp->otyp != HYPOSPRAY_AMPULE){
		You("can't inject that!");
		return MOVE_CANCELLED;
	}
    if (!getdir((char *)0)) return MOVE_CANCELLED;
	if(u.dz < 0){
		You("don't see a patient up there.");
		return MOVE_CANCELLED;
	} else if(u.dz > 0){
		You("doubt the floor will respond to drugs.");
		return MOVE_CANCELLED;
	} else if(u.dx || u.dy){
		struct monst *mtarg = m_at(u.ux+u.dx,u.uy+u.dy);
		if(!mtarg){
			You("don't find a patient there.");
			return MOVE_STANDARD;
		}
		if(amp->spe <= 0){
			pline("The ampule is empty!");
			return MOVE_STANDARD;
		}
		if(!has_blood_mon(mtarg)){
			pline("It would seem that the patient has no circulatory system....");
		} else switch(amp->ovar1_ampule){
			case POT_HEALING:
				if (mtarg->mtyp == PM_PESTILENCE){
					mtarg->mhp -= d(6 + 2 * bcsign(amp), 4);
					if(mtarg->mhp <= 0) xkilled(mtarg,1);
					break;
				}
				if(mtarg->mhp < mtarg->mhpmax) {
					mtarg->mhp = min_ints(mtarg->mhpmax,mtarg->mhp+d(6 + 2 * bcsign(amp), 4));
					if (canseemon(mtarg))
					pline("%s looks better.", Monnam(mtarg));
				}
			break;
			case POT_EXTRA_HEALING:
				if (mtarg->mtyp == PM_PESTILENCE){
					mtarg->mhp -= d(6 + 2 * bcsign(amp), 8);
					if(mtarg->mhp <= 0) xkilled(mtarg,1);
					break;
				}
				if(mtarg->mhp < mtarg->mhpmax) {
					mtarg->mhp = min_ints(mtarg->mhpmax,mtarg->mhp+d(6 + 2 * bcsign(amp), 8));
					if (canseemon(mtarg))
					pline("%s looks much better.", Monnam(mtarg));
				}
			break;
			case POT_FULL_HEALING:
				if (mtarg->mtyp == PM_PESTILENCE){
					if((mtarg->mhpmax > 3) && !resist(mtarg, POTION_CLASS, 0, NOTELL))
						mtarg->mhpmax /= 2;
					if((mtarg->mhp > 2) && !resist(mtarg, POTION_CLASS, 0, NOTELL))
						mtarg->mhp /= 2;
					if (mtarg->mhp > mtarg->mhpmax) mtarg->mhp = mtarg->mhpmax;
					if(mtarg->mhp <= 0) xkilled(mtarg,1);
					if (canseemon(mtarg))
						pline("%s looks rather ill.", Monnam(mtarg));
					break;
				}
			case POT_GAIN_ABILITY:
			case POT_RESTORE_ABILITY:
				if(mtarg->mhp < mtarg->mhpmax) {
					mtarg->mhp = min(mtarg->mhpmax,mtarg->mhp+400);
					if (canseemon(mtarg))
					pline("%s looks sound and hale again.", Monnam(mtarg));
				}
			break;
			case POT_BLINDNESS:
				if(haseyes(mtarg->data)) {
					register int btmp = rn1(200, 250 - 125 * bcsign(amp));
					btmp += mtarg->mblinded;
					mtarg->mblinded = min(btmp,127);
					mtarg->mcansee = 0;
				}
			break;
			case POT_HALLUCINATION:
				if(!resist(mtarg, POTION_CLASS, 0, NOTELL)) 
					mtarg->mstun = TRUE;
			case POT_CONFUSION:
				if(!resist(mtarg, POTION_CLASS, 0, NOTELL)) 
					mtarg->mconf = TRUE;
			break;
			case POT_PARALYSIS:
				if (mtarg->mcanmove) {
					mtarg->mcanmove = 0;
					mtarg->mfrozen = rn1(10, 25 - 12*bcsign(amp));
				}
			break;
			case POT_SPEED:
				mon_adjust_speed(mtarg, 1, amp, TRUE);
			break;
			case POT_GAIN_ENERGY:
				if(amp->cursed){
					if (canseemon(mtarg))
						pline("%s looks lackluster.", Monnam(mtarg));
					set_mcan(mtarg, TRUE);
				} else {
					if (canseemon(mtarg))
						pline("%s looks full of energy.", Monnam(mtarg));
					mtarg->mspec_used = 0;
					set_mcan(mtarg, FALSE);
				}
			break;
			case POT_SLEEPING:
				if (sleep_monst(mtarg, rn1(10, 25 - 12*bcsign(amp)), POTION_CLASS)) {
					pline("%s falls asleep.", Monnam(mtarg));
					slept_monst(mtarg);
				}
			break;
			case POT_POLYMORPH:
				if (canseemon(mtarg)) pline("%s suddenly mutates!", Monnam(mtarg));
				if(!resists_poly(mtarg->data))
					newcham(mtarg, NON_PM, FALSE, FALSE);
			break;
			case POT_AMNESIA:
				if(!amp->cursed){
					if (canseemon(mtarg))
						pline("%s looks more tranquil.", Monnam(mtarg));
					if(!amp->blessed){
						untame(mtarg, 1);
						mtarg->mferal = 0;
					}
					mtarg->mcrazed = 0;
					mtarg->mdisrobe = 0;
					mtarg->mberserk = 0;
					mtarg->mdoubt = 0;
				} else {
					if (canseemon(mtarg))
						pline("%s looks angry and confused!", Monnam(mtarg));
					untame(mtarg, 0);
					mtarg->mcrazed = 1;
					mtarg->mberserk = 1;
					mtarg->mconf = 1;
					mtarg->mferal = 0;
				}
			break;
		}
	} else {
		if(amp->spe <= 0){
			pline("The ampule is empty!");
			return MOVE_STANDARD;
		}
		switch(amp->ovar1_ampule){
			case POT_GAIN_ABILITY:
				if(amp->cursed) {
					//poison
				} else if (Fixed_abil) {
					nothing++;
				} else {      /* If blessed, increase all; if not, try up to */
					int itmp; /* 6 times to find one which can be increased. */
					i = -1;		/* increment to 0 */
					for (ii = A_MAX; ii > 0; ii--) {
					i = (amp->blessed ? i + 1 : rn2(A_MAX));
					/* only give "your X is already as high as it can get"
					   message on last attempt (except blessed potions) */
					itmp = (amp->blessed || ii == 1) ? 0 : -1;
					if (adjattrib(i, 1, itmp) && !amp->blessed)
						break;
					}
				}
			break;
			case POT_RESTORE_ABILITY:
				if (amp->blessed && u.ulevel < u.ulevelmax) {
					pluslvl(FALSE);
				}
				if(amp->blessed && u.umorgul>0){
					u.umorgul--;
					if(u.umorgul)
						You_feel("the chill of death lessen.");
					else
						You_feel("the chill of death fade away.");
				}
				if(amp->blessed && u.umummyrot){
					u.umummyrot = 0;
					You("stop shedding dust.");
				}
				if(!amp->cursed){
					//Restore sanity if blessed or uncursed
					if(amp->blessed)
						change_usanity(20, FALSE);
					else
						change_usanity(5, FALSE);
				}
				if(amp->cursed) {
					pline("Ulch!  This makes you feel mediocre!");
					break;
				} else {
					int lim;
					pline("Wow!  This makes you feel %s!",
					  (amp->blessed) ?
						(unfixable_trouble_count(FALSE) ? "better" : "great")
					  : "good");
					i = rn2(A_MAX);		/* start at a random point */
					for (ii = 0; ii < A_MAX; ii++) {
					lim = AMAX(i);
					if (i == A_STR && u.uhs >= 3) --lim;	/* WEAK */
					if (ABASE(i) < lim) {
						ABASE(i) = lim;
						flags.botl = 1;
						/* only first found if not blessed */
						if (!amp->blessed) break;
					}
					if(++i >= A_MAX) i = 0;
					}
				}
			break;
			case POT_BLINDNESS:
				if(Blind) nothing++;
				make_blinded(itimeout_incr(Blinded,
							   rn1(200, 250 - 125 * bcsign(amp))),
						 (boolean)!Blind);
			break;
			case POT_CONFUSION:
				if(!Confusion)
					if (Hallucination) {
						pline("What a trippy feeling!");
					} else pline("Huh, What?  Where am I?");
				else nothing++;
				make_confused(itimeout_incr(HConfusion,
								rn1(7, 16 - 8 * bcsign(amp))),
						  FALSE);
			break;
			case POT_PARALYSIS:
				if (Free_action)
					You("stiffen momentarily.");
				else {
					if (Levitation || Weightless || Is_waterlevel(&u.uz))
					You("are motionlessly suspended.");
#ifdef STEED
					else if (u.usteed)
					You("are frozen in place!");
#endif
					else
					Your("%s are frozen to the %s!",
						 makeplural(body_part(FOOT)), surface(u.ux, u.uy));
					nomul(-(rn1(10, 25 - 12*bcsign(amp))), "frozen by a potion");
					nomovemsg = You_can_move_again;
					exercise(A_DEX, FALSE);
				}
			break;
			case POT_SPEED:
				if(Wounded_legs && !amp->cursed
#ifdef STEED
				&& !u.usteed	/* heal_legs() would heal steeds legs */
#endif
								) {
					heal_legs();
					break;
				}
				if (!(HFast & INTRINSIC)) {
					if (!Fast) You("speed up.");
					else Your("quickness feels more natural.");
					HFast |= TIMEOUT_INF;
				} else nothing++;
				exercise(A_DEX, TRUE);
			break;
			case POT_HALLUCINATION:
				if (Hallucination || Halluc_resistance) nothing++;
				(void) make_hallucinated(itimeout_incr(HHallucination,
							   rn1(200, 600 - 300 * bcsign(amp))),
						  TRUE, 0L);
				//Bad drugs: inflict brain damage
				if(amp->cursed){
					if(u.usanity > 0)
						change_usanity(-1, FALSE);
					if(u.uinsight > 0)
						change_uinsight(-1);
					exercise(A_WIS, FALSE);
					exercise(A_INT, FALSE);
				}
			break;
			case POT_HEALING:
				You_feel("better.");
				healup(d(6 + 2 * bcsign(amp), 4),
					   (!amp->cursed ? 1 : 0), amp->blessed, !amp->cursed);
				exercise(A_CON, TRUE);
			break;
			case POT_EXTRA_HEALING:
				You_feel("much better.");
				healup(d(6 + 2 * bcsign(amp), 8),
					    (1+1*bcsign(amp)), !amp->cursed, TRUE);
				(void) make_hallucinated(0L,TRUE,0L);
				exercise(A_CON, TRUE);
				exercise(A_STR, TRUE);
			break;
			case POT_GAIN_ENERGY:
			{	register int num;
				num = rnd(2) + 2 * amp->blessed + 1;
				u.uenbonus += (amp->cursed) ? -num : num;
				calc_total_maxen();
				u.uen += (amp->cursed) ? -100 : (amp->blessed) ? 200 : 100;
				if(u.uen > u.uenmax) u.uen = u.uenmax;
				if(u.uen <= 0 && !Race_if(PM_INCANTIFIER)) u.uen = 0;
				flags.botl = 1;
				if(!amp->cursed) exercise(A_WIS, TRUE);
				//Doing the print last causes the bottom line update to show the changed energy scores.
				if(amp->cursed)
					You_feel("lackluster.");
				else
					pline("Magical energies course through your body.");
			}
			break;
			case POT_SLEEPING:
				if(Sleep_resistance || Free_action)
					You("yawn.");
				else {
					You("suddenly fall asleep!");
					fall_asleep(-rn1(10, 25 - 12*bcsign(amp)), TRUE);
				}
				//Sedative
				change_usanity(5 + 10*bcsign(amp), FALSE);
			break;
			case POT_FULL_HEALING:
				You_feel("completely healed.");
				healup(400, (2+2*bcsign(amp)), !amp->cursed, TRUE);
				/* Restore one lost level if blessed */
				if (amp->blessed && u.ulevel < u.ulevelmax) {
					///* when multiple levels have been lost, drinking
					//   multiple potions will only get half of them back */
					// u.ulevelmax -= 1;
					pluslvl(FALSE);
				}
				/* Dissolve one morgul blade shard if blessed*/
				if(amp->blessed && u.umorgul>0){
					u.umorgul--;
					if(u.umorgul)
						You_feel("the chill of death lessen.");
					else
						You_feel("the chill of death fade away.");
				}
				if(amp->blessed && u.umummyrot){
					u.umummyrot = 0;
					You("stop shedding dust.");
				}
				(void) make_hallucinated(0L,TRUE,0L);
				exercise(A_STR, TRUE);
				exercise(A_CON, TRUE);
			break;
			case POT_POLYMORPH:
				You_feel("a little %s.", Hallucination ? "normal" : "strange");
				if (!Unchanging) polyself(FALSE);
			break;
			case POT_AMNESIA:
				forget(amp->cursed ? 25 : amp->blessed ? 0 : 10);
				if (Hallucination)
					pline("Hakuna matata!");
				else
					You_feel("your memories dissolve.");

				/* Blessed amnesia makes you forget lycanthropy, sickness */
				if (amp->blessed) {
					if (u.ulycn >= LOW_PM && !Race_if(PM_HUMAN_WEREWOLF)) {
					You("forget your affinity to %s!",
							makeplural(mons[u.ulycn].mname));
					if (youmonst.data->mtyp == u.ulycn)
						you_unwere(FALSE);
					u.ulycn = NON_PM;	/* cure lycanthropy */
					}
					make_sick(0L, (char *) 0, TRUE, SICK_ALL);

					/* You feel refreshed */
					if(Race_if(PM_INCANTIFIER)) u.uen += 50 + rnd(50);
					else u.uhunger += 50 + rnd(50);
					
					newuhs(FALSE);
				} else {
					if(Role_if(PM_MADMAN)){
						You_feel("ashamed of wiping your own memory.");
						u.hod += amp->cursed ? 5 : 2;
					}
					exercise(A_WIS, FALSE);
				}

				//All amnesia causes you to forget your crisis of faith
				if(Doubt)
					You("forget your doubts.");
				make_doubtful(0L, FALSE);
			break;
		}
		if(nothing) {
			You("have a %s feeling for a moment, then it passes.",
			  Hallucination ? "normal" : "peculiar");
		}
	}
	amp->spe--;
	return MOVE_STANDARD;
}

/* Place a landmine/bear trap.  Helge Hafting */
STATIC_OVL void
use_trap(otmp)
struct obj *otmp;
{
	int ttyp, tmp;
	const char *what = (char *)0;
	char buf[BUFSZ];
	const char *occutext = "setting the trap";

	if (nohands(youracedata))
	    what = "without hands";
	else if(!freehand())
	    what = "without free hands";
	else if (Stunned)
	    what = "while stunned";
	else if (u.uswallow)
	    what = is_animal(u.ustuck->data) ? "while swallowed" :
			"while engulfed";
	else if (Underwater)
	    what = "underwater";
	else if (Levitation)
	    what = "while levitating";
	else if (is_pool(u.ux, u.uy, TRUE))
	    what = "in water";
	else if (is_lava(u.ux, u.uy))
	    what = "in lava";
	else if (On_stairs(u.ux, u.uy))
	    what = (u.ux == xdnladder || u.ux == xupladder) ?
			"on the ladder" : "on the stairs";
	else if (IS_FURNITURE(levl[u.ux][u.uy].typ) ||
		IS_ROCK(levl[u.ux][u.uy].typ) ||
		closed_door(u.ux, u.uy) || t_at(u.ux, u.uy))
	    what = "here";
	if (what) {
	    You_cant("set a trap %s!",what);
	    reset_trapset();
	    return;
	}
	ttyp = (otmp->otyp == LAND_MINE) ? LANDMINE : BEAR_TRAP;
	if (otmp == trapinfo.tobj &&
		u.ux == trapinfo.tx && u.uy == trapinfo.ty) {
	    You("resume setting %s %s.",
		shk_your(buf, otmp),
		defsyms[trap_to_defsym(what_trap(ttyp))].explanation);
	    set_occupation(set_trap, occutext, 0);
	    return;
	}
	trapinfo.tobj = otmp;
	trapinfo.tx = u.ux,  trapinfo.ty = u.uy;
	tmp = ACURR(A_DEX);
	trapinfo.time_needed = (tmp > 17) ? 2 : (tmp > 12) ? 3 :
				(tmp > 7) ? 4 : 5;
	if (Blind) trapinfo.time_needed *= 2;
	tmp = ACURR(A_STR);
	if (ttyp == BEAR_TRAP && tmp < 18)
	    trapinfo.time_needed += (tmp > 12) ? 1 : (tmp > 7) ? 2 : 4;
	/*[fumbling and/or confusion and/or cursed object check(s)
	   should be incorporated here instead of in set_trap]*/
#ifdef STEED
	if (u.usteed && P_SKILL(P_RIDING) < P_BASIC) {
	    boolean chance;

	    if (Fumbling || otmp->cursed) chance = (rnl(100) > 30);
	    else  chance = (rnl(100) > 50);
	    You("aren't very skilled at reaching from %s.",
		mon_nam(u.usteed));
	    Sprintf(buf, "Continue your attempt to set %s?",
		the(defsyms[trap_to_defsym(what_trap(ttyp))].explanation));
	    if(yn(buf) == 'y') {
		if (chance) {
			switch(ttyp) {
			    case LANDMINE:	/* set it off */
			    	trapinfo.time_needed = 0;
			    	trapinfo.force_bungle = TRUE;
				break;
			    case BEAR_TRAP:	/* drop it without arming it */
				reset_trapset();
				You("drop %s!",
			  the(defsyms[trap_to_defsym(what_trap(ttyp))].explanation));
				dropx(otmp);
				return;
			}
		}
	    } else {
	    	reset_trapset();
		return;
	    }
	}
#endif
	You("begin setting %s %s.",
	    shk_your(buf, otmp),
	    defsyms[trap_to_defsym(what_trap(ttyp))].explanation);
	
	if (otmp->unpaid){
		verbalize("You set it, you buy it!");
		if (!rn2(3))
			verbalize("If you hurt somebody, it's not my fault!");
		bill_dummy_object(otmp);
	} else if (costly_spot(u.ux, u.uy)) {
		verbalize("Don't hurt my customers!");
	}
	
	set_occupation(set_trap, occutext, 0);
	return;
}

STATIC_PTR
int
set_trap()
{
	struct obj *otmp = trapinfo.tobj;
	struct trap *ttmp;
	int ttyp;
	boolean obj_cursed = otmp->cursed;

	if (!otmp || !carried(otmp) ||
		u.ux != trapinfo.tx || u.uy != trapinfo.ty) {
	    /* ?? */
	    reset_trapset();
	    return MOVE_CANCELLED;
	}

	if (--trapinfo.time_needed > 0) return MOVE_STANDARD;	/* still busy */

	ttyp = (otmp->otyp == LAND_MINE) ? LANDMINE : BEAR_TRAP;
	ttmp = maketrap(u.ux, u.uy, ttyp);
	if (ttmp) {
	    ttmp->tseen = 1;
	    ttmp->madeby_u = 1;
	    newsym(u.ux, u.uy); /* if our hero happens to be invisible */

		/* Our object becomes the new ammo of the trap. */
		if (otmp->quan > 1) {
			otmp = splitobj(otmp, 1);
		}
		freeinv(otmp);
		set_trap_ammo(ttmp, otmp);

	    if (*in_rooms(u.ux,u.uy,SHOPBASE)) {
		add_damage(u.ux, u.uy, 0L);		/* schedule removal */
	    }
	    if (!trapinfo.force_bungle)
			You("finish arming %s.",
				the(defsyms[trap_to_defsym(what_trap(ttyp))].explanation));
		if (((obj_cursed || Fumbling) && (rnl(100) > 50)) || trapinfo.force_bungle)
			dotrap(ttmp,
				(unsigned)(trapinfo.force_bungle ? FORCEBUNGLE : 0));
	} else {
	    /* this shouldn't happen */
	    Your("trap setting attempt fails.");
	}
	reset_trapset();
	return MOVE_FINISHED_OCCUPATION;
}

STATIC_OVL int
use_droven_cloak(optr)
struct obj **optr;
{
	struct obj *otmp = *optr;
	int rx, ry;
	const char *what = (char *)0;
	struct trap *ttmp;
	struct monst *mtmp;

    if (!getdir((char *)0) || u.dz < 0) return MOVE_CANCELLED;

    if (Stunned || (Confusion && !rn2(5))) confdir();
    rx = u.ux + u.dx;
    ry = u.uy + u.dy;
    mtmp = m_at(rx, ry);
	ttmp = t_at(rx, ry);

	if (!isok(rx, ry))
		what = "here";
	else if (Stunned)
	    what = "while stunned";
	else if (u.uswallow)
	    what = is_animal(u.ustuck->data) ? "while swallowed" :
			"while engulfed";
	else if (Underwater)
	    what = "underwater";
	else if (Levitation)
	    what = "while levitating";
	else if (is_pool(rx, ry, TRUE))
	    what = "in water";
	else if (is_lava(rx, ry))
	    what = "in lava";
	else if (On_stairs(rx, ry) && !(ttmp && ttmp->ttyp == WEB))
	    what = (rx == xdnladder || rx == xupladder) ?
			"on the ladder" : "on the stairs";
	else if (IS_FURNITURE(levl[rx][ry].typ) ||
		IS_ROCK(levl[rx][ry].typ) ||
		closed_door(rx, ry) || (ttmp && ttmp->ttyp != WEB))
	    what = "here";
	if (what && !(ttmp && ttmp->ttyp == WEB)) {
	    You_cant("set a trap %s!",what);
	    reset_trapset();
	    return MOVE_CANCELLED;
	}
	
	if(ttmp) {
		if(otmp->oeroded3) otmp->oeroded3--;
		pline("The cloak sweeps up a web!");
		if(!Is_lolth_level(&u.uz)){ //results in unlimited recharging in lolths domain, no big deal
			deltrap(ttmp);
			newsym(rx, ry);
		}
		if(rx==u.ux && ry==u.uy) u.utrap = 0;
		else if(mtmp) mtmp->mtrapped = 0;
	}
	else if(!(otmp->oartifact) || otmp->oeroded3 < 3){
		ttmp = maketrap(rx, ry, WEB);
		if(ttmp){
			pline("A web spins out from the cloak!");
			ttmp->madeby_u = 1;
			ttmp->tseen = 1;
			newsym(rx, ry);
			if (*in_rooms(rx,ry,SHOPBASE)) {
				add_damage(rx, ry, 0L);		/* schedule removal */
			}
			if(rx==u.ux && ry==u.uy) dotrap(ttmp, NOWEBMSG);
			else if(mtmp) mintrap(mtmp);
		} else pline("The cloak cannot spin a web there!");
		if(otmp->oeroded3 == 3){
			useup(otmp);
			*optr = 0;
			pline("The thoroughly tattered cloak falls to pieces");
		} else otmp->oeroded3++;
	} else {
		pline("The cloak cannot spin any more webs.");
		return MOVE_CANCELLED;
	}
	reset_trapset();
	return MOVE_STANDARD;
}

STATIC_OVL int
use_darkweavers_cloak(otmp)
struct obj *otmp;
{
	const char *what = (char *)0;

	if (Stunned)
	    what = "while stunned";
	else if (u.uswallow)
	    what = is_animal(u.ustuck->data) ? "while swallowed" :
			"while engulfed";
	else if (Underwater)
	    what = "underwater";
	if (what) {
	    You_cant("release darkness %s!",what);
	    return MOVE_CANCELLED;
	}
	
	if(!levl[u.ux][u.uy].lit) {
		if(otmp->spe < 7) otmp->spe++;
		pline("The cloak sweeps up the dark!");
		litroom(TRUE, otmp);	/* only needs to be done once */
	}
	else if(otmp->spe > -5){
		otmp->spe--;
		pline("The cloak releases a cloud of darkness!");
		litroom(FALSE, otmp);	/* only needs to be done once */
	}
	return MOVE_STANDARD;
}

STATIC_OVL int
use_eilistran_armor(optr)
struct obj **optr;
{
	struct obj *otmp = *optr;
	winid tmpwin;
	anything any;
	menu_item *selected;
	int n;

	any.a_void = 0;         /* zero out all bits */
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	
	if(otmp->ovar1_eilistran_charges > 0){
		any.a_int = 1;
		add_menu(tmpwin, NO_GLYPH, &any , 't', 0, ATR_NONE,
			 (otmp->altmode == EIL_MODE_ON) ? "Turn off." : "Turn on.", MENU_UNSELECTED);
	}
	if(otmp->ovar1_eilistran_charges <= 540){
		any.a_int = 2;
		add_menu(tmpwin, NO_GLYPH, &any , 'r', 0, ATR_NONE,
			 "Replace worn components.", MENU_UNSELECTED);
	}

	end_menu(tmpwin, "Do what?");
	n = select_menu(tmpwin, PICK_ONE, &selected);
	if(n > 0){
		n = selected[0].item.a_int;
		free(selected);
	}
	destroy_nhwindow(tmpwin);
	if(!n)
		return MOVE_CANCELLED;
	
	switch(n){
		case 1:
			if(otmp->altmode == EIL_MODE_ON){
				otmp->altmode = EIL_MODE_OFF;
				You("Switch the armor off.");
				return MOVE_PARTIAL;
			}
			else {
				otmp->altmode = EIL_MODE_ON;
				You("Switch the armor on.");
				return MOVE_PARTIAL;
			}
		break;
		case 2:{
			struct obj *component = getobj(tools, "replace with");
			if(!component)
				return MOVE_CANCELLED;
			else if(component->otyp != CLOCKWORK_COMPONENT){
				pline("This device requires clockwork components.");
				return MOVE_CANCELLED;
			}
			else if(component->cursed){
				pline("The component won't go into the mechanism!");
				return MOVE_STANDARD;
			}
			//else
			useup(component);
			You("put the new component into the armor's mechanism.");
			otmp->ovar1_eilistran_charges += 60;
			return MOVE_STANDARD;
		}break;
	}
	return MOVE_STANDARD;
}

int
use_whip(obj)
struct obj *obj;
{
    char buf[BUFSZ];
    struct monst *mtmp;
    struct obj *otmp;
    int rx, ry, proficient, res = MOVE_CANCELLED;
    const char *msg_slipsfree = "The whip slips free.";
    const char *msg_snap = "Snap!";
	boolean ranged = FALSE;

    if (obj != uwep) {
	if (!wield_tool(obj, "lash")) return MOVE_CANCELLED;
	else res = MOVE_STANDARD;
    }
	if(Straitjacketed){
		You("can't snap a whip while your %s are bound!", makeplural(body_part(ARM)));
		return MOVE_CANCELLED;
	}
    if (!getdir((char *)0)) return res;

	if(obj->otyp == FORCE_WHIP && !u.dx && !u.dy && !u.dz){
		return use_force_sword(obj);
	}

    if (Stunned || (Confusion && !rn2(5))) confdir();
    rx = u.ux + u.dx;
    ry = u.uy + u.dy;
	if (!isok(rx,ry)) {
		pline("%s",msg_snap);
		return MOVE_STANDARD;
	}
    mtmp = m_at(rx, ry);
	if(obj->oartifact == ART_GOLDEN_SWORD_OF_Y_HA_TALLA && ZAP_POS(levl[rx][ry].typ)){
		if(u.utrap && u.utraptype == TT_PIT){
			if(!mtmp && !IS_FURNITURE(levl[rx][ry].typ) && !boulder_at(rx, ry)){
				rx = rx + u.dx;
				ry = ry + u.dy;
				ranged = TRUE;
			}
		}
		else if(!mtmp){
			rx = rx + u.dx;
			ry = ry + u.dy;
			ranged = TRUE;
		}
		if (!isok(rx,ry)) {
			pline("%s",msg_snap);
			return MOVE_STANDARD;
		}
		mtmp = m_at(rx, ry);
	}

    /* proficiency check */
    proficient = P_SKILL(P_WHIP)-P_UNSKILLED;
    if (Role_if(PM_ARCHEOLOGIST)) ++proficient;
    if (ACURR(A_DEX) < 6) proficient--;
    else if (ACURR(A_DEX) >= 14) proficient += (ACURR(A_DEX) - 11)/3;
    if (Fumbling) --proficient;
    if (proficient > 3) proficient = 3;
    if (proficient < 0) proficient = 0;

    if (u.uswallow && attack2(u.ustuck)) {
	There("is not enough room to flick your whip.");

    } else if (Underwater) {
	There("is too much resistance to flick your whip.");

    } else if (u.dz < 0) {
	You("flick a bug off of the %s.",ceiling(u.ux,u.uy));

    } else if ((!u.dx && !u.dy) || (u.dz > 0)) {
	int dam;

#ifdef STEED
		/* Sometimes you hit your steed by mistake */
		if (u.usteed && !rn2(proficient + 2)) {
			You("whip %s!", mon_nam(u.usteed));
			kick_steed();
			return MOVE_STANDARD;
		}
#endif
		if (Levitation
#ifdef STEED
			|| u.usteed
#endif
			) {
			/* Have a shot at snaring something on the floor */
			otmp = level.objects[u.ux][u.uy];
			if (otmp && otmp->otyp == CORPSE && otmp->corpsenm == PM_HORSE) {
			pline("Why beat a dead horse?");
			return MOVE_STANDARD;
			}
			if (otmp && proficient) {
			You("wrap your whip around %s on the %s.",
				an(singular(otmp, xname)), surface(u.ux, u.uy));
			if (rnl(100) >= 16 || pickup_object(otmp, 1L, TRUE) < 1)
				pline("%s", msg_slipsfree);
			return MOVE_STANDARD;
			}
		}
		dam = rnd(2) + dbon(obj) + obj->spe;
		if (dam <= 0) dam = 1;
		You("hit your %s with your whip.", body_part(FOOT));
		Sprintf(buf, "killed %sself with %s whip", uhim(), uhis());
		losehp(dam, buf, NO_KILLER_PREFIX);
		flags.botl = 1;
		return MOVE_STANDARD;

    } else if ((Fumbling || Glib) && !rn2(5)) {
		pline_The("whip slips out of your %s.", body_part(HAND));
		dropx(obj);

    } else if (u.utrap && u.utraptype == TT_PIT) {
		/*
		 *     Assumptions:
		 *
		 *	if you're in a pit
		 *		- you are attempting to get out of the pit
		 *		- or, if you are applying it towards a small
		 *		  monster then it is assumed that you are
		 *		  trying to hit it.
		 *	else if the monster is wielding a weapon
		 *		- you are attempting to disarm a monster
		 *	else
		 *		- you are attempting to hit the monster
		 *
		 *	if you're confused (and thus off the mark)
		 *		- you only end up hitting.
		 *
		 */
		const char *wrapped_what = (char *)0;

		if (mtmp) {
			if (bigmonst(mtmp->data)) {
				wrapped_what = strcpy(buf, mon_nam(mtmp));
			} else if (proficient) {
				struct attack attk = {AT_WEAP, AD_PHYS, 0, 0};
				if (ranged ? (xmeleehity(&youmonst, mtmp, &attk, &otmp, -1, 0, TRUE) != MM_AGR_DIED) : attack2(mtmp)) return MOVE_ATTACKED;
				else pline("%s", msg_snap);
			}
		}
		if (!wrapped_what) {
			if (IS_FURNITURE(levl[rx][ry].typ))
			wrapped_what = something;
			else if (boulder_at(rx, ry))
			wrapped_what = xname(boulder_at(rx,ry));
		}
		if (wrapped_what) {
			coord cc;

			cc.x = rx; cc.y = ry;
			You("wrap your whip around %s.", wrapped_what);
			if (proficient && rn2(proficient + 2)) {
			if (!mtmp || enexto(&cc, rx, ry, youracedata)) {
				You("yank yourself out of the pit!");
				teleds(cc.x, cc.y, TRUE);
				u.utrap = 0;
				vision_full_recalc = 1;
			}
			} else {
			pline("%s", msg_slipsfree);
			}
			if (mtmp) wakeup(mtmp, TRUE);
		} else pline("%s", msg_snap);

    } else if (mtmp) {
		if (!canspotmon(mtmp) &&
			!glyph_is_invisible(levl[rx][ry].glyph)) {
		   pline("A monster is there that you couldn't see.");
		   map_invisible(rx, ry);
		}
		otmp = rn2(3) ? MON_WEP(mtmp) : MON_SWEP(mtmp);	/* can be null */
		if (otmp) {
			char onambuf[BUFSZ];
			const char *mon_hand;
			boolean gotit = proficient && (!Fumbling || !rn2(10));

			Strcpy(onambuf, cxname(otmp));
			if (gotit) {
			mon_hand = mbodypart(mtmp, HAND);
			if (bimanual(otmp,mtmp->data)) mon_hand = makeplural(mon_hand);
			} else
			mon_hand = 0;	/* lint suppression */

			You("wrap your whip around %s %s.",
			s_suffix(mon_nam(mtmp)), onambuf);
			if (gotit && otmp->cursed && !is_weldproof_mon(mtmp)) {
			pline("%s welded to %s %s%c",
				  (otmp->quan == 1L) ? "It is" : "They are",
				  mhis(mtmp), mon_hand,
				  !otmp->bknown ? '!' : '.');
			otmp->bknown = 1;
			gotit = FALSE;	/* can't pull it free */
			}
			if (gotit) {
			obj_extract_self(otmp);
			possibly_unwield(mtmp, FALSE);
			setmnotwielded(mtmp,otmp);

			switch (rn2(proficient + 1)) {
			case 2:
				/* to floor near you */
				You("yank %s %s to the %s!", s_suffix(mon_nam(mtmp)),
				onambuf, surface(u.ux, u.uy));
				place_object(otmp, u.ux, u.uy);
				stackobj(otmp);
				break;
			case 3:
				/* right to you */
#if 0
				if (!rn2(25)) {
				/* proficient with whip, but maybe not
				   so proficient at catching weapons */
				int hitu, hitvalu;
				int dieroll;
				hitvalu = tohitval((struct monst *)0, &youmonst, (struct attack *)0, otmp, (void *)0, HMON_PROJECTILE, 8, (int *) 0);
				if(hitvalu > (dieroll = rnd(20)) || dieroll == 1) {
					boolean wepgone = FALSE;
					pline_The("%s hits you as you try to snatch it!" the(onambuf));
					hmon_general((struct monst *)0, &youmonst, (struct attack *)0, &otmp, (void *)0, HMON_PROJECTILE,
						0, 0, FALSE, dieroll, FALSE, -1);
				}
				else {
					if(Blind || !flags.verbose) pline("It misses.");
					else You("are almost hit by %s.", the(onambuf));
				}
				place_object(otmp, u.ux, u.uy);
				stackobj(otmp);
				break;
				}
#endif /* 0 */
				/* right into your inventory */
				You("snatch %s %s!", s_suffix(mon_nam(mtmp)), onambuf);
				if (otmp->otyp == CORPSE &&
					touch_petrifies(&mons[otmp->corpsenm]) &&
					!uarmg && !Stone_resistance &&
					!(poly_when_stoned(youracedata) &&
					polymon(PM_STONE_GOLEM))) {
				char kbuf[BUFSZ];

				Sprintf(kbuf, "%s corpse",
					an(mons[otmp->corpsenm].mname));
				pline("Snatching %s is a fatal mistake.", kbuf);
				instapetrify(kbuf);
				}
				otmp = hold_another_object(otmp, "You drop %s!",
							   doname(otmp), (const char *)0);
				break;
			default:
				/* to floor beneath mon */
				You("yank %s from %s %s!", the(onambuf),
				s_suffix(mon_nam(mtmp)), mon_hand);
				obj_no_longer_held(otmp);
				place_object(otmp, mtmp->mx, mtmp->my);
				stackobj(otmp);
				break;
			}
			} else {
			pline("%s", msg_slipsfree);
			}
			wakeup(mtmp, TRUE);
		} else {
			if ((mtmp->m_ap_type && mtmp->m_ap_type != M_AP_MONSTER) &&
			!Protection_from_shape_changers && !sensemon(mtmp))
			stumble_onto_mimic(mtmp);
			else You("flick your whip towards %s.", mon_nam(mtmp));
			if (proficient) {
				struct attack attk = {AT_WEAP, AD_PHYS, 0, 0};
				if (ranged ? (xmeleehity(&youmonst, mtmp, &attk, &otmp, -1, 0, TRUE) != MM_AGR_DIED) : attack2(mtmp)) return MOVE_ATTACKED;
				else pline("%s", msg_snap);
			}
		}

    } else if (Weightless || Is_waterlevel(&u.uz)) {
	    /* it must be air -- water checked above */
	    You("snap your whip through thin air.");

    } else {
	pline("%s", msg_snap);

    }
    return MOVE_STANDARD;
}


int
use_nunchucks(obj)
struct obj *obj;
{
    char buf[BUFSZ];
    struct monst *mtmp;
    struct obj *otmp;
    int rx, ry, proficient, res = MOVE_CANCELLED;
    const char *msg_slipsfree = "The nunchaku slip free.";
    const char *msg_snap = "Swish!";

    if (obj != uwep) {
		if (!wield_tool(obj, "nunchaku")) return MOVE_CANCELLED;
		else res = Role_if(PM_MONK) ? MOVE_PARTIAL : MOVE_STANDARD;
    }
	if(Straitjacketed){
		You("can't swing nunchaku while your %s are bound!", makeplural(body_part(ARM)));
		return MOVE_CANCELLED;
	}

	if (!getdir((char *)0)) return res;
	else res = Role_if(PM_MONK) ? MOVE_PARTIAL : MOVE_STANDARD;

	if (Stunned || (Confusion && !rn2(5))) confdir();
	rx = u.ux + u.dx;
	ry = u.uy + u.dy;
	if (!isok(rx,ry)) {
		pline("%s",msg_snap);
		return res;
	}
    mtmp = m_at(rx, ry);

    /* proficiency check */
    proficient = 0;
	if(martial_bonus()){
		proficient = min(P_SKILL(P_FLAIL)-P_UNSKILLED, P_SKILL(P_MARTIAL_ARTS)-P_BASIC);
	}
    if (Role_if(PM_MONK)) ++proficient;
    if (ACURR(A_DEX) < 6) proficient--;
    else if (ACURR(A_DEX) >= 14) proficient += (ACURR(A_DEX) - 11)/3;
    if (Fumbling) --proficient;
    if (proficient > 3) proficient = 3;
    if (proficient < 0) proficient = 0;

    if (u.uswallow && attack2(u.ustuck)) {
	There("is not enough room to swing your nunchaku.");

    } else if (Underwater) {
	There("is too much resistance to swing your nunchaku.");

    } else if (u.dz < 0) {
		if(!Levitation && !Flying)
			You("glare impotently at the bugs on the %s.",ceiling(u.ux,u.uy));
		else 
			You("smash a bug on the %s.",ceiling(u.ux,u.uy));

    } else if ((!u.dx && !u.dy) || (u.dz > 0)) {
		int dam;
		dam = rnd(4) + dbon(obj) + obj->spe;
		if (dam <= 0) dam = 1;
		You("hit your %s with your nunchaku.", body_part((u.dz > 0) ? FOOT : HAND));
		Sprintf(buf, "killed %sself with %s whip", uhim(), uhis());
		losehp(dam, buf, NO_KILLER_PREFIX);
		flags.botl = 1;
		return MOVE_STANDARD;

    } else if ((Fumbling || Glib) && !rn2(5)) {
		pline_The("nunchaku slips out of your %s.", body_part(HAND));
		dropx(obj);

    } else if (mtmp) {
		if (!canspotmon(mtmp) &&
			!glyph_is_invisible(levl[rx][ry].glyph)) {
		   pline("A monster is there that you couldn't see.");
		   map_invisible(rx, ry);
		}
		otmp = rn2(3) ? MON_WEP(mtmp) : MON_SWEP(mtmp);	/* can be null */
		if (otmp) {
			char onambuf[BUFSZ];
			const char *mon_hand;
			boolean gotit = proficient && (!Fumbling || !rn2(10));

			Strcpy(onambuf, cxname(otmp));
			if (gotit) {
				mon_hand = mbodypart(mtmp, HAND);
				if (bimanual(otmp,mtmp->data)) mon_hand = makeplural(mon_hand);
			} else
			mon_hand = 0;	/* lint suppression */

			You("wrap your nunchaku-chain around %s %s.",
			s_suffix(mon_nam(mtmp)), onambuf);
			if (gotit && otmp->cursed && !is_weldproof_mon(mtmp)) {
				pline("%s welded to %s %s%c",
					  (otmp->quan == 1L) ? "It is" : "They are",
					  mhis(mtmp), mon_hand,
					  !otmp->bknown ? '!' : '.');
				otmp->bknown = 1;
				gotit = FALSE;	/* can't pull it free */
			}
			if (gotit) {
				obj_extract_self(otmp);
				possibly_unwield(mtmp, FALSE);
				setmnotwielded(mtmp,otmp);

				switch (rn2(proficient + 1)) {
					case 2:
						/* to floor near you */
						You("yank %s %s to the %s!", s_suffix(mon_nam(mtmp)),
						onambuf, surface(u.ux, u.uy));
						place_object(otmp, u.ux, u.uy);
						stackobj(otmp);
						break;
					case 3:
						/* right to you */
						/* right into your inventory */
						You("snatch %s %s!", s_suffix(mon_nam(mtmp)), onambuf);
						if (otmp->otyp == CORPSE &&
							touch_petrifies(&mons[otmp->corpsenm]) &&
							!uarmg && !Stone_resistance &&
							!(poly_when_stoned(youracedata) &&
							polymon(PM_STONE_GOLEM))
						){
							char kbuf[BUFSZ];

							Sprintf(kbuf, "%s corpse",
								an(mons[otmp->corpsenm].mname));
							pline("Snatching %s is a fatal mistake.", kbuf);
							instapetrify(kbuf);
						}
						otmp = hold_another_object(otmp, "You drop %s!",
									   doname(otmp), (const char *)0);
					break;
				default:
					/* to floor beneath mon */
					You("yank %s from %s %s!", the(onambuf),
					s_suffix(mon_nam(mtmp)), mon_hand);
					obj_no_longer_held(otmp);
					place_object(otmp, mtmp->mx, mtmp->my);
					stackobj(otmp);
					break;
				}
			} else {
				pline("%s", msg_slipsfree);
			}
			wakeup(mtmp, TRUE);
		} else {
			res |= MOVE_ATTACKED;
			if ((mtmp->m_ap_type && mtmp->m_ap_type != M_AP_MONSTER) &&
			!Protection_from_shape_changers && !sensemon(mtmp))
			stumble_onto_mimic(mtmp);
			else You("swing your nunchaku toward %s.", mon_nam(mtmp));
			if (proficient) {
				if (attack2(mtmp)) return res;
				else pline("%s", msg_snap);
			}
		}

    } else if (Weightless || Is_waterlevel(&u.uz)) {
	    /* it must be air -- water checked above */
	    You("swing your nunchaku through thin air.");

    } else {
		pline("%s", msg_snap);

    }
    return res;
}


//Used to coordinate polearm_menu and targeting code
const int pole_dy[16] = {-2,-2,-2,-1, 0, 1, 2, 2, 2, 2, 2, 1, 0,-1,-2,-2};
const int pole_dx[16] = { 0, 1, 2, 2, 2, 2, 2, 1, 0,-1,-2,-2,-2,-2,-2,-1};
const char pole_dir[16][4] = {"N","NNE","NE",
						  "ENE","E","ESE","SE",
						  "SSE","S","SSW","SW",
						  "WSW","W","WNW","NW",
						  "NNW"};
#define N_POLEDIRS 16

STATIC_OVL int
polearm_menu(pole)
struct obj *pole;
{
	winid tmpwin;
	int n = 0, how, i, cx, cy;
	int typ, max_range;
	struct monst *mtmp;
	char buf[BUFSZ];
	char incntlet;
	menu_item *selected;
	anything any;

	/* Calculate range */
	typ = weapon_type(pole);
	int skill = P_SKILL(typ);
	if(pole->otyp == POLEAXE)
		skill--;
	if (typ == P_NONE || skill <= P_BASIC) max_range = 4;
	else if ( skill == P_SKILLED) max_range = 5;
	else max_range = 8;
	
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	
	Sprintf(buf, "Targets");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	incntlet = 'a';
	for(i=0; i<16; i++){
		cx = u.ux + pole_dx[i];
		cy = u.uy + pole_dy[i];
		if(isok(cx, cy) && (mtmp = m_at(cx, cy)) 
			&& canseemon(mtmp)
			&& couldsee(cx, cy)
			&& distu(cx, cy) <= max_range
			&& !(mtmp->mappearance && mtmp->m_ap_type != M_AP_MONSTER && !sensemon(mtmp))
			&& !(flags.peacesafe_polearms && mtmp->mpeaceful && !Hallucination)
			&& !(flags.petsafe_polearms && mtmp->mtame && !Hallucination)
		){
			Sprintf(buf, "%s (%s)", Monnam(mtmp), pole_dir[i]);
			any.a_int = i+1;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
			incntlet++;
		}
		else if(!flags.relative_polearms)
			incntlet++; /* always increment, so that the same direction is always the same letter*/
	}

	/* add an option to target manually */
	Sprintf(buf, "(some location)");
	any.a_int = N_POLEDIRS+1;
	add_menu(tmpwin, NO_GLYPH, &any,
		'z', 0, ATR_NONE, buf,
		MENU_UNSELECTED);

	end_menu(tmpwin, "Choose target:");

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

static const char
	not_enough_room[] = "There's not enough room here to use that.",
	where_to_hit[] = "Where do you want to hit?",
	cant_see_spot[] = "won't hit anything if you can't see that spot.",
	cant_reach[] = "can't reach that spot from here.";

/*Note: If input is invalid or cancelled, may return MOVE_CANCELLED OR MOVE_STANDARD
	* MOVE_STANDARD: The PC wielded the polearm before failing to give input.
	* MOVE_CANCELLED: The PC either failed to wield the pole, or was already wielding it before failing to give input.
	* If input succeeds, MOVE_STANDARD is returned.
	* 
	* If input fails, cc should be set to 0,0 and *outptr should be 0.
	* The caller should therefor always check isok() before using the coordinates.
	*/

STATIC_OVL int
pick_polearm_target(obj,outptr,ccp)
struct obj *obj;
struct monst **outptr;
coord *ccp;
{
	int res = MOVE_CANCELLED, typ, max_range = 4, min_range = 4;
	int i;
	struct monst *mtmp = (struct monst *) 0;
	*outptr = (struct monst *) 0;

	ccp->x = 0; ccp->y = 0;
	/* Are you allowed to use the pole? */
	if (u.uswallow) {
	    pline("%s", not_enough_room);
	    return MOVE_CANCELLED;
	}
	if (obj != uwep && obj != uarmg) {
	    if (!wield_tool(obj, "swing")) return MOVE_CANCELLED;
	    else res = MOVE_STANDARD;
	}
	if(Straitjacketed){
		You("can't swing a polearm while your %s are bound!", makeplural(body_part(ARM)));
		return MOVE_CANCELLED;
	}
     /* assert(obj == uwep); */

	/* Prompt for a location */
	if(flags.standard_polearms){
		pline("%s", where_to_hit);
		ccp->x = u.ux;
		ccp->y = u.uy;
		if (getpos(ccp, TRUE, "the spot to hit") < 0){
			ccp->x = 0; ccp->y = 0;
			return res;	/* user pressed ESC */
		}
	}
	else {
		if((i = polearm_menu(uwep))){
			i--; /*Remove the off-by-one offset used to make all returns from polearm_menu non-zero*/
			if (i<N_POLEDIRS) {
				ccp->x = u.ux + pole_dx[i];
				ccp->y = u.uy + pole_dy[i];
			}
			else {
				/* use standard targeting; save retval to return */
				flags.standard_polearms = TRUE;
				int retval = pick_polearm_target(obj,outptr,ccp);
				flags.standard_polearms = FALSE;
				if(res == MOVE_STANDARD || retval == MOVE_STANDARD)
					return MOVE_STANDARD;
				return res | retval;
			}
		}
		else {
			ccp->x = 0; ccp->y = 0;
			return res;	/* user pressed ESC */
		}
	}

	/* Calculate range */
	typ = uwep_skill_type();
	int skill = P_SKILL(typ);
	if(obj->otyp == POLEAXE)
		skill--;
	if (typ == P_NONE || skill <= P_BASIC) max_range = 4;
	else if ( skill == P_SKILLED) max_range = 5;
	else max_range = 8;
	mtmp = m_at(ccp->x, ccp->y);
	if (distu(ccp->x, ccp->y) > max_range) {
	    pline("Too far!");
		ccp->x = 0; ccp->y = 0;
	    return (res);
	} else if (distu(ccp->x, ccp->y) < min_range) {
	    pline("Too close!");
		ccp->x = 0; ccp->y = 0;
	    return (res);
	} else if (!cansee(ccp->x, ccp->y) &&
		   (mtmp == (struct monst *)0 ||
		    !canseemon(mtmp))) {
	    You(cant_see_spot);
		ccp->x = 0; ccp->y = 0;
	    return (res);
	} else if (!couldsee(ccp->x, ccp->y)) { /* Eyes of the Overworld */
	    You(cant_reach);
		ccp->x = 0; ccp->y = 0;
	    return res;
	}

	*outptr = mtmp;
	return MOVE_STANDARD;
}

/* Distance attacks by pole-weapons */
STATIC_OVL int
use_pole(obj)
	struct obj *obj;
{
	coord cc = {0};
	struct monst *mtmp;
	
	int res = pick_polearm_target(obj, &mtmp, &cc);
	if(!isok(cc.x, cc.y))
		return res;

	/* Attack the monster there */
	if (mtmp) {
	    bhitpos = cc;
	    check_caitiff(mtmp);
		if (u_pole_pound(mtmp)) {
			u.uconduct.weaphit++;
		}
	} else if(levl[cc.x][cc.y].typ == GRASS){
		   levl[cc.x][cc.y].typ = SOIL;
		   if(!rn2(3)) mksobj_at(SHEAF_OF_HAY,cc.x,cc.y,NO_MKOBJ_FLAGS);
		   You("cut away the grass!");
		   newsym(cc.x,cc.y);
	} else {
	    /* Now you know that nothing is there... */
	    pline("%s", nothing_happens);
	}
	return MOVE_ATTACKED;
}

STATIC_OVL int
use_cream_pie(obj)
struct obj *obj;
{
	boolean wasblind = Blind;
	boolean wascreamed = u.ucreamed;
	boolean several = FALSE;

	if (obj->quan > 1L) {
		several = TRUE;
		obj = splitobj(obj, 1L);
	}
	if (Hallucination)
		You("give yourself a facial.");
	else
		pline("You immerse your %s in %s%s.", body_part(FACE),
			several ? "one of " : "",
			several ? makeplural(the(xname(obj))) : the(xname(obj)));
	if(can_blnd((struct monst*)0, &youmonst, AT_WEAP, obj)) {
		int blindinc = rnd(25);
		u.ucreamed += blindinc;
		make_blinded(Blinded + (long)blindinc, FALSE);
		if (!Blind || (Blind && wasblind))
			pline("There's %ssticky goop all over your %s.",
				wascreamed ? "more " : "",
				body_part(FACE));
		else /* Blind  && !wasblind */
			You_cant("see through all the sticky goop on your %s.",
				body_part(FACE));
	}
	if (obj->unpaid) {
		verbalize("You used it, you bought it!");
		bill_dummy_object(obj);
	}
	obj_extract_self(obj);
	delobj(obj);
	return MOVE_INSTANT;
}

STATIC_OVL int
use_grapple (obj)
	struct obj *obj;
{
	int res = MOVE_CANCELLED, typ, max_range = 4, tohit;
	coord cc = {0};
	struct monst *mtmp;
	struct obj *otmp;

	/* Are you allowed to use the hook? */
	if (u.uswallow) {
	    pline("%s", not_enough_room);
	    return MOVE_CANCELLED;
	}
	if (obj != uwep) {
	    if (!wield_tool(obj, "cast")) return MOVE_CANCELLED;
	    else res = MOVE_STANDARD;
	}
	if(Straitjacketed){
		You("can't cast a grappling hook while your %s are bound!", makeplural(body_part(ARM)));
		return MOVE_CANCELLED;
	}
     /* assert(obj == uwep); */

	/* Prompt for a location */
	pline("%s", where_to_hit);
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, TRUE, "the spot to hit") < 0)
	    return res;	/* user pressed ESC */

	/* Calculate range */
	typ = uwep_skill_type();
	if (typ == P_NONE || P_SKILL(typ) <= P_BASIC) max_range = 4;
	else if (P_SKILL(typ) == P_SKILLED) max_range = 5;
	else max_range = 8;
	if (distu(cc.x, cc.y) > max_range) {
	    pline("Too far!");
	    return (res);
	} else if (!cansee(cc.x, cc.y)) {
	    You(cant_see_spot);
	    return (res);
	} else if (!couldsee(cc.x, cc.y)) { /* Eyes of the Overworld */
	    You(cant_reach);
	    return res;
	}

	/* What do you want to hit? */
	tohit = rn2(5);
	if (typ != P_NONE && P_SKILL(typ) >= P_SKILLED) {
	    winid tmpwin = create_nhwindow(NHW_MENU);
	    anything any;
	    char buf[BUFSZ];
	    menu_item *selected;

	    any.a_void = 0;	/* set all bits to zero */
	    any.a_int = 1;	/* use index+1 (cant use 0) as identifier */
	    start_menu(tmpwin);
	    any.a_int++;
	    Sprintf(buf, "an object on the %s", surface(cc.x, cc.y));
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 buf, MENU_UNSELECTED);
	    any.a_int++;
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			"a monster", MENU_UNSELECTED);
	    any.a_int++;
	    Sprintf(buf, "the %s", surface(cc.x, cc.y));
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 buf, MENU_UNSELECTED);
	    end_menu(tmpwin, "Aim for what?");
	    tohit = rn2(4);
	    if (select_menu(tmpwin, PICK_ONE, &selected) > 0 &&
			rn2(P_SKILL(typ) > P_SKILLED ? 20 : 2)
		)
			tohit = selected[0].item.a_int - 1;
	    free((genericptr_t)selected);
	    destroy_nhwindow(tmpwin);
	}

	/* What did you hit? */
	switch (tohit) {
	case 0:	/* Trap */
	    /* FIXME -- untrap needs to deal with non-adjacent traps */
	    break;
	case 1:	/* Object */
	    if ((otmp = level.objects[cc.x][cc.y]) != 0) {
		You("snag an object from the %s!", surface(cc.x, cc.y));
		(void) pickup_object(otmp, 1L, FALSE);
		/* If pickup fails, leave it alone */
		newsym(cc.x, cc.y);
		return MOVE_STANDARD;
	    }
	break;
	case 2:	/* Monster */
	    if ((mtmp = m_at(cc.x, cc.y)) == (struct monst *)0) break;
	    if (verysmall(mtmp->data) && !rn2(4) &&
			enexto(&cc, u.ux, u.uy, (struct permonst *)0)) {
		You("pull in %s!", mon_nam(mtmp));
		mtmp->mundetected = 0;
		rloc_to(mtmp, cc.x, cc.y);
		return MOVE_STANDARD;
		}
		else if ((!bigmonst(mtmp->data) && !strongmonst(mtmp->data)) || rn2(4)) {
			u_pole_pound(mtmp);
			return MOVE_ATTACKED;
		}
	    /* FALL THROUGH */
	case 3:	/* Surface */
	    if (IS_AIR(levl[cc.x][cc.y].typ) || is_pool(cc.x, cc.y, TRUE))
		pline_The("hook slices through the %s.", surface(cc.x, cc.y));
	    else {
		You("are yanked toward the %s!", surface(cc.x, cc.y));
		hurtle(sgn(cc.x-u.ux), sgn(cc.y-u.uy), 1, FALSE, TRUE);
		spoteffects(TRUE);
	    }
	    return MOVE_STANDARD;
	default:	/* Yourself (oops!) */
	    if (P_SKILL(typ) <= P_BASIC) {
		You("hook yourself!");
		losehp(rn1(10,10), "a grappling hook", KILLED_BY);
		return MOVE_STANDARD;
	    }
	    break;
	}
	pline("%s", nothing_happens);
	return MOVE_STANDARD;
}

STATIC_OVL int
use_crook (obj)
	struct obj *obj;
{
	int res = MOVE_CANCELLED, typ, max_range = 4, tohit;
	coord cc = {0};
	struct monst *mtmp;
	struct obj *otmp;

	/* Are you allowed to use the crook? */
	if (u.uswallow) {
	    pline("%s", not_enough_room);
	    return MOVE_CANCELLED;
	}
	if (obj != uwep) {
	    if (!wield_tool(obj, "hook")) return MOVE_CANCELLED;
	    else res = MOVE_STANDARD;
	}
	if(Straitjacketed){
		You("can't use a shepherd's crook while your %s are bound!", makeplural(body_part(ARM)));
		return MOVE_CANCELLED;
	}
     /* assert(obj == uwep); */

	/* What do you want to hit? */
	{
	    winid tmpwin = create_nhwindow(NHW_MENU);
	    anything any;
	    char buf[BUFSZ];
	    menu_item *selected;

	    any.a_void = 0;	/* set all bits to zero */
	    any.a_int = 1;	/* use index+1 (cant use 0) as identifier */
	    start_menu(tmpwin);
			any.a_int++;
			add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
				"hit a monster", MENU_UNSELECTED);
			
			any.a_int++;
			add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
				"pull a monster", MENU_UNSELECTED);
			
			any.a_int++;
			add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
				 "pull an object", MENU_UNSELECTED);
	    end_menu(tmpwin, "Aim for what?");
	    if (select_menu(tmpwin, PICK_ONE, &selected) > 0){
			tohit = selected[0].item.a_int - 1;
		}
		else {
			free((genericptr_t)selected);
			destroy_nhwindow(tmpwin);
			return MOVE_CANCELLED;
		}
	    free((genericptr_t)selected);
	    destroy_nhwindow(tmpwin);
	}

	typ = uwep_skill_type();
	
	/* What did you hit? */
	switch (tohit) {
	case 0:	/* Trap */
	    /* FIXME -- untrap needs to deal with non-adjacent traps */
	    break;
	case 1:	/*Hit Monster */
		res |= use_pole(obj);
		if(res & MOVE_STANDARD)
			return MOVE_STANDARD;
		return res;
	break;
	case 2:	/*Hook Monster */
		res |= pick_polearm_target(obj, &mtmp, &cc);
		if(!mtmp){
			if(res & MOVE_STANDARD)
				return MOVE_STANDARD;
			return res;
		}
		if(mtmp->mpeaceful){
			if (!bigmonst(mtmp->data) &&
				enexto(&cc, u.ux, u.uy, (struct permonst *)0)
			) {
				You("pull in %s!", mon_nam(mtmp));
				mtmp->mundetected = 0;
				mtmp->movement = max(0, mtmp->movement - 12);
				rloc_to(mtmp, cc.x, cc.y);
				return MOVE_STANDARD;
			} else {
				You("pull yourself toward %s!", mon_nam(mtmp));
				hurtle(sgn(mtmp->mx-u.ux), sgn(mtmp->my-u.uy), 1, FALSE, TRUE);
				spoteffects(TRUE);
				return MOVE_STANDARD;
			}
		} else {
			if (!bigmonst(mtmp->data) && !strongmonst(mtmp->data) && rn2(P_SKILL(typ)) &&
				enexto(&cc, u.ux, u.uy, (struct permonst *)0)
			) {
				You("pull in %s!", mon_nam(mtmp));
				mtmp->mundetected = 0;
				mtmp->movement = max(0, mtmp->movement - 12);
				rloc_to(mtmp, cc.x, cc.y);
				return MOVE_STANDARD;
			} else if ((!bigmonst(mtmp->data) && !strongmonst(mtmp->data)) ||
				   rn2(P_SKILL(typ))
			) {
				u_pole_pound(mtmp);
				return MOVE_ATTACKED;
			} else {
				You("are yanked toward %s!", mon_nam(mtmp));
				hurtle(sgn(mtmp->mx-u.ux), sgn(mtmp->my-u.uy), 1, FALSE, TRUE);
				spoteffects(TRUE);
				return MOVE_STANDARD;
			}
		}
	break;
	case 3:	/* Object */
		{
			int tmp = flags.standard_polearms;
			int subres;
			flags.standard_polearms = 1;
			subres = pick_polearm_target(obj, &mtmp, &cc);
			flags.standard_polearms = tmp;
			//Note: May have wielded polearm.
			if (!isok(cc.x, cc.y))
				return res;
		}
	    if ((otmp = level.objects[cc.x][cc.y]) != 0) {
			You("snag an object from the %s!", surface(cc.x, cc.y));
			(void) pickup_object(otmp, 1L, FALSE);
			/* If pickup fails, leave it alone */
			newsym(cc.x, cc.y);
			return MOVE_STANDARD;
	    }
	    break;
	// case 3:	/* Surface */
	    // if (IS_AIR(levl[cc.x][cc.y].typ) || is_pool(cc.x, cc.y, TRUE))
			// pline_The("hook slices through the %s.", surface(cc.x, cc.y));
	    // else {
			// You("are yanked toward the %s!", surface(cc.x, cc.y));
			// hurtle(sgn(cc.x-u.ux), sgn(cc.y-u.uy), 1, FALSE, TRUE);
			// spoteffects(TRUE);
	    // }
	    // return MOVE_STANDARD;
	// break;
	default:
		pline("Invalid target for crook-hook");
	break;
	}
	pline("%s", nothing_happens);
	return MOVE_STANDARD;
}

STATIC_OVL int
use_rift(obj)
	struct obj *obj;
{
	struct monst *mtmp = 0;
	if(!freehand()){
		You("can't break %s with no free %s!", xname(obj), body_part(HAND));
		return MOVE_CANCELLED;
	}
	if(yn("Shatter the gem?") == 'y'){
		if(!Deadmagic && !Blind){
			pline("The black flaw tears free, then expands to fill the world!");
			if(base_casting_stat() == A_INT)
				pline("The Weave frays and fails!");
		}
		//Don't bother doing math if the timeout is already infinite.
		// It's not EASY to permanently break magic with these, but it is POSSIBLE!
		if(Deadmagic < TIMEOUT_INF){
			long increment = (long)(50 * pow(1.1,u.rift_count));
			incr_itimeout(&Deadmagic, increment);
			// Need to be careful with counts, since exponential math means that we can easily start overflowing stuff.
			if(increment < TIMEOUT){
				u.rift_count++;
			}
		}
		useup(obj);
		return MOVE_STANDARD;
	}
	return MOVE_CANCELLED;
}

STATIC_OVL int
use_vortex(obj)
	struct obj *obj;
{
	struct monst *mtmp = 0;
	if(!freehand()){
		You("can't break %s with no free %s!", xname(obj), body_part(HAND));
		return MOVE_CANCELLED;
	}
	if(yn("Shatter the gem?") == 'y'){
		if(!Catapsi && !Blind){
			pline("The silvery-gray flaw spins free, then expands to fill the world!");
			if(base_casting_stat() == A_CHA)
				pline("Your mind fills with static!");
		}
		//Don't bother doing math if the timeout is already infinite.
		// It's not EASY to permanently break magic with these, but it is POSSIBLE!
		if(Catapsi < TIMEOUT_INF){
			long increment = (long)(100 * pow(1.1,u.vortex_count));
			incr_itimeout(&Catapsi, increment);
			// Need to be careful with counts, since exponential math means that we can easily start overflowing stuff.
			if(increment < TIMEOUT){
				u.vortex_count++;
			}
		}
		useup(obj);
		return MOVE_STANDARD;
	}
	return MOVE_CANCELLED;
}
void
pyramid_effects(obj,x,y)
struct obj *obj;
int x, y;
{
	//Don't bother doing math if the timeout is already infinite.
	// It's not EASY to permanently break magic with these, but it is POSSIBLE!
	if(Misotheism < TIMEOUT_INF){
		long increment = (long)(33 * pow(1.1,u.miso_count));
		if(!Inhell && !Misotheism && u.ualign.type != A_VOID){
			godlist[u.ualign.god].anger++;
			gods_angry(u.ualign.god);
		}
		// Need to be careful with counts, since exponential math means that we can easily start overflowing stuff.
		if(increment < TIMEOUT){
			u.miso_count++;
		}
		if(obj->otyp == MISOTHEISTIC_PYRAMID){
			if(!Blind && cansee(x, y)){
				if(!DarksightOnly)
					pline("An impossible darkness pours forth!");
				else
					pline("A mighty darkness issues forth!");
			}
			incr_itimeout(&HDarksight, increment);
			incr_itimeout(&DarksightOnly, increment);
			doredraw();
		}
		if(obj->otyp == MISOTHEISTIC_FRAGMENT){
			if(!Shattering)
				You_hear("a great fracturing sound!");
			if(!Blind && cansee(x, y)){
				if(!(u.specialSealsKnown&SEAL_NUDZIRATH)){
					/*Note: this is intended to work for any PC, not just Binders */
					if(!(u.specialSealsKnown&SEAL_NUDZIRATH)){
						pline("The shattered fragments form part of a seal.");
						pline("In fact, you realize that all cracked and broken mirrors everywhere together are working towards writing this seal.");
						pline("With that realization comes knowledge of the seal's final form!");
						u.specialSealsKnown |= SEAL_NUDZIRATH;
					}
				}
			}
			incr_itimeout(&Shattering, increment);
		}
		if(!Misotheism && base_casting_stat() == A_WIS)
			pline("You can no longer hear your god!");
		incr_itimeout(&Misotheism, increment);
	}
}

STATIC_OVL int
use_pyramid(obj)
struct obj *obj;
{
	struct monst *mtmp = 0;
	if(!freehand()){
		You("can't %s %s with no free %s!", obj->otyp == MISOTHEISTIC_PYRAMID ? "open" : "shatter",xname(obj), body_part(HAND));
		return MOVE_CANCELLED;
	}
	if(yn(obj->otyp == MISOTHEISTIC_PYRAMID ? "Open the pyramid?" : "Further shatter the pyramid?") == 'y'){
		pyramid_effects(obj, u.ux, u.uy);
		useup(obj);
		return MOVE_STANDARD;
	}
	return MOVE_CANCELLED;
}

STATIC_OVL int
use_dimensional_lock(obj)
struct obj *obj;
{
	struct monst *mtmp = 0;
	if(!Blind)
		pline("The cerulean tree flashes and disapears.");
	pline("The disk crumbles to dust!");
	incr_itimeout(&DimensionalLock, 100L);
	expel_summons();
	useup(obj);
	return MOVE_STANDARD;
}

STATIC_OVL int
use_dilithium(obj)
	struct obj *obj;
{
	struct monst *mtmp = 0;
	struct obj *dollobj = 0;
	dollobj = getobj(chain_class, "install dilithium in");
	if(!dollobj)
		return MOVE_CANCELLED;

	if(dollobj->otyp != BROKEN_ANDROID && dollobj->otyp != BROKEN_GYNOID){
		pline("That's not a droid.");
		return MOVE_CANCELLED;
	}

	mtmp = revive(dollobj, TRUE);

	if(!mtmp){
		pline("Nothing happens....");
		return MOVE_CANCELLED;
	}
	
	mtmp->mhp = max(1, mtmp->m_lev);
	pline("%s comes back online!", Monnam(mtmp));
	
	useup(obj);
	return MOVE_STANDARD;
}

STATIC_OVL int
use_doll_tear(obj)
	struct obj *obj;
{
	struct monst *mtmp = 0;
	if(yn("Give doll's tear to a monster?") == 'y'){
		getdir("Which monster?");
		if(u.dx || u.dy){
			if(u.uswallow)
				mtmp = u.ustuck;
			else if(!isok(u.ux + u.dx, u.uy + u.dy)) return MOVE_CANCELLED;
			else mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
			
			if(!mtmp)
				return MOVE_CANCELLED;
			
			if(!is_dollable(mtmp->data)){
				pline("That's not a doll.");
				return MOVE_CANCELLED;
			}
			
			if(mtmp->m_insight_level){
				pline("Nothing happens....");
				return MOVE_CANCELLED;
			}
			
			mtmp->mvar_dollTypes = obj->ovar1_dollTypes;
			mtmp->m_insight_level = obj->spe;
			useup(obj);
			return MOVE_STANDARD;
		}
		return MOVE_CANCELLED;
	} else {
		struct obj *dollobj = 0;
		dollobj = getobj(chain_class, "give the tear to");
		if(!dollobj)
			return MOVE_CANCELLED;

		if(dollobj->otyp != BROKEN_ANDROID && dollobj->otyp != BROKEN_GYNOID && dollobj->otyp != LIFELESS_DOLL){
			pline("That's not a doll.");
			return MOVE_CANCELLED;
		}
		
		if(get_ox(dollobj, OX_EMON))
			mtmp = get_mtraits(dollobj, FALSE);
		else {
			pline("Nothing happens....");
			return MOVE_CANCELLED;
		}
		
		//I don't think this is possible given the above, but I'm feeling paranoid....
		if(!mtmp){
			pline("Nothing happens....");
			return MOVE_CANCELLED;
		}
		
		dollobj->ovar1_insightlevel = (long)obj->spe;
		mtmp->m_insight_level = (long)obj->spe;
		mtmp->mvar_dollTypes = obj->ovar1_dollTypes;
		useup(obj);
		return MOVE_STANDARD;
	}
	return MOVE_CANCELLED;
}

STATIC_OVL int
use_doll(obj)
	struct obj *obj;
{
	int res = MOVE_CANCELLED;
	struct monst *mtmp;
	switch(obj->otyp){
		case DOLL_OF_JUMPING:
			if (jump(4)){
				res = MOVE_STANDARD;
				give_intrinsic(JUMPING, 100L);
				if(!Blind)
					pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
				else pline("The little doll vanishes.");
				useup(obj);
			}
		break;
		case DOLL_OF_FRIENDSHIP:
			getdir((char *)0);
			if(u.dx || u.dy){
				if(u.uswallow)
					mtmp = u.ustuck;
				else if (!isok(u.ux + u.dx, u.uy + u.dy)) break;
				else mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
				pline("The doll sings sweetly.");
				if(mtmp && resist_song(mtmp, SNG_TAME, obj) >= 0){
					if (mtmp->mtame){
						if(!get_mx(mtmp, MX_EDOG) || (EDOG(mtmp)->friend))
							break;
						if(mtmp->mtame < 16) mtmp->mtame++;
					} else {
						xchar waspeaceful = mtmp->mpeaceful;
						mtmp = tamedog(mtmp, obj);
						if(mtmp){
							if (canseemon(mtmp) && flags.verbose && !mtmp->msleeping)
								pline("%s seems to like the doll's song.", Monnam(mtmp));
							mtmp->mtame = 9;
							EDOG(mtmp)->waspeaceful = TRUE;
							if(!waspeaceful || mtmp->mpeacetime){ /*Should it become untame, remain tame peaceful for a short period of time*/
								mtmp->mpeacetime = max(mtmp->mpeacetime, 90);
							}
						}
					}
					res = MOVE_STANDARD;
					if(!Blind)
						pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
					else pline("The little doll vanishes.");
					useup(obj);
				}
			}
		break;
		case DOLL_OF_CHASTITY:
			res = MOVE_STANDARD;
			pline("You feel chaste.");
			give_intrinsic(CHASTITY, 100L);
			if(!Blind)
				pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
			else pline("The little doll vanishes.");
			useup(obj);
		break;
		case DOLL_OF_CLEAVING:
			res = MOVE_STANDARD;
			if(!Blind)
				pline("The doll swings its %s in wide arcs.", rn2(2) ? "greatsword" : "greataxe");
			give_intrinsic(CLEAVING, 100L);
			if(!Blind)
				pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
			else pline("The little doll vanishes.");
			useup(obj);
		break;
		case DOLL_OF_SATIATION:
			if(satiate_uhunger()){
				res = MOVE_STANDARD;
				if(!Blind)
					pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
				else pline("The little doll vanishes.");
				useup(obj);
			}
		break;
		case DOLL_OF_GOOD_HEALTH:
			if(Slimed){
				Slimed = 0L;
				flags.botl = 1;
			}
			healup(0, 0, TRUE, FALSE);
			if (Stoned) fix_petrification();
			if (Golded) fix_petrification();
			res = MOVE_STANDARD;
			pline("You feel very healthy.");
			give_intrinsic(GOOD_HEALTH, 100L);
			if(!Blind)
				pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
			else pline("The little doll vanishes.");
			useup(obj);
		break;
		case DOLL_OF_FULL_HEALING:
			res = MOVE_STANDARD;
			if((!Upolyd && u.uhp < u.uhpmax) ||
				(Upolyd && u.mh < u.mhmax)
			)
				pline("Your wounds begin rapidly knitting shut.");
			give_intrinsic(RAPID_HEALING, 5L);
			if(!Blind)
				pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
			else pline("The little doll vanishes.");
			useup(obj);
		break;
		case DOLL_OF_DESTRUCTION:
			res = MOVE_STANDARD;
			if(!Blind)
				pline("The many-armed doll begins dancing!");
			give_intrinsic(DESTRUCTION, 64L);
			if(!Blind)
				pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
			else pline("The little doll vanishes.");
			useup(obj);
		break;
		case DOLL_OF_MEMORY:
			if(doreinforce_spell()){
				res = MOVE_STANDARD;
				if(!Blind)
					pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
				else pline("The little doll vanishes.");
				useup(obj);
			}
		break;
		case DOLL_OF_BINDING:
			if(doreinforce_binding()){
				res = MOVE_STANDARD;
				if(!Blind)
					pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
				else pline("The little doll vanishes.");
				useup(obj);
			}
		break;
		case DOLL_OF_PRESERVATION:
			res = MOVE_STANDARD;
			if(!Blind)
				pline("The doll opens its umbrella, and a rubbery film forms around your body!");
			give_intrinsic(PRESERVATION, 1000L);
			if(!Blind)
				pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
			else pline("The little doll vanishes.");
			useup(obj);
		break;
		case DOLL_OF_QUICK_DRAWING:
			res = MOVE_STANDARD;
			if(!Blind)
				pline("The doll draws a wand with blinding speed!");
			give_intrinsic(QUICK_DRAW, 100L);
			if(!Blind)
				pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
			else pline("The little doll vanishes.");
			useup(obj);
		break;
		case DOLL_OF_WAND_CHARGING:
			if(dowand_refresh()){
				res = MOVE_STANDARD;
				if(!Blind)
					pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
				else pline("The little doll vanishes.");
				useup(obj);
			}
		break;
		case DOLL_OF_STEALING:
			getdir((char *)0);
			if(u.dx || u.dy){
				if(u.uswallow)
					mtmp = u.ustuck;
				else if (!isok(u.ux + u.dx, u.uy + u.dy)) break;
				else mtmp = m_at(u.ux + u.dx, u.uy + u.dy);
				if(mtmp){
					struct obj *otmp;
					long unwornmask;
					//Note: unlike normal theft, you are never petrified by a stolen item because the doll is doing it.
					if(!Blind) pline("The black-clad doll steals %s possessions.", s_suffix(mon_nam(mtmp)));
					while ((otmp = mtmp->minvent) != 0) {
						/* take the object away from the monster */
						obj_extract_self(otmp);
						if ((unwornmask = otmp->owornmask) != 0L) {
							mtmp->misc_worn_check &= ~unwornmask;
							if (otmp->owornmask & W_WEP) {
								setmnotwielded(mtmp,otmp);
								MON_NOWEP(mtmp);
							}
							if (otmp->owornmask & W_SWAPWEP){
								setmnotwielded(mtmp,otmp);
								MON_NOSWEP(mtmp);
							}
							otmp->owornmask = 0L;
						}
						update_mon_intrinsics(mtmp, otmp, FALSE, FALSE);
						if(!Blind) pline("The doll steals %s %s and drops it to the %s.",
							  s_suffix(mon_nam(mtmp)), xname(otmp), surface(u.ux, u.uy));
						dropy(otmp);
						/* more take-away handling, after theft message */
						if (unwornmask & W_WEP || unwornmask & W_SWAPWEP) {		/* stole wielded weapon */
							possibly_unwield(mtmp, FALSE);
						} else if (unwornmask & W_ARMG) {	/* stole worn gloves */
							mselftouch(mtmp, (const char *)0, TRUE);
							if (mtmp->mhp <= 0)	/* it's now a statue */
								break;		/* can't continue stealing */
						}
					}
#ifndef GOLDOBJ
					if (mtmp->mgold){
						struct obj *mongold = mksobj(GOLD_PIECE, MKOBJ_NOINIT);
						mongold->quan = mtmp->mgold;
						mongold->owt = weight(mongold);
						mtmp->mgold = 0;
						if(!Blind) pline("The doll steals %s gold and drops it to the %s.", s_suffix(mon_nam(mtmp)), surface(u.ux, u.uy));
						dropy(mongold);
					}
#else
					{
						struct obj *mongold = findgold(mtmp->minvent);
						if (mongold) {
							obj_extract_self(mongold);
							if(!Blind) pline("The doll steals %s gold and drops it to the %s.", s_suffix(mon_nam(mtmp)), surface(u.ux, u.uy));
							dropy(mongold);
						}
					}
#endif
					res = MOVE_STANDARD;
					if(!Blind)
						pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
					else pline("The little doll vanishes.");
					useup(obj);
					setmangry(mtmp);
				}
			}
		break;
		case DOLL_OF_MOLLIFICATION:
			if(godlist[u.ualign.god].anger) {
				if(!Blind)
					pline("The %s says a prayer.", OBJ_DESCR(objects[obj->otyp]));
				pline("%s seems %s.", u_gname(),
				  Hallucination ? "groovy" : "mollified");
				godlist[u.ualign.god].anger = 0;
				if ((int)u.uluck < 0) u.uluck = 0;
				u.reconciled = REC_MOL;
				res = MOVE_STANDARD;
				if(!Blind)
					pline("The little doll vanishes in a flash of moonlight.");
				else pline("The little doll vanishes.");
				useup(obj);
			}
		break;
		case DOLL_OF_CLEAR_THINKING:
			res = MOVE_STANDARD;
			pline("The doll takes up your mental burdens!");
			give_intrinsic(CLEAR_THOUGHTS, 100L);
			if(!Blind)
				pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
			else pline("The little doll vanishes.");
			useup(obj);
		break;
		case DOLL_OF_MIND_BLASTING:
			res = MOVE_STANDARD;
			give_intrinsic(MIND_BLASTS, 8L);
			if(!Blind)
				pline("The %s vanishes in a flash of moonlight.", OBJ_DESCR(objects[obj->otyp]));
			else pline("The little doll vanishes.");
			useup(obj);
		break;
		default:
		break;
	}
	return res;
}

boolean
use_ring_of_wishes(obj)
struct obj *obj;
{
	struct monst * mtmp;
	struct monst * mtmp2 = (struct monst*)0;
	struct monst * mtmp3 = (struct monst*)0;
	boolean madewish = FALSE;

	if (obj->otyp != RIN_WISHES)
	{
		impossible("object other than ring of wishes passed to use_ring_of_wishes");
		return FALSE;
	}
	if (obj->cursed || (Luck + rn2(5) < 0)){	// to be less cruel, it doesn't use up a charge
		pline1(nothing_happens);
		return FALSE;
	}
	if (!(obj->owornmask & W_RING)) {
		if (objects[RIN_WISHES].oc_name_known)
			You_feel("that you should be wearing %s.", the(xname(obj)));
		else
			pline1(nothing_happens);
		return FALSE;
	}

	if (obj->spe > 0)
	{
		if (!(mtmp = makemon(&mons[PM_DJINNI], u.ux, u.uy, NO_MM_FLAGS))){
			pline1(nothing_happens);
		}
		else
		{
			if (!DimensionalLock) {
				if ((u.uevent.utook_castle & ARTWISH_EARNED) && !(u.uevent.utook_castle & ARTWISH_SPENT))
					mtmp2 = makemon(&mons[PM_PSYCHOPOMP], u.ux, u.uy, NO_MM_FLAGS);
				if ((u.uevent.uunknowngod & ARTWISH_EARNED) && !(u.uevent.uunknowngod & ARTWISH_SPENT))
					mtmp3 = makemon(&mons[PM_PRIEST_OF_AN_UNKNOWN_GOD], u.ux, u.uy, NO_MM_FLAGS);
			}

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
			verbalize("I am the djinni of the ring.  I will grant one wish!");
			int artwishes = u.uconduct.wisharti;
			if (makewish(WISH_VERBOSE | (DimensionalLock ? 0 :allow_artwish()))) {
				obj->spe--;
				madewish = TRUE;
			}
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

			if (!objects[RIN_WISHES].oc_name_known) {
				makeknown(RIN_WISHES);
				more_experienced(0, 10);
			}
		}
	}
	else
	{
		pline1(nothing_happens);
	}
	return madewish;
}

#define SUMMON_DJINNI		1
#define SUMMON_SERVANT		2
#define SUMMON_DEMON_LORD	3

int
do_candle_menu()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Summon Djinni");
	any.a_int = SUMMON_DJINNI;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet + 1) : 'A';

	Sprintf(buf, "Summon Servant");
	any.a_int = SUMMON_SERVANT;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet + 1) : 'A';

	Sprintf(buf, "Summon Demon Lord");
	any.a_int = SUMMON_DEMON_LORD;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet + 1) : 'A';

	end_menu(tmpwin, "What creature do you wish to summon?");

	how = PICK_ONE;
	do{
		n = select_menu(tmpwin, how, &selected);
	} while (n <= 0);
	destroy_nhwindow(tmpwin);
	if(n > 0){
		int picked = selected[0].item.a_int;
		free(selected);
		return picked;
	}
	return 0;
}

int
do_demon_lord_summon_menu()
{
	winid tmpwin;
	int n, how, i;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	for (i = 0; i >= LOW_PM && i<SPECIAL_PM; i++)
	{
		if (is_dlord(&mons[i]) && type_is_pname(&mons[i]) && !(mvitals[i].mvflags & G_EXTINCT))
		{
			Sprintf(buf, "%s", mons[i].mname);
			any.a_int = i;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
			incntlet = (incntlet != 'z') ? (incntlet + 1) : 'A';
		}
	}

	end_menu(tmpwin, "Which demon do you wish to summon?");

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

boolean
use_candle_of_invocation(obj)	// incomplete
struct obj *obj;
{
	int choice = 0;
	boolean consumed = FALSE;
	struct monst * mtmp = (struct monst *)0;

	if (obj->otyp != CANDLE_OF_INVOCATION)
	{
		impossible("object other than candle of invocation passed to use_candle_of_invocation");
		return FALSE;
	}
	if (!obj->lamplit || DimensionalLock) {
		pline1(nothing_happens);
		return FALSE;
	}

	pline("The %s flares, and a planar gate opens!", xname(obj));
	do{
		choice = do_candle_menu();

		switch (choice)
		{
		case SUMMON_DJINNI:
			if (!(mtmp = makemon(&mons[PM_DJINNI], u.ux, u.uy, NO_MM_FLAGS))){
				pline("Nothing responds to the call.");
				consumed = FALSE;
				break;
			}
			else {
				struct monst *mtmp2 = (struct monst*)0;
				struct monst *mtmp3 = (struct monst*)0;
				if ((u.uevent.utook_castle & ARTWISH_EARNED) && !(u.uevent.utook_castle & ARTWISH_SPENT))
					mtmp2 = makemon(&mons[PM_PSYCHOPOMP], u.ux, u.uy, NO_MM_FLAGS);
				if ((u.uevent.uunknowngod & ARTWISH_EARNED) && !(u.uevent.uunknowngod & ARTWISH_SPENT))
					mtmp3 = makemon(&mons[PM_PRIEST_OF_AN_UNKNOWN_GOD], u.ux, u.uy, NO_MM_FLAGS);

				if (!Blind) {
					pline("%s passes through the gate in a cloud of smoke!", Amonnam(mtmp));
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
				consumed = TRUE;
			}
			break;
		case SUMMON_SERVANT:
			mtmp = create_particular(u.ux, u.uy, MT_DOMESTIC, 0, FALSE, MA_MINION | MA_DEMON | MA_FEY | MA_PRIMORDIAL, MG_NOWISH | MG_NOTAME, G_UNIQ, (char *)0);
			if (!mtmp) {
				pline("Perhaps try summoning something else?");
				consumed = FALSE;
			}
			else {
				pline("The gate closes as %s passes through.", a_monnam(mtmp));
				consumed = TRUE;
			}
			break;
		case SUMMON_DEMON_LORD:
			choice = do_demon_lord_summon_menu();
			if (!choice){
				// the player didn't choose an option
				consumed = FALSE;
				break;
			}
			if (!(mtmp = makemon(&mons[choice], u.ux, u.uy, NO_MM_FLAGS))){
				// the demon was already generated
				pline("Nothing responds to the call.");
				consumed = FALSE;
				break;
			}
			else {
				if (!Blind) {
					pline("%s passes through the gate.", Monnam(mtmp));
				}
				else {
					You_feel("a hostile presence.");
				}
				mtmp->mpeaceful = 0;
				consumed = TRUE;
			}
			break;
		default:
			consumed = FALSE;
			break;
		}
	} while (!consumed);
	pline("The %s is consumed.", xname(obj));
	useupall(obj);
	if (!objects[CANDLE_OF_INVOCATION].oc_name_known) {
		makeknown(CANDLE_OF_INVOCATION);
		more_experienced(0, 10);
	}
	return TRUE;
}

#undef SUMMON_DJINNI
#undef SUMMON_SERVANT
#undef SUMMON_DEMON_LORD

#define BY_OBJECT	((struct monst *)0)

STATIC_OVL int
do_break_wand(obj)
    struct obj *obj;
{
    static const char nothing_else_happens[] = "But nothing else happens...";
    register int i, x, y;
    register struct monst *mon;
    int dmg, damage;
    boolean affects_objects, is_fragile;
    boolean shop_damage = FALSE;
    int expltype = EXPL_MAGICAL;
    char confirm[QBUFSZ], the_wand[BUFSZ], buf[BUFSZ];

    Strcpy(the_wand, yname(obj));
    Sprintf(confirm, "Are you really sure you want to break %s?",
	safe_qbuf("", sizeof("Are you really sure you want to break ?"),
				the_wand, ysimple_name(obj), "the wand"));
    if (yn(confirm) == 'n' ) return MOVE_CANCELLED;

    is_fragile = (!strcmp(OBJ_DESCR(objects[obj->otyp]), "balsa"));

    if (nolimbs(youracedata)) {
	You_cant("break %s without limbs!", the_wand);
	return MOVE_CANCELLED;
    } else if (obj->oartifact || ACURR(A_STR) < (is_fragile ? 5 : 10)) {
	You("don't have the strength to break %s!", the_wand);
	return MOVE_CANCELLED;
    }
    pline("Raising %s high above your %s, you %s it in two!",
	  the_wand, body_part(HEAD), is_fragile ? "snap" : "break");

    /* [ALI] Do this first so that wand is removed from bill. Otherwise,
     * the freeinv() below also hides it from setpaid() which causes problems.
     */
    if (obj->unpaid) {
	check_unpaid(obj);		/* Extra charge for use */
	bill_dummy_object(obj);
    }
	
	if(u.sealsActive&SEAL_ASTAROTH) unbind(SEAL_ASTAROTH, TRUE);

    current_wand = obj;		/* destroy_item might reset this */
    freeinv(obj);		/* hide it from destroy_item instead... */
    setnotworn(obj);		/* so we need to do this ourselves */

    if (obj->spe <= 0) {
	pline("%s", nothing_else_happens);
	goto discard_broken_wand;
    }
    obj->ox = u.ux;
    obj->oy = u.uy;
    dmg = obj->spe * 4;
    affects_objects = FALSE;

    switch (obj->otyp) {
    case WAN_NOTHING:
	pline("Suddenly, and without warning, nothing happens.");
	goto discard_broken_wand;
    case WAN_WISHING:
    case WAN_LOCKING:
    case WAN_PROBING:
    case WAN_ENLIGHTENMENT:
    case WAN_SECRET_DOOR_DETECTION:
	pline("%s", nothing_else_happens);
	goto discard_broken_wand;
    case WAN_OPENING:
	/* make trap door if you broke a wand of opening */
	if ((obj->spe > 2) && rn2(obj->spe - 2) && !u.uswallow &&
	    !On_stairs(u.ux, u.uy) && (!IS_FURNITURE(levl[u.ux][u.uy].typ) &&
	    !IS_ROCK(levl[u.ux][u.uy].typ) && !closed_door(u.ux, u.uy) &&
	    !t_at(u.ux, u.uy) && Can_dig_down(&u.uz))) {
		struct trap *ttmp;
		ttmp = maketrap(u.ux, u.uy, TRAPDOOR);
		    if (ttmp) {
			ttmp->madeby_u = 1;
			ttmp->tseen = 1;
			newsym(u.ux, u.uy);
			    if (*in_rooms(u.ux,u.uy,SHOPBASE)) {
				add_damage(u.ux, u.uy, 0L);             
			    }
		    }
	}
	goto discard_broken_wand;
    case WAN_DEATH:
    case WAN_LIGHTNING:
	dmg *= 4;
	goto wanexpl;
    case WAN_FIRE:
	expltype = EXPL_FIERY;
    case WAN_COLD:
	if (expltype == EXPL_MAGICAL) expltype = EXPL_FROSTY;
	dmg *= 2;
    case WAN_MAGIC_MISSILE:
    wanexpl:
	explode(u.ux, u.uy,
		wand_adtype(obj->otyp), WAND_CLASS, dmg, expltype, 1);
	makeknown(obj->otyp);	/* explode described the effect */
	/* make magic trap if you broke a wand of magic missile */
	if ((obj->spe > 2) && rn2(obj->spe - 2) && !u.uswallow &&
	    !On_stairs(u.ux, u.uy) && (!IS_FURNITURE(levl[u.ux][u.uy].typ) &&
	    !IS_ROCK(levl[u.ux][u.uy].typ) &&
	    !closed_door(u.ux, u.uy) && !t_at(u.ux, u.uy))) {
		struct trap *ttmp;
		ttmp = maketrap(u.ux, u.uy, MAGIC_TRAP);
		    if (ttmp) {
			ttmp->madeby_u = 1;
			ttmp->tseen = 1;
			newsym(u.ux, u.uy);
			    if (*in_rooms(u.ux,u.uy,SHOPBASE)) {
				add_damage(u.ux, u.uy, 0L);             
			    }
		    }
	}
	goto discard_broken_wand;
    case WAN_STRIKING:
	/* we want this before the explosion instead of at the very end */
	pline("A wall of force smashes down around you!");
	dmg = d(1 + obj->spe,6);	/* normally 2d12 */
	break;
    case WAN_TELEPORTATION:
		/* WAC make tele trap if you broke a wand of teleport */
		/* But make sure the spot is valid! */
	    if ((obj->spe > 2) && rn2(obj->spe - 2) && !notel_level() &&
		    !u.uswallow && !On_stairs(u.ux, u.uy) && (!IS_FURNITURE(levl[u.ux][u.uy].typ) &&
		    !IS_ROCK(levl[u.ux][u.uy].typ) &&
		    !closed_door(u.ux, u.uy) && !t_at(u.ux, u.uy))) {
			struct trap *ttmp;
			ttmp = maketrap(u.ux, u.uy, TELEP_TRAP);
			if (ttmp) {
				ttmp->madeby_u = 1;
				ttmp->tseen = 1;
				newsym(u.ux, u.uy); /* if our hero happens to be invisible */
				if (*in_rooms(u.ux,u.uy,SHOPBASE)) {
					/* shopkeeper will remove it */
					add_damage(u.ux, u.uy, 0L);             
				}
			}
		}
	affects_objects = TRUE;
	break;
	case WAN_POLYMORPH:
	/* make poly trap if you broke a wand of polymorph */
	if ((obj->spe > 2) && rn2(obj->spe - 2) && !u.uswallow &&
	    !On_stairs(u.ux, u.uy) && (!IS_FURNITURE(levl[u.ux][u.uy].typ) &&
	    !IS_ROCK(levl[u.ux][u.uy].typ) &&
	    !closed_door(u.ux, u.uy) && !t_at(u.ux, u.uy))) {
		struct trap *ttmp;
		ttmp = maketrap(u.ux, u.uy, POLY_TRAP);
		    if (ttmp) {
			ttmp->madeby_u = 1;
			ttmp->tseen = 1;
			newsym(u.ux, u.uy);
			    if (*in_rooms(u.ux,u.uy,SHOPBASE)) {
				add_damage(u.ux, u.uy, 0L);             
			    }
		    }
	}
	affects_objects = TRUE;
	break;
	case WAN_SLEEP:
	/* make sleeping gas trap if you broke a wand of sleep */
	if ((obj->spe > 2) && rn2(obj->spe - 2) && !u.uswallow &&
	    !On_stairs(u.ux, u.uy) && (!IS_FURNITURE(levl[u.ux][u.uy].typ) &&
	    !IS_ROCK(levl[u.ux][u.uy].typ) &&
	    !closed_door(u.ux, u.uy) && !t_at(u.ux, u.uy))) {
		struct trap *ttmp;
		ttmp = maketrap(u.ux, u.uy, SLP_GAS_TRAP);
		    if (ttmp) {
			ttmp->madeby_u = 1;
			ttmp->tseen = 1;
			newsym(u.ux, u.uy);
			    if (*in_rooms(u.ux,u.uy,SHOPBASE)) {
				add_damage(u.ux, u.uy, 0L);             
			    }
		    }
	}
	break;
    case WAN_CANCELLATION:
	/* make anti-magic trap if you broke a wand of cancellation */
	if ((obj->spe > 2) && rn2(obj->spe - 2) && !u.uswallow &&
	    !On_stairs(u.ux, u.uy) && (!IS_FURNITURE(levl[u.ux][u.uy].typ) &&
	    !IS_ROCK(levl[u.ux][u.uy].typ) &&
	    !closed_door(u.ux, u.uy) && !t_at(u.ux, u.uy))) {
		struct trap *ttmp;
		ttmp = maketrap(u.ux, u.uy, ANTI_MAGIC);
		    if (ttmp) {
			ttmp->madeby_u = 1;
			ttmp->tseen = 1;
			newsym(u.ux, u.uy);
			    if (*in_rooms(u.ux,u.uy,SHOPBASE)) {
				add_damage(u.ux, u.uy, 0L);             
			    }
		    }
	}
	affects_objects = TRUE;
	break;
    case WAN_UNDEAD_TURNING:
	affects_objects = TRUE;
	break;
    default:
	break;
    }

    /* magical explosion and its visual effect occur before specific effects */
    explode(obj->ox, obj->oy, AD_MAGM, WAND_CLASS, rnd(dmg), EXPL_MAGICAL, 1);

    /* this makes it hit us last, so that we can see the action first */
    for (i = 0; i <= 8; i++) {
	bhitpos.x = x = obj->ox + xdir[i];
	bhitpos.y = y = obj->oy + ydir[i];
	if (!isok(x,y)) continue;

	if (obj->otyp == WAN_DIGGING) {
	    if(dig_check(BY_OBJECT, FALSE, x, y)) {
		if (IS_WALL(levl[x][y].typ) || IS_DOOR(levl[x][y].typ)) {
		    /* normally, pits and holes don't anger guards, but they
		     * do if it's a wall or door that's being dug */
		    watch_dig((struct monst *)0, x, y, TRUE);
		    if (*in_rooms(x,y,SHOPBASE)) shop_damage = TRUE;
		}
		if(IS_GRAVE(levl[x][y].typ)){
			digactualhole(x, y, BY_OBJECT, PIT, FALSE, TRUE);
			dig_up_grave(x,y);
		} else if(IS_SEAL(levl[x][y].typ)){
			// digactualhole(x, y, BY_OBJECT, PIT, FALSE, TRUE);
			break_seal(x,y);
		} else{
			digactualhole(x, y, BY_OBJECT,
					  (rn2(obj->spe) < 3 || !Can_dig_down(&u.uz)) ?
					   PIT : HOLE, FALSE, TRUE);
			}
		}
	    continue;
	} else if(obj->otyp == WAN_CREATE_MONSTER) {
	    /* u.ux,u.uy creates it near you--x,y might create it in rock */
		if(!DimensionalLock)
			(void) makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS);
	    continue;
	} else {
	    if (x == u.ux && y == u.uy) {
		/* teleport objects first to avoid race with tele control and
		   autopickup.  Other wand/object effects handled after
		   possible wand damage is assessed */
		if (obj->otyp == WAN_TELEPORTATION &&
		    affects_objects && level.objects[x][y]) {
		    (void) bhitpile(obj, bhito, x, y);
		    if (flags.botl) bot();		/* potion effects */
		}
		damage = zapyourself(obj, FALSE);
		if (damage) {
		    Sprintf(buf, "killed %sself by breaking a wand", uhim());
		    losehp(damage, buf, NO_KILLER_PREFIX);
		}
		if (flags.botl) bot();		/* blindness */
	    } else if ((mon = m_at(x, y)) != 0) {
		(void) bhitm(mon, obj);
	     /* if (flags.botl) bot(); */
	    }
	    if (affects_objects && level.objects[x][y]) {
		(void) bhitpile(obj, bhito, x, y);
		if (flags.botl) bot();		/* potion effects */
	    }
	}
    }

    /* Note: if player fell thru, this call is a no-op.
       Damage is handled in digactualhole in that case */
    if (shop_damage) pay_for_damage("dig into", FALSE);

    if (obj->otyp == WAN_LIGHT){
		litroom(TRUE, obj);	/* only needs to be done once */
		if(u.sealsActive&SEAL_TENEBROUS) unbind(SEAL_TENEBROUS,TRUE);
	}
    else if (obj->otyp == WAN_DARKNESS){
		litroom(FALSE, obj);	/* only needs to be done once */
	}

 discard_broken_wand:
    obj = current_wand;		/* [see dozap() and destroy_item()] */
    current_wand = 0;
    if (obj)
	delobj(obj);
    nomul(0, NULL);
    return MOVE_STANDARD;
}

STATIC_OVL boolean
uhave_graystone()
{
	register struct obj *otmp;

	for(otmp = invent; otmp; otmp = otmp->nobj)
		if(is_graystone(otmp))
			return TRUE;
	return FALSE;
}

STATIC_OVL int
do_flip_coin(obj)
struct obj *obj;
{
#ifndef GOLDOBJ
    u.ugold += obj->quan;
    dealloc_obj(obj);
#endif

    if (nohands(youracedata)) {
	pline("And how would you flip the coin without hands?");
	return MOVE_CANCELLED;
    } else if (!freehand()) {
	You("need at least one free %s.", body_part(HAND));
	return MOVE_CANCELLED;
    } else if (Underwater) {
	pline("This coin wasn't designed to be flipped underwater.");
	return MOVE_CANCELLED;
    }

    You("flip %s coin.",
#ifndef GOLDOBJ
	(u.ugold > 1)
#else
	(obj->quan > 1)
#endif
	? "a" : "the");

    if (!Fumbling && !Glib && !Blind &&
	((ACURR(A_DEX) + Luck) > 0) && rn2((ACURR(A_DEX) + Luck))) {
	xchar ht = rn2(2);
	pline("%s.", ht ? "Heads" : "Tails");
	if (Hallucination && ht && !rn2(8))
	    pline("Oh my, it %s at you!", rn2(2) ? "grins" : "winks");
    } else {
	struct obj *gold;
	You("try to catch the coin but it slips from your %s.",
	    makeplural(body_part(HAND)));
#ifndef GOLDOBJ
	gold = mkgoldobj(1);
#else
	if (obj->quan > 1) gold = splitobj(obj, 1L);
	else gold = obj;
#endif
	dropx(gold);
    }
    return MOVE_STANDARD;
}

STATIC_DCL void
soul_crush_consequence(obj)
struct obj * obj;
{
	if (obj->blessed) {
		You("crush the %s in your %s.", obj->dknown ? OBJ_DESCR(objects[obj->otyp]) : "coin", body_part(HAND));
		You("release the soul!");
		u.ublesscnt = 0;
		if(rn2(100)){
			pline("...you feel the soul sink towards Gehennom.");
			/* Points for trying */
			u.ualign.sins = max(u.ualign.sins-1, 0);
			adjalign(7);
			if(godlist[u.ualign.god].anger) {
				godlist[u.ualign.god].anger--;
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
			}
		}
		else {
			/* Freed someone who wasn't supposed to be there! */
			u.ualign.sins = max(u.ualign.sins-7, 0);
			u.ualign.record = ALIGNLIM;
			if(godlist[u.ualign.god].anger) {
				godlist[u.ualign.god].anger = 0;
				pline("%s seems %s.", u_gname(), Hallucination ?
				"cosmic (not a new fact)" : "mollified");
				if ((int)u.uluck < 0) u.uluck = 0;
				u.reconciled = REC_MOL;
			}
		}
	}
	else if (!obj->cursed) {
		u.ualign.sins++;
		if(!rn2(100)){
			You("feel the soul fly free!");
			adjalign(1);
			if ((int)u.uluck < 0) change_luck(1);
		}
	}
	else {
		u.ualign.sins += 9;
		adjalign(-99);
		if(!rn2(100)){
			You("feel the soul scream as it sinks towards Gehennom under the weight of the curse!");
			/* Just sent someone back who shouldn't be there :( */
			gods_upset(u.ualign.god);
		}
	}
}

STATIC_OVL int
do_soul_coin(obj)
struct obj *obj;
{
	char coinbuff[50] = {0};
	struct monst *mtmp = (struct monst *)0;
	struct obj *otmp;
	int x, y;
	int tmp;
	coord cc = {u.ux, u.uy};

	if(!freehand()){
		You("can't crush %s with no free %s!", the(xname(obj)), body_part(HAND));
		return MOVE_CANCELLED;
	}

	/* most wages need a target, or else they abort.
	 * Blessed wages all have the same non-targeted effect */
	if (!obj->blessed &&
		obj->otyp != WAGE_OF_SLOTH &&
		obj->otyp != WAGE_OF_LUST
		){
		boolean needs_mon = (obj->otyp != WAGE_OF_GREED);
		boolean needs_space = (obj->otyp == WAGE_OF_GREED);
		/* choose target */
		if (needs_mon)
			pline("Pick a creature to target.");
		else
			pline("Pick a location to target.");
		if(getpos(&cc, TRUE, "the target") < 0) return MOVE_CANCELLED;
		x = cc.x;
		y = cc.y;
		if(distmin(u.ux, u.uy, x, y) > BOLT_LIM || !clear_path(u.ux, u.uy, x, y)){
			pline("It can't reach there!");
			return MOVE_CANCELLED;
		}

		if(needs_space && (closed_door(x, y) ||	IS_ROCK(levl[x][y].typ))){
			You("can't use that there.");
			return MOVE_CANCELLED;
		}
		mtmp = m_u_at(x, y);
		if(needs_mon && (!mtmp || DEADMONSTER(mtmp))){
			You("see no target there!");
			return MOVE_CANCELLED;
		}
		if(mtmp == &youmonst){
			Sprintf(coinbuff, "Use the %s on yourself?", obj->dknown ? OBJ_DESCR(objects[obj->otyp]) : "coin");
			if(yn(coinbuff) == 'n')
				return MOVE_CANCELLED;
		}
		else if(needs_mon && !canspotmon(mtmp)){
			You("see no target there!");
			return MOVE_CANCELLED;
		}
	}
	/* did not abort during targeting; activate */
	You("crush the %s in your %s.", obj->dknown ? OBJ_DESCR(objects[obj->otyp]) : "coin", body_part(HAND));

	/* main effect -- does not happen with blessed wages. */
	if (!obj->blessed) {
		switch(obj->otyp){
			case WAGE_OF_SLOTH:
				tmp = obj->cursed ? 4 : 2;
				You("blur with stolen time!");
				HTimeStop += (long)tmp;
				for(mtmp = fmon; mtmp; mtmp = mtmp->nmon){
					if(is_uvuudaum(mtmp->data) && !DEADMONSTER(mtmp)){
						mtmp->movement += NORMAL_SPEED*tmp;
					}
				}
			break;
			case WAGE_OF_LUST:
				tmp = obj->cursed ? 22 : 9;

				if(!BlowingWinds){
					pline("Hurricane-force winds surround you!");
				}
				else {
					pline("The hurricane-force winds intensify!");
				}
				HBlowingWinds += (long)tmp;
			break;
			case WAGE_OF_GLUTTONY:
				if(inediate(mtmp->data)){
					pline("%s %s unbothered.",
						(mtmp == &youmonst) ? "You" : Monnam(mtmp),
						(mtmp == &youmonst) ? "are" : "is");
				}
				else {
					tmp = obj->cursed ? 2000 : 1000;
					if(mtmp == &youmonst){
						morehungry(tmp*get_uhungersizemod());
					}
					else {
						if(mtmp->mtame && get_mx(mtmp, MX_EDOG)){
							EDOG(mtmp)->hungrytime = max(EDOG(mtmp)->hungrytime - tmp, monstermoves - (500 + 250*obj->cursed));
						}
						else {
							if(obj->cursed){
								mtmp->mhp -= min(333, mtmp->mhpmax);
							}
							else {
								mtmp->mhp -= max(33, min(99, mtmp->mhpmax/2));
							}
							if(mtmp->mhp <= 0){
								pline("%s starves.", Monnam(mtmp));
								mondied(mtmp);
							}
							else {
								pline("%s is confused from hunger.", Monnam(mtmp));
								mtmp->mconf = 1;
							}
						}
					}
				}
			break;
			case WAGE_OF_GREED:
				otmp = mksobj(MASS_OF_STUFF, MKOBJ_NOINIT);
				if (!otmp) break;  /* Shouldn't happen */
				curse(otmp);

				/* Note, blessed was handled above. */
				if(obj->cursed){
					projectile(&youmonst, otmp, (void *)0, HMON_PROJECTILE|HMON_FIRED, u.ux, u.uy, (x-u.ux), (y-u.uy), 0, 1, FALSE, FALSE, FALSE);
				}
				else if(mtmp){
					int dmg;
					boolean youdef = mtmp == &youmonst;
					if(youdef)
						pline("%s drops out of nowhere and hits you!", An(xname(otmp)));
					else if (cansee(mtmp->mx, mtmp->my)) {
						pline("%s is hit by %s!", Monnam(mtmp),
										doname(otmp));
						if (mtmp->minvis && !canspotmon(mtmp))
						map_invisible(mtmp->mx, mtmp->my);
					}

					dmg = dmgval(otmp, mtmp, 0, &youmonst);
					struct obj *helmet = youdef ? uarmh : which_armor(mtmp, W_ARMH);
					if (helmet) {
						if(is_hard(helmet)) {
							if(youdef)
								pline("Fortunately, you are wearing a hard helmet.");
							else if (canspotmon(mtmp))
								pline("Fortunately, %s is wearing a hard helmet.", mon_nam(mtmp));
							else if (flags.soundok)
								You_hear("a clanging sound.");
							if (dmg > 2) dmg = 2;
						}
						else if (flags.verbose) {
							if(youdef)
								Your("%s does not protect you.",
									xname(helmet));
							else if(canspotmon(mtmp))
								pline("%s's %s does not protect %s.",
								Monnam(mtmp), xname(helmet),
								mhim(mtmp));
						}
					}
					if (!flooreffects(otmp, x, y, "fall")) {
						place_object(otmp, x, y);
						stackobj(otmp);
						newsym(x, y);
					}

					dmg = reduce_dmg(mtmp,dmg,TRUE,FALSE);
					if(youdef)
						losehp(dmg, "mass of falling stuff", KILLED_BY_AN);
					else {
						mtmp->mhp -= dmg;
						if (mtmp->mhp <= 0)
							xkilled(mtmp, 1);
					}
				}
				else {
					if (!flooreffects(otmp, x, y, "fall")) {
						place_object(otmp, x, y);
						stackobj(otmp);
						newsym(x, y);  /* map the rock */
					}
				}
			break;
			case WAGE_OF_WRATH:
				if(Breathless_res(mtmp)){
					pline("%s %s unbothered.",
						(mtmp == &youmonst) ? "You" : Monnam(mtmp),
						(mtmp == &youmonst) ? "are" : "is");
					water_damage(mtmp == &youmonst ? invent : mtmp->minvent, FALSE, FALSE, WD_BLOOD, mtmp);
				}
				else {
					if(mtmp == &youmonst){
						BloodDrown += obj->cursed ? 9 : 4;
					}
					else {
						water_damage(mtmp->minvent, FALSE, FALSE, WD_BLOOD, mtmp);
						if(obj->cursed){
							//Note: 2x water damage
							water_damage(mtmp->minvent, FALSE, FALSE, WD_BLOOD, mtmp);
							mtmp->mbdrown = min(100, mtmp->mbdrown+9);
						}
						mtmp->mhp -= min(99, obj->cursed ? mtmp->mhpmax : mtmp->mhpmax/2);
						if(mtmp->mhp <= 0){
							pline("%s drowns in blood!", Monnam(mtmp));
							mondied(mtmp);
						}
						else if(!resist(mtmp, RING_CLASS, 0, NOTELL)){
							pline("%s is crazed with anger!", Monnam(mtmp));
							mtmp->mberserk = 1;
						}
					}
				}
			break;
			case WAGE_OF_ENVY:
				if(mtmp != &youmonst && mindless_mon(mtmp)) { /* player is never mindless */
					pline("%s is unbothered.", Monnam(mtmp));
				}
				if(mtmp == &youmonst){
					if(!roll_madness(MAD_TALONS)){ /*May refuse to give up things*/
						struct obj *otmp2;
						int delay = 0;
						if (*u.ushops) sellobj_state(SELL_DELIBERATE);
						for(otmp = invent; otmp; otmp = otmp2) {
							if(otmp == obj) {otmp2 = otmp->nobj; continue;}
							if(obj->cursed && otmp->owornmask){
								if(otmp->oclass == ARMOR_CLASS)
									delay = max(delay, objects[otmp->otyp].oc_delay);
								remove_worn_item(otmp, TRUE);
							}
							otmp2 = otmp->nobj;
							drop(otmp);
						}
						if (*u.ushops) sellobj_state(SELL_NORMAL);
						reset_occupations();
						if(delay)
							nomul(-delay, "taking off clothes");
					}
				}
				else {
					relobj_envy(mtmp,canspotmon(mtmp));
					mtmp->menvy = TRUE;
					if(obj->cursed){
						mtmp->mdisrobe = TRUE;
					}
				}
			break;
			case WAGE_OF_PRIDE:
				if(mtmp != &youmonst && mindless_mon(mtmp)){ /* player is never mindless */
					pline("%s is unbothered.", Monnam(mtmp));
				}
				
				if(mtmp == &youmonst){
					u.uencouraged -= 9;
					if(obj->cursed)
						u.ustdy += 45;
				}
				else {
					mtmp->encouraged -= 9;
					if(obj->cursed){
						mtmp->mstdy += 45;
					}
					if(is_minion(mtmp->data) && !(mtmp->data->geno&G_UNIQ)
					&& (obj->cursed || !resist(mtmp, RING_CLASS, 0, NOTELL))
					){
						if(In_endgame(&u.uz)){
							if(canspotmon(mtmp)){
								pline("%s plummets through the %s!", Monnam(mtmp), surface(mtmp->mx, mtmp->my));
							}
							mongone(mtmp);
						}
						else if(!templated(mtmp)){
							if(canspotmon(mtmp)){
								pline("%s falls from grace!", Monnam(mtmp));
							}
							set_template(mtmp, FALLEN_TEMPLATE);
						}
					}
				}
			break;
		}
		/* if you targeted a monster, it gets angry, even if it was unaffected */
		if (mtmp) {
			wakeup(mtmp, TRUE);
		}
	}
	/* handle B/U/C effect of wages */
	soul_crush_consequence(obj);
	
	useup(obj);
    return MOVE_STANDARD;
}

int
use_vital(obj)
struct obj *obj;
{
	if(!freehand()){
		You("can't crush %s with no free %s!", xname(obj), body_part(HAND));
		return MOVE_CANCELLED;
	}
	if(yn("Crush the stone?") == 'y'){
		if(!obj->blessed){
			You_feel("full of vital energy!");
			healup((obj->cursed ? 900 : 400), 0, FALSE, FALSE);
		}
		soul_crush_consequence(obj);
		useup(obj);
		return MOVE_STANDARD;
	}
	return MOVE_CANCELLED;
}

int
use_spiritual(obj)
struct obj *obj;
{
	if(!freehand()){
		You("can't crush %s with no free %s!", xname(obj), body_part(HAND));
		return MOVE_CANCELLED;
	}
	if(yn("Crush the stone?") == 'y'){
		if(!obj->blessed) {
			You_feel("deeply spiritual!");
			u.uen = min(u.uenmax, u.uen + (obj->cursed ? 900 : 400));
		}
		soul_crush_consequence(obj);
		useup(obj);
		return MOVE_STANDARD;
	}
	return MOVE_CANCELLED;
}

void
salve_effect(otmp)
struct obj *otmp;
{
	int oerodedLevel = 3;//3, 2, 1
	int speLevel = -5; //-5, -1, +3
	
	for(int i = 0; i < 3; i++){
		if(otmp->oeroded >= oerodedLevel){
			otmp->oeroded--;
			return;
		}
		else if(otmp->oeroded2 >= oerodedLevel){
			otmp->oeroded2--;
			return;
		}
		else if(otmp->oeroded3 >= oerodedLevel){
			otmp->oeroded3--;
			return;
		}
		else if(is_enchantable(otmp) && otmp->spe <= speLevel){
			otmp->spe = min(3, otmp->spe+2);
			return;
		}
		oerodedLevel--;
		speLevel += 4;
	}
}

int
use_armor_salve(obj)
struct obj *obj;
{
	struct obj *otmp;
	if(!freehand()){
		You("can't use salve with no free %s!", body_part(HAND));
		return MOVE_CANCELLED;
	}
	if(obj->spe <= 0){
		pline("There's no salve left.");
		return MOVE_CANCELLED;
	}
	// Create an array with all classes explicitly listed in it, 1-MAXOCLASSES :(
	char all_classes[MAXOCLASSES] = {0};
	for(int i = 1; i < MAXOCLASSES; i++)
		all_classes[i-1] = i;
	otmp = getobj(all_classes, "salve");
	if(otmp){
		You("spread some salve on %s.", yname(otmp));
		salve_effect(otmp);
		obj->spe--;
		return MOVE_STANDARD;
	}
	return MOVE_CANCELLED;
}

STATIC_OVL int
res_engine_menu(obj)
struct obj *obj;
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;
	struct obj *otmp;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	
	// Sprintf(buf, "Do what?");
	// add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	if(obj->spe < 8){
		Sprintf(buf, "Replace component");
		any.a_int = 1;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(obj->altmode != ENG_MODE_OFF){
		Sprintf(buf, "Switch off");
		any.a_int = 2;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(obj->altmode != ENG_MODE_PYS){
		Sprintf(buf, "Switch to low intensity (physical only)");
		any.a_int = 3;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(obj->altmode != ENG_MODE_ENR){
		Sprintf(buf, "Switch to high intensity (physical and energy)");
		any.a_int = 4;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	end_menu(tmpwin, "Do what?");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if( n <= 0 )
		return FALSE;
	
	switch(selected[0].item.a_int){
		case 1:
			otmp = getobj(tools, "replace with");
			if(!otmp)
				return FALSE;
			else if(otmp->otyp != HELLFIRE_COMPONENT){
				pline("This device requires hellfire components.");
				return FALSE;
			}
			//else
			useup(otmp);
			You("put the new component into the engine.");
			obj->spe = 8;
		break;
		case 2:
			You("switch the engine off.");
			obj->altmode = ENG_MODE_OFF;
		break;
		case 3:
			You("switch the engine to low intensity.");
			obj->altmode = ENG_MODE_PYS;
		break;
		case 4:
			You("switch the engine to high intensity.");
			obj->altmode = ENG_MODE_ENR;
		break;
	}
	free(selected);
	return TRUE;
}

int
check_res_engine(mdef, adtyp)
struct monst *mdef;
int adtyp;
{
	boolean youdef = (mdef == &youmonst);
	struct obj *engine;
	boolean vis = youdef;
	for(engine = youdef ? invent : mdef->minvent; engine; engine = engine->nobj)
		if(engine->otyp == PRESERVATIVE_ENGINE && engine->spe > 0 && engine->altmode != ENG_MODE_OFF){
			if(adtyp == AD_VORP || adtyp == AD_SHRD || adtyp == AD_TENT || engine->altmode == ENG_MODE_ENR){
				if(vis) pline("%s infernal engine whirrs.", youdef ? "Your" : s_suffix(Monnam(mdef)));
				engine->spe--;
				return TRUE;
			}
		}
	return FALSE;
}

void
add_class(cl, class)
char *cl;
char class;
{
	char tmp[2];
	tmp[0] = class;
	tmp[1] = '\0';
	Strcat(cl, tmp);
}

const int carveTurns[6] = {3,4,2,1,5,2};

int
do_carve_obj(obj)
struct obj *obj;
{
	int rune;
	char carveelet;
	struct obj *carvee;
	struct obj *otmp;
	multi = 0;		/* moves consumed */
	nomovemsg = (char *)0;	/* occupation end message */

	rune = pick_rune(FALSE);
	if(!rune) return MOVE_CANCELLED;
	carveelet = pick_carvee();
	
	for (otmp = invent; otmp; otmp = otmp->nobj) {
		if(otmp->invlet == carveelet) break;
	}
	if(otmp) carvee = otmp;
	else return MOVE_CANCELLED;
	
	if(carvee == obj){
//		pline("Your grasp of physics is appalling.");
		pline("Is this a zen thing?");
		return MOVE_CANCELLED;
	}
	
	if(carvee->spe > obj->spe){
		pline("The %s is too dull to cut into the %s.", xname(obj), xname(carvee));
		return MOVE_CANCELLED;
	}
	if(carvee->oward != 0 ){
		You("chip off the existing rune.");
		multi-=1;
		if(carvee->oartifact) pline("The wood heals like the rune was never there.");
		else if(--carvee->spe < -1*rn2(8)) {
				You("destroyed the %s in the process.", xname(carvee));
				useup(carvee);
				return MOVE_CANCELLED;
		}
	}
	multi -= carveTurns[rune-FIRST_RUNE];
	nomovemsg = "You finish carving.";;
	carvee->oward = get_wardID(rune);
	You("carve a %s into the %s.",wardDecode[decode_wardID(carvee->oward)],xname(carvee));
    u.uconduct.wardless++;
	see_monsters(); //Some magic staves grant detection, so recheck that now.
	return MOVE_STANDARD;
}

int
pick_rune(describe)
boolean describe;
{
	if (!(u.wardsknown & (WARD_TOUSTEFNA | WARD_DREPRUN | WARD_OTTASTAFUR | WARD_KAUPALOKI | WARD_VEIOISTAFUR | WARD_THJOFASTAFUR))){
		You("can't think of anything to carve.");
		return 0;
	}

	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	
	Sprintf(buf, "Known Magical Staves");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	if(u.wardsknown & WARD_TOUSTEFNA){
		Sprintf(buf, "Toustefna stave");
		any.a_int = TOUSTEFNA;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(u.wardsknown & WARD_DREPRUN){
		Sprintf(buf, "Dreprun stave");
		any.a_int = DREPRUN;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(u.wardsknown & WARD_VEIOISTAFUR){
		Sprintf(buf, "Veioistafur stave");
		any.a_int = VEIOISTAFUR;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(u.wardsknown & WARD_THJOFASTAFUR){
		Sprintf(buf, "Thjofastafur stave");
		any.a_int = THJOFASTAFUR;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}

	if (!describe){
		// Describe a glyph
		Sprintf(buf, "Describe a glyph instead");
		any.a_int = -1;					/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'?', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	else {
		Sprintf(buf, "Carve a glyph instead");
		any.a_int = -1;					/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'!', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}

	end_menu(tmpwin, (describe) ? "Choose stave to describe:" : "Choose stave to carve:");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);

	if (n > 0 && selected[0].item.a_int == -1){
		free(selected);
		return pick_rune(!describe);
	}

	if (n > 0 && describe){
		describe_rune(selected[0].item.a_int);
		free(selected);
		return pick_rune(describe);
	}
	if (n > 0 && !describe){
		int picked = selected[0].item.a_int;
		free(selected);
		return picked;
	}

	return 0;
}


void
describe_rune(floorID)
int floorID;
{
	winid datawin;
	char name[80];
	char turns[80];
	char warded[80];
	char reinforce[80];
	char secondary[80];

	switch (floorID){
	case TOUSTEFNA:
		strcpy(name, " Toustefna stave");
		strcpy(turns, " 3 turns");
		strcpy(warded, " d, f");
		strcpy(secondary, " None.");
		break;
	case DREPRUN:
		strcpy(name, " Dreprun stave");
		strcpy(turns, " 4 turns");
		strcpy(warded, " q, u, bats, birds");
		strcpy(secondary, " None.");
		break;
	case VEIOISTAFUR:
		strcpy(name, " Veioistafur stave");
		strcpy(turns, " 5 turns");
		strcpy(warded, " ;");
		strcpy(secondary, " Bonus d20 damage vs ; when carved onto wielded weapon.");
		break;
	case THJOFASTAFUR:
		strcpy(name, " Thjofastafur stave");
		strcpy(turns, " 2 turns");
		strcpy(warded, " n, l");
		strcpy(secondary, " Grants detection of nymphs and leprechauns while wielded.");
		break;
	default:
		impossible("No such stave to carve: %d", floorID);
		return;
	}

	datawin = create_nhwindow(NHW_TEXT);
	putstr(datawin, 0, "");
	putstr(datawin, 0, name);
	putstr(datawin, 0, "");
	putstr(datawin, 0, " Turns to carve:");
	putstr(datawin, 0, turns);
	putstr(datawin, 0, "");
	putstr(datawin, 0, " Warded creatures:");
	putstr(datawin, 0, warded);
	putstr(datawin, 0, "");
	putstr(datawin, 0, " Secondary effects:");
	putstr(datawin, 0, secondary);
	putstr(datawin, 0, "");
	display_nhwindow(datawin, FALSE);
	destroy_nhwindow(datawin);
	return;
}

char
pick_carvee()
{
	winid tmpwin;
	int n=0, how,count=0;
	char buf[BUFSZ];
	struct obj *otmp;
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	
	Sprintf(buf, "Carvable items");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	for(otmp = invent; otmp; otmp = otmp->nobj){
		if(otmp->oclass == WEAPON_CLASS && otmp->obj_material == WOOD && otmp->otyp != MOON_AXE
				&& otmp->oartifact != ART_BOW_OF_SKADI && otmp->oartifact != ART_GUNGNIR
				&& otmp->oartifact != ART_STAFF_OF_AESCULAPIUS && otmp->oartifact != ART_ESSCOOAHLIPBOOURRR){
			Sprintf1(buf, doname(otmp));
			any.a_char = otmp->invlet;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				otmp->invlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
			count++;
		}
	}
	end_menu(tmpwin, "Choose ward:");

	how = PICK_ONE;
	if(count) n = select_menu(tmpwin, how, &selected);
	else You("don't have any carvable items.");
	destroy_nhwindow(tmpwin);
	if(n > 0){
		int picked = selected[0].item.a_int;
		free(selected);
		return picked;
	}
	return 0;
}

STATIC_OVL boolean
clockwork_location_checks(obj, cc, quietly)
struct obj *obj;
coord *cc;
boolean quietly;
{
	xchar x,y;

	x = cc->x; y = cc->y;
	if (!isok(x,y)) {
		if (!quietly)
			You("cannot build a clockwork there.");
		return FALSE;
	}
	if (IS_ROCK(levl[x][y].typ) &&
	    !(species_passes_walls(&mons[obj->corpsenm]) && may_passwall(x,y))) {
		if (!quietly)
		    You("cannot build a clockwork in %s!",
			IS_TREES(levl[x][y].typ) ? "a tree" : "solid rock");
		return FALSE;
	}
	if (boulder_at(x,y) && !species_passes_walls(&mons[obj->corpsenm])
			&& !throws_rocks(&mons[obj->corpsenm])) {
		if (!quietly)
			You("cannot fit a clockwork under the %s.",xname(boulder_at(x,y)));
		return FALSE;
	}
	if (m_at(x,y)) {
		if (!quietly)
			You("cannot fit a clockwork there!");
		return FALSE;
	}
	return TRUE;
}

STATIC_OVL struct permonst *
clockworkMenu(obj)
struct obj *obj;
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	
	Sprintf(buf, "Clockwork types");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	if(obj->otyp == CLOCKWORK_COMPONENT){
		Sprintf(buf, "clockwork soldier");
		any.a_int = PM_CLOCKWORK_SOLDIER;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
		
		Sprintf(buf, "clockwork dwarf");
		any.a_int = PM_CLOCKWORK_DWARF;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
		
		Sprintf(buf, "faberge sphere");
		any.a_int = PM_FABERGE_SPHERE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	} else if(obj->otyp == SUBETHAIC_COMPONENT){
		Sprintf(buf, "golden heart");
		any.a_int = PM_GOLDEN_HEART;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	} else if(obj->otyp == HELLFIRE_COMPONENT){
		Sprintf(buf, "hellfire orb");
		any.a_int = PM_HELLFIRE_ORB;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	end_menu(tmpwin, "Choose type:");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if(n > 0){
		struct permonst * picked = &mons[selected[0].item.a_int];
		free(selected);
		return picked;
	}
	return (struct permonst *) 0;
}

STATIC_OVL int
doUseComponents(optr)
struct obj **optr;
{
	struct obj *obj = *optr;
	struct monst *mm = (struct monst *)0;
	struct permonst *pmm;
	coord cc;

	if(!getdir((char *)0)) {
		return MOVE_CANCELLED;
	}
	if (u.uswallow && (u.dx || u.dy || u.dz)) {
		/* can't activate a figurine while swallowed */
		You("don't have enough room in here to build a clockwork.");
		return MOVE_CANCELLED;
	}

	if (!(u.dx || u.dy || u.dz))
		mm = &youmonst;
	else if (u.dz > 0 && u.usteed)
		mm = u.usteed;
	else if (isok(u.ux + u.dx, u.uy + u.dy))
		mm = m_at(u.ux + u.dx, u.uy + u.dy);
	
	if (mm) {
		/* check that the part matches the creature's needed type */
		boolean can_use_obj = FALSE;
		if (is_clockwork(mm->data)) {
			switch (mm->mtyp) {
			case PM_GOLDEN_HEART:
			case PM_ID_JUGGERNAUT:
				can_use_obj = (obj->otyp == SUBETHAIC_COMPONENT);
				break;
			case PM_HELLFIRE_ORB:
			case PM_HELLFIRE_COLOSSUS:
				can_use_obj = (obj->otyp == HELLFIRE_COMPONENT);
				break;
			case PM_SCRAP_TITAN:
				can_use_obj = (obj->otyp == CLOCKWORK_COMPONENT
							|| obj->otyp == SUBETHAIC_COMPONENT
							|| obj->otyp == HELLFIRE_COMPONENT
							|| obj->otyp == SCRAP);
				break;
			default:
				/* androids need the rarer subethaic components */
				if (is_android(mm->data))
					can_use_obj = (obj->otyp == SUBETHAIC_COMPONENT);
				/* all other clockworks need standard clockwork components */
				else
					can_use_obj = (obj->otyp == CLOCKWORK_COMPONENT);
				break;
			}
		}
		else {
			if (mm == &youmonst)
				You("aren't made of clockwork!");
			else
				pline("It isn't made of clockwork.");
			return MOVE_CANCELLED;
		}

		/* check we have the right type of part */
		if (!can_use_obj) {
			if (mm == &youmonst)
				You("can't repair yourself with this kind of part.");
			else
				pline("It can't take this kind of part.");
			return MOVE_CANCELLED;
		}

		if (mm != &youmonst) {
			/* repairing monsters */
			if (mm->mhp < mm->mhpmax){
				if (yn("Repair it?") == 'y'){
					mm->mhp += mm->m_lev;
					if (mm->mhp > mm->mhpmax)
						mm->mhp = mm->mhpmax;
					useup(obj);
					*optr = 0;
					return MOVE_STANDARD;
				}
			}
			else {
				pline("It doesn't need repairs.");
				return MOVE_CANCELLED;
			}
			/* done */
		}
		else {
			/* repairing yourself */
			if (uhp() >= uhpmax()) {
				You("don't need repairs.");
				return MOVE_CANCELLED;
			}
			else {
				healup(Upolyd ? mons[u.umonnum].mlevel : u.ulevel, 0, FALSE, FALSE);
				/* subethaic components also restore a small amount of pw */
				if (obj->otyp == SUBETHAIC_COMPONENT) {
					u.uen += d(4,2);
					if (u.uen > u.uenmax)
						u.uen = u.uenmax;
				}
				useup(obj);
				*optr = 0;
				return MOVE_STANDARD;
			}
			/* done */
		}
	}
	else {
		/* no one there, attempt to make a clockwork servant */

		/* Scrap is useless in making clockworks */
		if (obj->otyp == SCRAP) {
			You("can't build anything with this worthless scrap.");
			return MOVE_CANCELLED;
		}

		cc.x = u.ux + u.dx; cc.y = u.uy + u.dy;
		/* Passing FALSE arg here will result in messages displayed */

		if (obj->quan < 10 && obj->otyp != SCRAP){
			You("don't have enough components to build a clockwork servant.");
			return MOVE_CANCELLED;
		}
		pmm = clockworkMenu(obj);
		if (!pmm) return MOVE_CANCELLED;
		if (!clockwork_location_checks(obj, &cc, FALSE)) return MOVE_CANCELLED;
		You("build a clockwork and %s.",
			(u.dx || u.dy) ? "set it beside you" :
			(Weightless || Is_waterlevel(&u.uz) ||
			is_pool(cc.x, cc.y, TRUE)) ?
			"release it" :
			(u.dz < 0 ?
			"toss it into the air" :
			"set it on the ground"));

		mm = makemon(pmm, u.ux + u.dx, u.uy + u.dy, MM_EDOG | MM_ADJACENTOK | NO_MINVENT | MM_NOCOUNTBIRTH);
		if (mm){
			initedog(mm);
			mm->m_lev = u.ulevel / 2 + 1;
			mm->mhpmax = (mm->m_lev * hd_size(mm->data)) - hd_size(mm->data)/2;
			mm->mhp = mm->mhpmax;
			mm->mtame = 10;
			mm->mpeaceful = 1;
			if ((u.dx || u.dy) && (mm->mtyp == PM_CLOCKWORK_SOLDIER || mm->mtyp == PM_CLOCKWORK_DWARF ||
				mm->mtyp == PM_FABERGE_SPHERE || mm->mtyp == PM_FIREWORK_CART ||
				mm->mtyp == PM_JUGGERNAUT || mm->mtyp == PM_ID_JUGGERNAUT)
				){
				mm->mvar_vector = -1;
				while (xdir[(int)(++mm->mvar_vector)] != u.dx || ydir[(int)mm->mvar_vector] != u.dy);
			}
		}
		obj->quan -= 9;
		useup(obj);
		*optr = 0;
		return MOVE_STANDARD;
	}
	/* if we used up the part, this took time */
	return ((*optr) == 0) ? MOVE_STANDARD : MOVE_CANCELLED;
}

STATIC_OVL long
upgradeMenu()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	
	Sprintf(buf, "Upgrade types");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	if(!(u.clockworkUpgrades&OIL_STOVE)){
		Sprintf(buf, "oil stove");
		any.a_int = 1;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(!(u.clockworkUpgrades&WOOD_STOVE) && u.clockworkUpgrades&OIL_STOVE){
		Sprintf(buf, "wood stove");
		any.a_int = 2;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(!(u.clockworkUpgrades&FAST_SWITCH)){
		Sprintf(buf, "fast speed switch");
		any.a_int = 3;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(!(u.clockworkUpgrades&EFFICIENT_SWITCH)){
		Sprintf(buf, "efficient speed switch");
		any.a_int = 4;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(!(u.clockworkUpgrades&ARMOR_PLATING)){
		Sprintf(buf, "armor plating");
		any.a_int = 5;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(!(u.clockworkUpgrades&PHASE_ENGINE) && !flags.beginner){
		Sprintf(buf, "phase engine");
		any.a_int = 6;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(!(u.clockworkUpgrades&MAGIC_FURNACE) && u.clockworkUpgrades&OIL_STOVE && !flags.beginner){
		Sprintf(buf, "magic furnace");
		any.a_int = 7;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(!(u.clockworkUpgrades&HELLFIRE_FURNACE) && u.clockworkUpgrades&OIL_STOVE && !flags.beginner){
		Sprintf(buf, "hellfire furnace");
		any.a_int = 8;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(!(u.clockworkUpgrades&SCRAP_MAW && !flags.beginner)){
		Sprintf(buf, "scrap maw");
		any.a_int = 9;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(!(u.clockworkUpgrades&HIGH_TENSION)){
		Sprintf(buf, "high-tension spring");
		any.a_int = 10;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	end_menu(tmpwin, "Choose type:");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if(n > 0){
		long picked = 0x1L<<(selected[0].item.a_int - 1);
		free(selected);
		return picked;
	}
	return 0;
}

boolean
set_obj_shape(obj, shape)
struct obj *obj;
long shape;
{
	long starting_shape = obj->bodytypeflag;
	//Dragon scales don't have a shape.
	if(Is_dragon_scales(obj))
		return FALSE;
	if(is_shirt(obj) || is_suit(obj)){
		//Only CHANGE the shape if the result will be valid.
		if(shape&MB_BODYTYPEMASK){
			//Body gloves cover the whole body.
			if(obj->otyp == BODYGLOVE)
				obj->bodytypeflag = shape&MB_BODYTYPEMASK;
			//Shirts, dresses, and togas aren't concerned by the shape of the lower body.
			else if (is_shirt(obj) || obj->otyp == ELVEN_TOGA || is_dress(obj->otyp))
				obj->bodytypeflag = (shape&MB_HUMANOID) ? MB_HUMANOID : (shape&MB_BODYTYPEMASK);
			else if (is_suit(obj))
				obj->bodytypeflag = shape&MB_BODYTYPEMASK;
		}
		//If the given shape is invalid and the object's current shape is invalid, set the object to humanoid.
		// I believe this will never happen, as armor is initialized to humanoid.
		else if((obj->bodytypeflag&MB_BODYTYPEMASK) == 0)
				obj->bodytypeflag = MB_HUMANOID;
	}
	else if (is_helmet(obj) && !is_hat(obj))
		obj->bodytypeflag = shape&MB_HEADMODIMASK;
	if(obj->bodytypeflag != starting_shape)
		return TRUE;
	return FALSE;
}

STATIC_OVL int
resizeArmor()
{
	struct obj *otmp;
	struct permonst *ptr;
	struct monst *mtmp;
	int rx, ry;
	boolean changed = FALSE;
	
    if (!getdir("Resize armor to fit what creature? (in what direction)")) {
		/* decided not to */
		return MOVE_CANCELLED;
	}

#ifdef STEED
    if (u.usteed && u.dz > 0) ptr = u.usteed->data;
	else 
#endif
	if(u.dz){
		pline("No creature there.");
		return MOVE_CANCELLED;
	} else if (u.dx == 0 && u.dy == 0) {
		ptr = youracedata;
    } else {
		rx = u.ux+u.dx; ry = u.uy+u.dy;
		mtmp = m_at(rx, ry);
		if(!mtmp){
			pline("No creature there.");
			return MOVE_CANCELLED;
		}
		ptr = mtmp->data;
	}
	// attempt to find a piece of armor to resize
	NEARDATA const char clothes[] = { ARMOR_CLASS, TOOL_CLASS, 0 };
	otmp = getobj(clothes, "resize");
	if (!otmp) return MOVE_CANCELLED;
	
	// check that the armor is not currently being worn
	if (otmp->owornmask){
		You("are wearing that!");
		return MOVE_CANCELLED;
	}
	// check that the armor is not dragon scales (which cannot be resized)
	if (Is_dragon_scales(otmp)){
		pline("Dragon scales cannot be resized.");
		return MOVE_CANCELLED;
	}

	// change shape
	if (is_shirt(otmp) || otmp->otyp == ELVEN_TOGA || is_suit(otmp)){
		//Check that the monster can actually have armor that fits it.
		if(!(ptr->mflagsb&MB_BODYTYPEMASK)){
			You("can't figure out how to make it fit.");
			return MOVE_CANCELLED;
		}
		changed |= set_obj_shape(otmp, ptr->mflagsb);
	}
	else if (is_helmet(otmp) && !is_hat(otmp)){
		//Check that the monster can actually have armor that fits it.
		if(!has_head(ptr)){
			pline("No head!");
			return MOVE_CANCELLED;
		}
		changed |= set_obj_shape(otmp, ptr->mflagsb);
	}
	
	// change size (AFTER shape, because this may be aborted during that step.
	if(otmp->objsize != ptr->msize){
		otmp->objsize = ptr->msize;
		changed = TRUE;
	}

	fix_object(otmp);

	if(!changed){
		You("figure it already fits fine.");
		return MOVE_CANCELLED;
	}
	You("resize the armor to fit.");
	pline("The kit is used up.");
	return MOVE_STANDARD;
}

#define STANDARD_UPGRADE(prop, subsystem) \
	if(check_imp_mod(arm, prop)){ \
		pline("You've already repaired the %s.", subsystem); \
		return MOVE_CANCELLED; \
	} \
	else { \
		pline("You repair the %s.", subsystem); \
		add_imp_mod(arm, prop); \
		useup(upitm); \
		return MOVE_STANDARD; \
	}


STATIC_OVL int
upgradeImpArmor()
{
	struct obj *upitm;
	struct obj *arm = getobj(apply_armor, "armor piece to repair");
	if(!arm){
		return MOVE_CANCELLED;
	}
	if(!is_imperial_elven_armor(arm)){
		pline("That doesn't look like a piece of imperial armor.");
		return MOVE_CANCELLED;
	}
	if(check_imp_mod(arm, IEA_NOUPGRADES)){
		pline("This less-sophisticated armor may lack certain functions, but that is not the result of damage.");
		return MOVE_CANCELLED;
	}
	if(arm->owornmask){
		You("will need to take that off to repair it.");
		return MOVE_CANCELLED;
	}
	switch(arm->otyp){
		case IMPERIAL_ELVEN_HELM:
			upitm = getobj(imperial_repairs, "repair the helm with");
			if(!upitm || !helm_upgrade_obj(upitm)){
				pline("Never mind.");
				return MOVE_CANCELLED;
			}
			if(upitm->owornmask){
				pline("You're still using that.");
				return MOVE_CANCELLED;
			}
			switch(upitm->otyp){
				case AMULET_OF_MAGICAL_BREATHING:
					STANDARD_UPGRADE(IEA_NOBREATH, "life-support subsystem")
				break;
				case WAN_DRAINING:
					STANDARD_UPGRADE(IEA_LIFESENSE, "life-sign sensor")
				break;
				case RIN_SEE_INVISIBLE:
					STANDARD_UPGRADE(IEA_SEE_INVIS, "crystal eye")
				break;
				case HELM_OF_TELEPATHY:
				case AMULET_OF_ESP:
					STANDARD_UPGRADE(IEA_TELEPAT, "extrasensory perception subsystem")
				break;
				case CRYSTAL_HELM:
					STANDARD_UPGRADE(IEA_BLIND_RES, "visor")
				break;
				case RIN_INCREASE_ACCURACY:
					STANDARD_UPGRADE(IEA_INC_ACC, "targetting subsystem")
				break;
				case RIN_TELEPORT_CONTROL:
					STANDARD_UPGRADE(IEA_TELE_CNTRL, "teleportation control subsystem")
				break;
				case RIN_PROTECTION_FROM_SHAPE_CHAN:
					STANDARD_UPGRADE(IEA_PROT_SHAPE, "self-bored lens")
				break;
				default:
					impossible("Unknown repair component, sorry :(.");
					return MOVE_CANCELLED;
				break;
			}
		break;
		case IMPERIAL_ELVEN_GAUNTLETS:
			upitm = getobj(imperial_repairs, "repair the gauntlets with");
			if(!upitm || !gauntlets_upgrade_obj(upitm)){
				pline("Never mind.");
				return MOVE_CANCELLED;
			}
			if(upitm->owornmask){
				pline("You're still using that.");
				return MOVE_CANCELLED;
			}
			switch(upitm->otyp){
				case WATER_WALKING_BOOTS:
					STANDARD_UPGRADE(IEA_SWIMMING, "swimming webs")
				break;
				case GAUNTLETS_OF_POWER:
					STANDARD_UPGRADE(IEA_GOPOWER, "power servos")
				break;
				case GAUNTLETS_OF_DEXTERITY:
					STANDARD_UPGRADE(IEA_GODEXTERITY, "dexterity servos")
				break;
				case RIN_INCREASE_DAMAGE:
					STANDARD_UPGRADE(IEA_INC_DAM, "microtargetting servos")
				break;
				case WAN_MAGIC_MISSILE:
					STANDARD_UPGRADE(IEA_BOLTS, "missile projectors")
				break;
				case AMULET_OF_STRANGULATION:
					STANDARD_UPGRADE(IEA_STRANGLE, "grappling servos")
				break;
				default:
					impossible("Unknown repair component, sorry :(.");
					return MOVE_CANCELLED;
				break;
			}
		break;
		case IMPERIAL_ELVEN_ARMOR:
			upitm = getobj(imperial_repairs, "repair the armor with");
			if(!upitm || !armor_upgrade_obj(upitm)){
				pline("Never mind.");
				return MOVE_CANCELLED;
			}
			if(upitm->owornmask){
				pline("You're still using that.");
				return MOVE_CANCELLED;
			}
			switch(upitm->otyp){
				case FLYING_BOOTS:
					STANDARD_UPGRADE(IEA_FLYING, "moth wings")
				break;
				case RIN_SUSTAIN_ABILITY:
					STANDARD_UPGRADE(IEA_FIXED_ABIL, "stasis subsystem")
				break;
				case RIN_REGENERATION:
				case AMULET_OF_WOUND_CLOSURE:
					STANDARD_UPGRADE(IEA_FAST_HEAL, "medical subsystem")
				break;
				case AMULET_OF_REFLECTION:
				case SHIELD_OF_REFLECTION:
				case JUMPSUIT:
					STANDARD_UPGRADE(IEA_REFLECTING, "reflective chestplate")
				break;
				case AMULET_VERSUS_SICKNESS:
				case HEALER_UNIFORM:
					STANDARD_UPGRADE(IEA_SICK_RES, "sealed bodyglove")
				break;
				case CLOAK_OF_PROTECTION:
					STANDARD_UPGRADE(IEA_HALF_PHDAM, "ballistic base layer")
				break;
				case CLOAK_OF_MAGIC_RESISTANCE:
				case ORIHALCYON_GAUNTLETS:
					STANDARD_UPGRADE(IEA_HALF_SPDAM, "dispersive underlayer")
				break;
				case CLOAK_OF_DISPLACEMENT:
					STANDARD_UPGRADE(IEA_DISPLACED, "holographic projector")
				break;
				case CLOAK_OF_INVISIBILITY:
				case RIN_INVISIBILITY:
				case WAN_MAKE_INVISIBLE:
					STANDARD_UPGRADE(IEA_INVIS, "active camouflage system")
				break;
				case RIN_PROTECTION:
					STANDARD_UPGRADE(IEA_DEFLECTION, "deflectors")
				break;
				default:
					impossible("Unknown repair component, sorry :(.");
					return MOVE_CANCELLED;
				break;
			}
		break;
		case IMPERIAL_ELVEN_BOOTS:
			upitm = getobj(imperial_repairs, "repair the boots with");
			if(!upitm || !boots_upgrade_obj(upitm)){
				pline("Never mind.");
				return MOVE_CANCELLED;
			}
			if(upitm->owornmask){
				pline("You're still using that.");
				return MOVE_CANCELLED;
			}
			switch(upitm->otyp){
				case JUMPING_BOOTS:
					STANDARD_UPGRADE(IEA_JUMPING, "jump jets")
				break;
				case WAN_SPEED_MONSTER:
				case RIN_ALACRITY:
				case SPEED_BOOTS:
					STANDARD_UPGRADE(IEA_FAST, "speed boosters")
				break;
				case WAN_TELEPORTATION:
				case RIN_TELEPORTATION:
					STANDARD_UPGRADE(IEA_TELEPORT, "blink subsystem")
				break;
				case KICKING_BOOTS:
					STANDARD_UPGRADE(IEA_KICKING, "concussive impactors")
				break;
				default:
					impossible("Unknown repair component, sorry :(.");
					return MOVE_CANCELLED;
				break;
			}
		break;
		default:
			impossible("Unknown armor piece?");
			return MOVE_CANCELLED;
		break;
	}
}

STATIC_OVL int
doUseUpgradeKit(optr)
struct obj **optr;
{
	struct obj *obj = *optr;
	struct obj *comp;
	// Create an array with all classes explicitly listed in it, 1-MAXOCLASSES :(
	char all_classes[MAXOCLASSES] = {0};
	for(int i = 1; i < MAXOCLASSES; i++)
		all_classes[i-1] = i;
	if(uclockwork){
		if (yn("Make an upgrade to yourself?") == 'y'){
			long upgrade = upgradeMenu();
			switch(upgrade){
				case OIL_STOVE:
					You("use the components in the upgrade kit to install an oil stove.");
					u.clockworkUpgrades |= upgrade;
					useup(obj);
					*optr = 0;
					return MOVE_STANDARD;
				break;
				case WOOD_STOVE:
					comp = getobj(tools, "upgrade your stove with");
					if(!comp || comp->otyp != TINNING_KIT){
						pline("Never mind.");
						return MOVE_CANCELLED;
					}
					You("use the components in the upgrade kit and the tinning kit to install a wood-burning stove.");
					u.clockworkUpgrades |= upgrade;
					useup(comp);
					useup(obj);
					*optr = 0;
					return MOVE_STANDARD;
				break;
				case FAST_SWITCH:
					You("use the components in the upgrade kit to install a fast switch on your clock.");
					u.clockworkUpgrades |= upgrade;
					useup(obj);
					*optr = 0;
					return MOVE_STANDARD;
				break;
				case EFFICIENT_SWITCH:
					comp = getobj(tools, "upgrade your switch with");
					if(!comp || comp->otyp != CROSSBOW){
						pline("Never mind.");
						return MOVE_CANCELLED;
					}
					You("use the components in the upgrade kit and the crossbow to upgrade the switch on your clock.");
					u.clockworkUpgrades |= upgrade;
					useup(comp);
					useup(obj);
					*optr = 0;
					return MOVE_STANDARD;
				break;
				case ARMOR_PLATING:
					comp = getobj(apply_armor, "upgrade your armor with");
					if(!comp ||
						!((comp->otyp == ARCHAIC_PLATE_MAIL || comp->otyp == PLATE_MAIL) &&
						(comp->obj_material == COPPER))){
						pline("Never mind.");
						return MOVE_CANCELLED;
					}
					You("use the components in the upgrade kit to reinforce your armor with bronze plates.");
					u.clockworkUpgrades |= upgrade;
					useup(comp);
					useup(obj);
					*optr = 0;
					return MOVE_STANDARD;
				break;
				case PHASE_ENGINE:
					comp = getobj(all_classes, "build a phase engine with");
					if(!comp || comp->otyp != SUBETHAIC_COMPONENT){
						pline("Never mind.");
						return MOVE_CANCELLED;
					}
					You("combine the components in the upgrade kit with the subethaic component and build a phase engine.");
					u.clockworkUpgrades |= upgrade;
					useup(comp);
					useup(obj);
					*optr = 0;
					return MOVE_STANDARD;
				break;
				case MAGIC_FURNACE:
					comp = getobj(apply_corpse, "build a magic furnace with");
					if(!comp || comp->otyp != CORPSE || comp->corpsenm != PM_DISENCHANTER){
						pline("Never mind.");
						return MOVE_CANCELLED;
					}
					You("combine the components in the upgrade kit with the disenchanter corpse and build a magic furnace.");
					u.clockworkUpgrades |= upgrade;
					useup(comp);
					useup(obj);
					*optr = 0;
					return MOVE_STANDARD;
				break;
				case HELLFIRE_FURNACE:
					comp = getobj(all_classes, "build a hellfire furnace with");
					if(!comp || comp->otyp != HELLFIRE_COMPONENT){
						pline("Never mind.");
						return MOVE_CANCELLED;
					}
					You("combine the components in the upgrade kit with the hellfire component and build a hellfire furnace.");
					u.clockworkUpgrades |= upgrade;
					useup(comp);
					useup(obj);
					*optr = 0;
					return MOVE_STANDARD;
				break;
				case SCRAP_MAW:
					comp = getobj(tools, "build a scrap maw with");
					if(!comp || comp->otyp != SCRAP){
						pline("Never mind.");
						return MOVE_CANCELLED;
					}
					You("combine the components in the upgrade kit with the scrap and build a scrap maw.");
					u.clockworkUpgrades |= upgrade;
					useup(comp);
					useup(obj);
					*optr = 0;
					return MOVE_STANDARD;
				break;
				case HIGH_TENSION:
					// Maybe one day a spring pistol or something
					// comp = getobj(tools, "build a scrap maw with");
					// if(!comp || comp->otyp != SCRAP){
						// pline("Never mind.");
						// return MOVE_CANCELLED;
					// }
					You("use the components in the upgrade kit to increase the maximum tension in your mainspring.");
					u.uhungermax += DEFAULT_HMAX;
					if(u.uhungermax >= DEFAULT_HMAX*10) u.clockworkUpgrades |= upgrade;
					// useup(comp);
					useup(obj);
					*optr = 0;
					return MOVE_STANDARD;
				break;
			}
		}
	}
	else if(u.uiearepairs && carrying_imperial_elven_armor()){
		if (yn("Repair your imperial armor?") == 'y'){
			if (upgradeImpArmor() != MOVE_CANCELLED){
				useup(obj);
				*optr = 0;
				return MOVE_STANDARD;
			}
		}
	}
	if (yn("Resize a piece of armor?") == 'y'){
		if (resizeArmor() != MOVE_CANCELLED){
			useup(obj);
			*optr = 0;
			return MOVE_STANDARD;
		}
		else
			return MOVE_CANCELLED;
	}
	return MOVE_CANCELLED;
}

int
doapply()
{
	struct obj *obj;
	register int res = MOVE_DEFAULT;
	int waslabile = FALSE;
	char class_list[MAXOCLASSES+2];

	if(check_capacity((char *)0)) return MOVE_CANCELLED;

	if (carrying(POT_OIL) || uhave_graystone())
		Strcpy(class_list, tools_too);
	else
		Strcpy(class_list, tools);
	if (carrying(CREAM_PIE) || carrying(EUCALYPTUS_LEAF))
		add_class(class_list, FOOD_CLASS);
	if (carrying(DWARVISH_HELM) || carrying(LANTERN_PLATE_MAIL) ||
		carrying(GNOMISH_POINTY_HAT) || carrying(DROVEN_CLOAK) ||
		carrying_art(ART_AEGIS) || carrying(EILISTRAN_ARMOR))
		add_class(class_list, ARMOR_CLASS);
	if(carrying_applyable_ring()){
		add_class(class_list, RING_CLASS);
	}
	if(carrying_applyable_amulet()){
		add_class(class_list, AMULET_CLASS);
	}
	if(carrying_applyable_gem()){
		add_class(class_list, GEM_CLASS);
	}


	obj = getobj(class_list, "use or apply");
	if(!obj) return MOVE_CANCELLED;

	waslabile = objects[obj->otyp].oc_merge; //Some functions leave a stale pointer here if they merge the item
	
	waslabile |= obj->otyp == DOLL_S_TEAR; //Not mergeable, but still consumable.
	
	if (obj->oartifact && !(uwep == obj && is_pole(obj)) && !touch_artifact(obj, &youmonst, FALSE))
	    return MOVE_STANDARD;	/* evading your grasp costs a turn; just be
			   grateful that you don't drop it as well */

	if(obj->ostolen && u.sealsActive&SEAL_ANDROMALIUS) unbind(SEAL_ANDROMALIUS, TRUE);

	if (obj->oclass == WAND_CLASS)
	    return do_break_wand(obj);
	else if (obj->oclass == COIN_CLASS)
	    return do_flip_coin(obj);
	else if (obj->oclass == SCOIN_CLASS)
	    return do_soul_coin(obj);
	else if (obj->oclass == RING_CLASS || obj->oclass == AMULET_CLASS)
	    return do_present_item(obj);
	else if(is_knife(obj) && !(obj->oartifact==ART_PEN_OF_THE_VOID && obj->ovar1_seals&SEAL_MARIONETTE)) 
		return do_carve_obj(obj);
	
	if(obj->oartifact == ART_SILVER_STARLIGHT) res = do_play_instrument(obj);
	else if(obj->oartifact == ART_HOLY_MOONLIGHT_SWORD && !u.veil) use_lamp(obj);
	else if(obj->oartifact == ART_BLOODLETTER && artinstance[obj->oartifact].BLactive >= monstermoves) res = do_bloodletter(obj);
	else if(obj->oartifact == ART_AEGIS) res = swap_aegis(obj);
	else if(obj->oartifact == ART_STAFF_OF_AESCULAPIUS) res = aesculapius_poke(obj);
	else if(obj->oartifact == ART_ESSCOOAHLIPBOOURRR) res = aesculapius_poke(obj);
	else if(obj->oartifact == ART_RED_CORDS_OF_ILMATER) res = ilmater_touch(obj);
	else if(obj->otyp == RAKUYO || obj->otyp == RAKUYO_SABER){
		return use_rakuyo(obj);
	}
	else if(obj->otyp == BLADE_OF_MERCY || obj->otyp == BLADE_OF_GRACE){
		return use_mercy_blade(obj);
	} else if(obj->otyp == DOUBLE_FORCE_BLADE || obj->otyp == FORCE_BLADE){
		return use_force_blade(obj);
	} else switch(obj->otyp){
	case BLINDFOLD:
	case ANDROID_VISOR:
	case LENSES:
	case SUNGLASSES:
	case LIVING_MASK:
	case R_LYEHIAN_FACEPLATE:
	case MASK:
		if (obj == ublindf) {
		    if (!cursed(obj)) Blindf_off(obj);
		} else if (ublindf){
			You("are already %s.",
				ublindf->otyp == TOWEL ?     "covered by a towel" :
				(ublindf->otyp == MASK || ublindf->otyp == LIVING_MASK || ublindf->otyp == R_LYEHIAN_FACEPLATE ) ? "wearing a mask" :
				(ublindf->otyp == BLINDFOLD || ublindf->otyp == ANDROID_VISOR) ? "wearing a blindfold" :
						     "wearing lenses");
		} else if((obj->otyp == LENSES || obj->otyp == SUNGLASSES)
			&& obj->objsize != youracedata->msize
		){
			pline("They don't fit!");
		} else
		    Blindf_on(obj);
		break;
	case CREAM_PIE:
		res = use_cream_pie(obj);
		break;
	case FORCE_SWORD:
		res = use_force_sword(obj);
		break;
	case FORCE_WHIP:
	case VIPERWHIP:
	case BULLWHIP:
		res = use_whip(obj);
		break;
	case NUNCHAKU:
		res = use_nunchucks(obj);
		break;
	case GRAPPLING_HOOK:
		res = use_grapple(obj);
		break;
	case SHEPHERD_S_CROOK:
		res = use_crook(obj);
		break;
	case BOX:
	case CHEST:
	case ICE_BOX:
	case SACK:
	case SARCOPHAGUS:
	case BAG_OF_HOLDING:
	case OILSKIN_SACK:
		res = use_container(obj, 1);
		goto xit2; /* obj may have been destroyed */
		break;
	case BAG_OF_TRICKS:
		bagotricks(obj, FALSE, (int *) 0);
		break;
	case CAN_OF_GREASE:
		use_grease(obj);
		break;
	case LOCK_PICK:
#ifdef TOURIST
	case CREDIT_CARD:
#endif
	case SKELETON_KEY:
		res = pick_lock(&obj);
		break;
	case UNIVERSAL_KEY:
		res =  pick_lock(&obj);
		break;
	case PICK_AXE:
	case DWARVISH_MATTOCK:
	case SEISMIC_HAMMER:
		res = use_pick_axe(obj);
		break;
	case TINNING_KIT:
		use_tinning_kit(obj);
		break;
	case TREPHINATION_KIT:
		use_trephination_kit(obj);
		break;
	case LEASH:
		use_leash(obj);
		break;
#ifdef STEED
	case SADDLE:
		res = use_saddle(obj);
		break;
#endif
	case MAGIC_WHISTLE:
		use_magic_whistle(obj);
		break;
	case WHISTLE:
		use_whistle(obj);
		break;
	case EUCALYPTUS_LEAF:
		/* MRKR: Every Australian knows that a gum leaf makes an */
		/*	 excellent whistle, especially if your pet is a  */
		/*	 tame kangaroo named Skippy.			 */
		if (obj->blessed) {
		    use_magic_whistle(obj);
		    /* sometimes the blessing will be worn off */
		    if (!rn2(49)) {
			if(obj->quan > 1) obj = splitobj(obj, 1L);

			if (!Blind) {
			    char buf[BUFSZ];

			    pline("%s %s %s.", Shk_Your(buf, obj),
				  aobjnam(obj, "glow"), hcolor("brown"));
			    obj->bknown = 1;
			}
			unbless(obj);
			obj_extract_self(obj);	/* free from inv */
			/* shouldn't merge */
			obj = hold_another_object(obj, "You drop %s!",
						  doname(obj), (const char *)0);
		    }
		} else {
		    use_whistle(obj);
		}
		break;
	case STETHOSCOPE:
		res = use_stethoscope(obj);
		break;
	case MIRROR:
		res = use_mirror(&obj);
		break;
	case SPOON:
		if(Role_if(PM_CONVICT)) pline("The guards used to hand these out with our food rations.  No one was ever able to figure out why.");
		else pline("You have never in your life seen such an odd item.  You have no idea how to use it.");
	break;
	case BELL:
	case BELL_OF_OPENING:
		use_bell(&obj, FALSE);
		break;
	case CANDELABRUM_OF_INVOCATION:
		use_candelabrum(obj);
		break;
	case WAX_CANDLE:
	case TALLOW_CANDLE:
	case CANDLE_OF_INVOCATION:
	case GNOMISH_POINTY_HAT:
		use_candle(&obj);
	break;
	case BULLET_FABBER:
	if(!(Role_if(PM_ANACHRONONAUT) || Role_if(PM_TOURIST))) pline("It seems inert.");
	else {
		static const char all_count[] = { ALLOW_COUNT, WEAPON_CLASS, GEM_CLASS, 0 };
		struct obj *otmp = getobj(all_count, "feed to the fabber");
		if (!otmp) break;
		switch(otmp->otyp){
			case ROCK:
				obj_extract_self(otmp);
				otmp = poly_obj(otmp,BULLET);
				otmp = hold_another_object(otmp, u.uswallow ?
						   "Oops!  %s out of your reach!" :
						(Weightless ||
						 Is_waterlevel(&u.uz) ||
						 levl[u.ux][u.uy].typ < IRONBARS ||
						 levl[u.ux][u.uy].typ >= ICE) ?
							   "Oops!  %s away from you!" :
							   "Oops!  %s to the floor!",
							   The(aobjnam(otmp, "slip")),
							   (const char *)0);
			break;
			case SILVER_SLINGSTONE:
				obj_extract_self(otmp);
				otmp = poly_obj(otmp,SILVER_BULLET);
				otmp->quan *= 2;
				otmp->owt = weight(otmp);
				otmp = hold_another_object(otmp, u.uswallow ?
						   "Oops!  %s out of your reach!" :
						(Weightless ||
						 Is_waterlevel(&u.uz) ||
						 levl[u.ux][u.uy].typ < IRONBARS ||
						 levl[u.ux][u.uy].typ >= ICE) ?
							   "Oops!  %s away from you!" :
							   "Oops!  %s to the floor!",
							   The(aobjnam(otmp, "slip")),
							   (const char *)0);
			break;
			case FLINT:
				obj_extract_self(otmp);
				otmp = poly_obj(otmp,SHOTGUN_SHELL);
				otmp = hold_another_object(otmp, u.uswallow ?
						   "Oops!  %s out of your reach!" :
						(Weightless ||
						 Is_waterlevel(&u.uz) ||
						 levl[u.ux][u.uy].typ < IRONBARS ||
						 levl[u.ux][u.uy].typ >= ICE) ?
							   "Oops!  %s away from you!" :
							   "Oops!  %s to the floor!",
							   The(aobjnam(otmp, "slip")),
							   (const char *)0);
			break;
			case LOADSTONE:
				obj_extract_self(otmp);
				otmp = poly_obj(otmp,ROCKET);
				otmp = hold_another_object(otmp, u.uswallow ?
						   "Oops!  %s out of your reach!" :
						(Weightless ||
						 Is_waterlevel(&u.uz) ||
						 levl[u.ux][u.uy].typ < IRONBARS ||
						 levl[u.ux][u.uy].typ >= ICE) ?
							   "Oops!  %s away from you!" :
							   "Oops!  %s to the floor!",
							   The(aobjnam(otmp, "slip")),
							   (const char *)0);
			break;
			case BULLET:
				obj_extract_self(otmp);
				otmp = poly_obj(otmp,SHOTGUN_SHELL);
				otmp = hold_another_object(otmp, u.uswallow ?
						   "Oops!  %s out of your reach!" :
						(Weightless ||
						 Is_waterlevel(&u.uz) ||
						 levl[u.ux][u.uy].typ < IRONBARS ||
						 levl[u.ux][u.uy].typ >= ICE) ?
							   "Oops!  %s away from you!" :
							   "Oops!  %s to the floor!",
							   The(aobjnam(otmp, "slip")),
							   (const char *)0);
			break;
			case SHOTGUN_SHELL:
				obj_extract_self(otmp);
				otmp = poly_obj(otmp,BULLET);
				otmp = hold_another_object(otmp, u.uswallow ?
						   "Oops!  %s out of your reach!" :
						(Weightless ||
						 Is_waterlevel(&u.uz) ||
						 levl[u.ux][u.uy].typ < IRONBARS ||
						 levl[u.ux][u.uy].typ >= ICE) ?
							   "Oops!  %s away from you!" :
							   "Oops!  %s to the floor!",
							   The(aobjnam(otmp, "slip")),
							   (const char *)0);
			break;
			case SILVER_BULLET:
			if(otmp->quan >= 10){
				struct obj *rocket;
				rocket = mksobj(ROCKET, MKOBJ_NOINIT);
				rocket->blessed = otmp->blessed;
				rocket->cursed = otmp->cursed;
				rocket->quan = (otmp->quan)/10;
				rocket->spe = otmp->spe;
				rocket->dknown = TRUE;
				rocket->known = otmp->known;
				rocket->bknown = otmp->bknown;
				rocket->rknown = otmp->rknown;
				rocket->sknown = otmp->sknown;
				if((otmp->quan = (otmp->quan)%10) == 0) useup(otmp);
				else otmp->owt = weight(otmp);
				rocket->owt = weight(rocket);
				rocket = hold_another_object(rocket, u.uswallow ?
						   "Oops!  %s out of your reach!" :
						(Weightless ||
						 Is_waterlevel(&u.uz) ||
						 levl[u.ux][u.uy].typ < IRONBARS ||
						 levl[u.ux][u.uy].typ >= ICE) ?
							   "Oops!  %s away from you!" :
							   "Oops!  %s to the floor!",
							   The(aobjnam(rocket, "slip")),
							   (const char *)0);
			}break;
			case ROCKET:{
				struct obj *bullets;
				bullets = mksobj(SILVER_BULLET, MKOBJ_NOINIT);
				bullets->blessed = otmp->blessed;
				bullets->cursed = otmp->cursed;
				bullets->quan = (otmp->quan)*10;
				bullets->spe = otmp->spe;
				bullets->dknown = TRUE;
				bullets->known = otmp->known;
				bullets->bknown = otmp->bknown;
				bullets->rknown = otmp->rknown;
				bullets->sknown = otmp->sknown;
				useupall(otmp);
				bullets->owt = weight(bullets);
				bullets = hold_another_object(bullets, u.uswallow ?
						   "Oops!  %s out of your reach!" :
						(Weightless ||
						 Is_waterlevel(&u.uz) ||
						 levl[u.ux][u.uy].typ < IRONBARS ||
						 levl[u.ux][u.uy].typ >= ICE) ?
							   "Oops!  %s away from you!" :
							   "Oops!  %s to the floor!",
							   The(aobjnam(bullets, "slip")),
							   (const char *)0);
			}break;
		}
	}
	break;
	case POWER_PACK:{
		static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };
		struct obj *otmp = getobj(all_count, "charge");
		if (!otmp) break;
		if(otmp == obj){
			if(obj->quan > 1) pline("That seems rather pointless.");
			else pline("That seems rather difficult.");
		}
		You("press %s up against %s.", the(singular(obj,xname)), the(xname(otmp)));
		recharge(otmp, obj->cursed ? -1 : (obj->blessed ? 1 : 0));
		pline("%s is used up!",The(singular(obj,xname)));
		if(obj->quan>1)
			useup(obj);
		else{
			useup(obj);
			obj = 0;
		}
	}break;
	case LIGHTSABER:
  	case BEAMSWORD:
	case DOUBLE_LIGHTSABER:
	case ROD_OF_FORCE:
		if (uwep != obj && !(u.twoweap && uswapwep == obj) && !wield_tool(obj, (const char *)0)) break;
		/* Fall through - activate via use_lamp */
		    
/* MRKR: dwarvish helms are mining helmets. 
 * They can be used as brass lanterns. 	
 * From an idea posted to RGRN by "Dr Darth"
 * Code by Malcom Ryan
 */
	case DWARVISH_HELM: 
	case LANTERN_PLATE_MAIL: 
	case OIL_LAMP:
	case MAGIC_LAMP:
	case LANTERN:
		use_lamp(obj);
	break;
	case POT_OIL:
		light_cocktail(obj);
		obj = 0; //May have been dealocated, just get rid of it
	break;
	case SHADOWLANDER_S_TORCH:
		light_torch(obj);
	break;
	case MAGIC_TORCH:
		light_torch(obj);
	break;
	case SUNROD:
		light_torch(obj);
	break;
	case TORCH:
		light_torch(obj);
	break;
#ifdef TOURIST
	case EXPENSIVE_CAMERA:
		res = use_camera(obj);
	break;
#endif
	case TOWEL:
		res = use_towel(obj);
	break;
	case CRYSTAL_BALL:
		res = use_crystal_ball(obj);
	break;
	case MAGIC_MARKER:
		res = dowrite(obj);
	break;
	case TIN_OPENER:
		if(!carrying(TIN)) {
			You("have no tin to open.");
			goto xit;
		}
		You("cannot open a tin without eating or discarding its contents.");
		if(flags.verbose)
			pline("In order to eat, use the 'e' command.");
		if(obj != uwep)
    pline("Opening the tin will be much easier if you wield the tin opener.");
		goto xit;

	case FIGURINE:
		res = use_figurine(&obj);
	break;
	case CRYSTAL_SKULL:
		res = use_crystal_skull(&obj);
	break;
	case EFFIGY:{
	    struct obj *curo;
		if (Hallucination) You_feel("the tall leather doll take up your burdens!");
		else You_feel("like someone is helping you.");

		if(u.sealsActive&SEAL_MARIONETTE){
			unbind(SEAL_MARIONETTE,TRUE);
		}
		
		for (curo = invent; curo; curo = curo->nobj) {
#ifdef GOLDOBJ
			/* gold isn't subject to cursing and blessing */
			if (curo->oclass == COIN_CLASS) continue;
#endif
			if (curo->cursed) uncurse(curo);
		}
		if(Punished) unpunish();
		
#ifdef STEED
		/* also affects saddles */
		if (u.usteed) {
			curo = which_armor(u.usteed, W_SADDLE);
			if (curo) uncurse(curo);
		}
#endif

		//Share your insanity
		if(u.usanity < 100){
			change_usanity((Insanity)/2, FALSE);
		}

		if(u.wimage >= 10){
			struct monst *mtmp;
			u.wimage = 0;
			mtmp = makemon(&mons[PM_WEEPING_ANGEL], u.ux, u.uy, MM_ADJACENTOK|NO_MINVENT|MM_NOCOUNTBIRTH);
			if(mtmp && HDoubt)
				mtmp->mdoubt = TRUE;
			if(Blind) pline("The effigy grows and turns to stone!");
			else pline("The effigy becomes a weeping angel!");
		} else if(u.umorgul){
			int i = rnd(u.umorgul);
			struct obj *frags;
			u.umorgul -= i;
			frags = mksobj(SHURIKEN, MKOBJ_NOINIT);
			if(frags){
				frags->quan = i;
				add_oprop(frags, OPROP_LESSER_MORGW);
				set_material_gm(frags, METAL);
				curse(frags);
				fix_object(frags);
				pline("The effigy is pierced by %s!", 
					i==1 ? "a blade" : "blades");
				frags = hold_another_object(frags, "You drop %s!",
							  doname(frags), (const char *)0); /*shouldn't merge, but may drop*/
			}
			if(u.umummyrot)
				pline("The effigy crumbles to dust!");
			else {
				if(Blind) pline("The effigy bursts into flames!");
				else pline("The effigy burns with sickly flames!");
			}
		} else if (u.umummyrot){
				pline("The effigy crumbles to dust!");
		} else if (u.uhpmod < -18){
			if(Blind) pline("The effigy drips with a sticky liquid!");
			else pline("The effigy is scored by wounds!");
		} else {
			if(Blind) pline("The effigy bursts into flames!");
			else pline("The effigy burns with sickly flames!");
		}
		
		if(HDoubt){
			make_doubtful(0L, TRUE);
		}
		
		u.wimage = 0; //Sub-critical images are removed anyway.
		
		u.umummyrot = 0;
		
		if (u.uhpmod < 0){
			u.uhpmod /= 2; // we only print a message if it's a lot, but fix regardless
			calc_total_maxhp();		
		}
		
		if(obj->quan>1)
			useup(obj);
		else{
			useup(obj);
			obj = 0;
		}
		update_inventory();
	} break;
	case DOLL_OF_JUMPING:
		case DOLL_OF_FRIENDSHIP:
		case DOLL_OF_CHASTITY:
		case DOLL_OF_CLEAVING:
		case DOLL_OF_SATIATION:
		case DOLL_OF_GOOD_HEALTH:
		case DOLL_OF_FULL_HEALING:
		case DOLL_OF_DESTRUCTION:
		case DOLL_OF_MEMORY:
		case DOLL_OF_BINDING:
		case DOLL_OF_PRESERVATION:
		case DOLL_OF_QUICK_DRAWING:
		case DOLL_OF_WAND_CHARGING:
		case DOLL_OF_STEALING:
		case DOLL_OF_MOLLIFICATION:
		case DOLL_OF_CLEAR_THINKING:
		case DOLL_OF_MIND_BLASTING:
			res = use_doll(obj);
		break;
	case DOLL_S_TEAR:
		res = use_doll_tear(obj);
	break;
	case DILITHIUM_CRYSTAL:
		if(Role_if(PM_ANACHRONONAUT) && !obj->oartifact)
			res = use_dilithium(obj);
		else {
			pline("Sorry, I don't know how to use that.");
			nomul(0, NULL);
			return MOVE_CANCELLED;
		}
	break;
	case HOLY_SYMBOL_OF_THE_BLACK_MOTHE:
		return commune_with_goat();
	break;
	case PURIFIED_MIRROR:
		if(u.silver_atten) return commune_with_silver_flame();
		else res = use_mirror(&obj);
	break;
	case MISOTHEISTIC_PYRAMID:
	case MISOTHEISTIC_FRAGMENT:
		res = use_pyramid(obj);
	break;
	case DIMENSIONAL_LOCK:
		res = use_dimensional_lock(obj);
	break;
	case CATAPSI_VORTEX:
		res = use_vortex(obj);
	break;
	case ANTIMAGIC_RIFT:
		res = use_rift(obj);
	break;
	case VITAL_SOULSTONE:
		if (objects[obj->otyp].oc_name_known)
			res = use_vital(obj);
		else
			use_stone(obj);
	break;
	case SPIRITUAL_SOULSTONE:
		if (objects[obj->otyp].oc_name_known)
			res = use_spiritual(obj);
		else
			use_stone(obj);
	break;
	case PRESERVATIVE_ENGINE:
		res = res_engine_menu(obj);
	break;
	case ARMOR_SALVE:
		res = use_armor_salve(obj);
	break;
	case UNICORN_HORN:
		use_unicorn_horn(obj);
	break;
	case FLUTE:
	case MAGIC_FLUTE:
	case TOOLED_HORN:
	case FROST_HORN:
	case FIRE_HORN:
	case HARP:
	case MAGIC_HARP:
	case BUGLE:
	case DRUM:
	case DRUM_OF_EARTHQUAKE:
		res = do_play_instrument(obj);
	break;
	case HORN_OF_PLENTY:	/* not a musical instrument */
		(void) hornoplenty(obj, FALSE);
		break;
	break;
	case LAND_MINE:
	case BEARTRAP:
		use_trap(obj);
	break;
	case DROVEN_CLOAK:
		if(obj->oartifact == ART_DARKWEAVER_S_CLOAK) res = use_darkweavers_cloak(obj);
		else res = use_droven_cloak(&obj);
	break;
	case EILISTRAN_ARMOR:
		res = use_eilistran_armor(&obj);
	break;
	case FLINT:
	case LUCKSTONE:
	case LOADSTONE:
	case TOUCHSTONE:
		use_stone(obj);
	break;
//ifdef FIREARMS
	case SENSOR_PACK:
		res = use_sensor(obj);
	break;
	case HYPOSPRAY:
		res = use_hypospray(obj);
	break;
	case RAYGUN:
		if(obj->altmode == AD_FIRE){
			obj->altmode = AD_DEAD;
			You("set %s to kill.", yname(obj));
		} else if(obj->altmode == AD_DEAD){
			obj->altmode = AD_DISN;
			You("set %s to disintegrate.", yname(obj));
		} else if(obj->altmode == AD_DISN){
			obj->altmode = AD_SLEE;
			You("set %s to stun.", yname(obj));
		} else {
			obj->altmode = AD_FIRE;
			You("set %s to heat.", yname(obj));
		}
		res = MOVE_PARTIAL;
	break;
	case MASS_SHADOW_PISTOL:
		res = use_massblaster(obj);
	break;
	case ARM_BLASTER:
	case ASSAULT_RIFLE:
		/* Switch between WP_MODE_SINGLE, WP_MODE_BURST and WP_MODE_AUTO */

		if (obj->altmode == WP_MODE_AUTO) {
			obj->altmode = WP_MODE_BURST;
		} else if (obj->altmode == WP_MODE_BURST) {
			obj->altmode = WP_MODE_SINGLE;
		} else {
			obj->altmode = WP_MODE_AUTO;
		}
		res = MOVE_PARTIAL;
		
		You("switch %s to %s mode.", yname(obj), 
			((obj->altmode == WP_MODE_SINGLE) ? "semi-automatic" : 
			 ((obj->altmode == WP_MODE_BURST) ? "burst" :
			  "full automatic")));
		break;	
	case BFG:
		if (obj->altmode == WP_MODE_AUTO) obj-> altmode = WP_MODE_BURST;
		else obj->altmode = WP_MODE_AUTO;
		res = MOVE_PARTIAL;
		You("switch %s to %s mode.", yname(obj), 
			(obj->altmode ? "burst" : "full automatic"));
		break;
	case AUTO_SHOTGUN:
	case SUBMACHINE_GUN:
		if (obj->altmode == WP_MODE_AUTO) obj-> altmode = WP_MODE_SINGLE;
		else obj->altmode = WP_MODE_AUTO;
		res = MOVE_PARTIAL;
		You("switch %s to %s mode.", yname(obj), 
			(obj->altmode ? "semi-automatic" : "full automatic"));
		break;
	case FRAG_GRENADE:
	case GAS_GRENADE:
		if (!obj->oarmed) {
			You("arm %s.", yname(obj));
			arm_bomb(obj, TRUE);
			res = MOVE_PARTIAL;
		} else {
			pline("It's already armed!");
			res = MOVE_CANCELLED;
		}
		break;
	case STICK_OF_DYNAMITE:
		light_cocktail(obj);
		break;
//endif
	case CLOCKWORK_COMPONENT:
	case SUBETHAIC_COMPONENT:
	case HELLFIRE_COMPONENT:
	case SCRAP:
		res = doUseComponents(&obj);
	break;
	case UPGRADE_KIT:
		res = doUseUpgradeKit(&obj);
		check_loadout_trophy();
	break;
	default:
		/* Pole-weapons can strike at a distance */
		if (is_pole(obj)) {
			res = use_pole(obj);
			break;
		} else if (is_pick(obj) || is_axe(obj)) {
			res = use_pick_axe(obj);
			break;
		}
		pline("Sorry, I don't know how to use that.");
	xit:
		nomul(0, NULL);
		return MOVE_CANCELLED;
	}
	if(!(res == MOVE_CANCELLED) && !waslabile && obj && obj->oartifact) arti_speak(obj);
xit2:
	nomul(0, NULL);
	return res;
}

/* Keep track of unfixable troubles for purposes of messages saying you feel
 * great.
 */
int
unfixable_trouble_count(is_horn)
	boolean is_horn;
{
	int unfixable_trbl = 0;

	if (Stoned) unfixable_trbl++;
	if (Golded) unfixable_trbl++;
	if (Strangled) unfixable_trbl++;
	if (Panicking) unfixable_trbl++;
	if (StumbleBlind) unfixable_trbl++;
	if (StaggerShock) unfixable_trbl++;
	if (Babble) unfixable_trbl++;
	if (Screaming) unfixable_trbl++;
	if (FaintingFits) unfixable_trbl++;
	if (Wounded_legs
#ifdef STEED
		    && !u.usteed
#endif
				) unfixable_trbl++;
	if (Slimed) unfixable_trbl++;
	if (FrozenAir) unfixable_trbl++;
	if (BloodDrown) unfixable_trbl++;
	/* lycanthropy is not desirable, but it doesn't actually make you feel
	   bad */

	/* we'll assume that intrinsic stunning from being a bat/stalker
	   doesn't make you feel bad */
	if (!is_horn) {
	    if (Confusion) unfixable_trbl++;
	    if (Sick) unfixable_trbl++;
	    if (HHallucination) unfixable_trbl++;
	    if (Vomiting) unfixable_trbl++;
	    if (HStun) unfixable_trbl++;
	}
	return unfixable_trbl;
}

int
dotrephination_menu()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Extract what?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	incntlet = 'a';
	
	if (u.thoughts&CLOCKWISE_METAMORPHOSIS){
		Sprintf(buf, "Extract clockwise gyre");
		any.a_int = CLOCKWISE_METAMORPHOSIS_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&ANTI_CLOCKWISE_METAMORPHOSIS){
		Sprintf(buf, "Extract anti-clockwise gyre");
		any.a_int = ANTI_CLOCKWISE_METAMORPHOSIS_G;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&ARCANE_BULWARK){
		Sprintf(buf, "Extract memory of a sparking lake-shore");
		any.a_int = SPARKLING_LAKE_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&DISSIPATING_BULWARK){
		Sprintf(buf, "Extract memory of a pure lake-shore");
		any.a_int = FADING_LAKE_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&SMOLDERING_BULWARK){
		Sprintf(buf, "Extract memory of embers drowning in still water");
		any.a_int = SMOKING_LAKE_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&FROSTED_BULWARK){
		Sprintf(buf, "Extract memory of snowflakes on a lake");
		any.a_int = FROSTED_LAKE_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&BLOOD_RAPTURE){
		Sprintf(buf, "Extract memory of blood-mist rainbows");
		any.a_int = RAPTUROUS_EYE_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&CLAWMARK){
		Sprintf(buf, "Extract memory of clawmarks");
		any.a_int = CLAWMARK_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&CLEAR_DEEPS){
		Sprintf(buf, "Extract the deep blue waters");
		any.a_int = CLEAR_SEA_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&DEEP_SEA){
		Sprintf(buf, "Extract the pitch-black waters");
		any.a_int = DEEP_SEA_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&TRANSPARENT_SEA){
		Sprintf(buf, "Extract the perfectly clear sea");
		any.a_int = HIDDEN_SEA_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&COMMUNION){
		Sprintf(buf, "Extract memory of the strange minister's sermon");
		any.a_int = COMMUNION_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&CORRUPTION){
		Sprintf(buf, "Extract the bloody tears");
		any.a_int = CORRUPTION_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&EYE_THOUGHT){
		Sprintf(buf, "Extract the writhing eyes");
		any.a_int = EYE_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&FORMLESS_VOICE){
		Sprintf(buf, "Extract sound of the great voice");
		any.a_int = FORMLESS_VOICE_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&GUIDANCE){
		Sprintf(buf, "Extract the sight of the dancing sprites");
		any.a_int = GUIDANCE_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&IMPURITY){
		Sprintf(buf, "Extract the red millipedes and their filth");
		any.a_int = IMPURITY_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&MOON){
		Sprintf(buf, "Extract memory of the face of the moon");
		any.a_int = MOON_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&WRITHE){
		Sprintf(buf, "Extract subtle mucus from your brain");
		any.a_int = WRITHE_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (u.thoughts&RADIANCE){
		Sprintf(buf, "Extract the golden pyramid");
		any.a_int = RADIANCE_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	
	if (u.thoughts&BEASTS_EMBRACE){
		Sprintf(buf, "Extract the bestial figure");
		any.a_int = BEAST_S_EMBRACE_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	
	if (u.thoughts&SIGHT){
		Sprintf(buf, "Extract the recursive eye");
		any.a_int = ORRERY_GLYPH;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	
	end_menu(tmpwin, "Pick thought to extract");

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

STATIC_OVL int
dotrephination_options()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Use kit on what?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	incntlet = 'a';
	
	Sprintf(buf, "Your brain");
	any.a_int = TREPH_THOUGHTS;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet++;

	Sprintf(buf, "Your skulls");
	any.a_int = TREPH_CRYSTALS;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet++;
	
	end_menu(tmpwin, "Pick patient");

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

//Returns 0 if this is the first time its called this round, 1 otherwise.
int
partial_action()
{
	int res = (moves == u.last_used_move) &&
	      (youmonst.movement == u.last_used_movement);
	u.last_used_move = moves;
	u.last_used_movement = youmonst.movement;
	return res;
}

//Returns returns 1 if the partial_action() has been used this round.
int
check_partial_action()
{
	return ((moves == u.last_used_move) &&
	      (youmonst.movement == u.last_used_movement));
}

#endif /* OVLB */

/*apply.c*/
