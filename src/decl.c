/*	SCCS Id: @(#)decl.c	3.2	2001/12/10	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

int NDECL((*afternmv));
int NDECL((*occupation));

/* from xxxmain.c */
const char *hname = 0;		/* name of the game (argv[0] of main) */
int hackpid = 0;		/* current process id */
#if defined(UNIX) || defined(VMS)
int locknum = 0;		/* max num of simultaneous users */
#endif
#ifdef DEF_PAGER
char *catmore = 0;		/* default pager */
#endif

NEARDATA int bases[MAXOCLASSES] = DUMMY;

long has_loaded_bones = 0L;

long last_clear_screen = 0L;

NEARDATA int multi = 0;
char multi_txt[BUFSZ] = DUMMY;
#if 0
NEARDATA int warnlevel = 0;		/* used by movemon and dochugw */
#endif
NEARDATA int nroom = 0;
NEARDATA int nsubroom = 0;
NEARDATA int occtime = 0;

int x_maze_max, y_maze_max;	/* initialized in main, used in mkmaze.c */
int otg_temp;			/* used by object_to_glyph() [otg] */

#ifdef REDO
NEARDATA int in_doagain = 0;
#endif

/*
 *	The following structure will be initialized at startup time with
 *	the level numbers of some "important" things in the game.
 */
struct dgn_topology dungeon_topology = {DUMMY};

#include "quest.h"
struct q_score	quest_status = DUMMY;

NEARDATA int smeq[MAXNROFROOMS+1] = DUMMY;
NEARDATA int doorindex = 0;

NEARDATA char *save_cm = 0;
NEARDATA int killer_format = 0;
const char *killer = 0;
const char *delayed_killer = 0;
#ifdef GOLDOBJ
NEARDATA long done_money = 0;
#endif
char killer_buf[BUFSZ] = DUMMY;

long killer_flags = 0L;

const char *nomovemsg = 0;
const char nul[40] = DUMMY;			/* contains zeros */
NEARDATA char plname[PL_NSIZ] = DUMMY;		/* player name */
NEARDATA char pl_character[PL_CSIZ] = DUMMY;
NEARDATA char pl_race = '\0';

NEARDATA char pl_fruit[PL_FSIZ] = DUMMY;
NEARDATA int current_fruit = 0;
NEARDATA struct fruit *ffruit = (struct fruit *)0;

NEARDATA char tune[6] = DUMMY;

const char *occtxt = DUMMY;
const char quitchars[] = " \r\n\033";
const char vowels[] = "aeiouAEIOU";
const char ynchars[] = "yn";
const char ynqchars[] = "ynq";
const char ynaqchars[] = "ynaq";
const char ynNaqchars[] = "yn#aq";
NEARDATA long yn_number = 0L;

const char disclosure_options[] = "iavgc";

#if defined(MICRO) || defined(WIN32)
char hackdir[PATHLEN];		/* where rumors, help, record are */
# ifdef MICRO
char levels[PATHLEN];		/* where levels are */
# endif
#endif /* MICRO || WIN32 */


#ifdef MFLOPPY
char permbones[PATHLEN];	/* where permanent copy of bones go */
int ramdisk = FALSE;		/* whether to copy bones to levels or not */
int saveprompt = TRUE;
const char *alllevels = "levels.*";
const char *allbones = "bones*.*";
#endif

struct linfo level_info[MAXLINFO];

NEARDATA struct sinfo program_state;

/* 'rogue'-like direction commands (cmd.c) */
#ifdef QWERTZ
const char qykbd_dir[] = "hykulnjb><";       /* qwerty layout */
const char qzkbd_dir[] = "hzkulnjb><";       /* qwertz layout */
char const *sdir=qykbd_dir;
#else
const char sdir[] = "hykulnjb><";
#endif
const char ndir[] = "47896321><";	/* number pad mode */
const schar xdir[10] = { -1,-1, 0, 1, 1, 1, 0,-1, 0, 0 };
const schar ydir[10] = {  0,-1,-1,-1, 0, 1, 1, 1, 0, 0 };
const schar zdir[10] = {  0, 0, 0, 0, 0, 0, 0, 0, 1,-1 };
char misc_cmds[] = {'g', 'G', 'F', 'm', 'M', '\033'
#ifdef REDO
		    , '\001'
#endif
};

