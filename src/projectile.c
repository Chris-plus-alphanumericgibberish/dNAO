#include "hack.h"
#include "xhity.h"

STATIC_DCL void FDECL(destroy_projectile, (struct monst *, struct obj *));
STATIC_DCL void FDECL(end_projectile, (struct monst *, struct monst *, struct obj *, struct obj *, boolean, boolean));
STATIC_DCL int FDECL(projectile_attack, (struct monst *, struct monst *, struct obj *, struct obj *, boolean, int*, int*, int*, int*, boolean, boolean*));
STATIC_DCL void FDECL(quest_art_swap, (struct obj *, struct monst *, boolean *));
STATIC_DCL void FDECL(sho_obj_return, (struct obj *, int, int));

/* grab some functions from dothrow.c */
extern boolean FDECL(quest_arti_hits_leader, (struct obj *, struct monst *));
extern int FDECL(gem_accept, (struct monst *, struct obj *));
extern void FDECL(check_shop_obj, (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P));
extern void FDECL(breakmsg, (struct obj *, BOOLEAN_P));
extern void FDECL(breakobj, (struct obj *, XCHAR_P, XCHAR_P, BOOLEAN_P, BOOLEAN_P));

/* some damn global variables because passing these as parameters would be a lot to add for something so rarely used.
 * The player threw an object, these save what the player's state was just prior to throwing so it can be restored */
static boolean u_was_twoweap;
static boolean u_was_swallowed;
static long old_wep_mask;

/* 
 * projectile()
 * 
 * Omnibus projectile firing function
 * 
 * returns MM_HIT/MM_DEF_DIED-etc values IF it hit a creature, otherwise returns MM_MISS (0).
 * 
 *  
 */
