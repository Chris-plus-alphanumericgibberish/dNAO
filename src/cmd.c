/*	SCCS Id: @(#)cmd.c	3.4	2003/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <ctype.h>

#include "hack.h"
#include "func_tab.h"
/* #define DEBUG */	/* uncomment for debugging */

/*
 * Some systems may have getchar() return EOF for various reasons, and
 * we should not quit before seeing at least NR_OF_EOFS consecutive EOFs.
 */
#if defined(SYSV) || defined(DGUX) || defined(HPUX)
#define NR_OF_EOFS	20
#endif

#define CMD_TRAVEL (char)0x90
#define DOATTRIB_RESISTS	1
#define DOATTRIB_ARMOR		2
#define DOATTRIB_ENLIGHTEN	3
#define DOATTRIB_BINDINGS	4
#define DOATTRIB_SPIRITS	5

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
extern int NDECL(doextlist); /**/
extern int NDECL(dodrop); /**/
extern int NDECL(doddrop); /**/
extern int NDECL(dodown); /**/
extern int NDECL(doup); /**/
extern int NDECL(donull); /**/
extern int NDECL(dowait); /**/
extern int NDECL(dowipe); /**/
extern int NDECL(do_mname); /**/
extern int NDECL(ddocall); /**/
extern void FDECL(do_oname, (struct obj *));
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
STATIC_PTR int NDECL(dofightingform);
STATIC_PTR int NDECL(dooverview_or_wiz_where);
# ifdef WIZARD
STATIC_PTR int NDECL(wiz_mk_mapglyphdump);
STATIC_PTR int NDECL(wiz_wish);
STATIC_PTR int NDECL(wiz_identify);
STATIC_PTR int NDECL(wiz_map);
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
STATIC_PTR int NDECL(doattributes);
STATIC_PTR int NDECL(doconduct); /**/
STATIC_PTR int NDECL(minimal_enlightenment);
STATIC_PTR void NDECL(resistances_enlightenment);
STATIC_PTR void NDECL(signs_enlightenment);
STATIC_PTR void NDECL(spirits_enlightenment);

static void FDECL(bind_key, (UCHAR_P, char*));
static void NDECL(init_bind_list);
static void NDECL(change_bind_list);
#ifdef WIZARD
//static void NDECL(add_debug_extended_commands); //Moved to extern.h
#endif /* WIZARD */

#ifdef OVLB
STATIC_DCL void FDECL(enlght_line, (const char *,const char *,const char *));
STATIC_DCL char *FDECL(enlght_combatinc, (const char *,int,int,char *));
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
	(*timed_occ_fn)();
	if (multi > 0)
		multi--;
	return multi > 0;
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
			}
			/* note: when shield and armor match despite color differences, it is the shield's color that is used */
			armormatch = Dragon_shield_to_pm(uarms) == &mons[mtyp];
		}
		if (!armormatch) {
			add_ability('a', "Use your armor's breath weapon", MATTK_DSCALE);
		}
	}
	if (mon_abilities && is_were(youracedata)){
		add_ability('A', "Summon aid", MATTK_SUMM);
	}
	if (mon_abilities && (can_breathe(youmonst.data) || Race_if(PM_HALF_DRAGON))){
		add_ability('b', "Use your breath weapon", MATTK_BREATH);
	}
	if (mon_abilities && (Upolyd && can_breathe(youmonst.data) && Race_if(PM_HALF_DRAGON))){
		add_ability('B', "Use your halfdragon breath weapon", MATTK_HBREATH);
	}
	if (mon_abilities && youracedata->mtyp == PM_TOVE){
		add_ability('B', "Bore a hole", MATTK_HOLE);
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
	if (you_abilities && uwep && is_lightsaber(uwep)) {	/* I can't wait until fighting forms are mainstream */
		add_ability('F', "Pick a fighting form", MATTK_U_STYLE);
	}
	if (mon_abilities && attacktype(youracedata, AT_GAZE)){
		add_ability('g', "Gaze at something", MATTK_GAZE);
	}
	if (mon_abilities && is_hider(youracedata)){
		add_ability('h', "Hide", MATTK_HIDE);
	}
	if (mon_abilities && is_drow(youracedata)){
		add_ability('i', "Invoke the darkness", MATTK_DARK);
	}
	if (mon_abilities && youracedata->mlet == S_NYMPH){
		add_ability('I', "Remove an iron ball", MATTK_REMV);
	}
	if (mon_abilities && is_mind_flayer(youracedata)){
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
		return 0;
	}
	
	if (mon_abilities && you_abilities)
		Strcpy(buf, "Choose which ability to use");
	else
		Sprintf(buf, "Choose which %s ability to use", mon_abilities ? "monster" : "player");
	end_menu(tmpwin, buf);

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	
	if(n <= 0) return 0;
	
	switch (selected[0].item.a_int) {
	/* Player abilities */
	case MATTK_U_WORD: return dowords(SPELLMENU_CAST);
	case MATTK_U_SPELLS: return docast();
	case MATTK_U_SPIRITS: return dospirit();
	case MATTK_U_TURN_UNDEAD: return doturn();
	case MATTK_U_STYLE: return dofightingform();
	case MATTK_U_MONST: return domonability();
	case MATTK_U_ELMENTAL: return doelementalbreath();

	/* Monster (or monster-like) abilities */
	case MATTK_BREATH: return dobreathe(youmonst.data);
	case MATTK_HBREATH: return dobreathe(&mons[PM_HALF_DRAGON]);
	case MATTK_DSCALE:{
		int res = dobreathe(Dragon_shield_to_pm(uarms));
		if(res){
			if(!uarm->oartifact) uarm->age = monstermoves + (long)(rnz(100)*(Role_if(PM_CAVEMAN) ? .8 : 1));
			uarms->age = monstermoves + (long)(rnz(100)*(Role_if(PM_CAVEMAN) ? .8 : 1));
		}
		return res;
	}
	case MATTK_SPIT: return dospit();
	case MATTK_MAGIC: 
		return xcasty(&youmonst, (struct monst *)0, &youracedata->mattk[attackindex(youracedata, AT_MAGC, AD_ANY)], 0, 0);
		
//		return castum((struct monst *)0,
//	                   &youracedata->mattk[attackindex(youracedata, 
//			                         AT_MAGC,AD_ANY)]);
	case MATTK_REMV: return doremove();
	case MATTK_GAZE: return dogaze();
	case MATTK_TNKR: return dotinker();
	case MATTK_SUMM: return dosummon();
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
			return 1;
	    } else {
			There("is no fountain here.");
			return 0;
		}
	}
	break;
	case MATTK_UHORN: {
	    use_unicorn_horn((struct obj *)0);
	    return 1;
	}
	break;
	case MATTK_SHRIEK: {
	    You("shriek.");
	    if(u.uburied) pline("Unfortunately sound does not carry well through rock.");
	    else aggravate();
		return 1;
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
		return 1;
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
			(lev->typ == DRAWBRIDGE_UP) ||
			(IS_THRONE(lev->typ)) ||
			(IS_ALTAR(lev->typ)) ||
			(Is_airlevel(&u.uz))
		)){
			typ = fillholetyp(u.ux,u.uy);
			You("gyre and gimble into the %s.", surface(u.ux,u.uy));
			if (typ != ROOM) {
				lev->typ = typ;
				if (ttmp) (void) delfloortrap(ttmp);
				/* if any objects were frozen here, they're released now */
				unearth_objs(u.ux, u.uy);

					if(cansee(u.ux, u.uy))
						pline_The("hole fills with %s!",
						  typ == LAVAPOOL ? "lava" : "water");
				if (!Levitation && !Flying) {
					if (typ == LAVAPOOL)
					(void) lava_effects();
					else if (!Wwalking)
					(void) drown();
				}
			}
			if (nohole || !ttmp || (ttmp->ttyp != PIT && ttmp->ttyp != SPIKED_PIT && ttmp->ttyp != TRAPDOOR))
				digactualhole(u.ux, u.uy, &youmonst, PIT, FALSE, TRUE);
			else
				digactualhole(u.ux, u.uy, &youmonst, HOLE, FALSE, TRUE);
			return 1;
		} else {
			You("gyre and gimble, but the %s is too hard!", surface(u.ux,u.uy));
			return 1;
		}
	}
	break;
	case MATTK_REACH: return use_reach_attack();
	break;
	}
	return 0;
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
		return 0;
	}
	
	if(P_SKILL(P_RIDING) < P_EXPERT){
		pline("Only an expert is skilled enough to direct a mount's attacks.");
		return 0;
	}
	
	if(!getdir("Attack in what direction?")){
		pline("never mind");
		return 0;
	}
	
	for(new_x = u.ux+u.dx, new_y = u.uy+u.dy; isok(new_x,new_y); new_x += u.dx, new_y += u.dy){
		mtmp = m_at(new_x, new_y);
		if(mtmp && mon_can_see_mon(u.usteed, mtmp) && canspotmon(mtmp)){
			You("direct your mount to attack %s", mon_nam(mtmp));
			mattackm(u.usteed, mtmp);
			return 1;
		}
	}
	pline("Your mount can't find anything to attack!");
	return 0;
#else
	pline("You can't ride anything!");
#endif
}

