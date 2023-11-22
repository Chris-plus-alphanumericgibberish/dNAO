/*	SCCS Id: @(#)obj.h	3.4	2002/01/07	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifdef TEXTCOLOR
# define COPY_OBJ_DESCR(o_dst,o_src) \
			o_dst.oc_descr_idx = o_src.oc_descr_idx,\
			o_dst.oc_color = o_src.oc_color
#else
# define COPY_OBJ_DESCR(o_dst,o_src) o_dst.oc_descr_idx = o_src.oc_descr_idx
#endif

#ifndef OBJ_H
#define OBJ_H

enum {
	OPROP_NONE = 0,
	OPROP_FIRE,
	OPROP_COLD,
	OPROP_WOOL,
	OPROP_ELEC,
	OPROP_ACID,
	OPROP_MAGC,
	OPROP_ANAR,
	OPROP_CONC,
	OPROP_AXIO,
	OPROP_HOLY,
	OPROP_UNHY,
	OPROP_REFL,
	OPROP_DISN,
	OPROP_BCRS,
	OPROP_HEAL,
	OPROP_LIFE,
	OPROP_GRES,
	OPROP_FLAYW,
	OPROP_LESSER_FLAYW,
	OPROP_PHSEW,
	OPROP_FIREW,
	OPROP_OONA_FIREW,
	OPROP_LESSER_FIREW,
	OPROP_COLDW,
	OPROP_OONA_COLDW,
	OPROP_LESSER_COLDW,
	OPROP_ELECW,
	OPROP_OONA_ELECW,
	OPROP_LESSER_ELECW,
	OPROP_ACIDW,
	OPROP_LESSER_ACIDW,
	OPROP_MAGCW,
	OPROP_LESSER_MAGCW,
	OPROP_ANARW,
	OPROP_LESSER_ANARW,
	OPROP_CONCW,
	OPROP_LESSER_CONCW,
	OPROP_AXIOW,
	OPROP_LESSER_AXIOW,
	OPROP_HOLYW,
	OPROP_LESSER_HOLYW,
	OPROP_UNHYW,
	OPROP_LESSER_UNHYW,
	OPROP_WATRW,
	OPROP_LESSER_WATRW,
	OPROP_DEEPW,
	OPROP_PSIOW,
	OPROP_LESSER_PSIOW,
	OPROP_DRANW,
	OPROP_VORPW,
	OPROP_MORGW,
	OPROP_LESSER_MORGW,
	OPROP_WRTHW,
	OPROP_CCLAW,
	OPROP_LIVEW,
	OPROP_ASECW,
	OPROP_PSECW,
	OPROP_GOATW,
	OPROP_OCLTW,
	OPROP_RETRW,
	OPROP_RAKUW,
	OPROP_SPIKED,
	OPROP_BLADED,
	OPROP_SOTHW,
	OPROP_SFLMW,
	OPROP_GSSDW,
	OPROP_BLAST,
	OPROP_BRIL,
	OPROP_CGLZ,
	OPROP_RWTH,
	OPROP_RBRD,
	OPROP_SLIF,
	OPROP_MORTW,
	OPROP_TDTHW,
	OPROP_SFUWW,
	OPROP_TACTB,
	OPROP_INSTW,
	OPROP_ELFLW,
	OPROP_CURS,
	OPROP_BYAK,
	MAX_OPROP
};

#define OPROP_LISTSIZE	((MAX_OPROP-1)/32 + 1)

struct obj {
	struct obj *nobj;
	union {	/* for use with obj->where */
		struct obj * nexthere;		/* OBJ_FLOOR: next object on this level's xy-coord */
		struct obj * ocontainer;	/* OBJ_CONTAINED: container this obj is in */
		struct monst * ocarry;		/* OBJ_MINVENT: monster carrying this obj */
		struct trap * otrap;		/* OBJ_INTRAP: trap containing this obj */
	};
	struct obj *cobj;	/* contents list for containers */
	unsigned o_id;
	xchar ox,oy;
	int otyp;		/* object class number */
	unsigned owt;	/* object weight */
	long quan;		/* number of items */

	schar spe;		/* quality of weapon, armor or ring (+ or -)
				   number of charges for wand ( >= -1 )
				   marks your eggs, spinach tins
				   royal coffers for a court ( == 2)
				   tells which fruit a fruit is
				   special for uball and amulet
				   do not decay for corpses
				   historic and gender for statues
				   loyalty, gender, and template for figurines */
#define STATUE_HISTORIC 0x01
#define STATUE_MALE     0x02
#define STATUE_FEMALE   0x04
#define STATUE_FACELESS 0x08
#define STATUE_EPRE		0x10

#define FIGURINE_LOYAL	0x01
#define FIGURINE_MALE	0x02
#define FIGURINE_FEMALE	0x04
#define FIGURINE_FLAGS	0x07
#define FIGURINE_PSEUDO	(0x07+1)

#define check_fig_template(spe_val, fig_temp)	((spe_val&~(FIGURINE_FLAGS)) == (fig_temp))
	char	oclass;		/* object class */
	char	invlet;		/* designation in inventory */
	int		oartifact;	/* artifact array index */
	schar 	altmode; 	/* alternate modes - eg. SMG, double Lightsaber */
				/* WP_MODEs are in decreasing speed */
#define WP_MODE_AUTO	0	/* Max firing speed */
#define WP_MODE_BURST	1	/* 1/3 of max rate */
#define WP_MODE_SINGLE 	2	/* Single shot */
				/* ENG_MODEs track how reactive a preservation engine is */
#define ENG_MODE_OFF	0	/* Engine is off */
#define ENG_MODE_PYS	1	/* Engine reacts to vorpal and shredding only */
#define ENG_MODE_ENR	2	/* Engine reacts to energy damage (fire/elec/cold/acid) too */
				/* ELI_MODEs if the eilistran armor is on or off */
#define EIL_MODE_OFF	0	/* Armor is off */
#define EIL_MODE_ON		1	/* Armor is on */


	xchar where;		/* where the object thinks it is */
#define OBJ_FREE	0		/* object not attached to anything */
#define OBJ_FLOOR	1		/* object on floor */
#define OBJ_CONTAINED	2		/* object in a container */
#define OBJ_INVENT	3		/* object in the hero's inventory */
#define OBJ_MINVENT	4		/* object in a monster inventory */
#define OBJ_MIGRATING	5		/* object sent off to another level */
#define OBJ_BURIED	6		/* object buried */
#define OBJ_ONBILL	7		/* object on shk bill */
#define OBJ_MAGIC_CHEST	8		/* object in shared magic chest */
#define OBJ_INTRAP 9    /* object is trap ammo */
#define NOBJ_STATES	10

	Bitfield(cursed,1);
	Bitfield(blessed,1);
	Bitfield(unpaid,1);	/* on some bill */
	Bitfield(no_charge,1);	/* if shk shouldn't charge for this */
	Bitfield(known,1);	/* exact nature known */
	Bitfield(dknown,1);	/* color or text known */
	Bitfield(bknown,1);	/* blessing or curse known */
	Bitfield(rknown,1);	/* rustproof or not known */
	Bitfield(sknown,1);	/* stolen or not known */

	Bitfield(oeroded,2);	/* rusted/burnt/vaporized weapon/armor */
	Bitfield(oeroded2,2);	/* corroded/rotted/fractured weapon/armor */
	Bitfield(oeroded3,2);	/* tatteredness */
#define MAX_ERODE 3
	Bitfield(orotten,1);	/* rotten food */
	Bitfield(odiluted,1);	/* diluted potions */
	Bitfield(norevive,1);	/* don't revive */
	Bitfield(oerodeproof,1); /* erodeproof weapon/armor */
	Bitfield(olarva,2);	/* object has been partially brought to life */
	Bitfield(odead_larva,2);	/* object was partially brought to life, but died again */
	Bitfield(obyak,2);	/* object has been partially brought to life (as byakhee larva)*/
	Bitfield(olocked,1);	/* object is locked */
#define oarmed olocked
#define odrained olocked	/* drained corpse */
	Bitfield(obroken,1);	/* lock has been broken */
#define odebone obroken		/* corpse has been de-boned */
	Bitfield(otrapped,1);	/* container is trapped */
#define obolted otrapped	/* magic chest is permanently attached to floor */
				/* or accidental tripped rolling boulder trap */

	Bitfield(recharged,3);	/* number of times it's been recharged */
#define ostriking recharged	/* extra whip heads striking (imposes cap of +7) */
	/* 0 free bits */
	Bitfield(lamplit,1);	/* a light-source -- can be lit */
#ifdef INVISIBLE_OBJECTS
	Bitfield(oinvis,1);	/* invisible */
#endif
	Bitfield(greased,1);	/* covered with grease */

	Bitfield(in_use,1);	/* for magic items before useup items */
	Bitfield(bypass,1);	/* mark this as an object to be skipped by bhito() */
	Bitfield(lifted,1); /* dipped in potion of levitation */
	Bitfield(lightened,1);/* dipped in potion of enlightenment */
	Bitfield(shopOwned,1);	/* owned by a shopkeeper */
	Bitfield(ostolen,1); 	/* was removed from a shop without being sold */
    Bitfield(was_thrown,1); /* for pickup_thrown */
	Bitfield(fromsink,1);
	Bitfield(yours,1);	/* obj is yours (eg. thrown by you) */
	Bitfield(masters,1);	/* obj is given by a monster's master, it will not drop it */
	Bitfield(objsize,3);	/* 0-7 */
	//See objclass for values
	Bitfield(nomerge,1);	/* temporarily block from merging */
	Bitfield(forceconf,1);	/* when set on a scroll, forces the confusion effect. Meant for use with pseudo objects, isn't checked while merging */
	Bitfield(ohaluengr,1);	/* engraving on item isn't a "real" ward */
	/* 15 free bits in this field, I think -CM */
	
	int obj_material;		/*Object material (from lookup table)*/
	int sub_material;		/*Sub-material*/
		/*For gemstone mat, gemstone object id*/
		/*For mineral */
#define SUBMAT_MARBLE 1
	int obj_color;
	union {
		long bodytypeflag;	/* MB tag(s) this item goes with. Overloaded with wrathdata */
		long wrathdata;		/* MA flags this item is currently wrathful against. Overloaded with bodytypeflag; */
	};
	union {
		int	corpsenm;		/* various:       type of corpse is mons[corpsenm] */
		unsigned leashmon;	/* leash:         m_id of attached pet */
		int spestudied;		/* spellbooks:    of times a spellbook has been studied */
		int opoisonchrgs;	/* rings/weapons: number of poison doses left */
	};
	
