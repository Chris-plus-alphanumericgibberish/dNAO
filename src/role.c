/*	SCCS Id: @(#)role.c	3.4	2003/01/08	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985-1999. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/*** Table of all roles ***/
/* According to AD&D, HD for some classes (ex. Wizard) should be smaller
 * (4-sided for wizards).  But this is not AD&D, and using the AD&D
 * rule here produces an unplayable character.  Thus I have used a minimum
 * of an 10-sided hit die for everything.  Another AD&D change: wizards get
 * a minimum strength of 4 since without one you can't teleport or cast
 * spells. --KAA
 *
 * As the wizard has been updated (wizard patch 5 jun '96) their HD can be
 * brought closer into line with AD&D. This forces wizards to use magic more
 * and distance themselves from their attackers. --LSZ
 *
 * With the introduction of races, some hit points and energy
 * has been reallocated for each race.  The values assigned
 * to the roles has been reduced by the amount allocated to
 * humans.  --KMH
 *
 * God names use a leading underscore to flag goddesses.
 */
struct Role roles[] = {
{	{"Archeologist", 0}, {
	{"Digger",      0},
	{"Field Worker",0},
	{"Investigator",0},
	{"Exhumer",     0},
	{"Excavator",   0},
	{"Spelunker",   0},
	{"Speleologist",0},
	{"Collector",   0},
	{"Curator",     0} },
	GOD_QUETZALCOATL, GOD_CAMAXTLI, GOD_HUHETOTL, /* Central American */
	"Arc", "the College of Archeology", "the Tomb of the Toltec Kings",
	PM_ARCHEOLOGIST, NON_PM, NON_PM,
	PM_LORD_CARNARVON, PM_STUDENT, PM_MINION_OF_HUHETOTL,
	NON_PM, PM_HUMAN_MUMMY, S_SNAKE, S_MUMMY,
	ART_ITLACHIAYAQUE,
	MA_HUMAN|MA_DWARF|MA_GNOME|MA_CLOCK|MA_VAMPIRE|MA_DRAGON, ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10, 10,  7,  7,  7 },
	{  20, 20, 20, 10, 20, 10 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  0, 1 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_MAGIC_MAPPING,   -9
},
{	{"Anachrononaut", 0}, {
	{"Survivor",   0},
	{"Recruit",0},
	{"Insurgent",0},
	{"Trooper",     0},
	{"Rebel Leader",   0},
	{"Chrono Commandant",   0},
	{"Anachrononaut",0},
	{"Fatebreaker",   0},
	{"Hope of the Future",     0} },
	0, 0, 0, /* chosen randomly from among the other roles */
	"Ana", "the Last Redoubt", "Ground Zero",
	PM_ANACHRONONAUT, NON_PM, NON_PM,
	PM_SARA__THE_LAST_ORACLE, PM_TROOPER, NON_PM,
	NON_PM, NON_PM, NON_PM, NON_PM,
	ART_ANNULUS,
	MA_HUMAN|MA_DWARF|MA_ELF|MA_VAMPIRE|MA_DRAGON|MA_CLOCK, ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  12, 10,  7, 10,  10,  7 },
	{  15, 17, 15, 12, 15, 15 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 0,  0, 2,  0, 2 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_SLOW_MONSTER,   -9
},
{	{"Barbarian", 0}, {
	{"Plunderer",   "Plunderess"},
	{"Pillager",    0},
	{"Bandit",      0},
	{"Brigand",     0},
	{"Raider",      0},
	{"Reaver",      0},
	{"Slayer",      0},
	{"Chieftain",   "Chieftainess"},
	{"Conqueror",   "Conqueress"} },
	GOD_MITRA, GOD_CROM, GOD_SET, /* Hyborian */
	"Bar", "the Camp of the Duali Tribe", "the Duali Oasis",
	PM_BARBARIAN, NON_PM, NON_PM,
	PM_PELIAS, PM_CHIEFTAIN, PM_THOTH_AMON,
	PM_OGRE, PM_TROLL, S_OGRE, S_TROLL,
	ART_HEART_OF_AHRIMAN,
	MA_HUMAN|MA_ORC|MA_VAMPIRE|MA_DRAGON, ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  16,  7,  7, 15, 16,  6 },
	{  30,  6,  7, 20, 30,  7 },
	/* Init   Lower  Higher */
	{ 14, 0,  0,10,  0, 2 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	10, 14, 0, 0,  8, A_INT, SPE_HASTE_SELF,      -1000
},
{	{"Binder", 		 0}, {
	{"Exile",        0},
	{"Heretic",      0},
	{"Cultist",      0},
	{"Binder",       0},
	{"Akousmatikoi", 0},
	{"Mathematikoi", 0},
	{"Doctor",       0},
	{"Unbinder",     0},
	{"Gnostikos",    0} },
	0, 0, 0,	/* chosen randomly from among the other roles */
	"Bin", "the lost library", "the Tower of Woe",
	PM_EXILE, NON_PM, NON_PM,
	PM_STRANGE_CORPSE, PM_PEASANT, PM_ACERERAK,
	PM_SKELETON, PM_LICH, S_HUMAN, S_HUMANOID,
	ART_PEN_OF_THE_VOID,
	MA_HUMAN|MA_DWARF|MA_GNOME|MA_ELF|MA_ORC|MA_CLOCK|MA_VAMPIRE|MA_DRAGON|MA_ANIMAL|MA_FEY, ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_CHAOTIC|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  6,  6,  6,  6,  6,  6 },
	{ 11,  9,  9, 11, 11,  9 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 10,  0, 2 },	/* Hit points */
	{  5, 0,  0,  1,  1, 0 },15,	/* Energy */
	-5, 10, 5, 10,  25, 0/*Special*/, SPE_SLEEP, -14
},
{	{"Caveman", "Cavewoman"}, {
	{"Troglodyte",  0},
	{"Aborigine",   0},
	{"Wanderer",    0},
	{"Vagrant",     0},
	{"Wayfarer",    0},
	{"Roamer",      0},
	{"Nomad",       0},
	{"Rover",       0},
	{"Pioneer",     0} },
	GOD_ANU, GOD_ISHTAR, GOD_ANSHAR, /* Babylonian */
	"Cav", "the Caves of the Ancestors", "the Dragon's Lair",
	PM_CAVEMAN, PM_CAVEWOMAN, PM_LITTLE_DOG,
	PM_SHAMAN_KARNOV, PM_NEANDERTHAL, PM_CHROMATIC_DRAGON,
	PM_BUGBEAR, PM_HILL_GIANT, S_HUMANOID, S_GIANT,
	ART_SCEPTRE_OF_MIGHT,
	MA_HUMAN|MA_DWARF|MA_GNOME|MA_DRAGON|MA_ANIMAL, ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  7,  7,  8,  6 },
	{  30,  6,  7, 20, 30,  7 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 8,  0, 2 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 12, 0, 1,  8, A_INT, SPE_DIG, -1000
},
#ifdef CONVICT
{	{"Convict", 0}, {
	{"Detainee",     0},
	{"Inmate",   0},
	{"Jail-bird",0},
	{"Prisoner",0},
	{"Outlaw",    0},
	{"Crook",   0},
	{"Desperado",     0},
	{"Felon",    0},
	{"Fugitive",  0} },
	GOD_ILMATER, GOD_GRUMBAR, GOD_TYMORA,	/* Faerunian */
	"Con", "Castle Waterdeep Dungeon", "the Warden's Level",
	PM_CONVICT, NON_PM, PM_SEWER_RAT,
	PM_ROBERT_THE_LIFER, PM_INMATE, PM_WARDEN_ARIANNA,
	PM_SOLDIER_ANT, PM_MALKUTH_SEPHIRAH, S_RODENT, S_SPIDER,
	ART_IRON_SPOON_OF_LIBERATION,
	// ART_IRON_BALL_OF_LIBERATION,
	MA_HUMAN|MA_DWARF|MA_GNOME|MA_ORC|MA_ELF|MA_VAMPIRE|MA_DRAGON|MA_ANIMAL|MA_FEY, ROLE_MALE|ROLE_FEMALE |
	  ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  7,  7, 13,  6 },
	{  20, 20, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 8,  0, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },10,	/* Energy */
	-10, 5, 0, 2, 10, A_INT, SPE_TELEPORT_AWAY,   -19
},
#endif	/* CONVICT */
{	{"Healer", 0}, {
	{"Rhizotomist",    0},
	{"Empiric",        0},
	{"Embalmer",       0},
	{"Dresser",        0},
	{"Medicus ossium", "Medica ossium"},
	{"Herbalist",      0},
	{"Magister",       "Magistra"},
	{"Physician",      0},
	{"Chirurgeon",     0} },
	GOD_ATHENA, GOD_HERMES, GOD_POSEIDON, /* Greek */
	"Hea", "the Temple of Epidaurus", "the Temple of Coeus",
	PM_HEALER, NON_PM, NON_PM,
	PM_HIPPOCRATES, PM_ATTENDANT, PM_CYCLOPS,
	PM_GIANT_RAT, PM_SNAKE, S_RODENT, S_YETI,
	ART_STAFF_OF_AESCULAPIUS,
	MA_HUMAN|MA_GNOME|MA_FEY, ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7,  7, 13,  7, 11, 16 },
	{  15, 20, 20, 15, 25, 5 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 4,  1, 2,  0, 3 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_FULL_HEALING,   -14
},
{	{"Knight", 0}, {
	{"Gallant",     0},
	{"Esquire",     0},
	{"Bachelor",    0},
	{"Sergeant",    0},
	{"Knight",      0},
	{"Banneret",    0},
	{"Chevalier",   "Chevaliere"},
	{"Seignieur",   "Dame"},
	{"Paladin",     0} },
	GOD_LUGH, GOD_BRIGIT, GOD_MANANNAN_MAC_LIR, /* Celtic */
	"Kni", "Camelot Castle", "the Isle of Glass",
	PM_KNIGHT, NON_PM, PM_PONY,
	PM_KING_ARTHUR, PM_PAGE, PM_NIMUNE,
	PM_QUASIT, PM_ELF_LORD, S_IMP, S_NYMPH, /*demons and fairies*/
	ART_MAGIC_MIRROR_OF_MERLIN,
	MA_DWARF|MA_HUMAN|MA_CLOCK|MA_DRAGON, ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL,
	/* Str Int Wis Dex Con Cha */
	{  13,  7, 14,  8, 10, 17 },
	{  30, 15, 15, 10, 20, 10 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 8,  0, 2 },	/* Hit points */
	{  1, 4,  1, 2,  0, 3 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD, -1000
},
{	{"Monk", 0}, {
	{"Candidate",         0},
	{"Novice",            0},
	{"Initiate",          0},
	{"Student of Stones", 0},
	{"Student of Waters", 0},
	{"Student of Metals", 0},
	{"Student of Winds",  0},
	{"Student of Fire",   0},
	{"Master",            0} },
	GOD_SHAN_LAI_CHING, GOD_CHIH_SUNG_TZU, GOD_HUAN_TI, /* Chinese */
	"Mon", "the Monastery of Chan-Sune",
	  "the Monastery of the Earth-Lord",
	PM_MONK, NON_PM, NON_PM,
	PM_GRAND_MASTER, PM_ABBOT, PM_MASTER_KAEN,
	NON_PM, NON_PM, NON_PM, NON_PM,
	ART_EYES_OF_THE_OVERWORLD,
	MA_HUMAN|MA_CLOCK|MA_ANIMAL|MA_DRAGON|MA_FEY, ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  8,  8,  7,  7 },
	{  25, 10, 20, 20, 15, 10 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 8,  1, 0 },	/* Hit points */
	{  8, 0,  0, 2,  0, 2 },10,	/* Energy */
	10, 8,-2, 2, 20, A_WIS, SPE_RESTORE_ABILITY, -24
},
{	{"Madman", "Madwoman"}, {
	{"Ward",     0},
	{"Escapee",   0},
	{"Drifter",0},
	{"Derelict",0},
	{"Raver",    0},
	{"Lunatic",   0},
	{"Maniac",     0},
	{"Augur",    0},
	{"Seer",  0} },
	GOD_ZO_KALAR, GOD_LOBON, GOD_TAMASH,	/* Dreamlands */
	"Mad", "Archer Asylum", "the ground floor",
	PM_MADMAN, PM_MADWOMAN, NON_PM,
	PM_CASSILDA_THE_IRON_MAIDEN, PM_PATIENT, PM_DOCTOR_ARCHER,
	PM_ENORMOUS_RAT, PM_CONTAMINATED_PATIENT, S_RODENT, S_BAT,
	ART_STAR_OF_HYPERNOTUS,
	MA_HUMAN|MA_DWARF|MA_GNOME|MA_ORC|MA_ELF|MA_VAMPIRE|MA_DRAGON|MA_FEY, ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7, 10, 3, 10,  7, 10 },
	{  10, 10, 3, 20, 20, 30 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 8,  0, 1 },	/* Hit points */
	{  4, 3,  0, 1,  2, 4 },14,	/* Energy */
	10, 8,-2, 2, 20, A_CHA, SPE_CONFUSE_MONSTER, -24
},
	// "The Silver Fire", "_The Fury", "The Shadow",	/* Sorta-eberron */
	// "_The Inheritor", "_The Dawnflower", "_The Everbloom",	/* Sorta-glorion */
{	{"Nobleman", "Noblewoman"}, {
	{"Pargar",       0},
	{"Cneaz",	     0},
	{"Ban",		     0},
	{"Jude",	     0},
	{"Boier",	     0},
	{"Cupar",	     0},
	{"Clucer",	     0},
	{"Domn",	     0},
	{"Domnitor",     0} },
	GOD_GOD_THE_FATHER, GOD_MOTHER_EARTH, GOD_THE_SATAN, /* Romanian, sorta */
	"Nob", "your ancestral home",
	  "the rebel village",
	PM_NOBLEMAN, PM_NOBLEWOMAN, PM_PONY,
	PM_OLD_FORTUNE_TELLER, PM_SERVANT, PM_REBEL_RINGLEADER,
	PM_SOLDIER, PM_PEASANT, S_HUMANOID, S_HUMAN,
	ART_MANTLE_OF_HEAVEN,
	MA_DWARF|MA_ELF|MA_HUMAN|MA_VAMPIRE|MA_DRAGON, ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   10,10,  7, 10,  7,  7 },
	{  20, 18, 10, 20, 15, 17 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 2,  0, 2,  0, 2 },10,	/* Energy */
	10, 4,-4, 4, 8, A_INT, SPE_PROTECTION, -1000
},
{	{"Priest", "Priestess"}, {
	{"Aspirant",    0},
	{"Acolyte",     0},
	{"Adept",       0},
	{"Priest",      "Priestess"},
	{"Curate",      0},
	{"Canon",       "Canoness"},
	{"Lama",        0},
	{"Patriarch",   "Matriarch"},
	{"High Priest", "High Priestess"} },
	0, 0, 0,	/* chosen randomly from among the other roles */
	"Pri", "the Great Temple", "the Temple of the Legion",
	PM_PRIEST, PM_PRIESTESS, NON_PM,
	PM_ARCH_PRIEST, PM_ACOLYTE, PM_LEGION,
	PM_LEGIONNAIRE, PM_BLACK_PUDDING, S_ZOMBIE, S_PUDDING,
	ART_MITRE_OF_HOLINESS,
	MA_HUMAN|MA_ELF|MA_CLOCK|MA_DRAGON|MA_ANIMAL, ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7,  7, 10,  7,  7,  7 },
	{  15, 10, 30, 15, 20, 10 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 8,  1, 0 },	/* Hit points */
	{  4, 3,  2, 4,  0, 4 },10,	/* Energy */
	0, 3,-2, 1, 10, A_WIS, SPE_REMOVE_CURSE,    -7
},
{	{"Pirate", 0}, {
	{"Landlubber",    	0},
	{"Swabbie",        	0},
	{"Cutthroat",       0},
	{"Bosun",      		0},
	{"Second Mate",     0},
	{"First Mate",      0},
	{"Captain",			0},
	{"Pirate Lord",   	0},
	{"Dread Pirate",  	0} },
	GOD_THE_LORD, GOD_THE_DEEP_BLUE_SEA, GOD_THE_DEVIL,	/* Christian, sorta */
	"Pir", "Tortuga", "Shipwreck Island",
	PM_PIRATE, NON_PM, NON_PM,
	PM_MAYOR_CUMMERBUND, PM_PIRATE_BROTHER, PM_BLACKBEARD_S_GHOST,
	PM_SKELETAL_PIRATE, PM_SOLDIER, S_RODENT, S_ELEMENTAL, /* Ghost pirates, soldiers, rats in the food stores, and the occasional storm*/
	ART_TREASURY_OF_PROTEUS,
	MA_HUMAN|MA_ELF, ROLE_MALE|ROLE_FEMALE |
	  ROLE_CHAOTIC|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  10,  7, 7,  10, 10,  7 },
	{  22, 15, 10, 22, 20, 10 },
	/* Init   Lower  Higher */
	{ 20, 0,  0, 6,  0, 2 },	/* Hit points (10 +d8 for 11 levels comes up a bit over 20 +d6 for 11 levels) */
	{  1, 0,  0, 1,  0, 0 },12,	/* Energy */
	10, 8, 0, 2,  9, A_INT, SPE_CAUSE_FEAR,    -1000
},
  /* Note:  Rogue precedes Ranger so that use of `-R' on the command line
     retains its traditional meaning. */
{	{"Rogue", 0}, {
	{"Footpad",     0},
	{"Cutpurse",    0},
	{"Rogue",       0},
	{"Pilferer",    0},
	{"Robber",      0},
	{"Burglar",     0},
	{"Filcher",     0},
	{"Magsman",     "Magswoman"},
	{"Thief",       0} },
	GOD_ISSEK, GOD_MOG, GOD_KOS, /* Nehwon */
	"Rog", "the Thieves' Guild Hall", "the Assassins' Guild Hall",
	PM_ROGUE, NON_PM, NON_PM,
	PM_MASTER_OF_THIEVES, PM_THUG, PM_MASTER_ASSASSIN,
	PM_LEPRECHAUN, PM_GUARDIAN_NAGA, S_NYMPH, S_NAGA,
	ART_MASTER_KEY_OF_THIEVERY,
	MA_HUMAN|MA_ORC|MA_VAMPIRE|MA_ELF|MA_DRAGON|MA_ANIMAL, ROLE_MALE|ROLE_FEMALE |
	  ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7, 10,  7,  6 },
	{  20, 10, 10, 30, 20, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 6,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 8, 0, 1,  9, A_INT, SPE_DETECT_TREASURE, -1000
},
{	{"Ranger", 0}, {
	{"Tenderfoot",    0},
	{"Lookout",       0},
	{"Trailblazer",   0},
	{"Reconnoiterer", "Reconnoiteress"},
	{"Scout",         0},
	{"Arbalester",    0},	/* One skilled at crossbows */
	{"Archer",        0},
	{"Sharpshooter",  0},
	{"Marksman",      "Markswoman"} },
	GOD_APOLLO, GOD_LATONA, GOD_DIANA, /* Roman */
	"Ran", "Orion's camp", "the cave of the wumpus",
	PM_RANGER, NON_PM, PM_LITTLE_DOG /* Orion & canis major */,
	PM_ORION, PM_HUNTER, PM_SCORPIUS,
	PM_FOREST_CENTAUR, PM_SCORPION, S_CENTAUR, S_SPIDER,
	ART_LONGBOW_OF_DIANA,
	MA_HUMAN|MA_ELF|MA_GNOME|MA_ORC|MA_DRAGON|MA_ANIMAL, ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  13, 13, 13,  9, 13,  7 },
	{  30, 10, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 13, 0,  0, 6,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_INVISIBILITY,   -1000
},
{	{"Samurai", 0}, {
	{"Hatamoto",    0},  /* Banner Knight */
	{"Ronin",       0},  /* no allegiance */
	{"Ninja",       "Kunoichi"},  /* secret society */
	{"Joshu",       0},  /* heads a castle */
	{"Ryoshu",      0},  /* has a territory */
	{"Kokushu",     0},  /* heads a province */
	{"Daimyo",      0},  /* a samurai lord */
	{"Kuge",        0},  /* Noble of the Court */
	{"Shogun",      0} },/* supreme commander, warlord */
	GOD_AMATERASU_OMIKAMI, GOD_RAIJIN, GOD_SUSANOWO, /* Japanese */
	"Sam", "the Castle of the Taro Clan", "the Shogun's Castle",
	PM_SAMURAI, NON_PM, PM_LITTLE_DOG,
	PM_LORD_SATO, PM_ROSHI, PM_ASHIKAGA_TAKAUJI,
	PM_WOLF, PM_STALKER, S_DOG, S_ELEMENTAL,
	ART_TSURUGI_OF_MURAMASA,
	MA_HUMAN|MA_CLOCK|MA_DRAGON|MA_FEY, ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL,
	/* Str Int Wis Dex Con Cha */
	{  10,  8,  7, 10, 17,  6 },
	{  30, 10,  8, 30, 14,  8 },
	/* Init   Lower  Higher */
	{ 13, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 10, 0, 0,  8, A_INT, SPE_CLAIRVOYANCE,    -10
},
#ifdef TOURIST
{	{"Tourist", 0}, {
	{"Rambler",     0},
	{"Sightseer",   0},
	{"Excursionist",0},
	{"Peregrinator","Peregrinatrix"},
	{"Traveler",    0},
	{"Journeyer",   0},
	{"Voyager",     0},
	{"Explorer",    0},
	{"Adventurer",  0} },
	GOD_BLIND_IO, GOD_THE_LADY, GOD_OFFLER, /* Discworld */
	"Tou", "Ankh-Morpork", "the Thieves' Guild Hall",
	PM_TOURIST, NON_PM, NON_PM,
	PM_TWOFLOWER, PM_GUIDE, PM_MASTER_OF_THIEVES,
	PM_DWARF, PM_ROCK_TROLL, S_RODENT, S_HUMANOID,
	ART_YENDORIAN_EXPRESS_CARD,
	MA_HUMAN, ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10,  6,  7,  7, 10 },
	{  15, 10, 10, 15, 30, 20 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 8,  0, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },14,	/* Energy */
	0, 5, 1, 2, 10, A_INT, SPE_CHARM_MONSTER,   -15
},
#endif
#ifdef BARD
{	{"Troubadour", 0}, {
	{"Rhymer",      0},
	{"Lyrist",      0},
	{"Sonneteer",   0},
	{"Jongleur",    0},
	{"Troubadour",  0},
	{"Minstrel",    0},
	{"Lorist",      0},
	{"Bard",        0},
	{"Master Bard", 0} },
	GOD_APOLLON, GOD_PAN, GOD_DIONYSUS, /* Thracian? */
	"Brd", "the Conservatorium", "the Island of Anthemoessa",
	PM_BARD, NON_PM, NON_PM,
	PM_PINDAR, PM_RHYMER, PM_AGLAOPE,
	PM_SNAKE, PM_WHITE_UNICORN, S_SNAKE, S_UNICORN,
	ART_LYRE_OF_ORPHEUS,
	MA_HUMAN|MA_ELF|MA_GNOME|MA_ORC|MA_CLOCK|MA_DRAGON|MA_FEY|MA_VAMPIRE, ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7, 10,  6, 10 },
	{  10, 10, 15, 25, 10, 30 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 4,  1, 0 },	/* Hit points */
	{  4, 3,  1, 2,  0, 3 },10,	/* Energy */
	10, 3,-3, 2, 9, A_INT, SPE_CREATE_MONSTER, -24
},
#endif
{	{"Valkyrie", 0}, {
	{"Stripling",   0},
	{"Skirmisher",  0},
	{"Fighter",     0},
	{"Man-at-arms", "Woman-at-arms"},
	{"Warrior",     0},
	{"Swashbuckler",0},
	{"Hero",        "Heroine"},
	{"Champion",    0},
	{"Lord",        "Lady"} },
	GOD_TYR, GOD_ODIN, GOD_LOKI, /* Norse */
	"Val", "the Shrine of Destiny", "the cave of Surtur",
	PM_VALKYRIE, NON_PM, NON_PM /*PM_WINTER_WOLF_CUB*/,
	PM_NORN, PM_WARRIOR, PM_LORD_SURTUR,
	PM_FIRE_ANT, PM_FIRE_GIANT, S_ANT, S_GIANT,
	ART_ORB_OF_FATE,
	MA_HUMAN|MA_DWARF|MA_FEY, ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  12,  10, 12, 10, 10, 12 },
	{  18,  14, 16, 18, 20, 14 },
	/* Init   Lower  Higher */
	{ 14, 0,  0, 8,  0, 2 },	/* Hit points */
	{  1, 0,  2, 2,  0, 2 },10,	/* Energy */
	0, 10,-2, 0,  9, A_WIS, SPE_CONE_OF_COLD,    -1000 /*Valks CAN ALWAYS cast Cone of Cold*/
},
{	{"Wizard", 0}, {
	{"Evoker",      0},
	{"Conjurer",    0},
	{"Thaumaturge", 0},
	{"Magician",    0},
	{"Enchanter",   "Enchantress"},
	{"Sorcerer",    "Sorceress"},
	{"Necromancer", 0},
	{"Wizard",      0},
	{"Archmage",        0} },
	GOD_PTAH, GOD_THOTH, GOD_ANHUR, /* Egyptian */
	"Wiz", "the Lonely Tower", "the Tower of Darkness",
	PM_WIZARD, NON_PM, PM_KITTEN,
	PM_NEFERET_THE_GREEN, PM_APPRENTICE, PM_DARK_ONE,
	PM_VAMPIRE_BAT, PM_XORN, S_BAT, S_WRAITH,
	ART_EYE_OF_THE_AETHIOPICA,
	MA_HUMAN|MA_ELF|MA_GNOME|MA_ORC|MA_VAMPIRE|MA_DRAGON, ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7, 10,  7,  7,  7,  7 },
	{  10, 30, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 4,  0, 1 },	/* Hit points */
	{  4, 3,  2, 4,  0, 6 },12,	/* Energy */
	0, 1, 0, 3, 10, A_INT, SPE_MAGIC_MISSILE,   -5
},
/* Array terminator */
{{0, 0}}
};

