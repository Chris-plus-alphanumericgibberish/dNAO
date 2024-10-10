/*	SCCS Id: @(#)fountain.c	3.4	2003/03/23	*/
/*	Copyright Scott R. Turner, srt@ucla, 10/27/86 */
/* NetHack may be freely redistributed.  See license for details. */

/* Code for drinking from fountains. */

#include "hack.h"
#include "artifact.h"


STATIC_DCL void NDECL(dowatersnakes);
STATIC_DCL void NDECL(dowaterdemon);
STATIC_DCL void NDECL(dowaternymph);
STATIC_DCL void NDECL(dolavademon);
STATIC_PTR void FDECL(gush, (int,int,genericptr_t));
STATIC_DCL void NDECL(dofindgem);
STATIC_DCL void FDECL(blowupforge, (int, int));

void
floating_above(what)
const char *what;
{
    You("are floating high above the %s.", what);
}

STATIC_OVL void
dowatersnakes() /* Fountain of snakes! */
{
    register int num = rn1(5,2);
    struct monst *mtmp;

    if (!(mvitals[PM_WATER_MOCCASIN].mvflags & G_GONE && !In_quest(&u.uz))) {
	if (!Blind)
	    pline("An endless stream of %s pours forth!",
		  Hallucination ? makeplural(rndmonnam()) : "snakes");
	else
	    You_hear("%s hissing!", something);
	while(num-- > 0)
	    if((mtmp = makemon(&mons[PM_WATER_MOCCASIN],
			u.ux, u.uy, NO_MM_FLAGS)) && t_at(mtmp->mx, mtmp->my))
		(void) mintrap(mtmp);
    } else
	pline_The("fountain bubbles furiously for a moment, then calms.");
}

STATIC_OVL
void
dowaterdemon() /* Water demon */
{
    register struct monst *mtmp;

    if(!(mvitals[PM_MARID].mvflags & G_GONE && !In_quest(&u.uz))) {
	if((mtmp = makemon(&mons[PM_MARID],u.ux,u.uy, NO_MM_FLAGS))) {
	    if (!Blind)
		You("unleash %s!", a_monnam(mtmp));
	    else
		You_feel("the presence of evil.");

	/* Give those on low levels a (slightly) better chance of survival */
	    if (rnd(100) > (80 + level_difficulty())) {
		pline("Grateful for %s release, %s grants you a wish!",
		      mhis(mtmp), mhe(mtmp));
		makewish(allow_artwish() | WISH_VERBOSE);
		mongone(mtmp);
	    } else if (t_at(mtmp->mx, mtmp->my))
		(void) mintrap(mtmp);
	}
    } else
	pline_The("fountain bubbles furiously for a moment, then calms.");
}

/* Lava Demon */
STATIC_OVL void
dolavademon()
{
    struct monst *mtmp;

    if (!(mvitals[PM_LAVA_DEMON].mvflags & G_GONE)) {
        if ((mtmp = makemon(&mons[PM_LAVA_DEMON], u.ux, u.uy,
                            MM_ADJACENTOK)) != 0) {
            if (!Blind)
                You("summon %s!", a_monnam(mtmp));
            else
                You_feel("the temperature rise significantly.");

            /* Give those on low levels a (slightly) better chance of survival
             */
            if (rnd(100) > (80 + level_difficulty())) {
                pline("Freed from the depths of Gehennom, %s offers to aid you in your quest!",
                      mhe(mtmp));
                (void) tamedog_core(mtmp, (struct obj *) 0, TRUE);
            } else if (t_at(mtmp->mx, mtmp->my))
                (void) mintrap(mtmp);
        }
    } else
        pline_The("forge violently spews lava for a moment, then settles.");
}

STATIC_OVL void
dowaternymph() /* Water Nymph */
{
	register struct monst *mtmp;

	if(!(mvitals[PM_NAIAD].mvflags & G_GONE && !In_quest(&u.uz)) &&
	   (mtmp = makemon(&mons[PM_NAIAD],u.ux,u.uy, NO_MM_FLAGS))) {
		if (!Blind)
		   You("attract %s!", a_monnam(mtmp));
		else
		   You_hear("a seductive voice.");
		mtmp->msleeping = 0;
		if (t_at(mtmp->mx, mtmp->my))
		    (void) mintrap(mtmp);
	} else
		if (!Blind)
		   pline("A large bubble rises to the surface and pops.");
		else
		   You_hear("a loud pop.");
}

void
dogushforth(drinking) /* Gushing forth along LOS from (u.ux, u.uy) */
int drinking;
{
	int madepool = 0;

	do_clear_area(u.ux, u.uy, 7, gush, (genericptr_t)&madepool);
	if (!madepool) {
	    if (drinking)
		Your("thirst is quenched.");
	    else
		pline("Water sprays all over you.");
	}
}

STATIC_PTR void
gush(x, y, poolcnt)
int x, y;
genericptr_t poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;

	if (((x+y)%2) || (x == u.ux && y == u.uy) ||
	    (rn2(1 + distmin(u.ux, u.uy, x, y)))  ||
	    (levl[x][y].typ != ROOM) ||
	    (boulder_at(x, y)) || nexttodoor(x, y))
		return;

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	if (!((*(int *)poolcnt)++))
	    pline("Water gushes forth from the overflowing fountain!");

	/* Put a puddle at x, y */
	levl[x][y].typ = PUDDLE;
	del_engr_ward_at(x, y);
	water_damage(level.objects[x][y], FALSE, TRUE, FALSE, (struct monst *) 0);

	if ((mtmp = m_at(x, y)) != 0)
		(void) minliquid(mtmp);
	else
		newsym(x,y);
}

STATIC_OVL void
dofindgem() /* Find a gem in the sparkling waters. */
{
	if (!Blind) You("spot a gem in the sparkling waters!");
	else You_feel("a gem here!");
	(void) mksobj_at(rnd_class(DILITHIUM_CRYSTAL, LUCKSTONE-1),
			 u.ux, u.uy, MKOBJ_NOINIT);
	SET_FOUNTAIN_LOOTED(u.ux,u.uy);
	newsym(u.ux, u.uy);
	exercise(A_WIS, TRUE);			/* a discovery! */
}

void
dryup(x, y, isyou)
xchar x, y;
boolean isyou;
{
	if (IS_FOUNTAIN(levl[x][y].typ) &&
	    (!rn2(3) || FOUNTAIN_IS_WARNED(x,y))) {
		if(isyou && in_town(x, y) && !FOUNTAIN_IS_WARNED(x,y)) {
			struct monst *mtmp;
			SET_FOUNTAIN_WARNED(x,y);
			/* Warn about future fountain use. */
			for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
			    if (DEADMONSTER(mtmp)) continue;
			    if ((mtmp->mtyp == PM_WATCHMAN ||
				mtmp->mtyp == PM_WATCH_CAPTAIN) &&
			       couldsee(mtmp->mx, mtmp->my) &&
			       mtmp->mpeaceful) {
				pline("%s yells:", Amonnam(mtmp));
				verbalize("Hey, stop using that fountain!");
				break;
			    }
			}
			/* You can see or hear this effect */
			if(!mtmp) pline_The("flow reduces to a trickle.");
			return;
		}
#ifdef WIZARD
		if (isyou && wizard) {
			if (yn("Dry up fountain?") == 'n')
				return;
		}
#endif
		/* replace the fountain with ordinary floor */
		levl[x][y].typ = ROOM;
		levl[x][y].looted = 0;
		levl[x][y].blessedftn = 0;
		if (cansee(x,y)) pline_The("fountain dries up!");
		/* The location is seen if the hero/monster is invisible */
		/* or felt if the hero is blind.			 */
		newsym(x, y);
		level.flags.nfountains--;
		if(isyou && in_town(x, y))
		    (void) angry_guards(FALSE);
		
		if(isyou && u.sealsActive&SEAL_EDEN) unbind(SEAL_EDEN,TRUE);
	}
}

