/*	SCCS Id: @(#)explode.c	3.4	2002/11/10	*/
/*	Copyright (C) 1990 by Ken Arromdee */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "xhity.h"

#ifdef OVL0

/* ExplodeRegions share some commonalities with NhRegions, but not enough to
 * make it worth trying to create a common implementation.
 */
typedef struct {
    xchar x, y;
    xchar blast;	/* blast symbol */
    xchar shielded;	/* True if this location is shielded */
} ExplodeLocation;

typedef struct {
    ExplodeLocation *locations;
    short nlocations, alocations;
} ExplodeRegion;

STATIC_DCL ExplodeRegion *
create_explode_region()
{
    ExplodeRegion *reg;

    reg = (ExplodeRegion *)alloc(sizeof(ExplodeRegion));
    reg->locations = (ExplodeLocation *)0;
    reg->nlocations = 0;
    reg->alocations = 0;
    return reg;
}

STATIC_DCL void
add_location_to_explode_region(x, y, reg)
xchar x, y;
ExplodeRegion *reg;
{
    int i;
    ExplodeLocation *new;
    for(i = 0; i < reg->nlocations; i++)
	if (reg->locations[i].x == x && reg->locations[i].y == y)
	    return;
    if (reg->nlocations == reg->alocations) {
	reg->alocations = reg->alocations ? 2 * reg->alocations : 32;
	new = (ExplodeLocation *)
		alloc(reg->alocations * sizeof(ExplodeLocation));
	(void) memcpy((genericptr_t)new, (genericptr_t)reg->locations,
		reg->nlocations * sizeof(ExplodeLocation));
	free((genericptr_t)reg->locations);
	reg->locations = new;
    }
    reg->locations[reg->nlocations].x = x;
    reg->locations[reg->nlocations].y = y;
    /* reg->locations[reg->nlocations].blast = 0; */
    /* reg->locations[reg->nlocations].shielded = 0; */
    reg->nlocations++;
}

STATIC_DCL int
compare_explode_location(loc1, loc2)
ExplodeLocation *loc1, *loc2;
{
    return loc1->y == loc2->y ? loc1->x - loc2->x : loc1->y - loc2->y;
}

STATIC_DCL void
set_blast_symbols(reg)
ExplodeRegion *reg;
{
    int i, j, bitmask;
    /* The index into the blast symbol array is a bitmask containing 4 bits:
     * bit 3: True if the location immediately to the north is present
     * bit 2: True if the location immediately to the south is present
     * bit 1: True if the location immediately to the east is present
     * bit 0: True if the location immediately to the west is present
     */
    static int blast_symbols[16] = {
	S_explode5, S_explode6, S_explode4, S_explode5,
	S_explode2, S_explode3, S_explode1, S_explode2,
	S_explode8, S_explode9, S_explode7, S_explode8,
	S_explode5, S_explode6, S_explode4, S_explode5,
    };
    /* Sort in order of North -> South, West -> East */
    qsort(reg->locations, reg->nlocations, sizeof(ExplodeLocation),
	    compare_explode_location);
    /* Pass 1: Build the bitmasks in the blast field */
    for(i = 0; i < reg->nlocations; i++)
	reg->locations[i].blast = 0;
    for(i = 0; i < reg->nlocations; i++) {
	bitmask = 0;
	if (i && reg->locations[i-1].y == reg->locations[i].y &&
		reg->locations[i-1].x == reg->locations[i].x-1) {
	    reg->locations[i].blast |= 1;	/* Location to the west */
	    reg->locations[i-1].blast |= 2;	/* Location to the east */
	}
	for(j = i-1; j >= 0; j--) {
	    if (reg->locations[j].y < reg->locations[i].y-1)
		break;
	    else if (reg->locations[j].y == reg->locations[i].y-1 &&
		    reg->locations[j].x == reg->locations[i].x) {
		reg->locations[i].blast |= 8;	/* Location to the north */
		reg->locations[j].blast |= 4;	/* Location to the south */
		break;
	    }
	}
    }
    /* Pass 2: Set the blast symbols */
    for(i = 0; i < reg->nlocations; i++)
	reg->locations[i].blast = blast_symbols[reg->locations[i].blast];
}

STATIC_DCL void
free_explode_region(reg)
ExplodeRegion *reg;
{
    free((genericptr_t)reg->locations);
    free((genericptr_t)reg);
}

/* This is the "do-it-all" explosion command */
STATIC_DCL void FDECL(do_explode,
	(int,int,ExplodeRegion *,int,int,int,int,int,BOOLEAN_P));

/* Note: I had to choose one of three possible kinds of "type" when writing
 * this function: a wand type (like in zap.c), an adtyp, or an object type.
 * Wand types get complex because they must be converted to adtyps for
 * determining such things as fire resistance.  Adtyps get complex in that
 * they don't supply enough information--was it a player or a monster that
 * did it, and with a wand, spell, or breath weapon?  Object types share both
 * these disadvantages....
 */
void
explode(x, y, adtyp, olet, dam, color, radius)
int x, y;
int adtyp; /* the same as in zap.c */
int olet;
int dam;
int color;
int radius;
{
	ExplodeRegion *area;
	area = create_explode_region();
	if (radius == 0)
	{
		if (isok(x, y))
			add_location_to_explode_region(x, y, area);
	}
	else if (radius == 1)
	{	// can use simple method of creating explosions
		int i, j;
		for (i = -1; i <= 1; i++)
		for (j = -1; j <= 1; j++)
			if (isok(x + i, y + j))
				add_location_to_explode_region(x + i, y + j, area);
	}
	else
	{	// use circles
		do_clear_area(x, y, radius, add_location_to_explode_region, (genericptr_t)(area));
	}

	do_explode(x, y, area, adtyp, olet, dam, color, 0, !flags.mon_moving);
	free_explode_region(area);
}