const struct Role WtWrole = {	
	{"Worm that Walks", 0}, {
	{"",      0},
	{"",0},
	{"",0},
	{"",     0},
	{"",   0},
	{"",   0},
	{"",0},
	{"",   0},
	{"",     0} },
	0, 0, 0, /* Determined by Mask */
	"", "", "", /* Determined by Mask */
	PM_WORM_THAT_WALKS, NON_PM, NON_PM,
	NON_PM, NON_PM, NON_PM, /* Determined by Mask */
	NON_PM, NON_PM, NON_PM, NON_PM, /* Determined by Mask */
	0, /* Determined by Mask */
	0, /* Determined by Mask */
	0, /* Determined by Mask */
	/* Str Int Wis Dex Con Cha */
	{  3,  3,  3,  3,  3,  3 },
	{ 10, 10, 10, 10, 10, 10 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_MAGIC_MAPPING,   -9
};

/*
{	{"Ranger", 0}, {
	{"Tenderfoot",    0},
	{"Lookout",       0},
	{"Trailblazer",   0},
	{"Reconnoiterer", "Reconnoiteress"},
	{"Scout",         0},
	{"Arbalester",    0},
	{"Archer",        0},
	{"Sharpshooter",  0},
	{"Marksman",      "Markswoman"} },
	0, 0, 0,
	"Ran", "Orion's camp", "the cave of the wumpus",
	PM_RANGER, NON_PM, PM_LITTLE_DOG,
	PM_ORION, PM_HUNTER, PM_SCORPIUS,						//Should be PM_NECROMACER, PM_HIGH_ELF, 
	PM_FOREST_CENTAUR, PM_SCORPION, S_CENTAUR, S_SPIDER,	//Should be PM_MORDOR_ORC, PM_OLOG_HAI, S_ORC, S_TROLL
	ART_LONGBOW_OF_DIANA,
	MA_HUMAN|MA_ELF|MA_GNOME|MA_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	// Str Int Wis Dex Con Cha
	{  13, 13, 13,  9, 13,  7 },
	{  30, 10, 10, 20, 20, 10 },
	// Init   Lower  Higher
	{ 13, 0,  0, 6,  1, 0 },	// Hit points
	{  1, 0,  0, 1,  0, 1 },12,	// Energy (Ranger)
	{  4, 3,  0, 2,  0, 3 },12,	// Energy (Wizard)
	10, 9, 2, 1, 10, A_INT, SPE_INVISIBILITY,   -13
} 
*/
struct RoleName DwarfRanks[9] = {
	{"Khuzd",       	0},
	{"Khuzd",       	0},  /* dwarf */
	{"Felakgundu",		0},	 /* cave digger (Q.) ['a] */
	{"Azaghal",     	0},  /* warrior? */
	{"Khuzdu-barak",	0},	 /* Dwarf of Axes - guess */
	{"Khuzdu-ganad",	0},  /* Dwarf of Halls - guess */
	{"Khuzdu-kibil",	0},  /* Dwarf of silver - guess */
	{"Khuzdu-gathal",	0},  /* Dwarf of fortresses - guess */
	{"Zubd",     		0}}; /* Lord (as in lord of moria) - guess */
	

int ElfRangerFavoredSpell = SPE_DETECT_MONSTERS;
int ElfRangerFavoredBonus = -26;

struct RoleName ElfRangerRanks[9] = {
	{"Edhel",       "Elleth"},
	{"Edhel",       "Elleth"},      /* elf-maid */
	{"Ohtar",       "Ohtie"},       /* warrior */
	{"Kano",			/* commander (Q.) ['a] */
			"Kanie"},	/* educated guess, until further research- SAC */
	{"Arandur",			/* king's servant, minister (Q.) - guess */
			"Aranduriel"},	/* educated guess */
	{"Hir",         "Hiril"},       /* lord, lady (S.) ['ir] */
	{"Aredhel",     "Arwen"},       /* noble elf, maiden (S.) */
	{"Ernil",       "Elentariel"},  /* prince (S.), elf-maiden (Q.) */
	{"Elentar",     "Elentari"}};	/* Star-king, -queen (Q.) */

int DrowPriestessFavoredSpell = SPE_CREATE_FAMILIAR;
int DrowPriestessFavoredBonus = -20;

struct RoleName DrowRanks[9] = {
	{"Wiu",			"Ligrr"},		/* boy, girl */
	{"Wanre",		"Wenress"},		/* servant, maiden */
	{"Glenn",		"Kyorl"},		/* soldier, guard*/
	{"Elg'hasek",	"Venta'kyorl"},	/* ranger, captain */
	{"Beldrar",		"An'kin"},		/* builder, teacher */
	{"Helothann",		"Jallil"},	/* traveler, lady */
	{"Linthar",     "Sil'in"},		/* bard, noble */
	{"Kas'ka",       "Qu'ess"},		/* merchant, prince */
	{"Zil",     "Val'sharess"}};	/* Consort, Queen */
	

struct RoleName FemHDNobRanks[9] = {
	{"Deprived",   		0},
	{"Forlorn Wanderer",0},
	{"Forest Hunter",	0},
	{"Forlorn Warrior", 0},
	{"Knight Errant",	0},
	{"Winter Herald",	0},
	{"Livid Saint",		0},
	{"Heir of Frost",	0},
	{"Lord of Rime",	0}};

/* The player's role, created at runtime from initial
 * choices.  This may be munged in role_init().
 */

struct Role urole =
{	{"Undefined", 0}, { {0, 0}, {0, 0}, {0, 0},
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	GOD_NONE, GOD_NONE, GOD_NONE, "Xxx", "home", "locate",
	NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM,
	NON_PM, NON_PM, 0, 0, 0, 0, 0,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7,  7,  7,  7 },
	{  20, 15, 15, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 0,  0, 2,  0, 3 },14,	/* Energy */
	0, 10, 0, 0,  4, A_INT, 0, -3
};



/* Table of all races */
const struct Race races[] = {
{	"human", "human", "humanity", "Hum",
	{"man", "woman"},
	PM_HUMAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN,
	ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MA_HUMAN, 0, MA_GNOME|MA_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 },		/* Energy */
	NORMALNIGHTVIS,
	SPE_ABJURATION, -20
},
{	"clockwork automaton", "automatic", "clockwork-kind", "Clk",
	{0, 0},
	PM_CLOCKWORK_AUTOMATON, NON_PM, NON_PM, NON_PM,
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	MA_CLOCK, MA_DWARF, MA_GNOME,
	/*    Str     Int Wis Dex Con Cha */
	{      8,      8,  8,  8,  8,  8 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  3, 0,  3, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 },	/* Energy */
	NORMALNIGHTVIS,
	SPE_WIZARD_LOCK, -1000
},
{	"Chiropteran", "Chiropteric", "Chiropteran-hood", "Bat",
	{0, 0},
	PM_CHIROPTERAN, NON_PM, PM_CHIROPTERAN_MUMMY, PM_CHIROPTERAN,
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_CHAOTIC|ROLE_NEUTRAL,
	MA_ANIMAL, 0, MA_GNOME|MA_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(20),  16, 16, 20, 14, 16 },
	/* Init   Lower  Higher */
	{  5, 0,  1, 3,  1, 2 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 },	/* Energy */
	NORMALNIGHTVIS,
	SPE_MAGIC_MAPPING, -15
},
{	"dwarf", "dwarven", "dwarvenkind", "Dwa",
	{0, 0},
	PM_DWARF, NON_PM, PM_DWARF_MUMMY, PM_DWARF,
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL,
	MA_DWARF, MA_DWARF|MA_GNOME, MA_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 16, 16, 20, 20, 16 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 3,  2, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 },	/* Energy */
	NORMALNIGHTVIS,
	SPE_DIG, -15
},
{	"Drow", "droven", "drovenkind", "Dro",
	{"hedrow", "drow"},
	PM_DROW, NON_PM, PM_DROW_MUMMY, PM_HEDROW_ZOMBIE,
	ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MA_ELF, 0, MA_FEY|MA_ELF|MA_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{   18,    20, 18, 20, 16, 20 },
	/* Init   Lower  Higher */
	{  8, 0,  0, 4,  1, 0 },	/* Hit points */
	{  8, 0,  0, 4,  0, 4 },	/* Energy */
	NO_NIGHTVISION,
	SPE_SLEEP, -5
},
{	"elf", "elven", "elvenkind", "Elf",
	{"elf", "elf-maid"},
	PM_ELF, NON_PM, PM_ELF_MUMMY, PM_ELF,
	ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MA_ELF, MA_ELF, MA_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{   18,    20, 20, 20, 16, 18 },
	/* Init   Lower  Higher */
	{   7, 0,  3, 0,  1, 0 },	/* Hit points */
	{   7, 0,  3, 0,  3, 0 },	/* Energy */
	NIGHTVISION3,
	SPE_REMOVE_CURSE, -10
},
{	"gnome", "gnomish", "gnomehood", "Gno",
	{0, 0},
	PM_GNOME, NON_PM, PM_GNOME_MUMMY, PM_GNOME,
	ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	MA_GNOME, MA_DWARF|MA_GNOME, MA_HUMAN,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{STR18(50),19, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{ 12, 0,  4, 0,  2, 2 },	/* Energy */
	NIGHTVISION2,
	SPE_INVISIBILITY, -10
},
{	"Half-dragon", "Half-dragon", "Half-dragonhood", "Hlf",
	{0, 0},
	PM_HALF_DRAGON, NON_PM, PM_HALF_DRAGON_MUMMY, PM_HALF_DRAGON,
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_CHAOTIC|ROLE_NEUTRAL,
	MA_DRAGON, 0, MA_ELF|MA_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25),  20, 20, 10, 20, 20 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 4,  2, 0 },	/* Hit points */
	{  4, 0,  0, 4,  2, 0 },	/* Energy */
	NORMALNIGHTVIS,
	SPE_DETECT_TREASURE, -10
},
{	"incantifier", "incantifier", "wanterkind", "Inc",
	{0, 0},
	PM_INCANTIFIER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN,
	ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MA_HUMAN, 0, MA_GNOME|MA_ORC|MA_ELF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  0, 1 },	/* Hit points */
	{  0, 0,  100, 0, 100, 0 },		/* Energy */
	NORMALNIGHTVIS
	//Note: Bonus to all spells.
},
{	"Inheritor", "human", "humanity", "Inh",
	{"man", "woman"},
	PM_INHERITOR, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN,
	ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/*MA_HUMAN disabled*/ 0, 0, MA_GNOME|MA_ORC|MA_ELF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 },		/* Energy */
	NORMALNIGHTVIS,
	SPE_ABJURATION, -20
},
{	"orc", "orcish", "orcdom", "Orc",
	{0, 0},
	PM_ORC, NON_PM, PM_ORC_MUMMY, PM_ORC,
	ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MA_ORC, 0, MA_FEY|MA_HUMAN|MA_ELF|MA_DWARF,
	/*  Str    Int Wis Dex Con Cha */
	{   3,      3,  3,  3,  3,  3 },
	{   18 ,   16, 16, 18, 18, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 3 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 },	/* Energy */
	NIGHTVISION2,
	SPE_CANCELLATION, -15
},
{	"vampire", "vampiric", "vampirehood", "Vam",
	{"vampire", "vampiress"},
	PM_VAMPIRE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN,
	ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC|ROLE_NEUTRAL,
	MA_VAMPIRE, 0, MA_FEY|MA_ELF|MA_GNOME|MA_DWARF|MA_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(19), 18, 18, 20, 20, 20 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  2, 0 },	/* Hit points */
	{  3, 0,  4, 0,  4, 0 },	/* Energy */
	NORMALNIGHTVIS,
	SPE_DRAIN_LIFE, -5
},
{	"yuki-onna", "yuki-onnic", "feyhood", "Swn",
	{0, 0},
	PM_YUKI_ONNA, NON_PM, PM_HUMAN_MUMMY, PM_YUKI_ONNA,
	ROLE_FEMALE | ROLE_LAWFUL|ROLE_CHAOTIC|ROLE_NEUTRAL,
	MA_FEY, 0, MA_ELF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{     16,     18, 18, 20, 14, 20 },
	/* Init   Lower  Higher */
	{  2, 0,  1, 0,  0, 1 },	/* Hit points */
	{  2, 0,  3, 0,  3, 0 },	/* Energy */
	NORMALNIGHTVIS,
	SPE_CHARM_MONSTER, -15
},
/* Array terminator */
{ 0, 0, 0, 0, 0 }};


