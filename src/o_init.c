/*	SCCS Id: @(#)o_init.c	3.4	1999/12/09	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"	/* save & restore info */

STATIC_DCL void FDECL(setgemprobs, (d_level*));
STATIC_DCL void FDECL(shuffle,(int,int,BOOLEAN_P));
STATIC_DCL void NDECL(shuffle_all);
STATIC_DCL int FDECL(find_otyp_of_desc, (const char *, const char **, int, int, boolean(*)(int)));
STATIC_DCL void NDECL(randomize_nonmatwands);
STATIC_DCL boolean FDECL(interesting_to_discover,(int));


static NEARDATA short disco[NUM_OBJECTS] = DUMMY;

#ifdef USE_TILES
STATIC_DCL void NDECL(shuffle_tiles);
extern short glyph2tile[];	/* from tile.c */

/* Shuffle tile assignments to match descriptions, so a red potion isn't
 * displayed with a blue tile and so on.
 *
 * Tile assignments are not saved, and shouldn't be so that a game can
 * be resumed on an otherwise identical non-tile-using binary, so we have
 * to reshuffle the assignments from oc_descr_idx information when a game
 * is restored.  So might as well do that the first time instead of writing
 * another routine.
 */
STATIC_OVL void
shuffle_tiles()
{
	int i;
	short tmp_tilemap[NUM_OBJECTS];

	for (i = 0; i < NUM_OBJECTS; i++)
		tmp_tilemap[i] =
			glyph2tile[objects[i].oc_descr_idx + GLYPH_OBJ_OFF];

	for (i = 0; i < NUM_OBJECTS; i++)
		glyph2tile[i + GLYPH_OBJ_OFF] = tmp_tilemap[i];
}
#endif	/* USE_TILES */

STATIC_OVL void
setgemprobs(dlev)
d_level *dlev;
{
	int j, first, lev, totalprob=0;

	if (dlev)
	    lev = (ledger_no(dlev) > maxledgerno())
				? maxledgerno() : ledger_no(dlev);
	else
	    lev = 0;
	first = bases[GEM_CLASS];

	for(j = 0; j < 11-lev/3; j++){
		totalprob += objects[first+j].oc_prob;
		objects[first+j].oc_prob = 0;
	}/*j=0;*/ //turned off gem limitation by dungeon level. Then turned it back on.
	first += j;
	if (first > LAST_GEM || objects[first].oc_class != GEM_CLASS ||
	    OBJ_NAME(objects[first]) == (char *)0) {
		raw_printf("Not enough gems? - level=%d first=%d j=%d LAST_GEM=%d",
			lev, first, j, LAST_GEM);
		wait_synch();
	    }
	//Note, 175 is the sum of the oc_probs for the true gems
	for (j = first; j <= LAST_GEM; j++)
		objects[j].oc_prob = (175+j-first)/(LAST_GEM+1-first);
}

/* shuffle descriptions on objects o_low to o_high */
STATIC_OVL void
shuffle(o_low, o_high, domaterial)
	int o_low, o_high;
	boolean domaterial;
{
	int i, j, num_to_shuffle;
	short sw;
	int color;

	for (num_to_shuffle = 0, j=o_low; j <= o_high; j++)
		if (!objects[j].oc_name_known) num_to_shuffle++;
	if (num_to_shuffle < 2) return;

	for (j=o_low; j <= o_high; j++) {
		if (objects[j].oc_name_known) continue;
		do
			i = j + rn2(o_high-j+1);
		while (objects[i].oc_name_known);
		sw = objects[j].oc_descr_idx;
		objects[j].oc_descr_idx = objects[i].oc_descr_idx;
		objects[i].oc_descr_idx = sw;
		sw = objects[j].oc_tough;
		objects[j].oc_tough = objects[i].oc_tough;
		objects[i].oc_tough = sw;
		color = objects[j].oc_color;
		objects[j].oc_color = objects[i].oc_color;
		objects[i].oc_color = color;

		/* shuffle material */
		if (domaterial) {
			sw = objects[j].oc_material;
			objects[j].oc_material = objects[i].oc_material;
			objects[i].oc_material = sw;
		}
	}
}