NEARDATA schar tbx = 0, tby = 0;	/* mthrowu: target */

/* for xname handling of multiple shot missile volleys:
   number of shots, index of current one, validity check, shoot vs throw */
NEARDATA struct multishot m_shot = { 0, 0, STRANGE_OBJECT, FALSE };

NEARDATA struct dig_info digging;

NEARDATA dungeon dungeons[MAXDUNGEON];	/* ini'ed by init_dungeon() */
NEARDATA s_level *sp_levchn;
NEARDATA stairway upstair = { 0, 0 }, dnstair = { 0, 0 };
NEARDATA stairway upladder = { 0, 0 }, dnladder = { 0, 0 };
NEARDATA stairway sstairs = { 0, 0 };
NEARDATA dest_area updest = { 0, 0, 0, 0, 0, 0, 0, 0 };
NEARDATA dest_area dndest = { 0, 0, 0, 0, 0, 0, 0, 0 };
NEARDATA coord inv_pos = { 0, 0 };

NEARDATA boolean in_mklev = FALSE;
NEARDATA boolean stoned = FALSE;	/* done to monsters hit by 'c' */
NEARDATA boolean golded = FALSE;	/* done to monsters hit by Mammon's breath */
NEARDATA boolean glassed = FALSE;	/* done to monsters hit by Baalphegor's breath */
NEARDATA boolean unweapon = FALSE;
NEARDATA boolean mrg_to_wielded = FALSE;
boolean mon_ranged_gazeonly = FALSE;
			 /* weapon picked is merged with wielded one */
NEARDATA struct obj *current_wand = 0;	/* wand currently zapped/applied */

NEARDATA boolean in_steed_dismounting = FALSE;

NEARDATA coord bhitpos = DUMMY;
NEARDATA struct door doors[DOORMAX] = {DUMMY};

NEARDATA struct mkroom rooms[(MAXNROFROOMS+1)*2] = {DUMMY};
NEARDATA struct mkroom* subrooms = &rooms[MAXNROFROOMS+1];
struct mkroom *upstairs_room, *dnstairs_room, *sstairs_room;

dlevel_t level;		/* level map */
struct trap *ftrap = (struct trap *)0;
NEARDATA struct monst youmonst = DUMMY;
NEARDATA struct permonst upermonst = DUMMY;
NEARDATA struct flag flags = DUMMY;
NEARDATA struct instance_flags iflags = DUMMY;
NEARDATA struct you u = DUMMY;

/* objects that have been stored in a magic chest */
NEARDATA struct obj *magic_chest_objs[10] = {0};

NEARDATA struct obj *invent = (struct obj *)0,
	*uwep = (struct obj *)0, *uarm = (struct obj *)0,
	*uswapwep = (struct obj *)0,
	*uquiver = (struct obj *)0, /* quiver */
#ifdef TOURIST
	*uarmu = (struct obj *)0, /* under-wear, so to speak */
#endif
	*uskin = (struct obj *)0, /* dragon armor, if a dragon */
	*uarmc = (struct obj *)0, *uarmh = (struct obj *)0,
	*uarms = (struct obj *)0, *uarmg = (struct obj *)0,
	*uarmf = (struct obj *)0, *uamul = (struct obj *)0,
	*uright = (struct obj *)0,
	*uleft = (struct obj *)0,
	*ublindf = (struct obj *)0,
	*uchain = (struct obj *)0,
	*uball = (struct obj *)0,
	*urope = (struct obj *)0;

#ifdef TEXTCOLOR
/*
 * zap_glyph_color in zap.c assumes that this is in the same order as defined in color.c
 */
const int zapcolors[NUM_ZAP] = {
	CLR_BLACK,
	CLR_RED,
	CLR_GREEN,
	CLR_BROWN,
	CLR_BLUE,
	CLR_MAGENTA,
	CLR_CYAN,
	CLR_GRAY,
	NO_COLOR,
	CLR_ORANGE,
	CLR_BRIGHT_GREEN,
	CLR_YELLOW,
	CLR_BRIGHT_BLUE,
	CLR_BRIGHT_MAGENTA,
	CLR_BRIGHT_CYAN,
	CLR_WHITE
};
#endif /* text color */

