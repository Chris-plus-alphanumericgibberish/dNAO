/*	SCCS Id: @(#)mondata.c	3.4	2003/06/02	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#include "horrordata.h"
#include "mondata.h"
#include "mattkbp.h"
#include "xhity.h"

/*	These routines provide basic data for any type of monster. */
STATIC_DCL void FDECL(set_template_data, (struct permonst *, struct permonst *, int));

char * nameless_horror_name;

#ifdef OVLB

/* 
 * saves the index number of each part of the permonst array to itself
 * is called at gamestart (and after restoring a save)
 */
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
 * Some monster intrinsics go away if the monster is cancelled
 * (and return if uncancelled) so mcan must be set via this
 * wrapper.
 */

void
set_mcan(mon, state)
struct monst *mon;
boolean state;
{
	boolean weap_attack, xwep_attack;
	mon->mcan = state;
	if(mon->mcan && mon->mtyp == PM_UNMASKED_TETTIGON){
		set_mon_data(mon, PM_TRANSCENDENT_TETTIGON);
		mon->m_insight_level += 35;
		mon->mvar1_tettigon_uncancel = TRUE;
	}
	set_mon_data_core(mon, mon->data, FALSE);
	weap_attack = mon_attacktype(mon, AT_WEAP) ? TRUE : FALSE;
	xwep_attack = mon_attacktype(mon, AT_XWEP) ? TRUE : FALSE;
	if(weap_attack && !MON_WEP(mon)){
		mon->weapon_check = NEED_WEAPON;
	}
	else if(!weap_attack && MON_WEP(mon)){
		setmnotwielded(mon, MON_WEP(mon));
	}
	
	if(xwep_attack && !MON_SWEP(mon)){
		mon->weapon_check = NEED_WEAPON;
	}
	else if(!xwep_attack && MON_SWEP(mon)){
		setmnotwielded(mon, MON_SWEP(mon));
	}
}

/*
 * Safely sets mon->data from an mtyp, including mon's template.
 * Gets and/or allocates (via permonst_of) memory for mon's data field
 * Calling `set_mon_data(mon, mon->mtyp)` is always ok.
 *
 * new_form: see set_mon_data_core().
 */
static void
set_mon_data_flagged(struct monst *mon, int mtyp, boolean new_form)
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
		else {
			ptr->mname = mons[ptr->mtyp].mname;
			bas->mname = mons[bas->mtyp].mname;
		}
		/* adjust permonst if needed */
		if (mon != &youmonst && templated(mon))
			set_template_data(bas, ptr, get_template(mon));
	}
	/* eveything else has permonst memory assigned by permonst_of */
	else {
		ptr = permonst_of(mtyp, get_template(mon));
		bas = &mons[mtyp];
	}
	/* set monster data */
	set_mon_data_core(mon, ptr, new_form);
	return;
}

/*
 * Body parts a monster of this type has at the given level, over and above
 * what its permonst lists -- the parts mon_bodyparts() cannot see because
 * they depend on the individual. Its permonst-side counterpart is
 * mon_insight_hidden_bits() (src/mattkbp.c).
 *
 * Takes the level rather than reading mlev(mon) so that a level change can
 * be applied as a difference between two calls (mon_relevel_bodyparts()).
 */
static struct atkbp_set
mon_extra_bodyparts(struct monst *mon, int lev)
{
	/* An Iksh'na deva at max level swings a million arms in place of her
	 * weapon (getattk(), src/xhity.c) -- nothing in her stat block hints
	 * at it. */
	if (mon->mtyp == PM_IKSH_NA_DEVA && lev >= 45)
		return ATKBP(ARM);
	return ATKBP(NONE);
}

/* Drop injuries to parts mon no longer has. */
static void
prune_injuries(struct monst *mon)
{
	mon->minjuries = atkbp_and(mon->minjuries, mon->mbodyparts_full);
	return;
}

/*
 * Recompute which of mon's body parts the PC can currently perceive, by
 * veiling the insight-gated ones. Derived from mbodyparts_full, not
 * recomputed from the permonst.
 *
 * Deliberately leaves minjuries alone: a hidden limb is still there and can
 * still be hurt, so injuries outlive their limb's visibility.
 */
void
mon_refresh_visible_bodyparts(struct monst *mon)
{
	/* change_uinsight() can fire during u_init(), before init_uasmon() has
	 * given youmonst a permonst to read a body plan out of */
	if (!mon->data)
		return;
	mon->mbodyparts = atkbp_diff(mon->mbodyparts_full,
				     mon_insight_veil(mon->data, Insight));
	return;
}

/*
 * Rebuild mon's whole body plan from its form, discarding whatever it held.
 * Only for creation and for taking on a new form: everywhere else the full
 * plan is the monster's own record, and may legitimately differ from its
 * form's by parts it has gained or lost as an individual.
 */
void
mon_refresh_bodyparts(struct monst *mon)
{
	if (!mon->data)
		return;
	mon->mbodyparts_full = atkbp_union(mon->data->bodyparts,
					   mon_extra_bodyparts(mon, mlev(mon)));
	prune_injuries(mon);
	mon_refresh_visible_bodyparts(mon);
	return;
}

/* A leg mon has lost is still counted -- it is a leg it ought to have and does
 * not, which is what mon_injured_leg_count() below reports it as. */
int
mon_body_leg_count(struct monst *mon)
{
	if (!mon->data)
		return atkbp_leg_count(mon->mbodyparts_full);
	return atkbp_leg_count(atkbp_union(mon->mbodyparts_full, mon->data->bodyparts));
}

/* How many of those legs are unusable: injured, or missing outright. */
int
mon_injured_leg_count(struct monst *mon)
{
	struct atkbp_set missing;

	if (!mon->data)
		return atkbp_leg_count(mon->minjuries);
	missing = atkbp_diff(mon->data->bodyparts, mon->mbodyparts_full);
	return atkbp_leg_count(atkbp_union(mon->minjuries, missing));
}

boolean
mon_enough_legs_to_stand(struct monst *mon)
{
	int full = mon_body_leg_count(mon);

	return full > 0 && 2 * (full - mon_injured_leg_count(mon)) > full;
}

/* Injuries are dealt against this set, not mbodyparts_full: hurting a limb
 * must never be what reveals it. */
int
mon_visible_leg_count(struct monst *mon)
{
	return atkbp_leg_count(mon->mbodyparts);
}

/* The parts of `family` an injury could still land on: perceivable, and not
 * already hurt. */
static struct atkbp_set
mon_injurable_parts(struct monst *mon, struct atkbp_set family)
{
	return atkbp_diff(atkbp_and(mon->mbodyparts, family), mon->minjuries);
}

static struct atkbp_set
mon_injurable_legs(struct monst *mon)
{
	return mon_injurable_parts(mon, ATKBP_LEG_ORDINALS_MASK());
}

/* True when injure_random_legs() below would find nothing to hurt. Legless
 * monsters and monsters whose legs are all insight-hidden read as wounded
 * here. */
boolean
all_visible_legs_wounded(struct monst *mon)
{
	return atkbp_is_none(mon_injurable_legs(mon));
}

/* No-op when the arm was empty, so callers can hand over MON_WEP()/MON_SWEP()
 * unchecked. */
static void
mon_drop_wielded(struct monst *mon, struct obj *wep)
{
	if (!wep)
		return;

	if (canseemon(mon))
		pline("%s drops %s %s!", Monnam(mon), mhis(mon), xname(wep));
	obj_extract_and_unequip_self(wep);
	mdrop_obj(mon, wep, FALSE);
}

/* `how` is the trailing phrase of the message -- "by the spears", or "" for no
 * qualifier. `plural` names the family for a multi-part injury; it is copied up
 * front because the callers build it out of the rotating name buffers.
 * Returns how many parts were newly hurt. */
static int
injure_random_parts(struct monst *mon, struct atkbp_set family, int count,
	const char *how, const char *plural)
{
	struct atkbp_set hurt = ATKBP(NONE), pick;
	char plurbuf[BUFSZ];
	int avail, n = 0;
	const char *part;

	Strcpy(plurbuf, plural);
	while (count-- > 0) {
		struct atkbp_set parts = atkbp_diff(mon_injurable_parts(mon, family), hurt);

		avail = atkbp_bit_count(parts);
		if (!avail)
			break;
		pick = atkbp_nth_bit(parts, rn2(avail));
		hurt = atkbp_union(hurt, pick);
		n++;
	}
	if (!n)
		return 0;
	mon->minjuries = atkbp_union(mon->minjuries, hurt);

	if (canseemon(mon)) {
		part = (n == 1) ? atkbp_bodypart_name(pick, mon->data) : plurbuf;
		pline("%s %s %s injured%s%s!", s_suffix(Monnam(mon)), part,
			(n == 1) ? "is" : "are", *how ? " " : "", how);
	}
	if (atkbp_intersects(hurt, ATKBP(ARM_DOMINANT)))
		mon_drop_wielded(mon, MON_WEP(mon));
	if (atkbp_intersects(hurt, ATKBP(ARM_OFFHAND)))
		mon_drop_wielded(mon, MON_SWEP(mon));
	if (!mon_enough_legs_to_stand(mon)) {
		if (mon == &youmonst)
			make_prone(TRUE);
		else
			make_mon_prone(mon, TRUE);
	}
	return n;
}

int
injure_random_limbs(struct monst *mon, int count, const char *how)
{
	return injure_random_parts(mon, ATKBP_LIMB_MASK(), count, how, "limbs");
}

/* TRUE when `attk` is made with a limb that could still be hurt. */
boolean
attack_limb_injurable(struct monst *mon, struct attack *attk)
{
	struct atkbp_set limbs = atkbp_and(attk->bodypart, ATKBP_LIMB_MASK());

	return !atkbp_is_none(mon_injurable_parts(mon, limbs));
}

int
injure_attack_limb(struct monst *mon, struct attack *attk, const char *how)
{
	struct atkbp_set limbs = atkbp_and(attk->bodypart, ATKBP_LIMB_MASK());

	if (atkbp_is_none(limbs))
		return 0;
	return injure_random_parts(mon, limbs, 1, how, "limbs");
}

int
injure_random_legs(struct monst *mon, int count, const char *how)
{
	return injure_random_parts(mon, ATKBP_LEG_ORDINALS_MASK(), count, how,
		makeplural(mbodypart(mon, LEG)));
}

/* Judged against the body mon has rather than the body the PC can see: walking
 * is a fact about the monster. */
boolean
mon_hobbled(struct monst *mon)
{
	int hurt = mon_injured_leg_count(mon);
	int legs;

	if (!hurt)
		return FALSE;
	legs = mon_body_leg_count(mon);
	return legs > 0 && hurt * 2 >= legs;
}

/* Injuries knit on their own only once mon is otherwise whole. Each is rolled
 * independently, so a turn may mend none of them or all of them. */
void
mon_heal_injuries_natural(struct monst *mon)
{
	struct atkbp_set healed = ATKBP(NONE);
	int i;

	if (mon->mhp < mon->mhpmax || atkbp_is_none(mon->minjuries))
		return;
	for (i = atkbp_bit_count(mon->minjuries) - 1; i >= 0; i--)
		if (!rn2(60))
			healed = atkbp_union(healed, atkbp_nth_bit(mon->minjuries, i));
	mon->minjuries = atkbp_diff(mon->minjuries, healed);
	return;
}

/* Returns how many injuries were mended. */
int
mon_heal_injuries(struct monst *mon, int count)
{
	int avail, n = 0;

	while (count == HEAL_ALL_INJURIES || count-- > 0) {
		avail = atkbp_bit_count(mon->minjuries);
		if (!avail)
			break;
		mon->minjuries = atkbp_diff(mon->minjuries,
					    atkbp_nth_bit(mon->minjuries, rn2(avail)));
		n++;
	}
	return n;
}

/*
 * Two things stop an attack: a part that has been injured, and a part mon's
 * form has that mon itself does not -- missing stops an attack the same way
 * wrecked does. A part the attack names that mon's form never had does not
 * block, since that comes of a borrowed attack (a lillend wearing another
 * monster's mask); the body plan used here is mon's own, not the mask's.
 */
boolean
injuries_block(struct monst *mon, struct attack *attk)
{
	struct permonst *ptr = (mon == &youmonst) ? youracedata : mon->data;
	struct atkbp_set missing;

	if (!ptr)
		return FALSE;
	missing = atkbp_diff(ptr->bodyparts, mon->mbodyparts_full);
	return injuries_would_block_attk(ptr, attk, atkbp_union(mon->minjuries, missing));
}

/*
 * Move mon's body plan from the parts oldlev granted to the ones newlev
 * does, touching nothing else -- so a part gained or lost as an individual
 * survives a level change.
 */
void
mon_relevel_bodyparts(struct monst *mon, int oldlev, int newlev)
{
	struct atkbp_set old_extra, new_extra, lost;

	if (!mon->data)
		return;
	old_extra = mon_extra_bodyparts(mon, oldlev);
	new_extra = mon_extra_bodyparts(mon, newlev);
	/* a part the form itself provides is not the level's to take back */
	lost = atkbp_diff(atkbp_diff(old_extra, new_extra), mon->data->bodyparts);
	mon->mbodyparts_full = atkbp_union(atkbp_diff(mon->mbodyparts_full, lost), new_extra);
	prune_injuries(mon);
	mon_refresh_visible_bodyparts(mon);
	return;
}

/* mon is taking on mtyp as a new form -- the ordinary case. */
void
set_mon_data(struct monst *mon, int mtyp)
{
	set_mon_data_flagged(mon, mtyp, TRUE);
	return;
}

/* Re-point a restored monster's data field, which is the one part of it
 * that can't be saved. */
void
restore_mon_data(struct monst *mon, int mtyp)
{
	set_mon_data_flagged(mon, mtyp, FALSE);
	/* Insight may have moved while this monster's level was unloaded */
	mon_refresh_visible_bodyparts(mon);
	return;
}

int
hell_dice()
{
	if (u.uz.dnum != dungeon_topology.d_hell1_level.dnum
	    || u.uz.dlevel <= dungeon_topology.d_hell1_level.dlevel)
		return 1;
	if (u.uz.dlevel <= dungeon_topology.d_hell2_level.dlevel)
		return 4;
	return !mvitals[PM_ASMODEUS].died ? 9 : 4;
}

int
choose_hellish_energy()
{
	int variant;
	static const int chroma_types[] = { AD_FIRE, AD_ACID, AD_ELEC, AD_COLD, AD_DRCO };

	if (u.uz.dnum != dungeon_topology.d_hell1_level.dnum
	    || u.uz.dlevel < dungeon_topology.d_hell1_level.dlevel)
		variant = 1;
	else if (u.uz.dlevel < dungeon_topology.d_hell2_level.dlevel)
		variant = 2;
	else
		variant = 3;

	switch (variant) {
	case 1:
		switch (dungeon_topology.hell1_variant) {
		case BAEL_LEVEL:     return AD_FIRE;
		case DISPATER_LEVEL: return AD_FIRE;
		case MAMMON_LEVEL:   return AD_ACID;
		case BELIAL_LEVEL:   return AD_FIRE;
		case CHROMA_LEVEL:   return chroma_types[rn2(5)];
		default:             return AD_FIRE;
		}
	case 2:
		switch (dungeon_topology.hell2_variant) {
		case LEVIATHAN_LEVEL:      return AD_COLD;
		case LILITH_LEVEL:         return AD_ACID;
		case BAALZEBUB_LEVEL:      return AD_ELEC;
		case MEPHISTOPHELES_LEVEL: return rn2(2) ? AD_FIRE : rn2(8) ? AD_COLD : AD_ECLD;
		default:                   return AD_COLD;
		}
	default: { /* Asmodeus: 3/9 fire, 3/9 cold, 1/9 each phys/acid/elec */
		int r = rn2(9);
		if (r < 3) return AD_FIRE;
		if (r < 6) return AD_COLD;
		if (r < 7) return AD_DRLI;
		if (r < 8) return AD_ACID;
		return AD_ELEC;
	}
	}
}

