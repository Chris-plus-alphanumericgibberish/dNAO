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

int
have_glyph(mtmp)
struct monst *mtmp;
{
	switch (monsndx(mtmp->data)){
	case PM_BESTIAL_DERVISH:
		if (u.thoughts & CLOCKWISE_METAMORPHOSIS)
			return 1;
		break;
	case PM_ETHEREAL_DERVISH:
		if (u.thoughts & ANTI_CLOCKWISE_METAMORPHOSIS)
			return 1;
		break;
	case PM_SPARKLING_LAKE:
		if (u.thoughts & ARCANE_BULWARK)
			return 1;
		break;
	case PM_FLASHING_LAKE:
		if (u.thoughts & DISSIPATING_BULWARK)
			return 1;
		break;
	case PM_SMOLDERING_LAKE:
		if (u.thoughts & SMOLDERING_BULWARK)
			return 1;
		break;
	case PM_FROSTED_LAKE:
		if (u.thoughts & FROSTED_BULWARK)
			return 1;
		break;
	case PM_BLOOD_SHOWER:
		if (u.thoughts & BLOOD_RAPTURE)
			return 1;
		break;
	case PM_MANY_TALONED_THING:
		if (u.thoughts & CLAWMARK)
			return 1;
		break;
	case PM_DEEP_BLUE_CUBE:
		if (u.thoughts & CLEAR_DEEPS)
			return 1;
		break;
	case PM_PITCH_BLACK_CUBE:
		if (u.thoughts & DEEP_SEA)
			return 1;
		break;
	case PM_PRAYERFUL_THING:
		if (u.thoughts & COMMUNION)
			return 1;
		break;
	case PM_HEMORRHAGIC_THING:
		if (u.thoughts & CORRUPTION)
			return 1;
		break;
	case PM_MANY_EYED_SEEKER:
		if (u.thoughts & EYE_THOUGHT)
			return 1;
		break;
	case PM_VOICE_IN_THE_DARK:
		if (u.thoughts & FORMLESS_VOICE)
			return 1;
		break;
	case PM_TINY_BEING_OF_LIGHT:
		if (u.thoughts & GUIDANCE)
			return 1;
		break;
	case PM_MAN_FACED_MILLIPEDE:
		if (u.thoughts & IMPURITY)
			return 1;
		break;
	case PM_MIRRORED_MOONFLOWER:
		if (u.thoughts & MOON)
			return 1;
		break;
	case PM_CRIMSON_WRITHER:
		if (u.thoughts & WRITHE)
			return 1;
		break;
	case PM_RADIANT_PYRAMID:
		if (u.thoughts & RADIANCE)
			return 1;
		break;
	default:
		return 0;
		break;
	}
	return 0;
}

