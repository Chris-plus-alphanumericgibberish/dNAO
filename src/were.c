/*	SCCS Id: @(#)were.c	3.4	2002/11/07	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef OVL0

void
were_change(mon)
register struct monst *mon;
{
	if( !is_were(mon->data)
		&& !(is_heladrin(mon->data) && mon->mhp < .5*mon->mhpmax && rn2(2))
		&& !(is_eeladrin(mon->data) && mon->mhp > .5*mon->mhpmax)
		&& !(is_yochlol(mon->data) && mon->mhp < .5*mon->mhpmax)
		&& !(mon->mtyp == PM_SELKIE || mon->mtyp == PM_SEAL)
		&& !(mon->mtyp == PM_INCUBUS || mon->mtyp == PM_SUCCUBUS)
		&& !(is_duergar(mon))
		&& !(mon->mtyp == PM_LURKING_HAND || mon->mtyp == PM_BLASPHEMOUS_HAND)
	) return;

	if(u.ustuck == mon && u.uswallow)
		return;

	if(mon->mtyp == PM_NOVIERE_ELADRIN && !is_pool(mon->mx, mon->my, FALSE)) return;
	
	if (humanoid_torso(mon->data)) {
		if (mon->mtyp == PM_INCUBUS || mon->mtyp == PM_SUCCUBUS){
			if(!Protection_from_shape_changers 
			&& !canseemon(mon) 
			&& mon->mfaction != INCUBUS_FACTION
			&& mon->mfaction != SUCCUBUS_FACTION
			&& !rn2(300)
			){
				mon->female = !mon->female;
				new_were(mon);
			}
	    } else if (mon->mtyp == PM_MAMMON || mon->mtyp == PM_GREEN_PIT_FIEND){
			if(!Protection_from_shape_changers 
			&& mon->mhp < mon->mhpmax
			&& !rn2(20)
			){
				//Both mammon's forms have humanoid torsos
				new_were(mon);
			}
	    } else if (is_heladrin(mon->data)){
			if(!Protection_from_shape_changers) new_were(mon);
	    } else if (is_duergar(mon)){
			if(mon->data->mlet == S_HUMANOID){
				if(Protection_from_shape_changers)
					;//Do nothing
				else if(!mon->mpeaceful && mon_can_see_you(mon)){
					if(rn2(2)) new_were(mon);
				}
				else if(nearby_targets(mon)){
					if(!rn2(4)) new_were(mon);
				}
			}
			else {
				if(Protection_from_shape_changers)
					new_were(mon); //Revert
				else if(!mon->mpeaceful && !mon_can_see_you(mon)){
					if(!rn2(20)) new_were(mon);
				}
				else if(!nearby_targets(mon)){
					if(!rn2(20)) new_were(mon);
				}
			}
	    } else if (
			!rn2(night() ? (flags.moonphase == FULL_MOON ?  3 : 30)
					 : (flags.moonphase == FULL_MOON ? 10 : 50))
		){
			if(!Protection_from_shape_changers){
				new_were(mon);		/* change into animal form */
				if (flags.soundok && !canseemon(mon)) {
					const char *howler;

					switch (monsndx(mon->data)) {
					case PM_WEREWOLF:	howler = "wolf";    break;
					case PM_WEREJACKAL: howler = "jackal";  break;
					case PM_ANUBAN_JACKAL: howler = "jackal";  break;
					default:		howler = (char *)0; break;
					}
					if (howler)
					You_hear("a %s howling at the moon.", howler);
				}
			}
	    }
	} else if (mon->mtyp == PM_LURKING_HAND){
		if(!rn2(20)) new_were(mon);
	} else if (mon->mtyp == PM_BLASPHEMOUS_HAND){
		if(!rn2(10)) new_were(mon);
	} else if (!rn2(30) || (is_were(mon->data) && Protection_from_shape_changers) 
		|| (is_yochlol(mon->data) && !Protection_from_shape_changers)
		|| (is_eeladrin(mon->data) && mon->mhp >= mon->mhpmax && !Protection_from_shape_changers)
	) {
		if(mon->mtyp == PM_BALL_OF_LIGHT){
			mon->mflee = 0;
			mon->mfleetim = 0;
		}
	    new_were(mon);		/* change back into human form */
		if(is_yochlol(mon->data)) mon->movement += 12;
	}
}

#endif /* OVL0 */
#ifdef OVLB

