/*	SCCS Id: @(#)mcastu.c	3.4	2003/01/08	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "xhity.h"

extern void you_aggravate(struct monst *);

STATIC_DCL int FDECL(choose_magic_spell, (int,int,boolean));
STATIC_DCL int FDECL(choose_clerical_spell, (int,int,boolean,boolean));
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
choose_clerical_spell(spellnum,mid,hostile,quake)
int spellnum;
int mid;
boolean hostile;
boolean quake;
{
	/* Alternative spell lists:
		Alternate list slection is based on the monster's ID number, which is
		annotated as staying constant. Priests are divided up into constructive and
		destructive casters (constructives favor heal self, destructives favor 
		inflict wounds, constructives summon angels, destructives summon devils). 
		Their spell list is divided into blocks. The order that
		they recieve spells from each block is radomized based on their monster
		ID.
	*/
	 spellnum = spellnum % 18;
	//case "17"
	if(spellnum == ((mid/100+3)%4)+14) return PUNISH;
	//case "16"
	if(spellnum == ((mid/100+2)%4)+14) return (mid % 2) ? SUMMON_ANGEL : SUMMON_DEVIL;
	//case "15"
	//Cure/Inflict
	//case "14"
	if(spellnum == ((mid/100+0)%4)+14) return PLAGUE;
	//case "13"
	if(spellnum == ((mid+4)%5)+9) return quake ? EARTHQUAKE : TREMOR;
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

/* default spell selection for psychic-flavored casters */
STATIC_OVL int
choose_psionic_spell(spellnum,mid,hostile)
int spellnum;
int mid;
boolean hostile;
{
    switch (spellnum) {
     case 18:
     case 17:
       return MON_WARP;
     case 16:
     case 14:
	return PAIN_BOLT;
    case 13:
    case 12:
	return VULNERABILITY;
    case 11:
    case 9:
	return !hostile ? PSI_BOLT : NIGHTMARE;
    case 8:
    case 7:
	return mid%3 ? PARALYZE : BARF_BOLT;
    case 6:
    case 4:
	return !hostile ? PSI_BOLT : mid%5 ? STUN_YOU : SAN_BOLT;
    case 3:
    case 2:
	return !hostile ? PSI_BOLT : mid%7 ? CONFUSE_YOU : BABBLE_BOLT;
    case 1:
    case 0:
    default:/*5,10,15,19+*/
	return PSI_BOLT;
    }
}

