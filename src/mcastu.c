/*	SCCS Id: @(#)mcastu.c	3.4	2003/01/08	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "xhity.h"

extern void you_aggravate(struct monst *);

STATIC_DCL int FDECL(choose_magic_spell, (int,int,boolean));
STATIC_DCL int FDECL(choose_clerical_spell, (int,int,boolean));
STATIC_DCL int FDECL(choose_psionic_spell, (int,int,boolean));
STATIC_DCL int FDECL(elemspell, (struct monst *, struct monst *, struct attack *, int, int));
STATIC_DCL boolean FDECL(is_undirected_spell,(int));
STATIC_DCL boolean FDECL(is_directed_attack_spell,(int));
STATIC_DCL boolean FDECL(is_aoe_attack_spell	 ,(int));
STATIC_DCL boolean FDECL(is_buff_spell			 ,(int));
STATIC_DCL boolean FDECL(is_summon_spell		 ,(int));
STATIC_DCL boolean FDECL(is_debuff_spell		 ,(int));
STATIC_DCL boolean FDECL(spell_would_be_useless, (struct monst *, struct monst *, int, int, int));

#ifdef OVL0

/* feedback when frustrated creature couldn't cast a spell */
void
cursetxt(magr, mdef, undirected)
struct monst * magr;
struct monst * mdef;
boolean undirected;
{
	boolean youagr = (magr == &youmonst);
	boolean youdef = (mdef == &youmonst);

	if (magr->mtyp == PM_HOUND_OF_TINDALOS)
		return;

	if (youagr) {
		if (undirected) {
			You("point all around and curse");
		}
		else if (mdef) {
			You("point at %s and curse.",
				mon_nam(mdef));
		}
		else {
			/* huh? */
			You_feel("vexed.");
		}
	}
	else if (canseemon(magr) && couldsee(magr->mx, magr->my)) {
		const char *point_msg;  /* spellcasting monsters are impolite */

		if (undirected)
			point_msg = "all around, then curses";
		else if (youdef) {
			if ((Invis && !mon_resistance(magr, SEE_INVIS) &&
				(magr->mux != u.ux || magr->muy != u.uy)) ||
				(youmonst.m_ap_type == M_AP_OBJECT &&
				youmonst.mappearance == STRANGE_OBJECT) ||
				u.uundetected)
				point_msg = "and curses in your general direction";
			else if (Displaced && (magr->mux != u.ux || magr->muy != u.uy))
				point_msg = "and curses at your displaced image";
			else
				point_msg = "at you, then curses";
		} else {
			/* extremely bland mvm message */
			point_msg = "and curses";
		}
		pline("%s points %s.", Monnam(magr), point_msg);
	}
	else if ((!(moves % 4) || !rn2(4))) {
		if (flags.soundok) Norep("You hear a mumbled curse.");
	}
}

#endif /* OVL0 */
#ifdef OVLB

/* default spell selection for mages */
STATIC_OVL int
choose_magic_spell(spellval,mid,hostile)
int spellval;
int mid;
boolean hostile;
{
	/* Alternative spell lists: since the alternative lists contain spells that aren't
		yet implemented for m vs m combat, non-hostile monsters always use the vanilla 
		list. Alternate list selection is based on the monster's ID number, which is
		annotated as staying constant.
	*/
	if(!hostile || mid % 10 < 5){
    switch (spellval % 24) {
    case 23:
    case 22:
    return TURN_TO_STONE;
    case 21:
    case 20:
	return DEATH_TOUCH;
    case 19:
    case 18:
	return CLONE_WIZ;
    case 17:
    case 16:
    case 15:
	return SUMMON_MONS;
    case 14:
    case 13:
	return AGGRAVATION;
    case 12:
    case 11:
    case 10:
	return CURSE_ITEMS;
    case 9:
    case 8:
	return DESTRY_ARMR;
    case 7:
    case 6:
	return WEAKEN_YOU;
    case 5:
    case 4:
	return DISAPPEAR;
    case 3:
	return STUN_YOU;
    case 2:
	return HASTE_SELF;
    case 1:
	return CURE_SELF;
    case 0:
    default:
	return PSI_BOLT;
    }
	}else if(mid % 10 < 7){
     switch (spellval % 24) {
   case 23:
    case 22:
    return DRAIN_LIFE;
    case 21:
    case 20:
	return DRAIN_ENERGY;
    case 19:
    case 18:
	return CLONE_WIZ;
    case 17:
    case 16:
    case 15:
	return WEAKEN_STATS;
    case 14:
    case 13:
	return CONFUSE_YOU;
    case 12:
    case 11:
    case 10:
	return DRAIN_LIFE;
    case 9:
    case 8:
	return DESTRY_WEPN;
    case 7:
    case 6:
	return WEAKEN_YOU;
    case 5:
    case 4:
	return DARKNESS;
    case 3:
	return STUN_YOU;
    case 2:
	return MAKE_WEB;
    case 1:
	return CURE_SELF;
    case 0:
    default:
	return PSI_BOLT;
	}
	}else if(mid % 10 < 9){
    switch (spellval % 24) {
    case 23:
    case 22:
    return CURE_SELF;
    case 21:
    case 20:
	return ARROW_RAIN;
    case 19:
    case 18:
	return CLONE_WIZ;
    case 17:
    case 16:
    case 15:
	return SUMMON_MONS;
    case 14:
    case 13:
	return SUMMON_SPHERE;
    case 12:
    case 11:
    case 10:
	return DROP_BOULDER;
    case 9:
    case 8:
	return DESTRY_ARMR;
    case 7:
    case 6:
	return EVIL_EYE;
    case 5:
    case 4:
	return MAKE_VISIBLE;
    case 3:
	return SUMMON_SPHERE;
    case 2:
	return HASTE_SELF;
    case 1:
	return CURE_SELF;
    case 0:
    default:
	return SUMMON_SPHERE;
	}
	}else{
    switch (spellval % 24) {
    case 23:
    case 22:
    case 21:
    case 20:
    case 19:
	return DRAIN_LIFE;
    case 18:
	return CLONE_WIZ;
    case 17:
    case 16:
    case 15:
	return SUMMON_MONS;
    case 14:
    case 13:
	return AGGRAVATION;
    case 12:
    case 11:
    case 10:
	return CURSE_ITEMS;
    case 9:
	return DESTRY_WEPN;
    case 8:
	return DESTRY_ARMR;
    case 7:
    case 6:
	return EVIL_EYE;
    case 5:
	return MAKE_VISIBLE;
    case 4:
	return DISAPPEAR;
    case 3:
	return STUN_YOU;
    case 2:
	return HASTE_SELF;
    case 1:
	return CONFUSE_YOU;
    case 0:
    default:
	return PSI_BOLT;
	}
	}
}

/* default spell selection for priests/monks */
STATIC_OVL int
choose_clerical_spell(spellnum,mid,hostile)
int spellnum;
int mid;
boolean hostile;
{
	/* Alternative spell lists: since the alternative lists contain spells that aren't
		yet implemented for m vs m combat, non-hostile monsters always use the vanilla 
		list. Alternate list slection is based on the monster's ID number, which is
		annotated as staying constant. Priests are divided up into constructive and
		destructive casters (constructives favor heal self, destructives favor 
		inflict wounds). Their spell list is divided into blocks. The order that
		they recieve spells from each block is radomized based on their monster
		ID.
	*/
	if(!hostile){
     switch (spellnum % 18) {
     case 17:
       return PUNISH;
     case 16:
       return SUMMON_ANGEL;
     case 14:
       return PLAGUE;
    case 13:
       return ACID_RAIN;
    case 12:
	return GEYSER;
    case 11:
	return FIRE_PILLAR;
    case 9:
	return LIGHTNING;
    case 8:
       return DRAIN_LIFE;
    case 7:
       return CURSE_ITEMS;
    case 6:
       return INSECTS;
    case 4:
	return BLIND_YOU;
    case 3:
	return PARALYZE;
    case 2:
	return CONFUSE_YOU;
    case 1:
	return OPEN_WOUNDS;
    case 0:
    default:/*5,10,15,18+*/
	return CURE_SELF;
    }
	}else{
		 spellnum = spellnum % 18;
		//case "17"
		if(spellnum == ((mid/100+3)%4)+14) return PUNISH;
		//case "16"
		//case "15"
		//Cure/Inflict
		if(spellnum == ((mid/100+2)%4)+14) return (mid % 2) ? SUMMON_ANGEL : SUMMON_DEVIL;
		//case "14"
		if(spellnum == ((mid/100+0)%4)+14) return PLAGUE;
		//case "13"
		if(spellnum == ((mid+4)%5)+9) return EARTHQUAKE;
		//case "12"
		if(spellnum == ((mid+3)%5)+9) return ( (mid/11) % 2) ? GEYSER : ACID_RAIN;
		//case "11"
		if(spellnum == ((mid+2)%5)+9) return ( (mid/13) % 2) ? FIRE_PILLAR : ICE_STORM;
		//case "10"
		//Cure/Inflict
		//case "9"
		if(spellnum == ((mid+0)%5)+9) return LIGHTNING;
		//case "8"
		if(spellnum == ((mid/10+3)%4)+5) return DRAIN_LIFE;
		//case "7"
		if(spellnum == ((mid/10+2)%4)+5) return ( (mid/3) % 2) ? CURSE_ITEMS : EVIL_EYE;
		//case "6"
		if(spellnum == ((mid/10+1)%4)+5) return INSECTS;
		//case "5"
		//Cure/Inflict
		//case "4"
		if(spellnum == ((mid+2)%3)+2) return BLIND_YOU;
		//case "3"
		if(spellnum == ((mid+1)%3)+2) return PARALYZE;
		//case "2"
		if(spellnum == ((mid+0)%3)+2) return CONFUSE_YOU;
		//case "1"
		if(spellnum == ((mid+1)%2)+0) return ( (mid+1) % 2) ? CURE_SELF : OPEN_WOUNDS;
		//case "0", "5", "10", "15", "18+"
		return (mid % 2) ? CURE_SELF : OPEN_WOUNDS;
	}
}

/* default spell selection for psychic-flavored casters */
STATIC_OVL int
choose_psionic_spell(spellnum,mid,hostile)
int spellnum;
int mid;
boolean hostile;
{
	if(!hostile)
		return PSI_BOLT;
     switch (spellnum % 18) {
     case 17:
       return MON_WARP;
     case 16:
     case 14:
	return PSI_BOLT;
    case 13:
    case 12:
	return VULNERABILITY;
    case 11:
    case 9:
	return NIGHTMARE;
    case 8:
    case 7:
	return PARALYZE;
    case 6:
    case 4:
	return STUN_YOU;
    case 3:
    case 2:
	return CONFUSE_YOU;
    case 1:
    case 0:
    default:/*5,10,15,18+*/
	return PSI_BOLT;
    }
}

