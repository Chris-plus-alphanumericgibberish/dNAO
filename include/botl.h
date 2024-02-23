/* SPDX-License-Identifier: NGPL */

#ifndef BOTL_H
#define BOTL_H

#define BL_MASK_STONE    (0x1LL << 0)
#define BL_MASK_SLIME    (0x1LL << 1)
#define BL_MASK_SUFCT    (0x1LL << 2)
#define BL_MASK_ILL      (0x1LL << 3)
#define BL_MASK_FOODPOIS (0x1LL << 4)
#define BL_MASK_BLIND    (0x1LL << 5)
#define BL_MASK_STUN     (0x1LL << 6)
#define BL_MASK_CONF     (0x1LL << 7)
#define BL_MASK_HALLU    (0x1LL << 8)
#define BL_MASK_PANIC    (0x1LL << 9)
#define BL_MASK_STMBLNG  (0x1LL << 10)
#define BL_MASK_STGGRNG  (0x1LL << 11)
#define BL_MASK_BABBLE   (0x1LL << 12)
#define BL_MASK_SCREAM   (0x1LL << 13)
#define BL_MASK_FAINT    (0x1LL << 14)
#define BL_MASK_HELD     (0x1LL << 15)
#define BL_MASK_UHOLD    (0x1LL << 16)
#define BL_MASK_LYCN     (0x1LL << 17)
#define BL_MASK_INVL     (0x1LL << 18)
#define BL_MASK_LEV      (0x1LL << 19)
#define BL_MASK_FLY      (0x1LL << 20)
#define BL_MASK_RIDE     (0x1LL << 21)
#define BL_MASK_TIMESTOP (0x1LL << 22)
#define BL_MASK_LUST     (0x1LL << 23)
#define BL_MASK_DEADMAGC (0x1LL << 24)
#define BL_MASK_MISO     (0x1LL << 25)
#define BL_MASK_CATAPSI  (0x1LL << 26)
#define BL_MASK_DIMLOCK  (0x1LL << 27)

struct status_effect {
	long long mask;
	char *name;
	char *abbrev1;
	char *abbrev2;
};

static const struct status_effect status_effects[] = {
	/* Delayed instadeaths */
	{ BL_MASK_STONE,    "Stone",    "Ston",  "Sto" },
	{ BL_MASK_SLIME,    "Slime",    "Slim",  "Slm" },
	{ BL_MASK_SUFCT,    "Sufct",    "Sfct",  "Sfc" },
	{ BL_MASK_ILL,      "Ill",      "Ill",   "Ill" },
	{ BL_MASK_FOODPOIS, "FoodPois", "Fpois", "Poi" },
	/* Hunger and encumbrance should go here, but are special-cased */
	/* Other status effects */
	{ BL_MASK_BLIND,    "Blind",    "Blnd",  "Bl"  },
	{ BL_MASK_STUN,     "Stun",     "Stun",  "St"  },
	{ BL_MASK_CONF,     "Conf",     "Cnf",   "Cf"  },
	{ BL_MASK_HALLU,    "Hallu",    "Hal",   "Hl"  },
	/* Insanity messages */
	{ BL_MASK_PANIC,    "Panic",    "Pnc",   "Pnc" },
	{ BL_MASK_STMBLNG,  "Stmblng",  "Stmbl", "Stm" },
	{ BL_MASK_STGGRNG,  "Stggrng",  "Stggr", "Stg" },
	{ BL_MASK_BABBLE,   "Babble",   "Babl",  "Bbl" },
	{ BL_MASK_SCREAM,   "Scream",   "Scrm",  "Scr" },
	{ BL_MASK_FAINT,    "Faint",    "Fnt",   "Fnt" },
	/* Less important */
	{ BL_MASK_HELD,     "Held",     "Hld",   "Hd"  },
	{ BL_MASK_UHOLD,    "UHold",    "UHld",  "UHd" },
	{ BL_MASK_LYCN,     "Lycn",     "Lyc",   "Ly"  },
	{ BL_MASK_INVL,     "Invl",     "Invl",  "In"  },
	{ BL_MASK_LEV,      "Lev",      "Lev",   "Lv"  },
	{ BL_MASK_FLY,      "Fly",      "Fly",   "Fl"  },
	{ BL_MASK_RIDE,     "Ride",     "Rid",   "Rd"  },
	/* Temporary effects with known duration */
	{ BL_MASK_TIMESTOP, "TimeStop", "TStop", "TS"  },
	{ BL_MASK_LUST,     "Lust",     "Lust",  "Lst" },
	{ BL_MASK_DEADMAGC, "DeadMagc", "DMagc", "DM"  },
	{ BL_MASK_MISO,     "Miso",     "Miso",  "Mi"  },
	{ BL_MASK_CATAPSI,  "Catapsi",  "Ctpsi", "Cps" },
	{ BL_MASK_DIMLOCK,  "DimLock",  "DLock", "DL"  },
};

#endif
