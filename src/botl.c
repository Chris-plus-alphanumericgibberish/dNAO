/*	SCCS Id: @(#)botl.c	3.4	1996/07/15	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#ifdef TTY_GRAPHICS
# include "wintty.h"
#endif

#ifdef OVL0
extern const char *hu_stat[];	/* defined in eat.c */
extern const char *ca_hu_stat[];	/* defined in eat.c */

const char * const enc_stat[] = {
	"",
	"Burdened",
	"Stressed",
	"Strained",
	"Overtaxed",
	"Overloaded"
};

const char * const enc_stat_abbrev1[] = {
	"",
	"Burden",
	"Stress",
	"Strain",
	"Overtax",
	"Overload"
};

const char * const enc_stat_abbrev2[] = {
	"",
	"Brd",
	"Strs",
	"Strn",
	"Ovtx",
	"Ovld"
};

STATIC_DCL void NDECL(bot1);
STATIC_DCL void NDECL(bot2);
#endif /* OVL0 */

#if defined(STATUS_COLORS) && defined(TEXTCOLOR)

extern const struct percent_color_option *hp_colors;
extern const struct percent_color_option *pw_colors;
extern const struct text_color_option *text_colors;

struct color_option
text_color_of(text, color_options)
     const char *text;
     const struct text_color_option *color_options;
{
    if (color_options == NULL) {
	struct color_option result = {NO_COLOR, 0};
	return result;
    }
    if (strstri(color_options->text, text)
	|| strstri(text, color_options->text))
	return color_options->color_option;
    return text_color_of(text, color_options->next);
}

struct color_option
percentage_color_of(value, max, color_options)
     int value, max;
     const struct percent_color_option *color_options;
{
    if (color_options == NULL) {
	struct color_option result = {NO_COLOR, 0};
	return result;
    }
    switch (color_options->statclrtype) {
    default:
    case STATCLR_TYPE_PERCENT:
	if (100 * value <= color_options->percentage * max)
	    return color_options->color_option;
	break;
    case STATCLR_TYPE_NUMBER_EQ:
	if (value == color_options->percentage)
	    return color_options->color_option;
	break;
    case STATCLR_TYPE_NUMBER_LT:
	if (value < color_options->percentage)
	    return color_options->color_option;
	break;
    case STATCLR_TYPE_NUMBER_GT:
	if (value > color_options->percentage)
	    return color_options->color_option;
	break;
    }
    return percentage_color_of(value, max, color_options->next);
}

void
start_color_option(color_option)
     struct color_option color_option;
{
#ifdef TTY_GRAPHICS
    int i;
    if (color_option.color != NO_COLOR)
	term_start_color(color_option.color);
    for (i = 0; (1 << i) <= color_option.attr_bits; ++i)
	if (i != ATR_NONE && color_option.attr_bits & (1 << i))
	    term_start_attr(i);
#endif  /* TTY_GRAPHICS */
}

void
end_color_option(color_option)
     struct color_option color_option;
{
#ifdef TTY_GRAPHICS
    int i;
    if (color_option.color != NO_COLOR)
	term_end_color();
    for (i = 0; (1 << i) <= color_option.attr_bits; ++i)
	if (i != ATR_NONE && color_option.attr_bits & (1 << i))
	    term_end_attr(i);
#endif  /* TTY_GRAPHICS */
}

static
void
apply_color_option(color_option, newbot2, statusline)
     struct color_option color_option;
     const char *newbot2;
     int statusline; /* apply color on this statusline: 1, 2, or 3 */
{
    if (!iflags.use_status_colors || !iflags.use_color) return;
    curs(WIN_STATUS, 1, statusline-1);
    start_color_option(color_option);
    putstr(WIN_STATUS, 0, newbot2);
    end_color_option(color_option);
}

