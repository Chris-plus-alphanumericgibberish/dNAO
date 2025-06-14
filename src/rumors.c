/*	SCCS Id: @(#)rumors.c	3.4	1996/04/20	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "lev.h"
#include "dlb.h"
#include "artifact.h"


/*	[note: this comment is fairly old, but still accurate for 3.1]
 * Rumors have been entirely rewritten to speed up the access.  This is
 * essential when working from floppies.  Using fseek() the way that's done
 * here means rumors following longer rumors are output more often than those
 * following shorter rumors.  Also, you may see the same rumor more than once
 * in a particular game (although the odds are highly against it), but
 * this also happens with real fortune cookies.  -dgk
 */

/*	3.1
 * The rumors file consists of a "do not edit" line, a hexadecimal number
 * giving the number of bytes of useful/true rumors, followed by those
 * true rumors (one per line), followed by the useless/false/misleading/cute
 * rumors (also one per line).  Number of bytes of untrue rumors is derived
 * via fseek(EOF)+ftell().
 *
 * The oracles file consists of a "do not edit" comment, a decimal count N
 * and set of N+1 hexadecimal fseek offsets, followed by N multiple-line
 * records, separated by "---" lines.  The first oracle is a special case,
 * and placed there by 'makedefs'.
 */

STATIC_DCL void FDECL(init_rumors, (dlb *));
STATIC_DCL void FDECL(init_oracles, (dlb *));

static long true_rumor_start,  true_rumor_size,  true_rumor_end,
	    false_rumor_start, false_rumor_size, false_rumor_end;
static int oracle_flg = 0;  /* -1=>don't use, 0=>need init, 1=>init done */
static unsigned oracle_cnt = 0;
static long *oracle_loc = 0;

STATIC_OVL void
init_rumors(fp)
dlb *fp;
{
	char line[BUFSZ];

	(void) dlb_fgets(line, sizeof line, fp); /* skip "don't edit" comment */
	(void) dlb_fgets(line, sizeof line, fp);
	if (sscanf(line, "%6lx\n", &true_rumor_size) == 1 &&
	    true_rumor_size > 0L) {
	    (void) dlb_fseek(fp, 0L, SEEK_CUR);
	    true_rumor_start  = dlb_ftell(fp);
	    true_rumor_end    = true_rumor_start + true_rumor_size;
	    (void) dlb_fseek(fp, 0L, SEEK_END);
	    false_rumor_end   = dlb_ftell(fp);
	    false_rumor_start = true_rumor_end;	/* ok, so it's redundant... */
	    false_rumor_size  = false_rumor_end - false_rumor_start;
	} else
	    true_rumor_size = -1L;	/* init failed */
}

/* exclude_cookie is a hack used because we sometimes want to get rumors in a
 * context where messages such as "You swallowed the fortune!" that refer to
 * cookies should not appear.  This has no effect for true rumors since none
 * of them contain such references anyway.
 */