#ifdef RECORD_ACHIEVE
#define record_achieve_special corpsenm
#endif
#define osinging corpsenm	/* song that the Singing Sword is singing */
#define OSING_FEAR		1
#define OSING_HEALING	2
#define OSING_RALLY		3
#define OSING_CONFUSE	4
#define OSING_HASTE		5
#define OSING_LETHARGY	6
#define OSING_COURAGE	7
#define OSING_DIRGE		8
#define OSING_FIRE		9
#define OSING_FROST		10
#define OSING_ELECT		11
#define OSING_QUAKE		12
#define OSING_OPEN		13
#define OSING_DEATH		14
#define OSING_LIFE		15
#define OSING_INSANE	16
#define OSING_CANCEL	17
#define SELECT_NOTHING	18	/* must be non-zero */

	
	int opoisoned; /* poisons smeared on the weapon*/
#define OPOISON_NONE	0x00
#define OPOISON_BASIC	0x001 /* Deadly Poison */
#define OPOISON_FILTH	0x002 /* Deadly Sickness */
#define OPOISON_SLEEP	0x004 /* Sleeping Poison */
#define OPOISON_BLIND	0x008 /* Blinding Poison */
#define OPOISON_PARAL	0x010 /* Paralysis Poison */
#define OPOISON_AMNES	0x020 /* Amnesia Poison */
#define OPOISON_ACID	0x040 /* Acid coating */
#define OPOISON_SILVER	0x080 /* Silver coating */
#define OPOISON_HALLU	0x100 /* Hallucination coating */
#define OPOISON_DIRE	0x200 /* Strong poison */
#define NUM_POISONS		10	/* number of specifiable poison coatings */

	unsigned long int oproperties[OPROP_LISTSIZE];/* special properties */

	unsigned oeaten;	/* nutrition left in food, if partly eaten */
	long age;		/* creation date */
	/* in order to prevent alignment problems oextra should
	   be (or follow) a long int */
	long owornmask;
	/* Weapons and artifacts */
//	define W_ARM	    0x00000001L /* Body armor */
//	define W_ARMC	    0x00000002L /* Cloak */
//	define W_ARMH	    0x00000004L /* Helmet/hat */
//	define W_ARMS	    0x00000008L /* Shield */
//	define W_ARMG	    0x00000010L /* Gloves/gauntlets */
//	define W_ARMF	    0x00000020L /* Footwear */
//ifdef TOURIST
//	define W_ARMU	    0x00000040L /* Undershirt */
//	define W_ARMOR	     (W_ARM | W_ARMC | W_ARMH | W_ARMS | W_ARMG | W_ARMF | W_ARMU)
//else
//	define W_ARMOR	     (W_ARM | W_ARMC | W_ARMH | W_ARMS | W_ARMG | W_ARMF)
//endif
//define W_WEP	    0x00000100L /* Wielded weapon */
//define W_QUIVER     0x00000200L /* Quiver for (f)iring ammo */
//define W_SWAPWEP    0x00000400L /* Secondary weapon */
//define W_ART	    0x00001000L /* Carrying artifact (not really worn) */
//define W_ARTI	    0x00002000L /* Invoked artifact  (not really worn) */
	/* Amulets, rings, tools, and other items */
//define W_AMUL	    0x00010000L /* Amulet */
//define W_RINGL	    0x00020000L /* Left ring */
//define W_RINGR	    0x00040000L /* Right ring */
//define W_RING	    (W_RINGL | W_RINGR)
//define W_TOOL	    0x00080000L /* Eyewear */
//define W_SADDLE     0x00100000L	/* KMH -- For riding monsters */
//define W_BALL	    0x00200000L /* Punishment ball */
//define W_CHAIN	    0x00400000L /* Punishment chain */
	long oward;
			/*Records the warding sign of spellbooks. */
			/*Records the warding sign of scrolls of ward. */
			/*Records the warding sign of rings. */
			/*Records runes for wooden weapons */
			
	long ovar1;		/* extra variable. Specifies: */
#define ovar1_charges ovar1
#define ovar1_ampule ovar1
#define ovar1_insightlevel ovar1
#define ovar1_dollTypes ovar1
#define ovar1_darklight ovar1
#define ovar1_eilistran_charges ovar1
#define ovar1_necronomicon ovar1
#define ovar1_infinitespells ovar1
#define ovar1_lostNames ovar1
#define ovar1_moonPhase ovar1
#define ovar1_pincerTarget ovar1
#define ovar1_heads ovar1
#define ovar1_lightsaberHandle ovar1
#define ovar1_webGush ovar1
#define ovar1_mgclcknm ovar1
#define ovar1_projectileSkill ovar1
#define ovar1_acidSplashDamage ovar1
#define ovar1_corpseRumorCooldown ovar1
#define ovar1_secretsSecret ovar1
#define ovar1_iea_upgrades ovar1
#define ovar1_puzzle_steps ovar1

#define ovar1_gober ovar1
#define ovar1_seals ovar1
#define ovar1_timeout ovar1
#define ovar1_lifeDeath ovar1
#define ovar1_artiTheftType ovar1
#define ovar1_heard ovar1
	/* Number of viperwhip heads */
	/* Moon axe phase */
	/* Acid venom non-1d6 damage */
	/* Mask erosion (may be moved to oeroded3 at some point) */
	/* Ammo for futuretech weapons */
	/* Hilt engraving for lightsabers */
	/* Ampule type for hypospray ampules */
	/* Engraving for rings */
	/* doll's tear */
	/* Upgrades made to imperial elven armor */
#define obj_type_uses_ovar1(otmp) (\
	   (otmp)->otyp == VIPERWHIP \
	|| (otmp)->otyp == MOON_AXE \
	|| (otmp)->otyp == ACID_VENOM \
	|| (otmp)->otyp == MASK \
	|| is_blaster((otmp)) \
	|| (otmp)->otyp == RAYGUN \
	|| (otmp)->otyp == SEISMIC_HAMMER \
	|| is_vibroweapon((otmp)) \
	|| (otmp)->otyp == LIGHTSABER \
	|| (otmp)->otyp == BEAMSWORD \
	|| (otmp)->otyp == DOUBLE_LIGHTSABER \
	|| (otmp)->otyp == HYPOSPRAY_AMPULE \
	|| (otmp)->oclass == RING_CLASS \
	|| (otmp)->otyp == DOLL_S_TEAR \
	|| (otmp)->otyp == PINCER_STAFF \
	|| is_imperial_elven_armor(otmp) \
	)
#define ECLIPSE_MOON	0
#define CRESCENT_MOON	1
#define HALF_MOON		2
#define GIBBOUS_MOON	3
#define FULL_MOON	 	4

#define check_imp_mod(obj, prop) ((obj)->ovar1_iea_upgrades&(prop))
#define add_imp_mod(obj, prop) ((obj)->ovar1_iea_upgrades |= (prop))

#define IEA_FIXED_ABIL	0x00000001L
#define IEA_FAST_HEAL	0x00000002L
#define IEA_REFLECTING	0x00000004L
#define IEA_SICK_RES	0x00000008L
#define IEA_HALF_PHDAM	0x00000010L
#define IEA_HALF_SPDAM	0x00000020L
#define IEA_DISPLACED	0x00000040L
#define IEA_INVIS		0x00000080L
#define IEA_SWIMMING	0x00000100L
#define IEA_GOPOWER		0x00000200L
#define IEA_GODEXTERITY	0x00000400L
#define IEA_INC_DAM		0x00000800L
#define IEA_BOLTS		0x00001000L
#define IEA_FLYING		0x00002000L
#define IEA_JUMPING		0x00004000L
#define IEA_FAST		0x00008000L
#define IEA_TELEPORT	0x00010000L
#define IEA_NOBREATH	0x00020000L
#define IEA_LIFESENSE	0x00040000L
#define IEA_SEE_INVIS	0x00080000L
#define IEA_TELEPAT		0x00100000L
#define IEA_BLIND_RES	0x00200000L
#define IEA_INC_ACC		0x00400000L
#define IEA_TELE_CNTRL	0x00800000L
#define IEA_KICKING		0x01000000L
#define IEA_PROT_SHAPE	0x02000000L
#define IEA_DEFLECTION	0x04000000L
#define IEA_STRANGLE	0x08000000L
#define IEA_MITHRIL		0x10000000L

#define IEA_NOUPGRADES	0x80000000L
#define IEA_HELM_MASK	(IEA_NOBREATH|IEA_LIFESENSE|IEA_SEE_INVIS|IEA_TELEPAT|IEA_BLIND_RES|IEA_INC_ACC|IEA_TELE_CNTRL|IEA_PROT_SHAPE)
#define IEA_BODY_MASK	(IEA_FLYING|IEA_FIXED_ABIL|IEA_FAST_HEAL|IEA_REFLECTING|IEA_SICK_RES|IEA_HALF_PHDAM|IEA_HALF_SPDAM|IEA_DISPLACED|IEA_INVIS|IEA_DEFLECTION|IEA_MITHRIL)
#define IEA_GLOVE_MASK	(IEA_SWIMMING|IEA_GOPOWER|IEA_GODEXTERITY|IEA_INC_DAM|IEA_BOLTS|IEA_STRANGLE)
#define IEA_BOOT_MASK	(IEA_JUMPING|IEA_FAST|IEA_TELEPORT|IEA_KICKING)



	/* Songs that the Singing Sword has heard */
	/* Spirits bound into the Pen of the Void */
	/* The ema of damage taken for gloves of the berserker */
	/* Life/Death for the scalpel of life and death */
	/* Theft type for stealing artifacts (reaver (scimitar) and avarice (shortsword) */
	/* Misc data for the artifact spellbooks */
#define obj_art_uses_ovar1(otmp) (\
	   (otmp)->oartifact == ART_SINGING_SWORD \
	|| (otmp)->oartifact == ART_PEN_OF_THE_VOID \
	|| (otmp)->oartifact == ART_GAUNTLETS_OF_THE_BERSERKER \
	|| (otmp)->oartifact == ART_SCALPEL_OF_LIFE_AND_DEATH \
	|| (otmp)->oartifact == ART_REAVER \
	|| (otmp)->oartifact == ART_AVARICE \
	|| (otmp)->oartifact == ART_NECRONOMICON \
	|| (otmp)->oartifact == ART_BOOK_OF_LOST_NAMES \
	|| (otmp)->oartifact == ART_BOOK_OF_INFINITE_SPELLS \
	)