/* ...but first, check for monster-specific spells */
STATIC_OVL int
choose_magic_special(mtmp, type)
struct monst *mtmp;
unsigned int type;
{
	//50% favored spells
    if (rn2(2)) {
       switch(monsndx(mtmp->data)) {
       case PM_WIZARD_OF_YENDOR:
           return (rn2(4) ? rnd(STRANGLE) :
                 (!rn2(3) ? STRANGLE : !rn2(2) ? CLONE_WIZ : HASTE_SELF));

       case PM_ORCUS:
       case PM_NALZOK:
           if (rn2(2)) return RAISE_DEAD;
		break;
       case PM_DISPATER:
           if (rn2(2)) return (rn2(2) ? TURN_TO_STONE : CURSE_ITEMS);
		break;
       case PM_DEMOGORGON:
           return (!rn2(3) ? HASTE_SELF : rn2(2) ? FILTH : WEAKEN_YOU);
		   
       case PM_LAMASHTU:
			// pline("favored");
			switch(rnd(8)){
				case 1:
					return SUMMON_ANGEL;
				break;
				case 2:
					return SUMMON_DEVIL;
				break;
				case 3:
					return SUMMON_ALIEN;
				break;
				case 4:
					return NIGHTMARE;
				break;
				case 5:
					return FILTH;
				break;
				case 6:
					return CURSE_ITEMS;
				break;
				case 7:
					return DEATH_TOUCH;
				break;
				case 8:
					return EVIL_EYE;
				break;
			}
		break;

       case PM_APPRENTICE:
          if (rn2(3)) return SUMMON_SPHERE;
       /* fallthrough */
       case PM_NEFERET_THE_GREEN:
           return ARROW_RAIN;

       case PM_DARK_ONE:
           return (!rn2(4) ? TURN_TO_STONE : !rn2(3) ? RAISE_DEAD :
                    rn2(2) ? DARKNESS : MAKE_WEB);

       case PM_THOTH_AMON:
           if (!rn2(3)) return NIGHTMARE;
       /* fallthrough */
       case PM_CHROMATIC_DRAGON:
           return (rn2(2) ? DESTRY_WEPN : EARTHQUAKE);

       case PM_PLATINUM_DRAGON:
           return (rn2(2) ? LIGHTNING : FIRE_PILLAR);
			
       case PM_IXOTH:
			return FIRE_PILLAR;
       case PM_NIMUNE:
           if(!rn2(3)) return NIGHTMARE;
		   else if(rn2(2)) return MASS_CURE_CLOSE;
		   else return SLEEP;
	   break;
       case PM_MOVANIC_DEVA:
		return MASS_CURE_FAR;
	   break;
       case PM_GRAHA_DEVA:
		switch(rn2(15)){
			case 0:
				return OPEN_WOUNDS;
			break;
				return LIGHTNING;
			case 1:
				return FIRE_PILLAR;
			case 2:
				return GEYSER;
			case 3:
				return ACID_RAIN;
			case 4:
				return ICE_STORM;
			case 5:
				return MASS_CURE_CLOSE;
			case 6:
				return MASS_CURE_FAR;
			break;
				return MAKE_VISIBLE;
			case 7:
				return BLIND_YOU;
			case 8:
				return CURSE_ITEMS;
			case 9:
				return INSECTS;
			case 10:
				return SUMMON_ANGEL;
			case 11:
				return AGGRAVATION;
			case 12:
				return PUNISH;
			case 13:
				return EARTHQUAKE;
			case 14:
				return VULNERABILITY;
		}
	   break;
       case PM_SURYA_DEVA:
           return (rn2(2) ? MASS_CURE_CLOSE : FIRE_PILLAR);
	
       case PM_GRAND_MASTER:
       case PM_MASTER_KAEN:
          return (rn2(2) ? WEAKEN_YOU : EARTHQUAKE);

       case PM_MINION_OF_HUHETOTL:
           return (rn2(2) ? CURSE_ITEMS : (rn2(2) ? DESTRY_WEPN : DROP_BOULDER));

       case PM_TITAN:
           return (rn2(2) ? DROP_BOULDER : LIGHTNING_BOLT);
       case PM_THRONE_ARCHON:
           return (rn2(2) ? SUMMON_ANGEL : LIGHTNING_BOLT);
       case PM_KI_RIN:
           return FIRE_PILLAR;

       case PM_ARCH_LICH:
           if (!rn2(6)) return TURN_TO_STONE;
       /* fallthrough */
#if 0
       case PM_VAMPIRE_MAGE:
#endif
       case PM_MASTER_LICH:
           if (!rn2(5)) return RAISE_DEAD;
       /* fallthrough */
       case PM_DEMILICH:
           if (!rn2(4)) return DRAIN_LIFE;
       /* fallthrough */
       case PM_LICH:
           if (!rn2(3)) return CURSE_ITEMS;
       /* fallthrough */
       // case PM_NALFESHNEE:
           if (rn2(2)) return (rn2(2) ? DESTRY_ARMR : DESTRY_WEPN);
       /* fallthrough */
       case PM_BARROW_WIGHT:
           return (!rn2(3) ? DARKNESS : (rn2(2) ? MAKE_WEB : SLEEP));

       case PM_PRIEST_OF_GHAUNADAUR:
           if (rn2(2)) return FIRE_PILLAR;
		   else if(rn2(2)) return MON_FIRAGA;
		break; /*Go on to regular spell list*/

       case PM_GNOMISH_WIZARD:
           if (rn2(2)) return SUMMON_SPHERE;
		break;
       case PM_HIGH_SHAMAN:
			switch (rnd(3)) {
				case 3:
				return AGGRAVATION;
				break;
				case 2:
				return MON_FIRA;
				break;
				case 1:
				return SUMMON_DEVIL;
				break;
			}
       }
    }//50% favored spells
	
	//100% favored spells
	switch(monsndx(mtmp->data)) {
	case PM_HOUND_OF_TINDALOS:
		return OPEN_WOUNDS;
	break;
	case PM_DWARF_CLERIC:
	case PM_DWARF_QUEEN:
		switch (rnd(4)) {
			case 4:
			return MON_PROTECTION;
			break;
			case 3:
			return MASS_CURE_CLOSE;
			break;
			case 2:
			return MASS_CURE_FAR;
			break;
			case 1:
			return AGGRAVATION;
			break;
		}
	break;
	case PM_WARRIOR_OF_SUNLIGHT:
		switch (rn2(mtmp->m_lev-10)) {
			default:/* 15 -> 19*/
				return LIGHTNING;
			case 14:
			case 13:
			case 12:
			case 11:
			case 10:
				return MON_PROTECTION;
			case 9:
			case 8:
			case 7:
			case 6:
			case 5:
				return MASS_CURE_CLOSE;
			case 4:
			case 3:
			case 2:
			case 1:
			case 0:
				return CURE_SELF;
		}
	break;
	case PM_UNDEAD_MAIDEN:
		switch (rn2(mtmp->m_lev/3)) {
			default:/* 15 -> 19*/
				return MASS_CURE_CLOSE;
			case 3:
			case 2:
			case 1:
			case 0:
				return CURE_SELF;
		}
	break;
	case PM_PISACA:
	case PM_KNIGHT_OF_THE_PRINCESS_S_GUARD:
		switch (rn2(mtmp->m_lev-10)) {
			default:/* 15 -> 19*/
				return MASS_CURE_FAR;
			case 14:
			case 13:
			case 12:
			case 11:
			case 10:
				return MON_PROTECTION;
			case 9:
			case 8:
			case 7:
			case 6:
			case 5:
				return MASS_CURE_CLOSE;
			case 4:
			case 3:
			case 2:
			case 1:
			case 0:
				return CURE_SELF;
		}
	break;
	case PM_BLUE_SENTINEL:
		switch (rn2(mtmp->m_lev/3)) {
			default:/* 6 up*/
				return MON_PROTECTION;
			case 2:
			case 1:
			case 0:
				return CURE_SELF;
		}
	break;
	case PM_DARKMOON_KNIGHT:
		switch (rn2(mtmp->m_lev-10)) {
			default:/* 15 -> 19*/
				// return MAGIC_BLADE;
			case 14:
			case 13:
			case 12:
			case 11:
			case 10:
				return PARALYZE;
			case 9:
			case 8:
			case 7:
			case 6:
			case 5:
				return PUNISH;
			case 4:
			case 3:
			case 2:
			case 1:
			case 0:
				return CURE_SELF;
		}
	break;
	case PM_PARDONER:
		switch (rn2(mtmp->m_lev-10)) {
			default:/* 15 -> 19*/
				return VULNERABILITY;
			case 14:
			case 13:
			case 12:
				return CURSE_ITEMS;
			case 11:
			case 10:
				return EVIL_EYE;
			case 9:
			case 8:
				return AGGRAVATION;
			case 7:
			case 6:
				return PUNISH;
			case 5:
			case 4:
			case 3:
			case 2:
			case 1:
			case 0:
				return CURE_SELF;
		}
	break;
	case PM_OCCULTIST:
		switch (rn2((mtmp->m_lev-10)/3)) {
			default:/* 15 -> 19*/
				return TURN_TO_STONE;
			case 5:
			case 4:
			case 3:
			case 2:
				return MON_PROTECTION;
			case 1:
			case 0:
				return CURE_SELF;
		}
	break;
	case PM_HALF_STONE_DRAGON:
		switch (rn2(mtmp->m_lev)) {
			default:/* 10 -> 29*/
				return LIGHTNING;
			case 9:
			case 8:
			case 7:
			case 6:
			case 5:
				return MASS_CURE_CLOSE;
			case 4:
			case 3:
			case 2:
			case 1:
			case 0:
				return CURE_SELF;
		}
	break;
	case PM_ELF_LADY:
	case PM_ELVENQUEEN:
		switch (rnd(6)) {
			case 6:
			return CURE_SELF;
			break;
			case 5:
			return MASS_CURE_FAR;
			break;
			case 4:
			return SLEEP;
			break;
			case 3:
			return BLIND_YOU;
			break;
			case 2:
			return CONFUSE_YOU;
			break;
			case 1:
			return DISAPPEAR;
			break;
		}
	break;
	case PM_ALIDER:
		switch (rnd(8)) {
			case 8:
			return FIRE_PILLAR;
			break;
			case 7:
			return ICE_STORM;
			break;
			case 6:
			return CURE_SELF;
			break;
			case 5:
			return MASS_CURE_FAR;
			break;
			case 4:
			return SLEEP;
			break;
			case 3:
			return OPEN_WOUNDS;
			break;
			case 2:
			return LIGHTNING;
			break;
			case 1:
			return DISINT_RAY;
			break;
		}
	break;
	case PM_PARASITIZED_EMBRACED_ALIDER:
		switch (rnd(6)) {
			case 6:
			return FIRE_PILLAR;
			break;
			case 5:
			return ICE_STORM;
			break;
			case 4:
			return CURE_SELF;
			break;
			case 3:
			return OPEN_WOUNDS;
			break;
			case 2:
			return LIGHTNING;
			break;
			case 1:
			return DISINT_RAY;
			break;
		}
	break;
	case PM_FORD_GUARDIAN:
		switch(rn2(5)){
			case 0:
				return CURE_SELF;
			break;
			case 1:
				return RECOVER;
			break;
			case 2:
				return OPEN_WOUNDS;
			break;
			case 3:
				switch(rn2(2)){
					case 0:
						return MON_PROTECTION;
					break;
					case 1:
						return MASS_CURE_FAR;
					break;
				}
			break;
			case 4:
				switch(rn2(5)){
					case 0:
						return LIGHTNING;
					break;
					case 1:
						return ACID_RAIN;
					break;
					case 2:
						return FIRE_PILLAR;
					break;
					case 3:
						return GEYSER;
					break;
					case 4:
						return ICE_STORM;
					break;
				}
			break;
		}
	break;
	case PM_MINOTAUR_PRIESTESS:
		switch (d(1,5)+8) {
			case 13:
			return GEYSER;
			break;
			case 12:
			return FIRE_PILLAR;
			break;
			case 11:
			return LIGHTNING;
			break;
			case 10:
			case 9:
			return CURSE_ITEMS;
			break;
			default:
			return OPEN_WOUNDS;
			break;
		}
	break;
	case PM_GNOLL_MATRIARCH:
		switch (d(1,10)-4) {
			case 6:
				return BLIND_YOU;
			break;
			case 5:
			case 4:
				return PARALYZE;
			break;
			case 3:
			case 2:
				return CONFUSE_YOU;
			break;
			case 1:
				return MASS_CURE_FAR;
			break;
			case 0:
			default:
				return OPEN_WOUNDS;
			break;
		}
	break;
       case PM_MAHADEVA:
		switch(rn2(10)){
			case 0:
				return MASS_CURE_CLOSE;
			break;
			case 1:
			case 2:
			case 3:
				return ARROW_RAIN;
			break;
			case 4:
			case 5:
			case 6:
				return ICE_STORM;
			break;
			case 7:
			case 8:
			case 9:
				return GEYSER;
			break;
		}
	   break;
	case PM_GAE_ELADRIN:
		switch (rnd(4)) {
			case 1:
				return ICE_STORM;
			break;
			case 2:
				return LIGHTNING;
			break;
			case 3:
				return FIRE_PILLAR;
			break;
			case 4:
				return STERILITY_CURSE;
			break;
		}
	break;
	case PM_WARDEN_TREE:
		switch (rnd(4)) {
			case 1:
				return ICE_STORM;
			break;
			case 2:
				return GEYSER;
			break;
			case 3:
				return MASS_CURE_CLOSE;
			break;
			case 4:
				return STERILITY_CURSE;
			break;
		}
	break;
	case PM_KUKER:
		switch (rnd(6)) {
			case 6:
			return PUNISH;
			break;
			case 5:
			return MON_PROTECTION;
			break;
			case 4:
			return CURSE_ITEMS;
			break;
			case 3:
			return EVIL_EYE;
			break;
			case 2:
			return MAKE_VISIBLE;
			break;
			case 1:
			return CONFUSE_YOU;
			break;
		}
	break;
	case PM_AMM_KAMEREL:
	case PM_HUDOR_KAMEREL:
	case PM_ARA_KAMEREL:
		return OPEN_WOUNDS;
	break;
	case PM_SHARAB_KAMEREL:
		return PSI_BOLT;
	break;
	case PM_PLUMACH_RILMANI:
		return SOLID_FOG;
	break;
	case PM_FERRUMACH_RILMANI:
		if(rn2(4)) return HAIL_FLURY;
		return SOLID_FOG;
	break;
	case PM_STANNUMACH_RILMANI:
		switch(rn2(6)){
			case 0: return ICE_STORM;
			case 1: return SOLID_FOG;
			case 2: return DISAPPEAR;
			case 3: return MAKE_VISIBLE;
			case 4: return MASS_CURE_FAR;
			case 5: return MON_PROTECTION;
		}
	break;
	case PM_CUPRILACH_RILMANI:
		switch(rn2(6)){
			case 0: return DRAIN_LIFE;
			case 1: return ACID_BLAST;
			case 2: return SOLID_FOG;
			case 3: return DISAPPEAR;
			case 4: return MON_POISON_GAS;
			case 5: return MAKE_VISIBLE;
		}
	break;
	case PM_ARGENACH_RILMANI:
		if(rn2(4)) return SILVER_RAYS;
		switch(rn2(4)){
			case 0: return ICE_STORM;
			case 1: return SOLID_FOG;
			case 2: return DISAPPEAR;
			case 3: return MAKE_VISIBLE;
		}
	break;
	case PM_CENTER_OF_ALL:
	case PM_AURUMACH_RILMANI:
		if(rn2(4)) return GOLDEN_WAVE;
		switch(rn2(7)){
			case 0: return ICE_STORM;
			case 1: return ACID_RAIN;
			case 2: return SOLID_FOG;
			case 3: return DISAPPEAR;
			case 4: return MON_POISON_GAS;
			case 5: return MAKE_VISIBLE;
			case 6: return PRISMATIC_SPRAY;
		}
	break;
	case PM_BAELNORN:
		switch(rnd(6)){
			case 1: return ACID_RAIN;
			case 2: return CURSE_ITEMS;
			case 3: return MASS_CURE_CLOSE;
			case 4: return SLEEP;
			case 5: return DARKNESS;
			case 6: return CONFUSE_YOU;
		}
	break;
	case PM_UVUUDAUM:
	case PM_MASKED_QUEEN:
		switch(rn2(8)){
			case 0: return PSI_BOLT;
			case 1: return MON_WARP;
			case 2: return STUN_YOU;
			case 3: return PARALYZE;
			case 4: return MON_TIME_STOP;
			case 5: return TIME_DUPLICATE;
			case 6: return NAIL_TO_THE_SKY;
			case 7: return PRISMATIC_SPRAY;
		}
	break;
	case PM_GREAT_HIGH_SHAMAN_OF_KURTULMAK:
		return SUMMON_DEVIL; 
	case PM_LICH__THE_FIEND_OF_EARTH:
		if(mtmp->mvar_spList_1 > 3) mtmp->mvar_spList_1 = 0;
		switch(mtmp->mvar_spList_1++){
			case 0: return MON_FLARE;
			case 1: return PARALYZE;
			case 2: return MON_WARP;
			case 3: return DEATH_TOUCH;
		}
	break;
	case PM_KARY__THE_FIEND_OF_FIRE:
		if(mtmp->mvar_spList_1 > 7) mtmp->mvar_spList_1 = 0;
		switch(mtmp->mvar_spList_1++){
			case 0: return MON_FIRAGA;
			case 1: return BLIND_YOU;
			case 2: return MON_FIRAGA;
			case 3: return BLIND_YOU;
			case 4: return MON_FIRAGA;
			case 5: return STUN_YOU;
			case 6: return MON_FIRAGA;
			case 7: return STUN_YOU;
		}
	break;
	case PM_KRAKEN__THE_FIEND_OF_WATER:
		if(rn2(100)<71) return MON_THUNDARA;
		else return BLIND_YOU;
	break;
	case PM_TIAMAT__THE_FIEND_OF_WIND:
		if(rn2(3)){
			if(mtmp->mvar_spList_1 > 3) mtmp->mvar_spList_1 = 0;
			switch(mtmp->mvar_spList_1++){
				case 0: return PLAGUE;
				case 1: return MON_BLIZZARA;
				case 2: return MON_THUNDARA;
				case 3: return MON_FIRA;
			}
		} else {
			if(mtmp->mvar2 > 3) mtmp->mvar2 = 0;
			switch(mtmp->mvar2++){
				case 0: return LIGHTNING_BOLT;
				case 1: return MON_POISON_GAS;
				case 2: return ICE_STORM;
				case 3: return FIRE_PILLAR;
			}
		}
	break;
	case PM_CHAOS:
		if(rn2(2)){
			if(mtmp->mvar_spList_1 > 7) mtmp->mvar_spList_1 = 0;
			switch(mtmp->mvar_spList_1++){
				case 0: return MON_BLIZZAGA;
				case 1: return WEAKEN_STATS;
				case 2: return MON_THUNDAGA;
				case 3: return CURE_SELF;
				case 4: return HASTE_SELF;
				case 5: return MON_FIRAGA;
				case 6: return ICE_STORM;
				case 7: return MON_FLARE;
			}
		} else {
			if(mtmp->mvar2 > 3) mtmp->mvar2 = 0;
			switch(mtmp->mvar2++){
				case 0: return FIRE_PILLAR;
				case 1: return GEYSER;
				case 2: return MON_POISON_GAS;
				case 3: return EARTHQUAKE;
			}
		}
	break;
	   case PM_ALABASTER_ELF_ELDER:
			switch (rnd(8)) {
				case 8:
				return MASS_CURE_FAR;
				break;
				case 7:
				return AGGRAVATION;
				break;
				case 6:
				return MASS_CURE_CLOSE;
				break;
				case 5:
				return MASS_CURE_FAR;
				break;
				case 4:
				return SLEEP;
				break;
				case 3:
				return BLIND_YOU;
				break;
				case 2:
				return CONFUSE_YOU;
				break;
				case 1:
				return DISAPPEAR;
				break;
			}
	   break;
       case PM_WITCH_S_FAMILIAR:
			return OPEN_WOUNDS;
	   break;
       case PM_APPRENTICE_WITCH:
			switch(rnd(2)){
				case 1: return PSI_BOLT;
				case 2: return EVIL_EYE;
			}
	   break;
       case PM_WITCH:
			switch(rnd(4)){
				case 1: return PSI_BOLT;
				case 2: return EVIL_EYE;
				case 3: return CURSE_ITEMS;
				case 4: return ACID_RAIN;
			}
	   break;
       case PM_COVEN_LEADER:
			switch(rnd(8)){
				case 1: return PSI_BOLT;
				case 2: return EVIL_EYE;
				case 3: return CURSE_ITEMS;
				case 4: return ACID_RAIN;
				case 5: return SUMMON_DEVIL;
				case 6: return SUMMON_ALIEN;
				case 7: return DEATH_TOUCH;
				case 8: return DRAIN_LIFE;
			}
	   break;
       case PM_GOOD_NEIGHBOR:
			switch(rnd(12)){
				case 1: return PSI_BOLT;
				case 2: return EVIL_EYE;
				case 3: return CURSE_ITEMS;
				case 4: return ACID_RAIN;
				case 5: return SUMMON_DEVIL;
				case 6: return SUMMON_ALIEN;
				case 7: return DEATH_TOUCH;
				case 8: return DRAIN_LIFE;
				case 9: return VULNERABILITY;
				case 10: return MASS_CURE_CLOSE;
				case 11: return RECOVER;
				case 12: return MON_WARP;
			}
	   break;
       case PM_HMNYW_PHARAOH:
			switch(rnd(10)){
				case 1: 
				return CURSE_ITEMS;
				case 2: 
				//Summon amphibians?
				return GEYSER;
				case 3: 
				//dust to gnats
				case 4: 
				return INSECTS;
				case 5: 
				return PLAGUE;
				case 6: 
				return OPEN_WOUNDS;
				case 7: 
				return rn2(2) ? ICE_STORM : LIGHTNING;
				case 8: 
				return INSECTS;
				case 9: 
				return DARKNESS;
				case 10: 
				return DEATH_TOUCH;
			}
       case PM_NITOCRIS:
       case PM_GHOUL_QUEEN_NITOCRIS:
			if(type == AD_CLRC){
				switch(rnd(9)){
					case 1: 
					case 2: 
					return OPEN_WOUNDS;
					case 3: 
					case 4: 
					return CURSE_ITEMS;
					case 5: 
					case 6: 
					return EVIL_EYE;
					case 7: 
					return INSECTS;
					case 8: 
					return DARKNESS;
					case 9: 
					return PLAGUE;
				}
			} else {
				switch(rnd(9)){
					case 1: 
					case 2: 
					return GEYSER;
					case 3: 
					case 4: 
					return CURSE_ITEMS;
					case 5: 
					case 6: 
					return FIRE_PILLAR;
					case 7: 
					return INSECTS;
					case 8: 
					return DARKNESS;
					case 9: 
					return PLAGUE;
				}
			}
	   break;
    case PM_BLESSED:
		switch(rnd(8)){
			case 1: return OPEN_WOUNDS;
			case 2: return PSI_BOLT;
			case 3: return ICE_STORM;
			case 4: return ACID_RAIN;
			case 5: return GEYSER;
			case 6: return SUMMON_YOUNG;
			case 7: return MASS_CURE_CLOSE;
			case 8: return LIGHTNING;
		}
	break;
	case PM_SHOGGOTH:
		if(!rn2(20)) return SUMMON_MONS; 
		else return 0;
	case PM_VERIER: 
		if(!rn2(3)) return WEAKEN_YOU;
		else return DESTRY_ARMR;
	case PM_CRONE_LILITH:
		switch(rn2(6)){
			case 0:
			case 1:
			case 2:
				return CURSE_ITEMS;
			break;
			case 3:
				return WEAKEN_STATS;
			break;
			case 4:
				return CURE_SELF;
			break;
			case 5:
				return DEATH_TOUCH;
			break;
		}
	case PM_MILITANT_CLERIC:
		switch(rn2(6)){
			case 0:
			case 1:
				return CURE_SELF;
			break;
			case 2:
				return MASS_CURE_FAR;
			break;
			case 3:
				return MASS_CURE_CLOSE;
			break;
			case 4:
				return RECOVER;
			break;
			case 5:
				return FIRE_PILLAR;
			break;
		}
	case PM_ADVENTURING_WIZARD:
		switch(rn2(4)){
			case 0:
			case 1:
				return SLEEP;
			break;
			case 2:
				return MAGIC_MISSILE;
			break;
			case 3:
				return SUMMON_SPHERE;
			break;
		}
	case PM_NALFESHNEE:
		switch(rn2(5)){
			case 0:
				return OPEN_WOUNDS;
			break;
			case 1:
				return CURSE_ITEMS;
			break;
			case 2:
				return LIGHTNING;
			break;
			case 3:
				return FIRE_PILLAR;
			break;
			case 4:
				return PUNISH;
			break;
		}
	break;
	case PM_ANCIENT_OF_THE_BURNING_WASTES:
		switch(rn2(2)){
			case 0:
				return MON_WARP;
			break;
			case 1:
				return MON_WARP_THROW;
			break;
		}
	break;
	case PM_FAERINAAL:{
		int spelln;
		do spelln = rnd(MON_LASTSPELL);
		while(spelln == CLONE_WIZ);
		return spelln;
	} break;
	case PM_PALE_NIGHT:
		switch(rn2(5)){
			case 0:
				return OPEN_WOUNDS;
			break;
			case 1:
				return WEAKEN_YOU;
			break;
			case 2:
				return PSI_BOLT;
			break;
			case 3:
				return CURSE_ITEMS;
			break;
			case 4:
				return DEATH_TOUCH;
			break;
		}
	break;
	case PM_ASMODEUS:
		switch(rn2(9)){
			case 0:
				return CURE_SELF;
			break;
			case 1:
				return OPEN_WOUNDS;
			break;
			case 2:
				return ACID_RAIN;
			break;
			case 3:
				return LIGHTNING;
			break;
			case 4:
				return FIRE_PILLAR;
			break;
			case 5:
				return GEYSER;
			break;
			case 6:
				return SUMMON_MONS;
			break;
			case 7:
				return PARALYZE;
			break;
			case 8:
				return DEATH_TOUCH;
			break;
		}
	break;
	case PM_ALRUNES:
		switch(rn2(6)){
			case 0:
				return MON_PROTECTION;
			break;
			case 1:
				return MASS_CURE_CLOSE;
			break;
			case 2:
				return MASS_CURE_FAR;
			break;
			case 3:
				return AGGRAVATION;
			break;
			case 4:
				return VULNERABILITY;
			break;
			case 5:
				return EVIL_EYE;
			break;
		}
	break;
	case PM_HATEFUL_WHISPERS:
		switch(rn2(6)){
			case 0:
				return SUMMON_MONS;
			break;
			case 1:
				return CURE_SELF;
			break;
			case 2:
				return SUMMON_DEVIL;
			break;
			case 3:
				return AGGRAVATION;
			break;
			case 4:
				return VULNERABILITY;
			break;
			case 5:
				return EVIL_EYE;
			break;
		}
	break;
	case PM_DOMIEL:
		switch(rn2(7)){
			case 0:
				return MON_PROTECTION;
			break;
			case 1:
				return HASTE_SELF;
			break;
			case 2:
				return MASS_CURE_CLOSE;
			break;
			case 3:
				return MASS_CURE_FAR;
			break;
			case 4:
				return AGGRAVATION;
			break;
			case 5:
				return FIRE_PILLAR;
			break;
			case 6:
				return STUN_YOU;
			break;
		}
	break;
	case PM_ERATHAOL:
		switch(rn2(7)){
			case 0:
				return MON_PROTECTION;
			break;
			case 1:
				return VULNERABILITY;
			break;
			case 2:
				return EVIL_EYE;
			break;
			case 3:
				return BLIND_YOU;
			break;
			case 4:
				return AGGRAVATION;
			break;
			case 5:
				return CONFUSE_YOU;
			break;
			case 6:
				return HASTE_SELF;
			break;
		}
	break;
	case PM_SEALTIEL:
		switch(rn2(7)){
			case 0:
				return MON_PROTECTION;
			break;
			case 1:
				return HASTE_SELF;
			break;
			case 2:
				return MASS_CURE_CLOSE;
			break;
			case 3:
				return MASS_CURE_FAR;
			break;
			case 4:
				return ICE_STORM;
			break;
			case 5:
				return DROP_BOULDER;
			break;
			case 6:
				return TURN_TO_STONE;
			break;
		}
	break;
	case PM_ZAPHKIEL:
		switch(rn2(7)){
			case 0:
				return MON_PROTECTION;
			break;
			case 1:
				return SUMMON_ANGEL;
			break;
			case 2:
				return MASS_CURE_CLOSE;
			break;
			case 3:
				return MASS_CURE_FAR;
			break;
			case 4:
				return AGGRAVATION;
			break;
			case 5:
				return RECOVER;
			break;
			case 6:
				return MASS_HASTE;
			break;
		}
	break;
	}
    if (type == AD_CLRC)
        return choose_clerical_spell(mtmp->m_id == 0 ? (rn2(u.ulevel) * 18 / 30) : rn2(mtmp->m_lev),mtmp->m_id,!(mtmp->mpeaceful));
    else if (type == AD_PSON)
        return choose_psionic_spell(mtmp->m_id == 0 ? (rn2(u.ulevel) * 18 / 30) : rn2(mtmp->m_lev),mtmp->m_id,!(mtmp->mpeaceful));
    return choose_magic_spell(mtmp->m_id == 0 ? (rn2(u.ulevel) * 24 / 30) : rn2(mtmp->m_lev),mtmp->m_id,!(mtmp->mpeaceful));
}


const char * spellname[] =
{
	"dummyspell",
	"PSI_BOLT",
	"OPEN_WOUNDS",
	"MAGIC_MISSILE",
	"DRAIN_LIFE",
	"ARROW_RAIN",
	//5
	"CONE_OF_COLD",
	"LIGHTNING",
	"LIGHTNING_BOLT",
	"FIRE_PILLAR",
	"GEYSER",
	//10
	"ACID_RAIN",
	"ICE_STORM",
	"HAIL_FLURY",
	"SUMMON_MONS",
	"SUMMON_DEVIL",
	//15
	"DEATH_TOUCH",
	"CURE_SELF",
	"MASS_CURE_CLOSE",
	"MASS_CURE_FAR",
	"RECOVER",
	//20
	"MAKE_VISIBLE",
	"HASTE_SELF",
	"STUN_YOU",
	"CONFUSE_YOU",
	"PARALYZE",
	//25
	"BLIND_YOU",
	"SLEEP",
	"DRAIN_ENERGY",
	"WEAKEN_STATS",
	"WEAKEN_YOU",
	//30
	"DESTRY_ARMR",
	"DESTRY_WEPN",
	"EVIL_EYE",
	"CURSE_ITEMS",
	"INSECTS",
	//35
	"RAISE_DEAD",
	"SUMMON_ANGEL",
	"SUMMON_ALIEN",
	"SUMMON_YOUNG",
	"PLAGUE",
	//40
	"PUNISH",
	"AGGRAVATION",
	"DISAPPEAR",
	"DARKNESS",
	"SUMMON_SPHERE",
	//45
	"MAKE_WEB",
	"DROP_BOULDER",
	"EARTHQUAKE",
	"TURN_TO_STONE",
	"NIGHTMARE",
	//50
	"FILTH",
	"CLONE_WIZ",
	"STRANGLE",
	"MON_FIRA",
	"MON_FIRAGA",
	//55
	"MON_BLIZZARA",
	"MON_BLIZZAGA",
	"MON_THUNDARA",
	"MON_THUNDAGA",
	"MON_FLARE",
	//60
	"MON_WARP",
	"MON_POISON_GAS",
	"MON_PROTECTION",
	"SOLID_FOG",
	"ACID_BLAST",
	//65
	"PRISMATIC_SPRAY",
	"SILVER_RAYS",
	"GOLDEN_WAVE",
	"VULNERABILITY",
	"MASS_HASTE",
	//70
	"MON_TIME_STOP",
	"TIME_DUPLICATE",
	"NAIL_TO_THE_SKY",
	"STERILITY_CURSE",
	"DISINT_RAY",
	//75
	"MON_WARP_THROW",
};