void
add_colored_text(const char *hilite, const char *text, char *newbot2,
                 boolean terminal_output, int statusline, boolean first,
                 long duration)
{
    char *nb;
    struct color_option color_option;
    int maxlength = (terminal_output ? min(MAXCO, CO) : MAXCO) - 1;

    if (*text == '\0') return;

    /* don't add anything if it can't be displayed.
     * Otherwise the color of invisible text may bleed into
     * the statusline. */
    if (strlen(newbot2) >= maxlength) return;

    if (!iflags.use_status_colors || !terminal_output) {
        if (duration)
            Snprintf(nb = eos(newbot2), MAXCO - strlen(newbot2), first ? "%s:%ld" : " %s:%ld", text, duration);
        else
            Snprintf(nb = eos(newbot2), MAXCO - strlen(newbot2), first ? "%s" : " %s", text);
        return;
    }

    Strncat(nb = eos(newbot2), first ? "" : " ", MAXCO - strlen(newbot2));
    curs(WIN_STATUS, 1, statusline-1);
    putstr(WIN_STATUS, 0, newbot2);

    Strncat(nb = eos(nb), text, MAXCO - strlen(newbot2));
    if (duration)
        Snprintf(nb = eos(nb), MAXCO - strlen(newbot2), ":%ld", duration);
    curs(WIN_STATUS, 1, statusline-1);
    color_option = text_color_of(hilite, text_colors);
    start_color_option(color_option);
    /* Trim the statusline to always have the end color
     * to have effect. */
    newbot2[max(0, maxlength)] = '\0';
    putstr(WIN_STATUS, 0, newbot2);
    end_color_option(color_option);
}

#endif

#ifndef OVLB
STATIC_DCL int mrank_sz;
#else /* OVLB */
STATIC_OVL NEARDATA int mrank_sz = 0; /* loaded by max_rank_sz (from u_init) */
#endif /* OVLB */

STATIC_DCL const char *NDECL(rank);

#ifdef OVL1

/* convert experience level (1..30) to rank index (0..8) */
int
xlev_to_rank(xlev)
int xlev;
{
	return (xlev <= 2) ? 0 : (xlev <= 30) ? ((xlev + 2) / 4) : 8;
}

#if 0	/* not currently needed */
/* convert rank index (0..8) to experience level (1..30) */
int
rank_to_xlev(rank)
int rank;
{
	return (rank <= 0) ? 1 : (rank <= 8) ? ((rank * 4) - 2) : 30;
}
#endif

const char *
code_of(monnum)
	short monnum;
{
	register struct Role *role;
	register int i;


	for (role = (struct Role *) roles; role->name.m; role++)
	    if (monnum == role->malenum || monnum == role->femalenum)
	    	break;
	
	if(role) return role->filecode;
	// else return ("Player");
	return ("Plr");
}

const char *
rank_of(lev, monnum, female)
	int lev;
	short monnum;
	boolean female;
{
	register struct Role *role;
	register int i;

	if(monnum == PM_INCANTIFIER)
		monnum = PM_WIZARD;
	if(monnum == PM_HALF_DRAGON)
		monnum = PM_BARBARIAN;
	if(monnum == PM_ITINERANT_PRIESTESS)
		monnum = PM_PRIESTESS;

	/* Find the role */
	if(Role_if(monnum)) role = &urole;
	else for (role = (struct Role *) roles; role->name.m; role++)
	    if (monnum == role->malenum || monnum == role->femalenum)
	    	break;
	
	if (!role->name.m)
	    role = &urole;

	/* Find the rank */
	for (i = xlev_to_rank((int)lev); i >= 0; i--) {
	    if (female && role->rank[i].f) return (role->rank[i].f);
	    if (role->rank[i].m) return (role->rank[i].m);
	}

	/* Try the role name, instead */
	if (female && role->name.f) return (role->name.f);
	else if (role->name.m) return (role->name.m);
	return ("Player");
}