void
init_objects()
{
register int i, first, last, sum;
register char oclass;
	/* bug fix to prevent "initialization error" abort on Intel Xenix.
	 * reported by mikew@semike
	 */
	for (i = 0; i < MAXOCLASSES; i++)
		bases[i] = 0;
	/* initialize object descriptions */
	for (i = 0; i < NUM_OBJECTS; i++)
		objects[i].oc_name_idx = objects[i].oc_descr_idx = i;
	/* init base; if probs given check that they add up to 1000,
	   otherwise compute probs */
	first = 0;
	while( first < NUM_OBJECTS ) {
		oclass = objects[first].oc_class;
		last = first+1;
		while (last < NUM_OBJECTS && objects[last].oc_class == oclass) last++;
		bases[(int)oclass] = first;

		if (oclass == GEM_CLASS) {
			setgemprobs((d_level *)0);

			if (rn2(2)) { /* change turquoise from green to blue? */
			    COPY_OBJ_DESCR(objects[TURQUOISE],objects[SAPPHIRE]);
			}
			if (rn2(2)) { /* change aquamarine from green to blue? */
			    COPY_OBJ_DESCR(objects[AQUAMARINE],objects[SAPPHIRE]);
			}
			// switch (rn2(4)) { /* change fluorite from violet? */
			    // case 0:  break;
			    // case 1:	/* blue */
				// COPY_OBJ_DESCR(objects[FLUORITE],objects[SAPPHIRE]);
				// break;
			    // case 2:	/* white */
				// COPY_OBJ_DESCR(objects[FLUORITE],objects[DIAMOND]);
				// break;
			    // case 3:	/* green */
				// COPY_OBJ_DESCR(objects[FLUORITE],objects[EMERALD]);
				// break;
			// }
		}
	check:
		sum = 0;
		for(i = first; i < last; i++) sum += objects[i].oc_prob;
		if(sum == 0) {
			if(objects[first].oc_class == RING_CLASS)
				first++;//Ring of wishes should remain 0
			for(i = first; i < last; i++)
			    objects[i].oc_prob = (1000+i-first)/(last-first);
			goto check;
		}
		if(sum != 1000)
			error("init-prob error for class %d (%d%%)", oclass, sum);
		first = last;
	}
	/* shuffle descriptions */
	shuffle_all();
#ifdef USE_TILES
	shuffle_tiles();
#endif
}

STATIC_OVL void
shuffle_all()
{
	int first, last, oclass, signetring;

	for (oclass = 1; oclass < MAXOCLASSES; oclass++) {
		first = bases[oclass];
		last = first+1;
		while (last < NUM_OBJECTS && objects[last].oc_class == oclass)
			last++;

		if (OBJ_DESCR(objects[first]) != (char *)0 &&
				oclass != TOOL_CLASS &&
				oclass != WEAPON_CLASS &&
				oclass != ARMOR_CLASS &&
				oclass != SCOIN_CLASS &&
				oclass != GEM_CLASS) {
			int j = last-1;

			if (oclass == POTION_CLASS)
			    j -= 6;  /* goat's milk, space mead, water, starlight, amnesia, and blood have fixed descriptions */
			if (oclass == RING_CLASS)
				first += 1;	/* three wishes has a fixed description */
			if (oclass == WAND_CLASS)
			    first += 3;  /* light, darkness, and wishing have fixed descriptions */
			else if (oclass == AMULET_CLASS ||
				 oclass == TILE_CLASS ||
				 oclass == SPBOOK_CLASS
			){
			    while (!objects[j].oc_magic || objects[j].oc_unique || j == SPE_SECRETS)
				j--;
			}
			else if (oclass == SCROLL_CLASS){
				j=first;
			    while (objects[j].oc_magic) //Assumes blank scroll is the sentential
					j++;
				j--;
			}

			/* non-magical amulets, scrolls, and spellbooks
			 * (ex. imitation Amulets, blank, scrolls of mail)
			 * and one-of-a-kind magical artifacts at the end of
			 * their class in objects[] have fixed descriptions.
			 */
			shuffle(first, j, TRUE);
		}
	}
	
	/* check signet ring */
	
	signetring = find_signet_ring();
	while(signetring == RIN_LEVITATION ||
	   signetring== RIN_AGGRAVATE_MONSTER ||
	   signetring== RIN_HUNGER ||
	   signetring== RIN_POLYMORPH ||
	   signetring== RIN_POLYMORPH_CONTROL
	){
		shuffle(RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, TRUE);
		signetring = find_signet_ring();
	}
	
	/* shuffle the helmets */
	shuffle(HELMET, HELM_OF_DRAIN_RESISTANCE, TRUE);
	
	/* shuffle the gloves */
	shuffle(GLOVES, GAUNTLETS_OF_DEXTERITY, FALSE);

	/* shuffle the cloaks */
	shuffle(CLOAK_OF_PROTECTION, CLOAK_OF_DISPLACEMENT, FALSE);

	/* shuffle the boots [if they change, update find_skates() below] */
	shuffle(SPEED_BOOTS, FLYING_BOOTS, FALSE);
}

/* called on init and restore
 * modify some objects
 */
void
hack_objects()
{
	/* Fix up the crown */
	switch (find_gcirclet())
	{
	case HELMET:
		obj_descr[HELMET].oc_name = "circlet";
		break;
	case HELM_OF_BRILLIANCE:
		obj_descr[HELM_OF_BRILLIANCE].oc_name = "crown of cognizance";
		break;
	case HELM_OF_OPPOSITE_ALIGNMENT:
		obj_descr[HELM_OF_OPPOSITE_ALIGNMENT].oc_name = "tiara of treachery";
		break;
	case HELM_OF_TELEPATHY:
		obj_descr[HELM_OF_TELEPATHY].oc_name = "tiara of telepathy";
		break;
	case HELM_OF_DRAIN_RESISTANCE:
		obj_descr[HELM_OF_DRAIN_RESISTANCE].oc_name = "diadem of drain resistance";
		break;
	}
}

