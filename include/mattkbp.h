/* NetHack may be freely redistributed.  See license for details. */

#ifndef MATTKBP_H
#define MATTKBP_H

#include "atkbpnames.h"
#include "atkbpbits.h"

/* ATKBP(name) builds a single-bit struct atkbp_set, e.g. ATKBP(HEAD). */
#define ATKBP(name) atkbp_lit(ATKBP_WORD_##name, ATKBP_BIT_##name)

extern struct atkbp_set FDECL(atkbp_lit, (int,long));
extern struct atkbp_set FDECL(atkbp_or, (const struct atkbp_set *));
extern boolean FDECL(atkbp_intersects, (struct atkbp_set,struct atkbp_set));
extern struct atkbp_set FDECL(atkbp_and, (struct atkbp_set,struct atkbp_set));
extern struct atkbp_set FDECL(atkbp_union, (struct atkbp_set,struct atkbp_set));
extern struct atkbp_set FDECL(atkbp_diff, (struct atkbp_set,struct atkbp_set));
extern boolean FDECL(atkbp_is_none, (struct atkbp_set));
extern int FDECL(atkbp_bit_count, (struct atkbp_set));
extern struct atkbp_set FDECL(atkbp_nth_bit, (struct atkbp_set,int));
extern struct atkbp_set FDECL(leg_ordinal_bit, (int));

#define ATKBP_ARM_ORDINALS_MASK() \
	atkbp_or((struct atkbp_set[]){ \
	    ATKBP(ARM_DOMINANT), ATKBP(ARM_OFFHAND), \
	    ATKBP(ARM_3RD), ATKBP(ARM_4TH), ATKBP(ARM_5TH), ATKBP(ARM_6TH), ATKBP(ARM_7TH), ATKBP(ARM_8TH), \
	    ATKBP(NONE) })
#define ATKBP_ARM_LOWER_ORDINALS_MASK() \
	atkbp_or((struct atkbp_set[]){ ATKBP(ARM_LOWER_DOMINANT), ATKBP(ARM_LOWER_OFFHAND), ATKBP(NONE) })

#define ATKBP_MISKA_ARM_ORDINALS_MASK() \
	atkbp_or((struct atkbp_set[]){ ATKBP(MISKA_ARM_1ST), ATKBP(MISKA_ARM_2ND), ATKBP(NONE) })

#define ATKBP_LEG_ORDINALS_MASK() \
	atkbp_or((struct atkbp_set[]){ \
	    ATKBP(LEG_DOMINANT), ATKBP(LEG_OFFHAND), \
	    ATKBP(LEG_3RD), ATKBP(LEG_4TH), ATKBP(LEG_5TH), ATKBP(LEG_6TH), ATKBP(LEG_7TH), ATKBP(LEG_8TH), \
	    ATKBP(NONE) })
#define ATKBP_TENTACLE_ARM_ORDINALS_MASK() \
	atkbp_or((struct atkbp_set[]){ \
	    ATKBP(TENTACLE_ARM_DOMINANT), ATKBP(TENTACLE_ARM_OFFHAND), \
	    ATKBP(TENTACLE_ARM_3RD), ATKBP(TENTACLE_ARM_4TH), ATKBP(TENTACLE_ARM_5TH), ATKBP(TENTACLE_ARM_6TH), \
	    ATKBP(NONE) })
#define ATKBP_HORN_ORDINALS_MASK() \
	atkbp_or((struct atkbp_set[]){ \
	    ATKBP(HORN_1ST), ATKBP(HORN_2ND), \
	    ATKBP(HORN_3RD), ATKBP(HORN_4TH), ATKBP(HORN_5TH), ATKBP(HORN_6TH), \
	    ATKBP(NONE) })

/* Bits that qualify a body part rather than naming one. */
#define ATKBP_MODIFIER_MASK() \
	atkbp_or((struct atkbp_set[]){ \
	    ATKBP(INNUMERABLE), ATKBP(EQUIPPED_LIKE_WING), ATKBP(HIT_WITH_SHIELD), \
	    ATKBP(PHASED), ATKBP(HITS_INSUBSTANTIAL), ATKBP(ARMOR_SKIN), ATKBP(NONE) })

/* Arms past the dominant/offhand pair -- vague ARM counts, since it only
 * survives into a body plan under INNUMERABLE.
 */
#define ATKBP_EXTRA_ARM_MASK() \
	atkbp_or((struct atkbp_set[]){ \
	    ATKBP(ARM_3RD), ATKBP(ARM_4TH), ATKBP(ARM_5TH), \
	    ATKBP(ARM_6TH), ATKBP(ARM_7TH), ATKBP(ARM_8TH), \
	    ATKBP_MISKA_ARM_ORDINALS_MASK(), ATKBP(ARM), ATKBP(NONE) })