void
explode_yours(x, y, adtyp, olet, dam, color, radius, yours)
int x, y;
int adtyp; /* the same as in zap.c */
int olet;
int dam;
int color;
int radius;
boolean yours; /* is it your fault (for killing monsters) */
{
	ExplodeRegion *area;
	area = create_explode_region();
	if (radius == 0)
	{
		if (isok(x, y))
			add_location_to_explode_region(x, y, area);
	}
	else if (radius == 1)
	{	// can use simple method of creating explosions
		int i, j;
		for (i = -1; i <= 1; i++)
		for (j = -1; j <= 1; j++)
			if (isok(x + i, y + j))
				add_location_to_explode_region(x + i, y + j, area);
	}
	else
	{	// use circles
		do_clear_area(x, y, radius, add_location_to_explode_region, (genericptr_t)(area));
	}

	do_explode(x, y, area, adtyp, olet, dam, color, 0, yours);
	free_explode_region(area);
}

void
splash(x, y, dx, dy, adtyp, olet, dam, color)
int x, y, dx, dy;
int adtyp;
int olet;
int dam;
int color;
{
	/*
	Splash pattern:
	.....  .....
	...XX  .....
	..@OX  ..@X.
	...XX  ..XOX
	.....  ...XX
	O is located at (x,y)
	@->O gives dx and dy
	*/
	ExplodeRegion *area;
	int i, j;
	boolean diag = ((!!dx + !!dy) / 2);
	area = create_explode_region();

	if (isok(x, y))
		add_location_to_explode_region(x, y, area);

	for (i = -1; i <= 1; i++)
	for (j = -1; j <= 1; j++)
	if (isok(x + i, y + j) && ((!i && dx) || (!j && dy) || ((!dx || i == dx) & (!dy || j == dy))) && ((ZAP_POS(levl[x][y].typ) || distmin(x - dx, y - dy, x + i, y + j) == 1) || ZAP_POS(levl[x - dx + i][y - dy + j].typ))) // it looks strange, but it works
		add_location_to_explode_region(x + i, y + j, area);

	do_explode(x, y, area, adtyp, olet, dam, color, 0, !flags.mon_moving);
	free_explode_region(area);
}