/* finds the object index for an item whose description matches str (first) or any of strs[],
 * within the given bounds of the objects array 
 * Caller is responsible for storing returned otyp.
 */
/* finds the object index for an item whose description matches str (first) or any of strs[],
* within the given bounds of the objects array
* Caller is responsible for storing returned otyp.
*/
int
find_otyp_of_desc(str, strs, lbound, ubound, extracondition)
const char * str;
const char ** strs;
int lbound;
int ubound;
boolean FDECL((*extracondition), (int));	/* extracondition CANNOT call find_otyp_of_desc() */
{
	register int i;
	register int j;
	register const char *s;
	const char * curstr;

	j = -1;
	/* for each string in our list of strings (or just str if onlyone==TRUE) */
	do {
		curstr = (j == -1 ? str : strs[j]);

		/* from lbound of objects array to ubound, */
		for (i = lbound; i <= ubound; i++)
		{
			/* if description matches, */
			if ((s = OBJ_DESCR(objects[i])) != 0 && (
				!strcmp(s, curstr)
				))
			{
				/* if that's all we're looking for OR it matches our additional criteria, */
				if (!extracondition || extracondition(i))
					return i;	/* return it */
				else
					break;	/* stop checking strs[j] against objects[i]; increment i. */
			}
		}
	} while (strs && strs[++j]);

	/* did not find it */
	/* caller should raise impossible */
	return -1;
}

/* list of non-material-ed wands */
/* order can and will be randomized during runtime (terminator excluded) */
const char * matNeutralWands[] = {
	"hexagonal",
	"short",
	"runed",
	"long",
	"curved",
	"forked",
	"spiked",
	(const char *)0
};
#define NUM_NONMAT_WANDS 7
/* randomizes order of non-material-ed wands list */
void
randomize_nonmatwands()
{
	int i;
	int j;
	const char * t;
	for (i = 0; i < NUM_NONMAT_WANDS - 1; i++)
	{
		j = i + rand() / (RAND_MAX / (NUM_NONMAT_WANDS - i) + 1);
		t = matNeutralWands[j];
		matNeutralWands[j] = matNeutralWands[i];
		matNeutralWands[i] = t;
	}
	return;
}

/* find the object index for silver attack wand */
int
find_sawant()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	randomize_nonmatwands();
	if ((i = find_otyp_of_desc("silver", matNeutralWands, WAN_LIGHT, WAN_LIGHTNING, is_attack_wand)) != -1)
		return i;
	else
		impossible("could not find silver attack wand");
	return 0;
}

/* find the object index for gold attack wand */
int
find_gawant()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	randomize_nonmatwands();
	if ((i = find_otyp_of_desc("gold", matNeutralWands, WAN_LIGHT, WAN_LIGHTNING, is_attack_wand)) != -1)
		return i;
	else
		impossible("could not find gold attack wand");
	return 0;
}

/* find the object index for platinum attack wand */
int
find_pawant()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	randomize_nonmatwands();
	if ((i = find_otyp_of_desc("platinum", matNeutralWands, WAN_LIGHT, WAN_LIGHTNING, is_attack_wand)) != -1)
		return i;
	else
		impossible("could not find platinum attack wand");
	return 0;
}

/* find the object index for platinum wand */
int
find_riwant()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	randomize_nonmatwands();
	if ((i = find_otyp_of_desc("iron", matNeutralWands, WAN_LIGHT, WAN_LIGHTNING, 0)) != -1)
		return i;
	else
		impossible("could not find iron wand");
	return 0;
}

/* find the object index for (gold) circlet */
int
find_gcirclet()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("band", 0, HELMET, HELM_OF_DRAIN_RESISTANCE, 0)) != -1)
		return i;
	else
		impossible("could not find circlet");
	return 0;
}

/* find the object index for silver ring */
int
find_sring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("silver", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find silver ring");
	return 0;
}

/* find the object index for iron ring */
int
find_iring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("iron", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find iron ring");
	return 0;
}