STATIC_OVL const char *
rank()
{
	int i;
	/* Find the rank */
	for (i = xlev_to_rank((int)(u.ulevel)); i >= 0; i--) {
	    if (flags.female && urole.rank[i].f) return (urole.rank[i].f);
	    if (urole.rank[i].m) return (urole.rank[i].m);
	}

	/* Try the role name, instead */
	if (flags.female && urole.name.f) return (urole.name.f);
	else if (urole.name.m) return (urole.name.m);
	return ("Player");
	// return(rank_of(u.ulevel, Role_switch, flags.female));
}

int
title_to_mon(str, rank_indx, title_length)
const char *str;
int *rank_indx, *title_length;
{
	register int i, j;


	/* Loop through each of the roles */
	for (i = 0; roles[i].name.m; i++)
	    for (j = 0; j < 9; j++) {
	    	if (roles[i].rank[j].m && !strncmpi(str,
	    			roles[i].rank[j].m, strlen(roles[i].rank[j].m))) {
	    	    if (rank_indx) *rank_indx = j;
	    	    if (title_length) *title_length = strlen(roles[i].rank[j].m);
	    	    return roles[i].malenum;
	    	}
	    	if (roles[i].rank[j].f && !strncmpi(str,
	    			roles[i].rank[j].f, strlen(roles[i].rank[j].f))) {
	    	    if (rank_indx) *rank_indx = j;
	    	    if (title_length) *title_length = strlen(roles[i].rank[j].f);
	    	    return ((roles[i].femalenum != NON_PM) ?
	    	    		roles[i].femalenum : roles[i].malenum);
	    	}
	    }
	return NON_PM;
}

#endif /* OVL1 */
#ifdef OVLB

void
max_rank_sz()
{
	register int i, r, maxr = 0;
	for (i = 0; i < 9; i++) {
	    if (urole.rank[i].m && (r = strlen(urole.rank[i].m)) > maxr) maxr = r;
	    if (urole.rank[i].f && (r = strlen(urole.rank[i].f)) > maxr) maxr = r;
	}
	mrank_sz = maxr;
	return;
}

#endif /* OVLB */
#ifdef OVL0

#ifdef SCORE_ON_BOTL
long
botl_score()
{
    int deepest = deepest_lev_reached(FALSE);
#ifndef GOLDOBJ
    long ugold = u.ugold + hidden_gold();

    if ((ugold -= u.ugold0) < 0L) ugold = 0L;
    return ugold + u.urexp + (long)(50 * (deepest - 1))
#else
    long umoney = money_cnt(invent) + hidden_gold();

    if ((umoney -= u.umoney0) < 0L) umoney = 0L;
    return umoney + u.urexp + (long)(50 * (deepest - 1))
#endif
			  + (long)(deepest > 30 ? 10000 :
				   deepest > 20 ? 1000*(deepest - 20) : 0);
}
#endif