int
projectile(magr, ammo, launcher, fired, initx, inity, dx, dy, dz, initrange, destroy, verbose)
struct monst * magr;			/* Creature responsible for the projectile. Can be non-existant. */
struct obj * ammo;				/* Projectile object. Must exist. May be in an inventory, or free, or anywhere. */
struct obj * launcher;			/* Launcher for the projectile. Can be non-existant. Implies "fired" is true. */
boolean fired;					/* Whether or not the projectile was fired (ex arrow from a bow). Fired without a launcher is possible (ex AT_ARRW). */
int initx;						/* x; Where the projectile originates from. Does not hit this location. */
int inity;						/* y; Where the projectile originates from. Does not hit this location. */
int dx;							/* x; Direction of projectile's movement */
int dy;							/* y; Direction of projectile's movement */
int dz;							/* z; Direction of projectile's movement */
int initrange;					/* Maximum range for projectile */
boolean destroy;				/* TRUE if projectile should be forced to be destroyed at the end */
boolean verbose;				/* TRUE if messages should be printed even if the player can't see what happened */
{
	boolean youagr = (magr && magr == &youmonst);
	struct obj * thrownobj;		/* singular fired/thrown object */
	boolean onlyone;			/* if ammo only consists of thrownobj */
	boolean wepgone = FALSE;	/* TRUE if thrownobj is destroyed */
	struct monst * mdef = (struct monst *)0;
	int result = 0;
	int range = initrange;
	int boomerang_init;
	bhitpos.x = initx;
	bhitpos.y = inity;

	/* Sanity checks */
	if (!ammo) {
		impossible("projectile() called without ammo?");
		return 0;
	}
	if (launcher && !fired) {
		impossible("projectile() called with launcher but not fired?");
		return 0;
	}
	/* If player is doing the throwing, save some details like if they were twoweaponing and what slot the object was in */
	if (youagr) {
		u_was_twoweap = u.twoweap;
		u_was_swallowed = u.uswallow;
		old_wep_mask = ammo->owornmask;
	}

	/* First, create thrownobj */
	if (ammo->quan > 1L) {
		/* split off 1 object */
		thrownobj = splitobj(ammo, 1L);
		onlyone = FALSE;
	}
	else {
		/* there is just the one being thrown */
		thrownobj = ammo;
		onlyone = TRUE;
	}
	/* clean up where it came from */
	switch (thrownobj->where)
	{
	case OBJ_FREE:		/* object not attached to anything */
		/* nothing required */
		break;
	case OBJ_FLOOR:		/* object on floor */
		obj_extract_self(thrownobj);
		break;
	case OBJ_INVENT:	/* object in the hero's inventory */
		if (onlyone) {
			if (thrownobj->owornmask)
				remove_worn_item(thrownobj, FALSE);
		}
		freeinv(thrownobj);
		break;
	case OBJ_MINVENT:	/* object in a monster inventory */
		if (!magr) {
			impossible("thrownobj in monster's inventory with no monster?");
			return 0;
		}
		if (onlyone) {
			if (MON_WEP(magr) == thrownobj) {
				setmnotwielded(magr, thrownobj);
				MON_NOWEP(magr);
			}
			if (MON_SWEP(magr) == thrownobj) {
				setmnotwielded(magr, thrownobj);
				MON_NOSWEP(magr);
			}
		}
		m_freeinv(thrownobj);
		break;
	default:			/* object elsewhere (migrating? contained?) */
		impossible("unhandled where of thrown item, %d", thrownobj->where);
		break;
	}
	thrownobj->owornmask = 0;
	/* set that it was thrown... if the player threw it */
	if (youagr) {
		thrownobj->was_thrown = TRUE;
	}

	/* mis-throw/fire can change the direction of the projectile */
	if (/* needs a creature to be involved in throwing the projectile */
		magr && (
		/* cursed thrownobj */
		(thrownobj->cursed && !((magr == &youmonst) ? is_weldproof(youracedata) : is_weldproof_mon(magr)))
		||
		/* flintlock (fired by not-you) */
		(magr != &youmonst && fired && launcher && launcher->otyp == FLINTLOCK)
		)
		&&
		/* at a 1/7 chance */
		(dx || dy) && !rn2(7)
		) {
		/* message */
		if (magr == &youmonst){
			if (fired)
				Your("weapon misfires!");
			else
				pline("%s as you throw it!", Tobjnam(thrownobj, "slip"));
		}
		if (canseemon(magr) && flags.verbose) {
			if (fired)
				pline("%s misfires!", Monnam(magr));
			else
				pline("%s as %s throws it!",
					Tobjnam(thrownobj, "slip"), mon_nam(magr));
		}
		/* new direction */
		dx = rn2(3) - 1;
		dy = rn2(3) - 1;
		/* check validity of new direction */
		if (!dx && !dy) {
			end_projectile(magr, (struct monst *)0, thrownobj, launcher, fired, destroy);
			return MM_MISS;
		}
	}

	/* player exercises STR just be throwing heavy things */
	if (youagr && !fired && (
		thrownobj->otyp == BOULDER ||
		(thrownobj->otyp == STATUE && is_boulder(thrownobj)) ||
		thrownobj->otyp == HEAVY_IRON_BALL
		))
	{
		exercise(A_STR, TRUE);
	}

	/* if the player is swallowed, projectile is guaranteed to hit engulfer and no more */
	if (youagr && u.uswallow) {
		mdef = u.ustuck;
		result = projectile_attack(magr, mdef, thrownobj, launcher, fired, &dx, &dy, &range, &initrange, destroy, &wepgone);
		end_projectile(magr, mdef, thrownobj, launcher, fired, wepgone);
		return result;
	}

	/* set projectile glyph to show */
	tmp_at(DISP_FLASH, obj_to_glyph(thrownobj));

	/* initialize boomerang thrown direction */
	if (is_boomerang(thrownobj)) {
		for (boomerang_init = 0; boomerang_init < 8; boomerang_init++)
		if (xdir[boomerang_init] == dx && ydir[boomerang_init] == dy)
			break;
	}

	/* move the projectile loop */
	/* always ends via break */
	while (TRUE)
	{
		/* boomerangs: change dx/dy to make signature circle */
		if (is_boomerang(thrownobj)) {
			/* assumes boomerangs always start with 10 range */
			/* don't worry about the math; it works */
			dx = xdir[((10-range) - (10-range+4)/5 + boomerang_init) % 8];
			dy = ydir[((10-range) - (10-range+4)/5 + boomerang_init) % 8];
			/* TODO: spinny glyphs for all possible boomerangs? */
			/* by not doing the spinny )()()( nonsense we can easily show a properly silver chakram */
		}

		/* priority order: */
		/* pickaxes in shops special case */
		/* space has monster and is not initxy */
		/* space has sink/wall and is not initxy */
		/* space ahead has wall and no monster */
		/* space ahead has iron bars and no monster */
		/* heavy iron ball specific checks */
		/* out of range */
		/* actually move the projectile */

		if ((is_pick(thrownobj)) &&
			(inside_shop(bhitpos.x, bhitpos.y)) &&
			(shkcatch(thrownobj, bhitpos.x, bhitpos.y))) {
			/* shopkeeper caught it */
			wepgone = TRUE;
			break;
		}

		/* projectile is on a creature */
		if ((range != initrange) &&
			(mdef = creature_at(bhitpos.x, bhitpos.y)))
		{
			result = projectile_attack(magr, mdef, thrownobj, launcher, fired, &dx, &dy, &range, &initrange, destroy, &wepgone);

			if (result)
			{
				break;
			}
		}

		/* projectile is on a sink (it "sinks" down) or is on a non-allowable square */
		if ((range != initrange) &&
			(!ZAP_POS(levl[bhitpos.x][bhitpos.y].typ)) || (IS_SINK(levl[bhitpos.x][bhitpos.y].typ)))
		{
			range = 0;
		}

		/* space ahead has wall and no monster */
		if (!isok(bhitpos.x + dx, bhitpos.y + dy) ||
			(!ZAP_POS(levl[bhitpos.x+dx][bhitpos.y+dy].typ) &&
			!m_at(bhitpos.x+dx, bhitpos.y+dy)))
		{
			range = 0;
		}
		/* space ahead has iron bars and no monster */
		/* 1/5 chance for 'small' objects (see hits_bars), unless if launched from right beside the iron bars */
		if (levl[bhitpos.x + dx][bhitpos.y + dy].typ == IRONBARS &&
			!m_at(bhitpos.x + dx, bhitpos.y + dy) && 
			hits_bars(
				/* object fired   */ &thrownobj,
				/* current coords */ bhitpos.x, bhitpos.y,
				/* force hit?     */ Is_illregrd(&u.uz) || ((bhitpos.x == initx && bhitpos.y == inity) ? 0 : !rn2(5)),
				/* player caused  */ (magr == &youmonst))
			) {
			range = 0;
		}

		/* limit range of iron balls so hero won't make an invalid move */
		if (range > 0 && thrownobj && thrownobj->otyp == HEAVY_IRON_BALL) {
			struct obj *bobj;
			struct trap *t;
			if ((bobj = boulder_at(bhitpos.x, bhitpos.y)) != 0) {
				if (cansee(bhitpos.x, bhitpos.y))
					pline("%s hits %s.",
					The(distant_name(thrownobj, xname)), an(xname(bobj)));
				range = 0;
			}
			else if (thrownobj == uball) {
				if (!test_move(bhitpos.x - dx, bhitpos.y - dy, dx, dy, TEST_MOVE)) {
					/* nb: it didn't hit anything directly */
					if (cansee(bhitpos.x, bhitpos.y)) {
						pline("%s jerks to an abrupt halt.",
							The(distant_name(thrownobj, xname))); /* lame */
					}
					range = 0;
				}
				else if (In_sokoban(&u.uz) && (t = t_at(bhitpos.x, bhitpos.y)) != 0 &&
					(t->ttyp == PIT || t->ttyp == SPIKED_PIT ||
					t->ttyp == HOLE || t->ttyp == TRAPDOOR)) {
					/* hero falls into the trap, so ball stops */
					range = 0;
				}
			}
		}

		/* no projectile -- something destroyed it */
		if (!thrownobj)
		{
			break;
		}

		/* out of range */
		if (range < 1)
		{
			break;
		}
		else {
			/* move the projectile */
			range--;
			bhitpos.x += dx;
			bhitpos.y += dy;
			tmp_at(bhitpos.x, bhitpos.y);
			if (couldsee(bhitpos.x, bhitpos.y))
				delay_output();
		}
	}
	/* end glyph showing */
	tmp_at(DISP_END, 0);

	/* some artifacts magically return to the location they were thrown from */
	if ((Race_if(PM_ANDROID) && !fired && youagr) ||	/* there's no android monster helper? */
		(thrownobj->oartifact == ART_MJOLLNIR && (youagr ? (Role_if(PM_VALKYRIE)) : magr ? (magr->data == &mons[PM_VALKYRIE]) : FALSE)) ||
		(thrownobj->oartifact == ART_AXE_OF_THE_DWARVISH_LORDS && (youagr ? (Race_if(PM_DWARF)) : magr ? (is_dwarf(magr->data)) : FALSE)) ||
		thrownobj->oartifact == ART_SICKLE_MOON ||
		thrownobj->oartifact == ART_ANNULUS ||
		thrownobj->oartifact == ART_KHAKKHARA_OF_THE_MONKEY ||
		thrownobj->oartifact == ART_DART_OF_THE_ASSASSIN ||
		thrownobj->oartifact == ART_WINDRIDER ||
		thrownobj->oartifact == ART_AMHIMITL
		) {
		/* show object's return flight */
		sho_obj_return(thrownobj, initx, inity);
		/* move it */
		bhitpos.x = initx;
		bhitpos.y = inity;
		/* check to see if magr is still there */
		if (magr && x(magr) == bhitpos.x && y(magr) == bhitpos.y)
		{
			/* attempt to catch it */
		}
		/* else just drop it on the floor */
	}

	/* end the projectile */
	end_projectile(magr, mdef, thrownobj, launcher, fired, wepgone);
	return result;
}