#define OHEARD_FEAR		0x00000001L
#define OHEARD_HEALING	0x00000002L
#define OHEARD_RALLY	0x00000004L
#define OHEARD_CONFUSE	0x00000008L
#define OHEARD_HASTE	0x00000010L
#define OHEARD_LETHARGY	0x00000020L
#define OHEARD_COURAGE	0x00000040L
#define OHEARD_DIRGE	0x00000080L
#define OHEARD_FIRE		0x00000100L
#define OHEARD_FROST	0x00000200L
#define OHEARD_ELECT	0x00000400L
#define OHEARD_QUAKE	0x00000800L
#define OHEARD_OPEN		0x00001000L
#define OHEARD_DEATH	0x00002000L
#define OHEARD_LIFE		0x00004000L
#define OHEARD_INSANE	0x00008000L
#define OHEARD_CANCEL	0x00010000L

	schar gifted; /*gifted is of type aligntyp.  For some reason aligntyp isn't being seen at compile*/
	
	struct mask_properties *mp;

	struct ls_t * light;

	struct timer * timed;

	union oextra * oextra_p;
};

//Useful items (ovar1 flags for planned cloak of useful items)
#define USE_DAGGER	0x0000001L
#define TWO_DAGGER	0x0000002L
#define USE_LANTERN	0x0000004L
#define TWO_LANTERN	0x0000008L
#define USE_MIRROR	0x0000010L
#define TWO_MIRROR	0x0000020L
#define USE_POLE	0x0000040L
#define USE_SACK	0x0000100L
#define USE_GOLD	0x0000400L
#define USE_CHEST	0x0001000L
#define USE_DOOR	0x0004000L
#define USE_GEMS	0x0010000L
#define USE_PONY	0x0040000L
#define USE_PIT		0x0100000L
#define USE_POTION	0x0400000L
#define USE_LAND	0x1000000L
#define USE_DOGS	0x4000000L
//Useful items 2
#define USE_SCROLL	0x0000001
#define USE_WINDOW	0x0000004
#define USE_RAM		0x0000010
#define USE_STARS	0x0000040
// #define USE_	0x0000100
// #define USE_	0x0000400
// #define USE_	0x0001000

#define SPEC_FIRE		0x0000001L
#define SPEC_COLD		0x0000002L
#define SPEC_ELEC		0x0000004L
#define SPEC_ACID		0x0000008L
#define SPEC_WILT		0x0000010L
#define SPEC_RADI		0x0000020L
#define SPEC_BONUS		0x0000040L
#define SPEC_DESTRUCTOR	0x0000080L
#define SPEC_MARIONETTE	0x0000100L
#define SPEC_HYPO		0x0000200L

// flags for use with readobjnam()
#define WISH_QUIET      0x0000001L
#define WISH_WIZARD     0x0000002L
#define WISH_VERBOSE	0x0000004L
#define WISH_ARTALLOW	0x0000008L
#define WISH_MKLEV		(WISH_QUIET|WISH_WIZARD|WISH_ARTALLOW)

#define WISH_NOTHING	0x0000100L
#define WISH_SUCCESS	0x0000200L
#define WISH_FAILURE	0x0000400L
#define WISH_DENIED		0x0000800L
#define WISH_ARTEXISTS		0x0001000L




#define newobj(xl)	(struct obj *)alloc((unsigned)(xl) + sizeof(struct obj))

/* Weapons and weapon-tools */
/* KMH -- now based on skill categories.  Formerly:
 *	#define is_sword(otmp)	(otmp->oclass == WEAPON_CLASS && \
 *			 objects[otmp->otyp].oc_wepcat == WEP_SWORD)
 *	#define is_blade(otmp)	(otmp->oclass == WEAPON_CLASS && \
 *			 (objects[otmp->otyp].oc_wepcat == WEP_BLADE || \
 *			  objects[otmp->otyp].oc_wepcat == WEP_SWORD))
 *	#define is_weptool(o)	((o)->oclass == TOOL_CLASS && \
 *			 objects[(o)->otyp].oc_weptool)
 *	#define is_multigen(otyp) (otyp <= SHURIKEN)
 *	#define is_poisonable(otyp) (otyp <= BEC_DE_CORBIN)
 */
#define artitypematch(a, o) (( (a)->otyp ) == BEAMSWORD ? ((o)->otyp==BROADSWORD) : \
							( (a)->otyp ) == UNIVERSAL_KEY ? ((o)->otyp==SKELETON_KEY) : \
							( (a)->otyp ) == ROUNDSHIELD ? ((o)->otyp==DWARVISH_ROUNDSHIELD) : \
							( (a) == &artilist[ART_LANCE_OF_LONGINUS] ) ? ((o)->otyp==SCALPEL||(o)->otyp==LIGHTSABER) : \
							( (a) == &artilist[ART_GUNGNIR] ) ? (is_spear(o)) : \
							( (a) == &artilist[ART_SINGING_SWORD] ) ? ((o)->otyp == LONG_SWORD || (o)->otyp == RAPIER || (o)->otyp == SABER) : \
							( (a) == &artilist[ART_DIRGE] ) ? (o->oclass == WEAPON_CLASS || is_weptool(o)) : \
							( (a) == &artilist[ART_SKY_REFLECTED] ) ? (o->obj_material == MERCURIAL && (o->oclass == WEAPON_CLASS || is_weptool(o))) : \
							((a) == &artilist[ART_FIRE_BRAND] || (a) == &artilist[ART_FROST_BRAND]) ? \
								(u.brand_otyp == STRANGE_OBJECT ? \
									((is_blade((o)) && objects[(o)->otyp].oc_size < MZ_HUGE && \
										!is_vibroweapon((o)) && \
										!is_rakuyo((o)) && \
										!((o)->otyp == SET_OF_CROW_TALONS) && \
										!((o)->otyp == CROW_QUILL) && \
										!((o)->otyp == CRYSTAL_SWORD) && \
										!((o)->otyp == RUNESWORD) && \
										!((o)->otyp == MIRRORBLADE)) \
									|| (o)->otyp == GAUNTLETS) : \
									(u.brand_otyp == (o)->otyp)) : \
							(a)->otyp == (o)->otyp)
#define is_blade(otmp)	(otmp->oclass == WEAPON_CLASS && \
			 objects[otmp->otyp].oc_skill >= P_DAGGER && \
			 objects[otmp->otyp].oc_skill <= P_SABER)
#define is_knife(otmp)	(otmp->oclass == WEAPON_CLASS && \
			 (objects[otmp->otyp].oc_skill == P_DAGGER || \
			 objects[otmp->otyp].oc_skill == P_KNIFE))
#define is_axe(otmp)	((otmp->oclass == WEAPON_CLASS || \
			 otmp->oclass == TOOL_CLASS) && \
			 objects[otmp->otyp].oc_skill == P_AXE)
#define is_pick(otmp)	(((otmp->oclass == WEAPON_CLASS || \
			 otmp->oclass == TOOL_CLASS) && \
			 (objects[otmp->otyp].oc_skill == P_PICK_AXE)) || \
			  arti_digs(otmp))
#define is_sword(otmp)	(otmp->oclass == WEAPON_CLASS && \
			 objects[otmp->otyp].oc_skill >= P_SHORT_SWORD && \
			 objects[otmp->otyp].oc_skill <= P_SABER)
#define is_rapier(otmp)	(otmp->oclass == WEAPON_CLASS && (\
			 otmp->otyp == RAPIER || \
			 otmp->otyp == RAKUYO || \
			 otmp->otyp == RAKUYO_SABER || \
			 otmp->otyp == CROW_QUILL))
#define is_rakuyo(otmp)	(otmp->otyp == RAKUYO || \
			 otmp->otyp == RAKUYO_SABER || \
			 otmp->otyp == RAKUYO_DAGGER)
#define is_mercy_blade(otmp)	(otmp->otyp == BLADE_OF_MERCY || \
			 otmp->otyp == BLADE_OF_GRACE || \
			 otmp->otyp == BLADE_OF_PITY)
#define rakuyo_prop(otmp)	(check_oprop(otmp, OPROP_RAKUW))
#define is_insight_weapon(otmp) (check_oprop(otmp, OPROP_CCLAW) || \
			 is_rakuyo(otmp) ||\
			 rakuyo_prop(otmp) || \
			 otmp->otyp == PINCER_STAFF || \
			 otmp->otyp == CARCOSAN_STING || \
			 check_oprop(otmp,OPROP_GSSDW) || \
			 check_oprop(otmp,OPROP_INSTW) || \
			 check_oprop(otmp,OPROP_ELFLW) || \
			 otmp->oartifact == ART_HOLY_MOONLIGHT_SWORD || \
			 otmp->oartifact == ART_BLOODLETTER || \
			 otmp->oartifact == ART_LASH_OF_THE_COLD_WASTE || \
			 otmp->oartifact == ART_GOLDEN_SWORD_OF_Y_HA_TALLA || \
			 otmp->oartifact == ART_RUINOUS_DESCENT_OF_STARS || \
			 (otmp->oartifact == ART_PEN_OF_THE_VOID && otmp->ovar1_seals&SEAL_OSE) ||\
			 otmp->obj_material == MERCURIAL || \
			 is_mercy_blade(otmp) || \
			 otmp->otyp == ISAMUSEI ||\
			 otmp->otyp == DISKOS ||\
			 otmp->otyp == BESTIAL_CLAW)
#define is_future_otyp(typ)	(\
		typ == LIGHTSABER ||\
		typ == BEAMSWORD ||\
		typ == DOUBLE_LIGHTSABER ||\
		typ == VIBROBLADE ||\
		typ == WHITE_VIBROSWORD ||\
		typ == GOLD_BLADED_VIBROSWORD ||\
		typ == WHITE_VIBROZANBATO ||\
		typ == GOLD_BLADED_VIBROZANBATO ||\
		typ == RED_EYED_VIBROSWORD ||\
		typ == SEISMIC_HAMMER ||\
		typ == FORCE_PIKE ||\
		typ == DOUBLE_FORCE_BLADE ||\
		typ == FORCE_BLADE ||\
		typ == FORCE_SWORD ||\
		typ == WHITE_VIBROSPEAR ||\
		typ == GOLD_BLADED_VIBROSPEAR ||\
		(typ >= PISTOL && typ <= RAYGUN) ||\
		(typ >= SHOTGUN_SHELL && typ <= LASER_BEAM) ||\
		typ == FLACK_HELMET ||\
		typ == PLASTEEL_HELM ||\
		typ == PLASTEEL_ARMOR ||\
		typ == JUMPSUIT ||\
		typ == BODYGLOVE ||\
		typ == PLASTEEL_GAUNTLETS ||\
		typ == PLASTEEL_BOOTS ||\
		typ == IMPERIAL_ELVEN_HELM ||\
		typ == IMPERIAL_ELVEN_GAUNTLETS ||\
		typ == IMPERIAL_ELVEN_ARMOR ||\
		typ == IMPERIAL_ELVEN_BOOTS ||\
		(typ >= SENSOR_PACK && typ <= HYPOSPRAY_AMPULE) ||\
		typ == BULLET_FABBER ||\
		typ == PROTEIN_PILL\
		)