/* find the object index for a non-polymorph iron ring */
int
find_good_iring()
{
	static int i = -1;
    register const char *s;
	if (i != -1) return i;
	
	/* maybe use iron ring */
	if ((i = find_iring()) && i != RIN_POLYMORPH)
		return i;
	/* if we don't like the iron ring, use "twisted" */
	if ((i = find_otyp_of_desc("twisted", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find twisted ring");
    return 0;
}

/* find the object index for a non-polymorph granite or gold ring */
int
find_good_dring()
{
	static int i = -1;
    register const char *s;
	if (i != -1) return i;
	
	/* maybe use granite ring */
	if ((i = find_otyp_of_desc("granite", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1 && i != RIN_POLYMORPH)
		return i;
	/* if we don't like the iron ring, use "gold" */
	if ((i = find_otyp_of_desc("gold", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find gold ring");
    return 0;
}

/* find the object index for a non-polymorph ruby or gold ring */
int
find_good_fring()
{
	static int i = -1;
    register const char *s;
	if (i != -1) return i;
	
	/* maybe use ruby ring */
	if ((i = find_otyp_of_desc("ruby", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1 && i != RIN_POLYMORPH)
		return i;
	/* if we don't like the iron ring, use "gold" */
	if ((i = find_otyp_of_desc("gold", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find gold ring");
    return 0;
}

/* find the object index for a non-polymorph iron ring */
int
find_good_wring()
{
	static int i = -1;
    register const char *s;
	if (i != -1) return i;
	
	/* maybe use diamond ring */
	if ((i = find_otyp_of_desc("diamond", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1 && i != RIN_POLYMORPH)
		return i;
	/* if we don't like the diamond ring, use "silver" */
	if ((i = find_otyp_of_desc("silver", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find silver ring");
    return 0;
}

/* find the object index for a non-polymorph iron ring */
int
find_good_aring()
{
	static int i = -1;
    register const char *s;
	if (i != -1) return i;
	
	/* maybe use sapphire ring */
	if ((i = find_otyp_of_desc("sapphire", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1 && i != RIN_POLYMORPH)
		return i;
	/* if we don't like the sapphire ring, use "brass" */
	if ((i = find_otyp_of_desc("brass", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find brass ring");
    return 0;
}

/* find the object index for visored helmet */
int
find_vhelm()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;
	
	if ((i = find_otyp_of_desc("visored helmet", 0, HELMET, HELM_OF_DRAIN_RESISTANCE, 0)) != -1)
		return i;
	else
		impossible("could not find visored helmet");
	return 0;
}

/* find the object index for snow boots; used [once] by slippery ice code */
int
find_skates()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("snow boots", 0, SPEED_BOOTS, FLYING_BOOTS, 0)) != -1)
		return i;
	else
		impossible("could not find snow boots");
	return 0;
}

/* find the object index for combat boots */
int
find_cboots()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("combat boots", 0, SPEED_BOOTS, FLYING_BOOTS, 0)) != -1)
		return i;
	else
		impossible("could not find combat boots");
	return 0;
}

/* find the object index for mud boots */
int
find_mboots()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("mud boots", 0, SPEED_BOOTS, FLYING_BOOTS, 0)) != -1)
		return i;
	else
		impossible("could not find mud boots");
	return 0;
}

/* find the object index for hiking boots */
int
find_hboots()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("hiking boots", 0, SPEED_BOOTS, FLYING_BOOTS, 0)) != -1)
		return i;
	else
		impossible("could not find hiking boots");
	return 0;
}

/* find the object index for buckled boots */
int
find_bboots()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("buckled boots", 0, SPEED_BOOTS, FLYING_BOOTS, 0)) != -1)
		return i;
	else
		impossible("could not find buckled boots");
	return 0;
}

/* find the object index for jungle boots */
int
find_jboots()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("jungle boots", 0, SPEED_BOOTS, FLYING_BOOTS, 0)) != -1)
		return i;
	else
		impossible("could not find jungle boots");
	return 0;
}

/* find the object index for riding boots */
int
find_rboots()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("riding boots", 0, SPEED_BOOTS, FLYING_BOOTS, 0)) != -1)
		return i;
	else
		impossible("could not find riding boots");
	return 0;
}

/* find the object index for the ornamental cope */
int
find_cope()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("ornamental cope", 0, CLOAK_OF_PROTECTION, CLOAK_OF_DISPLACEMENT, 0)) != -1)
		return i;
	else
		impossible("could not find ornamental cope");
	return 0;
}

/* find the object index for the opera cloak */
int
find_opera_cloak()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("opera cloak", 0, CLOAK_OF_PROTECTION, CLOAK_OF_DISPLACEMENT, 0)) != -1)
		return i;
	else
		impossible("could not find opera cloak");
	return 0;
}

/* find the object index for piece of cloth */
int
find_pcloth()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("piece of cloth", 0, CLOAK_OF_PROTECTION, CLOAK_OF_DISPLACEMENT, 0)) != -1)
		return i;
	else
		impossible("could not find piece of cloth");
	return 0;
}