/*
 * destroy_projectile()
 *
 * Call this when the projectile should violently cease to exist.
 * Does not end tmp_at.
 */
void
destroy_projectile(magr, thrownobj)
struct monst * magr;			/* Creature responsible for the projectile. Might not exist. */
struct obj * thrownobj;			/* Projectile object. Must be free. Will no longer exist after this function. */
{
	boolean youagr = (magr && (magr == &youmonst));

	switch (thrownobj->otyp)
	{
	case FRAG_GRENADE:
	case GAS_GRENADE:
		grenade_explode(thrownobj, bhitpos.x, bhitpos.y, youagr, 0);
		break;

	case ROCKET:
		if (cansee(bhitpos.x, bhitpos.y))
			pline("%s explodes in a ball of fire!", Doname2(thrownobj));
		else
			You_hear("an explosion");
		explode(bhitpos.x, bhitpos.y, AD_FIRE, WEAPON_CLASS,
			d(3, 8), EXPL_FIERY, youagr);
		obfree(thrownobj, (struct obj *)0);
		break;

	case BLASTER_BOLT:
	case HEAVY_BLASTER_BOLT:
		explode(bhitpos.x, bhitpos.y, AD_PHYS, 0,
			(thrownobj->otyp == HEAVY_BLASTER_BOLT ? (d(3, 10))   : (d(3, 6))),
			(thrownobj->otyp == HEAVY_BLASTER_BOLT ? (EXPL_FIERY) : (EXPL_RED)),
			youagr);
		obfree(thrownobj, (struct obj *)0);
		break;

	default:
		obfree(thrownobj, (struct obj *)0);
	}
	return;
}

/*
 * end_projectile(magr, thrownobj, launcher, fired, destroy)
 * 
 * The projectile may or may not have hit a monster, but it is ending.
 * If the projectile had hit a monster, that may have destroyed the projectile.
 * 
 * [magr] might not exist -- if it doesn't exist, we need to be careful what we call
 * [thrownobj] may have been destroyed already -- if so, we need to end graphics and return early
 * 
 */
