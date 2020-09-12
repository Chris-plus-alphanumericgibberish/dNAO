/*	SCCS Id: @(#)mextra.c	3.4	2003/12/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "mextra.h"
#include "lev.h"

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

	/* assign allocating size if it's a variable-size mx */
	if (mx_list[mx_id].s_size == -1)
		len += sizeof(int);

	/* allocate and link it */
	mx_p = malloc(len);
	memset(mx_p, 0, len);
	mtmp->mextra_p->eindex[mx_id] = mx_p;
	/* assign size if it's a variable-size mx */
	if (mx_list[mx_id].s_size == -1)
		*((int *)mx_p) = len - sizeof(int);

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
		free((genericptr_t)(mtmp->mextra_p));
		mtmp->mextra_p = (union mextra *)0;
	}
	return;
}

/* removes all components from mon */
void
rem_all_mx(mtmp)
struct monst * mtmp;
{
	int mx_id;

	for (mx_id = 0; (mtmp->mextra_p) && (mx_id < NUM_MX); mx_id++) {
		rem_mx(mtmp, mx_id);
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
			add_mx_l(mon2, mx_id, siz_mx(mon1, mx_id)-sizeof(int));
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

void
mov_all_mx(mon1, mon2)
struct monst * mon1;
struct monst * mon2;
{
	int mx_id;
	for (mx_id=0; mx_id<NUM_MX; mx_id++)
		cpy_mx(mon1, mon2, mx_id);
	rem_all_mx(mon1);
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

/* allocates a block of memory containing mtmp and it's components */
/* free this memory block when done using it! */
void *
bundle_mextra(mtmp, len_p)
struct monst * mtmp;
int * len_p;
{
	int i;
	int len = 0;
	int towrite = 0;
	void * output;
	void * output_ptr;

	/* determine what components are being bundled, and how large they are */
	for (i = 0; i < NUM_MX; i++) {
		if (mtmp->mextra_p->eindex[i]) {
			towrite |= (1 << i);
			len += siz_mx(mtmp, i);
		}
	}
	len += sizeof(int);	/* to store which components are going to be written */

	/* allocate that much memory */
	output_ptr = output = malloc(len);

	/* add what compenents are being written */
	memcpy(output_ptr, &towrite, sizeof(int));
	output_ptr = ((int *)output_ptr) + 1;

	/* add those components */
	for (i = 0; i < NUM_MX; i++) {
		if (!(towrite & (1 << i)))
			continue;
		/* copy memory */
		memcpy(output_ptr,mtmp->mextra_p->eindex[i],siz_mx(mtmp, i));
		/* increment output_ptr (char is 1 byte) */
		output_ptr = ((char *)output_ptr) + siz_mx(mtmp, i);
	}

	*len_p = len;
	return output;
}

/* takes a pointer to a block of memory containing the components of an mextra and assigns them to mtmp */
void
unbundle_mextra(mtmp, mextra_block)
struct monst * mtmp;
void * mextra_block;
{
	int i;
	int toread = 0;
	int len;
	void * mx_p;

	/* clear stale mextra pointer from mtmp */
	mtmp->mextra_p = (union mextra *)0;

	/* determine what components are here */
	toread = *((int *)mextra_block);
	mextra_block = ((int *)mextra_block) + 1;
	
	/* read those components */
	for (i = 0; i < NUM_MX; i++) {
		if (!(toread & (1 << i)))
			continue;
		/* get length to use */
		len = mx_list[i].s_size;
		if (len == -1)	{// was saved
			len = *((int *)mextra_block);
			mextra_block = ((int *)mextra_block) + 1;
		}

		/* allocate component */
		add_mx_l(mtmp, i, len);
		/* create pointer to it */
		mx_p = get_mx(mtmp, i);
		/* if we had a variable len, rewrite it */
		if (mx_list[i].s_size == -1) {
			*((int *)mx_p) = len;
			mx_p = ((int *)mx_p) + 1;
		}
		/* fill in the body of the component */
		memcpy(mx_p, mextra_block, len);
		mextra_block = ((char *)mextra_block) + len;
	}
	return;
}

/* saves mextra from mtmp to fd */
void
save_mextra(mtmp, fd, mode)
struct monst * mtmp;
int fd;
int mode;
{
	void * mextra_block;
	int len;

	/* don't save nothing */
	if (!mtmp->mextra_p)
		return;

	/* get mextra as one continous bundle of memory */
	mextra_block = bundle_mextra(mtmp, &len);

	/* write it */
	bwrite(fd, mextra_block, len);

	/* deallocate the block */
	free(mextra_block);

	if (release_data(mode)) {
		rem_all_mx(mtmp);
	}

	return;
}

/* restores mextra from fd onto mtmp */
/* should only be called if mtmp->mextra_p existed (currently a stale pointer) */
/* TODO: combine somehow with unbundle_mextra? Might not be possible. */
void
rest_mextra(mtmp, fd, ghostly)
struct monst * mtmp;
int fd;
boolean ghostly;
{
	int i;
	int toread = 0;
	int len;
	void * mx_p;

	/* clear stale mextra pointer from mtmp */
	mtmp->mextra_p = (union mextra *)0;

	/* determine what components are being read */
	mread(fd, (genericptr_t) &toread, sizeof(int));
	
	/* read those components */
	for (i = 0; i < NUM_MX; i++) {
		if (!(toread & (1 << i)))
			continue;
		/* get length to read */
		len = mx_list[i].s_size;
		if (len == -1) {	// was saved
			mread(fd, (genericptr_t) &len, sizeof(int));
		}

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

	/* some components need special handling on restore */
	if (u.uz.dlevel) {
		if (mtmp->mextra_p->eshk_p) {
			struct eshk * eshkp = mtmp->mextra_p->eshk_p;
			/* assign bill pointer */
			if (eshkp->bill_p != (struct bill_x *) -1000)
				eshkp->bill_p = eshkp->bill;
			/* assign to level */
			if (ghostly) {
				assign_level(&(eshkp->shoplevel), &u.uz);
				if (!mtmp->mpeaceful && strncmpi(eshkp->customer, plname, PL_NSIZ))
				pacify_shk(mtmp);
			}
		}
		if (mtmp->mextra_p->epri_p) {
			/* assign to level */
			if (ghostly) {
				assign_level(&(mtmp->mextra_p->epri_p->shrlevel), &u.uz);
			}
		}
	}
	return;
}

/*mextra.c*/