STATIC_DCL void
do_explode(x, y, area, adtyp, olet, dam, color, dest, yours)
int x, y;
ExplodeRegion *area;
int adtyp; /* AD_TYPE and O_CLASS describing the cause of the explosion */
int olet;
int dam;
int color;
int dest; /* 0 = normal, 1 = silent, 2 = silent/remote */	
boolean yours; /* is it your fault (for killing monsters) */
{
	int i, k, damu = dam;
	boolean starting = 1, silver = FALSE;
	boolean visible, any_shield;
	int uhurt = 0; /* 0=unhurt, 1=items damaged, 2=you and items damaged, 3=half elemental damage */
	const char *str;
	int idamres, idamnonres;
	struct monst *mtmp;
	boolean explmask;
	boolean shopdamage = FALSE;
	boolean generic = FALSE;
	boolean silent = FALSE, remote = FALSE;
	xchar xi, yi;

	if (dest > 0) silent = TRUE;	
	if (dest == 2) remote = TRUE;

	if (olet == WAND_CLASS)		/* retributive strike */
		switch (Role_switch) {
			case PM_PRIEST:
			case PM_MONK:
			case PM_WIZARD: damu /= 5;
				  break;
			case PM_HEALER:
			case PM_KNIGHT: damu /= 2;
				  break;
			default:  break;
		}

	if (olet == MON_EXPLODE && killer) {
	    str = killer;
	    killer = 0;		/* set again later as needed */
	    // adtyp = AD_PHYS;
	}
	else switch (adtyp) {
		case AD_MAGM: str = "magical blast";
			break;
		case AD_EFIR:
		case AD_FIRE: str = (olet != BURNING_OIL ? olet != SCROLL_CLASS ? "fireball" : "tower of flame" : "burning oil");
			break;
		case AD_COLD: str = "ball of cold";
			break;
		case AD_DEAD: str = "death field";
			break;
		case AD_DISN: str = "disintegration field";
			break;
		case AD_ELEC: str = "ball of lightning";
			break;
		case AD_DRST: str = "poison gas cloud";
			break;
		case AD_EACD:
		case AD_ACID: str = "splash of acid";
			break;
		case AD_SLIM: str = "spout of acidic slime";
			break;
		case AD_PHYS: str = (olet != TOOL_CLASS ? olet != WEAPON_CLASS ? "blast" : "flying shards of obsidian" : "flying shards of mirror");
			break;
		case AD_DISE: str = "cloud of spores";
			break;
		case AD_DARK: str = "blast of darkness";
			break;
		case AD_BLUD: str = "spray of tainted blood";
			break;
		case AD_WET: str = "wall of water";
			break;
		default:
			impossible("unaccounted-for explosion damage type in do_explode: %d", adtyp);
			str = "404 BLAST NOT FOUND";
			break;
	}

	any_shield = visible = FALSE;
	for(i = 0; i < area->nlocations; i++) {
		explmask = FALSE;
		xi = area->locations[i].x;
		yi = area->locations[i].y;
		if (xi == u.ux && yi == u.uy) {
		    switch(adtyp) {
			case AD_PHYS:                        
				break;
			case AD_MAGM:
				explmask = !!Antimagic;
				break;
			case AD_EFIR:
			case AD_FIRE:
				explmask = !!Fire_resistance;
				break;
			case AD_ECLD:
			case AD_COLD:
				explmask = !!Cold_resistance;
				roll_frigophobia();
				break;
			case AD_DISN:
				explmask = !!Disint_resistance;
				break;
			case AD_DEAD:
				explmask = !!(resists_death(&youmonst) || u.sealsActive&SEAL_OSE);
				break;
			case AD_EELC:
			case AD_ELEC:
				explmask = !!Shock_resistance;
				break;
			case AD_DRST:
				explmask = !!Poison_resistance;
				break;
			case AD_EACD:
			case AD_ACID:
				explmask = !!Acid_resistance;
				break;
			case AD_SLIM:
				explmask = (Acid_resistance || Slime_res(&youmonst));
				break;
			case AD_DISE: /*assumes only swamp ferns have disease explosions*/
				explmask = !!Sick_resistance;
				diseasemu(&mons[PM_SWAMP_FERN_SPORE]);
				break;
			case AD_DARK:
				explmask = !!Dark_immune;
				break;
			case AD_BLUD:
				// explmask = !has_blood(youracedata);
				break;
			case AD_WET:
				break;
			default:
				impossible("explosion type %d?", adtyp);
				break;
		    }
		}
		/* can be both you and mtmp if you're swallowed */
		mtmp = m_at(xi, yi);
#ifdef STEED
		if (!mtmp && xi == u.ux && yi == u.uy)
			mtmp = u.usteed;
#endif
		if (mtmp) {
		    if (mtmp->mhp < 1) explmask = 2;
		    else switch(adtyp) {
			case AD_PHYS:
				if(mtmp->mtyp == PM_LICH__THE_FIEND_OF_EARTH || 
				   mtmp->mtyp == PM_BURNING_FERN_SPROUT ||
				   mtmp->mtyp == PM_BURNING_FERN ||
				   mtmp->mtyp == PM_CHAOS
				) explmask |= TRUE;
				break;
			case AD_MAGM:
				explmask |= resists_magm(mtmp);
				break;
			case AD_EFIR:
			case AD_FIRE:
				explmask |= resists_fire(mtmp);
				break;
			case AD_ECLD:
			case AD_COLD:
				explmask |= resists_cold(mtmp);
				break;
			case AD_DISN:
				explmask |= resists_disint(mtmp);
				break;
			case AD_DEAD:
				explmask |= resists_death(mtmp);
				break;
			case AD_EELC:
			case AD_ELEC:
				explmask |= resists_elec(mtmp);
				break;
			case AD_DRST:
				explmask |= resists_poison(mtmp);
				break;
			case AD_EACD:
			case AD_ACID:
				explmask |= resists_acid(mtmp);
				break;
			case AD_SLIM:
				explmask |= resists_acid(mtmp) || Slime_res(mtmp);
				break;
			case AD_DISE:
				explmask |= resists_sickness(mtmp);
				break;
			case AD_DARK:
				explmask |= dark_immune(mtmp);
				break;
			case AD_BLUD:
				// explmask |= has_blood_mon(mtmp);
				break;
			case AD_WET:
				break;
			default:
				impossible("explosion type %d?", adtyp);
				break;
		    }
		}
		if (mtmp && cansee(xi,yi) && !canspotmon(mtmp))
		    map_invisible(xi, yi);
		else if (!mtmp && glyph_is_invisible(levl[xi][yi].glyph)) {
		    unmap_object(xi, yi);
		    newsym(xi, yi);
		}
		if (cansee(xi, yi)) visible = TRUE;
		if (explmask) any_shield = TRUE;
		area->locations[i].shielded = explmask;
	}

	/* Not visible if remote */
	if (remote) visible = FALSE;
	
	if (visible) {
		set_blast_symbols(area);
		/* Start the explosion */
		for(i = 0; i < area->nlocations; i++) {
			tmp_at(starting ? DISP_BEAM : DISP_CHANGE,
			    explosion_to_glyph(color,
			    area->locations[i].blast));
		    tmp_at(area->locations[i].x, area->locations[i].y);
			starting = 0;
		}
		curs_on_u();	/* will flush screen and output */

		if (any_shield && flags.sparkle) { /* simulate shield effect */
		    for (k = 0; k < SHIELD_COUNT; k++) {
			for(i = 0; i < area->nlocations; i++) {
			    if (area->locations[i].shielded && cansee(area->locations[i].x, area->locations[i].y))
				/*
				 * Bypass tmp_at() and send the shield glyphs
				 * directly to the buffered screen.  tmp_at()
				 * will clean up the location for us later.
				 */
				show_glyph(area->locations[i].x,
					area->locations[i].y,
					cmap_to_glyph(shield_static[k]));
			}
			curs_on_u();	/* will flush screen and output */
			delay_output();
		    }

		    /* Cover last shield glyph with blast symbol. */
		    for(i = 0; i < area->nlocations; i++) {
			if (area->locations[i].shielded)
			    show_glyph(area->locations[i].x,
				    area->locations[i].y,
				    explosion_to_glyph(color,
				    area->locations[i].blast));
		    }

		} else {		/* delay a little bit. */
		    delay_output();
		    delay_output();
		}
		tmp_at(DISP_END, 0); /* clear the explosion */
	} else if (!remote) {
	    if (olet == MON_EXPLODE) {
		str = "explosion";
		generic = TRUE;
	    }
	    if (flags.soundok)
		You_hear(is_pool(x, y, FALSE) ? "a muffled explosion." : "a blast.");
	}

	    if (dam) for(i = 0; i < area->nlocations; i++) {
		xi = area->locations[i].x;
		yi = area->locations[i].y;
		if (xi == u.ux && yi == u.uy)
		    uhurt = area->locations[i].shielded ? 1 : 2;
		idamres = idamnonres = 0;

		/* DS: Allow monster induced explosions also */
		//if (type >= 0 || type <= -10) //what was this supposed to correspond to? It isn't listed by buzz() in zap.c
		(void)zap_over_floor((xchar)xi, (xchar)yi, adtyp, WAND_CLASS, FALSE, &shopdamage);

		mtmp = m_at(xi, yi);
#ifdef STEED
		if (!mtmp && xi == u.ux && yi == u.uy)
			mtmp = u.usteed;
#endif
		if (!mtmp) continue;
		if (DEADMONSTER(mtmp)) continue;
		if (u.uswallow && mtmp == u.ustuck) {
			if (is_animal(u.ustuck->data)) {
				if (!silent) pline("%s gets %s!",
				      Monnam(u.ustuck),
				      (adtyp == AD_EFIR) ? "heartburn" :
				      (adtyp == AD_FIRE) ? "heartburn" :
				      (adtyp == AD_ECLD) ? "chilly" :
				      (adtyp == AD_COLD) ? "chilly" :
				      (adtyp == AD_DISN) ? "perforated" :
					  (adtyp == AD_DEAD) ? "irradiated by pure energy" :
				      (adtyp == AD_EELC) ? "shocked" :
				      (adtyp == AD_ELEC) ? "shocked" :
				      (adtyp == AD_DRST) ? "poisoned" :
				      (adtyp == AD_DISE) ? "high-yield food poisoning" :
				      (adtyp == AD_EACD) ? "an upset stomach" :
				      (adtyp == AD_ACID) ? "an upset stomach" :
				      (adtyp == AD_SLIM) ? "a little green" :
				      (adtyp == AD_WET) ? "bloated" :
				      (adtyp == AD_PHYS) ? ((olet == TOOL_CLASS) ?
				       "perforated" : "a bloated stomach") :
				       "fried");
			} else {
				if (!silent) pline("%s gets slightly %s!",
				      Monnam(u.ustuck),
				      (adtyp == AD_EFIR) ? "toasted" :
				      (adtyp == AD_FIRE) ? "toasted" :
				      (adtyp == AD_ECLD) ? "chilly" :
				      (adtyp == AD_COLD) ? "chilly" :
				      (adtyp == AD_DISN) ? "perforated" :
					  (adtyp == AD_DEAD) ? "overwhelmed by pure energy" :
				      (adtyp == AD_EELC) ? "shocked" :
				      (adtyp == AD_ELEC) ? "shocked" :
				      (adtyp == AD_DRST) ? "intoxicated" :
				      (adtyp == AD_DISE) ? "quesy" :
				      (adtyp == AD_EACD) ? "burned" :
				      (adtyp == AD_ACID) ? "burned" :
				      (adtyp == AD_SLIM) ? "green" :
				      (adtyp == AD_WET) ? "bloated" :
				      (adtyp == AD_PHYS) ? ((olet == TOOL_CLASS) ?
				       "perforated" : "blasted open") :
				       "fried");
			}
		} else if (!silent && cansee(xi, yi)) {
		    if(mtmp->m_ap_type) see_passive_mimic(mtmp);
		    pline("%s is caught in the %s!", Monnam(mtmp), str);
		}

		if (!(area->locations[i].shielded)) { /* Was affected */
			idamres += destroy_item(mtmp, SCROLL_CLASS, (int) adtyp);
			idamres += destroy_item(mtmp, SPBOOK_CLASS, (int) adtyp);
			idamnonres += destroy_item(mtmp, POTION_CLASS, (int) adtyp);
			idamnonres += destroy_item(mtmp, WAND_CLASS, (int) adtyp);
			idamnonres += destroy_item(mtmp, RING_CLASS, (int) adtyp);
		}

		if (area->locations[i].shielded) {
			golemeffects(mtmp, (int) adtyp, dam + idamres);
		} 
		//Golem effects handled for elemental damage effects, now either proceed to damage or do damage from items.
		if(area->locations[i].shielded && adtyp != AD_EFIR
		 && adtyp != AD_ECLD && adtyp != AD_EELC && adtyp != AD_EACD
		){
			mtmp->mhp -= idamnonres;
		}
		else {
		/* call resist with 0 and do damage manually so 1) we can
		 * get out the message before doing the damage, and 2) we can
		 * call mondied, not killed, if it's not your blast
		 */
			int mdam = dam;

			if (resist(mtmp, olet, 0, FALSE)) {
			    if (!silent && cansee(xi,yi))
				pline("%s resists the %s!", Monnam(mtmp), str);
			    mdam = dam/2;
			}
			//Elemental damage types continue through.
			if(area->locations[i].shielded)
				mdam /= 2;
			if(yours && mtmp->female && humanoid_torso(mtmp->data) && roll_madness(MAD_SANCTITY)){
			    mdam /= 4;
			}
			if(yours && roll_madness(MAD_ARGENT_SHEEN)){
			    mdam /= 6;
			}
			if(yours && (is_spider(mtmp->data) 
				|| mtmp->mtyp == PM_SPROW
				|| mtmp->mtyp == PM_DRIDER
				|| mtmp->mtyp == PM_PRIESTESS_OF_GHAUNADAUR
				|| mtmp->mtyp == PM_AVATAR_OF_LOLTH
			) && roll_madness(MAD_ARACHNOPHOBIA)){
			    mdam /= 4;
			}
			if(yours && mtmp->female && humanoid_upperbody(mtmp->data) && roll_madness(MAD_ARACHNOPHOBIA)){
			    mdam /= 2;
			}
			if(yours && is_aquatic(mtmp->data) && roll_madness(MAD_THALASSOPHOBIA)){
			    mdam /= 10;
			}
			if (mtmp == u.ustuck && x==u.ux && y==u.uy)
				mdam *= 2;
			if (mtmp == u.ustuck)
				mdam *= 2;
			if(hates_silver(mtmp->data) && silver){
				pline("The %s sear %s!", str, mon_nam(mtmp));
				mdam += rnd(20);
			}

			if (resists_cold(mtmp) && adtyp == AD_FIRE)
				mdam *= 2;
			else if (resists_fire(mtmp) && adtyp == AD_COLD)
				mdam *= 2;
			else if (Dark_vuln(mtmp) && adtyp == AD_DARK)
				mdam *= 2;
			else if (has_blood_mon(mtmp) && adtyp == AD_BLUD)
				mdam += mlev(mtmp);
			
			if(adtyp == AD_WET){
				water_damage(mtmp->minvent, FALSE, FALSE, FALSE, mtmp);
			}
			else if (adtyp == AD_BLUD){
				water_damage(mtmp->minvent, FALSE, FALSE, WD_BLOOD, mtmp);
				if(!yours){
					mtmp->mcrazed = TRUE;
					mtmp->mberserk = TRUE;
					mtmp->mconf = TRUE;
				}
			}
			
			if(adtyp == AD_SLIM && !Slime_res(mtmp) &&
				(!resists_acid(mtmp) ? (mtmp->mhp <= mdam*2) : (mtmp->mhp <= mdam))
			){
				(void)newcham(mtmp, PM_GREEN_SLIME, FALSE, canseemon(mtmp));
				mtmp->mstrategy &= ~STRAT_WAITFORU;
			} else {
				mtmp->mhp -= mdam;
				//Elemental damage types make it down here.
				if(!area->locations[i].shielded)
					mtmp->mhp -= idamres;
				mtmp->mhp -= idamnonres;
			}
		}
		if (mtmp->mhp <= 0) {
			/* KMH -- Don't blame the player for pets killing gas spores */
			if (yours) xkilled(mtmp, (silent ? 0 : 1));
			else monkilled(mtmp, "", (int)adtyp);
		} else if (!flags.mon_moving && yours) setmangry(mtmp);
	}

	/* Do your injury last */
	
	/* You are not hurt if this is remote */
	if (remote) uhurt = FALSE;
	
	if (uhurt) {
		/* [ALI] Give message if it's a weapon (grenade) exploding */
		if ((yours || adtyp == AD_PHYS || olet == WEAPON_CLASS) &&
			/* gas spores */
				flags.verbose && olet != SCROLL_CLASS)
			You("are caught in the %s!", str);

		if(uhurt == 1 && 
			(adtyp == AD_EFIR || adtyp == AD_ECLD || adtyp == AD_EELC || adtyp == AD_EACD)
		){
			damu /= 2;
			uhurt = 3;
		}

		if(hates_silver(youracedata) && silver){
			You("are seared by the %s!", str);
			damu += rnd(20);
		}
		if (adtyp == AD_BLUD && has_blood(youracedata)){
			pline("Rotten blood tears through your veins!");
			damu += u.ulevel;
		}
		/* do property damage first, in case we end up leaving bones */
		if (adtyp == AD_FIRE || adtyp == AD_EFIR){
			burn_away_slime();
			melt_frozen_air();
		}
		if (Invulnerable) {
		    damu = 0;
		    You("are unharmed!");
		} else {
			if (Half_physical_damage && adtyp == AD_PHYS)
		    damu = (damu+1) / 2;
			if (u.uvaul_duration) damu = (damu + 1) / 2;
		}
		if (adtyp == AD_FIRE) (void) burnarmor(&youmonst, FALSE);
		if(uhurt == 2){
			destroy_item(&youmonst, SCROLL_CLASS, (int) adtyp);
			destroy_item(&youmonst, SPBOOK_CLASS, (int) adtyp);
			destroy_item(&youmonst, POTION_CLASS, (int) adtyp);
			destroy_item(&youmonst, RING_CLASS, (int) adtyp);
			destroy_item(&youmonst, WAND_CLASS, (int) adtyp);
		}

		if(adtyp == AD_WET){
			water_damage(invent, FALSE, FALSE, FALSE, mtmp);
		}
		else if (adtyp == AD_BLUD){
			water_damage(invent, FALSE, FALSE, WD_BLOOD, mtmp);
			if(!yours){
				u.umadness |= MAD_APOSTASY;
				change_usanity(-rnd(6), FALSE);
			}
		}
		ugolemeffects((int) adtyp, damu);

		if (uhurt == 2 || uhurt == 3) {
			if(adtyp == AD_SLIM && !Slime_res(&youmonst)){
				You("don't feel very well.");
				Slimed = 10L;
				flags.botl = 1;
			}
		    if (Upolyd)
		    	u.mh  -= damu;
		    else
				u.uhp -= damu;
		    flags.botl = 1;
		}

		if (u.uhp <= 0 || (Upolyd && u.mh <= 0)) {
		    if (Upolyd) {
			rehumanize();
		    } else {
			if (olet == MON_EXPLODE) {
			    /* killer handled by caller */
			    if (str != killer_buf && !generic)
				Strcpy(killer_buf, str);
			    killer_format = KILLED_BY_AN;
			} else if (olet != SCROLL_CLASS && yours) {
			    killer_format = NO_KILLER_PREFIX;
			    Sprintf(killer_buf, "caught %sself in %s own %s",
				    uhim(), uhis(), str);
			} else if (olet != BURNING_OIL) {
			    killer_format = KILLED_BY_AN;
			    Strcpy(killer_buf, str);
			} else {
			    killer_format = KILLED_BY;
			    Strcpy(killer_buf, str);
			}
			killer = killer_buf;
			/* Known BUG: BURNING suppresses corpse in bones data,
			   but done does not handle killer reason correctly */
			done((adtyp == AD_FIRE) ? BURNING : DIED);
		    }
		}
		if(uhurt == 2) exercise(A_STR, FALSE);
	}

	if (shopdamage) {
		pay_for_damage(adtyp == AD_FIRE ? "burn away" :
			       adtyp == AD_COLD ? "shatter" :
			       adtyp == AD_DISN ? "disintegrate" : "destroy",
			       FALSE);
	}

	/* explosions are noisy */
	i = dam * dam;
	if (i < 50) i = 50;	/* in case random damage is very small */
	wake_nearto_noisy(x, y, 2*i);
}
#endif /* OVL0 */
#ifdef OVL1

