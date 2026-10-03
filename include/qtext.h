/*	SCCS Id: @(#)qtext.h	3.4	1997/02/02	*/
/* Copyright (c) Mike Stephenson 1991.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef QTEXT_H
#define QTEXT_H

#define N_HDR	34		/* Maximum number of categories */
				/* (i.e., num roles + 1) */
#define LEN_HDR 3		/* Maximum length of a category name */

struct qtmsg {
	int	msgnum;
	char	delivery;
	long	offset,
		size;
};

#ifdef MAKEDEFS_C	/***** MAKEDEFS *****/

#define N_MSG	400		/* arbitrary (was 100) */

struct msghdr {
	int	n_msg;
	struct	qtmsg	qt_msg[N_MSG];
};

struct	qthdr {
	int	n_hdr;
	char	id[N_HDR][LEN_HDR];
	long	offset[N_HDR];
};

/* Error message macros */
#define CREC_IN_MSG	"Control record encountered during message - line %d\n"
#define DUP_MSG		"Duplicate message number at line %d\n"
#define END_NOT_IN_MSG	"End record encountered before message - line %d\n"
#define TEXT_NOT_IN_MSG	"Text encountered outside message - line %d\n"
#define UNREC_CREC	"Unrecognized Control record at line %d\n"
#define OUT_OF_HEADERS	"Too many message types (line %d)\nAdjust N_HDR in qtext.h and recompile.\n"
#define OUT_OF_MESSAGES "Too many messages in class (line %d)\nAdjust N_MSG in qtext.h and recompile.\n"


#else	/***** !MAKEDEFS *****/

struct	qtlists {
	struct	qtmsg	*common,
#if 0	/* UNUSED but available */
			*chrace,
#endif
			*chrole;
};


/*
 *	Quest message defines.	Used in quest.c to trigger off "realistic"
 *	dialogue to the player.
 */
#define QT_FIRSTTIME	 1
#define QT_NEXTTIME	 2
#define QT_OTHERTIME	 3

#define QT_GUARDTALK	 5	/* 5 random things guards say before quest */
#define QT_GUARDTALK2	10	/* 5 random things guards say after quest */

#define QT_FIRSTLEADER	15
#define QT_NEXTLEADER	16
#define QT_OTHERLEADER	17
#define QT_LASTLEADER	18
#define QT_BADLEVEL	19
#define QT_BADALIGN	20
#define QT_ASSIGNQUEST	21

#define QT_PISSED_OFF	22

#define QT_ENCOURAGE	25	/* 1-10 random encouragement messages */

#define QT_FIRSTLOCATE	35
#define QT_NEXTLOCATE	36

#define QT_PAUPER_HEALER_FIRSTTIME	37	/* pauper drow healer alternates for QT_FIRSTTIME */
#define QT_PAUPER_HEALER_FIRSTTIME_FREED	38

#define QT_FIRSTGOAL	40
#define QT_NEXTGOAL	41

#define QT_FIRSTNEMESIS 50
#define QT_NEXTNEMESIS	51
#define QT_OTHERNEMESIS 52
#define QT_NEMWANTSIT	53	/* you somehow got the artifact */

#define QT_DISCOURAGE	60	/* 1-10 random maledictive messages */

#define QT_GOTIT	70

#define QT_KILLEDNEM	80
#define QT_OFFEREDIT	81
#define QT_OFFEREDIT2	82	/* if you throw artifact to leader after #81 */

#define QT_OFFERART2	83	/* if you throw the alternate artifact to leader after #81 */

#define QT_MADMAN_OFFEREDIT	90	/* Madman alternate use: The Stranger is dead before completing the quest */
#define QT_POSTHANKS	90
#define QT_HASAMULET	91

#define QT_VARIANT		100 /* offset for variant 2 messages */

/*
 *	Message defines for common text used in maledictions.
 */
#define COMMON_ID	"-"	/* Common message id value */

#define QT_ANGELIC	10
#define QTN_ANGELIC	10

#define QT_DEMONIC	30
#define QTN_DEMONIC	20

#define QT_BANISHED	60

#define QT_COMMON_LEGACY		1
#define QT_COMMON_QCALL			2
#define QT_COMMON_QCALL_AGAIN		3
#define QT_COMMON_QCALL_AGAIN_ROGUE	4
#define QT_COMMON_NEEDBELL		5

#define QT_COMMON_LEGACY_CONVICT	199
#define QT_COMMON_DEMOGORGON		200
#define QT_COMMON_LAMASHTU		201
#define QT_COMMON_SILENCE_REIGNS	202
#define QT_COMMON_SILENCE_FALLS		203

#define QT_COMMON_LEGACY_ELF		211
#define QT_COMMON_LEGACY_WORM		212
#define QT_COMMON_LEGACY_WORM_ELF	213
#define QT_COMMON_LEGACY_WORM_CONVICT	214
#define QT_COMMON_LEGACY_WORM2		215
#define QT_COMMON_LEGACY_WORM3		216
#define QT_COMMON_LEGACY_BINDER		217
#define QT_COMMON_LEGACY_ANA		218
#define QT_COMMON_LEGACY_ANA2		219
#define QT_COMMON_LEGACY_ANA3		220
#define QT_COMMON_LEGACY_ANA4		221
#define QT_COMMON_LEGACY_ANDROID	222
#define QT_COMMON_LEGACY_ANDROID2	223
#define QT_COMMON_LEGACY_ANDROID3	224
#define QT_COMMON_LEGACY_ANDROID4	225
#define QT_COMMON_DROW_HEALER_AMULET	226
#define QT_COMMON_LEGACY_UH		227
#define QT_COMMON_LEGACY_ANDROID2_PAUPER	228
#define QT_COMMON_LEGACY_ANDROID4_PAUPER	229
#endif	/***** !MAKEDEFS *****/

#endif /* QTEXT_H */