#ifdef DUMP_LOG
void bot1str(char *newbot1)
#else
STATIC_OVL void
bot1()
#endif
{
#ifndef DUMP_LOG
	char newbot1[MAXCO];
#endif
	register char *nb;
	register int i=0,j;
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
        int save_botlx = flags.botlx;
#endif

        Strcpy(newbot1, "");
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
        if (iflags.hitpointbar) {
	    flags.botlx = 0;
	    curs(WIN_STATUS, 1, 0);
	    putstr(WIN_STATUS, 0, newbot1);
	    Strcat(newbot1, "[");
	    i = 1; /* don't overwrite the string in front */
	    curs(WIN_STATUS, 1, 0);
	    putstr(WIN_STATUS, 0, newbot1);
        }
#endif

	Strcat(newbot1, plname);
	if('a' <= newbot1[i] && newbot1[i] <= 'z') newbot1[i] += 'A'-'a';
	newbot1[10] = '\0';
	Sprintf(nb = eos(newbot1)," the ");

	if (Upolyd) {
		char mbot[BUFSZ];
		int k = 0;

		Strcpy(mbot, mons[u.umonnum].mname);
		while(mbot[k] != 0) {
		    if ((k == 0 || (k > 0 && mbot[k-1] == ' ')) &&
					'a' <= mbot[k] && mbot[k] <= 'z')
			mbot[k] += 'A' - 'a';
		    k++;
		}
		Sprintf(nb = eos(nb), "%s", mbot);
	} else
	    Sprintf(nb = eos(nb), "%s", rank());

#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
        if (iflags.hitpointbar) {
	    int bar_length = strlen(newbot1)-1;
	    char tmp[MAXCO];
	    char *p = tmp;
	    int filledbar = ((uhp() < 0) ? 0 : uhp()) * bar_length / max(1, uhpmax());
	    if (filledbar >= MAXCO-1) { filledbar = MAXCO-2; }
	    Strcpy(tmp, newbot1);
	    p++;

	    /* draw hp bar */
	    if (iflags.use_inverse) term_start_attr(ATR_INVERSE);
	    p[filledbar] = '\0';
	    if (iflags.use_color) {
		/* draw in color mode */
		apply_color_option(percentage_color_of(uhp(), uhpmax(), hp_colors), tmp, 1);
	    } else {
		/* draw in inverse mode */
		curs(WIN_STATUS, 1, 0);
		putstr(WIN_STATUS, 0, tmp);
	    }
	    term_end_color();
	    if (iflags.use_inverse) term_end_attr(ATR_INVERSE);

	    Strcat(newbot1, "]");
        }
#endif

	Sprintf(nb = eos(nb),"  ");
	i = mrank_sz + 15;
	j = (nb + 2) - newbot1; /* aka strlen(newbot1) but less computation */
	if((i - j) > 0)
		Sprintf(nb = eos(nb),"%*s", i-j, " ");	/* pad with spaces */
	if (ACURR(A_STR) > 18) {
		if (ACURR(A_STR) > STR18(100))
		    Sprintf(nb = eos(nb),"St:%2d ",ACURR(A_STR)-20);
		else if (ACURR(A_STR) < STR18(100))
		    Sprintf(nb = eos(nb), "St:18/%02d ",(ACURR(A_STR)-18)*5);
		else
		    Sprintf(nb = eos(nb),"St:18/** ");
	} else
		Sprintf(nb = eos(nb), "St:%-1d ",ACURR(A_STR));
	Sprintf(nb = eos(nb),
		"Dx:%-1d Co:%-1d In:%-1d Wi:%-1d Ch:%-1d",
		ACURR(A_DEX), ACURR(A_CON), ACURR(A_INT), ACURR(A_WIS), ACURR(A_CHA));
	Sprintf(nb = eos(nb), 
			(u.ualign.type == A_CHAOTIC) ? "  Chaotic" :
			(u.ualign.type == A_NEUTRAL) ? "  Neutral" : 
			(u.ualign.type == A_LAWFUL) ?  "  Lawful " :
			(u.ualign.type == A_VOID) ?    "  Gnostic" : "   Other ");
#ifdef SCORE_ON_BOTL
	if (flags.showscore)
	    Sprintf(nb = eos(nb), " S:%ld", botl_score());
#endif
#ifdef DUMP_LOG
}
STATIC_OVL void
bot1()
{
	char newbot1[MAXCO];

	int save_botlx = flags.botlx;
	bot1str(newbot1);
#endif
	curs(WIN_STATUS, 1, 0);
	putstr(WIN_STATUS, 0, newbot1);
	flags.botlx = save_botlx;
}

