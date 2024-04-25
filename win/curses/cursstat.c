/* vim:set cin ft=c sw=4 sts=4 ts=8 et ai cino=Ls\:0t0(0 : -*- mode:c;fill-column:80;tab-width:8;c-basic-offset:4;indent-tabs-mode:nil;c-file-style:"k&r" -*-*/

#include "curses.h"
#include "hack.h"
#include "wincurs.h"
#include "cursstat.h"
#include "botl.h"

/* Status window functions for curses interface */

/* Private declarations */

/* Used to track previous value of things, to highlight changes. */
typedef struct nhs {
    long value;
    int highlight_turns;
    int highlight_color;
} nhstat;

static attr_t get_trouble_color(const char *);
static void draw_trouble_str(const char *, const char *);
static void print_statdiff(const char *append, nhstat *, int, int);
static void get_playerrank(char *);
static int hpen_color(boolean, int, int);
static void draw_bar(boolean, int, int, const char *);
static void draw_horizontal(int, int, int, int);
static void draw_horizontal_new(int, int, int, int);
static void draw_vertical(int, int, int, int);
static void curses_add_statuses(WINDOW *, boolean, boolean, int *, int *, boolean);
static void curses_add_status(WINDOW *, boolean, boolean, int *, int *,
                              const char *, const char *, long, boolean);
static int decrement_highlight(nhstat *, boolean);

#ifdef STATUS_COLORS
static attr_t hpen_color_attr(boolean, int, int);
extern struct color_option text_color_of(const char *text,
                                         const struct text_color_option *color_options);
struct color_option percentage_color_of(int value, int max,
                                        const struct percent_color_option *color_options);

extern const struct text_color_option *text_colors;
extern const struct percent_color_option *hp_colors;
extern const struct percent_color_option *pw_colors;
#endif

/* Whether or not we have printed status window content at least once.
   Used to ensure that prev* doesn't end up highlighted on game start. */
static boolean first = TRUE;
static nhstat prevdepth;
static nhstat prevstr;
static nhstat prevint;
static nhstat prevwis;
static nhstat prevdex;
static nhstat prevcon;
static nhstat prevcha;
static nhstat prevau;
static nhstat prevlevel;
static nhstat prevdive;
static nhstat prevac;
static nhstat prevdr;
static nhstat prevexp;
static nhstat prevtime;

#ifdef SCORE_ON_BOTL
static nhstat prevscore;
#endif

extern const char *hu_stat[];   /* from eat.c */
extern const char *ca_hu_stat[];   /* from eat.c */
extern const char *enc_stat[];  /* from botl.c */
extern const char *enc_stat_abbrev1[]; /* from botl.c */
extern const char *enc_stat_abbrev2[]; /* from botl.c */

/* If the statuscolors patch isn't enabled, have some default colors for status problems
   anyway */

struct statcolor {
    const char *txt; /* For status problems */
    int color; /* Default color assuming STATUS_COLORS isn't enabled */
};

static const struct statcolor default_colors[] = {
    /* Hunger */
    {"Satiated", CLR_YELLOW},
    {"Hungry", CLR_YELLOW},
    {"Weak", CLR_ORANGE},
    {"Fainted", CLR_BRIGHT_MAGENTA},
    {"Fainting", CLR_BRIGHT_MAGENTA},
    {"OvrWound", CLR_YELLOW},
    {"Waning", CLR_YELLOW},
    {"Unwound", CLR_ORANGE},
    {"Slipping", CLR_BRIGHT_MAGENTA},
    {"Slipped", CLR_BRIGHT_MAGENTA},
    /* Encumbrance */
    {"Burdened", CLR_RED},
    {"Stressed", CLR_RED},
    {"Strained", CLR_ORANGE},
    {"Overtaxed", CLR_ORANGE},
    {"Overloaded", CLR_BRIGHT_MAGENTA},
    /* Delayed instadeaths */
    {"Stone", CLR_BRIGHT_MAGENTA},
    {"Slime", CLR_BRIGHT_MAGENTA},
    {"Sufct", CLR_BRIGHT_MAGENTA},
    {"Ill", CLR_BRIGHT_MAGENTA},
    {"FoodPois", CLR_BRIGHT_MAGENTA},
    /* Other status effects */
    {"Blind", CLR_BRIGHT_BLUE},
    {"Stun", CLR_BRIGHT_BLUE},
    {"Conf", CLR_BRIGHT_BLUE},
    {"Hallu", CLR_BRIGHT_BLUE},
    /* Insanity messages */
    {"Panic", CLR_BRIGHT_CYAN},
    {"Stmblng", CLR_BRIGHT_CYAN},
    {"Stggrng", CLR_BRIGHT_CYAN},
    {"Babble", CLR_BRIGHT_CYAN},
    {"Scream", CLR_BRIGHT_CYAN},
    {"Faint", CLR_BRIGHT_CYAN},
    /* Less important */
    {"Held", CLR_GREEN},
    {"UHold", CLR_GREEN},
    {"Lycn", CLR_GREEN},
    {"Invl", CLR_GREEN},
    {"Lev", CLR_GREEN},
    {"Fly", CLR_GREEN},
    {"Ride", CLR_GREEN},
    /* Temporary effects with known duration */
    {"TimeStop", CLR_BRIGHT_GREEN},
    {"Lust", CLR_BRIGHT_GREEN},
    {"DeadMagc", CLR_BRIGHT_GREEN},
    {"Miso", CLR_BRIGHT_GREEN},
    {"Catapsi", CLR_BRIGHT_GREEN},
    {"DimLock", CLR_BRIGHT_GREEN},
    {NULL, NO_COLOR},
};

