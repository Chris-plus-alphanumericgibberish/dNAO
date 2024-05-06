/*	SCCS Id: @(#)cmd.c	3.4	2003/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <ctype.h>

#include "hack.h"
#include "artifact.h"
#include "lev.h"
#include "func_tab.h"
#include "xhity.h"
/* #define DEBUG */	/* uncomment for debugging */

/*
 * Some systems may have getchar() return EOF for various reasons, and
 * we should not quit before seeing at least NR_OF_EOFS consecutive EOFs.
 */
#if defined(SYSV) || defined(DGUX) || defined(HPUX)
#define NR_OF_EOFS	20
#endif

#define CMD_TRAVEL (char)0x90

#ifdef DEBUG
/*
 * only one "wiz_debug_cmd" routine should be available (in whatever
 * module you are trying to debug) or things are going to get rather
 * hard to link :-)
 */
extern int NDECL(wiz_debug_cmd);
#endif

#ifdef DUMB	/* stuff commented out in extern.h, but needed here */
extern int NDECL(doapply); /**/
extern int NDECL(dorub); /**/
extern int NDECL(dojump); /**/
extern int NDECL(docome); /**/
extern int NDECL(dodropall); /**/
extern int NDECL(dopassive); /**/
extern int NDECL(doextlist); /**/
extern int NDECL(doattack); /**/
extern int NDECL(dodrop); /**/
extern int NDECL(doddrop); /**/
extern int NDECL(dodown); /**/
extern int NDECL(dodownboy); /**/
extern int NDECL(doup); /**/
extern int NDECL(donull); /**/
extern int NDECL(dowait); /**/
extern int NDECL(dowipe); /**/
extern int NDECL(do_mname); /**/
extern int NDECL(ddocall); /**/
extern void FDECL(do_oname, (struct obj *));
extern void NDECL(do_floorname); /**/
extern int NDECL(dotakeoff); /**/
extern int NDECL(doremring); /**/
extern int NDECL(dowear); /**/
extern int NDECL(doputon); /**/
extern int NDECL(doddoremarm); /**/
extern int NDECL(dokick); /**/
extern int NDECL(dofire); /**/
extern int NDECL(dothrow); /**/
extern int NDECL(doeat); /**/
extern int NDECL(done2); /**/
extern int NDECL(doengward); /**/
extern int NDECL(doengrave); /**/
extern int NDECL(doward); /**/
extern int NDECL(dopickup); /**/
extern int NDECL(ddoinv); /**/
extern int NDECL(dotypeinv); /**/
extern int NDECL(dolook); /**/
extern int NDECL(doprgold); /**/
extern int NDECL(doprwep); /**/
extern int NDECL(doprarm); /**/
extern int NDECL(doprring); /**/
extern int NDECL(dopramulet); /**/
extern int NDECL(doprtool); /**/
extern int NDECL(dosuspend); /**/
extern int NDECL(doforce); /**/
extern int NDECL(doopen); /**/
extern int NDECL(doclose); /**/
extern int NDECL(dosh); /**/
extern int NDECL(dodiscovered); /**/
extern int NDECL(doset); /**/
extern int NDECL(dotogglepickup); /**/
extern int NDECL(dowhatis); /**/
extern int NDECL(doquickwhatis); /**/
extern int NDECL(dowhatdoes); /**/
extern int NDECL(dohelp); /**/
extern int NDECL(dohistory); /**/
extern int NDECL(doloot); /**/
extern int NDECL(dodrink); /**/
extern int NDECL(dodip); /**/
extern int NDECL(dosacrifice); /**/
extern int NDECL(dopray); /**/
extern int NDECL(doturn); /**/
extern int NDECL(dotip); /**/
extern int NDECL(doredraw); /**/
extern int NDECL(doread); /**/
extern int NDECL(dosave); /**/
extern int NDECL(dosearch); /**/
extern int NDECL(doidtrap); /**/
extern int NDECL(dopay); /**/
extern int NDECL(dosickem); /**/
extern int NDECL(dosit); /**/
extern int NDECL(dodeepswim); /**/
extern int NDECL(dotalk); /**/
extern int NDECL(docast); /**/
extern int NDECL(dovspell); /**/
extern int NDECL(reorder_spirit_powers); /**/
extern int NDECL(dotele); /**/
extern int NDECL(dountrap); /**/
extern int NDECL(dounmaintain); /**/
extern int NDECL(doversion); /**/
extern int NDECL(doextversion); /**/
extern int NDECL(doswapweapon); /**/
extern int NDECL(dowield); /**/
extern int NDECL(dowieldquiver); /**/
extern int NDECL(dozap); /**/
extern int NDECL(doorganize); /**/
#endif /* DUMB */

#ifdef OVL1
static int NDECL((*timed_occ_fn));
#endif /* OVL1 */

STATIC_DCL int NDECL(use_reach_attack);
STATIC_PTR int NDECL(doprev_message);
STATIC_PTR int NDECL(timed_occupation);
STATIC_PTR int NDECL(doextcmd);
STATIC_PTR int NDECL(doability);
STATIC_PTR int NDECL(domonability);
STATIC_PTR int FDECL(ability_menu, (boolean, boolean));
STATIC_PTR int NDECL(domountattk);
STATIC_PTR int NDECL(hasfightingforms);
STATIC_PTR int NDECL(doMysticForm);
STATIC_PTR int NDECL(doLightsaberForm);
STATIC_PTR int NDECL(doKnightForm);
STATIC_PTR int NDECL(dofightingform);
STATIC_PTR int NDECL(dooverview_or_wiz_where);
STATIC_PTR int NDECL(doclearinvissyms);
# ifdef WIZARD
STATIC_PTR int NDECL(wiz_bind);
STATIC_PTR int NDECL(wiz_mutate);
STATIC_PTR int NDECL(wiz_cult);
STATIC_PTR int NDECL(wiz_mk_mapglyphdump);
STATIC_PTR int NDECL(wiz_wish);
STATIC_PTR int NDECL(wiz_identify);
STATIC_PTR int NDECL(wiz_map);
//STATIC_PTR int NDECL(wiz_makemap);
STATIC_PTR int NDECL(wiz_genesis);
STATIC_PTR int NDECL(wiz_where);
STATIC_PTR int NDECL(wiz_detect);
STATIC_PTR int NDECL(wiz_panic);
STATIC_PTR int NDECL(wiz_polyself);
STATIC_PTR int NDECL(wiz_level_tele);
STATIC_PTR int NDECL(wiz_level_change);
STATIC_PTR int NDECL(wiz_show_seenv);
STATIC_PTR int NDECL(wiz_show_vision);
STATIC_PTR int NDECL(wiz_mon_polycontrol);
STATIC_PTR int NDECL(wiz_show_wmodes);
STATIC_PTR int NDECL(wiz_showkills);	/* showborn patch */
STATIC_PTR int NDECL(wiz_setinsight);
STATIC_PTR int NDECL(wiz_setsanity);
STATIC_PTR int FDECL(getvalue, (const char *));
#ifdef SHOW_BORN
extern void FDECL(list_vanquished, (int, BOOLEAN_P)); /* showborn patch */
#endif /* SHOW_BORN */
#if defined(__BORLANDC__) && !defined(_WIN32)
extern void FDECL(show_borlandc_stats, (winid));
#endif
#ifdef DEBUG_MIGRATING_MONS
STATIC_PTR int NDECL(wiz_migrate_mons);
#endif
STATIC_DCL void FDECL(count_obj, (struct obj *, long *, long *, BOOLEAN_P, BOOLEAN_P));
STATIC_DCL void FDECL(obj_chain, (winid, const char *, struct obj *, long *, long *));
STATIC_DCL void FDECL(magic_chest_obj_chain, (winid, const char *, long *, long *));
STATIC_DCL void FDECL(mon_invent_chain, (winid, const char *, struct monst *, long *, long *));
STATIC_DCL void FDECL(mon_chain, (winid, const char *, struct monst *, long *, long *));
STATIC_DCL void FDECL(contained, (winid, const char *, long *, long *));
STATIC_PTR int NDECL(wiz_show_stats);
#  ifdef PORT_DEBUG
STATIC_DCL int NDECL(wiz_port_debug);
#  endif
# endif
STATIC_PTR int NDECL(enter_explore_mode);

static void FDECL(bind_key, (UCHAR_P, char*));
static void NDECL(init_bind_list);
static void NDECL(change_bind_list);
#ifdef WIZARD
//static void NDECL(add_debug_extended_commands); //Moved to extern.h
#endif /* WIZARD */

#ifdef OVLB
#ifdef UNIX
static void NDECL(end_of_input);
#endif
#endif /* OVLB */

static const char* readchar_queue="";
static char last_cmd_char='\0';

STATIC_DCL char *NDECL(parse);
STATIC_DCL boolean FDECL(help_dir, (CHAR_P,const char *));

#ifdef OVL1

STATIC_PTR int
doprev_message()
{
    return nh_doprev_message();
}

/* Count down by decrementing multi */
STATIC_PTR int
timed_occupation()
{
	int result;
	result = (*timed_occ_fn)();
	if (multi > 0)
		multi--;
	return (multi > 0) ? result : (result|MOVE_FINISHED_OCCUPATION);
}

/* If you have moved since initially setting some occupations, they
 * now shouldn't be able to restart.
 *
 * The basic rule is that if you are carrying it, you can continue
 * since it is with you.  If you are acting on something at a distance,
 * your orientation to it must have changed when you moved.
 *
 * The exception to this is taking off items, since they can be taken
 * off in a number of ways in the intervening time, screwing up ordering.
 *
 *	Currently:	Take off all armor.
 *			Picking Locks / Forcing Chests.
 *			Setting traps.
 */
void
reset_occupations()
{
	reset_remarm();
	reset_pick();
	reset_trapset();
}

/* If a time is given, use it to timeout this function, otherwise the
 * function times out by its own means.
 */
void
set_occupation(fn, txt, xtime)
int NDECL((*fn));
const char *txt;
int xtime;
{
	if (xtime) {
		occupation = timed_occupation;
		timed_occ_fn = fn;
	} else
		occupation = fn;
	occtxt = txt;
	occtime = 0;
	return;
}

#ifdef REDO

static char NDECL(popch);

/* Provide a means to redo the last command.  The flag `in_doagain' is set
 * to true while redoing the command.  This flag is tested in commands that
 * require additional input (like `throw' which requires a thing and a
 * direction), and the input prompt is not shown.  Also, while in_doagain is
 * TRUE, no keystrokes can be saved into the saveq.
 */
#define BSIZE 20
static char pushq[BSIZE], saveq[BSIZE];
static NEARDATA int phead, ptail, shead, stail;

static char
popch() {
	/* If occupied, return '\0', letting tgetch know a character should
	 * be read from the keyboard.  If the character read is not the
	 * ABORT character (as checked in pcmain.c), that character will be
	 * pushed back on the pushq.
	 */
	if (occupation) return '\0';
	if (in_doagain) return(char)((shead != stail) ? saveq[stail++] : '\0');
	else		return(char)((phead != ptail) ? pushq[ptail++] : '\0');
}

char
pgetchar() {		/* curtesy of aeb@cwi.nl */
	register int ch;
	

	if (iflags.debug_fuzzer)
       		return randomkey();

	if(!(ch = popch()))
		ch = nhgetch();
	return((char)ch);
}

/* A ch == 0 resets the pushq */
void
pushch(ch)
char ch;
{
	if (!ch)
		phead = ptail = 0;
	if (phead < BSIZE)
		pushq[phead++] = ch;
	return;
}

/* A ch == 0 resets the saveq.	Only save keystrokes when not
 * replaying a previous command.
 */
void
savech(ch)
char ch;
{
	if (!in_doagain) {
		if (!ch)
			phead = ptail = shead = stail = 0;
		else if (shead < BSIZE)
			saveq[shead++] = ch;
	}
	return;
}
#endif /* REDO */

#endif /* OVL1 */
#ifdef OVLB

STATIC_PTR int
doextcmd()	/* here after # - now read a full-word command */
{
	int idx, retval;

	/* keep repeating until we don't run help or quit */
	do {
	    idx = get_ext_cmd();
	    if (idx < 0) return 0;	/* quit */

	    retval = (*extcmdlist[idx].ef_funct)();
	} while (extcmdlist[idx].ef_funct == doextlist);

	return retval;
}

int
doextlist()	/* here after #? - now list all full-word commands */
{
	register const struct ext_func_tab *efp;
	char	 buf[BUFSZ];
	winid datawin;

	datawin = create_nhwindow(NHW_TEXT);
	putstr(datawin, 0, "");
	putstr(datawin, 0, "            Extended Commands List");
	putstr(datawin, 0, "");
	putstr(datawin, 0, "    Press '#', then type:");
	putstr(datawin, 0, "");

	for(efp = extcmdlist; efp->ef_txt; efp++) {
		/* Show name and text for each command.  Autocompleted
		 * commands are marked with an asterisk ('*'). */
		Sprintf(buf, "  %c %-15s - %s.",
			efp->autocomplete ? '*' : ' ',
			efp->ef_txt, efp->ef_desc);
		putstr(datawin, 0, buf);
	}
	putstr(datawin, 0, "");
	putstr(datawin, 0, "    Commands marked with a * will be autocompleted.");
	display_nhwindow(datawin, FALSE);
	destroy_nhwindow(datawin);
	return 0;
}

#if defined(TTY_GRAPHICS) || defined(CURSES_GRAPHICS)
#define MAX_EXT_CMD 200		/* Change if we ever have more ext cmds */
/*
 * This is currently used only by the tty port and is
 * controlled via runtime option 'extmenu'
 */
int
extcmd_via_menu()	/* here after # - now show pick-list of possible commands */
{
    const struct ext_func_tab *efp;
    menu_item *pick_list = (menu_item *)0;
    winid win;
    anything any;
    const struct ext_func_tab *choices[MAX_EXT_CMD];
    char buf[BUFSZ];
    char cbuf[QBUFSZ], prompt[QBUFSZ], fmtstr[20];
    int i, n, nchoices, acount;
    int ret,  biggest;
    int accelerator, prevaccelerator;
    int  matchlevel = 0;

    ret = 0;
    cbuf[0] = '\0';
    biggest = 0;
    while (!ret) {
	    i = n = 0;
	    accelerator = 0;
	    any.a_void = 0;
	    /* populate choices */
	    for(efp = extcmdlist; efp->ef_txt; efp++) {
		if (!matchlevel || !strncmp(efp->ef_txt, cbuf, matchlevel)) {
			choices[i++] = efp;
			if ((int)strlen(efp->ef_desc) > biggest) {
				biggest = strlen(efp->ef_desc);
				Sprintf(fmtstr,"%%-%ds", biggest + 15);
			}
#ifdef DEBUG
			if (i >= MAX_EXT_CMD - 2) {
			    impossible("Exceeded %d extended commands in doextcmd() menu",
					MAX_EXT_CMD - 2);
			    return 0;
			}
#endif
		}
	    }
	    choices[i] = (struct ext_func_tab *)0;
	    nchoices = i;
	    /* if we're down to one, we have our selection so get out of here */
	    if (nchoices == 1) {
		for (i = 0; extcmdlist[i].ef_txt != (char *)0; i++)
			if (!strncmpi(extcmdlist[i].ef_txt, cbuf, matchlevel)) {
				ret = i;
				break;
			}
		break;
	    }

	    /* otherwise... */
	    win = create_nhwindow(NHW_MENU);
	    start_menu(win);
	    prevaccelerator = 0;
	    acount = 0;
	    for(i = 0; choices[i]; ++i) {
		accelerator = choices[i]->ef_txt[matchlevel];
		if (accelerator != prevaccelerator || nchoices < (ROWNO - 3)) {
		    if (acount) {
 			/* flush the extended commands for that letter already in buf */
			Sprintf(buf, fmtstr, prompt);
			any.a_char = prevaccelerator;
			add_menu(win, NO_GLYPH, &any, any.a_char, 0,
					ATR_NONE, buf, FALSE);
			acount = 0;
		    }
		}
		prevaccelerator = accelerator;
		if (!acount || nchoices < (ROWNO - 3)) {
		    Sprintf(prompt, "%s [%s]", choices[i]->ef_txt,
				choices[i]->ef_desc);
		} else if (acount == 1) {
		    Sprintf(prompt, "%s or %s", choices[i-1]->ef_txt,
				choices[i]->ef_txt);
		} else {
		    Strcat(prompt," or ");
		    Strcat(prompt, choices[i]->ef_txt);
		}
		++acount;
	    }
	    if (acount) {
		/* flush buf */
		Sprintf(buf, fmtstr, prompt);
		any.a_char = prevaccelerator;
		add_menu(win, NO_GLYPH, &any, any.a_char, 0, ATR_NONE, buf, FALSE);
	    }
	    Sprintf(prompt, "Extended Command: %s", cbuf);
	    end_menu(win, prompt);
	    n = select_menu(win, PICK_ONE, &pick_list);
	    destroy_nhwindow(win);
	    if (n==1) {
		if (matchlevel > (QBUFSZ - 2)) {
			free((genericptr_t)pick_list);
#ifdef DEBUG
			impossible("Too many characters (%d) entered in extcmd_via_menu()",
				matchlevel);
#endif
			ret = -1;
		} else {
			cbuf[matchlevel++] = pick_list[0].item.a_char;
			cbuf[matchlevel] = '\0';
			free((genericptr_t)pick_list);
		}
	    } else {
		if (matchlevel) {
			ret = 0;
			matchlevel = 0;
		} else
			ret = -1;
	    }
    }
    return ret;
}
#endif

/* #ability command - use standard abilities, maybe polymorphed */
STATIC_PTR int
doability()
{
	return ability_menu(iflags.quick_m_abilities, TRUE);
}

/* #monster command - use special monster abilities while polymorphed */
STATIC_PTR int
domonability()
{
	return ability_menu(TRUE, FALSE);
}

STATIC_PTR int
ability_menu(mon_abilities, you_abilities)
boolean mon_abilities;
boolean you_abilities;
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;
	boolean atleastone = FALSE;
	
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	

