/* NetHack may be freely redistributed.  See license for details. */

#include "config.h"
#include "permonst.h"
#include "pm.h"
#include "monsym.h"
#include "mondata.h"
#include "mattkbp.h"

/* struct atkbp_set primitives. */
struct atkbp_set
atkbp_lit(int word, long bit)
{
    struct atkbp_set s;
    int i;

    for (i = 0; i < ATKBP_NWORDS; i++)
	s.w[i] = 0L;
    s.w[word] = bit;
    return s;
}

boolean
atkbp_is_none(struct atkbp_set s)
{
    int i;

    for (i = 0; i < ATKBP_NWORDS; i++)
	if (s.w[i] != 0L)
	    return FALSE;
    return TRUE;
}

/* ORs together a list of sets, word-wise, stopping at the first all-zero
 * (ATKBP(NONE)-valued) entry -- callers pass the list as a C99 compound
 * literal array terminated with ATKBP(NONE), e.g.:
 *   atkbp_or((struct atkbp_set[]){ ATKBP(HEAD), ATKBP(MOUTH), ATKBP(NONE) })
 * Never place a set that might itself be empty anywhere but the final
 * (terminator) slot, or the scan stops early and silently drops everything
 * after it.
 */
struct atkbp_set
atkbp_or(const struct atkbp_set *sets)
{
    struct atkbp_set result;
    int i, j;

    for (j = 0; j < ATKBP_NWORDS; j++)
	result.w[j] = 0L;
    for (i = 0; !atkbp_is_none(sets[i]); i++)
	for (j = 0; j < ATKBP_NWORDS; j++)
	    result.w[j] |= sets[i].w[j];
    return result;
}

boolean
atkbp_intersects(struct atkbp_set a, struct atkbp_set b)
{
    int i;

    for (i = 0; i < ATKBP_NWORDS; i++)
	if (a.w[i] & b.w[i])
	    return TRUE;
    return FALSE;
}

/* claw_count()/claw_ordinal(): how many of ptr's attacks are AT_CLAW and
 * not already claimed by the offhand/polywep flags (those short-circuit
 * above before either is ever consulted), and which one (0-based) is
 * `attk`? claw_ordinal() returns -1 if attk isn't literally one of
 * ptr->mattk[] (e.g. a synthesized attack at a runtime call site).
 */
static int
claw_count(struct permonst *ptr)
{
    int i, count = 0;

    for (i = 0; i < NATTK; i++) {
	struct attack *cur = &ptr->mattk[i];

	if (cur->aatyp == AT_CLAW && !cur->offhand && !cur->polywep)
	    count++;
    }
    return count;
}

static int
claw_ordinal(struct permonst *ptr, struct attack *attk)
{
    int i, ord = 0;

    for (i = 0; i < NATTK; i++) {
	struct attack *cur = &ptr->mattk[i];

	if (cur->aatyp != AT_CLAW || cur->offhand || cur->polywep)
	    continue;
	if (cur == attk)
	    return ord;
	ord++;
    }
    return -1;
}

/* kick_count()/kick_ordinal(): how many of ptr's attacks are AT_KICK, and
 * which one (0-based) is `attk`? Used to split multiple AT_KICK attacks
 * across a monster's rear legs -- see the AT_KICK case below.
 */
static int
kick_count(struct permonst *ptr)
{
    int i, count = 0;

    for (i = 0; i < NATTK; i++)
	if (ptr->mattk[i].aatyp == AT_KICK)
	    count++;
    return count;
}

static int
kick_ordinal(struct permonst *ptr, struct attack *attk)
{
    int i, ord = 0;

    for (i = 0; i < NATTK; i++) {
	struct attack *cur = &ptr->mattk[i];

	if (cur->aatyp != AT_KICK)
	    continue;
	if (cur == attk)
	    return ord;
	ord++;
    }
    return -1;
}

/* leg_ordinal_bit(): the ATKBP_LEG_* bit for the `leg_number`th leg (1 ==
 * LEG_DOMINANT, 2 == LEG_OFFHAND, 3..8 == LEG_3RD..LEG_8TH). Used to turn a
 * computed leg number (see the AT_KICK case below) into the matching
 * concrete bit.
 */
static struct atkbp_set
leg_ordinal_bit(int leg_number)
{
    switch (leg_number) {
    case 1: return ATKBP(LEG_DOMINANT);
    case 2: return ATKBP(LEG_OFFHAND);
    case 3: return ATKBP(LEG_3RD);
    case 4: return ATKBP(LEG_4TH);
    case 5: return ATKBP(LEG_5TH);
    case 6: return ATKBP(LEG_6TH);
    case 7: return ATKBP(LEG_7TH);
    case 8: return ATKBP(LEG_8TH);
    }
    return ATKBP(NONE);
}

/* arm_ordinal_bit(): the ATKBP_ARM_* bit for the `arm_number`th arm (1 ==
 * ARM_DOMINANT, 2 == ARM_OFFHAND, 3..8 == ARM_3RD..ARM_8TH). Mirrors
 * leg_ordinal_bit() below -- replaces hand-written index->bit switches
 * wherever one turns up (e.g. lolth_alt_bodypart()/Shaktari's case, both
 * below).
 */
static struct atkbp_set
arm_ordinal_bit(int arm_number)
{
    switch (arm_number) {
    case 1: return ATKBP(ARM_DOMINANT);
    case 2: return ATKBP(ARM_OFFHAND);
    case 3: return ATKBP(ARM_3RD);
    case 4: return ATKBP(ARM_4TH);
    case 5: return ATKBP(ARM_5TH);
    case 6: return ATKBP(ARM_6TH);
    case 7: return ATKBP(ARM_7TH);
    case 8: return ATKBP(ARM_8TH);
    }
    return ATKBP(NONE);
}

static struct atkbp_set
horn_ordinal_bit(int horn_number)
{
    switch (horn_number) {
    case 1: return ATKBP(HORN_1ST);
    case 2: return ATKBP(HORN_2ND);
    case 3: return ATKBP(HORN_3RD);
    case 4: return ATKBP(HORN_4TH);
    case 5: return ATKBP(HORN_5TH);
    case 6: return ATKBP(HORN_6TH);
    }
    return ATKBP(NONE);
}

/* tentacle_arm_ordinal_bit(): the ATKBP_TENTACLE_ARM_* bit for the
 * `tentacle_number`th major/prominent tentacle (1 == TENTACLE_ARM_DOMINANT,
 * 2 == TENTACLE_ARM_OFFHAND, 3..6 == TENTACLE_ARM_3RD..TENTACLE_ARM_6TH).
 * Used to turn a computed tentacle number into the matching concrete bit --
 * see the AT_TENT/AT_5SQR special cases below.
 */
static struct atkbp_set
tentacle_arm_ordinal_bit(int tentacle_number)
{
    switch (tentacle_number) {
    case 1: return ATKBP(TENTACLE_ARM_DOMINANT);
    case 2: return ATKBP(TENTACLE_ARM_OFFHAND);
    case 3: return ATKBP(TENTACLE_ARM_3RD);
    case 4: return ATKBP(TENTACLE_ARM_4TH);
    case 5: return ATKBP(TENTACLE_ARM_5TH);
    case 6: return ATKBP(TENTACLE_ARM_6TH);
    }
    return ATKBP(NONE);
}

/* butt_count()/butt_ordinal(): how many of ptr's attacks are AT_BUTT, and
 * which one (0-based) is `attk`? Used to split multiple AT_BUTT attacks
 * across a horned monster's individual horns -- see the AT_BUTT case below.
 */
static int
butt_count(struct permonst *ptr)
{
    int i, count = 0;

    for (i = 0; i < NATTK; i++)
	if (ptr->mattk[i].aatyp == AT_BUTT)
	    count++;
    return count;
}

static int
butt_ordinal(struct permonst *ptr, struct attack *attk)
{
    int i, ord = 0;

    for (i = 0; i < NATTK; i++) {
	struct attack *cur = &ptr->mattk[i];

	if (cur->aatyp != AT_BUTT)
	    continue;
	if (cur == attk)
	    return ord;
	ord++;
    }
    return -1;
}

static boolean
immediately_followed_by_offhand_attack(struct permonst *ptr, struct attack *attk)
{
    int i;

    for (i = 0; i < NATTK - 1; i++) {
	if (&ptr->mattk[i] == attk)
	    return ptr->mattk[i + 1].offhand;
    }
    return FALSE;
}

/* claw_immediately_follows_offhand_claw(): is `attk` immediately preceded,
 * in ptr->mattk[], by an offhand-flagged AT_CLAW attack? Used for the
 * S_XORN "radially symmetric" 3-arm pattern -- mainhand claw, offhand
 * claw, third claw -- below.
 */
static boolean
claw_immediately_follows_offhand_claw(struct permonst *ptr, struct attack *attk)
{
    int i;

    for (i = 1; i < NATTK; i++) {
	if (&ptr->mattk[i] == attk)
	    return ptr->mattk[i - 1].aatyp == AT_CLAW
		   && ptr->mattk[i - 1].offhand;
    }
    return FALSE;
}

/* attack_index(): 0-based position of `attk` in ptr->mattk[], or -1 if
 * attk isn't literally one of ptr->mattk[].
 */
static int
attack_index(struct permonst *ptr, struct attack *attk)
{
    int i;

    for (i = 0; i < NATTK; i++)
	if (&ptr->mattk[i] == attk)
	    return i;
    return -1;
}