struct god_details god_list[MAX_GOD] = {
	GOD(A_NONE, 0, ""),
	PANTHEON_DEFAULT("Quetzalcoatl", "Camaxtli", "Huhetotl"),
	PANTHEON_DEFAULT("Mitra", "Crom", "Set"),
	
	GOD(A_LAWFUL,	NEUTRAL_HOLINESS, 	"Anu"),
	GOD(A_NEUTRAL,	HOLY_HOLINESS, 		"_Ishtar"),
	GOD(A_CHAOTIC,	NEUTRAL_HOLINESS, 	"Anshar"),
	
	GOD(A_LAWFUL,	HOLY_HOLINESS, "Ilmater"),
	GOD(A_NEUTRAL,	NEUTRAL_HOLINESS, "Grumbar"),
	GOD(A_CHAOTIC,	HOLY_HOLINESS, "_Tymora"),
	
	GOD(A_LAWFUL,	HOLY_HOLINESS, "_Athena"),
	GOD(A_NEUTRAL,	HOLY_HOLINESS, "Hermes"),
	GOD(A_CHAOTIC,	NEUTRAL_HOLINESS, "Poseidon"),
	
	PANTHEON_HOLY("Lugh", "_Brigit", "Manannan Mac Lir"),
//"Shan Lai Ching", "Chih Sung-tzu", "Huan Ti", /* Chinese */
	PANTHEON_HOLY("Shan Lai Ching", "Chih Sung-tzu", "Huan Ti"),
//"God the Father", "_Mother Earth", "the Satan", /* Romanian, sorta */
	PANTHEON_DEFAULT("God the Father", "_Mother Earth", "the Satan"),
//"the Lord", "_the deep blue sea", "the Devil",	/* Christian, sorta */
	PANTHEON_DEFAULT("the Lord", "_the deep blue sea", "the Devil"),
//"Issek", "Mog", "Kos", /* Nehwon */
	GOD(A_LAWFUL,	HOLY_HOLINESS, "Issek of the Jug"),
	GOD(A_NEUTRAL,	NEUTRAL_HOLINESS, "Mog"),
	GOD(A_CHAOTIC,	NEUTRAL_HOLINESS, "Kos"),
//"Apollo", "_Latona", "_Diana", /* Roman */
	PANTHEON_HOLY("Apollo", "_Latona", "_Diana"),
//"_Amaterasu Omikami", "Raijin", "Susanowo", /* Japanese */
	PANTHEON_HOLY("_Amaterasu Omikami", "Raijin", "Susanowo"),
//"Blind Io", "_The Lady", "Offler", /* Discworld */
	PANTHEON_HOLY("Blind Io", "_The Lady", "Offler"),
//"Apollon", "Pan", "Dionysus", /* Thracian? */
	PANTHEON_HOLY("Apollon", "Pan", "Dionysus"),
//"Tyr", "Odin", "Loki", /* Norse */
	PANTHEON_DEFAULT("Tyr", "Odin", "Loki"),
//"Ptah", "Thoth", "Anhur", /* Egyptian */
	PANTHEON_DEFAULT("Ptah", "Thoth", "Anhur"),
	PANTHEON_HOLY("Orome", "_Yavanna", "Tulkas"),
	PANTHEON_HOLY("_Varda Elentari", "_Vaire", "_Nessa"),
	PANTHEON_HOLY("Manwe Sulimo", "Mandos", "Lorien"),
//Future
	GOD(A_LAWFUL,	UNHOLY_HOLINESS, "_Ilsensine"),
//Hedrow shared
	GOD(A_LAWFUL,	UNHOLY_HOLINESS, "Eddergud"),
	GOD(A_NEUTRAL,	UNHOLY_HOLINESS, "Vhaeraun"),
	GOD(A_CHAOTIC,	UNHOLY_HOLINESS, "_Lolth"),
//Hedrow noble
	GOD(A_LAWFUL,	UNHOLY_HOLINESS, "_Ver'tas"),
	GOD(A_NEUTRAL,	HOLY_HOLINESS,   "_Pen'a"),
	GOD(A_NEUTRAL,	UNHOLY_HOLINESS, "Keptolo"),
	GOD(A_CHAOTIC,	UNHOLY_HOLINESS, "Ghaunadaur"),
//Drow shared
	GOD(A_LAWFUL,	HOLY_HOLINESS,   "_Eilistraee"),
	GOD(A_NEUTRAL,	UNHOLY_HOLINESS, "_Kiaransali"),
	//Lolth again
//Drow noble
	//Ver'tas, Kiaransali, and Lolth again
//Binder
	GOD(A_LAWFUL,	HOLY_HOLINESS,   "Yaldabaoth"),
	GOD(A_VOID,		NEUTRAL_HOLINESS,"the void"),
	GOD(A_CHAOTIC,	HOLY_HOLINESS,   "_Pistis Sophia"),
//Dwarf
	GOD(A_LAWFUL,	HOLY_HOLINESS, "Mahal"),
	GOD(A_NEUTRAL,	UNHOLY_HOLINESS, "Holashner"),
	GOD(A_CHAOTIC,	UNHOLY_HOLINESS, "Armok"),
//Gnome
	GOD(A_LAWFUL,	UNHOLY_HOLINESS, "Kurtulmak"),
	GOD(A_NEUTRAL,	HOLY_HOLINESS, "Garl Glittergold"),
	GOD(A_CHAOTIC,	UNHOLY_HOLINESS, "Urdlen"),
//Half dragon noble
	GOD(A_LAWFUL,	HOLY_HOLINESS, "Gwyn, Lord of Sunlight"),
	GOD(A_NEUTRAL,	HOLY_HOLINESS, "_Gwynevere, Princess of Sunlight"),
	GOD(A_CHAOTIC,	NEUTRAL_HOLINESS, "Dark Sun Gwyndolin"),
//Orc noble
	PANTHEON_UNHOLY("Ilneval", "_Luthic", "Gruumsh"),
//Orc noble, elf
	PANTHEON_HOLY("_Vandria", "Corellon", "_Sehanine"),
//Orc noble, human
	PANTHEON_DEFAULT("Saint Cuthbert", "Helm", "_Mask"),
//Chaos quest
	GOD(A_NONE,	UNHOLY_HOLINESS, "The Silence"),
	GOD(A_NONE,	UNHOLY_HOLINESS, "Chaos"),
//Unaligned
	GOD(A_NONE,	UNHOLY_HOLINESS,  "Moloch"),
	GOD(A_NONE,	NEUTRAL_HOLINESS, "an alien god"),
	GOD(A_NONE,	NEUTRAL_HOLINESS, "_the Black Mother"),
	GOD(A_NONE,	NEUTRAL_HOLINESS, "Nodens"),
	GOD(A_NONE,	NEUTRAL_HOLINESS, "_Bast"),
	GOD(A_NONE,	UNHOLY_HOLINESS,  "the Dread Fracture"),
	GOD(A_NONE,	NEUTRAL_HOLINESS, "Yog-Sothoth")
};