static attr_t
get_trouble_color(const char *stat)
{
    attr_t res = curses_color_attr(CLR_GRAY, 0);
    const struct statcolor *clr;
    for (clr = default_colors; clr->txt; clr++) {
        if (stat && !strcmp(clr->txt, stat)) {
#ifdef STATUS_COLORS
            /* Check if we have a color enabled with statuscolors */
            if (!iflags.use_status_colors)
                return curses_color_attr(CLR_GRAY, 0); /* no color configured */

            struct color_option stat_color;

            stat_color = text_color_of(clr->txt, text_colors);
            if (stat_color.color == NO_COLOR && !stat_color.attr_bits)
                return curses_color_attr(CLR_GRAY, 0);

            if (stat_color.color != NO_COLOR)
                res = curses_color_attr(stat_color.color, 0);

            res = curses_color_attr(stat_color.color, 0);
            int count;
            for (count = 0; (1 << count) <= stat_color.attr_bits; count++) {
                if (count != ATR_NONE &&
                    (stat_color.attr_bits & (1 << count)))
                    res |= curses_convert_attr(count);
            }

            return res;
#else
            return curses_color_attr(clr->color, 0);
#endif
        }
    }

    return res;
}

/* TODO: This is in the wrong place. */
void
get_playerrank(char *rank)
{
    char buf[BUFSZ];
    if (Upolyd) {
        int k = 0;

        Strncpy(buf, mons[u.umonnum].mname, BUFSZ);
        while(buf[k] != 0) {
            if ((k == 0 || (k > 0 && buf[k-1] == ' ')) &&
                'a' <= buf[k] && buf[k] <= 'z')
                buf[k] += 'A' - 'a';
            k++;
        }
        Strncpy(rank, buf, BUFSZ);
    } else
        Strncpy(rank, rank_of(u.ulevel, Role_switch, flags.female), BUFSZ);
}

/* Handles numerical stat changes of various kinds.
   type is generally STAT_OTHER (generic "do nothing special"),
   but is used if the stat needs to be handled in a special way. */
static void
print_statdiff(const char *append, nhstat *stat, int new, int type)
{
    char buf[BUFSZ];
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

    int color = CLR_GRAY;

    /* Turncount isn't highlighted, or it would be highlighted constantly. */
    if (type != STAT_TIME && new != stat->value) {
        /* Less AC is better */
        if ((type == STAT_AC && new < stat->value) ||
            (type != STAT_AC && new > stat->value)) {
            color = STAT_UP_COLOR;
            if (type == STAT_GOLD)
                color = HI_GOLD;
        } else
            color = STAT_DOWN_COLOR;

        stat->value = new;
        stat->highlight_color = color;
        stat->highlight_turns = 5;
    } else if (stat->highlight_turns)
        color = stat->highlight_color;

    attr_t attr = curses_color_attr(color, 0);
    wattron(win, attr);
    wprintw(win, "%s", append);
    if (type == STAT_STR && new > 18) {
        if (new > STR18(100))
            wprintw(win, "%d", new - 20);
        else if (new == STR18(100))
            wprintw(win, "18/**");
        else
            wprintw(win, "18/%02d", (new - 18)*5);
    } else
        wprintw(win, "%d", new);

    wattroff(win, attr);
}

static void
draw_trouble_str(const char *hilite, const char *str)
{
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

    attr_t attr = get_trouble_color(hilite);
    wattron(win, attr);
    wprintw(win, "%s", str);
    wattroff(win, attr);
}

/* Returns a ncurses attribute for foreground and background.
   This should probably be in cursinit.c or something. */
attr_t
curses_color_attr(int nh_color, int bg_color)
{
    int color = nh_color + 1;
	int cnum = COLORS >= 16 ? 16 : 8;
    attr_t cattr = A_NORMAL;

    if (!nh_color) {
#ifdef USE_DARKGRAY
        if (iflags.wc2_darkgray) {
            if (!can_change_color() || COLORS <= 16)
                cattr |= A_BOLD;
        } else
#endif
            color = COLOR_BLUE;
    }

    if (COLORS < 16 && color > 8) {
        color -= 8;
        cattr = A_BOLD;
    }

    /* Can we do background colors? We can if we have more than
       16*7 colors (more than 8*7 for terminals with bold) */
    if (COLOR_PAIRS > cnum * 7) {
		if (bg_color != NO_COLOR && bg_color != CLR_BLACK)
			color += cnum * (1+bg_color);
    }
    cattr |= COLOR_PAIR(color);

    return cattr;
}

