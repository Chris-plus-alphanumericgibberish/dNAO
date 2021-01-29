/*	SCCS Id: @(#)crown.c	3.4	2003/02/03	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */


#include "hack.h"
#include "artifact.h"

STATIC_DCL int NDECL(choose_crowning);

struct crowning {
	int crowninggift;			/* artifact */
	const char * title;			/* title displayed in enlightenment and dumplog */
	const char * announcement;	/* godvoice() announcement -- if godvoice is not used, 0 and special-cased */
	const char * livelogstr;	/* livelog string */
	int title_mod;				/* what should %s in the title be filled with? */
#define CRWN_TTL_LORD	1	/* Lord / Lady */
#define CRWN_TTL_KING	2	/* King / Queen */
#define CRWN_TTL_PRIE	3	/* Priest / Priestess */
#define CRWN_TTL_NAME	4	/* playername */
};

/* helpers to announce your crowning */
#define chosen(rest)		"Thou art chosen to " rest "!"
#define verb_thee_the(verb)	"I " verb " thee...  The %s!"
#define verb_thee(verb)		"I " verb " thee...  %s!"
#define dub_thee_the		verb_thee_the("dub")
#define dub_thee			verb_thee("dub")

/* helpers to livelog your crowning */
#define became_the			"became the %s"