/* find the object index for the signet ring */
/* bugfix: can't be static-optimized, because we sometimes reshuffle descriptions after calling this function */
int
find_signet_ring()
{
	int i = -1;
	register const char *s;

	if ((i = find_otyp_of_desc("black signet", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find signet ring");
	return 0;
}
/* legacy function */
boolean
isSignetRing(otyp)
int otyp;
{
	return (otyp == find_signet_ring());
}

/* find the object index for the engagement ring */
int
find_engagement_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("engagement", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find engagement ring");
	return 0;
}

/* find the object index for the gold ring */
int
find_gold_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("gold", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find gold ring");
	return 0;
}

int
find_silver_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("silver", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find silver ring");
	return 0;
}

int
find_opal_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("opal", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find opal ring");
	return 0;
}

int
find_clay_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("clay", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find clay ring");
	return 0;
}

int
find_coral_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("coral", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find coral ring");
	return 0;
}

int
find_onyx_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("black onyx", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find onyx ring");
	return 0;
}

int
find_moonstone_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("moonstone", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find moonstone ring");
	return 0;
}

int
find_jade_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("jade", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find jade ring");
	return 0;
}

int
find_agate_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("agate", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find agate ring");
	return 0;
}

int
find_topaz_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("topaz", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find topaz ring");
	return 0;
}

int
find_sapphire_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("sapphire", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find sapphire ring");
	return 0;
}

int
find_ruby_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("ruby", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find ruby ring");
	return 0;
}

int
find_pearl_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("pearl", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find pearl ring");
	return 0;
}

int
find_ivory_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("ivory", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find ivory ring");
	return 0;
}

int
find_emerald_ring()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("emerald", 0, RIN_ADORNMENT, RIN_PROTECTION_FROM_SHAPE_CHAN, 0)) != -1)
		return i;
	else
		impossible("could not find emerald ring");
	return 0;
}

/* test if a ring is an engravable ring */
boolean
isEngrRing(otyp)
int otyp;
{
    register int i;
	
	static int egrings[13] = {0,0,0, 0,0,0, 0,0,0, 0,0,0, 0}; 
	if(!egrings[0]){ 
		egrings[0] = find_opal_ring(); 
		egrings[1] = find_clay_ring(); 
		egrings[2] = find_coral_ring();
		
		egrings[3] = find_onyx_ring();
		egrings[4] = find_moonstone_ring();
		egrings[5] = find_jade_ring();
		
		egrings[6] = find_agate_ring();
		egrings[7] = find_topaz_ring();
		egrings[8] = find_sapphire_ring();
		
		egrings[9] = find_ruby_ring();
		egrings[10] = find_pearl_ring(); /* diamond is too hard */
		egrings[11] = find_ivory_ring();
	
		egrings[12] = find_emerald_ring();
	}
	
    for (i = 0; i < 13; i++)
		if (egrings[i] == otyp) return TRUE;

    return FALSE;
}

/* find the object index for golden potions */
int
find_golden_potion()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("golden", 0, POT_GAIN_ABILITY, POT_BLOOD, 0)) != -1)
		return i;
	else
		impossible("could not find golden potion");
	return 0;
}

int
find_cloth_book()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("cloth", 0, SPE_DIG, SPE_BLANK_PAPER, 0)) != -1)
		return i;
	else
		impossible("could not find cloth spellbook");
	return 0;
}

int
find_leather_book()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("leather", 0, SPE_DIG, SPE_BLANK_PAPER, 0)) != -1)
		return i;
	else
		impossible("could not find leather spellbook");
	return 0;
}

int
find_bronze_book()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("bronze", 0, SPE_DIG, SPE_BLANK_PAPER, 0)) != -1)
		return i;
	else
		impossible("could not find bronze spellbook");
	return 0;
}

int
find_silver_book()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("silver", 0, SPE_DIG, SPE_BLANK_PAPER, 0)) != -1)
		return i;
	else
		impossible("could not find silver spellbook");
	return 0;
}

int
find_gold_book()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("gold", 0, SPE_DIG, SPE_BLANK_PAPER, 0)) != -1)
		return i;
	else
		impossible("could not find gold spellbook");
	return 0;
}

/* find a wand that is compatable with a given material*/
int
matWand(otyp, mat)
int otyp;
int mat;
{
    int i, j, ctyp;
	
	/* check if there is a wand whose material matches already (prefered) */
	/* traverses in order of otyp */
	for(i = WAN_SECRET_DOOR_DETECTION; i <= WAN_LIGHTNING; i++){
		ctyp = WAN_SECRET_DOOR_DETECTION + (((otyp+i)-WAN_SECRET_DOOR_DETECTION)%(WAN_LIGHTNING-WAN_SECRET_DOOR_DETECTION));
		if(objects[ctyp].oc_material == mat)
			return ctyp;
	}

	/* otherwise, grab a random material-neutral wand */
	randomize_nonmatwands();
	if ((i = find_otyp_of_desc("dummy-not-a-wand", matNeutralWands, WAN_SECRET_DOOR_DETECTION, WAN_LIGHTNING, 0)) != -1)
		return i;
	else
		impossible("couldn't find material-neutral wand");

	return 0;
}

