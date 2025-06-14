/* SPDX-License-Identifier: NGPL */

#ifndef BOTL_H
#define BOTL_H

#define BL_MASK_STONE    (0x1LL << 0)
#define BL_MASK_SLIME    (0x1LL << 1)
#define BL_MASK_SUFCT    (0x1LL << 2)
#define BL_MASK_ILL      (0x1LL << 3)
#define BL_MASK_FOODPOIS (0x1LL << 4)
#define BL_MASK_BLEED 	 (0x1LL << 5)
#define BL_MASK_BLIND    (BL_MASK_BLEED << 1)
#define BL_MASK_STUN     (BL_MASK_BLIND << 1)
#define BL_MASK_CONF     (BL_MASK_STUN << 1)
#define BL_MASK_HALLU    (BL_MASK_CONF << 1)
#define BL_MASK_PANIC    (BL_MASK_HALLU << 1)
#define BL_MASK_STMBLNG  (BL_MASK_PANIC << 1)
#define BL_MASK_STGGRNG  (BL_MASK_STMBLNG << 1)
#define BL_MASK_BABBLE   (BL_MASK_STGGRNG << 1)
#define BL_MASK_SCREAM   (BL_MASK_BABBLE << 1)
#define BL_MASK_FAINT    (BL_MASK_SCREAM << 1)
#define BL_MASK_HELD     (BL_MASK_FAINT << 1)
#define BL_MASK_UHOLD    (BL_MASK_HELD << 1)
#define BL_MASK_LYCN     (BL_MASK_UHOLD << 1)
#define BL_MASK_INVL     (BL_MASK_LYCN << 1)
#define BL_MASK_LEV      (BL_MASK_INVL << 1)
#define BL_MASK_FLY      (BL_MASK_LEV << 1)
#define BL_MASK_RIDE     (BL_MASK_FLY << 1)
#define BL_MASK_TIMESTOP (BL_MASK_RIDE << 1)
#define BL_MASK_LUST     (BL_MASK_TIMESTOP << 1)
#define BL_MASK_DEADMAGC (BL_MASK_LUST << 1)
#define BL_MASK_MISO     (BL_MASK_DEADMAGC << 1)
#define BL_MASK_CATAPSI  (BL_MASK_MISO << 1)
#define BL_MASK_DIMLOCK  (BL_MASK_CATAPSI << 1)
//Note:28

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
	{ BL_MASK_BLEED,    "Bleed",    "Bleed", "Bld" },
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