/* Returns a complete curses attribute. Used to possibly bold/underline/etc HP/Pw. */
#ifdef STATUS_COLORS
static attr_t
hpen_color_attr(boolean is_hp, int cur, int max)
{
    struct color_option stat_color;
    int count;
    attr_t attr = 0;
    if (!iflags.use_status_colors)
        return curses_color_attr(CLR_GRAY, 0);

    stat_color = percentage_color_of(cur, max, is_hp ? hp_colors : pw_colors);

    if (stat_color.color != NO_COLOR)
        attr |= curses_color_attr(stat_color.color, 0);

    for (count = 0; (1 << count) <= stat_color.attr_bits; count++) {
        if (count != ATR_NONE && (stat_color.attr_bits & (1 << count)))
            attr |= curses_convert_attr(count);
    }

    return attr;
}
#endif

/* Return color for the HP bar.
   With status colors ON, this respect its configuration (defaulting to gray), but
   only obeys the color (no weird attributes for the HP bar).
   With status colors OFF, this returns reasonable defaults which are also used
   for the HP/Pw text itself. */
static int
hpen_color(boolean is_hp, int cur, int max)
{
#ifdef STATUS_COLORS
    if (iflags.use_status_colors) {
        struct color_option stat_color;
        stat_color = percentage_color_of(cur, max, is_hp ? hp_colors : pw_colors);

        if (stat_color.color == NO_COLOR)
            return CLR_GRAY;
        else
            return stat_color.color;
    } else
        return CLR_GRAY;
#endif

    int color = CLR_GRAY;
    if (cur == max)
        color = CLR_GRAY;
    else if (cur * 3 > max * 2) /* >2/3 */
        color = is_hp ? CLR_GREEN : CLR_CYAN;
    else if (cur * 3 > max) /* >1/3 */
        color = is_hp ? CLR_YELLOW : CLR_BLUE;
    else if (cur * 7 > max) /* >1/7 */
        color = is_hp ? CLR_RED : CLR_MAGENTA;
    else
        color = is_hp ? CLR_ORANGE : CLR_BRIGHT_MAGENTA;

    return color;
}

/* Draws a bar
   is_hp: TRUE if we're drawing HP, Pw otherwise (determines colors)
   cur/max: Current/max HP/Pw
   title: Not NULL if we are drawing as part of an existing title.
   Otherwise, the format is as follows: [   11 / 11   ] */
static void
draw_bar(boolean is_hp, int cur, int max, const char *title)
{
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

    char buf[BUFSZ];
    if (title)
        Strncpy(buf, title, BUFSZ);
    else {
        int len = 5;
        snprintf(buf, BUFSZ, "%*d / %-*d", len, cur, len, max);
#ifdef STATUS_COLORS
        if (!iflags.hitpointbar) {
            attr_t attr = hpen_color_attr(is_hp, cur, max);
            wattron(win, attr);
            wprintw(win, "%s", buf);
            wattroff(win, attr);
            return;
        }
#endif
    }

#ifdef STATUS_COLORS
    if (!iflags.hitpointbar) {
        wprintw(win, "%s", buf);
        return;
    }
#endif

    /* Colors */
    attr_t fillattr, attr;
    int color = hpen_color(is_hp, cur, max);
    int invcolor = color & 7;

    fillattr = curses_color_attr(color, invcolor);
    attr = curses_color_attr(color, 0);

    /* Figure out how much of the bar to fill */
    int fill = 0;
    int len = strlen(buf);
    if (cur > 0 && max > 0)
        fill = len * cur / max;
    if (fill > len)
        fill = len;

    waddch(win, '[');
    wattron(win, fillattr);
    wprintw(win, "%.*s", fill, buf);
    wattroff(win, fillattr);
    wattron(win, attr);
    wprintw(win, "%.*s", len - fill, &buf[fill]);
    wattroff(win, attr);
    waddch(win, ']');
}

/* Update the status win - this is called when NetHack would normally
   write to the status window, so we know somwthing has changed.  We
   override the write and update what needs to be updated ourselves. */