/* xcasty()
 * 
 * Magr attempts to cast a monster spell at mdef, who they think is at (tarx, tary)
 * If !mdef or (tarx, tary) is (0,0), magr doesn't have a target and should use an undirected spell.
 * 
 * Returns MM_MISS if the spellcasting failed
 * 
 * Can handle any of uvm, mvm, mvu.
 *
 * Does not consider whether or not magr should be able to cast at mdef (line of sight, range)
 */
int
xcasty(magr, mdef, attk, tarx, tary)
struct monst * magr;
struct monst * mdef;
struct attack * attk;
int tarx;
int tary;
{
	boolean youagr = (magr == &youmonst);
	boolean youdef = (mdef == &youmonst);
	struct permonst * pa = (youagr ? youracedata : magr->data);
	boolean foundem = (mdef && (tarx == x(mdef) && tary == y(mdef)));
	boolean notarget = (!mdef || (!tarx && !tary));
	int result = 0;
	int spellnum = 0;
	int chance = 0;
	char buf[BUFSZ];

	/* things that block monster spells from even being attempted */
	if (cantmove(magr))
		return MM_MISS;
	if (youagr ? Nullmagic : mon_resistance(magr, NULLMAGIC))
		return MM_MISS;

	/* Attempt to find a spell to cast */
	if (mlev(magr) > 0 && (attk->adtyp == AD_SPEL || attk->adtyp == AD_CLRC || attk->adtyp == AD_PSON)) {
		int cnt = 40;	/* 40 attempts at a spell if it has a target, but only 1 attempt if no target */

		do {
			/* get spell */
			spellnum = choose_magic_special(magr, attk->adtyp);
			/* check that the spell selection code did not abort the cast */
			if (!spellnum)
				return 0;
			/* if we have no target... */
			if (notarget) {
				/* !youagr means it's a monster maybe casting something as they wander
				 * so if the selected spell would be useless, move on 
				 * 
				 * a player casting with no target used alt-M, so try to get them a real spell   */
				if (!youagr && spell_would_be_useless(magr, mdef, spellnum, tarx, tary))
					return MM_MISS;
			}
		} while ((--cnt > 0) && (spell_would_be_useless(magr, mdef, spellnum, tarx, tary)));

		if (cnt == 0) {
			if (youagr) {
				You("can't think of anything to cast.");
			}
			return MM_MISS;
		}
	}

	/* things that cause spellcasting to fail loudly */
	if (youagr ? (
		(u.uen < mlev(magr))
		) : (
		(magr->mcan) ||
		(magr->mspec_used && !nospellcooldowns_mon(magr)) ||
		(mlev(magr) == 0) ||
		(youdef && (u.uinvulnerable || (u.spiritPColdowns[PWR_PHASE_STEP] >= moves + 20))) ||
		(needs_familiar(magr))
		)) {
		cursetxt(magr, mdef, is_undirected_spell(spellnum));
		return MM_MISS;
	}

	/* At this point, magr is committing to attempting to cast a spell */

	/* set spell cooldown for monsters */
	if (!youagr && (attk->adtyp == AD_SPEL || attk->adtyp == AD_CLRC) && !nospellcooldowns_mon(magr)) {
		if (magr->mtyp == PM_HEDROW_WARRIOR) magr->mspec_used = d(4, 4);
		else if(magr->mtyp == PM_ALIDER){
			struct monst *mtmp;
			magr->mspec_used = 60;
			for(mtmp = fmon; mtmp; mtmp = mtmp->nmon){
				if(!DEADMONSTER(mtmp) && (mtmp != magr) && (mtmp->mpeaceful == magr->mpeaceful)
					&& (mtmp->mtyp == PM_MYRKALFAR_WARRIOR || mtmp->mtyp == PM_MYRKALFAR_MATRON 
						|| (is_drow(mtmp->data) && mtmp->mtame && magr->mtame))
				) magr->mspec_used /= 2;
			}
			if(magr->mtame)
				magr->mspec_used /= 2;
		}
		else magr->mspec_used = 10 - magr->m_lev;
		if (magr->mspec_used < 2) magr->mspec_used = 2;
	}
	/* cost pw for players */
	if (youagr) {
		u.uen -= mlev(magr);	/* we already checked that u.uen >= mlev(magr) */
		flags.botl = 1;
	}

	/* check for spells that do absolutely nothing when they miss */
	if (spellnum && !notarget && !foundem &&
		!is_buff_spell(spellnum) &&
		!is_summon_spell(spellnum) &&
		!is_aoe_attack_spell(spellnum)) {
		/* message */
		if ((youagr || canspotmon(magr)) && magr->mtyp != PM_HOUND_OF_TINDALOS)	{
			pline("%s cast%s a spell at %s!",
				youagr ? "You" : canspotmon(magr) ? Monnam(magr) : "Something",
				youagr ? "" : "s",
				levl[tarx][tary].typ == WATER
				? "empty water" : "thin air");
		}
		/* monsters figure out you aren't there */
		if (!youagr && youdef) {
			magr->mux = magr->muy = 0;
		}
		return MM_MISS;
	}

	/* interrupt the player if the player is being targeted (comes after filtering out spells that miss and fizzle entirely) */
	if (youdef && canspotmon(magr))
		nomul(0, (const char *)0);

	/* calculate success rate of spell */
	chance = 2;
	if (!youagr && is_alabaster_mummy(magr->data) && magr->mvar_syllable == SYLLABLE_OF_THOUGHT__NAEN)
		chance -= 2;
	if (youagr ? Confusion : magr->mconf)
		chance += 8;
	if (!youagr && is_kamerel(pa)){
		struct obj * mirror;
		for (mirror = magr->minvent; mirror; mirror = mirror->nobj)
		if (mirror->otyp == MIRROR && !mirror->cursed) {
			chance -= 1000;	/* even overcomes the spire's anti-casting effect */
			break;
		}
	}
	/*In some cases, monsters have a 0% success rate*/
	boolean force_fail = FALSE;
	if (u.uz.dnum == neutral_dnum && u.uz.dlevel <= sum_of_all_level.dlevel){
		if (u.uz.dlevel == sum_of_all_level.dlevel) chance -= 1;
		else if (u.uz.dlevel == spire_level.dlevel - 0) force_fail = TRUE;
		else if (u.uz.dlevel == spire_level.dlevel - 1) chance += 10;
		else if (u.uz.dlevel == spire_level.dlevel - 2) chance += 8;
		else if (u.uz.dlevel == spire_level.dlevel - 3) chance += 6;
		else if (u.uz.dlevel == spire_level.dlevel - 4) chance += 4;
		else if (u.uz.dlevel == spire_level.dlevel - 5) chance += 2;
	}

	/*This.... may never be reached :( */
	/* There is code is getattk() that prevents lilitus from attempting to cast vs. invalid targets */
	if(pa->mtyp == PM_LILITU && attk->adtyp == AD_CLRC){
		if(youdef && !Doubt)
			force_fail = TRUE;
		else if(!youdef && !(mdef && mdef->mdoubt))
			force_fail = TRUE;
	}

	/* failure chance determined, check if attack fumbles */
	if (force_fail || rn2(mlev(magr) * 2) < chance) {
		if (youagr) {
			pline_The("air crackles around you.");
			u.uen += mlev(magr) / 2;
		}
		else if (canseemon(magr) && flags.soundok) {
			pline_The("air crackles around %s.", mon_nam(magr));
			if (magr->mspec_used)
				magr->mspec_used /= 2;
		}
		return MM_MISS;
	}

	/* print spell-cast message */
	if (spellnum) {
		if ((youagr || (youdef && !is_undirected_spell(spellnum) && cansee(tarx, tary)) || canspotmon(magr)) && magr->mtyp != PM_HOUND_OF_TINDALOS) {
			if (is_undirected_spell(spellnum) || notarget)
				buf[0] = '\0';
			else
			{
				Sprintf(buf, " at %s",
					youdef
					? ( (Displaced && (!foundem)) ?
						"your displaced image" :
						(!foundem) ?
						"a spot near you" :
						"you")
					: (canspotmon(mdef) ? mon_nam(mdef) : "something"));
			}
			pline("%s cast%s a spell%s!",
				youagr ? "You" : canspotmon(magr) ? Monnam(magr) : "Something",
				youagr ? "" : "s",
				buf);
		}
	}

	/* do spell */
	if (spellnum) {
		/* special case override: the avatar of lolth can ask Lolth to intercede instead of casting a spell */
		if (youdef && magr->mtyp == PM_AVATAR_OF_LOLTH && !strcmp(urole.cgod, "Lolth") && !is_undirected_spell(spellnum) && !magr->mpeaceful){
			u.ugangr[Align2gangr(A_CHAOTIC)]++;
			angrygods(A_CHAOTIC);
			result = MM_HIT;
		}
		/* generally: cast the spell */
		result = cast_spell(magr, mdef, attk, spellnum, tarx, tary);
	}
	else if (!notarget || youagr) {
		/* no spell selected; this probably means we have an elemental spell to cast */
		/* these typically result in either a beam (zaps a cone of cold, etc) or hand-to-hand magic (covered in frost, etc) */
		/* the player can be prompted to cast in a direction; otherwise, we need a target */
		result = elemspell(magr, mdef, attk, tarx, tary);
	}
	if (result) {
		/* if attacking a displacement, monsters figure out you weren't there */
		if (magr && youdef && (tarx != u.ux || tary != u.uy)) {
			magr->mux = magr->muy = 0;
		}
	}
	return result;
}

/* elemspell()
 * 
 * Magr casts an elemental spell (ray or hth) at mdef, who they think is at (tarx, tary)
 * If !mdef or (tarx, tary) is (0,0), magr does nothing
 * 
 * Returns MM_MISS only if the spellcasting failed silently and took no time at all
 */
int
elemspell(magr, mdef, attk, tarx, tary)
struct monst * magr;
struct monst * mdef;
struct attack * attk;
int tarx;
int tary;
{
	boolean youagr = (magr == &youmonst);
	boolean youdef = (mdef == &youmonst);
	struct permonst * pa = (youagr ? youracedata : magr->data);
	boolean foundem = (mdef && (tarx == x(mdef) && tary == y(mdef)));
	boolean rangedspell;
	int adtyp = attk->adtyp;
	int dmn = mlev(magr) / 3 + 1;
	/* cap level contribution to ndice to MAX_BONUS_DICE */
	if (dmn > MAX_BONUS_DICE)
		dmn = MAX_BONUS_DICE;
	/* increment ndice by specified attack */
	if (attk->damn)
		dmn += (int)(attk->damn);
	/* floor dmn */
	if (dmn < 1) dmn = 1;

	/* is it a ranged spell? */
	if (!tarx && !tary) {
		if (youagr) {
			/* get a direction to cast in (assumes ranged spell is possible) */
			if (!getdir((const char *)0)) {
				pline_The("air crackles around you.");
				return MM_MISS;
			}
			else {
				rangedspell = TRUE;
				tarx = u.ux + u.dx;
				tary = u.uy + u.dy;
			}
		}
		else {
			/* monsters just fail */
			if (canseemon(magr) && flags.soundok)
				pline_The("air crackles around %s.", mon_nam(magr));
			impossible("monster got to elemspell() with no target location?"); // test; does this happen?
			pline("%s is attempting to cast at %s.", Monnam(magr), mdef ? mon_nam(mdef) : "no target");
			return MM_MISS;
		}
	}
	else if (mdef && dist2(x(magr), y(magr), tarx, tary) <= 2 && tarx == x(mdef) && tary == y(mdef)) {
		rangedspell = FALSE;
	}
	else {
		rangedspell = TRUE;
	}

	/* hand to hand magic */
	if (!rangedspell)
	{
		int dmd = 6;
		int dmg;

		/* if there's no target where we're casting, fail */
		if (!foundem) {
			if ((youagr || youdef || canspotmon(magr)) && magr->mtyp != PM_HOUND_OF_TINDALOS)	{
				pline("%s cast%s a spell at %s!",
					youagr ? "You" : canseemon(magr) ? Monnam(magr) : "Something",
					youagr ? "" : "s",
					levl[tarx][tary].typ == WATER
					? "empty water" : "thin air");
			}
			return MM_MISS;
		}
		/* otherwise, print a spellcasting message */
		else {
			if ((youagr || youdef || canspotmon(magr)) && magr->mtyp != PM_HOUND_OF_TINDALOS) {
				pline("%s cast%s a spell at %s!",
					youagr ? "You" : canspotmon(magr) ? Monnam(magr) : "Something",
					youagr ? "" : "s",
					youdef ? "you" : (canspotmon(mdef) ? mon_nam(mdef) : "something"));
			}
		}

		/* possibly override die size */
		if (attk->damd)
			dmd = (int)(attk->damd);
		/* increase die size */
		if (!youagr && is_alabaster_mummy(magr->data) && magr->mvar_syllable == SYLLABLE_OF_POWER__KRAU)
			dmd *= 1.5;

		/* calculate damage */
		dmg = d(dmn, dmd);

		/* apply damage reductions */
		if (Half_spel(mdef))
			dmg = (dmg + 1) / 2;
		if (youdef && u.uvaul_duration)
			dmg = (dmg + 1) / 2;
		
		/* handle damage type modifiers */
		switch (adtyp) {
		case AD_OONA:
			adtyp = u.oonaenergy;
			break;
		case AD_RBRE:
			switch (rnd(3)){
			case 1: adtyp = AD_FIRE; break;
			case 2: adtyp = AD_COLD; break;
			case 3: adtyp = AD_ELEC; break;
			}
			break;
		}

		/* do the melee-range spell */
		/* this should return */
		switch (adtyp) {
		case AD_SLEE:
			/* message */
			if (youdef || canspotmon(mdef)) {
				pline("%s%s enveloped in a puff of gas.",
					youdef ? "You" : Monnam(mdef),
					youdef ? "'re" : " is"
					);
			}
			/* do effect */
			if (Sleep_res(mdef) || 
				(youdef ? Breathless : breathless_mon(mdef))) {
				if (youdef || canseemon(mdef)) {
					shieldeff(tarx, tary);
					if (youdef) You("don't feel sleepy!");
					else pline("%s is unaffected.", Monnam(mdef));
				}
			}
			else {
				if (youdef)
					fall_asleep(-dmg, TRUE);
				else if (sleep_monst(mdef, dmg, -1)) {
					if (canseemon(mdef))
						pline("%s falls asleep!", Monnam(mdef));
					mdef->mstrategy &= ~STRAT_WAITFORU;
					slept_monst(mdef);
				}
			}
			return MM_HIT;

		case AD_ELEC:
			/* message */
			if (youdef || canspotmon(mdef)) {
				pline("Lightning crackles around %s.",
					youdef ? "you" : mon_nam(mdef)
					);
			}
			/* do effect */
			if (Shock_res(mdef)) {
				if (youdef || canseemon(mdef)) {
					shieldeff(tarx, tary);
					if (youdef) pline("But you resist the effects.");
					else pline("But %s resists the effects.", mhe(mdef));
				}
				dmg = 0;
			}
			/* damage inventory */
			if (!InvShock_res(mdef)){
				destroy_item(mdef, WAND_CLASS, AD_ELEC);
			}
			return xdamagey(magr, mdef, attk, dmg);

		case AD_FIRE:
			/* message */
			if (youdef || canspotmon(mdef)) {
				pline("%s%s enveloped in flames.",
					youdef ? "You" : Monnam(mdef),
					youdef ? "'re" : " is"
					);
			}
			/* do effect */
			if (Fire_res(mdef)) {
				if (youdef || canseemon(mdef)) {
					shieldeff(tarx, tary);
					if (youdef) pline("But you resist the effects.");
					else pline("But %s resists the effects.", mhe(mdef));
				}
				dmg = 0;
			}
			/* damage inventory */
			if (!InvFire_res(mdef)){
				destroy_item(mdef, POTION_CLASS, AD_FIRE);
				if (!rn2(6)) destroy_item(mdef, SCROLL_CLASS, AD_FIRE);
				if (!rn2(10)) destroy_item(mdef, SPBOOK_CLASS, AD_FIRE);
			}
			/* other effects */
			if (youdef) {
				burn_away_slime();
				melt_frozen_air();
			}
			return xdamagey(magr, mdef, attk, dmg);

		case AD_COLD:
			/* message */
			if (youdef || canspotmon(mdef)) {
				pline("%s%s covered in frost.",
					youdef ? "You" : Monnam(mdef),
					youdef ? "'re" : " is"
					);
			}
			/* do effect */
			if (Cold_res(mdef)) {
				if (youdef || canseemon(mdef)) {
					shieldeff(tarx, tary);
					if (youdef) pline("But you resist the effects.");
					else pline("But %s resists the effects.", mhe(mdef));
				}
				dmg = 0;
			}
			/* damage inventory */
			if (!InvCold_res(mdef)){
				destroy_item(mdef, POTION_CLASS, AD_FIRE);
			}
			/* other effects */
			if (youdef) {
				roll_frigophobia();
			}
			return xdamagey(magr, mdef, attk, dmg);

		case AD_MAGM:
			/* message */
			if (youdef || canspotmon(mdef)) {
				pline("%s %s hit by a shower of missiles!",
					youdef ? "You" : Monnam(mdef),
					youdef ? "are" : "is"
					);
			}
			/* do effect */
			if (Magic_res(mdef)) {
				if (youdef || canseemon(mdef)) {
					shieldeff(tarx, tary);
					pline_The("missiles bounce off!");
				}
				dmg = 0;
			}
			return xdamagey(magr, mdef, attk, dmg);

		case AD_STAR:
			/* message */
			if (youdef || canspotmon(mdef)) {
				pline("%s %s hit by a shower of silver stars!",
					youdef ? "You" : Monnam(mdef),
					youdef ? "are" : "is"
					);
			}
			/* special antimagic effect */
			if (youdef)
				drain_en(dmg / 2);
			else
				mdef->mspec_used += dmg / 2;
			/* approximate as dmn/3 silver stars */
			
			if (Half_phys(mdef))
				dmg = (dmg + 1) / 2;
			if (youdef && u.uvaul_duration)
				dmg = (dmg + 1) / 2;

			if (dmg > 0) {
				int i;
				/* reduce by DR */
				for (i = dmn / 3; i > 0; i--) {
					dmg -= (youdef ? roll_udr(magr) : roll_mdr(mdef, magr));
				}
				/* deals silver-hating damage */
				if (hates_silver((youdef ? youracedata : mdef->data))) {
					for (i = dmn / 3; i > 0; i--) {
						dmg += rnd(20);
					}
				}
			}
			else {
				dmg = 1;
			}
			/* extra message for the silver */
			if (youdef && hates_silver(youracedata)) {
				if (noncorporeal(youracedata)) {
					pline("The silver stars sear you!");
				}
				else {
					pline("The silver stars sear your %s!", body_part(BODY_FLESH));
				}
			}
			return xdamagey(magr, mdef, attk, dmg);
		}
	}
	/* ranged magic */
	else {
		/* set up zapdata structure */
		struct zapdata zapdata = { 0 };

		/* determine if magr and mdef are lined up (or magr thinks they are) */
		/* also checks for direct friendly fire */
		if (m_online(magr, mdef, tarx, tary, (youagr ? FALSE : (magr->mtame && !magr->mconf)), FALSE)) {
			/* dmn was calculated above */
			/* swap damage type as appropriate */
			if (adtyp == AD_RBRE || adtyp == AD_RETR)
			{
				/* random between fire/cold/elec damage */
				switch (rn2(3))
				{
				case 0: adtyp = AD_FIRE; break;
				case 1: adtyp = AD_COLD; break;
				case 2: adtyp = AD_ELEC; break;
				}
			}
			else if (adtyp == AD_OONA) {
				/* Oona */
				adtyp = u.oonaenergy;
			}
			/* now that we have finallized adtyp, do basic setup of zapdata structure */
			basiczap(&zapdata, adtyp, ZAP_SPELL, dmn);

			/* message */
			if (youdef) {
				/* message */
				if (canseemon(magr)) {
					pline("%s zaps you with a %s!", Monnam(magr),
						flash_type(adtyp, ZAP_SPELL));
				}
				else {
					You("are zapped with a %s!",
						flash_type(adtyp, ZAP_SPELL));
				}
			}
			else {
				if (youagr || canseemon(magr)) {
					if (mdef) {
						pline("%s %s %s with a %s!",
							(youagr ? "You" : Monnam(magr)),
							(youagr ? "zap" : "zaps"),
							(canspotmon(mdef) ? mon_nam(mdef) : "something"),
							flash_type(adtyp, ZAP_SPELL)
							);
					}
					else {
						pline("%s %s a %s!",
							(youagr ? "You" : Monnam(magr)),
							(youagr ? "cast" : "casts"),
							flash_type(adtyp, ZAP_SPELL)
							);
					}
				}
			}
			/* modify zap for some spells */
			if (adtyp == AD_STAR) {
				zapdata.unreflectable = ZAP_REFL_NEVER;
				zapdata.damd = 8;
			}
			if (adtyp == AD_DISN) {
				zapdata.unreflectable = ZAP_REFL_NEVER;
			}

			/* do the zap */
			zap(magr, x(magr), y(magr), sgn(tarx - x(magr)), sgn(tary - y(magr)), rn1(7, 7), &zapdata);

			/* return result */
			/* unfortunately, cannot tell if either lifesaved */
			return (MM_HIT
				| ((*hp(magr) < 0) ? MM_AGR_DIED : 0L)
				| ((mdef && *hp(mdef) < 0) ? MM_DEF_DIED : 0L)	/* note: mdef is not guaranteed here */
				);
		}
		/* if not lined up, no casting */
		else {
			return MM_MISS;
		}
	}
	/* should not be reached */
	impossible("end of elemspell() reached");
	return MM_MISS;
}