/* NOTE: the numbering of these must be consistent with choose_crowning() */
static const struct crowning hand_of_elbereth[] = {
	/* dummy so all non-zero are interesting */
/*00*/	{0},
	/* Default */
/*01*/	{ART_EXCALIBUR,					"the Arm of the Law",					dub_thee_the,							became_the	},
/*02*/	{ART_VORPAL_BLADE,				"the Envoy of Balance",					"Thou shalt be my %s!",					became_the	},
/*03*/	{ART_STORMBRINGER,				"the Glory of Arioch",					chosen("steal souls for My Glory"),		became_the	},
	/* Monk */
/*04*/	{ART_GRANDMASTER_S_ROBE,		"the Sage of Law",						dub_thee_the,							became_the	},
/*05*/	{ART_GRANDMASTER_S_ROBE,		"the Grandmaster of Balance",			"Thou shalt be the %s!",				became_the	},
/*06*/	{ART_ROBE_OF_THE_ARCHMAGI,		"the Glory of Eequor",					chosen("cause dismay in My Name"),		became_the	},
	/* Noble (human, vampire, incant). Vampires always get Dark Lord regardless of alignment */
/*07*/	{ART_CROWN_OF_THE_SAINT_KING,	"the Saint %s",							verb_thee_the("crown"),					"received the crown of the Saint King",	CRWN_TTL_KING	},
/*08*/	{ART_CROWN_OF_THE_SAINT_KING,	"the Grey Saint",						verb_thee_the("crown"),					"received the crown of the Saint King"	},
/*09*/	{ART_HELM_OF_THE_DARK_LORD,		"the Dark %s",							verb_thee_the("crown"),					"received the helm of the Dark Lord",	CRWN_TTL_LORD	},
	/* Wizard. Artifact is replaced by Book of Infinite Spells if Necronomicon already exists. */
/*10*/	{ART_NECRONOMICON,				"the Magister of Law",					dub_thee_the,							became_the	},
/*11*/	{ART_NECRONOMICON,				"the Wizard of Balance",				"Thou shalt be the %s!",				became_the	},
/*12*/	{ART_NECRONOMICON,				"the Glory of Chardros",				chosen("take lives for My Glory"),		became_the	},
	/* Elf */
/*13*/	{ART_ARCOR_KERYM,				"the Hand of Elbereth",					verb_thee_the("crown"),					became_the	},
/*14*/	{ART_ARYFAERN_KERYM,			"the Doomspeaker of Vaire",				dub_thee_the,							became_the	},
/*15*/	{ART_ARYVELAHR_KERYM,			"the Whisperer of Este",				dub_thee_the,							became_the	},
	/* Drow */
/*16*/	{ART_SICKLE_MOON,				"the Hand of Eilistraee",				dub_thee_the,							became_the	},
/*17*/	{ART_CLAWS_OF_THE_REVENANCER,	"the Hand of Kiaransali",				dub_thee_the,							became_the	},
/*18*/	{ART_WEB_OF_LOLTH,				"the Hand of Lolth",					verb_thee_the("crown"),					became_the	},
	/* Hedrow */
/*19*/	{0,								"the Shepherd of spiders",				(const char *)0,						"became the Shepherd of the Black Web"	},
/*20*/	{ART_LOLTH_S_FANG,				"the Sword of Vhaeraun",				dub_thee_the,							became_the	},
/*21*/	{ART_LOLTH_S_FANG,				"the Fang of Lolth",					dub_thee_the,							became_the	},
	/* Drow Noble */
/*22*/	{ART_LIECLEAVER,				"the Blade of Ver'tas",					dub_thee_the,							became_the	},
/*23*/	{ART_CLAWS_OF_THE_REVENANCER,	"the Hand of Kiaransali",				dub_thee_the,							became_the	},
/*24*/	{ART_WEB_OF_LOLTH,				"the Hand of Lolth",					verb_thee_the("crown"),					became_the	},
	/* Hedrow Noble */
/*25*/	{ART_LIECLEAVER,				"the Blade of Ver'tas",					dub_thee_the,							became_the	},
/*26*/	{ART_LOLTH_S_FANG,				"the Hand of Keptolo",					dub_thee_the,							became_the	},
/*27*/	{ART_RUINOUS_DESCENT_OF_STARS,	"the Hammer of Ghaunadaur",				dub_thee_the,							became_the	},
	/* Ranger */
/*28*/	{ART_SUNBEAM,					"the High %s of Apollo",				verb_thee_the("anoint"),				"anointed by Apollo",	CRWN_TTL_PRIE},
/*29*/	{ART_VEIL_OF_LATONA,			"the High %s of Latona",				verb_thee_the("anoint"),				"anointed by Latona",	CRWN_TTL_PRIE},
/*30*/	{ART_MOONBEAM,					"the High %s of Diana",					verb_thee_the("anoint"),				"anointed by Diana",	CRWN_TTL_PRIE},
	/* Gnome Ranger */
/*31*/	{ART_STEEL_SCALES_OF_KURTULMAK,	"the Great Slave-Vassal of Kurtulmak",	verb_thee("claim"),						"claimed by Kurtulmak"	},
/*32*/	{ART_GLITTERSTONE,				"the Thane of Garl Glittergold",		dub_thee,								became_the	},
/*33*/	{ART_GREAT_CLAWS_OF_URDLEN,		"the Claw of Urdlen",					chosen("rend the Earth in My Name"),	"chosen by Urdlen"	},
	/* Healer */
/*34*/	{ART_AEGIS,						"the Arm of Athena",					dub_thee_the,							became_the	},
/*35*/	{ART_HERMES_S_SANDALS,			"the Messenger of Hermes",				dub_thee,								became_the	},
/*36*/	{ART_POSEIDON_S_TRIDENT,		"the Glory of Poseidon",				dub_thee,								became_the	},
	/*Archeologist*/ /* law -- warrior or high priest? */
/*37*/	{ART_EHECAILACOCOZCATL,			"the Warrior of Quetzalcoatl",			verb_thee_the("proclaim"),				became_the	},
/*38*/	{ART_AMHIMITL,					"the Champion of Camaxtli",				verb_thee_the("proclaim"),				became_the	},
/*39*/	{ART_TECPATL_OF_HUHETOTL,		"the Fire-bearer of Huhetotl",			dub_thee_the,							became_the	},
	/*Female Half Dragon Noble*/
/*40*/	{ART_DRAGONHEAD_SHIELD,			"the Dragon-slayer of Gwyn",			dub_thee_the,							became_the	},
/*41*/	{ART_CRUCIFIX_OF_THE_MAD_KING,	"the Guardian of the Old Lords",		dub_thee_the,							became_the	},
/*42*/	{ART_RINGED_BRASS_ARMOR,		"the Darkmoon Champion",				dub_thee_the,							became_the	},
	/* Knight -- lawful only */
/*43*/	{ART_CLARENT,					"the King of the Angles",				dub_thee,								"crowned %s"	},
	/* Pirate -- all alignments */
/*44*/	{ART_REAVER,					"the Pirate King",						(const char *)0,						became_the	},
	/* Valkyrie -- all alignments, female only */
/*45*/	{ART_BOW_OF_SKADI,				"the Daughter of Skadi",				"I greet you, my daughter.",			"greeted as a daughter of Skadi"	},
	/* Binder */
/*46*/	{0,								"the Emissary of Elements",				(const char *)0,						became_the	},
	/* Dwarf Knight -- all alignments */
/*47*/	{ART_ARKENSTONE,				"the King under the Mountain",			"Hail, %s!",							became_the	},
	/* Dwarf Noble -- all alignments */
/*48*/	{ART_DURIN_S_AXE,				"the Lord of Moria",					"Hail, %s!",							became_the	},
	/* Samurai -- lawful only */
/*49*/	{ART_YOICHI_NO_YUMI,			"Nasu no %s",							verb_thee("proclaim"),					became_the,	CRWN_TTL_NAME	}
};

