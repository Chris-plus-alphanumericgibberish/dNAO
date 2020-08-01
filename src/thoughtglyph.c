/*	SCCS Id: @(#)thoughtglyph.c	3.4	1990/02/22	*/
/* Copyright (c) 1990 by Jean-Christophe Collet	 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

int
count_glyphs(){
	int i, count = 0;
	for (i = 0; i<32; i++){
		if (((u.thoughts >> i) & 1L) == 1L) count++;
	}
	return count;
}

/* returns TRUE if the player has the thought that mtmp gives */
boolean
have_glyph(mtmp)
struct monst *mtmp;
{
	int i;

	if (!mtmp)
		return FALSE;

	for (i = 0; i < SIZE(thoughtglyphs); i++) {
		if (mtmp->mtyp == thoughtglyphs[i].mtyp)
			return !!(u.thoughts & thoughtglyphs[i].thought);
	}
	return FALSE;
}

/* gives the player the thought granted by mtmp */
/* returns TRUE if it succeeded */
boolean
give_glyph(mtmp)
struct monst *mtmp;
{
	int i;
	long int glyph = 0L;

	if (!mtmp)
		return FALSE;

	for (i = 0; i < SIZE(thoughtglyphs); i++) {
		if (mtmp->mtyp == thoughtglyphs[i].mtyp) {
			glyph = thoughtglyphs[i].thought;
			break;
		}
	}
	if (!glyph) {
		impossible("MS_GLYPH monster with no valid glyph!");
		return FALSE;
	}

	u.thoughts |= glyph;
	if (active_glyph(glyph))
		change_glyph_active(glyph, TRUE);
	return TRUE;
}

int
glyph_sanity(thought)
long int thought;
{
	/* disabled. */
	return 1000;

	int i;
	for (i = 0; i < SIZE(thoughtglyphs); i++) {
		if (thought == thoughtglyphs[i].thought)
			return thoughtglyphs[i].sanity_level;
	}

	impossible("bad glyph %ld in glyph_sanity!", thought);
	return 0;
}

int
glyph_insight(thought)
long int thought;
{
	int i;
	for (i = 0; i < SIZE(thoughtglyphs); i++) {
		if (thought == thoughtglyphs[i].thought)
			return thoughtglyphs[i].insight_level;
	}

	impossible("bad glyph %ld in glyph_insight!", thought);
	return 0;
}

/* returns TRUE if <thought> is currently active */
boolean
active_glyph(thought)
long int thought;
{
	if (!(u.thoughts&thought))
		return FALSE;
	if (u.uinsight >= glyph_insight(thought) && u.usanity <= glyph_sanity(thought))
		return TRUE;
	return FALSE;
}
/* returns TRUE if <thought> would have been active at old insight/sanity values */
boolean
was_active_glyph(thought, oldinsight, oldsanity)
long int thought;
int oldinsight;
int oldsanity;
{
	if (!(u.thoughts&thought))
		return FALSE;
	if (oldinsight >= glyph_insight(thought) && oldsanity <= glyph_sanity(thought))
		return TRUE;
	return FALSE;
}

/* perform all tasks when activating/deactivating <thought> */
void
change_glyph_active(thought, on)
long int thought;
boolean on;	/* TRUE if activating, FALSE if deactivating */
{
	/* print activation message */
	if (on) {
		int i;
		for (i = 0; i < SIZE(thoughtglyphs); i++) {
			if (thought == thoughtglyphs[i].thought) {
				pline1(thoughtglyphs[i].activate_msg);
				break;
			}
		}
	}
	else {
		/* no deactivation message at this time */
	}

#define toggle_extrinsic(prop) do{if(on) u.uprops[(prop)].extrinsic |= W_GLYPH; else u.uprops[(prop)].extrinsic &= ~W_GLYPH;}while(0)
	switch (thought)
	{
	case ARCANE_BULWARK:
		toggle_extrinsic(ANTIMAGIC);
		break;
	case DISSIPATING_BULWARK:
		toggle_extrinsic(SHOCK_RES);
		break;
	case SMOLDERING_BULWARK:
		toggle_extrinsic(FIRE_RES);
		break;
	case FROSTED_BULWARK:
		toggle_extrinsic(COLD_RES);
		break;
	case CLEAR_DEEPS:
		toggle_extrinsic(POISON_RES);
		break;
	default:
		/* nothing needed */
		break;
	}
#undef toggle_extrinsic
}


/* does the activated effect of the guidance glyph */
void
doguidance(mdef, dmg)
struct monst *mdef;
int dmg;
{
	if (mdef && mdef->mattackedu) {
		int life = (int)(dmg*0.2 + 1);
		healup(life, 0, FALSE, FALSE);
	}
}


/*glyph.c*/
