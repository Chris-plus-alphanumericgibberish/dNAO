/*	SCCS Id: @(#)align.h	3.4	1991/12/29	*/
/* Copyright (c) Mike Stephenson, Izchak Miller  1991.		  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef ALIGN_H
#define ALIGN_H

typedef schar	aligntyp;	/* basic alignment type */

typedef struct	align {		/* alignment & record */
	aligntyp	type;
	int		record;
	int		sins; //records trasgressions that should lower the max record.
} align;

/* NetHack may be freely redistributed.  See license for details. */

enum {
	NO_GOD = 0,
//"Quetzalcoatl", "Camaxtli", "Huhetotl", /* Central American */
	GOD_QUETZALCOA,
	GOD_CAMAXTLI,
	GOD_HUHETOTL,
//"Mitra", "Crom", "Set", /* Hyborian */
	GOD_MITRA,
	GOD_CROM,
	GOD_SET,
//"Anu", "_Ishtar", "Anshar", /* Babylonian */
	GOD_ANU,
	GOD_ISHTAR,
	GOD_ANSHAR,
//"Ilmater", "Grumbar", "_Tymora",	/* Faerunian */
	GOD_ILMATER,
	GOD_GRUMBAR,
	GOD_TYMORA,
//"_Athena", "Hermes", "Poseidon", /* Greek */
	GOD_ATHENA,
	GOD_HERMES,
	GOD_POSEIDON,
//"Lugh", "_Brigit", "Manannan Mac Lir", /* Celtic */
	GOD_LUGH,
	GOD_BRIGIT,
	GOD_MANANNAN,
//"Shan Lai Ching", "Chih Sung-tzu", "Huan Ti", /* Chinese */
	GOD_SHAN,
	GOD_CHIH,
	GOD_HUAN,
//"God the Father", "_Mother Earth", "the Satan", /* Romanian, sorta */
	GOD_FATHER,
	GOD_MOTHER,
	GOD_SATAN,
//"the Lord", "_the deep blue sea", "the Devil",	/* Christian, sorta */
	GOD_LORD,
	GOD_SEA,
	GOD_DEVIL,
//"Issek", "Mog", "Kos", /* Nehwon */
	GOD_ISSEK,
	GOD_MOG,
	GOD_KOS,
//"Apollo", "_Latona", "_Diana", /* Roman */
	GOD_APOLLO,
	GOD_LATONA,
	GOD_DIANA,
//"_Amaterasu Omikami", "Raijin", "Susanowo", /* Japanese */
	GOD_AMATERASU,
	GOD_RAIJIN,
	GOD_SUSANOWO,
//"Blind Io", "_The Lady", "Offler", /* Discworld */
	GOD_IO,
	GOD_LADY,
	GOD_OFFLER,
//"Apollon", "Pan", "Dionysus", /* Thracian? */
	GOD_APOLLON,
	GOD_PAN,
	GOD_DIONYSUS,
//"Tyr", "Odin", "Loki", /* Norse */
	GOD_TYR,
	GOD_ODIN,
	GOD_LOKI,
//"Ptah", "Thoth", "Anhur", /* Egyptian */
	GOD_PTAH,
	GOD_THOTH,
	GOD_ANHUR,
//Elven shared
	GOD_OROME,
	GOD_YAVANNA,
	GOD_TULKAS,
//Elf priestess
	GOD_VARDA,
	GOD_VAIRE,
	GOD_NESSA,
//Elf priest
	GOD_MANWE,
	GOD_MANDOS,
	GOD_LORIEN,
//Future
	GOD_ILSENSINE,
//Drow
//Hedrow shared
	GOD_EDDERGUD,
	GOD_VHAERAUN,
	GOD_LOLTH,
//Hedrow noble
	GOD_VERTAS,
	GOD_PEN_A,
	GOD_KEPTOLO,
	GOD_GHAUNADAUR,
//Drow shared
	GOD_EILISTRAEE,
	GOD_KIARANSALI,
	//Lolth again
//Drow noble
	//Ver'tas, Kiaransali, and Lolth again
//Binder
	GOD_YALDABAOTH,
	GOD_VOID,
	GOD_SOPHIA,
//Dwarf
	GOD_MAHAL,
	GOD_HOLASHNER,
	GOD_ARMOK,
//Gnome
	GOD_KURTULMAK,
	GOD_GARL,
	GOD_URDLEN,
//Half dragon noble
	GOD_GWYN,
	GOD_GWYNEVERE,
	GOD_GWYNDOLIN,
//Orc noble
	GOD_ILNEVAL,
	GOD_LUTHIC,
	GOD_GRUUMSH,
//Orc noble, elf
	GOD_VANDRIA,
	GOD_CORELLON,
	GOD_SEHANINE,
//Orc noble, human
	GOD_CUTHBERT,
	GOD_HELM,
	GOD_MASK,
//Chaos quest
	GOD_SILENCE,
	GOD_CHAOS_FF,
//Unaligned
	GOD_MOLOCH,
	GOD_OTHER,
	GOD_SHUBBIE,
	GOD_NODENS,
	GOD_BAST,
	GOD_FRACTURE,
	GOD_SOTHOTH,
	MAX_GOD
};

