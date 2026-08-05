/* NetHack may be freely redistributed.  See license for details. */

/* atkbp_name_list[]: ordered ATKBP_* vocabulary -- the single source of
 * truth include/atkbpbits.h and util/attkbpdefs.c's symbolic-name table
 * are both built from. Reorder/delete entries freely -- word/bit values
 * are assigned by position (atkbp_assign_bits(), below) and aren't
 * persisted across builds, so there's no compatibility cost.
 */

#include "config.h"
#include "atkbpnames.h"

#include <string.h>

struct atkbp_name_entry atkbp_name_list[] = {
    /* no physical limb: ranged, no-contact, passive, meta */
    { "NONE" },

    /* orthogonal modifier, OR'd onto another bit above: this attack is one
     * of an uncountable/unbounded number of identical body parts (e.g.
     * Legion's arms, Medusa's snake-hair heads) rather than a specific,
     * literally-countable one -- may matter for grapple/restraint logic
     */
    { "INNUMERABLE" },

    /* orthogonal modifier, OR'd onto another bit above: this attack's limb
     * is physically positioned like a wing, so armor-slot logic
     * (attk_protection()/attk_equip_slot()) should treat it as wing-guard
     * (W_ARMW) regardless of its real family -- e.g. lilitu's
     * back-tentacle stingers and the TT_WING_CLAW_3 mutation's wing-tip
     * stinger.
     */
    { "WORN_LIKE_WING" },

    /* orthogonal modifier, OR'd onto another bit above: this attack, despite
     * coming from an otherwise-real/contactable body part, doesn't
     * actually make physical contact -- attk_protection() treats it as
     * ~0L regardless of the underlying identity bit(s), the same way the
     * fixed "no contact" bit set works (MIND_NOLIMB, EYES, ...) but
     * attached per-attack instead of baked into a bit's own meaning.
     * Named to match the existing "Phased non-contact attack" terminology
     * (AT_SRPR/AT_XSPR/AT_MSPR/AT_DSPR/AT_ESPR).
     */
    { "PHASED" },

    /* orthogonal modifier, OR'd onto another bit above: this attack hits an
     * insubstantial target for full damage even when unarmed --
     * hits_insubstantial() returns 2 for it, the same way it already does
     * for a handful of hardcoded adtyp checks. Independent from PHASED --
     * OR both in if an attack needs both.
     */
    { "HITS_INSUBSTANTIAL" },

    /* generic claw/grasp/touch/reach, NOT confirmed to be a true arm/hand
     * (AT_CLAW/AT_HUGS/AT_TUCH/AT_LRCH baseline, unconfirmed)
     */
    { "LIMB_GENERIC" },

    /* generic ranged-attack launcher, NOT confirmed to be an arm/hand or any
     * other specific body part (AT_ARRW baseline, unconfirmed) -- AT_ARRW
     * covers a wide variety of unrelated mechanisms (a drawn bow, a tail
     * spike volley, a built-in construct mechanism, ...); needs a
     * PM_*-scoped override once the actual mechanism for a given monster is
     * known
     */
    { "LAUNCHER_GENERIC" },

    /* confirmed to be a built-in internal mechanism, NOT any kind of limb at
     * all -- distinct from LAUNCHER_GENERIC, which means "we don't know
     * what launches this," not "we know it's not a limb." For constructs
     * (e.g. argentum golem, Arsenal) whose AT_ARRW is a launcher built into
     * the body itself.
     */
    { "MECHANISM_GENERIC" },

    /* confirmed arm/hand, not individually ordinaled -- AT_CLAW/AT_HUGS (if
     * humanoid()), AT_TUCH/AT_LRCH (if humanoid_torso()), AT_MARI/AT_MSPR
     * (category fact only -- see specific-identity bits below), AT_WHIP,
     * AT_HODS, AT_JUGL, AT_DEVA, AT_5SQR, AT_MMGC
     */
    { "ARM" },

    /* AT_KICK for a non-animal-bodied kicker (e.g. a monk's single leg), or
     * an animaloid(ptr)+nohands(ptr) two-legged monster's (e.g. any bird)
     * generic AT_CLAW -- LEG_FRONT would wrongly imply a rear counterpart
     * that a biped doesn't have
     */
    { "LEG" },