/* ...but first, check for monster-specific spells */
STATIC_OVL int
choose_magic_special(mtmp, type)
struct monst *mtmp;
unsigned int type;
{
	int clrc_spell_power;
	int wzrd_spell_power;
	if(mtmp->m_id == 0){
		clrc_spell_power = rn2(u.ulevel) * 18 / 30;
		wzrd_spell_power = rn2(u.ulevel) * 24 / 30;
	}
	else {
		if(mtmp->mtemplate == YITH){
			clrc_spell_power = wzrd_spell_power = rn2(45);
		}
		else {
			clrc_spell_power = wzrd_spell_power = rn2(mtmp->m_lev);
		}
	}
	boolean quake = FALSE;
	if(has_template(mtmp, PSURLON)){
		if(rn2(2))
			return CRUSH_BOLT;
	}
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
       case PM_IKSH_NA_DEVA:
           return MOTHER_S_GAZE;
	
       case PM_GRAND_MASTER:
       case PM_MASTER_KAEN:
          return (!rn2(3) ? MON_AURA_BOLT : rn2(2) ? WEAKEN_YOU : EARTHQUAKE);

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
		break;
		case PM_ITINERANT_PRIESTESS:
			if(has_template(mtmp, MISTWEAVER)){
				return !rn2(7) ? HYPNOTIC_COLORS : MON_RED_WORD;
			}
		break;
		case PM_TWIN_SIBLING:
			switch(rn2(4)){
				case 0:
				if(check_mutation(YOG_GAZE_1)){
					return MADF_BURST;
				}
				break;
				case 1:
				if(check_mutation(YOG_GAZE_2)){
					return MADF_BURST;
				}
				break;
				case 2:
				if(check_mutation(MIND_STEALER)){
					return CRUSH_BOLT;
				}
				break;
				case 3:
				if(check_mutation(SHUB_CLAWS)){
					return MON_WARP;
				}
				break;
			}
		break;
       }
    }//50% favored spells
	
	//100% favored spells
	switch(monsndx(mtmp->data)) {
	case PM_HOUND_OF_TINDALOS:
		return OPEN_WOUNDS;
	break;
	case PM_PRIEST_OF_IB:
		switch (rnd((phase_of_the_moon() == 3 || phase_of_the_moon() == 5) ? 7 : 6)) {
			//Uses fire when the moon is gibbous
			case 7:
			return FIRE_PILLAR;
			break;
			default://5, 6
			return BARF_BOLT;
			break;
			case 4:
			return OPEN_WOUNDS;
			break;
			case 3:
			return GEYSER;
			break;
			case 2:
			return ACID_RAIN;
			break;
			case 1:
			return PLAGUE;
			break;
		}
	break;
	case PM_TWIN_SIBLING:
	switch(wzrd_spell_power){
		case 30:
		case 29:
			return TURN_TO_STONE;
			break;
		case 28:
		case 27:
			return DEATH_TOUCH;
			break;
		case 26:
		case 25:
			return LIGHTNING;
			break;
		case 24:
		case 23:
		case 22:
			return DESTRY_ARMR;
			break;
		case 21:
		case 20:
		case 19:
			return DRAIN_ENERGY;
			break;
		case 18:
		case 17:
		case 16:
			return MON_POISON_GAS;
			break;
		case 15:
		case 14:
		case 13:
			if(!(HFast&INTRINSIC)){
				return HASTE_SELF;
				break;
			}
		case 12:
		case 11:
		case 10:
			return CURE_SELF;
		default:
			return check_mutation(TWIN_MIND) ? BARF_BOLT : PSI_BOLT;
			break;
	}
	break;
	case PM_ITINERANT_PRIESTESS:
		if(straitjacketed_mon(mtmp) && has_template(mtmp, MISTWEAVER))
			return  0;
	case PM_PRIEST:
	case PM_PRIESTESS:
	case PM_ALIGNED_PRIEST:
	case PM_HIGH_PRIEST:
	case PM_ARCH_PRIEST:
		quake = !mtmp->mtame; //Casts earthquake instead of tremor
	break;
	case PM_BLIBDOOLPOOLP__GRAVEN_INTO_FLESH:
		switch(rn2(6)){
			case 0:
				return GEYSER;
			break;
			case 1:
				return STEAM_GEYSER;
			break;
			case 2:
				return RAIN;
			break;
			case 3:
				return ACID_RAIN;
			break;
			case 4:
				return BLOOD_RAIN;
			break;
			case 5:
				return FILTH;
			break;
		}
	break;
	case PM_STRANGER:
		switch (clrc_spell_power % 18) {
			case 17:
				return INCARCERATE;
			case 16:
				return SUMMON_ALIEN;
			// case 15:
				//Cure
			case 14:
				return PLAGUE;
			case 13:
				return EARTHQUAKE;
			case 12:
				return rn2(2) ? GEYSER : ACID_RAIN;
			case 11:
				return rn2(2) ? FIRE_PILLAR : ICE_STORM;
			// case 10:
				//Cure
			case 9:
				return LIGHTNING;
			case 8:
				return DRAIN_LIFE;
			case 7:
				return MUMMY_CURSE;
			case 6:
				return YELLOW_DEAD;
			// case 5:
				//Cure
			case 4:
				return NIGHTMARE;
			case 3:
				return VULNERABILITY;
			case 2:
				return STUN_YOU;
			case 1:
				return OPEN_WOUNDS;
			//case "0", "5", "10", "15"
			default:
				return CURE_SELF;
		}
	break;
	case PM_SUZERAIN:
		switch (clrc_spell_power % 18) {
			case 17:
				return ARROW_RAIN;
			case 16:
				return SUMMON_ALIEN;
			// case 15:
				//Cure
			case 14:
				return PLAGUE;
			case 13:
				return EARTHQUAKE;
			case 12:
				return rn2(2) ? GEYSER : ACID_RAIN;
			case 11:
				return TURN_TO_STONE;
			// case 10:
				//Cure
			case 9:
				return DROP_BOULDER;
			case 8:
				return DRAIN_LIFE;
			case 7:
				return MUMMY_CURSE;
			case 6:
				return YELLOW_DEAD;
			// case 5:
				//Cure
			case 4:
				return NIGHTMARE;
			case 3:
				return VULNERABILITY;
			case 2:
				return PAIN_BOLT;
			case 1:
				return OPEN_WOUNDS;
			//case "0", "5", "10", "15"
			default:
				return CURE_SELF;
		}
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
	case PM_SHADOWSMITH:
		switch(clrc_spell_power){
			default:
				return OPEN_WOUNDS;
			break;
			case 0:
			case 2:
			case 4:
				return BLIND_YOU;
			break;
			case 1:
			case 3:
			case 5:
			case 7:
				return DARKNESS;
			break;
			case 20:
			case 22:
			case 24:
				return DRAIN_LIFE;
			break;
			case 29:
				return DEATH_TOUCH;
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
		switch (clrc_spell_power) {
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
			return DISINTEGRATION;
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
			return DISINTEGRATION;
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
       case PM_HEALER:
		switch(clrc_spell_power/2){
			case 14:
				return RECOVER;
			case 16:
			case 13:
				return MASS_CURE_FAR;
			case 12:
				return PARALYZE;
			case 11:
				return SLEEP;
			case 10:
				return DESTRY_ARMR;
			case 9:
				return DESTRY_WEPN;
			case 15:
			case 8:
			case 7:
			case 6:
				return MASS_CURE_CLOSE;
			default:
				return CURE_SELF;
		}
	   break;
       case PM_PANAKEIAN_ARCHON:
		switch(clrc_spell_power/2){
			case 14:
				return RECOVER;
			case 16:
			case 13:
			case 12:
			case 11:
				return MASS_CURE_FAR;
			case 10:
			case 9:
				return PARALYZE;
			case 15:
			case 8:
			case 7:
			case 6:
				return MASS_CURE_CLOSE;
			case 5:
			case 4:
				return SLEEP;
			case 3:
				return DESTRY_ARMR;
			case 2:
				return DESTRY_WEPN;
			case 1:
				return MAKE_WEB;
			default:
				return CURE_SELF;
		}
	   break;
       case PM_HYGIEIAN_ARCHON:
		switch(wzrd_spell_power/5){
			case 8:
			case 7:
				return DEATH_TOUCH;
			case 6:
				return LIGHTNING;
			case 5:
				return GEYSER;
			case 4:
				return ACID_RAIN;
			case 3:
				return CURE_SELF;
			default:
				return FIRE_PILLAR;
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
	case PM_IKSH_NA_DEVA:
		switch(clrc_spell_power/2){
			case 14:
				return RECOVER;
			case 16:
			case 13:
				return MASS_CURE_FAR;
			case 12:
				return PARALYZE;
			case 11:
				return SLEEP;
			case 10:
				return DESTRY_ARMR;
			case 9:
				return DESTRY_WEPN;
			case 15:
			case 8:
			case 7:
			case 6:
				return MASS_CURE_CLOSE;
			default:
				return CURE_SELF;
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
	case PM_WATERSPOUT:
		return GEYSER;
	break;
	case PM_CAILLEA_ELADRIN:
			switch(rnd(8)){
				default:
				case 1: return ICE_STORM;
				case 2: return EVIL_EYE;
				case 3: return RAISE_DEAD;
				case 4: return MASS_CURE_CLOSE;
				case 5: return DEATH_TOUCH;
				case 6: return DRAIN_LIFE;
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
	case PM_GLASYA:
	case PM_COUATL:
		switch(rn2(3)){
			case 0:
				return choose_clerical_spell(clrc_spell_power, mtmp->m_id,!(mtmp->mpeaceful), quake);
			case 1:
				return choose_psionic_spell(clrc_spell_power, mtmp->m_id,!(mtmp->mpeaceful));
			case 2:
				return choose_magic_spell(wzrd_spell_power,mtmp->m_id,!(mtmp->mpeaceful));
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
	case PM_DOOM_KNIGHT:
		switch(rn2(4)){
			case 0: return BLIND_YOU;
			case 1: return VULNERABILITY;
			case 2: return MON_FLARE;
			case 3: return DRAIN_LIFE;
		}
	break;
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
			if(mtmp->mvar_spList_2 > 3) mtmp->mvar_spList_2 = 0;
			switch(mtmp->mvar_spList_2++){
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
				case 1: return MON_THUNDAGA;
				case 2: return !PURIFIED_EARTH ? DEATH_TOUCH : WEAKEN_STATS;
				case 3: return CURE_SELF;
				case 4: return MON_FIRAGA;
				case 5: return !PURIFIED_EARTH ? MON_WARP : ICE_STORM;
				case 6: return HASTE_SELF;
				case 7: return MON_FLARE;
			}
		} else {
			if(mtmp->mvar_spList_2 > 3) mtmp->mvar_spList_2 = 0;
			switch(mtmp->mvar_spList_2++){
				case 0: return !PURIFIED_EARTH ? MON_FLARE : EARTH_CRACK;
				case 1: return (!PURIFIED_FIRE && rn2(2)) ? STUN_YOU : FIRE_PILLAR;
				case 2: return !PURIFIED_WATER ? ICE_STORM : GEYSER;
				case 3: return !PURIFIED_WIND ? PLAGUE : MON_POISON_GAS;
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
	   case PM_DROW_ALIENIST:
			switch (rnd(8)) {
				case 8:
				return MAKE_WEB;
				break;
				case 7:
				return SUMMON_ALIEN;
				break;
				case 6:
				return DESTRY_WEPN;
				break;
				case 5:
				return DESTRY_ARMR;
				break;
				case 4:
				return BLIND_YOU;
				break;
				case 3:
				return PARALYZE;
				break;
				case 2:
				return MON_CANCEL;
				break;
				case 1:
				return PSI_BOLT;
				break;
			}
	   break;
       case PM_ANULO:
       case PM_ANULO_DANCER:
			return rn2(2) ? choose_clerical_spell(clrc_spell_power, mtmp->m_id,!(mtmp->mpeaceful), quake) 
						  : choose_magic_spell(wzrd_spell_power,mtmp->m_id,!(mtmp->mpeaceful));
	   break;
       case PM_PEN_A_MENDICANT:
       case PM_MENDICANT_SPROW:
       case PM_MENDICANT_DRIDER:
       case PM_SISTER_T_EIRASTRA:
			switch (rnd(8)) {
				case 8:
				return (!quest_status.offered_artifact ? CURE_SELF : RECOVER);
				break;
				case 7:
				return (!quest_status.offered_artifact ? CURE_SELF : MASS_CURE_CLOSE);
				break;
				case 6:
				return DESTRY_WEPN;
				break;
				case 5:
				return DESTRY_ARMR;
				break;
				case 4:
				return BLIND_YOU;
				break;
				case 3:
				return PARALYZE;
				break;
				case 2:
				return (!quest_status.offered_artifact ? CURSE_ITEMS : EVIL_EYE);
				break;
				case 1:
				return OPEN_WOUNDS;
				break;
			}
       case PM_SHUUSHAR_THE_ENLIGHTENED:
          return (!quest_status.offered_artifact ? CURE_SELF : MASS_CURE_CLOSE);
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
				case 1: return DOUBT_BOLT;
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
		switch(rnd(10)){
			case 1: return OPEN_WOUNDS;
			case 2: return SAN_BOLT;
			case 3: return ICE_STORM;
			case 4: return ACID_RAIN;
			case 5: return GEYSER;
			case 6: return SUMMON_YOUNG;
			case 7: return MASS_CURE_CLOSE;
			case 8: return LIGHTNING;
			case 9: return rn2(10) ? GOD_RAY : DISINTEGRATION;
			case 10: return MON_RED_WORD;
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
		switch(rn2(6)){
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
			case 5:
				return rn2(6) ? SUMMON_DEVIL : SUMMON_TANNIN;
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
        return choose_clerical_spell(clrc_spell_power, mtmp->m_id,!(mtmp->mpeaceful), quake);
    else if (type == AD_PSON)
        return choose_psionic_spell(clrc_spell_power, mtmp->m_id,!(mtmp->mpeaceful));
    return choose_magic_spell(wzrd_spell_power,mtmp->m_id,!(mtmp->mpeaceful));
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
	"SLIMIFY",
	//50
	"NIGHTMARE",
	"FILTH",
	"CLONE_WIZ",
	"STRANGLE",
	"MON_FIRA",
	//55
	"MON_FIRAGA",
	"MON_BLIZZARA",
	"MON_BLIZZAGA",
	"MON_THUNDARA",
	"MON_THUNDAGA",
	//60
	"MON_FLARE",
	"MON_WARP",
	"MON_POISON_GAS",
	"MON_PROTECTION",
	"SOLID_FOG",
	//65
	"ACID_BLAST",
	"PRISMATIC_SPRAY",
	"SILVER_RAYS",
	"GOLDEN_WAVE",
	"VULNERABILITY",
	//70
	"MASS_HASTE",
	"MON_TIME_STOP",
	"TIME_DUPLICATE",
	"NAIL_TO_THE_SKY",
	"STERILITY_CURSE"
	//75
	"DISINT_RAY"
	"MON_WARP_THROW",
	"SUMMON_TANNIN",
	"DISINTEGRATION",
	"TREMOR",
	//80
	"INCARCERATE",
	"MUMMY_CURSE",
	"YELLOW_DEAD",
	"MON_CANCEL",
	"STARFALL",
	//85
	"EARTH_CRACK",
	"AURA_BOLT",
	"RAIN",
	"BLOOD_RAIN",
	"STEAM_GEYSER",
	//90
	"MOTHER_S_GAZE",
	"PAIN_BOLT",
	"SAN_BOLT",
	"DOUBT_BOLT",
	"BARF_BOLT",
	//95
	"BABBLE_BOLT",
	"MON_SPE_BEARTRAP",
	"PYRO_STORM",
	"GOD_RAY",
	"MON_RED_WORD",
	//100
	"HYPNOTIC_COLORS",
	"CRUSH_BOLT",
	"MADF_BURST",
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
	int spell_skill = 0;
	char buf[BUFSZ];

	/* things that block monster spells from even being attempted */
	if (cantmove(magr))
		return MM_MISS;
	if (youagr ? Nullmagic : mon_resistance(magr, NULLMAGIC))
		return MM_MISS;
	if (Deadmagic && attk->adtyp != AD_PSON && attk->adtyp != AD_CLRC)
		return MM_MISS;
	if (Catapsi && attk->adtyp == AD_PSON)
		return MM_MISS;
	if (Misotheism && attk->adtyp == AD_CLRC)
		return MM_MISS;
	if (attk->adtyp == AD_PSON && !youdef && (!mdef || mindless_mon(mdef)))
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
		(magr->mtyp == PM_SHADOWSMITH && dimness(magr->mx,magr->my) <= 0) ||
		(magr->mspec_used && !nospellcooldowns_mon(magr)) ||
		(mlev(magr) == 0) ||
		(youdef && Invulnerable) ||
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
					&& (mtmp->mtyp == PM_MYRKALFR || mtmp->mtyp == PM_MYRKALFAR_WARRIOR || mtmp->mtyp == PM_MYRKALFAR_MATRON 
						|| (is_drow(mtmp->data) && mtmp->mtame && magr->mtame))
				) magr->mspec_used /= 2;
			}
			if(magr->mtame)
				magr->mspec_used /= 2;
		}
		else {
			magr->mspec_used = 10 - magr->m_lev;
			if (magr->mspec_used < 2) magr->mspec_used = 2;
		}
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
			if(attk->adtyp == AD_PSON){
				pline("%s concentrate%s.",
					youagr ? "You" : canspotmon(magr) ? Monnam(magr) : "Something",
					youagr ? "" : "s");
			}
			else {
				pline("%s cast%s a spell at %s!",
					youagr ? "You" : canspotmon(magr) ? Monnam(magr) : "Something",
					youagr ? "" : "s",
					levl[tarx][tary].typ == WATER
					? "empty water" : "thin air");
			}
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
	if(level.flags.has_minor_spire)
		chance += 2;

	/*This.... may never be reached :( */
	/* There is code is getattk() that prevents lilitus from attempting to cast vs. invalid targets */
	if(pa->mtyp == PM_LILITU && attk->adtyp == AD_CLRC){
		if(youdef && !Doubt)
			force_fail = TRUE;
		else if(!youdef && !(mdef && mdef->mdoubt))
			force_fail = TRUE;
	}
	
	spell_skill = mlev(magr) * 2;
	if(youagr){
		int delta = NightmareAware_Insanity;
		if(u.umadness&MAD_RAGE && !BlockableClearThoughts){
			spell_skill = spell_skill*delta/100;
		}
		if((u.umadness&MAD_FORMICATION || u.umadness&MAD_SCORPIONS) && !BlockableClearThoughts){
			spell_skill = spell_skill*(delta/2)/100;
		}
		if(u.umadness&MAD_SCIAPHILIA && !BlockableClearThoughts && unshadowed_square(u.ux, u.uy)){
			spell_skill = spell_skill*delta/100;
		}
		if(u.umadness&MAD_NUDIST && !BlockableClearThoughts && NightmareAware_Sanity < 100){
			int discomfort = u_clothing_discomfort();
			if (discomfort) {
				spell_skill = spell_skill*((discomfort * delta)/10)/100;
			} else {
				if (!uwep && !uarms) {
					//Bonus, not reduced by ClearThoughts+Nightmare combo
					spell_skill += Insanity/10;
				}
			}
		}
	}
	else {
		if(magr->mformication || magr->mscorpions)
			spell_skill /= 2;
		if(magr->msciaphilia && unshadowed_square(x(magr), y(magr)))
			spell_skill /= 2;
		if(magr->mnudist && magr->misc_worn_check)
			spell_skill /= 2;
	}
	
	/* failure chance determined, check if attack fumbles */
	if (force_fail 
		|| rn2(spell_skill) < chance
		|| (magr->mtoobig && magr->m_lev < rnd(100))
		|| (magr->msciaphilia && magr->m_lev < rnd(100) && unshadowed_square(magr->mx, magr->my))
	) {
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
		if ((youagr || (youdef && !is_undirected_spell(spellnum) && cansee(tarx, tary)) || canspotmon(magr))
			&& magr->mtyp != PM_HOUND_OF_TINDALOS
			&& spellnum != MOTHER_S_GAZE
			&& spellnum != MON_RED_WORD
			&& spellnum != HYPNOTIC_COLORS
		) {
			if(attk->adtyp == AD_PSON){
				pline("%s concentrate%s.",
					youagr ? "You" : canspotmon(magr) ? Monnam(magr) : "Something",
					youagr ? "" : "s");
			}
			else {
				if (is_undirected_spell(spellnum) || notarget || (!foundem && distmin(x(mdef), y(mdef), tarx, tary) > 2))
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
	}
	
	if(youdef && ublindf && ublindf->oartifact == ART_MIRRORED_MASK && rn2(2)){
		Your("mask shifts and reflects the spell!");
		youdef = FALSE;
		mdef = magr;
		tarx = mdef->mx;
		tary = mdef->my;
	}
	
	/* do spell */
	if (spellnum) {
		/* special case override: the avatar of lolth can ask Lolth to intercede instead of casting a spell */
		if (youdef && magr->mtyp == PM_AVATAR_OF_LOLTH && (urole.cgod == GOD_LOLTH) && !is_undirected_spell(spellnum) && !magr->mpeaceful){
			godlist[GOD_LOLTH].anger++;
			angrygods(GOD_LOLTH);
			result = MM_HIT;
		}
		else /* generally: cast the spell */
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
	else if (foundem && dist2(x(magr), y(magr), tarx, tary) <= 2) {
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
			if ((youagr || youdef || canspotmon(magr)) 
				&& magr->mtyp != PM_HOUND_OF_TINDALOS
			) {
				if(attk->adtyp == AD_PSON){
					pline("%s concentrate%s.",
						youagr ? "You" : canspotmon(magr) ? Monnam(magr) : "Something",
						youagr ? "" : "s");
				}
				else {
					pline("%s cast%s a spell at %s!",
						youagr ? "You" : canseemon(magr) ? Monnam(magr) : "Something",
						youagr ? "" : "s",
						levl[tarx][tary].typ == WATER
						? "empty water" : "thin air");
				}
			}
			return MM_MISS;
		}
		/* otherwise, print a spellcasting message */
		else {
			if ((youagr || youdef || canspotmon(magr)) && magr->mtyp != PM_HOUND_OF_TINDALOS) {
				if(attk->adtyp == AD_PSON){
					pline("%s concentrate%s.",
						youagr ? "You" : canspotmon(magr) ? Monnam(magr) : "Something",
						youagr ? "" : "s");
				}
				else {
					pline("%s cast%s a spell at %s!",
						youagr ? "You" : canspotmon(magr) ? Monnam(magr) : "Something",
						youagr ? "" : "s",
						youdef ? "you" : (canspotmon(mdef) ? mon_nam(mdef) : "something"));
				}
			}
		}

		/* possibly override die size */
		if (attk->damd)
			dmd = (int)(attk->damd);
		/* increase die size */
		if (!youagr && is_alabaster_mummy(magr->data) && magr->mvar_syllable == SYLLABLE_OF_POWER__KRAU)
			dmd *= 1.5;
		if(adtyp == AD_MADF && (youagr || magr->mtyp == PM_TWIN_SIBLING)){
			dmn = 6 + P_SKILL(P_ATTACK_SPELL);
			dmd = spiritDsize();
			// zapdata.bonus += Insanity/10;
		}
		/* calculate damage */
		dmg = d(dmn, dmd);

		if(adtyp == AD_MADF && (youagr || magr->mtyp == PM_TWIN_SIBLING)){
			dmg += Insanity/10;
		}

		/* apply damage reductions */
		dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);
		
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
			if (!UseInvShock_res(mdef)){
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
			if (!UseInvFire_res(mdef)){
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

		case AD_MADF:
			/* message */
			if (youdef || canspotmon(mdef)) {
				pline("%s%s enveloped in magenta flames.",
					youdef ? "You" : Monnam(mdef),
					youdef ? "'re" : " is"
					);
			}
			/* do effect */
			if (Fire_res(mdef) && Magic_res(mdef)) {
				if (youdef || canseemon(mdef)) {
					shieldeff(tarx, tary);
					if (youdef) pline("But you resist the effects.");
					else pline("But %s resists the effects.", mhe(mdef));
				}
				dmg = 0;
			}
			else if(Fire_res(mdef)){
				dmg -= dmg/2;
			}
			/* damage inventory */
			if (!UseInvFire_res(mdef)){
				destroy_item(mdef, POTION_CLASS, AD_FIRE);
				if (!rn2(6)) destroy_item(mdef, SCROLL_CLASS, AD_FIRE);
				if (!rn2(10)) destroy_item(mdef, SPBOOK_CLASS, AD_FIRE);
			}
			/* other effects */
			if (youdef) {
				burn_away_slime();
				melt_frozen_air();
			}
			//Share madness
			if(magr == mdef); //You can't share your madness with yourself
			else if(youdef){
				if(!save_vs_sanloss()){
					change_usanity(-1*d(3,6), TRUE);
				}
			}
			else if(youagr || magr->mtyp == PM_TWIN_SIBLING){
				if(!mindless_mon(mdef) && (mon_resistance(mdef,TELEPAT) || tp_sensemon(mdef) || !rn2(5)) && roll_generic_madness(FALSE)){
					//reset seen madnesses
					mdef->seenmadnesses = 0L;
					you_inflict_madness(mdef);
				}
			}
			else {
				if(!mindless_mon(mdef) && (mon_resistance(mdef,TELEPAT) || !rn2(5))){
					if(!resist(mdef, '\0', 0, FALSE))
						mdef->mcrazed = TRUE;
				}
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
			if (!UseInvCold_res(mdef)){
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

		case AD_HOLY:
			/* message */
			if (youdef || canspotmon(mdef)) {
				pline("%s %s hit by a shower of holy missiles!",
					youdef ? "You" : Monnam(mdef),
					youdef ? "are" : "is"
					);
			}
			if(hates_holy_mon(mdef))
				dmg *= 2;
			else if(hates_unholy_mon(mdef))
				dmg /= 3;
			return xdamagey(magr, mdef, attk, dmg);

		case AD_UNHY:
			/* message */
			if (youdef || canspotmon(mdef)) {
				pline("%s %s hit by a shower of unholy missiles!",
					youdef ? "You" : Monnam(mdef),
					youdef ? "are" : "is"
					);
			}
			if(hates_unholy_mon(mdef))
				dmg *= 2;
			else if(hates_holy_mon(mdef))
				dmg /= 3;
			return xdamagey(magr, mdef, attk, dmg);

		case AD_HLUH:
			/* message */
			if (youdef || canspotmon(mdef)) {
				pline("%s %s hit by a shower of corrupted missiles!",
					youdef ? "You" : Monnam(mdef),
					youdef ? "are" : "is"
					);
			}
			if(hates_unholy_mon(mdef) && hates_holy_mon(mdef))
				dmg *= 3;
			else if(hates_unholy_mon(mdef) || hates_holy_mon(mdef))
				dmg *= 2;
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
			
			dmg = reduce_dmg(mdef,dmg,TRUE,FALSE);

			if (dmg > 0) {
				int i;
				/* reduce by DR */
				for (i = dmn / 3; i > 0; i--) {
					dmg -= (youdef ? roll_udr(magr, attk->aatyp) : roll_mdr(mdef, magr, attk->aatyp));
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
			if (youdef && foundem) {
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
					if (mdef && foundem) {
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
			if (adtyp == AD_MADF) {
				if(youagr || magr->mtyp == PM_TWIN_SIBLING){
					zapdata.damn = 6 + P_SKILL(P_ATTACK_SPELL);
					zapdata.damd = spiritDsize();
					zapdata.bonus += Insanity/10;
				}
				zapdata.no_bounce = TRUE;
			}
			if (adtyp == AD_HOLY || adtyp == AD_UNHY || adtyp == AD_HLUH) {
				zapdata.affects_floor = FALSE;
				zapdata.phase_armor = TRUE;
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
	if (spell_would_be_useless(magr, mdef, spell, tarx, tary))
		return MM_MISS;

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
		if ((!youdef && mindless_mon(mdef)) || Catapsi) {
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

	case PAIN_BOLT:
	case SAN_BOLT:
	case DOUBT_BOLT:
	case BARF_BOLT:
	case BABBLE_BOLT:
	case CRUSH_BOLT:
		/* needs direct target */
		if (!foundem) {
			impossible("No mdef for mind-bolt");
			return MM_MISS;
		}
		if ((!youdef && mindless_mon(mdef)) || Catapsi) {
			return MM_MISS;
		}
		dmg = (dmg + 1) / 2;
		if (dmg > 75)
			dmg = 75;

		/* calculate resistance */
		if ((!youdef && resist(mdef, 0, 0, FALSE))) {
			shieldeff(x(mdef), y(mdef));
			dmg = (dmg + 1) / 2;
		}
		if(Half_spel(mdef))
			dmg = (dmg + 1) / 2;
		/* message */
		if (youdef) {
			if (dmg <= 5)
				You("get a %sache.", body_part(HEAD));
			else if (dmg <= 10)
				Your("brain is on fire!");
			else if (dmg <= 20)
				Your("%s suddenly aches painfully!", body_part(HEAD));
			else{
				Your("%s suddenly aches very painfully!", body_part(HEAD));
			}
		}
		if(spell == PAIN_BOLT){
			if (youdef) {
				if (!is_silent(mdef->data)){
					You("%s from the pain!", humanoid_torso(mdef->data) ? "scream" : "shriek");
				}
				else {
					You("writhe in pain!");
				}
				HScreaming += (dmg+9)/10;
			}
			else {
				if (!is_silent_mon(mdef)){
					if (canseemon(mdef))
						pline("%s %s in pain!", Monnam(mdef), humanoid_torso(mdef->data) ? "screams" : "shrieks");
					else You_hear("%s %s in pain!", mdef->mtame ? noit_mon_nam(mdef) : mon_nam(mdef), humanoid_torso(mdef->data) ? "screaming" : "shrieking");
				}
				else {
					if (canseemon(mdef))
						pline("%s writhes in pain!", Monnam(mdef));
				}
				mdef->movement -= 6+(dmg+9)/10;
			}
		}
		else if(spell == SAN_BOLT){
			if (youdef) {
				pline("Your buried fears surface!");
				//Note: San damage is *increased* by your drunkard score
				//  Finally a case where you SHOULD lay off the sauce!
				if(save_vs_sanloss())
					change_usanity(-1*((dmg+u.udrunken)/20), FALSE);
				else
					change_usanity(-1*((dmg+u.udrunken)/10), TRUE);
			}
			else {
				if (canseemon(mdef))
					pline("%s staggers!", Monnam(mdef));
				mdef->mconf = TRUE;
				if(rn2(dmg) > mdef->m_lev)
					mdef->mcrazed = TRUE;
			}
		}
		else if(spell == DOUBT_BOLT){
			if (youdef) {
				pline("You suffer a violent conflict of faith!");
				HDoubt += dmg;
			}
			else {
				if (canseemon(mdef))
					pline("%s falters!", Monnam(mdef));
				mdef->mdoubt = TRUE;
				if(rn2(dmg) > mdef->m_lev)
					mdef->mapostasy = TRUE;
			}
		}
		else if(spell == BARF_BOLT){
			if (youdef) {
				pline("Revolting sensations assail you!");
				if(!Vomiting && !(inediate(youracedata) && !uandroid && !Race_if(PM_INCANTIFIER)))
					make_vomiting(Vomiting+15+d(5,4), TRUE);
			}
			else {
				if (canseemon(mdef))
					pline("%s shakes %s %s!", Monnam(mdef), mhis(mdef), mbodypart(mdef, HEAD));
				mdef->movement -= (dmg+19)/20;
				if(rn2(dmg) > mdef->m_lev/3 && !(inediate(mdef->data) && !is_android(mdef->data) && mdef->mtyp != PM_INCANTIFIER)){
					if(canseemon(mdef))
						pline("%s vomits!", Monnam(mdef));
					mdef->mcanmove = 0;
					if ((mdef->mfrozen + 3) > 127)
						mdef->mfrozen = 127;
					else mdef->mfrozen += 3;
				}
			}
		}
		else if(spell == BABBLE_BOLT){
			if (youdef) {
				pline("Your thoughts fade, and you babble in confusion!");
				if(!Fixed_abil && ACURR(A_INT) > 3){
					adjattrib(A_INT, -1, FALSE);
					exercise(A_WIS, FALSE);
					exercise(A_CHA, FALSE);
				}
				damage_spells(dmg/10);
				HBabble += dmg;
			}
			else {
				if (canseemon(mdef))
					pline("%s falters!", Monnam(mdef));
				mdef->mspec_used += dmg;
				mdef->mconf = TRUE;
			}
		}
		else if(spell == CRUSH_BOLT){
			if (youdef) {
				int stat = ACURR(A_INT) <= ACURR(A_CHA) ? A_CHA : A_INT;
				stat = ACURR(stat) <= ACURR(A_WIS) ? A_WIS : stat;
				pline("Your mind is being crushed!");
				if(!Fixed_abil){
					adjattrib(stat, -1, FALSE);
					exercise(A_INT, FALSE);
					exercise(A_WIS, FALSE);
					exercise(A_CHA, FALSE);
					check_brainlessness();
				}
				if(save_vs_sanloss())
					change_usanity(-1*(25-ACURR(stat))/2, FALSE);
				else
					change_usanity(-1*(25-ACURR(stat)), TRUE);
				if(stat == A_INT)
					damage_spells(dmg/10);
				else if(stat == A_WIS)
					drain_en(dmg);
				else if(stat == A_CHA)
					HBabble += dmg;
			}
			else {
				if (canseemon(mdef))
					pline("%s falters!", Monnam(mdef));
				mdef->mspec_used += dmg;
				mdef->mconf = TRUE;
			}
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
	case MADF_BURST:
	case MON_AURA_BOLT:
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
			case MADF_BURST:		alt_attk.adtyp = AD_MADF; break;
			case MON_AURA_BOLT:
				if(youagr){
					if(u.ualign.record < -3){
						alt_attk.damn = 0;
						alt_attk.damd = min(alt_attk.damd, 4);
						alt_attk.adtyp = AD_UNHY;
					}
					else if(u.ualign.record > 3){
						alt_attk.damn = 0;
						alt_attk.damd = min(alt_attk.damd, 4);
						alt_attk.adtyp = AD_HOLY; 
					}
					else
						return MM_MISS;
				}
				else if(magr->mtyp == PM_GRAND_MASTER){
					alt_attk.damd = 4;
					alt_attk.adtyp = AD_HOLY;
				}
				else if(magr->mtyp == PM_MASTER_KAEN){
					alt_attk.damn = 0;
					alt_attk.damd = 4;
					alt_attk.adtyp = AD_UNHY; 
				}
				else if(is_unholy_mon(magr)){
					alt_attk.damn = 0;
					alt_attk.damd = min(alt_attk.damd, 4);
					alt_attk.adtyp = AD_UNHY; 
				}
				else {
					alt_attk.damn = 0;
					alt_attk.damd = min(alt_attk.damd, 4);
					alt_attk.adtyp = AD_HOLY; 
				}
			break;
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
				int weapons[] = {ARROW, ELVEN_ARROW, ORCISH_ARROW, SILVER_ARROW, GOLDEN_ARROW, ANCIENT_ARROW, YA, CROSSBOW_BOLT, DROVEN_BOLT, DART, 
								SHURIKEN, BOOMERANG, CHAKRAM, SPEAR, ATGEIR, ELVEN_SPEAR, DROVEN_SPEAR, ORCISH_SPEAR, DWARVISH_SPEAR, JAVELIN,
								TRIDENT, DAGGER, ELVEN_DAGGER, DROVEN_DAGGER, ORCISH_DAGGER, ATHAME, TECPATL, SCALPEL, KNIFE, STILETTO, 
								SICKLE, ELVEN_SICKLE, AXE
				};
				weap = ROLL_FROM(weapons);
			}
			/* make them */
			otmp = mksobj(weap, NO_MKOBJ_FLAGS);
			otmp->quan = min(dmn, 16);
			otmp->owt = weight(otmp);
			otmp->spe = 0;
			if(magr && magr->mtyp == PM_SUZERAIN){
				add_oprop(otmp, OPROP_LESSER_FLAYW);
			}

			/* call projectile() to shoot fire all the weapons */
			m_shot.n = otmp->quan;
			m_shot.s = TRUE;
			m_shot.o = otmp->otyp;
			for (m_shot.i = 1; m_shot.i <= m_shot.n; m_shot.i++) {
				(void)projectile(magr, otmp, (void *)0, HMON_PROJECTILE|HMON_FIRED, x(mdef), y(mdef), 0, 0, 0, 0, FALSE, FALSE, FALSE);
			}
		}
		return MM_HIT;

	case DISINTEGRATION:
		/* needs direct target */
		if (!foundem) {
			impossible("disintegration with no mdef?");
			return MM_MISS;
		}
		if(magr){
			if(youdef || cansee(mdef->mx, mdef->my))
				pline("A disintegration beam shines down on %s from above!",
					youdef ? "you" : mon_nam(mdef));
			struct attack disintegrate = {AT_BEAM, AD_DISN, magr->mtyp == PM_BLESSED ? 7 : magr->mtyp == PM_PARASITIZED_EMBRACED_ALIDER ? 4 : 3, 1};
			//xmeleehurty(magr, mdef, attk, originalattk, weapon, dohitmsg, flatdmg, dieroll, vis, ranged)
			(void)xmeleehurty(magr, mdef, &disintegrate, &disintegrate, (struct obj **)0, FALSE, -1, rn1(18, 2), canseemon(mdef), TRUE);
		}
		else {
			return cast_spell(magr, mdef, attk, PSI_BOLT, tarx, tary);
		}
		if(youdef)
			stop_occupation();
		return MM_HIT;

		break;
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
				dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);
			}

			/* destroy items if it hit */
			if (!(reflects || UseInvShock_res(mdef))) {
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

	case MOTHER_S_GAZE:
		dmg = 0;
		/* needs direct target */
		if (!foundem) {
			impossible("mother's gaze with no mdef?");
			return MM_MISS;
		}
		else
		{
			magr->movement += NORMAL_SPEED;
			if(youagr)
				pline("The wild, staring eyes that cover your extra hand-appendages focus on %s.", mon_nam(mdef));
			else if(youdef)
				pline("The wild, staring eyes that cover %s extra hand-appendages focus on you.", s_suffix(mon_nam(magr)));
			else if(canseemon(magr))
				pline("The wild, staring eyes that cover %s extra hand-appendages focus on %s.", s_suffix(mon_nam(magr)), mon_nam(mdef));
			if(distmin(x(magr), y(magr), x(mdef), y(mdef)) <= mlev(magr)/10+1 && !resist(mdef, '\0', 0, NOTELL)){
				if(!youdef){
					mdef->mcanmove = 0;
					mdef->mfrozen = min_ints(7, max(mdef->mfrozen, u.uinsight/11));
				}
				else {
					mdef->movement -= u.uinsight/11;
				}
			}
			if(!youdef && cansee(x(mdef),y(mdef)))
				pline("%s is struck by a bolt of lightning.", Monnam(mdef));
			else if(youdef)
				You("are struck  by a bolt of lightning.");
			if (Shock_res(mdef)) {
				shieldeff(mdef->mx, mdef->my);
			} else {
				dmg = d(min(10, u.uinsight/11*2),6);
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
				dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);
			}

			if (!UseInvFire_res(mdef)) {
				(void)burnarmor(mdef, FALSE);
				destroy_item(mdef, SCROLL_CLASS, AD_FIRE);
				destroy_item(mdef, POTION_CLASS, AD_FIRE);
				destroy_item(mdef, SPBOOK_CLASS, AD_FIRE);
			}

			if (youdef) {
				burn_away_slime();
				melt_frozen_air();
			}

			(void)burn_floor_paper(tarx, tary, TRUE, youagr);
		}
		return xdamagey(magr, mdef, attk, dmg);

	case GEYSER:
	case STEAM_GEYSER:
		/* needs direct target */
		if (!foundem) {
			impossible("geyser with no mdef?");
			return MM_MISS;
		}
		else
		{
			struct obj * boots = (youdef ? uarmf : which_armor(mdef, W_ARMF));
#define	TOTAL_DUNK		1
#define PARTIAL_DUNK	2
			char dunked = FALSE;

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
						dunked = PARTIAL_DUNK;
					}
				}
				else {
					if (mdef->data->mmove >= 14) pline("%s puts the added monmentum to good use!", Monnam(mdef));
					else if (mdef->data->mmove <= 10){
						pline("%s is knocked around by the geyser's force!", Monnam(mdef));
						dunked = PARTIAL_DUNK;
					}
					
					if (mdef->data->mmove >= 25) mdef->movement += 12;
					else if (mdef->data->mmove >= 18) mdef->movement += 8;
					else if (mdef->data->mmove >= 14) mdef->movement += 6;
					else if (mdef->data->mmove <= 3) dmg = d(8, 6);
					else if (mdef->data->mmove <= 6) dmg = d(4, 6);
					else if (mdef->data->mmove <= 10) dmg = rnd(6);
				}
			}
			else {
				dunked = TOTAL_DUNK;
				/* message */
				if (youagr || youdef || canseemon(mdef)) {
					pline("A sudden geyser slams into %s from nowhere!",
						youdef ? "you" : mon_nam(mdef));
				}
				/* this is physical damage, not magical damage */
				dmg = d(8, 6);
				dmg = reduce_dmg(mdef,dmg,TRUE,FALSE);

				if (boots && boots->otyp == find_mboots()) {
					if(youdef) pline("Good thing you're wearing mud boots!");
				}
				else
					water_damage(youdef ? invent : mdef->minvent, FALSE, FALSE, FALSE, mdef);
			}
			if(spell == STEAM_GEYSER && dunked){
				if(!Fire_res(mdef)){
					if(dunked == TOTAL_DUNK)
						dmg += d(3,6);
					else
						dmg += d(1,10);
				}
				//Boiling water just boils potions
				if(dunked == TOTAL_DUNK && !UseInvFire_res(mdef)){
					destroy_item(mdef, POTION_CLASS, AD_FIRE);
				}
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
				if (!UseInvAcid_res(mdef)) {
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

	case RAIN:
		/* needs direct target */
		if (!foundem) {
			impossible("rain with no mdef?");
			return MM_MISS;
		}
		else {
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("A torrent of water rains down on %s!",
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
				dmg = flaming(mdef->data) ? d(8, 6) : 0;
				if (!dmg) {
					if (youdef)
						pline("It feels mildly uncomfortable.");
				}
				water_damage(youdef ? invent : mdef->minvent, FALSE, FALSE, FALSE, mdef);
			}
		}
		return xdamagey(magr, mdef, attk, dmg);

	case BLOOD_RAIN:
		/* needs direct target */
		if (!foundem) {
			impossible("blood rain with no mdef?");
			return MM_MISS;
		}
		else {
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("A torrent of blood rains down on %s!",
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
				if(helm->blessed)
					unbless(helm);
				else if(!helm->cursed)
					curse(helm);
				if(youdef)
					change_usanity(save_vs_sanloss() ? 0 : -1*d(1,4), TRUE);
			}
			else {
				/* check resistance and override damage */
				dmg = flaming(mdef->data) ? d(8, 6) : 0;
				water_damage(youdef ? invent : mdef->minvent, FALSE, FALSE, WD_BLOOD, mdef);
				if(youdef)
					change_usanity(save_vs_sanloss() ? -1 : -1*d(2,6), TRUE);
			}
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
			pdmg = reduce_dmg(mdef,pdmg,TRUE,FALSE);
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
				cdmg = reduce_dmg(mdef,cdmg,FALSE,TRUE);
			}
			if (!UseInvCold_res(mdef)) {
				destroy_item(mdef, POTION_CLASS, AD_COLD);
			}

			/* sum damage components to override dmg */
			dmg = pdmg + cdmg;

			/* player cold madness*/
			if (youdef) roll_frigophobia();
		}
		return xdamagey(magr, mdef, attk, dmg);

	case STARFALL:
		/* needs direct target */
		if (!foundem) {
			impossible("starfall with no mdef?");
			return MM_MISS;
		}
		else {
			int pdmg = d(8, 8);	/* physical */
			int cdmg = d(4, 8);	/* cold */
			int edmg = d(4, 8);	/* energy */
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("%s %s hit by a flury of hail and silver stars!",
					youdef ? "You" : Monnam(mdef),
					youdef ? "are" : "is"
					);
			}
			
			/* extra message for the silver */
			if (hates_silver(youracedata)) {
				pdmg += d(4, 20);/* silver */
				if(youdef){
					if (noncorporeal(youracedata)) {
						pline("The silver stars sear you!");
					}
					else {
						pline("The silver stars sear your %s!", body_part(BODY_FLESH));
					}
				}
			}

			/* calculate physical damage */
			pdmg = reduce_dmg(mdef,pdmg,TRUE,FALSE);
			/* apply average DR */
			pdmg -= max(0, (youdef ? u.udr : avg_mdr(mdef)));
			if (pdmg < 1)
				pdmg = 1;

			/* special antimagic effect */
			if (youdef)
				drain_en(edmg);
			else
				mdef->mspec_used += edmg;

			/* calculate cold damage */
			if (Cold_res(mdef)) {
				shieldeff(x(mdef), y(mdef));
				cdmg = 0;
			}
			else {
				cdmg = reduce_dmg(mdef,cdmg,FALSE,TRUE);
			}
			if (!UseInvCold_res(mdef)) {
				destroy_item(mdef, POTION_CLASS, AD_COLD);
			}

			/* sum damage components to override dmg */
			dmg = pdmg + cdmg;

			/* player cold madness*/
			if (youdef) roll_frigophobia();
		}
		return xdamagey(magr, mdef, attk, dmg);

	case PYRO_STORM:
		if (!foundem) {
			impossible("pyroclastic storm with no mdef?");
			return MM_MISS;
		}
		else {
			int pdmg = d(2,12);	/* physical */
			int fdmg = d(2,12);	/* fire */
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("Burning rocks pummel %s from all sides!",
					youdef ? "you" : mon_nam(mdef));
			}

			/* calculate physical damage */
			pdmg = reduce_dmg(mdef,pdmg,TRUE,FALSE);
			/* apply average DR */
			pdmg -= max(0, (youdef ? u.udr : avg_mdr(mdef)));
			if (pdmg < 1)
				pdmg = 1;

			/* calculate fire damage */
			if (Fire_res(mdef)) {
				shieldeff(x(mdef), y(mdef));
				fdmg = 0;
			}
			else {
				fdmg = reduce_dmg(mdef,fdmg,FALSE,TRUE);
			}
			if (!UseInvFire_res(mdef)) {
				destroy_item(mdef, POTION_CLASS, AD_FIRE);
				if (!rn2(6)) destroy_item(mdef, SCROLL_CLASS, AD_FIRE);
				if (!rn2(10)) destroy_item(mdef, SPBOOK_CLASS, AD_FIRE);
			}

			/* sum damage components to override dmg */
			dmg = pdmg + fdmg;

			/* other effects */
			if (youdef) {
				burn_away_slime();
				melt_frozen_air();
			}
		}
		return xdamagey(magr, mdef, attk, dmg);

	case GOD_RAY:
		if (!foundem) {
			impossible("god ray with no mdef?");
			return MM_MISS;
		}
		else {
			int hdmg = d(6,7);	/* holy damage */
			int ldmg = d(1,7);	/* lightning */
			/* message */
			if (youagr || youdef || canseemon(mdef)) {
				pline("Holy light shines down on %s from above!",
					youdef ? "you" : mon_nam(mdef));
			}

			if(mdef->data->mlet == S_TROLL){
				pline("The sunlight strikes %s!",
					youdef ? "you" : mon_nam(mdef));
				if (Stone_res(mdef)) pline("But %s %s even slow down!", youdef ? "you" : mon_nam(mdef), youdef ? "don't" : "doesn't");
				if (youdef)
					instapetrify("Holy sunlight");
				else if(!Stone_res(mdef) && !munstone(mdef, youagr))
					minstapetrify(mdef, youagr);
				if (!Stone_res(mdef)) {
					if (*hp(mdef) > 0)
						return MM_DEF_LSVD;
					else
						return MM_DEF_DIED;
				}
			}
			else if(mdef->mtyp == PM_GREMLIN || mdef->mtyp == PM_HUNTING_HORROR){
				pline("The sunlight sears %s!",
					youdef ? "you" : mon_nam(mdef));
				hdmg += *hp(mdef);
			}
			else if(hates_holy_mon(mdef)){
				hdmg = reduce_dmg(mdef,hdmg,FALSE,TRUE);
			}
			else hdmg = 0;

			/* calculate lightning damage */
			if (Shock_res(mdef)) {
				shieldeff(x(mdef), y(mdef));
				ldmg = 0;
			}
			else {
				ldmg = reduce_dmg(mdef,ldmg,FALSE,TRUE);
			}
			if (!UseInvShock_res(mdef)) {
				destroy_item(mdef, WAND_CLASS, AD_ELEC);
				destroy_item(mdef, RING_CLASS, AD_ELEC);
			}

			/* sum damage components to override dmg */
			dmg = hdmg + ldmg;
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

					if (!(resisted = (Magic_res(mdef) || resists_death(mdef) || resist(mdef, 0, 0, FALSE))) &&
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


	case EARTH_CRACK:
		/* needs direct target */
		if (!(tarx || tary)) {
			impossible("earth-crack spell with no target location?");
			return MM_MISS;
		}
		else {
			struct trap * ttmp = t_at(tarx, tary);
			if(!ttmp || !(ttmp->ttyp == PIT || ttmp->ttyp == SPIKED_PIT || ttmp->ttyp == HOLE || ttmp->ttyp == TRAPDOOR)){
				/* message */
				pline_The("%s tremors!",
					In_endgame(&u.uz) ? "plane" : "dungeon");

				do_earthquake(tarx, tary, 0, min((mlev(magr) - 1) / 6 + 1, 8), FALSE, magr);

				aggravate(); /* wake up without scaring */
				if(couldsee(tarx, tary))
					stop_occupation();	/* even if you weren't targeted, you certainly noticed! */
				return MM_HIT;
			}
			else {
				/*Note: May be different than the intended target!*/
				struct monst *smdef = m_u_at(tarx, tary);
				/* message */
				char heshe[BUFSZ];
				youdef = smdef == &youmonst;

				/* check resistance cases and do effects */
				if (youdef ? !u.utrap : !smdef->mtrapped) {
					pline("The earth's maw snaps shut!");
					deltrap(ttmp);
					return MM_MISS;
				}
				else if ((youdef && (u.sealsActive & SEAL_OSE)) || (smdef == u.usteed && u.sealsActive&SEAL_BERITH && u.sealsActive&SEAL_OSE)) {
					//phasing
					shieldeff(x(smdef), y(smdef));
					dmg = 0;
				}
				else {
					if (rn2(mlev(magr)) > 12) {
						dmg += *hp(smdef)/4;
						if(youdef){
							pline("The earth's maw chews you!");
							killer_format = KILLED_BY;
							killer = "the hungry earth";
						}
						else if(canspotmon(smdef)){
							pline("The earth's maw chews %s!", mon_nam(smdef));
						}
						if(youdef || smdef == u.usteed)
							set_wounded_legs(BOTH_SIDES, (int)HWounded_legs + rn1(100,50));
					}
					else {
						if(youdef)
							pline("The earth's maw squeezes you!");
						else if(canspotmon(smdef))
							pline("The earth's maw squeezes %s!", mon_nam(smdef));
						dmg /= 4; //still take damage
					}
				}
				if(couldsee(tarx, tary))
					stop_occupation();	/* even if you weren't targeted, you certainly noticed! */
				return xdamagey(magr, smdef, attk, dmg);
			}
		}
		impossible("Bad flow through crack spell handling.");
		return MM_MISS;/*shouldn't be reached*/


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
				/* message */
				if (youagr || youdef || canseemon(mdef)) {
					pline("%s is afflicted with disease!", Monnam(mdef));
				}
				/* 1/10 chance of instakill */
				if (!rn2(10)){
					if (youagr) killed(mdef);
					else monkilled(mdef, "", AD_SPEL);
					/* instakill */
					return ((*hp(mdef) > 0 ? MM_DEF_LSVD : MM_DEF_DIED) | MM_HIT);
				}
				//else damage
			}
			else return MM_MISS;
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
			dmg = reduce_dmg(mdef,dmg,TRUE,FALSE);
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

	case SLIMIFY:
		/* needs direct target */
		if (!foundem) {
			impossible("slimify with no mdef?");
			return MM_MISS;
		}
		else {
			/* message player */
			if (youdef && malediction)
				verbalize(rn2(2) ? "Thou shalt quiver before me!" :
				"Thy flesh shall be goo!");

			static struct attack slimify = { AT_NONE, AD_SLIM, 0, 0 };
			result = xmeleehurty(magr, mdef, &slimify, &slimify, (struct obj **)0, FALSE, 0, 0, -1, TRUE);
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
		else {
			int n = 0;
			char * rays;

			if (zap_hit(mdef, 0, TRUE))
				n++;
			if (zap_hit(mdef, 0, TRUE))
				n++;
			if (!n){
				if (youagr || youdef || canseemon(mdef))
					pline("Silver rays whiz past %s!",
					youdef ? "you" : mon_nam(mdef));
				return MM_MISS;
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
				if (!UseInvFire_res(mdef)) {
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
				if (!UseInvCold_res(mdef)) {
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
				if (!UseInvFire_res(mdef)) {
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
				if (!UseInvShock_res(mdef)) {
					destroy_item(mdef, WAND_CLASS, AD_ELEC);
					destroy_item(mdef, RING_CLASS, AD_ELEC);
				}
			}
			else if (!Cold_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s frozen by %s of silver light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n, 20);
				if (!UseInvCold_res(mdef)) {
					destroy_item(mdef, POTION_CLASS, AD_COLD);
				}
			}
			else if (!Acid_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s burned by %s of silver light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n, 20);
				if (!UseInvAcid_res(mdef)) {
					destroy_item(mdef, POTION_CLASS, AD_FIRE);
				}
			}
			else {
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s pierced by %s of silver light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is", rays);
				dmg = d(n, 20) - (youdef ? roll_udr(magr, attk->aatyp) : roll_mdr(mdef, magr, attk->aatyp));
				if(dmg < 1)
					dmg = 1;
				dmg = reduce_dmg(mdef,dmg,TRUE,FALSE);
			}
			dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);
		}
		return xdamagey(magr, mdef, attk, dmg);

	case GOLDEN_WAVE:
		/* needs direct target */
		if (!foundem) {
			impossible("golden wave with no mdef?");
			return MM_MISS;
		}
		else {
			if (!Fire_res(mdef) && species_resists_cold(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s burned by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = (d(2, 12) * 3 + 1) / 2;
				if (!UseInvFire_res(mdef)) {
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
				if (!UseInvCold_res(mdef)) {
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
				if (!UseInvFire_res(mdef)) {
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
				if (!UseInvShock_res(mdef)) {
					destroy_item(mdef, WAND_CLASS, AD_ELEC);
					destroy_item(mdef, RING_CLASS, AD_ELEC);
				}
			}
			else if (!Cold_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s frozen by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12);
				if (!UseInvCold_res(mdef)) {
					destroy_item(mdef, POTION_CLASS, AD_COLD);
				}
			}
			else if (!Acid_res(mdef)){
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s burned by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12);
				if (!UseInvAcid_res(mdef)) {
					destroy_item(mdef, POTION_CLASS, AD_FIRE);
				}
			}
			else {
				if (youagr || youdef || canseemon(mdef))
					pline("%s %s slashed by golden light!",
					youdef ? "You" : Monnam(mdef), youdef ? "are" : "is");
				dmg = d(2, 12) - (youdef ? roll_udr(magr, attk->aatyp) : roll_mdr(mdef, magr, attk->aatyp));
				if(dmg < 1)
					dmg = 1;
				dmg = reduce_dmg(mdef,dmg,TRUE,FALSE);
			}
			dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);
		}
		return xdamagey(magr, mdef, attk, dmg);

	case MON_WARP_THROW:
		/* needs direct target */
		if (!foundem) {
			impossible("warp-throw with no mdef?");
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
				hurtle(dx, dy, BOLT_LIM, FALSE, FALSE);
			}
			else {
				mhurtle(mdef, dx, dy, BOLT_LIM, TRUE);
			}
		}
		return MM_HIT;

	case MON_WARP:
		/* needs direct target */
		if (!foundem) {
			impossible("warp-blades with no mdef?");
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
			dmg = reduce_dmg(mdef,dmg,TRUE,TRUE);
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
				dmg = dmgval(otmp, &youmonst, 0, magr);
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

			dmg = reduce_dmg(mdef,dmg,TRUE,FALSE);
		}
		return xdamagey(magr, mdef, attk, dmg);

	case MON_RED_WORD:
		if(youdef){
			// if(!deaf){
			// }
			// else 
			if(u.ufirst_know){
				pline("%s whispers the red truth to you. You already know!", Monnam(magr));
				return MM_MISS;
			}
			else {
				pline("%s whispers a dreadful secret to you! Your mind blanks in self-defense!", Monnam(magr));
			}
			int discomfort = u_clothing_discomfort();
			if(discomfort > 0){
				pline("Your clothing fills you with horror!");
				change_usanity(-discomfort, TRUE);
				HPanicking += discomfort;
			}
		}
		else if(canseemon(magr)){
			pline("%s whispers %s to %s.", Monnam(magr), u.ufirst_know ? "the red truth" : "some secret", mon_nam(mdef));

			if(is_deaf(mdef) || resist(mdef, '\0', 0, 0))
				return MM_MISS;
			mdef->mcrazed = TRUE;
			mdef->mflee = TRUE;
			mdef->mdisrobe = TRUE;
			mon_throw_armor(mdef);
		}
		return MM_HIT;

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
				color = EXPL_FROSTY;
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
				int x;
				int y;
				int i;
				for(i = 0; i < 3; i++){
					x = tarx + rn2(3) - 1;
					y = tary + rn2(3) - 1;
					if(!isok(x,y) || !ZAP_POS(levl[x][y].typ)){
						x = tarx;
						y = tary;
					}
					explode(x, y, adtyp, MON_EXPLODE, dmg, color, 1);
				}
			}
		}
		return MM_HIT | ((mdef && DEADMONSTER(mdef)) ? MM_DEF_DIED : 0) | ((magr && DEADMONSTER(magr)) ? MM_AGR_DIED : 0);

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
		return MM_HIT | ((mdef && DEADMONSTER(mdef)) ? MM_DEF_DIED : 0) | ((magr && DEADMONSTER(magr)) ? MM_AGR_DIED : 0);

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
		return MM_HIT | ((mdef && DEADMONSTER(mdef)) ? MM_DEF_DIED : 0) | ((magr && DEADMONSTER(magr)) ? MM_AGR_DIED : 0);

	case HYPNOTIC_COLORS:{
		struct attack gaze = {AT_WDGZ, AD_PLYS, 2, 6};
		int subresult = 0;
		int result = 0;
		struct monst *nmon;
		int dx;
		int dy;
		if(canseemon(magr))
			pline("%s eye-studded tentacles flash with hypnotic colors!", s_suffix(Monnam(magr))); //Assumes mistweaver
		for(mdef = fmon; mdef; mdef = nmon){
			nmon = mdef->nmon;
			if(DEADMONSTER(mdef))
				continue;
			if(!mm_aggression(magr, mdef) || !mon_can_see_mon(mdef, magr))
				continue;
			subresult = xgazey(magr, mdef, &gaze, 0);
			if(subresult & MM_HIT){
				dx = x(magr) - x(mdef);
				dy = y(magr) - y(mdef);
				if(canseemon(mdef))
					pline("%s stumbles towards %s, mesmerized.", Monnam(mdef), mon_nam(magr));
				mhurtle(mdef, sgn(dx), sgn(dy), 1, TRUE);
			}
		}
		if(!magr->mpeaceful && canseemon(magr)){
			subresult = xgazey(magr, &youmonst, &gaze, 0);
			if(subresult & MM_HIT){
				dx = x(magr) - u.ux;
				dy = y(magr) - u.uy;
				pline("Mesmerized, you stumble towards %s!", mhim(magr));
				hurtle(sgn(dx), sgn(dy), 1, FALSE, FALSE);
			}
		}
			
		return subresult;
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
				magr->mspec_used += 1000;
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

	case TREMOR:
		if (!(tarx || tary)) {
			impossible("tremor spell with no target location?");
			return MM_MISS;
		}
		else {
			/* message */
			pline_The("%s tremors!",
				In_endgame(&u.uz) ? "plane" : "dungeon");

			do_earthquake(tarx, tary, 0, min((mlev(magr) - 1) / 6 + 1, 8), FALSE, magr);

			aggravate(); /* wake up without scaring */
			if(couldsee(tarx, tary))
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
				if(magr->mtyp == PM_CHAOS){
					//Chaos could heal himself fully, but lets not do that.
					*hp(magr) += 999;
				}
				else {
					*hp(magr) += d(dmn, 8);
				}
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
			set_mcan(magr, FALSE);
			magr->mberserk = 0;
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
			mon_adjust_speed(magr, 1, (struct obj *)0, TRUE);
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
					mon_adjust_speed(cmon, 1, (struct obj *)0, TRUE);
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
		if (youagr) {
			/* you don't get to cast this one, either */
			return cast_spell(magr, mdef, attk, (foundem ? PSI_BOLT : CURE_SELF), tarx, tary);
		}
		else {
			int extraturns = d(1, 4) + 1, i;
			struct monst *tmpm;
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
			if (DimensionalLock) return MM_MISS;
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
		else if(DimensionalLock)
			return MM_MISS;
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
			mkundead(&mm, TRUE, NO_MINVENT, normalAngel(magr) ? HOLYDEAD_FACTION : magr->mfaction);
			stop_occupation();
		}
		return MM_HIT;

	case YELLOW_DEAD:
		/* creatures raised are not marked as summoned */
		if (!youdef) {
			/* only mvu allowed */
			return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
		}
		else if(rn2(2))
			yellow_nasty();
		else
		{
			coord mm;
			if (canseemon(magr))
				pline("%s raised the dead!", Monnam(magr));
			mm.x = x(magr);
			mm.y = y(magr);
			mk_yellow_undead(&mm, TRUE, NO_MINVENT, YELLOW_FACTION);
			stop_occupation();
		}
		return MM_HIT;

	case MON_CANCEL:
		if (!mdef) {
			impossible("debuff spell with no target?");
			return MM_MISS;
		}
		if(!foundem)
			return MM_MISS;
		cancel_monst(mdef, (struct obj	*)0, youagr, TRUE, FALSE,0);
		return MM_HIT;

	case SUMMON_MONS:
		if(DimensionalLock)
			return MM_MISS;
		else if (!youdef || u.summonMonster || Infuture) {
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
		if(DimensionalLock)
			return MM_MISS;
		else if (!youdef || u.summonMonster || !foundem) {
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

	case SUMMON_TANNIN:
		if(DimensionalLock)
			return MM_MISS;
		else if (!youdef || u.summonMonster || !foundem) {
			/* only mvu allowed */
			/* only one summon spell per global turn allowed */
			/* since it always summons adjacent to player, only allow casting if they've found you */
			return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
		}
		else
		{
			struct monst * mtmp;
			/* summon_minion always appears near the player */
			mtmp = makemon(&mons[pick_tannin(magr)], tarx, tary, MM_ADJACENTOK | MM_NOCOUNTBIRTH | MM_ESUM);
			if (mtmp) {
				// mtmp->mvar_tannintype = pick_tannin(magr);
				u.summonMonster = TRUE;
				if (canspotmon(mtmp))
					pline("%s ascends from below!",
					An(Hallucination ? rndmonnam() : "fiend"));
				else
					You("sense the arrival of %s.",
					an(Hallucination ? rndmonnam() : "hostile fiend"));
				mark_mon_as_summoned(mtmp, magr, ESUMMON_PERMANENT, 0);
			}
			else
				return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
			stop_occupation();
		}
		return MM_HIT;

	case SUMMON_ANGEL:
		if(DimensionalLock)
			return MM_MISS;
		else if (!youdef || u.summonMonster) {
			/* only mvu allowed */
			/* only one summon spell per global turn allowed */
			return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
		}
		else if (is_alienist(magr->data) || has_template(magr, MAD_TEMPLATE)) {
			/* alienists summon aliens. wowzers. */
			return cast_spell(magr, mdef, attk, SUMMON_ALIEN, tarx, tary);
		}
		else if (has_template(magr, FALLEN_TEMPLATE)) {
			return cast_spell(magr, mdef, attk, SUMMON_DEVIL, tarx, tary);
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
				int gnum;
				aligntyp alignment;
				if (get_mx(magr, MX_EMIN)) {
					alignment = EMIN(magr)->min_align;
					gnum = EMIN(magr)->godnum;
				}
				else if (get_mx(magr, MX_EPRI)) {
					alignment = EPRI(magr)->shralign;
					gnum = EPRI(magr)->godnum;
				}
				else {
					alignment = sgn(magr->data->maligntyp);
					gnum = align_to_god(alignment);
				}
				mtmp->isminion = TRUE;
				EMIN(mtmp)->min_align = alignment;
				EMIN(mtmp)->godnum = gnum;

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
					set_faction(mtmp, LAMASHTU_FACTION);
				//Note: these cases are not currently used, but handled anyway to be safe
				if(has_template(magr, MAD_TEMPLATE))
					set_template(mtmp, MAD_TEMPLATE);
				else if(has_template(magr, FALLEN_TEMPLATE))
					set_template(mtmp, FALLEN_TEMPLATE);
				mark_mon_as_summoned(mtmp, magr, ESUMMON_PERMANENT, 0);
			}
			else
				return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
			stop_occupation();
		}
		return MM_HIT;

	case SUMMON_ALIEN:
		if(DimensionalLock)
			return MM_MISS;
		else if (!youdef || u.summonMonster) {
			/* only mvu allowed */
			/* only one summon spell per global turn allowed */
			return cast_spell(magr, mdef, attk, (foundem ? OPEN_WOUNDS : CURE_SELF), tarx, tary);
		}
		else {
			struct monst *mtmp;
			int tries = 0;
			static struct permonst *aliens[] = {
				&mons[PM_SHOGGOTH],
				&mons[PM_HOUND_OF_TINDALOS],
				&mons[PM_HOOLOOVOO],
				&mons[PM_SHAMBLING_HORROR],
				&mons[PM_STUMBLING_HORROR],
				&mons[PM_WANDERING_HORROR],
				&mons[PM_MASTER_MIND_FLAYER],
				&mons[PM_EDDERKOP],
				&mons[PM_AOA],
				&mons[PM_HUNTING_HORROR],
				&mons[PM_BYAKHEE],
				&mons[PM_BEBELITH],
				&mons[PM_WEEPING_ANGEL],
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
		if(DimensionalLock)
			return MM_MISS;
		else if (!youdef || u.summonMonster) {
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
				mtmp = makemon_full(young[rn2(SIZE(young))], tarx, tary, MM_ADJACENTOK | MM_NOCOUNTBIRTH | MM_ESUM, -1, GOATMOM_FACTION);
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
		if(DimensionalLock)
			return MM_MISS;
		else if (!youdef || u.summonMonster) {
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
					if (otmp->owornmask && objects[otmp->otyp].oc_oprop[0] == INVIS)
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
						dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);
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
						dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);
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
					dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);
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
			dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);

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
					mdef->mcansee = 0;
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
			dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);

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
			impossible("drain energy with no target?");
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
				dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);
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
				dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);
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

				dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);
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
				else if (youdef && (rn2(3) && is_chaotic_mon(magr)) && !Hallucination) {
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
								hisherits(mdef),
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
					destroy_marm(mdef, smarm);
				}
				else {
					dmg = rnd(4);
					dmg = reduce_dmg(mdef,dmg,FALSE,TRUE);

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

	case EVIL_EYE:{
		struct attack evilEye = { AT_GAZE, AD_LUCK, 1, 4 };
		(void)xgazey(magr, mdef, &evilEye, -1);
		return MM_HIT;
	}
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

	case MUMMY_CURSE:
		if (!mdef) {
			impossible("mummy curse with no target?");
			return MM_MISS;
		}
		return mummy_curses_x(magr, mdef);

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
		else {
			if (u.ualign.record <= 1 || !rn2(min(u.ualign.record, 20))){
				if (!Punished) {
					punish((struct obj *)0);
					if (is_prince(magr->data) && Punished) uball->owt += 160;
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

	case INCARCERATE:{
		struct obj *otmp = 0;
		if (!youdef) {
			/* only makes sense vs player */
			return cast_spell(magr, mdef, attk, OPEN_WOUNDS, tarx, tary);
		}
		else {
			switch(rn2(3)){
				case 0:
				case 1:
					otmp = uarmc;
					if(!otmp)
						otmp = uarm;

					if(!otmp){
						if(humanoid_torso(youracedata)){
							otmp = mksobj(STRAITJACKET, MKOBJ_NOINIT);
							otmp->obj_color = CLR_YELLOW;
							otmp->objsize = youracedata->msize;
							pickup_object(otmp, 1, TRUE);
							curse(otmp);
							setworn(otmp, W_ARM);
							Armor_on();
							// Did something
							break;
						}
						// Didn't do anything, keep going
					}
					else if(otmp->otyp == STRAITJACKET && !otmp->cursed){
						curse(otmp);
						// Did something
						break;
					}
					
					if(!otmp || otmp->otyp == STRAITJACKET){
						otmp = some_armor(&youmonst);
					}
					
					if(otmp && otmp->otyp != STRAITJACKET){
						if(otmp->obj_color == CLR_YELLOW){
							teleport_steal_arm(magr, otmp);
						}
						else if(is_metallic(otmp)){
							if(!obj_resists(otmp, 55, 95)){
								if(!Blind)
									Your("%s turns golden yellow!", xname(otmp));
								set_material(otmp, GOLD);
								//Item may not naturally be yellow.
								otmp->obj_color = CLR_YELLOW;
							}
						}
						else {
							if(!obj_resists(otmp, 55, 95)){
								if(!Blind)
									Your("%s abruptly turns yellow!", xname(otmp));
								otmp->obj_color = CLR_YELLOW;
							}
						}
						// Did something
						break;
					}
				//If we didn't break before now, nothing was done. Fall through.
				case 2:
					if (!Punished) {
						punish((struct obj *)0);
						if (is_prince(magr->data) && Punished) uball->owt += 160;
					}
					else {
						Your("iron ball gets heavier!");
						if (is_prince(magr->data)) uball->owt += 240;
						else uball->owt += 160;
					}
					stop_occupation();
				break;
			}
		}
		return MM_HIT;
	}

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
			struct trap * ttmp;
			if ((ttmp = maketrap(mdef->mx, mdef->my, WEB))) {
				mintrap(mdef);
				newsym(mdef->mx, mdef->my);
			}
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
		case MON_SPE_BEARTRAP:
		if (!youdef) {
			struct trap * ttmp;
			if ((ttmp = maketrap(mdef->mx, mdef->my, BEAR_TRAP))) {
				mintrap(mdef);
				newsym(mdef->mx, mdef->my);
			}
		}
		else
		{
			struct trap * ttmp;
			if ((ttmp = maketrap(u.ux, u.uy, BEAR_TRAP))) {
				dotrap(ttmp, 0);
				newsym(u.ux, u.uy);
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
	case PAIN_BOLT:
	case SAN_BOLT:
	case DOUBT_BOLT:
	case BARF_BOLT:
	case BABBLE_BOLT:
	case CRUSH_BOLT:
	case OPEN_WOUNDS:
	case MAGIC_MISSILE:
	case CONE_OF_COLD:
	case LIGHTNING_BOLT:
	case SLEEP:
	case DRAIN_LIFE:
	case ARROW_RAIN:
	case DISINTEGRATION:
	case LIGHTNING:
	case MOTHER_S_GAZE:
	case FIRE_PILLAR:
	case GEYSER:
	case STEAM_GEYSER:
	case ACID_RAIN:
	case RAIN:
	case BLOOD_RAIN:
	case HAIL_FLURY:
	case ICE_STORM:
	case PYRO_STORM:
	case GOD_RAY:
	case DEATH_TOUCH:
	case PLAGUE:
	case FILTH:
	case TURN_TO_STONE:
	case SLIMIFY:
	case STRANGLE:
	case SILVER_RAYS:
	case GOLDEN_WAVE:
	case MON_WARP:
	case MON_WARP_THROW:
	case DROP_BOULDER:
	case DISINT_RAY:
	case MADF_BURST:
	case STARFALL:
	case MON_AURA_BOLT:
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
	case HYPNOTIC_COLORS:
	case MON_POISON_GAS:
	case SOLID_FOG:
	case EARTHQUAKE:
	case TREMOR:
	case EARTH_CRACK:
	/* also directed attack spells */
	case MAGIC_MISSILE:
	case CONE_OF_COLD:
	case LIGHTNING_BOLT:
	case SLEEP:
	case DISINT_RAY:
	case MADF_BURST:
	case MON_AURA_BOLT:
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
	case SUMMON_TANNIN:
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
	case MUMMY_CURSE:
	case NAIL_TO_THE_SKY:
	case STERILITY_CURSE:
	case PUNISH:
	case INCARCERATE:
	case MON_SPE_BEARTRAP:
	case DARKNESS:
	case MAKE_WEB:
	case MON_CANCEL:
	case MON_RED_WORD:
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
		&& !Infuture) /* does not work in Ana quest */
	{
		if ((sengr_at("Elbereth", tarx, tary) && (!Race_if(PM_DROW) || ELBERETH_HIGH_POWER))
			|| (sengr_at("Lolth", tarx, tary) && LOLTH_HIGH_POWER && (mlev(magr) < u.ulevel || u.ualign.record-- > 0)))
		{
			return TRUE;
		}
	}

	/* Don't cast summon spells (with some exceptions) in the Anachrononaut quest */
	if (Infuture && is_summon_spell(spellnum) && !(
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
	///The wizard can't even cast this anyway :(
	if ((spellnum == EARTHQUAKE || spellnum == TREMOR || spellnum == EARTH_CRACK) && In_endgame(&u.uz))
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
	if ((spellnum == MAGIC_MISSILE || spellnum == SLEEP || spellnum == CONE_OF_COLD || spellnum == LIGHTNING_BOLT || spellnum == MON_AURA_BOLT || spellnum == DISINT_RAY || spellnum == MADF_BURST)
		&& !clearline)
		return TRUE;

	/* don't cast drain life, death touch, whisper word if not in melee range */
	if ((spellnum == DRAIN_LIFE || spellnum == DEATH_TOUCH || spellnum == MON_RED_WORD)
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

	/* don't cast red word if target is already disrobed/disrobing */
	if (spellnum == MON_RED_WORD
		&& (youdef ? 
			(u.ufirst_know || !(uarmh || uarmc || uarm || uarmu || uarmg || uarmf || uamul || ublindf || uleft || uright))
			: (!(mdef->misc_worn_check&(W_ARMOR|W_AMUL|W_TOOL)) || mdef->mdisrobe)
			)
	)
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
			if (!magr->mtame == !tmpm->mtame || magr->mpeaceful == tmpm->mpeaceful){
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

	/* don't bother re-canceling already canceled target */
	if (spellnum == MON_CANCEL && mdef != &youmonst && mdef->mcan)
		return TRUE;

	/* the wiz won't use the following cleric-specific or otherwise weak spells */
	if (!youagr && magr->iswiz && (
		spellnum == SUMMON_SPHERE || spellnum == DARKNESS
		|| spellnum == PUNISH || spellnum == INSECTS
		|| spellnum == SUMMON_ANGEL || spellnum == DROP_BOULDER
		|| spellnum == DISINT_RAY || spellnum == DISINTEGRATION
		|| spellnum == MADF_BURST
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
		spellnum == LIGHTNING_BOLT || spellnum == MON_AURA_BOLT
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

/*
 * Pick a minion for the given monster
 */

int
pick_tannin(mon)
struct monst *mon;
{
	switch(mon->mtyp){
		case PM_PALE_NIGHT:
			if(rn2(6))
				return PM_SHALOSH_TANNAH;
			else return PM_TERAPHIM_TANNAH;
		break;
	}
	return PM_AKKABISH_TANNIN;
}

#endif /* OVL0 */

/*mcastu.c*/