#define add_ability(letter, string, value) \
	do { \
	Sprintf(buf, (string)); any.a_int = (value); atleastone = TRUE; \
	add_menu(tmpwin, NO_GLYPH, &any, (letter), 0, ATR_NONE, buf, MENU_UNSELECTED); \
	} while (0)

	Sprintf(buf, "Abilities");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	if (mon_abilities && uarm && uarms &&
		Is_dragon_armor(uarm) && Is_dragon_shield(uarms) && 
		(Dragon_armor_matches_mtyp(uarm, Dragon_armor_to_pm(uarms)->mtyp)
		|| Dragon_armor_matches_mtyp(uarms, Dragon_armor_to_pm(uarm)->mtyp)) &&
		uarm->age < monstermoves && uarms->age < monstermoves
	){
		boolean armormatch = FALSE;
		/* halfdragons combine armor breath with their own; don't list if it matches */
		if (Race_if(PM_HALF_DRAGON)) {
			int mtyp;
			switch (flags.HDbreath) {
			case AD_FIRE: mtyp = PM_RED_DRAGON;    break;
			case AD_COLD: mtyp = PM_WHITE_DRAGON;  break;
			case AD_ELEC: mtyp = PM_BLUE_DRAGON;   break;
			case AD_DRST: mtyp = PM_GREEN_DRAGON;  break;
			case AD_SLEE: mtyp = PM_ORANGE_DRAGON; break;
			case AD_ACID: mtyp = PM_YELLOW_DRAGON; break;
			case AD_MAGM: mtyp = PM_GRAY_DRAGON;   break;
			case AD_RBRE: mtyp = PM_SHIMMERING_DRAGON;   break;
			default: impossible("unhandled HD type %d", flags.HDbreath); mtyp = PM_RED_DRAGON; break;
			}
			/* note: when shield and armor match despite color differences, it is the shield's color that is used */
			armormatch = Dragon_shield_to_pm(uarms) == &mons[mtyp];
		}
		if (!armormatch) {
			add_ability('a', "Use your armor's breath weapon", MATTK_DSCALE);
		}
	}
	if (mon_abilities && (is_were(youracedata) || gates_in_help(youracedata))){
		/* shared letter; assumes a polyform will only be one or the other */
		add_ability('A', "Summon aid", MATTK_SUMM);
	}
	if (mon_abilities && (can_breathe(youmonst.data) || Race_if(PM_HALF_DRAGON))){
		add_ability('b', "Use your breath weapon", MATTK_BREATH);
	}
	if (mon_abilities && (Upolyd && can_breathe(youmonst.data) && Race_if(PM_HALF_DRAGON))){
		add_ability('B', "Use your halfdragon breath weapon", MATTK_HBREATH);
	}
	if (mon_abilities && uclockwork){
		add_ability('c', "Adjust your clockspeed", MATTK_CLOCK);
	}
	if (mon_abilities && uandroid){
		add_ability('d', "Use Android abilities", MATTK_DROID);
	}
	if (you_abilities && Race_if(PM_HALF_DRAGON) && Role_if(PM_BARD) && u.ulevel >= 14) {
		add_ability('E', "Sing an Elemental into being", MATTK_U_ELMENTAL);
	}
	if (you_abilities && (Role_if(PM_EXILE) || u.sealsActive || u.specialSealsActive)) {
		add_ability('f', "Fire a spirit power", MATTK_U_SPIRITS);
	}
	if (you_abilities && hasfightingforms() > 0) {	/* I can't wait until fighting forms are mainstream */
		add_ability('F', "Pick a fighting form", MATTK_U_STYLE);
	}
	if (mon_abilities && (attacktype(youracedata, AT_GAZE) || (!Upolyd && check_vampire(VAMPIRE_GAZE)))){
		add_ability('g', "Gaze at something", MATTK_GAZE);
	}
	if (mon_abilities && is_hider(youracedata)){
		add_ability('h', "Hide", MATTK_HIDE);
	}
	if (mon_abilities && youracedata->mtyp == PM_TOVE){
		add_ability('H', "Bore a hole", MATTK_HOLE);
	}
	if (mon_abilities && is_drow(youracedata)){
		add_ability('i', "Invoke the darkness", MATTK_DARK);
	}
	if (mon_abilities && youracedata->mlet == S_NYMPH){
		add_ability('I', "Remove an iron ball", MATTK_REMV);
	}
	if (mon_abilities && (is_mind_flayer(youracedata)
			|| Role_if(PM_MADMAN)
			|| (check_mutation(TWIN_DREAMS) && u.specialSealsActive&SEAL_YOG_SOTHOTH)
	) && !Catapsi){
		add_ability('m', "Emit a mind blast", MATTK_MIND);
	}
	if (you_abilities && !mon_abilities){
		add_ability('M', "Use a monstrous ability", MATTK_U_MONST);
	}
	if (you_abilities && (u.ufirst_light || u.ufirst_sky || u.ufirst_life || u.ufirst_know)){
		add_ability('p', "Speak a word of power", MATTK_U_WORD);
	}
	if (mon_abilities && (attacktype(youracedata, AT_LNCK) || attacktype(youracedata, AT_LRCH))){
		add_ability('r', "Make a reach attack", MATTK_REACH);
	}
	if (mon_abilities && u.umonnum == PM_GREMLIN){
		add_ability('R', "Replicate yourself", MATTK_REPL);
	}
	if (mon_abilities && attacktype(youracedata, AT_SPIT)){
		add_ability('s', "Spit", MATTK_SPIT);
	}
	if (mon_abilities && (youracedata->msound == MS_SHRIEK || youracedata->msound == MS_SHOG)){ //player can't speak elder thing.
		add_ability('S', "Shriek", MATTK_SHRIEK);
	}
	if (mon_abilities && youracedata->msound == MS_JUBJUB){
		add_ability('S', "Scream", MATTK_SCREAM);
	}
	if (mon_abilities && attacktype(youracedata, AT_TNKR)){
		add_ability('t', "Tinker", MATTK_TNKR);
	}
	if (you_abilities && (Role_if(PM_PRIEST) || Role_if(PM_KNIGHT) || Race_if(PM_VAMPIRE) || (Role_if(PM_NOBLEMAN) && Race_if(PM_ELF)))) {
		add_ability('T', "Turn undead", MATTK_U_TURN_UNDEAD);
	}
	if (mon_abilities && is_unicorn(youracedata)){
		add_ability('u', "Use your unicorn horn", MATTK_UHORN);
	}
	if (mon_abilities && is_vampire(youracedata) && u.ulevel > 1){
		add_ability('V', "Raise a vampiric minion", MATTK_VAMP);
	}
	if (mon_abilities && webmaker(youracedata)){
		add_ability('w', "Spin a web", MATTK_WEBS);
	}
	if (Role_if(PM_MADMAN) && u.whisperturn < moves && !Catapsi && !DimensionalLock){
		add_ability('W', "Call your whisperer", MATTK_WHISPER);
	}
	if (Role_if(PM_SAMURAI) && u.kiaiturn < moves && u.uencouraged < xlev_to_rank(u.ulevel)){
		add_ability('K', "Kiai shout", MATTK_KI);
	}
	if (you_abilities && spellid(0) != NO_SPELL) {
		add_ability('z', "Cast spells", MATTK_U_SPELLS);
	}
	if (mon_abilities && attacktype(youracedata, AT_MAGC)){
		add_ability('Z', "Cast a monster spell", MATTK_MAGIC);
	}

#undef add_ability

	if(!atleastone){
		if (!you_abilities) {
			if (Upolyd) pline("Any special ability you may have is purely reflexive.");
			else You("don't have a special ability in your normal form!");
		}
		else {
			pline("You are extraordinary mundane.");
		}
		destroy_nhwindow(tmpwin);
		return MOVE_CANCELLED;
	}
	
	if (mon_abilities && you_abilities)
		Strcpy(buf, "Choose which ability to use");
	else
		Sprintf(buf, "Choose which %s ability to use", mon_abilities ? "monster" : "player");
	end_menu(tmpwin, buf);

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	
	if(n <= 0) return MOVE_CANCELLED;
	
	int picked = selected[0].item.a_int;
	free(selected);
	
	switch (picked) {
	/* Player abilities */
	case MATTK_U_WORD: return dowords(SPELLMENU_CAST);
	case MATTK_U_SPELLS: return docast();
	case MATTK_U_SPIRITS: return dospirit();
	case MATTK_U_TURN_UNDEAD: return doturn();
	case MATTK_U_STYLE: return dofightingform();
	case MATTK_U_MONST: return domonability();
	case MATTK_U_ELMENTAL: return doelementalbreath();
	case MATTK_WHISPER: return domakewhisperer();
	case MATTK_KI: return dokiai();

	/* Monster (or monster-like) abilities */
	case MATTK_BREATH: return dobreathe(youmonst.data);
	case MATTK_HBREATH: return dobreathe(&mons[PM_HALF_DRAGON]);
	case MATTK_DSCALE:{
		int res = dobreathe(Dragon_shield_to_pm(uarms));
		if(!(res & MOVE_CANCELLED)){
			if(!uarm->oartifact) uarm->age = monstermoves + (long)(rnz(100)*(Role_if(PM_CAVEMAN) ? .8 : 1));
			uarms->age = monstermoves + (long)(rnz(100)*(Role_if(PM_CAVEMAN) ? .8 : 1));
		}
		return res;
	}
	case MATTK_SPIT: return dospit();
	case MATTK_MAGIC: 
		return xcasty(&youmonst, (struct monst *)0, &youracedata->mattk[attackindex(youracedata, AT_MAGC, AD_ANY)], 0, 0, 0) ? MOVE_CASTSPELL : MOVE_CANCELLED;
		
//		return castum((struct monst *)0,
//	                   &youracedata->mattk[attackindex(youracedata, 
//			                         AT_MAGC,AD_ANY)]);
	case MATTK_REMV: return doremove();
	case MATTK_GAZE: return dogaze();
	case MATTK_TNKR: return dotinker();
	case MATTK_SUMM: return (is_were(youracedata) ? dosummon() : dodemonpet());
	case MATTK_VAMP: return dovampminion();
	case MATTK_WEBS: return dospinweb();
	case MATTK_HIDE: return dohide();
	case MATTK_MIND: return domindblast();
	case MATTK_CLOCK: return doclockspeed();
	case MATTK_DROID: return doandroid();
	case MATTK_DARK: return dodarken();
	case MATTK_REPL: {
	    if(IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
			if (split_mon(&youmonst, (struct monst *)0))
				dryup(u.ux, u.uy, TRUE);
			return MOVE_STANDARD;
	    } else {
			There("is no fountain here.");
			return MOVE_CANCELLED;
		}
	}
	break;
	case MATTK_UHORN: {
	    use_unicorn_horn((struct obj *)0);
	    return MOVE_STANDARD;
	}
	break;
	case MATTK_SHRIEK: {
	    You("shriek.");
	    if(u.uburied) pline("Unfortunately sound does not carry well through rock.");
	    else aggravate();
		return MOVE_STANDARD;
	}
	break;
	case MATTK_SCREAM: {
	    You("scream high and shrill.");
	    if(u.uburied) pline("Unfortunately sound does not carry well through rock.");
	    else{
			struct monst *tmpm;
			for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
				if(tmpm->mtame && tmpm->mtame<20) tmpm->mtame++;
				if(d(1,tmpm->mhp) < Upolyd ? u.mh : u.uhp){
					tmpm->mflee = 1;
				}
			}
		}
		return MOVE_STANDARD;
	}
	break;
	case MATTK_HOLE: {
		struct trap *ttmp = t_at(u.ux, u.uy);
		struct rm *lev = &levl[u.ux][u.uy];
		schar typ;
		boolean nohole = !Can_dig_down(&u.uz);
		if (!(
			!isok(u.ux,u.uy) || 
			(ttmp && ttmp->ttyp == MAGIC_PORTAL) ||
		   /* ALI - artifact doors from slash'em */
			(IS_DOOR(levl[u.ux][u.uy].typ) && artifact_door(u.ux, u.uy)) ||
			(IS_ROCK(lev->typ) && lev->typ != SDOOR &&
			(lev->wall_info & W_NONDIGGABLE) != 0) ||
			(is_pool(u.ux, u.uy, TRUE) || is_lava(u.ux, u.uy)) ||
			(lev->typ == DRAWBRIDGE_DOWN ||
			   (is_drawbridge_wall(u.ux, u.uy) >= 0)) ||
			(boulder_at(u.ux, u.uy)) ||
			(IS_GRAVE(lev->typ)) ||
			(IS_SEAL(lev->typ)) ||
			(lev->typ == DRAWBRIDGE_UP) ||
			(IS_THRONE(lev->typ)) ||
			(IS_ALTAR(lev->typ)) ||
			(Is_airlevel(&u.uz))
		)){
			typ = fillholetyp(u.ux,u.uy);
			You("gyre and gimble into the %s.", surface(u.ux,u.uy));
			if (typ != ROOM) {
				lev->typ = typ;
				if (ttmp) {
					if(delfloortrap(ttmp)) ttmp = (struct trap *)0;
				}
				/* if any objects were frozen here, they're released now */
				unearth_objs(u.ux, u.uy);

					if(cansee(u.ux, u.uy))
						pline_The("hole fills with %s!",
						  typ == LAVAPOOL ? "lava" : "water");
				if (!Levitation && !Flying) {
					if (typ == LAVAPOOL)
					(void) lava_effects(TRUE);
					else if (!Wwalking)
					(void) drown();
				}
			}
			if (nohole || !ttmp || (ttmp->ttyp != PIT && ttmp->ttyp != SPIKED_PIT && ttmp->ttyp != TRAPDOOR))
				digactualhole(u.ux, u.uy, &youmonst, PIT, FALSE, TRUE);
			else
				digactualhole(u.ux, u.uy, &youmonst, HOLE, FALSE, TRUE);
			return MOVE_STANDARD;
		} else {
			You("gyre and gimble, but the %s is too hard!", surface(u.ux,u.uy));
			return MOVE_STANDARD;
		}
	}
	break;
	case MATTK_REACH: return use_reach_attack();
	break;
	}
	return MOVE_CANCELLED;
}

/* #mount command - order mount to attack */
STATIC_PTR int
domountattk()
{
#ifdef STEED	
	struct monst *mtmp;
	int new_x,new_y;
	if(!u.usteed){
		You("don't have a mount.");
		return MOVE_CANCELLED;
	}
	
	if(P_SKILL(P_RIDING) < P_EXPERT){
		pline("Only an expert is skilled enough to direct a mount's attacks.");
		return MOVE_CANCELLED;
	}
	
	if(!getdir("Attack in what direction?")){
		pline("never mind");
		return MOVE_CANCELLED;
	}
	
	for(new_x = u.ux+u.dx, new_y = u.uy+u.dy; isok(new_x,new_y); new_x += u.dx, new_y += u.dy){
		mtmp = m_at(new_x, new_y);
		if(mtmp && mon_can_see_mon(u.usteed, mtmp) && canspotmon(mtmp)){
			You("direct your mount to attack %s", mon_nam(mtmp));
			mattackm(u.usteed, mtmp);
			return MOVE_STANDARD;
		}
	}
	pline("Your mount can't find anything to attack!");
	return MOVE_CANCELLED;
#else
	pline("You can't ride anything!");
#endif
}

STATIC_OVL int
use_reach_attack()
{
	int typ, max_range = 4, min_range = 1;
	coord cc;
	struct monst *mtmp;


	/* Are you allowed to use a reach attack? */
	if (u.uswallow) {
	    pline("There's not enough room here to use that.");
	    return MOVE_CANCELLED;
	}
	/* Prompt for a location */
	pline("Where do you want to hit?");
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, TRUE, "the spot to hit") < 0)
	    return MOVE_CANCELLED;	/* user pressed ESC */

	/* Calculate range */
	typ = P_BARE_HANDED_COMBAT;
	if (typ == P_NONE || P_SKILL(typ) <= P_BASIC) max_range = 4;
	else if ( P_SKILL(typ) == P_SKILLED) max_range = 5;
	else if ( P_SKILL(typ) == P_EXPERT) max_range = 8;
	else if ( P_SKILL(typ) == P_MASTER) max_range = 9;
	else max_range = 10;
	if (distu(cc.x, cc.y) > max_range) {
	    pline("Too far!");
	    return MOVE_CANCELLED;
	} else if (distu(cc.x, cc.y) < min_range) {
	    pline("Too close!");
	    return MOVE_CANCELLED;
	} else if (!cansee(cc.x, cc.y) &&
		   ((mtmp = m_at(cc.x, cc.y)) == (struct monst *)0 ||
		    !canseemon(mtmp))) {
	    You("won't hit anything if you can't see that spot.");
	    return MOVE_CANCELLED;
	} else if (!couldsee(cc.x, cc.y)) { /* Eye of the Overworld */
	    You("can't reach that spot from here.");
	    return MOVE_CANCELLED;
	}

	/* Attack the monster there */
	if ((mtmp = m_at(cc.x, cc.y)) != (struct monst *)0) {
		int tmp, tmpw, tmpt;

	    bhitpos = cc;
	    check_caitiff(mtmp);
		(void)xattacky(&youmonst, mtmp, cc.x, cc.y);
	} else
	    /* Now you know that nothing is there... */
	    pline("%s", nothing_happens);
	return MOVE_STANDARD;
}
int
doMysticForm()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	Sprintf(buf,	"Known Techniques");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	if(monk_style_active(DIVE_KICK)) {
		Sprintf(buf,	"Dive Kick (active)");
	} else {
		Sprintf(buf,	"Dive Kick (disabled)");
	}
	any.a_int = DIVE_KICK;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	if(monk_style_active(AURA_BOLT)) {
		Sprintf(buf,	"Aura Bolt (active)");
	} else {
		Sprintf(buf,	"Aura Bolt (disabled)");
	}
	any.a_int = AURA_BOLT;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	if(monk_style_active(BIRD_KICK)) {
		Sprintf(buf,	"Bird Kick (active)");
	} else {
		Sprintf(buf,	"Bird Kick (disabled)");
	}
	any.a_int = BIRD_KICK;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	if(monk_style_active(METODRIVE)) {
		Sprintf(buf,	"Meteor Drive (active)");
	} else {
		Sprintf(buf,	"Meteor Drive (disabled)");
	}
	any.a_int = METODRIVE;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	if(monk_style_active(PUMMEL)) {
		Sprintf(buf,	"Pummel (active)");
	} else {
		Sprintf(buf,	"Pummel (disabled)");
	}
	any.a_int = PUMMEL;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	end_menu(tmpwin,	"Toggle mystic techniques:");
	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);

	if(n <= 0){
		free(selected);
		return MOVE_CANCELLED;
	} else {
		toggle_monk_style(selected[0].item.a_int);
		free(selected);
		return MOVE_INSTANT;
	}

}