    /* one or more of the rear legs, not individually ordinaled -- an
     * MB_ANIMAL-tagged monster's AT_KICK when it has only one such attack
     * (not enough evidence to say which specific rear leg, or how many at
     * once); a monster with 2+ AT_KICK attacks gets concrete LEG_3RD..8TH
     * bits split across them instead. Medium specificity: narrower than
     * the fully generic LEG above, but not individually ordinaled the way
     * LEG_DOMINANT/OFFHAND/3RD..8TH below are.
     */
    { "LEG_REAR" },

    /* one or more of the front legs, not individually ordinaled -- a
     * quadruped's/feline's/lizard's/unicorn's low-precision forelimb
     * claw/stamp when it doesn't fit the dominant/offhand pair below (e.g.
     * a housecat's single claw). Paired with LEG_REAR above; same medium
     * specificity tier. Gated on animaloid(ptr), not just MB_ANIMAL, so
     * centauroid monsters' claws (which come from their humanoid upper
     * body, not a leg) are never attributed here.
     */
    { "LEG_FRONT" },

    /* generic/low-precision lower-body arm, not individually ordinaled --
     * Masked-Queen-style separate lower-arm set (AT_DSPR), when there isn't
     * a specific dominant/offhand pair to name (e.g. the masked queen's own
     * innumerable lower arms). Deliberately NOT in
     * ATKBP_UPPER_BODY_ARM_MASK. See ARM_LOWER_DOMINANT/OFFHAND below for
     * monsters with exactly two, individually-identified lower arms.
     */
    { "ARM_LOWER" },

    /** Specific arms begin here: keep in sync with ATKBP_ARM_ORDINALS_MASK */
    /* dominant (1st/main) arm/hand -- AT_WEAP/AT_SRPR, or polywep override */
    { "ARM_DOMINANT" },

    /* off-hand (2nd) arm/hand -- AT_XWEP/AT_XSPR, or offhand-flag override */
    { "ARM_OFFHAND" },


    /* AT_EXPL/AT_BOOM/AT_ENGL/AT_ILUR -- deliberately ambiguous
     * mouth-or-body for AT_ENGL (no monst.c field reliably separates
     * mouth- from whole-body-engulfers; best proxy, adtyp==AD_DGST, is
     * 29/31 but has real exceptions)
     */
    { "WHOLE_BODY" },

    /* dominant of exactly two individually-identified lower-body arms,
     * e.g. Blibdoolpoolp's mindgraven champion -- deliberately NOT in
     * ATKBP_UPPER_BODY_ARM_MASK. See generic ARM_LOWER above for a monster
     * with lower arms that aren't a clean pair (e.g. the masked queen's
     * innumerable ones). Not adjacent to ARM_LOWER_OFFHAND below (ARM_3RD..
     * ARM_8TH and MISKA_ARM_1ST/2ND sit between them) -- keep both in sync
     * with ATKBP_ARM_LOWER_ORDINALS_MASK.
     */
    { "ARM_LOWER_DOMINANT" },

    /* extra arms beyond dominant+offhand, e.g. a marilith's 3rd arm */
    { "ARM_3RD" },
    { "ARM_4TH" },
    { "ARM_5TH" },
    { "ARM_6TH" },
    { "ARM_7TH" },
    { "ARM_8TH" },
    /** Specific arms end here: keep in sync with ATKBP_ARM_ORDINALS_MASK */

    { "MISKA_ARM_1ST" },
    { "MISKA_ARM_2ND" },

    /* the second lower arm, paired with ARM_LOWER_DOMINANT above -- keep in
     * sync with ATKBP_ARM_LOWER_ORDINALS_MASK
     */
    { "ARM_LOWER_OFFHAND" },

    /* head, non-mouth (AT_BUTT) */
    { "HEAD" },

    /* AT_BUTT for a monster with MB_HORNS, unordinaled -- a single AT_BUTT
     * attack is the vague "one or more of this monster's horns", not HEAD
     * (has_horns_mon() already treats a horned head as mechanically
     * distinct from a bare skull elsewhere, e.g. helmet-fitting)
     */
    { "HORN" },

    /** Specific horns begin here: keep in sync with horn_ordinal_bit() and ATKBP_HORN_ORDINALS_MASK */
    /* 1st horn, for a monster with 2+ AT_BUTT attacks -- e.g. triceratops's
     * three horns map 1:1 onto its three AT_BUTT attacks. Named 1ST/2ND
     * rather than DOMINANT/OFFHAND (unlike the ARM/LEG ordinals) since
     * horns have no handedness to borrow that terminology from.
     */
    { "HORN_1ST" },