struct scatter_chain {
	struct scatter_chain *next;	/* pointer to next scatter item	*/
	struct obj *obj;		/* pointer to the object	*/
	xchar ox;			/* location of			*/
	xchar oy;			/*	item			*/
	schar dx;			/* direction of			*/
	schar dy;			/*	travel			*/
	int range;			/* range of object		*/
	boolean stopped;		/* flag for in-motion/stopped	*/
};

/*
 * scflags:
 *	VIS_EFFECTS	Add visual effects to display
 *	MAY_HITMON	Objects may hit monsters
 *	MAY_HITYOU	Objects may hit hero
 *	MAY_HIT		Objects may hit you or monsters
 *	MAY_DESTROY	Objects may be destroyed at random
 *	MAY_FRACTURE	Stone objects can be fractured (statues, boulders)
 */

/* returns number of scattered objects */
long
scatter(sx,sy,blastforce,scflags, obj, loss, shkp)
int sx,sy;				/* location of objects to scatter */
int blastforce;				/* force behind the scattering	*/
unsigned int scflags;
struct obj *obj;			/* only scatter this obj        */
long *loss;				/* report $ value of damage caused here if non-null */
struct monst *shkp;		/* shopkeepr that owns the object (may be null) */
{
	register struct obj *otmp;
	register int tmp;
	int farthest = 0;
	uchar typ;
	long qtmp;
	boolean used_up;
	boolean individual_object = obj ? TRUE : FALSE;
	struct monst *mtmp;
	struct scatter_chain *stmp, *stmp2 = 0;
	struct scatter_chain *schain = (struct scatter_chain *)0;
	long total = 0L;