int
smithing_bonus(){
	switch(P_SKILL(P_SMITHING)){
		case P_BASIC:
			return 1;
		break;
		case P_SKILLED:
			return 2;
		break;
		case P_EXPERT:
			return 5;
		break;
	}
	return 0;
}

boolean
have_blood_smithing_crystal(struct obj * obj)
{
	return !!find_blood_smithing_crystal(obj);
}

struct obj *
find_blood_smithing_crystal(struct obj * obj)
{
	struct obj *otmp;
	for(otmp = invent; otmp; otmp = otmp->nobj){
		if(otmp->otyp == CRYSTAL && otmp->obj_material == HEMARGYOS && otmp->spe == 1+(obj->spe/3) && obj->spe < 10)
			break;
	}
	return otmp;
}

boolean
have_blood_smithing_fire(struct obj * obj)
{
	return have_blood_smithing_x(obj, OPROP_FIREW);
}

boolean
have_blood_smithing_cold(struct obj * obj)
{
	return have_blood_smithing_x(obj, OPROP_COLDW);
}

boolean
have_blood_smithing_lightning(struct obj * obj)
{
	return have_blood_smithing_x(obj, OPROP_ELECW);
}

boolean
have_blood_smithing_acid(struct obj * obj)
{
	return have_blood_smithing_x(obj, OPROP_ACIDW);
}

boolean
have_blood_smithing_magic(struct obj * obj)
{
	return have_blood_smithing_x(obj, OPROP_MAGCW);
}

boolean
have_blood_smithing_buc(struct obj * obj)
{
	return have_blood_smithing_x(obj, OPROP_HOLYW) || have_blood_smithing_x(obj, OPROP_UNHYW);
}

boolean
have_blood_smithing_x(struct obj * obj, int oprop)
{
	int oprop_lesser = 0;
	int spell = 0;
	if(oprop == OPROP_FIREW){
		oprop_lesser = OPROP_LESSER_FIREW;
		spell = SPE_FIREBALL;
	}
	else if(oprop == OPROP_COLDW){
		oprop_lesser = OPROP_LESSER_COLDW;
		spell = SPE_CONE_OF_COLD;
	}
	else if(oprop == OPROP_ELECW){
		oprop_lesser = OPROP_LESSER_ELECW;
		spell = SPE_LIGHTNING_BOLT;
	}
	else if(oprop == OPROP_ACIDW){
		oprop_lesser = OPROP_LESSER_ACIDW;
		spell = SPE_ACID_SPLASH;
	}
	else if(oprop == OPROP_MAGCW){
		oprop_lesser = OPROP_LESSER_MAGCW;
		spell = SPE_MAGIC_MISSILE;
	}
	else if(oprop == OPROP_HOLYW){
		oprop_lesser = OPROP_LESSER_HOLYW;
		spell = SPE_REMOVE_CURSE;
	}
	else if(oprop == OPROP_UNHYW){
		oprop_lesser = OPROP_LESSER_UNHYW;
		spell = SPE_REMOVE_CURSE;
	}

	if(obj->oartifact && !is_malleable_artifact(&artilist[obj->oartifact]))
		return FALSE;

	if(obj_is_material(obj, MERCURIAL))
		return FALSE;

	if(check_oprop(obj, oprop) || (oprop_lesser && check_oprop(obj, oprop_lesser)))
		return FALSE;
	if(check_oprop(obj, OPROP_SMITHU) && oprop != OPROP_HOLYW && oprop != OPROP_UNHYW)
		return FALSE;
	int j;
	for (j = 0; j < MAXSPELL; j++)
		if (spellid(j) == spell && spellknow(j) > 0)
			break;
	if(j < MAXSPELL)
		return TRUE;

	struct obj *otmp;
	for(otmp = invent; otmp; otmp = otmp->nobj){
		if(otmp->otyp == CRYSTAL && check_oprop(otmp, oprop))
			break;
	}
	if(otmp)
		return TRUE;

	return FALSE;
}

boolean
get_blood_smithing_x(int oprop, struct obj **crystal, int *spellnum)
{
	int spell = 0;
	if(oprop == OPROP_FIREW){
		spell = SPE_FIREBALL;
	}
	else if(oprop == OPROP_COLDW){
		spell = SPE_CONE_OF_COLD;
	}
	else if(oprop == OPROP_ELECW){
		spell = SPE_LIGHTNING_BOLT;
	}
	else if(oprop == OPROP_ACIDW){
		spell = SPE_ACID_SPLASH;
	}
	else if(oprop == OPROP_MAGCW){
		spell = SPE_MAGIC_MISSILE;
	}
	else if(oprop == OPROP_HOLYW){
		spell = SPE_REMOVE_CURSE;
	}
	else if(oprop == OPROP_UNHYW){
		spell = SPE_REMOVE_CURSE;
	}

	struct obj *otmp;
	for(otmp = invent; otmp; otmp = otmp->nobj){
		if(otmp->otyp == CRYSTAL && check_oprop(otmp, oprop))
			break;
	}
	if(otmp){
		*crystal = otmp;
		return TRUE;
	}

	int j;
	for (j = 0; j < MAXSPELL; j++)
		if (spellid(j) == spell && spellknow(j) > 0)
			break;
	if(j < MAXSPELL){
		*spellnum = j;
		return TRUE;
	}

	return FALSE;
}

