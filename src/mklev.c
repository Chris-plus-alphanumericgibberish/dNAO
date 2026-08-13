/*	SCCS Id: @(#)mklev.c	3.4	2001/11/29	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
/* #define DEBUG */	/* uncomment to enable code debugging */
extern const int monstr[];

#ifdef DEBUG
# ifdef WIZARD
#define debugpline	if (wizard) pline
# else
#define debugpline	pline
# endif
#endif

/* for UNIX, Rand #def'd to (long)lrand48() or (long)random() */
/* croom->lx etc are schar (width <= int), so % arith ensures that */
/* conversion of result to int is reasonable */

/* a cell's classification for fallback-corridor purposes (see
   hallway_fallback_attempt()) */
#define FB_CELL_OPEN  0 /* STONE or SCORR -- open rock, walkable by our own corridor */
#define FB_CELL_CORR  1 /* a primary hallway's own floor */
#define FB_CELL_OTHER 2 /* anything else (room floor, wall, door, off-map...) */

/* per-endpoint escalation-level outcomes */
#define FB_STOP     0
#define FB_ESCALATE 1
#define FB_KEEP     2
#define FB_CONTINUE 3

/* fb_try_direct_hit() outcomes */
#define FB_HIT_NONE   0 /* not a hit at all -- fall through to normal escalation */
#define FB_HIT_DONE   1 /* connected successfully, terrain committed */
#define FB_HIT_CORNER 2 /* touching otherroom at a corner -- abort the whole candidate (deferred) */

/* one endpoint's fallback-corridor placement state as it escalates through
   levels 1-3 (see hallway_fallback_attempt()) */
struct fb_endpoint {
	int room;
	int level;              /* 1, 2, or 3 */
	xchar doorx, doory;      /* current door (mutates as level increases) */
	xchar goalx, goaly;      /* current line endpoint (mutates) */
	xchar odoorx, odoory;    /* the ORIGINAL level-1 door -- fixed once set, needed by level 2's look-back */
	schar odx, ody;          /* room's own outward wall normal; also level-3's walk direction */
	schar tdx, tdy;          /* perpendicular "away from the hallway" direction; set at level 2 */
	xchar wallx, wally;      /* W: the room-wall connector tile; set at level 2 */
	boolean need_door;       /* does W need a new dosdoor() at commit, or does one already exist? */
};

STATIC_DCL void NDECL(makevtele);
STATIC_DCL void NDECL(clear_level_structures);
STATIC_DCL void NDECL(makelevel);
STATIC_DCL boolean FDECL(bydoor,(XCHAR_P,XCHAR_P));
STATIC_DCL struct mkroom *FDECL(find_branch_room, (coord *));
STATIC_DCL struct mkroom *FDECL(pos_to_room, (XCHAR_P, XCHAR_P));
STATIC_DCL boolean FDECL(place_niche,(struct mkroom *,int*,int*,int*));
STATIC_DCL void FDECL(makeniche,(int));
STATIC_DCL void NDECL(make_niches);

STATIC_PTR int FDECL( CFDECLSPEC do_comp,(const genericptr,const genericptr));

STATIC_DCL int FDECL(smeq_root,(int));
STATIC_DCL void FDECL(smeq_union,(int,int));
STATIC_DCL void FDECL(join,(int,int,BOOLEAN_P));
STATIC_DCL void FDECL(do_room_or_subroom, (struct mkroom *,int,int,int,int,
				       BOOLEAN_P,SCHAR_P,BOOLEAN_P,BOOLEAN_P));
STATIC_DCL void NDECL(makerooms);
STATIC_DCL void FDECL(finddpos,(coord *,XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P));
STATIC_DCL boolean FDECL(wall_slot_range,(struct mkroom *,int,xchar *,xchar *,xchar *));
STATIC_DCL boolean FDECL(door_slot_range,(struct mkroom *,int,xchar *,xchar *,xchar *));
STATIC_DCL boolean FDECL(hallway_swath_ok,(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,BOOLEAN_P,BOOLEAN_P));
STATIC_DCL boolean FDECL(hallway_cross_overlaps,(XCHAR_P,XCHAR_P,BOOLEAN_P));
STATIC_DCL void FDECL(hallway_cross_carve,(XCHAR_P,XCHAR_P,BOOLEAN_P,SCHAR_P));
STATIC_DCL void FDECL(hallway_connector_carve,(XCHAR_P,XCHAR_P,BOOLEAN_P,SCHAR_P,XCHAR_P,XCHAR_P,SCHAR_P));
STATIC_DCL void FDECL(hallway_swath_carve,(XCHAR_P,XCHAR_P,XCHAR_P,XCHAR_P,BOOLEAN_P,BOOLEAN_P,BOOLEAN_P,SCHAR_P));
STATIC_DCL void FDECL(hallway_endpoint,(int,XCHAR_P,XCHAR_P,xchar *,xchar *,schar *,schar *));
STATIC_DCL boolean FDECL(find_hallway,(int,int,xchar *,xchar *));
STATIC_DCL boolean FDECL(dohalldoor,(XCHAR_P,XCHAR_P,struct mkroom *));
STATIC_DCL void FDECL(carve_hallway,(int,int,XCHAR_P,XCHAR_P));
STATIC_DCL void NDECL(wallify_mithardir_corridors);
STATIC_DCL void NDECL(wallify_hallway_corridors);
STATIC_DCL int FDECL(hallway_legs,(int,int,int,int,xchar *,xchar *,xchar *,boolean *));
STATIC_DCL boolean FDECL(hallway_legs_overlap,(int,int,int,int,int,int,int,int));
STATIC_DCL boolean FDECL(hallway_wall_square_stat,(int,int,int,int,boolean *));
STATIC_DCL void FDECL(hallway_paint_floor,(int,xchar *,xchar *,xchar *,boolean *,int));
STATIC_DCL boolean NDECL(all_rooms_connected);
STATIC_DCL int FDECL(room_gap,(struct mkroom *,struct mkroom *,int *,int *));
STATIC_DCL int FDECL(room_wall_pairs,(struct mkroom *,struct mkroom *,int *,int *,int *));
STATIC_DCL int FDECL(hallway_cell_class,(int,int));
STATIC_DCL boolean FDECL(hallway_corr_adjacent,(int,int));
STATIC_DCL boolean FDECL(hallway_valid_door_wall,(int,int));
STATIC_DCL boolean FDECL(line_path_clear,(int,int,int,int,xchar *,xchar *,int *));
STATIC_DCL int FDECL(fb_level1,(struct fb_endpoint *));
STATIC_DCL int FDECL(fb_level2,(struct fb_endpoint *));
STATIC_DCL int FDECL(fb_level3_step,(struct fb_endpoint *,struct fb_endpoint *));
STATIC_DCL void FDECL(fb_commit_endpoint,(struct fb_endpoint *));
STATIC_DCL int FDECL(fb_try_direct_hit,(struct fb_endpoint *,int));
STATIC_DCL boolean FDECL(hallway_fallback_attempt,(int,int,int,int,int,int));
STATIC_DCL boolean FDECL(hallway_fallback,(int));
STATIC_DCL void FDECL(greenway,(int,xchar *,xchar *,xchar *,boolean *,xchar,xchar));
STATIC_DCL void FDECL(tree_lined,(int,int,int,boolean));
STATIC_DCL void FDECL(statue_lined,(int,int,int,boolean));
STATIC_DCL void NDECL(makehallways);
STATIC_DCL void FDECL(mkinvpos, (XCHAR_P,XCHAR_P,int));
STATIC_DCL void FDECL(mk_knox_portal, (XCHAR_P,XCHAR_P));

#define create_vault()	create_room(-1, -1, 2, 2, -1, -1, VAULT, TRUE)
#define init_vault()	vault_x = -1
#define do_vault()	(vault_x != -1)
static xchar		vault_x, vault_y;
boolean goldseen;
boolean wantanmivault, wantasepulcher, wantfingerprint;
static boolean made_branch;	/* used only during level creation */

/* Args must be (const genericptr) so that qsort will always be happy. */

STATIC_PTR int CFDECLSPEC
do_comp(vx,vy)
const genericptr vx;
const genericptr vy;
{
#ifdef LINT
/* lint complains about possible pointer alignment problems, but we know
   that vx and vy are always properly aligned. Hence, the following
   bogus definition:
*/
	return (vx == vy) ? 0 : -1;
#else
	register const struct mkroom *x, *y;

	x = (const struct mkroom *)vx;
	y = (const struct mkroom *)vy;
	if(x->lx < y->lx) return(-1);
	return(x->lx > y->lx);
#endif /* LINT */
}

STATIC_OVL void
finddpos(cc, xl,yl,xh,yh)
coord *cc;
xchar xl,yl,xh,yh;
{
	register xchar x, y;

	x = (xl == xh) ? xl : (xl + rn2(xh-xl+1));
	y = (yl == yh) ? yl : (yl + rn2(yh-yl+1));
	if(okdoor(x, y))
		goto gotit;

	for(x = xl; x <= xh; x++) for(y = yl; y <= yh; y++)
		if(okdoor(x, y))
			goto gotit;

	for(x = xl; x <= xh; x++) for(y = yl; y <= yh; y++)
		if(IS_DOOR(levl[x][y].typ) || levl[x][y].typ == SDOOR)
			goto gotit;
	/* cannot find something reasonable -- strange */
	x = xl;
	y = yh;
gotit:
	cc->x = x;
	cc->y = y;
	return;
}

void
sort_rooms()
{
#if defined(SYSV) || defined(DGUX)
	qsort((genericptr_t) rooms, (unsigned)nroom, sizeof(struct mkroom), do_comp);
#else
	qsort((genericptr_t) rooms, nroom, sizeof(struct mkroom), do_comp);
#endif
}

STATIC_OVL void
do_room_or_subroom(croom, lowx, lowy, hix, hiy, lit, rtype, special, is_room)
    register struct mkroom *croom;
    int lowx, lowy;
    register int hix, hiy;
    boolean lit;
    schar rtype;
    boolean special;
    boolean is_room;
{
	register int x, y;
	struct rm *lev;

	/* locations might bump level edges in wall-less rooms */
	/* add/subtract 1 to allow for edge locations */
	if(!lowx) lowx++;
	if(!lowy) lowy++;
	if(hix >= COLNO-1) hix = COLNO-2;
	if(hiy >= ROWNO-1) hiy = ROWNO-2;

	if(lit) {
		for(x = lowx-1; x <= hix+1; x++) {
			lev = &levl[x][max(lowy-1,0)];
			for(y = lowy-1; y <= hiy+1; y++)
				lev++->lit = 1;
		}
		croom->rlit = 1;
	} else
		croom->rlit = 0;

	croom->lx = lowx;
	croom->hx = hix;
	croom->ly = lowy;
	croom->hy = hiy;
	croom->rtype = rtype;
	croom->doorct = 0;
	/* if we're not making a vault, doorindex will still be 0
	 * if we are, we'll have problems adding niches to the previous room
	 * unless fdoor is at least doorindex
	 */
	croom->fdoor = doorindex;
	croom->irregular = FALSE;
	croom->solidwall = 0;
	croom->nsubrooms = 0;
	croom->sbrooms[0] = (struct mkroom *) 0;
	if (!special) {
	    for(x = lowx-1; x <= hix+1; x++)
		for(y = lowy-1; y <= hiy+1; y += (hiy-lowy+2)) {
		    levl[x][y].typ = HWALL;
		    levl[x][y].horizontal = 1;	/* For open/secret doors. */
		}
	    for(x = lowx-1; x <= hix+1; x += (hix-lowx+2))
		for(y = lowy; y <= hiy; y++) {
		    levl[x][y].typ = VWALL;
		    levl[x][y].horizontal = 0;	/* For open/secret doors. */
		}
	    for(x = lowx; x <= hix; x++) {
		lev = &levl[x][lowy];
		for(y = lowy; y <= hiy; y++)
		    lev++->typ = ROOM;
	    }
	    if (is_room) {
		levl[lowx-1][lowy-1].typ = TLCORNER;
		levl[hix+1][lowy-1].typ = TRCORNER;
		levl[lowx-1][hiy+1].typ = BLCORNER;
		levl[hix+1][hiy+1].typ = BRCORNER;
	    } else {	/* a subroom */
		wallification(lowx-1, lowy-1, hix+1, hiy+1);
	    }
	}
}


void
add_room(lowx, lowy, hix, hiy, lit, rtype, special)
register int lowx, lowy, hix, hiy;
boolean lit;
schar rtype;
boolean special;
{
	register struct mkroom *croom;

	croom = &rooms[nroom];
	do_room_or_subroom(croom, lowx, lowy, hix, hiy, lit,
					    rtype, special, (boolean) TRUE);
	croom++;
	croom->hx = -1;
	nroom++;
}

void
add_subroom(proom, lowx, lowy, hix, hiy, lit, rtype, special)
struct mkroom *proom;
register int lowx, lowy, hix, hiy;
boolean lit;
schar rtype;
boolean special;
{
	register struct mkroom *croom;

	croom = &subrooms[nsubroom];
	do_room_or_subroom(croom, lowx, lowy, hix, hiy, lit,
					    rtype, special, (boolean) FALSE);
	proom->sbrooms[proom->nsubrooms++] = croom;
	croom++;
	croom->hx = -1;
	nsubroom++;
}

STATIC_OVL void
makerooms()
{
	boolean tried_vault = FALSE;

	/* make rooms until satisfied */
	/* rnd_rect() will returns 0 if no more rects are available... */
	wantanmivault = !rn2(8);
	wantasepulcher = (depth(&u.uz) > 12 && !rn2(8));
	wantfingerprint = (depth(&u.uz) > 21 && !rn2(8) && !art_already_exists(ART_FINGERPRINT_SHIELD));
	int u_depth = depth(&u.uz);
	boolean knox_range = (u.uz.dnum != oracle_level.dnum		// not in main dungeon
		|| (u_depth = depth(&u.uz)) < 10	// not beneath 10
		|| u_depth > depth(&challenge_level)// not below medusa
	) && !u.uevent.knoxmade;

	if(In_mithardir_terminus(&u.uz)){
		create_room(-1, -1, 7, 7, -1, -1, OROOM, 0);
		mkroom(SLABROOM);
	}
		// create_room(ROWNO/2-3, COLNO/2-3, 7, 7, 1, 1, ISLAND, 0);
	while(nroom < MAXNROFROOMS && rnd_rect() && 
			(In_mithardir_quest(&u.uz) ? (nroom<9 || rn2(MAXNROFROOMS-nroom)) : (nroom<6 || rn2(12-nroom)))
	){
	// while(nroom < MAXNROFROOMS && rnd_rect() && (nroom<6 || )) {
		if(In_mithardir_quest(&u.uz)){
			if (!create_room(-1, -1, 2+rnd(4), 2+rnd(4), -1, -1, OROOM, -1))
				continue;
		}
		else if(nroom >= (MAXNROFROOMS/6) && rn2(3) && !tried_vault && (knox_range || (!wantanmivault && !wantasepulcher && !wantfingerprint))) {
			tried_vault = TRUE;
			if (create_vault()) {
				vault_x = rooms[nroom].lx;
				vault_y = rooms[nroom].ly;
				rooms[nroom].hx = -1;
				wantanmivault = FALSE;
				wantasepulcher = FALSE;
				wantfingerprint = FALSE;
			}
		} else {
		    if (!create_room(-1, -1, -1, -1, -1, -1, OROOM, -1))
			return;
		}
	}
	return;
}

/* follow smeq[]'s merge chain to its current root.  smeq[] is only ever
   updated by copying one room's current tag into another room's slot (see
   smeq_union()) -- the room whose slot gets overwritten is never revisited
   if something else merges into what it pointed to, so a non-root room's
   own value can go stale.  Only the root (smeq[r]==r) is always current;
   smeq_union() always attaches the larger-index root under the smaller,
   so this chain is a valid forest with strictly decreasing values and is
   guaranteed to terminate. */
STATIC_OVL int
smeq_root(int r)
{
	while (smeq[r] != r)
		r = smeq[r];
	return r;
}

/* merge rooms a and b's connected components, resolving both to their
   current root first so repeated calls in any order stay globally
   consistent -- plain union, no path compression (nroom is tiny enough
   that it doesn't matter) */
STATIC_OVL void
smeq_union(int a, int b)
{
	int ra = smeq_root(a), rb = smeq_root(b);

	if (ra < rb)
		smeq[rb] = ra;
	else
		smeq[ra] = rb;
}

STATIC_OVL void
join(a,b,nxcor)
register int a, b;
boolean nxcor;
{
	coord cc,tt, org, dest;
	register xchar tx, ty, xx, yy;
	register struct mkroom *croom, *troom;
	register int dx, dy;

	croom = &rooms[a];
	troom = &rooms[b];

	/* find positions cc and tt for doors in croom and troom
	   and direction for a corridor between them */

	if(troom->hx < 0 || croom->hx < 0 || doorindex >= DOORMAX) return;
	if(troom->lx > croom->hx) {
		dx = 1;
		dy = 0;
		xx = croom->hx+1;
		tx = troom->lx-1;
		finddpos(&cc, xx, croom->ly, xx, croom->hy);
		finddpos(&tt, tx, troom->ly, tx, troom->hy);
	} else if(troom->hy < croom->ly) {
		dy = -1;
		dx = 0;
		yy = croom->ly-1;
		finddpos(&cc, croom->lx, yy, croom->hx, yy);
		ty = troom->hy+1;
		finddpos(&tt, troom->lx, ty, troom->hx, ty);
	} else if(troom->hx < croom->lx) {
		dx = -1;
		dy = 0;
		xx = croom->lx-1;
		tx = troom->hx+1;
		finddpos(&cc, xx, croom->ly, xx, croom->hy);
		finddpos(&tt, tx, troom->ly, tx, troom->hy);
	} else {
		dy = 1;
		dx = 0;
		yy = croom->hy+1;
		ty = troom->ly-1;
		finddpos(&cc, croom->lx, yy, croom->hx, yy);
		finddpos(&tt, troom->lx, ty, troom->hx, ty);
	}
	xx = cc.x;
	yy = cc.y;
	tx = tt.x - dx;
	ty = tt.y - dy;
	if(nxcor && levl[xx+dx][yy+dy].typ)
		return;
	if (IS_WALL(levl[xx][yy].typ) && IS_ROOM(levl[xx + dx][yy + dy].typ))	// make a doorway into a room where the wall is shared
	{
		// brutishly find the room we are connecting to
		int i;
		for (i = 0; i < nroom; i++){
			if (   (rooms[i].lx <= xx + dx)
				&& (rooms[i].hx >= xx + dx)
				&& (rooms[i].ly <= yy + dy)
				&& (rooms[i].hy >= yy + dy)
				)
				break;
		}
		if (i == nroom)
			return;	// it isn't a room?
		else
			troom = &rooms[i];
		// disallow adding extra doors to shops
		if (troom->rtype >= SHOPBASE)
			return;

		// add the door
		if (okdoor(xx, yy))
		{
			dodoor(xx, yy, croom);
			add_door(xx, yy, troom);
		}
		// note the connection
		if (smeq[a] < smeq[i])
			smeq[i] = smeq[a];
		else
			smeq[a] = smeq[i];
		return;
	}
	if (IS_WALL(levl[xx + dx][yy + dy].typ) || IS_ROOM(levl[xx + dx][yy + dy].typ))	// prevent trying to open corridors into adjacent rooms
		return;

	if (okdoor(xx,yy))
	    dodoor(xx,yy,croom);

	org.x  = xx+dx; org.y  = yy+dy;
	dest.x = tx; dest.y = ty;

	if (!dig_corridor(&org, &dest, nxcor,
			level.flags.arboreal ? ROOM : CORR, STONE))
	    return;

	/* we succeeded in digging the corridor */
	if (okdoor(tt.x, tt.y))
	    dodoor(tt.x, tt.y, troom);

	if(smeq[a] < smeq[b])
		smeq[b] = smeq[a];
	else
		smeq[a] = smeq[b];
}