struct atkbp_set
attk_bodyparts(struct permonst *ptr, struct attack *attk)
{
    /* pure function of (permonst*, attack*) -- no live game state, no
     * array-scan, so AT_MARI/AT_MSPR only ever get the category ATKBP_ARM
     * bit here; their specific ordinal identity bit is assigned only by
     * the generator (util/attkbpdefs.c's do_attkbp()), which can scan a
     * monster's whole attack list to count them.
     */

    /* hooloovoos are so formless not even a generic limb is responsible --
     * checked before the polywep case below so it overrides that too.
     */
    if (ptr->mtyp == PM_HOOLOOVOO)
	return ATKBP(NONE);

    /* moon flea's AT_LRCH is POLYWEP_ATTK-flagged (its bite is dexterous
     * enough to hold a weapon), but the reach itself is still its own
     * blood-drinking tongue, not a hand -- checked before the polywep case
     * below for the same reason as hooloovoo above.
     */
    if (ptr->mtyp == PM_MOON_FLEA && attk->aatyp == AT_LRCH)
	return ATKBP(TONGUE);

    /* polywep ("wielding while polymorphed") only applies to hand-delivered
     * attacks -- a bite/rend/hug/tentacle stays what it is regardless. A
     * nohands(ptr) monster can still wield this way, but that doesn't make
     * it an arm, so it falls through to ordinary (non-polywep) attribution.
     */
    if (attk->polywep
	    && !nohands(ptr)
	    && attk->aatyp != AT_BITE
	    && attk->aatyp != AT_REND
	    && attk->aatyp != AT_HUGS
	    && attk->aatyp != AT_TENT)
	return ATKBP(ARM_DOMINANT);
    /* ancient of corruption's AT_WHIP (dominant and offhand-flagged both)
     * is whipping tentacles, not arms -- checked before the offhand check
     * below so it overrides that too, the same reason the hooloovoo case
     * above precedes the polywep check.
     */
    if (ptr->mtyp == PM_ANCIENT_OF_CORRUPTION && attk->aatyp == AT_WHIP)
	return ATKBP(TENTACLE_GENERIC);
    /* AT_TUCH/AT_LRCH/AT_TENT are excluded -- offhand only means "the
     * offhand arm" if this attack type was already going to be an arm at
     * all (their own case below checks attk->offhand once it knows that).
     * Blindly assuming ARM_OFFHAND broke yochlol's tentacle and
     * Voice-In-Screams's non-arm touch. Every other offhand-flagged type
     * keeps the unconditional assumption (e.g. a scorpion's pincers are
     * arm-analogous even on an otherwise leg-attributed claw).
     */
    if (attk->offhand
	    && attk->aatyp != AT_TUCH && attk->aatyp != AT_LRCH
	    && attk->aatyp != AT_TENT)
	return ATKBP(ARM_OFFHAND);

    /* daat sephirah has 6 arms that attack (4 AT_TUCH, 2 AT_CLAW) plus a
     * 7th, shield-only arm that never attacks and so needs no bit here --
     * none of the 6 are offhand-flagged, so that unused offhand slot is
     * skipped entirely: main arm, then 5 more (3rd through 7th).
     */
    if (ptr->mtyp == PM_DAAT_SEPHIRAH
	    && (attk->aatyp == AT_TUCH || attk->aatyp == AT_CLAW)) {
	switch (attack_index(ptr, attk)) {
	case 0: return ATKBP(ARM_DOMINANT);
	case 1: return ATKBP(ARM_3RD);
	case 2: return ATKBP(ARM_4TH);
	case 3: return ATKBP(ARM_5TH);
	case 4: return ATKBP(ARM_6TH);
	case 5: return ATKBP(ARM_7TH);
	}
    }

    /* Shaktari has 8 arms, each swinging her one wielded weapon (a magical
     * effect, not 6 spare weapons the way a real marilith's AT_MARI arms
     * work) -- her 8 AT_WEAP attacks are 8 distinct limbs, not one arm
     * swinging 8 times, so every named concrete arm bit gets used.
     */
    if (ptr->mtyp == PM_SHAKTARI && attk->aatyp == AT_WEAP)
	return arm_ordinal_bit(attack_index(ptr, attk) + 1);

    /* hounds of Tindalos cast with their tongues, not an arm they don't
     * have.
     */
    if (ptr->mtyp == PM_HOUND_OF_TINDALOS && attk->aatyp == AT_MAGC)
	return ATKBP(TONGUE);

    /* Light Archon's AT_WEAP already resolves to ARM_DOMINANT below; her
     * arrows use a second pair (3rd/4th) and her casting a third (5th/6th)
     * -- fixed pairs, not "every arm", so this bypasses the generic
     * spellcasting-arm machinery below entirely.
     */
    if (ptr->mtyp == PM_LIGHT_ARCHON) {
	if (attk->aatyp == AT_ARRW)
	    return atkbp_or((struct atkbp_set[]){ ATKBP(ARM_3RD), ATKBP(ARM_4TH), ATKBP(NONE) });
	if (attk->aatyp == AT_MAGC)
	    return atkbp_or((struct atkbp_set[]){ ATKBP(ARM_5TH), ATKBP(ARM_6TH), ATKBP(NONE) });
    }

    /* Hygieian Archon's caduceus snake gets the dedicated SNAKE identity
     * for all 4 of its attacks, not the generic OTHER_APPENDAGE bucket
     * (Medusa's hair uses) -- expected to be independently interactable
     * later, unlike a wielded head-weapon or a mass of hair.
     */
    if (ptr->mtyp == PM_HYGIEIAN_ARCHON
	    && (attk->aatyp == AT_OBIT || attk->aatyp == AT_TUCH
		|| attk->aatyp == AT_CLAW || attk->aatyp == AT_HUGS))
	return ATKBP(SNAKE);

    /* rotting monk has a centipede crawling out of it -- its wrapping claw
     * and bite (AT_OBIT/AD_DRST) both belong to the centipede, not the
     * monk's own body. AT_SQUZ isn't listed here -- it inherits whichever
     * bits its AD_WRAP sibling (the claw) gets via squz_inherited_bits()
     * (util/attkbpdefs.c), so it picks up CENTIPEDE automatically.
     */
    if (ptr->mtyp == PM_ROTTING_MONK
	    && (attk->aatyp == AT_CLAW || attk->aatyp == AT_OBIT))
	return ATKBP(CENTIPEDE);

    /* priest of Ghaunadaur is an amorphous blob whose other attacks
     * (AT_TUCH, AT_HUGS) already resolve as generic/unconfirmed limbs, not
     * humanoid enough to earn a true arm -- its spellcasting is the same
     * generic limb, not a concrete arm.
     */
    if (ptr->mtyp == PM_PRIEST_OF_GHAUNADAUR && attk->aatyp == AT_MAGC)
	return ATKBP(LIMB_GENERIC);

    /* spellweavers have 6 arms and cast in pairs -- each of their 3 AT_MAGC
     * attacks is two arms working together, not one arm per cast.
     */
    if (ptr->mtyp == PM_SPELLWEAVER && attk->aatyp == AT_MAGC) {
	switch (attack_index(ptr, attk)) {
	case 0: return atkbp_or((struct atkbp_set[]){ ATKBP(ARM_DOMINANT), ATKBP(ARM_OFFHAND), ATKBP(NONE) });
	case 1: return atkbp_or((struct atkbp_set[]){ ATKBP(ARM_3RD), ATKBP(ARM_4TH), ATKBP(NONE) });
	case 2: return atkbp_or((struct atkbp_set[]){ ATKBP(ARM_5TH), ATKBP(ARM_6TH), ATKBP(NONE) });
	}
    }

    /* the goddess-mocker variant has all 6 of the same arms, but casts one
     * arm per attack instead of pairing them up.
     */
    if (ptr->mtyp == PM_SPELLWEAVER_GODDESS_MOCKER && attk->aatyp == AT_MAGC) {
	switch (attack_index(ptr, attk)) {
	case 0: return ATKBP(ARM_DOMINANT);
	case 1: return ATKBP(ARM_OFFHAND);
	case 2: return ATKBP(ARM_3RD);
	case 3: return ATKBP(ARM_4TH);
	case 4: return ATKBP(ARM_5TH);
	case 5: return ATKBP(ARM_6TH);
	}
    }

    switch (attk->aatyp) {
    case AT_WEAP:
    case AT_SRPR:
	return ATKBP(ARM_DOMINANT);
    case AT_XWEP:
    case AT_XSPR:
	return ATKBP(ARM_OFFHAND);
    case AT_CLAW:
	if (attk->aatyp == AT_CLAW && ptr->mtyp == PM_VERMIURGE) {
	    switch (claw_ordinal(ptr, attk)) {
	    case 0: return ATKBP(ARM_DOMINANT);
	    case 1: return ATKBP(ARM_3RD);
	    case 2: return ATKBP(ARM_4TH);
	    }
	}
	/* the yurian's dominant/offhand pair (ordinal 0, handled by
	 * immediately_followed_by_offhand_attack() below) is followed by two
	 * more claws that are extra limbs, not generic/unconfirmed ones.
	 */
	if (attk->aatyp == AT_CLAW && ptr->mtyp == PM_YURIAN) {
	    switch (claw_ordinal(ptr, attk)) {
	    case 1: return ATKBP(ARM_3RD);
	    case 2: return ATKBP(ARM_4TH);
	    }
	}
	/* S_XORN ("radially symmetric organism") with a mainhand claw, an
	 * offhand claw, and an immediately-following third claw gets that
	 * third claw as a true 3rd arm rather than generic.
	 */
	if (attk->aatyp == AT_CLAW && ptr->mlet == S_XORN
		&& claw_immediately_follows_offhand_claw(ptr, attk))
	    return ATKBP(ARM_3RD);
	/* the bestial/ethereal dervishes are S_XORN too (so their first
	 * three claws already resolve via the rule above) but each has a
	 * 4th claw beyond that, which is a 4th arm rather than generic.
	 */
	if (attk->aatyp == AT_CLAW &&
		(ptr->mtyp == PM_BESTIAL_DERVISH || ptr->mtyp == PM_ETHEREAL_DERVISH)) {
	    switch (claw_ordinal(ptr, attk)) {
	    case 2: return ATKBP(ARM_4TH);
	    }
	}
	/* teraphim tannah (6 arms) and ethereal filcher (4), neither S_XORN --
	 * polywep dominant + offhand claw (resolved above), then trailing
	 * bare claws each a distinct extra arm: 2 more for the filcher, 4
	 * more for teraphim tannah.
	 */
	if (attk->aatyp == AT_CLAW &&
		(ptr->mtyp == PM_TERAPHIM_TANNAH || ptr->mtyp == PM_ETHEREAL_FILCHER)) {
	    switch (claw_ordinal(ptr, attk)) {
	    case 0: return ATKBP(ARM_3RD);
	    case 1: return ATKBP(ARM_4TH);
	    case 2: return ATKBP(ARM_5TH);
	    case 3: return ATKBP(ARM_6TH);
	    }
	}
	/* Chaos has a weapon dominant/offhand pair (handled above) plus two
	 * more bare claws that are a 3rd and 4th arm -- 4 arms total.
	 */
	if (attk->aatyp == AT_CLAW && ptr->mtyp == PM_CHAOS) {
	    switch (claw_ordinal(ptr, attk)) {
	    case 0: return ATKBP(ARM_3RD);
	    case 1: return ATKBP(ARM_4TH);
	    }
	}
	/* the parasitized doll's standard dominant/offhand arms are present
	 * but never attack -- its two claws are a 3rd and 4th (parasite)
	 * limb instead.
	 */
	if (attk->aatyp == AT_CLAW && ptr->mtyp == PM_PARASITIZED_DOLL) {
	    switch (claw_ordinal(ptr, attk)) {
	    case 0: return ATKBP(ARM_3RD);
	    case 1: return ATKBP(ARM_4TH);
	    }
	}
	/* Blibdoolpoolp's mindgraven champion has a weapon dominant/offhand
	 * pair (handled above) plus two more bare claws that are a separate
	 * dominant/offhand lower-body arm pair, not upper-body extra arms.
	 */
	if (attk->aatyp == AT_CLAW
		&& ptr->mtyp == PM_BLIBDOOLPOOLP_S_MINDGRAVEN_CHAMPION) {
	    switch (claw_ordinal(ptr, attk)) {
	    case 0: return ATKBP(ARM_LOWER_DOMINANT);
	    case 1: return ATKBP(ARM_LOWER_OFFHAND);
	    }
	}
	/* Stranger and Suzerain are humanoid, so their claws would default
	 * to ATKBP_ARM, but their claws aren't confirmed arms -- downgrade
	 * to the generic/unconfirmed limb attribution.
	 */
	if (attk->aatyp == AT_CLAW
		&& (ptr->mtyp == PM_STRANGER || ptr->mtyp == PM_SUZERAIN))
	    return ATKBP(LIMB_GENERIC);
	/* the metroid's AT_CLAW/AD_STCK "claw" is really its latching mouth,
	 * not a limb -- it's MB_NOLIMBS/MB_NOHANDS.
	 */
	if (attk->aatyp == AT_CLAW && attk->adtyp == AD_STCK
		&& ptr->mtyp == PM_METROID)
	    return ATKBP(MOUTH);
	/* Legion has innumerable arms -- still an unconfirmed/generic limb
	 * (not humanoid), but flagged as one of uncountably many.
	 */
	if (attk->aatyp == AT_CLAW && ptr->mtyp == PM_LEGION)
	    return atkbp_or((struct atkbp_set[]){ ATKBP(LIMB_GENERIC), ATKBP(INNUMERABLE), ATKBP(NONE) });
	/* the rage-walker's first two claws are both dominant (a double
	 * strike), then offhand, then two generic seduction claws -- unlike
	 * the general rule below, both leading claws are dominant, not just
	 * the one before offhand.
	 */
	if (attk->aatyp == AT_CLAW && ptr->mtyp == PM_RAGE_WALKER) {
	    switch (claw_ordinal(ptr, attk)) {
	    case 0:
	    case 1: return ATKBP(ARM_DOMINANT);
	    }
	}
	if (attk->aatyp == AT_CLAW && immediately_followed_by_offhand_attack(ptr, attk))
	    return ATKBP(ARM_DOMINANT);
	/* vrock's flavor text says it claws with both arms and feet, with no
	 * clean way to say "confirmed both, ambiguous which" -- resolved to
	 * LEG. Ossifruge is structurally identical and gets the same
	 * treatment. Crow-winged half-dragon and Aglaope are MB_HUMANOID but
	 * share the same MB_WINGS talon-footed reasoning.
	 */
	if (attk->aatyp == AT_CLAW
		&& (ptr->mtyp == PM_VROCK || ptr->mtyp == PM_OSSIFRUGE
		    || ptr->mtyp == PM_CROW_WINGED_HALF_DRAGON
		    || ptr->mtyp == PM_AGLAOPE))
	    return ATKBP(LEG);
	/* every animaloid(ptr) monster gets one of these two branches, placed
	 * below every other AT_CLAW special case so it only catches what
	 * nothing more specific already claimed. Which one depends on
	 * whether the animal has humanoid hands (!nogloves(ptr)) vs.
	 * non-humanoid manipulatory appendages or no manipulatory appendages
	 * at all (when nogloves(ptr) is TRUE).
	 */
	if (attk->aatyp == AT_CLAW && animaloid(ptr) && !nogloves(ptr)) {
	    if (claw_count(ptr) == 2) {
		switch (claw_ordinal(ptr, attk)) {
		case 0: return ATKBP(ARM_DOMINANT);
		case 1: return ATKBP(ARM_OFFHAND);
		}
	    }
	    return ATKBP(ARM);
	}
	if (attk->aatyp == AT_CLAW && animaloid(ptr)) {
	    if (claw_count(ptr) == 2) {
		switch (claw_ordinal(ptr, attk)) {
		case 0: return ATKBP(LEG_DOMINANT);
		case 1: return ATKBP(LEG_OFFHAND);
		}
	    }
	    /* LEG_FRONT implies a rear counterpart -- only true when there's
	     * an actual front/rear split (mon_rear_leg_count(ptr) > 0); a
	     * two-legged monster (any bird, per mon_leg_count() above) has
	     * no such counterpart, so it's the plain generic LEG instead.
	     */
	    return mon_rear_leg_count(ptr) > 0 ? ATKBP(LEG_FRONT) : ATKBP(LEG);
	}
	/* same nohands()-aware fallback as AT_HUGS's default just below --
	 * a humanoid(ptr) monster with no hands at all still isn't a
	 * confirmed arm.
	 */
	return (humanoid(ptr) && !nohands(ptr)) ? ATKBP(ARM) : ATKBP(LIMB_GENERIC);
    case AT_HUGS: {
	/* AT_HUGS requires the two attacks immediately preceding it to both
	 * hit this turn, so by default it's performed by whichever
	 * appendages those used (the same two-predecessor rule
	 * squz_inherited_bits(), util/attkbpdefs.c, follows for AT_SQUZ).
	 * pisaca and raglayim tannin are slithy(ptr) and would otherwise hit
	 * the serpent-tail rule below, which doesn't fit either.
	 */
	if (ptr->mtyp == PM_PISACA)
	    return ATKBP(TENTACLE_GENERIC);
	if (ptr->mtyp == PM_RAGLAYIM_TANNIN)
	    return atkbp_or((struct atkbp_set[]){ ATKBP(ARM_DOMINANT), ATKBP(ARM_OFFHAND), ATKBP(NONE) });
	/* Baphomet's hug is preceded by his headbutt and his weapon -- the
	 * two-predecessor rule below would credit ARM_DOMINANT (from AT_WEAP)
	 * and HEAD (from AT_BUTT), but his weapon hand is occupied swinging
	 * that weapon, not free to grab -- it's his off-hand doing the hug.
	 */
	if (ptr->mtyp == PM_BAPHOMET)
	    return ATKBP(ARM_OFFHAND);
	/* shayateen's hug is preceded by its two weapon arms, but its AD_VORP
	 * (decapitating) damage type says this isn't an arm-grab at all -- a
	 * second, devouring maw in its torso, distinct from its head's MOUTH.
	 */
	if (ptr->mtyp == PM_SHAYATEEN)
	    return ATKBP(TORSO_MAW);
	/* mothering mass's hug (AD_SUCK) is the same suction-hug mechanic as
	 * shoggoth/priest of Ghaunadaur's -- called out explicitly since its
	 * preceding attacks are a tentacle/touch mix the two-predecessor
	 * rule would otherwise turn into a needlessly specific-looking blend.
	 */
	if (ptr->mtyp == PM_MOTHERING_MASS)
	    return ATKBP(LIMB_GENERIC);
	/* Moon-entity manipalp's hug follows its bite and touch, but it's the
	 * latching mouth doing the work, not the touch that happens to
	 * precede it.
	 */
	if (ptr->mtyp == PM_MOON_ENTITY_MANIPALP)
	    return ATKBP(MOUTH);
	/* devil's snare only has one attack (AT_VINE) before its hug, one
	 * short of the two-predecessor rule below -- but the vine is
	 * obviously what's doing the hugging, so it's called out explicitly
	 * rather than falling through to the nohands()/humanoid() default.
	 */
	if (ptr->mtyp == PM_DEVIL_S_SNARE)
	    return ATKBP(VINE);
	/* every other slithy(ptr) monster's hug is a boa-constrictor-style
	 * tail coil regardless of its other attacks -- even when arms/claws
	 * precede it (an ancient naga wields two weapons; a kraken has claws
	 * but is a sea serpent, not a squid).
	 */
	if (slithy(ptr))
	    return ATKBP(TAIL);

	{
	    int i = attack_index(ptr, attk);

	    if (i >= 2) {
		struct atkbp_set inherited = atkbp_or((struct atkbp_set[]){
		    attk_bodyparts(ptr, &ptr->mattk[i - 1]),
		    attk_bodyparts(ptr, &ptr->mattk[i - 2]),
		    ATKBP(NONE) });

		if (!atkbp_is_none(inherited))
		    return inherited;
	    }
	}
	return (!nohands(ptr) && humanoid(ptr)) ? ATKBP(ARM) : ATKBP(LIMB_GENERIC);
    }
    case AT_TUCH:
	/* tettigon legatus's touch is a directed psychic effect (AD_PSH3),
	 * not a limb at all, despite MB_HUMANOID.
	 */
	if (ptr->mtyp == PM_TETTIGON_LEGATUS)
	    return ATKBP(MIND_NOLIMB);
	/* itinerant priestess's holy touch is a genuine third arm, not a
	 * vague/unordinaled one -- her AT_WEAP/AT_XWEP already claim
	 * dominant/offhand.
	 */
	if (ptr->mtyp == PM_ITINERANT_PRIESTESS)
	    return ATKBP(ARM_3RD);
	/* an offhand touch immediately preceded by an already-confirmed arm
	 * (e.g. mercurial essence's polywep claw, despite not being
	 * MB_HUMANOID) pairs with that arm regardless of body shape -- a
	 * straitjacket blocking one hand of a pair but not the other would
	 * be incoherent.
	 */
	if (attk->offhand) {
	    int i = attack_index(ptr, attk);

	    if (i > 0 && atkbp_intersects(attk_bodyparts(ptr, &ptr->mattk[i - 1]),
					   ATKBP_UPPER_BODY_ARM_MASK()))
		return ATKBP(ARM_OFFHAND);
	}
	/* not humanoid_torso -- an unconfirmed limb regardless of offhand;
	 * Voice-In-Screams's offhand middle AT_TUCH shouldn't disagree with
	 * its two LIMB_GENERIC siblings just because of the flag.
	 */
	if (!humanoid_torso(ptr))
	    return ATKBP(LIMB_GENERIC);
	/* humanoid_torso and offhand-flagged -- the offhand arm specifically
	 * (same pattern AT_CLAW's blanket offhand check applies, but decided
	 * here instead since AT_TUCH needs humanoid_torso(ptr) checked
	 * first).
	 */
	if (attk->offhand)
	    return ATKBP(ARM_OFFHAND);
	/* humanoid_torso, not itself offhand, but immediately followed by an
	 * offhand attack -- e.g. unmasked tettigon's leading AT_TUCH, paired
	 * with its own OFFHND_ATTK(AT_TUCH,...) right after it.
	 */
	if (immediately_followed_by_offhand_attack(ptr, attk))
	    return ATKBP(ARM_DOMINANT);
	/* humanoid_torso but no hands at all -- same fallback as AT_CLAW's
	 * and AT_HUGS's final catch-alls.
	 */
	return nohands(ptr) ? ATKBP(LIMB_GENERIC) : ATKBP(ARM);
    /* AT_LRCH is its own case, separate from AT_TUCH -- almost every user
     * needs a PM_*-scoped answer, so folding it in would only muddy that
     * case. Default for anything not called out below is LIMB_GENERIC.
     */
    case AT_LRCH:
	/* hound of Tindalos' reach is its own blood-drinking tongue (its
	 * AT_MAGC uses the same organ, see the preamble above). index wolf's
	 * reach is an abnormally long, were-transmitting tongue.
	 */
	if (ptr->mtyp == PM_HOUND_OF_TINDALOS || ptr->mtyp == PM_INDEX_WOLF)
	    return ATKBP(TONGUE);
	/* warrior changed's two reaches are the same alien blade-tail, held
	 * and swung twice by its one dominant hand -- not two separate
	 * limbs (same "one limb, two hits" pattern as rage-walker's AT_CLAW
	 * pair above).
	 */
	if (ptr->mtyp == PM_WARRIOR_CHANGED)
	    return ATKBP(ARM_DOMINANT);
	/* shalosh tannah's three reaches are three distinct flesh-hook
	 * arms.
	 */
	if (ptr->mtyp == PM_SHALOSH_TANNAH) {
	    switch (attack_index(ptr, attk)) {
	    case 0: return ATKBP(ARM_DOMINANT);
	    case 1: return ATKBP(ARM_OFFHAND);
	    case 2: return ATKBP(ARM_3RD);
	    }
	}
	/* black flower's two reaches are its primary body's dominant/offhand
	 * arms (its AT_5SQR/AT_TUCH attacks are a separate, unaudited
	 * question -- out of scope here).
	 */
	if (ptr->mtyp == PM_BLACK_FLOWER) {
	    switch (attack_index(ptr, attk)) {
	    case 1: return ATKBP(ARM_DOMINANT);
	    case 2: return ATKBP(ARM_OFFHAND);
	    }
	}
	/* Moon's Chosen's two reaches are simply its left and right arm,
	 * despite the differing damage types.
	 */
	if (ptr->mtyp == PM_MOON_S_CHOSEN) {
	    switch (attack_index(ptr, attk)) {
	    case 0: return ATKBP(ARM_DOMINANT);
	    case 1: return ATKBP(ARM_OFFHAND);
	    }
	}
	/* Axus's reach is a plain, unconfirmed-ordinal arm. */
	if (ptr->mtyp == PM_AXUS)
	    return ATKBP(ARM);
	/* twitching four-armed changed's two reaches are alien blade-tail
	 * swords held in a 5th/6th arm pair -- its AT_MSPR pair already
	 * separately claims ARM_3RD/ARM_4TH via the generator's extra-arm
	 * counter (util/attkbpdefs.c), so these get the next pair up rather
	 * than colliding with it.
	 */
	if (ptr->mtyp == PM_TWITCHING_FOUR_ARMED_CHANGED) {
	    switch (attack_index(ptr, attk)) {
	    case 0: return ATKBP(ARM_5TH);
	    case 1: return ATKBP(ARM_6TH);
	    }
	}
	/* uvuudaum's reach is its one prominent headspike tentacle; alabaster
	 * cactoid's is its one reaching tentacle despite MB_NOHANDS -- both
	 * are TENTACLE_ARM_DOMINANT (see atkbpnames.c), not the many-tentacle
	 * TENTACLE_GENERIC.
	 */
	if (ptr->mtyp == PM_UVUUDAUM || ptr->mtyp == PM_ALABASTER_CACTOID)
	    return ATKBP(TENTACLE_ARM_DOMINANT);
	/* parasitized embraced alider/commander's two reaches are the
	 * parasite's own pair of prominent primary tentacles.
	 */
	if (ptr->mtyp == PM_PARASITIZED_EMBRACED_ALIDER
		|| ptr->mtyp == PM_PARASITIZED_COMMANDER) {
	    switch (attack_index(ptr, attk)) {
	    case 0: return ATKBP(TENTACLE_ARM_DOMINANT);
	    case 1: return ATKBP(TENTACLE_ARM_OFFHAND);
	    }
	}
	/* charybdisone's reach is one of its own tentacles, just
	 * longer-ranged than usual -- an ordinary many-tentacled body plan,
	 * unlike uvuudaum/alabaster cactoid above.
	 */
	if (ptr->mtyp == PM_CHARYBDISONE)
	    return ATKBP(TENTACLE_GENERIC);
	/* Daruth Xaxox and drow alienist sprout long, spider-like aberrant
	 * legs -- not part of their normal (humanoid, two-leg) leg count,
	 * and not true arms either.
	 */
	if (ptr->mtyp == PM_DARUTH_XAXOX) {
	    switch (attack_index(ptr, attk)) {
	    case 2: return ATKBP(ALIEN_LIMB_1ST);
	    case 3: return ATKBP(ALIEN_LIMB_2ND);
	    }
	}
	if (ptr->mtyp == PM_DROW_ALIENIST)
	    return ATKBP(ALIEN_LIMB_1ST);
	/* dread seraph's two reaches are wing-borne. */
	if (ptr->mtyp == PM_DREAD_SERAPH)
	    return ATKBP(WING);
	return ATKBP(LIMB_GENERIC);
    case AT_TENT:
	/* displacer beast's two tentacles are its real manipulatory limbs
	 * (its claws are legs -- see the nogloves(ptr) entry above) -- each
	 * one is individually prominent, not a main-mass-plus-pair like the
	 * mind flayer family below.
	 */
	if (ptr->mtyp == PM_DISPLACER_BEAST) {
	    switch (attack_index(ptr, attk)) {
	    case 0: return ATKBP(TENTACLE_ARM_DOMINANT);
	    case 1: return ATKBP(TENTACLE_ARM_OFFHAND);
	    }
	}
	/* master mind flayer, parasitic master mind flayer, Lugribossk, and
	 * alhoon each have a 1d4 tentacle (the main mass of generic
	 * tentacles -- falls through to the is_mind_flayer() default below,
	 * TENTACLE_GENERIC|MOUTH) plus two 1d2 tentacles, each one of the
	 * major/prominent tentacles.
	 */
	if (ptr->mtyp == PM_MASTER_MIND_FLAYER) {
	    switch (attack_index(ptr, attk)) {
	    case 3: return ATKBP(TENTACLE_ARM_DOMINANT);
	    case 4: return ATKBP(TENTACLE_ARM_OFFHAND);
	    }
	}
	if (ptr->mtyp == PM_PARASITIC_MASTER_MIND_FLAYER) {
	    switch (attack_index(ptr, attk)) {
	    case 1: return ATKBP(TENTACLE_ARM_DOMINANT);
	    case 2: return ATKBP(TENTACLE_ARM_OFFHAND);
	    }
	}
	if (ptr->mtyp == PM_LUGRIBOSSK || ptr->mtyp == PM_ALHOON) {
	    switch (attack_index(ptr, attk)) {
	    case 2: return ATKBP(TENTACLE_ARM_DOMINANT);
	    case 3: return ATKBP(TENTACLE_ARM_OFFHAND);
	    }
	}
	return is_mind_flayer(ptr)
	       ? atkbp_or((struct atkbp_set[]){ ATKBP(TENTACLE_GENERIC), ATKBP(MOUTH), ATKBP(NONE) })
	       : ATKBP(TENTACLE_GENERIC);
    case AT_MAGC:
    case AT_MMGC:
	/* "blessed" (PM_BLESSED) casts through her horned light, not an arm. */
	if (ptr->mtyp == PM_BLESSED)
	    return ATKBP(HORNED_LIGHT);
	/* AD_PSON is purely mental regardless of aatyp; nohands(ptr) can't
	 * gesture to cast either -- both collapse to limbless. Otherwise
	 * this is category-only (like AT_MARI/AT_MSPR below); the generator
	 * resolves it to the monster's concrete arms via
	 * atkbp_spellcast_arm_mask().
	 */
	return (attk->adtyp == AD_PSON || nohands(ptr))
	       ? ATKBP(MIND_NOLIMB) : ATKBP(ARM);
    case AT_HITS:
	/* AT_HITS is a generic "auto-hit" mechanism reused for many
	 * unrelated, non-limb effects, so it defaults to NONE. The
	 * aphanactonan audient/assessor are the one case where it's
	 * genuinely an arm.
	 */
	if (ptr->mtyp == PM_APHANACTONAN_ASSESSOR)
	    return ATKBP(ARM_DOMINANT);
	if (ptr->mtyp == PM_APHANACTONAN_AUDIENT)
	    return ATKBP(ARM);
	return ATKBP(NONE);
    case AT_NONE:
	/* AT_NONE is normally limbless (e.g. an acid blob's corrosive
	 * touch), but some AD_PLYS passives are actually delivered via gaze
	 * -- see paralysis_is_gaze() (mondata.h), shared with xhity.c's
	 * AD_PLYS passive-retaliation code so the two can't drift apart.
	 */
	if (attk->adtyp == AD_PLYS && paralysis_is_gaze(ptr))
	    return ATKBP(EYES);
	return ATKBP(NONE);
    case AT_BITE:
    case AT_LNCK:
    case AT_5SBT:
	return ATKBP(MOUTH);
    case AT_BUTT:
	/* juggernaut/id juggernaut: a giant headless rolling ram, not a
	 * literal head-butt at all.
	 */
	if (!has_head(ptr))
	    return ATKBP(WHOLE_BODY);
	if (!has_horns(ptr))
	    return ATKBP(HEAD);
	/* a single-horned monster (unicorns, Nightmare, ki-rin, titanothere,
	 * ancient of corruption) has no ambiguity to be vague about -- its
	 * AT_BUTT is unquestionably that one horn, even though it's only a
	 * single attack.
	 */
	if (mon_horn_count(ptr) == 1)
	    return ATKBP(HORN_1ST);
	/* musimon gores with a pair of its four horns at once, not one horn
	 * per attack -- its first AT_BUTT is horns 1+2, its second is horns
	 * 3+4, unlike the 1-attack-per-horn mapping every other multi-horn
	 * butter below uses.
	 */
	if (ptr->mtyp == PM_MUSIMON) {
	    return (butt_ordinal(ptr, attk) == 0)
		   ? atkbp_or((struct atkbp_set[]){ ATKBP(HORN_1ST), ATKBP(HORN_2ND), ATKBP(NONE) })
		   : atkbp_or((struct atkbp_set[]){ ATKBP(HORN_3RD), ATKBP(HORN_4TH), ATKBP(NONE) });
	}
	/* a monster with 2+ AT_BUTT attacks has enough evidence to name
	 * which specific horn each one is -- triceratops's three horns map
	 * 1:1 onto its three AT_BUTT attacks. A single AT_BUTT attack stays
	 * the vague, unordinaled HORN bit.
	 */
	if (butt_count(ptr) >= 2) {
	    int b = butt_ordinal(ptr, attk) + 1;

	    if (b <= mon_horn_count(ptr))
		return horn_ordinal_bit(b);
	}
	return ATKBP(HORN);
    case AT_TONG:
	return ATKBP(TONGUE);
    case AT_VOMT:
    case AT_SPIT:
    case AT_BREA:
    case AT_BRSH:
	/* nomouth() monsters genuinely have no mouth -- teraphim tannah's
	 * AT_BRSH still comes out of its head; hellfire orb has neither
	 * mouth nor head, so it's whole-body. MB_NOHEAD alone does NOT
	 * imply no mouth -- most other MB_NOHEAD users of these types still
	 * have some kind of mouth and stay the plain default below.
	 */
	if (nomouth(ptr->mtyp))
	    return has_head(ptr) ? ATKBP(HEAD) : ATKBP(WHOLE_BODY);
	return ATKBP(MOUTH);
    case AT_GAZE:
	return ATKBP(EYES);
    case AT_WDGZ:
	/* "blessed" (PM_BLESSED) has her own unique radiant body part
	 * instead of a plain halo -- her light comes from/through her horns
	 * (MB_HORNS), not a generic radiance. */
	if (ptr->mtyp == PM_BLESSED)
	    return ATKBP(HORNED_LIGHT);
	/* a blinding radiance is a halo, not literally an eye-gaze. */
	return attk->adtyp == AD_BLND ? ATKBP(HALO) : ATKBP(EYES);
    case AT_KICK:
	/* "blessed" kicks with legs that don't fit the humanoid-feet/
	 * animal-bodied dichotomy below -- MB_NOFEET, no discrete feet to
	 * count. INNUMERABLE (same treatment as the masked queen's AT_DSPR
	 * arms) keeps the vague LEG bit from being dropped by
	 * mon_bodypart_bits() (util/attkbpdefs.c).
	 */
	if (ptr->mtyp == PM_BLESSED)
	    return atkbp_or((struct atkbp_set[]){ ATKBP(LEG), ATKBP(INNUMERABLE), ATKBP(NONE) });
	/* animal_bodied(ptr) monsters kick with a hind leg -- narrower than
	 * the fully generic LEG a non-animal kicker (e.g. a monk) gets. 2+
	 * AT_KICK attacks split the rear legs round-robin (a quadruped's
	 * two kicks are legs 3 and 4; a demonic black widow's are 5+7 and
	 * 6+8); a single AT_KICK stays the vague LEG_REAR.
	 */
	if (!animal_bodied(ptr))
	    return ATKBP(LEG);
	if (kick_count(ptr) >= 2) {
	    int nkicks = kick_count(ptr);
	    int total = mon_leg_count(ptr);
	    int rear = mon_rear_leg_count(ptr);
	    int rear_start = total - rear + 1;
	    int k = kick_ordinal(ptr, attk);
	    struct atkbp_set legs[9];
	    int n = 0, leg;

	    for (leg = rear_start + k; leg <= total; leg += nkicks)
		legs[n++] = leg_ordinal_bit(leg);
	    legs[n] = ATKBP(NONE);
	    return atkbp_or(legs);
	}
	return ATKBP(LEG_REAR);
    case AT_TAIL:
	return ATKBP(TAIL);
    case AT_STNG:
	/* xan and pisaca's stings are a piercing proboscis, not a tail or an
	 * insect-abdomen stinger.
	 */
	if (ptr->mtyp == PM_XAN || ptr->mtyp == PM_PISACA)
	    return ATKBP(PROBOSCIS);
	/* vermiurge is MA_INSECTOID but has a literal scorpion-like tail,
	 * unlike the rest of its insectoid kin below.
	 */
	if (ptr->mtyp == PM_VERMIURGE)
	    return ATKBP(TAIL);
	/* Shelob's "sting" is more likely an imprecise description of a bite
	 * (real spiders don't have tail-stingers), but dNAO deliberately
	 * keeps the insect-stinger reading anyway -- it makes her more alien.
	 */
	if (ptr->mtyp == PM_SHELOB)
	    return ATKBP(STINGER);
	/* lilitu's six stings are her six individually-prominent
	 * back-tentacles -- the reason ATKBP_TENTACLE_ARM_* goes up to 6TH.
	 * attack_index() runs 2..7 for her six AT_STNG attacks, so -1 lines
	 * that up with tentacle_arm_ordinal_bit()'s 1-based range.
	 * WORN_LIKE_WING marks that they're positioned/armored like wings
	 * despite their real TENTACLE_ARM_* family.
	 */
	if (ptr->mtyp == PM_LILITU)
	    return atkbp_or((struct atkbp_set[]){
		tentacle_arm_ordinal_bit(attack_index(ptr, attk) - 1),
		ATKBP(WORN_LIKE_WING), ATKBP(NONE) });
	/* Obox-ob's stings are a piercing proboscis, same reading as
	 * xan/pisaca above.
	 */
	if (ptr->mtyp == PM_OBOX_OB)
	    return ATKBP(PROBOSCIS);
	/* sartan tannin's three stings read better as three
	 * individually-prominent facial tentacles than sharing one
	 * PROBOSCIS. Its AT_REND (3 predecessors, not the usual 2) already
	 * unions all three, so that stays correct automatically.
	 */
	if (ptr->mtyp == PM_SARTAN_TANNIN)
	    return tentacle_arm_ordinal_bit(attack_index(ptr, attk) + 1);
	/* akkabish tannin's six stings are six individually-prominent
	 * ovipositor tentacles, not a literal tail, insect stinger, or an
	 * unspecified mass -- same reasoning as lilitu's six back-tentacles
	 * above.
	 */
	if (ptr->mtyp == PM_AKKABISH_TANNIN)
	    return tentacle_arm_ordinal_bit(attack_index(ptr, attk) + 1);
	/* AD_PAIN marks the jellyfish-style diffuse stinging-tentacle sting
	 * -- no tail, no abdomen, just a stinging surface.
	 */
	if (attk->adtyp == AD_PAIN)
	    return ATKBP(TENTACLE_GENERIC);
	/* MA_INSECTOID (bee/wasp/hornet/formian) stings with an abdomen tip,
	 * not a tail; everyone else (scorpions, devils, ...) defaults to the
	 * classic jointed tail-stinger.
	 */
	return is_insectoid(ptr) ? ATKBP(STINGER) : ATKBP(TAIL);
    case AT_WING:
	return ATKBP(WING);
    case AT_EXPL:
    case AT_BOOM:
    case AT_ENGL:
    case AT_ILUR:
	return ATKBP(WHOLE_BODY);
    case AT_VINE:
	return ATKBP(VINE);
    case AT_OBIT:
    case AT_WBIT:
	/* Medusa's single AT_OBIT stands in for her whole head of live
	 * snakes, each capable of biting -- innumerable other-heads, not
	 * just one.
	 */
	if (ptr->mtyp == PM_MEDUSA)
	    return atkbp_or((struct atkbp_set[]){ ATKBP(OTHER_APPENDAGE), ATKBP(INNUMERABLE), ATKBP(NONE) });
	return ATKBP(OTHER_APPENDAGE);
    case AT_WHIP:
    case AT_HODS:
    case AT_JUGL:
    case AT_DEVA:
    case AT_5SQR:
	/* rhombohedroid's AT_HODS is a directed mental effect, not a
	 * physical limb at all -- distinct from cuboid's literal
	 * whip-chains, which fall through to the general nohands(ptr)
	 * fallback below.
	 */
	if (ptr->mtyp == PM_RHOMBOHEDROID && attk->aatyp == AT_HODS)
	    return ATKBP(MIND_NOLIMB);
	/* Moon-entity tongue's AT_5SQR (AD_LICK) is, per its name, its
	 * tongue.
	 */
	if (ptr->mtyp == PM_MOON_ENTITY_TONGUE && attk->aatyp == AT_5SQR)
	    return ATKBP(TONGUE);
	/* AT_5SQR/AD_SHDW is a shadow-blade strike (M_GREAT_WEB's template
	 * insertion uses this combination) -- a directed shadow effect, not
	 * a physical limb. General rule keyed on (aatyp, adtyp), not
	 * per-monster, matching AT_WDGZ/AD_BLND's HALO split above.
	 */
	if (attk->aatyp == AT_5SQR && attk->adtyp == AD_SHDW)
	    return ATKBP(MIND_NOLIMB);
	/* Dagon and khaamnun tannin's AT_5SQR/AD_PULL attacks are each one
	 * major tentacle -- khaamnun tannin has five, Dagon two. Ordinal is
	 * this attack's position among just the AD_PULL AT_5SQR attacks,
	 * not its raw mattk[] index -- Dagon's pair isn't at index 0/1.
	 */
	if (attk->aatyp == AT_5SQR && attk->adtyp == AD_PULL
		&& (ptr->mtyp == PM_DAGON || ptr->mtyp == PM_KHAAMNUN_TANNIN)) {
	    int i, ord = 0;

	    for (i = 0; i < NATTK; i++) {
		struct attack *cur = &ptr->mattk[i];

		if (cur->aatyp != AT_5SQR || cur->adtyp != AD_PULL)
		    continue;
		ord++;
		if (cur == attk)
		    return tentacle_arm_ordinal_bit(ord);
	    }
	}
	/* same category-only contract as AT_CLAW/AT_TUCH -- a nohands(ptr)
	 * monster (e.g. cuboid's whip-like chains) has nothing confirmed to
	 * credit this to but an unconfirmed limb.
	 */
	return nohands(ptr) ? ATKBP(LIMB_GENERIC) : ATKBP(ARM);
    case AT_ARRW:
	/* manticore's AT_ARRW is its mythological tail-spike volley. */
	if (ptr->mtyp == PM_MANTICORE)
	    return ATKBP(TAIL);
	/* daughter of bedlam is a horror monster -- her AT_ARRW is a dread
	 * gaze effect (AD_PLYS), not a literal projectile.
	 */
	if (ptr->mtyp == PM_DAUGHTER_OF_BEDLAM)
	    return ATKBP(EYES);
	/* edderkop's mainhand wields her AT_SRPR rapier (ARM_DOMINANT below);
	 * her AT_ARRW is a shadow-magic effect (AD_SHDW) worked with the
	 * off-hand.
	 */
	if (ptr->mtyp == PM_EDDERKOP)
	    return ATKBP(ARM_OFFHAND);
	/* Center of All's three AT_WEAP entries are all the same dominant
	 * hand (ARM_DOMINANT below, same pattern as silverknight) -- he's
	 * correctly only 2 arms total, and his AT_ARRW is a rock thrown with
	 * the off-hand.
	 */
	if (ptr->mtyp == PM_CENTER_OF_ALL)
	    return ATKBP(ARM_OFFHAND);
	/* alider already has a confirmed dominant/offhand weapon pair plus
	 * two AT_MSPR extra arms -- which of those (if any) also draws the
	 * bow isn't known, so this stays the vague/unordinaled category bit
	 * rather than claiming a 5th/6th arm or reusing an already-claimed
	 * one.
	 */
	if (ptr->mtyp == PM_ALIDER)
	    return ATKBP(ARM);
	/* dao lao gui monk spits poisoned darts (the common modern reading of
	 * the original source, which never actually describes the
	 * projectile or how it's launched).
	 */
	if (ptr->mtyp == PM_DAO_LAO_GUI_MONK)
	    return ATKBP(MOUTH);
	/* siege ogre's arrows are a siege engine it operates, not drawn by
	 * its own (already weapon-wielding) hands; ancient nupperibo's
	 * launching mechanism isn't confirmed either (it's mounted on its
	 * back, nowhere near its tentacles).
	 */
	if (ptr->mtyp == PM_SIEGE_OGRE || ptr->mtyp == PM_ANCIENT_NUPPERIBO)
	    return ATKBP(LAUNCHER_GENERIC);
	/* argentum golem and Arsenal are constructs whose AT_ARRW is a
	 * launcher built into the body itself, not any kind of limb.
	 */
	if (ptr->mtyp == PM_ARGENTUM_GOLEM || ptr->mtyp == PM_ARSENAL)
	    return ATKBP(MECHANISM_GENERIC);
	/* AT_ARRW covers many unrelated ranged attacks beyond the PM_*-scoped
	 * cases above -- a monster with arms defaults to a two-handed-bow
	 * assumption; one without gets the generic/unconfirmed placeholder.
	 */
	return nohands(ptr)
	       ? ATKBP(LAUNCHER_GENERIC)
	       : atkbp_or((struct atkbp_set[]){ ATKBP(ARM_DOMINANT), ATKBP(ARM_OFFHAND), ATKBP(NONE) });
    case AT_TNKR:
	/* clockwork factory's AT_TNKR is a launcher built into the body
	 * itself, not any kind of limb -- same reasoning as argentum
	 * golem/Arsenal's AT_ARRW above.
	 */
	if (ptr->mtyp == PM_CLOCKWORK_FACTORY)
	    return ATKBP(MECHANISM_GENERIC);
	/* "tinkering" implies hands-on manipulation of tools/gadgets rather
	 * than a discrete launched projectile, so -- unlike AT_ARRW above --
	 * the generic/unconfirmed fallback here is an unconfirmed limb, not
	 * an unconfirmed launcher.
	 */
	return nohands(ptr)
	       ? ATKBP(LIMB_GENERIC)
	       : atkbp_or((struct atkbp_set[]){ ATKBP(ARM_DOMINANT), ATKBP(ARM_OFFHAND), ATKBP(NONE) });
    case AT_DSPR:
	/* only the masked queen has AT_DSPR today -- her lower arms are
	 * innumerable, not a clean dominant/offhand pair, so this stays the
	 * generic/low-precision lower-arm attribution.
	 */
	return atkbp_or((struct atkbp_set[]){ ATKBP(ARM_LOWER), ATKBP(INNUMERABLE), ATKBP(NONE) });
    case AT_ESPR:
	return ATKBP(MIND_NOLIMB);
    case AT_REND: {
	/* AT_REND requires the two attacks immediately preceding it to both
	 * hit this turn -- sartan tannin needs three (it has three AT_STNG
	 * attacks before its rend). A plain union of predecessor bits, no
	 * nohands()/humanoid() fallback needed.
	 */
	int i = attack_index(ptr, attk);
	int predecessors = (ptr->mtyp == PM_SARTAN_TANNIN) ? 3 : 2;
	struct atkbp_set parts[4];
	int n = 0, k;

	for (k = 1; k <= predecessors && i - k >= 0; k++)
	    parts[n++] = attk_bodyparts(ptr, &ptr->mattk[i - k]);
	parts[n] = ATKBP(NONE);
	return atkbp_or(parts);
    }
    case AT_BKGT:
	/* a swirling mass of countless eyes, tentacles, mouths, legs, and
	 * horns -- the storm elementals and Mouth of the Goat this
	 * represents don't have one discrete instance of any of these,
	 * they're covered in innumerably many.
	 * Note for later review: mon_flag_bodyparts()'s flag-based
	 * has_horns(ptr) check won't independently confirm these monsters
	 * have horns (none of them set MB_HORNS) -- they aren't intended to
	 * have horns *on a head* (none of them has a head at all per
	 * MB_NOHEAD), so this is probably fine as-is, but worth a second
	 * look if that ever matters.
	 */
	return atkbp_or((struct atkbp_set[]){
	    ATKBP(EYES), ATKBP(TENTACLE_GENERIC), ATKBP(MOUTH), ATKBP(LEG), ATKBP(HORN),
	    ATKBP(INNUMERABLE), ATKBP(NONE) });
    case AT_BKG2:
	/* only "blessed" (PM_BLESSED) uses this today -- her horned light,
	 * same body part as her AT_WDGZ/AT_MAGC attacks.
	 */
	return ATKBP(HORNED_LIGHT);
    case AT_MARI:
	/* gynoid/parasitized gynoid's 4th "weapon arm" is a nanomachine
	 * telekinetic effect, not a literal extra limb like a real
	 * marilith's arms. */
	if (ptr->mtyp == PM_GYNOID || ptr->mtyp == PM_PARASITIZED_GYNOID)
	    return ATKBP(MIND_NOLIMB);
	return ATKBP(ARM);
    case AT_MSPR:
	return ATKBP(ARM);
    case AT_BEAM:
	/* Keto's beam is a jetting siphon organ, not a limb. */
	if (ptr->mtyp == PM_KETO)
	    return ATKBP(SIPHON);
	/* binah sephirah's beam is fired from (one of) her arms. */
	if (ptr->mtyp == PM_BINAH_SEPHIRAH)
	    return ATKBP(ARM);
	/* moon flea's beam is expelled from its mouth. */
	if (ptr->mtyp == PM_MOON_FLEA)
	    return ATKBP(MOUTH);
	/* ancient tempest (MB_NOHEAD|MB_NOLIMBS|MB_NOEYES -- a bodiless
	 * elemental), cuboid, and rhombohedroid (abstract S_KETER constructs)
	 * have nothing in their data to attribute this to.
	 */
	return ATKBP(NONE);
    default:
	return ATKBP(NONE);
    }
}

