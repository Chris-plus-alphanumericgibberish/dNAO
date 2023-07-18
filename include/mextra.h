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
#define MX_ESUM 6
#define MX_EVGD 7
#define MX_ESMT 8
#define NUM_MX 9

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
		struct esum * esum_p;
		struct evgd * evgd_p;
		struct esmt * esmt_p;
	};
};

#endif /* MEXTRA_H */
