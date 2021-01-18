/*	SCCS Id: @(#)mplayer.c	3.4	1997/02/04	*/
/*	Copyright (c) Izchak Miller, 1992.			  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"


STATIC_DCL const char *NDECL(dev_name);
STATIC_DCL void FDECL(get_mplname, (struct monst *, char *));
STATIC_DCL void FDECL(mk_mplayer_armor, (struct monst *, int));

/* These are the names of those who
 * contributed to the development of NetHack 3.2/3.3/3.4.
 *
 * Keep in alphabetical order within teams.
 * Same first name is entered once within each team.
 */
static const char *developers[] = {
	/* devteam */
	"Dave", "Dean", "Eric", "Izchak", "Janet", "Jessie",
	"Ken", "Kevin", "Michael", "Mike", "Pat", "Paul", "Steve", "Timo",
	"Warwick",
	/* PC team */
	"Bill", "Eric", "Keizo", "Ken", "Kevin", "Michael", "Mike", "Paul",
	"Stephen", "Steve", "Timo", "Yitzhak",
	/* Amiga team */
	"Andy", "Gregg", "Janne", "Keni", "Mike", "Olaf", "Richard",
	/* Mac team */
	"Andy", "Chris", "Dean", "Jon", "Jonathan", "Kevin", "Wang",
	/* Atari team */
	"Eric", "Marvin", "Warwick",
	/* NT team */
	"Alex", "Dion", "Michael",
	/* OS/2 team */
	"Helge", "Ron", "Timo",
	/* VMS team */
	"Joshua", "Pat",
	""};


/* return a randomly chosen developer name */
STATIC_OVL const char *
dev_name()
{
	register int i, m = 0, n = SIZE(developers);
	register struct monst *mtmp;
	register boolean match;

	do {
	    match = FALSE;
	    i = rn2(n);
	    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if(!is_mplayer(mtmp->data)) continue;
		if(!M_HAS_NAME(mtmp)) continue;
		if(!strncmp(developers[i], MNAME(mtmp),
			               strlen(developers[i]))) {
		    match = TRUE;
		    break;
	        }
	    }
	    m++;
	} while (match && m < 100); /* m for insurance */

	if (match) return (const char *)0;
	return(developers[i]);
}

STATIC_OVL void
get_mplname(mtmp, nam)
register struct monst *mtmp;
char *nam;
{
	boolean fmlkind = is_female(mtmp->data);
	const char *devnam;

	devnam = dev_name();
	if (!devnam)
	    Strcpy(nam, fmlkind ? "Eve" : "Adam");
	else if (fmlkind && !!strcmp(devnam, "Janet"))
	    Strcpy(nam, rn2(2) ? "Maud" : "Eve");
	else Strcpy(nam, devnam);

	if (fmlkind || !strcmp(nam, "Janet"))
	    mtmp->female = 1;
	else
	    mtmp->female = 0;
	Strcat(nam, " the ");
	Strcat(nam, rank_of((int)mtmp->m_lev,
			    monsndx(mtmp->data),
			    (boolean)mtmp->female));
}

STATIC_OVL void
mk_mplayer_armor(mon, typ)
struct monst *mon;
int typ;
{
	struct obj *obj;

	if (typ == STRANGE_OBJECT) return;
	obj = mksobj(typ, MKOBJ_NOINIT);
	if (!rn2(3)) obj->oerodeproof = 1;
	else if (!rn2(2)) obj->greased = 1;
	if (!rn2(3)) curse(obj);
	if (!rn2(3)) bless(obj);
	/* Most players who get to the endgame who have cursed equipment
	 * have it because the wizard or other monsters cursed it, so its
	 * chances of having plusses is the same as usual....
	 */
	obj->spe = rn2(10) ? (rn2(3) ? rn2(5) : rn1(4,4)) : -rnd(3);
	(void) mpickobj(mon, obj);
}