#undef chosen
#undef verb_thee_the
#undef verb_thee
#undef dub_thee_the
#undef dub_thee
#undef became_the

/* returns the appropriate index of hand_of_elbereth[] for your character */
int
choose_crowning()
{
	int align_offset = 1-(u.ualign.type);

	/* binder */
	if (Role_if(PM_EXILE))
		return 46;
	/* alignment-spanning */
	if (Pantheon_if(PM_PIRATE) || Role_if(PM_PIRATE))
		return 44;
	if((Pantheon_if(PM_VALKYRIE) || Role_if(PM_VALKYRIE)) && flags.initgend)
		return 45;
	if (Race_if(PM_DWARF) && Role_if(PM_KNIGHT))
		return 47;
	if (Race_if(PM_DWARF) && Role_if(PM_NOBLEMAN))
		return 48;
	/* racial */
	if (Race_if(PM_DROW))
		return 16 + align_offset + 3*(!flags.initgend) + 6*(Role_if(PM_NOBLEMAN));
	if (Race_if(PM_ELF))
		return 13 + align_offset;
	/* non-racial ranger */
	if((Race_if(PM_HUMAN) || Race_if(PM_INHERITOR) || Race_if(PM_INCANTIFIER) || Race_if(PM_HALF_DRAGON)) && (Pantheon_if(PM_RANGER) || Role_if(PM_RANGER)))
		return 28 + align_offset;
	/* gnome ranger */
	if (Race_if(PM_GNOME) && Role_if(PM_RANGER))
		return 31 + align_offset;
	/* female half-dragon noble */
	if (Race_if(PM_HALF_DRAGON) && Role_if(PM_NOBLEMAN) && flags.initgend)
		return 40 + align_offset;
	/* vampire noble */
	if (Race_if(PM_VAMPIRE) && (Pantheon_if(PM_NOBLEMAN) || Role_if(PM_NOBLEMAN)))
		return 9;
	/* non-vampire noble */
	if (Pantheon_if(PM_NOBLEMAN) || Role_if(PM_NOBLEMAN))
		return 7 + align_offset;
	/* lawful knight */
	if ((Pantheon_if(PM_KNIGHT) || Role_if(PM_KNIGHT)) && u.ualign.type == A_LAWFUL)
		return 43;
	/* lawful samurai */
	if ((Pantheon_if(PM_SAMURAI) || Role_if(PM_SAMURAI)) && u.ualign.type == A_LAWFUL)
		return 49;
	/* monk */
	if (Pantheon_if(PM_MONK) || Role_if(PM_MONK))
		return 4 + align_offset;
	/* wizard */
	if (Pantheon_if(PM_WIZARD) || Role_if(PM_WIZARD))
		return 10 + align_offset;
	/* healer */
	if (Pantheon_if(PM_HEALER) || Role_if(PM_HEALER))
		return 34 + align_offset;
	/* archeologist */
	if (Pantheon_if(PM_ARCHEOLOGIST) || Role_if(PM_ARCHEOLOGIST))
		return 37 + align_offset;
	/* default */
	return 1 + align_offset;
}