void
end_projectile(magr, mdef, thrownobj, launcher, fired, wepgone)
struct monst * magr;			/* Creature responsible for the projectile. Might not exist. */
struct monst * mdef;			/* Creature hit by the projectile. Might not exist. */
struct obj * thrownobj;			/* Projectile object. Must be free. At this point, might not exist. */
struct obj * launcher;			/* Launcher for the projectile. Can be non-existant. Implies "fired" is true. */
boolean fired;					/* Whether or not the projectile was fired (ex arrow from a bow). Fired without a launcher is possible (ex AT_ARRW). */
boolean wepgone;				/* TRUE if projectile is already destroyed */
{
	boolean youagr = (magr && (magr == &youmonst));

	/* need to check that thrownobj hasn't been dealt with already */
	if (wepgone)
	{
		return;
	}

	/* projectiles that never survive being fired; their special effects are handled in destroy_projectile() */
	if (fired && (
		thrownobj->otyp == ROCKET ||
		thrownobj->otyp == BLASTER_BOLT ||
		thrownobj->otyp == HEAVY_BLASTER_BOLT
		))
	{
		destroy_projectile(magr, thrownobj);
		return;
	}

	/* if the player was swallowed before throwing the object, special handling */
	if (youagr && u_was_swallowed) {
		if (u.uswallow) {
			/* you are still swallowed */
			pline("%s into %s %s.",
				Tobjnam(thrownobj, "vanish"), s_suffix(mon_nam(u.ustuck)),
				is_animal(u.ustuck->data) ? "entrails" : "currents");
			/* corpses are digested, other objects just stored */
			if (is_animal(u.ustuck->data) &&
				thrownobj->otyp == CORPSE) {
				delobj(thrownobj);
				return;
			}
			else {
				mpickobj(u.ustuck, thrownobj);
				return;
			}
		}
		else {
			/* the thing you threw freed you (probably by killing the swallower) */
			/* we still want to remove a corpse thrown into the swallower -- hopefully mdef hasn't been cleaned up yet,
			 * otherwise we'll need to save mdef's data or something */
			if (mdef && is_animal(mdef->data) &&
				thrownobj->otyp == CORPSE) {
				delobj(thrownobj);
				return;
			}
			/* else go through the rest of the function */
		}
	}

	/* fragile objects shatter */
	if (!IS_SOFT(levl[bhitpos.x][bhitpos.y].typ) &&
		breaktest(thrownobj)) {

		/* Nudzirath effects */
		if (youagr && is_shatterable(thrownobj) && !thrownobj->oerodeproof && u.specialSealsActive&SEAL_NUDZIRATH){
			int dmg, dsize = spiritDsize(), sx, sy;
			struct monst *msmon;
			sx = bhitpos.x;
			sy = bhitpos.y;
			if (thrownobj->otyp == MIRROR){
				if (u.spiritPColdowns[PWR_MIRROR_SHATTER] < monstermoves && !u.uswallow && uwep && uwep->otyp == MIRROR && !(uwep->oartifact)){
					useup(uwep);
					explode(u.ux, u.uy, AD_PHYS, TOOL_CLASS, d(5, dsize), HI_SILVER, 1);
					explode(sx, sy, AD_PHYS, TOOL_CLASS, d(5, dsize), HI_SILVER, 1);

					while (sx != u.ux && sy != u.uy){
						sx -= u.dx;
						sy -= u.dy;
						if (!isok(sx, sy)) break; //shouldn't need this, but....
						else {
							msmon = m_at(sx, sy);
							/* reveal/unreveal invisible msmonsters before tmp_at() */
							if (msmon && !canspotmon(msmon) && cansee(sx, sy))
								map_invisible(sx, sy);
							else if (!msmon && glyph_is_invisible(levl[sx][sy].glyph)) {
								unmap_object(sx, sy);
								newsym(sx, sy);
							}
							if (msmon) {
								if (resists_magm(msmon)) {	/* match effect on player */
									shieldeff(msmon->mx, msmon->my);
								}
								else {
									dmg = d(5, dsize);
									if (hates_silver(msmon->data)){
										dmg += rnd(20);
										pline("The flying shards of mirror sear %s!", mon_nam(msmon));
									}
									else {
										pline("The flying shards of mirror hit %s.", mon_nam(msmon));
										u_teleport_mon(msmon, TRUE);
									}
									msmon->mhp -= dmg;
									if (msmon->mhp <= 0){
										xkilled(msmon, 1);
									}
								}
							}
						}
					}
					u.spiritPColdowns[PWR_MIRROR_SHATTER] = monstermoves + 25;
				}
				else explode(sx, sy, AD_PHYS, TOOL_CLASS, d(rnd(5), dsize), HI_SILVER, 1);
			}
			else if (thrownobj->obj_material == OBSIDIAN_MT) explode(sx, sy, AD_PHYS, WEAPON_CLASS, d(rnd(5), dsize), EXPL_DARK, 1);
		}

		tmp_at(DISP_FLASH, obj_to_glyph(thrownobj));
		tmp_at(bhitpos.x, bhitpos.y);
		if (cansee(bhitpos.x, bhitpos.y))
			delay_output();
		tmp_at(DISP_END, 0);
		breakmsg(thrownobj, cansee(bhitpos.x, bhitpos.y));
		breakobj(thrownobj, bhitpos.x, bhitpos.y, youagr, TRUE);
		return;
	}

	/* do floor effects (like lava) -- if it returned true the item was destroyed */
	if (flooreffects(thrownobj, bhitpos.x, bhitpos.y, "fall")) {
		return;
	}
	
	/* note that the item is no longer in anything's inventory */
	obj_no_longer_held(thrownobj);

	/* shopkeepers snatch up pickaxes */
	if (mdef && mdef->isshk && is_pick(thrownobj) && !DEADMONSTER(mdef)) {
		if (cansee(bhitpos.x, bhitpos.y))
			pline("%s snatches up %s.",
			Monnam(mdef), the(xname(thrownobj)));
		if (*u.ushops)
			check_shop_obj(thrownobj, bhitpos.x, bhitpos.y, FALSE);
		(void)mpickobj(mdef, thrownobj);	/* may merge and free obj */
		return;
	}

	/* candles are snuffed */
	(void)snuff_candle(thrownobj);

	/* fired grenades are armed */
	if (is_grenade(thrownobj))
		arm_bomb(thrownobj, youagr);

	/* if there wasn't a creature at the projectile's hitspot, it might fall down the stairs */
	if (!mdef && ship_object(thrownobj, bhitpos.x, bhitpos.y, FALSE)) {
		return;
	}

	/* now, place the object on the floor */
	place_object(thrownobj, bhitpos.x, bhitpos.y);

	/* possibly 'donate' it to a shop */
	if (*u.ushops && thrownobj != uball)
		check_shop_obj(thrownobj, bhitpos.x, bhitpos.y, FALSE);

	/* stack it */
	stackobj(thrownobj);

	/* ball-related stuff */
	if (thrownobj == uball)
		drop_ball(bhitpos.x, bhitpos.y);

	/* update screen to see the new location of thrownobj */
	if (cansee(bhitpos.x, bhitpos.y))
		newsym(bhitpos.x, bhitpos.y);

	/* recalculate vision if we threw a lightsource */
	if (obj_sheds_light(thrownobj))
		vision_full_recalc = 1;

	/* damage the contents of a container */
	if (!IS_SOFT(levl[bhitpos.x][bhitpos.y].typ))
		container_impact_dmg(thrownobj);

	return;
}

