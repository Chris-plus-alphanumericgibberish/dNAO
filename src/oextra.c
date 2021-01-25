/*	SCCS Id: @(#)oextra.c	3.4	2003/12/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"

struct ox_table {
	int indexnum;
	int s_size;
} ox_list[] = {
	{OX_ENAM, -1},	/* variable; actual size is stored in structure. 1st item is a long containing size */
	{OX_EMON, -1},	/* variable; actual size is stored in structure. 1st item is a long containing size */
	{OX_EMID, sizeof(int)}
};

/* add one component to obj */
/* automatically finds size of component */
void
add_ox(otmp, ox_id)
struct obj * otmp;
int ox_id;
{
	int size = ox_list[ox_id].s_size;
	if (size == -1) {
		impossible("Attempting to make variable-length ox struct with no size given");
		return;
	}
	add_ox_l(otmp, ox_id, size);
	return;
}

/* add one component to obj */
/* for a fixed-size component, len is its total size (from table) */
/* for a variable-size compenent, len is the size of the extra data (does NOT include sizeof(int) to store len itself) */
void
add_ox_l(otmp, ox_id, len)
struct obj * otmp;
int ox_id;
long len;
{
	void * ox_p;

	/* allocate oextra structure if needed */
	if (!otmp->oextra_p) {
		otmp->oextra_p = malloc(sizeof(union oextra));
		memset((void *)(otmp->oextra_p), 0, sizeof(union oextra));
	}

	/* check that component doesn't already exist */
	if ((ox_p = get_ox(otmp, ox_id))) {
		impossible("oextra substruct %d already exists on %s", ox_id, xname(otmp));
		return;
	}

	/* assign allocating size if it's a variable-size ox */
	if (ox_list[ox_id].s_size == -1)
		len += sizeof(long);

	/* allocate and link it */
	ox_p = malloc(len);
	memset(ox_p, 0, len);
	otmp->oextra_p->eindex[ox_id] = ox_p;
	/* assign size if it's a variable-size ox */
	if (ox_list[ox_id].s_size == -1)
		*((long *)ox_p) = len - sizeof(long);

	return;
}

/* removes one component from obj */
/* removes oextra if that was the last one */
void
rem_ox(otmp, ox_id)
struct obj * otmp;
int ox_id;
{
	void * ox_p;

	if (!otmp->oextra_p) {
		/* already done */
		return;
	}

	if (!(ox_p = get_ox(otmp, ox_id))) {
		/* already done */
		return;
	}

	/* dealloc ox_p */
	free((genericptr_t)ox_p);
	otmp->oextra_p->eindex[ox_id] = (void *)0;
	
	/* if all of oextra_p is 0, dealloc its oextra entirely */
	register int i;
	boolean foundany = FALSE;
	for (i = 0; i < NUM_OX; i++)
		if (otmp->oextra_p->eindex[i])
			foundany = TRUE;
	if (!foundany) {
		/* dealloc oextra_p */
		free((genericptr_t)(otmp->oextra_p));
		otmp->oextra_p = (union oextra *)0;
	}
	return;
}

/* removes all components from obj */
void
rem_all_ox(otmp)
struct obj * otmp;
{
	int ox_id;

	for (ox_id = 0; (otmp->oextra_p) && (ox_id < NUM_OX); ox_id++) {
		rem_ox(otmp, ox_id);
	}
	return;
}

/* copies one component from obj1 to obj2 */
void
cpy_ox(obj1, obj2, ox_id)
struct obj * obj1;
struct obj * obj2;
int ox_id;
{
	void * ox_p1;
	void * ox_p2;
	if ((ox_p1 = get_ox(obj1, ox_id))) {
		ox_p2 = get_ox(obj2, ox_id);
		if(!ox_p2)
			if (ox_list[ox_id].s_size != -1)
				add_mx(obj2, ox_id);
			else
				add_ox_l(obj2, ox_id, siz_ox(obj1, ox_id)-sizeof(long));
		memcpy(get_ox(obj2, ox_id), ox_p1, siz_ox(obj1, ox_id));
	}
	return;
}

/* moves one component from obj1 to obj2 */
void
mov_ox(obj1, obj2, ox_id)
struct obj * obj1;
struct obj * obj2;
int ox_id;
{
	cpy_ox(obj1, obj2, ox_id);
	rem_ox(obj1, ox_id);
	return;
}

void
mov_all_ox(obj1, obj2)
struct obj * obj1;
struct obj * obj2;
{
	int ox_id;
	for (ox_id=0; ox_id<NUM_OX; ox_id++)
		cpy_ox(obj1, obj2, ox_id);
	rem_all_ox(obj1);
	return;
}

/* returns pointer to wanted component */
void *
get_ox(otmp, ox_id)
struct obj * otmp;
int ox_id;
{
	if (!otmp || !otmp->oextra_p)
		return (void *)0;
	
	return otmp->oextra_p->eindex[ox_id];
}

