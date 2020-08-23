/*	SCCS Id: @(#)mextra.h	3.4	1992/11/14	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MEXTRA_H
#define MEXTRA_H

#include "exstruct.h"

#define MX_EDOG 0
#define MX_EHOR 1
#define MX_EMIN 2
#define MX_ENAM 3
#define MX_EPRI 4
#define MX_ESHK 5
#define NUM_MX 6

/* container for all mextra */
union mextra {
	void * eindex[NUM_MX];
	struct {
		struct edog * edog_p;
		struct ehor * ehor_p;
		struct emin * emin_p;
		struct enam * enam_p;
		struct epri * epri_p;
		struct eshk * eshk_p;
	};
};

struct mx_table {
	int indexnum;
	int s_size;
};

static struct mx_table mx_list[] = {
	{MX_EDOG, sizeof(struct edog)},
	{MX_EHOR, sizeof(struct ehor)},
	{MX_EMIN, sizeof(struct emin)},
	{MX_ENAM, -1},	/* variable; actual size is stored in structure. 1st item is an int containing size */
	{MX_EPRI, sizeof(struct epri)},
	{MX_ESHK, sizeof(struct eshk)}
};

#endif /* MEXTRA_H */
