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

/* returns the thought associated with the otyp, if there is one */
long int
otyp_to_thought(otyp)
int otyp;
{
	int i;

	for (i = 0; i < SIZE(thoughtglyphs); i++) {
		if (otyp == thoughtglyphs[i].otyp)
			return thoughtglyphs[i].thought;
	}
	return 0L;
}

/* returns the thought associated with the mtyp, if there is one */
long int
mtyp_to_thought(mtyp)
int mtyp;
{
	int i;

	for (i = 0; i < SIZE(thoughtglyphs); i++) {
		if (mtyp == thoughtglyphs[i].mtyp)
			return thoughtglyphs[i].thought;
	}
	return 0L;
}

/* gives the player the thought IF they meet the insight/san requirements */
/* returns TRUE if it succeeds */
boolean
maybe_give_thought(thought)
long int thought;
{
	if ((count_glyphs() >= 3) ||
		(u.thoughts & thought) ||
		(u.uinsight < glyph_insight(thought)) ||
		(u.usanity > glyph_sanity(thought))
		)
		return FALSE;

	give_thought(thought);
	return TRUE;
}

/* gives the player the thought */
void
give_thought(thought)
long int thought;
{
	u.thoughts |= thought;
	if (active_glyph(thought))
		change_glyph_active(thought, TRUE);
}

void
remove_thought(thought)
long int thought;
{
	if (!(u.thoughts & thought)) {
		impossible("removing thought %ld?", thought);
		return;
	}

	if (active_glyph(thought))
		change_glyph_active(thought, FALSE);
	u.thoughts &= ~thought;
	return;
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