/* AT_SQUZ has no limb of its own -- it only ever fires as a follow-up once
 * the target is already grappled, so it inherits the body-part bits of
 * whichever attack in this monster's mattk[] established that grapple (the
 * one with adtyp==AD_WRAP). Every AT_SQUZ user in src/monst.c has exactly
 * one such sibling (verified by hand).
 */
static struct atkbp_set
squz_inherited_bits(struct permonst *ptr)
{
    int j;

    for (j = 0; j < NATTK; j++) {
	if (ptr->mattk[j].adtyp == AD_WRAP)
	    return attk_bodyparts(ptr, &ptr->mattk[j]);
    }
    return ATKBP(NONE);	/* should not happen -- see comment above */
}

static boolean
atkbp_equal(struct atkbp_set a, struct atkbp_set b)
{
    int i;

    for (i = 0; i < ATKBP_NWORDS; i++)
	if (a.w[i] != b.w[i])
	    return FALSE;
    return TRUE;
}

/* spellcast_arm_bits(): the union of every concrete arm this monster's
 * other attacks establish -- what an AT_MAGC/AT_MMGC attack should be
 * attributed to instead of the bare "casts via arm" placeholder, so a
 * future grapple/injury system can block spellcasting per-arm rather
 * than only once every arm is impaired. ARM_DOMINANT/ARM_OFFHAND are
 * always included unconditionally, since attk_bodyparts() only routes
 * here once !nohands(ptr) is confirmed -- e.g. silverknight's two plain
 * AT_WEAP attacks both resolve to ARM_DOMINANT, but it still gets
 * ARM_OFFHAND here.
 */