/* The player's race, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Race urace =
{	"something", "undefined", "something", "Xxx",
	{0, 0},
	NON_PM, NON_PM, NON_PM, NON_PM,
	0, 0, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 },	/* Energy */
	NO_NIGHTVISION
};

struct Race myrkalfr = 
{	"myrkalfr", "myrkalfar", "myrkalfrkind", "Dro",
	{"myrkalfr", "myrkalfr"},
	PM_MYRKALFR, NON_PM, PM_DROW_MUMMY, PM_HEDROW_ZOMBIE,
	ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MA_ELF, 0, MA_ELF|MA_ORC|MA_DROW,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{   18,    20, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  7, 1,  1, 2,  1, 0 },	/* Hit points */
	{  7, 1,  1, 2,  3, 0 },	/* Energy */
	NO_NIGHTVISION
};

struct Race android = 
{	"android", "android", "android-kind", "And",
	{"android", "gynoid"},
	PM_ANDROID, PM_GYNOID, PM_MUMMIFIED_ANDROID, PM_FLAYED_ANDROID,
	ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	MA_CLOCK, 0, MA_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{   20,    18, 16, 22, 22, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  1, 3,  1, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 },	/* Energy */
	NO_NIGHTVISION
};

/* Table of all genders */
const struct Gender genders[] = {
	{"male",	"he",	"him",	"his",	"Mal",	ROLE_MALE},
	{"female",	"she",	"her",	"her",	"Fem",	ROLE_FEMALE},
	{"neuter",	"it",	"it",	"its",	"Ntr",	ROLE_NEUTER}
};