void
curses_update_stats(void)
{
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

    /* Clear the window */
    werase(win);

    int orient = curses_get_window_orientation(STATUS_WIN);

    boolean horiz = FALSE;
    if ((orient != ALIGN_RIGHT) && (orient != ALIGN_LEFT))
        horiz = TRUE;

    boolean border = curses_window_has_border(STATUS_WIN);

    /* Figure out if we have proper window dimensions for horizontal statusbar. */
    if (horiz) {
        /* correct y */
        int cy = 3;
        if (!iflags.classic_status && iflags.statuslines >= 4)
            cy = 4;
        if (iflags.classic_status && iflags.statuslines <= 2)
            cy = 2;

        /* actual y (and x) */
        int ax = 0;
        int ay = 0;
        getmaxyx(win, ay, ax);
        if (border)
            ay -= 2;

        if (cy != ay) {
            curses_create_main_windows();
            curses_last_messages();
            doredraw();

            /* Reset XP highlight (since classic_status and new show different numbers) */
            prevexp.highlight_turns = 0;
            curses_update_stats();
            return;
        }
    }

    /* Starting x/y. Passed to draw_horizontal/draw_vertical to keep track of
       window positioning. */
    int x = 0;
    int y = 0;

    /* Don't start at border position if applicable */
    if (border) {
        x++;
        y++;
    }

    /* Get HP values. */
    int hp = u.uhp;
    int hpmax = u.uhpmax;
    if (Upolyd) {
        hp = u.mh;
        hpmax = u.mhmax;
    }

    if (orient != ALIGN_RIGHT && orient != ALIGN_LEFT)
        draw_horizontal(x, y, hp, hpmax);
    else
        draw_vertical(x, y, hp, hpmax);

    if (border)
        box(win, 0, 0);

    wnoutrefresh(win);

    if (first) {
        first = FALSE;

        /* Zero highlight timers. This will call curses_update_status again if needed */
        curses_decrement_highlights(TRUE);
    }
}

static void
draw_horizontal(int x, int y, int hp, int hpmax)
{
    if (!iflags.classic_status) {
        /* Draw new-style statusbar */
        draw_horizontal_new(x, y, hp, hpmax);
        return;
    }
    char buf[BUFSZ];
    char rank[BUFSZ];
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

    /* Line 1 */
    wmove(win, y, x);

    get_playerrank(rank);
#pragma GCC diagnostic ignored "-Wformat-truncation"
    snprintf(buf, BUFSZ, "%s the %s", plname, rank);
#pragma GCC diagnostic pop

    /* Use the title as HP bar (similar to hitpointbar) */
    draw_bar(TRUE, hp, hpmax, buf);

    /* Attributes */
    print_statdiff(" St:", &prevstr, ACURR(A_STR), STAT_STR);
    print_statdiff(" Dx:", &prevdex, ACURR(A_DEX), STAT_OTHER);
    print_statdiff(" Co:", &prevcon, ACURR(A_CON), STAT_OTHER);
    print_statdiff(" In:", &prevint, ACURR(A_INT), STAT_OTHER);
    print_statdiff(" Wi:", &prevwis, ACURR(A_WIS), STAT_OTHER);
    print_statdiff(" Ch:", &prevcha, ACURR(A_CHA), STAT_OTHER);

    wprintw(win, (u.ualign.type == A_CHAOTIC ? " Chaotic" :
                  u.ualign.type == A_NEUTRAL ? " Neutral" :
                  u.ualign.type == A_VOID    ? " Gnostic" :
                  u.ualign.type == A_LAWFUL  ? " Lawful"  :
                  u.ualign.type == A_NONE    ? " Unaligned" : " Other"));

#ifdef SCORE_ON_BOTL
    if (flags.showscore)
        print_statdiff(" S:", &prevscore, botl_score(), STAT_OTHER);
#endif /* SCORE_ON_BOTL */


    /* Line 2 */
    y++;
    wmove(win, y, x);

    describe_level(buf);

    wprintw(win, "%s", buf);

#ifndef GOLDOBJ
    print_statdiff("$", &prevau, u.ugold, STAT_GOLD);
#else
    print_statdiff("$", &prevau, money_cnt(invent), STAT_GOLD);
#endif

    /* HP/Pw use special coloring rules */
    attr_t hpattr, pwattr;
#ifdef STATUS_COLORS
    hpattr = hpen_color_attr(TRUE, hp, hpmax);
    pwattr = hpen_color_attr(FALSE, u.uen, u.uenmax);
#else
    int hpcolor, pwcolor;
    hpcolor = hpen_color(TRUE, hp, hpmax);
    pwcolor = hpen_color(FALSE, u.uen, u.uenmax);
    hpattr = curses_color_attr(hpcolor, 0);
    pwattr = curses_color_attr(pwcolor, 0);
#endif
    wprintw(win, " HP:");
    wattron(win, hpattr);
    wprintw(win, "%d(%d)", hp, hpmax);
    wattroff(win, hpattr);

    wprintw(win, " Pw:");
    wattron(win, pwattr);
    wprintw(win, "%d(%d)", u.uen, u.uenmax);
    wattroff(win, pwattr);

    print_statdiff(" Br:", &prevdive, u.divetimer, STAT_OTHER);
	print_statdiff(" AC:", &prevac, (u.uac + u.ustdy), STAT_AC);
	print_statdiff(" DR:", &prevdr, u.udr, STAT_OTHER);

    if (Upolyd)
        print_statdiff(" HD:", &prevlevel, mons[u.umonnum].mlevel, STAT_OTHER);
#ifdef EXP_ON_BOTL
    else if (flags.showexp) {
        print_statdiff(" Xp:", &prevlevel, u.ulevel, STAT_OTHER);
        /* use waddch, we don't want to highlight the '/' */
        waddch(win, '/');
        print_statdiff("", &prevexp, u.uexp, STAT_OTHER);
    }
#endif
    else
        print_statdiff(" Exp:", &prevlevel, u.ulevel, STAT_OTHER);

    if (flags.time)
        print_statdiff(" T:", &prevtime, moves, STAT_TIME);
    if (wizard && flags.movetoprint) {
        int i, tmp;
        *buf='\0';
        for (i=0, tmp = flags.movetoprint; tmp>=1<<i; i++)
            switch(tmp & (1<<i)) {
                case MOVE_STANDARD:  Strcat(buf, "standard/");   break;
                case MOVE_INSTANT:   Strcat(buf, "instant/");    break;
                case MOVE_PARTIAL:   Strcat(buf, "bonus/");      break;
                case MOVE_ATTACKED:  Strcat(buf, "attacked/");   break;
                case MOVE_MOVED:     Strcat(buf, "moved/");      break;
                case MOVE_QUAFFED:   Strcat(buf, "quaffed/");    break;
                case MOVE_ZAPPED:    Strcat(buf, "zapped/");     break;
                case MOVE_READ:      Strcat(buf, "read/");       break;
                case MOVE_CASTSPELL: Strcat(buf, "cast/");       break;
                case MOVE_ATE:       Strcat(buf, "ate/");        break;
                case MOVE_FIRED:     Strcat(buf, "fired/");      break;
            }
        if (strlen(buf) > 0) Strcpy(eos(buf)-1, "");
        wprintw(win, " [%d:%s]", flags.movetoprintcost, buf);
    }
    if (iflags.statuslines >= 3) {
        y++;
        wmove(win, y, x);
    }
    curses_add_statuses(win, FALSE, FALSE, NULL, NULL, iflags.statuslines >= 3);
}