int
doLightsaberForm()
{
	winid tmpwin;
	int n, how, i;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;
	boolean remotely_competent = FALSE;
	int curskill;

	for (i = FIRST_LS_FFORM; i <= LAST_LS_FFORM; i++) {
		if (FightingFormSkillLevel(i) >= P_BASIC)
			remotely_competent = TRUE;
	}

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf,	"Known Lightsaber Forms");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);

	if (remotely_competent){
		for (i = FIRST_LS_FFORM; i <= LAST_LS_FFORM; i++) {
			curskill = FightingFormSkillLevel(i);
			if (curskill >= P_BASIC) {
				boolean active = selectedFightingForm(i);
				boolean blocked = blockedFightingForm(i);

				Strcpy(buf, nameOfFightingForm(i));
				Strcat(buf, " (");
				Strcat(buf, (curskill >= P_EXPERT) ? "expert" : ((curskill >= P_SKILLED) ? "skilled" : "basic"));
				if (active && blocked)
					Strcat(buf, ", selected; blocked by armor");
				else if (active)
					Strcat(buf, ", active");
				else if (blocked)
					Strcat(buf, ", blocked by armor");
				Strcat(buf, ")");

				any.a_int = i;	/* must be non-zero */
				add_menu(tmpwin, NO_GLYPH, &any,
					incntlet, 0, ATR_NONE, buf,
					MENU_UNSELECTED);
				incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
			}
		}
	} else {
		Strcpy(buf, nameOfFightingForm(FFORM_SHII_CHO));
		Strcat(buf, " (unskilled, active due to lack of alternatives)");
		any.a_int = FFORM_SHII_CHO;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	end_menu(tmpwin,	"Choose preferred lightsaber form:");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);

	if(n <= 0 || selectedFightingForm(selected[0].item.a_int)){
		if(n>0) free(selected);
		return MOVE_CANCELLED;
	} else {
		setFightingForm(selected[0].item.a_int);
		free(selected);
		return MOVE_INSTANT;
	}
}

int doEldritchKniForm()
{
	winid tmpwin;
	int n, how, i, j, damagetype, success_odds, spell_id;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;
	boolean remotely_competent = FALSE;
	int curskill = FightingFormSkillLevel(FFORM_KNI_ELDRITCH);;
	int spell_list[] = {0, SPE_FIREBALL, SPE_FIRE_STORM, SPE_CONE_OF_COLD, SPE_BLIZZARD,
		SPE_LIGHTNING_BOLT, SPE_LIGHTNING_STORM, SPE_ACID_SPLASH, SPE_POISON_SPRAY, SPE_FINGER_OF_DEATH, 0};

	for (i = 1; spell_list[i]; i++)
		for (j = 0; j < MAXSPELL; j++)
			if (spellid(j) == spell_list[i] && spellknow(j) > 0)
				remotely_competent = TRUE;

	if (!remotely_competent){
		pline("You don't know any appropriate spells!");
		return MOVE_CANCELLED;
	} else {
		setFightingForm(FFORM_KNI_ELDRITCH);
	}

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Known Eldritch Styles");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);

	for (i = 1; spell_list[i]; i++) {
		for (spell_id = 0; spell_id < MAXSPELL; spell_id++)
			if (spellid(spell_id) == spell_list[i])
				break;

		if (spell_id >= MAXSPELL || spellknow(spell_id) <= 0)
			continue;

		success_odds = percent_success(spell_id);

		Strcpy(buf, spellname(spell_id));
		Strcat(buf, " (");
		if (success_odds <= 0)
			Strcat(buf, "impossible");
		else if (success_odds < 25)
			Strcat(buf, "very difficult");
		else if (success_odds < 50)
			Strcat(buf, "difficult");
		else if (success_odds < 75)
			Strcat(buf, "moderate");
		else if (success_odds < 95)
			Strcat(buf, "easy");
		else
			Strcat(buf, "trivial");

		if (u.ueldritch_style == spell_list[i])
			Strcat(buf, ", active");
		Strcat(buf, ")");

		any.a_int = i;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}

	end_menu(tmpwin, "Choose preferred eldritch style:");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);

	if(n <= 0 ){
		return MOVE_CANCELLED;
	} else {
		if (spell_list[selected[0].item.a_int] == u.ueldritch_style){
			free(selected);
			return MOVE_CANCELLED;
		} else {
			u.ueldritch_style = spell_list[selected[0].item.a_int];
			free(selected);
			return MOVE_INSTANT;
		}
	}
	return MOVE_CANCELLED;
}

int
doKnightForm()
{
	winid tmpwin;
	int n, how, i;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;
	boolean remotely_competent = FALSE;
	int curskill;
	char* block_reason;

	for (i = FIRST_BASIC_KNI_FFORM; i <= LAST_ADV_KNI_FFORM; i++) {
		if (i > LAST_BASIC_KNI_FFORM && i < FIRST_ADV_KNI_FFORM) continue;
		if (FightingFormSkillLevel(i) >= P_BASIC)
			remotely_competent = TRUE;
	}
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Known Knightly Styles");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);


	for (i = FIRST_BASIC_KNI_FFORM; i <= LAST_ADV_KNI_FFORM; i++) {
		if (i > LAST_BASIC_KNI_FFORM && i < FIRST_ADV_KNI_FFORM) continue;

		curskill = FightingFormSkillLevel(i);
		if (curskill == P_ISRESTRICTED)
			continue;

		/* knight forms are shown if unskilled but not restricted, since training involves starting from unskilled */
		boolean active = selectedFightingForm(i);
		boolean blocked = blockedFightingForm(i);

		Strcpy(buf, nameOfFightingForm(i));
		Strcat(buf, " (");
		if (fake_skill(getFightingFormSkill(i)))
			Strcat(buf, "trained");
		else if (curskill == P_UNSKILLED)
			Strcat(buf, "unskilled");
		else if (curskill == P_BASIC)
			Strcat(buf, "basic");
		else if (curskill == P_SKILLED)
			Strcat(buf, "skilled");
		else if (curskill == P_EXPERT)
			Strcat(buf, "expert");

		if (i == FFORM_SHIELD_BASH)
			block_reason = "lack of a shield";
		else if (i == FFORM_KNI_RUNIC || i == FFORM_POMMEL)
			block_reason = "lack of a longsword";
		else if (i == FFORM_HALF_SWORD){
			if (!uwep || uwep->otyp != LONG_SWORD) block_reason = "lack of a longsword";
			else block_reason = "lack of a free hand";
		}
		else if (i == FFORM_GREAT_WEP)
			block_reason = "lack of a two-handed weapon";
		else if (i == FFORM_KNI_ELDRITCH)
			block_reason = "your metallic armor";

		if (active && blocked){
			Strcat(buf, ", selected; blocked by ");
			Strcat(buf, block_reason);
		}
		else if (active)
			Strcat(buf, ", active");
		else if (blocked){
			Strcat(buf, ", blocked by ");
			Strcat(buf, block_reason);
		}
		Strcat(buf, ")");

		any.a_int = i;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	end_menu(tmpwin, "Choose preferred fighting style:");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);

	if(n <= 0){
		return MOVE_CANCELLED;
	} else if ((selected[0].item.a_int == FFORM_HALF_SWORD || activeFightingForm(FFORM_HALF_SWORD)) &&
				(!freehand() || (uwep && uwep->otyp == LONG_SWORD && welded(uwep)))){
		free(selected);
		pline("You need a free hand to adjust your grip!");
		return MOVE_CANCELLED;
	} else if (selected[0].item.a_int == FFORM_KNI_ELDRITCH){
		free(selected);
		return doEldritchKniForm();
	} else if (selectedFightingForm(selected[0].item.a_int)) {
		unSetFightingForm(selected[0].item.a_int);
		free(selected);
		return MOVE_INSTANT;
	} else {
		setFightingForm(selected[0].item.a_int);
		free(selected);
		return MOVE_INSTANT;
	}
}

int
doGithForm()
{
	winid tmpwin;
	int n, how, i;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;
	int curskill;
	char* block_reason;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "Known Mental Edges");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);


	for (i = FIRST_GSTYLE; i <= LAST_GSTYLE; i++) {
		if (i == GSTYLE_RESONANT && (u.ulevel < 30 || u.uinsight < 81) && (artinstance[ART_SILVER_SKY].GithStylesSeen & 2) == 0)
			continue;
		if (i == GSTYLE_COLD && u.uinsight < 9 && (artinstance[ART_SILVER_SKY].GithStylesSeen & 1) == 0)
			continue;

		/* knight forms are shown if unskilled but not restricted, since training involves starting from unskilled */
		boolean active = (artinstance[ART_SILVER_SKY].GithStyle & (1 << i)) != 0;
		boolean blocked = blockedMentalEdge(i);

		Strcpy(buf, nameOfMentalEdge(i));
		Strcat(buf, " (");
		if (u.ulevel < 14)
			block_reason = "lack of skill";
		else if (i == GSTYLE_PENETRATE)
			block_reason = "lack of hate";
		else if (i == GSTYLE_COLD)
			block_reason = (u.uinsight < 9) ? "lack of knowledge" : "lack of wrath";
		else if (i == GSTYLE_RESONANT)
			block_reason = (u.ulevel < 30) ? "lack of skill" : ((u.uinsight < 81) ? "lack of knowledge" : "lack of mental discipline");
		else
			block_reason = "lack of mental discipline";

		if (active && blocked){
			Strcat(buf, "selected; blocked by ");
			Strcat(buf, block_reason);
		}
		else if (active)
			Strcat(buf, "active");
		else if (blocked){
			Strcat(buf, "blocked by ");
			Strcat(buf, block_reason);
		}
		else
			Strcat(buf, "ready");
		Strcat(buf, ")");

		any.a_int = i;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	end_menu(tmpwin, "Choose preferred mental edge:");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);

	if(n <= 0){
		return MOVE_CANCELLED;
	} else if ((artinstance[ART_SILVER_SKY].GithStyle&(1 << selected[0].item.a_int)) != 0) {
		artinstance[ART_SILVER_SKY].GithStyle &= ~(1 << selected[0].item.a_int);
		free(selected);
		return MOVE_INSTANT;
	} else {
		if (selected[0].item.a_int == GSTYLE_COLD || selected[0].item.a_int == GSTYLE_PENETRATE)
			artinstance[ART_SILVER_SKY].GithStyle &= ~((1 << GSTYLE_PENETRATE) | (1 << GSTYLE_COLD));
		else
			artinstance[ART_SILVER_SKY].GithStyle &= ~((1 << GSTYLE_DEFENSE) | (1 << GSTYLE_ANTIMAGIC) | (1 << GSTYLE_RESONANT));

		artinstance[ART_SILVER_SKY].GithStyle |= (1 << selected[0].item.a_int);

		if (selected[0].item.a_int == GSTYLE_RESONANT) artinstance[ART_SILVER_SKY].GithStylesSeen |= 2;
		if (selected[0].item.a_int == GSTYLE_COLD) artinstance[ART_SILVER_SKY].GithStylesSeen |= 1;

		free(selected);
		return MOVE_INSTANT;
	}
}

#define MONK_FORMS			0x001L
#define LIGHTSABER_FORMS	0x002L
#define KNIGHT_FORMS		0x004L
#define AVOID_PASSIVES		0x008L
#define AVOID_MSPLCAST		0x010L
#define AVOID_GRABATTK		0x020L
#define AVOID_ENGLATTK		0x040L
#define AVOID_UNSAFETOUCH	0x080L
#define GITH_FORMS			0x100L


int
hasfightingforms(){
	/* lotsa shit to go "yo do we have a starblade" */
	/* copied wholesale from the same usage in mondata.c*/
	struct attack *attk;
	struct attack prev_attk = {0};
	struct attack prev_attk2 = {0};
	int	indexnum, subout[SUBOUT_ARRAY_SIZE]={0}, tohitmod, res[4];
	indexnum = tohitmod = 0;
	int i;

	/* always shown */
	int formmask = AVOID_UNSAFETOUCH;

	/* forms relevant due to situation/role are shown, even if you're bad at them (if applicable) */
	if(Role_if(PM_MONK))
		formmask |= MONK_FORMS;
	if(Role_if(PM_KNIGHT))
		formmask |= KNIGHT_FORMS;
	if((uwep && is_lightsaber(uwep)) || (uswapwep && is_lightsaber(uswapwep)))
		formmask |= LIGHTSABER_FORMS;
	if (u.uavoid_passives)
		formmask |= AVOID_PASSIVES;
	else {
		indexnum = tohitmod = 0;
		zero_subout(subout);
		res[0] = MM_MISS;
		res[1] = MM_MISS;
		res[2] = MM_MISS;
		res[3] = MM_MISS;
		for(attk = getattk(&youmonst, (struct monst *) 0, res, &indexnum, &prev_attk, FALSE, subout, &tohitmod);
			!is_null_attk(attk);
			attk = getattk(&youmonst, (struct monst *) 0, res, &indexnum, &prev_attk, FALSE, subout, &tohitmod)
		){
			if(no_contact_attk(attk)) formmask |= AVOID_PASSIVES;
		}
	}
	if (u.uavoid_msplcast)
		formmask |= AVOID_MSPLCAST;
	else {
		indexnum = tohitmod = 0;
		zero_subout(subout);
		res[0] = MM_MISS;
		res[1] = MM_MISS;
		res[2] = MM_MISS;
		res[3] = MM_MISS;
		for(attk = getattk(&youmonst, (struct monst *) 0, res, &indexnum, &prev_attk2, FALSE, subout, &tohitmod);
			!is_null_attk(attk);
			attk = getattk(&youmonst, (struct monst *) 0, res, &indexnum, &prev_attk2, FALSE, subout, &tohitmod)
		){
			if(attk->aatyp == AT_MAGC) formmask |= AVOID_MSPLCAST;
		}
	}
	if (u.uavoid_grabattk || sticks(&youmonst))
		formmask |= AVOID_GRABATTK;

	if (u.uavoid_englattk)
		formmask |= AVOID_ENGLATTK;
	else {
		indexnum = tohitmod = 0;
		zero_subout(subout);
		res[0] = MM_MISS;
		res[1] = MM_MISS;
		res[2] = MM_MISS;
		res[3] = MM_MISS;
		for(attk = getattk(&youmonst, (struct monst *) 0, res, &indexnum, &prev_attk2, FALSE, subout, &tohitmod);
			!is_null_attk(attk);
			attk = getattk(&youmonst, (struct monst *) 0, res, &indexnum, &prev_attk2, FALSE, subout, &tohitmod)
		){
			if(attk->aatyp == AT_ENGL) formmask |= AVOID_ENGLATTK;
		}
	}
	
	if(u.ulevel >= 14){
		if(uwep && uwep->oartifact){
			const struct artifact *weap = get_artifact(uwep);
			if(weap->inv_prop == GITH_ART || weap->inv_prop == AMALGUM_ART)
				formmask |= GITH_FORMS;
		}
		if(uswapwep && uswapwep->oartifact){
			const struct artifact *weap = get_artifact(uswapwep);
			if(weap->inv_prop == GITH_ART || weap->inv_prop == AMALGUM_ART)
				formmask |= GITH_FORMS;
		}
	}

	/* next, forms trained are shown, even if inapplicable at the moment*/
	for (i = FIRST_LS_FFORM; i <= LAST_LS_FFORM; i++) {
		if (FightingFormSkillLevel(i) >= P_BASIC)
			formmask |= LIGHTSABER_FORMS;
	}

	for (i = FIRST_BASIC_KNI_FFORM; i <= LAST_ADV_KNI_FFORM; i++) {
		if (i > LAST_BASIC_KNI_FFORM && i < FIRST_ADV_KNI_FFORM) continue;
		if (FightingFormSkillLevel(i) >= P_BASIC)
			formmask |= KNIGHT_FORMS;
	}
	
	return formmask;
}


int
dofightingform()
{
	winid tmpwin;
	int n, how, i;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	/* forms relevant due to situation/role are shown, even if you're bad at them (if applicable) */
	int formmask = hasfightingforms();

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	if (formmask & MONK_FORMS) {
		any.a_int = 1;
		add_menu(tmpwin, NO_GLYPH, &any, 'm', 0, ATR_NONE, "Select Mystic Forms", MENU_UNSELECTED);
	}
	if (formmask & LIGHTSABER_FORMS) {
		any.a_int = 2;
		add_menu(tmpwin, NO_GLYPH, &any, 'l', 0, ATR_NONE, "Select Lightsaber Forms", MENU_UNSELECTED);
	}
	if (formmask & KNIGHT_FORMS) {
		any.a_int = 3;
		add_menu(tmpwin, NO_GLYPH, &any, 'k', 0, ATR_NONE, "Select Knightly Forms", MENU_UNSELECTED);
	}
	if (formmask & GITH_FORMS) {
		any.a_int = 9;
		add_menu(tmpwin, NO_GLYPH, &any, 'h', 0, ATR_NONE, "Select Mental Edge", MENU_UNSELECTED);
	}
	if (formmask & AVOID_PASSIVES) {
		any.a_int = 4;
		if (!u.uavoid_passives) Strcpy(buf, "Only make passive-safe attacks");
		else Strcpy(buf, "Allow passive-unsafe attacks");

		add_menu(tmpwin, NO_GLYPH, &any, 'p', 0, ATR_NONE, buf, MENU_UNSELECTED);
	}
	if (formmask & AVOID_MSPLCAST) {
		any.a_int = 5;
		if (!u.uavoid_msplcast) Strcpy(buf, "Avoid automatically casting spells when attacking");
		else Strcpy(buf, "Allow the automatic casting of spells when attacking");

		add_menu(tmpwin, NO_GLYPH, &any, 's', 0, ATR_NONE, buf, MENU_UNSELECTED);
	}
	if (formmask & AVOID_GRABATTK) {
		any.a_int = 6;
		if (!u.uavoid_grabattk) Strcpy(buf, "Avoid grabbing monsters when attacking");
		else Strcpy(buf, "Allow grabbing monsters when attacking");

		add_menu(tmpwin, NO_GLYPH, &any, 'g', 0, ATR_NONE, buf, MENU_UNSELECTED);
	}
	if (formmask & AVOID_ENGLATTK) {
		any.a_int = 7;
		if (!u.uavoid_englattk) Strcpy(buf, "Avoid engulfing monsters when attacking");
		else Strcpy(buf, "Allow engulfing monsters when attacking");

		add_menu(tmpwin, NO_GLYPH, &any, 'e', 0, ATR_NONE, buf, MENU_UNSELECTED);
	}
	if (formmask & AVOID_UNSAFETOUCH) {
		any.a_int = 8;
		if (!u.uavoid_unsafetouch) Strcpy(buf, "Avoid directly touching potentially unsafe monsters");
		else Strcpy(buf, "Allow directly touching potentally unsafe monsters");

		add_menu(tmpwin, NO_GLYPH, &any, 't', 0, ATR_NONE, buf, MENU_UNSELECTED);
	}

	end_menu(tmpwin, "Adjust fighting styles:");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);

	if(n <= 0){
		return MOVE_CANCELLED;
	} else {
		n = selected[0].item.a_int;
		free(selected);
	}

	switch (n){
		case 1:
			return doMysticForm();
		case 2:
			return doLightsaberForm();
		case 3:
			return doKnightForm();
		case 4:
			u.uavoid_passives = !u.uavoid_passives;
			return MOVE_INSTANT;
		case 5:
			u.uavoid_msplcast = !u.uavoid_msplcast;
			return MOVE_INSTANT;
		case 6:
			u.uavoid_grabattk = !u.uavoid_grabattk;
			return MOVE_INSTANT;
		case 7:
			u.uavoid_englattk = !u.uavoid_englattk;
			return MOVE_INSTANT;
		case 8:
			u.uavoid_unsafetouch = !u.uavoid_unsafetouch;
			return MOVE_INSTANT;
		case 9:
			return doGithForm();
		default:
			impossible("unknown fighting form set %d", n);
			return MOVE_CANCELLED;
	}
	return MOVE_CANCELLED;
}