void
update_mon_mvar(mon, oldpm, newpm)
struct monst *mon;
int oldpm;
int newpm;
{
	//set melement
	if(is_half_dragon(&mons[newpm])){
		static const int half_dragon_types[] = { AD_COLD, AD_FIRE, AD_SLEE, AD_ELEC, AD_DRST, AD_ACID };
		boolean already_valid = FALSE;
		for(int i = 0; i < SIZE(half_dragon_types); i++){
			if(mon->mvar_hdBreath == half_dragon_types[i]){
				already_valid = TRUE;
				break;
			}
		}
		if(!already_valid){
			mon->mvar_hdBreath = half_dragon_types[rn2(6)];
		}
	}
	else if(is_boreal_dragoon(&mons[newpm])){
		if(!is_boreal_dragoon(&mons[oldpm])){
			static const int boreal_dragon_types[] = { AD_COLD, AD_FIRE, AD_MAGM, AD_PHYS };
			boolean already_valid = FALSE;
			for(int i = 0; i < SIZE(boreal_dragon_types); i++){
				if(mon->mvar_hdBreath == boreal_dragon_types[i]){
					already_valid = TRUE;
					break;
				}
			}
			if(!already_valid){
				mon->mvar_hdBreath = boreal_dragon_types[rn2(4)];
			}
		}
	}
	else if(is_hellish_wielder(&mons[newpm])){
		if(!is_hellish_wielder(&mons[oldpm]))
			mon->melement = choose_hellish_energy();
		/* else keep existing energy across poly forms */
	}
	//else leave as-is. May as well stay consistent between forms, if possib
	//set mvar1
	if(is_vectored_mtyp(newpm)){
		if(!is_vectored_mtyp(oldpm))
			mon->mvar_vector = rn2(8);
		//else same as old
	}
	else if(has_sunflask(newpm)){
		if(!has_sunflask(oldpm)){
			mon->mvar_flask_charges = MAX_FLASK_CHARGES(mon);
		}
	}
	else switch(newpm){
		case PM_PALE_NIGHT:
			mon->mvar_paleWarning = 0;
		break;
		case PM_WITCH_S_FAMILIAR:
			if(oldpm != PM_WITCH_S_FAMILIAR)
				mon->mvar_witchID = 0;
		break;
		case PM_DANCING_BLADE:
			if(oldpm != PM_DANCING_BLADE)
				mon->mvar_suryaID = 0;
		break;
		case PM_BLOB_OF_PRESERVED_ORGANS:
			if(oldpm != PM_BLOB_OF_PRESERVED_ORGANS)
				mon->mvar_huskID = 0;
		break;
		case PM_ALABASTER_MUMMY:
			if(oldpm != PM_ALABASTER_MUMMY){
				switch(rnd(6)){
					case 1:
						mon->mvar_syllable = SYLLABLE_OF_STRENGTH__AESH;
					break;
					case 2:
						mon->mvar_syllable = SYLLABLE_OF_GRACE__UUR;
					break;
					case 3:
						mon->mvar_syllable = SYLLABLE_OF_LIFE__HOON;
					break;
					case 4:
						mon->mvar_syllable = SYLLABLE_OF_SPIRIT__VAUL;
						give_mintrinsic(mon, DISPLACED);
					break;
					case 5:
						mon->mvar_syllable = SYLLABLE_OF_POWER__KRAU;
					break;
					case 6:
						mon->mvar_syllable = SYLLABLE_OF_THOUGHT__NAEN;
					break;
				}
			}
		break;
		case PM_LICH__THE_FIEND_OF_EARTH:
		case PM_KARY__THE_FIEND_OF_FIRE:
		case PM_KRAKEN__THE_FIEND_OF_WATER:
		case PM_TIAMAT__THE_FIEND_OF_WIND:
		case PM_CHAOS:
			mon->mvar_spList_1 = 0;
		break;
		case PM_DREAD_SERAPH:
			mon->mvar_dreadPrayer_cooldown = 0;
		break;
		case PM_DRACAE_ELADRIN:{
			struct permonst *ptr = mkclass(S_CHA_ANGEL, G_PLANES);
			if(ptr)
				mon->mvar_dracaePreg = monsndx(ptr);
		}
		break;
		case PM_LIVING_DOLL:
			mon->mvar_dollTypes = init_doll_sales();
		break;
		case PM_STRANGE_LARVA:
			mon->mvar_tanninType = rn2(10) ? PM_ANCIENT_NUPPERIBO : rn2(5) ? PM_BYAKHEE : PM_MARILITH;
		break;
		case PM_DEMINYMPH:
			mon->mvar_deminymph_role = PM_CAVEMAN;
		break;
		case PM_VERMIURGE:
			mon->mvar_vermiurge = 1000;
		break;
		default:
			//mon->mvar_ancient_breath_cooldown = 0;
			//mon->mvar_yellow_lifesaved = FALSE;
			//mon->mvar_twin_lifesaved = FALSE;
			//mon->mvar_witchID = 0;
			//mon->mvar_suryaID = 0;
			//mon->mvar_huskID = 0;
			//mon->mvar_elfwraith_target = 0;
			//mon->mvar_lucksucker = 0;
			//mon->mvar_star_vampire_blood = 0;
			//mon->mvar_spellweaver_count = 0;
			//mon->mvar1_tettigon_uncancel = 0;
			//mon->mvar1_summonerID = 0;
			mon->mvar1 = 0;
		break;
	}

	mon->mvar2 = 0;
	// mvar_dracaePregTimer = 0
	// mvar_spList_2 = 0
	// mvar_dreadPrayer_progress = 0
	// mvar_attack_pm = 0
	// mvar_elfwraith_spell = 0
	// mvar_spellweaver_seed = 0

	// mvar_conversationTracker = 0
	// mvar_lifesigns = 0 /*Note: lifesigns are 0ed */
	// mvar_spellweaver_last_cast = 0

	mon->mvar3 = 0;

	/* a stance is species-scoped; a form change invalidates the old one */
	mon->mstance = MSTANCE_DEFAULT;
}

/*
 * Safely sets mon->data from an existing data pointer.
 * Calling `set_mon_data_core(mon, mon->data, FALSE)` is always ok.
 *
 * new_form: mon is taking on ptr as a new form, so its body plan should be
 * re-read from it. FALSE for the callers that only want intrinsics
 * recalculated against the form mon already has (set_mcan(),
 * give_mintrinsic()).
 */
void
set_mon_data_core(struct monst *mon, struct permonst *ptr, boolean new_form)
{
	int i;
	int oldpm = mon->mtyp;

	/* data and type */
	mon->data = ptr;
	mon->mtyp = ptr->mtyp;

	if (new_form)
		mon_refresh_bodyparts(mon);

	/* zero out intrinsics to be set now */
	for(i = 0; i < MPROP_SIZE; i++){
		mon->mintrinsics[i] = 0;
	}

	/*Updates monster mvar*/
	if(new_form && oldpm != ptr->mtyp)
		update_mon_mvar(mon, oldpm, ptr->mtyp);

	/* resistances */
	mon->mintrinsics[0] = (ptr->mresists & MR_MASK);
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
	} else if(is_hellish_wielder(ptr)){
		if (!mon->melement)
			mon->melement = choose_hellish_energy();
	}
#define set_mintrinsic(ptr_condition, intrinsic) \
	if ((ptr_condition))	{ mon->mintrinsics[((intrinsic)-1)/32] |=  (1L<<((intrinsic)-1)%32); } \
	else					{ mon->mintrinsics[((intrinsic)-1)/32] &= ~(1L<<((intrinsic)-1)%32); }

#define set_mintrinsic_cancelable(ptr_condition, intrinsic) \
	if ((ptr_condition) && !mon->mcan)	{ mon->mintrinsics[((intrinsic)-1)/32] |=  (1L<<((intrinsic)-1)%32); } \
	else								{ mon->mintrinsics[((intrinsic)-1)/32] &= ~(1L<<((intrinsic)-1)%32); }

	/* resistances */
	if(ptr->mresists & MR_FIRE){
		set_mintrinsic(TRUE, FIRE_RES);
		set_mintrinsic(is_demon(ptr) || is_undead(ptr), HELL_FIRE_RES);
		set_mintrinsic(is_angel(ptr) || is_elemental(ptr), HOLY_FIRE_RES);
	}
	if(ptr->mresists & MR_COLD){
		set_mintrinsic(TRUE, COLD_RES);
		set_mintrinsic(is_demon(ptr) || is_undead(ptr), HELL_COLD_RES);
		set_mintrinsic(is_angel(ptr) || is_elemental(ptr), HOLY_COLD_RES);
	}
	set_mintrinsic(ptr->mresists & MR_SLEEP, SLEEP_RES);
	set_mintrinsic(ptr->mresists & MR_DISINT, DISINT_RES);
	if(ptr->mresists & MR_ELEC){
		set_mintrinsic(TRUE, SHOCK_RES);
		set_mintrinsic(is_demon(ptr) || is_undead(ptr), HELL_SHOCK_RES);
		set_mintrinsic(is_angel(ptr) || is_elemental(ptr), HOLY_SHOCK_RES);
	}
	set_mintrinsic(ptr->mresists & MR_POISON, POISON_RES);
	if(ptr->mresists & MR_ACID){
		set_mintrinsic(TRUE, ACID_RES);
		set_mintrinsic(is_demon(ptr) || is_undead(ptr), HELL_ACID_RES);
		set_mintrinsic(is_angel(ptr) || is_elemental(ptr), HOLY_ACID_RES);
	}
	set_mintrinsic(ptr->mresists & MR_STONE, STONE_RES);
	set_mintrinsic(ptr->mresists & MR_DRAIN, DRAIN_RES);
	set_mintrinsic(ptr->mresists & MR_SICK, SICK_RES);
	set_mintrinsic(ptr->mresists & MR_MAGIC, ANTIMAGIC);
	set_mintrinsic(ptr->mresists & MR_REFLECT, REFLECTING);

	/* other intrinsics */
	set_mintrinsic(species_swims(mon->data), SWIMMING);
	set_mintrinsic((species_flies(mon->data) && (!mon->mcan || has_wings_mon(mon))), FLYING);
	set_mintrinsic_cancelable(species_floats(mon->data), LEVITATION);
	set_mintrinsic_cancelable(species_displaces(mon->data), DISPLACED);
	set_mintrinsic_cancelable(species_passes_walls(mon->data), PASSES_WALLS);
	set_mintrinsic_cancelable(species_regenerates(mon->data), REGENERATION);
	set_mintrinsic_cancelable(species_perceives(mon->data), SEE_INVIS);
	set_mintrinsic_cancelable(species_teleports(mon->data), TELEPORT);
	set_mintrinsic_cancelable(species_controls_teleports(mon->data), TELEPORT_CONTROL);
	set_mintrinsic_cancelable(species_is_telepathic(mon->data), TELEPAT);
	if(is_hellish_wielder(ptr)){
		switch(mon->melement){
		case AD_FIRE:
			set_mintrinsic(TRUE, FIRE_RES);
			set_mintrinsic(TRUE, HELL_FIRE_RES);
		break;
		case AD_COLD:
		case AD_ECLD:
			set_mintrinsic(TRUE, COLD_RES);
			set_mintrinsic(TRUE, HELL_COLD_RES);
		break;
		case AD_ELEC:
			set_mintrinsic(TRUE, SHOCK_RES);
			set_mintrinsic(TRUE, HELL_SHOCK_RES);
		break;
		case AD_ACID:
			set_mintrinsic(TRUE, ACID_RES);
			set_mintrinsic(TRUE, HELL_ACID_RES);
		break;
		case AD_DRCO:
			set_mintrinsic(TRUE, POISON_RES);
		break;
		case AD_DRLI:
			set_mintrinsic(TRUE, DRAIN_RES);
		break;
		}
	}
#undef set_mintrinsic
#undef set_mintrinsic_cancelable
	for(i = 0; i < MPROP_SIZE; i++){
		mon->mintrinsics[i] |= mon->acquired_trinsics[i];
	}
    return;
}

void
give_mintrinsic(mon, intrinsic)
struct monst * mon;
long intrinsic;
{
	mon->acquired_trinsics[((intrinsic)-1)/32] |=  (1L<<((intrinsic)-1)%32);
	set_mon_data_core(mon, mon->data, FALSE);
}