	while ((otmp = individual_object ? obj : level.objects[sx][sy]) != 0) {
	    if (otmp->quan > 1L) {
		qtmp = otmp->quan - 1;
		if (qtmp > LARGEST_INT) qtmp = LARGEST_INT;
		qtmp = (long)rnd((int)qtmp);
		otmp = splitobj(otmp, qtmp);
	    } else {
		obj = (struct obj *)0; /* all used */
	    }
	    obj_extract_self(otmp);
	    used_up = FALSE;

	    /* 9 in 10 chance of fracturing boulders or statues */
	    if ((scflags & MAY_FRACTURE)
			&& ((otmp->otyp == BOULDER) || (otmp->otyp == STATUE))
			&& rn2(10)) {
		if (otmp->otyp == BOULDER) {
		    pline("%s apart.", Tobjnam(otmp, "break"));
			if(shkp){
				int loss_cost = stolen_value(otmp, otmp->ox, otmp->oy, (boolean)shkp->mpeaceful, TRUE);
				if(loss)
					*loss += loss_cost;
			}
		    break_boulder(otmp);
		    place_object(otmp, sx, sy);
		    if ((otmp = boulder_at(sx, sy)) != 0) {
			/* another boulder here, restack it to the top */
			obj_extract_self(otmp);
			place_object(otmp, sx, sy);
		    }
		} else {
		    struct trap *trap;

		    if ((trap = t_at(sx,sy)) && trap->ttyp == STATUE_TRAP)
			    deltrap(trap);
		    pline("%s.", Tobjnam(otmp, "crumble"));
		    (void) break_statue(otmp);
		    place_object(otmp, sx, sy);	/* put fragments on floor */
		}
		used_up = TRUE;

	    /* 1 in 10 chance of destruction of obj; glass, egg destruction */
	    } else if ((scflags & MAY_DESTROY) && (!rn2(10)
			|| otmp->obj_material == GLASS
			|| otmp->obj_material == OBSIDIAN_MT
			|| otmp->otyp == EGG
			|| otmp == uchain)
		){
			if (breaktest(otmp)){
				if(shkp){
					int loss_cost = stolen_value(otmp, sx, sy, (boolean)shkp->mpeaceful, TRUE);
					if(loss)
						*loss += loss_cost;
				}
				breakobj(otmp, sx, sy, FALSE, FALSE);
				used_up = TRUE;
			}
			else if (otmp == uchain) {
				unpunish();
				used_up = TRUE;
			}
	    }

	    if (!used_up) {
		stmp = (struct scatter_chain *)
					alloc(sizeof(struct scatter_chain));
		stmp->next = (struct scatter_chain *)0;
		stmp->obj = otmp;
		stmp->ox = sx;
		stmp->oy = sy;
		tmp = rn2(8);		/* get the direction */
		stmp->dx = xdir[tmp];
		stmp->dy = ydir[tmp];
		tmp = blastforce - (otmp->owt/40);
		if (tmp < 1) tmp = 1;
		stmp->range = rnd(tmp); /* anywhere up to that determ. by wt */
		if (otmp == uchain || otmp == uball) stmp->range = 0;
		if (farthest < stmp->range) farthest = stmp->range;
		stmp->stopped = FALSE;
		if (!schain)
		    schain = stmp;
		else
		    stmp2->next = stmp;
		stmp2 = stmp;
	    }
	}