const char *
crowning_title()
{
	if (!u.uevent.uhand_of_elbereth) {
		impossible("called crowning_title() without being crowned?");
		return "no one special";
	}

	static char titlebuf[BUFSZ];	/* shared buffer should be safe -- we should only ever call this function with one unchanging input to get one unchanging output */
	const char * insert;
	switch (hand_of_elbereth[u.uevent.uhand_of_elbereth].title_mod) {
		case CRWN_TTL_LORD:
			insert = flags.female ? "Lady" : "Lord";
			break;
		case CRWN_TTL_KING:
			insert = flags.female ? "Queen" : "King";
			break;
		case CRWN_TTL_PRIE:
			insert = flags.female ? "Priestess" : "Priest";
			break;
		case CRWN_TTL_NAME:
			insert = plname;
			break;			
		default:
			insert = (const char *)0;
			break;
	}
	/* safety-check -- we should only have an insert if there's a place to put it (and vice versa) */
	if (!(strstri(hand_of_elbereth[u.uevent.uhand_of_elbereth].title, "%s")) != !insert)
		impossible("inserting string does not match crowning title");

	if (insert)
		Sprintf(titlebuf, hand_of_elbereth[u.uevent.uhand_of_elbereth].title, insert);
	else
		Strcpy(titlebuf, hand_of_elbereth[u.uevent.uhand_of_elbereth].title);

	return titlebuf;
}