/* Table of all alignments */
const struct Align aligns[] = {
	{"law",		"lawful",	"Law",	ROLE_LAWFUL,	A_LAWFUL},
	{"balance",	"neutral",	"Neu",	ROLE_NEUTRAL,	A_NEUTRAL},
	{"chaos",	"chaotic",	"Cha",	ROLE_CHAOTIC,	A_CHAOTIC},
	{"evil",	"unaligned",	"Una",	0,		A_NONE},
	{"void",	"non-aligned",	"Non",	0,		A_VOID}
};

STATIC_DCL char * FDECL(promptsep, (char *, int));
STATIC_DCL int FDECL(role_gendercount, (int));
STATIC_DCL int FDECL(race_alignmentcount, (int));

/* used by str2XXX() */
static char NEARDATA randomstr[] = "random";


boolean
validrole(rolenum)
	int rolenum;
{
	return (rolenum >= 0 && rolenum < SIZE(roles)-1);
}


int
randrole(ralign)
	int ralign;
{
	if(!ralign) return (rn2(SIZE(roles)-1));
	else{
		int rolenum=0,i,allowed = 0;
		for(i=0;i<SIZE(roles)-1;i++) if(roles[i].allow & ralign) allowed++;
		allowed = rnd(allowed);
		for(i=0;i<SIZE(roles)-1;i++) if(roles[i].allow & ralign) if(!(--allowed)) return i;
	}
	return 0;/* Backup, should never reach */
}

int
str2role(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; roles[i].name.m; i++) {
	    /* Does it match the male name? */
	    if (!strncmpi(str, roles[i].name.m, len))
		return i;
	    /* Or the female name? */
	    if (roles[i].name.f && !strncmpi(str, roles[i].name.f, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, roles[i].filecode))
		return i;
	}

	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}

struct Role *
pm2role(tpm)
	int tpm;
{
	int i;
	for(i = 0; SIZE(roles)-1; i++){
		if(roles[i].malenum==tpm || roles[i].femalenum==tpm) return &roles[i];
	}
	return pm2role(PM_BARBARIAN); //default to barbarian
}


boolean
validrace(rolenum, racenum)
	int rolenum, racenum;
{
	/* Assumes validrole */
	return (racenum >= 0 && racenum < SIZE(races)-1 &&
		(roles[rolenum].marace & races[racenum].selfmask));
}


int
randrace(rolenum)
	int rolenum;
{
	int i, n = 0;

	/* Count the number of valid races */
	for (i = 0; races[i].noun; i++)
	    if (roles[rolenum].marace & races[i].selfmask)
	    	n++;

	/* Pick a random race */
	/* Use a factor of 100 in case of bad random number generators */
	if (n) n = rn2(n*100)/100;
	for (i = 0; races[i].noun; i++)
	    if (roles[rolenum].marace & races[i].selfmask) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role has no permitted races? */
	return (rn2(SIZE(races)-1));
}


int
str2race(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; races[i].noun; i++) {
	    /* Does it match the noun? */
	    if (!strncmpi(str, races[i].noun, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, races[i].filecode))
		return i;
	}

	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validgend(rolenum, racenum, gendnum)
	int rolenum, racenum, gendnum;
{
	/* Assumes validrole and validrace */
	return (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		(roles[rolenum].allow & races[racenum].allow &
		 genders[gendnum].allow & ROLE_GENDMASK));
}


int
randgend(rolenum, racenum)
	int rolenum, racenum;
{
	int i, n = 0;

	/* Count the number of valid genders */
	for (i = 0; i < ROLE_GENDERS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK)
	    	n++;

	/* Pick a random gender */
	if (n) n = rn2(n);
	for (i = 0; i < ROLE_GENDERS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role/race has no permitted genders? */
	return (rn2(ROLE_GENDERS));
}


int
str2gend(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; i < ROLE_GENDERS; i++) {
	    /* Does it match the adjective? */
	    if (!strncmpi(str, genders[i].adj, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, genders[i].filecode))
		return i;
	}
	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}

/* I TRIED to add forced alignments in a non-hacky manner, but SOMETHING SOMEWHERE nullpointers if the
	Role struct changes lengths, gdb was no help, and I'm tired of screwing with it.  */
boolean
validalign(rolenum, racenum, alignnum)
	int rolenum, racenum, alignnum;
{
	/* Assumes validrole and validrace */
	return (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		(((roles[rolenum].allow & races[racenum].allow &
		 aligns[alignnum].allow & ROLE_ALIGNMASK))
#ifdef CONVICT
	|| (roles[rolenum].malenum==PM_CONVICT && aligns[alignnum].allow & roles[rolenum].allow & ROLE_ALIGNMASK && roles[rolenum].marace & races[racenum].selfmask)
#endif
		));
}


int
randalign(rolenum, racenum)
	int rolenum, racenum;
{
	int i, n = 0;

	/* Count the number of valid alignments */
	for (i = 0; i < ROLE_ALIGNS; i++)
	    if ((roles[rolenum].allow & races[racenum].allow &
	    		aligns[i].allow & ROLE_ALIGNMASK)
#ifdef CONVICT
		|| (roles[rolenum].malenum==PM_CONVICT && aligns[i].allow & roles[rolenum].allow & ROLE_ALIGNMASK && roles[rolenum].marace & races[racenum].selfmask)
#endif
			)
	    	n++;

	/* Pick a random alignment */
	if (n) n = rn2(n);
	for (i = 0; i < ROLE_ALIGNS; i++)
	    if ((roles[rolenum].allow & races[racenum].allow &
	    		aligns[i].allow & ROLE_ALIGNMASK)
#ifdef CONVICT
		|| (roles[rolenum].malenum==PM_CONVICT && aligns[i].allow & roles[rolenum].allow & ROLE_ALIGNMASK && roles[rolenum].marace & races[racenum].selfmask)
#endif
			) {
	    	if (n) n--;
	    	else return (i);
	    }
	/* This role/race has no permitted alignments? */
	return (rn2(ROLE_ALIGNS));
}


int
str2align(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; i < ROLE_ALIGNS; i++) {
	    /* Does it match the adjective? */
	    if (!strncmpi(str, aligns[i].adj, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, aligns[i].filecode))
		return i;
	}
	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}

/* is rolenum compatible with any racenum/gendnum/alignnum constraints? */
boolean
ok_role(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (rolenum >= 0 && rolenum < SIZE(roles)-1) {
	allow = roles[rolenum].allow;
	if (racenum >= 0 && racenum < SIZE(races)-1 &&
		!(roles[rolenum].marace & races[racenum].selfmask))
	    return FALSE;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK
#ifdef CONVICT
		|| (roles[rolenum].malenum==PM_CONVICT && aligns[alignnum].allow & allow & ROLE_ALIGNMASK && roles[rolenum].marace & races[racenum].selfmask)
#endif
	))
		return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(roles)-1; i++) {
	    allow = roles[i].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1 &&
		    !(roles[i].marace & races[racenum].selfmask))
			continue;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
			continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK
#ifdef CONVICT
			|| (roles[rolenum].malenum==PM_CONVICT && aligns[alignnum].allow & allow & ROLE_ALIGNMASK && roles[i].marace & races[racenum].selfmask)
#endif
		))
			continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random role subject to any racenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a role is returned only if there is  */
/* a single possibility */
int
pick_role(racenum, gendnum, alignnum, pickhow)
int racenum, gendnum, alignnum, pickhow;
{
    int i;
    int roles_ok = 0;

    for (i = 0; i < SIZE(roles)-1; i++) {
	if (ok_role(i, racenum, gendnum, alignnum))
	    roles_ok++;
    }
    if (roles_ok == 0 || (roles_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    roles_ok = rn2(roles_ok);
    for (i = 0; i < SIZE(roles)-1; i++) {
	if (ok_role(i, racenum, gendnum, alignnum)) {
	    if (roles_ok == 0)
		return i;
	    else
		roles_ok--;
	}
    }
    return ROLE_NONE;
}

/* is racenum compatible with any rolenum/gendnum/alignnum constraints? */
boolean
ok_race(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (racenum >= 0 && racenum < SIZE(races)-1) {
	allow = races[racenum].allow;
	if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		!(races[racenum].selfmask & roles[rolenum].marace))
	    return FALSE;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK
#ifdef CONVICT
		|| (roles[rolenum].malenum==PM_CONVICT && aligns[alignnum].allow & roles[rolenum].allow & ROLE_ALIGNMASK && roles[rolenum].marace & races[racenum].selfmask)
#endif
	))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(races)-1; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		    !(races[i].selfmask & roles[rolenum].marace))
		continue;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
		continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK
#ifdef CONVICT
			|| (roles[rolenum].malenum==PM_CONVICT && aligns[alignnum].allow & roles[rolenum].allow & ROLE_ALIGNMASK && roles[rolenum].marace & races[i].selfmask)
#endif
	))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random race subject to any rolenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a race is returned only if there is  */
