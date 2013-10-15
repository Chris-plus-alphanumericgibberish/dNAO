/*	SCCS Id: @(#)engrave.c	3.4	2001/11/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"
#include <ctype.h>

STATIC_VAR NEARDATA struct engr *head_engr;

#ifdef OVLB
/* random engravings */
static const char *random_mesg[] = {
	"Elbereth",
	/* trap engravings */
	"Vlad was here", "ad aerarium",
	/* take-offs and other famous engravings */
	"Owlbreath", "Galadriel",
	"Kilroy was here",
	"A.S. ->", "<- A.S.", /* Journey to the Center of the Earth */
	"You won't get it up the steps", /* Adventure */
	"Lasciate ogni speranza o voi ch'entrate.", /* Inferno */
	"Well Come", /* Prisoner */
	"We apologize for the inconvenience.", /* So Long... */
	"See you next Wednesday", /* Thriller */
	"notary sojak", /* Smokey Stover */
	"For a good time call 8?7-5309",
	"Please don't feed the animals.", /* Various zoos around the world */
	"Madam, in Eden, I'm Adam.", /* A palindrome */
	"Two thumbs up!", /* Siskel & Ebert */
	"Hello, World!", /* The First C Program */
#ifdef MAIL
	"You've got mail!", /* AOL */
#endif
	"As if!", /* Clueless */
        "Arooo!  Werewolves of Yendor!", /* gang tag */
        "Dig for Victory here", /* pun, duh */
        "Don't go this way",
        "Gaius Julius Primigenius was here.  Why are you late?", /* pompeii */
        "Go left --->", "<--- Go right",
        "Haermund Hardaxe carved these runes", /* viking graffiti */
        "Here be dragons",
        "Need a light?  Come visit the Minetown branch of Izchak's Lighting Store!",
        "Save now, and do your homework!",
        "Snakes on the Astral Plane - Soon in a dungeon near you",
        "There was a hole here.  It's gone now.",
        "The Vibrating Square",
        "This is a pit!",
        "This is not the dungeon you are looking for.",
        "This square deliberately left blank.",
        "Warning, Exploding runes!",
        "Watch out, there's a gnome with a wand of death behind that door!",
        "X marks the spot",
        "X <--- You are here.",
        "You are the one millionth visitor to this place!  Please wait 200 turns for your wand of wishing.",
};

char *
random_engraving(outbuf)
char *outbuf;
{
	const char *rumor;

	/* a random engraving may come from the "rumors" file,
	   or from the list above */
	if (!rn2(4) || !(rumor = getrumor(0, outbuf, TRUE)) || !*rumor)
	    Strcpy(outbuf, random_mesg[rn2(SIZE(random_mesg))]);

	wipeout_text(outbuf, (int)(strlen(outbuf) / 4), 0);
	return outbuf;
}

/* Partial rubouts for engraving characters. -3. */
static const struct {
	char		wipefrom;
	const char *	wipeto;
} rubouts[] = {
	{'A', "^"},     {'B', "Pb["},   {'C', "("},     {'D', "|)["},
	{'E', "|FL[_"}, {'F', "|-"},    {'G', "C("},    {'H', "|-"},
	{'I', "|"},     {'K', "|<"},    {'L', "|_"},    {'M', "|"},
	{'N', "|\\"},   {'O', "C("},    {'P', "F"},     {'Q', "C("},
	{'R', "PF"},    {'T', "|"},     {'U', "J"},     {'V', "/\\"},
	{'W', "V/\\"},  {'Z', "/"},
	{'b', "|"},     {'d', "c|"},    {'e', "c"},     {'g', "c"},
	{'h', "n"},     {'j', "i"},     {'k', "|"},     {'l', "|"},
	{'m', "nr"},    {'n', "r"},     {'o', "c"},     {'q', "c"},
	{'w', "v"},     {'y', "v"},
	{':', "."},     {';', ","},
	{'0', "C("},    {'1', "|"},     {'6', "o"},     {'7', "/"},
	{'8', "3o"}
};

void
wipeout_text(engr, cnt, seed)
char *engr;
int cnt;
unsigned seed;		/* for semi-controlled randomization */
{
	char *s;
	int i, j, nxt, use_rubout, lth = (int)strlen(engr);

	if (lth && cnt > 0) {
	    while (cnt--) {
		/* pick next character */
		if (!seed) {
		    /* random */
		    nxt = rn2(lth);
		    use_rubout = rn2(4);
		} else {
		    /* predictable; caller can reproduce the same sequence by
		       supplying the same arguments later, or a pseudo-random
		       sequence by varying any of them */
		    nxt = seed % lth;
		    seed *= 31,  seed %= (BUFSZ-1);
		    use_rubout = seed & 3;
		}
		s = &engr[nxt];
		if (*s == ' ') continue;

		/* rub out unreadable & small punctuation marks */
		if (index("?.,'`-|_", *s)) {
		    *s = ' ';
		    continue;
		}

		if (!use_rubout)
		    i = SIZE(rubouts);
		else
		    for (i = 0; i < SIZE(rubouts); i++)
			if (*s == rubouts[i].wipefrom) {
			    /*
			     * Pick one of the substitutes at random.
			     */
			    if (!seed)
				j = rn2(strlen(rubouts[i].wipeto));
			    else {
				seed *= 31,  seed %= (BUFSZ-1);
				j = seed % (strlen(rubouts[i].wipeto));
			    }
			    *s = rubouts[i].wipeto[j];
			    break;
			}

		/* didn't pick rubout; use '?' for unreadable character */
		if (i == SIZE(rubouts)) *s = '?';
	    }
	}

	/* trim trailing spaces */
	while (lth && engr[lth-1] == ' ') engr[--lth] = 0;
}

boolean
can_reach_floor()
{
	return (boolean)(!u.uswallow &&
#ifdef STEED
			/* Restricted/unskilled riders can't reach the floor */
			!(u.usteed && P_SKILL(P_RIDING) < P_BASIC) &&
#endif
			 (!Levitation ||
			  Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)));
}
#endif /* OVLB */
#ifdef OVL0

const char *
surface(x, y)
register int x, y;
{
	register struct rm *lev = &levl[x][y];

	if ((x == u.ux) && (y == u.uy) && u.uswallow &&
		is_animal(u.ustuck->data))
	    return "maw";
	else if (IS_AIR(lev->typ) && Is_airlevel(&u.uz))
	    return "air";
	else if (is_pool(x,y))
	    return (Underwater && !Is_waterlevel(&u.uz)) ? "bottom" : "water";
	else if (is_ice(x,y))
	    return "ice";
	else if (is_lava(x,y))
	    return "lava";
	else if (lev->typ == DRAWBRIDGE_DOWN)
	    return "bridge";
	else if(IS_ALTAR(levl[x][y].typ))
	    return "altar";
	else if(IS_GRAVE(levl[x][y].typ))
	    return "headstone";
	else if(IS_FOUNTAIN(levl[x][y].typ))
	    return "fountain";
	else if ((IS_ROOM(lev->typ) && !Is_earthlevel(&u.uz)) ||
		 IS_WALL(lev->typ) || IS_DOOR(lev->typ) || lev->typ == SDOOR)
	    return "floor";
	else
	    return "ground";
}