static struct atkbp_set
spellcast_arm_bits(struct permonst *ptr, const struct atkbp_set *resolved, int nattk)
{
    struct atkbp_set mask = atkbp_spellcast_arm_mask(ptr);
    boolean allow_lower = atkbp_intersects(mask, ATKBP_LOWER_ARM_MASK());
    /* +3: the unconditional base pair, every qualifying sibling entry, plus
     * the ATKBP(NONE) terminator atkbp_or() requires -- never put a
     * possibly-NONE accumulator into this list, it would be misread as that
     * terminator and stop the scan early.
     */
    struct atkbp_set qualifying[NATTK + 3];
    int i, n = 0;

    qualifying[n++] = ATKBP(ARM_DOMINANT);
    qualifying[n++] = ATKBP(ARM_OFFHAND);
    for (i = 0; i < nattk; i++) {
	if (atkbp_intersects(resolved[i], mask)
		|| (allow_lower
		    && atkbp_intersects(resolved[i], ATKBP(ARM_LOWER))
		    && atkbp_intersects(resolved[i], ATKBP(INNUMERABLE))))
	    qualifying[n++] = resolved[i];
    }
    qualifying[n] = ATKBP(NONE);
    return atkbp_or(qualifying);
}

/* how many extra ordinal arm identities (ARM_3RD..ARM_8TH) are available
 * for AT_MARI/AT_MSPR attacks beyond the first two -- arm_ordinal_bit()
 * (above) supplies the actual bits, correctly tracking each name's real
 * (word, bit) regardless of vocabulary growth.
 */