/* a single possibility */
int
pick_race(rolenum, gendnum, alignnum, pickhow)
int rolenum, gendnum, alignnum, pickhow;
{
    int i;
    int races_ok = 0;

    for (i = 0; i < SIZE(races)-1; i++) {
	if (ok_race(rolenum, i, gendnum, alignnum))
	    races_ok++;
    }
    if (races_ok == 0 || (races_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    races_ok = rn2(races_ok);
    for (i = 0; i < SIZE(races)-1; i++) {
	if (ok_race(rolenum, i, gendnum, alignnum)) {
	    if (races_ok == 0)
		return i;
	    else
		races_ok--;
	}
    }
    return ROLE_NONE;
}

/* is gendnum compatible with any rolenum/racenum/alignnum constraints? */
/* gender and alignment are not comparable (and also not constrainable) */
boolean
ok_gend(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (gendnum >= 0 && gendnum < ROLE_GENDERS) {
	allow = genders[gendnum].allow;
	if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		!(allow & roles[rolenum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (racenum >= 0 && racenum < SIZE(races)-1 &&
		!(allow & races[racenum].allow & ROLE_GENDMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_GENDERS; i++) {
	    allow = genders[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		    !(allow & roles[rolenum].allow & ROLE_GENDMASK))
		continue;
	    if (racenum >= 0 && racenum < SIZE(races)-1 &&
		    !(allow & races[racenum].allow & ROLE_GENDMASK))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random gender subject to any rolenum/racenum/alignnum constraints */
/* gender and alignment are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID a gender is returned only if there is  */
/* a single possibility */
int
pick_gend(rolenum, racenum, alignnum, pickhow)
int rolenum, racenum, alignnum, pickhow;
{
    int i;
    int gends_ok = 0;

    for (i = 0; i < ROLE_GENDERS; i++) {
	if (ok_gend(rolenum, racenum, i, alignnum))
	    gends_ok++;
    }
    if (gends_ok == 0 || (gends_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    gends_ok = rn2(gends_ok);
    for (i = 0; i < ROLE_GENDERS; i++) {
	if (ok_gend(rolenum, racenum, i, alignnum)) {
	    if (gends_ok == 0)
		return i;
	    else
		gends_ok--;
	}
    }
    return ROLE_NONE;
}

/* is alignnum compatible with any rolenum/racenum/gendnum constraints? */
/* alignment and gender are not comparable (and also not constrainable) */
boolean
ok_align(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    short allow;

    if (alignnum >= 0 && alignnum < ROLE_ALIGNS) {
	allow = aligns[alignnum].allow;
	if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		!(allow & roles[rolenum].allow & ROLE_ALIGNMASK))
	    return FALSE;
	if (racenum >= 0 && racenum < SIZE(races)-1 &&
		!(allow & races[racenum].allow & ROLE_ALIGNMASK
#ifdef CONVICT
		|| (roles[rolenum].malenum==PM_CONVICT && allow & roles[rolenum].allow & ROLE_ALIGNMASK && roles[rolenum].marace & races[racenum].selfmask)
#endif
	))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_ALIGNS; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1 &&
		    !(allow & roles[rolenum].allow & ROLE_ALIGNMASK))
		continue;
	    if (racenum >= 0 && racenum < SIZE(races)-1 &&
		    !(allow & races[racenum].allow & ROLE_ALIGNMASK
#ifdef CONVICT
			|| (roles[rolenum].malenum==PM_CONVICT && allow & roles[rolenum].allow & ROLE_ALIGNMASK && roles[rolenum].marace & races[racenum].selfmask)
#endif
		))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random alignment subject to any rolenum/racenum/gendnum constraints */
/* alignment and gender are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID an alignment is returned only if there is  */
/* a single possibility */
int
pick_align(rolenum, racenum, gendnum, pickhow)
int rolenum, racenum, gendnum, pickhow;
{
    int i;
    int aligns_ok = 0;

    for (i = 0; i < ROLE_ALIGNS; i++) {
	if (ok_align(rolenum, racenum, gendnum, i))
	    aligns_ok++;
    }
    if (aligns_ok == 0 || (aligns_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    aligns_ok = rn2(aligns_ok);
    for (i = 0; i < ROLE_ALIGNS; i++) {
	if (ok_align(rolenum, racenum, gendnum, i)) {
	    if (aligns_ok == 0)
		return i;
	    else
		aligns_ok--;
	}
    }
    return ROLE_NONE;
}

void
rigid_role_checks()
{
    /* Some roles are limited to a single race, alignment, or gender and
     * calling this routine prior to XXX_player_selection() will help
     * prevent an extraneous prompt that actually doesn't allow
     * you to choose anything further. Note the use of PICK_RIGID which
     * causes the pick_XX() routine to return a value only if there is one
     * single possible selection, otherwise it returns ROLE_NONE.
     *
     */
    if (flags.initrole == ROLE_RANDOM) {
	/* If the role was explicitly specified as ROLE_RANDOM
	 * via -uXXXX-@ then choose the role in here to narrow down
	 * later choices. Pick a random role in this case.
	 */
	flags.initrole = pick_role(flags.initrace, flags.initgend,
					flags.initalign, PICK_RANDOM);
	if (flags.initrole < 0)
	    flags.initrole = randrole(0);
    }
    if (flags.initrole != ROLE_NONE) {
	if (flags.initrace == ROLE_NONE)
	     flags.initrace = pick_race(flags.initrole, flags.initgend,
						flags.initalign, PICK_RIGID);
	if (flags.initalign == ROLE_NONE)
	     flags.initalign = pick_align(flags.initrole, flags.initrace,
						flags.initgend, PICK_RIGID);
	if (flags.initgend == ROLE_NONE)
	     flags.initgend = pick_gend(flags.initrole, flags.initrace,
						flags.initalign, PICK_RIGID);
    }
}

#define BP_ALIGN	0
#define BP_GEND		1
#define BP_RACE		2
#define BP_ROLE		3
#define NUM_BP		4

STATIC_VAR char pa[NUM_BP], post_attribs;

STATIC_OVL char *
promptsep(buf, num_post_attribs)
char *buf;
int num_post_attribs;
{
	const char *conj = "and ";
	if (num_post_attribs > 1
	    && post_attribs < num_post_attribs && post_attribs > 1)
	 	Strcat(buf, ","); 
	Strcat(buf, " ");
	--post_attribs;
	if (!post_attribs && num_post_attribs > 1) Strcat(buf, conj);
	return buf;
}

STATIC_OVL int
role_gendercount(rolenum)
int rolenum;
{
	int gendcount = 0;
	if (validrole(rolenum)) {
		if (roles[rolenum].allow & ROLE_MALE) ++gendcount;
		if (roles[rolenum].allow & ROLE_FEMALE) ++gendcount;
		if (roles[rolenum].allow & ROLE_NEUTER) ++gendcount;
	}
	return gendcount;
}

STATIC_OVL int
race_alignmentcount(racenum)
int racenum;
{
	int aligncount = 0;
	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
		if (races[racenum].allow & ROLE_CHAOTIC) ++aligncount;
		if (races[racenum].allow & ROLE_LAWFUL) ++aligncount;
		if (races[racenum].allow & ROLE_NEUTRAL) ++aligncount;
	}
	return aligncount;
}

char *
root_plselection_prompt(suppliedbuf, buflen, rolenum, racenum, gendnum, alignnum)
char *suppliedbuf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
	int k, gendercount = 0, aligncount = 0;
	char buf[BUFSZ];
	static char err_ret[] = " character's";
	boolean donefirst = FALSE;

	if (!suppliedbuf || buflen < 1) return err_ret;

	/* initialize these static variables each time this is called */
	post_attribs = 0;
	for (k=0; k < NUM_BP; ++k)
		pa[k] = 0;
	buf[0] = '\0';
	*suppliedbuf = '\0';
	
	/* How many alignments are allowed for the desired race? */
	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM)
		aligncount = race_alignmentcount(racenum);

	if (alignnum != ROLE_NONE && alignnum != ROLE_RANDOM) {
		/* if race specified, and multiple choice of alignments for it */
		if ((racenum >= 0) && (aligncount > 1)) {
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, aligns[alignnum].adj);
			donefirst = TRUE;
		} else {
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, aligns[alignnum].adj);
			donefirst = TRUE;
		}
	} else {
		/* if alignment not specified, but race is specified
			and only one choice of alignment for that race then
			don't include it in the later list */
		if ((((racenum != ROLE_NONE && racenum != ROLE_RANDOM) &&
			ok_race(rolenum, racenum, gendnum, alignnum))
		      && (aligncount > 1))
		     || (racenum == ROLE_NONE || racenum == ROLE_RANDOM)) {
			pa[BP_ALIGN] = 1;
			post_attribs++;
		}
	}
	/* <your lawful> */

	/* How many genders are allowed for the desired role? */
	if (validrole(rolenum))
		gendercount = role_gendercount(rolenum);

	if (gendnum != ROLE_NONE  && gendnum != ROLE_RANDOM) {
		if (validrole(rolenum)) {
		     /* if role specified, and multiple choice of genders for it,
			and name of role itself does not distinguish gender */
			if ((rolenum != ROLE_NONE) && (gendercount > 1)
						&& !roles[rolenum].name.f) {
				if (donefirst) Strcat(buf, " ");
				Strcat(buf, genders[gendnum].adj);
				donefirst = TRUE;
			}
	        } else {
			if (donefirst) Strcat(buf, " ");
	        	Strcat(buf, genders[gendnum].adj);
			donefirst = TRUE;
	        }
	} else {
		/* if gender not specified, but role is specified
			and only one choice of gender then
			don't include it in the later list */
		if ((validrole(rolenum) && (gendercount > 1)) || !validrole(rolenum)) {
			pa[BP_GEND] = 1;
			post_attribs++;
		}
	}
	/* <your lawful female> */

	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
		if (validrole(rolenum) && ok_race(rolenum, racenum, gendnum, alignnum)) {
			if (donefirst) Strcat(buf, " "); 
			Strcat(buf, (rolenum == ROLE_NONE) ?
				races[racenum].noun :
				races[racenum].adj);
			donefirst = TRUE;
		} else if (!validrole(rolenum)) {
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, races[racenum].noun);
			donefirst = TRUE;
		} else {
			pa[BP_RACE] = 1;
			post_attribs++;
		}
	} else {
		pa[BP_RACE] = 1;
		post_attribs++;
	}
	/* <your lawful female gnomish> || <your lawful female gnome> */

	if (validrole(rolenum)) {
		if (donefirst) Strcat(buf, " ");
		if (gendnum != ROLE_NONE) {
		    if (gendnum == 1  && roles[rolenum].name.f)
			Strcat(buf, roles[rolenum].name.f);
		    else
  			Strcat(buf, roles[rolenum].name.m);
		} else {
			if (roles[rolenum].name.f) {
				Strcat(buf, roles[rolenum].name.m);
				Strcat(buf, "/");
				Strcat(buf, roles[rolenum].name.f);
			} else 
				Strcat(buf, roles[rolenum].name.m);
		}
		donefirst = TRUE;
	} else if (rolenum == ROLE_NONE) {
		pa[BP_ROLE] = 1;
		post_attribs++;
	}
	
	if ((racenum == ROLE_NONE || racenum == ROLE_RANDOM) && !validrole(rolenum)) {
		if (donefirst) Strcat(buf, " ");
		Strcat(buf, "character");
		donefirst = TRUE;
	}
	/* <your lawful female gnomish cavewoman> || <your lawful female gnome>
	 *    || <your lawful female character>
	 */
	if (buflen > (int) (strlen(buf) + 1)) {
		Strcpy(suppliedbuf, buf);
		return suppliedbuf;
	} else
		return err_ret;
}

char *
build_plselection_prompt(buf, buflen, rolenum, racenum, gendnum, alignnum)
char *buf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
	const char *defprompt = "Shall I pick a character for you? [ynq] ";
	int num_post_attribs = 0;
	char tmpbuf[BUFSZ];
	
	if (buflen < QBUFSZ)
		return (char *)defprompt;

	Strcpy(tmpbuf, "Shall I pick ");
	if (racenum != ROLE_NONE || validrole(rolenum))
		Strcat(tmpbuf, "your ");
	else {
		Strcat(tmpbuf, "a ");
	}
	/* <your> */

	(void)  root_plselection_prompt(eos(tmpbuf), buflen - strlen(tmpbuf),
					rolenum, racenum, gendnum, alignnum);
	Sprintf(buf, "%s", s_suffix(tmpbuf));

	/* buf should now be:
	 * < your lawful female gnomish cavewoman's> || <your lawful female gnome's>
	 *    || <your lawful female character's>
	 *
         * Now append the post attributes to it
	 */

	num_post_attribs = post_attribs;
	if (post_attribs) {
		if (pa[BP_RACE]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, "race");
		}
		if (pa[BP_ROLE]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, "role");
		}
		if (pa[BP_GEND]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, "gender");
		}
		if (pa[BP_ALIGN]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, "alignment");
		}
	}
	Strcat(buf, " for you? [ynq] ");
	return buf;
}