const int shield_static[SHIELD_COUNT] = {
    S_ss1, S_ss2, S_ss3, S_ss2, S_ss1, S_ss2, S_ss4,	/* 7 per row */
//    S_ss1, S_ss2, S_ss3, S_ss2, S_ss1, S_ss2, S_ss4,
//    S_ss1, S_ss2, S_ss3, S_ss2, S_ss1, S_ss2, S_ss4,
};

NEARDATA struct spell spl_book[MAXSPELL + 1] = {DUMMY};

NEARDATA long moves = 1L, monstermoves = 1L;
	 /* These diverge when player is Fast */
NEARDATA long nonce = 0L;
NEARDATA long wailmsg = 0L;

/* objects that are moving to another dungeon level */
NEARDATA struct obj *migrating_objs = (struct obj *)0;
/* objects not yet paid for */
NEARDATA struct obj *billobjs = (struct obj *)0;

/* used to zero all elements of a struct obj */
NEARDATA struct obj zeroobj = DUMMY;

/* used to zero out union any */
NEARDATA anything zeroany = (anything) 0;

/* originally from dog.c */
NEARDATA char dogname[PL_PSIZ] = DUMMY;
NEARDATA char catname[PL_PSIZ] = DUMMY;
NEARDATA char parrotname[PL_PSIZ] = DUMMY;
NEARDATA char monkeyname[PL_PSIZ] = DUMMY;
NEARDATA char spidername[PL_PSIZ] = DUMMY;
NEARDATA char lizardname[PL_PSIZ] = DUMMY;
NEARDATA char dragonname[PL_PSIZ] = DUMMY;
NEARDATA char horsename[PL_PSIZ] = DUMMY;
#ifdef CONVICT
NEARDATA char ratname[PL_PSIZ] = DUMMY;
#endif /* CONVICT */
char preferred_pet;	/* '\0', 'c', 'd', 'n' (none) */
/* monsters that went down/up together with @ */
NEARDATA struct monst *mydogs = (struct monst *)0;
/* monsters that are moving to another dungeon level */
NEARDATA struct monst *migrating_mons = (struct monst *)0;