	while (farthest-- > 0) {
		for (stmp = schain; stmp; stmp = stmp->next) {
		   if ((stmp->range-- > 0) && (!stmp->stopped)) {
			bhitpos.x = stmp->ox + stmp->dx;
			bhitpos.y = stmp->oy + stmp->dy;
			typ = levl[bhitpos.x][bhitpos.y].typ;
			if(!isok(bhitpos.x, bhitpos.y)) {
				bhitpos.x -= stmp->dx;
				bhitpos.y -= stmp->dy;
				stmp->stopped = TRUE;
			} else if(!ZAP_POS(typ) ||
					closed_door(bhitpos.x, bhitpos.y)) {
				bhitpos.x -= stmp->dx;
				bhitpos.y -= stmp->dy;
				stmp->stopped = TRUE;
			} else if ((mtmp = m_at(bhitpos.x, bhitpos.y)) != 0) {
				if (scflags & MAY_HITMON) {
					struct obj ** s_p = &(stmp->obj);
					int loss_cost = 0;
				    stmp->range--;
					int dieroll = rnd(20);
					if (tohitval((struct monst *)0, mtmp, (struct attack *)0, stmp->obj, (void *)0, HMON_FIRED, 0, (int *) 0) > dieroll || dieroll == 1) {
						if (shkp) loss_cost = stolen_value(stmp->obj, sx, sy, TRUE, TRUE);
						(void)hmon_with_unowned_obj(mtmp, s_p, dieroll);
					}
					else
						miss(xname(stmp->obj), mtmp);
					if (!(*s_p)) {
						*loss += loss_cost;
						stmp->obj = (struct obj *)0;
						stmp->stopped = TRUE;
				    }
				}
			} else if (bhitpos.x==u.ux && bhitpos.y==u.uy) {
				if (scflags & MAY_HITYOU) {
				    if (multi) nomul(0, NULL);
					struct obj ** s_p = &(stmp->obj);
					int loss_cost = 0;
					int hitu, hitvalu;
					int dieroll;
					hitvalu = tohitval((struct monst *)0, &youmonst, (struct attack *)0, stmp->obj, (void *)0, HMON_FIRED, 8, (int *) 0);
					if (hitvalu > (dieroll = rnd(20)) || dieroll == 1) {
						if (shkp) loss_cost = stolen_value(stmp->obj, sx, sy, TRUE, TRUE);
						killer = "flying object";
						killer_format = KILLED_BY_AN;
						(void)hmon_with_unowned_obj(&youmonst, s_p, dieroll);
						stmp->range -= 3;
						stop_occupation();
					}
					else {
						if (Blind || !flags.verbose) pline("It misses.");
						else You("are almost hit by %s.", the(xname(stmp->obj)));
					}
					if (!(*s_p)) {
						*loss += loss_cost;
						stmp->obj = (struct obj *)0;
						stmp->stopped = TRUE;
				    }
				}
			} else {
				if (scflags & VIS_EFFECTS) {
				    /* tmp_at(bhitpos.x, bhitpos.y); */
				    /* delay_output(); */
				}
			}
			stmp->ox = bhitpos.x;
			stmp->oy = bhitpos.y;
		   }
		}
	}
	for (stmp = schain; stmp; stmp = stmp2) {
		int x,y;

		stmp2 = stmp->next;
		x = stmp->ox; y = stmp->oy;
		if (stmp->obj) {
			if ( x!=sx || y!=sy )
			    total += stmp->obj->quan;
			place_object(stmp->obj, x, y);
			stackobj(stmp->obj);
		}
		free((genericptr_t)stmp);
		newsym(x,y);
	}

