/* SPDX-License-Identifier: NGPL */

#ifndef MAPSEEN_H
#define MAPSEEN_H

#include "gnames.h"

/*
 * This has to be in its own header to avoid a circular makedefs ->
 * dungeon.h -> gnames.h -> makedefs dependency.  gnames.h needs to be
 * included for MAX_GOD.
 */

/* types and structures for dungeon map recording
 *
 * It is designed to eliminate the need for an external notes file for some of
 * the more mundane dungeon elements.  "Where was the last altar I passed?" etc...
 * Presumably the character can remember this sort of thing even if, months
 * later in real time picking up an old save game, I can't.
 *
 * To be consistent, one can assume that this map is in the player's mind and
 * has no physical correspondence (eliminating illiteracy/blind/hands/hands free
 * concerns.) Therefore, this map is not exaustive nor detailed ("some fountains").
 * This makes it also subject to player conditions (amnesia).
 */

typedef struct mapseen_feat {
	/* feature knowledge that must be calculated from levl array */
	Bitfield(nfount, 2);
	Bitfield(nforge, 2);
	Bitfield(nsink, 2);
	Bitfield(naltar, 2);
	Bitfield(ngrave, 2);
	Bitfield(nthrone, 2);
	Bitfield(ntree, 2);
	/* water, lava, ice are too verbose so commented out for now */
	/*
	Bitfield(water, 1);
	Bitfield(lava, 1);
	Bitfield(ice, 1);
	*/

	/* calculated from rooms array */
	Bitfield(nshop, 2);
	Bitfield(nmorgue, 2);
	Bitfield(ntemple, 2);
	Bitfield(shoptype, 5);

	Bitfield(forgot, 1); /* player has forgotten about this level? */

#define MSGODS_ARRAY_SIZE (MAX_GOD/16+1)
	unsigned short msgods[MSGODS_ARRAY_SIZE];
} mapseen_feat;

/* for mapseen->rooms */
#define MSR_SEEN		1

/* what the player knows about a single dungeon level */
/* initialized in mklev() */
typedef struct mapseen  {
	struct mapseen *next; /* next map in the chain */
	branch *br; /* knows about branch via taking it in goto_level */
	d_level lev; /* corresponding dungeon level */

	mapseen_feat feat;

	/* custom naming */
	char *custom;
	unsigned custom_lth;

	/* maybe this should just be in struct mkroom? */
	schar rooms[(MAXNROFROOMS+1)*2];
} mapseen;

#endif /* MAPSEEN_H */