#define EXTRA_ARM_BITS_COUNT 6

/* attk_bodyparts_all(): resolves every real attack in ptr->mattk[] (stops
 * at the first null entry) to its final .bodypart value in one pass,
 * handling the three whole-attack-list-aware cases attk_bodyparts() alone
 * can't: AT_SQUZ inherits its AD_WRAP sibling's bits; AT_MARI/AT_MSPR
 * beyond a concrete per-monster override fall back to an ordinal
 * ARM_3RD.. identity by counting siblings; AT_MAGC/AT_MMGC get rewritten
 * to the union of the monster's other established arms. Writes
 * resolved[0..return value), the number of real attacks found (<= NATTK).
 *
 * Ordinal overflow falls back to the bare ARM category bit rather than
 * erroring -- this function must stay safe to call on live, possibly-
 * malformed game data, unlike the generator's build-time mons[] data,
 * which it validates separately via the optional `ordinal_overflow`
 * out-param (NULL-safe) instead of ever crashing here.
 */
int
attk_bodyparts_all(struct permonst *ptr, struct atkbp_set *resolved, boolean *ordinal_overflow)
{
    int i, nattk = 0, extra_arm_count = 0;
    struct atkbp_set spell_arms;

    if (ordinal_overflow)
	*ordinal_overflow = FALSE;

    for (i = 0; i < NATTK; i++) {
	struct attack *attk = &ptr->mattk[i];

	if (is_null_attk(attk))
	    break;

	if (attk->aatyp == AT_SQUZ) {
	    resolved[i] = squz_inherited_bits(ptr);
	} else if (attk->aatyp == AT_MARI || attk->aatyp == AT_MSPR) {
	    struct atkbp_set generic = attk_bodyparts(ptr, attk);

	    if (!atkbp_equal(generic, ATKBP(ARM))) {
		/* a monster-specific override (e.g. gynoid/parasitized
		 * gynoid's psi-held "arm") -- keep it as-is, not an ordinal
		 * extra-arm slot.
		 */
		resolved[i] = generic;
	    } else if (extra_arm_count >= EXTRA_ARM_BITS_COUNT) {
		resolved[i] = generic;	/* overflow -- best effort, no crash */
		if (ordinal_overflow)
		    *ordinal_overflow = TRUE;
	    } else {
		resolved[i] = arm_ordinal_bit(3 + extra_arm_count);
		extra_arm_count++;
	    }
	} else {
	    resolved[i] = attk_bodyparts(ptr, attk);
	}
	nattk++;
    }

    spell_arms = spellcast_arm_bits(ptr, resolved, nattk);
    for (i = 0; i < nattk; i++) {
	struct attack *attk = &ptr->mattk[i];

	if ((attk->aatyp == AT_MAGC || attk->aatyp == AT_MMGC)
		&& atkbp_equal(resolved[i], ATKBP(ARM)))
	    resolved[i] = spell_arms;
    }

    return nattk;
}