int
counter_were(pm)
int pm;
{
	switch(pm) {
	    case PM_WEREWOLF:	      return(PM_HUMAN_WEREWOLF);
	    case PM_HUMAN_WEREWOLF:   return(PM_WEREWOLF);
	    case PM_WEREJACKAL:	      return(PM_HUMAN_WEREJACKAL);
	    case PM_HUMAN_WEREJACKAL: return(PM_WEREJACKAL);
	    case PM_WERERAT:	      return(PM_HUMAN_WERERAT);
	    case PM_HUMAN_WERERAT:    return(PM_WERERAT);
		case PM_ANUBITE:		  return(PM_ANUBAN_JACKAL);
		case PM_ANUBAN_JACKAL:	  return(PM_ANUBITE);
		case PM_COURE_ELADRIN:	  return(PM_MOTE_OF_LIGHT);
		case PM_MOTE_OF_LIGHT:	  return(PM_COURE_ELADRIN);
		case PM_NOVIERE_ELADRIN:  return(PM_WATER_DOLPHIN);
		case PM_WATER_DOLPHIN:	  return(PM_NOVIERE_ELADRIN);
		case PM_BRALANI_ELADRIN:  return(PM_SINGING_SAND);
		case PM_SINGING_SAND:	  return(PM_BRALANI_ELADRIN);
		case PM_FIRRE_ELADRIN:	  return(PM_DANCING_FLAME);
		case PM_DANCING_FLAME:	  return(PM_FIRRE_ELADRIN);
		case PM_SHIERE_ELADRIN:	  return(PM_BALL_OF_LIGHT);
		case PM_BALL_OF_LIGHT:	  return(PM_SHIERE_ELADRIN);
		case PM_GHAELE_ELADRIN:	  return(PM_LUMINOUS_CLOUD);
		case PM_LUMINOUS_CLOUD:	  return(PM_GHAELE_ELADRIN);
		case PM_TULANI_ELADRIN:	  return(PM_BALL_OF_RADIANCE);
		case PM_BALL_OF_RADIANCE: return(PM_TULANI_ELADRIN);
		case PM_GAE_ELADRIN:	  return(PM_WARDEN_TREE);
		case PM_WARDEN_TREE:	  return(PM_GAE_ELADRIN);
		case PM_BRIGHID_ELADRIN:  return(PM_PYROCLASTIC_VORTEX);
		case PM_PYROCLASTIC_VORTEX: return(PM_BRIGHID_ELADRIN);
		case PM_UISCERRE_ELADRIN: return(PM_WATERSPOUT);
		case PM_WATERSPOUT:       return(PM_UISCERRE_ELADRIN);
		case PM_CAILLEA_ELADRIN:  return(PM_MOONSHADOW);
		case PM_MOONSHADOW:       return(PM_CAILLEA_ELADRIN);
		case PM_DRACAE_ELADRIN:	  return(PM_MOTHERING_MASS);
		case PM_MOTHERING_MASS:	  return(PM_DRACAE_ELADRIN);
		case PM_GWYNHARWYF:		  return(PM_FURIOUS_WHIRLWIND);
		case PM_FURIOUS_WHIRLWIND: return(PM_GWYNHARWYF);
		case PM_ASCODEL:		  return(PM_BLOODY_SUNSET);
		case PM_BLOODY_SUNSET:	  return(PM_ASCODEL);
		case PM_FAERINAAL:		  return(PM_BALL_OF_GOSSAMER_SUNLIGHT);
		case PM_BALL_OF_GOSSAMER_SUNLIGHT: return(PM_FAERINAAL);
		case PM_QUEEN_MAB:		  return(PM_COTERIE_OF_MOTES);
		case PM_COTERIE_OF_MOTES: return(PM_QUEEN_MAB);
		case PM_KETO:			  return(PM_ANCIENT_TEMPEST);
		case PM_ANCIENT_TEMPEST:  return(PM_KETO);
		case PM_QUEEN_OF_STARS:	  return(PM_ETERNAL_LIGHT);
		case PM_ETERNAL_LIGHT:	  return(PM_QUEEN_OF_STARS);
		case PM_ALRUNES:		  return(PM_HATEFUL_WHISPERS);
		case PM_HATEFUL_WHISPERS: return(PM_ALRUNES);
		case PM_SUCCUBUS: return(PM_INCUBUS);
		case PM_INCUBUS: return(PM_SUCCUBUS);
		case PM_MAMMON:			  return(PM_GREEN_PIT_FIEND);
		case PM_GREEN_PIT_FIEND:  return(PM_MAMMON);
		case PM_SELKIE: return(PM_SEAL);
		case PM_SEAL: return(PM_SELKIE);
		
		case PM_DUERGAR: return(PM_GIANT_DUERGAR);
		case PM_GIANT_DUERGAR: return(PM_DUERGAR);
		case PM_DUERGAR_STONEGUARD: return(PM_GIANT_STONEGUARD);
		case PM_GIANT_STONEGUARD: return(PM_DUERGAR_STONEGUARD);
		case PM_DUERGAR_DEBILITATOR: return(PM_DUERGAR_ANNIHILATOR);
		case PM_DUERGAR_ANNIHILATOR: return(PM_DUERGAR_DEBILITATOR);

		case PM_LURKING_HAND: return(PM_BLASPHEMOUS_HAND);
		case PM_BLASPHEMOUS_HAND: return(PM_LURKING_HAND);
		
		case PM_YOCHLOL: 
			switch(rnd(3)){
			case 1: return(PM_UNEARTHLY_DROW);
			case 2: return(PM_STINKING_CLOUD);
			case 3: return(PM_DEMONIC_BLACK_WIDOW);
		}
		case PM_UNEARTHLY_DROW: 
			switch(rnd(3)){
			case 1: return(PM_YOCHLOL);
			case 2: return(PM_STINKING_CLOUD);
			case 3: return(PM_DEMONIC_BLACK_WIDOW);
		}
		case PM_STINKING_CLOUD: 
			switch(rnd(3)){
			case 1: return(PM_UNEARTHLY_DROW);
			case 2: return(PM_YOCHLOL);
			case 3: return(PM_DEMONIC_BLACK_WIDOW);
		}
		case PM_DEMONIC_BLACK_WIDOW: 
			switch(rnd(3)){
			case 1: return(PM_UNEARTHLY_DROW);
			case 2: return(PM_STINKING_CLOUD);
			case 3: return(PM_YOCHLOL);
		}
	    default:			      return(0);
	}
}