const char *
ceiling(x, y)
register int x, y;
{
	register struct rm *lev = &levl[x][y];
	const char *what;

	/* other room types will no longer exist when we're interested --
	 * see check_special_room()
	 */
	if (*in_rooms(x,y,VAULT))
	    what = "vault's ceiling";
	else if (*in_rooms(x,y,TEMPLE))
	    what = "temple's ceiling";
	else if (*in_rooms(x,y,SHOPBASE))
	    what = "shop's ceiling";
	else if (IS_AIR(lev->typ))
	    what = "sky";
	else if (Underwater)
	    what = "water's surface";
	else if ((IS_ROOM(lev->typ) && !Is_earthlevel(&u.uz)) ||
		 IS_WALL(lev->typ) || IS_DOOR(lev->typ) || lev->typ == SDOOR)
	    what = "ceiling";
	else
	    what = "rock above";

	return what;
}

struct engr *
engr_at(x, y)
xchar x, y;
{
	register struct engr *ep = head_engr;

	while(ep) {
		if(x == ep->engr_x && y == ep->engr_y)
			return(ep);
		ep = ep->nxt_engr;
	}
	return((struct engr *) 0);
}

#ifdef ELBERETH
/* Decide whether a particular string is engraved at a specified
 * location; a case-insensitive substring match used.
 * Ignore headstones, in case the player names herself "Elbereth".
 */
int
sengr_at(s, x, y)
	const char *s;
	xchar x, y;
{
	register struct engr *ep = engr_at(x,y);

	return (ep && ep->engr_type != HEADSTONE &&
		ep->engr_time <= moves && strstri(ep->engr_txt, s) != 0);
}
#endif /* ELBERETH */

#endif /* OVL0 */
#ifdef OVL2

void
u_wipe_engr(cnt)
register int cnt;
{
	if (can_reach_floor())
		wipe_engr_at(u.ux, u.uy, cnt);
}

#endif /* OVL2 */
#ifdef OVL1

void
wipe_engr_at(x,y,cnt)
register xchar x,y,cnt;
{
	register struct engr *ep = engr_at(x,y);

	/* Headstones are indelible */
	if(ep && ep->engr_type != HEADSTONE){
	    if(ep->engr_type != BURN || is_ice(x,y)) {
		if(ep->engr_type != DUST && ep->engr_type != ENGR_BLOOD) {
			cnt = rn2(1 + 50/(cnt+1)) ? 0 : 1;
		}
		wipeout_text(ep->engr_txt, (int)cnt, 0);
		while(ep->engr_txt[0] == ' ')
			ep->engr_txt++;
		if(!ep->engr_txt[0]) del_engr(ep);
	    }
	}
}

#endif /* OVL1 */
#ifdef OVL2

void
read_engr_at(x,y)
register int x,y;
{
	register struct engr *ep = engr_at(x,y);
	register int	sensed = 0;
	char buf[BUFSZ];
	
	/* Sensing an engraving does not require sight,
	 * nor does it necessarily imply comprehension (literacy).
	 */
	if(ep && ep->engr_txt[0]) {
	    switch(ep->engr_type) {
	    case DUST:
		if(!Blind) {
			sensed = 1;
			pline("%s is written here in the %s.", Something,
				is_ice(x,y) ? "frost" : "dust");
		}
		break;
	    case ENGRAVE:
	    case HEADSTONE:
		if (!Blind || can_reach_floor()) {
			sensed = 1;
			pline("%s is engraved here on the %s.",
				Something,
				surface(x,y));
		}
		break;
	    case BURN:
		if (!Blind || can_reach_floor()) {
			sensed = 1;
			pline("Some text has been %s into the %s here.",
				is_ice(x,y) ? "melted" : "burned",
				surface(x,y));
		}
		break;
	    case MARK:
		if(!Blind) {
			sensed = 1;
			pline("There's some graffiti on the %s here.",
				surface(x,y));
		}
		break;
	    case ENGR_BLOOD:
		/* "It's a message!  Scrawled in blood!"
		 * "What's it say?"
		 * "It says... `See you next Wednesday.'" -- Thriller
		 */
		if(!Blind) {
			sensed = 1;
			You("see a message scrawled in blood here.");
		}
		break;
	    default:
		impossible("%s is written in a very strange way.",
				Something);
		sensed = 1;
	    }
	    if (sensed) {
	    	char *et;
	    	unsigned maxelen = BUFSZ - sizeof("You feel the words: \"\". ");
	    	if (strlen(ep->engr_txt) > maxelen) {
	    		(void) strncpy(buf,  ep->engr_txt, (int)maxelen);
			buf[maxelen] = '\0';
			et = buf;
		} else
			et = ep->engr_txt;
		You("%s: \"%s\".",
		      (Blind) ? "feel the words" : "read",  et);
		if(flags.run > 1) nomul(0, NULL);
	    }
	}
}

#endif /* OVL2 */
#ifdef OVLB

void
make_engr_at(x,y,s,e_time,e_type)
register int x,y;
register const char *s;
register long e_time;
register xchar e_type;
{
	register struct engr *ep;

	if ((ep = engr_at(x,y)) != 0)
	    del_engr(ep);
	ep = newengr(strlen(s) + 1);
	ep->nxt_engr = head_engr;
	head_engr = ep;
	ep->engr_x = x;
	ep->engr_y = y;
	ep->engr_txt = (char *)(ep + 1);
	Strcpy(ep->engr_txt, s);
	/* engraving Elbereth shows wisdom */
	if (!in_mklev && !strcmp(s, "Elbereth")) exercise(A_WIS, TRUE);
	ep->engr_time = e_time;
	ep->engr_type = e_type > 0 ? e_type : rnd(N_ENGRAVE-1);
	ep->engr_lth = strlen(s) + 1;
}

/* delete any engraving at location <x,y> */
void
del_engr_at(x, y)
int x, y;
{
	register struct engr *ep = engr_at(x, y);

	if (ep) del_engr(ep);
}

/*
 *	freehand - returns true if player has a free hand
 */
int
freehand()
{
	return(!uwep || !welded(uwep) ||
	   (!bimanual(uwep) && (!uarms || !uarms->cursed)));
/*	if ((uwep && bimanual(uwep)) ||
	    (uwep && uarms))
		return(0);
	else
		return(1);*/
}

static NEARDATA const char styluses[] =
	{ ALL_CLASSES, ALLOW_NONE, TOOL_CLASS, WEAPON_CLASS, WAND_CLASS,
	  GEM_CLASS, RING_CLASS, 0 };