/* cast_spell
 * 
 * magr is casting a spell, possibly at mdef (who may or may not exist)
 * 
 * handles all of mvu, mvm, uvm.
 */
int
cast_spell(magr, mdef, attk, spell, tarx, tary)
struct monst * magr;
struct monst * mdef;	/* optional */
struct attack * attk;
int spell;
int tarx;
int tary;
{
	/* validate */
	if (!magr) {
		impossible("cast_spell() called with no caster");
		return MM_MISS;
	}

	/*debug*/
	//if (wizard) {
	//	pline("[%s casting %s at %s]",
	//		m_monnam(magr),
	//		spellname[spell],
	//		(mdef == &youmonst) ? "you" : mdef ? m_monnam(mdef) : "no one"
	//		);
	//}

	boolean youagr = (magr == &youmonst);
	boolean youdef = (mdef == &youmonst);
	boolean malediction = (youdef && (magr->iswiz || (magr->data->msound == MS_NEMESIS && rn2(2))));
	boolean foundem = (mdef && (tarx == x(mdef) && tary == y(mdef)));
	int result = MM_MISS;	/* to store intermediary xhity-esque returns */

	/* common to all summon spells */
	int summonflags = (NO_MINVENT | MM_NOCOUNTBIRTH | MM_ESUM |
		((youagr || magr->mtame) ? MM_EDOG : 0) |
		((!youagr && !magr->mpeaceful) ? MM_ANGRY : 0));

	/* calculate default damage -- many spells ignore or override this amount */
	int dmn = min(MAX_BONUS_DICE, mlev(magr) / 3 + 1);
	if (attk && attk->damn)
		dmn += attk->damn;
	int dmd = (attk && attk->damd) ? attk->damd : 6;
	int dmg = d(dmn, dmd);

	switch (spell)
	{
//////////////////////////////////////////////////////////////////////////////////////
// SINGLE-TARGET OFFENSE
//////////////////////////////////////////////////////////////////////////////////////
	case PSI_BOLT:
		/* needs direct target */
		if (!foundem) {
			impossible("No mdef for psibolt");
			return MM_MISS;
		}
		/* calculate resistance */
		if (Magic_res(mdef) || (!youdef && resist(mdef, 0, 0, FALSE))) {
			shieldeff(x(mdef), y(mdef));
			dmg = (dmg + 1) / 2;
			if (dmg > 25)
				dmg = 25;
		}
		else {
			if (dmg > 50)
				dmg = 50;
		}
		/* message */
		if (youdef) {
			if (dmg <= 5)
				You("get a slight %sache.", body_part(HEAD));
			else if (dmg <= 10)
				Your("brain is on fire!");
			else if (dmg <= 20)
				Your("%s suddenly aches painfully!", body_part(HEAD));
			else{
				Your("%s suddenly aches very painfully!", body_part(HEAD));
			}
		}
		else if (canseemon(mdef)) {
			pline("%s winces%s", Monnam(mdef), (dmg <= 5) ? "." : "!");
		}
		/* deal damage */
		return xdamagey(magr, mdef, attk, dmg);

	case OPEN_WOUNDS:
		/* needs direct target */
		if (!foundem) {
			impossible("No mdef for open wounds");
			return MM_MISS;
		}
		/* calculate resistance */
		if (Magic_res(mdef) || (!youdef && resist(mdef, 0, 0, FALSE))) {
			shieldeff(x(mdef), y(mdef));
			dmg = (dmg + 1) / 2;
			if (dmg > 30)
				dmg = 30;
		}
		else {
			if (dmg > 60)
				dmg = 60;
		}
		/* message */
		if (youdef) {
			if (dmg >= *hp(mdef)) {
				Your("body is covered with deadly wounds!");
				dmg = max(*hp(mdef) - 5, 0);	/* NOT JUST A MESSAGE, ALSO HAS AN EFFECT FOR YOUDEF */
			}
			else if (dmg <= 5)
				Your("skin itches badly for a moment.");
			else if (dmg <= 15)
				pline("Wounds appear on your body!");
			else if (dmg <= 30)
				pline("Severe wounds appear on your body!");
			else
				Your("body is covered with deep, painful wounds!");
		}
		else if (canseemon(mdef)) {
			if (dmg <= 5)
				pline("%s looks itchy!", Monnam(mdef));
			else if (dmg <= 10)
				pline("Wounds appear on %s!", mon_nam(mdef));
			else if (dmg <= 20)
				pline("Severe wounds appear on %s!", mon_nam(mdef));
			else
				pline("%s is covered in wounds!", Monnam(mdef));
		}
		/* deal damage */
		return xdamagey(magr, mdef, attk, dmg);

	case MAGIC_MISSILE:
	case CONE_OF_COLD:
	case LIGHTNING_BOLT:
	case SLEEP:
	case DISINT_RAY:
		/* these are allowed to miss */
		if (!mdef) {
			impossible("ray spell with no mdef?");
			return MM_MISS;
		}
		else if (spell == DISINT_RAY && dist2(x(magr), y(magr), tarx, tary) <= 2) {
			/* disint ray is ranged-only, substitute with psibolt */
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		else {
			struct attack alt_attk = *attk;
			switch (spell)
			{
			case MAGIC_MISSILE:		alt_attk.adtyp = AD_MAGM; break;
			case CONE_OF_COLD:		alt_attk.adtyp = AD_COLD; break;
			case LIGHTNING_BOLT:	alt_attk.adtyp = AD_ELEC; break;
			case SLEEP:				alt_attk.adtyp = AD_SLEE; break;
			case DISINT_RAY:		alt_attk.adtyp = AD_DISN; break;
			}
			return elemspell(magr, mdef, &alt_attk, tarx, tary);
		}

	case DRAIN_LIFE: 
		/* similar to player spell "drain life", but only works at close range */
		if (dist2(x(magr), y(magr), tarx, tary) <= 2) {
			/* note: magic resistance doesn't protect against "drain life" spell */
			if (Drain_res(mdef)) {
				shieldeff(x(mdef), y(mdef));
				if (youdef)
					You_feel("momentarily frail.");
				/* no message for monster mdef */
			}
			else {
				if (youdef) {
					Your("body deteriorates!");
					exercise(A_CON, FALSE);
					losexp("life drainage", TRUE, FALSE, FALSE);
				}
				else {
					/* drainlevel from mon */
					/* print message first -- this should happen before the victim is drained/dies */
					if (canseemon(mdef))
						pline("%s suddenly seems weaker!", Monnam(mdef));

					/* for monsters, we need to make something up -- drain 2d6 maxhp, 1 level */
					dmg = d(2, 6);

					/* kill if this will level-drain below 0 m_lev, or lifedrain below 1 maxhp */
					if (mlev(mdef) == 0 || *hpmax(mdef) <= dmg) {
						/* clean up the maybe-dead monster, return early */
						if (youagr)
							killed(mdef);
						else
							monkilled(mdef, "", attk->adtyp);
						/* is it dead, or was it lifesaved? */
						if (mdef->mhp > 0)
							return (MM_HIT | MM_DEF_LSVD);	/* lifesaved */
						else
							return (MM_HIT | MM_DEF_DIED | ((youagr || grow_up(magr, mdef)) ? 0 : MM_AGR_DIED));
					}
					else {
						/* drain stats */
						mdef->m_lev--;
						mdef->mhpmax -= dmg;
					}
				}
			}
			if (youdef) stop_occupation();
		}
		return MM_HIT;

	case ARROW_RAIN:
		/* needs direct target */
		if (!foundem) {
			impossible("arrow rain with no mdef?");
			return MM_MISS;
		}
		else
		{
			struct obj *otmp;
			int weap;

			/* get weapon type to rain */
			if (rn2(3)) weap = ARROW;
			else if (!rn2(3)) weap = DAGGER;
			else if (!rn2(3)) weap = SPEAR;
			else if (!rn2(3)) weap = KNIFE;
			else if (!rn2(3)) weap = JAVELIN;
			else if (!rn2(3)) weap = AXE;
			else {
				weap = rnd_class(ARROW, WORM_TOOTH - 1);
				if (weap == TRIDENT) weap = JAVELIN;
			}
			/* make them */
			otmp = mksobj(weap, NO_MKOBJ_FLAGS);
			otmp->quan = min(dmn, 16);
			otmp->owt = weight(otmp);
			otmp->spe = 0;

			/* call projectile() to shoot fire all the weapons */
			m_shot.n = otmp->quan;
			m_shot.s = TRUE;
			m_shot.o = otmp->otyp;
			for (m_shot.i = 1; m_shot.i <= m_shot.n; m_shot.i++) {
				(void)projectile(magr, otmp, (void *)0, HMON_FIRED, x(mdef), y(mdef), 0, 0, 0, 0, FALSE, FALSE, FALSE);
			}
		}
		return MM_HIT;

	case LIGHTNING:
		/* needs direct target */
		if (!foundem) {
			impossible("lightning with no mdef?");
			return MM_MISS;
		}
		else
		{
			boolean reflects;
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("A bolt of lightning strikes down at %s from above!",
					youdef ? "you" : mon_nam(mdef));
			}
			if (youdef)
				reflects = ureflects("It bounces off your %s%s.", "");
			else
				reflects = mon_reflects(mdef, "It bounces off %s %s.");

			/* check resistance and override damage */
			if (reflects || Shock_res(mdef)) {
				shieldeff(x(mdef), y(mdef));
				dmg = 0;
			}
			else {
				dmg = d(8, 6);
				if (Half_spel(mdef))
					dmg = (dmg + 1) / 2;
				if (youdef && u.uvaul_duration)
					dmg = (dmg + 1) / 2;
			}

			/* destroy items if it hit */
			if (!(reflects || InvShock_res(mdef))) {
				destroy_item(mdef, WAND_CLASS, AD_ELEC);
				destroy_item(mdef, RING_CLASS, AD_ELEC);
			}
			/* blinds defender (player only?) */
			if (youdef && !resists_blnd(&youmonst)) {
				You("are blinded by the flash!");
				make_blinded((long)rnd(100), FALSE);
				if (!Blind) Your1(vision_clears);
			}
		}
		return xdamagey(magr, mdef, attk, dmg);

	case FIRE_PILLAR:
		/* needs direct target */
		if (!foundem) {
			impossible("fire pillar with no mdef?");
			return MM_MISS;
		}
		else
		{
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("A pillar of fire strikes all around %s!",
					youdef ? "you" : mon_nam(mdef));
			}

			/* check resistance and override damage */
			if (Fire_res(mdef)) {
				shieldeff(x(mdef), y(mdef));
				dmg = 0;
			}
			else {
				dmg = d(8, 6);
				if (Half_spel(mdef))
					dmg = (dmg + 1) / 2;
				if (youdef && u.uvaul_duration)
					dmg = (dmg + 1) / 2;
			}

			(void)burnarmor(mdef);
			destroy_item(mdef, SCROLL_CLASS, AD_FIRE);
			destroy_item(mdef, POTION_CLASS, AD_FIRE);
			destroy_item(mdef, SPBOOK_CLASS, AD_FIRE);

			if (youdef) {
				burn_away_slime();
				melt_frozen_air();
			}

			(void)burn_floor_paper(tarx, tary, TRUE, youagr);
		}
		return xdamagey(magr, mdef, attk, dmg);

	case GEYSER:
		/* needs direct target */
		if (!foundem) {
			impossible("geyser with no mdef?");
			return MM_MISS;
		}
		else
		{
			struct obj * boots = (youdef ? uarmf : which_armor(mdef, W_ARMF));

			if (boots && boots->otyp == WATER_WALKING_BOOTS) {
				/* message part 1*/
				if (youagr || youdef || canseemon(mdef)){
					pline("A sudden geyser erupts under %s feet!",
						youdef ? "your" : s_suffix(mon_nam(mdef)));

					if (youdef || canseemon(mdef))
						makeknown(boots->otyp);
				}

				/* message part 2 and effect */
				if (youdef) {
					if (ACURR(A_DEX) >= 14){
						You("put the added momentum to good use.");
						if (ACURR(A_DEX) == 25) youmonst.movement += 12;
						else if (ACURR(A_DEX) >= 18) youmonst.movement += 8;
						else youmonst.movement += 6;
					}
					else if (ACURR(A_DEX) <= 10){
						You("are knocked around by the geyser's force!");
						if (ACURR(A_DEX) <= 3) dmg = d(8, 6);
						else if (ACURR(A_DEX) <= 6) dmg = d(4, 6);
						else if (ACURR(A_DEX) <= 10) dmg = rnd(6);
					}
				}
				else {
					if (mdef->data->mmove >= 14) pline("%s puts the added monmentum to good use!", Monnam(mdef));
					else if (mdef->data->mmove <= 10) pline("%s is knocked around by the geyser's force!", Monnam(mdef));
					
					if (mdef->data->mmove >= 25) mdef->movement += 12;
					else if (mdef->data->mmove >= 18) mdef->movement += 8;
					else if (mdef->data->mmove >= 14) mdef->movement += 6;
					else if (mdef->data->mmove <= 3) dmg = d(8, 6);
					else if (mdef->data->mmove <= 6) dmg = d(4, 6);
					else if (mdef->data->mmove <= 10) dmg = rnd(6);
				}
			}
			else {
				/* message */
				if (youagr || youdef || canseemon(mdef)) {
					pline("A sudden geyser slams into %s from nowhere!",
						youdef ? "you" : mon_nam(mdef));
				}
				/* this is physical damage, not magical damage */
				dmg = d(8, 6);
				if (Half_phys(mdef))
					dmg = (dmg + 1) / 2;
				if (youdef && u.uvaul_duration)
					dmg = (dmg + 1) / 2;

				if (boots && boots->otyp == find_mboots()) {
					if(youdef) pline("Good thing you're wearing mud boots!");
				}
				else
					water_damage(youdef ? invent : mdef->minvent, FALSE, FALSE, FALSE, mdef);
			}
		}
		return xdamagey(magr, mdef, attk, dmg);

	case ACID_RAIN: /* as seen in the Lethe patch */
		/* needs direct target */
		if (!foundem) {
			impossible("acid rain with no mdef?");
			return MM_MISS;
		}
		else {
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("A torrent of burning acid rains down on %s!",
					youdef ? "you" : mon_nam(mdef));
			}

			struct obj * helm = (youdef ? uarmh : which_armor(mdef, W_ARMH));

			if (helm && is_wide_helm(helm)) {
				dmg = 0;
				if (youagr || youdef || canseemon(mdef)) {
					pline("It runs off the brim of %s %s.",
						youdef ? "your" : s_suffix(mon_nam(mdef)),
						OBJ_DESCR(objects[helm->otyp]));
				}
			}
			else {
				/* check resistance and override damage */
				dmg = d(8, 6);
				if (Acid_res(mdef)) {
					shieldeff(x(mdef), y(mdef));
					if (youdef)
						pline("It feels mildly uncomfortable.");
					dmg = 0;
				}
				/* damage inventory */
				if (!InvAcid_res(mdef)) {
					erode_obj(youdef ? uwep : MON_WEP(mdef), TRUE, FALSE);
					erode_obj(youdef ? uswapwep : MON_SWEP(mdef), TRUE, FALSE);
					erode_armor(mdef, TRUE);
					destroy_item(mdef, POTION_CLASS, AD_FIRE);
				}
				water_damage(youdef ? invent : mdef->minvent, FALSE, FALSE, FALSE, mdef);

				/* blind defender (player only?) */
				if (youdef && !resists_blnd(&youmonst) && rn2(2)) {
					pline_The("acid gets into your %s!", eyecount(youracedata) == 1 ?
						body_part(EYE) : makeplural(body_part(EYE)));
					make_blinded((long)rnd(Acid_resistance ? 10 : 50), FALSE);
					if (!Blind) Your1(vision_clears);
				}
			}
			/* TODO: corrode floor objects */
		}
		return xdamagey(magr, mdef, attk, dmg);

	case HAIL_FLURY:
	case ICE_STORM:
		/* ice storm is identical to hail flury, except it overrides dmg to 8d8 */
		/* needs direct target */
		if (!foundem) {
			impossible("ice storm / hail flury with no mdef?");
			return MM_MISS;
		}
		else {
			int pdmg = (spell == ICE_STORM ? d(4, 8) : (dmg + 1)/2);	/* physical */
			int cdmg = (spell == ICE_STORM ? d(4, 8) : (dmg + 1)/2);	/* cold */
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("%s pummel %s from all sides!",
					(spell == ICE_STORM ? "Chunks of ice" : "Hailstones"),
					youdef ? "you" : mon_nam(mdef));
			}

			/* calculate physical damage */
			if (Half_phys(mdef))
				pdmg = (pdmg + 1) / 2;
			/* apply average DR */
			pdmg -= max(0, (youdef ? u.udr : avg_mdr(mdef)));
			if (pdmg < 1)
				pdmg = 1;

			/* calculate cold damage */
			if (Cold_res(mdef)) {
				shieldeff(x(mdef), y(mdef));
				cdmg = 0;
			}
			else {
				if (Half_spel(mdef))
					cdmg = (cdmg + 1) / 2;
			}
			if (!InvCold_resistance) {
				destroy_item(mdef, POTION_CLASS, AD_COLD);
			}

			/* sum damage components to override dmg */
			dmg = pdmg + cdmg;
			/* apply u.uvaul to all */
			if (youdef && u.uvaul_duration)
				dmg = (dmg + 1) / 2;

			/* player cold madness*/
			if (youdef) roll_frigophobia();
		}
		return xdamagey(magr, mdef, attk, dmg);

	case DEATH_TOUCH:
		/* needs direct target */
		if (!foundem) {
			impossible("death touch with no mdef?");
			return MM_MISS;
		}
		else if (youagr && (u.sealsActive & SEAL_BUER)) {
			/* no death magic -- substitute psi bolt */
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		else if (dist2(x(magr), y(magr), tarx, tary) <= 2) {
			/* message */
			char heshe[BUFSZ];
			if (!youagr)
				Strcpy(heshe, mhe(magr));	/* need to allocate a buffer for upstart to modify */

			if (youagr)
				pline("You're using the touch of death!");
			else if (youdef || (canseemon(mdef) && mdef->mtame))
				pline("Oh no, %s's using the touch of death!", heshe);
			else if (canseemon(mdef))
				pline("%s's using the touch of death!", upstart(heshe));

			/* check resistance cases and do effects */
			if (nonliving(mdef->data) || is_demon(mdef->data)) {
				if (youdef || canseemon(mdef)) {
					pline("%s seem%s no deader than before.",
						youdef ? "You" : Monnam(mdef),
						youdef ? "" : "s"
						);
				}
				dmg = 0;
			}
			else if (ward_at(x(mdef), y(mdef)) == CIRCLE_OF_ACHERON) {
				if (youdef || canseemon(mdef)) {
					pline("%s %s already beyond Acheron.",
						youdef ? "You" : Monnam(mdef),
						youdef ? "are" : "is"
						);
				}
				dmg = 0; 
			}
			else if ((youdef && (u.sealsActive & SEAL_OSE)) || resists_death(mdef)) {
				shieldeff(x(mdef), y(mdef));
				dmg = 0;
			}
			else {
				/* split between vs player and vs monster */
				if (youdef) {
					if (!Magic_res(mdef) && (rn2(mlev(magr)) > 12)) {
						if (Hallucination) {
							You("have an out of body experience.");
						}
						else if (*hp(mdef) >= 100){
							Your("%s stops!  When it finally beats again, it is weak and thready.", body_part(HEART));
							*hp(mdef) -= d(8, 8);
						}
						else {
							killer_format = KILLED_BY_AN;
							killer = "touch of death";
							dmg = *hp(mdef);
						}
					}
					else {
						if (Magic_res(mdef))
							shieldeff(x(mdef), y(mdef));
						//	    pline("Lucky for you, it didn't work!");
						Your("%s flutters!", body_part(HEART));
						dmg = 8; //you still take damage
					}
				}
				else {
					/* monster */
					boolean resisted = FALSE;

					if (!(resisted = (Magic_res(mdef) || resists_death(mdef) || resist(mdef, 0, 0, FALSE))) ||
						rn2(mlev(magr)) > 12
						){
						if (is_delouseable(mdef->data)){
							pline("The parasite is killed!");
							delouse(mdef, AD_DEAD);
						}
						else {
							mdef->mhp = -1;
							if (youagr) killed(mdef);
							else monkilled(mdef, "", AD_SPEL);
						}
						return ((*hp(mdef)>0 ? MM_DEF_LSVD : MM_DEF_DIED) | MM_HIT);
					}
					else {
						if (resisted) shieldeff(x(mdef), y(mdef));
						if (youagr || canseemon(mdef))
						{
							if (mdef->mtame)
								pline("Lucky for %s, it didn't work!", mon_nam(mdef));
							else
								pline("That didn't work...");
						}
						dmg = 0;
					}
				}
			}
		}
		return xdamagey(magr, mdef, attk, dmg);


	case PLAGUE:
		/* needs direct target */
		if (!foundem) {
			impossible("plague with no mdef?");
			return MM_MISS;
		}
		else {
			/* apply sickness to defender (player-only) */
			if (youdef) {
				dmg = 0;
				if (!Sick_res(mdef) && !umechanoid) {
					You("are afflicted with disease!");
					make_sick(Sick ? Sick / 3L + 1L : (long)rn1(ACURR(A_CON), 20),
						magr->data->mname, TRUE, SICK_NONVOMITABLE);
				}
				else {
					You_feel("slightly infectious.");
				}
			}
			else if (!Sick_res(mdef)) {
				/* 1/10 chance of instakill */
				if (!rn2(10)){
					if (youagr) killed(mdef);
					else monkilled(mdef, "", AD_SPEL);
					/* instakill */
					return ((*hp(mdef) > 0 ? MM_DEF_LSVD : MM_DEF_DIED) | MM_HIT);
				}
				else {
					dmg = rnd(12);
				}
			}
		}
		return xdamagey(magr, mdef, attk, dmg);

	case FILTH:
		/* needs direct target */
		if (!foundem) {
			impossible("filth with no mdef?");
			return MM_MISS;
		}
		else
		{
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("A cascade of filth pours onto %s!",
					youdef ? "you" : mon_nam(mdef));
			}

			/* glib your hands (player-only) */
			if (youdef && freehand() && rn2(3)) {
				boolean wasGlib = !!Glib;
				Glib += rn1(20, 9);
				Your("%s %s!", makeplural(body_part(HAND)),
					(wasGlib ? "are filthier than ever" : "get slimy"));
			}

			/* filth weapons */
			const int slots[] = { W_WEP, W_SWAPWEP, 0L };
			int i = 0;
			while(slots[i]) {
				struct obj *otmp;
				i++;
				/* get weapon */
				switch (slots[i]) {
				case W_WEP:
					otmp = youdef ? uwep : MON_WEP(mdef);
					break;
				case W_SWAPWEP:
					otmp = youdef ? uswapwep : MON_SWEP(mdef);
					break;
				}
				/* if we got a weapon, maybe filth it */
				if (otmp && !rn2(20)) {
					if (youdef || canseemon(mdef)) {
						pline("%s %s is coated in gunk!",
							youdef ? "Your" : s_suffix(Monnam(mdef)),
							xname(otmp));
					}
					/* filth weapon */
					if (is_poisonable(otmp)) otmp->opoisoned = OPOISON_FILTH;
					if (otmp->otyp == VIPERWHIP) otmp->opoisonchrgs = 0;

					/* glib */
					otmp->greased = TRUE;
					if (youdef) {
						Glib += rn1(20, 9);
					}
					else {
						/* since monsters don't glib, we need to forcibly drop their weapon now */
						if (!otmp->cursed) {
							obj_extract_self(otmp);
							mdrop_obj(mdef, otmp, FALSE);
						}
					}
				}
			}

			/* filth blinds the defender */
			if (haseyes(mdef->data) && rn2(3)
				&& !(magr->mtyp == PM_DEMOGORGON)	/* Demogorgon's doesn't */
				&& !(youdef && Blindfolded)			/* Blindfolding yourself keeps it out of your eyes */
				) {
				if (youdef) {
					long old = u.ucreamed;
					u.ucreamed += rn1(20, 9);
					Your("%s is coated in %sgunk!", body_part(FACE),
						(old ? "even more " : ""));
					make_blinded(Blinded + (long)u.ucreamed - old, FALSE);
				}
				else {
					mdef->mcansee = 0;
					mdef->mblinded = max(mdef->mblinded, rn1(20, 9));
				}
			}

			/* vs player only: vomit */
			if (youdef) {
				You("smell putrid!%s", !umechanoid ? " You gag and vomit." : "");
				if (!umechanoid) vomit();
				/* same effect as "This water gives you bad breath!" */
				struct monst * mtmp;
				for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
					if (!DEADMONSTER(mtmp) && (mtmp != magr))
						monflee(mtmp, 0, FALSE, FALSE);
				}
			}

			/* apply sickness to defender (player-only) */
			if (youdef) {
				if (!Sick && !umechanoid) make_sick((long)rn1(ACURR(A_CON), 20), /* Don't make the PC more sick */
					magr->data->mname, TRUE, SICK_NONVOMITABLE);
			}
			else if (!Sick_res(mdef)) {
				/* 1/10 chance of instakill */
				if (!rn2(10)){
					if (youagr) killed(mdef);
					else monkilled(mdef, "", AD_SPEL);
					/* instakill */
					return ((*hp(mdef) > 0 ? MM_DEF_LSVD : MM_DEF_DIED) | MM_HIT);
				}
			}

			/* piddling physical damage, regardless of sickness resistance */
			dmg = rnd(10);
			if (Half_phys(mdef))
				dmg = (dmg + 1) / 2;
			if (youdef && u.uvaul_duration)
				dmg = (dmg + 1) / 2;
		}
		return xdamagey(magr, mdef, attk, dmg);

	case TURN_TO_STONE:
		/* needs direct target */
		if (!foundem) {
			impossible("turn to stone with no mdef?");
			return MM_MISS;
		}
		else {
			/* message player */
			if (youdef && malediction)
				verbalize(rn2(2) ? "I shall make a statue of thee!" :
				"I condemn thee to eternity unmoving!");

			if (!Stone_res(mdef) &&
				(!rn2(10) || !youdef || !have_lizard())
				){
				result |= xstoney(magr, mdef);
				/* print extra message for player */
				if (youdef) {
					if (Stoned) You_feel("less limber.");
					else You_feel("a momentary stiffness.");
				}
			}
		}
		return result;

	case STRANGLE:
		/* needs direct target */
		if (!foundem) {
			impossible("strangle with no mdef?");
			return MM_MISS;
		}
		else if (!youdef) {	/* only works against player */
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		else {
			struct obj *otmp;
			if (uamul && (Antimagic || uamul->oartifact || uamul->otyp == AMULET_OF_YENDOR)) {
				shieldeff(u.ux, u.uy);
				if (!Blind) Your("%s looks vaguely %s for a moment.", xname(uamul),
					OBJ_DESCR(objects[AMULET_OF_STRANGULATION]));
				else You_feel("a momentary pressure around your %s.", body_part(NECK));
			}
			else {
				if (uamul) {
					Your("%s warps strangely, then turns %s.", xname(uamul),
						OBJ_DESCR(objects[AMULET_OF_STRANGULATION]));
					poly_obj(uamul, AMULET_OF_STRANGULATION);
					curse(uamul);
					Amulet_on();
				}
				else {
					if (malediction) {
						verbalize(rn2(2) ? "Thou desirest the amulet? I'll give thee the amulet!" :
							"Here is the only amulet you'll need!");
					}
					otmp = mksobj(AMULET_OF_STRANGULATION, MKOBJ_NOINIT);
					curse(otmp);
					(void)addinv(otmp);
					pline("%s appears around your %s!", An(xname(otmp)), body_part(NECK));
					setworn(otmp, W_AMUL);
					Amulet_on();
				}
			}
			/* interrupt player */
			nomul(0, (const char *)0);
		}
		return MM_HIT;

	case SILVER_RAYS:
		/* needs direct target */
		if (!foundem) {
			impossible("silver rays with no mdef?");
			return MM_MISS;
		}
		else
		{
			int n = 0;
			char * rays;

			if (zap_hit(mdef, 0, TRUE))
				n++;
			if (zap_hit(mdef, 0, TRUE))
				n++;
			if (!n){
				if (youagr || youdef || canseemon(mdef))
					pline("Silver rays whiz past %s!",
					mon_nam(mdef));
				break;
			}
			if (n == 1)
				rays = "a ray";
			if (n >= 2)
				rays = "rays";
			if (hates_silver(mdef->data)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s seared by %s of silver light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n * 2, 20);
			}
			else if (!Fire_res(mdef) && species_resists_cold(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s burned by %s of silver light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = (d(n, 20) * 3 + 1) / 2;
				if (!InvFire_res(mdef)) {
					destroy_item(mdef, SCROLL_CLASS, AD_FIRE);
					destroy_item(mdef, POTION_CLASS, AD_FIRE);
					destroy_item(mdef, SPBOOK_CLASS, AD_FIRE);
				}
			}
			else if (!Cold_res(mdef) && species_resists_fire(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s frozen by %s of silver light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = (d(n, 20) * 3 + 1) / 2;
				if (!InvCold_res(mdef)) {
					destroy_item(mdef, POTION_CLASS, AD_COLD);
				}
			}
			else if (hates_unblessed_mon(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s seared by %s of concordant light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n, 20) + d(n, 8);
			}
			else if (hates_unholy_mon(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s seared by %s of unholy light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n, 20) + d(n, 9);
			}
			else if (hates_holy_mon(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s seared by %s of holy light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n, 20) + d(n, 7);
			}
			else if (!Fire_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s burned by %s of silver light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n, 20);
				if (!InvFire_res(mdef)) {
					destroy_item(mdef, SCROLL_CLASS, AD_FIRE);
					destroy_item(mdef, POTION_CLASS, AD_FIRE);
					destroy_item(mdef, SPBOOK_CLASS, AD_FIRE);
				}
			}
			else if (!Shock_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s shocked by %s of silver light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n, 20);
				if (!InvShock_res(mdef)) {
					destroy_item(mdef, WAND_CLASS, AD_ELEC);
					destroy_item(mdef, RING_CLASS, AD_ELEC);
				}
			}
			else if (!Cold_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s frozen by %s of silver light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n, 20);
				if (!InvCold_res(mdef)) {
					destroy_item(mdef, POTION_CLASS, AD_COLD);
				}
			}
			else if (!Acid_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s burned by %s of silver light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n, 20);
				if (!InvAcid_res(mdef)) {
					destroy_item(mdef, POTION_CLASS, AD_FIRE);
				}
			}
			else {
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s pierced by %s of silver light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n, 20);
			}
		}
		return xdamagey(magr, mdef, attk, dmg);

	case GOLDEN_WAVE:
		/* needs direct target */
		if (!foundem) {
			impossible("golden wave with no mdef?");
			return MM_MISS;
		}
		else
		{
			if (!Fire_res(mdef) && species_resists_cold(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s burned by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = (d(2, 12) * 3 + 1) / 2;
				if (!InvFire_res(mdef)) {
					destroy_item(mdef, SCROLL_CLASS, AD_FIRE);
					destroy_item(mdef, POTION_CLASS, AD_FIRE);
					destroy_item(mdef, SPBOOK_CLASS, AD_FIRE);
				}
			}
			else if (!Cold_res(mdef) && species_resists_fire(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s frozen by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = (d(2, 12) * 3 + 1) / 2;
				if (!InvCold_res(mdef)) {
					destroy_item(mdef, POTION_CLASS, AD_COLD);
				}
			}
			else if (hates_silver(mdef->data)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s seared by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12) + d(1, 20);
			}
			else if (hates_unblessed_mon(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s seared by concordant light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12) + d(1, 8);
			}
			else if (hates_unholy_mon(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s seared by unholy light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12) + d(1, 9);
			}
			else if (hates_holy(mdef->data)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s seared by holy light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12) + d(1, 7);
			}
			else if (!Fire_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s burned by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12);
				if (!InvFire_res(mdef)) {
					destroy_item(mdef, SCROLL_CLASS, AD_FIRE);
					destroy_item(mdef, POTION_CLASS, AD_FIRE);
					destroy_item(mdef, SPBOOK_CLASS, AD_FIRE);
				}
			}
			else if (!Shock_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s shocked by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12);
				if (!InvShock_res(mdef)) {
					destroy_item(mdef, WAND_CLASS, AD_ELEC);
					destroy_item(mdef, RING_CLASS, AD_ELEC);
				}
			}
			else if (!Cold_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s frozen by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12);
				if (!InvCold_res(mdef)) {
					destroy_item(mdef, POTION_CLASS, AD_COLD);
				}
			}
			else if (!Acid_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s burned by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12);
				if (!InvAcid_res(mdef)) {
					destroy_item(mdef, POTION_CLASS, AD_FIRE);
				}
			}
			else {
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s slashed by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12);
			}
		}
		return xdamagey(magr, mdef, attk, dmg);

	case MON_WARP_THROW:
		/* needs direct target */
		if (!foundem) {
			impossible("warp with no mdef?");
			return MM_MISS;
		}
		else
		{
			int dx, dy;
			dmg = 0;
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("Space warps and flings %s away!",
					youdef ? "you" : mon_nam(mdef));
			}
			do{
				dx = rn2(3) - 1;
				dy = rn2(3) - 1;
			} while(
				!(dx || dy) &&
				!(dx == x(mdef) - x(magr) && dy == y(mdef) - y(magr))
			);
			if(youdef){
				hurtle(dx, dy, BOLT_LIM, FALSE, TRUE);
			}
			else {
				mhurtle(mdef, dx, dy, BOLT_LIM, TRUE);
			}
		}
		return MM_HIT;

	case MON_WARP:
		/* needs direct target */
		if (!foundem) {
			impossible("warp-throw with no mdef?");
			return MM_MISS;
		}
		else
		{
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("Space warps into deadly blades around %s!",
					youdef ? "you" : mon_nam(mdef));
			}

			/* reduce damage */
			if (Half_spel(mdef))
				dmg = (dmg + 1) / 2;
			if (Half_phys(mdef))
				dmg = (dmg + 1) / 2;
			if (youdef && u.uvaul_duration)
				dmg = (dmg + 3) / 4;
			if (dmg > 100)
				dmg = 100;
		}
		return xdamagey(magr, mdef, attk, dmg);

	case DROP_BOULDER:
		if (!youdef) {
			/* only written for vs player*/
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		else {
			struct obj *otmp;
			dmg = 0;
			boolean iron = (!rn2(4) ||
#ifdef REINCARNATION
				Is_rogue_level(&u.uz) ||
#endif
				(In_endgame(&u.uz) && !Is_earthlevel(&u.uz)));
			otmp = mksobj(iron ? HEAVY_IRON_BALL : BOULDER, MKOBJ_NOINIT);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			if (iron) otmp->owt += 160 * rn2(2);

			if (foundem) {
				pline("%s drops out of %s and hits you!", An(xname(otmp)),
					iron ? "nowhere" : the(ceiling(tarx, tary)));
				dmg = dmgval(otmp, &youmonst, 0);
				if (uarmh) {
					if (is_hard(uarmh)) {
						pline("Fortunately, you are wearing a hard helmet.");
						if (dmg > 2) dmg = 2;
					}
					else if (flags.verbose) {
						Your("%s does not protect you.",
							xname(uarmh));
					}
				}
			}
			else {
				if (cansee(tarx, tary))
					pline("%s drops out of %s!", An(xname(otmp)), iron ? "nowhere" : the(ceiling(tarx, tary)));
				dmg = 0;
			}
			if (!flooreffects(otmp, tarx, tary, "fall")) {
				place_object(otmp, tarx, tary);
				stackobj(otmp);
				newsym(tarx, tary);
			}

			if (Half_phys(mdef))
				dmg = (dmg + 1) / 2;
			if (youdef && u.uvaul_duration)
				dmg = (dmg + 1) / 2;
		}
		return xdamagey(magr, mdef, attk, dmg);

//////////////////////////////////////////////////////////////////////////////////////
// AOE OFFENSE
//////////////////////////////////////////////////////////////////////////////////////
	case ACID_BLAST:
	case MON_FIRA:
	case MON_FIRAGA:
	case MON_BLIZZARA:
	case MON_BLIZZAGA:
	case MON_THUNDARA:
	case MON_THUNDAGA:
		if (!(tarx || tary)) {
			impossible("AoE explosion spell with no target location?");
			return MM_MISS;
		}
		else {
			boolean triple = (spell == MON_FIRAGA || spell == MON_BLIZZAGA || spell == MON_THUNDAGA);
			int adtyp;
			int color;
			/* get adtyp, color */
			switch (spell) {
			case ACID_BLAST:
				adtyp = AD_ACID;
				color = EXPL_NOXIOUS;
				break;
			case MON_FIRA:
			case MON_FIRAGA:
				adtyp = AD_FIRE;
				color = EXPL_FIERY;
				break;
			case MON_BLIZZARA:
			case MON_BLIZZAGA:
				adtyp = AD_COLD;
				color = EXPL_FIERY;
				break;
			case MON_THUNDARA:
			case MON_THUNDAGA:
				adtyp = AD_ELEC;
				color = EXPL_MAGICAL;
				break;
			}
			/* cap damage to 60 per explosion (30 for triple) */
			if (dmg > 60)
				dmg = 60;
			if (triple)
				dmg /= 2;

			if (!triple) {
				explode(tarx, tary, adtyp, MON_EXPLODE, dmg, color, 1);
			}
			else {
				explode(tarx + rn2(3) - 1, tary + rn2(3) - 1, adtyp, MON_EXPLODE, dmg, color, 1);
				explode(tarx + rn2(3) - 1, tary + rn2(3) - 1, adtyp, MON_EXPLODE, dmg, color, 1);
				explode(tarx + rn2(3) - 1, tary + rn2(3) - 1, adtyp, MON_EXPLODE, dmg, color, 1);
			}
		}
		return MM_HIT | ((mdef && !youdef && DEADMONSTER(mdef)) ? MM_DEF_DIED : 0);

	case MON_FLARE:
		if (!(tarx || tary)) {
			impossible("flare with no target location?");
			return MM_MISS;
		}
		else {
			if (dmg > 60)
				dmg = 60;
			explode(tarx + rn2(3) - 1, tary + rn2(3) - 1, AD_PHYS, MON_EXPLODE, dmg / 3, EXPL_FROSTY, 1);
			explode(tarx + rn2(3) - 1, tary + rn2(3) - 1, AD_PHYS, MON_EXPLODE, dmg / 3, EXPL_FIERY, 1);
			explode(tarx + rn2(3) - 1, tary + rn2(3) - 1, AD_PHYS, MON_EXPLODE, dmg / 3, EXPL_MUDDY, 1);
			explode(tarx, tary, AD_PHYS, MON_EXPLODE, dmg, EXPL_FROSTY, 2);
			dmg = 0;
		}
		return MM_HIT | ((mdef && !youdef && DEADMONSTER(mdef)) ? MM_DEF_DIED : 0);

	case PRISMATIC_SPRAY:
		if (!(tarx || tary)) {
			impossible("prismatic spray with no target location?");
			return MM_MISS;
		}
		else {
			int dx = 0, dy = 0, typ;
			dmg /= 10;
			if (dmg > 7)
				dmg = 7;
			const int expls[][2] =
			{
				{ AD_PHYS, EXPL_RED },
				{ AD_FIRE, EXPL_FIERY },
				{ AD_DRST, EXPL_YELLOW },
				{ AD_ACID, EXPL_LIME },
				{ AD_COLD, EXPL_BBLUE },
				{ AD_ELEC, EXPL_MAGENTA },
				{ AD_DISN, EXPL_MAGICAL }
			};	/* 7 types of explosions; must match with typ=rn2(7) */
			for (; dmg; dmg--) {
				typ = rn2(7);
				explode(tarx + dx, tary + dy, expls[typ][0], MON_EXPLODE, d(6, 6), expls[typ][1], 2);
				dx = rnd(3) - 2;
				dy = rnd(3) - 2;
				if (!isok(tarx + dx, tary + dy) ||
					IS_STWALL(levl[tarx + dx][tary + dy].typ)
					) {
					/* Spell is reflected back to center */
					dx = 0; dy = 0;
				}
			}
		}

//////////////////////////////////////////////////////////////////////////////////////
// CLOUDS
//////////////////////////////////////////////////////////////////////////////////////
	case MON_POISON_GAS:
		if (!(tarx || tary)) {
			impossible("gas region spell with no target location?");
			return MM_MISS;
		}
		else {
			create_gas_cloud(tarx, tary, rnd(3), rnd(3) + 1, youagr);
			if (youdef)
				stop_occupation();
		}
		return MM_HIT;

	case SOLID_FOG:
		if (!(tarx || tary)) {
			impossible("fog region spell with no target location?");
			return MM_MISS;
		}
		else {
			create_fog_cloud(tarx, tary, 3, 8, youagr);
			if (!youagr && magr->mtyp == PM_PLUMACH_RILMANI)
				magr->mcan = 1;
			if (youdef)
				stop_occupation();
		}
		return MM_HIT;

	case EARTHQUAKE:
		if (!(tarx || tary)) {
			impossible("earthquake spell with no target location?");
			return MM_MISS;
		}
		else {
			/* message */
			pline_The("entire %s is shaking around you!",
				In_endgame(&u.uz) ? "plane" : "dungeon");
			/* Quest nemesis maledictions */
			if (malediction && (!In_endgame(&u.uz) || Is_earthlevel(&u.uz))) {
				if (rn2(2)) verbalize("The earth trembles before my %s!",
					rn2(2) ? "power" : "might");
				else verbalize("Open thy maw, mighty earth!");
			}

			/* only summons creatures if targeting the player */
			do_earthquake(tarx, tary, min((mlev(magr) - 1) / 3 + 1, 24), min((mlev(magr) - 1) / 6 + 1, 8), youdef, magr);

			aggravate(); /* wake up without scaring */
			stop_occupation();	/* even if you weren't targeted, you certainly noticed! */
		}
		return MM_HIT;

//////////////////////////////////////////////////////////////////////////////////////
// HEALING / BUFFING
//////////////////////////////////////////////////////////////////////////////////////
	case CURE_SELF:
		if (*hp(magr) < *hpmax(magr))
		{
			if (youagr) {
				You("feel better.");
				healup(d(3, 6), 0, FALSE, FALSE);
			}
			else {
				if (canseemon(magr))
					pline("%s looks better.", Monnam(magr));
				*hp(magr) += d(dmn, 8);
				if (*hp(magr) > *hpmax(magr))
					*hp(magr) = *hpmax(magr);
			}
		}
		return MM_HIT;

	case MASS_CURE_CLOSE:
	case MASS_CURE_FAR:
		if (TRUE) {
			struct monst *cmon;

			/* maybe retarget spell */
			if ((spell == MASS_CURE_CLOSE)
				|| (tarx == 0 && tary == 0)) {
				tarx = (int)x(magr);
				tary = (int)y(magr);
			}

			/* go through monsters list */
			for (cmon = fmon; cmon; cmon = cmon->nmon){
				if (((cmon != magr) || spell == MASS_CURE_CLOSE) &&	/* curefar doesn't affect self, cureclose does. */
					(cmon->mhp < cmon->mhpmax) &&
					(!DEADMONSTER(cmon)) &&
					(!mm_aggression(magr, cmon)) &&
					(cmon->mpeaceful == (youagr || magr->mpeaceful)) &&
					dist2(tarx, tary, cmon->mx, cmon->my) <= 3 * 3 + 1
					)
				{
					cmon->mhp += d(dmn, 8);
					if (cmon->mhp > cmon->mhpmax)
						cmon->mhp = cmon->mhpmax;
					if (canseemon(cmon))
						pline("%s looks better.", Monnam(cmon));
				}
			}
			/* include player, if yours or tame */
			if ((youagr || magr->mtame)
				&& (!youagr || spell == MASS_CURE_CLOSE)
				&& (*hp(&youmonst) < *hpmax(&youmonst))
				&& dist2(tarx, tary, u.ux, u.uy) <= 3*3 + 1)
			{
				healup(d(dmn, 8), 0, FALSE, FALSE);
				You("feel better.");
			}
		}
		return MM_HIT;

	case RECOVER:
		if (youagr) {
			/* no player version; cast basic healing magic instead */
			return cast_spell(magr, mdef, attk, CURE_SELF, tarx, tary);
		}
		else {
			if (!magr->perminvis) magr->minvis = 0;
			if (magr->permspeed == MSLOW) magr->permspeed = 0;
			magr->mcan = 0;
			magr->mcrazed = 0;
			magr->mdisrobe = 0;
			magr->mcansee = 1;
			magr->mblinded = 0;
			magr->mcanmove = 1;
			magr->mfrozen = 0;
			magr->msleeping = 0;
			magr->mstun = 0;
			magr->mconf = 0;
			if (canseemon(magr))
				pline("%s looks recovered.", Monnam(magr));
		}
		return MM_HIT;

	case HASTE_SELF:
		if (youagr) {
			if (!(HFast & INTRINSIC)) {
				You("are suddenly moving faster.");
				exercise(A_DEX, TRUE);
			}
			HFast |= INTRINSIC;
		}
		else {
			mon_adjust_speed(magr, 1, (struct obj *)0);
		}
		return MM_HIT;

	case MASS_HASTE:
		if (TRUE) {
			struct monst *cmon;

			/* always centered on caster */
			tarx = (int)x(magr);
			tary = (int)y(magr);

			/* go through monsters list */
			for (cmon = fmon; cmon; cmon = cmon->nmon){
				if ((!DEADMONSTER(cmon)) &&
					(cmon->mpeaceful == (youagr || magr->mpeaceful)) &&
					(!mm_aggression(magr, cmon)) &&
					dist2(tarx, tary, cmon->mx, cmon->my) <= 3 * 3 + 1
					)
				{
					mon_adjust_speed(cmon, 1, (struct obj *)0);
				}
			}
			/* include player, if yours or tame */
			if ((youagr || magr->mtame)
				&& dist2(tarx, tary, u.ux, u.uy) <= 3 * 3 + 1)
			{
				/* only temporary haste */
				if (!Very_fast)
					You("are suddenly moving %sfaster.",
					Fast ? "" : "much ");
				else {
					Your("%s get new energy.",
						makeplural(body_part(LEG)));
				}
				exercise(A_DEX, TRUE);
				incr_itimeout(&HFast, rn1(10, 100));
			}
		}
		return MM_HIT;

	case MON_PROTECTION:
		if (TRUE) {
			struct monst *cmon;

			/* maybe retarget spell */
			if (tarx == 0 && tary == 0) {
				tarx = (int)x(magr);
				tary = (int)y(magr);
			}

			/* go through monsters list */
			for (cmon = fmon; cmon; cmon = cmon->nmon){
				if ((cmon != magr) &&	/* does not affect caster */
					(!DEADMONSTER(cmon)) &&
					(cmon->mpeaceful == (youagr || magr->mpeaceful)) &&
					(!mm_aggression(magr, cmon)) &&
					dist2(tarx, tary, cmon->mx, cmon->my) <= 3 * 3 + 1
					)
				{
					int prot = -1 * rnd(dmn);
					cmon->mstdy = max(prot, cmon->mstdy + prot);
					if (canseemon(cmon))
						pline("A shimmering shield surrounds %s!", mon_nam(cmon));
				}
			}
			/* include player, if tame */
			if (!youagr
				&& (magr->mtame)
				&& dist2(tarx, tary, u.ux, u.uy) <= 3 * 3 + 1)
			{
				const char *hgolden = hcolor(NH_GOLDEN);
				if (u.uspellprot)
					pline_The("%s haze around you becomes more dense.",
					hgolden);
				else
					pline_The("%s around you begins to shimmer with %s haze.",
					(Underwater || Is_waterlevel(&u.uz)) ? "water" :
					u.uswallow ? mbodypart(u.ustuck, STOMACH) :
					IS_STWALL(levl[u.ux][u.uy].typ) ? "stone" : "air",
					an(hgolden));
				u.uspellprot = max(rnd(dmn), u.uspellprot);
				u.uspmtime = max(1, u.uspmtime);
				if (!u.usptime)
					u.usptime = u.uspmtime;
				find_ac();
			}
		}
		return MM_HIT;

	case MON_TIME_STOP:
		if (u.summonMonster || youagr) {
			/* only allow one "summoning" spell per turn. This isn't summoning but it's close enough? */
			/* you don't get to cast this one, either */
			return cast_spell(magr, mdef, attk, (foundem ? PSI_BOLT : CURE_SELF), tarx, tary);
		}
		else {
			int extraturns = d(1, 4) + 1, i;
			struct monst *tmpm;
			// if(canseemon(magr))
			// pline("%s blurs with speed!", Monnam(magr));
			// magr->movement += (extraturns)*12;
			// for(tmpm = fmon; tmpm; tmpm = tmpm->nmon){
			// if(tmpm->mtyp == PM_UVUUDAUM && tmpm != magr){
			// tmpm->movement += (extraturns)*12;
			// if(canseemon(tmpm))
			// pline("%s blurs with speed!", Monnam(tmpm));
			// }
			// }
			// u.summonMonster = TRUE;//Not exactly a summoning, but don't stack this too aggressively.
			//Note: 1-4 free turns is too strong.  Just give that much healing instead.
			if (canseemon(magr))
				pline("%s blurs with speed!", Monnam(magr));
			for (i = extraturns; i > 0; i--){
				mon_regen(magr, TRUE);
				timeout_problems(magr);
			}
			for (tmpm = fmon; tmpm; tmpm = tmpm->nmon){
				if (is_uvuudaum(tmpm->data) && tmpm != magr && !DEADMONSTER(tmpm)){
					if (canseemon(tmpm))
						pline("%s blurs with speed!", Monnam(tmpm));
					for (i = extraturns; i > 0; i--){
						mon_regen(tmpm, TRUE);
						timeout_problems(tmpm);
					}
				}
			}
		}
		return MM_HIT;

	case DISAPPEAR:
		if (youagr) {
			if (!(HInvis & (INTRINSIC))) {
				HInvis |= TIMEOUT_INF;
				if (!Blind && !BInvis) self_invis_message();
			}
		}
		else {
			if (!magr->minvis && !magr->invis_blkd) {
				if (canseemon(magr)) {
					pline("%s suddenly %s!", Monnam(magr),
						!See_invisible(x(magr), y(magr)) ? "disappears" : "becomes transparent");
				}
				mon_set_minvis(magr);
				if (malediction && !canspotmon(magr))
					You_hear("%s fiendish laughter all around you.", s_suffix(mon_nam(magr)));
			}
			else
				impossible("no reason for monster to cast disappear spell?");
		}
		return MM_HIT;

//////////////////////////////////////////////////////////////////////////////////////
// SUMMONING
//////////////////////////////////////////////////////////////////////////////////////
	/* NOTE: summoning is by default only mvu. Exceptions made as desired and as balance allows. */
	case SUMMON_SPHERE:
		if (!(tarx || tary)) {
			impossible("summon sphere with no target location");
			return MM_MISS;
		}
		else {
			/* full uvm / mvm / mvu allowed */
			int sphere;
			/* For a change, let's not assume the spheres are together. : ) */
			switch (rn2(3)) {
				case 0: sphere = PM_FLAMING_SPHERE; break;
				case 1: sphere = PM_FREEZING_SPHERE; break;
				case 2: sphere = PM_SHOCKING_SPHERE; break;
			}
			boolean dotame = (youagr || magr->mtame);
			struct monst *mtmp;
			/* try to make a sphere */
			if (!(mvitals[sphere].mvflags & G_GONE && !In_quest(&u.uz))) {
				if ((mtmp = makemon(&mons[sphere], tarx, tary, MM_ADJACENTOK|summonflags)) != 0) {
					/* check if we can see it */
					if (canspotmon(mtmp)) {
						pline("%s is created!",
							Hallucination ? rndmonnam() : Amonnam(mtmp));
					}
					/* maybe tame */
					if (dotame)
						initedog(mtmp);
					mtmp->msleeping = 0;
					set_malign(mtmp);
					/* all spheres are very temporary */
					mark_mon_as_summoned(mtmp, magr, d(dmn, 3)+1, 0);
				}
			}
		}
		return MM_HIT;

	case INSECTS:
		if (!(tarx || tary)) {
			impossible("summon insects with no target location");
			return MM_MISS;
		}
		else if (!(youdef || youagr)) {
			/* only uvm / mvu allowed */
			return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
		}
		else {
			/* Try for insects, and if there are none
			left, go for (sticks to) snakes.  -3. */
			boolean arachnids = (youagr ? Race_if(PM_DROW) : is_drow(magr->data));
			struct permonst *pm = mkclass(arachnids ? S_SPIDER : S_ANT, 0);
			struct monst *mtmp = (struct monst *) 0;
			char let = (pm ? (arachnids ? S_SPIDER : S_ANT) : S_SNAKE);
			boolean created = FALSE;
			boolean dotame = (youagr || magr->mtame);
			int i, quan, oldseen, newseen;
			coord bypos;
			const char *fmt;

			quan = (mlev(magr) < 2) ? 1 : rnd((int)mlev(magr) / 2);
			if (quan < 3)
				quan = 3;

			for (i = 0; i <= quan; i++) {
				/* find a spot suitable for a giant beetle, regardless of what creature will actually be summoned */
				/* slightly less dumb than finding spots suitable for the spellcaster */
				if (!enexto(&bypos, tarx, tary, &mons[PM_GIANT_BEETLE]))
					break;
				if ((pm = mkclass(let, 0)) != 0
					&& (mtmp = makemon(pm, bypos.x, bypos.y, summonflags)) != 0)
				{
					if (canspotmon(mtmp))
						created = TRUE;
					if (dotame) {
						initedog(mtmp);
					}
					mtmp->msleeping = 0;
					/* arbitrarily strengthen enemies in astral and sanctum */
					if (Is_astralevel(&u.uz) || Is_sanctum(&u.uz)) {
						mtmp->m_lev += rn1(3, 3);
						mtmp->mhp = (mtmp->mhpmax += rn1((int)mlev(magr), 20));
					}
					set_malign(mtmp);
					mark_mon_as_summoned(mtmp, magr, ESUMMON_PERMANENT, 0);
				}
			}

			if (!created) {
				pline("%s cast%s at a clump of sticks, but nothing happens.",
					youagr ? "You" : Monnam(magr),
					youagr ? "" : "s");
			}
			else if (let == S_SNAKE) {
				pline("%s transform%s a clump of sticks into snakes!",
					youagr ? "You" : Monnam(magr),
					youagr ? "" : "s");
			}
			else if (youdef && Invisible && !mon_resistance(magr, SEE_INVIS) &&
				(magr->mux != u.ux || magr->muy != u.uy)) {
				pline("%s summons %s around a spot near you!",
					youagr ? "You" : Monnam(magr), let == S_SPIDER ? "arachnids" : "insects");
			}
			else if (youdef && Displaced && (magr->mux != u.ux || magr->muy != u.uy)) {
				pline("%s summons %s around your displaced image!",
					youagr ? "You" : Monnam(magr), let == S_SPIDER ? "arachnids" : "insects");
			}
			else {
				pline("%s summon%s %s!",
					youagr ? "You" : Monnam(magr),
					youagr ? "" : "s",
					let == S_SPIDER ? "arachnids" : "insects");
			}
		}
		return MM_HIT;

	case RAISE_DEAD:
		/* creatures raised are not marked as summoned */
		if (!youdef) {
			/* only mvu allowed */
			return cast_spell(magr, mdef, attk, (foundem ? PSI_BOLT : CURE_SELF), tarx, tary);
		}
		else
		{
			coord mm;
			if (canseemon(magr))
				pline("%s raised the dead!", Monnam(magr));
			mm.x = x(magr);
			mm.y = y(magr);
			mkundead(&mm, TRUE, NO_MINVENT);
			stop_occupation();
		}
		return MM_HIT;

	case SUMMON_MONS:
		if (!youdef || u.summonMonster || (Role_if(PM_ANACHRONONAUT) && In_quest(&u.uz))) {
			/* only mvu allowed */
			/* only one summon spell per global turn allowed */
			/* disallowed in Anachrononaut quest */
			return cast_spell(magr, mdef, attk, (foundem ? PSI_BOLT : CURE_SELF), tarx, tary);
		}
		else
		{
			int count;
			u.summonMonster = TRUE;
			count = nasty(magr);	/* summon something nasty */
			if (magr->iswiz)
				verbalize("Destroy the thief, my pet%s!", plur(count));
			else {
				const char *mappear =
					(count == 1) ? "A monster appears" : "Monsters appear";

				/* messages not quite right if plural monsters created but
				only a single monster is seen */
				if (Invisible && !mon_resistance(magr, SEE_INVIS) &&
					(tarx != u.ux || tary != u.uy))
					pline("%s around a spot near you!", mappear);
				else if (Displaced && (tarx != u.ux || tary != u.uy))
					pline("%s around your displaced image!", mappear);
				else
					pline("%s from nowhere!", mappear);
			}
			stop_occupation();
		}
		return MM_HIT;

	case SUMMON_DEVIL:
		if (!youdef || u.summonMonster || !foundem) {
			/* only mvu allowed */
			/* only one summon spell per global turn allowed */
			/* since it always summons adjacent to player, only allow casting if they've found you */
			return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
		}
		else if (is_alienist(magr->data)) {
			/* alienists summon aliens. wowzers. */
			return cast_spell(magr, mdef, attk, SUMMON_ALIEN, tarx, tary);
		}
		else
		{
			struct monst * mtmp;
			/* summon_minion always appears near the player */
			mtmp = summon_minion(sgn(magr->data->maligntyp), FALSE, TRUE, FALSE);
			if (mtmp) {
				u.summonMonster = TRUE;
				if (canspotmon(mtmp))
					pline("%s ascends from below!",
					An(Hallucination ? rndmonnam() : "fiend"));
				else
					You("sense the arrival of %s.",
					an(Hallucination ? rndmonnam() : "hostile fiend"));
				/* summon_minion marks mtmp as summoned */
			}
			else
				return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
			stop_occupation();
		}
		return MM_HIT;

	case SUMMON_ANGEL:
		if (!youdef || u.summonMonster) {
			/* only mvu allowed */
			/* only one summon spell per global turn allowed */
			return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
		}
		else if (is_alienist(magr->data) || has_template(magr, MAD_TEMPLATE)) {
			/* alienists summon aliens. wowzers. */
			return cast_spell(magr, mdef, attk, SUMMON_ALIEN, tarx, tary);
		}
		else
		{
			struct monst *mtmp;
			/* maybe summon nearby the caster, instead of at a target */
			if (!(tarx || tary) || !clear_path(x(magr), y(magr), tarx, tary)) {
				tarx = x(magr);
				tary = y(magr);
			}
			mtmp = makemon(&mons[PM_ANGEL], tarx, tary, MM_ADJACENTOK | MM_NOCOUNTBIRTH | MM_ESUM);
			if (mtmp) {
				add_mx(mtmp, MX_EMIN);
				mtmp->isminion = TRUE;
				EMIN(mtmp)->min_align = sgn(magr->data->maligntyp);

				u.summonMonster = TRUE;
				if (canspotmon(mtmp))
					pline("%s %s!",
					An(Hallucination ? rndmonnam() : "angel"),
					Is_astralevel(&u.uz) ? "appears near you" :
					"descends from above");
				else
					You("sense the arrival of %s.",
					an(Hallucination ? rndmonnam() : "hostile angel"));
				//Despite being a demon lord, Lamashtu is  able to summon angels
				if(magr->mtyp == PM_LAMASHTU || magr->mfaction == LAMASHTU_FACTION)
					mtmp->mfaction = LAMASHTU_FACTION;
				mark_mon_as_summoned(mtmp, magr, ESUMMON_PERMANENT, 0);
			}
			else
				return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
			stop_occupation();
		}
		return MM_HIT;

	case SUMMON_ALIEN:
		if (!youdef || u.summonMonster) {
			/* only mvu allowed */
			/* only one summon spell per global turn allowed */
			return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
		}
		else {
			struct monst *mtmp;
			int tries = 0;
			static struct permonst *aliens[] = {
				&mons[PM_HOOLOOVOO],
				&mons[PM_SHAMBLING_HORROR],
				&mons[PM_STUMBLING_HORROR],
				&mons[PM_WANDERING_HORROR],
				&mons[PM_MASTER_MIND_FLAYER],
				&mons[PM_EDDERKOP],
				&mons[PM_AOA],
				&mons[PM_HUNTING_HORROR],
				&mons[PM_BYAKHEE],
				&mons[PM_UVUUDAUM] };

			/* maybe summon nearby the caster, instead of at a target */
			if (!(tarx || tary) || !clear_path(x(magr), y(magr), tarx, tary)) {
				tarx = x(magr);
				tary = y(magr);
			}

			do {
				mtmp = makemon(aliens[rn2(SIZE(aliens))], tarx, tary, MM_ADJACENTOK | MM_NOCOUNTBIRTH | MM_ESUM);
			} while (!mtmp && tries++ < 10);
			if (mtmp) {
				u.summonMonster = TRUE;
				if (canspotmon(mtmp))
					pline("The world tears open, and %s steps through!",
					an(Hallucination ? rndmonnam() : "alien"));
				else
					You("sense the arrival of %s.",
					an(Hallucination ? rndmonnam() : "alien"));
				mark_mon_as_summoned(mtmp, magr, ESUMMON_PERMANENT, 0);
			}
			else
				return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
			stop_occupation();
		}
		return MM_HIT;

	case SUMMON_YOUNG:
		if (!youdef || u.summonMonster) {
			/* only mvu allowed */
			/* only one summon spell per global turn allowed */
			return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
		}
		else {
			struct monst *mtmp;
			int tries = 0;
			static struct permonst *young[] = {
				&mons[PM_GIANT_GOAT_SPAWN],
				&mons[PM_SWIRLING_MIST],
				&mons[PM_ICE_STORM],
				&mons[PM_THUNDER_STORM],
				&mons[PM_FIRE_STORM],
				&mons[PM_DEMINYMPH],
				&mons[PM_DARK_YOUNG],
				&mons[PM_DARK_YOUNG],
				&mons[PM_DARK_YOUNG],
				&mons[PM_DARK_YOUNG],
				&mons[PM_BLESSED] };

			/* maybe summon nearby the caster, instead of at a target */
			if (!(tarx || tary) || !clear_path(x(magr), y(magr), tarx, tary)) {
				tarx = x(magr);
				tary = y(magr);
			}

			do {
				mtmp = makemon(young[rn2(SIZE(young))], tarx, tary, MM_ADJACENTOK | MM_NOCOUNTBIRTH | MM_ESUM);
			} while (!mtmp && tries++ < 10);
			if (mtmp) {
				u.summonMonster = TRUE;
				if (canspotmon(mtmp))
					pline("A monster appears in a swirl of mist!");
				else
					You("sense the arrival of a monster!");
				mark_mon_as_summoned(mtmp, magr, ESUMMON_PERMANENT, 0);
			}
			else
				return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
			stop_occupation();
		}
		return MM_HIT;

	case TIME_DUPLICATE:
		if (!youdef || u.summonMonster) {
			/* only mvu allowed */
			/* only one summon spell per global turn allowed */
			return cast_spell(magr, mdef, attk, (foundem ? PSI_BOLT : CURE_SELF), tarx, tary);
		}
		else {
			struct monst *mtmp;

			/* maybe summon nearby the caster, instead of at a target */
			if (!(tarx || tary) || !clear_path(x(magr), y(magr), tarx, tary)) {
				tarx = x(magr);
				tary = y(magr);
			}

			mtmp = makemon(magr->data, tarx, tary, MM_ADJACENTOK | MM_NOCOUNTBIRTH | NO_MINVENT | MM_ESUM);
			if (mtmp){
				u.summonMonster = TRUE;
				mtmp->mclone = 1;
				/* does not stick around long */
				mark_mon_as_summoned(mtmp, magr, d(1, 4) + 1, 0);
			}
		}
		return MM_HIT;

	case CLONE_WIZ:
		/* creature created is not marked as a vanishing summon */
		if (!youdef) {
			/* only mvu allowed */
			return cast_spell(magr, mdef, attk, (foundem ? PSI_BOLT : CURE_SELF), tarx, tary);
		}
		else {
			if (magr->iswiz && flags.no_of_wizards == 1) {
				pline("Double Trouble...");
				clonewiz();
			}
			else {
				impossible("bad wizard cloning?");
			}
		}
		return MM_HIT;

//////////////////////////////////////////////////////////////////////////////////////
// DEBUFFING AND MISC
//////////////////////////////////////////////////////////////////////////////////////
	case MAKE_VISIBLE:
		if (!mdef) {
			impossible("make visible with no target?");
			return MM_MISS;
		}
		else {
			if (youdef) {
				HInvis &= ~INTRINSIC;
				You_feel("paranoid.");
				stop_occupation();
			}
			else {
				if (mdef->minvis){
					mdef->perminvis = 0;
					struct obj* otmp;
					/* not a perfect method to check if mdef gets INVIS from an item */
					for (otmp = mdef->minvent; otmp; otmp = otmp->nobj)
					if (otmp->owornmask && objects[otmp->otyp].oc_oprop == INVIS)
						break;
					if (!otmp) mdef->minvis = 0;
					newsym(x(mdef), y(mdef));
				}
			}
		}
		return MM_HIT;

	case AGGRAVATION:
		if (!mdef) {
			impossible("aggravation with no target?");
			return MM_MISS;
		}
		else {
			if (youdef) {
				You_feel("that monsters are aware of your presence.");
				aggravate();
				stop_occupation();
			}
			else {
				you_aggravate(mdef);
			}
		}
		return MM_HIT;

	case STUN_YOU:
	case CONFUSE_YOU:
	case PARALYZE:
		if (!mdef) {
			impossible("basic debuff spell with no target?");
			return MM_MISS;
		}
		else {
			/* these three spells are very similar, and share their resist checks */
			if (Magic_res(mdef) || (youdef ? Free_action : resist(mdef, 0, 0, FALSE))) {
				shieldeff(x(mdef), y(mdef));

				switch (spell)
				{
				case STUN_YOU:
					if (youdef) {
						if (!Stunned)
							You_feel("momentarily disoriented.");
						if (!Free_action)
							make_stunned(1L, FALSE);
					}
					else if (youagr || canseemon(mdef)) {
						pline("%s seems momentarily disoriented.", Monnam(mdef));
					}
					break;
				case CONFUSE_YOU:
					if (youdef) {
						You_feel("momentarily dizzy.");
					}
					else if (youagr || canseemon(mdef)) {
						pline("%s seems momentarily dizzy.", Monnam(mdef));
					}
					break;
				case PARALYZE:
					if (youdef) {
						if (multi >= 0){
							You("stiffen briefly.");
							if (!Free_action) nomul(-1, "paralyzed by a monster");
						}
					}
					else if (youagr || canseemon(mdef)) {
						pline("%s stiffens briefly.", Monnam(mdef));
					}
					break;
				}
			}
			else {
				switch (spell)
				{
				case STUN_YOU:
					if (youdef) {
						You(Stunned ? "struggle to keep your balance." : "reel...");
						dmg = d(ACURR(A_DEX) < 12 ? 2 : 1, 4);
						if (Half_spell_damage) dmg = (dmg + 1) / 2;
						if (u.uvaul_duration) dmg = (dmg + 1) / 2;
						make_stunned(HStun + dmg, FALSE);
					}
					else {
						if (youagr || canseemon(mdef)) {
							if (mdef->mstun)
								pline("%s struggles to keep %s balance.",
								Monnam(mdef), mhis(mdef));
							else
								pline("%s reels...", Monnam(mdef));
						}
						mdef->mstun = 1;	/* no timer, so Half_spel can't be used */
					}
					break;
				case CONFUSE_YOU:
					if (youdef) {
						boolean oldprop = !!Confusion;

						dmg = rnd(10) + rnd(mlev(magr));
						if (Half_spell_damage) dmg = (dmg + 1) / 2;
						if (u.uvaul_duration) dmg = (dmg + 1) / 2;
						make_confused(HConfusion + dmg, TRUE);
						if (Hallucination)
							You_feel("%s!", oldprop ? "trippier" : "trippy");
						else
							You_feel("%sconfused!", oldprop ? "more " : "");
					}
					else {
						if (youagr || canseemon(mdef)) {
							pline("%s seems %sconfused!", Monnam(mdef),
								mdef->mconf ? "more " : "");
						}
						mdef->mconf = 1;	/* no timer, so Half_spel can't be used */
					}
					break;
				case PARALYZE:
					/* set damage */
					dmg = min(youdef ? rnd(4) : rnd(30), mlev(magr));	/* much less effective vs player */
					if (Half_spel(mdef))
						dmg = (dmg + 1) / 2;
					if (youdef && u.uvaul_duration)
						dmg = (dmg + 1) / 2;
					/* apply with message */
					if (youdef) {
						if (multi >= 0) You("are frozen in place!");
						nomul(-dmg, "paralyzed by a monster");
					}
					else {
						if (youagr || canseemon(mdef)) {
							pline("%s is frozen in place!", Monnam(mdef));
						}
						mdef->mcanmove = 0;
						mdef->mfrozen = dmg;
					}
					break;
				}
			}
			if (youdef)
				stop_occupation();
		}
		return MM_HIT;

	case BLIND_YOU:
		if (!mdef) {
			impossible("blind you with no target?");
			return MM_MISS;
		}
		else {
			/* set damage */
			dmg = rnd(200);
			if (Half_spel(mdef))
				dmg = (dmg + 1) / 2;
			if (youdef && u.uvaul_duration)
				dmg = (dmg + 1) / 2;

			/* note: resist_blnd does not apply here */
			if (youdef) {
				if (!Blinded) {
					int num_eyes = eyecount(youracedata);
					if (attk->adtyp == AD_CLRC)
						pline("Scales cover your %s!",
						(num_eyes == 1) ?
						body_part(EYE) : makeplural(body_part(EYE)));
					else if (Hallucination)
						pline("Oh, bummer!  Everything is dark!  Help!");
					else pline("A cloud of darkness falls upon you.");

					make_blinded((long)dmg, FALSE);
					if (!Blind) Your1(vision_clears);
				}
			}
			else {
				if (!mdef->mblinded && haseyes(mdef->data)) {
					/* message */
					if (youagr || canseemon(mdef)) {
						if (attk->adtyp == AD_CLRC){
							int num_eyes = eyecount(mdef->data);
							pline("Scales cover %s %s!",
								s_suffix(mon_nam(mdef)),
								(num_eyes == 1) ? "eye" : "eyes");
						}
						else {
							pline("%s goes blind!", Monnam(mdef));
						}
					}
					mdef->mblinded = min(dmg, 127);
				}
			}
		}
		return MM_HIT;

	case NIGHTMARE:
		if (!mdef) {
			impossible("nightmare with no target?");
			return MM_MISS;
		}
		else {
			dmg = rnd(mlev(magr));
			if (Magic_res(mdef))
				dmg = (dmg + 1) / 2;
			if (Half_spel(mdef))
				dmg = (dmg + 1) / 2;
			if (youdef && u.uvaul_duration)
				dmg = (dmg + 1) / 2;

			if (youdef) {
				You_hear("%s laugh menacingly as the world blurs around you...", mon_nam(magr));
				make_confused(HConfusion + dmg * 10, FALSE);
				make_stunned(HStun + dmg, FALSE);
				make_hallucinated(HHallucination + dmg * 15, FALSE, 0L);
				stop_occupation();
			}
			else {
				if (youagr || canseemon(mdef)) {
					pline("%s world blurs...", s_suffix(Monnam(mdef)));
				}
				mdef->mconf = 1;
				mdef->mstun = 1;
			}
		}
		return MM_HIT;

	case DRAIN_ENERGY:
		if (!mdef) {
			impossible("nightmare with no target?");
			return MM_MISS;
		}
		else {
			if (Magic_res(mdef) && !(youdef && Race_if(PM_INCANTIFIER))) {
				shieldeff(x(mdef), y(mdef));
				if (youdef)
					You_feel("momentarily lethargic.");
			}
			else {
				if (youdef) {
					drain_en(rn1(u.ulevel, dmg));
				}
				else {
					mdef->mspec_used += dmg;
				}
			}
			stop_occupation();
		}
		return MM_HIT;

	case WEAKEN_YOU:		/* drain strength */
		if (!youdef) {
			/* only makes sense vs player */
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		else {
			if (Fixed_abil) {
				You_feel("momentarily weakened.");
			}
			else if (Magic_res(mdef)) {
				shieldeff(x(mdef), y(mdef));
				if (rn2(2)){
					You_feel("a bit weaker.");
					losestr(1);
					if (u.uhp < 1)
						done_in_by(magr);
				}
				else {
					You_feel("momentarily weakened.");
				}
			}
			else {
				You("suddenly feel weaker!");
				dmg = rnd(2);
				if (Half_spel(mdef))
					dmg = (dmg + 1) / 2;
				if (youdef && u.uvaul_duration)
					dmg = (dmg + 1) / 2;
				losestr(dmg);
				if (u.uhp < 1)
					done_in_by(magr);
			}
			stop_occupation();
		}
		return MM_HIT;

	case WEAKEN_STATS:           /* drain any stat */
		if (!youdef) {
			/* only makes sense vs player */
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		else {
			boolean drained = FALSE;
			if (Fixed_abil) {
				You_feel("momentarily weakened.");
			}
			else if (is_prince(magr->data)) {
				/* drain all stats */
				int typ = 0;
				boolean change = FALSE;
				do {
					if (adjattrib(typ, -rnd(2), -1)) drained = TRUE;
				} while (++typ < A_MAX);
			}
			else {
				/* drain one stat by a lot */
				int typ = rn2(A_MAX);
				dmg = rnd(4);
				if (Half_spel(mdef))
					dmg = (dmg + 1) / 2;
				if (youdef && u.uvaul_duration)
					dmg = (dmg + 1) / 2;
				/* try for a random stat */
				if (adjattrib(typ, -dmg, -1)) {
					/* Quest nemesis maledictions */
					if (malediction)
						verbalize("Thy powers are waning, %s!", plname);
					drained = TRUE;
				}
			}
			if (!drained && !Fixed_abil) { /* if statdrain fails due to min stats, drain max HP a bit */
				You_feel("your life force draining away...");

				if (Half_spel(mdef))
					dmg = (dmg + 1) / 2;
				if (youdef && u.uvaul_duration)
					dmg = (dmg + 1) / 2;
				if (dmg > 20)
					dmg = 20;

				if (Upolyd) {
					u.mh -= dmg;
					u.mhmax -= dmg;
				}
				else {
					u.uhp -= dmg;
					u.uhpmod -= dmg;
					calc_total_maxhp();
				}
				if (u.uhp < 1)
					done_in_by(magr);
				/* Quest nemesis maledictions */
				if (malediction)
					verbalize("Verily, thou art no mightier than the merest newt.");
			}
			stop_occupation();
		}
		return MM_HIT;

	case DESTRY_WEPN:
		if (!mdef) {
			impossible("destroy weapon with no target?");
			return MM_MISS;
		}
		else
		{
			struct obj *otmp = (youdef ? uwep : MON_WEP(mdef));
			const char *hands;
			boolean dofailmsg = FALSE;
			hands = bimanual(otmp, mdef->data) ? makeplural(mbodypart(mdef, HAND)) : mbodypart(mdef, HAND);
			if (otmp && otmp->oclass == WEAPON_CLASS && !Magic_res(mdef) && !otmp->oartifact && rn2(4)) {
				if (otmp->spe > -7){
					otmp->spe -= (youdef ? 1 : rnd(7));	/* it's a bother to you, but not much to monsters */
					if (otmp->spe < -7)
						otmp->spe = -7;

					if (youdef)
						pline("Your %s has been damaged!", xname(otmp));
					else if (canseemon(mdef))
						pline("%s weapon is damaged!", s_suffix(Monnam(mdef)));
				}
				else if (youdef && (rn2(3) && magr->data->maligntyp < 0) && !Hallucination) {
					if (malediction)
						verbalize("%s, your %s broken!", plname, aobjnam(otmp, "are"));
					Your("%s to pieces in your %s!", aobjnam(otmp, "shatter"), hands);
					setuwep((struct obj *)0);
					useup(otmp);
				}
				else {
					if (youdef || youagr || canseemon(mdef)) {
						pline("%s %s shape in %s %s.",
							youdef ? "Your" : s_suffix(Monnam(mdef)),
							aobjnam(otmp, "change"),
							youdef ? "your" : s_suffix(mon_nam(mdef)),
							hands);
					}
					poly_obj(otmp, BANANA);
				}
			}
			else if (otmp && !(youdef ? welded(otmp) : (otmp->cursed && !is_weldproof_mon(mdef) && otmp->otyp != CORPSE))
					&& otmp->otyp != LOADSTONE && (!Magic_res(mdef) || !rn2(4)))
			{
				if (rn2(mlev(magr)) > (youdef ? (ACURRSTR) : rn2(mlev(mdef)))) {
					if (youdef) {
						Your("%s knocked out of your %s!",
							aobjnam(otmp, "are"), hands);
						setuwep((struct obj *)0);
						dropx(otmp);
					}
					else {
						if (youagr || canseemon(mdef)) {
							pline("%s %s knocked out of %s %s!",
								s_suffix(Monnam(mdef)),
								aobjnam(otmp, "are"),
								s_suffix(mon_nam(mdef)),
								hands
								);
						}
						/* unwield their weapon and drop it */
						obj_extract_self(otmp);
						mdrop_obj(mdef, otmp, FALSE);
					}
				}
				else dofailmsg = TRUE;
			}
			else dofailmsg = TRUE; 

			if (dofailmsg) {
				if (youdef || canseemon(mdef)) {
					if (otmp) {
						pline("%s %s for a moment.",
							youdef ? "Your" : s_suffix(Monnam(mdef)),
							aobjnam(otmp, "shudder"));
					}
					else {
						pline("%s %s shudder for a moment.",
							youdef ? "Your" : s_suffix(Monnam(mdef)),
							makeplural(mbodypart(mdef, HAND))
							);
					}
				}
			}
			stop_occupation();
		}
		return MM_HIT;

	case DESTRY_ARMR:
		if (!mdef) {
			impossible("destroy armor with no target?");
			return MM_MISS;
		}
		else {
			struct obj *smarm;
			if (Magic_res(mdef)) {
				shieldeff(x(mdef), y(mdef));
				if (youagr || youdef || canseemon(mdef)) {
					pline("A field of force surrounds %s!",
						youdef ? "you" : mon_nam(mdef));
				}
			}
			else if ((smarm = some_armor(mdef)) == (struct obj *)0) {
				if (youdef) {
					Your("skin itches.");
				}
				else if (canseemon(mdef)) {
					pline("%s looks itchy.",
						Monnam(mdef));
				}
			}
			else if (!oresist_disintegration(smarm)){
				if (smarm->spe <= -1 * a_acdr(objects[smarm->otyp])) {
					youdef ? destroy_arm(smarm) : destroy_marm(mdef, smarm);
				}
				else {
					dmg = rnd(4);
					if (Half_spel(mdef))
						dmg = (dmg + 1) / 2;
					if (youdef && u.uvaul_duration)
						dmg = (dmg + 1) / 2;

					smarm->spe -= dmg;
					if (smarm->spe < -1 * a_acdr(objects[smarm->otyp]))
						smarm->spe = -1 * a_acdr(objects[smarm->otyp]);
					if (youdef || canseemon(mdef)) {
						pline("A field of force surrounds %s %s!",
							youdef ? "your" : s_suffix(mon_nam(mdef)),
							xname(smarm));
					}
				}
				if (malediction) {
					if (rn2(2)) verbalize("Thy defenses are useless!");
					else verbalize("Thou might as well be naked!");
				}
			}
			stop_occupation();
		}
		return MM_HIT;
	case VULNERABILITY:
		if (TRUE) {
			struct monst *cmon;

			/* maybe retarget spell */
			if (tarx == 0 && tary == 0) {
				tarx = (int)x(magr);
				tary = (int)y(magr);
			}

			/* go through monsters list */
			for (cmon = fmon; cmon; cmon = cmon->nmon){
				if ((cmon != magr) &&
					(!DEADMONSTER(cmon)) &&
					(cmon->mpeaceful != (youagr || magr->mpeaceful)) &&	/* targets hostiles, not friendlies */
					dist2(tarx, tary, cmon->mx, cmon->my) <= 3 * 3 + 1
					)
				{
					dmg = rnd(dmn);
					if (Half_spel(mdef))
						dmg = (dmg + 1) / 2;
					if (Magic_res(mdef))
						dmg = (dmg + 1) / 2;
					/* study */
					cmon->mstdy = min(dmg, cmon->mstdy + dmg);
				}
			}
			/* include player, if hostile */
			if (!(youagr || magr->mtame)
				&& dist2(tarx, tary, u.ux, u.uy) <= 3 * 3 + 1)
			{
				dmg = rnd(dmn);
				if (Half_spel(mdef))
					dmg = (dmg + 1) / 2;
				if (Magic_res(mdef))
					dmg = (dmg + 1) / 2;

				u.ustdy = min(dmg, u.ustdy + dmg);
				You_feel("vulnerable!");
			}
		}
		return MM_HIT;

	case EVIL_EYE:
		if (!youdef) {
			/* only makes sense vs player */
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		else {
			struct attack evilEye = { AT_GAZE, AD_LUCK, 1, 4 };
			(void)xgazey(magr, mdef, &evilEye, -1);
		}
		return MM_HIT;

	case CURSE_ITEMS:
		if (!mdef) {
			impossible("curse items with no target?");
			return MM_MISS;
		}
		else if (dist2(x(magr), y(magr), tarx, tary) <= 24) {
			if (youdef) {
				if (rndcurse())
					You_feel("as if you need some help.");
				stop_occupation();
			}
			else {
				if (mrndcurse(mdef) && (youagr || canseemon(mdef)))
					You_feel("as though %s needs some help.", mon_nam(mdef));
			}
		}
		return MM_HIT;

	case NAIL_TO_THE_SKY:
		if (!youdef) {
			/* only makes sense vs player */
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		else
		{
			HLevitation &= ~I_SPECIAL;
			if (!Levitation) {
				/* kludge to ensure proper operation of float_up() */
				HLevitation = 1;
				float_up();
				/* reverse kludge */
				HLevitation = 0;
				if (!Is_waterlevel(&u.uz)) {
					if ((u.ux != xupstair || u.uy != yupstair)
						&& (u.ux != sstairs.sx || u.uy != sstairs.sy || !sstairs.up)
						&& (!xupladder || u.ux != xupladder || u.uy != yupladder)
						) {
						You("hit your %s on the %s.",
							body_part(HEAD),
							ceiling(u.ux, u.uy));
						losehp(uarmh ? 1 : rnd(10),
							"colliding with the ceiling",
							KILLED_BY);
					}
					else (void)doup();
				}
			}
			incr_itimeout(&HLevitation, (d(1, 4) + 1) * 100);
			spoteffects(FALSE);	/* for sinks */
		}
		return MM_HIT;

	case STERILITY_CURSE:
		if (youdef && !HSterile && !Drain_resistance) {
			/* only works vs player, and should fall through to drain life */
			You_feel("old!");
			HSterile |= TIMEOUT_INF;
		}
		else {
			return cast_spell(magr, mdef, attk, DRAIN_LIFE, tarx, tary);
		}
		return MM_HIT;

	case PUNISH:
		if (!youdef) {
			/* only makes sense vs player */
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		else
		{
			if (u.ualign.record <= 1 || !rn2(min(u.ualign.record, 20))){
				if (!Punished) {
					punish((struct obj *)0);
					if (is_prince(magr->data)) uball->owt += 160;
				}
				else {
					Your("iron ball gets heavier!");
					if (is_prince(magr->data)) uball->owt += 240;
					else uball->owt += 160;
				}
			}
			else Your("sins do not demand punishment.");
			stop_occupation();
		}
		return MM_HIT;

	case DARKNESS:
		if (!youdef) {
			/* only targets player */
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		else {
			litroom(FALSE, (struct obj *)0);
			stop_occupation();
		}
		return MM_HIT;

	case MAKE_WEB:
		if (!youdef) {
			/* only written vs player */
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		else
		{
			struct trap * ttmp;
			if ((ttmp = maketrap(u.ux, u.uy, WEB))) {
				You("become entangled in hundreds of %s!",
					Hallucination ? "two-minute noodles" : "thick cobwebs");
				dotrap(ttmp, NOWEBMSG);
				newsym(u.ux, u.uy);
				/* Quest nemesis maledictions */
				if (malediction) {
					if (rn2(ACURR(A_STR)) > 15) verbalize("Thou art dressed like a meal for %s!",
						rn2(2) ? "Ungoliant" : "Arachne");
					else verbalize("Struggle all you might, but it will get thee nowhere.");
				}
				stop_occupation();
			}
		}
		return MM_HIT;
	}
	impossible("end of cast_spell reached");
	return MM_MISS;
}

/* spells that do not appear to have a target when cast, and may be cast with none of (mdef, tarx, tary) */
boolean
is_undirected_spell(spellnum)
int spellnum;
{
	if ((is_buff_spell(spellnum) && !(
		spellnum == MASS_CURE_FAR ||
		spellnum == MON_PROTECTION
		)) ||
		spellnum == RAISE_DEAD ||
		spellnum == TIME_DUPLICATE ||
		spellnum == CLONE_WIZ
		)
		return TRUE;
	return FALSE;
}

/* Directed attack spells need a target (mdef), and by extension need to see its location (tarx, tary) */
/* they cannot be used against warded creatures */
boolean
is_directed_attack_spell(spellnum)
int spellnum;
{
	switch (spellnum)
	{
	case PSI_BOLT:
	case OPEN_WOUNDS:
	case MAGIC_MISSILE:
	case CONE_OF_COLD:
	case LIGHTNING_BOLT:
	case SLEEP:
	case DRAIN_LIFE:
	case ARROW_RAIN:
	case LIGHTNING:
	case FIRE_PILLAR:
	case GEYSER:
	case ACID_RAIN:
	case HAIL_FLURY:
	case ICE_STORM:
	case DEATH_TOUCH:
	case PLAGUE:
	case FILTH:
	case TURN_TO_STONE:
	case STRANGLE:
	case SILVER_RAYS:
	case GOLDEN_WAVE:
	case MON_WARP:
	case MON_WARP_THROW:
	case DROP_BOULDER:
	case DISINT_RAY:
		return TRUE;
	default:
		break;
	}
	return FALSE;
}
/* AOE attack spells only need a location (tarx, tary) */
/* they can be used against warded creatures */
boolean
is_aoe_attack_spell(spellnum)
int spellnum;
{
	switch (spellnum)
	{
	case ACID_BLAST:
	case MON_FIRA:
	case MON_FIRAGA:
	case MON_BLIZZARA:
	case MON_BLIZZAGA:
	case MON_THUNDARA:
	case MON_THUNDAGA:
	case MON_FLARE:
	case PRISMATIC_SPRAY:
	case MON_POISON_GAS:
	case SOLID_FOG:
	case EARTHQUAKE:
	/* also directed attack spells */
	case MAGIC_MISSILE:
	case CONE_OF_COLD:
	case LIGHTNING_BOLT:
	case SLEEP:
	case DISINT_RAY:
		return TRUE;
	default:
		break;
	}
	return FALSE;
}
/* Buff spells need neither a target nor a location */
/* some of them may use a location, even if it's not strictly necessary */
boolean
is_buff_spell(spellnum)
int spellnum;
{
	switch (spellnum)
	{
	case CURE_SELF:
	case MASS_CURE_CLOSE:
	case MASS_CURE_FAR:
	case RECOVER:
	case HASTE_SELF:
	case MASS_HASTE:
	case MON_PROTECTION:
	case MON_TIME_STOP:
	case DISAPPEAR:
		return TRUE;
	default:
		break;
	}
	return FALSE;
}
/* Summon spells need a target (mdef) and a location (tarx, tary) */
/* they can be used against warded creatures */
/* almost all summoning spells may only be used against the player */
/* creatures are summoned at the targeted location */
boolean
is_summon_spell(spellnum)
int spellnum;
{
	switch (spellnum)
	{
	case SUMMON_SPHERE:
	case INSECTS:
	case RAISE_DEAD:
	case SUMMON_MONS:
	case SUMMON_DEVIL:
	case SUMMON_ANGEL:
	case SUMMON_ALIEN:
	case SUMMON_YOUNG:
	case TIME_DUPLICATE:
	case CLONE_WIZ:
		return TRUE;
	default:
		break;
	}
	return FALSE;
}
/* Debuff spells need a target (mdef), and by extension need to see its location (tarx, tary) */
/* several debuff spells only work against the player */
/* they cannot be used against warded creatures */
boolean
is_debuff_spell(spellnum)
int spellnum;
{
	switch (spellnum)
	{
	case MAKE_VISIBLE:
	case AGGRAVATION:
	case STUN_YOU:
	case CONFUSE_YOU:
	case PARALYZE:
	case BLIND_YOU:
	case NIGHTMARE:
	case DRAIN_ENERGY:
	case WEAKEN_YOU:
	case WEAKEN_STATS:
	case DESTRY_WEPN:
	case DESTRY_ARMR:
	case VULNERABILITY:
	case EVIL_EYE:
	case CURSE_ITEMS:
	case NAIL_TO_THE_SKY:
	case STERILITY_CURSE:
	case PUNISH:
	case DARKNESS:
	case MAKE_WEB:
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

/* Note that spells that are not implemented for all of uvm/mvm/mvu handle those cases gracefully in cast_spell */
boolean
spell_would_be_useless(magr, mdef, spellnum, tarx, tary)
struct monst * magr;
struct monst * mdef;
int spellnum;
int tarx;
int tary;
{
	boolean youagr = (magr == &youmonst);
	boolean youdef = (mdef == &youmonst);
	int wardAt = ward_at(tarx, tary);
	struct monst *tmpm;
	/* Most spells need a target */
	boolean notarget = (!mdef || (!tarx && !tary));
	if (notarget) {
		/* most spells need a target */
		if (!is_undirected_spell(spellnum) && !is_buff_spell(spellnum)) {
			return TRUE;
		}
		else {
			/* undirected spells, or directed buff spells with no target, are instead self-targeted */
			tarx = x(magr);
			tary = y(magr);
		}
	}

	/* Some spells work with a valid line of sight */
	boolean clearpath = clear_path(x(magr), y(magr), tarx, tary);
	/* Some spells need a clear line of fire */
	boolean clearline = m_online(magr, mdef, tarx, tary, (magr->mtame && !magr->mconf), TRUE);

//////////////////////////////////////////////////////////////////////////////////////
// PART 1:  SPELLS SHOULD NEVER BE CAST IN THESE CASES 
//////////////////////////////////////////////////////////////////////////////////////

	/* only undirected spells may be cast with no clear line to the target */
	if (!clearpath && !is_undirected_spell(spellnum))
		return TRUE;

	/* Don't cast directed attack or debuff spells at warded spaces */
	if (!youagr
		&& (is_directed_attack_spell(spellnum) || is_debuff_spell(spellnum))
		&& onscary(tarx, tary, magr))
		return TRUE;

	/* Drow casters respect Elbereth/Lolth */
	if (!youagr
		&& is_drow(magr->data)
		&& (is_directed_attack_spell(spellnum) || is_debuff_spell(spellnum))	/* only affects directed and debuff spells */
		&& !(Role_if(PM_ANACHRONONAUT) && In_quest(&u.uz))) /* does not work in Ana quest */
	{
		if ((sengr_at("Elbereth", tarx, tary) && !Race_if(PM_DROW))
			|| (sengr_at("Lolth", tarx, tary) && Race_if(PM_DROW) && (mlev(magr) < u.ulevel || u.ualign.record-- > 0)))
		{
			return TRUE;
		}
	}

	/* Don't cast summon spells (with some exceptions) in the Anachrononaut quest */
	if ((Role_if(PM_ANACHRONONAUT) && In_quest(&u.uz)) && is_summon_spell(spellnum) && !(
		(spellnum == SUMMON_SPHERE) ||
		(spellnum == TIME_DUPLICATE) ||
		(spellnum == CLONE_WIZ)
		))
		return TRUE;

	/* only the wiz makes a clone, and only one clone at that */
	if (spellnum == CLONE_WIZ
		&& (youagr || !magr->iswiz || flags.no_of_wizards > 1))
		return TRUE;

	/* angels can't be summoned in Gehennom */
	if (spellnum == SUMMON_ANGEL
		&& (In_hell(&u.uz)))
		return TRUE;

	/* aggravate monsters won't be cast by peaceful monsters */
	if (spellnum == AGGRAVATION
		&& (!youagr && magr->mpeaceful))
		return TRUE;

	/* earthquake should not be cast in the endgame (even for the plane of earth?) */
	if (spellnum == EARTHQUAKE && In_endgame(&u.uz))
		return TRUE;

	/* don't do strangulation if there's no room in player's inventory */
	if (spellnum == STRANGLE && 
		(youdef && inv_cnt() == 52 && (!uamul || uamul->oartifact || uamul->otyp == AMULET_OF_YENDOR)))
		return TRUE;

	/* don't try to make webs in bad spots */
	if (spellnum == MAKE_WEB &&
		(levl[tarx][tary].typ <= IRONBARS || levl[tarx][tary].typ > ICE || t_at(tarx, tary)))
		return TRUE;

//////////////////////////////////////////////////////////////////////////////////////
// PART 2:  SPELLS ARE INEFFECTIVE IN THESE CASES, AND SHOULD NOT BE CAST
//////////////////////////////////////////////////////////////////////////////////////

	/* ray attack when monster isn't lined up */
	if ((spellnum == MAGIC_MISSILE || spellnum == SLEEP || spellnum == CONE_OF_COLD || spellnum == LIGHTNING_BOLT || spellnum == DISINT_RAY)
		&& !clearline)
		return TRUE;

	/* don't cast drain life, death touch if not in melee range */
	if ((spellnum == DRAIN_LIFE || spellnum == DEATH_TOUCH)
		&& !(dist2(x(magr), y(magr), tarx, tary) <= 2))
		return TRUE;

	/* don't cast curse items from too far away */
	if (spellnum == CURSE_ITEMS
		&& !(dist2(x(magr), y(magr), tarx, tary) <= 24))
		return TRUE;

	/* don't cast haste self when already fast */
	if (spellnum == HASTE_SELF
		&& (youagr ? (HFast&(INTRINSIC)) : (magr->permspeed == MFAST)))
		return TRUE;

	/* don't cast invisibility when already invisible */
	if (spellnum == DISAPPEAR
		&& (youagr ? (HInvis&(INTRINSIC)) : (magr->minvis || magr->invis_blkd)))
		return TRUE;
	/* peaceful monsters won't cast invisibility if you can't see invisible,
	 * same as when monsters drink potions of invisibility.  This doesn't
	 * really make a lot of sense, but lets the player avoid hitting
	 * peaceful monsters by mistake */
	if (spellnum == DISAPPEAR 
		&& (!youagr && magr->mpeaceful && !See_invisible(u.ux, u.uy)))
		return TRUE;

	/* don't cast healing when already healed */
	if (spellnum == CURE_SELF
		&& (*hp(magr) == *hpmax(magr)))
		return TRUE;

	/* don't cast recovery when stats are ok */
	if (spellnum == RECOVER
		&& !youagr && !(magr->mcan || magr->mcrazed || !magr->mcansee || !magr->mcanmove ||
		magr->msleeping || magr->mstun || magr->mconf || magr->permspeed == MSLOW))
		return TRUE;

	/* don't cast mass healing with no injured allies */
	if ((spellnum == MASS_CURE_CLOSE || spellnum == MASS_CURE_FAR))
	{
		boolean friendly = (youagr || magr->mtame);
		boolean peaceful = (!friendly && magr->mpeaceful);

		/* heal you? */
		if (friendly
			&& (*hp(&youmonst) < *hpmax(&youmonst))
			&& (dist2(tarx, tary, u.ux, u.uy) <= 3 * 3 + 1))
			return FALSE;
		
		/* heal allies? */
		for (tmpm = fmon; tmpm; tmpm = tmpm->nmon){
			if (((friendly && tmpm->mtame) || (peaceful == tmpm->mpeaceful && !tmpm->mtame && !friendly))
				&& (tmpm != magr)
				&& (dist2(tarx, tary, tmpm->mx, tmpm->my) <= 3 * 3 + 1)
				&& (youagr || !mm_aggression(magr, tmpm))
				&& (*hp(tmpm) < *hpmax(tmpm)))
				return FALSE;
		}
		/* heal self? */
		if (spellnum == MASS_CURE_CLOSE
			&& (*hp(magr) < *hpmax(magr)))
			return FALSE;

		return TRUE;
	}

	/* don't cast mass protection/haste without provocation */
	if (spellnum == MON_PROTECTION || spellnum == MASS_HASTE) {
		/* you: always */
		if (youagr)
			return FALSE;
		/* all: when injured */
		if (*hp(magr) < *hpmax(magr))
			return FALSE;
		/* tame: if you're injured and nearby */
		if (magr->mtame
			&& (*hp(&youmonst) < *hpmax(&youmonst))
			&& (dist2(tarx, tary, magr->mx, magr->my) <= 3 * 3 + 1))
			return FALSE;
		/* all: if nearby ally injured, or enemy near */
		for (tmpm = fmon; tmpm; tmpm = tmpm->nmon){
			if (magr->mtame == tmpm->mtame || magr->mpeaceful == tmpm->mpeaceful){
				if (!mm_aggression(magr, tmpm)
					&& (*hp(tmpm) < *hpmax(tmpm))
					&& dist2(magr->mx, magr->my, tmpm->mx, tmpm->my) <= 3 * 3 + 1
					) return FALSE;
			}
			else if (magr->mtame != tmpm->mtame
				&& magr->mpeaceful != tmpm->mpeaceful
				&& distmin(magr->mx, magr->my, tmpm->mx, tmpm->my) <= 5)
				return FALSE;
		}
		return TRUE;
	}

	/* don't summon if... */
	if (is_summon_spell(spellnum) && (
		(!youagr && !magr->mtame && magr->mpeaceful) || 		/* caster is peaceful */
		(!youagr && magr->summonpwr >= magr->data->mlevel) || 	/* already summoned lots of things */
		(u.summonMonster)										/* something's been summoned this turn already */
		))
		return TRUE;

	/* only cast earthquake on found target, too annoying when spammed */
	if (spellnum == EARTHQUAKE && !(tarx == x(mdef) && tary == y(mdef)))
		return TRUE;

	/* the wiz won't use the following cleric-specific or otherwise weak spells */
	if (!youagr && magr->iswiz && (
		spellnum == SUMMON_SPHERE || spellnum == DARKNESS ||
		spellnum == PUNISH || spellnum == INSECTS ||
		spellnum == SUMMON_ANGEL || spellnum == DROP_BOULDER ||
		spellnum == DISINT_RAY
		))
		return TRUE;

//////////////////////////////////////////////////////////////////////////////////////
// PART 3:  SPELLS ARE INEFFECTIVE AGAINST MDEF IN THESE CASES, AND CASTERS WILL SOMETIMES CAST THEM ANYWAYS
//////////////////////////////////////////////////////////////////////////////////////

	/* if the spell is a buff spell, and therfore can have no target, we can cast it -- all further checks depend on defender */
	if (is_buff_spell(spellnum))
		return FALSE;
	
	/* if we don't have a defender, we definitely can't cast it now */
	if (!mdef)
		return TRUE;

	/* maybe cast the spell now, ignoring the checks in part 3 */
	/* dependent on magr's level */
	int skiplev = 10;
	int loglev = 0;
	int lcnt = mlev(magr);
	if (youagr || is_prince(magr->data) || magr->iswiz)
		skiplev -= 2;
	while (lcnt) {
		loglev++;
		lcnt /= 2;
	}
	if (loglev > rn2(skiplev))
		return FALSE;

	/* Death Touch cannot affect creatures on a Circle of Acheron */
	if ((wardAt == CIRCLE_OF_ACHERON || wardAt == HEPTAGRAM || wardAt == HEXAGRAM) && (
		spellnum == DEATH_TOUCH
		))
		return TRUE;
	/* Magic Resistance */
	if (Magic_res(mdef) && (
		(spellnum == MAGIC_MISSILE || spellnum == DESTRY_ARMR ||
		spellnum == PARALYZE || spellnum == CONFUSE_YOU ||
		spellnum == STUN_YOU || spellnum == DRAIN_ENERGY)
		))
		return TRUE;
	/* Reflection */
	if ((youdef ? Reflecting : mon_resistance(mdef, REFLECTING)) && (
		spellnum == MAGIC_MISSILE || spellnum == SLEEP ||
		spellnum == CONE_OF_COLD || spellnum == LIGHTNING ||
		spellnum == LIGHTNING_BOLT
		))
		return TRUE;
	/* Elemental Resistances */
	if (Fire_res(mdef) && (
		spellnum == FIRE_PILLAR
		))
		return TRUE;
	if (Cold_res(mdef) && (
		spellnum == CONE_OF_COLD
		))
		return TRUE;
	if (Shock_res(mdef) && (
		spellnum == LIGHTNING || spellnum == LIGHTNING_BOLT
		))
		return TRUE;
	if (Acid_res(mdef) && (
		spellnum == ACID_RAIN
		))
		return TRUE;
	if (Drain_res(mdef) && (
		spellnum == DRAIN_LIFE
		))
		return TRUE;
	if (Sick_res(mdef) && (
		spellnum == PLAGUE
		))
		return TRUE;
	/* resistant, or currently afflicted */
	if ((Sleep_res(mdef) || (youdef ? Sleeping : mdef->msleeping)) && (
		spellnum == SLEEP
		))
		return TRUE;
	if ((Stone_res(mdef) || (youdef && Stoned)) && (
		spellnum == TURN_TO_STONE
		))
		return TRUE;
	if (youdef && (Hallucination || Halluc_resistance) && (
		spellnum == NIGHTMARE
		))
		return TRUE;
	if (youdef && Strangled && (
		spellnum == STRANGLE
		))
		return TRUE;
	if (youdef ? Blinded : mdef->mblinded && (
		spellnum == BLIND_YOU
		))
		return TRUE;
	/* Free action */
	if (youdef && Free_action && (
		(spellnum == STUN_YOU || spellnum == PARALYZE)
		))
		return TRUE;
	/* Fixed abilites */
	if (youdef && Fixed_abil && (
		spellnum == WEAKEN_YOU || spellnum == WEAKEN_STATS
		))
		return TRUE;
	/* Don't de-stone the player */
	if (youdef && (Stoned || Golded) && (
		spellnum == ACID_RAIN
		))
		return TRUE;
	/* make visible spell against visible creature */
	if (!(youdef ? (HInvis&INTRINSIC) : mdef->minvis) && (
		spellnum == MAKE_VISIBLE
		))
		return TRUE;
	/* don't destroy weapon if not wielding anything */
	if (!(youdef ? uwep : MON_WEP(mdef)) && (
		spellnum == DESTRY_WEPN
		))
		return TRUE;
	/* don't destroy armor if target has no armor */
	if (!some_armor(mdef) && (
		spellnum == DESTRY_ARMR
		))
		return TRUE;
	/* make visible spell by spellcaster with see invisible. */
	if ((youagr ? See_invisible_old : mon_resistance(magr, SEE_INVIS)) && (
		spellnum == MAKE_VISIBLE
		))
		return TRUE;
	/* various conditions where webs won't be effective */
	if ((amorphous(mdef->data) || is_whirly(mdef->data) || flaming(mdef->data) || unsolid(mdef->data) ||
		(youdef && uwep && uwep->oartifact == ART_STING) || (youdef ? (ACURR(A_STR) >= 18) : strongmonst(mdef->data))) && (
		spellnum == MAKE_WEB
		))
		return TRUE;
	/* Drow aren't hindered by darkness */
	if (is_drow(mdef->data) && (
		spellnum == DARKNESS
		))
		return TRUE;
	/* don't summon spheres when all types are gone */
	if (spellnum == SUMMON_SPHERE && !In_quest(&u.uz) &&
		((mvitals[PM_FLAMING_SPHERE].mvflags & G_GONE) &&
		(mvitals[PM_FREEZING_SPHERE].mvflags & G_GONE) &&
		(mvitals[PM_SHOCKING_SPHERE].mvflags & G_GONE)))
		return TRUE;

	return FALSE;
}

#endif /* OVLB */
#ifdef OVL0
int
needs_familiar(mon)
struct monst *mon;
{
	struct monst *mtmp;
	if(!is_witch_mon(mon))
		return FALSE;
	for(mtmp = fmon; mtmp; mtmp = mtmp->nmon)
		if(mtmp->mtyp == PM_WITCH_S_FAMILIAR && mtmp->mvar_witchID == (long)mon->m_id)
			return FALSE;
	return TRUE;
}

#endif /* OVL0 */

/*mcastu.c*/