static void
draw_horizontal_new(int x, int y, int hp, int hpmax)
{
    char buf[BUFSZ];
    char rank[BUFSZ];
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

    /* Line 1 */
    wmove(win, y, x);

    get_playerrank(rank);
    char race[BUFSZ];
    Strncpy(race, urace.adj, BUFSZ);
    race[0] = highc(race[0]);
    wprintw(win, "%s the %s %s%s%s", plname,
            (u.ualign.type == A_CHAOTIC ? "Chaotic" :
             u.ualign.type == A_NEUTRAL ? "Neutral" :
             u.ualign.type == A_VOID    ? "Gnostic" :
             u.ualign.type == A_LAWFUL  ? "Lawful"  :
             u.ualign.type == A_NONE    ? "Unaligned" : "Other"),
            Upolyd ? "" : race, Upolyd ? "" : " ",
            rank);

    /* Line 2 */
    y++;
    wmove(win, y, x);
    wprintw(win, "HP:");
    draw_bar(TRUE, hp, hpmax, NULL);
	print_statdiff(" AC:", &prevac, (u.uac + u.ustdy), STAT_AC);
	print_statdiff(" DR:", &prevdr, u.udr, STAT_OTHER);
    if (Upolyd)
        print_statdiff(" HD:", &prevlevel, mons[u.umonnum].mlevel, STAT_OTHER);
#ifdef EXP_ON_BOTL
    else if (flags.showexp) {
        /* Ensure that Xp have proper highlight on level change. */
        int levelchange = 0;
        if (prevlevel.value != u.ulevel) {
            if (prevlevel.value < u.ulevel)
                levelchange = 1;
            else
                levelchange = 2;
        }
        print_statdiff(" Xp:", &prevlevel, u.ulevel, STAT_OTHER);
        /* use waddch, we don't want to highlight the '/' */
        waddch(win, '(');

        /* Figure out amount of Xp needed to next level */
        int xp_left = 0;
        if (u.ulevel < 30)
            xp_left = (newuexp(u.ulevel) - u.uexp);

        if (levelchange) {
            prevexp.value = (xp_left + 1);
            if (levelchange == 2)
                prevexp.value = (xp_left - 1);
        }
        print_statdiff("", &prevexp, xp_left, STAT_AC);
        waddch(win, ')');
    }
#endif
    else
        print_statdiff(" Exp:", &prevlevel, u.ulevel, STAT_OTHER);

    waddch(win, ' ');
    describe_level(buf);

    wprintw(win, "%s", buf);

    /* Line 3 */
    y++;
    wmove(win, y, x);
    wprintw(win, "Pw:");
    draw_bar(FALSE, u.uen, u.uenmax, NULL);

    print_statdiff(" Br:", &prevdive, u.divetimer, STAT_OTHER);

#ifndef GOLDOBJ
    print_statdiff(" $", &prevau, u.ugold, STAT_GOLD);
#else
    print_statdiff(" $", &prevau, money_cnt(invent), STAT_GOLD);
#endif

#ifdef SCORE_ON_BOTL
    if (flags.showscore)
        print_statdiff(" S:", &prevscore, botl_score(), STAT_OTHER);
#endif /* SCORE_ON_BOTL */

    if (flags.time)
        print_statdiff(" T:", &prevtime, moves, STAT_TIME);
    if (wizard && flags.movetoprint) {
        int i, tmp;
        *buf='\0';
        for (i=0, tmp = flags.movetoprint; tmp>=1<<i; i++)
            switch(tmp & (1<<i)) {
                case MOVE_STANDARD:  Strcat(buf, "standard/");   break;
                case MOVE_INSTANT:   Strcat(buf, "instant/");    break;
                case MOVE_PARTIAL:   Strcat(buf, "bonus/");      break;
                case MOVE_ATTACKED:  Strcat(buf, "attacked/");   break;
                case MOVE_MOVED:     Strcat(buf, "moved/");      break;
                case MOVE_QUAFFED:   Strcat(buf, "quaffed/");    break;
                case MOVE_ZAPPED:    Strcat(buf, "zapped/");     break;
                case MOVE_READ:      Strcat(buf, "read/");       break;
                case MOVE_CASTSPELL: Strcat(buf, "cast/");       break;
                case MOVE_ATE:       Strcat(buf, "ate/");        break;
                case MOVE_FIRED:     Strcat(buf, "fired/");      break;
            }
        if (strlen(buf) > 0) Strcpy(eos(buf)-1, "");
        wprintw(win, " [%d:%s]", flags.movetoprintcost, buf);
    }
    if (iflags.statuslines >= 4) {
        y++;
        wmove(win, y, x);
    }
    curses_add_statuses(win, TRUE, FALSE, &x, &y, iflags.statuslines >= 4);

    /* Right-aligned attributes */
    int stat_length = 6; /* " Dx:xx" */
    int str_length = 6;
    if (ACURR(A_STR) > 18 && ACURR(A_STR) < 119)
        str_length = 9;

    getmaxyx(win, y, x);

    /* We want to deal with top line of y. getmaxx would do what we want, but it only
       exist for compatibility reasons and might not exist at all in some versions. */
    y = 0;
    if (curses_window_has_border(STATUS_WIN)) {
        x--;
        y++;
    }

    x -= stat_length;
    int orig_x = x;
    wmove(win, y, x);
    print_statdiff(" Co:", &prevcon, ACURR(A_CON), STAT_OTHER);
    x -= stat_length;
    wmove(win, y, x);
    print_statdiff(" Dx:", &prevdex, ACURR(A_DEX), STAT_OTHER);
    x -= str_length;
    wmove(win, y, x);
    print_statdiff(" St:", &prevstr, ACURR(A_STR), STAT_STR);

    x = orig_x;
    y++;
    wmove(win, y, x);
    print_statdiff(" Ch:", &prevcha, ACURR(A_CHA), STAT_OTHER);
    x -= stat_length;
    wmove(win, y, x);
    print_statdiff(" Wi:", &prevwis, ACURR(A_WIS), STAT_OTHER);
    x -= str_length;
    wmove(win, y, x);
    print_statdiff(" In:", &prevint, ACURR(A_INT), STAT_OTHER);
}