/* find the object index for old gloves */
int
find_ogloves()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("old gloves", 0, GLOVES, GAUNTLETS_OF_DEXTERITY, 0)) != -1)
		return i;
	else
		impossible("could not find old gloves");
	return 0;
}

/* find the object index for fighting gloves */
int
find_tgloves()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("fighting gloves", 0, GLOVES, GAUNTLETS_OF_DEXTERITY, 0)) != -1)
		return i;
	else
		impossible("could not find fighting gloves");
	return 0;
}

/* find the object index for padded gloves */
int
find_pgloves()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("padded gloves", 0, GLOVES, GAUNTLETS_OF_DEXTERITY, 0)) != -1)
		return i;
	else
		impossible("could not find padded gloves");
	return 0;
}

/* find the object index for fencing gloves */
int
find_fgloves()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("fencing gloves", 0, GLOVES, GAUNTLETS_OF_DEXTERITY, 0)) != -1)
		return i;
	else
		impossible("could not find fencing gloves");
	return 0;
}

/* find the object index for riding gloves */
int
find_rgloves()
{
	static int i = -1;
	register const char *s;
	if (i != -1) return i;

	if ((i = find_otyp_of_desc("riding gloves", 0, GLOVES, GAUNTLETS_OF_DEXTERITY, 0)) != -1)
		return i;
	else
		impossible("could not find riding gloves");
	return 0;
}

void
oinit()			/* level dependent initialization */
{
	setgemprobs(&u.uz);
}

void
savenames(fd, mode)
int fd, mode;
{
	register int i;
	unsigned int len;

	if (perform_bwrite(mode)) {
	    bwrite(fd, (genericptr_t)bases, sizeof bases);
	    bwrite(fd, (genericptr_t)disco, sizeof disco);
	    bwrite(fd, (genericptr_t)objects,
		   sizeof(struct objclass) * NUM_OBJECTS);
	}
	/* as long as we use only one version of Hack we
	   need not save oc_name and oc_descr, but we must save
	   oc_uname for all objects */
	for (i = 0; i < NUM_OBJECTS; i++)
	    if (objects[i].oc_uname) {
		if (perform_bwrite(mode)) {
		    len = strlen(objects[i].oc_uname)+1;
		    bwrite(fd, (genericptr_t)&len, sizeof len);
		    bwrite(fd, (genericptr_t)objects[i].oc_uname, len);
		}
		if (release_data(mode)) {
		    free((genericptr_t)objects[i].oc_uname);
		    objects[i].oc_uname = 0;
		}
	    }
}

void
restnames(fd)
register int fd;
{
	register int i;
	unsigned int len;

	mread(fd, (genericptr_t) bases, sizeof bases);
	mread(fd, (genericptr_t) disco, sizeof disco);
	mread(fd, (genericptr_t) objects, sizeof(struct objclass) * NUM_OBJECTS);
	for (i = 0; i < NUM_OBJECTS; i++)
	    if (objects[i].oc_uname) {
		mread(fd, (genericptr_t) &len, sizeof len);
		objects[i].oc_uname = (char *) alloc(len);
		mread(fd, (genericptr_t)objects[i].oc_uname, len);
	    }
#ifdef USE_TILES
	shuffle_tiles();
#endif
	hack_objects();
}

void
discover_object(oindx, mark_as_known, credit_hero)
register int oindx;
boolean mark_as_known;
boolean credit_hero;
{
    if (!objects[oindx].oc_name_known) {
	register int dindx, acls = objects[oindx].oc_class;

	/* Loop thru disco[] 'til we find the target (which may have been
	   uname'd) or the next open slot; one or the other will be found
	   before we reach the next class...
	 */
	for (dindx = bases[acls]; disco[dindx] != 0; dindx++)
	    if (disco[dindx] == oindx) break;
	disco[dindx] = oindx;

	if (mark_as_known) {
	    objects[oindx].oc_name_known = 1;
	    if (credit_hero) exercise(A_WIS, TRUE);
	}
	if (moves > 1L) update_inventory();
    }
}

/* if a class name has been cleared, we may need to purge it from disco[] */
void
undiscover_object(oindx)
register int oindx;
{
    if (!objects[oindx].oc_name_known) {
	register int dindx, acls = objects[oindx].oc_class;
	register boolean found = FALSE;

	/* find the object; shift those behind it forward one slot */
	for (dindx = bases[acls];
	      dindx < NUM_OBJECTS && disco[dindx] != 0
		&& objects[dindx].oc_class == acls; dindx++)
	    if (found)
		disco[dindx-1] = disco[dindx];
	    else if (disco[dindx] == oindx)
		found = TRUE;

	/* clear last slot */
	if (found) disco[dindx-1] = 0;
	/*else impossible("named object not in disco"); Papering over a bug: something about repeatedly using the amnesia code
		seems to be tripping this. However, the bug seems harmless so I'm removing the error message. Famous last words. */
	update_inventory();
    }
}

