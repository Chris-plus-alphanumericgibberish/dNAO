/*	SCCS Id: @(#)light.c	3.4	1997/04/10	*/
/* Copyright (c) Dean Luick, 1994					*/
/* NetHack may be freely redistributed.  See license for details.	*/

#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"

#include "hack.h"
#include "lev.h"	/* for checking save modes */

/*
 * Mobile light sources.
 *
 * This implementation minimizes memory at the expense of extra
 * recalculations.
 *
 * Light sources are "things" that have a physical position and range.
 * They have a type, which gives us information about them.  Currently
 * they are only attached to objects and monsters.  Note well:  the
 * polymorphed-player handling assumes that both youmonst.m_id and
 * youmonst.mx will always remain 0.
 *
 * Light sources, like timers, either follow game play (RANGE_GLOBAL) or
 * stay on a level (RANGE_LEVEL).  Light sources are unique by their
 * (type, id) pair.  For light sources attached to objects, this id
 * is a pointer to the object.
 *
 * The major working function is do_light_sources(). It is called
 * when the vision system is recreating its "could see" array.  Here
 * we add a flag (TEMP_LIT#) to the array for all locations that are lit
 * via a light source.  The bad part of this is that we have to
 * re-calculate the LOS of each light source every time the vision
 * system runs.  Even if the light sources and any topology (vision blocking
 * positions) have not changed.  The good part is that no extra memory
 * is used, plus we don't have to figure out how far the sources have moved,
 * or if the topology has changed.
 *
 * The structure of the save/restore mechanism is amazingly similar to
 * the timer save/restore.  This is because they both have the same
 * principals of having pointers into objects that must be recalculated
 * across saves and restores.
 */

#ifdef OVL3

/* flags */
#define LSF_SHOW	0x1		/* display the light source */
#define LSF_NEEDS_FIXUP	0x2		/* need oid fixup */

static struct ls_t *light_base = 0;

STATIC_DCL void FDECL(add_chain_ls, (struct ls_t *));
STATIC_DCL void FDECL(rem_chain_ls, (struct ls_t *));
STATIC_DCL void FDECL(write_ls, (int, struct ls_t *));
STATIC_DCL int FDECL(maybe_write_ls, (int, int, BOOLEAN_P));

/* imported from vision.c, for small circles */
extern char circle_data[];
extern int circle_start[];

#define owner_ls(lstype, owner) (\
	(lstype) == LS_OBJECT  ? &(((struct obj *)(owner))->light) : \
	(lstype) == LS_MONSTER ? &(((struct monst *)(owner))->light) : \
	(struct ls_t **)0)

/* adds an existing light source to the processing chain */
void
add_chain_ls(ls)
struct ls_t * ls;
{
	struct ls_t * lstmp;
	/* check for duplicates */
	for (lstmp = light_base; lstmp; lstmp = lstmp->next) {
		if (lstmp == ls) {
			impossible("ls already in processing chain");
			return;
		}
	}
	ls->next = light_base;
	light_base = ls;
	return;
}

/* removes a light source from the procesing chain */
void
rem_chain_ls(ls)
struct ls_t * ls;
{
	struct ls_t * lstmp;
	if (light_base == ls) {
		light_base = light_base->next;
		return;
	}
	else for (lstmp = light_base; lstmp; lstmp = lstmp->next) {
		if (lstmp->next == ls) {
			lstmp->next = ls->next;
			return;
		}
	}
	return;
}

/* Create a new light source.  */
/* If a lightsource is already attached to (type, id), replace it (with a warning) */
void
new_light_source(lstype, owner, range)
	int lstype;			/* is it attached to a mon or an obj? */
    genericptr_t owner;	/* pointer to mon/obj this is attached to */
	int range;			/* range of ls */
{
    struct ls_t *ls;
	boolean duplicate = FALSE;

    if (range > MAX_RADIUS || range < 0) {
	impossible("new_light_source:  illegal range %d", range);
	return;
    }

	/* check that this is a unique lightsource */
	if ((ls = *owner_ls(lstype, owner)))
		duplicate = TRUE;

	if (duplicate) {
		impossible("duplicate lightsource attempting to be created on %s",
			lstype == LS_OBJECT ? xname(owner) : m_monnam(owner));
	}
	else {
		ls = (struct ls_t *)alloc(sizeof(struct ls_t));
	}
	/* set ls data */
    ls->range = range;
    ls->lstype = lstype;
    ls->owner = owner;
    ls->flags = 0;
	/* set initial location of lightsource */
	switch(lstype) {
	case LS_OBJECT:
		get_obj_location((struct obj *) ls->owner, &ls->x, &ls->y, 0);
		break;
	case LS_MONSTER:
		get_mon_location((struct monst *) ls->owner, &ls->x, &ls->y, 0);
		break;
	}
	/* add to owner */
	*owner_ls(lstype, owner) = ls;
	/* add to processing chain */
	if (!duplicate)
		add_chain_ls(ls);

    vision_full_recalc = 1;	/* make the source show up */
}

