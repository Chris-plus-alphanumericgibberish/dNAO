/*	SCCS Id: @(#)mondata.c	3.4	2003/06/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#include "horrordata.h"
/*	These routines provide basic data for any type of monster. */
STATIC_DCL void FDECL(set_template_data, (struct permonst *, struct permonst *, int));
STATIC_DCL struct permonst * FDECL(permonst_of, (int, int));

char * nameless_horror_name;

#ifdef OVLB

/* saves the index number of each part of the permonst array to itself */
void
id_permonst()
{
	int i;
	if (mons->mtyp == NON_PM)
	for (i = 0; i < NUMMONS; i++)
		mons[i].mtyp = i;
	return;
}

/* 
 * safely sets mon->data from an mtyp
 */
void
set_mon_data(mon, mtyp)
struct monst *mon;
int mtyp;
{
	struct permonst * bas;
	struct permonst * ptr;
	/* players in their base form are a very special case */
	if (mon == &youmonst && (mtyp == u.umonster)) {
		ptr = &upermonst;
		/* I'm not sure what bas should be, if it were to be needed */
	}
	/* horrors are a special case, and have memory allocated on a per-monster basis */
	else if (is_horror(&mons[mtyp])) {
		ptr = &(EHOR(mon)->currhorrordata);
		bas = &(EHOR(mon)->basehorrordata);
		if (mtyp == PM_NAMELESS_HORROR) {
			ptr->mname = EHOR(mon)->randname;
			bas->mname = EHOR(mon)->randname;
		}
	}
	/* eveything else has permonst memory assigned by permonst_of */
	else {
		ptr = permonst_of(mtyp, get_template(mon));
		bas = &mons[mtyp];
	}
	/* adjust permonst if needed */
	if (mon != &youmonst && templated(mon))
		set_template_data(bas, ptr, get_template(mon));
	/* set monster data */
	set_mon_data_core(mon, ptr);
	return;
}

/*
 * safely sets mon->data from an existing data pointer
 */
void
set_mon_data_core(mon, ptr)
struct monst *mon;
struct permonst * ptr;
{
	int i;

	/* data and type */
	mon->data = ptr;
	mon->mtyp = ptr->mtyp;

	/* resistances */
	mon->mintrinsics[0] = (ptr->mresists & MR_MASK);
	if(is_half_dragon(ptr)){
		/*
			Store half dragon breath type in mvar_hdBreath
		*/
		if(is_half_dragon(ptr)){
			static const int half_dragon_types[] = { AD_COLD, AD_FIRE, AD_SLEE, AD_ELEC, AD_DRST, AD_ACID };
			if (!mon->mvar_hdBreath)
				mon->mvar_hdBreath = half_dragon_types[rn2(6)];
			switch (mon->mvar_hdBreath){
				case AD_COLD:
					mon->mintrinsics[(COLD_RES-1)/32] |= (1 << (COLD_RES-1)%32);
				break;
				case AD_FIRE:
					mon->mintrinsics[(FIRE_RES-1)/32] |= (1 << (FIRE_RES-1)%32);
				break;
				case AD_SLEE:
					mon->mintrinsics[(SLEEP_RES-1)/32] |= (1 << (SLEEP_RES-1)%32);
				break;
				case AD_ELEC:
					mon->mintrinsics[(SHOCK_RES-1)/32] |= (1 << (SHOCK_RES-1)%32);
				break;
				case AD_DRST:
					mon->mintrinsics[(POISON_RES-1)/32] |= (1 << (POISON_RES-1)%32);
				break;
				case AD_ACID:
					mon->mintrinsics[(ACID_RES-1)/32] |= (1 << (ACID_RES-1)%32);
				break;
				case AD_MAGM:
					mon->mintrinsics[(ANTIMAGIC-1)/32] |= (1 << (ANTIMAGIC-1)%32);
				break;
			}
		} else if(is_boreal_dragoon(ptr)){
			static const int boreal_dragon_types[] = { AD_COLD, AD_FIRE, AD_MAGM, AD_PHYS };
			if (!mon->mvar_hdBreath)
				mon->mvar_hdBreath = boreal_dragon_types[rn2(4)];
			switch (mon->mvar_hdBreath){
				case AD_COLD:
					mon->mintrinsics[(COLD_RES-1)/32] |= (1 << (COLD_RES-1)%32);
				break;
				case AD_FIRE:
					mon->mintrinsics[(FIRE_RES-1)/32] |= (1 << (FIRE_RES-1)%32);
				break;
				case AD_MAGM:
					mon->mintrinsics[(ANTIMAGIC-1)/32] |= (1 << (ANTIMAGIC-1)%32);
				break;
				case AD_PHYS:
				break;
			}
		}
	}
#define set_mintrinsic(ptr_condition, intrinsic) \
	if ((ptr_condition))	{ mon->mintrinsics[((intrinsic)-1)/32] |=  (1L<<((intrinsic)-1)%32); } \
	else					{ mon->mintrinsics[((intrinsic)-1)/32] &= ~(1L<<((intrinsic)-1)%32); }

	/* other intrinsics */
	set_mintrinsic(species_flies(mon->data), FLYING);
	set_mintrinsic(species_floats(mon->data), LEVITATION);
	set_mintrinsic(species_swims(mon->data), SWIMMING);
	set_mintrinsic(species_displaces(mon->data), DISPLACED);
	set_mintrinsic(species_passes_walls(mon->data), PASSES_WALLS);
	set_mintrinsic(species_regenerates(mon->data), REGENERATION);
	set_mintrinsic(species_perceives(mon->data), SEE_INVIS);
	set_mintrinsic(species_teleports(mon->data), TELEPORT);
	set_mintrinsic(species_controls_teleports(mon->data), TELEPORT_CONTROL);
	set_mintrinsic(species_is_telepathic(mon->data), TELEPAT);
#undef set_mintrinsic
	for(int i = 0; i < MPROP_SIZE; i++){
		mon->mintrinsics[i] |= mon->acquired_trinsics[i];
	}
    return;
}

void
give_mintrinsic(mon, intrinsic)
struct monst * mon;
long intrinsic;
{
	mon->mintrinsics[((intrinsic)-1)/32] |=  (1L<<((intrinsic)-1)%32);
	mon->acquired_trinsics[((intrinsic)-1)/32] |=  (1L<<((intrinsic)-1)%32);
}

void
remove_mintrinsic(mon, intrinsic)
struct monst * mon;
long intrinsic;
{
	mon->mintrinsics[((intrinsic)-1)/32] &= ~(1L<<((intrinsic)-1)%32);
	mon->acquired_trinsics[((intrinsic)-1)/32] &= ~(1L<<((intrinsic)-1)%32);
}

//Note: intended to be mental things relating to a faction a monster belongs to
void
set_faction(mtmp, faction)
struct monst * mtmp;
int faction;
{
	mtmp->mfaction = faction;
	set_mon_data(mtmp, mtmp->mtyp); //Should be unlikely to actually result in a change in data.
	return;
}

//Note: intended to be physical things like zombification.
void
set_template(mtmp, template)
struct monst * mtmp;
int template;
{
	mtmp->mtemplate = template;
	set_mon_data(mtmp, mtmp->mtyp);
	return;
}