#undef BP_ALIGN
#undef BP_GEND
#undef BP_RACE
#undef BP_ROLE
#undef NUM_BP

void
plnamesuffix()
{
	char *sptr, *eptr;
	int i;

	/* Look for tokens delimited by '-' */
	if ((eptr = index(plname, '-')) != (char *) 0)
	    *eptr++ = '\0';
	while (eptr) {
	    /* Isolate the next token */
	    sptr = eptr;
	    if ((eptr = index(sptr, '-')) != (char *)0)
		*eptr++ = '\0';

	    /* Try to match it to something */
	    if ((i = str2role(sptr)) != ROLE_NONE)
		flags.initrole = i;
	    else if ((i = str2race(sptr)) != ROLE_NONE)
		flags.initrace = i;
	    else if ((i = str2gend(sptr)) != ROLE_NONE)
		flags.initgend = i;
	    else if ((i = str2align(sptr)) != ROLE_NONE)
		flags.initalign = i;
	}
	if(!plname[0]) {
	    askname();
	    plnamesuffix();
	}

	/* commas in the plname confuse the record file, convert to spaces */
	for (sptr = plname; *sptr; sptr++) {
		if (*sptr == ',') *sptr = ' ';
	}
}


/*
 *	Special setup modifications here:
 *
 *	Unfortunately, this is going to have to be done
 *	on each newgame or restore, because you lose the permonst mods
 *	across a save/restore.  :-)
 *
 *	1 - The Rogue Leader is the Tourist Nemesis.
 *	2 - Priests start with a random alignment - convert the leader and
 *	    guardians here.
 *	3 - Elves can have one of two different leaders, but can't work it
 *	    out here because it requires hacking the level file data (see
 *	    sp_lev.c).
 *
 * This code also replaces quest_init().
 */