#define ensouled_item(otmp)	(((otmp)->otyp >= WAGE_OF_SLOTH && (otmp)->otyp <= WAGE_OF_PRIDE)\
			 || (otmp)->otyp == VITAL_SOULSTONE\
			 || (otmp)->otyp == SPIRITUAL_SOULSTONE\
			 || ((otmp)->otyp >= EFFIGY && (otmp)->otyp <= DOLL_S_TEAR)\
			 || (otmp)->otyp == HOLY_SYMBOL_OF_THE_BLACK_MOTHE\
			 || (otmp)->oartifact == ART_ESSCOOAHLIPBOOURRR\
			 || (otmp)->otyp == MAGIC_LAMP\
			 || (otmp)->otyp == CANDLE_OF_INVOCATION\
			 || (otmp)->otyp == RIN_WISHES\
			 || (otmp)->otyp == AMULET_OF_LIFE_SAVING\
			 || (otmp)->otyp == MISOTHEISTIC_PYRAMID\
			 || (otmp)->otyp == MISOTHEISTIC_FRAGMENT\
			 || (otmp)->otyp == DIMENSIONAL_LOCK\
			 || (otmp)->otyp == PRESERVATIVE_ENGINE\
			 || (otmp)->otyp == ARMOR_SALVE\
			 || (otmp)->otyp == LIVING_MASK\
			 || (otmp)->oclass == TILE_CLASS\
			 || (otmp)->otyp == ANTIMAGIC_RIFT\
			 || (otmp)->otyp == CATAPSI_VORTEX\
			)
#define goat_acidable(otmp)	(accepts_weapon_oprops(otmp)\
				&& !check_oprop((otmp), OPROP_GOATW) && !check_oprop((otmp), OPROP_ACIDW) && !check_oprop((otmp), OPROP_LESSER_ACIDW))
#define goat_droolable(otmp) (accepts_weapon_oprops(otmp) && !check_oprop((otmp), OPROP_GOATW) && !check_oprop((otmp), OPROP_ACIDW))
#define yog_magicable(otmp)	(accepts_weapon_oprops(otmp)\
				&& !check_oprop((otmp), OPROP_SOTHW) && !check_oprop((otmp), OPROP_MAGCW) && !check_oprop((otmp), OPROP_LESSER_MAGCW))
#define yog_windowable(otmp) (accepts_weapon_oprops(otmp) && !check_oprop((otmp), OPROP_SOTHW) && !check_oprop((otmp), OPROP_MAGCW))
#define sflm_able(otmp)	(((otmp)->obj_material == SILVER || (otmp)->obj_material == PLATINUM || (otmp)->obj_material == MITHRIL)\
			 || ((otmp)->oartifact == ART_IBITE_ARM && artinstance[ART_IBITE_ARM].IbiteUpgrades&IPROP_REFLECT))
#define sflm_offerable(otmp)	(accepts_weapon_oprops(otmp) && sflm_able(otmp) && !check_oprop(otmp, OPROP_SFLMW))
#define sflm_mirrorable(otmp)	(((otmp)->oclass == WEAPON_CLASS || is_weptool(otmp) || is_suit(otmp) || is_shield(otmp))\
				 && sflm_able(otmp) && !check_oprop(otmp, OPROP_REFL))
#define sflm_glazeable(otmp)	(is_suit(otmp) && sflm_able(otmp) && !check_oprop(otmp, OPROP_CGLZ) && !check_oprop(otmp, OPROP_BCRS))
#define sflm_wrathable(otmp)	(is_gloves(otmp) && sflm_able(otmp) && !check_oprop(otmp, OPROP_RWTH))
#define sflm_burdenable(otmp)	(is_boots(otmp) && sflm_able(otmp) && !check_oprop(otmp, OPROP_RBRD))
#define sflm_lifeable(otmp)	(is_helmet(otmp) && sflm_able(otmp) && !check_oprop(otmp, OPROP_SLIF) && !check_oprop(otmp, OPROP_LIFE))
#define sflm_smeltable_silver(otmp)	(is_metallic(otmp) && (otmp)->obj_material != SILVER)
#define sflm_smeltable_platinum(otmp)	(is_metallic(otmp) && (otmp)->obj_material != PLATINUM)
#define sflm_smeltable_mithril(otmp)	(is_metallic(otmp) && (otmp)->obj_material != MITHRIL)
#define sflm_mortalable(otmp)	(check_oprop(otmp, OPROP_SFLMW) && !check_oprop(otmp, OPROP_MORTW))
#define sflm_truedeathable(otmp)	(check_oprop(otmp, OPROP_SFLMW) && !check_oprop(otmp, OPROP_TDTHW))
#define sflm_unworthyable(otmp)	(check_oprop(otmp, OPROP_SFLMW) && !check_oprop(otmp, OPROP_SFUWW))
/* Can accept weapon props.  Not necessarily a weapon that you wield. */
#define accepts_weapon_oprops(otmp)	((otmp)->oclass == WEAPON_CLASS || is_weptool(otmp) || is_gloves(otmp) || is_boots(otmp) || is_helmet(otmp))
#define is_pole(otmp)	(((otmp)->oclass == WEAPON_CLASS || \
			(otmp)->oclass == TOOL_CLASS) && \
			 (objects[(otmp)->otyp].oc_skill == P_POLEARMS || \
			  objects[(otmp)->otyp].oc_skill == P_LANCE || \
			  (otmp)->otyp==AKLYS || \
			  (otmp)->otyp==DISKOS || \
			  (check_oprop(otmp, OPROP_CCLAW) && u.uinsight >= 15) || \
			  (otmp)->oartifact==ART_SOL_VALTIVA || \
			  (otmp)->oartifact==ART_SHADOWLOCK || \
			  (otmp)->oartifact==ART_RUYI_JINGU_BANG || \
			  ((otmp)->oartifact==ART_GOLDEN_SWORD_OF_Y_HA_TALLA && (otmp)->otyp == BULLWHIP) || \
			  (otmp)->oartifact==ART_DEATH_SPEAR_OF_KEPTOLO || \
			  ((otmp)->oartifact==ART_PEN_OF_THE_VOID && (otmp)->ovar1_seals&SEAL_MARIONETTE ) \
			 ))
#define is_bad_melee_pole(otmp) (!((otmp)->otyp == POLEAXE ||\
									(otmp)->otyp == DISKOS ||\
									is_vibropike(otmp) ||\
									(otmp)->oartifact == ART_WEBWEAVER_S_CROOK ||\
									(otmp)->oartifact == ART_SILENCE_GLAIVE ||\
									(otmp)->oartifact == ART_HEARTCLEAVER ||\
									(otmp)->oartifact == ART_GREEN_DRAGON_CRESCENT_BLAD ||\
									(otmp)->oartifact == ART_CRUCIFIX_OF_THE_MAD_KING ||\
									(otmp)->oartifact == ART_SOL_VALTIVA ||\
									(otmp)->oartifact == ART_DEATH_SPEAR_OF_KEPTOLO ||\
									(otmp)->oartifact == ART_SHADOWLOCK ||\
									(otmp)->oartifact == ART_RUYI_JINGU_BANG ||\
									(otmp)->oartifact == ART_GOLDEN_SWORD_OF_Y_HA_TALLA ||\
									(otmp)->oartifact == ART_PEN_OF_THE_VOID\
								) && is_pole(otmp))
#define is_spear(otmp)	((otmp->oclass == WEAPON_CLASS || is_weptool(otmp)) && \
			 objects[otmp->otyp].oc_skill == P_SPEAR)
#define is_hammer(otmp)	((otmp->oclass == WEAPON_CLASS || is_weptool(otmp)) && \
			 objects[otmp->otyp].oc_skill == P_HAMMER)
#define is_farm(otmp)	((otmp->oclass == WEAPON_CLASS || is_weptool(otmp)) && \
			 objects[otmp->otyp].oc_skill == P_HARVEST)
#define is_sickle(otmp)	((otmp)->otyp == ELVEN_SICKLE || (otmp)->otyp == SICKLE)

#define is_launcher(otmp)	(otmp->oclass == WEAPON_CLASS && \
			 ((objects[otmp->otyp].oc_skill >= P_BOW && \
			 objects[otmp->otyp].oc_skill <= P_CROSSBOW) || otmp->otyp == ATLATL))
#define is_ammo(otmp)	((otmp->oclass == WEAPON_CLASS || \
			 otmp->oclass == GEM_CLASS) && \
			 ((objects[otmp->otyp].oc_skill >= -P_CROSSBOW && \
			 objects[otmp->otyp].oc_skill <= -P_BOW)))
#define is_grenade(otmp)	(is_ammo(otmp) && \
			 	 objects[(otmp)->otyp].w_ammotyp == WP_GRENADE)
#define ammo_and_launcher(otmp,ltmp) \
			 ((otmp) && (ltmp) && (\
			  (\
			   (ltmp->otyp == BFG) ||\
			   (ltmp->oartifact == ART_PEN_OF_THE_VOID && ltmp->ovar1_seals&SEAL_EVE) ||\
			   (ltmp->otyp == MASS_SHADOW_PISTOL && ltmp->cobj && (otmp->otyp == ltmp->cobj->otyp)) ||\
			   (ltmp->otyp == ATLATL && is_spear(otmp)) ||\
			   (\
			    (otmp->objsize == (ltmp)->objsize || objects[(ltmp)->otyp].oc_skill == P_SLING) &&\
			    (objects[(otmp)->otyp].w_ammotyp & objects[(ltmp)->otyp].w_ammotyp) && \
			    (objects[(otmp)->otyp].oc_skill == -objects[(ltmp)->otyp].oc_skill)\
			   )\
			  )\
			 ))
#define is_missile(otmp)	((otmp->oclass == WEAPON_CLASS || \
			 otmp->oclass == TOOL_CLASS || otmp->oclass == GEM_CLASS) && \
			 ((objects[otmp->otyp].oc_skill >= -P_BOOMERANG && \
				 objects[otmp->otyp].oc_skill <= -P_DART) ||\
				 objects[otmp->otyp].oc_skill == -P_SLING)\
			 )
#define is_enchantable(o) ((o)->oclass == ARMOR_CLASS || (o)->oclass == WEAPON_CLASS || is_weptool(o))
#define is_weptool(o)	((o)->oclass == TOOL_CLASS && \
			 objects[(o)->otyp].oc_skill != P_NONE)
