/*	SCCS Id: @(#)objects.c	3.4	2002/07/31	*/
/* Copyright (c) Mike Threepoint, 1989.				  */
/* NetHack may be freely redistributed.  See license for details. */

/* both passes */
#include "macromagic.h"

/* object field overrides */
#define O_MERGE(x)		C02(x)
#define O_USKWN(x)		C03(x)
#define O_MAGIC(x)		C05(x)
#define O_UNIQ(x)		C07(x)
#define O_NOWISH(x)		C08(x)
#define O_SIZE(x)		C09(x)
#define O_DIR(x)		C12(x)
#define O_DRSLOT(x)		C13(x)
#define O_DTYPE(x)		C13(x)
#define O_MAT(x)		C14(x)
#define O_MATSPEC(x)	C15(x)
#define O_SKILL(x)		C16(x)
#define O_POWER(x)		C17(x)
#define O_DELAY(x)		C19(x)
#define O_COLOR(x)		C20(x)
#define O_PROB(x)		C21(x)
#define O_WT(x)			C22(x)
#define O_COST(x)		C23(x)
#define O_NUT(x)		C29(x)

/* modifying the blindname of an object */
#define DEF_BLINDNAME(names, blindname) (SETNAMES(names, C03(blindname)))
#define SETNAMES(names, ...) SET03(((char *)0), ((char *)0), ((char *)0), ##__VA_ARGS__, FILLNAMES__(_DEPAREN(names)))
#define FILLNAMES__(...) FILLNAMES_(NARGS(__VA_ARGS__), __VA_ARGS__)
#define FILLNAMES_(...)	FILLNAMES(__VA_ARGS__)
#define FILLNAMES(N, ...) FILLNAMES##N(__VA_ARGS__)
#define FILLNAMES01(a)     C01((a))
#define FILLNAMES02(a,b)   C01((a)), C02((b))
#define FILLNAMES03(a,b,c) C01((a)), C02((b)), C03((c))

/* setting damage dice in a non-awful way */
#define DMG_(...) DMG__(__VA_ARGS__)
#define DMG__(ocn, ocd, bonn, bond, flat) \
						({ocn, ocd, bonn, bond, flat, 0, 0, 0})
#define DMG(...)		DMG_(SET05(1, 2, 0, 0, 0, NWEPDICE(NARGS(__VA_ARGS__),__VA_ARGS__)))
#define NWEPDICE(...)			NWEPDICE_(__VA_ARGS__)
#define NWEPDICE_(N, ...)		NWEPDICE##N(NARGS(__VA_ARGS__),__VA_ARGS__)
#define NWEPDICE03(...)			NWEPDICE03_(__VA_ARGS__)
#define NWEPDICE02(...)			NWEPDICE02_(__VA_ARGS__)
#define NWEPDICE01(...)			NWEPDICE01_(__VA_ARGS__)
#define NWEPDICE03_(N, a, b, c)	PREFIXDICE##N##c, NWEPDICE02_(02, a, b)
#define NWEPDICE02_(N, a, b)	PREFIXDICE##N##b, NWEPDICE01_(01, a)
#define NWEPDICE01_(N, a)		PREFIXDICE##N##a
#define PREFIXDICE01D(...)	OCDICE(NARGS(__VA_ARGS__), __VA_ARGS__)
#define OCDICE(...)			OCDICE_(__VA_ARGS__)
#define OCDICE_(N, ...)		OCDICE##N(__VA_ARGS__)
#define OCDICE01(x)			C02(x)
#define OCDICE02(n, x)		C01(n), C02(x)
#define PREFIXDICE02D(...)	BONDICE(NARGS(__VA_ARGS__), __VA_ARGS__)
#define BONDICE(...)		BONDICE_(__VA_ARGS__)
#define BONDICE_(N, ...)	BONDICE##N(__VA_ARGS__)
#define BONDICE01(x)		C03(1), C04(x)
#define BONDICE02(n, x)		C03(n), C04(x)
#define PREFIXDICE01F(x)	C05(x)
#define PREFIXDICE02F(x)	C05(x)
#define PREFIXDICE03F(x)	C05(x)

#ifndef OBJECTS_PASS_2_
/* first pass */
struct monst { struct monst *dummy; };	/* lint: struct obj's union */
#include "config.h"
#include "obj.h"
#include "objclass.h"
#include "prop.h"
#include "skills.h"

#else	/* !OBJECTS_PASS_2_ */
/* second pass */
#include "color.h"
#include "monflag.h"
#  define COLOR_FIELD(X) X,
#endif	/* !OBJECTS_PASS_2_ */


/* objects have symbols: ) [ = " ( % ! ? + / $ * ` 0 _ . */

/*
 *	Note:  () and BITS() macros are used to avoid exceeding argument
 *	limits imposed by some compilers.  The ctnr field of BITS currently
 *	does not map into struct objclass, and is ignored in the expansion.
 *	The 0 in the expansion corresponds to oc_pre_discovered, which is
 *	set at run-time during role-specific character initialization.
 */

#ifndef OBJECTS_PASS_2_
/* first pass -- object descriptive text */
# define OBJECT(names,bits,prp,sym,prob,dly,wt,cost,sdam,ldam,oc1,oc2,oc3,nut,color,...) \
	{SETNAMES(names)}