/* provide the name of the current level for display by various ports */
int
describe_level(buf)
char *buf;
{
	int ret = 1;

	/* TODO:	Add in dungeon name */
	if (Is_knox(&u.uz))
		Sprintf(buf, "%s ", dungeons[u.uz.dnum].dname);
	else if (In_quest(&u.uz))
		Sprintf(buf, "Home %d ", dunlev(&u.uz));
	else if (In_endgame(&u.uz))
		Sprintf(buf,
			Is_astralevel(&u.uz) ? "Astral Plane " : "End Game ");
	else if (In_tower(&u.uz)) {
		if(dunlev(&u.uz) == 4) Sprintf(buf, "Ravine ");
		else Sprintf(buf, "Tower %d ", 5 - dunlev(&u.uz));
	} else if (In_law(&u.uz))
		Sprintf(buf, "Arcadia %d ", (path1_level.dlevel - u.uz.dlevel)+1);
	else if (In_neu(&u.uz)){
		if(In_depths(&u.uz)) Sprintf(buf, "Depths %d ", dunlev(&u.uz)+dungeons[neutral_dnum].num_dunlevs-1);
		else Sprintf(buf, "Outlands %d ", dunlev(&u.uz));
	} else if (In_cha(&u.uz)){
		if(In_FF_quest(&u.uz)){
			if(dungeons[chaos_dnum].entry_lev == u.uz.dlevel) Sprintf(buf, "Ruined Temple ");
			else Sprintf(buf, "Temple %d ", dunlev(&u.uz));
		} else if(In_mithardir_quest(&u.uz)){
			if(dungeons[chaos_dnum].entry_lev == u.uz.dlevel) Sprintf(buf, "Elshava ");
			else Sprintf(buf, "Mithardir %d ", dunlev(&u.uz));
		} else if(In_mordor_quest(&u.uz)){
			if(In_mordor_forest(&u.uz)) Sprintf(buf, "Forest %d ", dunlev(&u.uz));
			else if(Is_ford_level(&u.uz)) Sprintf(buf, "Ford ");
			else if(In_mordor_fields(&u.uz)) Sprintf(buf, "Field ");
			else if(In_mordor_buildings(&u.uz)) Sprintf(buf, "Fortress %d ", dunlev(&u.uz)-5);
			else if(Is_spider_cave(&u.uz)) Sprintf(buf, "Spider ");
			else if(In_mordor_depths(&u.uz)) Sprintf(buf, "Cracks %d ", dunlev(&u.uz)-8);
			else if(In_mordor_borehole(&u.uz)) Sprintf(buf, "Bore %d ", dunlev(&u.uz)-11);
		}
	} else {
		/* ports with more room may expand this one */
		Sprintf(buf, "Dlvl:%-2d ", depth(&u.uz) > 0 ? depth(&u.uz) : depth(&u.uz)-1);
		ret = 0;
	}
	return ret;
}