#define is_worn_tool(o)	((o)->otyp == BLINDFOLD || (o)->otyp == ANDROID_VISOR || \
							 (o)->otyp == TOWEL || (o)->otyp == LENSES || (o)->otyp == SUNGLASSES || \
							 (o)->otyp == LIVING_MASK || (o)->otyp == MASK || (o)->otyp == R_LYEHIAN_FACEPLATE)
#define is_opaque_worn_tool(o)	((o)->otyp == BLINDFOLD || (o)->otyp == TOWEL || (o)->otyp == R_LYEHIAN_FACEPLATE)
#define is_instrument(o)	((o)->otyp >= FLUTE && \
			 (o)->otyp <= DRUM_OF_EARTHQUAKE)
#define is_mummy_wrap(o)	((o)->otyp == MUMMY_WRAPPING || \
			 (o)->otyp == PRAYER_WARDED_WRAPPING)
#define is_lightsaber(otmp) ((otmp)->otyp == LIGHTSABER || \
							 (otmp)->otyp == KAMEREL_VAJRA || \
							 (otmp)->otyp == ROD_OF_FORCE || \
							 (otmp)->otyp == BEAMSWORD || \
							 (otmp)->otyp == DOUBLE_LIGHTSABER)
#define is_gemable_lightsaber(otmp) (((otmp)->otyp == LIGHTSABER\
							  || (otmp)->otyp == BEAMSWORD\
							  || (otmp)->otyp == DOUBLE_LIGHTSABER)\
							&& (otmp)->oartifact != ART_ANNULUS\
							&& (otmp)->oartifact != ART_INFINITY_S_MIRRORED_ARC\
							)
#define valid_focus_gem(otmp) ((otmp)->oclass == GEM_CLASS && ((otmp)->otyp < LUCKSTONE \
								|| (otmp)->otyp == CHUNK_OF_FOSSIL_DARK \
								|| (otmp)->otyp == CATAPSI_VORTEX \
								|| (otmp)->otyp == ANTIMAGIC_RIFT))
#define is_vibroweapon(otmp) (force_weapon(otmp) || \
							  is_vibrosword(otmp) || \
							  otmp->otyp == VIBROBLADE || \
							  is_vibropike(otmp))
#define is_vibrosword(otmp)	 ((otmp)->otyp == WHITE_VIBROSWORD || \
						  (otmp)->otyp == GOLD_BLADED_VIBROSWORD || \
						  (otmp)->otyp == WHITE_VIBROZANBATO || \
						  (otmp)->otyp == GOLD_BLADED_VIBROZANBATO || \
						  (otmp)->otyp == RED_EYED_VIBROSWORD)
#define is_vibropike(otmp)	 ((otmp)->otyp ==  WHITE_VIBROSPEAR || \
						  (otmp)->otyp == GOLD_BLADED_VIBROSPEAR || \
						  (otmp)->otyp == FORCE_PIKE)
#define fast_weapon(otmp)	 ((((otmp)->otyp == WHITE_VIBROSWORD || \
							  (otmp)->otyp == GOLD_BLADED_VIBROSWORD || \
							  (otmp)->otyp == WHITE_VIBROZANBATO || \
							  (otmp)->otyp == GOLD_BLADED_VIBROZANBATO || \
							  (otmp)->otyp ==  WHITE_VIBROSPEAR || \
							  (otmp)->otyp == GOLD_BLADED_VIBROSPEAR) && otmp->spe >= 2)\
						  || fast_mercurial(otmp)\
						  || otmp->oartifact == ART_FIRE_BRAND\
						  )
 
#define fast_mercurial(otmp)	 ((otmp)->obj_material == MERCURIAL && (carried(otmp) ? u.ulevel >= 10 : mcarried(otmp) ? otmp->ocarry->m_lev >= 10 : FALSE))
#define is_wet_merc(obj) (is_streaming_merc(obj) || is_melting_merc(obj))
#define is_melting_merc(obj) ((obj)->obj_material == MERCURIAL && obj->where != OBJ_MINVENT && obj->where != OBJ_INVENT)
#define is_streaming_merc(obj) ((obj)->obj_material == MERCURIAL && (obj->where == OBJ_MINVENT ? mon_merc_streaming(obj) : \
							obj->where == OBJ_INVENT ? you_merc_streaming(obj) : FALSE))
#define mon_merc_streaming(obj) ((obj->ocarry->encouraged + (obj->ocarry->mtame ? (obj->ocarry->mtame - 5) : 0)) >= 3)
#define you_merc_streaming(obj) (Insanity <= 20 && !u.veil)

#define is_kinstealing_merc(obj) ((obj)->obj_material == MERCURIAL && (obj->where == OBJ_MINVENT ? mon_merc_kinstealing(obj) : \
							obj->where == OBJ_INVENT ? you_merc_kinstealing(obj) : FALSE))
#define mon_merc_kinstealing(obj) ((obj->ocarry->encouraged + (obj->ocarry->mtame ? (obj->ocarry->mtame - 5) : 0)) < 0)
#define you_merc_kinstealing(obj) (Insanity > 50)

#define is_chained_merc(obj) ((obj)->obj_material == MERCURIAL && (obj->where == OBJ_MINVENT ? mon_merc_chained(obj) : \
							obj->where == OBJ_INVENT ? you_merc_chained(obj) : FALSE))
#define mon_merc_chained(obj) (!mon_merc_kinstealing(obj) && !mon_merc_streaming(obj))
#define you_merc_chained(obj) (!you_merc_kinstealing(obj) && !you_merc_streaming(obj))
#define force_weapon(otmp)	 ((otmp)->otyp == FORCE_PIKE || \
						  (otmp)->otyp == DOUBLE_FORCE_BLADE || \
						  (otmp)->otyp == FORCE_BLADE || \
						  (otmp)->otyp == FORCE_SWORD || \
						  (otmp)->otyp ==  FORCE_WHIP)
#define pure_weapon(otmp)	 ((otmp)->otyp == WHITE_VIBROSWORD || \
						  (otmp)->otyp == WHITE_VIBROZANBATO || \
						  (otmp)->otyp ==  WHITE_VIBROSPEAR)
#define dark_weapon(otmp)	 ((otmp)->otyp == GOLD_BLADED_VIBROSWORD || \
						  (otmp)->otyp == GOLD_BLADED_VIBROZANBATO || \
						  (otmp)->otyp == GOLD_BLADED_VIBROSPEAR)
#define spec_prop_otyp(otmp)	((pure_weapon(otmp) || dark_weapon(otmp) || force_weapon(otmp)) || \
						  (otmp)->otyp == SUNROD || \
						  (otmp)->otyp == TORCH || \
						  (otmp)->otyp == MAGIC_TORCH || \
						  (otmp)->otyp == SHADOWLANDER_S_TORCH || \
						  (otmp)->otyp == CROW_QUILL || \
						  (otmp)->otyp == SET_OF_CROW_TALONS || \
						  (otmp)->otyp == ISAMUSEI || \
						  (otmp)->otyp == DISKOS || \
						  (otmp)->otyp == PINCER_STAFF || \
						  is_mercy_blade(otmp) || \
						  (otmp)->otyp == KAMEREL_VAJRA)
#define spec_prop_material(otmp)	(otmp->obj_material == MERCURIAL)
#define is_multigen(otmp)	((otmp->oclass == WEAPON_CLASS && \
			 objects[otmp->otyp].oc_skill >= -P_SHURIKEN && \
			 objects[otmp->otyp].oc_skill <= -P_BOW))
// define is_poisonable(otmp)	(otmp->oclass == WEAPON_CLASS && \
			 // objects[otmp->otyp].oc_skill >= -P_SHURIKEN && \
			 // objects[otmp->otyp].oc_skill <= -P_BOW)
//#ifdef FIREARMS
#define is_unpoisonable_firearm_ammo(otmp)	\
			 (is_bullet(otmp) || (otmp)->otyp == STICK_OF_DYNAMITE)
//#else
//#define is_unpoisonable_firearm_ammo(otmp)	0
//#endif
#define is_poisonable(otmp)	(((otmp->oclass == WEAPON_CLASS || is_weptool(otmp)) && \
			!is_launcher(otmp) &&\
			!is_lightsaber(otmp) &&\
			!is_unpoisonable_firearm_ammo(otmp) &&\
			objects[otmp->otyp].oc_dtyp &&\
			objects[otmp->otyp].oc_dtyp != WHACK)\
			|| otmp->otyp == BEARTRAP)
#define uslinging()	(uwep && objects[uwep->otyp].oc_skill == P_SLING)
#define is_bludgeon(otmp)	(otmp->oclass == SPBOOK_CLASS || \
			otmp->oclass == WAND_CLASS || \
			(objects[otmp->otyp].oc_dtyp & WHACK)) //Whack == 1
#define is_stabbing(otmp)	(otmp->oclass != SPBOOK_CLASS && \
			otmp->oclass != WAND_CLASS && \
			(objects[otmp->otyp].oc_dtyp & PIERCE)) //Pierce == 2
#define is_slashing(otmp)	(otmp->oclass != SPBOOK_CLASS && \
			otmp->oclass != WAND_CLASS && \
			(objects[otmp->otyp].oc_dtyp & SLASH)) //Slash == 4
#define is_blasting(otmp)	(otmp->oclass != SPBOOK_CLASS && \
			otmp->oclass != WAND_CLASS && \
			(objects[otmp->otyp].oc_dtyp & EXPLOSION))
//#ifdef FIREARMS
#define is_blaster(otmp) \
			((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].w_ammotyp == WP_BLASTER && \
			 objects[(otmp)->otyp].oc_skill == P_FIREARM)
#define is_firearm(otmp) \
			((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill == P_FIREARM)
#define is_bullet(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill == -P_FIREARM)
//#endif
#define is_monk_weapon(otmp)	(((otmp)->oclass == WEAPON_CLASS && (\
			 (otmp)->otyp == QUARTERSTAFF\
			 || (otmp)->otyp == KHAKKHARA\
			 || (otmp)->otyp == DOUBLE_SWORD\
			 || (otmp)->otyp == CHAKRAM\
			 || (otmp)->otyp == SET_OF_CROW_TALONS\
			 || (otmp)->otyp == NUNCHAKU\
			 || (otmp)->otyp == BESTIAL_CLAW\
			 || (otmp)->otyp == SHURIKEN\
			 || (otmp)->otyp == KATAR\
			 ))\
			 || (otmp)->otyp == WIND_AND_FIRE_WHEELS\
			 )

/* multistriking() is 0-based so that only actual multistriking weapons return multistriking!=0 */
#define multistriking(otmp)	(!(otmp) ? 0 : \
	(otmp)->otyp == SET_OF_CROW_TALONS ? 2 : \
	(otmp)->otyp == VIPERWHIP ? ((otmp)->ovar1_heads - 1) : \
	(((otmp) == uwep || (otmp) == uswapwep) && martial_bonus() && (otmp)->otyp == NUNCHAKU && P_SKILL(P_FLAIL) >= P_EXPERT && P_SKILL(P_BARE_HANDED_COMBAT) >= P_EXPERT) ? 1 : \
	arti_threeHead((otmp)) ? 2 : \
	arti_tentRod((otmp)) ? 6 : \
	0)
