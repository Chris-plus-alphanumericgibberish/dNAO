/*	SCCS Id: @(#)wintype.h	3.4	1996/02/18	*/
/* Copyright (c) David Cohrs, 1991				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef WINTYPE_H
#define WINTYPE_H

typedef int winid;		/* a window identifier */

/* generic parameter - must not be any larger than a pointer */
typedef union any {
    genericptr_t a_void;
    struct obj *a_obj;
    int  a_int;
    char a_char;
    long a_long;
    schar a_schar;
    /* add types as needed */
} anything;
#define ANY_P union any /* avoid typedef in prototypes */
			/* (buggy old Ultrix compiler) */

/* menu return list */
typedef struct mi {
    anything item;		/* identifier */
    long count;			/* count */
} menu_item;
#define MENU_ITEM_P struct mi

/* select_menu() "how" argument types */
#define PICK_NONE 0	/* user picks nothing (display only) */
#define PICK_ONE  1	/* only pick one */
#define PICK_ANY  2	/* can pick any amount */

/* window types */
/* any additional port specific types should be defined in win*.h */
#define NHW_MESSAGE 1
#define NHW_STATUS  2
#define NHW_MAP     3
#define NHW_MENU    4
#define NHW_TEXT    5

/* attribute types for putstr; the same as the ANSI value, for convenience */
#define ATR_NONE    0
#define ATR_BOLD    1
#define ATR_DIM     2
#define ATR_ULINE   4
#define ATR_BLINK   5
#define ATR_INVERSE 7

/* bitmask flags for mapglyph() oattr; distinct from ATR_*. Deliberately
   vague -- the player decides what each bit should render as. */
#define GLYPH_ATR_NONE     0x00
#define GLYPH_ATR_1        0x01
#define GLYPH_ATR_2        0x02

/* user-selectable rendering choices for GLYPH_ATR_1/2, set via the
   glyphattr1/glyphattr2 options; not all are renderable everywhere
   (see termcap.c/cursmain.c). */
#define GLYPHATTR_NONE        0
#define GLYPHATTR_INVERT      1
#define GLYPHATTR_BOLD        2
#define GLYPHATTR_ITALIC      3
#define GLYPHATTR_ULINE       4
#define GLYPHATTR_DOUBLEULINE 5
#define GLYPHATTR_BLINK       6
#define GLYPHATTR_CURLYULINE  7
#define GLYPHATTR_STRIKE      8
#define NUM_GLYPHATTRS        9

/* nh_poskey() modifier types */
#define CLICK_1     1
#define CLICK_2     2

/* invalid winid */
#define WIN_ERR ((winid) -1)

/* menu window keyboard commands (may be mapped) */
#define MENU_FIRST_PAGE		'^'
#define MENU_LAST_PAGE		';'
#define MENU_NEXT_PAGE		'>'
#define MENU_PREVIOUS_PAGE	'<'
#define MENU_SELECT_ALL		'.'
#define MENU_UNSELECT_ALL	']'
#define MENU_INVERT_ALL		'@'
#define MENU_SELECT_PAGE	','
#define MENU_UNSELECT_PAGE	'}'
#define MENU_INVERT_PAGE	'~'
#define MENU_SEARCH		':'


#endif /* WINTYPE_H */