/* Blibdoolpoolp, graven-into-flesh's high-Insight alternate attack routine
 * (PM_BLIBDOOLPOOLP__GRAVEN_INTO_FLESH, src/xhity.c's getattk()) -- a
 * scaled-up copy of her own real mattk[], with one extra AT_REND spliced in
 * at index 2. Design numbers only (aatyp/adtyp/damn/damd) -- .bodypart is
 * deliberately not set here; src/xhity.c's getattk() derives each entry's
 * .bodypart from her real mattk[] at the corresponding BLIB_ALT_REAL_INDEX()
 * at the point of use. check_blib_alt_shape() (util/attkbpdefs.c) validates
 * this table's aatyp sequence against her real mattk[] at build time,
 * failing the build on divergence.
 */
const struct attack blib_alt_attacks[NATTK] = {
    { AT_CLAW, AD_SQUE, 4, 8, 0, 0, 1 },	/* polywep, matches her real dominant claw */
    { AT_CLAW, AD_SQUE, 4, 8, 0, 1, 0 },	/* offhand, matches her real offhand claw */
    { AT_REND, AD_SHRD, 3, 12 },
    { AT_HUGS, AD_PHYS, 3, 4 },
    { AT_HITS, AD_TSMI, 0, 0 },
    { AT_MAGC, AD_CLRC, 2, 8 },
    { AT_NONE, AD_MROT, 0, 0 }
};