#define ATKBP_UPPER_BODY_ARM_MASK() \
	atkbp_or((struct atkbp_set[]){ \
	    ATKBP_ARM_ORDINALS_MASK(), ATKBP_MISKA_ARM_ORDINALS_MASK(), ATKBP(ARM), ATKBP(NONE) })

#define would_straitjacket_block(attk) \
	atkbp_intersects((attk)->bodypart, ATKBP_UPPER_BODY_ARM_MASK())

#define ATKBP_CONCRETE_ARM_MASK() \
	atkbp_or((struct atkbp_set[]){ \
	    ATKBP_ARM_ORDINALS_MASK(), ATKBP_MISKA_ARM_ORDINALS_MASK(), ATKBP_ARM_LOWER_ORDINALS_MASK(), ATKBP(NONE) })

#define ATKBP_LOWER_ARM_MASK() \
	atkbp_or((struct atkbp_set[]){ ATKBP_ARM_LOWER_ORDINALS_MASK(), ATKBP(ARM_LOWER), ATKBP(NONE) })

/* Every part that counts as one of a monster's limbs: concrete ordinals only,
 * so the generic termini (LIMB_GENERIC, TENTACLE_GENERIC, OTHER_APPENDAGE) and
 * the vague family bits are all excluded.
 */
#define ATKBP_LIMB_MASK() \
	atkbp_or((struct atkbp_set[]){ \
	    ATKBP_CONCRETE_ARM_MASK(), ATKBP_TENTACLE_ARM_ORDINALS_MASK(), ATKBP_LEG_ORDINALS_MASK(), \
	    ATKBP(ALIEN_LIMB_1ST), ATKBP(ALIEN_LIMB_2ND), ATKBP(WING), ATKBP(TAIL), \
	    ATKBP(NONE) })

extern struct atkbp_set FDECL(atkbp_spellcast_arm_mask, (struct permonst *));

extern int FDECL(mon_leg_count, (struct permonst *));

extern int FDECL(mon_rear_leg_count, (struct permonst *));

extern int FDECL(atkbp_leg_count, (struct atkbp_set));

extern int FDECL(mon_horn_count, (struct permonst *));

extern struct atkbp_set FDECL(mon_flag_bodyparts, (struct permonst *));

/* mon_bodyparts() insight argument: no gating, i.e. the whole potential body plan. */
#define INSIGHT_ALL	(-1)

extern struct atkbp_set FDECL(mon_bodyparts, (struct permonst *,int));

extern struct atkbp_set FDECL(mon_insight_veil, (struct permonst *,int));

extern boolean FDECL(injuries_would_block_attk, (struct permonst *,struct attack *,struct atkbp_set));

/* struct mon_atkbp: one entry per monster (indexed the same way mons[] is).
 * `attacks[]` mirrors ptr->mattk[] -- same order, same per-attack meaning
 * attkbp[][NATTK] always had. `bodyparts` is this monster's whole body
 * plan as mon_bodyparts() computes it, precomputed so callers don't have
 * to; attkbp_init() copies it into mons[]'s own permonst field.
 */
struct mon_atkbp {
    struct atkbp_set attacks[NATTK];
    struct atkbp_set bodyparts;
};

extern struct mon_atkbp attkbp[];

/* repeated here (not just include/extern.h) since util/attkbpdefs.c, which calls it, doesn't include extern.h */
extern struct atkbp_set FDECL(attk_bodyparts, (struct permonst *,struct attack *));
extern int FDECL(attk_bodyparts_all, (struct permonst *,struct atkbp_set *,boolean *));

/* BLIB_ALT_REAL_INDEX(i): which index into her real mattk[] the i'th entry
 * of blib_alt_attacks[] corresponds to, or -1 for the spliced-in AT_REND
 * (index 2), which has no real analog.
 * BLIB_ALT_ATTACK_COUNT: how many of blib_alt_attacks[]'s NATTK slots are
 * meaningful (the rest are implicit zero-initialized padding).
 */
extern const struct attack blib_alt_attacks[NATTK];
#define BLIB_ALT_REAL_INDEX(i) ((i) < 2 ? (i) : (i) == 2 ? -1 : (i) - 1)
#define BLIB_ALT_ATTACK_COUNT 7

extern const struct attack lolth_alt_attacks[NATTK];
extern struct atkbp_set FDECL(lolth_alt_bodypart, (int));
#define LOLTH_ALT_ATTACK_COUNT 8

#endif /* MATTKBP_H */