NEARDATA struct mvitals mvitals[NUMMONS];

/* originally from end.c */
#ifdef DUMP_LOG
#ifdef DUMP_FN
char dump_fn[] = DUMP_FN;
#else
char dump_fn[PL_PSIZ] = DUMMY;
#endif
#endif /* DUMP_LOG */

NEARDATA struct c_color_names c_color_names = {
	"black", "amber", "golden",
	"light blue", "red", "green",
	"silver", "blue", "purple",
	"white"
};

const char *c_obj_colors[] = {
	"black",		/* CLR_BLACK */
	"red",			/* CLR_RED */
	"green",		/* CLR_GREEN */
	"brown",		/* CLR_BROWN */
	"blue",			/* CLR_BLUE */
	"magenta",		/* CLR_MAGENTA */
	"cyan",			/* CLR_CYAN */
	"gray",			/* CLR_GRAY */
	"transparent",		/* no_color */
	"orange",		/* CLR_ORANGE */
	"bright green",		/* CLR_BRIGHT_GREEN */
	"yellow",		/* CLR_YELLOW */
	"bright blue",		/* CLR_BRIGHT_BLUE */
	"bright magenta",	/* CLR_BRIGHT_MAGENTA */
	"bright cyan",		/* CLR_BRIGHT_CYAN */
	"white",		/* CLR_WHITE */
};

#ifdef MENU_COLOR
struct menucoloring *menu_colorings = 0;
#endif

struct c_common_strings c_common_strings = {
	"Nothing happens.",		"That's enough tries!",
	"That is a silly thing to %s.",	"shudder for a moment.",
	"something", "Something", "You can move again.", "Never mind.",
	"vision quickly clears.", {"the", "your"}
};

/* struct defined in objclass.h, the master list of materials
 *
 * Densities used to be an attempt at making them super realistic, being in
 * terms of their kg/m^3 and as close to real life as possible, but that just
 * doesn't work because it makes materials infeasible to use. Nobody wants
 * anything gold or platinum if it weighs three times as much as its iron
 * counterpart, and things such as wooden plate mails were incredibly
 * overpowered by weighing about one-tenth as much as the iron counterpart.
 * Instead, use arbitrary units.
 */