/* 
 * Merges rooms that have adjacent walls (but not overlapping walls)
 */
void
merge_adj_rooms()
{
	int i, j;
	int f, g;
	struct mkroom *a, *b;
	boolean xadj, yadj;
	int minx, maxx, miny, maxy;

	for (i = 0; i < nroom - 1; i++)
	{
		a = &rooms[i];
		for (j = i + 1; j < nroom; j++)
		{
			b = &rooms[j];
			// 1st condition: both rooms must be ordinary or joined rooms
			if (!((a->rtype == OROOM || a->rtype == JOINEDROOM)
				&&(b->rtype == OROOM || b->rtype == JOINEDROOM)
				))
				continue;
			// Determine the wall overlap distance
			minx = max(a->lx, b->lx);
			maxx = min(a->hx, b->hx);
			miny = max(a->ly, b->ly);
			maxy = min(a->hy, b->hy);
			// 2nd condition: the rooms must be adjacent in either x or y
			xadj = abs(minx-maxx) == 3;
			yadj = abs(miny-maxy) == 3;
			if (xadj){
				if (maxy - miny < 0)
					xadj = FALSE;
			}
			if (yadj){
				if (maxx - minx < 0)
					yadj = FALSE;
			}
			if (!(xadj || yadj))
				continue;
			// preferred option: merge the rooms by expanding one room so that they share a wall, and add a door along the shared wall
			boolean okay = TRUE;
			int dp = 0;
			schar *p;
			struct mkroom *t, *u;
			// determine which room is smaller; only attempt to expand that room
			t = ((a->hx - a->lx)*(a->hy - a->ly) < (b->hx - b->lx)*(b->hy - b->ly)) ? a : b;
			u = (t == a) ? b : a;

			// determine which of t's corners has to move, and in which direction
			if (xadj){
				dp = (t->hx < u->lx) ? 1 : -1;
				p = (t->hx < u->lx) ? &(t->hx) : &(t->lx);
			}
			if (yadj){
				dp = (t->hy < u->ly) ? 1 : -1;
				p = (t->hx < u->lx) ? &(t->hy) : &(t->ly);
			}

			// check that it is okay to expand in that direction
			if (xadj){
				for (g = t->ly - 1; g <= t->hy + 1; g++)
				if (!IS_STWALL(levl[*p+dp][g].typ))
					okay = FALSE;
			}
			if (yadj){
				for (f = t->lx - 1; f <= t->hx + 1; f++)
				if (!IS_STWALL(levl[f][*p+dp].typ))
					okay = FALSE;
			}
			if (okay && rn2(4))	// use this method most of the time, but the oddly-shaped rooms are fun too
			{
				// expand the room
				if (xadj){
					for (g = t->ly - 1; g <= t->hy + 1; g++) {
						levl[*p + dp * 2][g].typ = VWALL;
						levl[*p + dp * 2][g].horizontal = 0;
					}
					for (g = t->ly; g <= t->hy; g++) {
						levl[*p + dp * 1][g].typ = ROOM;
						levl[*p + dp * 1][g].horizontal = 0;
					}
				}
				if (yadj){
					for (f = t->lx - 1; f <= t->hx + 1; f++) {
						levl[f][*p + dp * 2].typ = HWALL;
						levl[f][*p + dp * 2].horizontal = 1;
					}
					for (f = t->lx; f <= t->hx; f++) {
						levl[f][*p + dp * 1].typ = ROOM;
						levl[f][*p + dp * 1].horizontal = 0;
					}
				}
				*p += dp;
				// attempt to add a door over the shared length
				if (xadj){
					f = *p + dp;
					g = rn2(maxy - miny + 1) + miny;
				}
				if (yadj){
					f = rn2(maxx - minx + 1) + minx;
					g = *p + dp;
				}

				if (okdoor(f, g))
				{
					dodoor(f, g, t);
					add_door(f, g, u);
				}
				else
				{
					continue;	// it failed to connect the rooms, but it's too late to go to the fallback
				}
			}
			else {
				// fallback option: merge the rooms by replacing the walls
				if (xadj){
					for (g = miny; g <= maxy; g++)
					for (f = maxx + 1; f <= minx - 1; f++)
						levl[f][g].typ = ROOM;
				}
				if (yadj){
					for (f = minx; f <= maxx; f++)
					for (g = maxy + 1; g <= miny - 1; g++)
						levl[f][g].typ = ROOM;
				}
			}
			// I now pronounce you... one room for pathing purposes.
			smeq_union(i, j);
			// make the lighting consistent in the rooms
			t->rlit = u->rlit;
			for (f = t->lx - 1; f <= t->hx + 1; f++) {
			for (g = t->ly - 1; g <= t->hy + 1; g++)
				levl[f][g].lit = t->rlit;
			}
			// change the room types
			a->rtype = JOINEDROOM;
			b->rtype = JOINEDROOM;
		}
	}
	return;
}

void
makecorridors()
{
	int a, b, i;
	boolean any = TRUE;

	/* connect rooms to their first neighbour */
	for (a = 0; a < nroom - 1; a++) {
		/* connect to-and-from multi-rooms at 1/2 the rate */
		if ((rooms[a].rtype == JOINEDROOM || rooms[a + 1].rtype == JOINEDROOM) && rn2(2))
			continue;
		/* join them */
		join(a, a + 1, FALSE);
		/* allow some randomness and occasionally just stop connecting rooms in this manner */
		if (!rn2(50))
			break;
	}
	/* connect rooms to their second neighbour */
	/* but only if they aren't connected yet */
	for (a = 0; a < nroom - 2; a++)
	{
		if (smeq[a] != smeq[a + 2])
			join(a, a + 2, FALSE);
	}
	/* make additional connections between rooms, at random */
	if (nroom > 2) {
		/* makes from 4 to (rooms+4) attempts */
		for (i = rn2(nroom) + 4; i; i--) {
			/* pick two rooms */
			a = rn2(nroom);
			b = rn2(nroom - 2);
			if (b >= a) b += 2;
			/* connect to-and-from multi-rooms at 1/8 the rate */
			if ((rooms[a].rtype == JOINEDROOM || rooms[b].rtype == JOINEDROOM) && rn2(8))
				continue;
			/* allow join() to fail partway through making the corridor */
			join(a, b, TRUE);
		}
	}
	/* finish connecting all rooms */
	/* but only if they aren't connected yet */
	for (a = 0; any && a < nroom; a++) {
		/* "any" just serves to end this loop early to reduce processing time */
		any = FALSE;
		for (b = 0; b < nroom; b++)
		{
			if (smeq[a] != smeq[b]) {
				join(a, b, FALSE);
				any = TRUE;
			}
		}
	}
	return;
}

enum halldir { HALL_E = 0, HALL_W, HALL_N, HALL_S };
static const schar halldx[4] = { 1, -1, 0, 0 };
static const schar halldy[4] = { 0, 0, -1, 1 };

/* find room r's valid 3-wide attachment slot range on one wall; returns
   FALSE (empty range) if the room is too small on that wall */
STATIC_OVL boolean
wall_slot_range(struct mkroom *r, int wall, xchar *fixed, xchar *lo, xchar *hi)
{
	switch (wall) {
	case HALL_E: *fixed = r->hx + 1; *lo = r->ly + 1; *hi = r->hy - 1; break;
	case HALL_W: *fixed = r->lx - 1; *lo = r->ly + 1; *hi = r->hy - 1; break;
	case HALL_N: *fixed = r->ly - 1; *lo = r->lx + 1; *hi = r->hx - 1; break;
	case HALL_S: *fixed = r->hy + 1; *lo = r->lx + 1; *hi = r->hx - 1; break;
	default:     *fixed = *lo = *hi = 0; return FALSE;
	}
	return (*hi >= *lo);
}

/* find room r's valid single-wide door slot range on one wall; returns
   FALSE (empty range) if the room is too small on that wall.  Covers the
   full wall including corner rows/columns -- okdoor() (flat wall, no
   adjacent door) is the only real constraint on a single door. */
STATIC_OVL boolean
door_slot_range(struct mkroom *r, int wall, xchar *fixed, xchar *lo, xchar *hi)
{
	switch (wall) {
	case HALL_E: *fixed = r->hx + 1; *lo = r->ly; *hi = r->hy; break;
	case HALL_W: *fixed = r->lx - 1; *lo = r->ly; *hi = r->hy; break;
	case HALL_N: *fixed = r->ly - 1; *lo = r->lx; *hi = r->hx; break;
	case HALL_S: *fixed = r->hy + 1; *lo = r->lx; *hi = r->hx; break;
	default:     *fixed = *lo = *hi = 0; return FALSE;
	}
	return (*hi >= *lo);
}

/* is a straight 3-wide run from (x1,y1) to (x2,y2) clear?  horiz TRUE
   means the run travels along x (y1 must equal y2, the fixed row);
   horiz FALSE means it travels along y (x1 must equal x2, the fixed
   column).  STONE only during pass 1; also allows already-carved hallway
   terrain during pass 2 (allow_existing), same as dig_corridor()'s own
   passthrough of previously-dug corridor tiles. */
STATIC_OVL boolean
hallway_swath_ok(int x1, int y1, int x2, int y2, int horiz, int allow_existing)
{
	xchar x, y, w, lo, hi;
	int typ;

	if (horiz) {
		lo = min(x1, x2); hi = max(x1, x2);
		for (x = lo; x <= hi; x++)
			for (w = -1; w <= 1; w++) {
				y = y1 + w;
				if (!isok(x, y)) return FALSE;
				typ = levl[x][y].typ;
				if (typ == STONE) continue;
				if (allow_existing && (typ == CORR || typ == SCORR ||
						(level.flags.arboreal && typ == ROOM)))
					continue;
				return FALSE;
			}
	} else {
		lo = min(y1, y2); hi = max(y1, y2);
		for (y = lo; y <= hi; y++)
			for (w = -1; w <= 1; w++) {
				x = x1 + w;
				if (!isok(x, y)) return FALSE;
				typ = levl[x][y].typ;
				if (typ == STONE) continue;
				if (allow_existing && (typ == CORR || typ == SCORR ||
						(level.flags.arboreal && typ == ROOM)))
					continue;
				return FALSE;
			}
	}
	return TRUE;
}

/* does the 3-wide cross-section at travel position `pos` (fixed
   perpendicular coordinate `fixed`) already contain corridor floor from
   an earlier join this same pass? */
STATIC_OVL boolean
hallway_cross_overlaps(int pos, int fixed, int horiz)
{
	int w, x, y, typ;

	for (w = -1; w <= 1; w++) {
		if (horiz) { x = pos; y = fixed + w; }
		else       { x = fixed + w; y = pos; }
		if (!isok(x, y)) continue;
		typ = levl[x][y].typ;
		if (typ == CORR || typ == SCORR ||
				(level.flags.arboreal && typ == ROOM))
			return TRUE;
	}
	return FALSE;
}

/* carve one 3-wide cross-section at travel position `pos`; only
   converts cells that are currently STONE */
STATIC_OVL void
hallway_cross_carve(int pos, int fixed, int horiz, int ftyp)
{
	int w, x, y;

	for (w = -1; w <= 1; w++) {
		if (horiz) { x = pos; y = fixed + w; }
		else       { x = fixed + w; y = pos; }
		if (isok(x, y) && levl[x][y].typ == STONE)
			levl[x][y].typ = ftyp;
	}
}

/* carve a 3-cross-section-long connector anchored at `pos`: dir=1
   reaches forward (pos,pos+1,pos+2) to bridge into an existing corridor
   just encountered; dir=-1 reaches backward (pos,pos-2,pos-1) to
   backfill the under-width tail of a coasting stretch that just ended.
   Never reaches outside [lo,hi] -- pass 1 only validated cells inside
   the leg's own range, so anything past either end is unvalidated rock
   with no guaranteed connection to anything, and carving into it would
   just leave an orphaned stub. */
STATIC_OVL void
hallway_connector_carve(int pos, int fixed, int horiz, int dir, int lo, int hi, int ftyp)
{
	int i, q;

	for (i = 0; i < 3; i++) {
		q = pos + i * dir;
		if (q < lo || q > hi) continue;
		hallway_cross_carve(q, fixed, horiz, ftyp);
	}
}

/* carve a validated straight 3-wide run (see hallway_swath_ok() for the
   horiz convention), stepping from (x1,y1) to (x2,y2) one cross-section
   at a time.  (x1,y1) is always the room's own outward step, which may
   land on either side of (x2,y2) depending on the leg's travel
   direction.  Wherever the path runs into corridor floor an earlier
   join this same pass already carved, stop digging our own width and
   coast through the existing corridor instead, bridging the transition
   with a connector rather than letting the two corridors' swaths
   overlap and locally widen the passage.

   ends_at_elbow is TRUE when (x2,y2) is an elbow leg's own endpoint, one
   step past the shared corner (see carve_hallway()), rather than a
   room's own wall.  In that case, once coasting, don't re-emerge within
   the last 3 steps -- that span is exactly the corner's own extent,
   which the *other* leg's own matching extension independently reaches
   too.  This does not apply to a straight hallway's own room-side
   endpoint, which has no such backup.

   start_coasting is TRUE when the door at (x1,y1)'s room end failed to
   place (see dohalldoor()), typically because another hallway's door
   already sits right there.  The room end is then already effectively
   merged with whatever's adjacent, so the walk begins already coasting
   instead of digging its own redundant width there.  It can still
   emerge normally later, same as any other coasting stretch, once
   overlap genuinely ends. */
STATIC_OVL void
hallway_swath_carve(int x1, int y1, int x2, int y2, int horiz, int ends_at_elbow, int start_coasting, int ftyp)
{
	int fixed = horiz ? y1 : x1;
	int start = horiz ? x1 : y1;
	int end = horiz ? x2 : y2;
	int lo = min(start, end), hi = max(start, end);
	int step = (end >= start) ? 1 : -1;
	int p, i, n = hi - lo + 1;
	boolean coasting = start_coasting;

	for (i = 0, p = start; i < n; i++, p += step) {
		boolean overlap = hallway_cross_overlaps(p, fixed, horiz);

		if (overlap && !coasting) {
			hallway_connector_carve(p, fixed, horiz, step, lo, hi, ftyp);
			coasting = TRUE;
			continue;
		}
		if (!overlap && coasting) {
			if (ends_at_elbow) {
				int dist = (end > p) ? end - p : p - end;
				if (dist < 3)
					continue; /* the other leg's own extension covers this */
			}
			hallway_connector_carve(p, fixed, horiz, -step, lo, hi, ftyp);
			coasting = FALSE;
			continue;
		}
		if (!coasting)
			hallway_cross_carve(p, fixed, horiz, ftyp);
	}
}

/* recover room r's wall attachment point and outward normal from a
   hallelbow[][] coordinate -- exactly one of the horizontal/vertical
   tests holds by construction (see find_hallway()) */
STATIC_OVL void
hallway_endpoint(int r, int ex, int ey, xchar *px, xchar *py, schar *pdx, schar *pdy)
{
	struct mkroom *room = &rooms[r];
	boolean horiz = (room->ly <= ey && ey <= room->hy) &&
			(ex < room->lx || ex > room->hx);
	boolean vert  = (room->lx <= ex && ex <= room->hx) &&
			(ey < room->ly || ey > room->hy);

	if (horiz) {
		*py = ey;
		*pdy = 0;
		if (ex > room->hx) { *px = room->hx + 1; *pdx = 1; }
		else                { *px = room->lx - 1; *pdx = -1; }
	} else if (vert) {
		*px = ex;
		*pdx = 0;
		if (ey > room->hy) { *py = room->hy + 1; *pdy = 1; }
		else                { *py = room->ly - 1; *pdy = -1; }
	} else {
		impossible("hallway_endpoint: room %d has no wall facing (%d,%d)",
				r, (int)ex, (int)ey);
		*px = ex; *py = ey; *pdx = 0; *pdy = 0;
	}
}

/* pass 1: search every 3-wide, <=1-turn hallway between rooms a and b
   against the pristine map, reservoir-sampling uniformly among every
   valid candidate found (straight and elbow alike).  Returns FALSE if no
   candidate exists. */
STATIC_OVL boolean
find_hallway(int a, int b, xchar *elbx, xchar *elby)
{
	struct mkroom *ra = &rooms[a], *rb = &rooms[b];
	static const int opp[4]     = { HALL_W, HALL_E, HALL_S, HALL_N };
	static const int perp[4][2] = {
		{ HALL_N, HALL_S },	/* HALL_E */
		{ HALL_N, HALL_S },	/* HALL_W */
		{ HALL_E, HALL_W },	/* HALL_N */
		{ HALL_E, HALL_W }	/* HALL_S */
	};
	int wa, wb, k, chcnt = 0;
	xchar fa, loa, hia, fb, lob, hib;
	xchar bestx = 0, besty = 0;

	/* straight (0-turn): opposite-facing wall pairs */
	for (wa = 0; wa < 4; wa++) {
		xchar lo, hi, s;
		boolean horiz;

		wb = opp[wa];
		if (!wall_slot_range(ra, wa, &fa, &loa, &hia)) continue;
		if (!wall_slot_range(rb, wb, &fb, &lob, &hib)) continue;
		horiz = (wa == HALL_E || wa == HALL_W);
		lo = max(loa, lob);
		hi = min(hia, hib);
		for (s = lo; s <= hi; s++) {
			xchar x1, y1, x2, y2;

			if (horiz) { x1 = fa + halldx[wa]; y1 = s; x2 = fb + halldx[wb]; y2 = s; }
			else       { x1 = s; y1 = fa + halldy[wa]; x2 = s; y2 = fb + halldy[wb]; }
			if (x1 > x2 || y1 > y2) continue; /* rooms face the wrong way */
			if (hallway_swath_ok(x1, y1, x2, y2, horiz, FALSE)) {
				chcnt++;
				if (!rn2(chcnt)) {
					if (horiz) { bestx = fb; besty = s; }
					else       { bestx = s;  besty = fb; }
				}
			}
		}
	}

	/* elbow (1-turn): perpendicular wall pairs, full cross-product of slots */
	for (wa = 0; wa < 4; wa++) {
		xchar sa;
		boolean horiz_a;

		if (!wall_slot_range(ra, wa, &fa, &loa, &hia)) continue;
		horiz_a = (wa == HALL_E || wa == HALL_W);
		for (k = 0; k < 2; k++) {
			boolean horiz_b;

			wb = perp[wa][k];
			if (!wall_slot_range(rb, wb, &fb, &lob, &hib)) continue;
			horiz_b = (wb == HALL_E || wb == HALL_W);
			for (sa = loa; sa <= hia; sa++) {
				xchar pax, pay, sb;

				if (horiz_a) { pax = fa; pay = sa; }
				else         { pax = sa; pay = fa; }
				for (sb = lob; sb <= hib; sb++) {
					xchar pbx, pby, ex, ey;

					if (horiz_b) { pbx = fb; pby = sb; }
					else         { pbx = sb; pby = fb; }

					if (horiz_a) { ex = pbx; ey = pay; }
					else         { ex = pax; ey = pby; }

					if ((ex - pax) * halldx[wa] + (ey - pay) * halldy[wa] < 1)
						continue;
					if ((ex - pbx) * halldx[wb] + (ey - pby) * halldy[wb] < 1)
						continue;

					if (hallway_swath_ok(pax + halldx[wa], pay + halldy[wa], ex, ey,
								horiz_a, FALSE) &&
					    hallway_swath_ok(pbx + halldx[wb], pby + halldy[wb], ex, ey,
								horiz_b, FALSE)) {
						chcnt++;
						if (!rn2(chcnt)) { bestx = ex; besty = ey; }
					}
				}
			}
		}
	}

	if (!chcnt) return FALSE;
	*elbx = bestx;
	*elby = besty;
	return TRUE;
}

