/*	SCCS Id: @(#)mextra.h	3.4	1992/11/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef OEXTRA_H
#define OEXTRA_H

#include "exstruct.h"

#define OX_ENAM 0
#define OX_EMON 1
#define OX_EMID 2
#define NUM_OX 3

/* container for all oextra */
union oextra {
	void * eindex[NUM_OX];
	struct {
		struct enam * enam_p;
		struct emon * emon_p;
		int * emid_p;
	};
};

struct ox_table {
	int indexnum;
	int s_size;
};

static struct ox_table ox_list[] = {
	{OX_ENAM, -1},	/* variable; actual size is stored in structure. 1st item is an int containing size */
	{OX_EMON, -1},	/* variable; actual size is stored in structure. 1st item is an int containing size */
	{OX_EMID, sizeof(int)}
};

#endif /* OEXTRA_H */
