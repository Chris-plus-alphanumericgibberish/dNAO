/*	SCCS Id: @(#)enlighten.c	3.4	2003/02/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_DCL void FDECL(enlght_line, (const char *,const char *,const char *, boolean));
STATIC_DCL void FDECL(put_or_dump, (const char *, boolean));
STATIC_DCL char *FDECL(enlght_combatinc, (const char *,int,int,char *));
STATIC_DCL int NDECL(minimal_enlightenment);
STATIC_DCL void NDECL(resistances_enlightenment);
STATIC_DCL void NDECL(signs_enlightenment);
STATIC_DCL void NDECL(spirits_enlightenment);

#define DOATTRIB_RESISTS	1
#define DOATTRIB_ARMOR		2
#define DOATTRIB_ENLIGHTEN	3
#define DOATTRIB_BINDINGS	4
#define DOATTRIB_SPIRITS	5

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
			enlght_line(prefix, final ? past : present, suffix, dumping)
#define put_enl(msg) put_or_dump(msg, dumping) 
#define you_are(attr)	enl_msg(You_,are,were,attr)
#define you_have(attr)	enl_msg(You_,have,had,attr)
#define you_can(attr)	enl_msg(You_,can,could,attr)
#define you_have_been(goodthing) enl_msg(You_,have_been,were,goodthing)
#define you_have_never(badthing) enl_msg(You_,have_never,never,badthing)
#define you_have_X(something)	enl_msg(You_,have,(const char *)"",something)

static void
enlght_line(start, middle, end, dumping)
const char *start, *middle, *end;
boolean dumping;
{
	char buf[BUFSZ];

	Sprintf(buf, "%s%s%s.", start, middle, end);

	put_or_dump(buf, dumping);
}

static void
put_or_dump(msg, dumping)
const char * msg;
boolean dumping;
{
#ifdef DUMP_LOG
	char buf[BUFSZ];
	Strcpy(buf, msg);
	if (dumping)
		dump("", (char *)buf);
	else
#endif /* DUMP_LOG */

	putstr(en_win, 0, msg);
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


int
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
			doenlightenment();
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
int
doconduct()
{
	show_conduct(0, FALSE);
	return 0;
}

int
doenlightenment()
{
	show_enlightenment(0, FALSE);
	return 0;
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
	Sprintf(buf, fmtstr, "alignment", align_str(galign(u.ugodbase[UGOD_ORIGINAL])));
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
	if(Infuture){
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "The God of the Future", FALSE);
		Sprintf(buf2, deity_fmtstr, godname(GOD_ILSENSINE), "");
		Sprintf(buf, fmtstr, "Mundane", buf2);
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
	}
	else {
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", FALSE);
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings, "Deities", FALSE);
		/* pantheon Chaotic */
		Sprintf(buf2, deity_fmtstr, align_gname(A_CHAOTIC),
			(u.ugodbase[UGOD_ORIGINAL] == u.ualign.god
			&& u.ualign.god == align_to_god(A_CHAOTIC)) ? " (s,c)" :
			(u.ugodbase[UGOD_ORIGINAL] == align_to_god(A_CHAOTIC))       ? " (s)" :
			(u.ualign.god == align_to_god(A_CHAOTIC))       ? " (c)" : "");
		if(Role_if(PM_EXILE) && Is_astralevel(&u.uz)) Sprintf(buf, fmtstr, "Xaotic", buf2);
		else Sprintf(buf, fmtstr, "Chaotic", buf2);
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
		/* pantheon Neutral */
		Sprintf(buf2, deity_fmtstr, align_gname(A_NEUTRAL),
			(u.ugodbase[UGOD_ORIGINAL] == u.ualign.god
			&& u.ualign.god == align_to_god(A_NEUTRAL)) ? " (s,c)" :
			(u.ugodbase[UGOD_ORIGINAL] == align_to_god(A_NEUTRAL))       ? " (s)" :
			(u.ualign.god == align_to_god(A_NEUTRAL))       ? " (c)" : "");
		if(Role_if(PM_EXILE) && Is_astralevel(&u.uz)) Sprintf(buf, fmtstr, "Gnostic", buf2);
		else Sprintf(buf, fmtstr, "Neutral", buf2);
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
		/* pantheon Lawful */
		Sprintf(buf2, deity_fmtstr, align_gname(A_LAWFUL),
			(u.ugodbase[UGOD_ORIGINAL] == u.ualign.god
			&& u.ualign.god == align_to_god(A_LAWFUL))  ? " (s,c)" :
			(u.ugodbase[UGOD_ORIGINAL] == align_to_god(A_LAWFUL))        ? " (s)" :
			(u.ualign.god == align_to_god(A_LAWFUL))        ? " (c)" : "");
		if(Role_if(PM_EXILE) && Is_astralevel(&u.uz)) Sprintf(buf, fmtstr, "Mundane", buf2);
		else Sprintf(buf, fmtstr, "Lawful", buf2);
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
		/* non-pantheon starting (only if applicable) */
		if (u.ugodbase[UGOD_ORIGINAL] != align_to_god(A_LAWFUL) &&
			u.ugodbase[UGOD_ORIGINAL] != align_to_god(A_NEUTRAL) &&
			u.ugodbase[UGOD_ORIGINAL] != align_to_god(A_CHAOTIC)
		) {
		Sprintf(buf2, deity_fmtstr, godname(u.ugodbase[UGOD_ORIGINAL]),
			(u.ugodbase[UGOD_ORIGINAL] == u.ualign.god)  ? " (s,c)" : " (s)");
		Sprintf(buf, fmtstr, align_str_proper(galign(u.ualign.god)), buf2);
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
		}
		/* non-pantheon current (only if applicable) */
		else if (u.ualign.god != align_to_god(A_LAWFUL) &&
			u.ualign.god != align_to_god(A_NEUTRAL) &&
			u.ualign.god != align_to_god(A_CHAOTIC)
		) {
		Sprintf(buf2, deity_fmtstr, godname(u.ualign.god),
			(u.ugodbase[UGOD_ORIGINAL] == u.ualign.god)  ? " (s,c)" : " (c)");
		Sprintf(buf, fmtstr, align_str_proper(galign(u.ualign.god)), buf2);
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, FALSE);
		}
	}

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
		//doenlightenment();
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


