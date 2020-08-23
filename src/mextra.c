/*	SCCS Id: @(#)mextra.c	3.4	2003/12/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "mextra.h"

/* add one component to mon */
/* automatically finds size of component */
void
add_mx(mtmp, mx_id)
struct monst * mtmp;
int mx_id;
{
	int size = mx_list[mx_id].s_size;
	if (size == -1) {
		impossible("Attempting to make variable-length mx struct with no size given");
		return;
	}
	add_mx_l(mtmp, mx_id, size);
	return;
}

/* add one component to mon */
/* for a fixed-size component, len is its total size (from table) */
/* for a variable-size compenent, len is the size of the extra data (does NOT include sizeof(int) to store len itself) */
void
add_mx_l(mtmp, mx_id, len)
struct monst * mtmp;
int mx_id;
int len;
{
	void * mx_p;

	/* allocate mextra structure if needed */
	if (!mtmp->mextra_p) {
		mtmp->mextra_p = malloc(sizeof(union mextra));
		memset((void *)(mtmp->mextra_p), 0, sizeof(union mextra));
	}

	/* check that component doesn't already exist */
	if (mx_p = get_mx(mtmp, mx_id)) {
		impossible("mextra substruct %d already exists on %s", mx_id, m_monnam(mtmp));
		return;
	}

	/* allocate and link it */
	mx_p = malloc(len);
	memset(mx_p, 0, len);
	mtmp->mextra_p->eindex[mx_id] = mx_p;
	/* assign size if it's a variable-size mx */
	if (mx_list[mx_id].s_size == -1)
		*((int *)mx_p) = len;

	return;
}

/* removes one component from mon */
/* removes mextra if that was the last one */
void
rem_mx(mtmp, mx_id)
struct monst * mtmp;
int mx_id;
{
	void * mx_p;

	if (!mtmp->mextra_p) {
		/* already done */
		return;
	}

	if (!(mx_p = get_mx(mtmp, mx_id))) {
		/* already done */
		return;
	}

	/* dealloc mx_p */
	free((genericptr_t)mx_p);
	mtmp->mextra_p->eindex[mx_id] = (void *)0;
	
	/* if all of mextra_p is 0, dealloc its mextra entirely */
	register int i;
	boolean foundany = FALSE;
	for (i = 0; i < NUM_MX; i++)
		if (mtmp->mextra_p->eindex[i])
			foundany = TRUE;
	if (!foundany) {
		/* dealloc mextra_p */
		free((genericptr_t)mtmp->mextra_p);
		mtmp->mextra_p = (union mextra *)0;
	}
	return;
}

/* copies one component from mon1 to mon2 */
void
cpy_mx(mon1, mon2, mx_id)
struct monst * mon1;
struct monst * mon2;
int mx_id;
{
	void * mx_p1;
	void * mx_p2;
	if (mx_p1 = get_mx(mon1, mx_id)) {
		mx_p2 = get_mx(mon2, mx_id);
		if(!mx_p2)
			add_mx(mon2, mx_id);
		memcpy(mx_p1, get_mx(mon2, mx_id), siz_mx(mon1, mx_id));
	}
	return;
}

/* moves one component from mon1 to mon2 */
void
mov_mx(mon1, mon2, mx_id)
struct monst * mon1;
struct monst * mon2;
int mx_id;
{
	cpy_mx(mon1, mon2, mx_id);
	rem_mx(mon1, mx_id);
	return;
}

/* returns pointer to wanted component */
void *
get_mx(mtmp, mx_id)
struct monst * mtmp;
int mx_id;
{
	if (!mtmp || !mtmp->mextra_p)
		return (void *)0;
	
	return mtmp->mextra_p->eindex[mx_id];
}

/* returns the size, in bytes, of component. Includes sizeof(int) for variable-size components. */
int
siz_mx(mtmp, mx_id)
struct monst * mtmp;
int mx_id;
{
	void * mx_p;

	if (!(mx_p = get_mx(mtmp, mx_id)))
		return 0;

	int size = mx_list[mx_id].s_size;

	if (size == -1) {
		/* marker that size is instead stored as the first bit of the structure as an int */
		size = *((int *)mx_p) + sizeof(int);
	}
	return size;
}

/* saves mextra from mtmp to fd */
void
save_mextra(mtmp, fd)
struct monst * mtmp;
int fd;
{
	int i;
	int towrite = 0;

	/* don't save nothing */
	if (!mtmp->mextra_p)
		return;

	/* determine what components are being written */
	for (i = 0; i < NUM_MX; i++) {
		if (mtmp->mextra_p->eindex[i])
			towrite |= (1 << i);
	}
	bwrite(fd, &towrite, sizeof(int));
	
	/* write those components */
	for (i = 0; i < NUM_MX; i++) {
		if (!(towrite & (1 << i)))
			continue;
		bwrite(fd, get_mx(mtmp, i), siz_mx(mtmp, i));
	}
	return;
}

/* restores mextra from fd onto mtmp */
/* should only be called if mtmp->mextra_p existed (currently a stale pointer) */
void
rest_mextra(mtmp, fd)
struct monst * mtmp;
int fd;
{
	int i;
	int toread = 0;
	int len;
	void * mx_p;

	/* determine what components are being read */
	mread(fd, (genericptr_t) &toread, sizeof(int));
	
	/* read those components */
	for (i = 0; i < NUM_MX; i++) {
		if (!(toread & (1 << i)))
			continue;
		/* get length to read */
		len = mx_list[i].s_size;
		if (len == -1)	// was saved
			mread(fd, (genericptr_t) &len, sizeof(int));

		/* allocate component */
		add_mx_l(mtmp, i, len);
		/* create pointer to it */
		mx_p = get_mx(mtmp, i);
		/* if we had read len, rewrite it */
		if (mx_list[i].s_size == -1) {
			*((int *)mx_p) = len;
			mx_p = ((int *)mx_p) + 1;
		}
		mread(fd, mx_p, len);
	}
	return;
}

/*mextra.c*/