STATIC_OVL int
use_reach_attack()
{
	int res = 0, typ, max_range = 4, min_range = 1;
	coord cc;
	struct monst *mtmp;


	/* Are you allowed to use a reach attack? */
	if (u.uswallow) {
	    pline("There's not enough room here to use that.");
	    return (0);
	}
	/* Prompt for a location */
	pline("Where do you want to hit?");
	cc.x = u.ux;
	cc.y = u.uy;
	if (getpos(&cc, TRUE, "the spot to hit") < 0)
	    return 0;	/* user pressed ESC */

	/* Calculate range */
	typ = P_BARE_HANDED_COMBAT;
	if (typ == P_NONE || P_SKILL(typ) <= P_BASIC) max_range = 4;
	else if ( P_SKILL(typ) == P_SKILLED) max_range = 5;
	else if ( P_SKILL(typ) == P_EXPERT) max_range = 8;
	else if ( P_SKILL(typ) == P_MASTER) max_range = 9;
	else max_range = 10;
	if (distu(cc.x, cc.y) > max_range) {
	    pline("Too far!");
	    return (res);
	} else if (distu(cc.x, cc.y) < min_range) {
	    pline("Too close!");
	    return (res);
	} else if (!cansee(cc.x, cc.y) &&
		   ((mtmp = m_at(cc.x, cc.y)) == (struct monst *)0 ||
		    !canseemon(mtmp))) {
	    You("won't hit anything if you can't see that spot.");
	    return (res);
	} else if (!couldsee(cc.x, cc.y)) { /* Eyes of the Overworld */
	    You("can't reach that spot from here.");
	    return res;
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
	return (1);
}

int
dofightingform()
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;
	
	if(!(uwep && is_lightsaber(uwep))){
		pline("You don't know any special fighting styles for use in this situation.");
		return 0;
	}
	
	if(P_SKILL(weapon_type(uwep)) < P_BASIC){
		pline("You must have at least some basic skill in the use of your weapon before you can employ special fighting styles.");
		return 0;
	}

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */
	
	Sprintf(buf,	"Known Forms");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	if(P_SKILL(P_SHII_CHO) >= P_BASIC){
		if(activeFightingForm(FFORM_SHII_CHO)) {
			Sprintf(buf,	"Shii-Cho (active)");
		} else {
			Sprintf(buf,	"Shii-Cho");
		}
		any.a_int = FFORM_SHII_CHO;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(P_SKILL(P_MAKASHI) >= P_BASIC){
		if(activeFightingForm(FFORM_MAKASHI)) {
			if(uarm && !(is_light_armor(uarm) || is_medium_armor(uarm))){
				Sprintf(buf,	"Makashi (selected; blocked by armor)");
			} else {
				Sprintf(buf,	"Makashi (active)");
			}
		} else {
			if(uarm && !(is_light_armor(uarm) || is_medium_armor(uarm))){
				Sprintf(buf,	"Makashi (blocked by armor)");
			} else {
				Sprintf(buf,	"Makashi");
			}
		}
		any.a_int = FFORM_MAKASHI;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(P_SKILL(P_SORESU) >= P_BASIC){
		if(activeFightingForm(FFORM_SORESU)) {
			if(uarm && !(is_light_armor(uarm) || is_medium_armor(uarm))){
				Sprintf(buf,	"Soresu (selected; blocked by armor)");
			} else {
				Sprintf(buf,	"Soresu (active)");
			}
		} else {
			if(uarm && !(is_light_armor(uarm) || is_medium_armor(uarm))){
				Sprintf(buf,	"Soresu (blocked by armor)");
			} else {
				Sprintf(buf,	"Soresu");
			}
		}
		any.a_int = FFORM_SORESU;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(P_SKILL(P_ATARU) >= P_BASIC){
		if(activeFightingForm(FFORM_ATARU)) {
			if(uarm && !(is_light_armor(uarm))){
				Sprintf(buf,	"Ataru (selected; blocked by armor)");
			} else {
				Sprintf(buf,	"Ataru (active)");
			}
		} else {
			if(uarm && !(is_light_armor(uarm))){
				Sprintf(buf,	"Ataru (blocked by armor)");
			} else {
				Sprintf(buf,	"Ataru");
			}
		}
		any.a_int = FFORM_ATARU;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(P_SKILL(P_SHIEN) >= P_BASIC){
		if(activeFightingForm(FFORM_SHIEN)) {
			if(uarm && !(is_light_armor(uarm))){
				Sprintf(buf,	"Shien (selected; blocked by armor)");
			} else {
				Sprintf(buf,	"Shien (active)");
			}
		} else {
			if(uarm && !(is_light_armor(uarm))){
				Sprintf(buf,	"Shien (blocked by armor)");
			} else {
				Sprintf(buf,	"Shien");
			}
		}
		any.a_int = FFORM_SHIEN;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(P_SKILL(P_DJEM_SO) >= P_BASIC){
		if(activeFightingForm(FFORM_DJEM_SO)) {
			if(uarm && !(is_light_armor(uarm) || is_medium_armor(uarm))){
				Sprintf(buf,	"Djem So (selected; blocked by armor)");
			} else {
				Sprintf(buf,	"Djem So (active)");
			}
		} else {
			if(uarm && !(is_light_armor(uarm) || is_medium_armor(uarm))){
				Sprintf(buf,	"Djem So (blocked by armor)");
			} else {
				Sprintf(buf,	"Djem So");
			}
		}
		any.a_int = FFORM_DJEM_SO;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(P_SKILL(P_NIMAN) >= P_BASIC){
		if(activeFightingForm(FFORM_NIMAN)) {
			if(uarm && (is_metallic(uarm))){
				Sprintf(buf,	"Niman (selected; blocked by armor)");
			} else {
				int nskill = P_SKILL(P_NIMAN);
				if(u.lastcast >= monstermoves && nskill >= P_BASIC){
					Sprintf(buf,	"Niman (active; +%dd%ld)", 
						nskill == P_BASIC ? 3 : 
						nskill == P_SKILLED ? 6 : 
						nskill == P_EXPERT ? 9 : 0, 
						u.lastcast-monstermoves+1);
				} else
					Sprintf(buf,	"Niman (active)");
			}
		} else {
			if(uarm && (is_metallic(uarm))){
				Sprintf(buf,	"Niman (blocked by armor)");
			} else {
				Sprintf(buf,	"Niman");
			}
		}
		any.a_int = FFORM_NIMAN;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	if(P_SKILL(P_JUYO) >= P_BASIC){
		if(activeFightingForm(FFORM_JUYO)) {
			if(uarm && !(is_light_armor(uarm))){
				Sprintf(buf,	"Juyo (selected; blocked by armor)");
			} else {
				Sprintf(buf,	"Juyo (active)");
			}
		} else {
			if(uarm && !(is_light_armor(uarm))){
				Sprintf(buf,	"Juyo (blocked by armor)");
			} else {
				Sprintf(buf,	"Juyo");
			}
		}
		any.a_int = FFORM_JUYO;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			incntlet, 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	}
	end_menu(tmpwin,	"Choose fighting style:");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	
	if(n <= 0 || activeFightingForm(selected[0].item.a_int)){
		return 0;
	} else {
		setFightingForm(selected[0].item.a_int);
		return 0;
	}
}

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
		return 0;
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
		return 0;
	} else {
		pline("You cease to maintain %s.",
			  OBJ_NAME(objects[selected[0].item.a_int]));
		spell_unmaintain(selected[0].item.a_int);
		return 0;
	}
}

STATIC_PTR int
enter_explore_mode()
{
#ifdef PARANOID
	char buf[BUFSZ];
	int really_xplor = FALSE;
#endif
	pline("Explore mode is for local games, not public servers.");
	return 0;

	if(!discover && !wizard) {
		pline("Beware!  From explore mode there will be no return to normal game.");
#ifdef PARANOID
		if (iflags.paranoid_quit) {
		  getlin ("Do you want to enter explore mode? [yes/no]?",buf);
		  (void) lcase (buf);
		  if (!(strcmp (buf, "yes"))) really_xplor = TRUE;
		} else {
		  if (yn("Do you want to enter explore mode?") == 'y') {
		    really_xplor = TRUE;
		  }
		}
		if (really_xplor) {
#else
		if (yn("Do you want to enter explore mode?") == 'y') {
#endif
			clear_nhwindow(WIN_MESSAGE);
			You("are now in non-scoring explore mode.");
			discover = TRUE;
		}
		else {
			clear_nhwindow(WIN_MESSAGE);
			pline("Resuming normal game.");
		}
	}
	return 0;
}

STATIC_PTR int
dooverview_or_wiz_where()
{
#ifdef WIZARD
	if (wizard) return wiz_where();
	else
#endif
	dooverview();
	return 0;
}

#ifdef WIZARD

STATIC_PTR int
wiz_mk_mapglyphdump()
{
#ifdef MAPDUMP_FN
    mk_mapdump(MAPDUMP_FN);
#endif
    return 0;
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
	return 0;
}

/* ^I command - identify hero's inventory */
STATIC_PTR int
wiz_identify()
{
	if (wizard)	identify_pack(0);
	else		pline("Unavailable command '^I'.");
	return 0;
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
	return 0;
}

/* ^G command - generate monster(s); a count prefix will be honored */
STATIC_PTR int
wiz_genesis()
{
	if (wizard)	(void) create_particular(-1, -1, TRUE, 0, 0, 0);
	else		pline("Unavailable command '^G'.");
	return 0;
}

/* ^O command - display dungeon layout */
STATIC_PTR int
wiz_where()
{
	if (wizard) (void) print_dungeon(FALSE, (schar *)0, (int *)0);
	else	    pline("Unavailable command '^O'.");
	return 0;
}

/* ^E command - detect unseen (secret doors, traps, hidden monsters) */
STATIC_PTR int
wiz_detect()
{
	if(wizard)  (void) findit();
	else	    pline("Unavailable command '^E'.");
	return 0;
}

/* ^V command - level teleport */
STATIC_PTR int
wiz_level_tele()
{
	if (wizard)	level_tele();
	else		pline("Unavailable command '^V'.");
	return 0;
}

/* #monpolycontrol command - choose new form for shapechangers, polymorphees */
STATIC_PTR int
wiz_mon_polycontrol()
{
    iflags.mon_polycontrol = !iflags.mon_polycontrol;
    pline("Monster polymorph control is %s.",
	  iflags.mon_polycontrol ? "on" : "off");
    return 0;
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
	return 0;
    }
    if (newlevel == u.ulevel) {
	You("are already that experienced.");
    } else if (newlevel < u.ulevel) {
	if (u.ulevel == 1) {
	    You("are already as inexperienced as you can get.");
	    return 0;
	}
	if (newlevel < 1) newlevel = 1;
	while (u.ulevel > newlevel)
	    losexp("#levelchange",TRUE,TRUE,TRUE);
    } else {
	if (u.ulevel >= MAXULEV) {
	    You("are already as experienced as you can get.");
	    return 0;
	}
	if (newlevel > MAXULEV) newlevel = MAXULEV;
	while (u.ulevel < newlevel)
	    pluslvl(FALSE);
    }
    u.ulevelmax = u.ulevel;
    return 0;
}

/* #panic command - test program's panic handling */
STATIC_PTR int
wiz_panic()
{
	if (iflags.debug_fuzzer) {
        	u.uhp = u.uhpmax = 1000;
      		u.uen = u.uenmax = 1000;
        	return 0;
	}

	if (yn("Do you want to call panic() and end your game?") == 'y')
		panic("crash test.");
        return 0;
}

/* #polyself command - change hero's form */
STATIC_PTR int
wiz_polyself()
{
        polyself(TRUE);
        return 0;
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
	return 0;
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
	return 0;
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
	return 0;
}

/* #showkills command */
STATIC_PTR int wiz_showkills()		/* showborn patch */
{
	list_vanquished('y', FALSE);
	return 0;
}

STATIC_PTR int wiz_setinsight()
{
	char buf[BUFSZ];
	int newval;
	int ret;

	getlin("Set your insight to what?", buf);

	(void)mungspaces(buf);
	if (buf[0] == '\033' || buf[0] == '\0') ret = 0;
	else ret = sscanf(buf, "%d", &newval);

	if (ret != 1) {
		pline1(Never_mind);
		return 0;
	}
	change_uinsight(newval - u.uinsight);
	return 0;
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
		return 0;
	}
	change_usanity(newval - u.usanity);
	return 0;
}

#endif /* WIZARD */


/* -enlightenment and conduct- */
static winid en_win;
static const char
	You_[] = "You ",
	are[]  = "are ",  were[]  = "were ",
	have[] = "have ", had[]   = "had ",
	can[]  = "can ",  could[] = "could ";
static const char
	have_been[]  = "have been ",
	have_never[] = "have never ", never[] = "never ";

#define enl_msg(prefix,present,past,suffix) \
			enlght_line(prefix, final ? past : present, suffix)
#define you_are(attr)	enl_msg(You_,are,were,attr)
#define you_have(attr)	enl_msg(You_,have,had,attr)
#define you_can(attr)	enl_msg(You_,can,could,attr)
#define you_have_been(goodthing) enl_msg(You_,have_been,were,goodthing)
#define you_have_never(badthing) enl_msg(You_,have_never,never,badthing)
#define you_have_X(something)	enl_msg(You_,have,(const char *)"",something)

static void
enlght_line(start, middle, end)
const char *start, *middle, *end;
{
	char buf[BUFSZ];

	Sprintf(buf, "%s%s%s.", start, middle, end);
	putstr(en_win, 0, buf);
}

/* format increased damage or chance to hit */
static char *
enlght_combatinc(inctyp, incamt, final, outbuf)
const char *inctyp;
int incamt, final;
char *outbuf;
{
	char numbuf[24];
	const char *modif, *bonus;

	if (final
#ifdef WIZARD
		|| wizard
#endif
	  ) {
	    Sprintf(numbuf, "%s%d",
		    (incamt > 0) ? "+" : "", incamt);
	    modif = (const char *) numbuf;
	} else {
	    int absamt = abs(incamt);

	    if (absamt <= 3) modif = "small";
	    else if (absamt <= 6) modif = "moderate";
	    else if (absamt <= 12) modif = "large";
	    else modif = "huge";
	}
	bonus = (incamt > 0) ? "bonus" : "penalty";
	/* "bonus to hit" vs "damage bonus" */
	if (!strcmp(inctyp, "damage") || !strcmp(inctyp, "spell damage")) {
	    const char *ctmp = inctyp;
	    inctyp = bonus;
	    bonus = ctmp;
	}
	Sprintf(outbuf, "%s %s %s", an(modif), bonus, inctyp);
	return outbuf;
}

void
enlightenment(final)
int final;	/* 0 => still in progress; 1 => over, survived; 2 => dead */
{
	int ltmp;
	char buf[BUFSZ];
	char prebuf[BUFSZ];

	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, final ? "Final Attributes:" : "Current Attributes:");
	putstr(en_win, 0, "");

	if (u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[42] = {
				/* Default */
				"the Arm of the Law",		 /*01*/
				"the Envoy of Balance",		 /*02*/
				"the Glory of Arioch",		 /*03*/
				/* Monk */
				"the Sage of Law",			 /*04*/
				"the Grandmaster of Balance",/*05*/
				"the Glory of Eequor",		 /*06*/
				/* Noble (human, vampire, incant) */
				"the Saint %s",				 /*07*/
				"the Grey Saint",			 /*08*/
				"the Dark %s",				 /*09*/
				/* Wizard */
				"the Magister of Law",		 /*10*/
				"the Wizard of Balance",	 /*11*/
				"the Glory of Chardros",	 /*12*/
				/* Elf */
				"the Hand of Elbereth",		 /*13*/
				"the Doomspeaker of Vaire",	 /*14*/
				"the Whisperer of Este",	 /*15*/
				/* Drow */
				"the Hand of Eilistraee",	 /*16*/
				"the Hand of Kiaransali",	 /*17*/
				"the Hand of Lolth",		 /*18*/
				/* Hedrow */
				"the Shepherd of spiders",	 /*19*/
				"the Sword of Vhaeraun",	 /*20*/
				"the Fang of Lolth",		 /*21*/
				/* Drow Noble */
				"the Blade of Ver'tas",		 /*22*/
				"the Hand of Kiaransali",	 /*23*/
				"the Hand of Lolth",		 /*24*/
				/* Hedrow Noble */
				"the Sword of Selvetarm",	 /*25*/
				"the Hand of Keptolo",		 /*26*/
				"the Hammer of Ghaunadaur",	 /*27*/
				/* Ranger */
				"the High %s of Apollo",	 /*28*/
				"the High %s of Latona",	 /*29*/
				"the High %s of Diana",	 	 /*30*/
				/* Gnome Ranger */
				"the Great Slave-Vassal of Kurtulmak",	 /*31*/
				"the Thane of Garl Glittergold",	 /*32*/
				"the Claw of Urdlen",	 	 /*33*/
				/* Healer */
				"the Hand of Athena",		 /*34*/
				"the Messenger of Hermes",	 /*35*/
				"the Glory of Poseidon",	 /*36*/
				/*Archeologist*/
				"the Warrior of Quetzalcoatl",/*37*/
				"the Champion of Camaxtli",	  /*38*/
				"the Fire-bearer of Huhetotl",/*39*/
				/*Female Half Dragon Noble*/
				"the Dragon-slayer of Gwyn", /*40*/
				"the Guardian of the Old Lords",/*41*/
				"the Darkmoon Champion",	 /*42*/
				
				/* uhand_of_elbereth max == 63 */
	    };
		
	    if(Role_if(PM_EXILE)) you_are("the Emissary of Elements");
	    else if(Pantheon_if(PM_PIRATE) || Role_if(PM_PIRATE)) you_are("the Pirate King");
	    else if((Pantheon_if(PM_VALKYRIE) || Role_if(PM_VALKYRIE)) && flags.initgend) you_are("the Daughter of Skadi");
	    else if(Race_if(PM_DWARF) && (urole.ldrnum == PM_THORIN_II_OAKENSHIELD || urole.ldrnum == PM_DAIN_II_IRONFOOT)){
			if(urole.ldrnum == PM_THORIN_II_OAKENSHIELD) you_are("King under the Mountain");
			else if(urole.ldrnum == PM_DAIN_II_IRONFOOT) you_are("Lord of Moria");
	    } else if((Pantheon_if(PM_KNIGHT) || Role_if(PM_KNIGHT)) && u.uevent.uhand_of_elbereth == 1) you_are("the King of the Angles");
	    else if((Pantheon_if(PM_SAMURAI) || Role_if(PM_SAMURAI)) && u.uevent.uhand_of_elbereth == 1){
			strcpy(buf, "Nasu no ");
			strcat(buf, plname);
			you_are(buf);
		} else if(u.uevent.uhand_of_elbereth >= 28 && u.uevent.uhand_of_elbereth <= 30){
			Sprintf(buf, hofe_titles[u.uevent.uhand_of_elbereth - 1], flags.female ? "Priestess" : "Priest");
			enl_msg("You ", "are ", "were ", buf);
		} else if(Role_if(PM_NOBLEMAN) && !Race_if(PM_DROW)){
			if(u.uevent.uhand_of_elbereth == 9) Sprintf(buf, hofe_titles[u.uevent.uhand_of_elbereth - 1], flags.female ? "Lady" : "Lord");
			else if(u.uevent.uhand_of_elbereth == 7) Sprintf(buf, hofe_titles[u.uevent.uhand_of_elbereth - 1], flags.female ? "Queen" : "King");
			else Sprintf(buf, " %s", hofe_titles[u.uevent.uhand_of_elbereth - 1]);
			enl_msg("You ", "are ", "were ", buf);
		} else you_are(hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}
	
	if(u.lastprayed){
		Sprintf(buf, "You last %s %ld turns ago", u.lastprayresult==PRAY_GIFT ? "received a gift" :
												  u.lastprayresult==PRAY_ANGER ? "angered your god" : 
												  u.lastprayresult==PRAY_CONV ? "converted to a new god" : 
												  "prayed",
			moves - u.lastprayed);
		putstr(en_win, 0, buf);
		if(u.lastprayresult==PRAY_GOOD){
			Sprintf(buf, "That prayer was well received");
			putstr(en_win, 0, buf);
		} else if(u.lastprayresult==PRAY_BAD){
			Sprintf(buf, "That prayer was poorly received");
			putstr(en_win, 0, buf);
		}
		if(u.reconciled){
			if(u.reconciled==REC_REC){
				Sprintf(buf, " since reconciled with your god");
				enl_msg("You ", "have","had",buf);
			} else if(u.reconciled==REC_MOL){
				Sprintf(buf, " since mollified your god");
				enl_msg("You ", "have","had",buf);
			}
		}
	}
	
	/* note: piousness 20 matches MIN_QUEST_ALIGN (quest.h) */
	if (u.ualign.record >= 20)	you_are("piously aligned");
	else if (u.ualign.record > 13)	you_are("devoutly aligned");
	else if (u.ualign.record > 8)	you_are("fervently aligned");
	else if (u.ualign.record > 3)	you_are("stridently aligned");
	else if (u.ualign.record == 3)	you_are("aligned");
	else if (u.ualign.record > 0)	you_are("haltingly aligned");
	else if (u.ualign.record == 0)	you_are("nominally aligned");
	else if (u.ualign.record >= -3)	you_have("strayed");
	else if (u.ualign.record >= -8)	you_have("sinned");
	else you_have("transgressed");
#ifdef WIZARD
	if (wizard) {
		Sprintf(buf, "%ld gold ", u.spawnedGold);
		enl_msg(buf, "has been", "was", " created");
		Sprintf(buf, " %d", u.ualign.record);
		enl_msg("Your alignment ", "is", "was", buf);
		Sprintf(buf, " %d sins", u.ualign.sins);
		enl_msg("You ", "carry", "carried", buf);
		Sprintf(buf, " %d", (int) ALIGNLIM);
		enl_msg("Your max alignment ", "is", "was", buf);
		if(flags.stag) enl_msg("You ", "have","had"," turned stag on your quest leader");
		else enl_msg("You ", "have","had"," stayed true to your quest");
		if(flags.leader_backstab) enl_msg("You ", "have","had"," been betrayed by your quest leader");
		if(u.hod){
			Sprintf(buf, "a hod wantedness of %d", u.hod);
			you_have(buf);
		}
		if(u.gevurah){
			Sprintf(buf, "a gevurah wantedness of %d", u.gevurah);
			you_have(buf);
		}
		if(u.keter){
			Sprintf(buf, "a chokhmah wantedness of %d", u.keter);
			you_have(buf);
		}
		if(u.chokhmah){
			Sprintf(buf, "%d chokhmah sephiroth ", u.chokhmah);
			enl_msg(buf, "are", "were", " deployed");
		}
		if(u.ustdy){
			Sprintf(buf, "%d weakness from being studied", u.ustdy);
			you_have(buf);
		}
		if(u.sealCounts){
			Sprintf(buf, "spirits bound: %d", u.sealCounts);
			you_have(buf);
		}
		if(u.sealsActive){
			Sprintf(buf, "seals active: %lx", u.sealsActive);
			you_have(buf);
		}
		if(u.specialSealsActive){
			Sprintf(buf, "special seals active: %lx", u.specialSealsActive);
			you_have(buf);
		}
	}
#endif
	
	if(u.sealsActive || u.specialSealsActive){
		int i,j,numBound,numFound=0;
		numBound = u.sealCounts;
		if(u.spirit[QUEST_SPIRIT]) numBound++;
		if(Role_if(PM_EXILE) && u.uevent.uhand_of_elbereth) numBound++;
		if(u.spirit[CROWN_SPIRIT]) numBound++;
		if(u.spirit[GPREM_SPIRIT]) numBound++;
		if(u.spirit[ALIGN_SPIRIT]) numBound++;
		if(u.spirit[OUTER_SPIRIT]) numBound++;
		if(Role_if(PM_ANACHRONONAUT) && u.specialSealsActive&SEAL_BLACK_WEB) numBound++;
		Sprintf(prebuf, "Your soul ");
		Sprintf(buf, " bound to ");
		for(i=0;i<QUEST_SPIRIT;i++){
			if(u.spirit[i]) for(j=0;j<32;j++){
				if((u.spirit[i] >> j) == 1){
					Strcat(buf,sealNames[j]);
					numFound++;
					if(numBound==2 && numFound==1) Strcat(buf," and ");
					else if(numBound>=3){
						if(numFound<numBound-1) Strcat(buf,", ");
						if(numFound==numBound-1) Strcat(buf,", and ");
					}
					break;
				}
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_DAHLVER_NAR){
			Strcat(buf, sealNames[(DAHLVER_NAR) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_ACERERAK){
			Strcat(buf, sealNames[(ACERERAK) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_COUNCIL){
			Strcat(buf, sealNames[(COUNCIL) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.spirit[CROWN_SPIRIT]) for(j=0;j<32;j++){
			if((u.spirit[CROWN_SPIRIT] >> j) == 1){
				Strcat(buf,sealNames[j]);
				numFound++;
				if(numBound==2 && numFound==1) Strcat(buf," and ");
				else if(numBound>=3){
					if(numFound<numBound-1) Strcat(buf,", ");
					if(numFound==numBound-1) Strcat(buf,", and ");
				}
				break;
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_COSMOS){
			Strcat(buf, sealNames[(COSMOS) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_LIVING_CRYSTAL){
			Strcat(buf, sealNames[(LIVING_CRYSTAL) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_TWO_TREES){
			Strcat(buf, sealNames[(TWO_TREES) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_MISKA){
			Strcat(buf, sealNames[(MISKA) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_NUDZIRATH){
			Strcat(buf, sealNames[(NUDZIRATH) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_ALIGNMENT_THING){
			Strcat(buf, sealNames[(ALIGNMENT_THING) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_UNKNOWN_GOD){
			Strcat(buf, sealNames[(UNKNOWN_GOD) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_BLACK_WEB){
			Strcat(buf, sealNames[(BLACK_WEB) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_NUMINA){
			Strcat(buf, sealNames[(NUMINA) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		enl_msg(prebuf, "is", "was", buf);
	}
	if(u.sealsKnown || u.specialSealsKnown){
		int numSlots;
		if(Role_if(PM_EXILE)){
			if(u.ulevel <= 2) numSlots=1;
			else if(u.ulevel <= 9) numSlots=2;
			else if(u.ulevel <= 18) numSlots=3;
			else if(u.ulevel <= 25) numSlots=4;
			else numSlots=5;
		} else {
			numSlots=1;
		}
		if(!u.spirit[QUEST_SPIRIT] && u.specialSealsKnown&(SEAL_DAHLVER_NAR|SEAL_ACERERAK|SEAL_BLACK_WEB)){
			you_are("able to bind with a quest spirit");
		}
		if(!u.spirit[ALIGN_SPIRIT] && u.specialSealsKnown&(SEAL_COSMOS|SEAL_LIVING_CRYSTAL|SEAL_TWO_TREES|SEAL_MISKA|SEAL_NUDZIRATH|SEAL_ALIGNMENT_THING|SEAL_UNKNOWN_GOD)){
			you_are("able to bind with an aligned spirit");
		}
		if(!u.spirit[OUTER_SPIRIT] && u.ulevel == 30 && Role_if(PM_EXILE)){
			you_are("able to bind with the Numina");
		}
		if(u.sealCounts < numSlots){
			Sprintf(prebuf, "You ");
			Sprintf(buf, " bind to %d more spirit%s", numSlots-u.sealCounts, (numSlots-u.sealCounts)>1 ? "s" : "");
			enl_msg(prebuf, "can", "could", buf);
		}
	}

	/*** Resistances to troubles ***/
	if (Acid_resistance) you_are("acid resistant");
	if (Cold_resistance) you_are("cold resistant");
	if (Disint_resistance) you_are("disintegration-resistant");
	if (Fire_resistance) you_are("fire resistant");
	if (Halluc_resistance)
		you_are("resistant to hallucinations");
	if (Invulnerable) you_are("invulnerable");
	if (Drain_resistance) you_are("level-drain resistant");
	if (Antimagic) you_are("magic-protected");
	if (Nullmagic) you_are("shrouded in anti-magic");
	if (Waterproof) you_are("waterproof");
	if (Stone_resistance)
		you_are("petrification resistant");
	if (Poison_resistance) you_are("poison resistant");
	if (Shock_resistance) you_are("shock resistant");
	if (Sick_resistance) you_are("immune to sickness");
	if (Sleep_resistance) you_are("sleep resistant");
	if (u.uedibility || u.sealsActive&SEAL_BUER) you_can("recognize detrimental food");
	if ( (ublindf && ublindf->otyp == R_LYEHIAN_FACEPLATE && !ublindf->cursed) || 
		 (uarmc && uarmc->otyp == OILSKIN_CLOAK && !uarmc->cursed) ||
		 (u.sealsActive&SEAL_ENKI) || (u.ufirst_sky)
	) you_are("waterproof");
	Sprintf(buf, "a drunkard score of %d", u.udrunken);
	if(u.udrunken >= u.ulevel*3) Sprintf(eos(buf), ", the maximum for an adventurer of your level");
	you_have(buf);
	
	/*** Thoughts ***/
	if (active_glyph(CLOCKWISE_METAMORPHOSIS)){
		enl_msg("A clockwise gyre ", "increases", "increased", " your HP by 30%");
	}
	if (active_glyph(ANTI_CLOCKWISE_METAMORPHOSIS)){
		enl_msg("An anti-clockwise gyre ", "increases", "increased", " your speed by 25%");
	}
	if (active_glyph(ARCANE_BULWARK)){
		enl_msg("A sparkling lake ", "shields", "shielded", " you from magic");
	}
	if (active_glyph(DISSIPATING_BULWARK)){
		enl_msg("A pure lake ", "shields", "shielded", " you from electricity");
	}
	if (active_glyph(SMOLDERING_BULWARK)){
		enl_msg("A still lake ", "shields", "shielded", " you from fire");
	}
	if (active_glyph(FROSTED_BULWARK)){
		enl_msg("A calm lake ", "shields", "shielded", " you from cold");
	}
	if (active_glyph(BLOOD_RAPTURE)){
		enl_msg("A mist of blood ", "causes", "caused", " sneak attacks to heal you by 30 HP");
	}
	if (active_glyph(CLAWMARK)){
		enl_msg("The clawmarks ", "strengthen", "strengthened", " your sneak attacks by 30%");
	}
	if (active_glyph(CLEAR_DEEPS)){
		enl_msg("The deep blue waters ", "protect", "protected", " you from poison");
	}
	if (active_glyph(DEEP_SEA)){
		enl_msg("The pitch-black waters ", "reduce", "reduced", " physical damage by 3");
	}
	if (active_glyph(COMMUNION)){
		enl_msg("The strange minister's prayer ", "increases", "increased", " your carry capacity by 25%");
	}
	if (active_glyph(CORRUPTION)){
		enl_msg("The tears of blood ", "heal", "healed", " you when below 30% HP");
	}
	if (active_glyph(EYE_THOUGHT)){
		enl_msg("The eyes inside your head ", "doubles", "doubled", " the number of death-drops");
	}
	if (active_glyph(FORMLESS_VOICE)){
		enl_msg("The great voice ", "increases", "increased", " your magic energy by 30%");
	}
	if (active_glyph(GUIDANCE)){
		enl_msg("The dancing sprites ", "lets", "let", " you heal by attacking those who attacked you");
	}
	if (active_glyph(IMPURITY)){
		enl_msg("The impure millipedes ", "reduce", "reduced", " physical damage for your companions by 3");
	}
	if (active_glyph(MOON)){
		enl_msg("The sympathetic moon ", "increases", "increased", " your earned experience by 30%");
	}
	if (active_glyph(WRITHE)){
		enl_msg("The subtle mucus in your brain ", "causes", "caused", " sneak attacks to restore 30 energy");
	}
	if (active_glyph(RADIANCE)){
		enl_msg("The golden pyramid ", "increases", "increased", " magical healing by 30%");
	}
	if (active_glyph(BEASTS_EMBRACE)){
		enl_msg("The hidden figure inside of you ", "lets", "let", " you succumb to the inner beast");
	}
	
	/*** Troubles ***/
	Sprintf(buf, "%d sanity points", u.usanity);
	you_have(buf);
	Sprintf(buf, "%d insight points", u.uinsight);
	you_have(buf);
	
	/*** Madnesses ***/
	if(u.usanity < 100 && !ClearThoughts){
		if (u.umadness&MAD_DELUSIONS){
			you_have("a tendency to hallucinate, obscuring some monsters true forms");
		}
		if(u.usanity < 80 && u.umadness&MAD_REAL_DELUSIONS){
			enl_msg("Some monsters ", "will change", "changed", " forms randomly");
		}
		if (u.umadness&MAD_SANCTITY){
			enl_msg("Sometimes, you ", "will fail", "failed", " to attack female humanoids and centaurs");
			enl_msg("Sometimes, female humanoids and centaurs ", "will take", "took", " reduced damage from your magic");
		}
		if (u.umadness&MAD_GLUTTONY){
			you_have("increased hunger");
		}
		if (u.umadness&MAD_SPORES&& !Race_if(PM_ANDROID) && !Race_if(PM_CLOCKWORK_AUTOMATON)){
			enl_msg("You ", "will periodically suffer", "periodically suffered", " hallucinations, confusion, stunning, damage, and ability score drain");
		}
		if (u.umadness&MAD_FRIGOPHOBIA){
			enl_msg("Sometimes, you ", "will panic and lose turns", "panicked and lost turns", " after taking cold damage or moving over ice");
		}
		if (u.umadness&MAD_CANNIBALISM){
			enl_msg("Sometimes, you ", "will vomit", "vomited", " after eating vegetarian or vegan food");
			enl_msg("Sometimes, you ", "will not be", "weren't", " warned before committing cannibalism");
		}
		if (u.umadness&MAD_RAGE){
			you_have("reduced AC, reduced spell success, and increased damage");
		}
		if (u.umadness&MAD_ARGENT_SHEEN){
			enl_msg("Sometimes, monsters ", "will panic and lose turns", "panicked and lost turns", " will gain reflection for a turn");
			enl_msg("Sometimes, monsters ", "will take", "took", " reduced damage from your magic");
			enl_msg("Sometimes, you ", "will stop", "stopped", " to admire yourself in mirrors, losing turns");
			enl_msg("You ", "take", "took", " increased damage from male humanoids and centaurs");
		}
		if (u.umadness&MAD_SUICIDAL){
			enl_msg("Sometimes, you ", "will have", "had", " greatly reduced AC");
			enl_msg("You ", "take", "took", " increased damage from all attacks");
		}
		if (u.umadness&MAD_NUDIST){
			enl_msg("You ", "suffer", "suffered", " reduced AC, DR, accuracy, healing, power regeneration, and spell success for each piece of equipment.");
		}
		if (u.umadness&MAD_OPHIDIOPHOBIA){
			enl_msg("Sometimes, you ", "will fail", "failed", " to attack serpentine monsters");
			enl_msg("Sometimes, you ", "will panic and lose turns", "panicked and lost turns", " after being poisoned");
			enl_msg("You ", "take", "took", " increased damage from serpentine monsters");
		}
		if (u.umadness&MAD_ARACHNOPHOBIA){
			//Note: Arachnophobia == women and spiders because it's the madness Lolth inflicts.
			enl_msg("Sometimes, female humanoids, centaurs, and spiders ", "will take", "took", " reduced damage from your magic");
			enl_msg("Sometimes, you ", "will fail", "failed", " to attack female humanoids, centaurs, and spiders");
			enl_msg("You ", "take", "took", " increased damage from spiders and from female humanoids and centaurs");
		}
		if (u.umadness&MAD_ENTOMOPHOBIA){
			enl_msg("Sometimes, you ", "will fail", "failed", " to attack insects and arachnids");
			enl_msg("You ", "take", "took", " increased damage from insects and arachnids");
		}
		if (u.umadness&MAD_THALASSOPHOBIA){
			enl_msg("Sometimes, you ", "will fail", "failed", " to cause much harm to aquatic monsters");
			enl_msg("Sometimes, aquatic monsters ", "will take", "took", " reduced damage from your magic");
			enl_msg("Sometimes, you ", "will fail", "failed", " to attack aquatic monsters");
			enl_msg("You ", "take", "took", " increased damage from aquatic monsters");
			enl_msg("Sometimes, you ", "will panic and lose turns", "panicked and lost turns", " after being attacked by aquatic monsters");
		}
		if (u.umadness&MAD_PARANOIA){
			//Severe because it triggers much more frequently than other madnesses
			enl_msg("Sometimes, you ", "will attack", "attacked", " monsters' hallucinatory twins instead");
			you_have("a hard time discerning the location of unseen monsters");
		}
		if (u.umadness&MAD_TALONS){
			enl_msg("Sometimes, you ", "will panic and lose turns", "panicked and lost turns", " losing or releasing an item");
		}
		if (u.umadness&MAD_COLD_NIGHT){
			enl_msg("Sometimes, you ", "slip", "slipped", " nonexistent ice");
			enl_msg("Sometimes, the air ", "will freeze", "froze", " solid, causing you to suffocate and destroying your potions");
		}
		if (u.umadness&MAD_OVERLORD){
			enl_msg("Sometimes, you ", "feel", "felt", " the burning gaze of the Overlord, weakening you and burning your possessions");
		}
		if (u.umadness&MAD_DREAMS){
			enl_msg("Sometimes, you ", "pass out", "passed out", " and dream of strange cities, suffering damage, stunning, and reduced sanity");
		}
		if (u.umadness&MAD_HELMINTHOPHOBIA){
			enl_msg("Sometimes, you ", "will fail", "failed", " to attack worms and tentacled monsters");
			enl_msg("You ", "take", "took", " increased damage from worms and tentacles");
		}
		if(has_blood(youracedata)){
			if (u.umadness&MAD_FRENZY){
				Sprintf(buf, "your %s seethe below your %s", body_part(BLOOD), body_part(BODY_SKIN));
				enl_msg("You ", "feel ", "felt ", buf);
			}
		}
	}
	
	/*** More Troubles ***/
	if (final) {
		if (Hallucination) you_are("hallucinating");
		if (Stunned) you_are("stunned");
		if (Confusion) you_are("confused");
		if (Blinded) you_are("blinded");
		if (Sick) {
			if (u.usick_type & SICK_VOMITABLE)
				you_are("sick from food poisoning");
			if (u.usick_type & SICK_NONVOMITABLE)
				you_are("sick from illness");
		}
#ifdef CONVICT
        if (Punished) {
            you_are("punished");
        }
#endif /* CONVICT */
	}
	if (Stoned) you_are("turning to stone");
	if (Golded) you_are("turning to gold");
	if (Slimed) you_are("turning into slime");
	if (FrozenAir) you_are("suffocating in the cold night");
	if (Strangled) you_are((u.uburied) ? "buried" : "being strangled");
	if (Glib) {
		Sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
		you_have(buf);
	}
	if (Fumbling) enl_msg("You fumble", "", "d", "");
	if(u_healing_penalty()) enl_msg("You heal", "", "ed", " slowly due to your equipment");
	if (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  ) {
		Sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
		you_have(buf);
	}
#if defined(WIZARD) && defined(STEED)
	if (Wounded_legs && u.usteed && wizard) {
	    Strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
	    *buf = highc(*buf);
	    enl_msg(buf, " has", " had", " wounded legs");
	}
#endif
	if (Sleeping) enl_msg("You ", "fall", "fell", " asleep");
	if (Hunger) enl_msg("You hunger", "", "ed", " rapidly");
	if(u.wimage >= 10){
		if(ACURR(A_WIS) < 6){
			Sprintf(buf, " filled with the image of a weeping angel");
			enl_msg("Your mind ", "is","was",buf);
		} else if(ACURR(A_WIS) < 9){
			Sprintf(buf, " ever on your mind");
			enl_msg("The image of a weeping angel ", "is","was",buf);
		} else if(ACURR(A_WIS) < 12){
			Sprintf(buf, " seem to shake the image of a weeping angel from your mind");
			enl_msg("You ", "can't","couldn't",buf);
		} else {
			Sprintf(buf, " in the back of your mind");
			enl_msg("The image of a weeping angel ", "lurks","lurked",buf);
		}
	}
	if (u.umorgul) enl_msg("You ", "feel", "felt", " deathly cold");

	/*** Vision and senses ***/
	if (See_invisible(u.ux,u.uy)) enl_msg(You_, "see", "saw", " invisible");
	if (Blind_telepat) you_are("telepathic");
	if (Warning) you_are("warned");
	if (Warn_of_mon && (flags.warntypem||flags.warntypet||flags.warntypeb||flags.warntypeg||flags.warntypea||flags.warntypev||flags.montype)) {
		if(flags.warntypea & MA_ANIMAL) you_are("aware of the presence of animals");
		if(flags.warntypea & MA_ARACHNID) you_are("aware of the presence of arachnids");
		if(flags.warntypea & MA_AVIAN) you_are("aware of the presence of birds");
		if(flags.warntypea & MA_DEMIHUMAN) you_are("aware of the presence of demihumans");
		if(flags.warntypea & MA_DEMON) you_are("aware of the presence of demons");
		if(flags.warntypea & MA_MINION) you_are("aware of the presence of divine beings");
		if(flags.warntypea & MA_DRAGON) you_are("aware of the presence of dragons");
		if(flags.warntypea & MA_DWARF) you_are("aware of the presence of dwarves");
		if(flags.warntypea & MA_ELEMENTAL) you_are("aware of the presence of elemental beings");
		if(flags.warntypea & MA_ELF) you_are("aware of the presence of elves");
		if(flags.warntypea & MA_FEY) you_are("aware of the presence of fair folk");
		if(flags.warntypea & MA_GIANT) you_are("aware of the presence of giants");
		if(flags.warntypea & MA_HUMAN) you_are("aware of the presence of humans");
		if(flags.warntypea & MA_INSECTOID) you_are("aware of the presence of insectoids");
		if(flags.warntypet & MT_MAGIC) you_are("aware of the presence of magic seekers");
		if(flags.warntypeg & MG_LORD) you_are("aware of the presence of nobles");
		if(flags.warntypea & MA_PRIMORDIAL) you_are("aware of the presence of old ones");
		if(flags.warntypea & MA_ORC) you_are("aware of the presence of orcs");
		if(flags.warntypea & MA_PLANT) you_are("aware of the presence of plants");
		if(flags.warntypea & MA_REPTILIAN) you_are("aware of the presence of reptiles");
		if(flags.warntypeg & MG_PRINCE) you_are("aware of the presence of rulers");
		if(flags.warntypea & MV_TELEPATHIC) you_are("aware of the presence of telepaths");
		if(flags.warntypea & MA_WERE) you_are("aware of the presence of werecreatures");
	}
	if (Searching) you_have("automatic searching");
	if (Clairvoyant) you_are("clairvoyant");
	if (Infravision) you_have("infravision");
	if (Detect_monsters) you_are("sensing the presence of monsters");

	/*** Appearance and behavior ***/
#ifdef WIZARD
	Sprintf(buf, "a carrying capacity of %d remaining", -1*inv_weight());
    you_have(buf);
	Sprintf(buf, "%d points of nutrition remaining", YouHunger);
    you_have(buf);
#endif
	if (Adornment) {
	    int adorn = 0;

	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
		you_are("poorly adorned");
	    else
		you_are("adorned");
	}
	if (Invisible) you_are("invisible");
	else if (Invis) you_are("invisible to others");
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((HInvis || EInvis || pm_invisible(youracedata)) && BInvis)
	    you_are("visible");
	if (Displaced) you_are("displaced");
	if (Stealth) you_are("stealthy");
	if (Aggravate_monster) enl_msg("You aggravate", "", "d", " monsters");
	if (Conflict) enl_msg("You cause", "", "d", " conflict");

	/*** Transportation ***/
	if (Jumping) you_can("jump");
	if (Teleportation) you_can("teleport");
	if (Teleport_control) you_have("teleport control");
	if (Lev_at_will) you_are("levitating, at will");
	else if (Levitation) you_are("levitating");	/* without control */
	else if (Flying) you_can("fly");
	if (Wwalking) you_can("walk on water");
	if (Swimming) you_can("swim");
	if (Breathless) you_can("survive without air");
	else if (Amphibious) you_can("breathe water");
	if (Passes_walls) you_can("walk through walls");
#ifdef STEED
	/* If you die while dismounting, u.usteed is still set.  Since several
	 * places in the done() sequence depend on u.usteed, just detect this
	 * special case. */
	if (u.usteed && (final < 2 || strcmp(killer, "riding accident"))) {
	    Sprintf(buf, "riding %s", y_monnam(u.usteed));
	    you_are(buf);
	}
#endif
	if (u.uswallow) {
	    Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%u)", u.uswldtim);
#endif
	    you_are(buf);
	} else if (u.ustuck) {
	    Sprintf(buf, "%s %s",
		    (sticks(&youmonst) ? "holding" : "held by"),
		    a_monnam(u.ustuck));
	    you_are(buf);
	}

	/*** Physical attributes ***/
	if (uclockwork){
		if(u.ucspeed==HIGH_CLOCKSPEED) you_are("set to high clockspeed");
		if(u.ucspeed==NORM_CLOCKSPEED) you_are("set to normal clockspeed");
		if(u.ucspeed==SLOW_CLOCKSPEED) you_are("set to low clockspeed");
		if(u.phasengn) you_are("in phase mode");
	}
	if (uandroid){
		if(u.ucspeed==HIGH_CLOCKSPEED) you_are("set to emergency speed");
		if(u.phasengn) you_are("in phase mode");
	}
	if (u.uhitinc || u.uuur_duration)
	    you_have(enlght_combatinc("to hit", u.uhitinc + (u.uuur_duration ? 10 : 0), final, buf));
	if (u.udaminc || (u.uaesh/3) || u.uaesh_duration)
	    you_have(enlght_combatinc("damage", u.udaminc+u.uaesh/3 + (u.uaesh_duration ? 10 : 0), final, buf));
	if(u.ukrau_duration){
		you_have("+50% to spell damage");
	}
	if (u.ukrau/3)
	    you_have(enlght_combatinc("spell damage", u.ukrau/3, final, buf));
	if (u.uhoon/3 || u.uhoon_duration)
	    you_have(enlght_combatinc("to healing", u.uhoon/3 + (u.uhoon_duration ? 30 : 0), final, buf));
	if (u.unaen/3 || u.unaen_duration)
	    you_have(enlght_combatinc("to energy regeneration", u.unaen/3 + (u.unaen_duration ? 30 : 0), final, buf));
	if (Slow_digestion) you_have("slower digestion");
	if (Regeneration) enl_msg("You regenerate", "", "d", "");
	if (u.uspellprot || Protection || u.uuur/2 || u.uuur_duration || (u.uvaul+4)/5) {
	    int prot = 0;

	    if(uleft && uleft->otyp == RIN_PROTECTION) prot += uleft->spe;
	    if(uright && uright->otyp == RIN_PROTECTION) prot += uright->spe;
	    if (HProtection & INTRINSIC) prot += u.ublessed;
	    prot += u.uspellprot;
	    prot += u.uuur/2;
	    prot += u.uuur_duration ? 10 : 0;
	    prot += (u.uvaul+4)/5;

	    if (prot < 0)
		you_are("ineffectively protected");
	    else
		you_are("protected");
	}
	if (Protection_from_shape_changers)
		you_are("protected from shape changers");
	if (Polymorph) you_are("polymorphing");
	if (Polymorph_control) you_have("polymorph control");
	if (u.ulycn >= LOW_PM && !umechanoid) {
		Strcpy(buf, an(mons[u.ulycn].mname));
		you_are(buf);
	}
	if (Upolyd) {
	    if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form");
	    else Sprintf(buf, "polymorphed into %s", an(youmonst.data->mname));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
	    you_are(buf);
	}
	if (Unchanging) you_can("not change from your current form");
	if (Fast) you_are(Very_fast ? "very fast" : "fast");
	if (Reflecting) you_have("reflection");
	if (Reflecting && (
			(uwep && is_lightsaber(uwep) && litsaber(uwep) && 
				((activeFightingForm(FFORM_SHIEN) && (!uarm || is_light_armor(uarm))) || 
				 (activeFightingForm(FFORM_SORESU) && (!uarm || is_light_armor(uarm) || is_medium_armor(uarm)))
				)
			) ||
			(uarm && (uarm->otyp == SILVER_DRAGON_SCALE_MAIL || uarm->otyp == SILVER_DRAGON_SCALES || uarm->otyp == SILVER_DRAGON_SCALE_SHIELD)) ||
			(uwep && uwep->oartifact == ART_DRAGONLANCE)
	)) you_have("dragon-breath reflection");
	if (Free_action) you_have("free action");
	if (Fixed_abil) you_have("fixed abilities");
	if (Lifesaved)
		enl_msg("Your life ", "will be", "would have been", " saved");
	if (u.twoweap) you_are("wielding two weapons at once");
	if (u.umconf) you_are("going to confuse monsters");

	/*** Miscellany ***/
	if (Spellboost) you_have("augmented spellcasting");
	if (Luck) {
	    ltmp = abs((int)Luck);
	    Sprintf(buf, "%s%slucky",
		    ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "",
		    Luck < 0 ? "un" : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", Luck);
#endif
	    you_are(buf);
	}
#ifdef WIZARD
	 else if (wizard) enl_msg("Your luck ", "is", "was", " zero");
#endif
	if (u.moreluck > 0) you_have("extra luck");
	else if (u.moreluck < 0) you_have("reduced luck");
	if (has_luckitem()) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
		enl_msg("Bad luck ", "times", "timed", " out slowly for you");
	    if (ltmp >= 0)
		enl_msg("Good luck ", "times", "timed", " out slowly for you");
	}

	if (u.ugangr[Align2gangr(u.ualign.type)]) {
	    Sprintf(buf, " %sangry with you",
		    u.ugangr[Align2gangr(u.ualign.type)] > 6 ? "extremely " : u.ugangr[Align2gangr(u.ualign.type)] > 3 ? "very " : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ugangr[Align2gangr(u.ualign.type)]);
#endif
	    enl_msg(u_gname(), " is", " was", buf);
	} else
	    /*
	     * We need to suppress this when the game is over, because death
	     * can change the value calculated by can_pray(), potentially
	     * resulting in a false claim that you could have prayed safely.
	     */
	  if (!final) {
#if 0
	    /* "can [not] safely pray" vs "could [not] have safely prayed" */
	    Sprintf(buf, "%s%ssafely pray%s", can_pray(FALSE) ? "" : "not ",
		    final ? "have " : "", final ? "ed" : "");
#else
	    Sprintf(buf, "%ssafely pray", can_pray(FALSE) ? "" : "not ");
#endif
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ublesscnt);
#endif
	    you_can(buf);
	}

    {
	const char *p;

	buf[0] = '\0';
	if (final < 2) {    /* still in progress, or quit/escaped/ascended */
	    p = "survived after being killed ";
	    switch (u.umortality) {
	    case 0:  p = !final ? (char *)0 : "survived";  break;
	    case 1:  Strcpy(buf, "once");  break;
	    case 2:  Strcpy(buf, "twice");  break;
	    case 3:  Strcpy(buf, "thrice");  break;
	    default: Sprintf(buf, "%d times", u.umortality);
		     break;
	    }
	} else {		/* game ended in character's death */
	    p = "are dead";
	    switch (u.umortality) {
	    case 0:  impossible("dead without dying?");
	    case 1:  break;			/* just "are dead" */
	    default: Sprintf(buf, " (%d%s time!)", u.umortality,
			     ordin(u.umortality));
		     break;
	    }
	}
	if (p) enl_msg(You_, "have been killed ", p, buf);
    }

	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
	return;
}

#ifdef DUMP_LOG
void
dump_enlightenment(final)
int final;
{
	int ltmp;
	char buf[BUFSZ];
	char buf2[BUFSZ];
	char prebuf[BUFSZ];
	const char *enc_stat[] = { /* copied from botl.c */
	     "",
	     "burdened",
	     "stressed",
	     "strained",
	     "overtaxed",
	     "overloaded"
	};
	char *youwere = "  You were ";
	char *youhave = "  You have ";
	char *youhad  = "  You had ";
	char *youcould = "  You could ";

	dump("", "Final attributes");

	if (u.uevent.uhand_of_elbereth) {
	    static const char * const hofe_titles[27] = {
				/* Default */
				"the Arm of the Law",		 /*01*/
				"the Envoy of Balance",		 /*02*/
				"the Glory of Arioch",		 /*03*/
				/* Monk */
				"the Sage of Law",			 /*04*/
				"the Grandmaster of Balance",/*05*/
				"the Glory of Eequor",		 /*06*/
				/* Noble (human, vampire, incant) */
				"the Saint %s",				 /*07*/
				"the Grey Saint",			 /*08*/
				"the Dark %s",				 /*09*/
				/* Wizard */
				"the Magister of Law",		 /*10*/
				"the Wizard of Balance",	 /*11*/
				"the Glory of Chardros",	 /*12*/
				/* Elf */
				"the Hand of Elbereth",		 /*13*/
				"the Doomspeaker of Vaire",	 /*14*/
				"the Whisperer of Este",	 /*15*/
				/* Drow */
				"the Hand of Eilistraee",	 /*16*/
				"the Hand of Kiaransali",	 /*17*/
				"the Hand of Lolth",		 /*18*/
				/* Hedrow */
				"the Shepherd of spiders",	 /*19*/
				"the Sword of Vhaeraun",	 /*20*/
				"the Fang of Lolth",		 /*21*/
				/* Drow Noble */
				"the Blade of Ver'tas",		 /*22*/
				"the Hand of Kiaransali",	 /*23*/
				"the Hand of Lolth",		 /*24*/
				/* Hedrow Noble */
				"the Sword of Selvetarm",	 /*25*/
				"the Hand of Keptolo",		 /*26*/
				"the Hammer of Ghaunadaur",	 /*27*/
				
				/* uhand_of_elbereth max == 31 */
	    };
	    if(Role_if(PM_EXILE)) dump(youwere,"the Emissary of Elements");
	    else if(Pantheon_if(PM_PIRATE) || Role_if(PM_PIRATE)) dump(youwere,"the Pirate King");
	    else if((Pantheon_if(PM_VALKYRIE) || Role_if(PM_VALKYRIE)) && flags.initgend) dump(youwere,"the Daughter of Skadi");
	    else if(Race_if(PM_DWARF) && (urole.ldrnum == PM_THORIN_II_OAKENSHIELD || urole.ldrnum == PM_DAIN_II_IRONFOOT)){
			if(urole.ldrnum == PM_THORIN_II_OAKENSHIELD) dump(youwere,"King under the Mountain");
			else if(urole.ldrnum == PM_DAIN_II_IRONFOOT) dump(youwere,"Lord of Moria");
	    } else if((Pantheon_if(PM_SAMURAI) || Role_if(PM_SAMURAI)) && u.uevent.uhand_of_elbereth == 1){
			strcpy(buf, "Nasu no ");
			strcat(buf, plname);
			dump(youwere,buf);
		} else if(Role_if(PM_NOBLEMAN) && !Race_if(PM_DROW)){
			if(u.uevent.uhand_of_elbereth == 9) Sprintf(buf, hofe_titles[u.uevent.uhand_of_elbereth - 1], flags.female ? "Lady" : "Lord");
			else if(u.uevent.uhand_of_elbereth == 7) Sprintf(buf, hofe_titles[u.uevent.uhand_of_elbereth - 1], flags.female ? "Queen" : "King");
			else Sprintf(buf, " %s", hofe_titles[u.uevent.uhand_of_elbereth - 1]);
			dump(youwere, buf);
		} else dump(youwere,
		(char *)hofe_titles[u.uevent.uhand_of_elbereth - 1]);
	}
	
	if(u.lastprayed){
		Sprintf(buf, "You last %s %ld turns ago", u.lastprayresult==PRAY_GIFT ? "recieved a gift" :
												  u.lastprayresult==PRAY_ANGER ? "angered your god" : 
												  u.lastprayresult==PRAY_CONV ? "converted to a new god" : 
												  "prayed",
			moves - u.lastprayed);
		dump("", buf);
		if(u.lastprayresult==PRAY_GOOD){
			Sprintf(buf, "That prayer was well recieved");
			dump("", buf);
		} else if(u.lastprayresult==PRAY_BAD){
			Sprintf(buf, "That prayer was poorly recieved");
			dump("", buf);
		}
		if(u.reconciled){
			if(u.reconciled==REC_REC){
				Sprintf(buf, " since reconciled with your god");
				dump(youhad,buf);
			} else if(u.reconciled==REC_MOL){
				Sprintf(buf, " since mollified your god");
				dump(youhad,buf);
			}
		}
	}
	
	Sprintf(buf, "%ld pieces of gold created in the game", u.spawnedGold);
	dump("  There were ", buf);
	
	/* note: piousness 20 matches MIN_QUEST_ALIGN (quest.h) */
	if (u.ualign.record >= 20)
		dump(youwere, "piously aligned");
	else if (u.ualign.record > 13)
	    dump(youwere, "devoutly aligned");
	else if (u.ualign.record > 8)
	    dump(youwere, "fervently aligned");
	else if (u.ualign.record > 3)
	    dump(youwere, "stridently aligned");
	else if (u.ualign.record == 3)
	    dump(youwere, "aligned");
	else if (u.ualign.record > 0)
	    dump(youwere, "haltingly aligned");
	else if (u.ualign.record == 0)
	    dump(youwere, "nominally aligned");
	else if (u.ualign.record >= -3)	dump(youhave, "strayed");
	else if (u.ualign.record >= -8)	dump(youhave, "sinned");
	else dump("  You have ", "transgressed");
	Sprintf(buf, " %d", u.ualign.record);
	dump("  Your alignment was ", buf);
	Sprintf(buf, " %d sins", u.ualign.sins);
	dump("  You carried", buf);
	if(flags.stag) dump(youhad,"turned stag on your quest leader");
	if(flags.leader_backstab) dump(youwere,"betrayed by your quest leader");
	Sprintf(buf, "a hod wantedness of %d", u.hod);
	dump(youhad,buf);
	Sprintf(buf, "a gevurah wantedness of %d", u.gevurah);
	dump(youhad,buf);
	Sprintf(buf, "a chokhmah wantedness of %d", u.keter);
	dump(youhad,buf);
	Sprintf(buf, "%d weakness from being studied", u.ustdy);
	dump(youhad,buf);

	if(u.sealsActive || u.specialSealsActive){
		int i,j,numBound,numFound=0;
		numBound = u.sealCounts;
		if(u.spirit[QUEST_SPIRIT]) numBound++;
		if(Role_if(PM_EXILE) && u.uevent.uhand_of_elbereth) numBound++;
		if(u.spirit[CROWN_SPIRIT]) numBound++;
		if(u.spirit[GPREM_SPIRIT]) numBound++;
		if(u.spirit[ALIGN_SPIRIT]) numBound++;
		if(u.spirit[OUTER_SPIRIT]) numBound++;
		Sprintf(prebuf, "Your soul was ");
		Sprintf(buf, "bound to ");
		for(i=0;i<QUEST_SPIRIT;i++){
			if(u.spirit[i]) for(j=0;j<32;j++){
				if((u.spirit[i] >> j) == 1){
					Strcat(buf,sealNames[j]);
					numFound++;
					if(numBound==2 && numFound==1) Strcat(buf," and ");
					else if(numBound>=3){
						if(numFound<numBound-1) Strcat(buf,", ");
						if(numFound==numBound-1) Strcat(buf,", and ");
					}
					break;
				}
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_DAHLVER_NAR){
			Strcat(buf, sealNames[(DAHLVER_NAR) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_ACERERAK){
			Strcat(buf, sealNames[(ACERERAK) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_COUNCIL){
			Strcat(buf, sealNames[(COUNCIL) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.spirit[CROWN_SPIRIT]) for(j=0;j<32;j++){
			if((u.spirit[CROWN_SPIRIT] >> j) == 1){
				Strcat(buf,sealNames[j]);
				numFound++;
				if(numBound==2 && numFound==1) Strcat(buf," and ");
				else if(numBound>=3){
					if(numFound<numBound-1) Strcat(buf,", ");
					if(numFound==numBound-1) Strcat(buf,", and ");
				}
				break;
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_COSMOS){
			Strcat(buf, sealNames[(COSMOS) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_LIVING_CRYSTAL){
			Strcat(buf, sealNames[(LIVING_CRYSTAL) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_TWO_TREES){
			Strcat(buf, sealNames[(TWO_TREES) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_MISKA){
			Strcat(buf, sealNames[(MISKA) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_NUDZIRATH){
			Strcat(buf, sealNames[(NUDZIRATH) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_ALIGNMENT_THING){
			Strcat(buf, sealNames[(ALIGNMENT_THING) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_UNKNOWN_GOD){
			Strcat(buf, sealNames[(UNKNOWN_GOD) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_BLACK_WEB){
			Strcat(buf, sealNames[(BLACK_WEB) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_NUMINA){
			Strcat(buf, sealNames[(NUMINA) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		dump(prebuf, buf);
	}
	if(u.sealsKnown || u.specialSealsKnown){
		int numSlots;
		if(Role_if(PM_EXILE)){
			if(u.ulevel <= 2) numSlots=1;
			else if(u.ulevel <= 9) numSlots=2;
			else if(u.ulevel <= 18) numSlots=3;
			else if(u.ulevel <= 25) numSlots=4;
			else numSlots=5;
		} else {
			numSlots=1;
		}
		if(!u.spirit[QUEST_SPIRIT] && u.specialSealsKnown&(SEAL_DAHLVER_NAR|SEAL_ACERERAK|SEAL_BLACK_WEB)){
			you_are("able to bind with a quest spirit");
		}
		if(!u.spirit[ALIGN_SPIRIT] && u.specialSealsKnown&(SEAL_LIVING_CRYSTAL|SEAL_TWO_TREES|SEAL_COSMOS|SEAL_MISKA|SEAL_NUDZIRATH|SEAL_ALIGNMENT_THING|SEAL_UNKNOWN_GOD)){
			you_are("able to bind with an aligned spirit");
		}
		if(!u.spirit[OUTER_SPIRIT] && u.ulevel == 30 && Role_if(PM_EXILE)){
			you_are("able to bind with the Numina");
		}
		if(u.sealCounts < numSlots){
			Sprintf(prebuf, "You could ");
			Sprintf(buf, " bind to %d more spirit%s", numSlots-u.sealCounts, (numSlots-u.sealCounts)>1 ? "s" : "");
			dump(prebuf, buf);
		}
	}


	/*** Resistances to troubles ***/
	if (Acid_resistance) dump(youwere, "acid resistant");
	if (Cold_resistance) dump(youwere, "cold resistant");
	if (Disint_resistance) dump(youwere, "disintegration-resistant");
	if (Fire_resistance) dump(youwere, "fire resistant");
	if (Halluc_resistance)
		dump(youwere, "resistant to hallucinations");
	if (Invulnerable) dump(youwere, "invulnerable");
	if (Drain_resistance) dump(youwere, "level-drain resistant");
	if (Antimagic) dump(youwere, "magic-protected");
	if (Nullmagic) dump(youwere, "shrouded in anti-magic");
	if (Waterproof) dump(youwere, "waterproof");
	if (Stone_resistance)
		dump(youwere, "petrification resistant");
	if (Poison_resistance) dump(youwere, "poison resistant");
	if (Shock_resistance) dump(youwere, "shock resistant");
	if (Sick_resistance) dump(youwere, "immune to sickness");
	if (Sleep_resistance) dump(youwere, "sleep resistant");
	if (u.uedibility || u.sealsActive&SEAL_BUER) dump(youcould, "recognize detrimental food");
	if ( (ublindf && ublindf->otyp == R_LYEHIAN_FACEPLATE && !ublindf->cursed) || 
		 (uarmc && uarmc->otyp == OILSKIN_CLOAK && !uarmc->cursed) ||
		 (u.sealsActive&SEAL_ENKI) || (u.ufirst_sky)
	) dump(youwere, "waterproof");

	/*** Thoughts ***/
	if (active_glyph(CLOCKWISE_METAMORPHOSIS))
		dump("  ", "A clockwise gyre increased your HP");
	if (active_glyph(ANTI_CLOCKWISE_METAMORPHOSIS))
		dump("  ", "An anti-clockwise gyre increased your speed");
	if (active_glyph(ARCANE_BULWARK))
		dump("  ", "A sparkling lake shielded you from magic");
	if (active_glyph(DISSIPATING_BULWARK))
		dump("  ", "A pure lake shielded you from electricity");
	if (active_glyph(SMOLDERING_BULWARK))
		dump("  ", "A still lake shielded you from fire");
	if (active_glyph(FROSTED_BULWARK))
		dump("  ", "A calm lake shielded you from cold");
	if (active_glyph(BLOOD_RAPTURE))
		dump("  ", "A mist of blood caused sneak attacks to heal you");
	if (active_glyph(CLAWMARK))
		dump("  ", "The clawmarks strengthened your sneak attacks");
	if (active_glyph(CLEAR_DEEPS))
		dump("  ", "The deep blue waters protected you from poison");
	if (active_glyph(DEEP_SEA))
		dump("  ", "The pitch-black waters reduced physical damage");
	if (active_glyph(COMMUNION))
		dump("  ", "The strange minister lifted your burdens");
	if (active_glyph(CORRUPTION))
		dump("  ", "The tears of blood healed you when you were badly injured");
	if (active_glyph(EYE_THOUGHT))
		dump("  ", "The eyes inside your head helped find items inside your foes");
	if (active_glyph(FORMLESS_VOICE))
		dump("  ", "The great voice increased your magic energy");
	if (active_glyph(GUIDANCE))
		dump("  ", "The dancing sprites let you heal by attacking those who attacked you");
	if (active_glyph(IMPURITY))
		dump("  ", "The impure millipedes reduced physical damage for your companions");
	if (active_glyph(MOON))
		dump("  ", "The sympathetic moon increased your earned experience");
	if (active_glyph(WRITHE))
		dump("  ", "The subtle mucus in your brain caused sneak attacks to restore your energy");
	if (active_glyph(RADIANCE))
		dump("  ", "The golden pyramid increased your healing");
	
	/*** Troubles ***/
	if(u.usanity < 100)
		dump("  ", "You were a little touched in the head");
	else if(u.usanity < 50)
		dump("  ", "You sometimes struggled with insanity");
	else if(u.usanity < 25)
		dump("  ", "You frequently struggled with insanity");
	else if(u.usanity < 10)
		dump("  ", "You constantly struggled with insanity");
	else if(u.usanity == 0)
		dump("  ", "You were quite insane");
	
	if(u.uinsight > 40)
		dump("  ", "You frequently saw things you wished you hadn't");
	else if(u.uinsight > 20)
		dump("  ", "You often saw things you wished you hadn't");
	else if(u.uinsight > 1)
		dump("  ", "You occasionally saw things you wished you hadn't");
	
	/*** Madnesses ***/
	if(u.usanity < 100 && !ClearThoughts){
		if (u.umadness&MAD_DELUSIONS){
			dump("  ", "You had a tendency to hallucinate");
		}
		if(u.usanity < 80){
			if (u.umadness&MAD_REAL_DELUSIONS){
				if(u.umadness&MAD_DELUSIONS)
					dump("  ", "...at least, you THOUGHT you were hallucinating...");
				else
					dump("  ", "You had a tendency to hallucinate... you thought");
			}
		}
		if (u.umadness&MAD_SANCTITY){
			dump("  ", "You had a tendency to treat women as delicate and holy beings who shouldn't be harmed");
		}
		if (u.umadness&MAD_GLUTTONY){
			dump("  ", "You had a mad hunger");
		}
		if (u.umadness&MAD_SPORES && !Race_if(PM_ANDROID) && !Race_if(PM_CLOCKWORK_AUTOMATON)){
			//Note: Race_if is correct because it works on your original brain (because magic)
			dump("  ", "Her spores ate your brain.");
		}
		if (u.umadness&MAD_FRIGOPHOBIA){
			dump("  ", "You had an irrational fear of the cold");
		}
		if (u.umadness&MAD_CANNIBALISM){
			dump("  ", "You had a mad desire to consume living flesh, even the flesh of your own kind");
		}
		if (u.umadness&MAD_RAGE){
			dump("  ", "You had a burning, irrational rage");
		}
		if (u.umadness&MAD_ARGENT_SHEEN){
			dump("  ", "The world was full of mirrors, and you couldn't help but admire yourself.");
		}
		if (u.umadness&MAD_SUICIDAL){
			dump("  ", "You had a tendency towards suicidal behavior (now satisfied)");
		}
		if (u.umadness&MAD_NUDIST){
			dump("  ", "You had an irrational dislike of clothing");
		}
		if (u.umadness&MAD_OPHIDIOPHOBIA){
			dump("  ", "You had an irrational fear of snakes");
		}
		if (u.umadness&MAD_ARACHNOPHOBIA){
			//Note: Arachnophobia == women and spiders because it's the madness Lolth inflicts.
			dump("  ", "You had an irrational fear of spiders");
			dump("  ", "You had an irrational fear of women");
		}
		if (u.umadness&MAD_ENTOMOPHOBIA){
			dump("  ", "You had an irrational fear of insects");
		}
		if (u.umadness&MAD_THALASSOPHOBIA){
			dump("  ", "You had an irrational fear of sea-monsters");
		}
		if (u.umadness&MAD_PARANOIA){
			//Severe because it triggers much more frequently than other madnesses
			dump("  ", "You had a severe tendency towards paranoia");
		}
		if (u.umadness&MAD_TALONS){
			dump("  ", "You had an irrational fear of loss");
		}
		if (u.umadness&MAD_COLD_NIGHT){
			dump("  ", "It's even colder here");
		}
		if (u.umadness&MAD_OVERLORD){
			dump("  ", "He can see you even clearer here");
		}
		if (u.umadness&MAD_DREAMS){
			dump("  ", "You dreamt of strange cities");
		}
		if (u.umadness&MAD_HELMINTHOPHOBIA){
			dump("  ", "You had an irrational fear of squirming things");
		}
		if(has_blood(youracedata)){
			if (u.umadness&MAD_FRENZY){
				Sprintf(buf, "You felt your %s seethe below your %s", body_part(BLOOD), body_part(BODY_SKIN));
				dump("  ", buf);
			}
		}
	}
	/*** More Troubles ***/
	if (Halluc_resistance) 	dump("  ", "You resisted hallucinations");
	if (Hallucination) dump(youwere, "hallucinating");
	if (Stunned) dump(youwere, "stunned");
	if (Confusion) dump(youwere, "confused");
	if (Blinded) dump(youwere, "blinded");
	if (Sick) {
		if (u.usick_type & SICK_VOMITABLE)
			dump(youwere, "sick from food poisoning");
		if (u.usick_type & SICK_NONVOMITABLE)
			dump(youwere, "sick from illness");
	}
	if (Stoned) dump(youwere, "turning to stone");
	if (Golded) dump(youwere, "turning to gold");
	if (Slimed) dump(youwere, "turning into slime");
	if (FrozenAir) dump(youwere, "suffocating in the cold night");
	if (Strangled)
		dump(youwere, (u.uburied) ? "buried" : "being strangled");
	if (Glib) {
		Sprintf(buf, "slippery %s", makeplural(body_part(FINGER)));
		dump(youhad, buf);
	}
	if(u_healing_penalty()) dump("  ", "You healed slowly as a result of your equipment");
	if (Fumbling) dump("  ", "You fumbled");
	if (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  ) {
		Sprintf(buf, "wounded %s", makeplural(body_part(LEG)));
		dump(youhad, buf);
	}
#ifdef STEED
	if (Wounded_legs && u.usteed) {
	    Strcpy(buf, x_monnam(u.usteed, ARTICLE_YOUR, (char *)0, 
		    SUPPRESS_SADDLE | SUPPRESS_HALLUCINATION, FALSE));
	    *buf = highc(*buf);
	    Strcat(buf, " had wounded legs");
	    dump("  ", buf);
	}
#endif
	if (Sleeping) dump("  ", "You fell asleep");
	if (Hunger) dump("  ", "You hungered rapidly");
	if(u.wimage){
		if(ACURR(A_WIS) < 6){
			Sprintf(buf, " filled with the image of a weeping angel");
			dump("  Your mind was ",buf);
		} else if(ACURR(A_WIS) < 9){
			Sprintf(buf, " ever on your mind");
			dump("  The image of a weeping angel was ",buf);
		} else if(ACURR(A_WIS) < 12){
			Sprintf(buf, " seem to shake the image of a weeping angel from your mind");
			dump("  You couldn't ",buf);
		} else {
			Sprintf(buf, " in the back of your mind");
			dump("  The image of a weeping angel lurked ",buf);
		}
	}

	/*** Vision and senses ***/
	if (See_invisible(u.ux,u.uy)) dump("  ", "You saw invisible");
	if (Blind_telepat) dump(youwere, "telepathic");
	if (Warning) dump(youwere, "warned");
	if (Warn_of_mon && flags.warntypea) {
		Sprintf(buf, "aware of the presence of %s",
			(flags.warntypea & MA_ORC) ? "orcs" :
			(flags.warntypea & MA_DEMON) ? "demons" :
			something); 
		dump(youwere, buf);
	}
	if (Searching) dump(youhad, "automatic searching");
	if (Clairvoyant) dump(youwere, "clairvoyant");
	if (Infravision) dump(youhad, "infravision");
	if (Detect_monsters)
	  dump(youwere, "sensing the presence of monsters");

	/*** Appearance and behavior ***/
	Sprintf(buf, "a carrying capacity of %d remaining", -1*inv_weight());
    dump(youhad, buf);
	Sprintf(buf, "%d points of nutrition remaining", YouHunger);
    dump(youhad, buf);
	if (Adornment) {
	    int adorn = 0;
	    if(uleft && uleft->otyp == RIN_ADORNMENT) adorn += uleft->spe;
	    if(uright && uright->otyp == RIN_ADORNMENT) adorn += uright->spe;
	    if (adorn < 0)
		dump(youwere, "poorly adorned");
	    else
		dump(youwere, "adorned");
	}
	if (Invisible) dump(youwere, "invisible");
	else if (Invis) dump(youwere, "invisible to others");
	/* ordinarily "visible" is redundant; this is a special case for
	   the situation when invisibility would be an expected attribute */
	else if ((HInvis || EInvis || pm_invisible(youracedata)) && BInvis)
	    dump(youwere, "visible");
	if (Displaced) dump(youwere, "displaced");
	if (Stealth) dump(youwere, "stealthy");
	if (Aggravate_monster) dump("  ", "You aggravated monsters");
	if (Conflict) dump("  ", "You caused conflict");

	/*** Transportation ***/
	if (Jumping) dump(youcould, "jump");
	if (Teleportation) dump(youcould, "teleport");
	if (Teleport_control) dump(youhad, "teleport control");
	if (Lev_at_will) dump(youwere, "levitating, at will");
	else if (Levitation)
	  dump(youwere, "levitating");	/* without control */
	else if (Flying) dump(youcould, "fly");
	if (Wwalking) dump(youcould, "walk on water");
	if (Swimming) dump(youcould, "swim");
	if (Breathless) dump(youcould, "survive without air");
	else if (Amphibious) dump(youcould, "breathe water");
	if (Passes_walls) dump(youcould, "walk through walls");
#ifdef STEED
	if (u.usteed && (final < 2 || strcmp(killer, "riding accident"))) {
	    Sprintf(buf, "riding %s", y_monnam(u.usteed));
	    dump(youwere, buf);
	}
#endif
	if (u.uswallow) {
	    Sprintf(buf, "swallowed by %s", a_monnam(u.ustuck));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%u)", u.uswldtim);
#endif
	    dump(youwere, buf);
	} else if (u.ustuck) {
	    Sprintf(buf, "%s %s",
		    (sticks(&youmonst) ? "holding" : "held by"),
		    a_monnam(u.ustuck));
	    dump(youwere, buf);
	}

	/*** Physical attributes ***/
	if (uclockwork){
		if(u.ucspeed==HIGH_CLOCKSPEED) dump(youwere, "set to high clockspeed");
		if(u.ucspeed==NORM_CLOCKSPEED) dump(youwere, "set to normal clockspeed");
		if(u.ucspeed==SLOW_CLOCKSPEED) dump(youwere, "set to low clockspeed");
		if(u.phasengn) dump(youwere, "in phase mode");
	}
	if (uandroid){
		if(u.ucspeed==HIGH_CLOCKSPEED) dump(youwere, "set to emergency speed");
		if(u.phasengn) dump(youwere, "in phase mode");
	}
	if (u.uhitinc || u.uuur_duration)
	    dump(youhad,
		enlght_combatinc("to hit", u.uhitinc + (u.uuur_duration ? 10 : 0), final, buf));
	if (u.udaminc || (u.uaesh/3) || u.uaesh_duration)
	    dump(youhad,
		enlght_combatinc("damage", u.udaminc+u.uaesh/3 + (u.uaesh_duration ? 10 : 0), final, buf));
	if(u.ukrau_duration){
	    dump(youhad, "+50% to spell damage");
	}
	if (u.ukrau/3)
	    dump(youhad, enlght_combatinc("spell damage", u.ukrau/3, final, buf));
	if (u.uhoon/3 || u.uhoon_duration)
	    dump(youhad, enlght_combatinc("to healing", u.uhoon/3 + (u.uhoon_duration ? 30 : 0), final, buf));
	if (u.unaen/3 || u.unaen_duration)
	    dump(youhad, enlght_combatinc("to energy regeneration", u.unaen/3 + (u.unaen_duration ? 30 : 0), final, buf));
	if (Slow_digestion) dump(youhad, "slower digestion");
	if (Regeneration) dump("  ", "You regenerated");
	if (u.uspellprot || Protection || u.uuur/2 || u.uuur_duration || (u.uvaul+4)/5) {
	    int prot = 0;

	    if(uleft && uleft->otyp == RIN_PROTECTION) prot += uleft->spe;
	    if(uright && uright->otyp == RIN_PROTECTION) prot += uright->spe;
	    if (HProtection & INTRINSIC) prot += u.ublessed;
	    prot += u.uspellprot;
	    prot += u.uuur/2;
	    prot += u.uuur_duration ? 10 : 0;
	    prot += (u.uvaul+4)/5;
	    
	    if (prot < 0)
		dump(youwere, "ineffectively protected");
	    else
		dump(youwere, "protected");
	}
	if (Protection_from_shape_changers)
		dump(youwere, "protected from shape changers");
	if (Polymorph) dump(youwere, "polymorphing");
	if (Polymorph_control) dump(youhad, "polymorph control");
	if (u.ulycn >= LOW_PM) {
		Strcpy(buf, an(mons[u.ulycn].mname));
		dump(youwere, buf);
	}
	if (Upolyd) {
	    if (u.umonnum == u.ulycn) Strcpy(buf, "in beast form");
	    else Sprintf(buf, "polymorphed into %s",
			 an(youmonst.data->mname));
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.mtimedone);
#endif
	    dump(youwere, buf);
	}
	if (Unchanging)
	  dump(youcould, "not change from your current form");
	if (Fast) dump(youwere, Very_fast ? "very fast" : "fast");
	if (Reflecting) dump(youhad, "reflection");
	if (Free_action) dump(youhad, "free action");
	if (Fixed_abil) dump(youhad, "fixed abilities");
	if (Lifesaved)
		dump("  ", "Your life would have been saved");
	if (u.twoweap) dump(youwere, "wielding two weapons at once");
	if (u.umconf) dump(youwere, "going to confuse monsters");

	/*** Miscellany ***/
	if (Luck) {
	    ltmp = abs((int)Luck);
	    Sprintf(buf, "%s%slucky (%d)",
		    ltmp >= 10 ? "extremely " : ltmp >= 5 ? "very " : "",
		    Luck < 0 ? "un" : "", Luck);
	    dump(youwere, buf);
	}
#ifdef WIZARD
	 else if (wizard) dump("  ", "Your luck was zero");
#endif
	if (u.moreluck > 0) dump(youhad, "extra luck");
	else if (u.moreluck < 0) dump(youhad, "reduced luck");
	if (carrying(LUCKSTONE) || stone_luck(TRUE)) {
	    ltmp = stone_luck(FALSE);
	    if (ltmp <= 0)
		dump("  ", "Bad luck did not time out for you");
	    if (ltmp >= 0)
		dump("  ", "Good luck did not time out for you");
	}

	if (u.ugangr[Align2gangr(u.ualign.type)]) {
	    Sprintf(buf, " %sangry with you",
		u.ugangr[Align2gangr(u.ualign.type)] > 6 ? "extremely " : u.ugangr[Align2gangr(u.ualign.type)] > 3 ? "very " : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", u.ugangr[Align2gangr(u.ualign.type)]);
#endif
	    Sprintf(buf2, "%s was %s", u_gname(), buf);
	    dump("  ", buf2);
	}

    {
	const char *p;

	buf[0] = '\0';
	if (final < 2) {    /* quit/escaped/ascended */
	    p = "survived after being killed ";
	    switch (u.umortality) {
	    case 0:  p = "survived";  break;
	    case 1:  Strcpy(buf, "once");  break;
	    case 2:  Strcpy(buf, "twice");  break;
	    case 3:  Strcpy(buf, "thrice");  break;
	    default: Sprintf(buf, "%d times", u.umortality);
		     break;
	    }
	} else {		/* game ended in character's death */
	    p = "are dead";
	    switch (u.umortality) {
	    case 0:  impossible("dead without dying?");
	    case 1:  break;			/* just "are dead" */
	    default: Sprintf(buf, " (%d%s time!)", u.umortality,
			     ordin(u.umortality));
		     break;
	    }
	}
	if (p) {
	  Sprintf(buf2, "You %s %s", p, buf);
	  dump("  ", buf2);
	}
    }
	dump("", "");
	return;

} /* dump_enlightenment */
#endif

STATIC_OVL void
resistances_enlightenment()
{
	char buf[BUFSZ];

	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, "Current Status:");
	putstr(en_win, 0, "");

	if (uclockwork){
		if(u.ucspeed==HIGH_CLOCKSPEED) putstr(en_win, 0, "Your clock is set to high speed.");
		if(u.ucspeed==NORM_CLOCKSPEED) putstr(en_win, 0, "Your clock is set to normal speed.");
		if(u.ucspeed==SLOW_CLOCKSPEED) putstr(en_win, 0, "Your clock is set to low speed.");
		if(u.phasengn) putstr(en_win, 0, "Your phase engine is activated.");
	}
	if (uandroid){
		if(u.ucspeed==HIGH_CLOCKSPEED) putstr(en_win, 0, "You are set to emergency speed.");
		if(u.phasengn) putstr(en_win, 0, "Your phase engine is activated.");
	}
	/*** Resistances to troubles ***/
	/* It is important to inform the player as to the status of any resistances that can expire */
	if (Fire_resistance && Cold_resistance) putstr(en_win, 0, "You feel comfortable.");
	else{
		if (Fire_resistance) putstr(en_win, 0, "You feel chilly.");
		if (Cold_resistance) putstr(en_win, 0, "You feel warm inside.");
	}
	if (Sleep_resistance) putstr(en_win, 0, "You feel wide awake.");
	if (Disint_resistance) putstr(en_win, 0, "You feel very firm.");
	if (Shock_resistance) putstr(en_win, 0, "You feel well grounded.");
	if (Poison_resistance) putstr(en_win, 0, "You feel healthy.");
	if (Acid_resistance) putstr(en_win, 0, "Your skin feels leathery.");
	if (Displaced) putstr(en_win, 0, "Your outline shimmers and shifts.");
	if (Drain_resistance) putstr(en_win, 0, "You feel especially energetic.");
	if (u.uinwater && Waterproof){
		if (ublindf && ublindf->otyp == R_LYEHIAN_FACEPLATE)
			putstr(en_win, 0, "Your faceplate wraps you in a waterproof field.");
		else if (ublindf && ublindf->oartifact == ART_MASK_OF_TLALOC)
			putstr(en_win, 0, "Your mask protects you from rain & storms greater than this.");
		else if (u.ufirst_sky)
			putstr(en_win, 0, "The water is separated from you.");
		else if (uarmc && (uarmc->greased || uarmc->otyp == OILSKIN_CLOAK))
			putstr(en_win, 0, "Your greased cloak protects your gear.");
		else if (u.sealsActive&SEAL_ENKI)
			putstr(en_win, 0, "YOU'RE soaked, but the water doesn't wet your gear.");
		else
			putstr(en_win, 0, "Your equipment protects you from the water around you.");
	}
	
	if(Nullmagic){
		int i;
		update_alternate_spells();
		for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
			putstr(en_win, 0, "Your magic is blocked.");
			break;
		}
	}
/*
	if (Sick_resistance) you_are("immune to sickness");
	if (Antimagic) you_are("magic-protected");
	if (Stone_resistance)
		you_are("petrification resistant");
	if (Invulnerable) you_are("invulnerable");
	if (u.uedibility) you_can("recognize detrimental food");
	if (Halluc_resistance)
		enl_msg("You resist", "", "ed", " hallucinations");
*/
	if(u.umconf){
		if(Blind) Sprintf(buf, "Your %s are tingling.", makeplural(body_part(HAND)));
		else Sprintf(buf, "Your %s are glowing%s %s.", makeplural(body_part(HAND)), u.umconf > 20 ? " brilliantly" : u.umconf > 10 ? " brightly" : "", hcolor(NH_RED));
		putstr(en_win, 0, buf);
	}
	
	/*** Thoughts ***/
	if (active_glyph(CLOCKWISE_METAMORPHOSIS)) putstr(en_win, 0, "A clockwise gyre turns in the depths below your id.");
	if (active_glyph(ANTI_CLOCKWISE_METAMORPHOSIS)) putstr(en_win, 0, "An anti-clockwise gyre turns in the heights above your soul.");
	if (active_glyph(ARCANE_BULWARK)) putstr(en_win, 0, "You remember the shores of a sparkling lake.");
	if (active_glyph(DISSIPATING_BULWARK)) putstr(en_win, 0, "You remember a flashing storm over pure lake-water.");
	if (active_glyph(SMOLDERING_BULWARK)) putstr(en_win, 0, "You remember embers drowning in still waters.");
	if (active_glyph(FROSTED_BULWARK)) putstr(en_win, 0, "You remember snowflakes on the surface of a lake.");
	if (active_glyph(BLOOD_RAPTURE)) putstr(en_win, 0, "You see rainbows in a mists of blood.");
	if (active_glyph(CLAWMARK)) putstr(en_win, 0, "Every surface you have ever seen was scored by claws.");
	if (active_glyph(CLEAR_DEEPS)) putstr(en_win, 0, "Your thoughts drift through blue water.");
	if (active_glyph(DEEP_SEA)) putstr(en_win, 0, "Your fears drown in pitch-black water.");
	if (active_glyph(COMMUNION)) putstr(en_win, 0, "A strange minister preaches continuously in your childhood home.");
	if (active_glyph(CORRUPTION)) putstr(en_win, 0, "It weeps tears of blood.");
	if (active_glyph(EYE_THOUGHT)) putstr(en_win, 0, "Eyes writhe inside your head.");
	if (active_glyph(FORMLESS_VOICE)) putstr(en_win, 0, "A great voice speaks to you.");
	if (active_glyph(GUIDANCE)) putstr(en_win, 0, "You see dancing sprites, far away in the dark behind your eyes.");
	if (active_glyph(IMPURITY)) putstr(en_win, 0, "Red millipedes crawl through your brain.");
	if (active_glyph(MOON)) putstr(en_win, 0, "You see the face of the sympathetic moon.");
	if (active_glyph(WRITHE)) putstr(en_win, 0, "A subtle mucus covers your brain.");
	if (active_glyph(RADIANCE)) putstr(en_win, 0, "Your mind is impaled on a golden pyramid.");
	if (active_glyph(BEASTS_EMBRACE)) putstr(en_win, 0, "A bestial figure hides inside of you.");
	
	/*** Troubles ***/
	if(u.usanity < 100)
		putstr(en_win, 0, "You are a little touched in the head.");
	else if(u.usanity < 50)
		putstr(en_win, 0, "You sometimes struggle with insanity.");
	else if(u.usanity < 25)
		putstr(en_win, 0, "You frequently struggle with insanity.");
	else if(u.usanity < 10)
		putstr(en_win, 0, "You constantly struggle with insanity.");
	else if(u.usanity == 0)
		putstr(en_win, 0, "You are quite insane.");
	
	if(u.uinsight > 40)
		putstr(en_win, 0, "You frequently see things you wish you hadn't.");
	else if(u.uinsight > 20)
		putstr(en_win, 0, "You often see things you wish you hadn't.");
	else if(u.uinsight > 1)
		putstr(en_win, 0, "You occasionally see things you wish you hadn't.");
	
	/*** Madnesses ***/
	if(u.usanity < 100 && !ClearThoughts){
		if (u.umadness&MAD_DELUSIONS){
			putstr(en_win, 0, "You have a tendency to hallucinate.");
		}
		if(u.usanity < 80){
			if (u.umadness&MAD_REAL_DELUSIONS){
				if(u.umadness&MAD_DELUSIONS)
					putstr(en_win, 0, "...at least, you THINK you're hallucinating....");
				else
					putstr(en_win, 0, "You have a tendency to hallucinate... you think.");
			}
		}
		if (u.umadness&MAD_SANCTITY){
			putstr(en_win, 0, "You have a tendency to treat women as delicate and holy beings who shouldn't be harmed.");
		}
		if (u.umadness&MAD_GLUTTONY){
			putstr(en_win, 0, "You have a mad hunger.");
		}
		if (u.umadness&MAD_SPORES && !Race_if(PM_ANDROID) && !Race_if(PM_CLOCKWORK_AUTOMATON)){
			//Note: Race_if is correct because it works on your original brain (because magic)
			putstr(en_win, 0, "She's eating your brain.");
		}
		if (u.umadness&MAD_FRIGOPHOBIA){
			putstr(en_win, 0, "You have an irrational fear of the cold.");
		}
		if (u.umadness&MAD_CANNIBALISM){
			putstr(en_win, 0, "You have a mad desire to consume living flesh, even the flesh of your own kind.");
		}
		if (u.umadness&MAD_RAGE){
			putstr(en_win, 0, "You have a burning, irrational rage.");
		}
		if (u.umadness&MAD_ARGENT_SHEEN){
			putstr(en_win, 0, "The world is full of mirrors, and you can't help but admire yourself.");
		}
		if (u.umadness&MAD_SUICIDAL){
			putstr(en_win, 0, "You have a tendency towards suicidal behavior.");
		}
		if (u.umadness&MAD_NUDIST){
			putstr(en_win, 0, "You have an irrational dislike of clothing.");
		}
		if (u.umadness&MAD_OPHIDIOPHOBIA){
			putstr(en_win, 0, "You have an irrational fear of snakes.");
		}
		if (u.umadness&MAD_ARACHNOPHOBIA){
			//Note: Arachnophobia == women and spiders because it's the madness Lolth inflicts.
			putstr(en_win, 0, "You have an irrational fear of spiders.");
			putstr(en_win, 0, "You have an irrational fear of women.");
		}
		if (u.umadness&MAD_ENTOMOPHOBIA){
			putstr(en_win, 0, "You have an irrational fear of insects.");
		}
		if (u.umadness&MAD_THALASSOPHOBIA){
			putstr(en_win, 0, "You have an irrational fear of sea-monsters.");
		}
		if (u.umadness&MAD_PARANOIA){
			//Severe because it triggers much more frequently than other madnesses
			putstr(en_win, 0, "You have a severe tendency towards paranoia.");
		}
		if (u.umadness&MAD_TALONS){
			putstr(en_win, 0, "You have an irrational fear of loss.");
		}
		if (u.umadness&MAD_COLD_NIGHT){
			putstr(en_win, 0, "It's so cold.");
		}
		if (u.umadness&MAD_OVERLORD){
			putstr(en_win, 0, "He can see you.");
		}
		if (u.umadness&MAD_DREAMS){
			putstr(en_win, 0, "You dream of strange cities.");
		}
		if (u.umadness&MAD_HELMINTHOPHOBIA){
			putstr(en_win, 0, "You have an irrational fear of squirming things.");
		}
		if(has_blood(youracedata)){
			if (u.umadness&MAD_FRENZY){
				Sprintf(buf, "You feel your %s seethe below your %s.", body_part(BLOOD), body_part(BODY_SKIN));
				putstr(en_win, 0, buf);
			}
		}
	}
	/*** More Troubles ***/
	if(u_healing_penalty()) putstr(en_win, 0, "You feel itchy.");
	if (Wounded_legs
#ifdef STEED
	    && !u.usteed
#endif
			  ) {
		Sprintf(buf, "You have wounded %s", makeplural(body_part(LEG)));
		putstr(en_win, 0, buf);
	}
	
	if(u.wimage >= 10){
		if(ACURR(A_WIS) < 6) Sprintf(buf, "Your mind is filled with the image of a weeping angel");
		else if(ACURR(A_WIS) < 9) Sprintf(buf, "The image of a weeping angel is ever on your mind");
		else if(ACURR(A_WIS) < 12) Sprintf(buf, "You can't seem to shake the image of a weeping angel from your mind");
		else Sprintf(buf, "The image of a weeping angel lurks in the back of your mind");
		putstr(en_win, 0, buf);
	}
	if (u.umorgul){
		Sprintf(buf, "You feel deathly cold");
		putstr(en_win, 0, buf);
	}
	
	if(u.lastprayed){
		Sprintf(buf, "You last %s %ld turns ago", u.lastprayresult==PRAY_GIFT ? "recieved a gift" :
												  u.lastprayresult==PRAY_ANGER ? "angered your god" : 
												  u.lastprayresult==PRAY_CONV ? "converted to a new god" : 
												  "prayed",
			moves - u.lastprayed);
		putstr(en_win, 0, buf);
		if(u.lastprayresult==PRAY_GOOD){
			Sprintf(buf, "That prayer was well recieved");
			putstr(en_win, 0, buf);
		} else if(u.lastprayresult==PRAY_BAD){
			Sprintf(buf, "That prayer was poorly recieved");
			putstr(en_win, 0, buf);
		}
		if(u.reconciled){
			if(u.reconciled==REC_REC) Sprintf(buf, "You have since reconciled with your god");
			else if(u.reconciled==REC_MOL) Sprintf(buf, "You have since mollified your god");
			putstr(en_win, 0, buf);
		}
	}
	
	if(u.sealsActive || u.specialSealsActive){
		int i,j,numBound,numFound=0;
		numBound = u.sealCounts;
		if(u.spirit[QUEST_SPIRIT]) numBound++;
		if(Role_if(PM_EXILE) && u.uevent.uhand_of_elbereth) numBound++;
		if(u.spirit[CROWN_SPIRIT]) numBound++;
		if(u.spirit[GPREM_SPIRIT]) numBound++;
		if(u.spirit[ALIGN_SPIRIT]) numBound++;
		if(u.spirit[OUTER_SPIRIT]) numBound++;
		if(Role_if(PM_ANACHRONONAUT) && (u.specialSealsActive&SEAL_BLACK_WEB))
			numBound++;
		Sprintf(buf, "Your soul is bound to ");
		for(i=0;i<QUEST_SPIRIT;i++){
			if(u.spirit[i]) for(j=0;j<32;j++){
				if((u.spirit[i] >> j) == 1){
					Strcat(buf,sealNames[j]);
					numFound++;
					if(numBound==2 && numFound==1) Strcat(buf," and ");
					else if(numBound>=3){
						if(numFound<numBound-1) Strcat(buf,", ");
						if(numFound==numBound-1) Strcat(buf,", and ");
					}
					break;
				}
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_DAHLVER_NAR){
			Strcat(buf, sealNames[(DAHLVER_NAR) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_ACERERAK){
			Strcat(buf, sealNames[(ACERERAK) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_COUNCIL){
			Strcat(buf, sealNames[(COUNCIL) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.spirit[CROWN_SPIRIT]) for(j=0;j<32;j++){
			if((u.spirit[CROWN_SPIRIT] >> j) == 1){
				Strcat(buf,sealNames[j]);
				numFound++;
				if(numBound==2 && numFound==1) Strcat(buf," and ");
				else if(numBound>=3){
					if(numFound<numBound-1) Strcat(buf,", ");
					if(numFound==numBound-1) Strcat(buf,", and ");
				}
				break;
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_COSMOS){
			Strcat(buf, sealNames[(COSMOS) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_LIVING_CRYSTAL){
			Strcat(buf, sealNames[(LIVING_CRYSTAL) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_TWO_TREES){
			Strcat(buf, sealNames[(TWO_TREES) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_MISKA){
			Strcat(buf, sealNames[(MISKA) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_NUDZIRATH){
			Strcat(buf, sealNames[(NUDZIRATH) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_ALIGNMENT_THING){
			Strcat(buf, sealNames[(ALIGNMENT_THING) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_UNKNOWN_GOD){
			Strcat(buf, sealNames[(UNKNOWN_GOD) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_BLACK_WEB){
			Strcat(buf, sealNames[(BLACK_WEB) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		if(numFound < numBound && u.specialSealsActive&SEAL_NUMINA){
			Strcat(buf, sealNames[(NUMINA) - (FIRST_SEAL)]);
			numFound++;
			if(numBound==2 && numFound==1) Strcat(buf," and ");
			else if(numBound>=3){
				if(numFound<numBound-1) Strcat(buf,", ");
				if(numFound==numBound-1) Strcat(buf,", and ");
			}
		}
		Strcat(buf,".");
		putstr(en_win, 0, buf);
	}
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
	return;
}

void
udr_enlightenment()
{
	int dr;
	char mbuf[BUFSZ] = {'\0'};
	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, "Current Damage Reduction:");
	putstr(en_win, 0, "");
	
	if(!has_head_mon(&youmonst)){
		Sprintf(mbuf, "You have no head; shots hit upper body");
		putstr(en_win, 0, mbuf);
	} else {
		dr = slot_udr(HEAD_DR, (struct monst *)0);
	Sprintf(mbuf, "Head Armor:       %s%d", (dr>11) ? "11-" : "", dr);
		putstr(en_win, 0, mbuf);
	}
	dr = slot_udr(UPPER_TORSO_DR, (struct monst *)0);
	Sprintf(mbuf, "Upper Body Armor: %s%d", (dr>11) ? "11-" : "", dr);
	dr = slot_udr(LOWER_TORSO_DR, (struct monst *)0);
	putstr(en_win, 0, mbuf);
	Sprintf(mbuf, "Lower Body Armor: %s%d", (dr>11) ? "11-" : "", dr);
	putstr(en_win, 0, mbuf);
	if(!can_wear_gloves(youracedata)){
		Sprintf(mbuf, "You have no hands; shots hit upper body");
		putstr(en_win, 0, mbuf);
	} else {
		dr = slot_udr(ARM_DR, (struct monst *)0);
	Sprintf(mbuf, "Hand Armor:       %s%d", (dr>11) ? "11-" : "", dr);
		putstr(en_win, 0, mbuf);
	}
	if(!can_wear_boots(youracedata)){
		Sprintf(mbuf, "You have no feet; shots hit lower body");
		putstr(en_win, 0, mbuf);
	} else {
		dr = slot_udr(LEG_DR, (struct monst *)0);
	Sprintf(mbuf, "Foot Armor:       %s%d", (dr>11) ? "11-" : "", dr);
		putstr(en_win, 0, mbuf);
	}
	
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
	return;
}

STATIC_OVL void
spirits_enlightenment()
{
	char buf[BUFSZ];
	int i;
	en_win = create_nhwindow(NHW_MENU);

	putstr(en_win, 0, "Currently bound spirits:");
	putstr(en_win, 0, "");

#define addseal(id) do {if(u.sealTimeout[decode_sealID(u.spirit[(id)]) - (FIRST_SEAL)] > moves)\
	Sprintf(buf, "  %-23s (timeout:%ld)", sealNames[decode_sealID(u.spirit[(id)]) - (FIRST_SEAL)], \
		u.sealTimeout[decode_sealID(u.spirit[(id)]) - (FIRST_SEAL)] - moves); \
	else\
	Sprintf(buf, "  %-23s", sealNames[decode_sealID(u.spirit[(id)]) - (FIRST_SEAL)]); \
	putstr(en_win, 0, buf); } while (0)
#define addpen(seal) do {\
	Sprintf(buf, "  %-23s (timeout:%ld)", sealNames[decode_sealID(seal) - (FIRST_SEAL)], \
		u.sealTimeout[decode_sealID(seal) - (FIRST_SEAL)] - moves); \
	putstr(en_win, 0, buf); } while (0)
#define addempty() do {Sprintf(buf,"  (empty)"); putstr(en_win, 0, buf);} while(0)

	/* only show gnosis premonition when it is being used */
	if (u.spirit[GPREM_SPIRIT] != 0L) {
		putstr(en_win, 0, "Gnosis Premonition");
		addseal(GPREM_SPIRIT);
		putstr(en_win, 0, "");
	}
	/* only show near void spirits if you know any seals */
	if (u.sealsKnown) {
		putstr(en_win, 0, "Spirits of the Near Void");
		for (i = 0; i < u.sealCounts; i++) {
			addseal(i);
		}
		for (; i < binder_nearvoid_slots(); i++) {
			addempty();
		}
		putstr(en_win, 0, "");
	}
	/* only show quest spirits if you know either seal */
	if ((u.specialSealsKnown & (SEAL_ACERERAK | SEAL_DAHLVER_NAR | SEAL_BLACK_WEB))
		/* needs special case for myrkalfyr who don't know the seal, but are bound anyways */
		|| (u.specialSealsActive&SEAL_BLACK_WEB)) {
		putstr(en_win, 0, "Quest Spirit");
		if (u.spirit[QUEST_SPIRIT] != 0L) {
			addseal(QUEST_SPIRIT);
		}
		else {
			addempty();
		}
		putstr(en_win, 0, "");
	}
	/* only show alignment spirits if you know any */
	if (u.specialSealsKnown & (
			SEAL_COSMOS |
			SEAL_LIVING_CRYSTAL |
			SEAL_TWO_TREES |
			SEAL_MISKA |
			SEAL_NUDZIRATH |
			SEAL_ALIGNMENT_THING |
			SEAL_UNKNOWN_GOD
			)) {
		putstr(en_win, 0, "Alignment Spirit");
		if (u.spirit[ALIGN_SPIRIT] != 0L) {
			addseal(ALIGN_SPIRIT);
		}
		else {
			addempty();
		}
		putstr(en_win, 0, "");
	}
	/* the Embassy of Elements's spirit */
	if (u.specialSealsActive & SEAL_COUNCIL)
	{
		putstr(en_win, 0, "Embassy of Elements");
		if (u.spirit[CROWN_SPIRIT] != 0L) {
			addseal(CROWN_SPIRIT);
		}
		else {
			addempty();
		}
		putstr(en_win, 0, "");
	}
	/* Show the Numina for Binders once they have hit XL 30 */
	if (Role_if(PM_EXILE) && (u.ulevelmax >= 30))
	{
		putstr(en_win, 0, "Outer Spirit");
		if (u.spirit[OUTER_SPIRIT] != 0L) {
			addseal(OUTER_SPIRIT);
		}
		else {
			addempty();
		}
		putstr(en_win, 0, "");
	}

	/* Show spirits bound into the Pen of the Void */
	if (!undiscovered_artifact(ART_PEN_OF_THE_VOID)) {
		if (u.voidChime)
			putstr(en_win, 0, "Bound to you and the Pen of the Void");
		else
			putstr(en_win, 0, "Bound to the Pen of the Void");

		/* All get the first slot */
		if (u.spiritTineA)
			addpen(u.spiritTineA);
		else
			addempty();
		/* Second slot belongs to discipled Binders */
		if (quest_status.killed_nemesis && Role_if(PM_EXILE)) {
			if (u.spiritTineB)
				addpen(u.spiritTineB);
			else
				addempty();
		}
		putstr(en_win, 0, "");
	}
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
	return;

#undef addseal
#undef addempty
}

STATIC_OVL void
signs_enlightenment()
{
	boolean message = FALSE;

	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, "Current Appearance:");
	putstr(en_win, 0, "");

	if(Invis){
		putstr(en_win, 0, "You are invisible.");
		message = TRUE;
	}
	
	// if(u.sealsActive&SEAL_AHAZU && !(ublindf && ublindf->otyp==MASK));
	if(u.sealsActive&SEAL_AMON && !Invis){
//		if(!(uarmh && is_metallic(uarmh))){
		putstr(en_win, 0, "You have a pair of large ram's horns.");
//		} else putstr(en_win, 0, "Your ram's horns have fused with your helm, taking on a metalic hue.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_ANDREALPHUS && !Invis && !(levl[u.ux][u.uy].lit == 0 && !(viz_array[u.uy][u.ux]&TEMP_LIT1 && !(viz_array[u.uy][u.ux]&TEMP_DRK3)))){
		putstr(en_win, 0, "Up close, it is plain your shadow aspires to depth as well as width and height.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_ANDROMALIUS && !NoBInvis){
			putstr(en_win, 0, "Your features have taken on the rigidity of a cheap disguise.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_ASTAROTH && !Invis){
		if(!ublindf || ublindf->otyp == LENSES || ublindf->otyp == LIVING_MASK)
			putstr(en_win, 0, "A black liquid leaks from around your eyes.");
		else if(ublindf && (ublindf->otyp == MASK || ublindf->otyp == R_LYEHIAN_FACEPLATE))
			putstr(en_win, 0, "The black liquid leaking from your eyes is hidden by your mask.");
		else
			putstr(en_win, 0, "The black liquid leaking from your eyes is soaked up by your blindfold.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_BALAM && !Invis){
		if(uarmc || uarm)
			putstr(en_win, 0, "Freezing water leaks from a gash in you neck, but is hidden by your clothes.");
		else
			putstr(en_win, 0, "Freezing water leaks from a deep gash in you neck.");
		if(!uarmg)
			putstr(en_win, 0, "Freezing water leaks from deep holes in your wrists.");
		if(!uarmf)
			putstr(en_win, 0, "Freezing water leaks from deep holes in your ankles.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_BERITH && !Invis){
		if(u.usteed)
			putstr(en_win, 0, "Your steed is drenched in gore.");
		if(!(uarm && is_metallic(uarm) && uarmg && uarmf && uarmh))
			putstr(en_win, 0, "You are drenched in gore.");
		else
			putstr(en_win, 0, "Your armor is faced with crimson enamel.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_BUER && !Invis){
		if(!uarmf)
			putstr(en_win, 0, "Your legs bifurcate into twin pairs of cloven-hoved feet.");
		else
			putstr(en_win, 0, "Your strange feet are hidden in your shoes.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_CHUPOCLOPS){
		putstr(en_win, 0, "You feel something in your cheeks.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_DANTALION && !NoBInvis){
		if(!(uarmc || ((uarm && uarm->otyp != CRYSTAL_PLATE_MAIL) || uarmu))){
			switch(u.ulevel/10+1){
				case 1:
				putstr(en_win, 0, "There is an extra face growing on your chest.");
				break;
				case 2:
				putstr(en_win, 0, "There is a pair of faces growing on your chest.");
				break;
				case 3:
				putstr(en_win, 0, "Many extra faces grow on your chest.");
				break;
				case 4:
				putstr(en_win, 0, "There are legions of faces growing on your chest.");
				break;
			}
		}
		else{
			if(u.ulevel/10){
				putstr(en_win, 0, "Your extra faces are covered by your clothes.");
			} else
				putstr(en_win, 0, "Your extra face is covered by your clothes.");
		}
		message = TRUE;
	}
	// if(u.sealsActive&SEAL_SHIRO);
	if(u.sealsActive&SEAL_ECHIDNA && !Invis){
		if(!(uarmf && (uarmc || uarm)))
			putstr(en_win, 0, "Your hips give rise to twin serpent's tails instead of legs.");
		else
			putstr(en_win, 0, "Your serpentine legs are disguised by your clothes.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_EDEN){
		putstr(en_win, 0, "There is something rigid in the crown of your skull.");
		message = TRUE;
	} 
	if(u.sealsActive&SEAL_ENKI && !Invis){
		if(!(uarm || uarmc))
			putstr(en_win, 0, "Water runs off your body in steady rivulets.");
		else
			putstr(en_win, 0, "Your body's runoff is caught by your clothes.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_EURYNOME && !Invis){
		if(levl[u.ux][u.uy].lit != 0){
			putstr(en_win, 0, "Your shadow is that of a dancing nymph.");
			message = TRUE;
		} else if(viz_array[u.uy][u.ux]&TEMP_LIT1 && !(viz_array[u.uy][u.ux]&TEMP_DRK3)){
			putstr(en_win, 0, "It's a bit hard to see, but your shadow is a dancing nymph.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_EVE && !NoBInvis){
		if(!uarm && !uarmc){
			putstr(en_win, 0, "There is a blood-caked wound on your stomach.");
			message = TRUE;
		}
		if(!uarmf){
			putstr(en_win, 0, "Your feet are torn by thorns and stones.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_FAFNIR && !NoBInvis){ 
		if(!(uright || uarmg)){
			putstr(en_win, 0, "There is a ring-shaped burn scar around your right ring-finger.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_HUGINN_MUNINN){
		putstr(en_win, 0, "There is something rustling around in your ear.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_IRIS && !NoBInvis){
		if(moves <= u.irisAttack+1){
			putstr(en_win, 0, "Waving, iridescent tentacles sprout from your forearms.");
			message = TRUE;
		} else if(!uarmc && moves <= u.irisAttack+5){
			putstr(en_win, 0, "There are iridescent tentacles wrapped around your forearms.");
			message = TRUE;
		} else if(!uarm && !uarmc){
			putstr(en_win, 0, "There are iridescent veins just under the skin of your forearms.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_JACK){
		putstr(en_win, 0, "There is something on your back.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_MALPHAS){
		putstr(en_win, 0, "You feel things pecking the inside of your mouth.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_MARIONETTE && !NoBInvis){
		if(!(uarm && is_metallic(uarm)))
			putstr(en_win, 0, "Metal wires protrude from your elbows, knees, and back.");
		else
			putstr(en_win, 0, "The metal wires protruding from your body have merged with your armor.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_MOTHER && !NoBInvis){
		if(!uarmg)
			putstr(en_win, 0, "The eyes on your fingers and palms stare back at you.");
		else
			putstr(en_win, 0, "The eyes on your fingers and palms are covered up.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_NABERIUS){
		putstr(en_win, 0, "Your tongue feels odd.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_ORTHOS && !NoBInvis){
		if(uarmc && uarmc->otyp != MUMMY_WRAPPING){
			putstr(en_win, 0, "Your cloak blows in a nonexistent wind.");
			message = TRUE;
		}
	}
	// // if(u.sealsActive&SEAL_OSE && !BClairvoyant && !(uarmh && is_metallic(uarmh) && uarmh->otyp != HELM_OF_TELEPATHY)) count++;
	if(u.sealsActive&SEAL_OTIAX && !Invis){
		if(moves <= u.otiaxAttack+5){
			putstr(en_win, 0, "The mist around you is formed into writhing tendrils.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_PAIMON && !Invis){
		putstr(en_win, 0, "There is a crown floating over your head.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_SIMURGH && !Invis){
		if(!uarmg)
			putstr(en_win, 0, "You have iron claws.");
		else
			putstr(en_win, 0, "Your iron claws seem to be part of your gloves.");
		putstr(en_win, 0, "There are prismatic feathers around your head.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_TENEBROUS && !Invis){
		if(!(levl[u.ux][u.uy].lit == 0 && !(viz_array[u.uy][u.ux]&TEMP_LIT1 && !(viz_array[u.uy][u.ux]&TEMP_DRK3)))){
			putstr(en_win, 0, "Your shadow is deep black and pools unnaturally close to you.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_YMIR && !Invis){
		if(moves>5000 && moves <= 10000){
			if(!(uarm || uarmc))
				putstr(en_win, 0, "Your skin color is a bit off.");
			else
				putstr(en_win, 0, "Your skin is hidden under your clothes.");
			message = TRUE;
		} else if(moves>10000 && moves <= 20000){
			if(!(uarmc))
				putstr(en_win, 0, "Maggots burrow through your skin.");
			else
				putstr(en_win, 0, "Your rotting is hidden under your clothes.");
			message = TRUE;
		} else if(moves>20000 && moves <= 50000){
			if(!(uarmc && uarmg && uarmf))
				putstr(en_win, 0, "Your skin is rotting off.");
			else
				putstr(en_win, 0, "Your rotting is hidden under your clothes.");
			message = TRUE;
		} else if(moves>50000 && moves <= 100000){
			if(!(uarmc && uarmg && uarmf && uarm && uarmh))
				putstr(en_win, 0, "Your rotted body bristles with fungal sporangia and burrowing vermin.");
			else
				putstr(en_win, 0, "Your rotted form is hidden under your clothes.");
			message = TRUE;
		} else if(moves>100000){
			if(!(uarmc && uarmg && uarmf && uarm && uarmh && ublindf && ublindf->otyp==MASK))
				putstr(en_win, 0, "Your putrid body is a riot of fungal forms and saprophagous insects.");
			else
				putstr(en_win, 0, "Your putrid form is hidden under your clothes.");
			message = TRUE;
		}
	}
	if(u.specialSealsActive&SEAL_DAHLVER_NAR && !NoBInvis){
		if(dahlverNarVis())
			putstr(en_win, 0, "Your wounds are full of sharp teeth!");
		else
			putstr(en_win, 0, "You feel teeth beneath your skin.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_ACERERAK){
		putstr(en_win, 0, "Your eyes feel odd.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_BLACK_WEB){
		putstr(en_win, 0, "Your shadow is wrapped in the black web.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_COSMOS){
		putstr(en_win, 0, "You feel like something is behind you, but you can't see anything.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_LIVING_CRYSTAL){
		putstr(en_win, 0, "You feel like something is behind you, but you can't see anything.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_TWO_TREES){
		putstr(en_win, 0, "Glorious dappled light dances on your body.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_MISKA && u.ulevel >= 10){
		static char mbuf[BUFSZ] = {'\0'};
		if(u.ulevel >= 26){
			int howManyArms = (youracedata->mtyp == PM_VALAVI) ? 6 : 
						  (youracedata->mtyp == PM_MAN_SERPENT_MAGE) ? 6 : 
						  (youracedata->mtyp == PM_PHALANX) ? 6 : 
						  (youracedata->mtyp == PM_MARILITH) ? 8 : 
						  (youracedata->mtyp == PM_KARY__THE_FIEND_OF_FIRE) ? 8 : 
						  (youracedata->mtyp == PM_CATHEZAR) ? 8 : 
						  (youracedata->mtyp == PM_SHAKTARI) ? 8 : 
						  4;
			Sprintf(mbuf, "You have %d arms, and a wolf head grows from each hip.", howManyArms);
			putstr(en_win, 0, mbuf);
		} else if(u.ulevel >= 18) {
			putstr(en_win, 0, "You have a wolf head growing from each hip.");
		} else {
			putstr(en_win, 0, "You have a wolf head growing from your lower stomach.");
		}
		message = TRUE;
	}
	// if(u.specialSealsActive&SEAL_NUDZIRATH){
		// putstr(en_win, 0, "Your eyes feel odd.");
		// message = TRUE;
	// }
	if(u.specialSealsActive&SEAL_ALIGNMENT_THING){
		putstr(en_win, 0, "You feel like someone is staring at the back of your head.");
		putstr(en_win, 0, "You hear an argument raging in the distance.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_NUMINA){
		putstr(en_win, 0, "You are surrounded by whispers.");
		message = TRUE;
	}
	
	if(!message){
		putstr(en_win, 0, "You think you look pretty normal.");
	}
	
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
	return;
}

void
signs_mirror()
{
	boolean message = FALSE;
	int count;
	const char *comma;
	char msgbuf[BUFSZ];

	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, "Current Appearance:");
	putstr(en_win, 0, "");

	if(Invis){
		putstr(en_win, 0, "You are invisible.");
		message = TRUE;
	}
	
#define PUNCTUTATION	if(count > 0){if(count == 1) Sprintf(eos(msgbuf), "%s and ", comma); else Sprintf(eos(msgbuf), "%s ", comma);}
	//Words
	if(u.ufirst_light || u.ufirst_sky || u.ufirst_life || u.ufirst_know){
		count = 0;
		if(u.ufirst_light) count++;
		if(u.ufirst_sky) count++;
		if(u.ufirst_life) count++;
		if(u.ufirst_know) count++;
		
		if(count == 1){
			if(u.ufirst_light)
				putstr(en_win, 0, "There is a blinding glyph on your brow.");
			else if(u.ufirst_sky)
				putstr(en_win, 0, "There is a cerulean glyph on your brow.");
			else if(u.ufirst_life)
				putstr(en_win, 0, "There is a verdant glyph on your brow.");
			else if(u.ufirst_know)
				putstr(en_win, 0, "There is a crimson glyph on your brow.");
		} else {
			Sprintf(msgbuf, "There is ");
			if(count == 2) comma = "";
			else comma = ",";
			if(u.ufirst_light){
				Sprintf(eos(msgbuf), "a blinding glyph");
				count--;
				PUNCTUTATION
			}
			if(u.ufirst_sky){
				Sprintf(eos(msgbuf), "a cerulean glyph");
				count--;
				PUNCTUTATION
			}
			if(u.ufirst_life){
				Sprintf(eos(msgbuf), "a verdant glyph");
				count--;
				PUNCTUTATION
			}
			if(u.ufirst_know){
				Sprintf(eos(msgbuf), "a crimson glyph");
				count--;
				PUNCTUTATION
			}
			Sprintf(eos(msgbuf), " on your brow.");
			putstr(en_win, 0, msgbuf);
		}
	}
#undef PUNCTUTATION
	
#define PUNCTUTATION	if(count > 0){if(count == 1) Sprintf(eos(msgbuf), "%s and a ", comma); else Sprintf(eos(msgbuf), "%s a ", comma);}
	//Active syllables
	if(u.uaesh_duration || u.ukrau_duration || u.uhoon_duration || u.uuur_duration || u.unaen_duration || u.uvaul_duration){
		count = 0;
		if(u.uaesh_duration) count++;
		if(u.ukrau_duration) count++;
		if(u.uhoon_duration) count++;
		if(u.uuur_duration) count++;
		if(u.unaen_duration) count++;
		if(u.uvaul_duration) count++;
		if(count == 1){
			if(u.uaesh_duration)
				Sprintf(msgbuf, "A %sglowing %s floats above your brow.",  u.uaesh_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_STRENGTH__AESH]));
			else if(u.ukrau_duration)
				Sprintf(msgbuf, "A %sglowing %s floats above your brow.",  u.ukrau_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_POWER__KRAU]));
			else if(u.uhoon_duration)
				Sprintf(msgbuf, "A %sglowing %s floats above your brow.",  u.uhoon_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_LIFE__HOON]));
			else if(u.uuur_duration)
				Sprintf(msgbuf, "A %sglowing %s floats above your brow.",  u.uuur_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_GRACE__UUR]));
			else if(u.unaen_duration)
				Sprintf(msgbuf, "A %sglowing %s floats above your brow.",  u.unaen_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_THOUGHT__NAEN]));
			else if(u.uvaul_duration)
				Sprintf(msgbuf, "A %sglowing %s floats above your brow.",  u.uvaul_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_SPIRIT__VAUL]));
			putstr(en_win, 0, msgbuf);
		} else {
			if(count == 2) comma = "";
			else comma = ",";
			Sprintf(msgbuf, "A ");
			if(u.uaesh_duration){
				Sprintf(eos(msgbuf), "%sglowing %s",  u.uaesh_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_STRENGTH__AESH]));
				count--;
				PUNCTUTATION
			}
			if(u.ukrau_duration){
				Sprintf(eos(msgbuf), "%sglowing %s",  u.ukrau_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_POWER__KRAU]));
				count--;
				PUNCTUTATION
			}
			if(u.uhoon_duration){
				Sprintf(eos(msgbuf), "%sglowing %s",  u.uhoon_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_LIFE__HOON]));
				count--;
				PUNCTUTATION
			}
			if(u.uuur_duration){
				Sprintf(eos(msgbuf), "%sglowing %s",  u.uuur_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_GRACE__UUR]));
				count--;
				PUNCTUTATION
			}
			if(u.unaen_duration){
				Sprintf(eos(msgbuf), "%sglowing %s",  u.unaen_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_THOUGHT__NAEN]));
				count--;
				PUNCTUTATION
			}
			if(u.uvaul_duration){
				Sprintf(eos(msgbuf), "%sglowing %s",  u.uvaul_duration > 10 ?  "brilliantly " : "", OBJ_DESCR(objects[SYLLABLE_OF_SPIRIT__VAUL]));
				count--;
				PUNCTUTATION
			}
			Sprintf(eos(msgbuf), " float in a column above your brow.");
			putstr(en_win, 0, msgbuf);
		}
	}
#undef PUNCTUTATION
	
	//Inactive syllables
#define STRIPGLYPH		;
//define STRIPGLYPH		if ((bp = strstri(msgbuf, " glyph")) != 0) *bp = '\0';
#define PUNCTUTATION	if(count > 0){if(count == 1) Sprintf(eos(msgbuf), "%s and ", comma); else Sprintf(eos(msgbuf), "%s ", comma);}
	count = 0;
	if(u.uaesh && u.uaesh - !!u.uaesh_duration){
		count++;
	}
	if(u.ukrau && u.ukrau - !!u.ukrau_duration){
		count++;
	}
	if(u.uhoon && u.uhoon - !!u.uhoon_duration){
		count++;
	}
	if(u.uuur && u.uuur - !!u.uuur_duration){
		count++;
	}
	if(u.unaen && u.unaen - !!u.unaen_duration){
		count++;
	}
	if(u.uvaul && u.uvaul - !!u.uvaul_duration){
		count++;
	}
	if(count){
		int num;
		int total = 0;
		char *bp;
		if(count > 2) comma = ",";
		else comma = "";
		
		msgbuf[0] = 0;
		if(u.uaesh && u.uaesh - !!u.uaesh_duration){
			num = u.uaesh - !!u.uaesh_duration;
			total += num;
			if(num == 1)
				Sprintf(eos(msgbuf), "a %s", OBJ_DESCR(objects[SYLLABLE_OF_STRENGTH__AESH]));
			else
				Sprintf(eos(msgbuf), "%d %ss", num, OBJ_DESCR(objects[SYLLABLE_OF_STRENGTH__AESH]));
			count--;
			STRIPGLYPH
			PUNCTUTATION
		}
		if(u.ukrau && u.ukrau - !!u.ukrau_duration){
			num = u.ukrau - !!u.ukrau_duration;
			total += num;
			if(num == 1)
				Sprintf(eos(msgbuf), "a %s", OBJ_DESCR(objects[SYLLABLE_OF_POWER__KRAU]));
			else
				Sprintf(eos(msgbuf), "%d %ss", num, OBJ_DESCR(objects[SYLLABLE_OF_POWER__KRAU]));
			count--;
			STRIPGLYPH
			PUNCTUTATION
		}
		if(u.uhoon && u.uhoon - !!u.uhoon_duration){
			num = u.uhoon - !!u.uhoon_duration;
			total += num;
			if(num == 1)
				Sprintf(eos(msgbuf), "a %s", OBJ_DESCR(objects[SYLLABLE_OF_LIFE__HOON]));
			else
				Sprintf(eos(msgbuf), "%d %ss", num, OBJ_DESCR(objects[SYLLABLE_OF_LIFE__HOON]));
			count--;
			STRIPGLYPH
			PUNCTUTATION
		}
		if(u.uuur && u.uuur - !!u.uuur_duration){
			num = u.uuur - !!u.uuur_duration;
			total += num;
			if(num == 1)
				Sprintf(eos(msgbuf), "a %s", OBJ_DESCR(objects[SYLLABLE_OF_GRACE__UUR]));
			else
				Sprintf(eos(msgbuf), "%d %ss", num, OBJ_DESCR(objects[SYLLABLE_OF_GRACE__UUR]));
			count--;
			STRIPGLYPH
			PUNCTUTATION
		}
		if(u.unaen && u.unaen - !!u.unaen_duration){
			num = u.unaen - !!u.unaen_duration;
			total += num;
			if(num == 1)
				Sprintf(eos(msgbuf), "a %s", OBJ_DESCR(objects[SYLLABLE_OF_THOUGHT__NAEN]));
			else
				Sprintf(eos(msgbuf), "%d %ss", num, OBJ_DESCR(objects[SYLLABLE_OF_THOUGHT__NAEN]));
			count--;
			STRIPGLYPH
			PUNCTUTATION
		}
		if(u.uvaul && u.uvaul - !!u.uvaul_duration){
			num = u.uvaul - !!u.uvaul_duration;
			total += num;
			if(num == 1)
				Sprintf(eos(msgbuf), "a %s", OBJ_DESCR(objects[SYLLABLE_OF_SPIRIT__VAUL]));
			else
				Sprintf(eos(msgbuf), "%d %ss", num, OBJ_DESCR(objects[SYLLABLE_OF_SPIRIT__VAUL]));
			count--;
			STRIPGLYPH
			PUNCTUTATION
		}
		// Sprintf(eos(msgbuf), " glyph%s %s around your head.", total == 1 ? "" : "s", total == 1 ? "drifts" : "drift");
		Sprintf(eos(msgbuf), " %s around your head.", total == 1 ? "drifts" : "drift");
		msgbuf[0] = highc(msgbuf[0]);
		if(msgbuf[0]) putstr(en_win, 0, msgbuf);
	}
#undef STRIPGLYPH
#undef PUNCTUTATION
	
	if(u.sealsActive&SEAL_AHAZU && !NoBInvis){
		if(!(ublindf && ublindf->otyp==MASK)){
			putstr(en_win, 0, "There is a starry void in your throat.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_AMON && !Invis){
		if(!(uarmh && is_metallic(uarmh))){
			putstr(en_win, 0, "You have a pair of large ram's horns.");
		} else putstr(en_win, 0, "Your ram's horns have fused with your helm, taking on a metalic hue.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_ANDROMALIUS && !NoBInvis){
		if((levl[u.ux][u.uy].lit == 0 && !(viz_array[u.uy][u.ux]&TEMP_LIT1 && !(viz_array[u.uy][u.ux]&TEMP_DRK3))))
			putstr(en_win, 0, "Your rigid features can't be seen in the dark.");
		else if((ublindf && (ublindf->otyp==MASK || ublindf->otyp==R_LYEHIAN_FACEPLATE)) //face-covering mask
			 || (uarmh && (uarmh->otyp==PLASTEEL_HELM || uarmh->otyp==PONTIFF_S_CROWN)) //face-covering helm
			 || (uarmc && (uarmc->otyp==WHITE_FACELESS_ROBE || uarmc->otyp==BLACK_FACELESS_ROBE || uarmc->otyp==SMOKY_VIOLET_FACELESS_ROBE))//face-covering robe
		) putstr(en_win, 0, "Your rigid features can't be seen through your disguise.");
		else putstr(en_win, 0, "Your features have taken on the rigidity of a cheap disguise.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_ASTAROTH && !Invis){
		if(!ublindf || ublindf->otyp == LENSES || ublindf->otyp == LIVING_MASK)
			putstr(en_win, 0, "A black liquid leaks from around your eyes.");
		else if(ublindf && (ublindf->otyp == MASK || ublindf->otyp == R_LYEHIAN_FACEPLATE))
			putstr(en_win, 0, "The black liquid leaking from your eyes is hidden by your mask.");
		else
			putstr(en_win, 0, "The black liquid leaking from your eyes is soaked up by your blindfold.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_BALAM && !Invis){
		if(uarmc || uarm)
			putstr(en_win, 0, "Freezing water leaks from a gash in you neck, but is hidden by your clothes.");
		else
			putstr(en_win, 0, "Freezing water leaks from a deep gash in you neck.");
		if(!uarmg)
			putstr(en_win, 0, "Freezing water leaks from deep holes in your wrists.");
		if(!uarmf)
			putstr(en_win, 0, "Freezing water leaks from deep holes in your ankles.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_BERITH && !Invis){
		if(!(uarm && is_metallic(uarm) && uarmg && uarmf && uarmh))
			putstr(en_win, 0, "You are drenched in gore.");
		else
			putstr(en_win, 0, "Your armor is faced with crimson enamel.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_BUER && !Invis){
		if(!uarmf)
			putstr(en_win, 0, "Your legs bifurcate into twin pairs of cloven-hoved feet.");
		else
			putstr(en_win, 0, "Your strange feet are hidden in your shoes.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_CHUPOCLOPS && !NoBInvis){
		if(!(ublindf && ublindf->otyp==MASK)){
			putstr(en_win, 0, "You see a pair of chelicerae in your mouth!");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_DANTALION && !NoBInvis){
		if(!(uarmc || ((uarm && uarm->otyp != CRYSTAL_PLATE_MAIL) || uarmu))){
			switch(u.ulevel/10+1){
				case 1:
				putstr(en_win, 0, "There is an extra face growing on your chest.");
				break;
				case 2:
				putstr(en_win, 0, "There is a pair of faces growing on your chest.");
				break;
				case 3:
				putstr(en_win, 0, "Many extra faces grow on your chest.");
				break;
				case 4:
				putstr(en_win, 0, "There are legions of faces growing on your chest.");
				break;
			}
		}
		else{
			if(u.ulevel/10){
				putstr(en_win, 0, "Your extra faces are covered by your clothes.");
			} else
				putstr(en_win, 0, "Your extra face is covered by your clothes.");
		}
		message = TRUE;
	}
	// if(u.sealsActive&SEAL_SHIRO);
	if(u.sealsActive&SEAL_ECHIDNA && !Invis){
		if(!(uarmf && (uarmc || uarm)))
			putstr(en_win, 0, "Your hips give rise to twin serpent's tails instead of legs.");
		else
			putstr(en_win, 0, "Your serpentine legs are disguised by your clothes.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_EDEN && !NoBInvis){
		putstr(en_win, 0, "You see a garden through the dome of cerulean crystal embedded in your head!");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_ENKI && !Invis){
		if(!(uarm || uarmc))
			putstr(en_win, 0, "Water runs off your body in steady rivulets.");
		else
			putstr(en_win, 0, "Your body's runoff is caught by your clothes.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_EVE && !NoBInvis){
		if(!uarm && !uarmc){
			putstr(en_win, 0, "There is a blood-caked wound on your stomach.");
			message = TRUE;
		}
		if(!uarmf){
			putstr(en_win, 0, "Your feet are torn by thorns and stones.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_FAFNIR && !NoBInvis){ 
		if(!(uright || uarmg)){
			putstr(en_win, 0, "There is a ring-shaped burn scar around your right ring-finger.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_HUGINN_MUNINN && !NoBInvis){
		if(!(ublindf && ublindf->otyp==MASK)){
			putstr(en_win, 0, "You find a raven nesting in each ear!");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_IRIS && !NoBInvis){
		if(moves <= u.irisAttack+1){
			putstr(en_win, 0, "Waving, iridescent tentacles sprout from your forearms.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_JACK && !NoBInvis){
		if(!uarmc){
			putstr(en_win, 0, "You see an old, old man on your back!");
		} else {
			putstr(en_win, 0, "You see a bump under your cloak on your back.");
		}
		message = TRUE;
	}
	if(u.sealsActive&SEAL_MALPHAS && !NoBInvis){
		if(!(ublindf && ublindf->otyp==MASK)){
			putstr(en_win, 0, "There is a whole flock's worth of crows peeking out of your throat!");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_MARIONETTE && !NoBInvis){
		if(!(uarm && is_metallic(uarm)))
			putstr(en_win, 0, "Metal wires protrude from your elbows, knees, and back.");
		else
			putstr(en_win, 0, "The metal wires protruding from your body have merged with your armor.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_MOTHER && !NoBInvis){
		if(!uarmg)
			putstr(en_win, 0, "The eyes on your fingers and palms stare back at you.");
		else
			putstr(en_win, 0, "The eyes on your fingers and palms are covered up.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_NABERIUS && !NoBInvis){
		if(!(ublindf && ublindf->otyp==MASK)){
			putstr(en_win, 0, "Your tongue is forked!");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_ORTHOS && !NoBInvis){
		if(uarmc && uarmc->otyp != MUMMY_WRAPPING){
			putstr(en_win, 0, "Your cloak blows in a nonexistent wind.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_OSE && !Blind && !BClairvoyant && !(uarmh && is_metallic(uarmh) && uarmh->otyp != HELM_OF_TELEPATHY)){
			putstr(en_win, 0, "You feel your gaze as a tangible force.");
			message = TRUE;
	}
	if(u.sealsActive&SEAL_OTIAX && !Invis){
		if(moves <= u.otiaxAttack+5){
			putstr(en_win, 0, "The mist around you is formed into writhing tendrils.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_PAIMON && !Invis){ 
		if(!uarmh)
			putstr(en_win, 0, "There is a crown floating over your head.");
		else
			putstr(en_win, 0, "There is a crown sitting on your helm.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_SIMURGH && !Invis){
		if(!uarmg)
			putstr(en_win, 0, "You have iron claws.");
		else
			putstr(en_win, 0, "Your iron claws seem to be part of you gloves.");
		if(!uarmh)
			putstr(en_win, 0, "There is a pair of prismatic wings reaching around your head.");
		else
			putstr(en_win, 0, "Your helm has a crest of prismatic feathers.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_YMIR && !Invis){
		if(moves>5000 && moves <= 10000){
			if(!(uarm || uarmc))
				putstr(en_win, 0, "Your skin color is a bit off.");
			else
				putstr(en_win, 0, "Your skin is hidden under your clothes.");
			message = TRUE;
		} else if(moves>10000 && moves <= 20000){
			if(!(uarmc))
				putstr(en_win, 0, "Maggots burrow through your skin.");
			else
				putstr(en_win, 0, "Your rot is hidden under your clothes.");
			message = TRUE;
		} else if(moves>20000 && moves <= 50000){
			if(!(uarmc && uarmg && uarmf))
				putstr(en_win, 0, "Your skin is rotting off.");
			else
				putstr(en_win, 0, "Your rot is hidden under your clothes.");
			message = TRUE;
		} else if(moves>50000 && moves <= 100000){
			if(!(uarmc && uarmg && uarmf && uarm && uarmh))
				putstr(en_win, 0, "Your rotted body bristles with fungal sporangia and burrowing vermin.");
			else
				putstr(en_win, 0, "Your rotted form is hidden under your clothes.");
			message = TRUE;
		} else if(moves>100000){
			if(!(uarmc && uarmg && uarmf && uarm && uarmh && ublindf && ublindf->otyp==MASK))
				putstr(en_win, 0, "Your putrid body is a riot of fungal forms and saprophagous insects.");
			else
				putstr(en_win, 0, "Your putrid form is hidden under your clothes.");
			message = TRUE;
		}
	}
	if(u.specialSealsActive&SEAL_DAHLVER_NAR && !NoBInvis){
		if(dahlverNarVis()){
			putstr(en_win, 0, "Your wounds are full of sharp teeth!");
			message = TRUE;
		}
	}
	if(u.specialSealsActive&SEAL_ACERERAK && !NoBInvis && !(ublindf && ublindf->otyp != LENSES && ublindf->otyp != LIVING_MASK)){
		putstr(en_win, 0, "You have gemstones for eyes!");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_COUNCIL && !NoBInvis){
		if(!ublindf || ublindf->otyp == LENSES || ublindf->otyp == LIVING_MASK)
			putstr(en_win, 0, "Your eyes shine like stars.");
		else if(ublindf && (ublindf->otyp == MASK))
			putstr(en_win, 0, "Your star-like eyes shine through your mask.");
		else if(ublindf && (ublindf->otyp == R_LYEHIAN_FACEPLATE))
			putstr(en_win, 0, "Your star-like eyes are covered by your opaque mask.");
		else
			putstr(en_win, 0, "Your star-like eyes are covered by your blindfold.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_COSMOS){
		putstr(en_win, 0, "A bright, crystalline aureola hangs behind you.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_LIVING_CRYSTAL){
		putstr(en_win, 0, "Broken rings of fragmentary glyphs form and dissolve in the dustlight behind you.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_TWO_TREES){
		putstr(en_win, 0, "Glorious dappled light dances on your body.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_MISKA && !Invis && u.ulevel >= 10){
		static char mbuf[BUFSZ] = {'\0'};
		if(u.ulevel >= 26){
			int howManyArms = (youracedata->mtyp == PM_VALAVI) ? 6 : 
						  (youracedata->mtyp == PM_MAN_SERPENT_MAGE) ? 6 : 
						  (youracedata->mtyp == PM_PHALANX) ? 6 : 
						  (youracedata->mtyp == PM_MARILITH) ? 8 : 
						  (youracedata->mtyp == PM_KARY__THE_FIEND_OF_FIRE) ? 8 : 
						  (youracedata->mtyp == PM_CATHEZAR) ? 8 : 
						  (youracedata->mtyp == PM_SHAKTARI) ? 8 : 
						  4;
			Sprintf(mbuf, "You have %d arms, and a wolf head grows from each hip.", howManyArms);
			putstr(en_win, 0, mbuf);
		} else if(u.ulevel >= 18) {
			putstr(en_win, 0, "You have a wolf head growing from each hip.");
		} else {
			putstr(en_win, 0, "You have a wolf head growing from your lower stomach.");
		}
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_NUDZIRATH){
		putstr(en_win, 0, "A nimbus of tiny mirrored shards surrounds your head.");
		message = TRUE;
	}
	if(u.specialSealsActive&SEAL_ALIGNMENT_THING){
		putstr(en_win, 0, "You see a small black halo just behind your head. There is an eye in the center, staring at you!");
		message = TRUE;
	}
	
	if(!message){
		putstr(en_win, 0, "You think you look pretty normal.");
	}
	
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
	return;
}

/*
 * Courtesy function for non-debug, non-explorer mode players
 * to help refresh them about who/what they are.
 * Returns FALSE if menu cancelled (dismissed with ESC), TRUE otherwise.
 */
STATIC_OVL int
minimal_enlightenment()
{
	winid tmpwin;
	menu_item *selected;
	anything any;
	int genidx, n;
	char buf[BUFSZ], buf2[BUFSZ];
	static const char untabbed_fmtstr[] = "%-15s: %-12s";
	static const char untabbed_deity_fmtstr[] = "%-17s%s";
	static const char tabbed_fmtstr[] = "%s:\t%-12s";
	static const char tabbed_deity_fmtstr[] = "%s\t%s";
	static const char *fmtstr;
	static const char *deity_fmtstr;

	fmtstr = iflags.menu_tab_sep ? tabbed_fmtstr : untabbed_fmtstr;
	deity_fmtstr = iflags.menu_tab_sep ?
			tabbed_deity_fmtstr : untabbed_deity_fmtstr; 
	any.a_void = 0;
	buf[0] = buf2[0] = '\0';
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Starting", FALSE);

	/* Starting name, race, role, gender */
	Sprintf(buf, fmtstr, "name", plname);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr, "race", urace.noun);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr, "role",
		(flags.initgend && urole.name.f) ? urole.name.f : urole.name.m);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	Sprintf(buf, fmtstr, "gender", genders[flags.initgend].adj);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Starting alignment */
	Sprintf(buf, fmtstr, "alignment", align_str(u.ualignbase[A_ORIGINAL]));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Current name, race, role, gender */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Current", FALSE);
	Sprintf(buf, fmtstr, "race", Upolyd ? youmonst.data->mname : urace.noun);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	if (Upolyd) {
	    Sprintf(buf, fmtstr, "role (base)",
		(u.mfemale && urole.name.f) ? urole.name.f : urole.name.m);
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	} else {
	    Sprintf(buf, fmtstr, "role",
		(flags.female && urole.name.f) ? urole.name.f : urole.name.m);
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}
	/* don't want poly_gender() here; it forces `2' for non-humanoids */
	genidx = is_neuter(youracedata) ? 2 : flags.female;
	Sprintf(buf, fmtstr, "gender", genders[genidx].adj);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	if (Upolyd && (int)u.mfemale != genidx) {
	    Sprintf(buf, fmtstr, "gender (base)", genders[u.mfemale].adj);
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}

	/* Current alignment */
	Sprintf(buf, fmtstr, "alignment", align_str(u.ualign.type));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Deity list */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Deities", FALSE);
	Sprintf(buf2, deity_fmtstr, align_gname(A_CHAOTIC),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
		&& u.ualign.type == A_CHAOTIC) ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_CHAOTIC)       ? " (s)" :
	    (u.ualign.type   == A_CHAOTIC)       ? " (c)" : "");
	if(Role_if(PM_EXILE) && Is_astralevel(&u.uz)) Sprintf(buf, fmtstr, "Xaotic", buf2);
	else Sprintf(buf, fmtstr, "Chaotic", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	Sprintf(buf2, deity_fmtstr, align_gname(A_NEUTRAL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type
		&& u.ualign.type == A_NEUTRAL) ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_NEUTRAL)       ? " (s)" :
	    (u.ualign.type   == A_NEUTRAL)       ? " (c)" : "");
	if(Role_if(PM_EXILE) && Is_astralevel(&u.uz)) Sprintf(buf, fmtstr, "Gnostic", buf2);
	else Sprintf(buf, fmtstr, "Neutral", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	Sprintf(buf2, deity_fmtstr, align_gname(A_LAWFUL),
	    (u.ualignbase[A_ORIGINAL] == u.ualign.type &&
		u.ualign.type == A_LAWFUL)  ? " (s,c)" :
	    (u.ualignbase[A_ORIGINAL] == A_LAWFUL)        ? " (s)" :
	    (u.ualign.type   == A_LAWFUL)        ? " (c)" : "");
	if(Role_if(PM_EXILE) && Is_astralevel(&u.uz)) Sprintf(buf, fmtstr, "Mundane", buf2);
	else Sprintf(buf, fmtstr, "Lawful", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);

	/* Menu options */
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
	if (TRUE) {
		Sprintf(buf, "Describe how you feel.");
		any.a_int = DOATTRIB_RESISTS;
		add_menu(tmpwin, NO_GLYPH, &any,
			'a', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		//resistances_enlightenment();
	}

	if (TRUE) {
		Sprintf(buf, "Show your armor's damage reduction.");
		any.a_int = DOATTRIB_ARMOR;
		add_menu(tmpwin, NO_GLYPH, &any,
			'b', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		//udr_enlightenment();
	}

	if (wizard || discover) {
		Sprintf(buf, "Perform full enlightenment.");
		any.a_int = DOATTRIB_ENLIGHTEN;
		add_menu(tmpwin, NO_GLYPH, &any,
			'c', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		//enlightenment(0);
	}
	if (u.sealsActive || u.specialSealsActive) {
		Sprintf(buf, "Describe your binding marks.");
		any.a_int = DOATTRIB_BINDINGS;
		add_menu(tmpwin, NO_GLYPH, &any,
			'd', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		//signs_enlightenment();
	}
	if (u.sealsKnown || u.specialSealsKnown || u.sealsActive || u.specialSealsActive) {
		Sprintf(buf, "Show your bound spirits.");
		any.a_int = DOATTRIB_SPIRITS;
		add_menu(tmpwin, NO_GLYPH, &any,
			'e', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		//spirits_enlightenment();
	}

	end_menu(tmpwin, "Base Attributes");
	n = select_menu(tmpwin, PICK_ONE, &selected);
	destroy_nhwindow(tmpwin);
	return (n>0 ? selected[0].item.a_int : 0);
}

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

      any.a_int = 0;
      add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, "", MENU_UNSELECTED);

      end_menu(win, "What do you wish to do?");
      n = select_menu(win, PICK_ONE, &pick_list);
      destroy_nhwindow(win);

      if (pick_list) {
	n = (pick_list[0].item.a_int - 1);
	free((genericptr_t) pick_list);
      } else return 0;
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
		return 0;
	    }
	    docall(obj);
	}
	break;
    case 3: dodiscovered(); break;
    }
    return 0;
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


STATIC_PTR int
doattributes()
{
	int choice;
	
	/* ends via return */
	while (TRUE) {
		choice = minimal_enlightenment();
		switch (choice)
		{
		case DOATTRIB_RESISTS:
			resistances_enlightenment();
			break;
		case DOATTRIB_ARMOR:
			udr_enlightenment();
			break;
		case DOATTRIB_ENLIGHTEN:
			enlightenment(0);
			break;
		case DOATTRIB_BINDINGS:
			signs_enlightenment();
			break;
		case DOATTRIB_SPIRITS:
			spirits_enlightenment();
			break;
		default:
			return 0;
		}
	}
	return 0;
}

/* KMH, #conduct
 * (shares enlightenment's tense handling)
 */
STATIC_PTR int
doconduct()
{
	show_conduct(0);
	return 0;
}

void
show_conduct(final)
int final;
{
	char buf[BUFSZ];
	int ngenocided;

	/* Create the conduct window */
	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, "Voluntary challenges:");
	putstr(en_win, 0, "");

	if (!u.uconduct.food)
	    enl_msg(You_, "have gone", "went", " without food");
	    /* But beverages are okay */
	else if (!u.uconduct.unvegan)
	    you_have_X("followed a strict vegan diet");
	else if (!u.uconduct.unvegetarian)
	    you_have_been("vegetarian");

	if (!u.uconduct.gnostic)
	    you_have_been("an atheist");

	if (!u.uconduct.weaphit)
	    you_have_never("hit with a wielded weapon");
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "used a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    you_have_X(buf);
	}
#endif
	if (!u.uconduct.killer)
	    you_have_been("a pacifist");

	if (!u.uconduct.literate)
	    you_have_been("illiterate");
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "read items or engraved %ld time%s",
		    u.uconduct.literate, plur(u.uconduct.literate));
	    you_have_X(buf);
	}
#endif

	ngenocided = num_genocides();
	if (ngenocided == 0) {
	    you_have_never("genocided any monsters");
	} else {
	    Sprintf(buf, "genocided %d type%s of monster%s",
		    ngenocided, plur(ngenocided), plur(ngenocided));
	    you_have_X(buf);
	}

	if (!u.uconduct.polypiles)
	    you_have_never("polymorphed an object");
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "polymorphed %ld item%s",
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    you_have_X(buf);
	}
#endif

	if (!u.uconduct.polyselfs)
	    you_have_never("changed form");
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "changed form %ld time%s",
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    you_have_X(buf);
	}
#endif

	if (!u.uconduct.wishes)
	    you_have_X("used no wishes");
	else {
	    Sprintf(buf, "used %ld wish%s",
		    u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : "");
	    you_have_X(buf);

	    if (!u.uconduct.wisharti)
		enl_msg(You_, "have not wished", "did not wish",
			" for any artifacts");
	}

	if (u.uconduct.shopID == 0) {
	    you_have_never("paid a shopkeeper to identify an item");
	} else {
	    Sprintf(buf, "paid to have %ld item%s identified",
		    u.uconduct.shopID, plur(u.uconduct.shopID));
	    you_have_X(buf);
	}
	if (u.uconduct.IDs == 0) {
	    you_have_never("magically identified an item");
	} else {
	    Sprintf(buf, "magically identified %ld item%s",
		    u.uconduct.IDs, plur(u.uconduct.IDs));
	    you_have_X(buf);
	}
	/* Pop up the window and wait for a key */
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
}

#ifdef DUMP_LOG
void
dump_conduct(final)
int final;
{
	char buf[BUFSZ];
	int ngenocided;

	dump("", "Voluntary challenges");

	if (!u.uconduct.food)
	    dump("", "  You went without food");
	    /* But beverages are okay */
	else if (!u.uconduct.unvegan)
	    dump("", "  You followed a strict vegan diet");
	else if (!u.uconduct.unvegetarian)
	    dump("", "  You were a vegetarian");
	else if (Role_if(PM_MONK) && u.uconduct.unvegetarian < 10) {
	    sprintf(buf, "  You ate non-vegetarian food %ld time%s.", 
		u.uconduct.unvegetarian, plur(u.uconduct.unvegetarian));
	    dump("", buf);
	}

	if (!u.uconduct.gnostic)
	    dump("", "  You were an atheist");

	if (!u.uconduct.weaphit)
	    dump("", "  You never hit with a wielded weapon");
	else if (Role_if(PM_MONK) && u.uconduct.weaphit < 10) {
	    Sprintf(buf, "  You hit with a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    dump("", buf);
	}
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "hit with a wielded weapon %ld time%s",
		    u.uconduct.weaphit, plur(u.uconduct.weaphit));
	    dump("  You ", buf);
	}
#endif
	if (!u.uconduct.killer)
	    dump("", "  You were a pacifist");

	if (!u.uconduct.literate)
	    dump("", "  You were illiterate");
#ifdef WIZARD
	else if (wizard) {
	    Sprintf(buf, "read items or engraved %ld time%s",
		    u.uconduct.literate, plur(u.uconduct.literate));
	    dump("  You ", buf);
	}
#endif

	ngenocided = num_genocides();
	if (ngenocided == 0) {
	    dump("", "  You never genocided any monsters");
	} else {
	    Sprintf(buf, "genocided %d type%s of monster%s",
		    ngenocided, plur(ngenocided), plur(ngenocided));
	    dump("  You ", buf);
	}

	if (!u.uconduct.polypiles)
	    dump("", "  You never polymorphed an object");
	else {
	    Sprintf(buf, "polymorphed %ld item%s",
		    u.uconduct.polypiles, plur(u.uconduct.polypiles));
	    dump("  You ", buf);
	}

	if (!u.uconduct.polyselfs)
	    dump("", "  You never changed form");
	else {
	    Sprintf(buf, "changed form %ld time%s",
		    u.uconduct.polyselfs, plur(u.uconduct.polyselfs));
	    dump("  You ", buf);
	}

	if (!u.uconduct.wishes)
	    dump("", "  You used no wishes");
	else {
	    Sprintf(buf, "used %ld wish%s",
		    u.uconduct.wishes, (u.uconduct.wishes > 1L) ? "es" : "");
	    dump("  You ", buf);

	    if (!u.uconduct.wisharti)
		dump("", "  You did not wish for any artifacts");
	}

	if (u.uconduct.shopID == 0) {
	    dump("", "  You never paid a shopkeeper to identify an item");
	} else {
	    Sprintf(buf, "paid to have %ld item%s identified",
		    u.uconduct.shopID, plur(u.uconduct.shopID));
	    dump("  You ", buf);
	}
	if (u.uconduct.IDs == 0) {
	    dump("", "  You never magically identified an item");
	} else {
	    Sprintf(buf, "magically identified %ld item%s",
		    u.uconduct.IDs, plur(u.uconduct.IDs));
	    dump("  You ", buf);
	}
	dump("", "");
}
#endif /* DUMP_LOG */

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
	{"chat", "talk to someone", dotalk, IFBURIED, AUTOCOMPLETE},	/* converse? */
	{"combo", "use an android combo based on your weapon", android_combo, !IFBURIED},
	{"come", "order pets to come", docome, !IFBURIED, AUTOCOMPLETE},
	{"conduct", "list which challenges you have adhered to", doconduct, IFBURIED, AUTOCOMPLETE},
	{"dip", "dip an object into something", dodip, !IFBURIED, AUTOCOMPLETE},
	{"enhance", "advance or check weapons skills", enhance_weapon_skill, IFBURIED, AUTOCOMPLETE},
	{"equip", "give a pet an item", dopetequip, !IFBURIED, AUTOCOMPLETE},
	{"force", "force a lock", doforce, !IFBURIED, AUTOCOMPLETE},
	{"invoke", "invoke an object's powers", doinvoke, IFBURIED, AUTOCOMPLETE},
	{"jump", "jump to a location", dojump, !IFBURIED, AUTOCOMPLETE},
	{"loot", "loot a box on the floor", doloot, !IFBURIED, AUTOCOMPLETE},
	{"monster", "use a monster's special ability", domonability, IFBURIED, AUTOCOMPLETE},
	{"mount", "order mount to attack", domountattk, !IFBURIED, AUTOCOMPLETE},
	{"name", "name an item or type of object", do_naming_ddocall, IFBURIED, AUTOCOMPLETE},
	{"nameold", "name an item or type of object (vanilla)", ddocall, IFBURIED},
	{"offer", "offer a sacrifice to the gods", dosacrifice, !IFBURIED, AUTOCOMPLETE},
	{"overview", "give an overview of dungeon", dooverview, !IFBURIED, AUTOCOMPLETE},
	{"pray", "pray to the gods for help", dopray, IFBURIED, AUTOCOMPLETE},
	{"quit", "exit without saving current game", done2, IFBURIED, AUTOCOMPLETE},
#ifdef STEED
	{"ride", "ride (or stop riding) a monster", doride, !IFBURIED, AUTOCOMPLETE},
#endif
	{"rub", "rub a lamp", dorub, !IFBURIED, AUTOCOMPLETE},
	{"style", "switch fighting style", dofightingform, !IFBURIED, AUTOCOMPLETE},
	{"sit", "sit down", dosit, !IFBURIED, AUTOCOMPLETE},
	{"swim", "swim under water", dodeepswim, !IFBURIED, AUTOCOMPLETE},
	{"turn", "turn undead", doturn, IFBURIED, AUTOCOMPLETE},
	{"tip", "empty a container", dotip, IFBURIED, AUTOCOMPLETE},
	{"twoweapon", "toggle two-weapon combat", dotwoweapon, !IFBURIED, AUTOCOMPLETE},
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
	{(char *)0, (char *)0, donull, TRUE}, /* #identify */
	{(char *)0, (char *)0, donull, TRUE}, /* #levelport */
	{(char *)0, (char *)0, donull, TRUE}, /* #wish */
	{(char *)0, (char *)0, donull, TRUE}, /* #where */
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
	{"dump_map", "dump map glyphs into a file", wiz_mk_mapglyphdump, IFBURIED, AUTOCOMPLETE},
#ifdef DEBUG
	{"wizdebug", "wizard debug command", wiz_debug_cmd, IFBURIED, AUTOCOMPLETE},
#endif
	{"wmode", "show wall modes", wiz_show_wmodes, IFBURIED, AUTOCOMPLETE},
	{"detect", "detect secret doors and traps", wiz_detect, IFBURIED},
	{"map", "do magic mapping", wiz_map, IFBURIED},
	{"genesis", "create monster", wiz_genesis, IFBURIED},
	{"identify", "identify items in pack", wiz_identify, IFBURIED},
	{"levelport", "to trans-level teleport", wiz_level_tele, IFBURIED},
	{"wish", "make wish", wiz_wish, IFBURIED, AUTOCOMPLETE},
	{"where", "tell locations of special levels", dooverview_or_wiz_where, IFBURIED},
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
		size += sizeof(struct obj) + obj->oxlth + obj->onamelth;
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
	    size += sizeof(struct monst) + mon->mxlth + mon->mnamelth;
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
	return 0;
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
	if (*inbuf == '\033') return 0;
	mcount = atoi(inbuf);
	if (mcount < 0 || mcount > (COLNO * ROWNO) || Is_botlevel(&u.uz))
		return 0;
	while (mcount > 0) {
		if (Is_stronghold(&u.uz))
		    assign_level(&tolevel, &valley_level);
		else
		    get_level(&tolevel, depth(&u.uz) + 1);
		ptr = rndmonst();
		mtmp = makemon(ptr, 0, 0, NO_MM_FLAGS);
		if (mtmp) migrate_to_level(mtmp, ledger_no(&tolevel),
				MIGR_RANDOM, (coord *)0);
		mcount--;
	}
	return 0;
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
		flags.move = FALSE;
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
		flags.move = FALSE;
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
	    flags.move = FALSE;
	    multi = 0;
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
		    res = 0;
		} else {
		    func = extcmd->ef_funct;
		    if (extcmd->f_text && !occupation && multi)
			set_occupation(func, extcmd->f_text, multi);
		    last_cmd_char = *cmd;	/* remember pressed character */
		    res = (*func)();		/* perform the command */
		}
		if (!res) {
		    flags.move = FALSE;
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
	}
	/* didn't move */
	flags.move = FALSE;
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
	if(!(dp = index(sdp, sym))) return 0;
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
	flags.move = 1;
	flush_screen(1); /* Flush screen buffer. Put the cursor on the hero. */

	if (!iflags.num_pad || (foo = readchar()) == 'n')
	    for (;;) {
		foo = readchar();
		if (foo >= '0' && foo <= '9') {
		    multi = 10 * multi + foo - '0';
		    if (multi < 0 || multi >= LARGEST_INT) multi = LARGEST_INT;
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

	if (!iflags.travelcmd) return 0;
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
		return 0;
	}
	iflags.travelcc.x = u.tx = cc.x;
	iflags.travelcc.y = u.ty = cc.y;
	cmd[0] = CMD_TRAVEL;
	readchar_queue = cmd;
	return 0;
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

/*cmd.c*/