/* Personally I never understood the point of a vertical status bar. But removing the
   option would be silly, so keep the functionality. */
static void
draw_vertical(int x, int y, int hp, int hpmax)
{
    char buf[BUFSZ];
    char rank[BUFSZ];
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

    /* Print title and dungeon branch */
    wmove(win, y++, x);

    get_playerrank(rank);
    int ranklen = strlen(rank);
    int namelen = strlen(plname);
    int maxlen = 19;
#ifdef STATUS_COLORS
    if (!iflags.hitpointbar)
        maxlen += 2; /* With no hitpointbar, we can fit more since there's no "[]" */
#endif

    if ((ranklen + namelen) > maxlen) {
        /* The result doesn't fit. Strip name if >10 characters, then strip title */
        if (namelen > 10) {
            while (namelen > 10 && (ranklen + namelen) > maxlen)
                namelen--;
        }

        while ((ranklen + namelen) > maxlen)
            ranklen--; /* Still doesn't fit, strip rank */
    }
#pragma GCC diagnostic ignored "-Wformat-truncation"
    snprintf(buf, BUFSZ, "%-*s the %-*s", namelen, plname, ranklen, rank);
#pragma GCC diagnostic pop
    draw_bar(TRUE, hp, hpmax, buf);
    wmove(win, y++, x);
    wprintw(win, "%s", dungeons[u.uz.dnum].dname);

    y++; /* Blank line inbetween */
    wmove(win, y++, x);

    /* Attributes. Old  vertical order is preserved */
    print_statdiff("Strength:      ", &prevstr, ACURR(A_STR), STAT_STR);
    wmove(win, y++, x);
    print_statdiff("Intelligence:  ", &prevint, ACURR(A_INT), STAT_OTHER);
    wmove(win, y++, x);
    print_statdiff("Wisdom:        ", &prevwis, ACURR(A_WIS), STAT_OTHER);
    wmove(win, y++, x);
    print_statdiff("Dexterity:     ", &prevdex, ACURR(A_DEX), STAT_OTHER);
    wmove(win, y++, x);
    print_statdiff("Constitution:  ", &prevcon, ACURR(A_CON), STAT_OTHER);
    wmove(win, y++, x);
    print_statdiff("Charisma:      ", &prevcha, ACURR(A_CHA), STAT_OTHER);
    wmove(win, y++, x);
    wprintw(win,   "Alignment:     ");
    wprintw(win, (u.ualign.type == A_CHAOTIC ? "Chaotic" :
                  u.ualign.type == A_NEUTRAL ? "Neutral" :
                  u.ualign.type == A_VOID    ? "Gnostic" :
                  u.ualign.type == A_LAWFUL  ? "Lawful"  :
                  u.ualign.type == A_NONE    ? "Unaligned" : "Other"));
    wmove(win, y++, x);
    wprintw(win,   "Dungeon Level: ");

    /* Astral Plane doesn't fit */
    if (In_endgame(&u.uz))
        wprintw(win, "%s", Is_astralevel(&u.uz) ? "Astral" : "End Game");
    else
        wprintw(win, "%d", depth(&u.uz));
    wmove(win, y++, x);

#ifndef GOLDOBJ
    print_statdiff("Gold:          ", &prevau, u.ugold, STAT_GOLD);
#else
    print_statdiff("Gold:          ", &prevau, money_cnt(invent), STAT_GOLD);
#endif
    wmove(win, y++, x);

    /* HP/Pw use special coloring rules */
    attr_t hpattr, pwattr;
#ifdef STATUS_COLORS
    hpattr = hpen_color_attr(TRUE, hp, hpmax);
    pwattr = hpen_color_attr(FALSE, u.uen, u.uenmax);
#else
    int hpcolor, pwcolor;
    hpcolor = hpen_color(TRUE, hp, hpmax);
    pwcolor = hpen_color(FALSE, u.uen, u.uenmax);
    hpattr = curses_color_attr(hpcolor, 0);
    pwattr = curses_color_attr(pwcolor, 0);
#endif

    wprintw(win,   "Hit Points:    ");
    wattron(win, hpattr);
    wprintw(win, "%d/%d", hp, hpmax);
    wattroff(win, hpattr);
    wmove(win, y++, x);

    wprintw(win,   "Magic Power:   ");
    wattron(win, pwattr);
    wprintw(win, "%d/%d", u.uen, u.uenmax);
    wattroff(win, pwattr);
    wmove(win, y++, x);

    print_statdiff("Breath:        ", &prevdive, u.divetimer, STAT_OTHER);
    wmove(win, y++, x);
    print_statdiff("Armor Class:   ", &prevac, (u.uac + u.ustdy), STAT_AC);
    wmove(win, y++, x);
	print_statdiff("Damage Resist: ", &prevdr, u.udr, STAT_OTHER);
	wmove(win, y++, x);

    if (Upolyd)
        print_statdiff("Hit Dice:      ", &prevlevel, mons[u.umonnum].mlevel, STAT_OTHER);
#ifdef EXP_ON_BOTL
    else if (flags.showexp) {
        print_statdiff("Experience:    ", &prevlevel, u.ulevel, STAT_OTHER);
        /* use waddch, we don't want to highlight the '/' */
        waddch(win, '/');
        print_statdiff("", &prevexp, u.uexp, STAT_OTHER);
    }
#endif
    else
        print_statdiff("Level:         ", &prevlevel, u.ulevel, STAT_OTHER);
    wmove(win, y++, x);

    if (flags.time) {
        print_statdiff("Time:          ", &prevtime, moves, STAT_TIME);
        wmove(win, y++, x);
    }

#ifdef SCORE_ON_BOTL
    if (flags.showscore) {
        print_statdiff("Score:         ", &prevscore, botl_score(), STAT_OTHER);
        wmove(win, y++, x);
    }
#endif /* SCORE_ON_BOTL */

    curses_add_statuses(win, FALSE, TRUE, &x, &y, FALSE);
}