void
init_mplayer_gear(ptr, special, weapon, secweapon, rweapon, rwammo, armor, shirt, cloak, helm, boots, gloves, shield, tool)
register struct permonst *ptr;
boolean special;
int *weapon, *secweapon, *rweapon, *rwammo, *armor, *shirt, *cloak, *helm, *boots, *gloves, *shield, *tool;
{
	//Default to nothing
	*weapon = STRANGE_OBJECT;
	*secweapon = STRANGE_OBJECT;
	*rweapon = STRANGE_OBJECT;
	*rwammo = STRANGE_OBJECT;
	*armor = STRANGE_OBJECT;
	*shirt = STRANGE_OBJECT;
	*cloak = STRANGE_OBJECT;
	*helm = STRANGE_OBJECT;
	*boots = STRANGE_OBJECT;
	*gloves = STRANGE_OBJECT;
	*shield = STRANGE_OBJECT;
	*tool = STRANGE_OBJECT;
	switch(monsndx(ptr)) {
	case PM_ARCHEOLOGIST:
		*weapon = BULLWHIP;
		*armor = JACKET;
		*helm = FEDORA;
		*boots = HIGH_BOOTS;
		*tool = PICK_AXE;
	break;
	case PM_ANACHRONONAUT:
		if(special){
			if (!rn2(2)) *weapon = LIGHTSABER;
			else *weapon = FORCE_PIKE;
			*rweapon = ARM_BLASTER;
			*rwammo = FRAG_GRENADE;
			*shield = CRYSTAL_SHIELD;
			*armor = CRYSTAL_PLATE_MAIL;
			*shirt = BODYGLOVE;
			*helm = CRYSTAL_HELM;
			*gloves = CRYSTAL_GAUNTLETS;
			*boots = CRYSTAL_BOOTS;
		} else {
			if (!rn2(4)) *weapon = FORCE_PIKE;
			else *weapon = VIBROBLADE;
			*rweapon = ASSAULT_RIFLE;
			*rwammo = BULLET;
			*shield = CRYSTAL_SHIELD;
			*armor = PLASTEEL_ARMOR;
			*helm = FLACK_HELMET;
			*gloves = PLASTEEL_GAUNTLETS;
			*boots = HIGH_BOOTS;
		}
	break;
	case PM_BARBARIAN:
	case PM_HALF_DRAGON:{
		if (rn2(2)) {
			*weapon = rn2(2) ? TWO_HANDED_SWORD : BATTLE_AXE;
		}
		*helm = HELMET;
		*armor = CHAIN_MAIL;
		*boots = GLOVES;
		*boots = HIGH_BOOTS;
		*tool = TORCH;
	}break;
	case PM_BARD:{
		static int trotyp[] = {
			FLUTE, TOOLED_HORN, HARP,
			BELL, BUGLE, DRUM
		};
		*tool = trotyp[rn2(SIZE(trotyp))];
		*armor = rn2(2) ? ELVEN_MITHRIL_COAT : ELVEN_TOGA;
		*cloak = rn2(2) ? DWARVISH_CLOAK : CLOAK;
		*boots = HIGH_BOOTS;
	}break;
	case PM_CAVEMAN:
	case PM_CAVEWOMAN:
		if (rn2(4)) *weapon = MACE;
		else *weapon = CLUB;
		if(special){
			*cloak = LEO_NEMAEUS_HIDE;
		} else {
			*cloak = CLOAK;
			*rweapon = FLINT;
			*rwammo = ROCK;
		}
	break;
#ifdef PM_CONVICT
	case PM_CONVICT:
		if (rn2(4)) *weapon = FLAIL;
		else if(rn2(3)) *weapon = HEAVY_IRON_BALL;
		else *weapon = SPOON;
		if(special){
			*helm = find_vhelm();
			*cloak = find_cope();
			// OBJECT:'#',"blessed iron +2 magic-resistant visored helmet named Mask of Waterdeep",contained
			// OBJECT:'#',"blessed +2 ornamental cope named Masked Lord's Cope",contained
		} else {
			*shirt = STRIPED_SHIRT;
		}
	break;
#endif
	case PM_EXILE:
		if(!special){
			*weapon = SCYTHE;
			*rweapon = SLING;
			*rwammo = ROCK;
			*cloak = CLOAK;
		}
	break;
	case PM_HEALER:
		if (rn2(4)){
			*weapon = QUARTERSTAFF;
		} else if (rn2(2)) *weapon = rn2(2) ? UNICORN_HORN : SCALPEL;
		if (special && rn2(4)) *helm = rn2(2) ? HELM_OF_BRILLIANCE : HELM_OF_TELEPATHY;
		*armor = HEALER_UNIFORM;
		*boots = LOW_BOOTS;
		*tool = POT_EXTRA_HEALING;
	break;
	case PM_INCANTIFIER:
		if(Role_if(PM_ANACHRONONAUT) && In_quest(&u.uz)){
			*weapon = rn2(2) ? DOUBLE_LIGHTSABER : LIGHTSABER;
			*armor = !rn2(3) ? JUMPSUIT : 
					 rn2(2) ? BODYGLOVE :
					 PLASTEEL_ARMOR;
			*helm = HELM_OF_BRILLIANCE;
			*cloak = rn2(4) ? ROBE : CLOAK_OF_PROTECTION;
		} else {
			if (rn2(4)) *weapon = rn2(2) ? QUARTERSTAFF : ATHAME;
			if(special){
				*armor = rn2(2) ? BLACK_DRAGON_SCALE_MAIL :
						SILVER_DRAGON_SCALE_MAIL;
			}
			*cloak = ROBE;
			if (rn2(4)) *helm = HELM_OF_BRILLIANCE;
		}
	break;
	case PM_KNIGHT:
		*weapon = LONG_SWORD;
		*rweapon = LANCE;
		*shield = KITE_SHIELD;
		*helm = HELMET;
		*armor = PLATE_MAIL;
		*gloves = GAUNTLETS;
		*boots = ARMORED_BOOTS;
	break;
	case PM_MONK:
		*helm = SEDGE_HAT;
		*gloves = GLOVES;
		*boots = LOW_BOOTS;
		*cloak = ROBE;
	break;
	case PM_NOBLEMAN:
		*weapon = special ? LONG_SWORD : RAPIER;
		*armor = special ? CRYSTAL_PLATE_MAIL : GENTLEMAN_S_SUIT;
		*shirt = RUFFLED_SHIRT;
		*cloak = special ? find_opera_cloak() : CLOAK;
		*gloves = special ? CRYSTAL_GAUNTLETS : GLOVES;
		*boots = special ? CRYSTAL_BOOTS : HIGH_BOOTS;
	break;
	case PM_NOBLEWOMAN:
		*weapon = special ? RAKUYO : RAPIER;
		*armor = special ? NOBLE_S_DRESS : GENTLEWOMAN_S_DRESS;
		*shirt = special ? BLACK_DRESS : VICTORIAN_UNDERWEAR;
		*cloak = special ? find_opera_cloak() : CLOAK;
		*gloves = GLOVES;
		*boots = STILETTOS;
	break;
	case PM_PRIEST:
	case PM_PRIESTESS:
		*weapon = MACE;
		if (special && rn2(2)) *armor = PLATE_MAIL;
		*cloak = ROBE;
		if (special && rn2(4)) *helm = rn2(2) ? HELM_OF_BRILLIANCE : HELM_OF_TELEPATHY;
		if (rn2(2)) *shield = BUCKLER;
		*boots = LOW_BOOTS;
		*tool = POT_WATER;
	break;
	case PM_PIRATE:
		*weapon = SCIMITAR;
		*armor = JACKET;
		*shield = BUCKLER;
		*rweapon = FLINTLOCK;
		*rwammo = BULLET;
		*gloves = GLOVES;
		*boots = HIGH_BOOTS;
	break;
	case PM_RANGER:
		*weapon = LONG_SWORD;
		*armor = LEATHER_ARMOR;
		*helm = LEATHER_HELM;
		*rweapon = BOW;
		*rwammo = special ? SILVER_ARROW : ARROW;
		*gloves = GLOVES;
		*boots = HIGH_BOOTS;
	break;
	case PM_ROGUE:
		*weapon = SHORT_SWORD;
		*secweapon = STILETTO;
		*rwammo = DAGGER;
		*helm = LEATHER_HELM;
		*armor = STUDDED_LEATHER_ARMOR;
		*gloves = GLOVES;
		*boots = LOW_BOOTS;
	break;
	case PM_SAMURAI:
		*weapon = KATANA;
		*secweapon = SHORT_SWORD;
		*rweapon = YUMI;
		*rwammo = YA;
		*helm = HELMET;
		*armor = SPLINT_MAIL;
		*gloves = GAUNTLETS;
		*boots = ARMORED_BOOTS;
		*tool = MASK;
	break;
#ifdef TOURIST
	case PM_TOURIST:
		if (special && !rn2(4)) *weapon = LIGHTSABER;
		*rweapon = EXPENSIVE_CAMERA;
		*rwammo = DART;
		*shirt = HAWAIIAN_SHIRT;
		*boots = LOW_BOOTS;
		*tool = CREDIT_CARD;
	break;
#endif
	case PM_VALKYRIE:
		if (rn2(2)) *weapon = WAR_HAMMER;
		else *weapon = SPEAR;
		*rweapon = BOW;
		*rwammo = ARROW;
		if(special){
			*helm = HELMET;
			*armor = PLATE_MAIL;
			*gloves = GAUNTLETS;
			*boots = ARMORED_BOOTS;
			*shield = KITE_SHIELD;
		} else {
			*armor = LEATHER_ARMOR;
			*gloves = GLOVES;
			*boots = HIGH_BOOTS;
			*shield = BUCKLER;
		}
	break;
	case PM_WORM_THAT_WALKS:
		*weapon = ATHAME;
		*armor = ARCHAIC_PLATE_MAIL;
		*cloak = MUMMY_WRAPPING;
		*shield = SHIELD_OF_REFLECTION;
		*boots = ARMORED_BOOTS;
	break;
	case PM_WIZARD:
	   *weapon = rn2(2) ? QUARTERSTAFF : ATHAME;
		if (special && rn2(2)) {
			*armor = rn2(2) ? BLACK_DRAGON_SCALE_MAIL :
					SILVER_DRAGON_SCALE_MAIL;
		}
		*cloak = special ? CLOAK_OF_MAGIC_RESISTANCE : CLOAK;
		if(special){
			if (rn2(4)) *helm = rn2(2) ? CORNUTHAUM : HELM_OF_BRILLIANCE;
		}
	break;
	default:
		pline("Received %d.",monsndx(ptr));
		impossible("bad mplayer monster");
		weapon = 0;
		break;
	}
}


