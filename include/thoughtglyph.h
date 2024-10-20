/*	SCCS Id: @(#)thoughtglyph.h	3.4	1997/05/01	*/
/* Copyright (c) Izchak Miller, 1989.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef THOUGHTGLYPH_H
#define THOUGHTGLYPH_H

struct thoughtglyph {
	int otyp;
	int mtyp;
	long int thought;
	int insight_level;
	int sanity_level;
	const char * activate_msg;
};

/* thoughts */
#define	ANTI_CLOCKWISE_METAMORPHOSIS	0x0000000000000001L
#define	CLOCKWISE_METAMORPHOSIS			0x0000000000000002L
#define	ARCANE_BULWARK					0x0000000000000004L
#define	DISSIPATING_BULWARK				0x0000000000000008L
#define	SMOLDERING_BULWARK				0x0000000000000010L
#define	FROSTED_BULWARK					0x0000000000000020L
#define	BLOOD_RAPTURE					0x0000000000000040L
#define	CLAWMARK						0x0000000000000080L
#define	CLEAR_DEEPS						0x0000000000000100L
#define	DEEP_SEA						0x0000000000000200L
#define	TRANSPARENT_SEA					0x0000000000000400L
#define	COMMUNION						0x0000000000000800L
#define	CORRUPTION						0x0000000000001000L
#define	EYE_THOUGHT						0x0000000000002000L
#define	FORMLESS_VOICE					0x0000000000004000L
#define	GUIDANCE						0x0000000000008000L
#define	IMPURITY						0x0000000000010000L
#define	MOON							0x0000000000020000L
#define	WRITHE							0x0000000000040000L
#define	RADIANCE						0x0000000000080000L
#define	BEASTS_EMBRACE					0x0000000000100000L
#define	DEFILEMENT						0x0000000000200000L
#define	LUMEN							0x0000000000400000L
#define	ROTTEN_EYES						0x0000000000800000L
#define	SIGHT							0x0000000001000000L

#define FIRST_GLYPH	ANTI_CLOCKWISE_METAMORPHOSIS_G
static struct thoughtglyph thoughtglyphs[] =
{
	{ ANTI_CLOCKWISE_METAMORPHOSIS_G, PM_ETHEREAL_DERVISH, ANTI_CLOCKWISE_METAMORPHOSIS,
	20, 100, "An anti-clockwise gyre forms in your mind." },

	{ CLOCKWISE_METAMORPHOSIS_GLYPH, PM_BESTIAL_DERVISH, CLOCKWISE_METAMORPHOSIS,
	20, 100, "A clockwise gyre forms in your mind." },

	{ SPARKLING_LAKE_GLYPH, PM_SPARKLING_LAKE, ARCANE_BULWARK,
	18, 100, "A great volume of sparkling water pours into your mind." },

	{ FADING_LAKE_GLYPH, PM_FLASHING_LAKE, DISSIPATING_BULWARK,
	16, 100, "A great volume of pure water pours into your mind." },

	{ SMOKING_LAKE_GLYPH, PM_SMOLDERING_LAKE, SMOLDERING_BULWARK,
	11, 100, "A great volume of ash-filled water pours into your mind." },

	{ FROSTED_LAKE_GLYPH, PM_FROSTED_LAKE, FROSTED_BULWARK,
	12, 100, "A great volume of freezing water pours into your mind." },

	{ RAPTUROUS_EYE_GLYPH, PM_BLOOD_SHOWER, BLOOD_RAPTURE,
	14, 90, "A rapturous shower of blood drifts into your mind." },

	{ CLAWMARK_GLYPH, PM_MANY_TALONED_THING, CLAWMARK,
	16, 90, "A many-taloned clawmark is scoured into your mind." },

	{ CLEAR_SEA_GLYPH, PM_DEEP_BLUE_CUBE, CLEAR_DEEPS,
	10, 100, "A deep sea of blue water fills your mind." },

	{ DEEP_SEA_GLYPH, PM_PITCH_BLACK_CUBE, DEEP_SEA,
	22, 100, "A deep sea of pitch-black water fills your mind." },

	{ HIDDEN_SEA_GLYPH, PM_PERFECTLY_CLEAR_CUBE, TRANSPARENT_SEA,
	30, 100, "You suddenly notice a perfectly clear sea around your mind." },

	{ COMMUNION_GLYPH, PM_PRAYERFUL_THING, COMMUNION,
	25, 100, "A strange minister's prayer echoes in your mind." },

	{ CORRUPTION_GLYPH, PM_HEMORRHAGIC_THING, CORRUPTION,
	15, 80, "Thoughts of weeping form in your mind." },

	{ EYE_GLYPH, PM_MANY_EYED_SEEKER, EYE_THOUGHT,
	17, 100, "A seeking eye opens in your mind." },

	{ FORMLESS_VOICE_GLYPH, PM_VOICE_IN_THE_DARK, FORMLESS_VOICE,
	19, 100, "The voice of a formless thing speaks in your mind." },

	{ GUIDANCE_GLYPH, PM_TINY_BEING_OF_LIGHT, GUIDANCE,
	13, 100, "You see tiny spirits dancing in the nothing behind your eyes." },

	{ IMPURITY_GLYPH, PM_MAN_FACED_MILLIPEDE, IMPURITY,
	5, 80, "Vermin writhe in the filth inside your head." },

	{ MOON_GLYPH, PM_MIRRORED_MOONFLOWER, MOON,
	10, 100, "A reflection of the sympathetic moon fills your mind." },

	{ WRITHE_GLYPH, PM_CRIMSON_WRITHER, WRITHE,
	14, 90, "A subtle mucus is revealed in the blood in your brain." },

	{ RADIANCE_GLYPH, PM_RADIANT_PYRAMID, RADIANCE,
	12, 100, "An irregular golden pyramid rises from the depths of your mind." },

	{ BEAST_S_EMBRACE_GLYPH, NON_PM, BEASTS_EMBRACE,
	0, 99, "A bestial figure takes refuge inside you." },

	{ DEFILEMENT_GLYPH, NON_PM, DEFILEMENT,
	27, 100, "A twisted bloodless hand rises from the mire of your soul." },

	{ WEED_GLYPH, NON_PM, LUMEN,
	27, 100, "Tightly packed florets burst from your skull." },

	{ VACUOUS_GLYPH, NON_PM, ROTTEN_EYES,
	27, 100, "Great jolts of lightning rot your brain into milky mushy eyes." },

	{ ORRERY_GLYPH, PM_GROTESQUE_PEEPER, SIGHT,
	16, 100, "Your brain blinks open. There is an eye within an eye within an eye within an...." },
};
#define LAST_GLYPH	ORRERY_GLYPH





#endif /* THOUGHTGLYPH_H */