void
remove_mintrinsic(mon, intrinsic)
struct monst * mon;
long intrinsic;
{
	mon->acquired_trinsics[((intrinsic)-1)/32] &= ~(1L<<((intrinsic)-1)%32);
	set_mon_data_core(mon, mon->data, FALSE);
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

/* 
 * Modifies a base permonst struct for a specific template and saves it to `ptr`
 */
void
set_template_data(base, ptr, template)
struct permonst * base;
struct permonst * ptr;
int template;
{
	int mtyp = base->mtyp;
	/* copy original */
	*ptr = *base;
	char nameBuffer[BUFSZ];

#define MT_ITEMS (MT_GREEDY|MT_JEWELS|MT_COLLECT|MT_MAGIC)

	/* make changes to the permonst as necessary */
	switch (template)
	{
	case ZOMBIFIED:
	case ROT_ZOMBIE:
		/* flags: */
		ptr->mflagsm |= (MM_BREATHLESS);
		if(ptr->mflagsm&MM_NEEDPICK)
			ptr->mflagsm &= ~(MM_TUNNEL|MM_NEEDPICK);
		ptr->mflagst |= (MT_MINDLESS | MT_HOSTILE | MT_STALK | MT_CARNIVORE);
		ptr->mflagst &= ~(MT_ANIMAL | MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL | MT_HERBIVORE);
		ptr->mflagsg |= (MG_RPIERCE | MG_RBLUNT);
		ptr->mflagsg &= ~(MG_RSLASH | MG_INFRAVISIBLE);
		if(template == ROT_ZOMBIE){
			ptr->mflagsg |= MG_REGEN;
		}
		ptr->mflagsa |= (MA_UNDEAD);
		ptr->mflagsw |= (MW_ELDER_EYE_ENERGY);
		
		/*Undead are against natural law*/
		if(ptr->maligntyp > 0)
			ptr->maligntyp = -1*ptr->maligntyp;
		else if(ptr->maligntyp == 0)
			ptr->maligntyp = -5;

		/*Zombies have no skill*/
		/*Note: The actual effect of this is to zero out mflagsf, but flags are removed explicitly for futureproofing reasons.*/
		ptr->mflagsf &= ~(MF_MARTIAL_B|MF_MARTIAL_S|MF_MARTIAL_E);
		ptr->mflagsf &= ~(MF_BAB_FULL|MF_BAB_HALF);
		ptr->mflagsf &= ~(MF_LEVEL_30|MF_LEVEL_45);
		ptr->mflagsf &= ~(MF_PHYS_SCALING);
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
		if(template == ROT_ZOMBIE){
			ptr->mresists |= MR_SICK;
			ptr->mresists &= ~MR_FIRE;
		}
		/* misc: */
		ptr->msound = MS_SILENT;
		/* speed: 0.50x, min 6 */
		if (ptr->mmove > 6)
			ptr->mmove = max(6, ptr->mmove / 2);
		break;
	case SKELIFIED:
	case SPARK_SKELETON:
		/* flags: */
		ptr->geno |= (G_NOCORPSE);
		ptr->mflagsm |= (MM_BREATHLESS);
		if(ptr->mflagsm&MM_NEEDPICK)
			ptr->mflagsm &= ~(MM_TUNNEL|MM_NEEDPICK);
		ptr->mflagsc |= (MC_SKELETAL);
		ptr->mflagst |= (MT_MINDLESS | MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_ANIMAL | MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL | MT_HERBIVORE | MT_CARNIVORE | MT_METALLIVORE | MT_MAGIVORE);
		ptr->mflagsg |= (MG_RPIERCE | MG_RSLASH);
		ptr->mflagsg &= ~(MG_RBLUNT | MG_INFRAVISIBLE);
		ptr->mflagsa |= (MA_UNDEAD);
		ptr->mflagsw |= (MW_ELDER_EYE_ENERGY);

		/*Undead are against natural law*/
		if(ptr->maligntyp > 0)
			ptr->maligntyp = -1*ptr->maligntyp;
		else if(ptr->maligntyp == 0)
			ptr->maligntyp = -5;

		/*Skeletons have no skill*/
		/*Note: The actual effect of this is to zero out mflagsf, but flags are removed explicitly for futureproofing reasons.*/
		ptr->mflagsf &= ~(MF_MARTIAL_B|MF_MARTIAL_S|MF_MARTIAL_E);
		ptr->mflagsf &= ~(MF_BAB_FULL|MF_BAB_HALF);
		ptr->mflagsf &= ~(MF_LEVEL_30|MF_LEVEL_45);
		ptr->mflagsf &= ~(MF_PHYS_SCALING);
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
		ptr->mflagsm |= (MM_BREATHLESS|MM_WEBRIP);
		ptr->mflagst |= (MT_MINDLESS | MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_ANIMAL | MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL | MT_HERBIVORE | MT_CARNIVORE | MT_METALLIVORE | MT_MAGIVORE);
		ptr->mflagsg |= (MG_RPIERCE | MG_RSLASH);
		ptr->mflagsg &= ~(MG_RBLUNT | MG_INFRAVISIBLE);
		ptr->mflagsa |= (MA_UNDEAD);
		ptr->mflagsc |= (MC_INDIGESTIBLE);

		/*Crystal dead have no free will*/
		ptr->maligntyp = 20;

		/*Crystal dead have no skill*/
		ptr->mflagsf &= ~(MF_BAB_HALF);
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
	case CORDYCEPS:
		ptr->mflagsm |= (MM_STATIONARY);
	case SPORE_ZOMBIE:
		/* flags: */
		ptr->mflagsm |= (MM_BREATHLESS);
		if(ptr->mflagsm&MM_NEEDPICK)
			ptr->mflagsm &= ~(MM_TUNNEL|MM_NEEDPICK);
		ptr->mflagst = (MT_HOSTILE|MT_MINDLESS|MT_NOTAKE);
		ptr->mflagsg &= ~(MG_INFRAVISIBLE);
		ptr->mflagsv = (MV_ECHOLOCATE|MV_SCENT);
		ptr->mflagsb |= (MB_NOEYES);
		//Note: Plant, NOT Undead. It's a living zombie. Also less resistant to damage
		ptr->mflagsa |= (MA_PLANT);
		ptr->mflagsw |= MW_ELDER_EYE_ELEM;
		
		/*Zuggtmoy's spores are against natural law*/
		if(ptr->maligntyp > 0)
			ptr->maligntyp = -1*ptr->maligntyp;
		else if(ptr->maligntyp == 0)
			ptr->maligntyp = -5;

		/*Zombies have no skill*/
		/*Note: The actual effect of this is to zero out mflagsf, but flags are removed explicitly for futureproofing reasons.*/
		ptr->mflagsf &= ~(MF_MARTIAL_B|MF_MARTIAL_S|MF_MARTIAL_E);
		ptr->mflagsf &= ~(MF_BAB_FULL|MF_BAB_HALF);
		ptr->mflagsf &= ~(MF_LEVEL_30|MF_LEVEL_45);
		ptr->mflagsf &= ~(MF_PHYS_SCALING);
		/* defense: */
		ptr->dac += -2;	/* penalty to dodge AC */
		ptr->hdr += 1;
		ptr->bdr += 1;
		ptr->gdr += 1;
		ptr->ldr += 1;
		ptr->fdr += 1;
		/* resists: */
		ptr->mresists |= MR_SICK;
		/* misc: */
		ptr->msound = MS_SILENT;
		break;
	case FRACTURED:
		/* flags: */
		ptr->mflagsm |= (MM_BREATHLESS);
		ptr->mflagst |= (MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsb |= (MB_NOEYES);
		ptr->mflagsc |= (MC_INDIGESTIBLE);
		ptr->mflagsg &= ~(MG_INFRAVISIBLE);
		ptr->mflagsa |= (MA_UNDEAD);
		ptr->mflagsv |= MV_LIFESENSE;
		ptr->mflagsv &= ~(MV_NORMAL|MV_LOWLIGHT2|MV_LOWLIGHT3|MV_DARKSIGHT|MV_EXTRAMISSION|MV_CATSIGHT);
		if(!(ptr->mflagsw&MW_EYE_OF_YGG)){
			ptr->mflagsw |= MW_ELDER_SIGN;
		}
		break;
	case VAMPIRIC:
		/* flags: */
		ptr->mflagsm |= (MM_BREATHLESS);
		ptr->mflagst |= (MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsg &= ~(MG_INFRAVISIBLE|MG_HATESUNHOLY);
		ptr->mflagsg |= (MG_REGEN|MG_HATESHOLY|MG_HATESSILVER);
		ptr->mflagsa |= (MA_UNDEAD | MA_VAMPIRE);
		ptr->mflagsv |= (MV_BLOODSENSE);
		/* resists: */
		ptr->mresists |= (MR_SLEEP | MR_POISON);	/* individual monsters gain cold res at mlev >= 10 */
		ptr->mflagsw |= (MW_ELDER_EYE_ENERGY);
		ptr->msound = MS_VAMPIRE;
		break;
	case FLAYED:
		ptr->mlevel = max(20, ptr->mlevel);
		ptr->nac += 6;
		ptr->pac += 3;
		ptr->hdr += 6;
		ptr->bdr += 6;
		ptr->gdr += 6;
		ptr->ldr += 6;
		ptr->fdr += 6;
		ptr->spe_hdr += 3;
		ptr->spe_bdr += 3;
		ptr->spe_gdr += 3;
		ptr->spe_ldr += 3;
		ptr->spe_fdr += 3;
		/* flags: */
		ptr->mflagsb |= (MB_NOEYES);
		ptr->mflagst |= (MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsg &= ~(MG_HATESUNHOLY);
		ptr->mflagsg |= (MG_REGEN|MG_HATESHOLY|MG_HATESSILVER);
		ptr->mflagsa |= (MA_DEMON|MA_MINION);
		ptr->mflagsv |= MV_BLOODSENSE|MV_EXTRAMISSION;
		ptr->mflagsv &= ~(MV_NORMAL|MV_LOWLIGHT2|MV_LOWLIGHT3|MV_DARKSIGHT|MV_CATSIGHT);

		/* resists: */
		ptr->mresists |= (MR_SLEEP | MR_POISON);	/* individual monsters gain cold res at mlev >= 10 */
		
		ptr->msound = (ptr->msound == MS_SILENT) ? MS_SILENT : MS_SCREAM;

		if (ptr->mmove < 6)
			ptr->mmove = 12;
		else
			ptr->mmove += 6;
		break;
	case MANITOU:
		ptr->pac += 9;
		ptr->spe_hdr += 6;
		ptr->spe_bdr += 6;
		ptr->spe_gdr += 6;
		ptr->spe_ldr += 6;
		ptr->spe_fdr += 6;
		ptr->mflagsv |= MV_BLOODSENSE;
		ptr->mflagsa |= MA_DEMON;
		ptr->mflagst |= MT_HOSTILE;
		ptr->mflagst &= ~(MT_PEACEFUL);
		ptr->mflagsg |= MG_SANLOSS;
		ptr->mflagsg &= ~(MG_HATESUNHOLY);
		ptr->mflagsg |= (MG_REGEN|MG_HATESHOLY|MG_HATESSILVER);
		if(ptr->mmove < 1)
			ptr->mmove = 12;
		ptr->mflagsm &= ~(MM_STATIONARY);
	break;
	case GUECUBU:
		ptr->pac += 9;
		ptr->spe_hdr += 6;
		ptr->spe_bdr += 6;
		ptr->spe_gdr += 6;
		ptr->spe_ldr += 6;
		ptr->spe_fdr += 6;
		if(!(ptr->mflagsb&MB_NOEYES)){
			ptr->mflagsb |= MB_NOEYES; //Techically, shut eyes
			ptr->mflagsv &= ~(MV_NORMAL|MV_LOWLIGHT2|MV_LOWLIGHT3|MV_DARKSIGHT|MV_CATSIGHT);
		}
		ptr->mresists |= (MR_SLEEP);
		ptr->mflagsv |= MV_TELEPATHIC;
		ptr->mflagsa |= MA_DEMON;
		ptr->mflagst |= MT_HOSTILE|MT_TRAITOR;
		ptr->mflagst &= ~(MT_PEACEFUL|MT_MINDLESS|MT_ANIMAL|MT_DOMESTIC);
		ptr->mflagsg &= ~(MG_HATESUNHOLY);
		ptr->mflagsg |= (MG_HATESHOLY);
		ptr->mlevel += 6;
	break;
	case ILLUMINATED:
		/* flags: */
		ptr->mflagsg |= (MG_HATESUNHOLY);
		ptr->mflagsg &= ~(MG_HATESHOLY);
		ptr->mflagsa |= (MA_MINION);
		ptr->mflagsw |= (MW_ELDER_EYE_PLANES);
		ptr->light_radius = max(3, ptr->light_radius);
		break;
	case PSEUDONATURAL:
		/* flags */
		ptr->mflagsa |= (MA_PRIMORDIAL);
		/* resists: */
		ptr->mresists |= (MR_POISON);
		/*More intelligent*/
		if(ptr->mflagst&MT_MINDLESS){
			ptr->mflagst &= ~MT_MINDLESS;
			ptr->mflagst |= MT_ANIMAL;
		}
		else if(ptr->mflagst&MT_ANIMAL){
			//Shouldn't be a mindless animal, but
			ptr->mflagst &= ~MT_MINDLESS;
			ptr->mflagst &= ~MT_ANIMAL;
		}
		ptr->mflagsf &= ~(MF_MARTIAL_B|MF_MARTIAL_S);
		ptr->mflagsf |= MF_MARTIAL_E;
		ptr->mflagsf &= ~(MF_BAB_HALF);
		ptr->mflagsf |= MF_BAB_FULL;
		ptr->mflagsg |= (MG_INSIGHT|MG_SANLOSS);
		/*Pseudonaturals have tentacles, which changes their grasp situation.*/
		if(ptr->mflagsb&MB_NOLIMBS){
			ptr->mflagsb &= ~MB_NOLIMBS;
			ptr->mflagsb |= MB_NOFEET|MB_NOGLOVES;
		}
		else if(ptr->mflagsb&MB_NOHANDS){
			ptr->mflagsb &= ~MB_NOHANDS;
			ptr->mflagsb |= MB_NOGLOVES;
		}
		if(!(ptr->mflagsw&MW_EYE_OF_YGG)){
			ptr->mflagsw |= MW_ELDER_SIGN;
		}
		ptr->mflagsc |= MC_ACID|MC_POIS;
		ptr->mlevel *= 1.5;
		break;
	case TOMB_HERD:
		/* flags: */
		ptr->geno |= (G_NOCORPSE);
		ptr->mflagsm &= ~(MM_AMORPHOUS|MM_WALLWALK|MM_NOTONL|MM_FLEETFLEE);
		if(ptr->mflagsm&MM_NEEDPICK)
			ptr->mflagsm &= ~(MM_TUNNEL|MM_NEEDPICK);
		ptr->mflagsm |= (MM_TENGTPORT|MM_AMPHIBIOUS|MM_BREATHLESS|MM_TPORT|MM_TPORT_CNTRL|MM_WEBRIP);
		ptr->mflagst &= ~(MT_MINDLESS|MT_HERBIVORE|MT_METALLIVORE);
		ptr->mflagst |= (MT_HOSTILE|MT_ANIMAL|MT_CARNIVORE|MT_TRAITOR);
		ptr->mflagsg &= ~(MG_INFRAVISIBLE|MG_RBLUNT);
		ptr->mflagsg |= (MG_VBLUNT|MG_SANLOSS);
		ptr->mflagsb &= ~(MB_OVIPAROUS);
		ptr->mflagsc &= ~(MC_UNSOLID|MC_ACID|MC_POIS|MC_TOSTY|MC_HALUC|MC_INSUBSTANTIAL);
		ptr->mflagsb |= (MB_STRONG);
		ptr->mflagsc |= (MC_INDIGESTIBLE|MC_THICK_HIDE);

		if(!(ptr->mflagsw&MW_EYE_OF_YGG)){
			ptr->mflagsw |= MW_ELDER_SIGN;
		}
		/*The tomb herd is neutral*/
		ptr->maligntyp = 0;

		/* defense: */
		ptr->nac += 6;
		/* resists: */
		ptr->mresists |= (MR_FIRE | MR_COLD | MR_SLEEP | MR_POISON | MR_STONE | MR_DRAIN | MR_SICK | MR_MAGIC);
		/* misc: */
		ptr->msound = MS_SILENT;
		break;
	case YITH:
		/*Yith are lawful*/
		ptr->maligntyp = 10;

		ptr->mflagst &= ~(MT_MINDLESS|MT_ANIMAL|MT_DOMESTIC);
		if(!(ptr->mflagsb&(MB_NOLIMBS|MB_NOHANDS)) && !(ptr->mflagsm&MM_TUNNEL) && !(ptr->mflagsm&MM_WALLWALK))
			ptr->mflagsm |= (MM_TUNNEL|MM_NEEDPICK);
		if(!(ptr->mflagsw&MW_EYE_OF_YGG)){
			ptr->mflagsw |= MW_ELDER_SIGN;
		}
		ptr->mflagsm |= (MM_STANDOFF);
		break;
	case CRANIUM_RAT:
		/* defense: */
		ptr->dac += 4;
		ptr->hdr = 0; //Exposed brain
		ptr->mflagst &= ~(MT_ANIMAL|MT_MINDLESS);
		ptr->mflagsg |= (MG_INSIGHT|MG_SANLOSS);
		ptr->mflagsv |= MV_TELEPATHIC;
		if(!(ptr->mflagsw&MW_EYE_OF_YGG)){
			ptr->mflagsw |= MW_ELDER_SIGN;
		}
		break;
	case PSURLON:
		ptr->pac += 6;
		ptr->spe_hdr += 6;
		ptr->spe_bdr += 6;
		ptr->spe_gdr += 6;
		ptr->spe_wdr += 6;
		ptr->spe_ldr += 6;
		ptr->spe_fdr += 6;
		ptr->mflagst &= ~(MT_ANIMAL|MT_MINDLESS);
		ptr->mflagst |= (MT_HOSTILE|MT_GREEDY|MT_JEWELS|MT_COLLECT|MT_TRAITOR|MT_OMNIVORE);
		ptr->mflagsg |= (MG_NASTY|MG_INSIGHT|MG_SANLOSS);
		ptr->mflagsb &= ~(MB_NOLIMBS|MB_NOHANDS|MB_ANIMAL);
		ptr->mflagsb |= MB_HUMANOID|MB_SLITHY;
		ptr->mflagsv |= MV_TELEPATHIC|MV_NORMAL;
		ptr->mflagsa |= MA_ET|MA_G_O_O;
		if(!(ptr->mflagsm&MM_TUNNEL) && !(ptr->mflagsm&MM_WALLWALK))
			ptr->mflagsm |= (MM_TUNNEL|MM_NEEDPICK);
		if(!(ptr->mflagsw&MW_EYE_OF_YGG)){
			ptr->mflagsw |= MW_ELDER_SIGN;
		}
		break;
	case MINDLESS:
		if(ptr->mflagsm&MM_NEEDPICK)
			ptr->mflagsm &= ~(MM_TUNNEL|MM_NEEDPICK);
		ptr->mflagst |= (MT_MINDLESS | MT_HOSTILE);
		ptr->mflagst &= ~(MT_ANIMAL | MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL | MT_STALK);
		/* defense: */
		ptr->dac = 0;
		ptr->hdr += 2; //No one home
		ptr->msound = MS_SILENT;
		ptr->mflagsf = 0L;

		/*Mindless monsters are neutral*/
		ptr->maligntyp = 0;

		/*Mindless monsters have no skill*/
		/*Note: The actual effect of this is to zero out mflagsf, but flags are removed explicitly for futureproofing reasons.*/
		ptr->mflagsf &= ~(MF_MARTIAL_B|MF_MARTIAL_S|MF_MARTIAL_E);
		ptr->mflagsf &= ~(MF_BAB_FULL|MF_BAB_HALF);
		ptr->mflagsf &= ~(MF_LEVEL_30|MF_LEVEL_45);
		ptr->mflagsf &= ~(MF_PHYS_SCALING);
		break;
	case MISTWEAVER:
		/* flags */
		ptr->mflagsb |= (MB_CAN_AMULET|MB_NOHEAD|MB_STRONG);
		ptr->mresists |= (MR_POISON|MR_ACID|MR_COLD|MR_FIRE|MR_ELEC);
		ptr->mflagsm |= (MM_AMPHIBIOUS);
		ptr->mflagst |= (MT_CARNIVORE);
		ptr->mflagsv |= (MV_ECHOLOCATE|MV_SCENT);
		ptr->mflagsa |= (MA_ANIMAL|MA_PLANT|MA_PRIMORDIAL);
		ptr->mflagsg |= (MG_INSIGHT|MG_SANLOSS);
		if(!(ptr->mflagsw&MW_EYE_OF_YGG)){
			ptr->mflagsw |= MW_ELDER_SIGN;
		}
#define AVG_DR(typ) if(ptr->typ < 5) ptr->typ = (ptr->typ + 5)/2;
#define AVG_AC(typ) if(ptr->typ < 16) ptr->typ = (ptr->typ + 16)/2;
		AVG_AC(nac)
		AVG_DR(bdr)
		AVG_DR(ldr)
		AVG_DR(gdr)
		AVG_DR(fdr)
		if(ptr->mtyp == PM_ITINERANT_PRIESTESS){
			AVG_AC(dac)
			AVG_AC(pac)
			ptr->spe_bdr += 4;
			ptr->spe_gdr += 2;
			ptr->spe_wdr += 2;
			ptr->spe_ldr += 2;
		}
		break;
	case DELOUSED:
		/* flags */
		ptr->mflagsa &= ~(MA_PRIMORDIAL);
		break;
	case M_BLACK_WEB:
		/* flags: */
		ptr->mflagsm |= (MM_BREATHLESS);
		ptr->mflagst |= (MT_MINDLESS | MT_HOSTILE | MT_STALK | MT_TRAITOR);
		ptr->mflagst &= ~(MT_ANIMAL | MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsg |= (MG_RPIERCE | MG_RBLUNT | MG_NOSPELLCOOLDOWN);
		ptr->mflagsg &= ~(MG_RSLASH | MG_INFRAVISIBLE);
		ptr->mflagsa |= (MA_UNDEAD);
		/*Black web victims have no skill*/
		/*Note: The actual effect of this is to zero out mflagsf, but flags are removed explicitly for futureproofing reasons.*/
		ptr->mflagsf &= ~(MF_MARTIAL_B|MF_MARTIAL_S|MF_MARTIAL_E);
		ptr->mflagsf &= ~(MF_BAB_FULL|MF_BAB_HALF);
		ptr->mflagsf &= ~(MF_LEVEL_30|MF_LEVEL_45);
		ptr->mflagsf &= ~(MF_PHYS_SCALING);
		/* defense: */
		ptr->pac = max(ptr->pac, 8);
		ptr->dac += -2;	/* penalty to dodge AC */
		/* resists: */
		ptr->mresists |= (MR_COLD | MR_SLEEP | MR_POISON);
		/* misc: */
		ptr->msound = MS_SILENT;
		/* speed: 0.50x, min 6 */
		if (ptr->mmove > 6)
			ptr->mmove = max(6, ptr->mmove / 2);
		break;
	case BLOOD_MON:
		ptr->mflagsm |= (MM_BREATHLESS);
		ptr->mflagst &= ~(MT_PEACEFUL);
		ptr->mflagsg |= (MG_RALL);
		ptr->mflagsa |= (MA_UNDEAD);
		ptr->mresists |= (MR_SLEEP | MR_POISON);
		break;
	case M_GREAT_WEB:
		/* attacks only */
		break;
	case SLIME_REMNANT:
		/* flags: */
		ptr->geno |= (G_NOCORPSE);
		ptr->mflagsm |= (MM_AMORPHOUS|MM_SWIM|MM_AMPHIBIOUS);
		ptr->mflagst |= (MT_OMNIVORE | MT_MINDLESS | MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsg |= (MG_VSLASH|MG_REGEN|MG_SANLOSS);
		ptr->mflagsg &= ~(MG_RBLUNT|MG_PNAME);
		ptr->mflagsa |= (MA_PRIMORDIAL|MA_AQUATIC);
		ptr->mflagsb |= (MB_NOLIMBS|MB_STRONG);
		ptr->mflagsc |= (MC_ACID|MC_POIS);
		/*Slime remnants have no skill*/
		/*Note: The actual effect of this is to zero out mflagsf, but flags are removed explicitly for futureproofing reasons.*/
		ptr->mflagsf &= ~(MF_MARTIAL_B|MF_MARTIAL_S|MF_MARTIAL_E);
		ptr->mflagsf &= ~(MF_BAB_FULL|MF_BAB_HALF);
		ptr->mflagsf &= ~(MF_LEVEL_30|MF_LEVEL_45);
		ptr->mflagsf &= ~(MF_PHYS_SCALING);
		/* defense: */
		ptr->nac = 14;
		ptr->dac = 0;
		ptr->pac = 0;
		ptr->hdr = 16;
		ptr->bdr = 16;
		ptr->gdr = 16;
		ptr->ldr = 16;
		ptr->fdr = 16;
		/* resists: */
		ptr->mresists |= (MR_ACID|MR_FIRE|MR_COLD|MR_POISON|MR_STONE|MR_SICK);
		/* misc: */
		ptr->msound = MS_SILENT;
		ptr->mflagsw |= (MW_ELDER_SIGN);
		break;
	case YELLOW_TEMPLATE:
		ptr->nac += 2;
		ptr->pac += 3;
		ptr->hdr += 2;
		ptr->bdr += 2;
		ptr->gdr += 2;
		ptr->ldr += 2;
		ptr->fdr += 2;
		ptr->spe_hdr += 3;
		ptr->spe_bdr += 3;
		ptr->spe_gdr += 3;
		ptr->spe_wdr += 3;
		ptr->spe_ldr += 3;
		ptr->spe_fdr += 3;
		ptr->mflagst |= (MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_ANIMAL | MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsg |= (MG_RPIERCE | MG_RBLUNT);
		ptr->mflagsg &= ~(MG_RSLASH | MG_INFRAVISIBLE);
		ptr->mflagsa |= (MA_UNDEAD);
		if(!(ptr->mflagsw&MW_EYE_OF_YGG)){
			ptr->mflagsw |= MW_ELDER_SIGN;
		}
		/*Yellow dead have no skill*/
		/*Note: The actual effect of this is to zero out mflagsf, but flags are removed explicitly for futureproofing reasons.*/
		ptr->mflagsf &= ~(MF_MARTIAL_B|MF_MARTIAL_S|MF_MARTIAL_E);
		ptr->mflagsf &= ~(MF_BAB_FULL|MF_BAB_HALF);
		ptr->mflagsf &= ~(MF_LEVEL_30|MF_LEVEL_45);
		ptr->mflagsf &= ~(MF_PHYS_SCALING);
		/* resists: */
		ptr->mresists |= (MR_COLD | MR_SLEEP | MR_POISON);
		ptr->mresists |= (MR_COLD | MR_SLEEP | MR_POISON);
		// ptr->mcolor = CLR_YELLOW;
		break;
	case DREAM_LEECH:
		ptr->pac += 5;
		ptr->spe_hdr += 5;
		ptr->mflagst |= (MT_HOSTILE | MT_STALK);
		ptr->mflagsa |= (MA_UNDEAD);
		ptr->mflagsv |= (MV_TELEPATHIC);
		if(!(ptr->mflagsw&MW_EYE_OF_YGG)){
			ptr->mflagsw |= MW_ELDER_SIGN;
		}
		// ptr->mcolor = CLR_YELLOW;
		break;
	case MAD_TEMPLATE:
		// ptr->mmove += 12;
		ptr->dac = -5;
		ptr->pac += 4;
		ptr->spe_hdr += 4;
		ptr->spe_bdr += 4;
		ptr->spe_gdr += 4;
		ptr->spe_wdr += 4;
		ptr->spe_ldr += 4;
		ptr->spe_fdr += 4;
		ptr->msound = MS_SCREAM;
		ptr->mflagst |= (MT_HOSTILE | MT_STALK);
		if(!(ptr->mflagst&(MT_ANIMAL|MT_MINDLESS)))
			ptr->mflagst |= MT_ANIMAL;
		ptr->mflagst &= ~(MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL);
		ptr->mflagsg |= (MG_DISPLACEMENT);
		ptr->maligntyp = min(ptr->maligntyp-6, -6);
		break;
	case FALLEN_TEMPLATE:
		ptr->pac = max(9, ptr->pac);
		ptr->spe_hdr = 9;
		ptr->msound = MS_CUSS;
		ptr->mflagst |= (MT_HOSTILE | MT_STALK);
		ptr->mflagst &= ~(MT_PEACEFUL);
		ptr->mflagsg &= ~(MG_HATESUNHOLY);
		ptr->mflagsg |= (MG_HATESHOLY);
		ptr->maligntyp = max(ptr->maligntyp+9, 9);
		break;
	case WORLD_SHAPER:
		/* flags */
		ptr->mflagsm |= MM_WALLWALK;
		ptr->mflagsv |= (MV_EARTHSENSE | MV_OMNI);
		/* defense */
		ptr->mr = min(ptr->mr + 50, 120);
		ptr->hdr += 5;
		ptr->bdr += 5;
		ptr->gdr += 5;
		ptr->ldr += 5;
		ptr->fdr += 5;
		/* resists: */
		ptr->mresists |= MR_STONE;
		/* misc: */
		ptr->mmove = (ptr->mmove+3)/4;
		break;
	case POISON_TEMPLATE:
		if(ptr->mtyp == PM_JRT_NETJER){
			ptr->geno &= ~(G_NOCORPSE);
			ptr->mflagst |= MT_HOSTILE;
			ptr->mflagsg |= MG_NOTAME;
			ptr->spe_bdr = 0;
			ptr->bdr = -4;
		}
		else{
			ptr->dac = 0;
			ptr->pac = 0;
			ptr->spe_hdr = 0;
			ptr->spe_bdr = 0;
			ptr->spe_gdr = 0;
			ptr->spe_wdr = 0;
			ptr->spe_ldr = 0;
			ptr->spe_fdr = 0;
			ptr->mflagst |= MT_BOLD|MT_HOSTILE;
		}
		break;
		case MOLY_TEMPLATE:
			ptr->mflagst &= ~(MT_ANIMAL | MT_MINDLESS | MT_PEACEFUL);
			ptr->mflagst |= MT_HOSTILE;
			ptr->mflagsg |= MG_LORD|MG_COMMANDER|MG_NOTAME;
			ptr->mlevel *= 1.5;
			ptr->dac += 6;
			ptr->pac += 6;
			ptr->hdr += 6;
			ptr->mflagsw |= MW_ELDER_EYE_PLANES;
		break;
		case PLAGUE_TEMPLATE:
			ptr->mflagst &= ~(MT_HOSTILE);
			ptr->mflagst |= MT_PEACEFUL;
			ptr->mlevel = (ptr->mlevel+2)/3;
			ptr->mr /= 3;
			ptr->dac = -5;
			ptr->pac = 0;
			ptr->spe_hdr = 0;
			ptr->spe_bdr = 0;
			ptr->spe_gdr = 0;
			ptr->spe_wdr = 0;
			ptr->spe_ldr = 0;
			ptr->spe_fdr = 0;
			ptr->mmove = 0;
			ptr->msound = (ptr->msound == MS_SILENT) ? MS_SILENT : MS_COUGH;
			ptr->mresists &= ~(MR_POISON|MR_DRAIN|MR_SICK|MR_MAGIC);
		break;
	case CONSTELLATION:
			ptr->geno |= (G_NOCORPSE);
			ptr->nac = 0;
			ptr->hdr = 0;
			ptr->bdr = 0;
			ptr->gdr = 0;
			ptr->ldr = 0;
			ptr->fdr = 0;
			//Monsters that can move get faster, to at least normal speed
			if(ptr->mmove){
				ptr->mmove += 4;
				if(ptr->mmove < 12)
					ptr->mmove = 12;
			}
			ptr->msound = MS_SILENT;
			ptr->mresists = MR_ALL|MR_MAGIC;
			ptr->mflagsm |= MM_FLY|MM_BREATHLESS|MM_FLOAT;
			ptr->mflagst &= ~(MT_ANIMAL | MT_PEACEFUL | MT_CARNIVORE | MT_HERBIVORE | MT_TRAITOR);
			ptr->mflagst |= MT_WANDER|MT_STALK|MT_HOSTILE|MT_DETACHED;
			ptr->mflagsc |= MC_UNSOLID|MC_INSUBSTANTIAL;
			ptr->mflagsg &= ~(MG_PNAME);
			ptr->mflagsg |= MG_NASTY|MG_HATESUNHOLY;
			ptr->mflagsa = MA_ET;
			ptr->mflagsv = MV_DETECTION|MV_OMNI;
			ptr->mflagsw = MW_ELDER_EYE_PLANES;
		break;
	case TONGUE_PUPPET:
		/* flags: */
		ptr->mflagsm |= (MM_FLY|MM_FLOAT|MM_WEBRIP|MM_DOORBUST);
		if(ptr->mflagsm&MM_NEEDPICK)
			ptr->mflagsm &= ~(MM_TUNNEL|MM_NEEDPICK);
		ptr->mflagst |= (MT_MINDLESS | MT_HOSTILE | MT_STALK | MT_CARNIVORE);
		ptr->mflagst &= ~(MT_ANIMAL | MT_PEACEFUL | MT_ITEMS | MT_HIDE | MT_CONCEAL | MT_HERBIVORE);
		ptr->mflagsg &= ~(MG_RSLASH);
		ptr->mflagsg |= MG_INFRAVISIBLE | MG_SANLOSS | MG_INSIGHT;
		ptr->mflagsa = (MA_G_O_O|MA_ANIMAL|MA_ET);
		ptr->mflagsw = (MW_EYE_OF_YGG);
		
		ptr->maligntyp = -28;

		/*Note: The actual effect of this is to zero out mflagsf, but flags are removed explicitly for futureproofing reasons.*/
		ptr->mflagsf &= ~(MF_MARTIAL_B|MF_MARTIAL_S|MF_MARTIAL_E);
		ptr->mflagsf &= ~(MF_BAB_FULL|MF_BAB_HALF);
		ptr->mflagsf &= ~(MF_LEVEL_30|MF_LEVEL_45);
		ptr->mflagsf &= ~(MF_PHYS_SCALING);
		/* defense: */
		ptr->mr = 90;
		ptr->nac = max(ptr->nac+4, 12);
		ptr->dac += 2;
		ptr->pac = 12;
		ptr->hdr += 4;
		ptr->bdr += 4;
		ptr->gdr += 4;
		ptr->ldr += 4;
		ptr->fdr += 4;
		ptr->spe_hdr = 8;
		ptr->spe_bdr = 8;
		ptr->spe_gdr = 8;
		ptr->spe_ldr = 8;
		ptr->spe_fdr = 8;
		ptr->spe_wdr = 8;
		/* resists: */
		ptr->mresists |= (MR_POISON|MR_STONE|MR_COLD|MR_MAGIC);
		/* misc: */
		ptr->msound = MS_SILENT;

		ptr->mmove = 15;
		break;
		case SWOLLEN_TEMPLATE:
			ptr->msize = MZ_GIGANTIC;
			ptr->geno |= (G_NOCORPSE);
		break;
	}
#undef MT_ITEMS

	/* adjust attacks in the permonst */
	extern const struct attack noattack;
	boolean special = FALSE;
	boolean special_2 = FALSE;
	boolean special_3 = FALSE;
	struct attack * attk;
	boolean insert;
	int i, j;
	for (i = 0; i < NATTK; i++)
	{
		attk = &(ptr->mattk[i]);
		insert = FALSE;

		/* some templates completely skip specific attacks */
		while ((template == ZOMBIFIED || template == ROT_ZOMBIE || template == SKELIFIED || template == SPORE_ZOMBIE || template == SPARK_SKELETON) &&
			(
			attk->lev_req > ptr->mlevel ||
			attk->aatyp == AT_SPIT ||
			attk->aatyp == AT_BREA ||
			attk->aatyp == AT_BRSH ||
			attk->aatyp == AT_GAZE ||
			attk->aatyp == AT_ARRW ||
			attk->aatyp == AT_MMGC ||
			attk->aatyp == AT_TNKR ||
			spirit_rapier_at(attk->aatyp) ||
			attk->aatyp == AT_BEAM ||
			attk->aatyp == AT_MAGC ||
			(attk->aatyp == AT_TENT && template == SKELIFIED) ||
			attk->aatyp == AT_GAZE ||
			attk->aatyp == AT_WDGZ ||
			(attk->aatyp == AT_NONE && attk->adtyp == AD_PLYS)
			)
		){
			/* shift all further attacks forwards one slot, and make last one all 0s */
			for (j = 0; j < (NATTK - i - 1); j++)
				attk[j] = attk[j + 1];
			attk[j] = noattack;
		}

		/* some templates completely skip specific attacks */
		while ((template == CRYSTALFIED) &&
			(
			attk->aatyp == AT_SPIT ||
			attk->aatyp == AT_BREA ||
			attk->aatyp == AT_BRSH
			)
		){
			/* shift all further attacks forwards one slot, and make last one all 0s */
			for (j = 0; j < (NATTK - i - 1); j++)
				attk[j] = attk[j + 1];
			attk[j] = noattack;
		}

		/* some templates completely skip specific attacks */
		while ((template == SLIME_REMNANT) &&
			(
			attk->lev_req > ptr->mlevel ||
			attk->aatyp == AT_CLAW ||
			attk->aatyp == AT_BITE ||
			attk->aatyp == AT_OBIT ||
			attk->aatyp == AT_WBIT ||
			attk->aatyp == AT_KICK ||
			attk->aatyp == AT_BUTT ||
			attk->aatyp == AT_TAIL ||
			attk->aatyp == AT_ARRW ||
			attk->aatyp == AT_LRCH ||
			attk->aatyp == AT_HODS ||
			attk->aatyp == AT_LNCK ||
			attk->aatyp == AT_MMGC ||
			attk->aatyp == AT_ILUR ||
			attk->aatyp == AT_TNKR ||
			attk->aatyp == AT_BEAM ||
			attk->aatyp == AT_DEVA ||
			attk->aatyp == AT_JUGL ||
			attk->aatyp == AT_5SQR ||
			attk->aatyp == AT_5SBT ||
			attk->aatyp == AT_WDGZ ||
			attk->aatyp == AT_REND ||
			attk->aatyp == AT_VINE ||
			attk->aatyp == AT_BKGT ||
			attk->aatyp == AT_BKG2 ||
			attk->aatyp == AT_MAGC ||
			attk->aatyp == AT_WEAP ||
			attk->aatyp == AT_XWEP
			)
		){
			/* shift all further attacks forwards one slot, and make last one all 0s */
			for (j = 0; j < (NATTK - i - 1); j++)
				attk[j] = attk[j + 1];
			attk[j] = noattack;
		}

		/* some templates completely skip specific attacks */
		while ((template == MINDLESS) &&
			(
			attk->lev_req > ptr->mlevel ||
			attk->aatyp == AT_ARRW ||
			attk->aatyp == AT_ESPR ||
			attk->aatyp == AT_MMGC ||
			attk->aatyp == AT_TNKR ||
			attk->aatyp == AT_MAGC
			)
		){
			/* shift all further attacks forwards one slot, and make last one all 0s */
			for (j = 0; j < (NATTK - i - 1); j++)
				attk[j] = attk[j + 1];
			attk[j] = noattack;
		}

		/* some templates completely skip specific attacks */
		while ((template == CONSTELLATION) &&
			(
			attk->aatyp == AT_SPIT
			|| attk->aatyp == AT_ENGL
			|| attk->aatyp == AT_GAZE
			|| attk->aatyp == AT_ARRW
			|| attk->aatyp == AT_VINE
			)
		){
			/* shift all further attacks forwards one slot, and make last one all 0s */
			for (j = 0; j < (NATTK - i - 1); j++)
				attk[j] = attk[j + 1];
			attk[j] = noattack;
		}

		/* some templates alter damage types */
		if (template == POISON_TEMPLATE){
			if(ptr->mtyp == PM_JRT_NETJER){
				if(attk->aatyp == AT_SRPR)
					attk->adtyp = AD_SVPN;
			}
			else if(attk->adtyp == AD_STAR
			|| attk->adtyp == AD_ECLD
			|| attk->adtyp == AD_FIRE
			)
				attk->adtyp = AD_EDRC;
			else if(attk->adtyp == AD_SLEE)
				attk->adtyp = AD_DRDX;
			else if(attk->adtyp == AD_PHYS && (attk->aatyp || attk->damn || attk->damd))
				attk->adtyp = AD_DRCO;
			//Note: also affects AD_MOON, but this must be handled after the phase of moon code in xhity.
		}

		/* some templates alter attacks */
		if (template == CONSTELLATION){
			if(attk->aatyp == AT_SRPR || attk->aatyp == AT_WEAP || attk->aatyp == AT_JUGL){
				attk->aatyp = AT_SRPR;
				attk->adtyp = AD_STAR;
				attk->damn = 4;
				attk->damd = 8;
			}
			else if(attk->aatyp == AT_XSPR || attk->aatyp == AT_XWEP){
				attk->aatyp = AT_XSPR;
				attk->adtyp = AD_STAR;
				attk->damn = 4;
				attk->damd = 8;
			}
			else if(attk->aatyp == AT_MSPR || attk->aatyp == AT_MARI){
				attk->aatyp = AT_MSPR;
				attk->adtyp = AD_STAR;
				attk->damn = 4;
				attk->damd = 8;
			}
			else if(attk->aatyp == AT_DSPR || attk->aatyp == AT_DEVA){
				attk->aatyp = AT_DSPR;
				attk->adtyp = AD_STAR;
				attk->damn = 4;
				attk->damd = 8;
			}
		}

		/* if creatures don't have eyes, some gaze attacks are impossible */
		if ((attk->aatyp == AT_GAZE || attk->aatyp == AT_WDGZ) && !haseyes(ptr))
		{
			boolean needs_magr_eyes;
			getgazeinfo(attk->aatyp, attk->adtyp, ptr, (struct monst *) 0, (struct monst *) 0, &needs_magr_eyes, (boolean *)0, (boolean *)0, (boolean *)0);
			if (needs_magr_eyes == TRUE)
			{
				/* remove attack */
				/* shift all further attacks forwards one slot, and make last one all 0s */
				for (j = 0; j < (NATTK - i - 1); j++)
					attk[j] = attk[j + 1];
				attk[j] = noattack;
			}
		}

		/* some templates want to adjust existing attacks, or add additional attacks */
#define insert_okay(specvar) (!(specvar) && (is_null_attk(attk) || \
		((attk->aatyp > AT_HUGS && !weapon_aatyp(attk->aatyp) \
			&& !(attk->aatyp == AT_BREA && ptr->mlet == S_DRAGON)) || attk->aatyp == AT_NONE)) \
	&& (insert = TRUE))
#define end_insert_okay(specvar) (!(specvar) && (is_null_attk(attk) || attk->aatyp == AT_NONE) && (insert = TRUE))
#define maybe_insert() if(insert) {for(j=NATTK-i-1;j>0;j--)attk[j]=attk[j-1];*attk=noattack;insert=FALSE;}
		/* zombies/skeletons get a melee attack if they don't have any (likely due to disallowed aatyp) */
		if ((template == ZOMBIFIED || template == ROT_ZOMBIE || template == SPORE_ZOMBIE || template == SKELIFIED || template == SPARK_SKELETON || template == MINDLESS) && (
			i == 0 && (!nolimbs(ptr) || has_head(ptr)) && (
			is_null_attk(attk) ||
			(attk->aatyp == AT_NONE || attk->aatyp == AT_BOOM)
			) && (insert = TRUE)
			)
		){
			maybe_insert();
			attk->aatyp = !nolimbs(ptr) ? AT_CLAW : AT_BITE;
			attk->adtyp = template == ROT_ZOMBIE ? AD_DISE : AD_PHYS;
			attk->damn = ptr->mlevel / 10 + (template == ZOMBIFIED ? 1 : 2);
			attk->damd = max(ptr->msize * 2, 4);
		}

		/* skeletons get a paralyzing touch */
		if ((template == SKELIFIED || template == SPARK_SKELETON) && (
			insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_TUCH;
			attk->adtyp = AD_SLOW;
			attk->damn = 1;
			attk->damd = max(ptr->msize * 2, 4);
			special = TRUE;
		}
		
		/* spark skeletons get a bolt touch */
		if (template == SPARK_SKELETON && (
			insert_okay(special_2)
			))
		{
			maybe_insert();
			attk->aatyp = AT_TUCH;
			attk->adtyp = AD_ELEC;
			attk->damn = 2;
			attk->damd = max(ptr->msize * 3, 6);
			special_2 = TRUE;
		}
		
		/* vitreans get a cold touch */
		if (template == CRYSTALFIED && (
			insert_okay(special)
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
			end_insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_TENT;
			attk->adtyp = AD_DRST;
			attk->damn = 4;
			attk->damd = 4;
			special = TRUE;
		}
		/* rot zombies get an acid vomit attack */
		if ((template == ROT_ZOMBIE) && !nomouth(ptr->mtyp) && (
			insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_VOMT;
			attk->adtyp = AD_ACID;
			attk->damn = max(1, min(10, ptr->mlevel / 3));
			attk->damd = 6;
			special = TRUE;
		}
		
		/* fractured turn their claws into glass shards */
		if (template == FRACTURED && (
			(attk->aatyp == AT_CLAW && (
			attk->adtyp == AD_PHYS ||
			attk->adtyp == AD_SQUE ||
			attk->adtyp == AD_SAMU
			))
			|| insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_CLAW;
			attk->adtyp = AD_GLSS;
			attk->damn = max(ptr->mlevel / 10 + 1, attk->damn);
			attk->damd = max(ptr->msize * 2, max(attk->damd, 4));
			special = TRUE;
		}
		/* Any bites that rot zombies have are diseased */
		if (template == ROT_ZOMBIE && (
			attk->aatyp == AT_BITE
			|| attk->aatyp == AT_OBIT
			|| attk->aatyp == AT_LNCK
			)
		){
			attk->adtyp = AD_DISE;
			attk->damn = max(1, attk->damn);
			attk->damd = max(4, max(ptr->msize * 2, attk->damd));
		}
		/* vampires' bites are vampiric: pt 1: other bites */
		if (template == VAMPIRIC && (
			attk->aatyp == AT_OBIT
			|| attk->aatyp == AT_LNCK
			)
		){
			attk->adtyp = AD_VAMP;
			attk->damn = max(1, attk->damn);
			attk->damd = max(4, max(ptr->msize * 2, attk->damd));
		}
		/* vampires' bites are vampiric: pt 2: primary bites*/
		if (template == VAMPIRIC && (
			attk->aatyp == AT_BITE
			|| (insert_okay(special) && !nomouth(ptr->mtyp))
			)
		){
			maybe_insert();
			attk->aatyp = AT_BITE;
			attk->adtyp = AD_VAMP;
			attk->damn = max(1, attk->damn);
			attk->damd = max(4, max(ptr->msize * 2, attk->damd));
			special = TRUE;
		}
		/* infectees' bites are sickening: pt 1: other bites */
		if (template == SPORE_ZOMBIE && (
			attk->aatyp == AT_OBIT
			|| attk->aatyp == AT_LNCK
			)
		){
			attk->adtyp = AD_DISE;
			attk->damn = max(1, attk->damn);
			attk->damd = max(4, max(ptr->msize * 2, attk->damd));
		}
		/* infectees' bites are sickening: pt 2: primary bites*/
		if (template == SPORE_ZOMBIE && (
			attk->aatyp == AT_BITE
			|| (insert_okay(special) && !nomouth(ptr->mtyp))
			)
		){
			maybe_insert();
			attk->aatyp = AT_BITE;
			attk->adtyp = AD_DISE;
			attk->damn = max(1, attk->damn);
			attk->damd = max(4, max(ptr->msize * 2, attk->damd));
			special = TRUE;
		}
		/* Cordyceps always have the same attacks */
		if(template == CORDYCEPS){
			if(i==0){
				attk->aatyp = AT_GAZE;
				attk->adtyp = AD_SPOR;
				attk->damn = 0;
				attk->damd = 0;
			}
			else if(i==1){
				attk->aatyp = AT_NONE;
				attk->adtyp = AD_DISE;
				attk->damn = 0;
				attk->damd = 0;
			}
			else {
				/* shift all further attacks forwards one slot, and make last one all 0s */
				for (j = 0; j < (NATTK - i - 1); j++)
					attk[j] = attk[j + 1];
				attk[j] = noattack;
			}
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
			insert_okay(special)
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
			end_insert_okay(special)
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
			attk->aatyp == AT_MAGC || end_insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_MAGC;
			attk->adtyp = AD_PSON;
			attk->damn = 0;
			attk->damd = 15;
			special = TRUE;
		}
		/* psurlons have hands and psionic spellcasting */
		if (template == PSURLON && (
			attk->aatyp == AT_WEAP || insert_okay(special_2)
			)
		){
			maybe_insert();
			attk->aatyp = AT_WEAP;
			attk->adtyp = AD_PHYS;
			attk->damn = 1;
			attk->damd = ptr->msize+1;
			special_2 = TRUE;
		}
		if (template == PSURLON && (
			attk->aatyp == AT_XWEP || insert_okay(special_3)
			))
		{
			maybe_insert();
			attk->aatyp = AT_XWEP;
			attk->adtyp = AD_PHYS;
			attk->damn = 1;
			attk->damd = ptr->msize+1;
			special_3 = TRUE;
		}
		if (template == PSURLON && (
			attk->aatyp == AT_MAGC || end_insert_okay(special)
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
			insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = nohands(ptr) ? AT_ESPR
				: !attacktype(ptr, AT_WEAP) ? AT_SRPR
				: !attacktype(ptr, AT_XWEP) ? AT_XSPR : AT_ESPR;
			attk->adtyp = AD_SHDW;
			attk->damn = 2; //4-6 with undead bonus.
			attk->damd = 8;
			special = TRUE;
		}
		if (template == M_GREAT_WEB && (
			insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_5SQR;
			attk->adtyp = AD_SHDW;
			attk->damn = 8;
			attk->damd = 8;
			special = TRUE;
		}
		/* blood rusts blades */
		if (template == BLOOD_MON && (
				attk->aatyp == AT_NONE || attk->adtyp == AD_RUST
			)
			&& (insert_okay(special))
		){
			maybe_insert();
			attk->aatyp = AT_NONE;
			attk->adtyp = AD_RUST;
			attk->damn = 0;
			attk->damd = 0;
			special = TRUE;
		}
		
		/* slimy monsters get acid attacks */
		if (template == SLIME_REMNANT && 
			i == 0 && (
				is_null_attk(attk) || attk->aatyp == AT_NONE || attk->aatyp == AT_BOOM
			)
		){
			attk->aatyp = (!nolimbs(base) && humanoid_torso(base)) ? AT_CLAW : has_head(base) ? AT_BITE : AT_TUCH;
			attk->adtyp = AD_EACD;
			attk->damn = 2*(ptr->mlevel / 10 + 2);
			attk->damd = max(ptr->msize * 2, 4);
		}
		if (template == SLIME_REMNANT && 
			i == 1 && (!nolimbs(base) && humanoid_torso(base)) && (
				is_null_attk(attk) || attk->aatyp == AT_NONE || attk->aatyp == AT_BOOM
			)
		){
			attk->aatyp = !nolimbs(base) ? AT_CLAW : AT_BITE;
			attk->adtyp = AD_EACD;
			attk->damn = 2*(ptr->mlevel / 10 + 2);
			attk->damd = max(ptr->msize * 2, 4);
		}
		if (template == SLIME_REMNANT && (
			end_insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_NONE;
			attk->adtyp = AD_ACID;
			attk->damn = 10;
			attk->damd = 4;
			special = TRUE;
		}
		if (template == YELLOW_TEMPLATE && !is_null_attk(attk) && attk->adtyp == AD_PHYS){
			attk->damn++;
			attk->damd += 2;
		}
		if (template == GUECUBU && !is_null_attk(attk)){
			if(attk->adtyp == AD_PHYS){
				attk->damn++;
				attk->damd += 2;
			}
			else if(attk->damd > 0){
				if(attk->damn < 6)
					attk->damd += 6;
				else
					attk->damd += 2;
			}
			//Note: overrides the above for magic
			if(attk->aatyp == AT_MAGC || attk->aatyp == AT_MMGC){
				attk->adtyp = AD_PSON;
				attk->damn = (attk->damn+1)/2;
				attk->damd = 15;
			}
		}
		if (template == YELLOW_TEMPLATE && (
			end_insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_TUCH;
			attk->adtyp = AD_SLEE;
			attk->damn = 1;
			attk->damd = 1;
			special = TRUE;
		}
		if (template == MAD_TEMPLATE && !is_null_attk(attk) && attk->adtyp != AD_DISN && attk->adtyp != AD_SURY){
			if(attk->adtyp == AD_PHYS)
				attk->damn++;
			attk->damd += 4;
		}
		if (template == MOLY_TEMPLATE && (
			end_insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_OBIT;
			attk->adtyp = AD_SVPN;
			attk->damn = 6;
			attk->damd = 6;
			special = TRUE;
		}
		if (template == TONGUE_PUPPET && (
			end_insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_TONG;
			attk->adtyp = AD_FATK;
			attk->damn = 1;
			attk->damd = 3;
			special = TRUE;
		}
		/* swollen monsters's attacks are generally stronger */
		if (template == SWOLLEN_TEMPLATE && (
			!is_null_attk(attk))
			)
		{
			int delta = ptr->msize - mons[ptr->mtyp].msize;
			if (attk->damn < 3)
				attk->damd += delta*2;
			else
				attk->damn += delta;

			if(ptr->mmove){
				ptr->mmove /= 2;
				if(ptr->mmove < 6)
					ptr->mmove = 6;
			}
		}
		/* flayed are psychic */
		if (template == FLAYED && (
			(attk->aatyp == AT_MAGC
			|| attk->aatyp == AT_MMGC
			)
			|| insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_MAGC;
			attk->adtyp = AD_PSON;
			attk->damn = (attk->damn+1)/2;
			attk->damd = 15;
			special = TRUE;
		}
		/* Manitou vines */
		if (template == MANITOU && (
			end_insert_okay(special)
			)
		){
			maybe_insert();
			attk->aatyp = AT_VINE;
			attk->adtyp = AD_VAMP;
			attk->damn = (ptr->mlevel+1) / 3;
			if(attk->damn > 10) attk->damn = 10;
			attk->damd = 6;
			special = TRUE;
		}
		if (template == MANITOU && (
			(attk->aatyp == AT_NONE && attk->adtyp == AD_BARB) || insert_okay(special_2)
			)
		){
			maybe_insert();
			attk->aatyp = AT_NONE;
			attk->adtyp = AD_BARB;
			attk->damn = (ptr->mlevel+1) / 3;
			if(attk->damn > 10) attk->damn = 10;
			attk->damd = 4;
			special_2 = TRUE;
		}
	}

	/* pass 3: derive .bodypart for every finalized real attack via the
	 * standard attribution rules. */
	{
		struct atkbp_set resolved[NATTK];
		int nattk = attk_bodyparts_all(ptr, resolved, (boolean *)0);

		for (i = 0; i < nattk; i++)
			ptr->mattk[i].bodypart = resolved[i];
	}

	/* pass 4: attack adjustments with nonstandard attribution rules */
	for (i = 0; i < NATTK; i++) {
		attk = &(ptr->mattk[i]);
		if (is_null_attk(attk))
			break;

		/* Standard limb, but no longer skillfully used */
		if ((template == MINDLESS) &&
			(
			attk->aatyp == AT_HODS ||
			attk->aatyp == AT_DEVA ||
			attk->aatyp == AT_JUGL ||
			attk->aatyp == AT_SRPR ||
			attk->aatyp == AT_XSPR ||
			attk->aatyp == AT_MSPR ||
			attk->aatyp == AT_DSPR ||
			attk->aatyp == AT_MARI ||
			attk->aatyp == AT_WEAP ||
			attk->aatyp == AT_XWEP
			)
		){
			if(attk->aatyp == AT_XWEP || attk->aatyp == AT_XSPR)
				attk->offhand = 1;
			attk->aatyp = AT_CLAW;
			attk->damn = max(attk->damn, ptr->mlevel / 10 + 1);
			attk->damd = max(attk->damd, max(ptr->msize * 2, 4));
		}

		/* Mouth-tentacles */
		if (template == PSEUDONATURAL && (
			(attk->aatyp == AT_BITE)
			|| insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_TENT;
			attk->adtyp = AD_DRIN;
			attk->damn = 1;
			attk->damd = 4;
			attk->bodypart = has_head(ptr)
				? atkbp_or((struct atkbp_set[]){ ATKBP(HEAD), ATKBP(TENTACLE_GENERIC), ATKBP(NONE) })
				: ATKBP(TENTACLE_GENERIC);
			special = TRUE;
		}

		/* Mind parasite */
		if (template == DREAM_LEECH && (
			end_insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_TUCH;
			attk->adtyp = AD_DRIN;
			attk->damn = 1;
			attk->damd = 5;
			attk->bodypart = has_head(ptr)
				? atkbp_or((struct atkbp_set[]){ ATKBP(OTHER_APPENDAGE), ATKBP(HEAD), ATKBP(PHASED), ATKBP(HITS_INSUBSTANTIAL), ATKBP(NONE) })
				: atkbp_or((struct atkbp_set[]){ ATKBP(OTHER_APPENDAGE), ATKBP(PHASED), ATKBP(HITS_INSUBSTANTIAL), ATKBP(NONE) });
			special = TRUE;
		}

		if (template == FALLEN_TEMPLATE && (
			end_insert_okay(special)
			))
		{
			maybe_insert();
			attk->aatyp = AT_NONE;
			attk->adtyp = AD_FIRE;
			attk->damn = 0;
			attk->damd = 9;
			attk->bodypart = ATKBP(HALO);
			special = TRUE;
		}

		/* Mistweavers cast with their tentacles, not their arms */
		if (template == MISTWEAVER && attk->aatyp == AT_MMGC && attk->adtyp == AD_CLRC)
			attk->bodypart = ATKBP(TENTACLE_GENERIC);

		/* A tomb herd strikes with the statue it is piloting, not with a
		 * body of its own -- every attack keeps its ordinary identity, but
		 * the limb making contact is the statue's */
		if (template == TOMB_HERD)
			attk->bodypart = atkbp_or((struct atkbp_set[]){
				attk->bodypart, ATKBP(ARMOR_SKIN), ATKBP(NONE) });
	}
#undef insert_okay
#undef end_insert_okay
#undef maybe_insert

	/* pass 5: summarize the finished body plan. Runs after pass 4, not
	 * with pass 3, so that the overrides above are part of the summary. */
	ptr->bodyparts = mon_bodyparts(ptr, INSIGHT_ALL);

	/*Adjust the name*/
	/* horrors are disallowed out of caution - they definitely break if this is enabled */
	if(!is_horror(&mons[mtyp])){
		Strcpy(nameBuffer, base->mname);
		adjust_permonst_template_desc(base, nameBuffer, template);
		ptr->mname = malloc(sizeof(char)*(strlen(nameBuffer)+1));
		Strcpy((char *)ptr->mname, nameBuffer);
	}
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
	case SLIME_REMNANT:
		/* anything goes */
		return (ptr->mresists&(MR_FIRE|MR_SICK)) != (MR_FIRE|MR_SICK) && ptr->mtyp != PM_GREEN_SLIME && ptr->mtyp != PM_ANCIENT_OF_CORRUPTION;
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
	case MINDLESS:
		/* must have had a mind for the yith to lose, must have a body. */
		return !mindless(ptr) && can_undead(ptr);
	case CRANIUM_RAT:
		/* is a rodent */
		return is_rat(ptr);
	case PSURLON:
		/* is a basic worm */
		return is_basic_worm(ptr);
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
	case YELLOW_TEMPLATE:
		return TRUE;
	case DREAM_LEECH:
		return TRUE;
	case WORLD_SHAPER:
		return (ptr->mtyp != PM_EARTH_ELEMENTAL && ptr->mtyp != PM_WIZARD_OF_YENDOR);
	case POISON_TEMPLATE:
		return is_minion(ptr);
	case MOLY_TEMPLATE:
		return is_cha_demon(ptr);
	case CORDYCEPS:
	case SPORE_ZOMBIE:
		return can_undead(ptr);
	case FLAYED:
		/* makes minimal sense for flaying */
		return !is_naturally_unalive(ptr) && !thick_skinned(ptr);
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
	struct permonst * bas;
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
	bas = &mons[mtyp];
	set_template_data(bas, ptr, template);
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
		horror->spe_wdr = 0;

		horror->mr = rn2(11) * 10;				/* varying amounts of MR */
		horror->maligntyp = d(2, 9) - 10;			/* any alignment */

		/* attacks...?  */
		horrorattacks = 0;
		extern const struct attack noattack;
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
			while (!rn2(3) && horrorattacks<6 && attkptr->aatyp != AT_DEVA && attkptr->aatyp != AT_JUGL && attkptr->aatyp != AT_DSPR) {
				attkptr = &horror->mattk[horrorattacks];
				*attkptr = *(attkptr - 1);

				if (rn2(3) && attkptr->aatyp == AT_WEAP)
					attkptr->aatyp = AT_XWEP;
				else if (rn2(3) && attkptr->aatyp == AT_XWEP)
					attkptr->aatyp = AT_MARI;
				
				/*Magic blade attacks may transition to normal weapon attacks*/
				if (rn2(3) && attkptr->aatyp == AT_SRPR){
					if(rn2(10)){
						attkptr->aatyp = AT_XSPR;
					}
					else {
						attkptr->aatyp = AT_XWEP;
						attkptr->adtyp = get_random_of(randWeaponDamageTypes);
					}
				}
				else if (rn2(3) && attkptr->aatyp == AT_XSPR){
					if(rn2(10)){
						attkptr->aatyp = AT_MSPR;
					}
					else {
						attkptr->aatyp = AT_MARI;
						attkptr->adtyp = get_random_of(randWeaponDamageTypes);
					}
				}

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


			/* int-drain attack uses dice as stat damage, reduce dice to compensate */
			if (attkptr->adtyp == AD_DRIN) {
				attkptr->damn /= 2;	/* 1 - 2 */
				attkptr->damd /= 2;	/* 3 - 5 */
			}

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
			case AT_BRSH:
				attkptr->adtyp = get_random_of(randSplashDamageTypes);
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
		horror->mflagsc = 0;
		horror->mflagsg = 0;
		horror->mflagsa = 0;
		horror->mflagsv = 0;

		for (i = 0; i < rnd(17); i++) {
			horror->mflagsm |= (1L << rn2(33));		/* trainwreck this way :D */
		}
		for (i = 0; i < rnd(17); i++) {
			horror->mflagst |= (1L << rn2(33));
		}
		for (i = 0; i < rnd(17); i++) {
			horror->mflagsb |= (1L << rn2(33));
		}
		for (i = 0; i < rnd(17); i++) {
			horror->mflagsc |= (1L << rn2(33));
		}
		for (i = 0; i < rnd(17); i++) {
			horror->mflagsg |= (1L << rn2(33));
		}
		for (i = 0; i < rnd(17); i++) {
			horror->mflagsa |= (1L << rn2(33));
		}
		for (i = 0; i < rnd(17); i++) {
			horror->mflagsv |= (1L << rn2(33));
		}

		// horror->mflagsc &= ~MC_UNSOLID;			/* no ghosts */
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
			horror->mflagst &= ~(MT_ANIMAL | MT_DETACHED);
		if (horror->mflagst & MT_DETACHED)
			horror->mflagst &= ~(MT_MINDLESS | MT_ANIMAL);
		if (horror->mflagst & MT_ANIMAL)
			horror->mflagst &= ~(MT_MINDLESS | MT_DETACHED);
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

	/* derive .bodypart for every finalized attack via the standard
	 * attribution rules -- NULL for the validation-error out-param since a
	 * malformed horror should not crash the game.
	 */
	{
		struct atkbp_set resolved[NATTK];
		int nattk = attk_bodyparts_all(horror, resolved, (boolean *)0);

		for (i = 0; i < nattk; i++)
			horror->mattk[i].bodypart = resolved[i];
		horror->bodyparts = mon_bodyparts(horror, INSIGHT_ALL);
	}

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
		if (a->ins_req <= Insight && a->aatyp == atyp && (dtyp == AD_ANY || a->adtyp == dtyp)) 
			return a;
	}

    return (struct attack *)0;
}

struct attack *
permonst_dmgtype(ptr, dtyp)
struct permonst *ptr;
int dtyp;
{
    struct attack *a;

    for (a = &ptr->mattk[0]; a < &ptr->mattk[NATTK]; a++){
		if (a->ins_req <= Insight && (dtyp == AD_ANY || a->adtyp == dtyp)) 
			return a;
	}

    return (struct attack *)0;
}

boolean
at_least_one_attack(magr)
struct monst *magr;
{
	struct attack *attk;
	struct attack prev_attk = {0};
	int	indexnum = 0,	/* loop counter */
		subout[SUBOUT_ARRAY_SIZE] = {0},	/* remembers what attack substitutions have been made for [magr]'s attack chain */
		tohitmod = 0,	/* flat accuracy modifier for a specific attack */
		res[4];		/* results of previous 2 attacks ([0] -> current attack, [1] -> 1 ago, [2] -> 2 ago) -- this is dynamic! */

	/* zero out res[] */
	res[0] = MM_MISS;
	res[1] = MM_MISS;
	res[2] = MM_MISS;
	res[3] = MM_MISS;
	
	for(attk = getattk(magr, (struct monst *) 0, res, &indexnum, &prev_attk, FALSE, subout, &tohitmod);
		!is_null_attk(attk);
		attk = getattk(magr, (struct monst *) 0, res, &indexnum, &prev_attk, FALSE, subout, &tohitmod)
	){
		if(attk->aatyp != AT_NONE && attk->aatyp != AT_SPIT && attk->aatyp != AT_BREA
		&& attk->aatyp != AT_BOOM && attk->aatyp != AT_GAZE && attk->aatyp != AT_ARRW
		&& attk->aatyp != AT_TNKR && attk->aatyp != AT_WDGZ && attk->aatyp != AT_REND
		)
			return TRUE;
	}

    return FALSE;
}

boolean
has_multi_or_nonweapon_nonkick_attk(struct monst *magr)
{
	struct attack *attk;
	struct attack prev_attk = {0};
	int real_attk_count = 0;
	int	indexnum = 0,	/* loop counter */
		subout[SUBOUT_ARRAY_SIZE] = {0},	/* remembers what attack substitutions have been made for [magr]'s attack chain */
		tohitmod = 0,	/* flat accuracy modifier for a specific attack */
		res[4];		/* results of previous 2 attacks ([0] -> current attack, [1] -> 1 ago, [2] -> 2 ago) -- this is dynamic! */

	/* zero out res[] */
	res[0] = MM_MISS;
	res[1] = MM_MISS;
	res[2] = MM_MISS;
	res[3] = MM_MISS;
	
	for(attk = getattk(magr, (struct monst *) 0, res, &indexnum, &prev_attk, FALSE, subout, &tohitmod);
		!is_null_attk(attk);
		attk = getattk(magr, (struct monst *) 0, res, &indexnum, &prev_attk, FALSE, subout, &tohitmod)
	){
		if(attk->aatyp != AT_NONE && attk->aatyp != AT_SPIT && attk->aatyp != AT_BREA
		&& attk->aatyp != AT_BOOM && attk->aatyp != AT_GAZE && attk->aatyp != AT_ARRW
		&& attk->aatyp != AT_TNKR && attk->aatyp != AT_WDGZ && attk->aatyp != AT_REND
		&& attk->aatyp != AT_KICK
		){
			real_attk_count++;
			if((attk->aatyp != AT_WEAP) || real_attk_count > 1)
				return TRUE;
		}
	}

    return FALSE;
}

boolean
attacktype(ptr, atyp)
struct permonst *ptr;
int atyp;
{
    return attacktype_fordmg(ptr, atyp, AD_ANY) ? TRUE : FALSE;
}

//Does monster have an attack of type atyp? Use get_attacktype to avoid duplicating code.

boolean
mon_attacktype(mon, atyp)
struct monst *mon;
int atyp;
{
	struct attack prev_attk = {0};

	if(mon_get_attacktype(mon, atyp, &prev_attk))
		return TRUE;

    return FALSE;
}

//Count the number of attacks of type atyp a monster has.

int
mon_count_attacktype(mon, atyp)
struct monst *mon;
int atyp;
{
	struct attack *attk;
	struct attack prev_attk = {0};
	int	indexnum = 0,	/* loop counter */
		subout[SUBOUT_ARRAY_SIZE] = {0},	/* remembers what attack substitutions have been made for [mon]'s attack chain */
		tohitmod = 0,	/* flat accuracy modifier for a specific attack */
		res[4];		/* results of previous 2 attacks ([0] -> current attack, [1] -> 1 ago, [2] -> 2 ago) -- this is dynamic! */
	int counter = 0;

	/* zero out res[] */
	res[0] = MM_MISS;
	res[1] = MM_MISS;
	res[2] = MM_MISS;
	res[3] = MM_MISS;
	
	for(attk = getattk(mon, (struct monst *) 0, res, &indexnum, &prev_attk, TRUE, subout, &tohitmod);
		!is_null_attk(attk);
		attk = getattk(mon, (struct monst *) 0, res, &indexnum, &prev_attk, TRUE, subout, &tohitmod)
	){
		if(attk->aatyp == atyp)
			counter++;
	}

    return counter;
}

//Get a pointer to mon's first attack of type atyp. prev_attk must point to the attack buffer the attack's data should end up in.

struct attack *
mon_get_attacktype(mon, atyp, prev_attk)
struct monst *mon;
int atyp;
struct attack *prev_attk;
{
	struct attack *attk;
	int	indexnum = 0,	/* loop counter */
		subout[SUBOUT_ARRAY_SIZE] = {0},	/* remembers what attack substitutions have been made for [mon]'s attack chain */
		tohitmod = 0,	/* flat accuracy modifier for a specific attack */
		res[4];		/* results of previous 2 attacks ([0] -> current attack, [1] -> 1 ago, [2] -> 2 ago) -- this is dynamic! */

	/* zero out res[] */
	res[0] = MM_MISS;
	res[1] = MM_MISS;
	res[2] = MM_MISS;
	res[3] = MM_MISS;
	
	for(attk = getattk(mon, (struct monst *) 0, res, &indexnum, prev_attk, TRUE, subout, &tohitmod);
		!is_null_attk(attk);
		attk = getattk(mon, (struct monst *) 0, res, &indexnum, prev_attk, TRUE, subout, &tohitmod)
	){
		if(attk->aatyp == atyp)
			return attk;
	}

    return (struct attack *)0;
}

boolean
mon_offhand_attack(mon)
struct monst *mon;
{
	struct attack *attk;
	struct attack prev_attk_buffer = {0};
	int	indexnum = 0,	/* loop counter */
		subout[SUBOUT_ARRAY_SIZE] = {0},	/* remembers what attack substitutions have been made for [mon]'s attack chain */
		tohitmod = 0,	/* flat accuracy modifier for a specific attack */
		res[4];		/* results of previous 2 attacks ([0] -> current attack, [1] -> 1 ago, [2] -> 2 ago) -- this is dynamic! */

	/* zero out res[] */
	res[0] = MM_MISS;
	res[1] = MM_MISS;
	res[2] = MM_MISS;
	res[3] = MM_MISS;
	
	for(attk = getattk(mon, (struct monst *) 0, res, &indexnum, &prev_attk_buffer, TRUE, subout, &tohitmod);
		!is_null_attk(attk);
		attk = getattk(mon, (struct monst *) 0, res, &indexnum, &prev_attk_buffer, TRUE, subout, &tohitmod)
	){
		if(attk->offhand || attk->aatyp == AT_XSPR || (attk->aatyp == AT_XWEP && MON_SWEP(mon)))
			return TRUE;
	}

    return FALSE;
}

boolean
cantwield(mon)
struct monst *mon;
{
	if(mon_attacktype(mon, AT_WEAP) || mon_attacktype(mon, AT_DEVA) || mon_attacktype(mon, AT_JUGL))
		return FALSE;

    return TRUE;
}

boolean
you_cantwield(ptr)
struct permonst *ptr;
{
    struct attack *a;
	
	if(allow_wield(ptr))
		return FALSE;
	
    for (a = &ptr->mattk[0]; a < &ptr->mattk[NATTK]; a++){
		if (a->aatyp == AT_WEAP || a->aatyp == AT_DEVA || a->aatyp == AT_JUGL || a->polywep) 
			return FALSE;
	}
	
	return TRUE;
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
	
	if(mon_vulnerability(mon, FIRE_RES))
		return FALSE;

	if(!mon->mpeaceful)
		return (species_resists_fire(mon) || mon_resistance(mon, FIRE_RES) ||
		        mon_resistance(mon, HELL_FIRE_RES) || mon_resistance(mon, HOLY_FIRE_RES) ||
		        ward_at(mon->mx, mon->my) == SIGIL_OF_CTHUGHA);

	if(mon_resistance(mon, HOLY_FIRE_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Fire_resistance))
		return TRUE;

	if(mon_resistance(mon, HELL_FIRE_RES) && !In_endgame(&u.uz))
		return TRUE;

	if((species_resists_fire(mon) || mon_resistance(mon, FIRE_RES)) && !Inhell && !In_endgame(&u.uz))
		return TRUE;

	if(ward_at(mon->mx, mon->my) == SIGIL_OF_CTHUGHA)
		return TRUE;

	return FALSE;
}

boolean
resists_cold(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	if(mon_vulnerability(mon, COLD_RES))
		return FALSE;

	if(!mon->mpeaceful)
		return (species_resists_cold(mon) || mon_resistance(mon, COLD_RES) ||
		        mon_resistance(mon, HELL_COLD_RES) || mon_resistance(mon, HOLY_COLD_RES) ||
		        ward_at(mon->mx, mon->my) == BRAND_OF_ITHAQUA);

	if(mon_resistance(mon, HOLY_COLD_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Cold_resistance))
		return TRUE;

	if(mon_resistance(mon, HELL_COLD_RES) && !In_endgame(&u.uz))
		return TRUE;

	if((species_resists_cold(mon) || mon_resistance(mon, COLD_RES)) && !Inhell && !In_endgame(&u.uz))
		return TRUE;

	if(ward_at(mon->mx, mon->my) == BRAND_OF_ITHAQUA)
		return TRUE;

	return FALSE;
}

boolean
resists_sleep(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	if(mon_vulnerability(mon, SLEEP_RES))
		return FALSE;
	
	return (species_resists_sleep(mon) || mon_resistance(mon, SLEEP_RES) ||  
		((mon) == u.usteed && u.sealsActive&SEAL_BERITH && Sleep_resistance) || (mon)->cham == CHAM_DREAM);
}

boolean
resists_disint(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	if(mon_vulnerability(mon, DISINT_RES))
		return FALSE;
	
	return (species_resists_disint(mon) || mon_resistance(mon, DISINT_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Disint_resistance));
}

boolean
resists_elec(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	if(mon_vulnerability(mon, SHOCK_RES))
		return FALSE;

	if(!mon->mpeaceful)
		return (species_resists_elec(mon) || mon_resistance(mon, SHOCK_RES) ||
		        mon_resistance(mon, HELL_SHOCK_RES) || mon_resistance(mon, HOLY_SHOCK_RES) ||
		        ward_at(mon->mx, mon->my) == TRACERY_OF_KARAKAL);

	if(mon_resistance(mon, HOLY_SHOCK_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Shock_resistance))
		return TRUE;

	if(mon_resistance(mon, HELL_SHOCK_RES) && !In_endgame(&u.uz))
		return TRUE;

	if((species_resists_elec(mon) || mon_resistance(mon, SHOCK_RES)) && !Inhell && !In_endgame(&u.uz))
		return TRUE;

	if(ward_at(mon->mx, mon->my) == TRACERY_OF_KARAKAL)
		return TRUE;

	return FALSE;
}

boolean
resists_poison(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	if(mon_vulnerability(mon, POISON_RES))
		return FALSE;
	
	return (species_resists_poison(mon) || mon_resistance(mon, POISON_RES) || 
		(mon == u.usteed && u.sealsActive&SEAL_BERITH && Poison_resistance));
}

boolean
resists_acid(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	if(mon_vulnerability(mon, ACID_RES))
		return FALSE;

	if(!mon->mpeaceful)
		return (species_resists_acid(mon) || mon_resistance(mon, ACID_RES) ||
		        mon_resistance(mon, HELL_ACID_RES) || mon_resistance(mon, HOLY_ACID_RES));

	if(mon_resistance(mon, HOLY_ACID_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Acid_resistance))
		return TRUE;

	if(mon_resistance(mon, HELL_ACID_RES) && !In_endgame(&u.uz))
		return TRUE;

	if((species_resists_acid(mon) || mon_resistance(mon, ACID_RES)) && !Inhell && !In_endgame(&u.uz))
		return TRUE;

	return FALSE;
}

boolean
resists_hellfire(mon)
	struct monst *mon;
{
	if (!mon) return FALSE;
	if (mon_vulnerability(mon, FIRE_RES)) return FALSE;
	if (!mon->mpeaceful)
		return (mon_resistance(mon, HELL_FIRE_RES) || mon_resistance(mon, HOLY_FIRE_RES) ||
		        ward_at(mon->mx, mon->my) == SIGIL_OF_CTHUGHA);
	if (mon_resistance(mon, HOLY_FIRE_RES)) return TRUE;
	if (mon_resistance(mon, HELL_FIRE_RES) && !In_endgame(&u.uz)) return TRUE;
	if (ward_at(mon->mx, mon->my) == SIGIL_OF_CTHUGHA) return TRUE;
	return FALSE;
}

boolean
resists_hellcold(mon)
	struct monst *mon;
{
	if (!mon) return FALSE;
	if (mon_vulnerability(mon, COLD_RES)) return FALSE;
	if (!mon->mpeaceful)
		return (mon_resistance(mon, HELL_COLD_RES) || mon_resistance(mon, HOLY_COLD_RES) ||
		        ward_at(mon->mx, mon->my) == BRAND_OF_ITHAQUA);
	if (mon_resistance(mon, HOLY_COLD_RES)) return TRUE;
	if (mon_resistance(mon, HELL_COLD_RES) && !In_endgame(&u.uz)) return TRUE;
	if (ward_at(mon->mx, mon->my) == BRAND_OF_ITHAQUA) return TRUE;
	return FALSE;
}

boolean
resists_hellelec(mon)
	struct monst *mon;
{
	if (!mon) return FALSE;
	if (mon_vulnerability(mon, SHOCK_RES)) return FALSE;
	if (!mon->mpeaceful)
		return (mon_resistance(mon, HELL_SHOCK_RES) || mon_resistance(mon, HOLY_SHOCK_RES) ||
		        ward_at(mon->mx, mon->my) == TRACERY_OF_KARAKAL);
	if (mon_resistance(mon, HOLY_SHOCK_RES)) return TRUE;
	if (mon_resistance(mon, HELL_SHOCK_RES) && !In_endgame(&u.uz)) return TRUE;
	if (ward_at(mon->mx, mon->my) == TRACERY_OF_KARAKAL) return TRUE;
	return FALSE;
}

boolean
resists_hellacid(mon)
	struct monst *mon;
{
	if (!mon) return FALSE;
	if (mon_vulnerability(mon, ACID_RES)) return FALSE;
	if (!mon->mpeaceful)
		return (mon_resistance(mon, HELL_ACID_RES) || mon_resistance(mon, HOLY_ACID_RES));
	if (mon_resistance(mon, HOLY_ACID_RES)) return TRUE;
	if (mon_resistance(mon, HELL_ACID_RES) && !In_endgame(&u.uz)) return TRUE;
	return FALSE;
}

boolean
resists_holyfire(mon)
	struct monst *mon;
{
	if (!mon) return FALSE;
	if (mon_vulnerability(mon, FIRE_RES)) return FALSE;
	if (mon_resistance(mon, HOLY_FIRE_RES)) return TRUE;
	if (ward_at(mon->mx, mon->my) == SIGIL_OF_CTHUGHA) return TRUE;
	return FALSE;
}

boolean
resists_holycold(mon)
	struct monst *mon;
{
	if (!mon) return FALSE;
	if (mon_vulnerability(mon, COLD_RES)) return FALSE;
	if (mon_resistance(mon, HOLY_COLD_RES)) return TRUE;
	if (ward_at(mon->mx, mon->my) == BRAND_OF_ITHAQUA) return TRUE;
	return FALSE;
}

boolean
resists_holyelec(mon)
	struct monst *mon;
{
	if (!mon) return FALSE;
	if (mon_vulnerability(mon, SHOCK_RES)) return FALSE;
	if (mon_resistance(mon, HOLY_SHOCK_RES)) return TRUE;
	if (ward_at(mon->mx, mon->my) == TRACERY_OF_KARAKAL) return TRUE;
	return FALSE;
}

boolean
resists_holyacid(mon)
	struct monst *mon;
{
	if (!mon) return FALSE;
	if (mon_vulnerability(mon, ACID_RES)) return FALSE;
	return mon_resistance(mon, HOLY_ACID_RES);
}

boolean
Slime_res(mon)
struct monst *mon;
{
	struct obj *otmp;
	if(!mon) return FALSE;
	
	if(mon == &youmonst)
		return (Slimed || Unchanging || GoodHealth || flaming(youracedata) 
		|| youracedata->mtyp == PM_ANCIENT_OF_CORRUPTION
		|| youracedata->mtyp == PM_GREEN_SLIME
		|| youracedata->mtyp == PM_FLUX_SLIME
		|| youracedata->mtyp == PM_RED_DRAGON
		|| is_rider(youracedata)
		|| wearing_dragon_armor(mon, PM_RED_DRAGON)
		|| (Race_if(PM_HALF_DRAGON) && flags.HDbreath == AD_FIRE && u.ulevel >= 15)
		);
	//else
	return (Change_res(mon) || mon_resistance(mon, GOOD_HEALTH) || flaming(mon->data) 
		|| mon->mtyp == PM_ANCIENT_OF_CORRUPTION
		|| mon->mtyp == PM_GREEN_SLIME
		|| mon->mtyp == PM_FLUX_SLIME
		|| mon->mtyp == PM_RED_DRAGON
		|| has_template(mon, SLIME_REMNANT)
		|| is_rider(mon->data)
		|| wearing_dragon_armor(mon, PM_RED_DRAGON)
		|| (is_half_dragon(mon->data) && mon->mvar_hdBreath == AD_FIRE && mon->m_lev >= 15)
		);
}

boolean
resists_ston(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	if(mon_vulnerability(mon, STONE_RES))
		return FALSE;
	
	return (species_resists_ston(mon) || mon_resistance(mon, STONE_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Stone_resistance));
}

boolean
resists_drain(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	if(mon_vulnerability(mon, DRAIN_RES))
		return FALSE;
	
	return (species_resists_drain(mon) || mon_resistance(mon, DRAIN_RES) || (mon == u.usteed && u.sealsActive&SEAL_BERITH && Drain_resistance));
}

boolean
resists_sickness(mon)
	struct monst *mon;
{
	if(!mon) return FALSE;
	
	if(mon_vulnerability(mon, SICK_RES))
		return FALSE;
	
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

	if(mon_vulnerability(mon, ANTIMAGIC))
		return FALSE;
	
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
	case AT_WING:
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
				(o->otyp == find_vhelm() || o->otyp == CRYSTAL_HELM || o->otyp == PLASTEEL_HELM || o->otyp == PONTIFF_S_CROWN || o->otyp == FACELESS_HELM || o->otyp == FACELESS_HOOD || (o->otyp == IMPERIAL_ELVEN_HELM && check_imp_mod(o, IEA_BLIND_RES)))
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
ranged_attk(struct permonst *ptr) /* returns TRUE if monster can attack at range */
{
	register int i, atyp;
	long long atk_mask = (1LL << AT_BREA) | (1LL << AT_BRSH) | (1LL << AT_SPIT)
					| (1LL << AT_GAZE) | (1LL << AT_LRCH) | (1LL << AT_LNCK)
					| (1LL << AT_MMGC) | (1LL << AT_TNKR) | (1LL << AT_ARRW)
					| (1LL << AT_BEAM) | (1LL << AT_5SQR) | (1LL << AT_5SBT);

	/* was: (attacktype(ptr, AT_BREA) || attacktype(ptr, AT_WEAP) ||
		attacktype(ptr, AT_SPIT) || attacktype(ptr, AT_GAZE) ||
		attacktype(ptr, AT_MAGC));
	   but that's too slow -dlc
	 */
	for (i = 0; i < NATTK; i++) {
	    atyp = ptr->mattk[i].aatyp;
	    if (atyp >= AT_WEAP) return TRUE;
	 /* assert(atyp < 32); */
	    if ((atk_mask & (1LL << atyp)) != 0L) return TRUE;
	}

	return FALSE;
}

/* true iff the type of monster pass through iron bars */
boolean
passes_bars(mon)
struct monst *mon;
{
	struct permonst *mptr = mon->data;

    return (boolean) (mon_resistance(mon,PASSES_WALLS) || amorphous_mon(mon) ||
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
	if (mtmp == &youmonst && check_mutation(AAT_PRIMINAL))
		return TRUE;
	if (dmgtype(ptr, AD_STCK) || dmgtype(ptr, AD_WRAP) || attacktype(ptr, AT_HUGS))
		return TRUE;
	/* or if wearing the Grappler's Grasp */
	struct obj * gloves = ((mtmp == &youmonst) ? uarmg : which_armor(mtmp, W_ARMG));
	if (gloves && (gloves->oartifact == ART_GRAPPLER_S_GRASP || (gloves->otyp == IMPERIAL_ELVEN_GAUNTLETS && check_imp_mod(gloves, IEA_STRANGLE))))
		return TRUE;

	return FALSE;
}

int
num_horns(struct monst *mon)
{
	if(mon == &youmonst){
		if (check_mutation(TT_UNICORN_HORN)) return 1;
	}
	return mon_horn_count(mon->data);
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
		{ "green steel golem",		PM_GREEN_STEEL_GOLEM },
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
	    /* and now the dnh specific stuff */
	    /* myrmidon monster names are from Greek, so use appropriate plurals */
	        { "myrmidon lochies",	PM_MYRMIDON_LOCHIAS },
	    	{ "myrmidon ypolochagoi",	PM_MYRMIDON_YPOLOCHAGOS },
	    	{ "myrmidon lochagoi",	PM_MYRMIDON_LOCHAGOS },
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
	struct permonst * mdat = mtmp->data;
	if(mtmp->m_ap_type == M_AP_MONSTER) mdat = &mons[mtmp->mappearance];

	if(is_neuter(mdat) || !canspotmon(mtmp)) return 2;
	if(has_template(mtmp, SKELIFIED) && !Role_if(PM_ARCHEOLOGIST))
		return 2;
	if (!(humanoid_torso(mdat) || (mdat->geno & G_UNIQ) ||
		type_is_pname(mdat)))
		return 2;
	if (mtmp->m_ap_type == M_AP_MONSTER) {
		return is_female(mdat) ? 1 : is_male(mdat) ? 0 : mtmp->female;
	}
	return (int)mtmp->female;
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

	/* summons often cannot leave the level they were generated on */
	if (get_mx(mtmp, MX_ESUM) && !mtmp->mextra_p->esum_p->sticky) return FALSE;
	
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
	{PM_STAR_ELF, PM_STAR_EMPEROR}, {PM_STAR_ELF, PM_STAR_EMPRESS},
	{PM_DROW, PM_HEDROW_WARRIOR},
	{PM_DROW, PM_DROW_CAPTAIN}, {PM_DROW_CAPTAIN, PM_DROW_MATRON},
	{PM_NUPPERIBO, PM_METAMORPHOSED_NUPPERIBO}, {PM_METAMORPHOSED_NUPPERIBO, PM_ANCIENT_NUPPERIBO},
	{PM_LICH, PM_DEMILICH}, {PM_DEMILICH, PM_MASTER_LICH},
	{PM_MASTER_LICH, PM_ARCH_LICH},
	{PM_BABY_METROID, PM_METROID},{PM_METROID, PM_ALPHA_METROID}, {PM_ALPHA_METROID, PM_GAMMA_METROID},
	{PM_GAMMA_METROID, PM_ZETA_METROID}, {PM_ZETA_METROID, PM_OMEGA_METROID}, 
	{PM_OMEGA_METROID, PM_METROID_QUEEN},
	{PM_VAMPIRE, PM_VAMPIRE_LORD}, {PM_VAMPIRE, PM_VAMPIRE_LADY}, {PM_BAT, PM_GIANT_BAT},
	{PM_GIANT_BAT, PM_BATTLE_BAT}, {PM_BATTLE_BAT, PM_WARBAT},
	{PM_PLAINS_CENTAUR, PM_CENTAUR_CHIEFTAIN},
	{PM_FOREST_CENTAUR, PM_CENTAUR_CHIEFTAIN},
	{PM_MOUNTAIN_CENTAUR, PM_CENTAUR_CHIEFTAIN},
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
	{PM_CHUUL, PM_ELDER_CHUUL},
	{PM_BABY_CAVE_LIZARD,PM_SMALL_CAVE_LIZARD}, {PM_SMALL_CAVE_LIZARD, PM_CAVE_LIZARD}, {PM_CAVE_LIZARD, PM_LARGE_CAVE_LIZARD},
	{PM_SOLDIER, PM_SERGEANT}, {PM_SERGEANT, PM_LIEUTENANT}, {PM_LIEUTENANT, PM_CAPTAIN},
	{PM_MYRMIDON_HOPLITE, PM_MYRMIDON_LOCHIAS}, {PM_MYRMIDON_LOCHIAS, PM_MYRMIDON_YPOLOCHAGOS}, 
		{PM_MYRMIDON_YPOLOCHAGOS, PM_MYRMIDON_LOCHAGOS},
	{PM_WATCHMAN, PM_WATCH_CAPTAIN},
	{PM_ALIGNED_PRIEST, PM_HIGH_PRIEST},
	{PM_STUDENT, PM_ARCHEOLOGIST},
	{PM_RHYMER, PM_BARD},
	{PM_HEDROW_WIZARD, PM_HEDROW_MASTER_WIZARD},
	{PM_MYRKALFR, PM_MYRKALFAR_WARRIOR},
	{PM_ATTENDANT, PM_HEALER},
	{PM_PAGE, PM_KNIGHT},
	{PM_ACOLYTE, PM_PRIEST},
	{PM_ACOLYTE, PM_PRIESTESS},
	{PM_SECRET_WHISPERER, PM_TRUTH_SEER}, {PM_TRUTH_SEER, PM_DREAM_EATER}, {PM_DREAM_EATER, PM_VEIL_RENDER},
	{PM_APPRENTICE, PM_WIZARD},
	{PM_VALKYRIE, PM_AWAKENED_VALKYRIE}, {PM_AWAKENED_VALKYRIE, PM_TRANSCENDENT_VALKYRIE},
	{PM_DUNGEON_FERN_SPROUT, PM_DUNGEON_FERN},
	{PM_SWAMP_FERN_SPROUT, PM_SWAMP_FERN},
	{PM_BURNING_FERN_SPROUT, PM_BURNING_FERN},
	{PM_SILVERGRUB, PM_SILVERMAN},
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

/*
 * Other than has_wings(youmonst.data)
 */
boolean
u_variable_wings()
{
	if(Race_if(PM_HALF_DRAGON) && (HFlying&FROMRACE)) return TRUE;
	if(TIEFLING_WING_SLOT) return TRUE;

	return FALSE;
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
		amorphous_mon(mon)  ? ooze[capitalize] :
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
		amorphous_mon(mon)  ? ooze[capitalize] :
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

		if(ptr->mlevel < ptr->mattk[i].lev_req)
			continue;

		if(ptr->mattk[i].ins_req > 0)
			continue;

		tmp2 = ptr->mattk[i].aatyp;
		n += (tmp2 > 0);
		n += (tmp2 == AT_MAGC || tmp2 == AT_MMGC ||
			tmp2 == AT_TUCH || tmp2 == AT_TNKR ||
			spirit_rapier_at(tmp2));
		n += (tmp2 == AT_WEAP && (ptr->mflagsb & MB_STRONG));
		if(has_phys_scaling(ptr) && 
		  (ptr->mattk[i].aatyp || ptr->mattk[i].adtyp || ptr->mattk[i].damn || ptr->mattk[i].damd)
		  &&!(
			tmp2 == AT_SPIT
			|| tmp2 == AT_BREA
			|| tmp2 == AT_BRSH
			|| tmp2 == AT_BOOM
			|| tmp2 == AT_GAZE
			|| tmp2 == AT_ARRW
			|| tmp2 == AT_MMGC
			|| tmp2 == AT_TNKR
			|| tmp2 == AT_WDGZ
			|| tmp2 == AT_MAGC
		))
			n += 2;
	}

	/*	For each "special" damage type */
	for (i = 0; i < NATTK; i++) {

		if(ptr->mlevel < ptr->mattk[i].lev_req)
			continue;

		if(ptr->mattk[i].ins_req > 0)
			continue;

		tmp2 = ptr->mattk[i].adtyp;
		if ((tmp2 == AD_DRLI) || (tmp2 == AD_STON) || (tmp2 == AD_DRST)
			|| (tmp2 == AD_DRDX) || (tmp2 == AD_DRCO) || (tmp2 == AD_WERE)
			|| (tmp2 == AD_SVPN)
			|| (tmp2 == AD_SHDW) || (tmp2 == AD_STAR) || (tmp2 == AD_BLUD)
			|| (tmp2 == AD_MOON) || (tmp2 == AD_HOLY) || (tmp2 == AD_UNHY) || (tmp2 == AD_HLUH)
			|| (tmp2 == AD_DETH) || (tmp2 == AD_PEST) || (tmp2 == AD_FAMN) || (tmp2 == AD_CNFT)
			|| (tmp2 == AD_BLAS)
		)
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

/*Gets a fake strength score for a monster.
 * Used for carry cap.
 */
long
mon_str(mon)
struct monst *mon;
{
	struct obj *gloves = which_armor(mon, W_ARMG);
	struct obj *weap = MON_WEP(mon);
	struct obj *xweap = MON_SWEP(mon);
	
	if(gloves && (gloves->otyp == GAUNTLETS_OF_POWER || (gloves->otyp == IMPERIAL_ELVEN_GAUNTLETS && check_imp_mod(gloves, IEA_GOPOWER))))
		return 25L;
	//else
	if(weap){
		if(weap->oartifact == ART_SCEPTRE_OF_MIGHT
		|| (weap->oartifact == ART_PEN_OF_THE_VOID && weap->ovara_seals&SEAL_YMIR && mvitals[PM_ACERERAK].died > 0)
		|| weap->oartifact == ART_STORMBRINGER
		|| weap->oartifact == ART_OGRESMASHER
		)
		return 25L;
	}
	//else
	if(xweap){
		if(xweap->oartifact == ART_OGRESMASHER)
			return 25L;
	}
	//else
	if(strongmonst(mon->data))
		return 18L;
	//else
	return 11L;
}

/*Gets a fake constitution score for a monster.
 * Used for carry cap.
 */
long
mon_con(mon)
struct monst *mon;
{
	struct obj *gloves = which_armor(mon, W_ARMG);
	struct obj *weap = MON_WEP(mon);
	struct obj *xweap = MON_SWEP(mon);
	
	if(gloves && gloves->oartifact == ART_GREAT_CLAWS_OF_URDLEN)
		return 25L;
	//else
	if(weap){
		if(weap->oartifact == ART_OGRESMASHER || weap->oartifact == ART_STORMBRINGER)
			return 25L;
	}
	//else
	if(xweap){
		if(xweap->oartifact == ART_OGRESMASHER)
			return 25L;
	}
	//else
	int norm_max_hp = mon->data->mlevel*8;
	long mcon;
	if(!norm_max_hp)
		norm_max_hp = 4;
	mcon = (18L*mon->mhpmax)/(norm_max_hp);
	if(mcon > 18L)
		mcon = 18L;
	if(mcon < 3L)
		mcon = 3L;
	return mcon;
}

boolean
hiddenwidegaze(magr)
struct monst *magr;
{
	struct obj *cloak = which_armor(magr, W_ARMC);
	if(magr->mtyp == PM_MEDUSA){
		//Face
		struct obj *helm = which_armor(magr, W_ARMH);
		if((helm && is_opaque(helm) && FacelessHelm(helm))
			|| (cloak && is_opaque(cloak) && FacelessCloak(cloak))
		)
			return TRUE;
	}
	else {
		//Body shape
		struct obj *armor = which_armor(magr, W_ARM);
		struct obj *under = which_armor(magr, W_ARMU);
		int mcan = 0;
		if(cloak) mcan = objects[cloak->otyp].a_can;
		if(armor) mcan = max(mcan, objects[armor->otyp].a_can);
		if(under) mcan = max(mcan, objects[under->otyp].a_can);
		if(rn2(3) < mcan)
			return TRUE;
	}
	return FALSE;
}

int
hd_size(ptr)
struct permonst *ptr;
{
	int size = 8;
	if(ptr->mtyp == PM_ZHI_REN_MONK)
		size = 4;
	else if(ptr->mtyp == PM_ANCIENT_OF_DEATH || ptr->mtyp == PM_BAALPHEGOR)
		size = 20;
	else if(ptr->mtyp == PM_TETTIGON_LEGATUS || ptr->mtyp == PM_UNMASKED_TETTIGON || ptr->mtyp == PM_TRANSCENDENT_TETTIGON)
		size = 16;
	else switch(ptr->msize){
		case MZ_TINY:
			size = 4;
		break;
		case MZ_SMALL:
			size = 6;
		break;
		case MZ_MEDIUM:
			size = 8;
		break;
		case MZ_LARGE:
			size = 10;
		break;
		case MZ_HUGE:
			size = 12;
		break;
		case MZ_GIGANTIC:
			size = 20;
		break;
	}

	if(centauroid(ptr) && size < 20){
		size += 2;
	}

	if(is_elf(ptr)){
		size += 3;
	}
	else if(is_drow(ptr)){
		size += 2;
	}
	return size;
}

#endif /* OVLB */

/*mondata.c*/