STATIC_OVL boolean
interesting_to_discover(i)
register int i;
{
	/* Pre-discovered objects are now printed with a '*' */
    return((boolean)(objects[i].oc_uname != (char *)0 ||
	    (objects[i].oc_name_known && OBJ_DESCR(objects[i]) != (char *)0)));
}

/* items that should stand out once they're known */
static short uniq_objs[] = {
	AMULET_OF_YENDOR,
	SPE_BOOK_OF_THE_DEAD,
	CANDELABRUM_OF_INVOCATION,
	BELL_OF_OPENING,
};

int
dodiscovered()				/* free after Robert Viduya */
{
    register int i, dis;
    int	ct = 0;
    char *s, oclass, prev_class, classes[MAXOCLASSES];
    winid tmpwin;
	char buf[BUFSZ];

    tmpwin = create_nhwindow(NHW_MENU);
    putstr(tmpwin, 0, "Discoveries");
    putstr(tmpwin, 0, "");

    /* gather "unique objects" into a pseudo-class; note that they'll
       also be displayed individually within their regular class */
    for (i = dis = 0; i < SIZE(uniq_objs); i++)
	if (objects[uniq_objs[i]].oc_name_known) {
	    if (!dis++)
		putstr(tmpwin, iflags.menu_headings, "Unique Items");
		Sprintf(buf, "  %s", OBJ_NAME(objects[uniq_objs[i]]));
	    putstr(tmpwin, 0, buf);
	    ++ct;
	}
    /* display any known artifacts as another pseudo-class */
    ct += disp_artifact_discoveries(tmpwin);

    /* several classes are omitted from packorder; one is of interest here */
    Strcpy(classes, flags.inv_order);
    if (!index(classes, VENOM_CLASS)) {
	s = eos(classes);
	*s++ = VENOM_CLASS;
	*s = '\0';
    }

    for (s = classes; *s; s++) {
	oclass = *s;
	prev_class = oclass + 1;	/* forced different from oclass */
	for (i = bases[(int)oclass];
	     i < NUM_OBJECTS && objects[i].oc_class == oclass; i++) {
	    if ((dis = disco[i]) && interesting_to_discover(dis)) {
		ct++;
		if (oclass != prev_class) {
		    putstr(tmpwin, iflags.menu_headings, let_to_name(oclass, FALSE, FALSE));
		    prev_class = oclass;
		}
		Sprintf(buf, "%s %s",(objects[dis].oc_pre_discovered ? "*" : " "),
				obj_typename(dis));
		putstr(tmpwin, 0, buf);
	    }
	}
    }
    if (ct == 0) {
	You("haven't discovered anything yet...");
    } else
	display_nhwindow(tmpwin, TRUE);
    destroy_nhwindow(tmpwin);

    return MOVE_INSTANT;
}

void
set_isamusei_color(obj)
struct obj *obj;
{
	if(u.uinsight >= 70){
		obj->obj_color = CLR_MAGENTA;
	} else if(u.uinsight >= 57){
		obj->obj_color = CLR_BRIGHT_MAGENTA;
	} else if(u.uinsight >= 45){
		obj->obj_color = CLR_BRIGHT_BLUE;
	} else if(u.uinsight >= 33){
		obj->obj_color = CLR_BRIGHT_CYAN;
	} else if(u.uinsight >= 22){
		obj->obj_color = CLR_BRIGHT_GREEN;
	} else if(u.uinsight >= 10){
		obj->obj_color = CLR_YELLOW;
	} else {
		obj->obj_color = CLR_ORANGE;
	}
}

void
set_object_color(otmp)
struct obj *otmp;
{
	const char * s;
	
	/* should never happen */
	if (!otmp)
	{
		impossible("set_object_color called with no object");
		return;
	}
	/* fake mimic objects use the default color 
	 * they also have unset data in too many places
	 */
	if (otmp->oclass == STRANGE_OBJECT)
	{
		otmp->obj_color = objects[otmp->otyp].oc_color;
		return;
	}
	/* color-changing artifacts override this */
	
	/* these change color in a not-terribly-well-handled way */
	if (otmp->otyp == ISAMUSEI)
	{
		set_isamusei_color(otmp);
		return;
	}

	/* gold pieces are gold */
	if (otmp->otyp == GOLD_PIECE){
		otmp->obj_color = HI_GOLD;
		return;
	}

	/* plumed helmets and etched helmets get fancy colors, but only if their material is boring (iron/metal) */
	if ((is_iron_obj(otmp) || otmp->obj_material == METAL)
		&& ((s = OBJ_DESCR(objects[otmp->otyp])) != (char *)0 && !strncmpi(s, "plumed", 6))){
		otmp->obj_color = CLR_RED;
		return;
	}
	if ((is_iron_obj(otmp) || otmp->obj_material == METAL)
		&& ((s = OBJ_DESCR(objects[otmp->otyp])) != (char *)0 && !strncmpi(s, "etched", 6))){
		otmp->obj_color = CLR_BRIGHT_GREEN;
		return;
	}