/* Avatar of Lolth's alternate "marilith-hands" attack routine
 * (PM_AVATAR_OF_LOLTH, src/xhity.c's getattk()). Unlike blib_alt_attacks[]
 * above, this isn't a scaled copy of her own real mattk[] at matching
 * indices, so there's no real-attack shape to validate against -- it's
 * fresh, hand-attributed design (her own body, not an external mechanism
 * like Bael's animated-hands form).
 */
const struct attack lolth_alt_attacks[NATTK] = {
    { AT_WEAP, AD_PHYS, 4, 8 },
    { AT_XWEP, AD_PHYS, 4, 8 },
    { AT_MARI, AD_PHYS, 2, 8 },
    { AT_MARI, AD_PHYS, 2, 8 },
    { AT_MARI, AD_PHYS, 1, 8 },
    { AT_MARI, AD_PHYS, 1, 8 },
    { AT_MARI, AD_PHYS, 1, 8 },
    { AT_MARI, AD_PHYS, 1, 8 }
};

struct atkbp_set
lolth_alt_bodypart(int idx)
{
    return arm_ordinal_bit(idx + 1);
}

/* atkbp_spellcast_arm_mask(): which of a monster's own arm bits should
 * count as "one of the arms it casts spells with" -- default is every
 * concrete arm (see ATKBP_CONCRETE_ARM_MASK()), but some monsters cast
 * with only a subset of their arms.
 */

