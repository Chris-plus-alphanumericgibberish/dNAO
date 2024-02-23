/*	SCCS Id: @(#)vision.h	3.4	1995/01/26	*/
/* Copyright (c) Dean Luick, with acknowledgements to Dave Cohrs, 1990. */
/* NetHack may be freely redistributed.  See license for details.	*/

#ifndef VISION_H
#define VISION_H

#if 0	/* (moved to decl.h) */
extern boolean vision_full_recalc;	/* TRUE if need vision recalc */
extern char **viz_array;		/* could see/in sight row pointers */
extern char *viz_rmin;			/* min could see indices */
extern char *viz_rmax;			/* max could see indices */
#endif
/*Note: these are stored in char arrays*/
#define COULD_SEE  0x01		/* location could be seen, if it were lit */
#define IN_SIGHT   0x02		/* location can be seen */
#define TEMP_LIT1  0x04		/* location is temporarily lit (regular) */
#define TEMP_LIT2  0x08		/* location is temporarily lit (lowlight 2) */
#define TEMP_LIT3  0x10		/* location is temporarily lit (lowlight 3) */
#define TEMP_DRK1  0x20		/* location is temporarily dark (regular) */
#define TEMP_DRK2  0x40		/* location is temporarily dark (lowlight 2) */
#define TEMP_DRK3  0x80		/* location is temporarily dark (lowlight 3) */

#define TEMP_LITMASK	(TEMP_LIT1|TEMP_LIT2|TEMP_LIT3)
#define TEMP_DRKMASK	(TEMP_DRK1|TEMP_DRK2|TEMP_DRK3)

#define TEMP_LITVAL(f)	(!!(f&TEMP_LIT1)+!!(f&TEMP_LIT2)+!!(f&TEMP_LIT3))
#define TEMP_DRKVAL(f)	(!!(f&TEMP_DRK1)+!!(f&TEMP_DRK2)+!!(f&TEMP_DRK3))

#define TEMP_LIT  TEMP_LIT1	/* location is temporarily lit */
#define TEMP_DRK  TEMP_DRK1	/* location is temporarily lit */

/*
 * Light source sources
 */
#define LS_OBJECT 0
#define LS_MONSTER 1

/*
 *  cansee()	- Returns true if the hero can see the location.
 *
 *  couldsee()	- Returns true if the hero has a clear line of sight to
 *		  the location.
 */
#define cansee(x,y)	(viz_array[y][x] & IN_SIGHT)
#define couldsee(x,y)	(viz_array[y][x] & COULD_SEE)
/* dimness function: if nvrange > dimness(x,y), square can be seen */
#define dimness(x,y)	(3 + TEMP_DRKVAL(viz_array[y][x]) - TEMP_LITVAL(viz_array[y][x]) \
							- levl[x][y].lit*(!(viz_array[y][x]&TEMP_DRKMASK) ? 3 : 1))

#define isdark(x,y)	( (!levl[x][y].lit && !(viz_array[y][x] & TEMP_LIT1 && !(viz_array[y][x] & TEMP_DRK1))) ||\
					   (levl[x][y].lit &&  (viz_array[y][x] & TEMP_DRK1 && !(viz_array[y][x] & TEMP_LIT1))))

#define unshadowed_square(x,y)	((levl[x][y].lit && !(viz_array[y][x]&TEMP_DRKMASK)) || (!levl[x][y].lit && !(viz_array[y][x]&TEMP_LITMASK)))

#define xrayrange()	((Xray_vision ? 3 : -1) + ((Role_if(PM_MONK) && ublindf && ublindf->oartifact == ART_EYE_OF_THE_OVERWORLD) ? ((u.ulevel+1)/10) : 0)+ (u.sealsActive&SEAL_ORTHOS ? spiritDsize() + 1 : 0))
#define xraydist() (xrayrange() < 0 ? -1 : xrayrange()*xrayrange())

/*
 *  The following assume the monster is not blind.
 *
 *  m_cansee()	- Returns true if the monster can see the given location.
 *
 *  m_canseeu() - Returns true if the monster could see the hero.  Assumes
 *		  that if the hero has a clear line of sight to the monster's
 *		  location and the hero is visible, then monster can see the
 *		  hero.
 */
#define m_cansee(mtmp,x2,y2)	clear_path((mtmp)->mx,(mtmp)->my,(x2),(y2))

#define m_canseeu(m)	((!Invis || mon_resistance((m),SEE_INVIS)) && \
			  !(Underwater || u.uburied || (m)->mburied) ? \
			     couldsee((m)->mx,(m)->my) : 0)

/*
 *  Circle information
 */
#define MAX_RADIUS 30	/* this is in points from the source */

/* Use this macro to get a list of distances of the edges (see vision.c). */
#define circle_ptr(z) (&circle_data[(int)circle_start[z]])

#endif /* VISION_H */