struct monst *
mk_mplayer(ptr, x, y, special)
register struct permonst *ptr;
xchar x, y;
register boolean special;
{
	register struct monst *mtmp;
	char nam[PL_NSIZ];

	if(!is_mplayer(ptr))
		return((struct monst *)0);

	if(MON_AT(x, y))
		(void) rloc(m_at(x, y), FALSE); /* insurance */

	if(!In_endgame(&u.uz)) special = FALSE;

	if ((mtmp = makemon(ptr, x, y, NO_MM_FLAGS)) != 0) {
	    int quan;
	    struct obj *otmp;
		int weapon, secweapon, rweapon, rwammo, armor, shirt, cloak, helm, boots, gloves, shield, tool;
		
		init_mplayer_gear(ptr, special, &weapon, &secweapon, &rweapon, &rwammo, &armor, &shirt, &cloak, &helm, &boots, &gloves, &shield, &tool);
		
		if(special){
			static int sweptyp[] = {
				CRYSKNIFE, MOON_AXE, BATTLE_AXE, HIGH_ELVEN_WARSWORD,
				SABER, CRYSTAL_SWORD, TWO_HANDED_SWORD,
				KATANA, DWARVISH_MATTOCK, RUNESWORD
			};
			weapon = rn2(2) ? sweptyp[rn2(SIZE(sweptyp))] : weapon ? weapon : LONG_SWORD;
			//Why turn off ranged attacks?
			// rweapon = STRANGE_OBJECT;
			// rwammo = STRANGE_OBJECT;
			armor = rnd_class(GRAY_DRAGON_SCALE_MAIL, YELLOW_DRAGON_SCALE_MAIL);
			cloak = !rn2(8) ? cloak :
					rnd_class(OILSKIN_CLOAK, CLOAK_OF_DISPLACEMENT);
			helm = !rn2(8) ? helm :
					rnd_class(HELMET, HELM_OF_TELEPATHY);
			shield = !rn2(2) ? rnd_class(GRAY_DRAGON_SCALE_SHIELD, YELLOW_DRAGON_SCALE_SHIELD) 
							 : (!rn2(8) ? shield : rnd_class(ELVEN_SHIELD, SHIELD_OF_REFLECTION));
		}

	    mtmp->m_lev = (special ? rn1(16,15) : rnd(16));
	    mtmp->mhp = mtmp->mhpmax = d((int)mtmp->m_lev,10) +
					(special ? (30 + rnd(30)) : 30);
	    if(special) {
	        get_mplname(mtmp, nam);
	        mtmp = christen_monst(mtmp, nam);
			/* that's why they are "stuck" in the endgame :-) */
			(void)mongets(mtmp, FAKE_AMULET_OF_YENDOR);
	    }
	    mtmp->mpeaceful = 0;
	    set_malign(mtmp); /* peaceful may have changed again */

	    if (weapon != STRANGE_OBJECT) {
			otmp = mksobj(weapon, NO_MKOBJ_FLAGS);
			otmp->spe = (special ? rn1(5,4) : rn2(4));
			if(otmp->otyp == RAKUYO && special)
				otmp->spe = 10;
			if (!rn2(3)) otmp->oerodeproof = 1;
			else if (!rn2(2)) otmp->greased = 1;
			if (special){
				if(rn2(2)) otmp = mk_artifact(otmp, A_NONE);
				
				if(!otmp->oartifact){//mk_artifact can fail, esp for odd base items
					if(rn2(2)) otmp = mk_special(otmp);
					else if(rn2(4)) otmp = mk_minor_special(otmp);
				}
			}
			/* mplayers knew better than to overenchant Magicbane */
			if (otmp->oartifact == ART_MAGICBANE)
				otmp->spe = rnd(4);
			(void) mpickobj(mtmp, otmp);
	    }

	    if (secweapon != STRANGE_OBJECT) {
			otmp = mksobj(secweapon, NO_MKOBJ_FLAGS);
			otmp->spe = (special ? rn1(5,4) : rn2(4));
			if(otmp->otyp == RAKUYO && special)
				otmp->spe = 10;
			if (!rn2(3)) otmp->oerodeproof = 1;
			else if (!rn2(2)) otmp->greased = 1;
			if (special){
				if(rn2(2)) otmp = mk_artifact(otmp, A_NONE);
				
				if(!otmp->oartifact){//mk_artifact can fail, esp for odd base items
					if(rn2(2)) otmp = mk_special(otmp);
					else if(rn2(4)) otmp = mk_minor_special(otmp);
				}
			}
			/* mplayers knew better than to overenchant Magicbane */
			if (otmp->oartifact == ART_MAGICBANE)
				otmp->spe = rnd(4);
			(void) mpickobj(mtmp, otmp);
	    }

	    if (rweapon != STRANGE_OBJECT) {
			otmp = mksobj(rweapon, NO_MKOBJ_FLAGS);
			otmp->spe = (special ? rn1(5,4) : rn2(4));
			if (!rn2(3)) otmp->oerodeproof = 1;
			else if (!rn2(2)) otmp->greased = 1;
			if (special && rn2(2))
				otmp = mk_artifact(otmp, A_NONE);
			(void) mpickobj(mtmp, otmp);
	    }

	    if (rwammo != STRANGE_OBJECT) {
			otmp = mksobj(rwammo, NO_MKOBJ_FLAGS);
			otmp->spe = (special ? rn1(5,4) : rn2(4));
			if (!rn2(3)) otmp->oerodeproof = 1;
			otmp->quan += special ? 20 : 10;
			(void) mpickobj(mtmp, otmp);
	    }

		if (tool != STRANGE_OBJECT) {
			otmp = mksobj(tool, NO_MKOBJ_FLAGS);
			(void) mpickobj(mtmp, otmp);
		}

	    if(special) {
			if (!rn2(10))
				(void) mongets(mtmp, rn2(3) ? LUCKSTONE : LOADSTONE);
			mk_mplayer_armor(mtmp, armor);
			mk_mplayer_armor(mtmp, shirt);
			mk_mplayer_armor(mtmp, cloak);
			mk_mplayer_armor(mtmp, helm);
			mk_mplayer_armor(mtmp, boots);
			mk_mplayer_armor(mtmp, gloves);
			mk_mplayer_armor(mtmp, shield);
			
			m_dowear(mtmp, TRUE);
			init_mon_wield_item(mtmp);

			quan = rn2(3) ? rn2(3) : rn2(16);
			while(quan--)
				(void)mongets(mtmp, rnd_class(DILITHIUM_CRYSTAL, JADE));
			/* To get the gold "right" would mean a player can double his */
			/* gold supply by killing one mplayer.  Not good. */
#ifndef GOLDOBJ
			mtmp->mgold = rn2(1000);
			u.spawnedGold += mtmp->mgold;
#else
			mkmonmoney(mtmp, rn2(1000));
#endif
			quan = rn2(10);
			while(quan--)
				(void) mpickobj(mtmp, mkobj(RANDOM_CLASS, FALSE));
	    } else {
			(void) mongets(mtmp, armor);
			(void) mongets(mtmp, shirt);
			(void) mongets(mtmp, cloak);
			(void) mongets(mtmp, helm);
			(void) mongets(mtmp, helm);
			(void) mongets(mtmp, boots);
			(void) mongets(mtmp, gloves);
			(void) mongets(mtmp, shield);
			
			m_dowear(mtmp, TRUE);
			init_mon_wield_item(mtmp);
		}
	    quan = rnd(3);
	    while(quan--)
			(void)mongets(mtmp, rnd_offensive_item(mtmp));
	    quan = rnd(3);
	    while(quan--)
			(void)mongets(mtmp, rnd_defensive_item(mtmp));
	    quan = rnd(3);
	    while(quan--)
			(void)mongets(mtmp, rnd_misc_item(mtmp));
	}

	return(mtmp);
}