/* place a single door at a hallway's wall attachment point, same as
   join() -- the flanking wall tiles are never touched.  Returns FALSE
   if no door could be placed (doorindex exhausted, or another door --
   typically from a different join reaching the same room nearby --
   already sits on or next to this exact spot); callers use this to
   tell whether the corridor here is starting genuinely fresh or is
   effectively already merged with whatever's adjacent. */
STATIC_OVL boolean
dohalldoor(int cx, int cy, struct mkroom *aroom)
{
	if (!okdoor(cx, cy))
		return FALSE;
	dodoor(cx, cy, aroom);
	return TRUE;
}

/* pass 2: carve one pass-1-approved hallway and merge the two rooms'
   connected components.  By construction every cell along the path is
   either still STONE or already-carved hallway floor from an earlier
   join this same pass, so no backtracking is needed. */
STATIC_OVL void
carve_hallway(int a, int b, int ex, int ey)
{
	xchar pax, pay, pbx, pby;
	schar dxa, dya, dxb, dyb;
	boolean horiz_a, horiz_b, doored_a, doored_b;
	schar ftyp = level.flags.arboreal ? ROOM : CORR;

	hallway_endpoint(a, ex, ey, &pax, &pay, &dxa, &dya);
	hallway_endpoint(b, ex, ey, &pbx, &pby, &dxb, &dyb);
	horiz_a = (dya == 0);
	horiz_b = (dyb == 0);

	/* place both doors before carving -- if one fails (typically
	   because a different join already put a door right there), that
	   leg's walk starts already coasting instead of digging its own
	   redundant width beside whatever caused the door to fail */
	doored_a = dohalldoor(pax, pay, &rooms[a]);
	doored_b = dohalldoor(pbx, pby, &rooms[b]);

	if (horiz_a == horiz_b) {
		/* straight: a single leg from A's outward step to B's */
		hallway_swath_carve(pax + dxa, pay + dya, pbx + dxb, pby + dyb,
					horiz_a, FALSE, !doored_a, ftyp);
	} else {
		/* elbow: each leg travels one step past the shared corner, in
		   its own direction of travel, instead of stopping exactly at
		   it.  Since a leg's normal cross-section carve always sweeps
		   its full 3-wide perpendicular band, that extra step makes
		   each leg's own sweep reach every cell of the corner's 3x3
		   block on its own. */
		hallway_swath_carve(pax + dxa, pay + dya, ex + dxa, ey + dya,
					horiz_a, TRUE, !doored_a, ftyp);
		hallway_swath_carve(pbx + dxb, pby + dyb, ex + dxb, ey + dyb,
					horiz_b, TRUE, !doored_b, ftyp);
	}

	smeq_union(a, b);
}

/* convert every corridor (and secret corridor/door) cell on the level
   into room floor with proper surrounding walls, giving a built,
   walled-hallway appearance instead of a raw-rock tunnel look. */
STATIC_OVL void
wallify_mithardir_corridors(void)
{
	int x, y;

	for (x = 0; x < COLNO; x++) {
		for (y = 0; y < ROWNO; y++) {
			levl[x][y].lit = FALSE;
			if (levl[x][y].typ == CORR)
				levl[x][y].typ = ROOM;
			else if (levl[x][y].typ == SCORR)
				levl[x][y].typ = ROOM;
			else if (levl[x][y].typ == SDOOR) {
				levl[x][y].typ = DOOR;
				levl[x][y].doormask = D_LOCKED;
			} else if (IS_ROCK(levl[x][y].typ))
				levl[x][y].typ = VWALL;
		}
	}
	wallification(1, 0, COLNO - 1, ROWNO - 1);
}

/* convert makehallways()'s own CORR/SCORR terrain into a walled
   hallway.  SCORR cells caught by the neighbor scan become SDOOR
   (fallback-corridor terrain bridging into a wallified primary
   hallway).  Only walls the 3x3 neighborhood of each corridor cell,
   rather than the whole map, so that SCORR elsewhere is preserved.
   These will be reverted to plain CORR before wallification(). */
STATIC_OVL void
wallify_hallway_corridors(void)
{
	int x, y, i, j, ix, jy;

	for (x = 0; x < COLNO; x++) {
		for (y = 0; y < ROWNO; y++) {
			if (levl[x][y].typ == CORR){
				levl[x][y].typ = ROOM;
			}
			if(levl[x][y].typ != STONE && levl[x][y].typ != SCORR
				 && levl[x][y].typ != SDOOR && levl[x][y].typ != DOOR
				 && !IS_WALL(levl[x][y].typ)
			){
				for(i = -1; i <= 1; i++){
					for(j = -1; j <= 1; j++){
						ix = x + i;
						jy = y + j;
						if (isok(ix, jy)){
							if(levl[ix][jy].typ == STONE)
								levl[ix][jy].typ = VWALL;
							else if(levl[ix][jy].typ == SCORR){
								levl[ix][jy].typ = SDOOR;
								levl[ix][jy].doormask = !rn2(5) ? D_LOCKED : D_CLOSED;
							}
						}
					}
				}
			}
		}
	}
	for (x = 0; x < COLNO; x++) {
		for (y = 0; y < ROWNO; y++) {
			if (levl[x][y].typ == SCORR)
				levl[x][y].typ = CORR;
		}
	}
	wallification(1, 0, COLNO - 1, ROWNO - 1);
}

/* compute a placed pair's 1 or 2 leg rectangles -- the same straight-vs-
   elbow, extend-one-step-past-the-corner geometry carve_hallway() already
   uses to carve, just producing rectangle descriptions instead of
   digging.  lo/hi/fixed/horiz are caller-supplied length-2 arrays; only
   index 0 is meaningful when the return value is 1. */
STATIC_OVL int
hallway_legs(int a, int b, int ex, int ey, xchar *lo, xchar *hi, xchar *fixed, boolean *horiz)
{
	xchar pax, pay, pbx, pby;
	schar dxa, dya, dxb, dyb;
	boolean horiz_a, horiz_b;
	int s1, s2;

	hallway_endpoint(a, ex, ey, &pax, &pay, &dxa, &dya);
	hallway_endpoint(b, ex, ey, &pbx, &pby, &dxb, &dyb);
	horiz_a = (dya == 0);
	horiz_b = (dyb == 0);

	if (horiz_a == horiz_b) {
		s1 = horiz_a ? pax + dxa : pay + dya;
		s2 = horiz_a ? pbx + dxb : pby + dyb;
		lo[0] = min(s1, s2);
		hi[0] = max(s1, s2);
		fixed[0] = horiz_a ? pay : pax;
		horiz[0] = horiz_a;
		return 1;
	}

	s1 = horiz_a ? pax + dxa : pay + dya;
	s2 = horiz_a ? ex + dxa : ey + dya;
	lo[0] = min(s1, s2);
	hi[0] = max(s1, s2);
	fixed[0] = horiz_a ? pay : pax;
	horiz[0] = horiz_a;

	s1 = horiz_b ? pbx + dxb : pby + dyb;
	s2 = horiz_b ? ex + dxb : ey + dyb;
	lo[1] = min(s1, s2);
	hi[1] = max(s1, s2);
	fixed[1] = horiz_b ? pby : pbx;
	horiz[1] = horiz_b;
	return 2;
}

/* do two hallway legs' rectangles overlap in coordinate space?  Because
   the only non-STONE terrain any hallway's swath can ever run into is
   another makehallways() hallway's own carved floor from this same pass
   (see the coasting/merge design), two different hallways' legs
   overlapping here is equivalent to an actual physical merge having
   happened during carving. */
STATIC_OVL boolean
hallway_legs_overlap(int lo1, int hi1, int fixed1, int horiz1,
			int lo2, int hi2, int fixed2, int horiz2)
{
	int xlo1, xhi1, ylo1, yhi1, xlo2, xhi2, ylo2, yhi2;

	if (horiz1) { xlo1 = lo1; xhi1 = hi1; ylo1 = fixed1 - 1; yhi1 = fixed1 + 1; }
	else        { xlo1 = fixed1 - 1; xhi1 = fixed1 + 1; ylo1 = lo1; yhi1 = hi1; }
	if (horiz2) { xlo2 = lo2; xhi2 = hi2; ylo2 = fixed2 - 1; yhi2 = fixed2 + 1; }
	else        { xlo2 = fixed2 - 1; xhi2 = fixed2 + 1; ylo2 = lo2; yhi2 = hi2; }

	return (boolean) (xlo1 <= xhi2 && xlo2 <= xhi1 && ylo1 <= yhi2 && ylo2 <= yhi1);
}

/* is the candidate wall square at perpendicular offset side*2 from fixed
   (side is -1 or +1), travel position p, actually a wall at all -- and if
   so, is it padded by three STONE tiles (itself plus the two cells
   further out)?  Returns FALSE (candidate not eligible, *padded
   untouched) if the square is off the map or is itself corridor floor --
   which means it's actually floor belonging to this hallway's other leg,
   or a different hallway entirely, not a wall. */
STATIC_OVL boolean
hallway_wall_square_stat(int p, int fixed, int horiz, int side, boolean *padded)
{
	int i, x, y, off, typ;

	off = side * 2;
	x = horiz ? p : fixed + off;
	y = horiz ? fixed + off : p;
	if (!isok(x, y))
		return FALSE;
	typ = levl[x][y].typ;
	if (typ == CORR || typ == SCORR || (level.flags.arboreal && typ == ROOM))
		return FALSE;

	*padded = TRUE;
	for (i = 2; i <= 4; i++) {
		off = side * i;
		x = horiz ? p : fixed + off;
		y = horiz ? fixed + off : p;
		if (!isok(x, y) || levl[x][y].typ != STONE) {
			*padded = FALSE;
			break;
		}
	}
	return TRUE;
}

/* replace an "interesting" hallway's corridor terrain with a distinctive
   floor type -- only cells that are still CORR/SCORR are touched, so
   doors and any terrain this leg merely coasted through (belonging to
   some other hallway) are left alone.  Must run before
   wallify_hallway_corridors(), which would otherwise have already
   turned this hallway's own CORR cells into ordinary ROOM floor. */
STATIC_OVL void
hallway_paint_floor(int nlegs, xchar *lo, xchar *hi, xchar *fixed, boolean *horiz, int ftyp)
{
	int i, p, w, x, y;

	for (i = 0; i < nlegs; i++)
		for (p = lo[i]; p <= hi[i]; p++)
			for (w = -1; w <= 1; w++) {
				x = horiz[i] ? p : fixed[i] + w;
				y = horiz[i] ? fixed[i] + w : p;
				if (isok(x, y) &&
						(levl[x][y].typ == CORR || levl[x][y].typ == SCORR))
					levl[x][y].typ = ftyp;
			}
}

STATIC_OVL boolean
all_rooms_connected(void)
{
	int i;

	for (i = 1; i < nroom; i++)
		if (smeq_root(i) != smeq_root(0))
			return FALSE;
	return TRUE;
}

/* Manhattan gap between two rooms' bounding boxes (0 on an axis where
   they already overlap); wall_from/wall_to give one facing-wall pair
   (whichever axis has the larger gap).  See room_wall_pairs() for every
   axis that actually separates the two rooms. */
STATIC_OVL int
room_gap(struct mkroom *from, struct mkroom *to, int *wall_from, int *wall_to)
{
	int dx = 0, dy = 0;

	if (from->hx < to->lx)      dx = to->lx - from->hx;
	else if (to->hx < from->lx) dx = from->lx - to->hx;
	if (from->hy < to->ly)      dy = to->ly - from->hy;
	else if (to->hy < from->ly) dy = from->ly - to->hy;

	if (dx >= dy) {
		*wall_from = (to->lx > from->hx) ? HALL_E : HALL_W;
		*wall_to   = (to->lx > from->hx) ? HALL_W : HALL_E;
	} else {
		*wall_from = (to->ly > from->hy) ? HALL_S : HALL_N;
		*wall_to   = (to->ly > from->hy) ? HALL_N : HALL_S;
	}
	return dx + dy;
}

/* enumerate every axis that actually separates two rooms (as opposed to
   room_gap()'s single dominant pick) as a facing-wall pair to try a
   fallback corridor on, shorter gap first.  A room pair offset diagonally
   (both bounding-box axes non-overlapping, e.g. two rooms in different
   corners) yields both same-axis pairs (from's W<->to's E, from's S<->to's
   N) plus the two cross pairs (from's W<->to's N, from's S<->to's E) --
   an irregularly-shaped room can easily have its "natural" axis-matched
   wall blocked while a cross-facing wall is wide open, and
   line_path_clear()'s Bresenham line has no trouble with a non-axis-
   aligned path between them.  A pair that already shares a row or column
   yields only the one axis that actually separates them (no cross pairs
   make sense without a genuine diagonal gap).  Returns the count filled
   into wall_from/wall_to/gap (length-4 caller-supplied arrays) -- 0, 1, or
   4 (never 2 or 3: a diagonal offset always yields all 4 candidates in one
   shot, since we don't know in advance which will find a clear path). */
STATIC_OVL int
room_wall_pairs(struct mkroom *from, struct mkroom *to,
			int *wall_from, int *wall_to, int *gap)
{
	int dx = 0, dy = 0;
	int wfx = -1, wtx = -1, wfy = -1, wty = -1, n = 0;

	if (from->hx < to->lx)      { dx = to->lx - from->hx; wfx = HALL_E; wtx = HALL_W; }
	else if (to->hx < from->lx) { dx = from->lx - to->hx; wfx = HALL_W; wtx = HALL_E; }
	if (from->hy < to->ly)      { dy = to->ly - from->hy; wfy = HALL_S; wty = HALL_N; }
	else if (to->hy < from->ly) { dy = from->ly - to->hy; wfy = HALL_N; wty = HALL_S; }

	if (wfx != -1 && wfy != -1) {
		if (dx <= dy) {
			wall_from[0] = wfx; wall_to[0] = wtx; gap[0] = dx;
			wall_from[1] = wfy; wall_to[1] = wty; gap[1] = dy;
		} else {
			wall_from[0] = wfy; wall_to[0] = wty; gap[0] = dy;
			wall_from[1] = wfx; wall_to[1] = wtx; gap[1] = dx;
		}
		wall_from[2] = wfx; wall_to[2] = wty; gap[2] = dx + dy;
		wall_from[3] = wfy; wall_to[3] = wtx; gap[3] = dx + dy;
		n = 4;
	} else if (wfx != -1) {
		wall_from[0] = wfx; wall_to[0] = wtx; gap[0] = dx;
		n = 1;
	} else if (wfy != -1) {
		wall_from[0] = wfy; wall_to[0] = wty; gap[0] = dy;
		n = 1;
	}
	return n;
}

/* classify a single map cell for fallback-corridor purposes */
STATIC_OVL int
hallway_cell_class(int x, int y)
{
	int typ;

	if (!isok(x, y)) return FB_CELL_OTHER;
	typ = levl[x][y].typ;
	if (typ == STONE || typ == SCORR) return FB_CELL_OPEN;
	if (typ == CORR) return FB_CELL_CORR;
	return FB_CELL_OTHER;
}

/* does any of (x,y)'s 8 neighbors belong to a primary hallway?  SCORR
   neighbors don't count -- corridors already cross each other freely
   in NetHack, only running parallel to a revealed hallway is the
   problem (see hallway_fallback_attempt()'s doc comment). */
STATIC_OVL boolean
hallway_corr_adjacent(int x, int y)
{
	int i, j;

	for (i = -1; i <= 1; i++)
		for (j = -1; j <= 1; j++) {
			if (!i && !j) continue;
			if (hallway_cell_class(x + i, y + j) == FB_CELL_CORR)
				return TRUE;
		}
	return FALSE;
}

/* is (x,y) a plain, unbroken room-wall tile -- i.e. exactly the terrain
   type okdoor() itself requires, checked in isolation from doorindex/
   bydoor() so a caller can tell "not a valid wall at all" (room corner,
   or anything else) apart from "a valid wall but already doored" */
STATIC_OVL boolean
hallway_valid_door_wall(int x, int y)
{
	return (boolean) (isok(x, y) &&
			(levl[x][y].typ == HWALL || levl[x][y].typ == VWALL));
}

/* does a straight, orthogonally-stepped line from (x1,y1) to (x2,y2) run
   entirely through open rock (STONE/SCORR), with none of it running
   alongside (orthogonally or diagonally adjacent to) a primary hallway's
   own CORR floor?  Pure predicate, no map mutation -- a standard
   Bresenham error accumulator, except whenever both axes would change in
   the same step, that step is split into two sequential single-axis
   steps instead of one diagonal step, so every two consecutive path
   cells stay orthogonally adjacent.  On success, fills the caller-
   supplied pathx/pathy (sized COLNO+ROWNO, comfortably above the
   dx+dy+1-cell worst case) and *pathlen; returns FALSE (path arrays
   untouched) the instant any cell fails, including the endpoints. */
STATIC_OVL boolean
line_path_clear(int x1, int y1, int x2, int y2, xchar *pathx, xchar *pathy, int *pathlen)
{
	int dx = abs(x2 - x1), dy = abs(y2 - y1);
	int sx = (x2 > x1) ? 1 : -1;
	int sy = (y2 > y1) ? 1 : -1;
	int x = x1, y = y1, err = dx - dy, n = 0;

	if (hallway_cell_class(x, y) != FB_CELL_OPEN || hallway_corr_adjacent(x, y)) return FALSE;
	pathx[n] = x; pathy[n] = y; n++;

	while (x != x2 || y != y2) {
		int e2 = 2 * err;

		if (e2 > -dy) {
			err -= dy; x += sx;
			if (hallway_cell_class(x, y) != FB_CELL_OPEN || hallway_corr_adjacent(x, y)) return FALSE;
			pathx[n] = x; pathy[n] = y; n++;
		}
		if (e2 < dx) {
			err += dx; y += sy;
			if (hallway_cell_class(x, y) != FB_CELL_OPEN || hallway_corr_adjacent(x, y)) return FALSE;
			pathx[n] = x; pathy[n] = y; n++;
		}
	}
	*pathlen = n;
	return TRUE;
}

/* level 1: does this endpoint's nominated door have a usable rock square
   immediately outside it?  Sets e->goalx/goaly either way; e->doorx/doory,
   e->odoorx/odoory, and e->odx/ody must already be filled in by the
   caller. */
STATIC_OVL int
fb_level1(struct fb_endpoint *e)
{
	e->goalx = e->doorx + e->odx;
	e->goaly = e->doory + e->ody;
	if (hallway_cell_class(e->goalx, e->goaly) != FB_CELL_OPEN)
		return FB_STOP;
	if (hallway_corr_adjacent(e->goalx, e->goaly))
		return FB_ESCALATE;
	return FB_KEEP;
}

/* level 2: pivot away from the hallway that blocked level 1, and validate
   (or establish) a physical connection from the room's own wall into
   that hallway.  Only called when fb_level1() returned FB_ESCALATE, so
   e->goalx/goaly is already known OPEN and CORR-adjacent -- the two
   along-wall neighbors checked below can't both be non-CORR without
   contradicting that, so at least one of corrA/corrB is always TRUE.
   Never returns FB_KEEP: a successful pivot always hands off to level 3,
   even where a single step already happens to be fully clear -- level
   3's own step-0 evaluation covers that case uniformly. */