static void
curses_add_statuses(WINDOW *win, boolean align_right,
                    boolean vertical, int *x, int *y, boolean first)
{
    if (align_right) {
        /* Right-aligned statuses. Since add_status decrease one x more
           (to separate them with spaces), add 1 to x unless we have borders
           (which would offset what add_status does) */
        int mx = *x;
        int my = *y;
        getmaxyx(win, my, mx);
        if (!curses_window_has_border(STATUS_WIN))
            mx++;

        *x = mx;
    }

    /* Find out how much abbreviation is required to make it fit, if not using
       vertical status. do_statuseffects (used by tty and dumplogs) can print to
       a buffer without affecting the terminal unlike curses statusline code, so
       call that to find the length. */
    int abbrev = 0;
    if (!vertical) {
        int cx, cy, mx, my;
        getyx(win, cy, cx);
        getmaxyx(win, my, mx);
        char buf[MAXCO];
        for (;;) {
            buf[0] = '\0';      /* so eos() works */
            do_statuseffects(buf, FALSE, abbrev, first ? 3 : 2);
            if (abbrev >= 2 || strlen(buf) < mx - cx)
                break;
            abbrev++;
	}
    }

#define status_effect_duration(str1, str2, str3, duration)              \
    (curses_add_status(win, align_right, vertical, x, y, str1,          \
                       abbrev == 2 ? (str3) : abbrev == 1 ? (str2) : (str1), \
                       duration, first), first = FALSE)
#define status_effect(str1, str2, str3) status_effect_duration(str1, str2, str3, 0)
    long long mask = get_status_mask();
    for (int i = 0; i < SIZE(status_effects); i++) {
        struct status_effect status = status_effects[i];
        if (mask & status.mask & iflags.statuseffects) {
            long duration = get_status_duration(status.mask);
            status_effect_duration(status.name, status.abbrev1, status.abbrev2, duration);
        }
        /* Add hunger and encumbrance after foodpois */
        if (status.mask == BL_MASK_FOODPOIS) {
            if (u.uhs != NOT_HUNGRY) {
                if (uclockwork)
                    status_effect(ca_hu_stat[u.uhs], ca_hu_stat[u.uhs], ca_hu_stat[u.uhs]);
                else
                    status_effect(hu_stat[u.uhs], hu_stat[u.uhs], hu_stat[u.uhs]);
            }
            int enc = near_capacity();
            if (enc > UNENCUMBERED)
                status_effect(enc_stat[enc], enc_stat_abbrev1[enc], enc_stat_abbrev2[enc]);
        }
    }
#undef status_effect
#undef status_effect_duration
}

