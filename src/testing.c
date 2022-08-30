/*	SCCS Id: @(#)testing.c 3.4	2003/08/11	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"

STATIC_DCL void NDECL(test_readobjnam);
STATIC_DCL void NDECL(test_levelgen);

#define TEST_READOBJNAM 1
#define TEST_LEVELGEN 2

int
wiz_testmenu()
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

	Sprintf(buf, "Select test to run:");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	Sprintf(buf, "Test readobjnam()");
	any.a_int = TEST_READOBJNAM;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet++;

	Sprintf(buf, "Test level generation");
	any.a_int = TEST_LEVELGEN;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet++;
	
	end_menu(tmpwin, "Select test to run:");

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	
	if (n > 0) {
		switch(selected[0].item.a_int) {
			case TEST_READOBJNAM:
				test_readobjnam();
				break;
			case TEST_LEVELGEN:
				test_levelgen();
				break;
		}
		free(selected);
	}

	return 0;
}

void
test_readobjnam()
{
	int i;
	int wishreturn;
	struct obj * otmp;
	char buf[BUFSZ];
	char buf2[BUFSZ];
	boolean savestate;

#define TEST(str, correct_condition) do{										\
	Strcpy(buf, (str));															\
	Strcpy(buf2, buf);															\
	otmp = readobjnam(buf, &wishreturn, WISH_WIZARD|WISH_ARTALLOW);				\
	if (otmp == &zeroobj) {														\
		pline("wish failed: \"%s\"", buf2);										\
	}																			\
	else if (!(correct_condition)) {											\
		fully_identify_obj(otmp);												\
		pline("wrong item: \"%s\" (got %s)", buf2, xname(otmp));				\
	}																			\
	if (otmp != &zeroobj)														\
		delobj(otmp);}while(0)


	/* blanket test to try to wish for all objects */
	for (i=1; i < NUM_OBJECTS; i++) {
		/* skip non-objects that exist just for additional descriptions */
		if (!obj_descr[(objects[i].oc_name_idx)].oc_name) continue;
		/* known and acceptable failures */
		if (i == CLOAK || i == SHOES || i == GAUNTLETS ||	// both a common item and a class; common item wishable with material
			i == HANDGUN ||	// unused item and synonym for pistol
			i == GUN ||		// unused item and a class
			i == GOLD_PIECE)// correctly gives a gold piece, which is not an object
			continue;
		/* save old name_known state; set to TRUE so simple_typename() gives real name */
		savestate = objects[i].oc_name_known;
		objects[i].oc_name_known = 1;
		/* test */
		TEST(simple_typename(i), otmp->otyp == i);
		/* restore old name_known state */
		objects[i].oc_name_known = savestate;
	}

	/* blanket test to try to wish for 2 of all mergable objects */
	for (i=1; i < NUM_OBJECTS; i++) {
		/* skip non-objects that exist just for additional descriptions */
		if (!obj_descr[(objects[i].oc_name_idx)].oc_name) continue;
		/* skip non-mergable objects */
		if (!objects[i].oc_merge) continue;
		/* known and acceptable failures */
		if (i == GOLD_PIECE)// correctly gives 2 gold pieces, which are not an object
		/* save old name_known state; set to TRUE so simple_typename() gives real name */
		savestate = objects[i].oc_name_known;
		objects[i].oc_name_known = 1;
		/* test */
		Sprintf(buf2, "2 %s", makeplural(simple_typename(i)));
		TEST(buf2, otmp->otyp == i && otmp->quan == 2);
		/* restore old name_known state */
		objects[i].oc_name_known = savestate;
	}

	/* blanket test to try to wish for all artifacts */
	for (i=1; i < NROFARTIFACTS; i++) {
		/* known and acceptable failures */
		if (i == ART_GENOCIDE ||	// gives scroll, which is acceptable since it's literally the same name
			i == ART_NECRONOMICON || i == ART_BOOK_OF_LOST_NAMES || i == ART_BOOK_OF_INFINITE_SPELLS)//gives randomly of the three
			continue;
		/* save old artifact-existance state */
		savestate = artinstance[i].exists;
		artinstance[i].exists = 0;
		/* test */
		TEST(artiname(i), otmp->oartifact == i);
		/* restore old artifact-existance state */
		artinstance[i].exists = savestate;
	}

	/* blanket test to try to wish for a statue of every monster */
	for (i=LOW_PM; i < NUMMONS; i++) {
		/* known and acceptable failures */
		if (i == PM_HUMAN_WEREWOLF || i == PM_HUMAN_WERERAT || i == PM_HUMAN_WEREJACKAL ||	//gives animal form (of same name)
			i == PM_LONG_WORM_TAIL || i == PM_HUNTING_HORROR_TAIL)// gives main creature
			continue;
		Sprintf(buf2, "statue of %s",
			type_is_pname(&mons[i]) ? mons[i].mname : an(mons[i].mname));
		TEST(buf2, otmp->otyp == STATUE && otmp->corpsenm == i);
	}

	/* tests for specific items */
	TEST("potion of green dragon blood", otmp->otyp == POT_BLOOD && otmp->corpsenm == PM_GREEN_DRAGON);
	TEST("2 potions of tengu blood", otmp->otyp == POT_BLOOD && otmp->corpsenm == PM_TENGU && otmp->quan == 2);
	TEST("tin of spinach", otmp->otyp == TIN && otmp->spe == 1);
	TEST("healing potion", otmp->otyp == POT_HEALING);

#undef TEST
}

/* 
 * attempts to generate every level many times,
 * to catch segfaults, impossible()s, and panic()s.
 *   excludes the Endgame
 *   excludes Ludios if the portal didn't generate
 */
void
test_levelgen()
{
	int i, j;
	struct d_level newlevel;
	struct d_level curlevel = u.uz;
	int newlev;
	boolean inverted;
	boolean savestate = quest_status.got_quest;
	quest_status.got_quest = TRUE;

	extern int n_dgns;	// from dungeon.c
    for (i = 0; i < n_dgns; i++)
	{
		newlevel.dnum = i;
		newlevel.dlevel = 1;
		goto_level(&newlevel, FALSE, FALSE, FALSE);
		inverted = (dungeons[u.uz.dnum].entry_lev == dungeons[u.uz.dnum].num_dunlevs);
		do {
			for (j=0; j<40; j++)
				(void)wiz_makemap();
			if ((!inverted) ? !Is_botlevel(&u.uz) : u.uz.dlevel != 1) {
				newlev = depth(&u.uz) + ((!inverted) ? 1 : -1);
				get_level(&newlevel, newlev);
				goto_level(&newlevel, FALSE, FALSE, FALSE);
			}
			else
				break;
		}while(TRUE);
	}
	quest_status.got_quest = savestate;

	goto_level(&curlevel, FALSE, FALSE, FALSE);
}