/* create the indicated number (num) of monster-players,
 * randomly chosen, and in randomly chosen (free) locations
 * on the level.  If "special", the size of num should not
 * be bigger than the number of _non-repeated_ names in the
 * developers array, otherwise a bunch of Adams and Eves will
 * fill up the overflow.
 */
void
create_mplayers(num, special)
register int num;
boolean special;
{
	int pm, x, y;
	struct monst fakemon = {0};

	while(num) {
		int tryct = 0;

		/* roll for character class */
		pm = PM_ARCHEOLOGIST + rn2(PM_WIZARD - PM_ARCHEOLOGIST + 1);
		fakemon.data = &mons[pm];

		/* roll for an available location */
		do {
		    x = rn1(COLNO-4, 2);
		    y = rnd(ROWNO-2);
		} while(!goodpos(x, y, &fakemon, 0) && tryct++ <= 50);

		/* if pos not found in 50 tries, don't bother to continue */
		if(tryct > 50) return;

		(void) mk_mplayer(&mons[pm], (xchar)x, (xchar)y, special);
		num--;
	}
}

void
mplayer_talk(mtmp)
register struct monst *mtmp;
{
	static const char *same_class_msg[3] = {
		"I can't win, and neither will you!",
		"You don't deserve to win!",
		"Mine should be the honor, not yours!",
	},		  *other_class_msg[3] = {
		"The low-life wants to talk, eh?",
		"Fight, scum!",
		"Here is what I have to say!",
	};

	if(mtmp->mpeaceful) return; /* will drop to humanoid talk */

	pline("Talk? -- %s",
		(mtmp->mtyp == urole.malenum ||
		mtmp->mtyp == urole.femalenum) ?
		same_class_msg[rn2(3)] : other_class_msg[rn2(3)]);
}

/*mplayer.c*/