	return total;
}


/*
 * Splatter burning oil from x,y to the surrounding area.
 *
 * This routine should really take a how and direction parameters.
 * The how is how it was caused, e.g. kicked verses thrown.  The
 * direction is which way to spread the flaming oil.  Different
 * "how"s would give different dispersal patterns.  For example,
 * kicking a burning flask will splatter differently from a thrown
 * flask hitting the ground.
 *
 * For now, just perform a "regular" explosion.
 */
void
splatter_burning_oil(x, y)
    int x, y;
{
    explode(x, y, AD_FIRE, BURNING_OIL, d(4,4), EXPL_FIERY, 1);
}

#ifdef FIREARMS

#define BY_OBJECT       ((struct monst *)0)

STATIC_DCL int
dp(n, p)		/* 0 <= dp(n, p) <= n */
int n, p;
{
    int tmp = 0;
    while (n--) tmp += !rn2(p);
    return tmp;
}

#define GRENADE_TRIGGER(obj)	\
    if ((obj)->otyp == FRAG_GRENADE) { \
	delquan = dp((obj)->quan, 10); \
	no_fiery += delquan; \
    } else if ((obj)->otyp == GAS_GRENADE) { \
	delquan = dp((obj)->quan, 10); \
	no_gas += delquan; \
    } else if ((obj)->otyp == STICK_OF_DYNAMITE) { \
	delquan = (obj)->quan; \
	no_fiery += (obj)->quan * 2; \
	no_dig += (obj)->quan; \
    } else if (is_bullet(obj)) \
	delquan = (obj)->quan; \
    else \
	delquan = 0

struct grenade_callback {
    ExplodeRegion *fiery_area, *gas_area, *dig_area;
    boolean isyou;
};

STATIC_DCL void FDECL(grenade_effects, (struct obj *,XCHAR_P,XCHAR_P,
	ExplodeRegion *,ExplodeRegion *,ExplodeRegion *,BOOLEAN_P));

STATIC_DCL int
grenade_fiery_callback(data, x, y)
genericptr_t data;
int x, y;
{
    int is_accessible = ZAP_POS(levl[x][y].typ);
    struct grenade_callback *gc = (struct grenade_callback *)data;
    if (is_accessible) {
	add_location_to_explode_region(x, y, gc->fiery_area);
	grenade_effects((struct obj *)0, x, y,
		gc->fiery_area, gc->gas_area, gc->dig_area, gc->isyou);
    }
    return !is_accessible;
}

STATIC_DCL int
grenade_gas_callback(data, x, y)
genericptr_t data;
int x, y;
{
    int is_accessible = ZAP_POS(levl[x][y].typ);
    struct grenade_callback *gc = (struct grenade_callback *)data;
    if (is_accessible)
	add_location_to_explode_region(x, y, gc->gas_area);
    return !is_accessible;
}

STATIC_DCL int
grenade_dig_callback(data, x, y)
genericptr_t data;
int x, y;
{
    struct grenade_callback *gc = (struct grenade_callback *)data;
    if (dig_check(BY_OBJECT, FALSE, x, y))
	add_location_to_explode_region(x, y, gc->dig_area);
    return !ZAP_POS(levl[x][y].typ);
}

