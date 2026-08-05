/* NetHack may be freely redistributed.  See license for details. */

#ifndef ATKBPNAMES_H
#define ATKBPNAMES_H

/* atkbp_name_list[]: the ordered ATKBP_* name list -- the single source of
 * truth for the vocabulary. util/makedefs's do_atkbpbits() walks this to
 * generate include/atkbpbits.h (the ATKBP_WORD_ / ATKBP_BIT_ #defines);
 * util/attkbpdefs.c walks it again (via the same atkbp_assign_bits(), so
 * the two can't disagree) to build the symbolic name table it uses when
 * printing src/attkbp.c. Reordering entries here is purely cosmetic --
 * word/bit values are assigned by position, and attkbp[] is never
 * persisted across builds, so there's no compatibility reason to keep bit
 * numbers stable.
 *
 * `word`/`bit` start at 0 and are filled in by atkbp_assign_bits(); they
 * aren't meaningful until that's been called.
 */
struct atkbp_name_entry {
    const char *name;
    int word;
    long bit;
};

extern struct atkbp_name_entry atkbp_name_list[];
extern int atkbp_nwords;

extern void NDECL(atkbp_assign_bits);

/* Hand-maintained ceiling on how many `long` words struct atkbp_set needs
 * to hold every bit in atkbp_name_list[] above. This is deliberately NOT
 * generated -- it has to live in a plain header with no generated
 * dependencies, because include/permonst.h (needed to build
 * util/makedefs, which is what generates include/atkbpbits.h) uses it to
 * define struct attack's bodypart field, and a generated-header dependency
 * there would be circular (util/makedefs is built from src/monst.c, which
 * #includes permonst.h). util/makedefs -k (do_atkbpbits(),
 * util/makedefs.c) checks the real word count atkbp_name_list[] needs
 * against this constant and fails the build with an explicit message if
 * it's too small -- when that happens, just bump this number and rebuild.
 * No headroom is kept on purpose: a failure here means a developer just
 * added a body part, so they're already looking at the right place to fix
 * it.
 */
#define ATKBP_NWORDS 3

struct atkbp_set {
    long w[ATKBP_NWORDS];
};

#endif /* ATKBPNAMES_H */