/* bounds for "record" -- respect initial alignments of 10 */
#define ALIGNLIM	(min(100L, 10L + (moves/200L) - u.ualign.sins))

#define A_NONE		(-128)	/* the value range of type */

#define A_CHAOTIC	(-1)
#define A_NEUTRAL	 0
#define A_LAWFUL	 1

#define A_VOID		 -3 /* chosen to line up with topten and aligns[] (aligns[] is defined in role.c) */

#define A_COALIGNED	 1
#define A_OPALIGNED	(-1)

#define AM_NONE		 0
#define AM_CHAOTIC	 1
#define AM_NEUTRAL	 2
#define AM_LAWFUL	 4
#define AM_VOID		 8
//AM_SHRINE			16 //Note: this is in another .h file, but must be kept in sync

#define AM_MASK		 15

#define AM_SPLEV_CO	 3
#define AM_SPLEV_NONCO	 7

#define HOLY_HOLINESS	 1
#define NEUTRAL_HOLINESS 0
#define UNHOLY_HOLINESS -1
#define GOD(aligntyp, holiness, name)	{{aligntyp, 0, 0}, holiness, name, 0}
#define PANTHEON_DEFAULT(lname, nname, cname)	GOD(A_LAWFUL, HOLY_HOLINESS, lname), GOD(A_NEUTRAL, NEUTRAL_HOLINESS, nname), GOD(A_CHAOTIC, UNHOLY_HOLINESS, cname)
#define PANTHEON_HOLY(lname, nname, cname)	GOD(A_LAWFUL, HOLY_HOLINESS, lname), GOD(A_NEUTRAL, HOLY_HOLINESS, nname), GOD(A_CHAOTIC, HOLY_HOLINESS, cname)
#define PANTHEON_UNHOLY(lname, nname, cname)	GOD(A_LAWFUL, UNHOLY_HOLINESS, lname), GOD(A_NEUTRAL, UNHOLY_HOLINESS, nname), GOD(A_CHAOTIC, UNHOLY_HOLINESS, cname)
struct god_details {
	struct align galgin;
	char holiness;
	char *gname;
	int gangr, blessed, blesscnt;
};

extern struct god_details god_list[MAX_GOD]; //defined in decl.c

#define GA_NONE		 0
#define GA_CHAOTIC	 1
#define GA_NEUTRAL	 2
#define GA_LAWFUL	 3
#define GA_VOID		 4
#define GA_SILENCE	 5
#define GA_CHAOS_FF	 6
#define GA_DEMIURGE	 7
#define GA_SOPHIA	 8
#define GA_OTHER	 9
#define GA_MOTHER	10
#define GA_NODENS	11
#define GA_FRACTURE	12
#define GA_SOTHOTH	13
#define GA_NUM		14

#define GOAT_EAT_PASSIVE 0
#define GOAT_EAT_OFFERED 1
#define GOAT_EAT_MARKED 2

#define Amask2align(x)	((aligntyp) ( (x)==AM_NONE ? A_NONE : \
									  (x)==AM_CHAOTIC ? A_CHAOTIC :\
									  (x)==AM_NEUTRAL ? A_NEUTRAL :\
									  (x)==AM_LAWFUL ? A_LAWFUL :\
									  (x)==AM_VOID ? A_VOID : A_NONE\
									))