int
gcrownu()
{
	struct obj * obj;
	char buf[BUFSZ];
	const char * ptr;

	/* set crowning event, and grab appropriate entry from array */
	u.uevent.uhand_of_elbereth = choose_crowning();
	const struct crowning * crowndata = &(hand_of_elbereth[u.uevent.uhand_of_elbereth]); 

	/* print the announcement of your crowning */
	/* special case -- not using godvoice */
	if (!crowndata->announcement) {
		switch (u.uevent.uhand_of_elbereth) {
			case 19:
				verbalize("As shadows define the light, so too do webs define the spider.");
				verbalize("You shall be my shepherd, to wrap the world in webs of shadow!");
				break;
			case 44:
				verbalize("Hurrah for our Pirate King!");
				break;
			case 46:
				You("suddenly perceive 15 pairs of star-like eyes, staring at you from within your head.");
				pline("<<We are the Council of Elements>>");
				pline("<<Guardians of the Material world>>");
				pline("<<You who straddle the line between our world and the void beyond,");
				pline("  you shall be our emissary to that which gave rise to us all>>");
				break;
			default:
				impossible("bad unannounced crowning %d", u.uevent.uhand_of_elbereth);
				break;
		}
	}
	else {
		/* announcement that contains the title */
		if (strstri(crowndata->announcement, "%s")) {
			/* get crowning title without any preceding "the" */
			ptr = crowning_title();
			if (!strncmpi(ptr, "the ", 4))
				ptr += 4;
			Sprintf(buf, crowndata->announcement, ptr);
		}
		/* announcement that does not contain the title */
		else {
			Strcpy(buf, crowndata->announcement);
		}
		/* speak it! */
		godvoice(Align2gangr(u.ualign.type), buf);
	}

	/* livelog your crowning */
	/* livelogstr that contains the title */
	if (strstri(crowndata->livelogstr, "%s")) {
		/* get crowning title without any preceding "the" */
		ptr = crowning_title();
		if (!strncmpi(ptr, "the ", 4))
			ptr += 4;
		Sprintf(buf, crowndata->livelogstr, ptr);
	}
	/* livelogstr that does not contain the title */
	else {
		Strcpy(buf, crowndata->livelogstr);
	}
	/* livelog it! */
	livelog_write_string(buf);


	/* start handing out the goodies! */

	/* except for binders, being crowned grants a nice bunch of intrinsics and a ward */	
	if(!Role_if(PM_EXILE)){
		HSee_invisible |= FROMOUTSIDE;
		HFire_resistance |= FROMOUTSIDE;
		HCold_resistance |= FROMOUTSIDE;
		HShock_resistance |= FROMOUTSIDE;
		HSleep_resistance |= FROMOUTSIDE;
		HPoison_resistance |= FROMOUTSIDE;
		u.wardsknown |= WARD_HEPTAGRAM;
	}

	/* special cases: those that don't get artifact gifts at all */
	if (!crowndata->crowninggift) {
		obj = (struct obj *)0;
		switch(u.uevent.uhand_of_elbereth) {
			case 19:
				u.specialSealsKnown |= SEAL_BLACK_WEB;
				break;
			case 46:
				bindspirit(COUNCIL);
				break;
			default:
				impossible("bad giftless crowning %d", u.uevent.uhand_of_elbereth);
				break;
		}
	}
	/* general case: there is an intended crowning gift */
	else {
		boolean already_exists = FALSE;
		int arti = crowndata->crowninggift;	/* make a modifiable local-scope copy */
		
		/* does it already exist? might need to change what artifact we try to give */
		if (art_already_exists(arti)) {
			already_exists = TRUE;
			if (arti == ART_NECRONOMICON) {
				arti = ART_BOOK_OF_INFINITE_SPELLS;
				already_exists = art_already_exists(arti);
			}
		}

		/* if it doesn't already exist, make it */
		if (!already_exists) {
			/* special-case: Excalibur can be made from a wielded long sword */
			if (arti == ART_EXCALIBUR && uwep && uwep->otyp == LONG_SWORD && !uwep->oartifact) {
				if (!Blind) Your("sword shines brightly for a moment.");
				obj = oname(uwep, artiname(arti));
			}
			else {
				obj = oname(mksobj(artilist[arti].otyp, FALSE, FALSE), artiname(arti));
			}

			/* enchant, if allowable */
			if (obj->oclass == WEAPON_CLASS || obj->oclass == ARMOR_CLASS || is_weptool(obj)) {
				switch (arti) {
					case ART_EXCALIBUR:
						obj->spe = max(obj->spe, 1);
						break;
					case ART_POSEIDON_S_TRIDENT:
						obj->spe = 3;
						break;
					case ART_MOONBEAM:
						obj->spe = 6;
						break;
					case ART_VEIL_OF_LATONA:
					case ART_SUNBEAM:
						obj->spe = 7;
						break;
					default:
						obj->spe = 1;
						break;
				}
			}

			/* grant appropriate skill(s) */
			if (obj->oclass == WEAPON_CLASS) {
				expert_weapon_skill(abs(objects[obj->otyp].oc_skill));
			}
			switch (arti)
			{
				case ART_GRANDMASTER_S_ROBE:
				case ART_GREAT_CLAWS_OF_URDLEN:
				case ART_CLAWS_OF_THE_REVENANCER:
					gm_weapon_skill(P_BARE_HANDED_COMBAT);
					break;
				case ART_ROBE_OF_THE_ARCHMAGI:
					expert_weapon_skill(P_ATTACK_SPELL);
					expert_weapon_skill(P_ENCHANTMENT_SPELL);
					expert_weapon_skill(P_ESCAPE_SPELL);
					expert_weapon_skill(P_MATTER_SPELL);
					break;
				case ART_LIECLEAVER:
					expert_weapon_skill(P_SCIMITAR);
					break;
				case ART_GLITTERSTONE:
					expert_weapon_skill(P_ENCHANTMENT_SPELL);
					expert_weapon_skill(P_HEALING_SPELL);
					expert_weapon_skill(P_ESCAPE_SPELL);
					break;
				case ART_STEEL_SCALES_OF_KURTULMAK:
					expert_weapon_skill(P_RIDING);
					break;
				case ART_CROWN_OF_THE_SAINT_KING:
				case ART_HELM_OF_THE_DARK_LORD:
				case ART_CLARENT:
					expert_weapon_skill(P_BEAST_MASTERY);
					break;
				case ART_TECPATL_OF_HUHETOTL:
					expert_weapon_skill(P_CLERIC_SPELL);
					break;
				case ART_EHECAILACOCOZCATL:
					expert_weapon_skill(P_ATTACK_SPELL);
					break;
				case ART_HERMES_S_SANDALS:
					expert_weapon_skill(P_LONG_SWORD);
					break;
				case ART_AEGIS:
					expert_weapon_skill(P_SPEAR);
					break;
			}
			/* other artifact specials */
			switch (arti) {
				case ART_GREAT_CLAWS_OF_URDLEN:
					obj->objsize = MZ_SMALL;	/* why isn't this their default size??? */
					fix_object(obj);
					break;
				case ART_NECRONOMICON:
					obj->ovar1 |= SP_DEATH;
					break;
				case ART_BOOK_OF_INFINITE_SPELLS:
					obj->ovar1 = SPE_FINGER_OF_DEATH;
					break;
				case ART_POSEIDON_S_TRIDENT:
					HSwimming |= FROMOUTSIDE;
					break;
			}

			/* special-case: place the crown on your brow */
			if (arti == ART_CROWN_OF_THE_SAINT_KING || arti == ART_HELM_OF_THE_DARK_LORD) {
				obj = hold_another_object(obj, 
					"A %s appears at your feet", 
					arti == ART_CROWN_OF_THE_SAINT_KING ? "crown" : "helm",
					(const char *)0);
				if(carried(obj)){
					if(uarmh) remove_worn_item(uarmh, TRUE);
					setworn(obj, W_ARMH);
					Helmet_on();
				}
			}
			/* at thy feet */
			else {
				if (obj->quan > 1)
					Sprintf(buf, "Some %s", obj_descname(obj));
				else
					Strcpy(buf, An(obj_descname(obj)));
				if (strstri(buf, " named "))
					*strstri(buf, " named ") = '\0';
				at_your_feet(buf);
				dropy(obj);
			}
			u.ugifts++;
			obj->gifted = Align2gangr(u.ualign.type);
			discover_artifact(arti);
		}
		else if (uwep && uwep->oartifact == arti) {
			obj = uwep;
			/* ha, you already have it! */
			switch(arti)
			{
			case ART_REAVER:
				Your("%s rings with the sound of waves!", xname(uwep));
				uwep->dknown = TRUE;
				break;
			case ART_STORMBRINGER:
				Sprintf(buf, "%s %s", hcolor(NH_BLACK), xname(uwep));
				Your("%s hums ominously!", buf);
				uwep->dknown = TRUE;
				break;
			case ART_VORPAL_BLADE:
				Your("%s goes snicker-snack!", xname(uwep));
				uwep->dknown = TRUE;
				break;
			}
		}
		else {
			/* well, whatever it is you're wielding, it'll get very slightly enchanted, made rustproof, and unrestricted */
			obj = uwep;
		}
	}

	/* enhance weapon regardless of alignment or artifact status */
    if (obj && (obj->oclass == WEAPON_CLASS || obj->oclass == ARMOR_CLASS || is_weptool(obj))) {
		bless(obj);
		obj->oeroded = obj->oeroded2 = 0;
		if(obj->oartifact != ART_PEN_OF_THE_VOID)
			obj->oerodeproof = TRUE;
		obj->bknown = obj->rknown = TRUE;
		if (obj->spe < 1) obj->spe = 1;
		/* acquire at least basic skill in this weapon */
		if (valid_weapon(obj)) {
			unrestrict_weapon_skill(weapon_type(obj));
		}
    }

	/* finally, FOR SOME REASON you get weapon skill for these ones even if you didn't get the weapon */
	switch (u.uevent.uhand_of_elbereth) {
		case 1:	/* Excalibur */
		case 2:	/* Vorpal Blade */
			expert_weapon_skill(P_LONG_SWORD);
			break;
		case 3:	/* Stormbringer */
			expert_weapon_skill(P_BROAD_SWORD);
			break;
		case 44:/* Reaver */
			expert_weapon_skill(P_SCIMITAR);
			break;
	}

	update_inventory();
	return 1;
}