STATIC_DCL void
grenade_effects(source, x, y, fiery_area, gas_area, dig_area, isyou)
struct obj *source;
xchar x, y;
ExplodeRegion *fiery_area, *gas_area, *dig_area;
boolean isyou;
{
    int i, r;
    struct obj *obj, *obj2;
    struct monst *mon;
    /*
     * Note: These count explosive charges in arbitary units. Grenades
     *       are counted as 1 and sticks of dynamite as 2 fiery and 1 dig.
     */
    int no_gas = 0, no_fiery = 0, no_dig = 0;
    int delquan;
    boolean shielded = FALSE, redraw;
    struct grenade_callback gc;

    if (source) {
	if (source->otyp == GAS_GRENADE)
	    no_gas += source->quan;
	else if (source->otyp == FRAG_GRENADE)
	    no_fiery += source->quan;
	else if (source->otyp == STICK_OF_DYNAMITE) {
	    no_fiery += source->quan * 2;
	    no_dig += source->quan;
	}
	redraw = source->where == OBJ_FLOOR;
	obj_extract_self(source);
	obfree(source, (struct obj *)0);
	if (redraw) newsym(x, y);
    }
    mon = m_at(x, y);
#ifdef STEED
    if (!mon && x == u.ux && y == u.uy)
	mon = u.usteed;
#endif
    if (mon && !DEADMONSTER(mon)) {
		if (resists_fire(mon)) {
		    shielded = TRUE;
		} else {
		    for(obj = mon->minvent; obj; obj = obj2) {
			obj2 = obj->nobj;
			GRENADE_TRIGGER(obj);
			for(i = 0; i < delquan; i++)
			    m_useup(mon, obj);
		    }
		}
    }
    if (x == u.ux && y == u.uy) {
		if (Fire_resistance) {
		    shielded = TRUE;
		} else {
		    for(obj = invent; obj; obj = obj2) {
			obj2 = obj->nobj;
			GRENADE_TRIGGER(obj);
			for(i = 0; i < delquan; i++)
			    useup(obj);
		    }
		}
    }
    if (!shielded)
	for(obj = level.objects[x][y]; obj; obj = obj2) {
	    obj2 = obj->nexthere;
	    GRENADE_TRIGGER(obj);
	    if (delquan) {
		if (isyou)
		    useupf(obj, delquan);
		else if (delquan < obj->quan)
		    obj->quan -= delquan;
		else
		    delobj(obj);
	    }
	}
    gc.fiery_area = fiery_area;
    gc.gas_area = gas_area;
    gc.dig_area = dig_area;
    gc.isyou = isyou;
    if (no_gas) {
	/* r = floor(log2(n))+1 */
	r = 0;
	while(no_gas) {
	    r++;
	    no_gas /= 2;
	}
	xpathto(r, x, y, grenade_gas_callback, (genericptr_t)&gc);
    }
    if (no_fiery) {
	/* r = floor(log2(n))+1 */
	r = 0;
	while(no_fiery) {
	    r++;
	    no_fiery /= 2;
	}
	xpathto(r, x, y, grenade_fiery_callback, (genericptr_t)&gc);
    }
    if (no_dig) {
	/* r = floor(log2(n))+1 */
	r = 0;
	while(no_dig) {
	    r++;
	    no_dig /= 2;
	}
	xpathto(r, x, y, grenade_dig_callback, (genericptr_t)&gc);
    }
}

/*
 * Note: obj is not valid after return
 */

void
grenade_explode(obj, x, y, isyou, dest)
struct obj *obj;
int x, y;
boolean isyou;
int dest;
{
    int i, adtyp;
    boolean shop_damage = FALSE;
    int ox, oy;
    ExplodeRegion *fiery_area, *gas_area, *dig_area;
    struct trap *trap;
    
    fiery_area = create_explode_region();
    gas_area = create_explode_region();
    dig_area = create_explode_region();
    grenade_effects(obj, x, y, fiery_area, gas_area, dig_area, isyou);
    if (fiery_area->nlocations) {
	adtyp = AD_FIRE;
	do_explode(x, y, fiery_area, adtyp, WEAPON_CLASS, d(3,6), EXPL_FIERY, dest, isyou);
    }
    wake_nearto(x, y, 400);
    /* Like cartoons - the explosion first, then
     * the world deals with the holes produced ;)
     */
    for(i = 0; i < dig_area->nlocations; i++) {
	ox = dig_area->locations[i].x;
	oy = dig_area->locations[i].y;
	if (IS_WALL(levl[ox][oy].typ) || IS_DOOR(levl[ox][oy].typ)) {
	    watch_dig((struct monst *)0, ox, oy, TRUE);
	    if (*in_rooms(ox, oy, SHOPBASE)) shop_damage = TRUE;
	}
	digactualhole(ox, oy, BY_OBJECT, PIT, TRUE, FALSE);
    }
    free_explode_region(dig_area);
    for(i = 0; i < fiery_area->nlocations; i++) {
	ox = fiery_area->locations[i].x;
	oy = fiery_area->locations[i].y;
	if ((trap = t_at(ox, oy)) != 0 && trap->ttyp == LANDMINE)
	    blow_up_landmine(trap);
    }
    free_explode_region(fiery_area);
    if (gas_area->nlocations) {
	adtyp = AD_DRST;
	do_explode(x, y, gas_area, adtyp, WEAPON_CLASS, d(3,6),
	  EXPL_NOXIOUS, dest, isyou);
    }
    free_explode_region(gas_area);
    if (shop_damage) pay_for_damage("damage", FALSE);
}

void arm_bomb(obj, yours)
struct obj *obj;
boolean yours;
{
	if (is_grenade(obj)) {
		attach_bomb_blow_timeout(obj, 
			    (obj->cursed ? rn2(5) + 2 : obj->blessed ? 4 : 
			    	rn2(2) + 3)
			     , yours);			
	}
	/* Otherwise, do nothing */
}

#endif /* FIREARMS */

int
adtyp_expl_color(adtyp)
int adtyp;
{
	switch(adtyp){
		case AD_PHYS:
			return EXPL_MUDDY;
		case AD_EFIR:
		case AD_FIRE:
			return EXPL_FIERY;
		case AD_ECLD:
		case AD_COLD:
			return EXPL_FROSTY;
		case AD_EELC:
		case AD_ELEC:
			return EXPL_MAGICAL;
		case AD_DISE:
		case AD_DRST:
			return EXPL_MAGENTA;
		case AD_ACID:
		case AD_EACD:
		case AD_SLIM:
			return EXPL_NOXIOUS;
		case AD_DARK:
			return EXPL_DARK;
		case AD_WET:
			return EXPL_WET;
		case AD_BLUD:
			return EXPL_RED;
		default:
			impossible("unhandled explosion color for attack damage type %d", adtyp);
			return EXPL_MAGICAL;
	}
}
#endif /* OVL1 */

/*explode.c*/