    /* the second horn, paired with HORN_1ST */
    { "HORN_2ND" },

    /* 3rd through 6th horn identity -- covers Graz'zt/Fierna/Dracae
     * Eladrin's six lore-horns, the highest mon_horn_count() ever returns
     */
    { "HORN_3RD" },
    { "HORN_4TH" },
    { "HORN_5TH" },
    { "HORN_6TH" },
    /** Specific horns end here: keep in sync with horn_ordinal_bit() and ATKBP_HORN_ORDINALS_MASK */

    /* mouth (AT_BITE/AT_LNCK/AT_5SBT/AT_VOMT/AT_SPIT/AT_BREA/AT_BRSH, and
     * confirmed AT_TENT)
     */
    { "MOUTH" },

    /* tongue specifically (AT_TONG), and flavor-attributed casting (e.g.
     * hounds of Tindalos) -- distinct from the general MOUTH bit since a
     * tongue is its own organ, injurable separately from the rest of the
     * mouth
     */
    { "TONGUE" },

    /* a second, distinct mouth in the torso, separate from the head's own
     * MOUTH -- shayateen's vorpal AT_HUGS is a devouring maw in its gut,
     * not an arm attack
     */
    { "TORSO_MAW" },

    /* AT_BEAM for Keto -- a jetting siphon organ (jellyfish/cnidarian-style
     * water-jet), distinct from both MOUTH and her own AT_TENT tentacles
     */
    { "SIPHON" },

    /* AT_GAZE, and AT_WDGZ except when adtyp==AD_BLND (see HALO below) */
    { "EYES" },

    /* AT_WDGZ specifically when adtyp==AD_BLND -- a blinding radiance, not
     * literally an eye-gaze (angels and other radiant beings -- Throne
     * Archon, Light Archon, Barachiel, "beauteous one", "god", ...)
     */
    { "HALO" },

    /* "blessed" (PM_BLESSED) specifically -- her own unique body part, used
     * both instead of a halo (her AT_WDGZ/AD_BLND attack) and instead of
     * arms for spellcasting (her AT_MAGC attack). Not a general category;
     * PM_BLESSED-scoped overrides only.
     */
    { "HORNED_LIGHT" },

    /* AT_TAIL, and AT_STNG's default -- a scorpion/devil-style jointed
     * tail-stinger
     */
    { "TAIL" },

    /* AT_STNG for an insect-abdomen stinger (bee/wasp/hornet/formian) --
     * no tail in the body plan at all, distinct from TAIL above
     */
    { "STINGER" },

    /* AT_STNG for xan/pisaca -- a piercing proboscis, distinct from both
     * TAIL and STINGER above
     */
    { "PROBOSCIS" },

    /* AT_WING */
    { "WING" },

    /* AT_TENT baseline (not mind-flayer) */
    { "TENTACLE_GENERIC" },

    /** Specific tentacle-arms begin here: keep in sync with
     * tentacle_arm_ordinal_bit()
     */
    /* used for AT_LRCH, AT_TENT (mind flayer family, displacer beast), and
     * AT_5SQR (Dagon/khaamnun tannin) alike -- a monster with only a few
     * noticeably prominent tentacles, distinct from TENTACLE_GENERIC's
     * unspecified mass. Goes up to 6TH (matching HORN's highest
     * lore-count) since khaamnun tannin has five pull-tentacles.
     */
    { "TENTACLE_ARM_DOMINANT" },
    { "TENTACLE_ARM_OFFHAND" },
    { "TENTACLE_ARM_3RD" },
    { "TENTACLE_ARM_4TH" },
    { "TENTACLE_ARM_5TH" },
    { "TENTACLE_ARM_6TH" },
    /** Specific tentacle-arms end here: keep in sync with
     * tentacle_arm_ordinal_bit()
     */

    /* AT_VINE */
    { "VINE" },

    /* AT_ESPR ("just floats"), and AT_MAGC when adtyp==AD_PSON */
    { "MIND_NOLIMB" },