/* like multistriking, but all ends always roll attacks. multi_ended() is 0-based so that only actual multi_ended weapons return multi_ended!=0 */
#define multi_ended(otmp)	(!(otmp) ? 0 : \
	(otmp)->otyp == DOUBLE_SWORD ? 1 : \
	((otmp) == uwep && martial_bonus() && (otmp)->otyp == QUARTERSTAFF && P_SKILL(P_QUARTERSTAFF) >= P_EXPERT && P_SKILL(P_BARE_HANDED_COMBAT) >= P_EXPERT && !uarms && !(u.twoweap && uswapwep)) ? 1 : \
	0)
/*  */
#define is_multi_hit(otmp)	(multistriking(otmp) || multi_ended(otmp) || otmp->otyp == STILETTOS || otmp->otyp == WIND_AND_FIRE_WHEELS)
/* if weapon should use unarmed skill */
#define martial_aid(otmp)	((is_lightsaber((otmp)) && !litsaber((otmp)) && (otmp)->otyp != KAMEREL_VAJRA) || \
							(valid_weapon((otmp)) && objects[(otmp)->otyp].oc_skill == P_BARE_HANDED_COMBAT))
/* general "is this a valid weapon" check; there are exceptions to it */
#define valid_weapon(otmp)		((otmp)->oclass == WEAPON_CLASS || \
	is_weptool((otmp)) || \
	(otmp)->otyp == BOULDER || \
	(otmp)->otyp == MASS_OF_STUFF || \
	(otmp)->otyp == HEAVY_IRON_BALL || \
	(otmp)->otyp == CHAIN || \
	(otmp)->oclass == GEM_CLASS)
#define throwing_weapon(otmp)	(is_missile((otmp)) || is_spear((otmp)) || \
	(is_blade((otmp)) && !is_sword((otmp)) && \
	(objects[(otmp)->otyp].oc_dtyp & PIERCE)) || \
	(otmp)->otyp == WAR_HAMMER || (otmp)->otyp == AKLYS || \
	(otmp)->otyp == SICKLE || \
	(otmp)->oartifact == ART_HOUCHOU)

/* Armor */
#define is_shield(otmp) (otmp->oclass == ARMOR_CLASS && \
			 objects[otmp->otyp].oc_armcat == ARM_SHIELD)
#define is_helmet(otmp) (otmp->oclass == ARMOR_CLASS && \
			 objects[otmp->otyp].oc_armcat == ARM_HELM)
#define is_boots(otmp)	(otmp->oclass == ARMOR_CLASS && \
			 objects[otmp->otyp].oc_armcat == ARM_BOOTS)
#define is_gloves(otmp) (otmp->oclass == ARMOR_CLASS && \
			 objects[otmp->otyp].oc_armcat == ARM_GLOVES)
#define is_cloak(otmp)	(otmp->oclass == ARMOR_CLASS && \
			 objects[otmp->otyp].oc_armcat == ARM_CLOAK)
#define is_shirt(otmp)	(otmp->oclass == ARMOR_CLASS && \
			 objects[otmp->otyp].oc_armcat == ARM_SHIRT)
#define is_suit(otmp)	(otmp->oclass == ARMOR_CLASS && \
			 objects[otmp->otyp].oc_armcat == ARM_SUIT)
#define is_harmonium_armor(otmp)	((otmp)->otyp == HARMONIUM_HELM || (otmp)->otyp == HARMONIUM_PLATE\
								|| (otmp)->otyp == HARMONIUM_SCALE_MAIL || (otmp)->otyp == HARMONIUM_GAUNTLETS\
								|| (otmp)->otyp == HARMONIUM_BOOTS)

#define is_light_armor(otmp)	(check_imp_mod(otmp, IEA_MITHRIL) || objects[(otmp)->otyp].oc_dexclass == ARMSZ_LIGHT)
#define is_medium_armor(otmp)	(objects[(otmp)->otyp].oc_dexclass == ARMSZ_MEDIUM)

#define is_elven_armor(otmp)	((otmp)->otyp == ELVEN_HELM\
				|| (otmp)->otyp == HIGH_ELVEN_HELM\
				|| (otmp)->otyp == HIGH_ELVEN_GAUNTLETS\
				|| (otmp)->otyp == HIGH_ELVEN_PLATE\
				|| (otmp)->otyp == ELVEN_MITHRIL_COAT\
				|| (otmp)->otyp == ELVEN_CLOAK\
				|| (otmp)->otyp == ELVEN_SHIELD\
				|| (otmp)->otyp == ELVEN_TOGA\
				|| is_imperial_elven_armor(otmp)\
				|| (otmp)->otyp == ELVEN_BOOTS)
#define is_imperial_elven_armor(otmp)	((otmp)->otyp == IMPERIAL_ELVEN_HELM \
				|| (otmp)->otyp == IMPERIAL_ELVEN_GAUNTLETS \
				|| (otmp)->otyp == IMPERIAL_ELVEN_ARMOR \
				|| (otmp)->otyp == IMPERIAL_ELVEN_BOOTS)
#define carrying_imperial_elven_armor()	(carrying(IMPERIAL_ELVEN_HELM)\
				|| carrying(IMPERIAL_ELVEN_GAUNTLETS)\
				|| carrying(IMPERIAL_ELVEN_ARMOR)\
				|| carrying(IMPERIAL_ELVEN_BOOTS)\
				)
#define helm_upgrade_obj(obj)	(((obj)->otyp == AMULET_OF_MAGICAL_BREATHING \
								|| (obj)->otyp == WAN_DRAINING \
								|| (obj)->otyp == RIN_SEE_INVISIBLE \
								|| (obj)->otyp == HELM_OF_TELEPATHY \
								|| (obj)->otyp == AMULET_OF_ESP \
								|| (obj)->otyp == CRYSTAL_HELM \
								|| (obj)->otyp == RIN_INCREASE_ACCURACY \
								|| (obj)->otyp == RIN_TELEPORT_CONTROL \
								|| (obj)->otyp == RIN_PROTECTION_FROM_SHAPE_CHAN \
								) && objects[(obj)->otyp].oc_name_known)
#define gauntlets_upgrade_obj(obj)	(((obj)->otyp ==  WATER_WALKING_BOOTS\
								|| (obj)->otyp == GAUNTLETS_OF_POWER \
								|| (obj)->otyp == GAUNTLETS_OF_DEXTERITY \
								|| (obj)->otyp == RIN_INCREASE_DAMAGE \
								|| (obj)->otyp == WAN_MAGIC_MISSILE \
								|| (obj)->otyp == AMULET_OF_STRANGULATION \
								) && objects[(obj)->otyp].oc_name_known)
#define armor_upgrade_obj(obj)	(((obj)->otyp == FLYING_BOOTS \
								|| (obj)->otyp == RIN_SUSTAIN_ABILITY \
								|| (obj)->otyp == RIN_REGENERATION \
								|| (obj)->otyp == AMULET_OF_WOUND_CLOSURE \
								|| (obj)->otyp == AMULET_OF_REFLECTION \
								|| (obj)->otyp == SHIELD_OF_REFLECTION \
								|| (obj)->otyp == JUMPING_BOOTS \
								|| (obj)->otyp == AMULET_VERSUS_SICKNESS \
								|| (obj)->otyp == HEALER_UNIFORM \
								|| (obj)->otyp == CLOAK_OF_PROTECTION \
								|| (obj)->otyp == CLOAK_OF_MAGIC_RESISTANCE \
								|| (obj)->otyp == ORIHALCYON_GAUNTLETS \
								|| (obj)->otyp == CLOAK_OF_DISPLACEMENT \
								|| (obj)->otyp == CLOAK_OF_INVISIBILITY \
								|| (obj)->otyp == RIN_INVISIBILITY \
								|| (obj)->otyp == WAN_MAKE_INVISIBLE \
								|| (obj)->otyp == RIN_PROTECTION \
								|| (obj)->otyp == ELVEN_MITHRIL_COAT \
								) && objects[(obj)->otyp].oc_name_known)
#define boots_upgrade_obj(obj)	(((obj)->otyp == JUMPING_BOOTS \
								|| (obj)->otyp == SPEED_BOOTS \
								|| (obj)->otyp == KICKING_BOOTS \
								|| (obj)->otyp == WAN_SPEED_MONSTER \
								|| (obj)->otyp == RIN_ALACRITY \
								|| (obj)->otyp == WAN_TELEPORTATION \
								|| (obj)->otyp == RIN_TELEPORTATION \
								) && objects[(obj)->otyp].oc_name_known)

#define is_orcish_armor(otmp)	((otmp)->otyp == ORCISH_HELM\
				|| (otmp)->otyp == ORCISH_CHAIN_MAIL\
				|| (otmp)->otyp == ORCISH_RING_MAIL\
				|| (otmp)->otyp == ORCISH_CLOAK\
				|| (otmp)->otyp == URUK_HAI_SHIELD\
				|| (otmp)->otyp == ORCISH_SHIELD)
#define is_dwarvish_armor(otmp)	((otmp)->otyp == DWARVISH_HELM\
				|| (otmp)->otyp == DWARVISH_MITHRIL_COAT\
				|| (otmp)->otyp == DWARVISH_CLOAK\
				|| (otmp)->otyp == DWARVISH_ROUNDSHIELD)
#define is_gnomish_armor(otmp)	((otmp)->otyp == GNOMISH_POINTY_HAT)

#define is_wide_helm(otmp)		((otmp)->otyp == SEDGE_HAT \
				|| (otmp)->otyp == WAR_HAT \
				|| (otmp)->otyp == WIDE_HAT \
				|| (otmp)->otyp == WITCH_HAT )

#define is_plusten(otmp)	(arti_plusten(otmp)\
								|| is_rakuyo(otmp)\
								|| rakuyo_prop(otmp)\
								|| is_mercy_blade(otmp)\
								|| otmp->otyp == ISAMUSEI\
								|| otmp->otyp == BESTIAL_CLAW\
								|| check_oprop(otmp, OPROP_CCLAW)\
								)
