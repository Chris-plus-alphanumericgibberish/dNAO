/*	SCCS Id: @(#)prop.c	3.4	2000/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

boolean
uprop(prop)
int prop;
{
	return mprop(&youmonst, prop);
}

boolean
mprop(mon, prop)
struct monst * mon;
int prop;
{
	boolean you = (mon == &youmonst);
	boolean intrinsic = FALSE;
	boolean extrinsic = FALSE;
	boolean blocked = FALSE;
	
	/* active intrinsics */
	intrinsic = (you ? u.uprops[prop].intrinsic : (((mon)->mintrinsics[((typ)-1) / 32] & (0x1L << ((typ)-1) % 32)) != 0));
	/* active extrinsics */
	extrinsic = (you ? u.uprops[prop].extrinsic : (((mon)->mextrinsics[((typ)-1) / 32] & (0x1L << ((typ)-1) % 32)) != 0));
	/* blocked properties */
	blocked = (you ? u.uprops[prop].blocked : FALSE);	/* monsters don't have blocked properties... yet */

	/* property special cases */
	switch (prop)
	{
	case LEVITATION:
		/* the player uses I_SPECIAL bit to decide if levitation is controllable; does not grant it itself */
		if (you)
			intrinsic = (u.uprops[prop].intrinsic & ~I_SPECIAL);
		break;
	}
}



/*prop.c*/