STATIC_OVL int
fb_level2(struct fb_endpoint *e)
{
	schar pAx, pAy, pBx, pBy;
	boolean corrA, corrB;

	if (e->odx) { pAx = 0; pAy = -1; pBx = 0; pBy = 1; }
	else        { pAx = -1; pAy = 0; pBx = 1; pBy = 0; }

	corrA = (hallway_cell_class(e->goalx + pAx, e->goaly + pAy) == FB_CELL_CORR);
	corrB = (hallway_cell_class(e->goalx + pBx, e->goaly + pBy) == FB_CELL_CORR);
	if (corrA == corrB)
		return FB_STOP; /* both sides blocked, or (shouldn't happen) neither */

	e->tdx = corrA ? pBx : pAx;
	e->tdy = corrA ? pBy : pAy;

	/* look back at the room's own wall, shifted one step along the wall
	   toward the hallway (opposite of the pivot direction) -- guaranteed
	   CORR-adjacent by the check above, no need to re-verify */
	e->wallx = e->odoorx - e->tdx;
	e->wally = e->odoory - e->tdy;

	if (!hallway_valid_door_wall(e->wallx, e->wally))
		return FB_STOP; /* room corner, or otherwise not a plain wall tile */
	e->need_door = !bydoor(e->wallx, e->wally);
	/* if need_door is TRUE but doorindex is exhausted, dosdoor()'s own
	   existing impossible("DOORMAX exceeded?") reports that at commit
	   time -- not this function's concern */

	e->doorx = e->goalx; e->doory = e->goaly; /* old goal becomes new door */
	e->goalx = e->doorx + e->tdx; e->goaly = e->doory + e->tdy;
	e->level = 2;
	return FB_ESCALATE;
}

/* level 3: walk door/goal together, one step at a time along e->odx/ody
   (the room's own outward normal, which is also the blocking hallway's
   own travel direction, since it runs perpendicular to the room's wall).
   e->doorx/doory may legitimately land on literal CORR (that tile becomes
   the room's bridge into the hallway's edge at commit time). e->goalx/
   goaly is meant to stay open rock (it's handed to line_path_clear() as
   the shared line's endpoint) -- but if the hallway itself turns to run
   across our path, it can transiently become CORR too; "invalid" below
   tracks that, recomputed fresh every call from the terrain itself so it
   can never desync from the actual map state. */
STATIC_OVL int
fb_level3_step(struct fb_endpoint *e, struct fb_endpoint *other)
{
	xchar ngoalx, ngoaly, ndoorx, ndoory;
	int goal_cls, door_cls;
	boolean invalid = (hallway_cell_class(e->goalx, e->goaly) == FB_CELL_CORR);
	boolean aligned;

	if (e->odx) aligned = (e->goalx == other->goalx);
	else        aligned = (e->goaly == other->goaly);
	if (aligned)
		return invalid ? FB_STOP : FB_KEEP; /* invalid: can't line-draw from here yet -- deferred */

	ngoalx = e->goalx + e->odx; ngoaly = e->goaly + e->ody;
	ndoorx = e->doorx + e->odx; ndoory = e->doory + e->ody;
	goal_cls = hallway_cell_class(ngoalx, ngoaly);
	door_cls = hallway_cell_class(ndoorx, ndoory);

	if (goal_cls == FB_CELL_OTHER || door_cls == FB_CELL_OTHER)
		return invalid ? FB_STOP : FB_KEEP; /* about to hit a third room */

	if (goal_cls == FB_CELL_CORR && door_cls == FB_CELL_OPEN) {
		/* the hallway is now on the door side instead of the goal side --
		   we're still linked to the room via the CORR squares already
		   crossed, so just swap roles and keep tracing */
		xchar tx = ndoorx, ty = ndoory;
		e->doorx = ngoalx; e->doory = ngoaly;
		e->goalx = tx;      e->goaly = ty;
		e->tdx = -e->tdx;   e->tdy = -e->tdy;
		return FB_CONTINUE;
	}

	if (goal_cls == FB_CELL_OPEN && door_cls == FB_CELL_OPEN && invalid) {
		/* emerged from crossing right at a corner: both sides open, but
		   still adjacent to the hallway -- re-pivot away from it like
		   level 2 did, then stop */
		schar pAx, pAy, pBx, pBy;
		boolean corrA, corrB;

		e->doorx = ndoorx; e->doory = ndoory;
		if (e->odx) { pAx = 0; pAy = -1; pBx = 0; pBy = 1; }
		else        { pAx = -1; pAy = 0; pBx = 1; pBy = 0; }
		corrA = (hallway_cell_class(e->doorx + pAx, e->doory + pAy) == FB_CELL_CORR);
		corrB = (hallway_cell_class(e->doorx + pBx, e->doory + pBy) == FB_CELL_CORR);
		if (corrA == corrB)
			return FB_STOP; /* no unambiguous side to pivot toward */
		e->tdx = corrA ? pBx : pAx;
		e->tdy = corrA ? pBy : pAy;
		e->goalx = e->doorx + e->tdx; e->goaly = e->doory + e->tdy;
		return FB_KEEP;
	}

	if (door_cls == FB_CELL_CORR || goal_cls == FB_CELL_CORR) {
		/* still tracing the hallway's flank -- or mid-crossing a junction
		   that turned to run across our path */
		e->goalx = ngoalx; e->goaly = ngoaly;
		e->doorx = ndoorx; e->doory = ndoory;
		return FB_CONTINUE;
	}
	return FB_KEEP; /* both sides open, never invalid: flank ended one step ahead */
}

/* commit one resolved endpoint: a level-1 endpoint gets its original
   wall tile doored exactly as before; an escalated (level 2/3) endpoint
   gets its room-wall connector doored only if one doesn't already exist
   there, plus its final door square marked SCORR -- the single bridge
   between the pre-existing hallway (walked via its own ordinary CORR
   floor) and this endpoint's own dedicated corridor. */
STATIC_OVL void
fb_commit_endpoint(struct fb_endpoint *e)
{
	if (e->level == 1) {
		dosdoor(e->doorx, e->doory, &rooms[e->room], SDOOR);
	} else {
		if (e->need_door)
			dosdoor(e->wallx, e->wally, &rooms[e->room], SDOOR);
		levl[e->doorx][e->doory].typ = SCORR;
	}
}

/* if a level-2 pivot's goal square landed directly on a wall or door tile
   belonging to the room we're trying to reach (otherroom), the two rooms
   are already touching through open rock at this point -- connect them
   directly with a single SCORR bridge, skipping levels 3/line_path_clear
   entirely.  No terrain is written except on FB_HIT_DONE. */
STATIC_OVL int
fb_try_direct_hit(struct fb_endpoint *e, int otherroom)
{
	struct mkroom *r;
	int typ;

	if (!isok(e->goalx, e->goaly))
		return FB_HIT_NONE;
	typ = levl[e->goalx][e->goaly].typ;
	if (!IS_WALL(typ) && !IS_DOOR(typ) && typ != SDOOR)
		return FB_HIT_NONE;
	r = pos_to_room(e->goalx, e->goaly);
	if (!r || r != &rooms[otherroom])
		return FB_HIT_NONE;

	if (IS_WALL(typ) && typ != HWALL && typ != VWALL)
		return FB_HIT_CORNER; /* deferred: could nudge to an adjacent
			plain-wall tile instead of bailing outright */

	fb_commit_endpoint(e); /* W (per fb_level2's own need_door) + doorx/doory -> SCORR */

	if ((typ == HWALL || typ == VWALL) && okdoor(e->goalx, e->goaly))
		dosdoor(e->goalx, e->goaly, &rooms[otherroom], SDOOR);
	/* IS_DOOR/SDOOR, or okdoor() false because a door's already adjacent:
	   entry is already provided right here, nothing more to place. */

	smeq_union(e->room, otherroom);
	return FB_HIT_DONE;
}

/* validate and, only if fully valid, commit one candidate fallback
   corridor: a door slot pa on room a's wall_a and pb on room b's wall_b.
   Each endpoint independently escalates through up to three levels if
   its straight-line origin turns out to run alongside an existing
   primary hallway (level 1: nominate the obvious point; level 2: pivot
   away from the hallway and validate a connection back into it; level 3:
   walk along the hallway's flank looking for a clear vantage).  No
   terrain is written until the whole candidate is confirmed viable end
   to end, so a failed attempt never leaves partial state. */
STATIC_OVL boolean
hallway_fallback_attempt(int a, int b, int wall_a, int wall_b, int pa, int pb)
{
	boolean horiz_a = (wall_a == HALL_E || wall_a == HALL_W);
	boolean horiz_b = (wall_b == HALL_E || wall_b == HALL_W);
	xchar fa, loa, hia, fbb, lob, hib;
	xchar pathx[COLNO + ROWNO], pathy[COLNO + ROWNO];
	int pathlen = 0, i;
	struct fb_endpoint ea, eb;
	int ra, rb;
	boolean line_ok, any_continue;

	(void) door_slot_range(&rooms[a], wall_a, &fa, &loa, &hia);
	(void) door_slot_range(&rooms[b], wall_b, &fbb, &lob, &hib);

	ea.room = a; ea.level = 1;
	if (horiz_a) { ea.doorx = fa; ea.doory = pa; } else { ea.doorx = pa; ea.doory = fa; }
	ea.odoorx = ea.doorx; ea.odoory = ea.doory;
	ea.odx = halldx[wall_a]; ea.ody = halldy[wall_a];

	eb.room = b; eb.level = 1;
	if (horiz_b) { eb.doorx = fbb; eb.doory = pb; } else { eb.doorx = pb; eb.doory = fbb; }
	eb.odoorx = eb.doorx; eb.odoory = eb.doory;
	eb.odx = halldx[wall_b]; eb.ody = halldy[wall_b];

	if (!okdoor(ea.doorx, ea.doory) || !okdoor(eb.doorx, eb.doory))
		return FALSE;

	ra = fb_level1(&ea);
	rb = fb_level1(&eb);
	if (ra == FB_STOP || rb == FB_STOP)
		return FALSE;

	if (ra == FB_ESCALATE) {
		ra = fb_level2(&ea);
		if (ra == FB_STOP) return FALSE;
		switch (fb_try_direct_hit(&ea, b)) {
		case FB_HIT_DONE:   return TRUE;
		case FB_HIT_CORNER: return FALSE;
		default: break; /* FB_HIT_NONE -- fall through to normal escalation */
		}
	}
	if (rb == FB_ESCALATE) {
		rb = fb_level2(&eb);
		if (rb == FB_STOP) return FALSE;
		switch (fb_try_direct_hit(&eb, a)) {
		case FB_HIT_DONE:   return TRUE;
		case FB_HIT_CORNER: return FALSE;
		default: break;
		}
	}

	line_ok = line_path_clear(ea.goalx, ea.goaly, eb.goalx, eb.goaly, pathx, pathy, &pathlen);

	if (!line_ok && !(ea.level == 1 && eb.level == 1)) {
		for (;;) {
			any_continue = FALSE;
			if (ea.level >= 2 && ra != FB_KEEP) {
				ra = fb_level3_step(&ea, &eb);
				if (ra == FB_STOP) return FALSE;
				if (ra == FB_CONTINUE) any_continue = TRUE;
			}
			if (eb.level >= 2 && rb != FB_KEEP) {
				rb = fb_level3_step(&eb, &ea);
				if (rb == FB_STOP) return FALSE;
				if (rb == FB_CONTINUE) any_continue = TRUE;
			}
			if (!any_continue) break;
		}
		line_ok = line_path_clear(ea.goalx, ea.goaly, eb.goalx, eb.goaly, pathx, pathy, &pathlen);
	}
	if (!line_ok)
		return FALSE;

	fb_commit_endpoint(&ea);
	fb_commit_endpoint(&eb);
	for (i = 0; i < pathlen; i++)
		levl[pathx[i]][pathy[i]].typ = SCORR;

	smeq_union(a, b);
	return TRUE;
}

/* fallback for a room find_hallway()/carve_hallway() couldn't reach:
   try, nearest-to-farthest, every other room in a different connected
   component; for each, every axis that actually separates the two rooms
   (room_wall_pairs(), shorter gap first -- a diagonally-offset pair
   tries both), 100 random door-slot pairs on that pair of walls, then
   (if none worked) one exhaustive, unrandomized scan of every slot pair
   on those same two walls, before falling through to the other axis (if
   any).  Returns FALSE only once every candidate room and axis has been
   tried and failed. */
STATIC_OVL boolean
hallway_fallback(int a)
{
	int candidates[MAXNROFROOMS + 1][2]; /* [room b, distance] */
	int ncand = 0, i, b;

	for (b = 0; b < nroom; b++) {
		int wa, wb;

		if (b == a || smeq_root(a) == smeq_root(b)) continue;
		candidates[ncand][0] = b;
		candidates[ncand][1] = room_gap(&rooms[a], &rooms[b], &wa, &wb);
		ncand++;
	}

	/* insertion sort ascending by distance -- ncand is tiny (< nroom <= MAXNROFROOMS) */
	for (i = 1; i < ncand; i++) {
		int keyb = candidates[i][0], keyd = candidates[i][1], j = i - 1;

		while (j >= 0 && candidates[j][1] > keyd) {
			candidates[j + 1][0] = candidates[j][0];
			candidates[j + 1][1] = candidates[j][1];
			j--;
		}
		candidates[j + 1][0] = keyb;
		candidates[j + 1][1] = keyd;
	}

	for (i = 0; i < ncand; i++) {
		int b2 = candidates[i][0];
		int wall_from[4], wall_to[4], gap[4], npairs, wp;
		boolean success = FALSE;

		npairs = room_wall_pairs(&rooms[a], &rooms[b2], wall_from, wall_to, gap);

		for (wp = 0; wp < npairs && !success; wp++) {
			int wall_a = wall_from[wp], wall_b = wall_to[wp], t;
			xchar fa, loa, hia, fb, lob, hib;

			if (!door_slot_range(&rooms[a], wall_a, &fa, &loa, &hia)) continue;
			if (!door_slot_range(&rooms[b2], wall_b, &fb, &lob, &hib)) continue;

			for (t = 0; t < 100 && !success; t++) {
				int pa = loa + rn2(hia - loa + 1);
				int pb = lob + rn2(hib - lob + 1);

				success = hallway_fallback_attempt(a, b2, wall_a, wall_b, pa, pb);
			}
			if (!success) {
				int pa, pb;

				for (pa = loa; pa <= hia && !success; pa++)
					for (pb = lob; pb <= hib && !success; pb++)
						success = hallway_fallback_attempt(a, b2, wall_a, wall_b, pa, pb);
			}
		}
		if (success) return TRUE;
	}
	return FALSE;
}

/* replace a hallway's two flanking cells (the 3-wide cross-section's g-#-g
   sides; the center lane is left as plain corridor floor) with greenway
   terrain: rnd(50) of 1 -> FOUNTAIN (1/7 of those magic, per the same odds
   normal fountain placement uses), 2-16 -> TREE, 17-20 -> POOL, 21-30 ->
   PUDDLE, otherwise GRASS.  The whole 3-wide swath, center lane included,
   is lit.  Two passes: first
   paint the ENTIRE swath of every leg (center lane included, corner
   block included), then walk each leg's center lane back over and
   restore it to plain corridor floor.  ex/ey (the elbow's own pivot,
   unused when nlegs == 1) are what make pass 2 possible: each leg's own
   lo/hi was extended one step past the pivot purely so pass 1's
   perpendicular sweep would reach the corner block's outer edge (see
   hallway_legs()) -- that extended cell is real outer-edge flank, not
   part of the true single-file walking path, which actually bends AT the
   pivot.  So pass 2 must stop restoring each leg's center at ex (for the
   horizontal leg) / ey (for the vertical leg), one short of that leg's
   own extended endpoint, rather than walking its full lo..hi.  Same
   CORR/SCORR-only guard as hallway_paint_floor() on what pass 1 touches
   -- doors and any terrain this leg merely coasted through (belonging to
   some other hallway) have their terrain left alone, though still get
   lit; pass 2 only restores cells pass 1 actually painted
   (TREE/GRASS/FOUNTAIN), so it can't clobber anything pass 1 skipped.
   Must run before wallify_hallway_corridors() removes CORR/SCORR distinctions. */
STATIC_OVL void
greenway(int nlegs, xchar *lo, xchar *hi, xchar *fixed, boolean *horiz, xchar ex, xchar ey)
{
	int i, p, w, x, y, roll;

	for (i = 0; i < nlegs; i++)
		for (p = lo[i]; p <= hi[i]; p++)
			for (w = -1; w <= 1; w++) {
				x = horiz[i] ? p : fixed[i] + w;
				y = horiz[i] ? fixed[i] + w : p;
				if (!isok(x, y))
					continue;
				levl[x][y].lit = TRUE;
				if (levl[x][y].typ != CORR && levl[x][y].typ != SCORR)
					continue;

				roll = rnd(50);
				if (roll == 1) {
					levl[x][y].typ = FOUNTAIN;
					level.flags.nfountains++;
					if (!rn2(7))
						levl[x][y].blessedftn = 1;
				} else if (roll <= 16) {
					levl[x][y].typ = TREE;
				} else if(roll <= 20) {
					levl[x][y].typ = POOL;
				} else if(roll <= 30) {
					levl[x][y].typ = PUDDLE;
				} else {
					levl[x][y].typ = GRASS;
				}
			}

	for (i = 0; i < nlegs; i++) {
		int rlo = lo[i], rhi = hi[i];

		if (nlegs == 2) {
			int corner_p = horiz[i] ? ex : ey;

			if (corner_p == hi[i] - 1)
				rhi = corner_p;
			else if (corner_p == lo[i] + 1)
				rlo = corner_p;
		}

		for (p = rlo; p <= rhi; p++) {
			x = horiz[i] ? p : fixed[i];
			y = horiz[i] ? fixed[i] : p;
			if (!isok(x, y))
				continue;
			if (levl[x][y].typ == FOUNTAIN) {
				level.flags.nfountains--;
				levl[x][y].blessedftn = 0;
			}
			if (levl[x][y].typ == FOUNTAIN || levl[x][y].typ == TREE ||
					levl[x][y].typ == GRASS || levl[x][y].typ == POOL ||
					levl[x][y].typ == PUDDLE)
				levl[x][y].typ = CORR;
		}
	}
}

/* replace a straight, odd-length hallway's two flanking cells with an
   alternating tree / "other" pattern along its length (the center lane is
   left as plain corridor floor); both flanks at a given travel position
   always match, and every "other" cell is the same terrain, decided once
   for the whole hallway: rnd(100) of 1 -> FOUNTAIN, 2-50 -> GRASS, 51-100
   -> CORR (left as corridor floor; wallify_hallway_corridors() upgrades it to
   room floor later, same as any other corridor cell).  Position lo (the
   near end) is always a tree, so a length-1 hallway is solid trees and a
   length-3 hallway goes tree-other-tree.  The whole 3-wide swath, center
   lane included, is lit.  Same CORR/SCORR-only guard on terrain
   replacement as greenway()/hallway_paint_floor().  Caller is responsible
   for confirming the hallway is actually straight and odd-length before
   calling -- unlike greenway(), this doesn't take a leg array, since an
   elbow wouldn't have a single well-defined alternation to run. */
STATIC_OVL void
tree_lined(int lo, int hi, int fixed, boolean horiz)
{
	int p, w, x, y, roll, other;

	roll = rnd(100);
	other = (roll == 1) ? FOUNTAIN : (roll < 51) ? GRASS : CORR;

	for (p = lo; p <= hi; p++) {
		boolean is_tree = !((p - lo) % 2);

		for (w = -1; w <= 1; w++) {
			x = horiz ? p : fixed + w;
			y = horiz ? fixed + w : p;
			if (!isok(x, y))
				continue;
			levl[x][y].lit = TRUE;
			if (!w || (levl[x][y].typ != CORR && levl[x][y].typ != SCORR))
				continue;
			if (is_tree) {
				levl[x][y].typ = TREE;
			} else {
				levl[x][y].typ = other;
				if (other == FOUNTAIN) level.flags.nfountains++;
			}
		}
	}
}