#define is_plussev_armor(otmp)	(is_elven_armor((otmp))\
								|| arti_plussev((otmp))\
								|| ((otmp)->otyp == CORNUTHAUM && Role_if(PM_WIZARD))\
								|| ((otmp)->otyp == ROBE && Role_if(PM_WIZARD) && (otmp)->oartifact == ART_ROBE_OF_THE_ARCHMAGI)\
								|| (otmp)->otyp == CRYSTAL_HELM\
								|| (otmp)->otyp == CRYSTAL_PLATE_MAIL\
								|| (otmp)->otyp == CRYSTAL_SHIELD\
								|| (otmp)->otyp == CRYSTAL_GAUNTLETS\
								|| (otmp)->otyp == CRYSTAL_BOOTS\
								|| (otmp)->otyp == CLOAK_OF_PROTECTION)

#define is_twoweapable_artifact(otmp) (always_twoweapable_artifact(otmp)\
				|| ((otmp)->oartifact == ART_CLARENT && uwep && uwep->oartifact==ART_EXCALIBUR)\
				|| ((otmp)->oartifact == ART_FROST_BRAND && uwep && uwep->oartifact==ART_FIRE_BRAND)\
				|| ((otmp)->oartifact == ART_FIRE_BRAND && uwep && uwep->oartifact==ART_FROST_BRAND)\
				|| ((otmp)->oartifact == ART_BLADE_DANCER_S_DAGGER && uwep && uwep->oartifact==ART_BLADE_SINGER_S_SABER)\
				|| ((otmp)->oartifact == ART_BLADE_DANCER_S_DAGGER && uwep && uwep->oartifact==ART_SODE_NO_SHIRAYUKI)\
				|| ((otmp)->oartifact == ART_FRIEDE_S_SCYTHE && uwep && is_farm(uwep))\
				|| ((otmp)->oartifact == ART_FLUORITE_OCTAHEDRON && uwep && uwep->oartifact==ART_FLUORITE_OCTAHEDRON)\
				|| ((otmp)->oartifact == ART_MJOLLNIR && Role_if(PM_VALKYRIE))\
				|| ((otmp)->oartifact == ART_CLEAVER && Role_if(PM_BARBARIAN))\
				|| ((otmp)->oartifact == ART_ATLANTEAN_ROYAL_SWORD && Role_if(PM_BARBARIAN))\
				|| ((otmp)->oartifact == ART_TECPATL_OF_HUHETOTL && Role_if(PM_ARCHEOLOGIST))\
				|| ((otmp)->oartifact == ART_KIKU_ICHIMONJI && Role_if(PM_SAMURAI))\
				|| ((otmp)->oartifact == ART_JINJA_NAGINATA && Role_if(PM_SAMURAI))\
				|| ((otmp)->oartifact == ART_SNICKERSNEE && (Role_if(PM_SAMURAI) || Role_if(PM_TOURIST) ))\
				|| ((otmp)->oartifact == ART_MAGICBANE && Role_if(PM_WIZARD)))
				
#define always_twoweapable_artifact(otmp) ((otmp)->oartifact == ART_STING\
				|| (otmp)->oartifact == ART_ORCRIST\
				|| (otmp)->oartifact == ART_GRIMTOOTH\
				|| (otmp)->oartifact == ART_CARNWENNAN\
				|| (otmp)->oartifact == ART_CLAIDEAMH\
				|| (otmp)->oartifact == ART_DRAGONLANCE\
				|| (otmp)->oartifact == ART_DEMONBANE\
				|| (otmp)->oartifact == ART_NODENSFORK\
				|| (otmp)->oartifact == ART_GAIA_S_FATE\
				|| (otmp)->oartifact == ART_WEREBANE\
				|| (otmp)->oartifact == ART_GIANTSLAYER\
				|| (otmp)->oartifact == ART_VAMPIRE_KILLER\
				|| (otmp)->oartifact == ART_KINGSLAYER\
				|| (otmp)->oartifact == ART_PEACE_KEEPER\
				|| (otmp)->oartifact == ART_OGRESMASHER\
				|| (otmp)->oartifact == ART_TROLLSBANE\
				|| (otmp)->oartifact == ART_PEN_OF_THE_VOID)
				
/* Eggs and other food */
#define MAX_EGG_HATCH_TIME 200	/* longest an egg can remain unhatched */
#define stale_egg(egg)	((monstermoves - (egg)->age) > (2*MAX_EGG_HATCH_TIME))
#define ofood(o) ((o)->otyp == CORPSE || (o)->otyp == EGG || (o)->otyp == TIN || (o)->otyp == POT_BLOOD)
#define polyfodder(obj) (ofood(obj) && \
			 pm_to_cham((obj)->corpsenm) != CHAM_ORDINARY)
#define mlevelgain(obj) (ofood(obj) && (obj)->corpsenm == PM_WRAITH)
#define mhealup(obj)	(ofood(obj) && (obj)->corpsenm == PM_NURSE)
#define drainlevel(corpse) (mons[(corpse)->corpsenm].cnutrit*4/5)

/* Containers */
#define carried(o)	((o)->where == OBJ_INVENT)
#define mcarried(o)	((o)->where == OBJ_MINVENT)
#define Has_contents(o) (/* (Is_container(o) || (o)->otyp == STATUE) && */ \
			 (o)->cobj != (struct obj *)0)
#define Is_container(o) ((o)->otyp >= BOX && (o)->otyp <= BAG_OF_TRICKS/*DISTRESSED_PRINCESS*/)
#define Is_real_container(o) ((o)->otyp >= BOX && (o)->otyp < BAG_OF_TRICKS)
#define Is_box(otmp)	(otmp->otyp == BOX || otmp->otyp == SARCOPHAGUS || otmp->otyp == CHEST || otmp->otyp == MAGIC_CHEST)
#define Is_mbag(otmp)	(otmp->otyp == BAG_OF_HOLDING || \
			 otmp->otyp == BAG_OF_TRICKS)

/* dragon gear */
#define Is_dragon_scales(obj)	((obj)->otyp >= GRAY_DRAGON_SCALES && \
				 (obj)->otyp <= YELLOW_DRAGON_SCALES)
#define Is_dragon_mail(obj)	((obj)->otyp >= GRAY_DRAGON_SCALE_MAIL && \
				 (obj)->otyp <= YELLOW_DRAGON_SCALE_MAIL)
#define Is_dragon_shield(obj) ((obj)->otyp >= GRAY_DRAGON_SCALE_SHIELD && \
							   (obj)->otyp <= YELLOW_DRAGON_SCALE_SHIELD)
#define Is_dragon_armor(obj)	(Is_dragon_scales(obj) || Is_dragon_mail(obj) || Is_dragon_shield(obj))
#define Dragon_scales_to_pm(obj) &mons[PM_GRAY_DRAGON + (obj)->otyp \
				       - GRAY_DRAGON_SCALES]
#define Dragon_mail_to_pm(obj)	&mons[PM_GRAY_DRAGON + (obj)->otyp \
				      - GRAY_DRAGON_SCALE_MAIL]
#define Dragon_shield_to_pm(obj)	&mons[PM_GRAY_DRAGON + (obj)->otyp \
				      - GRAY_DRAGON_SCALE_SHIELD]
#define Dragon_to_scales(pm)	(GRAY_DRAGON_SCALES + (pm - mons))

#define Dragon_armor_to_pm(obj) (\
	Is_dragon_shield((obj)) ? Dragon_shield_to_pm((obj)) : \
	Is_dragon_mail((obj)) ? Dragon_mail_to_pm((obj)) : \
	Is_dragon_scales((obj)) ? Dragon_scales_to_pm((obj)) : (struct permonst*)0)

#define Dragon_armor_matches_mtyp(obj, mtyp) (\
	(Dragon_armor_to_pm((obj)) == &mons[(mtyp)]) || \
	((obj)->oartifact == ART_DRAGON_PLATE && ( \
		(mtyp) == PM_SILVER_DRAGON || \
		(mtyp) == PM_BLACK_DRAGON || \
		(mtyp) == PM_BLUE_DRAGON || \
		(mtyp) == PM_RED_DRAGON || \
		(mtyp) == PM_WHITE_DRAGON || \
		(mtyp) == PM_GRAY_DRAGON || \
		(mtyp) == PM_ORANGE_DRAGON \
	)) || \
	((obj)->oartifact == ART_CHROMATIC_DRAGON_SCALES && (\
		(mtyp) == PM_BLACK_DRAGON || \
		(mtyp) == PM_RED_DRAGON || \
		(mtyp) == PM_BLUE_DRAGON || \
		(mtyp) == PM_WHITE_DRAGON || \
		(mtyp) == PM_YELLOW_DRAGON || \
		(mtyp) == PM_GREEN_DRAGON \
	)) || \
	((obj)->oartifact == ART_STEEL_SCALES_OF_KURTULMAK && (\
		(mtyp) == PM_GRAY_DRAGON || \
		(mtyp) == PM_RED_DRAGON \
	)))

/* Elven gear */
#define is_elven_weapon(otmp)	((otmp)->otyp == ELVEN_ARROW\
				|| (otmp)->otyp == ELVEN_SPEAR\
				|| (otmp)->otyp == ELVEN_DAGGER\
				|| (otmp)->otyp == ELVEN_SHORT_SWORD\
				|| (otmp)->otyp == ELVEN_BROADSWORD\
				|| (otmp)->otyp == ELVEN_LANCE\
				|| (otmp)->otyp == ELVEN_BOW)
#define is_elven_obj(otmp)	(is_elven_armor(otmp) || is_elven_weapon(otmp))

/* Droven gear */
#define is_droven_weapon(otmp) ((otmp)->otyp == DROVEN_BOLT\
				|| (otmp)->otyp == DROVEN_CROSSBOW\
				|| (otmp)->otyp == DROVEN_SPEAR\
				|| (otmp)->otyp == DROVEN_DAGGER\
				|| (otmp)->otyp == DROVEN_SHORT_SWORD\
				|| (otmp)->otyp == DROVEN_GREATSWORD\
				|| (otmp)->otyp == DROVEN_LANCE\
				|| (otmp)->otyp == VIPERWHIP)
#define is_droven_obj(otmp)	(is_droven_armor(otmp) || is_droven_weapon(otmp))


/* Orcish gear */
#define is_orcish_obj(otmp)	(is_orcish_armor(otmp)\
				|| (otmp)->otyp == ORCISH_ARROW\
				|| (otmp)->otyp == ORCISH_SPEAR\
				|| (otmp)->otyp == ORCISH_DAGGER\
				|| (otmp)->otyp == ORCISH_SHORT_SWORD\
				|| (otmp)->otyp == ORCISH_BOW)

/* Dwarvish gear */
#define is_dwarvish_obj(otmp)	(is_dwarvish_armor(otmp)\
				|| (otmp)->otyp == DWARVISH_SPEAR\
				|| (otmp)->otyp == DWARVISH_SHORT_SWORD\
				|| (otmp)->otyp == DWARVISH_MATTOCK)