void
do_statuseffects(char *newbot2, boolean terminal_output, int abbrev, int statusline)
{
  register char *nb = eos(newbot2);
  int cap = near_capacity();
  boolean first = statusline == 3; /* first text shown on line */
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
#define status_effect(str1, str2, str3)                                 \
  (add_colored_text((str1),                                             \
                    abbrev == 2 ? (str3) : abbrev == 1 ? (str2) : (str1), \
                    newbot2, terminal_output, statusline, first, 0),    \
   first = FALSE)
#define status_effect_duration(str1, str2, str3, duration)              \
  (add_colored_text((str1),                                             \
                    abbrev == 2 ? (str3) : abbrev == 1 ? (str2) : (str1), \
                    newbot2, terminal_output, statusline, first,        \
                    duration),                                          \
   first = FALSE)
#else
#define status_effect(str1, str2, str3)                                 \
  (Snprintf(nb = eos(nb), MAXCO - strlen(newbot2),                      \
            first ? "%s" : " %s",                                       \
            abbrev == 2 ? (str3) : abbrev == 1 ? (str2) : (str1)),      \
   first = FALSE)
#define status_effect_duration(str1, str2, str3, duration)		\
  (Snprintf(nb = eos(nb), MAXCO - strlen(newbot2),                      \
            first ? "%s:%ld" : " %s:%ld",				\
            abbrev == 2 ? (str3) : abbrev == 1 ? (str2) : (str1),	\
	    duration),							\
   first = FALSE)
#endif
/** Delayed instadeaths **/
  if(Stoned || Golded)
    status_effect("Stone", "Ston", "Sto");
  if(Slimed)
    status_effect("Slime", "Slim", "Slm");
  if(FrozenAir || Strangled || BloodDrown)
    status_effect("Sufct", "Sfct", "Sfc");
  if(Sick) {
    if (u.usick_type & SICK_VOMITABLE)
      status_effect("FoodPois", "Fpois", "Poi");
    if (u.usick_type & SICK_NONVOMITABLE)
      status_effect("Ill", "Ill", "Ill");
  }
/** Hunger **/
  if(u.uhs != NOT_HUNGRY) {
    if(uclockwork)
      status_effect(ca_hu_stat[u.uhs], ca_hu_stat[u.uhs], ca_hu_stat[u.uhs]);
    else
      status_effect(hu_stat[u.uhs], hu_stat[u.uhs], hu_stat[u.uhs]);
  }
/** Encumbrance **/
  if(cap > UNENCUMBERED)
    status_effect(enc_stat[cap], enc_stat_abbrev1[cap], enc_stat_abbrev2[cap]);
/** Other status effects **/
  if(Invulnerable)
    status_effect("Invl", "Invl", "In");
  if(Blind && !StumbleBlind)
    status_effect("Blind", "Blnd", "Bl");
  if(Stunned && !StaggerShock)
    status_effect("Stun", "Stun", "St");
  if(Confusion && !StumbleBlind)
    status_effect("Conf", "Cnf", "Cf");
  if(Hallucination)
    status_effect("Hallu", "Hal", "Hl");
/** Insanity messages **/
  if(Panicking)
    status_effect("Panic", "Pnc", "Pnc");
  if(StumbleBlind)
    status_effect("Stmblng", "Stmbl", "Stm");
  if(StaggerShock)
    status_effect("Stggrng", "Stggr", "Stg");
  if(Babble)
    status_effect("Babble", "Babl", "Bbl");
  if(Screaming)
    status_effect("Scream", "Scrm", "Scr");
  if(FaintingFits)
    status_effect("Faint", "Fnt", "Fnt");
/** Less important **/
  if(u.ustuck) {
    if(sticks(&youmonst) && !u.uswallow)
      status_effect("UHold", "UHld", "UHd");
    else
      status_effect("Held", "Hld", "Hd");
  }
  if(Levitation)
    status_effect("Lev", "Lev", "Lv");
  /* flying and levitation are mutually exclusive */
  if(Flying && !Levitation)
    status_effect("Fly", "Fly", "Fl");
  if(u.usteed)
    status_effect("Ride", "Rid", "Rd");
/** Temporary effects with known duration **/
  if (TimeStop)
    status_effect_duration("TimeStop", "TStop", "TS", HTimeStop ? HTimeStop : ETimeStop);
  if (BlowingWinds)
    status_effect_duration("Lust", "Lust", "Lst", HBlowingWinds ? HBlowingWinds : EBlowingWinds);
#undef status_effect
#undef status_effect_duration
}