void
show_enlightenment(final, dumping)
int final;	/* 0 => still in progress; 1 => over, survived; 2 => dead */
boolean dumping;
{
	int ltmp;
	char buf[BUFSZ];
	char prebuf[BUFSZ];

	if (!dumping)
		en_win = create_nhwindow(NHW_MENU);
	put_enl(final ? "Final Attributes:" : "Current Attributes:");
	put_enl("");

	if (u.uevent.uhand_of_elbereth) {
	    you_are(crowning_title());
	}
	
	if(u.lastprayed){
		Sprintf(buf, "You last %s %ld turns ago", u.lastprayresult==PRAY_GIFT ? "received a gift" :
												  u.lastprayresult==PRAY_ANGER ? "angered your god" : 
												  u.lastprayresult==PRAY_CONV ? "converted to a new god" : 
												  "prayed",
			moves - u.lastprayed);
		put_enl(buf);
		if(u.lastprayresult==PRAY_GOOD){
			Sprintf(buf, "That prayer was well received");
			put_enl(buf);
		} else if(u.lastprayresult==PRAY_IGNORED){
			Sprintf(buf, "That prayer went unanswered");
			put_enl(buf);
		} else if(u.lastprayresult==PRAY_BAD){
			Sprintf(buf, "That prayer was poorly received");
			put_enl(buf);
		} else if(u.lastprayresult==PRAY_INPROG){
			enl_msg("That prayer ", "is ", "was ", "in progress");
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
		Sprintf(prebuf, "Your soul ");
		Sprintf(buf, " bound to ");
		for (i = 0; (i<=NUMINA) && (numFound < numBound); i++) {
			if (((i<QUEST_SPIRITS) ? u.sealsActive : u.specialSealsActive) & (get_sealID(i) & ~SEAL_SPECIAL)) {
				Strcat(buf, sealNames[i - FIRST_SEAL]);
				numFound++;
				if(numBound==2 && numFound==1) Strcat(buf," and ");
				else if(numBound>=3){
					if(numFound<numBound-1) Strcat(buf,", ");
					if(numFound==numBound-1) Strcat(buf,", and ");
				}
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
	if (Deadmagic) you_are("in a dead-magic zone");
	if (Catapsi) you_are("in a psionic storm");
	if (Misotheism) you_are("in a divine-exclusion zone");
	if (Waterproof) you_are("waterproof");
	if (Stone_resistance)
		you_are("petrification resistant");
	if (Poison_resistance) you_are("poison resistant");
	if (Shock_resistance) you_are("shock resistant");
	if (Sick_resistance) you_are("immune to sickness");
	if (Sleep_resistance) you_are("sleep resistant");
	if (Half_physical_damage) you_are("resistant to physical damage");
	if (Half_spell_damage) you_are("resistant to magical damage");
	if (u.uedibility || u.sealsActive&SEAL_BUER) you_can("recognize detrimental food");
	// if ( (ublindf && ublindf->otyp == R_LYEHIAN_FACEPLATE && !ublindf->cursed) || 
		 // (uarmc && uarmc->otyp == OILSKIN_CLOAK && !uarmc->cursed) ||
		 // (u.sealsActive&SEAL_ENKI) || (u.ufirst_sky)
	// ) you_are("waterproof");
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
	if (active_glyph(TRANSPARENT_SEA)){
		enl_msg("The perfectly clear sea ", "speed", "sped", " sanity recovery");
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
	if (active_glyph(SIGHT)){
		enl_msg("The recursive eye ", "lets", "let", " you strike more accurately at monsters.");
	}
	
	/*** Troubles ***/
	Sprintf(buf, "%d sanity points", u.usanity);
	you_have(buf);
	Sprintf(buf, "%d insight points", u.uinsight);
	you_have(buf);
	if (u.utaneggs > 0) {
		Sprintf(buf, "%d parasite eggs", u.utaneggs);
		you_have(buf);
	}
	
	if(Doubt)
		enl_msg("You ", "can't", "couldn't", " pray or use clerical magic");
	/*** Madnesses ***/
	if(u.usanity < 100 && !ClearThoughts){
		if (u.umadness&MAD_DELUSIONS){
			you_have("a tendency to hallucinate, obscuring some monsters' true forms");
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
			enl_msg("Sometimes, you ", "will panic", "panicked", " after taking cold damage or moving over ice");
		}
		if (u.umadness&MAD_CANNIBALISM){
			enl_msg("Sometimes, you ", "will vomit", "vomited", " after eating vegetarian or vegan food");
			enl_msg("Sometimes, you ", "will not be", "weren't", " warned before committing cannibalism");
		}
		if (u.umadness&MAD_RAGE){
			you_have("reduced AC, reduced spell success, and increased damage");
		}
		if (u.umadness&MAD_ARGENT_SHEEN){
			enl_msg("Sometimes, monsters ", "will gain", "gained", "  reflection for a turn");
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
			enl_msg("Sometimes, you ", "will panic", "panicked", " after being poisoned");
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
			enl_msg("Sometimes, you ", "will panic", "panicked", " after being attacked by aquatic monsters");
		}
		if (u.umadness&MAD_PARANOIA){
			//Severe because it triggers much more frequently than other madnesses
			enl_msg("Sometimes, you ", "will attack", "attacked", " monsters' hallucinatory twins instead");
			you_have("a hard time discerning the location of unseen monsters");
		}
		if (u.umadness&MAD_TALONS){
			enl_msg("Sometimes, you ", "will panic", "panicked", " after losing or releasing an item");
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
		if (u.umadness&MAD_NON_EUCLID){
			enl_msg("Sometimes, monsters ", "strike", "struck", " at you from strange angles");
		}
		if (u.umadness&MAD_SPIRAL){
			enl_msg("Your madness ", "is", "was", " spiraling out of control");
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
		if (u.umadness&MAD_THOUSAND_MASKS){
			enl_msg("The God of the Thousand Masks ", "stalks", "stalked", " your every step");
		}
		if (u.umadness&MAD_FORMICATION){
			enl_msg("You ", "are", "were", " distracted by invisible insects, lowering your skills and increasing spell failure");
		}
		if (u.umadness&MAD_HOST){
			enl_msg("You ", "are", "were", " nausiated by the thing inside you, which sometimes makes you vomit");
		}
		if (u.umadness&MAD_SCIAPHILIA){
			enl_msg("You ", "wish", "wished", " to stand in partially illuminated areas, suffering reduced accuracy and sometimes failing to cast spells if not");
		}
		if (u.umadness&MAD_FORGETFUL){
			enl_msg("Your mind ", "is", "was", " disolving");
		}
		if (u.umadness&MAD_TOO_BIG){
			enl_msg("", "It's", "It was", " too big");
		}
		if (u.umadness&MAD_APOSTASY){
			enl_msg("You sometimes ", "doubt", "doubted", " the gods, blocking casting and offending them");
		}
		if (u.umadness&MAD_ROTTING){
			enl_msg("Your body ", "is", "was", " rotting from within");
		}
	}
	
	if(u.utaneggs){
		Sprintf(buf, " lumps under your %s.", body_part(BODY_SKIN));
		enl_msg("There ", "are", "were", buf);
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
	if (BloodDrown) you_are("drowning in blood");
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
	if (u.umummyrot) enl_msg("You ", "are", "were", " gradually rotting to dust");

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
				((activeFightingForm(FFORM_SHIEN)) || 
				 (activeFightingForm(FFORM_SORESU))
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

	if (godlist[u.ualign.god].anger) {
	    Sprintf(buf, " %sangry with you",
		    godlist[u.ualign.god].anger > 6 ? "extremely " : godlist[u.ualign.god].anger > 3 ? "very " : "");
#ifdef WIZARD
	    if (wizard) Sprintf(eos(buf), " (%d)", godlist[u.ualign.god].anger);
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

	if (!dumping) {
		display_nhwindow(en_win, TRUE);
		destroy_nhwindow(en_win);
	}
	return;
}

STATIC_OVL void
resistances_enlightenment()
{
	char buf[BUFSZ];

	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, "Current Status:");
	putstr(en_win, 0, "");
	
	if(check_partial_action())
		putstr(en_win, 0, "You have used your partial action this round.");
	
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
	
	if(Deadmagic && base_casting_stat() == A_INT){
		int i;
		update_alternate_spells();
		for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
			putstr(en_win, 0, "Magic is damaged.");
			break;
		}
	}
	else if(Catapsi && base_casting_stat() == A_CHA){
		int i;
		update_alternate_spells();
		for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
			putstr(en_win, 0, "Your mind is full of static.");
			break;
		}
	}
	else if(Misotheism && base_casting_stat() == A_WIS){
		int i;
		update_alternate_spells();
		for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
			putstr(en_win, 0, "Your mind is full of static.");
			break;
		}
	}
	else if(Nullmagic){
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
	if (active_glyph(BLOOD_RAPTURE)) putstr(en_win, 0, "You see rainbows in mists of blood.");
	if (active_glyph(CLAWMARK)) putstr(en_win, 0, "Every surface you have ever seen was scored by claws.");
	if (active_glyph(CLEAR_DEEPS)) putstr(en_win, 0, "Your thoughts drift through blue water.");
	if (active_glyph(DEEP_SEA)) putstr(en_win, 0, "Your fears drown in pitch-black water.");
	if (active_glyph(TRANSPARENT_SEA)) putstr(en_win, 0, "Your mind is bulwarked by the clear deep sea.");
	if (active_glyph(COMMUNION)) {
		Race_if(PM_ANDROID) ? putstr(en_win, 0, "A strange minister preaches continuously in the city where you were built and tested.") :
		Race_if(PM_CLOCKWORK_AUTOMATON) ? putstr(en_win, 0, "A strange minister preaches continuously in your maker's workshop.") :
		putstr(en_win, 0, "A strange minister preaches continuously in your childhood home.");
	}
	if (active_glyph(CORRUPTION)) putstr(en_win, 0, "It weeps tears of blood.");
	if (active_glyph(EYE_THOUGHT)) putstr(en_win, 0, "Eyes writhe inside your head.");
	if (active_glyph(FORMLESS_VOICE)) putstr(en_win, 0, "A great voice speaks to you.");
	if (active_glyph(GUIDANCE)) putstr(en_win, 0, "You see dancing sprites, far away in the dark behind your eyes.");
	if (active_glyph(IMPURITY)) putstr(en_win, 0, "Red millipedes crawl through the filth inside your brain.");
	if (active_glyph(MOON)) putstr(en_win, 0, "You see the face of the sympathetic moon.");
	if (active_glyph(WRITHE)) putstr(en_win, 0, "A subtle mucus covers your brain.");
	if (active_glyph(RADIANCE)) putstr(en_win, 0, "Your mind is impaled on a golden pyramid.");
	if (active_glyph(BEASTS_EMBRACE)) putstr(en_win, 0, "A bestial figure hides inside of you.");
	if (active_glyph(SIGHT)) putstr(en_win, 0, "Your brain is but the lid of an eye within an eye within an eye....");
	
	/*** Troubles ***/
	if(u.usanity == 0)
		putstr(en_win, 0, "You are quite insane.");
	else if(u.usanity < 10)
		putstr(en_win, 0, "You constantly struggle with insanity.");
	else if(u.usanity < 25)
		putstr(en_win, 0, "You frequently struggle with insanity.");
	else if(u.usanity < 50)
		putstr(en_win, 0, "You periodically struggle with insanity.");
	else if(u.usanity < 75)
		putstr(en_win, 0, "You occasionally struggle with insanity.");
	else if(u.usanity < 90)
		putstr(en_win, 0, "You rarely struggle with insanity.");
	else if(u.usanity < 100)
		putstr(en_win, 0, "You are a little touched in the head.");
	
	if(u.uinsight > 40)
		putstr(en_win, 0, "You frequently see things you wish you hadn't.");
	else if(u.uinsight > 20)
		putstr(en_win, 0, "You periodically see things you wish you hadn't.");
	else if(u.uinsight > 1)
		putstr(en_win, 0, "You occasionally see things you wish you hadn't.");
	
	if(Doubt)
		putstr(en_win, 0, "You are having a crisis of faith.");
	/*** Madnesses ***/
	if(u.usanity < 100 && !ClearThoughts){
		char messaged = 0;
		if (u.umadness&MAD_DELUSIONS){
			putstr(en_win, 0, "You have a tendency to hallucinate.");
			messaged++;
		}
		if(u.usanity < 80){
			if (u.umadness&MAD_REAL_DELUSIONS){
				if(u.umadness&MAD_DELUSIONS)
					putstr(en_win, 0, "...at least, you THINK you're hallucinating....");
				else
					putstr(en_win, 0, "You have a tendency to hallucinate... you think.");
				messaged++;
			}
		}
		if (u.umadness&MAD_SANCTITY){
			putstr(en_win, 0, "You have a tendency to treat women as delicate and holy beings who shouldn't be harmed.");
			messaged++;
		}
		if (u.umadness&MAD_GLUTTONY){
			putstr(en_win, 0, "You have a mad hunger.");
			messaged++;
		}
		if (u.umadness&MAD_SPORES && !Race_if(PM_ANDROID) && !Race_if(PM_CLOCKWORK_AUTOMATON)){
			//Note: Race_if is correct because it works on your original brain (because magic)
			putstr(en_win, 0, "She's eating your brain.");
			messaged++;
		}
		if (u.umadness&MAD_FRIGOPHOBIA){
			putstr(en_win, 0, "You have an irrational fear of the cold.");
			messaged++;
		}
		if (u.umadness&MAD_CANNIBALISM){
			putstr(en_win, 0, "You have a mad desire to consume living flesh, even the flesh of your own kind.");
			messaged++;
		}
		if (u.umadness&MAD_RAGE){
			putstr(en_win, 0, "You have a burning, irrational rage.");
			messaged++;
		}
		if (u.umadness&MAD_ARGENT_SHEEN){
			putstr(en_win, 0, "The world is full of mirrors, and you can't help but admire yourself.");
			messaged++;
		}
		if (u.umadness&MAD_SUICIDAL){
			putstr(en_win, 0, "You have a tendency towards suicidal behavior.");
			messaged++;
		}
		if (u.umadness&MAD_NUDIST){
			putstr(en_win, 0, "You have an irrational dislike of clothing.");
			messaged++;
		}
		if (u.umadness&MAD_OPHIDIOPHOBIA){
			putstr(en_win, 0, "You have an irrational fear of snakes.");
			messaged++;
		}
		if (u.umadness&MAD_ARACHNOPHOBIA){
			//Note: Arachnophobia == women and spiders because it's the madness Lolth inflicts.
			putstr(en_win, 0, "You have an irrational fear of spiders.");
			putstr(en_win, 0, "You have an irrational fear of women.");
			messaged++;
		}
		if (u.umadness&MAD_ENTOMOPHOBIA){
			putstr(en_win, 0, "You have an irrational fear of insects.");
			messaged++;
		}
		if (u.umadness&MAD_THALASSOPHOBIA){
			putstr(en_win, 0, "You have an irrational fear of sea-monsters.");
			messaged++;
		}
		if (u.umadness&MAD_PARANOIA){
			//Severe because it triggers much more frequently than other madnesses
			putstr(en_win, 0, "You have a severe tendency towards paranoia.");
			messaged++;
		}
		if (u.umadness&MAD_TALONS){
			putstr(en_win, 0, "You have an irrational fear of loss.");
			messaged++;
		}
		if (u.umadness&MAD_COLD_NIGHT){
			putstr(en_win, 0, "It's so cold.");
			messaged++;
		}
		if (u.umadness&MAD_OVERLORD){
			putstr(en_win, 0, "He can see you.");
			messaged++;
		}
		if (u.umadness&MAD_DREAMS){
			putstr(en_win, 0, "You dream of strange cities.");
			messaged++;
		}
		if (u.umadness&MAD_NON_EUCLID){
			putstr(en_win, 0, "Sometimes, monsters strike at you from strange angles.");
			messaged++;
		}
		if (u.umadness&MAD_SPIRAL){
			putstr(en_win, 0, "Your madness is spiraling out of control.");
			messaged++;
		}
		if (u.umadness&MAD_HELMINTHOPHOBIA){
			putstr(en_win, 0, "You have an irrational fear of squirming things.");
			messaged++;
		}
		if(has_blood(youracedata)){
			if (u.umadness&MAD_FRENZY){
				Sprintf(buf, "You feel your %s seethe below your %s.", body_part(BLOOD), body_part(BODY_SKIN));
				putstr(en_win, 0, buf);
				messaged++;
			}
		}
		if (u.umadness&MAD_THOUSAND_MASKS){
			putstr(en_win, 0, "The God of the Thousand Masks stalks your every step.");
		}
		if (u.umadness&MAD_FORMICATION){
			putstr(en_win, 0, "You are distracted by the feeling of insects crawling over your body.");
		}
		if (u.umadness&MAD_HOST){
			putstr(en_win, 0, "You are nausiated by the thing inside you.");
		}
		if (u.umadness&MAD_SCIAPHILIA){
			putstr(en_win, 0, "You are fascinated by the dancing shadows.");
		}
		if (u.umadness&MAD_FORGETFUL){
			putstr(en_win, 0, "Your mind is disolving.");
		}
		if (u.umadness&MAD_TOO_BIG){
			putstr(en_win, 0, "It's too BIG!");
		}
		if (u.umadness&MAD_APOSTASY){
			putstr(en_win, 0, "You sometimes doubt the gods.");
		}
		if (u.umadness&MAD_ROTTING){
			putstr(en_win, 0, "Your body is rotting from within.");
		}
		if(messaged){
			//Clockworks specifically can't get drunk (androids can)
			if(uclockwork && u.usanity <= 90){
				putstr(en_win, 0, "Perhaps some sedatives would settle your mind.");
			}
			else {
				//Advice around alcohol differs depending on drunkenness level
				if(u.udrunken < u.ulevel*3){
					if(u.usanity <= 45){
						putstr(en_win, 0, "You need a stiff drink, or some sedatives.");
					}
					else if(u.usanity <= 90){
						putstr(en_win, 0, "You could use a stiff drink, or maybe some sedatives.");
					}
					else if(u.usanity <= 95){
						putstr(en_win, 0, "You could use a drink.");
					}
				} else {
					if(u.usanity <= 45){
						putstr(en_win, 0, "You need a stiff drink, or some sedatives.");
					}
					else if(u.usanity <= 90){
						putstr(en_win, 0, "Perhaps some sedatives would settle your mind.");
					}
				}
			}
		}
	}
	/*** More Troubles ***/
	if(u.utaneggs){
		Sprintf(buf, "There are lumps under your %s.", body_part(BODY_SKIN));
		putstr(en_win, 0, buf);
	}
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
	if (u.umummyrot){
		Sprintf(buf, "You are gradually rotting to dust");
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
		} else if(u.lastprayresult==PRAY_IGNORED){
			Sprintf(buf, "That prayer went unanswered");
			putstr(en_win, 0, buf);
		} else if(u.lastprayresult==PRAY_BAD){
			Sprintf(buf, "That prayer was poorly recieved");
			putstr(en_win, 0, buf);
		} else if(u.lastprayresult==PRAY_INPROG){
			Sprintf(buf, "That prayer is in progress");
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
	int cap = 11;
	char mbuf[BUFSZ] = {'\0'};
	en_win = create_nhwindow(NHW_MENU);
	putstr(en_win, 0, "Current Damage Reduction:");
	putstr(en_win, 0, "");
	
	if(active_glyph(DEEP_SEA))
		cap += 3;
	if(!has_head_mon(&youmonst)){
		Sprintf(mbuf, "You have no head; shots hit upper body");
		putstr(en_win, 0, mbuf);
	} else {
		dr = slot_udr(HEAD_DR, (struct monst *)0, 0);
		if(dr > cap)
		Sprintf(mbuf, "Head Armor:       %d-%d", cap, dr);
		else
		Sprintf(mbuf, "Head Armor:       %d", dr);
		putstr(en_win, 0, mbuf);
	}
	dr = slot_udr(UPPER_TORSO_DR, (struct monst *)0, 0);
	if(dr > cap)
		Sprintf(mbuf, "Upper Body Armor: %d-%d", cap, dr);
	else
		Sprintf(mbuf, "Upper Body Armor: %d", dr);
	dr = slot_udr(LOWER_TORSO_DR, (struct monst *)0, 0);
	putstr(en_win, 0, mbuf);
	if(dr > cap)
		Sprintf(mbuf, "Lower Body Armor: %d-%d", cap, dr);
	else
		Sprintf(mbuf, "Lower Body Armor: %d", dr);
	putstr(en_win, 0, mbuf);
	if(!can_wear_gloves(youracedata)){
		Sprintf(mbuf, "You have no hands; shots hit upper body");
		putstr(en_win, 0, mbuf);
	} else {
		dr = slot_udr(ARM_DR, (struct monst *)0, 0);
		if(dr > cap)
		Sprintf(mbuf, "Hand Armor:       %d-%d", cap, dr);
		else
		Sprintf(mbuf, "Hand Armor:       %d", dr);
		putstr(en_win, 0, mbuf);
	}
	if(!can_wear_boots(youracedata)){
		Sprintf(mbuf, "You have no feet; shots hit lower body");
		putstr(en_win, 0, mbuf);
	} else {
		dr = slot_udr(LEG_DR, (struct monst *)0, 0);
		if(dr > cap)
		Sprintf(mbuf, "Foot Armor:       %d-%d", cap, dr);
		else
		Sprintf(mbuf, "Foot Armor:       %d", dr);
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
		if(!ublindf || ublindf->otyp == LENSES || ublindf->otyp == SUNGLASSES || ublindf->otyp == LIVING_MASK)
			putstr(en_win, 0, "A black liquid leaks from around your eyes.");
		else if(ublindf && (ublindf->otyp == MASK || ublindf->otyp == R_LYEHIAN_FACEPLATE))
			putstr(en_win, 0, "The black liquid leaking from your eyes is hidden by your mask.");
		else
			putstr(en_win, 0, "The black liquid leaking from your eyes is soaked up by your blindfold.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_BALAM && !Invis){
		if(uarmc || (uarm && arm_blocks_upper_body(uarm->otyp)))
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
		if(!(uarmc || ((uarm && !is_opaque(uarm)) || uarmu))){
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
		if(!(uarmf && (uarmc || (uarm && arm_blocks_upper_body(uarm->otyp)))))
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
		if(!((uarm && arm_blocks_upper_body(uarm->otyp)) || uarmc))
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
		if(!(uarm && arm_blocks_upper_body(uarm->otyp)) && !uarmc){
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
		} else if(!(uarm && arm_blocks_upper_body(uarm->otyp)) && !uarmc){
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
		if(!((uarm && arm_blocks_upper_body(uarm->otyp)) && is_metallic(uarm)))
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
		if(uarmc && is_mummy_wrap(uarmc)){
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
			if(!((uarm && arm_blocks_upper_body(uarm->otyp)) || uarmc))
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
			if(!(uarmc && uarmg && uarmf && (uarm && arm_blocks_upper_body(uarm->otyp)) && uarmh))
				putstr(en_win, 0, "Your rotted body bristles with fungal sporangia and burrowing vermin.");
			else
				putstr(en_win, 0, "Your rotted form is hidden under your clothes.");
			message = TRUE;
		} else if(moves>100000){
			if(!(uarmc && uarmg && uarmf && (uarm && arm_blocks_upper_body(uarm->otyp)) && uarmh && ublindf && ublindf->otyp==MASK))
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
		if(!(ublindf && (ublindf->otyp==MASK || ublindf->otyp == R_LYEHIAN_FACEPLATE))){
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
	if(u.sealsActive&SEAL_ANDREALPHUS && !Invis && (dimness(u.ux, u.uy) <= 0)) {
		putstr(en_win, 0, "Your shadow has a strange depth.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_ANDROMALIUS && !NoBInvis){
		if(dimness(u.ux, u.uy) <= 0)
			putstr(en_win, 0, "Your rigid features can't be seen in the dark.");
		else if((ublindf && (ublindf->otyp==MASK || ublindf->otyp==R_LYEHIAN_FACEPLATE)) //face-covering mask
			 || (uarmh && (uarmh->otyp==PLASTEEL_HELM || uarmh->otyp==PONTIFF_S_CROWN || uarmh->otyp==FACELESS_HELM)) //opaque face-covering helm
			 || (uarmc && (uarmc->otyp==WHITE_FACELESS_ROBE || uarmc->otyp==BLACK_FACELESS_ROBE || uarmc->otyp==SMOKY_VIOLET_FACELESS_ROBE))//face-covering robe
		) putstr(en_win, 0, "Your rigid features can't be seen through your disguise.");
		else putstr(en_win, 0, "Your features have taken on the rigidity of a cheap disguise.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_ASTAROTH && !Invis){
		if(!ublindf || ublindf->otyp == LENSES || ublindf->otyp == SUNGLASSES || ublindf->otyp == LIVING_MASK)
			putstr(en_win, 0, "A black liquid leaks from around your eyes.");
		else if(ublindf && (ublindf->otyp == MASK || ublindf->otyp == R_LYEHIAN_FACEPLATE))
			putstr(en_win, 0, "The black liquid leaking from your eyes is hidden by your mask.");
		else {
			Sprintf(msgbuf, "The black liquid leaking from your eyes is soaked up by your %s.", xname(ublindf));
			putstr(en_win, 0, msgbuf);
		}
		message = TRUE;
	}
	if(u.sealsActive&SEAL_BALAM && !Invis){
		if(uarmc || (uarm && arm_blocks_upper_body(uarm->otyp)))
			putstr(en_win, 0, "Freezing water leaks from a gash in your neck, but is hidden by your clothes.");
		else
			putstr(en_win, 0, "Freezing water leaks from a deep gash in your neck.");
		if(!uarmg)
			putstr(en_win, 0, "Freezing water leaks from deep holes in your wrists.");
		if(!uarmf)
			putstr(en_win, 0, "Freezing water leaks from deep holes in your ankles.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_BERITH && !Invis){
		if(!(uarm && arm_blocks_upper_body(uarm->otyp) && is_metallic(uarm) && uarmg && uarmf && uarmh))
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
		if(!(ublindf && (ublindf->otyp==MASK || ublindf->otyp==R_LYEHIAN_FACEPLATE))){
			putstr(en_win, 0, "You see a pair of chelicerae in your mouth!");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_DANTALION && !NoBInvis){
		if(!(uarmc || ((uarm && !is_opaque(uarm)) || uarmu))){
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
		if(!(uarmf && (uarmc || (uarm && arm_blocks_upper_body(uarm->otyp)))))
			putstr(en_win, 0, "Your hips give rise to twin serpent's tails instead of legs.");
		else
			putstr(en_win, 0, "Your serpentine legs are disguised by your clothes.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_EDEN && !NoBInvis && !uarmh){
		putstr(en_win, 0, "You see a garden through the dome of cerulean crystal embedded in your head!");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_ENKI && !Invis){
		if(!((uarm && arm_blocks_upper_body(uarm->otyp)) || uarmc))
			putstr(en_win, 0, "Water runs off your body in steady rivulets.");
		else
			putstr(en_win, 0, "Your body's runoff is caught by your clothes.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_EVE && !NoBInvis){
		if(!(uarm && arm_blocks_upper_body(uarm->otyp)) && !uarmc){
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
		if(!uarmh){
			putstr(en_win, 0, "You find a raven nesting in each ear!");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_IRIS && !NoBInvis){
		if(moves <= u.irisAttack+1){
			putstr(en_win, 0, "Waving, iridescent tentacles sprout from your forearms.");
			message = TRUE;
		} else if(!uarmc && moves <= u.irisAttack+5){
			putstr(en_win, 0, "There are iridescent tentacles wrapped around your forearms.");
			message = TRUE;
		} else if(!(uarm && arm_blocks_upper_body(uarm->otyp)) && !uarmc){
			putstr(en_win, 0, "There are iridescent veins just under the skin of your forearms.");
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
		if(!(ublindf && (ublindf->otyp==MASK || ublindf->otyp==R_LYEHIAN_FACEPLATE))){
			putstr(en_win, 0, "There is a whole flock's worth of crows peeking out of your throat!");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_MARIONETTE && !NoBInvis){
		if(!((uarm && arm_blocks_upper_body(uarm->otyp))&& is_metallic(uarm)))
			putstr(en_win, 0, "Metal wires protrude from your elbows, knees, and back.");
		else
			putstr(en_win, 0, "The metal wires protruding from your body have merged with your armor.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_MOTHER && !NoBInvis){
		if(!uarmg && !(uarmc && is_mummy_wrap(uarmc)))
			putstr(en_win, 0, "The eyes on your fingers and palms stare back at you.");
		else
			putstr(en_win, 0, "The eyes on your fingers and palms are covered up.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_NABERIUS && !NoBInvis){
		if(!(ublindf && (ublindf->otyp==MASK || ublindf->otyp==R_LYEHIAN_FACEPLATE))){
			putstr(en_win, 0, "Your tongue is forked!");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_ORTHOS && !NoBInvis){
		if(uarmc && !is_mummy_wrap(uarmc)){
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
			putstr(en_win, 0, "Your iron claws seem to be part of your gloves.");
		if(!uarmh)
			putstr(en_win, 0, "There is a pair of prismatic wings reaching around your head.");
		else
			putstr(en_win, 0, "Your helm has a crest of prismatic feathers.");
		message = TRUE;
	}
	if(u.sealsActive&SEAL_TENEBROUS && !Invis){
		if(dimness(u.ux,u.uy) <= 0) {
			putstr(en_win, 0, "Your shadow is unnaturally dark and pools close to you.");
			message = TRUE;
		}
	}
	if(u.sealsActive&SEAL_YMIR && !Invis){
		if(moves>5000 && moves <= 10000){
			if(!((uarm && arm_blocks_upper_body(uarm->otyp)) || uarmc))
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
			if(!(uarmc && uarmg && uarmf && (uarm && arm_blocks_upper_body(uarm->otyp)) && uarmh))
				putstr(en_win, 0, "Your rotted body bristles with fungal sporangia and burrowing vermin.");
			else
				putstr(en_win, 0, "Your rotted form is hidden under your clothes.");
			message = TRUE;
		} else if(moves>100000){
			if(!(uarmc && uarmg && uarmf && (uarm && arm_blocks_upper_body(uarm->otyp)) && uarmh && ublindf && (ublindf->otyp==MASK || ublindf->otyp==R_LYEHIAN_FACEPLATE)))
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
		else if(ublindf && (ublindf->otyp == SUNGLASSES))
			putstr(en_win, 0, "Your star-like eyes are covered by your mirrored lenses.");
		else {
			Sprintf(msgbuf, "Your star-like eyes are covered by your %s.", xname(ublindf));
			putstr(en_win, 0, msgbuf);
		}
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
	if(u.specialSealsActive&SEAL_BLACK_WEB && !Invis){
		if(dimness(u.ux, u.uy) <= 0) {
			putstr(en_win, 0, "Your shadow is wrapped in the black web.");
			message = TRUE;
		}
	}
	
	if(!message){
		putstr(en_win, 0, "You think you look pretty normal.");
	}
	
	display_nhwindow(en_win, TRUE);
	destroy_nhwindow(en_win);
	return;
}


void
show_conduct(final, dumping)
int final;
boolean dumping;
{
	char buf[BUFSZ];
	int ngenocided;

	/* Create the conduct window if not dumping */
	if (!dumping)
		en_win = create_nhwindow(NHW_MENU);

	put_enl("Voluntary challenges:");
	put_enl("");

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
	if(is_june()){
#define	CHECK_ACHIEVE(aflag, string) \
	if(achieve.trophies&aflag){\
		put_enl(string);\
	}
		put_enl("");
		put_enl("Junethack challenges:");
		put_enl("");
	CHECK_ACHIEVE(ARC_QUEST,"Walking international incident: completed archeologist quest")
	CHECK_ACHIEVE(CAV_QUEST,"Serpent slayer: completed caveman quest")
	CHECK_ACHIEVE(CON_QUEST,"Sentence commuted: completed convict quest")
	CHECK_ACHIEVE(KNI_QUEST,"Into the crystal cave: completed knight quest")
	CHECK_ACHIEVE(ANA_QUEST,"Back from the future: completed anachrononaut quest")
	CHECK_ACHIEVE(AND_QUEST,"Glory to mankind: completed android quest")
	CHECK_ACHIEVE(BIN_QUEST,"33 spirits: completed binder quest")
	CHECK_ACHIEVE(PIR_QUEST,"Not so inconceivable: completed pirate quest")
	CHECK_ACHIEVE(BRD_QUEST,"Not so spoony: completed bard quest")
	CHECK_ACHIEVE(NOB_QUEST,"Rebellion crushed: completed base noble quest")
	CHECK_ACHIEVE(MAD_QUEST,"Oh good. I'm not crazy: completed madman quest")
	CHECK_ACHIEVE(HDR_NOB_QUEST,"Family drama: completed hedrow noble quest")
	CHECK_ACHIEVE(HDR_SHR_QUEST,"On agency: completed hedrow shared quest")
	CHECK_ACHIEVE(DRO_NOB_QUEST,"Foreshadowing: completed drow noble quest")
	CHECK_ACHIEVE(DRO_SHR_QUEST,"Old friends: completed drow shared quest")
	CHECK_ACHIEVE(DWA_NOB_QUEST,"Durin's Bane's Bane: completed dwarf noble quest")
	CHECK_ACHIEVE(DWA_KNI_QUEST,"Battle of (5-4) armies: completed dwarf knight quest")
	CHECK_ACHIEVE(GNO_RAN_QUEST,"Strongest of all time: completed gnome ranger quest")
	CHECK_ACHIEVE(ELF_SHR_QUEST,"Driven out: completed elf shared quest")
	CHECK_ACHIEVE(LAW_QUEST,"Ripple-resistant tower: completed law quest")
	CHECK_ACHIEVE(NEU_QUEST,"Key to the (corpse) city: completed neutral quest")
	CHECK_ACHIEVE(CHA_QUEST,"Asinine paradigm: completed chaos temple quest")
	CHECK_ACHIEVE(MITH_QUEST,"Chasing after the wind: completed mithardir quest")
	CHECK_ACHIEVE(MORD_QUEST,"Fossil of the First Age: completed mordor quest")
	CHECK_ACHIEVE(SECOND_THOUGHTS,"Had second thoughts after a drow quest")
	CHECK_ACHIEVE(LAMASHTU_KILL,"Does this count as a paradox?: killed Lamashtu")
	CHECK_ACHIEVE(BAALPHEGOR_KILL,"A universe without motion: killed Baalphegor")
	CHECK_ACHIEVE(ANGEL_VAULT,"Opened an angelic hell-vault")
	CHECK_ACHIEVE(ANCIENT_VAULT,"Opened an ancient hell-vault")
	CHECK_ACHIEVE(TANNINIM_VAULT,"Opened a tanninim hell-vault")
	CHECK_ACHIEVE(UNKNOWN_WISH,"Earned a wish from an unknown god")
	CHECK_ACHIEVE(CASTLE_WISH,"Completed the castle")
	CHECK_ACHIEVE(ILLUMIAN,"Became illuminated")
	CHECK_ACHIEVE(RESCUE,"Lead an exodus")
	CHECK_ACHIEVE(FULL_LOADOUT,"Super Fighting Robot: fully upgraded a clockwork automata")
	CHECK_ACHIEVE(NIGHTMAREHUNTER,"Hunter of Nightmares")
	CHECK_ACHIEVE(QUITE_MAD,"Quite Mad: Suffered six madnesses")
	CHECK_ACHIEVE(TOTAL_DRUNK,"Booze Hound")
		
#undef	CHECK_ACHIEVE
	}
	/* Pop up the window and wait for a key */
	if (!dumping) {
		display_nhwindow(en_win, TRUE);
		destroy_nhwindow(en_win);
	}
}

/*enlighten.c*/