/* Gnomish gear */
#define is_gnomish_obj(otmp)	(is_gnomish_armor(otmp))

/* Light sources */
#define Is_candle(otmp) (otmp->otyp == TALLOW_CANDLE || \
			 otmp->otyp == WAX_CANDLE || \
			 otmp->otyp == CANDLE_OF_INVOCATION)

#define Is_torch(otmp) (otmp->otyp == TORCH || \
			 otmp->otyp == MAGIC_TORCH || \
			 otmp->otyp == SHADOWLANDER_S_TORCH || \
			 otmp->otyp == SUNROD)


#define MAX_OIL_IN_FLASK 400	/* maximum amount of oil in a potion of oil */
#define Is_darklight_source(otmp) ((otmp)->otyp == SHADOWLANDER_S_TORCH || \
			 (otmp)->otyp == CHUNK_OF_FOSSIL_DARK ||\
			 ((otmp)->otyp == DWARVISH_HELM && (otmp)->ovar1_darklight) ||\
			 (is_lightsaber(otmp) && otmp->cobj && (otmp->cobj->otyp == CHUNK_OF_FOSSIL_DARK || otmp->cobj->otyp == ANTIMAGIC_RIFT)))
// NOT an exhaustive list, but this SHOULD be everything that would fall under snuff_lit
// and shouldn't be put out by darkness spells
#define Darkness_cant_snuff(otmp) (Is_darklight_source(otmp) || (otmp->otyp == SUNLIGHT_MAGGOT) || \
			(is_lightsaber(otmp) && (otmp->oartifact == ART_ARKENSTONE || otmp->oartifact == ART_GLITTERSTONE)) || \
			(otmp->otyp == SUNROD))

/* MAGIC_LAMP intentionally excluded below */
/* age field of this is relative age rather than absolute */
#define age_is_relative(otmp)	((otmp)->otyp == LANTERN\
				|| (otmp)->otyp == OIL_LAMP\
				|| (otmp)->otyp == DWARVISH_HELM\
				|| (otmp)->otyp == LANTERN_PLATE_MAIL\
				|| (otmp)->otyp == LIGHTSABER\
				|| (otmp)->otyp == BEAMSWORD\
				|| (otmp)->otyp == DOUBLE_LIGHTSABER\
				|| (otmp)->otyp == ROD_OF_FORCE\
				|| (otmp)->otyp == GNOMISH_POINTY_HAT\
				|| (otmp)->otyp == CANDELABRUM_OF_INVOCATION\
				|| (otmp)->otyp == TALLOW_CANDLE\
				|| (otmp)->otyp == WAX_CANDLE\
				|| (otmp)->otyp == POT_OIL)
/* object can be ignited -- lightsources that should be inadvertantly lit by fire damage */
#define ignitable(otmp)	(\
				   (otmp)->otyp == OIL_LAMP\
				|| (otmp)->otyp == MAGIC_LAMP\
 				|| (otmp)->otyp == GNOMISH_POINTY_HAT\
				|| (otmp)->otyp == CANDELABRUM_OF_INVOCATION\
				|| (otmp)->otyp == TALLOW_CANDLE\
				|| (otmp)->otyp == WAX_CANDLE\
				|| (otmp)->otyp == TORCH\
				|| (otmp)->otyp == MAGIC_TORCH\
				|| (otmp)->otyp == SHADOWLANDER_S_TORCH\
				|| (otmp)->otyp == CANDLE_OF_INVOCATION\
				|| (otmp)->otyp == POT_OIL)

/* special stones */
#define is_graystone(obj)	((obj)->otyp == LUCKSTONE || \
				 (obj)->otyp == LOADSTONE || \
				 (obj)->otyp == FLINT     || \
				 (obj)->otyp == TOUCHSTONE || \
				 (obj)->otyp == VITAL_SOULSTONE || \
				 (obj)->otyp == SPIRITUAL_SOULSTONE)

/* spirit related */
#define is_berithable(otmp)	(otmp->otyp == SADDLE\
				|| otmp->otyp == SABER\
				|| otmp->otyp == LONG_SWORD\
				|| otmp->otyp == BOW\
				|| otmp->otyp == LANCE\
				|| (OBJ_DESCR(objects[otmp->otyp]) != (char *)0 && !strncmp(OBJ_DESCR(objects[otmp->otyp]), "riding ", 7))\
				)
#define is_chupodible(otmp) ((otmp)->otyp == CORPSE && your_race(&mons[(otmp)->corpsenm]))

#define is_unholy(otmp)		((otmp)->oartifact == ART_STORMBRINGER || (otmp)->cursed)

/* material */
#define is_flimsy(otmp)		((otmp)->obj_material <= LEATHER)
#define is_wood(otmp)		((otmp)->obj_material == WOOD)
#define is_veggy(otmp)		((otmp)->obj_material == VEGGY)
#define is_flesh(otmp)		((otmp)->obj_material == FLESH)
#define is_paper(otmp)		((otmp)->obj_material == PAPER)
/* Note: use is_metallic() */

/* misc */
#define is_boulder(otmp)		((otmp)->otyp == BOULDER || (otmp)->otyp == MASSIVE_STONE_CRATE || (otmp)->otyp == MASS_OF_STUFF || ((otmp)->otyp == STATUE && opaque(&mons[(otmp)->corpsenm])))

#define is_dress(onum)		(onum == NOBLE_S_DRESS || onum == GENTLEWOMAN_S_DRESS || onum == PLAIN_DRESS || onum == VICTORIAN_UNDERWEAR)

#define is_hat(otmp) (otmp->otyp == SEDGE_HAT \
			 || otmp->otyp == WIDE_HAT \
			 || otmp->otyp == GNOMISH_POINTY_HAT \
			 || otmp->otyp == FEDORA \
			 || otmp->otyp == CORNUTHAUM \
			 || otmp->otyp == WITCH_HAT \
			 || otmp->otyp == DUNCE_CAP \
			 || otmp->otyp == WAR_HAT \
			 || otmp->otyp == SHEMAGH \
			 || otmp->otyp == find_gcirclet() \
			)
#define arm_blocks_upper_body(onum)		(objects[onum].oc_dtyp&UPPER_TORSO_DR)
#define arm_blocks_lower_body(onum)		(objects[onum].oc_dtyp&LOWER_TORSO_DR)

#define is_magic_obj(obj)		(objects[(obj)->otyp].oc_magic \
								|| ((is_enchantable(obj)) && (obj)->spe > 0)\
								|| ((obj)->oartifact)\
								)

#define is_cha_otyp(onum)	(onum == NOBLE_S_DRESS\
							|| onum == GENTLEWOMAN_S_DRESS\
							|| onum == PLAIN_DRESS\
							|| onum == VICTORIAN_UNDERWEAR\
							|| onum == SMOKY_VIOLET_FACELESS_ROBE\
							|| onum == CONSORT_S_SUIT\
							|| onum == GENTLEMAN_S_SUIT\
							|| onum == find_gcirclet())

#define is_readable_armor_otyp(onum)	(onum == NOBLE_S_DRESS\
							  || onum == DROVEN_PLATE_MAIL\
							  || onum == DROVEN_CHAIN_MAIL\
							  || onum == CONSORT_S_SUIT\
							)

#define is_readable_armor(obj)	((obj)->oward)

#define is_museable_amulet(otyp) (otyp == AMULET_OF_LIFE_SAVING \
								||otyp == AMULET_OF_REFLECTION \
								||otyp == AMULET_OF_NULLIFY_MAGIC \
								||otyp == AMULET_OF_MAGICAL_BREATHING \
								||otyp == AMULET_OF_DRAIN_RESISTANCE \
								||otyp == AMULET_VERSUS_CURSES \
								||otyp == AMULET_OF_ESP \
								||otyp == AMULET_VERSUS_POISON \
								||otyp == AMULET_OF_WOUND_CLOSURE \
								||otyp == AMULET_VERSUS_EVIL_EYES \
								)

/* helpers, simple enough to be macros */
#define is_plural(o)	((o)->quan > 1 || \
			 (o)->oartifact == ART_EYES_OF_THE_OVERWORLD)

#define salve_target(otmp)	(\
			((otmp->oclass == ARMOR_CLASS || otmp->oclass == WEAPON_CLASS || is_weptool(otmp)) && otmp->spe < 0)\
			|| otmp->oeroded > 0\
			|| otmp->oeroded2 > 0\
			|| otmp->oeroded3 > 0\
			)

/* Potentially required for ascension, shouldn't fail resist checks or be put in containers. */
#define is_asc_obj(obj)	((obj)->otyp == AMULET_OF_YENDOR || \
		   (obj)->otyp == CANDELABRUM_OF_INVOCATION || \
		   (obj)->otyp == BELL_OF_OPENING || \
		   (obj)->oartifact == ART_SILVER_KEY || \
		   ((obj)->oartifact >= ART_FIRST_KEY_OF_LAW && (obj)->oartifact <= ART_THIRD_KEY_OF_NEUTRALITY) || \
		   (obj)->oartifact == ART_PEN_OF_THE_VOID || \
		   (obj)->oartifact == ART_ANNULUS || \
		   (obj)->otyp == SPE_BOOK_OF_THE_DEAD \
		)

/* Flags for get_obj_location(). */
#define CONTAINED_TOO	0x1
#define BURIED_TOO	0x2
#define INTRAP_TOO	0x4

#define higher_depth(armdepth, depth)	(armdepth == depth || (\
		(depth&(W_ARMC|W_GLYPH)) ? FALSE :\
		(depth&(W_ARMS|W_WEP|W_QUIVER|W_SWAPWEP|W_AMUL|W_SADDLE|W_CHAIN)) ? (armdepth == W_ARMC) :\
		(depth&(W_ARMH|W_ARMG|W_ARMF|W_ARM|W_RINGL|W_RINGR|W_TOOL)) ? (armdepth != W_ARMU) :\
		(depth&(W_ARMU|W_SKIN)) ? TRUE :\
		FALSE))

#define PURIFIED_CHAOS	(mvitals[PM_CHAOS].died)
#define PURIFIED_EARTH	(mvitals[PM_CHAOS].died || mvitals[PM_LICH__THE_FIEND_OF_EARTH].died)
#define PURIFIED_FIRE	(mvitals[PM_CHAOS].died || mvitals[PM_KARY__THE_FIEND_OF_FIRE].died)
#define PURIFIED_WATER	(mvitals[PM_CHAOS].died || mvitals[PM_KRAKEN__THE_FIEND_OF_WATER].died)
#define PURIFIED_WIND	(mvitals[PM_CHAOS].died || mvitals[PM_TIAMAT__THE_FIEND_OF_WIND].died)

#endif /* OBJ_H */