char *
getrumor(truth, rumor_buf, exclude_cookie)
int truth; /* 1=true, -1=false, 0=either */
char *rumor_buf;
boolean exclude_cookie; 
{
	dlb	*rumors;
	long tidbit, beginning;
	char	*endp, line[BUFSZ], xbuf[BUFSZ];

	rumor_buf[0] = '\0';
	if (true_rumor_size < 0L)	/* we couldn't open RUMORFILE */
		return rumor_buf;

	rumors = dlb_fopen(RUMORFILE, "r");

	if (rumors) {
	    int count = 0;
	    int adjtruth;

	    do {
		rumor_buf[0] = '\0';
		if (true_rumor_size == 0L) {	/* if this is 1st outrumor() */
		    init_rumors(rumors);
		    if (true_rumor_size < 0L) {	/* init failed */
			Sprintf(rumor_buf, "Error reading \"%.80s\".",
				RUMORFILE);
			return rumor_buf;
		    }
		}
		/*
		 *	input:      1    0   -1
		 *	 rn2 \ +1  2=T  1=T  0=F
		 *	 adj./ +0  1=T  0=F -1=F
		 */
		switch (adjtruth = truth + rn2(2)) {
		  case  2:	/*(might let a bogus input arg sneak thru)*/
		  case  1:  beginning = true_rumor_start;
			    tidbit = Rand() % true_rumor_size;
			break;
		  case  0:	/* once here, 0 => false rather than "either"*/
		  case -1:  beginning = false_rumor_start;
			    tidbit = Rand() % false_rumor_size;
			break;
		  default:
			    impossible("strange truth value for rumor");
			return strcpy(rumor_buf, "Oops...");
		}
		(void) dlb_fseek(rumors, beginning + tidbit, SEEK_SET);
		(void) dlb_fgets(line, sizeof line, rumors);
		if (!dlb_fgets(line, sizeof line, rumors) ||
		    (adjtruth > 0 && dlb_ftell(rumors) > true_rumor_end)) {
			/* reached end of rumors -- go back to beginning */
			(void) dlb_fseek(rumors, beginning, SEEK_SET);
			(void) dlb_fgets(line, sizeof line, rumors);
		}
		if ((endp = index(line, '\n')) != 0) *endp = 0;
		Strcat(rumor_buf, xcrypt(line, xbuf));
	    } while(count++ < 50 && exclude_cookie && (strstri(rumor_buf, "fortune") || strstri(rumor_buf, "pity")));
	    (void) dlb_fclose(rumors);
	    if (count >= 50)
		impossible("Can't find non-cookie rumor?");
	    else
		exercise(A_WIS, (adjtruth > 0));
	} else {
		pline("Can't open rumors file!");
		true_rumor_size = -1;	/* don't try to open it again */
	}
	return rumor_buf;
}

void
outrumor(truth, mechanism)
int truth; /* 1=true, -1=false, 0=either */
int mechanism;
{
	static const char fortune_msg[] =
		"This cookie has a scrap of paper inside.";
	const char *line;
	char buf[BUFSZ];
	boolean reading = (mechanism == BY_COOKIE ||
			   mechanism == BY_PAPER);

	if (reading) {
	    /* deal with various things that prevent reading */
	    if (is_fainted() && mechanism == BY_COOKIE)
	    	return;
	    else if (Blind) {
		if (mechanism == BY_COOKIE)
			pline(fortune_msg);
		pline("What a pity that you cannot read it!");
	    	return;
	    }
	}
	line = getrumor(truth, buf, reading ? FALSE : TRUE);
	if (!*line)
		line = "NetHack rumors file closed for renovation.";
	switch (mechanism) {
	    case BY_ORACLE:
	 	/* Oracle delivers the rumor */
		pline("True to her word, the Oracle %ssays: ",
		  (!rn2(4) ? "offhandedly " : (!rn2(3) ? "casually " :
		  (rn2(2) ? "nonchalantly " : ""))));
		verbalize("%s", line);
		exercise(A_WIS, TRUE);
		return;
	    case BY_COOKIE:
		pline(fortune_msg);
		/* FALLTHRU */
	    case BY_PAPER:
		pline("It reads:");
		break;
	}
	pline("%s", line);
}

STATIC_OVL void
init_oracles(fp)
dlb *fp;
{
	register int i;
	char line[BUFSZ];
	int cnt = 0;

	/* this assumes we're only called once */
	(void) dlb_fgets(line, sizeof line, fp); /* skip "don't edit" comment*/
	(void) dlb_fgets(line, sizeof line, fp);
	if (sscanf(line, "%5d\n", &cnt) == 1 && cnt > 0) {
	    oracle_cnt = (unsigned) cnt;
	    oracle_loc = (long *) alloc((unsigned)cnt * sizeof (long));
	    for (i = 0; i < cnt; i++) {
		(void) dlb_fgets(line, sizeof line, fp);
		(void) sscanf(line, "%5lx\n", &oracle_loc[i]);
	    }
	}
	return;
}

void
save_oracles(fd, mode)
int fd, mode;
{
	if (perform_bwrite(mode)) {
	    bwrite(fd, (genericptr_t) &oracle_cnt, sizeof oracle_cnt);
	    if (oracle_cnt)
		bwrite(fd, (genericptr_t)oracle_loc, oracle_cnt*sizeof (long));
	}
	if (release_data(mode)) {
	    if (oracle_cnt) {
		free((genericptr_t)oracle_loc);
		oracle_loc = 0,  oracle_cnt = 0,  oracle_flg = 0;
	    }
	}
}

