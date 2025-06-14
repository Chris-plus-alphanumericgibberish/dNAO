/*	SCCS Id: @(#)thoughtglyph.h	3.4	1997/05/01	*/
/* Copyright (c) Izchak Miller, 1989.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MUTATION_H
#define MUTATION_H

/* Macros for messages referring to hands, eyes, feet, etc... */
#define ARM 0
#define EYE 1
#define FACE 2
#define FINGER 3
#define FINGERTIP 4
#define FOOT 5
#define HAND 6
#define HANDED 7
#define HEAD 8
#define LEG 9
#define LIGHT_HEADED 10
#define NECK 11
#define SPINE 12
#define TOE 13
#define HAIR 14
#define BLOOD 15
#define LUNG 16
#define NOSE 17
#define STOMACH 18
#define HEART 19
#define BODY_SKIN 20
#define BODY_FLESH 21
#define BEAT 22
#define BONES 23
#define EAR 24
#define EARS 25
#define TONGUE 26
#define BRAIN 27
#define CREAK 28
#define CRACK 29

struct mutationtype {
	int mutation;
	int bodypart;
	const char * name;
	const char * description;
};

/* mutations (Must be non-zero) */
#define	ABHORRENT_SPORE	1
#define	CRAWLING_FLESH	2
#define	SHUB_RADIANCE	3
#define	TENDRIL_HAIR	4
#define	SHIFTING_MIND	5
#define	SHUB_CLAWS		6
#define	MIND_STEALER	7
#define	YOG_GAZE_1		8
#define	YOG_GAZE_2		9
#define	TWIN_MIND		10
#define	TWIN_DREAMS		11
#define	BY_THE_SMELL		12
#define	TWIN_SAVE		13
#define	SHUB_TENTACLES		14
#define	LAST_MUTATION		SHUB_TENTACLES

#define MUTATION_LISTSIZE	((LAST_MUTATION-1)/32 + 1)

#define check_mutation(mut) !!((u.mutations[(mut-1)/32]) & (0x1L << ((mut-1)%32)))
#define add_mutation(mut) (u.mutations[(mut-1)/32] |= (0x1L << ((mut-1)%32)))
#define remove_mutation(mut) (u.mutations[(mut-1)/32] &= ~(0x1L << ((mut-1)%32)))




#endif /* MUTATION_H */