/* replace a straight, odd-length hallway's two flanking cells with an
   alternating statue / plain-corridor pattern along its length (the
   center lane is always left as plain corridor floor; unlike
   tree_lined() there's no distinct "other" terrain, so the alternate
   position is just left as plain corridor floor too).  Position lo (the
   near end) always gets a statue, so a length-1 hallway is solid statues
   and a length-3 hallway goes statue-none-statue.  Each statue's monster
   is rndshape()'d the same way mksgardenstatueat()'s decorative statues
   are, with a 5% chance to be gold (set_material_gm(), otherwise left the
   default stone).  The whole 3-wide swath, center lane included, is lit.
   Same CORR/SCORR-only guard as tree_lined()/greenway().  Caller is
   responsible for confirming the hallway is actually straight and
   odd-length before calling. */
STATIC_OVL void
statue_lined(int lo, int hi, int fixed, boolean horiz)
{
	int p, w, x, y;
	struct obj *otmp;
	boolean gold_statues = !rn2(20);
	boolean alltraps = !rn2(100);
	struct trap *t;

	for (p = lo; p <= hi; p++) {
		boolean is_statue = !((p - lo) % 2);

		for (w = -1; w <= 1; w++) {
			x = horiz ? p : fixed + w;
			y = horiz ? fixed + w : p;
			if (!isok(x, y))
				continue;
			levl[x][y].lit = TRUE;
			if (!w || !is_statue ||
					(levl[x][y].typ != CORR && levl[x][y].typ != SCORR))
				continue;

			otmp = mksobj_at(STATUE, x, y, MKOBJ_ARTIF);
			if (otmp) {
				t = t_at(x, y);
				otmp->corpsenm = rndshape((void *) 0);
				if (gold_statues)
					set_material_gm(otmp, GOLD);
				else if(!t && 
					(alltraps || (depth(&u.uz) - monstr[otmp->corpsenm]) >= rn2(100))
				) {
					t = maketrap(x, y, MAGIC_TRAP);
					if (t) {
						t->ttyp = STATUE_TRAP;
						t->statueid = otmp->o_id;
					}
				}
				fix_object(otmp);
			}
		}
	}
}


STATIC_OVL void
hallway_of_the_damned(void)
{
	//Stub
}

/* alternate to makecorridors(): 3-tile-wide hallways, at most one turn.
   Pass zero unions any room pair already touching or overlapping, so
   smeq[] accounts for them before any hallway is placed.  Pass 1
   records a valid path (if any) for every room pair; pass 2 joins rooms
   using only those pre-cleared pairs, picking one random cross-
   component partner per room per sweep, until a full sweep makes no
   more merges.  Pass two and a half falls back to a secret, 1-wide
   corridor for any room pair pass 1/2 couldn't connect.  Pass three
   classifies each carved hallway's traits (straight/elbow, odd length,
   padded walls, long run); pass four uses those traits to dress some
   hallways with greenway/tree/statue terrain.  Wallifying the carved
   corridors into a built-hallway look happens separately, in the
   caller (see wallify_hallway_corridors()). */
STATIC_OVL void
makehallways(void)
{
	int a, b;
	schar hallelbow[MAXNROFROOMS + 1][MAXNROFROOMS + 1][3] = { { { 0, 0, 0 } } }; /* x, y, placed */
	xchar ex, ey;
	boolean any;
	int corridor_count = 0;

	/* Pass zero: rooms placed already touching or overlapping (e.g. via
	   flags.makelev_closerooms, true for most levels -- see mklev.c's
	   makelevel()) share open floor with no corridor ever carved between
	   them.  merge_adj_rooms() only catches an exact 3-tile gap, so
	   smeq[] never learns about a pair placed this close any other way
	   -- union them here so every smeq-based check below (and inside
	   this function) sees them as already connected. */
	for (a = 0; a < nroom; a++)
		for (b = a + 1; b < nroom; b++) {
			int wa, wb;

			if (room_gap(&rooms[a], &rooms[b], &wa, &wb) == 0)
				smeq_union(a, b);
		}

	/* Pass 1: find a valid hallway for every room pair, if any */
	for (a = 0; a < nroom; a++)
		for (b = a + 1; b < nroom; b++) {
			if (find_hallway(a, b, &ex, &ey)) {
				hallelbow[a][b][0] = hallelbow[b][a][0] = ex;
				hallelbow[a][b][1] = hallelbow[b][a][1] = ey;
			} else {
				hallelbow[a][b][0] = hallelbow[b][a][0] = -1;
			}
		}

	/* Pass 2: join rooms using the pre-cleared pairs */
	do {
		any = FALSE;
		for (a = 0; a < nroom; a++) {
			int chcnt = 0, bestb = -1;

			for (b = 0; b < nroom; b++) {
				if (a == b) continue;
				if (hallelbow[a][b][0] == -1) continue;
				if (smeq_root(a) == smeq_root(b)) continue;
				if (!rn2(++chcnt)) bestb = b;
			}
			if (bestb != -1) {
				carve_hallway(a, bestb, hallelbow[a][bestb][0], hallelbow[a][bestb][1]);
				any = TRUE;
				corridor_count++;
				hallelbow[a][bestb][2] = hallelbow[bestb][a][2] = TRUE;
			}
		}
	} while (any);
	/* Pass two and a half: fall back to a secret straight corridor for any rooms still disconnected */
	while (!all_rooms_connected()) {
		boolean fb_any = FALSE;

		for (a = 0; a < nroom; a++) {
			if (all_rooms_connected()) break;
			if (hallway_fallback(a))
				fb_any = TRUE;
		}
		if (!fb_any && !all_rooms_connected()){
			impossible("makehallways: could not connect all rooms");
			iflags.cut_level_gen_test = TRUE;
			break;
		}
	}
	/* Pass three: pick out some special hallways */
	int corridor_traits[corridor_count][5]; /* [x, y, traits, a, b] */
	int ncorridor_traits = 0;
	(void) memset((genericptr_t) corridor_traits, 0, sizeof(corridor_traits));
#define INTERSECTED 0
#define NINE_CLEAR 0x01
#define STRAIGHT 0x02
#define ELBOW 0x04
#define ODD 0x08
#define LONG_RUN 0x10
	{
		int c, d, i, j, n = 0;

		for (a = 0; a < nroom; a++){
			for (b = a + 1; b < nroom; b++) {
				xchar pax, pay, pbx, pby;
				schar dxa, dya, dxb, dyb;
				boolean straight, intersected;
				xchar lo[2], hi[2], fixed[2];
				boolean horiz[2];
				int nlegs, traits, eligible, padded_cnt;

				if (!hallelbow[a][b][2]) continue;
				ex = hallelbow[a][b][0];
				ey = hallelbow[a][b][1];

				hallway_endpoint(a, ex, ey, &pax, &pay, &dxa, &dya);
				hallway_endpoint(b, ex, ey, &pbx, &pby, &dxb, &dyb);
				straight = ((dya == 0) == (dyb == 0));
				nlegs = hallway_legs(a, b, ex, ey, lo, hi, fixed, horiz);

				/* did any OTHER placed hallway's carve physically merge
				   with this one anywhere along its length?  Only
				   answerable now that every hallway is carved -- skip
				   examining every other trait if so, and give this
				   hallway no output entry at all. */
				intersected = FALSE;
				for (c = 0; c < nroom && !intersected; c++)
					for (d = c + 1; d < nroom && !intersected; d++) {
						xchar olo[2], ohi[2], ofixed[2];
						boolean ohoriz[2];
						int onlegs;

						if (c == a && d == b) continue;
						if (!hallelbow[c][d][2]) continue;
						onlegs = hallway_legs(c, d, hallelbow[c][d][0],
									hallelbow[c][d][1],
									olo, ohi, ofixed, ohoriz);
						for (i = 0; i < nlegs && !intersected; i++)
							for (j = 0; j < onlegs; j++)
								if (hallway_legs_overlap(lo[i], hi[i], fixed[i], horiz[i],
											olo[j], ohi[j], ofixed[j], ohoriz[j])) {
									intersected = TRUE;
									break;
								}
					}
				if (intersected) continue;

				traits = straight ? STRAIGHT : ELBOW;
				{
					int dist = abs(pax - pbx) + abs(pay - pby);

					/* ODD means an odd number of squares BETWEEN the two
					   doors, i.e. (Manhattan distance - 1) is odd, i.e.
					   the distance itself is even */
					if (!(dist & 1))
						traits |= ODD;
					if (dist >= 10)
						traits |= LONG_RUN;
				}

				eligible = padded_cnt = 0;
				for (i = 0; i < nlegs; i++) {
					int p, side;

					for (p = lo[i]; p <= hi[i]; p++)
						for (side = -1; side <= 1; side += 2) {
							boolean was_padded;

							if (!hallway_wall_square_stat(p, fixed[i], horiz[i], side, &was_padded))
								continue;
							eligible++;
							if (was_padded) padded_cnt++;
						}
				}
				if (eligible > 0 && padded_cnt * 2 >= eligible)
					traits |= NINE_CLEAR;

				corridor_traits[n][0] = ex;
				corridor_traits[n][1] = ey;
				corridor_traits[n][2] = traits;
				corridor_traits[n][3] = a;
				corridor_traits[n][4] = b;
				n++;

			}
		}
		ncorridor_traits = n;
	}

	/* pass four: decide what, if anything, each surviving hallway (every
	   row corridor_traits picked up above) becomes.  Kept separate from
	   pass three above so a future dispatch can weigh one hallway's
	   choice against every other hallway's already-computed traits, not
	   just its own -- corridor_traits is fully populated by the time this
	   runs.  Only ex/ey/traits/a/b need to survive from pass three;
	   everything else (nlegs/lo/hi/fixed/horiz) is cheap to recompute
	   from a/b/ex/ey via hallway_legs(). */
	{
		int k;

		for (k = 0; k < ncorridor_traits; k++) {
			xchar lo[2], hi[2], fixed[2];
			boolean horiz[2];
			int nlegs, traits, ca, cb;

			ex = corridor_traits[k][0];
			ey = corridor_traits[k][1];
			traits = corridor_traits[k][2];
			ca = corridor_traits[k][3];
			cb = corridor_traits[k][4];
			nlegs = hallway_legs(ca, cb, ex, ey, lo, hi, fixed, horiz);

			if ((traits & (STRAIGHT | ODD)) == (STRAIGHT | ODD)){
				if(!rn2(4)){
					statue_lined(lo[0], hi[0], fixed[0], horiz[0]);
				}
				else if(!rn2(3)){
					tree_lined(lo[0], hi[0], fixed[0], horiz[0]);
				}
				else if(!rn2(2)){
					greenway(nlegs, lo, hi, fixed, horiz, ex, ey);
				}
			}
			else {
				if(!rn2(4)){
					greenway(nlegs, lo, hi, fixed, horiz, ex, ey);
				}
				else if(!rn2(10)){
					hallway_paint_floor(nlegs, lo, hi, fixed, horiz, ICE);
				}
			}
		}
	}
}


/*
ALI - Artifact doors: Track doors in maze levels as well.  From Slash'em
*/

int
add_door(x,y,aroom)
register int x, y;
register struct mkroom *aroom;
{
	register struct mkroom *broom;
	register int tmp;

	if (doorindex == DOORMAX)
	    return -1;

	if (aroom) {
	    aroom->doorct++;
	    broom = aroom+1;
	} else
	    /* ALI
	     * Roomless doors must go right at the beginning of the list
	     */
	    broom = &rooms[0];
	if(broom->hx < 0)
	    tmp = doorindex;
	else
		for(tmp = doorindex; tmp > broom->fdoor; tmp--)
			doors[tmp] = doors[tmp-1];
	doorindex++;
	doors[tmp].x = x;
	doors[tmp].y = y;
	for( ; broom->hx >= 0; broom++) broom->fdoor++;
	doors[tmp].arti_text = 0;
	return tmp;
}

void
dosdoor(x,y,aroom,type)
register xchar x, y;
register struct mkroom *aroom;
register int type;
{
	boolean shdoor = ((*in_rooms(x, y, SHOPBASE))? TRUE : FALSE);

	if(!IS_WALL(levl[x][y].typ)) /* avoid SDOORs on already made doors */
		type = DOOR;
	levl[x][y].typ = type;
	if(type == DOOR) {
	    if(!rn2(3)) {      /* is it a locked door, closed, or a doorway? */
		if(!rn2(5))
		    levl[x][y].doormask = D_ISOPEN;
		else if(!rn2(6))
		    levl[x][y].doormask = D_LOCKED;
		else
		    levl[x][y].doormask = D_CLOSED;

		if (levl[x][y].doormask != D_ISOPEN && !shdoor &&
		    level_difficulty() >= 5 && !rn2(25))
		    levl[x][y].doormask |= D_TRAPPED;
	    } else
#ifdef STUPID
		if (shdoor)
			levl[x][y].doormask = D_ISOPEN;
		else
			levl[x][y].doormask = D_NODOOR;
#else
		levl[x][y].doormask = (shdoor ? D_ISOPEN : D_NODOOR);
#endif
	    if(levl[x][y].doormask & D_TRAPPED) {
		struct monst *mtmp;

		if (level_difficulty() >= 9 && !rn2(5) &&
		   !((mvitals[PM_SMALL_MIMIC].mvflags & G_GONE && !In_quest(&u.uz)) &&
		     (mvitals[PM_LARGE_MIMIC].mvflags & G_GONE && !In_quest(&u.uz)) &&
		     (mvitals[PM_GIANT_MIMIC].mvflags & G_GONE && !In_quest(&u.uz)))) {
		    /* make a mimic instead */
		    levl[x][y].doormask = D_NODOOR;
		    mtmp = makemon(mkclass(S_MIMIC, Inhell ? G_HELL : G_NOHELL), x, y, NO_MM_FLAGS);
		    if (mtmp)
			set_mimic_sym(mtmp);
		}
	    }
	    /* newsym(x,y); */
	} else { /* SDOOR */
		if(shdoor || !rn2(5))	levl[x][y].doormask = D_LOCKED;
		else			levl[x][y].doormask = D_CLOSED;

		if(!shdoor && level_difficulty() >= 4 && !rn2(20))
		    levl[x][y].doormask |= D_TRAPPED;
	}

	add_door(x,y,aroom);
}

void
add_altar(x, y, alignment, shrine, godnum)
int x, y;
aligntyp alignment;
boolean shrine;
int godnum;
{
	if (altarindex == ALTARMAX) {
		impossible("Max altar reached!");
	    return;
	}

	levl[x][y].typ = ALTAR;
	levl[x][y].altar_num = altarindex;

	altars[altarindex].x = x;
	altars[altarindex].y = y;
	altars[altarindex].align = alignment;
	altars[altarindex].shrine = shrine;
	altars[altarindex].god = godnum;

	if(!In_endgame(&u.uz) && !In_quest(&u.uz)){
		if(godnum == GOD_THE_COLLEGE || (godnum == GOD_NONE && align_to_god(alignment) == GOD_THE_COLLEGE)){
			mksobj_at(rn2(2) ? PORTABLE_ELECTRODE : BELL, x, y, NO_MKOBJ_FLAGS);
		}
		else if(godnum == GOD_THE_CHOIR || (godnum == GOD_NONE && align_to_god(alignment) == GOD_THE_CHOIR)){
			mksobj_at(TREPHINATION_KIT, x, y, NO_MKOBJ_FLAGS);
		}
		else if(godnum == GOD_DEFILEMENT || (godnum == GOD_NONE && align_to_god(alignment) == GOD_DEFILEMENT)){
			mksobj_at(PHLEBOTOMY_KIT, x, y, NO_MKOBJ_FLAGS);
		}
	}
	if (Is_qlocate(&u.uz) && Role_if(PM_ARCHEOLOGIST)) {
		altars[altarindex].god = rn2(2) ? GOD_XOLOTL : GOD_TLALOC;
		altars[altarindex].align = galign(altars[altarindex].god);
	}
	
	altarindex++;
}

STATIC_OVL boolean
place_niche(aroom,dy,xx,yy)
register struct mkroom *aroom;
int *dy, *xx, *yy;
{
	coord dd;

	if(rn2(2)) {
	    *dy = 1;
	    finddpos(&dd, aroom->lx, aroom->hy+1, aroom->hx, aroom->hy+1);
	} else {
	    *dy = -1;
	    finddpos(&dd, aroom->lx, aroom->ly-1, aroom->hx, aroom->ly-1);
	}
	*xx = dd.x;
	*yy = dd.y;
	return((boolean)((isok(*xx,*yy+*dy) && levl[*xx][*yy+*dy].typ == STONE)
	    && (isok(*xx,*yy-*dy) && !IS_POOL(levl[*xx][*yy-*dy].typ)
				  && !IS_FURNITURE(levl[*xx][*yy-*dy].typ))));
}

/* there should be one of these per trap, in the same order as trap.h */
static NEARDATA const char *trap_engravings[TRAPNUM] = {
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0, (char *)0, (char *)0, (char *)0,
			/* 14..16: trap door, teleport, level-teleport */
			"Vlad was here", "ad aerarium", "ad aerarium",
			(char *)0, (char *)0, (char *)0, (char *)0, (char *)0,
			(char *)0,(char *)0,
};

STATIC_OVL void
makeniche(trap_type)
int trap_type;
{
	register struct mkroom *aroom;
	register struct rm *rm;
	register int vct = 8;
	int dy, xx, yy;
	register struct trap *ttmp;

	if(doorindex < DOORMAX)
	  while(vct--) {
	    aroom = &rooms[rn2(nroom)];
	    if(aroom->rtype != OROOM && aroom->rtype != JOINEDROOM) continue;	/* not an ordinary room */
	    if(aroom->doorct == 1 && rn2(5)) continue;
	    if(!place_niche(aroom,&dy,&xx,&yy)) continue;

	    rm = &levl[xx][yy+dy];
	    if(trap_type || !rn2(4)) {

		rm->typ = SCORR;
		if(trap_type) {
		    if((trap_type == HOLE || trap_type == TRAPDOOR)
			&& !Can_fall_thru(&u.uz))
			trap_type = ROCKTRAP;
		    ttmp = maketrap(xx, yy+dy, trap_type);
		    if (ttmp) {
			if (trap_type != ROCKTRAP) ttmp->once = 1;
			if (trap_engravings[trap_type]) {
			    make_engr_at(xx, yy-dy,
				     trap_engravings[trap_type], 0L, DUST);
			    wipe_engr_at(xx, yy-dy, 5); /* age it a little */
			}
		    }
		}
		dosdoor(xx, yy, aroom, SDOOR);
	    } else {
		rm->typ = CORR;
		if(rn2(7))
		    dosdoor(xx, yy, aroom, rn2(5) ? SDOOR : DOOR);
		else {
		    if (!level.flags.noteleport)
			(void) mksobj_at(SCR_TELEPORTATION, xx, yy+dy, NO_MKOBJ_FLAGS);
		    if (!rn2(3)) (void) mkobj_at(0, xx, yy+dy, MKOBJ_ARTIF);
		}
	    }
			if(In_mithardir_catacombs(&u.uz)){
				if(!rn2(2))
					makemon(&mons[PM_ALABASTER_MUMMY], xx, yy+dy,NO_MM_FLAGS);
				else
					mkobj_at(TILE_CLASS, xx, yy+dy, MKOBJ_ARTIF);
			}
	    return;
	}
}

STATIC_OVL void
make_niches()
{
	register int ct, dep;
	if(In_mithardir_catacombs(&u.uz)){
		ct = 6+rn2(3);
		while(ct--)
			makeniche(NO_TRAP);
	} else {
		ct = rnd((nroom>>1) + 1), dep = depth(&u.uz);
	
		boolean	ltptr = (!level.flags.noteleport && dep > 15),
			vamp = (dep > 5 && dep < 25);
	
		while(ct--) {
			if (ltptr && !rn2(6)) {
				ltptr = FALSE;
				makeniche(LEVEL_TELEP);
			} else if (vamp && !rn2(6)) {
				vamp = FALSE;
				makeniche(TRAPDOOR);
			} else	makeniche(NO_TRAP);
		}
	}
}