#define Align2amask(x)	( (x)==A_NONE ? AM_NONE : \
						  (x)==A_CHAOTIC ? AM_CHAOTIC :\
						  (x)==A_NEUTRAL ? AM_NEUTRAL :\
						  (x)==A_LAWFUL ? AM_LAWFUL :\
						  (x)==A_VOID ? AM_VOID : AM_NONE\
						)



#define Amask2gangr(x)	((aligntyp) ( (x)==AM_VOID ? GA_VOID :\
									  ((x)==AM_CHAOTIC && Role_if(PM_EXILE) && Is_astralevel(&u.uz)) ? GA_SOPHIA :\
									  (x)==AM_CHAOTIC ? GA_CHAOTIC :\
									  ((x)==AM_NEUTRAL && Role_if(PM_EXILE) && Is_astralevel(&u.uz)) ? GA_VOID :\
									  (x)==AM_NEUTRAL ? GA_NEUTRAL :\
									  ((x)==AM_LAWFUL && Role_if(PM_EXILE) && Is_astralevel(&u.uz)) ? GA_DEMIURGE :\
									  (x)==AM_LAWFUL ? GA_LAWFUL :\
									  In_mithardir_quest(&u.uz) ? GA_SILENCE :\
									  In_FF_quest(&u.uz) ? GA_CHAOS_FF :\
									  (Role_if(PM_EXILE) && In_quest(&u.uz)) ? GA_DEMIURGE :\
									  on_level(&lethe_temples,&u.uz) ? GA_MOTHER :\
									  on_level(&lethe_headwaters,&u.uz) ? GA_NODENS :\
									  In_outlands(&u.uz) ? GA_FRACTURE :\
									  on_level(&rlyeh_level,&u.uz) ? GA_SOTHOTH :\
									  In_lost_cities(&u.uz) ? GA_OTHER :\
									   GA_NONE\
									))
#define Align2gangr(x)	( (x)==A_VOID ? GA_VOID :\
						  ((x)==A_CHAOTIC && Role_if(PM_EXILE) && Is_astralevel(&u.uz)) ? GA_SOPHIA :\
						  (x)==A_CHAOTIC ? GA_CHAOTIC :\
						  ((x)==A_NEUTRAL && Role_if(PM_EXILE) && Is_astralevel(&u.uz)) ? GA_VOID :\
						  (x)==A_NEUTRAL ? GA_NEUTRAL :\
						  ((x)==A_LAWFUL && Role_if(PM_EXILE) && Is_astralevel(&u.uz)) ? GA_DEMIURGE :\
						  (x)==A_LAWFUL ? GA_LAWFUL :\
						  In_mithardir_quest(&u.uz) ? GA_SILENCE :\
						  In_FF_quest(&u.uz) ? GA_CHAOS_FF :\
						  (Role_if(PM_EXILE) && In_quest(&u.uz)) ? GA_DEMIURGE :\
						  on_level(&lethe_temples,&u.uz) ? GA_MOTHER :\
						  on_level(&lethe_headwaters,&u.uz) ? GA_NODENS :\
						  In_outlands(&u.uz) ? GA_FRACTURE :\
						  on_level(&rlyeh_level,&u.uz) ? GA_SOTHOTH :\
						  In_lost_cities(&u.uz) ? GA_OTHER :\
						  GA_NONE\
						)
#define Gangr2align(x)	((aligntyp) ( (x)==GA_NONE ? A_NONE : \
									  (x)==GA_CHAOTIC ? A_CHAOTIC :\
									  (x)==GA_NEUTRAL ? A_NEUTRAL :\
									  (x)==GA_LAWFUL ? A_LAWFUL :\
									  (x)==GA_SOPHIA ? A_CHAOTIC :\
									  (x)==GA_DEMIURGE ? A_LAWFUL :\
									  (x)==GA_VOID ? A_VOID : A_NONE\
									))
#define Gangr2amask(x)	( (x)==GA_NONE ? AM_NONE : \
						  (x)==GA_CHAOTIC ? AM_CHAOTIC :\
						  (x)==GA_NEUTRAL ? AM_NEUTRAL :\
						  (x)==GA_LAWFUL ? AM_LAWFUL :\
						  (x)==GA_SOPHIA ? AM_CHAOTIC :\
						  (x)==GA_DEMIURGE ? AM_LAWFUL :\
						  (x)==GA_VOID ? AM_VOID : AM_NONE\
						)
#endif /* ALIGN_H */