int
transmitted_were(mtyp)
int mtyp;
{
	switch(mtyp)
	{
	case PM_WEREJACKAL:
	case PM_HUMAN_WEREJACKAL:
	case PM_ANUBAN_JACKAL:
	case PM_ANUBITE:
		return PM_WEREJACKAL;
	case PM_WEREWOLF:
	case PM_HUMAN_WEREWOLF:
		return PM_WEREWOLF;
	case PM_WERERAT:
	case PM_HUMAN_WERERAT:
		return PM_WERERAT;
	}
	impossible("Unhandled were-foo transmission %d", mtyp);
	return mtyp;
}

void
new_were(mon)
struct monst *mon;
{
	int pm;
	struct permonst *olddata = mon->data;
	struct obj *otmp;

	pm = counter_were(monsndx(mon->data));
	if(!pm) {
	    impossible("unknown lycanthrope %s.", mon->data->mname);
	    return;
	}
	
	if(is_heladrin(mon->data) && nonthreat(mon))
		return;
	
	if(mon == u.ustuck && u.uswallow)
		expels(mon, mon->data, TRUE);
	else if(u.ustuck == mon)
		u.ustuck = 0;
	
	if(DEADMONSTER(mon) || MIGRATINGMONSTER(mon))
		return;
	
	if(canseemon(mon) && !Hallucination) {
		if(mon->mtyp == PM_BLASPHEMOUS_HAND)
			pline("%s relaxes its gesture.", Monnam(mon));
		else if(mon->mtyp == PM_LURKING_HAND)
			pline("%s adopts a blasphemous gesture.", Monnam(mon));
		else if(mon->mtyp != PM_ANUBITE && mon->mtyp != PM_ANUBAN_JACKAL
		  && !is_eladrin(mon->data) && !is_yochlol(mon->data)
		  && !(mon->mtyp == PM_SELKIE || mon->mtyp == PM_SEAL)
		  && !(mon->mtyp == PM_INCUBUS || mon->mtyp == PM_SUCCUBUS)
		  && !is_duergar(mon)
		) pline("%s changes into %s.", Monnam(mon),
			is_human(&mons[pm]) ? "a human" :
			an(mons[pm].mname+4));
		else pline("%s changes into %s.", Monnam(mon),
			an(mons[pm].mname));
	}