STATIC_OVL void
makevtele()
{
	makeniche(TELEP_TRAP);
}

int
random_special_room()
{
	int total_prob = 0;
	int i = 0;

	struct {
		int type;
		int prob;
	} special_rooms[MAXRTYPE] = {0};

#define mnotgone(x) !(mvitals[(x)].mvflags & G_GONE && !In_quest(&u.uz))
#define add_rspec_room(t, p, c) if(c) {special_rooms[i].type = (t); special_rooms[i].prob = (p); total_prob += (p); i++;} else
#define udepth depth(&u.uz)

	/* -------- GEHENNOM -------- */
	if (In_hell(&u.uz))
	{
		if (Is_bael_level(&u.uz)){
			/* BAEL */
			add_rspec_room(TEMPLE		,  1, !level.flags.has_temple);
			add_rspec_room(POOLROOM		, 15, TRUE);
			add_rspec_room(BARRACKS		, 50, mnotgone(PM_LEGION_DEVIL_GRUNT));
			add_rspec_room(0			, 50, TRUE);
		} else if (Is_orcus_level(&u.uz)){
			/* ORCUS */
			add_rspec_room(MORGUE		, 95, TRUE);
			add_rspec_room(0			, 50, TRUE);
		} else{
			/* STANDARD GEHENNOM */
			add_rspec_room(COURT		, 18, TRUE);
			add_rspec_room(COCKNEST		, 10, mnotgone(PM_COCKATRICE));
			add_rspec_room(POOLROOM		, 22, TRUE);
			add_rspec_room(BARRACKS		, 18, mnotgone(PM_LEGION_DEVIL_GRUNT));
			add_rspec_room(MORGUE		, 32, TRUE);
			add_rspec_room(LEPREHALL	,  8, mnotgone(PM_LEPRECHAUN));
			add_rspec_room(STATUEGRDN	,  2, TRUE);
			add_rspec_room(TEMPLE		,  5, !level.flags.has_temple);
			add_rspec_room(SHOPBASE		,  1, !rn2(3));
			add_rspec_room(HELL_VAULT	,100, !level.flags.has_vault);
			add_rspec_room(0			, 50, TRUE);
		}
	}
	/* -------- NEUTRAL QUEST OUTLANDS -------- */
	else if (In_outlands(&u.uz))
	{
		add_rspec_room(COURT			, 20, TRUE);
		add_rspec_room(BARRACKS			, 40, mnotgone(PM_FERRUMACH_RILMANI));
		add_rspec_room(0				, 50, TRUE);
	}
	/* -------- ROLE QUESTS -------- */
	else if (In_quest(&u.uz))
	{
		/* INCREDIBLY INCOMPLETE*/
		switch (Role_switch)
		{
		case PM_ANACHRONONAUT:
			add_rspec_room(0			, 50, TRUE);
			break;
		default:
			goto random_special_room_default_room_and_corridors;	/* <insert sad face> */
		}
	}
	/* -------- DEFAULT -------- */
	else
	{
		if (level.flags.is_maze_lev){
			/* MAZE */
			add_rspec_room(COURT		, 15, udepth >  4 && !Is_stronghold(&u.uz));
			add_rspec_room(COCKNEST		,  9, udepth > 16 && mnotgone(PM_COCKATRICE));
			add_rspec_room(POOLROOM		, 30, udepth > 15);
			add_rspec_room(BARRACKS		, 18, udepth > 14 && mnotgone(PM_SOLDIER));
			add_rspec_room(ARMORY		,  8, udepth <=14 && udepth > 1 && mnotgone(PM_RUST_MONSTER));
			add_rspec_room(MORGUE		, 10, udepth > 11);
			add_rspec_room(LEPREHALL	, 10, udepth >  4 && mnotgone(PM_LEPRECHAUN));
			add_rspec_room(STATUEGRDN	,  2, udepth >  2);
			add_rspec_room(TEMPLE		,  5, !level.flags.has_temple);
			add_rspec_room(SHOPBASE		,  5, TRUE);
			add_rspec_room(0			, 50, TRUE);
		} else if(level.flags.is_cavernous_lev){
		} else {
			/* ROOM-AND-CORRIDORS */
random_special_room_default_room_and_corridors:
			/* temples and shops are assumed to be generated separately from this case */
			add_rspec_room(COURT		, 21, udepth >  4);
			add_rspec_room(COCKNEST		, 16, udepth > 16 && mnotgone(PM_COCKATRICE));
			add_rspec_room(POOLROOM		, 15, udepth > 15);
			add_rspec_room(BARRACKS		, 17, udepth > 14 && mnotgone(PM_SOLDIER));
			add_rspec_room(ARMORY		,  8, udepth <=14 && udepth > 1 && mnotgone(PM_RUST_MONSTER));
			add_rspec_room(ANTHOLE		, 14, udepth > 12);
			add_rspec_room(MORGUE		, 18, udepth > 11);
			add_rspec_room(BEEHIVE		,  8, udepth >  9 && mnotgone(PM_KILLER_BEE));
			add_rspec_room(LIBRARY		,  6, udepth >  8);
			add_rspec_room(GARDEN		, 13, udepth >  7);
			add_rspec_room(ZOO       	, 12, udepth >  6);
			add_rspec_room(LEPREHALL	, 10, udepth >  4 && mnotgone(PM_LEPRECHAUN));
			add_rspec_room(STATUEGRDN	,  2, udepth >  2);
			add_rspec_room(0			, 50, TRUE);
		}
	}
#undef udepth
#undef add_rspec_room
#undef mnotgone

	/* pick a room */
	if (total_prob > 0)
		total_prob = rnd(total_prob);
	while (total_prob > 0 && i-- > 0)
		total_prob -= special_rooms[i].prob;

	if (i >= 0)
		return special_rooms[i].type;
	else
		return 0;	// should never happen?
}

/* clear out various globals that keep information on the current level.
 * some of this is only necessary for some types of levels (maze, normal,
 * special) but it's easier to put it all in one place than make sure
 * each type initializes what it needs to separately.
 */
STATIC_OVL void
clear_level_structures()
{
	static struct rm zerorm = { cmap_to_glyph(S_stone),
						0, 0, 0, 0, 0, 0, 0, 0 };
	register int x,y;
	register struct rm *lev;

	for(x=0; x<COLNO; x++) {
	    lev = &levl[x][0];
	    for(y=0; y<ROWNO; y++) {
		*lev++ = zerorm;
#ifdef MICROPORT_BUG
		level.objects[x][y] = (struct obj *)0;
		level.monsters[x][y] = (struct monst *)0;
#endif
	    }
	}
#ifndef MICROPORT_BUG
	(void) memset((genericptr_t)level.objects, 0, sizeof(level.objects));
	(void) memset((genericptr_t)level.monsters, 0, sizeof(level.monsters));
#endif
	level.objlist = (struct obj *)0;
	level.buriedobjlist = (struct obj *)0;
	level.monlist = (struct monst *)0;
	level.damagelist = (struct damage *)0;

	level.flags.nfountains = 0;
	level.flags.nforges = 0;
	level.flags.nsinks = 0;
	
	level.flags.goldkamcount_hostile = 0;
	level.flags.goldkamcount_peace = 0;
	
	level.flags.sp_lev_nroom = 0;
	level.flags.rage = 0;
	
	level.flags.has_shop = 0;
	level.flags.has_vault = 0;
	level.flags.has_zoo = 0;
	level.flags.has_court = 0;
	level.flags.has_morgue = level.flags.graveyard = 0;
	level.flags.has_beehive = 0;
	level.flags.has_armory = 0;
	level.flags.has_barracks = 0;
	level.flags.has_temple = 0;
	level.flags.has_swamp = 0;
	level.flags.has_garden = 0;
	level.flags.has_library = 0;
	level.flags.has_island = 0;
	level.flags.has_river = 0;
	level.flags.noteleport = 0;
	level.flags.hardfloor = 0;
	level.flags.nommap = 0;
	level.flags.hero_memory = 1;
	level.flags.shortsighted = 0;
	level.flags.arboreal = 0;
	level.flags.is_maze_lev = 0;
	level.flags.is_cavernous_lev = 0;
	level.flags.lethe = 0;
	
	/* Not currently used */
	level.flags.slime = 0;
	level.flags.fungi = 0;
	level.flags.dun = 0;
	level.flags.necro = 0;
	
	level.flags.cave = 0;
	level.flags.outside = 0;
	level.flags.has_minor_spire = 0;
	level.flags.has_kamerel_towers = 0;
	
	level.flags.mirror = 0;
	level.flags.day = 0;
	level.flags.walkers = 0;
	
	level.lastmove = monstermoves;

	/* rooms[] (and subrooms[], which aliases its back half) are global
	   and reused across levels, so a freshly generated room can otherwise
	   inherit a stale resident pointer left behind by whatever level
	   previously occupied that array slot */
	(void) memset((genericptr_t)rooms, 0, sizeof(rooms));
	nroom = 0;
	rooms[0].hx = -1;
	nsubroom = 0;
	subrooms[0].hx = -1;
	doorindex = 0;
	altarindex = 0;
	init_rect();
	init_vault();
	xdnstair = ydnstair = xupstair = yupstair = 0;
	sstairs.sx = sstairs.sy = sstairs.u_traversed = 0;
	xdnladder = ydnladder = xupladder = yupladder = 0;
	made_branch = FALSE;
	clear_regions();
}

STATIC_OVL void
special_mklev_actions()
{
	if(Is_qhome(&u.uz)){
		if(Race_if(PM_SILVERKNIGHT)){
			struct monst *mount = makedog();
			if(mount){
				for(int i = 0; i < 2; i++){
					grow_up(mount, (struct monst *)0);
					//Might grow into a genocided form.
					if(DEADMONSTER(mount))
						return;
				}
			}
		}
	}
}

STATIC_OVL void
makelevel()
{
	register struct mkroom *croom, *troom;
	register int tryct;
	register int x, y;
	struct monst *tmonst;	/* always put a web with a spider */
	branch *branchp;
	int room_threshold;
	boolean magic_chest = FALSE;
	boolean hallways = FALSE;

	if(wiz1_level.dlevel == 0) init_dungeons();
	oinit();	/* assign level dependent obj probabilities */
	clear_level_structures();
	flags.makelev_closerooms = FALSE;
	if(Infuture)
		level.lastmove = quest_status.time_doing_quest;
	if(Is_minetown_level(&u.uz)) livelog_write_string("entered Minetown for the first time");

	{
	    register s_level *slev = Is_special(&u.uz);

	    /* Kensei quest subout */
		if(Role_if(PM_KENSEI) && qstart_level.dnum == u.uz.dnum && qlocate_level.dlevel == u.uz.dlevel){
		    char	fillname[9];
			if(u.role_variant == ART_SKY_REFLECTED)
				Sprintf(fillname, "%s-locb", urole.filecode);
			else if(u.role_variant == ART_SILVER_SKY)
				Sprintf(fillname, "%s-locc", urole.filecode);
			else if(u.role_variant == ART_ANGUIREL)
				Sprintf(fillname, "%s-locd", urole.filecode);
			else if(u.role_variant == ART_RINGIL)
				Sprintf(fillname, "%s-loce", urole.filecode);
			else if(u.role_variant == ART_ANSERMEE)
				Sprintf(fillname, "%s-locf", urole.filecode);
			else if(u.role_variant == ART_EPITAPH_OF_WONGAS)
				Sprintf(fillname, "%s-locg", urole.filecode);
			else if(u.role_variant == ART_WINTER_REAPER)
				Sprintf(fillname, "%s-loch", urole.filecode);
			else if(u.role_variant == ART_BOREAL_SCEPTER)
				Sprintf(fillname, "%s-loci", urole.filecode);
			else if(u.role_variant == ART_MALICE)
				Sprintf(fillname, "%s-locj", urole.filecode);
			else if(u.role_variant == ART_KIKU_ICHIMONJI)
				Sprintf(fillname, "%s-lock", urole.filecode);
			else 
				Sprintf(fillname, "%s-loca", urole.filecode);
			makemaz(fillname);
		    return;
		}
	    /* check for special levels */
	    if (slev && !Is_rogue_level(&u.uz))
	    {
		    makemaz(slev->proto);
		    return;
	    } else if (dungeons[u.uz.dnum].proto[0]) {
		    makemaz("");
		    return;
	    } else if (In_mines(&u.uz)) {
		    makemaz("minefill");
		    return;
	    } else if (In_quest(&u.uz)) {
		    char	fillname[9];
		    s_level	*loc_lev;

			if(Role_if(PM_NOBLEMAN) && Race_if(PM_HALF_DRAGON) && flags.initgend && qstart_level.dnum == u.uz.dnum && qstart_level.dlevel == (u.uz.dlevel-1)){
				Sprintf(fillname, "%s-home", urole.filecode);
				// pline("%s",fillname);
				makemaz(fillname);
			}
			else if(Role_if(PM_MADMAN) && qstart_level.dnum == u.uz.dnum && qlocate_level.dlevel == (u.uz.dlevel+1)){
				Sprintf(fillname, "%s-home", urole.filecode);
				// pline("%s",fillname);
				makemaz(fillname);
			}
			else if(urole.neminum == PM_BLIBDOOLPOOLP__GRAVEN_INTO_FLESH && qstart_level.dnum == u.uz.dnum && qstart_level.dlevel == (u.uz.dlevel-1)){
				Sprintf(fillname, "%s-secn", urole.filecode);
				// pline("%s",fillname);
				makemaz(fillname);
			}
			else if(urole.neminum == PM_BLIBDOOLPOOLP__GRAVEN_INTO_FLESH && qstart_level.dnum == u.uz.dnum && qlocate_level.dlevel == (u.uz.dlevel-1)){
				Sprintf(fillname, "%s-flor", urole.filecode);
				// pline("%s",fillname);
				makemaz(fillname);
			}
			else {
			    Sprintf(fillname, "%s-loca", urole.filecode);
			    loc_lev = find_level(fillname);
	
			    Sprintf(fillname, "%s-fil", urole.filecode);
			    Strcat(fillname,
				   (u.uz.dlevel < loc_lev->dlevel.dlevel) ? "a" : "b");
			    makemaz(fillname);
			}
		    return;
	    } else if(In_hell(&u.uz) ||
		  (rn2(5) && u.uz.dnum == challenge_level.dnum
			  && depth(&u.uz) > depth(&challenge_level))) {
		    makemaz("");
		    return;
	    }
	}

	/* otherwise, fall through - it's a "regular" level. */

#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) {
		makeroguerooms();
		makerogueghost();
	} else
#endif
	{
	/* probably use the 'claustrophobic' room generation, since we aren't doing a special level */
	if (In_mithardir_catacombs(&u.uz) || rn2(7))
		flags.makelev_closerooms = TRUE;
	makerooms();
	}
	sort_rooms();

	/* construct stairs (up and down in different rooms if possible) */
	croom = &rooms[rn2(nroom)];
	if (!Is_botlevel(&u.uz))
	     mkstairs(somex(croom), somey(croom), 0, croom);	/* down */
	if (nroom > 1) {
	    troom = croom;
	    croom = &rooms[rn2(nroom-1)];
	    if (croom == troom) croom++;
	}

	if (u.uz.dlevel != 1) {
	    xchar sx, sy;
	    do {
			sx = somex(croom);
			sy = somey(croom);
	    } while(occupied(sx, sy));
	    mkstairs(sx, sy, 1, croom);	/* up */
	}

	branchp = Is_branchlev(&u.uz);	/* possible dungeon branch */
	room_threshold = branchp ? 4 : 3; /* minimum number of rooms needed
					     to allow a random special room */
#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) goto skip0;
#endif

	merge_adj_rooms();
	if(!rn2(6) && In_dungeons_of_doom(&u.uz)){
		makehallways();
		hallways = TRUE;
	}
	else {
		makecorridors();
		make_niches();
	}

	/* fix up room walls, which may have been broken by having overlapping or joined rooms */
	for (tryct = 0; tryct < nroom; tryct++)
	{
		croom = &rooms[tryct];
		wallification(croom->lx - 1, croom->ly - 1, croom->hx + 1, croom->hy + 1);
	}

	if(In_mithardir_catacombs(&u.uz)){
		//Mini-level level setup
		int i = rn2(7);
		while(i-- > 0) mkroom(POOLROOM);
		for(croom = rooms; croom->hx > 0; croom++) {
			if(croom->rtype != OROOM && croom->rtype != JOINEDROOM) continue;
			if(!rn2(3)) {
				x = somex(croom); y = somey(croom);
				tmonst = makemon(rn2(2) ? &mons[PM_ALABASTER_MUMMY] : 0, x,y,NO_MM_FLAGS);
			}
			if(!rn2(nroom * 5 / 2))
				(void) mksobj_at((rn2(3)) ? BOX : CHEST, somex(croom), somey(croom), NO_MKOBJ_FLAGS);
			if(!rn2(9))
				(void) mkobj_at(TILE_CLASS, somex(croom), somey(croom), MKOBJ_ARTIF);
		}
		goto mithardir_end;
	}
	
	/* make a secret treasure vault, not connected to the rest */
	if(do_vault()) {
		xchar w,h;
#ifdef DEBUG
		debugpline("trying to make a vault...");
#endif
		w = 1;
		h = 1;
		if (check_room(&vault_x, &w, &vault_y, &h, TRUE)) {
		    fill_vault:
			add_room(vault_x, vault_y, vault_x+w,
				 vault_y+h, TRUE, VAULT, FALSE);
			level.flags.has_vault = 1;
			++room_threshold;
			fill_room(&rooms[nroom - 1], FALSE);
			mk_knox_portal(vault_x+w, vault_y+h);
			if(!level.flags.noteleport && !rn2(3)) makevtele();
		} else if(rnd_rect() && create_vault()) {
			vault_x = rooms[nroom].lx;
			vault_y = rooms[nroom].ly;
			if (check_room(&vault_x, &w, &vault_y, &h, TRUE))
				goto fill_vault;
			else
				rooms[nroom].hx = -1;
		}
	}

    {
	int u_depth = depth(&u.uz);

#ifdef WIZARD
	if(wizard && nh_getenv("SHOPTYPE")) mkroom(SHOPBASE); else
#endif

	/* New room selection code:
		We divide special rooms into different types and put up to one of each in
		the level.  Because there are only so many suitable rooms on each
		random map, levels with two kinds of rooms still aren't too common,
		and I've yet to see three in tests.
		Also, we separate out level-wide specials, like swamps,
		and give them the chance to forbid special rooms from appearing.
	*/

	/* Part one: early level-wide modifications */
	if (u_depth > 15 && !rn2(8)) {
		mkroom(SWAMP);
		goto skiprooms;
	}
	
	/* Part two: special rooms */
	/* Shops */
	if (u_depth > 1 &&
	    u_depth < depth(&challenge_level) &&
	    nroom >= room_threshold &&
		rn2(u_depth) < 4) mkroom(SHOPBASE);	// small increase to shop spawnrate (3->4) to compensate for there being, in general, more rooms and thus fewer eligible shops

	/* Zoos */
	{
	int zootype;
	if ((zootype = random_special_room()))
		mkroom(zootype);
	}

	/* Terrain */
	if (u_depth > 2 && !rn2(8)) mkroom(ISLAND);
		else if (u_depth > 8 && !rn2(7)) mkroom(TEMPLE);

		/* Part three: late modifications */
		/* Rivers on vault levels are buggy, so we forbid that.
		Islands + rivers are potentially too blocking,
			so no that either. 
		dNethack adjustment: as Vlad's has water 
			walking boots, I'm allowing Islands+rivers. */
	if (u_depth > 3 && !rn2(4) &&
		!level.flags.has_vault) mkroom(RIVER);

		/* Part four: very late modifications */
	if (wantfingerprint &&
		!level.flags.has_vault){
		mkfingervault();
	}
	if (wantasepulcher &&
		!level.flags.has_vault){
		mksepulcher();
	}
	if (wantanmivault &&
		!level.flags.has_vault){
		mkmivault();
	}
	
	} /*end u_depth*/