void
role_init(newgame)
int newgame;
{
	int alignmnt;

	/* Strip the role letter out of the player name.
	 * This is included for backwards compatibility.
	 */
	plnamesuffix();

	/* Check for a valid role.  Try flags.initrole first. */
	if (!validrole(flags.initrole) && newgame) {
	    /* Try the player letter second */
	    if ((flags.initrole = str2role(pl_character)) < 0)
	    	/* None specified; pick a random role */
	    	flags.initrole = randrole(0);
	}

	/* We now have a valid role index.  Copy the role name back. */
	/* This should become OBSOLETE */
	Strcpy(pl_character, roles[flags.initrole].name.m);
	pl_character[PL_CSIZ-1] = '\0';

	/* Check for a valid race */
	if (!validrace(flags.initrole, flags.initrace) && newgame)
	    flags.initrace = randrace(flags.initrole);

	/* Check for a valid gender.  If new game, check both initgend
	 * and female.  On restore, assume flags.female is correct. */
	if (flags.pantheon == -1 && newgame) {	/* new game */
	    if (!validgend(flags.initrole, flags.initrace, flags.female))
		flags.female = !flags.female;
	}
	if (!validgend(flags.initrole, flags.initrace, flags.initgend) && newgame)
	    /* Note that there is no way to check for an unspecified gender. */
	    flags.initgend = flags.female;

	/* Check for a valid alignment */
	if (!validalign(flags.initrole, flags.initrace, flags.initalign) && newgame)
	    /* Pick a random alignment */
	    flags.initalign = randalign(flags.initrole, flags.initrace);
	alignmnt = aligns[flags.initalign].value;

	/* Initialize urole and urace */
	urole = roles[flags.initrole];
	urace = races[flags.initrace];
	if(Role_if(PM_ANACHRONONAUT)){
		if(Race_if(PM_DROW))
			urace = myrkalfr;
		if(Race_if(PM_CLOCKWORK_AUTOMATON)){
			urole.filecode = "And";
			urace = android;
			if(newgame){
				quest_status.got_quest = TRUE;
				quest_status.leader_is_dead = TRUE;
				flags.questprogress = 1;
			}
		}
	}
	else if(Humanoid_half_dragon(urole.malenum)){
		urace.attrmax[A_DEX] = 18;
	}

	/* Fix up the god names */
	if (flags.pantheon == -1) {		/* new game */
	    flags.pantheon = flags.initrole;	/* use own gods */
	    while (!roles[flags.pantheon].lgod 	/* unless they're missing */
			&& !(Race_if(PM_DROW) && !Role_if(PM_ANACHRONONAUT))
			&& !(Race_if(PM_ELF) && !Role_if(PM_ANACHRONONAUT))
			&& !Role_if(PM_EXILE)
		) flags.pantheon = randrole(0);
	    flags.panLgod = flags.pantheon;
	    flags.panNgod = flags.pantheon;
	    flags.panCgod = flags.pantheon;
		if(Role_if(PM_EXILE)){
			do{
				do flags.panLgod = randrole(AM_LAWFUL); while (!roles[flags.panLgod].lgod);
				do flags.panNgod = randrole(AM_NEUTRAL); while (!roles[flags.panNgod].ngod);
				do flags.panCgod = randrole(AM_CHAOTIC); while (!roles[flags.panCgod].cgod);
			} while(flags.panLgod == flags.panNgod ||
					flags.panLgod == flags.panCgod ||
					flags.panNgod == flags.panCgod );
		}
	}
	if((Pantheon_if(PM_NOBLEMAN) || Role_if(PM_NOBLEMAN)) && Race_if(PM_VAMPIRE))
		urole.questarti = ART_VESTMENT_OF_HELL;
	
	if(Role_if(PM_EXILE)){
		urole.lgod = roles[flags.panLgod].lgod;
		urole.ngod = roles[flags.panNgod].ngod;
		urole.cgod = roles[flags.panCgod].cgod;
		if(roles[flags.panLgod].guardnum){
			mons[roles[flags.panLgod].guardnum].mflagst &= ~(MT_CLOSE|MT_PEACEFUL);
			mons[roles[flags.panLgod].guardnum].mflagst |= MT_HOSTILE;
			mons[roles[flags.panLgod].guardnum].msound = MS_CUSS;
		}
		if(roles[flags.panNgod].guardnum){
			mons[roles[flags.panNgod].guardnum].mflagst &= ~(MT_CLOSE|MT_PEACEFUL);
			mons[roles[flags.panNgod].guardnum].mflagst |= MT_HOSTILE;
			mons[roles[flags.panNgod].guardnum].msound = MS_CUSS;
		}
		if(roles[flags.panCgod].guardnum){
			mons[roles[flags.panCgod].guardnum].mflagst &= ~(MT_CLOSE|MT_PEACEFUL);
			mons[roles[flags.panCgod].guardnum].mflagst |= MT_HOSTILE;
			mons[roles[flags.panCgod].guardnum].msound = MS_CUSS;
		}
	} else if(Race_if(PM_DROW) && (Role_if(PM_PRIEST) || Role_if(PM_ROGUE) || Role_if(PM_RANGER) || Role_if(PM_WIZARD) || Role_if(PM_NOBLEMAN))){
		flags.racial_pantheon = PM_DROW;
		urole.rank[0] = DrowRanks[0];
		urole.rank[1] = DrowRanks[1];
		urole.rank[2] = DrowRanks[2];
		urole.rank[3] = DrowRanks[3];
		urole.rank[4] = DrowRanks[4];
		urole.rank[5] = DrowRanks[5];
		urole.rank[6] = DrowRanks[6];
		urole.rank[7] = DrowRanks[7];
		urole.rank[8] = DrowRanks[8];
		if(flags.initgend){
			/*true = female*/
			if(Role_if(PM_NOBLEMAN)){
				urole.filecode = "Ndr";
				
				urole.lgod = GOD_VER_TAS;
				urole.ngod = GOD_KIARANSALI;
				urole.cgod = GOD_LOLTH;
				
				urole.homebase = "your house";
				urole.intermed = "DRA-MUR-SHOU";
				urole.questarti = ART_WEB_OF_THE_CHOSEN;
				
				urole.ldrnum = PM_MOTHER;
				urole.guardnum = PM_SISTER;
				urole.neminum = PM_ELDER_BRAIN;
				
				urole.enemy1num = PM_GRIMLOCK;
				urole.enemy2num = PM_UMBER_HULK;
				urole.enemy1sym = S_WORM;
				urole.enemy2sym = S_UMBER;
			} else {
				urole.filecode = "Dro";
				urole.lgod = GOD_EILISTRAEE;
				urole.ngod = GOD_KIARANSALI;
				urole.cgod = GOD_LOLTH;
				if(Role_if(PM_PRIEST)){
					urole.spelspec = DrowPriestessFavoredSpell;
					urole.spelsbon = DrowPriestessFavoredBonus;
				}
				if(flags.stag == 0){
					
					urole.homebase = "Erelhei-Cinlu";
					urole.intermed = "the drider caves";
					urole.questarti = ART_SILVER_STARLIGHT;
					
					urole.ldrnum = PM_ECLAVDRA;
					urole.guardnum = PM_DROW_MATRON_MOTHER;
					urole.neminum = PM_SEYLL_AUZKOVYN;
					
					urole.enemy1num = PM_GREY_ELF;
					urole.enemy2num = PM_ELF_LORD;
					urole.enemy1sym = S_QUADRUPED;
					urole.enemy2sym = S_LAW_ANGEL;
				} else {					
					urole.homebase = "the Grove of Eilistraee";
					urole.intermed = "Erelhei-Cinlu";
					urole.questarti = ART_TENTACLE_ROD;
					
					urole.ldrnum = PM_SEYLL_AUZKOVYN;
					urole.guardnum = PM_STJARNA_ALFR;
					urole.neminum = PM_ECLAVDRA;
					
					urole.enemy1num = PM_HEDROW_WARRIOR;
					urole.enemy2num = PM_DROW_MATRON;
					urole.enemy1sym = S_SPIDER;
					urole.enemy2sym = S_DEMON;
				}
			}
		} else {
			if(Role_if(PM_NOBLEMAN)){
				urole.filecode = "Nhd";
				
				urole.lgod = GOD_VER_TAS;
				urole.ngod = GOD_KEPTOLO;
				urole.cgod = GOD_GHAUNADAUR;
				
				urole.homebase = "your house";
				urole.intermed = "Eryndlyn";
				urole.questarti = ART_CLOAK_OF_THE_CONSORT;
				
				urole.ldrnum = PM_DROW_NOVICE;
				urole.guardnum = PM_HOUSELESS_DROW;
				urole.neminum = PM_A_SALOM;
				
				urole.enemy1num = PM_HEDROW_ZOMBIE;
				urole.enemy2num = PM_DROW_MUMMY;
				urole.enemy1sym = S_FUNGUS;
				urole.enemy2sym = S_DEMON;
			} else {
				urole.filecode = "Hdr";
				urole.lgod = GOD_EDDERGUD;
				urole.ngod = GOD_VHAERAUN;
				urole.cgod = GOD_LOLTH;
				
				if(flags.stag == 0){
					urole.homebase = "Menzoberranzan";
					urole.intermed = "the drider caves";
					urole.questarti = ART_DARKWEAVER_S_CLOAK;
					
					urole.ldrnum = PM_ECLAVDRA;
					urole.guardnum = PM_DROW_MATRON_MOTHER;
					urole.neminum = PM_DARUTH_XAXOX;
					
					urole.enemy1num = PM_EDDERKOP;
					urole.enemy2num = PM_HEDROW_WIZARD;
					urole.enemy1sym = S_MUMMY;
					urole.enemy2sym = S_UMBER;
				} else {
					urole.homebase = "Tower Xaxox";
					urole.intermed = "Menzoberranzan";
					urole.questarti = ART_TENTACLE_ROD;
					
					urole.ldrnum = PM_DARUTH_XAXOX;
					urole.guardnum = PM_DROW_ALIENIST;
					urole.neminum = PM_ECLAVDRA;
					
					urole.enemy1num = PM_HEDROW_ZOMBIE;
					urole.enemy2num = PM_DROW_MATRON;
					urole.enemy1sym = S_DEMON;
					urole.enemy2sym = S_ZOMBIE;
				}
			}
		}
	} else if((Race_if(PM_ELF) || Pantheon_if(PM_ELF)) && (Role_if(PM_RANGER) || Role_if(PM_WIZARD) || Role_if(PM_NOBLEMAN))){
		flags.racial_pantheon = PM_ELF;
		urole.filecode = "Elf";
		
		if(Role_if(PM_RANGER)){
			urole.spelspec = ElfRangerFavoredSpell;
			urole.spelsbon = ElfRangerFavoredBonus;
		}
		
		urole.rank[0] = ElfRangerRanks[0];
		urole.rank[1] = ElfRangerRanks[1];
		urole.rank[2] = ElfRangerRanks[2];
		urole.rank[3] = ElfRangerRanks[3];
		urole.rank[4] = ElfRangerRanks[4];
		urole.rank[5] = ElfRangerRanks[5];
		urole.rank[6] = ElfRangerRanks[6];
		urole.rank[7] = ElfRangerRanks[7];
		urole.rank[8] = ElfRangerRanks[8];
		
		if(Role_if(PM_RANGER)){
			urole.lgod = GOD_OROME;
			urole.ngod = GOD_YAVANNA;
			urole.cgod = GOD_TULKAS;
		} else {
			if(flags.initgend){ /*true = female*/
				urole.lgod = GOD_VARDA_ELENTARI;
				urole.ngod = GOD_MANDOS;
				urole.cgod = GOD_NESSA;
			} else {
				urole.lgod = GOD_MANWE_SULIMO;
				urole.ngod = GOD_YAVANNA;
				urole.cgod = GOD_LORIEN;
			}
		}
		
		urole.homebase = "Caras Galadhon";
		urole.intermed = "the Mirkwood caves";
		urole.questarti = ART_PALANTIR_OF_WESTERNESSE;
		
		urole.ldrnum = PM_GALADRIEL;
		urole.guardnum = PM_HIGH_ELF;
		urole.neminum = PM_NECROMANCER;
		
		urole.enemy1num = PM_MORDOR_ORC;
		urole.enemy2num = PM_ORC_SHAMAN;
		urole.enemy1sym = S_SPIDER;
		urole.enemy2sym = S_TROLL;
	}  else if(Race_if(PM_ELF) && Role_if(PM_PRIEST)){
		flags.racial_pantheon = PM_ELF;
		urole.filecode = "Elf";
		
		urole.homebase = "Caras Galadhon";
		urole.intermed = "the Mirkwood caves";
		urole.questarti = ART_PALANTIR_OF_WESTERNESSE;
		
		urole.ldrnum = PM_GALADRIEL;
		urole.guardnum = PM_HIGH_ELF;
		urole.neminum = PM_NECROMANCER;
		
		if(flags.initgend){ /*true = female*/
			urole.lgod = GOD_VARDA_ELENTARI;
			urole.ngod = GOD_MANDOS;
			urole.cgod = GOD_NESSA;
		} else {
			urole.lgod = GOD_MANWE_SULIMO;
			urole.ngod = GOD_YAVANNA;
			urole.cgod = GOD_LORIEN;
		}
		urole.enemy1num = PM_ELF;
		urole.enemy2num = PM_ORC_SHAMAN;
		urole.enemy1sym = S_SPIDER;
		urole.enemy2sym = S_TROLL;
	} else if((Race_if(PM_DWARF) || Pantheon_if(PM_DWARF)) && (Role_if(PM_NOBLEMAN) || Role_if(PM_KNIGHT))){
		flags.racial_pantheon = PM_DWARF;
		// if(flags.questvar == 0){
			// // pline("picking dwarf quest");
			// flags.questvar = rnd(2);
		// }
		// if(flags.questvar == 1){
		if(Role_if(PM_KNIGHT)){
			urole.filecode = "Dna";
			
			urole.homebase = "Lake Town";
			urole.intermed = "Erebor";
			urole.questarti = ART_ARMOR_OF_EREBOR;
			
			urole.ldrnum = PM_THORIN_II_OAKENSHIELD;
			urole.guardnum = PM_DWARF_WARRIOR;
			urole.neminum = PM_BOLG;
			
			urole.petnum = NON_PM;
		} else {
			urole.filecode = "Dnb";
			
			urole.homebase = "Erebor";
			urole.intermed = "Khazad-dum";
			urole.questarti = ART_WAR_MASK_OF_DURIN;
			
			urole.ldrnum = PM_DAIN_II_IRONFOOT;
			urole.guardnum = PM_DWARF_WARRIOR;
			urole.neminum = PM_DURIN_S_BANE;
			
			urole.petnum = NON_PM;
		}
		urole.rank[0] = DwarfRanks[0];
		urole.rank[1] = DwarfRanks[1];
		urole.rank[2] = DwarfRanks[2];
		urole.rank[3] = DwarfRanks[3];
		urole.rank[4] = DwarfRanks[4];
		urole.rank[5] = DwarfRanks[5];
		urole.rank[6] = DwarfRanks[6];
		urole.rank[7] = DwarfRanks[7];
		urole.rank[8] = DwarfRanks[8];
		
		urole.lgod = GOD_MAHAL;
		urole.ngod = GOD_HOLASHNER;
		urole.cgod = GOD_ARMOK;
		
		urole.enemy1num = PM_HILL_ORC;
		urole.enemy2num = PM_ORC_SHAMAN;
		urole.enemy1sym = S_TROLL;
		urole.enemy2sym = S_ORC;
	} else if ((Race_if(PM_GNOME) || Pantheon_if(PM_GNOME)) && (Role_if(PM_RANGER)) ) {
		flags.racial_pantheon = PM_GNOME;
		urole.filecode = "Gnr";
		
		urole.homebase = "the Golden Halls";
		urole.intermed = "Temple of Kurtulmak";
		urole.questarti = ART_ROGUE_GEAR_SPIRITS;
		
		urole.ldrnum = PM_DAMAGED_ARCADIAN_AVENGER;
		urole.guardnum = PM_GNOME_COMRADE;
		urole.neminum = PM_GREAT_HIGH_SHAMAN_OF_KURTULMAK;
		
		urole.lgod = GOD_KURTULMAK;
		urole.ngod = GOD_GARL_GLITTERGOLD;
		urole.cgod = GOD_URDLEN;
		
		urole.enemy1num = PM_KOBOLD;
		urole.enemy2num = PM_LARGE_KOBOLD;
		urole.enemy1sym = S_IMP;
		urole.enemy2sym = S_KOBOLD;
	// } else if ((Race_if(PM_ORC) || Pantheon_if(PM_ORC)) && (Role_if(PM_RANGER)) ) {
		// flags.racial_pantheon = PM_ORC;
		// urole.filecode = "Ror";
		
		// urole.homebase = "";
		// urole.intermed = "";
		// urole.questarti = 0;/*Determined by first loot you obtain*/
		
		// urole.ldrnum = PM_ORC_WARCHIEF;
		// urole.guardnum = PM_ORC_WARRIOR;
		// urole.neminum = 0;/*Determined by first loot you obtain*/
		
		// urole.lgod = OrcLgod;
		// urole.ngod = OrcNgod;
		// urole.cgod = OrcCgod;
		
		// urole.enemy1num = PM_WOLF;
		// urole.enemy2num = PM_MASTODON;
		// urole.enemy1sym = S_QUADRUPED;
		// urole.enemy2sym = S_LAW_ANGEL;
	} else if (Race_if(PM_HALF_DRAGON) && Role_if(PM_NOBLEMAN) && flags.initgend) {
		if(flags.initgend){
			flags.racial_pantheon = PM_HALF_DRAGON;
			urole.filecode = "Hdf";
			urole.petnum = PM_UNDEAD_KNIGHT;
			
			urole.homebase = "the forlorn settlement";
			urole.intermed = "the Cathedral of Velka";
			urole.questarti = ART_FRIEDE_S_SCYTHE;
			
			urole.ldrnum = NON_PM;//PM_CROW_WINGED_HALF_DRAGON;
			urole.guardnum = PM_CORVIAN;
			urole.neminum = PM_BASTARD_OF_THE_BOREAL_VALLEY;
			
			urole.lgod = GOD_GWYN__LORD_OF_SUNLIGHT;
			urole.ngod = GOD_GWYNEVERE__PRINCESS_OF_SUN;
			urole.cgod = GOD_DARK_SUN_GWYNDOLIN;
			
			urole.rank[0] = FemHDNobRanks[0];
			urole.rank[1] = FemHDNobRanks[1];
			urole.rank[2] = FemHDNobRanks[2];
			urole.rank[3] = FemHDNobRanks[3];
			urole.rank[4] = FemHDNobRanks[4];
			urole.rank[5] = FemHDNobRanks[5];
			urole.rank[6] = FemHDNobRanks[6];
			urole.rank[7] = FemHDNobRanks[7];
			urole.rank[8] = FemHDNobRanks[8];
			
			urole.enemy1num = PM_ZOMBIE;
			urole.enemy2num = PM_CORVIAN_KNIGHT;
			urole.enemy1sym = S_ZOMBIE;
			urole.enemy2sym = S_GOLEM;
			
			urace.attrmax[A_STR] = STR19(20);
			urace.attrmax[A_DEX] = 20;
			urace.attrmax[A_CON] = 18;
			quest_status.leader_is_dead = TRUE;
		} else {
			flags.racial_pantheon = PM_HALF_DRAGON;
			urole.filecode = "Hdm";
			urole.petnum = PM_UNDEAD_KNIGHT;
			
			urole.homebase = "the lost towertop";
			urole.intermed = "the Archdragon Caves";
			urole.questarti = ART_DRAGON_S_HEART_STONE;
			
			urole.ldrnum = PM_ADVENTURESS_ZARIA;
			urole.guardnum = NON_PM;
			urole.neminum = PM_VECHERNYAYA_THE_SUN_S_GRAVE_KEEPER;
			
			urole.lgod = GOD_GWYN__LORD_OF_SUNLIGHT;
			urole.ngod = GOD_GWYNEVERE__PRINCESS_OF_SUN;
			urole.cgod = GOD_DARK_SUN_GWYNDOLIN;
			
			urole.enemy1num = PM_MAN_SERPENT_SOLDIER;
			urole.enemy2num = PM_MAN_SERPENT_MAGE;
			urole.enemy1sym = S_SNAKE;
			urole.enemy2sym = S_LIZARD;
			
			urace.attrmax[A_STR] = STR19(20);
			urace.attrmax[A_DEX] = 18;
			urace.attrmax[A_CON] = 20;
		}
	} else if (!urole.lgod) {
	    urole.lgod = roles[flags.pantheon].lgod;
	    urole.ngod = roles[flags.pantheon].ngod;
	    urole.cgod = roles[flags.pantheon].cgod;
	}
	if(flags.leader_backstab){
		mons[urole.ldrnum].msound = MS_CUSS;
		mons[urole.ldrnum].mflagst &= ~(MT_PEACEFUL);
		mons[urole.ldrnum].mflagst |= (MT_WANTSARTI|MT_WAITFORU|MT_STALK|MT_HOSTILE);
		mons[urole.ldrnum].mflagsg |= MG_NASTY;
		
		mons[urole.guardnum].msound = MS_CUSS;
		mons[urole.guardnum].mflagst &= ~(MT_PEACEFUL);
		mons[urole.guardnum].mflagst |= (MT_WAITFORU|MT_STALK|MT_HOSTILE);
		mons[urole.guardnum].mflagsg |= MG_NASTY;
	} else {
		/* Fix up the quest leader */
		if (urole.ldrnum != NON_PM) {
			mons[urole.ldrnum].msound = MS_LEADER;
			mons[urole.ldrnum].mflagst |= (MT_PEACEFUL|MT_CLOSE);
			mons[urole.ldrnum].mflagst &= ~(MT_WAITFORU|MT_COVETOUS);
		}

		/* Fix up the quest guardians */
		if (urole.guardnum != NON_PM) {
			mons[urole.guardnum].mflagst |= (MT_PEACEFUL);
			mons[urole.guardnum].mflagst &= ~(MT_WAITFORU|MT_COVETOUS);
		}
	}

	/* Fix up the quest nemesis */
	if (urole.neminum != NON_PM && 
		!(Race_if(PM_DWARF) && (Role_if(PM_KNIGHT) || Role_if(PM_NOBLEMAN))) && 
		!(Race_if(PM_DROW) && Role_if(PM_NOBLEMAN) && flags.initgend)
	) {
	    mons[urole.neminum].msound = MS_NEMESIS;
	    mons[urole.neminum].mflagst &= ~(MT_PEACEFUL);
	    mons[urole.neminum].mflagst |= (MT_WANTSARTI|MT_WAITFORU|MT_STALK|MT_HOSTILE);
	    mons[urole.neminum].mflagsg |= MG_NASTY;
	}
	
	if(Role_if(PM_ANACHRONONAUT)){
		//Drow noble nemesis is regular monster for anachrononauts
		mons[PM_ELDER_BRAIN].msound = MS_SILENT;
		mons[PM_ELDER_BRAIN].geno &= ~G_UNIQ;
		if (flags.questprogress)
			urole.lgod = GOD_ILSENSINE;
	}

	if(!Role_if(PM_CAVEMAN)){
		//Caveman nemesis is regular unique for others
		mons[PM_CHROMATIC_DRAGON].msound = MS_ROAR;
		mons[PM_CHROMATIC_DRAGON].mflagst &= ~MT_WANTSARTI;
	}

	// /* Fix up infravision */
	// if (mons[urace.malenum].mflagsv & MV_INFRAVISION) {
	    // /* although an infravision intrinsic is possible, infravision
	     // * is purely a property of the physical race.  This means that we
	     // * must put the infravision flag in the player's current race
	     // * (either that or have separate permonst entries for
	     // * elven/non-elven members of each class).  The side effect is that
	     // * all NPCs of that class will have (probably bogus) infravision,
	     // * but since infravision has no effect for NPCs anyway we can
	     // * ignore this.
	     // */
	    // mons[urole.malenum].mflagsv |= MV_INFRAVISION;
	    // if (urole.femalenum != NON_PM)
	    	// mons[urole.femalenum].mflagsv |= MV_INFRAVISION;
	// }
	
	/* Fix up the unknown firearms descriptions */
	if(Role_if(PM_PIRATE) || Role_if(PM_ANACHRONONAUT)){
		COPY_OBJ_DESCR(objects[FLINTLOCK], objects[HANDGUN]);
		COPY_OBJ_DESCR(objects[PISTOL], objects[HANDGUN]);
		
		COPY_OBJ_DESCR(objects[RIFLE], objects[LONG_GUN]);
		COPY_OBJ_DESCR(objects[ASSAULT_RIFLE], objects[LONG_GUN]);
		COPY_OBJ_DESCR(objects[SNIPER_RIFLE], objects[LONG_GUN]);
		COPY_OBJ_DESCR(objects[SHOTGUN], objects[LONG_GUN]);
		COPY_OBJ_DESCR(objects[AUTO_SHOTGUN], objects[LONG_GUN]);
		
		COPY_OBJ_DESCR(objects[SUBMACHINE_GUN], objects[GUN]);
		
		COPY_OBJ_DESCR(objects[HEAVY_MACHINE_GUN], objects[HEAVY_GUN]);
		COPY_OBJ_DESCR(objects[GRENADE_LAUNCHER], objects[HEAVY_GUN]);
	}

	/* Artifacts are fixed in hack_artifacts() */

	/* Success! */
	return;
}