/* Mohs' Hardness Scale:
 *  1 - Talc		 6 - Orthoclase
 *  2 - Gypsum		 7 - Quartz
 *  3 - Calcite		 8 - Topaz
 *  4 - Fluorite	 9 - Corundum
 *  5 - Apatite		10 - Diamond
 *
 * Since granite is a igneous rock hardness ~ 7, anything >= 8 should
 * probably be able to scratch the rock.
 * Devaluation of less hard gems is not easily possible because obj struct
 * does not contain individual oc_cost currently. 7/91
 *
 * steel     -	5-8.5	(usu. weapon)
 * diamond    - 10			* jade	     -	5-6	 (nephrite)
 * ruby       -  9	(corundum)	* turquoise  -	5-6
 * sapphire   -  9	(corundum)	* opal	     -	5-6
 * topaz      -  8			* glass      - ~5.5
 * emerald    -  7.5-8	(beryl)		* dilithium  -	4-5??
 * aquamarine -  7.5-8	(beryl)		* iron	     -	4-5
 * garnet     -  7.25	(var. 6.5-8)	* fluorite   -	4
 * agate      -  7	(quartz)	* brass      -	3-4
 * amethyst   -  7	(quartz)	* gold	     -	2.5-3
 * jasper     -  7	(quartz)	* silver     -	2.5-3
 * onyx       -  7	(quartz)	* copper     -	2.5-3
 * moonstone  -  6	(orthoclase)	* amber      -	2-2.5
 */

/* return 1 if action took 1 (or more) moves, 0 if error or aborted */
int
doengrave()
{
	boolean dengr = FALSE;	/* TRUE if we wipe out the current engraving */
	boolean doblind = FALSE;/* TRUE if engraving blinds the player */
	boolean doknown = FALSE;/* TRUE if we identify the stylus */
	boolean eow = FALSE;	/* TRUE if we are overwriting oep */
	boolean jello = FALSE;	/* TRUE if we are engraving in slime */
	boolean ptext = TRUE;	/* TRUE if we must prompt for engrave text */
	boolean teleengr =FALSE;/* TRUE if we move the old engraving */
	boolean zapwand = FALSE;/* TRUE if we remove a wand charge */
	xchar type = DUST;	/* Type of engraving made */
	char buf[BUFSZ];	/* Buffer for final/poly engraving text */
	char ebuf[BUFSZ];	/* Buffer for initial engraving text */
	char qbuf[QBUFSZ];	/* Buffer for query text */
	char post_engr_text[BUFSZ]; /* Text displayed after engraving prompt */
	const char *everb;	/* Present tense of engraving type */
	const char *eloc;	/* Where the engraving is (ie dust/floor/...) */
	char *sp;		/* Place holder for space count of engr text */
	int len;		/* # of nonspace chars of new engraving text */
	int maxelen;		/* Max allowable length of engraving text */
	struct engr *oep = engr_at(u.ux,u.uy);
				/* The current engraving */
	struct obj *otmp;	/* Object selected with which to engrave */
	char *writer;

	multi = 0;		/* moves consumed */
	nomovemsg = (char *)0;	/* occupation end message */

	buf[0] = (char)0;
	ebuf[0] = (char)0;
	post_engr_text[0] = (char)0;
	maxelen = BUFSZ - 1;
	if (is_demon(youmonst.data) || youmonst.data->mlet == S_VAMPIRE)
	    type = ENGR_BLOOD;

	/* Can the adventurer engrave at all? */

	if(u.uswallow) {
		if (is_animal(u.ustuck->data)) {
			pline("What would you write?  \"Jonah was here\"?");
			return(0);
		} else if (is_whirly(u.ustuck->data)) {
			You_cant("reach the %s.", surface(u.ux,u.uy));
			return(0);
		} else
			jello = TRUE;
	} else if (is_lava(u.ux, u.uy)) {
		You_cant("write on the lava!");
		return(0);
	} else if (is_pool(u.ux,u.uy) || IS_FOUNTAIN(levl[u.ux][u.uy].typ)) {
		You_cant("write on the water!");
		return(0);
	}
	if(Is_airlevel(&u.uz) || Is_waterlevel(&u.uz)/* in bubble */) {
		You_cant("write in thin air!");
		return(0);
	}
	if (cantwield(youmonst.data)) {
		You_cant("even hold anything!");
		return(0);
	}
	if (check_capacity((char *)0)) return (0);

	/* One may write with finger, or weapon, or wand, or..., or...
	 * Edited by GAN 10/20/86 so as not to change weapon wielded.
	 */

	otmp = getobj(styluses, "write with");
	if(!otmp) return(0);		/* otmp == zeroobj if fingers */

	if (otmp == &zeroobj) writer = makeplural(body_part(FINGER));
	else writer = xname(otmp);

	/* There's no reason you should be able to write with a wand
	 * while both your hands are tied up.
	 */
	if (!freehand() && otmp != uwep && !otmp->owornmask) {
		You("have no free %s to write with!", body_part(HAND));
		return(0);
	}

	if (jello) {
		You("tickle %s with your %s.", mon_nam(u.ustuck), writer);
		Your("message dissolves...");
		return(0);
	}
	if (otmp->oclass != WAND_CLASS && !can_reach_floor()) {
		You_cant("reach the %s!", surface(u.ux,u.uy));
		return(0);
	}
	if (IS_ALTAR(levl[u.ux][u.uy].typ)) {
		You("make a motion towards the altar with your %s.", writer);
		altar_wrath(u.ux, u.uy);
		return(0);
	}
	if (IS_GRAVE(levl[u.ux][u.uy].typ)) {
	    if (otmp == &zeroobj) { /* using only finger */
		You("would only make a small smudge on the %s.",
			surface(u.ux, u.uy));
		return(0);
	    } else if (!levl[u.ux][u.uy].disturbed) {
		You("disturb the undead!");
		levl[u.ux][u.uy].disturbed = 1;
		(void) makemon(&mons[PM_GHOUL], u.ux, u.uy, NO_MM_FLAGS);
		exercise(A_WIS, FALSE);
		return(1);
	    }
	}

	/* SPFX for items */

	switch (otmp->oclass) {
	    default:
	    case AMULET_CLASS:
	    case CHAIN_CLASS:
	    case POTION_CLASS:
	    case COIN_CLASS:
		break;

	    case RING_CLASS:
		/* "diamond" rings and others should work */
	    case GEM_CLASS:
		/* diamonds & other hard gems should work */
		if (objects[otmp->otyp].oc_tough) {
			type = ENGRAVE;
			break;
		}
		break;

	    case ARMOR_CLASS:
		if (is_boots(otmp)) {
			type = DUST;
			break;
		}
		/* fall through */
	    /* Objects too large to engrave with */
	    case BALL_CLASS:
	    case ROCK_CLASS:
		You_cant("engrave with such a large object!");
		ptext = FALSE;
		break;

	    /* Objects too silly to engrave with */
	    case FOOD_CLASS:
	    case SCROLL_CLASS:
	    case SPBOOK_CLASS:
		Your("%s would get %s.", xname(otmp),
			is_ice(u.ux,u.uy) ? "all frosty" : "too dirty");
		ptext = FALSE;
		break;

	    case RANDOM_CLASS:	/* This should mean fingers */
		break;

	    /* The charge is removed from the wand before prompting for
	     * the engraving text, because all kinds of setup decisions
	     * and pre-engraving messages are based upon knowing what type
	     * of engraving the wand is going to do.  Also, the player
	     * will have potentially seen "You wrest .." message, and
	     * therefore will know they are using a charge.
	     */
	    case WAND_CLASS:
		if (zappable(otmp)) {
		    check_unpaid(otmp);
		    zapwand = TRUE;
		    if (Levitation) ptext = FALSE;

		    switch (otmp->otyp) {
		    /* DUST wands */
		    default:
			break;

			/* NODIR wands */
		    case WAN_LIGHT:
		    case WAN_SECRET_DOOR_DETECTION:
		    case WAN_CREATE_MONSTER:
		    case WAN_WISHING:
		    case WAN_ENLIGHTENMENT:
			zapnodir(otmp);
			break;

			/* IMMEDIATE wands */
			/* If wand is "IMMEDIATE", remember to affect the
			 * previous engraving even if turning to dust.
			 */
		    case WAN_STRIKING:
			Strcpy(post_engr_text,
			"The wand unsuccessfully fights your attempt to write!"
			);
			break;
		    case WAN_SLOW_MONSTER:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The bugs on the %s slow down!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_SPEED_MONSTER:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The bugs on the %s speed up!",
				   surface(u.ux, u.uy));
			}
			break;
		    case WAN_POLYMORPH:
			if(oep)  {
			    if (!Blind) {
				type = (xchar)0;	/* random */
				(void) random_engraving(buf);
			    }
			    dengr = TRUE;
			}
			break;
		    case WAN_NOTHING:
		    case WAN_UNDEAD_TURNING:
		    case WAN_OPENING:
		    case WAN_LOCKING:
		    case WAN_PROBING:
			break;

			/* RAY wands */
		    case WAN_MAGIC_MISSILE:
			ptext = TRUE;
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The %s is riddled by bullet holes!",
				   surface(u.ux, u.uy));
			}
			break;

		    /* can't tell sleep from death - Eric Backus */
		    case WAN_SLEEP:
		    case WAN_DEATH:
			if (!Blind) {
			   Sprintf(post_engr_text,
				   "The bugs on the %s stop moving!",
				   surface(u.ux, u.uy));
			}
			break;

		    case WAN_COLD:
			if (!Blind)
			    Strcpy(post_engr_text,
				"A few ice cubes drop from the wand.");
			if(!oep || (oep->engr_type != BURN))
			    break;
		    case WAN_CANCELLATION:
		    case WAN_MAKE_INVISIBLE:
			if (oep && oep->engr_type != HEADSTONE) {
			    if (!Blind)
				pline_The("engraving on the %s vanishes!",
					surface(u.ux,u.uy));
			    dengr = TRUE;
			}
			break;
		    case WAN_TELEPORTATION:
			if (oep && oep->engr_type != HEADSTONE) {
			    if (!Blind)
				pline_The("engraving on the %s vanishes!",
					surface(u.ux,u.uy));
			    teleengr = TRUE;
			}
			break;

		    /* type = ENGRAVE wands */
		    case WAN_DIGGING:
			ptext = TRUE;
			type  = ENGRAVE;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
				pline("This %s is a wand of digging!",
				xname(otmp));
			    doknown = TRUE;
			}
			if (!Blind)
			    Strcpy(post_engr_text,
				IS_GRAVE(levl[u.ux][u.uy].typ) ?
				"Chips fly out from the headstone." :
				is_ice(u.ux,u.uy) ?
				"Ice chips fly up from the ice surface!" :
				"Gravel flies up from the floor.");
			else
			    Strcpy(post_engr_text, "You hear drilling!");
			break;

		    /* type = BURN wands */
		    case WAN_FIRE:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			if (flags.verbose)
			    pline("This %s is a wand of fire!", xname(otmp));
			    doknown = TRUE;
			}
			Strcpy(post_engr_text,
				Blind ? "You feel the wand heat up." :
					"Flames fly from the wand.");
			break;
		    case WAN_LIGHTNING:
			ptext = TRUE;
			type  = BURN;
			if(!objects[otmp->otyp].oc_name_known) {
			    if (flags.verbose)
				pline("This %s is a wand of lightning!",
					xname(otmp));
			    doknown = TRUE;
			}
			if (!Blind) {
			    Strcpy(post_engr_text,
				    "Lightning arcs from the wand.");
			    doblind = TRUE;
			} else
			    Strcpy(post_engr_text, "You hear crackling!");
			break;

		    /* type = MARK wands */
		    /* type = ENGR_BLOOD wands */
		    }
		} else /* end if zappable */
		    if (!can_reach_floor()) {
			You_cant("reach the %s!", surface(u.ux,u.uy));
			return(0);
		    }
		break;

	    case WEAPON_CLASS:
		if (is_blade(otmp)) {
		    if ((int)otmp->spe > -3)
			type = ENGRAVE;
		    else
			Your("%s too dull for engraving.", aobjnam(otmp,"are"));
		}
		break;

	    case TOOL_CLASS:
		if(otmp == ublindf) {
		    pline(
		"That is a bit difficult to engrave with, don't you think?");
		    return(0);
		}
		switch (otmp->otyp)  {
		    case MAGIC_MARKER:
			if (otmp->spe <= 0)
			    Your("marker has dried out.");
			else
			    type = MARK;
			break;
		    case TOWEL:
			/* Can't really engrave with a towel */
			ptext = FALSE;
			if (oep)
			    if ((oep->engr_type == DUST ) ||
				(oep->engr_type == ENGR_BLOOD) ||
				(oep->engr_type == MARK )) {
				if (!Blind)
				    You("wipe out the message here.");
				else
				    Your("%s %s %s.", xname(otmp),
					 otense(otmp, "get"),
					 is_ice(u.ux,u.uy) ?
					 "frosty" : "dusty");
				dengr = TRUE;
			    } else
				Your("%s can't wipe out this engraving.",
				     xname(otmp));
			else
			    Your("%s %s %s.", xname(otmp), otense(otmp, "get"),
				  is_ice(u.ux,u.uy) ? "frosty" : "dusty");
			break;
		    default:
			break;
		}
		break;

	    case VENOM_CLASS:
#ifdef WIZARD
		if (wizard) {
		    pline("Writing a poison pen letter??");
		    break;
		}
#endif
	    case ILLOBJ_CLASS:
		impossible("You're engraving with an illegal object!");
		break;
	}

	if (IS_GRAVE(levl[u.ux][u.uy].typ)) {
	    if (type == ENGRAVE || type == 0)
		type = HEADSTONE;
	    else {
		/* ensures the "cannot wipe out" case */
		type = DUST;
		dengr = FALSE;
		teleengr = FALSE;
		buf[0] = (char)0;
	    }
	}

	/* End of implement setup */

	/* Identify stylus */
	if (doknown) {
	    makeknown(otmp->otyp);
	    more_experienced(0,10);
	}

	if (teleengr) {
	    rloc_engr(oep);
	    oep = (struct engr *)0;
	}

	if (dengr) {
	    del_engr(oep);
	    oep = (struct engr *)0;
	}

	/* Something has changed the engraving here */
	if (*buf) {
	    make_engr_at(u.ux, u.uy, buf, moves, type);
	    pline_The("engraving now reads: \"%s\".", buf);
	    ptext = FALSE;
	}

	if (zapwand && (otmp->spe < 0)) {
	    pline("%s %sturns to dust.",
		  The(xname(otmp)), Blind ? "" : "glows violently, then ");
	    if (!IS_GRAVE(levl[u.ux][u.uy].typ))
		You("are not going to get anywhere trying to write in the %s with your dust.",
		    is_ice(u.ux,u.uy) ? "frost" : "dust");
	    useup(otmp);
	    ptext = FALSE;
	}

	if (!ptext) {		/* Early exit for some implements. */
	    if (otmp->oclass == WAND_CLASS && !can_reach_floor())
		You_cant("reach the %s!", surface(u.ux,u.uy));
	    return(1);
	}

	/* Special effects should have deleted the current engraving (if
	 * possible) by now.
	 */

	if (oep) {
	    register char c = 'n';

	    /* Give player the choice to add to engraving. */

	    if (type == HEADSTONE) {
		/* no choice, only append */
		c = 'y';
	    } else if ( (type == oep->engr_type) && (!Blind ||
		 (oep->engr_type == BURN) || (oep->engr_type == ENGRAVE)) ) {
		c = yn_function("Do you want to add to the current engraving?",
				ynqchars, 'y');
		if (c == 'q') {
		    pline(Never_mind);
		    return(0);
		}
	    }

	    if (c == 'n' || Blind) {

		if( (oep->engr_type == DUST) || (oep->engr_type == ENGR_BLOOD) ||
		    (oep->engr_type == MARK) ) {
		    if (!Blind) {
			You("wipe out the message that was %s here.",
			    ((oep->engr_type == DUST)  ? "written in the dust" :
			    ((oep->engr_type == ENGR_BLOOD) ? "scrawled in blood"   :
							 "written")));
			del_engr(oep);
			oep = (struct engr *)0;
		    } else
		   /* Don't delete engr until after we *know* we're engraving */
			eow = TRUE;
		} else
		    if ( (type == DUST) || (type == MARK) || (type == ENGR_BLOOD) ) {
			You(
			 "cannot wipe out the message that is %s the %s here.",
			 oep->engr_type == BURN ?
			   (is_ice(u.ux,u.uy) ? "melted into" : "burned into") :
			   "engraved in", surface(u.ux,u.uy));
			return(1);
		    } else
			if ( (type != oep->engr_type) || (c == 'n') ) {
			    if (!Blind || can_reach_floor())
				You("will overwrite the current message.");
			    eow = TRUE;
			}
	    }
	}

	eloc = surface(u.ux,u.uy);
	switch(type){
	    default:
		everb = (oep && !eow ? "add to the weird writing on" :
				       "write strangely on");
		break;
	    case DUST:
		everb = (oep && !eow ? "add to the writing in" :
				       "write in");
		eloc = is_ice(u.ux,u.uy) ? "frost" : "dust";
		break;
	    case HEADSTONE:
		everb = (oep && !eow ? "add to the epitaph on" :
				       "engrave on");
		break;
	    case ENGRAVE:
		everb = (oep && !eow ? "add to the engraving in" :
				       "engrave in");
		break;
	    case BURN:
		everb = (oep && !eow ?
			( is_ice(u.ux,u.uy) ? "add to the text melted into" :
					      "add to the text burned into") :
			( is_ice(u.ux,u.uy) ? "melt into" : "burn into"));
		break;
	    case MARK:
		everb = (oep && !eow ? "add to the graffiti on" :
				       "scribble on");
		break;
	    case ENGR_BLOOD:
		everb = (oep && !eow ? "add to the scrawl on" :
				       "scrawl on");
		break;
	}

	/* Tell adventurer what is going on */
	if (otmp != &zeroobj)
	    You("%s the %s with %s.", everb, eloc, doname(otmp));
	else
	    You("%s the %s with your %s.", everb, eloc,
		makeplural(body_part(FINGER)));

	/* Prompt for engraving! */
	Sprintf(qbuf,"What do you want to %s the %s here?", everb, eloc);
	getlin(qbuf, ebuf);

	/* Count the actual # of chars engraved not including spaces */
	len = strlen(ebuf);
	for (sp = ebuf; *sp; sp++) if (isspace(*sp)) len -= 1;

	if (len == 0 || index(ebuf, '\033')) {
	    if (zapwand) {
		if (!Blind)
		    pline("%s, then %s.",
			  Tobjnam(otmp, "glow"), otense(otmp, "fade"));
		return(1);
	    } else {
		pline(Never_mind);
		return(0);
	    }
	}

	/* A single `x' is the traditional signature of an illiterate person */
	if (len != 1 || (!index(ebuf, 'x') && !index(ebuf, 'X')))
	    u.uconduct.literate++;

	/* Mix up engraving if surface or state of mind is unsound.
	   Note: this won't add or remove any spaces. */
	for (sp = ebuf; *sp; sp++) {
	    if (isspace(*sp)) continue;
	    if (((type == DUST || type == ENGR_BLOOD) && !rn2(25)) ||
		    (Blind && !rn2(11)) || (Confusion && !rn2(7)) ||
		    (Stunned && !rn2(4)) || (Hallucination && !rn2(2)))
		*sp = ' ' + rnd(96 - 2);	/* ASCII '!' thru '~'
						   (excludes ' ' and DEL) */
	}

	/* Previous engraving is overwritten */
	if (eow) {
	    del_engr(oep);
	    oep = (struct engr *)0;
	}

	/* Figure out how long it took to engrave, and if player has
	 * engraved too much.
	 */
	switch(type){
	    default:
		multi = -(len/10);
		if (multi) nomovemsg = "You finish your weird engraving.";
		break;
	    case DUST:
		multi = -(len/10);
		if (multi) nomovemsg = "You finish writing in the dust.";
		break;
	    case HEADSTONE:
	    case ENGRAVE:
		multi = -(len/10);
		if ((otmp->oclass == WEAPON_CLASS) &&
		    ((otmp->otyp != ATHAME) || otmp->cursed)) {
		    multi = -len;
		    maxelen = ((otmp->spe + 3) * 2) + 1;
			/* -2 = 3, -1 = 5, 0 = 7, +1 = 9, +2 = 11
			 * Note: this does not allow a +0 anything (except
			 *	 an athame) to engrave "Elbereth" all at once.
			 *	 However, you could now engrave "Elb", then
			 *	 "ere", then "th".
			 */
		    Your("%s dull.", aobjnam(otmp, "get"));
		    if (otmp->unpaid) {
			struct monst *shkp = shop_keeper(*u.ushops);
			if (shkp) {
			    You("damage it, you pay for it!");
			    bill_dummy_object(otmp);
			}
		    }
		    if (len > maxelen) {
			multi = -maxelen;
			otmp->spe = -3;
		    } else if (len > 1)
			otmp->spe -= len >> 1;
		    else otmp->spe -= 1; /* Prevent infinite engraving */
		} else
		    if ( (otmp->oclass == RING_CLASS) ||
			 (otmp->oclass == GEM_CLASS) )
			multi = -len;
		if (multi) nomovemsg = "You finish engraving.";
		break;
	    case BURN:
		multi = -(len/10);
		if (multi)
		    nomovemsg = is_ice(u.ux,u.uy) ?
			"You finish melting your message into the ice.":
			"You finish burning your message into the floor.";
		break;
	    case MARK:
		multi = -(len/10);
		if ((otmp->oclass == TOOL_CLASS) &&
		    (otmp->otyp == MAGIC_MARKER)) {
		    maxelen = (otmp->spe) * 2; /* one charge / 2 letters */
		    if (len > maxelen) {
			Your("marker dries out.");
			otmp->spe = 0;
			multi = -(maxelen/10);
		    } else
			if (len > 1) otmp->spe -= len >> 1;
			else otmp->spe -= 1; /* Prevent infinite grafitti */
		}
		if (multi) nomovemsg = "You finish defacing the dungeon.";
		break;
	    case ENGR_BLOOD:
		multi = -(len/10);
		if (multi) nomovemsg = "You finish scrawling.";
		break;
	}

	/* Chop engraving down to size if necessary */
	if (len > maxelen) {
	    for (sp = ebuf; (maxelen && *sp); sp++)
		if (!isspace(*sp)) maxelen--;
	    if (!maxelen && *sp) {
		*sp = (char)0;
		if (multi) nomovemsg = "You cannot write any more.";
		You("only are able to write \"%s\"", ebuf);
	    }
	}

	/* Add to existing engraving */
	if (oep) Strcpy(buf, oep->engr_txt);

	(void) strncat(buf, ebuf, (BUFSZ - (int)strlen(buf) - 1));

	make_engr_at(u.ux, u.uy, buf, (moves - multi), type);

	if (post_engr_text[0]) pline(post_engr_text);

	if (doblind && !resists_blnd(&youmonst)) {
	    You("are blinded by the flash!");
	    make_blinded((long)rnd(50),FALSE);
	    if (!Blind) Your(vision_clears);
	}

	return(1);
}