/*
 * Delete light source.
 */
void
del_light_source(ls)
struct ls_t * ls;
{
	/* check it exists */
	if (!ls) return;
	/* remove from processing chain */
	rem_chain_ls(ls);
	/* need to update vision */
	vision_full_recalc = 1;
	/* clean owner */
	*owner_ls(ls->lstype, ls->owner) = (struct ls_t *)0;
	/* free it */
	free(ls);
	return;
}

/* Mark locations that are temporarily lit via mobile light sources. */
void
do_light_sources(cs_rows)
    char **cs_rows;
{
    int x, y, min_x, max_x, max_y, offset;
    char *limits;
    short at_hero_range = 0;
    struct ls_t *ls;
    char *row;

    for (ls = light_base; ls; ls = ls->next) {
	ls->flags &= ~LSF_SHOW;

	/*
	 * Check for moved light sources.  It may be possible to
	 * save some effort if an object has not moved, but not in
	 * the current setup -- we need to recalculate for every
	 * vision recalc.
	 */
	if (ls->lstype == LS_OBJECT) {
	    if (get_obj_location((struct obj *) ls->owner, &ls->x, &ls->y, 0))
		ls->flags |= LSF_SHOW;
	} else if (ls->lstype == LS_MONSTER) {
	    if (get_mon_location((struct monst *) ls->owner, &ls->x, &ls->y, 0))
		ls->flags |= LSF_SHOW;
	}

	/* occluded lightsources should not be shown */
	/* note: contained objects are not found by get_obj_location()
	 * without a specific flag set, so they don't need extra handling */
	if (ls->flags & LSF_SHOW) {
		if (ls->lstype == LS_OBJECT && (
			(
			/* lightsource is in the stomach of an engulfer */
			mcarried((struct obj *)ls->owner) &&
			attacktype(((struct obj *)ls->owner)->ocarry->data, AT_ENGL))
			||
			(
			/* lightsource carried by player, who is engulfed */
			carried((struct obj *)ls->owner) &&
			u.uswallow)
			)){
			ls->flags &= ~LSF_SHOW;
		}
		else if (ls->lstype == LS_MONSTER && (
			(
			/* lightsource is the player; player is engulfed */
			u.uswallow &&
			(((struct monst *)ls->owner) == &youmonst))
			)){
			ls->flags &= ~LSF_SHOW;
		}
	}

	/* Candle ranges need to be recomputed to allow altar effects */
	if (ls->lstype == LS_OBJECT) {
		struct obj *otmp = (struct obj *) ls->owner;
		if (Is_candle(otmp)) {
			ls->range = candle_light_range(otmp);
		}
    }

	if ((ls->flags & LSF_SHOW) && ls->range > 0) {
		if((ls->lstype == LS_OBJECT && Is_darklight_source(((struct obj *)(ls->owner)))) ||
			(ls->lstype == LS_MONSTER && Is_darklight_monster(((struct monst *)(ls->owner))->data))
		){
			int range = ls->range;
			/*
			 * Walk the points in the circle and see if they are
			 * visible from the center.  If so, mark'em.
			 *
			 * Kevin's tests indicated that doing this brute-force
			 * method is faster for radius <= 3 (or so).
			 */
			limits = circle_ptr(range);
			if ((max_y = (ls->y + range)) >= ROWNO) max_y = ROWNO-1;
			if ((y = (ls->y - range)) < 0) y = 0;
			for (; y <= max_y; y++) {
			row = cs_rows[y];
			offset = limits[abs(y - ls->y)];
			if ((min_x = (ls->x - offset)) < 0) min_x = 0;
			if ((max_x = (ls->x + offset)) >= COLNO) max_x = COLNO-1;

			if (ls->x == u.ux && ls->y == u.uy) {
				/*
				 * If the light source is located at the hero, then
				 * we can use the COULD_SEE bits already calcualted
				 * by the vision system.  More importantly than
				 * this optimization, is that it allows the vision
				 * system to correct problems with clear_path().
				 * The function clear_path() is a simple LOS
				 * path checker that doesn't go out of its way
				 * make things look "correct".  The vision system
				 * does this.
				 */
				for (x = min_x; x <= max_x; x++)
				if (row[x] & COULD_SEE)
					row[x] |= TEMP_DRK1;
			} else {
				for (x = min_x; x <= max_x; x++)
				if ((ls->x == x && ls->y == y)
					|| clear_path((int)ls->x, (int) ls->y, x, y))
					row[x] |= TEMP_DRK1;
			}
			}
			range = max(ls->range*2/3,1);
			limits = circle_ptr(range);
			if ((max_y = (ls->y + range)) >= ROWNO) max_y = ROWNO-1;
			if ((y = (ls->y - range)) < 0) y = 0;
			for (; y <= max_y; y++) {
			row = cs_rows[y];
			offset = limits[abs(y - ls->y)];
			if ((min_x = (ls->x - offset)) < 0) min_x = 0;
			if ((max_x = (ls->x + offset)) >= COLNO) max_x = COLNO-1;

			if (ls->x == u.ux && ls->y == u.uy) {
				/*
				 * If the light source is located at the hero, then
				 * we can use the COULD_SEE bits already calcualted
				 * by the vision system.  More importantly than
				 * this optimization, is that it allows the vision
				 * system to correct problems with clear_path().
				 * The function clear_path() is a simple LOS
				 * path checker that doesn't go out of its way
				 * make things look "correct".  The vision system
				 * does this.
				 */
				for (x = min_x; x <= max_x; x++)
				if (row[x] & COULD_SEE)
					row[x] |= TEMP_DRK2;
			} else {
				for (x = min_x; x <= max_x; x++)
				if ((ls->x == x && ls->y == y)
					|| clear_path((int)ls->x, (int) ls->y, x, y))
					row[x] |= TEMP_DRK2;
			}
			}
			range = max(ls->range*1/3,1);
			limits = circle_ptr(range);
			if ((max_y = (ls->y + range)) >= ROWNO) max_y = ROWNO-1;
			if ((y = (ls->y - range)) < 0) y = 0;
			for (; y <= max_y; y++) {
			row = cs_rows[y];
			offset = limits[abs(y - ls->y)];
			if ((min_x = (ls->x - offset)) < 0) min_x = 0;
			if ((max_x = (ls->x + offset)) >= COLNO) max_x = COLNO-1;

			if (ls->x == u.ux && ls->y == u.uy) {
				/*
				 * If the light source is located at the hero, then
				 * we can use the COULD_SEE bits already calcualted
				 * by the vision system.  More importantly than
				 * this optimization, is that it allows the vision
				 * system to correct problems with clear_path().
				 * The function clear_path() is a simple LOS
				 * path checker that doesn't go out of its way
				 * make things look "correct".  The vision system
				 * does this.
				 */
				for (x = min_x; x <= max_x; x++)
				if (row[x] & COULD_SEE)
					row[x] |= TEMP_DRK3;
			} else {
				for (x = min_x; x <= max_x; x++)
				if ((ls->x == x && ls->y == y)
					|| clear_path((int)ls->x, (int) ls->y, x, y))
					row[x] |= TEMP_DRK3;
			}
			}
		} else {
			int range = ls->range;
			/*
			 * Walk the points in the circle and see if they are
			 * visible from the center.  If so, mark'em.
			 *
			 * Kevin's tests indicated that doing this brute-force
			 * method is faster for radius <= 3 (or so).
			 */
			limits = circle_ptr(range);
			if ((max_y = (ls->y + range)) >= ROWNO) max_y = ROWNO-1;
			if ((y = (ls->y - range)) < 0) y = 0;
			for (; y <= max_y; y++) {
			row = cs_rows[y];
			offset = limits[abs(y - ls->y)];
			if ((min_x = (ls->x - offset)) < 0) min_x = 0;
			if ((max_x = (ls->x + offset)) >= COLNO) max_x = COLNO-1;

			if (ls->x == u.ux && ls->y == u.uy) {
				/*
				 * If the light source is located at the hero, then
				 * we can use the COULD_SEE bits already calcualted
				 * by the vision system.  More importantly than
				 * this optimization, is that it allows the vision
				 * system to correct problems with clear_path().
				 * The function clear_path() is a simple LOS
				 * path checker that doesn't go out of its way
				 * make things look "correct".  The vision system
				 * does this.
				 */
				for (x = min_x; x <= max_x; x++)
				if (row[x] & COULD_SEE)
					row[x] |= TEMP_LIT1;
			} else {
				for (x = min_x; x <= max_x; x++)
				if ((ls->x == x && ls->y == y)
					|| clear_path((int)ls->x, (int) ls->y, x, y))
					row[x] |= TEMP_LIT1;
			}
			}
			range = ls->range*2;
			limits = circle_ptr(range);
			if ((max_y = (ls->y + range)) >= ROWNO) max_y = ROWNO-1;
			if ((y = (ls->y - range)) < 0) y = 0;
			for (; y <= max_y; y++) {
			row = cs_rows[y];
			offset = limits[abs(y - ls->y)];
			if ((min_x = (ls->x - offset)) < 0) min_x = 0;
			if ((max_x = (ls->x + offset)) >= COLNO) max_x = COLNO-1;

			if (ls->x == u.ux && ls->y == u.uy) {
				/*
				 * If the light source is located at the hero, then
				 * we can use the COULD_SEE bits already calcualted
				 * by the vision system.  More importantly than
				 * this optimization, is that it allows the vision
				 * system to correct problems with clear_path().
				 * The function clear_path() is a simple LOS
				 * path checker that doesn't go out of its way
				 * make things look "correct".  The vision system
				 * does this.
				 */
				for (x = min_x; x <= max_x; x++)
				if (row[x] & COULD_SEE)
					row[x] |= TEMP_LIT2;
			} else {
				for (x = min_x; x <= max_x; x++)
				if ((ls->x == x && ls->y == y)
					|| clear_path((int)ls->x, (int) ls->y, x, y))
					row[x] |= TEMP_LIT2;
			}
			}
			range = ls->range*3;
			limits = circle_ptr(range);
			if ((max_y = (ls->y + range)) >= ROWNO) max_y = ROWNO-1;
			if ((y = (ls->y - range)) < 0) y = 0;
			for (; y <= max_y; y++) {
			row = cs_rows[y];
			offset = limits[abs(y - ls->y)];
			if ((min_x = (ls->x - offset)) < 0) min_x = 0;
			if ((max_x = (ls->x + offset)) >= COLNO) max_x = COLNO-1;

			if (ls->x == u.ux && ls->y == u.uy) {
				/*
				 * If the light source is located at the hero, then
				 * we can use the COULD_SEE bits already calcualted
				 * by the vision system.  More importantly than
				 * this optimization, is that it allows the vision
				 * system to correct problems with clear_path().
				 * The function clear_path() is a simple LOS
				 * path checker that doesn't go out of its way
				 * make things look "correct".  The vision system
				 * does this.
				 */
				for (x = min_x; x <= max_x; x++)
				if (row[x] & COULD_SEE)
					row[x] |= TEMP_LIT3;
			} else {
				for (x = min_x; x <= max_x; x++)
				if ((ls->x == x && ls->y == y)
					|| clear_path((int)ls->x, (int) ls->y, x, y))
					row[x] |= TEMP_LIT3;
			}
			}
		}
	}
    }
}