/* 
 * projectile_attack()
 * 
 * A projectile is going towards mdef
 * 
 * This should end the projectile unless MM_MISS is returned.
 *  
 */
int
projectile_attack(magr, mdef, thrownobj, launcher, fired, pdx, pdy, prange, prange2, destroy, wepgone)
struct monst * magr;			/* Creature responsible for the projectile. Can be non-existant. */
struct monst * mdef;			/* Creature under fire. */
struct obj * thrownobj;			/* Projectile object. Must be free. */
struct obj * launcher;			/* Launcher for the projectile. Can be non-existant. Implies "fired" is true. */
boolean fired;					/* Whether or not the projectile was fired (ex arrow from a bow). Fired without a launcher is possible (ex AT_ARRW). */
int * pdx;						/* pointer to: x; Direction of projectile's movement */
int * pdy;						/* pointer to: y; Direction of projectile's movement */
int * prange;					/* pointer to: Remaining range for projectile */
int * prange2;					/* pointer to: Remaining 2x range for projectile */
boolean destroy;				/* TRUE if projectile should be forced to be destroyed at the end */
boolean * wepgone;				/* pointer to: TRUE if projectile has been destroyed */
{
	int dx		= *pdx;
	int dy		= *pdy;
	int range	= *prange;
	int range2	= *prange2;
	boolean youagr = (magr == &youmonst);
	boolean youdef = (mdef == &youmonst);
	struct permonst * pa = magr ? (youagr ? youracedata : magr->data) : (struct permonst *)0;
	struct permonst * pd = youdef ? youracedata : mdef->data;
	boolean misfired = (!fired && is_ammo(thrownobj) && thrownobj->oclass != GEM_CLASS);
	int result;
	int accuracy;
	int dieroll;
	int vis;

	/* Set up the visibility of action */
	if (youagr || youdef || ((!magr || cansee(x(magr), y(magr))) && cansee(x(mdef), y(mdef))))
	{
		if (youagr || (magr && cansee(x(magr), y(magr)) && canseemon(magr)))
			vis |= VIS_MAGR;
		if (youdef || (cansee(x(mdef), y(mdef)) && canseemon(mdef)))
			vis |= VIS_MDEF;
		if (youagr || youdef || (magr && canspotmon(magr)) || canspotmon(mdef))
			vis |= VIS_NONE;
	}

	/* train player's Shien skill, if player is defending */
	if (youdef && uwep && is_lightsaber(uwep) && litsaber(uwep) && P_SKILL(weapon_type(uwep)) >= P_BASIC){
		if (P_SKILL(FFORM_SHII_CHO) >= P_BASIC){
			if (u.fightingForm == FFORM_SHII_CHO ||
				(u.fightingForm == FFORM_SHIEN && (!uarm || is_light_armor(uarm)))
				) use_skill(FFORM_SHIEN, 1);
		}
	}

	/* sometimes projectiles have an early return */
	/* you can catch your own boomerangs */
	if (youdef && youagr && is_boomerang(thrownobj)) {
		if (!(Fumbling || (!thrownobj->oartifact && rn2(18) >= ACURR(A_DEX)))) {
			/* we catch it */
			You("skillfully catch the %s.", xname(thrownobj));
			*wepgone = TRUE;
			exercise(A_DEX, TRUE);
			thrownobj = addinv(thrownobj);
			(void)encumber_msg();
			if (old_wep_mask && !(thrownobj->owornmask & old_wep_mask)) {
				setworn(thrownobj, old_wep_mask);
				u.twoweap = u_was_twoweap;
			}
			return MM_HIT;
		}
		/* else do the rest of the function, ouch! */
	}
	/* Sansara Mirror */
	if (rn2(2) && (
		(youdef && (
		(uwep && uwep->oartifact == ART_SANSARA_MIRROR) || (uswapwep && uswapwep->oartifact == ART_SANSARA_MIRROR)
		))
		||
		(!youdef && (
		(MON_WEP(mdef) && MON_WEP(mdef)->oartifact == ART_SANSARA_MIRROR) || (MON_SWEP(mdef) && MON_SWEP(mdef)->oartifact == ART_SANSARA_MIRROR)
		))
		))
	{
		/* message */
		if (vis) {
			pline("Sansara twists and reflects.");
		}
		/* return to sender */
		*pdx *= -1;
		*pdy *= -1;
		*prange = *prange2;
		return MM_MISS;
	}
	/* blaster bolts and laser beams are reflected by regular reflection */
	else if ((thrownobj->otyp == LASER_BEAM || thrownobj->otyp == BLASTER_BOLT || thrownobj->otyp == HEAVY_BLASTER_BOLT)
		&& (youdef ? Reflecting : mon_reflects(mdef, (char *)0)))
	{
		boolean shienuse = FALSE;
		/* if the player is using Shien lightsaber form, they can direct the reflection */
		if (youdef && uwep && is_lightsaber(uwep) && litsaber(uwep)
			&& u.fightingForm == FFORM_SHIEN && (!uarm || is_light_armor(uarm)
			&& rn2(3) < max((min(P_SKILL(u.fightingForm), P_SKILL(weapon_type(uwep)))) - 1, 1))) {
			You("reflect the %s with your lightsaber!", doname(thrownobj));
			if (getdir("Redirect it which way?"))
				shienuse = TRUE;
		}
		if (youdef && shienuse && (u.dx || u.dy)) {
			*pdx = u.dx;
			*pdy = u.dy;
			*prange = *prange2;
		}
		else if (youdef || mdef->mfaction != FRACTURED){
			*pdx *= -1;
			*pdy *= -1;
		}
		else {
			int i = rn2(8);
			*pdx = xdir[i];
			*pdy = ydir[i];
		}
		return MM_MISS;
	}
	/* the player has a chance to burn some projectiles (not blaster bolts or laser beams) out of the air with a lightsaber */
	else if (!(thrownobj->otyp == LASER_BEAM || thrownobj->otyp == BLASTER_BOLT || thrownobj->otyp == HEAVY_BLASTER_BOLT)
		&& youdef && uwep && is_lightsaber(uwep) && litsaber(uwep)
		&& ((u.fightingForm == FFORM_SHIEN && (!uarm || is_light_armor(uarm))) ||
		(u.fightingForm == FFORM_SORESU && (!uarm || is_light_armor(uarm) || is_medium_armor(uarm)))
		&& 
		rn2(3) < max((min(P_SKILL(u.fightingForm), P_SKILL(weapon_type(uwep))))-1,1))
		)
	{
		You("burn %s out of the %s!", doname(thrownobj), (Underwater || Is_waterlevel(&u.uz)) ? "water" : "air");
		obfree(thrownobj, (struct obj*) 0);
		*wepgone = TRUE;
		return MM_HIT;
	}

	/* unicorns */
	if (thrownobj->oclass == GEM_CLASS && is_unicorn(pd)) {
		if (youdef)
		{
			if (thrownobj->otyp > LAST_GEM) {
				You("catch the %s.", xname(thrownobj));
				You("are not interested in %s junk.",
					(magr ? s_suffix(mon_nam(magr)) : "that"));
				makeknown(thrownobj->otyp);
				dropy(thrownobj);
			}
			else {
				You("accept %s gift in the spirit in which it was intended.",
					(magr ? s_suffix(mon_nam(magr)) : "the mysterious"));
				(void)hold_another_object(thrownobj,
					"You catch, but drop, %s.", xname(thrownobj),
					"You catch:");
			}
			return MM_HIT;
		}
		else if(youagr)
		{
			if (vis && mdef->mtame) {
				pline("%s catches and drops %s.", Monnam(mdef), the(xname(thrownobj)));
			}
			else {
				if (thrownobj->oartifact && vis){ //All gem artifacts need to force a "the" here.
					pline("%s catches the %s.", Monnam(mdef), xname(thrownobj));
				}
				else if (vis) {
					pline("%s catches %s.", Monnam(mdef), the(xname(thrownobj)));
				}
				if (gem_accept(mdef, thrownobj)) {
					/* caught gem; unlink thrownobj */
					thrownobj = (struct obj *)0;
					return MM_HIT;
				}
					
			}
			/* not accepted -- drop it there */
			*prange = *prange2 = 0;
			return MM_MISS;
		}
		/* mvm unicorn gem throw-catch is not a thing. */
	}

	/* quest leader */
	if (youagr && quest_arti_hits_leader(thrownobj, mdef)) {
		/* not wakeup(), which angers non-tame monsters */
		mdef->msleeping = 0;
		mdef->mstrategy &= ~STRAT_WAITMASK;
		if (mdef->mcanmove) {
			quest_art_swap(thrownobj, mdef, wepgone);
			return MM_HIT;
		}
		else {
			/* drop it there */
			*prange = *prange2 = 0;
			return MM_MISS;
		}
	}

	/* you throwing to a pet */
	if (youagr && mdef->mtame) {
		if (mdef->mcanmove &&
			(!is_animal(mdef->data)) &&
			(!mindless_mon(mdef) || (mdef->data == &mons[PM_CROW_WINGED_HALF_DRAGON] && thrownobj->oartifact == ART_YORSHKA_S_SPEAR)) &&
			!fired
			) {
			if (vis) {
				pline("%s catches %s.", Monnam(mdef), the(xname(thrownobj)));
			}
			obj_extract_self(thrownobj);
			(void)mpickobj(mdef, thrownobj);
			if ((attacktype(mdef->data, AT_WEAP) ||
				attacktype(mdef->data, AT_DEVA) ||
				attacktype(mdef->data, AT_XWEP)
				) &&
				mdef->weapon_check == NEED_WEAPON) {
				mdef->weapon_check = NEED_HTH_WEAPON;
				(void)mon_wield_item(mdef);
			}
			m_dowear(mdef, FALSE);
			newsym(x(mdef), y(mdef));
			return MM_HIT;
		}
	}

	/* feeding domestic animals */
	if (befriend_with_obj(mdef->data, thrownobj) ||
		(mdef->mtame && dogfood(mdef, thrownobj) <= 2)) {	/* 2 <=> ACCFOOD */
		if (tamedog(mdef, thrownobj))
		{
			return MM_HIT;           	/* obj is gone */
		}
		else {
			/* not tmiss(), which angers non-tame monsters */
			miss(xname(thrownobj), mdef);
			mdef->msleeping = 0;
			mdef->mstrategy &= ~STRAT_WAITMASK;
			return MM_MISS;
		}
	}

	/* Determine if the projectile hits */
	dieroll = rnd(20);
	struct attack dummy = { AT_WEAP, AD_PHYS, 0, 0 };
	accuracy = tohitval(magr, mdef, &dummy, thrownobj, launcher, (misfired ? 2 : 1), 0);

	if (accuracy > dieroll)
	{
		/* hit */
		/* (player-only) exercise dex */
		if (youagr)
			exercise(A_DEX, TRUE);
		/* call hmon to make the projectile hit */
		/* hmon will do hitmsg */
		result = hmon2point0(magr, mdef, &dummy, thrownobj, launcher, (misfired ? 2 : 1), 1, TRUE, dieroll, FALSE, vis, wepgone);

		/* wake up defender */
		wakeup2(mdef, youagr);

		/* anger shopkeep */
		if (youagr && mdef && mdef->isshk &&
			(!inside_shop(u.ux, u.uy) ||
			!index(in_rooms(mdef->mx, mdef->my, SHOPBASE), *u.ushops)))
			hot_pursuit(mdef);

		/* deal with projectile */
		if (*wepgone) {
			/*hmon destroyed it, we're already done*/;
		}
		else 
		/* projectiles other than magic stones
		 * sometimes disappear when thrown
		 * WAC - Spoon always disappears after doing damage
		 */
		if ((objects[thrownobj->otyp].oc_skill < P_NONE &&
			objects[thrownobj->otyp].oc_skill > -P_BOOMERANG &&
			thrownobj->oclass != GEM_CLASS &&
			!objects[thrownobj->otyp].oc_magic) ||
			(thrownobj->oartifact == ART_HOUCHOU)
			) {
			/* we were breaking 2/3 of everything unconditionally.
			 * we still don't want anything to survive unconditionally,
			 * but we need ammo to stay around longer on average.
			 */
			boolean broken = FALSE;
			if (thrownobj->oartifact && thrownobj->oartifact != ART_HOUCHOU){
				broken = FALSE;
			}
			else if (destroy ||
				(launcher && fired && (launcher->oartifact == ART_HELLFIRE || launcher->oartifact == ART_BOW_OF_SKADI)) ||
				(thrownobj->oartifact == ART_HOUCHOU) ||
				(fired && thrownobj->otyp == BULLET) || 
				(fired && thrownobj->otyp == SILVER_BULLET) || 
				(fired && thrownobj->otyp == SHOTGUN_SHELL) || 
				(fired && thrownobj->otyp == ROCKET) || 
				(fired && thrownobj->otyp == BLASTER_BOLT) ||
				(fired && thrownobj->otyp == HEAVY_BLASTER_BOLT) ||
				(fired && thrownobj->otyp == FRAG_GRENADE) || 
				(fired && thrownobj->otyp == GAS_GRENADE)) {
				broken = TRUE;
			}
			else {
				int break_chance = 3 + greatest_erosion(thrownobj) - thrownobj->spe;
				if (break_chance > 1)
					broken = rn2(break_chance);
				else
					broken = !rn2(4);
				if (thrownobj->blessed && rnl(100) < 25)
					broken = FALSE;
			}
			/* some projectiles should instead be transfered to the defender's inventory... if they lived */
			if (*hp(mdef)>0) {
				if (youdef && thrownobj->otyp == LOADSTONE && !rn2(3))
				{
					broken = FALSE;
					*wepgone = TRUE;
					pickup_object(thrownobj, 1, TRUE);
					result |= MM_HIT;
				}
			}

			if (broken) {
				if (*u.ushops) {
					check_shop_obj(thrownobj, bhitpos.x, bhitpos.y, TRUE);
				}
				//#ifdef FIREARMS
				destroy_projectile(magr, thrownobj);
				*wepgone = TRUE;

				/* check if explosions changed result */
				if (*hp(mdef) <= 0)
					result |= MM_DEF_DIED;
			}
		}
		else
		{
			/* possibly damage the projectile -- at this point it is surviving */
			passive_obj2(magr, mdef, thrownobj, (struct attack *)0, (struct attack *)0);
		}

		return result;
	}
	else
	{
		/* miss */
		/* print missmsg */
		if (vis) {
			/* If the target can't be seen or doesn't look like a valid target,
			 * avoid "the arrow misses it," or worse, "the arrows misses the mimic."
			 * An attentive player will still notice that this is different from
			 * an arrow just landing short of any target (no message in that case),
			 * so will realize that there is a valid target here anyway. 
			 */
			if (!canseemon(mdef) || (mdef->m_ap_type && mdef->m_ap_type != M_AP_MONSTER)) {
				pline("%s %s.", (The(mshot_xname(thrownobj))), (vtense(mshot_xname(thrownobj), "miss")));
			}
			else {
				pline("%s %s %s.",
					(The(mshot_xname(thrownobj))),
					(vtense(mshot_xname(thrownobj), "miss")),
					(((cansee(bhitpos.x, bhitpos.y) || canspotmon(mdef)) && flags.verbose) ? mon_nam(mdef) : "it")
					);
			}
		}
		/* possibly wake defender */
		if (!rn2(3))
		{
			wakeup2(mdef, youagr);
		}
		/* anger shopkeep */
		if (youagr && mdef && mdef->isshk &&
			(!inside_shop(u.ux, u.uy) ||
			!index(in_rooms(mdef->mx, mdef->my, SHOPBASE), *u.ushops)))
			hot_pursuit(mdef);
			
		/* cause projectile to fall onto the floor */
		*prange = *prange2 = 0;
		return MM_MISS;
	}

	return MM_MISS;
}