int
give_glyph(mtmp)
struct monst *mtmp;
{
	long int glyph;
	switch (monsndx(mtmp->data)){
	case PM_BESTIAL_DERVISH:
		pline("A clockwise gyre forms in your mind.");
		glyph = CLOCKWISE_METAMORPHOSIS;
		break;
	case PM_ETHEREAL_DERVISH:
		pline("An anti-clockwise gyre forms in your mind.");
		glyph = ANTI_CLOCKWISE_METAMORPHOSIS;
		break;
	case PM_SPARKLING_LAKE:
		pline("A great volume of sparkling water pours into your mind.");
		glyph = ARCANE_BULWARK;
		break;
	case PM_FLASHING_LAKE:
		pline("A great volume of pure water pours into your mind.");
		glyph = DISSIPATING_BULWARK;
		break;
	case PM_SMOLDERING_LAKE:
		pline("A great volume of ash-filled water pours into your mind.");
		glyph = SMOLDERING_BULWARK;
		break;
	case PM_FROSTED_LAKE:
		pline("A great volume of freezing water pours into your mind.");
		glyph = FROSTED_BULWARK;
		break;
	case PM_BLOOD_SHOWER:
		pline("A rapturous shower of blood drifts through your mind.");
		glyph = BLOOD_RAPTURE;
		break;
	case PM_MANY_TALONED_THING:
		pline("A many-taloned clawmark is scoured into your mind.");
		glyph = CLAWMARK;
		break;
	case PM_DEEP_BLUE_CUBE:
		pline("A deep sea of blue water fills your mind.");
		glyph = CLEAR_DEEPS;
		break;
	case PM_PITCH_BLACK_CUBE:
		pline("A deep sea of pitch-black water fills your mind.");
		glyph = DEEP_SEA;
		break;
	case PM_PRAYERFUL_THING:
		pline("A strange minister's prayer echoes in you mind.");
		glyph = COMMUNION;
		break;
	case PM_HEMORRHAGIC_THING:
		pline("Thoughts of weeping form in your mind.");
		glyph = CORRUPTION;
		break;
	case PM_MANY_EYED_SEEKER:
		pline("A seeking eye opens in your mind.");
		glyph = EYE_THOUGHT;
		break;
	case PM_VOICE_IN_THE_DARK:
		pline("The voice of a formless thing speaks in your mind.");
		glyph = FORMLESS_VOICE;
		break;
	case PM_TINY_BEING_OF_LIGHT:
		pline("You see tiny spirits dancing in the nothing behind your eyes.");
		glyph = GUIDANCE;
		break;
	case PM_MAN_FACED_MILLIPEDE:
		pline("Vermin writhe in the filth inside your head.");
		glyph = IMPURITY;
		break;
	case PM_MIRRORED_MOONFLOWER:
		pline("A reflection of the sympathetic moon fills your mind.");
		glyph = MOON;
		break;
	case PM_CRIMSON_WRITHER:
		pline("A subtle mucus is revealed in the blood in your brain.");
		glyph = WRITHE;
		break;
	case PM_RADIANT_PYRAMID:
		pline("An irregular golden pyramid rises from the depths of your mind.");
		glyph = RADIANCE;
		break;
	default:
		impossible("MS_GLYPH monster with no valid glyph!");
		return 1;
		break;
	}
	u.thoughts |= glyph;
	if (active_glyph(glyph))
		change_glyph_active(glyph, TRUE);
	return 0;
}


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
glyph_sanity(thought)
long int thought;
{
	/* disabled. */
	return 1000;

	int sanlevel = 1000;
	switch (thought){
	case ANTI_CLOCKWISE_METAMORPHOSIS:
		break;
	case CLOCKWISE_METAMORPHOSIS:
		break;
	case ARCANE_BULWARK:
		break;
	case DISSIPATING_BULWARK:
		break;
	case SMOLDERING_BULWARK:
		break;
	case FROSTED_BULWARK:
		break;
	case BLOOD_RAPTURE:
		sanlevel = 90;
		break;
	case CLAWMARK:
		sanlevel = 90;
		break;
	case CLEAR_DEEPS:
		break;
	case DEEP_SEA:
		break;
	case COMMUNION:
		break;
	case CORRUPTION:
		sanlevel = 80;
		break;
	case EYE_THOUGHT:
		break;
	case FORMLESS_VOICE:
		break;
	case GUIDANCE:
		break;
	case IMPURITY:
		sanlevel = 80;
		break;
	case MOON:
		break;
	case WRITHE:
		sanlevel = 90;
		break;
	case RADIANCE:
		break;
	case BEASTS_EMBRACE:
		sanlevel = 90;
		break;
	default:
		impossible("bad glyph %ld in active_glyph!", thought);
		return 0;
		break;
	}
	return sanlevel;
}

int
glyph_insight(thought)
long int thought;
{
	int insightlevel = 0;
	switch (thought){
	case ANTI_CLOCKWISE_METAMORPHOSIS:
		insightlevel = 20;
		break;
	case CLOCKWISE_METAMORPHOSIS:
		insightlevel = 20;
		break;
	case ARCANE_BULWARK:
		insightlevel = 18;
		break;
	case DISSIPATING_BULWARK:
		insightlevel = 16;
		break;
	case SMOLDERING_BULWARK:
		insightlevel = 11;
		break;
	case FROSTED_BULWARK:
		insightlevel = 12;
		break;
	case BLOOD_RAPTURE:
		insightlevel = 14;
		break;
	case CLAWMARK:
		insightlevel = 16;
		break;
	case CLEAR_DEEPS:
		insightlevel = 10;
		break;
	case DEEP_SEA:
		insightlevel = 22;
		break;
	case COMMUNION:
		insightlevel = 25;
		break;
	case CORRUPTION:
		insightlevel = 15;
		break;
	case EYE_THOUGHT:
		insightlevel = 17;
		break;
	case FORMLESS_VOICE:
		insightlevel = 19;
		break;
	case GUIDANCE:
		insightlevel = 13;
		break;
	case IMPURITY:
		insightlevel = 5;
		break;
	case MOON:
		insightlevel = 10;
		break;
	case WRITHE:
		insightlevel = 14;
		break;
	case RADIANCE:
		insightlevel = 12;
		break;
	case BEASTS_EMBRACE:
		insightlevel = 15;
		break;
	default:
		impossible("bad glyph %ld in active_glyph!", thought);
		return 0;
		break;
	}
	return insightlevel;
}

/* returns TRUE if <thought> is currently active */
int
active_glyph(thought)
long int thought;
{
	int insightlevel = 0, sanlevel = 1000;
	if (!(u.thoughts&thought))
		return 0;
	insightlevel = glyph_insight(thought);
	sanlevel = glyph_sanity(thought);
	if (u.uinsight >= insightlevel && u.usanity <= sanlevel)
		return 1;
	else return 0;
	return 0;
}
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
/* */
void
change_glyph_active(thought, on)
long int thought;
boolean on;	/* TRUE if activating, FALSE if deactivating */
{
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


/*glyph.c*/