	/* objects with non-standard materials whose base color is that of their material */
	if (otmp->obj_material != objects[otmp->otyp].oc_material
		&& materials[objects[otmp->otyp].oc_material].color == objects[otmp->otyp].oc_color)
	{
		/* Fancy gem colors */
		if (otmp->obj_material == GEMSTONE && otmp->sub_material){
			otmp->obj_color = objects[otmp->sub_material].oc_color;
			return;
		}
		/* Dragon hide/bone discrepancy -- dragonhide should be leather colored, not bone colored.
		 * We are neglecting dragon scales, since there currently no circumstances where that happens
		 * as it requires the base material to be dragonhide, and this code-block applies to modified mats
		 */
		if (otmp->obj_material == DRAGON_HIDE && !strncmpi(material_name(otmp, TRUE), "dragonhide", 10)){
			otmp->obj_color = HI_LEATHER;
			return;
		}
		
		if(otmp->obj_material == VEGGY){
			if (otmp->otyp == SEDGE_HAT || otmp->otyp == SHEAF_OF_HAY)
				otmp->obj_color = CLR_YELLOW;
			else if(otmp->obj_material != objects[otmp->otyp].oc_material){
				switch(rn2(12)){
					case 0:
						otmp->obj_color = CLR_BROWN;
					break;
					case 1:
						otmp->obj_color = CLR_GREEN;
					break;
					case 2:
						otmp->obj_color = CLR_BRIGHT_GREEN;
					break;
					case 3:
						otmp->obj_color = CLR_RED;
					break;
					case 4:
						otmp->obj_color = CLR_ORANGE;
					break;
					case 5:
						otmp->obj_color = CLR_YELLOW;
					break;
					case 6:
						otmp->obj_color = CLR_BLUE;
					break;
					case 7:
						otmp->obj_color = CLR_BRIGHT_BLUE;
					break;
					case 8:
						otmp->obj_color = CLR_MAGENTA;
					break;
					case 9:
						otmp->obj_color = CLR_BRIGHT_MAGENTA;
					break;
					case 10:
						otmp->obj_color = CLR_BLACK;
					break;
					case 11:
						otmp->obj_color = CLR_WHITE;
					break;
				}
			} else {
				otmp->obj_color = materials[otmp->obj_material].color;
			}
			return;
		}
		
		otmp->obj_color = materials[otmp->obj_material].color;
		return;
	}
	/* default color */
	otmp->obj_color = objects[otmp->otyp].oc_color;
	return;
}

int
object_color(otmp)
struct obj *otmp;
{
	/* should never happen */
	if (!otmp)
	{
		impossible("object_color called with no object");
		return 0;
	}
	/* fake mimic objects use the default color 
	 * they also have unset data in too many places
	 */
	if (otmp->oclass == STRANGE_OBJECT)
	{
		return objects[otmp->otyp].oc_color;
	}

	/* gold pieces are gold */
	if (otmp->otyp == GOLD_PIECE){
		return HI_GOLD;
	}
	
	/* Otherwise, the PC may not know the object's color (or description) */
	/*   If the description is not known, the object is of unknown color and is shown in grey */
	if(!otmp->dknown){
		return CLR_GRAY;
	}

	if(litsaber(otmp) && otmp->otyp != ROD_OF_FORCE)
		return lightsaber_colorCLR(otmp);
	
	/* artifacts with set colors */
	switch (otmp->oartifact)
	{
	case ART_EPOCH_S_CURVE:		return CLR_WHITE;
	case ART_BLACK_CRYSTAL:		return PURIFIED_CHAOS ? CLR_BLACK : HI_GLASS;
	case ART_EARTH_CRYSTAL:		return PURIFIED_EARTH ? CLR_YELLOW : CLR_BROWN;
	case ART_FIRE_CRYSTAL:		return PURIFIED_FIRE ? 	CLR_ORANGE : CLR_RED;
	case ART_WATER_CRYSTAL:		return PURIFIED_WATER ? CLR_BRIGHT_BLUE : CLR_BLUE;
	case ART_AIR_CRYSTAL:		return PURIFIED_WIND ?  CLR_BRIGHT_GREEN : CLR_GREEN;
	case ART_SPELL_WARDED_WRAPPINGS_OF_:		return CLR_BLACK;
	}
	
	//Use the set color.
	return otmp->obj_color;
}

void
set_submat(otmp, submat)
struct obj *otmp;
int submat;
{
	/* should never happen */
	if (!otmp)
	{
		impossible("set_submat called with no object");
		return;
	}
	otmp->sub_material = submat;
	object_color(otmp);
}

void
fix_object(otmp)
	struct obj *otmp;
{
	otmp->owt = weight(otmp);
	if (obj_eternal_light(otmp) && !otmp->lamplit) {
		begin_burn(otmp);
	}
}

/*o_init.c*/