void
quest_art_swap(obj, mon, swapped)
struct obj * obj;
struct monst * mon;
boolean * swapped;
{
	/* don't make game unwinnable if naive player throws artifact
	at leader.... */
	pline("%s catches %s.", Monnam(mon), the(xname(obj)));
	if (mon->mpeaceful) {
		if (Race_if(PM_ELF) && mon->data == &mons[PM_CELEBORN]){
			boolean next2u = monnear(mon, u.ux, u.uy);
			if (obj->oartifact == ART_PALANTIR_OF_WESTERNESSE &&
				yn("If you prefer, we can use the Palantir to secure the city, and you can use my bow in your travels.") == 'y'
				){
				*swapped = TRUE;
				obfree(obj, (struct obj *)0);
				obj = mksobj(ELVEN_BOW, TRUE, FALSE);
				obj = oname(obj, artiname(ART_BELTHRONDING));
				obj->oerodeproof = TRUE;
				obj->blessed = TRUE;
				obj->cursed = FALSE;
				obj->spe = rn2(8);
				pline("%s %s %s to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return(obj, u.ux, u.uy);
				obj = addinv(obj);	/* into your inventory */
				(void)encumber_msg();
			}
			else {
				pline("%s %s %s back to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return(obj, u.ux, u.uy);
				obj = addinv(obj);	/* back into your inventory */
				(void)encumber_msg();
			}
		}
		else if (Race_if(PM_DROW)){
			boolean next2u = monnear(mon, u.ux, u.uy);
			if (quest_status.got_thanks) finish_quest(obj);
			if (obj->oartifact == ART_SILVER_STARLIGHT && quest_status.got_thanks && mon->data == &mons[PM_ECLAVDRA] &&
				yn("Do you wish to take the Wrathful Spider, instead of this?") == 'y'
				){
				*swapped = TRUE;
				obfree(obj, (struct obj *)0);
				obj = mksobj(DROVEN_CROSSBOW, TRUE, FALSE);
				obj = oname(obj, artiname(ART_WRATHFUL_SPIDER));
				obj->oerodeproof = TRUE;
				obj->blessed = TRUE;
				obj->cursed = FALSE;
				obj->spe = rn2(8);
				finish_quest(obj);
				pline("%s %s %s to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return(obj, u.ux, u.uy);
				obj = addinv(obj);	/* into your inventory */
				(void)encumber_msg();
			}
			else if (obj->oartifact == ART_TENTACLE_ROD && quest_status.got_thanks && mon->data == &mons[PM_SEYLL_AUZKOVYN] &&
				yn("Do you wish to take the Crescent Blade, instead of this?") == 'y'
				){
				*swapped = TRUE;
				obfree(obj, (struct obj *)0);
				obj = mksobj(SABER, TRUE, FALSE);
				obj = oname(obj, artiname(ART_CRESCENT_BLADE));
				obj->oerodeproof = TRUE;
				obj->blessed = TRUE;
				obj->cursed = FALSE;
				obj->spe = rnd(7);
				finish_quest(obj);
				pline("%s %s %s to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return(obj, u.ux, u.uy);
				obj = addinv(obj);	/* into your inventory */
				(void)encumber_msg();
			}
			else if (obj->oartifact == ART_DARKWEAVER_S_CLOAK && quest_status.got_thanks && mon->data == &mons[PM_ECLAVDRA] &&
				yn("Do you wish to take Spidersilk, instead of this?") == 'y'
				){
				*swapped = TRUE;
				obfree(obj, (struct obj *)0);
				obj = mksobj(DROVEN_CHAIN_MAIL, TRUE, FALSE);
				obj = oname(obj, artiname(ART_SPIDERSILK));
				obj->oerodeproof = TRUE;
				obj->blessed = TRUE;
				obj->cursed = FALSE;
				obj->spe = rn2(8);
				finish_quest(obj);
				pline("%s %s %s to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return(obj, u.ux, u.uy);
				obj = addinv(obj);	/* into your inventory */
				(void)encumber_msg();
			}
			else if (obj->oartifact == ART_TENTACLE_ROD && quest_status.got_thanks && mon->data == &mons[PM_DARUTH_XAXOX] &&
				yn("Do you wish to take the Webweaver's Crook, instead of this?") == 'y'
				){
				*swapped = TRUE;
				obfree(obj, (struct obj *)0);
				obj = mksobj(FAUCHARD, TRUE, FALSE);
				obj = oname(obj, artiname(ART_WEBWEAVER_S_CROOK));
				obj->oerodeproof = TRUE;
				obj->blessed = TRUE;
				obj->cursed = FALSE;
				obj->spe = rnd(3) + rnd(4);
				finish_quest(obj);
				pline("%s %s %s to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return(obj, u.ux, u.uy);
				obj = addinv(obj);	/* into your inventory */
				(void)encumber_msg();
			}
			else {
				if (!quest_status.got_thanks) finish_quest(obj);	/* acknowledge quest completion */
				pline("%s %s %s back to you.", Monnam(mon),
					(next2u ? "hands" : "tosses"), the(xname(obj)));
				if (!next2u) sho_obj_return(obj, u.ux, u.uy);
				obj = addinv(obj);	/* back into your inventory */
				(void)encumber_msg();
			}
		}
		else {
			boolean next2u = monnear(mon, u.ux, u.uy);

			finish_quest(obj);	/* acknowledge quest completion */
			pline("%s %s %s back to you.", Monnam(mon),
				(next2u ? "hands" : "tosses"), the(xname(obj)));
			if (!next2u) sho_obj_return(obj, u.ux, u.uy);
			obj = addinv(obj);	/* back into your inventory */
			(void)encumber_msg();
		}
	}
	else {
		/* angry leader caught it and isn't returning it */
		(void)mpickobj(mon, obj);
	}
	return;
}

/* show the currently thrown object returning to (destx, desty) in a straight line (not for boomerangs) */
void
sho_obj_return(thrownobj, destx, desty)
struct obj * thrownobj;
int destx;
int desty;
{
	int dx = destx - bhitpos.x;
	int dy = desty - bhitpos.y;

	/* check that it is okay to reduce dx and dy to small increments */
	if (dx && dy && (abs(dx) != abs(dy))) {
		impossible("projectile returning not in straight line?");
		return;
	}
	/* change dx and dy to be small increments */
	dx = sgn(dx);
	dy = sgn(dy);

	/* might already be our location (bounced off a wall) */
	if (bhitpos.x != destx || bhitpos.y != desty) {
		int x = bhitpos.x;
		int y = bhitpos.y;

		/* show its return */
		tmp_at(DISP_FLASH, obj_to_glyph(thrownobj));

		while (x != destx || y != desty) {
			tmp_at(x, y);
			if (cansee(x, y)) delay_output();
			x += dx;
			y += dy;
		}
		tmp_at(DISP_END, 0);
	}
	return;
}