static void
curses_add_status(WINDOW *win, boolean align_right, boolean vertical,
                  int *x, int *y, const char *hilite, const char *str,
                  long duration, boolean first)
{
    /* If vertical is TRUE here with no x/y, that's an error. But handle
       it gracefully since NH3 doesn't recover well in crashes. */
    if (!x || !y)
        vertical = FALSE;

    if (!vertical && !align_right && !first)
        waddch(win, ' ');

    char buf[BUFSZ];
    if (duration & TIMEOUT)
        Snprintf(buf, BUFSZ, "%s:%ld", str, duration);
    else
        Strncpy(buf, str, BUFSZ);

    if (align_right) {
        *x -= (strlen(buf) + 1); /* add spacing */
        wmove(win, *y, *x);
    }

    draw_trouble_str(hilite, buf);

    if (vertical) {
        wmove(win, *y, *x);
        *y += 1; /* ++ advances the pointer addr */
    }
}

/* Decrement a single highlight, return 1 if decremented to zero. zero is TRUE if we're
   zeroing the highlight. */
static int
decrement_highlight(nhstat *stat, boolean zero)
{
    if (stat->highlight_turns > 0) {
        if (zero) {
            stat->highlight_turns = 0;
            return 1;
        }

        stat->highlight_turns--;
        if (stat->highlight_turns == 0)
            return 1;
    }
    return 0;
}

/* Decrement the highlight_turns for all stats.  Call curses_update_stats
   if needed to unhighlight a stat */
void
curses_decrement_highlights(boolean zero)
{
    int unhighlight = 0;

    unhighlight |= decrement_highlight(&prevdepth, zero);
    unhighlight |= decrement_highlight(&prevstr, zero);
    unhighlight |= decrement_highlight(&prevdex, zero);
    unhighlight |= decrement_highlight(&prevcon, zero);
    unhighlight |= decrement_highlight(&prevint, zero);
    unhighlight |= decrement_highlight(&prevwis, zero);
    unhighlight |= decrement_highlight(&prevcha, zero);
    unhighlight |= decrement_highlight(&prevau, zero);
    unhighlight |= decrement_highlight(&prevlevel, zero);
    unhighlight |= decrement_highlight(&prevdive, zero);
    unhighlight |= decrement_highlight(&prevac, zero);
	unhighlight |= decrement_highlight(&prevdr, zero);
#ifdef EXP_ON_BOTL
    unhighlight |= decrement_highlight(&prevexp, zero);
#endif
    unhighlight |= decrement_highlight(&prevtime, zero);
#ifdef SCORE_ON_BOTL
    unhighlight |= decrement_highlight(&prevscore, zero);
#endif

    if (unhighlight)
        curses_update_stats();
}