#undef MONK_FORMS
#undef LIGHTSABER_FORMS
#undef KNIGHT_FORMS
#undef AVOID_PASSIVES
#undef AVOID_MSPLCAST
#undef AVOID_GRABATTK
#undef AVOID_ENGLATTK
#undef AVOID_UNSAFETOUCH


int
dounmaintain()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	int spell;
	menu_item *selected;
	anything any;
	
	if(!(u.spells_maintained)){
		pline("You aren't maintaining any spells.");
		return MOVE_CANCELLED;
	}
	
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	
	Sprintf(buf,	"Spells Maintained");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	for (spell = SPE_DIG; spell != SPE_BLANK_PAPER; spell++) {
		if (!spell_maintained(spell))
			continue;
		
		Sprintf(buf,	"%s", OBJ_NAME(objects[spell]));
		any.a_int = spell;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	end_menu(tmpwin,	"Choose spell to stop maintaining:");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	
	if(n <= 0){
		return MOVE_CANCELLED;
	} else {
		pline("You cease to maintain %s.",
			  OBJ_NAME(objects[selected[0].item.a_int]));
		spell_unmaintain(selected[0].item.a_int);
		free(selected);
		return MOVE_INSTANT;
	}
}

STATIC_PTR int
enter_explore_mode()
{
	char buf[BUFSZ];
	pline("Explore mode is for local games, not public servers.");
	return MOVE_CANCELLED;

	if(!discover && !wizard) {
		pline("Beware!  From explore mode there will be no return to normal game.");
		if (yesno("Do you want to enter explore mode?", iflags.paranoid_quit) == 'y') {
			clear_nhwindow(WIN_MESSAGE);
			You("are now in non-scoring explore mode.");
			discover = TRUE;
		}
		else {
			clear_nhwindow(WIN_MESSAGE);
			pline("Resuming normal game.");
		}
	}
	return MOVE_CANCELLED;
}

STATIC_PTR int
dooverview_or_wiz_where()
{
#ifdef WIZARD
	if (wizard) return wiz_where();
	else
#endif
	dooverview();
	return MOVE_CANCELLED;
}

STATIC_PTR int
doclearinvissyms()
{
	register int x, y;
	for (x = 0; x < COLNO; x++)
	for (y = 0; y < ROWNO; y++)
	if (glyph_is_invisible(levl[x][y].glyph)) {
		unmap_object(x, y);
		newsym(x, y);
	}
	return MOVE_CANCELLED;
}

#ifdef WIZARD

STATIC_PTR int
wiz_bind()
{
	if (wizard) {
		int tmp;
		u.sealsKnown = ~0;
		u.specialSealsKnown = ~0;
		tmp = pick_seal("Bind spirit:");
		if (tmp)
			bindspirit(tmp);
	}
	else
		pline("Unavailable command.");
	return MOVE_CANCELLED;
}


STATIC_PTR int
wiz_mutate()
{
	if (wizard) {
		winid tmpwin;
		int n, how;
		char buf[BUFSZ];
		menu_item *selected;
		char inclet = 'a';
		anything any;

		tmpwin = create_nhwindow(NHW_MENU);
		start_menu(tmpwin);
		any.a_void = 0;		/* zero out all bits */

		Sprintf(buf, "Pick mutation:");
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
		*buf = '\0';
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
		
		int mut;
		int i;
		extern const int shubbie_mutation_list[];
		extern const struct mutationtype mutationtypes[];
		for (i = 0; mutationtypes[i].mutation; i++){
			any.a_int = mutationtypes[i].mutation;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, mutationtypes[i].name,
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;
		}
		end_menu(tmpwin, "You prepare to wizard your body....");

		how = PICK_ONE;
		n = select_menu(tmpwin, how, &selected);
		destroy_nhwindow(tmpwin);
		int picked;
		if(n > 0){
			picked = selected[0].item.a_int;
			free(selected);
		}
		else return MOVE_CANCELLED;
		
		confer_mutation(picked);
		return MOVE_CANCELLED;
	}
	else
		pline("Unavailable command.");
	return MOVE_CANCELLED;
}

#define GAIN_SHUB_ATTEN	1
#define GAIN_FLAM_ATTEN	2
#define GAIN_YOG_ATTEN	3
#define SET_SHUB_FAVOR	4
#define SET_FLAM_FAVOR	5
#define SET_YOG_FAVOR	6
#define SET_SHUB_MUT	7
#define SET_YOG_MUT		8
#define SET_SHUB_DEVOTION	9
#define SET_FLAM_DEVOTION	10
#define SET_YOG_DEVOTION	11

STATIC_PTR int
wiz_cult()
{
	if (wizard) {
		winid tmpwin;
		int n, how;
		char buf[BUFSZ];
		menu_item *selected;
		char inclet = 'a';
		anything any;

		tmpwin = create_nhwindow(NHW_MENU);
		start_menu(tmpwin);
		any.a_void = 0;		/* zero out all bits */
		
		if(!u.shubbie_atten){
			any.a_int = GAIN_SHUB_ATTEN;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, "Activate Shub-Nugganoth cult.",
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;
		}
		else {
			any.a_int = SET_SHUB_FAVOR;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, "Set Shub-Nugganoth favor.",
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;

			any.a_int = SET_SHUB_DEVOTION;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, "Set Shub-Nugganoth devotion.",
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;

			any.a_int = SET_SHUB_MUT;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, "Set Shub-Nugganoth mutagen.",
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;
		}
		if(!u.silver_atten){
			any.a_int = GAIN_FLAM_ATTEN;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, "Activate Silver Flame cult.",
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;
		}
		else {
			any.a_int = SET_FLAM_FAVOR;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, "Set Silver Flame favor.",
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;

			any.a_int = SET_FLAM_DEVOTION;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, "Set Silver Flame devotion.",
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;
		}
		if(!u.yog_sothoth_atten){
			any.a_int = GAIN_YOG_ATTEN;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, "Activate Yog-Sothoth cult.",
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;
		}
		else {
			any.a_int = SET_YOG_FAVOR;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, "Set Yog-Sothoth favor.",
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;

			any.a_int = SET_YOG_DEVOTION;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, "Set Yog-Sothoth devotion.",
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;

			any.a_int = SET_YOG_MUT;
			add_menu(tmpwin, NO_GLYPH, &any,
				inclet, 0, ATR_NONE, "Set Yog-Sothoth mutagen.",
				MENU_UNSELECTED);
			if(inclet == 'z')
				inclet = 'A';
			else
				inclet++;
		}

		end_menu(tmpwin, "Select one:");

		how = PICK_ONE;
		n = select_menu(tmpwin, how, &selected);
		destroy_nhwindow(tmpwin);
		int picked;
		if(n > 0){
			picked = selected[0].item.a_int;
			free(selected);
		}
		else return MOVE_CANCELLED;
		struct obj *icon;
		int newfavor;
#define set_value(str, val)					newfavor = getvalue(str);\
				if(newfavor < 0){\
					pline1(Never_mind);\
				}\
				else {\
					val = newfavor;\
				}


		switch(picked){
			case GAIN_SHUB_ATTEN:
				u.shubbie_atten = TRUE;
				icon = mksobj(HOLY_SYMBOL_OF_THE_BLACK_MOTHE, MKOBJ_NOINIT);
				icon = hold_another_object(icon, "You drop %s!",
							  doname(icon), (const char *)0); /*shouldn't merge, but may drop*/
			break;
			case GAIN_FLAM_ATTEN:
				u.silver_atten = TRUE;
				icon = mksobj(PURIFIED_MIRROR, MKOBJ_NOINIT);
				icon = hold_another_object(icon, "You drop %s!",
							  doname(icon), (const char *)0); /*shouldn't merge, but may drop*/
			break;
			case GAIN_YOG_ATTEN:
				u.yog_sothoth_atten = TRUE;
				pline("A seal is engraved into your mind!");
				u.specialSealsKnown |= SEAL_YOG_SOTHOTH;
			break;
			case SET_SHUB_FAVOR:
				set_value("Set Shub-Nuganoth favor to what?", u.shubbie_credit);
			break;
			case SET_SHUB_DEVOTION:
				set_value("Set Shub-Nuganoth devotion to what?", u.shubbie_devotion);
			break;
			case SET_SHUB_MUT:
				set_value("Set Shub-Nuganoth mutagen to what?", u.shubbie_mutagen);
			break;
			case SET_FLAM_FAVOR:
				set_value("Set Silver Flame favor to what?", u.silver_credit);
			break;
			case SET_FLAM_DEVOTION:
				set_value("Set Silver Flame devotion to what?", u.silver_devotion);
			break;
			case SET_YOG_FAVOR:
				set_value("Set Yog-Sothoth favor to what?", u.yog_sothoth_credit);
			break;
			case SET_YOG_DEVOTION:
				set_value("Set Yog-Sothoth devotion to what?", u.yog_sothoth_devotion);
			break;
			case SET_YOG_MUT:
				set_value("Set Yog-Sothoth mutagen to what?", u.yog_sothoth_mutagen);
			break;
		}
		return MOVE_CANCELLED;
	}
	else
		pline("Unavailable command.");
	return MOVE_CANCELLED;
}


STATIC_PTR int
wiz_mk_mapglyphdump()
{
#ifdef MAPDUMP_FN
    mk_mapdump(MAPDUMP_FN);
#endif
    return MOVE_CANCELLED;
}

/* ^W command - wish for something */
STATIC_PTR int
wiz_wish()	/* Unlimited wishes for debug mode by Paul Polderman */
{
	if (wizard) {
	    boolean save_verbose = flags.verbose;

	    flags.verbose = FALSE;
		makewish(WISH_WIZARD);
	    flags.verbose = save_verbose;
	    (void) encumber_msg();
	} else
	    pline("Unavailable command '^W'.");
	return MOVE_CANCELLED;
}

/* ^I command - identify hero's inventory */
STATIC_PTR int
wiz_identify()
{
	if (wizard)	identify_pack(0);
	else		pline("Unavailable command '^I'.");
	return MOVE_CANCELLED;
}


/* #wizmakemap - discard current dungeon level and replace with a new one */
int
wiz_makemap(VOID_ARGS)
{
    if (wizard) {
	struct monst *mtmp;

	rm_mapseen(ledger_no(&u.uz));

	for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
	    int ndx = monsndx(mtmp->data);
	    if (mtmp->isgd) {
			mtmp->isgd = 0;
			mongone(mtmp);
	    }
		if (mtmp->data->geno & G_UNIQ)
			mvitals[ndx].mvflags &= ~(G_EXTINCT);
		if (mvitals[ndx].born)
			mvitals[ndx].born--;
	    if (DEADMONSTER(mtmp))
		continue;
	    if (mtmp->isshk)
		setpaid(mtmp);
	}
	if (Punished) {
	    ballrelease(FALSE);
	    unplacebc();
	}
	if (is_box_picking_context())
	    reset_pick();

	if (on_level(&digging.level, &u.uz))
	    (void) memset((genericptr_t) &digging, 0, sizeof (struct dig_info));
	iflags.travelcc.x = iflags.travelcc.y = -1;

	u.utrap = 0;
	u.utraptype = 0;

	//float_vs_flight(); /* maybe block Lev and/or Fly */

	check_special_room(TRUE);
	u.ustuck = (struct monst *) 0;
	u.uswallow = 0;
	u.uinwater = 0;
	u.uundetected = 0;
	dmonsfree();
	keepdogs(TRUE, 0, 0);

        savelev(-1, ledger_no(&u.uz), FREE_SAVE);
        /* create a new level; various things like bestowing a guardian
           angel on Astral or setting off alarm on Ft.Ludios are handled
           by goto_level(do.c) so won't occur for replacement levels */
        mklev();

        vision_reset();
        vision_full_recalc = 1;
        cls();

	rn2(2) ? u_on_upstairs() : u_on_dnstairs();
        losedogs();
        initrack();
        if (Punished) {
            unplacebc();
            placebc();
        }
        docrt();
        flush_screen(1);
        check_special_room(FALSE); /* room entry */
    }
    return MOVE_CANCELLED;
}

/* ^F command - reveal the level map and any traps on it */
STATIC_PTR int
wiz_map()
{
	if (wizard) {
	    struct trap *t;
		int zx, zy;
	    long save_Hconf = HConfusion,
		 save_Hhallu = HHallucination;

	    HConfusion = HHallucination = 0L;
		for(zx = 0; zx < COLNO; zx++) for(zy = 0; zy < ROWNO; zy++)
			if (glyph_is_trap(levl[zx][zy].glyph)) {
				/* Zonk all memory of this location. */
				levl[zx][zy].seenv = 0;
				levl[zx][zy].waslit = 0;
				levl[zx][zy].glyph = cmap_to_glyph(S_stone);
				levl[zx][zy].styp = STONE;
			}
	    for (t = ftrap; t != 0; t = t->ntrap) {
		t->tseen = 1;
		map_trap(t, TRUE);
	    }
	    do_mapping();
	    HConfusion = save_Hconf;
	    HHallucination = save_Hhallu;
	} else
	    pline("Unavailable command '^F'.");
	return MOVE_CANCELLED;
}

/* ^G command - generate monster(s); a count prefix will be honored */
STATIC_PTR int
wiz_genesis()
{
	if (wizard)	(void) create_particular(u.ux, u.uy, -1, -1, TRUE, 0, 0, 0, (char *)0);
	else		pline("Unavailable command '^G'.");
	return MOVE_CANCELLED;
}

/* ^O command - display dungeon layout */
STATIC_PTR int
wiz_where()
{
	if (wizard) (void) print_dungeon(FALSE, FALSE, (schar *)0, (int *)0);
	else	    pline("Unavailable command '^O'.");
	return MOVE_CANCELLED;
}

/* ^E command - detect unseen (secret doors, traps, hidden monsters) */
STATIC_PTR int
wiz_detect()
{
	if(wizard)  (void) findit();
	else	    pline("Unavailable command '^E'.");
	return MOVE_CANCELLED;
}

/* ^V command - level teleport */
STATIC_PTR int
wiz_level_tele()
{
	if (wizard)	level_tele();
	else		pline("Unavailable command '^V'.");
	return MOVE_CANCELLED;
}

/* #monpolycontrol command - choose new form for shapechangers, polymorphees */
STATIC_PTR int
wiz_mon_polycontrol()
{
    iflags.mon_polycontrol = !iflags.mon_polycontrol;
    pline("Monster polymorph control is %s.",
	  iflags.mon_polycontrol ? "on" : "off");
    return MOVE_CANCELLED;
}

/* #levelchange command - adjust hero's experience level */
STATIC_PTR int
wiz_level_change()
{
    char buf[BUFSZ];
    int newlevel;
    int ret;

    getlin("To what experience level do you want to be set?", buf);
    (void)mungspaces(buf);
    if (buf[0] == '\033' || buf[0] == '\0') ret = 0;
    else ret = sscanf(buf, "%d", &newlevel);

    if (ret != 1) {
	pline1(Never_mind);
	return MOVE_CANCELLED;
    }
    if (newlevel == u.ulevel) {
	You("are already that experienced.");
    } else if (newlevel < u.ulevel) {
	if (u.ulevel == 1) {
	    You("are already as inexperienced as you can get.");
	    return MOVE_CANCELLED;
	}
	if (newlevel < 1) newlevel = 1;
	while (u.ulevel > newlevel)
	    losexp("#levelchange",TRUE,TRUE,TRUE);
    } else {
	if (u.ulevel >= MAXULEV) {
	    You("are already as experienced as you can get.");
	    return MOVE_CANCELLED;
	}
	if (newlevel > MAXULEV) newlevel = MAXULEV;
	while (u.ulevel < newlevel)
	    pluslvl(FALSE);
    }
    u.ulevelmax = u.ulevel;
    return MOVE_CANCELLED;
}

/* #panic command - test program's panic handling */
STATIC_PTR int
wiz_panic()
{
	if (iflags.debug_fuzzer) {
        	u.uhp = u.uhpmax = 1000;
      		u.uen = u.uenmax = 1000;
        	return MOVE_CANCELLED;
	}

	if (yn("Do you want to call panic() and end your game?") == 'y')
		panic("crash test.");
	return MOVE_CANCELLED;
}

/* #polyself command - change hero's form */
STATIC_PTR int
wiz_polyself()
{
	polyself(TRUE);
	return MOVE_CANCELLED;
}