void
restore_oracles(fd)
int fd;
{
	mread(fd, (genericptr_t) &oracle_cnt, sizeof oracle_cnt);
	if (oracle_cnt) {
	    oracle_loc = (long *) alloc(oracle_cnt * sizeof (long));
	    mread(fd, (genericptr_t) oracle_loc, oracle_cnt * sizeof (long));
	    oracle_flg = 1;	/* no need to call init_oracles() */
	}
}

void
outoracle(special, delphi)
boolean special;
boolean delphi;
{
	char	line[COLNO];
	char	*endp;
	dlb	*oracles;
	int oracle_idx;
	char xbuf[BUFSZ];

	if(oracle_flg < 0 ||			/* couldn't open ORACLEFILE */
	   (oracle_flg > 0 && oracle_cnt == 0))	/* oracles already exhausted */
		return;

	oracles = dlb_fopen(ORACLEFILE, "r");

	if (oracles) {
		winid tmpwin;
		if (oracle_flg == 0) {	/* if this is the first outoracle() */
			init_oracles(oracles);
			oracle_flg = 1;
			if (oracle_cnt == 0) return;
		}
		/* oracle_loc[0] is the special oracle;		*/
		/* oracle_loc[1..oracle_cnt-1] are normal ones	*/
		if (oracle_cnt <= 1 && !special) return;  /*(shouldn't happen)*/
		oracle_idx = special ? 0 : rnd((int) oracle_cnt - 1);
		(void) dlb_fseek(oracles, oracle_loc[oracle_idx], SEEK_SET);
		if (!special) oracle_loc[oracle_idx] = oracle_loc[--oracle_cnt];

		tmpwin = create_nhwindow(NHW_TEXT);
		if (delphi)
		    putstr(tmpwin, 0, special ?
		          "The Oracle scornfully takes all your money and says:" :
		          "The Oracle meditates for a moment and then intones:");
		else
		    putstr(tmpwin, 0, "The message reads:");
		putstr(tmpwin, 0, "");

		while(dlb_fgets(line, COLNO, oracles) && strcmp(line,"---\n")) {
			if ((endp = index(line, '\n')) != 0) *endp = 0;
			putstr(tmpwin, 0, xcrypt(line, xbuf));
		}
		display_nhwindow(tmpwin, TRUE);
		destroy_nhwindow(tmpwin);
		(void) dlb_fclose(oracles);
	} else {
		pline("Can't open oracles file!");
		oracle_flg = -1;	/* don't try to open it again */
	}
}

void
outgmaster()
{
	char	line[COLNO];
	char	*endp;
	dlb	*oracles;
	int oracle_idx;
	char xbuf[BUFSZ];

	if(oracle_flg < 0 ||			/* couldn't open ORACLEFILE */
	   (oracle_flg > 0 && oracle_cnt == 0))	/* oracles already exhausted */
		return;

	oracles = dlb_fopen(ORACLEFILE, "r");
	if (oracles) {
		winid tmpwin;
		if (oracle_flg == 0) {	/* if this is the first outoracle() */
			init_oracles(oracles);
			oracle_flg = 1;
			if (oracle_cnt == 0) return;
		}
		/* oracle_loc[0] is the special oracle;		*/
		/* oracle_loc[1..oracle_cnt-1] are normal ones	*/
		if (oracle_cnt <= 1) return;  /*(shouldn't happen?)*/
		oracle_idx = rnd((int) oracle_cnt - 1);
		(void) dlb_fseek(oracles, oracle_loc[oracle_idx], SEEK_SET);
		oracle_loc[oracle_idx] = oracle_loc[--oracle_cnt];

		tmpwin = create_nhwindow(NHW_TEXT);
		putstr(tmpwin, 0, "");

		while(dlb_fgets(line, COLNO, oracles) && strcmp(line,"---\n")) {
			if ((endp = index(line, '\n')) != 0) *endp = 0;
			putstr(tmpwin, 0, xcrypt(line, xbuf));
		}
		display_nhwindow(tmpwin, TRUE);
		destroy_nhwindow(tmpwin);
		(void) dlb_fclose(oracles);
	} else {
		pline("Can't open oracles file!");
		oracle_flg = -1;	/* don't try to open it again */
	}
}

#define SELECT_MINOR 1
#define SELECT_MAJOR 2
#define SELECT_ENLIT 3
#define SELECT_HINTS 4