skiprooms:

#ifdef REINCARNATION
skip0:
#endif
	/* Place multi-dungeon branch. */
	place_branch(branchp, 0, 0);

	/* for each room: put things inside */
	for(croom = rooms; croom->hx > 0; croom++) {
		if(croom->rtype != OROOM && croom->rtype != JOINEDROOM) continue;

		/* put a sleeping monster inside */
		/* Note: monster may be on the stairs. This cannot be
		   avoided: maybe the player fell through a trap door
		   while a monster was on the stairs. Conclusion:
		   we have to check for monsters on the stairs anyway. */

		if(u.uhave.amulet || !rn2(3)) {
		    x = somex(croom); y = somey(croom);
		    tmonst = makemon((struct permonst *) 0, x,y,NO_MM_FLAGS);
		    if (tmonst && tmonst->mtyp == PM_GIANT_SPIDER &&
			    !occupied(x, y))
			(void) maketrap(x, y, WEB);
		}
		/* put traps and mimics inside */
		goldseen = FALSE;
		x = 8 - (level_difficulty()/6);
		if (x <= 1) x = 2;
		while (!rn2(x))
		    mktrap(0,0,croom,(coord*)0);
		if (!goldseen && !rn2(3))
		    (void) mkgold(0L, somex(croom), somey(croom));
#ifdef REINCARNATION
		if(Is_rogue_level(&u.uz)) goto skip_nonrogue;
#endif
		/* greater chance of puddles if a water source is nearby */
		x = 40;
		if(!rn2(10)) {
		    if(mkfeature(FOUNTAIN, FALSE, croom))
				x -= 20;
		}
#ifdef SINKS
		if(!rn2(60)) {
		    if(mkfeature(SINK, FALSE, croom))
				x -= 20;
		}
		if(!rn2(40)) {
		    mkfeature(FORGE, FALSE, croom);
		}

		if(!rn2(280)) {
		    mkfeature(TREE, FALSE, croom);
		}

		if (x < 2) x = 2;
#endif
		if(!rn2(x))
			mkfeature(PUDDLE, FALSE, croom);

		if(!rn2(60))
			mkfeature(ALTAR, FALSE, croom);

		x = 80 - (depth(&u.uz) * 2);
		if (x < 2) x = 2;
		if(!rn2(x))
			mkfeature(GRAVE, FALSE, croom);

		/* put statues inside */
		if(!rn2(20))
		    (void) mkcorpstat(STATUE, (struct monst *)0,
				      (struct permonst *)0,
				      somex(croom), somey(croom), TRUE);
		/* put box/chest inside;
		 *  40% chance for at least 1 box, regardless of number
		 *  of rooms; about 5 - 7.5% for 2 boxes, least likely
		 *  when few rooms; chance for 3 or more is neglible.
		 */
		if(!rn2(nroom * 5 / 2))
		    (void) mksobj_at((rn2(3)) ? BOX : CHEST, somex(croom), somey(croom), NO_MKOBJ_FLAGS);

		//About a 10% chance of a magic chest per level
		if(!rn2(nroom * 10) && !magic_chest){
		    (void) mksobj_at(MAGIC_CHEST, somex(croom), somey(croom), NO_MKOBJ_FLAGS);
			magic_chest = TRUE;
		}

		/* maybe make some graffiti */
		if(!rn2(27 + 3 * abs(depth(&u.uz)))) {
		    char buf[BUFSZ];
		    const char *mesg = random_engraving(buf);
		    if (mesg) {
			do {
			    x = somex(croom);  y = somey(croom);
			} while(levl[x][y].typ != ROOM && !rn2(40));
			if (!(IS_POOL(levl[x][y].typ) ||
			      IS_FURNITURE(levl[x][y].typ)))
			    make_engr_at(x, y, mesg, 0L, MARK);
		    }
		}

#ifdef REINCARNATION
	skip_nonrogue:
#endif
		if(!rn2(3)) {
		    (void) mkobj_at(0, somex(croom), somey(croom), MKOBJ_ARTIF);
		    tryct = 0;
		    while(!rn2(5)) {
			if(++tryct > 100) {
			    impossible("tryct overflow4");
			    break;
			}
			(void) mkobj_at(0, somex(croom), somey(croom), MKOBJ_ARTIF);
		    }
		}
	}
mithardir_end:
	if(In_mithardir_catacombs(&u.uz)){
		wallify_mithardir_corridors();
		if(In_mithardir_terminus(&u.uz)) mkroom(RIVER);
	}
	else if(hallways){
		wallify_hallway_corridors();
	}
	if (flags.makelev_closerooms)			
		flags.makelev_closerooms = FALSE;
}

/*
 *	Place deposits of minerals (gold and misc gems) in the stone
 *	surrounding the rooms on the map.
 *	Also place kelp in water.
 */
void
mineralize(void)
{
	s_level *sp;
	struct obj *otmp;
	int goldprob, gemprob, silverprob, darkprob, fossilprob, x, y, cnt, d_lev, dun_lev;


	/* Place kelp, except on the plane of water */
	if(!Is_waterlevel(&u.uz)){
		for (x = 2; x < (COLNO - 2); x++)
			for (y = 1; y < (ROWNO - 1); y++)
			if ((levl[x][y].typ == POOL && !rn2(10)) ||
				(levl[x][y].typ == MOAT && !rn2(30)))
				(void) mksobj_at(KELP_FROND, x, y, NO_MKOBJ_FLAGS);
	}

	/* determine if it is even allowed;
	   almost all special levels are excluded */
	if (In_hell(&u.uz) || In_V_tower(&u.uz) ||
		(In_endgame(&u.uz) && !Is_earthlevel(&u.uz) && !Is_firelevel(&u.uz)) ||
#ifdef REINCARNATION
		Is_rogue_level(&u.uz) ||
#endif
		level.flags.arboreal
	) return;

	/* basic level-related probabilities */
	if(Is_earthlevel(&u.uz))
		d_lev = 60;
	else d_lev = depth(&u.uz);
	if(Is_earthlevel(&u.uz))
		dun_lev = 30;
	else dun_lev = dunlev(&u.uz);
	goldprob = 20 + d_lev / 3;
	gemprob = goldprob / 4;
	silverprob = gemprob * 2;
	fossilprob = gemprob / 2;
	darkprob = gemprob / 5;

	/* mines have ***MORE*** goodies - otherwise why mine? */
	if (In_mines_quest(&u.uz)) {
	    gemprob = goldprob*3 / 4;
	    silverprob = goldprob*2;
	    goldprob *= 2;
	} else if (Is_nemesis(&u.uz) && urole.neminum == PM_BLIBDOOLPOOLP__GRAVEN_INTO_FLESH) {
	    silverprob = goldprob;
	    darkprob = gemprob;
	} else if (In_quest(&u.uz)) {
	    goldprob /= 4;
	    gemprob /= 6;
	    silverprob /= 8;
	}

	/*
	 * Seed rock areas with gold and/or gems.
	 * We use fairly low level object handling to avoid unnecessary
	 * overhead from placing things in the floor chain prior to burial.
	 */
	for (x = 2; x < (COLNO - 2); x++)
	  for (y = 1; y < (ROWNO - 1); y++){
		if(levl[x][y].typ == ROOM){
			if(Is_nemesis(&u.uz) && urole.neminum == PM_BLIBDOOLPOOLP__GRAVEN_INTO_FLESH){
				if(!rn2(200)){
					if ((otmp = mksobj(CHUNK_OF_FOSSIL_DARK, MKOBJ_NOINIT)) != 0) {
						otmp->quan = 1L;
						otmp->owt = weight(otmp);
						otmp->ox = x,  otmp->oy = y;
						add_to_buried(otmp);
					}
				}
			}
		}
	    if (levl[x][y+1].typ != STONE) {	 /* <x,y> spot not eligible */
			y += 2;		/* next two spots aren't eligible either */
	    } else if (levl[x][y].typ != STONE) { /* this spot not eligible */
			y += 1;		/* next spot isn't eligible either */
	    } else if (!(levl[x][y].wall_info & (W_NONPASSWALL|W_NONDIGGABLE)) &&
		  levl[x][y-1].typ   == STONE &&
		  levl[x+1][y-1].typ == STONE && levl[x-1][y-1].typ == STONE &&
		  levl[x+1][y].typ   == STONE && levl[x-1][y].typ   == STONE &&
		  levl[x+1][y+1].typ == STONE && levl[x-1][y+1].typ == STONE
		) {
			if (rn2(1000) < goldprob) {
				if ((otmp = mksobj(GOLD_PIECE, MKOBJ_NOINIT)) != 0) {
				otmp->ox = x,  otmp->oy = y;
				otmp->quan = 1L + rnd(goldprob * 3);
				u.spawnedGold += otmp->quan;
				otmp->owt = weight(otmp);
				if (!rn2(3) && Can_dig_down(&u.uz)) add_to_buried(otmp);
				else place_object(otmp, x, y);
				}
			}
			if (rn2(1000) < gemprob) {
				for (cnt = rnd(2 + dun_lev / 3); cnt > 0; cnt--)
				if ((otmp = mkobj(GEM_CLASS, FALSE)) != 0) {
					if (otmp->otyp == ROCK) {
					dealloc_obj(otmp);	/* discard it */
					} else {
					otmp->ox = x,  otmp->oy = y;
					if (!rn2(3) && Can_dig_down(&u.uz)) add_to_buried(otmp);
					else place_object(otmp, x, y);
					}
				}
			}
			if (rn2(1000) < silverprob) {
				if ((otmp = mksobj(SILVER_SLINGSTONE, MKOBJ_NOINIT)) != 0) {
					otmp->quan = 1L + rn2(dun_lev);
					otmp->owt = weight(otmp);
					otmp->ox = x,  otmp->oy = y;
					if (!rn2(3) && Can_dig_down(&u.uz)) add_to_buried(otmp);
					else place_object(otmp, x, y);
				}
			}
			if (d_lev > 14 && rn2(1000) < darkprob) {
				if ((otmp = mksobj(CHUNK_OF_FOSSIL_DARK, MKOBJ_NOINIT)) != 0) {
					otmp->quan = 1L;
					otmp->owt = weight(otmp);
					otmp->ox = x,  otmp->oy = y;
					if (!rn2(3) && Can_dig_down(&u.uz)) add_to_buried(otmp);
					else place_object(otmp, x, y);
				}
			}
			if (d_lev > 14 && rn2(1000) < fossilprob) {
				if(!rn2(20)){
					otmp = mksobj(TOOTH, NO_MKOBJ_FLAGS);
					otmp->ox = x,  otmp->oy = y;
					if (!rn2(3) && Can_dig_down(&u.uz)) add_to_buried(otmp);
					else place_object(otmp, x, y);
				}
				else if ((otmp = mksobj(FOSSIL, NO_MKOBJ_FLAGS)) != 0) {
					otmp->quan = 1L;
					otmp->owt = weight(otmp);
					otmp->ox = x,  otmp->oy = y;
					if (!rn2(3) && Can_dig_down(&u.uz)) add_to_buried(otmp);
					else place_object(otmp, x, y);
				}
			}
			if(Is_earthlevel(&u.uz) && rn2(1000) < 1){
				if ((otmp = mksobj(SANCTIFIED_CALCITE_CRYSTAL, NO_MKOBJ_FLAGS)) != 0) {
					otmp->quan = 1L;
					otmp->owt = weight(otmp);
					otmp->ox = x,  otmp->oy = y;
					if (!rn2(3) && Can_dig_down(&u.uz)) add_to_buried(otmp);
					else place_object(otmp, x, y);
				}
			}
	    }
	  }
}


void
wallwalk_right(x,y,fgtyp,fglit,bgtyp,chance)
     xchar x,y;
     schar fgtyp,fglit,bgtyp;
     int chance;
{
    int sx,sy, nx,ny, dir, cnt;
    schar tmptyp;
    sx = x;
    sy = y;
    dir = 1;

    if (!isok(x,y)) return;
    if (levl[x][y].typ != bgtyp) return;

    do {
	if (!t_at(x,y) && !bydoor(x,y) && levl[x][y].typ == bgtyp && (chance >= rn2(100))) {
	    SET_TYPLIT(x,y, fgtyp, fglit);
	}
	cnt = 0;
	do {
	    nx = x;
	    ny = y;
	    switch (dir % 4) {
	    case 0: y--; break;
	    case 1: x++; break;
	    case 2: y++; break;
	    case 3: x--; break;
	    }
	    if (isok(x,y)) {
		tmptyp = levl[x][y].typ;
		if (tmptyp != bgtyp && tmptyp != fgtyp) {
		    dir++; x = nx; y = ny; cnt++;
		} else {
		    dir = (dir + 3) % 4;
		}
	    } else {
		dir++; x = nx; y = ny; cnt++;
	    }
	} while ((nx == x && ny == y) && (cnt < 5));
    } while ((x != sx) || (y != sy));
}

void
mklev()
{
	struct mkroom *croom;

	init_mapseen(&u.uz);
	if(getbones()) return;
	in_mklev = TRUE;
	makelevel();
	bound_digging();
	mineralize();
	special_mklev_actions();
	in_mklev = FALSE;
	/* has_morgue gets cleared once morgue is entered; graveyard stays
	   set (graveyard might already be set even when has_morgue is clear
	   [see fixup_special()], so don't update it unconditionally) */
	if (level.flags.has_morgue)
	    level.flags.graveyard = 1;
	if (!level.flags.is_maze_lev) {
	    for (croom = &rooms[0]; croom != &rooms[nroom]; croom++)
#ifdef SPECIALIZATION
		topologize(croom, FALSE);
#else
		topologize(croom);
#endif
	}
	set_wall_state();
	if(u.silver_flame_z.dnum == u.uz.dnum && u.silver_flame_z.dlevel == u.uz.dlevel){
		int limit = 1000;
		do {
			u.s_f_x = rnd(COLNO-1);
			u.s_f_y = rn2(ROWNO);
		} while((!isok(u.s_f_x, u.s_f_y) || !ZAP_POS(levl[u.s_f_x][u.s_f_y].typ)) && limit-- > 0);
	}
}

void
#ifdef SPECIALIZATION
topologize(croom, do_ordinary)
register struct mkroom *croom;
boolean do_ordinary;
#else
topologize(croom)
register struct mkroom *croom;
#endif
{
	register int x, y, roomno = (croom - rooms) + ROOMOFFSET;
	register int lowx = croom->lx, lowy = croom->ly;
	register int hix = croom->hx, hiy = croom->hy;
#ifdef SPECIALIZATION
	register schar rtype = croom->rtype;
#endif
	register int subindex, nsubrooms = croom->nsubrooms;

	/* skip the room if already done; i.e. a shop handled out of order */
	/* also skip if this is non-rectangular (it _must_ be done already) */
	if ((int) levl[lowx][lowy].roomno == roomno || croom->irregular)
	    return;
#ifdef SPECIALIZATION
# ifdef REINCARNATION
	if (Is_rogue_level(&u.uz))
	    do_ordinary = TRUE;		/* vision routine helper */
# endif
	if ((rtype != OROOM) || do_ordinary)
#endif
	{
	    /* do innards first */
	    for(x = lowx; x <= hix; x++)
		for(y = lowy; y <= hiy; y++)
#ifdef SPECIALIZATION
		    if (rtype == OROOM)
			levl[x][y].roomno = NO_ROOM;
		    else
#endif
			levl[x][y].roomno = roomno;
	    /* top and bottom edges */
	    for(x = lowx-1; x <= hix+1; x++)
		for(y = lowy-1; y <= hiy+1; y += (hiy-lowy+2)) {
		    levl[x][y].edge = 1;
		    if (levl[x][y].roomno)
			levl[x][y].roomno = SHARED;
		    else
			levl[x][y].roomno = roomno;
		}
	    /* sides */
	    for(x = lowx-1; x <= hix+1; x += (hix-lowx+2))
		for(y = lowy; y <= hiy; y++) {
		    levl[x][y].edge = 1;
		    if (levl[x][y].roomno)
			levl[x][y].roomno = SHARED;
		    else
			levl[x][y].roomno = roomno;
		}
	}
	/* subrooms */
	for (subindex = 0; subindex < nsubrooms; subindex++)
#ifdef SPECIALIZATION
		topologize(croom->sbrooms[subindex], (rtype != OROOM));
#else
		topologize(croom->sbrooms[subindex]);
#endif
}

/* Find an unused room for a branch location. */
STATIC_OVL struct mkroom *
find_branch_room(mp)
    coord *mp;
{
    struct mkroom *croom = 0;
	int tryct = 0;

    if (nroom == 0) {
	mazexy(mp);		/* already verifies location */
    } else {
	/* not perfect - there may be only one stairway */
	if(nroom > 2) {
	    tryct = 0;

	    do
		croom = &rooms[rn2(nroom)];
	    while((croom == dnstairs_room || croom == upstairs_room ||
		  (croom->rtype != OROOM && croom->rtype != JOINEDROOM)) && (++tryct < 100));
	} else
	    croom = &rooms[rn2(nroom)];
	
	tryct = 0;
	do {
	    if (!somexy(croom, mp))
		impossible("Can't place branch!");
	} while(occupied(mp->x, mp->y) ||
	    (levl[mp->x][mp->y].typ != CORR 
		&& levl[mp->x][mp->y].typ != ROOM
		&& levl[mp->x][mp->y].typ != GRASS
		&& levl[mp->x][mp->y].typ != SOIL
		&& levl[mp->x][mp->y].typ != SAND
		&& levl[mp->x][mp->y].typ != PUDDLE) ||
		tryct++ > 1000);
    }
    return croom;
}

/* Find the room for (x,y).  Return null if not in a room. */
STATIC_OVL struct mkroom *
pos_to_room(x, y)
    xchar x, y;
{
    int i;
    struct mkroom *curr;

    for (curr = rooms, i = 0; i < nroom; curr++, i++)
	if (inside_room(curr, x, y)) return curr;;
    return (struct mkroom *) 0;
}


/* If given a branch, randomly place a special stair or portal. */
void
place_branch(br, x, y)
branch *br;	/* branch to place */
xchar x, y;	/* location */
{
	coord	      m;
	d_level	      *dest;
	boolean	      make_stairs;
	struct mkroom *br_room;

	/*
	 * Return immediately if there is no branch to make or we have
	 * already made one.  This routine can be called twice when
	 * a special level is loaded that specifies an SSTAIR location
	 * as a favored spot for a branch.
	 */
	if (!br || made_branch) return;
	
	if (!x) {	/* find random coordinates for branch */
	    br_room = find_branch_room(&m);
	    x = m.x;
	    y = m.y;
	} else {
	    br_room = pos_to_room(x, y);
	}

	if (on_level(&br->end1, &u.uz)) {
	    /* we're on end1 */
	    make_stairs = br->type != BR_NO_END1;
	    dest = &br->end2;
	} else {
	    /* we're on end2 */
	    make_stairs = br->type != BR_NO_END2;
	    dest = &br->end1;
	}

	//The female half dragon noble has a lot of shenanigans going on
	if(Role_if(PM_NOBLEMAN) && Race_if(PM_HALF_DRAGON) && flags.initgend){
		//There is no return portal on the first quest level
		if(flags.initgend && Is_qstart(&u.uz))
			return;
		//The branch level has an artifact instead of a portal
		if(dest->dnum == quest_dnum){
			struct obj *obj;
			obj = mksobj_at(SCR_BLANK_PAPER, x, y, MKOBJ_NOINIT);
			if(obj) obj = oname(obj, artiname(ART_PAINTING_FRAGMENT));
			return;
		}
	}

	if (br->type == BR_PORTAL) {
	    mkportal(x, y, dest->dnum, dest->dlevel);
	} else if (make_stairs) {
	    sstairs.sx = x;
	    sstairs.sy = y;
		if (!u.uz.dnum && u.uz.dlevel == 1) sstairs.u_traversed = TRUE;
	    sstairs.up = (char) on_level(&br->end1, &u.uz) ?
					    br->end1_up : !br->end1_up;
	    assign_level(&sstairs.tolev, dest);
	    sstairs_room = br_room;

	    levl[x][y].ladder = sstairs.up ? LA_UP : LA_DOWN;
	    levl[x][y].typ = STAIRS;
	}
	/*
	 * Set made_branch to TRUE even if we didn't make a stairwell (i.e.
	 * make_stairs is false) since there is currently only one branch
	 * per level, if we failed once, we're going to fail again on the
	 * next call.
	 */
	made_branch = TRUE;
}