/* returns true if a swallowed player is in the dark
 *
 * needed for things that depend on the player being in/out of light
 * ex) Orthos binding, ex) shadowsteel equipment
 */
boolean
uswallow_indark()
{
	if (!u.uswallow || !u.ustuck) {
		return (dimness(u.ux, u.uy) > 0);
	}

	boolean tlit = FALSE;
	boolean tdark = FALSE;

	/* being inside a monster that emits light counts as a temp ls */
	if (emits_light_mon(u.ustuck)) {
		if (Is_darklight_monster(u.ustuck->data))
			tdark = TRUE;
		else
			tlit = TRUE;
	}
	/* being a creature that emits light counts as a temp ls */
	if (emits_light(youracedata)) {
		if (Is_darklight_monster(youracedata))
			tdark = TRUE;
		else
			tlit = TRUE;
	}
	/* your carried lightsources */
	struct ls_t *ls;
	for (ls = light_base; ls; ls = ls->next) {
		if ((ls->lstype == LS_OBJECT) &&
			(ls->x == u.ux) &&
			(ls->y == u.uy) &&
			(carried(((struct obj *)ls->owner))))
		{
			if (Is_darklight_source(((struct obj *)(ls->owner))))
				tdark = TRUE;
			else
				tlit = TRUE;
		}
	}
	return (tdark || !tlit);
}