#define GLIMPSE_ELDRN 1
#define GLIMPSE_DRAGN 2
#define GLIMPSE_POLYP 3
#define GLIPMSE_CHAOS 4
#define GLIMPSE_OOONA 5
#define GLIMPSE_ABYSS 6
#define GLIMPSE_HELLL 7
#define GLIMPSE_ARTIS 8

#define arti_selector(c) (('a' + c%52 > 'z') ? 'A' + ((c%52) - 26) : 'a' + c % 52)

int
doconsult(oracl)
register struct monst *oracl;
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	menu_item *selected;
	anything any;
#ifdef GOLDOBJ
	long umoney = money_cnt(invent);
#else
	long umoney = u.ugold;
#endif
	int minor_cost = 50;
	int major_cost = 500 + 50 * u.ulevel;
	int enl_cost = 200 + 20 * u.ulevel;
	int hint_cost = 150;

	if (!oracl) {
		There("is no one here to consult.");
		return 0;
	} else if (!oracl->mpeaceful) {
		pline("%s is in no mood for consultations.", Monnam(oracl));
		return 0;
	} else if (umoney == 0) {
		You("have no money.");
		return 0;
	}

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "A minor consultation");
	any.a_int = SELECT_MINOR;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		'm', 0, ATR_NONE, buf,
		MENU_UNSELECTED);

	Sprintf(buf, "A major consultation");
	any.a_int = SELECT_MAJOR;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		'M', 0, ATR_NONE, buf,
		MENU_UNSELECTED);

	Sprintf(buf, "After enlightenment");
	any.a_int = SELECT_ENLIT;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		'e', 0, ATR_NONE, buf,
		MENU_UNSELECTED);

	Sprintf(buf, "Glimpses of things to come");
	any.a_int = SELECT_HINTS;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		'g', 0, ATR_NONE, buf,
		MENU_UNSELECTED);

	end_menu(tmpwin, "What dost thou seek?");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if(n > 0){
		n = selected[0].item.a_int;
		free(selected);
	}
	else
		n = 0;

	switch (n){
		case SELECT_MINOR:
			if (umoney < minor_cost){
				You("don't have enough money for that!");
				return 0;
			} else {
#ifndef GOLDOBJ
				u.ugold -= (long)minor_cost;
				oracl->mgold += (long)minor_cost;
#else
				money2mon(oracl, (long)minor_cost);
#endif
				outrumor(1, BY_ORACLE);
				if (!u.uevent.minor_oracle){
					more_experienced(minor_cost / (u.uevent.major_oracle ? 25 : 10), 0);
					newexplevel();
					/* 5 pts if very 1st, or 2 pts if major already done */
					if(!u.uevent.major_oracle) livelog_write_string("consulted the oracle for the first time");
				}
				u.uevent.minor_oracle = TRUE;
			}
		break;
		case SELECT_MAJOR:
			if (umoney < (major_cost / 2)){
				You("don't have enough money for that!");
				return 0;
			} else {
#ifndef GOLDOBJ
				u.ugold -= (long)min(major_cost, umoney);
				oracl->mgold += (long)min(major_cost, umoney);
#else
				money2mon(oracl, (long)min(major_cost, umoney));
#endif
				boolean cheapskate = umoney < major_cost;
				outoracle(cheapskate, TRUE);
				if (!cheapskate && !u.uevent.major_oracle){
					more_experienced(major_cost / (u.uevent.major_oracle ? 25 : 10), 0);
					newexplevel();
					/* ~100 pts if very 1st, ~40 pts if minor already done */
					if(!u.uevent.minor_oracle) livelog_write_string("consulted the oracle for the first time");
				}
				u.uevent.major_oracle = TRUE;
			}
		break;
		case SELECT_ENLIT:
			if (umoney < enl_cost){
				You("don't have enough money for that!");
				return 0;
			} else {
#ifndef GOLDOBJ
				u.ugold -= (long)enl_cost;
				oracl->mgold += (long)enl_cost;
#else
				money2mon(oracl, (long)enl_cost);
#endif
				You_feel("self-knowledgeable...");
				display_nhwindow(WIN_MESSAGE, FALSE);
				doenlightenment();
				pline_The("feeling subsides.");
				if (!u.uevent.major_oracle){
					more_experienced(enl_cost / (u.uevent.major_oracle ? 25 : 10), 0);
					newexplevel();
					/* 5 pts if very 1st, or 2 pts if major already done */
					if(!u.uevent.major_oracle) livelog_write_string("consulted the oracle for the first time");
				}
				u.uevent.major_oracle = TRUE;
			}
		break;
		case SELECT_HINTS:
			if (umoney < hint_cost){
				You("don't have enough money for that!");
				return 0;
			} else {
#ifndef GOLDOBJ
				u.ugold -= (long)hint_cost;
				oracl->mgold += (long)hint_cost;
#else
				money2mon(oracl, (long)hint_cost);
#endif
				tmpwin = create_nhwindow(NHW_MENU);
				start_menu(tmpwin);
				any.a_void = 0;		/* zero out all bits */

				Sprintf(buf, "Sightings of the eladrin nobility");
				any.a_int = GLIMPSE_ELDRN;	/* must be non-zero */
				add_menu(tmpwin, NO_GLYPH, &any,
					's', 0, ATR_NONE, buf,
					MENU_UNSELECTED);

				Sprintf(buf, "Talk of the castle in the air");
				any.a_int = GLIMPSE_DRAGN;	/* must be non-zero */
				add_menu(tmpwin, NO_GLYPH, &any,
					't', 0, ATR_NONE, buf,
					MENU_UNSELECTED);

				Sprintf(buf, "The location of the primordial ones");
				any.a_int = GLIMPSE_POLYP;	/* must be non-zero */
				add_menu(tmpwin, NO_GLYPH, &any,
					'l', 0, ATR_NONE, buf,
					MENU_UNSELECTED);

				Sprintf(buf, "Knowledge of those who came before");
				any.a_int = GLIPMSE_CHAOS;	/* must be non-zero */
				add_menu(tmpwin, NO_GLYPH, &any,
					'k', 0, ATR_NONE, buf,
					MENU_UNSELECTED);

				Sprintf(buf, "Impressions of the Queen of the Fae");
				any.a_int = GLIMPSE_OOONA;	/* must be non-zero */
				add_menu(tmpwin, NO_GLYPH, &any,
					'i', 0, ATR_NONE, buf,
					MENU_UNSELECTED);

				Sprintf(buf, "Knowledge of the Layers of the Abyss");
				any.a_int = GLIMPSE_ABYSS;	/* must be non-zero */
				add_menu(tmpwin, NO_GLYPH, &any,
					'a', 0, ATR_NONE, buf,
					MENU_UNSELECTED);

				Sprintf(buf, "Knowledge of the Lords of the Nine Hells");
				any.a_int = GLIMPSE_HELLL;	/* must be non-zero */
				add_menu(tmpwin, NO_GLYPH, &any,
					'h', 0, ATR_NONE, buf,
					MENU_UNSELECTED);

				Sprintf(buf, "Knowledge of artifact birthplaces");
				any.a_int = GLIMPSE_ARTIS;	/* must be non-zero */
				add_menu(tmpwin, NO_GLYPH, &any,
					'A', 0, ATR_NONE, buf,
					MENU_UNSELECTED);

				end_menu(tmpwin, "What glimpses dost thou ask for?");

				how = PICK_ONE;
				n = select_menu(tmpwin, how, &selected);
				destroy_nhwindow(tmpwin);
				if(n > 0){
					n = selected[0].item.a_int;
					free(selected);
				}
				else n = rnd(8);
				switch (n){
					case GLIMPSE_ELDRN:
						switch(dungeon_topology.alt_tulani){
							case TULANI_CASTE:
								pline("They say radiant spheres roam the land.");
							break;
							case GAE_CASTE:
								pline("They say %s spirits roam the land.",
										!rn2(4) ? "aestival" : (!rn2(3) ? "vernal" :
											(!rn2(2) ? "hibernal" : "autumnal")));
							break;
							case BRIGHID_CASTE:
								pline("They say the deep nobility has emerged to roam the land.");
							break;
							case UISCERRE_CASTE:
								pline("They say the deep nobility has surfaced to roam the land.");
							break;
							case CAILLEA_CASTE:
								pline("They say the winter witches have come down from the mountains.");
							break;
						}
					break;
					case GLIMPSE_DRAGN:
						if (dungeon_topology.alt_tower)
							pline("They say Bahamut's palace has been sighted from afar.");
						else
							pline("Rumors of Bahamut's palace have been greatly exaggerated.");
					break;
					case GLIMPSE_POLYP:
						if (dungeon_topology.eprecursor_typ == PRE_DRACAE)
							pline("They say the ancient eladrin mothers have been seen once again.");
						else if (dungeon_topology.eprecursor_typ == PRE_POLYP){
							if (dungeon_topology.d_chaos_dvariant == MITHARDIR)
								pline("There's been rumors of invisible shapeshifters in the deserts.");
							else
								pline("There's been rumors of invisible shapeshifters in the forests.");
						} else
							pline("I've been considering expanding my statue collection. I hear Oona has quite the variety...");
					break;
					case GLIPMSE_CHAOS:
						if (dungeon_topology.d_chaos_dvariant == TEMPLE_OF_CHAOS)
							pline("Some adventurer came through here the other day, all dressed in blue and muttering about 'Materia'?");
						else if (dungeon_topology.d_chaos_dvariant == MITHARDIR)
							pline("Some adventurer came through here the other day, wearing the oddest white headscarf.");
						else if (dungeon_topology.d_chaos_dvariant == MORDOR){
							if (!rn2(4))
								pline("A pair of short adventurers came through here just the other day... there was an odd wretched creature following them though.");
							else
								pline("A pair of short adventurers came through here the other day, apparently in search of a volcano?");
						}
					break;
					case GLIMPSE_OOONA:
						if (u.oonaenergy == AD_FIRE)
							pline("They say Oona has a bit of a fiery personality...");
						else if (u.oonaenergy == AD_COLD)
							pline("They say Oona can be a bit cold at first...");
						else if (u.oonaenergy == AD_ELEC)
							pline("They say meeting Oona can be a bit of a shock...");
					break;
					case GLIMPSE_ABYSS:
						if (!rn2(3)){
							switch (dungeon_topology.brine_variant){ // demo, dagon, lamashtu
								case DEMOGORGON_LEVEL:
									pline("They say that a closed drawbridge should be left well enough alone.");
								break;
								case DAGON_LEVEL:
									pline("They say that the darkest depths hide the most dangerous foes.");
								break;
								case LAMASHTU_LEVEL:
									pline("They say that the depths of the Abyss can drive even the brightest lights to madness.");
								break;
								break;
									pline("unknown or un-initialized hell1");
								break;
							}
						} else if (rn2(2)) {
							switch (dungeon_topology.abyss_variant){ // juib, zugg, yeen, baph, pale night, kostch
								case JUIBLEX_LEVEL:
									pline("They say the Father of Slimes will always save his children from calamity.");
								break;
								case ZUGGTMOY_LEVEL:
									if (Hallucination || \
										(!ClearThoughts && u.umadness&MAD_SPORES && !Race_if(PM_ANDROID) && !Race_if(PM_CLOCKWORK_AUTOMATON))
										)
											pline("Can you feel your heart burning? Can you feel the struggle within?");
									else pline("They say that even decay itself is a form of life.");
								break;
								case YEENOGHU_LEVEL:
									pline("They say that even the most savage of butchers have those they bow to.");
								break;
								case BAPHOMET_LEVEL:
									pline("They say that the most twisted labyrinths trap the angriest denizens.");
								break;
								case NIGHT_LEVEL:
									pline("They say that sometimes, it's best to not look beyond the veil.");
								break;
								case KOSTCH_LEVEL:
									pline("They say that fury doesn't always burn hot.");
								break;
								default:
									pline("unknown or un-initialized hell2");
								break;
							}
						} else {
							switch (dungeon_topology.abys2_variant){ // orcus, mal, grazzt, lolth
								case ORCUS_LEVEL:
									if(u.sealsActive&SEAL_TENEBROUS) {
										if (u.ufirst_light) pline("They see that if Creation began with light, then darkness will persist at the end of all things.");
										else pline("They say that the Word that echoes around you lives on.");
									}
									else pline("They say that death is only a temporary setback to some denizens of the Abyss.");
								break;
								case MALCANTHET_LEVEL:
									pline("They say that despite its looks, paradise is only paradise to some.");
								break;
								case GRAZ_ZT_LEVEL:
									pline("They say that the most self indulgent tend to be jealous lovers.");
								break;
								case LOLTH_LEVEL:
									if (u.ualign.god == GOD_LOLTH) pline("The symbol you bear has been more common around here lately.");
									else pline("They say that not only demons haunt the depths of the Abyss, but divine power as well.");
								break;
									pline("unknown or un-initialized hell1");
								break;
							}
						}
					break;
					case GLIMPSE_HELLL:
						if (rn2(2)){
							switch (dungeon_topology.hell1_variant){ // bael, dis, mammon, belial
								case BAEL_LEVEL:
									pline("They say that not only minotaurs can be found at the centers of mazes.");
								break;
								case CHROMA_LEVEL:
									pline("They say that if you're lucky, not only demons can be found in the Abyss.");
								break;
								case DISPATER_LEVEL:
									pline("They say that the Iron City is ruled with an iron fist.");
								break;
								case MAMMON_LEVEL:
									pline("They say that sometimes, the filthiest muck hides the greatest riches.");
								break;
								case BELIAL_LEVEL:
									pline("They say the scorching heats of Gehennom have been worse recently.");
								break;
									pline("unknown or un-initialized hell1");
								break;
							}
						} else {
							switch (dungeon_topology.hell2_variant){ // levi, lilith, baalze, meph
								case LEVIATHAN_LEVEL:
									pline("They say that sometimes, it's best to leave frozen foes alone.");
								break;
								case LILITH_LEVEL:
									pline("They say that the only thing worse than an evil witch is a whole coven of them.");
								break;
								case BAALZEBUB_LEVEL:
									pline("They say that the harder they fall, the more dangerous they are.");
								break;
								case MEPHISTOPHELES_LEVEL:
									pline("They say that sculptures made of ice are sometimes more lifelike than ones made of stone.");
								break;
								default:
									pline("unknown or un-initialized hell2");
								break;
							}
						}
					break;
					case GLIMPSE_ARTIS:{
						int arti_count = 0;
						char tbuf[BUFSZ];
						int i;
						d_level lev;
						tmpwin = create_nhwindow(NHW_MENU);
						start_menu(tmpwin);
						any.a_void = 0;		/* zero out all bits */
	    					for (i = 1; artilist[i].otyp; i++){
							if(!artinstance[i].exists)
								continue;
							Sprintf(buf, "%s", artilist[i].name);
							any.a_int = i;	/* must be non-zero */
							add_menu(tmpwin, NO_GLYPH, &any,
								arti_selector(arti_count), 0, ATR_NONE, buf,
								MENU_UNSELECTED);
							arti_count++;
						}


						end_menu(tmpwin, "Know the birthplace of which artifact?");

						how = PICK_ONE;
						n = select_menu(tmpwin, how, &selected);
						destroy_nhwindow(tmpwin);
						if(n <= 0){
							free(selected);
							break;
						}
						n = selected[0].item.a_int;
						free(selected);
						lev.dnum = artinstance[n].spawn_dnum;
						lev.dlevel = artinstance[n].spawn_dlevel;
						name_by_lev(tbuf, &lev);	
						pline("%s was born on %s.", artilist[n].name, tbuf);
						
					}
					break;
					default:
						impossible("Oracle rolled a non-existent dungeon hint? :(");
					break;
				}

				if (!u.uevent.major_oracle){
					more_experienced(hint_cost / (u.uevent.major_oracle ? 25 : 10), 0);
					newexplevel();
				/* 5 pts if very 1st, or 2 pts if major already done */
					if(!u.uevent.major_oracle) livelog_write_string("consulted the oracle for the first time");
				}
				u.uevent.major_oracle = TRUE;
			}
		break;
		default:
			return 0;
		break;
	}

	flags.botl = 1;

	return 1;
}

#undef SELECT_MINOR
#undef SELECT_MAJOR
#undef SELECT_ENLIT
#undef SELECT_HINTS

#undef GLIMPSE_ELDRN
#undef GLIMPSE_DRAGN
#undef GLIMPSE_POLYP
#undef GLIPMSE_CHAOS
#undef GLIMPSE_OOONA
#undef GLIMPSE_ABYSS
#undef GLIMPSE_HELLL
#undef arti_selector
/*rumors.c*/