	set_mon_data(mon, pm);
	if (mon->msleeping || !mon->mcanmove) {
	    /* transformation wakens and/or revitalizes */
	    mon->msleeping = 0;
	    mon->mfrozen = 0;	/* not asleep or paralyzed */
	    mon->mcanmove = 1;
	}
	/* regenerate by 1/4 of the lost hit points */
	mon->mhp += (mon->mhpmax - mon->mhp) / 4;
	/* recheck if monster is a lightsource */
	del_light_source(mon->light);
	if (emits_light_mon(mon)) {
		new_light_source(LS_MONSTER, (genericptr_t)mon, emits_light_mon(mon));
	}
	newsym(mon->mx,mon->my);
	if(is_eeladrin(mon->data)){
		struct obj *mw_tmp = MON_WEP(mon);
		struct obj *msw_tmp = MON_SWEP(mon);
		for(otmp = mon->minvent; otmp; otmp = otmp->nobj){
			mon->misc_worn_check &= ~otmp->owornmask;
			if (otmp->owornmask)
				update_mon_intrinsics(mon, otmp, FALSE, FALSE);
			otmp->owornmask = 0L;
			if (otmp == mw_tmp){
				setmnotwielded(mon, mw_tmp);
				MON_NOWEP(mon);
				mon->weapon_check = NO_WEAPON_WANTED;
			}
			if (otmp == msw_tmp){
				setmnotwielded(mon, msw_tmp);
				MON_NOSWEP(mon);
				mon->weapon_check = NO_WEAPON_WANTED;
			}
		}
		if(mon->mtyp == PM_ANCIENT_TEMPEST){
			static int elemtypes[] = {PM_WATER_ELEMENTAL, PM_AIR_ELEMENTAL, PM_LIGHTNING_PARAELEMENTAL, PM_ICE_PARAELEMENTAL};
			struct monst *ltnt;
			for(ltnt = fmon; ltnt; ltnt = ltnt->nmon)
			if(ltnt->mtyp == PM_WIDE_CLUBBED_TENTACLE){
				del_light_source(ltnt->light);
				set_mon_data(ltnt, elemtypes[rn2(4)]);
				newsym(ltnt->mx,ltnt->my);
				if (emits_light_mon(ltnt))
					new_light_source(LS_MONSTER, (genericptr_t)ltnt, emits_light_mon(ltnt));
			}
		}
		m_dowear(mon, TRUE);
		init_mon_wield_item(mon);
	} else if(is_heladrin(mon->data)){
		m_dowear(mon, TRUE);
		init_mon_wield_item(mon);
	} else if(is_duergar(mon)){
		for(otmp = mon->minvent; otmp; otmp = otmp->nobj){
			if(otmp->owornmask && !otmp->oartifact && !((is_weptool(otmp) || otmp->oclass == WEAPON_CLASS) && !check_oprop(otmp, OPROP_NONE))){
				size_and_shape_to_fit(otmp, mon);
			}
		}
		mon_break_armor(mon, FALSE);
	} else mon_break_armor(mon, FALSE);
	possibly_unwield(mon, FALSE);
}

int
were_summon(caller,visible,genbuf)	/* were-creature (even you) summons a horde */
struct monst * caller;
int *visible;			/* number of visible helpers created */
char *genbuf;
{
	register int i, typ;
	register struct monst *mtmp;
	int total = 0;
	boolean yours = (caller == &youmonst);
	boolean summons = (!yours && get_mx(caller, MX_ESUM));

	*visible = 0;
	if(Protection_from_shape_changers && !yours)
		return 0;
	for(i = rnd(5); i > 0; i--) {
	   switch(caller->mtyp) {

		case PM_WERERAT:
		case PM_HUMAN_WERERAT:
			typ = rn2(3) ? PM_SEWER_RAT : rn2(3) ? PM_GIANT_RAT : PM_RABID_RAT ;
			if (genbuf) Strcpy(genbuf, "rat");
			break;
		case PM_WEREJACKAL:
		case PM_HUMAN_WEREJACKAL:
			typ = PM_JACKAL;
			if (genbuf) Strcpy(genbuf, "jackal");
			break;
		case PM_WEREWOLF:
		case PM_HUMAN_WEREWOLF:
			typ = rn2(5) ? PM_WOLF : PM_WINTER_WOLF ;
			if (genbuf) Strcpy(genbuf, "wolf");
			break;
		case PM_ANUBITE:
		case PM_ANUBAN_JACKAL:
			typ = PM_WEREJACKAL;
			if (genbuf) Strcpy(genbuf, "werejackal");
		default:
			continue;
	    }
	    mtmp = makemon(&mons[typ], u.ux, u.uy, summons ? MM_ESUM : NO_MM_FLAGS);
	    if (mtmp) {
			total++;
			if (canseemon(mtmp))
				*visible += 1;
			if (summons)
				mark_mon_as_summoned(mtmp, caller, ESUMMON_PERMANENT, 0);
			if (yours)
				(void) tamedog(mtmp, (struct obj *) 0);
	    }			
	}
	return total;
}

void
you_were()
{
	char qbuf[QBUFSZ];

	if (Unchanging || (u.umonnum == u.ulycn)) return;
	if (Polymorph_control) {
	    /* `+4' => skip "were" prefix to get name of beast */
	    Sprintf(qbuf, "Do you want to change into %s? ",
		    an(mons[u.ulycn].mname+4));
	    if(yn(qbuf) == 'n') return;
	}
	(void) polymon(u.ulycn);
}

void
you_unwere(purify)
boolean purify;
{
	if (purify) {
	    You_feel("purified.");
	    u.ulycn = NON_PM;	/* cure lycanthropy */
	}
	if (!Unchanging && is_were(youmonst.data) &&
		(!Polymorph_control || yn("Remain in beast form?") == 'n'))
	    rehumanize();
}

#endif /* OVLB */

/*were.c*/