/* (mon->mx == 0) implies migrating */
#define mon_is_local(mon)	((mon)->mx > 0)

struct monst *
find_mid(nid, fmflags)
unsigned nid;
unsigned fmflags;
{
	struct monst *mtmp;

	if (!nid)
	    return &youmonst;
	if (fmflags & FM_FMON)
		for (mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		    if (!DEADMONSTER(mtmp) && mtmp->m_id == nid) return mtmp;
	if (fmflags & FM_MIGRATE)
		for (mtmp = migrating_mons; mtmp; mtmp = mtmp->nmon)
	    	    if (mtmp->m_id == nid) return mtmp;
	if (fmflags & FM_MYDOGS)
		for (mtmp = mydogs; mtmp; mtmp = mtmp->nmon)
	    	    if (mtmp->m_id == nid) return mtmp;
	return (struct monst *) 0;
}

void
save_lightsource(ls, fd, mode)
struct ls_t * ls;
int fd;
int mode;
{
	if (perform_bwrite(mode))
	    bwrite(fd, (genericptr_t)ls, sizeof(struct ls_t));
	if (release_data(mode))
		del_light_source(ls);
	return;
}

void
rest_lightsource(lstype, owner, ls, fd, ghostly)
int lstype;
genericptr_t owner;
struct ls_t * ls;
int fd;
boolean ghostly;	/* unused */
{
	ls = (struct ls_t *)alloc(sizeof(struct ls_t));
	mread(fd, (genericptr_t) ls, sizeof(struct ls_t));
	add_chain_ls(ls);
	/* relink owner */
	ls->owner = owner;
	*owner_ls(lstype, owner) = ls;
	return;
}

/* return true if there exist any light sources */
boolean
any_light_source()
{
    return light_base != (struct ls_t *) 0;
}

/*
 * Snuff an object light source if at (x,y).  This currently works
 * only for burning light sources.
 */
void
snuff_light_source(x, y)
    int x, y;
{
    struct ls_t *ls, *nls;
    struct obj *obj;

    for (ls = light_base; ls; ls = nls){
		nls = ls->next;
		/*
		Is this position check valid??? Can I assume that the positions
		will always be correct because the objects would have been
		updated with the last vision update?  [Is that recent enough???]
		*/
		if (ls->lstype == LS_OBJECT && ls->x == x && ls->y == y) {
			obj = (struct obj *) ls->owner;
			if (obj_is_burning(obj) && !Darkness_cant_snuff(obj)) {
				// this assumes snuff_light_source is only called on you're making DARKNESS,
				// never when you're making LIGHT sources to "snuff" darkness sources (shadow torches)
				end_burn(obj, obj->otyp != MAGIC_LAMP);
			}
		}
	}
}

/* Return TRUE if object is currently shedding any light or darkness. */
boolean
obj_sheds_light(obj)
struct obj *obj;
{
	return (obj->lamplit && (						/* lamplit is sometimes off for even eternal lightsources */
		obj_is_burning(obj) ||						/* standard lightsources that must be lit */
		artifact_light(obj) ||						/* sometimes active artifact lightsource */
		obj_eternal_light(obj)						/* object should always be shedding light (except when occluded) */
		));
}

/* Return TRUE if object's light should in theory never go out */
boolean
obj_eternal_light(obj)
struct obj * obj;
{
	return (
		arti_light(obj) ||							/* artifact lightsource */
		obj->otyp == POT_STARLIGHT ||				/* always lit potion */
		obj->otyp == CHUNK_OF_FOSSIL_DARK ||		/* always dark rock */
		obj->otyp == SUNLIGHT_MAGGOT ||				/* always lit hat */
		(obj->otyp == SUNROD && obj->lamplit)		/* chemical reaction cannot be snuffed */
		);
}

/* Return TRUE if sheds light AND will be snuffed by end_burn(). */
/* These _usually_ may also be ended by snuff_light_source(), the overlap is close enough to not warrant a 2nd function */
boolean
obj_is_burning(obj)
    struct obj *obj;
{
    return (obj->lamplit &&
		 (	ignitable(obj)					/* lightsource uses a flame */
		 || obj->otyp == LANTERN			/* electric */
		 || obj->otyp == LANTERN_PLATE_MAIL	/* electric */
		 || obj->otyp == DWARVISH_HELM		/* electric */
		 || (is_lightsaber(obj) && obj->oartifact != ART_INFINITY_S_MIRRORED_ARC && obj->otyp != KAMEREL_VAJRA)	/* future-electric */
		 || obj->oartifact == ART_HOLY_MOONLIGHT_SWORD));	/* magical fire */
}

boolean
litsaber(obj)
	struct obj *obj;
{
	if(obj->oartifact == ART_INFINITY_S_MIRRORED_ARC){
		return infinity_s_mirrored_arc_litness(obj);
	} else if(obj->otyp == KAMEREL_VAJRA){
		if(u.goldkamcount_tame || level.flags.goldkamcount_peace || level.flags.goldkamcount_hostile || flags.goldka_level)
			return TRUE;
		else return FALSE;
	} else {
		return obj->lamplit;
	}
}

/* copy the light source attached to src, and attach it to dest */
void
obj_split_light_source(src, dest)
    struct obj *src, *dest;
{
	/* safety check */
	if (!src->light)
		return;

	/* make a new ls on dest */
	new_light_source(LS_OBJECT, (genericptr_t)dest, src->light->range);
	dest->lamplit = 1;		/* now an active light source */

	/* split candles may emit less light than original group */
	if (Is_candle(src)) {
		/* split candles may emit less light than original group */
		src->light->range = candle_light_range(src);
		dest->light->range = candle_light_range(dest);
		vision_full_recalc = 1;	/* in case range changed */
	}
	return;
}

/* light source `src' has been folded into light source `dest';
   used for merging lit candles and adding candle(s) to lit candelabrum */
void
obj_merge_light_sources(src, dest)
struct obj *src, *dest;
{
    struct ls_t *ls;

    /* src == dest implies adding to candelabrum */
    if (src != dest) end_burn(src, TRUE);		/* extinguish candles */

	if (dest->light) {
		dest->light->range = candle_light_range(dest);
		vision_full_recalc = 1;
	}
	return;
}

/* Candlelight is proportional to the number of candles;
   minimum range is 2 rather than 1 for playability. */
int
candle_light_range(obj)
struct obj *obj;
{
    int radius;

    if (obj->otyp == CANDELABRUM_OF_INVOCATION) {
	/*
	 *	The special candelabrum emits more light than the
	 *	corresponding number of candles would.
	 *	 1..3 candles, range 2 (minimum range);
	 *	 4..6 candles, range 3 (normal lamp range);
	 *	    7 candles, range 4 (bright).
	 */
	radius = (obj->spe < 4) ? 2 : (obj->spe < 7) ? 3 : 4;
    } else if (Is_candle(obj)) {
	/*
	 *	Range is incremented by powers of 7 so that it will take
	 *	wizard mode quantities of candles to get more light than
	 *	from a lamp, without imposing an arbitrary limit.
	 *	 1..6   candles, range 2;
	 *	 7..48  candles, range 3;
	 *	49..342 candles, range 4; &c.
	 */
	long n = obj->quan;

	radius = 1;	/* always incremented at least once */
	do {
	    radius++;
	    n /= 7L;
	} while (n > 0L);
	radius += candle_on_altar(obj);
    } else {
	/* get the lightradius -- very important that we catch all candles and the candelabraum before this */
	radius = lightsource_radius(obj);
    }
    return radius;
}

#ifdef WIZARD
extern char *FDECL(fmt_ptr, (const genericptr, char *));  /* from alloc.c */

int
wiz_light_sources()
{
    winid win;
    char buf[BUFSZ], arg_address[20];
    struct ls_t *ls;

    win = create_nhwindow(NHW_MENU);	/* corner text window */
    if (win == WIN_ERR) return 0;

    Sprintf(buf, "Mobile light sources: hero @ (%2d,%2d)", u.ux, u.uy);
    putstr(win, 0, buf);
    putstr(win, 0, "");

    if (light_base) {
	putstr(win, 0, "location range flags  type    id");
	putstr(win, 0, "-------- ----- ------ ----  -------");
	for (ls = light_base; ls; ls = ls->next) {
	    Sprintf(buf, "  %2d,%2d   %2d   0x%04x  %s  %s",
		ls->x, ls->y, ls->range, ls->flags,
		(ls->lstype == LS_OBJECT ? "obj" :
		 ls->lstype == LS_MONSTER ?
		    (mon_is_local((struct monst *)ls->owner) ? "mon" :
		     ((struct monst *)ls->owner == &youmonst) ? "you" :
		     "<m>") :		/* migrating monster */
		 "???"),
		fmt_ptr(ls->owner, arg_address));
	    putstr(win, 0, buf);
	}
    } else
	putstr(win, 0, "<none>");


    display_nhwindow(win, FALSE);
    destroy_nhwindow(win);

    return 0;
}

#endif /* WIZARD */

#endif /* OVL3 */

/*light.c*/