    /* a PC's own animate shadow acting independently -- TT_SHADOW_PAIN,
     * TT_SHADOW_SHRED (src/mutations.c), nachash tannin's AT_ESPR/AD_SHDW
     * (donachash(), src/allmain.c). Inherently non-contact, same as
     * MIND_NOLIMB -- see the ~0L set in attk_protection(), xhityhelpers.c.
     */
    { "SHADOW" },

    /* a Deva-type aasimar's extra arms, manifested from their holy aura --
     * getattk()'s AASIMAR_TYPE_DEVA case. Ethereal like SHADOW/MIND_NOLIMB
     * (~0L in attk_protection(), not straitjacket-blockable), but still
     * has a material offensive character: attk_equip_slot() maps it to
     * W_ARMG explicitly rather than through standard_arm_slot_bodyparts().
     */
    { "AURA_ARM" },

    /* extra marilith-style arms manifested by EILISTRAN_ARMOR's worn body
     * armor -- unlike AURA_ARM these are a real physical strike, but like
     * it they get an explicit W_ARM mapping instead of joining
     * standard_arm_slot_bodyparts(), so the armor's own material/
     * enchantments carry onto the attack even when thrown bare-handed.
     */
    { "ARMOR_ARM" },

    /* AT_OBIT/AT_WBIT -- Medusa's hair, a naga's canopy, a wolf-head
     * weapon, etc; NOT the attacker's own head
     */
    { "OTHER_APPENDAGE" },

    /* AT_LRCH for a supernumerary, aberrant limb sprouting from an
     * otherwise ordinary body -- Daruth Xaxox and drow alienist's
     * spider-like extra legs, not part of their normal (humanoid, two-leg)
     * leg count and not true arms either. 1ST/2ND rather than
     * DOMINANT/OFFHAND, same reasoning as HORN_1ST/2ND above -- an alien
     * leg has no handedness to borrow that terminology from.
     */
    { "ALIEN_LIMB_1ST" },
    { "ALIEN_LIMB_2ND" },

    /* a carried/companion snake with its own attacks -- e.g. Hygieian
     * Archon's caduceus snake. Distinct from OTHER_APPENDAGE (Medusa's
     * hair, a naga's canopy) since it's expected to be independently
     * interactable (injured, grappled) in a future system.
     */
    { "SNAKE" },

    /* a centipede that's a larger, more integral part of the carrying
     * monster than a carried companion like SNAKE above -- e.g. the
     * centipede crawling out of a rotting monk. Also expected to be
     * independently interactable eventually, so gets its own bit rather
     * than folding into SNAKE or OTHER_APPENDAGE.
     */
    { "CENTIPEDE" },

    /** Specific forelegs begin here: keep in sync with leg_ordinal_bit() and ATKBP_LEG_ORDINALS_MASK */
    /* dominant (1st) foreleg claw/stamp, for a quadruped/feline with
     * exactly two claw attacks -- distinct from AT_KICK's hind legs
     */
    { "LEG_DOMINANT" },

    /* the second foreleg, paired with LEG_DOMINANT */
    { "LEG_OFFHAND" },

    /* 3rd through 8th leg identity, for many-legged (e.g. spider)
     * monsters -- LEG_DOMINANT/LEG_OFFHAND above double as legs 1 and 2 of
     * this same 8-leg set
     */
    { "LEG_3RD" },
    { "LEG_4TH" },
    { "LEG_5TH" },
    { "LEG_6TH" },
    { "LEG_7TH" },
    { "LEG_8TH" },
    /** Specific forelegs end here: keep in sync with leg_ordinal_bit() and ATKBP_LEG_ORDINALS_MASK */

    { NULL }
};

int atkbp_nwords;

/* 31 usable bits per word -- 1L<<0 .. 1L<<30, keeping the sign bit
 * (1L<<31) unused.
 */
#define ATKBP_BITS_PER_WORD 31

void
atkbp_assign_bits(void)
{
    int i, word = 0, slot = 0;

    for (i = 0; atkbp_name_list[i].name; i++) {
	if (!strcmp(atkbp_name_list[i].name, "NONE")) {
	    atkbp_name_list[i].word = 0;
	    atkbp_name_list[i].bit = 0L;
	    continue;
	}
	if (slot >= ATKBP_BITS_PER_WORD) {
	    word++;
	    slot = 0;
	}
	atkbp_name_list[i].word = word;
	atkbp_name_list[i].bit = 1L << slot;
	slot++;
    }
    atkbp_nwords = word + 1;
}

/*atkbpnames.c*/
