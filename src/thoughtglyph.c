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
	if ((count_glyphs() >= MAX_GLYPHS) ||
		(u.thoughts & thought) ||
		(u.veil) ||
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
	if (u.veil)
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
	if (u.veil)
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
			if (thought == thoughtglyphs[i].thought && thoughtglyphs[i].activate_msg) {
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

int
dofreethought_menu()
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

	Sprintf(buf, "Learn what?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	incntlet = 'a';
	
	if (!(u.thoughts&CLOCKWISE_METAMORPHOSIS)){
		Sprintf(buf, "Increase HP");
		any.a_int = CLOCKWISE_METAMORPHOSIS;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&ANTI_CLOCKWISE_METAMORPHOSIS)){
		Sprintf(buf, "Increase speed");
		any.a_int = ANTI_CLOCKWISE_METAMORPHOSIS;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&ARCANE_BULWARK)){
		Sprintf(buf, "Magic resistance");
		any.a_int = ARCANE_BULWARK;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&DISSIPATING_BULWARK)){
		Sprintf(buf, "Shock resistance");
		any.a_int = DISSIPATING_BULWARK;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&SMOLDERING_BULWARK)){
		Sprintf(buf, "Fire resistance");
		any.a_int = SMOLDERING_BULWARK;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&FROSTED_BULWARK)){
		Sprintf(buf, "Cold resistance");
		any.a_int = FROSTED_BULWARK;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&BLOOD_RAPTURE)){
		Sprintf(buf, "Sneak attack healing");
		any.a_int = BLOOD_RAPTURE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&CLAWMARK)){
		Sprintf(buf, "Stronger sneak attacks");
		any.a_int = CLAWMARK;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&CLEAR_DEEPS)){
		Sprintf(buf, "Poison resistance");
		any.a_int = CLEAR_DEEPS;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&DEEP_SEA)){
		Sprintf(buf, "Damage reduction");
		any.a_int = DEEP_SEA;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&TRANSPARENT_SEA)){
		Sprintf(buf, "Sanity recovery");
		any.a_int = TRANSPARENT_SEA;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&COMMUNION)){
		Sprintf(buf, "Carrying capacity");
		any.a_int = COMMUNION;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&CORRUPTION)){
		Sprintf(buf, "Regeneration at low health");
		any.a_int = CORRUPTION;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&EYE_THOUGHT)){
		Sprintf(buf, "More battle loot");
		any.a_int = EYE_THOUGHT;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&FORMLESS_VOICE)){
		Sprintf(buf, "More magical energy");
		any.a_int = FORMLESS_VOICE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&GUIDANCE)){
		Sprintf(buf, "Counterattack healing");
		any.a_int = GUIDANCE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&IMPURITY)){
		Sprintf(buf, "Damage reduction for pets");
		any.a_int = IMPURITY;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&MOON)){
		Sprintf(buf, "Faster experience gain");
		any.a_int = MOON;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&WRITHE)){
		Sprintf(buf, "Recover energy from sneak attacks");
		any.a_int = WRITHE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	if (!(u.thoughts&RADIANCE)){
		Sprintf(buf, "More magical healing");
		any.a_int = RADIANCE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	
	if (!(u.thoughts&BEASTS_EMBRACE)){
		Sprintf(buf, "More powerful beast claws");
		any.a_int = BEASTS_EMBRACE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	
	if (!(u.thoughts&SIGHT)){
		Sprintf(buf, "Greater accuracy");
		any.a_int = SIGHT;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet++;
	}
	
	end_menu(tmpwin, "Pick thought to learn");

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
dofreethought()
{
	int thoughtID;
	thoughtID = dofreethought_menu();
	if(thoughtID){
		give_thought(thoughtID);
		return TRUE;
	}
	else return FALSE;
}

/*glyph.c*/