NEARDATA struct objdescr obj_descr[] = {
#else
/* second pass -- object definitions */

# define BITS(nmkn,mrg,uskn,ctnr,mgc,chrg,uniq,nwsh,size,dexc,tuf,dir,dtyp,sub,mtrl,shwmat) \
	nmkn, mrg, uskn, 0, \
	mgc, chrg, uniq, nwsh, \
	size, dexc, tuf, dir, \
	dtyp, mtrl, shwmat, sub
# define OBJECT(names,bits,prp,sym,prob,dly,wt,cost,sdam,ldam,oc1,oc2,oc3,nut,color,...) \
	{0, 0, (char *)0, SET29( \
	bits, \
	prp, sym, dly, color, \
	prob, wt, cost, \
	sdam, ldam, \
	oc1, oc2, oc3, nut, \
	__VA_ARGS__ \
	)}

# ifndef lint
#  define HARDGEM(n) (n >= 8)
# else
#  define HARDGEM(n) (0)
# endif

NEARDATA struct objclass objects[] = {
#endif
/* dummy object[0] -- description [2nd arg] *must* be NULL */
	OBJECT(("strange object",(char *)0), BITS(1,0,0,0,0,0,0,0,0,0,0,0,0,P_NONE,0,0),
			0, ILLOBJ_CLASS, 0, 0, 0, 0, {0}, {0}, 0, 0, 0, 0, 0),

/* weapons ... */
#define WEAPON(names,sdam,ldam,kn,mg,size,prob,wt,cost,hitbon,typ,sub,metal,shwmat,color,...) \
	OBJECT( \
		names, BITS(kn,mg,1,0,0,1,0,0,size,0,0,0,typ,sub,metal,shwmat), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, WP_GENERIC, 0, wt, color, __VA_ARGS__ )
#define PROJECTILE(names,sdam,ldam,kn,prob,wt,cost,hitbon,metal,sub,color,...) \
	OBJECT( \
		names, \
		BITS(kn,1,1,0,0,1,0,0,MZ_TINY,0,0,0,PIERCE,sub,metal,0), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, WP_GENERIC, 0, wt, color, __VA_ARGS__)
#define BOW(names,kn,size,prob,wt,cost,hitbon,metal,sub,color) \
	OBJECT( \
		names, BITS(kn,0,1,0,0,1,0,0,size,0,0,0,0,sub,metal,0), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, { 0 }, { 0 }, hitbon, WP_GENERIC, 0, wt, color)
#define BULLET(names,sdam,ldam,kn,size,prob,wt,cost,hitbon,ammotyp,typ,metal,sub,color,...) \
	OBJECT( \
		names, BITS(kn,1,1,0,0,1,0,0,size,0,0,0,typ,sub,metal,0), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, ammotyp, 0, wt, color, __VA_ARGS__)
#define GUN(names,kn,size,prob,wt,cost,range,rof,hitbon,ammotyp,metal,sub,color,...) \
	OBJECT( \
		names, BITS(kn,0,1,0,0,1,0,0,size,0,0,0,0,sub,metal,0), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, DMG(F(range)), DMG(F(rof)), hitbon, ammotyp, 0, wt, color, __VA_ARGS__)

/* Note: for weapons that don't do an even die of damage (ex. 2-7 or 3-18)
 * the extra damage is added on in weapon.c, not here! */

#define P PIERCE
#define S SLASH
#define B WHACK
#define E EXPLOSION

/* missiles */
PROJECTILE(("arrow"),
		DMG(D(6)), DMG(D(6)),
		1, 50, 1, 2,  0, IRON,   -P_BOW, HI_METAL),
PROJECTILE(("elven arrow", "runed arrow"),
		DMG(D(7)), DMG(D(5)),
		0, 18, 1, 2,  2, WOOD,   -P_BOW, HI_WOOD),
PROJECTILE(("orcish arrow", "crude arrow"),
		DMG(D(5)), DMG(D(8)),
		0, 18, 1, 2, -1, IRON,   -P_BOW, CLR_BLACK),
PROJECTILE(("silver arrow"),
		DMG(D(6)), DMG(D(6)),
		1, 12, 1, 5,  0, SILVER, -P_BOW, HI_SILVER),
PROJECTILE(("golden arrow"), /*Needs encyc entry*//*Needs tile*/
		DMG(D(13)), DMG(D(13)),
		1,  9, 2,10,  0, GOLD,   -P_BOW, HI_GOLD),
PROJECTILE(("ancient arrow"), /*Needs encyc entry*//*Needs tile*/
		DMG(D(10)), DMG(D(10)),
		1,  0, 1,10,  0, METAL,  -P_BOW, CLR_BLACK),
PROJECTILE(("ya", "bamboo arrow"),
		DMG(D(7)), DMG(D(7)),
		0, 15, 1, 4,  1, METAL,  -P_BOW, HI_METAL),
PROJECTILE(("crossbow bolt"),
		DMG(D(4), F(1)), DMG(D(6), F(1)),
		1, 55, 1, 2,  0, IRON,   -P_CROSSBOW, HI_METAL),
PROJECTILE(("droven bolt", "crossbow bolt"), /*Needs encyc entry*/
		DMG(D(9), F(1)), DMG(D(6), F(1)),
		0,  0, 1, 2,  2, OBSIDIAN_MT, -P_CROSSBOW, CLR_BLACK, O_MATSPEC(UNIDED)),

WEAPON(("dart"),
	DMG(D(3)), DMG(D(2)),
	1, 1,   MZ_TINY, 58,  1,  2,  0, P,   -P_DART, IRON, FALSE, HI_METAL),
WEAPON(("shuriken", "throwing star"),
	DMG(D(8)), DMG(D(6)),
	0, 1,   MZ_TINY, 33,  1,  5,  2, P|S, -P_SHURIKEN, IRON, FALSE, HI_METAL),
WEAPON(("boomerang"),
	DMG(D(9)), DMG(D(9)),
	1, 1,  MZ_SMALL, 13,  5, 20,  0, B,   -P_BOOMERANG, WOOD, FALSE, HI_WOOD),
WEAPON(("chakram", "circular blade"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(9)), DMG(D(9)),
	1, 1,  MZ_SMALL,  6,  5, 20,  0, S,   -P_BOOMERANG, SILVER, IDED|UNIDED, HI_SILVER),
WEAPON(("spike"), /*Needs encyc entry*/
	DMG(D(3)), DMG(D(1)),
	1, 1,   MZ_TINY,  0,  1,  2,  0, P,   -P_DART, BONE, IDED|UNIDED, CLR_WHITE),

/* spears */
WEAPON(("spear"),
	DMG(D(8)), DMG(D(10)),
	1, 0,  MZ_LARGE, 45, 25,  3,  0, P,   P_SPEAR, IRON, FALSE, HI_METAL),
WEAPON(("atgeir", "bladed spear"),
	DMG(D(2,4)), DMG(D(12)),
	0, 0,  MZ_LARGE,  5, 45, 15,  0, P|S, P_SPEAR, IRON, FALSE, HI_METAL),
WEAPON(("elven spear", "runed spear"),
	DMG(D(9)), DMG(D(9)),
	0, 0,  MZ_LARGE, 10, 10,  3,  2, P,   P_SPEAR, WOOD, FALSE, HI_WOOD),
WEAPON(("droven spear", "long spear"), /*Needs encyc entry*/
	DMG(D(12)), DMG(D(12)),
	0, 0,   MZ_HUGE,  0, 15,  3,  2, P,   P_SPEAR, OBSIDIAN_MT, UNIDED, CLR_BLACK),
WEAPON(("orcish spear", "crude spear"),
	DMG(D(6)), DMG(D(12)),
	0, 0,  MZ_LARGE, 13, 25,  3, -1, P,   P_SPEAR, IRON, FALSE, CLR_BLACK),
WEAPON(("dwarvish spear", "stout spear"),
	DMG(D(10)), DMG(D(10)),
	0, 0,  MZ_LARGE, 12, 30,  3,  0, P,   P_SPEAR, IRON, FALSE, HI_METAL),
WEAPON(("javelin", "throwing spear"),
	DMG(D(8)), DMG(D(8)),
	0, 1,  MZ_LARGE, 10, 20,  3,  0, P,   P_SPEAR, IRON, FALSE, HI_METAL),

WEAPON(("trident"), /*Needs encyc entry*/
	DMG(D(8), F(1)), DMG(D(3, 6)),
	1, 0,  MZ_LARGE,  8, 25,  5,  0, P,   P_TRIDENT, IRON, FALSE, HI_METAL),

/* blades */
WEAPON(("dagger"),
	DMG(D(4)), DMG(D(3)),
	1, 1,  MZ_SMALL, 24, 10,  4,  2, P,   P_DAGGER, IRON, FALSE, HI_METAL),
WEAPON(("elven dagger", "runed dagger"),
	DMG(D(5)), DMG(D(3)),
	0, 1,  MZ_SMALL,  7,  3,  4,  4, P,   P_DAGGER, WOOD, FALSE, HI_WOOD),
WEAPON(("droven dagger", "dagger"), /*Needs encyc entry*/
	DMG(D(8)), DMG(D(6)),
	0, 1,  MZ_SMALL,  0,  5, 12,  4, P,   P_DAGGER, OBSIDIAN_MT, UNIDED, CLR_BLACK),
WEAPON(("orcish dagger", "crude dagger"),
	DMG(D(3)), DMG(D(5)),
	0, 1,  MZ_SMALL,  9, 10,  5,  1, P,   P_DAGGER, IRON, FALSE, CLR_BLACK),
WEAPON(("athame"),
	DMG(D(4)), DMG(D(4)),
	1, 1,  MZ_SMALL,  0, 10,  4,  2, S,   P_DAGGER, IRON, FALSE, HI_METAL),
WEAPON(("set of crow talons", "set of three feather-etched daggers"),
	DMG(D(4)), DMG(D(3)),
	0, 0,  MZ_SMALL,  0,  9,200,  2, S,   P_DAGGER, METAL, FALSE, HI_METAL),
WEAPON(("tecpatl", "notched dagger"),
	DMG(D(8)), DMG(D(6)),
	0, 1,  MZ_SMALL,  0,  5, 12,  4, P,   P_DAGGER, OBSIDIAN_MT, FALSE, CLR_BLACK),
WEAPON(("scalpel"),
	DMG(D(3)), DMG(D(1)),
	1, 1,  MZ_SMALL,  0,  5,  6,  3, S,   P_KNIFE, METAL, FALSE, HI_METAL),
WEAPON(("knife"),
	DMG(D(5)), DMG(D(3)),
	1, 1,  MZ_SMALL, 14,  5,  4,  2, P|S, P_KNIFE, IRON, FALSE, HI_METAL),
WEAPON(("stiletto"),
	DMG(D(6)), DMG(D(2)),
	1, 1,  MZ_SMALL,  4,  5,  4,  1, P, P_KNIFE, IRON, FALSE, HI_METAL),
WEAPON(("worm tooth"),
	DMG(D(2)), DMG(D(2)),
	1, 0,  MZ_SMALL,  0, 20,  2,  0, P,   P_KNIFE, MINERAL, FALSE, CLR_WHITE),
WEAPON(("crysknife"),
	DMG(D(10)), DMG(D(10)),
	1, 0,  MZ_SMALL,  0, 20,100,  3, P,   P_KNIFE, MINERAL, FALSE, CLR_WHITE),

WEAPON(("sickle"), /* Vs plants: +6 to hit and double damage */
	DMG(D(4)), DMG(D(1)),
	1, 1,  MZ_SMALL, 22, 20,  4, -2, S,   P_HARVEST, IRON, FALSE, HI_METAL),
WEAPON(("elven sickle", "runed sickle"), /* Vs plants: +6 to hit and double damage *//*Needs tile*/
	DMG(D(6)), DMG(D(3)),
	0, 1,  MZ_SMALL,  0,  5,  4,  0, S,   P_HARVEST, WOOD, FALSE, HI_WOOD),

WEAPON(("axe"),
	DMG(D(6)), DMG(D(4)),
	1, 0, MZ_MEDIUM, 17, 60,  8,  0, S,   P_AXE, IRON, FALSE, HI_METAL),
WEAPON(("diskos", "circular-sawbladed axe"),
	DMG(D(6)), DMG(D(8)),
	0, 0,   MZ_MEDIUM, 5,120, 500,  0, S,   P_AXE, METAL, FALSE, HI_METAL, O_MAGIC(1)),
WEAPON(("battle-axe", "double-bitted axe"),/* was "double-headed" ? */
	DMG(D(8), D(4)), DMG(D(6), D(2,4)),
	0, 0,   MZ_HUGE, 14,120, 40,  0, S,   P_AXE, IRON, FALSE, HI_METAL),
WEAPON(("moon axe", "two-handed axe"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(6)), DMG(D(6)),	/* die size modified by phase of moon */
	0, 0,   MZ_HUGE, 12,160, 40,  0, S,   P_AXE, SILVER, UNIDED, HI_SILVER),

/* swords */
WEAPON(("short sword"),
	DMG(D(6)), DMG(D(8)),
	1, 0,  MZ_SMALL,  8, 30, 10,  0, P,   P_SHORT_SWORD, IRON, FALSE, HI_METAL),
WEAPON(("elven short sword", "runed short sword"),
	DMG(D(7)), DMG(D(7)),
	0, 0,  MZ_SMALL,  2, 10, 10,  2, P,   P_SHORT_SWORD, WOOD, FALSE, HI_WOOD),
WEAPON(("droven short sword", "short sword"), /*Needs encyc entry*/
	DMG(D(9)), DMG(D(9)),
	0, 0,  MZ_SMALL,  0, 15, 10,  2, P,   P_SHORT_SWORD, OBSIDIAN_MT, UNIDED, CLR_BLACK),
WEAPON(("orcish short sword", "crude short sword"),
	DMG(D(5)), DMG(D(10)),
	0, 0,  MZ_SMALL,  3, 30, 10, -1, P,   P_SHORT_SWORD, IRON, FALSE, CLR_BLACK),
WEAPON(("dwarvish short sword", "broad short sword"),
	DMG(D(8)), DMG(D(7)),
	0, 0,  MZ_SMALL,  2, 35, 10,  0, P|S, P_SHORT_SWORD, IRON, FALSE, HI_METAL),
WEAPON(("mirrorblade", "polished short sword"),
	DMG(D(6)), DMG(D(8)),
	0, 0,  MZ_SMALL,  0, 40,100,  0, P,   P_SHORT_SWORD, SILVER, FALSE, HI_SILVER),
WEAPON(("khopesh", "sickle-sword"),
	DMG(D(8)), DMG(D(6)),
	0, 0,  MZ_SMALL,  0, 30, 10,  0, S,   P_SHORT_SWORD, COPPER, FALSE, HI_COPPER),
WEAPON(("wakizashi", "samurai short sword"),
	DMG(D(8)), DMG(D(6)),
	0, 0,  MZ_SMALL,  0, 30, 40,  1, P|S,   P_SHORT_SWORD, IRON, FALSE, HI_METAL),

WEAPON(("scimitar", "curved sword"),
	DMG(D(8)), DMG(D(8)),
	0, 0, MZ_MEDIUM, 14, 40, 15,  0, S,   P_SCIMITAR, IRON, FALSE, HI_METAL),
WEAPON(("high-elven warsword", "runed curved sword"),
	DMG(D(10)), DMG(D(10)),
	0, 0, MZ_MEDIUM,  1, 20,150,  2, S,   P_SCIMITAR, MITHRIL, UNIDED, HI_MITHRIL),
WEAPON(("rapier"),
	DMG(D(6)), DMG(D(4)),
	1, 0, MZ_MEDIUM,  5, 28, 20,  2, P,   P_SABER, METAL, FALSE, HI_METAL),
WEAPON(("saber"),
	DMG(D(8)), DMG(D(8)),
	1, 0, MZ_MEDIUM,  6, 34, 75,  0, S,   P_SABER, SILVER, IDED|UNIDED, HI_SILVER),
WEAPON(("crow quill", "feather-etched rapier"),
	DMG(D(8)), DMG(D(8)),
	0, 0, MZ_MEDIUM,  0, 34,200,  2, P,   P_SABER, METAL, FALSE, HI_METAL),
WEAPON(("rakuyo", "double-bladed saber"),
	DMG(D(8)), DMG(D(8)),
	0, 0, MZ_MEDIUM,  0, 38,500,  2, P|S, P_SABER, METAL, FALSE, HI_METAL, O_MAGIC(1)),
WEAPON(("rakuyo-saber", "latch-pommeled saber"),
	DMG(D(8)), DMG(D(8)),
	0, 0, MZ_MEDIUM,  0, 28,400,  2, P|S, P_SABER, METAL, FALSE, HI_METAL, O_MAGIC(1)),
WEAPON(("rakuyo-dagger", "latch-pommeled dagger"),
	DMG(D(4)), DMG(D(3)),
	0, 0,  MZ_SMALL,  0, 10,100,  2, P|S, P_DAGGER, METAL, FALSE, HI_METAL, O_MAGIC(1)),
WEAPON(("isamusei", "oddly-luminous sword", "razor-thin sword"),
	DMG(D(12)), DMG(D(8)),
	0, 0, MZ_MEDIUM,  1, 100,500,  2, S|B, P_BROAD_SWORD, METAL, FALSE, CLR_ORANGE, O_MAGIC(1)),
WEAPON(("broadsword"),
	DMG(D(2, 4)), DMG(D(6), F(1)),
	1, 0,  MZ_LARGE,  8, 70, 10,  0, S,   P_BROAD_SWORD, IRON, FALSE, HI_METAL),
WEAPON(("elven broadsword", "runed broadsword"),
	DMG(D(6), D(4)), DMG(D(6), F(2)),
	0, 0,  MZ_LARGE,  4, 20, 10,  2, S,   P_BROAD_SWORD, WOOD, FALSE, HI_WOOD),
WEAPON(("long sword"),
	DMG(D(8)), DMG(D(12)),
	1, 0, MZ_MEDIUM, 46, 40, 15,  0, S|P, P_LONG_SWORD, IRON, FALSE, HI_METAL),
WEAPON(("crystal sword"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(2, 8)), DMG(D(2, 12)),
	1, 0,  MZ_LARGE, 2, 120,300,  0, S|P, P_LONG_SWORD, GLASS, FALSE, HI_GLASS),
WEAPON(("two-handed sword"),
	DMG(D(12)), DMG(D(3, 6)),
	1, 0,   MZ_HUGE, 22,150, 50,  0, S,   P_TWO_HANDED_SWORD, IRON, FALSE, HI_METAL),
WEAPON(("droven greatsword", "two-handed sword"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(18)), DMG(D(30)),
	0, 0,   MZ_HUGE,  0,120, 50,  2, S,   P_TWO_HANDED_SWORD, OBSIDIAN_MT, UNIDED, CLR_BLACK),
WEAPON(("katana", "samurai sword"),
	DMG(D(10)), DMG(D(12)),
	0, 0, MZ_MEDIUM,  4, 40, 80,  1, S,   P_LONG_SWORD, IRON, FALSE, HI_METAL),
/* special swords set up for artifacts and future weapons*/
WEAPON(("vibroblade", "gray short sword", "short sword"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(6)), DMG(D(8)),
	1, 0,  MZ_SMALL,  0,  5,1000, 0, P,   P_SHORT_SWORD, PLASTIC, FALSE, CLR_GRAY, O_MAGIC(1)),
WEAPON(("tsurugi", "long samurai sword"),
	DMG(D(16)), DMG(D(8), D(2,6)),
	0, 0,   MZ_HUGE,  0, 60,500,  2, S,   P_TWO_HANDED_SWORD, METAL, FALSE, HI_METAL),
WEAPON(("runesword", "runed black blade", "runed blade"),
	DMG(D(10), D(4)), DMG(D(10), F(1)),
	0, 0,  MZ_LARGE,  0, 40,300,  0, S,   P_BROAD_SWORD, IRON, FALSE, CLR_BLACK),
WEAPON(("white vibrosword", "white sword", "long sword"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(10)), DMG(D(12)),
	0, 0, MZ_MEDIUM,  0, 40,8000, 1, P|S, P_LONG_SWORD,  SILVER, FALSE, CLR_WHITE, O_MAGIC(1)),
WEAPON(("gold-bladed vibrosword", "black and gold sword", "long sword"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(10)), DMG(D(12)),
	0, 0, MZ_MEDIUM,  0, 53,8000, 1, P|S, P_LONG_SWORD,    GOLD, FALSE, CLR_BLACK, O_MAGIC(1)),
WEAPON(("red-eyed vibrosword", "blue-glowing sword", "long sword"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(10)), DMG(D(12)),
	0, 0, MZ_MEDIUM,  0, 10,8000, 1, P|S, P_LONG_SWORD, PLASTIC, FALSE, CLR_GRAY, O_MAGIC(1)),
WEAPON(("white vibrozanbato", "curved white sword", "long curved sword"),
	DMG(D(16)), DMG(D(8), D(2,6)),
	0, 0,   MZ_HUGE,  0, 60,16000,2, S,   P_TWO_HANDED_SWORD, SILVER, FALSE, CLR_WHITE, O_MAGIC(1)),
WEAPON(("gold-bladed vibrozanbato", "curved black and gold sword", "long curved sword"),
	DMG(D(16)), DMG(D(8), D(2,6)),
	0, 0,   MZ_HUGE,  0, 80,16000,2, S,   P_TWO_HANDED_SWORD, GOLD, FALSE, CLR_BLACK, O_MAGIC(1)),

WEAPON(("double force-blade", "double-bladed weapon"),
	DMG(D(6)), DMG(D(4)),
	0, 0,   MZ_HUGE,  0, 40,1000, 2, S,   P_QUARTERSTAFF, PLASTIC, FALSE, CLR_RED, O_MAGIC(1)),
						    /* 2x, but slower */
WEAPON(("force blade", "latch-ended blade"),
	DMG(D(6)), DMG(D(4)),
	0, 0,  MZ_SMALL,  0, 20,500,  2, S,   P_SHORT_SWORD, PLASTIC, FALSE, CLR_RED, O_MAGIC(1)),

WEAPON(("force sword", "hard segmented sword"),
	DMG(D(8)), DMG(D(6)),
	0, 0, MZ_MEDIUM,  0, 40,1000, 2, P|B, P_BROAD_SWORD, PLASTIC, FALSE, HI_SILVER, O_MAGIC(1)),
WEAPON(("force whip", "segmented whip"),
	DMG(D(6)), DMG(D(4)),
	0, 0, MZ_MEDIUM,  0, 40,1000, 2, P|S, P_WHIP, PLASTIC, FALSE, CLR_ORANGE, O_MAGIC(1)),
/* polearms */
/* spear-type */
WEAPON(("partisan", "vulgar polearm"),
	DMG(D(8)), DMG(D(8), F(1)),
	0, 0,   MZ_HUGE,  5, 80, 10,  0, P,   P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("ranseur", "hilted polearm"),
	DMG(D(2, 4)), DMG(D(2, 4)),
	0, 0,   MZ_HUGE,  5, 50,  6,  0, P,   P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("spetum", "forked polearm"),
	DMG(D(6), F(1)), DMG(D(2, 6)),
	0, 0,   MZ_HUGE,  5, 50,  5,  0, P,   P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("glaive", "single-edged polearm"),
	DMG(D(6), F(1)), DMG(D(10), F(1)),
	0, 0,   MZ_HUGE,  7, 75,  6,  0, S,   P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("naginata", "samurai-sword polearm"),
	DMG(D(8), F(1)), DMG(D(10), F(1)),
	0, 0,   MZ_HUGE,  1, 75, 90,  1, S,   P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("lance"),
	DMG(D(6)), DMG(D(8)),
	1, 0,  MZ_LARGE,  4, 80, 10,  0, P,   P_LANCE, IRON, FALSE, HI_METAL),
WEAPON(("force pike", "long gray spear", "long spear"),/*Needs tile*/
	DMG(D(6)), DMG(D(8)),
	0, 0,  MZ_LARGE,  0, 30,1000, 2, P|S, P_LANCE, PLASTIC, FALSE, CLR_GRAY, O_MAGIC(1)),
WEAPON(("white vibrospear", "long white spear", "long spear"),/*Needs tile*/
	DMG(D(6)), DMG(D(8)),
	0, 0,  MZ_LARGE,  0, 30,1000, 2, P|S, P_LANCE, PLASTIC, FALSE, CLR_WHITE, O_MAGIC(1)),
WEAPON(("gold-bladed vibrospear", "long black and gold spear", "long spear"),/*Needs tile*/
	DMG(D(6)), DMG(D(8)),
	0, 0,  MZ_LARGE,  0, 30,1000, 2, P|S, P_LANCE, GOLD, FALSE, CLR_BLACK, O_MAGIC(1)),
WEAPON(("elven lance", "runed lance"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(8)), DMG(D(8)),
	0, 0,  MZ_LARGE,  0, 60, 10,  2, P,   P_LANCE, WOOD, FALSE, HI_WOOD),
WEAPON(("droven lance", "lance"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(12)), DMG(D(12)),
	0, 0,   MZ_HUGE,  0, 60, 10,  2, P,   P_LANCE, OBSIDIAN_MT, UNIDED, CLR_BLACK),
/* axe-type */
WEAPON(("halberd", "angled poleaxe"),
	DMG(D(12)), DMG(D(2, 8)),
	0, 0,   MZ_HUGE,  6, 75, 10, 0, P|S, P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("poleaxe", "straight-bladed poleaxe"),
	DMG(D(10)), DMG(D(2, 6)),
	0, 0,   MZ_HUGE,  2, 75, 10, 0, P|S|B, P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("bardiche", "long-bladed poleaxe"),
	DMG(D(2, 6)), DMG(D(3, 6)),
	0, 0,   MZ_HUGE,  4, 80,  7, 0, S,   P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("voulge", "pole cleaver"),
	DMG(D(2, 4)), DMG(D(2, 4)),
	0, 0,   MZ_HUGE,  4, 50,  5, 0, S,   P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("dwarvish mattock", "broad pick"),
	DMG(D(12)), DMG(D(8), D(2, 6)),
	0, 0,   MZ_HUGE, 13,120, 50,-1, P|B, P_PICK_AXE, IRON, FALSE, HI_METAL),

/* curved/hooked */
WEAPON(("fauchard", "pole sickle"),
	DMG(D(6)), DMG(D(8)),
	0, 0,   MZ_HUGE,  5, 60,  5,  0, P|S, P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("guisarme", "pruning hook"),
	DMG(D(2, 4)), DMG(D(8)),
	0, 0,   MZ_HUGE,  5, 80,  5,  0, S,   P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("bill-guisarme", "hooked polearm"),
	DMG(D(2, 4)), DMG(D(10)),
	0, 0,   MZ_HUGE,  3, 80,  7,  0, P|S, P_POLEARMS, IRON, FALSE, HI_METAL),
/* other */
WEAPON(("lucern hammer", "pronged polearm"),
	DMG(D(2, 8)), DMG(D(8)),
	0, 0,   MZ_HUGE,  4, 85,  7,  0, B|P, P_POLEARMS, IRON, FALSE, HI_METAL),
WEAPON(("bec de corbin", "beaked polearm"),
	DMG(D(8), F(2)), DMG(D(12), F(2)),
	0, 0,   MZ_HUGE,  3, 75,  8,  0, B|P, P_POLEARMS, IRON, FALSE, HI_METAL),

WEAPON(("scythe"), 
	DMG(D(2, 4)), DMG(D(2, 4)), /* Vs plants: +6 to hit and double damage */
	1, 0,   MZ_HUGE,  5, 75,  6, -2, S,   P_HARVEST, IRON, FALSE, HI_METAL),

/* bludgeons */
WEAPON(("mace"),
	DMG(D(10), F(1)), DMG(D(10)),
	1, 0, MZ_MEDIUM, 40, 30,  5,  0, B,   P_MACE, IRON, FALSE, HI_METAL),
WEAPON(("elven mace", "runed mace"), /*Needs encyc entry*/
	DMG(D(7), F(3)), DMG(D(7)),
	0, 0, MZ_MEDIUM,  0, 10,  5,  2, B,   P_MACE, WOOD, FALSE, HI_WOOD),
WEAPON(("morning star"),
	DMG(D(2, 6)), DMG(D(8), F(1)),
	1, 0, MZ_MEDIUM, 12, 80, 10,  0, B|P, P_MORNING_STAR, IRON, FALSE, HI_METAL),
WEAPON(("war hammer"),
	DMG(D(8), F(1)), DMG(D(8)),
	1, 0, MZ_MEDIUM, 15, 50,  5,  0, B,   P_HAMMER, IRON, FALSE, HI_METAL),
WEAPON(("club"),
	DMG(D(6)), DMG(D(3)),
	1, 0, MZ_MEDIUM, 10, 10,  3,  0, B,   P_CLUB, WOOD, FALSE, HI_WOOD),
WEAPON(("clawed hand"),
	DMG(D(12)), DMG(D(6)),
	1, 0, MZ_MEDIUM,  0, 10, 300, 0, P|S, P_CLUB, BONE, FALSE, CLR_GRAY),
WEAPON(("macuahuitl", "obsidian-edged club"),
	DMG(D(8)), DMG(D(6)),
	0, 0, MZ_MEDIUM,  0, 40, 10,  0, B|S, P_CLUB, WOOD, FALSE, HI_WOOD),
WEAPON(("breaking wheel", "wagon wheel"),
	DMG(D(6)), DMG(D(3)),
	0, 0, MZ_HUGE,  1, 150,  500,  0, B,   P_NONE, WOOD, FALSE, HI_WOOD),
WEAPON(("quarterstaff", "staff"),
	DMG(D(6)), DMG(D(6)),
	0, 0,   MZ_HUGE,  9, 40,  5,  0, B,   P_QUARTERSTAFF, WOOD, FALSE, HI_WOOD),
WEAPON(("khakkhara", "monk's staff"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(6)), DMG(D(4)),
	0, 0,   MZ_HUGE,  2, 80, 50,  0, B|P, P_QUARTERSTAFF, SILVER, IDED, HI_SILVER),
WEAPON(("double sword"),
	DMG(D(8)), DMG(D(12)),
	1, 0,   MZ_HUGE,  1, 80, 30,  0, S|P, P_QUARTERSTAFF, IRON, FALSE, HI_METAL),
WEAPON(("kamerel vajra", "short mace"), /*Needs encyc entry*/
	DMG(D(6)), DMG(D(6)),	/* very different dice for different litness states */
	0, 0, MZ_MEDIUM,  0, 10,800,  1, S|E, P_MACE, GOLD, UNIDED, HI_GOLD),
WEAPON(("bar"),
	DMG(D(8)), DMG(D(6)),
	1, 0,   MZ_HUGE, 0, 400, 10,-10, B,   P_QUARTERSTAFF, IRON, IDED|UNIDED, HI_METAL),
/* two-piece */
WEAPON(("aklys", "thonged club"),
	DMG(D(6)), DMG(D(3)),
	0, 0, MZ_MEDIUM,  8, 15,  4,  0, B,   P_CLUB, IRON, FALSE, HI_METAL),
WEAPON(("flail"),
	DMG(D(6), F(1)), DMG(D(2, 4)),
	1, 0, MZ_MEDIUM, 25, 15,  4,  0, B,   P_FLAIL, IRON, FALSE, HI_METAL),
WEAPON(("nunchaku", "set of rope-sticks"),
	DMG(D(4), F(1)), DMG(D(3)),
	0, 0, MZ_MEDIUM,  5,  7,  4,  0, B,   P_FLAIL, WOOD, FALSE, HI_WOOD),
/* misc */
WEAPON(("bullwhip"),
	DMG(D(2)), DMG(D(1)),
	1, 0, MZ_MEDIUM,  5, 10,  4,  0, B,   P_WHIP, LEATHER, FALSE, CLR_BROWN),
WEAPON(("viperwhip"),
	DMG(D(4)), DMG(D(3)),
	1, 0, MZ_MEDIUM,  2, 30, 40,  2, P,   P_WHIP, SILVER, IDED|UNIDED, HI_SILVER),

WEAPON(("bestial claw"),
	DMG(D(10)), DMG(D(8)),
	1, 0, MZ_MEDIUM,  0, 10,100,  0, S|P, P_BARE_HANDED_COMBAT, BONE, FALSE, CLR_WHITE, O_MAGIC(1)),
WEAPON(("katar"),
	DMG(D(6)), DMG(D(4)),
	1, 0, MZ_SMALL,  5,  5,   4,  0, S|P, P_BARE_HANDED_COMBAT, IRON, FALSE, HI_METAL),



/* Firearms */
//ifdef FIREARMS
 /*Needs encyc entry*/
GUN(("flintlock", "broken hand-crossbow"),            0,   MZ_LARGE, 0,  10,   50,  8, -2, -2, WP_BULLET, IRON, P_FIREARM, HI_METAL), /*Needs tile*/

GUN(("pistol", "broken hand-crossbow"),               0,   MZ_SMALL, 0,  12,  100, 15,  1,  2, WP_BULLET, IRON, P_FIREARM, HI_METAL), /*Needs tile*/
GUN(("submachine gun", "strange broken crossbow"),    0,   MZ_SMALL, 0,  25,  250, 10,  3,  0, WP_BULLET, IRON, P_FIREARM, HI_METAL), /*Needs tile*/
GUN(("heavy machine gun", "strange broken crossbow"), 0,    MZ_HUGE, 0, 100, 2000, 20,  8, -4, WP_BULLET, IRON, P_FIREARM, HI_METAL), /*Needs tile*/
GUN(("rifle", "broken crossbow"),                     0,    MZ_HUGE, 0,  30,  150, 22, -1,  6, WP_BULLET, IRON, P_FIREARM, HI_METAL), /*Needs tile*/
GUN(("assault rifle", "broken crossbow"),             0,  MZ_MEDIUM, 0,  40, 1000, 20,  5,  2, WP_BULLET, IRON, P_FIREARM, HI_METAL), /*Needs tile*/
GUN(("sniper rifle", "broken crossbow"),              0,    MZ_HUGE, 0,  50, 4000, 25, -3,  4, WP_BULLET, IRON, P_FIREARM, HI_METAL), /*Needs tile*/
GUN(("shotgun", "broken crossbow"),                   0,  MZ_MEDIUM, 0,  35,  200,  3, -1, 20,  WP_SHELL, IRON, P_FIREARM, HI_METAL), /*Needs tile*/
GUN(("auto shotgun", "strange broken crossbow"),      0,    MZ_HUGE, 0,  60, 1500,  3,  2, 10,  WP_SHELL, IRON, P_FIREARM, HI_METAL), /*Needs tile*/
GUN(("rocket launcher", "metal tube"),                0,    MZ_HUGE, 0, 100, 3500, 20, -5, -4, WP_ROCKET, IRON, P_FIREARM, HI_METAL), /*Needs tile*/
GUN(("grenade launcher", "strange broken crossbow"),  0,   MZ_LARGE, 0,  55, 1500,  6, -3,  0,WP_GRENADE, IRON, P_FIREARM, HI_METAL), /*Needs tile*/

GUN(("BFG", "ovoid device"),                          0,    MZ_HUGE, 0, 100, 3000,100,  3,  0,WP_ROCKET|WP_GRENADE|WP_BULLET|WP_SHELL|WP_ROCKET,
                                                                                                SILVER, P_FIREARM, HI_SILVER), /*Needs tile*/
GUN(("handgun", "unfamiliar handgun"),                0,   MZ_SMALL, 0,  12,  100, 15,  1,  0, WP_BULLET, IRON, P_FIREARM, HI_METAL),/*Needs tile*/
GUN(("gun", "unfamiliar gun"),                        0,  MZ_MEDIUM, 0,  25,  250, 10,  3, -1, WP_BULLET, IRON, P_FIREARM, HI_METAL),/*Needs tile*/
GUN(("long gun", "unfamiliar long gun"),              0,    MZ_HUGE, 0,  30,  150, 22, -1,  1, WP_BULLET, IRON, P_FIREARM, HI_METAL),/*Needs tile*/
GUN(("heavy gun", "unfamiliar heavy gun"),            0,    MZ_HUGE, 0, 100, 2000, 20,  8, -4, WP_BULLET, IRON, P_FIREARM, HI_METAL),/*Needs tile*/
 /*Needs encyc entry*/
GUN(("hand blaster", "hard black handmirror", "hard handmirror"), 0,  MZ_MEDIUM, 0,   2, 1000, 10,  1,  0,WP_BLASTER, PLASTIC, P_FIREARM, CLR_BLACK, O_MAGIC(1)), /*Needs tile*/
GUN(("arm blaster",  "hard white bracer", "hard bracer"),         0,   MZ_LARGE, 0,   8, 4500, 15,  6,  0,WP_BLASTER, PLASTIC, P_FIREARM, CLR_WHITE, O_MAGIC(1)), /*Needs tile*/
GUN(("mass-shadow pistol",  "rectangular device"),                0,  MZ_MEDIUM, 0,   4, 4500, 10,  1,  0,WP_BLASTER, PLASTIC, P_FIREARM, CLR_GRAY, O_MAGIC(1)), /*Needs tile*/
GUN(("cutting laser","hard tan lozenge", "hard lozenge"),         0,   MZ_SMALL, 0,   1, 1000,  3, -1,  3,WP_BLASTER, PLASTIC, P_FIREARM, CLR_YELLOW, O_MAGIC(1)), /*Needs tile*/

GUN(("raygun", "hard handle ending in glassy disks"), 0,  MZ_MEDIUM, 0,   8, 3000, 15,  1,  0,WP_BLASTER, PLASTIC, P_FIREARM, CLR_BRIGHT_CYAN, O_MAGIC(1)), /*Needs tile*/
BULLET(("bullet", "pellet"),
	DMG(D(2, 8), F(4)), DMG(D(2, 6), F(4)),
	0,    MZ_TINY, 0,  1,   5, 0,  WP_BULLET,   P,   METAL, -P_FIREARM, HI_METAL),/*Needs tile*/
BULLET(("silver bullet", "silver pellet"),
	DMG(D(2, 8), F(4)), DMG(D(2, 6), F(4)),
	0,    MZ_TINY, 0,  1,  15, 0,  WP_BULLET,   P,  SILVER, -P_FIREARM, HI_SILVER),/*Needs tile*/
BULLET(("shotgun shell", "red tube", "tube"),
	DMG(D(2, 12), F(4)), DMG(D(2, 6), F(4)),
	0,    MZ_TINY, 0,  1,  10,10,   WP_SHELL,   S,   METAL, -P_FIREARM, CLR_RED),/*Needs tile*/
BULLET(("frag grenade", "green spheroid", "spheroid"),
	DMG(D(2)), DMG(D(2)),
	0,   MZ_SMALL, 0,  5, 350, 0, WP_GRENADE,   B,    IRON, -P_FIREARM, CLR_GREEN),/*Needs tile*/
BULLET(("gas grenade", "lime spheroid", "spheroid"),
	DMG(D(2)), DMG(D(2)),
	0,   MZ_SMALL, 0,  2, 350, 0, WP_GRENADE,   B,    IRON, -P_FIREARM, CLR_BRIGHT_GREEN),/*Needs tile*/
BULLET(("rocket", "firework"),
	DMG(D(2, 12), F(4)), DMG(D(2, 20), F(4)),
	0,   MZ_SMALL, 0, 20, 450, 0,  WP_ROCKET,   B,  SILVER, -P_FIREARM, CLR_BLUE),/*Needs tile*/
BULLET(("stick of dynamite", "red stick", "stick"),
	DMG(D(1)), DMG(D(1)),
	0,   MZ_SMALL, 0, 10, 150, 0, WP_GENERIC,   B, PLASTIC,     P_NONE, CLR_RED),/*Needs tile*/

BULLET(("blaster bolt", "ruby bolt", "bolt"),
	DMG(D(3, 6), F(6)), DMG(D(3, 8), F(8)),
	0,    MZ_TINY, 0,  1,   0, 0, WP_BLASTER,   E,   METAL, -P_FIREARM, CLR_RED),/*Needs tile*/
BULLET(("heavy blaster bolt", "scarlet bolt", "bolt"),
	DMG(D(3, 10), F(10)), DMG(D(3, 12), F(12)),
	0,    MZ_TINY, 0,  1,   0, 0, WP_BLASTER,   E,   METAL, -P_FIREARM, CLR_ORANGE),/*Needs tile*/
BULLET(("laser beam", "green bolt", "bolt"),
	DMG(D(3, 1), F(10)), DMG(D(3, 1), F(10)),
	0,    MZ_TINY, 0,  1,   0, 0, WP_BLASTER,   E|S, METAL, -P_FIREARM, CLR_BRIGHT_GREEN),/*Needs tile*/
//endif

/* bows */
BOW(("bow"),                                          1,  MZ_LARGE, 24, 30,  60,  0, WOOD, P_BOW, HI_WOOD),
BOW(("elven bow", "runed bow"),                       0,  MZ_LARGE, 12, 20,  60,  2, WOOD, P_BOW, HI_WOOD),
BOW(("orcish bow", "crude bow"),                      0,  MZ_LARGE, 12, 30,  60, -2, WOOD, P_BOW, CLR_BLACK),
BOW(("yumi", "long bow"),                             0,  MZ_LARGE,  0, 30,  60,  0, WOOD, P_BOW, HI_WOOD),
BOW(("sling"),                                        1, MZ_MEDIUM, 40,  3,  20, -1, LEATHER, P_SLING, HI_LEATHER),
BOW(("crossbow"),                                     1,  MZ_LARGE, 45, 50,  40,  1, WOOD, P_CROSSBOW, HI_WOOD),
BOW(("droven crossbow", "spider-emblemed crossbow"),  0,  MZ_LARGE,  0, 50, 120,  4, SILVER, P_CROSSBOW, CLR_BLACK), /*Needs encyc entry*/
BOW(("atlatl", "notched stick"),                      0, MZ_MEDIUM,  0, 12,  30,  0, WOOD, P_SPEAR, HI_WOOD), /*Needs encyc entry*/

#undef WEAPON
#undef PROJECTILE
#undef BOW
#undef BULLET
#undef GUN

/* armor ... */
/* IRON denotes ferrous metals, including steel.
 * Only IRON weapons and armor can rust.
 * Only COPPER (including brass) corrodes.
 * Some creatures are vulnerable to SILVER.
 */
#define ARMOR(names,kn,mgc,size,dexc,power,prob,delay,wt,cost,ac,dr,can,drslot,sub,metal,c,...) \
	OBJECT( \
		names, BITS(kn,0,1,0,mgc,1,0,0,size,dexc,0,0,drslot,sub,metal,0), power, \
		ARMOR_CLASS, prob, delay, wt, cost, \
		{0}, {0}, 10 - ac, can, dr, wt, c, __VA_ARGS__ )

#define SUIT(names,kn,mgc,size,dexc,power,prob,delay,wt,cost,ac,dr,can,metal,c,...) \
	ARMOR(names, kn, mgc, size, dexc, power, prob, delay, wt, cost, ac, dr, can, TORSO_DR, ARM_SUIT, metal, c, __VA_ARGS__)
#define SHIRT(names,kn,mgc,size,power,prob,delay,wt,cost,ac,dr,can,metal,c,...) \
	ARMOR(names, kn, mgc, size, 0, power, prob, delay, wt, cost, ac, dr, can, UPPER_TORSO_DR, ARM_SHIRT, metal, c, __VA_ARGS__)
#define HELM(names,kn,mgc,power,prob,delay,wt,cost,ac,dr,can,metal,c,...) \
	ARMOR(names, kn, mgc, MZ_SMALL, 0, power, prob, delay, wt, cost, ac, dr, can, HEAD_DR, ARM_HELM, metal, c, __VA_ARGS__)
#define CLOAK(names,kn,mgc,power,prob,delay,wt,cost,ac,dr,can,metal,c,...) \
	ARMOR(names, kn, mgc, MZ_MEDIUM, 0, power, prob, delay, wt, cost, ac, dr, can, CLOAK_DR, ARM_CLOAK, metal, c, __VA_ARGS__)
#define SHIELD(names,kn,mgc,size,power,prob,delay,wt,cost,ac,dr,can,metal,c,...) \
	ARMOR(names, kn, mgc, size, 0, power, prob, delay, wt, cost, ac, dr, can, 0, ARM_SHIELD, metal, c, __VA_ARGS__)
#define GLOVES(names,kn,mgc,power,prob,delay,wt,cost,ac,dr,can,metal,c,...) \
	ARMOR(names, kn, mgc, MZ_SMALL, 0, power, prob, delay, wt, cost, ac, dr, can, ARM_DR, ARM_GLOVES, metal, c, __VA_ARGS__)
#define BOOTS(names,kn,mgc,power,prob,delay,wt,cost,ac,dr,can,metal,c,...) \
	ARMOR(names, kn, mgc, MZ_SMALL, 0, power, prob, delay, wt, cost, ac, dr, can, LEG_DR, ARM_BOOTS, metal, c, __VA_ARGS__)

/* helmets */
HELM(("sedge hat", "wide conical hat"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	3, 1,  3,   8,10, 0, 1, VEGGY, CLR_YELLOW),
HELM(("wide hat", "wide-brimmed hat"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	0, 1,  3,   8,10, 0, 1, CLOTH, CLR_WHITE),
HELM(("leather helm", "leather hat"),
		0, 0,  0,	5, 1,  5,   8,10, 1, 0, LEATHER, HI_LEATHER),
HELM(("orcish helm", "skull cap"),
		0, 0,  0,	5, 1, 30,  10, 9, 0, 0, IRON, CLR_BLACK),
HELM(("dwarvish helm", "hard hat"),
		0, 0,  0,	5, 1, 40,  20, 9, 1, 0, IRON, HI_METAL),
HELM(("gnomish pointy hat", "conical hat"),
		0, 0,  0,	0, 1,  3,   2,10, 0, 0, CLOTH, CLR_RED),
HELM(("fedora"),
		1, 0,  0,	0, 0,  3,   1,10, 0, 0, CLOTH, CLR_BROWN),
HELM(("cornuthaum", "conical hat"),
		0, 1,  CLAIRVOYANT,
					3, 1,  4,  80,10, 0, 2, CLOTH, CLR_BLUE),
HELM(("witch hat", "wide-brimmed conical hat"),
		0, 1,  0,   0, 1,  4,  80,10, 0, 2, CLOTH, CLR_BLACK),
HELM(("dunce cap", "conical hat"),
		0, 1,  0,	3, 1,  4,   1, 10, 0, 0, CLOTH, CLR_BLUE),
HELM(("war hat", "wide helm"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	2, 0, 60,  30, 8, 4, 2, IRON, HI_METAL),
HELM(("flack helmet", "green bowl", "bowl"), /*Needs encyc entry*/
		0, 0,  0,	0, 0, 10,  50, 9, 3, 1, PLASTIC, CLR_GREEN),
HELM(("archaic helm", "helmet"),
		0, 0,  0,   0, 1, 30,  12, 9, 3, 0, COPPER, HI_COPPER),
HELM(("harmonium helm", "red-lacquered spined helm", "spined helm"),
		0, 0,  0,   0, 1, 45,   1, 9, 4, 0, METAL, CLR_RED),
HELM(("elven helm", "runed helm"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	0, 1, 10,   5, 9, 3, 0, WOOD, HI_WOOD),
HELM(("high-elven helm", "runed helm"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	0, 1, 15,   5, 8, 4, 0, MITHRIL, HI_MITHRIL, O_MATSPEC(UNIDED)),
HELM(("droven helm", "spider-shaped helm"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	0, 1, 20,   5, 8, 4, 0, SHADOWSTEEL, CLR_BLACK),
HELM(("plasteel helm", "white skull helm", "skull helm"), /*Needs encyc entry*//*Needs tile*/
		0, 1,  INFRAVISION,   0, 2, 25,  50, 8, 5, 2, PLASTIC, CLR_WHITE),
HELM(("crystal helm", "fish bowl"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,   0, 2,150, 300, 8, 5, 0, GLASS, HI_GLASS, O_MATSPEC(UNIDED)),
HELM(("pontiff's crown", "filigreed faceless helm"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,   0, 2, 90, 300, 8, 5, 0, GOLD, HI_GOLD, O_MATSPEC(IDED)),
HELM(("shemagh", "headscarf"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	0, 0, 5,   5, 10, 0, 0, CLOTH, CLR_WHITE, O_MATSPEC(UNIDED)),
HELM(("faceless helm"), /*Needs encyc entry*//*Needs tile*/
		1, 0,  0,   0, 2, 90, 300, 8, 5, 0, BONE, CLR_GRAY, O_MATSPEC(IDED)),

/* With shuffled appearances... */
HELM(("helmet", "plumed helmet"), /* circlet */
		0, 0,  0,         8, 1, 30,  10, 9, 3, 0, IRON, HI_METAL),
HELM(("helm of brilliance", "etched helmet"), /* crown of cognizance */
		0, 1,  0,	      6, 1, 30,  50, 9, 1, 0, IRON, HI_METAL),
HELM(("helm of opposite alignment", "crested helmet"), /* tiara of treachery */
		0, 1,  0,	      6, 1, 30,  50, 9, 1, 0, IRON, HI_METAL),
HELM(("helm of telepathy", "visored helmet"), /*tiara of telepathy*/ /*Note: 'visored' appearance gives +1 AC and DR*/
		0, 1,  TELEPAT,   2, 1, 30,  50, 9, 1, 0, IRON, HI_METAL),
HELM(("helm of drain resistance", "band"), /*diadem of drain resistance*//*Needs tile*//*Note: 'band' appearance halves AC and DR after adjustments for errosion and material (rounds down) */
		0, 1,  DRAIN_RES, 2, 1, 30,  50, 9, 1, 0, GOLD, HI_GOLD),

/* suits of armor */
/*
 * There is code in polyself.c that assumes (1) and (2).
 * There is code in obj.h, objnam.c, mon.c, read.c that assumes (2).
 *
 *	(1) The dragon scale mails and the dragon scales are together.
 *	(2) That the order of the dragon scale mail and dragon scales is the
 *	    the same defined in monst.c.
 */
#define DRGN_MAIL(names,mgc,power,cost,ac,dr,color,...) \
	SUIT(DEF_BLINDNAME(names, "dragon scale mail"),1,mgc, MZ_LARGE, ARMSZ_MEDIUM,power,0,5,150,cost,ac,dr,0,DRAGON_HIDE,color,__VA_ARGS__)
/* 3.4.1: dragon scale mail reclassified as "magic" since magic is
   needed to create them */
DRGN_MAIL(("gray dragon scale mail"),   1, ANTIMAGIC,  1200, 7, 5, CLR_GRAY),
DRGN_MAIL(("silver dragon scale mail"), 1, REFLECTING, 1200, 7, 5, DRAGON_SILVER),
DRGN_MAIL(("shimmering dragon scale mail"), 1, DISPLACED, 1200, 7, 5, CLR_CYAN),
DRGN_MAIL(("red dragon scale mail"),    1, FIRE_RES,    900, 7, 5, CLR_RED),
DRGN_MAIL(("white dragon scale mail"),  1, COLD_RES,    900, 7, 5, CLR_WHITE),
DRGN_MAIL(("orange dragon scale mail"), 1, FREE_ACTION,   900, 7, 5, CLR_ORANGE),
DRGN_MAIL(("black dragon scale mail"),  1, DISINT_RES, 1200, 7, 5, CLR_BLACK),
DRGN_MAIL(("blue dragon scale mail"),   1, SHOCK_RES,   900, 7, 5, CLR_BLUE),
DRGN_MAIL(("green dragon scale mail"),  1, POISON_RES,  900, 7, 5, CLR_GREEN),
DRGN_MAIL(("yellow dragon scale mail"), 1, ACID_RES,    900, 7, 5, CLR_YELLOW),
#undef DRGN_MAIL

#define DRGN_SCALES(names,mgc,power,cost,ac,dr,color,...) \
	SUIT(DEF_BLINDNAME(names, "dragon scales"),1,mgc, MZ_LARGE, ARMSZ_MEDIUM,power,0,5,150,cost,ac,dr,0,DRAGON_HIDE,color,__VA_ARGS__)
/* For now, only dragons leave these. */
/* 3.4.1: dragon scales left classified as "non-magic"; they confer
   magical properties but are produced "naturally" */
DRGN_SCALES(("gray dragon scales"),   0, ANTIMAGIC,  700, 9, 2, CLR_GRAY),
DRGN_SCALES(("silver dragon scales"), 0, REFLECTING, 700, 9, 2, DRAGON_SILVER),
DRGN_SCALES(("shimmering dragon scales"), 0, DISPLACED,  700, 9, 2, CLR_CYAN),
DRGN_SCALES(("red dragon scales"),    0, FIRE_RES,   500, 9, 2, CLR_RED),
DRGN_SCALES(("white dragon scales"),  0, COLD_RES,   500, 9, 2, CLR_WHITE),
DRGN_SCALES(("orange dragon scales"), 0, FREE_ACTION,  500, 9, 2, CLR_ORANGE),
DRGN_SCALES(("black dragon scales"),  0, DISINT_RES, 700, 9, 2, CLR_BLACK),
DRGN_SCALES(("blue dragon scales"),   0, SHOCK_RES,  500, 9, 2, CLR_BLUE),
DRGN_SCALES(("green dragon scales"),  0, POISON_RES, 500, 9, 2, CLR_GREEN),
DRGN_SCALES(("yellow dragon scales"), 0, ACID_RES,   500, 9, 2, CLR_YELLOW),
#undef DRGN_SCALES

SUIT(("plate mail"), /*Needs encyc entry*/
	1, 0,   MZ_HUGE,  ARMSZ_HEAVY, 0,  44,  5, 225,  600,  4, 6, 3, IRON, HI_METAL),
SUIT(("high-elven plate", "runed plate mail"), /*Needs encyc entry*/
	0, 0,   MZ_HUGE, ARMSZ_MEDIUM, 0,   0,  5, 110, 1200,  3, 7, 3, MITHRIL, HI_MITHRIL),
SUIT(("droven plate mail", "crested black plate", "crested plate mail"), /*Needs encyc entry*/
	0, 0,   MZ_HUGE, ARMSZ_MEDIUM, 0,   0,  5,  85, 2000,  3, 7, 3, SHADOWSTEEL, CLR_BLACK),
SUIT(("elven toga"), /*Needs encyc entry*//*Needs tile*/
	1, 0,  MZ_LARGE,  ARMSZ_LIGHT, 0,	0,  5,   5,  100, 10, 1, 2, CLOTH, CLR_GREEN),
SUIT(("noble's dress", "armored dress"), /*Needs encyc entry*/
	0, 0,  MZ_LARGE, ARMSZ_MEDIUM, 0,   0,  5,  40, 2000,  6, 4, 3, SHADOWSTEEL, CLR_BLACK),
SUIT(("consort's suit", "loud foppish suit", "clothes"), /*Needs encyc entry*//*Needs tile*/
	0, 0, MZ_MEDIUM,  ARMSZ_HEAVY, 0,   0,  5,  10, 1000, 10, 1, 1, CLOTH, CLR_BRIGHT_MAGENTA),
SUIT(("gentleman's suit", "expensive clothes"), /*Needs encyc entry*/
	0, 0, MZ_MEDIUM, ARMSZ_MEDIUM, 0,   0,  5,  10, 1000, 10, 1, 2, CLOTH, CLR_BLACK),
SUIT(("gentlewoman's dress", "expensive dress"), /*Needs encyc entry*/
	0, 0,  MZ_LARGE,  ARMSZ_HEAVY, 0,   0,  6, 100, 1000, 10, 1, 3, BONE, CLR_RED), /*Specifically, whale bone*/
SUIT(("crystal plate mail"), /*Needs encyc entry*/
	1, 0,   MZ_HUGE,  ARMSZ_HEAVY, 0,  10,  5, 170, 2000,  4, 6, 0, GLASS, HI_GLASS), /*Best armor, AC wise*/
#ifdef TOURIST
SUIT(("archaic plate mail"), /*Needs encyc entry*/
	1, 0,   MZ_HUGE,  ARMSZ_HEAVY, 0,  20,  5, 200,  400,  5, 5, 3, COPPER, HI_COPPER),
#else
SUIT(("archaic plate mail"),
	1, 0,   MZ_HUGE,  ARMSZ_HEAVY, 0,  35,  5, 200,  400,  5, 5, 3, COPPER, HI_COPPER),
#endif
SUIT(("harmonium plate", "red-lacquered bladed armor", "bladed armor"),
	0, 0,   MZ_HUGE,  ARMSZ_HEAVY, 0,   0,  5, 225,    1,  4, 6, 3, METAL, CLR_RED),
SUIT(("harmonium scale mail", "red-lacquered spiked scale mail", "spiked scale mail"),
	0, 0,  MZ_LARGE, ARMSZ_MEDIUM, 0,   0,  5, 125,    1,  8, 3, 1, METAL, CLR_RED),
SUIT(("plasteel armor", "hard white armor", "armor"), /*Needs encyc entry*//*Needs tile*/
	0, 0,   MZ_HUGE, ARMSZ_MEDIUM, 0,   0,  5, 100,  500,  7, 3, 3, PLASTIC, CLR_WHITE),
// ARMOR(("force armor", "gemstone-adorned clothing"),	/* out of date structure -- rewrite if adding */
	// 0, 0, 1, 0,	 0, 5,  50, 1000, 9, 3, ARM_SUIT, GEMSTONE, CLR_BRIGHT_GREEN),
SUIT(("splint mail"),
	1, 0,   MZ_HUGE,  ARMSZ_HEAVY, 0,  62,  5, 200,   80,  6, 5, 2, IRON, HI_METAL),
SUIT(("barnacle armor", "giant shell armor"),
	0, 1,   MZ_HUGE,  ARMSZ_HEAVY, 0,   0, 10, 150, 1000,  8, 4, 1, SHELL_MAT, CLR_GRAY),
SUIT(("banded mail"),
	1, 0,   MZ_HUGE, ARMSZ_MEDIUM, 0,  72,  5, 175,   90,  7, 3, 1, IRON, HI_METAL),
SUIT(("dwarvish mithril-coat"),
	1, 0,  MZ_LARGE,  ARMSZ_LIGHT, 0,  10,  1,  40,  240,  7, 3, 3, MITHRIL, HI_MITHRIL),
SUIT(("elven mithril-coat"),
	1, 0,  MZ_LARGE,  ARMSZ_LIGHT, 0,  15,  1,  20,  240,  7, 2, 3, MITHRIL, HI_MITHRIL),
SUIT(("chain mail"),
	1, 0,  MZ_LARGE,  ARMSZ_HEAVY, 0,  72,  5, 150,   75,  7, 4, 1, IRON, HI_METAL),
SUIT(("droven chain mail", "crested black mail", "crested mail"), /*Needs encyc entry*/
	0, 0,  MZ_LARGE, ARMSZ_MEDIUM, 0,   0,  5,  50, 1000,  6, 5, 2, SHADOWSTEEL, CLR_BLACK),
SUIT(("orcish chain mail", "crude chain mail"),
	0, 0,  MZ_LARGE,  ARMSZ_HEAVY, 0,  20,  5, 150,   75,  7, 3, 1, IRON, CLR_BLACK),
SUIT(("scale mail"),
	1, 0,  MZ_LARGE, ARMSZ_MEDIUM, 0,  72,  5, 125,   45,  8, 3, 0, IRON, HI_METAL),
SUIT(("studded leather armor"),
	1, 0,  MZ_LARGE,  ARMSZ_LIGHT, 0,  72,  3,  50,   15,  9, 2, 1, LEATHER, HI_LEATHER),
SUIT(("ring mail"),
	1, 0,  MZ_LARGE, ARMSZ_MEDIUM, 0,  72,  5, 125,  100,  9, 3, 1, IRON, HI_METAL),
SUIT(("orcish ring mail", "crude ring mail"),
	0, 0,  MZ_LARGE, ARMSZ_MEDIUM, 0,  20,  5, 125,   80,  9, 2, 1, IRON, CLR_BLACK),
SUIT(("leather armor"),
	1, 0,  MZ_LARGE,  ARMSZ_LIGHT, 0,  82,  3,  40,    5, 10, 2, 0, LEATHER, HI_LEATHER),
//ARMOR(names,
   //kn,mgc,blk,power,prob,delay,wt,cost,ac,dr,can,sub,metal,c)
SUIT(("living armor", "giant sea anemone"),
	0, 1,  MZ_LARGE, ARMSZ_MEDIUM, 0,   0,  6,  80,  500, 10, 2, 0, FLESH, CLR_ORANGE),
SUIT(("jacket"),
	1, 0, MZ_MEDIUM,  ARMSZ_LIGHT, 0,  12,  0,	20,   10, 10, 1, 0, LEATHER, HI_LEATHER, O_MATSPEC(IDED|UNIDED)),
SUIT(("straitjacket", "long-sleeved jacket"), /*Needs encyc entry*//*Needs tile*/
	0, 0, MZ_MEDIUM,  ARMSZ_HEAVY, 0,   0,  0,  15,   10, 10, 1, 2, CLOTH, CLR_WHITE, O_DRSLOT(UPPER_TORSO_DR|ARM_DR)),
SUIT(("healer uniform","clean clothes"), /*Needs encyc entry*//*Needs tile*/
	0, 0, MZ_MEDIUM, ARMSZ_LIGHT,SICK_RES,0,0, 30,   10, 10, 1, 2, CLOTH, CLR_WHITE),
SUIT(("jumpsuit", "silvery clothes", "clothes"),/*Needs encyc entry*//*Needs tile*/
	0, 0,  MZ_LARGE, ARMSZ_LIGHT, REFLECTING,0,5,5,1000, 10, 1, 3, PLASTIC, HI_SILVER, O_DRSLOT(ALL_DR)),
#ifdef TOURIST
/* shirts */
/*ARMOR(("Hawaiian shorts", "flowery shorts and lei"),
	1, 0, 0, 0,	 0, 0,	 5,   3, 10, 0, ARM_SUIT, CLOTH, CLR_MAGENTA),
*/
SUIT(("Hawaiian shorts", "flowery short pants", "short pants"), /*Needs encyc entry*//*Needs tile*/
	0, 0, MZ_SMALL,  ARMSZ_LIGHT, 0,   0,  0,  15,   10, 10, 0, 0, CLOTH, CLR_ORANGE, O_DRSLOT(LOWER_TORSO_DR)),
SHIRT(("Hawaiian shirt", "flowery shirt", "shirt"),
	0, 0, MZ_MEDIUM, 0,	 10, 0,	 5,   3, 10, 0, 0, CLOTH, CLR_MAGENTA),
SHIRT(("T-shirt"), /*Needs encyc entry*/
	1, 0, MZ_MEDIUM, 0,	 5, 0,	 5,   2, 10, 0, 0, CLOTH, CLR_WHITE),
SHIRT(("ichcahuipilli", "thick undershirt"), /*Needs encyc entry*/
	1, 0, MZ_MEDIUM, 0,	 0, 3,	10,   2, 10, 0, 0, CLOTH, CLR_WHITE),
SUIT(("waistcloth"), /*Needs encyc entry*//*Needs tile*/
	0, 0, MZ_SMALL,  ARMSZ_LIGHT, 0,   0,  0,  15,   10, 10, 0, 0, CLOTH, CLR_WHITE, O_DRSLOT(LOWER_TORSO_DR)),
# ifdef CONVICT
SHIRT(("striped shirt", (char *)0, "shirt"), /*Needs encyc entry*/
	1, 0, MZ_MEDIUM, 0,	 0, 0,	 5,   2, 10, 0, 0, CLOTH, CLR_GRAY),
# endif /* CONVICT */
SHIRT(("plain dress", "dress"), /*Needs encyc entry*/
	1, 0, MZ_MEDIUM, 0,	 0, 5,   5, 500, 10, 0, 2, CLOTH, CLR_BLACK, O_DRSLOT(TORSO_DR)),
#endif
/*Ruffled shirts are little different from other shirts*/
SHIRT(("ruffled shirt"), /*Needs encyc entry*/
	1, 0, MZ_MEDIUM, 0,	 0, 0,	 5,   2, 10, 0, 0, CLOTH, CLR_WHITE),
/* victorian underwear, on the other hand, inflicts a penalty to AC but grants MC 3 */
/* needs special case to be 'bulky' */
SHIRT(("victorian underwear", "frilly dress"),
	0, 0, MZ_MEDIUM, 0,	 0, 5,   5,  10, 10, 2, 3, CLOTH, CLR_WHITE,  O_DRSLOT(TORSO_DR)),
SHIRT(("bodyglove", "tight clothes"), /*Needs encyc entry*//*Needs tile*/
	0, 0, MZ_HUGE, SICK_RES,0,5, 5,1000, 10, 0, 3, PLASTIC, CLR_BLACK, O_DRSLOT(ALL_DR)),
/* cloaks */
/*  'cope' is not a spelling mistake... leave it be */
CLOAK(("mummy wrapping"),
		1, 0,	0,	    0, 0,  3,  2, 10, 0, 1, CLOTH, CLR_GRAY, O_DRSLOT(ALL_DR)),
CLOAK(("prayer-warded wrapping"),
		1, 0,	0,	    0, 0,  3, 100, 10, 0, 1, CLOTH, CLR_GRAY, O_DRSLOT(ALL_DR)),
CLOAK(("elven cloak", "faded pall"),
		0, 1,	STEALTH,    7, 0, 10, 60,  9, 0, 3, CLOTH, CLR_BLACK),
CLOAK(("droven cloak", "cobwebbed cloak"), /*Needs encyc entry*/
		0, 1,	0,      1, 0, 10, 60,  10, 0, 3, CLOTH, CLR_GRAY),
CLOAK(("orcish cloak", "coarse mantelet"),
		0, 0,	0,	    8, 0, 10, 40, 10, 0, 2, CLOTH, CLR_BLACK),
CLOAK(("dwarvish cloak", "hooded cloak"),
		0, 0,	0,	    8, 0, 10, 50,10, 1, 2, CLOTH, CLR_BLUE, O_DRSLOT(HEAD_DR|CLOAK_DR)),
CLOAK(("oilskin cloak", "slippery cloak"),
		0, 0,	WATERPROOF,	    8, 0, 10, 50,  9, 0, 3, CLOTH, HI_CLOTH),
CLOAK(("robe"),
		1, 1,	0,	    3, 0, 15, 50, 10, 2, 3, CLOTH, CLR_RED),
CLOAK(("white faceless robe", (char *)0, "faceless robe"),
		1, 1,	0,	    0, 2, 20, 50, 10, 1, 3, CLOTH, CLR_WHITE, O_DRSLOT(HEAD_DR|CLOAK_DR)),
CLOAK(("black faceless robe", (char *)0, "faceless robe"),
		1, 1,	COLD_RES,	    0, 2, 20, 50, 10, 2, 3, CLOTH, CLR_BLACK, O_DRSLOT(HEAD_DR|CLOAK_DR)),
CLOAK(("smoky violet faceless robe", (char *)0, "faceless robe"),
		1, 1,	COLD_RES,	    0, 2, 20,500, 10, 3, 3, CLOTH, CLR_MAGENTA, O_DRSLOT(HEAD_DR|CLOAK_DR)),
CLOAK(("alchemy smock", "apron"),
		0, 1,	POISON_RES, 9, 0, 10, 50, 10, 1, 3, CLOTH, CLR_WHITE),
CLOAK(("Leo Nemaeus hide", "lion skin"),
		0, 1,	HALF_PHDAM,	    0, 10, 60, 1200, 10, 5, 0, DRAGON_HIDE, HI_GOLD),
CLOAK(("cloak"),
		1, 0,			 0,	    8, 0, 15, 40, 10, 2, 1, LEATHER, CLR_BROWN, O_MATSPEC(IDED|UNIDED)),
CLOAK(("studded leather cloak"),
		1, 0,			 0,	    0, 0, 45, 80,  9, 2, 3, LEATHER, HI_COPPER),
/* With shuffled appearances... */
CLOAK(("cloak of protection", "tattered cape"),
		0, 1,	PROTECTION, 9, 0, 10, 50,  9, 1, 3, CLOTH, HI_CLOTH),
CLOAK(("cloak of invisibility", "opera cloak"),
		0, 1,	INVIS,	   10, 0, 10, 60,  9, 0, 2, CLOTH, CLR_BLACK),
CLOAK(("cloak of magic resistance", "ornamental cope"),
		0, 1,	ANTIMAGIC,  2, 0, 10, 60, 10, 0, 3, CLOTH, CLR_WHITE),
CLOAK(("cloak of displacement", "piece of cloth"),
		0, 1,	DISPLACED, 10, 0, 10, 50,  9, 0, 2, CLOTH, CLR_BRIGHT_MAGENTA),

/* shields */
SHIELD(("buckler"),
		1, 0,  MZ_SMALL, 0,	     6, 0, 30,	3,  9, 0, 0, WOOD, HI_WOOD),
SHIELD(("elven shield", "blue and green shield", "shield"),
		0, 0,  MZ_MEDIUM, 0,	     2, 0, 30,	7,  8, 0, 2, WOOD, CLR_GREEN),
SHIELD(("Uruk-hai shield", "white-handed shield", "shield"),
		0, 0, MZ_MEDIUM, 0,	     2, 0, 50,	7,  8, 0, 1, IRON, HI_METAL),
SHIELD(("orcish shield", "red-eyed shield", "shield"),
		0, 0, MZ_MEDIUM, 0,	     2, 0, 50,	7,  9, 0, 0, IRON, CLR_RED),
SHIELD(("kite shield"),
		1, 0,  MZ_LARGE, 0,	     5, 0,100, 10,  8, 0, 1, IRON, HI_METAL),
SHIELD(("tower shield"),
		1, 0,  MZ_HUGE, 0,	     1, 0,200, 20,  6, 0, 1, IRON, HI_METAL),
SHIELD(("roundshield", "round shield"),
		0, 0,  MZ_LARGE, 0,	     1, 0,120,  7,  8, 0, 1, COPPER, HI_COPPER, O_MATSPEC(IDED)),
SHIELD(("dwarvish roundshield", "round shield"),
		0, 0,  MZ_LARGE, 0,	     4, 0, 80, 10,  7, 0, 1, IRON, HI_METAL),
SHIELD(("crystal shield", "shield"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  MZ_LARGE, 0,	     0, 0, 80,150,  9, 0, 0, GLASS, HI_GLASS, O_MATSPEC(UNIDED)),
SHIELD(("shield of reflection", "polished shield", "smooth shield"),
		0, 1,  MZ_LARGE, REFLECTING, 3, 0, 60, 50,  8, 0, 0, SILVER, HI_SILVER),
/*#define SHIELD(names,kn,mgc,blk,power,prob,delay,wt,cost,ac,can,metal,c) \
     ARMOR(names,kn,mgc,blk,power,prob,delay,wt,cost,ac,can,ARM_SHIELD,metal,c) */
#define DRGN_SHIELD(names,mgc,power,cost,ac,dr,color,...)						\
	SHIELD(DEF_BLINDNAME(names, "dragon scale shield"),1,mgc,MZ_LARGE,power,0,0,75,cost,ac,dr,0,DRAGON_HIDE,color,__VA_ARGS__)
/* 3.4.1: dragon scale mail reclassified as "magic" since magic is
   needed to create them */
DRGN_SHIELD(("gray dragon scale shield"),       1, ANTIMAGIC,  1200, 7, 0, CLR_GRAY),
DRGN_SHIELD(("silver dragon scale shield"),     1, REFLECTING, 1200, 7, 0, DRAGON_SILVER),
DRGN_SHIELD(("shimmering dragon scale shield"), 1, DISPLACED,  1200, 7, 0, CLR_CYAN),
DRGN_SHIELD(("red dragon scale shield"),        1, FIRE_RES,    900, 7, 0, CLR_RED),
DRGN_SHIELD(("white dragon scale shield"),      1, COLD_RES,    900, 7, 0, CLR_WHITE),
DRGN_SHIELD(("orange dragon scale shield"),     1, FREE_ACTION, 900, 7, 0, CLR_ORANGE),
DRGN_SHIELD(("black dragon scale shield"),      1, DISINT_RES, 1200, 7, 0, CLR_BLACK),
DRGN_SHIELD(("blue dragon scale shield"),       1, SHOCK_RES,   900, 7, 0, CLR_BLUE),
DRGN_SHIELD(("green dragon scale shield"),      1, POISON_RES,  900, 7, 0, CLR_GREEN),
DRGN_SHIELD(("yellow dragon scale shield"),     1, ACID_RES,    900, 7, 0, CLR_YELLOW),
#undef DRGN_SHIELD
SHIELD(("stone dragon shield"),
		1, 1,  MZ_LARGE, 0, 0, 0, 75, 1200,  7, 0, 0, MINERAL, CLR_GRAY),

/* gloves */
/* these have their color but not material shuffled, so the IRON must stay
 * CLR_BROWN (== HI_LEATHER)
 */
//define GLOVES(names,kn,mgc,power,prob,delay,wt,cost,ac,dr,can,metal,c) \
	ARMOR(names,kn,mgc,0,power,prob,delay,wt,cost,ac,dr,can,ARM_GLOVES,metal,c)
GLOVES(("crystal gauntlets", "gauntlets"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	   0, 2, 20, 400, 8, 4, 0, GLASS, HI_GLASS, O_MATSPEC(UNIDED)),
GLOVES(("gauntlets"), /*Needs encyc entry*//*Needs tile*/
		1, 0,  0,	   4, 2, 25, 10, 8, 4, 0, IRON, HI_METAL, O_MATSPEC(IDED|UNIDED)),
GLOVES(("archaic gauntlets"), /*Needs encyc entry*//*Needs tile*/
		1, 0,  0,	   0, 2, 25, 10, 8, 3, 0, COPPER, HI_COPPER),
GLOVES(("long gloves"),
		1, 0,  0,	   0, 1,  5,  8, 10, 2, 1, CLOTH, CLR_WHITE),
GLOVES(("harmonium gauntlets", "red-lacquered hooked gauntlets", "hooked gauntlets"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	   0, 2, 40,  1, 9, 4, 0, METAL, CLR_RED),
GLOVES(("high-elven gauntlets", "runed gauntlets"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	   0, 2, 15, 50, 8, 4, 0, MITHRIL, HI_MITHRIL),
GLOVES(("plasteel gauntlets", "hard white gauntlets", "gauntlets"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	   0, 2, 15, 50,  8, 3, 0, PLASTIC, CLR_WHITE),
GLOVES(("gloves", "old gloves"),
		0, 0,  0,	   8, 1, 10,  8, 10, 1, 0, LEATHER, HI_LEATHER, O_MATSPEC(IDED)),
GLOVES(("gauntlets of fumbling", "padded gloves"), /*"padded" should give +1 DR*/
		0, 1,  FUMBLING,   7, 1, 10, 50, 10, 1, 0, LEATHER, HI_LEATHER),
GLOVES(("gauntlets of power", "riding gloves"),
		0, 1,  0,	   7, 1, 30, 50,  9, 1, 0, IRON, CLR_BROWN),
GLOVES(("orihalcyon gauntlets", "fighting gloves"),/*Needs tile*/
		0, 1,  ANTIMAGIC,  7, 1, 30, 50,  8, 2, 0, METAL, CLR_BROWN),
GLOVES(("gauntlets of dexterity", "fencing gloves"),
		0, 1,  0,	   7, 1, 10, 50,  8, 0, 0, LEATHER, HI_LEATHER),

/* boots */
//define BOOTS(names,kn,mgc,power,prob,delay,wt,cost,ac,dr,can,metal,c) \
	ARMOR(names,kn,mgc,0,power,prob,delay,wt,cost,ac,dr,can,ARM_BOOTS,metal,c)
BOOTS(("low boots", "walking shoes"),
		0, 0,  0,	  25, 2, 10,  8, 10, 1, 0, LEATHER, HI_LEATHER),
BOOTS(("shoes", "hard shoes"),
		0, 0,  0,	   7, 2, 50, 16,  9, 1, 0, IRON, HI_METAL, O_MATSPEC(IDED)),
BOOTS(("armored boots", "boots"),
		0, 0,  0,	   0, 1, 75, 16,  8, 4, 1, IRON, HI_METAL, O_MATSPEC(IDED|UNIDED)),
BOOTS(("archaic boots", "boots"),
		0, 0,  0,	   0, 1, 75, 16,  8, 3, 1, COPPER, HI_COPPER,O_MATSPEC(UNIDED)),
BOOTS(("harmonium boots", "red-lacquered boots", "boots"),
		0, 0,  0,	   0, 1, 95,  1,  8, 4, 1, METAL, CLR_RED),
BOOTS(("plasteel boots", "hard white boots", "boots"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	   0, 2, 25, 32,  8, 3, 1, PLASTIC, CLR_WHITE),
BOOTS(("stilettos", "high-heeled shoes"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	   0, 1, 10, 60, 10, 0, 0, METAL, HI_METAL),
BOOTS(("high boots", "jackboots"),
		0, 0,  0,	  15, 2, 20, 12, 10, 2, 0, LEATHER, HI_LEATHER),
BOOTS(("heeled boots", "tall boots"),
		1, 0,  0,	   0, 2, 20, 12, 10, 2, 0, LEATHER, CLR_BLACK),
BOOTS(("crystal boots", "boots"), /*Needs encyc entry*//*Needs tile*/
		0, 0,  0,	   0, 2, 60,300,  8, 4, 0, GLASS, HI_GLASS, O_MATSPEC(UNIDED)),
/* With shuffled appearances... */
BOOTS(("speed boots", "combat boots"),
		0, 1,  FAST,	  12, 2, 20, 50,  9, 0, 0, LEATHER, HI_LEATHER),
BOOTS(("water walking boots", "jungle boots"),
		0, 1,  WWALKING,  12, 2, 20, 50, 10, 1, 0, LEATHER, HI_LEATHER),
BOOTS(("jumping boots", "hiking boots"),
		0, 1,  JUMPING,   12, 2, 20, 50,  9, 1, 0, LEATHER, HI_LEATHER),
BOOTS(("elven boots", "mud boots"),
		0, 1,  STEALTH,   12, 2, 10,  8,  9, 0, 0, WOOD, CLR_BROWN),
BOOTS(("kicking boots", "buckled boots"),
		0, 1,  0,         12, 2, 15,  8,  9, 2, 0, IRON, CLR_BROWN),
BOOTS(("fumble boots", "riding boots"),
		0, 1,  FUMBLING,  12, 2, 20, 30, 10, 1, 0, LEATHER, HI_LEATHER),
BOOTS(("flying boots", "snow boots"),
		0, 1,  FLYING,12, 2, 15, 30,  9, 1, 0, LEATHER, HI_LEATHER),
#undef SUIT
#undef SHIRT
#undef HELM
#undef CLOAK
#undef SHIELD
#undef GLOVES
#undef BOOTS
#undef ARMOR

/* rings ... */
#define RING(names,power,cost,mgc,spec,mohs,metal,color,...) OBJECT( \
		DEF_BLINDNAME(names, "ring"), \
		BITS(0,0,spec,0,mgc,spec,0,0,MZ_TINY,0,HARDGEM(mohs),0,0,P_NONE,metal,0), \
		power, RING_CLASS, 0, 0, 3, cost, {0}, {0}, 0, 0, 0, 15, color,__VA_ARGS__ )
RING(("wishes", "black"), 0,                              500, 1, 1, 4, PLATINUM, CLR_BLACK, O_NOWISH(1)),
RING(("adornment", "wooden"), ADORNED,                    100, 1, 1, 2, WOOD, HI_WOOD),
RING(("gain strength", "granite"), 0,                     150, 1, 1, 7, MINERAL, HI_MINERAL),
RING(("gain constitution", "opal"), 0,                    150, 1, 1, 7, GEMSTONE,  CLR_WHITE),
RING(("increase accuracy", "clay"), 0,                    150, 1, 1, 4, MINERAL, CLR_RED),
RING(("increase damage", "coral"), 0,                     150, 1, 1, 4, MINERAL, CLR_ORANGE),
RING(("protection", "black onyx"), PROTECTION,            100, 1, 1, 7, GEMSTONE, CLR_BLACK),
RING(("regeneration", "moonstone"), REGENERATION,         200, 1, 0, 6, MINERAL, HI_MINERAL),
RING(("searching", "tiger eye"), SEARCHING,               200, 1, 0, 6, GEMSTONE, CLR_BROWN),
RING(("stealth", "jade"), STEALTH,                        100, 1, 0, 6, GEMSTONE, CLR_GREEN),
RING(("sustain ability", "bronze"), FIXED_ABIL,           100, 1, 0, 4, COPPER, HI_COPPER),
RING(("levitation", "agate"), LEVITATION,                 200, 1, 0, 7, GEMSTONE, CLR_RED),
RING(("hunger", "topaz"), HUNGER,                         100, 1, 0, 8, GEMSTONE, CLR_CYAN),
RING(("aggravate monster", "sapphire"), AGGRAVATE_MONSTER,150, 1, 0, 9, GEMSTONE, CLR_BLUE),
RING(("conflict", "ruby"), CONFLICT,                      300, 1, 0, 9, GEMSTONE, CLR_RED),
RING(("warning", "diamond"), WARNING,                     100, 1, 0,10, GEMSTONE, CLR_WHITE),
RING(("poison resistance", "pearl"), POISON_RES,          150, 1, 0, 4, MINERAL, CLR_WHITE),
RING(("fire resistance", "iron"), FIRE_RES,               200, 1, 0, 5, IRON, HI_METAL),
RING(("cold resistance", "brass"), COLD_RES,              150, 1, 0, 4, COPPER, HI_COPPER),
RING(("shock resistance", "copper"), SHOCK_RES,           150, 1, 0, 3, COPPER, HI_COPPER),
RING(("free action", "twisted"), FREE_ACTION,             200, 1, 0, 6, IRON, HI_METAL),
RING(("slow digestion", "steel"), SLOW_DIGESTION,         200, 1, 0, 8, IRON, HI_METAL),
RING(("teleportation", "silver"), TELEPORT,               200, 1, 0, 3, SILVER, HI_SILVER),
RING(("teleport control", "gold"), TELEPORT_CONTROL,      300, 1, 0, 3, GOLD, HI_GOLD),
RING(("polymorph", "ivory"), POLYMORPH,                   300, 1, 0, 4, BONE, CLR_WHITE),
RING(("polymorph control", "emerald"), POLYMORPH_CONTROL, 300, 1, 0, 8, GEMSTONE, CLR_BRIGHT_GREEN),
RING(("invisibility", "wire"), INVIS,  		            150, 1, 0, 5, IRON, HI_METAL),
RING(("see invisible", "engagement"), SEE_INVIS,          150, 1, 0, 5, GOLD, HI_METAL),
RING(("alacrity", "shiny"), FAST,				            100, 1, 0, 5, METAL, CLR_BRIGHT_CYAN),/*Needs tile*/
RING(("protection from shape changers", "black signet"), PROT_FROM_SHAPE_CHANGERS,
											            100, 1, 0, 5, MITHRIL, CLR_BLACK),
#undef RING

/* amulets ... - THE Amulet comes last because it is special */
#define AMULET(names,power,prob,...) OBJECT( \
		DEF_BLINDNAME(names, "amulet"), BITS(0,0,0,0,1,0,0,0,0,MZ_TINY,0,0,0,P_NONE,IRON,0), power, \
		AMULET_CLASS, prob, 0, 20, 150, {0}, {0}, 0, 0, 0, 20, HI_METAL, __VA_ARGS__ )

AMULET(("amulet of drain resistance","warped"),   DRAIN_RES,   60),
AMULET(("amulet of ESP",           "circular"),   TELEPAT,    130),
AMULET(("amulet of life saving",   "spherical"),  LIFESAVED,   70),
AMULET(("amulet of strangulation", "oval"),       STRANGLED,  100),
AMULET(("amulet of restful sleep", "triangular"), SLEEPING,   100),
AMULET(("amulet versus poison",    "pyramidal"),  POISON_RES, 130),
AMULET(("amulet versus sickness",    "teardrop"),  SICK_RES, 	 25),
AMULET(("amulet of change",        "square"),     0,          110),
AMULET(("amulet versus curses",    "convex"),     0,           55),/*Needs tile*/
						/* POLYMORPH */
AMULET(("amulet of unchanging",    "concave"),    UNCHANGING,	 45),
AMULET(("amulet of nullify magic", "pentagonal"),  NULLMAGIC,  45),/*Needs tile*/
AMULET(("amulet of reflection",    "hexagonal"),  REFLECTING,  70),
AMULET(("amulet of magical breathing", "octagonal"),      MAGICAL_BREATHING, 60),

AMULET(("cheap plastic imitation of the Amulet of Yendor", "Amulet of Yendor"), 0, 0,
	O_USKWN(1), O_MAGIC(0), O_MAT(PLASTIC), O_COST(20), O_NUT(1)),
AMULET(("Amulet of Yendor", "Amulet of Yendor"), 0, 0, /* note: description == name */
	O_USKWN(1), O_MAGIC(0), O_UNIQ(1), O_NOWISH(1), O_MAT(MITHRIL), O_COST(30000)),
#undef AMULET

/* tools ... */
/* tools with weapon characteristics come last */
#define TOOL(names,kn,size,mrg,mgc,chg,prob,wt,cost,mat,color,...) \
	OBJECT( names, \
		BITS(kn,mrg,chg,0,mgc,chg,0,0,size,0,0,0,0,P_NONE,mat,0), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, {0}, {0}, 0, 0, 0, wt, color, __VA_ARGS__)
#define CONTAINER(names,kn,size,mgc,chg,prob,wt,cost,mat,color,...) \
	OBJECT( names, \
		BITS(kn,0,chg,1,mgc,chg,0,0,size,0,0,0,0,P_NONE,mat,0), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, {0}, {0}, 0, 0, 0, wt, color, __VA_ARGS__)
#define WEPTOOL(names,sdam,ldam,kn,size,mgc,chg,prob,wt,cost,hitbon,typ,sub,mat,clr,...) \
	OBJECT( names, \
		BITS(kn,0,1,chg,mgc,1,0,0,size,0,0,0,typ,sub,mat,0), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, WP_GENERIC, 0, wt, clr, __VA_ARGS__)
/* containers */
CONTAINER(("box"),             1,   MZ_HUGE, 0, 0,  30, 350,   8, WOOD, HI_WOOD),
CONTAINER(("massive stone crate"),
										1,MZ_GIGANTIC,0, 0,   0,6000,  80, MINERAL,HI_MINERAL),/*Needs tile*/
CONTAINER(("chest"),           1,   MZ_HUGE, 0, 0,  30, 600,  16, WOOD, HI_WOOD),
CONTAINER(("magic chest", "big chest with 10 keyholes"),
										0,MZ_GIGANTIC,1, 0,  15,1001,7000, METAL, CLR_BRIGHT_MAGENTA),/*Needs tile*/
CONTAINER(("writing desk"),    1,   MZ_HUGE, 0, 0,   0, 600,  16, WOOD, HI_WOOD),
CONTAINER(("ice box"),         1,   MZ_HUGE, 0, 0,   5, 250,  42, PLASTIC, CLR_WHITE),
CONTAINER(("sack", "bag"),                0, MZ_MEDIUM, 0, 0,  20,  15,   2, CLOTH, HI_CLOTH),
CONTAINER(("oilskin sack", "bag"),        0, MZ_MEDIUM, 0, 0,  20,  15, 100, CLOTH, HI_CLOTH),
CONTAINER(("bag of holding", "bag"),      0, MZ_MEDIUM, 1, 0,  20,  15, 100, CLOTH, HI_CLOTH),
CONTAINER(("bag of tricks", "bag"),       0, MZ_MEDIUM, 1, 1,  20,  15, 100, CLOTH, HI_CLOTH),
#define HOSTAGE(names,kn,mgc,chg,prob,ntrtn,wt,cost,mat,color,...) \
	OBJECT( names, \
		BITS(kn,0,chg,1,mgc,chg,0,0,MZ_MEDIUM,0,0,0,0,P_NONE,mat), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, {0}, {0}, 0, ntrtn, 0, wt, color, __VA_ARGS__)

//HOSTAGE(("distressed princess"),           1, 0, 0,  0, 350,.9*1450,  1600, CLOTH, CLR_WHITE),
#undef HOSTAGE
#undef CONTAINER

/* lock opening tools */
TOOL(("skeleton key", "key"),              0,   MZ_TINY, 0, 0, 0,  80,  3,  10, IRON,    HI_METAL),
TOOL(("universal key", "key"),	         0,   MZ_TINY, 0, 0, 0,   0,  3,  10, SILVER,  HI_SILVER, O_MATSPEC(UNIDED)),
#ifdef TOURIST
TOOL(("lock pick"),             1,   MZ_TINY, 0, 0, 0,  60,  4,  20, IRON,    HI_METAL),
TOOL(("credit card"),           1,   MZ_TINY, 0, 0, 0,   0,  1,  10, PLASTIC, CLR_WHITE),
#else
TOOL(("lock pick"),             1,   MZ_TINY, 0, 0, 0,  60,  4,  20, IRON,    HI_METAL),
#endif
/* light sources */
TOOL(("tallow candle", "candle"),          0,   MZ_TINY, 1, 0, 0,  15,  2,  10, WAX,     CLR_WHITE),
TOOL(("wax candle", "candle"),             0,   MZ_TINY, 1, 0, 0,   5,  2,  20, WAX,     CLR_WHITE),
TOOL(("candle of invocation", "runed candle"), 
                                         0,   MZ_TINY, 0, 1, 0,  15,  2,  50, WAX,     CLR_ORANGE, O_NOWISH(1)),
TOOL(("lantern"),               1,  MZ_SMALL, 0, 0, 0,  20, 30,  12, COPPER,  CLR_YELLOW, O_MATSPEC(IDED|UNIDED)),
TOOL(("oil lamp", "lamp"),                 0,  MZ_SMALL, 0, 0, 0,  30, 20,  10, COPPER,  CLR_YELLOW),
TOOL(("magic lamp", "lamp"),               0,  MZ_SMALL, 0, 1, 0,  15, 20,  50, COPPER,  CLR_YELLOW, O_NOWISH(1)),
// TOOL(("shadowlander's torch", "black torch"),
								//       0,  MZ_SMALL, 0, 1, 0,  10, 20,  50, WOOD, CLR_BLACK),
/* other tools */
#ifdef TOURIST
TOOL(("expensive camera"),
				                1,  MZ_SMALL, 0, 0, 1,  15, 12, 200, PLASTIC, CLR_BLACK),
TOOL(("mirror", "looking glass"), 0,   MZ_TINY, 0, 0, 0,  45, 13,  10, GLASS, HI_SILVER),
#else
TOOL(("mirror", "looking glass"), 0,   MZ_TINY, 0, 0, 0,  60, 13,  10, GLASS, HI_SILVER),
#endif
TOOL(("crystal ball", "glass orb"),
								0,  MZ_SMALL, 0, 1, 1,  15, 50, 200, GLASS, HI_GLASS),
TOOL(("misotheistic pyramid", "black-stone pyramid"),
								0,  MZ_TINY,  1, 1, 0,   0,  1, 200, OBSIDIAN_MT, CLR_BLACK),
TOOL(("misotheistic fragment", "shattered mirrored pyramid"),
								0,  MZ_TINY,  1, 1, 0,   0,  1, 200, GLASS, HI_SILVER),
TOOL(("dimensional lock", "cerulean-willow-engraved disk", "disk"),
								0,  MZ_SMALL,  1, 1, 0,   0, 10,2000, MINERAL, CLR_BRIGHT_BLUE),
TOOL(("preservative engine", "infernal contraption"),
								0,  MZ_SMALL,  0, 1, 1,   0, 50,8000, METAL, CLR_RED),
TOOL(("armor salve", "strange shell"),
								0,  MZ_SMALL,  0, 1, 1,   0, 10, 666, SHELL_MAT, CLR_GRAY),
TOOL(("sensor pack", "rigid box"), /*Needs encyc entry*//*Needs tile*/
								0,  MZ_SMALL, 0, 1, 1,   0, 15,2000, PLASTIC,CLR_WHITE),
TOOL(("hypospray", "hammer-shaped device"), /*Needs encyc entry*//*Needs tile*/
								0,  MZ_SMALL, 0, 1, 0,   0, 15, 500, PLASTIC,CLR_GRAY),
TOOL(("hypospray ampule", "hard gray bottle", "hard bottle"), /*Needs encyc entry*//*Needs tile*/
								0,   MZ_TINY, 0, 1, 0,   0,  1,  50, PLASTIC,CLR_GRAY),
TOOL(("mask"),			1,  MZ_SMALL, 0, 0, 0,  10, 10,  80, LEATHER, CLR_WHITE),
TOOL(("R'lyehian faceplate", "ebon pane"), /*Needs tile*/
								0,  MZ_SMALL, 0, 1, 0,   0, 15, 200, GLASS, CLR_BLACK, O_POWER(POISON_RES)),
TOOL(("living mask", "gilled jellyfish"),  
								0,  MZ_SMALL, 0, 1, 0,   0,  5, 200, FLESH, CLR_BLUE, O_POWER(MAGICAL_BREATHING)),
TOOL(("lenses"),		1,   MZ_TINY, 0, 0, 0,   5,  3,  80, GLASS, HI_GLASS), /*Needs encyc entry*/
TOOL(("sunglasses", "mirrored lenses"),
					   0,   MZ_TINY, 0, 0, 0,   0,  3, 240, GLASS, CLR_BROWN, O_POWER(BLIND_RES)), /*Needs encyc entry*/
TOOL(("blindfold"),    1,   MZ_TINY, 0, 0, 0,  45,  2,  20, CLOTH, CLR_GRAY),
TOOL(("android visor", "black blindfold", "blindfold"),
								0,   MZ_TINY, 0, 0, 0,   0,  2,  40, CLOTH, CLR_BLACK),
TOOL(("towel"),        1,   MZ_TINY, 0, 0, 0,  45,  2,  50, CLOTH, CLR_MAGENTA),
#ifdef STEED
TOOL(("saddle"),       1,  MZ_LARGE, 0, 0, 0,   5,200, 150, LEATHER, HI_LEATHER),
TOOL(("leash"),        1,  MZ_SMALL, 0, 0, 0,  60, 12,  20, LEATHER, HI_LEATHER),
#else
TOOL(("leash"),        1,  MZ_SMALL, 0, 0, 0,  65, 12,  20, LEATHER, HI_LEATHER),
#endif
TOOL(("stethoscope"),  1,  MZ_SMALL, 0, 0, 0,  25,  4,  75, IRON, HI_METAL),
TOOL(("tinning kit"),  1, MZ_MEDIUM, 0, 0, 1,  15,100,  30, IRON, HI_METAL),
TOOL(("bullet fabber", "white box with a yellow fiddly bit", "fiddly box"),/*Needs tile*/
								0, MZ_MEDIUM, 0, 1, 0,   0, 20,  30, PLASTIC, CLR_WHITE),
TOOL(("upgrade kit"),  1, MZ_MEDIUM, 0, 0, 0,  40,100,  30, COPPER, HI_COPPER),/*Needs encyc entry*//*Needs tile*/
TOOL(("power pack", "little white cube", "little cube"), /*Needs encyc entry*//*Needs tile*/
								0,   MZ_TINY, 1, 1, 0,   0,  1,  300, PLASTIC, CLR_WHITE),
TOOL(("trephination kit"),  
								1, MZ_MEDIUM, 0, 0, 1,   5, 10,  30, METAL, HI_METAL),/*Needs encyc entry*//*Needs tile*/
TOOL(("tin opener"),   1,   MZ_TINY, 0, 0, 0,  20,  4,  30, IRON, HI_METAL),
TOOL(("can of grease"),1,  MZ_SMALL, 0, 0, 1,  15, 15,  20, IRON, HI_METAL),
TOOL(("figurine"),     1,  MZ_SMALL, 0, 1, 0,  20, 50,  80, MINERAL, HI_MINERAL),
/*Keep in sync with doll mvar flags*/
TOOL(("effigy",   (char *)0),     1,   MZ_TINY, 1, 1, 0,  20,  5,  80, LEATHER, HI_LEATHER),

#define DOLL(names, color) \
	TOOL(DEF_BLINDNAME(names, "doll"), 0, MZ_TINY, 1, 1, 0, 0, 1, 80, CLOTH, color)

DOLL(("doll of jumping",        "Pole-vaulter doll"),      CLR_BLUE),
DOLL(("doll of friendship",     "Bard doll"),              CLR_BRIGHT_GREEN),
DOLL(("doll of chastity",       "Priest doll"),            CLR_WHITE),
DOLL(("doll of cleaving",       "Berserker doll"),         CLR_RED),
DOLL(("doll of satiation",      "Chef doll"),              CLR_WHITE),
DOLL(("doll of good health",    "Healer doll"),            CLR_WHITE),
DOLL(("doll of full healing",   "Nurse doll"),             CLR_WHITE),
DOLL(("doll of destruction",    "Dancing doll"),           HI_COPPER),
DOLL(("doll of memory",         "Scholar doll"),           CLR_BRIGHT_BLUE),
DOLL(("doll of binding",        "Heretic doll"),           CLR_BROWN),
DOLL(("doll of preservation",   "Umbrella-wielding doll"), CLR_YELLOW),
DOLL(("doll of quick-drawing",  "Archaeologist doll"),     CLR_BROWN),
DOLL(("doll of wand-charging",  "Wandsman doll"),          CLR_BRIGHT_BLUE),
DOLL(("doll of stealing",       "Thief doll"),             CLR_BLACK),
DOLL(("doll of mollification",  "High priest doll"),       HI_GOLD),
DOLL(("doll of clear-thinking", "Madman doll"),            CLR_GRAY),
DOLL(("doll of mind-blasting",  "Squid-parasitized doll"), CLR_MAGENTA),
#undef DOLL
TOOL(("doll's tear",   "milky gemstone", "gem"),
								0,   MZ_TINY, 0, 1, 0,   0,  1,8000, GEMSTONE, CLR_WHITE),
TOOL(("holy symbol of the black mother", "tarnished triple goat-head"), 
								0,   MZ_TINY, 0, 1, 0,   0, 50,8000, SILVER, CLR_BLACK),
TOOL(("magic marker"), 1,   MZ_TINY, 0, 1, 1,  15,  2,  50, PLASTIC, CLR_RED),
/* traps */
TOOL(("land mine",(char *)0),     1,  MZ_LARGE, 0, 0, 0,   0,300, 180, IRON, CLR_RED),
TOOL(("beartrap"),     1,  MZ_LARGE, 0, 0, 0,   0,100,  60, IRON, HI_METAL),
WEPTOOL(("hook"),
	DMG(D(6)), DMG(D(6)),
	1,  MZ_LARGE, 0, 0, 0, 80,  50,  0, P,   P_PICK_AXE, CHITIN, CLR_GREEN, O_MATSPEC(IDED|UNIDED)),
/* instruments */
/* some code in invent.c and obj.h requires wooden flute .. drum of earthquake to be
   consecutive, with the wooden flute first and drum of earthquake last */
/* non-magical instruments always show what they are made of, because that's interesting (ie, a "wooden harp" vs a "harp")
 *   but for magical instruments that information is very secondary to it being magic
 *   and also sounds weird (ie, a "wooden magic harp" vs a "magic harp")
 */
TOOL(("whistle", "whistle"),         0,   MZ_TINY, 0, 0, 0, 60,  3, 10, METAL,   HI_METAL,   O_MATSPEC(IDED )),
TOOL(("magic whistle", "whistle"),   0,   MZ_TINY, 0, 1, 0, 30,  3, 10, METAL,   HI_METAL,   O_MATSPEC(NIDED)),
/* "If tin whistles are made out of tin, what do they make foghorns out of?" */		   
TOOL(("flute", "flute"),             0,  MZ_SMALL, 0, 0, 0,  4,  5, 12, WOOD,    HI_WOOD,    O_MATSPEC(IDED )),
TOOL(("magic flute", "flute"),       0,  MZ_SMALL, 0, 1, 1,  2,  5, 36, WOOD,    HI_WOOD,    O_MATSPEC(NIDED)),
TOOL(("tooled horn", "horn"),        0,  MZ_SMALL, 0, 0, 0,  5, 18, 15, BONE,    CLR_WHITE,  O_MATSPEC(IDED )),
TOOL(("frost horn", "horn"),         0,  MZ_SMALL, 0, 1, 1,  2, 18, 50, BONE,    CLR_WHITE,  O_MATSPEC(NIDED)),
TOOL(("fire horn", "horn"),          0,  MZ_SMALL, 0, 1, 1,  2, 18, 50, BONE,    CLR_WHITE,  O_MATSPEC(NIDED)),
TOOL(("horn of plenty", "horn"),     0,  MZ_SMALL, 0, 1, 1,  2, 18, 50, BONE,    CLR_WHITE,  O_MATSPEC(NIDED)),
TOOL(("harp", "harp"),               0, MZ_MEDIUM, 0, 0, 0,  4, 30, 50, WOOD,    HI_WOOD,    O_MATSPEC(IDED )),
TOOL(("magic harp", "harp"),         0, MZ_MEDIUM, 0, 1, 1,  2, 30, 50, WOOD,    HI_WOOD,    O_MATSPEC(NIDED)),
TOOL(("bell"),            1,   MZ_TINY, 0, 0, 0,  2, 30, 50, COPPER,  HI_COPPER,  O_MATSPEC(IDED )),
TOOL(("bugle"),           1,  MZ_SMALL, 0, 0, 0,  4, 10, 15, COPPER,  HI_COPPER,  O_MATSPEC(IDED )),
TOOL(("drum", "drum"),               0, MZ_MEDIUM, 0, 0, 0,  4, 25, 25, LEATHER, HI_LEATHER, O_MATSPEC(IDED )),
TOOL(("drum of earthquake", "drum"), 0, MZ_MEDIUM, 0, 1, 1,  2, 25, 25, LEATHER, HI_LEATHER, O_MATSPEC(NIDED)),
/* tools useful as weapons */
WEPTOOL(("pick-axe"),
	DMG(D(6)), DMG(D(3)),
	1,  MZ_LARGE, 0, 0, 20, 80,  50,  0, P,   P_PICK_AXE, IRON, HI_METAL),
WEPTOOL(("seismic hammer", "dull metallic hammer"),
	DMG(D(12)), DMG(D(10)),
	0,   MZ_HUGE, 1, 1,  0,150, 250, -5, B,   P_HAMMER,  METAL, HI_METAL),
/*
 * Torches work as clubs
 */
WEPTOOL(("torch"),
	DMG(D(6)), DMG(D(3)),
	1,  MZ_SMALL, 0, 0, 15, 10,   5,  0, B,   P_CLUB, WOOD, HI_WOOD),
WEPTOOL(("shadowlander's torch", "black torch", "torch"),
	DMG(D(6)), DMG(D(3)),
	0,  MZ_SMALL, 0, 0, 10, 10,  50,  0, B,   P_CLUB, WOOD, CLR_BLACK),
WEPTOOL(("sunrod", "rod"),
	DMG(D(6)), DMG(D(3)),
	1,  MZ_SMALL, 0, 0,  5, 20,  50,  0, B,   P_MACE, GOLD, HI_GOLD, O_MATSPEC(UNIDED)),
/* 
 * Lightsabers get 3x dice when lit, and go down to 1d2 damage when unlit
 */
WEPTOOL(("lightsaber", "sword hilt"), /*Needs (better) encyc entry*/
	DMG(D(8)), DMG(D(8)),
	0,  MZ_SMALL, 1, 1,  0, 10, 500, -3, S|E, P_SABER, SILVER, HI_SILVER, O_MATSPEC(IDED|UNIDED)),
WEPTOOL(("beamsword",  "broadsword hilt"), /*Needs encyc entry*/
	DMG(D(10)), DMG(D(10)),
	0,  MZ_SMALL, 1, 1,  0, 20, 500, -3, S|E, P_BROAD_SWORD, GOLD, HI_GOLD, O_MATSPEC(IDED|UNIDED)),
WEPTOOL(("double lightsaber",  "long grip"), /*Needs encyc entry*//*Needs tile*//*needs special case for 2handedness*/
	DMG(D(10)), DMG(D(10)),
	0,  MZ_SMALL, 1, 1,  0, 30,1000, -6, S|E, P_QUARTERSTAFF, PLATINUM, HI_SILVER, O_MATSPEC(IDED|UNIDED)),
WEPTOOL(("rod of force", "rod"), /*Needs encyc entry*/
	DMG(D(8)), DMG(D(12)),
	0,  MZ_SMALL, 1, 1,  0, 10,1000, 1, S|P|E, P_LONG_SWORD, IRON, CLR_BLACK, O_DIR(IMMEDIATE), O_MATSPEC(UNIDED)),
WEPTOOL(("grappling hook", "hook"),
	DMG(D(2)), DMG(D(6)),
	0, MZ_MEDIUM, 0, 0,  4, 30,  50,  0, B,   P_FLAIL, IRON, HI_METAL, O_MATSPEC(UNIDED)),
WEPTOOL(("shepherd's crook", "bent staff"),
	DMG(D(6)), DMG(D(4)),
	0,   MZ_LARGE, 0, 0, 1,  30,   5, 0, B,   P_QUARTERSTAFF, WOOD, HI_WOOD),
/* 3.4.1: unicorn horn left classified as "magic" */
WEPTOOL(("unicorn horn"),
	DMG(D(12)), DMG(D(12)),
	1, MZ_MEDIUM, 1, 0, 0,  20, 100,  0, P,   P_UNICORN_HORN, BONE, CLR_WHITE),
WEPTOOL(("spoon"), /*Needs encyc entry*//*Needs tile*/
	DMG(D(1)), DMG(D(1)),
	1,   MZ_TINY, 0, 0, 0,   1,   1,  0, P,   P_KNIFE, IRON, HI_METAL),

/* two special unique artifact tools */
TOOL(("Candelabrum of Invocation", "candelabrum"), 0, MZ_SMALL, 0, 1, 0, 0, 10, 5000, GOLD, HI_GOLD,
	O_USKWN(1), O_UNIQ(1), O_NOWISH(1), O_NUT(200)),
TOOL(("Bell of Opening",           "bell"),        0,  MZ_TINY, 0, 1, 1, 0, 10, 5000, SILVER, HI_SILVER,
	O_USKWN(1), O_UNIQ(1), O_NOWISH(1), O_MATSPEC(UNIDED), O_NUT(50)),
#undef TOOL
#undef WEPTOOL

/* Comestibles ... */
#define FOOD(names,prob,size,delay,wt,unk,tin,nutrition,color,...) OBJECT( \
		names, BITS(1,1,unk,0,0,0,0,0,size,0,0,0,0,P_NONE,tin,0), 0, \
		FOOD_CLASS, prob, delay, \
		wt, nutrition / 20 + 5, {0}, {0}, 0, 0, 0, nutrition, color, __VA_ARGS__)
/* all types of food (except tins & corpses) must have a delay of at least 1. */
/* delay on corpses is computed and is weight dependant */
/* dog eats foods 0-4 but prefers tripe rations above all others */
/* fortune cookies can be read */
/* carrots improve your vision */
/* +0 tins contain monster meat */
/* +1 tins (of spinach) make you stronger (like Popeye) */
/* food CORPSE is a cadaver of some type */
/* meatballs/sticks/rings are only created from objects via stone to flesh */

/* meat */
FOOD(("tripe ration"),          140,  MZ_SMALL,  2, 10, 0, FLESH, 200, CLR_BROWN),
FOOD(("corpse"),                  0, MZ_MEDIUM,  1,  0, 0, FLESH,   0, CLR_BROWN),
FOOD(("egg"),                    85,   MZ_TINY,  1,  1, 1, FLESH,  80, CLR_WHITE),
FOOD(("meatball"),                0,   MZ_TINY,  1,  1, 0, FLESH,   5, CLR_BROWN),
FOOD(("meat stick"),              0,   MZ_TINY,  1,  1, 0, FLESH,   5, CLR_BROWN),
FOOD(("massive chunk of meat"),   0,MZ_GIGANTIC,20,400, 0, FLESH,2000, CLR_BROWN),
FOOD(("meat ring"),               0,   MZ_TINY,  1,  1, 0, FLESH,   5, CLR_BROWN, O_MERGE(0)),

/* Body parts.... eeeww */
FOOD(("eyeball"),                 0,   MZ_TINY,  1,  0, 0, FLESH,  10, CLR_WHITE),/*Needs tile*/
FOOD(("severed hand"),            0,   MZ_TINY,  1,  0, 0, FLESH,  40, CLR_BROWN),/*Needs tile*/

/* fruits & veggies */
FOOD(("kelp frond"),              0,  MZ_SMALL,  1,  1, 0, VEGGY,  30, CLR_GREEN),
FOOD(("eucalyptus leaf"),         3,   MZ_TINY,  1,  1, 0, VEGGY,  30, CLR_GREEN),
/*Forbidden by Eve starts here:*/
FOOD(("apple"),                  15,   MZ_TINY,  1,  2, 0, VEGGY,  50, CLR_RED),
FOOD(("orange"),                 10,   MZ_TINY,  1,  2, 0, VEGGY,  80, CLR_ORANGE),
FOOD(("pear"),                   10,   MZ_TINY,  1,  2, 0, VEGGY,  50, CLR_BRIGHT_GREEN),
FOOD(("melon"),                  10,  MZ_SMALL,  1,  5, 0, VEGGY, 100, CLR_BRIGHT_GREEN),
FOOD(("banana"),                 10,   MZ_TINY,  1,  2, 0, VEGGY,  80, CLR_YELLOW),
FOOD(("carrot"),                 15,   MZ_TINY,  1,  2, 0, VEGGY,  50, CLR_ORANGE),
/*:and ends here*/
FOOD(("sprig of wolfsbane"),      7,   MZ_TINY,  1,  1, 0, VEGGY,  40, CLR_GREEN),
FOOD(("clove of garlic"),         7,   MZ_TINY,  1,  1, 0, VEGGY,  40, CLR_WHITE),
FOOD(("slime mold"),             75,   MZ_TINY,  1,  5, 0, VEGGY, 250, HI_ORGANIC),

/* people food */
FOOD(("lump of royal jelly"),	    0,   MZ_TINY,  1,  2, 0, VEGGY, 200, CLR_YELLOW),
FOOD(("lump of soldier's jelly"),  
							    0,   MZ_TINY,  1,  2, 0, VEGGY, 200, CLR_YELLOW),
FOOD(("lump of dancer's jelly"),
							    0,   MZ_TINY,  1,  2, 0, VEGGY, 200, CLR_YELLOW),
FOOD(("lump of philosopher's jelly"),
							    0,   MZ_TINY,  1,  2, 0, VEGGY, 200, CLR_YELLOW),
FOOD(("lump of priestess's jelly"),
							    0,   MZ_TINY,  1,  2, 0, VEGGY, 200, CLR_YELLOW),
FOOD(("lump of rhetor's jelly"),
							    0,   MZ_TINY,  1,  2, 0, VEGGY, 200, CLR_YELLOW),
FOOD(("honeycomb"),			    0,   MZ_TINY,  4, 10, 0, VEGGY, 800, CLR_YELLOW),
FOOD(("cream pie"),              25,   MZ_TINY,  1, 10, 0, VEGGY, 100, CLR_WHITE),
FOOD(("candy bar"),              13,   MZ_TINY,  1,  2, 0, VEGGY, 100, CLR_BROWN),
FOOD(("fortune cookie"),         55,   MZ_TINY,  1,  1, 0, VEGGY,  40, CLR_YELLOW),
FOOD(("pancake"),                25,   MZ_TINY,  2,  2, 0, VEGGY, 200, CLR_YELLOW),
FOOD(("lembas wafer"),           20,   MZ_TINY,  2,  5, 0, VEGGY, 800, CLR_WHITE),
FOOD(("cram ration"),            20,  MZ_SMALL,  3, 15, 0, VEGGY, 600, HI_ORGANIC),
FOOD(("food ration"),           380,  MZ_SMALL,  5, 20, 0, VEGGY, 800, HI_ORGANIC),
FOOD(("protein pill"),            0,   MZ_TINY,  1,  1, 0, VEGGY, 800, HI_ORGANIC), /*Needs encyc entry*//*Needs tile*/
FOOD(("K-ration"),                0,  MZ_SMALL,  1, 10, 0, VEGGY, 400, HI_ORGANIC),
FOOD(("C-ration"),                0,  MZ_SMALL,  1, 10, 0, VEGGY, 300, HI_ORGANIC),
FOOD(("tin"),                    75,   MZ_TINY,  0, 10, 1, METAL,   0, HI_METAL),
#undef FOOD

/* potions ... */
#define POTION(names,mgc,power,prob,cost,color,...) OBJECT( \
		DEF_BLINDNAME(names, "potion"), BITS(0,1,0,0,mgc,0,0,0,MZ_SMALL,0,0,0,0,P_NONE,GLASS,0), power, \
		POTION_CLASS, prob, 0, 20, cost, {0}, {0}, 0, 0, 0, 10, color, __VA_ARGS__)
POTION(("gain ability", "ruby"),          1, 0,          40, 300, CLR_RED),
POTION(("restore ability", "pink"),       1, 0,          40, 100, CLR_BRIGHT_MAGENTA),
POTION(("confusion", "orange"),           1, CONFUSION,  33, 100, CLR_ORANGE),
POTION(("blindness", "yellow"),           1, BLINDED,    33, 150, CLR_YELLOW),
POTION(("paralysis", "emerald"),          1, 0,          32, 300, CLR_BRIGHT_GREEN),
POTION(("speed", "dark green"),           1, FAST,       40, 200, CLR_GREEN),
POTION(("levitation", "cyan"),            1, LEVITATION, 40, 200, CLR_CYAN),
POTION(("hallucination", "sky blue"),     1, HALLUC,     40, 100, CLR_CYAN),
POTION(("invisibility", "brilliant blue"),1, INVIS,      40, 150, CLR_BRIGHT_BLUE),
POTION(("see invisible", "magenta"),      1, SEE_INVIS,  40,  50, CLR_MAGENTA),
POTION(("healing", "purple-red"),         1, 0,          57, 100, CLR_MAGENTA),
POTION(("extra healing", "puce"),         1, 0,          47, 100, CLR_RED),
POTION(("gain level", "milky"),           1, 0,          20, 300, CLR_WHITE),
POTION(("enlightenment", "swirly"),       1, 0,          20, 200, CLR_BROWN),
POTION(("monster detection", "bubbly"),   1, 0,          40, 150, CLR_WHITE),
POTION(("object detection", "smoky"),     1, 0,          42, 150, CLR_GRAY),
POTION(("gain energy", "cloudy"),         1, 0,          40, 150, CLR_WHITE),
POTION(("sleeping",  "effervescent"),     1, 0,          40, 100, CLR_GRAY),
POTION(("full healing",  "black"),        1, 0,          10, 200, CLR_BLACK),
POTION(("polymorph", "golden"),           1, 0,          10, 200, CLR_YELLOW),
POTION(("booze", "brown"),                0, 0,          42,  50, CLR_BROWN),
POTION(("sickness", "fizzy"),             0, 0,          40,  50, CLR_CYAN),
POTION(("fruit juice", "dark"),           0, 0,          42,  50, CLR_BLACK),
POTION(("acid", "white"),                 0, 0,          32, 250, CLR_WHITE),
POTION(("oil", "murky"),                  0, 0,          30, 250, CLR_BROWN),
POTION(("amnesia", "sparkling"),          1, 0,          8,  100, CLR_CYAN),
POTION(("goat's milk", "black"),          1, 0,          0,  900, CLR_BLACK),
POTION(("space mead", "golden"),          1, 0,          0,  900, CLR_YELLOW),
POTION(("starlight", "dimly-shining"),    1, 0,          4,  250, CLR_BRIGHT_CYAN),
POTION(("water", "clear"),                0, 0,          80, 100, CLR_CYAN),
POTION(("blood", "blood-red"),            0, 0,          18, 50,  CLR_RED, O_USKWN(1)),	/* each potion of blood must be ID-ed */
#undef POTION

/* scrolls ... */
#define SCROLL(names,mgc,prob,cost,...) OBJECT( \
		DEF_BLINDNAME(names, "scroll"), BITS(0,1,0,0,mgc,0,0,0,MZ_SMALL,0,0,0,0,P_NONE,PAPER,0), 0, \
		SCROLL_CLASS, prob, 0, 5, cost, {0}, {0}, 0, 0, 0, 6, HI_PAPER, __VA_ARGS__)
	SCROLL(("enchant armor",         "ZELGO MER"),            1,  59,  80),
	SCROLL(("destroy armor",         "JUYED AWK YACC"),       1,  35, 100),
	SCROLL(("confuse monster",       "NR 9"),                 1,  41, 100),
	SCROLL(("scare monster",         "XIXAXA XOXAXA XUXAXA"), 1,  32, 100),
	SCROLL(("remove curse",          "PRATYAVAYAH"),          1,  60,  80),
	SCROLL(("enchant weapon",        "DAIYEN FOOELS"),        1,  74,  60),
	SCROLL(("create monster",        "LEP GEX VEN ZEA"),      1,  41, 200),
	SCROLL(("taming",                "PRIRUTSENIE"),          1,  14, 200),
	SCROLL(("genocide",              "ELBIB YLOH"),           1,  13, 300),
	SCROLL(("light",                 "VERR YED HORRE"),       1,  71,  50),
	SCROLL(("teleportation",         "VENZAR BORGAVVE"),      1,  51, 100),
	SCROLL(("gold detection",        "THARR"),                1,  30, 100),
	SCROLL(("food detection",        "YUM YUM"),              1,  23, 100),
	SCROLL(("identify",              "KERNOD WEL"),           1, 169,  20),
	SCROLL(("magic mapping",         "ELAM EBOW"),            1,  41, 100),
	SCROLL(("amnesia",               "DUAM XNAHT"),           1,  27, 200),
	SCROLL(("fire",                  "ANDOVA BEGARIN"),       1,  23, 100),
	SCROLL(("earth",                 "KIRJE"),                1,  16, 200),
	SCROLL(("punishment",            "VE FORBRYDERNE"),       1,  12, 300),
	SCROLL(("charging",              "HACKEM MUCHE"),         1,  14, 300),
	SCROLL(("stinking cloud",        "VELOX NEB"),            1,  13, 300),
	SCROLL(("ward",                  "TLASFO SENIL"),         1,  55, 300),
	SCROLL(("warding",               "RW NW PRT M HRW"),      1,  13, 300),
	SCROLL(("antimagic",             "KARSUS"),               1,  18, 250),
	SCROLL(("resistance",            "DESREVER TSEPMET"),     1,  34, 250),
	SCROLL(("consecration",          "HLY HLS"),              1,   0,3000, O_NOWISH(1)),	/* unwishable/unwritable */
	SCROLL(((char *)0,               "FOOBIE BLETCH"),        1,   0, 100),
	SCROLL(((char *)0,               "TEMOV"),                1,   0, 100),
	SCROLL(((char *)0,               "GARVEN DEH"),           1,   0, 100),
	SCROLL(((char *)0,               "READ ME"),              1,   0, 100),
	/* these must come last because they have special descriptions */
#ifdef MAIL
	SCROLL(("mail",                  "stamped"),          0,   0,   0),
#endif
	SCROLL(("blank paper",           "unlabeled"),        0,  21,  60),

	SCROLL(("Gold Scroll of Law",    "golden"),           0,   0,  10,
		O_MAT(GOLD), O_COLOR(HI_GOLD), O_WT(50)), /* Shopkeepers aren't interested in these */
#undef SCROLL

#define CERAMIC_TILE(names,prob,...) OBJECT( \
		DEF_BLINDNAME(names, "shard"), BITS(0,1,0,0,1,0,0,0,MZ_TINY,0,0,0,0,P_NONE,MINERAL, IDED|UNIDED), 0, \
		TILE_CLASS, prob, 0, 3, 300, {0}, {0}, 0, 0, 0, 6, CLR_WHITE, __VA_ARGS__)
	/* Randomized descriptions */
	CERAMIC_TILE(("syllable of strength: Aesh","bipartite glyph"), 167),
	CERAMIC_TILE(("syllable of power: Krau",   "crossed glyph"),   166),
	CERAMIC_TILE(("syllable of life: Hoon",    "knotted glyph"),   167),
	CERAMIC_TILE(("syllable of grace: Uur",    "multilinear glyph"), 167),
	CERAMIC_TILE(("syllable of thought: Naen", "dotted glyph"),    167),
	CERAMIC_TILE(("syllable of spirit: Vaul",  "hanging glyph"),   166),
#undef CERAMIC_TILE
#define BONE_TILE(names,prob,...) OBJECT( \
		DEF_BLINDNAME(names, "shard"), BITS(0,1,0,0,0,0,0,0,MZ_TINY,0,0,0,0,P_NONE,BONE, IDED|UNIDED), 0, \
		TILE_CLASS, prob, 0, 3, 300, {0}, {0}, 0, 0, 0, 6, CLR_GRAY, __VA_ARGS__)
	BONE_TILE(("anti-clockwise metamorphosis glyph",  "counterclockwise-rotated cross"),   0),	// ANTI_CLOCKWISE_METAMORPHOSIS
	BONE_TILE(("clockwise metamorphosis glyph",  "clockwise-rotated cross"),   0),				// CLOCKWISE_METAMORPHOSIS
	BONE_TILE(("sparkling lake glyph",  "sparkling horizontal line"),   0),	// ARCANE_BULWARK
	BONE_TILE(("fading lake glyph",  "fading horizontal line"),   0),			// DISSIPATING_BULWARK
	BONE_TILE(("smoking lake glyph",  "smoking horizontal line"),   0),		// SMOLDERING_BULWARK
	BONE_TILE(("frosted lake glyph",  "frosted horizontal line"),   0),		// FROSTED_BULWARK
	BONE_TILE(("rapturous eye glyph",  "long-lashed eye"),   0),				// BLOOD_RAPTURE
	BONE_TILE(("clawmark glyph",  "set of many vertical lines"),   0),				// CLAWMARK
	BONE_TILE(("clear sea glyph",  "set of coalescing vertical lines"),   0),		// CLEAR_DEEPS
	BONE_TILE(("deep sea glyph",  "set of coalescing vertical marks"),   0),			// DEEP_SEA
	BONE_TILE(("hidden sea glyph",  "set of coalescing vertical scratches"),   0),		// TRANSPARENT_SEA
	BONE_TILE(("communion glyph",  "weeping-eyed leaf"),   0),				// COMMUNION
	BONE_TILE(("corruption glyph",  "bleeding caduceus"),   0),				// CORRUPTION
	BONE_TILE(("eye glyph",  "pentagonal eye"),   0),							// EYE_THOUGHT
	BONE_TILE(("formless voice glyph",  "thrice-sealed eye"),   0),			// FORMLESS_VOICE
	BONE_TILE(("guidance glyph",  "long-tailed watcher"),   0),					// GUIDANCE
	BONE_TILE(("impurity glyph",  "once-sealed six-sworn eye"),   0),			// IMPURITY
	BONE_TILE(("moon glyph",  "sword-crossed eye"),   0),						// MOON
	BONE_TILE(("writhe glyph",  "writhing vesica piscis"),   0),				// WRITHE
	BONE_TILE(("radiance glyph",  "eyed triangle"),   0),						// RADIANCE
	BONE_TILE(("beast's embrace glyph",  "curling beast's claw"),   0),		// BEASTS_EMBRACE
	BONE_TILE(("orrery glyph",  "set of nested dotted circles"),   0),		// SIGHT
#undef BONE_TILE
#define METALIC_SLAB(names, clr,...) OBJECT( \
		DEF_BLINDNAME(names, "slab"), BITS(0,0,0,0,1,0,1,1,MZ_HUGE,0,0,0,0,P_NONE,METAL, IDED|UNIDED), 0, \
		TILE_CLASS, 0, 0, 3, 3000, {0}, {0}, 0, 0, 0, 6, clr, __VA_ARGS__)
	/* Fixed descriptions (also, artifact base-items only) */
	METALIC_SLAB(("First Word",  "blinding glyph"), CLR_YELLOW),
	METALIC_SLAB(("Dividing Word",  "cerulean glyph"), HI_ZAP),
	METALIC_SLAB(("Nurturing Word",  "verdant glyph"), CLR_GREEN),
	METALIC_SLAB(("Word of Knowledge",  "crimson glyph"), CLR_RED),
#undef METALIC_SLAB

/* spellbooks ... */
#define SPELL(names,sub,prob,level,mgc,dir,color,...) OBJECT( \
		DEF_BLINDNAME(names, "spellbook"), BITS(0,0,0,0,mgc,0,0,0,MZ_LARGE,0,0,dir,0,sub,PAPER,0), 0, \
		SPBOOK_CLASS, prob, level, \
		50, level * 100, {0}, {0}, 0, level, 0, 20, color, __VA_ARGS__)
SPELL(("dig",             "parchment"),   P_MATTER_SPELL,			20, 5, 1, RAY,       HI_PAPER),
SPELL(("magic missile",   "vellum"),      P_ATTACK_SPELL, 		45, 2, 1, RAY,       HI_PAPER),
SPELL(("fireball",        "ragged"),      P_ATTACK_SPELL, 		20, 4, 1, RAY,       HI_PAPER),
SPELL(("cone of cold",    "dog eared"),   P_ATTACK_SPELL, 		20, 4, 1, RAY,       HI_PAPER),
SPELL(("sleep",           "mottled"),     P_ENCHANTMENT_SPELL,	50, 1, 1, RAY,       HI_PAPER),
SPELL(("finger of death", "stained"),     P_ATTACK_SPELL,  		 5, 7, 1, RAY,       HI_PAPER),
SPELL(("lightning bolt",  "storm-hued"),  P_ATTACK_SPELL,  		 0, 5, 1, RAY,       CLR_BLUE, O_NOWISH(1)), /* unwishable */
SPELL(("poison spray",    "snakeskin"),  	P_MATTER_SPELL,  		 0, 4, 1, RAY,       CLR_GREEN, O_NOWISH(1)), /* unwishable *//*Needs tile*/
SPELL(("acid splash",     "acid green"), 	P_MATTER_SPELL,  		 0, 4, 1, RAY,       CLR_BRIGHT_GREEN),/*Needs tile*/
SPELL(("light",           "cloth"),       P_DIVINATION_SPELL,  	45, 1, 1, NODIR,     HI_CLOTH),
SPELL(("fire storm",		 "flame-red"),   P_ATTACK_SPELL,  		 0, 6, 1, NODIR,     CLR_RED),
SPELL(("blizzard",        "snow white"),  P_ATTACK_SPELL,  		 0, 6, 1, NODIR,     CLR_WHITE),
SPELL(("detect monsters", "leather"),     P_DIVINATION_SPELL,	 	43, 2, 1, NODIR,     HI_LEATHER),
SPELL(("healing",         "white"),       P_HEALING_SPELL,  		45, 1, 1, IMMEDIATE, CLR_WHITE),
SPELL(("lightning storm", "ocean blue"),  P_ATTACK_SPELL,  		 0, 7, 1, NODIR,     CLR_BLUE, O_NOWISH(1)), /* unwishable */
SPELL(("knock",           "pink"),        P_MATTER_SPELL,  		30, 1, 1, IMMEDIATE, CLR_BRIGHT_MAGENTA),
SPELL(("force bolt",      "red"),         P_ATTACK_SPELL,  		35, 1, 1, IMMEDIATE, CLR_RED),
SPELL(("confuse monster", "orange"),      P_ENCHANTMENT_SPELL,	30, 2, 1, IMMEDIATE, CLR_ORANGE),
SPELL(("cure blindness",  "yellow"),      P_HEALING_SPELL,  		25, 2, 1, IMMEDIATE, CLR_YELLOW),
SPELL(("drain life",      "velvet"),      P_ATTACK_SPELL,  		10, 2, 1, IMMEDIATE, CLR_MAGENTA),
SPELL(("slow monster",    "light green"), P_ENCHANTMENT_SPELL,	30, 2, 1, IMMEDIATE, CLR_BRIGHT_GREEN),
SPELL(("wizard lock",     "dark green"),  P_MATTER_SPELL,  		25, 2, 1, IMMEDIATE, CLR_GREEN),
// SPELL(("create monster",  "turquoise"),   P_CLERIC_SPELL,  	35, 2, 1, NODIR,     CLR_BRIGHT_CYAN),
SPELL(("turn undead",     "copper"),      P_CLERIC_SPELL,  		25, 2, 1, IMMEDIATE, HI_COPPER),
SPELL(("detect food",     "cyan"),        P_DIVINATION_SPELL,  	30, 2, 1, NODIR,     CLR_CYAN),
SPELL(("cause fear",      "light blue"),  P_ENCHANTMENT_SPELL,  	25, 3, 1, NODIR,     CLR_BRIGHT_BLUE),
SPELL(("clairvoyance",    "dark blue"),   P_DIVINATION_SPELL,  	15, 3, 1, NODIR,     CLR_BLUE),
SPELL(("full healing",    "indigo"),      P_HEALING_SPELL,		12, 7, 1, IMMEDIATE,     CLR_BLUE),
SPELL(("pacify monster",  "fuchsia"),		P_ENCHANTMENT_SPELL,  	10, 3, 1, IMMEDIATE, CLR_MAGENTA),
SPELL(("charm monster",   "magenta"),     P_ENCHANTMENT_SPELL,  	10, 5, 1, IMMEDIATE, CLR_MAGENTA),
SPELL(("haste self",      "purple"),      P_ESCAPE_SPELL,			33, 3, 1, NODIR,     CLR_MAGENTA),
SPELL(("detect unseen",   "violet"),      P_DIVINATION_SPELL,  	20, 3, 1, NODIR,     CLR_MAGENTA),
SPELL(("levitation",      "tan"),         P_ESCAPE_SPELL,			20, 4, 1, NODIR,     CLR_BROWN),
SPELL(("extra healing",   "plaid"),       P_HEALING_SPELL,		32, 3, 1, IMMEDIATE, CLR_GREEN),
SPELL(("mass healing",    "spiralbound"), P_HEALING_SPELL,		 0, 6, 1, NODIR, CLR_GREEN),
SPELL(("restore ability", "light brown"), P_HEALING_SPELL,		30, 4, 1, NODIR,     CLR_BROWN),
SPELL(("invisibility",    "dark brown"),  P_ESCAPE_SPELL,			25, 4, 1, NODIR,     CLR_BROWN),
SPELL(("detect treasure", "gray"),        P_DIVINATION_SPELL,  	20, 4, 1, NODIR,     CLR_GRAY),
SPELL(("remove curse",    "wrinkled"),    P_CLERIC_SPELL,			25, 3, 1, NODIR,     HI_PAPER),
SPELL(("magic mapping",   "dusty"),       P_DIVINATION_SPELL,  	18, 5, 1, NODIR,     HI_PAPER),
SPELL(("identify",        "bronze"),      P_DIVINATION_SPELL,  	20, 3, 1, NODIR,     HI_COPPER),
// SPELL(("turn undead",     "copper"),      P_CLERIC_SPELL,		16, 6, 1, IMMEDIATE, HI_COPPER),
SPELL(("create monster",  "turquoise"),   P_CLERIC_SPELL,			16, 6, 1, NODIR,     CLR_BRIGHT_CYAN),
SPELL(("polymorph",       "silver"),      P_MATTER_SPELL,			10, 7, 1, IMMEDIATE, HI_SILVER),
SPELL(("teleport away",   "gold"),        P_ESCAPE_SPELL,			15, 6, 1, IMMEDIATE, HI_GOLD),
SPELL(("create familiar", "glittering"),  P_CLERIC_SPELL,			10, 6, 1, NODIR,     CLR_WHITE),
SPELL(("cancellation",    "shining"),     P_MATTER_SPELL,			15, 6, 1, IMMEDIATE, CLR_WHITE),
SPELL(("protection",	     "dull"),        P_CLERIC_SPELL,			18, 1, 1, NODIR,     HI_PAPER),
SPELL(("abjuration","ebony"),		P_CLERIC_SPELL,			10, 7, 1, NODIR, CLR_BLACK),
SPELL(("jumping",	     "thin"),        P_ESCAPE_SPELL,			20, 1, 1, IMMEDIATE, HI_PAPER),
SPELL(("stone to flesh",	 "thick"),       P_HEALING_SPELL,		20, 3, 1, IMMEDIATE, HI_PAPER),
#if 0	/* DEFERRED */
SPELL(("flame sphere",    "canvas"),      P_MATTER_SPELL,			20, 1, 1, NODIR, CLR_BROWN),
SPELL(("freeze sphere",   "hardcover"),   P_MATTER_SPELL,			20, 1, 1, NODIR, CLR_BROWN),
#endif
/* blank spellbook must come last because it retains its description */
SPELL(("blank paper",     "plain"),       P_NONE,					18, 0, 0, 0,         HI_PAPER),

/* a special, one of a kind, spellbook */
SPELL(("Book of the Dead","papyrus"),     P_NONE,                  0, 7, 1, 0,         HI_PAPER,
	O_USKWN(1), O_DELAY(0), O_WT(20), O_COST(10000), O_UNIQ(1), O_NOWISH(1)),
/* base item for many artifact spellbooks */
SPELL(("secrets", "ragged leather"),      P_NONE,                  0, 7, 1, 0,         CLR_BROWN,
	O_USKWN(1), O_DELAY(0), O_WT(20), O_COST(10000), O_NOWISH(1)),

#undef SPELL

/* wands ... */
#define WAND(names,prob,cost,mgc,dir,metal,color,...) OBJECT( \
		DEF_BLINDNAME(names, "wand"), BITS(0,0,1,0,mgc,1,0,0,MZ_TINY,0,0,dir,0,P_NONE,metal,0), 0, \
		WAND_CLASS, prob, 0, 7, cost, {0}, {0}, 0, 0, 0, 30, color, __VA_ARGS__)
WAND(("light",          "glass"),         90, 100, 1, NODIR,     GLASS,       CLR_WHITE),/*Needs tile?*/
WAND(("darkness",       "obsidian"),      10, 100, 1, NODIR,     OBSIDIAN_MT, CLR_BLACK),/*Needs tile*/
WAND(("wishing",        "dragon-bone"),    0, 500, 1, NODIR,     DRAGON_HIDE, CLR_WHITE, O_NOWISH(1)),	/* wizmode only */
WAND(("secret door detection", "balsa"),  50, 150, 1, NODIR,	   WOOD,        HI_WOOD),
WAND(("enlightenment",  "crystal"),       18, 150, 1, NODIR,     GLASS,       HI_GLASS),
WAND(("create monster", "maple"),         42, 200, 1, NODIR,     WOOD,        HI_WOOD),
WAND(("nothing",        "oak"),           25, 100, 0, IMMEDIATE, WOOD,        HI_WOOD),
WAND(("striking",       "ebony"),         75, 150, 1, IMMEDIATE, WOOD,        HI_WOOD),
WAND(("draining",       "ceramic"),        5, 175, 1, IMMEDIATE, MINERAL,     HI_MINERAL),
WAND(("make invisible", "marble"),        42, 150, 1, IMMEDIATE, MINERAL,     HI_MINERAL),
WAND(("slow monster",   "tin"),           46, 150, 1, IMMEDIATE, METAL,       HI_METAL),
WAND(("speed monster",  "brass"),         50, 150, 1, IMMEDIATE, COPPER,      HI_COPPER),
WAND(("undead turning", "copper"),        50, 150, 1, IMMEDIATE, COPPER,      HI_COPPER),
WAND(("polymorph",      "silver"),        45, 200, 1, IMMEDIATE, SILVER,      HI_SILVER),
WAND(("cancellation",   "platinum"),      42, 200, 1, IMMEDIATE, PLATINUM,    CLR_WHITE),
WAND(("teleportation",  "iridium"),       45, 200, 1, IMMEDIATE, METAL,       CLR_BRIGHT_CYAN),
WAND(("opening",        "zinc"),          30, 150, 1, IMMEDIATE, METAL,       HI_METAL),
WAND(("locking",        "aluminum"),      25, 150, 1, IMMEDIATE, METAL,       HI_METAL),
WAND(("probing",        "uranium"),       30, 150, 1, IMMEDIATE, METAL,       HI_METAL),
WAND(("digging",        "iron"),          55, 150, 1, RAY,       IRON,        HI_METAL),
WAND(("magic missile",  "steel"),         50, 150, 1, RAY,       IRON,        HI_METAL),
WAND(("fire",           "hexagonal"),     40, 175, 1, RAY,       IRON,        HI_METAL),
WAND(("cold",           "short"),         40, 175, 1, RAY,       IRON,        HI_METAL),
WAND(("sleep",          "runed"),         50, 175, 1, RAY,       IRON,        HI_METAL),
WAND(("death",          "long"),           5, 500, 1, RAY,       IRON,        HI_METAL),
WAND(("lightning",      "curved"),        40, 175, 1, RAY,       IRON,        HI_METAL),
WAND(((char *)0,        "pine"),           0, 150, 1, 0,         WOOD,        HI_WOOD),
WAND(((char *)0,        "forked"),         0, 150, 1, 0,         WOOD,        HI_WOOD),
WAND(((char *)0,        "spiked"),         0, 150, 1, 0,         IRON,        HI_METAL),
WAND(((char *)0,        "jeweled"),        0, 150, 1, 0,         IRON,        HI_MINERAL),
#undef WAND

/* coins ... - so far, gold is all there is */
#define COIN(names,prob,metal,worth,...) OBJECT( \
		names, BITS(0,1,0,0,0,0,0,0,MZ_TINY,0,0,0,0,P_NONE,metal,0), 0, \
		COIN_CLASS, prob, 0, 1, worth, {0}, {0}, 0, 0, 0, 0, HI_GOLD, __VA_ARGS__)
	COIN(("gold piece"),      1000, GOLD,1),
#undef COIN

/* soul coins */
#define SCOIN(names, prob, worth, clr,...) OBJECT( \
		names, BITS(0,1,0,0,0,0,0,0,MZ_TINY,0,0,0,0,P_NONE, LEAD,0), 0, \
		SCOIN_CLASS, prob, 0, 1, worth, {0}, {0}, 0, 0, 0, 0, clr, __VA_ARGS__)
	SCOIN(("wage of sloth", "black-eyed coin", "strange coin"),		270, 99, CLR_BLACK),
	SCOIN(("wage of lust", "blue-eyed coin", "strange coin"),		243, 99, CLR_BRIGHT_BLUE),
	SCOIN(("wage of gluttony", "orange-eyed coin", "strange coin"),	223, 99, CLR_ORANGE),
	SCOIN(("wage of greed", "gold-eyed coin", "strange coin"),		142, 99, HI_GOLD),
	SCOIN(("wage of wrath", "red-eyed coin", "strange coin"),		61,  99, CLR_RED),
	SCOIN(("wage of envy", "green-eyed coin", "strange coin"),		41,  99, CLR_BRIGHT_GREEN),
	SCOIN(("wage of pride", "purple-eyed coin", "strange coin"),	20,  99, CLR_MAGENTA),
#undef SCOIN

/* gems ... - includes stones and rocks but not boulders */
#define GEM(names,prob,wt,gval,nutr,mohs,glass,color,...) OBJECT( \
	    DEF_BLINDNAME(names, "gem"), \
	    BITS(0,1,0,0,0,0,0,0,MZ_TINY,0,HARDGEM(mohs),0,PIERCE,-P_SLING,glass,0), 0, \
		GEM_CLASS, prob, 0, 1, gval, DMG(D(3)), DMG(D(3)), 0, WP_GENERIC, 0, nutr, color, __VA_ARGS__)
#define ROCK(names,kn,prob,wt,gval,sdam,ldam,hitbon,mgc,nutr,mohs,glass,color,...) OBJECT( \
	    DEF_BLINDNAME(names, "stone"), \
	    BITS(kn,1,0,0,mgc,0,0,0,MZ_TINY,0,HARDGEM(mohs),0,WHACK,-P_SLING,glass,0), 0, \
		GEM_CLASS, prob, 0, wt, gval, DMG(D(sdam)), DMG(D(ldam)), hitbon, WP_GENERIC, 0, nutr, color, __VA_ARGS__)
GEM(("magicite crystal","brilliant blue"),1, 1, 9999, 15, 11, GEMSTONE, CLR_BRIGHT_BLUE),/*Needs tile*/
GEM(("dilithium crystal", "white"),      2,  1, 4500, 15,  5, GEMSTONE, CLR_WHITE),
GEM(("diamond", "white"),                3,  1, 4000, 15, 10, GEMSTONE, CLR_WHITE),
//3
GEM(("star sapphire", "blue"),           2,  1, 3750, 15,  9, GEMSTONE, CLR_BLUE),/*Needs tile*/
GEM(("ruby", "red"),                     4,  1, 3500, 15,  9, GEMSTONE, CLR_RED),
GEM(("jacinth", "orange"),               3,  1, 3250, 15,  9, GEMSTONE, CLR_ORANGE),
//6
GEM(("sapphire", "blue"),                4,  1, 3000, 15,  9, GEMSTONE, CLR_BLUE),
GEM(("black opal", "black"),             3,  1, 2500, 15,  8, GEMSTONE, CLR_BLACK),
GEM(("emerald", "green"),                3,  1, 2500, 15,  8, GEMSTONE, CLR_GREEN),
//9
GEM(("turquoise", "green"),              4,  1, 2000, 15,  6, GEMSTONE, CLR_GREEN),
GEM(("morganite", "pink"),               4,  1, 2000, 15,  6, GEMSTONE, CLR_ORANGE),
//11 Note: Only first 11 gems are affected by dungeon level
GEM(("citrine", "yellow"),               4,  1, 1500, 15,  6, GEMSTONE, CLR_YELLOW),
GEM(("aquamarine", "green"),             6,  1, 1500, 15,  8, GEMSTONE, CLR_GREEN),
GEM(("amber", "yellowish brown"),        8,  1, 1000, 15,  2, GEMSTONE, CLR_BROWN),
GEM(("topaz", "yellowish brown"),       10,  1,  900, 15,  8, GEMSTONE, CLR_BROWN),
GEM(("jet", "black"),                    6,  1,  850, 15,  7, GEMSTONE, CLR_BLACK),
GEM(("opal", "white"),                  12,  1,  800, 15,  6, GEMSTONE, CLR_WHITE),
GEM(("chrysoberyl", "yellow"),           8,  1,  700, 15,  5, GEMSTONE, CLR_YELLOW),
GEM(("garnet", "red"),                  12,  1,  700, 15,  7, GEMSTONE, CLR_RED),
GEM(("amethyst", "violet"),             14,  1,  600, 15,  7, GEMSTONE, CLR_MAGENTA),
GEM(("jasper", "red"),                  15,  1,  500, 15,  7, GEMSTONE, CLR_RED),
GEM(("violet fluorite", "violet"),       4,  1,  400, 15,  4, GEMSTONE, CLR_MAGENTA),/*Needs tile*/
GEM(("blue fluorite", "blue"),           4,  1,  400, 15,  4, GEMSTONE, CLR_BLUE),/*Needs tile*/
GEM(("white fluorite", "white"),         4,  1,  400, 15,  4, GEMSTONE, CLR_WHITE),/*Needs tile*/
GEM(("green fluorite", "green"),         4,  1,  400, 15,  4, GEMSTONE, CLR_GREEN),/*Needs tile*/
GEM(("obsidian", "black"),               9,  1,  200, 15,  6, GEMSTONE, CLR_BLACK),
GEM(("agate", "orange"),                12,  1,  200, 15,  6, GEMSTONE, CLR_ORANGE),
GEM(("jade", "green"),                  10,  1,  300, 15,  6, GEMSTONE, CLR_GREEN),
GEM(("worthless piece of white glass", "white"),   76, 1, 0, 6, 5, GLASS, CLR_WHITE),
GEM(("worthless piece of blue glass", "blue"),     76, 1, 0, 6, 5, GLASS, CLR_BLUE),
GEM(("worthless piece of red glass", "red"),       76, 1, 0, 6, 5, GLASS, CLR_RED),
GEM(("worthless piece of yellowish brown glass", "yellowish brown"),
						 77, 1, 0, 6, 5, GLASS, CLR_BROWN),
GEM(("worthless piece of orange glass", "orange"), 76, 1, 0, 6, 5, GLASS, CLR_ORANGE),
GEM(("worthless piece of yellow glass", "yellow"), 76, 1, 0, 6, 5, GLASS, CLR_YELLOW),
GEM(("worthless piece of black glass",  "black"),  76, 1, 0, 6, 5, GLASS, CLR_BLACK),
GEM(("worthless piece of green glass", "green"),   77, 1, 0, 6, 5, GLASS, CLR_GREEN),
GEM(("worthless piece of violet glass", "violet"), 77, 1, 0, 6, 5, GLASS, CLR_MAGENTA),

/* Placement note: there is a wishable subrange for
 * "gray stones" in the o_ranges[] array in objnam.c
 * that is currently everything between luckstones and flint (inclusive).
 */
ROCK(("luckstone", "gray"),	0,  8,   1, 60, 6, 6, 20, 1, 10, 7, MINERAL, CLR_GRAY),
ROCK(("loadstone", "gray"),	0,  8, 500,  1,30,30, -5, 1, 10, 6, MINERAL, CLR_GRAY),
ROCK(("touchstone", "gray"),0,  8,  1,45, 6,  6, 0, 1, 10, 6, MINERAL, CLR_GRAY),
ROCK(("flint", "gray"),		0,  8,   1,  1, 6, 6,  2, 0, 10, 7, MINERAL, CLR_GRAY, O_DTYPE(SLASH)),	/* does slashing damage, not blunt */
ROCK(("vital soulstone", "gray"),
							0,  3,   1, 99, 3, 3,  0, 1, 10, 1, MINERAL, CLR_GRAY),
ROCK(("spiritual soulstone", "gray"),
							0,  3,   1, 99, 3, 3,  0, 1, 10, 1, MINERAL, CLR_GRAY),
ROCK(("chunk of unrefined mithril", "silvery metal"), 
							0,  0,   1, 10000,3,3, 3, 0, 0, 10, MITHRIL, HI_MITHRIL),/*Needs tile*/
ROCK(("chunk of fossil dark", "black"),
							0,  0,  25, 	500, 8, 8, 4, 1, 0, 1, MINERAL, CLR_BLACK),/*Needs tile*/
ROCK(("chunk of salt", "white"),		
							0,  0,   1,  1, 3, 3,  0, 0, 10, 2, SALT, CLR_GRAY), /*Needs tile*/
ROCK(("silver slingstone", "silver"), 
							0,  0,   1, 10, 6, 6, 2, 0,  0, 5, SILVER, HI_SILVER),/*Needs tile*/
ROCK(("rock"),		1,100,   1, 0, 6, 6, 0, 0, 10, 7, MINERAL, CLR_GRAY),

GEM(("antimagic rift", "black-flawed white"),  0,  1, 4500, 15,  6, GEMSTONE, CLR_BLACK),
GEM(("catapsi vortex",  "gray-flawed white"),  0,  1, 4500, 15,  6, GEMSTONE, CLR_GRAY),
#undef GEM
#undef ROCK

/* miscellaneous ... */
/* Note: boulders and rocks are not normally created at random; the
 * probabilities only come into effect when you try to polymorph them.
 * Boulders weigh more than MAX_CARR_CAP; statues use corpsenm to take
 * on a specific type and may act as containers (both affect weight).
 */
//OBJECT(names,bits,prp,sym,prob,dly,wt,cost,sdam,ldam,oc1,oc2,oc3,nut,color,...)
								//BITS(nmkn,mrg,uskn,ctnr,mgc,chrg,uniq,nwsh,big,tuf,dir,dtyp,sub,mtrl,shwmat)
OBJECT(("boulder",(char *)0), BITS(1,0,0,0,0,0,0,0,MZ_GIGANTIC,0,0,0,0,P_NONE,MINERAL,0), 0,
		ROCK_CLASS,   100, 0, 6000,  0, DMG(D(20)), DMG(D(20)), 0, 0, 0, 2000, HI_MINERAL),
OBJECT(("mass of stuff",(char *)0), BITS(1,0,0,0,0,0,0,0,MZ_GIGANTIC,0,0,0,P|B|S,P_NONE,GOLD,0), 0,
		ROCK_CLASS,     0, 0, 9000, 9999, DMG(D(4,5)), DMG(D(5,4)), 0, 0, 0, 2000, HI_GOLD),
OBJECT(("statue"), BITS(1,0,0,1,0,0,0,0,    MZ_HUGE,0,0,0,0,P_NONE,MINERAL,0), 0,
		ROCK_CLASS,   800, 0, 2500,  0, DMG(D(20)), DMG(D(20)), 2, 0, 2, 2500, CLR_WHITE),
OBJECT(("fossil"), BITS(1,0,0,0,0,0,0,0,    MZ_HUGE,0,0,0,0,P_NONE,MINERAL,0), 0,
		ROCK_CLASS,   100, 0, 2500,  0, DMG(D(20)), DMG(D(20)), 0, 0, 0, 2500, CLR_BROWN),
OBJECT(("bed"), BITS(1,0,0,0,0,0,0,0,MZ_LARGE,1,0,0,0,P_NONE,WOOD,0), 0,
		BED_CLASS,   900, 0, 2000,  1000, DMG(D(20)), DMG(D(20)), 0, 0, 0, 2000, HI_WOOD),
OBJECT(("bedroll"), BITS(1,0,0,0,0,0,0,0,MZ_SMALL,1,0,0,0,P_NONE,CLOTH,0), 0,
		BED_CLASS,   100, 0, 60,  100, DMG(D(2)), DMG(D(2)), 0, 0, 0, 2000, CLR_GREEN),
OBJECT(("gurney"), BITS(1,0,0,0,0,0,0,0,MZ_MEDIUM,1,0,0,0,P_NONE,METAL,0), 0,
		BED_CLASS,     0, 0, 60,  100, DMG(D(4)), DMG(D(4)), 0, 0, 0, 2000, CLR_WHITE),

#ifdef CONVICT
OBJECT(("heavy iron ball"), BITS(1,0,0,0,0,0,0,0,MZ_LARGE,0,0,0,WHACK,P_FLAIL,IRON,0), 0,
#else
OBJECT(("heavy iron ball"), BITS(1,0,0,0,0,0,0,0,MZ_LARGE,0,0,0,WHACK,P_NONE,IRON,0), 0,
#endif /* CONVICT */
		BALL_CLASS,  1000, 0,  480, 10, DMG(D(25)), DMG(D(25)), 0, 0,  0, 200, HI_METAL),
						/* +d4 when "very heavy" */

#define CHAIN(names,sdam,ldam,nutr,wt,cost,hitbon,dtyp,mat,color,...) OBJECT( \
	    names, \
	    BITS(1,1,0,0,0,0,0,0,MZ_MEDIUM,0,0,0,dtyp,P_NONE,mat,0), 0, \
		CHAIN_CLASS, 0, 0, wt, cost, sdam, ldam, hitbon, WP_GENERIC, 0, nutr, color, __VA_ARGS__)
					
CHAIN(("chain"),
	DMG(D(4), F(1)), DMG(D(4), F(1)),
	200, 120, 0, 4, B, IRON, HI_METAL,
	O_MATSPEC(IDED|UNIDED), O_MERGE(0), O_SKILL(P_FLAIL), O_PROB(1000)),
CHAIN(("sheaf of hay"),
	DMG(D(1)), DMG(D(1)),
	100,  20, 2, 0, 0, VEGGY, CLR_YELLOW),
CHAIN(("clockwork component"),
	DMG(D(1)), DMG(D(1)),
	 20,   1, 0, 0, B, COPPER, HI_COPPER),
CHAIN(("subethaic component"),
	DMG(D(1)), DMG(D(1)),
	 20,   1, 0, 0, B, GLASS, HI_GLASS),
CHAIN(("scrap"),
	DMG(D(1)), DMG(D(1)),
	 20,   1, 0, 0, B, IRON, CLR_BROWN),
CHAIN(("hellfire component"),
	DMG(D(1)), DMG(D(1)),
	 20,   1, 0, 0, B, METAL, CLR_ORANGE),

#define CHAINCORPSE(names,wt,mat,color,...)\
	CHAIN(names,{0},{0},20,wt,0,0,0,mat,color,O_MERGE(0),O_SIZE(MZ_HUGE),__VA_ARGS__)

CHAINCORPSE(("broken android"), 3000, METAL, CLR_WHITE), /*Needs encyc entry*//*Needs tile*/
CHAINCORPSE(("broken gynoid"),  3000, METAL, CLR_WHITE), /*Needs encyc entry*//*Needs tile*/
CHAINCORPSE(("lifeless doll"),   750, WOOD, CLR_BRIGHT_MAGENTA), /*Needs encyc entry*//*Needs tile*/

#undef CHAINCORPSE

#define CHAINROPE(names,dtyp,mat,color,...)\
	CHAIN(names,DMG(D(4)),DMG(D(4)),200,120,0,0,dtyp,mat,color,O_MERGE(0),__VA_ARGS__)

CHAINROPE(("rope of entangling"),   B, VEGGY, CLR_BROWN),
CHAINROPE(("iron bands"),           B, IRON,  HI_METAL),
CHAINROPE(("razor wire"),           S, METAL, HI_METAL),
CHAINROPE(("shackles"),             B, IRON,  HI_METAL),

#undef CHAINROPE

#undef CHAIN

OBJECT(("blinding venom", "splash of venom"),
		BITS(0,1,0,0,0,0,0,1,MZ_TINY,0,0,0,0,P_NONE,LIQUID,0), 0,
		VENOM_CLASS,  500, 0,	 1,  0,  {0},  {0}, 0, 0,	 0, 0, CLR_BLACK),
OBJECT(("acid venom", "splash of venom"),
		BITS(0,1,0,0,0,0,0,1,MZ_TINY,0,0,0,0,P_NONE,LIQUID,0), 0,
		VENOM_CLASS,  500, 0,	 1,  0,  DMG(D(6)),  DMG(D(6)), 0, 0,	 0, 0, CLR_BRIGHT_GREEN),
OBJECT(("ball of webbing"),/*Needs tile*/
		BITS(1,1,0,0,0,0,0,1,MZ_TINY,0,0,0,0,P_NONE,LIQUID,0), 0,
		VENOM_CLASS,  0, 0,	 1,  0,  DMG(D(6)),  DMG(D(6)), 0, 0,	 0, 0, CLR_WHITE),
//OBJECT(("shoggoth venom", "splash of venom"),
//		BITS(0,1,0,0,0,0,0,1,MZ_TINY,0,0,0,0,P_NONE,LIQUID,0), 0,
//		VENOM_CLASS,  500, 0,	 1,  0,  {0},  {0}, 0, 0,	 0, 0, HI_ORGANIC),
		/* +d6 small or large */

/* fencepost, the deadly Array Terminator -- name [1st arg] *must* be NULL */
	OBJECT(((char *)0,(char *)0), BITS(0,0,0,0,0,0,0,0,0,0,0,0,0,P_NONE,0,0), 0,
		ILLOBJ_CLASS, 0, 0, 0, 0, {0}, {0}, 0, 0, 0, 0, 0)
}; /* objects[] */

#ifndef OBJECTS_PASS_2_

/* perform recursive compilation for second structure */
#  undef OBJ
#  undef OBJECT
#  define OBJECTS_PASS_2_
#include "objects.c"

void NDECL(objects_init);

/* dummy routine used to force linkage */
void
objects_init()
{
    return;
}

#endif	/* !OBJECTS_PASS_2_ */

/*objects.c*/