struct atkbp_set
atkbp_spellcast_arm_mask(struct permonst *ptr)
{
    /* Blibdoolpoolp's mindgraven champion casts only with her upper-body
     * arms -- her lower-body claws don't contribute to spellcasting 
     * concreteness even though she has them.
     */
    if (ptr->mtyp == PM_BLIBDOOLPOOLP_S_MINDGRAVEN_CHAMPION)
	return atkbp_or((struct atkbp_set[]){
	    ATKBP(ARM_DOMINANT), ATKBP(ARM_OFFHAND),
	    ATKBP(ARM_3RD), ATKBP(ARM_4TH), ATKBP(ARM_5TH), ATKBP(ARM_6TH), ATKBP(ARM_7TH), ATKBP(ARM_8TH),
	    ATKBP(NONE) });
    return ATKBP_CONCRETE_ARM_MASK();
}

/* mon_leg_count(): how many legs this monster is assumed to have --
 * guessed from body-plan flags, not tracked per-species. Order matters:
 * humanoid_feet(ptr) (bipedal) overrides everything below, so silverman
 * and Demogorgon (both MB_SLITHY but MB_HAS_FEET) get 2 rather than the
 * 0 a snake tail would otherwise imply. snakemanoid(ptr) is checked
 * instead of a raw MB_SLITHY test so a "legged snake" (MB_ANIMAL|
 * MB_SLITHY) still falls through to animal_bodied(ptr) below.
 * animal_bodied(ptr), not the stricter animaloid(), gates the
 * is_arachnid()/is_insectoid() leg-count bump, so a centauroid
 * arachnid/insectoid (e.g. a drider) still gets it.
 *
 * Obox-ob is canonically 108 legs, but nothing needs that precision, so
 * he isn't special-cased and gets the ordinary MB_ANIMAL default.
 */
int
mon_leg_count(struct permonst *ptr)
{
    if (humanoid_feet(ptr))
	return 2;
    if (nolimbs(ptr) || nofeet(ptr) || snakemanoid(ptr))
	return 0;
    if (animal_bodied(ptr)) {
	if (is_arachnid(ptr))
	    return 8;
	if (is_insectoid(ptr))
	    return 6;
	return 4;
    }
    if (naoid(ptr))
	return 0;
    return 2;
}

/* mon_rear_leg_count(): how many of this monster's legs (see
 * mon_leg_count() above) count as rear legs -- total legs divided by 4,
 * rounded down, times 2 (2 for the 4-leg default and for insectoids, 4 for
 * arachnids). The rear legs themselves are the highest-numbered ones --
 * legs (mon_leg_count(ptr) - mon_rear_leg_count(ptr) + 1) through
 * mon_leg_count(ptr), counting from LEG_DOMINANT as leg 1.
 */
int
mon_rear_leg_count(struct permonst *ptr)
{
    return (mon_leg_count(ptr) / 4) * 2;
}

int
mon_horn_count(struct permonst *ptr)
{
    switch (ptr->mtyp) {
    case PM_DRACAE_ELADRIN:
    case PM_FIERNA:
    case PM_GRAZ_ZT:
	return 6;
    case PM_MUSIMON:
	return 4;
    case PM_TRICERATOPS:
	return 3;
    case PM_TITANOTHERE:
    case PM_WHITE_UNICORN:
    case PM_GRAY_UNICORN:
    case PM_BLACK_UNICORN:
    case PM_NIGHTMARE:
    case PM_KI_RIN:
    case PM_ANCIENT_OF_CORRUPTION:
	return 1;
    }
    return 2;
}

/* mon_flag_bodyparts(): every body part this monster is known to have
 * from its flags alone, independent of any specific attack -- so a
 * monster with eyes but no AT_GAZE, or legs but no AT_KICK/AT_CLAW,
 * still shows up as having them. Deliberately conservative: only flags
 * that directly assert a body part's existence/absence are used here --
 * MB_ANIMAL/MB_HUMANOID aren't reliable signals on their own (already
 * folded into mon_leg_count()'s own policy).
 */
struct atkbp_set
mon_flag_bodyparts(struct permonst *ptr)
{
    struct atkbp_set parts[24];
    int n = 0;
    int nlegs, leg, nhorns, horn;

    if (haseyes(ptr))
	parts[n++] = ATKBP(EYES);
    if (has_head(ptr))
	parts[n++] = ATKBP(HEAD);
    if (has_wings(ptr))
	parts[n++] = ATKBP(WING);
    if (!nohands(ptr)) {
	parts[n++] = ATKBP(ARM_DOMINANT);
	/* the one confirmed exception to "no monster has exactly one hand"
	 * -- carcosan courtier has a single dominant arm and no offhand
	 * (see noshield(), mondata.h, for the same exception applied
	 * elsewhere).
	 */
	if (ptr->mtyp != PM_CARCOSAN_COURTIER)
	    parts[n++] = ATKBP(ARM_OFFHAND);
    }
    nlegs = mon_leg_count(ptr);
    for (leg = 1; leg <= nlegs; leg++)
	parts[n++] = leg_ordinal_bit(leg);
    /* a horned monster's horns are known body parts even if it has no
     * AT_BUTT attack at all (e.g. Fierna, Graz'zt) -- see mon_horn_count().
     */
    if (has_horns(ptr)) {
	nhorns = mon_horn_count(ptr);
	for (horn = 1; horn <= nhorns; horn++)
	    parts[n++] = horn_ordinal_bit(horn);
    }
    parts[n] = ATKBP(NONE);
    return atkbp_or(parts);
}

/*mattkbp.c*/