/* #seenv command */
STATIC_PTR int
wiz_show_seenv()
{
	winid win;
	int x, y, v, startx, stopx, curx;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	/*
	 * Each seenv description takes up 2 characters, so center
	 * the seenv display around the hero.
	 */
	startx = max(1, u.ux-(COLNO/4));
	stopx = min(startx+(COLNO/2), COLNO);
	/* can't have a line exactly 80 chars long */
	if (stopx - startx == COLNO/2) startx++;

	for (y = 0; y < ROWNO; y++) {
	    for (x = startx, curx = 0; x < stopx; x++, curx += 2) {
		if (x == u.ux && y == u.uy) {
		    row[curx] = row[curx+1] = '@';
		} else {
		    v = levl[x][y].seenv & 0xff;
		    if (v == 0)
			row[curx] = row[curx+1] = ' ';
		    else
			Sprintf(&row[curx], "%02x", v);
		}
	    }
	    /* remove trailing spaces */
	    for (x = curx-1; x >= 0; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return MOVE_CANCELLED;
}

/* #vision command */
STATIC_PTR int
wiz_show_vision()
{
	winid win;
	int x, y, v;
	char row[COLNO+1];

	win = create_nhwindow(NHW_TEXT);
	Sprintf(row, "Flags: 0x%x could see, 0x%x in sight, 0x%x temp lit",
		COULD_SEE, IN_SIGHT, TEMP_LIT);
	putstr(win, 0, row);
	putstr(win, 0, "");
	for (y = 0; y < ROWNO; y++) {
	    for (x = 1; x < COLNO; x++) {
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else {
		    v = viz_array[y][x]; /* data access should be hidden */
		    if (v == 0)
			row[x] = ' ';
		    else
			row[x] = '0' + viz_array[y][x];
		}
	    }
	    /* remove trailing spaces */
	    for (x = COLNO-1; x >= 1; x--)
		if (row[x] != ' ') break;
	    row[x+1] = '\0';

	    putstr(win, 0, &row[1]);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return MOVE_CANCELLED;
}

/* #wmode command */
STATIC_PTR int
wiz_show_wmodes()
{
	winid win;
	int x,y;
	char row[COLNO+1];
	struct rm *lev;

	win = create_nhwindow(NHW_TEXT);
	for (y = 0; y < ROWNO; y++) {
	    for (x = 0; x < COLNO; x++) {
		lev = &levl[x][y];
		if (x == u.ux && y == u.uy)
		    row[x] = '@';
		else if (IS_WALL(lev->typ) || lev->typ == SDOOR)
		    row[x] = '0' + (lev->wall_info & WM_MASK);
		else if (lev->typ == CORR)
		    row[x] = '#';
		else if (IS_ROOM(lev->typ) || IS_DOOR(lev->typ))
		    row[x] = '.';
		else
		    row[x] = 'x';
	    }
	    row[COLNO] = '\0';
	    putstr(win, 0, row);
	}
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);
	return MOVE_CANCELLED;
}

/* #showkills command */
STATIC_PTR int wiz_showkills()		/* showborn patch */
{
	list_vanquished('y', FALSE);
	return MOVE_CANCELLED;
}

STATIC_PTR int wiz_setinsight()
{
	char buf[BUFSZ];
	int newval;
	int ret;

	if (u.veil) {
		if (yn("Pierce veil?") == 'y')
			u.veil = FALSE;
		else
			return MOVE_CANCELLED;
	}
	getlin("Set your insight to what?", buf);

	(void)mungspaces(buf);
	if (buf[0] == '\033' || buf[0] == '\0') ret = 0;
	else ret = sscanf(buf, "%d", &newval);

	if (ret != 1) {
		pline1(Never_mind);
		return MOVE_CANCELLED;
	}
	change_uinsight(newval - u.uinsight);
	return MOVE_INSTANT;
}

STATIC_PTR int wiz_setsanity()
{
	char buf[BUFSZ];
	int newval;
	int ret;

	getlin("Set your sanity to what?", buf);

	(void)mungspaces(buf);
	if (buf[0] == '\033' || buf[0] == '\0') ret = 0;
	else ret = sscanf(buf, "%d", &newval);

	if (ret != 1) {
		pline1(Never_mind);
		return MOVE_CANCELLED;
	}
	change_usanity(newval - u.usanity, FALSE);
	return MOVE_INSTANT;
}

STATIC_PTR
int getvalue(str)
const char *str;
{
	char buf[BUFSZ];
	int newval;
	int ret;

	getlin(str, buf);

	(void)mungspaces(buf);
	if (buf[0] == '\033' || buf[0] == '\0') ret = 0;
	else ret = sscanf(buf, "%d", &newval);

	if (ret != 1) {
		return -1;
	}
	return newval;
}

#endif /* WIZARD */

int
do_naming(typ)
int typ;
{
    winid win;
    anything any;
    menu_item *pick_list = NULL;
    int n;
    register struct obj *obj;
    char allowall[2];
    static NEARDATA const char callable[] = {
	SCROLL_CLASS, TILE_CLASS, POTION_CLASS, WAND_CLASS, RING_CLASS, AMULET_CLASS,
	GEM_CLASS, SPBOOK_CLASS, ARMOR_CLASS, TOOL_CLASS, 0 };

    if (!typ) {
      any.a_void = 0;
      win = create_nhwindow(NHW_MENU);
      start_menu(win);

      any.a_int = 1;
      add_menu(win, NO_GLYPH, &any, 'a', 0, ATR_NONE, "Name a monster", MENU_UNSELECTED);

      any.a_int = 2;
      add_menu(win, NO_GLYPH, &any, 'b', 'y', ATR_NONE, "Name an individual item", MENU_UNSELECTED);

      any.a_int = 3;
      add_menu(win, NO_GLYPH, &any, 'c', 'n', ATR_NONE, "Name all items of a certain type", MENU_UNSELECTED);

      any.a_int = 4;
      add_menu(win, NO_GLYPH, &any, 'd', 0, ATR_NONE, "View discoveries", MENU_UNSELECTED);

      any.a_int = 5;
      add_menu(win, NO_GLYPH, &any, 'f', 0, ATR_NONE, "Call an item on the floor a certain type", MENU_UNSELECTED);

      any.a_int = 0;
      add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, "", MENU_UNSELECTED);

      end_menu(win, "What do you wish to do?");
      n = select_menu(win, PICK_ONE, &pick_list);
      destroy_nhwindow(win);

      if (pick_list) {
	n = (pick_list[0].item.a_int - 1);
	free((genericptr_t) pick_list);
      } else return MOVE_CANCELLED;
    } else {
      n = (typ - 1);
    }
    switch (n) {
    default: break;
    case 0: do_mname(); break;
	/* cases 1 & 2 duplicated from ddocall() */
    case 1:
	allowall[0] = ALL_CLASSES; allowall[1] = '\0';
	obj = getobj(allowall, "name");
	if(obj) do_oname(obj);
	break;
    case 2:
	obj = getobj(callable, "call");
	if (obj) {
	    /* behave as if examining it in inventory;
                           this might set dknown if it was picked up
                           while blind and the hero can now see */
	    (void) xname(obj);

	    if (!obj->dknown) {
		You("would never recognize another one.");
		return MOVE_CANCELLED;
	    }
	    docall(obj);
	}
	break;
    case 3: dodiscovered(); break;
    case 4: do_floorname(); break;
    }
    return MOVE_INSTANT;
}

int
do_naming_mname()
{
  if (iflags.old_C_behaviour) return do_naming(1);
  return do_naming(0);
}

int
do_naming_ddocall()
{
  return do_naming(0);
}


#endif /* OVLB */
#ifdef OVL1

/* Macros for meta and ctrl modifiers:
 *   M and C return the meta/ctrl code for the given character;
 *     e.g., (C('c') is ctrl-c
 *   ISMETA and ISCTRL return TRUE iff the code is a meta/ctrl code
 *   UNMETA and UNCTRL are the opposite of M/C and return the key for a given
 *     meta/ctrl code. */
#ifndef M
# ifndef NHSTDC
#  define M(c)		(0x80 | (c))
# else
#  define M(c)		((c) - 128)
# endif /* NHSTDC */
#endif
#define ISMETA(c) (((c) & 0x80) != 0)
#define UNMETA(c) ((c) & 0x7f)

#ifndef C
#define C(c)		(0x1f & (c))
#endif
#define ISCTRL(c) ((uchar)(c) < 0x20)
#define UNCTRL(c) (ISCTRL(c) ? (0x60 | (c)) : (c))

/* maps extended ascii codes for key presses to extended command entries in extcmdlist */
static struct key_tab cmdlist[256];

/* list built upon option loading; holds list of keys to be rebound later
 * see "crappy hack" below */
static struct binding_list_tab *bindinglist = NULL;

#define AUTOCOMPLETE TRUE
#define IFBURIED TRUE

#define EXTCMDLIST_SIZE (sizeof(extcmdlist) / sizeof(extcmdlist[1]))

struct ext_func_tab extcmdlist[] = {
	{"apply", "apply (use) a tool (pick-axe, key, lamp...)", doapply, !IFBURIED},
	{"attributes", "show your attributes (intrinsic ones included in debug or explore mode)", doattributes, IFBURIED},
	{"close", "close a door", doclose, !IFBURIED},
	{"cast", "zap (cast) a spell", docast, IFBURIED},
	{"discoveries", "show what object types have been discovered", dodiscovered, IFBURIED},
	{"down", "go down a staircase", dodown, !IFBURIED},
	{"downboy", "pets don't attack peaceful creatures", dodownboy, IFBURIED, AUTOCOMPLETE},
	{"drop", "drop an item", dodrop, !IFBURIED},
	{"dropall", "drop specific item types", doddrop, !IFBURIED},
	{"takeoffall", "remove all armor", doddoremarm, !IFBURIED},
	{"inventory", "show your inventory", ddoinv, IFBURIED},
	{"quaff", "quaff (drink) something", dodrink, !IFBURIED},
	{"#", "perform an extended command", doextcmd, IFBURIED},
	{"updatestatus", "update status lines", force_bot, IFBURIED},
	{"travel", "Travel to a specific location", dotravel, !IFBURIED},
	{"eat", "eat something", doeat, !IFBURIED},
/*	{"engrave", "engrave writing on the floor", doengward, !IFBURIED},*/
	{"engrave", "engrave writing on the floor", doengrave, !IFBURIED},
	{"ward", "engrave a ward on the floor", doward, !IFBURIED},
	{"seal", "engrave a seal on the floor", doseal, !IFBURIED},
	{"fire", "fire ammunition from quiver", dofire, !IFBURIED},
	{"power", "use an active spirit power", dospirit, IFBURIED},
	{"reorder", "re-order active spirit powers", reorder_spirit_powers, IFBURIED},
	{"history", "show long version and game history", dohistory, IFBURIED},
	{"help", "give a help message", dohelp, IFBURIED},
	{"seetrap", "show the type of a trap", doidtrap, IFBURIED},
	{"kick", "kick something", dokick, !IFBURIED},
	{"look", "look at the floor beneath you", dolook, IFBURIED},
	{"call", "call (name) a particular monster", do_naming_mname, IFBURIED},
	{"callold", "call (name) a particular monster (vanilla)", do_mname, IFBURIED},
	{"rest", "rest one move while doing nothing", donull, IFBURIED, !AUTOCOMPLETE, "waiting"},
	{"previous", "toggle through previously displayed game messages", doprev_message, IFBURIED},
	{"open", "open a door", doopen, !IFBURIED},
	{"pickup", "pick up things at the current location", dopickup, !IFBURIED},
	{"pay", "pay your shopping bill", dopay, !IFBURIED},
	{"puton", "put on an accessory (ring amulet, etc)", doputon, !IFBURIED},
	{"seeweapon", "show the weapon currently wielded", doprwep, IFBURIED},
	{"seearmor", "show the armor currently worn", doprarm, IFBURIED},
	{"seerings", "show the ring(s) currently worn", doprring, IFBURIED},
	{"seeamulet", "show the amulet currently worn", dopramulet, IFBURIED},
	{"seetools", "show the tools currently in use", doprtool, IFBURIED},
	{"seeall", "show all equipment in use (generally, ),[,=,\",( commands", doprinuse, IFBURIED},
	{"seegold", "count your gold", doprgold, IFBURIED},
	{"glance", "show what type of thing a map symbol on the level corresponds to", doquickwhatis, IFBURIED},
	{"remove", "remove an accessory (ring, amulet, etc)", doremring, !IFBURIED},
	{"read", "read a scroll or spellbook", doread, !IFBURIED},
	{"redraw", "redraw screen", doredraw, IFBURIED},
#ifdef SUSPEND
	{"suspend", "suspend game (only if defined)", dosuspend, IFBURIED},
#endif /* SUSPEND */
	{"setoptions", "show option settings, possibly change them", doset, IFBURIED},
	{"search", "search for traps and secret doors", dosearch, IFBURIED, !AUTOCOMPLETE, "searching"},
	{"save", "save the game", dosave, IFBURIED},
	{"swap", "swap wielded and secondary weapons", doswapweapon, !IFBURIED},
#ifdef SHELL
	{"shell", "do a shell escape (only if defined)", dosh, IFBURIED},
#endif
	{"throw", "throw something", dothrow, !IFBURIED},
	{"takeoff", "take off one piece of armor", dotakeoff, !IFBURIED},
	{"teleport", "teleport around level", dotele, IFBURIED},
	{"inventoryall", "inventory specific item types", dotypeinv, IFBURIED},
	{"autopickup", "toggle the pickup option on/off", dotogglepickup, IFBURIED},
	{"up", "go up a staircase", doup, !IFBURIED},
	{"version", "show version", doversion, IFBURIED},
	{"seespells", "list known spells", dovspell, IFBURIED},
	{"quiver", "select ammunition for quiver", dowieldquiver, !IFBURIED},
	{"whatis", "show what type of thing a symbol corresponds to", dowhatis, IFBURIED},
	{"whatdoes", "tell what a command does", dowhatdoes, IFBURIED},
	{"wield", "wield (put in use) a weapon", dowield, !IFBURIED},
	{"wear", "wear a piece of armor", dowear, !IFBURIED},
	{"zap", "zap a wand", dozap, !IFBURIED},
	{"explore_mode", "enter explore (discovery) mode (only if defined)", enter_explore_mode, IFBURIED},

	{"ability", "use an inherent or learned ability", doability, IFBURIED, AUTOCOMPLETE},
	{"adjust", "adjust inventory letters", doorganize, IFBURIED, AUTOCOMPLETE},
	{"annotate", "annotate current dungeon level", donamelevel, IFBURIED, AUTOCOMPLETE},
	{"attack", "order pets to battle foes", doattack, IFBURIED, AUTOCOMPLETE},
	{"chat", "talk to someone", dotalk, IFBURIED, AUTOCOMPLETE},	/* converse? */
	{"combo", "use an android combo based on your weapon", android_combo, !IFBURIED},
	{"come", "order pets to come", docome, !IFBURIED, AUTOCOMPLETE},
	{"conduct", "list which challenges you have adhered to", doconduct, IFBURIED, AUTOCOMPLETE},
	{"dip", "dip an object into something", dodip, !IFBURIED, AUTOCOMPLETE},
	{"dropit", "order pets to drop inventory", dodropall, IFBURIED, AUTOCOMPLETE},
	{"enhance", "advance or check weapons skills", enhance_weapon_skill, IFBURIED, AUTOCOMPLETE},
	{"equip", "give a pet an item", dopetequip, !IFBURIED, AUTOCOMPLETE},
	{"force", "force a lock", doforce, !IFBURIED, AUTOCOMPLETE},
	{"getem", "pets may attack peaceful creaturs", dosickem, IFBURIED, AUTOCOMPLETE},
	{"invoke", "invoke an object's powers", doinvoke, IFBURIED, AUTOCOMPLETE},
	{"jump", "jump to a location", dojump, !IFBURIED, AUTOCOMPLETE},
	{"loot", "loot a box on the floor", doloot, !IFBURIED, AUTOCOMPLETE},
	{"monster", "use a monster's special ability", domonability, IFBURIED, AUTOCOMPLETE},
	{"mount", "order mount to attack", domountattk, !IFBURIED, AUTOCOMPLETE},
	{"name", "name an item or type of object", do_naming_ddocall, IFBURIED, AUTOCOMPLETE},
	{"nameold", "name an item or type of object (vanilla)", ddocall, IFBURIED},
	{"passive", "order pets to never attack foes", dopassive, IFBURIED, AUTOCOMPLETE},
	{"offer", "offer a sacrifice to the gods", dosacrifice, !IFBURIED, AUTOCOMPLETE},
	{"overview", "give an overview of dungeon", dooverview, !IFBURIED, AUTOCOMPLETE},
	{"pray", "pray to the gods for help", dopray, IFBURIED, AUTOCOMPLETE},
	{"quit", "exit without saving current game", done2, IFBURIED, AUTOCOMPLETE},
#ifdef STEED
	{"ride", "ride (or stop riding) a monster", doride, !IFBURIED, AUTOCOMPLETE},
#endif
	{"rub", "rub a lamp", dorub, !IFBURIED, AUTOCOMPLETE},
	{"research", "perform research", doresearch, !IFBURIED, AUTOCOMPLETE},
	{"style", "switch fighting style", dofightingform, !IFBURIED, AUTOCOMPLETE},
	{"sit", "sit down", dosit, !IFBURIED, AUTOCOMPLETE},
	{"swim", "swim under water", dodeepswim, !IFBURIED, AUTOCOMPLETE},
	{"terrain", "display level terrain", doterrain, IFBURIED, AUTOCOMPLETE },
	{"tip", "empty a container", dotip, IFBURIED, AUTOCOMPLETE},
	{"twoweapon", "toggle two-weapon combat", dotwoweapon, !IFBURIED, AUTOCOMPLETE},
	{"turn", "turn undead", doturn, IFBURIED, AUTOCOMPLETE},
	{"unseeinvis", "forget suspected invisible creatures", doclearinvissyms, IFBURIED, AUTOCOMPLETE},
	{"untrap", "untrap something", dountrap, !IFBURIED, AUTOCOMPLETE},
	{"unmaintain", "stop maintaining a spell", dounmaintain, IFBURIED, AUTOCOMPLETE},
	{"versionext", "list compile time options for this version of NetHack",
		doextversion, IFBURIED, AUTOCOMPLETE},
	{"wait", "order pets to wait", dowait, !IFBURIED, AUTOCOMPLETE},
	{"wipe", "wipe off your face", dowipe, !IFBURIED, AUTOCOMPLETE},
	{"?", "get this list of extended commands", doextlist, IFBURIED, AUTOCOMPLETE},
#if defined(WIZARD)
 	/*
 	 * There must be a blank entry here for every entry in the table
 	 * below.
 	 */
	{(char *)0, (char *)0, donull, TRUE}, /* #levelchange */
	{(char *)0, (char *)0, donull, TRUE}, /* #lightsources */
	{(char *)0, (char *)0, donull, TRUE}, /* #detect */
	{(char *)0, (char *)0, donull, TRUE}, /* #gcrown */
	{(char *)0, (char *)0, donull, TRUE}, /* #map level*/
	{(char *)0, (char *)0, donull, TRUE}, /* #wizmakemap */
#ifdef DEBUG_MIGRATING_MONS
	{(char *)0, (char *)0, donull, TRUE}, /* #migratemons */
#endif
	{(char *)0, (char *)0, donull, TRUE}, /* #monpolycontrol */
	{(char *)0, (char *)0, donull, TRUE}, /* #panic */
	{(char *)0, (char *)0, donull, TRUE}, /* #polyself */
#ifdef PORT_DEBUG
	{(char *)0, (char *)0, donull, TRUE}, /* #portdebug */
#endif
	{(char *)0, (char *)0, donull, TRUE}, /* #seenv */
	{(char *)0, (char *)0, donull, TRUE}, /* #showkills (showborn patch) */
	{(char *)0, (char *)0, donull, TRUE}, /* #setinsight */
	{(char *)0, (char *)0, donull, TRUE}, /* #setsanity */
	{(char *)0, (char *)0, donull, TRUE}, /* #stats */
	{(char *)0, (char *)0, donull, TRUE}, /* #timeout */
	{(char *)0, (char *)0, donull, TRUE}, /* #vision */
	{(char *)0, (char *)0, donull, TRUE}, /* #dump_map */
#ifdef DEBUG
	{(char *)0, (char *)0, donull, TRUE}, /* #wizdebug */
#endif
	{(char *)0, (char *)0, donull, TRUE}, /* #wmode */
	{(char *)0, (char *)0, donull, TRUE}, /* #detect */
	{(char *)0, (char *)0, donull, TRUE}, /* #map */
	{(char *)0, (char *)0, donull, TRUE}, /* #genesis */
	{(char *)0, (char *)0, donull, TRUE}, /* #killall */
	{(char *)0, (char *)0, donull, TRUE}, /* #identify */
	{(char *)0, (char *)0, donull, TRUE}, /* #levelport */
	{(char *)0, (char *)0, donull, TRUE}, /* #wish */
	{(char *)0, (char *)0, donull, TRUE}, /* #where */
	{(char *)0, (char *)0, donull, TRUE}, /* #tests */
	{(char *)0, (char *)0, donull, TRUE}, /* #wizbind */
	{(char *)0, (char *)0, donull, TRUE}, /* #wizmutate */
	{(char *)0, (char *)0, donull, TRUE}, /* #wizcult */
#endif
	{(char *)0, (char *)0, donull, TRUE}	/* sentinel */
};

#if defined(WIZARD)
static struct ext_func_tab debug_extcmdlist[] = {
	{"levelchange", "change experience level", wiz_level_change, IFBURIED, AUTOCOMPLETE},
	{"lightsources", "show mobile light sources", wiz_light_sources, IFBURIED, AUTOCOMPLETE},
	{"detect", "do wizard detection", wiz_detect, IFBURIED, AUTOCOMPLETE},
	{"godcrown", "make your god crown you", gcrownu, IFBURIED, AUTOCOMPLETE},
	{"map", "map the current level", wiz_map, IFBURIED, AUTOCOMPLETE},
	{"wizmakemap", "recreate the current level", wiz_makemap, IFBURIED},
#ifdef DEBUG_MIGRATING_MONS
	{"migratemons", "migrate n random monsters", wiz_migrate_mons, IFBURIED, AUTOCOMPLETE},
#endif
	{"monpolycontrol", "control monster polymorphs", wiz_mon_polycontrol, IFBURIED, AUTOCOMPLETE},
	{"panic", "test panic routine (fatal to game)", wiz_panic, IFBURIED, AUTOCOMPLETE},
	{"polyself", "polymorph self", wiz_polyself, IFBURIED, AUTOCOMPLETE},
#ifdef PORT_DEBUG
	{"portdebug", "wizard port debug command", wiz_port_debug, IFBURIED, AUTOCOMPLETE},
#endif
	{"seenv", "show seen vectors", wiz_show_seenv, IFBURIED, AUTOCOMPLETE},
	{"stats", "show memory statistics", wiz_show_stats, IFBURIED, AUTOCOMPLETE},
	{"timeout", "look at timeout queue", wiz_timeout_queue, IFBURIED, AUTOCOMPLETE},
	{"vision", "show vision array", wiz_show_vision, IFBURIED, AUTOCOMPLETE},
	{"showkills", "show list of monsters killed", wiz_showkills, IFBURIED, AUTOCOMPLETE},
	{"setinsight", "sets your insight value", wiz_setinsight, IFBURIED, AUTOCOMPLETE },
	{"setsanity", "sets your sanity value", wiz_setsanity, IFBURIED, AUTOCOMPLETE },
	{"wizbind", "grants knowledge of all seals and binds one", wiz_bind, IFBURIED, AUTOCOMPLETE},
	{"wizcult", "manipulate cult variables", wiz_cult, IFBURIED, AUTOCOMPLETE},
	{"wizmutate", "applies a mutation", wiz_mutate, IFBURIED, AUTOCOMPLETE},
	{"dump_map", "dump map glyphs into a file", wiz_mk_mapglyphdump, IFBURIED, AUTOCOMPLETE},
#ifdef DEBUG
	{"wizdebug", "wizard debug command", wiz_debug_cmd, IFBURIED, AUTOCOMPLETE},
#endif
	{"wmode", "show wall modes", wiz_show_wmodes, IFBURIED, AUTOCOMPLETE},
	{"detect", "detect secret doors and traps", wiz_detect, IFBURIED},
	{"map", "do magic mapping", wiz_map, IFBURIED},
	{"genesis", "create monster", wiz_genesis, IFBURIED},
	{"killall", "kill all creatures on level", wiz_kill_all, IFBURIED, AUTOCOMPLETE },
	{"identify", "identify items in pack", wiz_identify, IFBURIED},
	{"levelport", "to trans-level teleport", wiz_level_tele, IFBURIED},
	{"wish", "make wish", wiz_wish, IFBURIED, AUTOCOMPLETE},
	{"where", "tell locations of special levels", dooverview_or_wiz_where, IFBURIED},
	{"tests", "pull up a menu of regression tests", wiz_testmenu, IFBURIED, AUTOCOMPLETE},
	{(char *)0, (char *)0, donull, IFBURIED}
};

static void
bind_key(key, command)
     uchar key;
     char* command;
{
	struct ext_func_tab * extcmd;

	/* special case: "nothing" is reserved for unbinding */
	if (!strcmp(command, "nothing")) {
		cmdlist[key].bind_cmd = NULL;
		return;
	}

	for(extcmd = extcmdlist; extcmd->ef_txt; extcmd++) {
		if (strcmp(command, extcmd->ef_txt)) continue;
		cmdlist[key].bind_cmd = extcmd;
		return;
	}

	pline("Bad command %s matched with key %c (ASCII %i). "
	      "Ignoring command.\n", command, key, key);
}

static void
init_bind_list(void)
{
	bind_key(C('d'), "kick" ); /* "D" is for door!...?  Msg is in dokick.c */
#ifdef WIZARD
	if (wizard) {
		bind_key(C('g'), "genesis" );
		bind_key(C('i'), "identify" );
		bind_key(C('o'), "where" );
		bind_key(C('v'), "levelport" );
	}
#endif
	bind_key(C('l'), "redraw" ); /* if number_pad is set */
	bind_key(C('p'), "previous" );
	bind_key(C('r'), "redraw" );
	bind_key(C('t'), "teleport" );
	bind_key(C('x'), "attributes" );
#ifdef SUSPEND
	bind_key(C('z'), "suspend" );
#endif
	bind_key('a',    "apply" );
	bind_key('A',    "takeoffall" );
	bind_key(C('a'), "mount" );
	bind_key(M('a'), "adjust" );
	/*       'b', 'B' : go sw */
	bind_key('B',    "ability" );
	bind_key(C('b'), "ability" );
	bind_key('c',    "close" );
	bind_key('C',    "call" );
	bind_key(M('c'), "chat" );
	bind_key('d',    "drop" );
	bind_key('D',    "dropall" );
	bind_key(M('d'), "dip" );
	bind_key('e',    "eat" );
	bind_key('E',    "engrave" );
	bind_key(M('e'), "enhance" );
	bind_key(C('e'), "seal" );
	bind_key('f',    "fire" );
	/*       'F' : fight (one time) */
	bind_key(M('f'), "force" );
	/*       'g', 'G' : multiple go */
	/*       'h', 'H' : go west */
	bind_key(C('f'), "power" );
	bind_key('h',    "help" ); /* if number_pad is set */
	bind_key('i',    "inventory" );
	bind_key('I',    "inventoryall" ); /* Robert Viduya */
	bind_key(M('i'), "invoke" );
	bind_key('j',    "jump" );
	/*       'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N' : move commands */
	bind_key(M('j'), "jump" ); /* if number_pad is on */
	bind_key('k',    "kick" ); /* if number_pad is on */
	bind_key('l',    "loot" ); /* if number_pad is on */
	bind_key(M('l'), "loot" );
	bind_key(M('m'), "monster" );
	bind_key('N',    "name" );
	/*       'n' prefixes a count if number_pad is on */
	bind_key(M('n'), "name" );
	bind_key(M('N'), "name" ); /* if number_pad is on */
	bind_key('o',    "open" );
	bind_key('O',    "setoptions" );
	bind_key(M('o'), "offer" );
#ifdef WIZARD
	if (!wizard) /* Avoid doubling up on Ctrl-o. */
#endif
	bind_key(C('o'), "overview");
	bind_key('p',    "pay" );
	bind_key('P',    "puton" );
	bind_key(M('p'), "pray" );
	bind_key('q',    "quaff" );
	bind_key('Q',    "quiver" );
//	bind_key(M('q'), "ward" );
	bind_key('r',    "read" );
	bind_key('R',    "remove" );
	bind_key(M('r'), "rub" );
	bind_key('s',    "search" );
	bind_key('S',    "save" );
	bind_key(M('s'), "sit" );
	bind_key('t',    "throw" );
	bind_key('T',    "takeoff" );
	bind_key(M('t'), "turn" );
	/*        'u', 'U' : go ne */
	bind_key('u',    "untrap" ); /* if number_pad is on */
	bind_key(C('u'), "unseeinvis");
	bind_key(M('u'), "untrap" );
	bind_key('v',    "version" );
	bind_key('V',    "history" );
	bind_key(M('v'), "versionext" );
	bind_key('w',    "wield" );
	bind_key('W',    "wear" );
	bind_key(C('w'), "ward" );
	bind_key(M('w'), "wipe" );
	bind_key('x',    "swap" );
	bind_key('X',    "twoweapon" );
	/*bind_key('X',    "explore_mode" );*/
	/*        'y', 'Y' : go nw */
#ifdef STICKY_COMMAND
	bind_key(M('y'), "sticky" );
#endif /* STICKY_COMMAND */
	bind_key('z',    "zap" );
	bind_key('Z',    "cast" );
	bind_key('<',    "up" );
	bind_key('>',    "down" );
	bind_key('/',    "whatis" );
	bind_key('&',    "whatdoes" );
	bind_key('?',    "help" );
	bind_key(M('?'), "?" );
#ifdef SHELL
	bind_key('!',    "shell" );
#endif
	bind_key('.',    "rest" );
	bind_key(' ',    "rest" );
	bind_key(',',    "pickup" );
	bind_key(':',    "look" );
	bind_key(';',    "glance" );
	bind_key('^',    "seetrap" );
	bind_key('\\',   "discoveries" ); /* Robert Viduya */
	bind_key('@',    "autopickup" );
	bind_key(M('2'), "twoweapon" );
	bind_key(WEAPON_SYM, "seeweapon" );
	bind_key(ARMOR_SYM,  "seearmor" );
	bind_key(RING_SYM,   "seerings" );
	bind_key(AMULET_SYM, "seeamulet" );
	bind_key(TOOL_SYM,   "seetools" );
	bind_key('*',        "seeall" ); /* inventory of all equipment in use */
	bind_key(GOLD_SYM,   "seegold" );
	bind_key(SPBOOK_SYM, "seespells" ); /* Mike Stephenson */
	bind_key('#', "#");
	bind_key('_', "travel");
}

/* takes the list of bindings loaded from the options file, and changes cmdlist
 * to match it */
static void
change_bind_list(void)
{
	struct binding_list_tab *binding;

	/* TODO: they must be loaded forward, not backward as they are now */
	while ((binding = bindinglist)) {
		bindinglist = bindinglist->next;
		bind_key(binding->key, binding->extcmd);
		free(binding->extcmd);
		free(binding);
	}
}


/*
 * Insert debug commands into the extended command list.  This function
 * assumes that the last entry will be the help entry.
 *
 * You must add entries in ext_func_tab every time you add one to the
 * debug_extcmdlist().
 */
void
add_debug_extended_commands()
{
	int i, j, k, n;

	/* count the # of help entries */
	for (n = 0; extcmdlist[n].ef_txt; n++) ;

	for (i = 0; debug_extcmdlist[i].ef_txt; i++) {
	    extcmdlist[n + i] = debug_extcmdlist[i];
	}
}

/* list all keys and their bindings, like dat/hh but dynamic */
void
dokeylist(void)
{
	char	buf[BUFSZ], buf2[BUFSZ];
	uchar	key;
	boolean keys_used[256] = {0};
	register const char*	dir_keys;
	winid	datawin;
	int	i;
	char*	dir_desc[10] = {"move west",
				"move northwest",
				"move north",
				"move northeast",
				"move east",
				"move southeast",
				"move south",
				"move southwest",
				"move downward",
				"move upward"};
	char*	misc_desc[MISC_CMD_COUNT] = 
		{"rush until something interesting is seen",
		 "run until something extremely interesting is seen",
		 "fight even if you don't see a monster",
		 "move without picking up objects/fighting",
		 "run without picking up objects/fighting",
		 "escape from the current query/action"
#ifdef REDO
		 , "redo the previous command"
#endif
		};


	datawin = create_nhwindow(NHW_TEXT);
	putstr(datawin, 0, "");
	putstr(datawin, 0, "            Full Current Key Bindings List");
	putstr(datawin, 0, "");

	/* directional keys */
	if (iflags.num_pad) dir_keys = ndir;
	else dir_keys = sdir;
	putstr(datawin, 0, "Directional keys:");
	{
	  Sprintf(buf, "  %c %c %c", dir_keys[1], dir_keys[2], dir_keys[3]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "   \\|/");
	  Sprintf(buf, "  %c-.-%c", dir_keys[0], dir_keys[4]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "   /|\\");
	  Sprintf(buf, "  %c %c %c", dir_keys[7], dir_keys[6], dir_keys[5]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "");
	  Sprintf(buf, "    %c  up", dir_keys[9]);
	  putstr(datawin, 0, buf);
	  Sprintf(buf, "    %c  down", dir_keys[8]);
	  putstr(datawin, 0, buf);
	  putstr(datawin, 0, "");
	}
	for (i = 0; i < 10; i++) {
		key = dir_keys[i];
		keys_used[key] = TRUE;
		if (!iflags.num_pad) {
			keys_used[toupper(key)] = TRUE;
			keys_used[C(key)] = TRUE;
		}
		/*
		Sprintf(buf, "%c\t%s", key, dir_desc[i]);
		putstr(datawin, 0, buf);
		*/
	}
	if (!iflags.num_pad) {
		putstr(datawin, 0, "Shift-<direction> will move in specified direction until you hit");
		putstr(datawin, 0, "        a wall or run into something.");
		putstr(datawin, 0, "Ctrl-<direction> will run in specified direction until something");
		putstr(datawin, 0, "        very interesting is seen.");
	}
	putstr(datawin, 0, "");

	/* special keys -- theoretically modifiable but many are still hard-coded*/
	putstr(datawin, 0, "Miscellaneous keys:");
	for (i = 0; i < MISC_CMD_COUNT; i++) {
		key = misc_cmds[i];
		keys_used[key] = TRUE;
		Sprintf(buf, "%s\t%s", key2txt(key, buf2), misc_desc[i]);
		putstr(datawin, 0, buf);
	}
	putstr(datawin, 0, "");

	/* more special keys -- all hard-coded */
#ifndef NO_SIGNAL
	putstr(datawin, 0, "^c\tbreak out of nethack (SIGINT)");
	keys_used[C('c')] = TRUE;
	if (!iflags.num_pad) putstr(datawin, 0, "");
#endif
	if (iflags.num_pad) {
		putstr(datawin, 0, "-\tforce fight (same as above)");
		putstr(datawin, 0, "5\trun (same as above)");
		putstr(datawin, 0, "0\tinventory (as #inventory)");
		keys_used['-'] = keys_used['5'] = keys_used['0'] = TRUE;
		putstr(datawin, 0, "");
	}

	/* command keys - can be rebound or remapped*/
	putstr(datawin, 0, "Command keys:");
	for(i=0; i<=255; i++) {
		struct ext_func_tab * extcmd;
		char* mapping;
		key = i;
		/* JDS: not the most efficient way, perhaps */
		if (keys_used[i]) continue;
		if (key == ' ' && !flags.rest_on_space) continue;
		if ((extcmd = cmdlist[i].bind_cmd)) {
			Sprintf(buf, "%s\t%s", key2txt(key, buf2),
				extcmd->ef_desc);
			putstr(datawin, 0, buf);
		}
	}
	putstr(datawin, 0, "");

	display_nhwindow(datawin, FALSE);
	destroy_nhwindow(datawin);
}

/* find the 1st command key that binds to the desired command */
char *
find_command_key(command_name, buf)
const char * command_name;
char * buf;
{
	int i;
	int key;
	boolean keys_used[256] = { 0 };

	/* kludge: copy from above to find all keys used by non-commands */
	for (i = 0; i < 10; i++) {
		key = iflags.num_pad ? ndir[i] : sdir[i];
		keys_used[key] = TRUE;
		if (!iflags.num_pad) {
			keys_used[toupper(key)] = TRUE;
			keys_used[C(key)] = TRUE;
		}
	}
	for (i = 0; i < MISC_CMD_COUNT; i++) {
		keys_used[(int)misc_cmds[i]] = TRUE;
	}

	for (i = 0; i <= 255; i++) {
		struct ext_func_tab * extcmd;
		char* mapping;
		key = i;
		if (keys_used[i]) continue;
		if (key == ' ' && !flags.rest_on_space) continue;
		if ((extcmd = cmdlist[i].bind_cmd)) {
			if (!strcmpi(extcmd->ef_txt, command_name))
				return key2txt(key, buf);
		}
	}
	return (char *)0;
}

static const char template[] = "%-18s %4ld  %6ld";
static const char count_str[] = "                   count  bytes";
static const char separator[] = "------------------ -----  ------";

STATIC_OVL void
count_obj(chain, total_count, total_size, top, recurse)
	struct obj *chain;
	long *total_count;
	long *total_size;
	boolean top;
	boolean recurse;
{
	long count, size;
	struct obj *obj;

	for (count = size = 0, obj = chain; obj; obj = obj->nobj) {
	    if (top) {
		count++;
		size += sizeof(struct obj);
	    }
	    if (recurse && obj->cobj)
		count_obj(obj->cobj, total_count, total_size, TRUE, TRUE);
	}
	*total_count += count;
	*total_size += size;
}

STATIC_OVL void
obj_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct obj *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;

	count_obj(chain, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
magic_chest_obj_chain(win, src, total_count, total_size)
	winid win;
	const char *src;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	int i;

	for(i=0;i<10;i++)
		count_obj(magic_chest_objs[i], &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_invent_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;

	for (mon = chain; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, TRUE, FALSE);
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
contained(win, src, total_count, total_size)
	winid win;
	const char *src;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count = 0, size = 0;
	struct monst *mon;
	int i;

	count_obj(invent, &count, &size, FALSE, TRUE);
	count_obj(fobj, &count, &size, FALSE, TRUE);
	count_obj(level.buriedobjlist, &count, &size, FALSE, TRUE);
	count_obj(migrating_objs, &count, &size, FALSE, TRUE);
	for (i = 0; i<10; i++)
	    count_obj(magic_chest_objs[i], &count, &size, FALSE, TRUE);
	/* DEADMONSTER check not required in this loop since they have no inventory */
	for (mon = fmon; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);
	for (mon = migrating_mons; mon; mon = mon->nmon)
	    count_obj(mon->minvent, &count, &size, FALSE, TRUE);

	*total_count += count; *total_size += size;

	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

STATIC_OVL void
mon_chain(win, src, chain, total_count, total_size)
	winid win;
	const char *src;
	struct monst *chain;
	long *total_count;
	long *total_size;
{
	char buf[BUFSZ];
	long count, size;
	struct monst *mon;

	for (count = size = 0, mon = chain; mon; mon = mon->nmon) {
	    count++;
	    size += sizeof(struct monst);
	}
	*total_count += count;
	*total_size += size;
	Sprintf(buf, template, src, count, size);
	putstr(win, 0, buf);
}

/*
 * Display memory usage of all monsters and objects on the level.
 */
static int
wiz_show_stats()
{
	char buf[BUFSZ];
	winid win;
	long total_obj_size = 0, total_obj_count = 0;
	long total_mon_size = 0, total_mon_count = 0;

	win = create_nhwindow(NHW_TEXT);
	putstr(win, 0, "Current memory statistics:");
	putstr(win, 0, "");
	Sprintf(buf, "Objects, size %d", (int) sizeof(struct obj));
	putstr(win, 0, buf);
	putstr(win, 0, "");
	putstr(win, 0, count_str);

	obj_chain(win, "invent", invent, &total_obj_count, &total_obj_size);
	obj_chain(win, "fobj", fobj, &total_obj_count, &total_obj_size);
	obj_chain(win, "buried", level.buriedobjlist,
				&total_obj_count, &total_obj_size);
	obj_chain(win, "migrating obj", migrating_objs,
				&total_obj_count, &total_obj_size);
	magic_chest_obj_chain(win, "magic chest obj",
				&total_obj_count,&total_obj_size);
	mon_invent_chain(win, "minvent", fmon,
				&total_obj_count,&total_obj_size);
	mon_invent_chain(win, "migrating minvent", migrating_mons,
				&total_obj_count, &total_obj_size);

	contained(win, "contained",
				&total_obj_count, &total_obj_size);

	putstr(win, 0, separator);
	Sprintf(buf, template, "Total", total_obj_count, total_obj_size);
	putstr(win, 0, buf);

	putstr(win, 0, "");
	putstr(win, 0, "");
	Sprintf(buf, "Monsters, size %d", (int) sizeof(struct monst));
	putstr(win, 0, buf);
	putstr(win, 0, "");

	mon_chain(win, "fmon", fmon,
				&total_mon_count, &total_mon_size);
	mon_chain(win, "migrating", migrating_mons,
				&total_mon_count, &total_mon_size);

	putstr(win, 0, separator);
	Sprintf(buf, template, "Total", total_mon_count, total_mon_size);
	putstr(win, 0, buf);

#if defined(__BORLANDC__) && !defined(_WIN32)
	show_borlandc_stats(win);
#endif

	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return MOVE_CANCELLED;
}

void
sanity_check()
{
	obj_sanity_check();
	timer_sanity_check();
	bc_sanity_check();
}

#ifdef DEBUG_MIGRATING_MONS
static int
wiz_migrate_mons()
{
	int mcount = 0;
	char inbuf[BUFSZ];
	struct permonst *ptr;
	struct monst *mtmp;
	d_level tolevel;
	getlin("How many random monsters to migrate? [0]", inbuf);
	if (*inbuf == '\033') return MOVE_CANCELLED;
	mcount = atoi(inbuf);
	if (mcount < 0 || mcount > (COLNO * ROWNO) || Is_botlevel(&u.uz))
		return MOVE_CANCELLED;
	while (mcount > 0) {
		if (Is_stronghold(&u.uz))
		    assign_level(&tolevel, &valley_level);
		else
		    get_level(&tolevel, depth(&u.uz) + 1);
		ptr = rndmonst(0, 0);
		mtmp = makemon(ptr, 0, 0, NO_MM_FLAGS);
		if (mtmp) migrate_to_level(mtmp, ledger_no(&tolevel),
				MIGR_RANDOM, (coord *)0);
		mcount--;
	}
	return MOVE_INSTANT;
}
#endif

#endif /* WIZARD */

static int
compare_commands(_cmd1, _cmd2)
     /* a wrapper function for strcmp.  Can this be done more simply? */
     void *_cmd1, *_cmd2;
{
	struct ext_func_tab *cmd1 = _cmd1, *cmd2 = _cmd2;

	return strcmp(cmd1->ef_txt, cmd2->ef_txt);
}

void
commands_init(void)
{
	int count = 0;

#ifdef WIZARD
	if (wizard) add_debug_extended_commands();
#endif
	while(extcmdlist[count].ef_txt) count++;

	qsort(extcmdlist, count, sizeof(struct ext_func_tab),
	      &compare_commands);

	init_bind_list();	/* initialize all keyboard commands */
	change_bind_list();	/* change keyboard commands based on options */
}

/* returns a one-byte character from the text (it may massacre the txt
 * buffer) */
char
txt2key(txt)
     char* txt;
{
	txt = stripspace(txt);
	if (!*txt) return 0;

	/* simple character */
	if (!txt[1]) return txt[0];

	/* a few special entries */
	if (!strcmp(txt, "<enter>")) return '\n';
	if (!strcmp(txt, "<space>")) return ' ';
	if (!strcmp(txt, "<esc>"))   return '\033';

	/* control and meta keys */
	switch (*txt) {
	    case 'm': /* can be mx, Mx, m-x, M-x */
	    case 'M':
		    txt++;
		    if(*txt == '-' && txt[1]) txt++;
		    if (txt[1]) return 0;
		    return M( *txt );
	    case 'c': /* can be cx, Cx, ^x, c-x, C-x, ^-x */
	    case 'C':
	    case '^':
		    txt++;
		    if(*txt == '-' && txt[1]) txt++;
		    if (txt[1]) return 0;
		    return C( *txt );
	}

	/* ascii codes: must be three-digit decimal */
	if (*txt >= '0' && *txt <= '9') {
		uchar key = 0;
		int i;
		for(i = 0; i < 3; i++) {
			if(txt[i]<'0' || txt[i]>'9') return 0;
			key = 10 * key + txt[i]-'0';
		}
		return key;
	}

	return 0;
}

/* returns the text for a one-byte encoding
 * must be shorter than a tab for proper formatting */
char*
key2txt(c, txt)
     char c;
     char* txt; /* sufficiently long buffer */
{
	if (c == ' ')
		Sprintf(txt, "<space>");
	else if (c == '\033')
		Sprintf(txt, "<esc>");
	else if (c == '\n')
		Sprintf(txt, "<enter>");
	else if (ISCTRL(c))
		Sprintf(txt, "^%c", UNCTRL(c));
	else if (ISMETA(c))
		Sprintf(txt, "M-%c", UNMETA(c));
	else if (c >= 33 && c <= 126)
		Sprintf(txt, "%c", c);		/* regular keys: ! through ~ */
	else
		Sprintf(txt, "A-%i", c);	/* arbitrary ascii combinations */
	return txt;
}

/* returns the text for a string of one-byte encodings */
char*
str2txt(s, txt)
     char* s;
     char* txt;
{
	char* buf = txt;
	
	while (*s) {
		(void) key2txt(*s, buf);
		buf = eos(buf);
		*buf = ' ';
		buf++;
		*buf = 0;
		s++;
      	}
	return txt;
}


/* strips leading and trailing whitespace */
char*
stripspace(txt)
     char* txt;
{
	char* end;
	while (isspace(*txt)) txt++;
	end = eos(txt);
	while (--end >= txt && isspace(*end)) *end = 0;
	return txt;
}

void
parsebindings(bindings)
     /* closely follows parseoptions in options.c */
     char* bindings;
{
	char *bind;
	char key;
	struct binding_list_tab *newbinding = NULL;

	/* break off first binding from the rest; parse the rest */
	if ((bind = index(bindings, ',')) != 0) {
		*bind++ = 0;
		parsebindings(bind);
	}
 
	/* parse a single binding: first split around : */
	if (! (bind = index(bindings, ':'))) return; /* it's not a binding */
	*bind++ = 0;

	/* read the key to be bound */
	key = txt2key(bindings);
	if (!key) {
		raw_printf("Bad binding %s.", bindings);
		wait_synch();
		return;
	}
	
	/* JDS: crappy hack because wizard mode information
	 * isn't read until _after_ key bindings are read,
	 * and to change this would cause numerous side effects.
	 * instead, I save a list of rebindings, which are later
	 * bound. */
	bind = stripspace(bind);
	newbinding = (struct binding_list_tab *)alloc(sizeof(*newbinding));
	newbinding->key = key;
	newbinding->extcmd = (char *)alloc(strlen(bind)+1);
	strcpy(newbinding->extcmd, bind);;
	newbinding->next = bindinglist;
	bindinglist = newbinding;
}
 
void
parseautocomplete(autocomplete,condition)
     /* closesly follows parsebindings and parseoptions */
     char* autocomplete;
     boolean condition;
{
	register char *autoc;
	int i;
	
	/* break off first autocomplete from the rest; parse the rest */
	if ((autoc = index(autocomplete, ','))
	    || (autoc = index(autocomplete, ':'))) {
		*autoc++ = 0;
		parseautocomplete(autoc, condition);
	}

	/* strip leading and trailing white space */
	autocomplete = stripspace(autocomplete);
	
	if (!*autocomplete) return;

	/* take off negations */
	while (*autocomplete == '!') {
		/* unlike most options, a leading "no" might actually be a part of
		 * the extended command.  Thus you have to use ! */
		autocomplete++;
		condition = !condition;
	}

	/* find and modify the extended command */
	/* JDS: could be much faster [O(log n) vs O(n)] if done differently */
	for (i=0; extcmdlist[i].ef_txt; i++) {
		if (strcmp(autocomplete, extcmdlist[i].ef_txt)) continue;
		extcmdlist[i].autocomplete = condition;
		return;
	}

#ifdef WIZARD
	/* do the exact same thing with the wizmode list */
	/* this is a hack because wizard-mode commands haven't been loaded yet when
	 * this code is run.  See "crappy hack" elsewhere. */
	for (i=0; debug_extcmdlist[i].ef_txt; i++) {
		if (strcmp(autocomplete, debug_extcmdlist[i].ef_txt)) continue;
		debug_extcmdlist[i].autocomplete = condition;
		return;
	}
#endif

	/* not a real extended command */
	raw_printf ("Bad autocomplete: invalid extended command '%s'.", autocomplete);
	wait_synch();
}



char
randomkey()
{
    static unsigned i = 0;
    char c;

    switch (rn2(16)) {
    default:
        c = '\033';
        break;
    case 0:
        c = '\n';
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        c = (char) rn1('~' - ' ' + 1, ' ');
        break;
    case 5:
        c = (char) (rn2(2) ? '\t' : ' ');
        break;
    case 6:
        c = (char) rn1('z' - 'a' + 1, 'a');
        break;
    case 7:
        c = (char) rn1('Z' - 'A' + 1, 'A');
        break;
    //case 8:
        //c = extcmdlist[i++ % SIZE(extcmdlist)].key;
      //  break;
    case 9:
        c = '#';
        break;
    /*case 10:
    case 11:
    case 12:
        c = Cmd.dirchars[rn2(8)];
        if (!rn2(7))
            c = !Cmd.num_pad ? (!rn2(3) ? C(c) : (c + 'A' - 'a')) : M(c);
        break;*/
    case 13:
        c = (char) rn1('9' - '0' + 1, '0');
        break;
    case 14:
        /* any char, but avoid '\0' because it's used for mouse click */
        c = (char) rnd(iflags.wc_eight_bit_input ? 255 : 127);
        break;
    }

    return c;
}




void
rhack(cmd)
register char *cmd;
{
	boolean do_walk, do_rush, prefix_seen, bad_command,
		firsttime = (cmd == 0);

	iflags.menu_requested = FALSE;
	if (firsttime) {
		flags.nopick = 0;
		cmd = parse();
	}
	if (*cmd == DOESCAPE) {
		flags.move |= MOVE_CANCELLED;
		return;
	}
#ifdef REDO
	if (*cmd == DOAGAIN && !in_doagain && saveq[0]) {
		in_doagain = TRUE;
		stail = 0;
		rhack((char *)0);	/* read and execute command */
		in_doagain = FALSE;
		return;
	}
	/* Special case of *cmd == ' ' handled better below */
	if(!*cmd || *cmd == (char)0377)
#else
	if(!*cmd || *cmd == (char)0377 || (!flags.rest_on_space && *cmd == ' '))
#endif
	{
		nhbell();
		flags.move |= MOVE_CANCELLED;
		return;		/* probably we just had an interrupt */
	}
	if (iflags.num_pad && iflags.num_pad_mode == 1) {
	/* This handles very old inconsistent DOS/Windows behaviour
	 * in a new way: earlier, the keyboard handler mapped these,
	 * which caused counts to be strange when entered from the
	 * number pad. Now do not map them until here. 
	 */
		switch (*cmd) {
			case '5':       *cmd = 'g'; break;
			case M('5'):    *cmd = 'G'; break;
			case M('0'):    *cmd = 'I'; break;
		}
	}
	/* handle most movement commands */
	do_walk = do_rush = prefix_seen = FALSE;
	flags.travel = iflags.travel1 = 0;
	if (*cmd == DORUSH) {
	    if (movecmd(cmd[1])) {
		flags.run = 2;
		do_rush = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if ( (*cmd == '5' && iflags.num_pad)
		    || *cmd == DORUN) {
	    if (movecmd(lowc(cmd[1]))) {
		flags.run = 3;
		do_rush = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if ( (*cmd == '-' && iflags.num_pad)
		    || *cmd == DOFORCEFIGHT) {
		/* Effects of movement commands and invisible monsters:
		 * m: always move onto space (even if 'I' remembered)
		 * F: always attack space (even if 'I' not remembered)
		 * normal movement: attack if 'I', move otherwise
		 */
	    if (movecmd(cmd[1])) {
		flags.forcefight = 1;
		do_walk = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if (*cmd == DONOPICKUP) {
	    if (movecmd(cmd[1]) || u.dz) {
		flags.run = 0;
		flags.nopick = 1;
		if (!u.dz) do_walk = TRUE;
		else cmd[0] = cmd[1];	/* "m<" or "m>" */
	    } else
		prefix_seen = TRUE;
	} else if (*cmd == DORUN_NOPICKUP) {
	    if (movecmd(lowc(cmd[1]))) {
		flags.run = 1;
		flags.nopick = 1;
		do_rush = TRUE;
	    } else
		prefix_seen = TRUE;
	} else if (*cmd == '0' && iflags.num_pad) {
	    (void)ddoinv(); /* a convenience borrowed from the PC */
		if(flags.move != MOVE_CANCELLED)
			flags.move |= MOVE_INSTANT;
	    multi = 0;
		return;
	} else if (*cmd == CMD_TRAVEL && iflags.travelcmd) {
	  flags.travel = 1;
	  u.itx = u.ity = 0;
	  iflags.travel1 = 1;
	  flags.run = 8;
	  flags.nopick = 1;
	  do_rush = TRUE;
	} else {
	    if (movecmd(*cmd)) {	/* ordinary movement */
		do_walk = TRUE;
	    } else if (movecmd(iflags.num_pad ?
			       UNMETA(*cmd) : lowc(*cmd))) {
		flags.run = 1;
		do_rush = TRUE;
	    } else if (movecmd(UNCTRL(*cmd))) {
		flags.run = 3;
		do_rush = TRUE;
	    }
	}

	/* some special prefix handling */
	/* overload 'm' prefix for ',' to mean "request a menu" */
	if (prefix_seen && cmd[1] == ',') {
		iflags.menu_requested = TRUE;
		++cmd;
	}

	if (do_walk) {
	    if (multi) flags.mv = TRUE;
	    domove();
	    flags.forcefight = 0;
	    return;
	} else if (do_rush) {
	    if (firsttime) {
		if (!multi) multi = max(COLNO,ROWNO);
		u.last_str_turn = 0;
	    }
	    flags.mv = TRUE;
	    domove();
	    return;
	} else if (prefix_seen && cmd[1] == DOESCAPE) {	/* <prefix><escape> */
	    /* don't report "unknown command" for change of heart... */
	    bad_command = FALSE;
	} else if (*cmd == ' ' && !flags.rest_on_space) {
	    bad_command = TRUE;		/* skip cmdlist[] loop */

	/* handle bound commands */
	} else {
	    const struct key_tab *keytab = &cmdlist[(unsigned char)*cmd];

	    if (keytab->bind_cmd != NULL) {
		struct ext_func_tab *extcmd = keytab->bind_cmd;
		int res, NDECL((*func));

		if (u.uburied && !extcmd->can_if_buried) {
		    You_cant("do that while you are buried!");
		    res = MOVE_CANCELLED;
		} else {
		    func = extcmd->ef_funct;
		    if (extcmd->f_text && !occupation && multi)
			set_occupation(func, extcmd->f_text, multi);
		    last_cmd_char = *cmd;	/* remember pressed character */
		    res = (*func)();		/* perform the command */

			/* backwards compatibility with functions returning 0 to mean no time taken */
			if (res==0)
				res = MOVE_INSTANT;
		}
		flags.move = res;
		if (res & MOVE_CANCELLED) {/* player cancelled action, don't try to repeat it */
			multi = 0;
		}
		return;
	    }
	    /* if we reach here, cmd wasn't found in cmdlist[] */
	    bad_command = TRUE;
	}

	if (bad_command) {
	    char expcmd[10];
	    register char *cp = expcmd;

	    while (*cmd && (int)(cp - expcmd) < (int)(sizeof expcmd - 3)) {
		if (*cmd >= 040 && *cmd < 0177) {
		    *cp++ = *cmd++;
		} else if (*cmd & 0200) {
		    *cp++ = 'M';
		    *cp++ = '-';
		    *cp++ = *cmd++ &= ~0200;
		} else {
		    *cp++ = '^';
		    *cp++ = *cmd++ ^ 0100;
		}
	    }
	    *cp = '\0';
	    if (!prefix_seen || !iflags.cmdassist ||
		!help_dir(0, "Invalid direction key!"))
		Norep("Unknown command '%s'.", expcmd);
		flags.move |= MOVE_CANCELLED;
	}
	else {
		/* didn't move */
		flags.move |= MOVE_INSTANT;
	}
	multi = 0;
	return;
}

int
xytod(x, y)	/* convert an x,y pair into a direction code */
schar x, y;
{
	register int dd;

	for(dd = 0; dd < 8; dd++)
	    if(x == xdir[dd] && y == ydir[dd]) return dd;

	return -1;
}

void
dtoxy(cc,dd)	/* convert a direction code into an x,y pair */
coord *cc;
register int dd;
{
	cc->x = xdir[dd];
	cc->y = ydir[dd];
	return;
}

int
movecmd(sym)	/* also sets u.dz, but returns false for <> */
char sym;
{
	register const char *dp;
	register const char *sdp;
	if(iflags.num_pad) sdp = ndir; else sdp = sdir;	/* DICE workaround */

	u.dz = 0;
	/* sym defaults to null at game start, causing dp - sdp to be 10, overflowing */
	if(!sym || !(dp = index(sdp, sym))) return 0;
	u.dx = xdir[dp-sdp];
	u.dy = ydir[dp-sdp];
	u.dz = zdir[dp-sdp];
	if (u.dx && u.dy && (u.umonnum == PM_GRID_BUG || u.umonnum == PM_BEBELITH)) {
		u.dx = u.dy = 0;
		return 0;
	}
	return !u.dz;
}

/*
 * uses getdir() but unlike getdir() it specifically
 * produces coordinates using the direction from getdir()
 * and verifies that those coordinates are ok.
 *
 * If the call to getdir() returns 0, Never_mind is displayed.
 * If the resulting coordinates are not okay, emsg is displayed.
 *
 * Returns non-zero if coordinates in cc are valid.
 */
int get_adjacent_loc(prompt,emsg,x,y,cc)
const char *prompt, *emsg;
xchar x,y;
coord *cc;
{
	xchar new_x, new_y;
	if (!getdir(prompt)) {
		pline1(Never_mind);
		return 0;
	}
	new_x = x + u.dx;
	new_y = y + u.dy;
	if (cc && isok(new_x,new_y)) {
		cc->x = new_x;
		cc->y = new_y;
	} else {
		if (emsg) pline1(emsg);
		return 0;
	}
	return 1;
}

int
getdir(s)
const char *s;
{
	char dirsym;
	static char saved_dirsym = '\0'; /* saved direction of the previous call of getdir() */
#ifdef REDO
	if(in_doagain || *readchar_queue)
	    dirsym = readchar();
	else
#endif
	    dirsym = yn_function ((s && *s != '^') ? s : "In what direction?",
					(char *)0, '\0');
#ifdef REDO
	savech(dirsym);
#endif
	if (dirsym == last_cmd_char) {
	  /* in here dirsym is not representing a direction
	   * but the same sym used before for calling the
	   * current cmd */
	  movecmd(saved_dirsym);
	  dirsym = saved_dirsym;
        } else if(dirsym == '.' || dirsym == 's')
		u.dx = u.dy = u.dz = 0;
	else if(!movecmd(dirsym) && !u.dz) {
		boolean did_help = FALSE;
		if(!index(quitchars, dirsym)) {
		    if (iflags.cmdassist) {
			did_help = help_dir((s && *s == '^') ? dirsym : 0,
					    "Invalid direction key!");
		    }
		    if (!did_help) pline("What a strange direction!");
		}
		return 0;
	}
	saved_dirsym = dirsym;
	if(!u.dz && (Stunned || (Confusion && !rn2(5)))) confdir();
	return 1;
}

STATIC_OVL boolean
help_dir(sym, msg)
char sym;
const char *msg;
{
	char ctrl;
	winid win;
	static const char wiz_only_list[] = "EFGIOVW";
	char buf[BUFSZ], buf2[BUFSZ], *expl;

	win = create_nhwindow(NHW_TEXT);
	if (!win) return FALSE;
	if (msg) {
		Sprintf(buf, "cmdassist: %s", msg);
		putstr(win, 0, buf);
		putstr(win, 0, "");
	}
	if (letter(sym)) { 
	    sym = highc(sym);
	    ctrl = (sym - 'A') + 1;
	    if ((expl = dowhatdoes_core(ctrl, buf2))
		&& (!index(wiz_only_list, sym)
#ifdef WIZARD
		    || wizard
#endif
	                     )) {
		Sprintf(buf, "Are you trying to use ^%c%s?", sym,
			index(wiz_only_list, sym) ? "" :
			" as specified in the Guidebook");
		putstr(win, 0, buf);
		putstr(win, 0, "");
		putstr(win, 0, expl);
		putstr(win, 0, "");
		putstr(win, 0, "To use that command, you press");
		Sprintf(buf,
			"the <Ctrl> key, and the <%c> key at the same time.", sym);
		putstr(win, 0, buf);
		putstr(win, 0, "");
	    }
	}
	if (iflags.num_pad && (u.umonnum == PM_GRID_BUG || u.umonnum == PM_BEBELITH)) {
	    putstr(win, 0, "Valid direction keys in your current form (with number_pad on) are:");
	    putstr(win, 0, "             8   ");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "          4- . -6");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "             2   ");
	} else if (u.umonnum == PM_GRID_BUG || u.umonnum == PM_BEBELITH) {
	    putstr(win, 0, "Valid direction keys in your current form are:");
	    putstr(win, 0, "             k   ");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "             |   ");
	    putstr(win, 0, "             j   ");
	} else if (iflags.num_pad) {
	    putstr(win, 0, "Valid direction keys (with number_pad on) are:");
	    putstr(win, 0, "          7  8  9");
	    putstr(win, 0, "           \\ | / ");
	    putstr(win, 0, "          4- . -6");
	    putstr(win, 0, "           / | \\ ");
	    putstr(win, 0, "          1  2  3");
	} else {
	    putstr(win, 0, "Valid direction keys are:");
	    putstr(win, 0, "          y  k  u");
	    putstr(win, 0, "           \\ | / ");
	    putstr(win, 0, "          h- . -l");
	    putstr(win, 0, "           / | \\ ");
	    putstr(win, 0, "          b  j  n");
	};
	putstr(win, 0, "");
	putstr(win, 0, "          <  up");
	putstr(win, 0, "          >  down");
	putstr(win, 0, "          .  direct at yourself");
	putstr(win, 0, "");
	putstr(win, 0, "(Suppress this message with !cmdassist in config file.)");
	display_nhwindow(win, FALSE);
	destroy_nhwindow(win);
	return TRUE;
}

#endif /* OVL1 */
#ifdef OVLB

void
confdir()
{
	register int x;
	if(u.udrunken >= 3 && rn2(u.udrunken/3 + 1))
		return;
	x = (u.umonnum == PM_GRID_BUG || u.umonnum == PM_BEBELITH) ? 2*rn2(4) : rn2(8);
	u.dx = xdir[x];
	u.dy = ydir[x];
	return;
}

#endif /* OVLB */
#ifdef OVL0

int
isok(x,y)
register int x, y;
{
	/* x corresponds to curx, so x==1 is the first column. Ach. %% */
	return x >= 1 && x <= COLNO-1 && y >= 0 && y <= ROWNO-1;
}

static NEARDATA int last_multi;

/*
 * convert a MAP window position into a movecmd
 */
const char *
click_to_cmd(x, y, mod)
    int x, y, mod;
{
    int dir;
    static char cmd[4];
    cmd[1]=0;

    x -= u.ux;
    y -= u.uy;

    if (iflags.travelcmd) {
        if (abs(x) <= 1 && abs(y) <= 1 ) {
            x = sgn(x), y = sgn(y);
        } else {
            u.tx = u.ux+x;
            u.ty = u.uy+y;
            cmd[0] = CMD_TRAVEL;
            return cmd;
        }

        if(x == 0 && y == 0) {
            /* here */
            if(IS_FOUNTAIN(levl[u.ux][u.uy].typ) || IS_SINK(levl[u.ux][u.uy].typ)) {
                cmd[0]=mod == CLICK_1 ? 'q' : M('d');
                return cmd;
            } else if(IS_FORGE(levl[u.ux][u.uy].typ)) {
                cmd[0]=M('d');
                return cmd;
            } else if(IS_THRONE(levl[u.ux][u.uy].typ)) {
                cmd[0]=M('s');
                return cmd;
            } else if((u.ux == xupstair && u.uy == yupstair)
                      || (u.ux == sstairs.sx && u.uy == sstairs.sy && sstairs.up)
                      || (u.ux == xupladder && u.uy == yupladder)) {
                return "<";
            } else if((u.ux == xdnstair && u.uy == ydnstair)
                      || (u.ux == sstairs.sx && u.uy == sstairs.sy && !sstairs.up)
                      || (u.ux == xdnladder && u.uy == ydnladder)) {
                return ">";
            } else if(OBJ_AT(u.ux, u.uy)) {
                cmd[0] = Is_container(level.objects[u.ux][u.uy]) ? M('l') : ',';
                return cmd;
            } else {
                return "."; /* just rest */
            }
        }

        /* directional commands */

        dir = xytod(x, y);

	if (!m_at(u.ux+x, u.uy+y) && !test_move(u.ux, u.uy, x, y, TEST_MOVE)) {
            cmd[1] = (iflags.num_pad ? ndir[dir] : sdir[dir]);
            cmd[2] = 0;
            if (IS_DOOR(levl[u.ux+x][u.uy+y].typ)) {
                /* slight assistance to the player: choose kick/open for them */
                if (levl[u.ux+x][u.uy+y].doormask & D_LOCKED) {
                    cmd[0] = C('d');
                    return cmd;
                }
                if (levl[u.ux+x][u.uy+y].doormask & D_CLOSED) {
                    cmd[0] = 'o';
                    return cmd;
                }
            }
            if (levl[u.ux+x][u.uy+y].typ <= SCORR) {
                cmd[0] = 's';
                cmd[1] = 0;
                return cmd;
            }
        }
    } else {
        /* convert without using floating point, allowing sloppy clicking */
        if(x > 2*abs(y))
            x = 1, y = 0;
        else if(y > 2*abs(x))
            x = 0, y = 1;
        else if(x < -2*abs(y))
            x = -1, y = 0;
        else if(y < -2*abs(x))
            x = 0, y = -1;
        else
            x = sgn(x), y = sgn(y);

        if(x == 0 && y == 0)	/* map click on player to "rest" command */
            return ".";

        dir = xytod(x, y);
    }

    /* move, attack, etc. */
    cmd[1] = 0;
    if(mod == CLICK_1) {
	cmd[0] = (iflags.num_pad ? ndir[dir] : sdir[dir]);
    } else {
	cmd[0] = (iflags.num_pad ? M(ndir[dir]) :
		(sdir[dir] - 'a' + 'A')); /* run command */
    }

    return cmd;
}

STATIC_OVL char *
parse()
{
#ifdef LINT	/* static char in_line[COLNO]; */
	char in_line[COLNO];
#else
	static char in_line[COLNO];
#endif
	register int foo;
	boolean prezero = FALSE;

	multi = 0;
	flags.move = MOVE_DEFAULT;
	flush_screen(1); /* Flush screen buffer. Put the cursor on the hero. */

	if (!iflags.num_pad || (foo = readchar()) == 'n')
	    for (;;) {
		foo = readchar();
		if (foo >= '0' && foo <= '9') {
		    multi = 10 * multi + foo - '0';
		    if (multi < 0 || multi >= 200) multi = 200;
		    if (multi > 9) {
			clear_nhwindow(WIN_MESSAGE);
			Sprintf(in_line, "Count: %d", multi);
			pline1(in_line);
			mark_synch();
		    }
		    last_multi = multi;
		    if (!multi && foo == '0') prezero = TRUE;
		} else break;	/* not a digit */
	    }

	if (foo == DOESCAPE) {   /* esc cancels count (TH) */
	    clear_nhwindow(WIN_MESSAGE);
	    multi = last_multi = 0;
# ifdef REDO
	} else if (foo == DOAGAIN || in_doagain) {
	    multi = last_multi;
	} else {
	    last_multi = multi;
	    savech(0);	/* reset input queue */
	    savech((char)foo);
# endif
	}

	if (multi) {
	    multi--;
	    save_cm = in_line;
	} else {
	    save_cm = (char *)0;
	}
	in_line[0] = foo;
	in_line[1] = '\0';
	if (foo == DORUSH || foo == DORUN || foo == DOFORCEFIGHT
	    || foo == DONOPICKUP || foo == DORUN_NOPICKUP
	    || (iflags.num_pad && (foo == '5' || foo == '-'))) {
	    foo = readchar();
#ifdef REDO
	    savech((char)foo);
#endif
	    in_line[1] = foo;
	    in_line[2] = 0;
	}
	clear_nhwindow(WIN_MESSAGE);
	if (prezero) in_line[0] = DOESCAPE;

	return(in_line);
}

#endif /* OVL0 */
#ifdef OVLB

#ifdef UNIX
static
void
end_of_input()
{
#ifndef NOSAVEONHANGUP
	if (!program_state.done_hup++ && program_state.something_worth_saving)
	    (void) dosave0();
#endif
	exit_nhwindows((char *)0);
	clearlocks();
	terminate(EXIT_SUCCESS);
}
#endif

#endif /* OVLB */
#ifdef OVL0

char
readchar()
{
	register int sym;
	int x = u.ux, y = u.uy, mod = 0;

	if (iflags.debug_fuzzer)
	        return randomkey();

	if ( *readchar_queue )
	    sym = *readchar_queue++;
	else
#ifdef REDO
	    sym = in_doagain ? Getchar() : nh_poskey(&x, &y, &mod);
#else
	    sym = Getchar();
#endif

#ifdef UNIX
# ifdef NR_OF_EOFS
	if (sym == EOF) {
	    register int cnt = NR_OF_EOFS;
	  /*
	   * Some SYSV systems seem to return EOFs for various reasons
	   * (?like when one hits break or for interrupted systemcalls?),
	   * and we must see several before we quit.
	   */
	    do {
		clearerr(stdin);	/* omit if clearerr is undefined */
		sym = Getchar();
	    } while (--cnt && sym == EOF);
	}
# endif /* NR_OF_EOFS */
	if (sym == EOF)
	    end_of_input();
#endif /* UNIX */

	if(sym == 0) {
	    /* click event */
	    readchar_queue = click_to_cmd(x, y, mod);
	    sym = *readchar_queue++;
	}
	return((char) sym);
}

int
dotravel()
{
	/* Keyboard travel command */
	static char cmd[2];
	coord cc;

	if (!iflags.travelcmd) return MOVE_CANCELLED;
	cmd[1]=0;
	cc.x = iflags.travelcc.x;
	cc.y = iflags.travelcc.y;
	if (cc.x == -1 && cc.y == -1) {
	    /* No cached destination, start attempt from current position */
	    cc.x = u.ux;
	    cc.y = u.uy;
	}
	pline("Where do you want to travel to?");
	if (getpos(&cc, TRUE, "the desired destination") < 0) {
		/* user pressed ESC */
		return MOVE_CANCELLED;
	}
	iflags.travelcc.x = u.tx = cc.x;
	iflags.travelcc.y = u.ty = cc.y;
	cmd[0] = CMD_TRAVEL;
	readchar_queue = cmd;
	return MOVE_INSTANT;
}

#ifdef PORT_DEBUG
# ifdef WIN32CON
extern void NDECL(win32con_debug_keystrokes);
extern void NDECL(win32con_handler_info);
# endif

int
wiz_port_debug()
{
	int n, k;
	winid win;
	anything any;
	int item = 'a';
	int num_menu_selections;
	struct menu_selection_struct {
		char *menutext;
		void NDECL((*fn));
	} menu_selections[] = {
#ifdef WIN32CON
		{"test win32 keystrokes", win32con_debug_keystrokes},
		{"show keystroke handler information", win32con_handler_info},
#endif
		{(char *)0, (void NDECL((*)))0}		/* array terminator */
	};

	num_menu_selections = SIZE(menu_selections) - 1;
	if (num_menu_selections > 0) {
		menu_item *pick_list;
		win = create_nhwindow(NHW_MENU);
		start_menu(win);
		for (k=0; k < num_menu_selections; ++k) {
			any.a_int = k+1;
			add_menu(win, NO_GLYPH, &any, item++, 0, ATR_NONE,
				menu_selections[k].menutext, MENU_UNSELECTED);
		}
		end_menu(win, "Which port debugging feature?");
		n = select_menu(win, PICK_ONE, &pick_list);
		destroy_nhwindow(win);
		if (n > 0) {
			n = pick_list[0].item.a_int - 1;
			free((genericptr_t) pick_list);
			/* execute the function */
			(*menu_selections[n].fn)();
		}
	} else
		pline("No port-specific debug capability defined.");
	return 0;
}
# endif /*PORT_DEBUG*/

#endif /* OVL0 */
#ifdef OVLB
/*
 *   Parameter validator for generic yes/no function to prevent
 *   the core from sending too long a prompt string to the
 *   window port causing a buffer overflow there.
 */
char
yn_function(plainquery,resp, def)
const char *plainquery,*resp;
char def;
{
	char qbuf[QBUFSZ];
	const char *query;
	unsigned truncspot, reduction = sizeof(" [N]  ?") + 1;

	/*Ben Collver's fixes*/
//	if(Role_if(PM_PIRATE)) query = piratesay(plainquery);
//	else query = plainquery;
	query = plainquery;
	
	if (resp) reduction += strlen(resp) + sizeof(" () ");
	if (strlen(query) < (QBUFSZ - reduction))
		return (*windowprocs.win_yn_function)(query, resp, def);
	paniclog("Query truncated: ", query);
	reduction += sizeof("...");
	truncspot = QBUFSZ - reduction;
	(void) strncpy(qbuf, query, (int)truncspot);
	qbuf[truncspot] = '\0';
	Strcat(qbuf,"...");
	return (*windowprocs.win_yn_function)(qbuf, resp, def);
}
#endif

/*
 * Replacement for yn() that can give a yes/no prompt instead.
 *
 * If paranoid is TRUE, gives a yes/no prompt.  Otherwise gives a y/n
 * prompt.  Returns 'y' or 'n'.
 */
char
yesno(const char *query, boolean paranoid)
{
	if (paranoid) {
		char qbuf[BUFSZ];
		char rbuf[BUFSZ];
	        Snprintf(qbuf, BUFSZ, "%s [yes/no] (no)", query);
		getlin(qbuf, rbuf);
	        lcase(rbuf);
		return !strcmp(rbuf, "yes") ? 'y' : 'n';
	} else {
		return yn_function(query, ynchars, 'n');
	}
}

/*cmd.c*/