void
set_template_data(base, ptr, template)
struct permonst * base;
struct permonst * ptr;
int template;
{
	int mtyp = base->mtyp;
	/* copy original */
	*ptr = *base;

#define MT_ITEMS (MT_GREEDY|MT_JEWELS|MT_COLLECT|MT_MAGIC)

	/* make changes to the permonst as necessary */
	switch (template)
	{
	case ZOMBIFIED:
		/* flags: */
		ptr->mflagsm |= (MM_BREATHLESS);
		ptr->mflagst |= (MT_MINDLESS | MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_ANIMAL | MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsg |= (MG_RPIERCE | MG_RBLUNT);
		ptr->mflagsg &= ~(MG_RSLASH | MG_INFRAVISIBLE);
		ptr->mflagsa |= (MA_UNDEAD);
		/* defense: */
		ptr->nac += 4;
		ptr->dac += -2;	/* penalty to dodge AC */
		ptr->hdr += 2;
		ptr->bdr += 2;
		ptr->gdr += 2;
		ptr->ldr += 2;
		ptr->fdr += 2;
		/* resists: */
		ptr->mresists |= (MR_COLD | MR_SLEEP | MR_POISON);
		/* misc: */
		ptr->msound = MS_SILENT;
		/* speed: 0.50x, min 6 */
		if (ptr->mmove > 6)
			ptr->mmove = max(6, ptr->mmove / 2);
		break;
	case SKELIFIED:
		/* flags: */
		ptr->geno |= (G_NOCORPSE);
		ptr->mflagsm |= (MM_BREATHLESS);
		ptr->mflagst |= (MT_MINDLESS | MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_ANIMAL | MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsg |= (MG_RPIERCE | MG_RSLASH);
		ptr->mflagsg &= ~(MG_RBLUNT | MG_INFRAVISIBLE);
		ptr->mflagsa |= (MA_UNDEAD);
		/* defense: */
		ptr->nac += 2;
		/* resists: */
		ptr->mresists |= (MR_COLD | MR_SLEEP | MR_POISON);
		/* misc: */
		if (mtyp != PM_ECHO)
			ptr->msound = MS_BONES;
		/* speed: 0.75x, min 6 */
		if (ptr->mmove > 6)
			ptr->mmove = max(6, ptr->mmove * 3 / 4);
		break;
	case CRYSTALFIED:
		/* flags: */
		ptr->geno |= (G_NOCORPSE);
		ptr->mflagsm |= (MM_BREATHLESS);
		ptr->mflagst |= (MT_MINDLESS | MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_ANIMAL | MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsg |= (MG_RPIERCE | MG_RSLASH);
		ptr->mflagsg &= ~(MG_RBLUNT | MG_INFRAVISIBLE);
		ptr->mflagsa |= (MA_UNDEAD);
		ptr->mflagsb |= (MB_INDIGESTIBLE);
		/* defense: */
		ptr->nac += 10;
		ptr->dac += 6;
		ptr->hdr += 8;
		ptr->bdr += 8;
		ptr->gdr += 8;
		ptr->ldr += 8;
		ptr->fdr += 8;
		/* resists: */
		ptr->mresists |= (MR_COLD | MR_SLEEP | MR_POISON);
		/* misc: */
		ptr->msound = MS_SILENT;
		break;
	case FRACTURED:
		/* flags: */
		ptr->mflagsm |= (MM_BREATHLESS);
		ptr->mflagst |= (MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsb |= (MB_NOEYES|MB_INDIGESTIBLE);
		ptr->mflagsg &= ~(MG_INFRAVISIBLE);
		ptr->mflagsa |= (MA_UNDEAD);
		break;
	case VAMPIRIC:
		/* flags: */
		ptr->mflagsm |= (MM_BREATHLESS);
		ptr->mflagst |= (MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsg &= ~(MG_INFRAVISIBLE);
		ptr->mflagsa |= (MA_UNDEAD | MA_VAMPIRE);
		/* resists: */
		ptr->mresists |= (MR_SLEEP | MR_POISON);	/* individual monsters gain cold res at mlev >= 10 */
		break;
	case ILLUMINATED:
		/* flags: */
		ptr->mflagsg |= (MG_HATESUNHOLY);
		ptr->mflagsg &= ~(MG_HATESHOLY);
		ptr->mflagsa |= (MA_MINION);
		break;
	case PSEUDONATURAL:
		/* flags */
		ptr->mflagsa |= (MA_PRIMORDIAL);
		/* resists: */
		ptr->mresists |= (MR_POISON);
		break;
	case TOMB_HERD:
		/* flags: */
		ptr->geno |= (G_NOCORPSE);
		ptr->mflagsm |= (MM_TENGTPORT);
		ptr->mflagst |= (MT_HOSTILE);
		ptr->mflagsg &= ~(MG_INFRAVISIBLE);
		ptr->mflagsb |= (MB_INDIGESTIBLE);
		/* defense: */
		ptr->nac += 6;
		/* resists: */
		ptr->mresists |= (MR_FIRE | MR_COLD | MR_SLEEP | MR_POISON | MR_STONE | MR_DRAIN | MR_SICK | MR_MAGIC);
		break;
	case YITH:
		/* attacks only */
		break;
	case CRANIUM_RAT:
		/* defense: */
		ptr->dac += 4;
		break;
	case MISTWEAVER:
		/* flags */
		ptr->mflagsb |= (MB_CAN_AMULET|MB_NOHEAD|MB_STRONG);
		ptr->mresists |= (MR_POISON|MR_ACID|MR_COLD|MR_FIRE|MR_ELEC);
		ptr->mflagsm |= (MM_AMPHIBIOUS);
		ptr->mflagst |= (MT_CARNIVORE);
		ptr->mflagsv |= (MV_ECHOLOCATE|MV_SCENT);
		ptr->mflagsa |= (MA_ANIMAL|MA_PLANT|MA_PRIMORDIAL);
		break;
	case DELOUSED:
		/* flags */
		ptr->mflagsa &= ~(MA_PRIMORDIAL);
		break;
	case M_BLACK_WEB:
		/* attacks only */
		break;
	case M_GREAT_WEB:
		/* attacks only */
		break;
	}
#undef MT_ITEMS

	/* adjust attacks in the permonst */
	extern struct attack noattack;
	boolean special = FALSE;
	struct attack * attk;
	boolean insert;
	int i, j;
	for (i = 0; i < NATTK; i++)
	{
		attk = &(ptr->mattk[i]);
		insert = FALSE;

		/* some templates completely skip specific attacks */
		while ((template == ZOMBIFIED || template == SKELIFIED || template == CRYSTALFIED) &&
			(
			attk->aatyp == AT_SPIT ||
			attk->aatyp == AT_BREA ||
			attk->aatyp == AT_GAZE ||
			attk->aatyp == AT_ARRW ||
			attk->aatyp == AT_MMGC ||
			attk->aatyp == AT_TNKR ||
			attk->aatyp == AT_SRPR ||
			attk->aatyp == AT_BEAM ||
			attk->aatyp == AT_MAGC ||
			(attk->aatyp == AT_TENT && template == SKELIFIED) ||
			attk->aatyp == AT_GAZE ||
			attk->aatyp == AT_WDGZ ||
			(attk->aatyp == AT_NONE && attk->adtyp == AD_PLYS)
			)
			)
		{
			/* shift all further attacks forwards one slot, and make last one all 0s */
			for (j = 0; j < (NATTK - i); j++)
				attk[j] = attk[j + 1];
			attk[j] = noattack;
		}

		/* if creatures don't have eyes, some gaze attacks are impossible */
		if ((attk->aatyp == AT_GAZE || attk->aatyp == AT_WDGZ) && !haseyes(ptr))
		{
			boolean needs_magr_eyes;
			getgazeinfo(attk->aatyp, attk->adtyp, ptr, &needs_magr_eyes, (boolean *)0, (boolean *)0);
			if (needs_magr_eyes == TRUE)
			{
				/* remove attack */
				/* shift all further attacks forwards one slot, and make last one all 0s */
				for (j = 0; j < (NATTK - i); j++)
					attk[j] = attk[j + 1];
				attk[j] = noattack;
			}
		}

		/* some templates want to adjust existing attacks, or add additional attacks */
#define insert_okay (!special && (is_null_attk(attk) || \
	((attk->aatyp > AT_HUGS && !weapon_aatyp(attk->aatyp) \
	&& !(attk->aatyp == AT_BREA && ptr->mlet == S_DRAGON)) || attk->aatyp == AT_NONE)) \
	&& (insert = TRUE))
#define end_insert_okay (!special && (is_null_attk(attk) || attk->aatyp == AT_NONE) && (insert = TRUE))
#define maybe_insert() if(insert) {for(j=NATTK-i-1;j>0;j--)attk[j]=attk[j-1];*attk=noattack;i++;}
		/* zombies/skeletons get a melee attack if they don't have any (likely due to disallowed aatyp) */
		if ((template == ZOMBIFIED || template == SKELIFIED) && (
			i == 0 && (!nolimbs(ptr) || has_head(ptr)) && (
			is_null_attk(attk) ||
			(attk->aatyp == AT_NONE || attk->aatyp == AT_BOOM)
			) && (insert = TRUE)
			))
		{
			maybe_insert()
				attk->aatyp = !nolimbs(ptr) ? AT_CLAW : AT_BITE;
			attk->adtyp = AD_PHYS;
			attk->damn = ptr->mlevel / 10 + (template == ZOMBIFIED ? 1 : 2);
			attk->damd = max(ptr->msize * 2, 4);
		}

		/* skeletons get a paralyzing touch */
		if (template == SKELIFIED && (
			insert_okay
			))
		{
			maybe_insert();
			attk->aatyp = AT_TUCH;
			attk->adtyp = AD_SLOW;
			attk->damn = 1;
			attk->damd = max(ptr->msize * 2, 4);
			special = TRUE;

		}
		/* vitreans get a cold touch */
		if (template == CRYSTALFIED && (
			insert_okay
			))
		{
			maybe_insert();
			attk->aatyp = AT_TUCH;
			attk->adtyp = AD_ECLD;
			attk->damn = max(1, min(10, ptr->mlevel / 3));
			attk->damd = 8;
			special = TRUE;
		}
		if (template == MISTWEAVER && (
			end_insert_okay
			))
		{
			maybe_insert();
			attk->aatyp = AT_TENT;
			attk->adtyp = AD_DRST;
			attk->damn = 4;
			attk->damd = 4;
			special = TRUE;
		}
		/* fractured turn their claws into glass shards */
		if (template == FRACTURED && (
			(attk->aatyp == AT_CLAW && (
			attk->adtyp == AD_PHYS ||
			attk->adtyp == AD_SQUE ||
			attk->adtyp == AD_SAMU
			))
			|| insert_okay
			))
		{
			maybe_insert();
			attk->aatyp = AT_CLAW;
			attk->adtyp = AD_GLSS;
			attk->damn = max(ptr->mlevel / 10 + 1, attk->damn);
			attk->damd = max(ptr->msize * 2, max(attk->damd, 4));
			special = TRUE;
		}
		/* vampires' bites are vampiric */
		if (template == VAMPIRIC && (
			(attk->aatyp == AT_BITE)
			|| insert_okay
			))
		{
			maybe_insert();
			attk->aatyp = AT_BITE;
			attk->adtyp = AD_VAMP;
			attk->damn = max(1, attk->damn);
			attk->damd = max(4, max(ptr->msize * 2, attk->damd));
			special = TRUE;
		}
		/* pseudonatural's bites become int-draining tentacles */
		if (template == PSEUDONATURAL && (
			(attk->aatyp == AT_BITE)
			|| insert_okay
			))
		{
			maybe_insert();
			attk->aatyp = AT_TENT;
			attk->adtyp = AD_DRIN;
			attk->damn = 1;
			attk->damd = 4;
			special = TRUE;
		}
		/* tomb herd's attacks are generally stronger */
		if (template == TOMB_HERD && (
			!is_null_attk(attk))
			)
		{
			if (attk->damn < 3)
				attk->damd += 2;
			else
				attk->damn++;
		}
		/* tomb herd also gets an abduction attack */
		if (template == TOMB_HERD && (
			insert_okay
			))
		{
			maybe_insert();
			attk->aatyp = AT_TUCH;
			attk->adtyp = AD_ABDC;
			attk->damn = 1;
			attk->damd = 1;
			special = TRUE;
		}
		/* yith gain spellcasting */
		if (template == YITH && (
			end_insert_okay
			))
		{
			maybe_insert();
			attk->aatyp = AT_MAGC;
			attk->adtyp = AD_SPEL;
			attk->damn = 2;
			attk->damd = 6;
			special = TRUE;
		}
		/* cranium rats gain psionic spellcasting */
		if (template == CRANIUM_RAT && (
			end_insert_okay
			))
		{
			maybe_insert();
			attk->aatyp = AT_MAGC;
			attk->adtyp = AD_PSON;
			attk->damn = 0;
			attk->damd = 15;
			special = TRUE;
		}
		/* monsters that have mastered the black web gain shadow blades */
		if (template == M_BLACK_WEB && (
			insert_okay
			))
		{
			maybe_insert();
			attk->aatyp = AT_SRPR;
			attk->adtyp = AD_SHDW;
			attk->damn = 4;
			attk->damd = 8;
			special = TRUE;
		}
		if (template == M_GREAT_WEB && (
			insert_okay
			))
		{
			maybe_insert();
			attk->aatyp = AT_5SQR;
			attk->adtyp = AD_SHDW;
			attk->damn = 8;
			attk->damd = 8;
			special = TRUE;
		}
	}
#undef insert_okay
#undef end_insert_okay
#undef maybe_insert
	return;
}

/* returns TRUE if mtyp and mtemplate are compatible */
boolean
mtemplate_accepts_mtyp(mtemplate, mtyp)
int mtemplate;
int mtyp;
{
	if (mtyp == NON_PM)
		return FALSE;
		
	struct permonst * ptr = &mons[mtyp];
	switch(mtemplate)
	{
	case ZOMBIFIED:
		/* basic undead check */
		return can_undead(ptr);
	case SKELIFIED:
		/* basic undead check */
		/* we don't have a way to check if a permonst has a skeleton ??? */
		return can_undead(ptr);
	case CRYSTALFIED:
		/* anything goes */
		return TRUE;
	case FRACTURED:
		/* kamerel are particularly vulnerable, but can afflict anything with eyes */
		return haseyes(ptr);
	case VAMPIRIC:
		/* must have blood */
		return has_blood(ptr);
	case ILLUMINATED:
		/* needs a soul -- not nonliving */
		return !nonliving(ptr);
	case INCUBUS_FACTION:
	case SUCCUBUS_FACTION:
		/* these are actually much more like proper factions -- leave be for now */
		return TRUE;
	case PSEUDONATURAL:
		/* basic undead check; good enough */
		return can_undead(ptr);
	case TOMB_HERD:
		/* anything goes */
		return TRUE;
	case YITH:
		/* must have had a mind for the yith to have swapped with */
		return !mindless(ptr);
	case CRANIUM_RAT:
		/* is a rodent */
		return is_rat(ptr);
	case MISTWEAVER:
		/* could be a worshipper of the Goat */
		return !(nonliving(ptr) || is_whirly(ptr) || noncorporeal(ptr));
	case DELOUSED:
		/* had a louse */
		return is_delouseable(ptr);
	case M_BLACK_WEB:
	case M_GREAT_WEB:
		/* ??? */
		return TRUE;
	}
	/* default fall through -- allow all */
	return TRUE;
}

/* 
 * Returns a pointer to the appropriate permonst structure for the monster parameters given
 * 
 * needs an mtyp; template optional
 * Do not call with mtyp==NON_PM unless you are intending to get PM_PLAYERMON
 * 
 * This function is responsible for allocating memory for new permonsts!
 */
struct permonst *
permonst_of(mtyp, template)
int mtyp;
int template;
{
	static struct permonst * monsarrays[NUMMONS][MAXTEMPLATE] = { 0 };
	struct permonst * ptr;
	int t_index = template - 1;	/* first template is 1-indexed, but we want 0-indexed */

	/* player is special, and has no handling for derived statblocks */
	if (mtyp == PM_PLAYERMON) {
		impossible("attempting to find permonst of playermon, template %d", template);
		return &upermonst;
	}
	
	/* validate mtyp */
	if (mtyp > NUMMONS || mtyp < 0) {
		impossible("Can not get permonst for mtyp=%d!", mtyp);
	}
	/* simplest case: return the common mons[] array */
	if (!template)
		return &mons[mtyp];

	/* next case: we have already generated that particular statblock */
	if (monsarrays[mtyp][t_index] != (struct permonst *)0) {
		return monsarrays[mtyp][t_index];
	}

	/* final case: we need to generate the statblock */
	/* allocate memory */
	monsarrays[mtyp][t_index] = (struct permonst *)malloc(sizeof(struct permonst));
	ptr = monsarrays[mtyp][t_index];
	return ptr;
}

void
make_horror(horror, target_level, level_bonus)
struct permonst * horror;
int target_level;
int level_bonus;
{
	extern int monstr[];
	struct attack* attkptr;
	int horrorattacks;
	int i;

#define get_random_of(x) (x)[rn2(SIZE((x)))]

	do {
		/* what a horrible night to have a curse */
		horror->mlevel = 1;							/* low starting level so difficulty is based on other things*/
		horror->mmove = rn2(7) * 2 + 6;				/* slow to very fast */
		switch (rn2(4)){
		case 0:
			horror->nac = rn2(21) + (rn2(3) ? 0 : +10);/* any AC */
			horror->dac = 0;
			horror->pac = 0;
			break;
		case 1:
			horror->nac = 0;
			horror->dac = rn2(21) + (rn2(3) ? 0 : +10);/* any AC */
			horror->pac = 0;
			break;
		case 2:
			horror->nac = 0;
			horror->dac = 0;
			horror->pac = rn2(21) + (rn2(3) ? 0 : +10);/* any AC */
			break;
		case 3:
			/* any AC (combo is slightly better) */
			horror->nac = rn2(8) + (rn2(9) ? 0 : +5);
			horror->dac = rn2(8) + (rn2(9) ? 0 : +5);
			horror->pac = rn2(8) + (rn2(9) ? 0 : +5);
			break;
		}
		switch (rn2(4)){
		case 0:
			horror->hdr = 0;
			horror->bdr = 0;
			horror->gdr = 0;
			horror->ldr = 0;
			horror->fdr = 0;
			break;
		case 1:{
				   schar dr = rnd(3);
				   horror->hdr = dr;
				   horror->bdr = dr;
				   horror->gdr = dr;
				   horror->ldr = dr;
				   horror->fdr = dr;
		}break;
		case 2:{
				   schar dr = rnd(6);
				   horror->hdr = dr;
				   horror->bdr = dr;
				   horror->gdr = dr;
				   horror->ldr = dr;
				   horror->fdr = dr;
		}break;
		case 3:
			horror->hdr = rn2(10);
			horror->bdr = rn2(10);
			horror->gdr = rn2(10);
			horror->ldr = rn2(10);
			horror->fdr = rn2(10);
			break;
		}
		horror->spe_hdr = 0;
		horror->spe_bdr = 0;
		horror->spe_gdr = 0;
		horror->spe_ldr = 0;
		horror->spe_fdr = 0;

		horror->mr = rn2(11) * 10;				/* varying amounts of MR */
		horror->maligntyp = d(2, 9) - 10;			/* any alignment */

		/* attacks...?  */
		horrorattacks = 0;
		extern struct attack noattack;
		for (i = 0; i < NATTK; i++) {
			horror->mattk[i] = noattack;
		}

		/* always start with weapon attacks... if it gets any */
		if (!rn2(4)) {
			attkptr = &horror->mattk[horrorattacks];

			attkptr->aatyp = get_random_of(randWeaponAttackTypes);
			attkptr->adtyp = get_random_of(randWeaponDamageTypes);
			attkptr->damn = d(1, 3);						/*  1 -  3 */
			attkptr->damd = rn2(5 - attkptr->damn) * 2 + 6;	/*  6 - 12 by 2s */

			/* fixups */
			switch (attkptr->aatyp)
			{
			case AT_SRPR:
				attkptr->adtyp = get_random_of(randRapierDamageTypes);
				attkptr->damn += 1;
				break;
			case AT_HODS:
				attkptr->adtyp = AD_HODS;
				break;
			case AT_DEVA:
				attkptr->adtyp = AD_PHYS;
				attkptr->damn = 1;
				break;
			}
			horrorattacks++;

			/* possibly make more identical attacks */
			while (!rn2(3) && horrorattacks<6) {
				attkptr = &horror->mattk[horrorattacks];
				*attkptr = *(attkptr - 1);

				if (attkptr->aatyp == AT_WEAP)
					attkptr->aatyp = AT_XWEP;
				else if (attkptr->aatyp == AT_XWEP)
					attkptr->aatyp = AT_MARI;

				horrorattacks++;
			}
		}
		/* get some more melee attacks in here (this will bring it up to at least 2, with 2/3 odds of at least 3) */
		while ((!rn2(horrorattacks) || !rn2(3)) && horrorattacks<NATTK) {
			attkptr = &horror->mattk[horrorattacks];
			if (rn2(7)) {
				attkptr->aatyp = get_random_of(randMeleeAttackTypes);
				attkptr->adtyp = rn2(3) ? get_random_of(randSpecialDamageTypes) : get_random_of(randWeaponDamageTypes);
			}
			else {
				attkptr->aatyp = (rn2(3) ? AT_TUCH : rn2(3) ? AT_LRCH : AT_5SQR);
				attkptr->adtyp = get_random_of(randTouchDamageTypes);
			}
			attkptr->damn = 1 + d(1, 2) + rn2(2)*rn2(3);	/* 2 -  5, trailing right */
			attkptr->damd = rn2(5 - attkptr->damn) * 2 + 6;	/* 6 - 10, by 2s */

			/* sometimes consolidate into a high-variance attack */
			if (!rn2(4)) {
				int n = 1 + rn2(2)*rn2(2);
				attkptr->damd = (attkptr->damn * attkptr->damd) / n;
				attkptr->damn = n;
			}

			horrorattacks++;
		}
		/* chance of getting special, hard-hitting melee attacks */
		if (horrorattacks <= 2 || (!rn2(8) && horrorattacks < NATTK-1)) {
			attkptr = &horror->mattk[horrorattacks];

			attkptr->aatyp = get_random_of(randSpecialAttackTypes);
			attkptr->adtyp = get_random_of(randRendDamageTypes);
			attkptr->damn = d(2, 3);						/* 2 -  6 */
			attkptr->damd = rn2(5 - attkptr->damn) * 2 + 6;	/* 6 - 10 by 2s */

			/* fixups */
			if (attkptr->aatyp == AT_ENGL) {
				attkptr->adtyp = get_random_of(randEngulfDamageTypes);
				/* engulf attacks can be stronger */
				attkptr->damn += 2 - attkptr->damn / 3;
				attkptr->damd += 2;
			}
			else if ((attkptr - 1)->aatyp != AT_XWEP) {
				/* duplicate previous attack and insert it */
				*(attkptr + 1) = *attkptr;
				*(attkptr) = *(attkptr - 1);
				(attkptr - 0)->damd -= 2;
				(attkptr - 1)->damd -= 2;
				horrorattacks++;
			}
			horrorattacks++;
		}
		/* chance of getting ranged attacks */
		while (!rn2(horrorattacks / 2) && horrorattacks < NATTK) {
			attkptr = &horror->mattk[horrorattacks];

			do {
				i = get_random_of(randRangedAttackTypes);
			} while (attacktype(horror, i) && rn2(40));
			attkptr->aatyp = i;

			attkptr->damn = d(2, 3);			/*  2 -  6 */
			attkptr->damd = rn2(3) * 2 + 6;		/*  6 - 10, by 2s */

			switch (attkptr->aatyp) {
			case AT_SPIT:
				attkptr->adtyp = get_random_of(randSpitDamageTypes);
				attkptr->damd = 6;
				break;
			case AT_ARRW:
				attkptr->adtyp = get_random_of(randArrowDamageTypes);
				attkptr->damn = 1;
				attkptr->damd = d(2, 3);
				break;
			case AT_BREA:
				attkptr->adtyp = get_random_of(randBreathDamageTypes);
				attkptr->damn += rnd(3);
				attkptr->damd += 2;
				break;
			case AT_BEAM:
				attkptr->adtyp = get_random_of(randBeamDamageTypes);
				break;
			case AT_GAZE:
				attkptr->adtyp = get_random_of(randGazeDamageTypes);
				break;
			case AT_WDGZ:
				attkptr->adtyp = get_random_of(randGazeDamageTypes);
				attkptr->damn = rnd(3);			/* reduce to 1-3 */
				attkptr->damd = rn2(3) * 2 + 4;	/* reduce to 4-8 by 2s */
				break;
			case AT_MAGC:
				attkptr->adtyp = get_random_of(randMagicDamageTypes);
				if (attkptr->adtyp == AD_SPEL || attkptr->adtyp == AD_CLRC) {
					attkptr->damn = !rn2(3) ? 0 : rnd(4);
					attkptr->damd = 6;
				}
				break;
			}
			/* damage overrides */
			switch (attkptr->adtyp){
			case AD_LUCK:
				attkptr->damn = 1;
				attkptr->damd = rnd(13);
				break;
			case AD_VBLD:
				attkptr->damn = d(1, 3);
				attkptr->damd = d(1, 3);
				break;
			case AD_BLNK:
			case AD_DEAD:
				attkptr->damn = 0;
				attkptr->damd = 0;
				break;
			}
			horrorattacks++;
		}

		horror->msize = !rn2(6) ? MZ_GIGANTIC : rn2(MZ_HUGE + 1);			/* any size */
		horror->cwt = randCorpseWeights[horror->msize];					/* probably moot as it's flagged NOCORPSE */
		horror->cnutrit = randCorpseNut[horror->msize];					/* see above */
		horror->msound = rn2(MS_HUMANOID);								/* any but the specials */
		horror->mresists = 0;


		for (i = 0; i < rnd(6); i++) {
			horror->mresists |= (1 << rn2(10));		/* physical resistances... */
		}
		// for (i = 0; i < rnd(5); i++) {
		// horror->mresists |= (0x100 << rn2(7));	/* 'different' resistances, even clumsy */
		// }
		horror->mconveys = 0;					/* flagged NOCORPSE */

		/*
		* now time for the random flags.  this will likely produce
		* a number of complete trainwreck monsters at first, but
		* every so often something will dial up nasty stuff
		*/
		horror->mflagsm = 0;
		horror->mflagst = 0;
		horror->mflagsb = 0;
		horror->mflagsg = 0;
		horror->mflagsa = 0;
		horror->mflagsv = 0;

		for (i = 0; i < rnd(17); i++) {
			horror->mflagsm |= (1 << rn2(33));		/* trainwreck this way :D */
		}
		for (i = 0; i < rnd(17); i++) {
			horror->mflagst |= (1 << rn2(33));
		}
		for (i = 0; i < rnd(17); i++) {
			horror->mflagsb |= (1 << rn2(33));
		}
		for (i = 0; i < rnd(17); i++) {
			horror->mflagsg |= (1 << rn2(33));
		}
		for (i = 0; i < rnd(17); i++) {
			horror->mflagsa |= (1 << rn2(33));
		}
		for (i = 0; i < rnd(17); i++) {
			horror->mflagsv |= (1 << rn2(33));
		}

		// horror->mflagsb &= ~MB_UNSOLID;			/* no ghosts */
		// horror->mflagsm &= ~MM_WALLWALK;			/* no wall-walkers */

		horror->mflagsg |= MG_NOPOLY;		/* Don't let the player be one of these yet. */

		horror->mflagst |= MT_HOSTILE;

		horror->mflagsg &= ~MG_MERC;				/* no guards */
		horror->mflagst &= ~MT_PEACEFUL;			/* no peacefuls */
		horror->mflagst &= ~MT_COVETOUS;			/* no covetous */
		horror->mflagsa &= ~MA_WERE;				/* no lycanthropes */
		horror->mflagsg &= ~MG_PNAME;				/* not a proper name */

		/* special adjustments for Nameless Horrors */
		if (horror->mtyp == PM_NAMELESS_HORROR) {
			static const char *Vowels[] = { "a", "e", "i", "o", "u", "y",
				"ae", "oe", "oo", "ai", "ou", "\'" };
			static const char *Consonants[] = { "b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "qu", "r", "s", "t", "v", "w", "x", "y", "z",
				"ch", "ll", "sh", "th", "zth", "cl", "kl", "cr", "kr", "gn", "tr", "gh", "bl", "nn", "mm", "\'" };
			int i;
			Strcpy(nameless_horror_name, "");
			for (i = !rn2(3); (i<4 || (rn2(10-i) && rn2(3))); i++) {
				if (!(i % 2) || !rn2(12))
					Strcat(nameless_horror_name, get_random_of(Consonants));
				else
					Strcat(nameless_horror_name, get_random_of(Vowels));
			}
			/* names cannot start with apostrophes */
			while(nameless_horror_name[0] == '\'')
				nameless_horror_name[0] = (get_random_of(Consonants))[0];
			/* names cannot end with apostrophes */
			while (eos(nameless_horror_name)[-1] == '\'')
				eos(nameless_horror_name)[-1] = (get_random_of(Vowels))[0];
			(void)upstart(nameless_horror_name);
			horror->mname = nameless_horror_name;
			horror->mflagsg |= MG_PRINCE;
			horror->mflagsg |= MG_PNAME;
			horror->mflagsv |= MV_EXTRAMISSION;
		}

		/* some cleanup to reduce the trainwreck*/
		if (horror->mflagsm & MM_WALLWALK)
			horror->mflagsm &= ~(MM_TUNNEL | MM_NEEDPICK);
		if (horror->mflagsm & MM_TUNNEL)
			horror->mflagsm &= ~MM_NEEDPICK;
		if (horror->mflagsm & MM_STATIONARY)
			horror->mflagsm &= ~(MM_FLEETFLEE | MM_TUNNEL | MM_NEEDPICK | MM_NOTONL);
		if (horror->mflagst & MT_NOTAKE)
			horror->mflagst &= ~MT_MAID;
		if (horror->mflagst & MT_MINDLESS)
			horror->mflagst &= ~MT_ANIMAL;
		if (horror->mflagst & MT_ANIMAL)
			horror->mflagst &= ~MT_MINDLESS;
		if (horror->mflagsb & MB_MALE)
			horror->mflagsb &= ~(MB_FEMALE | MB_NEUTER);
		if (horror->mflagsb & MB_FEMALE)
			horror->mflagsb &= ~(MB_MALE | MB_NEUTER);
		if (horror->mflagsb & MB_NEUTER)
			horror->mflagsb &= ~(MB_MALE | MB_FEMALE);
		if (horror->mflagsb & MB_NOHEAD)
			horror->mflagsb &= ~(MB_LONGHEAD | MB_LONGNECK);
		if (horror->mflagsv & MV_EXTRAMISSION)
			horror->mflagsv &= ~(MV_LOWLIGHT3 | MV_LOWLIGHT2 | MV_NORMAL | MV_CATSIGHT | MV_DARKSIGHT);
		if (horror->mflagsv & MV_CATSIGHT)
			horror->mflagsv &= ~(MV_LOWLIGHT3 | MV_LOWLIGHT2 | MV_NORMAL | MV_DARKSIGHT);
		if (horror->mflagsv & MV_LOWLIGHT3)
			horror->mflagsv &= ~(MV_LOWLIGHT2 | MV_NORMAL | MV_DARKSIGHT);
		if (horror->mflagsv & MV_LOWLIGHT2)
			horror->mflagsv &= ~(MV_NORMAL | MV_DARKSIGHT);
		if (horror->mflagsv & MV_NORMAL)
			horror->mflagsv &= ~(MV_DARKSIGHT);
		if (!(attacktype(horror, AT_MAGC) || attacktype(horror, AT_MMGC)))
			horror->mflagsg &= ~MG_NOSPELLCOOLDOWN;
		if (horror->mflagsg & MG_PRINCE)
			horror->mflagsg &= ~MG_LORD;

		for (i = 0; i < 2; i++) /* adjust its level and difficulty upwards */
		{
			horror->mlevel = mstrength(horror);
			monstr[monsndx(horror)] = mstrength(horror);
		}
		/* add specified flat level bonus */
		horror->mlevel += level_bonus;
		monstr[monsndx(horror)] = mstrength(horror);

	} while (abs(horror->mlevel - target_level) > 2);

	/* all nameless horrors are set to difficulty 40 */
	if (horror->mtyp == PM_NAMELESS_HORROR)
		monstr[PM_NAMELESS_HORROR] = 40;

#undef get_random_of
	return;
}


#endif /* OVLB */
#ifdef OVL0

struct attack *
attacktype_fordmg(ptr, atyp, dtyp)
struct permonst *ptr;
int atyp, dtyp;
{
    struct attack *a;

    for (a = &ptr->mattk[0]; a < &ptr->mattk[NATTK]; a++){
		if (a->aatyp == atyp && (dtyp == AD_ANY || a->adtyp == dtyp)) 
			return a;
	}

    return (struct attack *)0;
}

boolean
attacktype(ptr, atyp)
struct permonst *ptr;
int atyp;
{
    return attacktype_fordmg(ptr, atyp, AD_ANY) ? TRUE : FALSE;
}

boolean
noattacks(ptr)			/* returns TRUE if monster has no non-passive attacks */
struct permonst *ptr;
{
	int i;

	for (i = 0; i < NATTK; i++)
	if (ptr->mattk[i].aatyp)
		return FALSE;

	return TRUE;
}

int
attackindex(ptr, atyp, dtyp)
struct permonst *ptr;
int atyp, dtyp;
{
	int i;
    for (i = 0; i < NATTK; i++)
	if ((&ptr->mattk[i])->aatyp == atyp && (
		dtyp == AD_ANY || (&ptr->mattk[i])->adtyp == dtyp))
	    return i;
    return -1;
}

#endif /* OVL0 */
#ifdef OVLB

boolean
poly_when_stoned(ptr)
    struct permonst *ptr;
{
    return((boolean)(is_golem(ptr) && ptr->mtyp != PM_STONE_GOLEM && ptr->mtyp != PM_SENTINEL_OF_MITHARDIR &&
	    !(mvitals[PM_STONE_GOLEM].mvflags & G_GENOD && !In_quest(&u.uz))));
	    /* allow G_EXTINCT */
}

boolean
poly_when_golded(ptr)
    struct permonst *ptr;
{
    return((boolean)(is_golem(ptr) && ptr->mtyp != PM_GOLD_GOLEM &&
	    !(mvitals[PM_GOLD_GOLEM].mvflags & G_GENOD && !In_quest(&u.uz))));
	    /* allow G_EXTINCT */
}

boolean
resists_oona(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	switch(u.oonaenergy){
		case AD_ELEC: return resists_elec(mon);
		case AD_FIRE: return resists_fire(mon);
		case AD_COLD: return resists_cold(mon);
		default: return FALSE;
	}
}

boolean
resists_fire(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	return (species_resists_fire(mon) || mon_resistance(mon, FIRE_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Fire_resistance));
}

boolean
resists_cold(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	return (species_resists_cold(mon) || mon_resistance(mon, COLD_RES) || 
		(has_template(mon, VAMPIRIC) && mon->m_lev > 10) ||  
		(mon == u.usteed && u.sealsActive&SEAL_BERITH && Cold_resistance));
}

boolean
resists_sleep(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	return (species_resists_sleep(mon) || mon_resistance(mon, SLEEP_RES) ||  
		((mon) == u.usteed && u.sealsActive&SEAL_BERITH && Sleep_resistance) || (mon)->cham == CHAM_DREAM);
}

boolean
resists_disint(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	return (species_resists_disint(mon) || mon_resistance(mon, DISINT_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Disint_resistance));
}

boolean
resists_elec(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	return (species_resists_elec(mon) || mon_resistance(mon, SHOCK_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Shock_resistance));
}

boolean
resists_poison(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	return (species_resists_poison(mon) || mon_resistance(mon, POISON_RES) || 
		(mon == u.usteed && u.sealsActive&SEAL_BERITH && Poison_resistance));
}

boolean
resists_acid(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	return (species_resists_acid(mon) || mon_resistance(mon, ACID_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Acid_resistance));
}

boolean
resists_ston(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	return (species_resists_ston(mon) || mon_resistance(mon, STONE_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Stone_resistance));
}

boolean
resists_drain(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	return (species_resists_drain(mon) || mon_resistance(mon, DRAIN_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Drain_resistance));
}

boolean
resists_sickness(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	return (species_resists_sickness(mon) || mon_resistance(mon, SICK_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Sick_resistance));
}

boolean
resists_drli(mon)	/* returns TRUE if monster is drain-life resistant */
struct monst *mon;
{
	struct permonst *ptr;
	
	if(!mon) return FALSE;
	ptr = mon->data;

	return (boolean)(is_undead(ptr) || is_demon(ptr) || is_were(ptr) ||
			 species_resists_drain(mon) || 
			 ptr->mtyp == PM_DEATH ||
			 mon_resistance(mon, DRAIN_RES) ||
			 (mon == u.usteed && u.sealsActive&SEAL_BERITH && Drain_resistance));
}

boolean
resists_magm(mon)	/* TRUE if monster is magic-missile resistant */
struct monst *mon;
{
	if(!mon) return FALSE;

	return (species_resists_magic(mon) || mon_resistance(mon, ANTIMAGIC) ||  mon_resistance(mon, NULLMAGIC) ||
		(mon == u.usteed && u.sealsActive&SEAL_BERITH && Antimagic));
}

boolean
resists_death(mon)	/* TRUE if monster resists death magic */
struct monst *mon;
{
	struct permonst *ptr;
	
	if(!mon) return FALSE;
	ptr = mon->data;
	
	if(mon == u.usteed && u.sealsActive&SEAL_BERITH && u.sealsActive&SEAL_OSE) return TRUE;
	
	return nonliving(mon->data) || is_demon(mon->data) || is_angel(mon->data) || is_keter(mon->data);
}

/* TRUE iff monster is resistant to light-induced blindness */
boolean
resists_blnd(mon)
struct monst *mon;
{
	struct permonst *ptr;
	boolean is_you = (mon == &youmonst);
	struct obj *o;
	
	if(!mon) return FALSE;
	ptr = mon->data;
	
	if (is_you ? (NoLightBlind || u.usleep) :
		(mon->mblinded || !mon->mcansee || !haseyes(ptr) ||
		    /* BUG: temporary sleep sets mfrozen, but since
			    paralysis does too, we can't check it */
		    mon->msleeping))
	    return TRUE;
	/* yellow light, Archon; !dust vortex, !cobra, !raven */
	if (dmgtype_fromattack(ptr, AD_BLND, AT_EXPL)
		|| dmgtype_fromattack(ptr, AD_BLND, AT_GAZE)
		|| dmgtype_fromattack(ptr, AD_BLND, AT_WDGZ)
	)
	    return TRUE;

	if (is_you && Blind_res)
		return TRUE;
	else if (!is_you && mon_resistance(mon, BLIND_RES))
		return TRUE;

	return FALSE;
}

/* TRUE iff monster can be blinded by the given attack */
/* Note: may return TRUE when mdef is blind (e.g. new cream-pie attack) */
boolean
can_blnd(magr, mdef, aatyp, obj)
struct monst *magr;		/* NULL == no specific aggressor */
struct monst *mdef;
uchar aatyp;
struct obj *obj;		/* aatyp == AT_WEAP, AT_SPIT */
{
	boolean is_you = (mdef == &youmonst);
	boolean check_visor = FALSE;
	struct obj *o;
	const char *s;

	/* no eyes protect against all attacks for now */
	if (!haseyes(mdef->data))
	    return FALSE;

	switch(aatyp) {
	case AT_EXPL: case AT_BOOM: case AT_GAZE: case AT_WDGZ: case AT_MAGC: case AT_MMGC:
	case AT_BREA: /* assumed to be lightning */
	    /* light-based attacks may be cancelled or resisted */
	    if (magr && magr->mcan)
		return FALSE;
	    return !resists_blnd(mdef);

	case AT_WEAP: case AT_SPIT: case AT_NONE:
	    /* an object is used (thrown/spit/other) */
	    if (obj && (obj->otyp == CREAM_PIE)) {
		if (is_you && Blindfolded)
		    return FALSE;
	    } else if (obj && (obj->otyp == BLINDING_VENOM)) {
		/* all ublindf, including LENSES, protect, cream-pies too */
		if (is_you && (ublindf || u.ucreamed))
		    return FALSE;
		check_visor = TRUE;
	    } else if (obj && (obj->otyp == POT_BLINDNESS)) {
		return TRUE;	/* no defense */
	    } else
		return FALSE;	/* other objects cannot cause blindness yet */
	    if ((magr == &youmonst) && u.uswallow)
		return FALSE;	/* can't affect eyes while inside monster */
	    break;

	case AT_ENGL:
	    if (is_you && (Blindfolded || u.usleep || u.ucreamed))
		return FALSE;
	    if (!is_you && mdef->msleeping)
		return FALSE;
	    break;

	case AT_CLAW:
	    /* e.g. raven: all ublindf, including LENSES, protect */
	    if (is_you && ublindf)
		return FALSE;
	    if ((magr == &youmonst) && u.uswallow)
		return FALSE;	/* can't affect eyes while inside monster */
	    check_visor = TRUE;
	    break;

	case AT_TUCH: case AT_STNG:
	    /* some physical, blind-inducing attacks can be cancelled */
	    if (magr && magr->mcan)
		return FALSE;
	    break;

	default:
	    break;
	}

	/* check if wearing a visor (only checked if visor might help) */
	if (check_visor) {
	    o = (mdef == &youmonst) ? invent : mdef->minvent;
	    for ( ; o; o = o->nobj){
			if ((o->owornmask & W_ARMH) &&
				(o->otyp == find_vhelm() || o->otyp == CRYSTAL_HELM || o->otyp == PLASTEEL_HELM || o->otyp == PONTIFF_S_CROWN)
			) return FALSE;
			if ((o->owornmask & W_ARMC) &&
				(o->otyp == WHITE_FACELESS_ROBE
				|| o->otyp == BLACK_FACELESS_ROBE
				|| o->otyp == SMOKY_VIOLET_FACELESS_ROBE)
			) return FALSE;
		}
	}
	return TRUE;
}

#endif /* OVLB */
#ifdef OVL0

int
m_martial_skill(ptr)
struct permonst * ptr;
{
	switch (ptr->mflagsf & (MF_MARTIAL_B|MF_MARTIAL_S|MF_MARTIAL_E))
	{
	case 0L:           return P_UNSKILLED;
	case MF_MARTIAL_B: return P_BASIC;
	case MF_MARTIAL_S: return P_SKILLED;
	case MF_MARTIAL_E: return P_EXPERT;
	default:
		impossible("multiple martial skill flags for %s", ptr->mname);
		return P_UNSKILLED;
	}
}

boolean
ranged_attk(ptr)	/* returns TRUE if monster can attack at range */
struct permonst *ptr;
{
	register int i, atyp;
	long atk_mask = (1L << AT_BREA) | (1L << AT_SPIT) | (1L << AT_GAZE) | (1L << AT_LRCH) | (1L << AT_LNCK)
					| (1L << AT_MMGC) | (1L << AT_TNKR) | (1L << AT_ARRW) | (1L << AT_BEAM) | (1L << AT_5SQR)
					| (1L << AT_5SBT);

	/* was: (attacktype(ptr, AT_BREA) || attacktype(ptr, AT_WEAP) ||
		attacktype(ptr, AT_SPIT) || attacktype(ptr, AT_GAZE) ||
		attacktype(ptr, AT_MAGC));
	   but that's too slow -dlc
	 */
	for (i = 0; i < NATTK; i++) {
	    atyp = ptr->mattk[i].aatyp;
	    if (atyp >= AT_WEAP) return TRUE;
	 /* assert(atyp < 32); */
	    if ((atk_mask & (1L << atyp)) != 0L) return TRUE;
	}

	return FALSE;
}

/* true iff the type of monster pass through iron bars */
boolean
passes_bars(mon)
struct monst *mon;
{
	struct permonst *mptr = mon->data;

    return (boolean) (mon_resistance(mon,PASSES_WALLS) || amorphous(mptr) ||
		      is_whirly(mptr) || verysmall(mptr) ||
			  dmgtype(mptr, AD_CORR) || (dmgtype(mptr, AD_RUST) && mptr->mtyp != PM_NAIAD ) ||
		      (slithy(mptr) && !bigmonst(mptr)));
}

#endif /* OVL0 */
#ifdef OVL1

boolean
can_track(ptr)		/* returns TRUE if monster can track well */
	register struct permonst *ptr;
{
	if (uwep && (
		uwep->oartifact == ART_EXCALIBUR
		|| uwep->oartifact == ART_SLAVE_TO_ARMOK
		|| (uwep->oartifact == ART_ROD_OF_SEVEN_PARTS && is_demon(ptr) && is_chaotic(ptr))
		) ) return TRUE;
	else
		return((boolean)is_tracker(ptr));
}

#endif /* OVL1 */
#ifdef OVLB

#endif /* OVLB */
#ifdef OVL1

boolean
sticks(mtmp)	/* creature sticks other creatures it hits */
struct monst * mtmp;
{
	register struct permonst * ptr = (mtmp == &youmonst) ? youracedata : mtmp->data;
	/* monsters that can intrinsically do so */
	if (dmgtype(ptr, AD_STCK) || dmgtype(ptr, AD_WRAP) || attacktype(ptr, AT_HUGS))
		return TRUE;
	/* or if wearing the Grappler's Grasp */
	struct obj * gloves = ((mtmp == &youmonst) ? uarmg : which_armor(mtmp, W_ARMG));
	if (gloves && gloves->oartifact == ART_GRAPPLER_S_GRASP)
		return TRUE;

	return FALSE;
}

/* number of horns this type of monster has on its head */
int
num_horns(ptr)
struct permonst *ptr;
{
    switch (monsndx(ptr)) {
    case PM_HORNED_DEVIL:	/* ? "more than one" */
    case PM_MINOTAUR:
    case PM_ASMODEUS:
    case PM_BALROG:
    case PM_LEGION_DEVIL_GRUNT:
    case PM_LEGION_DEVIL_SOLDIER:
    case PM_LEGION_DEVIL_SERGEANT:
    case PM_LEGION_DEVIL_CAPTAIN:
    case PM_GLASYA:
	return 2;
    case PM_WHITE_UNICORN:
    case PM_GRAY_UNICORN:
    case PM_BLACK_UNICORN:
    case PM_KI_RIN:
	return 1;
    default:
	break;
    }
    return 0;
}

struct attack *
dmgtype_fromattack(ptr, dtyp, atyp)
struct permonst *ptr;
int dtyp, atyp;
{
    struct attack *a;

    for (a = &ptr->mattk[0]; a < &ptr->mattk[NATTK]; a++)
	if (a->adtyp == dtyp && (atyp == AT_ANY || a->aatyp == atyp))
	    return a;

    return (struct attack *)0;
}

boolean
dmgtype(ptr, dtyp)
struct permonst *ptr;
int dtyp;
{
    return dmgtype_fromattack(ptr, dtyp, AT_ANY) ? TRUE : FALSE;
}

/* returns the maximum damage a defender can do to the attacker via
 * a passive defense */
int
max_passive_dmg(mdef, magr)
    register struct monst *mdef, *magr;
{
    int	i, dmg = 0;
    uchar adtyp;

    for(i = 0; i < NATTK; i++)
	if(mdef->data->mattk[i].aatyp == AT_NONE ||
		mdef->data->mattk[i].aatyp == AT_BOOM) {
	    adtyp = mdef->data->mattk[i].adtyp;
	    if ((adtyp == AD_ACID && !resists_acid(magr)) ||
		    (adtyp == AD_COLD && !resists_cold(magr)) ||
		    (adtyp == AD_FIRE && !resists_fire(magr)) ||
		    (adtyp == AD_ELEC && !resists_elec(magr)) ||
		    adtyp == AD_PHYS) {
		dmg = mdef->data->mattk[i].damn;
		if(!dmg) dmg = mdef->data->mlevel+1;
		dmg *= mdef->data->mattk[i].damd;
	    } else dmg = 0;

	    return dmg;
	}
    return 0;
}

#endif /* OVL1 */
#ifdef OVL0

int
monsndx(ptr)		/* return an index into the mons array */
	struct	permonst	*ptr;
{
	return ptr->mtyp;
	//register int	i;
	//
	//if (ptr == &upermonst) return PM_PLAYERMON;
	//
	//i = (int)(ptr - &mons[0]);
	//if (i < LOW_PM || i >= NUMMONS) {
	//	/* ought to switch this to use `fmt_ptr' */
	//    panic("monsndx - could not index monster (%lx)",
	//	  (unsigned long)ptr);
	//    return NON_PM;		/* will not get here */
	//}
	//
	//return(i);
}

#endif /* OVL0 */
#ifdef OVL1


int
name_to_mon(in_str)
const char *in_str;
{
	/* Be careful.  We must check the entire string in case it was
	 * something such as "ettin zombie corpse".  The calling routine
	 * doesn't know about the "corpse" until the monster name has
	 * already been taken off the front, so we have to be able to
	 * read the name with extraneous stuff such as "corpse" stuck on
	 * the end.
	 * This causes a problem for names which prefix other names such
	 * as "ettin" on "ettin zombie".  In this case we want the _longest_
	 * name which exists.
	 * This also permits plurals created by adding suffixes such as 's'
	 * or 'es'.  Other plurals must still be handled explicitly.
	 */
	register int i;
	register int mntmp = NON_PM;
	register char *s, *str, *term;
	char buf[BUFSZ];
	int len, slen;

	str = strcpy(buf, in_str);

	if (!strncmp(str, "a ", 2)) str += 2;
	else if (!strncmp(str, "an ", 3)) str += 3;

	slen = strlen(str);
	term = str + slen;

	if ((s = strstri(str, "vortices")) != 0)
	    Strcpy(s+4, "ex");
	/* be careful with "ies"; "priest", "zombies" */
	else if (slen > 3 && !strcmpi(term-3, "ies") &&
		    (slen < 7 || strcmpi(term-7, "zombies")))
	    Strcpy(term-3, "y");
	/* luckily no monster names end in fe or ve with ves plurals */
	else if (slen > 3 && !strcmpi(term-3, "ves"))
	    Strcpy(term-3, "f");

	slen = strlen(str); /* length possibly needs recomputing */

    {
	static const struct alt_spl { const char* name; short pm_val; }
	    names[] = {
	    /* Alternate spellings */
		{ "grey dragon",	PM_GRAY_DRAGON },
		{ "baby grey dragon",	PM_BABY_GRAY_DRAGON },
		{ "grey unicorn",	PM_GRAY_UNICORN },
		{ "grey ooze",		PM_GRAY_OOZE },
		{ "gray-elf",		PM_GREY_ELF },
	    /* Hyphenated names */
		{ "ki rin",		PM_KI_RIN },
		{ "uruk hai",		PM_URUK_HAI },
		{ "orc captain",	PM_ORC_CAPTAIN },
		{ "uruk captain",	PM_URUK_CAPTAIN },
		{ "woodland elf",	PM_WOODLAND_ELF },
		{ "green elf",		PM_GREEN_ELF },
		{ "grey elf",		PM_GREY_ELF },
		{ "gray elf",		PM_GREY_ELF },
		{ "elf lord",		PM_ELF_LORD },
#if 0	/* OBSOLETE */
		{ "high elf",		PM_HIGH_ELF },
#endif
		{ "olog hai",		PM_OLOG_HAI },
		{ "arch lich",		PM_ARCH_LICH },
		/* spacing */
		{ "mindflayer",		PM_MIND_FLAYER },
		{ "master mindflayer",	PM_MASTER_MIND_FLAYER },
	    /* Some irregular plurals */
		{ "incubi",			PM_INCUBUS },
		{ "succubi",		PM_SUCCUBUS },
		{ "violet fungi",	PM_VIOLET_FUNGUS },
		{ "homunculi",		PM_HOMUNCULUS },
		{ "baluchitheria",	PM_BALUCHITHERIUM },
		{ "lurkers above",	PM_LURKER_ABOVE },
		{ "cavemen",		PM_CAVEMAN },
		{ "cavewomen",		PM_CAVEWOMAN },
		{ "djinn",			PM_DJINNI },
		{ "mumakil",		PM_MUMAK },
		{ "erinyes",		PM_ERINYS },
		/* Inappropriate singularization by -ves check above */
		{ "master of thief",	PM_MASTER_OF_THIEVES },
		/* Potential misspellings where we want to avoid falling back
		to the rank title prefix (input has been singularized) */
		{ "master thief",		PM_MASTER_OF_THIEVES },
		{ "master of assassin", PM_MASTER_ASSASSIN },
	    /* human-form weres */
		{ "wererat (human)",	PM_HUMAN_WERERAT },
		{ "werejackal (human)",	PM_HUMAN_WEREJACKAL },
		{ "werewolf (human)",	PM_HUMAN_WEREWOLF },
	    /* end of list */
		{ 0, 0 }
	};
	register const struct alt_spl *namep;

	for (namep = names; namep->name; namep++)
	    if (!strncmpi(str, namep->name, (int)strlen(namep->name)))
		return namep->pm_val;
    }

	for (len = 0, i = LOW_PM; i < NUMMONS; i++) {
	    register int m_i_len = strlen(mons[i].mname);
	    if (m_i_len > len && !strncmpi(mons[i].mname, str, m_i_len)) {
		if (m_i_len == slen) return i;	/* exact match */
		else if (slen > m_i_len &&
			(str[m_i_len] == ' ' ||
			 !strcmpi(&str[m_i_len], "s") ||
			 !strncmpi(&str[m_i_len], "s ", 2) ||
			 !strcmpi(&str[m_i_len], "'") ||
			 !strncmpi(&str[m_i_len], "' ", 2) ||
			 !strcmpi(&str[m_i_len], "'s") ||
			 !strncmpi(&str[m_i_len], "'s ", 3) ||
			 !strcmpi(&str[m_i_len], "es") ||
			 !strncmpi(&str[m_i_len], "es ", 3))) {
		    mntmp = i;
		    len = m_i_len;
		}
	    }
	}
	if (mntmp == NON_PM) mntmp = title_to_mon(str, (int *)0, (int *)0);
	return mntmp;
}

#endif /* OVL1 */
#ifdef OVL2

/* returns 3 values (0=male, 1=female, 2=none) */
int
gender(mtmp)
register struct monst *mtmp;
{
	if (is_neuter(mtmp->data)) return 2;
	return mtmp->female;
}

/* Like gender(), but lower animals and such are still "it". */
/* This is the one we want to use when printing messages. */
int
pronoun_gender(mtmp)
register struct monst *mtmp;
{
	if(is_neuter(mtmp->data) || !canspotmon(mtmp)) return 2;
	if(has_template(mtmp, SKELIFIED) && !Role_if(PM_ARCHEOLOGIST))
		return 2;
	return (humanoid_torso(mtmp->data) || (mtmp->data->geno & G_UNIQ) ||
		type_is_pname(mtmp->data)) ? (int)mtmp->female : 2;
}

#endif /* OVL2 */
#ifdef OVLB

/* used for nearby monsters when you go to another level */
boolean
levl_follower(mtmp)
struct monst *mtmp;
{
	/* monsters with the Amulet--even pets--won't follow across levels */
	if (mon_has_amulet(mtmp)) return FALSE;

	/* some monsters will follow even while intending to flee from you */
	if (mtmp->mtame || mtmp->iswiz || is_fshk(mtmp)) return TRUE;

	/* stalking types follow, but won't when fleeing unless you hold
	   the Amulet */
	return (boolean)((mtmp->data->mflagst & MT_STALK) &&
				(!mtmp->mflee || mtmp->mtyp == PM_BANDERSNATCH || u.uhave.amulet));
}

static const short grownups[][2] = {
	{PM_CHICKATRICE, PM_COCKATRICE},
	{PM_LITTLE_DOG, PM_DOG}, {PM_DOG, PM_LARGE_DOG},
	{PM_HELL_HOUND_PUP, PM_HELL_HOUND},
	{PM_WINTER_WOLF_CUB, PM_WINTER_WOLF},
	{PM_KITTEN, PM_HOUSECAT}, {PM_HOUSECAT, PM_LARGE_CAT},
	{PM_LAMB, PM_SHEEP},
	{PM_MUMAK_CALF, PM_MUMAK},
	{PM_TINY_PSEUDODRAGON, PM_PSEUDODRAGON}, {PM_PSEUDODRAGON, PM_RIDING_PSEUDODRAGON}, {PM_RIDING_PSEUDODRAGON, PM_LARGE_PSEUDODRAGON}, 
		{PM_LARGE_PSEUDODRAGON, PM_WINGED_PSEUDODRAGON}, {PM_WINGED_PSEUDODRAGON, PM_HUGE_PSEUDODRAGON}, {PM_HUGE_PSEUDODRAGON, PM_GIGANTIC_PSEUDODRAGON},
	{PM_PONY, PM_HORSE}, {PM_HORSE, PM_WARHORSE},
	{PM_UNDEAD_KNIGHT, PM_WARRIOR_OF_SUNLIGHT},
	{PM_UNDEAD_MAIDEN, PM_KNIGHT_OF_THE_PRINCESS_S_GUARD},
	{PM_BLUE_SENTINEL, PM_DARKMOON_KNIGHT},
	{PM_UNDEAD_REBEL, PM_OCCULTIST},
	{PM_KOBOLD, PM_LARGE_KOBOLD}, {PM_LARGE_KOBOLD, PM_KOBOLD_LORD},
	{PM_GNOME, PM_GNOME_LORD}, {PM_GNOME_LORD, PM_GNOME_KING},
	{PM_GNOME, PM_GNOME_LADY}, {PM_GNOME_LADY, PM_GNOME_QUEEN},
	{PM_DWARF, PM_DWARF_LORD}, {PM_DWARF_LORD, PM_DWARF_KING},
	{PM_DWARF, PM_DWARF_CLERIC}, {PM_DWARF_CLERIC, PM_DWARF_QUEEN},
	{PM_MIND_FLAYER, PM_MASTER_MIND_FLAYER},
	{PM_DEEP_ONE, PM_DEEPER_ONE}, {PM_DEEPER_ONE, PM_DEEPEST_ONE},
	{PM_SMALL_GOAT_SPAWN, PM_GOAT_SPAWN}, {PM_GOAT_SPAWN, PM_GIANT_GOAT_SPAWN},
	{PM_APPRENTICE_WITCH, PM_WITCH},
	{PM_ORC, PM_ORC_CAPTAIN}, {PM_HILL_ORC, PM_ORC_CAPTAIN},
	{PM_MORDOR_ORC, PM_ORC_CAPTAIN}, 
	{PM_URUK_HAI, PM_URUK_CAPTAIN},
	{PM_SEWER_RAT, PM_GIANT_RAT},
#ifdef CONVICT
	{PM_GIANT_RAT, PM_ENORMOUS_RAT},
	{PM_ENORMOUS_RAT, PM_RODENT_OF_UNUSUAL_SIZE},
#endif	/* CONVICT */
	{PM_CAVE_SPIDER, PM_GIANT_SPIDER}, {PM_GIANT_SPIDER, PM_MIRKWOOD_SPIDER}, {PM_MIRKWOOD_SPIDER, PM_MIRKWOOD_ELDER},
	{PM_OGRE, PM_OGRE_LORD}, {PM_OGRE_LORD, PM_OGRE_KING},
	{PM_ELF, PM_WOODLAND_ELF},
	{PM_WOODLAND_ELF, PM_ELF_LORD}, {PM_GREEN_ELF, PM_ELF_LORD}, {PM_GREY_ELF, PM_ELF_LORD},
	{PM_ELF_LORD, PM_ELVENKING},
	{PM_WOODLAND_ELF, PM_ELF_LADY},
	{PM_GREEN_ELF, PM_ELF_LADY}, {PM_GREY_ELF, PM_ELF_LADY},
	{PM_ELF_LADY, PM_ELVENQUEEN},
	{PM_ALABASTER_ELF, PM_ALABASTER_ELF_ELDER},
	{PM_DROW, PM_HEDROW_WARRIOR},
	{PM_DROW, PM_DROW_CAPTAIN}, {PM_DROW_CAPTAIN, PM_DROW_MATRON},
	{PM_NUPPERIBO, PM_METAMORPHOSED_NUPPERIBO}, {PM_METAMORPHOSED_NUPPERIBO, PM_ANCIENT_NUPPERIBO},
	{PM_LICH, PM_DEMILICH}, {PM_DEMILICH, PM_MASTER_LICH},
	{PM_MASTER_LICH, PM_ARCH_LICH},
	{PM_BABY_METROID, PM_METROID},{PM_METROID, PM_ALPHA_METROID}, {PM_ALPHA_METROID, PM_GAMMA_METROID},
	{PM_GAMMA_METROID, PM_ZETA_METROID}, {PM_ZETA_METROID, PM_OMEGA_METROID}, 
	{PM_OMEGA_METROID, PM_METROID_QUEEN},
	{PM_VAMPIRE, PM_VAMPIRE_LORD}, {PM_BAT, PM_GIANT_BAT},
	{PM_GIANT_BAT, PM_BATTLE_BAT}, {PM_BATTLE_BAT, PM_WARBAT},
	{PM_BABY_GRAY_DRAGON, PM_GRAY_DRAGON},
	{PM_BABY_SILVER_DRAGON, PM_SILVER_DRAGON},
	{PM_BABY_DEEP_DRAGON, PM_DEEP_DRAGON},
	{PM_BABY_SHIMMERING_DRAGON, PM_SHIMMERING_DRAGON},
	{PM_BABY_RED_DRAGON, PM_RED_DRAGON},
	{PM_BABY_WHITE_DRAGON, PM_WHITE_DRAGON},
	{PM_BABY_ORANGE_DRAGON, PM_ORANGE_DRAGON},
	{PM_BABY_BLACK_DRAGON, PM_BLACK_DRAGON},
	{PM_BABY_BLUE_DRAGON, PM_BLUE_DRAGON},
	{PM_BABY_GREEN_DRAGON, PM_GREEN_DRAGON},
	{PM_BABY_YELLOW_DRAGON, PM_YELLOW_DRAGON},
	{PM_RED_NAGA_HATCHLING, PM_RED_NAGA},
	{PM_BLACK_NAGA_HATCHLING, PM_BLACK_NAGA},
	{PM_GOLDEN_NAGA_HATCHLING, PM_GOLDEN_NAGA},
	{PM_GUARDIAN_NAGA_HATCHLING, PM_GUARDIAN_NAGA},
	{PM_STRANGE_LARVA, PM_AKKABISH_TANNIN},
	{PM_SMALL_MIMIC, PM_LARGE_MIMIC}, {PM_LARGE_MIMIC, PM_GIANT_MIMIC},
	{PM_BABY_LONG_WORM, PM_LONG_WORM},
	{PM_BABY_PURPLE_WORM, PM_PURPLE_WORM},
	{PM_BABY_CROCODILE, PM_CROCODILE},
	{PM_BABY_CAVE_LIZARD,PM_SMALL_CAVE_LIZARD}, {PM_SMALL_CAVE_LIZARD, PM_CAVE_LIZARD}, {PM_CAVE_LIZARD, PM_LARGE_CAVE_LIZARD},
	{PM_SOLDIER, PM_SERGEANT}, {PM_SERGEANT, PM_LIEUTENANT}, {PM_LIEUTENANT, PM_CAPTAIN},
	{PM_MYRMIDON_HOPLITE, PM_MYRMIDON_LOCHIAS}, {PM_MYRMIDON_LOCHIAS, PM_MYRMIDON_YPOLOCHAGOS}, 
		{PM_MYRMIDON_YPOLOCHAGOS, PM_MYRMIDON_LOCHAGOS},
	{PM_WATCHMAN, PM_WATCH_CAPTAIN},
	{PM_ALIGNED_PRIEST, PM_HIGH_PRIEST},
	{PM_STUDENT, PM_ARCHEOLOGIST},
	{PM_RHYMER, PM_BARD},
	{PM_HEDROW_WIZARD, PM_HEDROW_MASTER_WIZARD},
	{PM_MYRKALFR, PM_MYRKALFAR_WARRIOR}, {PM_MYRKALFAR_WARRIOR, PM_MYRKALFAR_MATRON},
	{PM_ATTENDANT, PM_HEALER},
	{PM_PAGE, PM_KNIGHT},
	{PM_ACOLYTE, PM_PRIEST},
	{PM_ACOLYTE, PM_PRIESTESS},
	{PM_APPRENTICE, PM_WIZARD},
	{PM_DUNGEON_FERN_SPROUT, PM_DUNGEON_FERN},
	{PM_SWAMP_FERN_SPROUT, PM_SWAMP_FERN},
	{PM_BURNING_FERN_SPROUT, PM_BURNING_FERN},
	{NON_PM,NON_PM}

};

int
little_to_big(montype, female)
int montype;
int female;
{
#ifndef AIXPS2_BUG
	register int i;

	for (i = 0; grownups[i][0] >= LOW_PM; i++)
		if(montype == grownups[i][0] && (
			!((mons[grownups[i][1]].mflagsb)&(MB_FEMALE|MB_MALE)) ||
			 ((mons[grownups[i][1]].mflagsb)& MB_NEUTER) ||
			 ((mons[grownups[i][0]].mflagsb)& MB_NEUTER) ||
			(female && ((mons[grownups[i][1]].mflagsb)&(MB_FEMALE))) ||
			(!female && ((mons[grownups[i][1]].mflagsb)&(MB_MALE)))
		)) return grownups[i][1];
	return montype;
#else
/* AIX PS/2 C-compiler 1.1.1 optimizer does not like the above for loop,
 * and causes segmentation faults at runtime.  (The problem does not
 * occur if -O is not used.)
 * lehtonen@cs.Helsinki.FI (Tapio Lehtonen) 28031990
 */
	int i;
	int monvalue;

	monvalue = montype;
	for (i = 0; grownups[i][0] >= LOW_PM; i++)
		if(montype == grownups[i][0] && (
			!((mons[grownups[i][1]].mflagsb)&(MB_FEMALE|MB_MALE)) ||
			 ((mons[grownups[i][1]].mflagsb)& MB_NEUTER) ||
			 ((mons[grownups[i][0]].mflagsb)& MB_NEUTER) ||
			(female && ((mons[grownups[i][1]].mflagsb)&(MB_FEMALE))) ||
			(!female && ((mons[grownups[i][1]].mflagsb)&(MB_MALE)))
		)) monvalue = grownups[i][1];

	return monvalue;
#endif
}

int
big_to_little(montype)
int montype;
{
	register int i;

	for (i = 0; grownups[i][0] >= LOW_PM; i++)
		if(montype == grownups[i][1]) return big_to_little(grownups[i][0]);
	return montype;
}

/*
 * Return the permonst ptr for the race of the monster.
 * Returns correct pointer for non-polymorphed and polymorphed
 * player.  It does not return a pointer to player role character.
 */
const struct permonst *
raceptr(mtmp)
struct monst *mtmp;
{
    if (mtmp == &youmonst && !Upolyd) return(&mons[urace.malenum]);
    else return(mtmp->data);
}

static const char *levitate[4]	= { "float", "Float", "wobble", "Wobble" };
static const char *flys[4]	= { "fly", "Fly", "flutter", "Flutter" };
static const char *flyl[4]	= { "fly", "Fly", "stagger", "Stagger" };
static const char *slither[4]	= { "slither", "Slither", "falter", "Falter" };
static const char *ooze[4]	= { "ooze", "Ooze", "tremble", "Tremble" };
static const char *immobile[4]	= { "wiggle", "Wiggle", "pulsate", "Pulsate" };
static const char *crawl[4]	= { "crawl", "Crawl", "falter", "Falter" };

const char *
locomotion(mon, def)
struct monst *mon;
const char *def;
{
	int capitalize = (*def == highc(*def));
	const struct permonst *ptr = mon->data;
	return (
		mon_resistance(mon,LEVITATION) ? levitate[capitalize] :
		(mon_resistance(mon,FLYING) && ptr->msize <= MZ_SMALL) ? flys[capitalize] :
		(mon_resistance(mon,FLYING) && ptr->msize > MZ_SMALL)  ? flyl[capitalize] :
		slithy(ptr)     ? slither[capitalize] :
		amorphous(ptr)  ? ooze[capitalize] :
		!ptr->mmove	? immobile[capitalize] :
		nolimbs(ptr)    ? crawl[capitalize] :
		def
	       );

}

const char *
stagger(mon, def)
struct monst *mon;
const char *def;
{
	int capitalize = 2 + (*def == highc(*def));
	const struct permonst *ptr = mon->data;
	return (
		mon_resistance(mon,LEVITATION) ? levitate[capitalize] :
		(mon_resistance(mon,FLYING) && ptr->msize <= MZ_SMALL) ? flys[capitalize] :
		(mon_resistance(mon,FLYING) && ptr->msize > MZ_SMALL)  ? flyl[capitalize] :
		slithy(ptr)     ? slither[capitalize] :
		amorphous(ptr)  ? ooze[capitalize] :
		!ptr->mmove	? immobile[capitalize] :
		nolimbs(ptr)    ? crawl[capitalize] :
		def
	       );

}

/* return a phrase describing the effect of fire attack on a type of monster */
const char *
on_fire(mptr, mattk)
struct permonst *mptr;
struct attack *mattk;
{
    const char *what;

    switch (monsndx(mptr)) {
    case PM_FLAMING_SPHERE:
    case PM_FIRE_VORTEX:
    case PM_FIRE_ELEMENTAL:
    case PM_LIGHTNING_PARAELEMENTAL:
    case PM_SALAMANDER:
	what = "already on fire";
	break;
    case PM_WATER_ELEMENTAL:
    case PM_FOG_CLOUD:
    case PM_STEAM_VORTEX:
    case PM_ACID_PARAELEMENTAL:
	what = "boiling";
	break;
    case PM_ICE_VORTEX:
    case PM_ICE_PARAELEMENTAL:
    case PM_GLASS_GOLEM:
	what = "melting";
	break;
    case PM_STONE_GOLEM:
    case PM_SENTINEL_OF_MITHARDIR:
    case PM_CLAY_GOLEM:
    case PM_GOLD_GOLEM:
    case PM_AIR_ELEMENTAL:
    case PM_POISON_PARAELEMENTAL:
    case PM_EARTH_ELEMENTAL:
    case PM_DUST_VORTEX:
    case PM_ENERGY_VORTEX:
	what = "heating up";
	break;
    default:
	what = (mattk->aatyp == AT_HUGS) ? "being roasted" : "on fire";
	break;
    }
    return what;
}

/*  An outright copy of the function of the same name in makedefs.c
since I have no clue how the heck to access it while it's in there
*/
int
mstrength(ptr)
struct permonst *ptr;
{
	int	i, tmp2, n, tmp = ptr->mlevel;

	if (tmp > 49)		/* special fixed hp monster */
		tmp = 2 * (tmp - 6) / 4;

	/*	For creation in groups */
	n = (!!(ptr->geno & G_SGROUP));
	n += (!!(ptr->geno & G_LGROUP)) << 1;

	/*	For ranged attacks */
	if (ranged_attk(ptr)) n++;

	/*	For higher ac values */
	tmp2 = 10-(ptr->nac+ptr->dac+ptr->pac);
	n += (tmp2 < 4);
	n += (tmp2 < 0);
	n += (tmp2 < -5);
	n += (tmp2 < -10);
	n += (tmp2 < -20);

	/*	For very fast monsters */
	n += (ptr->mmove >= 18);

	/*	For each attack and "special" attack */
	for (i = 0; i < NATTK; i++) {

		tmp2 = ptr->mattk[i].aatyp;
		n += (tmp2 > 0);
		n += (tmp2 == AT_MAGC || tmp2 == AT_MMGC ||
			tmp2 == AT_TUCH || tmp2 == AT_SRPR || tmp2 == AT_TNKR);
		n += (tmp2 == AT_WEAP && (ptr->mflagsb & MB_STRONG));
	}

	/*	For each "special" damage type */
	for (i = 0; i < NATTK; i++) {

		tmp2 = ptr->mattk[i].adtyp;
		if ((tmp2 == AD_DRLI) || (tmp2 == AD_STON) || (tmp2 == AD_DRST)
			|| (tmp2 == AD_DRDX) || (tmp2 == AD_DRCO) || (tmp2 == AD_WERE)
			|| (tmp2 == AD_SHDW) || (tmp2 == AD_STAR) || (tmp2 == AD_BLUD))
			n += 2;
		else if (strcmp(ptr->mname, "grid bug")) n += (tmp2 != AD_PHYS);
		n += ((int)(ptr->mattk[i].damd * ptr->mattk[i].damn) > 23);
	}

	/*	Leprechauns are special cases.  They have many hit dice so they
	can hit and are hard to kill, but they don't really do much damage. */
	if (!strcmp(ptr->mname, "leprechaun")) n -= 2;

	/*	Hooloovoo spawn many dangerous enemies. */
	if (!strcmp(ptr->mname, "hooloovoo")) n += 10;

/*	Some monsters have nonstandard groups that increase difficulty. */
	if (!strcmp(ptr->mname, "arcadian avenger")) n += 1;
	
	if (!strcmp(ptr->mname, "drow matron")) n += 2;
	if (!strcmp(ptr->mname, "Elvenking")) n += 2;
	if (!strcmp(ptr->mname, "Elvenqueen")) n += 2;
	if (!strcmp(ptr->mname, "chiropteran")) n += 2;
	
	/*	Finally, adjust the monster level  0 <= n <= 24 (approx.) */
	if (n == 0) tmp--;
	else if (n >= 6) tmp += (n / 2);
	else tmp += (n / 3 + 1);

	return((tmp >= 0) ? tmp : 0);
}

#endif /* OVLB */

/*mondata.c*/