const struct material materials[] = {
/*   id             color       density                    cost   defense  */
	{0,				CLR_BLACK,		  5,/*old:   5*/		  0,		0	},
	{LIQUID,		HI_ORGANIC,		 10,/*old:  10*/		  1, 		1	},
	{WAX,			CLR_WHITE,		 15,/*old:   9*/		  1, 		1	},
	{VEGGY,			HI_ORGANIC,		 10,/*old:   3*/		  1, 		1	},
	{FLESH,			CLR_RED,		 10,/*old:  11*/		  3, 		3	},
	{PAPER,			CLR_WHITE,		 10,/*old:  12*/		  2, 		1	},
	{CLOTH,			HI_CLOTH,		 10,/*old:  15*/		  3, 		2	},
	{LEATHER,		HI_LEATHER,		 15,/*old:   9*/		  5, 		3	},
	{WOOD,			HI_WOOD,		 30,/*old:   5*/		  8, 		4	},
	{BONE,			CLR_WHITE,		 25,/*old:  17*/		 20,		4	},
	{SHELL_MAT,		CLR_WHITE,		 25,/*old:  27*/		 30,		5	},
	{DRAGON_HIDE,	CLR_WHITE,		 35,/*old:  34*/		200,		8	},	/*note: color variable */
	{IRON,			HI_METAL,		 80,/*old:  79*/		 10,		5	},
	{METAL,			HI_METAL,		 70,/*old:  77*/		 10,		5	},
	{COPPER,		HI_COPPER,		 80,/*old:  89*/		 10,		5	},
	{SILVER,		HI_SILVER,		 90,/*old: 105*/		 30,		5	},
	{GOLD,			HI_GOLD,		120,/*old: 193*/		 60,		3	},
	{PLATINUM,		CLR_WHITE,		120,/*old: 214*/		 80,		4	},
	{MITHRIL,		HI_MITHRIL,		 40,/*old:  27*/		 50,		6	},
	{PLASTIC,		CLR_WHITE,		 20,/*old:  13*/		 10,		3	},
	{GLASS,			HI_GLASS,		 60,/*old:  24*/		 20,		5	},
	{GEMSTONE,		CLR_RED,		 55,/*old:  36*/		100,		7	},	/*note: color and cost variable */
	{MINERAL,		CLR_GRAY,		 50,/*old:  27*/		 15,		6	},
	{OBSIDIAN_MT,	CLR_BLACK,		 60,/*old:  26*/		 20,		4	},
	{SALT,			CLR_WHITE,		 60,/*old:  21*/		  5,		2	},
	{SHADOWSTEEL,	CLR_BLACK,		 30,/*old:  27*/		 50,		6	}
};

/* Vision */
NEARDATA boolean vision_full_recalc = 0;
NEARDATA char	 **viz_array = 0;/* used in cansee() and couldsee() macros */

/* Global windowing data, defined here for multi-window-system support */
NEARDATA winid WIN_MESSAGE = WIN_ERR, WIN_STATUS = WIN_ERR;
NEARDATA winid WIN_MAP = WIN_ERR, WIN_INVEN = WIN_ERR;
char toplines[TBUFSZ];
/* Windowing stuff that's really tty oriented, but present for all ports */
struct tc_gbl_data tc_gbl_data = { 0,0, 0,0 };	/* AS,AE, LI,CO */

char *fqn_prefix[PREFIX_COUNT] = { (char *)0, (char *)0, (char *)0, (char *)0,
				(char *)0, (char *)0, (char *)0, (char *)0, (char *)0 };

#ifdef PREFIXES_IN_USE
char *fqn_prefix_names[PREFIX_COUNT] = { "hackdir", "leveldir", "savedir",
					"bonesdir", "datadir", "scoredir",
					"lockdir", "configdir", "troubledir" };
#endif

#ifdef RECORD_ACHIEVE
struct u_achieve achieve = DUMMY;
#endif

#if defined(RECORD_REALTIME) || defined(REALTIME_ON_BOTL)
struct realtime_data realtime_data = { 0, 0, 0 };
#endif


struct _plinemsg *pline_msg = NULL;

/* FIXME: These should be integrated into objclass and permonst structs,
   but that invalidates saves */
glyph_t objclass_unicode_codepoint[NUM_OBJECTS] = DUMMY;
glyph_t permonst_unicode_codepoint[NUMMONS] = DUMMY;

/* FIXME: The curses windowport requires this stupid hack, in the
   case where a game is in progress and the user is asked if he
   wants to destroy old game.
   Without this, curses tries to show the yn() question with pline()
   ...but the message window isn't up yet.
 */
boolean curses_stupid_hack = 1;

/* dummy routine used to force linkage */
void
decl_init()
{
    return;
}

/*decl.c*/