STATIC_OVL boolean
bydoor(x, y)
register xchar x, y;
{
	register int typ;

	if (isok(x+1, y)) {
		typ = levl[x+1][y].typ;
		if (IS_DOOR(typ) || typ == SDOOR) return TRUE;
	}
	if (isok(x-1, y)) {
		typ = levl[x-1][y].typ;
		if (IS_DOOR(typ) || typ == SDOOR) return TRUE;
	}
	if (isok(x, y+1)) {
		typ = levl[x][y+1].typ;
		if (IS_DOOR(typ) || typ == SDOOR) return TRUE;
	}
	if (isok(x, y-1)) {
		typ = levl[x][y-1].typ;
		if (IS_DOOR(typ) || typ == SDOOR) return TRUE;
	}
	return FALSE;
}

/* see whether it is allowable to create a door at [x,y] */
int
okdoor(x,y)
register xchar x, y;
{
	register boolean near_door = bydoor(x, y);

	return((levl[x][y].typ == HWALL || levl[x][y].typ == VWALL) &&
			doorindex < DOORMAX && !near_door);
}

void
dodoor(x,y,aroom)
register int x, y;
register struct mkroom *aroom;
{
	if(doorindex >= DOORMAX) {
		impossible("DOORMAX exceeded?");
		return;
	}

	dosdoor(x,y,aroom,rn2(8) ? DOOR : SDOOR);
}

boolean
occupied(x, y)
register xchar x, y;
{
	return(!isok(x, y)
		|| (boolean)(t_at(x, y)
		|| IS_FURNITURE(levl[x][y].typ)
		|| IS_ROCK(levl[x][y].typ)
		|| is_lava(x,y)
		|| (is_pool(x,y, FALSE) && (!Is_waterlevel(&u.uz) || is_3dwater(x,y)))
		|| invocation_pos(x,y)
		));
}

/* make a trap somewhere (in croom if mazeflag = 0 && !tm) */
/* if tm != null, make trap at that location */
void
mktrap(num, mazeflag, croom, tm)
register int num, mazeflag;
register struct mkroom *croom;
coord *tm;
{
	register int kind;
	coord m;

	/* no traps in pools */
	if (tm && is_pool(tm->x,tm->y, TRUE)) return;

	if (num > 0 && num < TRAPNUM) {
	    kind = num;
#ifdef REINCARNATION
	} else if (Is_rogue_level(&u.uz)) {
	    switch (rn2(7)) {
		default: kind = BEAR_TRAP; break; /* 0 */
		case 1: kind = ARROW_TRAP; break;
		case 2: kind = DART_TRAP; break;
		case 3: kind = TRAPDOOR; break;
		case 4: kind = PIT; break;
		case 5: kind = SLP_GAS_TRAP; break;
		case 6: kind = RUST_TRAP; break;
	    }
#endif
	} else if (Inhell && !rn2(5)) {
	    /* bias the frequency of fire traps in Gehennom */
	    kind = FIRE_TRAP;
	} else {
	    unsigned lvl = level_difficulty();

	    do {
		kind = rnd(TRAPNUM-1);
		/* reject "too hard" traps */
		switch (kind) {
		    case VIVI_TRAP:
		    case SWITCH_TRAP:
		    case FLESH_HOOK:
			kind = NO_TRAP; break;
			case MUMMY_TRAP:
			if (!(Is_qlocate(&u.uz) && Role_if(PM_ARCHEOLOGIST))) kind = NO_TRAP; 
			break;
		    case MAGIC_PORTAL:
			kind = NO_TRAP; break;
		    case ROLLING_BOULDER_TRAP:
		    case SLP_GAS_TRAP:
			if (lvl < 2) kind = NO_TRAP; break;
		    case LEVEL_TELEP:
			// if (lvl < 5 || level.flags.noteleport)
			    // kind = NO_TRAP; break;
			if (lvl < 5)
			    kind = NO_TRAP;
			else kind = TRAPDOOR;
			break;
		    case SPIKED_PIT:
			if (lvl < 5) kind = NO_TRAP; break;
		    case LANDMINE:
			if (lvl < 6) kind = NO_TRAP; break;
		    case WEB:
			if (lvl < 7) kind = NO_TRAP; break;
		    case STATUE_TRAP:
		    case POLY_TRAP:
			if (lvl < 8) kind = NO_TRAP; break;
		    case FIRE_TRAP:
			if (!Inhell) kind = NO_TRAP; break;
		    case TELEP_TRAP:
			if (level.flags.noteleport) kind = NO_TRAP; break;
		    case HOLE:
			/* make these much less often than other traps */
			if (rn2(7)) kind = NO_TRAP; break;
		}
	    } while (kind == NO_TRAP);
	}

	if ((kind == TRAPDOOR || kind == HOLE) && !Can_fall_thru(&u.uz))
		kind = ROCKTRAP;

	if (tm)
	    m = *tm;
	else {
	    register int tryct = 0;
	    boolean avoid_boulder = (kind == PIT || kind == SPIKED_PIT ||
				     kind == TRAPDOOR || kind == HOLE);

	    do {
			if (++tryct > 200)
				return;
			if (mazeflag)
				mazexy(&m);
			else if (!somexy(croom,&m))
				return;
	    } while (occupied(m.x, m.y) ||
			(avoid_boulder && boulder_at(m.x, m.y)));
	}

	(void) maketrap(m.x, m.y, kind);
	if (kind == WEB) (void) makemon(&mons[PM_GIANT_SPIDER],
						m.x, m.y, NO_MM_FLAGS);
}

struct mkroom*
room_at(x, y)
xchar x, y;
{
	if (levl[x][y].roomno)
		return &rooms[levl[x][y].roomno - ROOMOFFSET];
	else
		return (struct mkroom *)0;
}

void
mkstairs(x, y, up, croom)
xchar x, y;
char  up;
struct mkroom *croom;
{
	if (!x) {
	    impossible("mkstairs:  bogus stair attempt at <%d,%d>", x, y);
	    return;
	}

	/*
	 * We can't make a regular stair off an end of the dungeon.  This
	 * attempt can happen when a special level is placed at an end and
	 * has an up or down stair specified in its description file.
	 */
	if ((dunlev(&u.uz) == 1 && up) ||
			(dunlev(&u.uz) == dunlevs_in_dungeon(&u.uz) && !up)
	){
		if(Role_if(PM_RANGER) && Race_if(PM_GNOME) && Is_qstart(&u.uz)){
			levl[x][y].typ = STAIRS;
			levl[x][y].ladder = up ? LA_UP : LA_DOWN;
			sstairs.sx = x;
			sstairs.sy = y;
			sstairs.up = up;
			assign_level(&sstairs.tolev, &minetown_level);
		}
	    return;
	}

	if(up) {
		xupstair = x;
		yupstair = y;
		upstairs_room = croom;
	} else {
		xdnstair = x;
		ydnstair = y;
		dnstairs_room = croom;
	}

	levl[x][y].typ = STAIRS;
	levl[x][y].ladder = up ? LA_UP : LA_DOWN;
}

boolean
mkfeature(typ, mazeflag, croom)
int typ;
int mazeflag;
struct mkroom *croom;
{
	coord m;
	int tryct = 0;
	int tmp = 0;

	do {
		if (++tryct > 200)
			return FALSE;
		if (mazeflag)
			mazexy(&m);
		else
		if (!somexy(croom, &m))
			return FALSE;
	} while (occupied(m.x, m.y) || bydoor(m.x, m.y));

	switch (typ)
	{
	case FOUNTAIN:
		/* Put a fountain at m.x, m.y */
		levl[m.x][m.y].typ = FOUNTAIN;
		/* Is it a "blessed" fountain? (affects drinking from fountain) */
		if (!rn2(7)) levl[m.x][m.y].blessedftn = 1;
		level.flags.nfountains++;
		break;
	case FORGE:
		/* Put a forge at m.x, m.y */
		levl[m.x][m.y].typ = FORGE;
		if (!rn2(7)){
			int pm = PM_HUMAN_SMITH;
			struct monst *smith;
			switch(rn2(7)){
				case 0:
				case 1:
					pm = PM_GOBLIN_SMITH;
				break;
				case 2:
				case 3:
					pm = PM_DWARF_SMITH;
				break;
				case 4:
				case 5:
					pm = PM_HUMAN_SMITH;
				case 6:
				break;
					pm = PM_MITHRIL_SMITH;
				break;
			}
			smith = makemon(&mons[pm], m.x, m.y, NO_MM_FLAGS);
			if(smith && HAS_ESMT(smith)){
				ESMT(smith)->frgpos.x = m.x;
				ESMT(smith)->frgpos.y = m.y;
				ESMT(smith)->frglevel = u.uz;
			}
		}
		level.flags.nforges++;
		break;
	case TREE:
		/* Put a forge at m.x, m.y */
		levl[m.x][m.y].typ = TREE;
		{
			struct monst *smith;
			smith = makemon(&mons[PM_TREESINGER], m.x, m.y, MM_ADJACENTOK);
			if(smith && HAS_ESMT(smith)){
				ESMT(smith)->frgpos.x = m.x;
				ESMT(smith)->frgpos.y = m.y;
				ESMT(smith)->frglevel = u.uz;
			}
		}
		break;
	case SINK:
		/* Put a sink at m.x, m.y */
		levl[m.x][m.y].typ = SINK;
		level.flags.nsinks++;
		break;
	case ALTAR:
		if (croom && croom->rtype != OROOM && croom->rtype != JOINEDROOM)
			return FALSE;
		/* Put an altar at m.x, m.y */
		/* -1 - A_CHAOTIC, 0 - A_NEUTRAL, 1 - A_LAWFUL */
		tmp = (Inhell ? A_NONE : rn2(3)-1);
		add_altar(m.x, m.y, tmp, FALSE, GOD_NONE);
		break;
	case PUDDLE:
		tmp = 0;	// number of puddles made
		do {
			// make puddles
			if (levl[m.x][m.y].typ != PUDDLE && levl[m.x][m.y].typ != POOL) {
				tmp++;
				levl[m.x][m.y].typ = (depth(&u.uz) > 9 && !rn2(4) ?
				POOL : PUDDLE);
			}
			// also make sea creatures
			if (tmp > 4 && depth(&u.uz) > 4) {
				(void)makemon(levl[m.x][m.y].typ == POOL ? mkclass(S_EEL, 0) :
					&mons[PM_PIRANHA], m.x, m.y, NO_MM_FLAGS);
				tmp -= 2; /* puddles created should always exceed piranhas */
			}
			tryct = 0;
			do {
				m.x += sgn(rn2(3) - 1);
				m.y += sgn(rn2(3) - 1);
			} while ((occupied(m.x, m.y) ||
				(croom && (
				m.x < croom->lx || m.x > croom->hx ||
				m.y < croom->ly || m.y > croom->hy)))
				&& (++tryct <= 27));
		} while (tryct <= 27);
		break;
	case GRAVE:
		if (croom && croom->rtype != OROOM && croom->rtype != JOINEDROOM)
			return FALSE;
		tmp = !rn2(20);
		/* Put a grave at m.x, m.y */
		make_grave(m.x, m.y, tmp ? "Saved by the bell!" : (char *)0);

		/* Possibly fill it with objects */
		if (!rn2(3)) (void)mkgold(0L, m.x, m.y);
		for (tryct = rn2(5); tryct; tryct--) {
			struct obj *otmp = mkobj(RANDOM_CLASS, TRUE);
			if (!otmp)
				continue;
			curse(otmp);
			otmp->ox = m.x;
			otmp->oy = m.y;
			add_to_buried(otmp);
		}
		/* Leave a bell, in case we accidentally buried someone alive */
		if (tmp) (void)mksobj_at(BELL, m.x, m.y, NO_MKOBJ_FLAGS);
		break;
	}
	return TRUE;
}

/* make a statue that identifies the boss inside a hellvault */
void
mkHVstatue(x, y, hv_id)
int x, y, hv_id;
{
	int mid = PM_LAMB;
	struct obj* obj;
	switch(hv_id){
		case VN_AKKABISH:
		case VN_SHALOSH:
		case VN_NACHASH:
		case VN_KHAAMNUN:
		case VN_RAGLAYIM:
		case VN_TERAPHIM:
		case VN_SARTAN:
			mid = PM_STRANGE_LARVA;
		break;
		case VN_A_O_BLESSINGS:
		case VN_A_O_VITALITY:
		case VN_A_O_CORRUPTION:
		case VN_A_O_BURNING_WASTES:
		case VN_A_O_THOUGHT:
		case VN_A_O_DEATH:
			mid = PM_ANCIENT_NUPPERIBO;
		break;
		case VN_APOCALYPSE:
		case VN_HARROWER:
		case VN_MAD_ANGEL:
		case VN_JRT:
			mid = PM_ANGEL;
		break;
		case VN_N_PIT_FIEND:
			mid = PM_LEMURE;
		break;
		case VN_SHAYATEEN:
			mid = PM_MANES;
		break;
	}
	obj = mksobj_at(STATUE, x, y, NO_MKOBJ_FLAGS);
	obj->corpsenm = mid;
	fix_object(obj);
}

/* maze levels have slightly different constraints from normal levels */
#define x_maze_min 2
#define y_maze_min 2
/*
 * Major level transmutation: add a set of stairs (to the Sanctum) after
 * an earthquake that leaves behind a a new topology, centered at inv_pos.
 * Assumes there are no rooms within the invocation area and that inv_pos
 * is not too close to the edge of the map.  Also assume the hero can see,
 * which is guaranteed for normal play due to the fact that sight is needed
 * to read the Book of the Dead.
 */
void
mkinvokearea()
{
    int dist;
    xchar xmin = inv_pos.x, xmax = inv_pos.x;
    xchar ymin = inv_pos.y, ymax = inv_pos.y;
    register xchar i;

    pline_The("floor shakes violently under you!");
    pline_The("walls around you begin to bend and crumble!");
    display_nhwindow(WIN_MESSAGE, TRUE);

    mkinvpos(xmin, ymin, 0);		/* middle, before placing stairs */

    for(dist = 1; dist < 7; dist++) {
	xmin--; xmax++;

	/* top and bottom */
	if(dist != 3) { /* the area is wider that it is high */
	    ymin--; ymax++;
	    for(i = xmin+1; i < xmax; i++) {
		mkinvpos(i, ymin, dist);
		mkinvpos(i, ymax, dist);
	    }
	}

	/* left and right */
	for(i = ymin; i <= ymax; i++) {
	    mkinvpos(xmin, i, dist);
	    mkinvpos(xmax, i, dist);
	}

	flush_screen(1);	/* make sure the new glyphs shows up */
	delay_output();
    }

    You("are standing at the top of a stairwell leading down!");
    mkstairs(u.ux, u.uy, 0, (struct mkroom *)0); /* down */
    newsym(u.ux, u.uy);
    vision_full_recalc = 1;	/* everything changed */

    livelog_write_string("performed the invocation");

#ifdef RECORD_ACHIEVE
    achieve.perform_invocation = 1;
#endif
}

/* Change level topology.  Boulders in the vicinity are eliminated.
 * Temporarily overrides vision in the name of a nice effect.
 */
STATIC_OVL void
mkinvpos(x,y,dist)
xchar x,y;
int dist;
{
    struct trap *ttmp;
    struct obj *otmp;
    boolean make_rocks;
    register struct rm *lev = &levl[x][y];

    /* clip at existing map borders if necessary */
    if (!within_bounded_area(x, y, x_maze_min + 1, y_maze_min + 1,
				   x_maze_max - 1, y_maze_max - 1)) {
	/* only outermost 2 columns and/or rows may be truncated due to edge */
	if (dist < (7 - 2))
	    panic("mkinvpos: <%d,%d> (%d) off map edge!", x, y, dist);
	return;
    }

    /* clear traps */
    if ((ttmp = t_at(x,y)) != 0) deltrap(ttmp);

    /* clear boulders; leave some rocks for non-{moat|trap} locations */
    make_rocks = (dist != 1 && dist != 4 && dist != 5) ? TRUE : FALSE;
    while ((otmp = boulder_at(x, y)) != 0) {
	if (make_rocks) {
	    break_boulder(otmp);
	    make_rocks = FALSE;		/* don't bother with more rocks */
	} else {
	    obj_extract_self(otmp);
	    obfree(otmp, (struct obj *)0);
	}
    }
    unblock_point(x,y);	/* make sure vision knows this location is open */

    /* fake out saved state */
    lev->seenv = 0;
    lev->doormask = 0;
    if(dist < 6) lev->lit = TRUE;
    lev->waslit = TRUE;
    lev->horizontal = FALSE;
    viz_array[y][x] = (dist < 6 ) ?
	(IN_SIGHT|COULD_SEE) : /* short-circuit vision recalc */
	COULD_SEE;

    switch(dist) {
    case 1: /* fire traps */
	if (is_pool(x,y, TRUE)) break;
	lev->typ = ROOM;
	ttmp = maketrap(x, y, FIRE_TRAP);
	if (ttmp) ttmp->tseen = TRUE;
	break;
    case 0: /* lit room locations */
    case 2:
    case 3:
    case 6: /* unlit room locations */
	lev->typ = ROOM;
	break;
    case 4: /* pools (aka a wide moat) */
    case 5:
	lev->typ = MOAT;
	/* No kelp! */
	break;
    default:
	impossible("mkinvpos called with dist %d", dist);
	break;
    }

    /* display new value of position; could have a monster/object on it */
    newsym(x,y);
}

/*
 * The portal to Ludios is special.  The entrance can only occur within a
 * vault in the main dungeon at a depth greater than 10.  The Ludios branch
 * structure reflects this by having a bogus "source" dungeon:  the value
 * of n_dgns (thus, Is_branchlev() will never find it).
 *
 * Ludios will remain isolated until the branch is corrected by this function.
 */
STATIC_OVL void
mk_knox_portal(x, y)
xchar x, y;
{
	extern int n_dgns;		/* from dungeon.c */
	d_level *source;
	branch *br;
	schar u_depth;

	br = dungeon_branch("Fort Ludios");
	if (on_level(&knox_level, &br->end1)) {
	    source = &br->end2;
	} else {
	    /* disallow Knox branch on a level with one branch already */
	    if(Is_branchlev(&u.uz))
		return;
	    source = &br->end1;
	}

	/* Already set -> nope. */
	if (source->dnum < n_dgns) return;

	if (!In_dungeons_of_doom(&u.uz)		// not in main dungeon
		|| (u_depth = depth(&u.uz)) < 10	// not beneath 10
		|| u_depth > depth(&challenge_level)// not below medusa
	)
	    return;

	/* Adjust source to be current level and re-insert branch. */
	*source = u.uz;
	insert_branch(br, TRUE);
	u.uevent.knoxmade = TRUE;

#ifdef DEBUG
	pline("Made knox portal.");
#endif
	place_branch(br, x, y);
}

/*mklev.c*/