void
bot2str(char *newbot2, boolean terminal_output, int abbrev, boolean dumplog)
{
	register char *nb;
	int hp, hpmax;
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
        int save_botlx = flags.botlx;
#endif

	hp = Upolyd ? u.mh : u.uhp;
	hpmax = Upolyd ? u.mhmax : u.uhpmax;

        if(hp < 0) hp = 0;
        (void) describe_level(newbot2);
        Sprintf(nb = eos(newbot2), "%c:%-2ld", oc_syms[COIN_CLASS],
#ifndef GOLDOBJ
                u.ugold
#else
                money_cnt(invent)
#endif
		);

#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
        Strcat(nb = eos(newbot2), " HP:");
        curs(WIN_STATUS, 1, 1);
        putstr(WIN_STATUS, 0, newbot2);
        flags.botlx = 0;

        Sprintf(nb = eos(nb), "%d(%d)", hp, hpmax);
        apply_color_option(percentage_color_of(hp, hpmax, hp_colors), newbot2, 2);
#else
        Sprintf(nb = eos(nb), " HP:%d(%d)", hp, hpmax);
#endif
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
        Strcat(nb = eos(nb), " Pw:");
        curs(WIN_STATUS, 1, 1);
        putstr(WIN_STATUS, 0, newbot2);

        Sprintf(nb = eos(nb), "%d(%d)", u.uen, u.uenmax);
        apply_color_option(percentage_color_of(u.uen, u.uenmax, pw_colors), newbot2, 2);
#else
        Sprintf(nb = eos(nb), " Pw:%d(%d)", u.uen, u.uenmax);
#endif
        Sprintf(nb = eos(nb), " Br:%d", u.divetimer);
        Sprintf(nb = eos(nb), " AC:%-2d", (u.uac + u.ustdy));
        Sprintf(nb = eos(nb), " DR:%-2d", u.udr - u.ustdy);
	if (Upolyd)
		Sprintf(nb = eos(nb), " HD:%d", mons[u.umonnum].mlevel);
#ifdef EXP_ON_BOTL
	else if(flags.showexp)
		Sprintf(nb = eos(nb), " Xp:%u/%-1ld", u.ulevel,u.uexp);
#endif
	else
		Sprintf(nb = eos(nb), " Exp:%u", u.ulevel);

	if(flags.time)
	    Sprintf(nb = eos(nb), " T:%ld", iflags.mod_turncount ? moves%10 : moves);
#ifdef REALTIME_ON_BOTL
  if(iflags.showrealtime) {
    time_t currenttime = get_realtime();
    Sprintf(nb = eos(nb), " %ld:%2.2ld", currenttime / 3600,
                                         (currenttime % 3600) / 60);
  }
#endif
  if (!dumplog && (LI <= ROWNO+3 || iflags.statuslines <= 2))
    do_statuseffects(newbot2, terminal_output, abbrev, 2);
}

STATIC_OVL void
bot2()
{
	char newbot2[MAXCO];
	int abbrev = 0;
	for (;;) {
		bot2str(newbot2, FALSE, abbrev, FALSE);
		if (abbrev >= 2 || strlen(newbot2) < min(MAXCO-1, CO))
			break;
		abbrev++;
	}
	bot2str(newbot2, TRUE, abbrev, FALSE);
	int save_botlx = flags.botlx;
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);
	flags.botlx = save_botlx;
}

void
bot3str(char *newbot3, boolean terminal_output, int abbrev)
{
	newbot3[0] = '\0';      /* so eos() works */
	do_statuseffects(newbot3, terminal_output, abbrev, 3);
}

void
bot3()
{
	char newbot3[MAXCO];
	int abbrev = 0;
	if (LI <= ROWNO+3 || iflags.statuslines <= 2)
		return;
	for (;;) {
		bot3str(newbot3, FALSE, abbrev);
		if (abbrev >= 2 || strlen(newbot3) < min(MAXCO-1, CO))
			break;
		abbrev++;
	}
	bot3str(newbot3, TRUE, abbrev);
	int save_botlx = flags.botlx;
	curs(WIN_STATUS, 1, 2);
	putstr(WIN_STATUS, 0, newbot3);
	flags.botlx = save_botlx;
}

void
bot()
{
	if (!iflags.botl_updates) {
	  flags.botl = flags.botlx = 0;
	  return;
	}
	bot1();
	bot2();
	bot3();
	flags.botl = flags.botlx = 0;
}

int
force_bot()
{
	bot1();
	bot2();
	bot3();
	flags.botl = flags.botlx = 0;
	return MOVE_CANCELLED;
}

#endif /* OVL0 */

/*botl.c*/