void
save_engravings(fd, mode)
int fd, mode;
{
	register struct engr *ep = head_engr;
	register struct engr *ep2;
	unsigned no_more_engr = 0;

	while (ep) {
	    ep2 = ep->nxt_engr;
	    if (ep->engr_lth && ep->engr_txt[0] && perform_bwrite(mode)) {
		bwrite(fd, (genericptr_t)&(ep->engr_lth), sizeof(ep->engr_lth));
		bwrite(fd, (genericptr_t)ep, sizeof(struct engr) + ep->engr_lth);
	    }
	    if (release_data(mode))
		dealloc_engr(ep);
	    ep = ep2;
	}
	if (perform_bwrite(mode))
	    bwrite(fd, (genericptr_t)&no_more_engr, sizeof no_more_engr);
	if (release_data(mode))
	    head_engr = 0;
}

void
rest_engravings(fd)
int fd;
{
	register struct engr *ep;
	unsigned lth;

	head_engr = 0;
	while(1) {
		mread(fd, (genericptr_t) &lth, sizeof(unsigned));
		if(lth == 0) return;
		ep = newengr(lth);
		mread(fd, (genericptr_t) ep, sizeof(struct engr) + lth);
		ep->nxt_engr = head_engr;
		head_engr = ep;
		ep->engr_txt = (char *) (ep + 1);	/* Andreas Bormann */
		/* mark as finished for bones levels -- no problem for
		 * normal levels as the player must have finished engraving
		 * to be able to move again */
		ep->engr_time = moves;
	}
}