/* returns the size, in bytes, of component. Includes sizeof(long) for variable-size components. */
long
siz_ox(otmp, ox_id)
struct obj * otmp;
int ox_id;
{
	void * ox_p;

	if (!(ox_p = get_ox(otmp, ox_id)))
		return 0;

	long size = ox_list[ox_id].s_size;

	if (size == -1) {
		/* marker that size is instead stored as the first bit of the structure as a long */
		size = *((long *)ox_p) + sizeof(long);
	}
	return size;
}

/* allocates a block of memory containing otmp and it's components */
/* free this memory block when done using it! */
void *
bundle_oextra(otmp, len_p)
struct obj * otmp;
long * len_p;
{
	int i;
	long len = 0;
	int towrite = 0;
	void * output;
	void * output_ptr;

	/* determine what components are being bundled, and how large they are */
	for (i = 0; i < NUM_OX; i++) {
		if (otmp->oextra_p->eindex[i]) {
			towrite |= (1 << i);
			len += siz_ox(otmp, i);
		}
	}
	len += sizeof(int);	/* to store which components are going to be written */

	/* allocate that much memory */
	output_ptr = output = malloc(len);

	/* add what compenents are being written */
	memcpy(output_ptr, &towrite, sizeof(int));
	output_ptr = output_ptr + sizeof(int);

	/* add those components */
	for (i = 0; i < NUM_OX; i++) {
		if (!(towrite & (1 << i)))
			continue;
		/* copy memory */
		memcpy(output_ptr,otmp->oextra_p->eindex[i],siz_ox(otmp, i));
		/* increment output_ptr (char is 1 byte) */
		output_ptr = ((char *)output_ptr) + siz_ox(otmp, i);
	}

	*len_p = len;
	return output;
}

/* takes a pointer to a block of memory containing the components of an oextra and assigns them to otmp */
void
unbundle_oextra(otmp, oextra_block)
struct obj * otmp;
void * oextra_block;
{
	int i;
	int toread = 0;
	long len;
	void * ox_p;

	/* clear stale oextra pointer from otmp */
	otmp->oextra_p = (union oextra *)0;

	/* determine what components are here */
	toread = *((int *)oextra_block);
	oextra_block = oextra_block + sizeof(int);
	
	/* read those components */
	for (i = 0; i < NUM_OX; i++) {
		if (!(toread & (1 << i)))
			continue;
		/* get length to use */
		len = ox_list[i].s_size;
		if (len == -1)	{// was saved
			len = *((long *)oextra_block);
			oextra_block = oextra_block + sizeof(long);
		}

		/* allocate component */
		add_ox_l(otmp, i, len);
		/* create pointer to it */
		ox_p = get_ox(otmp, i);
		/* if we had a variable len, rewrite it */
		if (ox_list[i].s_size == -1) {
			*((long *)ox_p) = len;
			ox_p = ox_p + sizeof(long);
		}
		/* fill in the body of the component */
		memcpy(ox_p, oextra_block, len);
		oextra_block = ((char *)oextra_block) + len;
	}
	return;
}

/* saves oextra from otmp to fd */
void
save_oextra(otmp, fd, mode)
struct obj * otmp;
int fd;
int mode;
{
	void * oextra_block;
	long len;

	/* don't save nothing */
	if (!otmp->oextra_p)
		return;

	/* get oextra as one continous bundle of memory */
	oextra_block = bundle_oextra(otmp, &len);

	/* write it */
	bwrite(fd, oextra_block, len);

	/* deallocate the block */
	free(oextra_block);

	if (release_data(mode)) {
		rem_all_ox(otmp);
	}

	return;
}

/* restores oextra from fd onto otmp */
/* should only be called if otmp->oextra_p existed (currently a stale pointer) */
/* TODO: combine somehow with unbundle_oextra? Might not be possible. */
void
rest_oextra(otmp, fd, ghostly)
struct obj * otmp;
int fd;
boolean ghostly;
{
	int i;
	int toread = 0;
	long len;
	void * ox_p;

	/* clear stale oextra pointer from otmp */
	otmp->oextra_p = (union oextra *)0;

	/* determine what components are being read */
	mread(fd, (genericptr_t) &toread, sizeof(int));
	
	/* read those components */
	for (i = 0; i < NUM_OX; i++) {
		if (!(toread & (1 << i)))
			continue;
		/* get length to read */
		len = ox_list[i].s_size;
		if (len == -1) {	// was saved
			mread(fd, (genericptr_t) &len, sizeof(long));
		}

		/* allocate component */
		add_ox_l(otmp, i, len);
		/* create pointer to it */
		ox_p = get_ox(otmp, i);
		/* if we had read len, rewrite it */
		if (ox_list[i].s_size == -1) {
			*((long *)ox_p) = len;
			ox_p = ox_p + sizeof(long);
		}
		mread(fd, ox_p, len);
	}
	return;
}

/*oextra.c*/