const char *
Hello(mtmp)
struct monst *mtmp;
{
	switch (Role_switch) {
	case PM_KNIGHT:
	    return ("Salutations"); /* Olde English */
	case PM_SAMURAI:
	    return (mtmp && mtmp->mtyp == PM_SHOPKEEPER ?
	    		"Irasshaimase" : "Konnichi wa"); /* Japanese */
	case PM_PIRATE:
		return ("Ahoy");
#ifdef TOURIST
	case PM_TOURIST:
	    return ("Aloha");       /* Hawaiian */
#endif
	case PM_VALKYRIE:
	    return (
#ifdef MAIL
	    		mtmp && mtmp->mtyp == PM_MAIL_DAEMON ? "Hallo" :
#endif
	    		"Velkommen");   /* Norse */
	default:
	    return ("Hello");
	}
}

const char *
Goodbye()
{
	switch (Role_switch) {
	case PM_KNIGHT:
	    return ("Fare thee well");  /* Olde English */
	case PM_SAMURAI:
	    return ("Sayonara");        /* Japanese */
#ifdef TOURIST
	case PM_TOURIST:
	    return ("Aloha");           /* Hawaiian */
#endif
	case PM_VALKYRIE:
	    return ("Farvel");          /* Norse */
	default:
	    return ("Goodbye");
	}
}

#ifdef RECORD_ACHIEVE
void
give_quest_trophy()
{
	if(urole.neminum == PM_MINION_OF_HUHETOTL)
		achieve.trophies |= ARC_QUEST;
	else if(urole.neminum == PM_CHROMATIC_DRAGON)
		achieve.trophies |= CAV_QUEST;
	else if(urole.neminum == PM_WARDEN_ARIANNA)
		achieve.trophies |= CON_QUEST;
	else if(urole.neminum == PM_NECROMANCER)
		achieve.trophies |= ELF_SHR_QUEST;
	else if(flags.initgend && (urole.neminum == PM_SEYLL_AUZKOVYN || urole.neminum == PM_ECLAVDRA))
		achieve.trophies |= DRO_SHR_QUEST;
	else if(!flags.initgend && (urole.neminum == PM_DARUTH_XAXOX || urole.neminum == PM_ECLAVDRA))
		achieve.trophies |= HDR_SHR_QUEST;
	else if(urole.neminum == PM_NIMUNE)
		achieve.trophies |= KNI_QUEST;
	else if(Role_if(PM_ANACHRONONAUT)){
		if(Race_if(PM_ANDROID))
			achieve.trophies |= AND_QUEST;
		else
			achieve.trophies |= ANA_QUEST;
	}
	else if(urole.neminum == PM_ACERERAK)
		achieve.trophies |= BIN_QUEST;
	else if(urole.neminum == PM_REBEL_RINGLEADER)
		achieve.trophies |= NOB_QUEST;
	else if(urole.neminum == PM_DURIN_S_BANE)
		achieve.trophies |= DWA_NOB_QUEST;
	else if(urole.neminum == PM_BOLG)
		achieve.trophies |= DWA_KNI_QUEST;
	else if(urole.neminum == PM_ELDER_BRAIN)
		achieve.trophies |= DRO_NOB_QUEST;
	else if(urole.neminum == PM_A_SALOM)
		achieve.trophies |= HDR_NOB_QUEST;
	else if(urole.neminum == PM_BASTARD_OF_THE_BOREAL_VALLEY)
		achieve.trophies |= FEM_DRA_NOB_QUEST;
	else if(urole.neminum == PM_BLACKBEARD_S_GHOST)
		achieve.trophies |= PIR_QUEST;
	else if(urole.neminum == PM_AGLAOPE)
		achieve.trophies |= BRD_QUEST;
	else if(urole.neminum == PM_GREAT_HIGH_SHAMAN_OF_KURTULMAK)
		achieve.trophies |= GNO_RAN_QUEST;
	else if(urole.neminum == PM_DOCTOR_ARCHER)
		achieve.trophies |= MAD_QUEST;
	
	if(quest_status.second_thoughts)
		achieve.trophies |= SECOND_THOUGHTS;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_ascension_trophy()
{
	if(Role_if(PM_ANACHRONONAUT) && flags.questprogress == 2)
		achieve.trophies |= ANA_ASC;
	else if(Role_if(PM_EXILE))
		achieve.trophies |= BIN_ASC;
		
	if(Race_if(PM_CLOCKWORK_AUTOMATON))
		achieve.trophies |= CLOCK_ASC;
	else if(Race_if(PM_CHIROPTERAN))
		achieve.trophies |= CHIRO_ASC;
	else if(Race_if(PM_HALF_DRAGON))
		achieve.trophies |= HALF_ASC;
	else if(Race_if(PM_YUKI_ONNA))
		achieve.trophies |= YUKI_ASC;
	int i;
	int keys = 0;
	for(i = 0; i<9; i++){
		if(achieve.get_keys&(0x1 << i))
			keys++;
	}
	if(keys <= 2)
		achieve.trophies |= SPEED_PHASE;
	
	//There were some bugs with these, which *should* be fixed, but check all of them anyway.
	check_mithardir_trophy();
	check_illumian_trophy();
	check_loadout_trophy();
	check_madman_trophy();
	check_drunkard_trophy();
}
#endif

#ifdef RECORD_ACHIEVE
void
give_lamashtu_trophy()
{
	achieve.trophies |= LAMASHTU_KILL;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_baalphegor_trophy()
{
	achieve.trophies |= BAALPHEGOR_KILL;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_angel_vault_trophy()
{
	achieve.trophies |= ANGEL_VAULT;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_ancient_vault_trophy()
{
	achieve.trophies |= ANCIENT_VAULT;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_tannin_vault_trophy()
{
	achieve.trophies |= TANNINIM_VAULT;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_devil_vault_trophy()
{
	achieve.trophies |= DEVIL_VAULT;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_demon_vault_trophy()
{
	achieve.trophies |= DEMON_VAULT;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_hell_vault_trophy(hv_id)
int hv_id;
{
	switch(hv_id){
		case VN_AKKABISH:
		case VN_SHALOSH:
		case VN_NACHASH:
		case VN_KHAAMNUN:
		case VN_RAGLAYIM:
		case VN_TERAPHIM:
		case VN_SARTAN:
			give_tannin_vault_trophy();
		break;
		case VN_A_O_BLESSINGS:
		case VN_A_O_VITALITY:
		case VN_A_O_CORRUPTION:
		case VN_A_O_BURNING_WASTES:
		case VN_A_O_THOUGHT:
		case VN_A_O_DEATH:
			give_ancient_vault_trophy();
		break;
		case VN_APOCALYPSE:
		case VN_HARROWER:
		case VN_MAD_ANGEL:
			give_angel_vault_trophy();
		case VN_N_PIT_FIEND:
			give_devil_vault_trophy();
		case VN_SHAYATEEN:
			give_demon_vault_trophy();
		break;
	}
}
#endif

#ifdef RECORD_ACHIEVE
void
give_castle_trophy()
{
	achieve.trophies |= CASTLE_WISH;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_ugwish_trophy()
{
	achieve.trophies |= UNKNOWN_WISH;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_law_trophy()
{
	achieve.trophies |= LAW_QUEST;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_neutral_trophy()
{
	achieve.trophies |= NEU_QUEST;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_chaos_temple_trophy()
{
	if(chaos_dvariant == TEMPLE_OF_CHAOS)
		achieve.trophies |= CHA_QUEST;
}
#endif

#ifdef RECORD_ACHIEVE
void
check_mithardir_trophy()
{
	if(chaos_dvariant == MITHARDIR){
		if(u.ufirst_light && u.ufirst_sky && u.ufirst_life)
			achieve.trophies |= MITH_QUEST;
	}
}
#endif

#ifdef RECORD_ACHIEVE
void
give_mordor_trophy()
{
	if(chaos_dvariant == MORDOR)
		achieve.trophies |= MORD_QUEST;
}
#endif

#ifdef RECORD_ACHIEVE
void
check_illumian_trophy()
{
	int syls;
	syls = u.uaesh + u.ukrau + u.uhoon + u.uuur + u.unaen + u.uvaul;
	if(u.ufirst_light && u.ufirst_sky && u.ufirst_life && u.ufirst_know && syls >= 30 && u.sealsUsed&SEAL_TENEBROUS)
		achieve.trophies |= ILLUMIAN;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_rescue_trophy()
{
	achieve.trophies |= RESCUE;
}
#endif

#ifdef RECORD_ACHIEVE
void
check_loadout_trophy()
{
	long allupgrades = (OIL_STOVE|WOOD_STOVE|FAST_SWITCH|EFFICIENT_SWITCH|ARMOR_PLATING|PHASE_ENGINE|MAGIC_FURNACE|HELLFIRE_FURNACE|SCRAP_MAW);
	if((u.clockworkUpgrades&allupgrades) == allupgrades)
		achieve.trophies |= FULL_LOADOUT;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_nightmare_hunter_trophy()
{
	achieve.trophies |= NIGHTMAREHUNTER;
}
#endif

#ifdef RECORD_ACHIEVE
void
check_madman_trophy()
{
	int i;
	int count = 0;
	for(i = 0; i<64; i++){
		if(((u.umadness>>i) & 1) == 1)
			count++;
	}
	if(count >= 6)
		achieve.trophies |= QUITE_MAD;
}
#endif

#ifdef RECORD_ACHIEVE
void
check_drunkard_trophy()
{
	if(u.udrunken >= 90)
		achieve.trophies |= TOTAL_DRUNK;
}
#endif

#ifdef RECORD_ACHIEVE
void
give_bokrug_trophy()
{
	achieve.trophies |= BOKRUG_QUEST;
}
#endif

/* role.c */