void
del_engr(ep)
register struct engr *ep;
{
	if (ep == head_engr) {
		head_engr = ep->nxt_engr;
	} else {
		register struct engr *ept;

		for (ept = head_engr; ept; ept = ept->nxt_engr)
		    if (ept->nxt_engr == ep) {
			ept->nxt_engr = ep->nxt_engr;
			break;
		    }
		if (!ept) {
		    impossible("Error in del_engr?");
		    return;
		}
	}
	dealloc_engr(ep);
}

/* randomly relocate an engraving */
void
rloc_engr(ep)
struct engr *ep;
{
	int tx, ty, tryct = 200;

	do  {
	    if (--tryct < 0) return;
	    tx = rn1(COLNO-3,2);
	    ty = rn2(ROWNO);
	} while (engr_at(tx, ty) ||
		!goodpos(tx, ty, (struct monst *)0, 0));

	ep->engr_x = tx;
	ep->engr_y = ty;
}


/* Epitaphs for random headstones */
static const char *epitaphs[] = {
	"Rest in peace",
	"R.I.P.",
	"Rest In Pieces",
	"Note -- there are NO valuable items in this grave",
	"1994-1995. The Longest-Lived Hacker Ever",
	"The Grave of the Unknown Hacker",
	"We weren't sure who this was, but we buried him here anyway",
	"Sparky -- he was a very good dog",
	"Beware of Electric Third Rail",
	"Made in Taiwan",
	"Og friend. Og good dude. Og died. Og now food",
	"Beetlejuice Beetlejuice Beetlejuice",
	"Look out below!",
	"Please don't dig me up. I'm perfectly happy down here. -- Resident",
	"Postman, please note forwarding address: Gehennom, Asmodeus's Fortress, fifth lemure on the left",
	"Mary had a little lamb/Its fleece was white as snow/When Mary was in trouble/The lamb was first to go",
	"Be careful, or this could happen to you!",
	"Soon you'll join this fellow in hell! -- the Wizard of Yendor",
	"Caution! This grave contains toxic waste",
	"Sum quod eris",
	"Here lies an Atheist, all dressed up and no place to go",
	"Here lies Ezekiel, age 102.  The good die young.",
	"Here lies my wife: Here let her lie! Now she's at rest and so am I.",
	"Here lies Johnny Yeast. Pardon me for not rising.",
	"He always lied while on the earth and now he's lying in it",
	"I made an ash of myself",
	"Soon ripe. Soon rotten. Soon gone. But not forgotten.",
	"Here lies the body of Jonathan Blake. Stepped on the gas instead of the brake.",
	"Go away!",
        "Alas fair Death, 'twas missed in life - some peace and quiet from my wife",
        "Applaud, my friends, the comedy is finished.",
        "At last... a nice long sleep.",
        "Audi Partem Alteram",
        "Basil, assaulted by bears",
        "Burninated",
        "Confusion will be my epitaph",
        "Do not open until Christmas",
        "Don't be daft, they couldn't hit an elephant at this dist-",
        "Don't forget to stop and smell the roses",
        "Don't let this happen to you!",
        "Dulce et decorum est pro patria mori",
        "Et in Arcadia ego",
        "Fatty and skinny went to bed.  Fatty rolled over and skinny was dead.  Skinny Smith 1983-2000.",
        "Finally I am becoming stupider no more",
        "Follow me to hell",
        "...for famous men have the whole earth as their memorial",
        "Game over, man.  Game over.",
        "Go away!  I'm trying to take a nap in here!  Bloody adventurers...",
        "Gone fishin'",
        "Good night, sweet prince: And flights of angels sing thee to thy rest!",
        "Go Team Ant!",
        "He farmed his way here",
        "Here lies a programmer.  Killed by a fatal error.",
        "Here lies Bob - decided to try an acid blob",
        "Here lies Dudley, killed by another %&#@#& newt.",
        "Here lies Gregg, choked on an egg",
        "Here lies Lies. It's True",
        "Here lies The Lady's maid, died of a Vorpal Blade",
        "Here lies the left foot of Jack, killed by a land mine.  Let us know if you find any more of him",
        "He waited too long",
        "I'd rather be sailing",
        "If a man's deeds do not outlive him, of what value is a mark in stone?",
        "I'm gonna make it!",
        "I took both pills!",
        "I will survive!",
        "Killed by a black dragon -- This grave is empty",
        "Let me out of here!",
        "Lookin' good, Medusa.",
        "Mrs. Smith, choked on an apple.  She left behind grieving husband, daughter, and granddaughter.",
        "Nobody believed her when she said her feet were killing her",
        "No!  I don't want to see my damn conduct!",
        "One corpse, sans head",
        "On the whole, I'd rather be in Minetown",
        "On vacation",
        "Oops.",
        "Out to Lunch",
        "SOLD",
        "Someone set us up the bomb!",
        "Take my stuff, I don't need it anymore",
        "Taking a year dead for tax reasons",
        "The reports of my demise are completely accurate",
        "(This space for sale)",
        "This was actually just a pit, but since there was a corpse, we filled it",
        "This way to the crypt",
        "Tu quoque, Brute?",
        "VACANCY",
        "Welcome!",
        "Wish you were here!",
        "Yea, it got me too",
        "You should see the other guy",
        "...and they made me engrave my own headstone too!",
        "...but the blood has stopped pumping and I am left to decay...",
        "<Expletive Deleted>",
        "A masochist is never satisfied.",
        "Ach, 'twas a wee monster in the loch",
        "Adapt.  Enjoy.  Survive.",
        "Adventure, hah!  Excitement, hah!",
        "After all, what are friends for...",
        "After this, nothing will shock me",
        "After three days, fish and guests stink",
        "Age and treachery will always overcome youth and skill",
        "Ageing is not so bad.  The real killer is when you stop.",
        "Ain't I a stinker?",
        "Algernon",
        "All else failed...",
        "All hail RNG",
        "All right, we'll call it a draw!",
        "All's well that end well",
        "Alone at last!",
        "Always attack a floating eye from behind!",
        "Am I having fun yet?",
        "And I can still crawl, I'm not dead yet!",
        "And all I wanted was a free lunch",
        "And all of the signs were right there on your face",
        "And don't give me that innocent look either!",
        "And everyone died.  Boo hoo hoo.",
        "And here I go again...",
        "And nobody cares until somebody famous dies...",
        "And so it ends?",
        "And so... it begins.",
        "And sometimes the bear eats you.",
        "And then 'e nailed me 'ead to the floor!",
        "And they said it couldn't be done!",
        "And what do I look like?  The living?",
        "And yes, it was ALL his fault!",
        "And you said it was pretty here...",
        "Another lost soul",
        "Any day above ground is a good day!",
        "Any more of this and I'll die of a stroke before I'm 30.",
        "Anybody seen my head?",
        "Anyone for deathmatch?",
        "Anything for a change.",
        "Anything that kills you makes you ... well, dead",
        "Anything worth doing is worth overdoing.",
        "Are unicorns supposedly peaceful if you're a virgin?  Hah!",
        "Are we all being disintegrated, or is it just me?",
        "At least I'm good at something",
        "Attempted suicide",
	"Auri sacra fames",
        "Auribus teneo lupum",
        "Be prepared",
        "Beauty survives",
        "Been Here. Now Gone. Had a Good Time.",
        "Been through Hell, eh?  What did you bring me?",
        "Beg your pardon, didn't recognize you, I've changed a lot.",
        "Being dead builds character",
        "Beloved daughter, a treasure, buried here.",
        "Best friends come and go...  Mine just die.",
        "Better be dead than a fat slave",
        "Better luck next time",
        "Beware of Discordians bearing answers",
        "Beware the ...",
        "Bloody Hell...",
        "Bloody barbarians!",
        "Blown upward out of sight: He sought the leak by candlelight",
        "Brains... Brains... Fresh human brains...",
        "Buried the cat.  Took an hour.  Damn thing kept fighting.",
        "But I disarmed the trap!",
        "CONNECT 1964 - NO CARRIER 1994",
        "Call me if you need my phone number!",
        "Can YOU fly?",
        "Can you believe that thing is STILL moving?",
        "Can you come up with some better ending for this?",
        "Can you feel anything when I do this?",
        "Can you give me mouth to mouth, you just took my breath away.",
        "Can't I just have a LITTLE peril?",
        "Can't eat, can't sleep, had to bury the husband here.",
        "Can't you hit me?!",
        "Chaos, panic and disorder.  My work here is done.",
        "Check enclosed.",
        "Check this out!  It's my brain!",
        "Chivalry is only reasonably dead",
        "Coffin for sale.  Lifetime guarantee.",
        "Come Monday, I'll be all right.",
        "Come and see the violence inherent in the system",
        "Come back here!  I'll bite your bloody knees off!",
        "Commodore Business Machines, Inc.   Died for our sins.",
        "Complain to one who can help you",
        "Confess my sins to god?  Which one?",
        "Confusion will be my epitaph",
        "Cooties?  Ain't no cooties on me!",
        "Could somebody get this noose off me?",
        "Could you check again?  My name MUST be there.",
        "Could you please take a breath mint?",
        "Couldn't I be sedated for this?",
        "Courage is looking at your setbacks with serenity",
        "Cover me, I'm going in!",
        "Crash course in brain surgery",
        "Cross my fingers for me.",
        "Curse god and die",
        "Cut to fit",
        "De'Ath",
        "Dead Again?  Pardon me for not getting it right the first time!",
        "Dead and loving every moment!",
        "Dear wife of mine. Died of a broken heart, after I took it out of her.",
        "Don't tread on me!",
        "Dragon? What dragon?",
        "Drawn and quartered",
        "Either I'm dead or my watch has stopped.",
        "Eliza -- Was I really alive, or did I just think I was?",
        "Elvis",
        "Enter not into the path of the wicked",
        "Eris?  I don't need Eris",
        "Eternal Damnation, Come and stay a long while!",
        "Even The Dead pay taxes (and they aren't Grateful).",
        "Even a tomb stone will say good things when you're down!",
        "Ever notice that live is evil backwards?",
        "Every day is starting to look like Monday",
        "Every day, in every way, I am getting better and better.",
        "Every survival kit should include a sense of humor",
        "Evil I did dwell;  lewd did I live",
        "Ex post fucto",
        "Excellent day to have a rotten day.",
        "Excuse me for not standing up.",
        "Experience isn't everything. First, You've got to survive",
        "First shalt thou pull out the Holy Pin",
        "For a Breath, I Tarry...",
        "For recreational use only.",
        "For sale: One soul, slightly used. Asking for 3 wishes.",
        "For some moments in life, there are no words.",
        "Forget Disney World, I'm going to Hell!",
        "Forget about the dog, Beware of my wife.",
        "Funeral - Real fun.",
        "Gawd, it's depressing in here, isn't it?",
        "Genuine Exploding Gravestone.  (c)Acme Gravestones Inc.",
        "Get back here!  I'm not finished yet...",
        "Go ahead, I dare you to!",
        "Go ahead, it's either you or him.",
        "Goldilocks -- This casket is just right",
        "Gone But Not Forgotten",
        "Gone Underground For Good",
        "Gone away owin' more than he could pay.",
        "Gone, but not forgiven",
        "Got a life. Didn't know what to do with it.",
        "Grave?  But I was cremated!",
        "Greetings from Hell - Wish you were here.",
        "HELP! It's dark in here... Oh, my eyes are closed - sorry",
        "Ha! I NEVER pay income tax!",
        "Have you come to raise the dead?",
        "Having a good time can be deadly.",
        "Having a great time. Where am I exactly??",
        "He died of the flux.",
        "He died today... May we rest in peace!",
        "He got the upside, I got the downside.",
        "He lost his face when he was beheaded.",
        "He missed me first.",
        "He's not dead, he just smells that way.",
        "Help! I've fallen and I can't get up!",
        "Help, I can't wake up!",
        "Here lies Pinocchio",
        "Here lies the body of John Round. Lost at sea and never found.",
        "Here there be dragons",
        "Hey, I didn't write this stuff!",
	"Hodie mihi, cras tibi",
        "Hold my calls",
        "Home Sweet Hell",
        "Humpty Dumpty, a Bad Egg.  He was pushed off the wall.",
        "I KNEW this would happen if I lived long enough.",
        "I TOLD you I was sick!",
        "I ain't broke but I am badly bent.",
        "I ain't old. I'm chronologically advantaged.",
        "I am NOT a vampire. I just like to bite..nibble, really!",
        "I am here. Wish you were fine.",
        "I am not dead yet, but watch for further reports.",
        "I believe them bones are me.",
        "I broke his brain.",
        "I can feel it.  My mind.  It's going.  I can feel it.",
        "I can't go to Hell. They're afraid I'm gonna take over!",
        "I can't go to hell, they don't want me.",
        "I didn't believe in reincarnation the last time, either.",
        "I didn't mean it when I said 'Bite me'",
        "I died laughing",
        "I disbelieved in reincarnation in my last life, too.",
        "I hacked myself to death",
        "I have all the time in the world",
        "I knew I'd find a use for this gravestone!",
        "I know my mind. And it's around here someplace.",
        "I lied!  I'll never be alright!",
        "I like it better in the dark.",
        "I like to be here when I can.",
        "I may rise but I refuse to shine.",
        "I never get any either.",
        "I said hit HIM with the fireball, not me!",
        "I told you I would never say goodbye.",
        "I used to be amusing. Now I'm just disgusting.",
        "I used up all my sick days, so now I'm calling in dead.",
        "I was killed by <illegible scrawl>",
        "I was somebody. Who, is no business of yours.",
        "I will not go quietly.",
        "I'd give you a piece of my mind... but I can't find it.",
        "I'd rather be breathing",
        "I'll be back!",
        "I'll be mellow when I'm dead. For now, let's PARTY!",
        "I'm doing this only for tax purposes.",
        "I'm not afraid of Death!  What's he gonna do? Kill me?",
        "I'm not getting enough money, so I'm not going to engrave anything useful here.",
        "I'm not saying anything.",
        "I'm weeth stupeed --->",
        "If you thought you had problems...",
        "Ignorance kills daily.",
        "Ignore me... I'm just here for my looks!",
        "Ilene Toofar -- Fell off a cliff",
        "Is that all?",
        "Is there life before Death?",
        "Is this a joke, or a grave matter?",
        "It happens sometimes. People just explode.",
        "It must be Thursday. I never could get the hang of Thursdays.",
        "It wasn't a fair fight",
        "It wasn't so easy.",
        "It's Loot, Pillage and THEN Burn...",
        "Just doing my job here",
        "Killed by diarrhea of mouth and constipation of brain.",
        "Let her RIP",
        "Let it be; I am dead.",
        "Let's play Hide the Corpse",
        "Life is NOT a dream",
        "Madge Ination -- It wasn't all in my head",
        "Meet me in Heaven",
        "Move on, there's nothing to see here.",
        "Mr. Flintstone -- Yabba-dabba-done",
        "My heart is not in this",
        "No one ever died from it",
        "No, you want room 12A, next door.",
        "Nope.  No trap on that chest.  I swear.",
        "Not again!",
        "Not every soil can bear all things",
        "Now I have a life",
        "Now I lay thee down to sleep... wanna join me?",
        "OK, here is a question: Where ARE your tanlines?",
        "Obesa Cantavit",
        "Oh! An untimely death.",
        "Oh, by the way, how was my funeral?",
        "Oh, honey..I missed you! She said, and fired again.",
        "Ok, so the light does go off. Now let me out of here.",
        "One stone brain",
        "Ooh! Somebody STOP me!",
        "Oops!",
        "Out for the night.  Leave a message.",
        "Ow!  Do that again!",
        "Pardon my dust.",
        "Part of me still works.",
        "Please, not in front of those orcs!",
        "Prepare to meet me in Heaven",
        "R2D2 -- Rest, Tin Piece",
        "Relax.  Nothing ever happens on the first level.",
        "Res omnia mea culpa est",
        "Rest In Pieces",
        "Rest, rest, perturbed spirit.",
        "Rip Torn",
        "She always said her feet were killing her but nobody believed her.",
        "She died of a chest cold.",
        "So let it be written, so let it be done!",
        "So then I says, How do I know you're the real angel of death?",
        "Some patients insist on dying.",
        "Some people have it dead easy, don't they?",
        "Some things are better left buried.",
        "Sure, trust me, I'm a lawyer...",
        "Thank God I wore my corset, because I think my sides have split.",
        "That is all",
        "The Gods DO have a sense of humor: I'm living proof!",
        "The frog's dead. He Kermitted suicide.",
        "This dungeon is a pushover",
        "This elevator doesn't go to Heaven",
        "This gravestone is shareware. To register, please send me 10 zorkmids",
        "This gravestone provided by The Yendorian Grave Services Inc.",
        "This is not an important part of my life.",
        "This one's on me.",
        "This side up",
        "Tim Burr -- Smashed by a tree",
        "Tone it down a bit, I'm trying to get some rest here.",
        "Virtually Alive",
        "We Will Meet Again.",
        "Weep not, he is at rest",
        "Welcome to Dante's.  What level please?",
        "Well, at least they listened to my sermon...",
        "Went to be an angel.",
        "What are you doing over there?",
        "What are you smiling at?",
        "What can you say, Death's got appeal...!",
        "What health care?",
        "What pit?",
        "When the gods want to punish you, they answer your prayers.",
        "Where e'er you be let your wind go free. Keeping it in was the death of me!",
        "Where's my refund?",
        "Will let you know for sure in a day or two...",
        "Wizards are wimps",
        "Worms at work, do not disturb!",
        "Would you mind moving a bit?  I'm short of breath down here.",
        "Would you quit being evil over my shoulder?",
        "Ya really had me going baby, but now I'm gone.",
        "Yes Dear, just a few more minutes...",
        "You said it wasn't poisonous!",
        "You set my heart aflame. You gave me heartburn."
};

/* Create a headstone at the given location.
 * The caller is responsible for newsym(x, y).
 */
void
make_grave(x, y, str)
int x, y;
const char *str;
{
	/* Can we put a grave here? */
	if ((levl[x][y].typ != ROOM && levl[x][y].typ != GRAVE) || t_at(x,y)) return;

	/* Make the grave */
	levl[x][y].typ = GRAVE;

	/* Engrave the headstone */
	if (!str) str = epitaphs[rn2(SIZE(epitaphs))];
	del_engr_at(x, y);
	make_engr_at(x, y, str, 0L, HEADSTONE);
	return;
}


#endif /* OVLB */

/*engrave.c*/