void
smithing_object(struct obj *obj)
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet;
	menu_item *selected;
	anything any;
	int selection;
	boolean upgradeable = obj->oclass == WEAPON_CLASS || is_weptool(obj) || obj->oclass == ARMOR_CLASS;
	boolean upgrade_gem_required; 

	while(TRUE){
		tmpwin = create_nhwindow(NHW_MENU);
		start_menu(tmpwin);
		any.a_void = 0;		/* zero out all bits */
		n = 0;
		upgrade_gem_required = (obj->spe >= smithing_bonus()) || (!is_metallic(obj) && obj->oclass == ARMOR_CLASS);

		// Sprintf(buf, "Functions");
		// add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);

		if(is_metallic(obj) && (obj->spe <= 0 || obj->spe < smithing_bonus()) && check_oprop(obj, OPROP_NONE) && !obj->oartifact){
			n++;
			incntlet = 'm';
			Sprintf(buf, "Melt for scrap");
			any.a_int = 1;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		if (is_metallic(obj) && greatest_erosion(obj) > 0) {
			n++;
			incntlet = 'r';
			Sprintf(buf, "Repair damage");
			any.a_int = 2;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		else if(upgradeable && (!upgrade_gem_required || (u.ublood_smithing && have_blood_smithing_crystal(obj)))){
			n++;
			incntlet = 'i';
			Sprintf(buf, "Improve %s", xname(obj));
			any.a_int = 3;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		if((obj->oclass == WEAPON_CLASS || is_weptool(obj))
			&& (u.ublood_smithing && have_blood_smithing_fire(obj))
		){
			n++;
			incntlet = 'f';
			Sprintf(buf, "Imbue %s with fire", xname(obj));
			any.a_int = 4;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		if((obj->oclass == WEAPON_CLASS || is_weptool(obj))
			&& (u.ublood_smithing && have_blood_smithing_cold(obj))
		){
			n++;
			incntlet = 'c';
			Sprintf(buf, "Imbue %s with cold", xname(obj));
			any.a_int = 5;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		if((obj->oclass == WEAPON_CLASS || is_weptool(obj))
			&& (u.ublood_smithing && have_blood_smithing_lightning(obj))
		){
			n++;
			incntlet = 'l';
			Sprintf(buf, "Imbue %s with lightning", xname(obj));
			any.a_int = 6;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		if((obj->oclass == WEAPON_CLASS || is_weptool(obj))
			&& (u.ublood_smithing && have_blood_smithing_acid(obj))
		){
			n++;
			incntlet = 'a';
			Sprintf(buf, "Imbue %s with acid", xname(obj));
			any.a_int = 7;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		if((obj->oclass == WEAPON_CLASS || is_weptool(obj))
			&& (u.ublood_smithing && have_blood_smithing_magic(obj))
		){
			n++;
			incntlet = 'g';
			Sprintf(buf, "Imbue %s with magic", xname(obj));
			any.a_int = 8;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		if((obj->oclass == WEAPON_CLASS || is_weptool(obj))
			&& (u.ublood_smithing && have_blood_smithing_buc(obj))
		){
			n++;
			incntlet = 's';
			Sprintf(buf, "Imbue %s with spiritual energy", xname(obj));
			any.a_int = 9;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		if(obj->otyp == CRYSTAL && obj_is_material(obj, HEMARGYOS) && obj->spe > 1){
			n++;
			incntlet = 'b';
			Sprintf(buf, "Break down into smaller crystals");
			any.a_int = 10;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		if(check_oprop(obj, OPROP_SMITHU)){
			n++;
			incntlet = 'e';
			Sprintf(buf, "Extract energy from %s", xname(obj));
			any.a_int = 11;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				incntlet, 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		if(!n){
			destroy_nhwindow(tmpwin);
			return;
		}

		Sprintf(buf, "Smithing %s:", doname(obj));
		end_menu(tmpwin, buf);

		how = PICK_ONE;
		n = select_menu(tmpwin, how, &selected);
		destroy_nhwindow(tmpwin);

		if(n > 0){
			selection = selected[0].item.a_int;
			free(selected);
		}
		else return;
		switch(selection){
			case 1:{
				struct obj *ingots = mksobj(INGOT, MKOBJ_NOINIT);
				ingots->quan = obj->owt;
				ingots->dknown = ingots->known = ingots->rknown = ingots->sknown = TRUE;
				ingots->bknown = obj->bknown;
				ingots->blessed = obj->blessed;
				ingots->cursed = obj->cursed;
				set_material_gm(ingots, obj->obj_material);
				fix_object(ingots);
				useupall(obj);
				hold_another_object(ingots, u.uswallow ?
						   "Oops!  %s out of your reach!" :
						(Weightless ||
						 Is_waterlevel(&u.uz) ||
						 levl[u.ux][u.uy].typ < IRONBARS ||
						 levl[u.ux][u.uy].typ >= ICE) ?
							   "Oops!  %s away from you!" :
							   "Oops!  %s to the floor!",
							   The(aobjnam(ingots, "slip")),
							   (const char *)0);
				exercise(A_STR, TRUE);
				exercise(A_STR, TRUE);
				exercise(A_STR, TRUE);
				return;
			}break;
			case 2:
				if (!Blind)
					You("successfully reforge your %s, repairing %sthe damage.",
						xname(obj),
						(obj->oeroded + obj->oeroded2) > 1 ? "some of " : ""
					);
				if (obj->oeroded > 0)
					obj->oeroded--;
				else if (obj->oeroded2 > 0)
					obj->oeroded2--;
				use_skill(P_SMITHING, 1);
				exercise(A_DEX, TRUE);
			break;
			case 3:
				if(!upgrade_gem_required){
					obj->spe++;
					if(obj->otyp == CHURCH_HAMMER || obj->otyp == CHURCH_BLADE)
						if(obj->cobj && obj->spe > obj->cobj->spe) /*Note: base obj already incremented*/
							obj->cobj->spe++;
					You("successfully improve your %s.", xname(obj));
					use_skill(P_SMITHING, 1);
					exercise(A_DEX, TRUE);
					exercise(A_STR, TRUE);
				}
				else {
					if(u.ublood_smithing && have_blood_smithing_crystal(obj)){
						struct obj *crystal = find_blood_smithing_crystal(obj);
						obj->blood_smithed = TRUE;
						obj->spe++;
						if(obj->otyp == CHURCH_HAMMER || obj->otyp == CHURCH_BLADE)
							if(obj->cobj && obj->spe > obj->cobj->spe){ /*Note: base obj already incremented*/
								obj->cobj->spe++;
								obj->cobj->blood_smithed = TRUE;
							}
						You("successfully improve your %s using %s.", xname(obj), an(singular(crystal, xname)));
						if(Role_if(PM_UNDEAD_HUNTER)){
							if(crystal->spe == 2 && OLD_P_MAX_SKILL(P_SMITHING) < P_SKILLED){
								pline("The knowledge from the crystal sinks into your subconscious.");
								skilled_weapon_skill(P_SMITHING);
							}
							else if(crystal->spe > 2 && OLD_P_MAX_SKILL(P_SMITHING) < P_EXPERT){
								pline("The knowledge from the crystal sinks into your subconscious.");
								expert_weapon_skill(P_SMITHING);
							}
						}
						useup(crystal);
						use_skill(P_SMITHING, crystal->spe);
						exercise(A_STR, TRUE);
						exercise(A_DEX, TRUE);
						exercise(A_INT, TRUE);
						exercise(A_CHA, TRUE);
					}
				}
			break;
			case 4:{
				int spellnum = 0;
				struct obj *crystal = (struct obj *) 0;
				get_blood_smithing_x(OPROP_FIREW, &crystal, &spellnum);
				if(crystal){
					useup(crystal);
				}
				else if(spellnum < MAXSPELL){
					percdecrnknow(spellnum, 100);
				}
				if(is_full_insight_weapon(obj))
					add_oprop(obj, OPROP_LESSER_FIREW);
				else
					add_oprop(obj, OPROP_FIREW);
				add_oprop(obj, OPROP_INSTW);
				add_oprop(obj, OPROP_SMITHU);
				use_skill(P_SMITHING, 4);
			}
			break;
			case 5:{
				int spellnum = 0;
				struct obj *crystal = (struct obj *) 0;
				get_blood_smithing_x(OPROP_COLDW, &crystal, &spellnum);

				if(crystal){
					useup(crystal);
				}
				else if(spellnum < MAXSPELL){
					percdecrnknow(spellnum, 100);
				}
				if(is_full_insight_weapon(obj))
					add_oprop(obj, OPROP_LESSER_COLDW);
				else
					add_oprop(obj, OPROP_COLDW);
				add_oprop(obj, OPROP_INSTW);
				add_oprop(obj, OPROP_SMITHU);
				use_skill(P_SMITHING, 4);
			}
			break;
			case 6:{
				int spellnum = 0;
				struct obj *crystal = (struct obj *) 0;
				get_blood_smithing_x(OPROP_ELECW, &crystal, &spellnum);

				if(crystal){
					useup(crystal);
				}
				else if(spellnum < MAXSPELL){
					percdecrnknow(spellnum, 100);
				}
				if(is_full_insight_weapon(obj))
					add_oprop(obj, OPROP_LESSER_ELECW);
				else
					add_oprop(obj, OPROP_ELECW);
				add_oprop(obj, OPROP_INSTW);
				add_oprop(obj, OPROP_SMITHU);
				use_skill(P_SMITHING, 5);
			}
			break;
			case 7:{
				int spellnum = 0;
				struct obj *crystal = (struct obj *) 0;
				get_blood_smithing_x(OPROP_ACIDW, &crystal, &spellnum);

				if(crystal){
					useup(crystal);
				}
				else if(spellnum < MAXSPELL){
					percdecrnknow(spellnum, 100);
				}
				if(is_full_insight_weapon(obj))
					add_oprop(obj, OPROP_LESSER_ACIDW);
				else
					add_oprop(obj, OPROP_ACIDW);
				add_oprop(obj, OPROP_INSTW);
				add_oprop(obj, OPROP_SMITHU);
				use_skill(P_SMITHING, 4);
			}
			break;
			case 8:{
				int spellnum = 0;
				struct obj *crystal = (struct obj *) 0;
				get_blood_smithing_x(OPROP_MAGCW, &crystal, &spellnum);

				if(crystal){
					useup(crystal);
				}
				else if(spellnum < MAXSPELL){
					percdecrnknow(spellnum, 100);
				}
				if(is_full_insight_weapon(obj))
					add_oprop(obj, OPROP_LESSER_MAGCW);
				else
					add_oprop(obj, OPROP_MAGCW);
				add_oprop(obj, OPROP_INSTW);
				add_oprop(obj, OPROP_SMITHU);
				use_skill(P_SMITHING, 2);
			}
			break;
			case 9:{
				int spellnum = 0;
				struct obj *crystal = (struct obj *) 0;
				get_blood_smithing_x(OPROP_HOLYW, &crystal, &spellnum);

				if(crystal){
					useup(crystal);
				}
				else if(spellnum < MAXSPELL){
					percdecrnknow(spellnum, 100);
				}
				if(is_full_insight_weapon(obj)){
					add_oprop(obj, OPROP_LESSER_HOLYW);
					add_oprop(obj, OPROP_LESSER_UNHYW);
				}
				else {
					add_oprop(obj, OPROP_HOLYW);
					add_oprop(obj, OPROP_UNHYW);
				}
				add_oprop(obj, OPROP_INSTW);
				use_skill(P_SMITHING, 5);
			}
			break;
			case 10:{
				struct obj *new;
				if(obj->spe == 2){
					You("split the twin columnar crystals appart.");
					new = mksobj(CRYSTAL, MKOBJ_NOINIT);
					new->spe = 1;
					new->quan = 2;
					set_material_gm(new, HEMARGYOS);
					fix_object(new);
					useup(obj);
					hold_another_object(new, u.uswallow ?
							   "Oops!  %s out of your reach!" :
							(Weightless ||
							 Is_waterlevel(&u.uz) ||
							 levl[u.ux][u.uy].typ < IRONBARS ||
							 levl[u.ux][u.uy].typ >= ICE) ?
								   "Oops!  %s away from you!" :
								   "Oops!  %s to the floor!",
								   The(aobjnam(new, "slip")),
								   (const char *)0);
					exercise(A_DEX, TRUE);
					use_skill(P_SMITHING, 2);
				}
				else if(obj->spe == 3){
					You("break up the chunk of columnar crystals.");
					int n = 2+rn2(2);
					new = mksobj(CRYSTAL, MKOBJ_NOINIT);
					new->spe = 2;
					new->quan = n;
					set_material_gm(new, HEMARGYOS);
					fix_object(new);
					useup(obj);
					hold_another_object(new, u.uswallow ?
							   "Oops!  %s out of your reach!" :
							(Weightless ||
							 Is_waterlevel(&u.uz) ||
							 levl[u.ux][u.uy].typ < IRONBARS ||
							 levl[u.ux][u.uy].typ >= ICE) ?
								   "Oops!  %s away from you!" :
								   "Oops!  %s to the floor!",
								   The(aobjnam(new, "slip")),
								   (const char *)0);
					n = 4-n;
					new = mksobj(CRYSTAL, MKOBJ_NOINIT);
					new->spe = 1;
					new->quan = 2*n;
					set_material_gm(new, HEMARGYOS);
					fix_object(new);
					hold_another_object(new, u.uswallow ?
							   "Oops!  %s out of your reach!" :
							(Weightless ||
							 Is_waterlevel(&u.uz) ||
							 levl[u.ux][u.uy].typ < IRONBARS ||
							 levl[u.ux][u.uy].typ >= ICE) ?
								   "Oops!  %s away from you!" :
								   "Oops!  %s to the floor!",
								   The(aobjnam(new, "slip")),
								   (const char *)0);
					exercise(A_DEX, TRUE);
					exercise(A_STR, TRUE);
					use_skill(P_SMITHING, 4);
				}
				else if(obj->spe == 4){
					You("break up the mass of columnar crystals.");
					new = mksobj(CRYSTAL, MKOBJ_NOINIT);
					//2 or more chunks
					int c = 2+rn2(3);
					new->spe = 3;
					new->quan = c;
					set_material_gm(new, HEMARGYOS);
					fix_object(new);
					useup(obj);
					hold_another_object(new, u.uswallow ?
							   "Oops!  %s out of your reach!" :
							(Weightless ||
							 Is_waterlevel(&u.uz) ||
							 levl[u.ux][u.uy].typ < IRONBARS ||
							 levl[u.ux][u.uy].typ >= ICE) ?
								   "Oops!  %s away from you!" :
								   "Oops!  %s to the floor!",
								   The(aobjnam(new, "slip")),
								   (const char *)0);
					//4 or more twins
					int n = 2+d(2,3);
					new = mksobj(CRYSTAL, MKOBJ_NOINIT);
					new->spe = 2;
					new->quan = n + (4-c)*2;
					set_material_gm(new, HEMARGYOS);
					fix_object(new);
					hold_another_object(new, u.uswallow ?
							   "Oops!  %s out of your reach!" :
							(Weightless ||
							 Is_waterlevel(&u.uz) ||
							 levl[u.ux][u.uy].typ < IRONBARS ||
							 levl[u.ux][u.uy].typ >= ICE) ?
								   "Oops!  %s away from you!" :
								   "Oops!  %s to the floor!",
								   The(aobjnam(new, "slip")),
								   (const char *)0);
					//8 or more singles
					n = 12-n;
					new = mksobj(CRYSTAL, MKOBJ_NOINIT);
					new->spe = 1;
					new->quan = 2*n;
					set_material_gm(new, HEMARGYOS);
					fix_object(new);
					hold_another_object(new, u.uswallow ?
							   "Oops!  %s out of your reach!" :
							(Weightless ||
							 Is_waterlevel(&u.uz) ||
							 levl[u.ux][u.uy].typ < IRONBARS ||
							 levl[u.ux][u.uy].typ >= ICE) ?
								   "Oops!  %s away from you!" :
								   "Oops!  %s to the floor!",
								   The(aobjnam(new, "slip")),
								   (const char *)0);
					exercise(A_DEX, TRUE);
					exercise(A_STR, TRUE);
					exercise(A_STR, TRUE);
					use_skill(P_SMITHING, 8);
				}
				return;
			}
			break;
			case 11:{
				int count_energy = 0;
				long latest_oprop = 0;
				long latest_energy = 0;
				if(check_oprop(obj, OPROP_FIREW) && !is_full_insight_weapon(obj)){
					count_energy++;
					latest_oprop = OPROP_FIREW;
					latest_energy = OPROP_FIREW;
				}
				else if(check_oprop(obj, OPROP_LESSER_FIREW) && is_full_insight_weapon(obj)){
					count_energy++;
					latest_oprop = OPROP_LESSER_FIREW;
					latest_energy = OPROP_FIREW;
				}

				if(check_oprop(obj, OPROP_COLDW) && !is_full_insight_weapon(obj)){
					count_energy++;
					latest_oprop = OPROP_COLDW;
					latest_energy = OPROP_COLDW;
				}
				else if(check_oprop(obj, OPROP_LESSER_COLDW) && is_full_insight_weapon(obj)){
					count_energy++;
					latest_oprop = OPROP_LESSER_COLDW;
					latest_energy = OPROP_COLDW;
				}

				if(check_oprop(obj, OPROP_ELECW) && !is_full_insight_weapon(obj)){
					count_energy++;
					latest_oprop = OPROP_ELECW;
					latest_energy = OPROP_ELECW;
				}
				else if(check_oprop(obj, OPROP_LESSER_ELECW) && is_full_insight_weapon(obj)){
					count_energy++;
					latest_oprop = OPROP_LESSER_ELECW;
					latest_energy = OPROP_ELECW;
				}

				if(check_oprop(obj, OPROP_ACIDW) && !is_full_insight_weapon(obj)){
					count_energy++;
					latest_oprop = OPROP_ACIDW;
					latest_energy = OPROP_ACIDW;
				}
				else if(check_oprop(obj, OPROP_LESSER_ACIDW) && is_full_insight_weapon(obj)){
					count_energy++;
					latest_oprop = OPROP_LESSER_ACIDW;
					latest_energy = OPROP_ACIDW;
				}

				if(check_oprop(obj, OPROP_MAGCW) && !is_full_insight_weapon(obj)){
					count_energy++;
					latest_oprop = OPROP_MAGCW;
					latest_energy = OPROP_MAGCW;
				}
				else if(check_oprop(obj, OPROP_LESSER_MAGCW) && is_full_insight_weapon(obj)){
					count_energy++;
					latest_oprop = OPROP_LESSER_MAGCW;
					latest_energy = OPROP_MAGCW;
				}

				// Didn't have a smith upgrade :(
				// if(check_oprop(obj, OPROP_HOLYW) && check_oprop(obj, OPROP_UNHYW) && !is_full_insight_weapon(obj)){
					// count_energy++;
					// latest_oprop = OPROP_HOLYW;
					// latest_energy = OPROP_HOLYW;
				// }
				// else if(check_oprop(obj, OPROP_LESSER_HOLYW) && check_oprop(obj, OPROP_LESSER_UNHYW) && is_full_insight_weapon(obj)){
					// count_energy++;
					// latest_oprop = OPROP_LESSER_HOLYW;
					// latest_energy = OPROP_HOLYW;
				// }
				
				if(count_energy == 0)
					break;
				else if(count_energy > 1){
				}
				remove_oprop(obj, latest_oprop);
				// if(latest_oprop == OPROP_HOLYW)
					// remove_oprop(obj, OPROP_UNHYW);
				// else if(latest_oprop == OPROP_LESSER_HOLYW)
					// remove_oprop(obj, OPROP_LESSER_UNHYW);
				remove_oprop(obj, OPROP_INSTW);
				remove_oprop(obj, OPROP_SMITHU);
				struct obj *crystal = mksobj(CRYSTAL, MKOBJ_NOINIT);
				add_oprop(crystal, latest_energy);

				set_material_gm(crystal, GEMSTONE);
				if(latest_energy == OPROP_FIREW){
					set_submat(crystal, RUBY);
				}
				else if(latest_energy == OPROP_COLDW){
					set_submat(crystal, DIAMOND);
				}
				else if(latest_energy == OPROP_ELECW){
					set_submat(crystal, TOPAZ);
				}
				else if(latest_energy == OPROP_ACIDW){
					set_submat(crystal, EMERALD);
				}
				else if(latest_energy == OPROP_MAGCW){
					set_submat(crystal, SAPPHIRE);
				}
				// else if(latest_energy == OPROP_HOLYW){
					// set_submat(crystal, );
				// }
				hold_another_object(crystal, u.uswallow ?
						   "Oops!  %s out of your reach!" :
						(Weightless ||
						 Is_waterlevel(&u.uz) ||
						 levl[u.ux][u.uy].typ < IRONBARS ||
						 levl[u.ux][u.uy].typ >= ICE) ?
							   "Oops!  %s away from you!" :
							   "Oops!  %s to the floor!",
							   The(aobjnam(crystal, "slip")),
							   (const char *)0);
			}
			break;
		}
	}
}

void
dipforge(struct obj *obj)
{
	if (Levitation) {
		floating_above("forge");
		return;
	}

	burn_away_slime();
	melt_frozen_air();
	
	boolean forgeable = (u.ublood_smithing && (!is_flammable(obj) || obj->oerodeproof)) || is_metallic(obj);

	/* Dipping something you're still wearing into a forge filled with
	 * lava, probably not the smartest thing to do. This is gonna hurt.
	 * Non-metallic objects are handled by lava_damage().
	 */
	if(obj->otyp == CHIKAGE && obj_is_material(obj, HEMARGYOS)){
		pline("The blood burns off the sword!");
		set_material_gm(obj, obj->ovar1_alt_mat);
		set_object_color(obj);
		obj->oeroded = access_oeroded(obj->ovar2_alt_erosion);
		obj->oeroded2 = access_oeroded2(obj->ovar2_alt_erosion);
		obj->oeroded3 = access_oeroded3(obj->ovar2_alt_erosion);
		(void) stop_timer(REVERT_OBJECT, obj->timed);
		fix_object(obj);
		update_inventory();
	}
	if(!P_RESTRICTED(P_SMITHING) && !forgeable){
		if(yn("Are you sure you want to forge a non-metal object?") != 'y')
			return;
	}
	if ((forgeable || is_metallic(obj)) && (obj->owornmask & (W_ARMOR | W_ACCESSORY))) {
		if (!Fire_resistance) {
			You("dip your worn %s into the forge.  You burn yourself!",
				xname(obj));
			if (!rn2(3))
				You("may want to remove your %s first...",
					xname(obj));
			losehp(d(2, 8),
				   "dipping a worn object into a forge", KILLED_BY);
		}
		else {
			You("dip your worn %s into the forge.  This is fine.",
				xname(obj));
			if (!rn2(3))
				You("may want to remove your %s first...",
					xname(obj));
		}
		return;
	}

	/* If punished and wielding a hammer, there's a good chance
	 * you can use a forge to free yourself */
	if (Punished && obj->otyp == HEAVY_IRON_BALL) {
		if ((uwep && !is_hammer(uwep)) || !uwep) { /* sometimes drop a hint */
			if (!rn2(4))
				pline("You'll need a hammer to be able to break the chain.");
			goto result;
		} else if (uwep && is_hammer(uwep)) {
			You("place the ball and chain inside the forge.");
			pline("Raising your %s, you strike the chain...",
				  xname(uwep));
			if (!rn2((P_SKILL(P_HAMMER) < P_SKILLED) ? 8 : 2)
				&& Luck >= 0) { /* training up hammer skill pays off */
				pline("The chain breaks free!");
				unpunish();
			} else {
				pline("Clang!");
			}
		}
		return;
	}

result:
	if(!P_RESTRICTED(P_SMITHING) && !Confusion && !Stunned && forgeable){
		if(!Blind){
			Your("%s glows in the heat.", xname(obj));
			if(!((obj->known || obj->oartifact || !check_oprop(obj, OPROP_NONE)) && obj->rknown && obj->sknown)){
				obj->known = obj->rknown = obj->sknown = TRUE;
				pline("You assess the quality of %s.", the(xname(obj)));
				use_skill(P_SMITHING, 1);
				exercise(A_INT, TRUE);
			}
			if(P_SKILL(P_SMITHING) >= P_EXPERT && !obj->bknown){
				if(obj->blessed){
					pline("The glow looks noticeably blue.");
				}
				else if (obj->cursed){
					pline("The glow looks slightly too red.");
				}
				else {
					pline("The glow is a cheery amber.");
				}
				use_skill(P_SMITHING, 1);
				exercise(A_WIS, TRUE);
				obj->bknown = TRUE;
			}
			if(obj != uwep && uwep && is_hammer(uwep) && !bimanual(uwep, youracedata)){
				smithing_object(obj);
			}
		}
	}
	else {
		switch (rnd(30)) {
		case 6:
		case 7:
		case 8:
		case 9: /* Strange feeling */
			pline("A weird sensation runs up your %s.", body_part(ARM));
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
			if (!is_metallic(obj))
				goto lava;
			Your("%s glows briefly from the heat.", xname(obj));

			if (is_metallic(obj)
				&& obj != uwep && uwep && is_hammer(uwep)
				&& !bimanual(uwep, youracedata) && Luck >= rnd(20)
			) {
				if (greatest_erosion(obj) > 0) {
					if (!Blind)
						You("successfully reforge your %s, repairing some of the damage.",
							xname(obj));
					if (obj->oeroded > 0)
						obj->oeroded--;
					if (obj->oeroded2 > 0)
						obj->oeroded2--;
				} else {
					if (!Blind) {
						Your("%s glows briefly from the heat, but looks reforged and as new as ever.",
							 xname(obj));
					}
				}
			}
			break;
		case 19:
		case 20:
			if (!is_metallic(obj))
				goto lava;
			You_feel("a sudden wave of heat.");
			// Achievements in ignorance: Smiths can't do this with forges, though they can use them to *check* blessedness.
			if (!obj->blessed && is_metallic(obj) && Luck > 5) {
				bless(obj);
				if (!Blind) {
					Your("%s glows blue for a moment.",
						 xname(obj));
				}
			} else {
				You_feel("a sudden wave of heat.");
			}
			break;
		case 21: /* Lava Demon */
			if (!rn2(8))
				dolavademon();
			else
				pline_The("forge violently spews lava for a moment, then settles.");
			break;
		case 22:
			if (Luck < 0) {
				blowupforge(u.ux, u.uy);
				/* Avoid destroying the same item twice (lava_damage) */
				return;
			} else {
				pline("Molten lava surges up and splashes all over you!");
				if(!Fire_resistance)
					losehp(d(6, 6), "dipping into a forge", KILLED_BY);
			}
			break;
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30: /* Strange feeling */
			You_feel("a sudden flare of heat.");
			break;
		}
lava:
		lava_damage(obj, u.ux, u.uy);
		update_inventory();
	}
}

void
drinkfountain()
{
	/* What happens when you drink from a fountain? */
	register boolean mgkftn = (levl[u.ux][u.uy].blessedftn == 1);
	register int fate = rnd(30);

	if (Levitation) {
		floating_above("fountain");
		return;
	}

	if (mgkftn && u.uluck >= 0 && fate >= 10) {
		int i, ii, littleluck = (u.uluck < 4);

		pline("Wow!  This makes you feel great!");
		/* blessed restore ability */
		for (ii = 0; ii < A_MAX; ii++)
		    if (ABASE(ii) < AMAX(ii)) {
			ABASE(ii) = AMAX(ii);
			flags.botl = 1;
		    }
		/* gain ability, blessed if "natural" luck is high */
		i = rn2(A_MAX);		/* start at a random attribute */
		for (ii = 0; ii < A_MAX; ii++) {
		    if (adjattrib(i, 1, littleluck ? -1 : 0) && littleluck)
			break;
		    if (++i >= A_MAX) i = 0;
		}
		display_nhwindow(WIN_MESSAGE, FALSE);
		pline("A wisp of vapor escapes the fountain...");
		exercise(A_WIS, TRUE);
		levl[u.ux][u.uy].blessedftn = 0;
		return;
	}

	if (fate < 10) {
		pline_The("cool draught refreshes you.");
		if(Race_if(PM_INCANTIFIER)) u.uen += rnd(10); /* don't choke on water */
		else u.uhunger += rnd(10); /* don't choke on water */
		newuhs(FALSE);
		if(mgkftn) return;
	} else {
	    switch (fate) {

		case 19: /* Self-knowledge */

			You_feel("self-knowledgeable...");
			display_nhwindow(WIN_MESSAGE, FALSE);
			doenlightenment();
			exercise(A_WIS, TRUE);
			pline_The("feeling subsides.");
			break;

		case 20: /* Foul water */

			if (!umechanoid){
				pline_The("water is foul!  You gag and vomit.");
				morehungry(rn1(20, 11));
				vomit();
	    		} 
			else {
				pline_The("water is foul! It offends your olfactory receptors.");
			}
			
			break;

		case 21: /* Poisonous */

			pline_The("water is contaminated!");
			if (Poison_resistance) {
			   pline(
			      "Perhaps it is runoff from the nearby %s farm.",
				 fruitname(FALSE));
			   losehp(rnd(4),"unrefrigerated sip of juice",
				KILLED_BY_AN);
			   break;
			}
			losestr(rn1(4,3));
			losehp(rnd(10),"contaminated water", KILLED_BY);
			exercise(A_CON, FALSE);
			break;

		case 22: /* Fountain of snakes! */

			dowatersnakes();
			break;

		case 23: /* Water demon */
			dowaterdemon();
			break;

		case 24: /* Curse an item */ {
			register struct obj *obj;

			pline("This water's no good!");
			morehungry(rn1(20, 11));
			exercise(A_CON, FALSE);
			rndcurse();
			break;
			}

		case 25: /* See invisible */

			if (Blind) {
			    if (Invisible) {
				You("feel transparent.");
			    } else {
			    	You("feel very self-conscious.");
			    	pline("Then it passes.");
			    }
			} else {
			   You("see an image of someone stalking you.");
			   pline("But it disappears.");
			}
			HSee_invisible |= TIMEOUT_INF;
			newsym(u.ux,u.uy);
			exercise(A_WIS, TRUE);
			break;

		case 26: /* See Monsters */

			(void) monster_detect((struct obj *)0, 0);
			exercise(A_WIS, TRUE);
			break;

		case 27: /* Find a gem in the sparkling waters. */

			if (!FOUNTAIN_IS_LOOTED(u.ux,u.uy)) {
				dofindgem();
				break;
			}

		case 28: /* Water Nymph */

			dowaternymph();
			break;

		case 29: /* Scare */ {
			register struct monst *mtmp;

			pline("This water gives you bad breath!");
			for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
			    if(!DEADMONSTER(mtmp))
				monflee(mtmp, 0, FALSE, FALSE);
			}
			break;

		case 30: /* Gushing forth in this room */

			dogushforth(TRUE);
			break;

		default:

			pline("This tepid water is tasteless.");
			break;
	    }
	}
	dryup(u.ux, u.uy, TRUE);
}

void
dipfountain(obj)
register struct obj *obj;
{
	if (Levitation) {
		floating_above("fountain");
		return;
	}

	/* Don't grant Excalibur when there's more than one object.  */
	/* (quantity could be > 1 if merged daggers got polymorphed) */
	if (obj->otyp == LONG_SWORD && obj->quan == 1L
	    && u.ulevel >= 5 && !rn2(6)
	    && !obj->oartifact
	    && !art_already_exists(ART_EXCALIBUR)) {

		if (u.ualign.type != A_LAWFUL) {
			/* Ha!  Trying to cheat her. */
			pline("A freezing mist rises from the water and envelops the sword.");
			pline_The("fountain disappears!");
			curse(obj);
			if (obj->spe > -6 && !rn2(3)) obj->spe--;
			obj->oerodeproof = FALSE;
			exercise(A_WIS, FALSE);
		} else {
			/* The lady of the lake acts! - Eric Backus */
			/* Be *REAL* nice */
			pline("From the murky depths, a hand reaches up to bless the sword.");
			pline("As the hand retreats, the fountain disappears!");
			obj = oname(obj, artiname(ART_EXCALIBUR));
			discover_artifact(ART_EXCALIBUR);
			bless(obj);
			obj->oeroded = obj->oeroded2 = 0;
			obj->oerodeproof = TRUE;
			exercise(A_WIS, TRUE);
		}
		update_inventory();
		levl[u.ux][u.uy].typ = ROOM;
		levl[u.ux][u.uy].looted = 0;
		newsym(u.ux, u.uy);
		level.flags.nfountains--;
		if(in_town(u.ux, u.uy))
		    (void) angry_guards(FALSE);
		return;
	} else if (get_wet(obj,FALSE) && !rn2(2))
		return;

	/* Acid and water don't mix */
	if (obj->otyp == POT_ACID) {
	    useup(obj);
	    return;
	}

	switch (rnd(30)) {
		case 16: /* Curse the item */
			curse(obj);
			break;
		case 17:
		case 18:
		case 19:
		case 20: /* Uncurse the item */
			if(obj->cursed) {
			    if (!Blind)
				pline_The("water glows for a moment.");
			    uncurse(obj);
			} else {
			    pline("A feeling of loss comes over you.");
			}
			break;
		case 21: /* Water Demon */
			dowaterdemon();
			break;
		case 22: /* Water Nymph */
			dowaternymph();
			break;
		case 23: /* an Endless Stream of Snakes */
			dowatersnakes();
			break;
		case 24: /* Find a gem */
			if (!FOUNTAIN_IS_LOOTED(u.ux,u.uy)) {
				dofindgem();
				break;
			}
		case 25: /* Water gushes forth */
			dogushforth(FALSE);
			break;
		case 26: /* Strange feeling */
			pline("A strange tingling runs up your %s.",
							body_part(ARM));
			break;
		case 27: /* Strange feeling */
			You_feel("a sudden chill.");
			break;
		case 28: /* Strange feeling */
			pline("An urge to take a bath overwhelms you.");
#ifndef GOLDOBJ
			if (u.ugold > 10) {
			    u.ugold -= somegold() / 10;
			    You("lost some of your gold in the fountain!");
			    CLEAR_FOUNTAIN_LOOTED(u.ux,u.uy);
			    exercise(A_WIS, FALSE);
			}
#else
			{
			    long money = money_cnt(invent);
			    struct obj *otmp;
                            if (money > 10) {
				/* Amount to loose.  Might get rounded up as fountains don't pay change... */
			        money = somegold(money) / 10; 
			        for (otmp = invent; otmp && money > 0; otmp = otmp->nobj) if (otmp->oclass == COIN_CLASS) {
				    int denomination = objects[otmp->otyp].oc_cost;
				    long coin_loss = (money + denomination - 1) / denomination;
                                    coin_loss = min(coin_loss, otmp->quan);
				    otmp->quan -= coin_loss;
				    money -= coin_loss * denomination;				  
				    if (!otmp->quan) delobj(otmp);
				}
			        You("lost some of your money in the fountain!");
				CLEAR_FOUNTAIN_LOOTED(u.ux,u.uy);
			        exercise(A_WIS, FALSE);
                            }
			}
#endif
			break;
		case 29: /* You see coins */

		/* We make fountains have more coins the closer you are to the
		 * surface.  After all, there will have been more people going
		 * by.	Just like a shopping mall!  Chris Woodbury  */

		    if (FOUNTAIN_IS_LOOTED(u.ux,u.uy)) break;
		    SET_FOUNTAIN_LOOTED(u.ux,u.uy);
		    (void) mkgold((long)
			(rnd((dunlevs_in_dungeon(&u.uz)-dunlev(&u.uz)+1)*2)+5),
			u.ux, u.uy);
		    if (!Blind)
		pline("Far below you, you see coins glistening in the water.");
		    exercise(A_WIS, TRUE);
		    newsym(u.ux,u.uy);
		    break;
	}
	update_inventory();
	dryup(u.ux, u.uy, TRUE);
}

void
breakforge(x, y)
int x, y;
{
    if (cansee(x, y) || (x == u.ux && y == u.uy))
        pline_The("forge splits in two as molten lava rushes forth!");
    levl[x][y].doormask = 0;
    levl[x][y].typ = LAVAPOOL;
    newsym(x, y);
    level.flags.nforges--;
}

void
blowupforge(x, y)
int x, y;
{
    if (cansee(x, y) || (x == u.ux && y == u.uy))
        pline_The("forge rumbles, then explodes!  Molten lava splashes everywhere!");
    levl[x][y].typ = ROOM, levl[x][y].flags = 0;
    levl[x][y].doormask = 0;
    newsym(x, y);
    level.flags.nforges--;
    explode(u.ux, u.uy, AD_FIRE, FORGE_EXPLODE, d(6,6), EXPL_FIERY, 1);
}

//Note: used in EvilHack if a forge is used up by forging magic or artifacts

void
coolforge(x, y)
int x, y;
{
    if (cansee(x, y) || (x == u.ux && y == u.uy))
        pline_The("lava in the forge cools and solidifies.");
    levl[x][y].typ = ROOM, levl[x][y].flags = 0;
    levl[x][y].doormask = 0;
    newsym(x, y);
    level.flags.nforges--;
}

void
drinkforge()
{
    if (Levitation) {
        floating_above("forge");
        return;
    }

    if (!likes_fire(youmonst.data)) {
        pline("Molten lava incinerates its way down your gullet...");
        losehp(Upolyd ? u.mh : u.uhp, "trying to drink molten lava", KILLED_BY);
        return;
    }
    burn_away_slime();
	melt_frozen_air();
    switch(rn2(20)) {
    case 0:
        pline("You drink some molten lava.  Mmmmm mmm!");
		if(!Race_if(PM_INCANTIFIER))
			lesshungry(rnd(50));
        break;
    case 1:
        breakforge(u.ux, u.uy);
        break;
    case 2:
    case 3:
        pline_The("%s moves as though of its own will!", hliquid("lava"));
        if ((mvitals[PM_FIRE_ELEMENTAL].mvflags & G_GONE)
            || !makemon(&mons[PM_FIRE_ELEMENTAL], u.ux, u.uy, MM_ADJACENTOK)
		)
            pline("But it settles down.");
        break;
    default:
        pline("You take a sip of molten lava.");
		if(!Race_if(PM_INCANTIFIER))
			lesshungry(rnd(5));
    }
}

#ifdef SINKS
void
breaksink(x,y)
int x, y;
{
    if(cansee(x,y) || (x == u.ux && y == u.uy))
	pline_The("pipes break!  Water spurts out!");
    level.flags.nsinks--;
    levl[x][y].doormask = 0;
    levl[x][y].typ = FOUNTAIN;
    level.flags.nfountains++;
    newsym(x,y);
}

void
drinksink()
{
	struct obj *otmp;
	struct monst *mtmp;

	if (Levitation) {
		floating_above("sink");
		return;
	}
	switch(rn2(20)) {
		case 0: You("take a sip of very cold water.");
			break;
		case 1: You("take a sip of very warm water.");
			break;
		case 2: You("take a sip of scalding hot water.");
			if (Fire_resistance)
				pline("It seems quite tasty.");
			else losehp(rnd(6), "sipping boiling water", KILLED_BY);
			break;
		case 3: if (mvitals[PM_SEWER_RAT].mvflags & G_GONE && !In_quest(&u.uz))
				pline_The("sink seems quite dirty.");
			else {
				mtmp = makemon(&mons[PM_SEWER_RAT],
						u.ux, u.uy, NO_MM_FLAGS);
				if (mtmp) pline("Eek!  There's %s in the sink!",
					(Blind || !canspotmon(mtmp)) ?
					"something squirmy" :
					a_monnam(mtmp));
			}
			break;
		case 4: do {
				/* use Luck here instead of u.uluck */
				if (!rn2(13) && ((Luck >= 0 && is_vampire(youracedata)) ||
				    (Luck <= 0 && !is_vampire(youracedata)))) {
					otmp = mksobj(POT_BLOOD, MKOBJ_NOINIT);
					if(Luck){
						/*Good luck: vampire, which wants cursed human blood, bad luck: sucks to be a human PC!*/
						otmp->corpsenm = PM_HUMAN;
						curse(otmp);
					}
					else {
						otmp->corpsenm = (mvitals[PM_SEWER_RAT].mvflags & G_GENOD && !In_quest(&u.uz)) ? PM_WERERAT : PM_SEWER_RAT;
					}
				} else {
					otmp = mkobj(POTION_CLASS, NO_MKOBJ_FLAGS);
					if (otmp->otyp == POT_WATER) {
						obfree(otmp, (struct obj *)0);
						otmp = (struct obj *) 0;
					}
				}
			} while(!otmp);
			otmp->cursed = otmp->blessed = 0;
			pline("Some %s liquid flows from the faucet.",
			      Blind ? "odd" :
			      hcolor(OBJ_DESCR(objects[otmp->otyp])));
			otmp->dknown = !(Blind || Hallucination);
			otmp->quan++; /* Avoid panic upon useup() */
			otmp->fromsink = 1; /* kludge for docall() */
			(void) dopotion(otmp, TRUE);
			obfree(otmp, (struct obj *)0);
			break;
		case 5: if (!(levl[u.ux][u.uy].looted & S_LRING)) {
			    You("find a ring in the sink!");
			    (void) mkobj_at(RING_CLASS, u.ux, u.uy, MKOBJ_ARTIF);
			    levl[u.ux][u.uy].looted |= S_LRING;
			    exercise(A_WIS, TRUE);
			    newsym(u.ux,u.uy);
			} else pline("Some dirty water backs up in the drain.");
			break;
		case 6: breaksink(u.ux,u.uy);
			break;
		case 7: pline_The("water moves as though of its own will!");
			if ((mvitals[PM_WATER_ELEMENTAL].mvflags & G_GONE && !In_quest(&u.uz))
			    || !makemon(&mons[PM_WATER_ELEMENTAL],
					u.ux, u.uy, NO_MM_FLAGS))
				pline("But it quiets down.");
			break;
		case 8: pline("Yuk, this water tastes awful.");
			more_experienced(1,0);
			newexplevel();
			break;
		case 9: if (!uclockwork) {
				pline("Gaggg... this tastes like sewage!  You vomit.");
				morehungry(max_ints(1, rn1(30-ACURR(A_CON), 11) * get_uhungersizemod()));
				vomit();
			}
			else {
				pline("Ugh, this tastes like sewage. Your gustatory receptors are offended.");
			}
			
			break;
		case 10: pline("This water contains toxic wastes!");
			if (!Unchanging) {
				You("undergo a freakish metamorphosis!");
				polyself(FALSE);
			}
			break;
		/* more odd messages --JJB */
		case 11: You_hear("clanking from the pipes...");
			break;
		case 12: You_hear("snatches of song from among the sewers...");
			break;
		case 19: if (Hallucination) {
		   pline("From the murky drain, a hand reaches up... --oops--");
				break;
			}
		default: You("take a sip of %s water.",
			rn2(3) ? (rn2(2) ? "cold" : "warm") : "hot");
	}
}
#endif /* SINKS */

/*fountain.c*/
